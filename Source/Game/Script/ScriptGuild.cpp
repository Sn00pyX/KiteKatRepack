
#include <vector>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "ScriptGuild.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "ScriptCommon.h"
#include "GuildManager.h"
#include "PartyManager.h"
#include "GameContent.h"
#include "DungeonManager.h"
#include "DB_Commands.h"

#include <toolkit/XEnv.h>


#include <lua/lua.hpp>

// 내부 참고용
static const int GUILD_CREATE_SUCCESS					= 0;
static const int GUILD_CREATE_ALREADY_IN_ANOTHER_GUILD	= 1;
static const int GUILD_CREATE_INVALID_GUILD_NAME		= 2;
static const int GUILD_CREATE_ALREADY_EXIST_NAME		= 3;
static const int GUILD_CREATE_INVALID_ARGUMENT			= 4;
static const int GUILD_CREATE_GUILD_BLOCK_TIME_LIMIT	= 5;

static const int ALLIANCE_CREATE_SUCCESS						= 0;
static const int ALLIANCE_CREATE_ALREADY_IN_ANOTHER_ALLIANCE	= 1;
static const int ALLIANCE_CREATE_INVALID_ALLIANCE_NAME			= 2;
static const int ALLIANCE_CREATE_ALREADY_EXIST_NAME				= 3;
static const int ALLIANCE_CREATE_INVALID_ARGUMENT				= 4;
static const int ALLIANCE_CREATE_NOT_GUILD_LEADER				= 5;
static const int ALLIANCE_CREATE_HAS_RAID_STARTED				= 6;
static const int ALLIANCE_CREATE_ALLIANCE_BLOCK_TIME_LIMIT		= 7;

static const int ALLIANCE_DESTROY_SUCCESS							= 0;
static const int ALLIANCE_DESTROY_NOT_IN_GUILD						= 1;
static const int ALLIANCE_DESTROY_NOT_IN_ALLIANCE					= 2;
static const int ALLIANCE_DESTROY_INVALID_ARGUMENT					= 3;
static const int ALLIANCE_DESTROY_NOT_GUILD_LEADER					= 4;
static const int ALLIANCE_DESTROY_NOT_ALLIANCE_LEADER				= 5;
static const int ALLIANCE_DESTROY_OTHER_MEMBER_GUILD_EXIST			= 6;
static const int ALLIANCE_DESTROY_DUNGEON_OWNER						= 7;
static const int ALLIANCE_DESTROY_DUNGEON_RAID_OR_SIEGE_REQUESTED	= 8;
static const int ALLIANCE_DESTROY_UNKNOWN							= 99;

int SCRIPT_ForceChangeGuildName( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isstring(L, 2) )
	{
		LUA()->Log( "SCRIPT_ForceChangeGuildName() : invalid argument" );
		return 0;
	}

	int nGuiildID = lua_tonumber( L, 1 );

	std::string szNewGuildName = lua_tostring_utf8( L, 2 );

	if( szNewGuildName.empty() || szNewGuildName.size() > 16 )
	{
		return 0;
	}

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, szNewGuildName.c_str(), static_cast< int >( szNewGuildName.size() ) + 1, 1, 16 ) ||
		GameContent::IsBannedWord( code_page, szNewGuildName.c_str() ) )
	{
		return 0;
	}

	GuildManager::GetInstance().ForceChangeGuildName( nGuiildID, szNewGuildName.c_str() );

	return 0;
}

int SCRIPT_ForcePromoteGuildLeader( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		LUA()->Log( "SCRIPT_ForcePromoteGuildLeader() : invalid argument" );
		return 0;
	}

	int nGuildID = lua_tonumber( L, 1 );
	PlayerUID nNewLeaderSID = lua_tonumber( L, 2 );

	PlayerUID nPrevLeaderSID = GuildManager::GetInstance().GetLeaderSID( nGuildID );

	unsigned short nResult = GuildManager::GetInstance().Promote( nGuildID, nNewLeaderSID );
	if( nResult != RESULT_SUCCESS )
	{
		lua_pushnumber( L, nResult );
		return 1;
	}
	const char * pszNewLeaderName = StructPlayer::GetPlayerName( GuildManager::GetInstance().GetLeaderSID( nGuildID ) );

	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "PROMOTE||%s", pszNewLeaderName );

	LOG::Log11N4S( LM_GUILD_PROMOTE, 0, nPrevLeaderSID, 0, nGuildID, nNewLeaderSID, 0, 0, 0, 0, 0, 0, "SCRIPT", LOG::STR_NTS, StructPlayer::GetPlayerName( nPrevLeaderSID ), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(), LOG::STR_NTS, pszNewLeaderName, LOG::STR_NTS );

	return 0;
}

