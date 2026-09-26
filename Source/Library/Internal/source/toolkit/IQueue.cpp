
#include <cassert>
#include <cstdlib>
#include <cstring>

#include "../../include/toolkit/IQueue.h"
#include "../../include/toolkit/ILock.h"

#include "../../include/dump/XException.h"
#include "../../include/dump/XExceptionHandler.h"

class XPlainQueue : public IQueue
{
public:

	XPlainQueue( IQueueAllocator * pAllocator, unsigned nLength = 8192 );
	~XPlainQueue();

	unsigned	Write( const void* pData, unsigned nSize );
	unsigned	Peek( void* pBuf, unsigned nSize );
	unsigned	Read( void* pBuf, unsigned nSize );
	bool		Clear();

	unsigned	Size()								{ return m_nSize; }
	unsigned	FreeSize()							{ return m_nLength - m_nSize; }
	unsigned	Count()								{ return 1; }
	const char* GetBuf()							{ return m_pData; }

	bool		Reserve( unsigned nSize )			{ return increaseBuffer( nSize ); }

	bool		Resize( unsigned nSize )
	{
		if ( m_nLength < nSize ) return false;
		
		m_nSize = nSize;
		return true;
	}

	unsigned	GetReservedSize()					{ return m_nLength; }
	void		ResetReservedSize( unsigned nSize )
	{
		// 사용중인 양보다 작을 수 없다.
		if( Size() > nSize )
		{
			return;
		}

		m_pData = static_cast< char* >( m_pAllocator->Realloc( m_pData, nSize ) );
		m_nLength = nSize;
	}

private:

	unsigned	freeSize()						{ return m_nLength - m_nSize; }
	bool		increaseBuffer( unsigned nSize );

	char*		m_pData;
	unsigned	m_nSize;
	unsigned	m_nLength;

	IQueueAllocator * m_pAllocator;
};

XPlainQueue::XPlainQueue( IQueueAllocator * pAllocator, unsigned nLength )
{
	m_pAllocator = pAllocator;
	m_pData = static_cast< char* >( m_pAllocator->Alloc( nLength ) );
	if ( !m_pData )
	{
		XSEH::InvokeUnhandledException( 0xC000000DL );
		throw XException( "XPlainQueue::Alloc() error!" );
	}
	m_nLength = nLength;
	m_nSize = 0;
}

XPlainQueue::~XPlainQueue()
{
	m_pAllocator->Free( m_pData );
}

bool XPlainQueue::increaseBuffer( unsigned nSize )
{
	m_pData = static_cast< char* >( m_pAllocator->Realloc( m_pData, m_nLength + nSize ) );

	m_nLength += nSize;

	return true;
}

unsigned XPlainQueue::Write( const void* pData, unsigned nSize )
{
	if ( !nSize ) return 0;

	assert( nSize && pData );

	unsigned int nFreeSize = freeSize();
	if ( nFreeSize < nSize )
	{
		if ( !increaseBuffer( nSize - nFreeSize ) ) return 0;
	}

	s_memcpy( &m_pData[ m_nSize ], freeSize(), pData, nSize );

	m_nSize += nSize;

	return nSize;
}

unsigned XPlainQueue::Read( void *pBuf, unsigned nSize )
{
	assert( nSize && Size() );

	unsigned nReadSize = (std::min)( nSize, Size() );

	if ( pBuf ) s_memcpy( pBuf, nSize, m_pData, nReadSize );

	if ( Size() - nReadSize )
	{
		s_memmove( m_pData, Size(), m_pData + nReadSize, Size() - nReadSize );
	}

	m_nSize = Size() - nReadSize;

#ifdef _DEBUG
	memset( m_pData + m_nSize, 0xcd, FreeSize() );
#endif

	return nReadSize;
}

unsigned XPlainQueue::Peek( void *pBuf, unsigned nSize )
{
	assert( Size() && nSize && pBuf );

	unsigned nReadSize = (std::min)( nSize, Size() );

	s_memcpy( pBuf, nSize, m_pData, nReadSize );

	return nReadSize;
}

bool XPlainQueue::Clear()
{
	m_nSize = 0;
	return true;
}

class XCircularQueue : public IQueue
{
public:

	XCircularQueue( IQueueAllocator * pAllocator, unsigned nLength = 8192 );
	~XCircularQueue();

