
#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

#include "../../include/internet/XHttpRequest.h"
#include "../../include/internet/XHttpContent.h"


class	XHttpSyncDelegate
{
public:

	XHttpSyncDelegate();
	~XHttpSyncDelegate();

	bool	Execute(	HINTERNET hSession,
						const wchar_t* pVerb,
						const XHttpRequest& Request,
						XHttpContent* pContent,
						size_t nMinBufferSize );

private:

	void	RequestProcess( const XHttpRequest& Request );
	bool	ReadProcess( XHttpContent* pContent, size_t nMinBufferSize );

	bool	OpenRequest( HINTERNET hSession, const wchar_t* pVerb, const XHttpRequest& Request );
	bool	SendRequest( const XHttpRequest& Request );

	void	Close();

private:

	HINTERNET	m_hConnect;
	HINTERNET	m_hRequest;

};