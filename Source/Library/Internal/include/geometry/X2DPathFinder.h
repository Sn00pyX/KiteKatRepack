// X2DPathFinder.h
//
//  by Testors , 2005/08/01

#pragma once


#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>

#include "X2DPolygon.h"
#include "X2DLeakyPolygon.h"
#include "../toolkit/XCyclicIndex.h"
#include "../toolkit/XDijkstra.h"


const int MAX_POLYGON_POINT = 300; //150 정도 되면, 제대로 검사 안됨


namespace X2D
{

	template< typename T >
	struct DeadEndChecker
	{
		static bool IsDeadEnd( const T& polygon, size_t index ) { return false; }
	};

	template< typename T >
	struct DeadEndChecker< X2D::LeakyPolygon< T > >
	{
		static bool IsDeadEnd( const X2D::LeakyPolygon< T >& polygon, size_t index ) 
		{ 
			return polygon.IsDeadEnd( index );
		}
	};

	template< typename T, typename PT >
	struct PathFinder
	{
	public:

		typedef typename NakedType< PT >::result	NakedPT;
		typedef std::vector< Point<T> > PATH;
		typedef DeadEndChecker< NakedPT >			CheckDeadEnd;

		template< typename PI >
		static void Find( PI begin ,PI end, const X2D::Point<T> & start, const X2D::Point<T> & goal, PATH & vPath, const T & max_length = 0 )
		{
			_Find( begin, end, start, goal, vPath, max_length );
		}

	private:

		static inline T GetFastPathLength( const std::vector< Point<T> > & vPath )
		{
			T total_length = 0;
			for( size_t i = 0; i+1 < vPath.size(); ++i )
			{
				total_length += vPath[i].GetDistance( vPath[i+1] );
			}		
			return total_length;
		}

		static inline bool IS_INSIDE( const bool & bIsClockWise, const Line<T> & prev_line, const Line<T> & line, const Point<T> & pt )
		{
			assert( prev_line.end == line.begin );

			bool bIsLeftSide = false;

			CCW_RESULT result_p = CheckClockWise( prev_line.begin, prev_line.end, line.end );
			CCW_RESULT result_1 = CheckClockWise( prev_line.begin, prev_line.end, pt );
			CCW_RESULT result_2 = CheckClockWise( line.end, line.begin, pt );

			if( bIsClockWise )
			{
				if( result_p == X2D::PARALLELISM )
				{
					return ( result_1 == X2D::CLOCK_WISE && result_2 == X2D::COUNTER_CLOCK_WISE );
				}
				else if( result_p == X2D::CLOCK_WISE )
				{
					return ( result_1 == X2D::CLOCK_WISE && result_2 == X2D::COUNTER_CLOCK_WISE );
				}
				else if( result_p == X2D::COUNTER_CLOCK_WISE )
				{
					if( result_1 == X2D::CLOCK_WISE && result_2 == X2D::CLOCK_WISE ) return true;
					else if( result_1 == X2D::PARALLELISM && result_2 == X2D::COUNTER_CLOCK_WISE ) return true;
					else if( result_2 == X2D::PARALLELISM && result_1 == X2D::CLOCK_WISE ) return true;
					else if( result_1 == X2D::CLOCK_WISE && result_2 == X2D::COUNTER_CLOCK_WISE ) return true;
					else if( result_1 == X2D::COUNTER_CLOCK_WISE && result_2 == X2D::COUNTER_CLOCK_WISE ) return true;
				}
			}
			else
			{
				if( result_p == X2D::PARALLELISM )
				{
					return ( result_1 == X2D::COUNTER_CLOCK_WISE && result_2 == X2D::CLOCK_WISE );
				}
				else if( result_p == X2D::COUNTER_CLOCK_WISE )
				{
					return ( result_1 == X2D::COUNTER_CLOCK_WISE && result_2 == X2D::CLOCK_WISE );
				}
				else if( result_p == X2D::CLOCK_WISE )
				{
					if( result_1 == X2D::CLOCK_WISE && result_2 == X2D::CLOCK_WISE ) return true;
					else if( result_1 == X2D::PARALLELISM && result_2 == X2D::CLOCK_WISE ) return true;
					else if( result_2 == X2D::PARALLELISM && result_1 == X2D::COUNTER_CLOCK_WISE ) return true;
					else if( result_1 == X2D::COUNTER_CLOCK_WISE && result_2 == X2D::CLOCK_WISE ) return true;
					else if( result_1 == X2D::COUNTER_CLOCK_WISE && result_2 == X2D::COUNTER_CLOCK_WISE ) return true;
				}
			}

			return false;
		}

