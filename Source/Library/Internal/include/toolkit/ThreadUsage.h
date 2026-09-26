
#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>


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

class	ThreadUsage
{
public:
	// THREAD 사용량이 이 수치를 넘으면 기록. (Update시)
	static const DWORD BOUNCE_THRESHOLD = 20;

	// 메모리는 기록 안 할거 우선 쓰레드는 CPU 측정만 하자
	/* 
	struct	T_MACHINE_MEMORY_USAGE_INFO
	{
		T_MACHINE_MEMORY_USAGE_INFO()
			: dwMemoryLoad( 0 )
			, ullTotalPhys( 0 )
			, ullAvailPhys( 0 )
			, ullTotalPageFile( 0 )
			, ullAvailPageFile( 0 )
			, ullTotalVirtual( 0 )
			, ullAvailVirtual( 0 )
			, ullAvailExtendedVirtual( 0 )
		{
		}

		DWORD		dwMemoryLoad;				// 머신의 메모리 사용 퍼센트
		DWORDLONG	ullTotalPhys;				// 물리메모리 총 사이즈(Bytes)
		DWORDLONG	ullAvailPhys;				// 물리메모리 가용 사이즈(Bytes)
		DWORDLONG	ullTotalPageFile;			// 페이지파일 총 사이즈(Bytes)
		DWORDLONG	ullAvailPageFile;			// 페이지파일 가용 사이즈(Bytes)
		DWORDLONG	ullTotalVirtual;			// 현재 프로세스 가상메모리 유저모드 총 사이즈(Bytes)
		DWORDLONG	ullAvailVirtual;			// 현재 프로세스 가상메모리 유저모드 가용 사이즈(Bytes)
		DWORDLONG	ullAvailExtendedVirtual;	// 확장 메모리 가용 사이즈(Bytes)
	};
	*/

	struct	T_THREAD_USAGE_INFO
	{
		T_THREAD_USAGE_INFO()
			: dTotal( 0 )
			, dUser( 0 )
			, dKernel( 0 )
		{
		}

		double	dTotal;
		double	dUser;
		double	dKernel;
	};

	/*
	struct	T_MEMORY_USAGE_INFO
		{
			T_MEMORY_USAGE_INFO()
				: PageFaultCount( 0 )
				, PeakWorkingSetSize( 0 )
				, WorkingSetSize( 0 )
				, PeakPagedPoolUsage( 0 )
				, PagedPoolUsage( 0 )
				, PeakNonPagedPoolUsage( 0 )
				, NonPagedPoolUsage( 0 )
				, PeakPrivateUsage( 0 )
				, PrivateUsage( 0 )
			{
			}
	
			DWORD	PageFaultCount;				// 페이지 폴트 횟수
			SIZE_T	PeakWorkingSetSize;			// 애플리케이션이 시작된후 가장 많은 램을 사용한 순간의 크기(Bytes)
			SIZE_T	WorkingSetSize;				// GetProcessMemoryInfo함수가 호출된 순간의 램 사용 크기(Bytes)
			SIZE_T	PeakPagedPoolUsage;			// (Bytes)
			SIZE_T	PagedPoolUsage;				// (Bytes)
			SIZE_T	PeakNonPagedPoolUsage;		// (Bytes)
			SIZE_T	NonPagedPoolUsage;			// (Bytes)
			SIZE_T	PeakPrivateUsage;			// (Bytes)
			SIZE_T	PrivateUsage;				// 애플리케이션에서 명시적으로 할당한 메모리의 크기(Bytes)
		};*/
	


	//static void	GetMachineMemoryStatus( T_MACHINE_MEMORY_USAGE_INFO* pMemoryStatus );
	
	ThreadUsage( DWORD _dwThreadID, std::string _strName );
	~ThreadUsage();

	bool	Open();
	void	Close();

	void UpdateCPU();
	//T_MEMORY_USAGE_INFO	GetMemory();

	const DWORD GetThreadID() const
	{
		return m_dwThreadID;
	}

	const char* GetThreadName() const
	{
		return m_strThreadName.c_str();
	}

	const T_THREAD_USAGE_INFO* GetCPUInfo() const
	{
		return &m_CPUInfo;
	}

	const DWORD GetBouncingCount() const
	{
		return m_dwBouncingCount;
	}

private:

	HANDLE		m_hHandle;

	DWORD		m_dwLastCheckTick;
	ULONGLONG	m_u64LastKernelTime;
	ULONGLONG	m_u64LastUserTime;

	std::string m_strThreadName;
	DWORD		m_dwThreadID;
	DWORD		m_dwBouncingCount;
	T_THREAD_USAGE_INFO m_CPUInfo;
};