#pragma once

#include <utility>

#include <toolkit/XStringUtil.h>

#include "UIDManager.h"
#include "ItemBase.h"
#include "UID.h"
#include "StructGold.h"

struct StructItem;

// 경매 ID 타입 개별화
typedef int AuctionUID;

// 아이템 보관 ID 타입 개별화
typedef int ItemKeepingUID;

// 아이템 보관 타입 상수 선언
enum KEEPING_TYPE
{
	KEEPING_TYPE_UNKNOWN					= 0,			// 보관 타입 불명

	KEEPING_TYPE_ITEM_BY_SUCCESSFUL_BID		= 1,			// 경매 물품 낙찰
	KEEPING_TYPE_ITEM_BY_INSTANT_PURCHASE	= 2,			// 경매 물품 즉시 구매
	KEEPING_TYPE_ITEM_BY_EXPIRATION			= 3,			// 경매 만료에 의한 물품 반환
	KEEPING_TYPE_ITEM_BY_CANCEL				= 4,			// 경매 취소에 의한 물품 반환

	KEEPING_TYPE_GOLD_BY_ITEM_SELL			= 30,			// 경매 물품 판매 대금
	KEEPING_TYPE_GOLD_BY_REG_TAX			= 31,			// 경매 물품 판매 이후 등록금 환급
	KEEPING_TYPE_GOLD_BY_HIGHER_BID			= 32,			// 상위 입찰에 의한 입찰금 환급
	KEEPING_TYPE_GOLD_BY_CANCEL				= 33,			// 경매 취소에 의한 입찰금 환급
	KEEPING_TYPE_GOLD_BY_ITEM_SOLD_OUT		= 34,			// 타인 경매 물품 즉시 구매에 의한 입찰금 환급
};

// 경매 카테고리 정보( category_id, sub_category_id 쌍으로 (-1, -1)은 전체, (-1, 0)은 기타 임 )
static const int		CATEGORY_SPECIAL		= -1;
static const int		SUB_CATEGORY_ALL		= -1;
static const int		SUB_CATEGORY_ETC		= 0;
static const int		SUB_CATEGORY_NONE		= -1;

typedef std::pair< /*ITEM_GROUP*/ int, /*ItemClass*/ int >		ItemCategory;
typedef std::vector< std::pair< int, int > >					ItemCategoryVector;
typedef std::vector< std::pair< int, int > >::const_iterator	ItemCategoryVecConstIterator;

class AuctionCategoryInfo
{
public:
	AuctionCategoryInfo( const int _nCategoryID, const int _nSubCategoryID )
		: nCategoryID( _nCategoryID )
		, nSubCategoryID( _nSubCategoryID )
	{}

	// 현재 AuctionCategoryInfo가 가지고 있는 ItemCategory들과 중복되는 부분이 있는지 검사
	bool IsOverlappedItemCategory( const int nItemGroup, const int nItemClass ) const;

	// 새로운 ItemCategory 추가(중첩된 ItemCategory를 추가하면 false 리턴)
	void AddItemCategory( const int nItemGroup, const int nItemClass )
	{
		m_vItemCategory.push_back( ItemCategory( nItemGroup, nItemClass ) );
	}

	// 현재 AuctionCategoryInfo에 속한 ItemCategory 수 반환
	const size_t GetItemCategoryCount() const		{ return m_vItemCategory.size(); }

	int									nCategoryID;
	int									nSubCategoryID;

	std::vector< AuctionCategoryInfo >	vChildAuctionCategory;

private:
	ItemCategoryVector		m_vItemCategory;
};

typedef std::vector< AuctionCategoryInfo >						AuctionCategoryVector;
typedef std::vector< AuctionCategoryInfo >::iterator			AuctionCategoryVecIterator;
typedef std::vector< AuctionCategoryInfo >::const_iterator		AuctionCategoryVecConstIterator;

// 진행 중인 경매에 대한 정보
class AuctionInfo
{
public:
	AuctionInfo( StructItem *_pItem, const PlayerUID _nSellerUID, const char *_szSellerName, const bool _bSecrouteOnly, const time_t _tAuctionDuration, const StructGold & _nStartPrice, const StructGold & _nInstantPurchasePrice, const StructGold & _nRegistrationTax );
	AuctionInfo( const AuctionUID _nAuctionID, StructItem *_pItem, const PlayerUID _nSellerUID, const char *_szSellerName, const bool _bSecrouteOnly, const time_t _tAuctionEnd, const StructGold & _nInstantPurchasePrice, const StructGold & _nRegistrationTax, const char *_szBidderList, const StructGold & _nHighestBiddingPrice, const PlayerUID _nHighestBidderUID, const char *_szHighestBidderName );

#ifdef _MEM_USAGE_DEBUG
	AuctionInfo( const AuctionInfo & instance );
	~AuctionInfo();
#endif

