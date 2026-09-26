
#include <ctime>
#include <set>
#include <queue>

#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>
#include <mmo/ArRegion.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/XSTLUtil.h>
#include <toolkit/XConsole.h>

#include "LogClient/LogClient.h"

#include "DungeonManager.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "ChannelManager.h"
#include "GameProc.h"
#include "NPCProc.h"
#include "GuildManager.h"
#include "PartyManager.h"
#include "RoamingManager.h"
#include "DB_Commands.h"
#include "FieldPropManager.h"
#include "TimeUtil.h"


static const int	DUNGEON_SIEGE_NOTICE_START_TIME		= 60 * 60 + 30;	// 60분 30초 전부터 공지 시작 (오차 생겨도 60분부터 공지 나가도록)
static const int	DUNGEON_SIEGE_NOTICE_TIME[]			= { 60 * 60 + 30, 60 * 30 + 30, 60 * 10 + 30 }; // 던전 시즈 공지 시간 (60분, 30분, 10분에 공지)
static const int	DUNGEON_SIEGE_GUILD_NOTICE_TIME		= 60 * 5 + 30;	// 5분 30초 전에 양쪽 길드에 5분후 입장 가능 공지
static const int	DUNGEON_SIEGE_KICK_TIME				= 60;		// 1분 후에 쫓겨남
static const int	DUNGEON_RAID_TIME_OUT				= 3600 * 4;	// 4시간 후에 종료


static const int	DUNGEON_RAID_NOTICE_PERIOD			= 60 * 5;

static const int	WEEK_TIME_IN_SECOND					= 3600 * 24 * 7;

static const int	GUARDIAN_RANGE						= 60 * GameRule::DEFAULT_UNIT_SIZE;

static const int	DUNGEON_BOSS_COUNT					= 2;		// 던전당 보스는 두 마리~


struct _DUNGEON_RAID_INFO : StructMonster::MonsterDeleteHandler
{

	_DUNGEON_RAID_INFO( int _guild_id, int _raid_record, time_t raid_end_time, struct _DUNGEON_INFO * _pDungeonInfo ) : raid_starting( false ), raid_complete( false ), guild_id ( _guild_id ), start_time( 0 ), end_time( raid_end_time ), last_notice_time( 0 ), raid_record( _raid_record ), layer( 0 ), pDungeonInfo( _pDungeonInfo )
	{
		for( int i = 0; i < DUNGEON_BOSS_COUNT; ++i )
		{
			boss_dead[i] = false;
		}
	}
	int						guild_id;
	AR_TIME					start_time;
	time_t					end_time;
	time_t					last_notice_time;
	AR_TIME					raid_record;
	unsigned char			layer;
	bool					boss_dead[DUNGEON_BOSS_COUNT];
	bool					raid_starting;
	bool					raid_complete;

	std::vector< struct StructMonster * >	vMonsters;
	struct _DUNGEON_INFO *	pDungeonInfo;

protected:
	virtual void onMonsterDelete( struct StructMonster * pMonster );
};

struct _TACTICAL_POSITION_INFO
{
	_TACTICAL_POSITION_INFO()
	{
		id = 0;
		own_by_original_owner = true;
		prop_id = 0;
	}

	int id;
	ArPosition pos;
	bool own_by_original_owner;
	int prop_id;
};

typedef std::pair< int /* RoamingID */, GameContent::ROAMING_CREATURE_RESPAWN_INFO > ROAMING_MONSTER_RESPAWN_INFO;

struct _DUNGEON_INFO : StructMonster::MonsterDeleteHandler
{
	_DUNGEON_INFO()
	{
		id = 0;
		type = DungeonManager::DUNGEON_TYPE_UNKNOWN;
		level = 0;
		core_id = 0;
		start_time = 0;
		end_time = 0;
		raid_start_time = 0;
		raid_end_time = 0;
		core_offset_z = 0.0f;
		core_around_x = 0.0f;
		core_around_y = 0.0f;
		core_around_z = 0.0f;
		core_scale_x = 0.0f;
		core_scale_y = 0.0f;
		core_scale_z = 0.0f;
		core_is_lock_height = false;
		core_lock_height = 0.0f;
		max_guild_party = 0;
		max_raid_party = 0;
		connector_id = 0;
		prev_owner_guild_id = 0;
		owner_guild_id = 0;
		raid_guild_id = 0;
		best_raid_time = 0;
		original_owner_guild_id = 0;
		last_dungeon_siege_finish_time = 0;
		last_dungeon_raid_wrap_up_time = 0;
		last_global_notice_count = 0;
		pDungeonCore = NULL;
		pConnector = NULL;
		bDungeonSiege = false;
		bDungeonSiegeCreated = false;
		bDungeonSiegeKicked = true;
		bDungeonSiegeNeedToDestroy = false;
		bNeedToChangePosition = false;
		bDungeonOwnerChanged = false;
		tax_rate = 0;
	}

	// m_CS를 제외한 복사 생성자(XCriticalSection의 복사 생성자를 정의하면 안 이래도 되긴 한다 -_ -)
	_DUNGEON_INFO( const _DUNGEON_INFO & rhs )
		: id( rhs.id )
		, level( rhs.level )
		, type( rhs.type )
		, raid_start_pos( rhs.raid_start_pos )
		, siege_start_pos( rhs.siege_start_pos )
		, siege_defence_pos( rhs.siege_defence_pos )
		, core_pos( rhs.core_pos )
		, connector_pos( rhs.connector_pos )
		, core_id( rhs.core_id )
		, start_time( rhs.start_time )
		, end_time( rhs.end_time )
		, raid_start_time( rhs.raid_start_time )
		, raid_end_time( rhs.raid_end_time )
		, core_offset_z( rhs.core_offset_z )
		, core_around_x( rhs.core_around_x )
		, core_around_y( rhs.core_around_y )
		, core_around_z( rhs.core_around_z )
		, core_scale_x( rhs.core_scale_x )
		, core_scale_y( rhs.core_scale_y )
		, core_scale_z( rhs.core_scale_z )
		, core_is_lock_height( rhs.core_is_lock_height )
		, core_lock_height( rhs.core_lock_height )
		, box( rhs.box )
		, connector_id( rhs.connector_id )
		, prev_owner_guild_id( rhs.prev_owner_guild_id )
		, owner_guild_id( rhs.owner_guild_id )
		, raid_guild_id( rhs.raid_guild_id )
		, best_raid_time( rhs.best_raid_time )
		, original_owner_guild_id( rhs.original_owner_guild_id )
		, last_global_notice_count( rhs.last_global_notice_count )
		, last_dungeon_siege_finish_time( rhs.last_dungeon_siege_finish_time )
		, last_dungeon_raid_wrap_up_time( rhs.last_dungeon_raid_wrap_up_time )
		, bDungeonSiege( rhs.bDungeonSiege )
		, bDungeonSiegeCreated( rhs.bDungeonSiegeCreated )
		, bDungeonSiegeKicked( rhs.bDungeonSiegeKicked )
		, bDungeonSiegeNeedToDestroy( rhs.bDungeonSiegeNeedToDestroy )
		, bNeedToChangePosition( rhs.bNeedToChangePosition )
		, bDungeonOwnerChanged( rhs.bDungeonOwnerChanged )
		, max_guild_party( rhs.max_guild_party )
		, max_raid_party( rhs.max_raid_party )
		, tax_rate( rhs.tax_rate )
		, pDungeonCore( rhs.pDungeonCore )
#ifndef _DUNGEON_CORE_AS_MONSTER
		, stCoreGuardianHandle( rhs.stCoreGuardianHandle )
#endif
		, pConnector( rhs.pConnector )
		, vGuardianRespawnInfo( rhs.vGuardianRespawnInfo )
		, vEnvironmentalGuardianRespawnInfo( rhs.vEnvironmentalGuardianRespawnInfo )
		, vRespawnedEnvironmentalGuardianInfo( rhs.vRespawnedEnvironmentalGuardianInfo )
		, qPendedGuardianRespawn( rhs.qPendedGuardianRespawn )
		, vRespawnInfo( rhs.vRespawnInfo )
		, vRoamerID( rhs.vRoamerID )
		, vRaidInfo( rhs.vRaidInfo )
		, vMonsters( rhs.vMonsters )
		, vRandomRespawnInfo( rhs.vRandomRespawnInfo )
		, vTacticalPositionInfo( rhs.vTacticalPositionInfo )
		, vNotices( rhs.vNotices )
	{
		s_memcpy( boss_id, sizeof( boss_id ), rhs.boss_id, sizeof( rhs.boss_id ) );
	}

	void procRespawnEnvironmentalGuardian();

	int						id;
	int						level;
	int						type;
	ArPosition				raid_start_pos;
	ArPosition				siege_start_pos;
	ArPosition				siege_defence_pos;
	ArPosition				core_pos;
	ArPosition				connector_pos;
	int						core_id;
	int						start_time;
	int						end_time;
	int						raid_start_time;
	int						raid_end_time;
	float					core_offset_z;
	float					core_around_x;
	float					core_around_y;
	float					core_around_z;
	float					core_scale_x;
	float					core_scale_y;
	float					core_scale_z;
	bool					core_is_lock_height;
	float					core_lock_height;
	int						boss_id[DUNGEON_BOSS_COUNT];
	X2D::Box< AR_UNIT >		box;
	int						connector_id;
	int						prev_owner_guild_id;
	int						owner_guild_id;
	int						raid_guild_id;
	AR_TIME					best_raid_time;
	int						original_owner_guild_id;
	int						last_global_notice_count;
	time_t					last_dungeon_siege_finish_time;
	time_t					last_dungeon_raid_wrap_up_time;
	bool					bDungeonSiege;
	bool					bDungeonSiegeCreated;
	bool					bDungeonSiegeKicked;
	bool					bDungeonSiegeNeedToDestroy;
	bool					bNeedToChangePosition;
	bool					bDungeonOwnerChanged;
	int						max_guild_party;
	int						max_raid_party;
	int						tax_rate;

#ifndef _DUNGEON_CORE_AS_MONSTER
	StructFieldProp *		pDungeonCore;
#else
	StructMonster *			pDungeonCore;
	std::set< AR_HANDLE >	stCoreGuardianHandle;
#endif
	StructMonster *			pConnector;

	// 지역 락 -> _DUNGEON_INFO::m_CS -> GuildManager::m_IntfLock 순서여야 함.
	mutable XCriticalSection		m_CS;

	struct RESPAWNED_ENVIRONMENTAL_GUARDIAN_INFO
	{
		RESPAWNED_ENVIRONMENTAL_GUARDIAN_INFO( StructMonster * _pMonster, GameContent::MONSTER_RESPAWN_INFO * _pRespawnInfo )
			: pMonster( _pMonster )
			, pRespawnInfo( _pRespawnInfo )
		{
			nNextRespawnTime = ( pMonster ) ? 0 : GetArTime();
		}

		StructMonster *							pMonster;
		AR_TIME									nNextRespawnTime;
		GameContent::MONSTER_RESPAWN_INFO *		pRespawnInfo;
	};

	std::vector< GameContent::MONSTER_RESPAWN_INFO >		vGuardianRespawnInfo;
	std::vector< GameContent::MONSTER_RESPAWN_INFO >		vEnvironmentalGuardianRespawnInfo;
	std::vector< RESPAWNED_ENVIRONMENTAL_GUARDIAN_INFO >	vRespawnedEnvironmentalGuardianInfo;
	std::queue< int >										qPendedGuardianRespawn;
	std::vector< GameContent::MONSTER_RESPAWN_INFO >		vRespawnInfo;
	std::vector< int >										vRoamerID;
	std::vector< _DUNGEON_RAID_INFO * >						vRaidInfo;
	std::vector< struct StructMonster * >					vMonsters; // guardian
	std::vector< GameContent::RANDOM_MONSTER_RESPAWN_INFO >	vRandomRespawnInfo;
	std::vector< _TACTICAL_POSITION_INFO >					vTacticalPositionInfo;
	std::vector< std::string >								vNotices;

protected:
	virtual void onMonsterDelete( struct StructMonster * pMonster );
};

void _DUNGEON_RAID_INFO::onMonsterDelete( struct StructMonster * pMonster )
{
	bool bChecked = false;
	bool bEndDungeonRaid = true;

	for( int i = 0; i < DUNGEON_BOSS_COUNT; ++i )
	{
		if( pDungeonInfo->boss_id[i] && !boss_dead[i] )
		{
			if( !bChecked && pDungeonInfo->boss_id[i] == pMonster->GetMonsterId() )
			{
				boss_dead[i] = true;
				bChecked = true;
			}
			else
			{
				bEndDungeonRaid = false;
			}

		}
	}

	{
		THREAD_SYNCRONIZE( pDungeonInfo->m_CS );

		for( std::vector< struct StructMonster * >::iterator it = vMonsters.begin(); it != vMonsters.end(); ++it )
		{
			if( (*it) == pMonster )
			{
				vMonsters.erase( it );
				break;
			}
		}

		if( bEndDungeonRaid )
		{
			raid_complete = true;
		}
	}
}

void _DUNGEON_INFO::onMonsterDelete( struct StructMonster * pMonster )
{
	THREAD_SYNCRONIZE( m_CS );

	bool bEndDungeonSiege = false;

	if( pConnector == pMonster )
	{
		bEndDungeonSiege = true;
		pConnector = NULL;
	}
	else
	{
#ifdef _DUNGEON_CORE_AS_MONSTER
		if( pDungeonCore == pMonster )
		{
			pDungeonCore = NULL;
			// 던전 코어의 주인 변경을 세팅하여 공수 교대 발생
			DungeonManager::Instance().ChangeOwner( id, 0 );
		}
		// 코어 가디언 핸들 리스트에서 제거(코어 가디언이 아니었으면 삭제되는 것 없음)
		else
		{
			stCoreGuardianHandle.erase( pMonster->GetHandle() );
		}
#endif
		for( std::vector< struct StructMonster * >::iterator it = vMonsters.begin(); it != vMonsters.end(); ++it )
		{
			if( (*it) == pMonster )
			{
				vMonsters.erase( it );
				break;
			}
		}

		for( std::vector< RESPAWNED_ENVIRONMENTAL_GUARDIAN_INFO >::iterator it = vRespawnedEnvironmentalGuardianInfo.begin() ; it != vRespawnedEnvironmentalGuardianInfo.end() ; ++it )
		{
			if( (*it).pMonster != pMonster )
				continue;

			(*it).nNextRespawnTime = GetArTime() + (*it).pRespawnInfo->interval;
			(*it).pMonster = NULL;
			break;
		}
	}

	if( bEndDungeonSiege )
		bDungeonSiegeNeedToDestroy = true;
}

