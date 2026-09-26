
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>
#include "ErrorCode/ErrorCode.h"

#include "PartyManager.h"
#include "DB_Commands.h"


bool DB_SetParty::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetParty : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_set_party" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_set_party";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, m_szCharacterName.length(), m_szCharacterName ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nPartyID ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_SetParty::onProcess( DBConnection & db )
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

