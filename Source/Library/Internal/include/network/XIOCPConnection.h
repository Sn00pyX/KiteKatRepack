
#pragma once

/*
 *	XIOCPConnection.h
 *
 *  by Testors
 */

#include <list>

#include "IConnection.h"
#include "../toolkit/ILock.h"

// send 시 최대 pending 가능한 패킷 수.
// 이 갯수 이상보다 많은 write 가 일어날경우
// 이후의 것은 queue 에 쌓인 후 이전것이 완료된 후에 처리된다.
const int MAX_PENDING_PACKET = 1; 

class XIOCPConnection : public IStreamSocketConnection
{
public:
	XIOCPConnection( struct OverlappedAllocator *pAllocator, bool bUseCipher = false );
	XIOCPConnection( struct OverlappedAllocator *pAllocator, XSocket sock, bool bUseCipher = false );
	virtual ~XIOCPConnection();

	bool Create();
	bool Close();

	volatile bool CheckDisconnectSignal()		{ return m_bDisconnectSignal;		}
	volatile LONG GetPendingQueryCount()		{ return m_nPendingQueryCount;		}
	volatile LONG GetPendingRecvQueryCount()	{ return m_nPendingRecvQueryCount;	}
	volatile LONG GetPendingSendQueryCount()	{ return m_nPendingSendQueryCount;	}

	bool Connect( const XAddr & addr );
	bool SyncConnect( const XAddr & addr );

	bool Reset();
	void SetDisconnectSignal( bool bSignal )	{ m_bDisconnectSignal = bSignal; }
	
	int	 Write( const void* szBuf, size_t nLen );

	int	 Read( void* szBuf, size_t nLen );
	int	 Peek( void* szBuf, size_t nLen );
	const char* GetBuf();
	int	 Size();

	void		 IncVar()					{ InterlockedIncrement( &m_nVar ); }
	void		 DecVar()					{ InterlockedDecrement( &m_nVar ); }
	volatile int GetVar()					{ return m_nVar; }

protected:

	// 이하 함수들은 구현을 위해 XIOCP 에서 호출하는 것들.
	// 수정시 XIOCPConnection 의 lock 전략을 충분히 이해할것..
	bool onConnectCompletionEvent( bool bFlag );
	void onSendCompletionEvent( int nSize );
	void onRecvCompletionEvent( int nSize );
	void decreaseQueryCount();
	void onConnect( void *pBuf );
	void onDisconnect( int nFlag );

	bool pendRecvRequest();

	friend struct XIOCP;

private:

	// 이하는 절대 외부에서 호출/사용 금지.

	// 공통적인 부분들의 초기화
	void	init( OverlappedAllocator *pAllocator, bool bUseCipher );

	// Send Queue 에 쌓인 내용중 WriteFile() 되지 않은 내용이 있다면 호출
	bool	procWriteFile();

	int m_nFlag;

	// 현재 WriteFile() 되었으나 I/O 완료 통지가 되지 않은 갯수
	volatile int	 m_bSendPending;
	
	// Send Queue 에 쌓인 내용중 현재 전송된 크기.
	volatile int	 m_nSendIndicator;

	volatile LONG	 m_nPendingQueryCount;
	volatile LONG	 m_nPendingRecvQueryCount;
	volatile LONG	 m_nPendingSendQueryCount;

	struct IQueue	*m_pSendQueue;
	struct IQueue	*m_pRecvQueue;

	struct XOVERLAPPED*	m_pSendOverlapped;
	struct XOVERLAPPED*	m_pRecvOverlapped;
	struct XOVERLAPPED*	m_pConnectOverlapped;

	// 꼭! critical section 이어야함. (같은 쓰레드가 재진입 하기때문)
	XCriticalSection	m_sendCS;
	XCriticalSection	m_recvCS;
	XCriticalSection	m_closeCS;
	
	WSABUF				m_RecvWSABUF;
	DWORD				m_dwRecvFlag;
	WSABUF				m_SendWSABUF;
	DWORD				m_dwSendFlag;

	HANDLE m_hIOCP;

	volatile bool		m_bIsPostedDisconnectEvent;
	volatile bool 		m_bDisconnectSignal;

	bool				m_bIsUsingCipher;

	OverlappedAllocator *m_pAllocator;

	struct ICipher *m_pSendCipher;
	struct ICipher *m_pRecvCipher;

	volatile LONG		m_nVar;	
};

class XIOCPConnectionCloser
{
public:

	struct DeleteHandler
	{
		virtual void onDelete( XIOCPConnection* pConn ) = 0;
	};

	XIOCPConnectionCloser( DeleteHandler* pDeleter = NULL );
	~XIOCPConnectionCloser();

	void DeInit();

	void Add( XIOCPConnection* pConn );
	bool Has( XIOCPConnection* pConn );

	void Process();

private:

	std::list< XIOCPConnection* > m_closingConns;
	XCriticalSection m_cs;

	DeleteHandler* m_pDel;
};
