
#include <cassert>

#include "../../include/network/XIOCPConnection.h"
#include "../../include/network/XIOCPStruct.h"
#include "../../include/network/XNetworkUtil.h"
#include "../../include/toolkit/IQueue.h"
#include "../../include/dump/XException.h"
#include "../../include/toolkit/XEnv.h"
#include "../../include/cipher/XRC4Cipher.h"


volatile LONG	g_nConnectionCount;

XIOCPConnection::XIOCPConnection( OverlappedAllocator *pAllocator, bool bUseCipher ) : m_closeCS( ".XIOCPConnection_Close" ), m_recvCS( ".XIOCPConnection_Recv" ), m_sendCS( ".XIOCPConnection_Send" )
{
	init( pAllocator, bUseCipher );

	InterlockedIncrement( &g_nConnectionCount );
}

XIOCPConnection::XIOCPConnection( OverlappedAllocator *pAllocator, XSocket sock, bool bUseCipher ) : IStreamSocketConnection( sock ), m_closeCS( ".XIOCPConnection_Close" ), m_recvCS( ".XIOCPConnection_Recv" ), m_sendCS( ".XIOCPConnection_Send" )
{
	init( pAllocator, bUseCipher );

	InterlockedIncrement( &g_nConnectionCount );
}

XIOCPConnection::~XIOCPConnection()
{
	//_oprint( "delete connection 0x%08X\n", this );

	if( m_pSendQueue != NULL )
	{
		int nRestSize = m_pSendQueue->Size();
		InterlockedExchangeAdd( &(XIOCPIOStat::GetInstance().nSendRestBytes), -nRestSize );
	}

	delete m_pSendQueue;
	delete m_pRecvQueue;

	delete m_pSendCipher;
	delete m_pRecvCipher;

	delete [] m_pRecvOverlapped->pBuf;
	
	if( m_pAllocator )
	{
		THREAD_SYNCRONIZE( m_pAllocator );	

		m_pAllocator->freeOverlapped( m_pRecvOverlapped );
		m_pAllocator->freeOverlapped( m_pSendOverlapped );
	}
	else
	{
		delete m_pRecvOverlapped;
		delete m_pSendOverlapped;
	}
	
	if( m_pConnectOverlapped ) 
	{
		if( m_pAllocator )
		{
			THREAD_SYNCRONIZE( m_pAllocator );	
			m_pAllocator->freeOverlapped( m_pConnectOverlapped );
		}
		else
		{
			delete m_pConnectOverlapped;
		}
	}

	InterlockedDecrement( &g_nConnectionCount );
}

