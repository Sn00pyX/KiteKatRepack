
#include "../../include/internet/XInternetSession2.h"
#include "../../include/toolkit/safe_function.h"

#include <cstdio>
#include <cassert>
#include <shlwapi.h>

const int URL_STRING_LENGTH = 256;
std::string XInternetSession2Factory::s_strDefaultScheme = "http://";

XInternetSession2::XInternetSession2(void)
	: m_hSession( NULL )
	, m_hConnection( NULL )
	, m_dwLastError( 0 )
{
	m_hSession = InternetOpen( "XModule", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
	if( m_hSession == NULL )
	{
		m_dwLastError = ::GetLastError();
	}

	memset( &m_urlComponent, 0, sizeof( m_urlComponent ) );

	m_urlComponent.dwStructSize = sizeof( m_urlComponent );
	m_urlComponent.lpszHostName = new char[URL_STRING_LENGTH];
	m_urlComponent.dwHostNameLength = URL_STRING_LENGTH;
	m_urlComponent.lpszUserName = new char[URL_STRING_LENGTH];
	m_urlComponent.dwUserNameLength = URL_STRING_LENGTH;
	m_urlComponent.lpszPassword = new char[URL_STRING_LENGTH];
	m_urlComponent.dwPasswordLength = URL_STRING_LENGTH;
}

XInternetSession2::~XInternetSession2(void)
{
	Close();

	delete [] m_urlComponent.lpszHostName;
	delete [] m_urlComponent.lpszUserName;
	delete [] m_urlComponent.lpszPassword;

	if( m_hSession != NULL )
	{
		InternetCloseHandle( m_hSession );
		m_hSession = NULL;
	}
}

int XInternetSession2::DownloadFile( HINTERNET hService, XInternetFileWriter2* pStream )
{
	int nSize = 0;

	while( true )
	{
		DWORD dwBytesAvailable = 0;
		if( InternetQueryDataAvailable( hService, &dwBytesAvailable, 0, 0 ) == FALSE )
		{
			DWORD last_error = ::GetLastError();
			if( last_error != ERROR_NO_MORE_FILES )
			{
				m_dwLastError = last_error;
				nSize = -1;
			}

			break;
		}

		if( dwBytesAvailable == 0 )
			break;
	
		char* lpBuffer = new char[dwBytesAvailable];
	
		DWORD dwBytesRead = 0;
		if( InternetReadFile( hService, lpBuffer, dwBytesAvailable, &dwBytesRead ) == FALSE )
		{
			m_dwLastError = ::GetLastError();
			delete [] lpBuffer;
			break;
		}

		if( dwBytesRead == 0 )
		{
			delete [] lpBuffer;
			break;
		}

		nSize += dwBytesRead;
		if( pStream->Write( lpBuffer, dwBytesRead ) == false )
		{
			delete [] lpBuffer;
			break;
		}
		
		delete [] lpBuffer;
	}

	return nSize;
}

int XHTTPSession::Connect( const char *szServerAddr, int nPort, const char* szAccount, const char* szPassword )
{
	if( m_hSession == NULL )
	{
		return -1;
	}

	m_urlComponent.nPort = static_cast< INTERNET_PORT >( nPort );
	s_strcpy( m_urlComponent.lpszHostName, URL_STRING_LENGTH, szServerAddr );
	s_strcpy( m_urlComponent.lpszUserName, URL_STRING_LENGTH, szAccount );
	s_strcpy( m_urlComponent.lpszPassword, URL_STRING_LENGTH, szPassword );
	m_hConnection = InternetConnect( m_hSession,
		m_urlComponent.lpszHostName,
		m_urlComponent.nPort,
		m_urlComponent.lpszUserName,
		m_urlComponent.lpszPassword,
		INTERNET_SERVICE_HTTP, 0, NULL );

	if( m_hConnection == NULL )
	{
		m_dwLastError = ::GetLastError();
		return -1;
	}

	return 0;
}

template< typename _Elem >
void url_encode( std::basic_string< _Elem > *pString  )
{
	for( size_t i = 0; i < pString->size(); ++i )
	{
		bool bEncode = false;

		int nChar = (*pString)[i];

		// ASCII Control characters
		if( ( nChar >= 0x00 && nChar <= 0x1F ) || ( nChar == 0x7F ) ) bEncode = true;
			
		// Non-ASCII characters
		else if( nChar >= 0x80 && nChar <= 0xFF ) bEncode = true;

		// Resserved characters
		else if( ( nChar == 0x24 ) || ( nChar == 0x26 ) ||	
				 ( nChar == 0x2B ) || ( nChar == 0x2C ) ||
				 ( nChar == 0x27 ) || ( nChar == 0x3A ) ||
				 ( nChar == 0x28 ) || ( nChar == 0x29 ) ||
				 ( nChar == 0x3B ) || ( nChar == 0x3D ) ||
				 ( nChar == 0x3F ) || ( nChar == 0x40 ) ) bEncode = true;

		// Unsafe characters
		else if( ( nChar == 0x20 ) || ( nChar == 0x22 ) ||
				 ( nChar == 0x3C ) || ( nChar == 0x3E ) ||
				 ( nChar == 0x23 ) || ( nChar == 0x25 ) ) bEncode = true;

		// Misc
		else if( ( nChar == 0x7B ) || ( nChar == 0x7D ) ||
				 ( nChar == 0x7C ) || ( nChar == 0x5C ) ||
				 ( nChar == 0x5E ) || ( nChar == 0x7E ) ||
				 ( nChar == 0x5B ) || ( nChar == 0x5D ) ||
				 ( nChar == 0x60 ) ) bEncode = true;

		if( bEncode )
		{
			(*pString)[i++] = '%';
			int nHigh = nChar / 16;
			int nLow  = nChar % 16;	
			nHigh = nHigh > 9 ? 'A' + nHigh - 10 : '0' + nHigh;
			nLow = nLow > 9 ? 'A' + nLow - 10 : '0' + nLow;
			pString->insert( i++, 1, static_cast< _Elem >( nHigh ) );
			pString->insert( i, 1, static_cast< _Elem >( nLow ) );
		}
	}
}

int XHTTPSession::GetFile( const char* szObject, XInternetFileWriter2* pStream, std::string& real_query )
{
	std::string strObject = szObject;
	url_encode( &strObject );
	real_query = strObject;

	HINTERNET hRequest = HttpOpenRequest( m_hConnection, m_strVerb.c_str(), strObject.c_str(), NULL, NULL, NULL,
		INTERNET_FLAG_RELOAD | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_PRAGMA_NOCACHE, NULL );
	if( hRequest == NULL )
	{
		m_dwLastError = ::GetLastError();
		return -1;
	}

	// char *szHeader = "Content-Type: application/x-www-form-urlencoded\nKeep-Alive: 300\nProxy-Connection: keep-alive\n";
	if( HttpSendRequest( hRequest, NULL, 0, NULL, 0 ) == FALSE )
	{
		m_dwLastError = ::GetLastError();
		InternetCloseHandle( hRequest );
		hRequest = NULL;
		return -2;
	}

	DWORD dwStatusCode = 0;
	DWORD dwStatusCodeSize = sizeof( dwStatusCode );
	if( HttpQueryInfo( hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatusCode, &dwStatusCodeSize, 0 ) == FALSE )
	{
		m_dwLastError = ::GetLastError();
		InternetCloseHandle( hRequest );
		hRequest = NULL;
		return -3;
	}

	if( dwStatusCode != HTTP_STATUS_OK )
	{
		m_dwLastError = dwStatusCode;
		InternetCloseHandle( hRequest );
		hRequest = NULL;
		return -4;
	}

	/*
	DWORD dwContentLen = 0;
	DWORD dwContentLenSize = sizeof( dwContentLen );
	if( HttpQueryInfo( hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwContentLen, &dwContentLenSize, 0 ) == FALSE )
	{
		dwContentLen = 0;
	}
	*/

	int size = DownloadFile( hRequest, pStream );

	/*
	if( dwContentLen != 0 && static_cast< int >( dwContentLen ) != size )
	{
		return -5;
	}
	*/

	InternetCloseHandle( hRequest );
	hRequest = NULL;

	return size;
}


int XFTPSession::Connect( const char *szServerAddr, int nPort, const char* szAccount, const char* szPassword )
{
	if( m_hSession == NULL )
	{
		return -1;
	}

	m_urlComponent.nPort = static_cast< INTERNET_PORT >( nPort );
	s_strcpy( m_urlComponent.lpszHostName, URL_STRING_LENGTH, szServerAddr );
	s_strcpy( m_urlComponent.lpszUserName, URL_STRING_LENGTH, szAccount );
	s_strcpy( m_urlComponent.lpszPassword, URL_STRING_LENGTH, szPassword );
	m_hConnection = InternetConnect( m_hSession,
		m_urlComponent.lpszHostName,
		m_urlComponent.nPort,
		m_urlComponent.lpszUserName,
		m_urlComponent.lpszPassword,
		INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0 );

	if( m_hConnection == NULL )
	{
		m_dwLastError = ::GetLastError();
		return -1;
	}

	return 0;
}


int XFTPSession::GetFile( const char* szObject, XInternetFileWriter2* pStream, std::string& real_query )
{
	real_query = szObject;
	HINTERNET hRequest = FtpOpenFile( m_hConnection, szObject, GENERIC_READ, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD, 0 );
	if( !hRequest )
	{
		m_dwLastError = ::GetLastError();
		return -1;
	}

	int size = DownloadFile( hRequest, pStream );

	if( hRequest )
		InternetCloseHandle( hRequest ); 
	return size;
}


XInternetSession2* XInternetSession2Factory::CreateSessionAndConnect( const char* szURL )
{
	std::string strURL = szURL;
	XInternetSession2* pSession = NULL;

	std::string::size_type nPos = 0;

	nPos = strURL.find( "'ftp'" );
	if ( nPos != std::string::npos )
		strURL.replace( nPos, 5, "ftp://" );

	nPos = strURL.find( "'http'" );
	if ( nPos != std::string::npos )
		strURL.replace( nPos, 6, "http://" );

	if ( strURL.find( "://" ) == std::string::npos )
	{
		strURL = GetDefaultScheme();
		strURL += szURL;
	}

	URL_COMPONENTS urlComp;
	memset( &urlComp, 0, sizeof( urlComp ) );

	urlComp.dwStructSize = sizeof( urlComp );
	urlComp.lpszHostName = new char[URL_STRING_LENGTH];
	urlComp.dwHostNameLength = URL_STRING_LENGTH;
	urlComp.lpszUserName = new char[URL_STRING_LENGTH];
	urlComp.dwUserNameLength = URL_STRING_LENGTH;
	urlComp.lpszPassword = new char[URL_STRING_LENGTH];
	urlComp.dwPasswordLength = URL_STRING_LENGTH;

	if ( InternetCrackUrl( strURL.c_str(), static_cast< DWORD >( strURL.size() ), ICU_ESCAPE, &urlComp ) )
	{
		switch ( urlComp.nScheme )
		{
		case INTERNET_SCHEME_FTP :
			pSession = new XFTPSession(); break;

		case INTERNET_SCHEME_HTTP :
			pSession = new XHTTPSession(); break;
		}

		if( pSession != NULL )
		{
			if( pSession->Connect( urlComp.lpszHostName, urlComp.nPort, urlComp.lpszUserName, urlComp.lpszPassword ) != 0 )
			{
				delete pSession;
				pSession = NULL;
			}
		}
	}

	delete [] urlComp.lpszHostName;
	delete [] urlComp.lpszUserName;
	delete [] urlComp.lpszPassword;

	return pSession;
}

