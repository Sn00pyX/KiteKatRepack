
#include "stdafx.h"

#include <vector>

#include <mmo/ArTime.h>

#include "StructEventItemManager.h"
#include "GameDBUtil.h"
#include "DB_Commands.h"


struct dbEventItemDropInfo : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbEventItemDropInfo)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, sid, sizeof(sid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, code, sizeof(code), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, remain_time, sizeof(remain_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, duration, sizeof(duration), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, count, sizeof(count), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, total_count, sizeof(total_count), FALSE)
END_ADO_BINDING()

	int sid;
	int code;
	int remain_time;
	int duration;
	int count;
	int total_count;
};

struct dbEventItemSupplyInfo : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbEventItemSupplyInfo)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, sid, sizeof(sid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, code, sizeof(code), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, min_count, sizeof(min_count), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, max_count, sizeof(max_count), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, flag, sizeof(flag), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adDBTimeStamp, start_time, sizeof(start_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adDBTimeStamp, end_time, sizeof(end_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, left_count, sizeof(left_count), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, total_count, sizeof(total_count), FALSE)
END_ADO_BINDING()

	int				sid;
	int				code;
	int				min_count;
	int				max_count;
	int				flag;
	DBTIMESTAMP		start_time;
	DBTIMESTAMP		end_time;
	int				left_count;
	int				total_count;
};

struct dbEventDungeonDroprateInfo : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbEventDungeonDroprateInfo)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, dungeon_id, sizeof(dungeon_id), FALSE)
	ADO_NUMERIC_ENTRY2(2, adDecimal, drop_rate, 10, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adDBTimeStamp, end_time, sizeof(end_time), FALSE)
END_ADO_BINDING()

	int dungeon_id;
	_decimal_variant drop_rate;
	DBTIMESTAMP end_time;
};

StructEventItemManager & StructEventItemManager::GetInstance()
{
	static StructEventItemManager _inst;
	return _inst;
}

bool StructEventItemManager::Init()
{
	LoadEventItemDropInfo();
	LoadEventItemSupplyInfo();
	LoadEventDungeonDroprateInfo();

	// 불러들인 게 있으면 시작하자. (이벤트 시작도 안했는데 true로 체크되어 몹 죽을때마다 검사되는 거 방지.)
	if( !m_vEventItemDropInfo.empty() )
		m_bStartEventDrop = true;
	if( !m_vEventItemSupplyInfo.empty() )
		m_bStartEventSupply = true;
	if( !m_vEventDungeonDroprateInfo.empty() )
		m_bDungeonDropRateEvent = true;
	
	return true;
}

bool StructEventItemManager::DeInit()
{
	while( !m_lQueryList.empty() )
	{
		Sleep( 100 );
	}

	return true;
}

void StructEventItemManager::onEndQuery()
{
	THREAD_SYNCRONIZE( m_QueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}

void StructEventItemManager::Push( GameDBManager::DBProc* pWork )
{
	THREAD_SYNCRONIZE( m_QueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}

float StructEventItemManager::GetEventDungeonDropRate( int nDungeonID )
{
	THREAD_SYNCHRONIZE( m_DropRateLock );

	std::vector< EVENT_DUNGEON_DROPRATE_INFO >::iterator it = m_vEventDungeonDroprateInfo.begin();
	for( ; it != m_vEventDungeonDroprateInfo.end() ; ++it )
	{
		if( nDungeonID != (*it).m_nDungeonID )
			continue;

		if( (*it).m_tEndTime < time( NULL ) )
		{
			m_vEventDungeonDroprateInfo.erase( it );
			break;
		}

		return (*it).m_fDropRate;
	}
	return 1.0f;
}

void StructEventItemManager::LoadEventDungeonDroprateInfo()
{
	THREAD_SYNCHRONIZE( m_DropRateLock );

	m_vEventDungeonDroprateInfo.clear();
	_ConnectionPtr		ConnPtr = NULL;
	InitUserDbConnection( ConnPtr );

	_RecordsetPtr pRS = NULL;

	pRS.CreateInstance( __uuidof(Recordset) );

	pRS->Open("SELECT * FROM dbo.EventDungeonDropRate ORDER BY end_time DESC", 
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdText);

	dbEventDungeonDroprateInfo emprs;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	pRS->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );
	picRs->BindToRecordset( &emprs );

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		struct tm tmEndTime;
		tmEndTime.tm_year = emprs.end_time.year - 1900;
		tmEndTime.tm_mon = emprs.end_time.month - 1;
		tmEndTime.tm_mday = emprs.end_time.day;
		tmEndTime.tm_hour = emprs.end_time.hour;
		tmEndTime.tm_min = emprs.end_time.minute;
		tmEndTime.tm_sec = emprs.end_time.second;
		tmEndTime.tm_isdst = -1;
		time_t tEndTime = mktime( &tmEndTime );

		StructEventItemManager::RegisterEventDungeonDroprateInfo( emprs.dungeon_id, emprs.drop_rate.getFloat(), tEndTime );

		pRS->MoveNext();
	}

}