void XIOCPConnection::init( OverlappedAllocator *pAllocator, bool bUseCipher )
{
	m_nVar = 0;
	m_nFlag = 0;

	m_pAllocator = pAllocator;

	// * Queue 생성에 관한 노트
	//
	// 1. SendQueue는 꼭 PLAIN 스타일의 큐여야 한다. 암호화 관련해서 Queue::GetBuf() 를 사용하기 때문.
	// 2. 펑션 단위에서 lock 을 하기 때문에 thread_safe 할 필요가 없음.
	XIOCPQueueInfo::GetInstance().Update();
	m_pSendQueue = IQueue::MakeQueue( XIOCPQueueInfo::GetInstance().nSendQueueSize, IQueue::PLAIN  );
	m_pRecvQueue = IQueue::MakeQueue( XIOCPQueueInfo::GetInstance().nRecvQueueSize, IQueue::PLAIN );


	// { OVERLAPPED 구조체 생성
	{
		if( m_pAllocator )
		{
			THREAD_SYNCRONIZE( m_pAllocator );
			m_pRecvOverlapped = m_pAllocator->allocOverlapped();
			m_pSendOverlapped = m_pAllocator->allocOverlapped();
		}
		else
		{
			m_pRecvOverlapped = new XOVERLAPPED;
			m_pSendOverlapped = new XOVERLAPPED;
		}

		// receive overlapped structure 초기화		
		m_pRecvOverlapped->Init( reinterpret_cast< HANDLE >( m_socket.GetSocketHandle() ), this, new char[ IOCP_BUFFER_SIZE ] );
		m_pRecvOverlapped->cFlag = XIOCP_RECV;
		
		// send overlapped structure 초기화		
		m_pSendOverlapped->Init( reinterpret_cast< HANDLE >( m_socket.GetSocketHandle() ), this );
		m_pSendOverlapped->cFlag = XIOCP_SEND;

		m_pConnectOverlapped = NULL;
	}	
	// }


	m_bSendPending = false;
	m_nPendingQueryCount = 0;
	m_nSendIndicator = 0;

	m_nPendingRecvQueryCount	= 0;
	m_nPendingSendQueryCount	= 0;

	m_hIOCP = NULL;

	m_bDisconnectSignal = false;
	m_bIsPostedDisconnectEvent = false;

	if( bUseCipher )
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

bool XIOCPConnection::Create()
{
	// 소켓이 정상이 아니면
	if( !m_socket.IsValidSocket() )
	{
		// 새로 소켓을 생성
		if( !m_socket.CreateStreamSocket() )
		{
			return false;
		}
	}

	return true;
}

bool XIOCPConnection::Close()
{
	if( m_bIsConnected ) onDisconnect( -999 );

	// cipher 초기화
	if( m_pSendCipher )	m_pSendCipher->Clear();
	if( m_pRecvCipher )	m_pRecvCipher->Clear();

	m_bIsConnected = false;
	m_socket.Destroy();

	return true;
}

bool XIOCPConnection::Connect( const XAddr & addr )
{
	// 이미 연결되어 있으면 실패
	if( m_bIsConnected ) return false;

	if( !m_pConnectOverlapped )
	{
		if( m_pSendOverlapped )
		{
			THREAD_SYNCRONIZE( m_pAllocator );
			m_pConnectOverlapped = m_pAllocator->allocOverlapped();			
		}
		else
		{
			m_pConnectOverlapped = new XOVERLAPPED;
		}

		m_pConnectOverlapped->Init( reinterpret_cast< HANDLE >( m_socket.GetSocketHandle() ), this );
		m_pConnectOverlapped->cFlag = XIOCP_CONNECT;
	}
	
	// 소켓이 정상이 아니면
	if( !m_socket.IsValidSocket() )
	{
		// 새로 소켓을 생성
		if( !m_socket.CreateStreamSocket() ) return false;
	}

	// 주소가 명확하지 않으면 실패
	struct sockaddr_in addr_in;
	if( !XNetworkUtil::ConvAddr( addr, addr_in ) ) return false;

	// { ConnectEx 의 주소를 얻어옴.
	static LPFN_CONNECTEX  lpfnConnectEx;
	if( !lpfnConnectEx )
	{
		DWORD dw = 0;
		GUID     GuidConnectEx=WSAID_CONNECTEX;
		int nErr = WSAIoctl( m_socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx), &lpfnConnectEx, sizeof(lpfnConnectEx), &dw, NULL, NULL );

		if( nErr ) return false;
	}
	// }

	// { 주소 변환
	struct sockaddr_in addr_in_local;
	ZeroMemory( &addr_in_local, sizeof(addr_in_local) );
	addr_in_local.sin_addr.S_un.S_addr = INADDR_ANY;
	addr_in_local.sin_family = AF_INET;
	// }

	// cipher 초기화
	if( m_pSendCipher )	m_pSendCipher->Clear();
	if( m_pRecvCipher )	m_pRecvCipher->Clear();

	// bind
	if( bind( m_socket.GetSocketHandle(), (struct sockaddr*)&addr_in_local, sizeof(addr_in_local) ) )
	{
		return false;
	}

	// call
	BOOL bRtn = (*lpfnConnectEx)( m_socket.GetSocketHandle(), reinterpret_cast< struct sockaddr* >( &addr_in ), sizeof( addr_in ), NULL, 0, NULL, m_pConnectOverlapped );

	if( !bRtn && WSAGetLastError() == WSA_IO_PENDING ) return true;
	
	return false;
}

