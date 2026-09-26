
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "../../include/toolkit/XFileUtil.h"
#include "../../include/toolkit/XStringUtil.h"
#include "../../include/toolkit/safe_function.h"


namespace XFileUtil
{

	const char *PATH_SEPARATOR		= "\\";
	const char  PATH_SEPARATOR_CHAR	= '\\';

	size_t ReadFile( const char *szFileName, void *pBuf, size_t nBufferSize )
	{
		FILE *fp = NULL;
		if( fopen_s( &fp, szFileName, "rb" ) != 0 )  return static_cast< size_t >( INVALID_SIZE );
		if( !fp ) return static_cast< size_t >( INVALID_SIZE );
		size_t rtn = fread( pBuf, 1, nBufferSize, fp );
		fclose( fp );
		return rtn;
	}

	bool WriteFile( const char *szFileName, const void *pBuf, size_t nBufferSize )
	{
		FILE *fp = NULL;
		if( fopen_s( &fp, szFileName, "w+b" ) != 0 ) return false;
		if( !fp ) return false;
		if( fwrite( pBuf, 1, nBufferSize, fp ) != nBufferSize ) 
		{
			fclose( fp );
			DeleteFile( szFileName );
			return false;
		}
		fclose( fp );
		return true;
	}

	size_t GetFileSize( const char *szFileName )
	{
		struct _stati64 tmp;
		if( ::_stati64( szFileName, &tmp ) ) return 0;
		return (size_t)tmp.st_size;
	}

	bool IsDirectory( const char *szFileName )
	{
		char szDirectoryName[ MAX_PATH ];
		s_strcpy( szDirectoryName, _countof( szDirectoryName ), szFileName );

		size_t len = strlen( szDirectoryName );
		if( len == 0 ) return false;

		while( szDirectoryName[ len - 1 ] == PATH_SEPARATOR_CHAR )
		{
			szDirectoryName[ len - 1 ] = 0;
			len--;
			if( len == 0 ) return false;
		}

		struct _stati64 tmp;
		if( _stati64( szDirectoryName, &tmp ) ) return false;

		return !!( tmp.st_mode & _S_IFDIR );
	}

	bool CreatePath( const char *szFullDirectoryName )
	{
		std::vector< std::string > vList;
		std::string strPath;
		XStringUtil::Split( szFullDirectoryName, vList, PATH_SEPARATOR, false );

		for( size_t i = 0; i < vList.size(); ++i )
		{
			strPath += vList[i];		
			strPath += PATH_SEPARATOR;

			NomalizeDirectoryName( strPath );

			if( vList[i].find( ":" ) == vList[i].npos && !IsDirectory( strPath.c_str() ) ) 
			{
				if( !CreateDirectory( strPath.c_str(), NULL ) ) return false;
			}		
		}

		return true;
	}

	bool IsFile( const char *szFileName )
	{
		struct  _stati64 tmp;
		if( _stati64( szFileName, &tmp ) ) return false;

		return !( tmp.st_mode & _S_IFDIR );
	}

	void NomalizeFileName( std::string & strPathName )
	{
		if( strPathName.empty() ) return;

		for( size_t idx = 0; idx < strPathName.size(); ++idx )
		{
			if( strPathName[ idx ] == '/' ) strPathName[ idx ] = PATH_SEPARATOR_CHAR;
		}
	}

	void NomalizeDirectoryName( std::string & strPathName )
	{
		NomalizeFileName( strPathName );

		if( strPathName[ strPathName.size() - 1 ] != '/' && strPathName[ strPathName.size() - 1 ] != PATH_SEPARATOR_CHAR ) strPathName += PATH_SEPARATOR;
	}

	bool GetFileName( const char *szFullPathName, std::string & strFileName )
	{
		std::string strFullPathName( szFullPathName );

		if( strFullPathName.empty() ) return false;
		
		NomalizeFileName( strFullPathName );	

		for( int idx = (int)strFullPathName.size() - 1; idx > 0 ; --idx )
		{
			if( strFullPathName[ idx ] == '/' || strFullPathName[ idx ] == PATH_SEPARATOR_CHAR ) 
			{
				strFileName.assign( strFullPathName.begin() + idx + 1, strFullPathName.end() );
				return true;
			}
		}

		strFileName = strFullPathName;
		return true;
	}

	bool GetFileExtension( const char *szFullPathName, std::string & strExtension )
	{
		std::string strFullPathName = szFullPathName;
		std::string strFileName;

		GetFileName( strFullPathName.c_str(), strFileName );

		if( strFileName.empty() ) return false;

		for( int idx = (int)strFileName.size() - 1; idx > 0 ; --idx )
		{
			if( strFileName[ idx ] == '.' )
			{
				strExtension.assign( strFileName.begin() + idx+1, strFileName.end() );
				return true;
			}
		}

		return true;
	}

