
#include <network/IConnection.h>
#include <toolkit/ILock.h>
#include <mmo/ArOption.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XConsole.h>
#include <toolkit/XEnv.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/XRandom.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "ScriptNPC.h"
#include "StructPlayer.h"
#include "StructMonster.h"
#include "StructSummon.h"
#include "StructNPC.h"
#include "StructSkill.h"
#include "InstanceDungeonManager.h"
#include "FieldPropManager.h"
#include "SendMessage.h"
#include "NPCProc.h"
#include "GameMessage.h"
#include "GameProc.h"
#include "ScriptCommon.h"
#include "GameContent.h"
#include "DungeonManager.h"
#include "ChannelManager.h"
#include "RoamingManager.h"

#include <lua/lua.hpp>


extern XCriticalSection						g_Lock;
extern std::vector< StructNPC* >			g_vNPC;
extern std::vector< StructSummon* >			g_vSummon;

int SCRIPT_GetString( struct lua_State *L )
{
	if( lua_gettop(L) < 1 ) return 0;

	const char *szString = getStringResource( lua_tostring_utf8( L, lua_gettop(L) ).c_str() );

	lua_pushstring_utf8( L, szString );

	return 1;
}

int SCRIPT_AddMonster( struct lua_State *L )
{	
	int n = lua_gettop(L);			// number of arguments

	if( n < 3 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddMonster() : invalid argument" );
		return 1;
	}

	int x = (int)lua_tonumber( L, 1 );
	int y = (int)lua_tonumber( L, 2 );
	int code = (int)lua_tonumber( L, 3 );

	unsigned char layer = 0;
	int count = 1;
	AR_TIME life_time = 0;

	StructPlayer::iterator pit;
	StructPlayer * pPlayer = NULL;
	if( lua_gettop( L ) > 5 && lua_isnumber( L, 6 ) && lua_isnumber( L, 5 ) && lua_isnumber( L, 4 ) )
	{
		count = lua_tonumber( L, 4 );
		life_time = lua_tonumber( L, 5 );
		layer = lua_tonumber( L, 6 );
	}
	else if( lua_gettop( L ) > 4 && lua_isnumber( L, 5 ) && lua_isnumber( L, 4 ) )
	{
		count = lua_tonumber( L, 4 );
		life_time = lua_tonumber( L, 5 );

		pit = getPlayer( L, 6 );
		pPlayer = *pit;
	}
	else if( lua_gettop( L ) > 3 && lua_isnumber( L, 4 ) )
	{
		count = lua_tonumber( L, 4 );

		pit = getPlayer( L, 5 );
		pPlayer = *pit;
	}
	else
	{
		pit = getPlayer( L, 4 );
		pPlayer = *pit;
	}

	if( !life_time )
		life_time = INFINITE_TIME;

	if( pPlayer && n < 6 )
	{
		layer = pPlayer->GetLayer();
	}

	std::vector< GameObject * > vObject;
	std::vector< AR_HANDLE > vHandle;

	int nInstanceDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( ArPosition( x, y ) );
	if( nInstanceDungeonID )
	{
		if( !pPlayer && n < 6 )
		{
			lua_pushstring( L, "invalid argument" );
			LUA()->Log( "SCRIPT_AddMonster() : script calling without specified layer" );
			return 1;
		}

		// 인던에 몬스터를 생성하려고 하는 경우 리스폰 프로세스를 인던 매니저에게 역임
		InstanceDungeonManager::Instance().PendRespawnMonster( nInstanceDungeonID, x, y, layer, code, count, life_time, NULL );
		// 인던에 소환했을 경우 소환된 몬스터 핸들이 아직 없으므로 0 리턴
		lua_pushnumber( L, 0 );
	}
	else
	{
		for( int i = 0; i < count; ++i )
		{
			// 몬스터 생성
			StructMonster *pMob = StructMonster::AllocMonster( code );

			if( !pMob )
			{
				return 0;
			}

			// 생성코드 설정
			pMob->SetGenerateCode( StructMonster::BY_SCRIPT );

			// 좌표 설정
			pMob->SetCurrentXY( x, y );

			pMob->SetCurrentLayer( layer );

			pMob->SetLifeTime( life_time );

			// 지역 락이 중첩해서 걸리는 문제를 방지하기 위해 월드에 등장시키는 걸 ObjectRespawner에게 일임
			vObject.push_back( pMob );
			vHandle.push_back( pMob->GetHandle() );
		}

		ObjectRespawner * pRespawner = new ObjectRespawner( vObject );
		ArcadiaServer::Instance().SetObjectPriority( pRespawner, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

		lua_newtable( L );

		for( int i = 1; i <= count; i++ )
		{
			lua_pushnumber( L, i );
			lua_pushnumber( L, vHandle[i-1] );
			lua_rawset( L, -3 );
		}

		lua_pushstring( L, "n" );
		lua_pushnumber( L, count );
		lua_rawset( L, -3 );
	}

	return 1;
}

int SCRIPT_AddMonster2(struct lua_State* L)
{
	int n = lua_gettop(L);			// number of arguments

	if (n < 1)
	{
		lua_pushstring(L, "invalid argument");
		LUA()->Log("SCRIPT_AddMonster2() : need at least 1 param");
		return 1;
	}

	//add_monster(pdata[base+1],  pdata[base+(j*2)], pdata[base+(j*2)+1] , 0, duration)

	int code = (int)lua_tonumber(L, 1);

	int x = 0;
	int y = 0;

	unsigned char layer = 0;
	int count = 1;
	AR_TIME life_time = 0;

	
	if (n > 2)
	{
		x = (int)lua_tonumber(L, 2);
		y = (int)lua_tonumber(L, 3);
	}

	if (n > 4)
	{
		layer = (unsigned char)lua_tonumber(L, 4);
		life_time = (AR_TIME)lua_tonumber(L, 5);
	}

	if (!life_time)
		life_time = INFINITE_TIME;


	std::vector< GameObject* > vObject;
	std::vector< AR_HANDLE > vHandle;

	// 몬스터 생성
	StructMonster* pMob = StructMonster::AllocMonster(code);

	if (!pMob)
	{
		return 0;
	}

	// 생성코드 설정
	pMob->SetGenerateCode(StructMonster::BY_SCRIPT);

	// 좌표 설정
	pMob->SetCurrentXY(x, y);

	pMob->SetCurrentLayer(layer);

	pMob->SetLifeTime(life_time);

		// 지역 락이 중첩해서 걸리는 문제를 방지하기 위해 월드에 등장시키는 걸 ObjectRespawner에게 일임
	vObject.push_back(pMob);

	lua_pushnumber(L, pMob->GetHandle());

	ObjectRespawner* pRespawner = new ObjectRespawner(vObject);
	ArcadiaServer::Instance().SetObjectPriority(pRespawner, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST);
	return 1;
}


int SCRIPT_SetHome(struct lua_State* L)
{
	// set_home(tmonster,pdata[base+(j*2)], pdata[base+(j*2)+1])
	int n = lua_gettop(L);			// number of arguments

	if (n < 3)
	{
		lua_pushstring(L, "invalid argument");
		LUA()->Log("SCRIPT_SetHome() : invalid argument");
		return 1;
	}

	AR_HANDLE hHandle = static_cast<AR_HANDLE>(lua_tonumber(L, 1));

	StructCreature::iterator it = StructCreature::get(hHandle);

	StructMonster* pMob = static_cast<StructMonster*>(*it);

	int x = lua_tonumber(L, 2);
	int y = lua_tonumber(L, 3);

	pMob->SetReturnPosition(x, y);
	pMob->comeBackHome(false);

	return 1;
}

int SCRIPT_AddInstanceDungeonMonster( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 3 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddInstanceDungeonMonster() : invalid argument" );
		return 1;
	}

	int nRespawnID = (int)lua_tonumber( L, 1 );
	int nInstanceDungeonID = (int)lua_tonumber( L, 2 );
	int nLayer = (int)lua_tonumber( L, 3 );

	// 인던에 몬스터를 생성하려고 하는 경우 리스폰 프로세스를 인던 매니저에게 역임
	if( !InstanceDungeonManager::Instance().PendRespawnMonster( nRespawnID, nInstanceDungeonID, nLayer ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddInstanceDungeonMonster() : failed to regenerate a monster group" );
		return 1;
	}
	
	return 0;	
}

int SCRIPT_AddNPC( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 3 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isnumber( L, 3 ) )
	{
		LUA()->Log( "SCRIPT_AddNPC() : invalid argument" );
		lua_pushnumber( L, 1 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer * pPlayer = *pit;

	int nX = lua_tonumber( L, 1 );
	int nY = lua_tonumber( L, 2 );
	unsigned char nLayer = ( nArgCnt > 3 ) ? lua_tonumber( L, 3 ) : ( ( pPlayer ) ? pPlayer->GetLayer() : 0 );
	int nNPCID = lua_tonumber( L, nArgCnt );

	NPCBase & info( GameContent::GetNPCInfo( nNPCID ) );
	if( !info.id )
	{
		lua_pushnumber( L, 2 );
		return 1;
	}

	StructNPC * pNPC = GameContent::GetNewNPC( info, nLayer );

	pNPC->SetCurrentXY( nX, nY );
	pNPC->SetHP( pNPC->GetMaxHP() );
	pNPC->SetStatus( StructNPC::STATUS_NORMAL );

	// 지역 락이 중첩해서 걸리는 문제를 방지하기 위해 월드에 등장시키는 걸 ObjectRespawner에게 일임
	ObjectRespawner * pRespawner = new ObjectRespawner( pNPC );
	ArcadiaServer::Instance().SetObjectPriority( pRespawner, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_ShowNPC(struct lua_State* L)
{
	// show_npc( pos_x1, pos_y1, code, hide)
	int nArgCnt = lua_gettop(L);

	if (nArgCnt < 4 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4))
	{
		LUA()->Log("SCRIPT_ShowNPC() : invalid argument");
		lua_pushnumber(L, 1);
		return 1;
	}

	StructPlayer::iterator pit = getPlayer(L, nArgCnt + 1);
	StructPlayer* pPlayer = *pit;

	int nX = lua_tonumber(L, 1);
	int nY = lua_tonumber(L, 2);
	int nNPCID = lua_tonumber(L,3);
	int nHide = lua_tonumber(L, 4);

	StructNPC* pNPC = NULL;

	extern std::vector< StructNPC* > g_vNPC;
	for (std::vector< StructNPC* >::iterator it = g_vNPC.begin(); it != g_vNPC.end(); ++it)
	{
		if ((*it)->GetNPCID() == nNPCID && (*it)->GetLayer() == 0 && (*it)->IsInWorld())
		{
			pNPC = (*it);

			ARCADIA_LOCK(ArcadiaServer::Instance().LockObjectWithVisibleRange(pNPC));

			if (nHide == 0)
				pNPC->SetInvisible(false);
			else
				pNPC->SetInvisible(false);

			BroadcastStatusMessage(pNPC);
		}
	}

	lua_pushnumber(L, 0);
	return 1;
}

typedef std::pair< StructFieldProp *, AR_TIME > PropExpirationInfo;
// 아래의 SCRIPT_AddFieldProp에서 리젠시킨 필드 프랍의 등장 제한 시간 경과 처리 및 삭제 처리
struct RespawnedFieldPropManager : public ArSchedulerObject, StructFieldProp::FieldPropDeleteHandler
{
	static RespawnedFieldPropManager & Instance()
	{
		static RespawnedFieldPropManager _instance;
		return _instance;
	}

	void AddFieldPropExpirationInfo( struct StructFieldProp * pProp, const AR_TIME nExpirationTime )
	{
		THREAD_SYNCHRONIZE( m_CS );

		m_vPropExpirationList.push_back( std::make_pair( pProp, nExpirationTime ) );

		if( GetFinalPriority() != UPDATE_PRIORITY_HIGH )
			ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_HIGH );
	}

	virtual void onFieldPropDelete( struct StructFieldProp * pProp )
	{
		{
			THREAD_SYNCHRONIZE( m_CS );

			for( std::vector< PropExpirationInfo >::iterator it = m_vPropExpirationList.begin() ; it != m_vPropExpirationList.end() ; ++it )
			{
				if( (*it).first != pProp )
					continue;

				m_vPropExpirationList.erase( it );
				break;
			}

			if( m_vPropExpirationList.empty() && GetFinalPriority() != UPDATE_PRIORITY_IDLE )
				ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_IDLE );
		}

		const GameContent::FIELD_PROP_RESPAWN_INFO * pInfo = pProp->GetRespawnInfo();

		if( pInfo )
			delete pInfo;
	}

	virtual void onProcess( int nThreadIdx )
	{
		char buf[255];
		s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
		ENV().Set( buf, "RespawnedFieldPropManager" );

		AR_TIME tCurrent = GetArTime();

		extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
		s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "RespawnedFieldPropManager(0x%08X)", (UINT_PTR)this );
		s_ThreadInfo.last_execute_time = tCurrent;

		// 지역락 -> m_CS의 순서를 어기지 않도록 만료 처리되어야 할 프랍 정보를 복사해놓고 처리
		std::vector< StructFieldProp * > vExpiredPropList;

		{
			THREAD_SYNCHRONIZE( m_CS );

			for( std::vector< PropExpirationInfo >::iterator it = m_vPropExpirationList.begin() ; it != m_vPropExpirationList.end() ; /* 루프에서 ++it 처리 */ )
			{
				if( (*it).second > tCurrent )
				{
					++it;
					continue;
				}

				vExpiredPropList.push_back( (*it).first );
				it = m_vPropExpirationList.erase( it );
			}

			if( m_vPropExpirationList.empty() && GetFinalPriority() != UPDATE_PRIORITY_IDLE )
				ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_IDLE );
		}

		for( std::vector< StructFieldProp * >::const_iterator it = vExpiredPropList.begin() ; it != vExpiredPropList.end() ; ++it )
		{
			StructFieldProp * pProp = (*it);

			// 다른 곳에서 동시에 삭제 처리가 되고 있을 수도 있으므로 실제로 삭제 처리가 여기서 성공한 이후에 delete를 하도록 미리 포인터만 보관
			const GameContent::FIELD_PROP_RESPAWN_INFO * pInfo = pProp->GetRespawnInfo();

			{
				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pProp ) );

				// 지역 락 건 이후에 어디선가 사용한다거나 해서 삭제 처리되었을 경우 건너 뛰기
				if( !pProp->IsInWorld() || !pProp->IsEnable() )
					continue;

				// m_vPropExpirationList에서 이미 삭제된 상태이므로 DeleteHandler를 해제함
				pProp->SetDeleteHandler( NULL );
				ArcadiaServer::Instance().RemoveObject( pProp );
				StructFieldProp::PendFreeFieldProp( pProp );
			}

			if( pInfo )
				delete pInfo;
		}
	}

