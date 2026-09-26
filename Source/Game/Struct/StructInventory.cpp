#include "StructItem.h"
#include "StructInventory.h"
#include "LogClient/LogClient.h"
#include "DB_Commands.h"

#include <toolkit/XSTLUtil.h>
#include <toolkit/XConsole.h>

StructInventory::StructInventory( InventoryEventReceiver * pEventReceiver ) : m_fWeight( 0.0f ), m_fWeightModifier( 0 ), m_pEventReceiver( pEventReceiver )
{
	m_vList.reserve( 50 );
}

StructInventory::~StructInventory()
{
}

struct StructItem*	StructInventory::Join( struct StructItem* pItem, const __int64 & cnt, bool bSkipUpdateItemToDB )
{
	if( cnt <= 0 )
		return NULL;

	// 중첩 가능한 아이템을 찾았다면
	StructItem *pExistItem = FindJoinablePair( pItem );
	if( pExistItem != NULL )
	{
		// 무게 조정(장착된 아이템의 경우 무게 작용 안 함... -_-; 장착된 아템을 획득 가능한가;;)
		m_fWeight += ( cnt * pExistItem->GetItemBase().fWeight );
		if( pExistItem->GetWearInfo() != -1 )
			m_fWeightModifier -= ( cnt * pExistItem->GetItemBase().fWeight );

		// 아이템 갯수 늘린다
		setCount( pExistItem, pExistItem->GetCount() + cnt, bSkipUpdateItemToDB );
	}

	return pExistItem;
}

StructItem* StructInventory::Push( StructItem *pItem, const __int64 & cnt, bool bSkipUpdateItemToDB )
{
	if( cnt <= 0 )
	{
		_cprint( "StructInventory::Push failed - cnt %I64d for Item code[%d], Count[%I64d], UID[%I64d]\n", 
			cnt, pItem->GetItemCode(), pItem->GetCount(), pItem->GetItemUID() );
		FILELOG( "StructInventory::Push failed - cnt %I64d for Item code[%d], Count[%I64d], UID[%I64d]", 
			cnt, pItem->GetItemCode(), pItem->GetCount(), pItem->GetItemUID() );
		return NULL;
	}

	StructItem *pExistItem	= Join( pItem, cnt, bSkipUpdateItemToDB );

	// 합쳐야 하는 경우가 아니라면...
	if( pExistItem == NULL )
	{
		// 무게 조정(장착된 아이템의 경우 무게 작용 안 함... -_-; 장착된 아템을 획득 가능한가;; <- 로그인시 아이템 정보 읽으면서 가능;;)
		m_fWeight += pItem->GetWeight();
		if( pItem->GetWearInfo() != -1 )
			m_fWeightModifier -= pItem->GetWeight();

		push( pItem, bSkipUpdateItemToDB );

		return pItem;
	}

	return pExistItem;
}

StructItem *StructInventory::Pop( StructItem *pItem, const __int64 & cnt, bool bSkipUpdateItemToDB )
{
	// 유효성 검사
	if( !check( pItem ) )
		return NULL;

	// 일부만 꺼낼 경우는 아이템을 복제한다
	if( pItem->GetCount() > cnt && cnt > 0 )
	{
		// 일단 모든 속성 복제
		StructItem *pNewItem = StructItem::AllocItem( 0, pItem->GetItemCode(), 1, ItemInstance::BY_DIVIDE );
		pNewItem->CopyFrom( pItem );

		// 아이템갯수 설정
		pNewItem->SetCount( cnt );

		// 꺼낸만큼 무게를 줄인다(장착된 아이템은 무게에 포함되지 않았으므로 안 뺌)
		m_fWeight -= ( cnt * pItem->GetItemBase().fWeight );
		if( pItem->GetWearInfo() != -1 )
			m_fWeightModifier += ( cnt * pItem->GetItemBase().fWeight );

		// 기존 아이템 갯수를 줄인다
		setCount( pItem, pItem->GetCount() - cnt, bSkipUpdateItemToDB );

		return pNewItem;
	}

	// 전부다 꺼내는 경우는 컨테이너에서 제거한다.
	if( pItem->GetCount() == cnt ) 
	{
		// 꺼낸만큼 무게를 줄인다(장착된 아이템은 무게에 포함되지 않았으므로 안 뺌)
		m_fWeight -= ( cnt * pItem->GetItemBase().fWeight );
		if( pItem->GetWearInfo() != -1 )
			m_fWeightModifier += pItem->GetWeight();

		pop( pItem, bSkipUpdateItemToDB );

		return pItem;
	}

	return NULL;
}

