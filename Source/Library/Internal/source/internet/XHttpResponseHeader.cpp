
#include "../../include/internet/XHttpResponseHeader.h"


XHttpResponseHeader::XHttpResponseHeader( HINTERNET hRequest )
	: m_hRequest( hRequest )
{
}

XHttpResponseHeader::~XHttpResponseHeader()
{
	m_hRequest = NULL;
}

DWORD	XHttpResponseHeader::StatusCode()	const
{
	DWORD dwCode = 0;
	DWORD dwCodeSize = sizeof( dwCode );
	::WinHttpQueryHeaders(
		m_hRequest,
		WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&dwCode,
		&dwCodeSize,
		WINHTTP_NO_HEADER_INDEX );

	return dwCode;
}

std::wstring	XHttpResponseHeader::ContentType()	const
{
	std::wstring strContentType;

	DWORD dwContentTypeSize = 0;
	if( ::WinHttpQueryHeaders(
			m_hRequest,
			WINHTTP_QUERY_CONTENT_TYPE,
			WINHTTP_HEADER_NAME_BY_INDEX,
			NULL,
			&dwContentTypeSize,
			WINHTTP_NO_HEADER_INDEX )
		== FALSE )
	{
		if( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
		{
			size_t nContentTypeLen = dwContentTypeSize / sizeof( wchar_t );
			wchar_t* temp = new wchar_t[nContentTypeLen];

			if( ::WinHttpQueryHeaders(
					m_hRequest,
					WINHTTP_QUERY_CONTENT_TYPE,
					WINHTTP_HEADER_NAME_BY_INDEX,
					temp,
					&dwContentTypeSize,
					WINHTTP_NO_HEADER_INDEX )
				!= FALSE )
			{
				temp[dwContentTypeSize/sizeof( wchar_t )] = L'\0';
				strContentType = temp;
			}

			delete [] temp;
		}
	}

	return strContentType;
}

size_t	XHttpResponseHeader::ContentLength()	const
{
	wchar_t szContentLen[64] = { 0, };
	DWORD dwContentLenSize = sizeof( szContentLen );
	if( ::WinHttpQueryHeaders(
			m_hRequest,
			WINHTTP_QUERY_CONTENT_LENGTH,
			WINHTTP_HEADER_NAME_BY_INDEX,
			szContentLen,
			&dwContentLenSize,
			WINHTTP_NO_HEADER_INDEX )
		!= FALSE )
	{
		szContentLen[dwContentLenSize/sizeof( szContentLen[0] )] = L'\0';
		unsigned long long n64ContentSize = ::_wcstoui64( szContentLen, NULL, 10 );

		return size_t( n64ContentSize );
	}

	return 0;
}

SYSTEMTIME	XHttpResponseHeader::ModifiedTime()	const
{
	SYSTEMTIME modified;
	DWORD dwSystemTimeSize = sizeof( modified );
	if( ::WinHttpQueryHeaders(
			m_hRequest,
			WINHTTP_QUERY_LAST_MODIFIED | WINHTTP_QUERY_FLAG_SYSTEMTIME,
			WINHTTP_HEADER_NAME_BY_INDEX,
			&modified,
			&dwSystemTimeSize,
			WINHTTP_NO_HEADER_INDEX )
		== FALSE )
	{
		::GetSystemTime( &modified );
	}

	return modified;
}

std::wstring	XHttpResponseHeader::Get()	const
{
	std::wstring strHeader;

	DWORD dwHeaderSize = 0;
	if( ::WinHttpQueryHeaders(
			m_hRequest,
			WINHTTP_QUERY_RAW_HEADERS_CRLF,
			WINHTTP_HEADER_NAME_BY_INDEX,
			NULL,
			&dwHeaderSize,
			WINHTTP_NO_HEADER_INDEX )
		== FALSE )
	{
		if( ::GetLastError() == ERROR_INSUFFICIENT_BUFFER )
		{
			size_t nHeaderLen = dwHeaderSize / sizeof( wchar_t );
			wchar_t* temp = new wchar_t[nHeaderLen];

			if( ::WinHttpQueryHeaders(
					m_hRequest,
					WINHTTP_QUERY_RAW_HEADERS_CRLF,
					WINHTTP_HEADER_NAME_BY_INDEX,
					temp,
					&dwHeaderSize,
					WINHTTP_NO_HEADER_INDEX )
				!= FALSE )
			{
				temp[dwHeaderSize/sizeof( wchar_t )] = L'\0';
				strHeader = temp;
			}

			delete [] temp;
		}
	}

	return strHeader;
}


