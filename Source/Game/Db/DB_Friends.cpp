
#include <oledberr.h>
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"
#include "GameMessage.h"
#include "SendMessage.h"
#include "StructPlayer.h"


DB_InsertFriend::DB_InsertFriend( const char *szFriendID, bool bIsDenial, struct StructPlayer * pPlayer )
: DBProc( "DB_InsertFriend", true ),
  m_strOwnerID( pPlayer->GetName() ),
  m_strFriendID( szFriendID ),
  m_bIsDenial( bIsDenial ),
  m_pPlayer( pPlayer )
{
}

bool DB_InsertFriend::onProcess( DBConnection & db )
{
	try
	{
		char pszResult[64] = "";
		do
		{
			// 대상이 유효한 캐릭터 이름인지 검사해야 함
			{
				_CommandPtr cmd;
				if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertFriend : CreateInstance(command) error" );

				cmd->CommandType = adCmdStoredProc;
				cmd->CommandText = _bstr_t( "dbo.smp_read_guild_party_id" );
				// Store the name of current stored-procedure for debugging
				szStoredProcedureName = "dbo.smp_read_guild_party_id";

				cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, m_strFriendID.length(), m_strFriendID ) );
				cmd->Parameters->Append( cmd->CreateParameter( "OUT_SID", adInteger, adParamOutput, 4, 0 ) );
				cmd->Parameters->Append( cmd->CreateParameter( "OUT_GUILDID", adInteger, adParamOutput, 4, 0 ) );
				cmd->Parameters->Append( cmd->CreateParameter( "OUT_PARTYID", adInteger, adParamOutput, 4, 0 ) );

				cmd->Execute( NULL, NULL, adCmdStoredProc );

				int nSID = cmd->Parameters->Item[ "OUT_SID" ]->Value.intVal;

				// 대상 캐릭터가 존재하지 않을 때
				if( !nSID )
				{
					s_sprintf( pszResult, _countof( pszResult ), "@481" );	// 존재하지 않는 캐릭터입니다.
					break;
				}
			}

			{
				_CommandPtr cmd;
				if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertFriend : CreateInstance(command) error" );

				cmd->CommandType = adCmdStoredProc;
				const char * pszParamName = NULL;

				if( m_bIsDenial )
				{
					if( !m_pPlayer->addDenial( m_strFriendID ) )
					{
						s_sprintf( pszResult, _countof( pszResult ), "@473\v#@denial_name@#\v%s", (const char *) m_strFriendID );	// XXXX 님은 이미 차단되어 있습니다.
						break;
					}

					SendDenialsList( m_pPlayer );

					AR_HANDLE hTarget = StructPlayer::FindPlayer( m_strFriendID );
					StructPlayer::iterator itTarget = StructPlayer::get( hTarget );
					if( *itTarget )
					{
						(*itTarget)->AddDenialOf( m_pPlayer->GetName() );

						if( m_pPlayer->IsFriendOf( m_strFriendID ) )
						{
							SendStatusMessageToFriendOfPlayer( m_pPlayer, (*itTarget), false );
						}
					}

					cmd->CommandText = _bstr_t( "dbo.smp_insert_denial" );
					// Store the name of current stored-procedure for debugging
					szStoredProcedureName = "dbo.smp_insert_denial";
					pszParamName = "IN_DENIAL_ID";

					s_sprintf( pszResult, _countof( pszResult ), "@472\v#@denial_name@#\v%s", (const char *) m_strFriendID );		// XXXX 님이 차단되었습니다.
				}
				else
				{
					if( !m_pPlayer->addFriend( m_strFriendID ) )
					{
						s_sprintf( pszResult, _countof( pszResult ), "@469\v#@friend_name@#\v%s", (const char *) m_strFriendID );	// XXXX 님은 이미 친구로 등록되어 있습니다.
						break;
					}

					cmd->CommandText = _bstr_t( "dbo.smp_insert_friend" );
					// Store the name of current stored-procedure for debugging
					szStoredProcedureName = "dbo.smp_insert_friend";
					pszParamName = "IN_FRIEND_ID";

					SendFriendsList( m_pPlayer );

					AR_HANDLE hTarget = StructPlayer::FindPlayer( m_strFriendID );
					StructPlayer::iterator itTarget = StructPlayer::get( hTarget );
					if( *itTarget )
					{
						(*itTarget)->AddFriendOf( m_pPlayer->GetName() );
					}

					s_sprintf( pszResult, _countof( pszResult ), "@467\v#@friend_name@#\v%s", (const char *) m_strFriendID );		// XXXX 님이 친구로 추가되었습니다.
				}

				cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, m_strOwnerID.length(), m_strOwnerID ) );
				cmd->Parameters->Append( cmd->CreateParameter( pszParamName, adBSTR, adParamInput, m_strFriendID.length(), m_strFriendID ) );

				cmd->Execute(NULL, NULL,adCmdStoredProc);
			}
		} while( false );

		if( pszResult[0] )
			SendChatMessage( false, CHAT_FRIEND, "@FRIEND", m_pPlayer, pszResult );

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

