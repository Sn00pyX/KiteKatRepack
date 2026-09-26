#pragma once

class	SchedulingPerformanceHelper
{
public:
	SchedulingPerformanceHelper()
	{
		m_startTime.QuadPart = 0;
		m_endTime.QuadPart = 0;
	}
	void start();
	void end();
	__int64 getElapsedTicks() const;

private:
	LARGE_INTEGER	m_startTime;
	LARGE_INTEGER	m_endTime;
};

class	SchedulingPerformanceTracker
{
public:
	SchedulingPerformanceTracker()
	{
		m_count = 0;
		m_totalTicks = 0;
		m_totalTime = 0;

		LARGE_INTEGER li;
		QueryPerformanceFrequency( &li );
		m_freq = li.QuadPart;
	}
	
	void addUpThisResult( const SchedulingPerformanceHelper& _helper );

	__int64 m_freq;
	volatile unsigned int m_count;
	volatile unsigned __int64 m_totalTicks;
	volatile unsigned int m_totalTime;
};

extern SchedulingPerformanceTracker g_MonsterPerformanceTracker;
extern SchedulingPerformanceTracker g_PlayerPerformanceTracker;
extern SchedulingPerformanceTracker g_SummonPerformanceTracker;