protected:
	RespawnedFieldPropManager() : m_CS( "RespawnedFieldPropManager::m_CS" )		{}

	// m_vPropExpirationList를 보호하며 지역락 -> m_CS 의 순서로 걸어야 함
	mutable XCriticalSection			m_CS;
	std::vector< PropExpirationInfo >	m_vPropExpirationList;
};

int SCRIPT_AddFieldProp( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		LUA()->Log( "SCRIPT_AddFieldProp() : invalid argument" );
		lua_pushnumber( L, 1 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer * pPlayer = (*pit);

	if( !pPlayer && ( nArgCnt < 5 || !lua_isnumber( L, 3 ) || !lua_isnumber( L, 4 ) || !lua_isnumber( L, 5 ) ) )
	{
		LUA()->Log( "SCRIPT_AddFieldProp() : invalid argument" );
		lua_pushnumber( L, 1 );
		return 1;
	}

	int nPropId				= lua_tonumber( L, 1 );

	if( !nPropId || FieldPropManager::GetInstance().GetFieldPropBase( nPropId ).nPropId != nPropId )
	{
		LUA()->Log( "SCRIPT_AddFieldProp() : invalid prop id" );
		lua_pushnumber( L, 2 );
		return 1;
	}

	AR_TIME nValidDuration	= lua_tonumber( L, 2 ) * 100;
	AR_UNIT nPosX			= ( nArgCnt >= 3	&& lua_isnumber( L, 3	) ) ? lua_tonumber( L, 3	) : pPlayer->GetX();
	AR_UNIT nPosY			= ( nArgCnt >= 4	&& lua_isnumber( L, 4	) ) ? lua_tonumber( L, 4	) : pPlayer->GetY();
	unsigned char nLayer	= ( nArgCnt >= 5	&& lua_isnumber( L, 5	) ) ? lua_tonumber( L, 5	) : pPlayer->GetLayer();
	float fZOffset			= ( nArgCnt >= 6	&& lua_isnumber( L, 6	) ) ? lua_tonumber( L, 6	) : 0.0f;
	float fRotateX			= ( nArgCnt >= 7	&& lua_isnumber( L, 7	) ) ? lua_tonumber( L, 7	) : 0.0f;
	float fRotateY			= ( nArgCnt >= 8	&& lua_isnumber( L, 8	) ) ? lua_tonumber( L, 8	) : 0.0f;
	float fRotateZ			= ( nArgCnt >= 9	&& lua_isnumber( L, 9	) ) ? lua_tonumber( L, 9	) : 0.0f;
	float fScaleX			= ( nArgCnt >= 10	&& lua_isnumber( L, 10	) ) ? lua_tonumber( L, 10	) : 1.0f;
	float fScaleY			= ( nArgCnt >= 11	&& lua_isnumber( L, 11	) ) ? lua_tonumber( L, 11	) : 1.0f;
	float fScaleZ			= ( nArgCnt >= 12	&& lua_isnumber( L, 12	) ) ? lua_tonumber( L, 12	) : 1.0f;
	bool bLockHeight		= ( nArgCnt >= 13	&& lua_isboolean( L, 13	) ) ? lua_toboolean( L, 13	) : false;
	float fLockHeight		= ( nArgCnt >= 14	&& lua_isnumber( L, 14	) ) ? lua_tonumber( L, 14	) : 0.0f;

	ArPosition pos = ArPosition( nPosX, nPosY );
	int nInstanceDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( pos );

	if( nInstanceDungeonID )
	{
		InstanceDungeonManager::Instance().PendRespawnProp( nInstanceDungeonID, nPropId, nPosX, nPosY, nLayer, fZOffset, fRotateX, fRotateY, fRotateZ, fScaleX, fScaleY, fScaleZ, bLockHeight, fLockHeight );
		lua_pushnumber( L, 0 );
	}
	else
	{
		GameContent::FIELD_PROP_RESPAWN_INFO * pInfo = new GameContent::FIELD_PROP_RESPAWN_INFO(
			nPropId, nPosX, nPosY, nLayer, fZOffset, fRotateX, fRotateY, fRotateZ, fScaleX, fScaleY, fScaleZ, bLockHeight, fLockHeight );

		StructFieldProp * pProp = StructFieldProp::CreateWithoutAddingToWorld( &RespawnedFieldPropManager::Instance(), pInfo );
		if( nValidDuration || pProp->IsExpireObject() )
		{
			RespawnedFieldPropManager::Instance().AddFieldPropExpirationInfo( pProp, GetArTime() + ( ( nValidDuration ) ? nValidDuration : pProp->GetFieldPropBase()->nLifeTime ) );
		}

		// 지역 락이 중첩해서 걸리는 문제를 방지하기 위해 월드에 등장시키는 걸 ObjectRespawner에게 일임
		ObjectRespawner * pRespawner = new ObjectRespawner( pProp );
		ArcadiaServer::Instance().SetObjectPriority( pRespawner, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
		lua_pushnumber(L, pProp->GetHandle());
	}

	//lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_RemoveFieldProp(struct lua_State* L)
{
	int nArgCnt = lua_gettop(L);

	if (nArgCnt < 1 || !lua_isnumber(L, 1))
	{
		LUA()->Log("SCRIPT_RemoveFieldProp() : invalid argument");
		lua_pushnumber(L, 1);
		return 1;
	}

	AR_HANDLE handle = static_cast<AR_HANDLE>(lua_tonumber(L, 1));

	StructFieldProp::iterator it = StructFieldProp::get(handle);
	StructFieldProp* pFieldProp = static_cast<StructFieldProp*>((*it));

	if (!pFieldProp)
		return 1;
	ArcadiaServer::Instance().RemoveObject(pFieldProp);
	StructFieldProp::PendFreeFieldProp(pFieldProp);

	return 1;
}

int SCRIPT_AddDungeonGuardianRespawnInfo( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 8 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) ||
		!lua_isnumber(L, 4) ||
		!lua_isnumber(L, 5) ||
		!lua_isnumber(L, 6) ||
		!lua_isnumber(L, 7) ||
		!lua_isnumber(L, 8) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddDungeonGardianRespawnInfo() : invalid argument" );
		return 1;
	}


	int way_point_id = 0;

	if( n > 8 && lua_isnumber( L, 9 ) )
	{
		way_point_id = lua_tonumber( L, 9 );
	}

	DungeonManager::Instance().RegisterDungeonGuardianRespawnInfo( (int)lua_tonumber( L, 1 ),
	GameContent::MONSTER_RESPAWN_INFO(
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 6 ),
		(int)lua_tonumber( L, 7 ),
		(int)lua_tonumber( L, 7 ),
		(bool)lua_tonumber( L, 8 ),
		way_point_id ) );

	return 0;
}