bool StructInventory::check( struct StructItem* pItem ) const
{
	if( pItem->GetIdx() >= static_cast< int >( m_vList.size() ) ) 
	{
		_cprint( "wrong indexed item in inventory detected while checking. Item(%I64d)\n", pItem->GetItemUID() );
		FILELOG( "wrong indexed item in inventory detected while chekcing. Item(%I64d)", pItem->GetItemUID() );
		assert( 0 );
		return false;
	}

	if( m_vList[ pItem->GetIdx() ] != pItem )
	{
		_cprint( "wrong indexed item in inventory detected while checking. Item(%I64d)\n", pItem->GetItemUID() );
		FILELOG( "wrong indexed item in inventory detected while checking. Item(%I64d)", pItem->GetItemUID() );
		assert( 0 );
		return false;
	}

	return true;
}

bool StructInventory::Erase( struct StructItem* pItem, const __int64 & cnt, bool bSkipUpdateItemToDB )
{
	// 유효성 검사
	if( !check( pItem ) ) return false;

	if( cnt < 1 )
	{
		return false;
	}

	// 일부만 지울 경우는 수량만 조절한다.
	if( pItem->GetCount() > cnt )
	{
		// 삭제된만큼 무게를 줄인다(장착된 아이템은 무게에 포함되지 않았으므로 안 뺌)
		m_fWeight -= cnt * pItem->GetItemBase().fWeight;
		if( pItem->GetWearInfo() != -1 )
			m_fWeightModifier += ( cnt * pItem->GetItemBase().fWeight );

		setCount( pItem, pItem->GetCount() - cnt, bSkipUpdateItemToDB );

		return true;
	}

	// 전부다 지우는 경우는 컨테이너에서 제거하고 아이템은 삭제한다.
	m_fWeight -= pItem->GetWeight();
	if( pItem->GetWearInfo() != -1 )
		m_fWeightModifier += pItem->GetWeight();

	pop( pItem, bSkipUpdateItemToDB );

	// _oprint( "F:" );
	StructItem::PendFreeItem_( pItem, NULL, 0 );

	return true;
}

bool StructInventory::Swap( int idx1, int idx2 )
{
	// 유효성 검사
	if( idx1 == idx2 )
		return false;
	int maxIdx = static_cast< int >( m_vList.size() ) - 1;
	if( idx1 > maxIdx|| idx2 > maxIdx )
		return false;
	if( m_vList[ idx1 ]->GetIdx() != idx1 )
		return false;
	if( m_vList[ idx2 ]->GetIdx() != idx2 )
		return false;

	// 순서까지 고려하면 햇갈리니까 idx1가 항상 작거나 같게
	if( idx1 > idx2 )
	{
		int temp = idx1;
		idx1 = idx2;
		idx2 = temp;
	}

	// 우선 리스트 내에서 바꿔주고 인덱스 설정한 후
	StructItem * pTempItem = m_vList[ idx1 ];
	m_vList[ idx1 ] = m_vList[ idx2 ];
	m_vList[ idx2 ] = pTempItem;

	m_vList[ idx1 ]->SetIdx( idx1 );
	m_vList[ idx2 ]->SetIdx( idx2 );

	// 마지막으로 PreviousUID까지 바꿔줌 (만약 첫번째 요소와 스왑할 경우 previousUID는 0)
	if( idx1 != 0 )
		m_vList[ idx1 ]->SetPreviousUID( m_vList[ idx1 - 1 ]->GetItemUID() );
	else
		m_vList[ idx1 ]->SetPreviousUID( 0 );
	m_vList[ idx1 ]->TurnOnUpdateFlag();
	m_vList[ idx1+1 ]->SetPreviousUID( m_vList[ idx1 ]->GetItemUID() );
	m_vList[ idx1+1 ]->TurnOnUpdateFlag();
	m_vList[ idx2 ]->SetPreviousUID( m_vList[ idx2 - 1 ]->GetItemUID() );
	m_vList[ idx2 ]->TurnOnUpdateFlag();
	if( idx2 != static_cast< int >( m_vList.size() - 1 ) )
	{
		m_vList[ idx2+1 ]->SetPreviousUID( m_vList[ idx2 ]->GetItemUID() );
		m_vList[ idx2+1 ]->TurnOnUpdateFlag();
	}
	
	return true;
}

StructItem*	StructInventory::Find( ItemUID uid ) const
{
	std::vector< StructItem* >::const_iterator it;
	for( it = m_vList.begin(); it != m_vList.end(); ++it )
	{
		if( (*it)->GetItemUID() == uid ) return *it;
	}
	return NULL;
}

StructItem*	StructInventory::Find( ItemBase::ItemCode code ) const
{
	std::vector< StructItem* >::const_iterator it;
	for( it = m_vList.begin(); it != m_vList.end(); ++it )
	{
		if( (*it)->GetItemCode() == code ) return *it;
	}
	return NULL;
}