void StructEventItemManager::LoadEventItemDropInfo()
{
	AR_TIME t = GetArTime();

	m_vEventItemDropInfo.clear();

	_ConnectionPtr		ConnPtr = NULL;
	InitUserDbConnection( ConnPtr );

	_RecordsetPtr pRS = NULL;

	pRS.CreateInstance( __uuidof(Recordset) );

	pRS->Open("SELECT * FROM dbo.EventItemDropInfo ORDER BY CAST( duration AS FLOAT ) / total_count DESC", 
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdText);

	dbEventItemDropInfo emprs;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	pRS->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );
	picRs->BindToRecordset( &emprs );

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		StructEventItemManager::RegisterEventItemDropInfo( emprs.sid, emprs.code, emprs.remain_time * 100, emprs.duration * 100, emprs.count, emprs.total_count );

		pRS->MoveNext();
	}

	// 드랍 개수 보관 벡터도 초기화.
	m_vEventItemDropPicker.clear();
	for( size_t i = 0 ; i < m_vEventItemDropInfo.size() ; ++i )
	{
		m_vEventItemDropPicker.push_back( 0 );
	}
}

void StructEventItemManager::RegisterEventDungeonDroprateInfo( int dungeon_id, float drop_rate, time_t end_time )
{
	THREAD_SYNCHRONIZE( m_DropRateLock );

	if( time( NULL ) < end_time )
	{
		m_vEventDungeonDroprateInfo.push_back( EVENT_DUNGEON_DROPRATE_INFO( dungeon_id, drop_rate, end_time ) );
	}
}

void StructEventItemManager::RegisterEventItemDropInfo( int sid, ItemBase::ItemCode code, AR_TIME remain_time, AR_TIME duration, int count, int total_count )
{
	THREAD_SYNCRONIZE( &m_IntfLock );

	AR_TIME t = GetArTime();

	std::vector< EVENT_ITEM_DROP_INFO >::iterator it;

	for( it = m_vEventItemDropInfo.begin(); it != m_vEventItemDropInfo.end(); ++it )
	{
		if( (*it).sid == sid )
		{
			(*it) = EVENT_ITEM_DROP_INFO( sid, code, 1, 1, -1, t + remain_time, duration, count, total_count );
			return;
		}
	}

	m_vEventItemDropInfo.push_back( EVENT_ITEM_DROP_INFO( sid, code, 1, 1, -1, t + remain_time, duration, count, total_count ) );
}

ItemBase::ItemCode StructEventItemManager::GetActiveDrop( AR_TIME t, float fDropRatePenalty )
{
	if( !m_bStartEventDrop || m_vEventItemDropInfo.size() == 0 || fDropRatePenalty == 0.0f )
		return 0;

	THREAD_SYNCRONIZE( &m_IntfLock );

	__int64 total_goal_count = 0;
	__int64 total_remain_count = 0;
	int i = 0;
	for( std::vector< EVENT_ITEM_DROP_INFO >::iterator it = m_vEventItemDropInfo.begin() ; it != m_vEventItemDropInfo.end() ; ++it, ++i )
	{
		if( (*it).left_count < 1 || (*it).total_count < 1 ) continue;

		int remain_time = 0;
		float ratio = 0.0f;

		if( (*it).end_time > t )
		{
			remain_time = (*it).end_time - t;
			// 이벤트 기간에 대한 비율, 0에 가까울 수록 이벤트 초반 / 1에 가까울 수록 이벤트 후반
			ratio = 1.0f - ( float( remain_time ) / float( (*it).duration ) );
		}

		// 현재 시간대에서 풀어야 할 수량
		__int64 goal_count = ratio * (*it).total_count;
		// 현재 떨어진 양을 감안하여 풀릴 수 있는 수량
		__int64 remain_count = goal_count - ( (*it).total_count - (*it).left_count );

		if( remain_count < 1 || goal_count < 1 )
			continue;

		total_remain_count += remain_count;
		total_goal_count += goal_count;
		m_vEventItemDropPicker[ i ] = remain_count;
	}

	if( total_remain_count < 1 || total_goal_count < 1 )
		return 0;

	// 드랍 확률은 [현재 시간 기준으로 남은 양] / [현재 시간 기준으로 풀려야 할 양]
	__int64 probability = ( total_remain_count * fDropRatePenalty ) / total_goal_count * 1000000;
	if( XRandom() % 1000000 < probability )
	{
		// 드랍 확률 통과. 이제는 무슨 아이템을 드랍시킬 지 결정.
		int nPick = XRandom() % total_remain_count;
		
		size_t i = 0;
		for( ; i < m_vEventItemDropPicker.size() ; ++i )
		{
			nPick -= m_vEventItemDropPicker[ i ];
			if( nPick < 0 )
				break;
		}
		--m_vEventItemDropInfo[ i ].left_count;

		Push( new DB_UpdateEventItemDropInfo( 
			m_vEventItemDropInfo[ i ].sid,
			( m_vEventItemDropInfo[ i ].end_time > t ) ? ( ( m_vEventItemDropInfo[ i ].end_time - t ) / 100 ) : 0, 
			m_vEventItemDropInfo[ i ].left_count )
			);

		return m_vEventItemDropInfo[ i ].code;
	}
	
	return 0;
}