bool XIOCPConnection::SyncConnect( const XAddr & addr )
{
	// 이미 연결되어 있으면 실패
	if( m_bIsConnected ) return false;
	
	// 소켓이 정상이 아니면
	if( !m_socket.IsValidSocket() )
	{
		// 새로 소켓을 생성
		if( !m_socket.CreateStreamSocket() ) return false;
	}

	// 주소가 명확하지 않으면 실패
	struct sockaddr_in addr_in;
	if( !XNetworkUtil::ConvAddr( addr, addr_in ) ) return false;

	// 연결
	int nRtn = connect( m_socket, reinterpret_cast< struct sockaddr* >( &addr_in ), sizeof( struct sockaddr_in ) );

	if( nRtn != SOCKET_ERROR ) m_bIsConnected = true;
	
	return m_bIsConnected;
}

bool XIOCPConnection::Reset()
{
	if( m_bIsConnected ) m_bIsConnected = false;

	if( m_socket.IsValidSocket() ) m_socket.Destroy();

	m_bSendPending				= false;

	m_nSendIndicator			= 0;

	m_nPendingQueryCount		= m_nPendingRecvQueryCount = m_nPendingSendQueryCount = 0;

	m_hIOCP						= NULL;

	m_bIsPostedDisconnectEvent = false;

	m_bDisconnectSignal	= false;

	return true;
}

int	 XIOCPConnection::Peek( void *szBuf, size_t nLen )
{
	THREAD_SYNCRONIZE( m_recvCS );

	// 읽을 크기 얻는다
	unsigned nReadableSize = (std::min)( ( unsigned int )nLen, m_pRecvQueue->Size() );

	// 암호화 풀고
	m_pRecvCipher->Decode( m_pRecvQueue->GetBuf(), szBuf, nReadableSize, true );

	// 읽은길이 리턴
	return nReadableSize;
}

int	 XIOCPConnection::Read( void* szBuf, size_t nLen )
{
	THREAD_SYNCRONIZE( m_recvCS );

	// 읽을 크기 얻는다
	unsigned nReadableSize = (std::min)( ( unsigned int )nLen, m_pRecvQueue->Size() );

	// 암호화 풀고
	m_pRecvCipher->Decode( m_pRecvQueue->GetBuf(), szBuf, nReadableSize );

	// 읽은만큼 버린다
	m_pRecvQueue->Read( NULL, nReadableSize );

	// 읽은길이 리턴
	return nReadableSize;
}

int XIOCPConnection::Write( const void * szBuf, size_t nLen )
{
	if( !IsConnected() ) 
	{
		return -1;
	}
	
	if( CheckDisconnectSignal() )
	{
		return -1;
	}

	if( !nLen ) return 0;

	THREAD_SYNCRONIZE( &m_sendCS );

	if( m_pSendQueue->FreeSize() < nLen )
	{
		unsigned int nReserveSize = XIOCPQueueInfo::GetInstance().nBaseQueueSize;
		if( nReserveSize < nLen )
		{
			nReserveSize = static_cast< unsigned int >( nLen );
		}

		m_pSendQueue->Reserve( nReserveSize );

		if( XIOCPQueueInfo::GetInstance().nMaxReallocSize < (int)m_pSendQueue->GetReservedSize() )
		{
			XIOCPQueueInfo::GetInstance().nMaxReallocSize = m_pSendQueue->GetReservedSize();
		}
	}

	unsigned int unPrevSize = m_pSendQueue->Size();
	if( m_pSendQueue->Write( szBuf, static_cast< unsigned int >( nLen ) ) != static_cast< unsigned int >( nLen ) )
	{
		assert(0);
		onDisconnect( -1 );
		return -1;
	}

	m_pSendCipher->Encode( m_pSendQueue->GetBuf() + unPrevSize, const_cast< char* >( m_pSendQueue->GetBuf() + unPrevSize ), static_cast< unsigned int >( nLen ) );

	InterlockedExchangeAdd( &(XIOCPIOStat::GetInstance().nSendRestBytes), (int)nLen );
	if( !m_bSendPending ) 
	{
		procWriteFile();
	}

	return ( int )nLen;
}


