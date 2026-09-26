
#include "../../include/network/IAcceptor.h"
#include "../../include/network/XNetworkUtil.h"

bool IAcceptor::StartAccept( const XAddr & addr )
{
	if( m_bIsStarted ) 
	{
		XError::XSetLastError( "IAcceptor already started" );
		return false;
	}

	struct sockaddr_in addr_in;

	if( !XNetworkUtil::ConvAddr( addr, addr_in ) ) {
		m_socket.Destroy();
		XError::XSetLastError( "Can't translate address" );
		return false;
	}

	char i = 1;
	if( setsockopt( m_socket, SOL_SOCKET | IPPROTO_TCP, SO_REUSEADDR, &i, sizeof( i ) ) )
	{
		XError::XSetLastError( "winsock setsockopt() error" );

		m_socket.Destroy();

		return false;
	}

	if( bind( m_socket, reinterpret_cast< struct sockaddr * >( &addr_in ), sizeof( addr_in ) ) )
	{
		XError::XSetLastError( "winsock bind() error" );

		m_socket.Destroy();

		return false;
	}

	if( listen( m_socket, SOMAXCONN ) )
	{
		XError::XSetLastError( "winsock listen() error" );

		m_socket.Destroy();
		return false;
	}

	m_bIsStarted = true;
	return true;
}

bool IAcceptor::EndAccept()
{
	if( !m_bIsStarted )
	{
		XError::XSetLastError( "It's not started acceptor" );
		return false;
	}

	m_bIsStarted = false;

	m_socket.Destroy();

	return m_socket.CreateStreamSocket();
}

XSocket	IAcceptor::Accept( XAddr & peerAddr )
{
	struct sockaddr_in addr;

	memset( &addr, 0, sizeof(addr) );
	int len = sizeof(struct sockaddr_in);
	TSOCKET handle = accept( m_socket.GetSocketHandle(), (sockaddr*)&addr, &len );

	XNetworkUtil::ConvAddr( addr, peerAddr );

	return XSocket( handle );
};
/*
XConnection* IAcceptor::Accept()
{
	if( !m_bIsStarted ) return NULL;

	return NULL;
}
*/