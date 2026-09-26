
#include <network/IConnection.h>
#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>
#include <toolkit/XStringUtil.h>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "ScriptMisc.h"
#include "StructPlayer.h"
#include "GameProc.h"
#include "StructSummon.h"
#include "GameContent.h"
#include "GameMessage.h"
#include "SendMessage.h"
#include "ScriptCommon.h"
#include "ChannelManager.h"
#include "GuildManager.h"
#include "StructSkill.h"
#include "DB_Commands.h"
#include "DungeonManager.h"
#include "InstanceDungeonManager.h"
#include "BattleArenaManager.h"
#include "PartyManager.h"
#include "Extern.h"
#include "HuntaholicManager.h"
#include "AuctionManager.h"
#include "RankingManager.h"
#include "Constant.h"

#include <lua/lua.hpp>


int SCRIPT_GetLayerOfChannel( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetUserCount() : invalid argument" );
		return 1;
	}
	
	int channel_id = lua_tonumber( L, 1 );
	int channel_no = lua_tonumber( L, 2 );

	lua_pushnumber( L, ChannelManager::GetLayerOfChannel( channel_id, channel_no ) );

	return 1;
}

int SCRIPT_GetMinChannelNo( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetMaxUserLimitChannelNum() : invalid argument" );
		return 1;
	}

	int channel_id = lua_tonumber( L, 1 );

	lua_pushnumber( L, ChannelManager::GetMinChannelNo( channel_id ) );

	return 1;
}

int SCRIPT_GetMaxChannelNo( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetMaxUserLimitChannelNum() : invalid argument" );
		return 1;
	}

	int channel_id = lua_tonumber( L, 1 );

	lua_pushnumber( L, ChannelManager::GetMaxChannelNo( channel_id ) );

	return 1;
}

int SCRIPT_GetProperChannelNo( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetProperChannelNo() : invalid argument" );
		return 1;
	}

	int channel_id = lua_tonumber( L, 1 );

	lua_pushnumber( L, ChannelManager::GetProperChannel( channel_id ) );

	return 1;
}

int SCRIPT_GetUserCountInChannel( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetUserCount() : invalid argument" );
		return 1;
	}

	
	int channel_id = lua_tonumber( L, 1 );
	int channel_no = lua_tonumber( L, 2 );

	unsigned char layer = ChannelManager::GetLayerOfChannel( channel_id, channel_no );

	if( layer )
		lua_pushnumber( L, ChannelManager::GetPlayerCountInLayer( layer ) );
	else
		lua_pushnumber( L, 0 );

	return 1;
}

int SCRIPT_Warp( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_Warp() : invalid argument" );
		return 1;
	}

	AR_UNIT x, y;
	unsigned char layer = 0;

	x = lua_tonumber( L, 1 );
	y = lua_tonumber( L, 2 );

	if( x < 0 || x > g_nMapWidth || y < 0 || y > g_nMapHeight )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_Warp() : invalid argument" );
		return 1;
	}

	StructPlayer *pPlayer = NULL;

	if( nArgCnt > 3 && lua_isnumber( L, 3 ) && lua_isnumber( L, 4 ) )
	{
		int channel_id = lua_tonumber( L, 3 );
		int channel_no = lua_tonumber( L, 4 );

		layer = ChannelManager::GetLayerOfChannel( channel_id, channel_no );

		StructPlayer::iterator pit = getPlayer( L, 5 );
		pPlayer = *pit;

	}
	else if( nArgCnt > 2 && lua_isnumber( L, 3 ) )
	{
		layer = lua_tonumber( L, 3 );

		StructPlayer::iterator pit = getPlayer( L, 4 );
		pPlayer = *pit;
	}
	else
	{
		StructPlayer::iterator pit = getPlayer( L, 3 );
		pPlayer = *pit;

		if( pPlayer && pPlayer->GetLayer() && ChannelManager::GetChannelId( pPlayer->GetLayer() ) == ChannelManager::GetChannelId( x, y ) )
			layer = pPlayer->GetLayer();
		else
			layer = ChannelManager::GetProperLayer( x, y );
	}

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid name" );
		LUA()->Log( "SCRIPT_Warp() : invalid name" );
		return 1;
	}

	if( layer >= MAX_LAYER || layer < 0 )
	{
		layer = 0;
	}

	AR_UNIT targetX = x, targetY = y;
	bool bValidPos = true;
	unsigned short try_cnt = 0;
	while( GameContent::IsBlocked( targetX, targetY ) )
	{
		targetX = XRandom( x - 60, x + 60 );
		targetY = XRandom( y - 60, y + 60 );

		if( ++try_cnt > 300 )
		{
			bValidPos = false;
			break;
		}
	}

	if( !bValidPos )
	{
		_cprint( "Fail To Warp With Given Position [%d, %d], PlayerSID[%d] (Script_WARP)\n", x, y, pPlayer->GetPlayerUID() );
		FILELOG( "Fail To Warp With Given Position [%d, %d], PlayerSID[%d] (Script_WARP)", x, y, pPlayer->GetPlayerUID() );
		lua_pushstring( L, "invalid position" );
		LUA()->Log( "SCRIPT_Warp() : warp failure" );
		return 1;
	}

	pPlayer->PendWarp( targetX, targetY, layer );
	ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	return 0;
}

int SCRIPT_ForceWarp( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_ForceWarp() : invalid argument" );
		return 1;
	}

	AR_UNIT x, y;
	unsigned char layer = 0;

	x = lua_tonumber( L, 1 );
	y = lua_tonumber( L, 2 );

	if( x < 0 || x > g_nMapWidth || y < 0 || y > g_nMapHeight )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_ForceWarp() : invalid argument" );
		return 1;
	}

	StructPlayer *pPlayer = NULL;

	if( nArgCnt > 3 && lua_isnumber( L, 3 ) && lua_isnumber( L, 4 ) )
	{
		int channel_id = lua_tonumber( L, 3 );
		int channel_no = lua_tonumber( L, 4 );

		layer = ChannelManager::GetLayerOfChannel( channel_id, channel_no );

		StructPlayer::iterator pit = getPlayer( L, 5 );
		pPlayer = *pit;

	}
	else if( nArgCnt > 2 && lua_isnumber( L, 3 ) )
	{
		layer = lua_tonumber( L, 3 );

		StructPlayer::iterator pit = getPlayer( L, 4 );
		pPlayer = *pit;
	}
	else
	{
		StructPlayer::iterator pit = getPlayer( L, 3 );
		pPlayer = *pit;

		if( pPlayer && pPlayer->GetLayer() && ChannelManager::GetChannelId( pPlayer->GetLayer() ) == ChannelManager::GetChannelId( x, y ) )
			layer = pPlayer->GetLayer();
		else
			layer = ChannelManager::GetProperLayer( x, y );
	}

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid name" );
		LUA()->Log( "SCRIPT_ForceWarp() : invalid name" );
		return 1;
	}

	if( layer >= MAX_LAYER || layer < 0 )
	{
		layer = 0;
	}

	pPlayer->PendWarp( x, y, layer, -1, true );
	ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	return 0;
}

int SCRIPT_RecallPlayer( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_RecallPlayer() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pTargetPlayer = *pit;

	pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pTargetPlayer || !pPlayer )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_RecallPlayer() : player not found" );
		return 1;
	}

	pTargetPlayer->PendWarp( pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer() );
	ArcadiaServer::Instance().SetObjectPriority( pTargetPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	return 0;
}

int SCRIPT_SetSpeed( struct lua_State *L )
{	
	int nArgCnt = lua_gettop( L );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;
	
	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetSpeed() : invalid argument" );
		return 1;
	}

	if( !pPlayer )
	{
		lua_pushstring( L, "Can't find player" );
		LUA()->Log( "SCRIPT_SetSpeed() : Can't find player" );
		return 1;
	}

	pPlayer->SetCreatureSpeed( lua_tonumber( L, nArgCnt ) );

	return 0;
}


int SCRIPT_Stat( struct lua_State *L )
{	
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "Can't find player" );
		LUA()->Log( "SCRIPT_Stat() : Can't find player" );
		return 1;
	}

	ArPosition pos = pPlayer->GetCurrentPosition( GetArTime() );
	LUA()->LogPrintf( "Name : %s", pPlayer->GetName() );
	LUA()->LogPrintf( "IP : %s", pPlayer->pConnection->GetPeerAddress().GetAddr() );
	LUA()->LogPrintf( "Position : %f,%f :%d", pos.x, pos.y, pPlayer->GetLayer() );
	
	return 0;
}

			
int SCRIPT_SaveAllPlayer( struct lua_State *L )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

	SaveAllPlayer();

	LUA()->Log( "Saving...\n" );

	return 0;
}

int SCRIPT_SavePlayer( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "Can't find player" );
		LUA()->Log( "SCRIPT_SavePlayer() : Can't find player" );
		return 1;
	}

	SavePlayer( pPlayer );

	return 0;
}

int SCRIPT_HasItem( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nID = lua_tonumber( L, nArgCnt );

	if( pPlayer->FindItem( (ItemBase::ItemCode)nID ) ) lua_pushnumber( L, 1 );
	else lua_pushnumber( L, 0 );

	return 1;
}

int SCRIPT_GetPlayerFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	std::string strValue = pPlayer->GetFlag( lua_tostring_utf8( L, nArgCnt ).c_str() );

	char * pEnd = NULL;
	double fValue = strtod( strValue.c_str(), &pEnd );

	if( !strValue.empty() && pEnd == ( strValue.c_str() + strValue.length() ) )
	{
		lua_pushnumber( L, fValue );
	}
	else
	{
		lua_pushstring_utf8( L, strValue.c_str() );
	}	

	return 1;
}

int SCRIPT_RemovePlayerFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, (int)pPlayer->RemoveFlag( lua_tostring_utf8( L, nArgCnt ).c_str() ) );
	return 1;
}

int SCRIPT_SetPlayerFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	pPlayer->SetFlag( lua_tostring_utf8( L, nArgCnt-1 ).c_str(), lua_tostring_utf8( L, nArgCnt ).c_str() );

	return 0;
}

int SCRIPT_GetAccountFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	std::string strValue = pPlayer->GetAccountFlag( lua_tostring_utf8( L, nArgCnt ).c_str() );

	char * pEnd = NULL;
	double fValue = strtod( strValue.c_str(), &pEnd );

	if( !strValue.empty() && pEnd == ( strValue.c_str() + strValue.length() ) )
	{
		lua_pushnumber( L, fValue );
	}
	else
	{
		lua_pushstring_utf8( L, strValue.c_str() );
	}	

	return 1;
}

int SCRIPT_RemoveAccountFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, (int)pPlayer->RemoveAccountFlag( lua_tostring_utf8( L, nArgCnt ).c_str() ) );
	return 1;
}

int SCRIPT_SetAccountFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	pPlayer->SetAccountFlag( lua_tostring_utf8( L, nArgCnt-1 ).c_str(), lua_tostring_utf8( L, nArgCnt ).c_str() );

	return 0;
}

int SCRIPT_GetTargetValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	GameObject* object = GameObject::raw_get( pPlayer->GetTarget() );

	if( !object )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	std::string szKey = lua_tostring_utf8( L, 1 );

	std::string strValue = object->IsPlayer() ? static_cast< StructPlayer* >( object )->GetAsString( szKey.c_str() ) :
	                       object->IsSummon() ? static_cast< StructSummon* >( object )->GetAsString( szKey.c_str() ) :
						   object->IsMonster() ? static_cast< StructMonster* >( object )->GetAsString( szKey.c_str() ) :
						   "";

	// 타입이 __int64인 경우 미리 처리
	if( _stricmp( szKey.c_str(), "exp" ) == 0 || _stricmp( szKey.c_str(), "gold" ) == 0 )
	{
		__int64 n64 = _atoi64( strValue.c_str() );

		lua_pushnumber( L, n64 );
		return 1;
	}

	char * pEnd = NULL;
	double fValue = strtod( strValue.c_str(), &pEnd );

	if( !strValue.empty() && pEnd == ( strValue.c_str() + strValue.length() ) )
	{
		lua_pushnumber( L, fValue );
	}
	else
	{
		lua_pushstring_utf8( L, strValue.c_str() );
	}

	return 1;
}

int SCRIPT_GetHandleValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) ) return 0;

	AR_HANDLE handle = lua_tonumber( L, 1 );

	GameObject* object = GameObject::raw_get( handle );

	if( !object )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	std::string szKey = lua_tostring_utf8( L, 2 );

	std::string strValue = object->IsPlayer() ? static_cast< StructPlayer* >( object )->GetAsString( szKey.c_str() ) :
		object->IsSummon() ? static_cast< StructSummon* >( object )->GetAsString( szKey.c_str() ) :
		object->IsMonster() ? static_cast< StructMonster* >( object )->GetAsString( szKey.c_str() ) :
		"";

	// 타입이 __int64인 경우 미리 처리
	if( _stricmp( szKey.c_str(), "exp" ) == 0 || _stricmp( szKey.c_str(), "gold" ) == 0 )
	{
		__int64 n64 = _atoi64( strValue.c_str() );

		lua_pushnumber( L, n64 );
		return 1;
	}

	char * pEnd = NULL;
	double fValue = strtod( strValue.c_str(), &pEnd );

	if( !strValue.empty() && pEnd == ( strValue.c_str() + strValue.length() ) )
	{
		lua_pushnumber( L, fValue );
	}
	else
	{
		lua_pushstring_utf8( L, strValue.c_str() );
	}

	return 1;
}

int SCRIPT_GetValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	std::string szKey = lua_tostring_utf8( L, 1 );

	std::string strValue = pPlayer->GetAsString( szKey.c_str() );

	// 타입이 __int64인 경우 미리 처리
	if( _stricmp( szKey.c_str(), "exp" ) == 0 || _stricmp( szKey.c_str(), "gold" ) == 0 || _stricmp( szKey.c_str(), "jp" ) == 0 )
	{
		__int64 n64 = _atoi64( strValue.c_str() );

		lua_pushnumber( L, n64 );
		return 1;
	}

	char * pEnd = NULL;
	double fValue = strtod( strValue.c_str(), &pEnd );

	if( !strValue.empty() && pEnd == ( strValue.c_str() + strValue.length() ) )
	{
		lua_pushnumber( L, fValue );
	}
	else
	{
		lua_pushstring_utf8( L, strValue.c_str() );
	}

	return 1;
}

int SCRIPT_EchoValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer *pPlayer = NULL, *pTarget = NULL;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	pTarget = *pit;

	if( nArgCnt == 1)
		pPlayer = pTarget;
	else if( nArgCnt == 2 )
	{
		StructPlayer::iterator pit = getPlayer( L, 3 );
		pPlayer = *pit;
	}
	else
		return 0;

	if( !pPlayer || !pTarget )
		return 0;

	std::string szKey = lua_tostring_utf8( L, 1 );

	std::string strValue = pTarget->GetAsString( szKey.c_str() );

	// 타입에 관계없이 알아서 스트링으로 얻어진 상태이므로 그대로 출력
	PrintfChatMessage( false, CHAT_EXP, "@SYSTEM", pPlayer, "<BR>* %s: %s<BR>", szKey.c_str(), strValue.c_str() );

	return 0;
}

int SCRIPT_GetAllValue( struct lua_State *L )
{
	
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	std::string strValue = pPlayer->EnumAllProperty();
	lua_pushstring_utf8( L, strValue.c_str() );

	return 1;
}


int SCRIPT_SetValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	
	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !(*pit) ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	pPlayer->SetAsString( lua_tostring_utf8( L, 1 ).c_str(), lua_tostring_utf8( L, 2 ).c_str() );

	return 0;
}

