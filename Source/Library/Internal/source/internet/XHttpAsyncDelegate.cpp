
#include "XHttpAsyncDelegate.h"
#include "../../include/internet/XHttpResponseHeader.h"
#include "XHttpUtil.h"


namespace
{

	void CALLBACK	WinHttpSystemCallback( HINTERNET hHandle, DWORD_PTR dwContext, DWORD dwStatus, void* pInfo, DWORD dwLength )
	{
		XHttpAsyncDelegate* pDelegate = reinterpret_cast< XHttpAsyncDelegate* >( dwContext );
		if( pDelegate != NULL )
		{
			pDelegate->OnSystemCallback( dwStatus, pInfo, dwLength );
		}
	}

};


XHttpAsyncDelegate::XHttpAsyncDelegate()
	: m_hConnect( NULL )
	, m_hRequest( NULL )
	, m_pContent( NULL )
	, m_pBuffer( NULL )
{
}

XHttpAsyncDelegate::~XHttpAsyncDelegate()
{
	Close();
}

void	XHttpAsyncDelegate::Close()
{
	if( m_hRequest != NULL )
	{
		::WinHttpSetStatusCallback( m_hRequest, NULL, NULL, NULL );
		::WinHttpCloseHandle( m_hRequest );
		m_hRequest = NULL;
	}

	if( m_hConnect != NULL )
	{
		::WinHttpCloseHandle( m_hConnect );
		m_hConnect = NULL;
	}

	m_pContent = NULL;

	if( m_pBuffer != NULL )
	{
		delete m_pBuffer;
		m_pBuffer = NULL;
	}
}

bool	XHttpAsyncDelegate::Execute(	HINTERNET hSession,
										const wchar_t* pVerb,
										const XHttpRequest& Request,
										XHttpContent* pContent,
										size_t nMinBufferSize )
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

	if( ::WinHttpSetStatusCallback(
			m_hRequest,
			WinHttpSystemCallback,
			WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS,
			0 )
		== WINHTTP_INVALID_STATUS_CALLBACK )
	{
		return false;
	}

	const XHttpBuffer& Buffer = Request.GetData();
	size_t nAllocSize = Buffer.Size();
	if( nAllocSize < nMinBufferSize )
	{
		nAllocSize = nMinBufferSize;
	}
	m_pBuffer = new XHttpBuffer( nAllocSize );
	m_pBuffer->Copy( Buffer.Buffer(), Buffer.Size() );

	m_pContent = pContent;

	XHttpUtil::XHttpSetRequestHeader Header( m_hRequest );
	if( Header.Add( Request.GetHeader() ) == false )
	{
		return false;
	}

	m_pBuffer->TurnBack();
	DWORD dwTotalSize = static_cast< DWORD >( m_pBuffer->Size() );
	if( ::WinHttpSendRequest(
			m_hRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0,
			WINHTTP_NO_REQUEST_DATA,
			0,
			dwTotalSize,
			reinterpret_cast< DWORD_PTR >( this ) )
		== FALSE )
	{
		return false;
	}

	return true;
}

void	XHttpAsyncDelegate::OnSystemCallback( DWORD dwStatus, void* pInfo, DWORD dwLength )
{
	try
	{
		if( DoSystemCallback( dwStatus, pInfo, dwLength ) == false )
		{
			delete this;
		}
	}
	catch( const XHttpUtil::XHttpException& except )
	{
		XHttpContent* pContent = m_pContent;
		if( pContent != NULL )
		{
			pContent->OnCallbackError(	XHttpUtil::GetErrorFunction( except.m_dwWhere ), except.m_dwError,
										except.m_szFunction, except.m_nLine );
		}

		delete this;
	}
}

bool	XHttpAsyncDelegate::DoSystemCallback( DWORD dwStatus, void* pInfo, DWORD dwLength )
{
	switch( dwStatus )
	{
	case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
	case WINHTTP_CALLBACK_STATUS_WRITE_COMPLETE:
		{
			if( pInfo != NULL && dwLength == sizeof( DWORD ) )
			{
				DWORD dwWritten = *(static_cast< DWORD* >( pInfo ));
				m_pBuffer->Flush( dwWritten );
			}

			if( m_pBuffer->Empty() == false )
			{
				if( ::WinHttpWriteData( m_hRequest, m_pBuffer->Buffer(), static_cast< DWORD >( m_pBuffer->Size() ), 0 ) == FALSE )
				{
					throw XHttpUtil::XHttpException( API_WRITE_DATA, ::GetLastError(), __FUNCTIONW__, __LINE__ );
				}
			}
			else
			{
				if( ::WinHttpReceiveResponse( m_hRequest, NULL ) == FALSE )
				{
					throw XHttpUtil::XHttpException( API_RECEIVE_RESPONSE, ::GetLastError(), __FUNCTIONW__, __LINE__ );
				}
			}
		}
		break;

	case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
		{
			XHttpResponseHeader Header( m_hRequest );
			DWORD dwStatusCode = Header.StatusCode();
			if( dwStatusCode != HTTP_STATUS_OK )
			{
				throw XHttpUtil::XHttpException( API_HEADER_STATUS_CODE, dwStatusCode, __FUNCTIONW__, __LINE__ );
			}

			XHttpContent* pContent = m_pContent;
			if( pContent == NULL || pContent->OnCallbackHeader( Header ) == false )
			{
				return false;
			}

			m_pBuffer->Clear();
			if( ::WinHttpReadData( m_hRequest, m_pBuffer->Buffer(), static_cast< DWORD >( m_pBuffer->MaxSize() ), NULL ) == FALSE )
			{
				throw XHttpUtil::XHttpException( API_READ_DATA, ::GetLastError(), __FUNCTIONW__, __LINE__ );
			}
		}
		break;

	case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
		{
			XHttpContent* pContent = m_pContent;
			if( pContent == NULL )
			{
				return false;
			}

			if( dwLength > 0 )
			{
				m_pBuffer->HadCopied( dwLength );
				if( pContent->OnCallbackBody( m_pBuffer->Buffer(), m_pBuffer->Size() ) == false )
				{
					return false;
				}

				m_pBuffer->Clear();
				if( ::WinHttpReadData( m_hRequest, m_pBuffer->Buffer(), static_cast< DWORD >( m_pBuffer->MaxSize() ), NULL ) == FALSE )
				{
					throw XHttpUtil::XHttpException( API_READ_DATA, ::GetLastError(), __FUNCTIONW__, __LINE__ );
				}
			}
			else
			{
				pContent->OnCallbackComplete();
				return false;
			}
		}
		break;

	case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
		{
			WINHTTP_ASYNC_RESULT* pResult = static_cast< WINHTTP_ASYNC_RESULT* >( pInfo );

			throw XHttpUtil::XHttpException( pResult->dwResult, pResult->dwError, __FUNCTIONW__, __LINE__ );
		}
		break;

	case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
		{
			throw XHttpUtil::XHttpException( API_HANDLE_CLOSING, ::GetLastError(), __FUNCTIONW__, __LINE__ );
		}
		break;
	}

	return true;
}
