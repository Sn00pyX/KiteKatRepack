#pragma once

#include <cstring>
#include <algorithm>

using namespace std;

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef FLOAT
typedef float FLOAT;
#endif

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

#pragma pack(push, 1)

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { if(x) { delete x; x = NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) { if(x) { delete [] x; x = NULL; } }
#endif

#ifndef SAFE_DELETE_VECTOR
#define SAFE_DELETE_VECTOR(x) { for(unsigned int __i__(0); __i__ < x.size(); ++__i__) delete(x.at(__i__)); x.clear(); }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) { if(x) { x->Release(); x = NULL; } }
#endif

struct KPoint
{
	KPoint() { }
	KPoint( int _x, int _y ) : x( _x ), y( _y ) { }

	int x;
	int y;

	bool operator==( const KPoint& r ) const { return (x == r.x && y == r.y); }
	bool operator!=( const KPoint& r ) const { return (x != r.x || y != r.y); }

	KPoint operator+( const KPoint& r ) const { return KPoint( x + r.x, y + r.y ); }
	KPoint operator-( const KPoint& r ) const { return KPoint( x - r.x, y - r.y ); }
};

struct KSize
{
	KSize()	{}
	KSize( int _cx, int _cy ) : cx(_cx), cy(_cy) {}
	union
	{
		int width;
		int cx;
	};
	union
	{
		int height;
		int cy;
	};
};

struct KRect
{
	KRect() {}
	KRect( int _left, int _top, int _right, int _bottom ) : left(_left), top(_top), right(_right), bottom(_bottom) {}
	KRect( const KPoint& rPoint, const KSize& rSize ) : left( rPoint.x ), top( rPoint.y ), right( rPoint.x + rSize.cx ), bottom( rPoint.y + rSize.cy ) { }
	int left;
	int top;
	int right;
	int bottom;

	KRect operator+( const KRect& r ) const { return KRect( left + r.left, top + r.top, right + r.right, bottom + r.bottom ); }
	KRect operator-( const KRect& r ) const { return KRect( left - r.left, top - r.top, right - r.right, bottom - r.bottom ); }
	bool operator==( const KRect& r ) const { return (left == r.left && top == r.top && right == r.right && bottom == r.bottom); }
	bool operator!=( const KRect& r ) const { return (left != r.left || top != r.top || right != r.right || bottom != r.bottom); }

	// Added by EiN - 2003/7/4
	bool IsInRect(int x, int y) const
	{
		if(left <= x && right > x)
		{
			if(top <= y && bottom > y)
				return true;
		}
		return false;		
	}
	// Added by EiN - 2003/9/23
	int GetWidth() const
	{
		return right - left;
	}
	int GetHeight() const
	{
		return bottom - top;
	}

	void Intersect(const KRect& rcRect)
	{
		left	= max(left, rcRect.left);
		top		= max(top, rcRect.top);
		right	= min(right, rcRect.right);
		bottom	= min( bottom, rcRect.bottom	);
	}
	void Union(const KRect& rcRect)
	{
		left	= min(left, rcRect.left);
		top		= min(top, rcRect.top);
		right	= max(right, rcRect.right);
		bottom	= max( bottom, rcRect.bottom	);
	}
};

struct KColor;

struct KTripleColor
{
	KTripleColor() { }
	KTripleColor( const KColor& rColor );
	KTripleColor( unsigned char _r, unsigned char _g, unsigned char _b ) { r = _r; g = _g; b = _b; }

    bool operator==( const KTripleColor& r ) const { return memcmp( this, &r, sizeof(*this) ) == 0; }
    bool operator!=( const KTripleColor& r ) const { return memcmp( this, &r, sizeof(*this) ) != 0; }

	unsigned char b,g,r;
};

struct KColor
{
	KColor()				{}
	KColor( DWORD argb )	{ color = argb; }
	KColor( const KTripleColor& rColor, unsigned char _a = 0 ) { r = rColor.r; g = rColor.g; b = rColor.b; a = _a; }
	KColor( unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a = 0 ) { r = _r; g = _g; b = _b; a = _a; }
	
	bool operator==(const KColor& kColor) const
	{
		return color == kColor.color;
	}
	bool operator < (const struct KColor& kColor) const
	{
		return color < kColor.color;
	}

	union
	{
		struct { unsigned char b,g,r,a; };
		unsigned long color;
	};
};

inline KTripleColor::KTripleColor( const KColor& rColor ) { r = rColor.r; g = rColor.g; b = rColor.b; }

#pragma pack(pop)
