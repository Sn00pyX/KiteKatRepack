// X2DRectAllocator.h
//
//  by Testors , 2005/08/010

#pragma once

#include "X2DQuadTree.h"

namespace X2D
{

	template< typename T >
	struct RectAllocator
	{
		typedef Rect<T>		Rect;
		typedef Point<T>	Point;
		typedef QuadTree< T, Rect, true >	QUAD_TREE;

		RectAllocator( T width, T height ) : m_AllocatedRectTree( width, height )
		{
			FreeRectChunk temp;
			temp.AddFreeRect( Rect( 0, 0, width, height ) );
			m_vChunkList.push_back( temp );
		}

		~RectAllocator()
		{
		}

		template< typename F >
		void EnumAllocatedBlock( F & f )
		{
			m_AllocatedRectTree.Enum( f );
		}

		template< typename F >
		void EnumFreeBlock( F & f )
		{
			std::vector< FreeRectChunk >::iterator it;
			for( it = m_vChunkList.begin(); it != m_vChunkList.end(); ++it ) (*it).EnumFreeBlock( f );
		}

		bool Alloc( const T & width, const T & height, Point *pPoint )
		{
			std::vector< Point > vPtList;
			std::vector< FreeRectChunk >::iterator it;

			for( it = m_vChunkList.begin(); it != m_vChunkList.end(); ++it )
			{
				if( (*it).Alloc( width, ( height < 2 ? 2 : height ), m_AllocatedRectTree, pPoint ) ) return true;
			}
			return false;
		}

		void Remove( const Point & pt )
		{
			std::vector< FreeRectChunk >::iterator it;

			_Rect block;
			block.bFound = false;
			m_AllocatedRectTree.Enum( pt, block );

			if( !block.bFound ) return;

			m_AllocatedRectTree.Remove( block.m_rect );

			for( it = m_vChunkList.begin(); it != m_vChunkList.end(); ++it )
			{
				if( (*it).IsTouchedFreeBlock( block.m_rect ) )
				{
					(*it).AddFreeRect( block.m_rect );
					break;
				}
			}
		}

		void Remove( const Rect & rc )
		{
			std::vector< Rect >::iterator rit;
			std::vector< FreeRectChunk >::iterator it;

			_RectList block;
			block.bFound = false;
			m_AllocatedRectTree.Enum( rc, block );

			if( !block.bFound ) return;

			for( rit = block.m_lstRect.begin(); rit != block.m_lstRect.end(); ++rit )
			{
				Remove( (*rit).GetLeftTop() );
			}
		}

	private:

		struct FreeRectChunk
		{
			FreeRectChunk()
			{
				m_bInitialized = false;
			}

			bool IsTouchedFreeBlock( const Rect & rc )
			{
				return true;
			}

			bool Alloc( const T & width, const T & height, QUAD_TREE & allocatedRectTree, Point *pPoint )
			{
				if( m_rcEffectiveArea.GetWidth() < width ||
					m_rcEffectiveArea.GetHeight() < height ) return false;

				bool bFound = false;

				// 딱 맞는놈이 있는지?
				for( std::vector< Rect >::iterator it = m_vFreeRectList.begin(); it != m_vFreeRectList.end(); ++it ) 
				{
					if( (*it).GetHeight() == height && (*it).GetWidth() == width )
					{
						T x = (*it).GetLeft();
						T y = (*it).GetTop();

						Rect rc( x, y, width, height );
						if( !INCLUDE( m_rcEffectiveArea, rc ) ) assert( 0 );

						pPoint->x = x;
						pPoint->y = y;

						bFound = true;
						break;
					}
				}
				
				// 그나마 비슷한 놈이 있는지?
				if( !bFound )
				{
					for( std::vector< Rect >::iterator it = m_vFreeRectList.begin(); it != m_vFreeRectList.end(); ++it ) 
					{
						if( (*it).GetHeight() == height || (*it).GetWidth() == width )
						{
							T x = (*it).GetLeft();
							T y = (*it).GetTop();
							Rect rc( x, y, width, height );

							if( !INCLUDE( m_rcEffectiveArea, rc ) ) continue;

							if( allocatedRectTree.Collision( rc ) ) continue;

							pPoint->x = x;
							pPoint->y = y;

							bFound = true;
							break;
						}
					}
				}

				if( !bFound )
				{
					// 찾아보세~
					for( std::vector< T >::iterator y = m_vYPtList.begin(); y != m_vYPtList.end(); ++y )				
					{
						for( std::vector< T >::iterator x = m_vXPtList.begin(); x != m_vXPtList.end(); ++x )
						{	
							Rect rc( *x, *y, width, height );

							if( !INCLUDE( m_rcEffectiveArea, rc ) ) continue;

							if( allocatedRectTree.Collision( rc ) ) continue;

							pPoint->x = *x;
							pPoint->y = *y;

							bFound = true;
							break;
						}
						if( bFound ) break;
					}
				}

				if( bFound )
				{
					Rect rc( pPoint->x, pPoint->y, width, height );
					RemoveFreeRect( rc );
					allocatedRectTree.Add( rc );
					m_FreeSize -= rc.GetSize();
				}

				return bFound;
			}

