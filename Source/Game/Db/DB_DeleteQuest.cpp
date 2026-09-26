
#include <toolkit/XConsole.h>
#include <network/IConnection.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"

#include "StructPlayer.h"
#include "SendMessage.h"

bool DB_DeleteQuest::onProcess( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteQuest : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_delete_quest" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_delete_quest";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nOwnerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ID", adInteger, adParamInput, 4, m_nID ) );

	cmd->Execute(NULL, NULL,adCmdStoredProc);

	m_pPlayer->onEndQuery();

	return true;
}

void DB_DeleteQuest::onFail( const _com_error & exception )
{
	m_pPlayer->onEndQuery();

	IStreamSocketConnection * pConn = m_pPlayer->pConnection;
	if( pConn && pConn->IsConnected() )
	{
		SendDisconnectDesc( pConn, TS_SC_DISCONNECT_DESC::DISCONNECT_TYPE_DB_ERROR );
		pConn->Close();
	}
	else
	{
		m_pPlayer->LogoutNowWithAccount( 9 );
	}
}
