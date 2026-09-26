
#include <oledberr.h>
#include <oledb.h>
#include <atlcomtime.h>
#include <icrsint.h>


#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"
#include "AuctionManager.h"


DB_InsertAuctionInfo::DB_InsertAuctionInfo( const AuctionInfo *pAuctionInfo, const ItemUID nOriginalItemUID, const __int64 nOriginalItemCount )
: DBProc( "DB_InsertAuctionInfo" )
	, m_AuctionInfo( *pAuctionInfo )
	, m_nItemUID( pAuctionInfo->pItem->GetItemUID() )
	, m_nItemCode( pAuctionInfo->pItem->GetItemCode() )
	, m_nItemFlag( *pAuctionInfo->pItem->GetInstanceFlag().GetRawData() )
	, m_nItemCount( pAuctionInfo->pItem->GetCount() )
	, m_nItemLevel( pAuctionInfo->pItem->GetItemLevel() )
	, m_nItemEnhance( pAuctionInfo->pItem->GetItemEnhance() )
	, m_nItemEhterealDurability( pAuctionInfo->pItem->GetCurrentEtherealDurability() )
	, m_nItemEndurance( pAuctionInfo->pItem->GetCurrentEndurance() )
	, m_nItemGenCode( pAuctionInfo->pItem->GetGenerateCode() )
	, m_nItemRemainTime( pAuctionInfo->pItem->GetExpireTime() - time( NULL ) )
	, m_cItemElementalEffectType( pAuctionInfo->pItem->GetElementalEffectType() )
	, m_tItemElementalEffectExpire( pAuctionInfo->pItem->GetElementalEffectExpireTime() )
	, m_nItemElementalEffectAttackPoint( pAuctionInfo->pItem->GetElementalEffectAttackPoint() )
	, m_nItemElementalEffectMagicPoint( pAuctionInfo->pItem->GetElementalEffectMagicPoint() )
	, m_nItemAppearanceCode( pAuctionInfo->pItem->GetAppearanceCode() )
	, m_nOriginalItemUID( nOriginalItemUID )
	, m_nOriginalItemCount( nOriginalItemCount )
{
	m_AuctionInfo.pItem = NULL;
}

