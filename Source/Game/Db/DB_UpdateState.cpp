#include <atlcomtime.h>
#include <ctime>

#include <toolkit/XConsole.h>


#include "DB_Commands.h"
#include "StructMisc.h"
#include "StructPlayer.h"
#include "StructSummon.h"


void DB_UpdateState::SetCommonStateDBParameters( StructState * pState )
{
	m_nSID = pState->GetUID();

	if( m_pCreature->IsPlayer() )
	{
		m_pPlayer = static_cast< StructPlayer * >( m_pCreature );
		m_nOwnerID = m_pPlayer->GetSID();
	}
	else
	{
		m_pSummon = static_cast< StructSummon * >( m_pCreature );
		m_nSummonID = m_pSummon->GetSummonSID();
	}

	m_nCode = static_cast< int >( pState->GetCode() );
	m_nLevel = pState->GetLevel();

	bool bIsHolded = pState->IsHolded();
	if( bIsHolded )
		pState->ReleaseRemainDuration();

	const AR_TIME & endTime = pState->GetEndTime();

	m_nDuration = ( endTime == (AR_TIME)-1 ) ? -1 : pState->GetFullDuration();
	if( m_nDuration < -1 )
	{
		assert( 0 );
		m_nDuration = ( endTime > t ) ? endTime - t : 0;
	}

	m_nRemainTime = ( endTime == (AR_TIME)-1 ) ? -1 : endTime - t;
	if( endTime != (AR_TIME)-1 && m_nRemainTime <= 0 )
	{
		m_nRemainTime = 0;
	}

	m_nBaseDamage = pState->GetBaseDamage();
	m_nRemainFireTime = std::max( GameContent::GetStateInfo( m_nCode )->fire_interval * 100 - (int)( t - pState->GetLastProcessedTime() ), 0 );

	if( bIsHolded )
		pState->HoldRemainDuration();

	m_nStateValue = pState->GetStateValue();
	m_strStateStringValue = pState->GetStateStringValue();
	m_nEnable = pState->GetEnable();
}

bool DB_UpdateState::InsertState( DBConnection & db )
{
	// { declare cmd
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateState : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_state" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_insert_state";
	s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_SID: %d", m_nSID );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nSID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nOwnerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_ID", adInteger, adParamInput, 4, m_nSummonID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, m_nCode ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LEVEL", adInteger, adParamInput, 4, m_nLevel ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_DURATION", adInteger, adParamInput, 4, m_nDuration ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TIME", adInteger, adParamInput, 4, m_nRemainTime ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_BASE_DAMAGE", adInteger, adParamInput, 4, m_nBaseDamage ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_FIRE_TIME", adInteger, adParamInput, 4, m_nRemainFireTime ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATE_VALUE", adInteger, adParamInput, 4, m_nStateValue ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATE_STRING_VALUE", adVarWChar, adParamInput, 32, m_strStateStringValue.c_str() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ENABLE", adInteger, adParamInput, 4, m_nEnable ) );
	// }

	cmd->Execute( NULL, NULL, adCmdStoredProc );

	return true;
}

bool DB_UpdateState::UpdateState( DBConnection & db )
{
	// { declare cmd
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateState : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_state" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_state";
	s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_SID: %d", m_nSID );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nSID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nOwnerID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_ID", adInteger, adParamInput, 4, m_nSummonID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, m_nCode ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LEVEL", adInteger, adParamInput, 4, m_nLevel ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_DURATION", adInteger, adParamInput, 4, m_nDuration ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TIME", adInteger, adParamInput, 4, m_nRemainTime ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_BASE_DAMAGE", adInteger, adParamInput, 4, m_nBaseDamage ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_FIRE_TIME", adInteger, adParamInput, 4, m_nRemainFireTime ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATE_VALUE", adInteger, adParamInput, 4, m_nStateValue ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STATE_STRING_VALUE", adVarWChar, adParamInput, 32, m_strStateStringValue.c_str() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ENABLE", adInteger, adParamInput, 4, m_nEnable ) );
	// }

	cmd->Execute( NULL, NULL, adCmdStoredProc );

	if( !m_nEnable )
		StructCreature::DeallocStateUID( m_nSID );

	return true;
}

bool DB_UpdateState::proc( DBConnection & db )
{
	if( m_bInsert )
		InsertState( db );
	else
		UpdateState( db );

	return true;
}

bool DB_UpdateState::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		m_pPlayer ? m_pPlayer->onEndQuery() : m_pSummon->onEndQuery();
		throw;
	}
	m_pPlayer ? m_pPlayer->onEndQuery() : m_pSummon->onEndQuery();

	return true;
}
