// Time.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <cstring>
#include <cstdio>
#include <cassert>
#include <ctime>

#include <vector>

#include <Windows.h>

#include "../../include/geometry/X2DPolygon.h"
#include "../../include/geometry/X2DQuadTree_divide_polygon.h"
#include "../../include/geometry/X2DPathFinder.h"
#include "../../include/toolkit/XFileUtil.h"

#include "../../include/kfile/IFileSystem.h"

#include <internal_lib_include.h>

bool	IsCheckPolygon( X2D::Point< int >* point, X2D::Polygon<int>* polygon, size_t count )
{
	assert( count == polygon->Size() );
	if( count != polygon->Size() )
	{
		return false;
	}

	for( size_t i = 0; i < count; ++i )
	{
		assert( point[i] == polygon->GetPoint( i ) );
		if( point[i] != polygon->GetPoint( i ) )
		{
			return false;
		}
	}

	return true;
}

int g_nMapWidth	 = 700000;
int g_nMapHeight = 1000000;

struct	polygon_deleter
{
	void	operator()( X2D::Polygon<int>* p )
	{
		delete p;
	}
};

struct	polygon_allocator
{
	X2D::Polygon<int>*	operator()()
	{
		return new X2D::Polygon< int >();
	}
};

X2D::QuadTree_divide_polygon< int, X2D::Polygon<int>*, 10 >			g_qtBlockInfo( 0, 0, g_nMapWidth, g_nMapHeight );

bool LoadAttributeFile( const char * szFileName, int x, int y )
{
	if( !XFileUtil::IsFile( szFileName ) )
		return false;

	float fAttrLen = 5.25;
	float fMapLength = 16128;

	std::auto_ptr< KStream > pStream( new KFileStream( szFileName ) );

	int nPolygonCnt = 0;

	pStream->Read( &nPolygonCnt, sizeof( nPolygonCnt ) );

	X2D::Polygon< int > block_info;

	for( int i = 0; i < nPolygonCnt; ++i )
	{
		int nPtCnt = 0;
		pStream->Read( &nPtCnt, sizeof( nPtCnt ) );

		if( nPtCnt < 3 ) continue;

		X2D::Point<int> * pPoints = new X2D::Point<int>[nPtCnt];

		pStream->Read( pPoints, sizeof( X2D::Point<int> ) * nPtCnt );

		block_info.Clear();

		for( int nPointNum = 0; nPointNum < nPtCnt; ++nPointNum )
		{
			pPoints[nPointNum].x = fMapLength * x + pPoints[nPointNum].x * fAttrLen;
			pPoints[nPointNum].y = fMapLength * y + pPoints[nPointNum].y * fAttrLen;
		}

		if( block_info.Set( pPoints, pPoints + nPtCnt ) )
		{
			X2D::Polygon< int > * pBlock_Info = new X2D::Polygon< int > ( block_info );

			g_qtBlockInfo.Add( pBlock_Info, polygon_allocator(), polygon_deleter() );
		}

		delete [] pPoints;
	}

	return true;
}


bool IsBlocked( int x, int y )
{
	X2D::Point< int > pt( x, y );

	if( g_qtBlockInfo.LooseCollision( pt ) )
	{
		return true;
	}

	return false;
}

void GetCollisionPolygons( int x1, int y1, int x2, int y2, std::vector<X2D::Polygon<int> *> & vPolygons )
{
	X2D::Point<int> ptStart( x1, y1 ), ptEnd( x2, y2 );
	X2D::Box<int> boxRegion( ptStart, ptEnd );

	g_qtBlockInfo.Enum( boxRegion, &vPolygons );
}

bool FindPath( int x1, int y1, int x2, int y2 )
{
	std::vector< X2D::Polygon<int>* > pathFindPolygons;

	GetCollisionPolygons( x1, y1, x2, y2, pathFindPolygons );
	if( pathFindPolygons.empty() )
	{
		return true;
	}

	typedef std::vector< X2D::Point<int> > PATH;
	PATH vPath;

	X2D::Point< int > ptStart( x1, y1 ), ptEnd( x2, y2 );

	// 최대 길찾기를 통하여 갈 수 있는 거리는 가시거리 * 2이다. (적절한 파라미터 테스트 중)
	X2D::PathFinder< int, X2D::Polygon<int>* >::Find( pathFindPolygons.begin(), pathFindPolygons.end(), ptStart, ptEnd, vPath, 525 * 2 );
	if( vPath.empty() )
	{
		return false;
	}

	return true;
}

bool CollisionToLine( int x1, int y1, int x2, int y2 )
{
	X2D::Line<int> line( x1, y1, x2, y2 );

	if( g_qtBlockInfo.LooseCollision( line ) == false )
	{
		return false;
	}

	return true;
}


