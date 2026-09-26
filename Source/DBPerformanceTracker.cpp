#pragma once

#include "DBPerformanceTracker.h"

#include <toolkit/XEnv.h>
#include <logging/FileLog.h>
#include <mmo/ArcadiaServer.h>

#include "DBAllocator.h"
#include "StructPlayer.h"


DBPerformanceTracker g_DBTracker;

DBPerformanceTracker::DBPerformanceTracker()
	: m_isTraceOn( false )
	, m_refreshPeriod( 0 )
	, m_lastRefreshTick( 0 )
	, m_freq( 0 )
	, m_maxQueryCount( 0 )
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency( &li );
	m_freq = li.QuadPart;
}

void	DBPerformanceTracker::init( int refreshPeriod )
{
	m_refreshPeriod = refreshPeriod;
	m_lastRefreshTick = ::GetTickCount();
	m_isTraceOn = true;

	ENV().Bind( "db.trace", &m_isTraceOn );
	ENV().Bind( "db.trace_refresh_period", &m_refreshPeriod );

	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "DB_Debug", "\tStart DB logging at frequency %lld", m_freq );
}

bool	DBPerformanceTracker::isTraceOn()	const
{
	return m_isTraceOn;
}

void	DBPerformanceTracker::add( HRESULT result, const char* className, const __int64 pendingTime, const __int64 workingTime )
{
	if( isTraceOn() == false || className == NULL )
	{
		return;
	}

	if( FAILED( result ) )	// 에러는 통계에 포함하지 않는다.
	{
		dump_error_info( result, className, pendingTime, workingTime );
	}
	else
	{
		THREAD_SYNCHRONIZE( m_lock );

		StoredProcedureTraceInfo& info = m_traceInfos[ className ];

		++info.queryCount;
		info.totalPendingTime += pendingTime;
		info.totalWorkingTime += workingTime;

		if( info.longestPending.longestTime <= pendingTime )
		{
			info.longestPending.longestTime = pendingTime;
			info.longestPending.execQuery = static_cast< int >( GameDBManager::DBProc::GetExecQueryCountPerSec() );
			info.longestPending.remainQuery = static_cast< int >( GetDBHeap().GetSize() );
			info.longestPending.playerCount = StructPlayer::GetPlayerCount();
		}

		if( info.longestWorking.longestTime <= workingTime )
		{
			info.longestWorking.longestTime = workingTime;
			info.longestWorking.execQuery = static_cast< int >( GameDBManager::DBProc::GetExecQueryCountPerSec() );
			info.longestWorking.remainQuery = static_cast< int >( GetDBHeap().GetSize() );
			info.longestWorking.playerCount = StructPlayer::GetPlayerCount();
		}

		// 갱신되었다면
		if( info.longestPending.longestTime == pendingTime ||
			info.longestWorking.longestTime == workingTime )
		{
			dump_info( className, info );
		}
	}
}

void	DBPerformanceTracker::updateMaxCount()
{
	if( isTraceOn() == false )
	{
		return;
	}

	int size = static_cast< int >( GetDBHeap().GetSize() );
	if ( size > m_maxQueryCount )
	{
		m_maxQueryCount = size;
	}
}

void	DBPerformanceTracker::dump()
{
	THREAD_SYNCHRONIZE( m_lock );

	dump_all();
}

void	DBPerformanceTracker::dump_all()	const
{
	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "DB_Debug", "\tStart DB Trace Dump count: %d", m_traceInfos.size() );

	TraceMap::const_iterator it = m_traceInfos.begin();
	TraceMap::const_iterator end = m_traceInfos.end();
	for( ; it != end; ++it )
	{
		const StoredProcedureTraceInfo& info = it->second;
		dump_info( it->first, info );
	}

	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "DB_Debug", "Max Concurrent Query Count\t%d", m_maxQueryCount );

	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "DB_Debug", "\tEnd DB Trace Dump\n" );
}

void	DBPerformanceTracker::dump_info( const char* className, const StoredProcedureTraceInfo& info )	const
{
	if( className != NULL && info.queryCount > 0 )
	{
		double avgPendingTime = (static_cast<double>( info.totalPendingTime ) / m_freq) / info.queryCount;
		double avgWorkingTime = (static_cast<double>( info.totalWorkingTime ) / m_freq) / info.queryCount;

		// total 정보
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "DB_Debug",
			"%s\tQueryCount: %d, PendingAVG: %f(s), WorkingAVG: %f(s)",
			className, info.queryCount, avgPendingTime, avgWorkingTime );

		// 가장 오래 기다릴때 당시 정보
		double longestPendingTime = static_cast< double >( info.longestPending.longestTime ) / m_freq;
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "DB_Debug",
			"\tLongestPending\tTime: %f(s)(at that time, ExecQuery: %d, RemainQuery: %d, PCCount: %d)",
			longestPendingTime, info.longestPending.execQuery, info.longestPending.remainQuery, info.longestPending.playerCount );

		// 가장 오래 일할때 당시 정보
		double longestWorkingTime = static_cast< double >( info.longestWorking.longestTime ) / m_freq;
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "DB_Debug",
			"\tLongestWorking\tTime: %f(s)(at that time, ExecQuery: %d, RemainQuery: %d, PCCount: %d)",
			longestWorkingTime, info.longestWorking.execQuery, info.longestWorking.remainQuery, info.longestWorking.playerCount );
	}
}

void	DBPerformanceTracker::dump_error_info( HRESULT result, const char* className, const __int64 pendingTime, const __int64 workingTime )	const
{
	if( className != NULL )
	{
		double pendingSec = static_cast< double >( pendingTime ) / m_freq;
		double workingSec = static_cast< double >( workingTime ) / m_freq;

		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "DB_Error",
			"Failed: %s(0x%X)\tPendingTime: %f(s), WorkingTime: %f(s)",
			className, result, pendingSec, workingSec );

	}
}

void	DBPerformanceTracker::onProcess( int nThreadIdx )
{
	if( m_refreshPeriod > 0 )
	{
		DWORD dwNowTick = ::GetTickCount();
		if( (dwNowTick-m_lastRefreshTick) >= (static_cast< DWORD >( m_refreshPeriod ) * (60*1000)) )
		{
			m_lastRefreshTick = dwNowTick;

			THREAD_SYNCHRONIZE( m_lock );

			dump_all();

			m_traceInfos.clear();
			m_maxQueryCount = 0;
		}
	}
}
