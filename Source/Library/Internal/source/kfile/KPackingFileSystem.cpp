
#include "../../include/kfile/KPackingFileSystem.h"
#include "../../include/toolkit/khash.h"
#include "../../include/dump/XException.h"
#include <stdlib.h>
#include <limits>
#include "../../include/kfile/XOREn.h"
#include <algorithm>
#include <errno.h>
#include "../../include/toolkit/nsl.h"
#include "../../include/toolkit/safe_function.h"
#include "../../include/kfile/JStringLib.h"


// 2010.04.29 - prodongi
#include "../../include/kfile/KFileManager.h"
#include "../../include/compress/XZip.h"


void addToCorruptedBlock( const char *szFileName )
{
	FILE *fp = NULL;
	fopen_s( &fp, "corrupted.txt", "a" );
	if ( fp )
	{
		fprintf( fp, "%s\n", szFileName );
		fclose( fp );
	}
}

//////////////////////////////////////////////////////////////////////////

/*
 *	KPackingFreeBockManager
 */
struct KPackingFreeBlockManager
{
public:
	KPackingFreeBlockManager()	{}
	~KPackingFreeBlockManager() {	Free();	}

	void AddFreeBlock( size_t pos, size_t size, bool bIsReuseable )
	{
		if ( bIsReuseable ) m_vecFreeBlockList.push_back( KFreeBlockInfo( pos, size ) );
		else m_vecDeletedBlockList.push_back( KFreeBlockInfo( pos, size ) );

		CalculateOptimalBlock();
	}

	size_t ReserveFreeBlock( size_t size );

	void CalculateOptimalBlock( bool bAllList = false );
	void Free() { m_vecFreeBlockList.clear(); }
	void SetBlockEnd( size_t size )
	{
		std::vector<KFreeBlockInfo>::iterator it		= m_vecFreeBlockList.begin();

		for ( ;it != m_vecFreeBlockList.end(); ++it )
		{
			if ( (*it).m_offset >= size )
			{
				m_vecFreeBlockList.erase( it, m_vecFreeBlockList.end() );
				return;
			}
		}
	}

	size_t	GetFreeBlockSize()	const
	{
		size_t total_size = 0;
		std::vector<KFreeBlockInfo>::const_iterator it = m_vecFreeBlockList.begin();
		std::vector<KFreeBlockInfo>::const_iterator end = m_vecFreeBlockList.end();
		for ( ;it != end; ++it )
		{
			total_size += (*it).m_size;
		}

		return total_size;
	}

	struct KFreeBlockInfo
	{
		KFreeBlockInfo( size_t offset, size_t size ) : m_offset(offset), m_size(size) {};

		bool operator<( const KFreeBlockInfo & rh ) const
		{
			return m_offset < rh.m_offset;
		}
		
		size_t m_offset;
		size_t m_size;
	};

	std::vector<KFreeBlockInfo>	m_vecFreeBlockList;
	std::vector<KFreeBlockInfo>	m_vecDeletedBlockList;
};

size_t KPackingFreeBlockManager::ReserveFreeBlock( size_t size )
{
	std::vector<KFreeBlockInfo>::iterator min_pos	= m_vecFreeBlockList.end();
	std::vector<KFreeBlockInfo>::iterator it		= m_vecFreeBlockList.begin();

	for ( ;it != m_vecFreeBlockList.end(); ++it )
	{
		KFreeBlockInfo info = (*it);

		// 일치하는 경우
		if ( info.m_size == size )
		{
			m_vecFreeBlockList.erase(it);
			return info.m_offset;
		}

		if ( info.m_size < size ) continue;
		
		if ( min_pos == m_vecFreeBlockList.end() ) 
		{
			min_pos = it;
			continue;
		}

		if ( (*min_pos).m_size > info.m_size )
		{
			min_pos = it;
		}
	}

	if ( min_pos == m_vecFreeBlockList.end() ) 
	{
		return (std::numeric_limits< size_t >::max)();
	}

	size_t pos = (*min_pos).m_offset;

	(*min_pos).m_offset += size;
	(*min_pos).m_size -= size;

	return pos;
}

/*
 *	join blocks
 */
void KPackingFreeBlockManager::CalculateOptimalBlock( bool bAllList )
{
	if ( m_vecFreeBlockList.empty() )	return;
	
	// offset 별로 정렬
	std::sort( m_vecFreeBlockList.begin(), m_vecFreeBlockList.end() );

	// size summary
	std::vector< KFreeBlockInfo >::iterator it;
	std::vector< KFreeBlockInfo >::iterator next;
	for ( it = m_vecFreeBlockList.begin(); it != m_vecFreeBlockList.end(); )
	{
		if ( &(*it) == &m_vecFreeBlockList.back() ) break;

		next = it + 1;

		if ( (*it).m_offset + (*it).m_size > (*next).m_offset )
		{
			assert( 0 );
			MessageBox( NULL, "Packing error! (#3)", "Error", MB_OK | MB_ICONERROR );
			return;
		}

		if ( (*it).m_offset + (*it).m_size == (*next).m_offset )
		{
			(*it).m_size += (*next).m_size;
			it = m_vecFreeBlockList.erase( next );
			if ( it != m_vecFreeBlockList.begin() ) --it;
			continue;
		}

		++it;
	}
}
//////////////////////////////////////////////////////////////////////////

