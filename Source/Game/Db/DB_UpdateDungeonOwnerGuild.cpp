
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"


bool DB_UpdateDungeonOwnerGuild::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateDungeonOwnerGuild : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_dungeon_owner_guild" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_dungeon_owner_guild";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DUNGEON_ID", adInteger, adParamInput, 4, nDungeonID ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_GUILD_ID", adInteger, adParamInput, 4, nOwnerGuildID ));

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

