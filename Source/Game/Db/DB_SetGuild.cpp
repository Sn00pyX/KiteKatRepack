
#include <oledb.h>
#include <icrsint.h>
#include <atlcomtime.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"


#include "GuildManager.h"
#include "DB_Commands.h"
#include "SendMessage.h"


bool DB_SetGuild::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuild : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_set_guild" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_set_guild";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAYER_SID", adInteger, adParamInput, 4, m_nPlayerId ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, 4, m_nGuildId ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PREV_GUILD_SID", adInteger, adParamInput, 4, m_nPrevGuildId ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_SetGuild::onProcess( DBConnection & db )
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

bool DB_ChangeGuildName::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ChangeGuildName : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_change_guild_name" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_change_guild_name";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nGuildId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, m_szNewName.length(), m_szNewName ) );

		cmd->Execute(NULL,NULL,adCmdStoredProc);

		GuildManager::GetInstance().onChangeGuildName( m_nGuildId, m_szNewName, m_pPlayer );
	}
	catch( ... )
	{
		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

void DB_ChangeGuildName::onFail( const _com_error & exception )
{
	if( m_pPlayer )
	{
		GuildManager::GetInstance().onChangeGuildNameFailed( m_nGuildId );
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, "@129" );
	}

	GuildManager::GetInstance().onEndQuery();
}

bool DB_SetGuildBlockTime::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildBlockTime : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_set_guild_block_time" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_set_guild_block_time";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAYER_SID", adInteger, adParamInput, 4, m_nPlayerId ) );
	COleDateTime dtGuildBlockTime( m_tGuildBlockTime );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_BLOCK_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtGuildBlockTime ) ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_SetGuildBlockTime::onProcess( DBConnection & db )
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

bool DB_UpdateGuildDonationPoint::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateGuildDonationPoint : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_guild_donation_point" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_guild_donation_point";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_POINT", adInteger, adParamInput, sizeof( m_nPoint ), m_nPoint ) );

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

bool DB_SetGuildNotice::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildNotice : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_notice" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_notice";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NOTICE", adBSTR, adParamInput, m_szNotice.length(), m_szNotice ) );

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

bool DB_SetGuildURL::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildURL : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_url" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_url";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_URL", adBSTR, adParamInput, m_szURL.length(), m_szURL ) );

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

bool DB_SetGuildPermission::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildPermission : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_permission" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_permission";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAYER_SID", adInteger, adParamInput, sizeof( m_nPlayerUID ), m_nPlayerUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_PERMISSION", adTinyInt, adParamInput, sizeof( m_nPermission ), m_nPermission ) );

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

bool DB_SetGuildPermissionName::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildPermissionName : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_permission_name" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_permission_name";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION", adTinyInt, adParamInput, sizeof( m_nPermission ), m_nPermission ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION_NAME", adBSTR, adParamInput, m_szName.length(), m_szName ) );

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

bool DB_SetGuildPermissionSet::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildPermissionSet : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_permission_set" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_permission_set";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION", adTinyInt, adParamInput, sizeof( m_nPermission ), m_nPermission ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PERMISSION_SET", adInteger, adParamInput, sizeof( m_nPermissionSet ), m_nPermissionSet ) );

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

bool DB_SetGuildMemberMemo::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildMemberMemo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_member_memo" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_guild_member_memo";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAYER_SID", adInteger, adParamInput, sizeof( m_nPlayerUID ), m_nPlayerUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MEMO", adBSTR, adParamInput, m_szMemo.length(), m_szMemo ) );

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

bool DB_SetGuildAdvertiseTypeAndTime::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildAdvertiseTypeAndTime : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_guild_advertise_type_and_time" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_guild_advertise_type_and_time";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ADVERTISE_TYPE", adTinyInt, adParamInput, sizeof( m_nAdvertiseType ), m_nAdvertiseType ) );
		COleDateTime dtAdvertiseEnd( m_tAdvertiseEnd );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ADVERTISE_END_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtAdvertiseEnd ) ) );

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

bool DB_SetGuildAdvertiseComment::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildAdvertiseComment : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_guild_advertise_comment" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_guild_advertise_comment";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ADVERTISE_COMMENT", adBSTR, adParamInput, m_szComment.length(), m_szComment ) );

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

bool DB_SetGuildRecruiting::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildRecruiting : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_guild_recruiting" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_guild_recruiting";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_RECRUITING", adBoolean, adParamInput, sizeof( m_bRecruiting ), m_bRecruiting ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MIN_RECRUIT_LEVEL", adSmallInt, adParamInput, sizeof( m_nMinRecruitLevel ), m_nMinRecruitLevel ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MAX_RECRUIT_LEVEL", adSmallInt, adParamInput, sizeof( m_nMaxRecruitLevel ), m_nMaxRecruitLevel ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

void DB_SetGuildRecruiting::onFail( const _com_error & exception )
{
	std::string strMessage( GameContent::GetString( 103 ) );
	strMessage += "(7)";
	SendGuildChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", m_nGuildID, strMessage.c_str() );

	GuildManager::GetInstance().SetGuildRecruiting( m_nGuildID, !m_bRecruiting, 1, GameRule::nMaxLevel, true );
	GuildManager::GetInstance().onEndQuery();
}

bool DB_SetGuildIcon::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildIcon : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_guild_icon");

		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ICON", adVarChar, adParamInput, 255, m_szIconFileName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ICON_SIZE", adInteger, adParamInput, sizeof( m_nIconSize ), m_nIconSize ) );

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

bool DB_SetGuildBanner::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetGuildBanner : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_guild_banner");

		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( m_nGuildID ), m_nGuildID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_BANNER", adVarChar, adParamInput, 255, m_szBannerFileName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_BANNER_SIZE", adInteger, adParamInput, sizeof( m_nBannerSize ), m_nBannerSize ) );

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