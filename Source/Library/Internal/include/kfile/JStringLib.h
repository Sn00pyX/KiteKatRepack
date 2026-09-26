
#pragma once


#include <string>

namespace JStringLib
{
	const int MAX_FILE_WRITE_SIZE = 50*1024*1024;

	void ReplaceString( std::string& strText, const char* szFind, const char* szReplace );
	bool IsFullPath( const char* szFile );
	void GetFilePathFromFile( std::string & strFullPath );
	void GetFileNameFormPath( std::string & strFileName );
}