			template< typename F >
			void EnumFreeBlock( F & f)
			{
				for( std::vector< Rect >::iterator it = m_vFreeRectList.begin(); it != m_vFreeRectList.end(); ++it ) f( *it );
			}

			void RemoveFreeRect( const Rect & rc )
			{
				std::vector< Rect >	vNewFreeRect;

				for( std::vector< Rect >::iterator it = m_vFreeRectList.begin(); it != m_vFreeRectList.end();  )
				{
					if( !COLLISION( *it, rc ) ) 
					{
						++it;
						continue;
					}

					DivideFreeRect( *it, rc, &vNewFreeRect );

					it = m_vFreeRectList.erase( it );
				}

				for( std::vector< Rect >::iterator it = vNewFreeRect.begin(); it != vNewFreeRect.end(); ++it ) AddFreeRect( *it );
				//m_vFreeRectList.insert( m_vFreeRectList.end(), vNewFreeRect.begin(), vNewFreeRect.end() );

				CalcPtList();
			}

			bool DivideFreeRect( const Rect & rcFree, const Rect & rcArea, std::vector< Rect > * pNewFreeRect )
			{
				// case #0. FreeRect 가 Area 안에 완전히 포함되어 나뉜 결과가 없는경우
				if( INCLUDE( rcArea, rcFree ) ) return false;

				std::vector< Point >	vIncludePtList;

				if( INCLUDE( rcFree, rcArea.GetLeftTop() ) &&
							 rcArea.GetLeft() != rcFree.GetLeft() &&
							 rcArea.GetTop() != rcFree.GetTop() ) vIncludePtList.push_back( rcArea.GetLeftTop() );

				if( INCLUDE( rcFree, rcArea.GetRightTop() ) &&
							 rcArea.GetRight() != rcFree.GetRight() &&
							 rcArea.GetTop() != rcFree.GetTop() ) vIncludePtList.push_back( rcArea.GetRightTop() );

				if( INCLUDE( rcFree, rcArea.GetLeftBottom() ) &&
							 rcArea.GetLeft() != rcFree.GetLeft() &&
							 rcArea.GetBottom() != rcFree.GetBottom() ) vIncludePtList.push_back( rcArea.GetLeftBottom() );

				if( INCLUDE( rcFree, rcArea.GetRightBottom() ) &&
							 rcArea.GetRight() != rcFree.GetRight() &&
							 rcArea.GetBottom() != rcFree.GetBottom() ) vIncludePtList.push_back( rcArea.GetRightBottom() );

				assert( vIncludePtList.size() < 3 );

				// a1 b1 c1
				// a2 b2 c2
				// a3 b3 c3

				if( vIncludePtList.size() == 4 )
				{
					// assert( rcArea.GetLeftTop() == rcFree.GetLeftTop() );
					assert( 0 );
				}

				if( vIncludePtList.size() == 2 )
				{
					// case #1. [a:b:c] 3덩이로 나뉘는 경우
					if( vIncludePtList[0].y == vIncludePtList[1].y )
					{
						if( vIncludePtList[0].x > vIncludePtList[1].x ) std::swap( vIncludePtList[0].x, vIncludePtList[1].x );

						// case #1-1. [a:b1:c] 3덩이로 나뉘는 경우
						if( rcArea.GetBottom() >= rcFree.GetBottom() )
						{	
							Rect a( rcFree.GetLeft(), rcFree.GetTop(), rcArea.GetLeft() - rcFree.GetLeft(), rcFree.GetHeight() );
							Rect b1( rcArea.GetLeft(), rcFree.GetTop(), rcArea.GetWidth(), rcArea.GetBottom() - rcFree.GetTop() );
							Rect c( rcArea.GetRight(), rcFree.GetTop(), rcFree.GetRight() - rcArea.GetRight(), rcFree.GetHeight() );
							if( a.GetSize() )	pNewFreeRect->push_back( a );
							if( b1.GetSize() )	pNewFreeRect->push_back( b1 );
							if( c.GetSize() )	pNewFreeRect->push_back( c );
							return true;
						}
						// case #1-1. [a:b3:c] 3덩이로 나뉘는 경우
						else if( rcArea.GetTop() <= rcFree.GetTop() )
						{
							Rect a( rcFree.GetLeft(), rcFree.GetTop(), rcArea.GetLeft() - rcFree.GetLeft(), rcFree.GetHeight() );
							Rect b3( rcArea.GetLeft(), rcArea.GetBottom(), rcArea.GetWidth(), rcFree.GetBottom() - rcArea.GetBottom() );
							Rect c( rcArea.GetRight(), rcFree.GetTop(), rcFree.GetRight() - rcArea.GetRight(), rcFree.GetHeight() );
							if( a.GetSize() )	pNewFreeRect->push_back( a );
							if( b3.GetSize() )	pNewFreeRect->push_back( b3 );
							if( c.GetSize() )	pNewFreeRect->push_back( c );
							return true;
						}
						else assert( false );

						return false;
					}

					// case #2. [1:2:3] 3덩이로 나뉘는 경우
					if( vIncludePtList[0].x == vIncludePtList[1].x )
					{
						if( vIncludePtList[0].y > vIncludePtList[1].y ) std::swap( vIncludePtList[0].y, vIncludePtList[1].y );

						// case #2-1. [1:a2:3] 3덩이로 나뉘는 경우
						if( rcArea.GetRight() >= rcFree.GetRight() )
						{
							Rect _1( rcFree.GetLeft(), rcFree.GetTop(), rcFree.GetWidth(), rcArea.GetTop() - rcFree.GetTop() );
							Rect _a2( rcFree.GetLeft(), rcArea.GetTop(), rcArea.GetLeft() - rcFree.GetLeft(), rcArea.GetHeight() );
							Rect _3( rcFree.GetLeft(), rcFree.GetTop() + _1.GetHeight() + _a2.GetHeight(), rcFree.GetWidth(), rcFree.GetHeight() - _1.GetHeight() - _a2.GetHeight() );
							if( _1.GetSize() )	pNewFreeRect->push_back( _1 );
							if( _a2.GetSize() )	pNewFreeRect->push_back( _a2 );
							if( _3.GetSize() )	pNewFreeRect->push_back( _3 );
							return true;
						}
						// case #2-1. [1:c2:3] 3덩이로 나뉘는 경우
						else if( rcArea.GetLeft() <= rcFree.GetLeft() )
						{
							Rect _1( rcFree.GetLeft(), rcFree.GetTop(), rcFree.GetWidth(), rcArea.GetTop() - rcFree.GetTop() );
							Rect _c2( rcArea.GetRight(), rcArea.GetTop(), rcFree.GetRight() - rcArea.GetRight(), rcArea.GetHeight() );
							Rect _3( rcFree.GetLeft(), rcFree.GetTop() + _1.GetHeight() + _c2.GetHeight(), rcFree.GetWidth(), rcFree.GetHeight() - _1.GetHeight() - _c2.GetHeight() );
							if( _1.GetSize() )	pNewFreeRect->push_back( _1 );
							if( _c2.GetSize() )	pNewFreeRect->push_back( _c2 );
							if( _3.GetSize() )	pNewFreeRect->push_back( _3 );
							return true;
						}
						else assert( false );

						return false;
					}

					assert( false );

					return false;
				}

				if( vIncludePtList.size() == 0 )
				{
					//assert( !INCLUDE( rcFree, rcArea ) );

					// case #3. a 생성
					if( rcArea.GetLeft() > rcFree.GetLeft() )
					{
						pNewFreeRect->push_back( Rect( rcFree.GetLeft(), rcFree.GetTop(), rcArea.GetLeft() - rcFree.GetLeft(), rcFree.GetHeight() ) );
						//return true;
					}

					// case #4. c 생성
					if( rcArea.GetRight() < rcFree.GetRight() )
					{
						pNewFreeRect->push_back( Rect( rcArea.GetRight(), rcFree.GetTop(), rcFree.GetRight() - rcArea.GetRight(), rcFree.GetHeight() ) );
						//return true;
					}

					// case #5. 1 생성
					if( rcArea.GetTop() > rcFree.GetTop() )
					{
						pNewFreeRect->push_back( Rect( rcFree.GetLeft(), rcFree.GetTop(), rcFree.GetWidth(), rcArea.GetTop() - rcFree.GetTop() ) );
						//return true;
					}

					// case #6. 3 생성
					if( rcArea.GetBottom() < rcFree.GetBottom() )
					{
						pNewFreeRect->push_back( Rect( rcFree.GetLeft(), rcArea.GetBottom(), rcFree.GetWidth(), rcFree.GetBottom() - rcArea.GetBottom() ) );
						//return true;
					}

					// case #5. 한쪽이 완전히 잘려나가 크기가 줄어드는 경우
					return false;
				}

				assert( vIncludePtList.size() == 1 );

				if( vIncludePtList[0] == rcArea.GetLeftTop() )
				{
					pNewFreeRect->push_back( Rect( rcFree.GetLeft(), rcFree.GetTop(), rcFree.GetWidth(), rcArea.GetTop() - rcFree.GetTop() ) );
					pNewFreeRect->push_back( Rect( rcFree.GetLeft(), rcArea.GetTop(), rcArea.GetLeft() - rcFree.GetLeft(), rcFree.GetBottom() - rcArea.GetTop() ) );
					return true;
				}
				else if( vIncludePtList[0] == rcArea.GetRightTop() )
				{
					pNewFreeRect->push_back( Rect( rcFree.GetLeft(), rcFree.GetTop(), rcFree.GetWidth(), rcArea.GetTop() - rcFree.GetTop() ) );
					pNewFreeRect->push_back( Rect( rcArea.GetRight(), rcArea.GetTop(), rcFree.GetRight() - rcArea.GetRight(), rcFree.GetBottom() - rcArea.GetTop() ) );
					return true;
				}
				else if( vIncludePtList[0] == rcArea.GetLeftBottom() )
				{
					pNewFreeRect->push_back( Rect( rcFree.GetLeft(), rcFree.GetTop(), rcArea.GetLeft() - rcFree.GetLeft(), rcFree.GetHeight() ) );
					pNewFreeRect->push_back( Rect( rcArea.GetLeft(), rcArea.GetBottom(), rcFree.GetRight() - rcArea.GetLeft(), rcFree.GetBottom() - rcArea.GetBottom() ) );
					return true;
				}
				else if( vIncludePtList[0] == rcArea.GetRightBottom() )
				{
					pNewFreeRect->push_back( Rect( rcArea.GetRight(), rcFree.GetTop(), rcFree.GetRight() - rcArea.GetRight(), rcFree.GetHeight() ) );
					pNewFreeRect->push_back( Rect( rcFree.GetLeft(), rcArea.GetBottom(), rcFree.GetWidth() - ( rcFree.GetRight() - rcArea.GetRight() ), rcFree.GetBottom() - rcArea.GetBottom() ) );
					return true;
				}
				else assert( 0 );

				// etc. 인접한 2덩이로 나뉘는 경우
				return false;
			}

