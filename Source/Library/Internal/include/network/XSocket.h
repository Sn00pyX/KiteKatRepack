
#pragma once

#include <winsock2.h>
#include "../dump/XException.h"


typedef SOCKET	TSOCKET;

struct XSocket
{
	XSocket( TSOCKET socket = INVALID_SOCKET )
	{
		m_socket = socket;
	}

	~XSocket()
	{
	}

	bool IsValidSocket() const
	{
		return ( m_socket != INVALID_SOCKET && m_socket != SOCKET_ERROR );
	}

	bool CreateStreamSocket()
	{
		m_socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

		if ( IsValidSocket() ) return true;

		XError::XSetLastError( "winsock socket() error" );
		return false;
	}

	bool CreateDatagramSocket()
	{
		m_socket = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );

		if ( IsValidSocket() ) return true;

		XError::XSetLastError( "winsock socket() error" );
		return false;
	}

	virtual bool Destroy()
	{
		if ( closesocket( m_socket ) == SOCKET_ERROR ) {
			return false;
		}

		m_socket = INVALID_SOCKET;

		return true;
	}

	TSOCKET GetSocketHandle() const
	{
		return m_socket;
	}

	operator TSOCKET() const
	{
		return m_socket;
	}

	void operator =( TSOCKET sock )
	{
		m_socket = sock;
	}

	bool operator ==( const TSOCKET & rh )
	{
		return m_socket == rh;
	}

private:
	TSOCKET m_socket;
};

