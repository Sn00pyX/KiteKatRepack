
#include "../../include/toolkit/XThreadMonitor.h"


std::vector< ThreadUsage >	XThreadMonitor::vWatchingThreadList;
XCriticalSection			XThreadMonitor::xWatchingThreadListCS;

void XThreadMonitor::AddWatchingThread( DWORD dwThreadID, std::string & strThreadName )
{
	ThreadUsage tUsage( dwThreadID, strThreadName );

	THREAD_SYNCRONIZE( &xWatchingThreadListCS );
	vWatchingThreadList.push_back( tUsage );
}

void XThreadMonitor::DeleteWatchingThread( DWORD dwThreadID )
{
	THREAD_SYNCRONIZE( &xWatchingThreadListCS );
	for( auto it = vWatchingThreadList.begin(); it != vWatchingThreadList.end() ; ++it )
	{
		if( (*it).GetThreadID() == dwThreadID )
		{
			vWatchingThreadList.erase( it );
			break;
		}
	}
}

void XThreadMonitor::UpdateThreadCPU()
{
	THREAD_SYNCRONIZE( &xWatchingThreadListCS );
	for( auto it = vWatchingThreadList.begin(); it != vWatchingThreadList.end() ; )
	{
		if( (*it).Open() )
		{
			(*it).UpdateCPU();
			(*it).Close();
			++it;
		}
		else
		{
			it = vWatchingThreadList.erase( it );
		}
	}
}

