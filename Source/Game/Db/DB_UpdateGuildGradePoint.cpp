
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "GuildManager.h"
#include "DB_Commands.h"



bool DB_UpdateGuildGradePoint::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if (db.CreateCommand(cmd) == false)	throw XException("DB_UpdateGuildGradePoint : CreateInstance(command) error");

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t("dbo.smp_update_guild_grade_point");
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_guild_grade_point";

	cmd->Parameters->Append(cmd->CreateParameter("IN_GUILD_SID", adInteger, adParamInput, 4, m_nGuildID));
	cmd->Parameters->Append(cmd->CreateParameter("IN_GRADE", adInteger, adParamInput, 4, m_nGrade));
	cmd->Parameters->Append(cmd->CreateParameter("IN_POINT", adInteger, adParamInput, 4, m_nPoint));

	cmd->Execute(NULL, NULL, adCmdStoredProc);

	return true;
}

bool DB_UpdateGuildGradePoint::onProcess( DBConnection & db )
{
	try
	{
		proc( db );
	}
	catch( ... )
	{
		GuildManager::GetInstance().onEndQuery();

		throw;
	}

	GuildManager::GetInstance().onEndQuery();

	return true;
}

