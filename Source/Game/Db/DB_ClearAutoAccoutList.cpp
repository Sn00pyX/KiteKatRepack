
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"


bool DB_ClearAutoAccountList::proc( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_clear_auto_account_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_clear_auto_account_list";
	db.command->Parameters->Refresh();
	
	db.command->Execute(NULL, NULL,adCmdStoredProc);

	return true;
}

bool DB_ClearAutoAccountList::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