void _DUNGEON_INFO::procRespawnEnvironmentalGuardian()
{
	AR_TIME tCurrent = GetArTime();

	for( std::vector< RESPAWNED_ENVIRONMENTAL_GUARDIAN_INFO >::iterator it = vRespawnedEnvironmentalGuardianInfo.begin() ; it != vRespawnedEnvironmentalGuardianInfo.end() ; /* 루프에서 ++it 처리 */ )
	{
		if( (*it).pMonster || (*it).nNextRespawnTime > tCurrent )
		{
			++it;
			continue;
		}

		AR_UNIT x = (*it).pRespawnInfo->left;
		AR_UNIT y = (*it).pRespawnInfo->top;

		if( GameContent::IsBlocked( x, y ) )
		{
			_cprint( "Environmental guardian monster(%d) could not be respawned at (%f,%f) in a dungeon(%d).\n", (*it).pRespawnInfo->monster_id, x, y, id );
			it = vRespawnedEnvironmentalGuardianInfo.erase( it );
			continue;
		}

		StructMonster * pMob = respawnMonster( x, y, DungeonManager::DUNGEON_SIEGE_LAYER, (*it).pRespawnInfo->monster_id, (*it).pRespawnInfo->is_wandering, (*it).pRespawnInfo->way_point_id, this, false );

		if( !pMob )
		{
			_cprint( "Environmental guardian monster(%d) could not be respawned in a dungeon(%d).\n", (*it).pRespawnInfo->monster_id, id );
			it = vRespawnedEnvironmentalGuardianInfo.erase( it );
			continue;
		}

		bool bOwnerGuardian = false;

		int nTacticalPositionID = (*it).pRespawnInfo->id;
		if( nTacticalPositionID == 0 )
		{
			if( original_owner_guild_id == owner_guild_id )
			{
				bOwnerGuardian = true;
			}
		}
		else
		{
			for( std::vector< _TACTICAL_POSITION_INFO >::iterator itTac = vTacticalPositionInfo.begin(); itTac != vTacticalPositionInfo.end(); ++itTac )
			{
				if( (*itTac).id == nTacticalPositionID )
				{
					if( (*itTac).own_by_original_owner )
					{
						bOwnerGuardian = true;
					}
					
					break;
				}
			}
		}

		if( bOwnerGuardian )
		{
			pMob->SetDungeonOwnerGuardian();
		}
		else
		{
			pMob->SetDungeonSiegerGuardian();
		}

		vMonsters.push_back( pMob );

		(*it).nNextRespawnTime = 0;
		(*it).pMonster = pMob;

		++it;
	}
}

time_t DungeonManager::GetDungeonRaidStartTime( int start_time )
{
	return GetWeekBeginTime() + start_time;
}

time_t DungeonManager::GetDungeonRaidEndTime( int end_time )
{
	return GetWeekBeginTime() + end_time;
}

time_t DungeonManager::GetNextDungeonSiegeStartTime( int start_time, time_t last_dungeon_siege_finish_time )
{
	time_t dungeon_siege_start_time = GetWeekBeginTime() + start_time;

	while( dungeon_siege_start_time <= last_dungeon_siege_finish_time )
		dungeon_siege_start_time += WEEK_TIME_IN_SECOND;

	return dungeon_siege_start_time;
}

time_t DungeonManager::GetNextDungeonSiegeEndTime( int end_time, time_t dungeon_siege_start_time )
{
	time_t dungeon_siege_end_time = GetWeekBeginTime() + end_time;

	while( dungeon_siege_end_time <= dungeon_siege_start_time )
		dungeon_siege_end_time += WEEK_TIME_IN_SECOND;

	return dungeon_siege_end_time;
}

void DungeonManager::RegisterDungeonInfo( int id, int level, int type, AR_UNIT raid_start_x, AR_UNIT raid_start_y, AR_UNIT siege_start_x, AR_UNIT siege_start_y, AR_UNIT siege_defence_x, AR_UNIT siege_defence_y, int connector_id, AR_UNIT connector_x, AR_UNIT connector_y, int core_id, AR_UNIT core_x, AR_UNIT core_y, float core_offset_z, float core_around_x, float core_around_y, float core_around_z, float scale_x, float scale_y, float scale_z, bool core_is_lock_height, float core_lock_height, int boss01_id, int boss02_id, int raid_start_time, int raid_end_time, int start_time, int end_time, X2D::Box< AR_UNIT > box, int owner_guild_id, int raid_guild_id, int best_raid_time, int last_dungeon_siege_finish_time, int last_dungeon_raid_wrap_up_time, int tax_rate, int max_guild_party, int max_raid_party )
{
	_DUNGEON_INFO info;

	info.id							= id;
	info.level						= level;
	info.type						= type;

	info.raid_start_pos				= ArPosition( raid_start_x, raid_start_y );
	info.siege_start_pos			= ArPosition( siege_start_x, siege_start_y );
	info.siege_defence_pos			= ArPosition( siege_defence_x, siege_defence_y );
	info.connector_id				= connector_id;
	info.connector_pos				= ArPosition( connector_x, connector_y );

	info.core_id					= core_id;
	info.core_pos					= ArPosition( core_x, core_y );

	info.raid_start_time			= raid_start_time;
	info.raid_end_time				= raid_end_time;

	info.core_offset_z				= core_offset_z;

	info.core_around_x				= core_around_x;
	info.core_around_y				= core_around_y;
	info.core_around_z				= core_around_z;

	info.core_scale_x				= scale_x;
	info.core_scale_y				= scale_y;
	info.core_scale_z				= scale_z;

	info.core_is_lock_height		= core_is_lock_height;
	info.core_lock_height			= core_lock_height;

	info.boss_id[0]					= boss01_id;
	info.boss_id[1]					= boss02_id;

	info.start_time					= start_time;
	info.end_time					= end_time;
	info.box						= box;
	info.owner_guild_id				= owner_guild_id;
	info.raid_guild_id				= raid_guild_id;
	info.best_raid_time				= best_raid_time;
	info.original_owner_guild_id	= owner_guild_id;

	info.last_dungeon_siege_finish_time	= last_dungeon_siege_finish_time;
	info.last_dungeon_raid_wrap_up_time = last_dungeon_raid_wrap_up_time;

	info.tax_rate					= tax_rate;

	info.max_guild_party			= max_guild_party;
	info.max_raid_party				= max_raid_party;

	m_vDungeonInfo.push_back( info );
}

void DungeonManager::LoadAllRaidInfo()
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		// 시즈 타입의 던전만 레이드 정보 로딩
		if( (*it).type == DUNGEON_TYPE_SIEGE )
			LoadRaidInfo( (*it).id, GetDungeonRaidStartTime( (*it).raid_start_time ) );
	}
}

void DungeonManager::OnEnterDungeon( const int nDungeonID, struct StructPlayer *pPlayer, const unsigned char nLayer )
{
	// AddState를 동반하므로 지역 락이 필요하다.
	if( nLayer ) return;
	int nGuildID = pPlayer->GetGuildID();
	if( !nGuildID ) return;

	for( std::vector< _DUNGEON_INFO >::const_iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			if( GuildManager::GetInstance().GetAllianceID( nGuildID ) )
			{
				nGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( GuildManager::GetInstance().GetAllianceID( nGuildID ) );
			}
 
			if( nGuildID == (*it).original_owner_guild_id )
				pPlayer->AddDungeonState();

			return;
		}
	}
}

void DungeonManager::OnExitDungeon( const int nDungeonID, struct StructPlayer *pPlayer, int nGuildID, const unsigned char nLayer )
{
	// RemoveState를 동반하므로 지역 락이 필요하다.
	if( nLayer ) return;
	if( !nGuildID ) return;

	for( std::vector< _DUNGEON_INFO >::const_iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			if( GuildManager::GetInstance().GetAllianceID( nGuildID ) )
			{
				nGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( GuildManager::GetInstance().GetAllianceID( nGuildID ) );
			}

			// 퇴장 이전에 던전 소유 길드가 변경되었으나 그에 따른 처리는 비동기로 동작하므로 아직 수행되지 않았을 수도 있다.
			// 따라서 소유권의 변동이 있었지만 그에 따른 처리가 발생하지 않았다면, 현재 던전 소유 길드는 아니지만 직전 던전 소유 길드인지를 확인하여 필요에 따라 퇴장 처리도 해주어야 한다.
			if( nGuildID == (*it).original_owner_guild_id ||
				( (*it).bDungeonOwnerChanged && nGuildID == (*it).prev_owner_guild_id ) )
				pPlayer->RemoveDungeonState();

			break;
		}
	}
}

void DungeonManager::RegisterTacticalPositionInfo( int dungeon_id, int id, AR_UNIT x, AR_UNIT y, int prop_id )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == dungeon_id )
		{
			for( std::vector< _TACTICAL_POSITION_INFO >::iterator itTac = (*it).vTacticalPositionInfo.begin(); itTac != (*it).vTacticalPositionInfo.end(); ++itTac )
			{
				if( (*itTac).id == id )
				{
					(*itTac).pos = ArPosition( x, y );
					(*itTac).prop_id = prop_id;

					return;
				}
			}

			_TACTICAL_POSITION_INFO info;

			info.id = id;
			info.pos = ArPosition( x, y );
			info.prop_id = prop_id;

			(*it).vTacticalPositionInfo.push_back( info );

			break;
		}
	}
}

void DungeonManager::RegisterDungeonRaidInfo( int dungeon_id, int guild_id, AR_TIME record, time_t raid_end_time )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == dungeon_id )
		{
			(*it).vRaidInfo.push_back( new _DUNGEON_RAID_INFO( guild_id, record, raid_end_time, &(*it) ) );
		}
	}
}

void DungeonManager::RegisterDungeonMonsterRespawnInfo( int nDungeonID, const struct GameContent::MONSTER_RESPAWN_INFO & info )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			(*it).vRespawnInfo.push_back( info );
			return;
		}
	}
} 

void DungeonManager::RegisterDungeonGuardianRespawnInfo( int nDungeonID, const GameContent::MONSTER_RESPAWN_INFO & info )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			(*it).vGuardianRespawnInfo.push_back( info );
			return;
		}
	}
}

void DungeonManager::RegisterDungeonEnvironmentalGuardianRespawnInfo( int nDungeonID, const GameContent::MONSTER_RESPAWN_INFO & info )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			(*it).vEnvironmentalGuardianRespawnInfo.push_back( info );
			return;
		}
	}
}

void DungeonManager::RegisterDungeonRoamerInfo( int nDungeonID, const int nRoamingID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			(*it).vRoamerID.push_back( nRoamingID );
			return;
		}
	}
} 

void DungeonManager::RegisterRandomDungeonMonsterRespawnInfo( int nDungeonID, const struct GameContent::RANDOM_MONSTER_RESPAWN_INFO & info )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			(*it).vRandomRespawnInfo.push_back( info );
			return;
		}
	}
}

bool DungeonManager::Init()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_HIGH );
	return true;
}

bool DungeonManager::DeInit()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_IDLE );
	return true;
}

DungeonManager & DungeonManager::Instance()
{
	static DungeonManager _inst;
	return _inst;
}

ArPosition DungeonManager::GetRaidStartPosition( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			return (*it).raid_start_pos;
		}
	}

	assert( 0 );

	return ArPosition();
}

ArPosition DungeonManager::GetSiegeStartPosition( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			return (*it).siege_start_pos;
		}
	}

	assert( 0 );

	return ArPosition();
}

ArPosition DungeonManager::GetSiegeDefencePosition( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			return (*it).siege_defence_pos;
		}
	}

	assert( 0 );

	return ArPosition();
}

void DungeonManager::wrapUpDungeonRaid( _DUNGEON_INFO * pDungeonInfo )
{
	pDungeonInfo->raid_guild_id = 0;
	pDungeonInfo->best_raid_time = AR_TIME( -1 );

	for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = pDungeonInfo->vRaidInfo.begin(); itRaidInfo != pDungeonInfo->vRaidInfo.end(); ++itRaidInfo )
	{
		if( (*itRaidInfo)->layer )
			return;
	}

	for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = pDungeonInfo->vRaidInfo.begin(); itRaidInfo != pDungeonInfo->vRaidInfo.end(); ++itRaidInfo )
	{
		if( (*itRaidInfo)->raid_record )
		{
			if( pDungeonInfo->best_raid_time > (*itRaidInfo)->raid_record ) 
			{
				pDungeonInfo->best_raid_time = (*itRaidInfo)->raid_record;
				pDungeonInfo->raid_guild_id = (*itRaidInfo)->guild_id;
			}

			(*itRaidInfo)->raid_record = 0;

			// 이번 주 기록 중에 성공 기록이 있다면 여긴 무조건 실패이므로 굳이 기록할 필요가 없음
			// 성공 기록이 없다면 실패를 한 시점에 대한 정보를 남기는 겸 기록해 줌
			AR_TIME tRecord = getDungeonRaidRecord( pDungeonInfo, (*itRaidInfo)->guild_id );
			if( tRecord <= 0 )
				DB().Push( new DB_UpdateDungeonRaidTime( pDungeonInfo->id, (*itRaidInfo)->guild_id, time( NULL ), (*itRaidInfo)->raid_record ) );
		}
	}

	struct _myGuildListFunctor : public GuildManager::GuildListFunctor
	{
		_myGuildListFunctor( int _dungeon_id, int _own_guild_id, int _raid_guild_id ) : m_dungeon_id( _dungeon_id ), m_dungeon_own_guild_id( _own_guild_id ), m_dungeon_raid_guild_id( _raid_guild_id )
		{
		}

		virtual bool operator()( int nGuildID, const char * szGuildName, const char * szGuildLeaderName, int nGuildLeaderLevel, int nGuildMemberCount, int nRaidDungeonID )
		{
			if( m_dungeon_id == nRaidDungeonID )
			{
				if( nGuildID != m_dungeon_own_guild_id && nGuildID != m_dungeon_raid_guild_id )
					GuildManager::GetInstance().SetRaidDungeonID( nGuildID, 0 );

				int nLeadPartyID = PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( nGuildID );

				if( nLeadPartyID )
				{
					BroadcastPartyDestroy( nLeadPartyID );

					LOG::Log11N4S( LM_PARTY_DESTROY, 0, 0, nLeadPartyID, 0, 0, 0, 7, 0, 0, 0, 0,
						"", 0, "", 0, PartyManager::GetInstance().GetPartyName( nLeadPartyID ).c_str(), LOG::STR_NTS, "", 0 );

					PartyManager::GetInstance().DestroyParty( nLeadPartyID );
				}
			}

			return true;
		}

		int m_dungeon_id;
		int m_dungeon_own_guild_id;
		int m_dungeon_raid_guild_id;

	} _fo( pDungeonInfo->id, pDungeonInfo->owner_guild_id, pDungeonInfo->raid_guild_id );

	GuildManager::GetInstance().DoEachGuild( _fo );

	pDungeonInfo->last_dungeon_raid_wrap_up_time = time( NULL );

	DB().Push( new DB_UpdateDungeon( pDungeonInfo->id, pDungeonInfo->owner_guild_id, pDungeonInfo->raid_guild_id, pDungeonInfo->best_raid_time, pDungeonInfo->last_dungeon_siege_finish_time, pDungeonInfo->last_dungeon_raid_wrap_up_time, pDungeonInfo->tax_rate ) );
}

void DungeonManager::wrapUpDungeonSiege( _DUNGEON_INFO *pDungeonInfo )
{
	// endDungeonSiege 함수에서 각 시즈공대 박살내주긴 하지만 end와 wrapUp 사이에 공대가 다시 만들어질 수도 있으므로 다시 박살내자
	if( pDungeonInfo->owner_guild_id )
	{
		int nLeadPartyID = PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( pDungeonInfo->owner_guild_id );

		if( nLeadPartyID )
		{
			BroadcastPartyDestroy( nLeadPartyID );

			LOG::Log11N4S( LM_PARTY_DESTROY, 0, 0, nLeadPartyID, 0, 0, 0, 8, 0, 0, 0, 0,
				"", 0, "", 0, PartyManager::GetInstance().GetPartyName( nLeadPartyID ).c_str(), LOG::STR_NTS, "", 0 );

			PartyManager::GetInstance().DestroyParty( nLeadPartyID );
		}
	}

	if( pDungeonInfo->raid_guild_id )
	{
		int nLeadPartyID = PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( pDungeonInfo->raid_guild_id );

		if( nLeadPartyID )
		{
			BroadcastPartyDestroy( nLeadPartyID );

			LOG::Log11N4S( LM_PARTY_DESTROY, 0, 0, nLeadPartyID, 0, 0, 0, 8, 0, 0, 0, 0,
				"", 0, "", 0, PartyManager::GetInstance().GetPartyName( nLeadPartyID ).c_str(), LOG::STR_NTS, "", 0 );

			PartyManager::GetInstance().DestroyParty( nLeadPartyID );
		}
	}

	// 던전 시즈가 종료될 때 DB와 메모리상의 던전 레이드 기록을 모두 제거
	// _DUNGEON_INFO 의 m_CS 락은 wrapUpDungeonRaid 함수 호출측(DungeonManager::onProcess)에서 걸었음
	for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = pDungeonInfo->vRaidInfo.begin(); itRaidInfo != pDungeonInfo->vRaidInfo.end(); ++itRaidInfo )
	{
		delete (*itRaidInfo);
	}
	pDungeonInfo->vRaidInfo.clear();

	DB().Push( new DB_ClearDungeonRaidRecord( pDungeonInfo->id, 0 ) );
}

