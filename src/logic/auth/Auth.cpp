#include "Auth.h"
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
bool Auth::initialize(sl::api::IKernel * pKernel){
	_self = this;

	return true;
}

bool Auth::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);

	_harbor->addNodeListener(this);

	RGS_NODE_HANDLER(_harbor, ChatProtocolID::CHANNEL_MSG_SYNC_TOKEN, Auth::onChannelSyncToken);
	RGS_NODE_HANDLER(_harbor, ChatProtocolID::CHATGATE_MSG_VERIFY_TOKEN, Auth::onGateVerifyToken);

	return true;
}
bool Auth::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Auth::onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){
	if (nodeType == NodeType::CHATBALANCER){
		IArgs<2, 256> args;
		args << pKernel->getInternetIp();
		args << sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("logic_port"));
		args.fix();
		_harbor->send(NodeType::CHATBALANCER, 1, NodeProtocol::CHANNEL_MSG_CHANNEL_REGISTER, args.out());
	}
}

void Auth::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
}

void Auth::onChannelSyncToken(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	SLASSERT(nodeType == NodeType::CHATCHANNEL, "wtf");
	const void* context = args.getContext();
	const int32 size = args.getSize();
	if (size != sizeof(ChatPublic::ticket)){
		SLASSERT(false, "wtf");
		return;
	}

	const ChatPublic::ticket* pTicket = (const ChatPublic::ticket*)context;
	TOKEN_MAP::iterator itor = _tokenMap.find(ChatPublic::TicketID(pTicket->appID, pTicket->ocID));
	if (itor == _tokenMap.end()){
		_tokenMap.insert(make_pair(ChatPublic::TicketID(pTicket->appID, pTicket->ocID), pTicket->token));
	}
	else{
		itor->second = pTicket->token;
	}

	IArgs<3, 256> inArgs;
	inArgs << pTicket->appID << pTicket->ocID << (int32)0;
	inArgs.fix();
	_harbor->send(nodeType, nodeId, ChatProtocolID::AUTH_MSG_SYNC_TOKEN_RES, inArgs.out());

	IArgs<2, 128> inArgs1;
	inArgs1 << pTicket->appID << pTicket->ocID;
	inArgs1.fix();
	_harbor->broadcast(NodeType::CHATGATE, ChatProtocolID::AUTH_MSG_SYNC_TOKEN, inArgs1.out());
}

void Auth::onGateVerifyToken(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args){
	SLASSERT(nodeType == NodeType::CHATGATE, "wtf");
	const void* context = args.getContext();
	const int32 size = args.getSize();
	if (size != sizeof(ChatPublic::ticket)){
		SLASSERT(false, "wtf");
		return;
	}

	const ChatPublic::ticket* pTicket = (const ChatPublic::ticket*)context;
	TOKEN_MAP::iterator itor = _tokenMap.find(ChatPublic::TicketID(pTicket->appID, pTicket->ocID));
	
	int32 errCode = 0;
	if (itor == _tokenMap.end() || itor->second != pTicket->token){
		errCode = -1;
	}

	IArgs<3, 256> inArgs;
	inArgs << pTicket->appID << pTicket->ocID << errCode;
	inArgs.fix();
	_harbor->send(nodeType, nodeId, ChatProtocolID::AUTH_MSG_VERIFY_TOKEN_RES, inArgs.out());
}
