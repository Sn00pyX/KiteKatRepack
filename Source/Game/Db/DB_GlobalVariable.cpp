#include "GlobalVariableManager.h"

#include "DB_Commands.h"
#include "GameDBUtil.h"

#include <atlcomtime.h>


bool DB_InsertGlobalVariable::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertGlobalVariable : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_global_variable" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_global_variable";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adVarChar, adParamInput, (ADO_LONGPTR)m_strName.length(), m_strName.c_str() ) );
		_bstr_t bstrBuffer( m_strValue.c_str() );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_VALUE", adBSTR, adParamInput, bstrBuffer.length(), bstrBuffer ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		GVM().onEndQuery();
		throw;
	}

	GVM().onEndQuery();

	return true;
}

bool DB_UpdateGlobalVariable::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateGlobalVariable : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_set_global_variable" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_set_global_variable";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adVarChar, adParamInput, (ADO_LONGPTR)m_strName.length(), m_strName.c_str() ) );
		_bstr_t bstrBuffer( m_strValue.c_str() );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_VALUE", adBSTR, adParamInput, bstrBuffer.length(), bstrBuffer ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		GVM().onEndQuery();
		throw;
	}

	GVM().onEndQuery();

	return true;
}

bool DB_DeleteGlobalVariable::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteGlobalVariable : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_delete_global_variable" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_delete_global_variable";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adVarChar, adParamInput, (ADO_LONGPTR)m_strName.length(), m_strName.c_str() ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		GVM().onEndQuery();
		throw;
	}

	GVM().onEndQuery();

	return true;
}
