#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>
#include <cassert>


//  LOGIC 요구사항 
//
//  bool is_exclusive( const LOGIC & )
//  copy constructor
//
//  LOCK 요구사항
//
//  void Lock()
//  void UnLock()


template< typename LOGIC, typename LOCK, int kMaxLogicLockCount = 20 >
class ExclusiveLogicLock
{
private:

	struct _LOCK_UNIT
	{
		_LOCK_UNIT()
		{
			thread_id = 0;
			refcount = 0;
			filename = NULL;
			linenumber = 0;
			id = 0;
		}

		LOGIC		c;
		volatile unsigned  refcount;
		int			thread_id;
		LOCK		lock;
		__int64		id;
		const char *	filename;
		int				linenumber;

		void Lock()				{ lock.Lock();				}
		void UnLock()			{ lock.UnLock();			}
		bool IsLocked() const	{ return lock.IsLocked();	}
	};

public:

	ExclusiveLogicLock()
	{
		m_freeBlock = 0;
		m_id = 0;

		m_nLockedThread = 0;
		m_nWaitingThread = 0;

	}

	// -1일경우 lock 하지 않아도 됨
	int LockLogic( const LOGIC & c, const char * filename, int linenumber )
	{
		int my_thread_id = GetCurrentThreadId();

		InterlockedIncrement( &m_nWaitingThread );

		m_lock.Lock();

		if( checkDoubleLock() )
		{
			for( unsigned i = 0; i < kMaxLogicLockCount; i++ )
			{
				if( !m_lockedRegion[i].IsLocked() ) continue;
				if( m_lockedRegion[i].thread_id != my_thread_id ) continue;

				// 락 안해도 되는 경우
				if( c.is_subset_of( m_lockedRegion[i].c ) ) 
				{
					InterlockedDecrement( &m_nWaitingThread );
					m_lock.UnLock();
					return -1;
				}
				else
				{
					assert(0);
					/* by Testors
					FILE *fp = fopen( "DOUBLE_LOCK.txt", "a+" );
					if( fp ) fclose( fp );
					*/
				}
			}
		}

		assert( !checkDoubleLock() );
		set_lock();

		if( m_freeBlock < 0 ) 
		{
			throw XException( "ExclusiveLogicLock() TOO MANY LOCK!" );
		}

		__int64 myid = m_id++;

		int index = m_freeBlock;

		m_lockedRegion[ index ].Lock();
		m_lockedRegion[ index ].c = c;
		m_lockedRegion[ index ].thread_id = my_thread_id;
		m_lockedRegion[ index ].id = myid;
		m_lockedRegion[ index ].filename = filename;
		m_lockedRegion[ index ].linenumber = linenumber;
		++m_lockedRegion[ index ].refcount;

		m_freeBlock = -1;

		for( int i = 0; i < kMaxLogicLockCount; i++ )
		{
			if( m_lockedRegion[i].refcount )
			{	
				if( i == index ) continue;

				if( myid < m_lockedRegion[i].id ) continue;

				// 두 logic 간이 배타적일경우
				if( c.is_exclusive( m_lockedRegion[i].c ) )
				{
					// 해당 logic 의 lock refcount 를 증가시키고
					++m_lockedRegion[i].refcount;

					if( m_freeBlock < 0 )
					{
						for( unsigned x = i+1; x < kMaxLogicLockCount; x++ )
						{
							if( m_lockedRegion[x].refcount ) continue;
							m_freeBlock = x;
							break;
						}
					}

					m_lock.UnLock();
					
					// 해당 logic 의 Lock 이 끝날때까지 대기한다
					m_lockedRegion[i].Lock();

					//해당 logic 을 무효화 시킨다
					m_lock.Lock();
					m_lockedRegion[i].refcount--;
					m_lockedRegion[i].UnLock();
				}
			} else ( m_freeBlock < 0 ? m_freeBlock = i : 0 );
		}

		m_lock.UnLock();
		
		InterlockedIncrement( &m_nLockedThread );
		InterlockedDecrement( &m_nWaitingThread );
  
		return index;
	}

	void UnLock( int handle )
	{
		if( handle < 0 ) return;

		m_lock.Lock();

		unset_lock();

		assert( m_lockedRegion[ handle ].thread_id == static_cast< int >( GetCurrentThreadId() ) );

		--m_lockedRegion[handle].refcount;

		m_lockedRegion[handle].UnLock();

		m_lock.UnLock();

		InterlockedDecrement( &m_nLockedThread );
	}

	const bool IsLocked( const LOGIC & c, const bool bPartialOK = false )
	{
		InterlockedIncrement( &m_nWaitingThread );

		m_lock.Lock();	

		bool bIsLocked = false;

		for( unsigned i = 0; i < kMaxLogicLockCount; i++ )
		{
			if( !m_lockedRegion[i].IsLocked() ) continue;

			// 현재 걸린 락에 일부라도 겹치는 경우
			if( bPartialOK )
			{
				if( c.is_exclusive( m_lockedRegion[i].c ) ) 
				{
					bIsLocked = true;
					break;
				}
			}
			// 현재 걸린 락에 완전히 포함되는 경우
			else if( c.is_subset_of( m_lockedRegion[i].c ) )
			{
				bIsLocked = true;
				break;
			}
		}

		InterlockedDecrement( &m_nWaitingThread );
		m_lock.UnLock();

		return bIsLocked;
	}

	const LONG	& GetLockedThreadCount()	{ return (const LONG &) m_nLockedThread; };
	const LONG	& GetWaitingThreadCount()	{ return (const LONG &) m_nWaitingThread; };
	
private:

	volatile LONG		m_nLockedThread;
	volatile LONG		m_nWaitingThread;

	_LOCK_UNIT			m_lockedRegion[kMaxLogicLockCount];
	LOCK				m_lock;
	volatile int		m_freeBlock;
	volatile __int64	m_id;

	void			set_lock()
	{
		void _exclusive_lock_set_lock();
		_exclusive_lock_set_lock();
	}
	void			unset_lock()
	{
		void _exclusive_lock_unset_lock();
		_exclusive_lock_unset_lock();
	}
	bool			checkDoubleLock()
	{
		bool _exclusive_lock_checkDoubleLock();
		return _exclusive_lock_checkDoubleLock();
	}

};