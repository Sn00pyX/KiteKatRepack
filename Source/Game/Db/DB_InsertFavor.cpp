
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "StructPlayer.h"
#include "DB_Commands.h"


bool DB_InsertFavor::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertFavor : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_favor" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_insert_favor";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_FAVOR_ID", adInteger, adParamInput, 4, m_nFavorId ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_FAVOR", adInteger, adParamInput, 4, m_nFavor ) );

	cmd->Execute(NULL, NULL,adCmdStoredProc);

	return true;
}

bool DB_InsertFavor::onProcess( DBConnection & db )
{
	try
	{
		proc( db );

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	return true;
}