int SCRIPT_ShowCreateGuild( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	SendShowCreateGuild( pPlayer );

	return 0;
};

int SCRIPT_ShowCreateAlliance( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	SendShowCreateAlliance( pPlayer );

	return 0;
};

int SCRIPT_GetMaxAllianceMemberCount( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer->GetGuildID() )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( pPlayer->GetGuildID(), pPlayer->GetGuildPermission(), GuildManager::PRA_ALLIANCE_MANAGEMENT ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( pPlayer->GetGuildID() );

	if( !nAllianceID || !GuildManager::GetInstance().IsAllianceLeader( nAllianceID, pPlayer->GetGuildID() ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, GuildManager::GetInstance().GetMaxAllianceCount( nAllianceID ) );
	return 1;
}

int SCRIPT_IncreaseMaxAllianceMemberCount( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer->GetGuildID() )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( pPlayer->GetGuildID(), pPlayer->GetGuildPermission(), GuildManager::PRA_ALLIANCE_MANAGEMENT ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( pPlayer->GetGuildID() );

	if( !nAllianceID || !GuildManager::GetInstance().IsAllianceLeader( nAllianceID, pPlayer->GetGuildID() ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	if( !GuildManager::GetInstance().IncMaxAllianceCount( nAllianceID ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_IsAllianceLeader( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer->GetGuildID() )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	if( !GuildManager::GetInstance().IsLeader( pPlayer->GetGuildID(), pPlayer->GetPlayerUID() ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( pPlayer->GetGuildID() );

	if( !nAllianceID || !GuildManager::GetInstance().IsAllianceLeader( nAllianceID, pPlayer->GetGuildID() ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_IsGuildLeader( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer->GetGuildID() )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	if( GuildManager::GetInstance().IsLeader( pPlayer->GetGuildID(), pPlayer->GetPlayerUID() ) )
	{
		lua_pushnumber( L, 1 );
		return 1;
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_CreateGuild( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isstring( L, 1 ) )
	{
		lua_pushnumber( L, GUILD_CREATE_INVALID_ARGUMENT );
		LUA()->Log( "SCRIPT_CreateGuild() : invalid argument" );
		return 1;
	}

	bool bEnrollInGuildList = ( n >= 2 && lua_isboolean( L, 2 ) ) ? lua_toboolean( L, 2 ) : false;
	bool bRecruiting = ( n >= 3 && lua_isboolean( L, 3 ) ) ? lua_toboolean( L, 3 ) : false;

	StructPlayer::iterator pit = getPlayer( L, n + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, GUILD_CREATE_INVALID_ARGUMENT );
		LUA()->Log( "SCRIPT_CreateGuild() : invalid argument" );
		return 1;
	}

	std::string szGuildName = lua_tostring_utf8( L, 1 );

	XStringUtil::Trim( szGuildName );
	if( szGuildName.empty() || szGuildName.size() > 16 )
	{
		lua_pushnumber( L, GUILD_CREATE_INVALID_GUILD_NAME );
		return 1;
	}

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, szGuildName.c_str(), static_cast< int >( szGuildName.size() ) + 1, 1, 16 ) ||
		GameContent::IsBannedWord( code_page, szGuildName.c_str() ) )
	{
		lua_pushnumber( L, GUILD_CREATE_INVALID_GUILD_NAME );
		return 1;
	}
	// 이미 길드에 가입되어 있으면..
	if( pPlayer->IsInGuild() )
	{	
		lua_pushnumber( L, GUILD_CREATE_ALREADY_IN_ANOTHER_GUILD );
		return 1;
	}

	// 길드에서 탈퇴한 지 7일이 지나지 않으면 창설 불가
	if( pPlayer->GetPrevGuildID() )
	{
		lua_pushnumber( L, GUILD_CREATE_GUILD_BLOCK_TIME_LIMIT );
		return 1;
	}

	if( GuildManager::GetInstance().TryMakeGuild( szGuildName.c_str(), pPlayer, bEnrollInGuildList, bRecruiting ) == false )
	{
		lua_pushnumber( L, GUILD_CREATE_ALREADY_EXIST_NAME );
		return 1;
	}

	pPlayer->Save( true );

	lua_pushnumber( L, GUILD_CREATE_SUCCESS );
	return 1;
}

int SCRIPT_DestroyGuild( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 && !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DestroyGuild() : invalid argument" );
		return 1;
	}

	int nGuildID = lua_tonumber( L, 1 );

	unsigned short nResult = GuildManager::GetInstance().IsDestroyableGuild( nGuildID );
	// 던전 레이드 기록/신청만 삭제하면 해산 가능할 경우 처리
	if( nResult == RESULT_NOT_ACTABLE_IN_SIEGE_OR_RAID )
	{
		DungeonManager::Instance().ClearDungeonRaidRecord( GuildManager::GetInstance().GetRaidDungeonID( nGuildID ), nGuildID );
		GuildManager::GetInstance().SetRaidDungeonID( nGuildID, 0 );
		nResult = GuildManager::GetInstance().IsDestroyableGuild( nGuildID );
	}

	if( nResult != RESULT_SUCCESS )
	{
		lua_pushstring( L, "unable to destroy" );
		LUA()->Log( "SCRIPT_DestroyGuild : unable to destroy" );
		return 1;
	}

	std::string strGuildName = GuildManager::GetInstance().GetGuildName( nGuildID );

	// 길드 해산 처리
	struct myGuildFunctor : GuildManager::GuildFunctor
	{
		myGuildFunctor( const std::string & strGuildName )
			: m_strGuildName( strGuildName )
		{}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer *pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( handle ) );
			if( !pPlayer ) return true;

			PrintfChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pPlayer, "DESTROY|%s|", m_strGuildName.c_str() );

			return true;
		}

		std::string m_strGuildName;
	} _fo( strGuildName );

	LOG::Log11N4S( LM_GUILD_DESTROY, 0, 0, 0, nGuildID, 0, 0, 0, 0, 0, 0, 0, "Script", LOG::STR_NTS, "GMTool", LOG::STR_NTS, strGuildName.c_str(), LOG::STR_NTS, "", 0 );

	GuildManager::GetInstance().DoEachMember( nGuildID, _fo );
	GuildManager::GetInstance().DestroyGuild( nGuildID );	

	char szBuf[255];
	s_sprintf( szBuf, _countof( szBuf ), "GDESTROY|%d", nGuildID );
	SendGlobalChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", szBuf, static_cast< int >( strlen( szBuf ) ) );

	return 0;
}

int SCRIPT_CreateAlliance( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isstring(L, 1) )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_INVALID_ARGUMENT );
		LUA()->Log( "SCRIPT_CreateAlliance() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	int nGuildID = pPlayer->GetGuildID();

	// 길드에 소속되어 있지 않으면 연합 결성 불가
	// 권한이 없으면 연합 결성 불가
	if( !nGuildID || !GuildManager::GetInstance().IsPermitted( nGuildID, pPlayer->GetGuildPermission(), GuildManager::PRA_ALLIANCE_MANAGEMENT ) )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_NOT_GUILD_LEADER );
		return 1;
	}

	// 연합에서 탈퇴/해산한 지 7일이 지나지 않았으면 연합 결성 불가
	if( GuildManager::GetInstance().IsInGuildAllianceBlockTime( nGuildID ) )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_ALLIANCE_BLOCK_TIME_LIMIT );
		return 1;
	}

	// 소유 중인 던전이 있을 경우 연합 결성 불가
	int nDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nGuildID );
	if( nDungeonID && DungeonManager::Instance().GetOwnGuildID( nDungeonID ) == nGuildID )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_HAS_RAID_STARTED );
		return 1;
	}

	std::string szAllianceName = lua_tostring_utf8( L, 1 );

	XStringUtil::Trim( szAllianceName );
	if( szAllianceName.empty() || szAllianceName.size() > 16 )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_INVALID_ALLIANCE_NAME );
		return 1;
	}

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, szAllianceName.c_str(), static_cast< int >( szAllianceName.size() ) + 1, 1, 16 ) ||
		GameContent::IsBannedWord( code_page, szAllianceName.c_str() ) )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_INVALID_ALLIANCE_NAME );
		return 1;
	}

	// 이미 연합에 가입되어 있으면 연합 결성 불가
	if( GuildManager::GetInstance().GetAllianceID( nGuildID ) )
	{	
		lua_pushnumber( L, ALLIANCE_CREATE_ALREADY_IN_ANOTHER_ALLIANCE );
		return 1;
	}

	// 연합 만들고
	if( !GuildManager::GetInstance().CreateAlliance( nGuildID, szAllianceName.c_str() ) )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_INVALID_ALLIANCE_NAME );
		return 1;
	}

	std::string strGuildName = GuildManager::GetInstance().GetGuildName( nGuildID );
	int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );

	LOG::Log11N4S( LM_ALLIANCE_CREATE, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, nGuildID, nAllianceID, 0, 0, 0, 0, 0, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, strGuildName.c_str(), LOG::STR_NTS, szAllianceName.c_str(), LOG::STR_NTS );

	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", nGuildID, "AINFO|%d|%s|1|%d|%d|%s|%s|%d|",
		nAllianceID, szAllianceName.c_str(), nGuildID, nGuildID, strGuildName.c_str(),
		StructPlayer::GetPlayerName( GuildManager::GetInstance().GetLeaderSID( nGuildID ) ),
		GuildManager::GetInstance().GetMemberCount( nGuildID ) );

	lua_pushnumber( L, ALLIANCE_CREATE_SUCCESS );
	return 1;
}

