#pragma once

#include <cmath>
#include <cstdio>
#include <vector>

#include "ArOption.h"
#include "ArTime.h"

#include "../dump/XException.h"


// 위치정보 타입 정의
struct ArPosition
{
	ArPosition()
	{
		x = y = z = 0;
		face = 0;
	}

	ArPosition( AR_UNIT _x, AR_UNIT _y, AR_UNIT _z = 0 )
	{
		x = _x;
		y = _y;
		z = _z;
		face = 0;
	}

	unsigned GetRX() const		{ return ::GetRegionX( x ); };
	unsigned GetRY() const		{ return ::GetRegionY( y ); };

	AR_UNIT GetDistance( const ArPosition & pos ) const
	{
		AR_UNIT _x = pos.x - x;
		AR_UNIT _y = pos.y - y;

		return static_cast< AR_UNIT >( sqrt( (float) _x*_x + _y*_y ) );
	}

	/// 2010.10.13 - prodongi
	AR_UNIT GetDistanceSqr( const ArPosition & pos ) const
	{
		AR_UNIT _x = pos.x - x;
		AR_UNIT _y = pos.y - y;

		return (_x*_x + _y*_y);
	}

	bool operator==( const ArPosition & rh ) const
	{
		if( (int)x != (int)rh.x ) return false;
		if( (int)y != (int)rh.y ) return false;
		if( (int)z != (int)rh.z ) return false;
		return true;
	}

	AR_UNIT			GetX() const	{ return x; }
	AR_UNIT			GetY() const	{ return y; }
	AR_UNIT			GetZ() const	{ return z; }

	AR_UNIT			x, y, z;
	float			face;
};

// 이동정보 타입 정의
struct ArMoveVector : ArPosition
{
	struct MOVE_INFO
	{
		MOVE_INFO( ArPosition _end, AR_TIME _end_time ) : end( _end ), end_time( _end_time ) {}

		ArPosition	end;
		AR_TIME		end_time;
	};

	ArMoveVector()
	{
		bWithZMoving = bIsMoving = false;
		speed = 0;
		proc_time = start_time = 0;

		bHasDirectionChanged	=	false;
	}

	// 시간 갭 t 를 준다. 이동이 끝나면 true, 여전히 이동해야 하면 false 리턴
	bool Step( AR_TIME current_time )
	{
		if( proc_time >= current_time ) return false;

		if( !bIsMoving ) return false;


		for( std::vector< MOVE_INFO >::iterator it = ends.begin(); it != ends.end(); )
		{
			if( current_time >= (*it).end_time )
			{
				x = (*it).end.x;
				y = (*it).end.y;
				z = (*it).end.z;

				proc_time = (*it).end_time;

				it = ends.erase( it );

				if( it != ends.end() )
				{
					SetDirection( (*it).end );
				}

				continue;
			}

			AR_TIME time_gap = current_time - proc_time;

			float v = ( time_gap / (float)((*it).end_time - proc_time) );

			x += ( (*it).end.x - x ) * v;
			y += ( (*it).end.y - y ) * v;

			if( bWithZMoving ) z += ( (*it).end.z - z ) * v;

			break;
		}

		proc_time = current_time;

		if( ends.size() < 1 || (ends.back().end.x == x && ends.back().end.y == y && ends.back().end.z == z) )
		{
			bIsMoving = false;
			ends.clear();
			return true;
		}

		return false;
	};

	void StopMove()
	{
		ends.clear();

		bIsMoving = false;
	}

	void SetMultipleMove( const std::vector< ArPosition > & _to, unsigned char _speed, AR_TIME _start_time, AR_TIME current_time )
	{
		ends.clear();

		if( _to.size() < 1 ) return;

		speed	= _speed;
		if( !_start_time ) start_time = GetArTime();
		else start_time = _start_time;

		proc_time = start_time;

		SetDirection( _to.front() );

		AR_UNIT before_x, before_y;
		AR_TIME _cur_start_time, _end_time;

		_cur_start_time = start_time;

		before_x = x;
		before_y = y;
		for( std::vector< ArPosition >::const_iterator it = _to.begin(); it != _to.end(); ++it )
		{
			AR_UNIT X = ( it->x - before_x );
			AR_UNIT Y = ( it->y - before_y );

			before_x = it->x;
			before_y = it->y;

			AR_UNIT length = AR_UNIT( sqrt( (double)X*X + Y*Y ) );

			_cur_start_time = _end_time = _cur_start_time + AR_TIME( length / ((float)speed/SPEED_UNIT) );

			ends.push_back( MOVE_INFO( (*it), _end_time ) );
		}

		bIsMoving = true;

	}

