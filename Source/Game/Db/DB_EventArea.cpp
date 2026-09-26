#include "DB_Commands.h"

#include "StructPlayer.h"


DB_InsertEventAreaEnterCount::DB_InsertEventAreaEnterCount( StructPlayer * pPlayer, const int nEventAreaID, const int nEnterCount )
	: DBProc( "DB_InsertEventAreaEnterCount" )
	, m_pPlayer( pPlayer )
	, m_nPlayerID( pPlayer->GetPlayerUID() )
	, m_nEventAreaID( nEventAreaID )
	, m_nEnterCount( nEnterCount )
{}

bool DB_InsertEventAreaEnterCount::onProcess( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertEventAreaEnterCount : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_event_area_enter_count" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_insert_event_area_enter_count";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAYER_SID", adInteger, adParamInput, 4, m_nPlayerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_EVENT_AREA_SID", adInteger, adParamInput, 4, m_nEventAreaID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ENTER_COUNT", adInteger, adParamInput, 4, m_nEnterCount ) );

	cmd->Execute(NULL, NULL,adCmdStoredProc);

	m_pPlayer->onEndQuery();

	return true;
}

void DB_InsertEventAreaEnterCount::onFail( const _com_error & exception )
{
	m_pPlayer->onEndQuery();
}


DB_UpdateEventAreaEnterCount::DB_UpdateEventAreaEnterCount( StructPlayer * pPlayer, const int nEventAreaID, const int nEnterCount )
	: DBProc( "DB_UpdateEventAreaEnterCount" )
	, m_pPlayer( pPlayer )
	, m_nPlayerID( pPlayer->GetPlayerUID() )
	, m_nEventAreaID( nEventAreaID )
	, m_nEnterCount( nEnterCount )
{}

bool DB_UpdateEventAreaEnterCount::onProcess( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateEventAreaEnterCount : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_event_area_enter_count" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_event_area_enter_count";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAYER_SID", adInteger, adParamInput, 4, m_nPlayerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_EVENT_AREA_SID", adInteger, adParamInput, 4, m_nEventAreaID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ENTER_COUNT", adInteger, adParamInput, 4, m_nEnterCount ) );

	cmd->Execute(NULL, NULL,adCmdStoredProc);

	m_pPlayer->onEndQuery();

	return true;
}

void DB_UpdateEventAreaEnterCount::onFail( const _com_error & exception )
{
	m_pPlayer->onEndQuery();
}
