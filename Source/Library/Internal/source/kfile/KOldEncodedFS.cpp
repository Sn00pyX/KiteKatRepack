
#include "../../include/kfile/KOldEncodedFS.h"
#include "../../include/kfile/XOREn.h"
#include "../../include/kfile/KEncodedFile.h"
#include "../../include/toolkit/safe_function.h"



std::string getFileExtention( const char *szFileName )
{
	for ( int idx = static_cast< int >( strlen(szFileName) )-1; idx >= 0 ; --idx )
	{
		if ( szFileName[idx] == '.' )
		{
			return &szFileName[idx+1];
		}
	}

	return "";
}

KFileSystemWrapper::KFileSystemWrapper()
{
	m_pNameCipher = NULL;
	m_pFileSystem = NULL;
	m_bSmartXOR   = false;
	m_bUseXOR	  = false;
}

KFileSystemWrapper::~KFileSystemWrapper()
{
}

bool KFileSystemWrapper::Init( KFileSystem* pFS, NameCipher *pCipher, bool bUseXOR, bool bSmartXOR )
{
	m_pNameCipher = pCipher;
	m_pFileSystem = pFS;
	m_bUseXOR	  = bUseXOR;
	m_bSmartXOR   = bSmartXOR;
	
	return true;
}

bool KFileSystemWrapper::deleteFile( const char* szFileName )
{
	std::string strFileName = szFileName;
	if ( strFileName.empty() )	return NULL;

	char buf[256] = { 0, };
	s_strcpy( buf, _countof( buf ), strFileName.c_str() );
	s_tolower( buf, _countof( buf ) );
	strFileName = buf;

	if ( m_pNameCipher ) m_pNameCipher->encode_file_name( strFileName );
	
	return m_pFileSystem->deleteFile( strFileName.c_str() );
}

KStream* KFileSystemWrapper::open( const char* szFileName, ACCESS_MODE access_mode )
{
	std::string strFileName = szFileName;
	if ( strFileName.empty() )	return NULL;

	char buf[256] = { 0, };
	s_strcpy( buf, _countof( buf ), strFileName.c_str() );
	s_tolower( buf, _countof( buf ) );
	strFileName = buf;

	if ( m_pNameCipher ) m_pNameCipher->encode_file_name( strFileName );
	
	KStream* pFile = m_pFileSystem->open( strFileName.c_str(), access_mode );
	if ( !pFile ) return NULL;

	// XOR 안하는 경우
	if ( !m_bUseXOR ) return pFile;

	if ( m_bSmartXOR && strFileName.length() > 4 )
	{
		const char *szExtension = ( szFileName + strlen(szFileName) - 4 );
		if ( !_stricmp( szExtension, ".dds" ) || 
			!_stricmp( szExtension, ".tga" ) || 
			!_stricmp( szExtension, ".naf" ) || 
			!_stricmp( szExtension, ".nx3" ) || 
			!_stricmp( szExtension, ".cob" ) || 
			!_stricmp( szExtension, ".itc" ) || 
			!_stricmp( szExtension, ".itm" ) || 
			!_stricmp( szExtension, ".itb" ) || 
			!_stricmp( szExtension, ".nfm" ) ) return pFile;
	}
	
	return new KEncodedFile( pFile );
}

std::string KFileSystemWrapper::getFullPathName( const char* szFileName ) const
{
	std::string strFileName = szFileName;
	if ( m_pNameCipher && !m_pNameCipher->is_encoded_file_name(strFileName) ) 
		m_pNameCipher->encode_file_name( strFileName );

	return m_pFileSystem->getFullPathName( strFileName.c_str() );
}

void KFileSystemWrapper::doEachFile( FileHandler & handler )
{
	struct myFileHandler : KFileSystem::FileHandler
	{
		myFileHandler( FileHandler & h, NameCipher * c ) : handler( h ), pCipher( c )
		{
		}

		virtual bool onFile( const char *szFileName )
		{
			std::string strName = szFileName;
			if ( pCipher )
			{
				pCipher->decode_file_name( strName );
			}
			handler.onFile( strName.c_str() );
			return true;
		}

		NameCipher * pCipher;
		FileHandler & handler;

	private:

		const myFileHandler& operator=( const myFileHandler& )
		{
			return *this;
		}
	};

	myFileHandler temp( handler, m_pNameCipher );

	
	return m_pFileSystem->doEachFile( temp );
}

bool KFileSystemWrapper::isFile( const char *szFileName ) const
{
	std::string strFileName = szFileName;
	
	char buf[256] = { 0, };
	s_strcpy( buf, _countof( buf ), strFileName.c_str() );
	s_tolower( buf, _countof( buf ) );
	strFileName = buf;

	if ( m_pNameCipher ) 
		m_pNameCipher->encode_file_name( strFileName );

	return m_pFileSystem->isFile( strFileName.c_str() );
}

size_t KFileSystemWrapper::getFileSize( const char *szFileName ) const
{
	std::string strFileName = szFileName;
	if ( m_pNameCipher && !m_pNameCipher->is_encoded_file_name(strFileName) ) 
		m_pNameCipher->encode_file_name( strFileName );

	return m_pFileSystem->getFileSize( strFileName.c_str() );
}