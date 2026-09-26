
#include "../../include/kfile/XFileBasedFS.h"
#include "../../include/kfile/KEncodedFile.h"
#include "../../include/toolkit/khash.h"
#include <stdlib.h>

struct DataFile
{
	bool    Init( const char *szBaseFileName )			{ return true; }
	bool    DeInit()									{ return true; }
	size_t	GetCurrentSize()							{ return 0; }
	size_t	Read( size_t offset, void *buf, size_t size ) { return size; }
	size_t	Write( size_t offset, void *buf, size_t size ) { return size; }
};

struct XFileInfo
{
	XFileInfo( size_t base, size_t size ) : nBaseOffset( base ), nFileSize( size )
	{
	}

	size_t			nBaseOffset;
	size_t			nFileSize;
};

__declspec( thread ) FILE*			g_pFP;

struct XFSInfo
{
	XFSInfo()
		: nDataFileSize( 0 )
	{}

	std::string								strDataFile;
	std::string								strIndexFile;

	KHash< XFileInfo, hashPr_string_nocase >	hsFileIndex;
	size_t									nDataFileSize;
};

struct XFile : public KStream
{
	XFile( XFileBasedFS* pParent, FILE *fp, const char *szFileName, size_t base, size_t size ) 
		: m_pParent( pParent ), m_fp( fp ), strFileName( szFileName ), m_nBaseOffset( base ), m_nSize( size ), m_nCurrentOffset( 0 ), m_bModified( false )
	{
		// 2010.04.29 - prodongi
		setClassId("XFileFile");
	}

	~XFile()
	{
		if ( m_pParent )
			m_pParent->close( this );
	}

	const char* GetFileName()											{ return strFileName.c_str(); }
	size_t GetBaseOffset()												{ return m_nBaseOffset; }

	virtual size_t Read( void *pBuf, size_t read_size );
	virtual size_t Write( const void *pBuf, size_t write_size );
	virtual size_t Seek( long offset, enum_seek_origin origin );
	virtual size_t GetLength() const									{ return m_nSize; }
	virtual size_t Tell() const											{ return m_nCurrentOffset; }
	virtual bool Eos() const											{ return m_nCurrentOffset >= m_nSize; }
	virtual bool IsValid() const										{ return m_fp != NULL; }

	size_t	GetMaxSize() { return m_nBaseOffset + m_nSize; }

private:
	XFileBasedFS* m_pParent;
	FILE * m_fp;
	std::string strFileName;
	size_t m_nBaseOffset;
	size_t m_nCurrentOffset;
	size_t m_nSize;
	bool   m_bModified;
};

size_t XFile::Write( const void *pBuf, size_t write_size )
{
#ifdef FILE_PACKER
	fseek( m_fp, 0, SEEK_END );
	if ( fwrite( pBuf, write_size, 1, m_fp ) != 1 ) return 0;

	m_nSize += write_size;
	m_bModified = true;

	return write_size;
#else
	return 0;
#endif
};


size_t XFile::Read( void *pBuf, size_t read_size )
{
	if ( m_nCurrentOffset + read_size > m_nSize )
		read_size = m_nSize - m_nCurrentOffset;

	if ( read_size == 0 )	return 0;

	::_fseeki64( m_fp, m_nBaseOffset + m_nCurrentOffset, SEEK_SET );
	if ( fread( pBuf, read_size, 1, m_fp ) == 1 ) 
	{
		m_nCurrentOffset += read_size;
		return read_size;
	}

	return 0;
}

size_t XFile::Seek( long offset, enum_seek_origin origin )
{
	if ( origin == seekCur )		m_nCurrentOffset += offset;
	else if ( origin == seekEnd )	m_nCurrentOffset = m_nSize;
	else if ( origin == seekSet )	m_nCurrentOffset = offset;

	if ( m_nCurrentOffset < 0 ) m_nCurrentOffset = 0;
	if ( m_nCurrentOffset > m_nSize ) m_nCurrentOffset = m_nSize;

	return m_nCurrentOffset;
}

KStream* XFileBasedFS::open( const char* szFileName, ACCESS_MODE access_mode )
{
	if ( !g_pFP )
	{
		char *szAccessMode = "rb";
		if ( access_mode & XFileBasedFS::WRITE ) szAccessMode = "a+b";

		g_pFP = NULL;
		fopen_s( &g_pFP, m_pInfo->strDataFile.c_str(), szAccessMode );
		if ( !g_pFP ) 
		{
//			assert( 0 );
			return NULL;
		}

		// 데이터 파일 크기 세팅
		fseek( g_pFP, 0, SEEK_END );
		m_pInfo->nDataFileSize = (size_t)::_ftelli64( g_pFP );
		fseek( g_pFP, 0, SEEK_SET );
	}

	XFileInfo tag( 0, 0 );

	if ( !m_pInfo->hsFileIndex.lookup( szFileName, tag ) )
	{
#ifdef FILE_PACKER
		m_pInfo->hsFileIndex.add( szFileName, XFileInfo( m_pInfo->nDataFileSize, 0 ) );
		return new XFile( this, g_pFP, szFileName, m_pInfo->nDataFileSize, 0 );
#else
		return NULL;
#endif
	}
	else
	{
		if ( tag.nFileSize == 0 ) return NULL;

#ifdef FILE_PACKER
		return new XFile( this, g_pFP, szFileName, tag.nBaseOffset, tag.nFileSize );
#else
		return new KEncodedFile( new XFile( this, g_pFP, szFileName, tag.nBaseOffset, tag.nFileSize ) );
#endif
	}
}

