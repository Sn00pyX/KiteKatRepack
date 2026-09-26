#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wininet.h>
#include <string>


class XInternetFileWriter2
{
public:
	virtual ~XInternetFileWriter2() {};
	virtual bool Write( const char* pData, unsigned nSize ) = 0;
};


class XInternetSession2
{
public:
	XInternetSession2(void);
	virtual ~XInternetSession2(void);

	virtual int Connect( const char* szURL, int nPort, const char* szAccount, const char* szPassword ) = 0;
	virtual int GetFile( const char* szObject, XInternetFileWriter2* pStream, std::string& real_query ) = 0;
	DWORD	GetErrorNumber()	const
	{
		return m_dwLastError;
	}

	void Close()	// 이 함수는 절대 절대 virtual 함수가 되어서는 안된다!
	{
		if( m_hConnection != NULL )
		{
			InternetCloseHandle( m_hConnection );
			m_hConnection = NULL;
		}
	}

	int DownloadFile( HINTERNET hService, XInternetFileWriter2* pStream );

protected:
	HINTERNET m_hSession;
	HINTERNET m_hConnection;
	URL_COMPONENTS m_urlComponent;
	DWORD m_dwLastError;

private:
	XInternetSession2( const XInternetSession2& other );
};


class XHTTPSession : public XInternetSession2
{
public:
	XHTTPSession()
		: m_strVerb( "GET" ) 
	{
		m_urlComponent.nScheme = INTERNET_SCHEME_HTTP;
	}

	virtual int Connect( const char* szURL, int nPort, const char* szAccount, const char* szPassword );
	virtual int GetFile( const char* szObject, XInternetFileWriter2* pStream, std::string& real_query );

	void SetVerb( const char* szVerb ) { m_strVerb = szVerb; }
	const char* GetVerb() { return m_strVerb.c_str(); }

	virtual void UrlEncode( std::string& strURL )
	{
		DWORD dwSize = 512;
		char szTemp[512];

		if ( InternetCanonicalizeUrl( strURL.c_str(), szTemp, &dwSize, ICU_BROWSER_MODE ) == TRUE )
			strURL = szTemp;
	}

private:
	std::string m_strVerb;

};


class XFTPSession : public XInternetSession2
{
public:
	XFTPSession()
	{
		m_urlComponent.nScheme = INTERNET_SCHEME_FTP;
	}
	virtual int Connect( const char* szURL, int nPort, const char* szAccount, const char* szPassword );
	virtual int GetFile( const char* szObject, XInternetFileWriter2* pStream, std::string& real_query );
};


class XInternetSession2Factory
{
public:
	static XInternetSession2* CreateSessionAndConnect( const char* szURL );
	static void SetDefaultScheme( const char* szScheme )
	{ s_strDefaultScheme = szScheme; }
	static const char* GetDefaultScheme()
	{ return s_strDefaultScheme.c_str(); }

	static std::string s_strDefaultScheme;
};