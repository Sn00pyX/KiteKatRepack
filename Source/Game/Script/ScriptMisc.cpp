
#include <toolkit/XConsole.h>
#include <toolkit/XEnv.h>
#include <mmo/ArcadiaServer.h>

#include "ErrorCode/ErrorCode.h"

#include "GameNetwork.h"
#include "ScriptMisc.h"
#include "ScriptCommon.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "ContentLoader.h"
#include "GameProc.h"
#include "DB_Commands.h"
#include "ScheduledCommandManager.h"
#include "GlobalVariableManager.h"
#include "InstanceDungeonManager.h"
#include "BattleArenaManager.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"
#include "ThreadPlayerHelper.h"

#include <lua/lua.hpp>


//////////////////////////////////////////////////////////////////////////
/* from lua 5.0.3 source
** {======================================================
** Time/Date operations
** { year=%Y, month=%m, day=%d, hour=%H, min=%M, sec=%S,
**   wday=%w+1, yday=%j, isdst=? }
** =======================================================
*/

static void setfield (lua_State *L, const char *key, int value) {
	lua_pushinteger(L, value);
	lua_setfield(L, -2, key);
}

static void setboolfield (lua_State *L, const char *key, int value) {
	if (value < 0)  /* undefined? */
		return;  /* does not set field */
	lua_pushboolean(L, value);
	lua_setfield(L, -2, key);
}

static int getboolfield (lua_State *L, const char *key) {
	int res;
	lua_getfield(L, -1, key);
	res = lua_isnil(L, -1) ? -1 : lua_toboolean(L, -1);
	lua_pop(L, 1);
	return res;
}

static int getfield (lua_State *L, const char *key, int d) {
	int res;
	lua_getfield(L, -1, key);
	if (lua_isnumber(L, -1))
		res = (int)lua_tointeger(L, -1);
	else {
		if (d < 0)
			return luaL_error(L, "field " LUA_QS " missing in date table", key);
		res = d;
	}
	lua_pop(L, 1);
	return res;
}

int	SCRIPT_LuaSTDLib_Time( struct lua_State *L )
{
	// from lua source 5.1.5 io_time()
	time_t t;
	if (lua_isnoneornil(L, 1))  /* called without args? */
		t = time(NULL);  /* get current time */
	else {
		struct tm ts;
		luaL_checktype(L, 1, LUA_TTABLE);
		lua_settop(L, 1);  /* make sure table is at the top */
		ts.tm_sec = getfield(L, "sec", 0);
		ts.tm_min = getfield(L, "min", 0);
		ts.tm_hour = getfield(L, "hour", 12);
		ts.tm_mday = getfield(L, "day", -1);
		ts.tm_mon = getfield(L, "month", -1) - 1;
		ts.tm_year = getfield(L, "year", -1) - 1900;
		ts.tm_isdst = getboolfield(L, "isdst");
		t = mktime(&ts);
	}
	if (t == (time_t)(-1))
		lua_pushnil(L);
	else
		lua_pushnumber(L, (lua_Number)t);
	return 1;
}

int	SCRIPT_GetMinute(struct lua_State* L)
{
	time_t t = time(NULL);
	struct tm ts;
	
	gmtime_s(&ts, &t);

	lua_pushnumber(L, (lua_Number)ts.tm_min);
	return 1;
}

int	SCRIPT_LuaSTDLib_Date( struct lua_State *L )
{
	// from lua source 5.1.5 io_date()
	const char *s = luaL_optstring(L, 1, "%c");
	time_t t = luaL_opt(L, (time_t)luaL_checknumber, 2, time(NULL));
	struct tm stm;
	if (*s == '!') {  /* UTC? */
		if (gmtime_s(&stm, &t) != 0) {
			lua_pushnil(L);
			return 1;
		}
		s++;  /* skip `!' */
	}
	else {
		if (localtime_s(&stm, &t) != 0) {
			lua_pushnil(L);
			return 1;
		}
	}
	if (strcmp(s, "*t") == 0) {
		lua_createtable(L, 0, 9);  /* 9 = number of fields */
		setfield(L, "sec", stm.tm_sec);
		setfield(L, "min", stm.tm_min);
		setfield(L, "hour", stm.tm_hour);
		setfield(L, "day", stm.tm_mday);
		setfield(L, "month", stm.tm_mon+1);
		setfield(L, "year", stm.tm_year+1900);
		setfield(L, "wday", stm.tm_wday+1);
		setfield(L, "yday", stm.tm_yday+1);
		setboolfield(L, "isdst", stm.tm_isdst);
	}
	else {
		char cc[3];
		luaL_Buffer b;
		cc[0] = '%'; cc[2] = '\0';
		luaL_buffinit(L, &b);
		for (; *s; s++) {
			if (*s != '%' || *(s + 1) == '\0')  /* no conversion specifier? */
				luaL_addchar(&b, *s);
			else {
				size_t reslen;
				char buff[200];  /* should be big enough for any conversion result */
				cc[1] = *(++s);
				reslen = strftime(buff, sizeof(buff), cc, &stm);
				luaL_addlstring(&b, buff, reslen);
			}
		}
		luaL_pushresult(&b);
	}
	return 1;
}
//////////////////////////////////////////////////////////////////////////


int SCRIPT_DeleteFromBlockAccount( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DeleteFromBlockAccount() : invalid argument" );
		return 1;
	}

	GameRule::DeleteFromBlockAccount( lua_tostring_utf8( L, 1 ).c_str() );

	return 0;
}

int SCRIPT_Shutdown( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	int nLastSec = 60;
	bool bNeedToCreateDump = false;

	if( n > 0 && lua_isnumber( L, 1 ) ) 
		nLastSec = lua_tonumber( L, 1 );

	if( n > 1 && lua_isnumber( L, 2 ) )
		bNeedToCreateDump = lua_tonumber( L, 2 );

	ShutdownManager::Instance().Shutdown( nLastSec, bNeedToCreateDump );

	return 0;
}

int SCRIPT_Notice( struct lua_State *L )
{	
	int n = lua_gettop(L);			// number of arguments

	std::string szSender = "@NOTICE";
	std::string szString = "";

	if( n < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_Notice() : invalid argument" );
		return 1;
	}

	if( n == 2 )
	{	
		szSender = lua_tostring_utf8( L, 1 );
		szString = lua_tostring_utf8( L, 2 );
	}
	else
	{
		szString = lua_tostring_utf8( L, 1 );
	}

	if( szString.empty() == false )
	{
		SendGlobalChatMessage( CHAT_NOTICE, szSender.c_str(), szString.c_str(), static_cast< unsigned int >( szString.size() ) );
	}

	return 0;
}

