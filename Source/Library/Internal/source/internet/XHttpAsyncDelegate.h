
#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

#include <string>

#include "../../include/internet/XHttpRequest.h"
#include "../../include/internet/XHttpContent.h"
#include "../../include/internet/XHttpBuffer.h"


class	XHttpAsyncDelegate
{
public:

	XHttpAsyncDelegate();
	~XHttpAsyncDelegate();

	bool	Execute(	HINTERNET hSession,
						const wchar_t* pVerb,
						const XHttpRequest& Request,
						XHttpContent* pContent,
						size_t nMinBufferSize );

	void	OnSystemCallback( DWORD dwStatus, void* pInfo, DWORD dwLength );

private:

	void	Close();

	bool	DoSystemCallback( DWORD dwStatus, void* pInfo, DWORD dwLength );

private:

	HINTERNET		m_hConnect;
	HINTERNET		m_hRequest;

	XHttpContent*	m_pContent;
	XHttpBuffer*	m_pBuffer;

};

