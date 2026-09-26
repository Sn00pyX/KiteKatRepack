// X2DPolygon.h
//
//  by Testors , 2005/08/01

#pragma once

#include <vector>
#include <algorithm>
#include <limits>
#include <cassert>

#include "X2DBasicTypes.h"

namespace X2D
{

	template< typename T = int, template< class > class PointType = Point >
	struct Polygon
	{
		typedef PointType< T >	Point;
		typedef Line< T >	Line;
		typedef Box< T >	Box;
		typedef Rect< T >	Rect;

		Polygon()
		{
			Clear();
		}

		virtual ~Polygon()
		{
		}

		Polygon( const Box & rh )
		{
			Point pt[4];
			pt[0].Set( rh.GetLeft(), rh.GetTop() );
			pt[1].Set( rh.GetRight(), rh.GetTop() );
			pt[2].Set( rh.GetRight(), rh.GetBottom() );
			pt[3].Set( rh.GetLeft(), rh.GetBottom() );
			Set( &pt[0], &pt[4] );
		}

		Polygon( const Polygon & rh )
		{
			Set( rh.m_vList.begin(), rh.m_vList.end() );
		}

		template< typename POINT_ITERATOR >
		Polygon( POINT_ITERATOR begin, POINT_ITERATOR end )
		{
			Set( begin, end );
		}

		template< typename POINT_ITERATOR >
		bool Set( POINT_ITERATOR begin, POINT_ITERATOR end, bool bCheckValidity = 
#ifdef _DEBUG
			false
#else
			//true
			false
#endif
			)
		{
			Clear();

			m_vList.reserve( end - begin );

			// 점 생성
			m_vList.assign( begin, end );

			RemoveDuplicatedPoint();

			m_bIsValid = ( bCheckValidity ) ? isValid( m_vList ) : true;

			if( m_bIsValid ) 
			{
				// 시계방향인지 여부 계산
				m_bIsClockWise = isClockWise();

				// 영역 설정
				calculateArea( m_vList, m_bxArea );
			}
			else
			{
				Clear();
			}

			return m_bIsValid;
		}

		/*
		void operator=( const Polygon & rh )
		{
			Set( rh.m_vList.begin(), rh.m_vList.end() );
		}*/

		bool Scale( float fScale, bool bAlignCenter = false )
		{
			std::vector< Point > tmpList = m_vList;
			for( std::vector< Point >::iterator it = tmpList.begin(); it != tmpList.end(); ++it )
			{
				(*it).x *= fScale;
				(*it).y *= fScale;
			}
			if( !isValid( tmpList ) ) return false;

			T x_offset;
			T y_offset;

			Box bxArea;
			calculateArea( tmpList, bxArea );

			if( bAlignCenter )
			{
				x_offset = bxArea.GetCenter().x - m_bxArea.GetCenter().x;
				y_offset = bxArea.GetCenter().y - m_bxArea.GetCenter().y;
			}
			else
			{
				x_offset = bxArea.GetLeft() - m_bxArea.GetLeft();
				y_offset = bxArea.GetTop() - m_bxArea.GetTop();
			}

			for( std::vector< Point >::iterator it = tmpList.begin(); it != tmpList.end(); ++it )
			{
				(*it).x -= x_offset;
				(*it).y -= y_offset;
			}

			Set( tmpList.begin(), tmpList.end() );

			return true;
		}

		void Clear()
		{	
			m_vList.erase( m_vList.begin(), m_vList.end() );
			m_bxArea.Set( 0, 0, 0, 0 );
			m_bIsClockWise = false;
			m_bIsValid = false;
		}

		void Reverse()
		{
			if( m_vList.empty() ) return;
			std::reverse( m_vList.begin(), m_vList.end() );
			m_bIsClockWise = !m_bIsClockWise;
		}

		bool IsClockWise() const	{ return m_bIsClockWise; }

		void MakeToClockwise()
		{
			if( !IsClockWise() ) Reverse();
		}

