
#include <oledberr.h>

#include <dump/XException.h>
#include <toolkit/XEnv.h>
#include <toolkit/XConsole.h>
#include <dump/XExceptionHandler.h>
#include <mmo/ArTime.h>

#include "GameDBManager.h"
#include "DBAllocator.h"
#include "DBPerformanceTracker.h"

extern __declspec( thread ) XSEH::THREAD_INFO	s_ThreadInfo;

void db_thread_init_func( int nThreadNum )
{
	HRESULT hr = ::CoInitialize( NULL );
	assert( hr == S_OK );

	s_sprintf( s_ThreadInfo.thread_name, _countof( s_ThreadInfo.thread_name ), "DB %02d", nThreadNum );
	s_ThreadInfo.job_info[0] = '\0';
	s_ThreadInfo.last_execute_time = 0;
}

volatile long	GameDBManager::DBProc::m_lExecQueryCount	= 0;
volatile DWORD	GameDBManager::DBProc::m_dwStartTick		= ::GetTickCount();

GameDBManager::GameDBManager()
{
	m_pGameConnection = NULL;
}

struct GameDBStatObserber
{
	static int GetPendingWorkCount()
	{
		return pDB->m_ThreadPool.GetPendingWorkCount();
	}

	static int GetThreadCount()
	{
		return pDB->m_ThreadPool.GetThreadCount();
	}

	static int GetActiveThreadCount()
	{
		return pDB->m_ThreadPool.GetActiveThreadCount();
	}

	static int GetWaitingWorkCount()
	{
		return static_cast< int >( pDB->m_ThreadPool.GetWaitingWorkCount() );
	}

	static int GetTotalWorkCount()
	{
		return static_cast< int >( pDB->m_ThreadPool.GetTotalWorkCount() );
	}

	static int GetExecQueryCount()
	{
		return GameDBManager::DBProc::GetExecQueryCountPerSec();
	}

	static int GetCurrentQueryCount()
	{
		return static_cast< int >( GetDBHeap().GetSize() );
	}

	static GameDBManager * pDB;
};

GameDBManager * GameDBStatObserber::pDB;

bool GameDBManager::initDBConnection(	DBConnection* & pConnection,
										unsigned thread_number,
										const char *szConnectionString,
										const char *szAccount,
										const char *szPassword )
{
	pConnection = new DBConnection[ thread_number ];

	for( unsigned i = 0; i < thread_number; i++ )
	{
		try
		{
			// create ADO connection instance
			pConnection[i].connection.CreateInstance(__uuidof(Connection));

			// connect to DB
			pConnection[i].connection->Open(	szConnectionString,
												szAccount,
												szPassword,
												adConnectUnspecified );

			if( pConnection[i].CreateCommand( pConnection[i].command ) == false )
			{
				throw XException( "ADO COMMAND_PTR CREATE ERROR!" );
			}
		}
		catch( _com_error &e )
		{
			LogDBError( e, "GameDBManager", "GameDBManager::initDBConnection()" );

			std::string strError = "GAME DB INIT ERROR : ";
			strError += e.Description();

			delete [] pConnection;
			pConnection = NULL;

			throw XException( strError );
		}
	}

	return true;
}

void GameDBManager::InitDBStatusObserver()
{
	GameDBStatObserber::pDB = this;

	ENV().Bind( "db.user.thread_total", GameDBStatObserber::GetActiveThreadCount );
	ENV().Bind( "db.user.thread_active", GameDBStatObserber::GetActiveThreadCount );
	ENV().Bind( "db.user.work_active", GameDBStatObserber::GetPendingWorkCount );
	ENV().Bind( "db.user.work_pending", GameDBStatObserber::GetWaitingWorkCount );
	ENV().Bind( "db.user.work_total", GameDBStatObserber::GetTotalWorkCount );
	ENV().Bind( "db.user.exec_query", GameDBStatObserber::GetExecQueryCount );
	ENV().Bind( "db.user.remain_query", GameDBStatObserber::GetCurrentQueryCount );
}

bool GameDBManager::InitGameDB(	unsigned thread_number,
								const char *szConnectionString,
								const char *szAccount,
								const char *szPassword )
{
	return initDBConnection( m_pGameConnection, thread_number, szConnectionString, szAccount, szPassword );
}

bool GameDBManager::DeInitGameDB()
{
	delete [] m_pGameConnection;

	m_pGameConnection = NULL;

	return true;
}

bool GameDBManager::Push( DBProc * pWork )
{	
	return m_ThreadPool.Push( pWork );
}

GameDBManager & DB()
{
	static GameDBManager _db;

	return _db;
}

//extern void InvokeUnhandledException( DWORD dwExceptionCode );

bool GameDBManager::DBProc::onProcess( int nThreadNum )
{
	// _oprint( "DB : %s\n", this->szProcName );

	DBPerformanceTrackHelper helper;
	try
	{	
		char buf[255];

		s_sprintf( buf, _countof( buf ), "thread.db.%d.proc", nThreadNum );
		ENV().Set( buf, szProcName );

		s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ),
			"%s(0x%p)",
			szProcName, this );

		s_ThreadInfo.last_execute_time = GetArTime();

		DBProc::IncExeQueryCount();

		helper.start();

		bool result = onProcess( DB().GetGameDBConnection( nThreadNum ) );

		helper.end( szProcName, creationTime );
		return result;
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), szProcName, creationTime );

		LogDBError( e, nThreadNum, szProcName, szStoredProcedureName,
			szStoredProcedureDebugInfo[0] == '\0' ? NULL : szStoredProcedureDebugInfo );

		if( bNeedToContinueOnFail )
		{
			onFail( e );
		}
		else
			XSEH::InvokeUnhandledException( (LONG)0xC000000DL );

		return false;
	}
	catch( std::exception &e )
	{
		helper.end( -1, szProcName, creationTime );

		std::string strError = "DB GENERAL ERROR : ";
		strError += szProcName;
		strError += " : ";
		strError += e.what();
		strError += "\n";
		_lprint( "GameLog.txt", "%s", strError.c_str() );

		XSEH::InvokeUnhandledException( (LONG)0xC000000DL );

		return false;
	}
	catch( ... )
	{
		helper.end( -1, szProcName, creationTime );

		_lprint( "GameLog.txt", "%s", "Unknown DB Error\n" );

		XSEH::InvokeUnhandledException( (LONG)0xC000000DL );

		return false;
	}
}

void * GameDBManager::DBProc::operator new( size_t nAllocSize )
{
	assert( GetDBHeap().GetBlockSize() >= nAllocSize );

	DBProc * pBlock = allocDBProcStruct();
	if( !pBlock )
		throw std::bad_alloc( "GameDBManager::DBProc alloc failed." );

	g_DBTracker.updateMaxCount();

	return pBlock;
}

void * GameDBManager::DBProc::operator new[]( size_t nArraySize )
{
	assert( 0 );
	throw std::bad_alloc( "GameDBManager::DBProc: XMemoryPool does not support allocating an array." );
}

void GameDBManager::DBProc::operator delete( void * pBlock )
{
	DBProc * pFreeBlock = reinterpret_cast< DBProc * >( pBlock );

	prepareFreeDBProcStruct( pFreeBlock );

	pFreeBlock->~DBProc();

	freeDBProcStruct( pFreeBlock );
}

void GameDBManager::DBProc::operator delete[]( void * pBlock )
{
	assert( 0 );
	throw std::exception( "GameDBManager::DBProc: XMemoryPool does not support de-allocating an array." );
}