			typename std::vector< Rect >::iterator FindJoinableFreeRect( const Rect & rc )
			{
				std::vector< Rect >::iterator it;
				for( it = m_vFreeRectList.begin(); it != m_vFreeRectList.end(); ++it )
				{
					if( ( rc.GetLeft() == (*it).GetLeft() && rc.GetWidth() == (*it).GetWidth() ) )
					{
						if( rc.GetTop() + rc.GetHeight() == (*it).GetTop() ||
							rc.GetTop() == (*it).GetTop() + (*it).GetHeight() ) return it;
					}
						
					if( rc.GetTop() == (*it).GetTop() && rc.GetHeight() == (*it).GetHeight() )
					{
						if( rc.GetLeft() + rc.GetWidth()  == (*it).GetLeft() ||
							rc.GetLeft() == (*it).GetLeft() + (*it).GetWidth()  ) return it;

					}
				}

				return it;
			}

			void AddFreeRect( Rect & rc )
			{
				if( !m_bInitialized ) 
				{
					m_bInitialized = true;
					m_rcEffectiveArea = rc;
				}
				else
				{
					/*
					if( m_rcEffectiveArea.GetLeft() > rc.GetLeft() )		m_rcEffectiveArea.SetLeft( rc.GetLeft() );
					if( m_rcEffectiveArea.GetRight() < rc.GetRight() )		m_rcEffectiveArea.SetRight( rc.GetRight() );
					if( m_rcEffectiveArea.GetTop() > rc.GetTop() )			m_rcEffectiveArea.SetTop( rc.GetTop() );
					if( m_rcEffectiveArea.GetBottom() < rc.GetBottom() )	m_rcEffectiveArea.SetBottom( rc.GetBottom() );
					*/
				}

				m_FreeSize += rc.GetSize();

				std::vector< Rect >::iterator it;
				while( ( it = FindJoinableFreeRect( rc ) ) != m_vFreeRectList.end() )
				{
					if( rc.GetLeft() == (*it).GetLeft() && rc.GetRight() == (*it).GetRight() )
					{
						// 위에 붙음
						if( rc.GetTop() + rc.GetHeight() == (*it).GetTop() )
						{
							(*it).SetHeight( (*it).GetHeight() + rc.GetHeight() );
							(*it).Move( rc.GetX(), rc.GetY() );
							rc = *it;
							it = m_vFreeRectList.erase(it);
							continue;
						}

						// 아래에 붙음
						if( rc.GetTop() == (*it).GetTop() + (*it).GetHeight() )
						{
							(*it).SetHeight( (*it).GetHeight() + rc.GetHeight() );
							rc = *it;
							it = m_vFreeRectList.erase(it);
							continue;
						}
					}

					if( rc.GetTop() == (*it).GetTop() && rc.GetBottom() == (*it).GetBottom() )
					{
						// 왼쪽에 붙음
						if( rc.GetLeft() + rc.GetWidth() == (*it).GetLeft() )
						{
							(*it).SetWidth( (*it).GetWidth() + rc.GetWidth() );
							(*it).Move( rc.GetX(), rc.GetY() );
							rc = *it;
							it = m_vFreeRectList.erase(it);
							continue;
						}

						// 오른쪽에 붙음
						if( rc.GetLeft() == (*it).GetLeft() + (*it).GetWidth() )
						{
							(*it).SetWidth( (*it).GetWidth() + rc.GetWidth() );
							rc = *it;
							it = m_vFreeRectList.erase(it);
							continue;
						}
					}
				}

				for( it = m_vFreeRectList.begin(); it != m_vFreeRectList.end(); ++it )
				{
					if( COLLISION( *it, rc ) )
					{
						assert( 0 );
					}
				}
				m_vFreeRectList.push_back( rc );

				CalcPtList();
			}