int SCRIPT_AddDungeonEnvironmentalGuardianRespawnInfo( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 8 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) ||
		!lua_isnumber(L, 4) ||
		!lua_isnumber(L, 5) ||
		!lua_isnumber(L, 6) ||
		!lua_isnumber(L, 7) ||
		!lua_isnumber(L, 8) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddDungeonEnvironmentalGuardianRespawnInfo() : invalid argument" );
		return 1;
	}


	int way_point_id = 0;

	if( n > 8 && lua_isnumber( L, 9 ) )
	{
		way_point_id = lua_tonumber( L, 9 );
	}

	DungeonManager::Instance().RegisterDungeonEnvironmentalGuardianRespawnInfo( (int)lua_tonumber( L, 1 ),
	GameContent::MONSTER_RESPAWN_INFO(
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 6 ),
		(int)lua_tonumber( L, 7 ),
		(int)lua_tonumber( L, 7 ),
		(bool)lua_tonumber( L, 8 ),
		way_point_id ) );

	GameContent::RegisterMonsterRespawnInfo( GameContent::MONSTER_RESPAWN_INFO(
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 6 ),
		(int)lua_tonumber( L, 7 ),
		(int)lua_tonumber( L, 7 ),
		(bool)lua_tonumber( L, 8 ),
		way_point_id ) );

	return 0;
}

