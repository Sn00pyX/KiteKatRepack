

#include <oledb.h>
#include <icrsint.h>

#include <network/IConnection.h>
#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"
#include "GameMessage.h"
#include "SendMessage.h"
#include "StructPlayer.h"



bool DB_UpdateClientInfo::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateClientInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_client_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_client_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_CLIENT_INFO", adVarChar, adParamInput, _countof(szValue), szValue ));

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	m_pPlayer->onEndQuery();
	return true;
}

bool DB_UpdateQuickSlot::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateQuickSlot : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_quick_slot" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_quick_slot";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_QUICK_SLOT", adVarChar, adParamInput, _countof(szValue), szValue ));

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	m_pPlayer->onEndQuery();
	return true;
}

bool DB_UpdateCurrentKey::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateCurrentKeyInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_current_key" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_current_key";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_CURRENT_KEY", adVarChar, adParamInput, _countof(szValue), szValue ));

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	m_pPlayer->onEndQuery();
	return true;
}

bool DB_UpdateSavedKey::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateSavedKey : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_saved_key" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_saved_key";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SAVED_KEY", adVarChar, adParamInput, _countof(szValue), szValue ));

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	m_pPlayer->onEndQuery();
	return true;
}