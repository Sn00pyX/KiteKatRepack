#pragma once

/*

	Scheduler 는 idle 상태가 아닌 object 들 - active된 object들 - 을 list 로 관리한다.

	각 priority level 마다 1개씩의 list 를 두고, priority 에 따라
	적절한 시간주기로 list 를 순회하며 process 를 처리한다.

	만약 scheduler thread 가 1개이고, list 에 object 들이 충분히 많다면,
	list 1회 순회에 걸리는 시간이 해당 priority 가 지켜줘야할 시간을 초과하게
	되는 문제가 발생한다. 해서 scheduler thread 갯수는 active된 object의 규모에 따라
	적절히 조정되어야 한다.

*/

#include "ArSchedulerObject.h"
#include "../framework/ArcadiaFramework.h"

class ArScheduler
{
public:

	ArScheduler();
	virtual ~ArScheduler();

	bool Init( unsigned thread_count = 1,  void (*init_func)( int ) = 0, bool bMonitoringThread = false );
	bool DeInit();

	void DeleteObject( ArSchedulerObject * obj );

	void SetObjectPriority( ArSchedulerObject * obj, ArSchedulerObject::AR_OBJECT_PRIORITY priority );

private:

	unsigned getLazyThread();
	unsigned m_nThreadCount;
	volatile long m_nInstructionCount;

	struct ArSchedulerInfo *m_pArSchedulerInfo;
	struct ArObjectDestroyerInfo *m_pArObjectDestroyerInfo;
};
