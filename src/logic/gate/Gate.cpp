#include "Gate.h"
#include "IAgent.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "ICacheDB.h"
#include "IIdMgr.h"
#include "DBDef.h"
#include "IRoleMgr.h"
#include "ProtocolID.pb.h"
#include "Protocol.pb.h"
#include <unordered_set>
#include "sltime.h"
#include "slxml_reader.h"
#include "slstring_utils.h"
#include <algorithm>
#include "GateTimer.h"
#include "slbinary_stream.h"

#define REPORT_LOAD_INTERVAL 1000
#define KICK_TIME_OUT (360 * 1000)
class GateCBMessageHandler : public IGateMessageHandler{
public:
	GateCBMessageHandler(const GATE_CB cb) : _cb(cb){}
	virtual ~GateCBMessageHandler() {}

	virtual void DealNodeMessage(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size){
		_cb(pKernel, id, pContext, size);
	}
private:
	GATE_CB		_cb;
};


class GateArgsCBMessageHandler : public IGateMessageHandler{
public:
	GateArgsCBMessageHandler(const GATE_ARGS_CB cb) : _cb(cb){}
	virtual ~GateArgsCBMessageHandler() {}

	virtual void DealNodeMessage(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size){
		sl::OBStream args(pContext, size);
		_cb(pKernel, id, args);
	}
private:
	GATE_ARGS_CB _cb;
};

bool Gate::initialize(sl::api::IKernel * pKernel){
	_self = this;

	sl::XmlReader svrConf;
	if (!svrConf.loadXml(pKernel->getCoreFile())){
		SLASSERT(false, "cant load core file");
		return false;
	}
	_isQueneOn = svrConf.root()["balance"][0].getAttributeInt32("max_tickets") > 0;

	return true;
}

bool Gate::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_IdMgr, IdMgr);
	FIND_MODULE(_agent, Agent);

	_agent->setListener(this);
	_harbor->addNodeListener(this);

	RGS_NODE_ARGS_HANDLER(_harbor, ChatProtocolID::AUTH_MSG_SYNC_TOKEN, Gate::onAuthSyncToken);
	RGS_NODE_ARGS_HANDLER(_harbor, ChatProtocolID::AUTH_MSG_VERIFY_TOKEN_RES, Gate::onAuthVerifyToken);


	if (_isQueneOn)
		START_TIMER(_self, 20000, TIMER_BEAT_FOREVER, REPORT_LOAD_INTERVAL);
	
	return true;
}
bool Gate::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Gate::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if (nodeType == NodeType::CHATBALANCER){
		IArgs<2, 256> args;
		args << pKernel->getInternetIp();
		args << sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("client_port"));
		args.fix();
		_harbor->send(NodeType::CHATBALANCER, 1, NodeProtocol::GATE_MSG_GATE_REGISTER, args.out());
	}
}

void Gate::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
}

void Gate::onTime(sl::api::IKernel* pKernel, int64 timetick){
	IArgs<1, 32> args;
	args << (int32)_players.size();
	args.fix();
	_harbor->send(NodeType::CHATBALANCER, 1, NodeProtocol::GATE_MSG_LOAD_REPORT, args.out());
}

void Gate::onAgentOpen(sl::api::IKernel* pKernel, const int8 type, const int64 id){
	if (type != AgentType::ATYPE_CLIENT){
		SLASSERT(false, "wtf");
		return;
	}

	SLASSERT(_players.find(id) == _players.end(), "duplicate agent id£º%d", id);
	_players[id] = { id, sl::getTimeMilliSecond(), 0, 0, "", AGENT_STATUS_DISABLE, GateTimer::create(id) };
	START_TIMER(_players[id].timer, 0, TIMER_BEAT_FOREVER, 1000);
}

void Gate::onAgentClose(sl::api::IKernel* pKernel, const int8 type, const int64 id){
	if (type != AgentType::ATYPE_CLIENT){
		SLASSERT(false, "wtf");
		return;
	}

	SLASSERT(_players.find(id) != _players.end(), "where is agent %d", id);
	if (_players[id].timer)
		pKernel->killTimer(_players[id].timer);

	_players.erase(id);
}

