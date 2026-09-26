
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"

bool DB_UpdateDungeonRaidTime::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateDungeonRaidTime : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_dungeon_raid_time" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_dungeon_raid_time";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DUNGEON_ID", adInteger, adParamInput, 4, nDungeonID ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_ID", adInteger, adParamInput, 4, nGuildID ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RAID_TIME", adInteger, adParamInput, 4, nRaidTime ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RECORD", adInteger, adParamInput, 4, nRecord ));

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

