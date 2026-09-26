
#include <oledb.h>
#include <icrsint.h>

#include <network/IConnection.h>
#include <network/XIOCPConnection.h>
#include <toolkit/XConsole.h>
#include <toolkit/XENV.h>
#include <mmo/ArcadiaServer.h>

#include "LogClient/LogClient.h"

#include "DB_Commands.h"

#include "StructPlayer.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "StructItem.h"
#include "StructSummon.h"
#include "GameProc.h"
#include "GameRule.h"



bool DB_GetCommercialStorageInfo::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_GetCommercialStorageInfo : CreateInstance(command) error" );

		// 느린 쿼리 타임아웃 조정
		cmd->CommandTimeout = 60;
		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_check_purchased_item" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_check_purchased_item";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_CHARACTER_ID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );

		cmd->Parameters->Append( cmd->CreateParameter( "OUT_TOTAL_ITEM_COUNT", adInteger, adParamOutput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_NEW_ITEM_COUNT", adInteger, adParamOutput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_PREMIUM_TICKET", adInteger, adParamOutput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_PREMIUM_REST_TIME", adInteger, adParamOutput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_STAMINA_REGEN_TIME", adInteger, adParamOutput, 4, 0 ) );

		_variant_t vNull;
		vNull.vt=VT_ERROR;
		vNull.scode=DISP_E_PARAMNOTFOUND;

		cmd->Execute(&vNull,&vNull,adCmdStoredProc);

		if(	cmd->Parameters->Item[ "OUT_TOTAL_ITEM_COUNT" ]->Value.vt != VT_NULL )
		{
			::TS_SC_COMMERCIAL_STORAGE_INFO msg;
			msg.total_item_count	= cmd->Parameters->Item[ "OUT_TOTAL_ITEM_COUNT" ]->Value;
			msg.new_item_count		= cmd->Parameters->Item[ "OUT_NEW_ITEM_COUNT" ]->Value;
			PendMessage( m_pPlayer, &msg );
		}

		if(	cmd->Parameters->Item[ "OUT_PREMIUM_TICKET" ]->Value.vt != VT_NULL )
		{
			int bIsPremiumUser = cmd->Parameters->Item[ "OUT_PREMIUM_TICKET" ]->Value;
			int nRestSecond = cmd->Parameters->Item[ "OUT_PREMIUM_REST_TIME" ]->Value;

			// 계정 권한 DB가 있는 서버라면, 전 동기화 과정에서 이미 시크루트 처리 됨.
			if( !GameRule::bUseAccountAuthorityDB )
				m_pPlayer->SetSetSecrouteFreePass( (bIsPremiumUser != 0), nRestSecond );
		}

		// 로그인과 함께 처리되는 중일 경우에 필요한 추가 처리
		if( m_bIsProcessingLogin )
		{
			// 시크루트 귀환권 아이템 지급 후에 퀵슬롯 정보를 보내야 하기 때문에 여기서 전송
			// (캐쉬 아이템 사용 가능 서버가 아닌 경우에는 SendLoginResult에서 보냄)
			// !! 클라이언트 세팅 정보를 보낼 때에는 client_info를 항상 먼저 보내야 한다.
			SendPropertyMessage( m_pPlayer, m_pPlayer->GetHandle(), "client_info", m_pPlayer->GetClientInfo() );
			SendPropertyMessage( m_pPlayer, m_pPlayer->GetHandle(), "quick_slot", m_pPlayer->GetQuickSlot() );
			SendPropertyMessage( m_pPlayer, m_pPlayer->GetHandle(), "current_key", m_pPlayer->GetCurrentKey() );
			SendPropertyMessage( m_pPlayer, m_pPlayer->GetHandle(), "saved_key", m_pPlayer->GetSavedKey() );

			// 시크루트 프리패스가 로그아웃 중에 종료됐을 경우 스테미너 회복
			// (프리패스가 종료되지 않았거나 로그아웃 이전에 종료됐으면 OUT_STAMINA_REGEN_TIME이 0으로 나온다)
			// 여기서는 텐트 사용자인지, 프리패스 유저인지 모든게 분명함
			int regen_rate = m_pPlayer->GetStaminaRegenRate();

			if( m_pPlayer->IsUsingTent() || m_pPlayer->IsGaiaMember() )
			{
				// 1. 텐트 사용자인데 시크루트 프리패스가 끝났을 경우
				//    OUT_STAMINA_REGEN_TIME 값이 세팅되어 오며, 현재 회복률로 로그아웃 시간 전체의 스태미너 회복 외에
				//    시크루트 프리패스에 의해 추가 회복되었어야 할 시간이 더해짐으로써 적용됨
				// 2. 텐트 사용 유무와 관계 없이 시크루트 프리패스 사용 중인 경우
				//    OUT_STAMINA_REGEN_TIME은 0으로 반환되어 전체 로그아웃 시간 동안
				//    현재 스태미너 회복률(시크루트에 의해 2배 적용 됨)로 스태미너 회복
				m_pPlayer->AddStamina( ( cmd->Parameters->Item[ "OUT_STAMINA_REGEN_TIME" ]->Value.intVal + m_pPlayer->GetLogoutDuration() ) * regen_rate );
			}
			else
			{
				// 여기서는 로그아웃 상태의 스테미너는 회복시키면 안되지만 로그아웃 시점부터
				// 시크루트 프리패스가 종료되는 시점까지의 스테미너는 회복시켜야 함
				// 현재 스테미너 회복률은 시크루트 프리패스가 없는 상태이므로 당시의 회복률은 현재의 2배
				m_pPlayer->AddStamina( cmd->Parameters->Item[ "OUT_STAMINA_REGEN_TIME" ]->Value.intVal * regen_rate * GameRule::SECROUTE_STAMINA_REGEN_BONUS_RATE );
			}

			// 이벤트 지속효과 부여 처리
			m_pPlayer->ProcEventState();

			LOG::Log11N4S( LM_CHARACTER_ENTER, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), 0, 0, m_pPlayer->GetPCBangMode(), m_pPlayer->GetStamina(), m_pPlayer->GetGold().GetRawData(), m_pPlayer->GetLayer(), m_pPlayer->GetX(), m_pPlayer->GetY(), 0, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
			LOG::Log11N4S( LM_CHARACTER_INFO, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), m_pPlayer->GetLevel(), m_pPlayer->GetJobLevel(), m_pPlayer->GetJobPoint(), m_pPlayer->GetJobId(), m_pPlayer->GetGold().GetRawData(), m_pPlayer->GetStorageGold().GetRawData(), m_pPlayer->GetChaos(), m_pPlayer->GetImmoralPoint(), m_pPlayer->GetEXP(), m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		}

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		throw;
	}

	return true;
	
}

