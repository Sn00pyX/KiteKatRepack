// X2DQuadTree.h
//
//  Last modified by Young-Hyun Joo, 2007/12/11
//
//  Created by Testors , 2005/08/01

#pragma once

//
//  X2D 에 기반이지만 어짜피 완전히 generic 하게 만들었기 때문에
//  X2D::INCLUDE( X2D::Rect< T >, U ), X2D::COLLISION( X2D::Rect< T >, U ), X2D::COLLISION( U, C ), X2D::COLLISION( X2D::Rect< T >, C ) 
//  만 주어진다면 3D 에서의 사용도 가능하다.
//
//	T : 좌표계 타입
//	U : 보관할 대상타입
//	THRESHOLD : 한 노드에 몇개정도까지 U 를 저장할것인가의 여부
//	C : 검색시 범위 타입
//

#include "X2DBasicTypes.h"
#include <vector>
#include <assert.h>

namespace X2D
{

template< typename T, typename U, bool LOOSE = true, size_t THRESHOLD = 20, size_t MAX_DEPTH = 10 >
struct QuadTree
{
	typedef Box< T > NodeType;

	QuadTree( const T & left, const T & top, const T & width, const T & height )
		: m_masterNode( 1, left, top, width, height )
	{
	}

	QuadTree( const T & width, const T & height )
		: m_masterNode( 1, 0, 0, width, height )
	{
	}

	~QuadTree()
	{
	}

	void ReInit( const T & left, const T & top, const T & width, const T & height )
	{
		m_masterNode.Node::~Node();
		new ( &m_masterNode ) Node( 1, left, top, width, height );
	}

	void ReInit( const T & width, const T & height )
	{
		m_masterNode.Node::~Node();
		new ( &m_masterNode ) Node( 1, 0, 0, width, height );
	}

	T GetX()
	{
		return m_masterNode.GetX();
	}

	T GetY()
	{
		return m_masterNode.GetY();
	}

	T GetWidth() const
	{
		return m_masterNode.GetWidth();
	}

	T GetHeight() const
	{
		return m_masterNode.GetHeight();
	}

	bool Add( const U & u )
	{
		return m_masterNode.Add( u );
	}

	template< typename C, typename F >
	void Enum( const C & c, F & f )
	{
		m_masterNode.Enum( c, f );
	}

	template< typename C >
	void Enum( const C & c, std::vector< U > * pResult )
	{
		_FUNCTOR_ADAPTOR adaptor;
		adaptor.pResult = pResult;
		m_masterNode.Enum( c, adaptor );
	}

	template< typename C >
	void Enum( const C & c, std::vector< const U* > * pResult )
	{
		_FUNCTOR_ADAPTOR_PTR adaptor;
		adaptor.pResult = pResult;
		m_masterNode.Enum( c, adaptor );
	}


	void Enum( std::vector< U > * pResult )
	{
		_FUNCTOR_ADAPTOR adaptor;
		adaptor.pResult = pResult;
		m_masterNode.Enum( adaptor );
	}

	template< typename F >
	void Enum( F & f )
	{
		m_masterNode.Enum( f );
	}	

	template< typename C >
	void Remove( const C & c )
	{
		m_masterNode.Remove( c );
	}

	template< typename C >
	bool Collision( const C & c ) const
	{
		return m_masterNode.Collision( c );
	}

	template< typename C >
	bool LooseCollision( const C & c ) const
	{
		return m_masterNode.LooseCollision( c );
	}

	bool Has( const U & u ) const
	{
		m_masterNode.Has( u );
	}

	void Remove( const U & u )
	{
		m_masterNode.Remove( u );
	}

//#ifdef _DEBUG
	template< typename NF >
	void EnumNode( NF & f, bool bReverse = false )
	{
		m_masterNode.EnumNode( f, bReverse );
	}

	template< typename IF >
	void EnumItem( IF & f, bool bReverse = false )
	{
		m_masterNode.EnumItem( f, bReverse );
	}

	size_t getItemCount() const
	{
		return m_masterNode.getItemCount();
	}

//#endif

private:

	struct Node
	{
		Node( unsigned short depth, const T & left, const T & top, const T & width, const T & height )
		{
			m_unDepth = depth;

			m_pNode[0] = NULL;
			m_pNode[1] = NULL;
			m_pNode[2] = NULL;
			m_pNode[3] = NULL;

			init( depth, left, top, width, height );
		}

		~Node()
		{
			if( hasChildNode() )
			{
				delete m_pNode[0];
				delete m_pNode[1];
				delete m_pNode[2];
				delete m_pNode[3];
			}
		}

