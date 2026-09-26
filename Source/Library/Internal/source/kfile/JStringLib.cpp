
#include <windows.h>
#include "../../include/kfile/JStringLib.h"

namespace JStringLib
{
	void ReplaceString( std::string& strText, const char* szFind, const char* szReplace )
	{
		for ( size_t nCurrentPos = 0; ; )
		{
			size_t nFindPos = strText.find( szFind, nCurrentPos );	if ( nFindPos == std::string::npos ) break;
			strText.replace( nFindPos, 1, szReplace );
			nCurrentPos = (nFindPos + 1);
		}
	}

	bool IsFullPath( const char* szFile )
	{
		std::string strFile = szFile;
		ReplaceString(strFile, "\\", "/");
		
		size_t pos = strFile.find( "/", 0 );	
		if ( pos == std::string::npos )	return false;
		return true;
	}
	
	void GetFilePathFromFile( std::string & strFullPath )
	{
		ReplaceString( strFullPath, "\\", "/" );

		size_t pos = strFullPath.find_last_of( "/" );
		if ( pos == std::string::npos )
		{
			char szDir[MAX_PATH];
			::memset( szDir, 0, sizeof( szDir ) );
			::GetCurrentDirectory( MAX_PATH, szDir );
			strFullPath =  szDir;
			ReplaceString( strFullPath, "\\", "/" );
		}
		else
		{
			strFullPath = strFullPath.substr( 0, pos+1 );
		}

		std::string strTemp = strFullPath.substr( strFullPath.size()-1, 1 );
		if ( strTemp != "/" )	strFullPath += "/";
	}

	void GetFileNameFormPath( std::string & strFileName )
	{
		ReplaceString( strFileName, "\\", "/" );

		std::string strDir = strFileName;
		GetFilePathFromFile( strDir );

		std::string strName = strFileName;

		strFileName = strName.substr( strDir.size(), strName.size() - strDir.size() );
	}
}