/*
 *	KPackingFile
 */
struct KPackingFile : public KStream
{
public:

	KPackingFile( KPackingFileSystem* pParent, KHFILE fp, const char *szFileName, size_t pos, size_t size, bool bIsReadOnly = true ) 
		: m_pParent( pParent )
		, m_pfp( fp )
		, m_strFileName( szFileName )
		, m_Offset( pos )
		, m_Size( size )
		, m_pBuf( NULL )
		, m_BufSize( 0 )
		, m_bIsClosed( false )
		, m_bIsReadOnly( bIsReadOnly )
		, m_CurPos( 0 )
		, m_hFM( NULL )
		, m_pMap( NULL )
	{
		// 2010.04.29 - prodongi
		setClassId("KPackingFile");
	}

	~KPackingFile()	
	{
		if ( m_pParent && m_bIsClosed == false )
			m_pParent->close( this );

		if ( m_pMap )
		{
			UnmapViewOfFile( m_pMap );
			m_pMap = NULL;
		}
		if ( m_hFM )
		{
			CloseHandle( m_hFM );
			m_hFM = NULL;
		}

		if ( m_pBuf )
		{
			delete [] m_pBuf;
			m_pBuf = NULL;
		}
	}

	bool init()
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);

		m_dwGranularity = m_Offset % si.dwAllocationGranularity;
		m_hFM = ::CreateFileMapping( m_pfp, NULL, PAGE_READONLY, 0, 0, NULL );
		if( m_hFM == NULL )
		{
			/*
			char buf[1024] = "";
			DWORD last_error = ::GetLastError();
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
			char szErr[1024] = { 0, };
			s_sprintf( szErr, _countof( szErr ), "%s(%d)", buf, last_error );

			MessageBox( NULL, szErr, "Rappelz", MB_OK | MB_ICONERROR );
			return false;
			*/
		}

		m_pMap = static_cast<LPBYTE>( MapViewOfFile( m_hFM, FILE_MAP_READ, 0, static_cast< DWORD >( m_Offset ) - m_dwGranularity, m_Size + m_dwGranularity) );
		if( m_pMap == NULL )
		{
			/*
			char buf[1024] = "";
			DWORD last_error = ::GetLastError();
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
			char szErr[1024] = { 0, };
			s_sprintf( szErr, _countof( szErr ), "%s(%d)", buf, last_error );

			MessageBox( NULL, szErr, "Rappelz", MB_OK | MB_ICONERROR );
			return false;
			*/
		}

		return true;
	}

	virtual size_t Read( void *pBuf, size_t read_size );
	virtual size_t Write( const void *pBuf, size_t write_size );
	virtual size_t Seek( long offset, enum_seek_origin origin );
	virtual size_t Tell() const;
	virtual size_t GetLength() const { return getOriginalSize( m_Size ); };
	virtual bool IsValid() const									{ return IsValidKHFile( m_pfp ); }

	char* GetBuffer() const { return m_pBuf; }
	const char* GetFileName() const { return m_strFileName.c_str(); }
	void SetOffset( size_t offset )	{ m_Offset = offset; }
	const bool IsReadOnly() const	{ return m_bIsReadOnly; }
	void  SetCloseFlag( bool fFlag)  { m_bIsClosed = fFlag; }
	const bool GetCloseFlag() const { return m_bIsClosed; }

	// 2010.04.29 - prodongi
	virtual size_t getOriginalSize(size_t srcSize) const
	{
		if (!m_pMap) 
			return 0;
		size_t size;
		LPBYTE pMap = m_pMap + 0 + m_dwGranularity;
		if (srcSize >= KFileSystem::MIN_COMPRESS_FILE_SIZE && XZip::IsCompressed(pMap, srcSize))
		{
			size = XZip::GetOriginalSize(pMap, srcSize);
		}
		else
			size = m_Size;

		return size;
	}
	virtual size_t getOriginalSize() const
	{
		return getOriginalSize(Size());
	}

private:

	KPackingFileSystem* m_pParent;
	KHFILE m_pfp;

	std::string m_strFileName;

	bool   m_bIsClosed;
	char*  m_pBuf;
	size_t m_BufSize;

	bool   m_bIsReadOnly;
	size_t m_Size;
	size_t m_Offset;
	size_t m_CurPos;

	HANDLE m_hFM;
	LPBYTE m_pMap;
	DWORD m_dwGranularity;

};