void DungeonManager::onChangeDungeonOwner( _DUNGEON_INFO * pDungeonInfo, const int nPrevOwnerID, const int nOwnerID )
{
	struct _MessageSender : ArObjectFunctor
	{
		_MessageSender( const int _nPrevOwnerID, const int _nOwnerID ) : nPrevOwnerID( _nPrevOwnerID ), nOwnerID( _nOwnerID ) {}

		void operator()( ArObject *pObj ) const
		{
			StructPlayer *pPlayer = static_cast< StructPlayer * >( pObj );
			int nGuildID = pPlayer->GetGuildID();

			if( !nGuildID ) return;

			if( GuildManager::GetInstance().GetAllianceID( nGuildID ) )
			{
				nGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( GuildManager::GetInstance().GetAllianceID( nGuildID ) );
			}

			if( nPrevOwnerID && nGuildID == nPrevOwnerID )
			{
				pPlayer->RemoveDungeonState();
			}
			else if( nOwnerID && nGuildID == nOwnerID )
			{
				pPlayer->AddDungeonState();
			}
		}

		int nPrevOwnerID;
		int nOwnerID;
	} _sender( nPrevOwnerID, nOwnerID );

	ForEachClientInDungeon( pDungeonInfo, DUNGEON_LAYER, _sender );
}

bool DungeonManager::beginDungeonRaid( _DUNGEON_RAID_INFO * pRaidInfo )
{
	_DUNGEON_INFO * pDungeonInfo = pRaidInfo->pDungeonInfo;

	if( !pRaidInfo->raid_starting )
		return false;

	struct _myPartyFuctor : PartyManager::PartyFunctor
	{
		_myPartyFuctor( const struct _DUNGEON_RAID_INFO * _pRaidInfo ) : pRaidInfo( _pRaidInfo ) {}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator it = StructPlayer::get( handle );

			if ( !(*it)->IsPlayer() || !(*it)->IsLogin() || !(*it)->IsInWorld() || DungeonManager::Instance().IsRestrictedToEnter( pRaidInfo->pDungeonInfo->level, (*it)->GetLevel() ) )
				return false;

			(*it)->PendWarp( pRaidInfo->pDungeonInfo->raid_start_pos.x, pRaidInfo->pDungeonInfo->raid_start_pos.y, pRaidInfo->layer );

			ArcadiaServer::Instance().SetObjectPriority( (*it), ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

			return true;
		};

		const _DUNGEON_RAID_INFO * pRaidInfo;
	} _fo( pRaidInfo );

	PartyManager::GetInstance().DoEachAttackTeamMemberByGuildId( pRaidInfo->guild_id, _fo );

	for( std::vector< GameContent::MONSTER_RESPAWN_INFO >::iterator itRespawn = pDungeonInfo->vRespawnInfo.begin(); itRespawn != pDungeonInfo->vRespawnInfo.end(); ++itRespawn )
	{
		for( unsigned int i = 0; i < (*itRespawn).max_num; ++i ) // always max~
		{
			AR_UNIT x, y;
			unsigned try_cnt = 0;

			bool bValidPos = true;

			do
			{
				x = XRandom( (*itRespawn).left, (*itRespawn).right );
				y = XRandom( (*itRespawn).top, (*itRespawn).bottom );

				if( ++try_cnt > 300 )
				{
					bValidPos = false;
					break;
				}
			} while( GameContent::IsBlocked( x, y ) );

			if( bValidPos )
			{
				StructMonster * pMob = respawnMonster( x, y, pRaidInfo->layer, (*itRespawn).monster_id, (*itRespawn).is_wandering, (*itRespawn).way_point_id, pRaidInfo, false );

				if( pMob )
				{
					pMob->SetDungeonRaidMonster();
					pMob->CalculateStat();

					pRaidInfo->vMonsters.push_back( pMob );
				}
			}
		}
	}

	for( std::vector< GameContent::MONSTER_RESPAWN_INFO >::iterator itRespawn = pDungeonInfo->vGuardianRespawnInfo.begin(); itRespawn != pDungeonInfo->vGuardianRespawnInfo.end(); ++itRespawn )
	{
		for( unsigned int i = 0; i < (*itRespawn).max_num; ++i ) // always max~
		{
			AR_UNIT x = (*itRespawn).left;
			AR_UNIT y = (*itRespawn).top;

			if( GameContent::IsBlocked( x, y ) )
			{
				_cprint( "Guardian monster(%d) could not be respawned at (%f,%f) in a dungeon(%d).\n", (*itRespawn).monster_id, x, y, pDungeonInfo->id );
				continue;
			}

			StructMonster * pMob = respawnMonster( x, y, pRaidInfo->layer, (*itRespawn).monster_id, (*itRespawn).is_wandering, (*itRespawn).way_point_id, pRaidInfo, false );

			if( pMob )
			{
				pMob->SetDungeonRaidMonster();
				pMob->CalculateStat();

				pRaidInfo->vMonsters.push_back( pMob );
			}
		}
	}

	// 환경 가디언 몬스터는 레이드 진행 시에는 일반 몬스터 리젠 정보에 의해 리젠되며, 시즈 진행 시에만 별도로 리젠시킴

	pRaidInfo->raid_starting = false;

	for( std::vector< GameContent::RANDOM_MONSTER_RESPAWN_INFO >::iterator itRespawn = pDungeonInfo->vRandomRespawnInfo.begin(); itRespawn != pDungeonInfo->vRandomRespawnInfo.end(); ++itRespawn )
	{
		for( unsigned int i = 0; i < (*itRespawn).prespawn_count; ++i ) // prespawn count equals max, always max~
		{
			AR_UNIT x, y;
			unsigned try_cnt = 0;

			bool bValidPos = true;

			do
			{
				GameContent::AREA_BOX randomBox = GameContent::GetRandomRespawnBox( (*itRespawn).random_area_id );
				x = XRandom( randomBox.left, randomBox.right );
				y = XRandom( randomBox.top, randomBox.bottom );

				if( ++try_cnt > 300 )
				{
					bValidPos = false;
					break;
				}
			} while( GameContent::IsBlocked( x, y ) );

			if( bValidPos )
			{
				unsigned int monster_id = 0;
				int nCum = 0;
				int nKey = XRandom( 1, 100000000 );

				for( std::vector< std::pair< int, int > >::const_iterator it = (*itRespawn).monster_list.begin(); it != (*itRespawn).monster_list.end(); it++ )
				{
					if( !(*it).first ) break;

					nCum += (*it).second;

					if( nKey > nCum ) continue;

					monster_id = (*it).first;
					break;
				}

				StructMonster * pMob = respawnMonster( x, y, pRaidInfo->layer, monster_id, (*itRespawn).is_wandering, (*itRespawn).way_point_id, pRaidInfo, false );

				if( pMob )
				{
					pMob->SetDungeonRaidMonster();
					pMob->CalculateStat();

					pRaidInfo->vMonsters.push_back( pMob );
				}
			}
		}
	}

	return true;
}

const AR_TIME DungeonManager::GetDungeonRaidRecord( int nDungeonID, int nGuildID ) const
{
	for( std::vector< _DUNGEON_INFO >::const_iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			return getDungeonRaidRecord( &(*it), nGuildID );
		}
	}

	return 0;
}

void DungeonManager::GetTopDungeonRaidRecord( const int nDungeonID, int & nGuildID, AR_TIME & nRecord ) const
{
	for( std::vector< _DUNGEON_INFO >::const_iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			return getTopDungeonRaidRecord( &(*it), nGuildID, nRecord );
		}
	}
}

const AR_TIME DungeonManager::getDungeonRaidRecord( const _DUNGEON_INFO * pDungeonInfo, const int nGuildID ) const
{
	time_t t = time( NULL );

	if( GetDungeonRaidEndTime( pDungeonInfo->raid_end_time ) < t && pDungeonInfo->last_dungeon_raid_wrap_up_time > GetWeekBeginTime() )
	{
		if( nGuildID == pDungeonInfo->raid_guild_id )
			return pDungeonInfo->best_raid_time;
		else
			return 0;
	}

	AR_TIME best_raid_record = 0;

	for( std::vector< _DUNGEON_RAID_INFO * >::const_iterator itRaidInfo = pDungeonInfo->vRaidInfo.begin() ; itRaidInfo != pDungeonInfo->vRaidInfo.end() ; ++itRaidInfo )
	{
		if( (*itRaidInfo)->guild_id == nGuildID )
		{
			if( best_raid_record <= 0 || ( 0 < (*itRaidInfo)->raid_record && (*itRaidInfo)->raid_record < best_raid_record ) )
				best_raid_record = (*itRaidInfo)->raid_record;
		}
	}

	return best_raid_record;
}

void DungeonManager::getTopDungeonRaidRecord( const _DUNGEON_INFO * pDungeonInfo, int & nGuildID, AR_TIME & nRecord ) const
{
	time_t t = time( NULL );

	// 레이드 기간이 아니라면 공격자 길드의 레이드 정보를 사용(없으면 0)
	if( GetDungeonRaidEndTime( pDungeonInfo->raid_end_time ) < t && pDungeonInfo->last_dungeon_raid_wrap_up_time > GetWeekBeginTime() )
	{
		if( nGuildID == pDungeonInfo->raid_guild_id )
		{
			nGuildID = pDungeonInfo->raid_guild_id;
			nRecord = pDungeonInfo->best_raid_time;
		}
		else
		{
			nGuildID = 0;
			nRecord = 0;
		}

		return;
	}

	// 레이드 기간이라면 기록 중 최고 기록을 찾음
	if( pDungeonInfo->vRaidInfo.empty() )
	{
		nGuildID = 0;
		nRecord = 0;
		return;
	}

	int best_guild_id = 0;
	AR_TIME best_raid_record = 0;

	for( std::vector< _DUNGEON_RAID_INFO * >::const_iterator itRaidInfo = pDungeonInfo->vRaidInfo.begin() ; itRaidInfo != pDungeonInfo->vRaidInfo.end() ; ++itRaidInfo )
	{
		// 완료된 레이드 기록에 대해서만 지금까지 나온 최고 기록보다 좋은 기록인지 체크
		if( best_raid_record <= 0 || ( 0 < (*itRaidInfo)->raid_record && (*itRaidInfo)->raid_record < best_raid_record ) )
		{
			best_guild_id = (*itRaidInfo)->guild_id;
			best_raid_record = (*itRaidInfo)->raid_record;
		}
	}

	if( best_guild_id )
	{
		nGuildID = best_guild_id;
		nRecord = best_raid_record;
	}
}

void DungeonManager::ClearDungeonRaidRecord( int nDungeonID, int nGuildID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			clearDungeonRaidRecord( &(*it), nGuildID );
			break;
		}
	}

	DB().Push( new DB_ClearDungeonRaidRecord( nDungeonID, nGuildID ) );
}

void DungeonManager::clearDungeonRaidRecord( _DUNGEON_INFO *pDungeonInfo, int nGuildID )
{
	for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaid = pDungeonInfo->vRaidInfo.begin();
		pDungeonInfo->vRaidInfo.empty() == false && itRaid != pDungeonInfo->vRaidInfo.end();
		/* 루프에서 ++itRaid 처리 */ )
	{
		if( (*itRaid)->guild_id == nGuildID )
		{
			delete (*itRaid);
			vector_fast_erase( &pDungeonInfo->vRaidInfo, itRaid );
			// 마지막 요소와 현재 요소를 바꿔치기 했으니 현재 요소를 다시 검사해야 하므로 ++itRaid 안 함

			// DB 갱신 따로 안 하니 DB_ClearDungeonRaidRecord를 쓰던 DB_DeleteGuild 등을 쓰던 알아서 처리해야 함
			continue;
		}
		++itRaid;
	}
}

time_t DungeonManager::GetLastRaidEndTime( int nDungeonID, int nGuildID )
{
	time_t tLastRaidEndTime = 0;

	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			// 레이드 기록 중에서 최근 종료 시간을 얻음
			for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = (*it).vRaidInfo.begin(); itRaidInfo != (*it).vRaidInfo.end(); ++itRaidInfo )
			{
				if( (*itRaidInfo)->guild_id == nGuildID && (*itRaidInfo)->end_time > GetWeekBeginTime() && (*itRaidInfo)->end_time > tLastRaidEndTime )
				{
					tLastRaidEndTime = (*itRaidInfo)->end_time;
				}
			}

			return tLastRaidEndTime;
		}
	}

	return 0;
}

AR_TIME DungeonManager::GetElaspedRaidTime( int nDungeonID, unsigned char layer )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = (*it).vRaidInfo.begin(); itRaidInfo != (*it).vRaidInfo.end(); ++itRaidInfo )
			{
				if( (*itRaidInfo)->layer == layer && !(*itRaidInfo)->end_time )
				{
					return GetArTime() - (*itRaidInfo)->start_time;
				}
			}
		}
	}

	return -1;
}

unsigned char DungeonManager::GetRaidDungeonLayer( int nDungeonID, int nGuildID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = (*it).vRaidInfo.begin(); itRaidInfo != (*it).vRaidInfo.end(); ++itRaidInfo )
			{
				if( (*itRaidInfo)->guild_id == nGuildID )
				{
					// 진행 중인 레이드 정보이면 해당 레이어 리턴
					// 진행 중인 레이드 정보가 아니었다면 다음 레이드 정보 검사하도록 넘어감(1주간 레이드가 4회까지 가능하므로)
					if( (*itRaidInfo)->layer && (*itRaidInfo)->start_time && !(*itRaidInfo)->raid_starting )
					{
						return (*itRaidInfo)->layer;
					}
				}
			}

			return 0;
		}
	}

	return -1;
}

bool DungeonManager::IsRaidBegin( int nDungeonID, int nGuildID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = (*it).vRaidInfo.begin(); itRaidInfo != (*it).vRaidInfo.end(); ++itRaidInfo )
			{
				if( (*itRaidInfo)->guild_id == nGuildID )
				{
					if( (*itRaidInfo)->layer && !(*itRaidInfo)->end_time )
						return true;

					//if( (*itRaidInfo)->raid_record )
					//	return true;
				}
			}

			return false;
		}
	}

	return false;
}

