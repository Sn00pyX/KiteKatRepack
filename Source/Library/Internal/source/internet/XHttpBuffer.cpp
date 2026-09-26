
#include "../../include/internet/XHttpBuffer.h"
#include "../../include/toolkit/safe_function.h"

#include <memory>


XHttpBuffer::XHttpBuffer( size_t nMaxSize )
	: m_pBuffer( NULL )
	, m_nMaxSize( 0 )
	, m_nDataSize( 0 )
	, m_nCurrentPos( 0 )
{
	Init( nMaxSize );
}

XHttpBuffer::~XHttpBuffer()
{
	if( m_pBuffer != NULL )
	{
		delete [] m_pBuffer;
	}
}

void	XHttpBuffer::Init( size_t nSize )
{
	if( m_nMaxSize < nSize )
	{
		if( m_pBuffer != NULL )
		{
			delete [] m_pBuffer;
			m_pBuffer = NULL;
		}

		m_pBuffer = new char[nSize];
		m_nMaxSize = nSize;
	}

	m_nDataSize = 0;
	m_nCurrentPos = 0;
}

void	XHttpBuffer::Copy( const void* pData, size_t nSize )
{
	Init( nSize );

	s_memcpy( m_pBuffer, m_nMaxSize, pData, nSize );
	m_nDataSize = nSize;
	m_nCurrentPos = 0;
}

void	XHttpBuffer::HadCopied( size_t nSize )
{
	if( (m_nDataSize+nSize) <= m_nMaxSize )
	{
		m_nDataSize += nSize;
	}
}

const void*	XHttpBuffer::Buffer()	const
{
	return m_pBuffer + m_nCurrentPos;
}

void*	XHttpBuffer::Buffer()
{
	return m_pBuffer + m_nCurrentPos;
}

size_t	XHttpBuffer::Size()	const
{
	return m_nDataSize - m_nCurrentPos;
}

size_t	XHttpBuffer::MaxSize()	const
{
	return m_nMaxSize;
}

bool	XHttpBuffer::Empty()	const
{
	return (m_nDataSize == m_nCurrentPos);
}

void	XHttpBuffer::Flush( size_t nCount )
{
	if( m_nCurrentPos+nCount <= m_nDataSize )
	{
		m_nCurrentPos += nCount;
	}
}

void	XHttpBuffer::TurnBack()
{
	m_nCurrentPos = 0;
}

void	XHttpBuffer::Clear()
{
	m_nDataSize = 0;
	m_nCurrentPos = 0;
}
