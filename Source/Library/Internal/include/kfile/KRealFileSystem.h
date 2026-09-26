
/**
	@file	KRealFileSystem.h
	@brief	게임용으로 특화된 FS
*/

#pragma once

#include "IFileSystem.h"
#include <string>
#include "../toolkit/khash.h"

struct KRealFileSystem : KFileSystem
{
	KRealFileSystem();
	virtual ~KRealFileSystem();

	bool					Init( const char *szRootPath/*, NameCipher *pNameCipher*/ );
	bool					DeInit();

	virtual KStream*		open( const char* szFileName, ACCESS_MODE access_mode );

	virtual std::string		getFullPathName( const char* szFileName ) const;

	virtual void			doEachFile( FileHandler & handler )							{}
	virtual bool			isFile( const char *szFileName ) const;

	virtual size_t			getFileSize( const char *szFileName ) const;

	virtual std::string		getCurDirectory() const										{ return ""; }
	virtual bool			changeDirectory( const char *szDirectory )					{ return false; }
	virtual bool			makeDirectory( const char *szDirectory )					{ return false; }
	virtual bool			deleteDirectory( const char *szDirectory )					{ return false; }
	virtual bool			deleteFile( const char *szFile )							{ return false; }

private:

	KHash< std::string, hashPr_string_nocase >	m_hsIndex;
	std::string									m_strBaseDirectory;
	//NameCipher									*m_pNameCipher;
};

struct KStdioFileSystem : KRealFileSystem
{
	virtual KStream*		open( const char* szFileName, ACCESS_MODE access_mode );

	virtual std::string		getFullPathName( const char* szFileName ) const;

	virtual bool			isFile( const char *szFileName ) const;

	virtual size_t			getFileSize( const char *szFileName ) const;
};