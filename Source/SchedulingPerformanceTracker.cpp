#pragma once

#include "SchedulingPerformanceTracker.h"
#include <WinBase.h>

SchedulingPerformanceTracker g_MonsterPerformanceTracker;
SchedulingPerformanceTracker g_PlayerPerformanceTracker;
SchedulingPerformanceTracker g_SummonPerformanceTracker;

void SchedulingPerformanceHelper::start()
{
	::QueryPerformanceCounter( &m_startTime );
}

void SchedulingPerformanceHelper::end()
{
	::QueryPerformanceCounter( &m_endTime );
}

__int64 SchedulingPerformanceHelper::getElapsedTicks() const
{
	LARGE_INTEGER li;
	li.QuadPart = m_endTime.QuadPart - m_startTime.QuadPart;

	return li.QuadPart;
}

void SchedulingPerformanceTracker::addUpThisResult( const SchedulingPerformanceHelper& _helper )
{
	InterlockedIncrement( &m_count );
	InterlockedExchangeAdd( &m_totalTicks, _helper.getElapsedTicks() );
	InterlockedExchange( &m_totalTime, ( m_totalTicks * 1000 ) / m_freq );
}