// InternetSession.cpp
//
//    FTP 나 WWW등 URL로 지시 가능한 위치에 있는 파일 혹은 WWW 요청 결과를 다운로드함.
//
// Created on 2001/09/09
//         by Testors - testors@welovebeer.org

#include "../../include/internet/XInternet.h"
#include "../../include/toolkit/safe_function.h"

#include <cstdio>
#include <cassert>

#pragma comment(lib, "wininet.lib")


static int downloadFile( HINTERNET hService, const char *szLocalFileName, char* pBuf, unsigned nBufSize, void* pStream );
static int getURL( HINTERNET hSession, const char* szURL, const char* szLocalFileName, char* pBuf, unsigned nBufSize, void* pStream );
static int queryHTTP( HINTERNET hSession, const char* szHTTPURL, const char* szLocalFileName, char* pBuf, unsigned nBufSize, void* pStream, const char* szPostData, const char* szAccount, const char* szPassword );


int XInternet::GetFile( const char* szURL, const char* szLocalFileName )
{
	return getURL( NULL, szURL, szLocalFileName, NULL, 0, NULL );
}

int XInternet::GetFile( const char* szURL, char* pBuf, unsigned nBufSize )
{
	return getURL( NULL, szURL, NULL, pBuf, nBufSize, NULL );

}

int XInternet::QueryHTTPRequest( const char* szHTTPURL, const char* szLocalFileName, const char* szPostData )
{
	return queryHTTP( NULL, szHTTPURL, szLocalFileName, NULL, 0, NULL, szPostData, NULL, NULL );	
}

int XInternet::QueryHTTPRequest( const char* szHTTPURL, char* pBuf, unsigned nBufSize, const char* szPostData )
{
	return queryHTTP( NULL, szHTTPURL, NULL, pBuf, nBufSize, NULL, szPostData, NULL, NULL );
}

int XInternet::GetFile( const char* szURL, XInternetFileWriter* pStream )
{
	return getURL( NULL, szURL, NULL, NULL, 0, pStream );
}

int XInternet::QueryHTTPRequest( const char* szHTTPURL, XInternetFileWriter* pStream, const char* szPostData )
{
	return queryHTTP( NULL, szHTTPURL, NULL, NULL, 0, pStream, szPostData, NULL, NULL );
}


bool XInternetSession::SetServer( const char* serverIP, unsigned port )
{
	if( m_pszServer ) delete [] m_pszServer;

	size_t server_len = strlen(serverIP)+1;
	m_pszServer = new char[server_len];
	s_strcpy(m_pszServer, server_len, serverIP);
	m_nPort = port;

	return true;
}

void XInternetSession::SetAuthInfo( const char* szAccount, const char* szPassword )
{
	if( m_pszAccount ) delete [] m_pszAccount;
	if( m_pszPassword ) delete [] m_pszPassword;

	size_t account_len = strlen( szAccount ) +1;
	size_t password_len = strlen( szPassword ) +1;
	m_pszAccount = new char[ account_len ];
	m_pszPassword = new char[ password_len ];

	s_strcpy( m_pszAccount, account_len, szAccount );
	s_strcpy( m_pszPassword, password_len, szPassword );
}

