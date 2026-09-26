// X2DLeakyPolygon.h
//
//  by Testors , 2005/08/01

#pragma once

#include <vector>
#include <algorithm>
#include <limits>
#include <assert.h>

#include "X2DBasicTypes.h"


namespace X2D
{

	template< typename T = int >
	struct IndexedPoint
	{
		typedef Point< T >			point_t;
		typedef Polygon< T >		owner_t;
		typedef IndexedPoint< T >	this_t;

		struct InputQuadTree {
			X2D::Line< T > operator()( X2D::IndexedPoint< T >* p ) {
				return X2D::Line< T >( p->getPoint(), p->getNextPoint() );
			}
		};

		IndexedPoint() {}
		IndexedPoint( /*const T& _x, const T& _y*/owner_t* _owner, int _index ) 
			: /*Point( _x, _y )
			,*/ owner( _owner )
			, index( _index )
		{
		}
		operator const point_t() const 
		{ 
			return getPoint(); 
		}
		operator point_t()
		{
			return const_cast< point_t& >( getPoint() );
		}
		bool operator ==( const this_t& rhs ) const
		{
			return ( owner == rhs.owner && index == rhs.index );
		}
		bool operator !=( const this_t& rhs ) const
		{
			return ( owner != rhs.owner || index != rhs.index );
		}
		bool operator <( const this_t& rhs ) const
		{
			if( owner < rhs.owner || ( owner == rhs.owner && index < rhs.index ) )
				return true;
			return false;
		}
		const point_t& getPoint() const
		{
			return owner->GetPoint( index );
		}
		size_t getNextIndex() const
		{
			return owner->getNextIndex( index );
		}
		size_t getPrevIndex() const
		{
			return owner->getPrevIndex( index );
		}
		const point_t& getNextPoint() const 
		{ 
			return owner->GetPoint( getNextIndex() );
		}
		const point_t& getPrevPoint() const
		{
			return owner->GetPoint( getPrevIndex() );
		}
		bool isPrevLinked( const this_t& rhs ) const
		{
			if( owner == rhs.owner && owner->getPrevIndex( index ) == rhs.index )
				return true;
			return false;
		}
		bool hasSameOwner( const this_t& rhs ) const
		{
			if( owner == rhs.owner )
				return true;
			return false;
		}
		owner_t*	owner;
		int			index;

		struct Less
		{
			bool operator()( const this_t* lhs, const this_t* rhs ) const
			{
				return *lhs < *rhs;
			}
		};
	};

	template< typename T = int >
	struct LeakyPolygon
	{
		typedef IndexedPoint< T >	Point;
		typedef Line< T >			Line;
		typedef Box< T >			Box;
		typedef Rect< T >			Rect;
				
		LeakyPolygon()
		{
			Clear();
		}

		virtual ~LeakyPolygon()
		{
		}

		LeakyPolygon( const Box & rh )
		{
			Point pt[4];
			pt[0].Set( rh.GetLeft(), rh.GetTop() );
			pt[1].Set( rh.GetRight(), rh.GetTop() );
			pt[2].Set( rh.GetRight(), rh.GetBottom() );
			pt[3].Set( rh.GetLeft(), rh.GetBottom() );
			Set( &pt[0], &pt[4] );
		}

		LeakyPolygon( const LeakyPolygon & rh )
		{
			Set( rh.m_vList.begin(), rh.m_vList.end() );
		}

		template< typename POINT_ITERATOR >
		LeakyPolygon( POINT_ITERATOR begin, POINT_ITERATOR end )
		{
			Set( begin, end );
		}

