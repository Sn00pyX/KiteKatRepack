
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>

#include <vector>
#include <list>
#include <algorithm>
#include <cassert>

#include "../../include/network/XIOCP.h"
#include "../../include/network/XIOCPConnection.h"
#include "../../include/network/XIOCPAcceptor.h"
#include "../../include/network/XIOCPDatagram.h"

#include "../../include/network/XNetworkUtil.h"
#include "../../include/network/XIOCPStruct.h"
#include "../../include/toolkit/XMemoryPool.h"
#include "../../include/toolkit/ILock.h"
#include "../../include/toolkit/XThread.h"
#include "../../include/toolkit/XConsole.h"
#include "../../include/toolkit/XEnv.h"
#include "../../include/toolkit/XThreadMonitor.h"
#include "../../include/logging/FileLog.h"
#include "../../include/dump/XExceptionHandler.h"

volatile LONG g_nDisconnectCnt;
static void	(*s_pfInitFunc)( int ) = NULL;

struct IOCPTAG
{	
	HANDLE						hIOCP;
	
	std::vector< uintptr_t >	vThreadHandle;
	std::vector< unsigned int >	vThreadId;

	OverlappedAllocator			*pOverlappedAllocator;

	INetworkEventReceiver*		pReceiver;
	XIOCP*						pIOCPMgr;
	volatile long* 				plActiveThreadCount;
	volatile long*				plInstructionCount;
	volatile long*				plCurrentThreadCount;
	volatile bool* 				pbIsPaused;
	bool						bIsFinished;

	IOCPTAG( INetworkEventReceiver* receiver)
	{
		bIsFinished = false;
		hIOCP		= NULL;
		pReceiver	= receiver;

		pIOCPMgr = NULL;
		plActiveThreadCount = NULL;
		plInstructionCount = NULL;
		plCurrentThreadCount = NULL;
		pbIsPaused = NULL;

		pOverlappedAllocator = new OverlappedAllocator;
	}

	virtual ~IOCPTAG()
	{
		delete pOverlappedAllocator;
	}

private:
	IOCPTAG( const IOCPTAG& );
	IOCPTAG& operator=( const IOCPTAG& );

};

bool XIOCP::onAcceptEvent( IOCPTAG * pTag, int nThreadNum, XOVERLAPPED * pOverlapped, bool bIsSuccess )
{
	XIOCPAcceptor* pAcceptor = static_cast< XIOCPAcceptor* >( pOverlapped->pObj );

	SOCKET hFileHandle = reinterpret_cast< SOCKET >( pOverlapped->hFileHandle );
	char buf[sizeof(sockaddr_in)*2 + 32];
	s_memcpy( buf, sizeof( buf ), pOverlapped->pBuf, sizeof(sockaddr_in)*2 + 32 );

	pAcceptor->deleteFromPendingList( hFileHandle );

	// 이전의 AcceptEx() 가 무효화 되었으므로 재차 콜을 해 준다.
	pAcceptor->pendAcceptRequest();

	if( bIsSuccess )
	{
		// NIOCPConnection() 을 조립해 아래 receiver 에 주어야 한다.
		XIOCPConnection * pConnection = static_cast< XIOCPConnection* >( pTag->pReceiver->createConnection( nThreadNum, pAcceptor, XSocket( hFileHandle ) ) );
		if ( !pConnection )
			pConnection = static_cast< XIOCPConnection* >( pTag->pReceiver->createConnection( XSocket( hFileHandle ) ) );

		if( pConnection )
		{
			pConnection->SetID( XOBJ_CONNECTION );
			pConnection->onConnect( buf );

			bool bIsNeedAddObject = true;
			// Event Receiver 호출
			if( pTag->pReceiver ) 
				bIsNeedAddObject = pTag->pReceiver->onAccept( nThreadNum, pAcceptor, pConnection );

			// 이것이 꼭 event receiver 보다 늦게 호출 되어야 한다~!
			if( bIsNeedAddObject ) 
			{
				pTag->pIOCPMgr->AddObject( pConnection );
				if( pTag->pReceiver )
					pTag->pReceiver->onAccepted( nThreadNum, pConnection );
			}
		}
	}
	else
	{
		closesocket( hFileHandle );
	}	

	return true;
}


