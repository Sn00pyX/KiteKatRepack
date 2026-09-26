// boss-worker thread model
//
//  by Testors 2003/08/??

#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

#include <list>

#include "XThread.h"
#include "../toolkit/safe_function.h"
#include "../toolkit/XThreadMonitor.h"

/*
	bool StartThread( int nThreadCount );
	bool EndThread();

	bool Push( XWorker* pWork );		// work 을 추가한다

	void ProcessWaitingWork();		// 대기중인 모든 work 을 현재 thread 에서 처리한다. 
	void WaitForPendingWork();		// 다른 thread 에서 동작중인 work 이 완료될때까지 대기한다

	long GetWaitingWorkCount();		// 현재 대기중인 work 의 갯수
	long GetThreadCount();			// 현재 만들어진 thread 의 갯수
	long GetActiveThreadCount();	// 현재 대기모드가 아닌 thread 의 갯수
	long GetPendingWorkCount();		// 현재 처리중인 work 의 갯수
	long GetTotalWorkCount();		// 총 work 갯수 (처리된것이든, 대기된것이든)
*/

class XBossWorker
{
public:

	struct XWorker
	{
		virtual ~XWorker()							{}
		virtual bool onProcess( int nThreadNum )	= 0;
		virtual void onEnd( bool bIsCancel )		= 0;
	};

	XBossWorker()
	{
		m_hEvent[0] = CreateEvent( NULL, false, false, NULL );
		m_hEvent[1] = CreateEvent( NULL, false, false, NULL );

		if ( !m_hEvent[0] || !m_hEvent[1] ) throw "XBossWorker::CreateEvent() ERROR!";

		m_bIsStarted = false;

		m_nWaitingWorkCount = 0;
		m_lThreadCount = 0;
		m_lActiveThreadCount = 0;
		m_lPendingWorkCount = 0;
		m_lTotalWorkCount = 0;

		InitializeCriticalSection( &m_CS );
	}

	virtual ~XBossWorker()
	{
		if ( m_bIsStarted )
		{
			EndThread();
		}

		CloseHandle( m_hEvent[0] );
		CloseHandle( m_hEvent[1] );

		DeleteCriticalSection( &m_CS );
	}

	bool StartThread( const char *szThreadName, int nThreadCount, int nPriority = THREAD_PRIORITY_NORMAL, void (*InitFunc)( int ) = NULL, bool bRegisterMonitoring = false )
	{
		DWORD dwVersion = ::GetVersion();		
		if( dwVersion >= 0x80000000 )
		{
			return false;
		}

		if ( m_bIsStarted ) return false;

		m_lActiveThreadCount = 0;
		m_nWaitingWorkCount = 0;
		m_lPendingWorkCount = 0;
		
		m_nState = BW_WAIT;

		m_bIsStarted = true;

		for ( int i = 0; i < nThreadCount; i++ )
		{
			_threadArg * pArg = new _threadArg;

			pArg->pnState	= &m_nState;
			pArg->hEvent	= m_hEvent;
			pArg->plstWork	= &m_lstWork;
			pArg->pCS		= &m_CS;
			pArg->pnState	= &m_nState;
			pArg->plActiveThreadCount = &m_lActiveThreadCount;
			pArg->pnWaitingWorkCount = &m_nWaitingWorkCount;
			pArg->plPendingWorkCount = &m_lPendingWorkCount;
			pArg->plThreadCount = &m_lThreadCount;
			pArg->nThreadNum = i;
			pArg->nThreadPriority = nPriority;
			pArg->InitFunc = InitFunc;
			s_strcpy( pArg->szThreadName, _countof( pArg->szThreadName ), szThreadName );

			unsigned dwThreadID;
			HANDLE hThread = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, XBossWorker::_threadFunc, pArg, 0, &dwThreadID ) );

			if ( !hThread )
			{
				delete pArg;
				return false;
			}

			if( bRegisterMonitoring )
			{
				char number[16];
				s_sprintf( number, sizeof( number ), "_%d", i );
				std::string strThreadName( szThreadName );
				strThreadName += number;
				XThreadMonitor::AddWatchingThread( dwThreadID, strThreadName );
			}

