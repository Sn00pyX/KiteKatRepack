#pragma once

// InternetSession.h
//
//    FTP 나 WWW등 URL로 지시 가능한 위치에 있는 파일 혹은 WWW 요청 결과를 다운로드함.
//
//         by Testors - testors@welovebeer.org

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wininet.h>

#include <vector>

struct XInternetFileWriter
{
	virtual bool Write( const char* pData, unsigned nSize ) = 0;
};

class XInternet
{
public:
	// { utility
	///   쿠키나 사용자 인증등을 필요로 하지 않는 일반적인 경우 아래의 유틸리티만으로 처리 가능.
	static int GetFile( const char* szURL, const char* szLocalFileName );
	static int GetFile( const char* szURL, char* pBuf, unsigned nBufSize );

	static int QueryHTTPRequest( const char* szHTTPURL, const char* szLocalFileName, const char* szPostData = NULL );
	static int QueryHTTPRequest( const char* szHTTPURL, char* pBuf, unsigned nBufSize, const char* szPostData = NULL );

	static int GetFile( const char* szURL, XInternetFileWriter* pStream );
	static int QueryHTTPRequest( const char* szHTTPURL, XInternetFileWriter* pStream, const char* szPostData = NULL );
	// }
};

class XInternetSession
{
public:
	// { common interface

	bool OpenSession();
	void CloseSession();

	/// 웹이나 FTP 등의 사용자 인증이 필요한 경우 query 나 connect 하기 전에 정보를 세팅해 주어야 한다.
	void SetAuthInfo( const char* szAccount = NULL, const char* szPassword = NULL );
	bool SetServer( const char* serverIP, unsigned port = 0 );

	// }


	// { HTTP
	int  HttpQuery( const char* szFilename, const char* szLocalFileName, const char* szPostData = NULL );
	int  HttpQuery( const char* szFilename, XInternetFileWriter* pStream, const char* szPostData = NULL );
	// }


	// { FTP
	// 구현중
	bool FtpConnect();
	void FtpClose();

	bool FtpRenameFile( const char* szRemoteFileName, const char* szNewRemoteFileName );
	bool FtpDeleteFile( const char* szRemoteFileName );
	int  FtpGetFile( const char* szRemoteFileName, const char* szLocalFileName );
	int  FtpPutFile( const char* szLocalFileName, const char* szRemoteFileName );

	bool FtpChangeDirectory( const char* szDirectoryName );
	bool FtpMakeDirectory( const char* szDirectoryName );
	bool FtpRemoveDirectory( const char* szDirectoryName );
	bool FtpGetCurrentDirectory( char* szBuf, unsigned len );
	bool FtpGetFileList( std::vector< WIN32_FIND_DATA >& storage );
	// }


public:
	// constructor / destructor
	XInternetSession() { m_hSession = NULL; m_hHttpConnection = NULL; m_hFtpConnection = NULL; m_pszServer = NULL; m_nPort = 80; m_pszAccount = m_pszPassword = NULL; }
	virtual ~XInternetSession() { Close(); }


private:

	int queryHTTPSession( HINTERNET hSession, const char* szHTTPURL, const char* szLocalFileName, char* pBuf, unsigned nBufSize, void* pStream, const char* szPostData, const char* szAccount, const char* szPassword );

	void Close();

	HINTERNET m_hSession;
	HINTERNET m_hFtpConnection;
	HINTERNET m_hHttpConnection;
	char* m_pszServer;
	char* m_pszAccount;
	char* m_pszPassword;
	unsigned m_nPort;

};