		static inline bool LOOSE_COLLISION( const bool & bIsClockWise, const Line<T> & prev_line, const Line<T> & line, const Line<T> & target )
		{
			return IS_INSIDE( bIsClockWise, prev_line, line, target.begin ) || IS_INSIDE( bIsClockWise, prev_line, line, target.end );
		}

		// 길찾기를 위해 필요한 느슨한 라인-폴리곤 충돌검사.
		// 만약 라인이 폴리곤을 구성하는 라인과 겹친다면 false 를 리턴한다.
		static inline bool LOOSE_COLLISION( const NakedPT & p, const Line<T> & line )
		{
			// TODO : 바운딩 박스 체크
			if( !p.GetBoundingBox().IsCollision( line ) ) return false;

			// 라인이 폴리곤을 구성하는 라인중 하나와 동일하다면 false
			//if( p.Has( line ) ) return false;

			size_t nTouchCnt = 0;

			Line<T>::INTERSECT_RESULT result = Line<T>::SEPARATE;
			Line<T>::INTERSECT_RESULT first_result = Line<T>::SEPARATE;
			Line<T>::INTERSECT_RESULT prev_result = Line<T>::SEPARATE;
			for( size_t idx = 0; idx < p.Size(); ++idx )
			{	
				Line<T> current_line = p.GetSegment( idx );
				result = current_line.IntersectCCW( line );

				// 라인이 교차하면 충돌이다.
				if( result == Line<T>::INTERSECT ) return true;

				if( result == Line<T>::TOUCH ) 
				{
					if( idx == 0 ) first_result = result;
					else
					{
						if( prev_result == Line<T>::TOUCH )
						{
							if( LOOSE_COLLISION( p.IsClockWise(), p.GetSegment( idx - 1 ), current_line, line ) )
								return true;
						}
					}

					nTouchCnt++;
				}
				prev_result = result;
			}

			if( result == Line<T>::TOUCH && first_result == Line<T>::TOUCH )
			{
				if( LOOSE_COLLISION( p.IsClockWise(), Line< T >( p.GetPoint( p.Size()-1 ), p.GetPoint( 0 ) ), p.GetSegment( 0 ), line ) ) 
					return true;
			}

			return false;
		}

		template< typename PI >
		static inline bool LOOSE_COLLISION( PI begin ,PI end, const Line<T> & line )
		{
			for( ; begin != end; ++begin ) 
			{
				if( LOOSE_COLLISION( *(*begin), line ) ) return true;
			}

			return false;
		}

		template< typename PI >
		static inline bool INCLUDE( PI begin ,PI end, const Point<T> & pt )
		{
			for( ; begin != end; ++begin ) 
			{
				if( X2D::INCLUDE( *begin, pt ) ) return true;
			}

			return false;
		}

		static inline void NORMALIZE( const Polygon<T> & p, std::vector< Point<T> > & vResult )
		{
			size_t z = vResult.size();

			if( z < 3 ) return;

			size_t nStartIdx = 1;
			if( p.Has( vResult.front() ) ) nStartIdx = 0;

			for( size_t i = 1; i < z-1; ++i )
			{
				for( size_t x = 0; x < z - i - 2; ++x )
				{
					size_t t = z - 1 - x;

					if( p.Has( Line<T>( vResult[i], vResult[t] ) ) ) continue;

					if( LOOSE_COLLISION( p, Line<T>( vResult[i], vResult[t] ) ) ) continue;
					
					vResult.erase( vResult.begin()+i+1, vResult.begin()+t );

					NORMALIZE( p, vResult );
					return;
				}
			}
		}
		
