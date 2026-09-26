
#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cassert>


// 2006.10.11 - c_unimempool 과 같은 방식의 free block linking 채택. 메모리 오버헤드 대폭 경감.   by Young-Hyun Joo


template< class Ty >
struct XMallocAllocator : public std::allocator< Ty >
{
	template<class _Ty> struct rebind { typedef XMallocAllocator<_Ty> other; };
	pointer allocate( size_t count )			{ return pointer( ::malloc( count * sizeof(value_type) ) ); }
	void deallocate( pointer p, size_t count )	{ return ::free( p ); }
};

struct HeapAllocHelper
{
	HeapAllocHelper()
	{
		m_hHeapHandle = 0;
		m_bSerialize = true;
	}

	~HeapAllocHelper()
	{
		if( m_hHeapHandle )
		{
			::HeapDestroy( m_hHeapHandle );

			m_hHeapHandle = 0;
		}
	}

	void Create( size_t default_size, bool bSerialize = true )
	{
		m_bSerialize = bSerialize;

		DWORD dwOption = HEAP_GENERATE_EXCEPTIONS;

		if( !m_bSerialize )
		{
			dwOption |= HEAP_NO_SERIALIZE;

		}
		m_hHeapHandle = ::HeapCreate( dwOption, default_size, 0 ); // 500메가는 써야 어디가서 아 좀 서버답구나 싶지.-_-
	}

	void * Alloc( size_t bytes )
	{
		return m_hHeapHandle ? ::HeapAlloc( m_hHeapHandle, m_bSerialize, bytes ) : 0;
	}

	void Free( void * pPtr )
	{
		DWORD dwOption = 0;

		if( !m_bSerialize )
		{
			dwOption |= HEAP_NO_SERIALIZE;

		}

		if( m_hHeapHandle ) ::HeapFree( m_hHeapHandle, dwOption, pPtr );
	}

	HANDLE m_hHeapHandle;
	bool	m_bSerialize;
};

struct XLinearMemoryPool
{
	/*
	 * static size_t getBlockCount( size_t block_size );
	 *
	 *   block_count 를 정해주지 않을경우 대략 50KB 정도의 블럭이 잡힐 수 있도록 한다.
	 *   만약 H/W 가 좋아져서 50KB 정도가 껌값이 되면 BLOCK_SIZE 와 BLOCK_COUNT 를 적당히
	 *   조절할것. 참고로 BLOCK_SIZE 가 4096, BLOCK_COUNT 가 56 일경우 할당크기는 다음과 같다.
	 *
	 *	        size : count : allocation size
	 *	   ----------+-------+----------------
	 *		      16 :  1792 : 28672
	 *		      32 :  1792 : 57344
	 *		      64 :  1792 : 114688
	 *		     128 :  1792 : 229376
	 *		     256 :  1792 : 458752
	 *		     512 :   896 : 458752
	 *		    1024 :   448 : 458752
	 *		    2048 :   224 : 458752
	 *		    4096 :   112 : 458752
	 *		    8192 :    56 : 458752
	 *		   16384 :    56 : 917504
	 *		   32768 :    56 : 1835008
	 */

	static size_t getOptimalBlockCount( size_t block_size )
	{
		const size_t BLOCK_SIZE		= 4096;
		const size_t BLOCK_COUNT	= 56;

			 if ( block_size > BLOCK_SIZE      )	return BLOCK_COUNT;		
		else if ( block_size > BLOCK_SIZE / 2  ) return BLOCK_COUNT * 2;
		else if ( block_size > BLOCK_SIZE / 4  ) return BLOCK_COUNT * 4;
		else if ( block_size > BLOCK_SIZE / 8  ) return BLOCK_COUNT * 8;
		else if ( block_size > BLOCK_SIZE / 16 ) return BLOCK_COUNT * 16;
		else									return BLOCK_COUNT * 32;
	}

