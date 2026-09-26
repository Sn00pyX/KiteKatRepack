#pragma once

#include <vector>

#include "ArObject.h"
#include "ArOption.h"

#include "../toolkit/XMemoryPool.h"
#include "../toolkit/ILock.h"
#include "../toolkit/XConsole.h"
#include "../logging/FileLog.h"
#include "../dump/XExceptionHandler.h"

/*
	Arcadia 는 object 들이 이동함에 따라 이들사이의 enter/leave 를 처리해야 한다.
	해서 Arcadia 는 맵상에 존재하는 object 들에 대한 정보를 참조할 필요가 있는데,
	해서 object 들의 정보를 저장하기 위해 REGION 이라는 단위를 사용한다.

	맵은 REGION_SIZE 만큼의 크기를 가진 REGION 들로 나뉘어진다.
	그리고 해당 REGION 에 위치하는 모든 ArObject 들의 포인터는 ArRegion 에 등록된다.
	ArRegion 은 x, y 를 기준으로 2차원으로 나뉘어고 (z는 무시된다) MAX_LAYER 만큼의 층을 갖는다.

	여러개의 ArRegion 은 ArRegionContainer 가 관리한다.
    여기서 실제 공간의 크기만큼 ArRegion 을 생성하는것은 실제로는 사용되지 않는 지역이
	더 많기 때문에굉장한 메모리 낭비이다. 해서 모든 위치에 대한 ArRegion 을	생성하지 않고
	ArRegionContainer 는 포인터 배열만 가지고 있다가, 해당 ArRegion 의 위치에 최초의
	개체가 등록될때, ArRegion 을 생성한다.

	맵상의 object 들이 이동함에 따라 그들이 속해있는 REGION 도 바뀌게 되는데
	이러한 REGION 변경 시에는 해당 REGION 들에 대해 AddObject, RemoveObject 등으로
	그것을 알려 주어야 한다.
*/

struct ArRegionFunctor
{
	virtual void operator()( const struct ArRegion * pRegion ) = 0;
};

inline void debug_region_list( std::vector< ArObject* > & v )
{
	//return true;
	if( !g_bUseRegionDebug )
		return;

	for( int i = 0; i < (int)v.size(); i++ )
	{
		if( v[i]->region_index != i )
		{
			XSEH::InvokeUnhandledException( 0xC000000DL );
		}
	}
}

struct ArRegion
{
	ArRegion()
	{
		x = 0;
		y = 0;
		layer = 0;
#ifdef _MEM_USAGE_DEBUG
		XSEH::IncreaseAllocCount( "ArRegion" );
#endif
	}

	~ArRegion()
	{
#ifdef _MEM_USAGE_DEBUG
		XSEH::DecreaseAllocCount( "ArRegion" );
#endif
	}

	void AddStaticObject( ArObject* obj )		{	addObject( obj, m_vStaticObject );		}
	void AddMovableObject( ArObject* obj )		{	addObject( obj, m_vMOVABLEObject );	}
	void AddClientObject( ArObject* obj )		{	addObject( obj, m_vClientObject );		}
	void AddObject( ArObject* obj )
	{
		
		switch( obj->GetObjectType() ) {
			case ArObject::STATIC_OBJECT: AddStaticObject( obj ); break;
			case ArObject::MOVABLE_OBJECT: AddMovableObject( obj ); break;
			case ArObject::CLIENT_OBJECT: AddClientObject( obj ); break;
		}
	}

	void RemoveStaticObject( ArObject* obj )	{	removeObject( obj, m_vStaticObject );	}
	void RemoveMovableObject( ArObject* obj )	{	removeObject( obj, m_vMOVABLEObject );	}
	void RemoveClientObject( ArObject* obj )	{	removeObject( obj, m_vClientObject );	}
	void RemoveObject( ArObject* obj )
	{	
		switch( obj->GetObjectType() ) {
			case ArObject::STATIC_OBJECT: RemoveStaticObject( obj ); break;
			case ArObject::MOVABLE_OBJECT: RemoveMovableObject( obj ); break;
			case ArObject::CLIENT_OBJECT: RemoveClientObject( obj ); break;
		}
	}

	size_t ClientCount()						{	return m_vClientObject.size();			}

	unsigned DoEachClient( ArObjectFunctor & _fo ) const
	{
		std::vector< ArObject* >::const_iterator it;
		for( it = m_vClientObject.begin(); it != m_vClientObject.end(); ++it )
		{
			_fo( *it );
		}
		return static_cast< unsigned >( m_vClientObject.size() );
	}