bool XIOCP::onConnectionEvent( IOCPTAG * pTag, int nThreadNum, XOVERLAPPED * pOverlapped, int nSize )
{
	XIOCPConnection * pConnection = static_cast< XIOCPConnection* >( pOverlapped->pObj );

	// _oprint( "IOCP EVENT : 0x%08X\n", pConnection );

	switch( pOverlapped->cFlag )
	{
	case XIOCP_RECV:
		{
			pConnection->onRecvCompletionEvent( nSize );

			if( nSize < 1 )
			{	
				pConnection->decreaseQueryCount();
				pConnection->onDisconnect( 1 );
				break;
			}

			// Event Receiver 호출
			if( pTag->pReceiver ) pTag->pReceiver->onRead( nThreadNum, pConnection );

			// 정상적인 Recv 였다면..
			if( nSize > 0 && pConnection->IsConnected() )
			{
				// 이전의 WSARecv() 가 무효화 되었으므로 재차 콜을 해 준다.
				if( !pConnection->pendRecvRequest() ) 
				{
					pConnection->decreaseQueryCount(); // ???
					break;
				}
			}

			// 쿼리 카운트 감소
			pConnection->decreaseQueryCount();

			// 다른 쓰레드에서 disconnect 이벤트가 발생했다면 연결 끊어짐 처리
			if( pConnection->CheckDisconnectSignal() || !pConnection->IsConnected() )
			{
				pConnection->onDisconnect( 2 );
				break;
			}			
		
		}
		break;

	case XIOCP_SEND:
		{
			// Event Receiver 호출
			if( pTag->pReceiver ) pTag->pReceiver->onWrite( nThreadNum, pConnection );

			// 보내진 데이터를 Send Queue 에서 제거한다
			pConnection->onSendCompletionEvent( nSize );

			// 쿼리 카운트 감소
			pConnection->decreaseQueryCount();

			// nSize 가 1 이하이면 연결 끊어진것임
			if( nSize < 1 )
			{
				pConnection->onDisconnect( 3 );
				break;
			}

			// 다른 쓰레드에서 disconnect 이벤트가 발생했다면 연결 끊어짐 처리
			if( pConnection->CheckDisconnectSignal() )
			{
				pConnection->onDisconnect( 4 );
				break;
			}
		}
		break;

	case XIOCP_CONNECT:
		{
			if( nSize >= 0 )
			{
				pConnection->onConnectCompletionEvent( true );

				// Event Receiver 호출
				if( pTag->pReceiver ) 
					pTag->pReceiver->onConnect( nThreadNum, pConnection );
			}
			else
			{
				pConnection->onConnectCompletionEvent( false );

				// Event Receiver 호출
				if( pTag->pReceiver ) 
					pTag->pReceiver->onCantConnect( nThreadNum, pConnection );
			}
		}
	}

	return true;
}

bool XIOCP::onDatagramEvent(IOCPTAG *pTag, int nThreadNum, XOVERLAPPED *pOverlapped, int nSize)
{
	XIOCPDatagram *pDatagram = static_cast<XIOCPDatagram *>(pOverlapped->pObj);

	switch(pOverlapped->cFlag)
	{
		case XIOCP_DATAGRAMRECV:
			{
				pDatagram->OnRecvFromCompletionEvent(nSize);

				if(pTag->pReceiver) pTag->pReceiver->onRead(nThreadNum, pDatagram, nSize);

				pDatagram->OnPendRecvFromRequest();

				if(pDatagram->IsOpened()) while(!pDatagram->PendRecvFromRequest());

				pDatagram->DecreaseQueryCount();
			}
			
			break;
		case XIOCP_DATAGRAMSEND:
			{
				if(pTag->pReceiver) pTag->pReceiver->onWrite(nThreadNum, pDatagram, nSize);

				pDatagram->OnSendToCompletionEvent(nSize);

				pDatagram->DecreaseQueryCount();
			}

			break;
	}

	return true;
}

