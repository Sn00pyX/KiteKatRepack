// X2DBasicTypes.h
//
//  by Testors , 2005/08/01

#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <algorithm>
#include <cmath>
#include <cassert>

namespace X2D
{

	template< typename T > struct NakedType				{ typedef T result; };
	template< typename T > struct NakedType< T* >		{ typedef T result; };
	template< typename T > struct NakedType< T& >		{ typedef T result; };
	template< typename T > struct NakedType< const T* > { typedef T result; };
	template< typename T > struct NakedType< const T& >	{ typedef T result; };

	enum CCW_RESULT { CLOCK_WISE = 1, COUNTER_CLOCK_WISE = -1, PARALLELISM = 0 };
	template< typename T >
	inline CCW_RESULT CheckClockWise( const T & x1, const T & y1, const T & x2, const T & y2, const T & x3, const T & y3 ) 
	{ 
		double l;
		T dx21 = x2 - x1;
		T dy21 = y2 - y1;
		T dx31 = x3 - x1;
		T dy31 = y3 - y1;

		l = ((double)dx21 * dy31) - ((double)dy21 * dx31);
			 if( l > 0 ) return CLOCK_WISE;			// 시계방향의 벡터
		else if( l < 0 ) return COUNTER_CLOCK_WISE; // 반시계방향의 벡터
		else			 return PARALLELISM;		// 평행
	}

	template< typename T = int >
	struct PointBase
	{
		inline PointBase & operator=( const PointBase & rh )	{ x = rh.x; y = rh.y; return *this;		}
		inline bool operator==( const PointBase & rh ) const	{ return ( x == rh.x  && y == rh.y );	}
		inline bool operator!=( const PointBase & rh ) const	{ return !( (*this) == rh );			}
		
		T x, y;
	};

	template< typename T = int >
	struct Point : PointBase< T >
	{
		Point( const T & _x, const T & _y )	{ x = _x; y = _y; }
		Point() {}

		inline T	GetX() const { return x; }
		inline T	GetY() const { return y; }

		inline Point operator+(const Point& rh)	const	{ return Point( x + rh.x, y + rh.y ); }
		inline Point operator-(const Point& rh)	const	{ return Point( x - rh.x, y - rh.y ); }
		inline Point & operator+=(const Point& rh)		{  x += rh.x; y += rh.y; return *this; }
		inline Point & operator-=(const Point& rh)		{  x -= rh.x; y -= rh.y; return *this; }
		inline Point operator*(T s)	const				{ return Point( x * s, y * s ); }
		inline Point operator/(T s)	const				{ return Point( x / s, y / s ); }
		inline Point & operator*=(T s)					{ x *= s; y *= s; return *this; }
		inline Point & operator/=(T s)					{ x /= s; y /= s; return *this; }

		void Set( const T & _x, const T & _y )
		{
			x = _x;
			y = _y;
		}

		T	GetAlternativeDistance( const Point & rh ) const
		{		
			T xd = ( x - rh.x );
			T yd = ( y - rh.y );

			if( xd < 0 ) xd = 0 - xd;
			if( yd < 0 ) yd = 0 - yd;

			return xd + yd;
		}

		T	GetDistance( const Point & rh ) const
		{
			T xd = ( x - rh.x );
			T yd = ( y - rh.y );
			return static_cast< T >( sqrt( ( ((double)xd*xd) + ((double)yd*yd) ) ) );
		}
	};

	template< typename T = int >
	struct Line
	{
		Line( const Point< T > & _begin, const Point< T > & _end ) : begin( _begin ), end( _end ) {}
		Line( const T & x1, const T & y1, const T & x2, const T & y2 ) : begin( x1, y1 ), end( x2, y2 ) {}
		Line() {}

		T GetBeginX() const		{ return begin.x; }
		T GetBeginY() const		{ return begin.y; }
		T GetEndX() const		{ return end.x; }
		T GetEndY() const		{ return end.y; }
		T GetLength() const		{ return begin.GetDistance( end ); }

		T GetLeftX() const		{ return ( begin.x > end.x ? end.x : begin.x ); }
		T GetRightX() const		{ return ( begin.x > end.x ? begin.x : end.x ); }
		T GetTopY() const		{ return ( begin.y > end.y ? end.y : begin.y ); }
		T GetBottomY() const	{ return ( begin.y > end.y ? begin.y : end.y ); }

		bool Has( const Point< T > & p ) const		{ return ( begin == p || end == p ); }

		inline bool IsCollision( const Line & rh ) const
		{
			if( GetLeftX() > rh.GetRightX() ) return false;
			if( GetRightX() < rh.GetLeftX() ) return false;
			if( GetTopY() > rh.GetBottomY() ) return false;
			if( GetBottomY() < rh.GetTopY() ) return false;

			return GetIntersectPoint( rh, NULL, false );
		}

		inline bool IsLooseCollision( const Line & rh ) const
		{
			// 선분끼리 교차하면 충돌
			if( IntersectCCW( rh ) == Line::INTERSECT )
				return true;

			return false;
		}

