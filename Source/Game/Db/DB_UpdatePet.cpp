
#include <toolkit/XConsole.h>

#include "DB_Commands.h"
#include "StructPlayer.h"
#include "StructPet.h"
#include "StructSkill.h"


bool DB_UpdatePet::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdatePet : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_pet" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_pet";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPet->GetPetSID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, ( m_pPet->GetParentCage() ) ? m_pPet->GetParentCage()->GetAccountID() : 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, ( m_pPet->GetMaster() ) ? m_pPet->GetMaster()->GetPlayerUID() : 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_COOL_TIME_01", adInteger, adParamInput, 4, m_pPet->GetSkill( StructSkill::SKILL_SHOVELING )->GetRemainCoolTime( GetArTime() ) / 100 ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_UpdatePet::onProcess( DBConnection & db )
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

bool DB_ChangePetName::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ChangePetName : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_change_pet_name" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_change_pet_name";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPet->GetPetSID() ) );
	_bstr_t strName( m_pPet->GetName() );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, strName.length(), strName ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_ChangePetName::onProcess( DBConnection & db )
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