int SCRIPT_DestroyAlliance( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n != 0 )
	{
		lua_pushnumber( L, ALLIANCE_DESTROY_INVALID_ARGUMENT );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	// 길드가 아니면 리턴
	int nGuildID = pPlayer->GetGuildID();
	if( !nGuildID )
	{
		lua_pushnumber( L, ALLIANCE_DESTROY_NOT_IN_GUILD );
		return 1;
	}

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );

	if( !nAllianceID )
	{
		lua_pushnumber( L, ALLIANCE_DESTROY_NOT_IN_ALLIANCE );
		return 1;
	}

	std::string allianceName( GuildManager::GetInstance().GetAllianceName( nAllianceID ) );
	std::string guildName( GuildManager::GetInstance().GetGuildName( nGuildID ) );

	// 리더가 아니면 리턴
	if( !GuildManager::GetInstance().IsLeader( nGuildID, pPlayer->GetPlayerUID() ) )
	{
		lua_pushnumber( L, ALLIANCE_DESTROY_NOT_GUILD_LEADER );
		return 1;
	}
	if( !GuildManager::GetInstance().IsAllianceLeader( nAllianceID, nGuildID ) )
	{
		lua_pushnumber( L, ALLIANCE_DESTROY_NOT_ALLIANCE_LEADER );
		return 1;
	}

	// 멤버 길드가 마스터 길드외에 더 존재하면 해산 불가
	if( GuildManager::GetInstance().GetAllianceMemberGuildCount( nAllianceID ) > 1 )
	{
		lua_pushnumber( L, ALLIANCE_DESTROY_OTHER_MEMBER_GUILD_EXIST );
		return 1;
	}

	// 던전을 소유 중/레이드 신청 중이면 해산 불가
	int nLeadGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID );
	int nDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nLeadGuildID );
	if( nDungeonID )
	{
		if( DungeonManager::Instance().GetOwnGuildID( nDungeonID ) == nLeadGuildID )
		{
			lua_pushnumber( L, ALLIANCE_DESTROY_DUNGEON_OWNER );
			return 1;
		}
		else
		{
			lua_pushnumber( L, ALLIANCE_DESTROY_DUNGEON_RAID_OR_SIEGE_REQUESTED );
			return 1;
		}
	}

	if( !GuildManager::GetInstance().DestroyAlliance( nAllianceID ) )
	{
		lua_pushnumber( L, ALLIANCE_DESTROY_UNKNOWN );
		return 1;
	}

	PrintfGuildChatMessage( CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", nGuildID, "DESTROY|%s|", allianceName.c_str() );

	LOG::Log11N4S( LM_ALLIANCE_DESTROY, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, nGuildID, nAllianceID, 0, 0, 0, 0, 0, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, guildName.c_str(), LOG::STR_NTS, allianceName.c_str(), LOG::STR_NTS );

	lua_pushnumber( L, ALLIANCE_DESTROY_SUCCESS );
	return 1;
}

