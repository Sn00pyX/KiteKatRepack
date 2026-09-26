#pragma once

#include "ArMove.h"
#include "ArOption.h"
#include "ArSchedulerObject.h"

#include "../logging/FileLog.h"

// 위치를 가지고 있는 모든 오브젝트의 근간
// 기본적인 위치와 타입, uid 를 가지고 있다.
struct ArObject : public ArSchedulerObject
{
	enum ObjectType { STATIC_OBJECT, MOVABLE_OBJECT, CLIENT_OBJECT };

	ObjectType				GetObjectType() const { return static_cast< ObjectType >( m_nArObjectType ); }
	
	int						region_index;	// Region 의 컨테이너중 몇번째에 위치하는지 기록. (사용시 빨리 접근하기 위해)

	ArObject( ObjectType t = STATIC_OBJECT )
		: m_nArObjectType( static_cast< unsigned char >( t ) )
		, pTag( NULL )
		, region_index( -1 )
		, bIsNearClient( false )
		, bIsDeleted( false )
		, layer( 0 )
		, bIsInWorld( false )
	{
		prev_rx = prev_ry = -1;
		bIsRegionChanging = false;

		nListIndex = nActiveIndex = 0;

		pRegion = NULL;

		lastStepTime = 0;
	}

	inline AR_UNIT	GetX() const					{ return mv.GetX();				};
	inline AR_UNIT	GetY() const					{ return mv.GetY();				};
	inline AR_UNIT	GetZ() const					{ return mv.GetZ();				};
	inline unsigned char GetLayer() const			{ return layer;			};
	void AddNoise( int r1, int r2, int v )
	{	
		AR_UNIT prev_x = mv.x;
		AR_UNIT prev_y = mv.y;

		unsigned prev_rx = mv.GetRX();
		unsigned prev_ry = mv.GetRY();

		mv.x += ( r1%v - v/2 );
		mv.y += ( r2%v - v/2 );

		if( mv.GetRX() != prev_rx ) mv.x = prev_x;
		if( mv.GetRY() != prev_ry ) mv.y = prev_y;
	}

	inline const ArPosition & GetPos() const		{ return mv.GetPos();			};
	inline const ArMoveVector & GetMv() const		{ return mv;					};
	inline unsigned	GetRX()	const					{ return mv.GetRX();			};
	inline unsigned	GetRY()	const					{ return mv.GetRY();			};

	ArPosition		GetCurrentPosition( AR_TIME t ) const
	{
		if( !IsMoving() ) return GetPos();

		ArMoveVector _mv = mv;

		_mv.Step( t );
		return _mv.GetPos();
	}
	
	/*
	virtual void SetCurrentXYZ( AR_UNIT x, AR_UNIT y, AR_UNIT z )
	{
		mv.SetCurrentXYZ( x, y, z );
	}
	void SetCurrentLayer( unsigned char layer )
	{
		mv.SetCurrentLayer( layer );
	}*/

	inline void SetTag( void *p )					{ pTag = p;						};
	inline void* GetTag()							{ return pTag;					};

	inline bool IsMoving() const					{ return ( mv.IsMoving() && IsInWorld() );	};
	inline bool IsMoving( AR_TIME t ) const			{ return mv.IsMoving( t );		};

	inline bool IsInWorld() const					{ return bIsInWorld;			};

	// 시간 t 만큼 이동한다.
	bool Step( AR_TIME t )
	{
		bool bRtn = mv.Step( t );
		if( bRtn )
		{
			mv.SetCurrentPos( mv.GetTargetPos() );
		}

		lastStepTime = t;

		return bRtn;
	}

	// 이동 세팅
	void SetMove( const ArPosition & _to, unsigned char _speed, AR_TIME _start_time = 0 )
	{
		mv.SetMove( _to, _speed, _start_time, GetArTime() );
		lastStepTime = mv.GetStartTime();
	}

	void SetMultipleMove( const std::vector< ArPosition > & _to, unsigned char _speed, AR_TIME _start_time = 0 )
	{
		mv.SetMultipleMove( _to, _speed, _start_time, GetArTime() );
		lastStepTime = mv.GetStartTime();
	}

	// 이동 세팅
	void SetMove( AR_UNIT _x, AR_UNIT _y, unsigned char _speed, AR_TIME _start_time = 0 )
	{
		mv.SetMove( ArPosition( _x, _y ), _speed, _start_time, GetArTime() );
		lastStepTime = mv.GetStartTime();
	}

	void StopMove()
	{
		mv.StopMove();
	}

	float	GetFace() const		{ return mv.GetFace();			}
	void	SetFace( float face )	{ mv.SetFace( face ); }

	AR_UNIT	GetTX() const		{ return mv.GetTargetPos().x;	}
	AR_UNIT	GetTY() const		{ return mv.GetTargetPos().y;	}
	AR_UNIT	GetTZ() const		{ return mv.GetTargetPos().z;	}

	const std::vector< ArMoveVector::MOVE_INFO > & GetTargetPosList() const	{ return mv.GetTargetPosList();	}
	const ArPosition & GetTargetPos() const	{ return mv.GetTargetPos(); }

	unsigned char GetSpeed() const	{ return mv.GetSpeed();	}
	
	AR_TIME	GetStartTime() const	{ return mv.GetStartTime();	}

	void SetDirection( const ArPosition & _to ) { mv.SetDirection( _to ); }

	bool IsLookingRegion( unsigned rx, unsigned ry ) const
	{
		if( IsVisibleRegion( GetRX(), GetRY(), rx, ry ) ) return true;
		if( IsVisibleRegion( prev_rx, prev_ry, rx, ry ) ) return true;

		return false;
	}

	void SetCurrentXY( AR_UNIT x, AR_UNIT y )
	{
		if( prev_rx > -1 )
		{
			prev_rx = mv.GetRX();
			prev_ry = mv.GetRY();
		}

		mv.SetCurrentXY( x, y );
	}

	void SetCurrentZ( AR_UNIT z )	{ mv.z = z; }
	void SetCurrentLayer( unsigned char _layer )	{ layer = _layer; }

	volatile bool		bIsInWorld;
	volatile bool		bIsNearClient;			// 근처에 플레이어가 있을수 있다는 의미. false 라면 명백히 플레이어 없음.

	int		prev_rx, prev_ry;
	bool	bIsRegionChanging;
	AR_TIME	lastStepTime;

	struct ArRegion * pRegion;

	size_t nListIndex;
	size_t nActiveIndex;

protected:

	virtual ~ArObject()
	{
		assert( !bIsDeleted );
		bIsDeleted = true;
	}
	
	unsigned char		m_nArObjectType;
	void *				pTag;
	ArMoveVector		mv;
	unsigned char		layer;
	bool				bIsDeleted;
};

// 오브젝트에 뭔가를 하기 위한 기본 Functor. 템플릿이 아니라 순수가상함수임.
struct ArObjectFunctor
{
	virtual void operator()( ArObject *pObj ) const	= 0;
};