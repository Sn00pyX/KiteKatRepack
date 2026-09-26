#pragma once

#include <map>

#include <toolkit/ILock.h>
#include <mmo/ArSchedulerObject.h>

struct	StoredProcedureTraceInfo
{
	struct	LongestInfo
	{
		LongestInfo()
			: longestTime( 0 )
			, execQuery( 0 )
			, remainQuery( 0 )
			, playerCount( 0 )
		{
		}

		__int64	longestTime;
		int		execQuery;
		int		remainQuery;
		int		playerCount;
	};

	StoredProcedureTraceInfo()
		: queryCount( 0 )
		, totalPendingTime( 0 )
		, totalWorkingTime( 0 )
	{
	}

	__int64		queryCount;
	__int64		totalPendingTime;
	__int64		totalWorkingTime;
	LongestInfo	longestPending;
	LongestInfo	longestWorking;
};


class	DBPerformanceTracker : public ArSchedulerObject
{
public:

	DBPerformanceTracker();

	void	init( int refreshPeriod = 0 );	// <= 0: 수동, 0 > 주기(단위 분)

	bool	isTraceOn()	const;

	void	add( HRESULT result, const char* className, const __int64 pendingTime, const __int64 workingTime );
	void	updateMaxCount();

	void	dump();
	void	onProcess( int nThreadIdx );

private:

	void	dump_all()	const;
	void	dump_info( const char* className, const StoredProcedureTraceInfo& info )	const;
	void	dump_error_info( HRESULT result, const char* className, const __int64 pendingTime, const __int64 workingTime )	const;

private:

	typedef std::map< const char*, StoredProcedureTraceInfo > TraceMap;

	bool				m_isTraceOn;
	TraceMap			m_traceInfos;

	int					m_refreshPeriod;
	DWORD				m_lastRefreshTick;

	__int64				m_freq;
	volatile int		m_maxQueryCount;

	XCriticalSection	m_lock;

};

extern DBPerformanceTracker	g_DBTracker;


class	DBPerformanceTrackHelper
{
public:

	DBPerformanceTrackHelper()
	{
		m_createTime.QuadPart = 0;
		m_queryTime.QuadPart = 0;
		m_endTime.QuadPart = 0;

		if( g_DBTracker.isTraceOn() == true )
		{
			::QueryPerformanceCounter( &m_createTime );
			m_queryTime = m_createTime;
			m_endTime = m_createTime;
		}
	}
	~DBPerformanceTrackHelper()
	{
	}

	void	start()
	{
		if( g_DBTracker.isTraceOn() == true )
		{
			::QueryPerformanceCounter( &m_queryTime );
		}
	}

	void	end( const char* className )
	{
		end( S_OK, className, m_createTime );
	}
	void	end( HRESULT result, const char* className )
	{
		end( result, className, m_createTime );
	}
	void	end( const char* className, LARGE_INTEGER creationTime )
	{
		end( S_OK, className, m_createTime );
	}
	void	end( HRESULT result, const char* className, LARGE_INTEGER creationTime )
	{
		if( g_DBTracker.isTraceOn() == true )
		{
			::QueryPerformanceCounter( &m_endTime );
			g_DBTracker.add( result, className, m_queryTime.QuadPart - creationTime.QuadPart, m_endTime.QuadPart - m_queryTime.QuadPart );
		}
	}

private:

	LARGE_INTEGER	m_createTime;
	LARGE_INTEGER	m_queryTime;
	LARGE_INTEGER	m_endTime;

};

