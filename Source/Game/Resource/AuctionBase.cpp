
#include <dump/XExceptionHandler.h>

#include "AuctionBase.h"
#include "GameRule.h"

bool AuctionCategoryInfo::IsOverlappedItemCategory( const int nItemGroup, const int nItemClass ) const
{
	// 대분류 전체를 검사하면 ItemCategory가 존재하면 무조건 중첩임
	if( nItemGroup == CATEGORY_SPECIAL && !m_vItemCategory.empty() )
	{
		return true;
	}

	for( ItemCategoryVecConstIterator it = m_vItemCategory.begin() ; it != m_vItemCategory.end() ; ++it )
	{
		if( (*it).first == nItemGroup )
		{
			// 대분류가 같은 ItemCategory에 대해서
			// 동일한 중분류이거나, 중분류 전체를 포함하는 ItemCategory가 있거나, 중분류 전체를 검사하면 중첩임
			if( (*it).second == nItemClass || (*it).second == SUB_CATEGORY_ALL || nItemClass == SUB_CATEGORY_ALL )
			{
				return true;
			}
		}
	}

	return false;
}

AuctionInfo::AuctionInfo( StructItem *_pItem, const PlayerUID _nSellerUID, const char *_szSellerName, const bool _bSecrouteOnly, const time_t _tAuctionDuration, const StructGold & _nStartPrice, const StructGold & _nInstantPurchasePrice, const StructGold & _nRegistrationTax )
	: nAuctionID( UIDManager::Instance().GetNewUID( "AuctionUID" ) )
	, pItem( _pItem )
	, nSellerUID( _nSellerUID )
	, bSecrouteOnly( _bSecrouteOnly )
	, tAuctionEnd( time( NULL ) + _tAuctionDuration )
	, nHighestBidderUID( 0 )
	, nHighestBiddingPrice( _nStartPrice )
	, nInstantPurchasePrice( _nInstantPurchasePrice )
	, nRegistrationTax( _nRegistrationTax )
{
	s_strcpy( szSellerName, _countof( szSellerName ), _szSellerName );
	memset( szHighestBidderName, 0, sizeof( szHighestBidderName ) );

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "AuctionInfo" );
#endif
}

AuctionInfo::AuctionInfo( const AuctionUID _nAuctionID, StructItem *_pItem, const PlayerUID _nSellerUID, const char *_szSellerName, const bool _bSecrouteOnly, const time_t _tAuctionEnd, const StructGold & _nInstantPurchasePrice, const StructGold & _nRegistrationTax, const char *_szBidderList, const StructGold & _nHighestBiddingPrice, const PlayerUID _nHighestBidderUID, const char *_szHighestBidderName )
	: nAuctionID( _nAuctionID )
	, pItem( _pItem )
	, nSellerUID( _nSellerUID )
	, bSecrouteOnly( _bSecrouteOnly )
	, tAuctionEnd( _tAuctionEnd )
	, nInstantPurchasePrice( _nInstantPurchasePrice )
	, nRegistrationTax( _nRegistrationTax )
	, nHighestBiddingPrice( _nHighestBiddingPrice )
	, nHighestBidderUID( _nHighestBidderUID )
{
	s_strcpy( szSellerName, _countof( szSellerName ), _szSellerName );

	std::vector< std::string > vBidderList;
	XStringUtil::Split( _szBidderList, vBidderList, "|", false );

	std::vector< std::string >::iterator it = vBidderList.begin();
	if( vBidderList.size() > GameRule::AUCTION_MAX_BIDDER_LIST_LENGTH )
	{
		it += vBidderList.size() - GameRule::AUCTION_MAX_BIDDER_LIST_LENGTH;
	}

	while( it != vBidderList.end() )
	{
		vBidderUID.push_back( atoi( (*it).c_str() ) );
		++it;
	}

	s_strcpy( szHighestBidderName, _countof( szHighestBidderName ), _szHighestBidderName );

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "AuctionInfo" );
#endif
}

#ifdef _MEM_USAGE_DEBUG
AuctionInfo::AuctionInfo( const AuctionInfo & instance )
: nAuctionID( instance.nAuctionID )
, pItem( instance.pItem )
, nSellerUID( instance.nSellerUID )
, bSecrouteOnly( instance.bSecrouteOnly )
, tAuctionEnd( instance.tAuctionEnd )
, nInstantPurchasePrice( instance.nInstantPurchasePrice )
, nRegistrationTax( instance.nRegistrationTax )
, nHighestBiddingPrice( instance.nHighestBiddingPrice )
, vBidderUID( instance.vBidderUID )
, nHighestBidderUID( instance.nHighestBidderUID )
{
	s_strcpy( szSellerName, _countof( szSellerName ), instance.szSellerName );
	s_strcpy( szHighestBidderName, _countof( szHighestBidderName ), instance.szHighestBidderName );

	XSEH::IncreaseAllocCount( "AuctionInfo", false );
}