int SCRIPT_AddRespawnInfo( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 9 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) ||
		!lua_isnumber(L, 4) ||
		!lua_isnumber(L, 5) ||
		!lua_isnumber(L, 6) ||
		!lua_isnumber(L, 7) ||
		!lua_isnumber(L, 8) ||
		!lua_isnumber(L, 9) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddRespawnInfo() : invalid argument" );
		return 1;
	}

	int way_point_id = 0;

	if( n > 9 && lua_isnumber( L, 10 ) )
	{
		way_point_id = lua_tonumber( L, 10 );
	}

	GameContent::RegisterMonsterRespawnInfo( GameContent::MONSTER_RESPAWN_INFO(
		(int)lua_tonumber( L, 1 ),
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 6 ),
		(int)lua_tonumber( L, 7 ),
		(int)lua_tonumber( L, 8 ),
		(int)lua_tonumber( L, 9 ),
		true,
		way_point_id ) );

	return 0;
}

int SCRIPT_AddRareMobRespawnInfo( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 7 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) ||
		!lua_isnumber(L, 4) ||
		!lua_isnumber(L, 5) ||
		!lua_isnumber(L, 6) ||
		!lua_isnumber(L, 7) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddRareMobRespawnInfo() : invalid argument" );
		return 1;
	}

	int way_point_id = 0;

	if( n > 7 && lua_isnumber( L, 8 ) )
	{
		way_point_id = lua_tonumber( L, 8 );
	}

	GameContent::RegisterMonsterRespawnInfo( GameContent::MONSTER_RESPAWN_INFO(
		(int)lua_tonumber( L, 1 ),
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 3 ) + 1,
		(int)lua_tonumber( L, 4 ) + 1,
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 6 ),
		(int)lua_tonumber( L, 6 ),
		(bool)lua_tonumber( L, 7 ),
		way_point_id ) );

	return 0;
}

int SCRIPT_AddRoamingMobRespawnInfo( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 4 ||
		!lua_isnumber( L, 1 ) ||
		!lua_isnumber( L, 2 ) ||
		!lua_isnumber( L, 3 ) ||
		!lua_isnumber( L, 4 ) ||
		( n >= 5 && !lua_isnumber( L, 5 ) ) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddRoamingMobRespawnInfo() : invalid argument" );
		return 1;
	}

	int nRoamingID = lua_tonumber( L, 1 );
	int nMonsterID = lua_tonumber( L, 2 );
	int nAngle = lua_tonumber( L, 3 );
	AR_UNIT nDistance = lua_tonumber( L, 4 ) * GameRule::DEFAULT_UNIT_SIZE;
	AR_TIME nRespawnInterval = ( n >= 5 ) ? lua_tonumber( L, 5 ) * 100 : 0;		// 리젠 그룹화 로밍 그룹이 아닌 경우 인자가 4개거나 5번째 인자가 0이면 1번 리젠하고 삭제함

	RoamingManager::Instance().AddRoamingCreatureRespawnInfo( nRoamingID, GameContent::ROAMING_CREATURE_RESPAWN_INFO( StructRoamer::ROAMING_CREATURE_MONSTER, nMonsterID, nRespawnInterval, nAngle, nDistance ) );

	return 0;
}

int SCRIPT_AddRaidRespawnInfo( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 9 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) ||
		!lua_isnumber(L, 4) ||
		!lua_isnumber(L, 5) ||
		!lua_isnumber(L, 6) ||
		!lua_isnumber(L, 7) ||
		!lua_isnumber(L, 8) ||
		!lua_isnumber(L, 9) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddRespawnInfo() : invalid argument" );
		return 1;
	}

	int way_point_id = 0;

	if( n > 9 && lua_isnumber( L, 10 ) )
	{
		way_point_id = lua_tonumber( L, 10 );
	}

	GameContent::RegisterRaidMonsterRespawnInfo( GameContent::MONSTER_RESPAWN_INFO(
		(int)lua_tonumber( L, 1 ),
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 6 ),
		(int)lua_tonumber( L, 7 ),
		(int)lua_tonumber( L, 8 ),
		(int)lua_tonumber( L, 9 ),
		true,
		way_point_id ) );

	return 0;
}

