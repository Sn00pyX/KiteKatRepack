#pragma once

#include <ctime>
#include <vector>
#include <list>

#include <toolkit/ILock.h>
#include <toolkit/KHash.h>
#include <mmo/ArSchedulerObject.h>

#include "StructPlayer.h"
#include "StructItem.h"
#include "GameDBManager.h"
#include "AuctionBase.h"
#include "GameMessage.h"

struct AuctionManager : ArSchedulerObject
{
public:
	typedef std::vector< AuctionInfo * >					AuctionVector;
	typedef std::vector< AuctionInfo * >::iterator			AuctionVecIterator;
	typedef std::list< AuctionInfo * >						AuctionList;
	typedef std::list< AuctionInfo * >::iterator			AuctionListIterator;
	typedef std::vector< ItemKeepingInfo * >				ItemKeepingVector;
	typedef std::vector< ItemKeepingInfo * >::iterator		ItemKeepingVecIterator;
	typedef std::vector< AutoAuctionInfo * >				AutoAuctionVector;
	typedef std::vector< AutoAuctionInfo * >::iterator		AutoAuctionVecIterator;

public:
	static AuctionManager & Instance();
	virtual ~AuctionManager();

	// AuctionCategory 추가
	bool RegisterAuctionCategory( const int category_id, const int sub_category_id, const int item_group, const int item_class );

	// 로드된 AuctionCategory 데이터 기준 Category 분류 벡터 생성
	bool FinalizeAuctionCategory();

	// AuctionCategory 트리의 노드 수 반환
	size_t GetAuctionCategoryCount( const bool bOnlyParentCategory = false ) const;

	// AuctionCategory 트리에 존재하는 ItemCategory 개수 반환
	size_t GetAuctionItemCategoryCount() const ;

	// Loading에 의해 생성된 AuctionInfo를 직접적으로 추가
	bool AddAuctionInfoByLoading( AuctionInfo *pAuctionInfo );
	// Loading에 의해 생성된 ItemKeepingInfo를 직접적으로 추가
	bool AddItemKeepingInfoByLoading( ItemKeepingInfo *pItemKeeping );

	bool Init();
	bool DeInit();

	// 아래의 3개 함수는 처리 성공 시(RESULT_SUCCESS 반환 시)에는 리스트를 직접 송신하지만 그 외의 경우에는 오류 관련 메시지를 송신하지 않음
	// 등록된 경매 물품 검색(최소 검색 시간 간격 제한도 함께 처리) 주의: szKeyword 처리를 위해서는 아이템 이름 관련 StringResource가 서버에 적용되어 있어야 함
	unsigned short SearchAndSendAuctionList( const StructPlayer *pPlayer, const int /*ItemBase::ITEM_GROUP*/ nGroup, const int /*ItemBase::ItemClass*/ nClass, const char *szKeyword, const int nPageNum, const bool bIsEquipable );
	// 자신이 판매 중인 경매 물품 검색
	unsigned short SendRegisteredAuctionList( const StructPlayer *pPlayer, const int nPageNum );
	// 자신이 입찰한 경매 물품 검색
	unsigned short SendBiddedAuctionList( const StructPlayer *pPlayer, const int nPageNum );

