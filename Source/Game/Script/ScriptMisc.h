#pragma once

#include <mmo/ArSchedulerObject.h>

#include "LuaVM.h"

// lua 표준 라이브러리 기능과 동일
int	SCRIPT_LuaSTDLib_Time( struct lua_State *L );
int	SCRIPT_LuaSTDLib_Date( struct lua_State *L );
int	SCRIPT_GetMinute(struct lua_State* L);

int SCRIPT_Notice( struct lua_State *L );
int SCRIPT_Announce( struct lua_State *L );
int SCRIPT_Shutdown( struct lua_State *L );
int SCRIPT_DeleteFromBlockAccount( struct lua_State *L );
int SCRIPT_Whisper( struct lua_State *L );
int SCRIPT_GetEnv( struct lua_State *L );
int SCRIPT_SetEnv( struct lua_State *L );
int SCRIPT_GetGameTime( struct lua_State *L );
int SCRIPT_Reload( struct lua_State *L );
int SCRIPT_Conv( struct lua_State *L );
int SCRIPT_SetCurrentLocationId( struct lua_State *L );
int SCRIPT_OpenUrl( struct lua_State *L );
int SCRIPT_OpenPopup( struct lua_State *L );
int SCRIPT_OpenPopupAndSetSize( struct lua_State *L );
int SCRIPT_GetServerCategory( struct lua_State *L );
int SCRIPT_GetLocalInfo( struct lua_State *L );
int SCRIPT_Suicide( struct lua_State *L );
int SCRIPT_SetScheduledCommandManagerPriority( struct lua_State *L );

int SCRIPT_SetGlobalVariable( struct lua_State *L );
int SCRIPT_GetGlobalVariable( struct lua_State *L );
int SCRIPT_DeleteGlobalVariable( struct lua_State *L );

int SCRIPT_GetInstanceDungeonTypeID( struct lua_State *L );
int SCRIPT_SetInstanceDungeonFlag( struct lua_State *L );
int SCRIPT_GetInstanceDungeonFlag( struct lua_State *L );
int SCRIPT_GetAliveInstanceRespawnGroupMonsterCount( struct lua_State *L );
int SCRIPT_DoEachPlayerInInstanceDungeon( struct lua_State *L );
int SCRIPT_BroadcastMissionTitle( struct lua_State *L );
int SCRIPT_BroadcastMissionReward( struct lua_State *L );
int SCRIPT_BroadcastMissionObjective( struct lua_State *L );
int SCRIPT_BroadcastMissionObjectiveProgress( struct lua_State *L );
int SCRIPT_BroadcastNotice( struct lua_State *L );

int SCRIPT_ActivateBattleArenaProp( struct lua_State *L );
int SCRIPT_SetBattleArenaInstanceFlag( struct lua_State *L );
int SCRIPT_GetBattleArenaInstanceFlag( struct lua_State *L );
int SCRIPT_DoEachPlayerInBattleArenaInstance( struct lua_State *L );
int SCRIPT_DoEachPlayerInBattleArenaTeam( struct lua_State *L );

int SCRIPT_DBTraceDump( struct lua_State *L );


struct ShutdownManager : public ArSchedulerObject
{
protected:
	ShutdownManager();

public:
	static ShutdownManager & Instance()
	{
		static ShutdownManager _inst;
		return _inst;
	}

	void Shutdown( int nLastSec, bool bNeedToCreateDump );

	virtual void				onProcess( int nThreadIdx );

	enum _SHUTDOWN_STATE
	{
		SDS_ON_SERVICE			= 0,
		SDS_BEFORE_NOTICE		= 1 << 0,
		SDS_AFTER_NOTICE_60SEC	= 1 << 1,
		SDS_AFTER_NOTICE_30SEC	= 1 << 2,
		SDS_NOTICE_COMPLETE		= 1 << 3,
		SDS_WRAPUP_COMPLETE		= 1 << 4
	};

	AR_TIME			m_nShutdownTime;
	int				m_nShutdownState;
	int				m_nCountdown;
	bool			m_bNeedToCreateDump;
};