int SCRIPT_CheckValidAllianceName( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isstring(L, 1) )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_INVALID_ARGUMENT );
		LUA()->Log( "SCRIPT_CheckValidAllianceName() : invalid argument" );
		return 1;
	}

	std::string szAllianceName = lua_tostring_utf8( L, 1 );

	if( szAllianceName.empty() || szAllianceName.size() > 16 )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_INVALID_ALLIANCE_NAME );
		return 1;
	}

	if( GuildManager::GetInstance().IsExistAlliance( szAllianceName.c_str() ) )
	{	
		lua_pushnumber( L, ALLIANCE_CREATE_ALREADY_EXIST_NAME );
		return 1;
	}

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( szAllianceName.size() > 16 ||
		!GameRule::IsValidName( code_page, szAllianceName.c_str(), static_cast< int >( szAllianceName.size() ) + 1, 1, 16 ) ||
		GameContent::IsBannedWord( code_page, szAllianceName.c_str() ) )
	{
		lua_pushnumber( L, ALLIANCE_CREATE_INVALID_ALLIANCE_NAME );
		return 1;
	}

	lua_pushnumber( L, ALLIANCE_CREATE_SUCCESS );
	return 1;
}

int SCRIPT_CheckValidGuildName( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isstring(L, 1) )
	{
		lua_pushnumber( L, GUILD_CREATE_INVALID_ARGUMENT );
		LUA()->Log( "SCRIPT_CheckValidGuildName() : invalid argument" );
		return 1;
	}

	std::string szGuildName = lua_tostring_utf8( L, 1 );

	if( szGuildName.empty() || szGuildName.size() > 16 )
	{
		lua_pushnumber( L, GUILD_CREATE_INVALID_GUILD_NAME );
		return 1;
	}

	if( GuildManager::GetInstance().IsExistGuild( szGuildName.c_str() ) )
	{	
		lua_pushnumber( L, GUILD_CREATE_ALREADY_EXIST_NAME );
		return 1;
	}

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( szGuildName.size() > 16 ||
		!GameRule::IsValidName( code_page, szGuildName.c_str(), static_cast< int >( szGuildName.size() ) + 1, 1, 16 ) ||
		GameContent::IsBannedWord( code_page, szGuildName.c_str() ) )
	{
		lua_pushnumber( L, GUILD_CREATE_INVALID_GUILD_NAME );
		return 1;
	}

	lua_pushnumber( L, GUILD_CREATE_SUCCESS );
	return 1;
}

