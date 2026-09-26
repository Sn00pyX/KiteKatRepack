
#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"
#include "StructSummon.h"
#include "StructPlayer.h"

bool DB_ResetSkill::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ResetSkill : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_reset_skill" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_reset_skill";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nOwnerID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_ID", adInteger, adParamInput, 4, m_nSummonID ) );

		cmd->Execute(NULL,NULL,adCmdStoredProc);

		if( m_pCreature->IsSummon() )
		{
			static_cast< StructSummon * >( m_pCreature )->onEndQuery();
		}
		else if( m_pCreature->IsPlayer() )
		{
			static_cast< StructPlayer * >( m_pCreature )->onEndQuery();
		}
	}
	catch( ... )
	{
		if( m_pCreature->IsSummon() )
		{
			static_cast< StructSummon * >( m_pCreature )->onEndQuery();
		}
		else if( m_pCreature->IsPlayer() )
		{
			static_cast< StructPlayer * >( m_pCreature )->onEndQuery();
		}

		throw;
	}

	return true;
}


bool DB_UpdateSkill::UpdateSkill( DBConnection & db , int nSID, int nSkillLevel, int nCoolTime )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateSkill : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_skill" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_skill";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, nSID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SKILL_LEVEL", adInteger, adParamInput, 4, nSkillLevel ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_COOL_TIME", adInteger, adParamInput, 4, nCoolTime ) );

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	catch( ... )
	{	
		throw;
	}

	return true;
}

bool DB_UpdateSkill::onProcess( DBConnection & db )
{
	bool bRet = false;
	try
	{
		bRet = UpdateSkill( db, m_nSID, m_nSkillLevel, m_nCoolTime );

		if( m_pCreature->IsSummon() )
		{
			static_cast< StructSummon * >( m_pCreature )->onEndQuery();
		}
		else if( m_pCreature->IsPlayer() )
		{
			static_cast< StructPlayer * >( m_pCreature )->onEndQuery();
		}
	}
	catch( ... )
	{

		if( m_pCreature->IsSummon() )
		{
			static_cast< StructSummon * >( m_pCreature )->onEndQuery();
		}
		else if( m_pCreature->IsPlayer() )
		{
			static_cast< StructPlayer * >( m_pCreature )->onEndQuery();
		}

		throw;
	}

	return bRet;
}


bool DB_DeleteSkill::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_DeleteSkill : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_delete_skill" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_delete_skill";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nSID ) );

		cmd->Execute(NULL,NULL,adCmdStoredProc);

		if( m_pCreature->IsSummon() )
		{
			static_cast< StructSummon * >( m_pCreature )->onEndQuery();
		}
		else if( m_pCreature->IsPlayer() )
		{
			static_cast< StructPlayer * >( m_pCreature )->onEndQuery();
		}
	}
	catch( ... )
	{
		if( m_pCreature->IsSummon() )
		{
			static_cast< StructSummon * >( m_pCreature )->onEndQuery();
		}
		else if( m_pCreature->IsPlayer() )
		{
			static_cast< StructPlayer * >( m_pCreature )->onEndQuery();
		}

		throw;
	}

	return true;
}