
#pragma once

/*
 *	XIOCPStruct.h
 *
 *  by Testors
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Mswsock.h>

#include "../toolkit/ILock.h"
#include "../toolkit/XEnv.h"

const int	IOCP_BUFFER_SIZE = 16384;	// 한번에 이 이상은 보내지 않음

const char	XIOCP_NULL		= 0x01;
const char	XIOCP_CONNECT	= 0x02;
const char	XIOCP_ACCEPT	= 0x03;
const char	XIOCP_DATAGRAM	= 0x04;
const char	XIOCP_SEND		= 0x05;
const char	XIOCP_RECV		= 0x06;
const char	XIOCP_CLOSE		= 0x07;
const char	XIOCP_DATAGRAMSEND		= 0x08;
const char	XIOCP_DATAGRAMRECV		= 0x09;

const int	XIOCP_HEAP_BLOCK	= 1024;

const int	XIOCP_EVENT_STOP					= 0xffffffff;
const int	XIOCP_EVENT_CONNECTION_CLOSED		= 0xfffffffe;

struct XOVERLAPPED : OVERLAPPED
{
	HANDLE				hFileHandle;
	DWORD				dwSize;
	IBaseObject*		pObj;
	char				cFlag;
	char *				pBuf;
	
	void Init( HANDLE _hFileHandle = NULL, IBaseObject * _pObj = NULL, char *_pBuf = NULL )
	{
		Internal	= 0; 
		InternalHigh= 0; 
		Offset		= 0; 
		OffsetHigh	= 0; 
		hEvent		= NULL; 
		dwSize		= 0;
		hFileHandle	= _hFileHandle;
		cFlag		= XIOCP_NULL;
		pObj		= _pObj;
		pBuf		= _pBuf;
	}
	
	XOVERLAPPED( HANDLE _hFileHandle = NULL, IBaseObject * _pObj = NULL, char *_pBuf = NULL )
	{
		Init( _hFileHandle, _pObj, _pBuf );
	}

	~XOVERLAPPED()
	{
	}
};

struct OverlappedAllocator : ILock
{
	OverlappedAllocator();
	virtual ~OverlappedAllocator();

	XOVERLAPPED * allocOverlapped();
	void freeOverlapped( XOVERLAPPED * ptr );

	virtual void Lock();
	virtual bool IsLocked() const;
	virtual void UnLock();

private:

	bool m_bUseHeap;
	struct _OverlappedAllocatorData * m_pData;
};

struct	XIOCPQueueInfo
{
	static XIOCPQueueInfo&	GetInstance()
	{
		static XIOCPQueueInfo queueInfo;
		return queueInfo;
	}

	XIOCPQueueInfo()
	{
		nMaxReallocSize = 0;
		ENV().Bind( "iocp.max_realloc_size", &nMaxReallocSize );

		nBaseQueueSize = ENV().GetInt( "iocp.bufsize", 8192 );
		ENV().Bind( "iocp.bufsize", &nBaseQueueSize );

		Update();
	}

	void	Update()
	{
		nSendQueueSize = nBaseQueueSize * 8;
		nRecvQueueSize = nBaseQueueSize;
	}

	int nBaseQueueSize;
	int	nSendQueueSize;
	int	nRecvQueueSize;

	int	nMaxReallocSize;
};


struct	XIOCPIOStat
{
	static XIOCPIOStat&	GetInstance()
	{
		static XIOCPIOStat iocpIOStat;
		return iocpIOStat;
	}

	XIOCPIOStat()
	{
		nSendCount = 0;
		nSendTraffic = 0;
		nSendRestBytes = 0;
		nRecvCount = 0;
		nRecvTraffic = 0;
		nRecvRestBytes = 0;

		ENV().Bind( "iocp.send_count", (int*) &nSendCount );
		ENV().Bind( "iocp.send_traffic", (int*) &nSendTraffic );
		ENV().Bind( "iocp.send_rest_bytes", (int*) &nSendRestBytes );
		ENV().Bind( "iocp.recv_count", (int*) &nRecvCount );
		ENV().Bind( "iocp.recv_traffic", (int*) &nRecvTraffic );
		ENV().Bind( "iocp.recv_rest_bytes", (int*) &nRecvRestBytes );
	}

	volatile LONG	nSendCount;
	volatile LONG	nSendTraffic;
	volatile LONG	nSendRestBytes;
	volatile LONG	nRecvCount;
	volatile LONG	nRecvTraffic;
	volatile LONG	nRecvRestBytes;
};

