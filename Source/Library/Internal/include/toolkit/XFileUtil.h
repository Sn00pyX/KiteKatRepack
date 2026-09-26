#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>
#include <vector>
#include <limits>


namespace XFileUtil
{
	extern const char *PATH_SEPARATOR;
	extern const char  PATH_SEPARATOR_CHAR;

	// 파일 유틸들
	enum { INVALID_SIZE = UINT_MAX };
	bool		IsDirectory( const char *szFileName );
	bool		IsFile( const char *szFileName );

	size_t		ReadFile( const char *szFileName, void *pBuf, size_t nBufferSize );
	bool		WriteFile( const char *szFileName, const void *pBuf, size_t nBufferSize );

	size_t		GetFileSize( const char *szFileName );
	bool		GetFileName( const char *szFullPathName, std::string & strFileName );
	bool		GetFileExtension( const char *szFullPathName, std::string & strExtension );

	void		NomalizeDirectoryName( std::string & strPathName );
	void		NomalizeFileName( std::string & strPathName );

	bool		CreatePath( const char *szFullDirectoryName );

	DWORD		ReadTxtFile( const char* szFileName, UINT codepage, std::wstring& strText );
	DWORD		ReadTxtFile( const char* szFileName, UINT codepage, std::string& strText );
};