bool DB_InsertAuctionInfo::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertAuctionInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_auction_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_auction_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_AUCTION_UID", adInteger, adParamInput, 4, m_AuctionInfo.nAuctionID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_UID", adBigInt, adParamInput, 8, m_nItemUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SELLER_UID", adInteger, adParamInput, 4, m_AuctionInfo.nSellerUID ) );
		_bstr_t bstrBuffer( m_AuctionInfo.szSellerName );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SELLER_NAME", adBSTR, adParamInput, bstrBuffer.length(), bstrBuffer ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SECROUTE_ONLY", adBoolean, adParamInput, 1, m_AuctionInfo.bSecrouteOnly ) );

		COleDateTime dtAuctionEnd( m_AuctionInfo.tAuctionEnd );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_END_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtAuctionEnd ) ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_INSTANT_PURCHASE_PRICE", adBigInt, adParamInput, 8, m_AuctionInfo.nInstantPurchasePrice.GetRawData() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_REGISTRATION_TAX", adBigInt, adParamInput, 8, m_AuctionInfo.nRegistrationTax.GetRawData() ) );

		std::string szBidderUID;
		if( !m_AuctionInfo.vBidderUID.empty() )
		{
			char szBuf[12];

			s_sprintf( szBuf, _countof( szBuf ), "%d", m_AuctionInfo.vBidderUID.front() );
			szBidderUID += szBuf;
			for( std::vector< PlayerUID >::const_iterator it = m_AuctionInfo.vBidderUID.begin() + 1 ; it != m_AuctionInfo.vBidderUID.end() ; ++it )
			{
				s_sprintf( szBuf, _countof( szBuf ), "|%d", (*it) );
				szBidderUID += szBuf;
			}
		}

		cmd->Parameters->Append( cmd->CreateParameter( "IN_BIDDER_LIST", adVarChar, adParamInput, 1024, szBidderUID.c_str() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HIGHEST_BIDDING_PRICE", adBigInt, adParamInput, 8, m_AuctionInfo.nHighestBiddingPrice.GetRawData() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HIGHEST_BIDDER_UID", adInteger, adParamInput, 4, m_AuctionInfo.nHighestBidderUID ) );
		bstrBuffer = m_AuctionInfo.szHighestBidderName;
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HIGHEST_BIDDER_NAME", adBSTR, adParamInput, bstrBuffer.length(), bstrBuffer ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_CODE", adInteger, adParamInput, 4, m_nItemCode ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_FLAG", adInteger, adParamInput, 4, m_nItemFlag ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_CNT", adBigInt, adParamInput, 8, m_nItemCount ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_LEVEL", adInteger, adParamInput, 4, m_nItemLevel ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_ENHANCE", adInteger, adParamInput, 4, m_nItemEnhance ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_ETHEREAL_DURABILITY", adInteger, adParamInput, 4, m_nItemEndurance ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_ENDURANCE", adInteger, adParamInput, 4, m_nItemEndurance ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_GCODE", adInteger, adParamInput, 4, m_nItemGenCode ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_REMAIN_TIME", adInteger, adParamInput, 4, m_nItemRemainTime ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_ELEMENTAL_EFFECT_TYPE", adTinyInt, adParamInput, 1, m_cItemElementalEffectType ) );
		COleDateTime dtItemElementalEffectExpire( m_tItemElementalEffectExpire );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_ELEMENTAL_EFFECT_EXPIRE_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtItemElementalEffectExpire ) ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_ELEMENTAL_EFFECT_ATTACK_POINT", adInteger, adParamInput, 4, m_nItemElementalEffectAttackPoint ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_ELEMENTAL_EFFECT_MAGIC_POINT", adInteger, adParamInput, 4, m_nItemElementalEffectMagicPoint ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_APPEARANCE_CODE", adInteger, adParamInput, 4, m_nItemAppearanceCode ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ORIGINAL_ITEM_UID", adBigInt, adParamInput, 8, m_nOriginalItemUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ORIGINAL_ITEM_CNT", adBigInt, adParamInput, 8, m_nOriginalItemCount ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		AuctionManager::Instance().onEndQuery();

		throw;
	}

	AuctionManager::Instance().onEndQuery();

	return true;
}

bool DB_UpdateAuctionBiddingInfo::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateAuctionBiddingInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_auction_bidding_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_auction_bidding_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_AUCTION_UID", adInteger, adParamInput, 4, m_nAuctionUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_BIDDER_LIST", adVarChar, adParamInput, 1024, m_szBidderList.c_str() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HIGHEST_BIDDING_PRICE", adBigInt, adParamInput, 8, m_nHighestBiddingPrice.GetRawData() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HIGHEST_BIDDER_UID", adInteger, adParamInput, 4, m_nHighestBidderUID ) );
		_bstr_t bstrHighestBidderName( m_szHighestBidderName.c_str() );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HIGHEST_BIDDER_NAME", adBSTR, adParamInput, bstrHighestBidderName.length(), bstrHighestBidderName ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		AuctionManager::Instance().onEndQuery();

		throw;
	}

	AuctionManager::Instance().onEndQuery();

	return true;
}

bool DB_DeleteAuctionInfo::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteAuctionInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_delete_auction_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_delete_auction_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_AUCTION_UID", adInteger, adParamInput, 4, m_nAuctionUID ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		AuctionManager::Instance().onEndQuery();

		throw;
	}

	AuctionManager::Instance().onEndQuery();

	return true;
}