void XIOCPConnection::onSendCompletionEvent( int nSize )
{
	InterlockedDecrement( &m_nPendingSendQueryCount );

	if( nSize < 1 ) return;

	THREAD_SYNCRONIZE( &m_sendCS );

	InterlockedIncrement( &(XIOCPIOStat::GetInstance().nSendCount) );
	InterlockedExchangeAdd( &(XIOCPIOStat::GetInstance().nSendTraffic), nSize );
	InterlockedExchangeAdd( &(XIOCPIOStat::GetInstance().nSendRestBytes), -nSize );

	assert( nSize <= (int)m_pSendQueue->Size() );

	m_pSendQueue->Read( NULL, nSize );

	m_nSendIndicator -= nSize;
	m_bSendPending = false;

	/*
	// 버퍼가 원래 크기보다 크고, 남은 양이 원래 크기보다 작으면(축소 시킬 수 있다면)
	unsigned int nOrginalSize = XIOCPQueueInfo::GetInstance().nSendQueueSize;
	if( m_pSendQueue->GetReservedSize() > nOrginalSize &&
		m_pSendQueue->Size() < nOrginalSize )
	{
		// 여유분이 충분하다면(50%) 버퍼를 축소 시킨다.
		if( m_pSendQueue->FreeSize() >= (nOrginalSize / 2) )
		{
			m_pSendQueue->ResetReservedSize( nOrginalSize );
		}
	}
	*/

	int nRestSize = m_pSendQueue->Size();

	// Send Queue 에 있으나 WriteFile() 되지 않은 부분이 있다면 처리
	if( nRestSize - m_nSendIndicator > 0 )
	{
		if( !m_bDisconnectSignal && m_bIsConnected && !m_bIsPostedDisconnectEvent )
		{
			procWriteFile();
		}
	}
}

void XIOCPConnection::decreaseQueryCount()
{
	InterlockedDecrement( &m_nPendingQueryCount );
}

void XIOCPConnection::onRecvCompletionEvent( int nSize )
{	
	InterlockedDecrement( &m_nPendingRecvQueryCount );

	if( nSize < 1 ) {
		m_recvCS.UnLock();
		return;
	}

	InterlockedIncrement( &(XIOCPIOStat::GetInstance().nRecvCount) );
	InterlockedExchangeAdd( &(XIOCPIOStat::GetInstance().nRecvTraffic), nSize );

	if( !m_pRecvQueue->Resize( m_pRecvQueue->Size() + nSize ) ) throw XException( "XIOCPConnection _procPushRecvQueue Error!!" );

	m_recvCS.UnLock();
}

void XIOCPConnection::onConnect( void *pBuf )
{
	struct sockaddr_in * pMyAddr = NULL;
	struct sockaddr_in * pPeerAddr = NULL;
	int nMyLen, nPeerLen;

	GetAcceptExSockaddrs( pBuf, 0, sizeof( sockaddr_in ) + 16, sizeof( sockaddr_in ) + 16, 
							reinterpret_cast< sockaddr ** >( &pMyAddr ), &nMyLen, reinterpret_cast< sockaddr ** >( &pPeerAddr ), &nPeerLen );

	m_myAddr.SetAddr( inet_ntoa( pMyAddr->sin_addr ) );
	m_myAddr.SetPortNum( ntohs( pMyAddr->sin_port ) );

	m_peerAddr.SetAddr( inet_ntoa( pPeerAddr->sin_addr ) );
	m_peerAddr.SetPortNum( ntohs( pPeerAddr->sin_port ) );

	m_bIsConnected = true;
}

#include <set>