int SCRIPT_AddValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	std::string szKey = lua_tostring_utf8( L, nArgCnt-1 );
	std::string szData = lua_tostring_utf8( L, nArgCnt );

	if( szKey.empty() || szData.empty() )
	{
		return 0;
	}

	std::string strValue = pPlayer->GetAsString( szKey.c_str() );
	char buf[256] = { 0, };

	// 타입이 __int64인 경우 미리 처리
	if( _stricmp( szKey.c_str(), "exp" ) == 0 || _stricmp( szKey.c_str(), "gold" ) == 0 || _stricmp( szKey.c_str(), "jp" ) == 0 )
	{
		__int64 n64 = _atoi64( strValue.c_str() );
		n64 += _atoi64( szData.c_str() );
		s_sprintf( buf, _countof( buf ), "%I64d", n64 );
	}
	else
	{
		char * pEnd = NULL;
		double fValue = strtod( strValue.c_str(), &pEnd );

		// 숫자 이외의 값은 AddValue 스크립트를 무시.
		if( strValue.empty() || pEnd != strValue.c_str() + strValue.length() )
			return 0;

		double d = atof( strValue.c_str() );
		d += atof( szData.c_str() );
		s_sprintf( buf, _countof( buf ), "%f", d );
	}

	pPlayer->SetAsString( szKey, buf );
	return 0;
}

int SCRIPT_AddExpJP( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushstring( L, "SCRIPT_AddExpJP() : invalid name" );
		LUA()->Log( "SCRIPT_AddExpJP() : invalid name" );
		return 1;
	}

	if( !nArgCnt ||
		( nArgCnt >= 1 && !lua_isnumber( L, 1 ) ) ||
		( nArgCnt >= 2 && !lua_isnumber( L, 2 ) ) ||
		( nArgCnt >= 3 && !lua_isboolean( L, 3 ) ) ||
		( nArgCnt >= 4 && !lua_isboolean( L, 4 ) ) )
	{
		lua_pushstring( L, "SCRIPT_AddExpJP() : invalid argument" );
		LUA()->Log( "SCRIPT_AddExpJP() : invalid argument" );
		return 1;
	}

	__int64 nExp = _atoi64( lua_tostring( L, 1 ) );
	__int64 nJP = ( nArgCnt >= 2 ) ? _atoi64( lua_tostring( L, 2 ) ) : 0;
	bool bApplyPlayerEXPRule = ( nArgCnt >= 3 ) ? lua_toboolean( L, 3 ) : false;
	bool bApplyStamina = ( nArgCnt >= 4 ) ? lua_toboolean( L, 4 ) : false;

	if( !nExp && !nJP )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// bApplyPlayerEXPRule가 false 라면 bApplyStamina도 false 여야 함.
	if( !bApplyPlayerEXPRule && bApplyStamina )
	{
		lua_pushstring( L, "SCRIPT_AddExpJP() : invalid argument" );
		LUA()->Log( "SCRIPT_AddExpJP() : invalid argument" );
		return 1;
	}

	if( bApplyPlayerEXPRule )
	{
		pPlayer->AddExp( nExp, nJP, bApplyStamina );
	}
	else
	{
		// 최대/최소 경험치/JP 보유량 체크 및 획득 가능한 수치만큼으로 획득량 조절
		__int64 nCurrentExp = pPlayer->GetEXP();
		if( GameRule::nMaxLevel > 0 && nExp > 0 )
		{
			const __int64 MAX_EXP = GameContent::GetNeedExp( GameRule::nMaxLevel ) - 1; // Epic 9.1 - MaxLevel 99.99%까지 경험치 얻을 수 있게 변경

			if( nCurrentExp + nExp > MAX_EXP )
				nExp = std::max( (__int64)0, MAX_EXP - nCurrentExp );
		}
		else if( nExp < 0 )
		{
			nExp = std::max( -1 * nCurrentExp, nExp );
		}

		__int64 nCurrentJP = pPlayer->GetJobPoint();
		if( nJP > 0 )
		{
			if( nCurrentJP + nJP > GameRule::MAX_OWNABLE_JP )
				nJP = std::max( (__int64)0, GameRule::MAX_OWNABLE_JP - nCurrentJP );
		}
		else
		{
			nJP = std::max( -1 * nCurrentJP, nJP );
		}

		// 획득량이 없어졌으면 아무것도 안 함
		if( !nExp && !nJP )
		{
			lua_pushnumber( L, 0 );
			return 1;
		}

		pPlayer->StructCreature::AddExp( nExp, nJP, bApplyStamina );
	}

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_GetBaseSkillLevel( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, pPlayer->GetBaseSkillLevel( lua_tonumber( L, nArgCnt ) ) );
	return 1;
}

int SCRIPT_LearnSkill( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	int nSkillID = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	if( !GameContent::GetSkillBase( nSkillID ) )
		return 0;

	int nLevel = pPlayer->GetBaseSkillLevel( nSkillID );

	int nJobId = 0;

	// 배울수 없으면 KIN(GM권한 있는 캐릭터의 경우는 검사 안함)
	int nErrorCode = RESULT_SUCCESS;

	if( pPlayer->GetPermission() == 0 )
		nErrorCode = pPlayer->IsLearnableSkill( nSkillID, nLevel+1, &nJobId );

	if( nErrorCode != RESULT_SUCCESS )
	{
		return 0;
	}
	
	pPlayer->RegisterSkill( nSkillID, nLevel+1, 0, nJobId );

	lua_pushnumber( L, nLevel+1 );
	return 1;
}

/*ServerPill*/
int SCRIPT_LearnAllSkill( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 1 );
		return 1;
	}

	__int64 nExp = GameContent::GetNeedExp( GameRule::MAX_LEVEL );
	pPlayer->SetEXP( nExp );

	while( GameContent::GetNeedJpForJobLevelUp( pPlayer->GetJobLevel(), pPlayer->GetJobDepth()) )
	{
		pPlayer->SetJobLevel( pPlayer->GetJobLevel() + 1 );
		pPlayer->CalculateStat();
	}

	if( !GameContent::LearnAllSkill( static_cast< StructCreature* >( pPlayer )) ) 
	{
		lua_pushnumber( L, 2 );
		return 1;
	}

	lua_pushnumber( L, 0 );
	return 1;
}

/*ServerPill*/
int SCRIPT_LearnCreatureAllSkill( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);

	StructPlayer::iterator pit = getPlayer( L, (nArgCnt) ? 2 : 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 1 );
		return 1;
	}

	__int64 nExp			= 0;
	int nSummonSlotIndex	= 0;
	struct StructSummon *pSummon = NULL;

	if( !lua_gettop(L) || !lua_tonumber( L, 1) )
	{
		for( nSummonSlotIndex = 0; nSummonSlotIndex < 6; ++nSummonSlotIndex )
		{
			pSummon = pPlayer->GetSummonAt( nSummonSlotIndex );
			if( !pSummon ) continue;

			switch( pSummon->GetTransformLevel() )
			{
			case SummonBase::EVOLVE_NORMAL:
				nExp = GameContent::GetNeedSummonExp( GameRule::NORMAL_SUMMON_MAX_LEVEL - 1 );
				break;
			case SummonBase::EVOLVE_GROWTH:
				nExp = GameContent::GetNeedSummonExp( GameRule::GROWTH_SUMMON_MAX_LEVEL - 1 );
				break;
			case SummonBase::EVOLVE_EVOLVE:
				nExp = GameContent::GetNeedSummonExp( GameRule::EVOLVE_SUMMON_MAX_LEVEL - 1 );
			}

			pSummon->SetEXP( nExp );
			if( !GameContent::LearnAllSkill( static_cast< StructCreature* >( pSummon )) )
			{
				lua_pushnumber( L, 2 );
				return 1;
			}
		}
	}
	else
	{
		nSummonSlotIndex = lua_tonumber( L, 1 );
		if( nSummonSlotIndex < 0 || nSummonSlotIndex > 6 )
		{
			lua_pushnumber( L, 3 );
			return 1;
		}
		
		pSummon = pPlayer->GetSummonAt( --nSummonSlotIndex );
		if( !pSummon )
		{
			lua_pushnumber( L, 4 );
			return 1;
		}
		
		switch( pSummon->GetTransformLevel() )
		{
		case SummonBase::EVOLVE_NORMAL:
			nExp = GameContent::GetNeedSummonExp( GameRule::NORMAL_SUMMON_MAX_LEVEL - 1 );
			break;
		case SummonBase::EVOLVE_GROWTH:
			nExp = GameContent::GetNeedSummonExp( GameRule::GROWTH_SUMMON_MAX_LEVEL - 1 );
			break;
		case SummonBase::EVOLVE_EVOLVE:
			nExp = GameContent::GetNeedSummonExp( GameRule::EVOLVE_SUMMON_MAX_LEVEL - 1 );
		}

		pSummon->SetEXP( nExp );
		if( !GameContent::LearnAllSkill( static_cast< StructCreature* >( pSummon )) )
		{
			lua_pushnumber( L, 5 );
			return 1;
		}
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_CreatureLearnSkill( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	int nSkillID = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	AR_HANDLE handle = (AR_HANDLE) lua_tonumber( L, 2 );

	StructSummon * pSummon = pPlayer->GetSummon( handle );

	if( !pSummon )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nLevel = pSummon->GetBaseSkillLevel( nSkillID );
	
	pSummon->RegisterSkill( nSkillID, nLevel+1 );

	lua_pushnumber( L, nLevel+1 );
	return 1;
}

int SCRIPT_ResetSkill( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( pPlayer == NULL || lua_isnumber( L, 1 ) == 0 )
	{
		lua_pushnumber( L, 0 );
	}
	else
	{
		const bool result = pPlayer->ResetSkill( StructCreature::SRM_SCRIPT, lua_tonumber( L, 1 ) );
		lua_pushnumber( L, result == true ? 1 : 0 );
	}	
	
	return 1;
}

int SCRIPT_SetJob( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}
	else
	{
		const bool result = pPlayer->ChangeJob( StructCreature::SRM_SCRIPT, lua_tonumber( L, 1 ) );
		lua_pushnumber( L, result );
	}

	return 1;
}

int SCRIPT_ResetJob( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( pPlayer == NULL || lua_isnumber( L, 1 ) == 0 )
	{
		lua_pushnumber( L, 0 );
	}
	else
	{
		const bool result = pPlayer->ResetJob( lua_tonumber( L, 1 ) );
		lua_pushnumber( L, result == true ? 1 : 0 );
	}	
	
	return 1;
}

int SCRIPT_SetRace( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( pPlayer == NULL || !lua_isnumber( L, 1 ) )
	{
		lua_pushnumber( L, 0 );
	}
	else
	{
		const bool result = pPlayer->SetRace( StructCreature::SRM_SCRIPT, lua_tonumber( L, 1 ) );
		lua_pushnumber( L, result );
	}	
	
	return 1;
}

int SCRIPT_Message( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);

	if( nArgCnt < 1 ) return 0;

	StructPlayer *pPlayer = GetCurrentThreadPlayer();

	std::string szText = lua_tostring_utf8( L, nArgCnt );
	if( pPlayer )
	{
		// 플레이어에게 결과 전송
		SendChatMessage( false, CHAT_NPC, "@SCRIPT", pPlayer, szText.c_str(), static_cast< unsigned int >( szText.size() ) );
	}

	return 0;
}

int SCRIPT_IsChangeableJob( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( pPlayer == NULL || !lua_isnumber( L, 1 ) )
	{
		lua_pushnumber( L, 0 );
	}
	else
	{
		const bool result = pPlayer->IsChangeableJob( lua_tonumber( L, 1 ) );
		lua_pushnumber( L, result );
	}	
	
	return 1;
}

int SCRIPT_AddState( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 3 ) return 0;

	int nStateID = lua_tonumber( L, 1 );
	int nLevel = lua_tonumber( L, 2 );
	int nTime = lua_tonumber( L, 3 );

	StructCreature *pTarget = NULL;
	if( nArgCnt == 4 && lua_isnumber( L, 4 ) )
	{
		pTarget = static_cast< StructCreature * >( StructCreature::raw_get( lua_tonumber( L, 4 ) ) );
		if( !pTarget )
			return 0;
	}
	else
	{
		StructPlayer::iterator pit = getPlayer( L, 4 );
		pTarget = static_cast< StructCreature *>( *pit );
	}

	if( !pTarget || !pTarget->IsCreature() ) return 0;

	AR_TIME t = GetArTime();
	pTarget->AddState( (StructState::StateCode)nStateID, pTarget->GetHandle(), nLevel, t, t + nTime );

	return 0;
}

int SCRIPT_GetStateLevel( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	int nStateID = lua_tonumber( L, 1 );

	StructCreature *pTarget = NULL;
	if( nArgCnt == 2 && lua_isnumber( L, 2 ) )
	{
		pTarget = static_cast< StructCreature *>( StructCreature::raw_get( lua_tonumber( L, 2 ) ) );
		if( !pTarget )
			return 0;
	}
	else
	{
		StructPlayer::iterator pit = getPlayer( L, 2 );
		pTarget = static_cast< StructCreature * >( *pit );
	}

	if( !pTarget || !pTarget->IsCreature() )
	{
		return 0;
	}

	StructState *pState = pTarget->GetState( static_cast< StructState::StateCode >( nStateID ) );

	if( !pState )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, pState->GetLevel() );
	return 1;
}

int SCRIPT_RemoveState( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	int nStateID = lua_tonumber( L, 1 );
	int nLevel = lua_tonumber( L, 2 );

	StructCreature *pTarget = NULL;
	if( nArgCnt == 3 && lua_isnumber( L, 3 ) )
	{
		pTarget = static_cast< StructCreature *>( StructCreature::raw_get( lua_tonumber( L, 3 ) ) );
		if( !pTarget )
			return 0;
	}
	else
	{
		StructPlayer::iterator pit = getPlayer( L, 3 );
		pTarget = static_cast< StructCreature * >( *pit );
	}

	if( !pTarget || !pTarget->IsCreature() ) return 0;

	AR_TIME t = GetArTime();
	pTarget->RemoveState( (StructState::StateCode)nStateID, nLevel );

	return 0;
}

int SCRIPT_AddCreatureState( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 3 ) return 0;

	int nStateID = lua_tonumber( L, 1 );
	int nLevel = lua_tonumber( L, 2 );
	int nTime = lua_tonumber( L, 3 );

	StructPlayer::iterator pit = getPlayer( L, 4 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	StructSummon *pTarget = pPlayer->GetMainSummon();

	if( !pTarget ) return 0;

	AR_TIME t = GetArTime();
	pTarget->AddState( (StructState::StateCode)nStateID, pTarget->GetHandle(), nLevel, t, t + nTime );

	return 0;
}

int SCRIPT_RemoveCreatureState( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	int nStateID = lua_tonumber( L, 1 );
	int nLevel = lua_tonumber( L, 2 );

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	StructSummon *pTarget = pPlayer->GetMainSummon();

	if( !pTarget ) return 0;

	AR_TIME t = GetArTime();
	pTarget->RemoveState( (StructState::StateCode)nStateID, nLevel );

	return 0;
}