DB_InsertNewItemAndKeepingInfo::DB_InsertNewItemAndKeepingInfo( const ItemKeepingInfo *pItemKeeping )
: DBProc( "DB_InsertNewItemAndKeepingInfo" )
	, m_ItemKeepingInfo( *pItemKeeping )
	// pItem만 들고 있으면 onProcess 불리기 전에 세상에 존재하지 않는 아템이 되시는 수가 있음...
	// 현재 존재하는 DB 클래스들의 공통 문제점임. 우선 피하기 위해 필요 정보만 복사
	, m_nItemUID( pItemKeeping->pItem->GetItemUID() )
	, m_nItemCode( pItemKeeping->pItem->GetItemCode() )
	, m_nItemFlag( *pItemKeeping->pItem->GetInstanceFlag().GetRawData() )
	, m_nItemCount( pItemKeeping->pItem->GetCount() )
	, m_nItemLevel( pItemKeeping->pItem->GetItemLevel() )
	, m_nItemEnhance( pItemKeeping->pItem->GetItemEnhance() )
	, m_nItemEtherealDurability( pItemKeeping->pItem->GetCurrentEtherealDurability() )
	, m_nItemEndurance( pItemKeeping->pItem->GetCurrentEndurance() )
	, m_nItemGenCode( pItemKeeping->pItem->GetGenerateCode() )
	, m_nItemRemainTime( pItemKeeping->pItem->GetExpireTime() - time( NULL ) )
	, m_cItemElementalEffectType( pItemKeeping->pItem->GetElementalEffectType() )
	, m_tItemElementalEffectExpire( pItemKeeping->pItem->GetElementalEffectExpireTime() )
	, m_nItemElementalEffectAttackPoint( pItemKeeping->pItem->GetElementalEffectAttackPoint() )
	, m_nItemElementalEffectMagicPoint( pItemKeeping->pItem->GetElementalEffectMagicPoint() )
	, m_nItemAppearanceCode( pItemKeeping->pItem->GetAppearanceCode() )
{
	m_ItemKeepingInfo.pItem = NULL;
}

bool DB_InsertNewItemAndKeepingInfo::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertNewItemAndKeepingInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_new_item_and_keeping_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_new_item_and_keeping_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_UID", adBigInt, adParamInput, 8, m_nItemUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, m_nItemCode ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_FLAG", adInteger, adParamInput, 4, m_nItemFlag ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_CNT", adBigInt, adParamInput, 8, m_nItemCount ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LEVEL", adInteger, adParamInput, 4, m_nItemLevel ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ENHANCE", adInteger, adParamInput, 4, m_nItemEnhance ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ETHEREAL_DURABILITY", adInteger, adParamInput, 4, m_nItemEtherealDurability ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ENDURANCE", adInteger, adParamInput, 4, m_nItemEndurance ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GCODE", adInteger, adParamInput, 4, m_nItemGenCode ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TIME", adInteger, adParamInput, 4, m_nItemRemainTime ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_TYPE", adTinyInt, adParamInput, 1, m_cItemElementalEffectType ) );
		COleDateTime dtItemElementalEffectExpire( m_tItemElementalEffectExpire );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_EXPIRE_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtItemElementalEffectExpire ) ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_ATTACK_POINT", adInteger, adParamInput, 4, m_nItemElementalEffectAttackPoint ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_MAGIC_POINT", adInteger, adParamInput, 4, m_nItemElementalEffectMagicPoint ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_APPEARANCE_CODE", adInteger, adParamInput, 4, m_nItemAppearanceCode ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_KEEPING_UID", adInteger, adParamInput, 4, m_ItemKeepingInfo.nKeepingID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, m_ItemKeepingInfo.nOwnerUID ) );

		COleDateTime dtKeepingExpiration( m_ItemKeepingInfo.tExpiration );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_EXPERATION_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtKeepingExpiration ) ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_TYPE", adInteger, adParamInput, 4, static_cast< int >( m_ItemKeepingInfo.eType ) ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_RELATED_AUCTION_ID", adInteger, adParamInput, 4, m_ItemKeepingInfo.nRelatedAuctionID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RELATED_ITEM_CODE", adInteger, adParamInput, 4, m_ItemKeepingInfo.nRelatedItemCode ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RELATED_ITEM_ENHANCE", adInteger, adParamInput, 4, m_ItemKeepingInfo.nRelatedItemEnhance ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RELATED_ITEM_LEVEL", adInteger, adParamInput, 4, m_ItemKeepingInfo.nRelatedItemLevel ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		AuctionManager::Instance().onEndQuery();

		throw;
	}

	AuctionManager::Instance().onEndQuery();

	return true;
}