StructItem*	StructInventory::Find( ItemBase::ItemCode code, unsigned int flag, bool bFlag ) const
{
	std::vector< StructItem* >::const_iterator it;
	for( it = m_vList.begin(); it != m_vList.end(); ++it )
	{
		bool bIsOn = (*it)->GetInstanceFlag().IsOn( flag );
		if( (*it)->GetItemCode() == code && ( (bFlag && bIsOn) || (!bFlag && !bIsOn) ) ) return *it;
	}
	return NULL;
}

void StructInventory::Find( ItemBase::ItemCode code, std::vector< StructItem * > & vList )
{
	std::vector< StructItem* >::const_iterator it;
	for( it = m_vList.begin(); it != m_vList.end(); ++it )
	{
		if( (*it)->GetItemCode() == code ) vList.push_back( (*it) );
	}
}

StructItem* StructInventory::FindJoinablePair( StructItem *pItem )
{
	if( !pItem->IsJoinable() )
		return NULL;

	std::vector< StructItem* >::const_iterator it;
	for( it = m_vList.begin(); it != m_vList.end(); ++it )
	{
		if( (*it)->GetItemCode() == pItem->GetItemCode() && 
			(*it)->GetItemEnhance() == pItem->GetItemEnhance() &&
			(*it)->GetItemLevel() == pItem->GetItemLevel() &&
			(*it) != pItem &&
			(*it)->IsJoinable()	)
		{
			return (*it);
		}
	}

	return NULL;
}

const __int64 StructInventory::GetItemCount( ItemBase::ItemCode code ) const
{
	__int64 nCount = 0;

	std::vector< StructItem* >::const_iterator it;

	for( it = m_vList.begin(); it != m_vList.end(); ++it )
	{
		if( (*it)->GetItemCode() == code ) nCount += (*it)->GetCount();
	}

	return nCount;
}

void StructInventory::setCount( struct StructItem* pItem, const __int64 & new_cnt, bool bSkipUpdateItemToDB )
{
	pItem->SetCount( new_cnt );

	if( m_pEventReceiver) m_pEventReceiver->onChangeCount( this, pItem, bSkipUpdateItemToDB );
}

void StructInventory::push( struct StructItem* pItem, bool bSkipUpdateItemToDB )
{
	// 빈 리스트가 아니면 PreviousUID 넣어주기
	if( m_vList.size() > 0 )
	{
		ItemUID lastItemUID = m_vList[ m_vList.size()-1 ]->GetItemUID();
		if( pItem->GetPreviousUID() != lastItemUID )
		{
			pItem->SetPreviousUID( lastItemUID );
			pItem->TurnOnUpdateFlag();
		}
	}
	else // 빈 리스트였다면 이 아이템의 PreviousUID는 0.
	{
		if( pItem->GetPreviousUID() != 0 )
		{
			pItem->SetPreviousUID( 0 );
			pItem->TurnOnUpdateFlag();
		}
	}

	m_vList.push_back( pItem );

	if( pItem->IsExpireItem() )
		m_vExpireItemList.push_back( pItem );

	pItem->SetIdx( (int)( m_vList.size() - 1 ) );

	if( m_pEventReceiver) m_pEventReceiver->onAdd( this, pItem, bSkipUpdateItemToDB );
}

void StructInventory::pop( struct StructItem* pItem, bool bSkipUpdateItemToDB )
{
	// PreviousUID 연결해주기
	if( pItem->GetIdx() != static_cast< int >( m_vList.size() - 1 ) )
	{
		StructItem * pNextItem = m_vList[ pItem->GetIdx() + 1 ];
		
		ItemUID previousUID = 0;
		if( pItem->GetIdx() != 0 )
		{
			previousUID = m_vList[ pItem->GetIdx() - 1 ]->GetItemUID();
		}
		pNextItem->SetPreviousUID( previousUID );
		pNextItem->TurnOnUpdateFlag();
	}

	// 한 칸씩 땡겨준다. GetIdx()의 실제 위치 유효성은 앞에 check에서 확인
	for( int i = pItem->GetIdx() ; i < m_vList.size() - 1 ; ++i )
	{
		m_vList[ i ] = m_vList[ i + 1 ];
		m_vList[ i ]->SetIdx( i );
	}

	// 마지막 원소 제거
	m_vList.pop_back();

	std::vector< StructItem * >::iterator itErase = std::find( m_vExpireItemList.begin(), m_vExpireItemList.end(), pItem );
	if( itErase != m_vExpireItemList.end() )
	{
		m_vExpireItemList.erase( itErase );
	}

	if( m_pEventReceiver ) m_pEventReceiver->onRemove( this, pItem, bSkipUpdateItemToDB );
}

