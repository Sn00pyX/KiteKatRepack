
#include <algorithm>

#include <toolkit/XEnv.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XSTLUtil.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "GameMessage.h"
#include "AuctionManager.h"
#include "DB_Commands.h"
#include "GameRule.h"

// fillAuctionMessageBuffer 함수를 사용해야 함
#include "SendMessage.h"

extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;


AuctionManager & AuctionManager::Instance()
{
	static AuctionManager _instance;

	return _instance;
}

AuctionManager::~AuctionManager()
{
	ClearAutoAuctionInfo();

	{
		THREAD_SYNCHRONIZE( m_QueryLock );

		while( !m_lQueryList.empty() )
			Sleep( 100 );
	}

	{
		THREAD_SYNCHRONIZE( m_AuctionLock );
		for( std::vector< AuctionList * >::iterator itAuctionList = m_vplAuctionListByCategory.begin() ; itAuctionList != m_vplAuctionListByCategory.end() ; ++itAuctionList )
		{
			for( AuctionListIterator itAuction = (*itAuctionList)->begin() ; itAuction != (*itAuctionList)->end() ; ++itAuction )
			{
				delete (*itAuction);
			}
			(*itAuctionList)->clear();
			delete (*itAuctionList);
		}
		m_vplAuctionListByCategory.clear();
	}

	{
		THREAD_SYNCHRONIZE( m_ItemKeepingLock );
		for( ItemKeepingVecIterator it = m_vItemKeepingList.begin() ; it != m_vItemKeepingList.end() ; ++it )
		{
			delete (*it);
		}
		m_vItemKeepingList.clear();
	}
}

bool AuctionManager::RegisterAuctionCategory( int category_id, int sub_category_id, int item_group, int item_class )
{
	// AuctionManager가 활성화 되었으면 이미 경매 데이터가 기존의 Category에 의해 분류되었을 수 있으므로 절대 금지
	if( GetFinalPriority() > ArSchedulerObject::UPDATE_PRIORITY_IDLE )
	{
		return false;
	}

	THREAD_SYNCHRONIZE( m_AuctionLock );

	// Level 2 트리를 vector를 이용하여 구성(상위 노드 id = category_id, 하위 노드 id = sub_category_id)

	// 상위 노드들 중 지정된 category_id 에 해당하는 노드 검색
	for( AuctionCategoryVecIterator itParentCategory = m_vAuctionCategory.begin() ; itParentCategory != m_vAuctionCategory.end() ; ++itParentCategory )
	{
		if( (*itParentCategory).nCategoryID == category_id )
		{
			// sub_category_id == SUB_CATEGORY_NONE 이라면 상위 노드 추가인데
			// 이미 동일한 category_id의 상위 노드가 있다면 ItemCategory 만 추가
			if( sub_category_id == SUB_CATEGORY_NONE )
			{
				if( (*itParentCategory).IsOverlappedItemCategory( item_group, item_class ) )
				{
					return false;
				}

				(*itParentCategory).AddItemCategory( item_group, item_class );
				return true;
			}

			// 자식 노드들 검색
			for( AuctionCategoryVecIterator itChildCategory = (*itParentCategory).vChildAuctionCategory.begin() ; itChildCategory != (*itParentCategory).vChildAuctionCategory.end() ; ++itChildCategory )
			{
				// category_id != CATEGORY_SPECIAL && sub_category_id != SUB_CATEGORY_NONE 이라면 하위 노드 추가인데
				// 이미 동일한 category_id, sub_category_id 의 노드가 있다면 ItemCategory 만 추가
				if( (*itChildCategory).nSubCategoryID == sub_category_id )
				{
					if( (*itChildCategory).IsOverlappedItemCategory( item_group, item_class ) )
					{
						return false;
					}

					(*itChildCategory).AddItemCategory( item_group, item_class );
					return true;
				}
			}

			// 새로운 자식 노드 추가
			(*itParentCategory).vChildAuctionCategory.push_back( AuctionCategoryInfo( category_id, sub_category_id ) );
			(*itParentCategory).vChildAuctionCategory.back().AddItemCategory( item_group, item_class );

			return true;
		}
	}

	// category_id에 해당하는 상위 카테고리가 없으므로 신규 상위 노드 추가

	// 상위 카테고리 추가를 해야 하는데 sub_category_id가 지정되어 있다면 데이터 이상(AuctionCategoryLoader의 LoadDbResource나 기획팀 데이터 확인 필요)
	if( sub_category_id != SUB_CATEGORY_NONE )
	{
		return false;
	}

	// 새로운 상위 노드 추가
	m_vAuctionCategory.push_back( AuctionCategoryInfo( category_id, sub_category_id ) );
	m_vAuctionCategory.back().AddItemCategory( item_group, item_class );

	if( category_id > m_nMaxCategoryIndex )
	{
		m_nMaxCategoryIndex = category_id;
	}

	return true;
}

bool AuctionManager::FinalizeAuctionCategory()
{
	// AuctionCategoryInfo가 존재하지 않으면 로딩 완료 이전에 Finalize 되는 것이므로 실패
	if( m_vAuctionCategory.empty() )
	{
		return false;
	}

	if( !m_vplAuctionListByCategory.empty() )
	{
		return false;
	}

	// AuctionCategory 트리의 카테고리 수 + 1(기타) 만큼의 AuctionList 생성(카테고리별 AuctionInfo 보관 리스트)
	for( int i = 0 ; i <= m_nMaxCategoryIndex ; ++i )
	{
		m_vplAuctionListByCategory.push_back( new AuctionList() );
	}

	return true;
}

size_t AuctionManager::GetAuctionCategoryCount( const bool bOnlyParentCategory ) const
{
	size_t nCount = 0;

	if( !bOnlyParentCategory )
	{
		for( AuctionCategoryVecConstIterator itParentCategory = m_vAuctionCategory.begin() ; itParentCategory != m_vAuctionCategory.end() ; ++itParentCategory )
		{
			nCount += (*itParentCategory).vChildAuctionCategory.size();
		}
	}

	nCount += m_vAuctionCategory.size();

	return nCount;
}

size_t AuctionManager::GetAuctionItemCategoryCount() const
{
	size_t nCount = 0;

	for( AuctionCategoryVecConstIterator itParentCategory = m_vAuctionCategory.begin() ; itParentCategory != m_vAuctionCategory.end() ; ++itParentCategory )
	{
		for( AuctionCategoryVecConstIterator itChildCategory = (*itParentCategory).vChildAuctionCategory.begin() ; itChildCategory != (*itParentCategory).vChildAuctionCategory.end() ; ++itChildCategory )
		{
			nCount += (*itChildCategory).GetItemCategoryCount();
		}

		nCount += (*itParentCategory).GetItemCategoryCount();
	}

	return nCount;
}

bool AuctionManager::AddAuctionInfoByLoading( AuctionInfo *pAuctionInfo )
{
	if( !pAuctionInfo )
	{
		return false;
	}

	THREAD_SYNCHRONIZE( m_AuctionLock );

	// 이미 등록된 AuctionUID 중복 사용 체크
	if( m_hsAuctionListByAuctionID.lookup( pAuctionInfo->nAuctionID ) )
	{
		return false;
	}

	// 경매 데이터 검색용 벡터/해쉬 추가
	if( addAuctionInfoToIndex( pAuctionInfo ) != RESULT_SUCCESS )
	{
		return false;
	}

	// BidderUID 기준 검색용 해쉬 추가
	AuctionVector *pvAuctionInfo = NULL;
	for( std::vector< PlayerUID >::iterator it = pAuctionInfo->vBidderUID.begin() ; it != pAuctionInfo->vBidderUID.end() ; ++it )
	{
		if( !m_hsBiddedAuctionList.lookup( (*it), pvAuctionInfo ) )
		{
			pvAuctionInfo = new AuctionVector;
			m_hsBiddedAuctionList.add( (*it), pvAuctionInfo );
		}
		pvAuctionInfo->push_back( pAuctionInfo );
	}

	// HighestBidderUID 에 대해 별도로 BidderUID 기준 검색용 해쉬 추가
	if( pAuctionInfo->nHighestBidderUID )
	{
		if( !m_hsBiddedAuctionList.lookup( pAuctionInfo->nHighestBidderUID, pvAuctionInfo ) )
		{
			pvAuctionInfo = new AuctionVector;
			m_hsBiddedAuctionList.add( pAuctionInfo->nHighestBidderUID, pvAuctionInfo );
		}
		pvAuctionInfo->push_back( pAuctionInfo );
	}

	return true;
}

bool AuctionManager::AddItemKeepingInfoByLoading( ItemKeepingInfo *pItemKeeping )
{
	if( !pItemKeeping )
	{
		return false;
	}

	THREAD_SYNCHRONIZE( m_ItemKeepingLock );

	// 이미 등록된 ItemKeepingUID 중복 사용 체크
	if( m_hsItemKeepingList.lookup( pItemKeeping->nKeepingID ) )
	{
		return false;
	}

	// 아이템 보관 데이터 검색용 벡터/해쉬 추가
	return ( addItemKeepingInfoToIndex( pItemKeeping ) == RESULT_SUCCESS );
}

bool AuctionManager::Init()
{
	THREAD_SYNCHRONIZE( m_AuctionLock );

	// 1분에 한 번씩 onProcess 처리
	ArcadiaServer::Instance().SetObjectPriority( &AuctionManager::Instance(), ArSchedulerObject::UPDATE_PRIORITY_LOW );

	return true;
}

bool AuctionManager::DeInit()
{
	ArcadiaServer::Instance().SetObjectPriority( &AuctionManager::Instance(), ArSchedulerObject::UPDATE_PRIORITY_IDLE );

	return true;
}

