
#include <cstdlib>

#include "../../include/network/XIOCPDatagram.h"
#include "../../include/network/XIOCPStruct.h"
#include "../../include/network/XNetworkUtil.h"
#include "../../include/cipher/XRC4Cipher.h"
#include "../../include/toolkit/XEnv.h"
#include "../../include/logging/FileLog.h"


//--------------------------------------------------------------------------
// File Name: XIOCPDatagram.cpp
// Description: XIOCPDataram를 구현합니다.
// Created: 2006. 02. 21
// Author: Myung-Gon Park <myunggoni@nflavor.com>
//--------------------------------------------------------------------------

XIOCPDatagram::XIOCPDatagram(OverlappedAllocator *pAllocator, bool bUseCipher) : m_SendCS( ".XIOCPDatagram_Send" ), m_RecvCS( ".XIOCPDatagram_Recv" )
{
	Init(pAllocator, bUseCipher);
}

XIOCPDatagram::XIOCPDatagram(OverlappedAllocator *pAllocator, XSocket sock, bool bUseCipher) : ISocketDatagram(sock)
{
	Init(pAllocator, bUseCipher);
}

XIOCPDatagram::~XIOCPDatagram()
{
	delete m_pSendQueue;
	delete m_pRecvQueue;

	delete m_pSendCipher;
	delete m_pRecvCipher;

	delete [] m_pRecvOverlapped->pBuf;
	
	THREAD_SYNCRONIZE(m_pAllocator);
	
	m_pAllocator->freeOverlapped(m_pRecvOverlapped);
	m_pAllocator->freeOverlapped(m_pSendOverlapped);
}

void XIOCPDatagram::Init(OverlappedAllocator *pAllocator, bool bUseCipher)
{
	m_pAllocator = pAllocator;

	m_pSendQueue = IQueue::MakeQueue(XIOCPQueueInfo::GetInstance().nSendQueueSize, IQueue::PLAIN);
	m_pRecvQueue = IQueue::MakeQueue(XIOCPQueueInfo::GetInstance().nRecvQueueSize, IQueue::PLAIN);

	{
		THREAD_SYNCRONIZE(m_pAllocator);

		m_pRecvOverlapped = m_pAllocator->allocOverlapped();
		m_pRecvOverlapped->Init(reinterpret_cast<HANDLE>(m_socket.GetSocketHandle()), this, new char[IOCP_BUFFER_SIZE]);
		m_pRecvOverlapped->cFlag = XIOCP_DATAGRAMRECV;
		
		m_pSendOverlapped = m_pAllocator->allocOverlapped();
		m_pSendOverlapped->Init(reinterpret_cast<HANDLE>(m_socket.GetSocketHandle()), this);
		m_pSendOverlapped->cFlag = XIOCP_DATAGRAMSEND;
	}

	m_bSendPending = false;
	m_PendingQueryCount = 0;

	m_PendingRecvQueryCount	= 0;
	m_PendingSendQueryCount	= 0;

	m_IOCP = NULL;

	if(bUseCipher)
	{
		m_pSendCipher = new XRC4Cipher;
		m_pRecvCipher = new XRC4Cipher;

		static_cast<XRC4Cipher *>(m_pSendCipher)->SetKey("}h79q~B%al;k'y $E");
		static_cast<XRC4Cipher *>(m_pRecvCipher)->SetKey("}h79q~B%al;k'y $E");
	}
	else 
	{
		m_pSendCipher = new IDummyCipher;
		m_pRecvCipher = new IDummyCipher;
	}
}

bool XIOCPDatagram::WriteFile()
{
	if(!IsOpened()) return false;

	XOVERLAPPED *pSendOverlapped = m_pSendOverlapped;

	InterlockedIncrement(&m_PendingQueryCount);
	InterlockedIncrement(&m_PendingSendQueryCount);

	struct PacketInfo
	{			
		sockaddr_in addr_in;
		unsigned int len;
	} packetInfo;

	m_pSendQueue->Peek(&packetInfo, sizeof(packetInfo));

	m_SendFlag = 0;
	m_SendWSABUF.buf = const_cast<char *>(m_pSendQueue->GetBuf() + sizeof(packetInfo));
	m_SendWSABUF.len = packetInfo.len;

	int result = WSASendTo(GetSocketHandle(), &m_SendWSABUF, 1, &pSendOverlapped->dwSize, m_SendFlag, reinterpret_cast<const sockaddr *>(m_pSendQueue->GetBuf()), sizeof(packetInfo.addr_in), pSendOverlapped, NULL);

	if(result == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			m_pSendQueue->Read(&packetInfo.addr_in, sizeof(packetInfo.addr_in));
			m_pSendQueue->Read(&packetInfo.len, sizeof(packetInfo.len));
			m_pSendQueue->Read(NULL, packetInfo.len);

			InterlockedDecrement(&m_PendingQueryCount);
			InterlockedDecrement(&m_PendingSendQueryCount);

			return false;
		}
	}

	m_bSendPending = true;

	return true;
}

void XIOCPDatagram::OnSendToCompletionEvent(int size)
{
	InterlockedDecrement(&m_PendingSendQueryCount);

	if(size < 1) return;

	THREAD_SYNCRONIZE(&m_SendCS);

	m_bSendPending = false;

	sockaddr_in addr_in;
	unsigned int len;

	m_pSendQueue->Read(&addr_in, sizeof(addr_in));
	m_pSendQueue->Read(&len, sizeof(len));
	m_pSendQueue->Read(NULL, len);

	if(m_pSendQueue->Size())
	{
		WriteFile();
	}
}

