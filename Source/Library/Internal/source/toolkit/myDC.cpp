
#include <cstdio>
#include <cstdarg>
#include <cmath>

#include "../../include/toolkit/myDC.h"
#include "../../include/toolkit/safe_function.h"

const float l = 15;		// 선 길이

const float ang = 40;		// 화살표와 본선분과의 각도
const float as = 2;		// 화살표 선분대 본선분의 길이비

myDC::myDC()
{
	m_bIsInitialized = false;
	m_hDC = NULL;

	m_hBitmap = NULL;

	m_hWhiteBrush = NULL;
	m_hBlackBrush = NULL;
	m_hBgBrush = NULL;
	m_hCurrentBrush = NULL;

	m_hBgColor = 0;
	m_hCurrentColor = 0;

	m_hFont = NULL;

	m_hOldBitmap = NULL;
	m_hOldObject = NULL;

	m_hWnd = NULL;
}

myDC::~myDC()
{
	if ( m_bIsInitialized ) DeInit();
}

bool myDC::Init( HWND hWnd )
{
	if ( m_bIsInitialized ) return false;

	m_bIsInitialized = true;

	m_hWnd = hWnd;

	HDC dc = ::GetDC( m_hWnd );

	m_hDC = CreateCompatibleDC( dc );
	m_hBitmap = CreateCompatibleBitmap( dc, 1600, 1200 );

	m_hOldBitmap = SelectObject( m_hDC, m_hBitmap );
	
	ReleaseDC( m_hWnd, dc );

	m_hWhiteBrush	= CreateSolidBrush( RGB( 255,255,255 ) );
	m_hBlackBrush	= CreateSolidBrush( RGB( 0, 0, 0 ) );

	m_hBgColor		= RGB( 255,255,255 );
	m_hCurrentColor	= RGB( 0, 0, 0 );

	m_hBgBrush		= CreateSolidBrush( m_hBgColor );
	m_hCurrentBrush	= CreateSolidBrush( m_hCurrentColor );

	m_hOldObject = SelectObject( m_hDC, m_hCurrentBrush );

	m_hFont = CreateFont( 12, 6, 0, 0, FW_DONTCARE, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "굴림체" );

	SetBkMode( m_hDC, TRANSPARENT );

	return true;
}

bool myDC::DeInit()
{
	if ( !m_bIsInitialized ) return false;

	DeleteObject( m_hWhiteBrush );
	DeleteObject( m_hBlackBrush );
	DeleteObject( m_hBgBrush );
	
	SelectObject( m_hDC, m_hOldBitmap );
	SelectObject( m_hDC, m_hOldObject );
	DeleteObject( m_hCurrentBrush );	// 얘하고

	DeleteObject( m_hFont );

	DeleteObject( m_hBitmap );			// 얘

	DeleteDC( m_hDC );

	m_bIsInitialized = false;

	return true;
}

void myDC::Box( int x, int y, int tx, int ty, COLORREF color, int nFillType )
{	
	HBRUSH newBrush = ::CreateSolidBrush( color );
	HBRUSH oldBrush = m_hCurrentBrush;
	m_hCurrentBrush = newBrush;
	
	COLORREF oldColor = SetDCPenColor( m_hDC, color );
	HGDIOBJ oldObj = SelectObject( m_hDC, newBrush );

	Box( x, y, tx, ty, nFillType );

	SetDCPenColor( m_hDC, oldColor );

	SelectObject( m_hDC, oldObj );
	DeleteObject( newBrush );

	m_hCurrentBrush = oldBrush;
}

void myDC::Polygon( const POINT* p, size_t cnt, bool bIsLine )
{
	if ( !bIsLine )
	{
		COLORREF color = RGB( 150, 150, 150 );

		HBRUSH newBrush = ::CreateSolidBrush( color );
		HBRUSH oldBrush = m_hCurrentBrush;
		m_hCurrentBrush = newBrush;
		
		COLORREF oldColor = SetDCPenColor( m_hDC, color );
		HGDIOBJ oldObj = SelectObject( m_hDC, newBrush );

		::Polygon( GetDC(), p, static_cast<int>( cnt ) );

		SetDCPenColor( m_hDC, oldColor );

		SelectObject( m_hDC, oldObj );
		DeleteObject( newBrush );

		m_hCurrentBrush = oldBrush;
	}
	else
	{
		size_t i;
		for ( i = 0; i < cnt-1; ++i )
		{
			Line( p[i].x, p[i].y, p[i+1].x, p[i+1].y );
		}
		Line( p[0].x, p[0].y, p[cnt-1].x, p[cnt-1].y );
	}
}