	unsigned	Write( const void* pData, unsigned nSize );
	unsigned	Peek( void* pBuf, unsigned nSize );
	unsigned	Read( void* pBuf, unsigned nSize );
	unsigned	Size();
	unsigned	FreeSize()							{ return freeSize(); }
	unsigned	Count()								{ return 1; }
	bool		Clear();

	bool		Reserve( unsigned nSize )			{ return increaseBuffer( nSize ); }
	unsigned	GetReservedSize()					{ return m_nLength; }
	bool		Resize( unsigned nSize )			{ return false; }

	const char* GetBuf()	{ return 0; }

	void		ResetReservedSize( unsigned nSize )
	{
		if( Size() > nSize )
		{
			return;
		}

		char* pBuf = static_cast< char* >( m_pAllocator->Alloc( nSize ) );

		if ( !pBuf ) throw XException( "XCircularQueue::ResetReservedSize::Alloc() error!" );

		m_nEnd = Read( pBuf, Size() );
		m_nStart = 0;

		m_pAllocator->Free( m_pData );
		m_pData = pBuf;

		m_nLength = nSize;
	}

private:

	unsigned	freeSize();
	bool		increaseBuffer( unsigned nSize );

	char*		m_pData;
	unsigned	m_nStart;
	unsigned	m_nEnd;
	unsigned	m_nLength;

	IQueueAllocator * m_pAllocator;
};

inline unsigned XCircularQueue::Size()
{
	if ( m_nStart == m_nEnd ) return 0;

	if ( m_nStart < m_nEnd )
	{
		return m_nEnd - m_nStart;
	}
	else
	{
		return ( m_nLength - m_nStart ) + m_nEnd; 
	}
}


inline unsigned XCircularQueue::freeSize()
{
	return m_nLength - Size();
}


XCircularQueue::XCircularQueue( IQueueAllocator * pAllocator, unsigned nLength )
{
	m_pAllocator = pAllocator;
	m_pData = static_cast< char* >( m_pAllocator->Alloc( nLength ) );
	if ( !m_pData ) throw XException( "XCircularQueue::Alloc() error!" );
	
	m_nLength = nLength;
	m_nStart = m_nEnd = 0;
}

XCircularQueue::~XCircularQueue()
{
	m_pAllocator->Free( m_pData );
}

unsigned XCircularQueue::Write( const void* pData, unsigned nSize )
{
	assert( pData && nSize );

	char* pSource = static_cast< char* >( const_cast< void* >( pData ) );

	unsigned nFreeSize = freeSize();

	if ( nFreeSize < nSize + 1 )
	{
		if ( !increaseBuffer( ( nSize - nFreeSize ) + ( m_nLength )  ) ) return 0;
	}

	if ( nSize + m_nEnd > m_nLength )
	{
		unsigned nCut = m_nLength - m_nEnd;
		s_memcpy( &m_pData[ m_nEnd ], m_nLength - m_nEnd, pData, nCut );
		if ( nSize - nCut > 0 ) s_memcpy( m_pData, m_nEnd, &pSource[ nCut ] , nSize - nCut );
		m_nEnd = nSize - nCut;
	}
	else
	{
		s_memcpy( &m_pData[ m_nEnd ], m_nLength - m_nEnd, pData, nSize );
		m_nEnd += nSize;
	}

	return nSize;
}

unsigned XCircularQueue::Peek( void* pBuf, unsigned nSize )
{
	assert( nSize && pBuf );

	unsigned nRtn = Read( pBuf, nSize );

	m_nStart -= nRtn;

	if ( m_nStart < 0 )
	{
		m_nStart = m_nStart + m_nLength;
	}

	return nRtn;
}

unsigned XCircularQueue::Read( void* pBuf, unsigned nSize )
{
	assert( nSize );

	char* pTarget = static_cast< char* >( pBuf );
	
	unsigned nReadableSize = (std::min)( Size(), nSize );
	
	if ( pBuf )
	{
		if ( m_nEnd < m_nStart )
		{
			if ( nReadableSize > m_nLength - m_nStart )
			{
				if ( m_nLength - m_nStart > 0 ) s_memcpy( pBuf, nSize, &m_pData[ m_nStart ] , m_nLength - m_nStart );
			}
			else
			{
				if ( m_nLength - m_nStart > 0 ) s_memcpy( pBuf, nSize, &m_pData[ m_nStart ] , nReadableSize );
			}

			if ( ( nReadableSize - ( m_nLength - m_nStart ) ) > 0 ) 
			{
				s_memcpy( &pTarget[ m_nLength - m_nStart ], nSize - (m_nLength-m_nStart), m_pData, nReadableSize - ( m_nLength - m_nStart ) );
			}
		}
		else
		{
			s_memcpy( pBuf, nSize, &m_pData[ m_nStart ], nReadableSize );
		}
	}

	m_nStart += nReadableSize;

	if ( m_nStart > m_nLength )
	{
		m_nStart = m_nStart - m_nLength;
	}

	return nReadableSize;
}