int SCRIPT_AddRaidRareMobRespawnInfo( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 7 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) ||
		!lua_isnumber(L, 4) ||
		!lua_isnumber(L, 5) ||
		!lua_isnumber(L, 6) ||
		!lua_isnumber(L, 7) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddRespawnInfo() : invalid argument" );
		return 1;
	}

	int way_point_id = 0;

	if( n > 7 && lua_isnumber( L, 8 ) )
	{
		way_point_id = lua_tonumber( L, 8 );
	}

	GameContent::RegisterRaidMonsterRespawnInfo( GameContent::MONSTER_RESPAWN_INFO(
		(int)lua_tonumber( L, 1 ),
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ),
		(int)lua_tonumber( L, 4 ),
		(int)lua_tonumber( L, 3 ) + 1,
		(int)lua_tonumber( L, 4 ) + 1,
		(int)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 6 ),
		(int)lua_tonumber( L, 6 ),
		(bool)lua_tonumber( L, 7 ),
		way_point_id ) );

	return 0;
}

int SCRIPT_SetRandomRespawnInfo( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments
	bool bExceptRaidSiege = false;

	if( n < 7 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) ||
		!lua_isnumber(L, 4) ||
		!lua_isnumber(L, 5) ||
		!lua_isnumber(L, 6) ||
		!lua_isnumber(L, 7) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetRandomRespawnInfo() : invalid argument" );
		return 1;
	}

	if( n > 7 && lua_isnumber( L, 8 ) )
	{
		bExceptRaidSiege = lua_tonumber( L, 8 );
	}

	GameContent::RegisterRandomMonsterRespawnInfo( GameContent::RANDOM_MONSTER_RESPAWN_INFO(
		(int)lua_tonumber( L, 1 ),
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ),
		(int)lua_tonumber( L, 4 ),
		(bool)lua_tonumber( L, 5 ),
		(int)lua_tonumber( L, 6 ),
		(int)lua_tonumber( L, 7 ),
		bExceptRaidSiege ) );

	return 0;
}

int SCRIPT_AddRandomAreaInfo( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 5 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) ||
		!lua_isnumber(L, 4) ||
		!lua_isnumber(L, 5) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddRandomAreaInfo() : invalid argument" );
		return 1;
	}

	GameContent::AddRandomAreaInfo( lua_tonumber( L, 1 ), lua_tonumber( L, 2 ), lua_tonumber( L, 3 ), lua_tonumber( L, 4), lua_tonumber( L, 5 ) );

	return 0;
}

int SCRIPT_AddRandomMonster( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 3 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddRandomMonster() : invalid argument" );
		return 1;
	}

	GameContent::AddRandomMonster(
		(int)lua_tonumber( L, 1 ),
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ) );

	return 0;
}

int SCRIPT_AddRandomMonsterGroup( struct lua_State *L )
{
	int n = lua_gettop( L );

	if( n < 3 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddRandomMonsterGroup() : invalid argument" );
		return 1;
	}

	GameContent::AddRandomMonsterGroup(
		(int)lua_tonumber( L, 1 ),
		(int)lua_tonumber( L, 2 ),
		(int)lua_tonumber( L, 3 ) );

	return 0;
}