			CloseHandle( hThread );
		}

		m_nState = BW_GO;

		return true;
	}

	bool EndThread()
	{
		if ( !m_bIsStarted ) return false;

		m_nState = BW_STOP;

		// 고마 하라고 알려준 다음..
		while ( m_lThreadCount )
		{
			SetEvent( m_hEvent[1] );
			Sleep( 50 );
		}
		
		clearWorkList();

		m_bIsStarted = false;

		m_lActiveThreadCount = 0;
		m_nWaitingWorkCount = 0;
		m_lPendingWorkCount = 0;
		m_lThreadCount = 0;

		return true;
	}

	bool Push( XWorker* pWork )
	{
		if ( !m_bIsStarted )
		{
			bool bRtn = pWork->onProcess( 0 );
			pWork->onEnd( false );
			return bRtn;
		}

		EnterCriticalSection( &m_CS );

		++m_lTotalWorkCount;

		m_lstWork.push_back( pWork );

		m_nWaitingWorkCount = m_lstWork.size();

		LeaveCriticalSection( &m_CS );

		SetEvent( m_hEvent[0] );

		return true;
	}

	void ProcessWaitingWork()
	{
		EnterCriticalSection( &m_CS );

		while ( !m_lstWork.empty() ) 
		{
			// { fetch work 
			XWorker * pWork;

			pWork = m_lstWork.front();
			m_lstWork.pop_front();

			// 일을 하자~
			pWork->onProcess( 0 );

			pWork->onEnd( false );
		}

		m_nWaitingWorkCount = m_lstWork.size();

		LeaveCriticalSection( &m_CS );
	}

	void WaitForPendingWork()
	{
		while ( m_lPendingWorkCount ) Sleep( 50 );
	}

	size_t	GetWaitingWorkCount()		{ return m_nWaitingWorkCount;	}
	long	GetThreadCount()			{ return m_lThreadCount;		}
	long	GetActiveThreadCount()		{ return m_lActiveThreadCount;	}
	long	GetPendingWorkCount()		{ return m_lPendingWorkCount;	}
	long	GetTotalWorkCount()			{ return m_lTotalWorkCount;		}

private:

	static unsigned __stdcall WINAPI _threadFunc( void* pArg )
	{
		_threadArg Arg;
	
		s_memcpy( &Arg, sizeof( Arg ), pArg, sizeof(Arg) );

		InterlockedIncrement( const_cast< long*>( Arg.plThreadCount ) );

		InterlockedIncrement( const_cast< long* >( Arg.plActiveThreadCount ) );

		delete reinterpret_cast< _threadArg* >( pArg );

		XWorker * pWork = NULL;
		size_t nWorkCount;

		/*
		char szThreadName[300];
		s_sprintf( szThreadName, _countof( szThreadName ), "%s %02d", Arg.szThreadName, Arg.nThreadNum );
		XSetThreadName( -1, szThreadName );
		*/

		SetThreadPriority( GetCurrentThread(), Arg.nThreadPriority );

		if ( Arg.InitFunc )
		{
			Arg.InitFunc( Arg.nThreadNum );
		}
 
		while ( true )
		{
			InterlockedDecrement( const_cast< long* >( Arg.plActiveThreadCount ) );

			WaitForMultipleObjects( 2, Arg.hEvent, false, INFINITE );

			if ( *Arg.pnState == BW_STOP )
			{
				break;
			}

			InterlockedIncrement( const_cast< long* >( Arg.plActiveThreadCount ) );

			while ( true ) 
			{
				// { fetch work 
				EnterCriticalSection( Arg.pCS );
				
				nWorkCount = Arg.plstWork->size();
				if ( nWorkCount ) 
				{
					pWork = Arg.plstWork->front();
					Arg.plstWork->pop_front();
				}

				*Arg.pnWaitingWorkCount = Arg.plstWork->size();

				LeaveCriticalSection( Arg.pCS );
				// }

				// 할일이 없다면 다시 대기모드로
				if ( !nWorkCount ) break;

				// 일을 하자~
				InterlockedIncrement( const_cast< long* >( Arg.plPendingWorkCount ) );

				pWork->onProcess( Arg.nThreadNum );

				pWork->onEnd( false );

				InterlockedDecrement( const_cast< long* >( Arg.plPendingWorkCount ) );
			}
		}

		InterlockedDecrement( const_cast< long* >( Arg.plThreadCount ) );
		
		return 0L;
	}

	void clearWorkList()
	{
		EnterCriticalSection( &m_CS );

		for ( std::list< XWorker* >::iterator it = m_lstWork.begin(); it != m_lstWork.end(); it++ )
		{
			(*it)->onEnd( true );
		}

		m_lstWork.clear();

		LeaveCriticalSection( &m_CS );
	}

	struct _threadArg
	{
		HANDLE *				hEvent;
		std::list< XWorker* >*	plstWork;	
		volatile int *			pnState;
		volatile long *			plActiveThreadCount;
		volatile size_t *		pnWaitingWorkCount;
		volatile long *			plPendingWorkCount;
		volatile long *			plThreadCount;
		CRITICAL_SECTION *		pCS;
		int						nThreadNum;
		char					szThreadName[256];
		int						nThreadPriority;
		void					(*InitFunc)( int );
	};

	enum
	{
		BW_WAIT = 0,
		BW_GO,
		BW_STOP,
	};

	volatile size_t		m_nWaitingWorkCount;
	volatile long		m_lThreadCount;
	volatile long		m_lActiveThreadCount;
	volatile long		m_lPendingWorkCount;
	volatile long		m_lTotalWorkCount;
	CRITICAL_SECTION	m_CS;
	bool				m_bIsStarted;
	HANDLE				m_hEvent[2];
	volatile int		m_nState;
	std::list< XWorker* >	m_lstWork;

};

