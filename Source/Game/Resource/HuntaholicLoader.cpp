
#include <atlcomtime.h>

#include <logging/FileLog.h>
#include <toolkit/XConsole.h>

#include "ContentLoader.h"
#include "HuntaholicManager.h"
#include "ChannelManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"


std::vector< GameContent::HUNTAHOLIC_BASE * > & GetHuntaholicVector()
{
	static std::vector< GameContent::HUNTAHOLIC_BASE * > vHuntaholicInfo;

	return vHuntaholicInfo;
}

struct dbHuntaholic : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbHuntaholic)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, id, sizeof( id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, name_id, sizeof( name_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, local_flag, sizeof( local_flag ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, hunting_period, sizeof( hunting_period ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, objective_point, sizeof( objective_point ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, max_point, sizeof( max_point ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, lobby_channel_id, sizeof( lobby_channel_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, lobby_x, sizeof( lobby_x ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, lobby_y, sizeof( lobby_y ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, dungeon_channel_id, sizeof( dungeon_channel_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 11, adInteger, dungeon_x, sizeof( dungeon_x ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 12, adInteger, dungeon_y, sizeof( dungeon_y ), FALSE )
END_ADO_BINDING()

	int id;
	int name_id;
	int local_flag;
	int hunting_period;
	int objective_point;
	int max_point;
	int lobby_channel_id;
	int lobby_x;
	int lobby_y;
	int dungeon_channel_id;
	int dungeon_x;
	int dungeon_y;
};

struct dbHuntaholicInstance : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbHuntaholicInstance)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, huntaholic_id, sizeof( huntaholic_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, instance_type_id, sizeof( instance_type_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, min_level, sizeof( min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, max_level, sizeof( max_level ), FALSE )
	ADO_NUMERIC_ENTRY2( 5, adDecimal, point_advantage, 10, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adBigInt, reward_exp, sizeof( reward_exp ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, reward_jp, sizeof( reward_jp ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, reward_success_item_id, sizeof( reward_success_item_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adBigInt, reward_success_item_count, sizeof( reward_success_item_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, reward_fail_item_id, sizeof( reward_fail_item_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 11, adBigInt, reward_fail_item_count, sizeof( reward_fail_item_count ), FALSE )
END_ADO_BINDING()

	int huntaholic_id;
	int instance_type_id;
	int min_level;
	int max_level;
	_decimal_variant point_advantage;
	__int64 reward_exp;
	int reward_jp;
	int reward_success_item_id;
	__int64 reward_success_item_count;
	int reward_fail_item_id;
	__int64 reward_fail_item_count;
};

struct dbHuntaholicMonsterRespawn : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbHuntaholicMonsterRespawn)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, huntaholic_id, sizeof( huntaholic_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, instance_type_id, sizeof( instance_type_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, respawn_id, sizeof( respawn_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, left, sizeof( left ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, top, sizeof( top ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, right, sizeof( right ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, bottom, sizeof( bottom ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, monster_id, sizeof( monster_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, count, sizeof( count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, period, sizeof( period ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 11, adBoolean, is_wandering, sizeof( is_wandering ), FALSE )
END_ADO_BINDING()

	int huntaholic_id;
	int instance_type_id;
	int respawn_id;
	int left;
	int top;
	int right;
	int bottom;
	int monster_id;
	int count;
	int period;
	bool is_wandering;
};

struct dbHuntaholicHealingProp : public CADORecordBinding
{
BEGIN_ADO_BINDING( dbHuntaholicHealingProp )

	ADO_VARIABLE_LENGTH_ENTRY4(	1, adInteger, huntaholic_id, 4, FALSE )
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

	int					huntaholic_id;
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

void onHuntaholicInfo( dbHuntaholic * emprs )
{
	extern volatile int g_nCurrentLocalFlag;
	if( emprs->local_flag & g_nCurrentLocalFlag )
		return;

	GameContent::HUNTAHOLIC_BASE *pInfo = new GameContent::HUNTAHOLIC_BASE();

	pInfo->nID					= emprs->id;
	pInfo->nNameID				= emprs->name_id;
	pInfo->tHuntingPeriod		= emprs->hunting_period;
	pInfo->nObjectivePoint		= emprs->objective_point;
	pInfo->nMaxPoint			= emprs->max_point;

	X2D::Box< AR_UNIT > bxArea;
	ChannelManager::GetRegionOfChannel( emprs->lobby_channel_id, bxArea );
	pInfo->nLobbyRegionLeft		= GetRegionX( bxArea.GetLeft() );
	pInfo->nLobbyRegionTop		= GetRegionY( bxArea.GetTop() );
	pInfo->nLobbyRegionRight	= GetRegionX( bxArea.GetRight() ) + 1;
	pInfo->nLobbyRegionBottom	= GetRegionY( bxArea.GetBottom() ) + 1;
	pInfo->posLobby.x			= emprs->lobby_x;
	pInfo->posLobby.y			= emprs->lobby_y;

	ChannelManager::GetRegionOfChannel( emprs->dungeon_channel_id, bxArea );
	pInfo->nDungeonRegionLeft	= GetRegionX( bxArea.GetLeft() );
	pInfo->nDungeonRegionTop	= GetRegionY( bxArea.GetTop() );
	pInfo->nDungeonRegionRight	= GetRegionX( bxArea.GetRight() ) + 1;
	pInfo->nDungeonRegionBottom	= GetRegionY( bxArea.GetBottom() ) + 1;
	pInfo->posDungeon.x			= emprs->dungeon_x;
	pInfo->posDungeon.y			= emprs->dungeon_y;

#ifdef _DEBUG
	// 디버그 모드일 때만 이미 로드된 헌터홀릭 정보인지 확인
	for( std::vector< GameContent::HUNTAHOLIC_BASE * >::const_iterator it = GetHuntaholicVector().begin() ; it != GetHuntaholicVector().end() ; ++it )
	{
		if( (*it)->nID == pInfo->nID )
		{
			assert( 0 );
			delete pInfo;
		}
	}
#endif

	GetHuntaholicVector().push_back( pInfo );
}

void onHuntaholicInstanceInfo( dbHuntaholicInstance * emprs )
{
	GameContent::HUNTAHOLIC_INSTANCE_BASE *pInfo = new GameContent::HUNTAHOLIC_INSTANCE_BASE();

	pInfo->nID = emprs->instance_type_id;
	pInfo->nMinLevel = emprs->min_level;
	pInfo->nMaxLevel = emprs->max_level;
	pInfo->fPointAdvantage.set( emprs->point_advantage.getMultipleInteger( 10000 ) );
	pInfo->nRewardExp = emprs->reward_exp;
	pInfo->nRewardJp = emprs->reward_jp;
	pInfo->nRewardSuccessItemCode = emprs->reward_success_item_id;
	pInfo->nRewardSuccessItemCount = emprs->reward_success_item_count;
	pInfo->nRewardFailItemCode = emprs->reward_fail_item_id;
	pInfo->nRewardFailItemCount = emprs->reward_fail_item_count;

	for( std::vector< GameContent::HUNTAHOLIC_BASE * >::iterator it = GetHuntaholicVector().begin() ; it != GetHuntaholicVector().end() ; ++it )
	{
		if( (*it)->nID != emprs->huntaholic_id )
			continue;

#ifdef _DEBUG
		for( std::vector< const GameContent::HUNTAHOLIC_INSTANCE_BASE * >::const_iterator itInstance = (*it)->vInstanceBase.begin() ; itInstance != (*it)->vInstanceBase.end() ; ++itInstance )
		{
			if( (*itInstance)->nID == pInfo->nID )
			{
				assert( 0 );
				delete pInfo;
				return;
			}
		}
#endif

		(*it)->vInstanceBase.push_back( pInfo );
		return;
	}

	assert( 0 );
	delete pInfo;
}

void onHuntaholicMonsterRespawnInfo( dbHuntaholicMonsterRespawn * emprs )
{
	GameContent::HUNTAHOLIC_MONSTER_RESPAWN_INFO * pInfo = new GameContent::HUNTAHOLIC_MONSTER_RESPAWN_INFO();

	pInfo->nID = emprs->respawn_id;
	pInfo->bxArea.Set( emprs->left, emprs->top, emprs->right, emprs->bottom );
	pInfo->nMonsterID = emprs->monster_id;
	pInfo->nCount = emprs->count;
	pInfo->nPeriod = emprs->period * 100;
	pInfo->bWandering = emprs->is_wandering;

	for( std::vector< GameContent::HUNTAHOLIC_BASE * >::iterator it = GetHuntaholicVector().begin() ; it != GetHuntaholicVector().end() ; ++it )
	{
		if( (*it)->nID != emprs->huntaholic_id )
			continue;

		for( std::vector< const GameContent::HUNTAHOLIC_INSTANCE_BASE * >::const_iterator itInstance = (*it)->vInstanceBase.begin() ; itInstance != (*it)->vInstanceBase.end() ; ++itInstance )
		{
			if( (*itInstance)->nID != emprs->instance_type_id )
				continue;

#ifdef _DEBUG
			for( std::vector< const GameContent::HUNTAHOLIC_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = (*itInstance)->vRespawnInfo.begin() ; itRespawn != (*itInstance)->vRespawnInfo.end() ; ++itRespawn )
			{
				if( (*itRespawn)->nID == pInfo->nID )
				{
					assert( 0 );
					delete pInfo;
					return;
				}
			}
#endif

			const_cast< GameContent::HUNTAHOLIC_INSTANCE_BASE * >( (*itInstance) )->vRespawnInfo.push_back( pInfo );
			return;
		}

		break;
	}

	assert( 0 );
	delete pInfo;
}

void onHuntaholicHealingPropInfo( dbHuntaholicHealingProp * emprs )
{
	extern volatile int g_nCurrentLocalFlag;
	if( g_nCurrentLocalFlag & emprs->local_flag )
		return;

	for( std::vector< GameContent::HUNTAHOLIC_BASE * >::iterator it = GetHuntaholicVector().begin() ; it != GetHuntaholicVector().end() ; ++it )
	{
		if( (*it)->nID != emprs->huntaholic_id )
			continue;

		for( std::vector< const GameContent::HUNTAHOLIC_INSTANCE_BASE * >::const_iterator itInstance = (*it)->vInstanceBase.begin() ; itInstance != (*it)->vInstanceBase.end() ; ++itInstance )
		{
			if( (*itInstance)->nID != emprs->instance_type_id )
				continue;

			GameContent::FIELD_PROP_RESPAWN_INFO * pInfo = new GameContent::FIELD_PROP_RESPAWN_INFO( emprs->prop_id, emprs->x, emprs->y, 0,
				emprs->dv_offset_z.getFloat(), emprs->dv_around_x.getFloat(), emprs->dv_around_y.getFloat(), emprs->dv_around_z.getFloat(),
				emprs->dv_scale_x.getFloat(), emprs->dv_scale_y.getFloat(), emprs->dv_scale_z.getFloat(), emprs->is_height_locked, emprs->dv_lock_height.getFloat() );

			const_cast< GameContent::HUNTAHOLIC_INSTANCE_BASE * >( (*itInstance) )->vHealingPropInfo.push_back( pInfo );
			return;
		}

		break;
	}

	assert( 0 );
}

bool HuntaholicLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		_ConnectionPtr ContentDBConnPtr = NULL;
		InitContentDbConnection( ContentDBConnPtr );
		_ConnectionPtr UserDBConnPtr = NULL;
		InitUserDbConnection( UserDBConnPtr );

		helper.start();
		size_t nLoadedHuntaholic = LoadDbResource< dbHuntaholic >( "HuntaholicResource", ContentDBConnPtr, onHuntaholicInfo );
		size_t nLoadedInstance = LoadDbResource< dbHuntaholicInstance >( "HuntaholicInstanceResource", ContentDBConnPtr, onHuntaholicInstanceInfo );
		size_t nLoadedMonsterRespawn = LoadDbResource< dbHuntaholicMonsterRespawn >( "HuntaholicMonsterRespawnResource", ContentDBConnPtr, onHuntaholicMonsterRespawnInfo );
		size_t nLoadedHealingProp = LoadDbResource< dbHuntaholicHealingProp >( "HuntaholicHealingPropResource", ContentDBConnPtr, onHuntaholicHealingPropInfo );
		helper.end( "HuntaholicLoader" );

		for( std::vector< GameContent::HUNTAHOLIC_BASE * >::const_iterator it = GetHuntaholicVector().begin() ; it != GetHuntaholicVector().end() ; ++it )
		{
			if( !HuntaholicManager::Instance().RegisterHuntaholicBase( (*it) ) )
			{
				assert( 0 );

				--nLoadedHuntaholic;

				nLoadedInstance -= (*it)->vInstanceBase.size();
				for( std::vector< const GameContent::HUNTAHOLIC_INSTANCE_BASE * >::const_iterator itInstance = (*it)->vInstanceBase.begin() ; itInstance != (*it)->vInstanceBase.end() ; ++itInstance )
				{
					nLoadedMonsterRespawn -= (*itInstance)->vRespawnInfo.size();

					for( std::vector< const GameContent::HUNTAHOLIC_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = (*itInstance)->vRespawnInfo.begin() ; itRespawn != (*itInstance)->vRespawnInfo.end() ; ++itRespawn )
					{
						delete (*itRespawn);
					}

					delete (*itInstance);
				}

				delete (*it);

				continue;
			}
		}

		GetHuntaholicVector().clear();

		_cprint( "Total %d Huntaholic info loaded...\n", nLoadedHuntaholic );
		FILELOG( "Total %d Huntaholic info loaded...", nLoadedHuntaholic );
		_cprint( "Total %d Huntaholic Instance info loaded...\n", nLoadedInstance );
		FILELOG( "Total %d Huntaholic Instance info loaded...", nLoadedInstance );
		_cprint( "Total %d Huntaholic Monster Respawn info loaded...\n", nLoadedMonsterRespawn );
		FILELOG( "Total %d Huntaholic Monster Respawn info loaded...", nLoadedMonsterRespawn );
		_cprint( "Total %d Huntaholic Healing Prop info loaded...\n", nLoadedHealingProp );
		FILELOG( "Total %d Huntaholic Healing Prop info loaded...", nLoadedHealingProp );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "HuntaholicLoader" );
		LogDBError( e, "HuntaholicLoader", "HuntaholicLoader::onProcess" );

		std::string strError = "HUNTAHOLIC RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}