void DB_GetCommercialStorageInfo::onFail( const _com_error & exception )
{
	std::string strMessage( GameContent::GetString( 103 ) );
	strMessage += "(1)";
	SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, strMessage.c_str() );
	m_pPlayer->onEndQuery();
}

bool DB_ReadCommercialStorageList::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ReadCommercialItemStorageList : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_read_purchased_item_list" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_purchased_item_list";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );

		_variant_t vNull;
		vNull.vt=VT_ERROR;
		vNull.scode=DISP_E_PARAMNOTFOUND;

		_RecordsetPtr pRS = cmd->Execute(&vNull,&vNull,adCmdStoredProc);

		/*
			sid,                   -- ID
			account_id,            -- 구매한 계정 ID
			avatar_id,             -- 구매한 캐릭 ID        ( -1 일경우홈피에서구매한것임)
			avatar_name,           -- 구매한 캐릭 이름        ( -1 일경우홈피에서구매한것임)
			server_name,			  -- 구매한 서버 이름
			item_code,             -- 아이템코드
			item_count,            -- 구매한 아이템갯수
			rest_item_count,		  -- 남은 아이템 갯수
			bought_time,           -- 구매시간
			valid_time             -- 유효시간( 제한없을경우-12-31 )
		*/

		TS_SC_COMMERCIAL_STORAGE_LIST						msg;
		TS_SC_COMMERCIAL_STORAGE_LIST::CommercialItemInfo	info;
		msg.count = 0;
		std::vector< TS_SC_COMMERCIAL_STORAGE_LIST::CommercialItemInfo > vList;
		
		while( pRS->State != adStateClosed && !pRS->EndOfFile )
		{	
			info.commercial_item_uid	= pRS->Fields->Item["sid"]->Value.intVal;
			info.code					= pRS->Fields->Item["item_code"]->Value.intVal;
			info.count					= pRS->Fields->Item["rest_item_count"]->Value.intVal;

			vList.push_back( info );
			++msg.count;
			msg.size += sizeof(info);

			pRS->MoveNext();
		}

		char *pBuf = NULL;
		char szBuf[ SENDMSG_BUFFER_SIZE ] = "";
		size_t buff_size = sizeof( szBuf );
		if( sizeof(msg) + sizeof(info) * msg.count < SENDMSG_BUFFER_SIZE )
		{
			pBuf = szBuf;
		}
		else
		{
			buff_size = sizeof(msg) + sizeof(info) * msg.count;
			pBuf = new char[ buff_size ];
		}

		s_memcpy( pBuf, buff_size, &msg, sizeof(msg) );
		if( msg.count ) s_memcpy( pBuf + sizeof(msg), buff_size - sizeof(msg), &*vList.begin(), sizeof(info) * msg.count );
		PendStream( m_pPlayer, pBuf, msg.size );

		if( pBuf != szBuf )
			delete [] pBuf;

		LOG::Log11N4S( LM_READ_COMMERCIAL_STORAGE, m_pPlayer->GetAccountID(), m_pPlayer->GetPlayerUID(), msg.count, 0, 0, 0, 0, 0, 0, 0, 0, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

void DB_ReadCommercialStorageList::onFail( const _com_error & exception )
{
	std::string strMessage( GameContent::GetString( 103 ) );
	strMessage += "(2)";
	SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, strMessage.c_str() );
	m_pPlayer->onEndQuery();
}

