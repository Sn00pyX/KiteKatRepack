#pragma once

#include <toolkit/ILock.h>
#include <mmo/ArSchedulerObject.h>

#include "GameDBManager.h"
#include "ScheduledCommandManager.h"


struct ScheduledCommandManager : ArSchedulerObject
{
public:
	enum _SCHEDULED_COMMAND_TYPE
	{
		SCT_LUA_SCRIPT		= 0,
		SCT_NOTICE			= 1,
	};

	typedef struct _SCHEDULED_COMMAND_INFO
	{
		_SCHEDULED_COMMAND_INFO()
			: nSID ( 0 )
			, eType( SCT_LUA_SCRIPT )
			, tBeginTime( 0 )
			, tEndTime( 0 )
			, tRepeatInterval( 0 )
			, bFinished( false )
			, tLastLaunchedTime( 0 )
			, tNextLaunchTime( 0 )
		{
			memset( szCommand, 0, sizeof( szCommand ) );
		}

		int							nSID;
		_SCHEDULED_COMMAND_TYPE		eType;
		time_t						tBeginTime;
		time_t						tEndTime;
		time_t						tRepeatInterval;
		wchar_t						szCommand[256];
		bool						bFinished;
		time_t						tLastLaunchedTime;
		time_t						tNextLaunchTime;
	} SCHEDULED_COMMAND_INFO;

	bool Init();
	bool DeInit();

	bool RegisterScheduledCommand( const int nSID, const _SCHEDULED_COMMAND_TYPE & eType, const time_t & tBeginTime, const time_t & tEndTime, const time_t & tRepeatInterval, const wchar_t * pszCommand );
	void ClearScheduledCommandList();

	static ScheduledCommandManager & Instance();

	virtual void onProcess( int nThreadIdx );

	void Push( GameDBManager::DBProc* pWork );
	void onEndQuery();

private:
	ScheduledCommandManager()
		: m_ScheduledCommandListLock( "ScheduledCommandManager::m_ScheduledCommandListLock" )
		, m_QueryLock( "ScheduledCommandManager::m_QueryLock" )
	{}

	XCriticalSection							m_ScheduledCommandListLock;
	std::vector< SCHEDULED_COMMAND_INFO >		m_vScheduledCommandList;

	XCriticalSection							m_QueryLock;
	std::list< GameDBManager::DBProc* >			m_lQueryList;
};