	// 물품 경매 등록(유저가 등록하는 경우에 사용) - pPlayer 골드 감소 처리 있으므로 const 아님
	unsigned short RegisterItemToSell( StructPlayer *pPlayer, AR_HANDLE item_handle, const __int64 & item_count, const time_t tAuctionDuration, const StructGold & nStartPrice, const StructGold & nInstantPurchasePrice, ItemBase::ItemCode * pResultItemCode );
	// 물품 경매 등록(시스템이 등록하는 경우에 사용)
	unsigned short RegisterItemToSell( const char *pszSellerName, AR_HANDLE item_handle, const bool bSecrouteOnly, const time_t tAuctionDuration, const StructGold & nStartPrice, const StructGold & nInstantPurchasePrice, ItemBase::ItemCode * pResultItemCode );
	// 경매에 대해 입찰 - pPlayer 골드 감소 처리 있으므로 const 아님
	unsigned short BidForAuction( StructPlayer *pPlayer, const AuctionUID nAuctionID, const StructGold & nBiddingPrice, ItemBase::ItemCode & nResultItemCode );
	// 경매 물품을 즉시 구매하기 - pPlayer 골드 감소 처리 있으므로 const 아님
	unsigned short InstantPurchase( StructPlayer *pPlayer, const AuctionUID nAuctionID, ItemBase::ItemCode & nResultItemCode );
	// 경매 취소
	unsigned short CancelAuction( const StructPlayer *pPlayer, const AuctionUID nAuctionID, ItemBase::ItemCode & nResultItemCode );
	// 경매 취소(GM 명령어 용으로 특정 유저의 경매를 모두 취소시킴)
	unsigned short CancelAuction( const PlayerUID & nSellerUID, std::vector< ItemBase::ItemCode > & vResultItemCode, const StructPlayer * pCanceller = NULL );

	// 아래의 함수는 처리 성공 시(RESULT_SUCCESS 반환 시)에는 리스트를 직접 송신하지만 그 외의 경우에는 오류 관련 메시지를 송신하지 않음
	// 자신의 보관함에 보관 중인 아이템 검색
	unsigned short SendKeepingItemList( const StructPlayer *pPlayer, const int nPageNum );

	// 보관함에 보관 중인 아이템 찾기 - pPlayer 아이템 지급 처리 있으므로 const 아님
	unsigned short TakeKeepedItem( StructPlayer *pPlayer, const ItemKeepingUID nKeepingID );

	// 경매 자동 등록 정보 초기화(최근 등록 일시 정보도 초기화 됨)
	void ClearAutoAuctionInfo();

	// 경매 자동 등록 정보 추가
	bool AddAutoAuctionInfo( const int nAutoAuctionID, const char *szSellerName, const ItemBase::ItemCode nItemCode, const __int64 & nItemCount,
		const StructGold & nStartPrice, const StructGold & nInstantPurchasePrice, const time_t tStartTime, const time_t tAuctionDuration, const bool bSecrouteOnly,
		const bool bNeedToRepeat, const time_t tRepeatTerm );

	// 경매 자동 등록 정보 개수 반환
	size_t GetAutoAuctionResouceCount()		{ return m_vAutoAuction.size(); }

	// 자동 등록 경매 최근 등록 일시 정보 초기화
	void ClearAutoAuctionRegistrationInfo();

	// 자동 등록 경매 최근 등록 일시 정보 설정
	bool SetLastAutoRegisteredTime( const int nAutoAuctionID, const time_t tLastRegisteredTime );

	// 등록된 자동 등록 경매 정보인지 
	bool IsRegisteredAutoAuction( const int nAutoAuctionID );

	// 경매 등록자/최고 입찰자 이름 정보를 변경(아바타 이름 변경 기능이 사용될 경우 처리 필요 - DB 업데이트는 DB_ChangeCharacterName 에서 처리)
	void OnChangeCharacterName( const int nPlayerUID, const char *szName );

	// ArScheduler가 호출해야 하므로 public
	virtual void onProcess( int nThreadIdx );

	// AuctionLoaer에서 잘못된 AuctionInfo/ItemKeepingInfo를 삭제해야 하므로 호출됨
	void Push( GameDBManager::DBProc* pWork );
	// Push 함수에 의해 추가된 타 클래스 인스턴스들이 완료되면서 호출됨
	void onEndQuery();

protected:
	unsigned short cancelAuction( AuctionInfo * pAuctionInfo, ItemBase::ItemCode & nResultItemCode, const char * pszCancellerAccount, const char * pszCancellerName, const PlayerUID & nCancellerUID );

private:
	AuctionManager()
		: m_AuctionLock( "AuctionManagger::m_AuctinoLock" )
		, m_nMaxCategoryIndex( -1 )
		, m_ItemKeepingLock( "AuctionManager::m_ItemKeepingLock" )
		, m_AutoAuctionLock( "AuctionManager::m_AutoAuctionLock" )
		, m_QueryLock( "AuctionManager::m_QueryLock" )
	{}

