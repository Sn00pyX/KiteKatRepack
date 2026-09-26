
#include <oledb.h>
#include <icrsint.h>

#include <network/IConnection.h>
#include <network/XIOCPConnection.h>
#include <toolkit/XConsole.h>
#include <toolkit/XENV.h>
#include <mmo/ArcadiaServer.h>

#include "LogClient/LogClient.h"

#include "DB_Commands.h"

#include "StructPlayer.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "StructItem.h"
#include "StructSummon.h"
#include "GameProc.h"
#include "GameRule.h"


bool DB_ReadAccountAuthorityInfo::onProcess( DBConnection & db )
{
	try
	{
		if( GameRule::bIsCashUsableServer )
		{
			syncronizeWithBillingDBForSecroute( db );
		}

		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ReadAccountAuthorityInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_read_authority_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_authority_info";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );

		_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

		while( pRS->State != adStateClosed && !pRS->EndOfFile )
		{
			int nAuthorityType = pRS->Fields->Item[ "authority_type" ]->Value.intVal;
			int nDuration = pRS->Fields->Item[ "remain_time" ]->Value.intVal;

			m_pPlayer->SetAccountAuthority( nAuthorityType, nDuration );

			pRS->MoveNext();
		}

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

void DB_ReadAccountAuthorityInfo::onFail( const _com_error & exception )
{
	m_pPlayer->onEndQuery();
}

bool DB_ReadAccountAuthorityInfo::syncronizeWithBillingDBForSecroute( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ReadAccountAuthorityInfo : CreateInstance(command) error" );

	cmd->CommandTimeout = 60;
	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_secroute_info" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_secroute_info";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );

	_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

	return true;
}


bool DB_SetAccountAuthorityInfo::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetAccountAuthorityInfo : CreateInstance(command) error" );

		// 느린 쿼리 타임아웃 조정
		cmd->CommandTimeout = 60;
		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_authority_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_authority_info";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_AUTHORITY_TYPE", adInteger, adParamInput, 4, m_nAuthorityType ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TIME", adInteger, adParamInput, 4, m_nDuration ) );

		_variant_t vNull;
		vNull.vt=VT_ERROR;
		vNull.scode=DISP_E_PARAMNOTFOUND;

		cmd->Execute(&vNull,&vNull,adCmdStoredProc);

		// 원래 시크루트 쓰고 있었으면 연장.
		if( m_pPlayer->IsGaiaMember() )
		{
			m_pPlayer->SetAccountAuthority( m_nAuthorityType, m_nDuration + m_pPlayer->GetGaiaMemberRemainTime() / 100 - GetArTime() / 100 );
		}
		else
		{
			m_pPlayer->SetAccountAuthority( m_nAuthorityType, m_nDuration );
		}

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

void DB_SetAccountAuthorityInfo::onFail( const _com_error & exception )
{
	m_pPlayer->onEndQuery();
}


bool DB_CloseAccountAuthorityInfo::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_CloseAccountAuthorityInfo : CreateInstance(command) error" );

		// 느린 쿼리 타임아웃 조정
		cmd->CommandTimeout = 60;
		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_close_authority_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_close_authority_info";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_AUTHORITY_TYPE", adInteger, adParamInput, 4, m_nAuthorityType ) );

		_variant_t vNull;
		vNull.vt=VT_ERROR;
		vNull.scode=DISP_E_PARAMNOTFOUND;

		cmd->Execute(&vNull,&vNull,adCmdStoredProc);

		m_pPlayer->SetAccountAuthority( m_nAuthorityType, 0 );

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

void DB_CloseAccountAuthorityInfo::onFail( const _com_error & exception )
{
	m_pPlayer->onEndQuery();
}
