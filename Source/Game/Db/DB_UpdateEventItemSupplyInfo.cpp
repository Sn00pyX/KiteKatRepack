
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "StructEventItemManager.h"
#include "DB_Commands.h"



bool DB_UpdateEventItemSupplyInfo::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateEventItemSupplyInfo : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_event_item_supply_info" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_event_item_supply_info";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nSID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LEFT_COUNT", adInteger, adParamInput, 4, m_nLeftCount ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_UpdateEventItemSupplyInfo::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		StructEventItemManager::GetInstance().onEndQuery();

		throw;
	}

	StructEventItemManager::GetInstance().onEndQuery();

	return true;
}