int SCRIPT_UpdateGuildInfo( struct lua_State *L )
{
	int n = lua_gettop( L );

	if( n < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_UpdateGuildInfo() : invalid argument" );
		return 0;
	}

	GuildManager::GetInstance().LoadGuildIconInfo( lua_tonumber( L, 1 ) );

	return 0;
}

int SCRIPT_UpdateGuildBannerInfo( struct lua_State *L )
{
	int n = lua_gettop( L );

	if( n < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_UpdateGuildBannerInfo() : invalid argument" );
		return 0;
	}

	GuildManager::GetInstance().LoadGuildBannerInfo( lua_tonumber( L, 1 ) );

	return 0;
}

int SCRIPT_SetGuildBlockTime( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 )  )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_SetGuildBlockTime() : invalid argument" );
		return 1;
	}

	time_t tGuildBlockTime = time( NULL ) + lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, -2 );
		LUA()->Log( "SCRIPT_SetGuildBlockTime() : invalid argument" );
		return 1;
	}

	pPlayer->SetGuildBlockTime( tGuildBlockTime );
	GuildManager::GetInstance().Push( new DB_SetGuildBlockTime( pPlayer->GetPlayerUID(), tGuildBlockTime ) );

	lua_pushnumber( L, lua_tonumber( L, 1 ) );
	return 1;
}

