#pragma once

#include <vector>

#include <geometry/X2DBasicTypes.h>
#include <mmo/ArSchedulerObject.h>
#include <toolkit/ILock.h>

#include "StructMonster.h"
#include "GameContent.h"


#define _DUNGEON_CORE_AS_MONSTER 1

// dungeon_name_text_id : dungeon_id + 70000000
struct DungeonManager : ArSchedulerObject
{
	enum
	{
		DUNGEON_LAYER				= 0,
		DUNGEON_SIEGE_LAYER			= 1,
		CUSTOMIZE_DUNGEON_LAYER		= 255,
	};

	// DUNGEON_INFO에 사용 될 던전 타입
	enum
	{
		DUNGEON_TYPE_SIEGE			= 0,
		DUNGEON_TYPE_NORMAL			= 1,
		DUNGEON_TYPE_UNKNOWN
	};

	enum
	{
		STATUS_IDLE					= 0,
		STATUS_RAID					= 1,
		STATUS_REST_BEFORE_SIEGE	= 2,
		STATUS_SIEGE				= 3,
		STATUS_REST_AFTER_SIEGE		= 4,
	};

	bool						Init();
	bool						DeInit();

	static DungeonManager &		Instance();

	static time_t 				GetDungeonRaidStartTime( int start_time );
	static time_t 				GetDungeonRaidEndTime( int end_time );
	static time_t 				GetNextDungeonSiegeStartTime( int start_time, time_t last_dungeon_siege_finish_time );
	static time_t 				GetNextDungeonSiegeEndTime( int end_time, time_t dungeon_siege_start_time );

	virtual void				onProcess( int nThreadIdx );

	ArPosition					GetRaidStartPosition( int nDungeonID );
	ArPosition					GetSiegeStartPosition( int nDungeonID );
	ArPosition					GetSiegeDefencePosition( int nDungeonID );

	int							GetOriginalOwnGuildID( int nDungeonID );
	int							GetOwnGuildID( int nDungeonID );
	bool						SetOwnGuildID( const int & nDungeonID, const int & nGuildID );
	bool						ClearRaidGuildID( const int & nDungeonID );
	int							GetRaidGuildID( int nDungeonID );
	int							GetDungeonID( AR_UNIT x, AR_UNIT y ) const;
	int							GetDungeonLevel( int nDungeonID );
	int							GetMaxRaidParty( int nDungeonID );
	int							GetMaxGuildParty( int nDungeonID );

	bool						BeginDungeonRaid( int nDungeonID, int nGuildID );

	unsigned char				GetRaidDungeonLayer( int nDungeonID, int nGuildID );
	// 던전의 시간 정보
	bool						IsRaidBegin( int nDungeonID, int nGuildID );
	bool						IsDungeonRaidTime( int nDungeonID );
	bool						IsDungeonSiegeAttacteamMakingPeriod( int nDungeonID );
	bool						IsEndSiegeOfThisWeek( int nDungeonID );

	void						CreateDungeonSiege( int nDungeonID );
	void						BeginDungeonSiege( int nDungeonID );
	void						EndDungeonSiege( int nDungeonID );
	bool						DropDungeonOwnership( int nDungeonID );
	bool						IsOwner( int nDungeonID, int nTacticalPositionID, int nGuildID );
	bool						IsOwner( int nGuildID );
#ifdef _DUNGEON_CORE_AS_MONSTER
	bool						IsCoreInvincible( int nDungeonID );
	void						ClearDungeonSiegeCoreGuardian( int nDungeonID );
#endif
	void						ChangeOwner( int nDungeonID, int nTacticalPositionID );
	void						PendRespawnGuardian( int nDungeonID, int nTacticalPositionID );
	void						SetTaxRate( int nDungeonID, int nTaxRate );
	void						GetTaxRate( int nDungeonID, int * nTaxRate, int * nOwnGuildID );
	const AR_TIME				GetDungeonRaidRecord( int nDungeonID, int nGuildID ) const;
	void						GetTopDungeonRaidRecord( const int nDungeonID, int & pnGuildID, AR_TIME & pnRecord ) const;
	void						ClearDungeonRaidRecord( int nDungeonID, int nGuildID );
	time_t						GetLastRaidEndTime( int nDungeonID, int nGuildID );
	AR_TIME						GetElaspedRaidTime( int nDungeonID, unsigned char layer );
	void						AddToDungeonRaidMonster( struct StructMonster * pMonster, int dungeon_id, unsigned char layer );
	int							GetConnectorHP( int nDungeonID );
	int							GetDungeonCoreHP( int nDungeonID );

	ArPosition					GetNearestTacticalPosition( int nDungeonID, int nGuildID, ArPosition curPos );

	bool						IsSiegeBegin( int nDungeonID );
	bool						IsEnterableSiegeDungeon( int nDungeonID, int nGuildID );