void XIOCPConnection::onDisconnect( int nFlag )
{
	bool bNotifyClosed = false;

	{
		THREAD_SYNCRONIZE( m_closeCS );

		// 같은 쓰레드의 critical section 재진입은 허용된다.
		/*
		THREAD_SYNCRONIZE1( m_recvCS );
		THREAD_SYNCRONIZE2( m_sendCS );*/

		// 첫 연결끊어짐 통지라면..
		if( !m_bDisconnectSignal )
		{
			m_bDisconnectSignal = true;
			m_nFlag = nFlag;
		}

		// 쿼리 갯수가 0이라면?
		if( !m_nPendingQueryCount )
		{
			if( !m_bIsPostedDisconnectEvent )
			{
				if( IsConnected() )
				{
					m_bIsConnected = false;

					if( !Close() )
					{
						throw XException( "XIOCPConnection::onDisconnect - CloseSocket() Error!" );
					}
				}

				// _oprint( "Before Post Disconnect Event : 0x%08X (%d) (%04x)\n", this, nFlag, nRandKey );

				m_bIsPostedDisconnectEvent = true;

				bNotifyClosed = true;
			}
		}
	}

	if( bNotifyClosed )
	{
		PostQueuedCompletionStatus( this->m_hIOCP, 0, static_cast< ULONG_PTR >( XIOCP_EVENT_CONNECTION_CLOSED ), m_pRecvOverlapped );
	}

	// _oprint( "After Post Disconnect Event : 0x%08X (%d:%d:%d) (%04x)\n", this, nFlag, nTmp, nProgKey, nRandKey );
}

bool XIOCPConnection::pendRecvRequest()
{
	if( !IsConnected() ) return false;

	m_recvCS.Lock();
	
	bool bRtn = true;

	if( m_pRecvQueue->Size() + m_pRecvQueue->FreeSize() > m_pRecvQueue->GetReservedSize() )
	{
		throw XException( "XIOCPConnection QUEUE OVERFLOW!!" );
	}

	// 버퍼가 가득 찼다면..
	if( m_pRecvQueue->FreeSize() == 0 )
	{
		m_pRecvQueue->Reserve( m_pRecvQueue->GetReservedSize() );
	}

	// IOCP 에서, WSARecv() 호출은 GetCompletionStatus() 와 함께 대개의 CPU 시간을 소모한다. 마음에 안드는군~
	m_RecvWSABUF.buf = const_cast< char* >(m_pRecvQueue->GetBuf() + m_pRecvQueue->Size());
	m_RecvWSABUF.len = m_pRecvQueue->FreeSize();

	assert( m_nPendingRecvQueryCount < 1 );

	InterlockedIncrement( &m_nPendingQueryCount );
	InterlockedIncrement( &m_nPendingRecvQueryCount );

	m_dwRecvFlag = 0;
	// _oprint( "WSARecv() : 0x%08X\n", this );
	int nRtn = WSARecv( GetSocketHandle(), &m_RecvWSABUF, 1, &m_pRecvOverlapped->dwSize, &m_dwRecvFlag, m_pRecvOverlapped, NULL );
	/*
	int nRtn = ReadFile( reinterpret_cast< HANDLE >( GetXSocket().GetSocketHandle() ),
						 const_cast< char* >(m_pRecvQueue->GetBuf() + m_pRecvQueue->Size()), m_pRecvQueue->FreeSize(),
						 &m_pRecvOverlapped->dwSize, m_pRecvOverlapped );  
						 */

	if( nRtn == SOCKET_ERROR )
	{
		int nErr = WSAGetLastError();
		if( nErr == WSA_IO_PENDING )
		{			
		}
		else
		{
			bRtn = false;
			InterlockedDecrement( &m_nPendingQueryCount );
			InterlockedDecrement( &m_nPendingRecvQueryCount );


			onDisconnect( -2 );
		}
	}

	return bRtn;
}

bool XIOCPConnection::onConnectCompletionEvent( bool bFlag )
{
	m_bIsConnected = bFlag;

	if( m_bIsConnected ) return pendRecvRequest();

	return false;
}

