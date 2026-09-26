
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "GuildManager.h"
#include "DB_Commands.h"



bool DB_DeleteGuild::proc( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_delete_guild" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_delete_guild";
	db.command->Parameters->Refresh();
	
	db.command->Parameters->Item[ "@IN_SID" ]->Value = m_nGuildId;

	db.command->Execute(NULL, NULL,adCmdStoredProc);

	return true;
}

bool DB_DeleteGuild::onProcess( DBConnection & db )
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