		template< typename GT >
		bool IsIn( const GT & t ) const
		{
			std::vector< Point >::const_iterator it;
			for( it = m_vList.begin(); it != m_vList.end(); ++it )
			{
				if( !t.IsInclude( *it ) )
					return false;
			}

			return true;
		}

		template< typename GT >
		bool IsLooseIn( const GT & t ) const
		{
			std::vector< Point >::const_iterator it;
			for( it = m_vList.begin(); it != m_vList.end(); ++it )
			{
				if( !t.IsLooseInclude( *it ) )
					return false;
			}

			return true;
		}

		template< typename GT >
		bool IsCloselyInclude( const GT & t ) const
		{
			std::vector< Point >::const_iterator it;
			for( it = m_vList.begin(); it != m_vList.end(); ++it )
			{
				if( !t.IsCloselyInclude( *it ) )
					return false;
			}

			return true;
		}

		bool Has( const Point & pt ) const
		{
			for( std::vector< Point >::const_iterator it = m_vList.begin(); it != m_vList.end(); ++it ) if( *it == pt ) return true;
			return false;
		}

		bool Has( const Line & line ) const
		{
			for( size_t idx = 0; idx < m_vList.size(); ++idx )
			{
				if( GetSegment( idx ) == line ) return true;
			}
			return false;
		}

		const Box & GetBoundingBox() const { return m_bxArea; }

		// 기본 컨셉은 폴리곤 바깥의 임의의 점과 pt 두개로 구성된 선분이 폴리곤을 이루는 선분들과 
		// 몇개나 충돌하는지를 검사하고, 홀수이면 포함, 짝수이면 포함이 아니라고 판정하는 것이다.
		bool IsInclude( const Point & pt ) const
		{
			assert( IsValid() );

			if( !m_bxArea.IsInclude( pt ) ) return false;

			// 폴리곤 바깥의 한 점
			Point farAway( m_bxArea.GetRight(), pt.y );
			farAway.x++;

			size_t nTouchCnt = 0;
			size_t nIntersectCnt = 0;

			for( size_t idx = 0; idx < m_vList.size(); ++idx )
			{	
				if( pt == m_vList[idx] ) return true;

				const Point & pt1 = m_vList[idx];
				const Point & pt2 = m_vList[getNextIndex(idx)];

				Line::INTERSECT_RESULT result = Line::IntersectCCW( pt1, pt2, farAway, pt );

				// 교차의 경우
				if( result == Line::INTERSECT )	nIntersectCnt++;

				// 선분의 끝점이 다른 선분에 걸친 경우
				if( result == Line::TOUCH )
				{
					if( pt.y == std::min( pt1.y, pt2.y ) && pt1.y != pt2.y ) nTouchCnt++;
				}
			}

			return ( ( nTouchCnt + nIntersectCnt ) % 2 == 1 );
		}

		bool IsInclude( const T & x, const T & y ) const	{ return IsInclude( Point( x, y ) ); }

		bool IsLooseInclude( const Point & pt ) const
		{
			assert( IsValid() );

			if( !m_bxArea.IsInclude( pt ) ) return false;

			// 폴리곤 바깥의 한 점
			Point farAway( m_bxArea.GetRight(), pt.y );
			farAway.x++;

			size_t nIntersectCnt = 0;
			size_t nTouchCnt = 0;

			for( size_t idx = 0; idx < m_vList.size(); ++idx )
			{
				if( pt == m_vList[idx] ) return false;

				const Point & pt1 = m_vList[idx];
				const Point & pt2 = m_vList[getNextIndex(idx)];

				Line::INTERSECT_RESULT result = Line::IntersectCCW( pt1, pt2, farAway, pt );

				// 교차의 경우
				if( result == Line::INTERSECT )	nIntersectCnt++;

				// 선분의 끝점이 다른 선분에 걸친 경우
				if( result == Line::TOUCH )
				{
					if( pt.y == std::min( pt1.y, pt2.y ) && pt1.y != pt2.y ) nTouchCnt++;
				}
			}

			return ( ( nIntersectCnt + nTouchCnt ) % 2 == 1 );
		}

		bool IsLooseInclude( const T & x, const T & y ) const	{ return IsLooseInclude( Point( x, y ) ); }