		template< typename PI >
		static inline void NORMALIZE( PI begin ,PI end, std::vector< Point<T> > & vResult )
		{
			size_t z = vResult.size();

			if( z < 3 ) return;

			for( size_t i = 0; i < z-1; ++i )
			{
				for( size_t x = 0; x < z - i - 2; ++x )
				{
					size_t t = z - 1 - x;

					//if( p.Has( Line<T>( vResult[i], vResult[t] ) ) ) continue;

					if( LOOSE_COLLISION( begin, end, Line<T>( vResult[i], vResult[t] ) ) ) continue;
					
					vResult.erase( vResult.begin()+i+1, vResult.begin()+t );

					NORMALIZE( begin, end, vResult );
					return;
				}
			}
		}

		static void getPath( const NakedPT & pl, const Point<T> & start, const Point<T> & end, PATH * pvPath )
		{
			PATH vGreenPath, vLeftPath, vRightPath;		
			std::vector< Point<T> >	vGreenNodeList;

			// bool bRtn = LOOSE_COLLISION( pl, Line<T>( Point<int>( 1250, 349 ), Point<int>( 775, 508 ) ) );

			// bool bStartNodeIsPartOfPolygon = pl.Has( start );

			pvPath->reserve( pl.Size() + 2 );

			// 각 점들이 시작점과 끝점에서 직접 도달 가능한지 검사해 놓는다.
			enum { 
				START	= 0x0001 << 0, 
				END		= 0x0001 << 1, 
				DEADEND	= 0x0001 << 2, 
			};
			std::vector< char >	vVisibleProperty( pl.Size(), 0 );
			size_t				nStartIndex = std::numeric_limits< size_t >::max();
			T					nStartLength = std::numeric_limits< T >::max();

			// 퍼포먼스를 위해서.. 임시로 삽입. 정밀하게 하고싶으면 nAdd 를 1로 세팅할것.
			//size_t					nAdd = ( pl.Size()/10 + 1 );
			size_t					nAdd = 1;	// 1.11.1	실내 던전 길찾기 오류 수정

			CCW_RESULT	nResult = pl.IsClockWise() ? CLOCK_WISE : COUNTER_CLOCK_WISE;
			for( size_t i = 0; i < pl.Size(); i += nAdd )
			{
				X2D::Point< T > pt = pl.GetPoint(i);

				if( CheckDeadEnd::IsDeadEnd( pl, i ) )
					vVisibleProperty[ i ] |= DEADEND;


				// { 일종의 은면제거랄까.. -_-
				bool bNeedStartCheck = true;
				bool bNeedEndCheck = true;

				X2D::Point< T > prev_pt = pl.GetPrevPoint( i );
				X2D::Point< T > next_pt = pl.GetNextPoint( i );
				
				CCW_RESULT nResult1 = X2D::CheckClockWise( start, pt, prev_pt );
				CCW_RESULT nResult2 = X2D::CheckClockWise( start, pt, next_pt );
				CCW_RESULT nResult3 = X2D::CheckClockWise( end, pt, prev_pt );
				CCW_RESULT nResult4 = X2D::CheckClockWise( end, pt, next_pt );

				CCW_RESULT nResultPl = X2D::CheckClockWise( prev_pt, pt, next_pt );
				if( nResultPl == nResult )
				{
					if( nResult1 != PARALLELISM && nResult2 != PARALLELISM && nResult1 != nResult2 && nResult != nResult1 ) bNeedStartCheck = false;
					if( nResult3 != PARALLELISM && nResult4 != PARALLELISM && nResult3 != nResult4 && nResult != nResult3 ) bNeedEndCheck = false;
				}
				else
				{
					if( nResult1 == PARALLELISM || nResult2 == PARALLELISM || nResult1 == nResult2 || nResultPl == nResult1 ) 
						bNeedStartCheck = false;
					if( nResult3 == PARALLELISM || nResult4 == PARALLELISM || nResult3 == nResult4 || nResultPl == nResult3 ) 
						bNeedEndCheck = false;
				}
				// }

				if( pl.GetPoint(i) == start )
				{	
					vVisibleProperty[i] |= START;	
				}
				if( pl.GetPoint(i) == end )
				{	
					vVisibleProperty[i] |= END;		
				}
				if( bNeedStartCheck && !LOOSE_COLLISION( pl, Line<T>( pt, start ) ) )	
				{	
					vVisibleProperty[i] |= START;	
				}
				if( bNeedEndCheck && !LOOSE_COLLISION( pl, Line<T>( pt, end	) ) )		
				{	
					vVisibleProperty[i] |= END;		
				}

				if( vVisibleProperty[i] & START )
				{
					// { 한 점만 거쳐서 바로 도달이 가능하다면 그 길을 찾아 놓는다.
					if( vVisibleProperty[i] & END )
					{
						vGreenNodeList.push_back( pt );
					}
					// }
					else
					{
						T length = start.GetDistance( pt );
						if( length < nStartLength )
						{
							nStartLength = length;
							nStartIndex = i;
						}
					}
				}
			}

			// { 한번에 거쳐가는 길 중 가장 빠른것을 추려낸다.
			if( !vGreenNodeList.empty() )
			{
				T green_length	= std::numeric_limits< T >::max();
				size_t idx = 0;
				for( size_t i = 0; i < vGreenNodeList.size(); ++i )
				{
					T current_path_length = vGreenNodeList[i].GetDistance( start ) + vGreenNodeList[i].GetDistance( end );
					if( current_path_length < green_length ) 
					{
						green_length = current_path_length;
						idx = i;
					}
				}
				vGreenPath.reserve( 3 );
				vGreenPath.push_back( start );
				vGreenPath.push_back( vGreenNodeList[idx] );
				vGreenPath.push_back( end );
			}
			// }

			if( nStartIndex == std::numeric_limits< size_t >::max() )
			{
				//assert( 0 );
				return;
			}

			vLeftPath.push_back( start );
			vRightPath.push_back( start );

			bool bLeftEnd = false;
			bool bRightEnd = false;

			T nRightLength = 0;
			T nLeftLength = 0;

			xmod::cyclic_index< int > left_idx( 0, int( pl.Size() ) ), next_left_idx( 0, int( pl.Size() ) );
			xmod::cyclic_index< int > right_idx( 0, int( pl.Size() ) ), next_right_idx( 0, int( pl.Size() ) );
			left_idx = int( nStartIndex );
			right_idx = int( nStartIndex );

			for( size_t i = 0; i < pl.Size() ; ++i, ++left_idx, --right_idx )
			{
				if( bRightEnd && bLeftEnd ) break;

				// { 왼쪽으로 비비고~
				while( !bLeftEnd && vLeftPath.size() < MAX_POLYGON_POINT )
				{
					next_left_idx = left_idx + 1;

					vLeftPath.push_back( pl.GetPoint(left_idx) );
					nLeftLength += vLeftPath[ vLeftPath.size() - 2 ].GetDistance( vLeftPath[ vLeftPath.size() - 1 ] );

					if( vVisibleProperty[left_idx] & END ) 
					{
						vLeftPath.push_back( end );
						nLeftLength += vLeftPath[ vLeftPath.size() - 2 ].GetDistance( vLeftPath[ vLeftPath.size() - 1 ] );
						bLeftEnd = true;
						break;
					}

					if( vVisibleProperty[left_idx] & START )
					{
						nLeftLength = 0;
						vLeftPath.erase( vLeftPath.begin() + 1, vLeftPath.end() );
						vLeftPath.push_back( pl.GetPoint(left_idx) );
						nLeftLength += vLeftPath[ vLeftPath.size() - 2 ].GetDistance( vLeftPath[ vLeftPath.size() - 1 ] );
					}

					if( bRightEnd && !vRightPath.empty() )
					{
						if( nLeftLength > nRightLength * 2 ) 
						{
							vLeftPath.clear();
							bLeftEnd = true;
							break;
						}
					}

					if( ( vVisibleProperty[ left_idx ] & DEADEND ) && ( vVisibleProperty[ next_left_idx ] & DEADEND ) )
					{
						vLeftPath.clear();
						bLeftEnd = true;
						break;
					}

					break;
				}
				// }

				// { 오른쪽으로 비비고~	
				while( !bRightEnd && vRightPath.size() < MAX_POLYGON_POINT )
				{
					next_right_idx = right_idx - 1;

					vRightPath.push_back( pl.GetPoint(right_idx) );
					nRightLength += vRightPath[ vRightPath.size() - 2 ].GetDistance( vRightPath[ vRightPath.size() - 1 ] );

					if( vVisibleProperty[right_idx] & END )
					{
						vRightPath.push_back( end );
						nRightLength += vRightPath[ vRightPath.size() - 2 ].GetDistance( vRightPath[ vRightPath.size() - 1 ] );
						bRightEnd = true;
						break;
					}

					if( vVisibleProperty[right_idx] & START )
					{
						nRightLength = 0;
						vRightPath.erase( vRightPath.begin() + 1, vRightPath.end() );
						vRightPath.push_back( pl.GetPoint(right_idx) );
						nRightLength += vRightPath[ vRightPath.size() - 2 ].GetDistance( vRightPath[ vRightPath.size() - 1 ] );
					}

					if( bLeftEnd && !vLeftPath.empty() )
					{
						if( nRightLength > nLeftLength * 2 ) 
						{
							vRightPath.clear();
							bRightEnd = true;
							break;
						}
					}

					if( ( vVisibleProperty[ right_idx ] & DEADEND ) && ( vVisibleProperty[ next_right_idx ] & DEADEND ) )
					{
						// we couldn't found a valid path while traveling to the right
						vRightPath.clear();
						bRightEnd = true;
						break;
					}

					break;
				}
				// }
			}

			/*
			NORMALIZE( pl, vLeftPath );	// 임시로 막음. 켜면 품질이좋지만 더 느려짐
			NORMALIZE( pl, vRightPath );	// 임시로 막음. 켜면 품질이좋지만 더 느려짐
			*/

			/*
			if( !vLeftPath.empty() )
			{
				vLeftPath.push_back( end );
				//NORMALIZE( pl, vLeftPath );	// 임시로 막음. 켜면 품질이좋지만 더 느려짐
			} else vLeftPath.clear();
			
			if( !vRightPath.empty() )
			{
				vRightPath.push_back( end );
				//NORMALIZE( pl, vRightPath );	//임시로 막음. 켜면 품질이좋지만 더 느려짐
			} else vRightPath.clear();
			*/

			if( vGreenPath.empty() && vLeftPath.empty() && vRightPath.empty() )
				return;

			// 성능문제가 일어날것 같으면 길찾기 포기
			if( vLeftPath.size() >= MAX_POLYGON_POINT ) 
			{
				//_oprint( "길찾기 에러 : vLeftPath.size() >= MAX_POLYGON_POINT\n" );
				vLeftPath.clear();
			}
			if( vRightPath.size() >= MAX_POLYGON_POINT ) 
			{
				//_oprint( "길찾기 에러 : vRightPath.clear() >= MAX_POLYGON_POINT\n" );
				vRightPath.clear();
			}

			dijkstra( pl, vLeftPath, vRightPath, vGreenPath, start, end, *pvPath );
		}

