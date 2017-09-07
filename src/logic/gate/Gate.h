#ifndef __SL_FRAMEWORK_GATE_H__
#define __SL_FRAMEWORK_GATE_H__
#include "slikernel.h"
#include "slimodule.h"
#include "IGate.h"
#include "IAgent.h"
#include "IHarbor.h"
#include "slbinary_stream.h"
#include <unordered_map>
#include <unordered_set>
#include <list>
#include "slsingleton.h"
#include "ChatProtocol.h"
#include "GameDefine.h"
#include "slstring.h"
using namespace sl;

class IIdMgr;
class GateTimer;
class IGateMessageHandler{
public:
	virtual ~IGateMessageHandler(){}
	virtual void DealNodeMessage(sl::api::IKernel*, const int64, const char* pContext, const int32 size) = 0;
};

class Gate :public IGate, public IAgentListener, public INodeListener, public sl::api::ITimer, public SLHolder<Gate>{
	enum {
		AGENT_STATUS_DISABLE = 0,
		AGENT_STATUS_TICKET_VERIFYING,
		AGENT_STATUS_TICKET_VERIFED,
	};

	struct Player{
		int64 agentId;
		int64 kickTick;
		int32 appid;
		int64 ocid;
		sl::SLString<game::MAX_TOKEN_LEN> token;
		int8 state;
		GateTimer* timer;
	};

	typedef std::unordered_map<ChatPublic::TicketID, int64> TICKETID_MAP;

public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onAgentOpen(sl::api::IKernel* pKernel, const int8 type, const int64 id);
	virtual int32 onAgentRecv(sl::api::IKernel* pKernel, const int8 type, const int64 id, const void* context, const int32 size);
	virtual void onAgentClose(sl::api::IKernel* pKernel, const int8 type, const int64 id);

	void onGateTimerStart(sl::api::IKernel * pKernel, int64 agentId, int64 tick);
	void onGateTimer(sl::api::IKernel * pKernel, int64 agentId, int64 tick);
	void onGateTimerEnd(sl::api::IKernel * pKernel, int64 agentId, int64 tick);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	virtual void rgsGateMessageHandler(int32 messageId, const GATE_CB& handler, const char* debug);
	virtual void rgsGateArgsMessageHandler(int32 messageId, const GATE_ARGS_CB& handler, const char* debug);


	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){}
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick){}

	void onAuthSyncToken(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onAuthVerifyToken(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:
	void clientVerifyToken(sl::api::IKernel* pKernel, int64 agentId, int32 appid, int64 ocid, const char* token);
	void sendVerifyRes(const int64 agentid, const int32 error);

private:
	sl::api::IKernel* _kernel;
	Gate*		_self;
	IHarbor*	_harbor;
	IAgent*		_agent;
	IIdMgr*		_IdMgr;
	
	TICKETID_MAP _ticketIDMap;
	std::unordered_map<int64, Player> _players;
	std::unordered_map<int32, IGateMessageHandler*> _gateProtos;
	bool _isQueneOn;
};

#endif