int32 Gate::onAgentRecv(sl::api::IKernel* pKernel, const int8 type, const int64 id, const void* context, const int32 size){
	if (type != AgentType::ATYPE_CLIENT){
		SLASSERT(false, "wtf");
		return 0;
	}

	if (size < 8)
		return 0;

	int32 msgId = *(int32*)context;
	int32 msgSize = *(int32*)((char*)context + sizeof(int32));
	if (msgSize >= size){
		switch (msgId){
			case ClientMsgID::CLIENT_MSG_LOGIN_CHAT_SERVER:{
				sl::OBStream stream = sl::OBStream((const char*)context + 2 * sizeof(int32), size - 2 * sizeof(int32));
				int32 appid = 0;
				int64 ocid = 0;
				const char* token = nullptr;
				if (!stream.readInt32(appid) || !stream.readInt64(ocid) || !stream.readString(token)){
					SLASSERT(false, "wtf");
					_agent->kick(id);
					return size;
				}

				ECHO_TRACE("player login char server, token=%s", token);
				clientVerifyToken(pKernel, id, appid, ocid, token);
				break;
			}
			
			case ClientMsgID::CLIENT_MSG_HEARTBEAT:{
				client::Header header;
				header.messageId = ServerMsgID::SERVER_MSG_CHAT_HEARTBEAT_ACK;
				header.size = sizeof(int32)* 2;
				_agent->send(id, &header, sizeof(header));
				_players[id].kickTick = sl::getTimeMilliSecond();
				break;
			}
			default:
				break;
		}
		return size;
	}
	
	return 0;
}

void Gate::onGateTimerStart(sl::api::IKernel * pKernel, int64 agentId, int64 tick){
}

void Gate::onGateTimer(sl::api::IKernel * pKernel, int64 agentId, int64 tick){
	if (_players.find(agentId) == _players.end()){
		SLASSERT(false, "wtf");
		return;
	}

	if (tick - _players[agentId].kickTick > KICK_TIME_OUT){
		_agent->kick(agentId);
		pKernel->killTimer(_players[agentId].timer);
		_players.erase(agentId);
	}
}

void Gate::onGateTimerEnd(sl::api::IKernel * pKernel, int64 agentId, int64 tick){
}

void Gate::rgsGateMessageHandler(int32 messageId, const GATE_CB& handler, const char* debug){
	SLASSERT(_gateProtos.find(messageId) == _gateProtos.end(), "duplicate agent msg %d", messageId);
	_gateProtos[messageId] = NEW GateCBMessageHandler(handler);
}

void Gate::rgsGateArgsMessageHandler(int32 messageId, const GATE_ARGS_CB& handler, const char* debug){
	SLASSERT(_gateProtos.find(messageId) == _gateProtos.end(), "duplicate agent msg %d", messageId);
	_gateProtos[messageId] = NEW GateArgsCBMessageHandler(handler);
}


