#ifndef __C_UNIMEMPOOL_V1_1__
#define __C_UNIMEMPOOL_V1_1__

/*
 *     Fast & low overhead fixed unit size memory pool based on c_uniheap, 
 *                                  with dynamic sub-block size control and deleted unit cache.
 *
 *                             2006.10.6 by Young-Hyun Joo
 *
 *           Note : unit size must be equal to or greater than pointer size
 */

#include <memory.h>
#include <vector>
#include <algorithm>
#include <assert.h>

// ----------------------------------------------------------
//
//    Raw memory allocation/deallocation   
//
// ----------------------------------------------------------

template < int UNIT_SIZE >	///< UNIT_SIZE >= sizeof(void*)
class c_unimempool 
{
	struct TBlock;

public:

	c_unimempool( unsigned short blockSize = 0, int cacheSize = 256 ) 
	{
		assert( UNIT_SIZE >= sizeof(void*) );

		// blockSize 를 따로 지정하지 않으면 블럭당 대략 4KB 또는 16 부터 시작

		m_blockSize = (blockSize == 0) ? MIN_BLOCK_SIZE : blockSize;

		m_pFreeBlock = alloc_block( m_blockSize );
		clear_block( m_pFreeBlock );
		m_blocks.push_back( m_pFreeBlock );

		m_pCache = NULL;
		m_cacheCount = 0;
		m_cacheSize = cacheSize;
	}

	~c_unimempool()
	{
		for ( TBlockList::iterator it = m_blocks.begin(); it != m_blocks.end(); it++ ) 
			::free( *it );
	}

	void clear()
	{
		for ( TBlockList::iterator it = m_blocks.begin()+1; it != m_blocks.end(); it++ ) 
			::free( *it );

		clear_block( m_blocks.front() );
		m_blocks.resize( 1 );
		m_pFreeBlock = m_blocks.front();
		m_capacity = m_pFreeBlock->capacity;

		m_pCache = NULL;
		m_cacheCount = 0;
		m_blockSize = MIN_BLOCK_SIZE;
	}

	int capacity() const
	{
		int c = 0;
		for ( TBlockList::const_iterator it = m_blocks.begin(); it != m_blocks.end(); it++ ) 
			c += (*it)->capacity;
		return c;
	}

	int count() const
	{
		int c = 0;
		for ( TBlockList::const_iterator it = m_blocks.begin(); it != m_blocks.end(); it++ ) 
			c += (*it)->count;
		return c - m_cacheCount;
	}

	static int unit_size()	{ return UNIT_SIZE; }

	int block_count() const	{ return int(m_blocks.size()); }

	void* alloc()
	{
		if ( m_pCache != NULL )
		{
			void* pCache = m_pCache;
			m_pCache = *reinterpret_cast< void** >( m_pCache );
			--m_cacheCount;
			return pCache;
		}

		TBlock* pFreeBlock = find_free_block();

		return alloc_from_block( pFreeBlock );
	}

	void free( void* ptr )
	{
		if ( m_blockSize > MIN_BLOCK_SIZE )		// dynamic block size control
            --m_blockSize;

		if ( m_cacheCount < m_cacheSize )
		{
			*reinterpret_cast< void** >( ptr ) = m_pCache;
			m_pCache = ptr;
			++m_cacheCount;
			return;
		}

		_free( ptr );
	}

	template< typename Fn >
	void for_each( Fn fn )
	{
		// clear cache

		for ( int i = 0; i < m_cacheCount; i++ )
		{
			void* pCache = m_pCache;
			m_pCache = *reinterpret_cast< void** >( pCache );
			_free( pCache );
		}
		assert( m_pCache == NULL );
		m_cacheCount = 0;

		// iterate and apply fn

		for ( TBlockList::iterator it = m_blocks.begin(); it != m_blocks.end(); ++it )
		{
			if ( (*it)->count == 0 )
				continue;

			sort_freelist( *it );

			char* pEnd = (*it)->buf + (*it)->capacity * UNIT_SIZE;

			char* pFree = (*it)->pFirstFree;
			if ( pFree == NULL ) pFree = pEnd;

			for ( char* p = (*it)->buf; p < pEnd; p += UNIT_SIZE )
			{
				if ( p < pFree )
				{
					if ( fn( p ) )
						return;
				}
				else
				{
					pFree = *reinterpret_cast< char** >( pFree );
					if ( pFree == NULL ) pFree = pEnd;
				}
			}
		}
	}

private:

