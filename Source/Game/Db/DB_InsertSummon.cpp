
#include <toolkit/XConsole.h>


#include "DB_Commands.h"
#include "StructPlayer.h"

/*

@OUT_SID		INT OUTPUT,
@IN_OWNER_ID	INT,
@IN_CODE		INT,
@IN_CARD_UID	BIGINT,
@IN_EXP			BIGINT,
@IN_JP			INT,
@IN_NAME		VARCHAR(64),
@IN_TRANSFORM	INT,
@IN_LV			INT,
@IN_JLV			INT,
@IN_FP			INT,
@IN_SP			INT,
@IN_HP			INT,
@IN_MP			INT

*/


bool DB_InsertSummon::proc( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertSummon : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_summon" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_insert_summon";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, sid ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, account_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, owner_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, code ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CARD_UID", adBigInt, adParamInput, 4, card_uid ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, strName.length(), strName ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SP", adInteger, adParamInput, 4, sp ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_HP", adInteger, adParamInput, 4, hp ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_MP", adInteger, adParamInput, 4, mp ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JP", adBigInt, adParamInput, 8, jp ) );
	
	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}

bool DB_InsertSummon::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		m_pMaster->onEndQuery();
		throw;
	}

	m_pMaster->onEndQuery();
	return true;
}