size_t KPackingFile::Read( void *pBuf, size_t read_size )
{
	if ( !IsValid() )
		return 0;

	if ( m_CurPos + read_size > m_Size )
		read_size = m_Size - m_CurPos;

	if ( read_size == 0 )	return 0;

	/*
	DWORD dwRead;
	SetFilePointer( m_pfp, static_cast<long>(m_Offset+m_CurPos), NULL, FILE_BEGIN );
	if ( ReadFile( m_pfp, pBuf, read_size, &dwRead, NULL ) )
	{
		m_CurPos += dwRead;
		return dwRead;
	}
	*/
	if ( !m_pMap )
		return 0;

	// 2010.04.29 xzip uncompress test - prodongi
	/*
	read_size : 압축된 사이즈
	*/
	size_t originalSize;
	LPBYTE pMap = m_pMap + m_CurPos + m_dwGranularity;
	if (read_size >= KFileSystem::MIN_COMPRESS_FILE_SIZE && XZip::IsCompressed(pMap, read_size))
	{
		originalSize = KFileManager::Instance().checkXZipBufferAlloc(pMap, read_size);
		bool ret = XZip::Uncompress(pMap, read_size, KFileManager::Instance().m_xzipBuffer, originalSize);
		if (!ret)
		{
			assert(0 && "failed uncompress");
			return 0;
		}
		s_memcpy(pBuf, read_size, KFileManager::Instance().m_xzipBuffer, originalSize);
	}
	else
	{
		originalSize = read_size;
		s_memcpy( pBuf, read_size, m_pMap + m_CurPos + m_dwGranularity, read_size );
	}
	//s_memcpy( pBuf, read_size, m_pMap + m_CurPos + m_dwGranularity, read_size );

	m_CurPos += read_size;
	// 2010.04.29 - prodongi
	return originalSize;
	//return read_size;
}

size_t KPackingFile::Write( const void *pBuf, size_t write_size )
{
	if ( m_bIsReadOnly || !IsValid() ) return 0;

	if ( m_CurPos + write_size > m_BufSize )
	{
		char *pNewBuf = new char[ m_BufSize + write_size ];
		m_BufSize = m_BufSize + write_size;

		if ( m_pBuf )
		{
			s_memcpy( pNewBuf, m_BufSize, m_pBuf, m_Size );
			delete [] m_pBuf;			
		}

		m_pBuf = pNewBuf;
	}

	s_memcpy( &m_pBuf[ m_CurPos ], m_BufSize-m_CurPos, pBuf, write_size );
	m_CurPos += write_size;

	if ( m_CurPos > m_Size ) m_Size = m_CurPos;

	return write_size;
}

size_t KPackingFile::Seek( long offset, enum_seek_origin origin )
{
	if ( !IsValid() ) return static_cast< size_t >( -1 );

	switch(origin)
	{
	case seekSet:
		m_CurPos = offset;
		break;
	case seekCur:
		m_CurPos += offset;
		break;
	case seekEnd:
		m_CurPos = m_Size;
		break;
	}

	if ( m_CurPos < 0 ) m_CurPos = 0;
	if ( m_CurPos > m_Size ) m_CurPos = m_Size;

	return m_CurPos;
}

size_t KPackingFile::Tell() const
{
	if ( m_pfp == NULL )	return 0;

	return m_CurPos;
}

//////////////////////////////////////////////////////////////////////////
// KPackingFileSystem
//////////////////////////////////////////////////////////////////////////


bool KPackingFileSystem::KFileInfo::saveFileInfo( KHFILE fp, int* seed ) const
{
	bool bRtn = true;
	unsigned char file_name_len = (unsigned char)strFileName.size();
	unsigned int total_size = sizeof(size_t)*2 + file_name_len + 1;
	unsigned char *pBuf = new unsigned char[ total_size ];
	unsigned char *p = pBuf;
	int remain_buf_size = total_size;
	s_memcpy( p, remain_buf_size, &file_name_len, 1 );						p += 1; remain_buf_size -= 1;
	s_memcpy( p, remain_buf_size, strFileName.c_str(), file_name_len );		p += file_name_len; remain_buf_size -= file_name_len;
	s_memcpy( p, remain_buf_size, &m_Offset, sizeof(size_t) );				p += sizeof(size_t); remain_buf_size -= sizeof( size_t );
	s_memcpy( p, remain_buf_size, &m_Size, sizeof(size_t) );				p += sizeof(size_t); remain_buf_size -= sizeof( size_t );
	for ( unsigned int i = 0; i < total_size; ++i )
	{
		pBuf[i] ^= XOREn::GetEncodeKeyChar( (*seed)++ );
	}

	DWORD dwWrite;
	if ( WriteFile( fp, pBuf, total_size, &dwWrite, NULL ) )
		bRtn = ( dwWrite == total_size );
	else
		bRtn = false;

	delete [] pBuf;

	return bRtn;
}


