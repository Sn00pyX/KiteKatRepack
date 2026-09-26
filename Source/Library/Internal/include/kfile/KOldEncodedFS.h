/**
	@brief	게임용으로 특화된 FS
*/

#pragma once

#include "IFileSystem.h"
#include <string>
#include "../toolkit/khash.h"

struct KFileSystemWrapper : KFileSystem
{
	KFileSystemWrapper();
	virtual ~KFileSystemWrapper();

	struct NameCipher
	{
		virtual bool is_encoded_file_name( std::string & strFileName )	{ return true; }
		virtual void encode_file_name( std::string & strFileName )		{};
		virtual void decode_file_name( std::string & strFileName )		{};
	};

	bool					Init( KFileSystem* pFS, NameCipher *pCipher = NULL, bool bUseXOR = true, bool bSmartXOR = false );

	
	virtual KStream*		open( const char* szFileName, ACCESS_MODE access_mode );

	virtual std::string		getFullPathName( const char* szFileName ) const;

	virtual void			doEachFile( FileHandler & handler );
	virtual bool			isFile( const char *szFileName ) const;

	virtual size_t			getFileSize( const char *szFileName ) const;

	virtual std::string		getCurDirectory() const										{ return ""; }
	virtual bool			changeDirectory( const char *szDirectory )					{ return false; }
	virtual bool			makeDirectory( const char *szDirectory )					{ return false; }
	virtual bool			deleteDirectory( const char *szDirectory )					{ return false; }
	virtual bool			deleteFile( const char *szFile );

private:

	bool						m_bUseXOR, m_bSmartXOR;
	KFileSystem*				m_pFileSystem;
	NameCipher*					m_pNameCipher;
};