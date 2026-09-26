
#pragma once

#include "IConnection.h"
#include "../../include/toolkit/ILock.h"
#include <process.h>

// 이 Connection 구조체를 쓰레드 빼서 돌렸을 때 나중에 쓰레드 종료 시 기다릴 시간. 우선 5초로 설정..
const int WAITING_TIME_FOR_THREAD_TO_DIE	= 5 * 1000;
const int RECV_BUF_SIZE_DEFAULT				= 8 * 1024;
const int SEND_BUF_SIZE_DEFAULT				= 4 * 1024;

struct XSyncStreamConnection	: public IStreamSocketConnection 
{
	XSyncStreamConnection( bool bUseCipher = false, bool bUseThreadForIO = false )
	{
		init( bUseCipher, bUseThreadForIO );
	}

	XSyncStreamConnection( XSocket sock, bool bUseCipher = false, bool bUseThreadForIO = false ) : IStreamSocketConnection( sock )
	{
		init( bUseCipher, bUseThreadForIO );
	}

	virtual ~XSyncStreamConnection();

	int		Read( void * szBuf, size_t nLen );
	int		Write( const void * szBuf, size_t nLen );
	int		Peek( void * szBuf, size_t nLen );

	bool	IsReadable();
	bool	IsWriteable();

	bool	Connect( const XAddr & addr );
	bool	Close();
	static unsigned __stdcall NetworkIOProc( void * pArg );

private:

	void	init( bool bUseCipher, bool bUseThreadForIO );
	void	Select();

	fd_set	m_rdSet, m_wrSet, m_exSet;
	struct ICipher *m_pSendCipher;
	struct ICipher *m_pRecvCipher;
	struct IQueue  *m_pSendQueue;
	struct IQueue  *m_pRecvQueue;

	bool	m_bUseThreadForIO;
	HANDLE	m_hIOThreadHandle;
	volatile bool	m_bThreadToBeTerminated;
	XCriticalSection *m_csSendQueueLock;
	XCriticalSection *m_csRecvQueueLock;
};
