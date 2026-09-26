
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "GuildManager.h"
#include "DB_Commands.h"



bool DB_UpdateGuildMemberPoint::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if (db.CreateCommand(cmd) == false)	throw XException("DB_UpdateGuildMemberPoint : CreateInstance(command) error");

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t("dbo.smp_update_guild_member_point");
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_guild_member_point";

	cmd->Parameters->Append(cmd->CreateParameter("IN_PLAYER_SID", adInteger, adParamInput, 4, m_nPlayerSID));
	cmd->Parameters->Append(cmd->CreateParameter("IN_POINT", adInteger, adParamInput, 4, m_nGuildPoint));
	cmd->Parameters->Append(cmd->CreateParameter("IN_TOTAL_POINT", adInteger, adParamInput, 4, m_nGuildTotalPoint));

	cmd->Execute(NULL, NULL, adCmdStoredProc);

	return true;
}

bool DB_UpdateGuildMemberPoint::onProcess( DBConnection & db )
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

