
#include <atlcomtime.h>

#include <mmo/ArcadiaServer.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "DB_Commands.h"
#include "AuctionManager.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructSkill.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"


struct AuctionLoader::dbAuction : public CADORecordBinding
{
	AuctionUID					auction_uid;
	ItemUID		item_uid;
	PlayerUID		seller_uid;
	wchar_t						seller_name[31];
	bool						is_secroute_only;
	DBTIMESTAMP					end_time;
	__int64						instant_purchase_price;
	__int64						registration_tax;
	char						bidder_list[1152];
	__int64						highest_bidding_price;
	PlayerUID		highest_bidder_uid;
	wchar_t						highest_bidder_name[31];

BEGIN_ADO_BINDING(dbAuction)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, auction_uid, sizeof( auction_uid ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adBigInt, item_uid, sizeof( item_uid ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, seller_uid, sizeof( seller_uid ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adVarWChar, seller_name, _countof( seller_name ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adBoolean, is_secroute_only, sizeof( is_secroute_only ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adDBTimeStamp, end_time, sizeof( end_time ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adBigInt, instant_purchase_price, sizeof( instant_purchase_price ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adBigInt, registration_tax, sizeof( registration_tax ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adVarChar, bidder_list, _countof( bidder_list ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, adBigInt, highest_bidding_price, sizeof( highest_bidding_price ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, adInteger, highest_bidder_uid, sizeof( highest_bidder_uid ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, adVarWChar, highest_bidder_name, _countof( highest_bidder_name ), FALSE)
END_ADO_BINDING()
};

struct AuctionLoader::dbItemKeeping : public CADORecordBinding
{
	ItemKeepingUID				keeping_uid;
	ItemUID		item_uid;
	PlayerUID		owner_uid;
	DBTIMESTAMP					expiration_time;
	int							keeping_type;
	AuctionUID					related_auction_id;
	ItemBase::ItemCode			related_item_code;
	int							related_item_enhance;
	int							related_item_level;

BEGIN_ADO_BINDING(dbItemKeeping)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, keeping_uid, sizeof( keeping_uid ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adBigInt, item_uid, sizeof( item_uid ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, owner_uid, sizeof( owner_uid ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adDBTimeStamp, expiration_time, sizeof( expiration_time ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, keeping_type, sizeof( keeping_type ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, related_auction_id, sizeof( related_auction_id ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adInteger, related_item_code, sizeof( related_item_code ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, related_item_enhance, sizeof( related_item_enhance ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, related_item_level, sizeof( related_item_level ), FALSE)
END_ADO_BINDING()
};

bool AuctionLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		_ConnectionPtr		ConnPtr = NULL;
		InitUserDbConnection( ConnPtr );
		// 느린 쿼리 타임아웃 조정
		ConnPtr->CommandTimeout = 5*60;	// 5분

		try
		{
			helper.start();
			loadAuctionAndKeepingSummonList( ConnPtr );
			helper.end( "dbo.smp_read_auction_and_keeping_summon_list" );
		}
		catch( _com_error &e )
		{
			helper.end( "dbo.smp_read_auction_and_keeping_summon_list" );
			LogDBError( e, "AuctionLoader", "dbo.smp_read_auction_and_keeping_summon_list" );
			throw;
		}

		try
		{
			helper.start();
			loadAuctionAndKeepingPetList( ConnPtr );
			helper.end( "dbo.smp_read_auction_and_keeping_pet_list" );
		}
		catch( _com_error &e )
		{
			helper.end( "dbo.smp_read_auction_and_keeping_pet_list" );
			LogDBError( e, "AuctionLoader", "dbo.smp_read_auction_and_keeping_pet_list" );
			throw;
		}

		try
		{
			helper.start();
			_loadRandomOptionList( ConnPtr );
			helper.end( "dbo.smp_read_auction_awaken_option_list" );
		}
		catch( _com_error &e )
		{
			helper.end( "dbo.smp_read_auction_awaken_option_list" );
			LogDBError( e, "AuctionLoader", "dbo.smp_read_auction_awaken_option_list" );
			throw;
		}

		try
		{
			helper.start();
			loadAuctionAndKeepingItem( ConnPtr );
			helper.end( "smp_read_auction_and_keeping_item_list" );
		}
		catch( _com_error &e )
		{
			helper.end( "smp_read_auction_and_keeping_item_list" );
			LogDBError( e, "AuctionLoader", "smp_read_auction_and_keeping_item_list" );
			throw;
		}

		try
		{
			helper.start();
			loadAuctionInfo( ConnPtr );
			helper.end( "Auction table binding" );
		}
		catch( _com_error &e )
		{
			helper.end( "Auction table binding" );
			LogDBError( e, "AuctionLoader", "Auction table binding" );
			throw;
		}

		try
		{
			helper.start();
			loadItemKeepingInfo( ConnPtr );
			helper.end( "ItemKeeping table binding" );
		}
		catch( _com_error &e )
		{
			helper.end( "ItemKeeping table binding" );
			LogDBError( e, "AuctionLoader", "ItemKeeping table binding" );
			throw;
		}

		clearLoadedItem();
		clearLoadedSummonAndPet();
		clearLoadedRandomOption();
	}
	catch( _com_error &e )
	{
		std::string strError = "AuctionLoader RESOURCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}

bool AuctionLoader::loadAuctionAndKeepingSummonList( _ConnectionPtr & ConnPtr )
{
	_CommandPtr cmd;
	if( CreateDBCommand( cmd, ConnPtr ) == false )	throw XException( "loadAuctionAndKeepingSummonList : CreateInstance(command) error" );

	// 느린 쿼리 타임아웃 조정
	ConnPtr->CommandTimeout = 5*60;	// 5분
	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_auction_and_keeping_summon_list" );

	_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		StructSummon *pSummon = StructSummon::AllocSummon( NULL, pRS->Fields->Item["code"]->Value );

		// _oprint( "ALLOC SUMMON : %08X\n", pSummon );
		pSummon->SetEXP( pRS->Fields->Item["exp"]->Value );
		pSummon->SetLevel( pRS->Fields->Item["lv"]->Value );
		pSummon->SetJobLevel( pRS->Fields->Item["lv"]->Value );
		pSummon->SetMaxReachedLevel( pRS->Fields->Item["max_level"]->Value );
		pSummon->SetSummonSID( pRS->Fields->Item["sid"]->Value );
		pSummon->SetJP( pRS->Fields->Item["jp"]->Value );
		pSummon->SetName( static_cast< _bstr_t >( pRS->Fields->Item["name"]->Value ) );
		pSummon->SetCurrentXY( 0, 0 );
		pSummon->SetPrevJobId( 0, pRS->Fields->Item["prev_id_01"]->Value );
		pSummon->SetPrevJobId( 1, pRS->Fields->Item["prev_id_02"]->Value );
		pSummon->SetPrevJobLevel( 0, pRS->Fields->Item["prev_level_01"]->Value );
		pSummon->SetPrevJobLevel( 1, pRS->Fields->Item["prev_level_02"]->Value );
		pSummon->SetLastDecreasedEXP( pRS->Fields->Item["last_decreased_exp"]->Value.llVal );

		m_vLoadedSummonList.push_back( pSummon );

		_loadSummonSkillList( ConnPtr, pSummon );

		pSummon->CalculateStat();

		pSummon->SetSP( pRS->Fields->Item["sp"]->Value );
		pSummon->SetHP( pRS->Fields->Item["hp"]->Value );
		pSummon->SetMP( pRS->Fields->Item["mp"]->Value );

		pRS->MoveNext();
	}

	return true;
}

bool AuctionLoader::loadAuctionAndKeepingPetList( _ConnectionPtr & ConnPtr )
{
	_CommandPtr cmd;
	if( CreateDBCommand( cmd, ConnPtr ) == false )	throw XException( "loadAuctionAndKeepingPetList : CreateInstance(command) error" );

	// 느린 쿼리 타임아웃 조정
	ConnPtr->CommandTimeout = 5*60;	// 5분
	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_auction_and_keeping_pet_list" );

	_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		StructPet *pPet = StructPet::AllocPet( NULL, pRS->Fields->Item["code"]->Value );

		pPet->SetPetSID( pRS->Fields->Item["sid"]->Value );
		pPet->SetName( static_cast< _bstr_t >( pRS->Fields->Item["name"]->Value ) );
		pPet->SetNameChanged( pRS->Fields->Item["name_changed"]->Value );
		pPet->GetSkill( StructSkill::SKILL_SHOVELING )->SetRemainCoolTime( pRS->Fields->Item["cool_time_01"]->Value.intVal * 100 );

		m_vLoadedPetList.push_back( pPet );
		
		pRS->MoveNext();
	}

	return true;
}

bool AuctionLoader::_loadSummonSkillList( _ConnectionPtr & ConnPtr, StructSummon * pSummon )
{
	_CommandPtr cmd;
	if( CreateDBCommand( cmd, ConnPtr ) == false )	throw XException( "_loadSummonSkillList : CreateInstance(command) error" );

	// 느린 쿼리 타임아웃 조정
	ConnPtr->CommandTimeout = 5*60;	// 5분
	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_summon_skill_list" );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, pSummon->GetSummonSID() ) );

	_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{	
		pSummon->SetSkill(	pRS->Fields->Item["sid"]->Value,
							pRS->Fields->Item["skill_id"]->Value,
							pRS->Fields->Item["skill_level"]->Value,
							pRS->Fields->Item["cool_time"]->Value );
		pRS->MoveNext();
	}

	return true;
}

bool AuctionLoader::loadAuctionAndKeepingItem( _ConnectionPtr & ConnPtr )
{
	_RecordsetPtr		pRstItemInfo = NULL;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	HRESULT  hr = S_OK;

	pRstItemInfo.CreateInstance( __uuidof(Recordset) );

	pRstItemInfo->Open( "smp_read_auction_and_keeping_item_list",
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdStoredProc );

	pRstItemInfo->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );

	StructItem * pItem;
	for( ; pRstItemInfo->State != adStateClosed && !pRstItemInfo->EndOfFile; pRstItemInfo->MoveNext() )
	{
		__int64 id = pRstItemInfo->Fields->Item["sid"]->Value;

		COleDateTime dtElementalEffectExpire( pRstItemInfo->Fields->Item["elemental_effect_expire_time"]->Value.date );

		struct tm tmElementalEffectExpire;
		tmElementalEffectExpire.tm_year = dtElementalEffectExpire.GetYear() - 1900;
		tmElementalEffectExpire.tm_mon = dtElementalEffectExpire.GetMonth() - 1;
		tmElementalEffectExpire.tm_mday = dtElementalEffectExpire.GetDay();
		tmElementalEffectExpire.tm_hour = dtElementalEffectExpire.GetHour();
		tmElementalEffectExpire.tm_min = dtElementalEffectExpire.GetMinute();
		tmElementalEffectExpire.tm_sec = dtElementalEffectExpire.GetSecond();
		tmElementalEffectExpire.tm_isdst = -1;
		time_t tElementalEffectExpire = mktime( &tmElementalEffectExpire );

		pItem = StructItem::AllocItem(	pRstItemInfo->Fields->Item["sid"]->Value,
										pRstItemInfo->Fields->Item["code"]->Value,
										pRstItemInfo->Fields->Item["cnt"]->Value.llVal,
										static_cast< ItemInstance::GenerateCode >( static_cast< int >( pRstItemInfo->Fields->Item["gcode"]->Value ) ),
										pRstItemInfo->Fields->Item["level"]->Value,
										pRstItemInfo->Fields->Item["enhance"]->Value,
										pRstItemInfo->Fields->Item["flag"]->Value,
										pRstItemInfo->Fields->Item["socket_0"]->Value,
										pRstItemInfo->Fields->Item["socket_1"]->Value,
										pRstItemInfo->Fields->Item["socket_2"]->Value,
										pRstItemInfo->Fields->Item["socket_3"]->Value,
										pRstItemInfo->Fields->Item["awaken_sid"]->Value,
										pRstItemInfo->Fields->Item["random_option_sid"]->Value,
										std::max( (int) pRstItemInfo->Fields->Item["remain_time"]->Value, 0 ),
										pRstItemInfo->Fields->Item["elemental_effect_type"]->Value.intVal,
										tElementalEffectExpire,
										pRstItemInfo->Fields->Item["elemental_effect_attack_point"]->Value,
										pRstItemInfo->Fields->Item["elemental_effect_magic_point"]->Value,
										pRstItemInfo->Fields->Item["appearance_code"]->Value,
										
										// imPakrht - smp_read_auction_and_keeping_item_list 추가 summon_code
										pRstItemInfo->Fields->Item["summon_code"]->Value );

		pItem->SetCurrentEtherealDurability( pRstItemInfo->Fields->Item["ethereal_durability"]->Value );
		pItem->SetCurrentEndurance( pRstItemInfo->Fields->Item["endurance"]->Value );
		pItem->SetIdx( 0 );
		pItem->SetWearInfo( ItemBase::WEAR_NONE, true );
		pItem->SetAuctionID( pRstItemInfo->Fields->Item["auction_id"]->Value );
		pItem->SetItemKeepingID( pRstItemInfo->Fields->Item["keeping_id"]->Value );
		pItem->TurnOffDbUpdateFlag();

		if( pItem->IsAwaken() )
		{
			ItemInstance::RANDOM_OPTION kAwakenOption;
			_getRandomOption( pItem->GetAwakenSID(), &kAwakenOption );
			pItem->SetAwakenOption( kAwakenOption );
		}

		if( pItem->IsIdentified() )
		{
			ItemInstance::RANDOM_OPTION kRandomOption;
			_getRandomOption( pItem->GetIdentifiedSID(), &kRandomOption );
			pItem->SetIdentifiedOption( kRandomOption );
		}

		// 소환수 카드일 경우 소환수 정보와 아이템 정보를 연결
		if( pItem->IsSummonCard() && pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON ) )
		{
			for( std::vector< StructSummon * >::iterator it = m_vLoadedSummonList.begin() ; it != m_vLoadedSummonList.end() ; ++it )
			{
				if( (*it)->GetSummonSID() == pItem->GetSummonSID() )
				{
					StructSummon *pSummon = (*it);

					m_vLoadedSummonList.erase( it );

					// 소환수 카드와 아이템의 소환수 코드가 잘못 연결 되 있으면 제대로 연결
					if( pItem->GetSummonCode() != pSummon->GetSummonCode() )
					{
						pItem->SetSummonCode( pSummon->GetSummonCode() );
						pItem->DBQuery( new DB_UpdateItem( pItem ) );
					}

					pSummon->SetParentCard( pItem );
					pItem->SetSummonStruct( pSummon );
					pItem->SetSummonSID( pSummon->GetSummonSID() );

					pSummon->SetLoginComplete();

					break;
				}
			}
		}

		// 펫 아이템 일 경우 펫 정보와 아이템 정보를 연결
		if( pItem->IsPetCage() )
		{
			for( std::vector< StructPet * >::iterator it = m_vLoadedPetList.begin() ; it != m_vLoadedPetList.end() ; ++it )
			{
				if( (*it)->GetPetSID() == pItem->GetPetSID() )
				{
					StructPet *pPet = (*it);
					
					m_vLoadedPetList.erase( it );

					pPet->SetParentCage( pItem );
					pItem->SetPetStruct( pPet );

					pPet->SetLoginComplete();
					
					break;
				}
			}
		}

		if( !pushLoadedItem( pItem ) )
		{
			_cprint( "Duplicated loaded auction item detected: sid(%I64d) auction_id(%d) keeping_id(%d)\n", pItem->GetItemUID(), pItem->GetAuctionID(), pItem->GetItemKeepingID() );
			FILELOG( "Duplicated loaded auction item detected: sid(%I64d) auction_id(%d) keeping_id(%d)", pItem->GetItemUID(), pItem->GetAuctionID(), pItem->GetItemKeepingID() );
			AuctionManager::Instance().Push( new DB_UpdateItemAuctionKeepingID( pItem->GetItemUID(), 0, 0 ) );
			StructItem::PendFreeItem( pItem );
		}
	}

	return true;
}

bool AuctionLoader::_loadRandomOptionList( _ConnectionPtr & ConnPtr )
{
	_CommandPtr cmd;
	if( CreateDBCommand( cmd, ConnPtr ) == false )	throw XException( "_loadRandomOptionList : CreateInstance(command) error" );

	// 느린 쿼리 타임아웃 조정
	ConnPtr->CommandTimeout = 5*60;	// 5분
	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_auction_random_option_list" );

	_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile ; pRS->MoveNext() )
	{
		struct ItemInstance::RANDOM_OPTION RandomOption;
		std::string strBuffer;
		_decimal_variant decBuffer;

		RandomOption.nSID		= pRS->Fields->Item["sid"]->Value;
		RandomOption.nRandomType= pRS->Fields->Item["random_type"]->Value;

		for( int i = 0; i < ItemInstance::MAX_RANDOM_OPTION_NUMBER; ++i )
		{
			XStringUtil::Format( strBuffer, "type_%02d", i + 1 );
			RandomOption.OptionInfo[i].nType = pRS->Fields->Item[strBuffer.c_str()]->Value;

			XStringUtil::Format( strBuffer, "value1_%02d", i + 1 );
			*static_cast< DECIMAL * >( &decBuffer ) = pRS->Fields->Item[strBuffer.c_str()]->Value.decVal;
			RandomOption.OptionInfo[i].fValue1.set( decBuffer.getMultipleInteger( 10000 ) );

			XStringUtil::Format( strBuffer, "value2_%02d", i + 1 );
			*static_cast< DECIMAL * >( &decBuffer ) = pRS->Fields->Item[strBuffer.c_str()]->Value.decVal;
			RandomOption.OptionInfo[i].fValue2.set( decBuffer.getMultipleInteger( 10000 ) );
		}

		m_vRandomOptionList.push_back( RandomOption );
	}

	return true;
}

void AuctionLoader::_getRandomOption( int _SID, ItemInstance::RANDOM_OPTION * _pRandomOption )
{
	std::vector< ItemInstance::RANDOM_OPTION >::iterator it = m_vRandomOptionList.begin();
	for( ; it != m_vRandomOptionList.end() ; ++it )
	{
		if( _SID == (*it).nSID )
		{
			_pRandomOption->nSID = _SID;
			_pRandomOption->nRandomType = it->nRandomType;

			for( int nCnt = 0 ; nCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++nCnt )
			{
				_pRandomOption->OptionInfo[ nCnt ].nType	= (*it).OptionInfo[ nCnt ].nType;
				_pRandomOption->OptionInfo[ nCnt ].fValue1	= (*it).OptionInfo[ nCnt ].fValue1;
				_pRandomOption->OptionInfo[ nCnt ].fValue2	= (*it).OptionInfo[ nCnt ].fValue2;
			}

			m_vRandomOptionList.erase( it );
			return;
		}
	}
}

bool AuctionLoader::loadAuctionInfo( _ConnectionPtr & ConnPtr )
{
	_RecordsetPtr		pRstAuctionInfo = NULL;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	HRESULT  hr = S_OK;
	
	dbAuction emprs;       // C++ Class object

	pRstAuctionInfo.CreateInstance( __uuidof(Recordset) );

	pRstAuctionInfo->Open("Auction", 
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdTable);

	pRstAuctionInfo->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );

	picRs->BindToRecordset(&emprs);

	int nTotalCnt = 0;
	int nLoadCount = 0;

	while( pRstAuctionInfo->State != adStateClosed && !pRstAuctionInfo->EndOfFile )
	{
		if( onAuctionInfoData( &emprs ) )
			++nLoadCount;

		hr = pRstAuctionInfo->MoveNext();
		++nTotalCnt;
	}

	_cprint( "Total %d/%d AuctionInfo loaded...\n", nLoadCount, nTotalCnt );
	FILELOG( "Total %d/%d AuctionInfo loaded...", nLoadCount, nTotalCnt );

	return true;
}

bool AuctionLoader::loadItemKeepingInfo( _ConnectionPtr & ConnPtr )
{
	_RecordsetPtr		pRstItemKeepingInfo = NULL;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	HRESULT  hr = S_OK;
	
	dbItemKeeping emprs;       // C++ Class object

	pRstItemKeepingInfo.CreateInstance( __uuidof(Recordset) );

	pRstItemKeepingInfo->Open("ItemKeeping", 
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdTable);

	pRstItemKeepingInfo->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );

	picRs->BindToRecordset(&emprs);

	int nTotalCnt = 0;
	int nLoadCount = 0;

	while( pRstItemKeepingInfo->State != adStateClosed && !pRstItemKeepingInfo->EndOfFile )
	{
		if( onItemKeepingInfoData( &emprs ) )
			++nLoadCount;

		hr = pRstItemKeepingInfo->MoveNext();
		++nTotalCnt;
	}

	_cprint( "Total %d/%d ItemKeepingInfo loaded...\n", nLoadCount, nTotalCnt );
	FILELOG( "Total %d/%d ItemKeepingInfo loaded...", nLoadCount, nTotalCnt );

	return true;
}

bool AuctionLoader::onAuctionInfoData( dbAuction * emprs )
{
	if( !emprs->auction_uid )
	{
		return false;
	}

	// 새로 생성될 경매 ID를 현재까지 로딩된 ID보다 커지도록 조정
	UIDManager::Instance().SetMaxUsedUID( "AuctionUID", emprs->auction_uid );

	StructItem *pItem = popLoadedItem( emprs->item_uid );
	if( !pItem )
	{
		_cprint( "AuctionInfo without related item detected: auction_id(%d) item_id(%I64d) seller_id(%d) registration_tax(%I64d) bidder_list(%s) highest_bidder_id(%d) bidding_price(%I64d)\n",
			emprs->auction_uid, emprs->item_uid, emprs->seller_uid, emprs->registration_tax, emprs->bidder_list, emprs->highest_bidder_uid, emprs->highest_bidding_price );
		FILELOG( "AuctionInfo without related item detected: auction_id(%d) item_id(%I64d) seller_id(%d) registration_tax(%I64d) bidder_list(%s) highest_bidder_id(%d) bidding_price(%I64d)",
			emprs->auction_uid, emprs->item_uid, emprs->seller_uid, emprs->registration_tax, emprs->bidder_list, emprs->highest_bidder_uid, emprs->highest_bidding_price );

		AuctionManager::Instance().Push( new DB_DeleteAuctionInfo( emprs->auction_uid ) );
		return false;
	}

	// 아이템 데이터가 경매 데이터와 매치되지 않는 경우
	if( pItem->GetAuctionID() != emprs->auction_uid || pItem->GetItemKeepingID() > 0 )
	{
		_cprint( "AuctionInfo with mis-related item detected: auction_id(%d) item_id(%I64d) keeping_id_on_item(%d) seller_id(%d) registration_tax(%I64d) bidder_list(%s) highest_bidder_id(%d) bidding_price(%I64d)\n",
			emprs->auction_uid, emprs->item_uid, pItem->GetItemKeepingID(), emprs->seller_uid, emprs->registration_tax, emprs->bidder_list, emprs->highest_bidder_uid, emprs->highest_bidding_price );
		FILELOG( "AuctionInfo with mis-related item detected: auction_id(%d) item_id(%I64d) keeping_id_on_item(%d) seller_id(%d) registration_tax(%I64d) bidder_list(%s)  highest_bidder_id(%d) bidding_price(%I64d)",
			emprs->auction_uid, emprs->item_uid, pItem->GetItemKeepingID(), emprs->seller_uid, emprs->registration_tax, emprs->bidder_list, emprs->highest_bidder_uid, emprs->highest_bidding_price );

		pushLoadedItem( pItem );
		AuctionManager::Instance().Push( new DB_DeleteAuctionInfo( emprs->auction_uid ) );
		return false;
	}

	struct tm tmEndTime;
	tmEndTime.tm_year = emprs->end_time.year - 1900;
	tmEndTime.tm_mon = emprs->end_time.month - 1;
	tmEndTime.tm_mday = emprs->end_time.day;
	tmEndTime.tm_hour = emprs->end_time.hour;
	tmEndTime.tm_min = emprs->end_time.minute;
	tmEndTime.tm_sec = emprs->end_time.second;
	tmEndTime.tm_isdst = -1;
	time_t tEndTime = mktime( &tmEndTime );

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	char szSellerName[62];
	WideCharToMultiByte( code_page, 0, emprs->seller_name, _countof( emprs->seller_name ), szSellerName, _countof( szSellerName ), NULL, NULL );
	char szHighestBidderName[62];
	WideCharToMultiByte( code_page, 0, emprs->highest_bidder_name, _countof( emprs->highest_bidder_name ), szHighestBidderName, _countof( szHighestBidderName ), NULL, NULL );

	AuctionInfo *pAuctionInfo = new AuctionInfo( emprs->auction_uid, pItem, emprs->seller_uid, szSellerName, emprs->is_secroute_only, tEndTime, emprs->instant_purchase_price, emprs->registration_tax, emprs->bidder_list, emprs->highest_bidding_price, emprs->highest_bidder_uid, szHighestBidderName );
	if( !pAuctionInfo )
	{
		// 여긴 데이터 자체가 잘못된 게 아니라 메모리 할당에 실패한 것이므로
		// DB를 갱신하지 말고 놔둬서 다음 번 로딩 때 사용되도록 함
		// 여기서 pItem을 다시 push 해두면 다른 경매에서 사용되지 않을 가능성이 높고
		// (중첩 등록이 있지 않은 이상 없음) 그러면 auction_id, keeping_id 가 0으로 리셋됨
		// (clearLoadedItem 함수 호출 및 역할 참조)
		//pushLoadedItem( pItem );
		_cprint( "Memory allocation failed for auction: auction_id(%d) item_id(%I64d)\n", emprs->auction_uid, emprs->item_uid );
		FILELOG( "Memory allocation failed for auction: auction_id(%d) item_id(%I64d)", emprs->auction_uid, emprs->item_uid );
		return false;
	}

	if( !AuctionManager::Instance().AddAuctionInfoByLoading( pAuctionInfo ) )
	{
		_cprint( "Duplicated auction id detected: auction_id(%d) item_id(%I64d) seller_id(%d) registration_tax(%I64d) bidder_list(%s) highest_bidder_id(%d) bidding_price(%I64d)\n",
			emprs->auction_uid, emprs->item_uid, emprs->seller_uid, emprs->registration_tax, emprs->bidder_list, emprs->highest_bidder_uid, emprs->highest_bidding_price );
		FILELOG( "Duplicated auction id detected: auction_id(%d) item_id(%I64d) seller_id(%d) registration_tax(%I64d) bidder_list(%s) highest_bidder_id(%d) bidding_price(%I64d)",
			emprs->auction_uid, emprs->item_uid, emprs->seller_uid, emprs->registration_tax, emprs->bidder_list, emprs->highest_bidder_uid, emprs->highest_bidding_price );

		pushLoadedItem( pItem );
		delete pAuctionInfo;
		AuctionManager::Instance().Push( new DB_DeleteAuctionInfo( emprs->auction_uid ) );
		return false;
	}

	return true;
}

bool AuctionLoader::onItemKeepingInfoData( dbItemKeeping * emprs )
{
	if( !emprs->keeping_uid )
	{
		return false;
	}

	// 새로 생성될 아이템 보관 데이터 ID를 현재까지 로딩된 ID보다 커지도록 조정
	UIDManager::Instance().SetMaxUsedUID( "ItemKeepingUID", emprs->keeping_uid );

	StructItem *pItem = popLoadedItem( emprs->item_uid );
	if( !pItem )
	{
		_cprint( "ItemKeepingInfo without related item detected: keeping_id(%d) item_id(%I64d) owner_id(%d)\n", emprs->keeping_uid, emprs->item_uid, emprs->owner_uid );
		FILELOG( "ItemKeepingInfo without related item detected: keeping_id(%d) item_id(%I64d) owner_id(%d)", emprs->keeping_uid, emprs->item_uid, emprs->owner_uid );

		AuctionManager::Instance().Push( new DB_DeleteItemKeepingInfo( emprs->keeping_uid, false, 0 ) );
		return false;
	}

	// 아이템 데이터가 아이템 보관 데이터와 매치되지 않는 경우
	if( pItem->GetItemKeepingID() != emprs->keeping_uid || pItem->GetAuctionID() > 0 )
	{
		_cprint( "ItemKeepingInfo with mis-related item detected: keeping_id(%d) item_id(%I64d) auction_id_on_item(%d) owner_id(%d)\n", emprs->keeping_uid, emprs->item_uid, pItem->GetAuctionID(), emprs->owner_uid );
		FILELOG( "ItemKeepingInfo with mis-related item detected: keeping_id(%d) item_id(%I64d) auction_id_on_item(%d) owner_id(%d)", emprs->keeping_uid, emprs->item_uid, pItem->GetAuctionID(), emprs->owner_uid );

		pushLoadedItem( pItem );
		AuctionManager::Instance().Push( new DB_DeleteItemKeepingInfo( emprs->keeping_uid, false, 0 ) );
		return false;
	}

	struct tm tmExpirationTime;
	tmExpirationTime.tm_year = emprs->expiration_time.year - 1900;
	tmExpirationTime.tm_mon = emprs->expiration_time.month - 1;
	tmExpirationTime.tm_mday = emprs->expiration_time.day;
	tmExpirationTime.tm_hour = emprs->expiration_time.hour;
	tmExpirationTime.tm_min = emprs->expiration_time.minute;
	tmExpirationTime.tm_sec = emprs->expiration_time.second;
	tmExpirationTime.tm_isdst = -1;
	time_t tExpiration = mktime( &tmExpirationTime );

	ItemKeepingInfo *pItemKeeping = new ItemKeepingInfo( emprs->keeping_uid, pItem, emprs->owner_uid, tExpiration, static_cast< KEEPING_TYPE >( emprs->keeping_type ),
		emprs->related_auction_id, emprs->related_item_code, emprs->related_item_enhance, emprs->related_item_level );
	if( !pItemKeeping )
	{
		// 여긴 데이터 자체가 잘못된 게 아니라 메모리 할당에 실패한 것이므로
		// DB를 갱신하지 말고 놔둬서 다음 번 로딩 때 사용되도록 함
		// 여기서 pItem을 다시 push 해두면 다른 아이템 보관 데이터에서 사용되지 않을 가능성이 높고
		// (중첩 등록이 있지 않은 이상 없음) 그러면 auction_id, keeping_id 가 0으로 리셋됨
		// (clearLoadedItem 함수 호출 및 역할 참조)
		//pushLoadedItem( pItem );
		// 아이템 데이터는 더 이상 현재 서버 상태에서는 사용되지 못하므로 메모리에서 해제해 줌
		StructItem::PendFreeItem( pItem );

		_cprint( "Memory allocation failed for item keeping: keeping_id(%d) item_id(%I64d)\n", emprs->keeping_uid, emprs->item_uid );
		FILELOG( "Memory allocation failed for item keeping: keeping_id(%d) item_id(%I64d)", emprs->keeping_uid, emprs->item_uid );

		return false;
	}

	if( !AuctionManager::Instance().AddItemKeepingInfoByLoading( pItemKeeping ) )
	{
		_cprint( "Duplicated item keeping id detected: keeping_id(%d) item_id(%I64d) owner_id(%d)\n", emprs->keeping_uid, emprs->item_uid, emprs->owner_uid );
		FILELOG( "Duplicated item keeping id detected: keeping_id(%d) item_id(%I64d) owner_id(%d)", emprs->keeping_uid, emprs->item_uid, emprs->owner_uid );

		assert( 0 );
		pushLoadedItem( pItem );
		delete pItemKeeping;
		AuctionManager::Instance().Push( new DB_DeleteItemKeepingInfo( emprs->keeping_uid, false, 0 ) );
		return false;
	}

	return true;
}

bool AuctionLoader::pushLoadedItem( StructItem *pItem )
{
	for( std::vector< StructItem * >::iterator it = m_vLoadedItemList.begin() ; it != m_vLoadedItemList.end() ; ++it )
	{
		// 이미 등록된 아이템 체크
		if( (*it) == pItem )
		{
			assert( 0 );
			return false;
		}

		// 이미 등록되어 있는 ItemUID
		if( (*it)->GetItemUID() == pItem->GetItemUID() )
		{
			assert( 0 );
			return false;
		}
	}

	m_vLoadedItemList.push_back( pItem );
	return true;
}

StructItem * AuctionLoader::popLoadedItem( __int64 nItemUID )
{
	for( std::vector< StructItem * >::iterator it = m_vLoadedItemList.begin() ; it != m_vLoadedItemList.end() ; ++it )
	{
		if( (*it)->GetItemUID() == nItemUID )
		{
			StructItem *pItem = (*it);
			m_vLoadedItemList.erase( it );
			return pItem;
		}
	}

	return NULL;
}

void AuctionLoader::clearLoadedItem()
{
	for( std::vector< StructItem * >::iterator it = m_vLoadedItemList.begin() ; it != m_vLoadedItemList.end() ; ++it )
	{
		_cprint("Unused loaded auction item detected: sid(%I64d) auction_id(%d) keeping_id(%d)\n", (*it)->GetItemUID(), (*it)->GetAuctionID(), (*it)->GetItemKeepingID() );
		FILELOG("Unused loaded auction item detected: sid(%I64d) auction_id(%d) keeping_id(%d)", (*it)->GetItemUID(), (*it)->GetAuctionID(), (*it)->GetItemKeepingID() );
		AuctionManager::Instance().Push( new DB_UpdateItemAuctionKeepingID( (*it)->GetItemUID(), 0, 0 ) );
		StructItem::PendFreeItem( (*it) );
	}

	m_vLoadedItemList.clear();
}

void AuctionLoader::clearLoadedSummonAndPet()
{
	for( std::vector< StructSummon * >::iterator it = m_vLoadedSummonList.begin() ; it != m_vLoadedSummonList.end() ; ++it )
	{
		_cprint("Unused loaded auction/keeping summon detected: sid(%d)\n", (*it)->GetSummonSID() );
		FILELOG("Unused loaded auction/keeping summon detected: sid(%d)", (*it)->GetSummonSID() );
		ArcadiaServer::Instance().DeleteObject( (*it) );
	}

	for( std::vector< StructPet * >::iterator it = m_vLoadedPetList.begin() ; it != m_vLoadedPetList.end() ; ++it )
	{
		_cprint("Unused loaded auction/keeping pet detected: sid(%d)\n", (*it)->GetPetSID() );
		FILELOG("Unused loaded auction/keeping pet detected: sid(%d)", (*it)->GetPetSID() );
		ArcadiaServer::Instance().DeleteObject( (*it) );
	}

	m_vLoadedSummonList.clear();
	m_vLoadedPetList.clear();
}

void AuctionLoader::clearLoadedRandomOption()
{
	for( std::vector< struct ItemInstance::RANDOM_OPTION >::iterator it = m_vRandomOptionList.begin() ; it != m_vRandomOptionList.end() ; ++it )
	{
		_cprint("Unused loaded awaken option detected: SID(%d)\n", it->nSID );
		FILELOG("Unused loaded awaken option detected: SID(%d)", it->nSID );
	}

	m_vRandomOptionList.clear();
}
