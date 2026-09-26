
#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"
#include "StructSummon.h"
#include "StructPlayer.h"

bool DB_InsertSkill::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertSkill : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_insert_skill" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_insert_skill";
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nUID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_nOwnerID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_JP_DEC", adInteger, adParamInput, 4, m_nJpDec ) );		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_ID", adInteger, adParamInput, 4, m_nSummonID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SKILL_ID", adInteger, adParamInput, 4, m_nSkillID ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SKILL_LEVEL", adInteger, adParamInput, 4, m_nSkillLevel ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_COOL_TIME", adInteger, adParamInput, 4, m_nCoolTime ) );

		cmd->Execute(NULL, NULL,adCmdStoredProc);

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