bool DungeonManager::BeginDungeonRaid( int nDungeonID, int nGuildID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			if( (*it).bDungeonSiegeCreated )
				return false;

			if( (*it).owner_guild_id == nGuildID )
				return false;

			std::vector< unsigned char > vLayers;

			_DUNGEON_RAID_INFO * pRaidInfo = NULL;
			for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = (*it).vRaidInfo.begin(); itRaidInfo != (*it).vRaidInfo.end(); ++itRaidInfo )
			{
				if( (*itRaidInfo)->guild_id == nGuildID )
				{
					if( (*itRaidInfo)->layer )
						//return false;
						continue;

					if( (*itRaidInfo)->raid_record )
						//return false;
						continue;

					pRaidInfo = (*itRaidInfo);
				}
				else if( (*itRaidInfo)->layer )
				{
					vLayers.push_back( (*itRaidInfo)->layer );
				}
			}

			std::sort( vLayers.begin(), vLayers.end() );

			unsigned char layer = DUNGEON_SIEGE_LAYER + 1;

			for( std::vector< unsigned char >::iterator itLayers = vLayers.begin(); itLayers != vLayers.end(); ++itLayers, ++layer )
			{
				if( (*itLayers) != layer )
					break;
			}

			if( layer >= CUSTOMIZE_DUNGEON_LAYER )
				return false;


			if( !pRaidInfo )
			{
				(*it).vRaidInfo.push_back( new _DUNGEON_RAID_INFO( nGuildID, 0, 0, &(*it) ) );

				pRaidInfo = (*it).vRaidInfo.back();
			}

			pRaidInfo->layer = layer;
			pRaidInfo->start_time = GetArTime();
			pRaidInfo->last_notice_time = time( NULL );
			pRaidInfo->raid_starting = true;

			for( int i = 0; i < DUNGEON_BOSS_COUNT; ++i )
			{
				pRaidInfo->boss_dead[i] = false;
			}

			SendGuildChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", nGuildID, "@681");

			return true;
		}
	}

	return false;
}

bool DungeonManager::endDungeonRaid( struct _DUNGEON_RAID_INFO * pRaidInfo, bool bCancel )
{
	if( !pRaidInfo )
		return false;

	if( bCancel )
	{
		pRaidInfo->raid_record = AR_TIME(-1);
	}
	else
	{
		pRaidInfo->raid_record = GetArTime() - pRaidInfo->start_time;
	}

	pRaidInfo->start_time = 0;
	pRaidInfo->end_time = time( NULL );

	for( std::vector< StructMonster * >::iterator itMonster = pRaidInfo->vMonsters.begin(); itMonster != pRaidInfo->vMonsters.end(); )
	{
		if( (*itMonster)->IsEnable() )
		{
			(*itMonster)->SetDeleteHandler( NULL );

			// 더이상의 스케쥴러 요청을 무시
			(*itMonster)->Disable();

			// 월드에서 제거한다.
			if( (*itMonster)->IsInWorld() )
			{
				RemoveMonsterFromWorld( (*itMonster) );
			}

			// object delete 요청
			ArcadiaServer::Instance().DeleteObject( (*itMonster) );

			itMonster = pRaidInfo->vMonsters.erase( itMonster );
		}
		else
		{
			++itMonster;
		}
	}

	// 레이드가 완료되었을 때, 기존에 성공한 기록이 없다면 무조건 기록
	// 기존에 성공한 기록이 있을 경우는 이번이 성공이고, 이번 기록이 기존 기록보다 짧아야만 기록
	AR_TIME tRecord = GetDungeonRaidRecord( pRaidInfo->pDungeonInfo->id, pRaidInfo->guild_id );
	if( tRecord <= 0 || ( tRecord > 0 && pRaidInfo->raid_record > 0 && tRecord >= pRaidInfo->raid_record ) )
		DB().Push( new DB_UpdateDungeonRaidTime( pRaidInfo->pDungeonInfo->id, pRaidInfo->guild_id, time( NULL ), pRaidInfo->raid_record ) );

	int nBossKillCount = 0;

	for( int i = 0; i < DUNGEON_BOSS_COUNT; ++i )
	{
		if( pRaidInfo->boss_dead[i] )
			++nBossKillCount;
	}

	const char * szRes = bCancel ? "FAIL" : "SUCS";

	broadcastRaidResult( pRaidInfo, !bCancel );

	LOG::Log11N4S( LM_END_DUNGEON_RAID, 0, 0, 0, pRaidInfo->guild_id, 0, 0, 0, 0, ( pRaidInfo->raid_record == -1 ) ? -1 : ( pRaidInfo->raid_record / 100 ), nBossKillCount, pRaidInfo->pDungeonInfo->id, "", 0, "", 0, "", 0, szRes, LOG::STR_NTS );

	int nLeadPartyID = PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( pRaidInfo->guild_id );

	if( nLeadPartyID )
	{
		BroadcastPartyDestroy( nLeadPartyID );

		LOG::Log11N4S( LM_PARTY_DESTROY, 0, 0, nLeadPartyID, 0, 0, 0, 9, 0, 0, 0, 0,
			"", 0, "", 0, PartyManager::GetInstance().GetPartyName( nLeadPartyID ).c_str(), LOG::STR_NTS, "", 0 );

		PartyManager::GetInstance().DestroyParty( nLeadPartyID );
	}

	return true;
}

int DungeonManager::GetDungeonID( AR_UNIT x, AR_UNIT y ) const
{
	X2D::Point< AR_UNIT > pt( x, y );

	for( std::vector< _DUNGEON_INFO >::const_iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).box.IsInclude( pt ) )
		{
			return (*it).id;
		}
	}

	return 0;
}

int DungeonManager::GetDungeonLevel( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			return (*it).level;
		}
	}

	return 0;
}

int DungeonManager::GetMaxRaidParty( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			return (*it).max_raid_party;
		}
	}

	return 0;
}

int DungeonManager::GetMaxGuildParty( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			return (*it).max_guild_party;
		}
	}

	return 0;
}

int DungeonManager::GetOriginalOwnGuildID( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			return (*it).original_owner_guild_id;
		}
	}

	return 0;
}

int DungeonManager::GetOwnGuildID( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			return (*it).owner_guild_id;
		}
	}

	return 0;
}

bool DungeonManager::SetOwnGuildID( const int & nDungeonID, const int & nGuildID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			// 이미 주인이면 패스
			if( (*it).owner_guild_id == nGuildID )
			{
				_cprint( "DungeonManager::SetOwnGuildID: Already the owner of that dungeon.(DungeonID: %d, Guild: %d)\n", nDungeonID, nGuildID );
				FILELOG( "DungeonManager::SetOwnGuildID: Already the owner of that dungeon.(DungeonID: %d, Guild: %d)", nDungeonID, nGuildID );
				return false;
			}

			// 시즈 진행 중에는 주인 길드 변경 불가
			if( (*it).bDungeonSiege )
			{
				_cprint( "DungeonManager::SetOwnGuildID: Tryed to change dungeon owner guild while a siege of that dungeon is going on.(DungeonID: %d, Guild: %d)\n", nDungeonID, nGuildID );
				FILELOG( "DungeonManager::SetOwnGuildID: Tryed to change dungeon owner guild while a siege of that dungeon is going on.(DungeonID: %d, Guild: %d)", nDungeonID, nGuildID );
				return false;
			}

			// 소유자 길드 변경 시 변경 대상 길드가 유효한 길드인지 체크
			if( nGuildID && GuildManager::GetInstance().GetGuildName( nGuildID ).empty() )
			{
				_cprint( "DungeonManager::SetOwnGuildID: Invalid new owner guild id.(DungeonID: %d, Guild: %d)\n", nDungeonID, nGuildID );
				FILELOG( "DungeonManager::SetOwnGuildID: Invalid new owner guild id.(DungeonID: %d, Guild: %d)", nDungeonID, nGuildID );
				return false;
			}

			int nRaidDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nGuildID );
			// 현재 던전에 신청되지 않은 길드라면
			if( nRaidDungeonID != nDungeonID )
			{
				// 타 던전에 신청되어 있는(혹은 소유 중인) 길드
				if( nRaidDungeonID )
				{
					for( std::vector< _DUNGEON_INFO >::iterator itRaid = m_vDungeonInfo.begin(); itRaid != m_vDungeonInfo.end(); ++itRaid )
					{
						if( (*itRaid).id == nRaidDungeonID )
						{
							THREAD_SYNCRONIZE1( (*itRaid).m_CS );

							// 새로운 주인 길드가 타 던전 소유 중인 길드면 불가
							if( (*itRaid).owner_guild_id == nGuildID )
							{
								_cprint( "DungeonManager::SetOwnGuildID: New owner guild is already owning a dungeon.(DungeonID: %d, Guild: %d)\n", nDungeonID, nGuildID );
								FILELOG( "DungeonManager::SetOwnGuildID: New owner guild is already owning a dungeon.(DungeonID: %d, Guild: %d)", nDungeonID, nGuildID );
								return false;
							}

							// 새로운 주인 길드가 타 던전 레이드/시즈 진행 중이면 불가
							bool bIsInRaid = false;
							for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = (*itRaid).vRaidInfo.begin(); itRaidInfo != (*itRaid).vRaidInfo.end(); ++itRaidInfo )
							{
								if( (*itRaidInfo)->guild_id == nGuildID && (*itRaidInfo)->layer && !(*itRaidInfo)->end_time )
								{
									bIsInRaid = true;
									break;
								}
							}
							if( (*itRaid).bDungeonSiege || bIsInRaid )
							{
								_cprint( "DungeonManager::SetOwnGuildID: New owner guild is in a dungeon siege or raid.(DungeonID: %d, Guild: %d)\n", nDungeonID, nGuildID );
								FILELOG( "DungeonManager::SetOwnGuildID: New owner guild is in a dungeon siege or raid.(DungeonID: %d, Guild: %d)", nDungeonID, nGuildID );
								return false;
							}

							// 기존 던전 신청 정보 제거
							clearDungeonRaidRecord( &(*itRaid), nGuildID );
							DB().Push( new DB_ClearDungeonRaidRecord( (*itRaid).id, nGuildID ) );
							if( (*itRaid).raid_guild_id == nGuildID )
							{
								(*itRaid).raid_guild_id = 0;
							}

							break;
						}
					}
				}

				GuildManager::GetInstance().SetRaidDungeonID( nGuildID, nDungeonID );
			}

			(*it).prev_owner_guild_id = (*it).owner_guild_id;
			(*it).owner_guild_id = nGuildID;
			(*it).original_owner_guild_id = nGuildID;

			// 던전의 소유주가 바뀐다.
			// 이에 따른 처리를 위해 던전 전체에 락이 필요한데 DropDungeonOwnership은 이미 지역락이 걸려있으므로 플래그만 세팅해둔다.
			// 만약 이전에 발생한 소유주 변경에 따른 처리가 되지 않은 상태(bDungeonOwnerChanged == true)에서 다시 한 번 소유주가 바뀌게 되면 문제가 발생할 것이다.
			assert( !(*it).bDungeonOwnerChanged );
			(*it).bDungeonOwnerChanged = true;

			// 주인이 바뀌는 경우에는 신규 던전 주인 길드의 레이드 기록도 함께 삭제 됨
			DB().Push( new DB_UpdateDungeonOwnerGuild( nDungeonID, nGuildID ) );

			LOG::Log11N4S( LM_DUNGEON_CHANGE_OWNER, 0, 0, 0, nGuildID, (*it).prev_owner_guild_id, 0, 0, 0, 0, 0, nDungeonID,
				GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(), LOG::STR_NTS,
				GuildManager::GetInstance().GetGuildName( (*it).prev_owner_guild_id ).c_str(), LOG::STR_NTS,
				"Script", LOG::STR_NTS, "", LOG::STR_NTS );

			return true;
		}
	}

	return false;
}

bool DungeonManager::ClearRaidGuildID( const int & nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			if( (*it).raid_guild_id )
			{
				for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = (*it).vRaidInfo.begin();
					(*it).vRaidInfo.empty() == false && itRaidInfo != (*it).vRaidInfo.end();
					/* 루프에서 ++it 처리 */ )
				{
					if( (*itRaidInfo)->guild_id == (*it).raid_guild_id )
					{
						delete (*itRaidInfo);

						vector_fast_erase( &(*it).vRaidInfo, itRaidInfo );
						// 마지막 요소와 현재 요소를 바꿔치기 했으니 현재 요소를 다시 검사해야 하므로 ++itRaidInfo 안 함
						continue;
					}
					++itRaidInfo;
				}

				(*it).raid_guild_id = 0;
			}
		}
	}

	return false;
}

int DungeonManager::GetRaidGuildID( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			return (*it).raid_guild_id;
		}
	}

	return 0;
}


DungeonManager::DungeonManager()
{
}

DungeonManager::~DungeonManager()
{
}

bool DungeonManager::IsSiegeBegin( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			if( (*it).bDungeonSiege )
				return true;

			return false;
		}
	}

	return false;
}

bool DungeonManager::IsEnterableSiegeDungeon( int nDungeonID, int nGuildID )
{
	if( !nGuildID )
		return false;

	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			if( (*it).bDungeonSiege && (*it).owner_guild_id == nGuildID )
				return true;

			if( (*it).bDungeonSiege && (*it).raid_guild_id == nGuildID )
				return true;

			return false;
		}
	}

	return false;
}

const char DungeonManager::IsRestrictedToEnter( const int nDungeonID, const struct StructCreature *pCreature, const bool bAllowLowLevel ) const
{
	// 플레이어와 소환수를 제외하고는 입장 레벨 제한을 받지 않음
	if( !pCreature->IsPlayer() && !pCreature->IsSummon() )
		return 0;

	for( std::vector< _DUNGEON_INFO >::const_iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
			return IsRestrictedToEnter( (*it).level, pCreature->GetLevel() );
	}

	assert( 0 );
	return -1;
}

const char DungeonManager::IsRestrictedToEnter( const int nDungeonLevel, const int nLevel, const bool bAllowLowLevel )
{
	if( !bAllowLowLevel && nLevel < nDungeonLevel - GameRule::DUNGEON_ENTER_LEVEL_LIMIT_LOWER_RANGE )
		return -1;

	// 던전 상한 레벨 체크 안 함
// 	if( nLevel > nDungeonLevel + GameRule::DUNGEON_ENTER_LEVEL_LIMIT_UPPER_RANGE )
// 		return 1;

	return 0;
}

