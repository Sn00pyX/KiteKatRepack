
#include <process.h>
#include <cassert>
#include <vector>

#include "../../include/mmo/ArScheduler.h"
#include "../../include/toolkit/ILock.h"
#include "../../include/toolkit/XThread.h"
#include "../../include/toolkit/XEnv.h"
#include "../../include/toolkit/SafeTickCount.h"
#include "../../include/toolkit/XThreadMonitor.h"
#include "../../include/dump/XExceptionHandler.h"

static void (*s_pfInitFunc)( int ) = NULL;

__declspec( thread ) static volatile bool	s_bIsDestroyer = false;

const DWORD AR_OBJECT_UPDATE_TIME[] = 
{
	0xFFFFFFFF,		// 무한
	10*60*1000,		// 10분
	1*60*1000,		// 1분
	3*1000,			// 3초
	1000,			// 1초
	100,			// 100ms
	0,
};


struct ArObjectDestroyerInfo
{
	ArObjectDestroyerInfo()
	{
		bReqStop = false;
		bIsFinished = false;
		hThread = NULL;
	}

	~ArObjectDestroyerInfo()
	{
	}

	std::vector< ArSchedulerObject* >	vDeleteList;
	std::vector< ArSchedulerObject* >	vWaitList;
	XCriticalSection					destroyLock;

	HANDLE								hThread;
	volatile bool						bReqStop;
	volatile bool						bIsFinished;
};

struct ArModifyTag
{
	ArModifyTag( ArSchedulerObject*	p )
	{
		pObj = p;
		bFlag = true;
	}

	ArSchedulerObject*	pObj;
	bool				bFlag;
};

struct ArSchedulerInfo
{
	ArSchedulerInfo()
	{
		vObjectList.reserve( 4096 );
		vModifyList.reserve( 512 );

		nObjectCount = 0;
		nInstructionCnt = 0;
		bStop = false;
		hThread = NULL;
		nThreadIndex = 0;
	}

	std::vector< ArSchedulerObject* >	vObjectList;
	std::vector< ArModifyTag >			vModifyList;

	XCriticalSection			modifyLock;

	volatile unsigned			nInstructionCnt;
	volatile unsigned			nObjectCount;

	volatile bool				bStop;
	HANDLE						hThread;
	unsigned					nThreadIndex;
};

bool debug_exist( std::vector< ArSchedulerObject* > & v, ArSchedulerObject * p )
{
	for( unsigned i = 0; i < v.size(); i++ )
	{
		if( v[i]->GetPriorityQueueIndex() < 0 )
		{
			return true;
		}
		if( v[i] == p )
		{
			return true;
		}
	}
	return false;
}

bool debug_index( std::vector< ArSchedulerObject* > & v )
{
	size_t sz = v.size();

	for( unsigned i = 0; i < sz; i++ )
	{
		if( v[i]->GetPriorityQueueIndex() < 0 )
		{
			return true;
		}
		if( v[i]->GetPriorityQueueIndex() != i )
		{
			return true;
		}
	}
	return false;
}


ArScheduler::ArScheduler()
{
	m_pArSchedulerInfo = NULL;

	m_pArObjectDestroyerInfo = NULL;

	m_nInstructionCount = 0;
	m_nThreadCount = 0;
}

ArScheduler::~ArScheduler()
{
	DeInit();
}

unsigned ArScheduler::getLazyThread()
{
	long nOld = InterlockedIncrement( &m_nInstructionCount ) - 1;

	return nOld % m_nThreadCount;
}

struct _ArSchedulerImpl
{
	static void SetTimer( ArSchedulerObject * pObj, DWORD t, unsigned tick  )
	{
		pObj->last_proc_time = t;
		pObj->last_proc_tick = tick;
	}

	static bool IsDeleteable( ArSchedulerObject * pPtr )
	{
		if( !pPtr->IsDeleteable() ) return false;
		if( !pPtr->ArSchedulerObject::IsDeleteable() ) return false;
		return true;

	}
	static bool ProcDelete(  ArSchedulerObject * pPtr )		{	return pPtr->ProcDelete();		}
};


