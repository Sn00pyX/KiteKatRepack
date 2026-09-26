
#include <toolkit/XConsole.h>

#include "DB_Commands.h"
#include "StructItem.h"

bool DB_UpdateItemOwner::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateItemOwner : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_item_owner" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_item_owner";
	s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_SID: %d, @IN_OWNER_ID: %d, @IN_ACCOUNT_ID: %d", m_pItem->GetItemUID(), m_pItem->GetOwnerUID(), m_pItem->GetAccountID() );
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adBigInt, adParamInput, 8, m_pItem->GetItemUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_pItem->GetOwnerUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pItem->GetAccountID() ) );	
	
	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}

bool DB_UpdateItemOwner::onProcess( DBConnection & db )
{
	try
	{
		if( !m_pItem->IsVirtualItem() )
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