		inline static bool GetIntersectPoint( const Point<T> & p1, const Point<T> & p2, const Point<T> & p3, const Point<T> & p4, Point<T> *pResult, bool collinear )
		{
			// 기울기 검사
			double under = ((double)p4.y-p3.y)*((double)p2.x-p1.x)-((double)p4.x-p3.x)*((double)p2.y-p1.y);
			if( under == 0 ) // under = 0 이면 기울기가 같다. (평행)
			{
				if( collinear == true )
				{
					// 겹치는 선인지 조사한다. 안 겹치면 평행이므로 return false.
					double discriminant = ((double)p2.y-p1.y) * ((double)p3.x-p1.x) - ((double)p3.y-p1.y) * ((double)p2.x-p1.x);
					if( discriminant == 0 )
					{
						// 겹치면 p4점 리턴 (끝점)
						if( pResult )
						{
							*pResult = p4;
						}

						return true;
					}
				}

				return false;
			}

			double _t = ((double)p4.x-p3.x)*((double)p1.y-p3.y) - ((double)p4.y-p3.y)*((double)p1.x-p3.x);
			double _s = ((double)p2.x-p1.x)*((double)p1.y-p3.y) - ((double)p2.y-p1.y)*((double)p1.x-p3.x);
			if( _t == 0 && _s == 0 )
				return false;

			double t = _t / under;
			double s = _s / under;

			if( t < 0.0 || t > 1.0 || s < 0.0 || s > 1.0 )
				return false;

			if( pResult )
			{
				pResult->x = static_cast< T >( (double)p1.x + t * ((double)p2.x-p1.x) );
				pResult->y = static_cast< T >( (double)p1.y + t * ((double)p2.y-p1.y) );
				// x,y 감소방향 1버퍼처리
				if (p1.x>p2.x) pResult->x += 1;
				if (p1.y>p2.y) pResult->y += 1;
			}

			return true;
		}
		inline static bool GetIntersectPoint( const Line<T> & l1, const Line<T> & l2, Point<T> *pResult, bool collinear ) { return GetIntersectPoint( l1.begin, l1.end, l2.begin, l2.end, pResult, collinear ); }
		inline bool GetIntersectPoint( const Line<T> &line, Point<T> *pResult, bool collinear = false ) const { return GetIntersectPoint( *this, line, pResult, collinear ); }

		enum INTERSECT_RESULT { NONE = -99, INTERSECT = 1, SEPARATE = -1, TOUCH = 0 };
		inline static INTERSECT_RESULT IntersectCCW(const Point<T> & p1,const Point<T> & p2,const Point<T> &p3,const Point<T> & p4 )
		{
			T l1_min_x = std::min( p1.x, p2.x );
			T l1_max_x = std::max( p1.x, p2.x );
			T l1_min_y = std::min( p1.y, p2.y );
			T l1_max_y = std::max( p1.y, p2.y );

			T l2_min_x = std::min( p3.x, p4.x );
			T l2_max_x = std::max( p3.x, p4.x );
			T l2_min_y = std::min( p3.y, p4.y );
			T l2_max_y = std::max( p3.y, p4.y );

			if( l1_min_y > l2_max_y ) return SEPARATE;
			if( l2_min_y > l1_max_y ) return SEPARATE;
			if( l1_min_x > l2_max_x ) return SEPARATE;
			if( l2_min_x > l1_max_x ) return SEPARATE;

			CCW_RESULT ccw123,ccw124,ccw341,ccw342; 

			Point<T> * _p1, *_p2, *_p3, *_p4;
			if( p1.x > p2.x )
			{
				_p1 = const_cast< Point<T>* >( &p2 );
				_p2 = const_cast< Point<T>* >( &p1 );
			}
			else
			{
				_p1 = const_cast< Point<T>* >( &p1 );
				_p2 = const_cast< Point<T>* >( &p2 );
			}

			if( p3.x > p4.x )
			{
				_p3 = const_cast< Point<T>* >( &p4 );
				_p4 = const_cast< Point<T>* >( &p3 );
			}
			else
			{
				_p3 = const_cast< Point<T>* >( &p3 );
				_p4 = const_cast< Point<T>* >( &p4 );
			}

			ccw123 = CheckClockWise(*_p1,*_p2,*_p3); 
			ccw124 = CheckClockWise(*_p1,*_p2,*_p4); 
			ccw341 = CheckClockWise(*_p3,*_p4,*_p1); 
			ccw342 = CheckClockWise(*_p3,*_p4,*_p2); 

			// 교차하는 경우
			if( ccw123 * ccw124 < 0 && ccw341 * ccw342 < 0 ) return INTERSECT;

			// 평행한 경우
			if( ccw123 == PARALLELISM && ccw124 == PARALLELISM )
			{	
				if( _p3->x > _p2-> x || _p1->x > _p4->x ) return SEPARATE;
				else									  return TOUCH; 
			} 

			if( ccw123 == PARALLELISM || ccw124 == PARALLELISM || ccw341 == PARALLELISM || ccw342 == PARALLELISM )
			{
				Point< T > *__p1, *__p2;
				if( _p1->y > _p2->y )
				{
					__p1 = _p2;
					__p2 = _p1;
				}
				else
				{
					__p1 = _p1;
					__p2 = _p2;
				}

				if( ccw123 == PARALLELISM )
				{
					return ( _p1->x > _p3->x || _p3->x > _p2->x || ( _p3->x == _p1->x && _p1->x == _p2->x && ( __p1->y > _p3->y || __p2->y < _p3->y ) ) ) ? SEPARATE : TOUCH;
				} 
				if( ccw124 == PARALLELISM )
				{
					return ( _p1->x > _p4->x || _p4->x > _p2->x || ( _p4->x == _p1->x && _p1->x == _p2->x && ( __p1->y > _p4->y || __p2->y < _p4->y ) ) ) ? SEPARATE : TOUCH; 
				} 

				Point< T > *__p3, *__p4;

				if( _p3->y > _p4->y )
				{
					__p3 = _p4;
					__p4 = _p3;
				}
				else
				{
					__p3 = _p3;
					__p4 = _p4;
				} 

				if( ccw341 == PARALLELISM )
				{
						return ( _p3->x > _p1->x || _p1->x > _p4->x || ( _p1->x == _p3->x && _p3->x == _p4->x && ( __p3->y > _p1->y || __p4->y < _p1->y ) ) ) ? SEPARATE : TOUCH; 
				} 
				if( ccw342 == PARALLELISM )
				{
					return ( _p3->x > _p2->x || _p2->x > _p4->x || ( _p2->x == _p3->x && _p3->x == _p4->x && ( __p3->y > _p2->y || __p4->y < _p2->y ) ) ) ? SEPARATE : TOUCH; 
				} 
			} 

			return SEPARATE; 
		}

