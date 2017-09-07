#include "GateTimer.h"
#include "Gate.h"
sl::SLPool<GateTimer> GateTimer::s_pool;
void GateTimer::onStart(sl::api::IKernel* pKernel, int64 timetick){
	Gate::getInstance()->onGateTimerStart(pKernel, _agentId, timetick);
}

void GateTimer::onTime(sl::api::IKernel* pKernel, int64 timetick){
	Gate::getInstance()->onGateTimer(pKernel, _agentId, timetick);
}

void GateTimer::onTerminate(sl::api::IKernel* pKernel, bool beForced, int64 timetick){
	Gate::getInstance()->onGateTimerEnd(pKernel, _agentId, timetick);
}