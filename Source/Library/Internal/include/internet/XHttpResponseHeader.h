
#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

#include <string>



class	XHttpResponseHeader
{
public:

	explicit XHttpResponseHeader( HINTERNET hRequest );
	~XHttpResponseHeader();

	DWORD			StatusCode()	const;

	std::wstring	ContentType()	const;
	size_t			ContentLength()	const;

	SYSTEMTIME		ModifiedTime()	const;

	std::wstring	Get()			const;

private:

	HINTERNET	m_hRequest;

};