		inline INTERSECT_RESULT IntersectCCW( const Line<T> &lh, const Line<T> &rh ) const	 { return lh.IntersectCCW( rh ); }
		inline INTERSECT_RESULT IntersectCCW( const Point<T> &p3,const Point<T> & p4 ) const { return IntersectCCW( begin, end, p3, p4 ); }
		inline INTERSECT_RESULT IntersectCCW( const Line<T> &line ) const					 { return IntersectCCW( begin, end, line.begin, line.end ); }

		inline bool operator==( const Line & rh ) const	{ return ( begin == rh.begin && end == rh.end ) || ( begin == rh.end && end == rh.begin ); }
		inline bool operator!=( const Line & rh ) const	{ return !( (*this) == rh ); }

		Point< T >	begin;
		Point< T >	end;
	};

	template< typename T = int >
	struct Box
	{
		Box( const Point< T > & _begin, const Point< T > & _end ) : begin( _begin ), end( _end )							{ normalize(); }
		Box( const T & left, const T & top, const T & right, const T & bottom ) : begin( left, top ), end( right, bottom ) { normalize(); }
		Box() {}

		inline T	GetX() const				{ return begin.x; }
		inline T	GetY() const				{ return begin.y; }
		inline T	GetLeft() const				{ return begin.x; }
		inline T	GetTop() const				{ return begin.y; }
		inline T	GetRight() const			{ return end.x; }
		inline T	GetBottom() const			{ return end.y; }
		inline T	GetWidth() const			{ return end.x - begin.x + 1; }
		inline T	GetHeight() const			{ return end.y - begin.y + 1; }
		inline void	SetLeft( const T & x )		{ begin.x = x;	if( begin.x > end.x ) std::swap( begin.x, end.x ); }
		inline void	SetTop( const T & y )		{ begin.y = y;	if( begin.y > end.y ) std::swap( begin.y, end.y ); }
		inline void	SetRight( const T & x )		{ end.x = x;	if( begin.x > end.x ) std::swap( begin.x, end.x ); }
		inline void	SetBottom( const T & y )	{ end.y = y;	if( begin.y > end.y ) std::swap( begin.y, end.y ); }
		inline T	GetSize() const				{ return GetWidth() * GetHeight(); }
		inline void Move( const T & x, const T & y )
		{
			T width		= GetWidth();
			T height	= GetHeight();

			begin.Set( x, y );
			end.Set( x + width - 1, y + height - 1 );
		}

		bool Has( const Point< T > & p ) const
		{
			if( begin == p || end == p ) return true;
			if( Point< T >( end.x, begin.y ) == p || Point<T>( begin.x, end.y ) == p ) return true;
			return false;
		}

		bool Has( const Line< T > & line ) const
		{
			return ( GetSegment( 0 ) == line || GetSegment( 1 ) == line || GetSegment( 2 ) == line || GetSegment( 3 ) == line );
		}

		Point<T> GetCenter() const
		{
			return Point<T>( ( GetLeft() + GetRight() ) / 2, ( GetTop() + GetBottom() ) / 2 );
		}

		inline bool IsInclude( const T & x, const T & y ) const	{ return !( begin.x > x || end.x < x || begin.y > y || end.y < y ); }
		inline bool IsInclude( const Point< T > & pt ) const	{ return IsInclude( pt.x, pt.y ); }
		inline bool IsInclude( const Box< T > & c ) const		{ return ( c.begin.x >= begin.x && c.end.x <= end.x && c.begin.y >= begin.y && c.end.y <= end.y ); }

		inline bool IsLooseInclude( const T & x, const T & y ) const	{ return !( begin.x >= x || end.x <= x || begin.y >= y || end.y <= y ); }
		inline bool IsLooseInclude( const Point< T > & pt ) const	{ return IsLooseInclude( pt.x, pt.y ); }
		inline bool IsLooseInclude( const Box< T > & c ) const		{ return ( c.begin.x > begin.x && c.end.x < end.x && c.begin.y > begin.y && c.end.y < end.y ); }

		inline bool IsCollision( const Point< T > & pt ) const	{ return IsInclude( pt.x, pt.y ); }