int _tmain(int argc, _TCHAR* argv[])
{
	// 문제 사항 테스트
	{
		// 
		X2D::QuadTree_divide_polygon< int, X2D::Polygon<int>*, 1, 2 > qtBlockInfo2( 0, 0, 100, 100 );

		X2D::Point<int> pPoints[6];
		X2D::Polygon< int >* temp = new X2D::Polygon< int >();

		pPoints[0].x = 50;
		pPoints[0].y = 70;

		pPoints[1].x = 10;
		pPoints[1].y = 10;

		pPoints[2].x = 50;
		pPoints[2].y = 10;

		pPoints[3].x = 50;
		pPoints[3].y = 30;

		pPoints[4].x = 90;
		pPoints[4].y = 90;

		pPoints[5].x = 50;
		pPoints[5].y = 90;
				
		temp->Set( pPoints, pPoints + _countof( pPoints ) );
		qtBlockInfo2.Add( temp, polygon_allocator(), polygon_deleter() );
	}



	// 맵 파일에서 실제 충돌 폴리곤을 읽어보자.
	// map 폴더에 충돌 파일(.nfa)이 있어야 한다.
 	for( int y = 0; y < 16; ++y )
 	{
 		for( int x = 0; x < 14; ++x )
 		{
 			char file_name[MAX_PATH] = "";
 			sprintf_s( file_name, _countof( file_name ), "map/M%03d_%03d.nfa", x, y );
 			if( LoadAttributeFile( file_name, x, y ) == true )
 			{
 				printf( "%s map file loaded.\n", file_name );
 			}
 		}
 	}

	X2D::Point<int> ptStart( 0, 0 ), ptEnd( 700000, 1000000 );
	X2D::Box<int> boxRegion( ptStart, ptEnd );
	std::vector<X2D::Polygon<int> *> vPolygons;
	g_qtBlockInfo.Enum( boxRegion, &vPolygons );

	size_t max_size = 0;
	X2D::Polygon<int>* p_m = NULL;
	for( auto pos = vPolygons.begin(); pos != vPolygons.end(); ++pos )
	{
		X2D::Polygon<int>* p = (*pos);
		if( p->Size() > max_size )
		{
			max_size = p->Size();
			p_m = p;
		}
	}

	IsBlocked(169960,10568);

	int x = g_qtBlockInfo.GetX();
	int y = g_qtBlockInfo.GetY();
	int w = g_qtBlockInfo.GetWidth();
	int h = g_qtBlockInfo.GetWidth();
	X2D::Rect<int> rc(x, y, w, h);

	g_qtBlockInfo.Remove( rc, polygon_deleter() );
	assert(g_qtBlockInfo.getItemCount() == 0);

	// 일부 라인이 겹칠때
	{
		X2D::Point<int> pPoints[6];
		pPoints[0].x = 10;
		pPoints[0].y = 10;

		pPoints[1].x = 50;
		pPoints[1].y = 10;

		pPoints[2].x = 50;	// in
		pPoints[2].y = 20;

		pPoints[3].x = 70;
		pPoints[3].y = 20;

		pPoints[4].x = 70;
		pPoints[4].y = 30;

		pPoints[5].x = 40;
		pPoints[5].y = 30;

		X2D::Polygon< int > temp;
		temp.Set( pPoints, pPoints + _countof( pPoints ) );

		X2D::Line< int > center( 50, 0, 50, 100 );
		static std::vector< X2D::Polygon<int>* > r =
			X2D::QuadTree_divide_polygon< int, X2D::Polygon<int>*, 1 >::divide_polygon( &temp, center, polygon_allocator(), polygon_deleter() );

		X2D::Point< int > result_pt1[5];
		result_pt1[0].Set( 10, 10 );
		result_pt1[1].Set( 50, 10 );
		result_pt1[2].Set( 50, 20 );
		result_pt1[3].Set( 50, 30 );
		result_pt1[4].Set( 40, 30 );

		X2D::Point< int > result_pt2[4];
		result_pt2[0].Set( 50, 20 );
		result_pt2[1].Set( 70, 20 );
		result_pt2[2].Set( 70, 30 );
		result_pt2[3].Set( 50, 30 );

		assert( r.size() == 2 );
		assert( IsCheckPolygon( result_pt1, r[0], _countof( result_pt1 ) ) );
		assert( IsCheckPolygon( result_pt2, r[1], _countof( result_pt2 ) ) );
	}

	{
		X2D::Point<int> pPoints[4];
		pPoints[0].x = 0;
		pPoints[0].y = 0;

		pPoints[1].x = 100;
		pPoints[1].y = 0;

		pPoints[2].x = 100;	// in
		pPoints[2].y = 100;

		pPoints[3].x = 0;
		pPoints[3].y = 100;

		X2D::Polygon< int > temp;
		temp.Set( pPoints, pPoints + _countof( pPoints ) );

		X2D::Line< int > center( 50, 0, 50, 100 );
		static std::vector< X2D::Polygon<int>* > r =
			X2D::QuadTree_divide_polygon< int, X2D::Polygon<int>*, 1 >::divide_polygon( &temp, center, polygon_allocator(), polygon_deleter() );

		assert( r.size() == 2 );

		int a = 0;
	}


	// 
	X2D::QuadTree_divide_polygon< int, X2D::Polygon<int>*, 1, 2 > qtBlockInfo2( 0, 0, 100, 100 );

	X2D::Point<int> pPoints[12];
	X2D::Polygon< int >* temp = new X2D::Polygon< int >();

	// 대충 ㄹ모양임 (100,100 좌표에 점 12개 ㄹ 모양으로)
	pPoints[0].x = 1;
	pPoints[0].y = 99;

	pPoints[1].x = 99;
	pPoints[1].y = 99;

	pPoints[2].x = 99;
	pPoints[2].y = 40;

	pPoints[3].x = 20;
	pPoints[3].y = 40;

	pPoints[4].x = 20;
	pPoints[4].y = 20;

	pPoints[5].x = 99;
	pPoints[5].y = 20;

	pPoints[6].x = 99;
	pPoints[6].y = 1;

	pPoints[7].x = 1;
	pPoints[7].y = 1;

	pPoints[8].x = 1;
	pPoints[8].y = 60;

	pPoints[9].x = 80;
	pPoints[9].y = 60;

	pPoints[10].x = 80;
	pPoints[10].y = 80;

	pPoints[11].x = 1;
	pPoints[11].y = 80;

	temp->Set( pPoints, pPoints + _countof( pPoints ) );
	qtBlockInfo2.Add( temp, polygon_allocator(), polygon_deleter() );

	return 0;
}

