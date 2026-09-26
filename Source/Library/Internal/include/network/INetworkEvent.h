// INetworkEvent.h
//
//	by Testors
//

#pragma once

#include "IConnection.h"
#include "IAcceptor.h"
#include "IDatagram.h"

struct INetworkEventReceiver
{
	virtual IConnection* createConnection( const XSocket & socket )											{ return NULL; }
	virtual IConnection* createConnection( int nID, struct IAcceptor * pAcceptor, const XSocket & socket )	{ return NULL; }
	virtual bool onAccept( int nID, struct IAcceptor * pAcceptor, struct IConnection * pConnection )		{ return true; }
	virtual void onAccepted( int nID, struct IConnection * pConnection )									{}
	virtual void onConnect( int nID, struct IConnection * pConnection )										{}
	virtual void onCantConnect( int nID, struct IConnection * pConnection )									{}
	virtual void onRead( int nID, struct IConnection * pConnection )										{}
	virtual void onRead( int nID, struct IDatagram * pDatagram, unsigned int size )							{}
	virtual void onWrite( int nID, struct IConnection * pConnection )										{}
	virtual void onWrite( int nID, struct IDatagram * pDatagram, unsigned int size )						{}
	virtual void onDisconnect( int nID, struct IConnection * pConnection )									{}
	virtual void onError( const char * szError )															{}
};


struct INetworkEvent
{
	INetworkEvent( INetworkEventReceiver * pReceiver )
	{
		m_pReceiver = pReceiver;
	}
	virtual ~INetworkEvent()
	{
	}

	virtual bool AddObject( IBaseObject * pObj )			= 0;
	virtual bool DelObject( IBaseObject * pObj )			= 0;

	//virtual bool StartThreadPool( unsigned nThreadNum = 0 )	= 0;
	//virtual bool EndThreadPool()							= 0;

protected:
	INetworkEventReceiver * m_pReceiver;
};

