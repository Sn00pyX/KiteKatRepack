#pragma once

#include "IFileSystem.h"
#include "XOREn.h"

class KEncodedFile : public KStream
{
public:

	KEncodedFile( KStream *pFile ) : m_pFile( pFile )
	{
		// 2010.04.29 - prodongi
		setClassId("KEncodedFile");
	}

	~KEncodedFile()
	{
		delete m_pFile;
	}

	virtual size_t Read( void *pBuf, size_t read_size )
	{
		size_t offset = m_pFile->Tell();
		size_t len = m_pFile->Read( pBuf, read_size );

		char* p0 = reinterpret_cast< char* >( pBuf );
		char* p3 = p0 + len;
		char* p1 = p0 + (sizeof(int) - offset % sizeof(int)) % sizeof(int);
		char* p2 = p3 - (offset + len) % sizeof(int);

		if ( p1 > p3 )
			p1 = p2 = p3;

		for (; p0 < p1; p0++, offset++ )
			*p0 ^= XOREn::GetEncodeKeyChar( offset ); 

		for (; p1 < p2; p1 += sizeof(int), offset += sizeof(int) )
			*reinterpret_cast<int*>(p1) ^= XOREn::GetEncodeKeyInt( offset );

		for (; p2 < p3; p2++, offset++ )
			*p2 ^= XOREn::GetEncodeKeyChar( offset ); 

		return len;
	}

	virtual size_t Write( const void *pBuf, size_t write_size )
	{	
		const int BUF_SIZE = 32768;
		char buf[ BUF_SIZE ];

		size_t offset = m_pFile->Tell();

		const char* p = reinterpret_cast< const char* >( pBuf );

		size_t sum_wlen = 0;
		while ( write_size > 0 )
		{
			size_t len = BUF_SIZE < write_size ? BUF_SIZE : write_size;
			for ( size_t i = 0; i < len; i++, p++ )
				buf[i] = *p ^ XOREn::GetEncodeKeyChar( offset+i );

			size_t wlen = m_pFile->Write( buf, len );
			if ( wlen == 0 )
				break;

			sum_wlen += wlen;
			write_size -= len;
			offset += len;
		}

		return sum_wlen;
	}

	virtual size_t Seek( long offset, enum_seek_origin origin )		{ return m_pFile->Seek( offset, origin ); }
	virtual size_t Tell() const										{ return m_pFile->Tell();				}
	virtual size_t GetLength() const								{ return m_pFile->GetLength();			}
	virtual bool Eos() const										{ return m_pFile->Eos(); }
	virtual bool IsValid() const									{ return m_pFile->IsValid(); }

	// 2010.04.29 - prodongi
	virtual size_t getOriginalSize() const
	{
		return m_pFile->getOriginalSize(Size());
	}

private:

	KStream *m_pFile;
};