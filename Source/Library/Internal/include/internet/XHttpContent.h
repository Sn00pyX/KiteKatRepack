
#pragma once

#include "XHttpResponseHeader.h"


class	XHttpContent
{
public:

	XHttpContent();
	virtual ~XHttpContent();

	bool	Reset();
	bool	Wait( DWORD dwTimeoutMs = INFINITE )	const;
	bool	IsSucceeded()	const;

	bool	OnCallbackHeader( const XHttpResponseHeader& Header );
	bool	OnCallbackBody( const void* pBuffer, size_t nReadBytes );
	void	OnCallbackComplete();
	void	OnCallbackError( const wchar_t* szWhere, DWORD dwError, const wchar_t* szFunction, int nLine );

private:

	virtual bool	OnHeader( const XHttpResponseHeader& Header )		= 0;
	virtual bool	OnBody( const void* pBuffer, size_t nReadBytes )	= 0;
	virtual void	OnComplete()										= 0;
	virtual void	OnError( const wchar_t* szWhere, DWORD dwError )	= 0;

protected:

	const wchar_t*	m_szFunction;
	int				m_nLine;

private:

	bool	m_bSucceeded;
	HANDLE	m_hEvent;

};