	// m_vplAuctionListByCategory 에서 아이템의 그룹(대분류)과 클래스(중분류)에 따라 카테고리 인덱스 번호를 반환(ITEM_GROUP,ItemClass -> CategoryIndex 맵 기능)
	int getCategoryIndex( const ItemBase::ITEM_GROUP nGroup, const ItemBase::ItemClass nClass ) const;

	// 검색용 벡터/해쉬에 경매 데이터 추가
	unsigned short addAuctionInfoToIndex( AuctionInfo *pAuctionInfo, const bool bNeedToBePreceded = false );
	// 검색용 벡터/해쉬에서 경매 데이터 제거(카테고리별 리스트 순회 중에 호출될 수 있으므로 삭제 방지 필요)
	unsigned short removeAuctionInfoFromIndex( const AuctionUID nAuctionID, const bool bSkipRemoveFromListByCategory = false );

	// 검색용 벡터/해쉬에 아이템 보관 데이터 추가
	unsigned short addItemKeepingInfoToIndex( ItemKeepingInfo *pItemKeeping );
	// 검색용 벡터/해쉬에서 아이템 보관 데이터 제거(전체 벡터 순회 중에 호출될 수 있으므로 삭제 방지 필요)
	unsigned short removeItemKeepingInfoFromIndex( const PlayerUID nOwnerUID, const ItemKeepingUID nKeepingID, const bool bSkipRemoveFromVector = false );

private:
	XCriticalSection						m_AuctionLock;

	// 경매 물품 카테고리 정보 보관
	AuctionCategoryVector					m_vAuctionCategory;
	int										m_nMaxCategoryIndex;

	// 카테고리별로 리스트를 따로 두고 포인터 보관(경매 물품 검색용 인덱스 기능. Ex. 무기 항목의 특정 키워드를 포함하는 물품 검색)
	std::vector< AuctionList * >			m_vplAuctionListByCategory;
	// 경매 번호를 해쉬 키로 포인터 보관
	KHash< AuctionInfo *, hashPr_mod_basic< AuctionUID > >						m_hsAuctionListByAuctionID;
	// SellerUID를 해쉬 키로 포인터 보관(자신이 판매 등록한 경매 데이터 인덱스 기능)
	KHash< AuctionVector *, hashPr_mod_basic< PlayerUID > >		m_hsAuctionListBySellerUID;
	// BidderUID를 해쉬 키로 포인터 보관(자신이 입찰한 적이 있는 진행 중인 경매 데이터 인덱스 기능, 타인이 상위 입찰해도 유지)
	KHash< AuctionVector *, hashPr_mod_basic< PlayerUID > >		m_hsBiddedAuctionList;

	XCriticalSection						m_ItemKeepingLock;

	// 보관 중인 물품 정보를 모두 보관하는 벡터
	ItemKeepingVector						m_vItemKeepingList;
	// OwnerUID를 해쉬 키로 포인터 보관(낙찰/상위 입찰에 의한 입찰 환급 등 개인 소유의 경매장 보관 중인 물품 정보)
	KHash< ItemKeepingVector *, hashPr_mod_basic< PlayerUID > >	m_hsItemKeepingList;

	XCriticalSection						m_AutoAuctionLock;

	// 자동 등록 경매 정보를 보관하는 벡터
	AutoAuctionVector						m_vAutoAuction;

	// DB 작업 관련
	XCriticalSection						m_QueryLock;
	std::list< GameDBManager::DBProc* >		m_lQueryList;
};
