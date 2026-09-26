#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>

#include "../../include/toolkit/ThreadUsage.h"
#include "../../include/toolkit/XThread.h"
#include "../../include/toolkit/SafeTickCount.h"

#pragma comment( lib, "psapi.lib" )

ThreadUsage::~ThreadUsage()
{
	Close();
}

ThreadUsage::ThreadUsage( DWORD _dwThreadID, std::string _strName )
	: m_hHandle( NULL )
	, m_dwLastCheckTick( 0 )
	, m_u64LastKernelTime( 0 )
	, m_u64LastUserTime( 0 )
	, m_dwBouncingCount( 0 )
	, m_dwThreadID( _dwThreadID )
	, m_strThreadName( _strName )
{
}

bool	ThreadUsage::Open()
{
	m_hHandle = ::OpenThread(THREAD_QUERY_INFORMATION, FALSE, m_dwThreadID );
	if( m_hHandle == NULL )
	{
		return false;
	}

	return true;
}

void	ThreadUsage::Close()
{
	if( m_hHandle != NULL )
	{
		::CloseHandle( m_hHandle );
		m_hHandle = NULL;
	}
}

void ThreadUsage::UpdateCPU()
{
	if( !m_hHandle )
	{
		return;
	}

	FILETIME tCreationTime = { 0, };
	FILETIME tExitTime = { 0, };
	FILETIME tKernelTime = { 0, };
	FILETIME tUserTime = { 0, };
	DWORD dwCurrentTick = GetSafeTickCount();

	if( ::GetThreadTimes( m_hHandle, &tCreationTime, &tExitTime, &tKernelTime, &tUserTime ) != FALSE )
	{
		ULONGLONG u64CurrentKernel = ConvertFiletimeToInteger( &tKernelTime );
		ULONGLONG u64CurrentUser = ConvertFiletimeToInteger( &tUserTime );
		if( m_dwLastCheckTick == 0 )
		{
			m_dwLastCheckTick = dwCurrentTick;
			m_u64LastKernelTime = u64CurrentKernel;
			m_u64LastUserTime = u64CurrentUser;
		}
		else
		{
			ULONGLONG u64User = u64CurrentUser - m_u64LastUserTime;
			ULONGLONG u64Kernel = u64CurrentKernel - m_u64LastKernelTime;
			ULONGLONG u64Total = u64User + u64Kernel;

			// 나누는 부분 나중에 수정할게~. 아니 안해도 되나?
			m_CPUInfo.dTotal = double( u64Total ) / (100. * (XProcess::GetProcessCount() * (dwCurrentTick - m_dwLastCheckTick )));
			if( u64Total == 0 )
			{
				m_CPUInfo.dUser = 0;
				m_CPUInfo.dKernel = 0;
			}
			else
			{
				m_CPUInfo.dUser = (double( u64User ) / u64Total) * 100.;
				m_CPUInfo.dKernel = (double( u64Kernel ) / u64Total) * 100.;
			}

			if( m_CPUInfo.dTotal > BOUNCE_THRESHOLD )
				m_dwBouncingCount++;

			m_dwLastCheckTick = dwCurrentTick;
			m_u64LastKernelTime = u64CurrentKernel;
			m_u64LastUserTime = u64CurrentUser;
		}
	}
}