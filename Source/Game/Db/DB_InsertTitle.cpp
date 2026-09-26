
#include <network/IConnection.h>
#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"

#include "StructPlayer.h"
#include "SendMessage.h"


bool DB_InsertTitle::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertTitle : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_title" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_insert_title";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nSID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nOwnerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, m_nCode ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS", adInteger, adParamInput, 4, m_nStatus ) );

	cmd->Execute(NULL, NULL,adCmdStoredProc);

	return true;
}

bool DB_InsertTitle::onProcess( DBConnection & db )
{
	proc( db );

	m_pPlayer->onEndQuery();

	return true;

}

void DB_InsertTitle::onFail( const _com_error & exception )
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
		m_pPlayer->LogoutNowWithAccount( 7 );
	}
}