unsigned __stdcall XIOCP::IOCPWorkerThread( void* pArg )
{
	XError::Debug();

	IOCPTAG * pTag = static_cast< IOCPTAG* >( pArg );

	InterlockedIncrement( pTag->plActiveThreadCount );

	unsigned nThreadNum = 0;
	for( nThreadNum = 0;
		 nThreadNum < pTag->vThreadId.size() && GetCurrentThreadId() != pTag->vThreadId[nThreadNum];
		 nThreadNum++ );	// do nothing (nThreadNum 을 얻기 위한 루프임)

	char buf[1024];
	s_sprintf( buf, _countof( buf ), "IOCP %02d", nThreadNum );
	XSetThreadName( -1, buf );

	if( s_pfInitFunc )
	{
		s_pfInitFunc( nThreadNum );
	}

	int nRtn;
	DWORD dwNumberOfBytes;
	ULONG_PTR ulpCompletionKey;
	XOVERLAPPED *pOverlapped;

	while( true ) 
	{
		try
		{
			// Active thread count 조정  ( 대기모드이므로 1 감소 )
			InterlockedDecrement( pTag->plActiveThreadCount );

			nRtn = GetQueuedCompletionStatus( pTag->hIOCP,
											&dwNumberOfBytes,
											&ulpCompletionKey,
											(OVERLAPPED**)&pOverlapped,
											INFINITE );

			while( *pTag->pbIsPaused )
			{
				Sleep( 100 );
			}

			// Active thread count 조정  ( 처리모드이므로 1 증가 )
			InterlockedIncrement( pTag->plActiveThreadCount );

			// Instruction count 조정  ( 처리모드이므로 1 증가 )
			InterlockedIncrement( pTag->plInstructionCount );

			// 쓰레드 종료 시그널이 도착했다면 중지
			if( ulpCompletionKey == XIOCP_EVENT_STOP || pTag->bIsFinished )
			{
				break;
			}

			// 커넥션 끊어짐 이벤트라면..
			if( ulpCompletionKey == XIOCP_EVENT_CONNECTION_CLOSED )
			{
				// _oprint( "DISCONNECT EVENT : 0x%08X\n", static_cast< XIOCPConnection* >( pOverlapped->pObj ) );

				// 이벤트 리시버에 disconnect 이벤트 알려주자
				pTag->pReceiver->onDisconnect( nThreadNum, static_cast< XIOCPConnection* >( pOverlapped->pObj ) );

				InterlockedIncrement( &g_nDisconnectCnt );

				continue;
			}

			// GetQueuedCompletionStatus() 에러라면..
			if( !nRtn )
			{
				int nErrorCode = WSAGetLastError();
				// { ConnectEx 실패의 경우
				if( nErrorCode == WSAECONNREFUSED )
				{	
					if( pTag->pReceiver ) pTag->pReceiver->onCantConnect( nThreadNum, static_cast< IConnection* >( pOverlapped->pObj ) );
					continue;
				}
				// }

				// { 디스커넥트

				if(	nErrorCode == WSAENETDOWN ||
					nErrorCode == WSAENETUNREACH ||
					nErrorCode == WSAENETRESET ||
					nErrorCode == WSAECONNABORTED ||
					nErrorCode == WSAECONNRESET ||
					nErrorCode == WSAETIMEDOUT ||
					nErrorCode == WSAEHOSTDOWN ||
					nErrorCode == WSAEHOSTUNREACH ||
					nErrorCode == WSAEDISCON ||
					nErrorCode == WSA_OPERATION_ABORTED ||

					nErrorCode == ERROR_SEM_TIMEOUT ||
					nErrorCode == ERROR_NETNAME_DELETED ||
					nErrorCode == ERROR_CONNECTION_ABORTED ||
					nErrorCode == ERROR_OPERATION_ABORTED ||

					nErrorCode == ERROR_HOST_UNREACHABLE )	// IP 스푸핑? 패킷 손상? 뭐 여튼 가끔 발생하여 추가...;;
				{
					if( pOverlapped->cFlag == XIOCP_RECV || pOverlapped->cFlag == XIOCP_SEND || pOverlapped->cFlag == XIOCP_CONNECT )
					{
						onConnectionEvent( pTag, nThreadNum, pOverlapped, -1 );
						//_IOCPImpl::onDisconnecEvent( static_cast< XIOCPConnection * >( pOverlapped->pObj ) );
						// _oprint( "GQCS ERR : 0x%08X (%d)\n", static_cast< XIOCPConnection * >( pOverlapped->pObj ), pOverlapped->cFlag );
					}
					else if( pOverlapped->cFlag == XIOCP_DATAGRAMRECV || pOverlapped->cFlag == XIOCP_DATAGRAMSEND )
					{
						onDatagramEvent(pTag, nThreadNum, pOverlapped, 0);
					}
					else
						//XIOCP_ACCEPT관련 에러처리..
						if( pOverlapped->cFlag == XIOCP_ACCEPT )
					{
						// TODO : acceptex 다시 해주자
						onAcceptEvent( pTag, nThreadNum, pOverlapped, false );
						// 로그는 남기도록 한다.
					}

					continue;
				}

				if( pOverlapped->cFlag == XIOCP_ACCEPT )
				{
					// TODO : acceptex 다시 해주자
					onAcceptEvent( pTag, nThreadNum, pOverlapped );
					// 로그는 남기도록 한다.
				}
				else if( pOverlapped->cFlag == XIOCP_RECV || pOverlapped->cFlag == XIOCP_SEND || pOverlapped->cFlag == XIOCP_CONNECT )
				{
					onConnectionEvent( pTag, nThreadNum, pOverlapped, -1 );
					//_IOCPImpl::onDisconnecEvent( static_cast< XIOCPConnection * >( pOverlapped->pObj ) );
					// _oprint( "GQCS ERR : 0x%08X (%d)\n", static_cast< XIOCPConnection * >( pOverlapped->pObj ), pOverlapped->cFlag );
				}
				//else if( pOverlapped->cFlag == XIOCP_DATAGRAMRECV || pOverlapped->cFlag == XIOCP_DATAGRAMSEND )
				//{
				//	onDatagramEvent(pTag, nThreadNum, pOverlapped, 0);
				//}
				// }

				// 기타에러임
				std::string strWinSockError = XNetworkUtil::GetWin32ErrorInfo( nErrorCode );
				// 개행 문자 제거
				if( strWinSockError.size() > 2 )
					strWinSockError.erase( strWinSockError.end() - 2, strWinSockError.end() );
				std::string strWindowsError = XNetworkUtil::GetWin32ErrorInfo( GetLastError() );
				// 개행 문자 제거
				if( strWindowsError.size() > 2 )
					strWindowsError.erase( strWindowsError.end() - 2, strWindowsError.end() );
				strWindowsError.erase( std::remove( strWindowsError.begin(), strWindowsError.end(), '\n' ), strWindowsError.end() );
				FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "IOCP", "ERROR :[%d] %s / %s (%d)\n", nErrorCode, strWindowsError.c_str(), strWinSockError.c_str(), pOverlapped->cFlag );
				_cprint( "IOCP ERROR :[%d] %s / %s (%d)\n", nErrorCode, strWindowsError.c_str(), strWinSockError.c_str(), pOverlapped->cFlag );
				//throw XException( strError );
				continue;
			}

			// 처리
			switch( pOverlapped->pObj->GetID() )
			{
			case XOBJ_ACCEPTOR:
				{
					onAcceptEvent( pTag, nThreadNum, pOverlapped );
					break;
				}

			case XOBJ_CONNECTION:
				{	
					onConnectionEvent( pTag, nThreadNum, pOverlapped, dwNumberOfBytes );
					break;       
				}
			case XOBJ_DATAGRAM:
				{
					onDatagramEvent( pTag, nThreadNum, pOverlapped, dwNumberOfBytes );
					break;
				}
			case XOBJ_NULL:
				break;

			default:
				throw XException( "IOCPWorker : INVALID OBJECT" );
				break;
			}
		}
		catch( std::exception & ex )
		{	
			assert( 0 );
			pTag->pReceiver->onError( ex.what() );
		}
		catch( const char* str )
		{
			assert( 0 );
			pTag->pReceiver->onError( str );
		}
		catch( ... )
		{
			assert( 0 );
			pTag->pReceiver->onError( "IOCP : Unknown\n" );
		}
	}

	// Active thread count 조정  ( 종료모드이므로 1 감소 )
	InterlockedDecrement( pTag->plActiveThreadCount );
	InterlockedDecrement( pTag->plCurrentThreadCount );	

	return 0L;
}






