
#include "../../include/network/XAsyncStreamConnection.h"
#include "../../include/cipher/XRC4Cipher.h"
#include "../../include/network/XNetworkUtil.h"


XAsyncStreamConnection::~XAsyncStreamConnection()
{
	WSAAsyncSelect( m_socket, NULL, 0, 0 );

	delete m_pRecvCipher;
	delete m_pSendCipher;
}

int XAsyncStreamConnection::Read( void * szBuf, unsigned nLen )
{
	int nRtn = recv( m_socket, static_cast< char* >( szBuf ), nLen, 0 );

	if ( nRtn <= 0 ) 
	{
		m_bIsConnected = false;
	}
	else
	{
		m_pRecvCipher->Decode( szBuf, szBuf, nRtn );
	}

	return nRtn;
}

int XAsyncStreamConnection::Write( const void * szBuf, unsigned nLen )
{
	char * pTmpBuf = new char[nLen];
	int ret = 0;
	
	m_pSendCipher->Encode( szBuf, pTmpBuf, nLen );

	ret = send( m_socket, static_cast< const char* >( pTmpBuf ), nLen, 0 );

	delete [] pTmpBuf;

	return ret;
}

int XAsyncStreamConnection::Peek( void * szBuf, unsigned nLen )
{
	throw XException( "Can't use Peek() method with sync stream connection." );
}

bool XAsyncStreamConnection::Connect( const XAddr & addr )
{
	if ( m_bIsConnected ) return false;
	
	init( m_bUseCipher );

	if ( !m_socket.IsValidSocket() )
	{
		if ( !m_socket.CreateStreamSocket() ) return false;
	}

	struct sockaddr_in addr_in;
	if ( !XNetworkUtil::ConvAddr( addr, addr_in ) ) return false;

	if ( WSAAsyncSelect( m_socket, m_hWndEvent, WM_SOCK_EVENT, FD_READ | FD_CONNECT | FD_CLOSE ) )
		return false;

	int nRtn = connect( m_socket,
						reinterpret_cast< const struct sockaddr * >( &addr_in ),
						sizeof( struct sockaddr_in ) );

	if ( !nRtn )
	{
		m_bIsConnected = true;
	}

	return m_bIsConnected;
}

bool XAsyncStreamConnection::Close()
{
	m_bIsConnected = false;

	m_pSendCipher->Clear();
	m_pRecvCipher->Clear();

	return CloseSocket();
}


void XAsyncStreamConnection::init( bool bUseCipher )
{
	if ( m_pSendCipher )
		delete m_pSendCipher;
	if ( m_pRecvCipher )
		delete m_pRecvCipher;
	if ( bUseCipher )
	{
		m_pSendCipher = new XRC4Cipher;
		m_pRecvCipher = new XRC4Cipher;

		static_cast< XRC4Cipher * >( m_pSendCipher )->SetKey( "}h79q~B%al;k'y $E" );
		static_cast< XRC4Cipher * >( m_pRecvCipher )->SetKey( "}h79q~B%al;k'y $E" );
	}
	else
	{
		m_pSendCipher = new IDummyCipher;
		m_pRecvCipher = new IDummyCipher;
	}
}


int	XAsyncStreamConnection::OnConnect( WORD wError )
{
	if ( wError == 0 )
	{
		m_bIsConnected = true;
		return 0;
	}
	else
		return -1;
}

int	XAsyncStreamConnection::OnClose( WORD wError )
{
	Close();
	return 0;
}

int	XAsyncStreamConnection::OnReceive( WORD wError )
{
	if ( wError == 0 )
		return 0;
	else
		return -1;
}

int	XAsyncStreamConnection::OnWrite( WORD wError )
{
	if ( wError == 0 )
		return 0;
	else
		return -1;
}
