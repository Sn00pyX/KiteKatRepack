#pragma once

#include <string>
#include <vector>

struct IDirectoryScanner
{
	struct Scanner
	{
		virtual bool onDirectory( const char *szFullPath, const char *szDirectory, const char *szFileName )	{ return true; }
		virtual bool onFile( const char *szFullPath, const char *szDirectory, const char *szFileName )	{ return true; }
	};

	virtual bool Scan( const char *szDirectory, Scanner *pScanner, const char* szFilter = "*" ) = 0;

	bool		EnumFiles( const char* szDirectory, std::vector< std::string > * vList );
	bool		EnumFiles( const char* szDirectory, const char *szWildCard, std::vector< std::string > * vList );

	static IDirectoryScanner & Instance();

};