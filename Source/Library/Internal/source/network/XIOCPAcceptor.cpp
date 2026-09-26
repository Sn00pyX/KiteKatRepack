
#include <cassert>

#include "../../include/network/XIOCPAcceptor.h"
#include "../../include/network/XIOCPStruct.h"
#include "../../include/network/XNetworkUtil.h"

XIOCPAcceptor::XIOCPAcceptor()
{
	m_pAcceptOverlapped = new XOVERLAPPED( NULL, this, new char[sizeof(sockaddr_in)*2 + 32] );
}

XIOCPAcceptor::~XIOCPAcceptor()
{
	for( std::list< XSocket* >::iterator it = m_lstPendingSocket.begin(); it != m_lstPendingSocket.end(); ++it )
	{
		(*it)->Destroy();

		delete (*it);
	}
	m_lstPendingSocket.clear();

	delete [] m_pAcceptOverlapped->pBuf;
	delete m_pAcceptOverlapped;
}


bool XIOCPAcceptor::pendAcceptRequest()
{	
	if( !m_bIsStarted ) return false;

	bool bRtn = true;

	XSocket	*pNewSocket = new XSocket;

	// 새 소켓을 생성
	if( !pNewSocket->CreateStreamSocket() )
	{
		assert( 0 );
		return false;
	}

	{
		THREAD_SYNCRONIZE( m_Lock );
		m_lstPendingSocket.push_back( pNewSocket );
	}

	ZeroMemory( m_pAcceptOverlapped->pBuf, sizeof(sockaddr_in)*2 + 32 );

	// OVERLAPPED 구조체에 새로운 연결을 받을 소켓 세팅
	m_pAcceptOverlapped->hFileHandle	= reinterpret_cast< HANDLE >( pNewSocket->GetSocketHandle() );
	m_pAcceptOverlapped->cFlag			= XIOCP_ACCEPT;

	DWORD dwByteReceived = 0;
	int nRtn = AcceptEx(
					GetSocketHandle(), pNewSocket->GetSocketHandle(), 
					m_pAcceptOverlapped->pBuf, 0,
					sizeof( sockaddr_in ) + 16, sizeof( sockaddr_in ) + 16,
					&dwByteReceived, m_pAcceptOverlapped );

	if( !nRtn )
	{
		int nErrorCode = WSAGetLastError();
		if( nErrorCode != WSA_IO_PENDING )
		{
			pNewSocket->Destroy();
			deleteFromPendingList( pNewSocket->GetSocketHandle() );

			std::string strError = "pendAcceptRequest() ERROR!";
			strError += XNetworkUtil::GetWin32ErrorInfo( nErrorCode );
			throw XException( strError );
		}
	}

	return bRtn;
}


void XIOCPAcceptor::deleteFromPendingList( SOCKET hSocket )
{
	THREAD_SYNCRONIZE( m_Lock );

	for( std::list< XSocket* >::iterator it = m_lstPendingSocket.begin(); it != m_lstPendingSocket.end(); ++it )
	{
		if( (*it)->GetSocketHandle() == hSocket ) 
		{	
			delete (*it);
			m_lstPendingSocket.erase( it );
			break;
		}
	}
}