void ArScheduler::SetObjectPriority( ArSchedulerObject * obj, ArSchedulerObject::AR_OBJECT_PRIORITY priority )
{
	// DeInit 되고난 후에 호출될 경우 아무것도 하지 않도록 변경(서버 종료 시 일부 객체 소멸자 호출 중 덤프 남는 문제 수정)
	if( !m_pArSchedulerInfo )
		return;

	unsigned target_thread = 0;

	obj->EnterSpinLock();	
	if( obj->thread_index != -1 )			target_thread = obj->thread_index;
	else if( obj->pending_thread_index != -1 )	target_thread = obj->pending_thread_index;
	else target_thread = getLazyThread();
	obj->LeaveSpinLock();

	ArSchedulerInfo *pNewThread = &m_pArSchedulerInfo[ target_thread ];

	// EnterSpinLock 하기 이전에 Lock 을 해야만 한다.
	THREAD_SYNCRONIZE( pNewThread->modifyLock );
	SPINLOCK_SYNCRONIZE( obj->lock );

	// 삭제요청된 객체라면 IDLE 이 되도록 유도해야 한다.
	if( obj->bIsDeleteRequested )
	{
		// 이미 idle 요청 중이면 무시
		if( obj->GetFinalPriority() == ArSchedulerObject::UPDATE_PRIORITY_IDLE )	return;

		// idle 요청이 아니라면 무시
		if( priority != ArSchedulerObject::UPDATE_PRIORITY_IDLE )					return;

		// 이미 idle 이면 무시
		if( obj->GetPriority() == ArSchedulerObject::UPDATE_PRIORITY_IDLE )			return;
	}

	// 이미 해당 상태로 진행중이라면
	if( obj->GetFinalPriority() == priority ) return;

	// pending 된적이 있다면
	if( obj->GetPendingPriority() != ArSchedulerObject::UPDATE_PRIORITY_NULL )
	{
		// 현재 pending 된 mode 라면..
		if( obj->priority == priority )
		{
			// { pending queue 에서 제거			
			pNewThread->vObjectList.back()->pending_priority_queue_index = obj->pending_priority_queue_index;
			pNewThread->vModifyList[ obj->pending_priority_queue_index ] = pNewThread->vObjectList.back();
			obj->pending_priority_queue_index = static_cast< size_t >( -1 );
			// }

			// pending 취소
			obj->pending_priority		= ArSchedulerObject::UPDATE_PRIORITY_NULL;
			obj->pending_thread_index	= -1;
			return;
		}

		// pending 상태만 변경
		obj->pending_priority		= priority;
		obj->pending_thread_index	= target_thread;

		return;
	}

	// 놀던 녀석이면 신규등록
	if( ( obj->GetPriority() == ArSchedulerObject::UPDATE_PRIORITY_IDLE && priority != ArSchedulerObject::UPDATE_PRIORITY_IDLE ) ||
		( obj->GetPriority() != ArSchedulerObject::UPDATE_PRIORITY_IDLE && priority == ArSchedulerObject::UPDATE_PRIORITY_IDLE ) )
	{
		pNewThread->vModifyList.push_back( ArModifyTag( obj ) );

		obj->pending_priority_queue_index = pNewThread->vModifyList.size() - 1;
		obj->pending_thread_index = target_thread;
		obj->pending_priority = priority;
		return;
	}

	// 아니면 priority 만 슬쩍 변경
	obj->priority = priority;
}