	XLinearMemoryPool( HeapAllocHelper * pHeapAllocHelper, size_t block_size, size_t block_count = 0 ) : m_unBlockSize( block_size < sizeof(void*) ? sizeof(void*) : block_size ), m_pHeapAllocHelper( pHeapAllocHelper )
	{
		// block_count 을 지정해 주지 않았을경우 최적으로 설정
		m_unBlockCount = block_count ? block_count : getOptimalBlockCount( block_size );

		size_t flag_size = (block_count + BITS_PER_TYPE-1) / BITS_PER_TYPE;
		m_pBuffer = (char*)m_pHeapAllocHelper->Alloc( m_unBlockSize * m_unBlockCount + sizeof(FLAG_TYPE)*flag_size );

		// 할당 블럭을 미리 찾아 놓는다
		for ( size_t idx = 0; idx < m_unBlockCount-1; ++idx )
		{
			*reinterpret_cast< char** >( m_pBuffer + idx * m_unBlockSize ) = m_pBuffer + (idx+1) * m_unBlockSize;
		}
		*reinterpret_cast< char** >( m_pBuffer + (m_unBlockCount-1) * m_unBlockSize ) = NULL;

		m_pFreeBlock = m_pBuffer;
		m_unCount = 0;

		// 할당 여부 플래그 세팅
		m_pAllocFlag = reinterpret_cast< FLAG_TYPE* >( m_pBuffer + m_unBlockSize * m_unBlockCount );
		memset( m_pAllocFlag, 0, flag_size*sizeof(FLAG_TYPE) );
	};

	~XLinearMemoryPool()
	{
		m_pHeapAllocHelper->Free( m_pBuffer );
	}

	void* Alloc( size_t size )
	{
		// 사이즈가 너무 크다면
		if ( size >m_unBlockSize ) throw std::bad_alloc( "too big size" );

		return Alloc();
	}

	void* Alloc()
	{
		// 이미 가득 차 있다면 예외 발생
		if ( IsFull() ) throw std::bad_alloc( "no free memory block in XLinearMemoryPool" );
        
		// 블럭을 하나 얻어온다
		char *p = m_pFreeBlock;
		m_pFreeBlock = *reinterpret_cast< char** >( p );
		++m_unCount;

		// 디버그 버젼일경우 0xcd 로 초기화
		assert( memset( p, 0xcd, m_unBlockSize ) );
		
		// flag 세팅
		size_t idx = GetIndexFromPtr( p );
		m_pAllocFlag[ idx / BITS_PER_TYPE ] |= ( 1 << ( idx % BITS_PER_TYPE ) );
		
		return p;
	}

	// 해당 포인터가 이 컨테이너의 소유인지 검사
	bool IsMyBlock( void* p ) const
	{		
		return ( p < m_pBuffer + m_unBlockSize * m_unBlockCount && p >= m_pBuffer );
	}

	void PrepareFree( void* p )
	{
		// 내 블록이 아니라면 예외 발생
		if ( !IsMyBlock( p ) ) 
		{
			assert( 0 );
			return;
		}

		// m_pBuffer 에서 m_unBlockSize 단위로 떨어지는 포인터인지 검사
		assert( !( ( (size_t)p - (size_t)m_pBuffer ) % m_unBlockSize ) );

		// flag 만 세팅해 놓는다.
		size_t idx = GetIndexFromPtr( p );
		m_pAllocFlag[ idx / BITS_PER_TYPE ] &= ~( 1 << ( idx % BITS_PER_TYPE ) );
	}

	void Free( void* p )
	{		
		// 내 블록이 아니라면 예외 발생
		if ( !IsMyBlock( p ) ) 
		{
			assert( 0 );
			return;
		}

		// 디버그 버젼일경우 0xdd 로 초기화
		assert( memset( p, 0xdd, m_unBlockSize ) );

		// m_pBuffer 에서 m_unBlockSize 단위로 떨어지는 포인터인지 검사
		assert( !( ( (size_t)p - (size_t)m_pBuffer ) % m_unBlockSize ) );

		// 수거된 포인터를 free block list 에 추가
		*reinterpret_cast< char** >( p ) = m_pFreeBlock;
		m_pFreeBlock = reinterpret_cast< char* >( p );
		--m_unCount;

		// flag 세팅
		size_t idx = GetIndexFromPtr( p );
		m_pAllocFlag[ idx / BITS_PER_TYPE ] &= ~( 1 << ( idx % BITS_PER_TYPE ) );
	}

	size_t GetIndexFromPtr( const void* p ) const	{ return ( (size_t)p - (size_t)m_pBuffer ) / m_unBlockSize; }
	void* GetPtrFromIndex( size_t idx ) const
	{
		assert( ( idx / BITS_PER_TYPE ) < ( GetBlockCount() + BITS_PER_TYPE-1 ) / BITS_PER_TYPE );

		// flag 세팅
		if ( !( ( 1 << (idx % BITS_PER_TYPE) ) & m_pAllocFlag[ ( idx / BITS_PER_TYPE ) ] ) )
		{
			//assert( 0 );
			return NULL;
		}

		return m_pBuffer + idx * GetBlockSize();
	}
 