void DungeonManager::onProcess( int nThreadNum )
{
	extern volatile int g_nCurrentLocalFlag;

	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadNum );
	ENV().Set( buf, "DungeonManager" );

	time_t t = time( NULL );

	if( GameRule::bDisableDungeonRaidSiege )
		return;

	std::vector< std::string > vClosedDungeons;
	XStringUtil::Split( ENV().GetString( "game.closed_dungeons", "" ).c_str(), vClosedDungeons, "|" );

	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		for( std::vector< std::string >::iterator itClosed = vClosedDungeons.begin() ; itClosed != vClosedDungeons.end() ; ++itClosed )
		{
			if( atoi( (*itClosed).c_str() ) == (*it).id )
				continue;
		}

		// 던전 중 시즈 타입이 아닐 경우 레이드와 시즈가 없으므로 밑의 처리를 해 줄 필요가 없다.
		if( (*it).type != DUNGEON_TYPE_SIEGE )
			continue;

		time_t dungeon_siege_start_time = GetNextDungeonSiegeStartTime( (*it).start_time, (*it).last_dungeon_siege_finish_time );
		time_t dungeon_siege_end_time = GetNextDungeonSiegeEndTime( (*it).end_time, dungeon_siege_start_time );

		{
			// notice 함수는 DUNGEON_SIEGE_LAYER 에 대해서만 방송을 하지만, wrapUpDungeonRaid는 생성된 레이드 던전 인스턴스 전체에 대해 동작하므로
			// 이 위치에서는 레이어 범위를 한정지을 수 없음. 따라서 전체 레이어에 락을 걸고 처리함. (아놔 니미 ㅠㅜ)
			ARCADIA_LOCK( ArcadiaServer::Instance().LockArea( GetRegionX( (*it).box.GetLeft() ), GetRegionY( (*it).box.GetTop() ),
				GetRegionX( (*it).box.GetRight() ), GetRegionY( (*it).box.GetBottom() ) ) );

			THREAD_SYNCRONIZE( (*it).m_CS );

			if( (*it).vNotices.size() )
			{
				notice( &(*it) );
			}

			if( t > GetDungeonRaidEndTime( (*it).raid_end_time ) && (*it).last_dungeon_raid_wrap_up_time < GetWeekBeginTime() )
			{
				wrapUpDungeonRaid( &(*it) );
			}
		}

		if( (*it).bDungeonOwnerChanged )
		{
			// 던전 소유주가 바뀌었을 경우 해야하는 처리가 현재는 던전 소유 길드원의 지속효과 제거 뿐이므로 일반 던전 레이어(DUNGEON_LAYER)만 락을 건다.
			ARCADIA_LOCK( ArcadiaServer::Instance().LockArea( GetRegionX( (*it).box.GetLeft() ), GetRegionY( (*it).box.GetTop() ),
				GetRegionX( (*it).box.GetRight() ), GetRegionY( (*it).box.GetBottom() ), DUNGEON_LAYER ) );

			THREAD_SYNCRONIZE( (*it).m_CS );

			// 락이 걸린 상태로 정확한 값을 다시 한 번 검사
			if( (*it).bDungeonOwnerChanged )
			{
				onChangeDungeonOwner( &(*it), (*it).prev_owner_guild_id, (*it).original_owner_guild_id );
				(*it).bDungeonOwnerChanged = false;
			}
		}

		if( t > dungeon_siege_start_time )
		{
			if( !(*it).bDungeonSiege && t < dungeon_siege_end_time )
				BeginDungeonSiege( (*it).id );

			// 여긴 전부 던전 시즈 관련 처리/방송이므로 시즈 레이어에 대해서만 락을 걸어도 됨(나중에라도 다른 레이어에 락이 필요하다는 게 확인되었다면 전체 레이어로 변경할 것)
			ARCADIA_LOCK( ArcadiaServer::Instance().LockArea( GetRegionX( (*it).box.GetLeft() ), GetRegionY( (*it).box.GetTop() ),
				GetRegionX( (*it).box.GetRight() ), GetRegionY( (*it).box.GetBottom() ), DUNGEON_SIEGE_LAYER ) );

			THREAD_SYNCRONIZE( (*it).m_CS );

			if( (*it).bNeedToChangePosition )
			{
				changePosition( &(*it) );
				(*it).bNeedToChangePosition = false;

				broadcastSiegeStatus( &(*it) );
			}
			else if( (*it).pConnector || (*it).pDungeonCore )
			{
				broadcastSiegeStatus( &(*it) );
			}

			if( (*it).vNotices.size() )
			{
				notice( &(*it) );
			}

			if( t > dungeon_siege_end_time )
			{
				// 여기 이외에서도 던전 시즈가 끝나는 조건이 있을 수 있으므로 종료 처리를 직접적으로 하지 말고 종료 대기 플래그만 세팅
				if( (*it).bDungeonSiege )
					(*it).bDungeonSiegeNeedToDestroy = true;
			}
			else
			{
#if _MSC_VER <= 1400	// For VC++ 2005(1400) or earlier
				for( std::queue< int >::container_type::const_iterator itRespawn = (*it).qPendedGuardianRespawn.c.begin() ; itRespawn != (*it).qPendedGuardianRespawn.c.end() ; ++itRespawn )
#elif _MSC_VER <= 1600 // For VC++ 2008(1500), 2010(1600)
				for( std::queue< int >::container_type::const_iterator itRespawn = (*it).qPendedGuardianRespawn._Get_container().begin() ; itRespawn != (*it).qPendedGuardianRespawn._Get_container().end() ; ++itRespawn )
#else
#error Please check reference for accessing the internal container class of 'std::queue' and write that code here.
#endif
				{
					RespawnGuardian( &(*it), (*itRespawn) );
				}
				(*it).qPendedGuardianRespawn = std::queue< int >();

				(*it).procRespawnEnvironmentalGuardian();
			}

			// 위에서 플래그를 세팅하는 경우 외에도 시즈 종료 처리를 모두 여기서 함
			if( (*it).bDungeonSiegeNeedToDestroy )
			{
				endDungeonSiege( &(*it) );

				(*it).bDungeonSiegeNeedToDestroy = false; 
			}
		}
		else
		{
			if( t + DUNGEON_SIEGE_NOTICE_START_TIME > dungeon_siege_start_time ) // 던전 시즈 공지 시간대라면 공지 출력하자
			{
				if( (*it).last_global_notice_count < _countof( DUNGEON_SIEGE_NOTICE_TIME ) ) // 일반 공지
				{
					// "(*it).last_global_notice_count" 번째 시간대의 공지사항 출력
					if( ( dungeon_siege_start_time - DUNGEON_SIEGE_NOTICE_TIME[ (*it).last_global_notice_count ] ) < t )
					{
						std::string strText;
						char szBuf[255];

						strText = "@686\v#@dungeon_name@#\v";

						s_sprintf( szBuf, _countof( szBuf ), "@%d", (*it).id + 70000000 );
						strText += szBuf;

						strText += "\v#@num@#\v";

						s_sprintf( szBuf, _countof( szBuf ), "%d", ( dungeon_siege_start_time - t ) / 60 );
						strText += szBuf;

						SendGlobalChatMessage( CHAT_NOTICE, "@NOTICE", strText.c_str(), static_cast<unsigned int>( strText.size() ) );

						(*it).last_global_notice_count = (*it).last_global_notice_count + 1;
					}
				}
			}

			if( t + DUNGEON_SIEGE_GUILD_NOTICE_TIME > dungeon_siege_start_time ) // 5분 남음 양쪽 길드 5분후 입장 가능
			{
				if( !(*it).bDungeonSiegeCreated )
					CreateDungeonSiege( (*it).id );
			}
		}

		// RoamingManager::(InitRoamer/DeInitRoamer) 함수 호출을 (begin/end)DungeonRaid 함수 안에서 호출할 경우 지역락 -> StructRoamer::m_CS 순서에 문제가 생기므로
		// 임시로 보관했다가 지역락을 해제한 이후에 RoamingManager::(InitRoamer/DeInitRoamer) 함수를 호출하도록 함.
		std::set< unsigned char > stRoamerInitPendingLayer;
		std::set< unsigned char > stRoamerDeinitPendingLayer;
		{
			// 여기서는 생성되어 있는 레이드 인스턴스 목록을 미리 확인할 수도 없고, (*it).m_CS 를 건 이후에 지역락을 걸 수도 없는 노릇이므로 전체 레이어에 락을 걸어 줌.
			ARCADIA_LOCK( ArcadiaServer::Instance().LockArea( GetRegionX( (*it).box.GetLeft() ), GetRegionY( (*it).box.GetTop() ),
				GetRegionX( (*it).box.GetRight() ), GetRegionY( (*it).box.GetBottom() ) ) );

			THREAD_SYNCRONIZE( (*it).m_CS );

			bool raid_time_over = false;

			if( t > GetDungeonRaidEndTime( (*it).raid_end_time ) )
			{
				raid_time_over = true;
			}

			for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = (*it).vRaidInfo.begin(); itRaidInfo != (*it).vRaidInfo.end(); ++itRaidInfo )
			{
				_DUNGEON_RAID_INFO * pRaidInfo = (*itRaidInfo);

				if( pRaidInfo->raid_starting )
				{
					beginDungeonRaid( pRaidInfo );
					stRoamerInitPendingLayer.insert( pRaidInfo->layer );
				}

				if( pRaidInfo->raid_complete )
				{
					if( pRaidInfo->layer && pRaidInfo->start_time )
					{
						endDungeonRaid( pRaidInfo, false );
						stRoamerDeinitPendingLayer.insert( pRaidInfo->layer );
					}

					pRaidInfo->raid_complete = false;
				}

				if( pRaidInfo->layer && !pRaidInfo->start_time && pRaidInfo->end_time + DUNGEON_SIEGE_KICK_TIME < t )
				{
					broadcastRaidEnd( pRaidInfo );

					kickPlayerInDungeon( &(*it), pRaidInfo->layer );

					pRaidInfo->layer = 0;
				}

				if( raid_time_over && pRaidInfo->start_time )
				{
					endDungeonRaid( pRaidInfo, true );
					stRoamerDeinitPendingLayer.insert( pRaidInfo->layer );
				}

				if( pRaidInfo->start_time && pRaidInfo->start_time + DUNGEON_RAID_TIME_OUT * 100 < GetArTime() )
				{
					endDungeonRaid( pRaidInfo, true );
					stRoamerDeinitPendingLayer.insert( pRaidInfo->layer );
				}

				if( pRaidInfo->start_time && pRaidInfo->layer && pRaidInfo->last_notice_time + DUNGEON_RAID_NOTICE_PERIOD < t )
				{
					PrintfAttackTeamChatMessageByGuildID( false, CHAT_GUILD_SYSTEM, "@GUILD", pRaidInfo->guild_id, "@682\v#@num@#\v%d", (GetArTime() - pRaidInfo->start_time ) / 6000 );
					pRaidInfo->last_notice_time = t;
				}
			}

			if( !(*it).bDungeonSiegeCreated && !(*it).bDungeonSiegeKicked && (*it).last_dungeon_siege_finish_time + DUNGEON_SIEGE_KICK_TIME < t )
			{
				broadcastSiegeEnd( &(*it) );

				kickPlayerInDungeon( &(*it), DUNGEON_SIEGE_LAYER );
				
				wrapUpDungeonSiege( &(*it) );

				(*it).bDungeonSiegeKicked = true;
			}
		}

		// 락 순서 문제로 위에서 임시 보관한 RoamingManager::(InitRoamer/DeInitRoamer) 함수 호출을 처리
		for( std::set< unsigned char >::const_iterator itRoamerInitLayer = stRoamerInitPendingLayer.begin() ; itRoamerInitLayer != stRoamerInitPendingLayer.end() ; ++itRoamerInitLayer )
		{
			for( std::vector< int >::const_iterator itRoamer = (*it).vRoamerID.begin() ; itRoamer != (*it).vRoamerID.end() ; ++itRoamer )
			{
				RoamingManager::Instance().InitRoamer( (*itRoamer), (*itRoamerInitLayer) );
			}
		}
		for( std::set< unsigned char >::const_iterator itRoamerDeinitLayer = stRoamerDeinitPendingLayer.begin() ; itRoamerDeinitLayer != stRoamerDeinitPendingLayer.end() ; ++itRoamerDeinitLayer )
		{
			for( std::vector< int >::const_iterator itRoamer = (*it).vRoamerID.begin() ; itRoamer != (*it).vRoamerID.end() ; ++itRoamer )
			{
				RoamingManager::Instance().DeInitRoamer( (*itRoamer), (*itRoamerDeinitLayer) );
			}
		}
	}


}

bool DungeonManager::IsDungeonRaidTime( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			// 시즈가 있는 던전이 아니면 체크할 필요가 없다.
			if( (*it).type != DUNGEON_TYPE_SIEGE )
				return false;

			time_t cur_t = time( NULL );

			if( cur_t > GetDungeonRaidEndTime( (*it).raid_end_time ) || cur_t < GetDungeonRaidStartTime( (*it).raid_start_time ) )
				return false;

			return true;
		}
	}

	return false;
}

bool DungeonManager::IsDungeonSiegeAttacteamMakingPeriod( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			// 시즈가 있는 던전이 아니면 체크할 필요가 없다.
			if( (*it).type != DUNGEON_TYPE_SIEGE )
				return false;

			time_t cur_t = time( NULL );

			if( cur_t >= GetDungeonRaidEndTime( (*it).raid_end_time ) && cur_t <= GetNextDungeonSiegeEndTime( (*it).end_time, GetWeekBeginTime() ) )
				return true;

			return false;
		}
	}

	return false;
}

bool DungeonManager::IsEndSiegeOfThisWeek( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			// 시즈가 있는 던전이 아니면 체크할 필요가 없다.
			if( (*it).type != DUNGEON_TYPE_SIEGE )
				return false;

			if( (*it).last_dungeon_siege_finish_time < GetWeekBeginTime() )
				return false;

			return true;
		}
	}

	return false;
}

void DungeonManager::CreateDungeonSiege( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			// 여긴 던전 시즈 관련 처리만 하므로 시즈 레이어만 락을 걸어 줌
			ARCADIA_LOCK( ArcadiaServer::Instance().LockArea( GetRegionX( (*it).box.GetLeft() ), GetRegionY( (*it).box.GetTop() ),
				GetRegionX( (*it).box.GetRight() ), GetRegionY( (*it).box.GetBottom() ), DUNGEON_SIEGE_LAYER ) );

			THREAD_SYNCRONIZE( (*it).m_CS );
			
			if( (*it).bDungeonSiegeCreated )
				return;

			for( std::vector< _TACTICAL_POSITION_INFO >::iterator itTac = (*it).vTacticalPositionInfo.begin(); itTac != (*it).vTacticalPositionInfo.end(); ++itTac )
			{
				(*itTac).own_by_original_owner = true;
			}

			(*it).original_owner_guild_id = (*it).owner_guild_id;

			{
				if( GameContent::IsBlocked( (*it).core_pos.x, (*it).core_pos.y ) )
				{
					_cprint( "DungeonCore could not be respawned at (%f,%f) in a dungeon(%d).\n", (*it).core_pos.x, (*it).core_pos.y, nDungeonID );
					FILELOG( "DungeonCore could not be respawned at (%f,%f) in a dungeon(%d).", (*it).core_pos.x, (*it).core_pos.y, nDungeonID );
				}

#ifndef _DUNGEON_CORE_AS_MONSTER
				GameContent::FIELD_PROP_RESPAWN_INFO core_info( (*it).core_id, (*it).core_pos.x, (*it).core_pos.y, DUNGEON_SIEGE_LAYER, (*it).core_offset_z, (*it).core_around_x, (*it).core_around_y, (*it).core_around_z, (*it).core_scale_x, (*it).core_scale_y, (*it).core_scale_z, (*it).core_is_lock_height, (*it).core_lock_height );

				(*it).pDungeonCore = StructFieldProp::Create( NULL, &core_info, 0 );
#else
				// 몬스터 형태 코어에서는 DungeonResource의 core_id가 몬스터 ID 를 나타냄
				(*it).pDungeonCore = respawnMonster( (*it).core_pos.x, (*it).core_pos.y, DUNGEON_SIEGE_LAYER, (*it).core_id, false, 0, &(*it), false );
				(*it).pDungeonCore->SetDungeonOwnerGuardian();
#endif
			}

			// 몬스터 형태 코어에서는 DungeonResource의 core_id가 몬스터 ID 를 나타냄
			if( GameContent::IsBlocked( (*it).connector_pos.x, (*it).connector_pos.y ) )
			{
				_cprint( "Connector could not be respawned at (%f,%f) in a dungeon(%d).\n", (*it).connector_pos.x, (*it).connector_pos.y, nDungeonID );
				FILELOG( "Connector could not be respawned at (%f,%f) in a dungeon(%d).", (*it).connector_pos.x, (*it).connector_pos.y, nDungeonID );
			}

			(*it).pConnector = respawnMonster( (*it).connector_pos.x, (*it).connector_pos.y, DUNGEON_SIEGE_LAYER, (*it).connector_id, false, 0, &(*it), false );

			if( (*it).pConnector )
			{
				(*it).pConnector->SetDungeonSiegerGuardian();
			}

			for( std::vector< GameContent::MONSTER_RESPAWN_INFO >::iterator itRespawn = (*it).vGuardianRespawnInfo.begin(); itRespawn != (*it).vGuardianRespawnInfo.end(); ++itRespawn )
			{
				for( unsigned int i = 0; i < (*itRespawn).max_num; ++i ) // always max~
				{
					AR_UNIT x = (*itRespawn).left;
					AR_UNIT y = (*itRespawn).top;

					if( GameContent::IsBlocked( x, y ) )
					{
						_cprint( "Guardian monster(%d) could not be respawned at (%f,%f) in a dungeon(%d).\n", (*itRespawn).monster_id, x, y, (*it).id );
						FILELOG( "Guardian monster(%d) could not be respawned at (%f,%f) in a dungeon(%d).", (*itRespawn).monster_id, x, y, (*it).id );
						continue;
					}

					StructMonster * pMob = respawnMonster( x, y, DUNGEON_SIEGE_LAYER, (*itRespawn).monster_id, (*itRespawn).is_wandering, (*itRespawn).way_point_id, &(*it), false );

					if( pMob )
					{
						pMob->SetDungeonOwnerGuardian();

						(*it).vMonsters.push_back( pMob );
					}

#ifdef _DUNGEON_CORE_AS_MONSTER
					if( !(*itRespawn).id )
					{
						(*it).stCoreGuardianHandle.insert( pMob->GetHandle() );
					}
#endif
				}
			}

			for( std::vector< GameContent::MONSTER_RESPAWN_INFO >::iterator itRespawn = (*it).vEnvironmentalGuardianRespawnInfo.begin(); itRespawn != (*it).vEnvironmentalGuardianRespawnInfo.end(); ++itRespawn )
			{
				for( unsigned int i = 0; i < (*itRespawn).max_num; ++i ) // always max~
				{
					AR_UNIT x = (*itRespawn).left;
					AR_UNIT y = (*itRespawn).top;

					if( GameContent::IsBlocked( (*itRespawn).left, (*itRespawn).top ) )
					{
						_cprint( "Environmental guardian monster(%d) could not be respawned at (%f,%f) in a dungeon(%d).\n", (*itRespawn).monster_id, x, y, (*it).id );
						FILELOG( "Environmental guardian monster(%d) could not be respawned at (%f,%f) in a dungeon(%d).", (*itRespawn).monster_id, x, y, (*it).id );
						continue;
					}

					StructMonster * pMob = respawnMonster( x, y, DUNGEON_SIEGE_LAYER, (*itRespawn).monster_id, (*itRespawn).is_wandering, (*itRespawn).way_point_id, &(*it), false );

					if( pMob )
					{
						pMob->SetDungeonOwnerGuardian();

						(*it).vMonsters.push_back( pMob );
					}

					(*it).vRespawnedEnvironmentalGuardianInfo.push_back( _DUNGEON_INFO::RESPAWNED_ENVIRONMENTAL_GUARDIAN_INFO( pMob, &(*itRespawn) ) );
				}
			}

			(*it).bDungeonSiegeCreated = true;
			(*it).bDungeonSiegeKicked = false;

			return;
		}
	}
}

