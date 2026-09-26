#pragma once

#include <oledb.h>
#include <vector>

#include <toolkit/XBossWorker.h>

#include "ADOConnection.h"
#include "GameContent.h"
#include "StructItem.h"


struct StringLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct MixLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct SkillLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct NPCLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct DungeonLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct FieldPropLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct FieldPropSwitchingLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct MonsterLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct ItemLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct ETCLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct FieldPropDataLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel );
};

struct MapLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel );

	bool InitMapInfo();
};

struct EventAreaHandlerLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel );
};

struct BitmapLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel );
};

struct QuestLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

struct ChannelLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

struct ScheduledCommandLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

struct AuctionCategoryLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

// Telecaster.dbo.Auction, Telecaster.dbo.ItemKeeping 로더(Telecaster.dbo.Item 테이블 참조함)
struct AuctionLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) { clearLoadedItem(); }

private:
	struct dbAuctionAndKeepingItem;
	struct dbAuction;
	struct dbItemKeeping;

private:
	bool loadAuctionAndKeepingSummonList( _ConnectionPtr & ConnPtr );
	bool loadAuctionAndKeepingPetList( _ConnectionPtr & ConnPtr );
	bool _loadSummonSkillList( _ConnectionPtr & ConnPtr, struct StructSummon * pSummon );
	bool loadAuctionAndKeepingItem( _ConnectionPtr & ConnPtr );
	bool _loadRandomOptionList( _ConnectionPtr & ConnPtr );
	void _getRandomOption( int _SID, struct ItemInstance::RANDOM_OPTION * _pRandomOption );
	bool loadAuctionInfo( _ConnectionPtr & ConnPtr );
	bool loadItemKeepingInfo( _ConnectionPtr & ConnPtr );
	bool onAuctionInfoData( dbAuction * emprs );
	bool onItemKeepingInfoData( dbItemKeeping * emprs );

	// 아이템 정보를 먼저 로딩하고 경매 정보를 로딩하기 위해 아이템 정보 임시 보관
	bool pushLoadedItem( struct StructItem *pItem );
	struct StructItem * popLoadedItem( __int64 nItemUID );
	void clearLoadedItem();
	void clearLoadedSummonAndPet();
	void clearLoadedRandomOption();

	std::vector< struct StructSummon * >	m_vLoadedSummonList;
	std::vector< struct StructPet * >		m_vLoadedPetList;
	std::vector< struct StructItem * >		m_vLoadedItemList;
	std::vector< struct ItemInstance::RANDOM_OPTION > m_vRandomOptionList;
};

// Arcadia.dbo.AutoAuctionResource 로더
// ItemResource 로딩 완료 후 사용해야 하며, AuctionManager가 DeInit 된 상태(priority == UPDATE_PRIORITY_IDLE)여야 함
struct AutoAuctionLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

// Telecaster.dbo.AutoAuctionRegistrationInfo 테이블 전체 로딩
// AutoAuctionLoader 로딩 완료 후 사용해야 하며, AuctionManager가 DeInit 된 상태(priority == UPDATE_PRIORITY_IDLE)여야 함
struct AutoAuctionRegistrationInfoLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
private:
	bool loadAutoAuctionRegistrationInfo();
};

// Arcadia.dbo.PetResource 로더
struct PetLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

// Arcadia.dbo.RoamingResource, Arcadia.dbo.RoamingPointResource 로더
struct RoamingLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

// Arcadia.dbo.HuntaholicResource, Arcadia.dbo.HuntaholicInstanceResource, Arcadia.dbo.HuntaholicMonsterRespawnResource 로더
// ChannelLoader 로딩 완료 후 사용해야 하며, HuntaholicManager가 DeInit 된 상태(priority == UPDATE_PRIORITY_IDLE)여야 함
struct HuntaholicLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

struct HuntaholicRankingLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

// Telecaster.dbo.RankingInfo, Telecaster.dbo.RankingScore 로더
struct RankingLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

struct DeathmatchLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct CreatureEnhanceLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct CreatureFarmLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );
	
	virtual void onEnd( bool bIsCancel )	{}
};

struct InstanceDungeonLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct TitleLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};

struct BattleArenaLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel )	{}
};


struct AwakenLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};

struct RandomItemLoader : XBossWorker::XWorker
{
	virtual bool onProcess( int nThreadNum );

	virtual void onEnd( bool bIsCancel ) {}
};