int SCRIPT_AddEventState( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 )
	{
		lua_pushstring( L, "SCRIPT_AddEventState() : invalid argument" );
		LUA()->Log( "SCRIPT_AddEventState() : invalid argument" );
		return 1;
	}

	int nStateID = lua_tonumber( L, 1 );
	int nLevel = lua_tonumber( L, 2 );
	int nStateValue = ( nArgCnt >= 3 ) ? lua_tonumber( L, 3 ) : 0;
	std::string strStateValue = ( nArgCnt >= 4 ) ? lua_tostring_utf8( L, 4 ) : "";

	if( StructPlayer::AddEventState( static_cast< StructState::StateCode >( nStateID ), nLevel, nStateValue, strStateValue.c_str() ) )
	{
		// GameContent::GetStateInfo( nStateID )가 NULL이 아님은 StructPlayer::AddEventState의 리턴값이 true 라는 것이 보장함
		PrintfGlobalChatMessage( CHAT_NOTICE, "@NOTICE", "@444\v#@skill_name@#\v@%d", GameContent::GetStateInfo( nStateID )->name_id );

		LUA()->Log( "Event state added successfully." );
		lua_pushnumber( L, 1 );
	}
	else
	{
		LUA()->Log( "Adding an event state is failed." );
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_RemoveEventState( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "SCRIPT_RemoveEventState() : invalid argument" );
		LUA()->Log( "SCRIPT_RemoveEventState() : invalid argument" );
		return 1;
	}

	int nStateID = lua_tonumber( L, 1 );

	if( StructPlayer::RemoveEventState( static_cast< StructState::StateCode >( nStateID ) ) )
	{
		PrintfGlobalChatMessage( CHAT_NOTICE, "@NOTICE", "@445\v#@skill_name@#\v@%d", GameContent::GetStateInfo( nStateID )->name_id );

		LUA()->Log( "Event state removed successfully." );
		lua_pushnumber( L, 1 );
	}
	else
	{
		LUA()->Log( "Removing an event state is failed." );
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_ClearEventState( struct lua_State *L )
{
	std::vector< StructPlayer::EVENT_STATE > vEventStateList;

	StructPlayer::GetEventStateList( vEventStateList );

	for( std::vector< StructPlayer::EVENT_STATE >::const_iterator it = vEventStateList.begin() ; it != vEventStateList.end() ; ++it )
	{
		PrintfGlobalChatMessage( CHAT_NOTICE, "@NOTICE", "@445\v#@skill_name@#\v@%d", GameContent::GetStateInfo( (*it).eCode )->name_id );
	}

	StructPlayer::ClearEventState();

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_GetEventStateList( struct lua_State *L )
{
	std::vector< StructPlayer::EVENT_STATE > vEventStateList;

	StructPlayer::GetEventStateList( vEventStateList );

	std::string strResult;
	char szBuffer[ 22 ];

	for( std::vector< StructPlayer::EVENT_STATE >::const_iterator it = vEventStateList.begin() ; it != vEventStateList.end() ; ++it )
	{
		s_sprintf( szBuffer, _countof( szBuffer ), "%d %d\n", (*it).eCode, (*it).nLevel );
		strResult += szBuffer;
	}

	lua_pushstring( L, strResult.c_str() );
	return 1;
}

int SCRIPT_CastWorldState( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	StructPlayer *pPlayer = NULL;

	if( !( nArgCnt == 3 || nArgCnt == 4 ) || !( pPlayer = *getPlayer( L, nArgCnt + 1 ) ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nStateID = lua_tonumber( L, 1 );
	int nLevel = lua_tonumber( L, 2 );
	int nTime = lua_tonumber( L, 3 );	
	int nCooltime = ( nArgCnt == 4 ) ? lua_tonumber( L, 4 ) : nTime + 6000;

	bool result = pPlayer->PendWorldState( (StructState::StateCode)nStateID, nLevel, nTime, nCooltime, 0 );

	lua_pushnumber( L, result ? 1 : 0 );

	return 1;
}

int SCRIPT_Kick( struct lua_State *L )
{	
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	if( pPlayer->pConnection && pPlayer->pConnection->IsConnected() )
	{
		SendDisconnectDesc( pPlayer->pConnection, TS_SC_DISCONNECT_DESC::DISCONNECT_TYPE_SCRIPT );
		pPlayer->pConnection->Close();
	}
	else
	{
		pPlayer->LogoutNowWithAccount( 5 );
	}

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_ForceUnregisterAccount( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);

	if( nArgCnt < 1 )
	{
		lua_pushnumber( L, 1 );
		return 1;
	}

	std::string strAccount = lua_tostring_utf8( L, 1 );

	if( !StructPlayer::IsRegisteredAccount( strAccount.c_str() ) )
	{
		lua_pushnumber( L, 2 );
		return 1;
	}

	StructPlayer::UnRegisterAccount( strAccount.c_str() );
	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_GetCreatureHandle( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	int index = lua_tonumber( L, nArgCnt );

	StructSummon *pSummon = NULL;

	if( index < 0 || index > 6 ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	pSummon = pPlayer->GetSummonAt( index );

	if( !pSummon ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, pSummon->GetSummonSID() );
	return 1;
}


int SCRIPT_GetCreatureValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	std::string szKey = lua_tostring_utf8( L, 2 );
	int index = lua_tonumber( L, 1 );

	StructSummon *pSummon = pPlayer->GetSummon( index );

	if( !pSummon )
	{
		pSummon = pPlayer->GetSummon( (AR_HANDLE) index );

		if( !pSummon )
		{
			lua_pushnumber( L, 0 );
			return 1;
		}
	}

	std::string strValue = pSummon->GetAsString( szKey.c_str() );
	int n = atoi( strValue.c_str() );
	if( strcmp( szKey.c_str(), "name" ) && ( n || ( strValue.size() == 1 && *strValue.c_str() =='0' ) ) )
	{
		lua_pushnumber( L, n );
	}
	else
	{
		lua_pushstring_utf8( L, strValue.c_str() );
	}

	return 1;
}

int SCRIPT_GetMainCreatureValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	std::string szKey = lua_tostring_utf8( L, 1 );

	StructSummon *pSummon = pPlayer->GetMainSummon();
	if( !pSummon || !pSummon->IsInWorld() )
	{
		return 0;
	}

	std::string strValue = pSummon->GetAsString( szKey.c_str() );
	int n = atoi( strValue.c_str() );
	if( n || ( strValue.size() == 1 && *strValue.c_str() == '0' ) )
	{
		lua_pushnumber( L, n );
	}
	else
	{
		lua_pushstring_utf8( L, strValue.c_str() );
	}

	return 1;
}

int SCRIPT_EchoMainCreatureValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer *pPlayer = NULL, *pTarget = NULL;

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	pPlayer = *pit;

	if( nArgCnt == 1 )
	{
		pTarget = pPlayer;
	}
	else if( nArgCnt == 2 )
	{
		pit = getPlayer( L, 2 );
		pTarget = *pit;
	}
	else
		return 0;

	if( !pPlayer || !pTarget || !pTarget->GetMainSummon() || !pTarget->GetMainSummon()->IsInWorld() )
		return 0;

	std::string szKey = lua_tostring_utf8( L, 1 );

	std::string strValue = pTarget->GetMainSummon()->GetAsString( szKey.c_str() );

	int n = atoi( strValue.c_str() );

	char szBuf[1024];

	if( _stricmp( szKey.c_str(), "exp" ) == 0 )
	{
		__int64 n64 = _atoi64( strValue.c_str() );
		s_sprintf( szBuf, _countof( szBuf ), "<BR>* %s: %d<BR>", szKey.c_str(), n64 );
	}
	else if( n || ( strValue.size() == 1 && *strValue.c_str() == '0' ) )
		s_sprintf( szBuf, _countof( szBuf ), "<BR>* %s: %d<BR>", szKey.c_str(), n );
	else
		s_sprintf( szBuf, _countof( szBuf ), "<BR>* %s: %s<BR>", szKey.c_str(), strValue.c_str() );

	SendChatMessage( false, CHAT_EXP, "@SYSTEM", pPlayer, szBuf );

	return 0;
}

int SCRIPT_GetSummonNameId( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	int index = lua_tonumber( L, 1 );

	StructSummon *pSummon = pPlayer->GetSummon( index );

	if( !pSummon )
	{
		pSummon = pPlayer->GetSummon( (AR_HANDLE) index );

		if( !pSummon )
		{
			lua_pushnumber( L, 0 );
			return 1;
		}
	}

	lua_pushnumber( L, pSummon->GetSummonNameId() );

	return 1;
}

int SCRIPT_ResetSummonSkill( struct lua_State *L )
{
	// 1.Handle 2.(생략가능)force_reset 3.(생략가능)PlayerName
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit;
	bool force_reset = false;
	if( nArgCnt >= 2 )
	{
		// 2번째 인자는 int값이지만 bool 처럼 씀.
		if( lua_tonumber( L, 2 ) )
			force_reset = true;
		pit = getPlayer( L, 3 );
	}
	else
	{
		pit = getPlayer( L, 2 );
	}

	StructPlayer *pPlayer = *pit;
	
	if( !pPlayer ) return 0;

	int index = lua_tonumber( L, 1 );

	StructSummon *pSummon = pPlayer->GetSummon( index );

	if( !pSummon ) 
	{
		pSummon = pPlayer->GetSummon( (AR_HANDLE) index );

		if( !pSummon || pSummon->GetMaster() != pPlayer )
		{
			return 0;
		}
	}

	if( pSummon->ResetSkill( StructCreature::SRM_SCRIPT, 0, force_reset ) )
	{
		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_ShowCreatureFarmWindow( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	pPlayer->SetDialogTitle( "CreatureFarm", TS_SC_DIALOG::TYPE_CREATURE_FARM_WINDOW );
	pPlayer->ShowDialog();

	return 0;
}

int SCRIPT_GetSummonList( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt != 0 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetSummonList() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid player" );
		LUA()->Log( "SCRIPT_GetSummonList() : invalid player" );
		return 0;
	}

	lua_newtable( L );
	StructInventory *pInventory = pPlayer->GetInventory();

	int count = 0;

	for( int i = 0; i < static_cast< int >( pInventory->GetCount() ); ++i )
	{
		StructItem *pItem = pInventory->Get( i );

		// 봉인된 소환수 카드이면서 아직 농장에 맡기지 않은 아이템
		// 기간제이거나 내구도가 다 한 소환수는 맡길 수 없음
		if(	!pItem->IsSummonCard() || !pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON ) || pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FARMED_SUMMON ) || pItem->IsExpireItem() || ( !pItem->GetCurrentEtherealDurability() && pItem->GetMaxEtherealDurability() ) )
			continue;
		
		// 한 번도 편성되지 않음
		StructSummon *pSummon = pItem->GetSummonStruct();
		if( !pSummon ) continue;

		// 레벨 및 진화 정도
		int nTransformLevel = pSummon->GetTransformLevel();
		if(	pSummon->GetLevel() >= GameRule::FARM_MAX_LEVEL || nTransformLevel == SummonBase::EVOLVE_EVOLVE || pSummon->GetLevel() >= pPlayer->GetLevel()
			|| ( nTransformLevel == SummonBase::EVOLVE_NORMAL && pSummon->GetLevel() >= GameRule::NORMAL_SUMMON_MAX_LEVEL )
			|| ( nTransformLevel == SummonBase::EVOLVE_GROWTH && pSummon->GetLevel() >= GameRule::GROWTH_SUMMON_MAX_LEVEL ) )
			continue;

		// 소환된 소환수
		if( pSummon->IsInWorld() ) continue;

		// 편성되어 있거나 벨트 장착 중인 소환수 카드
		bool check = false;
		for( int nSummonIdx = 0 ; nSummonIdx < 6 ; ++nSummonIdx )
			if( pPlayer->GetSummonCardAt( nSummonIdx ) == pItem || pPlayer->GetBeltSlotCardAt( nSummonIdx ) == pItem )
			{
				check = true;
				break;
			}
		if( check ) continue;

		lua_pushnumber( L, ++count );
		lua_pushnumber( L, pItem->GetHandle() );
		lua_rawset( L, -3 );
	}

	lua_pushstring( L, "n" );
	lua_pushnumber( L, count );
	lua_rawset( L, -3 );

	return 1;
}

int SCRIPT_GetFarmedSummonList( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt != 0 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetFarmedSummonList() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid player" );
		LUA()->Log( "SCRIPT_GetFarmedSummonList() : invalid player" );
		return 1;
	}

	lua_newtable( L );

	int count = 0;
	for( int i = 0; i < GameRule::FARM_MAX_COUNT; ++i )
	{
		StructPlayer::FARMED_SUMMON_INFO *pInfo = pPlayer->GetFarmedSummonInfo( i );
		if( !pInfo || pInfo->registration_time + pInfo->duration < time( NULL ) )
		{
			pPlayer->RegainSummon( pInfo->item->GetHandle() );
			continue;
		}

		lua_pushnumber( L, i+1 );
		lua_pushnumber( L, pInfo->item->GetHandle() );
		lua_rawset( L, -3 );

		++count;
	}

	lua_pushstring( L, "n" );
	lua_pushnumber( L, count );
	lua_rawset( L, -3 );

	return 1;
}

int SCRIPT_GetFarmedSummonCount( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt != 0 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetFarmedSummonCount() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid player" );
		LUA()->Log( "SCRIPT_GetFarmedSummonCount() : invalid player" );
		return 1;
	}

	lua_pushnumber( L, pPlayer->GetFarmedSummonCount() );

	return 1;
}

int SCRIPT_GetFarmedSummonValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	AR_HANDLE hCardHandle = lua_tonumber( L, 1 );
	std::string szKey = lua_tostring_utf8( L, 2 );
	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructItem *pCard = StructItem::FindItem( hCardHandle );
	StructSummon *pSummon = pCard ? pCard->GetSummonStruct() : 0;
	StructPlayer *pPlayer = *pit;

	if( !pCard || !pSummon || !pPlayer ) 
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	std::string strValue = pSummon->GetAsString( szKey.c_str() );
	int n = atoi( szKey.c_str() );
	if( strcmp( szKey.c_str(), "name" ) && ( n || ( strValue.size() == 1 && *strValue.c_str() == '0' ) ) )
	{
		lua_pushnumber( L, n );
	}
	else
	{
		lua_pushstring_utf8( L, strValue.c_str() );
	}
	
	return 1;
}

int SCRIPT_GetFarmedSummonLevel( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	AR_HANDLE hCardHandle = lua_tonumber( L, 1 );
	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid player" );
		LUA()->Log( "SCRIPT_GetFarmedSummonLevel() : invalid player" );
		return 1;
	}

	int nLevel = pPlayer->GetFarmedSummonLevel( hCardHandle );
	lua_pushnumber( L, nLevel );
	
	return 1;
}

int SCRIPT_GetSummonRate( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetSummonRate() : invalid argument" );
		return 1;
	}

	int sid = lua_tonumber( L, 1 );
	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid player" );
		LUA()->Log( "SCRIPT_GetSummonRate() : invalid player" );
		return 1;
	}

	StructSummon *pSummon = pPlayer->GetSummon( sid );
	
	if( !pSummon )
	{
		lua_pushstring( L, "invalid summon" );
		LUA()->Log( "SCRIPT_GetSummonRate() : invalid summon" );
		return 1;
	}

	lua_pushnumber( L, pSummon->GetRate() );

	return 1;
}

int SCRIPT_FarmSummon( lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 3 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_FarmSummon() : invalid argument" );
		return 1;
	}

	int nSlot = lua_tonumber( L, 1 );
	AR_HANDLE hCardHandle = lua_tonumber( L, 2 );
	int nDuration = lua_tonumber( L, 3 );
	bool bUseCracker = lua_tonumber( L, 4 );
	bool bCash = lua_tonumber( L, 5 );
	StructPlayer::iterator pit = getPlayer( L, 6 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid player" );
		LUA()->Log( "SCRIPT_FarmSummon() : invalid player" );
		return 1;
	}

	if( pPlayer->FarmSummon( nSlot-1, hCardHandle, nDuration, bUseCracker, bCash ) )
	{
		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_RegainSummon( lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_RegainSummon() : invalid argument" );
		return 1;
	}

	AR_HANDLE hCardHandle = lua_tonumber( L, 1 );
	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid player" );
		LUA()->Log( "SCRIPT_RegainSummon() : invalid player" );
		return 1;
	}

	if( pPlayer->RegainSummon( hCardHandle ) )
	{
		lua_pushnumber( L, 1);
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_NurseSummon( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_NurseSummon() : invalid argument" );
		return 1;
	}

	AR_HANDLE hCardHandle = lua_tonumber( L, 1 );
	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid player" );
		LUA()->Log( "SCRIPT_NurseSummon() : invalid player" );
		return 1;
	}

	if( pPlayer->NurseSummon( hCardHandle ) )
	{
		lua_pushnumber( L, 1);
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_GetMainTitle( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_GetMainTitle() : invalid name" );
		return 0;
	}

	const TitleBaseServer *pTitle = pPlayer->GetMainTitle();
	lua_pushnumber( L, pTitle ? pTitle->nID : 0 );

	return 1;
}

