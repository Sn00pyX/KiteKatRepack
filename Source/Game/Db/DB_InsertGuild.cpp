
#include <oledberr.h>
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"



bool DB_InsertGuild::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertGuild : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_guild" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_guild";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, m_szGuildName.length(), m_szGuildName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( m_nGuildSID ), m_nGuildSID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LEADER_SID", adInteger, adParamInput, sizeof( m_nLeaderSID ), m_nLeaderSID ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ADVERTISE_TYPE", adTinyInt, adParamInput, sizeof( m_nAdvertiseType ), m_nAdvertiseType ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RECRUITING", adBoolean, adParamInput, sizeof( m_bRecruiting ), m_bRecruiting ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MIN_RECRUIT_LEVEL", adSmallInt, adParamInput, sizeof( m_nMinRecruitLevel ), m_nMinRecruitLevel ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MAX_RECRUIT_LEVEL", adSmallInt, adParamInput, sizeof( m_nMaxRecruitLevel ), m_nMaxRecruitLevel ) );

		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION_NAME_1", adBSTR, adParamInput, m_szPermissionName1.length(), m_szPermissionName1 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION_NAME_2", adBSTR, adParamInput, m_szPermissionName2.length(), m_szPermissionName2 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION_NAME_3", adBSTR, adParamInput, m_szPermissionName3.length(), m_szPermissionName3 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION_NAME_4", adBSTR, adParamInput, m_szPermissionName4.length(), m_szPermissionName4 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION_NAME_5", adBSTR, adParamInput, m_szPermissionName5.length(), m_szPermissionName5 ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION_NAME_6", adBSTR, adParamInput, m_szPermissionName6.length(), m_szPermissionName6 ) );

		cmd->Execute( NULL, NULL,adCmdStoredProc );

		GuildManager::GetInstance().onInsertGuildSuccessed( m_pInfo, m_hLeader );
	}
	catch( _com_error & e )
	{
		GuildManager::GetInstance().onInsertGuildFailed( m_pInfo );

		GuildManager::GetInstance().onEndQuery();
		if( e.Error() == DB_E_INTEGRITYVIOLATION )
		{
			char szDebugInfo[512] = { 0, };
			s_sprintf( szDebugInfo, _countof( szDebugInfo ), "GuildName: %s", m_szGuildName );
			LogDBError( e, 0, szProcName, szStoredProcedureName, szDebugInfo );
			return false;
		}
		else
			throw;
	}
	catch( ... )
	{	
		GuildManager::GetInstance().onEndQuery();
		throw;
	}

	GuildManager::GetInstance().onEndQuery();
	return true;
}

