
#include "../../include/internet/XHttpClient.h"
#include "XHttpAsyncDelegate.h"
#include "XHttpSyncDelegate.h"


XHttpClient::XHttpClient( const std::wstring& strAppName )
	: m_hSessionSync( NULL )
	, m_hSessionAsync( NULL )
	, m_dwTimeoutMs( 0 )
	, m_strAppName( strAppName )
{
}

XHttpClient::~XHttpClient()
{
	Close();
}

bool	XHttpClient::OpenSync()
{
	if( m_hSessionSync == NULL )
	{
		m_hSessionSync = ::WinHttpOpen(
							(m_strAppName.empty() == false) ? m_strAppName.c_str() : NULL,
							WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
							WINHTTP_NO_PROXY_NAME,
							WINHTTP_NO_PROXY_BYPASS,
							0 );

		if( m_hSessionSync == NULL )
		{
			return false;
		}

		if( m_dwTimeoutMs > 0 )
		{
			::WinHttpSetTimeouts( m_hSessionSync, 0, m_dwTimeoutMs, m_dwTimeoutMs, m_dwTimeoutMs );
		}
	}

	return true;
}

bool	XHttpClient::OpenAsync()
{
	if( m_hSessionAsync == NULL )
	{
		m_hSessionAsync = ::WinHttpOpen(
								(m_strAppName.empty() == false) ? m_strAppName.c_str() : NULL,
								WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
								WINHTTP_NO_PROXY_NAME,
								WINHTTP_NO_PROXY_BYPASS,
								WINHTTP_FLAG_ASYNC );

		if( m_hSessionAsync == NULL )
		{
			return false;
		}

		if( m_dwTimeoutMs > 0 )
		{
			::WinHttpSetTimeouts( m_hSessionAsync, 0, m_dwTimeoutMs, m_dwTimeoutMs, m_dwTimeoutMs );
		}
	}

	return true;
}

void	XHttpClient::Close()
{
	if( m_hSessionSync != NULL )
	{
		::WinHttpCloseHandle( m_hSessionSync );
		m_hSessionSync = NULL;
	}

	if( m_hSessionAsync != NULL )
	{
		::WinHttpCloseHandle( m_hSessionAsync );
		m_hSessionAsync = NULL;
	}
}

void	XHttpClient::Timeout( DWORD dwTimeoutMs )
{
	m_dwTimeoutMs = dwTimeoutMs;
	if( m_dwTimeoutMs > 0 )
	{
		if( m_hSessionSync != NULL )
		{
			::WinHttpSetTimeouts( m_hSessionSync, 0, m_dwTimeoutMs, m_dwTimeoutMs, m_dwTimeoutMs );
		}

		if( m_hSessionAsync != NULL )
		{
			::WinHttpSetTimeouts( m_hSessionAsync, 0, m_dwTimeoutMs, m_dwTimeoutMs, m_dwTimeoutMs );
		}
	}
}

bool	XHttpClient::DoVerb( const wchar_t* pVerb, const XHttpRequest& Request, XHttpContent* pContent, bool bIsAsync )
{
	if( pContent != NULL && pContent->Reset() == false )
	{
		return false;
	}

	const size_t nMinBufferSize = 4096;
	if( bIsAsync == true )
	{
		if( OpenAsync() == false )
		{
			return false;
		}

		XHttpAsyncDelegate* pDelegate = new XHttpAsyncDelegate();
		if( pDelegate->Execute( m_hSessionAsync, pVerb, Request, pContent, nMinBufferSize ) == false )
		{
			delete pDelegate;
			return false;
		}
	}
	else
	{
		if( OpenSync() == false )
		{
			return false;
		}

		XHttpSyncDelegate Delegate;
		if( Delegate.Execute( m_hSessionSync, pVerb, Request, pContent, nMinBufferSize ) == false )
		{
			return false;
		}
	}

	return true;
}
