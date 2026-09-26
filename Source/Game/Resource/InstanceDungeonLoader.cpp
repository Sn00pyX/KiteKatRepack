
#include <atlcomtime.h>

#include <logging/FileLog.h>
#include <toolkit/XConsole.h>

#include "ContentLoader.h"
#include "InstanceDungeonManager.h"
#include "ChannelManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"


std::vector< GameContent::INSTANCE_DUNGEON_BASE * > & GetInstanceDungeonVector()
{
	static std::vector< GameContent::INSTANCE_DUNGEON_BASE * > vInstanceDungeonInfo;

	return vInstanceDungeonInfo;
}

struct dbInstanceDungeon : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbInstanceDungeon)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, id, sizeof( id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, name_id, sizeof( name_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, local_flag, sizeof( local_flag ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, dungeon_channel_id, sizeof( dungeon_channel_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, dungeon_x, sizeof( dungeon_x ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, dungeon_y, sizeof( dungeon_y ), FALSE )
END_ADO_BINDING()

	int id;
	int name_id;
	int local_flag;
	int dungeon_channel_id;
	int dungeon_x;
	int dungeon_y;
};

struct dbInstanceDungeonType : public CADORecordBinding, GameContent::INSTANCE_DUNGEON_TYPE_BASE
{
BEGIN_ADO_BINDING(dbInstanceDungeonType)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, instance_dungeon_id, sizeof( instance_dungeon_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, nID, sizeof( nID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, nMinLevel, sizeof( nMinLevel ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, nMaxLevel, sizeof( nMaxLevel ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, nNeedItemCode, sizeof( nNeedItemCode ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adBigInt, nNeedItemCount, sizeof( nNeedItemCount ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adVarChar, szScriptOnCreate, _countof( szScriptOnCreate ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adVarChar, szScriptOnJoin, _countof( szScriptOnJoin ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adVarChar, szScriptOnLeave, _countof( szScriptOnLeave ), FALSE )
END_ADO_BINDING()

	int instance_dungeon_id;
};

struct dbInstanceDungeonMonsterRespawn : public CADORecordBinding, GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO
{
BEGIN_ADO_BINDING(dbInstanceDungeonMonsterRespawn)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, instance_dungeon_id, sizeof( instance_dungeon_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, instance_type_id, sizeof( instance_type_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, nID, sizeof( nID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, left, sizeof( left ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, top, sizeof( top ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, right, sizeof( right ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, bottom, sizeof( bottom ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, nMonsterID, sizeof( nMonsterID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, nCount, sizeof( nCount ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, nPeriod, sizeof( nPeriod ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 11, adBoolean, bWandering, sizeof( bWandering ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 12, adInteger, nRespawnGroup, sizeof( nRespawnGroup ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 13, adBoolean, bRespawnControl, sizeof( bRespawnControl), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 14, adVarChar, szScriptOnDead, _countof( szScriptOnDead ), FALSE )
END_ADO_BINDING()

	int instance_dungeon_id;
	int instance_type_id;
	int left;
	int top;
	int right;
	int bottom;
};

struct dbInstanceDungeonHealingProp : public CADORecordBinding
{
BEGIN_ADO_BINDING( dbInstanceDungeonHealingProp )

	ADO_VARIABLE_LENGTH_ENTRY4(	1, adInteger, instance_dungeon_id, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	2, adInteger, instance_type_id, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	3, adInteger, id, 4, FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	4, adInteger, local_flag, 4, FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	5, adInteger, prop_id, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	6, adInteger, x, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	7, adInteger, y, 4, FALSE )

	ADO_NUMERIC_ENTRY2(			10, adDecimal, dv_offset_z, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(			11, adDecimal, dv_around_x, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(			12, adDecimal, dv_around_y, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(			13, adDecimal, dv_around_z, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(			14, adDecimal, dv_scale_x, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(			15, adDecimal, dv_scale_y, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(			16, adDecimal, dv_scale_z, 10, 8, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(	17, adBoolean, is_height_locked, sizeof( is_height_locked ), FALSE)
	ADO_NUMERIC_ENTRY2(			18, adDecimal, dv_lock_height, 10, 8, FALSE)

END_ADO_BINDING()

	int					instance_dungeon_id;
	int					instance_type_id;
	int					id;

	int					local_flag;

	int					prop_id;
	int					x;
	int					y;

	_decimal_variant	dv_offset_z;
	_decimal_variant	dv_around_x;
	_decimal_variant	dv_around_y;
	_decimal_variant	dv_around_z;
	_decimal_variant	dv_scale_x;
	_decimal_variant	dv_scale_y;
	_decimal_variant	dv_scale_z;
	bool				is_height_locked;
	_decimal_variant	dv_lock_height;
};

void onInstanceDungeonInfo( dbInstanceDungeon * emprs )
{
	extern volatile int g_nCurrentLocalFlag;
	if( emprs->local_flag & g_nCurrentLocalFlag )
		return;

	GameContent::INSTANCE_DUNGEON_BASE *pInfo = new GameContent::INSTANCE_DUNGEON_BASE();

	pInfo->nID					= emprs->id;
	pInfo->nNameID				= emprs->name_id;
	
	X2D::Box< AR_UNIT > bxArea;
	ChannelManager::GetRegionOfChannel( emprs->dungeon_channel_id, bxArea );
	pInfo->nDungeonRegionLeft	= GetRegionX( bxArea.GetLeft() );
	pInfo->nDungeonRegionTop	= GetRegionY( bxArea.GetTop() );
	pInfo->nDungeonRegionRight	= GetRegionX( bxArea.GetRight() ) + 1;
	pInfo->nDungeonRegionBottom	= GetRegionY( bxArea.GetBottom() ) + 1;
	pInfo->posDungeon.x			= emprs->dungeon_x;
	pInfo->posDungeon.y			= emprs->dungeon_y;

#ifdef _DEBUG
	// 디버그 모드일 때만 이미 로드된 인스턴스 던전 정보인지 확인
	for( std::vector< GameContent::INSTANCE_DUNGEON_BASE * >::const_iterator it = GetInstanceDungeonVector().begin() ; it != GetInstanceDungeonVector().end() ; ++it )
	{
		if( (*it)->nID == pInfo->nID )
		{
			assert( 0 );
			delete pInfo;
		}
	}
#endif

	GetInstanceDungeonVector().push_back( pInfo );
}

void onInstanceDungeonTypeInfo( dbInstanceDungeonType * emprs )
{
	// 실행할 내용이 없는 트리거는 길이를 0으로 만들어 줌
	if( !strcmp( emprs->szScriptOnCreate, "0" ) || !strcmp( emprs->szScriptOnCreate, " " ) )
		emprs->szScriptOnCreate[ 0 ] = NULL;
	if( !strcmp( emprs->szScriptOnJoin, "0" ) || !strcmp( emprs->szScriptOnJoin, " " ) )
		emprs->szScriptOnJoin[ 0 ] = NULL;
	if( !strcmp( emprs->szScriptOnLeave, "0" ) || !strcmp( emprs->szScriptOnLeave, " " ) )
		emprs->szScriptOnLeave[ 0 ] = NULL;

	GameContent::INSTANCE_DUNGEON_TYPE_BASE *pInfo = new GameContent::INSTANCE_DUNGEON_TYPE_BASE( *emprs );

	for( std::vector< GameContent::INSTANCE_DUNGEON_BASE * >::iterator it = GetInstanceDungeonVector().begin() ; it != GetInstanceDungeonVector().end() ; ++it )
	{
		if( (*it)->nID != emprs->instance_dungeon_id )
			continue;

#ifdef _DEBUG
		for( std::vector< const GameContent::INSTANCE_DUNGEON_TYPE_BASE * >::const_iterator itInstance = (*it)->vInstanceDungeonTypeBase.begin() ; itInstance != (*it)->vInstanceDungeonTypeBase.end() ; ++itInstance )
		{
			if( (*itInstance)->nID == pInfo->nID )
			{
				assert( 0 );
				delete pInfo;
				return;
			}
		}
#endif

		(*it)->vInstanceDungeonTypeBase.push_back( pInfo );
		return;
	}

	assert( 0 );
	delete pInfo;
}

void onInstanceDungeonMonsterRespawnInfo( dbInstanceDungeonMonsterRespawn * emprs )
{
	// 리스폰 주기는 DB에선 초 단위로 입력되어 있으므로 AR_TIME 단위로 변환해야 함
	emprs->nPeriod *= 100;

	emprs->bxArea.Set( emprs->left, emprs->top, emprs->right, emprs->bottom );

	// 실행할 내용이 없는 트리거는 길이를 0으로 만들어 줌
	if( !strcmp( emprs->szScriptOnDead, "0" ) || !strcmp( emprs->szScriptOnDead, " " ) )
		emprs->szScriptOnDead[ 0 ] = NULL;

	GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * pInfo = new GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO( *emprs );

	for( std::vector< GameContent::INSTANCE_DUNGEON_BASE * >::iterator it = GetInstanceDungeonVector().begin() ; it != GetInstanceDungeonVector().end() ; ++it )
	{
		if( (*it)->nID != emprs->instance_dungeon_id )
			continue;

		for( std::vector< const GameContent::INSTANCE_DUNGEON_TYPE_BASE * >::const_iterator itInstance = (*it)->vInstanceDungeonTypeBase.begin() ; itInstance != (*it)->vInstanceDungeonTypeBase.end() ; ++itInstance )
		{
			if( (*itInstance)->nID != emprs->instance_type_id )
				continue;

#ifdef _DEBUG
			for( std::vector< const GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = (*itInstance)->vRespawnInfo.begin() ; itRespawn != (*itInstance)->vRespawnInfo.end() ; ++itRespawn )
			{
				if( (*itRespawn)->nID == pInfo->nID )
				{
					assert( 0 );
					delete pInfo;
					return;
				}
			}
#endif

			const_cast< GameContent::INSTANCE_DUNGEON_TYPE_BASE * >( (*itInstance) )->vRespawnInfo.push_back( pInfo );
			return;
		}

		break;
	}

	assert( 0 );
	delete pInfo;
}

void onInstanceDungeonHealingPropInfo( dbInstanceDungeonHealingProp * emprs )
{
	extern volatile int g_nCurrentLocalFlag;
	if( g_nCurrentLocalFlag & emprs->local_flag )
		return;

	for( std::vector< GameContent::INSTANCE_DUNGEON_BASE * >::iterator it = GetInstanceDungeonVector().begin() ; it != GetInstanceDungeonVector().end() ; ++it )
	{
		if( (*it)->nID != emprs->instance_dungeon_id )
			continue;

		for( std::vector< const GameContent::INSTANCE_DUNGEON_TYPE_BASE * >::const_iterator itInstance = (*it)->vInstanceDungeonTypeBase.begin() ; itInstance != (*it)->vInstanceDungeonTypeBase.end() ; ++itInstance )
		{
			if( (*itInstance)->nID != emprs->instance_type_id )
				continue;

			GameContent::FIELD_PROP_RESPAWN_INFO * pInfo = new GameContent::FIELD_PROP_RESPAWN_INFO( emprs->prop_id, emprs->x, emprs->y, 0,
				emprs->dv_offset_z.getFloat(), emprs->dv_around_x.getFloat(), emprs->dv_around_y.getFloat(), emprs->dv_around_z.getFloat(),
				emprs->dv_scale_x.getFloat(), emprs->dv_scale_y.getFloat(), emprs->dv_scale_z.getFloat(), emprs->is_height_locked, emprs->dv_lock_height.getFloat() );

			const_cast< GameContent::INSTANCE_DUNGEON_TYPE_BASE * >( (*itInstance) )->vHealingPropInfo.push_back( pInfo );
			return;
		}

		break;
	}

	assert( 0 );
}

bool InstanceDungeonLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		_ConnectionPtr ContentDBConnPtr = NULL;
		InitContentDbConnection( ContentDBConnPtr );
		_ConnectionPtr UserDBConnPtr = NULL;
		InitUserDbConnection( UserDBConnPtr );

		helper.start();
		size_t nLoadedInstanceDungeon = LoadDbResource< dbInstanceDungeon >( "InstanceDungeonResource", ContentDBConnPtr, onInstanceDungeonInfo );
		size_t nLoadedInstanceDungeonType = LoadDbResource< dbInstanceDungeonType >( "InstanceDungeonTypeResource", ContentDBConnPtr, onInstanceDungeonTypeInfo );
		size_t nLoadedMonsterRespawn = LoadDbResource< dbInstanceDungeonMonsterRespawn >( "InstanceDungeonMonsterRespawnResource", ContentDBConnPtr, onInstanceDungeonMonsterRespawnInfo );
		size_t nLoadedHealingProp = LoadDbResource< dbInstanceDungeonHealingProp >( "InstanceDungeonHealingPropResource", ContentDBConnPtr, onInstanceDungeonHealingPropInfo );
		helper.end( "InstanceDungeonLoader" );

		for( std::vector< GameContent::INSTANCE_DUNGEON_BASE * >::const_iterator it = GetInstanceDungeonVector().begin() ; it != GetInstanceDungeonVector().end() ; ++it )
		{
			if( !InstanceDungeonManager::Instance().RegisterInstanceDungeonBase( (*it) ) )
			{
				assert( 0 );

				--nLoadedInstanceDungeon;

				nLoadedInstanceDungeon -= (*it)->vInstanceDungeonTypeBase.size();
				for( std::vector< const GameContent::INSTANCE_DUNGEON_TYPE_BASE * >::const_iterator itInstance = (*it)->vInstanceDungeonTypeBase.begin() ; itInstance != (*it)->vInstanceDungeonTypeBase.end() ; ++itInstance )
				{
					nLoadedMonsterRespawn -= (*itInstance)->vRespawnInfo.size();

					for( std::vector< const GameContent::INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = (*itInstance)->vRespawnInfo.begin() ; itRespawn != (*itInstance)->vRespawnInfo.end() ; ++itRespawn )
					{
						delete (*itRespawn);
					}

					delete (*itInstance);
				}

				delete (*it);

				continue;
			}
		}

		GetInstanceDungeonVector().clear();

		_cprint( "Total %d Instance Dungeon info loaded...\n", nLoadedInstanceDungeon );
		FILELOG( "Total %d Instance Dungeon info loaded...", nLoadedInstanceDungeon );
		_cprint( "Total %d Instance Dungeon Type loaded...\n", nLoadedInstanceDungeonType );
		FILELOG( "Total %d Instance Dungeon Type info loaded...", nLoadedInstanceDungeonType );
		_cprint( "Total %d Instance Dungeon Monster Respawn info loaded...\n", nLoadedMonsterRespawn );
		FILELOG( "Total %d Instance Dungeon Monster Respawn info loaded...", nLoadedMonsterRespawn );
		_cprint( "Total %d Instance Dungeon Healing Prop info loaded...\n", nLoadedHealingProp );
		FILELOG( "Total %d Instance Dungeon Healing Prop info loaded...", nLoadedHealingProp );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "InstanceDungeonLoader" );
		LogDBError( e, "InstanceDungeonLoader", "InstanceDungeonLoader::onProcess()" );

		std::string strError = "INSTANCE DUNGEON RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}