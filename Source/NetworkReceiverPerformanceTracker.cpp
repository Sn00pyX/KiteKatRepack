#pragma once

#include "NetworkReceiverPerformanceTracker.h"

NetworkReceiverPerformanceHelper::NetworkReceiverPerformanceHelper( unsigned short _messageID )
	: m_messageID( _messageID )
{
	// 시간 측정 꼬
	::QueryPerformanceCounter( &m_startTime );
}

NetworkReceiverPerformanceHelper::~NetworkReceiverPerformanceHelper()
{
	// 시간 측정 끝
	::QueryPerformanceCounter( &m_endTime );

	auto it = g_NetworkReceiverPerformanceTracker.m_data.find( m_messageID );
	if( it == g_NetworkReceiverPerformanceTracker.m_data.end() )
	{
		PerformanceUnit unit;
		it = g_NetworkReceiverPerformanceTracker.m_data.insert( std::pair< unsigned short, PerformanceUnit >( m_messageID, unit ) ).first;
	}

	LARGE_INTEGER li;
	li.QuadPart = m_endTime.QuadPart - m_startTime.QuadPart;
	(*it).second.addUpThisResult( li.QuadPart );
}

NetworkRecevierPerformanceTracker g_NetworkReceiverPerformanceTracker;