int SCRIPT_DialogTitle( struct lua_State *L )
{
	if( lua_gettop(L) < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	pPlayer->SetDialogTitle( lua_tostring_utf8( L, 1 ).c_str() );

	return 0;
}

typedef std::pair< int /* QuestCode */, int /* TextID */ > QuestLinkInfo;

int ShowQuestMenu( StructPlayer *pPlayer )
{
	StructNPC * pNPC = reinterpret_cast< StructNPC * > (GameObject::raw_get( pPlayer->GetContactNPCHandle() ) );

	if( !pNPC )
		return 0;

	struct myQuestFunctor : public StructNPC::QuestFunctor
	{
		myQuestFunctor( int progress )  : m_QuestProgress( progress )
		{
		}

		bool operator()( struct StructPlayer * pPlayer, const QuestLink &link_info )
		{
			const QuestBase & rQuestBase = StructQuest::GetQuestBase( link_info.code );

			int nTextID = 0;
			switch( m_QuestProgress )
			{
			case TS_SC_QUEST_INFOMATION::IS_STARTABLE:
				nTextID = link_info.nStartTextId;
				break;
			case TS_SC_QUEST_INFOMATION::IS_IN_PROGRESS:
				nTextID = link_info.nInProgressTextId;
				break;
			case TS_SC_QUEST_INFOMATION::IS_FINISHABLE:
				nTextID = link_info.nEndTextId;
				break;
			}

			// QuestLinkResource에 퀘스트의 현재 진행 단계에 대한 flag는 설정되어 있지만 text_id는 0인 경우
			if( !nTextID )
			{
				assert( 0 );
				return true;
			}

			char szBuf[255];

			if( rQuestBase.nType == QuestBase::QUEST_RANDOM_KILL_INDIVIDUAL || rQuestBase.nType == QuestBase::QUEST_RANDOM_COLLECT )
			{
				vRandomQuestInfo.push_back( QuestLinkInfo( link_info.code, nTextID ) );

				// 랜덤 퀘스트의 시작 메뉴는 아래에서 별도로 처리하며 진행 중이거나 완료 가능한 것만 메뉴를 추가함
				if( m_QuestProgress == TS_SC_QUEST_INFOMATION::IS_STARTABLE )
				{
					return true;
				}

				// 진행 중 또는 완료 가능 랜덤퀘는 NPC 대사로 해당 다이얼로그가 출력되도록 강제로 설정함
				s_sprintf( szBuf, _countof( szBuf ), "quest_info( %d, %d, 0 )", link_info.code, nTextID );
			}
			else
			{
				// 일반 퀘는 진행 상태에 따라 창의 타입이 자동 결정되도록 처리(SCRIPT_QuestInfo 함수 구현 참조)
				s_sprintf( szBuf, _countof( szBuf ), "quest_info( %d, %d )", link_info.code, nTextID );
			}

			char szButtonName[126];
			s_sprintf( szButtonName, _countof( szButtonName ), "QUEST|%d|%d", rQuestBase.nQuestTextId, m_QuestProgress );
			pPlayer->AddDialogMenu( szButtonName, szBuf );

			return true;
		}

		int  m_QuestProgress;
		std::vector< QuestLinkInfo > vRandomQuestInfo;
	};

	myQuestFunctor _foStart( TS_SC_QUEST_INFOMATION::IS_STARTABLE );
	myQuestFunctor _foProgress( TS_SC_QUEST_INFOMATION::IS_IN_PROGRESS );
	myQuestFunctor _foEnd( TS_SC_QUEST_INFOMATION::IS_FINISHABLE );

	pNPC->DoEachStartableQuest( pPlayer, _foStart );
	pNPC->DoEachInProgressQuest( pPlayer, _foProgress );
	pNPC->DoEachFinishableQuest( pPlayer, _foEnd );

	// 진행 중이거나 완료 가능한 랜덤 퀘스트가 없다면 새로 시작할 수 있도록 퀘스트 메뉴 추가
	if( !_foProgress.vRandomQuestInfo.size() && !_foEnd.vRandomQuestInfo.size() )
	{
		int max_level = 0;
		QuestLinkInfo qli( 0, 0 );

		// 현재 시작할 수 있는 랜덤 퀘스트 중 가장 제한 레벨이 높은 퀘스트를 골라 보여 줌
		for( std::vector< QuestLinkInfo >::iterator it = _foStart.vRandomQuestInfo.begin(); it != _foStart.vRandomQuestInfo.end(); ++it )
		{
			const int & nLimitLevel = StructQuest::GetQuestBase( (*it).first ).nLimitLevel;
			if( nLimitLevel > max_level )
			{
				qli = (*it);
				max_level = nLimitLevel;
			}
		}

		if( qli.first )
		{
			const QuestBase & rQuestBase = StructQuest::GetQuestBase( qli.first );

			// 랜덤 퀘스트의 경우 NPC 대사로 해당 다이얼로그가 출력되도록 강제로 설정함
			char szBuf[255];
			s_sprintf( szBuf, _countof( szBuf ), "quest_info( %d, %d, 0 )", qli.first, qli.second );

			char szButtonName[126];
			s_sprintf( szButtonName, _countof( szButtonName ), "QUEST|%d|%d", rQuestBase.nQuestTextId, TS_SC_QUEST_INFOMATION::IS_STARTABLE );
			pPlayer->AddDialogMenu( szButtonName, szBuf );

		}
	}

	return 0;
}

int SCRIPT_DialogText( struct lua_State *L )
{
	if( lua_gettop(L) < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	pPlayer->SetDialogText( lua_tostring_utf8( L, 1 ).c_str() );

	ShowQuestMenu( pPlayer );

	return 0;
}

int SCRIPT_QuestTextWithoutQuest( struct lua_State *L )
{
	if( lua_gettop(L) < 2 ) return 0;

	char buf[255];

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	int code = lua_tonumber( L, 1 );
	int text_id = lua_tonumber( L, 2 );

	s_sprintf( buf, _countof( buf ), "QUEST|%d|%d", code, text_id );
	pPlayer->SetDialogText( buf );

	return 0;
}

int SCRIPT_DialogTextWithoutQuest( struct lua_State *L )
{
	if( lua_gettop(L) < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	pPlayer->SetDialogText( lua_tostring_utf8( L, 1 ).c_str() );

	return 0;
}

int SCRIPT_DialogMenu( struct lua_State *L )
{
	if( lua_gettop(L) < 2 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	pPlayer->AddDialogMenu( lua_tostring_utf8( L, 1 ).c_str(), lua_tostring_utf8( L, 2 ).c_str() );

	return 0;
}

int SCRIPT_DialogShow( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1);
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	pPlayer->ShowDialog();

	return 0;
}

int SCRIPT_ShowMarket( struct lua_State *L )
{
	// 상점 이름 안주면 KIN
	if( lua_gettop(L) < 1 ) return 0;

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	// 상점 얻어오자
	_MARKET_INFO *pInfo = NULL;
	pInfo = GameContent::GetMarketInfo( lua_tostring_utf8( L, 1 ).c_str() );
	if( !pInfo ) return 0;

	SendMarketInfo( pPlayer, pPlayer->GetContactNPCHandle(), pInfo );

	return 0;
}

int SCRIPT_ShowCreatureDialog( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1);
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	SendCreatureEquipMessage( pPlayer, true );

	return 0;
}

int SCRIPT_ShowStorage( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1);
	StructPlayer *pPlayer = *pit;

	if( !pPlayer || !pPlayer->pConnection || !pPlayer->pConnection->GetTag() )
		return 0;

	_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pPlayer->pConnection->GetTag() );

	// 보안 비밀번호를 쓰는 옵션이 설정되어 있고
	// 항상 창고비밀번호 확인 옵션이 설정되어 있거나,
	// 창고비밀번호 확인이 한번도 안된경우, 비밀번호 확인해라!!!
	if ( GameRule::bUseSecurityNoForStorage 
		&& ( GameRule::bCheckStorageSecurityAlways || !pTag->bStorageSecurityCheck ) )
		pPlayer->RequestSecurityNo( TS_SC_REQUEST_SECURITY_NO::SC_OPEN_STORAGE );
	else
		pPlayer->OpenStorage();	


	return 0;
}