void DB_InsertFriend::onFail( const _com_error & exception )
{
	std::string strMessage( GameContent::GetString( 103 ) );
	strMessage += "(4)";
	SendChatMessage( false, CHAT_FRIEND, "@FRIEND", m_pPlayer, strMessage.c_str() );

	// 메모리에 친구/차단 등록은 완료되고 DB 처리 실패했을 경우를 대비해서 메모리상의 친구/차단 목록에서 제거
	// DelDenial / DelFriend 함수에서 이미 삭제된 사람이면 처리 안 함
	if( m_bIsDenial )
	{
		m_pPlayer->DelDenial( m_strFriendID );
	}
	else
	{
		m_pPlayer->DelFriend( m_strFriendID );
	}

	AR_HANDLE hTarget = StructPlayer::FindPlayer( m_strFriendID );
	StructPlayer::iterator itTarget = StructPlayer::get( hTarget );
	if( *itTarget )
	{
		// DelDenialOf / DelFriendOf 함수에서 이미 삭제된 사람이면 처리 안 함
		if( m_bIsDenial )
		{
			(*itTarget)->DelDenialOf( m_pPlayer->GetName() );
		}
		else
		{
			(*itTarget)->DelFriendOf( m_pPlayer->GetName() );
		}
	}

	m_pPlayer->onEndQuery();
}

DB_DeleteFriend::DB_DeleteFriend( const char * szOwnerID, const char *szFriendID, bool bIsDenial, struct StructPlayer * pPlayer )
: DBProc( "DB_DeleteFriend", true ),
  m_strOwnerID( szOwnerID ),
  m_strFriendID( szFriendID ),
  m_bIsDenial( bIsDenial ),
  m_pPlayer( pPlayer )
{
}

bool DB_DeleteFriend::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertFriend : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;

		if( m_bIsDenial )
		{
			cmd->CommandText = _bstr_t( "dbo.smp_delete_denial" );
			// Store the name of current stored-procedure for debugging
			szStoredProcedureName = "dbo.smp_delete_denial";
		}
		else
		{
			cmd->CommandText = _bstr_t( "dbo.smp_delete_friend" );
			// Store the name of current stored-procedure for debugging
			szStoredProcedureName = "dbo.smp_delete_friend";
		}
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, m_strOwnerID.length(), m_strOwnerID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_FRIEND_ID", adBSTR, adParamInput, m_strFriendID.length(), m_strFriendID ) );

		cmd->Execute(NULL, NULL,adCmdStoredProc);

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		throw;
	}

	const char * pszResult = NULL;

	if( m_bIsDenial )
	{
		m_pPlayer->delDenial( m_strFriendID );

		SendDenialsList( m_pPlayer );

		AR_HANDLE hTarget = StructPlayer::FindPlayer( m_strFriendID );
		StructPlayer::iterator itTarget = StructPlayer::get( hTarget );

		if( *itTarget )
		{
			(*itTarget)->DelDenialOf( m_pPlayer->GetName() );

			if( m_pPlayer->IsFriendOf( m_strFriendID ) )
			{
				SendStatusMessageToFriendOfPlayer( m_pPlayer, (*itTarget), true );
			}
		}

		pszResult = "@474\v#@denial_name@#\v%s";		// XXXX 님이 차단 목록에서 삭제되었습니다.
	}
	else
	{
		m_pPlayer->delFriend( m_strFriendID );

		SendFriendsList( m_pPlayer );

		AR_HANDLE hTarget = StructPlayer::FindPlayer( m_strFriendID );
		StructPlayer::iterator itTarget = StructPlayer::get( hTarget );

		if( *itTarget )
		{
			(*itTarget)->DelFriendOf( m_pPlayer->GetName() );
		}

		pszResult = "@470\v#@friend_name@#\v%s";		// XXXX 님이 친구 목록에서 삭제되었습니다.
	}

	PrintfChatMessage( false, CHAT_FRIEND, "@FRIEND", m_pPlayer, pszResult, static_cast< const char * >( m_strFriendID ) );

	return true;
}

void DB_DeleteFriend::onFail( const _com_error & exception )
{
	std::string strMessage( GameContent::GetString( 103 ) );
	strMessage += "(5)";
	SendChatMessage( false, CHAT_FRIEND, "@FRIEND", m_pPlayer, strMessage.c_str() );

	// 메모리에 친구/차단 삭제는 완료되고 DB 처리 실패했을 경우를 대비해서 메모리상의 친구/차단 목록에 다시 추가
	// addDenial / addFriend 함수에서 이미 등록된 사람이면 처리 안 함
	if( m_bIsDenial )
	{
		m_pPlayer->addDenial( m_strFriendID );
	}
	else
	{
		m_pPlayer->addFriend( m_strFriendID );
	}

	AR_HANDLE hTarget = StructPlayer::FindPlayer( m_strFriendID );
	StructPlayer::iterator itTarget = StructPlayer::get( hTarget );
	if( *itTarget )
	{
		// AddDenialOf / AddFriendOf 함수에서 이미 등록된 사람이면 처리 안 함
		if( m_bIsDenial )
		{
			(*itTarget)->AddDenialOf( m_pPlayer->GetName() );
		}
		else
		{
			(*itTarget)->AddFriendOf( m_pPlayer->GetName() );
		}
	}

	m_pPlayer->onEndQuery();
}
