
#include <oledb.h>
#include <icrsint.h>

#include <network/IConnection.h>
#include <network/XIOCPConnection.h>
#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "DB_Commands.h"
#include "GameMessage.h"
#include "SendMessage.h"
#include "GuildManager.h"
#include "PartyManager.h"
#include "RankingManager.h"
#include "BattleArenaManager.h"

extern XCriticalSection g_ConnectionTagLock;


bool DB_DeleteCharacter::proc( DBConnection & db )
{
	{
		// 길드, 파티 확인 및 처리
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteCharacter : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_read_guild_party_id" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_guild_party_id";
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_SID", adInteger, adParamOutput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_GUILDID", adInteger, adParamOutput, 4, 0 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_PARTYID", adInteger, adParamOutput, 4, 0 ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );

		int nSID = cmd->Parameters->Item[ "OUT_SID" ]->Value.intVal;
		int nGuildID = cmd->Parameters->Item[ "OUT_GUILDID" ]->Value.intVal;
		int nPartyID = cmd->Parameters->Item[ "OUT_PARTYID" ]->Value.intVal;

		if( !nSID )
		{
			SendResult( pConnection, TM_CS_DELETE_CHARACTER, RESULT_NOT_EXIST );
			return false;
		}

		// 길드가 있을 경우
		if( nGuildID > 0 )
		{
			// 길드장은 케릭터 삭제 금지
			if( GuildManager::GetInstance().IsLeader( nGuildID, nSID ) )
			{
				SendResult( pConnection, TM_CS_DELETE_CHARACTER, RESULT_ACCESS_DENIED );
				return false;
			}

			GuildManager::GetInstance().LeaveGuild( nGuildID, szCharacterName );
			PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "LEAVE|%s|", (const char *) szCharacterName );
		}

		// 파티가 있을 경우
		if( nPartyID )
		{
			// 배틀 아레나 파티라면...
			if( PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
			{
				BattleArenaManager::Instance().QuitGame( nPartyID, nSID, false, ALT_USER_REQUEST );
			}
			else
			{
				// 파티 리더면 파티 해산
				if( PartyManager::GetInstance().IsLeader( nPartyID, nSID ) )
				{
					BroadcastPartyDestroy( nPartyID );

					LOG::Log11N4S( LM_PARTY_DESTROY, nAccountID, 0, nPartyID, 0, 0, 0, 3, 0, 0, 0, 0,
						szAccountName, LOG::STR_NTS, szCharacterName, LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

					PartyManager::GetInstance().DestroyParty( nPartyID );
				}
				else
				{
					BroadcastPartyLeave( nPartyID, szCharacterName );

					LOG::Log11N4S( LM_PARTY_LEAVE, nAccountID, 0, nPartyID, 0, 0, 0, 2, 0, 0, 0, 0,
						szAccountName, LOG::STR_NTS, szCharacterName, LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

					PartyManager::GetInstance().LeaveParty( nPartyID, nSID );
				}
			}
		}
	}

	{
		// 친구 처리
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteCharacter : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_read_friends_list" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_friends_list";
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );

		_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

		while( pRS->State != adStateClosed && !pRS->EndOfFile )
		{
			AR_HANDLE handle = StructPlayer::FindPlayer( (const char * ) _bstr_t( pRS->Fields->Item["friend_id"]->Value.bstrVal ) );
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pFriend = *pit;

			// 온라인인 친구는 직접 삭제해주고, 나머지는 DB 명령어에서 친구 목록 일괄 제거
			if( pFriend )
			{
				pFriend->DelFriendOf( szCharacterName );
			}

			pRS->MoveNext();
		}
	}

	{
		// 친구 처리
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteCharacter : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_read_friendofs_list" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_friendofs_list";
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );

		_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

		while( pRS->State != adStateClosed && !pRS->EndOfFile )
		{
			AR_HANDLE handle = StructPlayer::FindPlayer( (const char * ) _bstr_t( pRS->Fields->Item["owner_id"]->Value.bstrVal ) );
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pFriend = *pit;

			// 온라인인 친구는 직접 삭제해주고, 나머지는 DB 명령어에서 친구 목록 일괄 제거
			if( pFriend )
			{
				pFriend->DelFriend( szCharacterName );
				SendFriendsList( pFriend );
			}

			pRS->MoveNext();
		}
	}

	{
		// 차단 처리
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteCharacter : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_read_denials_list" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_denials_list";
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );

		_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

		while( pRS->State != adStateClosed && !pRS->EndOfFile )
		{
			AR_HANDLE handle = StructPlayer::FindPlayer( (const char * ) _bstr_t( pRS->Fields->Item["denial_id"]->Value.bstrVal ) );
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pDenier = *pit;

			// 온라인인 차단자는 직접 삭제해주고, 나머지는 DB 명령어에서 차단 목록 일괄 제거
			if( pDenier )
			{
				pDenier->DelDenialOf( szCharacterName );
			}

			pRS->MoveNext();
		}
	}

	{
		// 차단 처리
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteCharacter : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_read_denialofs_list" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_denialofs_list";
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );

		_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

		while( pRS->State != adStateClosed && !pRS->EndOfFile )
		{
			AR_HANDLE handle = StructPlayer::FindPlayer( (const char * ) _bstr_t( pRS->Fields->Item["owner_id"]->Value.bstrVal ) );
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pDenier = *pit;

			// 온라인인 차단자는 직접 삭제해주고, 나머지는 DB 명령어에서 차단 목록 일괄 제거
			if( pDenier )
			{
				pDenier->DelDenial( szCharacterName );
				SendDenialsList( pDenier );
			}

			pRS->MoveNext();
		}
	}

	{
		// 케릭터 삭제
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteCharacter : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_delete_character" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_delete_character";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "OUT_SID", adInteger, adParamOutput, 4, 0 ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );

		SendResult( pConnection, TM_CS_DELETE_CHARACTER, RESULT_SUCCESS );

		int nSID = cmd->Parameters->Item[ "OUT_SID" ]->Value.intVal;

		if( !nSID )
			return false;

		// 랭킹 삭제 처리
		RankingManager::Instance().DeleteRankingScore( nSID );

		LOG::Log11N4S( LM_CHARACTER_DELETE, nAccountID, nSID, 0, 0, 0, 0, 0, 0, 0, 0, 0, szAccountName, LOG::STR_NTS, szCharacterName, LOG::STR_NTS, "", 0, "", 0 );
	}

	return true;
}

bool DB_DeleteCharacter::onProcess( DBConnection & db )
{
	// 캐릭터 삭제되면 로그인 가능 캐릭터 목록에서 해당 캐릭터 삭제
	if( proc( db ) )
	{
		THREAD_SYNCHRONIZE( g_ConnectionTagLock );

		_CONNECTION_TAG* pTag = static_cast< _CONNECTION_TAG* >( pConnection->GetTag() );

		for( std::vector< std::string >::iterator it = pTag->vCharacterNameList.begin() ; it != pTag->vCharacterNameList.end() ; ++it )
		{
			if( !_stricmp( (*it).c_str(), szCharacterName ) )
			{
				pTag->vCharacterNameList.erase( it );
				break;
			}
		}
	}

	static_cast< XIOCPConnection * >( pConnection )->DecVar();

	return true;
}

void DB_DeleteCharacter::onFail( const _com_error & exception )
{
	SendResult( pConnection, TM_CS_DELETE_CHARACTER, RESULT_DB_ERROR );
	static_cast< XIOCPConnection * >( pConnection )->DecVar();
}
