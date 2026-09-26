
#include <oledberr.h>
#include <oledb.h>
#include <atlcomtime.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"

#include "StructPlayer.h"


bool DB_InsertFarmInfo::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertFarmInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_farm_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_farm_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_FARM_UID", adInteger, adParamInput, 4, m_nFarmUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SLOT", adTinyInt, adParamInput, 1, m_nSlot ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_UID", adBigInt, adParamInput, 8, m_pCard->GetItemUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, m_pPlayer->GetSID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_MAX_LEVEL", adInteger, adParamInput, 4, m_nMaxLevel ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_IS_USING_CRACKER", adBoolean, adParamInput, 1, m_bUseCracker ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_IS_CASH", adBoolean, adParamInput, 1, m_bCash ) );
		COleDateTime dtRegistrationTime( m_tRegistrationTime );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_REGISTRATION_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtRegistrationTime ) ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_DURATION", adInteger, adParamInput, 4, m_nDuration ) );
		COleDateTime dtNursingTime( m_tNursingTime );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NURSING_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtNursingTime ) ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	m_pPlayer->onEndQuery();

	return true;
}

bool DB_UpdateNursingTime::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateNursingTime : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_nursing_time" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_nursing_time";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_FARM_UID", adInteger, adParamInput, 4, m_nFarmUID ) );
		COleDateTime dtNursingTime( m_tNursingTime );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NURSING_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtNursingTime ) ) );

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	m_pPlayer->onEndQuery();

	return true;
}

bool DB_DeleteFarmInfo::onProcess( DBConnection &db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteFarmInfo : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_delete_farm_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_delete_farm_info";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_FARM_UID", adInteger, adParamInput, 4, m_nFarmUID ) );
		
		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	m_pPlayer->onEndQuery();

	return true;
}