int SCRIPT_Announce( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	std::string szSender = "@ANNOUNCE";
	std::string szString = "";

	if( n < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_Announce() : invalid argument" );
		return 1;
	}

	if( n == 2 )
	{	
		szSender = lua_tostring_utf8( L, 1 );
		szString = lua_tostring_utf8( L, 2 );
	}
	else
	{
		szString = lua_tostring_utf8( L, 1 );
	}

	SendGlobalChatMessage( CHAT_ANNOUNCE, szSender.c_str(), szString.c_str(), static_cast< unsigned int >( szString.size() ) );

	return 0;
}

int SCRIPT_Whisper( struct lua_State *L )
{	
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_Whisper() : invalid argument" );
		return 1;
	}

	std::string szSender = "@NOTICE";
	std::string szTarget = "";
	std::string szString = "";
	
	if( n == 2 )
	{	
		szTarget = lua_tostring_utf8( L, 1 );
		szString = lua_tostring_utf8( L, 2 );
	}
	else if( n == 3 )
	{
		szSender = lua_tostring_utf8( L, 1 );
		szTarget = lua_tostring_utf8( L, 2 );
		szString = lua_tostring_utf8( L, 3 );
	}

	SendChatMessage( true, CHAT_WHISPER, szSender.c_str(), szTarget.c_str(), szString.c_str(), static_cast< unsigned int >( szString.size() ) );

	return 0;
}

int SCRIPT_GetEnv( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetEnv() : invalid argument" );
		return 1;
	}

	std::string strValue;
	strValue = ENV().GetString( lua_tostring_utf8( L, 1 ).c_str(), "<NULL>" );

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

int SCRIPT_SetEnv( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetEnv() : invalid argument" );
		return 1;
	}

	ENV().Set( lua_tostring_utf8( L, 1 ).c_str(), lua_tostring_utf8( L, 2 ).c_str() );

	return 0;
}

int SCRIPT_GetGameTime( struct lua_State *L )
{
	lua_pushnumber( L, GetArTime() );

	return 1;
}

int SCRIPT_Reload( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	bool bItem = false;
	bool bMonster = false;
	bool bSkill = false;
	bool bString = false;
	bool bEtc = false;
	bool bScript = false;
	bool bQuest = false;
	bool bScheduledCommand = false;

	if( n < 1 )
	{
		bScript = true;
		bItem = true;
		bMonster = true;
		bSkill = true;
		bString = true;
		bEtc = true;	
		bQuest = true;
	}
	else
	{
		for( int x = 0; x < n; ++x )
		{
			std::string pTarget = lua_tostring_utf8( L, x+1 );

			if( !_stricmp( pTarget.c_str(), "item" ) ) bItem = true;
			if( !_stricmp( pTarget.c_str(), "monster" ) ) bMonster = true;
			if( !_stricmp( pTarget.c_str(), "skill" ) ) bSkill = true;
			if( !_stricmp( pTarget.c_str(), "string" ) ) bString = true;
			if( !_stricmp( pTarget.c_str(), "etc" ) ) bEtc = true;
			if( !_stricmp( pTarget.c_str(), "script" ) ) bScript = true;
			if( !_stricmp( pTarget.c_str(), "quest" ) ) bQuest = true;
			if( !_stricmp( pTarget.c_str(), "scheduled_command" ) ) bScheduledCommand = true;
		}
	}

	if( bItem )		ItemLoader().onProcess( 0 );
	if( bMonster )	MonsterLoader().onProcess( 0 );
	if( bSkill )	SkillLoader().onProcess( 0 );
	if( bString )	StringLoader().onProcess( 0 );
	if( bEtc )		ETCLoader().onProcess( 0 );
	if( bQuest )	
	{
		QuestLoader().onProcess( 0 );
		GameContent::ApplyQuestLink();
	}
	if( bScript ) 
	{
		ResourceManager::Init();
		GameContent::LoadScript();
	}
	if( bScheduledCommand )
	{
		try
		{
			ScheduledCommandLoader().onProcess( 0 );
		}
		catch( XException & ex )
		{
			FILELOG( "Cannot load scheduled command ERROR: %s", ex.what() );
			_cprint( "Cannot load scheduled command ERROR: %s\n", ex.what() );
		}
	}

	return 0;
}

int SCRIPT_Conv( struct lua_State *L )
{
	int n = lua_gettop( L );

	if( n < 1 ) return 0;

	if( n % 2 != 1 ) return 0;

	if(!lua_isstring( L, 1 ) ) return 0;

	std::string strText;

	for( int i = 1; i <= n; ++i )
	{
		if( i != 1 ) strText += "\v";

		std::string szFormatString = lua_tostring_utf8( L, i );

		strText += szFormatString;
	}

	lua_pushstring_utf8( L, strText.c_str() );

	return 1;
}

int SCRIPT_SetCurrentLocationId( struct lua_State *L )
{
	extern int g_currentLocationId;

	int n = lua_gettop( L );

	if( n < 1 ) return 0;

	g_currentLocationId = lua_tonumber( L, 1 );

	return 0;
	
}

int SCRIPT_OpenUrl( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_OpenUrl() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_OpenUrl() : invalid name" );
		return 0;
	}

	std::string szUrl;

	szUrl = lua_tostring_utf8( L, 1 );

	for( int i = 2 ; i < nArgCnt ; ++i )
	{
		szUrl += "|";
		szUrl += lua_tostring_utf8( L, i );
	}

	bool bWaitForEventScene = lua_tonumber( L, nArgCnt );

	SendOpenUrl( pPlayer, szUrl.c_str(), bWaitForEventScene );
	return 0;
}

int SCRIPT_OpenPopup( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, nArgCnt ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_OpenPopup() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_OpenPopup() : invalid name" );
		return 0;
	}

	bool bIsRequireOnetimePassword = lua_tonumber( L, 2 );

	int one_time_key = 0;
	char szOnetimePassword[100];
	memset( szOnetimePassword, 0, sizeof( szOnetimePassword ) );

	if( bIsRequireOnetimePassword )
	{
		char *p = szOnetimePassword;
		size_t size = sizeof( szOnetimePassword );

		std::string szServerName = ENV().GetString( "app.name", "unknown" );

		s_memcpy( p, size, "?server=", 8 );
		p += 8;
		size -= 8;

		int nServerNameLength = static_cast< int >( szServerName.length() );
		s_memcpy( p, size, szServerName.c_str(), nServerNameLength );
		p += nServerNameLength;
		size -= nServerNameLength;

		s_memcpy( p, size, "&id=", 4 );										
		p += 4;
		size -= 4;

		one_time_key = XRandom() % 0x00ffffff;
		int id_len = GameRule::AppendOnetimePassword( p, size, one_time_key, pPlayer->GetSID(), pPlayer->GetAccountID() );
		p += id_len;

		*p = '\0';
	}
		
	std::string szUrl;

	szUrl = ENV().GetString( lua_tostring_utf8( L, 1 ).c_str(), "" );
	if( bIsRequireOnetimePassword )
	{
		szUrl += szOnetimePassword;
	}

	for( int i = 2 ; i < nArgCnt - 1 ; ++i )
	{
		szUrl += "|";
		szUrl += ENV().GetString( lua_tostring_utf8( L, i ).c_str(), "" );
		if( bIsRequireOnetimePassword )
		{
			szUrl += szOnetimePassword;
		}
	}

	bool bWaitForEventScene = lua_tonumber( L, 3 );

	if( bIsRequireOnetimePassword )
	{
		UpdateOTPCF_SendOpenURL * pFo = new UpdateOTPCF_SendOpenURL( pPlayer, szUrl.c_str(), bWaitForEventScene );
		DB().Push( new DB_UpdateOneTimePassword( pPlayer->GetSID(), one_time_key, pFo ) );
	}
	else
	{
		SendOpenUrl( pPlayer, szUrl.c_str(), bWaitForEventScene );
	}

	return 0;
}