	bool IsFull() const					{ return m_pFreeBlock == NULL;			}
	bool IsEmpty() const				{ return m_unCount == 0;				}

	size_t GetSize() const				{ return m_unCount;						}
	size_t GetBlockSize() const			{ return m_unBlockSize;					}
	size_t GetBlockCount() const		{ return m_unBlockCount;				}
	const char* GetFirstBlock() const	{ return m_pBuffer;						}

protected:

	typedef unsigned		FLAG_TYPE;
	enum 
	{
		BITS_PER_TYPE = (sizeof(FLAG_TYPE)*8),
	};

	size_t					m_unBlockSize;
	size_t					m_unBlockCount;
	size_t					m_unCount;
	char *					m_pFreeBlock;
	char *					m_pBuffer;
	FLAG_TYPE*				m_pAllocFlag;
	HeapAllocHelper *		m_pHeapAllocHelper;
};

struct XMemoryPool
{
	XMemoryPool( size_t block_size, size_t block_count = 0, size_t default_pool_count = 1024, bool bSerialize = true ) : m_unBlockSize( block_size ), m_pFreePool( NULL ) , m_unCount( 0 )
	{
		// block_count 을 지정해 주지 않았을경우 최적으로 설정
		m_unBlockCount = block_count ? block_count : XLinearMemoryPool::getOptimalBlockCount( block_size );

		m_HeapAllocHelper.Create( m_unBlockCount * block_size * ( default_pool_count > 0 ? default_pool_count : 8 ), bSerialize );

		if( default_pool_count > 0 )
		{
			for( size_t cnt = 0; cnt < default_pool_count; ++cnt )
			{
				XLinearMemoryPool *pPool = new XLinearMemoryPool( &m_HeapAllocHelper, m_unBlockSize, m_unBlockCount );
				m_vPool.push_back( pPool );
				m_pFreePool = pPool;

				// 인덱스를 새로 설정
				m_vTagIndex.push_back( _TAG( m_pFreePool, m_pFreePool->GetFirstBlock(), m_vPool.size()-1 ) );
			}

			std::sort( m_vTagIndex.begin(), m_vTagIndex.end(), _TAG::Less );
		}
	}

	~XMemoryPool()
	{
		std::vector< XLinearMemoryPool* >::iterator it;
		for ( it = m_vPool.begin(); it != m_vPool.end(); ++it )
		{
			delete (*it);
		}
	}

	void* Alloc( size_t size )
	{	
		void* p = getFreeMemoryPool()->Alloc( size );
		
		++m_unCount;

		return p;
	}

	void* Alloc()
	{	
		void* pPtr = getFreeMemoryPool()->Alloc();

		++m_unCount;

		return pPtr;
	}

	size_t GetAllocSize() const
	{
		return m_unBlockCount * m_unBlockSize * m_vPool.size();
	}

	void PrepareFree( void* ptr )
	{
		// 해당 블록을 소유한 XLinearMemoryPool 을 얻는다.
		XLinearMemoryPool* pIt = getBlockOwner( ptr );

		// 없다면 에러
		if ( !pIt )
		{
			assert( 0 );
			return;
		}

		pIt->PrepareFree( ptr );
	}

	void Free( void* ptr )
	{
		// 해당 블록을 소유한 XLinearMemoryPool 을 얻는다.
		XLinearMemoryPool* pIt = getBlockOwner( ptr );

		// 없다면 에러
		if ( !pIt )
		{
			assert( 0 );
			return;
		}

		--m_unCount;

		pIt->Free( ptr );
	}

	size_t GetIndexFromPtr( const void* p ) const
	{
		size_t idx = _find( p, 0, m_vTagIndex.size() );

		idx = m_vTagIndex[idx].pPool->GetIndexFromPtr( p ) + m_vTagIndex[idx].idx * GetBlockCount();

		return idx;
	}

	void* GetPtrFromIndex( size_t idx ) const
	{
		if ( m_vPool.empty() ) return NULL;

		if ( idx / GetBlockCount() >= m_vPool.size() )
		{
			return NULL;
		}

		return m_vPool[ idx / GetBlockCount() ]->GetPtrFromIndex( idx % GetBlockCount() );
	}