	enum
	{
		BOMTYPE_ASCII,		// ascii
		BOMTYPE_UTF8,		// utf-8
		BOMTYPE_UTF16_LE,	// utf-16 리틀 엔디안
	};

	DWORD	ReadBOMTXTFile( const char* szFileName, int& nBOMType, BYTE*& byBytes )
	{
		HANDLE hTXTFile = CreateFile( szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, NULL, NULL );
		if( hTXTFile == INVALID_HANDLE_VALUE )
		{
			return GetLastError();
		}

		DWORD nReadBytes = 0;
		BYTE byReadBOM[4] = { 0, };
		::ReadFile( hTXTFile, byReadBOM, sizeof( byReadBOM ), &nReadBytes, NULL );

		nBOMType = BOMTYPE_ASCII;
		if( byReadBOM[0] == 0xEF && byReadBOM[1] == 0xBB && byReadBOM[2] == 0xBF )	// utf-8
		{
			::SetFilePointer( hTXTFile, 3, NULL, FILE_BEGIN );
			nBOMType = BOMTYPE_UTF8;
		}
		else if( byReadBOM[0] == 0xFF && byReadBOM[1] == 0xFE )	// UTF-16 리틀 엔디안
		{
			::SetFilePointer( hTXTFile, 2, NULL, FILE_BEGIN );
			nBOMType = BOMTYPE_UTF16_LE;
		}
		else	// 다른 것도 있는데 윈도우에서는 잘 사용 안되므로 그냥 ascii로 처리한다.
		{
			::SetFilePointer( hTXTFile, 0, NULL, FILE_BEGIN );
			nBOMType = BOMTYPE_ASCII;
		}

		DWORD nFileSize = ::GetFileSize( hTXTFile, NULL );
		byBytes = new BYTE[ nFileSize + sizeof( wchar_t ) ];

		DWORD dwReadBytes = 0;
		if( !::ReadFile( hTXTFile, byBytes, nFileSize, &dwReadBytes, NULL ) )
		{
			DWORD error = ::GetLastError();
			CloseHandle( hTXTFile );

			delete [] byBytes;
			byBytes = NULL;
			return error;
		}

		CloseHandle( hTXTFile );
		hTXTFile = INVALID_HANDLE_VALUE;

		::memset( byBytes+dwReadBytes, 0, (nFileSize+sizeof( wchar_t )) - dwReadBytes );
		return ERROR_SUCCESS;
	}

	DWORD	ReadTxtFile( const char* szFileName, UINT codepage, std::wstring& strText )
	{
		int nBOMType = BOMTYPE_ASCII;
		BYTE* byBytes = NULL;
		DWORD error = ReadBOMTXTFile( szFileName, nBOMType, byBytes );
		if( error != ERROR_SUCCESS )
		{
			return error;
		}

		if( byBytes == NULL )
		{
			return ERROR_NOT_ENOUGH_MEMORY;
		}

		if( nBOMType == BOMTYPE_UTF16_LE )
		{
			strText = reinterpret_cast< wchar_t* >( byBytes );
		}
		else if( nBOMType == BOMTYPE_UTF8 )
		{
			strText = XStringUtil::Multi2Wide( reinterpret_cast< const char* >( byBytes ), CP_UTF8 );
		}
		else	// BOMTYPE_ASCII
		{
			strText = XStringUtil::Multi2Wide( reinterpret_cast< const char* >( byBytes ), codepage );
		}

		delete [] byBytes;
		return ERROR_SUCCESS;
	}

	DWORD	ReadTxtFile( const char* szFileName, UINT codepage, std::string& strText )
	{
		int nBOMType = BOMTYPE_ASCII;
		BYTE* byBytes = NULL;
		DWORD error = ReadBOMTXTFile( szFileName, nBOMType, byBytes );
		if( error != ERROR_SUCCESS )
		{
			return error;
		}

		if( byBytes == NULL )
		{
			return ERROR_NOT_ENOUGH_MEMORY;
		}

		if( nBOMType == BOMTYPE_UTF16_LE )
		{
			strText = XStringUtil::Wide2Multi( reinterpret_cast< const wchar_t* >( byBytes ), codepage );
		}
		else if( nBOMType == BOMTYPE_UTF8 )
		{
			std::wstring wstrText = XStringUtil::Multi2Wide( reinterpret_cast< const char* >( byBytes ), CP_UTF8 );
			strText = XStringUtil::Wide2Multi( wstrText.c_str(), codepage );
		}
		else	// BOMTYPE_ASCII
		{
			strText = reinterpret_cast< char* >( byBytes );
		}

		delete [] byBytes;
		return ERROR_SUCCESS;
	}


};