	enum { 
		ULONG_BITS = 8*sizeof(unsigned long), 
		_BLOCK_SIZE = (4096 + UNIT_SIZE-1) / UNIT_SIZE, 
		MIN_BLOCK_SIZE = _BLOCK_SIZE > 16 ? _BLOCK_SIZE : 16 
	};

	struct TBlock 
	{
		TBlock*			pNextFreeBlock;
		unsigned short	capacity;
		unsigned short	count;
		char*			pFirstFree;
		char			buf[ sizeof(void*) ];			///< for aligning
	};

	template< class Ty >
	struct TMalloc : public std::allocator< Ty >
	{
		template<class _Ty> struct rebind { typedef TMalloc<_Ty> other; };
		pointer allocate( size_t count )			{ return pointer( ::malloc( count * sizeof(value_type) ) ); }
		void deallocate( pointer p, size_t count )	{ return ::free( p ); }
	};

	typedef std::vector< TBlock*, TMalloc< TBlock* > >	TBlockList;

	TBlock* alloc_block( int blockSize )
	{
		TBlock* pBlock = reinterpret_cast< TBlock* >( ::malloc( sizeof(TBlock) - sizeof(void*) + 
																blockSize * UNIT_SIZE ) );
		pBlock->capacity = blockSize;
		pBlock->pNextFreeBlock = NULL;
		return pBlock;
	}

	void clear_block( TBlock* block )
	{
		block->pFirstFree = block->buf;
		block->count = 0;
		for ( int i = 0; i < block->capacity-1; i++ ) 
			*reinterpret_cast< char** >( block->buf + i*UNIT_SIZE ) = block->buf + (i+1)*UNIT_SIZE;
		*reinterpret_cast< char** >( block->buf + (block->capacity-1)*UNIT_SIZE ) = NULL;
	}

	TBlock* find_free_block()
	{
		while ( m_pFreeBlock )
		{
			if ( m_pFreeBlock->pFirstFree != NULL )
				return m_pFreeBlock;

			TBlock* pBlock = m_pFreeBlock->pNextFreeBlock;
			m_pFreeBlock->pNextFreeBlock = NULL;
			m_pFreeBlock = pBlock;
		}

		// all blocks are full, so we must make a new empty block and allocate from it.

		if ( m_blockSize > 65535 ) 
			m_blockSize = 65535;

		m_pFreeBlock = alloc_block( m_blockSize );
		clear_block( m_pFreeBlock );
		m_blocks.insert( std::lower_bound( m_blocks.begin(), m_blocks.end(), m_pFreeBlock ), m_pFreeBlock );

		m_blockSize = capacity();	// dynamic block size control

		return m_pFreeBlock;
	}

	int find_ptr_block_index( void *ptr )
	{
		int begin = 0, end = int(m_blocks.size());

		// binary search for matching block

		while ( begin != end )
		{
			int mid = ( begin + end ) / 2;
			if ( mid == begin ) 
				break;

			if ( m_blocks[ mid ]->buf <= ptr ) 
				begin = mid;
			else
				end = mid;
		}

		if ( begin < int(m_blocks.size()) && ptr < m_blocks[ begin ]->buf + m_blocks[ begin ]->capacity * UNIT_SIZE )
			return begin;

		return int(m_blocks.size());
	}

	void _free( void* ptr )
	{
		int index = find_ptr_block_index( ptr );
		if ( index < int(m_blocks.size()) ) 
		{
			TBlock* pBlock = m_blocks[ index ];

			assert( (reinterpret_cast<char*>(ptr) - pBlock->buf) % UNIT_SIZE == 0 );

			if ( m_pFreeBlock != pBlock && pBlock->pFirstFree == NULL )
			{
				pBlock->pNextFreeBlock = m_pFreeBlock->pNextFreeBlock;
				m_pFreeBlock->pNextFreeBlock = pBlock;
			}

			free_from_block( ptr, pBlock );

			if ( m_pFreeBlock != pBlock && pBlock->count == 0 && m_blocks.size() > 1 ) 
			{
				for ( TBlock* p = m_pFreeBlock; p != NULL; p = p->pNextFreeBlock )
				{
					if ( p->pNextFreeBlock == pBlock ) 
					{
						p->pNextFreeBlock = pBlock->pNextFreeBlock;
						break;
					}
				}

				::free( pBlock );
				m_blocks.erase( m_blocks.begin() + index );
			}

			return;
		}

		// error : ptr is not valid!!!
		assert( 0 );
	}

