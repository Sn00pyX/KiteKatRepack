
#include <memory>

#include "../../include/toolkit/XThread.h"
#include "../../include/network/XSyncStreamConnection.h"
#include "../../include/network/XNetworkUtil.h"
#include "../../include/toolkit/IQueue.h"
#include "../../include/dump/XException.h"
#include "../../include/cipher/XRC4Cipher.h"

unsigned __stdcall NetworkIOProc( void * pArg );

XSyncStreamConnection::~XSyncStreamConnection()
{
	if( m_bUseThreadForIO )
	{
		Close();

		delete m_csRecvQueueLock;
		delete m_csSendQueueLock;
	}

	delete m_pRecvCipher;
	delete m_pSendCipher;
	delete m_pSendQueue;
	delete m_pRecvQueue;
}

int XSyncStreamConnection::Read( void * szBuf, size_t nLen )
{
	int nRtn = -1;
	if( IsConnected() == false )
	{
		return nRtn;
	}

	if( m_bUseThreadForIO )
	{
		// IO 담당 Thread가 이미 복호화까지 다 해 줬기때문에 버퍼 복사만 해서 넘긴다.
		THREAD_SYNCHRONIZE( m_csRecvQueueLock );
		nRtn = ( int )( m_pRecvQueue->Read( szBuf, nLen ) );
	}
	else
	{
		nRtn = recv( m_socket, static_cast< char* >( szBuf ), ( int )( nLen ), 0 );
		if ( nRtn <= 0 )
		{
			// nonblocking
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				return 0;
			}

			m_bIsConnected = false;
			nRtn = -1;
		}
		else
		{
			m_pRecvCipher->Decode( szBuf, szBuf, nRtn );
		}
	}

	return nRtn;
}

int XSyncStreamConnection::Write( const void * szBuf, size_t nLen )
{
	if( IsConnected() == false )
	{
		return 0;
	}

	const int kStackBufSize = 4096;

	char stackBuf[ kStackBufSize ];
	char* pTmpBuf = nLen < sizeof( stackBuf ) ? stackBuf : new char[nLen];
	int ret = 0;

	m_pSendCipher->Encode( szBuf, pTmpBuf, ( unsigned int )( nLen ) );

	if( m_bUseThreadForIO )
	{
		if( m_bThreadToBeTerminated == false )
		{
			THREAD_SYNCHRONIZE( m_csSendQueueLock );
			ret = m_pSendQueue->Write( pTmpBuf, ( unsigned int )( nLen ) );
		}
	}
	else
	{
		ret = m_pSendQueue->Write( pTmpBuf, ( unsigned int )( nLen ) );
		Select();
	}

	if ( pTmpBuf != stackBuf )
		delete [] pTmpBuf;	

	return ret;
}

int XSyncStreamConnection::Peek( void * szBuf, size_t nLen )
{
	throw XException( "Can't use Peek() method with sync stream connection." );
}

bool XSyncStreamConnection::Connect( const XAddr & addr )
{
	if ( m_bIsConnected ) return false;
	
	if ( !m_socket.IsValidSocket() )
	{
		if ( !m_socket.CreateStreamSocket() ) return false;
	}

	struct sockaddr_in addr_in;
	if ( !XNetworkUtil::ConvAddr( addr, addr_in ) ) return false;

	int nRtn = connect( m_socket,
						reinterpret_cast< const struct sockaddr * >( &addr_in ),
						sizeof( addr_in ) );

	if ( !nRtn )
	{
		if( m_bUseThreadForIO )
		{
			m_hIOThreadHandle = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, NetworkIOProc, this, 0, NULL ) );
			if( !m_hIOThreadHandle )
			{
				CloseSocket();
				return false;
			}
		}
		m_bIsConnected = true;
	}

	return m_bIsConnected;
}

bool XSyncStreamConnection::Close()
{
	// 보내야 할 데이터가 있다면 다 보내고 종료한다.
	if( !m_bUseThreadForIO && m_pSendQueue != NULL )
	{
		while( m_bIsConnected == true && m_pSendQueue->Size() )
			Select();
	}

	if( m_bUseThreadForIO && m_hIOThreadHandle )
	{
		m_bThreadToBeTerminated = true;
		// 쓰레드 죽을 때 까지 기다림.
		DWORD dwRet = WaitForSingleObject( m_hIOThreadHandle, WAITING_TIME_FOR_THREAD_TO_DIE );
		if( dwRet != WAIT_OBJECT_0 )
		{
			// 쓰레드 안 죽으면 Terminate 호출.
			TerminateThread( m_hIOThreadHandle, -1 );
		}
		
		CloseHandle( m_hIOThreadHandle );
		m_hIOThreadHandle = NULL;
	}

	m_bIsConnected = false;

	return CloseSocket();
}

bool XSyncStreamConnection::IsReadable()
{
	if( m_bUseThreadForIO )
	{
		THREAD_SYNCHRONIZE( m_csRecvQueueLock );
		return ( m_pRecvQueue->Size() != 0 );
	}

	if( IsConnected() == false )
	{
		return false;
	}

	Select();

	return !!FD_ISSET( GetSocketHandle(), &m_rdSet );
}

