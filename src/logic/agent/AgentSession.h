#ifndef __SL_AGENT_SESSION_H__
#define __SL_AGENT_SESSION_H__
#include "slikernel.h"
#include "slpool.h"

class Agent;
class SessionBase : public sl::api::ITcpSession{
public:
	SessionBase(Agent* pAgent, int8 type) :_agent(pAgent), _type(type), _id(0){}
	virtual ~SessionBase() {}

	virtual int8 getType() const { return _type; }
protected:
	Agent* _agent;
	int8   _type;
	int32  _id;
};

class AgentSession : public SessionBase{
public:
	AgentSession(Agent* pAgent, int8 type): SessionBase(pAgent, type){}
	virtual ~AgentSession(){}

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);
};

class LogicSession : public SessionBase{
public:
	LogicSession(Agent* pAgent, int8 type): SessionBase(pAgent, type){}
	virtual ~LogicSession(){}

	virtual int32 onRecv(sl::api::IKernel* pKernel, const char* pContext, int dwLen);
	virtual void onConnected(sl::api::IKernel* pKernel);
	virtual void onDisconnect(sl::api::IKernel* pKernel);
};

#endif