const bool StructInventory::_ItemArrangeGreater( const struct StructItem * lhs, const struct StructItem * rhs )
{
	// 1차 정렬 기준: ItemBase::nArrangeCode
	// 2차 정렬 기준: ItemBase::ITEM_TYPE	(ItemBase::nType)
	// 3차 정렬 기준: ItemBase::ItemCode	(ItemInstance::Code)
	// 4차 정렬 기준: ItemInstance::nEnhance; 4차 정렬 기준은 내림차순

	// 1차 정렬 기준은 nArrangeCode 값을 오름차순으로 정리하면 된다.
	int nLHSArrangeCode = lhs->GetItemBase().nArrangeCode;
	int nRHSArrangeCode = rhs->GetItemBase().nArrangeCode;
	
	if( nLHSArrangeCode < nRHSArrangeCode )
		return true;
	else if( nLHSArrangeCode > nRHSArrangeCode )
		return false;
	
	// 2차 정렬 기준 순서 지정 배열(nType이 배열 인덱스가 되면 정렬 인덱스가 나옴)
	static const int aTypeOrder[] = { 7, 0, 1, 6, 2, 3, 5, 4 };
	static const int nOrderArrayLength = sizeof( aTypeOrder ) / sizeof( int );

	int nLHSType = lhs->GetItemBase().nType;
	int nRHSType = rhs->GetItemBase().nType;

	assert( nLHSType >= 0 && nLHSType < nOrderArrayLength );
	assert( nRHSType >= 0 && nRHSType < nOrderArrayLength );

	// 유효하지 않은 ITEM_TYPE 이면 맨 뒤로 빼버림
	nLHSType = ( nLHSType >= 0 && nLHSType < nOrderArrayLength ) ? aTypeOrder[ nLHSType ] : nOrderArrayLength;
	nRHSType = ( nRHSType >= 0 && nRHSType < nOrderArrayLength ) ? aTypeOrder[ nRHSType ] : nOrderArrayLength;

	// 2차 정렬 기준: 아이템 타입(오름 차순)
	if( nLHSType < nRHSType )
		return true;
	if( nLHSType > nRHSType )
		return false;

	// 3차 정렬 기준: 아이템 코드(오름 차순)
	if( lhs->GetItemCode() < rhs->GetItemCode() )
		return true;
	if( lhs->GetItemCode() > rhs->GetItemCode() )
		return false;

	// 4차 정렬 기준: 아이템 강화 정도(내림 차순)
	if( lhs->GetItemEnhance() > rhs->GetItemEnhance() )
		return true;

	return false;
}

void StructInventory::Arrange()
{
	std::vector< struct StructItem * > vSortBuffer( m_vList );
	std::sort( vSortBuffer.begin(), vSortBuffer.end(), StructInventory::_ItemArrangeGreater );
	
	m_vList.clear();

	ItemUID previousUID = 0;
	int i = 0;
	for( std::vector< struct StructItem * >::iterator it = vSortBuffer.begin() ; it != vSortBuffer.end() ; ++it )
	{
		StructItem * pItem = (*it);
		pItem->SetIdx( i++ );
		m_vList.push_back( pItem );

		if( pItem->GetPreviousUID() != previousUID )
		{
			pItem->SetPreviousUID( previousUID );
			pItem->TurnOnUpdateFlag();
		}

		previousUID = pItem->GetItemUID();
	}
}

void StructInventory::LoadItemList( std::vector< StructItem::INDEX_FINDER > &vItemList, bool isStorage )
{
	const char * szType = "Login";
	if( isStorage )
		szType = "StorageOpen";

	std::vector< StructItem::INDEX_FINDER >::iterator it = vItemList.begin();
	StructItem * pItem;
	ItemUID prevUID = 0;
	while( vItemList.begin() != vItemList.end() )
	{
		it = std::find( vItemList.begin(), vItemList.end(), prevUID );
		if( it == vItemList.end() )
		{
			it = vItemList.begin();
			prevUID = 0;
		}
		else
		{
			prevUID = (*it).pItem->GetItemUID();
		}

		pItem = (*it).pItem;
		StructItem * pExistItem = Push( pItem, pItem->GetCount() );
		
		if( pExistItem && pExistItem != pItem )
		{
			// 아이템이 합쳐지면 추적을 위해 로그 남겨준다.
			LOG::Log11N4S( LM_ITEM_JOIN, 0, 
				0, pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), 
				pItem->GetItemCode(), pItem->GetCount(),
				pExistItem->GetCount(), 0,
				0, 0,
				pExistItem->GetItemUID(), pItem->GetItemUID(),
				"", 0, 
				"", 0,
				"", 0,
				szType, LOG::STR_NTS );
			pExistItem->DBQuery( new DB_UpdateItem( pExistItem ) );
			pItem->SetOwnerInfo( NULL, 0, 0 );
			pItem->DBQuery( new DB_UpdateItemOwner( pItem ) );
			StructItem::PendFreeItem( pItem );
		}

		vector_fast_erase( &vItemList, it );
	}
}