bool XCircularQueue::Clear()
{
	m_nStart = m_nEnd = 0;
	return true;
}

bool XCircularQueue::increaseBuffer( unsigned nSize )
{
	if ( nSize < m_nLength ) return false;

	char* pBuf = static_cast< char* >( m_pAllocator->Alloc( m_nLength + nSize ) );

	if ( !pBuf ) throw XException( "XCircularQueue::increaseBuffer::Alloc() error!" );

	m_nEnd = Read( pBuf, m_nLength );
	m_nStart = 0;

	m_pAllocator->Free( m_pData );
	m_pData = pBuf;

	m_nLength += nSize;

	return true;
}

class XBlockQueue : public IQueue
{
public:

	XBlockQueue();
	~XBlockQueue();

	unsigned		Write( const void* pData, unsigned nSize );
	unsigned		Peek( void* pBuf, unsigned nSize );
	unsigned		Read( void* pBuf, unsigned nSize );
	unsigned		Count();
	bool			Clear();
	unsigned		Size();
	unsigned		FreeSize()							{ return 1; }

	bool			Reserve( unsigned nSize )			{ return false; }
	unsigned		GetReservedSize()					{ return 0; }
	bool			Resize( unsigned nSize )			{ return false; }

	const char*		GetBuf()			{ return NULL; }
	void			ResetReservedSize( unsigned nSize )	{}

private:

	void Init();
	void DeInit();

	struct Node
	{
		Node* pNext;
		Node* pPrev;
		char* pData;
		unsigned size;
	};

	Node * m_pFirst;
	Node * m_pLast;

	unsigned m_nCount;
};

XBlockQueue::XBlockQueue()
{
	Init();
}

XBlockQueue::~XBlockQueue()
{
	DeInit();
}

void XBlockQueue::Init()
{
	m_pFirst = NULL;
	m_pLast = NULL;

	m_nCount = 0;
}

void XBlockQueue::DeInit()
{
	Node* p = m_pFirst;
	Node* n;

	while ( p )
	{
		n = p->pNext;

		delete [] p->pData;
		delete p;

		p = n;
	}
}

unsigned	XBlockQueue::Write( const void* pData, unsigned nSize )
{
	assert( nSize );

	m_nCount++;

	Node* p = new Node;
	p->pData = new char[ nSize ];
	p->size = nSize * sizeof( char );
	p->pNext = NULL;
	s_memcpy( p->pData, p->size, pData, nSize );

	if ( m_pLast )
	{
		m_pLast->pNext = p;
		p->pPrev = m_pLast;
		m_pLast = p;
	}
	else
	{
		p->pPrev = NULL;
		m_pFirst = p;
		m_pLast = p;
	}

	return nSize;
}

unsigned	XBlockQueue::Peek( void* pBuf, unsigned nSize )
{
	if ( !m_pFirst ) return 0;

	unsigned nReadableSize = 0;
	
	if ( m_pFirst )
	{	
		nReadableSize = (std::min)( m_pFirst->size, nSize );
		s_memcpy( pBuf, nSize, m_pFirst->pData, nReadableSize );
	}

	return nReadableSize;
}

unsigned	XBlockQueue::Read( void* pBuf, unsigned nSize )
{
	if ( !m_pFirst ) return 0;

	unsigned nReadableSize = 0;
	nReadableSize = (std::min)( m_pFirst->size, nSize );

	m_nCount--;
	
	if ( pBuf != NULL )
	{
		s_memcpy( pBuf, nSize, m_pFirst->pData, nReadableSize );
	}

	Node * p;

	p = m_pFirst;

	m_pFirst = m_pFirst->pNext;

	delete [] p->pData;
	delete p;

	if ( m_pFirst )
	{
		m_pFirst->pPrev = NULL;
	}
	else
	{
		m_pLast = NULL;
	}

	return nReadableSize;
}


