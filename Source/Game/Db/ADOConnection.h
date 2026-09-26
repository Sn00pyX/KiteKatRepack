#pragma once

#ifndef _PRECOMPILED_HEADER
#define no_implementation
#endif

#import "Common Files\System\ADO\msado60_Backcompat.tlb" no_implementation no_namespace rename("EOF", "EndOfFile")

#ifndef _PRECOMPILED_HEADER
#undef no_implementation
#endif

#include <oledb.h>
#include <icrsint.h>

struct DBConnection
{
	DBConnection();
	virtual ~DBConnection();

	bool	CreateCommand( _CommandPtr& cmd );

	_ConnectionPtr	connection;
	_CommandPtr		command;

};


bool	CreateDBCommand( _CommandPtr& cmd, _ConnectionPtr connection );


void	LogDBError( _com_error& e, const char* szClassName, const char* szStoredProcedureName );
void	LogDBError( _com_error& e,
				   int nThreadNum,
				   const char* szClassName,
				   const char* szStoredProcedureName,
				   const char* szDebugInfo );

