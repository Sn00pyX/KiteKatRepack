
#include <network/IConnection.h>

#include "DB_Commands.h"

#include "StructPlayer.h"


bool DB_InsertQuestCoolTime::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertQuestCoolTime : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_quest_cool_time" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_insert_quest_cool_time";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nOwnerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, m_nCode ) );
	COleDateTime dtCoolTime( m_tCoolTime );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_COOL_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtCoolTime ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PROGRESS", adInteger, adParamInput, 4, m_nProgress ) );

	cmd->Execute(NULL, NULL,adCmdStoredProc);

	return true;
}

bool DB_InsertQuestCoolTime::onProcess( DBConnection & db )
{
	proc( db );

	m_pPlayer->onEndQuery();

	return true;
}

void DB_InsertQuestCoolTime::onFail( const _com_error & exception )
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