#ifndef __SL_FRAMEWORK_BALANCE_H__
#define __SL_FRAMEWORK_BALANCE_H__
#include "slimodule.h"
#include "slstring.h"
#include "GameDefine.h"
#include <unordered_map>
#include "IAgent.h"
#include "IHarbor.h"
#include <map>

class IHarbor;
class IAgent;
class Balance : public sl::api::IModule, public IAgentListener, public INodeListener{
	struct NodeInfo{
		int32 nodeId;
		sl::SLString<game::MAX_IP_LEN> ip;
		int32 port;
		int32 load;
	};

public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onAgentOpen(sl::api::IKernel* pKernel, const int8 type, const int64 id);
	virtual int32 onAgentRecv(sl::api::IKernel* pKernel, const int8 type, const int64 id, const void* context, const int32 size){ return 0; }
	virtual void onAgentClose(sl::api::IKernel* pKernel, const int8 type, const int64 id);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port){}
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	void onGateRegister(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onGateLoadReport(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

	void onChannelRegister(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);
	void onChannelLoadReport(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args);

private:
	void sendGate(sl::api::IKernel* pKernel, int64 agentId);
	void sendChannel(sl::api::IKernel* pKernel, int64 agentId);

private:
	Balance*			_self;
	sl::api::IKernel*	_kernel;
	IHarbor*			_harbor;
	IAgent*				_agent;
	
	std::unordered_map<int32, NodeInfo> _gates;
	std::unordered_map<int32, NodeInfo> _channels;
};
#endif