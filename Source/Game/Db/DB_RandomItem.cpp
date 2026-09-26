#include "DB_Commands.h"
#include "StructItem.h"
#include "StructPlayer.h"
#include "RandomManager.h"


#include <atlcomtime.h>


bool DB_SetRandomOption::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_SetRandomOption : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_set_random_option" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_set_random_option";

	if( ( m_RandomOption.nRandomType == ItemInstance::AWAKEN ) && m_pItem->GetAwakenSID() != m_RandomOption.nSID )
	{
		_cprint( "DB_SetRandomOption: Different Awaken SID is given. initial SID[%d], current SID[%d]\n", m_RandomOption.nSID, m_pItem->GetAwakenSID() );
		FILELOG( "DB_SetRandomOption: Different Awaken SID is given. initial SID[%d], current SID[%d]", m_RandomOption.nSID, m_pItem->GetAwakenSID() );
	}
	else if( ( m_RandomOption.nRandomType == ItemInstance::IDENTIFIED ) && m_pItem->GetIdentifiedSID() != m_RandomOption.nSID )
	{
		_cprint( "DB_SetRandomOption: Different Identified SID is given. initial SID[%d], current SID[%d]\n", m_RandomOption.nSID, m_pItem->GetIdentifiedSID() );
		FILELOG( "DB_SetRandomOption: Different Identified SID is given. initial SID[%d], current SID[%d]", m_RandomOption.nSID, m_pItem->GetIdentifiedSID() );
	}

	std::string strBuffer;
	_decimal_variant decValue1[ ItemInstance::MAX_RANDOM_OPTION_NUMBER ];
	_decimal_variant decValue2[ ItemInstance::MAX_RANDOM_OPTION_NUMBER ];
	_ParameterPtr parValue1[ ItemInstance::MAX_RANDOM_OPTION_NUMBER ];
	_ParameterPtr parValue2[ ItemInstance::MAX_RANDOM_OPTION_NUMBER ];
	for( int i = 0 ; i < ItemInstance::MAX_RANDOM_OPTION_NUMBER; ++i )
	{
		decValue1[ i ].setMultipleInteger( m_RandomOption.OptionInfo[ i ].fValue1.get(), 10000 );
		decValue2[ i ].setMultipleInteger( m_RandomOption.OptionInfo[ i ].fValue2.get(), 10000 );
	}

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID",		adInteger, adParamInput, 4, m_RandomOption.nSID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID",	adInteger, adParamInput, 4, m_pItem->GetOwnerUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_RANDOM_TYPE",adInteger, adParamInput, 4, m_RandomOption.nRandomType ) );

	for( int i = 0; i < ItemInstance::MAX_RANDOM_OPTION_NUMBER; ++i )
	{
		XStringUtil::Format( strBuffer, "IN_TYPE_%02d", i + 1 );
		cmd->Parameters->Append( cmd->CreateParameter( strBuffer.c_str(), adInteger, adParamInput, 4, m_RandomOption.OptionInfo[ i ].nType ) );

		XStringUtil::Format( strBuffer, "IN_VALUE1_%02d", i + 1 );
		parValue1[ i ] = cmd->CreateParameter( strBuffer.c_str(), adDecimal, adParamInput, sizeof( DECIMAL ), decValue1[ i ] );
		parValue1[ i ]->NumericScale = 1;
		parValue1[ i ]->Precision = 10;
		cmd->Parameters->Append( parValue1[ i ] );

		XStringUtil::Format( strBuffer, "IN_VALUE2_%02d", i + 1 );
		parValue2[ i ] = cmd->CreateParameter( strBuffer.c_str(), adDecimal, adParamInput, sizeof( DECIMAL ), decValue2[ i ] );
		parValue2[ i ]->NumericScale = 1;
		parValue2[ i ]->Precision = 10;
		cmd->Parameters->Append( parValue2[ i ] );
	}

	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}


bool DB_SetRandomOption::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		m_pItem->onEndQuery();
		throw;
	}
	
	m_pItem->onEndQuery();
	return true;
}




bool DB_DeleteRandomOption::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteRandomOption : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_delete_random_option" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_delete_random_option";

	cmd->Parameters->Append( cmd->CreateParameter( "SID",		adInteger, adParamInput, 4, sid ) );

	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}



bool DB_DeleteRandomOption::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		m_pItem->onEndQuery();
		throw;
	}

	m_pItem->onEndQuery();
	return true;
}




bool DB_UpdateItemAwakenSID::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateItemAwakenSID : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_item_awaken_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_item_awaken_sid";

	cmd->Parameters->Append( cmd->CreateParameter( "SID",			adBigInt, adParamInput, 8, m_pItem->GetItemUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "AWAKEN_SID",	adInteger, adParamInput, 4, m_pItem->GetAwakenSID() ) );

	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}



bool DB_UpdateItemAwakenSID::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		m_pItem->onEndQuery();
		throw;
	}

	m_pItem->onEndQuery();
	return true;
}



bool DB_UpdateItemRandomOptionSID::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateItemRandomOptionSID : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_item_random_option_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_item_random_option_sid";

	cmd->Parameters->Append( cmd->CreateParameter( "SID",			adBigInt, adParamInput, 8, m_pItem->GetItemUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "RANDOM_OPTION_SID",	adInteger, adParamInput, 4, m_pItem->GetIdentifiedSID() ) );

	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}



bool DB_UpdateItemRandomOptionSID::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		m_pItem->onEndQuery();
		throw;
	}

	m_pItem->onEndQuery();
	return true;
}



bool DB_UpdateRandomOptionOwnerInfo::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateRandomOptionOwnerInfo : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_random_option_owner_info" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_random_option_owner_info";

	cmd->Parameters->Append( cmd->CreateParameter( "SID",		adInteger, adParamInput, 4, sid ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OWNER_ID",	adInteger, adParamInput, 4, owner_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "ACCOUNT_ID",adInteger, adParamInput, 4, account_id ) );

	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}



bool DB_UpdateRandomOptionOwnerInfo::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();
		throw;
	}

	m_pPlayer->onEndQuery();
	return true;
}