		bool IsValid() const { return m_bIsValid; }

		Line GetSegment( size_t idx ) const
		{
			assert( idx < m_vList.size() );

			return Line( m_vList[idx], m_vList[getNextIndex(idx)] );
		}

		bool operator==( const Polygon & rh ) const
		{
			assert( IsValid() && rh.IsValid() );

			if( Size() != rh.Size() ) return false;
			
			int    begin = 0;
			int    add = 1;
			if( IsClockWise() != rh.IsClockWise() )
			{
				begin = (int)Size() - 1;
				add   = -1;
			}

			size_t mod = 0;
			std::vector< Point >::const_iterator it;
			for( it = rh.m_vList.begin(); it != rh.m_vList.end(); ++it, ++mod )
			{
				if( m_vList[begin] == *it ) break;
			}

			// 똑같은 점이 하나도 없으면..
			if( it == rh.m_vList.end() ) return false;
			
			int rh_idx = 0;
			for( int idx = begin; idx >= 0 && idx < (int)Size(); idx += add, ++rh_idx )
			{
				if( rh.m_vList[ ( rh_idx + mod ) % rh.Size() ] != m_vList[ idx ] ) return false;
			}

			return true;
		}

		bool IsInclude( const Polygon & rh ) const
		{
			if( !m_bxArea.IsInclude( rh.m_bxArea ) ) return false;

			std::vector< Point >::const_iterator it;
			for( it = rh.m_vList.begin(); it != rh.m_vList.end(); ++it ) if( !IsInclude( *it ) ) return false;

			// 선분끼리 충돌하면 충돌임
			for( size_t my_idx = 0; my_idx < Size(); ++my_idx )
			{
				for( size_t rh_idx = 0; rh_idx < rh.Size(); ++rh_idx )
				{
					if( GetSegment( my_idx ).IntersectCCW( rh.GetSegment( rh_idx ) ) != Line::SEPARATE ) return false;
				}
			}

			return true;
		}

		bool IsCollision( const Point & rh ) const	{ return IsInclude( rh ); }
		bool IsCollision( const Box & rh ) const
		{
			Polygon temp( rh );
			return IsCollision( temp );
		}
		bool IsCollision( const Rect & rc ) const
		{
			// Rect 의 왼쪽 끝점이 폴리곤 안에 있으면 충돌
			if( IsInclude( Point( rc.GetLeft(), rc.GetTop() ) ) ) return true;

			// 서로 한 점이라도 포함하면 충돌
			for( std::vector< Point >::const_iterator it = m_vList.begin(); it != m_vList.end(); ++it )
			{
				if( INCLUDE( rc, *it ) ) return true;
			}

			X2D::Line< T > line_a( rc.GetLeft(), rc.GetTop(),   rc.GetRight(),rc.GetTop() );
			X2D::Line< T > line_b( rc.GetRight(),rc.GetTop(),   rc.GetRight(),rc.GetBottom() );
			X2D::Line< T > line_c( rc.GetRight(),rc.GetBottom(),rc.GetLeft(), rc.GetBottom() );
			X2D::Line< T > line_d( rc.GetLeft(), rc.GetBottom(),rc.GetLeft(), rc.GetTop() );

			for( size_t my_idx = 0; my_idx < Size(); ++my_idx )
			{
				X2D::Line< T > line = GetSegment( my_idx );	

				X2D::Line< T >::INTERSECT_RESULT result_a, result_b, result_c, result_d;

				// 선분끼리 교차하면 충돌
				if( ( result_a = line.IntersectCCW( line_a ) ) == Line::INTERSECT ) return true;
				if( ( result_b = line.IntersectCCW( line_b ) ) == Line::INTERSECT ) return true;
				if( ( result_c = line.IntersectCCW( line_c ) ) == Line::INTERSECT ) return true;
				if( ( result_d = line.IntersectCCW( line_d ) ) == Line::INTERSECT ) return true;

				if( line.begin.x != line.end.x && line.begin.y != line.end.y )
				{
					if( result_b == Line::TOUCH && result_c == Line::TOUCH ) return true;
				}
			}

			return false;
		}

