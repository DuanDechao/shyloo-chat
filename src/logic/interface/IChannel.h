#ifndef _SL_ICHANNEL_H__
#define _SL_ICHANNEL_H__
#include "slikernel.h"
#include "slimodule.h"
#include <functional>
#include "slbinary_stream.h"

typedef std::function<void(sl::api::IKernel* pKernel, const int64 id, const char* pContext, const int32 size)> CHANNEL_CB;
typedef std::function<void(sl::api::IKernel* pKernel, const int64 id, const sl::OBStream& args)> CHANNEL_ARGS_CB;

class IChannel : public sl::api::IModule{
public:
	virtual ~IChannel() {}

	virtual void rgsChannelMessageHandler(int32 messageId, const CHANNEL_CB& handler, const char* debug) = 0;
	virtual void rgsChannelArgsMessageHandler(int32 messageId, const CHANNEL_ARGS_CB& handler, const char* debug) = 0;
};

#define RGS_CHANNEL_HANDLER(gate, messageId, handler) gate->rgsChannelMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3, std::placeholders::_4), #handler)
#define RGS_CHANNEL_ARGS_HANDLER(gate, messageId, handler) gate->rgsChannelArgsMessageHandler(messageId, std::bind(&handler, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3), #handler)

#endif