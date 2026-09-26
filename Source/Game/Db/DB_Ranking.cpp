
#include <atlcomtime.h>

#include <toolkit/XConsole.h>

#include "DB_Commands.h"
#include "GameDBUtil.h"
#include "RankingManager.h"


bool DB_SetRankingScore::onProcess( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetRankingScore : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_set_ranking_score" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_set_ranking_score";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_RANKING_ID", adInteger, adParamInput, 4, m_nRankingType ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nPlayerUID ) );
	_decimal_variant dvScore;
	dvScore.setMultipleInteger( m_fScore.get(), 10000 );
	_ParameterPtr paramPtr = cmd->CreateParameter( "IN_SCORE", adDecimal, adParamInput, sizeof( DECIMAL ), dvScore );
	paramPtr->NumericScale = 4;
	paramPtr->Precision = 18;
	cmd->Parameters->Append( paramPtr );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

void DB_SetRankingScore::onEnd( bool bIsCancel )
{
	RankingManager::Instance().onEndQuery();

	delete this;
}

bool DB_InvalidateRankingScore::onProcess( DBConnection &db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InvalidateRankingScore : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_invalidate_ranking_score" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_invalidate_ranking_score";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_RANKING_ID", adInteger, adParamInput, 4, m_nRankingType ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nPlayerUID ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

void DB_InvalidateRankingScore::onEnd( bool bIsCancel )
{
	RankingManager::Instance().onEndQuery();

	delete this;
}

bool DB_SettleRanking::onProcess( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetRankingScore : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_settle_ranking" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_settle_ranking";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_RANKING_ID", adInteger, adParamInput, 4, m_nRankingType ) );
	COleDateTime dtNextSettling( m_tNextSettling );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_NEXT_SETTLING_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtNextSettling ) ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

void DB_SettleRanking::onEnd( bool bIsCancel )
{
	RankingManager::Instance().onEndQuery();

	delete this;
}
