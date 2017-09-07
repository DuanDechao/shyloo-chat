#include "Channel.h"
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
#include "ChatProtocol.h"

#define REPORT_LOAD_INTERVAL 1000
#define TICKET_EXPIRE_TIME 10000

class ChannelCBMessageHandler : public IChannelMessageHandler{
public:
	ChannelCBMessageHandler(const CHANNEL_CB cb) : _cb(cb){}
	virtual ~ChannelCBMessageHandler() {}

	virtual void DealNodeMessage(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size){
		_cb(pKernel, id, pContext, size);
	}
private:
	CHANNEL_CB		_cb;
};


class ChannelArgsCBMessageHandler : public IChannelMessageHandler{
public:
	ChannelArgsCBMessageHandler(const CHANNEL_ARGS_CB cb) : _cb(cb){}
	virtual ~ChannelArgsCBMessageHandler() {}

	virtual void DealNodeMessage(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size){
		sl::OBStream args(pContext, size);
		_cb(pKernel, id, args);
	}
private:
	CHANNEL_ARGS_CB _cb;
};

bool Channel::initialize(sl::api::IKernel * pKernel){
	_self = this;

	return true;
}

bool Channel::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_IdMgr, IdMgr);
	FIND_MODULE(_agent, Agent);

	_agent->setListener(this);
	_harbor->addNodeListener(this);

	START_TIMER(_self, 20000, TIMER_BEAT_FOREVER, REPORT_LOAD_INTERVAL);

	RGS_NODE_ARGS_HANDLER(_harbor, ChatProtocolID::CHATGATE_MSG_SYNC_TICKET, Channel::onGateSyncTicket);
	
	return true;
}
bool Channel::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Channel::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if (nodeType == NodeType::CHATBALANCER){
		IArgs<2, 256> args;
		args << pKernel->getInternetIp();
		args << sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("logic_port"));
		args.fix();
		_harbor->send(NodeType::CHATBALANCER, 1, NodeProtocol::CHANNEL_MSG_CHANNEL_REGISTER, args.out());
	}
}

void Channel::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
}

void Channel::onTime(sl::api::IKernel* pKernel, int64 timetick){
	IArgs<1, 32> args;
	args << (int32)_players.size();
	args.fix();
	_harbor->send(NodeType::CHATBALANCER, 1, NodeProtocol::GATE_MSG_LOAD_REPORT, args.out());
}

void Channel::onAgentOpen(sl::api::IKernel* pKernel, const int8 type, const int64 id){
	SLASSERT(_players.find(id) == _players.end(), "duplicate agent id£º%d", id);
	_players[id] = { id, 0, 0, 0, GATE_STATE_NONE };
}

void Channel::onAgentClose(sl::api::IKernel* pKernel, const int8 type, const int64 id){
	SLASSERT(_players.find(id) != _players.end(), "where is agent %d", id);

	if (_players[id].state != GATE_STATE_NONE){
		reset(pKernel, id, GATE_STATE_NONE);
	}

	_players.erase(id);
}

int32 Channel::onAgentRecv(sl::api::IKernel* pKernel, const int8 type, const int64 id, const void* context, const int32 size){
	if (type != AgentType::ATYPE_LOGIC){
		SLASSERT(false, "wtf");
		return 0;
	}

	if (size < 8)
		return 0;

	int32 msgId = *(int32*)context;
	int32 msgSize = *(int32*)((char*)context + sizeof(int32));
	if (msgSize >= size){
		switch (msgId){
		case ChatProtocolID::CHAT_API_RGS_LISTENER:{
			OnMsgRgsListener(pKernel, id, (char*)context + 2 * sizeof(int32), size - 2 * sizeof(int32));
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

void Channel::rgsChannelMessageHandler(int32 messageId, const CHANNEL_CB& handler, const char* debug){
	SLASSERT(_gateProtos.find(messageId) == _gateProtos.end(), "duplicate agent msg %d", messageId);
	_gateProtos[messageId] = NEW ChannelCBMessageHandler(handler);
}

void Channel::rgsChannelArgsMessageHandler(int32 messageId, const CHANNEL_ARGS_CB& handler, const char* debug){
	SLASSERT(_gateProtos.find(messageId) == _gateProtos.end(), "duplicate agent msg %d", messageId);
	_gateProtos[messageId] = NEW ChannelArgsCBMessageHandler(handler);
}


void Channel::OnMsgRgsListener(sl::api::IKernel* pKernel, int64 agentId, const void* context, const int32 size){
	if (size != sizeof(ChatPublic::TicketID)){
		SLASSERT(false, "wtf");
		return;
	}

	const ChatPublic::TicketID * pTicketID = (const ChatPublic::TicketID*)context;

	ChatPublic::ticket ticket;
	ticket.appID = pTicketID->appid;
	ticket.ocID = pTicketID->ocid;
	SafeSprintf(ticket.token, sizeof(ticket.token), "%lld", (const int64)pTicketID + pTicketID->appid + pTicketID->ocid);

	syncTokenToChatAuth(ticket);

	int32 msgID = ChatProtocolID::CHAT_API_RGS_LISTENER_RES;
	int32 len = sizeof(int32)* 2 + sizeof(ticket);
	_agent->send(agentId, &msgID, sizeof(int32));
	_agent->send(agentId, &len, sizeof(int32));
	_agent->send(agentId, &ticket, sizeof(ticket));
}

void Channel::syncTokenToChatAuth(const ChatPublic::ticket& ticket){
	_harbor->prepareSend(NodeType::CHATAUTH, 1, ChatProtocolID::CHANNEL_MSG_SYNC_TOKEN, sizeof(ticket));
	_harbor->send(NodeType::CHATAUTH, 1, &ticket, sizeof(ticket));
}

void Channel::onGateSyncTicket(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	SLASSERT(nodeType == NodeType::CHATGATE, "wtf");
	int32 appID = args.getInt32(0);
	int64 ocID = args.getInt64(1);

	CLIENT_NODE_MAP::iterator itor = _clientNodeMap.find(ChatPublic::TicketID(appID, ocID));
	if (itor != _clientNodeMap.end()){
		itor->second = nodeId;
	}
	else{
		_clientNodeMap.insert(make_pair(ChatPublic::TicketID(appID, ocID), nodeId));
	}
}
