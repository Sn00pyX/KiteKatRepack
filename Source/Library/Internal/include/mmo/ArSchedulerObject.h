#pragma once

#include "ArType.h"
#include "../toolkit/ILock.h"

/*

	ArObject 의 priority 가 설정되면, 스케쥴러는 비교적 아래의 주기로 해당 object 의
	처리를 하려 노력한다. (비교적 이란 말은 서버의 부하가 많이 걸릴경우 아래의 시간
	정밀도가 정확히 지켜지지 않을수 있다는 것을 의미한다. 서버는 RTOS 가 아니다)
	아래의 시간은 ArScheduler.cpp 의 AR_OBJECT_UPDATE_TIME 에 설정되어 있다.
	
	----------------------------+-------------+--------------------------------------------
	Priority                    | time        | desc
	----------------------------+-------------+--------------------------------------------
	UPDATE_PRIORITY_HIGHEST		| 100 ms	  | NPC 의 전투처리등에 쓰도록 하시오.
	UPDATE_PRIORITY_HIGH		|   1 second  | 플레이어의 시야에 들어오면 기본적으로 이 상태임.
	UPDATE_PRIORITY_NORMAL		|   3 second  | 
	UPDATE_PRIORITY_LOW			|   1 minute  | 땅바닥에 떨어진 아이템, 시체처리등에 사용하면 적당
	UPDATE_PRIORITY_LOWEST		|  10 minute  |	
	UPDATE_PRIORITY_IDLE		|     no proc | IDLE 상태는 아무 처리가 되지 않음.

*/

struct ArSchedulerObject
{
	enum AR_OBJECT_PRIORITY
	{
		UPDATE_PRIORITY_NULL		= -1,
		UPDATE_PRIORITY_IDLE		= 0,
		UPDATE_PRIORITY_LOWEST,
		UPDATE_PRIORITY_LOW,
		UPDATE_PRIORITY_NORMAL,	
		UPDATE_PRIORITY_HIGH,
		UPDATE_PRIORITY_HIGHEST,
		UPDATE_PRIORITY_DUMMY,	// enum 의 마지막은 항상 UPDATE_PRIORITY_DUMMY 여야함.
	};

	ArSchedulerObject()
		: priority( UPDATE_PRIORITY_IDLE )
		, priority_queue_index( static_cast< size_t >( -1 ) )
		, pending_priority_queue_index( static_cast< size_t >( -1 ) )
		, pending_priority( UPDATE_PRIORITY_NULL )
		, pending_thread_index( -1 )
		, m_bIsEnable( true )
		, thread_index( -1 )
		, bIsDeleteRequested( false )
		, last_proc_time( 0 )
		, last_proc_tick( 0 )
	{
		spinlock_init( &lock );
	}

	

	
	virtual AR_HANDLE			GetHandle() const			{ return 0;					}

	virtual void				onProcess( int nThreadIdx )	{};

	inline bool					IsEnable() const		{ return m_bIsEnable; }
	inline void					Disable()				{ m_bIsEnable = false; }

	inline bool					IsDeleteRequested() const	{ return bIsDeleteRequested; }
	
	inline
	volatile AR_OBJECT_PRIORITY	GetPriority() const				{ return priority;	}

	inline volatile size_t		GetPriorityQueueIndex() const	{ return priority_queue_index; }
	inline volatile int			GetPendingPriority() const		{ return pending_priority; }
	inline volatile int			GetFinalPriority() const
	{
		return ( pending_priority == UPDATE_PRIORITY_NULL ? priority : pending_priority );
	}

	inline void					EnterSpinLock()					{ spinlock_enter( &lock ); }
	inline void					LeaveSpinLock()					{ spinlock_leave( &lock ); }

	inline volatile DWORD		GetLastProcTime() const			{ return last_proc_time;	 }
	inline volatile unsigned	GetLastProcTick() const			{ return last_proc_tick;	 }

	volatile bool					bIsDeleteRequested;
	

	// 시간
	volatile DWORD					last_proc_time;
	volatile unsigned				last_proc_tick;

	volatile AR_OBJECT_PRIORITY		pending_priority;		// 변경되고자 하는 priority	
	volatile int					pending_thread_index;
	volatile size_t					pending_priority_queue_index;	// priority queue 에서의 위치
	
	volatile AR_OBJECT_PRIORITY		priority;				// 현재 priority
	volatile int					thread_index;
	volatile size_t					priority_queue_index;	// priority queue 에서의 위치

	volatile LONG					lock;

protected:

	virtual bool				ProcDelete()			{ return false;				}

	virtual bool IsDeleteable()
	{
		return  ( pending_priority == UPDATE_PRIORITY_NULL && priority == UPDATE_PRIORITY_IDLE ) ;
	}

	virtual ~ArSchedulerObject()
	{
	}

private:

	friend struct					_ArSchedulerImpl;

	volatile bool					m_bIsEnable;
};
