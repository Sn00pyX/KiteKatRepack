// Time.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <cstring>
#include <cassert>
#include <ctime>

#include <Windows.h>

#include <boost/thread.hpp>

#include "../../include/toolkit/XRWLock.h"



rwlock g_rwlock;
XRWLock<> g_xrwlock;

volatile LONG g_read_count = 0;
volatile LONG g_write_count = 0;

void thread_func()
{
	DWORD start_time = ::GetTickCount();
	while( true )
	{
		bool read_lock = ((rand()%2) == 1);
		if( read_lock )
		{
			assert( g_rwlock.rdlock() == true );
			//printf( "read lock\n" );
			if( InterlockedIncrement( &g_read_count ) < 0 )
			{
				printf( "read count over\n" );
			}
		}
		else
		{
			assert( g_rwlock.wrlock() == true );
			//printf( "write lock\n" );
			if( InterlockedIncrement( &g_write_count ) < 0 )
			{
				printf( "write count over\n" );
			}
		}

		assert( g_rwlock.unlock() == true );

		DWORD now = ::GetTickCount();
		if( (now-start_time) >= 10*1000 )
		{
			break;
		}
	}
}


void thread_func2()
{
	DWORD start_time = ::GetTickCount();
	while( true )
	{
		bool read_lock = ((rand()%2) == 1);
		if( read_lock )
		{
			g_xrwlock.WaitToRead();
			//printf( "read lock\n" );
			if( InterlockedIncrement( &g_read_count ) < 0 )
			{
				printf( "read count over\n" );
			}
		}
		else
		{
			g_xrwlock.WaitToWrite();
			//printf( "write lock\n" );
			if( InterlockedIncrement( &g_write_count ) < 0 )
			{
				printf( "write count over\n" );
			}
		}

		g_xrwlock.Done();

		DWORD now = ::GetTickCount();
		if( (now-start_time) >= 10*1000 )
		{
			break;
		}
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	

	{
		g_read_count = 0;
		g_write_count = 0;

		boost::thread_group tg;

		for( int i = 0; i < 10; ++i )
		{
			tg.create_thread( thread_func2 );
		}

		tg.join_all();

		printf( "rd count: %d\n", g_read_count );
		printf( "wr count: %d\n", g_write_count );
	}

	{
		g_read_count = 0;
		g_write_count = 0;

		boost::thread_group tg;

		for( int i = 0; i < 10; ++i )
		{
			tg.create_thread( thread_func );
		}

		tg.join_all();

		printf( "rd count: %d\n", g_read_count );
		printf( "wr count: %d\n", g_write_count );
	}


	return 0;
}