void DungeonManager::BeginDungeonSiege( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			if( !(*it).bDungeonSiegeCreated )
				CreateDungeonSiege( nDungeonID );

			THREAD_SYNCRONIZE( (*it).m_CS );
			
			if( (*it).bDungeonSiege )
				return;

			// Dungeon siege start notice
			PrintfGlobalChatMessage(CHAT_NOTICE, "@NOTICE", "@690\v#@dungeon_name@#\v@%d", (*it).id + 70000000);

			// Defence guild enter notice
			if( (*it).owner_guild_id )
				PrintfGuildChatMessage( CHAT_GUILD, (*it).owner_guild_id, "@688\v#@guild_name@#\v%s", GuildManager::GetInstance().GetGuildName( (*it).owner_guild_id ).c_str() );

			// Offense guild enter notice
			if( (*it).raid_guild_id )
				PrintfGuildChatMessage( CHAT_GUILD, (*it).raid_guild_id, "@689\v#@guild_name@#\v%s", GuildManager::GetInstance().GetGuildName( (*it).raid_guild_id ).c_str() );

			(*it).bDungeonSiege = true;

			// 서버 다운 없이 2회 이상 던전 시즈가 진행될 경우 지난 주의 던전 시즈 종료 시(endDungeonSiege)
			// bNeedToChangePosition 이 true로 세팅된 상태로 종료되므로 시작하자마자 공수교대가 일어나는 불상사를 방지하기 위해 변수 초기화
			(*it).bNeedToChangePosition = false;

			return;
		}
	}
}

void DungeonManager::endDungeonSiege( _DUNGEON_INFO * pDungeonInfo )
{
	if( !pDungeonInfo->bDungeonSiege )
		return;

	pDungeonInfo->bNeedToChangePosition = true;

	if( pDungeonInfo->pDungeonCore )
	{
#ifdef _DUNGEON_CORE_AS_MONSTER
		pDungeonInfo->pDungeonCore->SetDeleteHandler( NULL );
		pDungeonInfo->pDungeonCore->Disable();

		if( pDungeonInfo->pDungeonCore->IsInWorld() )
		{
			RemoveMonsterFromWorld( pDungeonInfo->pDungeonCore );
		}
		ArcadiaServer::Instance().DeleteObject( pDungeonInfo->pDungeonCore );
#else
		if( pDungeonInfo->pDungeonCore->IsInWorld() )
		{
			ArcadiaServer::Instance().RemoveObject( pDungeonInfo->pDungeonCore );
			StructFieldProp::PendFreeFieldProp( pDungeonInfo->pDungeonCore );
		}
#endif
		pDungeonInfo->pDungeonCore = NULL;
	}


	bool bConnectorDestroyed = true;

	if( pDungeonInfo->pConnector )
	{
		if( !pDungeonInfo->pConnector->IsDead() )
		{
			bConnectorDestroyed = false;
		}

		pDungeonInfo->pConnector->SetDeleteHandler( NULL );

		// 더이상의 스케쥴러 요청을 무시
		pDungeonInfo->pConnector->Disable();

		// 월드에서 제거한다.
		if( pDungeonInfo->pConnector->IsInWorld() )
		{
			RemoveMonsterFromWorld( pDungeonInfo->pConnector );
		}

		// object delete 요청
		ArcadiaServer::Instance().DeleteObject( pDungeonInfo->pConnector );

		pDungeonInfo->pConnector = NULL;
	}

	// 리젠 예약 정보 및 리젠 상태 정보 소거
	pDungeonInfo->qPendedGuardianRespawn = std::queue< int >();
	pDungeonInfo->vRespawnedEnvironmentalGuardianInfo.clear();
	pDungeonInfo->stCoreGuardianHandle.clear();

	for( std::vector< StructMonster * >::iterator itMonster = pDungeonInfo->vMonsters.begin(); itMonster != pDungeonInfo->vMonsters.end(); )
	{
		if( (*itMonster)->IsEnable() )
		{
			(*itMonster)->SetDeleteHandler( NULL );

			// 더이상의 스케쥴러 요청을 무시
			(*itMonster)->Disable();

			// 월드에서 제거한다.
			if( (*itMonster)->IsInWorld() )
			{
				RemoveMonsterFromWorld( (*itMonster) );
			}

			// object delete 요청
			ArcadiaServer::Instance().DeleteObject( (*itMonster) );

			itMonster = pDungeonInfo->vMonsters.erase( itMonster );
		}
		else
		{
			++itMonster;
		}
	}

	broadcastSiegeResult( pDungeonInfo );

	std::string strText;

	if( bConnectorDestroyed )
	{
		strText = "@695\v#@dungeon_name@#\v";
	}
	else
	{
		strText = "@696\v#@dungeon_name@#\v";
	}

	char szBuf[256];

	s_sprintf( szBuf, _countof( szBuf ), "@%d", pDungeonInfo->id + 70000000 );
	strText += szBuf;

	pDungeonInfo->vNotices.push_back( strText );

	if( pDungeonInfo->owner_guild_id )
	{
		int nLeadPartyID = PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( pDungeonInfo->owner_guild_id );

		if( nLeadPartyID )
		{
			BroadcastPartyDestroy( nLeadPartyID );

			LOG::Log11N4S( LM_PARTY_DESTROY, 0, 0, nLeadPartyID, 0, 0, 0, 8, 0, 0, 0, 0,
				"", 0, "", 0, PartyManager::GetInstance().GetPartyName( nLeadPartyID ).c_str(), LOG::STR_NTS, "", 0 );

			PartyManager::GetInstance().DestroyParty( nLeadPartyID );
		}
	}

	if( pDungeonInfo->raid_guild_id )
	{
		GuildManager::GetInstance().SetRaidDungeonID( pDungeonInfo->raid_guild_id, 0 );

		int nLeadPartyID = PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( pDungeonInfo->raid_guild_id );

		if( nLeadPartyID )
		{
			BroadcastPartyDestroy( nLeadPartyID );

			LOG::Log11N4S( LM_PARTY_DESTROY, 0, 0, nLeadPartyID, 0, 0, 0, 8, 0, 0, 0, 0,
				"", 0, "", 0, PartyManager::GetInstance().GetPartyName( nLeadPartyID ).c_str(), LOG::STR_NTS, "", 0 );

			PartyManager::GetInstance().DestroyParty( nLeadPartyID );
		}
	}

	pDungeonInfo->bDungeonSiege = false;
	pDungeonInfo->bDungeonSiegeCreated = false;
	pDungeonInfo->last_dungeon_siege_finish_time = time( NULL );
	pDungeonInfo->last_global_notice_count = 0;

	pDungeonInfo->raid_guild_id = 0;
	pDungeonInfo->best_raid_time = 0;

	DB().Push( new DB_UpdateDungeon( pDungeonInfo->id, pDungeonInfo->owner_guild_id, pDungeonInfo->raid_guild_id, pDungeonInfo->best_raid_time, pDungeonInfo->last_dungeon_siege_finish_time, pDungeonInfo->last_dungeon_raid_wrap_up_time, pDungeonInfo->tax_rate ) );

	const char *szRes = "TIMEOUT";

	if( bConnectorDestroyed )
	{
		szRes = "DEAD";
	}

	LOG::Log11N4S( LM_END_DUNGEON_SIEGE, 0, 0, 0, pDungeonInfo->owner_guild_id, pDungeonInfo->raid_guild_id, 0, 0, 0, 0, 0, pDungeonInfo->id,
		GuildManager::GetInstance().GetGuildName( pDungeonInfo->owner_guild_id ).c_str(), LOG::STR_NTS,
		GuildManager::GetInstance().GetGuildName( pDungeonInfo->raid_guild_id ).c_str(), LOG::STR_NTS,
		"", 0,
		szRes, LOG::STR_NTS );

	return;
}

void DungeonManager::AddToDungeonRaidMonster( struct StructMonster * pMonster, int dungeon_id, unsigned char layer )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == dungeon_id )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			for( std::vector< _DUNGEON_RAID_INFO * >::iterator itRaidInfo = (*it).vRaidInfo.begin(); itRaidInfo != (*it).vRaidInfo.end(); ++itRaidInfo )
			{
				if( (*itRaidInfo)->layer == layer )
				{
					pMonster->SetDeleteHandler( (*itRaidInfo) );

					(*itRaidInfo)->vMonsters.push_back( pMonster );

					break;
				}
			}

			break;
		}
	}
}

int DungeonManager::GetConnectorHP( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( nDungeonID != (*it).id )
			continue;

		THREAD_SYNCRONIZE( (*it).m_CS );

		return (*it).pConnector ? (*it).pConnector->GetHPPercentage() : 0;
	}

	assert( 0 );

	return 0;
}

int DungeonManager::GetDungeonCoreHP( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( nDungeonID != (*it).id )
			continue;

		THREAD_SYNCRONIZE( (*it).m_CS );

		return (*it).pDungeonCore ? (*it).pDungeonCore->GetHPPercentage() : 0;
	}

	assert( 0 );

	return 0;
}

void DungeonManager::PendRespawnGuardian( int nDungeonID, int nTacticalPositionID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id != nDungeonID )
			continue;

		THREAD_SYNCRONIZE( (*it).m_CS );

		if( (*it).bDungeonSiege )
			(*it).qPendedGuardianRespawn.push( nTacticalPositionID );

		break;
	}
}

bool DungeonManager::IsOwner( int nDungeonID, int nTacticalPositionID, int nGuildID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			if( nTacticalPositionID == 0 )
			{
				if( nGuildID == (*it).owner_guild_id )
					return true;

				return false;
			}

			for( std::vector< _TACTICAL_POSITION_INFO >::iterator itTac = (*it).vTacticalPositionInfo.begin(); itTac != (*it).vTacticalPositionInfo.end(); ++itTac )
			{
				if( (*itTac).id == nTacticalPositionID )
				{

					if( nGuildID == (*it).original_owner_guild_id )
					{
						if( (*itTac).own_by_original_owner )
							return true;

						return false;
					}
					else
					{
						if( (*itTac).own_by_original_owner )
							return false;

						return true;
					}

					break;
				}
			}

			break;
		}
	}

	return false;
}

bool DungeonManager::IsOwner( int nGuildID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( nGuildID == (*it).owner_guild_id )
			return true;
	}
	return false;
}

#ifdef _DUNGEON_CORE_AS_MONSTER
bool DungeonManager::IsCoreInvincible( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( nDungeonID != (*it).id )
			continue;

		THREAD_SYNCRONIZE( (*it).m_CS );

		return ( (*it).bDungeonSiege && !(*it).stCoreGuardianHandle.empty() );
	}

	assert( 0 );

	return true;
}

void DungeonManager::ClearDungeonSiegeCoreGuardian( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( nDungeonID != (*it).id )
			continue;

		// 시즈 레이어에 있는 코어 가디언들만 처리하므로 시즈 레이어만 락을 걸어 줌
		ARCADIA_LOCK( ArcadiaServer::Instance().LockArea( GetRegionX( (*it).box.GetLeft() ), GetRegionY( (*it).box.GetTop() ),
			GetRegionX( (*it).box.GetRight() ), GetRegionY( (*it).box.GetBottom() ), DUNGEON_SIEGE_LAYER ) );

		THREAD_SYNCRONIZE( (*it).m_CS );

		for( std::set< AR_HANDLE >::const_iterator itGuardian = (*it).stCoreGuardianHandle.begin() ; itGuardian != (*it).stCoreGuardianHandle.end() ; ++itGuardian )
		{
			StructCreature::iterator itCreature = StructCreature::get( (*itGuardian ) );

			if( !(*itCreature) || !(*itCreature)->IsMonster() )
				continue;

			StructMonster *pGuardian = static_cast< StructMonster * >( (*itCreature) );

			if( !pGuardian->IsInWorld() )
			{
				_cprint( "Clearing core guardians of dungeon siege(%d): A guardian not existing in world detected.\n", nDungeonID );
				FILELOG( "Clearing core guardians of dungeon siege(%d): A guardian not existing in world detected.", nDungeonID );

				if( pGuardian->IsEnable() )
					pGuardian->Disable();

				ArcadiaServer::Instance().DeleteObject( pGuardian );
				continue;
			}

			if( !pGuardian->IsInWorld() )
				continue;

			if( pGuardian->IsDead() )
			{
				_cprint( "Clearing core guardians of dungeon siege(%d): A dead guardian existing in world detected.\n", nDungeonID );
				FILELOG( "Clearing core guardians of dungeon siege(%d): A dead guardian existing in world detected.", nDungeonID );

				if( pGuardian->IsEnable() )
					pGuardian->Disable();

				RemoveMonsterFromWorld( pGuardian );
				ArcadiaServer::Instance().DeleteObject( pGuardian );
				continue;
			}

			int nPrevHP = pGuardian->GetHP();
			pGuardian->damage( pGuardian, nPrevHP );
			BroadcastHPMPMsg( pGuardian, -nPrevHP, 0, true );
		}

		(*it).stCoreGuardianHandle.clear();
	}
}
#endif