int	SCRIPT_GetNPCId( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1);
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	if( pPlayer->GetContactNPCHandle() )
	{
		StructCreature::iterator it = StructCreature::get( pPlayer->GetContactNPCHandle() );

		if( *it && (*it)->IsNPC() )
		{
			lua_pushnumber( L, static_cast< StructNPC * >( *it )->GetNPCID() );
			return 1;
		}
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int	SCRIPT_GetNPCName(struct lua_State* L)
{
	StructPlayer::iterator pit = getPlayer(L, 1);
	StructPlayer* pPlayer = *pit;

	if (!pPlayer) return 0;

	if (pPlayer->GetContactNPCHandle())
	{
		StructCreature::iterator it = StructCreature::get(pPlayer->GetContactNPCHandle());

		if (*it && (*it)->IsNPC())
		{
			//lua_pushstring(L, static_cast<StructNPC*>(*it)->GetName());
			lua_pushnumber(L, static_cast<StructNPC*>(*it)->GetNPCBase()->name_text_id);
			return 1;
		}
	}

	lua_pushstring(L, 0);
	return 1;
}

int	SCRIPT_GetNPCType(struct lua_State* L)
{
	StructPlayer::iterator pit = getPlayer(L, 1);
	StructPlayer* pPlayer = *pit;

	if (!pPlayer) return 0;

	if (pPlayer->GetContactNPCHandle())
	{
		StructCreature::iterator it = StructCreature::get(pPlayer->GetContactNPCHandle());

		if (*it && (*it)->IsNPC())
		{
			lua_pushnumber(L, static_cast<StructNPC*>(*it)->GetNPCBase()->text_id);
			return 1;
		}
	}

	lua_pushnumber(L, 0);
	return 1;
}

int SCRIPT_GetNPCHandle( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	if( pPlayer->GetContactNPCHandle() )
	{
		StructCreature::iterator it = StructCreature::get( pPlayer->GetContactNPCHandle() );

		if( *it && (*it)->IsNPC() )
		{
			lua_pushnumber( L, static_cast< StructNPC * >( *it )->GetHandle() );
			return 1;
		}
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_MonsterSkillCast( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 2 || n > 4 )
	{
		LUA()->Log( "SCRIPT_MonsterSkillCast() : invalid argument" );
		return 0;
	}

	int skill_idx = lua_tonumber( L, 1 );
	AR_HANDLE hMonster = lua_tonumber( L, 2 );
	AR_HANDLE hTarget = 0;
	ArPosition posTarget;

	StructMonster::iterator itMonster = StructMonster::get( hMonster );

	if( !(*itMonster) || !(*itMonster)->IsMonster() || !(*itMonster)->IsAlive() )
		return 0;

	StructMonster * pMonster = static_cast< StructMonster * >( *itMonster );

	if( n == 3 && lua_isnumber( L, 3 ) )
	{
		hTarget = lua_tonumber( L, 3 );

		StructMonster::iterator itTarget = StructMonster::get( hTarget );

		if( !(*itTarget) || !(*itTarget)->IsCreature() || !(*itTarget)->IsAlive() )
			return 0;

		posTarget = (*itTarget)->GetCurrentPosition( GetArTime() );
	}
	else if( n == 4 && lua_isnumber( L, 3 ) && lua_isnumber( L, 4 ) )
	{
		posTarget.x = lua_tonumber( L, 3 );
		posTarget.y = lua_tonumber( L, 4 );
	}

	if( pMonster->GetMonsterBase()->skill_info_list->size() <= static_cast< size_t >( skill_idx ) )
	{
		std::string strError;
		XStringUtil::Format( strError, "SCRIPT_MonsterSkillCast() : invalid skill index(Monster:%s(%d), SkillIdx:%d)", pMonster->GetName(), pMonster->GetMonsterId(), skill_idx );
		LUA()->Log( strError.c_str() );
		return 0;
	}

	int nSkillId = pMonster->GetMonsterBase()->skill_info_list->at( skill_idx ).skill_id;
	StructSkill *pSkill = pMonster->GetSkill( nSkillId );

	if( !pSkill )
	{	
		std::string strError;
		XStringUtil::Format( strError, "SCRIPT_MonsterSkillCast() : invalid skill id(Monster:%s(%d), SkillIdx:%d, SkillId:%d)", pMonster->GetName(), pMonster->GetMonsterId(), skill_idx, nSkillId );
		LUA()->Log( strError.c_str() );
		return 0;
	}

	// 현재 스킬 사용 못하는 경우이면 패스
	if( ( pSkill->GetSkillBase()->IsPhysicalSkill() && !pMonster->IsSkillCastable() ) || ( pSkill->GetSkillBase()->IsMagicalSkill() && !pMonster->IsMagicCastable() ) )
	{
		pMonster->SetContinueAttack( true );
		return 0;
	}

	// 캐스팅 실패시 몬스터가 processAttack을 중지하지 않도록 세팅
	if( pMonster->CastSkill( nSkillId, pMonster->GetMonsterBase()->skill_info_list->at( skill_idx ).skill_lv, hTarget, posTarget, pMonster->GetLayer() ) != RESULT_SUCCESS )
		pMonster->SetContinueAttack( true );

	return 0;
}

int SCRIPT_GetMonsterSkillTargetPosition( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetMonsterSkillTargetPosition() : invalid argument" );
		return 1;
	}

	AR_HANDLE hMonster = lua_tonumber( L, 1 );

	StructMonster::iterator it = StructMonster::get( hMonster );

	if( !(*it) || !(*it)->IsMonster() )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructMonster * pMonster = static_cast< StructMonster * >( *it );

	lua_pushnumber( L, pMonster->GetSkillTargetPosition().x );
	lua_pushnumber( L, pMonster->GetSkillTargetPosition().y );

	return 2;
}

int SCRIPT_SetMonsterSkillTargetPosition( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 3 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetMonsterSkillTargetPosition() : invalid argument" );
		return 1;
	}

	AR_HANDLE hMonster = lua_tonumber( L, 1 );

	StructMonster::iterator it = StructMonster::get( hMonster );

	if( !(*it) || !(*it)->IsMonster() )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructMonster * pMonster = static_cast< StructMonster * >( *it );

	pMonster->SetSkillTargetPosition( ArPosition( lua_tonumber( L, 2 ), lua_tonumber( L, 3 ) ) );

	return 0;
}

int SCRIPT_SetNextAttackableTime( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		LUA()->Log( "SCRIPT_SetMonsterNextAttackableTime() : invalid argument" );
		return 0;
	}

	AR_TIME nDelay = static_cast< int >( lua_tonumber( L, 1 ) * 100 );
	AR_HANDLE hTarget = lua_tonumber( L, 2 );

	StructCreature::iterator itTarget = StructCreature::get( hTarget );
	StructCreature *pTarget = (*itTarget);

	if( !pTarget || !pTarget->IsInWorld() || !pTarget->IsAlive() || !pTarget->IsAttacking() )
		return 0;

	AR_TIME t = GetArTime();
	pTarget->SetNextAttackableTime( t + nDelay );
	pTarget->SetMovableTime( t + nDelay );
	return 0;
}

int SCRIPT_GetMonsterID( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetMonsterID() : invalid argument" );
		return 1;
	}

	AR_HANDLE hMonster = lua_tonumber( L, 1 );

	StructMonster::iterator it = StructMonster::get( hMonster );

	if( !(*it) || !(*it)->IsMonster() )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructMonster * pMonster = static_cast< StructMonster * >( *it );

	lua_pushnumber( L, pMonster->GetMonsterId() );

	return 1;
}