	unsigned DoEachStaticObject( ArObjectFunctor & _fo ) const
	{
		std::vector< ArObject* >::const_iterator it;
		for( it = m_vStaticObject.begin(); it != m_vStaticObject.end(); ++it )
		{
			_fo( *it );
		}
		return static_cast< unsigned >( m_vStaticObject.size() );
	}

	unsigned DoEachMovableObject( ArObjectFunctor & _fo ) const
	{
		std::vector< ArObject* >::const_iterator it;
		for( it = m_vMOVABLEObject.begin(); it != m_vMOVABLEObject.end(); ++it )
		{
			_fo( *it );
		}
		return static_cast< unsigned >( m_vMOVABLEObject.size() );
	}
	
	unsigned x, y;
	unsigned char layer;

private:

	void addObject( ArObject* obj, std::vector< ArObject* > & v  )
	{
		assert( !obj->bIsInWorld );

		debug_region_list( v );

		v.push_back( obj );

		obj->region_index = static_cast<int>( v.size() - 1 );

		if( obj->region_index < 0 ) assert( 0 );

		debug_region_list( v );

		obj->bIsInWorld = true;

		obj->pRegion = this;
	};

	void removeObject( ArObject* obj, std::vector< ArObject* > & v  )
	{
		assert( obj->bIsInWorld );

		debug_region_list( v );

		if( v.empty() )
		{
			FILELOG( "REMOVE_OBJECT ERROR 1" );
			_cprint( "REMOVE_OBJECT ERROR 1" );
			assert( !"REMOVE_OBJECT ERROR 1" );
		}

		if( v[obj->region_index] != obj )
		{
			FILELOG( "REMOVE_OBJECT ERROR 2" );
			_cprint( "REMOVE_OBJECT ERROR 2" );
			assert( !"REMOVE_OBJECT ERROR 2" );
		}

		// { 마지막 원소가 아니라면 마지막 원소를 현재 위치로 가져온다음..
		if( obj != v.back() )
		{
			v[obj->region_index] = v.back();
			v[obj->region_index]->region_index = obj->region_index;
		}
		// }

		// 마지막 원소 제거
		v.pop_back();

		obj->region_index = -1;

		obj->bIsInWorld = false;

		obj->pRegion = NULL;

		debug_region_list( v );
	};

	std::vector< ArObject* >	m_vStaticObject;
	std::vector< ArObject* >	m_vMOVABLEObject;
	std::vector< ArObject* >	m_vClientObject;
};

inline ArRegion* allocRegion()
{	
	return new ArRegion;
	//return m_MemPool.Alloc();
}

inline void freeRegion( ArRegion* p )
{
	delete p;
	//m_MemPool.Free( p );
}

struct ArRegionContainer
{
	enum
	{
		REGION_BLOCK_COUNT = 100,
	};

	ArRegionContainer( AR_UNIT map_width, AR_UNIT map_height )
	{
		m_MapWidth = map_width;
		m_MapHeight = map_height;

		m_nRegionWidth = unsigned int( m_MapWidth / g_nRegionSize + 1 );
		m_nRegionHeight = unsigned int( m_MapHeight / g_nRegionSize + 1 );

		m_nRegionBlockWidth = m_nRegionWidth / REGION_BLOCK_COUNT + 1;
		m_nRegionBlockHeight = m_nRegionHeight / REGION_BLOCK_COUNT + 1;

		initRegion();
	}

	~ArRegionContainer()
	{
		deinitRegion();
	}

	inline bool IsValidPosition( AR_UNIT x, AR_UNIT y )
	{
		if( x < 0 || y < 0 || x >= m_MapWidth || y >= m_MapHeight ) return false;
		return true;
	}

	inline bool IsValidPosition( const ArPosition & pos )
	{
		return IsValidPosition( pos.x, pos.y );
	}

	inline bool IsValidRegion( unsigned rx, unsigned ry, unsigned char layer = 0 )
	{
		if( layer >= MAX_LAYER ) 
		{
			//throw XException( "-_-; 리전범위 (LAYER)" );
			return false;
		}
		if( rx >= m_nRegionWidth || ry >= m_nRegionHeight )
		{
			//char buf[256];
			//s_sprintf( buf, _countof( buf ), "-_-; 리전범위 (%d %d)", rx, ry );
			//throw XException( buf );
			return false;
		}
		return true;
	}

	inline ArRegion* IfClientExistRegion( unsigned rx, unsigned ry, unsigned char layer = 0 )
	{
		ArRegion * pRegion = IfExistRegion( rx, ry, layer );
		
		if( !pRegion ) return NULL;

		if( pRegion->ClientCount() ) return pRegion;

		return NULL;
	}