void DungeonManager::ChangeOwner( int nDungeonID, int nTacticalPositionID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			if( !(*it).bDungeonSiege )
				break;

			int prev_own_guild_id = 0;
			int final_own_guild_id = 0;

			if( nTacticalPositionID == 0 )
			{
				if( (*it).bNeedToChangePosition )
					break;

				int raid_guild_id = (*it).owner_guild_id;

				(*it).owner_guild_id = (*it).raid_guild_id;
				(*it).raid_guild_id = raid_guild_id;

				if( !(*it).raid_guild_id )
					(*it).bDungeonSiegeNeedToDestroy = true;
				else
					(*it).bNeedToChangePosition = true;

				char szBuf[256];

				std::string szMsg;
				szMsg = "@694\v#@guild_name@#\v";
				szMsg += GuildManager::GetInstance().GetGuildName( (*it).owner_guild_id );
				szMsg += "\v#@prop_name@#\v@";

#ifndef _DUNGEON_CORE_AS_MONSTER
				const FieldPropBase *pCoreFieldPropBase = (*it).pDungeonCore->GetFieldPropBase();
				s_sprintf( szBuf, _countof( szBuf ), "%d", pCoreFieldPropBase->nPropTextId );
#else
				const MonsterBase *pCoreMonsterBase = GameContent::GetMonsterInfo( (*it).core_id );
				s_sprintf( szBuf, _countof( szBuf ), "%d", pCoreMonsterBase->name_id );
#endif

				szMsg += szBuf;

				(*it).vNotices.push_back( szMsg );

				prev_own_guild_id = (*it).raid_guild_id;
				final_own_guild_id = (*it).owner_guild_id;
			}
			else
			{

				for( std::vector< _TACTICAL_POSITION_INFO >::iterator itTac = (*it).vTacticalPositionInfo.begin(); itTac != (*it).vTacticalPositionInfo.end(); ++itTac )
				{
					if( (*itTac).id == nTacticalPositionID )
					{
						(*itTac).own_by_original_owner = !(*itTac).own_by_original_owner;

						char szBuf[256];

						int nGuildID = (*it).original_owner_guild_id;

						if( !(*itTac).own_by_original_owner )
						{
							if( (*it).original_owner_guild_id == (*it).owner_guild_id )
							{
								nGuildID = (*it).raid_guild_id;
							}
							else
							{
								nGuildID = (*it).owner_guild_id;
							}

						}
						std::string szMsg;
						szMsg = "@692\v#@guild_name@#\v";
						szMsg += GuildManager::GetInstance().GetGuildName( nGuildID );
						szMsg += "\v#@prop_name@#\v@";

						s_sprintf(szBuf, _countof( szBuf ), "%d", FieldPropManager::GetInstance().GetFieldPropBase( (*itTac).prop_id ).nPropTextId );

						szMsg += szBuf;

						(*it).vNotices.push_back( szMsg );

						prev_own_guild_id = ( nGuildID == (*it).owner_guild_id ) ? (*it).raid_guild_id : (*it).owner_guild_id;
						final_own_guild_id = nGuildID;
						break;
					}
				}
			}

			LOG::Log11N4S( LM_DUNGEON_CHANGE_OWNER, 0, 0, 0, final_own_guild_id, prev_own_guild_id, 0, 0, 0, 0, nTacticalPositionID, nDungeonID,
				GuildManager::GetInstance().GetGuildName( final_own_guild_id ).c_str(), LOG::STR_NTS,
				GuildManager::GetInstance().GetGuildName( prev_own_guild_id ).c_str(), LOG::STR_NTS,
				"", 0,
				"", 0 );
			break;
		}
	}
}

bool DungeonManager::DropDungeonOwnership( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			if( (*it).bDungeonSiege )
				return false;

			if( (*it).bDungeonSiegeCreated )
				return false;

			GuildManager::GetInstance().SetRaidDungeonID( (*it).owner_guild_id, 0 );

			time_t tDungeonBlockTime = time( NULL ) + GameRule::GUILD_DUNGEON_RECHALLENGE_BLOCK_DURATION_SEC;

			int nAllianceID = GuildManager::GetInstance().GetAllianceID( (*it).owner_guild_id );
			if( nAllianceID )
			{
				struct _myGuildListFunctor : public GuildManager::GuildListFunctor
				{
					_myGuildListFunctor( time_t _tDungeonBlockTime )
						: tDungeonBlockTime( _tDungeonBlockTime )
					{}

					virtual bool operator()( int nGuildID, const char * szGuildName, const char * szGuildLeaderName, int nGuildLeaderLevel, int nGuildMemberCount, int nRaidDungeonID )
					{
						return GuildManager::GetInstance().SetDungeonBlockTime( nGuildID, tDungeonBlockTime );
					}

					int nAllianceID;
					time_t tDungeonBlockTime;
				} _fo( tDungeonBlockTime );

				GuildManager::GetInstance().DoEachAllianceGuild( nAllianceID, _fo );
			}
			else
			{
				GuildManager::GetInstance().SetDungeonBlockTime( (*it).owner_guild_id, tDungeonBlockTime );
			}

			(*it).prev_owner_guild_id = (*it).owner_guild_id;
			(*it).owner_guild_id = 0;
			(*it).original_owner_guild_id = 0;

			// 던전 소유권을 포기하여 던전의 소유주가 바뀐다.
			// 이에 따른 처리를 위해 던전 전체에 락이 필요한데 DropDungeonOwnership은 이미 지역락이 걸려있으므로 플래그만 세팅해둔다.
			// 만약 이전에 발생한 소유주 변경에 따른 처리가 되지 않은 상태(bDungeonOwnerChanged == true)에서 다시 한 번 소유주가 바뀌게 되면 문제가 발생할 것이다.
			assert( !(*it).bDungeonOwnerChanged );
			(*it).bDungeonOwnerChanged = true;

			DB().Push( new DB_UpdateDungeon( (*it).id, (*it).owner_guild_id, (*it).raid_guild_id, (*it).best_raid_time, (*it).last_dungeon_siege_finish_time, (*it).last_dungeon_raid_wrap_up_time, (*it).tax_rate ) );

			LOG::Log11N4S( LM_DUNGEON_DROP_OWNERSHIP, 0, 0, 0, (*it).prev_owner_guild_id, 0, 0, 0, 0, 0, 0, nDungeonID, "", 0, "", 0, "", 0, "", 0 );

			break;
		}
	}

	return true;


}

void DungeonManager::EndDungeonSiege( int nDungeonID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			if( !(*it).bDungeonSiege )
				return;

			(*it).bDungeonSiegeNeedToDestroy = true;

			break;
		}
	}			
}

void DungeonManager::broadcastSiegeStatus( _DUNGEON_INFO * pDungeonInfo )
{
	int ownerGuildID = GetOwnGuildID( pDungeonInfo->id );
	int connectorHP = pDungeonInfo->pConnector ? pDungeonInfo->pConnector->GetHPPercentage() : 0;
	int coreHP = pDungeonInfo->pDungeonCore ? pDungeonInfo->pDungeonCore->GetHPPercentage() : 0;

	struct _MessageSender : ArObjectFunctor
	{
		_MessageSender( const int _ownerGuildID, const int _connectorHP, const int _coreHP ) : ownerGuildID( _ownerGuildID ), connectorHP( _connectorHP ), coreHP( _coreHP ) {}

		void operator()( ArObject *pObj ) const
		{
			char buf[256];

			StructPlayer *pPlayer = static_cast< StructPlayer * >( pObj );
			int nGuildID = pPlayer->GetGuildID();
			if( GuildManager::GetInstance().GetAllianceID( nGuildID ) )
			{
				nGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( GuildManager::GetInstance().GetAllianceID( nGuildID ) );
			}

			// 던전을 소유하여 방어자 길드일 경우 1, 그렇지 않아 공격자 길드일 경우 0
			int nPosition = ( nGuildID == ownerGuildID );

			s_sprintf( buf, _countof( buf ), "SIEGE_STATUS|%d|%d|%d|", connectorHP, coreHP, nPosition );

			SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pPlayer, buf );
		}

		int ownerGuildID;
		int connectorHP;
		int coreHP;
	} _sender( ownerGuildID, connectorHP, coreHP );

	ForEachClientInDungeon( pDungeonInfo, DUNGEON_SIEGE_LAYER, _sender );
}

void DungeonManager::broadcastSiegeResult( _DUNGEON_INFO * pDungeonInfo )
{
	char msgToOwner[256];
	char msgToAttacker[256];
	const bool isOwnerChanged = pDungeonInfo->owner_guild_id != pDungeonInfo->original_owner_guild_id;
	std::string ownerGuildName = GuildManager::GetInstance().GetGuildName( pDungeonInfo->owner_guild_id );

	if( isOwnerChanged )
	{
		// 공격 성공
		PrintfGlobalChatMessage( CHAT_NOTICE, "@NOTICE", "@698\v#@dungeon_name@#\v@%d\v#@guild_name@#\v%s",
			pDungeonInfo->id + 70000000, ownerGuildName.c_str() );

		s_sprintf( msgToOwner, _countof( msgToOwner ), "SIEGE_RESULT|FAIL|DEF|%d|%s|", pDungeonInfo->id, ownerGuildName.c_str() );
		s_sprintf( msgToAttacker, _countof( msgToAttacker ), "SIEGE_RESULT|SUCC|ATK|%d|%s|", pDungeonInfo->id, ownerGuildName.c_str() );
	}
	else if( pDungeonInfo->owner_guild_id )
	{
		// 방어 성공
		PrintfGlobalChatMessage( CHAT_NOTICE, "@NOTICE", "@697\v#@dungeon_name@#\v@%d\v#@guild_name@#\v%s",
			pDungeonInfo->id + 70000000, GuildManager::GetInstance().GetGuildName( pDungeonInfo->owner_guild_id ).c_str() );

		s_sprintf( msgToOwner, _countof( msgToOwner ), "SIEGE_RESULT|SUCC|DEF|%d|%s|", pDungeonInfo->id, ownerGuildName.c_str() );
		s_sprintf( msgToAttacker, _countof( msgToAttacker ), "SIEGE_RESULT|FAIL|ATK|%d|%s|", pDungeonInfo->id, ownerGuildName.c_str() );
	}
	else
	{
		// 원 소유주가 없었다면 점령이라는 개념 자체가 성립되지 않으므로 무시
		return;
	}

	struct _MessageSender : ArObjectFunctor
	{
		_MessageSender( const int _nDungeonID, const bool _bIsSuccess, const char * _msgToOwner, const char * _msgToAttacker, const int _originalOwnerGuildID )
			: nDungeonID( _nDungeonID ), bIsSuccess( _bIsSuccess ), msgToOwner( _msgToOwner ), msgToAttacker( _msgToAttacker ), originalOwnerGuildID( _originalOwnerGuildID )
		{}

		void operator()( ArObject *pObj ) const
		{
			StructPlayer* player = static_cast< StructPlayer * >( pObj );

			const bool bIsAttacker = player->GetGuildID() != originalOwnerGuildID;

			SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", player,
				bIsAttacker ? msgToAttacker : msgToOwner );

			// 공격자 입장에서 성공은 방어자 입장에서 실패이다.
			player->UpdateTitleConditionByDungeonSiegeEnd( nDungeonID, bIsAttacker, bIsAttacker ? bIsSuccess : !bIsSuccess );
		}

		const int nDungeonID;
		const bool bIsSuccess;
		const char * msgToOwner;
		const char * msgToAttacker;
		const int originalOwnerGuildID;
	} _sender( pDungeonInfo->id, isOwnerChanged, msgToOwner, msgToAttacker, pDungeonInfo->original_owner_guild_id );
	
	ForEachClientInDungeon( pDungeonInfo, DUNGEON_SIEGE_LAYER, _sender );

	pDungeonInfo->prev_owner_guild_id = pDungeonInfo->original_owner_guild_id;
	pDungeonInfo->original_owner_guild_id = pDungeonInfo->owner_guild_id;

	// 시즈의 결과로 던전의 소유주가 바뀐다.
	// 이에 따른 처리를 위해 던전 전체에 락이 필요한데 broadcastSiegeResult는 시즈 레이어에만 락이 걸려있으므로 플래그만 세팅해둔다.
	// 만약 이전에 발생한 소유주 변경에 따른 처리가 되지 않은 상태(bDungeonOwnerChanged == true)에서 다시 한 번 소유주가 바뀌게 되면 문제가 발생할 것이다.
	assert( !pDungeonInfo->bDungeonOwnerChanged );
	pDungeonInfo->bDungeonOwnerChanged = true;
}

void DungeonManager::broadcastSiegeEnd( _DUNGEON_INFO * pDungeonInfo )
{
	struct _MessageSender : ArObjectFunctor
	{
		void operator()( ArObject *pObj ) const
		{
			SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", static_cast< StructPlayer * >( pObj ), "SIEGE_END|" );
		}
	} _sender;
	
	ForEachClientInDungeon( pDungeonInfo, DUNGEON_SIEGE_LAYER, _sender );
}


void DungeonManager::broadcastRaidEnd( _DUNGEON_RAID_INFO * pRaidInfo )
{
	struct _MessageSender : ArObjectFunctor
	{
		void operator()( ArObject *pObj ) const
		{
			SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", static_cast< StructPlayer * >( pObj ), "RAID_END|" );
		}
	} _sender;
	
	ForEachClientInDungeon( pRaidInfo->pDungeonInfo, pRaidInfo->layer, _sender );
}

void DungeonManager::broadcastRaidResult( _DUNGEON_RAID_INFO * pRaidInfo, const bool success )
{
	char buf[256];

	if( success )
	{
		int hour, min, sec;
		AR_TIME total_record = pRaidInfo->raid_record / 100;

		hour = total_record / 3600;

		total_record -= hour * 3600;

		min = total_record / 60;

		total_record -= min * 60;

		sec = total_record;

		int nAllianceID = GuildManager::GetInstance().GetAllianceID( pRaidInfo->guild_id );
		std::string guildName = GuildManager::GetInstance().GetGuildName( pRaidInfo->guild_id );

		if( nAllianceID )
		{
			PrintfAllianceChatMessage( CHAT_GUILD, nAllianceID, "@GUILD",
				"@684\v#@guild_name@#\v%s\v#@dungeon_name@#\v@%d\v#@hour@#\v%d\v#@minute@#\v%d\v#@second@#\v%d",
				guildName.c_str(), pRaidInfo->pDungeonInfo->id + 70000000, hour, min, sec);
		}
		else
		{
			PrintfGuildChatMessage( CHAT_GUILD, pRaidInfo->guild_id,
				"@684\v#@guild_name@#\v%s\v#@dungeon_name@#\v@%d\v#@hour@#\v%d\v#@minute@#\v%d\v#@second@#\v%d",
				guildName.c_str(), pRaidInfo->pDungeonInfo->id + 70000000, hour, min, sec);
		}

		s_sprintf( buf, _countof( buf ), "RAID_RESULT|SUCC|%d|%d|%d|", hour, min, sec );
	}
	else
	{
		SendGuildChatMessage( CHAT_GUILD, "@GUILD", pRaidInfo->guild_id, "@683" );

		s_sprintf( buf, _countof( buf ), "RAID_RESULT|FAIL|" );
	}

	struct _MessageSender : ArObjectFunctor
	{
		_MessageSender( const char * _msg ) : msg( _msg ) {}

		void operator()( ArObject *pObj ) const
		{
			SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", static_cast< StructPlayer * >( pObj ), msg );
		}

		const char * msg;
	} _sender( buf );
	
	ForEachClientInDungeon( pRaidInfo->pDungeonInfo, pRaidInfo->layer, _sender );
}