		T GetX()
		{
			return m_rcEffectiveArea.GetX();
		}

		T GetY()
		{
			return m_rcEffectiveArea.GetY();
		}

		T GetWidth() const
		{
			return m_rcEffectiveArea.GetWidth();
		}

		T GetHeight() const
		{
			return m_rcEffectiveArea.GetHeight();
		}

		bool hasChildNode() const { return m_pNode[0] != NULL; }

		bool Add( const U & u )
		{
			if( !COLLISION( m_rcEffectiveArea, u ) ) 
			{
				assert( 0 );
				return false;
			}

			// 자식 노드가 있다면
			if( hasChildNode() )
			{
				Node *pNode = getFitNode( u );
				if( pNode ) 
				{
					if( pNode->Add( u ) ) return true;
				}
			}

			add( u );

			return true;
		}

		bool IsAddable( const U & u ) const
		{
			return INCLUDE( m_rcEffectiveArea, u );
		}

		template< typename C >
		bool Collision( const C & c ) const
		{
			if( !COLLISION( m_rcEffectiveArea, c ) ) return false;

			for( std::vector< U >::const_iterator it = m_vList.begin(); it != m_vList.end(); ++it )
			{
				if( COLLISION( (*it), c ) ) return true;
			}

			if( hasChildNode() )
			{	
				// quadtree(0,0,100,100) 일경우 Box(70,70,80,80) 를 add() 하면 m_pNode[3] 에 들어간다.
				// 그런데 이걸 point(70,70) 으로 검사하면 m_pNode[0] 이 fit 이 된다.
				// 그러므로 4 노드 모두 검사 해야 한다.
				if( m_pNode[0]->Collision( c ) ) return true;
				if( m_pNode[1]->Collision( c ) ) return true;
				if( m_pNode[2]->Collision( c ) ) return true;
				if( m_pNode[3]->Collision( c ) ) return true;
			}

			return false;
		}

		template< typename C >
		bool LooseCollision( const C & c ) const
		{
			if( !COLLISION( m_rcEffectiveArea, c ) ) return false;

			for( std::vector< U >::const_iterator it = m_vList.begin(); it != m_vList.end(); ++it )
			{
				if( LOOSE_COLLISION( (*it), c ) ) return true;
			}

			if( hasChildNode() )
			{	
				// quadtree(0,0,100,100) 일경우 Box(70,70,80,80) 를 add() 하면 m_pNode[3] 에 들어간다.
				// 그런데 이걸 point(70,70) 으로 검사하면 m_pNode[0] 이 fit 이 된다.
				// 그러므로 4 노드 모두 검사 해야 한다.
				if( m_pNode[0]->LooseCollision( c ) ) return true;
				if( m_pNode[1]->LooseCollision( c ) ) return true;
				if( m_pNode[2]->LooseCollision( c ) ) return true;
				if( m_pNode[3]->LooseCollision( c ) ) return true;
			}

			return false;
		}

		template< typename C, typename F >
		void Enum( const C & c, F & f )
		{
			if( !COLLISION( m_rcEffectiveArea, c ) ) return;

			if( hasChildNode() )
			{	
				// quadtree(0,0,100,100) 일경우 Box(70,70,80,80) 를 add() 하면 m_pNode[3] 에 들어간다.
				// 그런데 이걸 point(70,70) 으로 검사하면 m_pNode[0] 이 fit 이 된다.
				// 그러므로 4 노드 모두 검사 해야 한다.
				m_pNode[0]->Enum( c, f );
				m_pNode[1]->Enum( c, f );
				m_pNode[2]->Enum( c, f );
				m_pNode[3]->Enum( c, f );
			}

			for( std::vector< U >::iterator it = m_vList.begin(); it != m_vList.end(); ++it )
			{
				if( COLLISION( (*it), c ) ) f( *it );
			}
		}

		template< typename F >
		void Enum( F & f )
		{
			if( hasChildNode() )
			{	
				m_pNode[0]->Enum( f );
				m_pNode[1]->Enum( f );
				m_pNode[2]->Enum( f );
				m_pNode[3]->Enum( f );
			}

			for( std::vector< U >::iterator it = m_vList.begin(); it != m_vList.end(); ++it ) f( *it );
		}

		bool Has( const U & u ) const
		{
			if( !IsAddable( u ) ) return false;

			if( hasChildNode() )
			{	
				Node *pNode = getFitNode( u );
				if( pNode->Has( u ) ) return true;
			}

			for( std::vector< U >::iterator it = m_vList.begin(); it != m_vList.end(); )
			{
				if( (*it) == u ) 
				{
					if( it = m_vList.erase( it ) ) return true;
				}
				++it;
			}

			return false;
		}

