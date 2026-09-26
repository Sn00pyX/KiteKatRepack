#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>


// Usage: XSetThreadName (-1, "MainThread"); 
// 

namespace XThread
{
	struct ThreadInfo
	{
		char thread_name[64];
		char job_info[256];
		int  job_id;
		int  last_execute_time;
		int  counter;
	};

	void		SetThreadName( __int32 dwThreadID, const char* szThreadName);
	inline void	SetThreadName( const char* szThreadName) { SetThreadName( -1, szThreadName ); }
	const char* GetThreadName();
	void		SetThreadInfo( const char *szJobInfo, int job_id = 0 );
	ThreadInfo*	GetThreadInfo();
	int			GetThreadCounter();
};

inline void XSetThreadName( __int32 dwThreadID, const char* szThreadName) { XThread::SetThreadName( dwThreadID, szThreadName ); }
inline const char* XGetThreadName() { return XThread::GetThreadName(); }


namespace XProcess
{
	DWORD			GetProcessCount();

	std::wstring	GetParentProcessFullPath();
	std::string		GetProcessFileName( HANDLE hProcess );

};