int SCRIPT_RespawnNearMonster( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		LUA()->Log( "SCRIPT_RespawnNearMonster() : invalid argument" );
		return 0;
	}

	AR_HANDLE hMonster = lua_tonumber( L, 1 );
	int monster_id = lua_tonumber( L, 2 );
	int cnt = lua_tonumber( L, 3 );

	StructMonster::iterator it = StructMonster::get( hMonster );

	if( !(*it) || !(*it)->IsMonster() || !(*it)->IsAlive() )
		return 0;

	StructMonster * pMonster = static_cast< StructMonster * >( *it );

	AR_UNIT x, y;
	ArPosition pos = pMonster->GetCurrentPosition( GetArTime() );

	for( int i = 0; i < cnt; ++i )
	{
		x = XRandom( pos.x - 60, pos.x + 60 );
		y = XRandom( pos.y - 60, pos.y + 60 );

		if( GameContent::CollisionToLine( pos.x, pos.y, x, y ) )
		{
			x = pos.x;
			y = pos.y;

			if( GameContent::IsBlocked( x, y ) )
			{
				continue;
			}
		}

		StructMonster * pMob = respawnMonster( pos.x, pos.y, pMonster->GetLayer(), monster_id, true, 0, NULL, false );

		if( pMob )
		{
			ArPosition targetPos( x, y );
			pMob->SetMove( targetPos, pMob->GetRealMoveSpeed() * 3 );
			pMob->SetRespawnByScript();
			ArcadiaServer::Instance().SetObjectPriority( pMob, ArObject::UPDATE_PRIORITY_HIGHEST );

			if( pMonster->IsDungeonRaidMonster() )
			{
				int dungeon_id = DungeonManager::Instance().GetDungeonID( x, y );

				if( dungeon_id )
				{
					pMob->SetDungeonRaidMonster();
					pMob->CalculateStat();
					DungeonManager::Instance().AddToDungeonRaidMonster( pMob, dungeon_id, pMonster->GetLayer() );
				}
			}

			if( pMonster->IsAttacking() )
			{
				// 몬스터가 스킬 시전할 때만 사용되고 지역 락 걸린 채로 들어오므로 락 없음
				// 리젠 위치가 스킬 시전 몬스터와 약간 차이가 있으므로 락이 일부 샐 수 있음
				// 락으로 인해 문제 생길 경우 생성 몹을 스킬 시전 몹과 같은 위치에 리젠시키고
				// 그 후에 Hate 추가하고, 그 다음에 목표 지점으로 이동하게 할 것
				pMob->AddHate( pMonster->GetEnemyHandle(), 1 );
			}
		}
	}

	return 0;
}

#ifndef _USE_NEW_ROAMING_ONLY
int SCRIPT_SetWayPointType( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 2 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetWayPointType() : invalid argument" );
		return 1;
	}

	GameContent::SetWayPointType( lua_tonumber( L, 1 ), lua_tonumber( L, 2 ) );

	return 0;
}

int SCRIPT_SetWayPointSpeed( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 2 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_SetWayPointSpeed() : invalid argument" );
		return 1;
	}

	GameContent::SetWayPointSpeed( lua_tonumber( L, 1 ), lua_tonumber( L, 2 ) );

	return 0;
}

int SCRIPT_AddWayPoint( struct lua_State *L )
{
	int n = lua_gettop( L );			// number of arguments

	if( n < 3 ||
		!lua_isnumber(L, 1) ||
		!lua_isnumber(L, 2) ||
		!lua_isnumber(L, 3) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_AddWayPoint() : invalid argument" );
		return 1;
	}

	GameContent::AddWayPoint( lua_tonumber( L, 1 ), lua_tonumber( L, 2 ), lua_tonumber( L, 3 ) );

	return 0;

}
#endif

int SCRIPT_FindNPC( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer *pPlayer = *pit;

	int nNPCID = lua_tonumber( L, 1 );

	if( !nNPCID )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nLayer = ( nArgCnt >= 2 && lua_isnumber( L, 2 ) ) ? lua_tonumber( L, 2 ) : ( ( pPlayer ) ? pPlayer->GetLayer() : 0 );

	StructNPC *pNPC = NULL;

	extern std::vector< StructNPC* > g_vNPC;
	for( std::vector< StructNPC * >::iterator it = g_vNPC.begin() ; it != g_vNPC.end() ; ++it )
	{
		if( (*it)->GetNPCID() == nNPCID && (*it)->GetLayer() == nLayer )
		{
			pNPC = (*it);
			break;
		}
	}

	if( !pNPC )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, pNPC->GetHandle() );
	return 1;
}

int SCRIPT_ForceMonsterProcDead( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, -1 );
		return 1;
	}

	AR_HANDLE hTarget = pPlayer->GetTarget();
	if( !hTarget )
	{
		lua_pushnumber( L, -2 );
		return 1;
	}

	StructCreature::iterator itTarget = StructCreature::get( hTarget );
	StructCreature * pTarget = (*itTarget);

	if( !pTarget || !pTarget->IsMonster() )
	{
		lua_pushnumber( L, -3 );
		return 1;
	}

	if( !pTarget->IsDead() )
	{
		lua_pushnumber( L, -4 );
		return 1;
	}

	// 타겟 주변에 락이 걸려있지 않을 경우에만 락을 걸도록 함
	// * pTarget의 위치와 완전히 일치하는 곳에 락이 걸려 있지 않거나, 락을 걸어둔 쓰레드가 현재 쓰레드가 아닐 경우 아래와 같은 문제가 발생할 수 있음
	//   1. 현재 쓰레드가 pTarget의 락 영역과 일부만 겹치게 락을 걸어둔 경우
	//      더블 락 체크에서 assert가 걸리고, 추가로 락을 걸게 되므로 이것이 좌표를 기준으로 서로 교차+동시발생하면 데드락이 됨.
	//   2. 현재 쓰레드가 완전히 떨어진 영역에 락을 걸어뒀고 pTarget이 있는 곳에는 다른 쓰레드가 일부 영역만 겹치게 락을 걸어둔 경우
	//      pTarget이 있는 곳에 락을 걸어둔 쓰레드에서 현재 쓰레드가 걸고 있는 완전히 떨어진 영역에 락을 걸려고 시도하면 데드락이 됨
	//   3. 다른 쓰레드가 pTarget이 있는 곳에 정확히 일치하게 락을 걸어둔 경우(현재 쓰레드가 완전히 떨어진 영역에 락을 걸어놨든 아니든 상관없음)
	//      락이 걸려있다고 판단하고 AddHP, damage를 실행하므로 중간에 어디서 락이 풀려버릴지 알 수 없으므로 Access violation이 발생할 수 있음
	ArcadiaLock _lock( 0 );
	if( !ArcadiaServer::Instance().IsLocked( pTarget ) )
	{
		_lock = ArcadiaServer::Instance().LockObjectWithVisibleRange( pTarget );
	}

	{
		ARCADIA_LOCK( _lock );

		pTarget->AddHP( 1 );
		pTarget->damage( pPlayer, 1, false );
	}

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_ShowSoulStoneCraftWindow( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	SendShowSoulStoneCraftWindow( pPlayer );

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_ShowSoulStoneRepairWindow( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer ) return 0;

	SendShowSoulStoneRepairWindow( pPlayer );

	lua_pushnumber( L, 1 );
	return 1;
}