unsigned __stdcall ArObjectDestroyer( void * pArg )
{
	s_bIsDestroyer = true;

	XSetThreadName( -1, "ObjectDestroyer" );

	int nDestroyedCount = 0;
	int nDestroyerLoopCount = 0;

	ENV().Bind( "engine.scheduler.destroy_cnt", &nDestroyedCount );
	ENV().Bind( "engine.scheduler.destroy_loop", &nDestroyerLoopCount );

	ArObjectDestroyerInfo * pInfo = static_cast< ArObjectDestroyerInfo* >( pArg );

	const unsigned nProcTime = 100;
	std::vector< ArSchedulerObject* >::iterator it;

	while( true )
	{
		DWORD dwPrevTickCount = GetSafeTickCount();
		++nDestroyerLoopCount;

		{
			THREAD_SYNCRONIZE( pInfo->destroyLock );

			if( pInfo->bReqStop && pInfo->vDeleteList.empty() )
			{
				break;
			}

			// { 이 리스트 순회 도중 vWaitList 에 push_back 을 제외한 어떠한 시도도 하면 안됨.
			for( it = pInfo->vDeleteList.begin(); it != pInfo->vDeleteList.end(); ++it )
			{	
				// _oprint( "ENTER SPIN LOCK : %08X\n", *it );

				(*it)->EnterSpinLock();

				if( !pInfo->bReqStop && !_ArSchedulerImpl::IsDeleteable( *it ) ) 
				{
					(*it)->LeaveSpinLock();
					pInfo->vWaitList.push_back( *it );
					continue;
				}

				(*it)->Disable();
				(*it)->LeaveSpinLock();

				if( _ArSchedulerImpl::ProcDelete( *it ) == false )
				{
					pInfo->vWaitList.push_back( *it );
					continue;
				}

				++nDestroyedCount;

			}
			pInfo->vDeleteList.erase( pInfo->vDeleteList.begin(), pInfo->vDeleteList.end() );
			// }

			if( !pInfo->vWaitList.empty() )
			{
				pInfo->vDeleteList.swap( pInfo->vWaitList );
			}
			ENV().Set( "engine.scheduler.destroy_pend", static_cast< int >( pInfo->vDeleteList.size() ) );
		}

		DWORD dwCurrTickCount = GetSafeTickCount();
		if( dwCurrTickCount - dwPrevTickCount < nProcTime )
		{
			Sleep( nProcTime - ( dwCurrTickCount - dwPrevTickCount ) );
		}
	}

	pInfo->bIsFinished = true;

	return 0L;
}

