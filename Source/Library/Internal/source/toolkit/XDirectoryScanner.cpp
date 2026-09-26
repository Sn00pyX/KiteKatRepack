
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <sys/stat.h>

#include "../../include/toolkit/XDirectoryScanner.h"
#include "../../include/toolkit/XFileUtil.h"
#include "../../include/toolkit/XStringUtil.h"
#include "../../include/toolkit/safe_function.h"


struct Win32Scanner : IDirectoryScanner
{
	bool Scan( const char *szDirectory, Scanner *pScanner, const char* szFilter );
};

bool Win32Scanner::Scan( const char *szDirectory, Scanner *pScanner, const char* szFilter )
{
	struct _Scanner : Scanner
	{
		_Scanner( Scanner * _p ) : p( _p )	{}

		bool onDirectory( const char *szFullPath, const char *szDirectory, const char *szFileName )
		{
			return p->onDirectory( szFullPath, szDirectory, szFileName );
		}

		bool onFile( const char *szFullPath, const char *szDirectory, const char *szFileName )
		{
			return p->onFile( szFullPath, szDirectory, szFileName );
		}

		Scanner * p;

	} _scanner( pScanner );

	WIN32_FIND_DATA FileData; 
	HANDLE hSearch; 

	std::string strCurrentDirectory = szDirectory;
	XFileUtil::NomalizeDirectoryName( strCurrentDirectory );

	if( szFilter == NULL )
	{
		szFilter = "*";
	}

	hSearch = FindFirstFile( ( strCurrentDirectory + szFilter ).c_str() , &FileData); 
	if (hSearch == INVALID_HANDLE_VALUE) 
	{ 
		return false;
	} 
	 
	std::string strFullPath;
	while ( true ) 
	{ 
		if( strcmp( FileData.cFileName, "." ) &&
			strcmp( FileData.cFileName, ".." ) )
		{
			size_t len = strlen( FileData.cFileName );
			for( size_t idx = 0; idx < len; ++idx ) if( FileData.cFileName[idx] == '/' ) FileData.cFileName[idx] = '\\';

			strFullPath = strCurrentDirectory;
			strFullPath += FileData.cFileName;

			if( XFileUtil::IsDirectory( strFullPath.c_str() ) )
			{	
			
				strFullPath += "\\";
				if( !pScanner->onDirectory( strFullPath.c_str(), strCurrentDirectory.c_str(), FileData.cFileName ) ) break;
				Scan( strFullPath.c_str(), &_scanner, szFilter );
			}
			else
			{
				if( !pScanner->onFile( strFullPath.c_str(), strCurrentDirectory.c_str(), FileData.cFileName ) ) break;
			}
		}

		if (!FindNextFile(hSearch, &FileData)) 
		{
			if (GetLastError() == ERROR_NO_MORE_FILES) 
			{ 
				return true;
			} 
			else 
			{ 
				return false;
			} 
		}
	} 
	 
	FindClose(hSearch);
	
	return true;
}

bool IDirectoryScanner::EnumFiles( const char *szDirectory, std::vector< std::string > * pvList )
{
	struct _Scanner : Scanner
	{
		virtual bool onDirectory( const char *szFullPath, const char *szDirectory, const char *szFileName )	{ return true; }
		virtual bool onFile( const char *szFullPath, const char *szDirectory, const char *szFileName )		{ pList->push_back( szFullPath ); return true; }
		std::vector< std::string > * pList;
	};

	_Scanner temp;
	temp.pList = pvList;

	return Scan( szDirectory, &temp );
}

bool IDirectoryScanner::EnumFiles( const char *szDirectory, const char *szWildCard, std::vector< std::string > * vList )
{
	struct _Scanner : Scanner
	{
		virtual bool onDirectory( const char *szFullPath, const char *szDirectory, const char *szFileName )	{ return true; }

		virtual bool onFile( const char *szFullPath, const char *szDirectory, const char *szFileName )
		{
			if( XStringUtil::WildCardCmp( szWildCard, szFileName, true ) )
			{
				pList->push_back( szFullPath );
			}
			return true;
		}
		const char *szWildCard;
		std::vector< std::string > * pList;
	};

	_Scanner temp;
	temp.szWildCard = szWildCard;
	temp.pList = vList;

	return Scan( szDirectory, &temp );
}

IDirectoryScanner & IDirectoryScanner::Instance()
{
	static Win32Scanner _scanner;
	return _scanner;
}

