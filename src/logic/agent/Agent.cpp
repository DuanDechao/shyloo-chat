#include "Agent.h"
#include "AgentSession.h"
#include "slxml_reader.h"
#include "slstring_utils.h"
#include "GameDefine.h"
sl::SLPool<AgentSession> AgentSessionServer::s_pool;
sl::api::ITcpSession* AgentSessionServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return CREATE_FROM_POOL(s_pool, _agent, AgentType::ATYPE_CLIENT);
}

sl::SLPool<LogicSession> LogicSessionServer::s_pool;
sl::api::ITcpSession* LogicSessionServer::mallocTcpSession(sl::api::IKernel* pKernel){
	return CREATE_FROM_POOL(s_pool, _agent, AgentType::ATYPE_LOGIC);
}

bool Agent::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	return true;
}

bool Agent::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);

	sl::XmlReader conf;
	if (!conf.loadXml(pKernel->getConfigFile())){
		SLASSERT(false, "can not load core file %s", pKernel->getCoreFile());
		return false;
	}
	const sl::ISLXmlNode& agentConf = conf.root()["agent"][0];
	int32 agentRecvSize = agentConf.getAttributeInt32("recv");
	int32 agentSendSize = agentConf.getAttributeInt32("send");

	int32 clientPort = 0;
	int32 logicPort = 0;

	if (pKernel->getCmdArg("client_port"))
		clientPort = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("client_port"));
	if (pKernel->getCmdArg("logic_port"))
		logicPort = sl::CStringUtils::StringAsInt32(pKernel->getCmdArg("logic_port"));

	if (clientPort > 0){
		_agentServer = NEW AgentSessionServer(this);
		if (!_agentServer){
			SLASSERT(false, "wtf");
			return false;
		}

		if (!pKernel->startTcpServer(_agentServer, "0.0.0.0", clientPort, agentSendSize, agentRecvSize)){
			SLASSERT(false, "wtf");
			TRACE_LOG("start agent server[%s:%d] success", "0.0.0.0", clientPort);
			return false;
		}
		else{
			TRACE_LOG("start agent server[%s:%d] success", "0.0.0.0", clientPort);
		}
	}
	
	if (logicPort > 0){
		_logicServer = NEW LogicSessionServer(this);
		if (!_logicServer){
			SLASSERT(false, "wtf");
			return false;
		}

		if (!pKernel->startTcpServer(_logicServer, "0.0.0.0", logicPort, agentSendSize, agentRecvSize)){
			SLASSERT(false, "wtf");
			TRACE_LOG("start agent server[%s:%d] success", "0.0.0.0", logicPort);
			return false;
		}
		else{
			TRACE_LOG("start agent server[%s:%d] success", "0.0.0.0", logicPort);
		}
	}

	return true;
}

bool Agent::destory(sl::api::IKernel * pKernel){
	if (_agentServer)
		DEL _agentServer;
	_agentServer = nullptr;

	if (_logicServer)
		DEL _logicServer;
	_logicServer = nullptr;

	_listener = nullptr;
	_agentNextId = 0;
	_harbor = nullptr;

	DEL this;
	return true;
}

int64 Agent::onOpen(SessionBase* pSession){
	if (_agentNextId <= 0)
		_agentNextId = 1;

	int64 ret = _agentNextId++;
	_sessions[ret] = pSession;

	if (_listener)
		_listener->onAgentOpen(_kernel, pSession->getType(), ret);

	return ret;
}

int32 Agent::onRecv(int64 id, const char* pContext, const int32 size){
	SLASSERT(_sessions.find(id) != _sessions.end(), "where is agent %lld", id);
	if (_listener == nullptr)
		return 0;

	return _listener->onAgentRecv(_kernel, _sessions[id]->getType(), id, pContext, size);
}

void Agent::onClose(int64 id){
	SLASSERT(_sessions.find(id) != _sessions.end(), "where is agent %lld", id);
	_sessions.erase(id);

	if (_listener)
		_listener->onAgentClose(_kernel, _sessions[id]->getType(), id);
}

void Agent::setListener(IAgentListener* pListener){
	_listener = pListener;
}

void Agent::send(const int64 id, const void* pBuf, const int32 size){
	auto itor = _sessions.find(id);
	if (itor == _sessions.end()){
		SLASSERT(false, "where is agent %lld", id);
		return;
	}

	itor->second->send(pBuf, size);
}

void Agent::kick(const int64 id){
	auto itor = _sessions.find(id);
	if (itor == _sessions.end()){
		SLASSERT(false, "where is agent %lld", id);
		return;
	}

	itor->second->close();
}