int SCRIPT_SetMainTitle( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_SetMainTitle() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_SetMainTitle() : invalid name" );
		return 0;
	}

	pPlayer->SetMainTitle( lua_tonumber( L, 1 ) );

	return 0;
}

int SCRIPT_GetSubTitle( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_GetSubTitle() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_GetSubTitle() : invalid name" );
		return 0;
	}

	const TitleBaseServer *pTitle = pPlayer->GetSubTitle( lua_tonumber( L, 1 ) );
	lua_pushnumber( L, pTitle ? pTitle->nID : 0 );

	return 1;
}

int SCRIPT_SetSubTitle( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		LUA()->Log( "SCRIPT_SetSubTitle() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_SetSubTitle() : invalid name" );
		return 0;
	}

	pPlayer->SetSubTitle( lua_tonumber( L, 1 ), lua_tonumber( L, 2 ) );

	return 0;
}

int SCRIPT_OpenTitle( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_OpenTitle() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_OpenTitle() : invalid name" );
		return 0;
	}

	pPlayer->OpenTitle( lua_tonumber( L, 1 ) );

	return 0;
}

int SCRIPT_AchieveTitle( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_AchieveTitle() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_AchieveTitle() : invalid name" );
		return 0;
	}

	pPlayer->AchieveTitle( lua_tonumber( L, 1 ) );

	return 0;
}

int SCRIPT_UpdateTitleCondition( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		LUA()->Log( "SCRIPT_UpdateTitleCondition() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_UpdateTitleCondition() : invalid name" );
		return 0;
	}

	pPlayer->UpdateTitleCondition( lua_tonumber( L, 1 ), lua_tonumber( L, 2 ) );

	return 0;
}

int SCRIPT_SetRemainTitleTime( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1) )
	{
		LUA()->Log( "SCRIPT_SetRemainTitleTime() : invalid name" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit; 

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_SetRemainTitleTime() : invalid name" );
		return 0;
	}

	pPlayer->SetRemainTitleTime( GetArTime() + lua_tonumber( L, 1 ) );
	SendRemainTitleTime( pPlayer );

	return 0;
}

int SCRIPT_CreatureEvolution( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	int index = lua_tonumber( L, 1 );

	StructSummon *pSummon = pPlayer->GetSummon( index );

	if( !pSummon ) 
	{
		pSummon = pPlayer->GetSummon( (AR_HANDLE) index );

		if( !pSummon )
		{
			return 0;
		}
	}

	if( pSummon->DoEvolution() )
	{
		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_CreatureEnhance( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	int index = lua_tonumber( L, 1 );
	int level = lua_tonumber( L, 2 );

	StructSummon *pSummon = pPlayer->GetSummonAt( index );
	
	if( !pSummon )
		return 0;
	
	StructItem *pItem = pSummon->GetParentCard();
	
	if( !pItem )
		return 0;

	std::string strItemEnhance;

	XStringUtil::Format( strItemEnhance, "set_item_enhance( %u, %d )", pItem->GetHandle(), level );

	LUA()->RunString( strItemEnhance.c_str() );

	return 0;
}

int SCRIPT_ChangeCreatureName( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	std::string szName = lua_tostring_utf8( L, 2 );
	int index = lua_tonumber( L, 1 );

	StructSummon *pSummon = pPlayer->GetSummon( index );

	if( !pSummon )
	{
		pSummon = pPlayer->GetSummon( (AR_HANDLE) index );

		if( !pSummon )
		{
			return 0;
		}
	}

	LOG::Log11N4S( LM_SUMMON_CHANGE_NAME, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), pSummon->GetSID(), 1, 0, 0, 0, 0, 0, 0, pSummon->GetParentCard()->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, pSummon->GetName(), LOG::STR_NTS, szName.c_str(), LOG::STR_NTS );

	pSummon->SetName( szName.c_str() );

	return 0;
}

int SCRIPT_SetCreatureValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 3 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 4 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	std::string szKey = lua_tostring_utf8( L, 2 );
	std::string szData = lua_tostring_utf8( L, 3 );
	int index = lua_tonumber( L, 1 );

	StructSummon *pSummon = pPlayer->GetSummon( index );

	if( !pSummon )
	{
		pSummon = pPlayer->GetSummon( (AR_HANDLE) index );

		if( !pSummon )
		{
			return 0;
		}
	}


	pSummon->SetAsString( szKey.c_str(), szData.c_str() );
	
	SendPropertyMessage( pPlayer, pSummon->GetHandle(), szKey.c_str(), szData.c_str() );

	return 0;
}

int SCRIPT_SetMainCreatureValue( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	std::string szKey = lua_tostring_utf8( L, 1 );
	std::string szData = lua_tostring_utf8( L, 2 );

	StructSummon *pSummon = pPlayer->GetMainSummon();

	if( !pSummon || !pSummon->IsInWorld() )
	{
		return 0;
	}

	pSummon->SetAsString( szKey.c_str(), szData.c_str() );
	SendPropertyMessage( pPlayer, pSummon->GetHandle(), szKey.c_str(), szData.c_str() );

	return 0;
}

int SCRIPT_GetUserCountNear( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	AR_UNIT x, y;
	x = y = 0;
	unsigned char layer;

	if( nArgCnt < 3 )
	{
		return 0;
	}
	else
	{
		x = lua_tonumber( L, nArgCnt - 2 );
		y = lua_tonumber( L, nArgCnt - 1 );
		layer = lua_tonumber( L, nArgCnt );
	}

	std::vector< AR_HANDLE > vResult;
	ArcadiaServer::Instance().EnumMovableObject( ArPosition( x, y, 0 ), layer, 0xffff, &vResult, true, false );

	lua_pushnumber( L, vResult.size() );

	return 1;
}

int SCRIPT_EquipSummonCard( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_EquipSummonCard() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_EquipSummonCard() : invalid name" );
		return 1;
	}

	int nCreatureControlLevel = pPlayer->GetCurrentPassiveSkillLevel( StructSkill::SKILL_CREATURE_CONTROL );
	if( nCreatureControlLevel > 6 )
		nCreatureControlLevel = 6;

	int index = lua_tonumber( L, 1 );
	int card_handle = lua_tonumber( L, 2 );

	if( index < 0 || index >= nCreatureControlLevel )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_EquipSummonCard() : invalid argument" );
		return 1;
	}

	AR_HANDLE pCardHandle[6];
	memset( pCardHandle, 0, sizeof( pCardHandle ) );
	for( int i = 0; i < nCreatureControlLevel; ++i )
	{
		StructItem *pItem = pPlayer->GetSummonCardAt( i );

		if( pItem )
			pCardHandle[i] = pItem->GetHandle();
	}

	pCardHandle[index] = card_handle;

	pPlayer->EquipSummon( pCardHandle );

	return 0;
}

int SCRIPT_ShowChannelSet( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	pPlayer->SetSpecialDialogMenu( "on_channel_set" );

	SendWindowMessage( pPlayer, "number_input_window", pPlayer->GetName(), "on_channel_set" );

	return 0;
}

int SCRIPT_DialogGeneral( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DialogGeneral() : invalid argument" );
		return 1;
	}
	
	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_DialogGeneral() : invalid name" );
		return 1;
	}

	SendGeneralMessageBox( pPlayer, lua_tostring_utf8( L, 1 ).c_str() );

	return 0;
}

int SCRIPT_DialogSpecial( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DialogSpecial() : invalid argument" );
		return 1;
	}
	
	StructPlayer::iterator pit = getPlayer( L, 4 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_DialogSpecial() : invalid name" );
		return 1;
	}

	pPlayer->SetSpecialDialogMenu( lua_tostring_utf8( L, 2 ).c_str() );
	pPlayer->SetNonNPCDialog( true );

	SendWindowMessage( pPlayer, lua_tostring_utf8( L, 1 ).c_str(), lua_tostring_utf8( L, 3 ).c_str(), lua_tostring_utf8( L, 2 ).c_str() );

	return 0;
}

int SCRIPT_EnterSecretDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_EnterSecretDungeon() : invalid argument" );
		return 1;
	}

	int secret_dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_EnterSecretDungeon() : invalid name" );
		return 1;
	}

	pPlayer->SetSpecialDialogMenu( "warp_to_secret_dungeon" );
	pPlayer->SetNonNPCDialog( true );

	// 입장 확인 팝업을 띄움
	char buf[12];
	s_sprintf( buf, _countof( buf ), "%d", secret_dungeon_id );

	SendWindowMessage( pPlayer, "secret_dungeon_confirm_window", buf, "warp_to_secret_dungeon" );

	return 0;
}

int SCRIPT_WarpToSecretDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_WarpToSecretDungeon() : invalid argument" );
		return 1;
	}

	int secret_dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_WarpToSecretDungeon() : invalid name" );
		return 1;
	}

	// 숨던도 던전리소스를 사용하여 입장 검사를 한다.
	if( DungeonManager::Instance().IsRestrictedToEnter( secret_dungeon_id, pPlayer ) )
	{
		PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@563\v#@player_name@#\v%s", pPlayer->GetName() );

		lua_pushstring( L, "level limit" );
		return 1;
	}

	ArPosition secretDungeonPosition = DungeonManager::Instance().GetRaidStartPosition( secret_dungeon_id );

	pPlayer->SetInvincible( true );
	pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );
	pPlayer->PendWarp( secretDungeonPosition.GetX(), secretDungeonPosition.GetY(), 0 );

	ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	return 0;
}

int SCRIPT_EnterInstanceDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_EnterInstanceDungeon() : invalid argument" );
		return 1;
	}

	int instance_dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_EnterInstanceDungeon() : invalid name" );
		return 1;
	}

	pPlayer->SetSpecialDialogMenu( "warp_to_instance_dungeon" );
	pPlayer->SetNonNPCDialog( true );

	// 입장 확인 팝업을 띄움
	char buf[12];
	s_sprintf( buf, _countof( buf ), "%d", instance_dungeon_id );

	SendWindowMessage( pPlayer, "instance_dungeon_confirm_window", buf, "warp_to_instance_dungeon" );

	return 0;
}

int SCRIPT_EnterOtherInstanceDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 5 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isnumber( L, 3 ) || !lua_isnumber( L, 4 ) || !lua_isnumber( L, 5 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_EnterOtherInstanceDungeon() : invalid argument" );
		return 1;
	}

	int instance_dungeon_id = lua_tonumber( L, 1 );
	int current_floor = lua_tonumber( L, 2 );
	int next_floor = lua_tonumber( L, 3 );
	int current_floor_item_count = lua_tonumber( L, 4 );
	int next_floor_item_count = lua_tonumber( L, 5 );

	StructPlayer::iterator pit = getPlayer( L, 6 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_EnterOtherInstanceDungeon() : invalid name" );
		return 1;
	}

	pPlayer->SetDialogTitle( "InDun", TS_SC_DIALOG::TYPE_OTHER_INSTANCE_DUNGEON_CONFIRM_WINDOW );
	pPlayer->SetDialogText( "Warp" );

	std::string strTitle, strTrigger;
	XStringUtil::Format( strTitle, "%d", current_floor_item_count );
	XStringUtil::Format( strTrigger, "warp_indun(%d,%d)", instance_dungeon_id, current_floor );
	pPlayer->AddDialogMenu( strTitle.c_str(), strTrigger.c_str() );
	XStringUtil::Format( strTitle, "%d", next_floor_item_count );
	XStringUtil::Format( strTrigger, "warp_indun(%d,%d)", instance_dungeon_id, next_floor );
	pPlayer->AddDialogMenu( strTitle.c_str(), strTrigger.c_str() );

	pPlayer->ShowDialog();

	return 0;
}

int SCRIPT_SpecialMenu(struct lua_State* L)
{
	int nArgCnt = lua_gettop(L);

	// dlg_special_menu( '@9525', 'Warp', menu1, trigger1, menu2, trigger2, menu3, trigger3)
	// dlg_special_menu( '@170126024', 'Warp',menu2, 'vulcanus_warp_confirm(' .. nxt .. ')' )
	// dlg_special_menu( '@170126024', 'Warp', menu1, 'vulcanus_warp_confirm(' .. cur .. ')', menu2, 'vulcanus_warp_confirm(' .. nxt .. ')' )
	//dlg_special_menu( '@90610912' , 'War record', menu[1], "siege_board(1)", menu[2], "siege_board(2)", menu[3], "siege_board(3)" , menu[4], "siege_board(4)",menu[5],"siege_board(5)" ) 

	const std::string& strTitle = lua_tostring_utf8(L, 1);
	const std::string& strDialogMenu = lua_tostring_utf8(L, 2);

	StructPlayer::iterator pit = getPlayer(L, nArgCnt+1);
	StructPlayer* pPlayer = *pit;

	if (!pPlayer)
	{
		lua_pushstring_utf8(L, "invalid name");
		LUA()->Log("SCRIPT_SpecialMenu() : invalid name");
		return 1;
	}

	pPlayer->SetDialogTitle(strTitle.c_str(), TS_SC_DIALOG::TYPE_SELECT);
	pPlayer->SetDialogText(strDialogMenu.c_str());

	if (nArgCnt > 3)
	{
		const std::string& strButton = lua_tostring_utf8(L, 3);
		const std::string& strTrigger = lua_tostring_utf8(L, 4);

		pPlayer->AddDialogMenu(strButton.c_str(), strTrigger.c_str());
	}

	if (nArgCnt > 5)
	{
		const std::string& strButton = lua_tostring_utf8(L, 5);
		const std::string& strTrigger = lua_tostring_utf8(L, 6);

		pPlayer->AddDialogMenu(strButton.c_str(), strTrigger.c_str());
	}

	if (nArgCnt > 7)
	{
		const std::string& strButton = lua_tostring_utf8(L, 7);
		const std::string& strTrigger = lua_tostring_utf8(L, 8);

		pPlayer->AddDialogMenu(strButton.c_str(), strTrigger.c_str());
	}

	if (nArgCnt > 9)
	{
		const std::string& strButton = lua_tostring_utf8(L, 9);
		const std::string& strTrigger = lua_tostring_utf8(L, 10);

		pPlayer->AddDialogMenu(strButton.c_str(), strTrigger.c_str());
	}

	if (nArgCnt > 11)
	{
		const std::string& strButton = lua_tostring_utf8(L, 11);
		const std::string& strTrigger = lua_tostring_utf8(L, 12);

		pPlayer->AddDialogMenu(strButton.c_str(), strTrigger.c_str());
	}

	pPlayer->ShowDialog();

	return 0;
}

int SCRIPT_WarpToInstanceDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_WarpToInstanceDungeon() : invalid argument" );
		return 1;
	}

	int nArgNum = 1;

	int instance_dungeon_id = lua_tonumber( L, nArgNum );
	++nArgNum;

	int instance_dungeon_type = -1;
	if( nArgCnt >= nArgNum && lua_isnumber( L, nArgNum ) )
	{
		instance_dungeon_type = lua_tonumber( L, nArgNum );
		++nArgNum;
	}

	float posX = 0;
	float posY = 0;

	if (nArgCnt > 3)
	{
		posX = lua_tonumber(L, 3);
		posY = lua_tonumber(L, 4);
	}

	
	StructPlayer::iterator pit = getPlayer( L, nArgCnt+1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_WarpToInstanceDungeon() : invalid name" );
		return 1;
	}


	ArPosition pos;
	
	if( InstanceDungeonManager::Instance().GetInstanceDungeonPosition( instance_dungeon_id, pos ) )
	{
		if (nArgCnt > 3)
		{
			pos.x = posX;
			pos.y = posY;
		}

		// 이미 배틀 아레나 경기에 유저가 참여된 상태라면 인던 진입 불가
		if( pPlayer->GetBattleArenaInstanceNo() )
		{
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@2431" );

			lua_pushnumber( L, 1 );
			return 1;
		}

		// 배틀 아레나 대기열에 대기된 상태면 강제로 대기열에서 이탈시킴
		if( pPlayer->GetBattleArenaID() )
		{
			unsigned short nResult = BattleArenaManager::Instance().QuitGame( pPlayer, false, false, ALT_ENTER_INSTANCE_DUNGEON );

			// nResult는 RESULT_SUCCESS가 아니지만 다른 쪽에서 대기열 이탈 처리를 시켜줬으면
			// 아레나 ID가 리셋되었을 수도 있으므로 아레나 ID도 함께 체크(지역락 때문에 일반적으로 있을 수 없지만 -_ -;;)
			// 어쨌든 대기열 이탈 처리가 실패했으면 인던 진입 캔슬
			if( nResult != RESULT_SUCCESS && pPlayer->GetBattleArenaID() )
			{
				SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@2431" );

				lua_pushnumber( L, 1 );
				return 1;
			}
		}

		pPlayer->PendWarp( pos.x, pos.y, 0, instance_dungeon_type, nArgCnt > 3 ? 2 : 0);	
	}

	return 0;
}