		inline bool IsCollision( const Line< T > & line ) const
		{
			if( IsInclude( line.begin ) || IsInclude( line.end ) ) return true;

			/*
			if( line.GetLeftX() > GetRight() || line.GetRightX() < GetLeft() ) return false;
			if( line.GetTopY() > GetBottom() || line.GetBottomY() < GetTop() ) return false;
			*/

			if( GetSegment( 0 ).IsCollision( line ) || GetSegment( 1 ).IsCollision( line ) ||
				GetSegment( 2 ).IsCollision( line ) || GetSegment( 3 ).IsCollision( line ) ) return true;

			return false;
		}

		inline bool IsCollision( const T & left, const T & top, const T & right, const T & bottom ) const
		{
			return ( std::min( end.x, right ) >= std::max( begin.x,  left ) &&
					 std::min( end.y,  bottom ) >= std::max( begin.y, top ) );
		}

		inline bool IsCollision( const Box & c ) const
		{
			return ( std::min( end.x, c.end.x ) >= std::max( begin.x,  c.begin.x ) &&
					 std::min( end.y,  c.end.y ) >= std::max( begin.y, c.begin.y ) );
		}

		inline bool IsLooseCollision( const Point< T > & pt ) const	{ return IsLooseInclude( pt.x, pt.y ); }

		inline bool IsLooseCollision( const Line< T > & line ) const
		{
			if( IsLooseInclude( line.begin ) || IsLooseInclude( line.end ) )
				return true;

			/*
			if( line.GetLeftX() >= GetRight() || line.GetRightX() <= GetLeft() ) return false;
			if( line.GetTopY() >= GetBottom() || line.GetBottomY() <= GetTop() ) return false;
			*/

			if( GetSegment( 0 ).IsLooseCollision( line ) || GetSegment( 1 ).IsLooseCollision( line ) ||
				GetSegment( 2 ).IsLooseCollision( line ) || GetSegment( 3 ).IsLooseCollision( line ) )
				return true;

			return false;
		}

		inline bool IsLooseCollision( const T & left, const T & top, const T & right, const T & bottom ) const
		{
			return ( std::min( end.x, right ) > std::max( begin.x,  left ) &&
					 std::min( end.y,  bottom ) > std::max( begin.y, top ) );
		}

		inline bool IsLooseCollision( const Box & c ) const
		{
			return ( std::min( end.x, c.end.x ) > std::max( begin.x,  c.begin.x ) &&
					 std::min( end.y,  c.end.y ) > std::max( begin.y, c.begin.y ) );
		}

		void Set( const Point< T > & _begin, const Point< T > & _end )
		{
			begin	= _begin;
			end		= _end;

			normalize();
		}

		void Set( const T & left, const T & top, const T & right, const T & bottom )
		{
			begin.Set( left, top );
			end.Set( right, bottom );

			normalize();
		}

		const Line< T > GetSegment( size_t idx ) const
		{
			assert( idx < 4 );

				 if( idx == 0 ) return Line< T >( begin.x, begin.y, end.x,   begin.y );
			else if( idx == 1 ) return Line< T >( end.x,   begin.y, end.x,   end.y );
			else if( idx == 2 ) return Line< T >( end.x,   end.y ,  begin.x, end.y );
			else				return Line< T >( begin.x, end.y,   begin.x, begin.y );
		}

		const Point< T > GetPoint( size_t idx ) const
		{
			assert( idx < 4 );

				 if( idx == 0 ) return Point< T >( begin.x, begin.y );
			else if( idx == 1 ) return Point< T >( end.x,   begin.y );
			else if( idx == 2 ) return Point< T >( end.x,   end.y   );
			else				return Point< T >( begin.x, end.y   );
		}

		const Point< T > GetLeftTop() const		{ return Point< T >( begin.x, begin.y );}
		const Point< T > GetRightTop() const	{ return Point< T >( end.x, begin.y );	}
		const Point< T > GetLeftBottom() const	{ return Point< T >( begin.x, end.y );	}
		const Point< T > GetRightBottom() const	{ return Point< T >( end.x, end.y );	}


		inline bool operator==( const Box & rh ) const	{ return ( begin == rh.begin && end == rh.end ); }
		inline bool operator!=( const Box & rh ) const	{ return !( (*this) == rh ); }

	private:

		void		normalize()
		{
			if( begin.x > end.x ) std::swap( begin.x, end.x );
			if( begin.y > end.y ) std::swap( begin.y, end.y );
		}

		Point< T >	begin;
		Point< T >	end;
	};

	template< typename T = int >
	struct Rect
	{
		Rect( const Point< T > & _pos, const Point< T > & _size ) : pos( _pos ), size( _size )						{}
		Rect( const T & x, const T & y, const T & width, const T & height ) : pos( x, y ), size( width, height )	{}
		Rect() {}

		inline T	GetX() const				{ return pos.x; }
		inline T	GetY() const				{ return pos.y; }	
		inline T	GetWidth() const			{ return size.x; }
		inline T	GetHeight() const			{ return size.y; }
		inline T	GetSize() const				{ return GetWidth() * GetHeight(); }

		inline void Move( const T & x, const T & y )	{ pos.Set( x, y );	}

