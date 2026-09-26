
#pragma once


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <vector>

#include "ThreadUsage.h"
#include "ILock.h"


class	XThreadMonitor
{
public:

	static void			AddWatchingThread( DWORD dwThreadID, std::string & strThreadName );
	static void			DeleteWatchingThread( DWORD dwThreadID );
	static void			UpdateThreadCPU();

	static XCriticalSection				xWatchingThreadListCS;
	static std::vector< ThreadUsage >	vWatchingThreadList;
};

