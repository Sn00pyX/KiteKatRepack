
#include <oledb.h>
#include <oledberr.h>
#include <icrsint.h>

#include <network/IConnection.h>
#include <network/XIOCPConnection.h>
#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"
#include "GameMessage.h"
#include "SendMessage.h"
#include "StructItem.h"
#include "GuildManager.h"
#include "PartyManager.h"
#include "AuctionManager.h"
#include "BattleArenaManager.h"


/*
		@OUT_SID		INT OUTPUT,
		@IN_NAME		VARCHAR(31),
		@IN_ACCOUNT		VARCHAR(31),
		@IN_SLOT		INT,
		@IN_X			INT,
		@IN_Y			INT,
		@IN_Z			INT,
		@IN_LAYER		INT,
		@IN_RACE		INT,
		@IN_SEX			INT,
		@IN_LV			INT,
		@IN_HP			INT,
		@IN_MP			INT,
		@IN_JLV			INT,
		@IN_JP			BIGINT,
		@IN_ALI			INT,
		@IN_CHA			INT,
		@IN_SKIN_COLOR	INT,
		@IN_MODEL_00	INT,
		@IN_MODEL_01	INT,
		@IN_MODEL_02	INT,
		@IN_MODEL_03	INT,
		@IN_MODEL_04	INT
*/

bool DB_CreateCharacter::insertBasicItem( DBConnection & db )
{
	/*
	int nBasicItemCode = 10109;

	if( nOwnerID )
	{
		return DB_InsertItem::insertItemToDB(	db, 
												StructPlayer::allocItemUID(),
												nOwnerID,
												0,
												nBasicItemCode,
												0, 
												1, 
												0,
												0, 
												StructItem::GetItemBase( nBasicItemCode ).nEndurance, 
												ItemInstance::BY_BASIC );
	}*/ 

	return false;
}

bool DB_CreateCharacter::setDefaultClientInfo( DBConnection & db, int nOwnerID )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_CreateCharacter : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_client_info" );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, nOwnerID ) );
	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}

