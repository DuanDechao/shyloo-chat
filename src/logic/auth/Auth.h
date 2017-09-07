#ifndef __SL_FRAMEWORK_AUTH_H__
#define __SL_FRAMEWORK_AUTH_H__
#include "slikernel.h"
#include "slimodule.h"
#include "IAuth.h"
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

class Auth :public IAuth, public INodeListener, public sl::api::ITimer, public SLHolder<Auth>{
	typedef std::unordered_map<ChatPublic::TicketID, std::string> TOKEN_MAP;
public:
	virtual bool initialize(sl::api::IKernel * pKernel);
	virtual bool launched(sl::api::IKernel * pKernel);
	virtual bool destory(sl::api::IKernel * pKernel);

	virtual void onOpen(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const char* ip, const int32 port);
	virtual void onClose(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId);

	void onChannelSyncToken(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);
	void onGateVerifyToken(sl::api::IKernel* pKernel, const int32 nodeType, const int32 nodeId, const sl::OBStream& args);

private:
	sl::api::IKernel* _kernel;
	Auth*		_self;
	IHarbor*	_harbor;
	TOKEN_MAP	_tokenMap;
};

#endif