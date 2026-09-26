#pragma once

#include <time.h>


time_t GetThisMonthBeginTime()
{
	time_t t = time( NULL );

	struct tm curTime;
	localtime_s( &curTime, &t );

	curTime.tm_mday = 1;
	curTime.tm_hour = 0;
	curTime.tm_min = 0;
	curTime.tm_sec = 0;
	curTime.tm_isdst = -1;

	return mktime( &curTime );
}

time_t GetNextMonthBeginTime()
{
	time_t t = time( NULL );

	struct tm curTime;
	localtime_s( &curTime, &t );

	++curTime.tm_mon;
	curTime.tm_mday = 1;
	curTime.tm_hour = 0;
	curTime.tm_min = 0;
	curTime.tm_sec = 0;
	curTime.tm_isdst = -1;

	return mktime( &curTime );
}

time_t GetWeekBeginTime() // begin at monday
{
	time_t t = time( NULL );

	struct tm curTime;
	localtime_s( &curTime, &t );

	time_t week_begin_time = t;

	week_begin_time -= ( curTime.tm_wday - 1 ) * 3600 * 24 + curTime.tm_hour * 3600 + curTime.tm_min * 60 + curTime.tm_sec;

	if( !curTime.tm_wday ) // sunday
	{
		week_begin_time -= 7 * 3600 * 24;
	}

	return week_begin_time;
}

time_t GetDayBeginTime()
{
	time_t t = time( NULL );

	struct tm curTime;
	localtime_s( &curTime, &t );

	time_t day_begin_time = t;

	day_begin_time -= curTime.tm_hour * 3600 + curTime.tm_min * 60 + curTime.tm_sec;

	return day_begin_time;
}