bool XFileBasedFS::close( XFile *pFile )
{
#ifdef FILE_PACKER

	fflush( g_pFP );

	if ( pFile->GetMaxSize() > m_pInfo->nDataFileSize )
	{
		m_pInfo->nDataFileSize = pFile->GetMaxSize();
	}
	
	m_pInfo->hsFileIndex.modify( pFile->GetFileName(), XFileInfo( pFile->GetBaseOffset(), pFile->Size() ) );

#endif

	return true;
}

XFileBasedFS::XFileBasedFS() : m_pInfo( new XFSInfo )
{
}

XFileBasedFS::~XFileBasedFS()
{
	delete m_pInfo;
}

bool XFileBasedFS::initIndexFile( const char *szIndexFileName )
{
	m_pInfo->strIndexFile = szIndexFileName;
	
#ifdef FILE_PACKER
	char *szAccessMode = "rb";
#else
	char *szAccessMode = "rb";
#endif

	FILE *fp = NULL;
	fopen_s( &fp, szIndexFileName, szAccessMode );
	if ( !fp ) return false;

	size_t cnt = 0;
	fseek( fp, 0, SEEK_SET );

	while ( !feof(fp) )
	{
		unsigned char name_size;
		char szFileName[1024];
		size_t offset;
		size_t size;
		if ( fread( &name_size, 1, 1, fp ) != 1 ) 
		{
			break;
		}
		++cnt;
		assert( name_size );
		if ( fread( szFileName, name_size, 1, fp ) != 1 ) assert( 0 );
		szFileName[name_size] = 0;
		for ( unsigned i = 0; i < name_size; ++i ) szFileName[i] -= 80;
		if ( fread( &offset, sizeof(offset), 1, fp ) != 1 ) assert( 0 );
		if ( fread( &size, sizeof(size), 1, fp ) != 1 ) assert( 0 );
		
		m_pInfo->hsFileIndex.add( szFileName, XFileInfo( offset, size ) );
	}

	fclose( fp );

	return true;
}

bool XFileBasedFS::writeIndexFile()
{
#ifdef FILE_PACKER
	FILE *fp = fopen( m_pInfo->strIndexFile.c_str(), "wb" );
	if ( !fp ) return false;

	FILE *tfp = fopen( "index.txt", "w" );

	bool res;
	KHash< XFileInfo, hashPr_string_nocase >::node* _node = NULL;

	res = m_pInfo->hsFileIndex.get_first_node( _node );
	while ( res )
	{
		size_t nBaseOffset = _node->value.nBaseOffset;
		size_t nFileSize   = _node->value.nFileSize;

		// do something
		fprintf( tfp, "%s\n", _node->key.m_pStr );
		fprintf( tfp, "%u\n", nBaseOffset );
		fprintf( tfp, "%u\n", nFileSize );
		unsigned char flen = strlen(_node->key.m_pStr);		
		char buf[1024];
		s_memcpy( buf, sizeof( buf ), _node->key.m_pStr, flen );
		for ( int i = 0; i < flen; ++i ) buf[i] += 80;

		assert( flen );

		if ( fwrite( &flen, 1, 1, fp ) != 1 ) assert( 0 );
		if ( fwrite( buf, flen, 1, fp ) != 1 ) assert( 0 );
		if ( fwrite( &nBaseOffset, sizeof(nBaseOffset), 1, fp )  != 1 ) assert( 0 );
		if ( fwrite( &nFileSize, sizeof(nFileSize), 1, fp )  != 1 ) assert( 0 );

		res = m_pInfo->hsFileIndex.get_next_node( _node );

		fflush( tfp );
	}

	fclose( tfp );
	fclose( fp );
#endif

	return true;
}

size_t XFileBasedFS::getFileSize( const char *szFileName ) const
{
	XFileInfo temp( 0, 0 );
	if ( !m_pInfo->hsFileIndex.lookup( szFileName, temp ) ) return 0;

	return temp.nFileSize;
}

bool XFileBasedFS::Init( const char *szIndexFileName, const char *szDataFileName )
{
	// 데이터 파일 오픈
	m_pInfo->strDataFile = szDataFileName;

	// 인덱스 파일 오픈
	return initIndexFile( szIndexFileName );
}

bool XFileBasedFS::DeInit()
{
	fclose( g_pFP );

	writeIndexFile();
	m_pInfo->hsFileIndex.clear();

	m_pInfo->strIndexFile = "";

	return true;
}