bool DB_CreateCharacter::insertCharacter( DBConnection & db )
{	
	// { Env에서 지정된 개수 까지만 만들 수 있게 제한(최대 6개)
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_CreateCharacter : CreateInstance(command) error" );
		
		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_check_character_count" );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_SID", adInteger, adParamInput, sizeof( nAccountID ), nAccountID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_COUNT", adInteger, adParamOutput, 4, 0 ) );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_check_character_count";
		
		_RecordsetPtr rs = cmd->Execute(NULL, NULL,adCmdStoredProc);

		if( cmd->Parameters->Item[ "OUT_COUNT" ]->Value.vt != VT_NULL )
		{
			if( cmd->Parameters->Item[ "OUT_COUNT" ]->Value.lVal >= std::min( GameRule::nMaxCharactersPerAccount, 8 ) ) 
			{
				SendResult( pConnection, TM_CS_CREATE_CHARACTER, RESULT_LIMIT_MAX );
				return false;
			}
		}
	}
	// }


	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_CreateCharacter : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_character" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_character";

		int nDefaultArmorCode = 220100;		// 데바리안 슈츠
		if (wear_item == 602)
			nDefaultArmorCode = 220109;		// 데바리안 슈츠
		else if (wear_item == 603)
			nDefaultArmorCode = 220101;
		else if (wear_item == 604)
			nDefaultArmorCode = 220102;
		int nDefaultWeaponCode = 106100;		// 비기너스 메이스

		if( nRace == JobInfo::GAIA )
		{
			nDefaultArmorCode = 240100;
			if(wear_item == 602 )
				nDefaultArmorCode = 240109;
			else if (wear_item == 603)
				nDefaultArmorCode = 240101;
			else if (wear_item == 604)
				nDefaultArmorCode = 240102;
			nDefaultWeaponCode = 112100;	// 수련자용 밀림도: 101110, 수련자의 스몰엑스: 112100
		}
		else if( nRace == JobInfo::ASURA )
		{
			nDefaultArmorCode = 230100;		// 데바리안 슈츠
			if( wear_item == 602 )
				nDefaultArmorCode = 230109;		// 데바리안 슈츠
			else if (wear_item == 603)
				nDefaultArmorCode = 230101;
			else if (wear_item == 604)
				nDefaultArmorCode = 230102;
			nDefaultWeaponCode = 103100;		// 비기너스 메이스
		}

		cmd->Parameters->Append( cmd->CreateParameter( "RETURN_VALUE", adInteger, adParamReturnValue, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_SID", adInteger, adParamOutput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT", adVarChar, adParamInput, _countof(szAccountName), szAccountName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, nAccountID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SLOT", adInteger, adParamInput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_X", adInteger, adParamInput, 4, 19500) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_Y", adInteger, adParamInput, 4, 51000 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_Z", adInteger, adParamInput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LAYER", adInteger, adParamInput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RACE", adInteger, adParamInput, 4, nRace ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SEX", adInteger, adParamInput, 4, nSex ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LV", adInteger, adParamInput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MAX_REACHED_LEVEL", adInteger, adParamInput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HP", adInteger, adParamInput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MP", adInteger, adParamInput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_JLV", adInteger, adParamInput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_JP", adBigInt, adParamInput, 8, 0L ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_CHA", adInteger, adParamInput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SKIN_COLOR", adInteger, adParamInput, 4, nSkinColor ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MODEL_00", adInteger, adParamInput, 4, nModelId[0] ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MODEL_01", adInteger, adParamInput, 4, nModelId[1] ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MODEL_02", adInteger, adParamInput, 4, nModelId[2] ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MODEL_03", adInteger, adParamInput, 4, nModelId[3] ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MODEL_04", adInteger, adParamInput, 4, nModelId[4] ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HAIR_COLOR_INDEX", adInteger, adParamInput, 4, nHairColorIndex ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HAIR_COLOR_RGB", adInteger, adParamInput, 4, nHairColorRGB ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_TEXTURE_ID", adInteger, adParamInput, 4, nTextureId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DEFAULT_WEAPON_SID", adBigInt, adParamInput, 8, StructPlayer::allocItemUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DEFAULT_WEAPON_CODE", adInteger, adParamInput, 4, nDefaultWeaponCode ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DEFAULT_ARMOR_SID", adBigInt, adParamInput, 8, StructPlayer::allocItemUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DEFAULT_ARMOR_CODE", adInteger, adParamInput, 4, nDefaultArmorCode ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DEFAULT_BAG_SID", adBigInt, adParamInput, 8, StructPlayer::allocItemUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DEFAULT_BAG_CODE", adInteger, adParamInput, 4, ItemBase::ITEM_CODE_BEGINNERS_BAG ) );

		try
		{
			cmd->Execute(NULL,NULL,adCmdStoredProc);
		}
		catch( _com_error & e )
		{
			if( e.Error() == DB_E_INTEGRITYVIOLATION )
			{
				char szDebugInfo[512] = { 0, };
				s_sprintf( szDebugInfo, _countof( szDebugInfo ), "CharacterName: %s", szCharacterName );

				LogDBError( e, 0, szProcName, szStoredProcedureName, szDebugInfo );
				SendResult( pConnection, TM_CS_CREATE_CHARACTER, RESULT_ALREADY_EXIST );
				return false;
			}
			throw;
		}
		catch( ... )
		{
			throw;
		}

		if( cmd->Parameters->Item[ "RETURN_VALUE" ]->Value.intVal < 0 ) 
		{
			SendResult( pConnection, TM_CS_CREATE_CHARACTER, RESULT_ALREADY_EXIST );
			return false;
		}

		SendResult( pConnection, TM_CS_CREATE_CHARACTER, RESULT_SUCCESS );

		nOwnerID = cmd->Parameters->Item["OUT_SID"]->Value;
		LOG::Log11N4S( LM_CHARACTER_CRAETE, nAccountID, nOwnerID, 0, 0, 0, 0, 0, 0, 0, 0, 0, szAccountName, LOG::STR_NTS, szCharacterName, LOG::STR_NTS, "", 0, "", 0 );
	}

	return true;
}

bool DB_CreateCharacter::onProcess( DBConnection & db )
{
	if( insertCharacter( db ) )
	{
		insertBasicItem( db );
		setDefaultClientInfo( db, nOwnerID );
	}

	static_cast< XIOCPConnection * >( pConnection )->DecVar();

	return true;
}

void DB_CreateCharacter::onFail( const _com_error & exception )
{
	SendResult( pConnection, TM_CS_CREATE_CHARACTER, RESULT_DB_ERROR );
	static_cast< XIOCPConnection * >( pConnection )->DecVar();
}

