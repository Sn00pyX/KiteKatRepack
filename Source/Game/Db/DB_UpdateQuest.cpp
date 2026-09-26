
#include <network/IConnection.h>
#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"

#include "StructPlayer.h"
#include "SendMessage.h"


bool DB_UpdateQuest::update_quest( DBConnection & db, int nOwnerID, int nID, int nStatus1, int nStatus2, int nStatus3, int nStatus4, int nStatus5, int nStatus6, int nRemainTime, int nProgress )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateQuest : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_quest" );
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, nOwnerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ID", adInteger, adParamInput, 4, nID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS1", adInteger, adParamInput, 4, nStatus1 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS2", adInteger, adParamInput, 4, nStatus2 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS3", adInteger, adParamInput, 4, nStatus3 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS4", adInteger, adParamInput, 4, nStatus4 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS5", adInteger, adParamInput, 4, nStatus5 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATUS6", adInteger, adParamInput, 4, nStatus6 ) );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TIME", adInteger, adParamInput, 4, nRemainTime ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PROGRESS", adInteger, adParamInput, 4, nProgress ) );
	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_UpdateQuest::onProcess( DBConnection & db )
{
	update_quest( db, m_nOwnerID, m_nID, m_nStatus1, m_nStatus2, m_nStatus3, m_nStatus4, m_nStatus5, m_nStatus6, m_nRemainTime, m_nProgress );

	m_pPlayer->onEndQuery();

	return true;
}

void DB_UpdateQuest::onFail( const _com_error & exception )
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
