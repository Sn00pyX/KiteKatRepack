// X2DQuadTree_divide_polygon.h
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

#include <vector>
#include <cassert>

#include "X2DBasicTypes.h"


namespace X2D
{

	template< typename T, typename U, size_t THRESHOLD = 20, size_t MAX_DEPTH = 10 >
	struct QuadTree_divide_polygon
	{
		struct PointHelper
		{
			PointHelper( Point< double > _point, size_t _index )
				: point( _point ),
				nIndex( _index )
			{
				bUsed = false;
#ifdef	_DEBUG
				bIn = false;
#endif
				pPartner = NULL;
			}

			bool operator==( const Point< T >& r ) const
			{
				return (r.x == static_cast< T >( point.x )) &&
					(r.y == static_cast< T >( point.y ));
			}
			bool operator==( size_t idx ) const
			{
				return (nIndex == idx);
			}

			size_t nIndex; // 내 Index 정보
			Point< double > point; // 내 Point 정보(소수점 소실로 인한 오차 판별)
			bool bUsed; // 한번 사용 되었는 지 (폴리곤 중복 생성 안하기 위하여 구별)
#ifdef	_DEBUG
			bool bIn;
#endif
			PointHelper *pPartner; // Partner 정보
		};
		
		template< typename Allocator, typename Deleter >
		static std::vector< U >	divide_polygon( const U& polygon, const X2D::Line< T >& line, Allocator& allocator, Deleter& deleter )
		{
			std::vector< U > v;

			if( polygon->GetBoundingBox().IsLooseCollision( line ) == false &&
				polygon->IsLooseCollision( line ) == false )
			{
				v.push_back( polygon );
				return v;
			}

			X2D::Line< double > real_line( line.begin.x, line.begin.y, line.end.x, line.end.y );
			std::vector< PointHelper > vIntersection;
			std::vector< X2D::Point< T > > vList;
			X2D::Point< double > ptUnknownPT( 0 , 0 );
			// 1. index를 기록하면서 vList에 집어넣는다.
			for( size_t i = 0 ; i < polygon->Size(); ++i )
			{
				Line< T > currLine = polygon->GetSegment( i );
				vList.push_back( currLine.begin );

				Point< double > ptTemp;
				X2D::Line< double > real_curr( currLine.begin.x, currLine.begin.y, currLine.end.x, currLine.end.y );
				if( real_line.GetIntersectPoint( real_curr, &ptTemp ) )
				{
					// 시작점이 접해 있다면 끝점일때가 나오므로 그때 판단한다.
					if( ptTemp == real_curr.begin )
					{
						continue;
					}
					

					// 끝점이 접해 있다면 주변 상황을 파악해야 한다.
					// 1. 접해만 있고 나눌 필요없는 경우(|<), 2. 일치하는 경우(|), 3.접해서 통과하는 경우 (/|\)
					// 이 판단은 현재 선 첫 점과 다음선 끝 점을 선으로 만들어 기준선과 접하는지 본다.
					if( ptTemp == real_curr.end )
					{
						// 현재 점과 다다음의 점으로 가는 선을 구한다.
						size_t next_idx = i+1;
						if( next_idx >= polygon->Size() )	// 끝점.
						{
							next_idx = 0;
						}

						Line< T > nextLine = polygon->GetSegment( next_idx );
						Point< double > begin;
						if( ptUnknownPT == Point< double >( 0, 0 ) )
						{
							begin = real_curr.begin;
						}
						else
						{
							begin = ptUnknownPT;
						}

						Line< double > testLine( begin.x, begin.y, nextLine.end.x, nextLine.end.y );

						Point< double > ptTest;
						// 1. 접해만 있고 나눌 필요없는 경우(|<) -> vIntersection이 아님
						if( real_line.GetIntersectPoint( testLine, &ptTest ) == false )
						{
							ptUnknownPT.Set( 0, 0 );
							continue;
						}

						// 2-a). nextLine이 겹치는 경우( | ) 
						// -> 겹친 선을 빠져 나갈 때 판단 (2-b)를 기다린다 (반드시 다음 케이스가 2-b로 가게 됨, 단 처음부터 겹치는 점에서 시작하면 2-b)인 경우가 있긴 함)
						if( ptTest == testLine.end )
						{
							if( ptUnknownPT == Point< double >( 0, 0 ) )
							{
								ptUnknownPT = testLine.begin;
							}
							continue;
						}

						// 2-b). curLine 겹치는 경우( | ) -> 같은 쪽이면 X, 다른 쪽으로 나가면 점 기록
						if( ptTest == testLine.begin )
						{
							if( i == 0 ) // 맨 처음이 하필 겹치는 경우(예전 점 정보가 없으므로 끝점 정보 가져옴)
							{
								Point< T > lastPt = polygon->GetPoint( polygon->Size()-1 );
								ptUnknownPT.Set( lastPt.GetX(), lastPt.GetY() );
							}

							Line< double > newTestLine( ptUnknownPT, testLine.end );
							ptUnknownPT.Set( 0, 0 );
							// 단순히 겹쳤다가 다시 같은 쪽으로 가므로 아무 짓도 안 함
							if( real_line.GetIntersectPoint( testLine, &ptTest ) == false )
								continue;
						}

						// 3. 접해서 통과하는 경우 (/|\), 그리고 2-b)에서 겹쳐서 밖으로 나오는 경우
						// 맨마지막 꺼 외에는 vList에는 다음에 들어갈 것이다.
						size_t idx = vList.size();
						if( next_idx == 0 )
						{
							idx = 0;
						}

						vIntersection.push_back( PointHelper( ptTemp, idx ) );
						ptUnknownPT.Set( 0, 0 );
						continue;
					}

					Point< T > input( static_cast< T >( ptTemp.x ), static_cast< T >( ptTemp.y ) );
					vList.push_back( input );
					vIntersection.push_back( PointHelper( ptTemp, vList.size()-1 ) );
				}
			}
			// 선 분할 끝났는데 미 처리 점이 남아있다면 이 녀석도 접점임. (첫 번째 점이겠지)
// 			if( ptUnknownPT.GetX() != 0 || ptUnknownPT.GetY() != 0 )
// 			{
// 				vIntersection.push_back( PointHelper( Point< double >( ptUnknownPT.GetX(), ptUnknownPT.GetY() ) , 0 ) );
// 			}

			struct IntersectionPredicate
			{
				IntersectionPredicate( Point< double > _ptStandard )
					: m_ptStandard( _ptStandard )
				{}

				bool operator() ( PointHelper& lhs, PointHelper& rhs )
				{
					double ld = lhs.point.GetDistance( m_ptStandard );
					double rd = rhs.point.GetDistance( m_ptStandard );
					assert( ld != rd );
					if( ld < rd )
						return true;

					return false;
				}

			private:
				Point< double >	m_ptStandard;
			};

			if( vIntersection.empty() == true )
			{
				v.push_back( polygon );
				return v;
			}

			// 2. 분할 line에 가까운 쪽으로 sorting을 통하여 vLineList 라인을 정리한다.
			assert( (vIntersection.size() % 2) == 0 );
			std::sort( vIntersection.begin(), vIntersection.end(), IntersectionPredicate( real_line.begin ) );

#ifdef	_DEBUG
			for( auto it = vIntersection.begin(); it != vIntersection.end(); ++it )
			{
				PointHelper &a = (*it);
				a.bIn = false;
				size_t next_idx = a.nIndex + 1;
				if( next_idx >= vList.size() )
				{
					next_idx = 0;
				}

				if( line.begin.x == line.end.x )
				{
					if( vList[next_idx].x < line.begin.x )
					{
						a.bIn = true;
					}
				}

				if( line.begin.y == line.end.y )
				{
					if( vList[next_idx].y < line.begin.y )
					{
						a.bIn = true;
					}
				}
			}
#endif

			for( auto it = vIntersection.begin() ; it != vIntersection.end() ; it = it+2 )
			{
				PointHelper &a = (*it);
				PointHelper &b = (*(it+1));

				assert( a.bIn != b.bIn );

				a.pPartner = &b;
				b.pPartner = &a;
			}
			
			// 3. 폴리곤 생성 시작
			Polygon< T > myPolygon;
			if( myPolygon.Set( vList.begin(), vList.end() ) == false )
			{
				assert( 0 );
				v.push_back( polygon );
				return v;
			}

			std::vector< size_t > vCoverList;
			vCoverList.push_back( 0 );
			while( !vCoverList.empty() )
			{
				// 폴리곤 형성 시작
				size_t i = *(vCoverList.begin());
				vCoverList.erase( vCoverList.begin() );
				std::vector< X2D::Point< T > > pol;
				Point< T > pt = myPolygon.GetPoint( i );
				pol.push_back( pt );
				i = myPolygon.getNextIndex( i );
				pt = myPolygon.GetPoint( i );
				while( pol[0] != pt )
				{
					pol.push_back( pt );
					// 이 점이 라인 상에 있는 점인지 찾음
					auto itInter = std::find( vIntersection.begin(), vIntersection.end(), i );
					// 라인 상에 있다면 점프
					if( itInter != vIntersection.end() )
					{
						PointHelper *pPartner = (*itInter).pPartner;
						if( !(*itInter).bUsed )
						{
							pPartner->bUsed = true;
							(*itInter).bUsed = true;
							vCoverList.push_back( (*itInter).nIndex );
						}
						i = pPartner->nIndex;
						pt = myPolygon.GetPoint( i );
						if( pol[0] == pt )
							break;
						pol.push_back( pt );
						i = myPolygon.getNextIndex( i );
						pt = myPolygon.GetPoint( i );
					}
					// 라인 상의 점이 아니라면 계속 진행
					else
					{
						i = myPolygon.getNextIndex( i );
						pt = myPolygon.GetPoint( i );
					}
				}

 				U pp = allocator();
 				if( pp->Set( pol.begin(), pol.end() ) == false )
				{
					assert( 0 );
					for( auto pos = v.begin(); pos != v.end(); ++pos )
					{
						deleter( *pos );
					}

					v.clear();
					v.push_back( polygon );
					return v;
				}

 				v.push_back( pp );
			}

			return v;
		}

