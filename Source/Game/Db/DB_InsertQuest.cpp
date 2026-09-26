
#include <toolkit/XConsole.h>
#include <network/IConnection.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"

#include "StructPlayer.h"
#include "SendMessage.h"


bool DB_InsertQuest::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertQuest : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_quest" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_insert_quest";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nOwnerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ID", adInteger, adParamInput, 4, m_nID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, m_nCode ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_START_ID", adInteger, adParamInput, 4, m_nStartID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS1", adInteger, adParamInput, 4, m_nStatus1 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS2", adInteger, adParamInput, 4, m_nStatus2 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS3", adInteger, adParamInput, 4, m_nStatus3 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS4", adInteger, adParamInput, 4, m_nStatus4 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS5", adInteger, adParamInput, 4, m_nStatus5 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS6", adInteger, adParamInput, 4, m_nStatus6 ) );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TIME", adInteger, adParamInput, 4, m_nRemainTime ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PROGRESS", adInteger, adParamInput, 4, m_nProgress ) );

	cmd->Execute(NULL, NULL,adCmdStoredProc);

	return true;
}

bool DB_InsertQuest::onProcess( DBConnection & db )
{
	proc( db );

	m_pPlayer->onEndQuery();

	return true;

}

void DB_InsertQuest::onFail( const _com_error & exception )
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
