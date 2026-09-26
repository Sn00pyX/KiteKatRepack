
#pragma once

#include "IDatagram.h"
#include "XIOCP.h"
#include "XIOCPStruct.h"

#include "../toolkit/IQueue.h"
#include "../cipher/ICipher.h"

//--------------------------------------------------------------------------
// File Name: XIOCPDatagram.h
// Description: XIOCPDataram를 정의합니다.
// Created: 2006. 02. 21
// Author: Myung-Gon Park <myunggoni@nflavor.com>
//--------------------------------------------------------------------------

class XIOCPDatagram : public ISocketDatagram
{
public:
	XIOCPDatagram(OverlappedAllocator *pAllocator, bool bUseCipher = false);
	XIOCPDatagram(OverlappedAllocator *pAllocator, XSocket sock, bool bUseCipher = false);
	virtual ~XIOCPDatagram();

	bool Open(const XAddr& addr);
	bool Close();

	int SendTo(const XAddr& addr, const void *buf, unsigned int len);
	int RecvFrom(XAddr& addr, void *buf, unsigned int len);

protected:
	void OnSendToCompletionEvent(int size);
	void OnRecvFromCompletionEvent(int size);
	void OnPendRecvFromRequest();
	void DecreaseQueryCount();
	bool PendRecvFromRequest();

	friend XIOCP;

private:
	void Init(OverlappedAllocator *pAllocator, bool bUseCipher);
	bool WriteFile();

	ICipher *m_pSendCipher;
	ICipher *m_pRecvCipher;

	IQueue *m_pSendQueue;
	IQueue *m_pRecvQueue;

	volatile int m_bSendPending;

	volatile LONG m_PendingQueryCount;
	volatile LONG m_PendingRecvQueryCount;
	volatile LONG m_PendingSendQueryCount;

	OverlappedAllocator *m_pAllocator;

	XOVERLAPPED *m_pSendOverlapped;
	XOVERLAPPED *m_pRecvOverlapped;

	WSABUF m_RecvWSABUF;
	WSABUF m_SendWSABUF;
	
	DWORD m_RecvFlag;
	DWORD m_SendFlag;

	HANDLE m_IOCP;
	
	XCriticalSection m_SendCS;
	XCriticalSection m_RecvCS;
};
