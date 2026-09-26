
#include <toolkit/XConsole.h>

#include "DB_Commands.h"
#include "StructSummon.h"
#include "StructPlayer.h"


DB_UpdateSummon::DB_UpdateSummon( struct StructSummon * pSummon ) : DBProc( "DB_UpdateSummon" ), m_pSummon( pSummon )
{
	SummonInfo.IN_SID = m_pSummon->GetSummonSID();
	SummonInfo.IN_ACCOUNT_ID = m_pSummon->GetAccountId();
	SummonInfo.IN_OWNDER_ID = m_pSummon->GetMaster() ? m_pSummon->GetMaster()->GetPlayerUID() : 0 ;
	SummonInfo.IN_CODE = m_pSummon->GetSummonCode();
	SummonInfo.IN_NAME = m_pSummon->GetName();
	SummonInfo.IN_LV = m_pSummon->GetLevel();
	SummonInfo.IN_JLV = m_pSummon->GetJobLevel();
	SummonInfo.IN_MAX_LEVEL = m_pSummon->GetMaxReachedLevel();
	SummonInfo.IN_EXP = m_pSummon->GetEXP();
	SummonInfo.IN_JP = m_pSummon->GetJobPoint();
	SummonInfo.IN_LAST_DECREASED_EXP = m_pSummon->GetLastDecreasedEXP();
	SummonInfo.IN_HP = m_pSummon->GetHP();
	SummonInfo.IN_MP = m_pSummon->GetMP();
	SummonInfo.IN_SP = m_pSummon->GetSP();
	SummonInfo.IN_FP = 0;
	SummonInfo.IN_PREV_LEVEL_01 = m_pSummon->GetPrevJobLevel( 0 );
	SummonInfo.IN_PREV_LEVEL_02 = m_pSummon->GetPrevJobLevel( 1 );
	SummonInfo.IN_PREV_ID_01 = m_pSummon->GetPrevJobId( 0 );
	SummonInfo.IN_PREV_ID_02 = m_pSummon->GetPrevJobId( 1 );
	SummonInfo.IN_TRANSFORM = m_pSummon->GetTransformLevel();
}

bool DB_UpdateSummon::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateSummon : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_summon" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_summon";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, SummonInfo.IN_SID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, SummonInfo.IN_ACCOUNT_ID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNDER_ID", adInteger, adParamInput, 4, SummonInfo.IN_OWNDER_ID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, SummonInfo.IN_CODE ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, SummonInfo.IN_NAME.length(), SummonInfo.IN_NAME ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LV", adInteger, adParamInput, 4, SummonInfo.IN_LV ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JLV", adInteger, adParamInput, 4, SummonInfo.IN_JLV ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_MAX_LEVEL", adInteger, adParamInput, 4, SummonInfo.IN_MAX_LEVEL ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_EXP", adBigInt, adParamInput, 8, SummonInfo.IN_EXP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JP", adInteger, adParamInput, 4, SummonInfo.IN_JP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LAST_DECREASED_EXP", adBigInt, adParamInput, 8, SummonInfo.IN_LAST_DECREASED_EXP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_HP", adInteger, adParamInput, 4, SummonInfo.IN_HP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_MP", adInteger, adParamInput, 4, SummonInfo.IN_MP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SP", adInteger, adParamInput, 4, SummonInfo.IN_SP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_FP", adInteger, adParamInput, 4, SummonInfo.IN_FP ) );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_PREV_LEVEL_01", adInteger, adParamInput, 4, SummonInfo.IN_PREV_LEVEL_01 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PREV_LEVEL_02", adInteger, adParamInput, 4, SummonInfo.IN_PREV_LEVEL_02 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PREV_ID_01", adInteger, adParamInput, 4, SummonInfo.IN_PREV_ID_01 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PREV_ID_02", adInteger, adParamInput, 4, SummonInfo.IN_PREV_ID_02 ) );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_TRANSFORM", adInteger, adParamInput, 4, SummonInfo.IN_TRANSFORM ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_UpdateSummon::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		m_pSummon->onEndQuery();
		throw;
	}
	m_pSummon->onEndQuery();

	return true;
}
