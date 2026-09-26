
#include "../../include/toolkit/XThread.h"
#include "../../include/toolkit/safe_function.h"
#include "../../include/toolkit/SafeTickCount.h"

#include <Tlhelp32.h>
#include <Psapi.h>

#pragma comment( lib, "psapi.lib" )


namespace XThread
{

	__declspec( thread ) ThreadInfo g_ThreadInfo;

	void		SetThreadInfo( const char *szJobName, int job_id )
	{
		g_ThreadInfo.last_execute_time = GetSafeTickCount();
		s_strcpy( g_ThreadInfo.job_info, _countof( g_ThreadInfo.job_info ), szJobName );
		++g_ThreadInfo.counter;
		g_ThreadInfo.job_id = job_id;
	}

	ThreadInfo* GetThreadInfo()
	{
		return &g_ThreadInfo;
	}

	int GetThreadCounter()
	{
		return g_ThreadInfo.counter;
	}

	void SetThreadName( __int32 dwThreadID, const char* szThreadName) 
	{ 
		struct tagTHREADNAME_INFO 
		{ 
			DWORD dwType;  // must be 0x1000 
			LPCSTR szName; // pointer to name (in user addr space) 
			DWORD dwThreadID; // thread ID (-1=caller thread) 
			DWORD dwFlags; // reserved for future use, must be zero 

		} info; 

		info.dwType = 0x1000; 
		info.szName = szThreadName; 
		info.dwThreadID = dwThreadID; 
		info.dwFlags = 0; 

		s_strcpy( g_ThreadInfo.thread_name, _countof( g_ThreadInfo.thread_name ), szThreadName );

		__try 
		{ 
			RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info ); 
		}
		__except (EXCEPTION_CONTINUE_EXECUTION) 
		{ 
		} 

	}

	const char* GetThreadName()
	{
		return g_ThreadInfo.thread_name;
	}

};

namespace XProcess
{
	bool	IsWow64()
	{
		typedef BOOL (WINAPI *PIS_WOW64_PROCESS)( HANDLE, PBOOL );

		BOOL bResult = FALSE;
		PIS_WOW64_PROCESS fpIsWow64Process = reinterpret_cast< PIS_WOW64_PROCESS >( ::GetProcAddress( ::GetModuleHandleW( L"kernel32" ), "IsWow64Process" ) );
		if( fpIsWow64Process != NULL )
		{
			if( fpIsWow64Process( ::GetCurrentProcess(), &bResult ) == FALSE )
			{
				bResult = FALSE;
			}
		}

		return (bResult != FALSE);
	}

	void	GetSystemInfoForWow64( SYSTEM_INFO* pInfo )
	{
		if( pInfo == NULL )
		{
			return;
		}

		::memset( pInfo, 0, sizeof( *pInfo ) );

		if( IsWow64() == false )
		{
			::GetSystemInfo( pInfo );
		}
		else
		{
#if _WIN32_WINNT >= 0x0501
			::GetNativeSystemInfo( pInfo );
#else
			::GetSystemInfo( pInfo );
#endif
		}
	}

	DWORD	GetProcessCount()
	{
		static DWORD dwProcessCount = 0;
		if( dwProcessCount == 0 )
		{
			SYSTEM_INFO tInfo;
			GetSystemInfoForWow64( &tInfo );
			dwProcessCount = tInfo.dwNumberOfProcessors;
		}

		return dwProcessCount;
	}


	std::wstring	GetParentProcessFullPath()
	{
		DWORD dwMyID = GetCurrentProcessId();
		DWORD dwParentID = 0;

		HANDLE hSnapshot = ::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		if( hSnapshot != INVALID_HANDLE_VALUE )
		{
			PROCESSENTRY32 Process32;
			::memset( &Process32, 0, sizeof( Process32 ) );
			Process32.dwSize = sizeof( Process32 );

			if( ::Process32First( hSnapshot, &Process32 ) != FALSE )
			{
				do
				{
					if( dwMyID == Process32.th32ProcessID )
					{
						dwParentID = Process32.th32ParentProcessID;
						break;
					}
				} while( ::Process32Next( hSnapshot, &Process32 ) != FALSE );
			}

			::CloseHandle( hSnapshot );
		}

		if( dwParentID != 0 )
		{
			HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwParentID );
			if( hProcess != NULL )
			{
				wchar_t szFullPath[MAX_PATH] = { 0, };
				::GetModuleFileNameExW( hProcess, NULL, szFullPath, _countof( szFullPath ) );
				::CloseHandle( hProcess );
				return szFullPath;
			}
		}

		return L"";
	}

	std::string	GetProcessFileName( HANDLE hProcess )
	{
		char szPath[MAX_PATH] = { 0, };
		if( ::GetModuleFileNameExA( hProcess, NULL, szPath, _countof( szPath ) ) == 0 )
		{
			return "";
		}

		char szFile[_MAX_FNAME] = { 0, };
		char szExt[_MAX_EXT] = { 0, };

		::_splitpath_s( szPath, NULL, 0, NULL, 0, szFile, _countof( szFile ), szExt, _countof( szExt ) );

		char szOutput[MAX_PATH] = { 0, };
		::_makepath_s( szOutput, _countof( szOutput ), NULL, NULL, szFile, szExt );

		return szOutput;
	}

};
