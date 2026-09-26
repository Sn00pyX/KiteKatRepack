#pragma once

#include <list>

#include <network/XIOCPConnection.h>
#include <network/INetworkEvent.h>
#include <mmo/ArTime.h>
#include <mmo/ArSchedulerObject.h>
#include <toolkit/ILock.h>
#include <toolkit/khash.h>


struct ConnectionManager : ArSchedulerObject
{
	ConnectionManager() : m_nNextTimeoutResetTime( GetArTime() + 8640000 ) {}

	virtual void onProcess( int nThreadIdx );

	void Push( IConnection * pConnection )
	{
		THREAD_SYNCRONIZE( m_ConnectionCS );

		std::list< IConnection * >::iterator it;

		for( it = m_vConnection.begin(); it != m_vConnection.end(); ++it )
		{
			if( (*it) == pConnection )
				return;
		}
		m_vConnection.push_back( pConnection );
	}

	void Pop( IConnection * pConnection )
	{
		THREAD_SYNCRONIZE( m_ConnectionCS );

		std::list< IConnection * >::iterator it;

		for( it = m_vConnection.begin(); it != m_vConnection.end(); ++it )
		{
			if( (*it) == pConnection )
			{
				m_vConnection.erase( it );
				return;
			}
		}
	}

	void			DisconnectAll();
	const size_t	GetConnectionCount() const			{ return m_vConnection.size(); }

private:

	std::list< IConnection* >				m_vConnection;
	XCriticalSection						m_ConnectionCS;
	KHash< int, hashPr_string_nocase >		m_hsSecurityResponseTimeoutCount;
	AR_TIME									m_nNextTimeoutResetTime;

};

struct ConnectionCloser : ArSchedulerObject
{
	virtual void onProcess( int nThreadIdx );

	void Push( XIOCPConnection * pConnection )
	{
		THREAD_SYNCRONIZE( m_ClosedConnectionCS );

		m_vClosedConnection.push_back( pConnection );
	}

	const size_t	GetClosedConnectionCount() const	{ return m_vClosedConnection.size(); }

private:

	std::list< XIOCPConnection* >			m_vClosedConnection;
	XCriticalSection						m_ClosedConnectionCS;

};

struct myNetworkEventReceiver : INetworkEventReceiver
{
	IConnection* createConnection( const XSocket & socket );
	bool onAccept( int nID, IAcceptor * pAcceptor, IConnection * pConnection );
	void onRead( int nID, IConnection * pConnection );
	void onDisconnect( int nID, IConnection * pConnection );
	void onError( const char * szError );
};

void StartAccept();
void EndAccept();
bool ConnectToAuth();
bool ConnectToUpload();
bool ConnectToCoordinator();