	inline ArRegion* IfExistRegion( unsigned rx, unsigned ry, unsigned char layer = 0 )
	{
		if( !IsValidRegion( rx, ry, layer ) ) return NULL;
		
		return getRegionPtr( rx, ry, layer );
	}

	// 없으면 할당해서 리턴한다.
	inline ArRegion* GetRegion( unsigned rx, unsigned ry, unsigned char layer = 0 )
	{
		if( !IsValidRegion( rx, ry, layer ) ) return NULL;

		return getRegion( rx, ry, layer );
	}

	// 없으면 할당해서 리턴한다.
	inline ArRegion* GetRegion( AR_UNIT x, AR_UNIT y, unsigned char layer = 0 )
	{
		return GetRegion( GetRegionX( x ), GetRegionY( y ), layer );
	}

	// 없으면 할당해서 리턴한다.
	inline ArRegion* GetRegion( const ArPosition & pos, unsigned char layer )
	{
		return GetRegion( pos.x, pos.y, layer );
	}

	// 없으면 할당해서 리턴한다.
	inline ArRegion* GetRegion( ArObject * pObject )
	{
		return GetRegion( pObject->GetRX(), pObject->GetRY(), pObject->GetLayer() );
	}

	inline void DoEachSpecificRegion( unsigned rx, unsigned ry, unsigned range, unsigned char layer, ArRegionFunctor & Functor )
	{
		int left, right, top, bottom;

		left	= (std::max)( int(rx - range), 0 );
		right	= (std::min)( int(rx + range), (int)m_nRegionWidth-1 );
		top		= (std::max)( int(ry - range), 0 );
		bottom	= (std::min)( int(ry + range), (int)m_nRegionHeight-1 );

		for( int x = left; x <= right; x++)
		{
			for( int y = top; y <= bottom; y++)
			{	
				if( IsVisibleRegion( rx, ry, x, y ) )
				{
					ArRegion * pRegion = getRegionPtr( x, y, layer );
					if( pRegion ) Functor( pRegion );
				}
			}
		}
	}

	inline void DoEachVisibleRegion( unsigned rx, unsigned ry, unsigned char layer, ArRegionFunctor & Functor )
	{
		int left, right, top, bottom;

		left	= (std::max)( (int)rx - VISIBLE_REGION_RANGE, 0 );
		right	= (std::min)( (int)rx + VISIBLE_REGION_RANGE, (int)m_nRegionWidth-1 );
		top		= (std::max)( (int)ry - VISIBLE_REGION_RANGE, 0 );
		bottom	= (std::min)( (int)ry + VISIBLE_REGION_RANGE, (int)m_nRegionHeight-1 );

		for( int x = left; x <= right; x++)
		{
			for( int y = top; y <= bottom; y++)
			{	
				if( IsVisibleRegion( rx, ry, x, y ) )
				{
					ArRegion * pRegion = getRegionPtr( x, y, layer );
					if( pRegion ) Functor( pRegion );
				}
			}
		}
	}

	inline void DoEachNewRegion( unsigned rx, unsigned ry, unsigned prx, unsigned pry, unsigned char layer, ArRegionFunctor & Functor )
	{
		int left = (std::max)( 0, (std::min)( (int)rx - VISIBLE_REGION_RANGE, (int)prx - VISIBLE_REGION_RANGE ) );
		int top = (std::max)( 0, (std::min)( (int)ry - VISIBLE_REGION_RANGE, (int)pry - VISIBLE_REGION_RANGE ) );

		int right = (std::min)( (std::max)( (int)rx + VISIBLE_REGION_RANGE, (int)prx + VISIBLE_REGION_RANGE ), (int)m_nRegionWidth-1 );
		int bottom = (std::min)( (std::max)( (int)ry + VISIBLE_REGION_RANGE, (int)pry + VISIBLE_REGION_RANGE ),(int)m_nRegionHeight-1 );

		int nIsCurrentPos, nIsPrevPos;
		
		for( int y = top; y <= bottom; y++ )
		{
			for( int x = left; x <= right; x++ )
			{
				nIsCurrentPos = IsVisibleRegion( rx, ry, x, y );
				nIsPrevPos = IsVisibleRegion( prx, pry, x, y );

				if( nIsCurrentPos && !nIsPrevPos )
				{
					ArRegion * pRegion = getRegionPtr( x, y, layer );
					Functor( pRegion );
				}
			}
		}
	}

