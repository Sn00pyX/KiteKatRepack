#pragma once

#include "LuaVM.h"

int SCRIPT_ForceStartQuest( struct lua_State *L );
int SCRIPT_StartQuest( struct lua_State *L );
int SCRIPT_AddPendingQuest( struct lua_State *L );
int SCRIPT_EndQuest( struct lua_State *L );
int SCRIPT_DropQuest( struct lua_State *L );
int SCRIPT_QuestInfo( struct lua_State *L );
int SCRIPT_ShowQuestInfoWithoutNPC( struct lua_State *L );
int SCRIPT_GetQuestProgress( struct lua_State *L );
int SCRIPT_GetQuestStatus( struct lua_State *L );
int SCRIPT_SetQuestStatus( struct lua_State *L );
int SCRIPT_GetLastAcceptQuest( struct lua_State *L );
int SCRIPT_ResetFinishedQuest( struct lua_State *L );