	static const int							MAX_NAME_LENGTH = 31;					// 아바타 이름 저장 버퍼 길이 상수

	const AuctionUID							nAuctionID;								// 경매 ID

	StructItem *								pItem;									// 경매 물품 - 아이템은 메모리에 로드되어 있어야 하고 그 포인터만 유지함(아이템 소유권 변경, 삭제 처리가 필요하므로 const 아님)
	const PlayerUID								nSellerUID;								// 경매 물품 등록자 UID
	char										szSellerName[MAX_NAME_LENGTH];			// 경매 물품 등록자 이름

	const bool									bSecrouteOnly;							// 시크루트 프리패스 결재자 전용 경매 여부

	const time_t								tAuctionEnd;							// 경매 종료 시각

	const StructGold							nInstantPurchasePrice;					// 물품 즉시 구매 가격
	const StructGold							nRegistrationTax;						// 물품 경매 등록시 지불한 등록세(낙찰/유찰에 의한 경매 종료 후 환급해야 함)

	StructGold									nHighestBiddingPrice;					// 최고 입찰가 - 입찰자가 없던 경우에는 경매 시작가
	std::vector< PlayerUID >					vBidderUID;								// 입찰자 UID 리스트
	PlayerUID									nHighestBidderUID;						// 최고 입찰자 UID
	char										szHighestBidderName[MAX_NAME_LENGTH];	// 최고가 입찰자 이름
};

// 경매 물품 및 판매 대금, 타인 상위 입찰로 인한 입찰금 환불 등 아이템 보관 정보
class ItemKeepingInfo
{
public:
	ItemKeepingInfo( StructItem *_pItem, const PlayerUID _nOwnerUID, const time_t _tKeepDuration, const KEEPING_TYPE _eType, const AuctionUID & _nRelatedAuctionID, const ItemBase::ItemCode _nRelatedItemCode = 0, const int _nRelatedItemEnhance = 0, const int _nRelatedItemLevel = 0 );
	ItemKeepingInfo( const ItemKeepingUID _nKeepingID, StructItem *_pItem, const PlayerUID _nOwnerUID, const time_t _tExpiration, const KEEPING_TYPE _eType, const AuctionUID & _nRelatedAuctionID, const ItemBase::ItemCode _nRelatedItemCode = 0, const int _nRelatedItemEnhance = 0, const int _nRelatedItemLevel = 0 );

#ifdef _MEM_USAGE_DEBUG
	ItemKeepingInfo( const ItemKeepingInfo & instance );
	~ItemKeepingInfo();
#endif

	const ItemKeepingUID				nKeepingID;				// 아이템 보관 ID
	StructItem *						pItem;					// 보관 아이템 - 아이템은 메모리에 로드되어 있어야 하고 그 포인터만 유지함(아이템 소유권 변경, 삭제 처리가 필요하므로 const 아님)
	const PlayerUID						nOwnerUID;				// 보관 아이템 소유권자
	const time_t						tExpiration;			// 보관 시한
	const KEEPING_TYPE					eType;					// 보관 타입
	const AuctionUID					nRelatedAuctionID;		// 관련 경매 ID(보관을 발생시킨 경매의 ID)
	const ItemBase::ItemCode			nRelatedItemCode;		// 관련 아이템 코드(보관 아이템이 돈일 경우만 사용됨)
	const int							nRelatedItemEnhance;	// 관련 아이템 강화 레벨(보관 아이템이 돈일 경우만 사용됨)
	const int							nRelatedItemLevel;		// 관련 아이템 대장작 레벨(보관 아이템이 돈일 경우만 사용됨)
};

// 자동 등록 경매 정보
class AutoAuctionInfo
{
public:
	AutoAuctionInfo( const int _nAutoAuctionID /* 자동 발급 아니고 Arcadia.dbo.AutoAuctionResource에 지정되어 있음 */,
		const char *_szSellerName, const ItemBase::ItemCode _nItemCode, const __int64 & _nItemCount, const StructGold & _nStartPrice, const StructGold & _nInstantPurchasePrice,
		const time_t _tStartTime, const time_t _tAuctionDuration, const bool _bSecrouteOnly, const bool _bNeedToRepeat, const time_t _tRepeatTerm );
#ifdef _MEM_USAGE_DEBUG
	~AutoAuctionInfo();
#endif

	const int					nAutoAuctionID;
	char						szSellerName[31];
	const ItemBase::ItemCode	nItemCode;
	const __int64				nItemCount;
	const StructGold			nStartPrice;
	const StructGold			nInstantPurchasePrice;
	const time_t				tStartTime;
	const time_t				tAuctionDuration;
	const bool					bSecrouteOnly;
	const bool					bNeedToRepeat;
	const time_t				tRepeatTerm;

	time_t						tLastRegisteredTime;
};
