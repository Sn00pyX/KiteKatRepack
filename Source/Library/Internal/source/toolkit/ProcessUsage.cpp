
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>

#include "../../include/toolkit/ProcessUsage.h"
#include "../../include/toolkit/XThread.h"
#include "../../include/toolkit/SafeTickCount.h"

#pragma comment( lib, "psapi.lib" )


namespace
{
	ULONGLONG	ConvertFiletimeToInteger( const FILETIME* pFileTime )
	{
		if( pFileTime == NULL )
		{
			return 0;
		}

		return (static_cast< ULONGLONG >( pFileTime->dwHighDateTime ) << 32) + pFileTime->dwLowDateTime;
	}
}


void	ProcessUsage::GetMachineMemoryStatus( T_MACHINE_MEMORY_USAGE_INFO* pMemoryStatus )
{
	if( pMemoryStatus != NULL )
	{
		MEMORYSTATUSEX tStatus;
		::memset( &tStatus, 0, sizeof( tStatus ) );
		tStatus.dwLength = sizeof( tStatus );
		if( ::GlobalMemoryStatusEx( &tStatus ) != FALSE )
		{
			pMemoryStatus->dwMemoryLoad = tStatus.dwMemoryLoad;
			pMemoryStatus->ullTotalPhys = tStatus.ullTotalPhys;
			pMemoryStatus->ullAvailPhys = tStatus.ullAvailPhys;
			pMemoryStatus->ullTotalPageFile = tStatus.ullTotalPageFile;
			pMemoryStatus->ullAvailPageFile = tStatus.ullAvailPageFile;
			pMemoryStatus->ullTotalVirtual = tStatus.ullTotalVirtual;
			pMemoryStatus->ullAvailVirtual = tStatus.ullAvailVirtual;
			pMemoryStatus->ullAvailExtendedVirtual = tStatus.ullAvailExtendedVirtual;
		}
	}
}


ProcessUsage::ProcessUsage()
	: m_hHandle( NULL )
	, m_dwLastCheckTick( 0 )
	, m_u64LastKernelTime( 0 )
	, m_u64LastUserTime( 0 )
{
}

ProcessUsage::~ProcessUsage()
{
	Close();
}

bool	ProcessUsage::Open( DWORD dwProcessID )
{
	m_hHandle = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID );
	if( m_hHandle == NULL )
	{
		return false;
	}

	return true;
}

void	ProcessUsage::Close()
{
	if( m_hHandle != NULL )
	{
		::CloseHandle( m_hHandle );
		m_hHandle = NULL;
	}
}

ProcessUsage::T_CPU_USAGE_INFO	ProcessUsage::GetCPU()
{
	T_CPU_USAGE_INFO tUsage;
	if( m_hHandle != NULL )
	{
		FILETIME tCreationTime = { 0, };
		FILETIME tExitTime = { 0, };
		FILETIME tKernelTime = { 0, };
		FILETIME tUserTime = { 0, };
		DWORD dwCurrentTick = GetSafeTickCount();

		if( ::GetProcessTimes( m_hHandle, &tCreationTime, &tExitTime, &tKernelTime, &tUserTime ) != FALSE )
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

				tUsage.dTotal = double( u64Total ) / (100. * (XProcess::GetProcessCount() * (dwCurrentTick - m_dwLastCheckTick )));
				if( u64Total > 0 )
				{
					tUsage.dUser = (double( u64User ) / u64Total) * 100.;
					tUsage.dKernel = (double( u64Kernel ) / u64Total) * 100.;
				}

				m_dwLastCheckTick = dwCurrentTick;
				m_u64LastKernelTime = u64CurrentKernel;
				m_u64LastUserTime = u64CurrentUser;
			}
		}
	}

	return tUsage;
}

ProcessUsage::T_MEMORY_USAGE_INFO	ProcessUsage::GetMemory()
{
	T_MEMORY_USAGE_INFO tUsage;
	if( m_hHandle != NULL )
	{
#if _WIN32_WINNT >= 0x0501
		PROCESS_MEMORY_COUNTERS_EX tProcessMemory;
#else
		PROCESS_MEMORY_COUNTERS tProcessMemory;
#endif
		::memset( &tProcessMemory, 0, sizeof( tProcessMemory ) );
		tProcessMemory.cb = sizeof( tProcessMemory );

		if( ::GetProcessMemoryInfo( m_hHandle,
			reinterpret_cast< PROCESS_MEMORY_COUNTERS* >( &tProcessMemory ),
			sizeof( tProcessMemory ) ) != FALSE )
		{
			tUsage.PageFaultCount = tProcessMemory.PageFaultCount;
			tUsage.PeakWorkingSetSize = tProcessMemory.PeakWorkingSetSize;
			tUsage.WorkingSetSize = tProcessMemory.WorkingSetSize;
			tUsage.PeakPagedPoolUsage = tProcessMemory.QuotaPeakPagedPoolUsage;
			tUsage.PagedPoolUsage = tProcessMemory.QuotaPagedPoolUsage;
			tUsage.PeakNonPagedPoolUsage = tProcessMemory.QuotaPeakNonPagedPoolUsage;
			tUsage.NonPagedPoolUsage = tProcessMemory.QuotaNonPagedPoolUsage;
			tUsage.PeakPrivateUsage = tProcessMemory.PeakPagefileUsage;
#if _WIN32_WINNT >= 0x0501
			tUsage.PrivateUsage = tProcessMemory.PrivateUsage;
#else
			tUsage.PrivateUsage = tProcessMemory.PagefileUsage;
#endif
		}
	}

	return tUsage;
}
