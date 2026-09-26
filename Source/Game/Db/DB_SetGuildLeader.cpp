
#include <oledberr.h>
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>
#include "ErrorCode/ErrorCode.h"


#include "GuildManager.h"
#include "SendMessage.h"

#include "DB_Commands.h"


bool DB_SetGuildLeader::proc( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildLeader : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_leader" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_leader";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nGuildId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LEADER_SID", adInteger, adParamInput, 4, m_nLeaderId ) );


		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( _com_error & e )
	{
		if( e.Error() == DB_E_INTEGRITYVIOLATION )
		{
			LogDBError( e, szProcName, szStoredProcedureName );
			return false;
		}
		else
			throw;
	}

	return true;
}

bool DB_SetGuildLeader::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

bool DB_SetGuildDungeonID::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildDungeonID : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_dungeon_id" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_dungeon_id";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DUNGEON_ID", adInteger, adParamInput, sizeof( m_nDungeonID ), m_nDungeonID ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

bool DB_SetGuildDungeonBlockTime::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildDungeonBlockTime : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_dungeon_block_time" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_dungeon_block_time";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nGuildId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DUNGEON_BLOCK_TIME", adBigInt, adParamInput, 5, m_tDungeonBlockTime ) );

		cmd->Execute(NULL, NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

bool DB_SetGuildGold::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildGold : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_gold" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_gold";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nGuildId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GOLD", adBigInt, adParamInput, 8, m_nGold.GetRawData() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_CHOAS", adInteger, adParamInput, 5, m_nChaos ) );

		cmd->Execute(NULL, NULL,adCmdStoredProc);

	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

bool DB_SetGuildAllianceID::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildAllianceID : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_alliance_id" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_alliance_id";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nGuildId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ALLIANCE_ID", adInteger, adParamInput, 5, m_nAllianceID ) );

		cmd->Execute(NULL, NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

bool DB_SetGuildAllianceBlockTime::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildAllianceBlockTime : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_alliance_block_time" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_alliance_block_time";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nGuildId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ALLIANCE_BLOCK_TIME", adBigInt, adParamInput, 8, m_tAllianceBlockTime ) );

		cmd->Execute(NULL, NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}


bool DB_InsertAlliance::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertAlliance : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_alliance" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_alliance";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nAllianceId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LEAD_GUILD_ID", adInteger, adParamInput, 4, m_nLeadGuildId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, m_strName.length(), m_strName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MAX_ALLIANCE_CNT", adInteger, adParamInput, 4, m_nMaxAllianceCnt ) );

		try
		{
			cmd->Execute(NULL, NULL,adCmdStoredProc);
		}
		catch( _com_error & e )
		{
			if( e.Error() == DB_E_INTEGRITYVIOLATION )
			{
				char szDebugInfo[512] = { 0, };
				s_sprintf( szDebugInfo, _countof( szDebugInfo ), "AllianceName: %s", m_strName );

				LogDBError( e, 0, szProcName, szStoredProcedureName, m_strName );
			}
			else
			{
				throw;
			}
		}
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

bool DB_UpdateAllianceInfo::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateAllianceInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_alliance_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_alliance_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nAllianceId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MAX_ALLIANCE_CNT", adInteger, adParamInput, 4, m_nMaxAllianceCnt ) );

		try
		{
			cmd->Execute(NULL, NULL,adCmdStoredProc);
		}
		catch( _com_error & e )
		{
			if( e.Error() == DB_E_INTEGRITYVIOLATION )
			{
				LogDBError( e, szProcName, szStoredProcedureName );
			}
			else
			{
				throw;
			}
		}
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

bool DB_ChangeAllianceName::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ChangeAllianceName : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_change_alliance_name" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_change_alliance_name";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nAllianceId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, m_szNewName.length(), m_szNewName ) );

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		throw;
	}

	GuildManager::GetInstance().onChangeAllianceName( m_nAllianceId, m_szNewName, m_pPlayer );
	GuildManager::GetInstance().onEndQuery();

	return true;
}

void DB_ChangeAllianceName::onFail( const _com_error & exception )
{
	SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, "@129" );
	GuildManager::GetInstance().onChangeAllianceNameFailed( m_nAllianceId );

	GuildManager::GetInstance().onEndQuery();
}

bool DB_DeleteAllianceInfo::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateAllianceInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_delete_alliance" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_delete_alliance";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nAllianceId ) );

		try
		{
			cmd->Execute(NULL, NULL,adCmdStoredProc);
		}
		catch( _com_error & e )
		{
			if( e.Error() == DB_E_INTEGRITYVIOLATION )
			{
				LogDBError( e, szProcName, szStoredProcedureName );
			}
			else
			{
				throw;
			}
		}
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}