bool KPackingFileSystem::KFileInfo::loadFileInfo( KHFILE fp, int* seed, int nDataFileCount )
{
	unsigned char file_name_len = 0;
	DWORD dwRead;
	if( !ReadFile( fp, &file_name_len, 1, &dwRead, NULL ) )
		return false;	

	file_name_len ^= XOREn::GetEncodeKeyChar( (*seed)++ );

	unsigned char szName[MAX_PATH];
	if( !ReadFile( fp, szName, file_name_len, &dwRead, NULL ) || ( file_name_len != dwRead ) )
		return false;

	int i = 0;
	for( i = 0; i < file_name_len; ++i )
	{
		szName[i] ^= XOREn::GetEncodeKeyChar( (*seed)++ );
	}
	szName[i] = 0;

	size_t offset_size[2] = {0,};
	unsigned char *p = (unsigned char *)offset_size;

	if ( !ReadFile( fp, offset_size, sizeof(size_t) * 2, &dwRead, NULL ) || ( sizeof(size_t) * 2 != dwRead ) )
		return false;

	for ( int i = 0; i < sizeof(size_t)*2; ++i )
	{
		*(p++) ^= XOREn::GetEncodeKeyChar( (*seed)++ );
	}

	strFileName = reinterpret_cast< const char* >( szName );
	m_Offset = offset_size[0];
	m_Size = offset_size[1];
	m_Index = static_cast< unsigned short >( KPackingFileSystem::getFileHashKey( (const char*)szName, nDataFileCount ) );

	return true;
}

KPackingFileSystem::KPackingFileSystem( int nDataFileCount, int nMaxThreadCount, bool bIsReadOnly ) : m_bIsReadOnly( bIsReadOnly )
{
	m_nDataFileCount = nDataFileCount;
	m_nMaxThreadCount = nMaxThreadCount;

	m_nDataFileSize = new size_t[m_nDataFileCount];

	for ( size_t i = 0; i < m_nDataFileCount; ++i )
	{
		m_nDataFileSize[i] = 0;
	}

	m_hsIndexFileList.clear();

	m_pFreeBlockManager = NULL;
}

KPackingFileSystem::~KPackingFileSystem()
{
	// legacy 코드와의 하위호환성을 위해 소멸자에서는 save 파일을 저장하지 않는다. 
	// by Testors
	DeInit( false );

	//DeInit();

	if( m_pFreeBlockManager != NULL )
	{
		delete [] m_pFreeBlockManager;
		m_pFreeBlockManager = NULL;
	}

	delete [] m_nDataFileSize;
}

bool KPackingFileSystem::Init( const char* szIndexFileName, const char* szDataFileName )
{
	m_strIndexFileName = szIndexFileName;
	m_strDataFileName = szDataFileName;

	bool bFullIndexPath = JStringLib::IsFullPath( m_strIndexFileName.c_str() );
	bool bFullDataPath = JStringLib::IsFullPath( m_strDataFileName.c_str() );

	std::string strDirectory = m_strIndexFileName;
	JStringLib::GetFilePathFromFile( strDirectory );
	m_strCurDirectory = strDirectory;

	if ( bFullIndexPath )	JStringLib::GetFileNameFormPath( m_strIndexFileName );
	if ( bFullDataPath )	JStringLib::GetFileNameFormPath( m_strDataFileName );

	// load index, del_list file
	if ( !LoadIndexFile() ) return false;
	CalcurateFreeBlock();

	return true;
}

KPackingFileSystem::KFileHandleMap::~KFileHandleMap()
{
	for ( int i = 0; i < maxHandleCount; ++i )
	{
		if ( handle[i] ) 
		{
			::FlushFileBuffers( handle[i] );
			::CloseHandle( handle[i] );
		}
	}

	delete [] handle;
}

bool KPackingFileSystem::DeInit( bool bSaveIndexFile )
{
	if( bSaveIndexFile )
	{
		if ( !SaveIndexFile() ) return false;
	}
	
	if ( m_hsIndexFileList.size() > 0 )	m_hsIndexFileList.clear();

	{
		THREAD_SYNCRONIZE( m_lckDataFileHandle );
		for ( std::vector< KFileHandleMap* >::iterator it = m_vecFileHandle.begin(); it != m_vecFileHandle.end(); ++it )
			delete *it;

		m_vecFileHandle.clear();
	}

	return true;
}

bool KPackingFileSystem::Flush( bool bSaveIndexFile )
{
	bool ret = false;
	if( bSaveIndexFile == true )
	{
		ret = SaveIndexFile();
	}

	{
		THREAD_SYNCRONIZE( m_lckDataFileHandle );
		for ( std::vector< KFileHandleMap* >::iterator it = m_vecFileHandle.begin(); it != m_vecFileHandle.end(); ++it )
			delete *it;

		m_vecFileHandle.clear();
	}

	return ret;
}

void KPackingFileSystem::doEachFile( FileHandler & handler )
{
	THREAD_SYNCRONIZE( m_lckDataFileHandle );
	KHash< KFileInfo, hashPr_string_nocase >::node* _node = NULL;
	
	bool bResult = m_hsIndexFileList.get_first_node( _node );
	while ( bResult )
	{
		if ( !handler.onFile( _node->key.m_pStr ) ) break;

		bResult = m_hsIndexFileList.get_next_node( _node );
	}
}