int SCRIPT_OpenPopupAndSetSize( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 5 || !lua_isnumber( L, nArgCnt ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_OpenPopupAndSetSize() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_OpenPopupAndSetSize() : invalid name" );
		return 0;
	}

	bool bIsRequireOnetimePassword = lua_tonumber( L, nArgCnt - 1 );
	int nWidth = lua_tonumber( L, nArgCnt - 3 );
	int nHeight = lua_tonumber( L, nArgCnt - 2 );

	int one_time_key = 0;
	char szOnetimePassword[100];
	memset( szOnetimePassword, 0, sizeof( szOnetimePassword ) );

	if( bIsRequireOnetimePassword )
	{
		char *p = szOnetimePassword;
		size_t size = sizeof( szOnetimePassword );

		std::string szServerName = ENV().GetString( "app.name", "unknown" );

		s_memcpy( p, size, "?server=", 8 );
		p += 8;
		size -= 8;

		int nServerNameLength = static_cast< int >( szServerName.length() );
		s_memcpy( p, size, szServerName.c_str(), nServerNameLength );
		p += nServerNameLength;
		size -= nServerNameLength;

		s_memcpy( p, size, "&id=", 4 );										
		p += 4;
		size -= 4;

		one_time_key = XRandom() % 0x00ffffff;
		int id_len = GameRule::AppendOnetimePassword( p, size, one_time_key, pPlayer->GetSID(), pPlayer->GetAccountID() );
		p += id_len;

		*p = '\0';
	}

	std::string szUrl;

	szUrl = ENV().GetString( lua_tostring_utf8( L, 1 ).c_str(), "" );
	if( bIsRequireOnetimePassword )
	{
		szUrl += szOnetimePassword;
	}

	for( int i = 2 ; i < nArgCnt - 3 ; ++i )
	{
		szUrl += "|";
		szUrl += ENV().GetString( lua_tostring_utf8( L, i ).c_str(), "" );
		if( bIsRequireOnetimePassword )
		{
			szUrl += szOnetimePassword;
		}
	}

	bool bWaitForEventScene = lua_tonumber( L, nArgCnt );

	if( bIsRequireOnetimePassword )
	{
		UpdateOTPCF_SendOpenURL * pFo = new UpdateOTPCF_SendOpenURL( pPlayer, szUrl.c_str(), bWaitForEventScene, nWidth, nHeight );
		DB().Push( new DB_UpdateOneTimePassword( pPlayer->GetSID(), one_time_key, pFo ) );
	}
	else
	{
		SendOpenUrl( pPlayer, szUrl.c_str(), bWaitForEventScene, nWidth, nHeight );
	}

	return 0;
}


int SCRIPT_GetServerCategory( struct lua_State *L )
{
	DWORD ServiceServerBitMask	= 0x20000000;
	DWORD PKServerBitMask		= 0x10000000;

	DWORD nServerCategory		= 0;

	nServerCategory |= (!!ENV().GetInt( "game.ServiceServer" ))		* ServiceServerBitMask;	// 정섭 여부(1: 정섭, 0: 테섭+개발용)
	nServerCategory |= (!!ENV().GetInt( "game.PKServer" ))			* PKServerBitMask;		// PK 서버 여부(1: PK 서버, 0: 일반 서버)
	nServerCategory += ENV().GetInt( "auth.server_idx" );									// 0~7 값의 범위, 서버 인덱스

	lua_pushnumber( L, nServerCategory );
	return 1;
}

int SCRIPT_GetLocalInfo( struct lua_State *L )
{
	extern volatile int g_nCurrentLocalFlag;

	lua_pushnumber( L, g_nCurrentLocalFlag );

	return 1;
	
}

int SCRIPT_Suicide( struct lua_State *L )
{
	XSEH::InvokeUnhandledException( (LONG)0xC0000000DL );

	return 0;
}

int SCRIPT_SetScheduledCommandManagerPriority( struct lua_State *L )
{
	if( lua_gettop( L ) < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetScheduledCommandManagerPriority() : invalid argument" );
		return 1;
	}

	int nPriority = lua_tonumber( L, 1 );

	ArcadiaServer::Instance().SetObjectPriority( &ScheduledCommandManager::Instance(), static_cast< ArSchedulerObject::AR_OBJECT_PRIORITY >( nPriority ) );
	return 0;
}

