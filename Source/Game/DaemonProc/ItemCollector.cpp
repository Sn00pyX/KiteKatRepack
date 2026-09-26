
#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>

#include "LogClient/LogClient.h"

#include "ItemCollector.h"
#include "StructItem.h"
#include "GameProc.h"


int					ITEM_HOLD_TIME;


bool ItemCollector::Init()
{
	ENV().SetDefaultValue( "game.item_hold_time", 180000 );

	ITEM_HOLD_TIME = ENV().GetInt( "game.item_hold_time" );

#ifdef _DEBUG
	ArcadiaServer::Instance().SetObjectPriority( &ItemCollector::Instance(), ArSchedulerObject::UPDATE_PRIORITY_HIGH );
#else
	ArcadiaServer::Instance().SetObjectPriority( &ItemCollector::Instance(), ArSchedulerObject::UPDATE_PRIORITY_LOW );
#endif

	return true;
}

bool ItemCollector::DeInit()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

		THREAD_SYNCHRONIZE( m_lock );

		for( std::vector< StructItem * >::iterator it = m_vItem.begin() ; it != m_vItem.end() ; ++it )
		{
			// 루피 삭제는 로그 남기지 않음(불필요하게 많이 남으며, 유저는 돈을 버릴 수는 없음)
			if( (*it)->GetItemCode() > 0 )
			{
				LOG::Log11N4S( LM_ITEM_DELETE, 0, 0, (*it)->GetItemEnhance() * 100 + (*it)->GetItemLevel(), (*it)->GetItemCode(), (*it)->GetCount(), (*it)->GetCount(), 0, 0, (*it)->GetX(), (*it)->GetY(), (*it)->GetItemUID(), "", 0, "", 0, "SUCS", LOG::STR_NTS, "COLLECTOR", LOG::STR_NTS );
			}

			// 월드에서 제거 ( 여기서는 RemoveItemFromWorld 를 쓰면 안됨 -.- )
			ArcadiaServer::Instance().RemoveObject( (*it) );

			StructItem::PendFreeItem( *it );
		}
	}

	return true;
}

void ItemCollector::RegisterItem( struct StructItem* pItem )
{
	THREAD_SYNCRONIZE( m_lock );

	m_vItem.push_back( pItem );
	pItem->SetIdx( static_cast< int >( m_vItem.size()-1 ) );
}

const bool ItemCollector::UnregisterItem( struct StructItem* pItem )
{
	THREAD_SYNCRONIZE( m_lock );

	int nStorageIdx = pItem->GetIdx();

	// 삭제 처리 중인 아이템인 경우 실패~
	if( nStorageIdx == IDX_RESERVED_TO_DELETE )
		return false;

	// 리스트에서 제거
	if( m_vItem.back() != pItem )
	{
		if( m_vItem.size() <= nStorageIdx || pItem != m_vItem[ nStorageIdx ] )
		{
			assert( 0 );
			_cprint( "ItemCollector::UnregisterItem: Trying to unregister item not registered detected: Code(%d) Count(%I64d) GenCode(%d)\n", pItem->GetItemCode(), pItem->GetCount(), pItem->GetGenerateCode() );
			FILELOG( "ItemCollector::UnregisterItem: Trying to unregister item not registered detected: Code(%d) Count(%I64d) GenCode(%d)", pItem->GetItemCode(), pItem->GetCount(), pItem->GetGenerateCode() );

			return false;
		}

		StructItem *pReplacementItem = m_vItem.back();
		m_vItem[ nStorageIdx ] = pReplacementItem;
		pReplacementItem->SetIdx( nStorageIdx );
	}

	m_vItem.pop_back();

	return true;
}

void ItemCollector::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "ItemCollector" );

	AR_TIME	t = GetArTime();

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "ItemCollector(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	// 기존 자료 제거
	m_vTmp.clear();

	{
		THREAD_SYNCRONIZE( m_lock );

		for( size_t nIndex = 0 ; nIndex < m_vItem.size() ; /* 루프에서 ++nIndex 처리 */ )
		{
			StructItem * pItem = m_vItem[ nIndex ];

			// 버린 시간 체크. 버린지 얼마안되었으면 skip
			if( pItem->GetDropTime() + ITEM_HOLD_TIME > t )
			{
				++nIndex;
				continue;
			}

			// 이벤트용 아이템으로 월드에 드랍된 거면 skip
			if( pItem->GetEventDropFlag() )
			{
				++nIndex;
				continue;
			}

			// 지워도 된다면..
			if( pItem->IsDeleteable() )
			{
				pItem->SetIdx( IDX_RESERVED_TO_DELETE );

				// 리스트에서 제거하고..
				if( m_vItem.back() != pItem )
				{
					m_vItem.back()->SetIdx( static_cast< int >( nIndex ) );
					m_vItem[ nIndex ] = m_vItem.back();
				}

				m_vItem.pop_back();

				m_vTmp.push_back( pItem );

				continue;
			}

			++nIndex;
		}
	}

	std::vector< struct StructItem* >::iterator it;
	for( it = m_vTmp.begin(); it != m_vTmp.end(); ++it )
	{
		// 락 하고
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( *it ) );

		if( (*it)->IsInWorld() )
		{
			// 루피 삭제는 로그 남기지 않음(불필요하게 많이 남으며, 유저는 돈을 버릴 수는 없음)
			if( (*it)->GetItemCode() > 0 )
			{
				LOG::Log11N4S( LM_ITEM_DELETE, 0, 0, (*it)->GetItemEnhance() * 100 + (*it)->GetItemLevel(), (*it)->GetItemCode(), (*it)->GetCount(), (*it)->GetCount(), 0, 0, (*it)->GetX(), (*it)->GetY(), (*it)->GetItemUID(), "", 0, "", 0, "SUCS", LOG::STR_NTS, "COLLECTOR", LOG::STR_NTS );
			}

			// 월드에서 제거 ( 여기서는 RemoveItemFromWorld 를 쓰면 안됨 -.- )
			ArcadiaServer::Instance().RemoveObject( (*it) );

			StructItem::PendFreeItem( *it );
		}
	}

	
}

ItemCollector & ItemCollector::Instance()
{
	static ItemCollector _instance;

	return _instance;
}