#ifndef __SL_FRAMEWORK_CHANNEL_H__
#define __SL_FRAMEWORK_CHANNEL_H__
#include "slikernel.h"
#include "slimodule.h"
#include "IChannel.h"
#include "IAgent.h"
#include "IHarbor.h"
#include "slbinary_stream.h"
#include <unordered_map>
#include <unordered_set>
#include <list>
#include "slsingleton.h"
using namespace sl;

class IIdMgr;
class IRoleMgr;
class IRole;
class ICacheDB;

class IChannelMessageHandler{
public:
	virtual ~IChannelMessageHandler(){}
	virtual void DealNodeMessage(sl::api::IKernel*, const int64, const char* pContext, const int32 size) = 0;
};

class Channel :public IChannel, public IAgentListener, public INodeListener, public sl::api::ITimer, public SLHolder<Channel>{
public:
	typedef std::unordered_map<ChatPublic::TicketID, int32> CLIENT_NODE_MAP;

	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onAgentOpen(sl::api::IKernel* pKernel, const int8 type, const int64 id);
	virtual int32 onAgentRecv(sl::api::IKernel* pKernel, const int8 type, const int64 id, const void* context, const int32 size);
	virtual void onAgentClose(sl::api::IKernel* pKernel, const int8 type, const int64 id);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	virtual void rgsChannelMessageHandler(int32 messageId, const CHANNEL_CB& handler, const char* debug);
	virtual void rgsChannelArgsMessageHandler(int32 messageId, const CHANNEL_ARGS_CB& handler, const char* debug);


	void broadcast(const void* context, const int32 size);
	void send(int64 actorId, const void* context, const int32 size);

	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

	void onGateSyncTicket(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:
	void OnMsgRgsListener(sl::api::IKernel* pKernel, int64 agentId, const void* context, const int32 size);
	void syncTokenToChatAuth(const ChatPublic::ticket& ticket);

private:
	sl::api::IKernel* _kernel;
	Channel*		_self;
	IHarbor*	_harbor;
	IAgent*		_agent;
	ICacheDB*	_cacheDB;
	IIdMgr*		_IdMgr;
	
	std::unordered_map<int32, IChannelMessageHandler*> _gateProtos;
	CLIENT_NODE_MAP _clientNodeMap;
};

#endif