		bool IsCollision( const X2D::Line<T> & line ) const
		{
			if( IsInclude( line.begin ) || IsInclude( line.end ) ) return true;

			for( size_t my_idx = 0; my_idx < Size(); ++my_idx )
			{
				if( GetSegment( my_idx ).IntersectCCW( line ) != Line::SEPARATE ) return true;
			}

			return false;
		}

		bool IsCollision( const Polygon & rh ) const
		{
			if( !m_bxArea.IsCollision( rh.m_bxArea ) ) return false;

			// 서로가 한점이라도 포함하고 있으면 충돌임.
			std::vector< Point >::const_iterator it;
			for( it = m_vList.begin(); it != m_vList.end(); ++it ) if( rh.IsInclude( *it ) ) return true;
			for( it = rh.m_vList.begin(); it != rh.m_vList.end(); ++it ) if( IsInclude( *it ) ) return true;

			// 선분끼리 충돌하면 충돌임
			for( size_t my_idx = 0; my_idx < Size(); ++my_idx )
			{
				for( size_t rh_idx = 0; rh_idx < rh.Size(); ++rh_idx )
				{
					if( GetSegment( my_idx ).IntersectCCW( rh.GetSegment( rh_idx ) ) != Line::SEPARATE ) return true;
				}
			}

			return false;
		}

		bool IsLooseCollision( const Point & rh ) const	{ return IsLooseInclude( rh ); }
		bool IsLooseCollision( const Box & rh ) const
		{
			Polygon temp( rh );
			return IsLooseCollision( temp );
		}

		bool IsLooseCollision( const Rect & rc ) const
		{
			// Rect 의 왼쪽 끝점이 폴리곤 안에 있으면 충돌
			if( IsLooseInclude( Point( rc.GetLeft(), rc.GetTop() ) ) ) return true;

			// 서로 한 점이라도 포함하면 충돌
			for( std::vector< Point >::const_iterator it = m_vList.begin(); it != m_vList.end(); ++it )
			{
				if( LOOSE_INCLUDE( rc, *it ) ) return true;
			}

			X2D::Line< T > line_a( rc.GetLeft(), rc.GetTop(),   rc.GetRight(),rc.GetTop() );
			X2D::Line< T > line_b( rc.GetRight(),rc.GetTop(),   rc.GetRight(),rc.GetBottom() );
			X2D::Line< T > line_c( rc.GetRight(),rc.GetBottom(),rc.GetLeft(), rc.GetBottom() );
			X2D::Line< T > line_d( rc.GetLeft(), rc.GetBottom(),rc.GetLeft(), rc.GetTop() );

			for( size_t my_idx = 0; my_idx < Size(); ++my_idx )
			{
				X2D::Line< T > line = GetSegment( my_idx );	

				X2D::Line< T >::INTERSECT_RESULT result_a, result_b, result_c, result_d;

				// 선분끼리 교차하면 충돌
				if( ( result_a = line.IntersectCCW( line_a ) ) == Line::INTERSECT ) return true;
				if( ( result_b = line.IntersectCCW( line_b ) ) == Line::INTERSECT ) return true;
				if( ( result_c = line.IntersectCCW( line_c ) ) == Line::INTERSECT ) return true;
				if( ( result_d = line.IntersectCCW( line_d ) ) == Line::INTERSECT ) return true;
			}

			return false;
		}

		bool IsLooseCollision( const X2D::Line<T> & line ) const
		{
			if( IsLooseInclude( line.begin ) || IsLooseInclude( line.end ) ) return true;

			for( size_t my_idx = 0; my_idx < Size(); ++my_idx )
			{
				X2D::Line<T>::INTERSECT_RESULT result = GetSegment( my_idx ).IntersectCCW( line );
				if( result != Line::SEPARATE && result != Line::TOUCH ) return true;
			}

			return false;
		}