		enum TOUCH_POSITION { SEPARATE = -1, LEFT = 0, TOP = 1, RIGHT = 2, BOTTOM = 3, COLLISION = 4 };
		inline TOUCH_POSITION GetTouchPosition( const Rect< T > & rc ) const
		{
			if( IsCollision( rc ) ) return COLLISION;

			bool bX = ( rc.GetX() >= GetX() && rc.GetX() < GetRight() ) ||
					  ( GetX() >= rc.GetX() && GetX() < rc.GetRight() );
			bool bY = ( rc.GetY() >= GetY() && rc.GetY() < GetBottom() ) ||
					  ( GetY() >= rc.GetY() && GetY() < rc.GetBottom() );

			if( bX )
			{
				if( rc.GetY() + rc.GetHeight() == GetY() ) return TOP;
				if( GetY() + GetHeight() == rc.GetY() ) return BOTTOM;
			}

			if( bY )
			{
				if( rc.GetX() + rc.GetWidth() == GetX() ) return LEFT;
				if( GetX() + GetWidth() == rc.GetX() ) return RIGHT;
			}

			return SEPARATE;
		}

		inline bool IsInclude( const T & x, const T & y ) const	{ return ( GetX() <= x && GetRight() > x && GetY() <= y && GetBottom() > y ); }
		inline bool IsInclude( const Point< T > & pt ) const	{ return IsInclude( pt.x, pt.y ); }
		inline bool IsInclude( const Rect< T > & c ) const		{ return ( GetX() <= c.GetX() && GetRight() >= c.GetRight() && GetY() <= c.GetY() && GetBottom() >= c.GetBottom() ); }
		inline bool IsInclude( const Box< T > & c ) const		{ return ( GetX() <= c.GetLeft() && GetRight() > c.GetRight() && GetTop() <= c.GetY() && GetBottom() > c.GetBottom() ); }

		inline bool IsLooseInclude( const T & x, const T & y ) const	{ return ( GetX() < x && GetRight() > x && GetY() < y && GetBottom() > y ); }
		inline bool IsLooseInclude( const Point< T > & pt ) const	{ return IsLooseInclude( pt.x, pt.y ); }
		inline bool IsLooseInclude( const Rect< T > & c ) const		{ return ( GetX() < c.GetX() && GetRight() > c.GetRight() && GetY() < c.GetY() && GetBottom() > c.GetBottom() ); }
		inline bool IsLooseInclude( const Box< T > & c ) const		{ return ( GetX() < c.GetLeft() && GetRight() > c.GetRight() && GetTop() < c.GetY() && GetBottom() > c.GetBottom() ); }

		inline bool IsCloselyInclude( const T & x, const T & y ) const	{ return ( GetX() <= x && GetRight() >= x && GetY() <= y && GetBottom() >= y ); }
		inline bool IsCloselyInclude( const Point< T > & pt ) const		{ return IsCloselyInclude( pt.x, pt.y ); }
		inline bool IsCloselyInclude( const Rect< T > & c ) const		{ return ( GetX() <= c.GetX() && GetRight() >= c.GetRight() && GetY() <= c.GetY() && GetBottom() >= c.GetBottom() ); }
		inline bool IsCloselyInclude( const Box< T > & c ) const		{ return ( GetX() <= c.GetLeft() && GetRight() >= c.GetRight() && GetTop() <= c.GetY() && GetBottom() >= c.GetBottom() ); }

		inline bool IsCollision( const Point< T > & pt ) const	{ return IsInclude( pt.x, pt.y ); }

		inline bool IsCollision( const Line< T > & line ) const
		{
			if( size.x == 0 || size.y == 0 ) return false;

			Point< T > middle( ( line.begin.x + line.end.x ) / 2, ( line.begin.y + line.end.y ) / 2 );

			if( IsInclude( middle ) || IsInclude( line.begin ) || IsInclude( line.end ) ) return true;

			X2D::Line< T > top( pos.x, pos.y, pos.x + size.x, pos.y );
			X2D::Line< T > bottom( pos.x, pos.y + size.y, pos.x + size.x, pos.y + size.y );
			X2D::Line< T > left( pos.x, pos.y, pos.x, pos.y + size.y );
			X2D::Line< T > right( pos.x + size.x, pos.y, pos.x + size.x, pos.y + size.y );

			Line<T>::INTERSECT_RESULT result1 = line.IntersectCCW( top );
			if( result1 == Line<T>::INTERSECT ) return true;
			Line<T>::INTERSECT_RESULT result2 = line.IntersectCCW( bottom );
			if( result2 == Line<T>::INTERSECT ) return true;
			Line<T>::INTERSECT_RESULT result3 = line.IntersectCCW( left );
			if( result3 == Line<T>::INTERSECT ) return true;
			Line<T>::INTERSECT_RESULT result4 = line.IntersectCCW( right );
			if( result4 == Line<T>::INTERSECT ) return true;
			
			if( result3 == Line<T>::TOUCH )
			{
				return ( result2 != Line<T>::TOUCH );
			}

			if( result1 == Line<T>::TOUCH )
			{
				return ( result4 != Line<T>::TOUCH );
			}
			
			return false;
		}

		inline bool IsCollision( const Rect & c ) const
		{
			return ( std::min( GetRight(),  c.GetRight() )  > std::max( GetX(),  c.GetX() ) &&
					 std::min( GetBottom(), c.GetBottom() ) > std::max( GetY(),  c.GetY() ) );
		}

		inline bool IsCollision( const Box< T > & bx ) const
		{
			bool bX = ( GetX() >= bx.GetLeft() && GetX() <= bx.GetRight() ) ||
					  ( bx.GetLeft() >= GetX() && bx.GetLeft() < GetRight() );

			bool bY = ( GetY() >= bx.GetTop() && GetY() <= bx.GetBottom() ) ||
					  ( bx.GetTop() >= GetY() && bx.GetTop() < GetBottom() );

			return bX && bY;
		}

