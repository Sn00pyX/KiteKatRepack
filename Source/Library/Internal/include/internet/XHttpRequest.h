
#pragma once

#include <string>
#include <map>

#include "XHttpURL.h"
#include "XHttpRequestParam.h"
#include "XHttpBuffer.h"


class	XHttpRequest
{
public:

	XHttpRequest( const XHttpURL& URL );
	~XHttpRequest();

	const XHttpURL&		URL()		const;

	void				Referer( const std::wstring& strReferer );
	const std::wstring&	Referer()	const;

	void				AddHeader( const std::wstring& strName, const std::wstring& strValue );
	std::wstring		GetHeader()	const;

	void				Param( const XHttpRequestParam* pParam );

	void				SetData( const void* pData, size_t nSize );
	const XHttpBuffer&	GetData()	const;

private:

	typedef	std::map< std::wstring, std::wstring >	HEADER_MAP;

	XHttpURL		m_URL;
	std::wstring	m_strReferer;
	HEADER_MAP		m_mapHeader;

	XHttpBuffer		m_Buffer;

};
