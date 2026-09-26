
#include "../../include/kfile/KFileManager.h"
#include "../../include/kfile/IFileSystem.h"
#include "../../include/kfile/KStream.h"
#include "../../include/toolkit/ILock.h"
#include "../../include/toolkit/nsl.h"
#include "../../include/toolkit/nsluni.h"
#include <windows.h>
#include "../../include/toolkit/SafeTickCount.h"

// 2010.04.29 - prodongi
#include "../../include/compress/XZip.h"

//#include "SDebug_Util.h"

static XCriticalSection s_lock;

 KFileManager & KFileManager::Instance()
{
	s_lock.Lock();
	static KFileManager _inst;
	s_lock.UnLock();

	return _inst;
}

// 2010.04.29 - prodongi
 KFileManager::KFileManager() : m_xzipBuffer(NULL)
{
}

KFileManager::~KFileManager()
{
        // 2010.04.29 - prodongi
	if (m_xzipBuffer)
		free(m_xzipBuffer);
}

KStream * getFileFromSourceList( KFileSystem* & pFileSystem, const std::vector< struct KFileSystem* > & vSourceList, const char *szReourceName )
{
	KStream * pFile = NULL;

	std::vector< struct KFileSystem* >::const_iterator it;
	for( it = vSourceList.begin(); it != vSourceList.end(); ++it )
	{
		pFileSystem = (*it);
		if( pFileSystem )
			pFile = pFileSystem->open( szReourceName, KFileSystem::READ_ONLY );
		if( pFile ) break;
	}

	return pFile;
}

void KFileManager::AddResourceSource( struct KFileSystem *pFileSystem )
{
	THREAD_SYNCRONIZE( m_lckIntf );

	m_vSourceList.push_back( pFileSystem );
}

void KFileManager::RemoveResourceSource( struct KFileSystem *pFileSystem )
{
	THREAD_SYNCRONIZE( m_lckIntf );

	std::vector< struct KFileSystem* >::iterator it;

	for( it = m_vSourceList.begin(); it != m_vSourceList.end(); ++it )
	{
		if( (*it) != pFileSystem ) continue;

		m_vSourceList.erase( it );
		break;
	}
}

static bool getLocalizedFile( const char *szResource, std::string * strFileName )
{
	const char *szEncoding =  nsl::uni::get_resource_encoding();	/// 2011.06.15 듀얼 클라이언트 설정 - prodongi
	
	if( szEncoding && szEncoding[0] && szResource[0] )
	{
		const char *pDot = strchr( szResource, '.' );
		if( pDot )
		{
			(*strFileName).assign( szResource, pDot );
			(*strFileName) += "(";
			(*strFileName) += szEncoding;
			(*strFileName) += ")";
			(*strFileName) += pDot;
			return true;
		}
	}
	return false;
}

bool KFileManager::IsValidResource( const char *szResourceName )
{
	// { 로컬라이즈
	const char *szFileName = szResourceName;
	std::string strResourceName;
	bool bUseLocale = getLocalizedFile( szResourceName, &strResourceName );
	if( bUseLocale ) szFileName = strResourceName.c_str();
	// }

	{
		THREAD_SYNCRONIZE( m_lckIntf );

		// 파일 열고
		std::vector< struct KFileSystem* >::const_iterator it;
		for( it = m_vSourceList.begin(); it != m_vSourceList.end(); ++it )
		{
		//	if( (*it)->isFile( szResourceName ) ) return true;
			if( (*it)->isFile( szResourceName ) )   // 2011.07.26 - servnates : 코드 가독성 있도록 수정
				return true;

		//	if( szResourceName != szFileName ) if( (*it)->isFile( szFileName ) ) return true;		
			if( szResourceName != szFileName )   // 2011.07.26 - servnates : 코드 가독성 있도록 수정
			{
				if( (*it)->isFile( szFileName ) )
					return true;
			}
		}
	}

	return false;
}

std::string KFileManager::CreateTemporaryFileFromResource( const char *szResourceName )
{
	KStream *pFile = NULL;
	{
		THREAD_SYNCRONIZE( m_lckIntf );

		// 파일 열고
		KFileSystem *pFileSystem = NULL;
		pFile = getFileFromSourceList( pFileSystem, m_vSourceList, szResourceName );
		if( !pFile ) return "";
	}

	// 임시파일명 얻고
	char szPath[512] = { 0, };
	if( !GetTempPath( _countof(szPath), szPath ) ) return std::string( "./" ) + szResourceName;
	std::string strFileName = szPath;
	strFileName += szResourceName;

	// 있던거 지우고
	DeleteFile( strFileName.c_str() );

	// 파일 만들자
	KFileStream stream( strFileName.c_str(), KFileStream::wronly );
	if( !stream.IsValid() ) return "";

	// 메모리에 일단 올리고
	// 2010.04.29 - prodoingi
	size_t size = pFile->getOriginalSize();
	char *pBuffer = new char[size ];
	if( pFile->Read( pBuffer, pFile->Size() ) != size ) assert( 0 );

	// 파일에 쓴 다음
	stream.Write( pBuffer,size );
	/*
	char *pBuffer = new char[ pFile->Size() ];
	if( pFile->Read( pBuffer, pFile->Size() ) != pFile->Size() ) assert( 0 );

	// 파일에 쓴 다음
	stream.Write( pBuffer, pFile->Size() );
	*/

	// 임시자료 다 지우세~
	{
		delete [] pBuffer;
		THREAD_SYNCRONIZE( m_lckIntf );		
		delete pFile;
	}

	return strFileName;
}


