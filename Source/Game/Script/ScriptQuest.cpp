#include "ScriptQuest.h"
#include "SendMessage.h"
#include "ScriptCommon.h"
#include "StructNPC.h"
#include "GameProc.h"
#include <vector>

#include <lua/lua.hpp>


int SCRIPT_ForceStartQuest( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_ForceStartQuest() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	int nQuestCode = lua_tonumber( L, 1 );
	int nStartTextID = lua_tonumber( L, 2 );

	if( !pPlayer->StartQuest( nQuestCode, nStartTextID, true ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, 1 );

	return 1;
}

int SCRIPT_StartQuest( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_StartQuest() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	int nQuestCode = lua_tonumber( L, 1 );
	int nStartTextID = lua_tonumber( L, 2 );

	time_t tRemainQuestCoolTime = pPlayer->GetRemainQuestCoolTime( nQuestCode, time( NULL ) );
	if( tRemainQuestCoolTime )
	{
		char szBuf[255];
		s_sprintf( szBuf, _countof( szBuf ), "@595\v#@hours@#\v%d\v#@minutes@#\v%d\v#@seconds@#\v%d", tRemainQuestCoolTime / 3600, tRemainQuestCoolTime % 3600 / 60, tRemainQuestCoolTime % 60 );

		PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, szBuf );
		SendGeneralMessageBox( pPlayer, szBuf );
		lua_pushnumber( L, 0 );
		return 1;
	}

	if( !pPlayer->StartQuest( nQuestCode, nStartTextID ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, 1 );

	return 1;
}

int SCRIPT_AddPendingQuest( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_AddPendingQuest() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	// 최대 개수 제한에 걸린 경우
	if( pPlayer->GetActiveQuestCount() >= StructQuestManager::QUEST_MAX )
	{
		lua_pushnumber( L, 1 );
		return 1;
	}

	int nQuestCode = lua_tonumber( L, 1 );

	if( !pPlayer->IsStartableQuest( nQuestCode ) )
	{
		lua_pushnumber( L, 2 );
		return 1;
	}

	int nStartID = GameContent::GetQuestTextId( nQuestCode, QUEST_IS_STARTABLE );

	// NPC 없이 시작 가능한 QuestLink 데이터가 없는 퀘스트인 경우
	if( !nStartID )
	{
		lua_pushnumber( L, 3 );
		return 1;
	}

	if( !pPlayer->AddPendingQuest( nQuestCode, nStartID ) )
	{
		lua_pushnumber( L, 4 );
		return 1;
	}

	return 0;
}

int SCRIPT_EndQuest( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_EndQuest() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	int nQuestCode = lua_tonumber( L, 1 );
	int nOptionalReward = lua_tonumber( L, 2 );

	if( !pPlayer->EndQuest( nQuestCode, nOptionalReward ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, 1 );

	return 1;
}

int SCRIPT_DropQuest( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_DropQuest() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	int nQuestCode = lua_tonumber( L, 1 );

	if( !pPlayer->DropQuest( nQuestCode ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, 1 );

	return 1;
}

int SCRIPT_QuestInfo( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_QuestInfo() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, n + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	int nQuestCode = lua_tonumber( L, 1 );
	int nTextID = lua_tonumber( L, 2 );
	int nDialogType = 0;

	if( n < 3 || lua_tonumber( L, 3 ) == -1 )
	{
		nDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_AND_START;

		if( pPlayer->IsInProgressQuest( nQuestCode ) )
			nDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_IN_PROGRESS;
		else if( pPlayer->IsFinishableQuest( nQuestCode ) )
			nDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_AND_END;
	}
	else
	{
		nDialogType = lua_tonumber( L, 3 );
	}

	pPlayer->SetupQuestDialog( nQuestCode, nTextID, nDialogType );
	pPlayer->ShowDialog();

	return 0;
}

int SCRIPT_ShowQuestInfoWithoutNPC( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_ShowQuestInfoWithoutNPC() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, n + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	int nQuestCode = lua_tonumber( L, 1 );

	TS_SC_DIALOG::DIALOG_TYPE eDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_AND_START;
	int nTextID = GameContent::GetQuestTextId( nQuestCode, QUEST_IS_STARTABLE );

	if( pPlayer->IsInProgressQuest( nQuestCode ) )
	{
		eDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_IN_PROGRESS;
		nTextID = GameContent::GetQuestTextId( nQuestCode, QUEST_IS_IN_PROGRESS );
	}
	else if( pPlayer->IsFinishableQuest( nQuestCode ) )
	{
		eDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_AND_END;
		nTextID = GameContent::GetQuestTextId( nQuestCode, QUEST_IS_FINISHABLE );
	}

	bool bSuccess = false;

	if( n < 2 )
	{
		bSuccess = pPlayer->SetupQuestDialog( nQuestCode, nTextID, eDialogType );
	}
	else
	{
		bSuccess = pPlayer->SetupQuestDialog( nQuestCode, nTextID, lua_tonumber( L, 2 ) );
	}

	if( bSuccess )
		pPlayer->ShowDialog();

	return 0;
}

int SCRIPT_GetQuestProgress( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_QuestInfo() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	int nQuestCode = lua_tonumber( L, 1 );

	int nReturnCode = pPlayer->GetQuestProgress( nQuestCode );
	lua_pushnumber( L, nReturnCode );

	return 1;
}

int SCRIPT_GetQuestStatus( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetQuestStatus() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	QuestBase::QuestCode nQuestCode = static_cast< QuestBase::QuestCode >( (int)lua_tonumber( L, 1 ) );
	int nStatusIdx = lua_tonumber( L, 2 );

	if( nStatusIdx < 1 || nStatusIdx > QuestInstance::MAX_STATUS )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetQuestStatus() : invalid argument" );
		return 1;
	}

	StructQuest * pQuest = pPlayer->FindQuest( nQuestCode );

	lua_pushnumber( L, ( pQuest && pQuest->GetProgress() == QuestInstance::IN_PROGRESS ) ? pQuest->GetStatus( nStatusIdx - 1 ) : -1 );

	return 1;
}

int SCRIPT_SetQuestStatus( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 3 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_SetQuestStatus() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 4 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	QuestBase::QuestCode nQuestCode = static_cast< QuestBase::QuestCode >( (int)lua_tonumber( L, 1 ) );
	int nStatusIdx = lua_tonumber( L, 2 );

	if( nStatusIdx < 1 || nStatusIdx > QuestInstance::MAX_STATUS )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_SetQuestStatus() : invalid argument" );
		return 1;
	}

	StructQuest * pQuest = pPlayer->FindQuest( nQuestCode );

	if( pQuest && pQuest->GetProgress() == QuestInstance::IN_PROGRESS )
	{
		pQuest->UpdateStatus( nStatusIdx - 1, lua_tonumber( L, 3 ) );
		if ( pQuest->IsFinishable() )
		{
			pQuest->SetProgress( QuestInstance::FINISHABLE );
		}
		lua_pushnumber( L, 0 );
	}
	else
	{
		lua_pushnumber( L, -1 );
	}

	return 1;
}

int SCRIPT_GetLastAcceptQuest( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	lua_pushnumber( L, pPlayer->GetLastAcceptQuest() );

	return 1;
}

int SCRIPT_ResetFinishedQuest( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_StartQuest() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
		return 0;

	int nQuestCode = lua_tonumber( L, 1 );

	if( !pPlayer->ResetFinishedQuest( nQuestCode ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, 1 );

	return 1;
}