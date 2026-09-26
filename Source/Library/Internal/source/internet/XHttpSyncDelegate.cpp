
#include "XHttpSyncDelegate.h"
#include "../../include/internet/XHttpResponseHeader.h"
#include "../../include/internet/XHttpBuffer.h"
#include "XHttpUtil.h"


XHttpSyncDelegate::XHttpSyncDelegate()
	: m_hConnect( NULL )
	, m_hRequest( NULL )
{
}

XHttpSyncDelegate::~XHttpSyncDelegate()
{
	Close();
}

void	XHttpSyncDelegate::Close()
{
	if( m_hRequest != NULL )
	{
		::WinHttpCloseHandle( m_hRequest );
		m_hRequest = NULL;
	}

	if( m_hConnect != NULL )
	{
		::WinHttpCloseHandle( m_hConnect );
		m_hConnect = NULL;
	}
}

bool	XHttpSyncDelegate::Execute(	HINTERNET hSession,
									const wchar_t* pVerb,
									const XHttpRequest& Request,
									XHttpContent* pContent,
									size_t nMinBufferSize )
{
	if( OpenRequest( hSession, pVerb, Request ) == false )
	{
		return false;
	}

	try
	{
		RequestProcess( Request );

		if( pContent == NULL || pContent->OnCallbackHeader( XHttpResponseHeader( m_hRequest ) ) == false )
		{
			return false;
		}

		if( ReadProcess( pContent, nMinBufferSize ) == false )
		{
			return false;
		}
	}
	catch( const XHttpUtil::XHttpException& except )
	{
		if( pContent != NULL )
		{
			pContent->OnCallbackError(	XHttpUtil::GetErrorFunction( except.m_dwWhere ), except.m_dwError,
										except.m_szFunction, except.m_nLine );
		}
	}

	Close();
	return true;
}

void	XHttpSyncDelegate::RequestProcess( const XHttpRequest& Request )
{
	if( SendRequest( Request ) == false )
	{
		throw XHttpUtil::XHttpException( API_SEND_REQUEST, ::GetLastError(), __FUNCTIONW__, __LINE__ );
	}

	if( ::WinHttpReceiveResponse( m_hRequest, NULL ) == FALSE )
	{
		throw XHttpUtil::XHttpException( API_RECEIVE_RESPONSE, ::GetLastError(), __FUNCTIONW__, __LINE__ );
	}

	XHttpResponseHeader Header( m_hRequest );
	DWORD dwStatusCode = Header.StatusCode();
	if( dwStatusCode != HTTP_STATUS_OK )
	{
		throw XHttpUtil::XHttpException( API_HEADER_STATUS_CODE, dwStatusCode, __FUNCTIONW__, __LINE__ );
	}
}

bool	XHttpSyncDelegate::ReadProcess( XHttpContent* pContent, size_t nMinBufferSize )
{
	if( pContent == NULL )
	{
		return false;
	}

	XHttpBuffer Buffer( nMinBufferSize );
	while( true )
	{
		DWORD dwReadBytes = 0;
		if( ::WinHttpReadData( m_hRequest, Buffer.Buffer(), static_cast< DWORD >( Buffer.MaxSize() ), &dwReadBytes ) == FALSE )
		{
			throw XHttpUtil::XHttpException( API_READ_DATA, ::GetLastError(), __FUNCTIONW__, __LINE__ );
		}

		if( dwReadBytes > 0 )
		{
			Buffer.HadCopied( dwReadBytes );
			if( pContent->OnCallbackBody( Buffer.Buffer(), Buffer.Size() ) == false )
			{
				return false;
			}

			Buffer.Clear();
		}
		else
		{
			pContent->OnCallbackComplete();
			break;
		}
	}

	return true;
}

bool	XHttpSyncDelegate::OpenRequest( HINTERNET hSession, const wchar_t* pVerb, const XHttpRequest& Request )
{
	const XHttpURL& URL = Request.URL();
	m_hConnect = ::WinHttpConnect( hSession, URL.Host().c_str(), URL.Port(), 0 );
	if( m_hConnect == NULL )
	{
		return false;
	}

	DWORD dwFlag = URL.IsSecure() ? WINHTTP_FLAG_SECURE : 0;
	m_hRequest = ::WinHttpOpenRequest(
					m_hConnect,
					pVerb,
					URL.Path().c_str(),
					NULL,
					(Request.Referer().empty() == true) ? WINHTTP_NO_REFERER : Request.Referer().c_str(),
					WINHTTP_DEFAULT_ACCEPT_TYPES,
					dwFlag );
	if( m_hRequest ==  NULL )
	{
		return false;
	}

	return true;
}

bool	XHttpSyncDelegate::SendRequest( const XHttpRequest& Request )
{
	XHttpUtil::XHttpSetRequestHeader Header( m_hRequest );
	if( Header.Add( Request.GetHeader() ) == false )
	{
		return false;
	}

	const XHttpBuffer& Buffer = Request.GetData();
	void* pData = const_cast< void* >( Buffer.Buffer() );
	DWORD nDataSize = static_cast< DWORD >( Buffer.Size() );
	if( ::WinHttpSendRequest(
			m_hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0,
			pData,
			nDataSize,
			nDataSize,
			NULL )
		== FALSE )
	{
		return false;
	}

	return true;
}
