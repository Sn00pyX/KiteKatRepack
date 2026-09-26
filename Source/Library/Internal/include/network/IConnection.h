// IConnection.h
//
// Connection 특성과 Socket Connection, 그리고 Stream I/O 를 가지는 Socket Connection 을정의함
//
// by Testors

#pragma once

#include "XAddr.h"
#include "ISocketObject.h"
#include "IStream.h"

struct IConnection : public IBaseObject
{
	virtual ~IConnection() {}

	virtual bool	Connect( const XAddr & addr )	= 0;
	virtual bool	Close()							= 0;
	virtual bool	IsConnected() const				= 0;

	virtual void	GetPeerAddress( XAddr & addr )	= 0;
	virtual void	GetMyAddress( XAddr & addr )	= 0;

	virtual const XAddr &	GetPeerAddress()		= 0;
	virtual const XAddr &	GetMyAddress()			= 0;
};

struct ISocketConnection : public IConnection,
						   public ISocketObject
{
};

struct IStreamSocketConnection : public ISocketConnection,
								 public IStreamIntf
{
	IStreamSocketConnection()
	{
		m_bIsConnected = false;

		m_socket.CreateStreamSocket();
	}

	IStreamSocketConnection( XSocket sock )
	{
		m_bIsConnected = false;

		m_socket = sock;
	}
	virtual ~IStreamSocketConnection()
	{
	}

	virtual bool	Connect( const XAddr & addr )			= 0;
	virtual bool	Close()									= 0;
	virtual bool	IsConnected() const						{ return m_bIsConnected; }

	virtual bool	IsValid()								{ return IsConnected(); }
	virtual int		Size()									{ return -1; }

	virtual void			GetPeerAddress( XAddr & addr )	{ addr = m_peerAddr; }
	virtual void			GetMyAddress( XAddr & addr )	{ addr = m_myAddr;	 }
	virtual const XAddr &	GetPeerAddress()				{ return m_peerAddr; }
	virtual const XAddr &	GetMyAddress()					{ return m_myAddr;   }
	

	bool			EnableNoDelay()							{ BOOL bValue = TRUE; return !setsockopt( GetSocketHandle(), IPPROTO_TCP, TCP_NODELAY, (const char *) &bValue, sizeof( bValue ) ); }
	bool			DisableNoDelay()						{ BOOL bValue = FALSE; return !setsockopt( GetSocketHandle(), IPPROTO_TCP, TCP_NODELAY, (const char *) &bValue, sizeof( bValue ) ); }

protected:

	XAddr	m_myAddr;
	XAddr	m_peerAddr;

	bool	m_bIsConnected;
};
