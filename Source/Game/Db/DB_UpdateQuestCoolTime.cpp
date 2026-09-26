
#include <network/IConnection.h>

#include "DB_Commands.h"

#include "StructPlayer.h"


bool DB_UpdateQuestCoolTime::update_quest_cool_time( DBConnection & db, int nOwnerID, int nCode, time_t tCoolTime, int nProgress )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateQuestCoolTime : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_quest_cool_time" );
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, nOwnerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, nCode ) );
	COleDateTime dtCoolTime( tCoolTime );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_COOL_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtCoolTime ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PROGRESS", adInteger, adParamInput, 4, nProgress ) );
	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_UpdateQuestCoolTime::onProcess( DBConnection & db )
{
	update_quest_cool_time( db, m_nOwnerID, m_nCode, m_tCoolTime, m_nProgress );

	m_pPlayer->onEndQuery();

	return true;
}

void DB_UpdateQuestCoolTime::onFail( const _com_error & exception )
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