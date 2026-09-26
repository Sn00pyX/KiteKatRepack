
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "PartyManager.h"
#include "DB_Commands.h"


bool DB_DeleteParty::proc( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_delete_party" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_delete_party";
	db.command->Parameters->Refresh();
	
	db.command->Parameters->Item[ "@IN_SID" ]->Value = m_nPartyID;

	db.command->Execute(NULL, NULL,adCmdStoredProc);

	return true;
}

bool DB_DeleteParty::onProcess( DBConnection & db )
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

