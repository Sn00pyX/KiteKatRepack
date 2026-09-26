
#include "../../include/kfile/KRealFileSystem.h"
#include "../../include/toolkit/XDirectoryScanner.h"
#include <sys/types.h>
#include <sys/stat.h>

KRealFileSystem::KRealFileSystem()	{}
KRealFileSystem::~KRealFileSystem()	{}

bool KRealFileSystem::isFile( const char *szFileName ) const
{
	return m_hsIndex.has( szFileName );
}

size_t KRealFileSystem::getFileSize( const char *szFileName ) const
{
	std::string strFullPath;
	if ( !m_hsIndex.lookup( szFileName, strFullPath ) ) return 0;

	size_t sz = 0;
	FILE *fp = NULL;
	fopen_s( &fp, strFullPath.c_str(), "r" );
	if ( fp ) 
	{	
		sz = KRealFile( fp ).Size();
		fclose( fp );
	}
	return sz;
}

bool KRealFileSystem::Init( const char *szRootPath/*, NameCipher *pNameCipher*/ )
{
	/*m_pNameCipher = pNameCipher;

	struct _myScanner : IDirectoryScanner::Scanner
	{
		_myScanner( KHash< std::string, hashPr_string_nocase >	& hsIndex, NameCipher *pCipher ) : m_hsIndex( hsIndex ), m_pCipher( pCipher )  {}

		bool onFile( const char *szFullPath, const char *szDirectory, const char *szFileName )
		{
			std::string strFileName = szFileName;
			if ( m_pCipher ) 
			{
				if ( !m_pCipher->is_encoded_file_name( strFileName ) ) return true;
				m_pCipher->decode_file_name( strFileName );
			}

			m_hsIndex.add( strFileName, szFullPath );
			return true;
		}

		KHash< std::string, hashPr_string_nocase >	& m_hsIndex;
		NameCipher									* m_pCipher;

	} scanner( m_hsIndex, m_pNameCipher );

	return IDirectoryScanner::Instance().Scan( szRootPath, &scanner );*/

	// MJ 
	struct _myScanner : IDirectoryScanner::Scanner
	{
		_myScanner( KHash< std::string, hashPr_string_nocase >	& hsIndex ) : m_hsIndex( hsIndex )  {}

		bool onFile( const char *szFullPath, const char *szDirectory, const char *szFileName )
		{
			std::string strFileName = szFileName;
			m_hsIndex.add( strFileName, szFullPath );
			return true;
		}

		KHash< std::string, hashPr_string_nocase >	& m_hsIndex;

	private:

		const _myScanner& operator=( const _myScanner& )
		{
			return *this;
		}

	} scanner( m_hsIndex );

	return IDirectoryScanner::Instance().Scan( szRootPath, &scanner );
}

bool KRealFileSystem::DeInit()
{
	m_hsIndex.clear();

	return true;
}

std::string KRealFileSystem::getFullPathName( const char* szFileName ) const
{
	std::string strFullPath;

	m_hsIndex.lookup( szFileName, strFullPath );
	return strFullPath;
}

KStream* KRealFileSystem::open( const char* szFileName, ACCESS_MODE access_mode )
{
	std::string strFullPath;
	if ( !m_hsIndex.lookup( szFileName, strFullPath ) ) return NULL;

	const char* szAccessMode = "rb";

	// 우째야 할지.. --;
	if ( access_mode == READ_WRITE )	szAccessMode = "w+b";
	else if ( access_mode == READ_ONLY )	szAccessMode = "rb";
	else if ( access_mode == APPEND )	szAccessMode = "a+b";
	else if ( access_mode == CREATE )	szAccessMode = "w+b";
	else								szAccessMode = "w+b";

	FILE *fp = NULL;
	fopen_s( &fp, strFullPath.c_str(), szAccessMode );
	if ( !fp ) return NULL;

	return new KRealFile( fp );
}


// KStdioFileSystem implementation

KStream*	KStdioFileSystem::open( const char* szFileName, ACCESS_MODE access_mode )
{
	KStream* pFile = KRealFileSystem::open( szFileName, access_mode );
	
	if ( pFile == NULL )
	{
		const char* szAccessMode = "rb";

		if ( access_mode == READ_WRITE )	szAccessMode = "w+b";
		else if ( access_mode == READ_ONLY )	szAccessMode = "rb";
		else if ( access_mode == APPEND )	szAccessMode = "a+b";
		else if ( access_mode == CREATE )	szAccessMode = "w+b";
		else								szAccessMode = "w+b";

		FILE* fp = NULL;
		fopen_s( &fp, szFileName, szAccessMode );
		if ( fp != NULL )
			pFile = new KRealFile( fp );
	}
	return pFile;
}

std::string KStdioFileSystem::getFullPathName( const char* szFileName ) const
{
	std::string strPath = KRealFileSystem::getFullPathName( szFileName );

	return strPath.length() > 0 ? strPath : std::string( szFileName );
}

bool KStdioFileSystem::isFile( const char *szFileName ) const
{
	bool bFile = KRealFileSystem::isFile( szFileName );
	
	if ( !bFile ) {
		FILE* fp = NULL;
		fopen_s( &fp, szFileName, "rb" );
		if ( fp != NULL ) {
			bFile = true;
			fclose( fp );
		}
	}

	return bFile;
}

size_t KStdioFileSystem::getFileSize( const char *szFileName ) const
{
	size_t len = KRealFileSystem::getFileSize( szFileName );

	if ( len == 0 ) {
		FILE* fp = NULL;
		fopen_s( &fp, szFileName, "rb" );
		if ( fp != NULL ) {	
			len = KRealFile( fp ).Size();
			fclose( fp );
		}
	}

	return len;
}
