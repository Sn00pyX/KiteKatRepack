
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>
#include <mmo/ArcadiaServer.h>

#include "ScriptCommon.h"
#include "StructPlayer.h"
#include "StructMonster.h"
#include "StructNPC.h"
#include "StructFieldProp.h"
#include "SendMessage.h"
#include "GameContent.h"
#include "GameProc.h"
#include "GameMessage.h"
#include "LuaVM.h"

#include <lua/lua.hpp>

void ScriptLog( const char *szStr )
{
	StructPlayer *pPlayer = GetCurrentThreadPlayer();

	if( pPlayer )
	{
		// 플레이어에게 결과 전송
		SendChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, szStr, static_cast< unsigned >( strlen(szStr) ) );
	}
	else
	{
		FILELOG( szStr );
		_cprint( szStr );
		// 화면에 뿌림
	}
}


const char *getStringResource( const char *szString )
{
	if( !szString ) return "";

	char t = *szString;
	if( t != '@' && t != '#' ) return szString;

	int nID = atoi( szString+1 );
	if( !nID ) return szString;

	return GameContent::GetString( nID );
}

StructPlayer::iterator getPlayer( struct lua_State *L, int nArgNum )
{
	int n = lua_gettop(L);

	if( n == nArgNum - 1 )
	{	
		return StructPlayer::iterator( GetCurrentThreadPlayer() );
	}

	if( n < 1 ) return NULL;

	// 플레이어 이름이 아님
	if( !lua_isstring( L, nArgNum ) ) return NULL;

	std::string szName = lua_tostring_utf8( L, nArgNum );

	return StructPlayer::get( StructPlayer::FindPlayer( szName.c_str() ) );
}

void ObjectRespawner::onProcess( int nThreadIdx )
{
	ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_IDLE );

	for( std::vector< struct GameObject * >::iterator it = m_vObject.begin() ; it != m_vObject.end() ; ++it )
	{
		struct GameObject * pObject = (*it);

		assert( pObject && !pObject->IsDeleteRequested() );

		if( pObject && !pObject->IsDeleteRequested() )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pObject ) );

			// 타입에 따라 월드에 엔터 방송 날려주는 함수가 다 다르다... -_ - 니미...
			if( pObject->IsMonster() )
				AddMonsterToWorld( static_cast< StructMonster * >( pObject ) );
			else if( pObject->IsNPC() )
				::AddNPCToWorld( static_cast< StructNPC * >( pObject ) );
			else if( pObject->IsItem() )
				AddItemToWorld( static_cast< StructItem * >( pObject ) );
			else if( pObject->IsFieldProp() )
				AddFieldPropToWorld( static_cast< StructFieldProp * >( pObject ) );
		}
	}

	ArcadiaServer::Instance().DeleteObject( this );
	return;
}