int SCRIPT_LeaveInstanceDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_LeaveInstanceDungeon() : invalid argument" );
		return 1;
	}

	int instance_dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_LeaveInstanceDungeon() : invalid name" );
		return 1;
	}

	pPlayer->SetSpecialDialogMenu( "exit_indun" );
	pPlayer->SetNonNPCDialog( true );

	// 퇴장 확인 팝업을 띄움
	char buf[12];
	s_sprintf( buf, _countof( buf ), "%d", instance_dungeon_id );

	SendWindowMessage( pPlayer, "instance_dungeon_confirm_window2", buf, "exit_indun" );

	return 0;
}

int SCRIPT_QuitInstanceDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_QuitInstanceDungeon() : invalid argument" );
		return 1;
	}

	int instance_dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_QuitInstanceDungeon() : invalid name" );
		return 1;
	}

	InstanceDungeonManager::Instance().QuitInstanceDungeon( instance_dungeon_id, pPlayer );

	return 0;
}

int SCRIPT_GetInstanceDungeonID( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_GetInstanceDungeonID() : invalid name" );
		return 1;
	}

	int nInstanceDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( pPlayer->GetPos() );

	lua_pushnumber( L, nInstanceDungeonID );
	return 1;
}

int SCRIPT_SendMissionTitle( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isstring( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SendMissionTitle() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid name" );
		LUA()->Log( "SCRIPT_SendMissionTitle() : invalid name" );
		return 1;
	}

	const std::string & strTitle = lua_tostring_utf8( L, 1 );

	if( strTitle.find( '|' ) != std::string::npos )
	{
		lua_pushstring( L, "invalid title" );
		LUA()->Log( "SCRIPT_SendMissionTitle() : invalid title" );
		return 1;
	}

	char szBuffer[ 1024 ];
	s_sprintf( szBuffer, _countof( szBuffer ), "MTITLE|%s|", strTitle.c_str() );

	SendChatMessage( false, CHAT_DUNGEON_SYSTEM, "@DUNGEON", pPlayer, szBuffer );

	return 0;
}

int SCRIPT_SendMissionReward( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isstring( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SendMissionReward() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid name" );
		LUA()->Log( "SCRIPT_SendMissionReward() : invalid name" );
		return 1;
	}

	const std::string & strReward = lua_tostring_utf8( L, 1 );

	if( strReward.find( '|' ) != std::string::npos )
	{
		lua_pushstring( L, "invalid reward" );
		LUA()->Log( "SCRIPT_SendMissionReward() : invalid reward" );
		return 1;
	}

	char szBuffer[ 1024 ];
	s_sprintf( szBuffer, _countof( szBuffer ), "MREWARD|%s|", strReward.c_str() );

	SendChatMessage( false, CHAT_DUNGEON_SYSTEM, "@DUNGEON", pPlayer, szBuffer );

	return 0;
}

int SCRIPT_SendMissionObjective( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isstring( L, 3 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SendMissionObjective() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 4 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid name" );
		LUA()->Log( "SCRIPT_SendMissionObjective() : invalid name" );
		return 1;
	}

	int nObjectiveIndex = (int)lua_tonumber( L, 1 );
	int nMaxProgress = (int)lua_tonumber( L, 2 );
	const std::string & strObjective = lua_tostring_utf8( L, 3 );

	if( nObjectiveIndex < 0 || nObjectiveIndex > InstanceDungeonManager::MAX_OBJECTIVE_INDEX || strObjective.find( '|' ) != std::string::npos )
	{
		lua_pushstring( L, "invalid value" );
		LUA()->Log( "SCRIPT_SendMissionObjective() : invalid value" );
		return 1;
	}

	char szBuffer[ 1024 ];
	s_sprintf( szBuffer, _countof( szBuffer ), "MOBJECTIVE|%d|%d|%s|", nObjectiveIndex, nMaxProgress, strObjective.c_str() );

	SendChatMessage( false, CHAT_DUNGEON_SYSTEM, "@DUNGEON", pPlayer, szBuffer );

	return 0;
}

int SCRIPT_SendMissionObjectiveProgress( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SendMissionObjectiveProgress() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid name" );
		LUA()->Log( "SCRIPT_SendMissionObjectiveProgress() : invalid name" );
		return 1;
	}

	int nObjectiveIndex = (int)lua_tonumber( L, 1 );
	int nMaxProgress = (int)lua_tonumber( L, 2 );

	if( nObjectiveIndex < 0 || nObjectiveIndex > InstanceDungeonManager::MAX_OBJECTIVE_INDEX )
	{
		lua_pushstring( L, "invalid value" );
		LUA()->Log( "SCRIPT_SendMissionObjectiveProgress() : invalid value" );
		return 1;
	}

	char szBuffer[ 1024 ];
	s_sprintf( szBuffer, _countof( szBuffer ), "MPROGRESS|%d|%d|", nObjectiveIndex, nMaxProgress );

	SendChatMessage( false, CHAT_DUNGEON_SYSTEM, "@DUNGEON", pPlayer, szBuffer );

	return 0;
}

int SCRIPT_WarpToDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_WarpToDungeon() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_WarpToDungeon() : invalid name" );
		return 1;
	}

	std::vector< std::string > vClosedDungeons;
	XStringUtil::Split( ENV().GetString( "game.closed_dungeons", "" ).c_str(), vClosedDungeons, "|" );

	for( std::vector< std::string >::iterator it = vClosedDungeons.begin() ; it != vClosedDungeons.end() ; ++it )
	{
		if( atoi( (*it).c_str() ) == dungeon_id )
		{
			lua_pushstring( L, "dungeon closed" );
			//LUA()->Log( "SCRIPT_WarpToDungeon() : dungeon closed" );
			return 1;
		}
	}

	if( DungeonManager::Instance().IsRestrictedToEnter( dungeon_id, pPlayer ) )
	{
		PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@563\v#@player_name@#\v%s", pPlayer->GetName() );

		lua_pushstring( L, "level limit" );
		return 1;
	}

	AR_UNIT x, y;
	unsigned char layer = 0;

	ArPosition startPosition = DungeonManager::Instance().GetRaidStartPosition( dungeon_id );

	x = startPosition.GetX();
	y = startPosition.GetY();

	const int skyFortressStart[] = { 20325, 108660, 28195, 108735, 27960, 101010, 20380, 100950 };
	if (dungeon_id == 10600)
	{
		x = skyFortressStart[(GameRule::nSkyFortressStartIndex*2)] + XRandom(-50,50);
		y = skyFortressStart[(GameRule::nSkyFortressStartIndex * 2)+1] + XRandom(-50, 50);
	}

	do
	{
		int nPartyID = pPlayer->GetPartyID();
		if( !nPartyID )
			break;

		if( !PartyManager::GetInstance().IsAttackTeamParty( nPartyID ) )
			break;

		int guild_id = PartyManager::GetInstance().GetAttackTeamGuildID( nPartyID );

		if( GuildManager::GetInstance().GetRaidDungeonID( guild_id ) != dungeon_id )
			break;

		if( DungeonManager::Instance().IsEnterableSiegeDungeon( dungeon_id, guild_id ) )
		{
			if( PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_SIEGE_ATTACKTEAM )
				break;

			if( DungeonManager::Instance().GetOwnGuildID( dungeon_id ) == guild_id )
			{
				ArPosition defencePosition = DungeonManager::Instance().GetSiegeDefencePosition( dungeon_id );

				x = defencePosition.GetX();
				y = defencePosition.GetY();
			}
			else
			{
				ArPosition startPosition = DungeonManager::Instance().GetSiegeStartPosition( dungeon_id );

				x = startPosition.GetX();
				y = startPosition.GetY();
			}
			
			layer = DungeonManager::DUNGEON_SIEGE_LAYER;
		}
		else if( DungeonManager::Instance().IsDungeonRaidTime( dungeon_id ) )
		{
			if( PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_RAID_ATTACKTEAM )
				break;

			layer = DungeonManager::Instance().GetRaidDungeonLayer( dungeon_id, guild_id );

			if( !layer )
			{
				if( !DungeonManager::Instance().IsRaidBegin( dungeon_id, guild_id ) && DungeonManager::Instance().GetOwnGuildID( dungeon_id ) != guild_id &&
					PartyManager::GetInstance().GetLinkedPartyLeadPartyID( nPartyID ) == nPartyID &&
					PartyManager::GetInstance().IsLeader( nPartyID, pPlayer->GetPlayerUID() ) )
				{
					pPlayer->SetSpecialDialogMenu( "begin_dungeon_raid" );
					pPlayer->SetNonNPCDialog( true );

					// 입장을 하는게 아니라 입장 확인 팝업을 띄움
					SendWindowMessage( pPlayer, "dungeon_raid_confirm_window", pPlayer->GetName(), "begin_dungeon_raid" );

					return 0;
				}
			}
		}

	} while( false );

	if( layer >= MAX_LAYER || layer < 0 )
	{
		layer = 0;
	}

	pPlayer->SetInvincible( true );
	pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );
	pPlayer->PendWarp( x, y, layer );

	ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	return 0;
}

int SCRIPT_IsEnterableSiegeDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_Warp() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	if( DungeonManager::Instance().IsEnterableSiegeDungeon( dungeon_id, pPlayer->GetGuildID() ) )
	{
		lua_pushnumber( L, 1 );
		return 1;
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_BeginDungeonRaid( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_BeginDungeonRaid() : invalid name" );
		return 0;
	}

	int party_id = pPlayer->GetPartyID();

	if( !party_id )
		return 0;

	if( !PartyManager::GetInstance().IsAttackTeamParty( party_id ) )
		return 0;

	int guild_id = PartyManager::GetInstance().GetAttackTeamGuildID( party_id );

	if( !guild_id )
		return 0;

	int dungeon_id = GuildManager::GetInstance().GetRaidDungeonID( guild_id );

	if( !dungeon_id )
		return 0;

	if( !DungeonManager::Instance().IsDungeonRaidTime( dungeon_id ) )
		return 0;

	if( PartyManager::GetInstance().GetPartyType( party_id ) != PartyManager::TYPE_RAID_ATTACKTEAM )
		return 0;

	// 리더 파티의 파티장 진입할 때만 레이드 시작 가능
	party_id = PartyManager::GetInstance().GetLinkedPartyLeadPartyID( party_id );
	if( !PartyManager::GetInstance().IsLeader( party_id, pPlayer->GetPlayerUID() ) )
		return 0;

	if( !DungeonManager::Instance().IsRaidBegin( dungeon_id, guild_id ) )
	{
		LOG::Log11N4S( LM_BEGIN_DUNGEON_RAID, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, guild_id, 0, 0, 0, 0, 0, 0, dungeon_id, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
		DungeonManager::Instance().BeginDungeonRaid( dungeon_id, guild_id );
	}

	return 0;
}

int SCRIPT_WarpToStoredPosition( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring_utf8( L, "invalid name" );
		LUA()->Log( "SCRIPT_WarpToStoredPosition() : invalid name" );
		return 1;
	}

	ArPosition pos;
	pPlayer->GetLastTownPosition( &pos );

	unsigned char layer = 0;

	int current_channel = ChannelManager::GetChannelId( pPlayer->GetX(), pPlayer->GetY() );
	int target_channel = ChannelManager::GetChannelId( pos.x, pos.y );

	if( current_channel && current_channel == target_channel )
	{
		layer = pPlayer->GetLayer();
	}
	else if( target_channel )
	{
		layer = ChannelManager::GetProperLayer( pos.x, pos.y );
	}

	pPlayer->SetInvincible( true );
	pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );
	pPlayer->PendWarp( pos.x, pos.y, layer );

	ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
	
	return 0;
}

int SCRIPT_GetOwnGuildName( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 )  )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_GetOwnGuildName() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );

	int owner_guild_id = DungeonManager::Instance().GetOwnGuildID( dungeon_id );

	std::string strName;

	if( owner_guild_id )
	{
		strName = GuildManager::GetInstance().GetGuildName( owner_guild_id );
	}
	else
	{
		strName = GameContent::GetString( 633 );
	}

	lua_pushstring_utf8( L, strName.c_str() );

	return 1;
}

int SCRIPT_GetDungeonRelation( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 )  )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetDungeonRelation() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetDungeonRelation() : invalid name" );
		return 1;
	}

	int guild_id = pPlayer->GetGuildID();
	int alliance_id = GuildManager::GetInstance().GetAllianceID( guild_id );

	if( !guild_id )
	{
		lua_pushnumber( L, 0 );
	}
	else
	{
		int owner_guild_id = DungeonManager::Instance().GetOwnGuildID( dungeon_id );
		int raid_guild_id = DungeonManager::Instance().GetRaidGuildID( dungeon_id );

		// 던전 소유 길드원일 경우
		if( owner_guild_id && guild_id == owner_guild_id )
		{
			if( GuildManager::GetInstance().IsLeader( guild_id, pPlayer->GetPlayerUID() ) )
			{
				lua_pushnumber( L, 1 );
			}
			else if( GuildManager::GetInstance().IsPermitted( guild_id, pPlayer->GetGuildPermission(), GuildManager::PRA_DUNGEON_MANAGEMENT ) )
			{
				lua_pushnumber( L, 9 );
			}
			else
			{
				lua_pushnumber( L, 2 );
			}
		}
		// 던전 소유 길드원은 아니지만 같은 연합 소속 길드원일 경우
		else if( alliance_id && GuildManager::GetInstance().GetAllianceLeaderGuildID( alliance_id ) == owner_guild_id )
		{
			if( GuildManager::GetInstance().IsLeader( guild_id, pPlayer->GetPlayerUID() ) )
			{
				lua_pushnumber( L, 3 );
			}
			else
			{
				lua_pushnumber( L, 4 );
			}
		}
		// 던전 레이드 종료 시점부터 던전 시즈 종료 전까지의 던전 시즈 공격자 길드원일 경우
		else if( raid_guild_id && guild_id == raid_guild_id )
		{
			if( GuildManager::GetInstance().IsLeader( guild_id, pPlayer->GetPlayerUID() ) )
			{
				lua_pushnumber( L, 5 );
			}
			else
			{
				lua_pushnumber( L, 6 );
			}
		}
		// 던전 레이드 종료 시점부터 던전 시즈 종료 전까지의 던전 시즈 공격자 길드원은 아니지만 같은 연합 소속 길드원일 경우
		else if( alliance_id && GuildManager::GetInstance().GetAllianceLeaderGuildID( alliance_id ) == raid_guild_id )
		{
			if( GuildManager::GetInstance().IsLeader( guild_id, pPlayer->GetPlayerUID() ) )
			{
				lua_pushnumber( L, 7 );
			}
			else
			{
				lua_pushnumber( L, 8 );
			}
		}
		// 던전이랑 관계 없는 길드원일 경우
		else
		{
			lua_pushnumber( L, 0 );
		}
	}

	return 1;
}

