
#include <atlcomtime.h>

#include <network/IConnection.h>
#include <mmo/ArcadiaServer.h>

#include "DB_Commands.h"
#include "GameProc.h"
#include "ScheduledCommandManager.h"

bool DB_UpdateLaunchedScheduledCommand::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateLaunchedScheduledCommand : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_launched_scheduled_command" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_launched_scheduled_command";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, sizeof( m_nSID ), m_nSID ) );
		COleDateTime dtLastLaunchedTime( m_tLastLaunched );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_LAST_LAUNCHED_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtLastLaunchedTime ) ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_FINISHED", adBoolean, adParamInput, sizeof( m_bFinished ), m_bFinished ) );

		_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

		ScheduledCommandManager::Instance().onEndQuery();
	}
	catch( ... )
	{
		ScheduledCommandManager::Instance().onEndQuery();
		throw;
	}


	return true;
	
}