
#include "../../include/kfile/KBinaryFiler.h"
#include "../../include/kfile/KAsciiFiler.h"
#include <assert.h>
//
// Binary Filer
//


const int KFILE_MASTER_ID = 'KFID';
const int KFILE_TYPE_BINARY = 'KBIN';
const int KFILE_BLOCK_ID = 'KBLK';

#pragma pack(push)
#pragma pack(1)
struct KFILEHEADER
{
	int masterid;
	int type;

	int master_version;
	int sub_version;

	int nblocks;

	char reserved[108];
};

struct KFILEBLOCK
{
	int blockid;
	int blocksize;
	char reserved[24];
};
#pragma pack(pop)

KBinaryFiler::KBinaryFiler()
{
}

KBinaryFiler::~KBinaryFiler()
{
}

bool KBinaryFiler::Save( KStream &stream )
{
	KFILEHEADER header;

	memset( &header, 0, sizeof(header) );
	header.masterid = KFILE_MASTER_ID;
	header.type = KFILE_TYPE_BINARY;
	header.master_version = 1;
	header.sub_version = 0;
	header.nblocks = static_cast<int>(m_vectorData.size());

	stream.Write( &header, sizeof(header) );

	size_t blockheadersize = sizeof(KFILEBLOCK);

	KFILEBLOCK blockheader;
	memset( &blockheader, 0, sizeof(blockheader) );
	blockheader.blockid = KFILE_BLOCK_ID;

	for ( unsigned i=0 ; i<m_vectorData.size() ; ++i )
	{
		size_t pre_pos = stream.Tell();
		stream.Seek( static_cast<long>(blockheadersize), KFileStream::seekCur );
		if ( m_vectorData[i]->Save( stream, KDataObject::KDFM_BINARY ) == false ) 
		{
			assert( 0 && "KBinaryFiler::Save Failed!!!" );
			return false;
		}
		blockheader.blocksize = static_cast<int>(stream.Tell() - (pre_pos + blockheadersize));
		stream.Seek( static_cast<long>(pre_pos), KFileStream::seekSet );
		stream.Write( &blockheader, sizeof(blockheader) );
		stream.Seek( blockheader.blocksize, KFileStream::seekCur );
	}
	return true;
}

bool KBinaryFiler::Load( KStream &stream )
{
	KFILEHEADER header;
	KFILEBLOCK blockheader;

	stream.Read( &header, sizeof(header) );
	if ( header.masterid == KFILE_MASTER_ID &&
		header.type == KFILE_TYPE_BINARY )
	{
		if ( header.master_version >= 1 &&
			header.sub_version >= 0 )
		{
			for( int i=0 ; i<header.nblocks ; ++i )
			{
				stream.Tell();
				stream.Read( &blockheader, sizeof(blockheader) );
				if ( blockheader.blockid != KFILE_BLOCK_ID )
				{
					assert(false && "File Error!!!!");
					return false;
				}
				KTemplateDataObject *obj = new KTemplateDataObject( stream, KDataObject::KDFM_BINARY );
				m_vectorData.push_back( obj );
				m_vectorCreated.push_back( obj );
			}
			return true;
		}
	}
	return false;
}

// 같은 내용의 KAsciiFiler 객체를 만들어 포인터를 리턴
KAsciiFiler* KBinaryFiler::GetAsciiFiler()
{
	KAsciiFiler *pAsciiFiler = new KAsciiFiler;
	
	for ( int i = 0; i < GetDataObjectCount(); i++ )
	{
		pAsciiFiler->AddDataObject( GetDataObjectAt(i) );
	}
	return pAsciiFiler;
}