int SCRIPT_GetGuildBlockTime( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetGuildBlockTime() : invalid argument" );
		return 1;
	}

	time_t tCurrent = time( NULL );
	time_t tGuildBlockTime = pPlayer->GetGuildBlockTime();

	if( tGuildBlockTime > tCurrent )
		lua_pushnumber( L, tGuildBlockTime - tCurrent );
	else
		lua_pushnumber( L, 0 );

	return 1;
}

int SCRIPT_GetGuildName(struct lua_State* L)
{
	int n = lua_gettop(L);			// number of arguments

	if (n < 1 || !lua_isnumber(L, 1))
	{
		lua_pushstring(L, "");
		LUA()->Log("SCRIPT_GetGuildName() : invalid argument");
		return 1;
	}

	std::string name = GuildManager::GetInstance().GetGuildName(lua_tonumber(L, 1));

	lua_pushstring_utf8(L, name.c_str());

	return 1;
}

int SCRIPT_AddGuildPoint(struct lua_State* L)
{
	int n = lua_gettop(L);			// number of arguments

	if (n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L,2))
	{
		lua_pushstring(L, "");
		LUA()->Log("SCRIPT_AddGuildPoint() : invalid argument");
		return 1;
	}

	GuildManager::GuildInfo* pInfo = GuildManager::GetInstance().getGuildInfo(lua_tonumber(L, 1));

	if (!pInfo)
	{
		return 1;
	}

	int points = lua_tonumber(L, 2);
	pInfo->nPoint += points;

	pInfo->strGuildName;

	char buff[256];
	int ssize = s_sprintf(buff, 256, "GINFO|%d|%s|%s|%d|%s|%d|%d|%s|%d|%d",
		pInfo->nGuildId,
		pInfo->strGuildName.c_str(),
		StructPlayer::GetPlayerName(GuildManager::GetInstance().GetLeaderSID(pInfo->nGuildId)),
		pInfo->vMemberNameList.size(),
		pInfo->strNotice.c_str(),
		pInfo->bRecruiting,
		pInfo->nDungeonId,
		pInfo->strURL.c_str(),
		pInfo->nGrade,
		pInfo->nPoint);

	::SendGuildChatMessage(CHAT_GUILD_SYSTEM, "@GUILD", pInfo->nGuildId, buff, ssize);
	GuildManager::GetInstance().Push(new DB_UpdateGuildGradePoint(pInfo->nGuildId, pInfo->nGrade, pInfo->nPoint));
	return 1;
}

int SCRIPT_GuildNotice(struct lua_State* L)
{
	int n = lua_gettop(L);			// number of arguments

	if (n < 2 || !lua_isnumber(L, 1))
	{
		lua_pushstring(L, "");
		LUA()->Log("SCRIPT_AddGuildPoint() : invalid argument");
		return 1;
	}

	GuildManager::GuildInfo* pInfo = GuildManager::GetInstance().getGuildInfo(lua_tonumber(L, 1));

	if (!pInfo)
	{
		return 1;
	}

	std::string szMessage = lua_tostring_utf8(L, 2);

	::SendGuildChatMessage(CHAT_NOTICE, "@NOTICE", pInfo->nGuildId, szMessage.c_str());
	return 1;
}

int SCRIPT_GetGuildPoint(struct lua_State* L)
{
	int n = lua_gettop(L);			// number of arguments

	if (n < 1 || !lua_isnumber(L, 1))
	{
		lua_pushstring(L, "");
		LUA()->Log("SCRIPT_GetGuildPoint() : invalid argument");
		return 1;
	}

	GuildManager::GuildInfo* pInfo = GuildManager::GetInstance().getGuildInfo(lua_tonumber(L, 1));

	if (pInfo)
		lua_pushnumber(L, pInfo->nPoint);
	else
		lua_pushnumber(L, 0);

	return 1;
}