		template< typename POINT_ITERATOR >
		bool Set( POINT_ITERATOR begin, POINT_ITERATOR end )
		{
			Clear();

			m_vList.reserve( end - begin );

			// 점 생성
			m_vList.assign( begin, end );

			RemoveDuplicatedPoint();

			m_bIsValid = isValid( m_vList );

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
		void operator=( const LeakyPolygon & rh )
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
				if( !t.IsInclude( *it ) ) return false;
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

		const Box & GetBoundingBox() const 
		{ 
			if( m_vList.empty() )
				return m_bxArea;
			return m_vList[ 0 ].owner->GetBoundingBox();
		}

		/** 기본 컨셉은 폴리곤 바깥의 임의의 점과 pt 두개로 구성된 선분이 폴리곤을 이루는 선분들과 
			몇개나 충돌하는지를 검사하고, 홀수이면 포함, 짝수이면 포함이 아니라고 판정하는 것이다. */
		bool IsInclude( const X2D::Point< T > & pt ) const
		{
			if( m_vList.empty() ) return false;
			return m_vList[ 0 ].owner->IsInclude( pt );
		}

		bool IsInclude( const T & x, const T & y ) const	{ return IsInclude( X2D::Point< T >( x, y ) ); }

		bool IsValid() const { return m_bIsValid; }

		Line GetSegment( size_t idx ) const
		{
			assert( idx < m_vList.size() );

			return Line( m_vList[idx], GetNextPoint( idx ) );
		}

		bool operator==( const LeakyPolygon & rh ) const
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

		bool IsInclude( const LeakyPolygon & rh ) const
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

		bool IsCollision( const X2D::Point< T > & rh ) const	{ return IsInclude( rh ); }
		bool IsCollision( const Box & rh ) const
		{
			LeakyPolygon temp( rh );
			return IsCollision( temp );
		}
		bool IsCollision( const Rect & rc ) const
		{
			// Rect 의 왼쪽 끝점이 폴리곤 안에 있으면 충돌
			if( IsInclude( X2D::Point< T >( rc.GetLeft(), rc.GetTop() ) ) ) return true;

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

		bool IsCollision( const LeakyPolygon & rh ) const
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
		}

		size_t Size() const { return m_vList.size(); }
		const Point &			GetIndxPoint( size_t idx ) const	{ return m_vList[idx]; }
		const X2D::Point< T > & GetPoint( size_t idx ) const		{ return m_vList[idx].getPoint(); }
		const X2D::Point< T > & GetPrevPoint( size_t idx ) const	{ return m_vList[idx].getPrevPoint(); }
		const X2D::Point< T > & GetNextPoint( size_t idx ) const	{ return m_vList[idx].getNextPoint(); }
		const Point * GetRawPoint() const							{ return &m_vList[0]; }
		X2D::Point< T > GetCenter() const
		{
			return m_bxArea.GetCenter();
		}
		bool IsDeadEnd( size_t idx ) const				
		{ 
			if( idx == 0 )
				return GetPrevPoint( idx ) != GetPoint( Size() - 1 );
			else if( idx == ( Size() - 1 ) )
				return GetNextPoint( idx ) != GetPoint( 0 );
			return false;
		}

		const T GetTop() const		{ return m_bxArea.GetTop();		}
		const T GetBottom() const	{ return m_bxArea.GetBottom();	}
		const T GetLeft() const		{ return m_bxArea.GetLeft();	}
		const T GetRight() const	{ return m_bxArea.GetRight();	}

	protected:

		bool isValid( const std::vector< Point > & vList ) const
		{
			//if( vList.size() < 3 ) return false;
			
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
					const Point& pt1 = GetIndxPoint( idx_1 );
					const Point& pt2 = GetIndxPoint( idx_2 );

					/*size_t p1 = GetPoint( idx_1 ).index;
					size_t p2 = GetPoint( idx_1 ).owner->getNextIndex( GetPoint( idx_1 ).index ); getNextIndex(idx_1);
					size_t p3 = idx_2;
					size_t p4 = getNextIndex(idx_2);*/

					Line::INTERSECT_RESULT nResult = Line::IntersectCCW( pt1, pt1.getNextPoint(), pt2, pt2.getNextPoint() );					

					if( nResult == Line::INTERSECT ) return false;
					if( nResult == Line::TOUCH && pt1.getNextIndex() != pt2.index && pt1.index != pt2.getNextIndex() ) return false;
				}
			}

			return true;
		}

		bool isClockWise() const
		{
			if( !m_vList.empty() )
				return m_vList[ 0 ].owner->IsClockWise();

			// x 가 가장 작은 점을 찾는다		
			size_t mid_idx = 0;
			for( size_t i = 0; i < m_vList.size(); ++i ) 
			{
				if( m_vList[ mid_idx ].getPoint().x > m_vList[i].getPoint().x ) mid_idx = i;
			}

			int nCCWResult = X2D::CLOCK_WISE;

			size_t cnt = 0;
			while( true )
			{
				const X2D::Point< T >& prevPt = GetPrevPoint( mid_idx );
				const X2D::Point< T >& nextPt = GetNextPoint( mid_idx );

				nCCWResult = X2D::CheckClockWise( 
					prevPt.x, prevPt.y, 
					m_vList[mid_idx].getPoint().x, m_vList[mid_idx].getPoint().y,
					nextPt.x, nextPt.y );

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
				if( (*it).getPoint().x < area.GetLeft()		) area.SetLeft( (*it).getPoint().x );
				if( (*it).getPoint().y < area.GetTop()		) area.SetTop( (*it).getPoint().y );
				if( (*it).getPoint().x > area.GetRight()	) area.SetRight( (*it).getPoint().x );
				if( (*it).getPoint().y > area.GetBottom()	) area.SetBottom( (*it).getPoint().y );
			}
		}

