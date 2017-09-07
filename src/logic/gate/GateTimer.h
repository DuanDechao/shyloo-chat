#ifndef SL_GATE_TIMER_H
#define SL_GATE_TIMER_H
#include "slikernel.h"
#include "slpool.h"
class GateTimer : public sl::api::ITimer{
public:
	inline static GateTimer* create(int64 agentId){
		return CREATE_FROM_POOL(s_pool, agentId);
	}

	inline void release(){
		s_pool.recover(this);
	}

	virtual void onInit(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onStart(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTime(sl::api::IKernel* pKernel, int64 timetick);
	virtual void onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick);
	virtual void onPause(sl::api::IKernel* pKernel, int64 timetick){}
	virtual void onResume(sl::api::IKernel* pKernel, int64 timetick) {}

private:
	friend sl::SLPool<GateTimer>;
	GateTimer(int64 agentId) :_agentId(agentId){}
	~GateTimer(){}

private:
	int64	_agentId;
	static sl::SLPool<GateTimer> s_pool;
};

#endif