		inline bool IsLooseCollision( const Point< T > & pt ) const	{ return IsLooseInclude( pt.x, pt.y ); }

		inline bool IsLooseCollision( const Line< T > & line ) const
		{
			if( size.x == 0 || size.y == 0 ) return false;

			Point< T > middle( ( line.begin.x + line.end.x ) / 2, ( line.begin.y + line.end.y ) / 2 );

			if( IsLooseInclude( middle ) || IsLooseInclude( line.begin ) || IsLooseInclude( line.end ) ) return true;

			X2D::Line< T > top( pos.x, pos.y, pos.x + size.x, pos.y );
			X2D::Line< T > bottom( pos.x, pos.y + size.y, pos.x + size.x, pos.y + size.y );
			X2D::Line< T > left( pos.x, pos.y, pos.x, pos.y + size.y );
			X2D::Line< T > right( pos.x + size.x, pos.y, pos.x + size.x, pos.y + size.y );

			Line<T>::INTERSECT_RESULT result1 = line.IntersectCCW( top );
			if( result1 == Line<T>::INTERSECT ) return true;
			Line<T>::INTERSECT_RESULT result2 = line.IntersectCCW( bottom );
			if( result2 == Line<T>::INTERSECT ) return true;
			Line<T>::INTERSECT_RESULT result3 = line.IntersectCCW( left );
			if( result3 == Line<T>::INTERSECT ) return true;
			Line<T>::INTERSECT_RESULT result4 = line.IntersectCCW( right );
			if( result4 == Line<T>::INTERSECT ) return true;
			
			return false;
		}

		inline bool IsLooseCollision( const Rect & c ) const
		{
			return ( std::min( GetRight(),  c.GetRight() )  > std::max( GetX(),  c.GetX() ) &&
					 std::min( GetBottom(), c.GetBottom() ) > std::max( GetY(),  c.GetY() ) );
		}

		inline bool IsLooseCollision( const Box< T > & bx ) const
		{
			bool bX = ( GetX() > bx.GetLeft() && GetX() < bx.GetRight() ) ||
					  ( bx.GetLeft() > GetX() && bx.GetLeft() < GetRight() );

			bool bY = ( GetY() >= bx.GetTop() && GetY() <= bx.GetBottom() ) ||
					  ( bx.GetTop() >= GetY() && bx.GetTop() < GetBottom() );

			return bX && bY;
		}

		void Set( const Point< T > & _pos, const Point< T > & _size )
		{
			pos			= _pos;
			size		= _size;
		}

		void SetWidth( const T & width ) { size.x = width; }
		void SetHeight( const T & height ) { size.y = height; }

		void Set( const T & x, const T & y, const T & width, const T & height )
		{
			pos.Set( x, y );
			size.Set( width, height );
		}

		inline T	GetLeft() const				{ return pos.x; }
		inline T	GetTop() const				{ return pos.y; }
		inline T	GetRight() const			{ return pos.x + size.x; }
		inline T	GetBottom() const			{ return pos.y + size.y; }
		const Point< T > GetLeftTop() const		{ return Point< T >( GetLeft(), GetTop() );}
		const Point< T > GetRightTop() const	{ return Point< T >( GetRight(), GetTop() );	}
		const Point< T > GetLeftBottom() const	{ return Point< T >( GetLeft(), GetBottom() );	}
		const Point< T > GetRightBottom() const	{ return Point< T >( GetRight(), GetBottom() );	}

		inline bool operator==( const Rect & rh ) const	{ return ( pos == rh.pos && size == rh.size ); }
		inline bool operator!=( const Rect & rh ) const	{ return !( (*this) == rh ); }

	private:

		Point< T >	pos;
		Point< T >	size;
	};

	// reference & reference

