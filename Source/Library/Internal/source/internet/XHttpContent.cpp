
#include "../../include/internet/XHttpContent.h"


XHttpContent::XHttpContent()
	: m_szFunction( NULL )
	, m_nLine( 0 )
	, m_bSucceeded( false )
	, m_hEvent( NULL )
{
	Reset();
}

XHttpContent::~XHttpContent()
{
	if( m_hEvent != NULL )
	{
		::CloseHandle( m_hEvent );
		m_hEvent = NULL;
	}
}

bool	XHttpContent::Reset()
{
	m_szFunction = NULL;
	m_nLine = 0;

	m_bSucceeded = false;
	if( m_hEvent == NULL )
	{
		m_hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
		if( m_hEvent == NULL )
		{
			return false;
		}
	}
	else
	{
		if( ::ResetEvent( m_hEvent ) == FALSE )
		{
			return false;
		}
	}

	return true;
}

bool	XHttpContent::IsSucceeded()	const
{
	if( Wait( 0 ) == true )
	{
		return m_bSucceeded;
	}

	return false;
}

bool	XHttpContent::Wait( DWORD dwTimeoutMs )	const
{
	if( m_hEvent != NULL )
	{
		if( ::WaitForSingleObject( m_hEvent, dwTimeoutMs ) != WAIT_OBJECT_0 )
		{
			return false;
		}
	}

	return true;
}

bool	XHttpContent::OnCallbackHeader( const XHttpResponseHeader& Header )
{
	if( OnHeader( Header ) == false )
	{
		m_bSucceeded = false;
		::SetEvent( m_hEvent );

		return false;
	}

	return true;
}

bool	XHttpContent::OnCallbackBody( const void* pBuffer, size_t nReadBytes )
{
	if( OnBody( pBuffer, nReadBytes ) == false )
	{
		m_bSucceeded = false;
		::SetEvent( m_hEvent );

		return false;
	}

	return true;
}

void	XHttpContent::OnCallbackComplete()
{
	OnComplete();

	m_bSucceeded = true;
	::SetEvent( m_hEvent );
}

void	XHttpContent::OnCallbackError( const wchar_t* szWhere, DWORD dwError, const wchar_t* szFunction, int nLine )
{
	m_szFunction = szFunction;
	m_nLine = nLine;

	OnError( szWhere, dwError );

	m_bSucceeded = false;
	::SetEvent( m_hEvent );
}