unsigned __stdcall ArSchedulerEngine( void * pArg )
{
	ArSchedulerInfo * pInfo = static_cast< ArSchedulerInfo * >( pArg );

	volatile unsigned & tick = pInfo->nInstructionCnt;

	std::vector< ArSchedulerObject* >::iterator it;
	std::vector< ArModifyTag >::iterator modify_it;

	char buf[1024];
	s_sprintf( buf, _countof( buf ), "Scheduler %02d", pInfo->nThreadIndex );
	XSetThreadName( -1, buf );

	if( s_pfInitFunc )
	{
		s_pfInitFunc( pInfo->nThreadIndex );
	}

	s_sprintf( buf, _countof( buf ), "engine.scheduler.%d.debug", pInfo->nThreadIndex );

	while( !pInfo->bStop )
	{
		tick++;

		DWORD dwPrevTickCount = GetSafeTickCount();

		// process

		/*
		pInfo->modifyLock.Lock();
		assert( !debug_index( pInfo->vObjectList ) );
		pInfo->modifyLock.UnLock();
		*/

		for( it = pInfo->vObjectList.begin(); it != pInfo->vObjectList.end() && !pInfo->bStop; ++it )
		{
			ArSchedulerObject* pObject = (*it);
			if( !pObject->IsEnable() ) continue;

			// DEBUG
			if( pInfo->vObjectList[ (*it)->GetPriorityQueueIndex() ] != pObject ) assert( 0 );

			// 이미 수행된 녀석이면 스킵
			if( pObject->GetLastProcTick() == tick ) continue;

			if( pObject->bIsDeleteRequested ) continue;

			// { 이전 수행 시간과의 차이가 AR_OBJECT_UPDATE_TIME 만큼 있을때만 process
			DWORD NowSystemTick = GetSafeTickCount();
			if( pObject->GetLastProcTime() == 0 || pObject->GetLastProcTime() + AR_OBJECT_UPDATE_TIME[ pObject->GetPriority() ] < NowSystemTick )
			{
				_ArSchedulerImpl::SetTimer( *it, NowSystemTick, tick );
				pObject->onProcess( pInfo->nThreadIndex );
			}
			// }
		}

		//T assert( !debug_index( pInfo->vObjectList ) );

		{
			// modify
			THREAD_SYNCRONIZE( pInfo->modifyLock );

			for( modify_it = pInfo->vModifyList.begin(); modify_it != pInfo->vModifyList.end(); ++modify_it )
			{
				ArSchedulerObject* pObj = modify_it->pObj;

				// lock object
				SPINLOCK_SYNCRONIZE( pObj->lock );

				// pending 되었던 정보가 제거된 것임 
				if( pObj->pending_thread_index == -1 ) continue;

				// 쓰레드 ID 가 일치 하지 않으면 에러임. (릴리즈에서는 무시)
				if( pObj->priority != ArSchedulerObject::UPDATE_PRIORITY_IDLE && pObj->thread_index != static_cast< int >( pInfo->nThreadIndex ) )
				{
					assert( 0 );
					continue;
				}				

				// if current thread's priority queue doesn't own this obj, skip modify_it.
				if( pObj->pending_thread_index != static_cast< int >( pInfo->nThreadIndex ) )
				{
					assert( 0 );
					continue;
				}

				// 이 상황이 된다면 논리적 에러가 있는것임.
				if( pObj->GetPendingPriority() == ArSchedulerObject::UPDATE_PRIORITY_NULL )
				{
					assert( 0 );
					continue;
				}

				// 제거되어야할 녀석일경우
				if( pObj->GetPendingPriority() == ArSchedulerObject::UPDATE_PRIORITY_IDLE )
				{
					if( pObj->GetPriority() != ArSchedulerObject::UPDATE_PRIORITY_IDLE )
					{
						// move last object to here
						if( modify_it->pObj != pInfo->vObjectList.back() )
						{
							pInfo->vObjectList.back()->priority_queue_index = pObj->GetPriorityQueueIndex();
							pInfo->vObjectList[ pObj->GetPriorityQueueIndex() ] = pInfo->vObjectList.back();
						}

						// pop back last object
						pInfo->vObjectList.pop_back();

						//T assert( !debug_index( pInfo->vObjectList ) );

						// idle 상태로..
						pObj->priority				= ArSchedulerObject::UPDATE_PRIORITY_IDLE;
						pObj->thread_index			= -1;
						pObj->priority_queue_index	= static_cast< size_t >( -1 );

						pInfo->nObjectCount--;
					}
					else
					{
						// 이 상황이라면 논리적 에러가 있는것임
						assert( 0 );
					}
				}
				else
				{	
					pObj->thread_index	= pInfo->nThreadIndex;

					if( pObj->priority == ArSchedulerObject::UPDATE_PRIORITY_IDLE )
					{
						//T debug_exist( pInfo->vObjectList, pObj );

						// { 추가
						pInfo->vObjectList.push_back( pObj );
						pObj->priority_queue_index = pInfo->vObjectList.size() - 1;
						// }

						// priority 설정
						pObj->priority		= pObj->pending_priority;

						pInfo->nObjectCount++;

						//T assert( !debug_index( pInfo->vObjectList ) );
					}
				}				

				// pending 된 정보를 지움
				pObj->pending_priority		= ArSchedulerObject::UPDATE_PRIORITY_NULL;
				pObj->pending_thread_index	= -1;	

				//T assert( !debug_index( pInfo->vObjectList ) );				
			}

			// clear modify list. don't use clear(). VS .Net STL has some bug.
			pInfo->vModifyList.erase( pInfo->vModifyList.begin(), pInfo->vModifyList.end() );
		}

		// { if all works complete within UPDATE_PRIORITY_HIGHEST time,
		//   call Sleep() to avoid CPU monopoly.
		DWORD dwCurrTickCount = GetSafeTickCount();
		if( (dwCurrTickCount - dwPrevTickCount) < AR_OBJECT_UPDATE_TIME[ ArSchedulerObject::UPDATE_PRIORITY_HIGHEST ] )
		{
			Sleep( AR_OBJECT_UPDATE_TIME[ ArSchedulerObject::UPDATE_PRIORITY_HIGHEST ] - ( dwCurrTickCount - dwPrevTickCount ) );
		}
		// }
	}

	// set thread complete event
	pInfo->bStop = false;

	return 0;
}