XIOCP::XIOCP( INetworkEventReceiver * pReceiver ) : INetworkEvent( pReceiver )
{
	m_nThreadNum = 0;
	m_lActiveThreadCount = 0;
	m_lInstructionCount = 0;
	m_lCurrentThreadCount = 0;
	m_bIsPaused = false;

	m_pTag = new IOCPTAG( pReceiver );
	m_pTag->pIOCPMgr = this;
	m_pTag->plActiveThreadCount = &m_lActiveThreadCount;
	m_pTag->plInstructionCount = &m_lInstructionCount;
	m_pTag->plCurrentThreadCount = &m_lCurrentThreadCount;
	m_pTag->pbIsPaused = &m_bIsPaused;

	ENV().Bind( "iocp.active", (int*)&m_lActiveThreadCount );
	ENV().Bind( "iocp.instruction", (int*)&m_lInstructionCount );

	ENV().Bind( "iocp.dis_count", (int*)&g_nDisconnectCnt );
}

XIOCP::~XIOCP()
{
	if( m_pTag->hIOCP ) DeInit();

	delete m_pTag;
}

bool XIOCP::Init()
{
	m_pTag->hIOCP = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );

	if( m_pTag->hIOCP )
	{
		m_nThreadNum = 0;
		m_lCurrentThreadCount = 0;
		m_lActiveThreadCount = 0;
		m_lInstructionCount = 0;
		m_bIsPaused = false;

		return true;
	}

	XError::XSetLastError( "CreateIoCompletionPort() error" );

	return false;
}

