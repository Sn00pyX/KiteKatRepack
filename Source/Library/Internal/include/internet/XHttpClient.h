
#pragma once


#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

#include "XHttpRequest.h"
#include "XHttpContent.h"


struct	XHttpAsync
{
	static	const bool bIsAsync = true;
};

struct	XHttpSync
{
	static	const bool bIsAsync = false;
};

class	XHttpClient
{
public:

	explicit XHttpClient( const std::wstring& strAppName = std::wstring() );
	~XHttpClient();

	void	Timeout( DWORD dwTimeoutMs );

	template< typename HttpTag >
	bool	Get( const XHttpRequest& Request, XHttpContent* pContent, bool bIsAsync = HttpTag::bIsAsync )
	{
		return DoVerb( L"GET", Request, pContent, bIsAsync );
	}

	template< typename HttpTag >
	bool	Post( const XHttpRequest& Request, XHttpContent* pContent, bool bIsAsync = HttpTag::bIsAsync )
	{
		return DoVerb( L"POST", Request, pContent, bIsAsync );
	}

private:

	bool	OpenSync();
	bool	OpenAsync();

	void	Close();

	bool	DoVerb( const wchar_t* pVerb, const XHttpRequest& Request, XHttpContent* pContent, bool bIsAsync );

private:

	HINTERNET		m_hSessionSync;
	HINTERNET		m_hSessionAsync;

	DWORD			m_dwTimeoutMs;
	std::wstring	m_strAppName;

};