		bool IsLooseCollision( const Polygon & rh ) const
		{
			if( !m_bxArea.IsLooseCollision( rh.m_bxArea ) ) return false;

			// 서로가 한점이라도 포함하고 있으면 충돌임.
			std::vector< Point >::const_iterator it;
			for( it = m_vList.begin(); it != m_vList.end(); ++it ) if( rh.IsLooseInclude( *it ) ) return true;
			for( it = rh.m_vList.begin(); it != rh.m_vList.end(); ++it ) if( IsLooseInclude( *it ) ) return true;

			// 선분끼리 충돌하면 충돌임
			for( size_t my_idx = 0; my_idx < Size(); ++my_idx )
			{
				for( size_t rh_idx = 0; rh_idx < rh.Size(); ++rh_idx )
				{
					X2D::Line<T>::INTERSECT_RESULT result = GetSegment( my_idx ).IntersectCCW( rh.GetSegment( rh_idx ) );
					if( result != Line::SEPARATE && result != Line::TOUCH ) return true;
				}
			}

			return false;
		}

		void RemoveDuplicatedPoint()
		{
			if( !IsValid() ) return;

			Point prevPt								= m_vList.back();
			std::vector< Point >::iterator new_it		= m_vList.begin();
			std::vector< Point >::iterator target_it	= m_vList.begin();

			for( std::vector< Point >::iterator target_it = m_vList.begin(); target_it != m_vList.end(); ++target_it )
			{
				if( target_it == m_vList.begin() ) 
				{
					if( prevPt != *new_it ) new_it++;
					continue;
				}

				if( prevPt == *target_it ) continue;

				*new_it = *target_it; 
				prevPt = *target_it;
				new_it++;
			}

			m_vList.resize( new_it - m_vList.begin() );

			size_t cnt = m_vList.size();

			// 폴리곤이 아니라면..
			if( cnt < 3 ) Clear();
		}

		size_t Size() const { return m_vList.size(); }
		const Point & GetPoint( size_t idx ) const	{ return m_vList[idx];	}
		const Point & GetPrevPoint( size_t idx ) const	{ return GetPoint( getPrevIndex( idx ) ); }
		const Point & GetNextPoint( size_t idx ) const	{ return GetPoint( getNextIndex( idx ) ); }
		const Point * GetRawPoint() const			{ return &m_vList[0];	}
		Point		  GetCenter() const
		{
			return m_bxArea.GetCenter();
		}

		const T GetTop() const		{ return m_bxArea.GetTop();		}
		const T GetBottom() const	{ return m_bxArea.GetBottom();	}
		const T GetLeft() const		{ return m_bxArea.GetLeft();	}
		const T GetRight() const	{ return m_bxArea.GetRight();	}

		size_t getPrevIndex( size_t idx ) const { return idx == 0 ? m_vList.size() - 1 : idx - 1; }
		size_t getNextIndex( size_t idx ) const { return idx == m_vList.size() - 1 ? 0 : idx + 1; }

	protected:

		bool isValid( const std::vector< Point > & vList ) const
		{
			if( vList.size() < 3 ) return false;
			
			// 동일한 점이 있는지 검사
			std::vector< Point >::const_iterator cur_it		= vList.begin();
			std::vector< Point >::const_iterator target_it	= vList.begin();
			for( cur_it = vList.begin(); cur_it != vList.end(); ++cur_it )
			{
				for( target_it = cur_it + 1; target_it != vList.end(); ++target_it )
				{
					if( (*target_it) == (*cur_it) ) return false;
				}
			}

			// 선분이 서로 충돌하는지 검사
			for( size_t idx_1 = 0; idx_1 < vList.size(); ++idx_1 )
			{
				for( size_t idx_2 = idx_1 + 1; idx_2 < vList.size(); ++idx_2 )
				{
					size_t p1 = idx_1;
					size_t p2 = getNextIndex(idx_1);
					size_t p3 = idx_2;
					size_t p4 = getNextIndex(idx_2);

					Line::INTERSECT_RESULT nResult = Line::IntersectCCW( vList[p1], vList[p2], vList[p3], vList[p4] );

					if( nResult == Line::INTERSECT ) return false;
					if( nResult == Line::TOUCH && p2 != p3 && p1 != p4 ) return false;
				}			
			}

			return true;
		}