void XIOCPDatagram::OnRecvFromCompletionEvent(int size)
{
	InterlockedDecrement(&m_PendingRecvQueryCount);

	if(size < 1) return;

	if(!m_pRecvQueue->Resize(m_pRecvQueue->Size() + size)) throw XException("XIOCPDatagram Resizing RecvQueue Error!!");
}

void XIOCPDatagram::OnPendRecvFromRequest()
{
	m_RecvCS.UnLock();
}

void XIOCPDatagram::DecreaseQueryCount()
{
	InterlockedDecrement(&m_PendingQueryCount);
}

bool XIOCPDatagram::PendRecvFromRequest()
{
	if(!IsOpened()) return false;

	m_RecvCS.Lock();
	
	bool bResult = true;

	if(m_pRecvQueue->Size() + m_pRecvQueue->FreeSize() > m_pRecvQueue->GetReservedSize()) throw XException("XIOCPDatagram QUEUE OVERFLOW!!");
	
	sockaddr_in addr_in;
	int addr_len = sizeof(addr_in);

	ZeroMemory(&addr_in, sizeof(addr_in));

	char *address = const_cast<char *>(m_pRecvQueue->GetBuf() + m_pRecvQueue->Size());

	m_pRecvQueue->Write(&addr_in, sizeof(addr_in));
	
	char *size = const_cast<char *>(m_pRecvQueue->GetBuf() + m_pRecvQueue->Size());

	m_pRecvQueue->Write(&addr_len, sizeof(addr_len));

	m_RecvWSABUF.buf = const_cast<char *>(m_pRecvQueue->GetBuf() + m_pRecvQueue->Size());
	m_RecvWSABUF.len = m_pRecvQueue->FreeSize();

	InterlockedIncrement(&m_PendingQueryCount);
	InterlockedIncrement(&m_PendingRecvQueryCount);

	m_RecvFlag = 0;

	int result = WSARecvFrom(GetSocketHandle(), &m_RecvWSABUF, 1, &m_pRecvOverlapped->dwSize, &m_RecvFlag, reinterpret_cast<sockaddr *>(address), reinterpret_cast<LPINT>(size), m_pRecvOverlapped, NULL);
	
	if(result == SOCKET_ERROR )
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			bResult = false;

			m_pRecvQueue->Read(&addr_in, sizeof(addr_in));
			m_pRecvQueue->Read(&addr_len, sizeof(addr_len));

			InterlockedDecrement(&m_PendingQueryCount);
			InterlockedDecrement(&m_PendingRecvQueryCount);
					
			OnPendRecvFromRequest();
		}
	}

	return bResult;
}

bool XIOCPDatagram::Open(const XAddr& addr)
{
	if(IsOpened()) return false;

	if(!IsValidSocket() && !m_socket.CreateDatagramSocket()) return false;

	struct sockaddr_in addr_in;

	if(!XNetworkUtil::ConvAddr(addr, addr_in))
	{
		CloseSocket();

		return false;
	}

	if(bind(GetSocketHandle(), reinterpret_cast<struct sockaddr *>(&addr_in), sizeof(addr_in)))
	{
		CloseSocket();

		return false;
	}

	m_MyAddr = addr;

	m_bIsOpened = true;

	return true;
}

bool XIOCPDatagram::Close()
{
	if(m_pSendCipher) m_pSendCipher->Clear();
	if(m_pRecvCipher) m_pRecvCipher->Clear();

	m_bIsOpened = false;

	CloseSocket();

	return true;
}

int XIOCPDatagram::SendTo(const XAddr& addr, const void *buf, unsigned int len)
{
	if(!IsOpened()) return -1;
	
	if(!len) return 0;

	THREAD_SYNCRONIZE(&m_SendCS);

	sockaddr_in addr_in;

	if(!XNetworkUtil::ConvAddr(addr, addr_in)) return -1;

	unsigned int totalLen = sizeof(addr_in) + sizeof(len) + len;
	
	if(m_pSendQueue->FreeSize() < totalLen) return -1;

	m_pSendQueue->Write(&addr_in, sizeof(addr_in));
	m_pSendQueue->Write(&len, sizeof(len));

	unsigned int prevSize = m_pSendQueue->Size();

	m_pSendQueue->Write(buf, len);
		
	m_pSendCipher->Encode(m_pSendQueue->GetBuf() + prevSize, const_cast<char *>(m_pSendQueue->GetBuf() + prevSize), len);

	if(!m_bSendPending) WriteFile();

	return len;
}

int XIOCPDatagram::RecvFrom(XAddr& addr, void *buf, unsigned int len)
{
	if(!IsOpened()) return -1;

	sockaddr_in addr_in;
	int addr_len;

	unsigned readableSize = (std::min)( ( unsigned int )( len + sizeof( addr_in ) + sizeof( addr_len ) ), m_pRecvQueue->Size() );

	m_pRecvQueue->Read(&addr_in, sizeof(addr_in));
	m_pRecvQueue->Read(&addr_len, sizeof(addr_len));
	
	XNetworkUtil::ConvAddr(addr_in, addr);

	readableSize -= sizeof(addr_in) + sizeof(addr_len);

	if(!readableSize) return 0;

	m_pRecvCipher->Decode(m_pRecvQueue->GetBuf(), buf, readableSize);

	m_pRecvQueue->Read(NULL, readableSize);

	return readableSize;
}