AuctionInfo::~AuctionInfo()
{
	XSEH::DecreaseAllocCount( "AuctionInfo" );
}
#endif


ItemKeepingInfo::ItemKeepingInfo( StructItem *_pItem, const PlayerUID _nOwnerUID, const time_t _tKeepDuration, const KEEPING_TYPE _eType, const AuctionUID & _nRelatedAuctionID, const ItemBase::ItemCode _nRelatedItemCode, const int _nRelatedItemEnhance, const int _nRelatedItemLevel )
	: nKeepingID( UIDManager::Instance().GetNewUID( "ItemKeepingUID" ) )
	, pItem( _pItem )
	, nOwnerUID( _nOwnerUID )
	, tExpiration( time( NULL ) + _tKeepDuration )
	, eType( _eType )
	, nRelatedAuctionID( _nRelatedAuctionID )
	, nRelatedItemCode( _nRelatedItemCode )
	, nRelatedItemEnhance( _nRelatedItemEnhance )
	, nRelatedItemLevel( _nRelatedItemLevel )
{
#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "ItemKeepingInfo" );
#endif
}

ItemKeepingInfo::ItemKeepingInfo( const ItemKeepingUID _nKeepingID, StructItem *_pItem, const PlayerUID _nOwnerUID, const time_t _tExpiration, const KEEPING_TYPE _eType, const AuctionUID & _nRelatedAuctionID, const ItemBase::ItemCode _nRelatedItemCode, const int _nRelatedItemEnhance, const int _nRelatedItemLevel )
	: nKeepingID( _nKeepingID )
	, pItem( _pItem )
	, nOwnerUID( _nOwnerUID )
	, tExpiration( _tExpiration )
	, eType( _eType )
	, nRelatedAuctionID( _nRelatedAuctionID )
	, nRelatedItemCode( _nRelatedItemCode )
	, nRelatedItemEnhance( _nRelatedItemEnhance )
	, nRelatedItemLevel( _nRelatedItemLevel )
{
#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "ItemKeepingInfo" );
#endif
}

#ifdef _MEM_USAGE_DEBUG
ItemKeepingInfo::ItemKeepingInfo( const ItemKeepingInfo & instance )
	: nKeepingID( instance.nKeepingID )
	, pItem( instance.pItem )
	, nOwnerUID( instance.nOwnerUID )
	, tExpiration( instance.tExpiration )
	, eType( instance.eType )
	, nRelatedAuctionID( instance.nRelatedAuctionID )
	, nRelatedItemCode( instance.nRelatedItemCode )
	, nRelatedItemEnhance( instance.nRelatedItemEnhance )
	, nRelatedItemLevel( instance.nRelatedItemLevel )
{
	// new 연산자를 통해 복사 생성자를 호출하는 일은 없으므로 s_nTotalAllocCount는 증가시키지 않음
	XSEH::IncreaseAllocCount( "ItemKeepingInfo", false );
}

ItemKeepingInfo::~ItemKeepingInfo()
{
	XSEH::DecreaseAllocCount( "ItemKeepingInfo" );
}
#endif


AutoAuctionInfo::AutoAuctionInfo( const int _nAutoAuctionID /* 자동 발급 아니고 Arcadia.dbo.AutoAuctionResource에 지정되어 있음 */,
	const char *_szSellerName, const ItemBase::ItemCode _nItemCode, const __int64 & _nItemCount, const StructGold & _nStartPrice, const StructGold & _nInstantPurchasePrice,
	const time_t _tStartTime, const time_t _tAuctionDuration, const bool _bSecrouteOnly, const bool _bNeedToRepeat, const time_t _tRepeatTerm )
	: nAutoAuctionID( _nAutoAuctionID )
	, nItemCode( _nItemCode )
	, nItemCount( _nItemCount )
	, nStartPrice( _nStartPrice )
	, nInstantPurchasePrice( _nInstantPurchasePrice )
	, tStartTime( _tStartTime )
	, tAuctionDuration( _tAuctionDuration )
	, bSecrouteOnly( _bSecrouteOnly )
	, bNeedToRepeat( _bNeedToRepeat )
	, tRepeatTerm( _tRepeatTerm )
{
	s_strcpy( szSellerName, _countof( szSellerName ), _szSellerName );
	tLastRegisteredTime = 0;

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "AutoAuctionInfo" );
#endif
}

#ifdef _MEM_USAGE_DEBUG
AutoAuctionInfo::~AutoAuctionInfo()
{
	XSEH::DecreaseAllocCount( "AutoAuctionInfo" );
}
#endif