		bool isClockWise() const
		{
			// x 가 가장 작은 점을 찾는다		
			size_t mid_idx = 0;
			for( size_t i = 0; i < m_vList.size(); ++i ) 
			{
				if( m_vList[ mid_idx ].x > m_vList[i].x ) mid_idx = i;
			}

			int nCCWResult = X2D::CLOCK_WISE;

			size_t cnt = 0;
			while( true )
			{
				size_t prev_idx = getPrevIndex( mid_idx );
				size_t next_idx = getNextIndex( mid_idx );

				nCCWResult = X2D::CheckClockWise(	m_vList[prev_idx].x,	m_vList[prev_idx].y, 
													m_vList[mid_idx].x,		m_vList[mid_idx].y,
													m_vList[next_idx].x,	m_vList[next_idx].y );

				if( nCCWResult != PARALLELISM ) break;

				assert( mid_idx < m_vList.size() );

				++mid_idx;
				++cnt;
				if( cnt > m_vList.size() ) break;

				if( mid_idx == m_vList.size() ) mid_idx = 0;
			}

			return ( nCCWResult == CLOCK_WISE );
		}

		void loop()
		{
			for( size_t idx = 0; idx < m_vList.size(); ++idx )
			{
				// do( idx, getNextIndex( idx ) );
			}
		}

		void calculateArea( const std::vector< Point > & vList, Box & area )
		{
			std::vector< Point >::const_iterator it;

			area.Set( vList.front(), vList.front() );
			for( it = vList.begin(); it != vList.end(); ++it )
			{
				if( (*it).x < area.GetLeft()	) area.SetLeft( (*it).x );
				if( (*it).y < area.GetTop()		) area.SetTop( (*it).y );
				if( (*it).x > area.GetRight()	) area.SetRight( (*it).x );			
				if( (*it).y > area.GetBottom()	) area.SetBottom( (*it).y );
			}
		}

		bool						m_bIsValid;
		bool						m_bIsClockWise;		// 점 순서가 시계방향인지 여부
		std::vector< Point >		m_vList;
		Box							m_bxArea;			// 포함체크등에서 박스체크를 선행해서 속도 이득을 볼 수 있는 경우가 많으므로 미리 계산해 놓는다.
	};

