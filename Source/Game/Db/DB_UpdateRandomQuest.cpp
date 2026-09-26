
#include <network/IConnection.h>
#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"

#include "StructPlayer.h"
#include "SendMessage.h"


bool DB_UpdateRandomQuest::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateRandomQuest : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_random_quest" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_random_quest";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nOwnerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, m_nCode ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_KEY1", adInteger, adParamInput, 4, m_nKey1 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_KEY2", adInteger, adParamInput, 4, m_nKey2 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_KEY3", adInteger, adParamInput, 4, m_nKey3 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_VALUE1", adInteger, adParamInput, 4, m_nValue1 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_VALUE2", adInteger, adParamInput, 4, m_nValue2 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_VALUE3", adInteger, adParamInput, 4, m_nValue3 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IS_DROPPED", adTinyInt, adParamInput, 1, m_cDropped ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_UpdateRandomQuest::onProcess( DBConnection & db )
{
	proc( db );

	m_pPlayer->onEndQuery();

	return true;

}

void DB_UpdateRandomQuest::onFail( const _com_error & exception )
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
