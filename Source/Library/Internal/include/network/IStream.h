// IStream.h
//
// basic stream interface
// (STL 의 그것과 호환되게 하는것이 나으려나..?)
//
// by Testors

#pragma once

#include <cstdio>
#include <memory.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <algorithm>

struct IStreamIntf
{
	virtual ~IStreamIntf()	{}

	virtual int			Read( void * szBuf, size_t nLen )			= 0;
	virtual int			Write( const void * szBuf, size_t nLen )	= 0;
	virtual int			Peek( void * szBuf, size_t nLen )			= 0;
	virtual const char* GetBuf()									{ return NULL; }

	enum SEEK { BEGIN = SEEK_SET, CURRENT = SEEK_CUR, END = SEEK_END };
	virtual size_t		Tell()										{ return 0;		}
	virtual bool		Seek( SEEK seek, int offset )				{ return false; }

	virtual bool		IsValid()		= 0;
	virtual int			Size()			= 0;
};

struct XFileStream : public IStreamIntf
{
	enum MODE { READ = 0, CREATE = 2, APPEND = 3 };

	XFileStream() : m_fp( 0 ) {}
	XFileStream( const char *szFileName, MODE mode )
	{
		const char *pMode = "rb";

			 if ( mode == READ   ) pMode = "rb";
		else if ( mode == CREATE ) pMode = "wb";
		else if ( mode == APPEND ) pMode = "a+b";

		m_fp = NULL;
		fopen_s( &m_fp, szFileName, pMode );
	}

	virtual ~XFileStream()
	{
		if ( m_fp ) fclose( m_fp );
	}

	virtual int			Read( void * szBuf, size_t nLen )			{ return (int)fread( szBuf, 1, nLen, m_fp ); }
	virtual int			Write( const void * szBuf, size_t nLen )	{ return (int)fwrite( szBuf, 1, nLen, m_fp ); }
	virtual int			Peek( void * szBuf, size_t nLen )
	{
		size_t len = fread( szBuf, 1, nLen, m_fp );
		fseek( m_fp, SEEK_CUR, 0 - (int)len );
		return (int)len;
	}

	virtual const char* GetBuf()									{ return NULL; }

	virtual size_t		Tell()										{ return ftell( m_fp );		}
	virtual bool		Seek( SEEK seek, int offset )				{ return fseek( m_fp, seek, offset ) == 0;	 }

	virtual bool		IsValid()									{ return m_fp != 0; }
	virtual int			Size()
	{
		if ( !m_fp ) return -1;

		struct stat st;
		memset( &st, 0, sizeof(st) );

		if ( fstat( _fileno( m_fp ), &st ) < 0 ) return -1;

		return st.st_size;
	}

private:
	FILE *m_fp;
};

struct XMemoryStream : public IStreamIntf
{
	XMemoryStream()
		: m_bLink( false )
		, m_pBuffer( 0 )
		, m_nPos( 0 )
		, m_nBufLen( 0 )
	{
	}

	XMemoryStream( const void *pBuffer, size_t len, bool bDeepCopy = true )
	{
		m_nBufLen = len;
		m_nPos = 0;

		if ( bDeepCopy )
		{
			m_bLink = false;
			m_pBuffer = (char*)malloc( len );
			s_memcpy( m_pBuffer, len, pBuffer, len );
		}
		else
		{
			m_bLink = true;
			m_pBuffer = reinterpret_cast< char* >( const_cast< void* >( pBuffer ) );
		}
	}

	virtual ~XMemoryStream()
	{
		if ( m_pBuffer && !m_bLink ) 
		{
			free( m_pBuffer );
		}
	}

	virtual int			Read( void * szBuf, size_t nLen )
	{
		if ( !m_pBuffer ) return 0;

		size_t read_len = 0;

		if ( m_nBufLen - m_nPos < nLen ) read_len = nLen - ( m_nBufLen - m_nPos );
		else read_len = nLen;

		s_memcpy( szBuf, nLen, m_pBuffer + m_nPos, read_len );
		m_nPos += read_len;

		return (int)read_len;
	}

	virtual int			Write( const void * szBuf, size_t nLen )
	{
		if ( m_bLink ) remove_link();

		if ( !m_pBuffer ) alloc( nLen );

		if ( m_nBufLen - m_nPos < nLen ) 
		{
			char* temp = (char*)realloc( m_pBuffer, nLen - ( m_nBufLen - m_nPos ) );
			if( temp != NULL )
			{
				m_pBuffer = temp;
			}
			else
			{
				return 0;
			}

			m_nBufLen = nLen - ( m_nBufLen - m_nPos );
		}

		s_memcpy( m_pBuffer + m_nPos, m_nBufLen - m_nPos, szBuf, nLen );

		return (int)nLen;
	}

	virtual int			Peek( void * szBuf, size_t nLen )
	{
		if ( !m_pBuffer ) return 0;

		size_t read_len = 0;

		if ( m_nBufLen - m_nPos < nLen ) read_len = nLen - ( m_nBufLen - m_nPos );
		else read_len = nLen;

		s_memcpy( szBuf, nLen, m_pBuffer + m_nPos, read_len );

		return (int)read_len;
	}

	virtual size_t		Tell()										{ return m_nPos;		}
	virtual bool		Seek( SEEK seek, int offset )
	{
		if ( seek == BEGIN )			m_nPos = offset;
		else if ( seek == CURRENT )	m_nPos = (int)m_nPos+offset;
		else if ( seek == END )		m_nPos = (int)m_nBufLen+offset;

		if ( m_nPos > m_nBufLen )
			m_nPos = m_nBufLen;

		return true;
	}

	virtual bool		IsValid()									{ return !!m_pBuffer; }
	virtual int			Size()
	{
		return (int)m_nBufLen;
	}

	virtual const char* GetBuf()									{ return m_pBuffer; }

private:

	void alloc( size_t len )
	{
		m_bLink = false;
		m_pBuffer = (char*)malloc( len );
		m_nBufLen = len;
	}

	void remove_link()
	{
		m_bLink = false;
		char *pNewBuffer = (char*)malloc( m_nBufLen );
		s_memcpy( pNewBuffer, m_nBufLen, m_pBuffer, m_nBufLen );
		m_pBuffer = pNewBuffer;
	}

	bool	m_bLink;
	char	*m_pBuffer;
	size_t	m_nPos;
	size_t	m_nBufLen;
};