unsigned	XBlockQueue::Count()
{
	return m_nCount;
}

bool		XBlockQueue::Clear()
{
	DeInit();
	Init();
	return true;
}

unsigned	XBlockQueue::Size()
{
	if ( !m_nCount ) return 0;

	return m_pFirst->size;
}

struct XCipherQueue : public IQueue
{
	XCipherQueue( IQueue * pQueue )
	{
		m_pQueue = pQueue;
	}

	~XCipherQueue()
	{
		delete m_pQueue;
	}

	unsigned	Write( const void* pData, unsigned nSize )
	{
		// pData 를 암호화 한다

		return m_pQueue->Write( pData, nSize );
	}
	
	unsigned	Peek( void* pBuf, unsigned nSize )
	{
		return m_pQueue->Peek( pBuf, nSize );
	}

	unsigned	Read( void* pBuf, unsigned nSize )
	{
		return m_pQueue->Read( pBuf, nSize );
	}

	unsigned	Size()
	{
		return m_pQueue->Size();
	}

	unsigned	FreeSize()
	{
		return m_pQueue->FreeSize();
	}

	bool		Clear()
	{
		return m_pQueue->Clear();
	}

	unsigned	Count()
	{
		return m_pQueue->Count();
	}

	const char* GetBuf()
	{
		return m_pQueue->GetBuf();
	}

	bool Reserve( unsigned nSize )
	{
		return m_pQueue->Reserve( nSize );
	}

	unsigned GetReservedSize()
	{
		return m_pQueue->GetReservedSize();
	}

	void	ResetReservedSize( unsigned nSize )
	{
		m_pQueue->ResetReservedSize( nSize );
	}

	bool Resize( unsigned nSize )
	{
		return m_pQueue->Resize( nSize );
	}

private:

	IQueue	*			m_pQueue;
};


template< typename TLOCK = XCriticalSection >
struct XThreadSafeQueue : public IQueue
{
	XThreadSafeQueue( IQueue * pQueue )
	{
		m_pQueue = pQueue;
	}

	~XThreadSafeQueue()
	{
		m_lock.Lock();

		delete m_pQueue;
	}

	unsigned	Write( const void* pData, unsigned nSize )
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->Write( pData, nSize );
	}
	
	unsigned	Peek( void* pBuf, unsigned nSize )
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->Peek( pBuf, nSize );
	}

	unsigned	Read( void* pBuf, unsigned nSize )
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->Read( pBuf, nSize );
	}

	unsigned	Size()
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->Size();
	}

	unsigned	FreeSize()
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->FreeSize();
	}

	bool		Clear()
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->Clear();
	}

	unsigned	Count()
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->Count();
	}

	const char* GetBuf()
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->GetBuf();
	}

	bool Reserve( unsigned nSize )
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->Reserve( nSize );
	}

	unsigned GetReservedSize()
	{
		XScopedLock cs( &m_lock );

		return m_pQueue->GetReservedSize();
	}

	void	ResetReservedSize( unsigned nSize )
	{
		XScopedLock cs( &m_lock );

		m_pQueue->ResetReservedSize( nSize );
	}

	bool Resize( unsigned nSize )
	{
		XScopedLock cs( &m_lock );
		return m_pQueue->Resize( nSize );
	}

private:

	IQueue	*			m_pQueue;
	TLOCK				m_lock;
};


IQueue * IQueue::MakeQueue( IQueueAllocator * pAllocator, unsigned nQueueSize, const char cFlag )
{
	static IQueueAllocator defaultAllocator;
	if ( !pAllocator ) pAllocator = &defaultAllocator;
	
	IQueue *pQueue = NULL;

	switch( cFlag & ( IQueue::CIRCULAR | IQueue::PLAIN | IQueue::BLOCK ) )
	{

	// 디폴트는 PLAIN Queue
	default:
	case IQueue::PLAIN:
		pQueue = new XPlainQueue( pAllocator, nQueueSize );
		break;

	case IQueue::CIRCULAR:
		pQueue = new XCircularQueue( pAllocator, nQueueSize );
		break;

	case IQueue::BLOCK:
		pQueue = new XBlockQueue();
		break;
	} 

	return pQueue;
}

IQueue * IQueue::MakeQueue( unsigned nDefaultQueueSize, const char cFlag )
{
	return MakeQueue( NULL, nDefaultQueueSize, cFlag );
}