		void Remove( const U & u )
		{
			if( !IsAddable( u ) ) return;

			if( hasChildNode() )
			{	
				Node *pNode = getFitNode( u );
				if( pNode ) pNode->Remove( u );
			}

			for( std::vector< U >::iterator it = m_vList.begin(); it != m_vList.end(); )
			{
				if( (*it) == u ) 
				{
					it = m_vList.erase( it );
					continue;
				}
				++it;
			}
		}

		template< typename C >
		void Remove( const C & c )
		{
			if( hasChildNode() )
			{
				// quadtree(0,0,100,100) 일경우 Box(70,70,80,80) 를 add() 하면 m_pNode[3] 에 들어간다.
				// 그런데 이걸 point(70,70) 으로 검사하면 m_pNode[0] 이 fit 이 된다.
				// 그러므로 4 노드 모두 검사 해야 한다.
				m_pNode[0]->Remove( c );
				m_pNode[1]->Remove( c );
				m_pNode[2]->Remove( c );
				m_pNode[3]->Remove( c );
			}

			for( std::vector< U >::iterator it = m_vList.begin(); it != m_vList.end(); )
			{
				if( COLLISION( (*it), c ) ) 
				{
					it = m_vList.erase( it );
					continue;
				}
				++it;
			}
		}

		const Rect< T > & GetEffectiveArea() const	{ return m_rcEffectiveArea; }

		void SetDepth( unsigned short depth )
		{
			m_unDepth = depth;
			if( hasChildNode() )
			{
				m_pNode[0]->SetDepth( depth );
				m_pNode[1]->SetDepth( depth );
				m_pNode[2]->SetDepth( depth );
				m_pNode[3]->SetDepth( depth );
			}
		}

//#ifdef _DEBUG
		template< typename NF >
		void EnumNode( NF & f, bool bReverse )
		{
			if( !bReverse ) f( m_unDepth, m_rcEffectiveArea, m_vList );

			if( hasChildNode() )
			{
				m_pNode[0]->EnumNode( f, bReverse );
				m_pNode[1]->EnumNode( f, bReverse );
				m_pNode[2]->EnumNode( f, bReverse );
				m_pNode[3]->EnumNode( f, bReverse );
			}

			if( bReverse ) f( m_unDepth, m_rcEffectiveArea, m_vList );
		}

		template< typename IF >
		void enum_item( IF & f )
		{
			for( std::vector< U >::iterator it = m_vList.begin(); it != m_vList.end(); ++it ) f( m_unDepth, *it );
		}

		template< typename IF >
		void EnumItem( IF & f, bool bReverse )
		{
			if( !bReverse ) enum_item( f );

			if( hasChildNode() )
			{
				m_pNode[0]->EnumItem( f, bReverse );
				m_pNode[1]->EnumItem( f, bReverse );
				m_pNode[2]->EnumItem( f, bReverse );
				m_pNode[3]->EnumItem( f, bReverse );
			}

			if( bReverse ) enum_item( f );
		}

		size_t getItemCount() const
		{
			size_t total = 0;

			if( hasChildNode() )
			{
				total += m_pNode[0]->getItemCount();
				total += m_pNode[1]->getItemCount();
				total += m_pNode[2]->getItemCount();
				total += m_pNode[3]->getItemCount();
			}

			return total + m_vList.size();
		}
//#endif

	private:

		Node()
		{
			m_pNode[0] = NULL;
			m_pNode[1] = NULL;
			m_pNode[2] = NULL;
			m_pNode[3] = NULL;
		}

		Node* getFitNode( const U & u )
		{
			if( LOOSE )
			{
					 if( m_pNode[0]->IsAddable( u ) ) return m_pNode[0];
				else if( m_pNode[1]->IsAddable( u ) ) return m_pNode[1];
				else if( m_pNode[2]->IsAddable( u ) ) return m_pNode[2];
				else if( m_pNode[3]->IsAddable( u ) ) return m_pNode[3];
				else return NULL;
			}

			bool bA0 = m_pNode[0]->IsAddable( u );
			bool bA1 = m_pNode[1]->IsAddable( u );
			bool bA2 = m_pNode[2]->IsAddable( u );
			bool bA3 = m_pNode[3]->IsAddable( u );

			if( bA1 && bA2 && !bA0 && !bA3 ) return m_pNode[1];
			if( bA2 && bA3 && !bA0 && !bA1  ) return m_pNode[2];
			if( bA3 && bA0 && !bA1 && !bA2  ) return m_pNode[3];

			if( bA0 ) return m_pNode[0];
			if( bA1 ) return m_pNode[1];
			if( bA2 ) return m_pNode[2];
			if( bA3 ) return m_pNode[3];

			return NULL;
		}

