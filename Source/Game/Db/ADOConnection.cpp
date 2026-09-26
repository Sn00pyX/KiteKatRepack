
#include <toolkit/XConsole.h>

#include "ADOConnection.h"


DBConnection::DBConnection()
{
}

DBConnection::~DBConnection()
{
}

bool	DBConnection::CreateCommand( _CommandPtr& cmd )
{
	return CreateDBCommand( cmd, connection );
}

bool	CreateDBCommand( _CommandPtr& cmd, _ConnectionPtr connection )
{
	if( FAILED( cmd.CreateInstance(__uuidof(Command)) ) )
	{
		return false;
	}

	cmd->ActiveConnection = connection;
	return true;
}

void	LogDBError( _com_error& e, const char* szClassName, const char* szStoredProcedureName )
{
	LogDBError( e, 0, szClassName, szStoredProcedureName, NULL );
}

void	LogDBError( _com_error& e,
				   int nThreadNum,
				   const char* szClassName,
				   const char* szStoredProcedureName,
				   const char* szDebugInfo )
{
	char buf[512] = { 0, };

	GUID guidError = e.GUID();
	s_sprintf( buf, _countof( buf ), "DB COM ERROR(Thread:%d, HRESULT:%08X, GUID:%08lX-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X) : %s(%s",
		nThreadNum, e.Error(),
		guidError.Data1, guidError.Data2, guidError.Data3,
		guidError.Data4[0], guidError.Data4[1], guidError.Data4[2], guidError.Data4[3],
		guidError.Data4[4], guidError.Data4[5], guidError.Data4[6], guidError.Data4[7],
		szClassName, ( szStoredProcedureName ) ? szStoredProcedureName : "Unnamed" );

	std::string strError( buf );

	if( szDebugInfo != NULL )
	{
		strError += ": ";
		strError += szDebugInfo;
	}
	strError += ") : ";

	if( e.Description().length() )
		strError += e.Description();
	else
		strError += "<NULL>";

	strError += "\n";

	_lprint( "GameLog.txt", "%s", strError.c_str() );
}
