
#include "../../include/kfile/KAsciiFiler.h"
#include <assert.h>
//
// Ascii Filer
//


const int KFILE_MASTER_ID = 'KFID';
const int KFILE_TYPE_ASCII = 'KASC';
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

KAsciiFiler::KAsciiFiler()
{
}

KAsciiFiler::~KAsciiFiler()
{
}

bool KAsciiFiler::Save( KStream &stream )
{
	for ( unsigned i=0 ; i<m_vectorData.size() ; ++i )
	{
		if ( m_vectorData[i]->Save( stream, KDataObject::KDFM_ASCII ) == false ) return false;
	}
	return true;
}

bool KAsciiFiler::Load( KStream &stream )
{
	// 속도개선을 위해 메모리로 옮긴후 로드.
	char *buffer = new char[stream.GetLength()];
	stream.Read(buffer, stream.GetLength());

	KMemoryStream mStream ( buffer, stream.GetLength() );

	if ( mStream.IsValid() )
	{
		bool retval = true;
		while ( !mStream.Eos() && retval )
		{
			retval = false;
			KTemplateDataObject *obj = new KTemplateDataObject( mStream, retval, KDataObject::KDFM_ASCII );
			if ( retval == true )
			{
				m_vectorData.push_back( obj );
				m_vectorCreated.push_back( obj );
			}
		}
		if (!retval) {
#ifdef _DEBUG
			long	nCount=1;
			char	chr;
			char	buff[80];
			size_t	pos;

			pos = mStream.Tell();
			mStream.Seek( 0, KStream::seekSet );

			for (unsigned int i = 0; i < pos; i++)
			{
				mStream.Read( &chr, 1);
				if (chr == '\n') nCount++;
			}
			s_sprintf(buff, _countof( buff ), "%ld번째 줄 근처에 오류가 있습니다.", nCount);
			MessageBox(NULL, buff, "Error", MB_OK | MB_ICONERROR );
#endif
			delete [] buffer;
			return false;
		}
		delete [] buffer;
		return true;
	}
	else 
	{
		delete [] buffer;
		return false;
	}
}

// 같은 내용의 KBinaryFiler 객체를 만들어 포인터를 리턴
KBinaryFiler* KAsciiFiler::GetBinaryFiler()
{
	KBinaryFiler *pBinaryFiler = new KBinaryFiler;

	for ( int i = 0; i < GetDataObjectCount(); i++ )
	{
		pBinaryFiler->AddDataObject( GetDataObjectAt(i) );
	}
	return pBinaryFiler;
}