	template< typename T > inline bool INCLUDE(			const Polygon<T> & lh, const Point<T>   & rh )	{ return lh.IsInclude( rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> & lh, const Box<T>     & rh )	{ return lh.IsInclude( rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> & lh, const Rect<T>    & rh )	{ return lh.IsInclude( rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> & lh, const Line<T>    & rh )	{ return lh.IsInclude( rh.begin ) && lh.IsInclude( rh.end );	}
	template< typename T > inline bool INCLUDE(			const Line<T>    & lh, const Polygon<T> & rh )	{ return false;							}
	template< typename T > inline bool INCLUDE(			const Rect<T>    & lh, const Polygon<T> & rh )	{ return rh.IsIn( lh );					}
	template< typename T > inline bool INCLUDE(			const Box<T>     & lh, const Polygon<T> & rh )	{ return rh.IsIn( lh );					}
	template< typename T > inline bool INCLUDE(			const Polygon<T> & lh, const Polygon<T> & rh )	{ return lh.IsInclude( rh );			}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> & lh, const Point<T>   & rh )	{ return lh.IsLooseInclude( rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> & lh, const Box<T>     & rh )	{ return lh.IsLooseInclude( rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> & lh, const Rect<T>    & rh )	{ return lh.IsLooseInclude( rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> & lh, const Line<T>    & rh )	{ return lh.IsLooseInclude( rh.begin ) && lh.IsLooseInclude( rh.end );	}
	template< typename T > inline bool LOOSE_INCLUDE(	const Line<T>    & lh, const Polygon<T> & rh )	{ return false;							}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>    & lh, const Polygon<T> & rh )	{ return rh.IsLooseIn( lh );			}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>     & lh, const Polygon<T> & rh )	{ return rh.IsLooseIn( lh );			}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> & lh, const Polygon<T> & rh )	{ return lh.IsLooseInclude( rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> & lh, const Point<T>   & rh )	{ return lh.IsCloselyInclude( rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> & lh, const Box<T>     & rh )	{ return lh.IsCloselyInclude( rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> & lh, const Rect<T>    & rh )	{ return lh.IsCloselyInclude( rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> & lh, const Line<T>    & rh )	{ return lh.IsCloselyInclude( rh.begin ) && lh.IsCloselyInclude( rh.end );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Line<T>    & lh, const Polygon<T> & rh )	{ return false;							}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Rect<T>    & lh, const Polygon<T> & rh )	{ return rh.IsCloselyInclude( lh );			}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Box<T>     & lh, const Polygon<T> & rh )	{ return rh.IsCloselyInclude( lh );			}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> & lh, const Polygon<T> & rh )	{ return lh.IsCloselyInclude( rh );		}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Point<T>   & rh )	{ return lh.IsCollision( rh );			}
	template< typename T > inline bool COLLISION(		const Point<T>   & lh, const Polygon<T> & rh )	{ return rh.IsInclude( lh);				}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Box<T>     & rh )	{ return lh.IsCollision( rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Rect<T>    & rh )	{ return lh.IsCollision( rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Line<T>    & rh )	{ return lh.IsCollision( rh );			}
	template< typename T > inline bool COLLISION(		const Line<T>    & lh, const Polygon<T> & rh )	{ return rh.IsCollision( lh );			}
	template< typename T > inline bool COLLISION(		const Box<T>     & lh, const Polygon<T> & rh )	{ return COLLISION( rh, lh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>    & lh, const Polygon<T> & rh )	{ return COLLISION( rh, lh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Polygon<T> & rh )	{ return lh.IsCollision( rh );			}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> & lh, const Point<T>   & rh )	{ return lh.IsLooseCollision( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T>   & lh, const Polygon<T> & rh )	{ return rh.IsLooseInclude( lh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> & lh, const Box<T>     & rh )	{ return lh.IsLooseCollision( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> & lh, const Rect<T>    & rh )	{ return lh.IsLooseCollision( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> & lh, const Line<T>    & rh )	{ return lh.IsLooseCollision( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Line<T>    & lh, const Polygon<T> & rh )	{ return rh.IsLooseCollision( lh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>     & lh, const Polygon<T> & rh )	{ return LOOSE_COLLISION( rh, lh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>    & lh, const Polygon<T> & rh )	{ return LOOSE_COLLISION( rh, lh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> & lh, const Polygon<T> & rh )	{ return lh.IsLooseCollision( rh );		}

	template< typename T > inline bool INCLUDE(			const Polygon<T> * lh, const Point<T>   & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> * lh, const Box<T>     & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> * lh, const Rect<T>    & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(			const Rect<T>    * lh, const Polygon<T> & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(			const Box<T>     * lh, const Polygon<T> & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> * lh, const Polygon<T> & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> * lh, const Point<T>   & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> * lh, const Box<T>     & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> * lh, const Rect<T>    & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>    * lh, const Polygon<T> & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>     * lh, const Polygon<T> & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> * lh, const Polygon<T> & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> * lh, const Point<T>   & rh )	{ return CLOSELY_INCLUDE( *lh, rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> * lh, const Box<T>     & rh )	{ return CLOSELY_INCLUDE( *lh, rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> * lh, const Rect<T>    & rh )	{ return CLOSELY_INCLUDE( *lh, rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Rect<T>    * lh, const Polygon<T> & rh )	{ return CLOSELY_INCLUDE( *lh, rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Box<T>     * lh, const Polygon<T> & rh )	{ return CLOSELY_INCLUDE( *lh, rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> * lh, const Polygon<T> & rh )	{ return CLOSELY_INCLUDE( *lh, rh );	}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Point<T>   & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION(		const Point<T>   * lh, const Polygon<T> & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Box<T>     & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Rect<T>    & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Line<T>    & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION(		const Box<T>     * lh, const Polygon<T> & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>    * lh, const Polygon<T> & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION(		const Line<T>    * lh, const Polygon<T> & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Polygon<T> & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Point<T>   & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T>   * lh, const Polygon<T> & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Box<T>     & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Rect<T>    & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Line<T>    & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>     * lh, const Polygon<T> & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>    * lh, const Polygon<T> & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Line<T>    * lh, const Polygon<T> & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Polygon<T> & rh )	{ return LOOSE_COLLISION( *lh, rh );	}

	template< typename T > inline bool INCLUDE(			const Polygon<T> & lh, const Point<T>   * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> & lh, const Box<T>     * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> & lh, const Rect<T>    * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Rect<T>    & lh, const Polygon<T> * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Box<T>     & lh, const Polygon<T> * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> & lh, const Polygon<T> * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> & lh, const Point<T>   * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> & lh, const Box<T>     * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> & lh, const Rect<T>    * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>    & lh, const Polygon<T> * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>     & lh, const Polygon<T> * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> & lh, const Polygon<T> * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> & lh, const Point<T>   * rh )	{ return CLOSELY_INCLUDE( lh, *rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> & lh, const Box<T>     * rh )	{ return CLOSELY_INCLUDE( lh, *rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> & lh, const Rect<T>    * rh )	{ return CLOSELY_INCLUDE( lh, *rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Rect<T>    & lh, const Polygon<T> * rh )	{ return CLOSELY_INCLUDE( lh, *rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Box<T>     & lh, const Polygon<T> * rh )	{ return CLOSELY_INCLUDE( lh, *rh );	}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> & lh, const Polygon<T> * rh )	{ return CLOSELY_INCLUDE( lh, *rh );	}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Point<T>   * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Point<T>   & lh, const Polygon<T> * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Box<T>     * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Rect<T>    * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Line<T>    * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Box<T>     & lh, const Polygon<T> * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>    & lh, const Polygon<T> * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Line<T>    & lh, const Polygon<T> * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> & lh, const Polygon<T> * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool LOOSE_COLLISION( const Polygon<T> & lh, const Point<T>   * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION( const Point<T>   & lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION( const Polygon<T> & lh, const Box<T>     * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION( const Polygon<T> & lh, const Rect<T>    * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION( const Polygon<T> & lh, const Line<T>    * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION( const Box<T>     & lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION( const Rect<T>    & lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION( const Line<T>    & lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION( const Polygon<T> & lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( lh, *rh );	}

	template< typename T > inline bool INCLUDE(			const Polygon<T> * lh, const Point<T>   * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> * lh, const Box<T>     * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> * lh, const Rect<T>    * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Rect<T>    * lh, const Polygon<T> * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Box<T>     * lh, const Polygon<T> * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Polygon<T> * lh, const Polygon<T> * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> * lh, const Point<T>   * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> * lh, const Box<T>     * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> * lh, const Rect<T>    * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>    * lh, const Polygon<T> * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>     * lh, const Polygon<T> * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Polygon<T> * lh, const Polygon<T> * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> * lh, const Point<T>   * rh )	{ return CLOSELY_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> * lh, const Box<T>     * rh )	{ return CLOSELY_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> * lh, const Rect<T>    * rh )	{ return CLOSELY_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Rect<T>    * lh, const Polygon<T> * rh )	{ return CLOSELY_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Box<T>     * lh, const Polygon<T> * rh )	{ return CLOSELY_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool CLOSELY_INCLUDE(	const Polygon<T> * lh, const Polygon<T> * rh )	{ return CLOSELY_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Point<T>   * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Point<T>   * lh, const Polygon<T> * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Box<T>     * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Rect<T>    * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Line<T>    * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Box<T>     * lh, const Polygon<T> * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>    * lh, const Polygon<T> * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Line<T>    * lh, const Polygon<T> * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Polygon<T> * lh, const Polygon<T> * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Point<T>   * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T>   * lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Box<T>     * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Rect<T>    * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Line<T>    * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>     * lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>    * lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Line<T>    * lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Polygon<T> * lh, const Polygon<T> * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}


};	// namespace X2D
