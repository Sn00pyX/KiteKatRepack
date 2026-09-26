
#include <toolkit/XConsole.h>

#include "DB_Commands.h"
#include "StructItem.h"

bool DB_UpdateItemCode::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateItemCode : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_item_code" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_item_code";
	s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_SID: %d, @IN_CODE: %d", m_pItem->GetItemUID(), m_pItem->GetItemCode() );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adBigInt, adParamInput, 8, m_pItem->GetItemUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, m_pItem->GetItemCode() ) );	

	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}

bool DB_UpdateItemCode::onProcess( DBConnection & db )
{
	try
	{
		bool bRtn = true;

		if( !m_pItem->IsVirtualItem() )
			bRtn = proc( db );
	}
	catch( ... )
	{
		m_pItem->onEndQuery();
		throw;
	}

	m_pItem->onEndQuery();

	return true;
}