int SCRIPT_SetTaxRate( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetTaxRate() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );
	int tax_rate = lua_tonumber( L, 2 );

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_SetTaxRate() : invalid name" );
		return 0;
	}

	if( tax_rate < 1 || tax_rate > 10 )
		return 0;

	if( !pPlayer->GetGuildID() )
	{
		return 0;
	}

	int owner_guild_id = DungeonManager::Instance().GetOwnGuildID( dungeon_id );

	if( !owner_guild_id || pPlayer->GetGuildID() != owner_guild_id )
		return 0;

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( owner_guild_id, pPlayer->GetGuildPermission(), GuildManager::PRA_DUNGEON_MANAGEMENT ) )
		return 0;

	DungeonManager::Instance().SetTaxRate( dungeon_id, tax_rate );

	LOG::Log11N4S( LM_SET_DUNGEON_TAX_RATE, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, pPlayer->GetGuildID(), 0, 0, 0, 0, 0, tax_rate, dungeon_id, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

	return 0;
}

int SCRIPT_GetTaxAmount( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetTaxAmount() : invalid name" );
		return 1;
	}

	if( !pPlayer->GetGuildID() )
	{
		lua_pushnumber( L, 0 );

		return 1;
	}

	__int64 tax_amount = GuildManager::GetInstance().GetTaxAmount( pPlayer->GetGuildID() ).GetRawData();

	lua_pushnumber( L, tax_amount );

	return 1;
}

int SCRIPT_GetTaxChaosAmount( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetTaxAmount() : invalid name" );
		return 1;
	}

	if( !pPlayer->GetGuildID() )
	{
		lua_pushnumber( L, 0 );

		return 1;
	}

	int tax_amount = GuildManager::GetInstance().GetTaxChaosAmount( pPlayer->GetGuildID() );

	lua_pushnumber( L, tax_amount );

	return 1;
}

int SCRIPT_DrawTaxChaos( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_DrawTaxChaos() : invalid name" );
		return 1;
	}

	if( !pPlayer->GetGuildID() )
	{
		lua_pushnumber( L, 0 );

		return 1;
	}

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( pPlayer->GetGuildID(), pPlayer->GetGuildPermission(), GuildManager::PRA_DUNGEON_MANAGEMENT ) )
	{
		lua_pushnumber( L, 0 );

		return 1;
	}

	int chaos = pPlayer->GetMaxChaos() - pPlayer->GetChaos();

	int tax_amount = GuildManager::GetInstance().DrawTaxChaos( pPlayer->GetGuildID(), chaos );

	pPlayer->AddChaos( tax_amount );

	pPlayer->Save( true );

	LOG::Log11N4S( LM_DRAW_DUNGEON_TAX, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, pPlayer->GetGuildID(), 0, 0, pPlayer->GetGold().GetRawData(), 0, pPlayer->GetChaos(), tax_amount, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	lua_pushnumber( L, tax_amount );

	return 1;
}

int SCRIPT_DrawTax( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetTaxAmount() : invalid name" );
		return 1;
	}

	int nGuildID = pPlayer->GetGuildID();

	if( !nGuildID )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pPlayer->GetGuildPermission(), GuildManager::PRA_DUNGEON_MANAGEMENT ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 여기서 GetTaxAmount로 미리 검사 한 번 하지만 아래에서 DrawTax 할 때까지 증가될 수 있음
	if( pPlayer->GetGold() + GuildManager::GetInstance().GetTaxAmount( nGuildID ) > GameRule::MAX_GOLD_FOR_INVENTORY )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructGold tax_amount( GuildManager::GetInstance().DrawTax( nGuildID ) );

	if( pPlayer->ChangeGold( pPlayer->GetGold() + tax_amount ) != RESULT_SUCCESS )
	{
		// DrawTax에 의해 감소된 금액 다시 채워주기(닝기미 -,. -)
		GuildManager::GetInstance().GiveTax( nGuildID, tax_amount, 0 );
		lua_pushnumber( L, 0 );
		return 1;
	}

	pPlayer->Save( true );

	LOG::Log11N4S( LM_DRAW_DUNGEON_TAX, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, pPlayer->GetGuildID(), 0, 0, pPlayer->GetGold().GetRawData(), tax_amount.GetRawData(), pPlayer->GetChaos(), 0, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	lua_pushnumber( L, tax_amount.GetRawData() );

	return 1;
}

int SCRIPT_GetTaxRate( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 )  )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetTaxRate() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );

	int tax_rate = 0;
	int own_guild_id = 0;

	DungeonManager::Instance().GetTaxRate( dungeon_id, &tax_rate, &own_guild_id );

	lua_pushnumber( L, tax_rate );

	return 1;
}

int SCRIPT_DropDungeonOwnership( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_DropDungeonOwnership() : invalid name" );
		return 1;
	}

	int nGuildID = pPlayer->GetGuildID();

	if( !nGuildID )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pPlayer->GetGuildPermission(), GuildManager::PRA_DUNGEON_MANAGEMENT ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nGuildID );

	if( !nDungeonID || DungeonManager::Instance().GetOwnGuildID( nDungeonID ) != nGuildID )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	if( !DungeonManager::Instance().DropDungeonOwnership( nDungeonID ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	char szBuf[ 512 ];
	s_sprintf( szBuf, _countof( szBuf ), "GINFO|%d|%s|%s|%d|%s|%d|0|%s",
				nGuildID,
				GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(),
				StructPlayer::GetPlayerName( GuildManager::GetInstance().GetLeaderSID( nGuildID ) ),
				GuildManager::GetInstance().GetMemberCount( nGuildID ),
				GuildManager::GetInstance().GetGuildNotice( nGuildID ).c_str(),
				GuildManager::GetInstance().IsRecruitingGuild( nGuildID ),
				GuildManager::GetInstance().GetGuildURL( nGuildID ).c_str() );

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );
	if( nAllianceID )
	{
		PrintfAllianceChatMessage( CHAT_GUILD_SYSTEM, nAllianceID, "@GUILD", szBuf );
	}
	else
	{
		SendGuildChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", nGuildID, szBuf );
	}

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_GetOwnDungeonID( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetOwnDungeonID() : invalid name" );
		return 1;
	}

	int guild_id = pPlayer->GetGuildID();
	if( !guild_id )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int alliance_id = GuildManager::GetInstance().GetAllianceID( guild_id );
	if( alliance_id )
	{
		guild_id = GuildManager::GetInstance().GetAllianceLeaderGuildID( alliance_id );
	}

	int dungeon_id = GuildManager::GetInstance().GetRaidDungeonID( guild_id );

	if( !dungeon_id || DungeonManager::Instance().GetOwnGuildID( dungeon_id ) != guild_id )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, dungeon_id );

	return 1;
}

int SCRIPT_GetSiegeDungeonID( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetSiegeDungeonID() : invalid name" );
		return 1;
	}

	int guild_id = pPlayer->GetGuildID();
	if( !guild_id )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int alliance_id = GuildManager::GetInstance().GetAllianceID( guild_id );
	if( alliance_id )
	{
		guild_id = GuildManager::GetInstance().GetAllianceLeaderGuildID( alliance_id );
	}

	int dungeon_id = GuildManager::GetInstance().GetRaidDungeonID( guild_id );

	if( !dungeon_id || DungeonManager::Instance().GetRaidGuildID( dungeon_id ) != guild_id )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, dungeon_id );

	return 1;
}

int SCRIPT_ChangeDungeonOwner( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_ChangeDungeonOwner() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );
	int owner_guild_id = lua_tonumber( L, 2 );

	if( !DungeonManager::Instance().SetOwnGuildID( dungeon_id, owner_guild_id ) )
	{
		lua_pushstring( L, "changing dungeon owner guild failed" );
		LUA()->Log( "SCRIPT_ChangeDungeonOwner() : changing dungeon owner guild failed" );
		return 1;
	}

	return 0;
}

int SCRIPT_ClearDungeonCoreGuardian( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_ClearDungeonCoreGuardian() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );

	if( !DungeonManager::Instance().IsSiegeBegin( dungeon_id ) || !DungeonManager::Instance().IsCoreInvincible( dungeon_id ) )
	{
		lua_pushstring( L, "no guardian to clear" );
		return 1;
	}

	DungeonManager::Instance().ClearDungeonSiegeCoreGuardian( dungeon_id );
	return 0;
}

int SCRIPT_ChangeTacticalPositionOwner( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_ChangeTacticalPositionOwner() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );
	int tactical_position_id = lua_tonumber( L, 2 );

	DungeonManager::Instance().ChangeOwner( dungeon_id, tactical_position_id );

	return 0;
}

int SCRIPT_ShowDungeonStone( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_ShowDungeonStone() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_ShowDungeonStone() : invalid name" );
		return 1;
	}

	std::vector< std::string > vClosedDungeons;
	XStringUtil::Split( ENV().GetString( "game.closed_dungeons", "" ).c_str(), vClosedDungeons, "|" );

	for( std::vector< std::string >::iterator it = vClosedDungeons.begin() ; it != vClosedDungeons.end() ; ++it )
	{
		if( atoi( (*it).c_str() ) == dungeon_id )
		{
			lua_pushstring( L, "dungeon closed" );
			//LUA()->Log( "SCRIPT_ShowDungeonStone() : dungeon closed" );
			return 1;
		}
	}

	int own_guild_id = DungeonManager::Instance().GetOwnGuildID( dungeon_id );
	int tax_rate = 0;

	DungeonManager::Instance().GetTaxRate( dungeon_id, &tax_rate, &own_guild_id );

	struct _myGuildListFunctor : public GuildManager::GuildListFunctor
	{
		_myGuildListFunctor( int _dungeon_id, int _own_guild_id ) : dungeon_id( _dungeon_id ), dungeon_own_guild_id( _own_guild_id )
		{
		}

		virtual bool operator()( int nGuildID, const char * szGuildName, const char * szGuildLeaderName, int nGuildLeaderLevel, int nGuildMemberCount, int nRaidDungeonID )
		{
			GuildManager& guildManager = GuildManager::GetInstance();
			int allianceID = guildManager.GetAllianceID( nGuildID );

			std::string guildName; 
			std::string guildLeaderName;
			int guildMemberCount;
			int isAlliance;

			if( allianceID )
			{
				guildName = guildManager.GetAllianceName( allianceID );
				guildLeaderName = StructPlayer::GetPlayerName( guildManager.GetLeaderSID( guildManager.GetAllianceLeaderGuildID( allianceID ) ) );
				guildMemberCount = guildManager.GetAllianceMemberPlayerCount( allianceID );
				isAlliance = 1;
			}
			else
			{
				guildName = szGuildName;
				guildLeaderName = szGuildLeaderName;
				guildMemberCount = nGuildMemberCount;
				isAlliance = 0;
			}

			if( ( dungeon_own_guild_id && nGuildID == dungeon_own_guild_id ) ||
				( dungeon_id == nRaidDungeonID ) )
			{
				char buf[255];
				s_sprintf( buf, _countof( buf ), "%s|%s|%d|%d|", guildName.c_str(), guildLeaderName.c_str(), isAlliance, guildMemberCount );

				vGuildID.push_back( nGuildID );
				vStrMsg.push_back( std::string( buf ) );
			}

			return true;
		}

		int dungeon_id;
		int dungeon_own_guild_id;
		std::vector< int > vGuildID;
		std::vector< std::string > vStrMsg;

	} _fo( dungeon_id, own_guild_id );

	GuildManager::GetInstance().DoEachGuild( _fo );

	// _DUNGEON_INFO::m_CS -> GuildManager::m_IntfLock 순서를 지키기 위해 레이드 기록은 따로 구한다.
	int raid_guild_count = 0;
	std::string strDungeonOwnMsg;
	std::string strMsg;

	size_t nCount = _fo.vGuildID.size();
	for( int i = 0; i < static_cast< int >( nCount ); ++i )
	{
		char buf[255];
		s_sprintf( buf, _countof( buf ), "%d|", DungeonManager::Instance().GetDungeonRaidRecord( dungeon_id, _fo.vGuildID[ i ] ) );

		if( _fo.vGuildID[ i ] == own_guild_id )
			strDungeonOwnMsg = _fo.vStrMsg[ i ] + buf;
		else
		{
			strMsg += _fo.vStrMsg[ i ] + buf;
			++raid_guild_count;
		}
	}

	char szTitle[255];
	char szRequestHandler[255] = { ' ', 0, };
	char szCancelHandler[255] = { ' ', 0, };
	char szManageHandler[255] = { ' ', 0, };

	int dungeon_status = DungeonManager::STATUS_IDLE;

	// 레이드 중
	if( DungeonManager::Instance().IsDungeonRaidTime( dungeon_id ) )
		dungeon_status = DungeonManager::STATUS_RAID;
	// 시즈 중
	else if( DungeonManager::Instance().IsSiegeBegin( dungeon_id ) )
		dungeon_status = DungeonManager::STATUS_SIEGE;
	// 시즈 중은 아니나 해당 주의 시즈가 끝나지 않았다면 시즈 시작 전까지 대기 상태이다.
	else if( !DungeonManager::Instance().IsEndSiegeOfThisWeek( dungeon_id ) )
		dungeon_status = DungeonManager::STATUS_REST_BEFORE_SIEGE;
	// 시즈 중은 아니나 해당 주의 시즈가 끝나 다음 주 레이드 전까지 대기 상태이다.
	else
		dungeon_status = DungeonManager::STATUS_REST_AFTER_SIEGE;

	s_sprintf( szTitle, _countof( szTitle ), "%d|%d|%d|%d|%s", dungeon_id, raid_guild_count, tax_rate, dungeon_status, strDungeonOwnMsg.c_str() );

	pPlayer->SetDialogTitle( szTitle, TS_SC_DIALOG::TYPE_DUNGEON_STONE );
	pPlayer->SetDialogText( strMsg.c_str() );

	// 길드 연합에 소속되어 있으면 nGuildID를 연합 리더 길드 ID로 바꿔서
	// 연합 리더 길드의 던전스톤과 동일하게 보이도록 함(리더 길드인지도 따로 가지고 있어야 함)
	int nGuildID = pPlayer->GetGuildID();
	// bIsRequestable: 연합일 경우 현재 유저의 길드가 연합 마스터 길드의 길드원이며 던전 관련 권한이 있는지를 체크, 연합이 아닐 경우 현재 길드에서 던전 관련 권한이 있는지를 체크
	// 현재 PRA_REQUEST_DUNGEON_RAID를 사용하지 않고 있기 때문에 PRA_ATTACK_TEAM_CREATE에 포함
	bool bIsRequestable = GuildManager::GetInstance().IsPermitted( nGuildID, pPlayer->GetGuildPermission(), static_cast< const GuildManager::_PERMIT_REQUIRED_ACTION >( GuildManager::PRA_REQUEST_DUNGEON_RAID | GuildManager::PRA_ATTACK_TEAM_CREATE ) );
	int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );
	if( nAllianceID )
	{
		nGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID );

		// 연합 리더 길드가 있었다면 자신이 연합 길드 길마가 아니면 신청 불가이므로 bIsRequestable를 false로 세팅
		if( nGuildID )
		{
			if( nGuildID != pPlayer->GetGuildID() )
			{
				bIsRequestable = false;
			}
		}
		else
		{
			nGuildID = pPlayer->GetGuildID();
		}
	}

	if( !GameRule::bDisableDungeonRaidSiege && nGuildID && bIsRequestable )
	{
		if( DungeonManager::Instance().GetOwnGuildID( dungeon_id ) == nGuildID )
		{
			s_sprintf( szManageHandler, _countof( szManageHandler ), " " );
		}

		else if( DungeonManager::Instance().IsDungeonRaidTime( dungeon_id ) )
		{
			int prev_raid_dungeon_id = GuildManager::GetInstance().GetRaidDungeonID( nGuildID );

			// 레이드 신청이 있음(길드 정보에 던전ID가 있음)
			if( prev_raid_dungeon_id )
			{
				// 현재 던전에 신청 상태이면
				if( prev_raid_dungeon_id == dungeon_id )
				{
					time_t raid_end_time = DungeonManager::Instance().GetLastRaidEndTime( dungeon_id, nGuildID );

					// 레이드를 신청했지만 아직 시작하지 않았고, 현재 한 주 동안 완료한 기록도 없으면 취소 가능
					if( !DungeonManager::Instance().IsRaidBegin( dungeon_id, nGuildID ) && !raid_end_time )
					{
						s_sprintf( szCancelHandler, _countof( szCancelHandler ), "request_dungeon_raid( 0 )" );
					}
				}
			}
			// 던전 레이드 최소 도전 레벨 체크 및 레이드 신청 블럭 기간 체크
			else if( DungeonManager::Instance().GetDungeonLevel( dungeon_id ) - GameRule::DUNGEON_RAID_REQUEST_LEVEL_LIMIT <= pPlayer->GetLevel() &&
				!GuildManager::GetInstance().GetDungeonBlockTime( nGuildID ) )
			{
				s_sprintf( szRequestHandler, _countof( szRequestHandler ), "request_dungeon_raid( %d )", dungeon_id );
			}
		}
	}

	pPlayer->AddDialogMenu( "REQUEST", szRequestHandler );
	pPlayer->AddDialogMenu( "CANCEL", szCancelHandler );
	pPlayer->AddDialogMenu( "MANAGE", szManageHandler );

	pPlayer->ShowDialog();

	return 0;
}