bool XIOCP::DeInit()
{
	if( !m_pTag->hIOCP )
	{
		XError::XSetLastError( "invalid IOCP handle" );
		return false;
	}

	if( !CloseHandle( m_pTag->hIOCP ) )
	{
		throw XException( "Can't destroy IOCP handle" );
	}

	m_pTag->hIOCP = NULL;
	
	return true;
}

XOVERLAPPED * XIOCP::allocOverlapped()
{
	return m_pTag->pOverlappedAllocator->allocOverlapped();
}

void XIOCP::freeOverlapped( XOVERLAPPED * ptr )
{
	return m_pTag->pOverlappedAllocator->freeOverlapped( ptr );
}

struct OverlappedAllocator* XIOCP::getOverlappedAllocator()
{
	return m_pTag->pOverlappedAllocator;
}

void XIOCP::Pause()
{
	ENV().Set( "iocp.paused", 1 );
	m_bIsPaused = true;
}

void XIOCP::Resume()
{
	ENV().Remove( "iocp.paused" );
	m_bIsPaused = false;
}

bool XIOCP::AddObject( IBaseObject * pObj )
{
	if( !m_pTag->hIOCP ) return false;

	XIOCPAcceptor*		pAcceptor = NULL;
	XIOCPConnection*	pConnection = NULL;
	XIOCPDatagram*		pDatagram = NULL;
	
	HANDLE				hFileHandle;

	// 객채 얻어옴
	switch( pObj->GetID() )
	{
		case XOBJ_CONNECTION:
			pConnection = static_cast< XIOCPConnection * >( pObj );
			hFileHandle = reinterpret_cast< HANDLE >( pConnection->GetSocketHandle() );
			pConnection->m_hIOCP = m_pTag->hIOCP;
			break;
		case XOBJ_ACCEPTOR:
			pAcceptor	= static_cast< XIOCPAcceptor * >( pObj );
			hFileHandle = reinterpret_cast< HANDLE >( pAcceptor->GetSocketHandle() );
			break;
		case XOBJ_DATAGRAM:
			pDatagram = static_cast< XIOCPDatagram *>( pObj );
			hFileHandle = reinterpret_cast< HANDLE >( pDatagram->GetSocketHandle() );
			break;
		default:
			throw XException( "XIOCP::AddObject error #1" );
			break;
	}

	// 기본 IOCP 핸들에 등록
	if( CreateIoCompletionPort( hFileHandle,  m_pTag->hIOCP, (ULONG_PTR)hFileHandle, 0 ) == NULL )
	{
		return false;
	}
	
	// IO 미리 요청
	switch( pObj->GetID() )
	{
		case XOBJ_CONNECTION:
			// ReadFile
			if( pConnection->IsConnected() ) pConnection->pendRecvRequest();
			break;

		case XOBJ_ACCEPTOR:
			// AcceptEx 를 미리 요청해 놓는다.
			pAcceptor->pendAcceptRequest();
			break;
		
		case XOBJ_DATAGRAM:
			// RecvFrom을 미리 요청해 놓는다.
			if( pDatagram->IsOpened() ) pDatagram->PendRecvFromRequest();
			break;
	}

	return true;
}

