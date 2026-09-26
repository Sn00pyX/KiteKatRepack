#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wingdi.h>

const int FILL_EMPTY = 0;
const int FILL_SLASH = 1;
const int FILL_SOLID = 3;

class myDC
{	
public:
	myDC();
	~myDC();

	bool Init( HWND hWnd );
	bool DeInit();

	void Pixel( int x, int y, COLORREF color );
	void Box( int x, int y, int tx, int ty, int nFillType = 0 );
	void Box( int x, int y, int tx, int ty, COLORREF color, int nFillType = 0 );

	void Polygon( const POINT* p, size_t cnt, bool bIsLine = false );

	void Line( int x, int y, int tx, int ty );
	void SetFont( const char *szFaceName, int nWidth, int nHeight );
	void SetFont( HFONT font );
	void Printf( int x, int y, const char *str, ... );
	HDC  GetDC()	{ return m_hDC; }

	void SetPenColor( COLORREF color );

	void DrawArrow( int x, int y, float r );
	void DrawArrow( int x, int y, int tx, int ty );

	void Clear();

	void Begin();
	void Update();

private:

	bool m_bIsInitialized;

	HDC	 m_hDC;

	HBITMAP m_hBitmap;

	HBRUSH	m_hWhiteBrush;
	HBRUSH	m_hBlackBrush;
	HBRUSH	m_hBgBrush;
	HBRUSH	m_hCurrentBrush;

	COLORREF m_hBgColor;
	COLORREF m_hCurrentColor;

	HFONT m_hFont;

	HGDIOBJ m_hOldBitmap;
	HGDIOBJ m_hOldObject;

	HWND m_hWnd;
};
