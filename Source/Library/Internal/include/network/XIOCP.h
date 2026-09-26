
#pragma once

#include "INetworkEvent.h"


struct XIOCP : INetworkEvent
{
	XIOCP( INetworkEventReceiver * pReceiver );
	virtual ~XIOCP();

	bool Init();
	bool DeInit();

	bool AddObject( IBaseObject * pObj );
	bool DelObject( IBaseObject * pObj );

	bool StartThreadPool( unsigned nThreadNum, void (*init_func)( int ) = NULL, bool bRegisterMonitoring = false );
	bool IncreaseThreadPool( unsigned nThreadNum );
	bool EndThreadPool();

	struct OverlappedAllocator* getOverlappedAllocator();
	struct XOVERLAPPED * allocOverlapped();
	void freeOverlapped( struct XOVERLAPPED * ptr );

	void Pause();
	void Resume();
	bool IsPaused()					{ return m_bIsPaused; }
	long GetTotalThreadCount()		{ return m_nThreadNum; }
	long GetActiveThreadCount()		{ return m_lActiveThreadCount; }
	long GetInstructionCount()		{ return m_lInstructionCount; }

private:

	static unsigned __stdcall IOCPWorkerThread( void* pArg );

	static bool onAcceptEvent( struct IOCPTAG * pTag, int nThreadNum, struct XOVERLAPPED * pOverlapped, bool bIsSuccess = true );
	static bool onConnectionEvent( struct IOCPTAG * pTag, int nThreadNum, struct XOVERLAPPED * pOverlapped, int nSize );
	static bool onDatagramEvent( struct IOCPTAG * pTag, int nThreadNum, struct XOVERLAPPED * pOverlapped, int nSize );


	struct IOCPTAG * m_pTag;

	volatile bool m_bIsPaused;
	volatile long m_lCurrentThreadCount;
	volatile long m_lActiveThreadCount;
	volatile long m_lInstructionCount;
	unsigned m_nThreadNum;
};