unsigned short AuctionManager::SearchAndSendAuctionList( const StructPlayer *pPlayer, const int category_id, const int sub_category_id, const char *szKeyword, const int nPageNum, const bool bIsEquipable )
{
	THREAD_SYNCRONIZE( m_AuctionLock );

	// 파라미터 유효성 체크
	if( !pPlayer || category_id < CATEGORY_SPECIAL || category_id > m_nMaxCategoryIndex || sub_category_id < SUB_CATEGORY_NONE || nPageNum < 1 || nPageNum > static_cast< int >( m_hsAuctionListByAuctionID.size() / GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_SEARCHED ) + 1 )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	// 전체 또는 기타 카테고리는 category_id: CATEGORY_SPECIAL, sub_category_id: SUB_CATEGORY_ALL 또는 SUB_CATEGORY_ETC 이어야 함
	if( category_id == CATEGORY_SPECIAL && sub_category_id != SUB_CATEGORY_ALL && sub_category_id != SUB_CATEGORY_ETC )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	int nCategoryID = category_id;
	int nSubCategoryID = sub_category_id;

	// 기타 카테고리일 경우 카테고리 값 변경 처리
	if( nCategoryID == CATEGORY_SPECIAL && nSubCategoryID == SUB_CATEGORY_ETC )
	{
		nCategoryID = m_nMaxCategoryIndex;
		nSubCategoryID = SUB_CATEGORY_ALL;
	}

	// 검색 카테고리 확인(하위 카테고리 검색일 때에만 pCategory를 세팅함)
	const AuctionCategoryInfo *pCategory = NULL;
	if( nSubCategoryID != SUB_CATEGORY_ALL )
	{
		for( AuctionCategoryVecConstIterator itParentCategory = m_vAuctionCategory.begin() ; itParentCategory != m_vAuctionCategory.end() ; ++itParentCategory )
		{
			if( (*itParentCategory).nCategoryID == nCategoryID )
			{
				if( nSubCategoryID == SUB_CATEGORY_ALL )
				{
					pCategory = &(*itParentCategory);
					break;
				}

				for( AuctionCategoryVecConstIterator itChildCategory = (*itParentCategory).vChildAuctionCategory.begin() ; itChildCategory != (*itParentCategory).vChildAuctionCategory.end() ; ++itChildCategory )
				{
					if( (*itChildCategory).nSubCategoryID == nSubCategoryID )
					{
						pCategory = &(*itChildCategory);
						break;
					}
				}

				if( !pCategory )
				{
					// 요청된 nSubCategoryID에 해당하는 AuctionCategoryInfo 가 없음
					return RESULT_INVALID_ARGUMENT;
				}

				break;
			}
		}

		if( !pCategory )
		{
			// 요청된 nCategoryID에 해당하는 AuctionCategoryInfo 가 없음
			return RESULT_INVALID_ARGUMENT;
		}
	}

	// 여기서 pCategory == NULL 이면 전체, 또는 기타 카테고리 검색임

	int nValidCount = 0;
	int nTotalSearchedCount = 0;
	int nStartIndex = ( nPageNum - 1 ) * GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_SEARCHED + 1;

	TS_SC_AUCTION_SEARCH msg;

	for( int i = 0 ; i <= m_nMaxCategoryIndex ; ++i )
	{
		if( nCategoryID != CATEGORY_SPECIAL && i != nCategoryID )
			continue;

		for( AuctionListIterator it = m_vplAuctionListByCategory[i]->begin() ; it != m_vplAuctionListByCategory[i]->end() ; ++it )
		{
			// 시크루트 전용 경매의 경우 시크루트 프리패스 유저 체크
			if( (*it)->bSecrouteOnly && !pPlayer->IsGaiaMember() )
			{
				continue;
			}

			// 하위 카테고리 검색시 카테고리 조건 체크(상위 카테고리 검색이라면 리스트 전체를 그냥 주면 됨)
			// 전체/기타 카테고리 검색은 하위 카테고리가 없으므로 pCategory가 NULL임
			if( pCategory && !pCategory->IsOverlappedItemCategory( (*it)->pItem->GetItemGroup(), (*it)->pItem->GetItemClass() ) )
			{
				continue;
			}

			// 검색 키워드 조건 체크
			if( !szKeyword || !szKeyword[ 0 ] || XStringUtil::stristr( (*it)->pItem->GetNameInGame().c_str(), szKeyword ) )
			{
				// 장비 가능 조건 체크, 장비(TYPE_ARMOR)에 한해서만
				if( bIsEquipable && (*it)->pItem->IsEquipment() )
				{
					const ItemBaseServer & base = (*it)->pItem->GetItemBase();

					// 최소, 최대 레벨 및 랭크 체크
					if( base.nMaxLevel && pPlayer->GetLevel() > base.nMaxLevel ) continue;
					if( base.nMinLevel && pPlayer->GetLevel() < base.nMinLevel ) continue;
					if( pPlayer->GetLevel() < (*it)->pItem->GetLevelLimit() ) continue;

					// 직업 체크
					bool bWearable = false;

					if( base.nLimit & ItemBase::LIMIT_HUNTER && pPlayer->IsHunter() ) bWearable = true;
					if( base.nLimit & ItemBase::LIMIT_FIGHTER && pPlayer->IsFighter() ) bWearable = true;
					if( base.nLimit & ItemBase::LIMIT_MAGICIAN && pPlayer->IsMagician() ) bWearable = true;
					if( base.nLimit & ItemBase::LIMIT_SUMMONER && pPlayer->IsSummoner() ) bWearable = true;

					if( !bWearable ) continue;

					if( pPlayer->GetRace() == JobInfo::GAIA )	if( !( base.nLimit & ItemBase::LIMIT_GAIA ) ) continue;
					if( pPlayer->GetRace() == JobInfo::DEVA )	if( !( base.nLimit & ItemBase::LIMIT_DEVA ) ) continue;
					if( pPlayer->GetRace() == JobInfo::ASURA )	if( !( base.nLimit & ItemBase::LIMIT_ASURA ) ) continue;

					// 직업 차수 체크
					if( ( 1 << pPlayer->GetJobDepth() & base.nJobDepth ) == 0 ) continue;
				}

				if( ++nTotalSearchedCount >= nStartIndex && nValidCount < GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_SEARCHED )
				{
					fillAuctionMessageBuffer( &msg.auction_info[ nValidCount ], (*it) );
					s_strcpy( msg.auction_info[ nValidCount ].seller_name, _countof( msg.auction_info[ nValidCount ].seller_name ), (*it)->szSellerName );

					if( (*it)->nHighestBidderUID && (*it)->nHighestBidderUID == pPlayer->GetPlayerUID() )
					{
						msg.auction_info[ nValidCount ].flag |= TS_SC_AUCTION_SEARCH::SEARCHED_AUCTION_INFO::FLAG_IS_HIGHEST_BIDDER;
					}
					else if( !(*it)->nHighestBidderUID && (*it)->vBidderUID.empty() )
					{
						msg.auction_info[ nValidCount ].flag |= TS_SC_AUCTION_SEARCH::SEARCHED_AUCTION_INFO::FLAG_NO_OTHER_BIDDER;
					}

					++nValidCount;
				}
			}
		}
	}

	msg.auction_info_count = nValidCount;
	msg.page_num = nPageNum;
	msg.total_page_count = ( nTotalSearchedCount - 1 ) / GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_SEARCHED + 1;

	PendMessage( pPlayer, &msg );

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::SendRegisteredAuctionList( const StructPlayer *pPlayer, const int nPageNum )
{
	THREAD_SYNCRONIZE( m_AuctionLock );

	// 파라미터 유효성 체크
	if( !pPlayer || nPageNum < 1 )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	AuctionVector * pvRegisteredAuction = NULL;
	if( !m_hsAuctionListBySellerUID.lookup( pPlayer->GetPlayerUID(), pvRegisteredAuction ) )
	{
		return RESULT_NOT_EXIST;
	}

	int nTotalCount = static_cast< int >( pvRegisteredAuction->size() );

	if( nPageNum > nTotalCount / GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_REGISTERED + 1 )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	int nValidCount = 0;
	int nStartIndex = ( nPageNum - 1 ) * GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_REGISTERED;

	TS_SC_AUCTION_SELLING_LIST msg;

	for( AuctionVecIterator it = pvRegisteredAuction->begin() + nStartIndex ; it != pvRegisteredAuction->end() && nValidCount < GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_REGISTERED ; ++it )
	{
		fillAuctionMessageBuffer( &msg.auction_info[ nValidCount ], (*it) );
		if( !(*it)->nHighestBidderUID )
			msg.auction_info[ nValidCount ].status = TS_SC_AUCTION_SELLING_LIST::REGISTERED_AUCTION_INFO::STATUS_NO_BIDDER;
		else
			msg.auction_info[ nValidCount ].status = TS_SC_AUCTION_SELLING_LIST::REGISTERED_AUCTION_INFO::STATUS_BID_CALLING;

		++nValidCount;
	}

	msg.auction_info_count = nValidCount;
	msg.page_num = nPageNum;
	msg.total_page_count = ( nTotalCount - 1 ) / GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_REGISTERED + 1;

	PendMessage( pPlayer, &msg );

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::SendBiddedAuctionList( const StructPlayer *pPlayer, const int nPageNum )
{
	THREAD_SYNCRONIZE( m_AuctionLock );

	// 파라미터 유효성 체크
	if( !pPlayer || nPageNum < 1 )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	AuctionVector * pvBiddedAuction = NULL;
	if( !m_hsBiddedAuctionList.lookup( pPlayer->GetPlayerUID(), pvBiddedAuction ) )
	{
		return RESULT_NOT_EXIST;
	}

	int nTotalCount = static_cast< int >( pvBiddedAuction->size() );

	if( nPageNum > nTotalCount / GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_BIDDED + 1 )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	int nValidCount = 0;
	int nStartIndex = ( nPageNum - 1 ) * GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_BIDDED;

	TS_SC_AUCTION_BIDDED_LIST msg;

	for( AuctionVecIterator it = pvBiddedAuction->begin() + nStartIndex ; it != pvBiddedAuction->end() && nValidCount < GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_BIDDED ; ++it )
	{
		// 시크루트 전용 경매의 경우 시크루트 프피래스 유저 체크
		if( (*it)->bSecrouteOnly && !pPlayer->IsGaiaMember() )
		{
			continue;
		}

		fillAuctionMessageBuffer( &msg.auction_info[ nValidCount ], (*it) );
		if( (*it)->nHighestBidderUID && (*it)->nHighestBidderUID == pPlayer->GetPlayerUID() )
			msg.auction_info[ nValidCount ].status = TS_SC_AUCTION_BIDDED_LIST::BIDDED_AUCTION_LIST::STATUS_MINE;
		else
			msg.auction_info[ nValidCount ].status = TS_SC_AUCTION_BIDDED_LIST::BIDDED_AUCTION_LIST::STATUS_OTHERS;

		++nValidCount;
	}

	msg.auction_info_count = nValidCount;
	msg.page_num = nPageNum;
	msg.total_page_count = ( nTotalCount - 1 ) / GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_BIDDED + 1;

	PendMessage( pPlayer, &msg );

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::RegisterItemToSell( StructPlayer *pPlayer, AR_HANDLE item_handle, const __int64 & item_count, const time_t tAuctionDuration, const StructGold & nStartPrice, const StructGold & nInstantPurchasePrice, ItemBase::ItemCode * pResultItemCode )
{
	THREAD_SYNCHRONIZE( m_AuctionLock );

	if( !pPlayer || !tAuctionDuration )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	if( !item_handle || item_count <= 0 )
	{
		return RESULT_NOT_EXIST;
	}

	StructItem *pItem = StructItem::FindItem( item_handle );
	if( !pItem || pItem->GetCount() < item_count )
	{
		return RESULT_NOT_EXIST;
	}

	// 가격 수치 유효성 체크(상점 판매가 계산)
	StructGold nPricePerOne( GameRule::GetItemSellPrice( pItem->GetItemBase().nPrice, pItem->GetItemRank(), pItem->GetItemLevel(), ( pItem->GetItemCode() >= 602700 && pItem->GetItemCode() <= 602799 ), pItem->GetMaxEtherealDurability() && !pItem->GetCurrentEtherealDurability(), pItem->IsEquipment() ).GetRawData() );
	StructGold nTotalPrice = nPricePerOne * item_count;

	// 상점 판매가보다 낮게 등록 불가
	if( nStartPrice < nTotalPrice )
	{
		return RESULT_TOO_CHEAP;
	}

	// 최소 즉구가 조건 체크: 경매 시작가의 일정 비율 이상
	// 계산 부분은 c_fixed10 의 operator * 을 호출하기 위해 순서를 바꾸면 안 됨: 정확도 문제 발생함
	// StructGold 생성자 호출 부분은 소수자리 절삭 효과를 함께 가지므로 삭제하면 안 됨
	if( !!nInstantPurchasePrice && nInstantPurchasePrice < StructGold( ( GameRule::AUCTION_MIN_INSTANT_PURCHASE_PRICE_RATE * nStartPrice.GetRawData() ).GetAsInt64() ) )
	{
		return RESULT_TOO_CHEAP;
	}

	// 거래 불가 물품 등록 체크
	if( !pPlayer->IsTradable( pItem ) )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 기간제 물품 등록 체크
	if( pItem->IsExpireItem() )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 수수료율 체크 및 수수료 미리 계산
	c_fixed10 fTaxRate = 0;
	if( tAuctionDuration <= GameRule::AUCTION_DURATION_SHORTTERM )
		fTaxRate = GameRule::AUCTION_REGISTER_TAX_RATE_SHORTTERM;
	else if( tAuctionDuration <= GameRule::AUCTION_DURATION_MIDTERM )
		fTaxRate = GameRule::AUCTION_REGISTER_TAX_RATE_MIDTERM;
	else
		fTaxRate = GameRule::AUCTION_REGISTER_TAX_RATE_LONGTERM;
	StructGold nRegistrationTax( fTaxRate * nStartPrice.GetRawData() );

	// 수수료 오버플로우 체크
	if( nRegistrationTax < StructGold( 0 ) )
	{
		return RESULT_NOT_ENOUGH_MONEY;
	}

	// 거래 중에는 물품 등록 불가
	if( pPlayer->IsTrading() )
	{
		return RESULT_NOT_ACTABLE_WHILE_TRADING;
	}

	// 창고 이용 중에는 물품 등록 불가
	if( pPlayer->IsUsingStorage() )
	{
		return RESULT_NOT_ACTABLE_WHILE_USING_STORAGE;
	}

	// 노점 이용 중에는 물품 등록 불가
	if( pPlayer->IsBoothOpen() || pPlayer->IsBoothWatching() )
	{
		return RESULT_NOT_ACTABLE_WHILE_USING_BOOTH;
	}

	// 소유권 체크
	if( pPlayer->GetHandle() != pItem->GetOwnerHandle() || !pItem->IsInInventory() )
	{
		return RESULT_NOT_OWN;
	}

	// 장착 중인 아이템 등록 불가
	if( pItem->GetWearInfo() != ItemBase::WEAR_NONE )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 편성 중인 소환수 카드, 벨트 장착 중인 소환수 카드 등록 불가
	for( int nBindIndex = 0 ; nBindIndex < 6 ; ++nBindIndex )
	{
		if( pPlayer->GetSummonCardAt( nBindIndex ) == pItem || pPlayer->GetBeltSlotCardAt( nBindIndex ) == pItem )
		{
			return RESULT_NOT_ACTABLE;
		}
	}

	// 수수료 소지 여부 체크
	if( pPlayer->GetGold() < nRegistrationTax )
	{
		return RESULT_NOT_ENOUGH_MONEY;
	}

	// 등록 가능 카테고리 확인
	// m_nMaxCategoryIndex가 반환된 경우는 카테고리 분류가 등록되어 있지 않은 품목(m_nMaxCategoryIndex 자체가 기타 카테고리 인덱스)
	int nIndex = getCategoryIndex( static_cast< ItemBase::ITEM_GROUP >( pItem->GetItemGroup() ), static_cast< ItemBase::ItemClass >( pItem->GetItemClass() ) );
	if( nIndex <= CATEGORY_SPECIAL || nIndex > m_nMaxCategoryIndex )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 로그 메시지용 변수(최초 소지금)
	StructGold nPrevGold = pPlayer->GetGold();

	// 등록금 삭감
	if( pPlayer->ChangeGold( pPlayer->GetGold() - nRegistrationTax ) != RESULT_SUCCESS )
	{
		return RESULT_NOT_ENOUGH_MONEY;
	}

	ItemUID nItemUID = pItem->GetItemUID();
	if( pItem->IsJoinable() && pItem->GetCount() > item_count )
	{
		nItemUID = StructPlayer::allocItemUID();
	}
	else
	{
		// 중복 등록 체크(ItemUID 기준)
		AuctionList * plAuctionInfo = m_vplAuctionListByCategory[ nIndex ];
		for( AuctionListIterator it = plAuctionInfo->begin() ; it != plAuctionInfo->end() ; ++it )
		{
			if( nItemUID == (*it)->pItem->GetItemUID() )
			{
				return RESULT_ALREADY_EXIST;
			}
		}
	}

	// 아이템 데이터 갱신(경매장 내의 아이템 데이터는 저장되는 일이 없으므로 미리 저장되어야 함)
	// 아이템 분할 등록의 경우 기존 아이템만, 통째 등록인 경우 해당 아이템 자체 저장
	pItem->DBQuery( new DB_UpdateItem( pItem ) );

	// 로그 메시지용 변수(최초 아이템 소지량)
	__int64 nPrevItemCount = pItem->GetCount();

	// 플레이어 등록일 경우 인벤토리에서 아이템 꺼냄
	StructItem *pAuctionItem = pPlayer->PopItem( pItem, item_count, true );
	// 인벤에서 아이템 꺼내는 거 실패
	if( !pAuctionItem )
	{
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	bool bItemSplited = ( pAuctionItem != pItem );

	if( bItemSplited )
	{
		pAuctionItem->SetItemUID( nItemUID );

		// 인벤토리에 남아있는 아이템의 Count를 DB에 Update 하는 것과
		// 분할된 아이템을 DB에 Insert 하는 것이 동시에 일어나야 하므로
		// 여기서 업데이트되지 않도록 방지
		pItem->TurnOffDbUpdateFlag();
	}

	AuctionInfo *pAuctionInfo = new AuctionInfo( pAuctionItem, pPlayer->GetPlayerUID(), pPlayer->GetName(),
		false, tAuctionDuration, nStartPrice, nInstantPurchasePrice, nRegistrationTax );

	// 아이템 소유권 정보, 경매/보관 ID 변경
	pAuctionItem->SetOwnerInfo( 0, 0, 0 );
	pAuctionItem->SetAuctionID( pAuctionInfo->nAuctionID );
	pAuctionItem->SetItemKeepingID( 0 );

	// 경매 데이터 검색용 벡터/해쉬 추가
	addAuctionInfoToIndex( pAuctionInfo, true );

	// 경매 데이터 DB 추가
	// 아이템이 분할되어 새로 생성된 거라면 Item 테이블에 Insert와 기존 아이템의 cnt Update를,
	// 기존에 있던 거라면 owner_id, account_id, auction_id, keeping_id Update를 함께 처리함
	Push( new DB_InsertAuctionInfo( pAuctionInfo, ( bItemSplited ) ? pItem->GetItemUID() : 0, ( bItemSplited ) ? pItem->GetCount() : 0 ) );

	// 등록자 수수료 차감 상태를 DB에 가능한 바로 저장해주기 위해 Save 호출(DB_InsertAuctionInfo에서 같이 하게 하기엔 너무 많은 고려 사항이...)
	pPlayer->Save( true );

	// 시스템 메시지 출력에서 필요한 Item Code를 함수 호출부로 전달하기 위해 값 세팅
	if( pResultItemCode )
	{
		*pResultItemCode = pAuctionInfo->pItem->GetItemCode();
	}

	// 로그 메시지 기록
	if( bItemSplited )
	{
		LOG::Log11N4S( LM_AUCTION_BEGIN_ITEM_SPLITED, pItem->GetItemCode(), pPlayer->GetPlayerUID(), pAuctionInfo->nAuctionID,
			nPrevItemCount, item_count, pAuctionItem->GetCount(), 0, 0, 0, nItemUID, pItem->GetItemUID(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, pAuctionItem->GetName(), LOG::STR_NTS, "", 0 );
	}
	LOG::Log11N4S( LM_AUCTION_BEGIN, pAuctionItem->GetItemCode(), pPlayer->GetPlayerUID(), pAuctionInfo->nAuctionID,
		nStartPrice.GetRawData(), nInstantPurchasePrice.GetRawData(), tAuctionDuration ,
		nPrevGold.GetRawData(), nRegistrationTax.GetRawData(), pPlayer->GetGold().GetRawData(),
		pAuctionItem->GetCount(), pAuctionItem->GetItemUID(),
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::RegisterItemToSell( const char *pszSellerName, AR_HANDLE item_handle, const bool bSecrouteOnly, const time_t tAuctionDuration, const StructGold & nStartPrice, const StructGold & nInstantPurchasePrice, ItemBase::ItemCode * pResultItemCode )
{
	THREAD_SYNCHRONIZE( m_AuctionLock );

	if( !pszSellerName || !tAuctionDuration )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	if( !item_handle )
	{
		return RESULT_NOT_EXIST;
	}

	StructItem *pItem = StructItem::FindItem( item_handle );
	if( !pItem || !pItem->GetCount() )
	{
		return RESULT_NOT_EXIST;
	}

	// 상점 판매가보다 낮게 등록 가능(시스템 등록이니까 봐주기)

	// 최소 즉구가 조건 체크: 경매 시작가의 일정 비율 이상
	// 계산 부분은 c_fixed10 의 operator * 을 호출하기 위해 순서를 바꾸면 안 됨: 정확도 문제 발생함
	// StructGold 생성자 호출 부분은 소수자리 절삭 효과를 함께 가지므로 삭제하면 안 됨
	if( !!nInstantPurchasePrice && nInstantPurchasePrice < StructGold( ( GameRule::AUCTION_MIN_INSTANT_PURCHASE_PRICE_RATE * nStartPrice.GetRawData() ).GetAsInt64() ) )
	{
		return RESULT_TOO_CHEAP;
	}

	// 거래 불가 물품 등록 체크
	if( !pItem->IsTradable() )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 기간제 물품 등록 체크
	if( pItem->IsExpireItem() )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 강화 실패작 등록 체크
	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 테이밍 시도 중인 소환수 카드는 거래 불가
	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_TAMING ) )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 등록 가능 카테고리 확인
	// m_nMaxCategoryIndex가 반환된 경우는 카테고리 분류가 등록되어 있지 않은 품목이므로 CATEGORY_INDEX_ETC로 변경
	int nIndex = getCategoryIndex( static_cast< ItemBase::ITEM_GROUP >( pItem->GetItemGroup() ), static_cast< ItemBase::ItemClass >( pItem->GetItemClass() ) );
	if( nIndex <= CATEGORY_SPECIAL || nIndex > m_nMaxCategoryIndex )
	{
		return RESULT_NOT_ACTABLE;
	}

	ItemUID nItemUID = pItem->GetItemUID();

	// 중복 등록 체크(ItemUID 기준)
	AuctionList * plAuctionInfo = m_vplAuctionListByCategory[ nIndex ];
	for( AuctionListIterator it = plAuctionInfo->begin() ; it != plAuctionInfo->end() ; ++it )
	{
		if( nItemUID == (*it)->pItem->GetItemUID() )
		{
			return RESULT_ALREADY_EXIST;
		}
	}

	AuctionInfo *pAuctionInfo = new AuctionInfo( pItem, 0, ( pszSellerName ) ? pszSellerName : "@AUCTION" ,
		bSecrouteOnly, tAuctionDuration, nStartPrice, nInstantPurchasePrice, StructGold( 0 ) );

	// 아이템 소유권 정보, 경매/보관 ID 변경
	pItem->SetOwnerInfo( 0, 0, 0 );
	pItem->SetAuctionID( pAuctionInfo->nAuctionID );
	pItem->SetItemKeepingID( 0 );

	// 경매 데이터 검색용 벡터/해쉬 추가
	addAuctionInfoToIndex( pAuctionInfo );

	// 경매 데이터 DB 추가
	// 아이템이 새로 생성된 것이므로 Item 테이블에 pAuctionInfo->pItem을 추가하고
	// AuctionInfo에 경매 정보 추가 됨
	Push( new DB_InsertAuctionInfo( pAuctionInfo ) );

	// 시스템 메시지 출력에서 필요한 Item Code를 함수 호출부로 전달하기 위해 값 세팅
	if( pResultItemCode )
	{
		*pResultItemCode = pAuctionInfo->pItem->GetItemCode();
	}

	// 로그 메시지 기록
	LOG::Log11N4S( LM_AUCTION_BEGIN, pItem->GetItemCode(), 0, pAuctionInfo->nAuctionID,
		nStartPrice.GetRawData(), nInstantPurchasePrice.GetRawData(), tAuctionDuration,
		0, 0, 0,
		pItem->GetCount(), pItem->GetItemUID(),
		"AutoRegister", LOG::STR_NTS, pAuctionInfo->szSellerName, LOG::STR_NTS, "", 0, "", 0 );

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::BidForAuction( StructPlayer *pPlayer, const AuctionUID nAuctionID, const StructGold & nBiddingPrice, ItemBase::ItemCode & nResultItemCode )
{
	// 입찰자 또는 경매ID 유효성 체크
	if( !pPlayer || !nAuctionID || nBiddingPrice <= StructGold( 0 ) )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	// 거래 중에는 입찰 불가
	if( pPlayer->IsTrading() )
	{
		return RESULT_NOT_ACTABLE_WHILE_TRADING;
	}

	// 창고 이용 중에는 입찰 불가
	if( pPlayer->IsUsingStorage() )
	{
		return RESULT_NOT_ACTABLE_WHILE_USING_STORAGE;
	}

	// 노점 이용 중에는 입찰 불가
	if( pPlayer->IsBoothOpen() || pPlayer->IsBoothWatching() )
	{
		return RESULT_NOT_ACTABLE_WHILE_USING_BOOTH;
	}

	// 소지금 체크
	StructGold nOwnedGold( pPlayer->GetGold() );
	if( nBiddingPrice > nOwnedGold )
	{
		return RESULT_NOT_ENOUGH_MONEY;
	}

	THREAD_SYNCRONIZE( m_AuctionLock );

	AuctionInfo *pAuctionInfo = NULL;
	// 존재하지 않는 경매ID 체크
	if( !m_hsAuctionListByAuctionID.lookup( nAuctionID, pAuctionInfo ) )
	{
		return RESULT_NOT_EXIST;
	}

	// 자신이 등록한 경매에 입찰 불가
	PlayerUID nNewBidderUID = pPlayer->GetPlayerUID();
	if( pAuctionInfo->nSellerUID == nNewBidderUID )
	{
		return RESULT_ACCESS_DENIED;
	}

	// 자신이 최고 입찰자인 경매에 입찰 불가
	if( pAuctionInfo->nHighestBidderUID && pAuctionInfo->nHighestBidderUID == nNewBidderUID )
	{
		return RESULT_ACCESS_DENIED;
	}

	// 시크루트 전용 경매의 경우 시크루트 프피래스 유저 체크
	if( pAuctionInfo->bSecrouteOnly && !pPlayer->IsGaiaMember() )
	{
		return RESULT_NOT_EXIST;
	}

	// 입찰가가 기존 최고 입찰가 대비 최저 제한가보다 낮은지 체크
	// 계산 부분은 c_fixed10 의 operator * 을 호출하기 위해 순서를 바꾸면 안 됨: 정확도 문제 발생함
	// StructGold 생성자 호출 부분은 소수자리 절삭 효과를 함께 가지므로 삭제하면 안 됨
	// 단, 최초 입찰시에는 적용되지 않음(경매 시작가로 1회 입찰 가능)
	if( ( pAuctionInfo->nHighestBidderUID && nBiddingPrice < StructGold( ( GameRule::AUCTION_MIN_BIDDING_PRICE_RATE * pAuctionInfo->nHighestBiddingPrice.GetRawData() ).GetAsInt64() ) )
		|| nBiddingPrice < pAuctionInfo->nHighestBiddingPrice )
	{
		return RESULT_TOO_CHEAP;
	}

	// 즉구가보다 높게는 입찰 불가(즉구가가 0이면 즉구 불가능이므로 허용)
	if( !!pAuctionInfo->nInstantPurchasePrice && pAuctionInfo->nInstantPurchasePrice < nBiddingPrice )
	{
		return RESULT_TOO_MUCH_MONEY;
	}

	// 소지금 삭감
	if( pPlayer->ChangeGold( pPlayer->GetGold() - nBiddingPrice ) != RESULT_SUCCESS )
	{
		return RESULT_NOT_ENOUGH_MONEY;
	}

	// 로그 메시지 작성용 변수
	PlayerUID nPrevHighestBidderUID = 0;
	std::string strPrevHighestBidderName;
	StructGold nPrevHighestBiddingPrice( 0 );

	// 기존 최고 입찰자 입찰금 환급
	if( pAuctionInfo->nHighestBidderUID )
	{
		nPrevHighestBidderUID = pAuctionInfo->nHighestBidderUID;
		strPrevHighestBidderName = pAuctionInfo->szHighestBidderName;
		nPrevHighestBiddingPrice = pAuctionInfo->nHighestBiddingPrice;

		// ItemKeepingInfo 해쉬에 추가
		THREAD_SYNCHRONIZE1( m_ItemKeepingLock );

		StructItem *pGold = StructItem::AllocGold( nPrevHighestBiddingPrice, ItemInstance::BY_AUCTION );
		pGold->SetItemUID( StructPlayer::allocItemUID() );
		ItemKeepingInfo *pReturnBiddedGold = new ItemKeepingInfo( pGold, nPrevHighestBidderUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_GOLD_BY_HIGHER_BID,
			pAuctionInfo->nAuctionID, pAuctionInfo->pItem->GetItemCode(), pAuctionInfo->pItem->GetItemEnhance(), pAuctionInfo->pItem->GetItemLevel() );

		// 아이템 소유권 정보, 경매/보관 ID 변경
		pGold->SetOwnerInfo( 0, 0, 0 );
		pGold->SetAuctionID( 0 );
		pGold->SetItemKeepingID( pReturnBiddedGold->nKeepingID );

		addItemKeepingInfoToIndex( pReturnBiddedGold );

		// 아이템 보관 데이터 DB에 추가
		Push( new DB_InsertNewItemAndKeepingInfo( pReturnBiddedGold ) );

		// 로그 메시지 기록
		LOG::Log11N4S( LM_KEEPING_BEGIN, 0, nPrevHighestBidderUID, pReturnBiddedGold->nKeepingID, pAuctionInfo->nAuctionID,
			pReturnBiddedGold->eType, pReturnBiddedGold->pItem->GetItemCode(), pReturnBiddedGold->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
			0, 0, pReturnBiddedGold->pItem->GetItemUID(),
			"", 0, pAuctionInfo->szHighestBidderName, LOG::STR_NTS, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS );

		// 상위입찰 발생 안내 시스템 메시지 송신
		AR_HANDLE handle = StructPlayer::FindPlayer( pAuctionInfo->szHighestBidderName );
		StructPlayer::iterator pit = StructPlayer::get( handle );
		StructPlayer *pPrevHighestBidder = *pit;
		if( pPrevHighestBidder && pPrevHighestBidder->GetPlayerUID() == nPrevHighestBidderUID )
		{
			PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pPrevHighestBidder, "@713\v#@itemname@#\v@%d", pAuctionInfo->pItem->GetItemBase().nNameId );
		}
	}

	// 입찰 데이터 갱신
	pAuctionInfo->nHighestBiddingPrice = nBiddingPrice;
	pAuctionInfo->nHighestBidderUID = nNewBidderUID;
	// 기존 최고 입찰자를 입찰자 목록에 추가(기존 최고 입찰자는 절대로 vBidderUID에 속해있지 않음)
	if( nPrevHighestBidderUID )
		pAuctionInfo->vBidderUID.push_back( nPrevHighestBidderUID );
	s_strcpy( pAuctionInfo->szHighestBidderName, _countof( pAuctionInfo->szHighestBidderName ), pPlayer->GetName() );

	// 현재 경매에 이미 입찰했던 유저인지 확인
	std::vector< PlayerUID >::iterator itPrevBid = std::find( pAuctionInfo->vBidderUID.begin(), pAuctionInfo->vBidderUID.end(), nNewBidderUID );
	if( itPrevBid == pAuctionInfo->vBidderUID.end() )
	{

		// 새로운 입찰자로 추가되는 경우에만 BidderUID 기준 검색 해쉬에 데이터가 추가되어야 함
		// (현재 경매에 이미 입찰한 적이 있다면 BidderUID 기준 해쉬에 이미 등록되어 있음)
		// BidderUID 기준 검색 해쉬 갱신(기존에 어떠한 경매에도 입찰했던 기록이 없다면 벡터 추가)
		AuctionVector *pvAuctionList = NULL;
		if( !m_hsBiddedAuctionList.lookup( nNewBidderUID, pvAuctionList ) )
		{
			pvAuctionList = new AuctionVector;
			m_hsBiddedAuctionList.add( nNewBidderUID, pvAuctionList );
		}
		pvAuctionList->push_back( pAuctionInfo );
	}
	else
	{
		// 입찰한 적 있었으니 기존 입찰자 목록에서 삭제하고 최고 입찰자로 설정(순서 때문에 back과 바꿔치기하면 안 됨)
		pAuctionInfo->vBidderUID.erase( itPrevBid );
	}

	// 입찰자 목록은 최대 길이 만큼만 보존(DB 저장시 스트링화에서 길이 초과 방지)
	while( pAuctionInfo->vBidderUID.size() > GameRule::AUCTION_MAX_BIDDER_LIST_LENGTH )
	{
		PlayerUID nExpiredBidderUID = pAuctionInfo->vBidderUID.front();

		// 구매자별 벡터에서 포인터 제거
		AuctionVector * pvAuctionInfo = NULL;
		if( m_hsBiddedAuctionList.lookup( nExpiredBidderUID, pvAuctionInfo ) )
		{
			bool bNeedToDeleteAuctionInfo = false;

			for( AuctionVecIterator itAuction = pvAuctionInfo->begin() ; itAuction != pvAuctionInfo->end() ; ++itAuction )
			{
				if( (*itAuction) == pAuctionInfo )
				{
					vector_fast_erase( pvAuctionInfo, itAuction );

					if( pvAuctionInfo->empty() )
					{
						m_hsBiddedAuctionList.erase( nExpiredBidderUID );
						bNeedToDeleteAuctionInfo = true;
					}

					break;
				}
			}

			if( bNeedToDeleteAuctionInfo )
			{
				delete pvAuctionInfo;
			}
		}

		pAuctionInfo->vBidderUID.erase( pAuctionInfo->vBidderUID.begin() );
	}

	// 경매 데이터 DB 갱신
	Push( new DB_UpdateAuctionBiddingInfo( pAuctionInfo->nAuctionID, pAuctionInfo->vBidderUID, pAuctionInfo->nHighestBiddingPrice, pAuctionInfo->nHighestBidderUID, pAuctionInfo->szHighestBidderName ) );

	// 입찰자 루피 변화를 DB에 가능한 빨리 적용하도록 Save 호출(DB_UpdateAuctionBiddingInfo 에서 같이 하게 하기엔 너무 많은 일을 해서...)
	pPlayer->Save( true );

	// 시스템 메시지 출력에서 필요한 Item Code를 함수 호출부로 전달하기 위해 값 세팅
	nResultItemCode = pAuctionInfo->pItem->GetItemCode();

	// 로그 메시지 기록
	LOG::Log11N4S( LM_AUCTION_BID, pAuctionInfo->pItem->GetItemCode(), pPlayer->GetPlayerUID(), pAuctionInfo->nAuctionID,
		nPrevHighestBidderUID, nPrevHighestBiddingPrice.GetRawData(),
		pAuctionInfo->nHighestBidderUID, pAuctionInfo->nHighestBiddingPrice.GetRawData(),
		0, 0, pAuctionInfo->pItem->GetCount(), pAuctionInfo->pItem->GetItemUID(),
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, strPrevHighestBidderName.c_str(), LOG::STR_NTS, "", 0 );

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::InstantPurchase( StructPlayer *pPlayer, const AuctionUID nAuctionID, ItemBase::ItemCode & nResultItemCode )
{
	// 구매자, 경매 ID 체크
	if( !pPlayer || !nAuctionID )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	// 거래 중에는 즉시 불가
	if( pPlayer->IsTrading() )
	{
		return RESULT_NOT_ACTABLE_WHILE_TRADING;
	}

	// 창고 이용 중에는 즉시 구매 불가
	if( pPlayer->IsUsingStorage() )
	{
		return RESULT_NOT_ACTABLE_WHILE_USING_STORAGE;
	}

	// 노점 이용 중에는 즉시 구매 불가
	if( pPlayer->IsBoothOpen() || pPlayer->IsBoothWatching() )
	{
		return RESULT_NOT_ACTABLE_WHILE_USING_BOOTH;
	}

	THREAD_SYNCRONIZE( m_AuctionLock );

	// 경매 데이터 검색
	AuctionInfo *pAuctionInfo = NULL;
	// 존재하지 않는 경매ID 체크
	if( !m_hsAuctionListByAuctionID.lookup( nAuctionID, pAuctionInfo ) )
	{
		return RESULT_NOT_EXIST;
	}

	// 자신이 등록한 경매에 입찰 불가
	PlayerUID nBuyerUID = pPlayer->GetPlayerUID();
	if( pAuctionInfo->nSellerUID == nBuyerUID )
	{
		return RESULT_ACCESS_DENIED;
	}

	// 시크루트 전용 경매의 경우 시크루트 프피래스 유저 체크
	if( pAuctionInfo->bSecrouteOnly && !pPlayer->IsGaiaMember() )
	{
		return RESULT_NOT_EXIST;
	}

	// 즉시 구매 가격이 지정되지 않은 경매(nInstantPurchasePrice == 0)는 즉구 불가
	if( !pAuctionInfo->nInstantPurchasePrice )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 즉시 구매에 필요한 금액 계산
	StructGold nRequiredGold( pAuctionInfo->nInstantPurchasePrice );
	if( pAuctionInfo->nHighestBidderUID )
	{
		PlayerUID nHighestBidderUID = pAuctionInfo->nHighestBidderUID;

		// 구매자가 최고 입찰자였다면 차액만 소모
		if( nHighestBidderUID == nBuyerUID )
		{
			nRequiredGold -= pAuctionInfo->nHighestBiddingPrice;
		}
	}

	// 오버 플로우 검사
	if( nRequiredGold < StructGold( 0 ) )
	{
		return RESULT_TOO_CHEAP;
	}

	// 소지금 체크
	if( pPlayer->GetGold() < nRequiredGold )
	{
		return RESULT_NOT_ENOUGH_MONEY;
	}

	// 소지금 삭감
	if( nRequiredGold > StructGold( 0 ) )
	{
		if( pPlayer->ChangeGold( pPlayer->GetGold() - nRequiredGold ) != RESULT_SUCCESS )
		{
			return RESULT_NOT_ENOUGH_MONEY;
		}
	}

	// 로그 메시지 작성용 변수
	PlayerUID nPrevHighestBidderUID = 0;
	std::string strPrevHighestBidderName;
	StructGold nPrevHighestBiddingPrice( 0 );

	// 구매자와 최고 입찰자가 다른 사람이면 최고 입찰자에게 입찰금 환급
	if( pAuctionInfo->nHighestBidderUID && pAuctionInfo->nHighestBidderUID != nBuyerUID )
	{
		nPrevHighestBidderUID = pAuctionInfo->nHighestBidderUID;
		strPrevHighestBidderName = pAuctionInfo->szHighestBidderName;
		nPrevHighestBiddingPrice = pAuctionInfo->nHighestBiddingPrice;

		// ItemKeepingInfo 해쉬에 추가
		THREAD_SYNCHRONIZE1( m_ItemKeepingLock );

		StructItem *pGold = StructItem::AllocGold( nPrevHighestBiddingPrice, ItemInstance::BY_AUCTION );
		pGold->SetItemUID( StructPlayer::allocItemUID() );
		ItemKeepingInfo *pReturnBiddedGold = new ItemKeepingInfo( pGold, pAuctionInfo->nHighestBidderUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_GOLD_BY_ITEM_SOLD_OUT,
			pAuctionInfo->nAuctionID, pAuctionInfo->pItem->GetItemCode(), pAuctionInfo->pItem->GetItemEnhance(), pAuctionInfo->pItem->GetItemLevel() );

		// 아이템 소유권 정보, 경매/보관 ID 변경
		pGold->SetOwnerInfo( 0, 0, 0 );
		pGold->SetAuctionID( 0 );
		pGold->SetItemKeepingID( pReturnBiddedGold->nKeepingID );

		addItemKeepingInfoToIndex( pReturnBiddedGold );

		// 아이템 보관 데이터 DB에 추가
		Push( new DB_InsertNewItemAndKeepingInfo( pReturnBiddedGold ) );

		// 로그 메시지 기록
		LOG::Log11N4S( LM_KEEPING_BEGIN, 0, nPrevHighestBidderUID, pReturnBiddedGold->nKeepingID, pAuctionInfo->nAuctionID,
			pReturnBiddedGold->eType, pReturnBiddedGold->pItem->GetItemCode(), pReturnBiddedGold->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
			0, 0, pReturnBiddedGold->pItem->GetItemUID(),
			"", 0, pAuctionInfo->szHighestBidderName, LOG::STR_NTS, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS );

		// 즉시 구매에 의한 입찰금 환불 안내 시스템 메시지 송신
		AR_HANDLE handle = StructPlayer::FindPlayer( pAuctionInfo->szHighestBidderName );
		StructPlayer::iterator pit = StructPlayer::get( handle );
		StructPlayer *pPrevHighestBidder = *pit;
		if( pPrevHighestBidder && pPrevHighestBidder->GetPlayerUID() == nPrevHighestBidderUID )
		{
			PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pPrevHighestBidder, "@997\v#@itemname@#\v@%d", pAuctionInfo->pItem->GetItemBase().nNameId );
		}
	}

	// 아이템 판매 대금
	StructGold nSoldPrice( pAuctionInfo->nInstantPurchasePrice.GetRawData() - ( GameRule::AUCTION_SELL_TAX_RATE * pAuctionInfo->nInstantPurchasePrice.GetRawData() ).GetAsInt64() );

	// 구매자에게 아이템 지급
	{
		THREAD_SYNCHRONIZE1( m_ItemKeepingLock );

		ItemKeepingInfo *pItemKeeping = new ItemKeepingInfo( pAuctionInfo->pItem, nBuyerUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_ITEM_BY_INSTANT_PURCHASE, pAuctionInfo->nAuctionID );

		// 아이템 소유권 정보, 경매/보관 ID 변경
		pAuctionInfo->pItem->SetOwnerInfo( 0, 0, 0 );
		pAuctionInfo->pItem->SetAuctionID( 0 );
		pAuctionInfo->pItem->SetItemKeepingID( pItemKeeping->nKeepingID );

		addItemKeepingInfoToIndex( pItemKeeping );
		Push( new DB_DeleteAuctionInfoAndInsertItemKeepingInfo( pAuctionInfo->nAuctionID, pItemKeeping ) );

		// 여기서 구매자의 루피 정보를 가능한 빨리 DB에 업데이트하도록 Save 한 번 해 줌
		// * 사실 DB_DeleteAuctionInfoAndInsertItemKeepingInfo랑 transaction으로 묶어서 같이 처리되는게 최고지만
		//   인간적으로 DB job 하나에서 Delete_Auction - Insert_ItemKeeping - Update_CharacterGold 이거까지 다 하긴 좀...;;
		//   Stored procedure가 게임 로직을 가져갈 기세...;;
		//   그래서 그냥 아쉬운대로 Save만 호출함.
		pPlayer->Save( true );

		// 로그 메시지 기록
		LOG::Log11N4S( LM_KEEPING_BEGIN, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), pItemKeeping->nKeepingID, pAuctionInfo->nAuctionID,
			pItemKeeping->eType, pItemKeeping->pItem->GetItemCode(), pItemKeeping->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
			0, 0, pItemKeeping->pItem->GetItemUID(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

		// 물품 등록자가 유저였을 경우
		if( pAuctionInfo->nSellerUID )
		{
			// 판매자에게 판매 대금 지급
			StructItem *pGold = StructItem::AllocGold( nSoldPrice, ItemInstance::BY_AUCTION );
			pGold->SetItemUID( StructPlayer::allocItemUID() );
			ItemKeepingInfo *pItemKeeping = new ItemKeepingInfo( pGold, pAuctionInfo->nSellerUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_GOLD_BY_ITEM_SELL,
				pAuctionInfo->nAuctionID, pAuctionInfo->pItem->GetItemCode(), pAuctionInfo->pItem->GetItemEnhance(), pAuctionInfo->pItem->GetItemLevel() );

			// 아이템 소유권 정보, 경매/보관 ID 변경
			pGold->SetOwnerInfo( 0, 0, 0 );
			pGold->SetAuctionID( 0 );
			pGold->SetItemKeepingID( pItemKeeping->nKeepingID );

			addItemKeepingInfoToIndex( pItemKeeping );
			Push( new DB_InsertNewItemAndKeepingInfo( pItemKeeping ) );

			// 로그 메시지 기록
			LOG::Log11N4S( LM_KEEPING_BEGIN, 0, pAuctionInfo->nSellerUID, pItemKeeping->nKeepingID, pAuctionInfo->nAuctionID,
				pItemKeeping->eType, pItemKeeping->pItem->GetItemCode(), pItemKeeping->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
				0, 0, pItemKeeping->pItem->GetItemUID(),
				"", 0, pAuctionInfo->szSellerName, LOG::STR_NTS, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS );

			// 판매자에게 경매 등록세 환급
			if( !!pAuctionInfo->nRegistrationTax )
			{
				pGold = StructItem::AllocGold( pAuctionInfo->nRegistrationTax, ItemInstance::BY_AUCTION );
				pGold->SetItemUID( StructPlayer::allocItemUID() );
				pItemKeeping = new ItemKeepingInfo( pGold, pAuctionInfo->nSellerUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_GOLD_BY_REG_TAX,
					pAuctionInfo->nAuctionID, pAuctionInfo->pItem->GetItemCode(), pAuctionInfo->pItem->GetItemEnhance(), pAuctionInfo->pItem->GetItemLevel() );

				// 아이템 소유권 정보, 경매/보관 ID 변경
				pGold->SetOwnerInfo( 0, 0, 0 );
				pGold->SetAuctionID( 0 );
				pGold->SetItemKeepingID( pItemKeeping->nKeepingID );

				addItemKeepingInfoToIndex( pItemKeeping );
				Push( new DB_InsertNewItemAndKeepingInfo( pItemKeeping ) );

				// 로그 메시지 기록
				LOG::Log11N4S( LM_KEEPING_BEGIN, 0, pAuctionInfo->nSellerUID, pItemKeeping->nKeepingID, pAuctionInfo->nAuctionID,
					pItemKeeping->eType, pItemKeeping->pItem->GetItemCode(), pItemKeeping->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
					0, 0, pItemKeeping->pItem->GetItemUID(),
					"", 0, pAuctionInfo->szSellerName, LOG::STR_NTS, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS );
			}

			// 즉시 구매에 의한 판매 안내 시스템 메시지 송신
			AR_HANDLE handle = StructPlayer::FindPlayer( pAuctionInfo->szSellerName );
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pSeller = *pit;
			if( pSeller && pSeller->GetPlayerUID() == pAuctionInfo->nSellerUID )
			{
				PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pSeller, "@717\v#@itemname@#\v@%d", pAuctionInfo->pItem->GetItemBase().nNameId );
			}
		}
	}

	// 시스템 메시지 출력에서 필요한 Item Code를 함수 호출부로 전달하기 위해 값 세팅
	nResultItemCode = pAuctionInfo->pItem->GetItemCode();

	// 로그 메시지 기록
	LOG::Log11N4S( LM_AUCTION_INSTANT_PURCHASE, pAuctionInfo->pItem->GetItemCode(), pPlayer->GetPlayerUID(), pAuctionInfo->nAuctionID,
		nRequiredGold.GetRawData(), pPlayer->GetGold().GetRawData(),
		nPrevHighestBidderUID, nPrevHighestBiddingPrice.GetRawData(),
		pAuctionInfo->nSellerUID, nSoldPrice.GetRawData(),
		pAuctionInfo->nRegistrationTax.GetRawData(), pAuctionInfo->pItem->GetItemUID(),
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, strPrevHighestBidderName.c_str(), LOG::STR_NTS, ( pAuctionInfo->nSellerUID ) ? pAuctionInfo->szSellerName : "", LOG::STR_NTS );

	// 경매 데이터 제거
	removeAuctionInfoFromIndex( pAuctionInfo->nAuctionID );
	delete pAuctionInfo;

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::CancelAuction( const StructPlayer *pPlayer, const int nAuctionID, ItemBase::ItemCode & nResultItemCode )
{
	THREAD_SYNCRONIZE( m_AuctionLock );

	// 경매 데이터 검색
	AuctionInfo *pAuctionInfo = NULL;
	if( !m_hsAuctionListByAuctionID.lookup( nAuctionID, pAuctionInfo ) )
	{
		return RESULT_NOT_EXIST;
	}

	// 유저 취소일 경우 취소 불가 조건 처리
	if( pPlayer )
	{
		// 경매 등록자 이외에는 취소 처리 불가
		if( pPlayer->GetPlayerUID() != pAuctionInfo->nSellerUID )
		{
			return RESULT_ACCESS_DENIED;
		}

		// 경매에 입찰한 유저가 있는 경우에는 경매 취소 불가(유저 취소일 경우)
		if( pAuctionInfo->nHighestBidderUID )
		{
			return RESULT_NOT_ACTABLE;
		}
	}

	return cancelAuction( pAuctionInfo, nResultItemCode, pPlayer->GetAccountName(), pPlayer->GetName(), pPlayer->GetPlayerUID() );
}

unsigned short AuctionManager::CancelAuction( const PlayerUID & nSellerUID, std::vector< ItemBase::ItemCode > & vResultItemCode, const StructPlayer * pCanceller )
{
	THREAD_SYNCRONIZE( m_AuctionLock );

	// 해당 유저가 등록한 경매 목록 검색
	AuctionVector * pvAuction = NULL;
	if( !m_hsAuctionListBySellerUID.lookup( nSellerUID, pvAuction ) )
	{
		return RESULT_NOT_EXIST;
	}

	// pvAuction은 cancelAuction 함수 안에서 호출되는 removeAuctionInfoFromIndex 에 의해 변경되므로 포인터들을 보관하는 사본 벡터를 두고 순회해야 함
	AuctionVector vAuction( *pvAuction );

	for( AuctionVecIterator itAuction = vAuction.begin() ; itAuction != vAuction.end() ; ++itAuction )
	{
		ItemBase::ItemCode nResultItemCode = 0;
		if( cancelAuction( (*itAuction), nResultItemCode, ( pCanceller ) ? pCanceller->GetAccountName() : NULL, ( pCanceller ) ? pCanceller->GetName() : NULL, ( pCanceller ) ? pCanceller->GetPlayerUID() : 0 ) == RESULT_SUCCESS )
		{
			vResultItemCode.push_back( nResultItemCode );
		}
	}

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::SendKeepingItemList( const StructPlayer *pPlayer, const int nPageNum )
{
	THREAD_SYNCRONIZE( m_ItemKeepingLock );

	// 파라미터 유효성 체크
	if( !pPlayer || nPageNum < 1 )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	ItemKeepingVector * pvItemKeepingList = NULL;
	if( !m_hsItemKeepingList.lookup( pPlayer->GetPlayerUID(), pvItemKeepingList ) )
	{
		return RESULT_NOT_EXIST;
	}

	int nTotalCount = static_cast< int >( pvItemKeepingList->size() );

	if( nPageNum > nTotalCount / GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_ITEM_KEEPING + 1 )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	int nValidCount = 0;
	int nStartIndex = ( nPageNum - 1 ) * GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_ITEM_KEEPING;

	TS_SC_ITEM_KEEPING_LIST msg;

	for( ItemKeepingVecIterator it = pvItemKeepingList->begin() + nStartIndex ; it != pvItemKeepingList->end() && nValidCount < GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_ITEM_KEEPING ; ++it )
	{
		msg.keeping_info[ nValidCount ].keeping_uid = (*it)->nKeepingID;
		msg.keeping_info[ nValidCount ].duration = (*it)->tExpiration - time( NULL );
		fillItemBaseInfo( &msg.keeping_info[ nValidCount ].item_info, (*it)->pItem );
		msg.keeping_info[ nValidCount ].keeping_type = (*it)->eType;
		msg.keeping_info[ nValidCount ].related_item_code = (*it)->nRelatedItemCode;
		msg.keeping_info[ nValidCount ].related_item_enhance = (*it)->nRelatedItemEnhance;
		msg.keeping_info[ nValidCount ].related_item_level = (*it)->nRelatedItemLevel;

		++nValidCount;
	}

	msg.keeping_info_count = nValidCount;
	msg.page_num = nPageNum;
	msg.total_page_count = ( nTotalCount - 1 ) / GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_ITEM_KEEPING + 1;

	PendMessage( pPlayer, &msg );

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::TakeKeepedItem( StructPlayer *pPlayer, const ItemKeepingUID nKeepingID )
{
	THREAD_SYNCRONIZE( m_ItemKeepingLock );

	// 파라미터 유효성 체크
	if( !pPlayer || !nKeepingID )
	{
		return RESULT_INVALID_ARGUMENT;
	}

	// 거래 중에는 물품 수령 불가
	if( pPlayer->IsTrading() )
	{
		return RESULT_NOT_ACTABLE_WHILE_TRADING;
	}

	// 창고 이용 중에는 물품 수령 불가
	if( pPlayer->IsUsingStorage() )
	{
		return RESULT_NOT_ACTABLE_WHILE_USING_STORAGE;
	}

	// 노점 이용 중에는 물품 수령 불가
	if( pPlayer->IsBoothOpen() || pPlayer->IsBoothWatching() )
	{
		return RESULT_NOT_ACTABLE_WHILE_USING_BOOTH;
	}

	// 아이템 보관 데이터 검색
	ItemKeepingVector *pvKeepingInfo = NULL;
	if( !m_hsItemKeepingList.lookup( pPlayer->GetPlayerUID(), pvKeepingInfo ) )
	{
		return RESULT_NOT_EXIST;
	}
	ItemKeepingInfo *pItemKeeping = NULL;
	for( ItemKeepingVecIterator it = pvKeepingInfo->begin() ; it != pvKeepingInfo->end() ; ++it )
	{
		if( (*it)->nKeepingID == nKeepingID )
		{
			pItemKeeping = (*it);
			break;
		}
	}

	if( !pItemKeeping )
	{
		return RESULT_NOT_EXIST;
	}

	// 소유권을 가지고 있는 유저인지 체크
	if( pPlayer->GetPlayerUID() != pItemKeeping->nOwnerUID )
	{
		return RESULT_ACCESS_DENIED;
	}

	// 아이템 수령자 인벤토리 무게 여유 체크
	if( pPlayer->GetWeight() + pItemKeeping->pItem->GetWeight() >= pPlayer->GetMaxWeight() )
	{
		return RESULT_TOO_HEAVY;
	}

	// 루피일 경우 수령자 루피 소지 한도 체크
	if( pItemKeeping->pItem->IsGold() && pPlayer->GetGold() + pItemKeeping->pItem->GetCount() > GameRule::MAX_GOLD_FOR_INVENTORY )
	{
		return RESULT_TOO_MUCH_MONEY;
	}

	// 아이템 소유권 정보, 경매/보관 ID 변경
	pItemKeeping->pItem->SetOwnerInfo( 0, 0, 0 );
	pItemKeeping->pItem->SetAuctionID( 0 );
	pItemKeeping->pItem->SetItemKeepingID( 0 );

	// 인벤토리로 포함될 때 정렬 관련 문제 발생을 방지하기 위하여 idx를 0으로 초기화
	pItemKeeping->pItem->SetIdx( 0 );

	// 로그 메시지 작성용 변수
	ItemUID nItemUID = pItemKeeping->pItem->GetItemUID();
	ItemBase::ItemCode nItemCode = pItemKeeping->pItem->GetItemCode();
	__int64 nItemCount = pItemKeeping->pItem->GetCount();
	ItemUID nOwnedItemUID = nItemUID;

	// 아이템 지급(아이템 소유권 메모리에서만 변경되고 DB에는 업데이트 안 됨 - 아래의 DB_DeleteItemKeepingInfo에서 트랜젝션으로 묶어서 처리함)
	StructItem * pOwnedItem = pPlayer->PushItem( pItemKeeping->pItem, nItemCount, true );

	// DB에서 보관함 정보 삭제 및 관련 아이템의 인벤토리 지급
	// * DB상에서 유저에게 아이템을 지급하지 말고 삭제해야 하는 두 가지 조건
	//   1. 루피(nItemCode == 0)인 경우
	//   2. 중첩 가능 아이템이어서 지급된 아이템이 삭제되고 기존에 있던 아이템의 수량만 변경되는 경우
	Push( new DB_DeleteItemKeepingInfo( nKeepingID, true, ( pOwnedItem && pOwnedItem != pItemKeeping->pItem ) ? pOwnedItem->GetItemUID() : 0 ) );
	// 루피가 지급된 거라면 캐릭터 정보를 업데이트해야 하므로 캐릭터 정보 저장을 한 번 해 줌
	// * 트랜젝션화 되진 않아서 섭다시 손실 가능성은 남아 있지만 문제 발생 사례가 흔하지는 않을 것으로 예상되고, 로그로 복구하기도 어렵지 않으니 패스
	if( !nItemCode )
	{
		pPlayer->Save( true );
	}

	// 로그 메시지 기록
	LOG::Log11N4S( LM_KEEPING_TAKE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), pItemKeeping->nKeepingID,
		pItemKeeping->nRelatedAuctionID, pItemKeeping->eType, nItemCode, nItemCount, ( pOwnedItem ) ? pOwnedItem->GetCount() : nItemCount, 0, pPlayer->GetGold().GetRawData(), nItemUID,
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

	if( pOwnedItem && pOwnedItem != pItemKeeping->pItem )
	{
		StructItem::PendFreeItem( pItemKeeping->pItem );
	}

	// 아이템 보관 데이터 제거
	removeItemKeepingInfoFromIndex( pItemKeeping->nOwnerUID, nKeepingID );
	delete pItemKeeping;

	return RESULT_SUCCESS;
}

void AuctionManager::ClearAutoAuctionInfo()
{
	THREAD_SYNCHRONIZE( m_AutoAuctionLock );

	for( AutoAuctionVecIterator it = m_vAutoAuction.begin() ; it != m_vAutoAuction.end() ; ++it )
	{
		delete (*it);
	}
	m_vAutoAuction.clear();
}

bool AuctionManager::AddAutoAuctionInfo( const int nAutoAuctionID, const char *szSellerName, const ItemBase::ItemCode nItemCode, const __int64 & nItemCount,
										const StructGold & nStartPrice, const StructGold & nInstantPurchasePrice, const time_t tStartTime, const time_t tAuctionDuration, const bool bSecrouteOnly,
										const bool bNeedToRepeat, const time_t tRepeatTerm )
{
	THREAD_SYNCHRONIZE( m_AutoAuctionLock );

	// 중복 AutoAuctionID 등록 체크
	for( AutoAuctionVecIterator it = m_vAutoAuction.begin() ; it != m_vAutoAuction.end() ; ++it )
	{
		if( (*it)->nAutoAuctionID == nAutoAuctionID )
			return false;
	}

	AutoAuctionInfo *pAutoAuctionInfo = new AutoAuctionInfo( nAutoAuctionID, szSellerName, nItemCode, nItemCount, nStartPrice, nInstantPurchasePrice,
		tStartTime, tAuctionDuration, bSecrouteOnly, bNeedToRepeat, tRepeatTerm );

	if( !pAutoAuctionInfo )
		return false;

	m_vAutoAuction.push_back( pAutoAuctionInfo );

	return true;
}

void AuctionManager::ClearAutoAuctionRegistrationInfo()
{
	for( AutoAuctionVecIterator it = m_vAutoAuction.begin() ; it != m_vAutoAuction.end() ; ++it )
	{
		(*it)->tLastRegisteredTime = 0;
	}
}

bool AuctionManager::SetLastAutoRegisteredTime( const int nAutoAuctionID, const time_t tLastRegisteredTime )
{
	for( AutoAuctionVecIterator it = m_vAutoAuction.begin() ; it != m_vAutoAuction.end() ; ++it )
	{
		if( (*it)->nAutoAuctionID == nAutoAuctionID )
		{
			if( (*it)->tLastRegisteredTime < tLastRegisteredTime )
			{
				(*it)->tLastRegisteredTime = tLastRegisteredTime;
			}

			return true;
		}
	}

	return false;
}

bool AuctionManager::IsRegisteredAutoAuction( const int nAutoAuctionID )
{
	for( AutoAuctionVecIterator it = m_vAutoAuction.begin() ; it != m_vAutoAuction.end() ; ++it )
	{
		if( (*it)->nAutoAuctionID == nAutoAuctionID )
		{
			return true;
		}
	}

	return false;
}

void AuctionManager::OnChangeCharacterName( const int nPlayerUID, const char *szName )
{
	THREAD_SYNCRONIZE( m_AuctionLock );

	// 자신이 등록한 경매에 대해서 이름 변경 처리
	AuctionVector * pvRegisteredAuction = NULL;
	if( m_hsAuctionListBySellerUID.lookup( nPlayerUID, pvRegisteredAuction ) )
	{
		for( AuctionVecIterator it = pvRegisteredAuction->begin() ; it != pvRegisteredAuction->end() ; ++it )
		{
			// m_hsAuctionListBySellerUID 에 들어있는 AuctionVector에 엉뚱한 AuctionInfo 포인터가 들어있다는 -_ -?
			if( (*it)->nSellerUID != nPlayerUID )
			{
				assert( 0 );
				continue;
			}

			s_strcpy( (*it)->szSellerName, _countof( (*it)->szSellerName ), szName );
		}
	}

	// 자신이 입찰한 경매 중 최고 입찰자인 경매 건에 대해서 이름 변경 처리
	AuctionVector * pvBiddedAuction = NULL;
	if( m_hsBiddedAuctionList.lookup( nPlayerUID, pvBiddedAuction ) )
	{
		for( AuctionVecIterator it = pvBiddedAuction->begin() ; it != pvBiddedAuction->end() ; ++it )
		{
			// 최고 입찰 경매만 최고 입찰자 이름 변경
			if( (*it)->nHighestBidderUID != nPlayerUID )
				continue;

			s_strcpy( (*it)->szHighestBidderName, _countof( (*it)->szHighestBidderName ), szName );
		}
	}
}

void AuctionManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "AuctionManager" );

	AR_TIME t = GetArTime();

	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "AuctionManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	time_t tCurrnet = time( NULL );

	// 아이템 보관 데이터 처리(경매보다 먼저 처리해야 효율적임)
	{
		THREAD_SYNCRONIZE( m_ItemKeepingLock );

		for(	ItemKeepingVecIterator it = m_vItemKeepingList.begin();
				m_vItemKeepingList.empty() == false && it != m_vItemKeepingList.end();
				/* 루프에서 ++it 처리 */ )
		{
			// 만료된 아이템 보관 데이터 처리
			ItemKeepingInfo *pItemKeeping = (*it);

			// 만료되지 않은 아이템 보관 데이터는 처리 안 함
			if( pItemKeeping->tExpiration > tCurrnet )
			{
				++it;
				continue;
			}

			// 아이템 보관 데이터 DB에서 제거(아이템 데이터는 공중에 뜸)
			Push( new DB_DeleteItemKeepingInfo( pItemKeeping->nKeepingID, false, 0 ) );
			Push( new DB_UpdateItemAuctionKeepingID( pItemKeeping->pItem->GetItemUID(), 0, 0 ) );

			// 로그 메시지 기록
			LOG::Log11N4S( LM_KEEPING_EXPIRED, 0, pItemKeeping->nOwnerUID, pItemKeeping->nKeepingID,
				0, pItemKeeping->eType, pItemKeeping->pItem->GetItemCode(), pItemKeeping->pItem->GetCount(), 0, 0, 0, pItemKeeping->pItem->GetItemUID(),
				"", 0, "", 0, "", 0, "", 0 );

			// 아이템 보관 데이터 제거
			removeItemKeepingInfoFromIndex( pItemKeeping->nOwnerUID, pItemKeeping->nKeepingID, true );
			StructItem::PendFreeItem( pItemKeeping->pItem );
			delete pItemKeeping;

			vector_fast_erase( &m_vItemKeepingList, it );
			// 마지막 요소와 현재 요소를 바꿔치기 했으니 현재 요소를 다시 검사해야 하므로 ++it 안 함
		}
	}

	// 자동 등록 경매 처리(경매보다 먼저 처리해야 문제있는 데이터에 의한 피해를 줄일 수 있음)
	{
		THREAD_SYNCHRONIZE( m_AutoAuctionLock );

		for( AutoAuctionVecIterator it = m_vAutoAuction.begin() ; it != m_vAutoAuction.end() ; ++it )
		{
			time_t tStartTime = (*it)->tStartTime;
			time_t tLastRegisteredTime = (*it)->tLastRegisteredTime;

			// 이번 경매 등록 시간 미리 계산
			time_t tNextRegisterTime = tStartTime;

			// 반복 등록인 경우
			if( tLastRegisteredTime > tStartTime && (*it)->bNeedToRepeat )
			{
				time_t tRepeatTerm = (*it)->tRepeatTerm;

				// 기획팀 작업자를 찾아 조질 것... -_ -;;
				if( !tRepeatTerm )
				{
					_cprint( "Invalid auto auction info detected[bNeedToRepeat == true && tRepeatTerm == 0]: AutoAuctionID(%d)\n", (*it)->nAutoAuctionID );
					FILELOG( "Invalid auto auction info detected[bNeedToRepeat == true && tRepeatTerm == 0]: AutoAuctionID(%d)", (*it)->nAutoAuctionID );
					continue;
				}

				tNextRegisterTime = tStartTime + tRepeatTerm * ( ( tLastRegisteredTime - tStartTime ) / tRepeatTerm + 1 );
			}

			// 등록할 시간이 된 자동 등록 경매일 경우 처리(반복 경매이거나 혹은 최초 등록)
			if( tNextRegisterTime <= time( NULL ) && ( (*it)->bNeedToRepeat || !tLastRegisteredTime ) )
			{
				StructItem *pItem = StructItem::AllocItem( 0, (*it)->nItemCode, (*it)->nItemCount, ItemInstance::BY_AUCTION );
				pItem->SetItemUID( StructPlayer::allocItemUID() );
				pItem->TurnOffDbUpdateFlag();

				// 로그 메시지도 요기서 남음(LM_AUCTION_BEGIN)
				unsigned short nResult = RegisterItemToSell( (*it)->szSellerName, pItem->GetHandle(), (*it)->bSecrouteOnly, (*it)->tAuctionDuration, (*it)->nStartPrice,
					(*it)->nInstantPurchasePrice, NULL );

				if( nResult != RESULT_SUCCESS )
				{
					_cprint( "Error on registering an auto auction: ErrorCode(%d), AutoAuctionID(%d)\n", nResult, (*it)->nAutoAuctionID );
					FILELOG( "Error on registering an auto auction: ErrorCode(%d), AutoAuctionID(%d)", nResult, (*it)->nAutoAuctionID );
					StructItem::PendFreeItem( pItem );
					continue;
				}

				(*it)->tLastRegisteredTime = time( NULL );
				Push( new DB_InsertAutoAuctionRegistrationInfo( (*it)->nAutoAuctionID, tNextRegisterTime, (*it)->tLastRegisteredTime ) );
			}
		}
	}

	// 경매 데이터 처리
	{
		THREAD_SYNCRONIZE( m_AuctionLock );

		for( int i = 0 ; i <= m_nMaxCategoryIndex ; ++i )
		{
			for( AuctionListIterator it = m_vplAuctionListByCategory[i]->begin() ; it != m_vplAuctionListByCategory[i]->end() ; /* 루프에서 ++it 처리 */ )
			{
				// 종료 경매 처리
				AuctionInfo *pAuctionInfo = (*it);

				// 종료되지 않은 경매는 처리 안 함
				if( pAuctionInfo->tAuctionEnd > tCurrnet )
				{
					++it;
					continue;
				}

				StructItem * pExpiredItem = NULL;

				// 입찰자가 있었다면 낙찰 처리
				if( pAuctionInfo->nHighestBidderUID )
				{
					PlayerUID nBidderUID = pAuctionInfo->nHighestBidderUID;

					THREAD_SYNCHRONIZE1( m_ItemKeepingLock );

					ItemKeepingInfo *pItemKeeping = new ItemKeepingInfo( pAuctionInfo->pItem, nBidderUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_ITEM_BY_SUCCESSFUL_BID, pAuctionInfo->nAuctionID );

					addItemKeepingInfoToIndex( pItemKeeping );
					Push( new DB_DeleteAuctionInfoAndInsertItemKeepingInfo( pAuctionInfo->nAuctionID, pItemKeeping ) );

					// 로그 메시지 기록
					LOG::Log11N4S( LM_KEEPING_BEGIN, 0, nBidderUID, pItemKeeping->nKeepingID, pAuctionInfo->nAuctionID,
						pItemKeeping->eType, pItemKeeping->pItem->GetItemCode(), pItemKeeping->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
						0, 0, pItemKeeping->pItem->GetItemUID(),
						"", 0, pAuctionInfo->szHighestBidderName, LOG::STR_NTS, "", 0, "", 0 );
					// 물품 등록자가 유저였을 경우
					if( pAuctionInfo->nSellerUID )
					{
						// 아이템 판매 대금
						StructGold nSoldPrice( pAuctionInfo->nHighestBiddingPrice.GetRawData() - ( GameRule::AUCTION_SELL_TAX_RATE * pAuctionInfo->nHighestBiddingPrice.GetRawData() ).GetAsInt64() );

						// 로그 메시지 기록(낙찰 로그)
						LOG::Log11N4S( LM_AUCTION_SUCCESSFUL_BID, pAuctionInfo->pItem->GetItemCode(), nBidderUID, pAuctionInfo->nAuctionID,
							0, pAuctionInfo->nHighestBiddingPrice.GetRawData(),
							0, 0, pAuctionInfo->nSellerUID, nSoldPrice.GetRawData(), pAuctionInfo->nRegistrationTax.GetRawData(),
							pAuctionInfo->pItem->GetItemUID(),
							"", 0, pAuctionInfo->szHighestBidderName, LOG::STR_NTS, pAuctionInfo->szSellerName, LOG::STR_NTS, "", 0 );

						// 판매자에게 판매 대금 지급
						StructItem *pGold = StructItem::AllocGold( nSoldPrice, ItemInstance::BY_AUCTION );
						pGold->SetItemUID( StructPlayer::allocItemUID() );
						ItemKeepingInfo *pItemKeeping = new ItemKeepingInfo( pGold, pAuctionInfo->nSellerUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_GOLD_BY_ITEM_SELL,
							pAuctionInfo->nAuctionID, pAuctionInfo->pItem->GetItemCode(), pAuctionInfo->pItem->GetItemEnhance(), pAuctionInfo->pItem->GetItemLevel() );

						// 아이템 소유권 정보, 경매/보관 ID 변경
						pGold->SetOwnerInfo( 0, 0, 0 );
						pGold->SetAuctionID( 0 );
						pGold->SetItemKeepingID( pItemKeeping->nKeepingID );

						addItemKeepingInfoToIndex( pItemKeeping );
						Push( new DB_InsertNewItemAndKeepingInfo( pItemKeeping ) );

						// 로그 메시지 기록
						LOG::Log11N4S( LM_KEEPING_BEGIN, 0, pAuctionInfo->nSellerUID, pItemKeeping->nKeepingID, pAuctionInfo->nAuctionID,
							pItemKeeping->eType, pItemKeeping->pItem->GetItemCode(), pItemKeeping->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
							0, 0, pItemKeeping->pItem->GetItemUID(),
							"", 0, pAuctionInfo->szSellerName, LOG::STR_NTS, "", 0, "", 0 );

						// 판매자에게 경매 등록세 환급
						if( !!pAuctionInfo->nRegistrationTax )
						{
							pGold = StructItem::AllocGold( pAuctionInfo->nRegistrationTax, ItemInstance::BY_AUCTION );
							pGold->SetItemUID( StructPlayer::allocItemUID() );
							pItemKeeping = new ItemKeepingInfo( pGold, pAuctionInfo->nSellerUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_GOLD_BY_REG_TAX,
								pAuctionInfo->nAuctionID, pAuctionInfo->pItem->GetItemCode(), pAuctionInfo->pItem->GetItemEnhance(), pAuctionInfo->pItem->GetItemLevel() );

							// 아이템 소유권 정보, 경매/보관 ID 변경
							pGold->SetOwnerInfo( 0, 0, 0 );
							pGold->SetAuctionID( 0 );
							pGold->SetItemKeepingID( pItemKeeping->nKeepingID );

							addItemKeepingInfoToIndex( pItemKeeping );
							Push( new DB_InsertNewItemAndKeepingInfo( pItemKeeping ) );

							// 로그 메시지 기록
							LOG::Log11N4S( LM_KEEPING_BEGIN, 0, pAuctionInfo->nSellerUID, pItemKeeping->nKeepingID, pAuctionInfo->nAuctionID,
								pItemKeeping->eType, pItemKeeping->pItem->GetItemCode(), pItemKeeping->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
								0, 0, pItemKeeping->pItem->GetItemUID(),
								"", 0, pAuctionInfo->szSellerName, LOG::STR_NTS, "", 0, "", 0 );
						}

						// 경매 낙찰에 의한 판매 안내 시스템 메시지 송신
						AR_HANDLE handle = StructPlayer::FindPlayer( pAuctionInfo->szSellerName );
						StructPlayer::iterator pit = StructPlayer::get( handle );
						StructPlayer *pSeller = *pit;
						if( pSeller && pSeller->GetPlayerUID() == pAuctionInfo->nSellerUID )
						{
							PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pSeller, "@717\v#@itemname@#\v@%d", pAuctionInfo->pItem->GetItemBase().nNameId );
						}
					}
					// 시스템 등록이었을 경우
					else
					{
						// 로그 메시지 기록(낙찰 로그)
						LOG::Log11N4S( LM_AUCTION_SUCCESSFUL_BID, pAuctionInfo->pItem->GetItemCode(), 0, pAuctionInfo->nAuctionID,
							nBidderUID, pAuctionInfo->nHighestBiddingPrice.GetRawData(),
							0, 0, 0, 0, 0, pAuctionInfo->pItem->GetItemUID(),
							"", 0, pAuctionInfo->szHighestBidderName, LOG::STR_NTS, pAuctionInfo->szSellerName, LOG::STR_NTS, "", 0 );
					}

					// 경매 낙찰 안내 시스템 메시지 송신
					AR_HANDLE handle = StructPlayer::FindPlayer( pAuctionInfo->szHighestBidderName );
					StructPlayer::iterator pit = StructPlayer::get( handle );
					StructPlayer *pHighestBidder = *pit;
					if( pHighestBidder && pHighestBidder->GetPlayerUID() == nBidderUID )
					{
						PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pHighestBidder, "@714\v#@itemname@#\v@%d", pAuctionInfo->pItem->GetItemBase().nNameId );
					}
				}
				// 입찰자가 없었다면 유찰 처리
				else
				{
					// 물품 판매자가 유저였을 경우 등록 물품 반환
					if( pAuctionInfo->nSellerUID )
					{
						THREAD_SYNCHRONIZE1( m_ItemKeepingLock );

						// 로그 메시지 기록(유찰 로그)
						LOG::Log11N4S( LM_AUCTION_EXPIRED, pAuctionInfo->pItem->GetItemCode(), pAuctionInfo->nSellerUID, pAuctionInfo->nAuctionID,
							0, 0, 0, 0, 0, 0, pAuctionInfo->nRegistrationTax.GetRawData(),
							pAuctionInfo->pItem->GetItemUID(),
							"", 0, pAuctionInfo->szSellerName, LOG::STR_NTS, "", 0, "", 0 );

						ItemKeepingInfo *pItemKeeping = new ItemKeepingInfo( pAuctionInfo->pItem, pAuctionInfo->nSellerUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_ITEM_BY_EXPIRATION, pAuctionInfo->nAuctionID );

						// 아이템 소유권 정보, 경매/보관 ID 변경
						pItemKeeping->pItem->SetOwnerInfo( 0, 0, 0 );
						pItemKeeping->pItem->SetAuctionID( 0 );
						pItemKeeping->pItem->SetItemKeepingID( pItemKeeping->nKeepingID );

						addItemKeepingInfoToIndex( pItemKeeping );
						Push( new DB_DeleteAuctionInfoAndInsertItemKeepingInfo( pAuctionInfo->nAuctionID, pItemKeeping ) );

						// 로그 메시지 기록
						LOG::Log11N4S( LM_KEEPING_BEGIN, 0, pAuctionInfo->nSellerUID, pItemKeeping->nKeepingID, pAuctionInfo->nAuctionID,
							pItemKeeping->eType, pItemKeeping->pItem->GetItemCode(), pItemKeeping->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
							0, 0, pItemKeeping->pItem->GetItemUID(),
							"", 0, pAuctionInfo->szSellerName, LOG::STR_NTS, "", 0, "", 0 );

						// 판매자에게 경매 등록세 환급
						StructItem *pGold = StructItem::AllocGold( pAuctionInfo->nRegistrationTax, ItemInstance::BY_AUCTION );
						pGold->SetItemUID( StructPlayer::allocItemUID() );
						pItemKeeping = new ItemKeepingInfo( pGold, pAuctionInfo->nSellerUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_GOLD_BY_REG_TAX,
							pAuctionInfo->nAuctionID, pAuctionInfo->pItem->GetItemCode(), pAuctionInfo->pItem->GetItemEnhance(), pAuctionInfo->pItem->GetItemLevel() );

						// 아이템 소유권 정보, 경매/보관 ID 변경
						pGold->SetOwnerInfo( 0, 0, 0 );
						pGold->SetAuctionID( 0 );
						pGold->SetItemKeepingID( pItemKeeping->nKeepingID );

						addItemKeepingInfoToIndex( pItemKeeping );
						Push( new DB_InsertNewItemAndKeepingInfo( pItemKeeping ) );

						// 로그 메시지 기록
						LOG::Log11N4S( LM_KEEPING_BEGIN, 0, pAuctionInfo->nSellerUID, pItemKeeping->nKeepingID, pAuctionInfo->nAuctionID,
							pItemKeeping->eType, pItemKeeping->pItem->GetItemCode(), pItemKeeping->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
							0, 0, pItemKeeping->pItem->GetItemUID(),
							"", 0, pAuctionInfo->szSellerName, LOG::STR_NTS, "", 0, "", 0 );
					}
					// 물품 판매자가 시스템이었을 경우 경매 데이터 및 아이템 데이터 제거
					else
					{
						// 로그 메시지 기록(유찰 로그)
						LOG::Log11N4S( LM_AUCTION_EXPIRED, pAuctionInfo->pItem->GetItemCode(), 0, pAuctionInfo->nAuctionID,
							0, 0, 0, 0, 0,
							0, 0, pAuctionInfo->pItem->GetItemUID(),
							"", 0, "", 0, pAuctionInfo->szSellerName, LOG::STR_NTS, "", 0 );

						Push( new DB_DeleteAuctionInfo( pAuctionInfo->nAuctionID ) );
						pExpiredItem = pAuctionInfo->pItem;
					}
				}

				// 경매 데이터 제거
				removeAuctionInfoFromIndex( pAuctionInfo->nAuctionID, true );
				delete pAuctionInfo;

				// 필요할 경우 아이템도 제거 - 시스템이 등록한 경매의 만료
				if( pExpiredItem )
				{
					StructItem::PendFreeItem( pExpiredItem );
				}

				it = m_vplAuctionListByCategory[i]->erase( it );
			}
		}
	}
}

void AuctionManager::Push( GameDBManager::DBProc* pWork )
{
	THREAD_SYNCRONIZE( m_QueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}

void AuctionManager::onEndQuery()
{
	THREAD_SYNCRONIZE( m_QueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}

unsigned short AuctionManager::cancelAuction( AuctionInfo * pAuctionInfo, ItemBase::ItemCode & nResultItemCode, const char * pszCancellerAccount, const char * pszCancellerName, const PlayerUID & nCancellerUID )
{
	// 로그 메시지 작성용 변수
	PlayerUID nHighestBidderUID = 0;
	std::string strHighestBidderName;
	StructGold nHighestBiddingPrice( 0 );

	// 입찰자가 있었던 경우 입찰금 환불 처리
	if( pAuctionInfo->nHighestBidderUID )
	{
		nHighestBidderUID = pAuctionInfo->nHighestBidderUID;
		strHighestBidderName = pAuctionInfo->szHighestBidderName;
		nHighestBiddingPrice = pAuctionInfo->nHighestBiddingPrice;

		// ItemKeepingInfo 해쉬에 추가
		THREAD_SYNCHRONIZE( m_ItemKeepingLock );

		StructItem *pGold = StructItem::AllocGold( nHighestBiddingPrice, ItemInstance::BY_AUCTION );
		pGold->SetItemUID( StructPlayer::allocItemUID() );
		ItemKeepingInfo *pReturnBiddedGold = new ItemKeepingInfo( pGold, nHighestBidderUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_GOLD_BY_CANCEL,
			pAuctionInfo->nAuctionID, pAuctionInfo->pItem->GetItemCode(), pAuctionInfo->pItem->GetItemEnhance(), pAuctionInfo->pItem->GetItemLevel() );

		// 아이템 소유권 정보, 경매/보관 ID 변경
		pGold->SetOwnerInfo( 0, 0, 0 );
		pGold->SetAuctionID( 0 );
		pGold->SetItemKeepingID( pReturnBiddedGold->nKeepingID );

		addItemKeepingInfoToIndex( pReturnBiddedGold );

		// 아이템 보관 데이터 DB에 추가
		Push( new DB_InsertNewItemAndKeepingInfo( pReturnBiddedGold ) );

		// 로그 메시지 기록
		LOG::Log11N4S( LM_KEEPING_BEGIN, 0, nHighestBidderUID, pReturnBiddedGold->nKeepingID, pAuctionInfo->nAuctionID,
			pReturnBiddedGold->eType, pReturnBiddedGold->pItem->GetItemCode(), pReturnBiddedGold->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
			0, 0, pReturnBiddedGold->pItem->GetItemUID(),
			"", 0, pAuctionInfo->szHighestBidderName, LOG::STR_NTS, ( pszCancellerAccount ) ? pszCancellerAccount : "", LOG::STR_NTS, ( pszCancellerName ) ? pszCancellerName : "", LOG::STR_NTS );

		// 경매 취소에 의한 입찰금 환불 안내 시스템 메시지 송신
		AR_HANDLE handle = StructPlayer::FindPlayer( pAuctionInfo->szHighestBidderName );
		StructPlayer::iterator pit = StructPlayer::get( handle );
		StructPlayer *pHighestBidder = *pit;
		if( pHighestBidder && pHighestBidder->GetPlayerUID() == nHighestBidderUID )
		{
			PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pHighestBidder, "@996\v#@itemname@#\v@%d", pAuctionInfo->pItem->GetItemBase().nNameId );
		}
	}

	// 물품 판매자가 유저였을 경우 물품 반환
	if( pAuctionInfo->nSellerUID )
	{
		// ItemKeepingInfo 해쉬에 추가
		THREAD_SYNCHRONIZE( m_ItemKeepingLock );

		ItemKeepingInfo *pReturnItem = new ItemKeepingInfo( pAuctionInfo->pItem, pAuctionInfo->nSellerUID, GameRule::AUCTION_ITEM_KEEP_DURATION, KEEPING_TYPE_ITEM_BY_CANCEL, pAuctionInfo->nAuctionID );

		// 아이템 소유권 정보, 경매/보관 ID 변경
		pAuctionInfo->pItem->SetOwnerInfo( 0, 0, 0 );
		pAuctionInfo->pItem->SetAuctionID( 0 );
		pAuctionInfo->pItem->SetItemKeepingID( pReturnItem->nKeepingID );

		addItemKeepingInfoToIndex( pReturnItem );

		// 경매 데이터 및 아이템 보관 데이터 DB 적용
		Push( new DB_DeleteAuctionInfoAndInsertItemKeepingInfo( pAuctionInfo->nAuctionID, pReturnItem ) );

		// 로그 메시지 기록
		LOG::Log11N4S( LM_KEEPING_BEGIN, 0, pAuctionInfo->nSellerUID, pReturnItem->nKeepingID, pAuctionInfo->nAuctionID,
			pReturnItem->eType, pReturnItem->pItem->GetItemCode(), pReturnItem->pItem->GetCount(), GameRule::AUCTION_ITEM_KEEP_DURATION,
			0, 0, pReturnItem->pItem->GetItemUID(),
			( pszCancellerAccount ) ? pszCancellerAccount : "", LOG::STR_NTS, ( pszCancellerName ) ? pszCancellerName : "", LOG::STR_NTS, "", 0, "", 0 );
	}
	// 물품 판매자가 시스템이었을 경우 경매 데이터 및 아이템 데이터 제거
	else
	{
		StructItem::PendFreeItem( pAuctionInfo->pItem );
		Push( new DB_DeleteAuctionInfo( pAuctionInfo->nAuctionID ) );
	}

	// 시스템 메시지 출력에서 필요한 Item Code를 함수 호출부로 전달하기 위해 값 세팅
	nResultItemCode = pAuctionInfo->pItem->GetItemCode();

	// 로그 메시지 기록
	LOG::Log11N4S( LM_AUCTION_CANCEL, pAuctionInfo->pItem->GetItemCode(), nCancellerUID, pAuctionInfo->nAuctionID,
		nHighestBidderUID, nHighestBiddingPrice.GetRawData(),
		0, 0, 0, 0, pAuctionInfo->nRegistrationTax.GetRawData(), pAuctionInfo->pItem->GetItemUID(),
		( pszCancellerAccount ) ? pszCancellerAccount : "", LOG::STR_NTS, ( pszCancellerName ) ? pszCancellerName : "", LOG::STR_NTS, strHighestBidderName.c_str(), LOG::STR_NTS, "", 0 );

	// 경매 데이터 검색용 벡터/해쉬 제거
	removeAuctionInfoFromIndex( pAuctionInfo->nAuctionID );
	delete pAuctionInfo;

	return RESULT_SUCCESS;
}

int AuctionManager::getCategoryIndex( const ItemBase::ITEM_GROUP nGroup, const ItemBase::ItemClass nClass ) const
{
	for( AuctionCategoryVecConstIterator itParentCategory = m_vAuctionCategory.begin() ; itParentCategory != m_vAuctionCategory.end() ; ++itParentCategory )
	{
		for( AuctionCategoryVecConstIterator itChildCategory = (*itParentCategory).vChildAuctionCategory.begin() ; itChildCategory != (*itParentCategory).vChildAuctionCategory.end() ; ++itChildCategory )
		{
			if( (*itChildCategory).IsOverlappedItemCategory( nGroup, nClass ) )
			{
				return (*itChildCategory).nCategoryID;
			}
		}

		if( (*itParentCategory).IsOverlappedItemCategory( nGroup, nClass ) )
		{
			return (*itParentCategory).nCategoryID;
		}
	}

	// 어디에도 해당되지 않았으므로 m_nMaxCategoryIndex를 반환(기타 카테고리)
	return m_nMaxCategoryIndex;
}

unsigned short AuctionManager::addAuctionInfoToIndex( AuctionInfo *pAuctionInfo, const bool bNeedToBePreceded )
{
	if( !pAuctionInfo || !pAuctionInfo->pItem )
	{
		return RESULT_NOT_EXIST;
	}

	// 경매 번호 해쉬에 포인터 등록
	if( m_hsAuctionListByAuctionID.lookup( pAuctionInfo->nAuctionID ) )
	{
		return RESULT_ALREADY_EXIST;
	}
	m_hsAuctionListByAuctionID.add( pAuctionInfo->nAuctionID, pAuctionInfo );

	// 등록 가능 카테고리 확인
	// m_nMaxCategoryIndex가 반환된 경우는 카테고리 분류가 등록되어 있지 않은 품목이므로 CATEGORY_INDEX_ETC로 변경
	int nIndex = getCategoryIndex( static_cast< ItemBase::ITEM_GROUP >( pAuctionInfo->pItem->GetItemGroup() ), static_cast< ItemBase::ItemClass >( pAuctionInfo->pItem->GetItemClass() ) );
	if( nIndex <= CATEGORY_SPECIAL || nIndex > m_nMaxCategoryIndex )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 카테고리별 리스트에 포인터 등록
	AuctionList * plAuctionInfo = m_vplAuctionListByCategory[ nIndex ];
	bool bInserted = false;
	for( AuctionListIterator it = plAuctionInfo->begin() ; it != plAuctionInfo->end() ; ++it )
	{
		// 등록시 정렬을 해야 하므로 아래 조건을 따라야 함

		// 1-1. 시크루트 전용 경매가 아니라면 타 시크루트 전용 경매 경매 이후에 등록
		if( (*it)->bSecrouteOnly && !bNeedToBePreceded )
		{
			continue;
		}

		// 1-2. 시크루트 전용 경매인데 타 시크루트 전용 경매를 다 지났을 경우 바로 등록(시크루트 전용 경매 중 맨 끝에 추가됨)
		// 즉, 정렬에 참여해야 하는 경우는 시크루트 경매끼리 혹은 일반 경매 끼리
		// (일반 경매가 시크루트 경매 이전에 추가되는 것은 1-1 체크에서 막아짐)
		if( (*it)->bSecrouteOnly == bNeedToBePreceded )
		{
			// 2. 아이템 코드순 정렬(오름차순)
			if( (*it)->pItem->GetItemCode() < pAuctionInfo->pItem->GetItemCode() )
			{
				continue;
			}
			else if( (*it)->pItem->GetItemCode() == pAuctionInfo->pItem->GetItemCode() )
			{
				// 3. 아이템 강화 수치순 정렬(내림차순)
				if( (*it)->pItem->GetItemEnhance() > pAuctionInfo->pItem->GetItemEnhance() )
				{
					continue;
				}
				else if( (*it)->pItem->GetItemEnhance() == pAuctionInfo->pItem->GetItemEnhance() )
				{
					// 4. 아이템 대장작 레벨순 정렬(내림차순)
					if( (*it)->pItem->GetItemLevel() > pAuctionInfo->pItem->GetItemLevel() )
					{
						continue;
					}
				}
			}
		}

		plAuctionInfo->insert( it, pAuctionInfo );
		bInserted = true;
		break;
	}
	if( !bInserted )
	{
		plAuctionInfo->push_back( pAuctionInfo );
	}

	// 판매자가 있는 경우(유저 등록) 판매자별 벡터에 포인터 등록
	if( pAuctionInfo->nSellerUID )
	{
		AuctionVector *pvAuctionInfo = NULL;
		if( !m_hsAuctionListBySellerUID.lookup( pAuctionInfo->nSellerUID, pvAuctionInfo ) )
		{
			pvAuctionInfo = new AuctionVector;
			m_hsAuctionListBySellerUID.add( pAuctionInfo->nSellerUID, pvAuctionInfo );
		}
		pvAuctionInfo->push_back( pAuctionInfo );
	}

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::removeAuctionInfoFromIndex( const AuctionUID nAuctionID, const bool bSkipRemoveFromListByCategory )
{
	if( nAuctionID <= 0 )
	{
		return RESULT_NOT_EXIST;
	}

	// 경매 번호 해쉬에서 포인터 제거
	AuctionInfo *pAuctionInfo = NULL;
	if( !m_hsAuctionListByAuctionID.lookup( nAuctionID, pAuctionInfo ) )
	{
		return RESULT_NOT_EXIST;
	}
	m_hsAuctionListByAuctionID.erase( nAuctionID );

	// 등록된 카테고리 확인
	// m_nMaxCategoryIndex가 반환된 경우는 카테고리 분류가 등록되어 있지 않은 품목(m_nMaxCategoryIndex 자체가 기타 카테고리 인덱스)
	int nIndex = getCategoryIndex( static_cast< ItemBase::ITEM_GROUP >( pAuctionInfo->pItem->GetItemGroup() ), static_cast< ItemBase::ItemClass >( pAuctionInfo->pItem->GetItemClass() ) );
	if( nIndex <= CATEGORY_SPECIAL || nIndex > m_nMaxCategoryIndex )
	{
		return RESULT_NOT_EXIST;
	}

	// 카테고리별 리스트 순회 중에 리스트에서 제거하면 순회 중이던 iterator가 무효화 됨
	if( !bSkipRemoveFromListByCategory )
	{
		// 카테고리별 리스트에서 포인터 제거
		AuctionList * plAuctionInfo = m_vplAuctionListByCategory[ nIndex ];
		for( AuctionListIterator itAuction = plAuctionInfo->begin() ; itAuction != plAuctionInfo->end() ; ++itAuction )
		{
			if( (*itAuction) == pAuctionInfo )
			{
				plAuctionInfo->erase( itAuction );
				break;
			}
		}
	}

	// 판매자별 벡터에서 포인터 제거
	AuctionVector * pvAuctionInfo = NULL;
	if( m_hsAuctionListBySellerUID.lookup( pAuctionInfo->nSellerUID, pvAuctionInfo ) )
	{
		bool bNeedToDeleteAuctionInfo = false;

		for( AuctionVecIterator itAuction = pvAuctionInfo->begin() ; itAuction != pvAuctionInfo->end() ; ++itAuction )
		{
			if( (*itAuction) == pAuctionInfo )
			{
				vector_fast_erase( pvAuctionInfo, itAuction );

				if( pvAuctionInfo->empty() )
				{
					m_hsAuctionListBySellerUID.erase( pAuctionInfo->nSellerUID );
					bNeedToDeleteAuctionInfo = true;
				}

				break;
			}
		}

		if( bNeedToDeleteAuctionInfo )
		{
			delete pvAuctionInfo;
		}
	}

	// 구매자별 벡터에서 포인터 제거
	for( std::vector< PlayerUID >::iterator itBidder = pAuctionInfo->vBidderUID.begin() ; itBidder != pAuctionInfo->vBidderUID.end() ; ++itBidder )
	{
		if( !m_hsBiddedAuctionList.lookup( (*itBidder), pvAuctionInfo ) )
			continue;

		bool bNeedToDeleteAuctionInfo = false;

		for( AuctionVecIterator itAuction = pvAuctionInfo->begin() ; itAuction != pvAuctionInfo->end() ; ++itAuction )
		{
			if( (*itAuction) == pAuctionInfo )
			{
				vector_fast_erase( pvAuctionInfo, itAuction );

				if( pvAuctionInfo->empty() )
				{
					m_hsBiddedAuctionList.erase( *itBidder );
					bNeedToDeleteAuctionInfo = true;;
				}

				break;
			}
		}

		if( bNeedToDeleteAuctionInfo )
		{
			delete pvAuctionInfo;
		}
	}

	// HighestBidder에 대해서 구매자별 벡터에서 포인터 제거
	if( pAuctionInfo->nHighestBidderUID && m_hsBiddedAuctionList.lookup( pAuctionInfo->nHighestBidderUID, pvAuctionInfo ) )
	{
		bool bNeedToDeleteAuctionInfo = false;

		for( AuctionVecIterator itAuction = pvAuctionInfo->begin() ; itAuction != pvAuctionInfo->end() ; ++itAuction )
		{
			if( (*itAuction) == pAuctionInfo )
			{
				vector_fast_erase( pvAuctionInfo, itAuction );

				if( pvAuctionInfo->empty() )
				{
					m_hsBiddedAuctionList.erase( pAuctionInfo->nHighestBidderUID );
					bNeedToDeleteAuctionInfo = true;
				}

				break;
			}
		}

		if( bNeedToDeleteAuctionInfo )
		{
			delete pvAuctionInfo;
		}
	}

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::addItemKeepingInfoToIndex( ItemKeepingInfo *pItemKeeping )
{
	if( !pItemKeeping || !pItemKeeping->nOwnerUID || !pItemKeeping->pItem )
	{
		return RESULT_NOT_EXIST;
	}

	ItemKeepingVector *pvItemKeeping = NULL;
	// 경매 물품 보관 데이터 해쉬 추가
	if( !m_hsItemKeepingList.lookup( pItemKeeping->nOwnerUID, pvItemKeeping ) )
	{
		pvItemKeeping = new ItemKeepingVector;
		m_hsItemKeepingList.add( pItemKeeping->nOwnerUID, pvItemKeeping );
	}
	pvItemKeeping->push_back( pItemKeeping );

	// 경매 물품 보관 데이터 전체 벡터에 추가
	m_vItemKeepingList.push_back( pItemKeeping );

	return RESULT_SUCCESS;
}

unsigned short AuctionManager::removeItemKeepingInfoFromIndex( const PlayerUID nOwnerUID, const ItemKeepingUID nKeepingID, const bool bSkipRemoveFromVector )
{
	if( nKeepingID <= 0 )
	{
		return RESULT_NOT_EXIST;
	}

	ItemKeepingVector *pvItemKeeping = NULL;
	ItemKeepingInfo *pItemKeeping = NULL;
	// 경매 물품 보관 데이터 해쉬 제거
	if( !m_hsItemKeepingList.lookup( nOwnerUID, pvItemKeeping ) )
	{
		return RESULT_NOT_EXIST;
	}
	for( ItemKeepingVecIterator it = pvItemKeeping->begin() ; it != pvItemKeeping->end() ; ++it )
	{
		if( (*it)->nKeepingID == nKeepingID )
		{
			pItemKeeping = (*it);
			pvItemKeeping->erase( it );
			break;
		}
	}
	if( pvItemKeeping->empty() )
	{
		m_hsItemKeepingList.erase( nOwnerUID );
		delete pvItemKeeping;
	}

	// 전체 벡터 순회 중에 제거하면 순회 중이던 iterator가 무효화 됨
	if( !bSkipRemoveFromVector )
	{
		// 전체 벡터에서 포인터 제거
		for( ItemKeepingVecIterator it = m_vItemKeepingList.begin() ; it != m_vItemKeepingList.end() ; ++it )
		{
			if( (*it) == pItemKeeping )
			{
				m_vItemKeepingList.erase( it );
				break;
			}
		}
	}

	return RESULT_SUCCESS;
}