		typedef Box< T > NodeType;

		QuadTree_divide_polygon( const T & left, const T & top, const T & width, const T & height )
			: m_masterNode( 1, left, top, width, height )
		{
		}

		QuadTree_divide_polygon( const T & width, const T & height )
			: m_masterNode( 1, 0, 0, width, height )
		{
		}

		~QuadTree_divide_polygon()
		{
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

		size_t getItemCount() const
		{
			return m_masterNode.getItemCount();
		}

		template< typename Allocator, typename Deleter >
		bool Add( const U & u, Allocator& allocator, Deleter& deleter )
		{
			return m_masterNode.Add( u, allocator, deleter );
		}

		template< typename C, typename Deleter >
		void Remove( const C & c, Deleter& deleter )
		{
			m_masterNode.Remove( c, deleter );
		}

		template< typename C >
		void Enum( const C & c, std::vector< U > * pResult )
		{
			_FUNCTOR_ADAPTOR adaptor;
			adaptor.pResult = pResult;
			m_masterNode.Enum( c, adaptor );
		}

		template< typename C >
		void EnumLoose( const C & c, std::vector< U > * pResult )
		{
			_FUNCTOR_ADAPTOR adaptor;
			adaptor.pResult = pResult;
			m_masterNode.EnumLoose( c, adaptor );
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

			bool hasChildNode() const
			{
				return m_pNode[0] != NULL;
			}

			template< typename Allocator, typename Deleter >
			bool Add( const U & u, Allocator& allocator, Deleter& deleter )
			{
				if( !COLLISION( m_rcEffectiveArea, u ) ) 
				{
					assert( 0 );
					return false;
				}

				// 자식 노드가 있다면
				if( hasChildNode() )
				{
					//////////////////////////////////////////////////////////////////////////
					T half_width		= m_rcEffectiveArea.GetWidth() / 2;
					T half_height		= m_rcEffectiveArea.GetHeight() / 2;

					// 분할 선
					X2D::Line<T> width_center( m_rcEffectiveArea.GetLeft() + half_width, m_rcEffectiveArea.GetTop(),
						m_rcEffectiveArea.GetLeft() + half_width, m_rcEffectiveArea.GetBottom() );
					X2D::Line<T> height_center( m_rcEffectiveArea.GetLeft(), m_rcEffectiveArea.GetTop() + half_height,
						m_rcEffectiveArea.GetRight(), m_rcEffectiveArea.GetTop() + half_height );

					// 입력된 폴리곤 분할
					std::vector< U > r;	// 결과

					// width_center로 분할
					std::vector< U > v = QuadTree_divide_polygon::divide_polygon( u, width_center, allocator, deleter );

					// 분할된 폴리곤 다시 height_center로 분할
					for( auto pos = v.begin(); pos != v.end(); ++pos )
					{
						std::vector< U > t = QuadTree_divide_polygon::divide_polygon( (*pos), height_center, allocator, deleter );
						r.insert( r.end(), t.begin(), t.end() );
					}
					//////////////////////////////////////////////////////////////////////////

					// 자식 노드에 추가
					bool bdivide = (r.size() > 1);
					for( auto pos = r.begin(); pos != r.end(); )
					{
						const U& polygon = (*pos);
						Node *pNode = getFitNode( polygon );
						if( pNode )
						{
							if( pNode->Add( polygon, allocator, deleter ) == true )
							{
								pos = r.erase( pos );
								continue;
							}
						}

						assert( 0 );
						++pos;
					}

					// 모두 자식에게 넣기 실패했다면 자신이 가진다.
					if( !r.empty() )
					{
						m_vList.push_back( u );
					}
					// 분할 되었고 모두 자식에게 넣었다면 u 삭제
					else if( bdivide == true && r.empty() )
					{
						deleter( u );
					}

					// 추가 실패한 것들 정리
					for( auto pos = r.begin(); pos != r.end(); ++pos )
					{
						const U& polygon = (*pos);
						// 만약 u 추가에 실패했다면 부모가 처리한다.
						if( polygon != u )
						{
							deleter( polygon );
						}
					}
				}
				else
				{
					add( u, allocator, deleter );
				}

				return true;
			}

			template< typename C, typename Deleter >
			void Remove( const C & c, Deleter& deleter )
			{
				if( hasChildNode() )
				{
					// quadtree(0,0,100,100) 일경우 Box(70,70,80,80) 를 add() 하면 m_pNode[3] 에 들어간다.
					// 그런데 이걸 point(70,70) 으로 검사하면 m_pNode[0] 이 fit 이 된다.
					// 그러므로 4 노드 모두 검사 해야 한다.
					m_pNode[0]->Remove( c, deleter );
					m_pNode[1]->Remove( c, deleter );
					m_pNode[2]->Remove( c, deleter );
					m_pNode[3]->Remove( c, deleter );
				}

				for( std::vector< U >::iterator it = m_vList.begin(); it != m_vList.end(); )
				{
					if( COLLISION( (*it), c ) ) 
					{
						deleter( (*it) );

						it = m_vList.erase( it );
						continue;
					}
					++it;
				}
			}

			bool IsAddable( const U & u ) const
			{
				return CLOSELY_INCLUDE( m_rcEffectiveArea, u );
			}

			template< typename C >
			bool Collision( const C & c ) const
			{
				if( !COLLISION( m_rcEffectiveArea, c ) )
					return false;

				for( std::vector< U >::const_iterator it = m_vList.begin(); it != m_vList.end(); ++it )
				{
					if( COLLISION( (*it), c ) )
						return true;
				}

				if( hasChildNode() )
				{	
					// quadtree(0,0,100,100) 일경우 Box(70,70,80,80) 를 add() 하면 m_pNode[3] 에 들어간다.
					// 그런데 이걸 point(70,70) 으로 검사하면 m_pNode[0] 이 fit 이 된다.
					// 그러므로 4 노드 모두 검사 해야 한다.
					if( m_pNode[0]->Collision( c ) )
						return true;
					if( m_pNode[1]->Collision( c ) )
						return true;
					if( m_pNode[2]->Collision( c ) )
						return true;
					if( m_pNode[3]->Collision( c ) )
						return true;
				}

				return false;
			}

			template< typename C >
			bool LooseCollision( const C & c ) const
			{
				if( !COLLISION( m_rcEffectiveArea, c ) )
					return false;

				for( std::vector< U >::const_iterator it = m_vList.begin(); it != m_vList.end(); ++it )
				{
					if( LOOSE_COLLISION( (*it), c ) )
						return true;
				}

				if( hasChildNode() )
				{	
					// quadtree(0,0,100,100) 일경우 Box(70,70,80,80) 를 add() 하면 m_pNode[3] 에 들어간다.
					// 그런데 이걸 point(70,70) 으로 검사하면 m_pNode[0] 이 fit 이 된다.
					// 그러므로 4 노드 모두 검사 해야 한다.
					if( m_pNode[0]->LooseCollision( c ) )
						return true;
					if( m_pNode[1]->LooseCollision( c ) )
						return true;
					if( m_pNode[2]->LooseCollision( c ) )
						return true;
					if( m_pNode[3]->LooseCollision( c ) )
						return true;
				}

				return false;
			}

			template< typename C, typename F >
			void Enum( const C & c, F & f ) const
			{
				if( !COLLISION( m_rcEffectiveArea, c ) )
					return;

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

				for( std::vector< U >::const_iterator it = m_vList.begin(); it != m_vList.end(); ++it )
				{
					if( COLLISION( (*it), c ) )
						f( *it );
				}
			}

			template< typename C, typename F >
			void EnumLoose( const C & c, F & f )
			{
				if( !COLLISION( m_rcEffectiveArea, c ) )
					return;

				if( hasChildNode() )
				{	
					// quadtree(0,0,100,100) 일경우 Box(70,70,80,80) 를 add() 하면 m_pNode[3] 에 들어간다.
					// 그런데 이걸 point(70,70) 으로 검사하면 m_pNode[0] 이 fit 이 된다.
					// 그러므로 4 노드 모두 검사 해야 한다.
					m_pNode[0]->EnumLoose( c, f );
					m_pNode[1]->EnumLoose( c, f );
					m_pNode[2]->EnumLoose( c, f );
					m_pNode[3]->EnumLoose( c, f );
				}

				for( std::vector< U >::iterator it = m_vList.begin(); it != m_vList.end(); ++it )
				{
					U& u = (*it);
					if( u->GetBoundingBox().IsCollision( c ) == true )
						f( u );
				}
			}

		private:

			Node()
			{
				m_pNode[0] = NULL;
				m_pNode[1] = NULL;
				m_pNode[2] = NULL;
				m_pNode[3] = NULL;
			}

			Node* getFitNode( const U & u ) const
			{
				if( m_pNode[0]->IsAddable( u ) == true )
					return m_pNode[0];
				if( m_pNode[1]->IsAddable( u ) == true )
					return m_pNode[1];
				if( m_pNode[2]->IsAddable( u ) == true )
					return m_pNode[2];
				if( m_pNode[3]->IsAddable( u ) == true )
					return m_pNode[3];

				return NULL;
			}

			template< typename Allocator, typename Deleter >
			void add( const U & u, Allocator& allocator, Deleter& deleter )
			{
				m_vList.push_back( u );

				if( u->Size() > THRESHOLD && m_unDepth < MAX_DEPTH )
					divide( allocator, deleter );
			}

			template< typename Allocator, typename Deleter >
			void divide( Allocator& allocator, Deleter& deleter )
			{
				if( hasChildNode() )
					return;

				//////////////////////////////////////////////////////////////////////////
				// 자식 분할
				// 더 자를 수 없다.
				if( m_rcEffectiveArea.GetWidth() <= 2 || m_rcEffectiveArea.GetHeight() <= 2 )
					return;

				m_pNode[0] = new Node;
				m_pNode[1] = new Node;
				m_pNode[2] = new Node;
				m_pNode[3] = new Node;

				T half_width		= m_rcEffectiveArea.GetWidth() / 2;
				T half_height		= m_rcEffectiveArea.GetHeight() / 2;

				m_pNode[0]->init( m_unDepth + 1,
								m_rcEffectiveArea.GetLeft(), m_rcEffectiveArea.GetTop(),
								half_width, half_height );
				m_pNode[1]->init( m_unDepth + 1,
								m_rcEffectiveArea.GetLeft() + half_width, m_rcEffectiveArea.GetTop(),
								m_rcEffectiveArea.GetWidth() - half_width, half_height );
				m_pNode[2]->init( m_unDepth + 1,
								m_rcEffectiveArea.GetLeft(), m_rcEffectiveArea.GetTop() + half_height,
								half_width, m_rcEffectiveArea.GetHeight() - half_height );
				m_pNode[3]->init( m_unDepth + 1,
								m_rcEffectiveArea.GetLeft() + half_width, m_rcEffectiveArea.GetTop() + half_height,
								m_rcEffectiveArea.GetWidth() - half_width, m_rcEffectiveArea.GetHeight() - half_height );
				//////////////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////////////
				// 자식에게 분배
				std::vector< U > vTmpList = std::move( m_vList );
				for( auto it = vTmpList.begin(); it != vTmpList.end(); ++it )
				{
					if( Add( *it, allocator, deleter ) == false )
					{
						assert( 0 );
					}
				}
				//////////////////////////////////////////////////////////////////////////
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

		struct Node m_masterNode;
	};

}; // namespace X2D
