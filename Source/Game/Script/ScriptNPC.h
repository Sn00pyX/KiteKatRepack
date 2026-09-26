#pragma once

#include "LuaVM.h"

int SCRIPT_AddMonster( struct lua_State *L );
int SCRIPT_AddMonster2(struct lua_State* L);
int SCRIPT_SetHome(struct lua_State* L);
int SCRIPT_AddInstanceDungeonMonster( struct lua_State *L );
int SCRIPT_AddNPC( struct lua_State *L );
int SCRIPT_ShowNPC(struct lua_State* L);

int SCRIPT_AddFieldProp( struct lua_State *L );
int SCRIPT_RemoveFieldProp(struct lua_State* L);
int SCRIPT_AddDungeonGuardianRespawnInfo( struct lua_State *L );
int SCRIPT_AddDungeonEnvironmentalGuardianRespawnInfo( struct lua_State *L );

int SCRIPT_AddRespawnInfo( struct lua_State *L );
int SCRIPT_AddRareMobRespawnInfo( struct lua_State *L );
int SCRIPT_AddRoamingMobRespawnInfo( struct lua_State *L );
int SCRIPT_AddRaidRespawnInfo( struct lua_State *L );
int SCRIPT_AddRaidRareMobRespawnInfo( struct lua_State *L );

int SCRIPT_SetRandomRespawnInfo( struct lua_State *L );
int SCRIPT_AddRandomAreaInfo( struct lua_State *L );
int SCRIPT_AddRandomMonster( struct lua_State *L );

int SCRIPT_AddRandomMonsterGroup( struct lua_State *L );

int SCRIPT_DialogTitle( struct lua_State *L );
int SCRIPT_DialogText( struct lua_State *L );
int SCRIPT_DialogTextWithoutQuest( struct lua_State *L );
int SCRIPT_QuestTextWithoutQuest( struct lua_State *L );
int SCRIPT_DialogMenu( struct lua_State *L );
int SCRIPT_DialogShow( struct lua_State *L );
int SCRIPT_ShowMarket( struct lua_State *L );
int SCRIPT_ShowStorage( struct lua_State *L );
int SCRIPT_ShowCreatureDialog( struct lua_State *L );
int SCRIPT_GetString( struct lua_State *L );
int SCRIPT_ResurrectSummon( struct lua_State *L );
int SCRIPT_QuestDlg( struct lua_State *L );
int	SCRIPT_GetNPCId( struct lua_State *L );
int	SCRIPT_GetNPCName(struct lua_State* L);
int	SCRIPT_GetNPCType(struct lua_State* L);
int SCRIPT_GetNPCHandle( struct lua_State *L );

#ifndef _USE_NEW_ROAMING_ONLY
int SCRIPT_AddWayPoint( struct lua_State *L );
int SCRIPT_SetWayPointType( struct lua_State *L );
int SCRIPT_SetWayPointSpeed( struct lua_State *L );
#endif

int SCRIPT_GetMonsterID( struct lua_State *L );
int SCRIPT_RespawnNearMonster( struct lua_State *L );

int SCRIPT_MonsterSkillCast( struct lua_State *L );
int SCRIPT_GetMonsterSkillTargetPosition( struct lua_State *L );
int SCRIPT_SetMonsterSkillTargetPosition( struct lua_State *L );
int SCRIPT_SetNextAttackableTime( struct lua_State *L );
int SCRIPT_FindNPC( struct lua_State *L );
int SCRIPT_ForceMonsterProcDead( struct lua_State *L );

int SCRIPT_ShowSoulStoneCraftWindow( struct lua_State *L );
int SCRIPT_ShowSoulStoneRepairWindow( struct lua_State *L );
