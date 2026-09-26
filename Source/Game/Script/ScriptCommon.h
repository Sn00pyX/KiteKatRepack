#pragma once

#include "StructPlayer.h"

void ScriptLog( const char *szStr );
StructPlayer::iterator getPlayer( struct lua_State *L, int nArgNum );
const char *getStringResource( const char *szString );

// 스크립트 내에서 월드에 객체를 리젠시킬 때 지역락을 걸고 리젠시키면 캐난감해지므로(서버에서 락을 걸고 스크립트를 실행하는 경우 중첩락 100%~)
// ObjectRespawner를 만들어서 리젠을 예약시켜두고 ObjectRespawner::onProcess에서 다른 락과 무관하게 지역락 걸고 리젠시키도록 하기 위한 도구

// 활용도가 높아짐에 따라 객체를 매번 생성하고 삭제할 필요없이 static으로 사용할 수 있도록 수정보완 필요
struct ObjectRespawner : public ArSchedulerObject
{
	ObjectRespawner( struct GameObject * pObject )
	{
		m_vObject.push_back( pObject );
	}

	ObjectRespawner( std::vector< struct GameObject * > vObject )
		: m_vObject( vObject )
	{}
	
	virtual void onProcess( int nThreadIdx );
	virtual bool ProcDelete()					{ delete this; return true; }

	std::vector< struct GameObject * > m_vObject;
};

#define SCRIPT_ERROR( e )	lua_pushstring_utf8( L, e ); LUA()->Log( e ); return 1;