		struct GRAPH
		{
			GRAPH( size_t nNodeCount )
			{
				m_pGraph = new T[ nNodeCount*nNodeCount ];
				m_nNodeCount = nNodeCount;
				for( size_t idx = 0; idx < nNodeCount*nNodeCount; ++idx ) m_pGraph[idx] = std::numeric_limits<T>::max();
				for( size_t idx = 0; idx < nNodeCount; ++idx ) SetCost( idx, idx, 0 );
			}
			~GRAPH()					{ delete [] m_pGraph; }

			T		GetCost( size_t from, size_t to ) const				{ return m_pGraph[ to * m_nNodeCount + from ]; }
			void	SetCost( size_t from, size_t to, const T & v )		{ assert( to * m_nNodeCount + from < m_nNodeCount*m_nNodeCount ); m_pGraph[ to * m_nNodeCount + from ] = v; }
			bool	IsReachable( size_t from, size_t to ) const			{ return GetCost( from, to ) != std::numeric_limits< T >::max(); }

			T*		m_pGraph;
			size_t	m_nNodeCount;
		};

		static void dijkstra( const NakedPT & pl, const PATH & vLeftPath, const PATH & vRightPath, const PATH & vGreenPath, const Point<T> & start, const Point<T> & end, PATH & vResult )
		{
			std::vector< Point<T> > vPointList;
			size_t					node_count;

			// { 노드 구성
			vPointList.reserve( vLeftPath.size() + vRightPath.size() + vGreenPath.size() );
			vPointList.push_back( start );
			if( !vLeftPath.empty() )	{ vPointList.insert( vPointList.end(), vLeftPath.begin()+1, vLeftPath.end()-1 );    }
			if( !vRightPath.empty() )	{ vPointList.insert( vPointList.end(), vRightPath.begin()+1, vRightPath.end()-1 );  }
			if( !vGreenPath.empty() )	{ vPointList.insert( vPointList.end(), vGreenPath.begin()+1, vGreenPath.end()-1 );  }
			vPointList.push_back( end );
			node_count = vPointList.size();
			// }

			// 시작점, Left List, Right List, Green, end

			GRAPH graph( node_count+2 );
			
			size_t idx;
			size_t nAdd = 1;
			if( !vLeftPath.empty() )
			{
				graph.SetCost( 0, 1, vLeftPath[0].GetDistance( vLeftPath[1] ) );
				for( idx = 0; idx+3 < vLeftPath.size(); ++idx ) 
				{
					graph.SetCost( nAdd+idx, nAdd+idx+1, vLeftPath[idx+1].GetDistance( vLeftPath[idx+1+1] ) );
				}
				graph.SetCost( nAdd+idx, node_count-1, vLeftPath[vLeftPath.size()-2].GetDistance( vLeftPath[vLeftPath.size()-1] ) );
				nAdd += ( vLeftPath.size() - 2 );
			}
			if( !vRightPath.empty() )
			{
				graph.SetCost( 0, nAdd, vRightPath[0].GetDistance( vRightPath[1] ) );
				for( idx = 0; idx+3 < vRightPath.size(); ++idx ) 
				{
					graph.SetCost( nAdd+idx, nAdd+idx+1, vRightPath[idx+1].GetDistance( vRightPath[idx+1+1] ) );
				}
				graph.SetCost( nAdd+idx, node_count-1, vRightPath[vRightPath.size()-2].GetDistance( vRightPath[vRightPath.size()-1] ) );
				nAdd += ( vRightPath.size() - 2 );
			}

			// 왼쪽에서 오른쪽으로 가는 경우
			for( size_t li = 1; li+1 < vLeftPath.size(); ++li )
			{
				for( size_t ri = 1; ri+1 < vRightPath.size(); ++ri )
				{
					size_t from = li;
					size_t tt = vRightPath.size() - ri - 1;
					size_t to = vLeftPath.size() + tt - 2;

					if( !LOOSE_COLLISION( pl, Line<T>( vPointList[from], vPointList[to] ) ) )
					{
						graph.SetCost( from, to, vPointList[from].GetDistance( vPointList[to] ) );
						break;
					}
				}
			}

			// 오른쪽에서 왼쪽으로 가는경우
			for( size_t ri = 1; ri+1 < vRightPath.size(); ++ri )
			{
				for( size_t li = 1; li+1 < vLeftPath.size(); ++li )
				{
					size_t from = vLeftPath.size() + ri - 2;
					size_t tt = vLeftPath.size() - li - 1;
					size_t to = tt;

					if( !LOOSE_COLLISION( pl, Line<T>( vPointList[from], vPointList[to] ) ) )
					{
						graph.SetCost( from, to, vPointList[from].GetDistance( vPointList[to] ) );
						break;
					}
				}
			}

			if( !vGreenPath.empty() )
			{
				graph.SetCost( 0, node_count-2, vGreenPath[0].GetDistance( vGreenPath[1] ) );
				graph.SetCost( node_count-2, node_count-1, vGreenPath[1].GetDistance( vGreenPath[2] ) );
			}

			// 찾자~
			std::vector< size_t > vNodeIndex;
			T length = XDijkstra< T, GRAPH >().Find( graph, vPointList.size(), 0, vPointList.size() - 1, &vNodeIndex );
			
			vResult.reserve( vNodeIndex.size() );
			for( std::vector< size_t >::iterator it = vNodeIndex.begin(); it != vNodeIndex.end(); ++it ) vResult.push_back( vPointList[ *it ] );
		}

		