bool DB_CheckCharacterName::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_CheckCharacterName : CreateInstance(command) error" );
		
		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_check_character_name" );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_COUNT", adInteger, adParamOutput, 4, 0 ) );

		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_check_character_name";

		_RecordsetPtr rs = cmd->Execute( NULL, NULL,adCmdStoredProc );

		if( cmd->Parameters->Item[ "OUT_COUNT" ]->Value.vt != VT_NULL && cmd->Parameters->Item[ "OUT_COUNT" ]->Value.lVal == 0 )
		{
			SendResult( pConnection, TM_CS_CHECK_CHARACTER_NAME, RESULT_SUCCESS );
		}
		else
		{
			SendResult( pConnection, TM_CS_CHECK_CHARACTER_NAME, RESULT_ALREADY_EXIST );
		}
	}
	catch( ... )
	{
		throw;
	}

	static_cast< XIOCPConnection* >( pConnection )->DecVar();
	return true;
}

void DB_CheckCharacterName::onFail( const _com_error & exception )
{
	SendResult( pConnection, TM_CS_CHECK_CHARACTER_NAME, RESULT_DB_ERROR );
	static_cast< XIOCPConnection* >( pConnection )->DecVar();
}

bool DB_ChangeCharacterName::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ChangeCharacterName : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_character_name" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_character_name";

		cmd->Parameters->Append( cmd->CreateParameter( "RETURN_VALUE", adInteger, adParamReturnValue, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetSID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, m_szCharacterName.length(), m_szCharacterName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_REMOVE_FROM_FRIEND_DENIAL", adBoolean, adParamInput, 1, m_bRemoveFromFriendAndDenial ) );

		// 아이템에 의한 이름 변경인 경우 실행 전에 검사해서 아이템이 없으면 시도조차 안하고 캔슬, 있으면 끝나자마자 성공했으면 아이템 삭제해야 함
		if( m_hNameChangeItem )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

			StructItem * pItem = StructItem::FindItem( m_hNameChangeItem );
			if( !pItem || !pItem->IsInInventory() || pItem->GetOwnerUID() != m_pPlayer->GetPlayerUID() )
			{
				SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, "@2058" );
				throw std::exception( "Canceled" );
			}
		}

		cmd->Execute( NULL, NULL, adCmdStoredProc );

		if( cmd->Parameters->Item[ "RETURN_VALUE" ]->Value.intVal < 0 )
		{
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, "@18" );

			m_pPlayer->OnChangeName( m_szCharacterName, m_bRemoveFromFriendAndDenial, m_bIgnoreNameChangeCount, false );
			throw std::exception( "Canceled" );
		}

		// 이름 변경이 성공했고 아이템에 의한 것이었으면 아이템 삭제 후 지역락 해제
		// 프로시저 실행 전에 검사한 것에 대해 다시 검사. 만일 아이템이 없다면 원래 이름으로 롤백 처리
		// 아이템 보유 상태를 보장하며 쿼리 실행 후 삭제 처리를 바로 하려면 지역락을 건 상태로
		// 쿼리를 실행해야 하나 그러면 DB의 응답 시간이 서버의 락 지속 시간으로 연결되어 렉이 발생함
		if( m_hNameChangeItem )
		{
			ArcadiaLock __lock = ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer );

			StructItem * pItem = StructItem::FindItem( m_hNameChangeItem );
			if( !pItem || !pItem->IsInInventory() || pItem->GetOwnerUID() != m_pPlayer->GetPlayerUID() )
			{
				SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, "@2058" );

				ArcadiaServer::Instance().UnLock( &__lock );

				cmd->Parameters->GetItem( "IN_NAME" )->PutSize( static_cast< ADO_LONGPTR >( strlen( m_pPlayer->GetName() ) ) );
				cmd->Parameters->GetItem( "IN_NAME" )->PutValue( m_pPlayer->GetName() );

				cmd->Execute( NULL, NULL, adCmdStoredProc );

				throw std::exception( "Canceled" );
			}

			m_pPlayer->EraseItem( pItem, 1 );

			ArcadiaServer::Instance().UnLock( &__lock );
		}

		// 파티, 길드 처리
		int nPartyID = m_pPlayer->GetPartyID();
		int nGuildID = m_pPlayer->GetGuildID();

		if( nPartyID )
		{
			PartyManager::GetInstance().OnChangeCharacterName( nPartyID, m_pPlayer->GetPlayerUID(), m_szCharacterName );
		}

		if( nGuildID )
		{
			GuildManager::GetInstance().OnChangeCharacterName( nGuildID, m_pPlayer->GetPlayerUID(), m_szCharacterName );
		}

		// 경매 처리
		AuctionManager::Instance().OnChangeCharacterName( m_pPlayer->GetPlayerUID(), m_szCharacterName );

		LOG::Log11N4S( LM_CHARACTER_CHANGE_NAME, m_pPlayer->GetAccountID(), m_pPlayer->GetPlayerUID(), 0, 0, 0, 0, 0, 0, 0, 0, 0, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, m_szCharacterName, LOG::STR_NTS, "", 0 );

		// 친구, 차단, 전체 플레이어 리스트에서 이름 변경 처리
		m_pPlayer->OnChangeName( m_szCharacterName, m_bRemoveFromFriendAndDenial, m_bIgnoreNameChangeCount, true );

		TS_SC_CHANGE_NAME msg;
		msg.handle = m_pPlayer->GetHandle();
		s_strcpy( msg.name, _countof( msg.name ), m_pPlayer->GetName() );

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

		ArcadiaServer::Instance().Broadcast( m_pPlayer->GetRX(), m_pPlayer->GetRY(), m_pPlayer->GetLayer(), &msg );	

		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, "@131" );
	}
	// _com_error 예외는 발생하면 그냥 날려서 onFail에서 처리되도록 함
	catch( std::exception & e )
	{
		if( strncmp( e.what(), "Canceled", strlen( "Canceled" ) ) )
		{
			m_pPlayer->onEndQuery();
			throw;
		}
	}

	m_pPlayer->onEndQuery();

	return true;
}