KHFILE KPackingFileSystem::getDataFileHandle( const char *szFileName )
{
	THREAD_SYNCRONIZE( m_lckDataFileHandle );

	KHFILE fp = NULL;

	int thread_id = GetCurrentThreadId();

	std::vector< KFileHandleMap* >::iterator it;
	for ( it = m_vecFileHandle.begin(); it != m_vecFileHandle.end(); ++it )
	{
		if ( (*it)->threadId == thread_id )
		{
			KFileHandleMap* h = *it;
			m_vecFileHandle.erase( it );
			m_vecFileHandle.push_back( h );
			it = m_vecFileHandle.end()-1;
			break;
		}
	}

	if ( it == m_vecFileHandle.end() )
	{
		m_vecFileHandle.push_back( new KFileHandleMap( static_cast< int >( m_nDataFileCount ), thread_id ) );

		while ( m_vecFileHandle.size() > m_nMaxThreadCount )
		{
			delete m_vecFileHandle.front();
			m_vecFileHandle.erase( m_vecFileHandle.begin() );
		}
	}

	size_t hash_key = getFileHashKey( szFileName, m_nDataFileCount );

	KFileHandleMap* p = m_vecFileHandle.back();
	if ( p->handle[ hash_key ] != NULL ) 
		return p->handle[ hash_key ];

	std::string strFullPath = m_strCurDirectory + getDataFileName( m_strDataFileName.c_str(), hash_key );

#ifdef _DEBUG
	OutputDebugString( strFullPath.c_str() );
	OutputDebugString( "\n" );
#endif

	
	DWORD dwSwitch = GENERIC_READ | (m_bIsReadOnly ? 0 : GENERIC_WRITE );
	fp = ::CreateFile( strFullPath.c_str(), dwSwitch, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

	if ( !IsValidKHFile( fp ) )
	{
		if( m_bIsReadOnly == false )
		{
			fp = ::CreateFile( strFullPath.c_str(), dwSwitch, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
		}
	}

	if( !IsValidKHFile( fp ) )
	{
		DWORD last_error = GetLastError();
		char buf[1024] = "";
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
		char szErr[1024] = { 0, };
		s_sprintf( szErr, _countof( szErr ), "%s(%d)", buf, last_error );
		MessageBox( NULL, buf, "Rappelz", MB_OK | MB_ICONERROR );

		assert( false );
		return NULL;
	}

	p->handle[ hash_key ] = fp;

	CalcurateDataFileSize();

	return fp;
}

KStream* KPackingFileSystem::open( const char* szFileName, ACCESS_MODE access_mode )
{
	KHFILE fp = getDataFileHandle( szFileName );
	if ( !IsValidKHFile( fp ) )
	{
		return NULL;
	}

	std::string strName = szFileName;

	bool bIsReadOnly = false;
	if ( access_mode == KPackingFileSystem::READ_ONLY ) bIsReadOnly = true;

	KFileInfo info;
	if ( m_hsIndexFileList.lookup( strName.c_str(), info ) )
	{
		if ( !bIsReadOnly )
		{
			assert( 0 );
		}

		KPackingFile* pFile = new KPackingFile( this, fp, strName.c_str(), info.m_Offset, info.m_Size, bIsReadOnly );
		if( pFile != NULL )
		{
			if( pFile->init() == false )
			{
				delete pFile;
				pFile = NULL;
			}
		}

		return pFile;
	}
	else
	{
		// 쓰고 있는 파일을 또 열려 했을경우 NULL
		if ( m_hsIndexWritingFileList.has( strName ) )
		{
			return NULL;
		}

		if ( bIsReadOnly )
		{
			return NULL;
		}
		
		KPackingFile *pFile = new KPackingFile( this, fp, strName.c_str(), 0, 0, false );
		if( pFile != NULL )
		{
			if( pFile->init() == false )
			{
				delete pFile;
				pFile = NULL;
			}
			else
			{
				m_hsIndexWritingFileList.add( strName, pFile );
			}
		}

		return pFile;
	}
}

void KPackingFileSystem::CalcurateDataFileSize()
{
	for ( size_t i = 0; i < m_nDataFileCount; ++i )	m_nDataFileSize[i] = 0;
	bool bResult;
	KHash< KFileInfo, hashPr_string_nocase >::node* _node = NULL;
	
	bResult = m_hsIndexFileList.get_first_node( _node );
	while ( bResult )
	{
		if ( _node->value.m_Offset + _node->value.m_Size > m_nDataFileSize[ _node->value.m_Index ] )
		{
			m_nDataFileSize[ _node->value.m_Index ] = _node->value.m_Offset + _node->value.m_Size;
		}

		bResult = m_hsIndexFileList.get_next_node( _node );
	}

	for ( size_t i = 0; i < m_nDataFileCount; ++i )
	{
		m_pFreeBlockManager[i].SetBlockEnd( m_nDataFileSize[i] );
	}

	//m_nDataFileSize = nDataFileSize;
}

bool KPackingFileSystem::close( KStream *pStream )
{
	KPackingFile *pFile = static_cast< KPackingFile * >( pStream );

	if ( !pFile ) 
	{
		return false;
	}

	if( close( pFile, pFile->GetBuffer(), pFile->Size() ) == false )
	{
		return false;
	}

	return true;
}

bool	KPackingFileSystem::close( class KStream* pStream, const void* pBuffer, size_t size )
{
	KPackingFile *pFile = static_cast< KPackingFile * >( pStream );

	if ( !pFile ) 
	{
		return false;
	}

	if ( pFile->IsReadOnly() )
	{
		return true;
	}

	m_hsIndexWritingFileList.erase( pFile->GetFileName() );

	// 0바이트는 쓰지 말자
	if( size == 0 ) 
	{
		return true;
	}

	size_t hash_key = getFileHashKey( pFile->GetFileName(), m_nDataFileCount );

	KHFILE fp = getDataFileHandle( pFile->GetFileName() );
	if ( !IsValidKHFile( fp ) ) 
	{
		return false;
	}

	// 일단 기존 파일은 지운다
	deleteFile( pFile->GetFileName() );

	pFile->SetCloseFlag( true );

	// 빈블럭 있으면 빈블럭에 삽입
	size_t free_pos = m_pFreeBlockManager[hash_key].ReserveFreeBlock( size );
	if ( free_pos != (std::numeric_limits< size_t >::max)() )
	{
		pFile->SetOffset( free_pos );
		DWORD dwRet = SetFilePointer( fp, static_cast< LONG >( free_pos ), NULL, FILE_BEGIN );
		if( dwRet == INVALID_SET_FILE_POINTER )
		{
			DWORD last_error = ::GetLastError();
			char buf[1024] = "";
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
			char szErr[1024] = { 0, };
			s_sprintf( szErr, _countof( szErr ), "%s(%d)", buf, last_error );
			MessageBox( NULL, buf, "Rappelz", MB_OK | MB_ICONERROR );

			return false;
		}

		DWORD dwWrite;
		if ( WriteFile( fp, pBuffer, static_cast< DWORD >( size ), &dwWrite, NULL ) == FALSE )
		{
			DWORD last_error = ::GetLastError();
			char buf[1024] = "";
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
			char szErr[1024] = { 0, };
			s_sprintf( szErr, _countof( szErr ), "%s(%d)", buf, last_error );
			MessageBox( NULL, buf, "Rappelz", MB_OK | MB_ICONERROR );

			return false;
		}

		KFileInfo info( pFile->GetFileName(), size, free_pos, static_cast< unsigned short >( hash_key ) );
		m_hsIndexFileList.add( pFile->GetFileName(), info );
		return true;
	}

	// 맨 뒤에 삽입
	KFileInfo info( pFile->GetFileName(), size, m_nDataFileSize[hash_key], static_cast< unsigned short >( hash_key ) );
	m_hsIndexFileList.add( pFile->GetFileName(), info );

	// { FAT32 에서는 4G 이상의 파일은 만들 수 없다.
	__int64 data_size = m_nDataFileSize[hash_key];
	data_size += size;
	if ( (__int64)(std::numeric_limits< DWORD >::max)() < data_size )
	{
		return false;
	}
	// }

	DWORD dwRet = SetFilePointer( fp, static_cast< LONG >( m_nDataFileSize[hash_key] ), NULL, FILE_BEGIN );
	if( dwRet == INVALID_SET_FILE_POINTER )
	{
		DWORD last_error = ::GetLastError();
		char buf[1024] = "";
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
		char szErr[1024] = { 0, };
		s_sprintf( szErr, _countof( szErr ), "%s(%d)", buf, last_error );
		MessageBox( NULL, buf, "Rappelz", MB_OK | MB_ICONERROR );

		return false;
	}

	size_t write_size = size;
	const char* write_buffer = static_cast< const char* >( pBuffer );
	while( write_size > 0 )
	{
		DWORD request_size = write_size;
		if( request_size > JStringLib::MAX_FILE_WRITE_SIZE )
		{
			request_size = JStringLib::MAX_FILE_WRITE_SIZE;
		}

		DWORD written = 0;
		if( WriteFile( fp, write_buffer, request_size, &written, NULL ) == FALSE )
		{
			DWORD last_error = ::GetLastError();
			char buf[1024] = "";
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
			char szErr[1024] = { 0, };
			s_sprintf( szErr, _countof( szErr ), "%s(%d)", buf, last_error );
			MessageBox( NULL, buf, "Rappelz", MB_OK | MB_ICONERROR );

			return false;
		}

		//버퍼 이동
		write_buffer += written;
		if( write_size > written )
		{
			write_size -= written;
		}
		else
		{
			assert( write_size == written );
			write_size = 0;
		}
	}

	FlushFileBuffers( fp );

	CalcurateDataFileSize();
	return true;
}

size_t KPackingFileSystem::getFileSize( const char* szFileName ) const
{
	KFileInfo info;
	if ( m_hsIndexFileList.lookup( szFileName, info ) )	return info.m_Size;
	return 0;
}

unsigned __int64	KPackingFileSystem::getDataFileSize()
{
	unsigned __int64 total_size = 0;
	KHash< KFileInfo, hashPr_string_nocase >::node* _node = NULL;
	bool bResult = m_hsIndexFileList.get_first_node( _node );
	while ( bResult )
	{
		total_size += _node->value.m_Size;
		bResult = m_hsIndexFileList.get_next_node( _node );
	}

	return total_size;
}

unsigned __int64	KPackingFileSystem::getFreeSize() const
{
	unsigned __int64 total_free_size = 0;

	if( m_pFreeBlockManager != NULL )
	{
		for ( size_t i = 0; i < m_nDataFileCount; ++i )
		{
			total_free_size += m_pFreeBlockManager[i].GetFreeBlockSize();
		}
	}

	return total_free_size;
}

bool KPackingFileSystem::changeDirectory( const char *szDirectory )
{
	return true;
}

bool KPackingFileSystem::makeDirectory( const char *szDirectory )
{
	m_strCurDirectory = szDirectory;
	return !!(::CreateDirectory( szDirectory, NULL ));
}

bool KPackingFileSystem::deleteDirectory( const char *szDirectory )
{
	// 폴더에 있는 기존 파일 모두 삭제
	std::string strFile = szDirectory;
	std::string strTemp = strFile.substr( strFile.size()-1, 1 );
	if ( ::_stricmp( strTemp.c_str(), "/" ) != 0 )
		strFile += "/";
	strFile += "*.*";

	WIN32_FIND_DATA data;
	HANDLE hSearch = FindFirstFile( strFile.c_str(), &data );
	if ( hSearch == INVALID_HANDLE_VALUE )
		return !!(RemoveDirectory( szDirectory ));

	bool bResult = true;
	while ( bResult )
	{
		// hash 에서 삭제
		KFileInfo info;
		if ( m_hsIndexFileList.lookup(data.cFileName, info) )
			m_hsIndexFileList.erase(data.cFileName);

		bResult = (FindNextFile( hSearch, &data ) != FALSE);
	}

	return !!(RemoveDirectory( szDirectory ));
}

bool KPackingFileSystem::deleteFile( const char *szFile )
{
	KFileInfo info;
	if ( m_hsIndexFileList.lookup( szFile, info ) )
	{
		// add del_list
		m_pFreeBlockManager[getFileHashKey( szFile, m_nDataFileCount )].AddFreeBlock( info.m_Offset, info.m_Size, false );
		
		// delete in index list 
		m_hsIndexFileList.erase( szFile );

		std::string strFullPath = m_strCurDirectory + szFile;
		JStringLib::ReplaceString( strFullPath, "/", "\\" );

		return true;
	}

	return false;
}

/*
 *	load index file
 */
bool KPackingFileSystem::LoadIndexFile()
{
	std::string strIndexFile = m_strCurDirectory + m_strIndexFileName;

	KHFILE pFile;
	pFile = ::CreateFile( strIndexFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( !IsValidKHFile( pFile ) )
	{
		if( m_bIsReadOnly == false )
		{
			return true;
		}

		char buf[1024] = "";
		DWORD last_error = ::GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
		char szErr[1024] = { 0, };
		s_sprintf( szErr, _countof( szErr ), "%s(%d) - %s", buf, last_error, strIndexFile.c_str() );

		MessageBox( NULL, szErr, "Rappelz", MB_OK | MB_ICONERROR );

		return false;
	}
	
	int seed = 0;

	while ( SetFilePointer( pFile, 0, NULL, FILE_CURRENT ) < GetFileSize( pFile, NULL ) )
	{
		KFileInfo info;
		if( info.loadFileInfo( pFile, &seed, m_nDataFileCount ) == false )
		{
			break;
		}

		m_hsIndexFileList.add( info.strFileName, info );
	}

	CloseHandle( pFile );
	return true;
}
/*
 *	load del_list file
 */
void KPackingFileSystem::CalcurateFreeBlock()
{
	if( m_pFreeBlockManager != NULL )
	{
		delete [] m_pFreeBlockManager;
		m_pFreeBlockManager = NULL;
	}

	m_pFreeBlockManager = new KPackingFreeBlockManager[m_nDataFileCount];

	bool bResult;
	KHash< KFileInfo, hashPr_string_nocase >::node* _node = NULL;
	std::vector< KFileInfo > *vFileList = new std::vector< KFileInfo >[m_nDataFileCount];

	// 파일 리스트를 벡터에 복사
	bResult = m_hsIndexFileList.get_first_node( _node );
	while ( bResult )
	{
		vFileList[_node->value.m_Index].push_back( _node->value );
		bResult = m_hsIndexFileList.get_next_node( _node );
	}

	for ( size_t i = 0; i < m_nDataFileCount; ++i )
	{
		// 시작위치 기준으로 정렬
		std::sort( vFileList[i].begin(), vFileList[i].end() );

		// 빈 공간 찾아서 KFreeBlockManager 에 삽입
		std::vector< KFileInfo >::iterator it;
		std::vector< KFileInfo >::iterator next;
		for ( it = vFileList[i].begin(); it != vFileList[i].end(); ++it )
		{
			if ( it == vFileList[i].begin() )
			{
				if ( (*it).m_Offset != 0 ) m_pFreeBlockManager[i].AddFreeBlock( 0, (*it).m_Offset, true );
			}

			next = it + 1;

			if ( next == vFileList[i].end() ) break;

			if ( (*it).m_Offset + (*it).m_Size > (*next).m_Offset )
			{
				addToCorruptedBlock( (*it).strFileName.c_str() );
				addToCorruptedBlock( (*next).strFileName.c_str() );
			}

			size_t offset = (*it).m_Offset + (*it).m_Size;		

			if ( offset < (*next).m_Offset ) m_pFreeBlockManager[i].AddFreeBlock( offset, (*next).m_Offset - offset, true );
		}
	}

	delete [] vFileList;
}

#include <windows.h>

/*
 *	save index file
 */
bool KPackingFileSystem::SaveIndexFile() 
{
	const std::string strIndexFile = m_strCurDirectory + m_strIndexFileName;
	const std::string strOldIndexFile = m_strCurDirectory + m_strIndexFileName + ".old";
	const std::string strNewIndexFile = m_strCurDirectory + m_strIndexFileName + ".new";
	
	if ( m_hsIndexFileList.size() <= 0 )	return false;

	SetFileAttributes( strNewIndexFile.c_str(), FILE_ATTRIBUTE_NORMAL );
	DeleteFile( strNewIndexFile.c_str() );

	KHFILE pFile;
	pFile = ::CreateFile( strNewIndexFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( !IsValidKHFile( pFile) )	
	{
		char buf[1024] = "";
		DWORD last_error = ::GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
		char szErr[1024] = { 0, };
		s_sprintf( szErr, _countof( szErr ), "%s(%d) - %s", buf, last_error, strNewIndexFile.c_str() );

		MessageBox( NULL, szErr, "Rappelz", MB_OK | MB_ICONERROR );

		assert( 0 );
		return false;
	}

	// 모두 새로 저장한다.
	bool bResult;
	KHash< KFileInfo, hashPr_string_nocase >::node* _node = NULL;

	int seed = 0;
	bResult = m_hsIndexFileList.get_first_node( _node );
	while ( bResult )
	{
		if( _node->value.saveFileInfo( pFile, &seed ) == false )
		{
			char buf[1024] = "";
			DWORD last_error = ::GetLastError();
			FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
			char szErr[1024] = { 0, };
			s_sprintf( szErr, _countof( szErr ), "%s(%d) - %s", buf, last_error, strNewIndexFile.c_str() );

			MessageBox( NULL, szErr, "Rappelz", MB_OK | MB_ICONERROR );

			CloseHandle( pFile );
			DeleteFile( strNewIndexFile.c_str() );
			return false;
		}
		bResult = m_hsIndexFileList.get_next_node( _node );
	}

	FlushFileBuffers( pFile );
	CloseHandle( pFile );

	// old 파일 삭제
	SetFileAttributes( strOldIndexFile.c_str(), FILE_ATTRIBUTE_NORMAL );
	DeleteFile( strOldIndexFile.c_str() );

	// 백업
	MoveFile( strIndexFile.c_str(), strOldIndexFile.c_str() );

	// 적용
	if( ::MoveFile( strNewIndexFile.c_str(), strIndexFile.c_str() ) != TRUE )
	{
		char buf[1024] = "";
		DWORD last_error = ::GetLastError();
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, last_error, 0, buf, _countof(buf), NULL );
		char szErr[1024] = { 0, };
		s_sprintf( szErr, _countof( szErr ), "%s(%d) - %s", buf, last_error, strNewIndexFile.c_str() );

		MessageBox( NULL, szErr, "Rappelz", MB_OK | MB_ICONERROR );

		// 복구
		MoveFile( strOldIndexFile.c_str(), strIndexFile.c_str() );
		DeleteFile( strNewIndexFile.c_str() );
		return false;
	}

	return true;
}

const size_t KPackingFileSystem::GetIndexNameList( std::vector<std::string>& vecNameList )
{
	if ( m_hsIndexFileList.size() <= 0 )	
		return (std::numeric_limits<size_t>::max)();

	KHash< KPackingFileSystem::KFileInfo, hashPr_string_nocase >::node* _node = NULL;

	bool bResult = m_hsIndexFileList.get_first_node( _node );
	while ( bResult )
	{
		KPackingFileSystem::KFileInfo info = _node->value;
		if ( info.m_Size > 0 )
			vecNameList.push_back( _node->key.m_pStr );

		bResult = m_hsIndexFileList.get_next_node( _node );
	}

	return vecNameList.size();
}

size_t KPackingFileSystem::getFileHashKey( const char* szFileName, int nDataFileCount )
{
	int key = 0;
	for ( int i = 0; szFileName[i]; ++i )
	{
		int tmp = szFileName[i];

		if ( tmp >= 'A' && tmp <= 'Z' ) { tmp -= ( 'A' - 'a' ); }
		
		key = key * 32 - key + tmp;
	}
	if ( key < 0 ) key = 0 - key;
	return key % nDataFileCount;
}

std::string	KPackingFileSystem::getDataFileName( const char* szDataFileName, size_t hash_key )
{
	char buf[256] = "";
	s_sprintf( buf, _countof( buf ), "%s.%03d", szDataFileName, hash_key+1 );
	return buf;
}