	inline void DoEachVisibleRegion( unsigned rx1, unsigned ry1, unsigned rx2, unsigned ry2, unsigned char layer, ArRegionFunctor & Functor )
	{
		int left = (std::max)( 0, (std::min)( (int)rx1 - VISIBLE_REGION_RANGE, (int)rx2 - VISIBLE_REGION_RANGE ) );
		int top = (std::max)( 0, (std::min)( (int)ry1 - VISIBLE_REGION_RANGE, (int)ry2 - VISIBLE_REGION_RANGE ) );

		int right = (std::min)( (std::max)( (int)rx1 + VISIBLE_REGION_RANGE, (int)rx2 + VISIBLE_REGION_RANGE ), (int)m_nRegionWidth-1 );
		int bottom = (std::min)( (std::max)( (int)ry1 + VISIBLE_REGION_RANGE, (int)ry2 + VISIBLE_REGION_RANGE ),(int)m_nRegionHeight-1 );

		int nPos1, nPos2;
		
		ArRegion * pRegion;
		for( int y = top; y <= bottom; y++ )
		{
			for( int x = left; x <= right; x++ )
			{
				nPos1 = IsVisibleRegion( rx1, ry1, x, y );
				nPos2 = IsVisibleRegion( rx2, ry2, x, y );

				if( nPos1 || (!nPos1 && nPos2 ) )
				{
					pRegion = getRegionPtr( x, y, layer );
					if( pRegion ) Functor( pRegion );
				}
			}
		}
	}

	inline void DoEachRegion( unsigned rx1, unsigned ry1, unsigned rx2, unsigned ry2, unsigned char layer, ArRegionFunctor & Functor )
	{
		int left = (std::max)( 0, (std::min)( (int)rx1 - VISIBLE_REGION_RANGE, (int)rx2 - VISIBLE_REGION_RANGE ) );
		int top = (std::max)( 0, (std::min)( (int)ry1 - VISIBLE_REGION_RANGE, (int)ry2 - VISIBLE_REGION_RANGE ) );

		int right = (std::min)( (std::max)( (int)rx1 + VISIBLE_REGION_RANGE, (int)rx2 + VISIBLE_REGION_RANGE ), (int)m_nRegionWidth-1 );
		int bottom = (std::min)( (std::max)( (int)ry1 + VISIBLE_REGION_RANGE, (int)ry2 + VISIBLE_REGION_RANGE ),(int)m_nRegionHeight-1 );

		ArRegion * pRegion;
		for( int y = top; y <= bottom; y++ )
		{
			for( int x = left; x <= right; x++ )
			{
				pRegion = getRegionPtr( x, y, layer );
				if( pRegion ) Functor( pRegion );
			}
		}
	}

private:
	struct ArRegionBlock
	{
		ArRegionBlock()
		{
			THREAD_SYNCRONIZE( m_Lock );

			for( unsigned i = 0; i < MAX_LAYER; i++ )
			{
				//m_pRegion[i] = new ArRegion*[ REGION_BLOCK_COUNT * REGION_BLOCK_COUNT ];
				//ZeroMemory( m_pRegion[i], sizeof( ArRegion* ) * REGION_BLOCK_COUNT * REGION_BLOCK_COUNT );
				m_pRegion[i] = NULL;
			}
		}

		~ArRegionBlock()
		{
			THREAD_SYNCRONIZE( m_Lock );

			for( unsigned i = 0; i < MAX_LAYER; i++ )
			{
				if( m_pRegion[i] )
				{
					for( unsigned x = 0; x < REGION_BLOCK_COUNT * REGION_BLOCK_COUNT; x++ )
					{
						if( m_pRegion[i][x] ) freeRegion( m_pRegion[i][x] );
					}
					delete [] m_pRegion[i];
				}
			}
		}

		ArRegion* getRegionPtr( unsigned rx, unsigned ry, unsigned char layer )
		{
			THREAD_SYNCRONIZE( m_Lock );

			if( !m_pRegion[layer] )
				return NULL;

			return m_pRegion[layer][ ry * REGION_BLOCK_COUNT + rx ];
		}

