
#include "../../include/internet/XHttpURL.h"

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>


XHttpURL::XHttpURL()
{
	Clear();
}

XHttpURL::XHttpURL( const std::wstring& strURL )
{
	Set( strURL.c_str() );
}

XHttpURL::XHttpURL( const std::string& strURL )
{
	Set( strURL.c_str() );
}

XHttpURL::XHttpURL( const wchar_t* pURL )
{
	Set( pURL );
}

XHttpURL::XHttpURL( const char* pURL )
{
	Set( pURL );
}

bool	XHttpURL::Set( const wchar_t* pURL )
{
	URL_COMPONENTS urlcomp;
	::memset( &urlcomp, 0, sizeof( urlcomp ) );
	urlcomp.dwStructSize = sizeof( urlcomp );

	urlcomp.dwSchemeLength		= static_cast< DWORD >( -1 );
	urlcomp.dwHostNameLength	= static_cast< DWORD >( -1 );
	urlcomp.dwUrlPathLength		= static_cast< DWORD >( -1 );
	urlcomp.dwExtraInfoLength	= static_cast< DWORD >( -1 );

	if( ::WinHttpCrackUrl( pURL, 0, 0, &urlcomp ) == FALSE )
	{
		Clear();
		return false;
	}

	m_strScheme.assign( urlcomp.lpszScheme, urlcomp.dwSchemeLength );
	m_bIsSecure = (urlcomp.nScheme == INTERNET_SCHEME_HTTPS);

	m_strHost.assign( urlcomp.lpszHostName, urlcomp.dwHostNameLength );
	m_nPort = urlcomp.nPort;

	m_strPath.assign( urlcomp.lpszUrlPath, urlcomp.dwUrlPathLength );
	m_strExtra.assign( urlcomp.lpszExtraInfo, urlcomp.dwExtraInfoLength );
	return true;
}

bool	XHttpURL::Set( const char* pURL )
{
	int nResult = ::MultiByteToWideChar( CP_ACP, 0, pURL, -1, NULL, 0 );
	if( nResult > 0 )
	{
		bool bRet = false;
		wchar_t* temp = new wchar_t[nResult+1];
		temp[0] = L'\0';
		nResult = ::MultiByteToWideChar( CP_ACP, 0, pURL, -1, temp, nResult );
		if( nResult > 0 )
		{
			bRet = Set( temp );
		}

		delete [] temp;
		temp = NULL;

		return bRet;
	}

	return false;
}

void	XHttpURL::Clear()
{
	m_strScheme = L"http";
	m_bIsSecure = false;

	m_strHost.clear();
	m_nPort = INTERNET_DEFAULT_HTTP_PORT;

	m_strPath.clear();
	m_strExtra.clear();
}

std::wstring	XHttpURL::Build()	const
{
	URL_COMPONENTS urlcomp;
	::memset( &urlcomp, 0, sizeof( urlcomp ) );
	urlcomp.dwStructSize = sizeof( urlcomp );

	urlcomp.lpszScheme			= const_cast< wchar_t* >( m_strScheme.c_str() );
	urlcomp.dwSchemeLength		= static_cast< DWORD >( m_strScheme.size() );
	urlcomp.nScheme				= m_bIsSecure ? INTERNET_SCHEME_HTTPS : INTERNET_SCHEME_HTTP;

	urlcomp.lpszHostName		= const_cast< wchar_t* >( m_strHost.c_str() );
	urlcomp.dwHostNameLength	= static_cast< DWORD >( m_strHost.size() );
	urlcomp.nPort				= m_nPort;

	urlcomp.dwUserNameLength	= static_cast< DWORD >( -1 );
	urlcomp.dwPasswordLength	= static_cast< DWORD >( -1 );

	urlcomp.lpszUrlPath			= const_cast< wchar_t* >( m_strPath.c_str() );
	urlcomp.dwUrlPathLength		= static_cast< DWORD >( m_strPath.size() );
	urlcomp.lpszExtraInfo		= const_cast< wchar_t* >( m_strExtra.c_str() );
	urlcomp.dwExtraInfoLength	= static_cast< DWORD >( m_strExtra.size() );

	std::wstring strURL;
	DWORD dwURLLen = 0;
	::WinHttpCreateUrl( &urlcomp, 0, NULL, &dwURLLen );

	if( dwURLLen > 0 )
	{
		wchar_t* temp = new wchar_t[dwURLLen];

		if( ::WinHttpCreateUrl( &urlcomp, 0, temp, &dwURLLen ) != FALSE )
		{
			strURL = temp;
		}

		delete [] temp;
	}

	return strURL;
}

const std::wstring&	XHttpURL::Scheme()	const
{
	return m_strScheme;
}

bool	XHttpURL::IsSecure()	const
{
	return m_bIsSecure;
}

const std::wstring&	XHttpURL::Host()	const
{
	return m_strHost;
}

unsigned short	XHttpURL::Port()	const
{
	return m_nPort;
}

const std::wstring&	XHttpURL::Path()	const
{
	return m_strPath;
}

const std::wstring&	XHttpURL::Extra()	const
{
	return m_strExtra;
}

void	XHttpURL::Scheme( const std::wstring& strScheme )
{
	m_strScheme = strScheme;
}

void	XHttpURL::Secure( bool bIsSecure )
{
	m_bIsSecure = bIsSecure;
}

void	XHttpURL::Host( const std::wstring& strHost )
{
	m_strHost = strHost;
}

void	XHttpURL::Port( unsigned short nPort )
{
	m_nPort = nPort;
}

void	XHttpURL::Path( const std::wstring& strPath )
{
	m_strPath = strPath;
}

void	XHttpURL::Extra( const std::wstring& strExtra )
{
	m_strExtra = strExtra;
}

