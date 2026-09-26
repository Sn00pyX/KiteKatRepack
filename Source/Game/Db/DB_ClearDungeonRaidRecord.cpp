
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"


bool DB_ClearDungeonRaidRecord::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ClearDungeonRaidRecord : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_clear_dungeon_raid_record" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_clear_dungeon_raid_record";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DUNGEON_ID", adInteger, adParamInput, 4, nDungeonID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_ID", adInteger, adParamInput, 4, nGuildID ) );

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		throw;
	}

	return true;
}