void myDC::Box( int x, int y, int tx, int ty, int nFillType )
{
	if ( !m_bIsInitialized ) return;

	if ( nFillType == FILL_SOLID )
	{
		RECT rect = { x,y, tx, ty };
		::FillRect( m_hDC, &rect, m_hCurrentBrush );
	}

	Line( x, y, tx, y );
	Line( tx, y, tx, ty );
	Line( tx, ty, x, ty );
	Line( x, ty, x, y );

	if ( nFillType == FILL_SLASH )
	{
		int q;
		int size = ( tx - x + ty - y );
		for ( q = 0; q < size; q+=3 )
		{
			int _x = x+q;
			int _y = y;
			int _tx = x;
			int _ty = y+q;

			if ( _x > tx )
			{
				_y += ( _x - tx );
				_x = tx;
			}

			if ( _ty > ty )
			{
				_tx += ( _ty - ty );
				_ty = ty;
			}

			Line( _x, _y, _tx, _ty );
		}
	}
}

void myDC::Line( int x, int y, int tx, int ty )
{
	if ( !m_bIsInitialized ) return;

	if ( ( x == tx && y == ty ) || x < 0 || y < 0 || tx < 0 || ty < 0 ) return;

	MoveToEx( m_hDC, x, y, NULL );
	LineTo( m_hDC, tx, ty );
}

void myDC::Pixel( int x, int y, COLORREF color )
{
	::SetPixel( m_hDC, x, y, color );
}


void myDC::SetFont( const char *szFaceName, int nWidth, int nHeight )
{
	DeleteObject( m_hFont );
	m_hFont = CreateFont( 12, 5, 0, 0, FW_DONTCARE, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, "Arial" );
}

void myDC::SetFont( HFONT font )
{
	DeleteObject( m_hFont );
	m_hFont = font;
}

void myDC::SetPenColor( COLORREF color )
{
	SetDCPenColor( m_hDC, color );
}

void myDC::Printf( int x, int y, const char *str, ... )
{
	if ( !m_bIsInitialized ) return;

	char szBuf[1024];
	va_list va;
	va_start( va, str );
	s_vsprintf( szBuf, _countof(szBuf), str, va );
	va_end( va );

	HGDIOBJ oldObj = SelectObject( m_hDC, m_hFont );
	TextOut( m_hDC, x, y, szBuf, (int)strlen( szBuf ) );
	SelectObject( m_hDC, oldObj );

}

void myDC::Clear()
{
	if ( !m_bIsInitialized ) return;

	static RECT rect = { 0,0, 1600,1200 };
	::FillRect( m_hDC, &rect, m_hBgBrush );
}

void myDC::Begin()
{
	HGDIOBJ penObj = GetStockObject(DC_PEN);
	SelectObject( m_hDC, penObj );
	DeleteObject( penObj );
}

void myDC::Update()
{
	if ( !m_bIsInitialized ) return;

	HDC dc = ::GetDC( m_hWnd );

	::BitBlt( dc, 0, 0, 1600, 1200, m_hDC, 0, 0, SRCCOPY );

	ReleaseDC( m_hWnd, dc );
}

float radian( float degree )
{
	return 3.141592f*degree/180.f;
}

void myDC::DrawArrow( int x, int y, float r )
{
	float fx,fy,tx,ty;

	fx = float( x );
	fy = float( y );

	// { x,y 를 중점으로 보정
	tx = fx+cos( r ) * l;
	ty = fy+sin( r ) * l;
	fx -= (tx - fx)/2;
	fy -= (ty - fy)/2;
	// }

	// { 가운데선
	tx = fx+cos( r ) * l;
	ty = fy+sin( r ) * l;
	Line( int( fx ), int( fy ), int( tx ), int( ty ) );
	// }
	

	// { 목표점에서..
	fx = tx;
	fy = ty;
	// }

	// { 왼쪽선을 그리고
	tx = fx+cos( r+10 ) * l/as;
	ty = fy+sin( r+10 ) * l/as;
	Line( int( fx ), int( fy ), int( tx ), int( ty ) );
	// }

	// { 오른쪽선을 그린다
	tx = fx+cos( r-10 ) * l/as;
	ty = fy+sin( r-10 ) * l/as;
	Line( int( fx ), int( fy ), int( tx ), int( ty ) );
	// }
}

void myDC::DrawArrow( int _x, int _y, int _tx, int _ty )
{
	float fx,fy,tx,ty;

	fx = float( _x );
	fy = float( _y );
	tx = float( _tx );
	ty = float( _ty );
	
	float r = atan2( ty-fy, tx-fx );

	// { 가운데선
	Line( int( fx ), int( fy ), int( tx ), int( ty ) );
	// }

	// { 목표점에서..
	fx = tx;
	fy = ty;
	// }

	// { 왼쪽선을 그리고
	tx = fx+cos( r+10 ) * l/as;
	ty = fy+sin( r+10 ) * l/as;
	Line( int( fx ), int( fy ), int( tx ), int( ty ) );
	// }

	// { 오른쪽선을 그린다
	tx = fx+cos( r-10 ) * l/as;
	ty = fy+sin( r-10 ) * l/as;
	Line( int( fx ), int( fy ), int( tx ), int( ty ) );
	// }
}
