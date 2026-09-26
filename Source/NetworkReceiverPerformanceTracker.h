#pragma once

class NetworkReceiverPerformanceHelper
{
public:
	NetworkReceiverPerformanceHelper( unsigned short _messageID );
	~NetworkReceiverPerformanceHelper();

private:
	unsigned short m_messageID;
	LARGE_INTEGER	m_startTime;
	LARGE_INTEGER	m_endTime;
};

struct PerformanceUnit;

class NetworkRecevierPerformanceTracker
{
public:
	NetworkRecevierPerformanceTracker()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency( &li );
		m_freq = li.QuadPart;
	}

	__int64 m_freq;

	std::map< unsigned int, PerformanceUnit > m_data;
};

extern NetworkRecevierPerformanceTracker g_NetworkReceiverPerformanceTracker;


struct PerformanceUnit
{
	PerformanceUnit()
	{
		m_count = 0;
		m_totalTicks = 0;
		m_totalTime = 0;
	}

	void addUpThisResult( const __int64 _ticks )
	{
		InterlockedIncrement( &m_count );
		InterlockedExchangeAdd( &m_totalTicks, _ticks );
		InterlockedExchange( &m_totalTime, ( m_totalTicks * 1000 ) / g_NetworkReceiverPerformanceTracker.m_freq );
	}

	volatile unsigned int m_count;
	volatile unsigned __int64 m_totalTicks;
	volatile unsigned int m_totalTime;
};