int SCRIPT_RespawnGuardian( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_RespawnGuardian() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );
	int tactical_position_id = lua_tonumber( L, 2 );

	DungeonManager::Instance().PendRespawnGuardian( dungeon_id, tactical_position_id );

	return 0;
}

int SCRIPT_IsInSiegeDungeon( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_IsInSiegeDungeon() : invalid name" );
		return 1;
	}

	int dungeon_id = DungeonManager::Instance().GetDungeonID( pPlayer->GetX(), pPlayer->GetY() );
	if( dungeon_id && pPlayer->GetLayer() == DungeonManager::DUNGEON_SIEGE_LAYER )
	{

		lua_pushnumber( L, dungeon_id );
		return 1;
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_RequestDungeonRaid( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_RequestDungeonRaid() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_RequestDungeonRaid() : invalid name" );
		return 1;
	}

	// 현재 PRA_REQUEST_DUNGEON_RAID를 사용하지 않고 있기 때문에 PRA_ATTACK_TEAM_CREATE에 포함
	if( !pPlayer->GetGuildID() || !GuildManager::GetInstance().IsPermitted( pPlayer->GetGuildID(), pPlayer->GetGuildPermission(), static_cast< const GuildManager::_PERMIT_REQUIRED_ACTION >( GuildManager::PRA_REQUEST_DUNGEON_RAID | GuildManager::PRA_ATTACK_TEAM_CREATE ) ) )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@636" );

		lua_pushnumber( L, 0 );
		return 1;
	}

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( pPlayer->GetGuildID() );
	if( nAllianceID && !GuildManager::GetInstance().IsAllianceLeader( nAllianceID, pPlayer->GetGuildID() ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int prev_raid_dungeon_id = GuildManager::GetInstance().GetRaidDungeonID( pPlayer->GetGuildID() );

	if( dungeon_id != 0 && prev_raid_dungeon_id && prev_raid_dungeon_id != dungeon_id )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@636" );

		lua_pushnumber( L, 0 );
		return 1;
	}

	if( dungeon_id != 0 && !DungeonManager::Instance().IsDungeonRaidTime( dungeon_id ) )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@636" );

		lua_pushnumber( L, 0 );
		return 1;
	}

	if( DungeonManager::Instance().IsRaidBegin( dungeon_id, pPlayer->GetGuildID() ) || DungeonManager::Instance().GetOwnGuildID( dungeon_id ) == pPlayer->GetGuildID() )
	{
		if( dungeon_id == 0 )
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@641" );
		else
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@636" );

		lua_pushnumber( L, 0 );
		return 1;
	}

	if( dungeon_id == 0 )
	{
		std::string strCancelDungeonRaid;

		if( PartyManager::GetInstance().IsExistAttackTeam( pPlayer->GetGuildID() ) )
		{
			XStringUtil::Format( strCancelDungeonRaid, "dlg_special(\"confirm_window\",\"cancel_dungeon_raid(%d)\",\"@12036\")", dungeon_id );
			LUA()->RunString( strCancelDungeonRaid.c_str() );
		}
		else
		{
			XStringUtil::Format( strCancelDungeonRaid, "cancel_dungeon_raid(%d)", dungeon_id );
			LUA()->RunString( strCancelDungeonRaid.c_str() );
		}

		lua_pushnumber( L, 0 );
		return 1;
	}

	bool bRet = GuildManager::GetInstance().SetRaidDungeonID( pPlayer->GetGuildID(), dungeon_id );
	
	if( !bRet )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@636" );

		lua_pushnumber( L, 0 );
		return 1;
	}

	PrintfGuildChatMessage( CHAT_GUILD, pPlayer->GetGuildID(), "@635\v#@guild_name@#\v%s\v#@dungeon_name@#\v@%d", GuildManager::GetInstance().GetGuildName( pPlayer->GetGuildID() ).c_str(), dungeon_id + 70000000 );
	
	LOG::Log11N4S( LM_REQUEST_DUNGEON_RAID, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, pPlayer->GetGuildID(), 0, 0, 0, 0, 0, 0, dungeon_id, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "REQUEST", LOG::STR_NTS );

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_CancelDungeonRaid( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_BeginDungeonRaid() : invalid argument" );
		return 1;
	}

	int dungeon_id = lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_CancelDungeonRaid() : invalid name" );
		return 1;
	}

	int prev_raid_dungeon_id = GuildManager::GetInstance().GetRaidDungeonID( pPlayer->GetGuildID() );

	int leader_party_id = PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( pPlayer->GetGuildID() );
	if( leader_party_id )
	{
		SendLinkedPartyChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", leader_party_id, "RAID_CANCEL|" );

		if( PartyManager::GetInstance().IsSomeoneInSiegeOrRaidDungeon( leader_party_id ) )
		{
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@641" );

			lua_pushnumber( L, 0 );
			return 1;
		}
		else
		{
			BroadcastPartyDestroy( leader_party_id );

			LOG::Log11N4S( LM_PARTY_DESTROY, pPlayer->GetAccountID(), pPlayer->GetSID(), leader_party_id, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), 4, 0, 0, 0, 0,
				pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( leader_party_id ).c_str(), LOG::STR_NTS, "", 0 );

			PartyManager::GetInstance().DestroyParty( leader_party_id );

			PrintfGuildChatMessage( CHAT_GUILD, pPlayer->GetGuildID(), "@12037\v#@user_name@#\v%s", pPlayer->GetName() );
		}
	}

	bool bRet = GuildManager::GetInstance().SetRaidDungeonID( pPlayer->GetGuildID(), dungeon_id );

	if( !bRet )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@641" );

		lua_pushnumber( L, 0 );
		return 1;
	}

	PrintfGuildChatMessage( CHAT_GUILD, pPlayer->GetGuildID(), "@640\v#@guild_name@#\v%s\v#@dungeon_name@#\v@%d", GuildManager::GetInstance().GetGuildName( pPlayer->GetGuildID() ).c_str(), prev_raid_dungeon_id + 70000000 );

	LOG::Log11N4S( LM_REQUEST_DUNGEON_RAID, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, pPlayer->GetGuildID(), 0, 0, 0, 0, 0, 0, dungeon_id, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "CANCEL", LOG::STR_NTS );
	lua_pushnumber( L, 0 );

	return 1;
}

int SCRIPT_SetHuntaholicPoint( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_SetHuntaholicPoint() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_SetHuntaholicPoint() : invalid name" );
		return 0;
	}

	int nHuntaholicPoint = lua_tonumber( L, 1 );

	if( nHuntaholicPoint > GameRule::HUNTAHOLIC_MAX_OWNABLE_POINT )
	{
		nHuntaholicPoint = GameRule::HUNTAHOLIC_MAX_OWNABLE_POINT;
	}

	bool bNeedLock = !ArcadiaServer::Instance().IsLocked( pPlayer->GetRX(), pPlayer->GetRY() );
	ArcadiaLock _lock( 0 );

	if( bNeedLock )
	{
		_lock = ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer );
	}

	pPlayer->SetHuntaholicPoint( nHuntaholicPoint );

	if( bNeedLock )
	{
		ArcadiaServer::Instance().UnLock( &_lock );
	}

	return 0;
}

int SCRIPT_WarpToRevivePosition( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_WarpToRevivePosition() : invalid name" );
		return 0;
	}

	if( pPlayer->IsInBattleField() )
		return 0;

	ArPosition revive_pos;

	pPlayer->GetLastTownPosition( &revive_pos );

	unsigned char layer = 0;

	ArPosition pos = pPlayer->GetPos();

	int current_channel = ChannelManager::GetChannelId( pos.x, pos.y );
	int target_channel = ChannelManager::GetChannelId( revive_pos.x, revive_pos.y );

	int dungeon_id = DungeonManager::Instance().GetDungeonID( pos.x, pos.y );

	// 던전 밖에서 죽었을 때 또는 일반 던전 안에서 죽었을 때 수련자의 섬 같이 유저수 제한 채널에서 부활해야할 경우
	if( ( !dungeon_id || !pPlayer->GetLayer() ) && target_channel && ChannelManager::GetChannelType( target_channel ) == ChannelManager::TYPE_USER_LIMIT )
	{
		if( current_channel == target_channel )
		{
			// 그 동네가 그 동네면 현재 레이어 사용
			layer = pPlayer->GetLayer();
		}
		else
		{
			// 아니면 새로 받고...
			layer = ChannelManager::GetProperLayer( revive_pos.x, revive_pos.y );
		}
	}
	else
	{
		do
		{
			if( pPlayer->GetLayer() == 0 )
				break;

			if( !dungeon_id )
				break;

			int nPartyID = pPlayer->GetPartyID();

			if( !nPartyID )
				break;

			// 던전 안쪽
			int nGuildID = PartyManager::GetInstance().GetAttackTeamGuildID( nPartyID );

			if( !nGuildID )
			{	// 나가
				break;
			}

			if( pPlayer->GetLayer() != DungeonManager::DUNGEON_SIEGE_LAYER )
			{	// 레이드 던전이므로 시작위치로 이동
				revive_pos = DungeonManager::Instance().GetRaidStartPosition( dungeon_id );
				layer = pPlayer->GetLayer();

				break;
			}
			// 시즈다
			ArPosition nearest_pos = DungeonManager::Instance().GetNearestTacticalPosition( dungeon_id, nGuildID, pos );

			if( nearest_pos.x && nearest_pos.y )
			{
				// 제어장치랑 겹치지 않도록 랜덤 방향으로 4미터의 오차를 줌
				float fAngle = 3.14159265358979323846264f * ( (float)XRandom( 0, 359 ) ) / 180.0f;
				nearest_pos.x += GameRule::TACTICAL_POSITION_REBIRTH_ERROR * sin( fAngle );
				nearest_pos.y += GameRule::TACTICAL_POSITION_REBIRTH_ERROR * cos( fAngle );

				revive_pos = nearest_pos;
				layer = pPlayer->GetLayer();
				break;
			}
		} while( false );
	}

	pPlayer->PendWarp( revive_pos.x, revive_pos.y, layer );
	pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );
	ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	return 0;
}

int SCRIPT_RecallFeather( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isnumber( L, 3 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_RecallFeather() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 4 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_RecallFeather() : invalid name" );
		return 0;
	}

	int x = lua_tonumber( L, 1 );
	int y = lua_tonumber( L, 2 );
	int layer = lua_tonumber( L, 3 );

	pPlayer->PendWarp( x, y, layer );
	pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );
	ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	return 0;
}

int SCRIPT_SetAutoUser( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer *pPlayer = NULL;
	if( nArgCnt <= 2 && lua_isnumber( L, 2 ) )
	{
		StructCreature *pTarget = static_cast< StructPlayer *>( StructPlayer::raw_get( lua_tonumber( L, 2 ) ) );

		if( pTarget && pTarget->IsPlayer() )
			pPlayer = static_cast< StructPlayer * >(pTarget);
	}
	else
	{
		StructPlayer::iterator pit = getPlayer( L, 2 );
		pPlayer = *pit;
	}

	if( !pPlayer )
		return 0;

	int nIsAuto = lua_tonumber( L, 1 );

	int account_id = pPlayer->GetAccountID();

	if( nIsAuto == 0 )
	{	// 오토에서 해제
		pPlayer->SetAutoUsed( false );
		pPlayer->Save( true );
		StructPlayer::EraseFromAutoAccount( account_id );

		// DB에서 해당 계정의 다른 모든 케릭터도 오토 세팅이 해제되어야 서버 재부팅 후에 재접시 오토로 인식되지 않음
		DB().Push( new DB_EraseUserFromAutoAccountList( account_id ) );

		// 천벌 제거(오토 전용 천벌 11레벨 이하만)
		pPlayer->RemoveState( StructState::NEMESIS_FOR_AUTO, GameRule::MAX_STATE_LEVEL );

		lua_pushnumber( L, 0 );
	}
	else
	{	// 오토로 세팅
		pPlayer->SetAutoUsed();
		pPlayer->Save( true );
		StructPlayer::AddToAutoAccountList( account_id );

		LOG::Log11N4S( LM_AUTO_USER_CHECKED, pPlayer->GetAccountID(), pPlayer->GetSID(), pPlayer->GetLevel(), pPlayer->GetJobLevel(), pPlayer->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::BY_SCRIPT, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

		lua_pushnumber( L, 1 );
	}

	return 1;
}

// 오프라인 계정에 대해 적용
int SCRIPT_SetAutoAccount( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 ) return 0;

	int nIsAuto = lua_tonumber( L, 1 );

	int account_id = lua_tonumber( L, 2 );

	if( nIsAuto == 0 )
	{	// 오토에서 해제
		StructPlayer::EraseFromAutoAccount( account_id );

		// DB에서 해당 계정의 다른 모든 케릭터도 오토 세팅이 해제되어야 서버 재부팅 후에 재접시 오토로 인식되지 않음
		DB().Push( new DB_EraseUserFromAutoAccountList( account_id ) );

		lua_pushnumber( L, 0 );
	}
	else
	{	// 오토로 세팅
		LOG::Log11N4S( LM_AUTO_USER_CHECKED, account_id, 0, 0, 0, 0, 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::BY_SCRIPT, 0, "", 0, "", 0, "", 0, "", 0 );

		StructPlayer::AddToAutoAccountList( account_id );

		lua_pushnumber( L, 1 );
	}

	return 1;
}

int SCRIPT_ClearAutoAccount( struct lua_State *L )
{
	StructPlayer::ClearAutoAccountList();

	DB().Push( new DB_ClearAutoAccountList() );

	return 0;
}

int SCRIPT_SendGoldChaosUpdate( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SendUpdateGoldChaos() : invalid name" );
		return 1;
	}

	SendGoldChaosUpdateMsg( pPlayer );

	lua_pushnumber( L, 1 );

	return 1;
}

