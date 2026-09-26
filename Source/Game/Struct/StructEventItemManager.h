#pragma once

#include <list>

#include <toolkit/ILock.h>
#include <mmo/ArType.h>

#include "ItemBase.h"
#include "ItemInstance.h"
#include "GameDBManager.h"

struct EVENT_ITEM_INFO
{
	EVENT_ITEM_INFO( const ItemBase::ItemCode & _code, const int & _min_count, const int & _max_count, const int & _flag )
		: code( _code )
		, min_count( _min_count )
		, max_count( _max_count )
		, flag( _flag )
	{}

	ItemBase::ItemCode	code;
	int					min_count;
	int					max_count;
	int					flag;
};

struct EVENT_ITEM_DROP_INFO : EVENT_ITEM_INFO
{
	EVENT_ITEM_DROP_INFO( const int & _sid, const ItemBase::ItemCode & _code, const int & _min_count, const int & _max_count, const int & _flag, const AR_TIME & _end_time, const AR_TIME & _duration, const int & _left_count, const int & _total_count )
		: EVENT_ITEM_INFO( _code, _min_count, _max_count, _flag )
		, sid( _sid )
		, end_time( _end_time )
		, duration( _duration )
		, left_count( _left_count )
		, total_count( _total_count )
	{}

	int sid;
	AR_TIME end_time;
	AR_TIME duration;
	int left_count;
	int total_count;
};

struct EVENT_DUNGEON_DROPRATE_INFO
{
	EVENT_DUNGEON_DROPRATE_INFO( int _dungeon_id, float _drop_rate, time_t _end_time )
		: m_nDungeonID( _dungeon_id )
		, m_fDropRate( _drop_rate )
		, m_tEndTime( _end_time )
	{}

	int m_nDungeonID;
	float m_fDropRate;
	time_t m_tEndTime;
};

struct EVENT_ITEM_SUPPLY_INFO : EVENT_ITEM_INFO
{
	EVENT_ITEM_SUPPLY_INFO( const int & _sid, const ItemBase::ItemCode & _code, const int & _min_count, const int & _max_count, const int & _flag, const time_t & _start_time, const time_t & _end_time, const int & _left_count, const int & _total_count )
		: EVENT_ITEM_INFO( _code, _min_count, _max_count, flag )
		, sid( _sid )
		, start_time( _start_time )
		, end_time( _end_time )
		, left_count( _left_count )
		, total_count( _total_count )
	{}

	int					sid;
	time_t				start_time;
	time_t				end_time;
	int					left_count;
	int					total_count;
};

struct StructEventItemManager
{
public:
	static StructEventItemManager & GetInstance();

	bool Init();
	bool DeInit();

	void StartEventDrop()						{ m_bStartEventDrop = true; }
	void StopEventDrop()						{ m_bStartEventDrop = false; }

	// 던전 드랍 이벤트 관련 처리 (던전 드랍율 이벤트도 이벤트 아이템 매니저 관리 대상 중 하나로 본다.)
	void StartDungeonDropRateEvent()			{ m_bDungeonDropRateEvent = true; }
	void StopDungeonDropRateEvent()				{ m_bDungeonDropRateEvent = false; }
	bool IsStartingDungeonDropRateEvent()		{ return m_bDungeonDropRateEvent; }
	float GetEventDungeonDropRate( int nDungeonID );
	void LoadEventDungeonDroprateInfo();
	void RegisterEventDungeonDroprateInfo( int dungeon_id, float drop_rate, time_t end_time );

	void LoadEventItemDropInfo();
	void RegisterEventItemDropInfo( int sid, ItemBase::ItemCode code, AR_TIME remain_time, AR_TIME duration, int count, int total_count );
	ItemBase::ItemCode GetActiveDrop( AR_TIME t, float fDropRatePenalty );

	void StartEventSupply()						{ m_bStartEventSupply = true; }
	void StopEventSupply()						{ m_bStartEventSupply = false; }

	void LoadEventItemSupplyInfo();
	void RegisterEventItemSupplyInfo( const int & sid, const ItemBase::ItemCode & code, const int & min_count, const int & max_count, const int & flag, const time_t & start_time, const time_t & end_time, const int & left_count, const int & total_count );
	void SetActiveSupply( const ItemBase::ItemCode & code, const int & min_count = 1, const int & max_count = 1, const int & flag = ItemInstance::ITEM_FLAG_NONE );
	const EVENT_ITEM_INFO GetActiveSupply( const time_t & t );

	// DB Worker 클래스에서 호출해야 하므로 public이어야 함
	void onEndQuery();

protected:
	void Push( GameDBManager::DBProc* pWork );

private:
	// 기본값 false 여야 제어 가능 함
	StructEventItemManager()
		: m_QueryLock( "StructEventItemManager::m_QueryLock" )
		, m_IntfLock( "StructEventItemManager::m_IntfLock" )
		, m_DropRateLock( "StructEventItemManager::m_DropRateLock" )
		, m_bStartEventDrop( false )
		, m_bStartEventSupply( false )
		, m_bDungeonDropRateEvent( false )
		, m_ReservedActiveDrop( 0, 0, 0, 0 )
		, m_ReservedActiveSupply( 0, 0, 0, 0 )
	{}
	~StructEventItemManager() {}

private:
	XCriticalSection							m_QueryLock;
	XCriticalSection							m_IntfLock;
	XCriticalSection							m_DropRateLock;

	bool										m_bStartEventDrop;
	bool										m_bStartEventSupply;
	bool										m_bDungeonDropRateEvent;
	EVENT_ITEM_INFO								m_ReservedActiveDrop;
	std::vector< EVENT_ITEM_DROP_INFO >			m_vEventItemDropInfo;
	std::vector< int >							m_vEventItemDropPicker;		// 이벤트 아이템들의 떨어트려도 되는 개수 보관 벡터 (확률 계산시 사용)
	EVENT_ITEM_INFO								m_ReservedActiveSupply;
	std::vector< EVENT_ITEM_SUPPLY_INFO >		m_vEventItemSupplyInfo;
	std::vector< EVENT_DUNGEON_DROPRATE_INFO >	m_vEventDungeonDroprateInfo;

	std::list< GameDBManager::DBProc* >			m_lQueryList;
};
