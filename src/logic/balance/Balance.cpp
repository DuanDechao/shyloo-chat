#include "Balance.h"
#include "IHarbor.h"
#include "NodeDefine.h"
#include "NodeProtocol.h"
#include "IAgent.h"
#include "slbinary_stream.h"
#include "ProtocolID.pb.h"
#include "slxml_reader.h"
#include "GameDefine.h"
#include "ChatProtocol.h"

bool Balance::initialize(sl::api::IKernel * pKernel){
	_kernel = pKernel;
	_self = this;
	return true;
}

bool Balance::launched(sl::api::IKernel * pKernel){
	FIND_MODULE(_harbor, Harbor);
	FIND_MODULE(_agent, Agent);

	_harbor->addNodeListener(this);
	_agent->setListener(this);

	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GATE_MSG_GATE_REGISTER, Balance::onGateRegister);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::GATE_MSG_LOAD_REPORT, Balance::onGateLoadReport);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::CHANNEL_MSG_CHANNEL_REGISTER, Balance::onChannelRegister);
	RGS_NODE_ARGS_HANDLER(_harbor, NodeProtocol::CHANNEL_MSG_LOAD_REPORT, Balance::onChannelLoadReport);

	return true;
}

bool Balance::destory(sl::api::IKernel * pKernel){
	DEL this;
	return true;
}

void Balance::onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId){
	if (nodeType == NodeType::CHATGATE){
		_gates.erase(nodeId);
	}

	if (nodeType == NodeType::CHATCHANNEL){
		_channels.erase(nodeId);
	}
}

void Balance::onAgentOpen(sl::api::IKernel* pKernel, const int8 type, const int64 id){
	if (type == AgentType::ATYPE_CLIENT)
		sendGate(pKernel, id);
	if (type == AgentType::ATYPE_LOGIC)
		sendChannel(pKernel, id);
}

void Balance::onAgentClose(sl::api::IKernel* pKernel, const int8 type, const int64 id){
}

void Balance::onGateRegister(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	SLASSERT(_gates.find(nodeId) == _gates.end(), "wtf");
	const char* ip = args.getString(0);
	const int32 port = args.getInt32(1);
	TRACE_LOG("new gate node[%d] register", nodeId);

	NodeInfo& gate = _gates[nodeId];
	gate.nodeId = nodeId;
	gate.ip = ip;
	gate.port = port;
	gate.load = 0;
}

void Balance::onGateLoadReport(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	auto itor = _gates.find(nodeId);
	if (itor == _gates.end()){
		SLASSERT(false, "wtf");
		return;
	}

	const int32 load = args.getInt32(0);
	NodeInfo& info = _gates[nodeId];
	info.load = load;
}

void Balance::onChannelRegister(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	SLASSERT(_channels.find(nodeId) == _channels.end(), "wtf");
	const char* ip = args.getString(0);
	const int32 port = args.getInt32(1);
	TRACE_LOG("new gate node[%d] register", nodeId);

	NodeInfo& channel = _channels[nodeId];
	channel.nodeId = nodeId;
	channel.ip = ip;
	channel.port = port;
	channel.load = 0;
}

void Balance::onChannelLoadReport(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const OArgs& args){
	auto itor = _channels.find(nodeId);
	if (itor == _channels.end()){
		SLASSERT(false, "wtf");
		return;
	}

	const int32 load = args.getInt32(0);
	NodeInfo& info = _channels[nodeId];
	info.load = load;
}


void Balance::sendGate(sl::api::IKernel* pKernel, int64 agentId){
	SLASSERT(!_gates.empty(), "wtf");
	NodeInfo* gate = nullptr;
	for (auto itor = _gates.begin(); itor != _gates.end(); ++itor){
		if (gate == nullptr || gate->load > itor->second.load)
			gate = &(itor->second);
	}

	if (gate == nullptr){
		_agent->kick(agentId);
		return;
	}

	sl::IBStream<128> ack;
	ack << gate->ip.c_str() << gate->port;
	sl::OBStream out = ack.out();

	client::Header header;
	header.messageId = ServerMsgID::SERVER_MSG_CHAT_AGENT_INFO_SYNC;
	header.size = sizeof(client::Header) + out.getSize();
	_agent->send(agentId, &header, sizeof(header));
	_agent->send(agentId, out.getContext(), out.getSize());
}

void Balance::sendChannel(sl::api::IKernel* pKernel, int64 agentId){
	SLASSERT(!_channels.empty(), "wtf");
	NodeInfo* channel = nullptr;
	for (auto itor = _channels.begin(); itor != _channels.end(); ++itor){
		if (channel == nullptr || channel->load > itor->second.load)
			channel = &(itor->second);
	}

	if (channel == nullptr){
		_agent->kick(agentId);
		return;
	}

	ChatBalancer::addrinfo info;
	info.errCode = -1;
	if (channel){
		info.errCode = 0;
		info.ip = channel->ip.c_str();
		info.port = channel->port;
	}

	client::Header header;
	header.messageId = ChatProtocolID::BALANCE_MSG_CHANNEL_ADDRESS_INFO_SYNC;
	header.size = sizeof(client::Header) + sizeof(info);
	_agent->send(agentId, &header, sizeof(header));
	_agent->send(agentId, &info, sizeof(info));
}





