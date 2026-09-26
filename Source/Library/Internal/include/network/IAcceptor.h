
#pragma once

#include "XAddr.h"
#include "ISocketObject.h"

struct IConnection;

struct IAcceptor : ISocketObject, IBaseObject
{
	IAcceptor() : IBaseObject( XOBJ_ACCEPTOR )
	{
		m_socket.CreateStreamSocket();
		m_bIsStarted = false;
	}

	virtual ~IAcceptor()
	{
		if( m_bIsStarted ) EndAccept();
	}

	virtual bool	StartAccept( unsigned nPort )
	{
		return StartAccept( XAddr( "0.0.0.0", nPort ) );
	}
	virtual bool	StartAccept( const XAddr & addr );
	virtual bool	EndAccept();

	XSocket			Accept( XAddr & peerAddr );

protected:

	volatile bool	m_bIsStarted;
};
