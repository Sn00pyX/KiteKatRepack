#include "DB_Commands.h"
#include "StructPlayer.h"

#include <atlcomtime.h>


// 로그인 상태의 플레이어 페널티 정보 업데이트용
DB_UpdateBattleArenaPenalty::DB_UpdateBattleArenaPenalty( struct StructPlayer * pPlayer )
	: DBProc( "DB_UpdateBattleArenaPenalty", true )
	, m_pPlayer( pPlayer )
	, m_nPlayerUID( pPlayer->GetPlayerUID() )
	, m_tBlock( pPlayer->GetBattleArenaBlockTime() )
	, m_nPenaltyCount( pPlayer->GetBattleArenaPenaltyCount() )
	, m_tPenaltyDecrease( pPlayer->GetBattleArenaPenaltyDecTime() )
{}

// 로그아웃 상태의 플레이어 페널티 정보 업데이트용
DB_UpdateBattleArenaPenalty::DB_UpdateBattleArenaPenalty( PlayerUID nPlayerUID, time_t tBlock, int nPenaltyCount, time_t tPenaltyDecrease )
	: DBProc( "DB_UpdateBattleArenaPenalty", true )
	, m_pPlayer( NULL )
	, m_nPlayerUID( nPlayerUID )
	, m_tBlock( tBlock )
	, m_nPenaltyCount( nPenaltyCount )
	, m_tPenaltyDecrease( tPenaltyDecrease )
{}

bool DB_UpdateBattleArenaPenalty::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateBattleArenaPenalty : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_battle_arena_penalty" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_battle_arena_penalty";

	COleDateTime dtBlockTime( m_tBlock );
	COleDateTime dtPenaltyDecTime( m_tPenaltyDecrease );
	s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_OWNER_ID: %d, @IN_BLOCK_TIME: %s, @IN_PENALTY_COUNT: %d, @IN_LAST_PENALTY_INC_TIME: %s",
		m_nPlayerUID, static_cast< const char * >( dtBlockTime.Format( "%Y-%m-%d %H:%M:%S" ) ),
		m_nPenaltyCount, static_cast< const char * >( dtPenaltyDecTime.Format( "%Y-%m-%d %H:%M:%S" ) ) );
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAYER_SID", adInteger, adParamInput, 4, m_nPlayerUID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_BLOCK_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtBlockTime ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PENALTY_COUNT", adInteger, adParamInput, 4, m_nPenaltyCount ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PENALTY_DEC_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtPenaltyDecTime ) ) );
	
	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}

bool DB_UpdateBattleArenaPenalty::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		if( m_pPlayer )
			m_pPlayer->onEndQuery();

		throw;
	}

	if( m_pPlayer )
		m_pPlayer->onEndQuery();

	return true;
}
