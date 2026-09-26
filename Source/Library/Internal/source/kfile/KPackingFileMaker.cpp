
#include "../../include/kfile/KPackingFileMaker.h"
#include "../../include/kfile/KFileNameCipher.h"
#include "../../include/toolkit/XFileUtil.h"
#include "../../include/compress/XZip.h"


KPackingFileMaker::KPackingFileMaker( KPackingFileSystem* pPackingFS )
	: m_pPackingFS( pPackingFS )
	, m_destDataFileInfo( NULL )
	, m_nDataFileCount( 0 )
{
	m_destDataFileInfo = new sDataFileInfo[pPackingFS->GetDataFileCount()];
	m_nDataFileCount = pPackingFS->GetDataFileCount();
}

KPackingFileMaker::~KPackingFileMaker()
{
	close();
}

void	KPackingFileMaker::close()
{
	if( m_destDataFileInfo != NULL )
	{
		for( int i = 0; i < m_nDataFileCount; ++i )
		{
			m_destDataFileInfo[i].close();
		}

		delete [] m_destDataFileInfo;
		m_destDataFileInfo = NULL;

		m_nDataFileCount = 0;
	}

	m_destIndexFileList.clear();
}

KPackingFileMaker::sDataFileInfo*	KPackingFileMaker::getDataFileInfo( size_t hash_key )
{
	if( static_cast< int >( hash_key ) < m_nDataFileCount )
	{
		return &m_destDataFileInfo[hash_key];
	}

	return NULL;
}

bool	KPackingFileMaker::packingFile( const char *szFileName, const char* pBuffer, size_t size, bool bCompress )
{
	std::string strEncodedFileName = szFileName;
	std::string strFileName = strEncodedFileName;
	std::string strExtension;

	if( KFileNameCipher::IsEncodedName( strEncodedFileName ) )  //<= "Win98sUpdateUtil.exe" 제대로 체크 안된다.
		KFileNameCipher::DecodeFileName( strFileName );
	else
		KFileNameCipher::EncodeFileName( strEncodedFileName );

	XFileUtil::GetFileName( strFileName.c_str(), strFileName );
	XFileUtil::GetFileExtension( strFileName.c_str(), strExtension );

	//
	size_t hash_key = KPackingFileSystem::getFileHashKey( strEncodedFileName.c_str(), m_nDataFileCount );
	sDataFileInfo* d_info = getDataFileInfo( hash_key );
	if( d_info->m_file == NULL )
	{
		std::string dataFileName = KPackingFileSystem::getDataFileName( m_pPackingFS->GetDataFileName().c_str(), hash_key ) + ".new";
		if( !d_info->create( dataFileName.c_str() ) )
		{
			return false;
		}
	}

	size_t oldOffset = 0;
	if( (size >= KFileSystem::MIN_COMPRESS_FILE_SIZE) && (bCompress == true) )
	{
		size_t xzipBufferSize = XZip::GetBufferSize( size );
		char* xzipBuffer = new char[xzipBufferSize];

		size_t compressSize = XZip::Compress( pBuffer, size, xzipBuffer, xzipBufferSize );

		// 압축되지 않고 팩킹된 파일을 여러번 repacking할 때 compressSize < size를 하면 bIsNeedXOR 때문에 문제가 될수 있다
		if( compressSize < size )
		{
			if( d_info->write( xzipBuffer, compressSize, &oldOffset ) == false )
			{
				delete [] xzipBuffer;
				return false;
			}

			KPackingFileSystem::KFileInfo kFileInfo( strEncodedFileName.c_str(), compressSize, oldOffset, hash_key );
			m_destIndexFileList.push_back( kFileInfo );
		}
		else
		{
			if( d_info->write( pBuffer, size, &oldOffset ) == false )
			{
				delete [] xzipBuffer;
				return false;
			}

			KPackingFileSystem::KFileInfo kFileInfo( strEncodedFileName.c_str(), size, oldOffset, hash_key );
			m_destIndexFileList.push_back( kFileInfo );
		}

		delete [] xzipBuffer;
	}
	else
	{
		if( d_info->write( pBuffer, size, &oldOffset ) == false )
		{
			return false;
		}

		KPackingFileSystem::KFileInfo kFileInfo( strEncodedFileName.c_str(), size, oldOffset, hash_key );
		m_destIndexFileList.push_back( kFileInfo );
	}

	return true;
}

bool	KPackingFileMaker::saveIndexFile()
{
	std::string strNewIndexFile = std::string( m_pPackingFS->GetIndexFileName() ) + ".new";

	SetFileAttributes( strNewIndexFile.c_str(), FILE_ATTRIBUTE_NORMAL );
	DeleteFile( strNewIndexFile.c_str() );

	KHFILE pFile;
	pFile = ::CreateFile( strNewIndexFile.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( !IsValidKHFile( pFile) )	
	{
		assert( 0 );
		return false;
	}

	int seed = 0;
	std::vector< KPackingFileSystem::KFileInfo >::const_iterator pos = m_destIndexFileList.begin();
	std::vector< KPackingFileSystem::KFileInfo >::const_iterator end = m_destIndexFileList.end();

	for( ; pos != end; ++pos )
	{
		const KPackingFileSystem::KFileInfo& info = (*pos);
		if( info.saveFileInfo( pFile, &seed ) == false )
		{
			CloseHandle( pFile );
			DeleteFile( strNewIndexFile.c_str() );
			return false;
		}
	}

	FlushFileBuffers( pFile );
	CloseHandle( pFile );
	return true;
}