void StructEventItemManager::LoadEventItemSupplyInfo()
{
	m_vEventItemSupplyInfo.clear();

	_ConnectionPtr		ConnPtr = NULL;
	InitUserDbConnection( ConnPtr );

	_RecordsetPtr pRS = NULL;

	pRS.CreateInstance( __uuidof(Recordset) );

	pRS->Open("EventItemSupplyInfo", 
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdTable);

	dbEventItemSupplyInfo emprs;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	pRS->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );
	picRs->BindToRecordset( &emprs );

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		struct tm tmStartTime;
		tmStartTime.tm_year = emprs.start_time.year - 1900;
		tmStartTime.tm_mon = emprs.start_time.month - 1;
		tmStartTime.tm_mday = emprs.start_time.day;
		tmStartTime.tm_hour = emprs.start_time.hour;
		tmStartTime.tm_min = emprs.start_time.minute;
		tmStartTime.tm_sec = emprs.start_time.second;
		tmStartTime.tm_isdst = -1;
		time_t tStartTime = mktime( &tmStartTime );

		struct tm tmEndTime;
		tmEndTime.tm_year = emprs.end_time.year - 1900;
		tmEndTime.tm_mon = emprs.end_time.month - 1;
		tmEndTime.tm_mday = emprs.end_time.day;
		tmEndTime.tm_hour = emprs.end_time.hour;
		tmEndTime.tm_min = emprs.end_time.minute;
		tmEndTime.tm_sec = emprs.end_time.second;
		tmEndTime.tm_isdst = -1;
		time_t tEndTime = mktime( &tmEndTime );

		StructEventItemManager::RegisterEventItemSupplyInfo( emprs.sid, emprs.code, emprs.min_count, emprs.max_count, tStartTime, tStartTime, tEndTime, emprs.left_count, emprs.total_count );

		pRS->MoveNext();
	}
}

void StructEventItemManager::RegisterEventItemSupplyInfo( const int & sid, const ItemBase::ItemCode & code, const int & min_count, const int & max_count, const int & flag, const time_t & start_time, const time_t & end_time, const int & left_count, const int & total_count )
{
	THREAD_SYNCRONIZE( &m_IntfLock );

	std::vector< EVENT_ITEM_SUPPLY_INFO >::iterator it;

	for( it = m_vEventItemSupplyInfo.begin() ; it != m_vEventItemSupplyInfo.end() ; ++it )
	{
		if( (*it).sid == sid )
		{
			(*it) = EVENT_ITEM_SUPPLY_INFO( sid, code, min_count, max_count, flag, start_time, end_time, left_count, total_count );
			return;
		}
	}

	m_vEventItemSupplyInfo.push_back( EVENT_ITEM_SUPPLY_INFO( sid, code, min_count, max_count, flag, start_time, end_time, left_count, total_count ) );
}

void StructEventItemManager::SetActiveSupply( const ItemBase::ItemCode & code, const int & min_count, const int & max_count, const int & flag )
{
	THREAD_SYNCRONIZE( &m_IntfLock );

	m_ReservedActiveSupply = EVENT_ITEM_INFO( code, min_count, max_count, flag );
}

const EVENT_ITEM_INFO StructEventItemManager::GetActiveSupply( const time_t & t )
{
	EVENT_ITEM_INFO ActiveSupply( 0, 0, 0, 0 );

	if( !m_bStartEventSupply )
		return ActiveSupply;

	THREAD_SYNCRONIZE( &m_IntfLock );

	// 예약된 드랍이 있다면 드랍
	if( m_ReservedActiveSupply.code )
	{
		ActiveSupply = m_ReservedActiveSupply;
		m_ReservedActiveSupply.code = 0;
	}
	else
	{
		for( std::vector< EVENT_ITEM_SUPPLY_INFO >::iterator it = m_vEventItemSupplyInfo.begin() ; it != m_vEventItemSupplyInfo.end() ; ++it )
		{
			// 남은 수량이 없거나 아직 시작되지 않은 이벤트는 제낌
			if( (*it).left_count < 1 || (*it).start_time > t ) continue;

			bool bWinning = false;
			if( (*it).end_time > t )
			{
				// 이벤트 종료 시점 이전: 당첨 체크
				time_t tSecPerOneSupply = ( (*it).end_time - (*it).start_time ) / ( (*it).total_count + 1 );
				time_t tThisSupplyTime = (*it).start_time + ( tSecPerOneSupply * ( (*it).total_count - (*it).left_count + 1 ) );
				tThisSupplyTime += tSecPerOneSupply * ( ( XRandom( 50, 150 ) - 100 ) / 100 );
				if( tThisSupplyTime <= t )
				{
					bWinning = true;
				}
			}
			else
			{
				// 이벤트 종료 시점 이후: 100% 당첨
				bWinning = true;
			}

			if( bWinning )
			{
				ActiveSupply = (*it);
				--(*it).left_count;
				Push( new DB_UpdateEventItemSupplyInfo( (*it).sid, (*it).left_count ) );
			}
		}
	}

	return ActiveSupply;
}