		ArRegion* getRegion( unsigned rx, unsigned ry, unsigned char layer )
		{
			THREAD_SYNCRONIZE( m_Lock );

			if( m_pRegion[layer] && m_pRegion[layer][ ry * REGION_BLOCK_COUNT + rx ] )
				return m_pRegion[layer][ ry * REGION_BLOCK_COUNT + rx ];

			if( !m_pRegion[layer] )
			{
				m_pRegion[layer] = new ArRegion*[ REGION_BLOCK_COUNT * REGION_BLOCK_COUNT ];
				ZeroMemory( m_pRegion[layer], sizeof( ArRegion* ) * REGION_BLOCK_COUNT * REGION_BLOCK_COUNT );
			}

			m_pRegion[layer][ ry * REGION_BLOCK_COUNT + rx ] = allocRegion();
			m_pRegion[layer][ ry * REGION_BLOCK_COUNT + rx ]->x = rx;
			m_pRegion[layer][ ry * REGION_BLOCK_COUNT + rx ]->y = ry;

			m_pRegion[layer][ ry * REGION_BLOCK_COUNT + rx ]->layer = layer;

			return m_pRegion[layer][ ry * REGION_BLOCK_COUNT + rx ];
		}

		ArRegion**	m_pRegion[ MAX_LAYER ];
		XSpinLock	m_Lock;
	};

	void initRegion()
	{
		THREAD_SYNCRONIZE( m_Lock );

		m_pRegionBlock = new ArRegionBlock*[ m_nRegionBlockWidth * m_nRegionBlockHeight ];

		ZeroMemory( m_pRegionBlock, sizeof( ArRegionBlock* ) * m_nRegionBlockWidth * m_nRegionBlockHeight );
	}

	void deinitRegion()
	{
		THREAD_SYNCRONIZE( m_Lock );

		for( unsigned x = 0; x < m_nRegionBlockWidth * m_nRegionBlockHeight; x++ )
		{
			if( m_pRegionBlock[x] ) delete m_pRegionBlock[x];
		}

		delete [] m_pRegionBlock;
	}

	ArRegionBlock* getRegionBlockPtr( unsigned rcx, unsigned rcy )
	{
		THREAD_SYNCRONIZE( m_Lock );

		ArRegionBlock *pBlock = m_pRegionBlock[ rcy * m_nRegionBlockWidth + rcx ];

		return pBlock;
	}

	ArRegionBlock* getRegionBlock( unsigned rcx, unsigned rcy )
	{
		THREAD_SYNCRONIZE( m_Lock );

		ArRegionBlock *pBlock = m_pRegionBlock[ rcy * m_nRegionBlockWidth + rcx ];

		if( pBlock )
			return pBlock;

		m_pRegionBlock[ rcy * m_nRegionBlockWidth + rcx ] = new ArRegionBlock;

		return m_pRegionBlock[ rcy * m_nRegionBlockWidth + rcx ];
	}

	bool isExistRegion( unsigned rx, unsigned ry, unsigned char layer )
	{
		unsigned rcx = rx / REGION_BLOCK_COUNT;
		unsigned rcy = ry / REGION_BLOCK_COUNT;

		ArRegionBlock* pBlock = getRegionBlockPtr( rcx, rcy );

		if( !pBlock ) return false;

		ArRegion *pRegion = pBlock->getRegionPtr( rx % REGION_BLOCK_COUNT, ry % REGION_BLOCK_COUNT, layer );

		if( !pRegion ) return false;

		return true;
	}

	ArRegion* getRegionPtr( unsigned rx, unsigned ry, unsigned char layer )
	{
		unsigned rcx = rx / REGION_BLOCK_COUNT;
		unsigned rcy = ry / REGION_BLOCK_COUNT;

		ArRegionBlock* pBlock = getRegionBlockPtr( rcx, rcy );

		if( !pBlock ) return NULL;

		return pBlock->getRegionPtr( rx % REGION_BLOCK_COUNT, ry % REGION_BLOCK_COUNT, layer );
	}

	ArRegion* getRegion( unsigned rx, unsigned ry, unsigned char layer )
	{
		unsigned rcx = rx / REGION_BLOCK_COUNT;
		unsigned rcy = ry / REGION_BLOCK_COUNT;

		ArRegionBlock* pBlock = getRegionBlock( rcx, rcy );

		ArRegion * pRegion = pBlock->getRegion( rx % REGION_BLOCK_COUNT, ry % REGION_BLOCK_COUNT, layer );

		return pRegion;
	}

	AR_UNIT		m_MapWidth, m_MapHeight;
	unsigned	m_nRegionWidth;
	unsigned	m_nRegionHeight;

	unsigned	m_nRegionBlockWidth;
	unsigned	m_nRegionBlockHeight;

	ArRegionBlock**	m_pRegionBlock;

	XSpinLock	m_Lock;

	//ArRegion**	m_pRegion[MAX_LAYER];

	//XUnitPool<ArRegion>	m_MemPool;
};
