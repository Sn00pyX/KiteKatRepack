
#include <toolkit/XConsole.h>

#include "DB_Commands.h"

#include "StructPlayer.h"
#include "StructPet.h"


DB_InsertPet::DB_InsertPet( struct StructPet * pPet ) : DBProc( "DB_InsertPet" ), m_pPet( pPet )
	, sid( pPet->GetPetSID() )
	, account_id( ( pPet->GetParentCage() ) ? pPet->GetParentCage()->GetAccountID() : 0 )
	, owner_id( ( pPet->GetMaster() ) ? pPet->GetMaster()->GetPlayerUID() : 0 )
	, cage_uid( ( pPet->GetParentCage() ) ? pPet->GetParentCage()->GetItemUID() : 0 )
	, code( pPet->GetPetCode() )
	, name_changed( pPet->IsNameChanged() )
	, name( pPet->GetName() )
{
}

bool DB_InsertPet::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertPet : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_pet" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_insert_pet";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, sid ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, account_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, owner_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CAGE_UID", adBigInt, adParamInput, 4, cage_uid ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, code ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME_CHANGED", adInteger, adParamInput, 4, name_changed ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, name.length(), name ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_InsertPet::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		m_pPet->onEndQuery();
		throw;
	}
	m_pPet->onEndQuery();

	return true;
}