	template< typename T > inline bool INCLUDE(  		 const Point<T> & lh,	const Point<T> & rh )	{ return lh == rh;					}
	template< typename T > inline bool INCLUDE(  		 const Box<T>   & lh,	const Box<T>   & rh )	{ return lh.IsInclude( rh );		}
	template< typename T > inline bool INCLUDE(  		 const Rect<T>  & lh,	const Rect<T>  & rh )	{ return lh.IsInclude( rh );		}
	template< typename T > inline bool INCLUDE(  		 const Box<T>   & lh,	const Point<T> & rh )	{ return lh.IsInclude( rh );		}
	template< typename T > inline bool INCLUDE(  		 const Rect<T>  & lh,	const Point<T> & rh )	{ return lh.IsInclude( rh );		}
	template< typename T > inline bool INCLUDE(  		 const Point<T> & lh,	const Box<T>   & rh )	{ return rh.GetLeftTop() == lh && rh.GetRightBottom() == lh;	}
	template< typename T > inline bool INCLUDE(  		 const Point<T> & lh,	const Rect<T>  & rh )	{ return rh.GetWidth() == 0 && rh.GetHeight() == 0 && rh.GetX() == lh.x && rh.GetY() == lh.y;	}
	template< typename T > inline bool INCLUDE(  		 const Rect<T>  & lh,	const Box<T>   & rh )	{ return lh.IsInclude( rh );		}
	template< typename T > inline bool INCLUDE(  		 const Box<T>   & lh,	const Rect<T>  & rh )	{ return lh.IsInclude( rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> & lh,	const Point<T> & rh )	{ return lh == rh;					}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   & lh,	const Box<T>   & rh )	{ return lh.IsLooseInclude( rh );	}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  & lh,	const Rect<T>  & rh )	{ return lh.IsLooseInclude( rh );	}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   & lh,	const Point<T> & rh )	{ return lh.IsLooseInclude( rh );	}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  & lh,	const Point<T> & rh )	{ return lh.IsLooseInclude( rh );	}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> & lh,	const Box<T>   & rh )	{ return false;						}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> & lh,	const Rect<T>  & rh )	{ return false;						}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  & lh,	const Box<T>   & rh )	{ return lh.IsLooseInclude( rh );	}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   & lh,	const Rect<T>  & rh )	{ return lh.IsLooseInclude( rh );	}
	template< typename T > inline bool COLLISION( 		const Point<T> & lh,	const Point<T> & rh )	{ return lh == rh;					}
	template< typename T > inline bool COLLISION( 		const Box<T>   & lh,	const Box<T>   & rh )	{ return lh.IsCollision( rh );		}
	template< typename T > inline bool COLLISION( 		const Box<T>   & lh,	const Point<T> & rh )	{ return lh.IsInclude( rh );		}
	template< typename T > inline bool COLLISION( 		const Rect<T>  & lh,	const Box<T>   & rh )	{ return lh.IsCollision( rh );		}
	template< typename T > inline bool COLLISION( 		const Rect<T>  & lh,	const Point<T> & rh )	{ return lh.IsInclude( rh );		}
	template< typename T > inline bool COLLISION( 		const Rect<T>  & lh,	const Line<T> & rh )	{ return lh.IsCollision( rh );		}
	template< typename T > inline bool COLLISION( 		const Rect<T>  & lh,	const Rect<T>  & rh )	{ return lh.IsCollision( rh );		}
	template< typename T > inline bool COLLISION( 		const Point<T> & lh,	const Rect<T>  & rh )	{ return COLLISION( rh, lh );		}
	template< typename T > inline bool COLLISION( 		const Line<T>  & lh,	const Rect<T>  & rh )	{ return COLLISION( rh, lh );		}
	template< typename T > inline bool COLLISION( 		const Box<T>   & lh,	const Rect<T>  & rh )	{ return COLLISION( rh, lh );		}
	template< typename T > inline bool COLLISION( 		const Point<T> & lh,	const Box<T>   & rh )	{ return COLLISION( rh, lh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> & lh,	const Point<T> & rh )	{ return lh == rh;					}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   & lh,	const Box<T>   & rh )	{ return lh.IsLooseCollision( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   & lh,	const Point<T> & rh )	{ return lh.IsLooseInclude( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  & lh,	const Box<T>   & rh )	{ return lh.IsLooseCollision( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  & lh,	const Point<T> & rh )	{ return lh.IsLooseInclude( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  & lh,	const Line<T> & rh )	{ return lh.IsLooseCollision( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  & lh,	const Rect<T>  & rh )	{ return lh.IsLooseCollision( rh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> & lh,	const Rect<T>  & rh )	{ return LOOSE_COLLISION( rh, lh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Line<T>  & lh,	const Rect<T>  & rh )	{ return LOOSE_COLLISION( rh, lh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   & lh,	const Rect<T>  & rh )	{ return LOOSE_COLLISION( rh, lh );		}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> & lh,	const Box<T>   & rh )	{ return LOOSE_COLLISION( rh, lh );		}

	// 이하는 reference & reference 로 구현됨.


	// pointer & reference

	template< typename T > inline bool INCLUDE(   		const Point<T> * lh,	const Point<T> & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(   		const Box<T>   * lh,	const Box<T>   & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(   		const Rect<T>  * lh,	const Rect<T>  & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(   		const Box<T>   * lh,	const Point<T> & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(   		const Rect<T>  * lh,	const Point<T> & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(   		const Point<T> * lh,	const Box<T>   & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(   		const Point<T> * lh,	const Rect<T>  & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(   		const Rect<T>  * lh,	const Box<T>   & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool INCLUDE(   		const Box<T>   * lh,	const Rect<T>  & rh )	{ return INCLUDE( *lh, rh );			}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> * lh,	const Point<T> & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   * lh,	const Box<T>   & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  * lh,	const Rect<T>  & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   * lh,	const Point<T> & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  * lh,	const Point<T> & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> * lh,	const Box<T>   & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> * lh,	const Rect<T>  & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  * lh,	const Box<T>   & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   * lh,	const Rect<T>  & rh )	{ return LOOSE_INCLUDE( *lh, rh );		}
	template< typename T > inline bool COLLISION( 		const Point<T> * lh,	const Point<T> & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Box<T>   * lh,	const Box<T>   & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Box<T>   * lh,	const Point<T> & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Rect<T>  * lh,	const Box<T>   & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Rect<T>  * lh,	const Point<T> & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Rect<T>  * lh,	const Line<T>  & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Rect<T>  * lh,	const Rect<T>  & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Point<T> * lh,	const Rect<T>  & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Line<T>  * lh,	const Rect<T>  & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Box<T>   * lh,	const Rect<T>  & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool COLLISION( 		const Point<T> * lh,	const Box<T>   & rh )	{ return COLLISION( *lh, rh );			}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> * lh,	const Point<T> & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   * lh,	const Box<T>   & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   * lh,	const Point<T> & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  * lh,	const Box<T>   & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  * lh,	const Point<T> & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  * lh,	const Line<T> & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  * lh,	const Rect<T>  & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> * lh,	const Rect<T>  & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Line<T>  * lh,	const Rect<T>  & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   * lh,	const Rect<T>  & rh )	{ return LOOSE_COLLISION( *lh, rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> * lh,	const Box<T>   & rh )	{ return LOOSE_COLLISION( *lh, rh );	}


	// reference & pointer

	template< typename T > inline bool INCLUDE(			const Point<T> & lh,	const Point<T> * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Box<T>   & lh,	const Box<T>   * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Rect<T>  & lh,	const Rect<T>  * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Box<T>   & lh,	const Point<T> * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Rect<T>  & lh,	const Point<T> * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Point<T> & lh,	const Box<T>   * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Point<T> & lh,	const Rect<T>  * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Rect<T>  & lh,	const Box<T>   * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Box<T>   & lh,	const Rect<T>  * rh )	{ return INCLUDE( lh, *rh );			}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> & lh,	const Point<T> * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   & lh,	const Box<T>   * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  & lh,	const Rect<T>  * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   & lh,	const Point<T> * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  & lh,	const Point<T> * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> & lh,	const Box<T>   * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> & lh,	const Rect<T>  * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  & lh,	const Box<T>   * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   & lh,	const Rect<T>  * rh )	{ return LOOSE_INCLUDE( lh, *rh );		}
	template< typename T > inline bool COLLISION(		const Point<T> & lh,	const Point<T> * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Box<T>   & lh,	const Box<T>   * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Box<T>   & lh,	const Point<T> * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>  & lh,	const Box<T>   * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>  & lh,	const Point<T> * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>  & lh,	const Line<T>  * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>  & lh,	const Rect<T>  * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Point<T> & lh,	const Rect<T>  * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Line<T>  & lh,	const Rect<T>  * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Box<T>   & lh,	const Rect<T>  * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Point<T> & lh,	const Box<T>   * rh )	{ return COLLISION( lh, *rh );			}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> & lh,	const Point<T> * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   & lh,	const Box<T>   * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   & lh,	const Point<T> * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  & lh,	const Box<T>   * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  & lh,	const Point<T> * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  & lh,	const Line<T>  * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  & lh,	const Rect<T>  * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> & lh,	const Rect<T>  * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Line<T>  & lh,	const Rect<T>  * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   & lh,	const Rect<T>  * rh )	{ return LOOSE_COLLISION( lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> & lh,	const Box<T>   * rh )	{ return LOOSE_COLLISION( lh, *rh );	}


	// pointer & pointer

	template< typename T > inline bool INCLUDE(			const Point<T> * lh,	const Point<T> * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Box<T>   * lh,	const Box<T>   * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Rect<T>  * lh,	const Rect<T>  * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Box<T>   * lh,	const Point<T> * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Rect<T>  * lh,	const Point<T> * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Point<T> * lh,	const Box<T>   * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Point<T> * lh,	const Rect<T>  * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Rect<T>  * lh,	const Box<T>   * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool INCLUDE(			const Box<T>   * lh,	const Rect<T>  * rh )	{ return INCLUDE( *lh, *rh );			}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> * lh,	const Point<T> * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   * lh,	const Box<T>   * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  * lh,	const Rect<T>  * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   * lh,	const Point<T> * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  * lh,	const Point<T> * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> * lh,	const Box<T>   * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Point<T> * lh,	const Rect<T>  * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Rect<T>  * lh,	const Box<T>   * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool LOOSE_INCLUDE(	const Box<T>   * lh,	const Rect<T>  * rh )	{ return LOOSE_INCLUDE( *lh, *rh );		}
	template< typename T > inline bool COLLISION(		const Point<T> * lh,	const Point<T> * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Box<T>   * lh,	const Box<T>   * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Box<T>   * lh,	const Point<T> * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>  * lh,	const Box<T>   * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>  * lh,	const Point<T> * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>  * lh,	const Line<T>  * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Rect<T>  * lh,	const Rect<T>  * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Point<T> * lh,	const Rect<T>  * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Line<T>  * lh,	const Rect<T>  * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Box<T>   * lh,	const Rect<T>  * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool COLLISION(		const Point<T> * lh,	const Box<T>   * rh )	{ return COLLISION( *lh, *rh );			}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> * lh,	const Point<T> * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   * lh,	const Box<T>   * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   * lh,	const Point<T> * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  * lh,	const Box<T>   * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  * lh,	const Point<T> * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  * lh,	const Line<T>  * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Rect<T>  * lh,	const Rect<T>  * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> * lh,	const Rect<T>  * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Line<T>  * lh,	const Rect<T>  * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Box<T>   * lh,	const Rect<T>  * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}
	template< typename T > inline bool LOOSE_COLLISION(	const Point<T> * lh,	const Box<T>   * rh )	{ return LOOSE_COLLISION( *lh, *rh );	}

	template< typename T >
	inline CCW_RESULT CheckClockWise( const Point< T > & pt1, const Point< T > & pt2, const Point< T > & pt3 )
	{
		return CheckClockWise( pt1.x, pt1.y, pt2.x, pt2.y, pt3.x, pt3.y );
	}

}; // namespace X2D

