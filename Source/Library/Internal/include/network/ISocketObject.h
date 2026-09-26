
#pragma once

#include "XSocket.h"

/// NULL Object
const int XOBJ_NULL			= 0x00;
/// Acceptor
const int XOBJ_ACCEPTOR		= 0x01;
/// Conenctor
const int XOBJ_CONNECTOR	= 0x02;
/// Connection
const int XOBJ_CONNECTION	= 0x03;
/// Datagram
const int XOBJ_DATAGRAM		= 0x04;

struct IBaseObject
{
	IBaseObject( int id = XOBJ_NULL )	{ m_nID = id; m_pTag = 0; }

	void *	GetTag()					{ return m_pTag;}
	void	SetTag( void * tag )		{ m_pTag = tag;	}


	void	SetID( int id )				{ m_nID = id;	}
	int		GetID()						{ return m_nID;	}

protected:

	int		m_nID;
	void *	m_pTag;
};

struct ISocketObject
{
	virtual ~ISocketObject()
	{
		m_socket.Destroy();
	}

	const XSocket	& GetXSocket()		{ return m_socket;					 }
	const TSOCKET	  GetSocketHandle()	{ return m_socket.GetSocketHandle(); }
	bool			  CloseSocket()		{ return m_socket.Destroy();		 }
	bool			  IsValidSocket()	{ return m_socket.IsValidSocket();   }

protected:

	XSocket	m_socket;	
};