void DB_ChangeCharacterName::onFail( const _com_error & exception )
{
	if( exception.Error() == DB_E_INTEGRITYVIOLATION )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, "@18" );
	}
	else
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, "@129" );
	}

	m_pPlayer->OnChangeName( m_szCharacterName, m_bRemoveFromFriendAndDenial, m_bIgnoreNameChangeCount, false );

	m_pPlayer->onEndQuery();
}

bool DB_ChangeCharacterAlias::onProcess( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ChangeCharacterAlias : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_character_alias" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_character_alias";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetSID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ALIAS", adBSTR, adParamInput, m_szAlias.length(), m_szAlias ) );

	cmd->Execute( NULL, NULL, adCmdStoredProc );

	// 성공했으니 별칭 바꾸고 클라에게 성공했다고 알려 줌
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( m_pPlayer ) );
		if( m_bByUserRequest )
		{
			SendResult( m_pPlayer, TM_CS_CHANGE_ALIAS, RESULT_SUCCESS );
			m_pPlayer->SetAlias( m_szAlias );
		}
	}

	m_pPlayer->onEndQuery();
	return true;
}

void DB_ChangeCharacterAlias::onFail( const _com_error & exception )
{
	// DB 업데이트가 실패하면 유저가 온라인인 상태에서는 바뀐 별칭을 사용하지만,
	// 재접하게 되면 별칭이 롤백돼서 기존에 참여되어 있던 파티/경기에 재참여가
	// 정상적으로 되지 않는 문제가 발생할 수 있음.
	// 따라서 문제가 생기면 현재 메모리상의 캐릭터의 유지시켜야 함
	// 그리고 유저가 대기열이나 연습 경기 외의 경기에 참여 중이던 경우에는
	// 아직 별칭이 바뀌지 않았던 상태에서 참여한 것이므로 그냥 놔두면 됨
	if( m_bByUserRequest )
		SendResult( m_pPlayer, TM_CS_CHANGE_ALIAS, RESULT_DB_ERROR );
	else
	{
		// 유저가 요청해서 변경하는 게 아니라면 실패했을 때 복구할 방법이 없음 -_ -;;
		assert( 0 );
		XSEH::InvokeUnhandledException( (LONG)0xC000000DL );
	}

	m_pPlayer->onEndQuery();
}

bool DB_UpdateRace::onProcess( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateRace : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_character_race" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_character_race";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetSID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_RACE", adInteger, adParamInput, 4, m_pPlayer->GetRace() ) );

	cmd->Execute( NULL, NULL, adCmdStoredProc );

	m_pPlayer->onEndQuery();
	return true;
}

void DB_UpdateRace::onFail( const _com_error & exception )
{
	m_pPlayer->onEndQuery();
}