bool XInternetSession::OpenSession()
{
	if( !m_hSession ) m_hSession = InternetOpen("MyApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if( !m_hSession ) return false;
	return true;
}

void XInternetSession::CloseSession()
{	
	if( m_hSession ) {
		InternetCloseHandle( m_hSession );
		m_hSession = NULL;
	}
	Close();
}

int XInternetSession::HttpQuery( const char* szFilename, XInternetFileWriter* pStream, const char* szPostData )
{
	if( !m_hSession ) return -1;
	if( !m_pszServer ) return -1;

	size_t url_len = strlen(m_pszServer)+strlen(szFilename)+9;
	char* szURL = new char[url_len];
	s_strcpy(szURL, url_len, "http://");
	s_strcat(szURL, url_len, m_pszServer);
	if( szFilename[0] != '/' ) s_strcat(szURL, url_len, "/");
	s_strcat(szURL, url_len, szFilename);
	int size = queryHTTPSession( m_hSession, szURL, NULL, NULL, 0, pStream, szPostData, m_pszAccount, m_pszPassword);
	delete [] szURL;
	return size;
}

int XInternetSession::HttpQuery( const char* szFilename, const char* szLocalFileName, const char* szPostData )
{
	if( !m_hSession ) return -1;
	if( !m_pszServer ) return -1;

	size_t url_len = strlen(m_pszServer)+strlen(szFilename)+9;
	char* szURL = new char[url_len];
	s_strcpy(szURL, url_len, "http://");
	s_strcat(szURL, url_len, m_pszServer);
	if( szFilename[0] != '/' ) s_strcat(szURL, url_len, "/");
	s_strcat(szURL, url_len, szFilename);
	int size = queryHTTPSession( m_hSession, szURL, szLocalFileName, NULL, 0, NULL, szPostData, m_pszAccount, m_pszPassword);
	delete [] szURL;
	return size;
}

bool XInternetSession::FtpConnect()
{
	if( m_hFtpConnection ) assert( 0 );

	m_hFtpConnection = InternetConnect( m_hSession, m_pszServer, static_cast< INTERNET_PORT >( m_nPort ), m_pszAccount, m_pszPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE , 0 );

	if( !m_hFtpConnection ) return false;

	return true;
}

void XInternetSession::FtpClose()
{
	InternetCloseHandle( m_hFtpConnection );
	m_hFtpConnection = NULL;
}

bool XInternetSession::FtpRenameFile( const char* szRemoveFileName, const char* szNewRemoteFileName )
{
	if( ! ::FtpRenameFile( m_hFtpConnection, szRemoveFileName, szNewRemoteFileName ) ) return false;

	return true;
}

bool XInternetSession::FtpDeleteFile( const char* szRemoveFileName )
{
	if( ! ::FtpDeleteFile( m_hFtpConnection, szRemoveFileName ) ) return false;

	return true;
}

int  XInternetSession::FtpGetFile( const char* szRemoveFileName, const char* szLocalFileName )
{
	if( ! ::FtpGetFile( m_hFtpConnection, szRemoveFileName, szLocalFileName, false, 0, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 0 ) ) return false;

	return true;
}

int  XInternetSession::FtpPutFile( const char* szLocalFileName, const char* szRemoveFileName )
{
	if( ! ::FtpPutFile( m_hFtpConnection, szLocalFileName, szRemoveFileName, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, 0 ) ) return false;

	return true;
}

bool XInternetSession::FtpChangeDirectory( const char* szDirectoryName )
{
	if( ! ::FtpSetCurrentDirectory( m_hFtpConnection, szDirectoryName ) ) return false;

	return true;
}

bool XInternetSession::FtpMakeDirectory( const char* szDirectoryName )
{
	if( ! ::FtpCreateDirectory( m_hFtpConnection, szDirectoryName ) ) return false;

	return true;
}

bool XInternetSession::FtpRemoveDirectory( const char* szDirectoryName )
{
	if( ! ::FtpRemoveDirectory( m_hFtpConnection, szDirectoryName ) ) return false;

	return true;
}

bool XInternetSession::FtpGetCurrentDirectory( char* szBuf, unsigned len )
{
	DWORD rtn = len;
	if( ! ::FtpGetCurrentDirectory( m_hFtpConnection, szBuf, &rtn ) ) return false;

	return true;
}

bool XInternetSession::FtpGetFileList( std::vector< WIN32_FIND_DATA >& storage )
{
	WIN32_FIND_DATA data;
	HINTERNET findHandle = FtpFindFirstFile( m_hFtpConnection, NULL, &data, INTERNET_FLAG_RELOAD, 0 );

	while ( findHandle )
	{
		storage.push_back( data );
		if( !InternetFindNextFile( findHandle, &data ) ) break;
	}

	if( findHandle ) InternetCloseHandle( findHandle );

	if( GetLastError() == ERROR_NO_MORE_FILES ) return true;

	return false;
}



void XInternetSession::Close()
{
	if( m_hHttpConnection ) InternetCloseHandle( m_hHttpConnection );
	if( m_hFtpConnection ) InternetCloseHandle( m_hFtpConnection );
	if( m_hSession ) InternetCloseHandle( m_hSession );

	if( m_pszServer ) delete []  m_pszServer, m_pszServer = NULL;
	if( m_pszAccount ) delete [] m_pszAccount, m_pszAccount = NULL;
	if( m_pszPassword ) delete [] m_pszPassword, m_pszPassword = NULL;
}





//////////////////////////////////////////////////////////////////////////////
// 이하 전용 static utility 함수들
// 볼 필요 없음.


// Function : downloadFile
// Remark   : 해당 핸들로부터 파일을 다운로드.
//            szLocalFileName 이 NULL 이 아니면 해당 파일 이름으로 저장된다.
//            pBuf 가 NULL 이 아니고 nBufSize 가 0 이상이면 해당 버퍼로 복사된다.
static int downloadFile( HINTERNET hService, const char *szLocalFileName, char* pBuf, unsigned nBufSize, void* pStream )
{
	char* lpBuffer = NULL;
	char *pPtr = pBuf;
	int nSize = 0;
	FILE *fp = NULL;

	DWORD dwBytesAvailable, dwBytesRead;

	while( true )
	{
		if( !InternetQueryDataAvailable( hService, &dwBytesAvailable, 0, 0 ) )
		{
			if( GetLastError() != ERROR_NO_MORE_FILES )
				nSize = -1;

			break;
		}

		if( dwBytesAvailable == 0 )
			break;

		if( szLocalFileName && !fp )
		{
			fopen_s( &fp, szLocalFileName, "wb" );
			if( fp == NULL )
			{
				nSize = -1;
				break;
			}

			nSize = 0;
		}

		if( pPtr == NULL )
		{
			lpBuffer = new char[dwBytesAvailable];
			pPtr = lpBuffer;
		}
		else
		{	
			if( nBufSize < dwBytesAvailable )
			{
				dwBytesAvailable = nBufSize;
			}

			nBufSize -= dwBytesAvailable;
		}

		InternetReadFile( hService, pPtr, dwBytesAvailable, &dwBytesRead );
		nSize += dwBytesRead;

		// 버퍼가 제공된 경우
		if( pPtr != lpBuffer )
		{
			pPtr += dwBytesRead;
			// 버퍼에 여유공간이 없다면 끝
			if( nBufSize <= 0 )
			{
				break;
			}
		}

		if( fp )
		{
			if( fwrite( pPtr, dwBytesRead, 1, fp ) != 1 )
			{
				nSize = -1;
				break;
			}
		}

		if( pStream )
		{
			XInternetFileWriter* pXInternetFileWriter = static_cast< XInternetFileWriter* >( pStream );
			if( !pXInternetFileWriter->Write( lpBuffer, dwBytesRead ) )
				break;
		}

		if( lpBuffer )
		{
			// 여기서 할당한 버퍼라면,
			if( pPtr == lpBuffer )
			{
				pPtr = NULL;
			}

			delete [] lpBuffer;
			lpBuffer = NULL;
		}		
	}

	if( fp )
		fclose( fp );

	return nSize;
}


// Function : getURL
// Remark   : szLocalFileName 이 있다면 해당 파일에 다운로드.
//            pBuf 가 NULL 이 아니라면 해당 메모리 블럭에 카피.
//            사이즈를 리턴함. 에러일경우 음수 리턴.
static int getURL( HINTERNET hSession, const char* szURL, const char* szLocalFileName, char* pBuf, unsigned nBufSize, void* pStream )
{
	if( pBuf && nBufSize < 1 ) return -1;

	if( !pBuf ) nBufSize = 0;

	bool bIsNewSession = false;
	if( !hSession ) {
		hSession = InternetOpen("MyApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		bIsNewSession = true;
	}

	if( !hSession ) return -1;

	HINTERNET hService;
	hService = InternetOpenUrl(hSession, szURL, NULL, 0, INTERNET_FLAG_RAW_DATA | INTERNET_FLAG_RELOAD, 0);

	if ( !hService ) {
		if( bIsNewSession ) InternetCloseHandle( hSession );
		return -1;
	}

	int nSize = downloadFile( hService, szLocalFileName, pBuf, nBufSize, pStream );

	if( bIsNewSession ) InternetCloseHandle( hSession ); 

	return nSize;
}

static int queryHTTP( HINTERNET hSession, const char* szHTTPURL, const char* szLocalFileName, char* pBuf, unsigned nBufSize, void* pStream, const char* szPostData, const char* szAccount, const char* szPassword )
{

	char* pServerAddr;
	char* pObject;
	unsigned nPort = 80;

	// { URL 파싱
	if( _strnicmp( szHTTPURL, "http://", 7 ) ) return -1;
	const char *p = strstr( &szHTTPURL[7], "/" );

	// server 주소
	if( !p ) {
		p = const_cast< char* >( &szHTTPURL[ strlen( szHTTPURL ) ] );
	}
	if( p-szHTTPURL-7 < 1 ) return -1;

	const char *sep = strstr( &szHTTPURL[7], ":" );
	if( sep && sep < p ) {
		// 포트 얻자
		char szPort[100] = {0,};
		sep++;
		s_strncpy( szPort, _countof( szPort ), sep, p-sep );
		nPort = atoi( szPort );

		size_t addr_len = p-szHTTPURL-7-strlen(szPort);
		pServerAddr = new char[addr_len];
		s_strncpy( pServerAddr, addr_len, &szHTTPURL[7], p-szHTTPURL-8-strlen(szPort) );
	} else {
		size_t addr_len = p-szHTTPURL-6;
		pServerAddr = new char[ addr_len ];
		s_strncpy( pServerAddr, addr_len, &szHTTPURL[7], p-szHTTPURL-7 );
	}

	// object
	if( !(*p) ) {
		pObject = new char[2];
		pObject[0]='/';
		pObject[1]='\0';
	} else {
		size_t obj_len = strlen( p ) + 1;
		pObject = new char[ obj_len ];
		s_strcpy( pObject, obj_len, p );
	}
	// }

	HINTERNET hRequest, hResource;

	bool bIsNewSession = false;
	if( !hSession ) {
		hSession = InternetOpen("MyApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
		bIsNewSession = true;
	}

	if( !hSession ) {
		assert(0);
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	hResource = InternetConnect( hSession, pServerAddr, static_cast< INTERNET_PORT >( nPort ), szAccount, szPassword, INTERNET_SERVICE_HTTP, 0, 0 );
	if( !hResource ) {
		assert(0);
		if( bIsNewSession ) InternetCloseHandle( hSession ); 
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	hRequest = HttpOpenRequest( hResource, ( szPostData ? "POST" : "GET" ) , pObject, NULL, NULL, NULL, INTERNET_FLAG_RELOAD, NULL );
	if( !hRequest ) {
		assert(0);
		if( bIsNewSession ) InternetCloseHandle( hSession ); 
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	char *szHeader = "Content-Type: application/x-www-form-urlencoded";
	int bRtn = HttpSendRequest( hRequest, szHeader, (DWORD)strlen( szHeader ), const_cast< char* >( szPostData ), (DWORD)( szPostData ? strlen( szPostData ) : 0 ) );	// post 데이터
	if( !bRtn ) {
		assert(0);
		if( bIsNewSession ) InternetCloseHandle( hSession ); 
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	char lpvSomeBuffer[1024] = {0, };
	DWORD dwSize = 1024;
	if( HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS, lpvSomeBuffer, &dwSize, NULL) == FALSE )
	{
		if( bIsNewSession ) InternetCloseHandle( hSession ); 
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	if( !strstr( lpvSomeBuffer, "200" ) ) {
		if( bIsNewSession ) InternetCloseHandle( hSession ); 
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	int size = downloadFile( hRequest, szLocalFileName, pBuf, nBufSize, pStream );

	delete [] pServerAddr;
	delete [] pObject;

	if( bIsNewSession ) InternetCloseHandle( hSession ); 

	return size;
}

int XInternetSession::queryHTTPSession( HINTERNET hSession, const char* szHTTPURL, const char* szLocalFileName, char* pBuf, unsigned nBufSize, void* pStream, const char* szPostData, const char* szAccount, const char* szPassword )
{
	char* pServerAddr;
	char* pObject;
	unsigned nPort = 80;

	// { URL 파싱
	if( _strnicmp( szHTTPURL, "http://", 7 ) ) return -1;
	const char *p = strstr( &szHTTPURL[7], "/" );

	// server 주소
	if( !p ) {
		p = const_cast< char* >( &szHTTPURL[ strlen( szHTTPURL ) ] );
	}
	if( p-szHTTPURL-7 < 1 ) return -1;

	const char *sep = strstr( &szHTTPURL[7], ":" );
	if( sep && sep < p ) {
		// 포트 얻자
		char szPort[100] = {0,};
		sep++;
		s_strncpy( szPort, _countof( szPort ), sep, p-sep );
		nPort = atoi( szPort );

		size_t addr_len = p-szHTTPURL-7-strlen(szPort);
		pServerAddr = new char[addr_len];
		s_strncpy( pServerAddr, addr_len, &szHTTPURL[7], p-szHTTPURL-8-strlen(szPort) );
	} else {
		size_t addr_len = p-szHTTPURL-6;
		pServerAddr = new char[addr_len];
		s_strncpy( pServerAddr, addr_len, &szHTTPURL[7], p-szHTTPURL-7 );
	}

	// object
	if( !(*p) ) {
		pObject = new char[2];
		pObject[0]='/';
		pObject[1]='\0';
	} else {
		size_t obj_len = strlen( p ) + 1;
		pObject = new char[ obj_len ];
		s_strcpy( pObject, obj_len, p );
	}
	// }

	if( !m_hSession ) {
		m_hSession = InternetOpen("MyApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	}

	if( !m_hSession ) {
		assert(0);
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	HINTERNET hRequest = NULL;

	m_hHttpConnection = InternetConnect( hSession, pServerAddr, static_cast< INTERNET_PORT >( nPort ), szAccount, szPassword, INTERNET_SERVICE_HTTP, 0, 0 );
	if( !m_hHttpConnection ) {
		assert(0);
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	hRequest = HttpOpenRequest( m_hHttpConnection, ( szPostData ? "POST" : "GET" ) , pObject, NULL, NULL, NULL, INTERNET_FLAG_RELOAD, NULL );
	if( !hRequest ) {
		assert(0);
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	char *szHeader = "Content-Type: application/x-www-form-urlencoded\nKeep-Alive: 300\nProxy-Connection: keep-alive\n";
	int bRtn = HttpSendRequest( hRequest, szHeader, (DWORD)strlen( szHeader ), const_cast< char* >( szPostData ), (DWORD)( szPostData ? strlen( szPostData ) : 0 ) );	// post 데이터
	if( !bRtn ) {
		assert(0);
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	char lpvSomeBuffer[1024] = {0, };
	DWORD dwSize = 1024;
	if( HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS, lpvSomeBuffer, &dwSize, NULL) == FALSE )
	{
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	if( !strstr( lpvSomeBuffer, "200" ) ) {
		delete [] pServerAddr;
		delete [] pObject;
		return -1;
	}

	int size = downloadFile( hRequest, szLocalFileName, pBuf, nBufSize, pStream );

	delete [] pServerAddr;
	delete [] pObject;

	if( hRequest ) InternetCloseHandle( hRequest ); 

	return size;
}