void DungeonManager::notice( _DUNGEON_INFO * pDungeonInfo )
{
	struct _MessageSender : ArObjectFunctor
	{
		_MessageSender( std::vector< std::string > *  _pvNotices ) { pvNotices = _pvNotices; }

		void operator()( ArObject *pObj ) const
		{
			for( std::vector< std::string >::iterator it = pvNotices->begin(); it != pvNotices->end(); ++it )
				SendChatMessage( false, CHAT_NOTICE, "@NOTICE", static_cast< StructPlayer * >( pObj ), (*it).c_str(), static_cast<unsigned int>( (*it).size() ) );
		}

		std::vector< std::string > * pvNotices;

	} _sender( &( pDungeonInfo->vNotices ) );

	ForEachClientInDungeon( pDungeonInfo, DUNGEON_SIEGE_LAYER, _sender );

	pDungeonInfo->vNotices.clear();
}

void DungeonManager::ForEachClientInDungeon( _DUNGEON_INFO * pDungeonInfo, unsigned char layer, ArObjectFunctor& functor )
{
	struct Sender : ArRegionFunctor
	{
		Sender( ArObjectFunctor& _functor ) : functor( _functor ) {}

		void operator()( const struct ArRegion * pRegion )
		{
			pRegion->DoEachClient( functor );
		}

		ArObjectFunctor& functor;
	} _fo( functor );

	assert( ArcadiaServer::Instance().IsLocked( GetRegionX( pDungeonInfo->box.GetLeft() ), GetRegionY( pDungeonInfo->box.GetTop() ),
		GetRegionX( pDungeonInfo->box.GetRight() ), GetRegionY( pDungeonInfo->box.GetBottom() ), layer ) );

	ArcadiaServer::Instance().DoEachRegion( GetRegionX( pDungeonInfo->box.GetLeft() ), GetRegionY( pDungeonInfo->box.GetTop() ),
		GetRegionX( pDungeonInfo->box.GetRight() ), GetRegionY( pDungeonInfo->box.GetBottom() ), layer, _fo );
}

void DungeonManager::changePosition( _DUNGEON_INFO * pDungeonInfo )
{
	std::vector< AR_HANDLE > vResult;

	ArcadiaServer::Instance().EnumMovableObjectInEveryRegion( GetRegionX( pDungeonInfo->box.GetLeft() ), GetRegionY( pDungeonInfo->box.GetTop() ),
		GetRegionX( pDungeonInfo->box.GetRight() ), GetRegionY( pDungeonInfo->box.GetBottom() ), DUNGEON_SIEGE_LAYER, &vResult, true, false );

	for( std::vector< AR_HANDLE >::iterator itObject = vResult.begin(); itObject != vResult.end(); ++itObject )
	{
		StructCreature::iterator itPlayer = StructCreature::get( (*itObject) );

		if( (*itPlayer)->IsPlayer() )
		{
			StructPlayer * pPlayer = static_cast< StructPlayer * >( *itPlayer );

			// 현재 던전의 시즈 참여자인지 검사(파티, 길드 ID가 존재하고,
			// 공대 리드 파티 ID가 주인 또는 레이드 길드의 공대 리드 파티 ID 중 하나인지 검사
			if( !pPlayer->IsInParty() || !pPlayer->IsInGuild() )
				continue;

			int nAttackTeamLeadPartyID = PartyManager::GetInstance().GetLinkedPartyLeadPartyID( pPlayer->GetPartyID() );
			if( nAttackTeamLeadPartyID != PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( pDungeonInfo->owner_guild_id ) &&
				nAttackTeamLeadPartyID != PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( pDungeonInfo->raid_guild_id ) )
			{
				continue;
			}

			if( PartyManager::GetInstance().GetAttackTeamGuildID( pPlayer->GetPartyID() ) == pDungeonInfo->owner_guild_id )
			{
				pPlayer->PendWarp( pDungeonInfo->siege_defence_pos.x, pDungeonInfo->siege_defence_pos.y, DUNGEON_SIEGE_LAYER );
				ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
			}
			else
			{
				pPlayer->PendWarp( pDungeonInfo->siege_start_pos.x, pDungeonInfo->siege_start_pos.y, DUNGEON_SIEGE_LAYER );
				ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
			}
		}
	}

	if( pDungeonInfo->pConnector )
	{
		pDungeonInfo->pConnector->SetHP( pDungeonInfo->pConnector->GetMaxHP() );
		pDungeonInfo->pConnector->SetMP( pDungeonInfo->pConnector->GetMaxMP() );

		if( pDungeonInfo->original_owner_guild_id == pDungeonInfo->owner_guild_id )
		{
			pDungeonInfo->pConnector->SetDungeonSiegerGuardian();
			pDungeonInfo->pConnector->ResetDungeonOwnerGuardian();
		}
		else
		{
			pDungeonInfo->pConnector->ResetDungeonSiegerGuardian();
			pDungeonInfo->pConnector->SetDungeonOwnerGuardian();
		}

		BroadcastStatusMessage( pDungeonInfo->pConnector );
	}

#ifdef _DUNGEON_CORE_AS_MONSTER
	if( pDungeonInfo->pDungeonCore )
	{
		pDungeonInfo->pDungeonCore->SetHP( pDungeonInfo->pDungeonCore->GetMaxHP() );
		pDungeonInfo->pDungeonCore->SetMP( pDungeonInfo->pDungeonCore->GetMaxMP() );
	}
	else
	{
		if( GameContent::IsBlocked( pDungeonInfo->core_pos.x, pDungeonInfo->core_pos.y ) )
		{
			_cprint( "changePosition() DungeonCore could not be respawned at (%f,%f).\n", pDungeonInfo->core_pos.x, pDungeonInfo->core_pos.y );
			FILELOG( "changePosition() DungeonCore could not be respawned at (%f,%f).", pDungeonInfo->core_pos.x, pDungeonInfo->core_pos.y );
		}

		pDungeonInfo->pDungeonCore = respawnMonster( pDungeonInfo->core_pos.x, pDungeonInfo->core_pos.y, DUNGEON_SIEGE_LAYER, pDungeonInfo->core_id, false, 0, pDungeonInfo, false );
	}

	if( pDungeonInfo->original_owner_guild_id == pDungeonInfo->owner_guild_id )
	{
		pDungeonInfo->pDungeonCore->SetDungeonOwnerGuardian();
		pDungeonInfo->pDungeonCore->ResetDungeonSiegerGuardian();
	}
	else
	{
		pDungeonInfo->pDungeonCore->SetDungeonSiegerGuardian();
		pDungeonInfo->pDungeonCore->ResetDungeonOwnerGuardian();
	}

	BroadcastStatusMessage( pDungeonInfo->pDungeonCore );

	// 코어 주변의 가디언 리젠(프랍 코어일 때에는 코어 스크립트에서 호출해 줌)
	DungeonManager::Instance().RespawnGuardian( pDungeonInfo, 0 );
#endif
}

void DungeonManager::RespawnGuardian( struct _DUNGEON_INFO * pDungeonInfo, int nTacticalPositionID )
{
	bool bOwnerGuardian = false;

	if( nTacticalPositionID == 0 )
	{
		if( pDungeonInfo->original_owner_guild_id == pDungeonInfo->owner_guild_id )
		{
			bOwnerGuardian = true;
		}
	}
	else
	{
		for( std::vector< _TACTICAL_POSITION_INFO >::iterator itTac = pDungeonInfo->vTacticalPositionInfo.begin(); itTac != pDungeonInfo->vTacticalPositionInfo.end(); ++itTac )
		{
			if( (*itTac).id == nTacticalPositionID )
			{
				if( (*itTac).own_by_original_owner )
				{
					bOwnerGuardian = true;
				}
				
				break;
			}
		}
	}

	for( std::vector< GameContent::MONSTER_RESPAWN_INFO >::iterator itRespawn = pDungeonInfo->vGuardianRespawnInfo.begin(); itRespawn != pDungeonInfo->vGuardianRespawnInfo.end(); ++itRespawn )
	{
		for( unsigned int i = 0; i < (*itRespawn).max_num; ++i ) // always max~
		{
			ArPosition pos( (*itRespawn).left, (*itRespawn).top );

			if( (*itRespawn).id != nTacticalPositionID )
				break;

			AR_UNIT x, y;
			unsigned try_cnt = 0;

			bool bValidPos = true;

			do
			{
				x = XRandom( (*itRespawn).left, (*itRespawn).right );
				y = XRandom( (*itRespawn).top, (*itRespawn).bottom );

				if( ++try_cnt > 300 )
				{
					bValidPos = false;
					break;
				}
			} while( GameContent::IsBlocked( x, y ) );

			if( bValidPos )
			{
				StructMonster * pMob = respawnMonster( x, y, DUNGEON_SIEGE_LAYER, (*itRespawn).monster_id, (*itRespawn).is_wandering, (*itRespawn).way_point_id, pDungeonInfo, false );

				if( pMob )
				{
					if( bOwnerGuardian )
					{
						pMob->SetDungeonOwnerGuardian();
					}
					else
					{
						pMob->SetDungeonSiegerGuardian();
					}

					pDungeonInfo->vMonsters.push_back( pMob );

					BroadcastStatusMessage( pMob );

#ifdef _DUNGEON_CORE_AS_MONSTER
					if( nTacticalPositionID == 0 )
					{
						pDungeonInfo->stCoreGuardianHandle.insert( pMob->GetHandle() );
					}
#endif
				}
			}
		}
	}

	for( std::vector< _DUNGEON_INFO::RESPAWNED_ENVIRONMENTAL_GUARDIAN_INFO >::iterator it = pDungeonInfo->vRespawnedEnvironmentalGuardianInfo.begin() ; it != pDungeonInfo->vRespawnedEnvironmentalGuardianInfo.end() ; ++it )
	{
		StructMonster * pEnvironmentalGuardian = (*it).pMonster;

		if( (*it).pRespawnInfo->id != nTacticalPositionID || !pEnvironmentalGuardian )
			continue;

		if( bOwnerGuardian )
		{
			pEnvironmentalGuardian->ResetDungeonSiegerGuardian();
			pEnvironmentalGuardian->SetDungeonOwnerGuardian();
		}
		else
		{
			pEnvironmentalGuardian->ResetDungeonOwnerGuardian();
			pEnvironmentalGuardian->SetDungeonSiegerGuardian();
		}

		BroadcastStatusMessage( pEnvironmentalGuardian );
	}
}

void DungeonManager::kickPlayerInDungeon( struct _DUNGEON_INFO * pDungeonInfo, unsigned char layer )
{
	std::vector< AR_HANDLE > vResult;

	ArcadiaServer::Instance().EnumMovableObjectInEveryRegion( GetRegionX( pDungeonInfo->box.GetLeft() ), GetRegionY( pDungeonInfo->box.GetTop() ),
		GetRegionX( pDungeonInfo->box.GetRight() ), GetRegionY( pDungeonInfo->box.GetBottom() ), layer, &vResult, true, false );

	for( std::vector< AR_HANDLE >::iterator itObject = vResult.begin(); itObject != vResult.end(); ++itObject )
	{
		StructCreature::iterator itPlayer = StructCreature::get( (*itObject) );

		if( (*itPlayer)->IsPlayer() )
		{
			StructPlayer * pPlayer = static_cast< StructPlayer * >(*itPlayer);
			ArPosition townPosition;
			pPlayer->GetLastTownPosition( &townPosition );

			layer = 0;

			ArPosition pos = pPlayer->GetPos();

			int current_channel = ChannelManager::GetChannelId( pos.x, pos.y );
			int target_channel = ChannelManager::GetChannelId( townPosition.x, townPosition.y );

			int dungeon_id = pDungeonInfo->id;

			// 수련자의 섬 같이 유저수 제한 채널로 워프해야할 경우
			if( target_channel && ChannelManager::GetChannelType( target_channel ) == ChannelManager::TYPE_USER_LIMIT )
			{
				if( current_channel == target_channel )
				{
					// 대상 동네가 출발 동네면 현재 레이어 사용
					layer = pPlayer->GetLayer();
				}
				else
				{
					// 아니면 새로 받고...
					layer = ChannelManager::GetProperLayer( townPosition.x, townPosition.y );
				}
			}

			pPlayer->PendWarp( townPosition.x, townPosition.y, layer );
			ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
		}
	}
}

void DungeonManager::SetTaxRate( int nDungeonID, int nTaxRate )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			(*it).tax_rate = nTaxRate;

			DB().Push( new DB_UpdateDungeon( (*it).id, (*it).owner_guild_id, (*it).raid_guild_id, (*it).best_raid_time, (*it).last_dungeon_siege_finish_time, (*it).last_dungeon_raid_wrap_up_time, (*it).tax_rate ) );
		
			break;
		}
	}
}

void DungeonManager::GetTaxRate( int nDungeonID, int * nTaxRate, int * nOwnGuildID )
{
	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			*nTaxRate = (*it).tax_rate;
			*nOwnGuildID = (*it).owner_guild_id;

			return;
		}
	}
}

ArPosition DungeonManager::GetNearestTacticalPosition( int nDungeonID, int nGuildID, ArPosition curPos )
{
	ArPosition NearestPosition;

	AR_UNIT	distance = 0.0f;

	for( std::vector< _DUNGEON_INFO >::iterator it = m_vDungeonInfo.begin(); it != m_vDungeonInfo.end(); ++it )
	{
		if( (*it).id == nDungeonID )
		{
			THREAD_SYNCRONIZE( (*it).m_CS );

			if( !(*it).bDungeonSiege )
				break;

			if( (*it).owner_guild_id == nGuildID )
			{
				distance = (*it).siege_defence_pos.GetDistance( curPos );
				NearestPosition = (*it).siege_defence_pos;
			}
			else if( (*it).raid_guild_id == nGuildID )
			{
				distance = (*it).siege_start_pos.GetDistance( curPos );
				NearestPosition = (*it).siege_start_pos;
			}
			else
			{
				// 이새끼 뭐야?
				break;
			}

			bool bOriginalOwner = nGuildID == (*it).original_owner_guild_id;

			for( std::vector< _TACTICAL_POSITION_INFO >::iterator itTac = (*it).vTacticalPositionInfo.begin(); itTac != (*it).vTacticalPositionInfo.end(); ++itTac )
			{
				if( ( (*itTac).own_by_original_owner && bOriginalOwner ) || ( !(*itTac).own_by_original_owner && !bOriginalOwner ) )
				{
					if( (*itTac).pos.GetDistance( curPos ) < distance )
					{
						NearestPosition = (*itTac).pos;
						distance = (*itTac).pos.GetDistance( curPos );
					}
				}
			}

			// 던전 시즈 중 사망시 가장 가까운 전략 거점보다 코어와 더 가까울 경우 방어지점을 선택
			if( nGuildID == (*it).owner_guild_id && (*it).core_pos.GetDistance( curPos ) < distance )
			{
				NearestPosition = (*it).siege_defence_pos;
				// 거리는 코어와의 거리라고 뻥을 침. 이후에 다른 처리가 추가되면 코어보다도 가까워야 선택되도록 하기 위함.
				distance = NearestPosition.GetDistance( (*it).core_pos );
			}

			break;
		}
	}

	return NearestPosition;
}