void Gate::clientVerifyToken(sl::api::IKernel* pKernel, int64 agentId, int32 appid, int64 ocid, const char* token){
	TICKETID_MAP::iterator itor = _ticketIDMap.find(ChatPublic::TicketID(appid, ocid));
	if (itor != _ticketIDMap.end()){
		int64 oldAgentId = itor->second;
		if (oldAgentId == agentId)
			return;

		if (_players.find(oldAgentId) == _players.end()){
			SLASSERT(false, "wtf");
			return;
		}

		switch (_players[oldAgentId].state)
		{
		case AGENT_STATUS_TICKET_VERIFYING:{
			sendVerifyRes(agentId, protocol::ErrorCode::ERROR_CHAT_ANOTHER_CLIENT_VERIFYING);
			_agent->kick(agentId);
			return;
		}
		case AGENT_STATUS_TICKET_VERIFED:{
			if (strcmp(token, _players[oldAgentId].token.c_str())){
				sendVerifyRes(agentId, protocol::ErrorCode::ERROR_CHAT_VERIFY_TOKEN_ERROR);
				_agent->kick(agentId);
				return;
			}
			else{
				{
					sendVerifyRes(oldAgentId, protocol::ErrorCode::ERROR_CHAT_ANOTHER_CLIENT_LOGIN);
					_agent->kick(oldAgentId);
				}

				{
					_players[agentId].appid = appid;
					_players[agentId].ocid = ocid;
					_players[agentId].token = token;
					_players[agentId].state = AGENT_STATUS_TICKET_VERIFED;
					sendVerifyRes(agentId, protocol::ErrorCode::ERROR_NO_ERROR);
					itor->second = agentId;
				}
				return;
			}
		}

		default:
			SLASSERT(false, "error agent status");
			break;
		}
	}

	_players[agentId].appid = appid;
	_players[agentId].ocid = ocid;
	_players[agentId].token = token;
	_players[agentId].state = AGENT_STATUS_TICKET_VERIFYING;
	_ticketIDMap.insert(std::make_pair(ChatPublic::TicketID(appid, ocid), agentId));

	ChatPublic::ticket ticket;
	ticket.appID = appid;
	ticket.ocID = ocid;
	SafeSprintf(ticket.token, sizeof(ticket.token), "%s", token);

	_harbor->prepareSend(NodeType::CHATAUTH, 1, ChatProtocolID::CHATGATE_MSG_VERIFY_TOKEN, sizeof(ticket));
	_harbor->send(NodeType::CHATAUTH, 1, &ticket, sizeof(ticket));
	ECHO_TRACE("client login chat, token=%s", token);
}

void Gate::sendVerifyRes(const int64 agentid, const int32 error){
	char context[sizeof(int32)* 3];
	*(int32*)context = ServerMsgID::SERVER_MSG_CHAT_SERVER_LOGIN_RSP;
	*(int32*)((char*)context + sizeof(int32)) = 2 * sizeof(int32)+sizeof(int32);
	*(int32*)((char*)context + 2 * sizeof(int32)) = error;
	_agent->send(agentid, context, sizeof(int32)* 3);
}

void Gate::onAuthSyncToken(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	int32 appID = args.getInt32(0);
	int64 ocID = args.getInt64(1);
	
	ChatPublic::TicketID ticket(appID, ocID);
	TICKETID_MAP::iterator itor = _ticketIDMap.find(ticket);
	if (itor != _ticketIDMap.end()){
		sendVerifyRes(itor->second, protocol::ErrorCode::ERROR_CHAT_ANOTHER_CLIENT_LOGIN);
		_agent->kick(itor->second);
		_ticketIDMap.erase(itor);
	}
}

void Gate::onAuthVerifyToken(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	SLASSERT(nodeType == NodeType::CHATAUTH, "wtf");
	int32 appID = args.getInt32(0);
	int64 ocID = args.getInt64(1);
	int32 errCode = args.getInt32(2);

	TICKETID_MAP::iterator itor = _ticketIDMap.find(ChatPublic::TicketID(appID, ocID));
	if (itor == _ticketIDMap.end()){
		return;
	}

	if (errCode != 0){
		sendVerifyRes(itor->second, protocol::ErrorCode::ERROR_CHAT_VERIFY_TOKEN_ERROR);
		_agent->kick(itor->second);
		_ticketIDMap.erase(itor);
	}
	else{
		sendVerifyRes(itor->second, protocol::ErrorCode::ERROR_NO_ERROR);
		_players[itor->second].state = AGENT_STATUS_TICKET_VERIFED;

		IArgs<2, 128> inArgs;
		inArgs << appID << ocID;
		inArgs.fix();
		_harbor->broadcast(NodeType::CHATCHANNEL, ChatProtocolID::CHATGATE_MSG_SYNC_TICKET, inArgs.out());
	}
}