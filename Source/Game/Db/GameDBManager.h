#pragma once

#include <toolkit/XBossWorker.h>
#include <dump/XExceptionHandler.h>
#include <toolkit/ILock.h>

#include <cassert>
#include <list>

#include "ADOConnection.h"


void db_thread_init_func( int nThreadNum );

extern class GameDBManager & DB();

class GameDBManager
{
public:
	struct DBProc :	public XBossWorker::XWorker
	{
		DBProc( const char *_szProcName = NULL, bool _bNeedToContinueOnFail = false )
			: szProcName( _szProcName )
			, szStoredProcedureName( NULL )
			, bNeedToContinueOnFail( _bNeedToContinueOnFail )
		{
			LARGE_INTEGER li;
			QueryPerformanceCounter( &li );
			creationTime = li;

			memset( szStoredProcedureDebugInfo, 0, sizeof( szStoredProcedureDebugInfo ) );
		}

		virtual ~DBProc()									{}

		virtual bool				onProcess( DBConnection & db )	= 0;
		virtual void				onEnd( bool bIsCancel )				{}

		virtual bool				onProcess( int nThreadNum );

		virtual const std::string	getDebugInfo()							{ return "No debug info"; }
		virtual void				onFail( const _com_error & exception )	{}

		virtual void				setDBQueryQueue( class DBQueryQueue *p ) { pQueryQueue = p; }

		virtual bool operator==( const DBProc & rh )			{	return false;	}

		void * operator new( size_t nAllocSize );
		void * operator new[]( size_t nArraySize );
		void operator delete( void * pBlock );
		void operator delete[]( void * pBlock );

		class DBQueryQueue *pQueryQueue;
		const char *szProcName;
		const char *szStoredProcedureName;
		char szStoredProcedureDebugInfo[256];
		bool bNeedToContinueOnFail;

		LARGE_INTEGER creationTime;

		static void	IncExeQueryCount()			{ InterlockedIncrement( &m_lExecQueryCount );	}
		static int	GetExecQueryCountPerSec()
		{
			DWORD dwNowTick = ::GetTickCount();
			DWORD dwAccumulateTick = dwNowTick - m_dwStartTick;
			if( dwAccumulateTick < 1000 )
			{
				return m_lExecQueryCount;
			}

			int nQueryCount = static_cast< int >( m_lExecQueryCount / (dwAccumulateTick/1000.) );

			m_lExecQueryCount = 0;
			m_dwStartTick = dwNowTick;

			return nQueryCount;
		}

		static volatile DWORD	m_dwStartTick;
		static volatile long	m_lExecQueryCount;
	};

	GameDBManager();

	virtual ~GameDBManager()
	{
		if( m_pGameConnection ) DeInitGameDB();

		if( m_ThreadPool.GetThreadCount() )		EndThread();
	}

	void InitDBStatusObserver();

	bool InitGameDB(	unsigned thread_number,
						const char *szConnectionString,
						const char *szAccount,
						const char *szPassword );
	bool DeInitGameDB();

	bool StartThread( int thread_number )
	{
		return m_ThreadPool.StartThread( "DB", thread_number, THREAD_PRIORITY_NORMAL, db_thread_init_func, true );
	}
	bool EndThread()
	{
		return m_ThreadPool.EndThread();
	}

	bool Push( DBProc * pWork );

	DBConnection & GetGameDBConnection( int nThreadIndex )
	{
		return m_pGameConnection[ nThreadIndex ];
	}

	size_t	GetWaitingWorkCount()		{ return m_ThreadPool.GetWaitingWorkCount();	}
	long	GetThreadCount()			{ return m_ThreadPool.GetThreadCount();			}
	long	GetActiveThreadCount()		{ return m_ThreadPool.GetActiveThreadCount();	}
	long	GetPendingWorkCount()		{ return m_ThreadPool.GetPendingWorkCount();	}
	long	GetTotalWorkCount()			{ return m_ThreadPool.GetTotalWorkCount();		}

private:
	bool initDBConnection(	DBConnection* & pConnection,
							unsigned thread_number,
							const char *szConnectionString,
							const char *szAccount,
							const char *szPassword );

	friend struct GameDBStatObserber;

	DBConnection*				m_pGameConnection;
	XBossWorker					m_ThreadPool;
};

class DBQueryQueue
{
public:

	DBQueryQueue( GameDBManager* pManager ) : m_pManager( pManager )
	{
		m_nDelay = 0;
	};

	XCriticalSection & GetLock()		{ return m_Lock; }

	bool DBQuery( GameDBManager::DBProc * pWork, bool bDeleteIfCached = true )
	{
		THREAD_SYNCRONIZE( m_Lock );

		// 동일 쿼리가 대기중이면 무시..
		for( std::list< GameDBManager::DBProc* >::iterator it = m_listQuery.begin(); it != m_listQuery.end(); ++it )
		{
			if( (*pWork) == *(*it) ) 
			{
				if( bDeleteIfCached ) delete pWork;
				return false;
			}
		}

		pWork->setDBQueryQueue( this );

		bool bIsEmpty = m_listQuery.empty();

		m_listQuery.push_back( pWork );

		if( bIsEmpty ) 
		{
			return m_pManager->Push( pWork );
		}

		return true;
	}

	void onEndQuery()
	{
		THREAD_SYNCRONIZE( m_Lock );

		m_listQuery.pop_front();

		if( !m_listQuery.empty() )
		{
			DB().Push( m_listQuery.front() );
		}
	}

	int GetQueryCount()
	{
		THREAD_SYNCRONIZE( m_Lock );

		return (int)m_listQuery.size();
	}

	void SetLockName( const char* name )
	{
		m_Lock.m_strName = name;
	}

private:

	std::list< GameDBManager::DBProc* >	m_listQuery;
	GameDBManager*						m_pManager;

	int									m_nDelay;
	XCriticalSection					m_Lock;
};