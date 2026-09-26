
#include "../../include/internet/XHttpRequest.h"

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>


XHttpRequest::XHttpRequest( const XHttpURL& URL )
	: m_URL( URL )
{
	if( m_URL.Extra().empty() == false )
	{
		SetData( m_URL.Extra().c_str(), m_URL.Extra().size() * sizeof( std::wstring::value_type ) );
	}
}

XHttpRequest::~XHttpRequest()
{
}

const XHttpURL&	XHttpRequest::URL()	const
{
	return m_URL;
}

void	XHttpRequest::Referer( const std::wstring& strReferer )
{
	m_strReferer = strReferer;
}

const std::wstring&	XHttpRequest::Referer()	const
{
	return m_strReferer;
}

void	XHttpRequest::AddHeader( const std::wstring& strName, const std::wstring& strValue )
{
	m_mapHeader[strName] = strValue;
}

std::wstring	XHttpRequest::GetHeader()	const
{
	std::wstring strHeader;

	HEADER_MAP::const_iterator pos = m_mapHeader.begin();
	HEADER_MAP::const_iterator end = m_mapHeader.end();

	for( ; pos != end; ++pos )
	{
		strHeader += pos->first + L": " + pos->second + L"\r\n";
	}

	return strHeader;
}

void	XHttpRequest::Param( const XHttpRequestParam* pParam )
{
	std::wstring strParam = pParam->Build();
	size_t nParamSize = strParam.size() * sizeof( std::wstring::value_type );
	SetData( strParam.c_str(), nParamSize );
}

void	XHttpRequest::SetData( const void* pData, size_t nSize )
{
	m_Buffer.Copy( pData, nSize );
}

const XHttpBuffer&	XHttpRequest::GetData()	const
{
	return m_Buffer;
}
