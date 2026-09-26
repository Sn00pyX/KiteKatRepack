
#include <ctime>

#include <toolkit/XEnv.h>
#include <mmo/ArcadiaServer.h>

#include "LogClient/LogClient.h"

#include "LuaVM.h"
#include "ScheduledCommandManager.h"
#include "SendMessage.h"
#include "DB_Commands.h"


extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;

ScheduledCommandManager & ScheduledCommandManager::Instance()
{
	static ScheduledCommandManager _instance;

	return _instance;
}

bool ScheduledCommandManager::Init()
{
	ArcadiaServer::Instance().SetObjectPriority( &ScheduledCommandManager::Instance(), ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	return true;
}

bool ScheduledCommandManager::DeInit()
{
	ArcadiaServer::Instance().SetObjectPriority( &ScheduledCommandManager::Instance(), ArSchedulerObject::UPDATE_PRIORITY_IDLE );

	return true;
}

bool ScheduledCommandManager::RegisterScheduledCommand( const int nSID, const _SCHEDULED_COMMAND_TYPE & eType, const time_t & tBeginTime, const time_t & tEndTime, const time_t & tRepeatInterval, const wchar_t * pszCommand )
{
	THREAD_SYNCHRONIZE( &m_ScheduledCommandListLock );

	// 중복된 SID 를 가진 항목을 등록하려는 경우 등록 실패
	for( std::vector< SCHEDULED_COMMAND_INFO >::iterator it = m_vScheduledCommandList.begin() ; it != m_vScheduledCommandList.end() ; ++it )
	{
		if( (*it).nSID == nSID )
			return false;
	}

	// 실행할 내용이 없으면 등록 실패
	if( !wcslen( pszCommand ) )
		return false;

	// 이미 실행 기간이 지나간 항목이면 등록 실패
	time_t tCurrent = time( NULL );
	if( tCurrent > tEndTime )
	{
		Push( new DB_UpdateLaunchedScheduledCommand( nSID, 0, true ) );
		return false;
	}

	SCHEDULED_COMMAND_INFO cmdInfo;

	cmdInfo.nSID = nSID;
	cmdInfo.eType = eType;
	cmdInfo.tBeginTime = tBeginTime;
	cmdInfo.tEndTime = ( !tRepeatInterval || tBeginTime > tEndTime  || tEndTime == -1 ) ? tBeginTime : tEndTime;
	cmdInfo.tRepeatInterval = ( cmdInfo.tEndTime == tBeginTime ) ? 0 : tRepeatInterval;
	s_strcpy( cmdInfo.szCommand, _countof( cmdInfo.szCommand ), pszCommand );
	cmdInfo.bFinished = false;
	cmdInfo.tLastLaunchedTime = 0;
	if( cmdInfo.tRepeatInterval )
	{
		// 시작 시점을 지난 항목인 경우 다음 실행 시점을 계산
		if( tCurrent > tBeginTime )
		{
			// 마지막 실행 시점으로부터 지난 초 계산
			time_t tPastSecondFromLastLaunchTime = ( tCurrent - tBeginTime ) % cmdInfo.tRepeatInterval;

			// tPastSecondFromLastLaunchTime == 0 라면 현재 시점이 다음(실질적으로 이번) 실행 시점
			if( !tPastSecondFromLastLaunchTime )
				cmdInfo.tNextLaunchTime = tCurrent;
			// tPastSecondFromLastLaunchTime > 0 라면 해당 초를 실행 주기에서 뺀 초 만큼이 다음 실행 시점까지 남은 시간
			else
				cmdInfo.tNextLaunchTime = tCurrent + ( cmdInfo.tRepeatInterval - tPastSecondFromLastLaunchTime );
		}
		// 아직 시작 시점 전이라면 최초 시작 시점을 다음 실행 시점으로 설정
		else
		{
			cmdInfo.tNextLaunchTime = tBeginTime;
		}
	}
	else
	{
		cmdInfo.tNextLaunchTime = tBeginTime;
	}

	m_vScheduledCommandList.push_back( cmdInfo );

	return true;
}

void ScheduledCommandManager::ClearScheduledCommandList()
{
	THREAD_SYNCRONIZE( m_ScheduledCommandListLock );

	m_vScheduledCommandList.clear();
}

void ScheduledCommandManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "ScheduledCommandManager" );

	AR_TIME t = GetArTime();

	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "ScheduledCommandManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	THREAD_SYNCRONIZE( m_ScheduledCommandListLock );

	for( std::vector< SCHEDULED_COMMAND_INFO >::iterator it = m_vScheduledCommandList.begin() ; it != m_vScheduledCommandList.end() ; ++it )
	{
		SCHEDULED_COMMAND_INFO & sci = (*it);

		if( sci.bFinished )
			continue;

		time_t tCurrentTime = time( NULL );

		// 실행되어야 할 명령어 처리
		if( !sci.bFinished && sci.tNextLaunchTime <= tCurrentTime )
		{
			char szBuf[ sizeof( sci.szCommand ) * 4 ];
			if( WideCharToMultiByte( ENV().GetInt( "CodePage", CP_ACP ), 0, sci.szCommand, _countof( sci.szCommand ), szBuf, _countof( szBuf ), NULL, NULL ) )
			{
				switch( sci.eType )
				{
				case ScheduledCommandManager::SCT_LUA_SCRIPT:
					{
						// 실행할 때 전체 월드 락 걸고 명령어 처리
						ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

						LUA()->RunString( szBuf );
					}
					break;
				case ScheduledCommandManager::SCT_NOTICE:
					{
						SendGlobalChatMessage( CHAT_NOTICE, "@NOTICE", szBuf, static_cast< unsigned int >( strlen( szBuf ) ) );
					}
					break;
				}

				sci.tLastLaunchedTime = tCurrentTime;
				sci.tNextLaunchTime += sci.tRepeatInterval;
				sci.bFinished = ( !sci.tRepeatInterval || ( sci.tNextLaunchTime > sci.tEndTime ) );
				Push( new DB_UpdateLaunchedScheduledCommand( sci.nSID, sci.tLastLaunchedTime, sci.bFinished ) );

				struct tm currentTime;
				localtime_s( &currentTime, &tCurrentTime );

				_cprint( "Scheduled command launched : SID(%d) Scheduled time(%04d-%02d-%02d %02d:%02d:%02d) Type(%d) Command(%s)\n",
					sci.nSID, currentTime.tm_year + 1900, currentTime.tm_mon + 1, currentTime.tm_mday,
					currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec, sci.eType, szBuf );
				FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "ScheduledCommand", "Scheduled command launched : SID(%d) Scheduled time(%04d-%02d-%02d %02d:%02d:%02d) Type(%d) Command(%s)",
					sci.nSID, currentTime.tm_year + 1900, currentTime.tm_mon + 1, currentTime.tm_mday,
					currentTime.tm_hour, currentTime.tm_min, currentTime.tm_sec, sci.eType, szBuf );
			}
		}
	}

	char buff2[255] = { 0, };

	static time_t g_nLastSiegeUpdate = 0;
	static time_t g_nLastDungeonUpdate = 0;
	static bool g_bDataSetupRunOnce = false;

	time_t nCurrentTime = time(NULL);

	if (g_nLastDungeonUpdate + 60 < nCurrentTime)
	{
		// runs every minute
		GameRule::nSkyFortressStartIndex = XRandom(0,3);
		g_nLastDungeonUpdate += 60;
	}

	// run the siege ticks every 20 seconds
	if (g_nLastSiegeUpdate + 20 <= nCurrentTime) {
		if (!g_bDataSetupRunOnce)
		{
			LuaVM::Inst()->RunString("siege_start()");
			LuaVM::Inst()->RunString("skyfortress_start()");
			g_bDataSetupRunOnce = true;

#ifdef _DEBUG
			LuaVM::Inst()->RunString("eventBuffs()");

#endif
		}

		// the tick function is expecting 0-179 per hour so it basically increases by 1 every pass
		int nHour = nCurrentTime / 3600;
		int nMinute = nCurrentTime % 3600;
		int nTick = nMinute / 20;

		s_sprintf(buff2, 255, "siege_process(%d)", nTick);
		LuaVM::Inst()->RunString(buff2);
		g_nLastSiegeUpdate = nCurrentTime;
	}
}

void ScheduledCommandManager::Push( GameDBManager::DBProc* pWork )
{
	THREAD_SYNCRONIZE( m_QueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}


void ScheduledCommandManager::onEndQuery()
{
	THREAD_SYNCRONIZE( m_QueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}
