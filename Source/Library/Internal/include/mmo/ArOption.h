#pragma once

#include <algorithm>

#include "ArType.h"


// 이동속도의 기본 단위는 unsigned char 이다.
// 255배 이상의 정밀도는 필요치않기에, SPEED_UNIT 을 이용해 scale 을 조절한다.
const int		SPEED_UNIT	=	30;

// 한 region 의 길이를 정의한다.
extern int		g_nRegionSize;

extern volatile bool		g_bUseRegionDebug;

// 최대 layer 를 정의한다.
const unsigned	MAX_LAYER				= 256;

// 현재 region 기준으로 몇 블럭 떨어진 것까지 보이는지..
const int		VISIBLE_REGION_RANGE		= 3;
const int		VISIBLE_REGION_BOX_WIDTH	= (VISIBLE_REGION_RANGE*2+1);

// 이동중 이 AR_TIME 이상 지나치지 않는 스쳐가는 region 이라면 무시한다.
const int		IGNOREABLE_VISIT_TIME		= 10;

// 시야 판정 정보를 담고있는 2차원 boolean matrix
extern const int s_Matrix[VISIBLE_REGION_BOX_WIDTH][VISIBLE_REGION_BOX_WIDTH];

inline unsigned GetRegionX( AR_UNIT x )		{ return (unsigned)(x / g_nRegionSize); }
inline unsigned GetRegionY( AR_UNIT y )		{ return (unsigned)(y / g_nRegionSize); }

// rx, ry 의 관심영역 내에 _rx, _ry 가 들어가는지를 판정한다.
// 시야 판별의 핵심 함수임.
inline int IsVisibleRegion_Fast( unsigned rx, unsigned ry, unsigned _rx, unsigned _ry )
{
	return s_Matrix[VISIBLE_REGION_RANGE + _rx - rx][VISIBLE_REGION_RANGE + _ry - ry];
}

// 범위체크를 추가한 버젼.
inline int IsVisibleRegion( unsigned rx, unsigned ry, unsigned _rx, unsigned _ry )
{
	int nx = VISIBLE_REGION_RANGE + _rx - rx;
	int ny = VISIBLE_REGION_RANGE + _ry - ry;

	if( nx < 0 || nx >= VISIBLE_REGION_BOX_WIDTH ) return false;
	if( ny < 0 || ny >= VISIBLE_REGION_BOX_WIDTH ) return false;

	return s_Matrix[nx][ny];
}





// 이하 함수들은 IsVisibleRegion 을 이용해 구현되었기 때문에 수정할 필요 없음.

inline int IsVisiblePoint( unsigned rx, unsigned ry, AR_UNIT x, AR_UNIT y )
{
	return IsVisibleRegion( rx, ry, GetRegionX( x ), GetRegionY( y ) );
}

struct _RegionFunctor
{
	virtual void operator()( unsigned rx, unsigned ry ) const = 0;
};

inline void DoEachVisibleRegion( unsigned rx, unsigned ry, const _RegionFunctor & Functor )
{
	int tx = rx + VISIBLE_REGION_RANGE;
	int ty = ry + VISIBLE_REGION_RANGE;

	int fx = (std::max)( (int)rx - VISIBLE_REGION_RANGE, 0 );
	int fy = (std::max)( (int)ry - VISIBLE_REGION_RANGE, 0 );

	for( int x = fx; x <= tx; x++ )
	{
		for( int y = fy; y <= ty; y++ )
		{
			if( IsVisibleRegion( rx, ry, x, y ) )
			{
				Functor( (unsigned)x, (unsigned)y );
			}
		}
	}
}