bool DB_DeleteItemKeepingInfo::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteItemKeepingInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_delete_item_keeping_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_delete_item_keeping_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_KEEPING_UID", adInteger, adParamInput, 4, m_nItemKeepingUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RETURN_ITEM_TO_OWNER", adBoolean, adParamInput, sizeof( m_bReturnItemToOwner ), m_bReturnItemToOwner ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PREV_OWNED_ITEM_UID", adBigInt, adParamInput, 8, m_nPrevOwnedItemSID ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		AuctionManager::Instance().onEndQuery();

		throw;
	}

	AuctionManager::Instance().onEndQuery();

	return true;
}

DB_DeleteAuctionInfoAndInsertItemKeepingInfo::DB_DeleteAuctionInfoAndInsertItemKeepingInfo( const AuctionUID nAuctionUID, const ItemKeepingInfo *pItemKeeping )
: DBProc( "DB_DeleteAuctionInfoAndInsertItemKeepingInfo" )
	, m_nAuctionUID( nAuctionUID )
	, m_ItemKeepingInfo( *pItemKeeping )
	, m_nItemUID( pItemKeeping->pItem->GetItemUID() )
{
	m_ItemKeepingInfo.pItem = NULL;
}

bool DB_DeleteAuctionInfoAndInsertItemKeepingInfo::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteAuctionInfoAndInsertItemKeepingInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_delete_auction_info_and_insert_item_keeping_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_delete_auction_info_and_insert_item_keeping_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_AUCTION_UID", adInteger, adParamInput, 4, m_nAuctionUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_KEEPING_UID", adInteger, adParamInput, 4, m_ItemKeepingInfo.nKeepingID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_UID", adBigInt, adParamInput, 8, m_nItemUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, m_ItemKeepingInfo.nOwnerUID ) );

		COleDateTime dtKeepingExpiration( m_ItemKeepingInfo.tExpiration );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_EXPERATION_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtKeepingExpiration ) ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_TYPE", adInteger, adParamInput, 4, static_cast< int >( m_ItemKeepingInfo.eType ) ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_RELATED_AUCTION_ID", adInteger, adParamInput, 4, m_ItemKeepingInfo.nRelatedAuctionID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RELATED_ITEM_CODE", adInteger, adParamInput, 4, m_ItemKeepingInfo.nRelatedItemCode ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RELATED_ITEM_ENHANCE", adInteger, adParamInput, 4, m_ItemKeepingInfo.nRelatedItemEnhance ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RELATED_ITEM_LEVEL", adInteger, adParamInput, 4, m_ItemKeepingInfo.nRelatedItemLevel ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		AuctionManager::Instance().onEndQuery();

		throw;
	}

	AuctionManager::Instance().onEndQuery();

	return true;
}

bool DB_UpdateItemAuctionKeepingID::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateItemAuctionKeepingID : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_item_auction_keeping_id" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_item_auction_keeping_id";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_UID", adBigInt, adParamInput, 8, m_nItemUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_AUCTION_UID", adInteger, adParamInput, 4, m_nAuctionUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_KEEPING_UID", adInteger, adParamInput, 4, m_nItemKeepingUID ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		AuctionManager::Instance().onEndQuery();

		throw;
	}

	AuctionManager::Instance().onEndQuery();

	return true;
}

bool DB_InsertAutoAuctionRegistrationInfo::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateItemAuctionKeepingID : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_auto_auction_registration_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_auto_auction_registration_info";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_AUTO_AUCTION_ID", adInteger, adParamInput, 4, m_nAutoAuctionID ) );

		COleDateTime dtScheduledRegisterTime( m_tScheduledRegisterTime );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SCHEDULED_REGISTER_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtScheduledRegisterTime ) ) );
		COleDateTime dtRegisteredTime( m_tRegisteredTime );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_REGISTERED_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtRegisteredTime ) ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		AuctionManager::Instance().onEndQuery();

		throw;
	}

	AuctionManager::Instance().onEndQuery();

	return true;
}