bool DB_TakeOutCommercialItem::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_TakeOutCommercialItem : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_takeout_commercial_item" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_takeout_commercial_item";

		ItemUID item_uid = StructPlayer::allocItemUID();
		LOG::Log11N4S( LM_TAKEOUT_TEMP_ITEM, m_pPlayer->GetAccountID(), m_pPlayer->GetPlayerUID(), m_unItemCID, m_nCount, 0, 0, 0, 0, 0, 0, item_uid, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

		cmd->Parameters->Append( cmd->CreateParameter( "RETURN_VALUE", adInteger, adParamReturnValue, 4, 0 ) );

		cmd->Parameters->Append( cmd->CreateParameter( "OUT_ITEM_CODE", adInteger, adParamOutput, 4, 0 ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_CID", adInteger, adParamInput, 4, m_unItemCID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_COUNT", adInteger, adParamInput, 4, m_nCount ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_AVATAR_ID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_AVATAR_NAME", adVarChar, adParamInput, 61, m_pPlayer->GetName() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SERVER_NAME", adVarChar, adParamInput, 30, ENV().GetString( "app.name", "Unknown" ).c_str() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adBigInt, adParamInput, 8, item_uid ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TIME", adInteger, adParamInput, 4, 0 ) );

		_variant_t vNull;
		vNull.vt=VT_ERROR;
		vNull.scode=DISP_E_PARAMNOTFOUND;

		cmd->Execute(&vNull,&vNull,adCmdStoredProc);

		int nErrorCode = cmd->Parameters->Item[ "RETURN_VALUE" ]->Value;

		if(	cmd->Parameters->Item[ "OUT_ITEM_CODE" ]->Value.vt != VT_NULL )
		{
			int nItemCode = cmd->Parameters->Item[ "OUT_ITEM_CODE" ]->Value;

			if( nItemCode > 0 )
			{
				SendResult( m_pPlayer, TM_CS_TAKEOUT_COMMERCIAL_ITEM, ERROR_SUCCESS, nItemCode );

				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

				if( StructItem::GetItemBase( nItemCode ).Flag.IsOn( ItemBase::FLAG_JOIN ) )
				{
					StructItem *pNewItem = StructItem::AllocItem( item_uid, nItemCode, m_nCount, ItemInstance::BY_CASH );
					StructItem *pResultItem = NULL;
					pResultItem = m_pPlayer->PushItem( pNewItem, pNewItem->GetCount() );
					ItemUID uid = pResultItem->GetItemUID();

					if( pNewItem != pResultItem )
					{
						pNewItem->SetOwnerInfo( NULL, 0, 0 );
						pNewItem->DBQuery( new DB_UpdateItemOwner( pNewItem ) );
						StructItem::PendFreeItem( pNewItem );

						pResultItem->DBQuery( new DB_UpdateItem( pResultItem ) );
					}

					LOG::Log11N4S( LM_TAKEOUT_COMMERCIAL_ITEM,
						m_pPlayer->GetAccountID(), m_pPlayer->GetPlayerUID(),
						m_unItemCID,
						nItemCode,
						m_nCount,
						nErrorCode,
						0, 0, 0, 0,
						uid,
						m_pPlayer->GetAccountName(), LOG::STR_NTS,
						m_pPlayer->GetName(), LOG::STR_NTS,
						"", 0, "", 0 );
				}
				else
				{
					ItemUID last_uid = 0;
					for( __int64 i = 0; i < m_nCount; ++i )
					{
						if( last_uid == item_uid )
						{
							item_uid = StructPlayer::allocItemUID();
						}

						// JOINABLE 아이템이 아닌경우
						StructItem *pItem = NULL;
						StructItem *pNewItem = StructItem::AllocItem( item_uid, nItemCode, 1, ItemInstance::BY_CASH );
						pItem = m_pPlayer->PushItem( pNewItem, pNewItem->GetCount() );

						if( pItem )
						{
							last_uid = pItem->GetItemUID();
							pItem->DBQuery( new DB_UpdateItem( pItem ) );
						}

						LOG::Log11N4S( LM_TAKEOUT_COMMERCIAL_ITEM,
							m_pPlayer->GetAccountID(), m_pPlayer->GetPlayerUID(),
							m_unItemCID,
							nItemCode,
							1,
							nErrorCode,
							0, 0, 0, 0,
							last_uid,
							m_pPlayer->GetAccountName(), LOG::STR_NTS,
							m_pPlayer->GetName(), LOG::STR_NTS,
							"", 0, "", 0 );
					}
				}				
			}
			else
			{
				LOG::Log11N4S( LM_TAKEOUT_COMMERCIAL_ITEM, m_pPlayer->GetAccountID(), m_pPlayer->GetPlayerUID(), m_unItemCID, nItemCode, m_nCount, nErrorCode, 0, 0, 0, 0, 0, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
				SendResult( m_pPlayer, TM_CS_TAKEOUT_COMMERCIAL_ITEM, ERROR_ACCESS_DENIED );
			}
		}
		else
		{
			LOG::Log11N4S( LM_TAKEOUT_COMMERCIAL_ITEM, m_pPlayer->GetAccountID(), m_pPlayer->GetPlayerUID(), m_unItemCID, 0, m_nCount, nErrorCode, 0, 0, 0, 0, 0, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
			SendResult( m_pPlayer, TM_CS_TAKEOUT_COMMERCIAL_ITEM, ERROR_ACCESS_DENIED );
		}

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		LOG::Log11N4S( LM_TAKEOUT_COMMERCIAL_ITEM, m_pPlayer->GetAccountID(), m_pPlayer->GetPlayerUID(), m_unItemCID, -1, -1, 0, 0, 0, 0, 0, 0, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		throw;
	}

	return true;
}

void DB_TakeOutCommercialItem::onFail( const _com_error & exception )
{
	std::string strMessage( GameContent::GetString( 103 ) );
	strMessage += "(3)";
	SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, strMessage.c_str() );
	m_pPlayer->onEndQuery();
}
