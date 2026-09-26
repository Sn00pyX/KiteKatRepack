
#include <oledberr.h>
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"


#include "PartyManager.h"
#include "DB_Commands.h"



bool DB_InsertParty::proc( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertParty : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_party" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_party";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, m_szPartyName.length(), m_szPartyName ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PARTY_SID", adInteger, adParamInput, 4, m_nPartySID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LEADER_SID", adInteger, adParamInput, 4, m_nLeaderSID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PARTY_TYPE", adInteger, adParamInput, 4, m_nPartyType ) );

		cmd->Execute(NULL, NULL,adCmdStoredProc);
	}
	catch( _com_error & e )
	{
		if( e.Error() == DB_E_INTEGRITYVIOLATION )
		{
			char szDebugInfo[512] = { 0, };
			s_sprintf( szDebugInfo, _countof( szDebugInfo ), "PartyName: %s", m_szPartyName );

			LogDBError( e, 0, szProcName, szStoredProcedureName, szDebugInfo );
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

bool DB_InsertParty::onProcess( DBConnection & db )
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