		struct PolygonDistanceLess
		{
			template< typename PI >
			bool operator()( const PI & lh, const PI & rh )
			{
				T dist_lh = pt.GetAlternativeDistance( (*lh).GetCenter() );
				T dist_rh = pt.GetAlternativeDistance( (*rh).GetCenter() );
				return dist_lh < dist_rh;
			}

			X2D::Point<T> pt;
		};

		template< typename PI >
		static void _Find( PI begin ,PI end, const X2D::Point<T> & start, const X2D::Point<T> & goal, PATH & vPath, const T & max_length )
		{
			std::vector< PT > vPolygonList( begin, end );
			PolygonDistanceLess less;
			less.pt = start;
			std::sort( vPolygonList.begin(), vPolygonList.end(), less );

			for( std::vector< PT >::iterator it = vPolygonList.begin(); it != vPolygonList.end(); ++it  )
			{
				if( X2D::INCLUDE( *it, start ) ) return;
				if( X2D::INCLUDE( *it, goal ) ) return;
			}
			
			vPath.erase( vPath.begin(), vPath.end() );
			vPath.push_back( start );
			vPath.push_back( goal );

			PATH vNewPath;

			if( false )
			{
				getPath( **begin, start, goal, &vPath );
			}
			else
			{		
				for( size_t idx = 0; idx + 1 < vPath.size(); )
				{
					size_t next_idx = idx + 1;
					int nAdd = 1;

					for( PI it = vPolygonList.begin(); it != vPolygonList.end(); ++it  )
					{	
						if( COLLISION( *(*it), start ) ) assert( 0 );
						if( COLLISION( *(*it), goal ) ) assert( 0 );

						if( !LOOSE_COLLISION( *(*it), Line<T>( *(vPath.begin() + idx), *(vPath.begin() + next_idx) ) ) ) continue;

						vNewPath.reserve( (*it)->Size() );
						vNewPath.erase( vNewPath.begin(), vNewPath.end() );
						
						getPath( *(*it), *(vPath.begin() + idx), *(vPath.begin() + next_idx), &vNewPath );

						// debug
						/*
						for( size_t pi = 0; pi < vPath.size() && !vNewPath.empty(); ++pi )
						{
							for( size_t pi2 = 1; pi2 + 1 < vNewPath.size(); ++pi2 )
							{
								if( vPath[pi] == vNewPath[pi2] )
								{
									extern bool g_bMouseMove;
									g_bMouseMove=false;
									vNewPath.clear();
									break;
								}
							}
						}*/

						if( vNewPath.size() > 2 )
						{
							if( std::find( vPath.begin(), vPath.end(), vNewPath[1] ) != vPath.end() ) 
							{
								vPath.clear();
								return;
							}
							vPath.insert( vPath.begin()+idx+1, vNewPath.begin() +1 , vNewPath.end() - 1 );
							nAdd = 0;
						}
						else if( vNewPath.size() == 0 )
						{
							//vPath.erase( vPath.begin(), vPath.end() );
							vPath.clear();
							return;
						}

						if( max_length )
						{
							T total_length = 0;
							for( size_t i = 0; i+1 < vPath.size(); ++i )
							{
								total_length += vPath[i].GetDistance( vPath[i+1] );
								if( total_length > max_length || vPath[i] == vPath[i+1] ) 
								{
									vPath.clear();
									return;
								}
							}
						}

						break;
					}
					idx += nAdd;
				}
			}

			NORMALIZE( begin, end, vPath );

			//return vPath;
		}
	};


}; // namespace X2D