bool XIOCP::DelObject( IBaseObject * pObj )
{
	//XIOCPAcceptor*		pAcceptor;
	//XIOCPConnection*	pConnection;
	//XIOCPDatagram*		pDatagram;
	//
	//HANDLE				hFileHandle;

	//switch( pObj->GetID() )
	//{
	//	case XOBJ_CONNECTION:
	//		pConnection = static_cast< XIOCPConnection * >( pObj );
	//		hFileHandle = reinterpret_cast< HANDLE >( pConnection->GetSocketHandle() );
	//		break;
	//	case XOBJ_ACCEPTOR:
	//		pAcceptor	= static_cast< XIOCPAcceptor * >( pObj );
	//		hFileHandle = reinterpret_cast< HANDLE >( pAcceptor->GetSocketHandle() );
	//		break;
	//	case XOBJ_DATAGRAM:
	//		pDatagram = static_cast< XIOCPDatagram *>( pObj );
	//		hFileHandle = reinterpret_cast< HANDLE >( pDatagram->GetSocketHandle() );
	//		break;
	//	default:
	//		throw XException( "XIOCP::DelObject error #1" );
	//		break;
	//}

	//::CloseHandle( hFileHandle );

	return true;
}

bool XIOCP::StartThreadPool( unsigned nThreadNum, void (*init_func)( int ), bool bRegisterMonitoring )
{
	if( !m_pTag->hIOCP )
	{
		XError::XSetLastError( "Invalid IOCP handle" );
		return false;
	}

	m_pTag->vThreadHandle.reserve( nThreadNum*2 );

	unsigned	nErrCnt = 0;
	unsigned 	dwThreadID;

	unsigned	i;

	s_pfInitFunc = init_func;

	for( i = 0; i < nThreadNum; i++ )
	{	
		InterlockedIncrement( &m_lCurrentThreadCount );

		unsigned 	hThread;
		if( (hThread = (unsigned)_beginthreadex( NULL, 0, IOCPWorkerThread, m_pTag, CREATE_SUSPENDED, &dwThreadID )) > 0 )
		{
			if( !hThread ) {
				nErrCnt++;
				if( nErrCnt < nThreadNum ) {
					i--;
					continue;
				}
				else break;
			}

			m_pTag->vThreadHandle.push_back( hThread );
			m_pTag->vThreadId.push_back( dwThreadID );

			if( bRegisterMonitoring )
			{
				char szThreadName[64];
				s_sprintf( szThreadName, sizeof( szThreadName ), "IOCPWorker_%d", i );
				XThreadMonitor::AddWatchingThread( dwThreadID, std::string( szThreadName ) );
			}
		}
	}

	if( i != nThreadNum )
	{
		XError::XSetLastError( "Can't create thread" );
		return false;
	}

	for( i = 0; i < nThreadNum; i++ )
	{
		ResumeThread( (HANDLE)m_pTag->vThreadHandle[i] );
	}

	m_pTag->bIsFinished = false;

	m_nThreadNum = nThreadNum;

	ENV().Bind( "iocp.total", (int*)&m_nThreadNum );

	return true;
}