bool ArScheduler::Init( unsigned thread_count, void (*init_func)( int ), bool bMonitoringThread )
{
	if( m_pArSchedulerInfo ) return false;

	char tmp[128];
	unsigned  dwThreadID;

	m_nThreadCount = thread_count;

	s_pfInitFunc = init_func;
	m_pArSchedulerInfo = new ArSchedulerInfo[m_nThreadCount];
	for( unsigned i = 0; i < m_nThreadCount; i++ )
	{	
		m_pArSchedulerInfo[i].nThreadIndex = i;
		m_pArSchedulerInfo[i].hThread = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, ArSchedulerEngine, &m_pArSchedulerInfo[i], 0, &dwThreadID ) );
		s_sprintf( tmp, _countof( tmp ), "engine.scheduler.%d.instruction", i );
		ENV().Bind( tmp, (int*)&m_pArSchedulerInfo[i].nInstructionCnt );
		s_sprintf( tmp, _countof( tmp ), "engine.scheduler.%d.object", i );
		ENV().Bind( tmp, (int*)&m_pArSchedulerInfo[i].nObjectCount );

		if( bMonitoringThread )
		{
			char szThreadName[64];
			s_sprintf( szThreadName, sizeof( szThreadName ), "ArScheduler_%u", i );
			std::string strThreadName( szThreadName );
			XThreadMonitor::AddWatchingThread( dwThreadID, strThreadName );
		}
	}

	m_pArObjectDestroyerInfo = new ArObjectDestroyerInfo;
	m_pArObjectDestroyerInfo->hThread = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, ArObjectDestroyer, m_pArObjectDestroyerInfo, 0, &dwThreadID ) );

	return true;
}

bool ArScheduler::DeInit()
{
	if( !m_pArSchedulerInfo ) return false;

	unsigned i;
	for( i = 0; i < m_nThreadCount; i++ )
	{
		if( m_pArSchedulerInfo[i].hThread ) CloseHandle( m_pArSchedulerInfo[i].hThread );
		m_pArSchedulerInfo[i].bStop = true;
	}

	// 쓰레드가 끝나기를 기다린다
	for( i = 0; i < m_nThreadCount; i++ )
	{
		while( m_pArSchedulerInfo[i].bStop ) Sleep( 50 );
	}

	delete [] m_pArSchedulerInfo;
	m_pArSchedulerInfo = NULL;

	CloseHandle( m_pArObjectDestroyerInfo->hThread );
	m_pArObjectDestroyerInfo->bReqStop = true;
	while( !m_pArObjectDestroyerInfo->bIsFinished ) Sleep( 50 );

	delete m_pArObjectDestroyerInfo;
	m_pArObjectDestroyerInfo = NULL;

	return true;
}




void ArScheduler::DeleteObject( ArSchedulerObject * obj )
{
	m_pArObjectDestroyerInfo->destroyLock.Lock();
	assert( !obj->bIsDeleteRequested );
	obj->bIsDeleteRequested = true;

	SetObjectPriority( obj, ArSchedulerObject::UPDATE_PRIORITY_IDLE );

	if( s_bIsDestroyer ) 
	{
		m_pArObjectDestroyerInfo->vWaitList.push_back( obj );
	}
	else
	{
		m_pArObjectDestroyerInfo->vDeleteList.push_back( obj );
	}
	m_pArObjectDestroyerInfo->destroyLock.UnLock();
}