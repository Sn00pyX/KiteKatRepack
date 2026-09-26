#pragma once

#include "LuaVM.h"

int SCRIPT_CheckValidGuildName( struct lua_State *L );
int SCRIPT_CreateGuild( struct lua_State *L );
int SCRIPT_DestroyGuild( struct lua_State *L );
int SCRIPT_ForceChangeGuildName( struct lua_State *L );
int SCRIPT_ForcePromoteGuildLeader( struct lua_State *L );
int SCRIPT_ShowCreateGuild( struct lua_State *L );
int SCRIPT_UpdateGuildInfo( struct lua_State *L );
int SCRIPT_UpdateGuildBannerInfo( struct lua_State *L );
int SCRIPT_CreateAlliance( struct lua_State *L );
int SCRIPT_DestroyAlliance( struct lua_State *L );
int SCRIPT_CheckValidAllianceName( struct lua_State *L );
int SCRIPT_ShowCreateAlliance( struct lua_State *L );
int SCRIPT_IsGuildLeader( struct lua_State *L );
int SCRIPT_IsAllianceLeader( struct lua_State *L );
int SCRIPT_IncreaseMaxAllianceMemberCount( struct lua_State *L );
int SCRIPT_GetMaxAllianceMemberCount( struct lua_State *L );
int SCRIPT_SetGuildBlockTime( struct lua_State *L );
int SCRIPT_GetGuildBlockTime( struct lua_State *L );
int SCRIPT_GetGuildName(struct lua_State* L);
int SCRIPT_AddGuildPoint(struct lua_State* L);
int SCRIPT_GetGuildPoint(struct lua_State* L);
int SCRIPT_GuildNotice(struct lua_State* L);