	// 지정한 던전 입장 가능 최저 레벨보다 낮을 경우 0보다 작은 값, 최고 레벨보다 높을 경우 0보다 큰 값, 입장 가능할 경우 0 리턴(잘못된 던전 ID에 대해서는 무조건 0보다 작은 값 리턴)
	const char					IsRestrictedToEnter( const int nDungeonID, const struct StructCreature *pCreature, const bool bAllowLowLevel = false ) const;
	static const char			IsRestrictedToEnter( const int nDungeonLevel, const int nLevel, const bool bAllowLowLevel = false );

	void						RegisterDungeonInfo( int id, int level, int type, AR_UNIT raid_start_x, AR_UNIT raid_start_y, AR_UNIT siege_start_x, AR_UNIT siege_start_y, AR_UNIT siege_defence_x, AR_UNIT siege_defence_y, int connector_id, AR_UNIT connector_x, AR_UNIT connector_y, int core_id, AR_UNIT core_x, AR_UNIT core_y, float core_offset_z, float core_around_x, float core_around_y, float core_around_z, float scale_x, float scale_y, float scale_z, bool core_is_lock_height, float core_lock_height, int boss01_id, int boss02_id, int raid_start_time, int raid_end_time, int start_time, int end_time, X2D::Box< AR_UNIT > box, int owner_guild_id, int raid_guild_id, int best_raid_time, int last_dungeon_siege_finish_time, int last_dungeon_raid_wrap_up_time, int tax_rate, int max_guild_party, int max_raid_party );
	void						RegisterDungeonMonsterRespawnInfo( int nDungeonID, const GameContent::MONSTER_RESPAWN_INFO & info );
	void						RegisterDungeonGuardianRespawnInfo( int nDungeonID, const GameContent::MONSTER_RESPAWN_INFO & info );
	void						RegisterDungeonEnvironmentalGuardianRespawnInfo( int nDungeonID, const GameContent::MONSTER_RESPAWN_INFO & info );
	void						RegisterDungeonRoamerInfo( int nDungeonID, const int nRoamingID );
	void						RegisterDungeonRaidInfo( int dungeon_id, int guild_id, AR_TIME record, time_t raid_end_time );
	void						RegisterRandomDungeonMonsterRespawnInfo( int nDungeonID, const GameContent::RANDOM_MONSTER_RESPAWN_INFO & info );
	void						RegisterTacticalPositionInfo( int dungeon_id, int id, AR_UNIT x, AR_UNIT y, int prop_id );

	void						OnEnterDungeon( const int nDungeonID, struct StructPlayer *pPlayer, const unsigned char nLayer );
	void						OnExitDungeon( const int nDungeonID, struct StructPlayer *pPlayer, int nGuildID, const unsigned char nLayer );

	void						LoadAllRaidInfo();
	~DungeonManager();

protected:
	DungeonManager();

	void						LoadRaidInfo( int dungeon_id, int last_dungeon_siege_time );

	bool						beginDungeonRaid( struct _DUNGEON_RAID_INFO * pRaidInfo );
	bool						endDungeonRaid( struct _DUNGEON_RAID_INFO * pRaidInfo, bool bCancel );

	void						RespawnGuardian( struct _DUNGEON_INFO * pDungeonInfo, int nTacticalPositionID );

	void						kickPlayerInDungeon( struct _DUNGEON_INFO * pDungeonInfo, unsigned char layer );

	void						endDungeonSiege( _DUNGEON_INFO * pDungeonInfo );

	void						changePosition( _DUNGEON_INFO * pDungeonInfo );

	const AR_TIME				getDungeonRaidRecord( const _DUNGEON_INFO * pDungeonInfo, const int nGuildID ) const;
	void						getTopDungeonRaidRecord( const _DUNGEON_INFO * pDungeonInfo, int & nGuildID, AR_TIME & nRecord ) const;
	void						clearDungeonRaidRecord( _DUNGEON_INFO *pDungeonInfo, int nGuildID );

	void						notice( _DUNGEON_INFO * pDungeonInfo );
	void						broadcastSiegeStatus( _DUNGEON_INFO * pDungeonInfo );
	void						broadcastSiegeEnd( _DUNGEON_INFO * pDungeonInfo );
	void						broadcastSiegeResult( _DUNGEON_INFO * pDungeonInfo );
	void						broadcastRaidEnd( _DUNGEON_RAID_INFO * pRaidInfo );
	void						broadcastRaidResult( _DUNGEON_RAID_INFO * pRaidInfo, const bool success );

	void						ForEachClientInDungeon( _DUNGEON_INFO * pDungeonInfo, unsigned char layer, ArObjectFunctor& functor );

	void						wrapUpDungeonRaid( _DUNGEON_INFO * pDungeonInfo );
	void						wrapUpDungeonSiege( _DUNGEON_INFO * pDungeonInfo );

	void						onChangeDungeonOwner( _DUNGEON_INFO * pDungeonInfo, const int nPrevOwnerID, const int nOwnerID );

protected:
	std::vector< struct _DUNGEON_INFO >	m_vDungeonInfo;
};