	void* alloc_from_block( TBlock* block ) 
	{
		char* ptr = block->pFirstFree;

		assert( ptr != NULL );

		block->pFirstFree = *reinterpret_cast< char** >( block->pFirstFree );

		// memory marking for debugging

		assert( (memset( ptr, 0xcd, UNIT_SIZE ), block->count < block->capacity) );

		++block->count;
		return ptr;
	}

	void free_from_block( void* ptr, TBlock* block )
	{
		int index = int(reinterpret_cast<char*>(ptr) - block->buf) / UNIT_SIZE;

		*reinterpret_cast< char** >( ptr ) = block->pFirstFree;
		block->pFirstFree = reinterpret_cast< char* >( ptr );
		--block->count;

		// memory marking for debugging

		assert( (memset( reinterpret_cast<char*>(ptr) + sizeof(char*), 0xdd, UNIT_SIZE - sizeof(char*) ), block->count >= 0) );
	}

	template< typename Ev >
	char* bucket_sort( char* pList, char* pBase, Ev ev )
	{
		char* bucketFront[ 256 ];
		char* bucketBack[ 256 ];

		memset( bucketBack, 0, sizeof(bucketBack) );

		char* p;
		for ( p = pList; p != NULL; p = *reinterpret_cast< char** >( p ) )
		{
			int index = ev( (p - pBase) / UNIT_SIZE );
			if ( bucketBack[ index ] )
				*reinterpret_cast< char** >( bucketBack[ index ] ) = p, bucketBack[ index ] = p;
			else
				bucketFront[ index ] = p, bucketBack[ index ] = p;
		}

		p = NULL;
		for ( int i = 255; i >= 0; --i )
		{
			if ( bucketBack[ i ] )
				*reinterpret_cast< char** >( bucketBack[ i ] ) = p, p = bucketFront[ i ];
		}

		return p;
	}

	struct LoRadix { int operator () ( int index ) { return index % 256; } };
	struct HiRadix { int operator () ( int index ) { return index / 256; } };

	void sort_freelist( TBlock* block )
	{
		if ( block->capacity - block->count <= 1 )
			return;

		block->pFirstFree = bucket_sort( block->pFirstFree, block->buf, LoRadix() );

		if ( block->capacity > 255 )
			block->pFirstFree = bucket_sort( block->pFirstFree, block->buf, HiRadix() );
	}

	TBlockList	m_blocks;
	TBlock*		m_pFreeBlock;

	int m_cacheCount;
	void* m_pCache;

	int m_blockSize;
	int m_cacheSize;
};



// ----------------------------------------------------------
//
///    Object allocation/deallocation
//
// ----------------------------------------------------------

template< typename T, int ALIGN = 4 >
class c_objpool
{
public:

	c_objpool( int blockSize = 0, int cacheSize = 256 ) : m_pool( blockSize, cacheSize ) {}

	~c_objpool()
	{
		m_pool.for_each( Destruct() );
	}

	int capacity() const	{ return m_pool.capacity(); }
	int count() const		{ return m_pool.count(); }
	static int unit_size()	{ return CalcAlign< sizeof(T), ALIGN >::val; }
	int block_count() const	{ return m_pool.block_count(); }

	void clear()
	{
		m_pool.for_each( Destruct() );
		m_pool.clear();
	}

	T* alloc()
	{
		T* p = reinterpret_cast< T* >( m_pool.alloc() );
		if ( p )
		{
			try { new (p) T(); }
			catch (...) { free(p); throw; }
		}
		return p;
	}

	void free( T* p )
	{
		p->T::~T();
		m_pool.free( p );
	}

private:

	struct Destruct 
	{ 
		bool operator () ( void* p ) 
		{ 
			reinterpret_cast< T* >(p)->T::~T(); 
			return false; 
		} 
	};

	template< int U, int A > struct CalcAlignLess	{ enum { val = (U > A/2) ? A : CalcAlignLess< U, A/2 >::val }; };
	template< int U > struct CalcAlignLess< U, 1 >	{ enum { val = U }; };
	template< int U, int A > struct CalcAlign1		{ enum { val = (U >= A) ? (U + A-1) / A * A : CalcAlignLess< U, A >::val }; };
	template< int U, int A > struct CalcAlign		{ enum { val = (U < sizeof(void*)) ? CalcAlign1< sizeof(void*), A >::val : CalcAlign1< U,A >::val }; };

	c_unimempool< CalcAlign< sizeof(T), ALIGN >::val > m_pool;
};

#endif