bool XIOCP::IncreaseThreadPool( unsigned nThreadNum )
{
	if( !m_pTag->hIOCP )
	{
		XError::XSetLastError( "Invalid IOCP handle" );
		return false;
	}

	unsigned	nErrCnt = 0;
	unsigned 	dwThreadID;

	unsigned	i;

	nThreadNum += m_nThreadNum;

	for( i = m_nThreadNum; i < nThreadNum; i++ )
	{
		uintptr_t 	hThread;
		if( (hThread = _beginthreadex( NULL, 0, IOCPWorkerThread, m_pTag, CREATE_SUSPENDED, &dwThreadID )) > 0 )
		{
			if( !hThread ) {
				nErrCnt++;
				if( nErrCnt < nThreadNum ) {
					i--;
					continue;
				}
				else break;
			}

			m_pTag->vThreadHandle.push_back( hThread );
			m_pTag->vThreadId.push_back( dwThreadID );
		}
	}

	if( i != nThreadNum )
	{
		XError::XSetLastError( "Can't create thread" );
		return false;
	}

	for( i = m_nThreadNum; i < nThreadNum; i++ )
	{
		ResumeThread( (HANDLE)m_pTag->vThreadHandle[i] );
	}

	return true;
}


bool XIOCP::EndThreadPool()
{
	if( !m_pTag->hIOCP ) return false;

		unsigned nErrCnt = 0;
	std::vector< uintptr_t >::iterator it;
	std::vector< uintptr_t > & vThreadHandle = m_pTag->vThreadHandle;

	m_pTag->bIsFinished = false;

	// { 모든 쓰레드에게 종료 시그널(XIOCP_EVENT_STOP)을 전송한다
	for( it = vThreadHandle.begin() ; it != vThreadHandle.end(); ++it )
	{
		 if( !PostQueuedCompletionStatus( m_pTag->hIOCP, 0, static_cast< ULONG_PTR >( XIOCP_EVENT_STOP ), NULL ) ) nErrCnt++;
	}
	if( nErrCnt )
	{
		return false;
	}
	// }


	// { 모든 쓰레드가 마칠때까지 대기한다
	while( m_lCurrentThreadCount ){
		Sleep( 100 );
	}


	for( it = vThreadHandle.begin() ;
		 it != vThreadHandle.end(); ++it ) {

		 CloseHandle( (HANDLE)*it );
	}

	// 쓰래드 핸들 벡터 초기화
	vThreadHandle.clear();
	
	return true;
}

struct _OverlappedAllocatorData
{
	_OverlappedAllocatorData() : overlappedHeap( sizeof(XOVERLAPPED), 512, 8 )	{}

	XMemoryPool											overlappedHeap;
	XSpinLock											heapLock;
};

OverlappedAllocator::OverlappedAllocator()
{
	if( ENV().GetInt( "iocp.useheap", 0 ) == 0 )
	{
		m_bUseHeap = false;
	}
	else
	{
		m_bUseHeap = true;
	}

	m_pData = new _OverlappedAllocatorData;
}

OverlappedAllocator::~OverlappedAllocator()
{
	delete m_pData;
}

XOVERLAPPED * OverlappedAllocator::allocOverlapped()
{	
	return static_cast< XOVERLAPPED * >( m_pData->overlappedHeap.Alloc() );
}

void OverlappedAllocator::freeOverlapped( XOVERLAPPED * ptr )
{
	if( !HasOverlappedIoCompleted( ptr ) ) 
	{
		//assert( 0 );
	}

	m_pData->overlappedHeap.Free( ptr ); 
}

void OverlappedAllocator::Lock()
{
	m_pData->heapLock.Lock();
}

void OverlappedAllocator::UnLock()
{
	return m_pData->heapLock.UnLock();
}

bool OverlappedAllocator::IsLocked() const
{
	return m_pData->heapLock.IsLocked();
}