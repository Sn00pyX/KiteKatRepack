
#include <oledb.h>
#include <icrsint.h>

#include <network/IConnection.h>
#include <mmo/ArcadiaServer.h>

#include "DB_Commands.h"
#include "StructPlayer.h"
#include "GameProc.h"


bool DB_UpdateStorageGold::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateStorageGold : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_storage_gold" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_storage_gold";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_GOLD", adBigInt, adParamInput, 8, m_pPlayer->GetStorageGold().GetRawData() ) );

		_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();
		throw;
	}


	return true;
	
}