	void SetMove( const ArPosition & _to, unsigned char _speed, AR_TIME _start_time, AR_TIME current_time )
	{
		ends.clear();

		speed	= _speed;
		if( !_start_time ) start_time = GetArTime();
		else start_time = _start_time;

		proc_time = start_time;

		AR_UNIT X = ( _to.x - x );
		AR_UNIT Y = ( _to.y - y );

		SetDirection( _to );

		AR_UNIT length =  AR_UNIT( sqrt( (double)X*X + Y*Y ) );

		AR_TIME end_time = start_time + AR_TIME( length / ((float)speed/SPEED_UNIT) );

		ends.push_back( MOVE_INFO( _to, end_time ) );

		bIsMoving = true;
	}

	void SetKnockBack( const ArPosition & _to, unsigned char _speed, AR_TIME _start_time, AR_TIME current_time )
	{
		ends.clear();

		speed	= _speed;
		if( !_start_time ) start_time = GetArTime();
		else start_time = _start_time;

		proc_time = start_time;

		AR_UNIT X = ( _to.x - x );
		AR_UNIT Y = ( _to.y - y );

		AR_UNIT length =  AR_UNIT( sqrt( (double)X*X + Y*Y ) );

		AR_TIME end_time = start_time + AR_TIME( length / ((float)speed/SPEED_UNIT) );

		ends.push_back( MOVE_INFO( _to, end_time ) );

		bIsMoving = true;
	}

	AR_UNIT GetTX()	const
	{
		if( ends.size() )
			return ends.back().end.x;

		return x;
	}

	AR_UNIT GetTY()	const
	{
		if( ends.size() )
			return ends.back().end.y;

		return y;
	}

	AR_UNIT GetTZ()	const
	{
		if( ends.size() )
			return ends.back().end.z;

		return z;
	}

	AR_UNIT GetX() const							{ return x;				};
	AR_UNIT GetY() const							{ return y;				};
	AR_UNIT GetZ() const							{ return z;				};
	float	GetFace() const							{ return face;			}; //라디안~
	void	SetFace( float _face )					{ face = _face; }
	short	GetDegree() const						{ return short( 180/3.141592 * GetFace() ); };
	void	SetDirection( const ArPosition & _to )
	{
		AR_UNIT X = ( _to.x - x );
		AR_UNIT Y = ( _to.y - y );

		// sonador 	1.8.6	덩치큰 몬스터 공격 방향 오류 수정
		if( X == 0.0f && Y == 0.0f )
			return;

		float face1 = ::atan2( Y, X );
		bHasDirectionChanged = true;
		direction = _to;

		SetFace( face1 );
	}

	void SetCurrentXY( AR_UNIT _x, AR_UNIT _y )		{ x = _x; y = _y;		};
	void SetCurrentZ( AR_UNIT _z )					{ z = _z;				};
	void SetCurrentXYZ( AR_UNIT _x, AR_UNIT _y, AR_UNIT _z )		{ x = _x; y = _y; z = _z;		};

	const ArPosition & GetPos() const				{ return *this;			};
	const ArPosition & GetTargetPos() const
	{
		if( ends.size() )
			return ends.back().end;

		return *this;
	}
	DWORD GetTargetPosTime()
	{
		if( ends.size() )
			return ends.back().end_time;

		return 0;
	}
	DWORD GetProcTime()
	{
		return proc_time;
	}
	const std::vector< ArMoveVector::MOVE_INFO > & GetTargetPosList() const
	{
		return ends;
	}

	bool IsMoving()	const							{ return bIsMoving;		};
	bool IsMoving( AR_TIME t )	const				{ return bIsMoving ? ( ends.size() ? ends.back().end_time > t : false ) : false;	};
	void SetMoving( bool bMoving )					{ bIsMoving = bMoving; }


	bool HasDirectionChanged()						{ return bHasDirectionChanged;	};

	const ArPosition & GetDirection()				{ bHasDirectionChanged = false; return direction; };

	void SetCurrentPos( const ArPosition & pos )	{ x = pos.x; y = pos.y; z = pos.z; face = pos.face;	};

	AR_TIME	GetStartTime() const					{ return start_time;	};
	unsigned char GetSpeed() const					{ return speed;			};

	size_t GetWayPointCount() const					{ return ends.size();	};
	void setSpeed(unsigned char _speed)			{	speed = _speed;	}

private:

	bool			bIsMoving;
	bool			bWithZMoving;

	unsigned char	speed;

	AR_TIME			start_time;		// 시작시간
	AR_TIME			proc_time;		// 마지막으로 step 밟은 시간

	std::vector< MOVE_INFO > ends;

	bool			bHasDirectionChanged;
	ArPosition		direction;
};