bool XIOCPConnection::procWriteFile()
{
	if( !IsConnected() ) return false;

	if( m_bDisconnectSignal ) return false;
	if( m_bIsPostedDisconnectEvent ) return false;

	int nLen = m_pSendQueue->Size() - m_nSendIndicator;

	int nSendSize = ( IOCP_BUFFER_SIZE > nLen ? nLen : IOCP_BUFFER_SIZE );

	XOVERLAPPED * pSendOverlapped = m_pSendOverlapped;

	// 주의   : 꼭 m_pSendQueue 의 GetBuf() 를 받아서 사용할것.
	//          (암호화가 SendQueue 에 Write 될때 일어나기 때문)
	//
	// 덧붙임 : 스트림 압축 알고리즘을 Queue 에 구현시
	//          GetBuf() 뿐만 아니라 size 도 Queue 의것을 사용할 필요가 있다.

	InterlockedIncrement( &m_nPendingQueryCount );
	InterlockedIncrement( &m_nPendingSendQueryCount );

	int nRtn;
	m_dwSendFlag = 0;
	m_SendWSABUF.buf = const_cast< char* >( m_pSendQueue->GetBuf() + m_nSendIndicator );
	m_SendWSABUF.len = nSendSize;
	nRtn = WSASend( GetSocketHandle(), &m_SendWSABUF, 1, &pSendOverlapped->dwSize, m_dwSendFlag, pSendOverlapped, NULL );
	/*
	nRtn = WriteFile(	reinterpret_cast< HANDLE >( m_socket.GetSocketHandle() ),
							m_pSendQueue->GetBuf() + m_nSendIndicator, nSendSize,
							&pSendOverlapped->dwSize, pSendOverlapped );
							*/

	if( nRtn == SOCKET_ERROR )
	{
		DWORD nErr = WSAGetLastError();

		if( nErr == WSA_IO_PENDING )
		{			
		}
		else
		{
			InterlockedDecrement( &m_nPendingQueryCount );
			InterlockedDecrement( &m_nPendingSendQueryCount );

			onDisconnect( -3 );
			return false;
		}
	}
	else
	{
	}

	m_bSendPending = true;

	m_nSendIndicator += nSendSize;

	return true;
}

const char* XIOCPConnection::GetBuf()					{ return m_pRecvQueue->GetBuf(); }
int	 XIOCPConnection::Size()							{ return m_pRecvQueue->Size(); }



XIOCPConnectionCloser::XIOCPConnectionCloser( XIOCPConnectionCloser::DeleteHandler* pDel ) : m_cs( ".XIOCPConnectionCloser" )
{
	m_pDel = pDel;
}

XIOCPConnectionCloser::~XIOCPConnectionCloser()
{
	DeInit();
}

void XIOCPConnectionCloser::DeInit()
{
	THREAD_SYNCHRONIZE( m_cs );

	for ( std::list< XIOCPConnection* >::iterator it = m_closingConns.begin(); it != m_closingConns.end(); ++it )
	{
		if ( m_pDel )
			m_pDel->onDelete( *it );
		else
			delete *it;
	}
	m_closingConns.clear();
}

bool XIOCPConnectionCloser::Has( XIOCPConnection* pConn )
{
	THREAD_SYNCHRONIZE( m_cs );

	return std::find( m_closingConns.begin(), m_closingConns.end(), pConn ) != m_closingConns.end();
}

void XIOCPConnectionCloser::Add( XIOCPConnection* pConn )
{
	THREAD_SYNCHRONIZE( m_cs );

	if ( !pConn || Has( pConn ) )
		return;

	if ( pConn->IsConnected() )
		pConn->Close();

	if ( pConn->GetPendingQueryCount() == 0 )
	{
		if ( m_pDel )
			m_pDel->onDelete( pConn );
		else
			delete pConn;
		return;
	}
		
	m_closingConns.push_back( pConn );
}

void XIOCPConnectionCloser::Process()
{
	THREAD_SYNCHRONIZE( m_cs );

	for ( std::list< XIOCPConnection* >::iterator it = m_closingConns.begin(); it != m_closingConns.end(); )
	{
		if ( (*it)->GetPendingQueryCount() == 0 )
		{
			if ( m_pDel )
				m_pDel->onDelete( *it );
			else
				delete *it;
			it = m_closingConns.erase( it );
		}
		else
			++it;
	}
}
