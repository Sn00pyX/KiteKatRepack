
#include <oledberr.h>
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>
#include "ErrorCode/ErrorCode.h"

#include "PartyManager.h"
#include "DB_Commands.h"


bool DB_UpdatePartyInfo::proc( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdatePartyInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_party_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_party_info";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nPartyID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LEADER_SID", adInteger, adParamInput, 4, m_nLeaderId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SHARE_MODE", adInteger, adParamInput, 4, m_nShareMode ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PARTY_TYPE", adInteger, adParamInput, 4, m_nPartyType ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LEAD_PARTY_ID", adInteger, adParamInput, 4, m_nLeadPartyID ) );

		cmd->Execute(NULL, NULL,adCmdStoredProc);
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
	catch( ... )
	{
		throw;
	}

	return true;
}

bool DB_UpdatePartyInfo::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		PartyManager::GetInstance().onEndQuery();

		throw;
	}

	PartyManager::GetInstance().onEndQuery();

	return true;
}