// 2010.06.29 - prodongi
char* KFileManager::readData(char const* resourceName)
{
	std::string strResourceName;
	getLocalizedFile( resourceName, &strResourceName );
	KStream *pFile = NULL;
	{
		THREAD_SYNCRONIZE( m_lckIntf );

		// 파일 열고
		KFileSystem *pFileSystem = NULL;
		pFile = getFileFromSourceList( pFileSystem, m_vSourceList, strResourceName.c_str() );
		if( !pFile ) 
		{
			pFile = getFileFromSourceList( pFileSystem, m_vSourceList, resourceName );
			if ( !pFile ) 
			{
				return NULL;
			}
		}
	}

	size_t size = pFile->getOriginalSize();
	char *pBuffer = new char[ size+1 ];
	if( pFile->Read( pBuffer, pFile->Size() ) != size )
	{
		delete[] pBuffer;
		return NULL;
	}
	pBuffer[size] = 0x00;

	THREAD_SYNCRONIZE( m_lckIntf );		
	delete pFile;

	return pBuffer;
}

std::string KFileManager::CreateTemporaryFileFromResourceWithLocale( const char *szResourceName )
{
	std::string strResourceName;
	getLocalizedFile( szResourceName, &strResourceName );
	KStream *pFile = NULL;
	{
		THREAD_SYNCRONIZE( m_lckIntf );

		// 파일 열고
		KFileSystem *pFileSystem = NULL;
		pFile = getFileFromSourceList( pFileSystem, m_vSourceList, strResourceName.c_str() );
		if( !pFile ) 
		{
			pFile = getFileFromSourceList( pFileSystem, m_vSourceList, szResourceName );
			if ( !pFile ) 
			{
				return "";
			}
			strResourceName = szResourceName;
		}
	}

	// 임시파일명 얻고
	char szPath[512] = { 0, };
	if( !GetTempPath( _countof(szPath), szPath ) ) return std::string( "./" ) + strResourceName;
	std::string strFileName = szPath;
	strFileName += strResourceName;

	// 있던거 지우고
	DeleteFile( strFileName.c_str() );

	// 파일 만들자
	KFileStream stream( strFileName.c_str(), KFileStream::wronly );
	if( !stream.IsValid() ) return "";

	// 메모리에 일단 올리고
	// 2010.04.29 - prodongi
	size_t size = pFile->getOriginalSize();
	char *pBuffer = new char[ size ];
	if( pFile->Read( pBuffer, pFile->Size() ) != size ) assert( 0 );

	// 파일에 쓴 다음
	stream.Write( pBuffer, size );

	/*
	char *pBuffer = new char[ pFile->Size() ];
	if( pFile->Read( pBuffer, pFile->Size() ) != pFile->Size() ) assert( 0 );

	// 파일에 쓴 다음
	stream.Write( pBuffer, pFile->Size() );
	*/

	// 임시자료 다 지우세~
	{
		delete [] pBuffer;
		THREAD_SYNCRONIZE( m_lckIntf );		
		delete pFile;
	}

	return strFileName;
}


std::string KFileManager::GetFullPathNameFromResource( const char *szResourceName )
{
	THREAD_SYNCRONIZE( m_lckIntf );

	std::string strString;

	std::vector< struct KFileSystem* >::const_iterator it;
	for( it = m_vSourceList.begin(); it != m_vSourceList.end(); ++it )
	{
		strString = (*it)->getFullPathName( szResourceName );
		if( !strString.empty() ) return strString;
	}

	return "";
}

void KFileManager::RemoveAllStream()
{
	THREAD_SYNCRONIZE( m_lckIntf );

	bool bResult;
	KHash< StreamCache, hashPr_string_nocase >::node* _node = NULL;
	std::vector< std::string > vDeleteList;

	bResult = m_hsStreamCacheByName.get_first_node( _node );
	while( bResult )
	{
		vDeleteList.push_back( _node->key.m_pStr );
		bResult = m_hsStreamCacheByName.get_next_node( _node );
	}

	for( std::vector< std::string >::iterator it = vDeleteList.begin(); it != vDeleteList.end(); ++it )
	{
		StreamCache tmp;
		if( m_hsStreamCacheByName.lookup( *it, tmp ) )
		{
			delete tmp.pStream;
		}

		m_hsStreamCacheByName.erase( *it );
	}
}