	bool IsEmpty() const					{ return !GetSize();		}

	size_t GetSize() const					{ return m_unCount;			}
	size_t GetBlockSize() const				{ return m_unBlockSize;		}
	size_t GetBlockCount() const			{ return m_unBlockCount;	}

protected:

	// 포인터를 소유한 XLinearMemoryPool 를 빨리 찾기 위한 인덱스 
	struct _TAG
	{
		_TAG( XLinearMemoryPool *_pP, const char *_pB, size_t _idx ) : pPool( _pP ), pFirstBlock( _pB ), idx( _idx ) {}

		static bool Less( const _TAG &lh, const _TAG & rh )		{ return lh.pFirstBlock < rh.pFirstBlock ; }

		size_t				idx;
		XLinearMemoryPool	*pPool;
		const char			*pFirstBlock;
	};

	XLinearMemoryPool*  getBlockOwner( void* p )
	{
		// 2진탐색으로 해당 포인터를 소유한 XLinearMemoryPool 을 찾는다.
		size_t idx = _find( p, 0, m_vTagIndex.size() );

		assert( m_vTagIndex[idx].pPool->IsMyBlock( p ) );
		
		return m_vTagIndex[idx].pPool;
	}

	// 해당 포인터를 소유한 XLinearMemoryPool 의 index 를 얻는다. (2진탐색)
	size_t _find( const void* p, size_t begin, size_t end ) const
	{
		while ( begin != end )
		{
			size_t tmp = ( begin + end ) / 2;

			if ( tmp == begin ) 
				break;

			if ( m_vTagIndex[tmp].pFirstBlock <= p ) 
				begin = tmp;
			else
				end = tmp;
		}
		return begin;
	}

	// full 이 아닌 XLinearMemoryPool 을 찾아 리턴한다. ( searchFreeMemoryPool 보다 약간 빠르다. )
	XLinearMemoryPool* getFreeMemoryPool()
	{
		if ( !m_pFreePool || m_pFreePool->IsFull() ) return searchFreeMemoryPool();

		return m_pFreePool;
	}

	// full 이 아닌 XLinearMemoryPool 을 찾아 리턴한다. (없으면 생성한다)
	XLinearMemoryPool* searchFreeMemoryPool()
	{
		// 비어있는 XLinearMemoryPool 이 있다면 리턴 
		// ( TODO : 이부분도 미리 free pool list 를 관리하면 좀더 빨라질듯.. )
		std::vector< XLinearMemoryPool* >::iterator it;
		for ( it = m_vPool.begin(); it != m_vPool.end(); ++it )
		{
			if ( (*it)->IsFull() ) continue;
			
			m_pFreePool = *it;
			return m_pFreePool;
		}

		// 새 pool 추가7
		XLinearMemoryPool *pPool = new XLinearMemoryPool( &m_HeapAllocHelper, m_unBlockSize, m_unBlockCount );
		m_vPool.push_back( pPool );
		m_pFreePool = m_vPool.back();

		// 인덱스를 새로 설정
		m_vTagIndex.push_back( _TAG( m_pFreePool, m_pFreePool->GetFirstBlock(), m_vPool.size()-1 ) );
		std::sort( m_vTagIndex.begin(), m_vTagIndex.end(), _TAG::Less );
		
		return m_pFreePool;
	}

	size_t																	m_unCount;
	XLinearMemoryPool*														m_pFreePool;
	std::vector< XLinearMemoryPool* >										m_vPool;
	std::vector< _TAG >														m_vTagIndex;
	size_t																	m_unBlockSize;
	size_t																	m_unBlockCount;
	HeapAllocHelper															m_HeapAllocHelper;
};

template < typename T >
struct XUnitPool : XMemoryPool
{
	XUnitPool( size_t block_count = 0 ) : XMemoryPool( sizeof(T), block_count ) 
	{
	}

	T* Alloc()
	{
		T* p = reinterpret_cast< T* >( XMemoryPool::Alloc() );
		if ( p )
		{
			try { new (p) T(); }
			catch (...) { Free(p); throw; }
		}
		return p;
	}

	void Free( T* p )
	{
		p->~T();
		XMemoryPool::Free( p );
	};
};