		bool						m_bIsValid;
		bool						m_bIsClockWise;		///< 점 순서가 시계방향인지 여부
		std::vector< Point >		m_vList;
		Box							m_bxArea;			///< 포함체크등에서 박스체크를 선행해서 속도 이득을 볼 수 있는 경우가 많으므로 미리 계산해 놓는다.
	};

	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> & lh, const Point<T>   		& rh )	{ return lh.IsInclude( rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> & lh, const Box<T>     		& rh )	{ return lh.IsInclude( rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> & lh, const Rect<T>    		& rh )	{ return lh.IsInclude( rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> & lh, const Line<T>    		& rh )	{ return lh.IsInclude( rh.begin ) && lh.IsInclude( rh.end );	}
	template< typename T > inline bool INCLUDE(   const Line<T>			& lh, const LeakyPolygon<T> & rh )	{ return false;					}
	template< typename T > inline bool INCLUDE(	  const Rect<T>    		& lh, const LeakyPolygon<T> & rh )	{ return rh.IsIn( lh );			}
	template< typename T > inline bool INCLUDE(	  const Box<T>     		& lh, const LeakyPolygon<T> & rh )	{ return rh.IsIn( lh );			}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> & lh, const LeakyPolygon<T> & rh )	{ return lh.IsInclude( rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const Point<T>		& rh )	{ return lh.IsCollision( rh );	}
	template< typename T > inline bool COLLISION( const Point<T>		& lh, const LeakyPolygon<T> & rh )	{ return rh.IsInclude( lh);		}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const Box<T>     		& rh )	{ return lh.IsCollision( rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const Rect<T>    		& rh )	{ return lh.IsCollision( rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const Line<T>    		& rh )	{ return lh.IsCollision( rh );	}
	template< typename T > inline bool COLLISION( const Line<T>    		& lh, const LeakyPolygon<T> & rh )	{ return rh.IsCollision( lh );	}
	template< typename T > inline bool COLLISION( const Box<T>     		& lh, const LeakyPolygon<T> & rh )	{ return COLLISION( rh, lh );	}
	template< typename T > inline bool COLLISION( const Rect<T>    		& lh, const LeakyPolygon<T> & rh )	{ return COLLISION( rh, lh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const LeakyPolygon<T> & rh )	{ return lh.IsCollision( rh );	}
	template< typename T > inline bool COLLISION( const IndexedPoint<T> & lh, const Point<T>		& rh )  { return lh.owner->IsCollision( rh ); }
	template< typename T > inline bool COLLISION( const Point<T>		& lh, const IndexedPoint<T> & rh )  { return rh.owner->IsCollision( lh ); }
	template< typename T > inline bool COLLISION( const IndexedPoint<T> & lh, const Rect<T>			& rh )  { return lh.owner->IsCollision( rh ); }
	template< typename T > inline bool COLLISION( const Rect<T>			& lh, const IndexedPoint<T> & rh )  { return rh.owner->IsCollision( lh ); }


	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> * lh, const Point<T>   		& rh )	{ return INCLUDE( *lh, rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> * lh, const Box<T>     		& rh )	{ return INCLUDE( *lh, rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> * lh, const Rect<T>    		& rh )	{ return INCLUDE( *lh, rh );	}
	template< typename T > inline bool INCLUDE(	  const Rect<T>    		* lh, const LeakyPolygon<T> & rh )	{ return INCLUDE( *lh, rh );	}
	template< typename T > inline bool INCLUDE(	  const Box<T>     		* lh, const LeakyPolygon<T> & rh )	{ return INCLUDE( *lh, rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> * lh, const LeakyPolygon<T> & rh )	{ return INCLUDE( *lh, rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const Point<T>		& rh )	{ return COLLISION( *lh, rh );	}
	template< typename T > inline bool COLLISION( const Point<T>		* lh, const LeakyPolygon<T> & rh )	{ return COLLISION( *lh, rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const Box<T>     		& rh )	{ return COLLISION( *lh, rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const Rect<T>    		& rh )	{ return COLLISION( *lh, rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const Line<T>    		& rh )	{ return COLLISION( *lh, rh );	}
	template< typename T > inline bool COLLISION( const Box<T>     		* lh, const LeakyPolygon<T> & rh )	{ return COLLISION( *lh, rh );	}
	template< typename T > inline bool COLLISION( const Rect<T>    		* lh, const LeakyPolygon<T> & rh )	{ return COLLISION( *lh, rh );	}
	template< typename T > inline bool COLLISION( const Line<T>    		* lh, const LeakyPolygon<T> & rh )	{ return COLLISION( *lh, rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const LeakyPolygon<T> & rh )	{ return COLLISION( *lh, rh );	}
	template< typename T > inline bool COLLISION( const IndexedPoint<T> * lh, const Point<T>		& rh )  { return (*lh).owner->IsCollision( rh ); }
	template< typename T > inline bool COLLISION( const Point<T>		* lh, const IndexedPoint<T> & rh )  { return rh.owner->IsCollision( *lh ); }
	template< typename T > inline bool COLLISION( const IndexedPoint<T> * lh, const Rect<T>			& rh )  { return (*lh).owner->IsCollision( rh ); }
	template< typename T > inline bool COLLISION( const Rect<T>			* lh, const IndexedPoint<T> & rh )  { return rh.owner->IsCollision( *lh ); }


	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> & lh, const Point<T>   		* rh )	{ return INCLUDE( lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> & lh, const Box<T>     		* rh )	{ return INCLUDE( lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> & lh, const Rect<T>    		* rh )	{ return INCLUDE( lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const Rect<T>    		& lh, const LeakyPolygon<T> * rh )	{ return INCLUDE( lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const Box<T>     		& lh, const LeakyPolygon<T> * rh )	{ return INCLUDE( lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> & lh, const LeakyPolygon<T> * rh )	{ return INCLUDE( lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const Point<T>		* rh )	{ return COLLISION( lh, *rh );	}
	template< typename T > inline bool COLLISION( const Point<T>		& lh, const LeakyPolygon<T> * rh )	{ return COLLISION( lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const Box<T>     		* rh )	{ return COLLISION( lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const Rect<T>    		* rh )	{ return COLLISION( lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const Line<T>    		* rh )	{ return COLLISION( lh, *rh );	}
	template< typename T > inline bool COLLISION( const Box<T>     		& lh, const LeakyPolygon<T> * rh )	{ return COLLISION( lh, *rh );	}
	template< typename T > inline bool COLLISION( const Rect<T>    		& lh, const LeakyPolygon<T> * rh )	{ return COLLISION( lh, *rh );	}
	template< typename T > inline bool COLLISION( const Line<T>    		& lh, const LeakyPolygon<T> * rh )	{ return COLLISION( lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> & lh, const LeakyPolygon<T> * rh )	{ return COLLISION( lh, *rh );	}
	template< typename T > inline bool COLLISION( const IndexedPoint<T> & lh, const Point<T>		* rh )  { return lh.owner->IsCollision( *rh ); }
	template< typename T > inline bool COLLISION( const Point<T>		& lh, const IndexedPoint<T> * rh )  { return (*rh).owner->IsCollision( lh ); }
	template< typename T > inline bool COLLISION( const IndexedPoint<T> & lh, const Rect<T>			* rh )  { return lh.owner->IsCollision( *rh ); }
	template< typename T > inline bool COLLISION( const Rect<T>			& lh, const IndexedPoint<T> * rh )  { return (*rh).owner->IsCollision( lh ); }


	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> * lh, const Point<T>   		* rh )	{ return INCLUDE( *lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> * lh, const Box<T>     		* rh )	{ return INCLUDE( *lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> * lh, const Rect<T>    		* rh )	{ return INCLUDE( *lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const Rect<T>    		* lh, const LeakyPolygon<T> * rh )	{ return INCLUDE( *lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const Box<T>     		* lh, const LeakyPolygon<T> * rh )	{ return INCLUDE( *lh, *rh );	}
	template< typename T > inline bool INCLUDE(	  const LeakyPolygon<T> * lh, const LeakyPolygon<T> * rh )	{ return INCLUDE( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const Point<T>   		* rh )	{ return COLLISION( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const Point<T>		* lh, const LeakyPolygon<T>	* rh )	{ return COLLISION( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const Box<T>     		* rh )	{ return COLLISION( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const Rect<T>    		* rh )	{ return COLLISION( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const Line<T>    		* rh )	{ return COLLISION( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const Box<T>     		* lh, const LeakyPolygon<T> * rh )	{ return COLLISION( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const Rect<T>    		* lh, const LeakyPolygon<T> * rh )	{ return COLLISION( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const Line<T>    		* lh, const LeakyPolygon<T> * rh )	{ return COLLISION( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const LeakyPolygon<T> * lh, const LeakyPolygon<T> * rh )	{ return COLLISION( *lh, *rh );	}
	template< typename T > inline bool COLLISION( const IndexedPoint<T> * lh, const Point<T>		* rh )  { return (*lh).owner->IsCollision( *rh ); }
	template< typename T > inline bool COLLISION( const Point<T>		* lh, const IndexedPoint<T> * rh )  { return (*rh).owner->IsCollision( *lh ); }
	template< typename T > inline bool COLLISION( const IndexedPoint<T> * lh, const Rect<T>			* rh )  { return (*lh).owner->IsCollision( *rh ); }
	template< typename T > inline bool COLLISION( const Rect<T>			* lh, const IndexedPoint<T> * rh )  { return (*rh).owner->IsCollision( *lh ); }

};	// namespace X2D