			void CalcPtList()
			{
				m_FreeSize  = 0;

				std::vector< Rect >::iterator it;

				m_vXPtList.erase( m_vXPtList.begin(), m_vXPtList.end() );
				m_vYPtList.erase( m_vYPtList.begin(), m_vYPtList.end() );
				
				for( it = m_vFreeRectList.begin(); it != m_vFreeRectList.end(); ++it )
				{
					if( std::find( m_vXPtList.begin(), m_vXPtList.end(), (*it).GetLeft() ) == m_vXPtList.end() ) m_vXPtList.push_back( (*it).GetLeft() );
					if( std::find( m_vYPtList.begin(), m_vYPtList.end(), (*it).GetTop() ) == m_vYPtList.end() ) m_vYPtList.push_back( (*it).GetTop() );
					if( std::find( m_vXPtList.begin(), m_vXPtList.end(), (*it).GetRight() + 1 ) == m_vXPtList.end() ) m_vXPtList.push_back( (*it).GetRight() + 1 );
					if( std::find( m_vYPtList.begin(), m_vYPtList.end(), (*it).GetBottom() + 1 ) == m_vYPtList.end() ) m_vYPtList.push_back( (*it).GetBottom() + 1 );

					m_FreeSize += (*it).GetSize();
				}

				std::sort( m_vXPtList.begin(), m_vXPtList.end() );
				std::sort( m_vYPtList.begin(), m_vYPtList.end() );
			}

			bool				m_bInitialized;
			std::vector< T >	m_vXPtList;
			std::vector< T >	m_vYPtList;
			std::vector< Rect >	m_vFreeRectList;
			Rect				m_rcEffectiveArea;
			T					m_FreeSize;
		};

		QUAD_TREE						m_AllocatedRectTree;
		std::vector< FreeRectChunk >		m_vChunkList;

		struct _Rect
		{
			void operator()( const Rect & rc )
			{
				bFound = true;
				m_rect = rc;
			}
			bool bFound;
			Rect m_rect;
		};

		struct _RectList
		{
			void operator()( const Rect & rc )
			{
				bFound = true;
				m_lstRect.push_back( rc );
			}
			bool bFound;
			std::vector< Rect > m_lstRect;
		};
	};

}; // namespace X2D
