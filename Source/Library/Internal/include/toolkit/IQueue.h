// IQueue.h
//
// FIFO Queue 인터페이스와 plain queue & circual queue 의 구현을 제공.
// 옵션으로 thread-safe 지정 가능.
//
// by Testors

#pragma once

#include <malloc.h>

struct IQueueAllocator
{
	virtual void* Alloc( unsigned size )
	{
		return malloc( size );
	}

	virtual void* Realloc( void * ptr, unsigned size )
	{
		return realloc( ptr, size );
	}

	virtual void Free( void* ptr )
	{
		return free( ptr );
	}
};

struct IQueue
{
	enum
	{
		PLAIN				= 0x00,
		CIRCULAR			= 0x01 << 1,
		BLOCK				= 0x01 << 2,
	};

	virtual ~IQueue()	{}

	virtual unsigned Write( const void* pData, unsigned nSize )	= 0;
	virtual unsigned Peek( void* pBuf, unsigned nSize )			= 0;
	virtual unsigned Read( void* pBuf, unsigned nSize )			= 0;
	virtual unsigned Size()										= 0;
	virtual unsigned FreeSize()									= 0;
	virtual unsigned Count()									= 0;
	virtual bool	 Clear()									= 0;
	
	virtual bool	 Reserve( unsigned nSize )					= 0;
	virtual unsigned GetReservedSize()							= 0;
	virtual void	 ResetReservedSize( unsigned nSize )		= 0;
	virtual bool	 Resize( unsigned nSize )					= 0;

	virtual const char* GetBuf()								= 0;

	static IQueue *	 MakeQueue( IQueueAllocator * pAllocator, unsigned nQueueSize, const char cFlag = PLAIN );
	static IQueue *	 MakeQueue( unsigned nDefaultQueueSize, const char cFlag = PLAIN );

protected:

	IQueue() {}
};