int SCRIPT_SetGlobalVariable( struct lua_State *L )
{
	if( lua_gettop( L ) < 2 || !lua_isstring( L, 1 ) || !lua_isstring( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetGlobalVariable() : invalid argument" );
		return 1;
	}

	GVM().Set( lua_tostring_utf8( L, 1 ).c_str(), lua_tostring_utf8( L, 2 ).c_str() );
	return 0;
}

int SCRIPT_GetGlobalVariable( struct lua_State *L )
{
	if( lua_gettop( L ) < 1 || !lua_isstring( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetGlobalVariable() : invalid argument" );
		return 1;
	}

	const char * pszValue = GVM().Get( lua_tostring_utf8( L, 1 ).c_str() );
	int nValue = ( pszValue ) ? atoi( pszValue ) : 0;

	if( !pszValue )
		lua_pushstring( L, "" );
	else if( nValue || !strcmp( pszValue, "0" ) )
		lua_pushnumber( L, nValue );
	else
		lua_pushstring_utf8( L, pszValue );

	return 1;
}

int SCRIPT_DeleteGlobalVariable( struct lua_State *L )
{
	if( lua_gettop( L ) < 1 || !lua_isstring( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DeleteGlobalVariable() : invalid argument" );
		return 1;
	}

	lua_pushnumber( L, ( GVM().Delete( lua_tostring_utf8( L, 1 ).c_str() ) ) ? 0 : 1 );
	return 1;
}

int SCRIPT_GetInstanceDungeonTypeID( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetInstanceDungeonTypeID() : invalid argument" );
		return 1;
	}

	int nInstanceDungeonID = (int)lua_tonumber( L, 1 );
	unsigned char nLayer = (unsigned char)lua_tonumber( L, 2 );

	int nInstanceDungeonTypeID = InstanceDungeonManager::Instance().GetInstanceDungeonTypeID( nInstanceDungeonID, nLayer );

	lua_pushnumber( L, nInstanceDungeonTypeID );
	return 1;
}

int SCRIPT_SetInstanceDungeonFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 4 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isstring( L, 3 ) || !lua_isstring( L, 4 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetInstanceDungeonFlag() : invalid argument" );
		return 1;
	}

	int nInstanceDungeonID = (int)lua_tonumber( L, 1 );
	unsigned char nLayer = (unsigned char)lua_tonumber( L, 2 );
	const std::string & strName = lua_tostring_utf8( L, 3 );
	const std::string & strValue = lua_tostring_utf8( L, 4 );
	
	bool bSuccess = InstanceDungeonManager::Instance().SetInstanceDungeonTypeFlag( nInstanceDungeonID, nLayer, strName, strValue );

	lua_pushboolean( L, bSuccess );
	return 1;
}

int SCRIPT_GetInstanceDungeonFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isstring( L, 3 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetInstanceDungeonFlag() : invalid argument" );
		return 1;
	}

	int nInstanceDungeonID = (int)lua_tonumber( L, 1 );
	unsigned char nLayer = (unsigned char)lua_tonumber( L, 2 );
	const std::string & strName = lua_tostring_utf8( L, 3 );
	
	const std::string & strValue = InstanceDungeonManager::Instance().GetInstanceDungeonTypeFlag( nInstanceDungeonID, nLayer, strName );

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

int SCRIPT_GetAliveInstanceRespawnGroupMonsterCount( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isnumber( L, 3 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetAliveInstanceRespawnGroupMonsterCount() : invalid argument" );
		return 1;
	}

	int nInstanceDungeonID = (int)lua_tonumber( L, 1 );
	unsigned char nLayer = (unsigned char)lua_tonumber( L, 2 );
	int nRespawnGroup = (int)lua_tonumber( L, 3 );

	lua_pushnumber( L, InstanceDungeonManager::Instance().GetAliveInstanceDungeonRespawnGroupMonsterCount( nInstanceDungeonID, nLayer, nRespawnGroup ) );

	return 1;
}

int SCRIPT_DoEachPlayerInInstanceDungeon( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isstring( L, 3 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DoEachPlayerInInstanceDungeon() : invalid argument" );
		return 1;
	}

	int nInstanceDungeonID = (int)lua_tonumber( L, 1 );
	unsigned char nLayer = (unsigned char)lua_tonumber( L, 2 );
	const std::string & strScript = lua_tostring_utf8( L, 3 );

	// 스크립트가 너무 길면 실패
	if( strScript.size() > 1024 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DoEachPlayerInInstanceDungeon() : too long script" );
		return 1;
	}

	// 디버그 버전일 때만 스크립트에 사용되면 안 되는 함수가 사용되는지 체크
#ifdef _DEBUG
	std::vector< std::string > vTokens;
	XStringUtil::Split( strScript.c_str(), vTokens, " \t\r\n+-*=<>~/,()[]'\\", false );

	for( std::vector< std::string >::const_iterator it = vTokens.begin() ; it != vTokens.end() ; ++it )
	{
		if( !_stricmp( (*it).c_str(), "force_monster_proc_dead" ) ||
			!_stricmp( (*it).c_str(), "saveall" ) ||
			!_stricmp( (*it).c_str(), "kill" ) ||
			!_stricmp( (*it).c_str(), "kill_target" ) ||
			XStringUtil::WildCardCmp( "*_instance*", (*it).c_str() ) ||
			XStringUtil::WildCardCmp( "broadcast_mission_*", (*it).c_str() ) )
		{
			assert( 0 );
			lua_pushstring( L, "invalid script" );
			LUA()->Log( "SCRIPT_DoEachPlayerInInstanceDungeon() : invalid script" );
			return 1;
		}
	}
#endif

	struct ScriptFunctor : public ArObjectFunctor
	{
		ScriptFunctor( const char * pszScript, struct lua_State * _pLuaState )
			: pLuaState( _pLuaState )
		{
			int nCodePage = ENV().GetInt( "CodePage", CP_ACP );

			wchar_t wszString[ 1024 ];
			MultiByteToWideChar( nCodePage, 0, pszScript, -1, wszString, _countof( wszString ) );
			WideCharToMultiByte( CP_UTF8, 0, wszString, -1, szUTFString, _countof( szUTFString ), NULL, NULL );
		}

		virtual void operator()( ArObject * pObj ) const
		{
			ThreadPlayerHelper TPHelper( static_cast< StructPlayer * >( pObj ) );
			int pre_idx = lua_gettop( pLuaState );
			if( luaL_dostring( pLuaState, szUTFString ) != 0 )
			{
				const char* err_msg = lua_tostring( pLuaState, -1 );
				FILELOG( "Lua run string failed. %s(%s)", szUTFString, err_msg );
				_cprint( "Lua run string failed. %s(%s)\n", szUTFString, err_msg );

				lua_pop( pLuaState, 1 );
				return;
			}
			lua_settop( pLuaState, pre_idx );	// 스텍 원상복구
		}

		char szUTFString[ 3072 ];
		struct lua_State * pLuaState;
	} scriptFunctor( strScript.c_str(), L );

	int nAffectedUserCount = InstanceDungeonManager::Instance().DoEachPlayerInInstanceDungeon( nInstanceDungeonID, nLayer, scriptFunctor );

	lua_pushnumber( L, nAffectedUserCount );
	return 1;
}

struct ChatBroadcastFunctor : public ArObjectFunctor
{
	ChatBroadcastFunctor( CHAT_TYPE eChatType, const char * pszChat )
		: m_eChatType( eChatType )
		, m_pszChat( pszChat )
	{
		switch( eChatType )
		{
		case CHAT_DUNGEON_SYSTEM:	m_pszSender = "@DUNGEON"; break;
		case CHAT_NOTICE:			m_pszSender = "@NOTICE"; break;
		default:					m_pszSender = ""; break;
		}

	}

	virtual void operator()( ArObject * pObj ) const
	{
		SendChatMessage( false, m_eChatType, m_pszSender, static_cast< StructPlayer * >( pObj ), m_pszChat );
	}

	const CHAT_TYPE		m_eChatType;
	const char *		m_pszChat;
	const char *		m_pszSender;
};

int SCRIPT_BroadcastMissionTitle( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isstring( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_BroadcastMissionTitle() : invalid argument" );
		return 1;
	}

	// 타이틀 내 사용 금지 문자 체크
	const std::string & strTitle = lua_tostring_utf8( L, 2 );

	if( strTitle.find( '|' ) != std::string::npos )
	{
		lua_pushstring( L, "invalid title" );
		LUA()->Log( "SCRIPT_BroadcastMissionTitle() : invalid title" );
		return 1;
	}

	int nAffectedUserCount = 0;
	int nBroadcastType = lua_tonumber( L, 1 );
	if( nBroadcastType == 1 )
	{
		if( nArgCnt < 4 || !lua_isnumber( L, 3 ) || !lua_isnumber( L, 4 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastMissionTitle() : invalid argument" );
			return 1;
		}

		int nInstanceDungeonID = (int)lua_tonumber( L, 3 );
		unsigned char nLayer = (unsigned char)lua_tonumber( L, 4 );

		char szBuffer[ 1024 ];
		s_sprintf( szBuffer, _countof( szBuffer ), "MTITLE|%s|", strTitle.c_str() );

		ChatBroadcastFunctor cbf( CHAT_DUNGEON_SYSTEM, szBuffer );
		nAffectedUserCount = InstanceDungeonManager::Instance().DoEachPlayerInInstanceDungeon( nInstanceDungeonID, nLayer, cbf );
	}
	else if( nBroadcastType == 2 )
	{
		if( nArgCnt < 5 || !lua_isnumber( L, 3 ) || !lua_isnumber( L, 4 ) || !lua_isnumber( L, 5 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastMissionTitle() : invalid argument" );
			return 1;
		}

		int nArenaID = (int)lua_tonumber( L, 3 );
		unsigned char nInstanceNo = (unsigned char)lua_tonumber( L, 4 );
		int nTeamNo = (int)lua_tonumber( L, 5 );

		char szBuffer[ 1024 ];
		s_sprintf( szBuffer, _countof( szBuffer ), "MTITLE|%s|", strTitle.c_str() );

		ChatBroadcastFunctor cbf( CHAT_DUNGEON_SYSTEM, szBuffer );
		// 팀 구분 없이 전원 방송
		if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleInstance( nArenaID, nInstanceNo, cbf );
		// 특정 팀만 방송
		else
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleTeam( nArenaID, nInstanceNo, nTeamNo, cbf );
	}

	lua_pushnumber( L, nAffectedUserCount );
	return 1;
}

int SCRIPT_BroadcastMissionReward( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isstring( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_BroadcastMissionReward() : invalid argument" );
		return 1;
	}

	// 타이틀 내 사용 금지 문자 체크
	const std::string & strReward = lua_tostring_utf8( L, 2 );

	if( strReward.find( '|' ) != std::string::npos )
	{
		lua_pushstring( L, "invalid reward" );
		LUA()->Log( "SCRIPT_BroadcastMissionReward() : invalid reward" );
		return 1;
	}

	int nAffectedUserCount = 0;
	int nBroadcastType = lua_tonumber( L, 1 );
	if( nBroadcastType == 1 )
	{
		if( nArgCnt < 4 || !lua_isnumber( L, 3 ) || !lua_isnumber( L, 4 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastMissionReward() : invalid argument" );
			return 1;
		}

		int nInstanceDungeonID = (int)lua_tonumber( L, 3 );
		unsigned char nLayer = (unsigned char)lua_tonumber( L, 4 );

		char szBuffer[ 1024 ];
		s_sprintf( szBuffer, _countof( szBuffer ), "MREWARD|%s|", strReward.c_str() );

		ChatBroadcastFunctor cbf( CHAT_DUNGEON_SYSTEM, szBuffer );
		nAffectedUserCount = InstanceDungeonManager::Instance().DoEachPlayerInInstanceDungeon( nInstanceDungeonID, nLayer, cbf );
	}
	else if( nBroadcastType == 2 )
	{
		if( nArgCnt < 5 || !lua_isnumber( L, 3 ) || !lua_isnumber( L, 4 ) || !lua_isnumber( L, 5 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastMissionReward() : invalid argument" );
			return 1;
		}

		int nArenaID = (int)lua_tonumber( L, 3 );
		unsigned char nInstanceNo = (unsigned char)lua_tonumber( L, 4 );
		int nTeamNo = (int)lua_tonumber( L, 5 );

		char szBuffer[ 1024 ];
		s_sprintf( szBuffer, _countof( szBuffer ), "MREWARD|%s|", strReward.c_str() );

		ChatBroadcastFunctor cbf( CHAT_DUNGEON_SYSTEM, szBuffer );
		// 팀 구분 없이 전원 방송
		if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleInstance( nArenaID, nInstanceNo, cbf );
		// 특정 팀만 방송
		else
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleTeam( nArenaID, nInstanceNo, nTeamNo, cbf );
	}

	lua_pushnumber( L, nAffectedUserCount );
	return 1;
}

int SCRIPT_BroadcastMissionObjective( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 4 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isnumber( L, 3 ) || !lua_isstring( L, 4 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_BroadcastMissionReward() : invalid argument" );
		return 1;
	}

	int nObjectiveIndex = (int)lua_tonumber( L, 2 );
	int nMaxProgress = (int)lua_tonumber( L, 3 );
	const std::string & strObjective = lua_tostring_utf8( L, 4 );

	if( nObjectiveIndex < 0 || nObjectiveIndex > InstanceDungeonManager::MAX_OBJECTIVE_INDEX || strObjective.find( '|' ) != std::string::npos )
	{
		lua_pushstring( L, "invalid value" );
		LUA()->Log( "SCRIPT_BroadcastMissionObjective() : invalid value" );
		return 1;
	}

	int nAffectedUserCount = 0;
	int nBroadcastType = lua_tonumber( L, 1 );
	if( nBroadcastType == 1 )
	{
		if( nArgCnt < 6 || !lua_isnumber( L, 5 ) || !lua_isnumber( L, 6 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastMissionObjective() : invalid argument" );
			return 1;
		}

		int nInstanceDungeonID = (int)lua_tonumber( L, 5 );
		unsigned char nLayer = (unsigned char)lua_tonumber( L, 6 );

		char szBuffer[ 1024 ];
		s_sprintf( szBuffer, _countof( szBuffer ), "MOBJECTIVE|%d|%d|%s|", nObjectiveIndex, nMaxProgress, strObjective.c_str() );

		ChatBroadcastFunctor cbf( CHAT_DUNGEON_SYSTEM, szBuffer );
		nAffectedUserCount = InstanceDungeonManager::Instance().DoEachPlayerInInstanceDungeon( nInstanceDungeonID, nLayer, cbf );
	}
	else if( nBroadcastType == 2 )
	{
		if( nArgCnt < 7 || !lua_isnumber( L, 5 ) || !lua_isnumber( L, 6 ) || !lua_isnumber( L, 7 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastMissionObjective() : invalid argument" );
			return 1;
		}

		int nArenaID = (int)lua_tonumber( L, 5 );
		unsigned char nInstanceNo = (unsigned char)lua_tonumber( L, 6 );
		int nTeamNo = (int)lua_tonumber( L, 7 );

		char szBuffer[ 1024 ];
		s_sprintf( szBuffer, _countof( szBuffer ), "MOBJECTIVE|%d|%d|%s|", nObjectiveIndex, nMaxProgress, strObjective.c_str() );

		ChatBroadcastFunctor cbf( CHAT_DUNGEON_SYSTEM, szBuffer );
		// 팀 구분 없이 전원 방송
		if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleInstance( nArenaID, nInstanceNo, cbf );
		// 특정 팀만 방송
		else
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleTeam( nArenaID, nInstanceNo, nTeamNo, cbf );
	}

	lua_pushnumber( L, nAffectedUserCount );
	return 1;
}

int SCRIPT_BroadcastMissionObjectiveProgress( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isnumber( L, 3 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_BroadcastMissionObjectiveProgress() : invalid argument" );
		return 1;
	}

	int nObjectiveIndex = (int)lua_tonumber( L, 2 );
	int nMaxProgress = (int)lua_tonumber( L, 3 );

	if( nObjectiveIndex < 0 || nObjectiveIndex > InstanceDungeonManager::MAX_OBJECTIVE_INDEX )
	{
		lua_pushstring( L, "invalid value" );
		LUA()->Log( "SCRIPT_BroadcastMissionObjectiveProgress() : invalid value" );
		return 1;
	}

	int nAffectedUserCount = 0;
	int nBroadcastType = lua_tonumber( L, 1 );
	if( nBroadcastType == 1 )
	{
		if( nArgCnt < 5 || !lua_isnumber( L, 4 ) || !lua_isnumber( L, 5 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastMissionObjectiveProgress() : invalid argument" );
			return 1;
		}

		int nInstanceDungeonID = (int)lua_tonumber( L, 4 );
		unsigned char nLayer = (unsigned char)lua_tonumber( L, 5 );

		char szBuffer[ 1024 ];
		s_sprintf( szBuffer, _countof( szBuffer ), "MPROGRESS|%d|%d|", nObjectiveIndex, nMaxProgress );

		ChatBroadcastFunctor cbf( CHAT_DUNGEON_SYSTEM, szBuffer );
		nAffectedUserCount = InstanceDungeonManager::Instance().DoEachPlayerInInstanceDungeon( nInstanceDungeonID, nLayer, cbf );
	}
	else if( nBroadcastType == 2 )
	{
		if( nArgCnt < 6 || !lua_isnumber( L, 4 ) || !lua_isnumber( L, 5 ) || !lua_isnumber( L, 6 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastMissionObjective() : invalid argument" );
			return 1;
		}

		int nArenaID = (int)lua_tonumber( L, 4 );
		unsigned char nInstanceNo = (unsigned char)lua_tonumber( L, 5 );
		int nTeamNo = (int)lua_tonumber( L, 6 );

		char szBuffer[ 1024 ];
		s_sprintf( szBuffer, _countof( szBuffer ), "MPROGRESS|%d|%d|", nObjectiveIndex, nMaxProgress );

		ChatBroadcastFunctor cbf( CHAT_DUNGEON_SYSTEM, szBuffer );
		// 팀 구분 없이 전원 방송
		if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleInstance( nArenaID, nInstanceNo, cbf );
		// 특정 팀만 방송
		else
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleTeam( nArenaID, nInstanceNo, nTeamNo, cbf );
	}

	lua_pushnumber( L, nAffectedUserCount );
	return 1;
}

int SCRIPT_BroadcastNotice( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isstring( L, 2 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_BroadcastNotice() : invalid argument" );
		return 1;
	}
	
	const std::string & strNotice = lua_tostring_utf8( L, 2 );

	int nAffectedUserCount = 0;
	int nBroadcastType = lua_tonumber( L, 1 );
	if( nBroadcastType == 1 )
	{
		if( nArgCnt < 4 || !lua_isnumber( L, 3 ) || !lua_isnumber( L, 4 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastNotice() : invalid argument" );
			return 1;
		}

		int nInstanceDungeonID = (int)lua_tonumber( L, 3 );
		unsigned char nLayer = (unsigned char)lua_tonumber( L, 4 );
		
		ChatBroadcastFunctor cbf( CHAT_NOTICE, strNotice.c_str() );
		nAffectedUserCount = InstanceDungeonManager::Instance().DoEachPlayerInInstanceDungeon( nInstanceDungeonID, nLayer, cbf );
	}
	else if( nBroadcastType == 2 )
	{
		if( nArgCnt < 5 || !lua_isnumber( L, 3 ) || !lua_isnumber( L, 4 ) || !lua_isnumber( L, 5 ) )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_BroadcastNotice() : invalid argument" );
			return 1;
		}

		int nArenaID = (int)lua_tonumber( L, 3 );
		unsigned char nInstanceNo = (unsigned char)lua_tonumber( L, 4 );
		int nTeamNo = (int)lua_tonumber( L, 5 );

		ChatBroadcastFunctor cbf( CHAT_NOTICE, strNotice.c_str() );
		// 팀 구분 없이 전원 방송
		if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleInstance( nArenaID, nInstanceNo, cbf );
		// 특정 팀만 방송
		else
			nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleTeam( nArenaID, nInstanceNo, nTeamNo, cbf );
	}

	lua_pushnumber( L, nAffectedUserCount );
	return 1;
}

int SCRIPT_ActivateBattleArenaProp( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_ActivateBattleArenaProp() : invalid argument" );
		return 1;
	}

	int nPropIndex = (int)lua_tonumber( L, 1 );

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer * pPlayer = *pit;

	if( !pPlayer )
	{
		assert( 0 );
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_ActivateBattleArenaProp() : invalid argument" );
		return 1;
	}

	unsigned short nResult = BattleArenaManager::Instance().ActivateBattleInstanceProp( pPlayer, nPropIndex );

	if( nResult != RESULT_SUCCESS )
	{
		const char * pszErrorDesc = "not specified";
		switch( nResult )
		{
			// 에러 메시지 없이 종료해야 하는 경우(일반적으로 발생할 수 있는 오류 케이스)
		case RESULT_NOT_EXIST:
		case RESULT_NOT_ACTABLE:
		case RESULT_NOT_READY:
		case RESULT_ALREADY_EXIST:
			lua_pushnumber( L, nResult );
			return 1;

		case RESULT_INVALID_ARGUMENT:		pszErrorDesc = "invalid argument"; break;
		case RESULT_ACCESS_DENIED:			pszErrorDesc = "access denied"; break;

		default:
			assert( 0 );
			break;
		}

		lua_pushstring( L, pszErrorDesc );
		LUA()->LogPrintf( "SCRIPT_ActivateBattleArenaProp() : %s", pszErrorDesc );
		return 1;
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_SetBattleArenaInstanceFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 4 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isstring( L, 3 ) || !lua_isstring( L, 4 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetBattleArenaInstanceFlag() : invalid argument" );
		return 1;
	}

	int nArenaID = (int)lua_tonumber( L, 1 );
	unsigned char nInstanceNo = (unsigned char)lua_tonumber( L, 2 );
	const std::string & strName = lua_tostring_utf8( L, 3 );
	const std::string & strValue = lua_tostring_utf8( L, 4 );
	
	bool bSuccess = BattleArenaManager::Instance().SetBattleInstanceFlag( nArenaID, nInstanceNo, strName.c_str(), strValue.c_str() );

	lua_pushboolean( L, bSuccess );
	return 1;
}

int SCRIPT_GetBattleArenaInstanceFlag( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isstring( L, 3 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_GetBattleArenaInstanceFlag() : invalid argument" );
		return 1;
	}

	int nArenaID = (int)lua_tonumber( L, 1 );
	unsigned char nInstanceNo = (unsigned char)lua_tonumber( L, 2 );
	const std::string & strName = lua_tostring_utf8( L, 3 );
	
	const std::string & strValue = BattleArenaManager::Instance().GetBattleInstanceFlag( nArenaID, nInstanceNo, strName.c_str() );

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

int SCRIPT_DoEachPlayerInBattleArenaInstance( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isstring( L, 3 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DoEachPlayerInBattleArenaInstance() : invalid argument" );
		return 1;
	}

	int nArenaID = (int)lua_tonumber( L, 1 );
	unsigned char nInstanceNo = (unsigned char)lua_tonumber( L, 2 );
	const std::string & strScript = lua_tostring_utf8( L, 3 );

	// 스크립트가 너무 길면 실패
	if( strScript.size() > 1024 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DoEachPlayerInBattleArenaInstance() : too long script" );
		return 1;
	}

	// 디버그 버전일 때만 스크립트에 사용되면 안 되는 함수가 사용되는지 체크
#ifdef _DEBUG
	std::vector< std::string > vTokens;
	XStringUtil::Split( strScript.c_str(), vTokens, " \t\r\n+-*=<>~/,()[]'\\", false );

	for( std::vector< std::string >::const_iterator it = vTokens.begin() ; it != vTokens.end() ; ++it )
	{
		if( !_stricmp( (*it).c_str(), "force_monster_proc_dead" ) ||
			!_stricmp( (*it).c_str(), "saveall" ) ||
			!_stricmp( (*it).c_str(), "kill" ) ||
			!_stricmp( (*it).c_str(), "kill_target" ) ||
			XStringUtil::WildCardCmp( "*_instance*", (*it).c_str() ) ||
			XStringUtil::WildCardCmp( "broadcast_mission_*", (*it).c_str() ) )
		{
			assert( 0 );
			lua_pushstring( L, "invalid script" );
			LUA()->Log( "SCRIPT_DoEachPlayerInBattleArenaInstance() : invalid script" );
			return 1;
		}
	}
#endif

	struct ScriptFunctor : public ArObjectFunctor
	{
		ScriptFunctor( const char * pszScript, struct lua_State * _pLuaState )
			: pLuaState( _pLuaState )
		{
			int nCodePage = ENV().GetInt( "CodePage", CP_ACP );

			wchar_t wszString[ 1024 ];
			MultiByteToWideChar( nCodePage, 0, pszScript, -1, wszString, _countof( wszString ) );
			WideCharToMultiByte( CP_UTF8, 0, wszString, -1, szUTFString, _countof( szUTFString ), NULL, NULL );
		}

		virtual void operator()( ArObject * pObj ) const
		{
			ThreadPlayerHelper TPHelper( static_cast< StructPlayer * >( pObj ) );
			int pre_idx = lua_gettop( pLuaState );
			if( luaL_dostring( pLuaState, szUTFString ) != 0 )
			{
				const char* err_msg = lua_tostring( pLuaState, -1 );
				FILELOG( "Lua run string failed. %s(%s)", szUTFString, err_msg );
				_cprint( "Lua run string failed. %s(%s)\n", szUTFString, err_msg );

				lua_pop( pLuaState, 1 );
				return;
			}
			lua_settop( pLuaState, pre_idx );	// 스텍 원상복구
		}

		char szUTFString[ 3072 ];
		struct lua_State * pLuaState;
	} scriptFunctor( strScript.c_str(), L );

	int nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleInstance( nArenaID, nInstanceNo, scriptFunctor );

	lua_pushnumber( L, nAffectedUserCount );
	return 1;
}

int SCRIPT_DoEachPlayerInBattleArenaTeam( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 4 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isnumber( L, 3 ) || !lua_isstring( L, 4 ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DoEachPlayerInBattleArenaTeam() : invalid argument" );
		return 1;
	}

	int nArenaID = (int)lua_tonumber( L, 1 );
	unsigned char nInstanceNo = (unsigned char)lua_tonumber( L, 2 );
	int nTeamNo = (int)lua_tonumber( L, 3 );
	const std::string & strScript = lua_tostring_utf8( L, 4 );

	// 스크립트가 너무 길면 실패
	if( strScript.size() > 1024 )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DoEachPlayerInBattleArenaTeam() : too long script" );
		return 1;
	}

	// 디버그 버전일 때만 스크립트에 사용되면 안 되는 함수가 사용되는지 체크
#ifdef _DEBUG
	std::vector< std::string > vTokens;
	XStringUtil::Split( strScript.c_str(), vTokens, " \t\r\n+-*=<>~/,()[]'\\", false );

	for( std::vector< std::string >::const_iterator it = vTokens.begin() ; it != vTokens.end() ; ++it )
	{
		if( !_stricmp( (*it).c_str(), "force_monster_proc_dead" ) ||
			!_stricmp( (*it).c_str(), "saveall" ) ||
			!_stricmp( (*it).c_str(), "kill" ) ||
			!_stricmp( (*it).c_str(), "kill_target" ) ||
			XStringUtil::WildCardCmp( "*_instance*", (*it).c_str() ) ||
			XStringUtil::WildCardCmp( "broadcast_mission_*", (*it).c_str() ) )
		{
			assert( 0 );
			lua_pushstring( L, "invalid script" );
			LUA()->Log( "SCRIPT_DoEachPlayerInBattleArenaTeam() : invalid script" );
			return 1;
		}
	}
#endif

	struct ScriptFunctor : public ArObjectFunctor
	{
		ScriptFunctor( const char * pszScript, struct lua_State * _pLuaState )
			: pLuaState( _pLuaState )
		{
			int nCodePage = ENV().GetInt( "CodePage", CP_ACP );

			wchar_t wszString[ 1024 ];
			MultiByteToWideChar( nCodePage, 0, pszScript, -1, wszString, _countof( wszString ) );
			WideCharToMultiByte( CP_UTF8, 0, wszString, -1, szUTFString, _countof( szUTFString ), NULL, NULL );
		}

		virtual void operator()( ArObject * pObj ) const
		{
			ThreadPlayerHelper TPHelper( static_cast< StructPlayer * >( pObj ) );
			int pre_idx = lua_gettop( pLuaState );
			if( luaL_dostring( pLuaState, szUTFString ) != 0 )
			{
				const char* err_msg = lua_tostring( pLuaState, -1 );
				FILELOG( "Lua run string failed. %s(%s)", szUTFString, err_msg );
				_cprint( "Lua run string failed. %s(%s)\n", szUTFString, err_msg );

				lua_pop( pLuaState, 1 );
				return;
			}
			lua_settop( pLuaState, pre_idx );	// 스텍 원상복구
		}

		char szUTFString[ 3072 ];
		struct lua_State * pLuaState;
	} scriptFunctor( strScript.c_str(), L );

	int nAffectedUserCount = (int)BattleArenaManager::Instance().DoEachPlayerInBattleTeam( nArenaID, nInstanceNo, nTeamNo, scriptFunctor );

	lua_pushnumber( L, nAffectedUserCount );
	return 1;
}

int SCRIPT_DBTraceDump( struct lua_State *L )
{
	g_DBTracker.dump();
	return 0;
}


// 이하 ShutdownManager
ShutdownManager::ShutdownManager()
	: m_nShutdownTime( 0 )
	, m_nShutdownState( SDS_ON_SERVICE )
	, m_nCountdown( 10 )
	, m_bNeedToCreateDump( false )
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );
}

void ShutdownManager::Shutdown( int nLastSec, bool bNeedToCreateDump )
{
	if( m_nShutdownState != SDS_ON_SERVICE )
	{
		_lprint( "GameLog.txt", "Shutdown is called with state 0x%08d, %d.\n", m_nShutdownState, m_nCountdown );
		m_nShutdownState = SDS_ON_SERVICE;
		m_nCountdown = 10;
	}
	_lprint( "GameLog.txt", "Shutdown process is scheduled after %d sec.\n", nLastSec );

	EndAccept();

	m_nShutdownTime = GetArTime() + nLastSec * 100;
	m_nShutdownState |= SDS_BEFORE_NOTICE;

	if( nLastSec < 60 )
		m_nShutdownState |= SDS_AFTER_NOTICE_60SEC;

	if( nLastSec < 30 )
		m_nShutdownState |= SDS_AFTER_NOTICE_30SEC;

	if( nLastSec < 10 )
		m_nCountdown = nLastSec;

	m_bNeedToCreateDump = bNeedToCreateDump;

	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
}

void ShutdownManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "ShutdownManager" );

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "ShutdownManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = GetArTime();

	if( !m_nShutdownTime )
		return;

	AR_TIME tCurrent = GetArTime();

	extern ConnectionCloser g_ConnectionCloser;
	if( m_nShutdownTime >= tCurrent )
	{
		if( !( m_nShutdownState & SDS_AFTER_NOTICE_60SEC ) && tCurrent + 6000 > m_nShutdownTime )
		{
			char * sz1MinString = "@115";
			SendGlobalChatMessage( CHAT_NOTICE, "@NOTICE", sz1MinString, static_cast< unsigned >( strlen(sz1MinString) ) );
			m_nShutdownState |= SDS_AFTER_NOTICE_60SEC;
			_lprint( "GameLog.txt", "Shutdown after 60 sec. is noticed.\n" );
			return;
		}

		if( !( m_nShutdownState & SDS_AFTER_NOTICE_30SEC ) && tCurrent + 3000 > m_nShutdownTime )
		{
			char * sz30SecString = "@116";
			SendGlobalChatMessage( CHAT_NOTICE, "@NOTICE", sz30SecString, static_cast< unsigned >( strlen(sz30SecString) ) );
			m_nShutdownState |= SDS_AFTER_NOTICE_30SEC;
			_lprint( "GameLog.txt", "Shutdown after 30 sec. is noticed.\n" );
			return;
		}

		if( tCurrent + 1000 < m_nShutdownTime )
		{
			return;
		}

		if( !( m_nShutdownState & SDS_NOTICE_COMPLETE ) )
		{
			while( m_nCountdown > 0 && tCurrent + ( m_nCountdown * 100 ) > m_nShutdownTime )
			{
				char buf[ 3 ];
				s_sprintf( buf, _countof( buf ), "%d", m_nCountdown );
				SendGlobalChatMessage( CHAT_NOTICE, "@NOTICE", buf, static_cast< unsigned >( strlen(buf) ) );

				--m_nCountdown;
			}

			if( m_nCountdown > 0 )
				return;

			m_nShutdownState |= SDS_NOTICE_COMPLETE;
			_lprint( "GameLog.txt", "Shutdown notice is completed.\n" );
		}

		ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );

		// 완전히 종료 시각이 될 때까지 대기(랜덤 여유 초 추가 - 유저가 서버 다운 타이밍을 정확히 알 수 없도록)
		int nRandomDelay = XRandom( 1, 4 );
		AR_TIME nShutdownTime = m_nShutdownTime + nRandomDelay * 100;
		while( nShutdownTime > GetArTime() ) Sleep( 0 );

		// 접속 종료된 캐릭터들이 월드에 넋놓고 서서 죽지 않도록 접속 종료 처리를 쵸습히드하게 처리해 줌
		ArcadiaServer::Instance().SetObjectPriority( &g_ConnectionCloser, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

		// 모든 유저들이 로그아웃 대기시간 없이 즉시 로그아웃되도록 로그아웃 타이머를 해제시킴
		GameRule::nLogoutTimer = 0;
	}
	else
		ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );

	// 전부 접속종료 처리 걸어놓고 DB 처리 다 끝나지 않았으면 바로 리턴(다음 onProcess 호출 시점에 다시 체크)
	// * 함수를 종료하지 않고 다 끝날 때까지 기다리면 ArScheduler에서 ShutdownManager::onProcess와 같은 쓰레드에 묶인
	//   ArSchedulerObject 들의 onProcess가 호출되지 못하게 되므로 일부 유저들의 로그아웃 처리가 제대로 진행되지 않는다.
	extern ConnectionManager g_ConnectionManager;
	if( g_ConnectionManager.GetConnectionCount() )
		g_ConnectionManager.DisconnectAll();
	// 로그아웃 상태가 비정상적으로 걸려서 플레이어 카운트가 0으로 내려가는 체크가 영원히 풀리지 않을 가능성을 고려해
	// 해당 체크는 2분 동안만 하도록 함
	if( g_ConnectionManager.GetConnectionCount() || g_ConnectionCloser.GetClosedConnectionCount() ||
		DB().GetActiveThreadCount() + DB().GetWaitingWorkCount() || ( StructPlayer::GetPlayerCount() && tCurrent < m_nShutdownTime + 12000 ) )
	{
		ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGH );
		return;
	}

	m_nShutdownState |= SDS_WRAPUP_COMPLETE;
	_lprint( "GameLog.txt", "Shutdown wrap-up stage is completed.\n" );

	extern HWND hWnd;

	if( m_bNeedToCreateDump )
		XSEH::InvokeUnhandledException( (LONG)0xC000000DL );
	else
		::PostMessage( hWnd, WM_CLOSE, 0, 0 );
}