		void add( const U & u )
		{
			m_vList.push_back( u );

			if( m_vList.size() >= THRESHOLD && m_unDepth < MAX_DEPTH ) divide();
		}

		void operator()( const U & u )
		{
			m_vList.push_back( u );
		}

		void join()
		{
			if( !hasChildNode() ) return;

			m_pNode[0]->Enum( *this );
			m_pNode[1]->Enum( *this );
			m_pNode[2]->Enum( *this );
			m_pNode[3]->Enum( *this );

			delete m_pNode[0];
			delete m_pNode[1];
			delete m_pNode[2];
			delete m_pNode[3];

			m_pNode[0] = NULL;
			m_pNode[1] = NULL;
			m_pNode[2] = NULL;
			m_pNode[3] = NULL;
		}

		void divide()
		{
			if( hasChildNode() ) return;

			m_pNode[0] = new Node;
			m_pNode[1] = new Node;
			m_pNode[2] = new Node;
			m_pNode[3] = new Node;

			if( LOOSE )
			{
				T quad_width		= m_rcEffectiveArea.GetWidth() / 4;
				T quad_height		= m_rcEffectiveArea.GetHeight() / 4;

				T width				= m_rcEffectiveArea.GetWidth() - quad_width;
				T height			= m_rcEffectiveArea.GetHeight() - quad_height;

				m_pNode[0]->init( m_unDepth + 1, m_rcEffectiveArea.GetLeft(), m_rcEffectiveArea.GetTop(), width, height );
				m_pNode[1]->init( m_unDepth + 1, m_rcEffectiveArea.GetLeft() + quad_width, m_rcEffectiveArea.GetTop(), width, height );
				m_pNode[2]->init( m_unDepth + 1, m_rcEffectiveArea.GetLeft(), m_rcEffectiveArea.GetTop() + quad_height, width, height );
				m_pNode[3]->init( m_unDepth + 1, m_rcEffectiveArea.GetLeft() + quad_width, m_rcEffectiveArea.GetTop() + quad_height, width, height );
			}
			else
			{
				T half_width		= m_rcEffectiveArea.GetWidth() / 2;
				T half_height		= m_rcEffectiveArea.GetHeight() / 2;

				m_pNode[0]->init( m_unDepth + 1, m_rcEffectiveArea.GetLeft(), m_rcEffectiveArea.GetTop(), half_width, half_height );
				m_pNode[1]->init( m_unDepth + 1, m_rcEffectiveArea.GetLeft() + half_width, m_rcEffectiveArea.GetTop(), half_width, half_height );
				m_pNode[2]->init( m_unDepth + 1, m_rcEffectiveArea.GetLeft(), m_rcEffectiveArea.GetTop() + half_height, m_rcEffectiveArea.GetWidth() - half_width, m_rcEffectiveArea.GetHeight() - half_height );
				m_pNode[3]->init( m_unDepth + 1, m_rcEffectiveArea.GetLeft() + half_width, m_rcEffectiveArea.GetTop() + half_height, m_rcEffectiveArea.GetWidth() - half_width, m_rcEffectiveArea.GetHeight() - half_height );
			}

			std::vector< U >	vTmpList;
			vTmpList.reserve( m_vList.size() );

			for( std::vector< U >::iterator it = m_vList.begin(); it != m_vList.end(); ++it )
			{
				Node *pNode = getFitNode( *it );
				if( pNode ) 
				{
					pNode->Add( *it );
					continue;
				}

				vTmpList.push_back( *it );
			}

			m_vList.erase( m_vList.begin(), m_vList.end() );
			m_vList.assign( vTmpList.begin(), vTmpList.end() );
		}

		void init( unsigned short depth, const T & left, const T & top, const T & width, const T & height )
		{
			m_rcEffectiveArea.Set(	left, top, width, height );

			m_unDepth = depth;
		}

		std::vector< U >	m_vList;
		Rect< T >			m_rcEffectiveArea;
		struct Node*		m_pNode[4];
		unsigned short		m_unDepth;
	};

	struct _FUNCTOR_ADAPTOR
	{
		std::vector< U > * pResult;
		void operator()( const U & item )
		{
			pResult->push_back( item );
		}
	};

	struct _FUNCTOR_ADAPTOR_PTR
	{
		std::vector< const U* > * pResult;
		void operator()( const U & item )
		{
			pResult->push_back( &item );
		}
	};


	struct Node m_masterNode;
};

}; // namespace X2D