int SCRIPT_ShowDonationMenu( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_ShowDonationMenu() : invalid name" );
		return 1;
	}

	// 조금 신성한 성소?
	pPlayer->SetDialogTitle( "DONATION", TS_SC_DIALOG::TYPE_DONATION_PROP );

	//char szText[255];
	//s_sprintf( szText, _countof( szText ), "%f", -(pPlayer->GetImmoralPoint()) );
	//pPlayer->SetDialogText( szText );

	pPlayer->ShowDialog();

	// 지난 달 기부 보상 아이템 지급 체크 및 처리
	int nRewardRanking = RankingManager::Instance().GetRank( RankingManager::RANKING_TYPE_DONATION_REWARD, pPlayer->GetPlayerUID() );
	if( nRewardRanking && RankingManager::Instance().InvalidateRankingScore( RankingManager::RANKING_TYPE_DONATION_REWARD, pPlayer->GetPlayerUID() ) )
	{
		static const ItemBase::ItemCode sRewardItemCode[ 4 ] = {
			3630024,	// 체루빔의 날개조각<기부랭킹1위>
			3630023,	// 세라핌의 날개조각<기부랭킹2위>
			3630022,	// 엔젤의 날개조각<기부랭킹3위>
			3630021		// 아기천사의 날개조각<기부랭킹4위~10위> 
		};

		// 날개 아이템의 유효 기간 얻기(지난 달 기부 랭킹 유효 시점까지)
		time_t tValid = RankingManager::Instance().GetNextSettlingTime( RankingManager::RANKING_TYPE_DONATION_REWARD );

		// 남은 유효 기간이 1분 이상일 때에만 날개 아이템 지급(1분 이내면 아이템 생성되고 거의 바로 삭제될테니 아예 안 줌)
		if( tValid >= time( NULL ) + 60 )
		{
			StructItem * pRewardItem = StructItem::AllocItem( 0, sRewardItemCode[ ( nRewardRanking <= 4 ) ? nRewardRanking - 1 : 3 ], 1,
				ItemInstance::BY_DONATION_REWARD, -1, -1, -1, 0, 0, 0, 0, 0, 0, tValid );

			if( pRewardItem->IsJoinable() )
			{
				PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@254\v#@item_name@#\v@%d\v#@item_num@#\v%d", pRewardItem->GetItemBase().nNameId, pRewardItem->GetCount() );
			}
			else
			{
				PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@253\v#@item_name@#\v@%d", pRewardItem->GetItemBase().nNameId );
			}

			StructItem * pNewItem = pPlayer->PushItem( pRewardItem, pRewardItem->GetCount() );
			if( pNewItem )
			{
				LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pRewardItem->GetItemEnhance() * 100 + pRewardItem->GetItemLevel(), pRewardItem->GetItemCode(), pRewardItem->GetCount(), pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), pNewItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "DONATION_REWARD", LOG::STR_NTS );
			}

			if( pNewItem != pRewardItem ) 
			{
				StructItem::PendFreeItem( pRewardItem );
			}

			pPlayer->SetDialogTitle( "DONATION_REWARD", TS_SC_DIALOG::TYPE_DONATION_REWARD_NOTIFY );
			pPlayer->ShowDialog();
		}
	}

	lua_pushnumber( L, 1 );

	return 1;
}

int SCRIPT_GetCreatureNameID( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	int index = lua_tonumber( L, 1 );

	StructSummon *pSummon = pPlayer->GetSummon( index );

	if( !pSummon )
	{
		pSummon = pPlayer->GetSummon( (AR_HANDLE) index );

		if( !pSummon )
		{
			lua_pushnumber( L, 0 );
			return 1;
		}
	}

	int name_id = pSummon->GetSummonNameId();

	lua_pushnumber( L, name_id );

	return 1;
}

int SCRIPT_ShowCreatureNameChangeBox( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	int index = lua_tonumber( L, 1 );

	StructSummon *pSummon = pPlayer->GetSummon( index );

	if( !pSummon )
	{
		pSummon = pPlayer->GetSummon( (AR_HANDLE) index );

		if( !pSummon )
		{
			lua_pushnumber( L, 0 );
			return 1;
		}
	}

	// 대여 전용 크리처는 이름 변경 불가능
	if( pSummon->GetRidingInfo() == SummonBase::RIDING_LENT )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	pPlayer->SetNameChangeTarget( index );

	SendShowCreatureNameChangeBox( pPlayer, pSummon->GetHandle() );

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_SetPCBangUser( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	int nPCBangMode = lua_tonumber( L, 1 );

	switch( nPCBangMode )
	{
	case GameRule::PCBANG_NO_BONUS:
	case GameRule::PCBANG_ALLY_BONUS:
	case GameRule::PCBANG_PREMIUM_BONUS:
		pPlayer->SetPCBangMode( nPCBangMode );
		lua_pushnumber( L, nPCBangMode );
		break;

	default:
		lua_pushnumber( L, -1 );
		break;
	}

	return 1;
}

int SCRIPT_SetContinuousPlayTime( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	int nContinuousPlayTime = lua_tonumber( L, 1 );

	pPlayer->SetContinuousPlayTime( nContinuousPlayTime );
	lua_pushnumber( L, pPlayer->GetContinuousPlayTime() );
	return 1;
}

int SCRIPT_ShowAuctionWindow( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	pPlayer->SetDialogTitle( "Auction", TS_SC_DIALOG::TYPE_AUCTION_WINDOW );
	pPlayer->ShowDialog();

	return 0;
}

int SCRIPT_CancelAuctionBySeller( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_CancelAuctionBySeller() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	PlayerUID nSellerUID = static_cast< PlayerUID >( lua_tonumber( L, 1 ) );

	if( !nSellerUID )
	{
		LUA()->Log( "SCRIPT_CancelAuctionBySeller() : invalid argument" );
		return 0;
	}

	std::vector< ItemBase::ItemCode > vResultItemCode;

	AuctionManager::Instance().CancelAuction( nSellerUID, vResultItemCode, pPlayer );

	char szBuf[ 512 ];

	s_sprintf( szBuf, _countof( szBuf ), "%d auction(s) are cancelled.", vResultItemCode.size() );
	LUA()->Log( szBuf );
	for( std::vector< ItemBase::ItemCode >::const_iterator it = vResultItemCode.begin() ; it != vResultItemCode.end() ; ++it )
	{
		s_sprintf( szBuf, _countof( szBuf ), "@716\v#@itemname@#\v@%d", StructItem::GetItemBase( (*it) ).nNameId );
		LUA()->Log( szBuf );
	}

	return 0;
}

int SCRIPT_ShowHuntaholicLobbyWindow( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	pPlayer->SetDialogTitle( "Huntaholic", TS_SC_DIALOG::TYPE_HUNTAHOLIC_LOBBY );
	pPlayer->ShowDialog();

	return 0;
}

int SCRIPT_GetEventAreaEnterCount( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_GetEventAreaEnterCount() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_GetEventAreaEnterCount() : invalid player" );
		return 0;
	}

	int nEventAreaID = lua_tonumber( L, 1 );

	// 잘못된 이벤트 영역 ID일 경우
	if( !nEventAreaID || !GameContent::GetEnterCountLimitOfEventArea( nEventAreaID ) )
	{
		LUA()->Log( "SCRIPT_GetEventAreaEnterCount() : invalid event area id" );
		return 0;
	}

	lua_pushnumber( L, pPlayer->GetEventAreaEnterCount( nEventAreaID ) );
	return 1;
}

int SCRIPT_SetEventAreaEnterCount( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		LUA()->Log( "SCRIPT_SetEventAreaEnterCount() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_SetEventAreaEnterCount() : invalid player" );
		return 0;
	}

	int nEventAreaID = lua_tonumber( L, 1 );

	// 잘못된 이벤트 영역 ID일 경우
	if( !nEventAreaID || !GameContent::GetEnterCountLimitOfEventArea( nEventAreaID ) )
	{
		LUA()->Log( "SCRIPT_SetEventAreaEnterCount() : invalid event area id" );
		return 0;
	}

	pPlayer->SetEventAreaEnterCount( nEventAreaID, lua_tonumber( L, 2 ) );
	return 0;
}

int SCRIPT_SetPKMode( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);
	if( nArgCnt < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	if( lua_tonumber( L, 1 ) )
	{
		pPlayer->TurnOnPkMode( true );
	}
	else
	{
		pPlayer->TurnOffPkMode( true );
	}

	return 0;
}

int SCRIPT_Kill( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );
	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushnumber( L, 1 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 2 );
		return 1;
	}

	StructCreature::iterator cit = StructCreature::get( lua_tonumber( L, 1 ) );
	StructCreature *pTarget = *cit;

	if( !pTarget || !pTarget->IsInWorld() || pTarget->IsDead() )
	{
		lua_pushnumber( L, 3 );
		return 1;
	}

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pTarget ) );

		int nPrevHP = pTarget->GetHP();
		pTarget->damage( pTarget, nPrevHP, false );

		BroadcastHPMPMsg( pTarget, -nPrevHP, 0, true );
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_KillTarget( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer || !pPlayer->GetTarget() )
	{
		lua_pushnumber( L, 1 );
		return 1;
	}

	StructCreature::iterator cit = StructCreature::get( pPlayer->GetTarget() );
	StructCreature *pTarget = *cit;

	if( !pTarget || !pTarget->IsInWorld() || pTarget->IsDead() )
	{
		lua_pushnumber( L, 2 );
		return 1;
	}

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pTarget ) );

		int nPrevHP = pTarget->GetHP();
		pTarget->damage( pPlayer, nPrevHP, false );

		BroadcastHPMPMsg( pTarget, -nPrevHP, 0, true );
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_PrivateNotice( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_PrivateNotice() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, std::min( n + 1, 3 ) );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 1 );
		return 1;
	}

	std::string szSender = ( n >= 2 ) ? lua_tostring_utf8( L, 1 ).c_str() : "@NOTICE";
	std::string szString = lua_tostring_utf8( L, ( n >= 2 ) ? 2 : 1 );

	SendChatMessage( false, CHAT_NOTICE, szSender.c_str(), pPlayer, szString.c_str(), static_cast< unsigned int >( szString.size() ) );

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_GetBattleArenaBlockTime( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetBattleArenaBlockTime() : invalid argument" );
		return 1;
	}

	lua_pushnumber( L, pPlayer->GetBattleArenaBlockTimeLeft() / 100 );
	return 1;
}

int SCRIPT_SetBattleArenaBlockTime( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetBattleArenaBlockTime() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetBattleArenaBlockTime() : invalid argument" );
		return 1;
	}

	time_t tBlockDuration = lua_tonumber( L, 1 );

	if( !tBlockDuration )
		pPlayer->SetBattleArenaBlockTime( 0 );
	else
		pPlayer->SetBattleArenaBlockTime( time( NULL ) + tBlockDuration );

	pPlayer->DBQuery( new DB_UpdateBattleArenaPenalty( pPlayer ) );
	BattleArenaManager::SendBattleArenaPenaltyMessage( pPlayer );

	return 0;
}

int SCRIPT_GetBattleArenaPenaltyCount( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetBattleArenaPenaltyCount() : invalid argument" );
		return 1;
	}

	lua_pushnumber( L, pPlayer->GetBattleArenaPenaltyCount() );
	return 1;
}

int SCRIPT_SetBattleArenaPenaltyCount( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetBattleArenaPenaltyCount() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetBattleArenaPenaltyCount() : invalid argument" );
		return 1;
	}

	pPlayer->SetBattleArenaPenaltyCount( lua_tonumber( L, 1 ) );

	pPlayer->DBQuery( new DB_UpdateBattleArenaPenalty( pPlayer ) );
	BattleArenaManager::SendBattleArenaPenaltyMessage( pPlayer );

	return 0;
}

int SCRIPT_GetBattleArenaNextPenaltyDecTime( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetBattleArenaNextPenaltyDecTime() : invalid argument" );
		return 1;
	}

	time_t tNextDecTime = pPlayer->GetBattleArenaPenaltyDecTime();
	time_t t = time( NULL );

	lua_pushnumber( L, ( tNextDecTime > t ) ? ( tNextDecTime - t ) : 0 );
	return 1;
}

int SCRIPT_SetBattleArenaNextPenaltyDecTime( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetBattleArenaNextPenaltyDecTime() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetBattleArenaNextPenaltyDecTime() : invalid argument" );
		return 1;
	}

	time_t tDurationToNextDecTime = lua_tonumber( L, 1 );

	if( !tDurationToNextDecTime )
		pPlayer->SetBattleArenaPenaltyDecTime( 0 );
	else
		pPlayer->SetBattleArenaPenaltyDecTime( time( NULL ) + tDurationToNextDecTime );

	pPlayer->DBQuery( new DB_UpdateBattleArenaPenalty( pPlayer ) );
	BattleArenaManager::SendBattleArenaPenaltyMessage( pPlayer );

	return 0;
}

int SCRIPT_GetBattleArenaTeamNo( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetBattleArenaTeamNo() : invalid argument" );
		return 1;
	}

	lua_pushnumber( L, BattleArenaManager::Instance().GetTeamNo( pPlayer ) );
	return 1;
}


int SCRIPT_SetSecrouteFreePass( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );
	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetSecrouteFreePass() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer * pPlayer = *pit;

	if( pPlayer )
	{
		int bIsPremiumUser = lua_tonumber( L, 1 );
		int nRestSecond = lua_tonumber( L, 2 );

		pPlayer->SetSetSecrouteFreePass( (bIsPremiumUser != 0), nRestSecond );
	}

	return 0;
}


int SCRIPT_SetChatBlockTime( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );
	if( nArgCnt < 2 || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetChatBlockTime() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( pPlayer )
	{
		int nBlockMinute = lua_tonumber( L, 2 );
		if( nBlockMinute >= 0 )
		{
			pPlayer->SetChatBlockTime( nBlockMinute * 60 );
		}
	}

	return 0;
}

int SCRIPT_SetInvisible( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );
	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetInvisible() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( pPlayer )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

		int mode = lua_tonumber( L, 1 );
		if( mode == 1 )
		{
			pPlayer->SetInvisible( true );
		}
		else if( mode == 2 )
		{
			pPlayer->SetInvisible( false );
		}
		else
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_SetInvisible() : invalid argument" );
			return 1;
		}

		BroadcastStatusMessage( pPlayer );
	}

	return 0;
}

int SCRIPT_Rebirth( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer * pPlayer = *pit;

	if( pPlayer )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

		int prev_hp = pPlayer->GetHP();
		int prev_mp = pPlayer->GetMP();

		pPlayer->AddHP( pPlayer->GetMaxHP() );
		pPlayer->AddMP( pPlayer->GetMaxHP() );

		BroadcastHPMPMsg( pPlayer, pPlayer->GetHP() - prev_hp, pPlayer->GetMP() - prev_mp );
	}

	return 0;
}

int SCRIPT_SetLevel( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );
	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetLevel() : invalid argument" );
		return 1;
	}

	int new_level = lua_tonumber( L, 1 );
	if( new_level < 1 || new_level > GameRule::MAX_LEVEL )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetLevel() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( pPlayer )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

		__int64 exp = 0;
		if( new_level > 1 )
			exp = GameContent::GetNeedExp( new_level-1 );

		pPlayer->SetEXP( exp );
	}

	return 0;
}

int SCRIPT_Heal( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );
	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_Heal() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( pPlayer )
	{
		int add_hp = lua_tonumber( L, 1 );
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );
		pPlayer->AddHP( add_hp );
	}

	return 0;
}

int SCRIPT_SetAccountAuthority( struct lua_State *L )
{
	if( !GameRule::bUseAccountAuthorityDB )
		return 0;

	int nArgCnt = lua_gettop( L );
	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetAccountAuthority() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer * pPlayer = *pit;

	if( pPlayer )
	{
		int AuthorityType = lua_tonumber( L, 1 );
		int Duration = lua_tonumber( L, 2 );

		// 일단 빌링DB 갱신 하고 (게임 중에 웹 상에서 빌링쪽 서비스를 구매 할 가능성이 있으므로)
		pPlayer->DBQuery( new DB_ReadAccountAuthorityInfo( pPlayer ) );

		// Duration 이 0이면 이 계정 속성 해제 
		if( Duration )
		{
			pPlayer->DBQuery( new DB_SetAccountAuthorityInfo( pPlayer, AuthorityType, Duration ) );
		}
		else
		{
			pPlayer->DBQuery( new DB_CloseAccountAuthorityInfo( pPlayer, AuthorityType ) );
		}
	}

	return 0;
}