bool XSyncStreamConnection::IsWriteable()
{
	if( IsConnected() == false )
	{
		return false;
	}

	if( m_bUseThreadForIO )
	{
		if( m_bThreadToBeTerminated == true )
		{
			return false;
		}

		THREAD_SYNCHRONIZE( m_csSendQueueLock );
		return ( m_pSendQueue->Size() != 0 );
	}

	Select();

	return !!FD_ISSET( GetSocketHandle(), &m_wrSet );
}

void XSyncStreamConnection::Select()
{
	if( IsValidSocket() == false )
	{
		return;
	}

	FD_ZERO( &m_rdSet );
	FD_ZERO( &m_wrSet );
	FD_ZERO( &m_exSet );

	FD_SET( GetSocketHandle(), &m_rdSet );
	FD_SET( GetSocketHandle(), &m_wrSet );
	FD_SET( GetSocketHandle(), &m_exSet );

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	int nResult = select( 0, &m_rdSet, &m_wrSet, &m_exSet, &tv );
	if( nResult == SOCKET_ERROR )
	{
		m_bIsConnected = false;
		return;
	}
	else if( nResult == 0 )
	{
		return;
	}

	if( FD_ISSET( GetSocketHandle(), &m_exSet ) )
	{
		m_bIsConnected = false;
		return;
	}

	if ( m_pSendQueue->Size() && FD_ISSET( GetSocketHandle(), &m_wrSet ) )
	{
		char buf[ SEND_BUF_SIZE_DEFAULT ] = { 0, };
		int nSize = 0;
		if( m_bUseThreadForIO )
		{
			THREAD_SYNCHRONIZE( m_csSendQueueLock );
			nSize = ( m_pSendQueue->Size() > SEND_BUF_SIZE_DEFAULT )? SEND_BUF_SIZE_DEFAULT : m_pSendQueue->Size();
			s_memcpy( buf, sizeof( buf ), m_pSendQueue->GetBuf(), nSize );
		}
		else
		{
			nSize = ( m_pSendQueue->Size() > SEND_BUF_SIZE_DEFAULT )? SEND_BUF_SIZE_DEFAULT : m_pSendQueue->Size();
			s_memcpy( buf, sizeof( buf ), m_pSendQueue->GetBuf(), nSize );
		}

		if( nSize > 0 )
		{
			int ret = send( m_socket, buf, nSize, 0 );

			if ( ret != SOCKET_ERROR )
			{
				if( m_bUseThreadForIO )
				{
					THREAD_SYNCHRONIZE( m_csSendQueueLock );
					m_pSendQueue->Read( NULL, ret );
				}
				else
				{
					m_pSendQueue->Read( NULL, ret );
				}
			}
			else
				m_bIsConnected = false;
		}
	}

	if( m_bUseThreadForIO && FD_ISSET( GetSocketHandle(), &m_rdSet ) )
	{
		char buf[ RECV_BUF_SIZE_DEFAULT ];
		int nRtn = recv( m_socket, buf, sizeof( buf ), 0 );
			
		if ( nRtn <= 0 ) 
		{
			m_bIsConnected = false;
		}
		else
		{
			m_pRecvCipher->Decode( buf, buf, nRtn );
			if( m_bUseThreadForIO )
			{
				THREAD_SYNCHRONIZE( m_csRecvQueueLock );
				m_pRecvQueue->Write( buf, nRtn );
			}
			else
			{
				m_pRecvQueue->Write( buf, nRtn );
			}
		}
	}
}

void XSyncStreamConnection::init( bool bUseCipher, bool bUseThreadForIO )
{
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

	m_bUseThreadForIO = bUseThreadForIO;
	m_bThreadToBeTerminated = false;
	m_hIOThreadHandle = NULL;
	m_csRecvQueueLock = NULL;
	m_csSendQueueLock = NULL;

	if( bUseThreadForIO )
	{
		m_csRecvQueueLock = new XCriticalSection( "csRecvQueueLock" );
		m_csSendQueueLock = new XCriticalSection( "csSendQueueLock" );
	}

	m_pSendQueue = IQueue::MakeQueue( SEND_BUF_SIZE_DEFAULT, IQueue::PLAIN );
	m_pRecvQueue = IQueue::MakeQueue( RECV_BUF_SIZE_DEFAULT, IQueue::PLAIN );
}

unsigned __stdcall XSyncStreamConnection::NetworkIOProc( void * pArg )
{
	XSetThreadName( -1, "XSyncStreamConnection IO Thread" );

	XSyncStreamConnection *pConnection = static_cast< XSyncStreamConnection * >( pArg );

	while( true )
	{
		if( pConnection->m_bThreadToBeTerminated )
		{
			if( pConnection->m_pSendQueue->Size() == 0 )
				break;
		}

		pConnection->Select();
		::Sleep( 10 );
	}

	return 0;
}