
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>
#include "ErrorCode/ErrorCode.h"


#include "DB_Commands.h"


bool DB_UpdateDungeon::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateDungeon : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_dungeon" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_dungeon";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DUNGEON_ID", adInteger, adParamInput, 4, nDungeonID ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_GUILD_ID", adInteger, adParamInput, 4, nOwnGuildID ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_RAID_GUILD_ID", adInteger, adParamInput, 4, nRaidGuildID ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_BEST_RAID_TIME", adInteger, adParamInput, 4, nBestRaidTime ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DUNGEON_SIEGE_FINISH_TIME", adInteger, adParamInput, 4, nDungeonSiegeFinishTime ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DUNGEON_RAID_WRAP_UP_TIME", adInteger, adParamInput, 4, nDungeonRaidWrapUpTime ));
		cmd->Parameters->Append( cmd->CreateParameter( "IN_TAX_RATE", adInteger, adParamInput, 4, nTaxRate ));

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