void KFileManager::RemoveUnnecessaryStream()
{
	THREAD_SYNCRONIZE( m_lckIntf );

	const DWORD		STREAM_EXPIRE_TIME = 600000;	// 10분 지나면 삭제

	DWORD dwTime = GetSafeTickCount();
	bool bResult;
	KHash< StreamCache, hashPr_string_nocase >::node* _node = NULL;

	std::vector< std::string > vDeleteList;
	
	bResult = m_hsStreamCacheByName.get_first_node( _node );
	while( bResult )
	{
		if( _node->value.dwLastUse + STREAM_EXPIRE_TIME < dwTime ) vDeleteList.push_back( _node->key.m_pStr );
		bResult = m_hsStreamCacheByName.get_next_node( _node );
	}

	for( std::vector< std::string >::iterator it = vDeleteList.begin(); it != vDeleteList.end(); ++it )
	{
		_oprint( "DELETE STREAM CACHE : %s\n", (*it).c_str() );
		StreamCache tmp;
		if( m_hsStreamCacheByName.lookup( *it, tmp ) )
		{
			delete tmp.pStream;
		}
		m_hsStreamCacheByName.erase( *it );
	}
}

class KStream* KFileManager::CreateStreamFromResource( const char *szResourceName, bool bCache )
{
	if( szResourceName[0] == NULL ) return NULL;

	THREAD_SYNCRONIZE( m_lckIntf );

	{
		THREAD_SYNCRONIZE( m_lckCache );	
		StreamCache cache;
		if( m_hsStreamCacheByName.lookup( szResourceName, cache ) ) 
		{	
			cache.dwLastUse = GetSafeTickCount();
			return new KMemoryStream( cache.pStream->GetMappedPtr(0), cache.pStream->GetLength(), true );
		}
	}

	// { 로컬라이즈
	const char *szFileName = szResourceName;
	std::string strResourceName;
	bool bUseLocale = getLocalizedFile( szResourceName, &strResourceName );
	if( bUseLocale ) szFileName = strResourceName.c_str();
	// }

	KFileSystem *pFileSystem = NULL;

	// Search Localized file first, then global file
	KStream *pFile = getFileFromSourceList( pFileSystem, m_vSourceList, szFileName );

	if( !pFile ) 
	{
		if( szFileName != szResourceName && bUseLocale ) pFile = getFileFromSourceList( pFileSystem, m_vSourceList, szResourceName );
		
		if( !pFile ) 
		{
			return NULL;
		}
	}

	/*
	// Search Global file first, then Localized file
	KStream *pFile = getFileFromSourceList( pFileSystem, m_vSourceList, szResourceName );

	if( !pFile ) 
	{
		if( szFileName != szResourceName && bUseLocale ) pFile = getFileFromSourceList( pFileSystem, m_vSourceList, szFileName );
		
		if( !pFile ) 
		{
			return NULL;
		}
	}
	*/

	// 2010.04.29 - prodongi
	KMemoryStream* pStream = new KMemoryStream( pFile->getOriginalSize() );
	//KMemoryStream* pStream = new KMemoryStream( pFile->Size() );
	pFile->Read( pStream->GetMappedPtr( KStream::MAPPED_WRITE ), pFile->Size() );
	delete pFile;

	if( bCache )
	{
		THREAD_SYNCRONIZE( m_lckCache );
		m_hsStreamCacheByName.add( szResourceName, StreamCache( pStream, GetSafeTickCount() ) );
	
		pStream = new KMemoryStream( pStream->GetMappedPtr(0), pStream->GetLength(), true );
	}

	return pStream;
}

bool KFileManager::DeleteTemporaryFile( const char *szResourceName )
{
	return false;
}

bool KFileManager::DeleteStream( class KStream* pStream )
{
	if( !pStream ) return false;

	delete pStream;
	return true;
}

void KFileManager::EraseStream( std::string* pStreamName )
{
	StreamCache tmp;
	if( m_hsStreamCacheByName.lookup( *pStreamName, tmp ) )
	{
		delete tmp.pStream;
		m_hsStreamCacheByName.erase( *pStreamName );
	}
}

/// @brief 2010.04.29 - prodongi
size_t KFileManager::checkXZipBufferAlloc(void* src, size_t srcSize)
{	
	size_t originalSize = XZip::GetOriginalSize(src, srcSize);

	if (!m_xzipBuffer)
	{
		m_xzipBuffer = (char*)malloc(originalSize);
		m_xzipBufferSize = originalSize;
	}
	else if (originalSize > m_xzipBufferSize)
	{
		char* temp = (char*)realloc(m_xzipBuffer, originalSize);
		if( temp != NULL )
		{
			m_xzipBuffer = temp;
			m_xzipBufferSize = originalSize;
		}

		return m_xzipBufferSize;
	}
	return originalSize;
}