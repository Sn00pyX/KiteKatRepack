#pragma once

#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <cmath>


// Mersenne Twister 
extern void XInitRandom( unsigned seed );
extern int XRandom();
inline int XRandom( int min, int max )
{
	if ( min == max ) return min;

	assert( min < max );
	if( min > max )
	{
		std::swap( min, max );
	}

	return ( XRandom()%(max-min+1) + min );
}

// Error function 위키페이지의 Section 2.2 Inverse functions에서 Approximation with elementary functions 방법 이용
// http://en.wikipedia.org/wiki/Error_function 
inline double XInverseErf( double x )
{
	// 상수들
	const double XRAMDOM_PI = 3.14159265358979;			// 원주율
	const double XRAMDOM_squareTwo = 1.4142135623731;	// 루트 2
	const double XRAMDOM_ALPHA = 0.140012;				// a 상수 (erf inv 근사식에 사용)

	if( x >= 1 || x <= 0 )
		return RAND_MAX;

	double z = 2*x - 1;
	double fConst = 2/XRAMDOM_PI/XRAMDOM_ALPHA;				// 계산을 위한 상수
	double fVar = log( 1 - z*z ) / 2; 		// 계산을 위한 변수

	if( z > 0)
		return XRAMDOM_squareTwo * sqrt( sqrt( ( fConst + fVar )*( fConst+ fVar ) - fVar * 2 / XRAMDOM_ALPHA ) - ( fConst+ fVar ) );
	else 
		return -XRAMDOM_squareTwo * sqrt( sqrt( ( fConst + fVar )*( fConst+ fVar ) - fVar * 2 / XRAMDOM_ALPHA ) - ( fConst+ fVar ) );
}

// 극값이 나올 확률을 extreme으로 정한다. 
// 예) 1%의 확률로 최상옵이 뽑히게 하고싶다 -> extreme = 0.99
// 자동으로 1%확률로 최하옵도 뽑히게 된다.
// maxValue는 extreme의 확률로 일어나는 사건의 한계값을 설정한다.
// 만약 이 확률이 일어날 확률보다 낮은 사건이 일어낫을 경우에는 최대/최소를 넘으므로 다시 랜덤을 돌린다.
inline int XNormalRandom( int min, int max, double extreme )
{
	if ( min == max ) return min;

	double maxValue = XInverseErf( extreme );

	double dResult;
	do
	{
		int nRand = XRandom( 1, RAND_MAX-1 );
		double dProb = ( double )nRand/RAND_MAX;	// 확률 p (0,1)
		dResult = ( min + max )/2.0 + ( XInverseErf( dProb ) / maxValue / 2 ) * ( max-min );
	}
	while( dResult > max || dResult < min );

	return static_cast< int >( dResult + 0.5 ); // 0.5는 반올림을 위함.
}

inline int XFastRandom()
{
	return XRandom();
}	

inline int XFastRandom( int min, int max )
{
	return XRandom( min, max );
}
