#pragma once

#include "../toolkit/safe_function.h"

#include <algorithm>
#include <cassert>

struct XAlphaImage;

struct XFreeType
{
	XFreeType();
	~XFreeType();

	bool IsInitialized() const;

	bool Init();
	bool DeInit();

	bool LoadFont( const char *szAlias, const void *szBuffer, size_t nBufferSize );
	bool LoadFont( const char *szAlias, const char *szFilePath );

	enum FONT_EFFECT_TYPE
	{
		FT_PLAIN			= 0,
		FT_BOLD				= 1,
		FT_GROW_LV1			= 2,
		FT_GROW				= 3,
		FT_GROW_LV2			= 3,
		FT_GROW_LV3			= 4,
		FT_SHADOW			= 5,
		FT_SHADOW_LV1		= 5,
		FT_SHADOW_LV2		= 6,
		FT_SHADOW_LV3		= 7,
		FT_OUTLINE			= 8,
		FT_STROKE			= 8,
		FT_STROKE_LV1		= 8,
		FT_STROKE_LV2		= 9,
		FT_STROKE_LV3		= 10,
		FT_STROKE_LV0		= 11,
	};

	int get_x_margin( int nFontSize, int ft );
	int get_y_margin( int nFontSize, int ft );
	int get_font_height( int nFontSize, int ft );
	void set_compatible_mode( bool bFlag );
	bool get_compatible_mode() const;

	const XAlphaImage* Draw( const char *szAlias, const wchar_t* wszString, int* pWidth, int* pHeight, int nFontSize = 11, int ft = FT_PLAIN, int y_offset = -4, bool bUnderline = false, bool bBackGround = false, bool bOnlySize = false, bool bHinting = false )
	{
		size_t len = wcslen( wszString );
		if( !len ) return NULL;

		return Draw( szAlias, wszString, len, pWidth, pHeight, nFontSize, ft, y_offset, bUnderline, bBackGround, bOnlySize, bHinting );
	}

	const XAlphaImage* Draw( const char *szAlias, const wchar_t * wszString, size_t len, int * pWidth, int * pHeight, int nFontSize = 11, int ft = FT_PLAIN, int y_offset = -4, bool bUnderline = false, bool bBackGround = false, bool bOnlySize = false, bool bHinting = false );

	// 2010.07.20 - prodongi
	const bool GetSize( const char *szAlias, const wchar_t* wszString, int* pWidth, int* pHeight, int nFontSize = 11, bool bHinting = false )
	//const bool GetSize( const char *szAlias, const wchar_t* wszString, int* pWidth, int* pHeight, int nFontSize = 11 )
	{
		Draw( szAlias, wszString, pWidth, pHeight, nFontSize, FT_PLAIN, 0, false, false, true, bHinting );
		return true;
	}

	void	SetFontColor( int r, int g, int b, bool bAdjustEffectColor = true );
	void	SetBackGroundColor( int r, int g, int b );
	void	SetEffectColor( int r, int g, int b );

	int CalcCarretPos( const char *szAlias, int nFontSize, int ft, const wchar_t *wszString, int x, bool bHinting = false );

private:

	XFreeType( const XFreeType& );
	XFreeType& operator=( const XFreeType& );

	struct Imp;
	Imp* m_pImpl;
};




struct XAlphaImage
{
	typedef unsigned long COLOR;

	enum {
		B     = 256,
		DEPTH = 256,
		MAX_VALUE	  = (DEPTH-1),
	};

	struct Pixel
	{
		Pixel() {}
		Pixel( COLOR _c ) : c( _c ) {}

		union 
		{
			COLOR  c;
			struct RGBA
			{
				unsigned char r;
				unsigned char g;
				unsigned char b;
				unsigned char a;
			} rgba;
		};
	};

	XAlphaImage( size_t width, size_t height, COLOR color = 0 ) : m_nWidth( width ) , m_nHeight( height )
	{
		m_nBufferSize = width*height;
		pBuffer = new Pixel[ m_nBufferSize ];

		memset( pBuffer, 0, sizeof(Pixel)*m_nBufferSize );
	}

	virtual ~XAlphaImage()
	{
		delete [] pBuffer;
	}

	XAlphaImage( const XAlphaImage & rh )
	{
		m_nWidth	= rh.m_nWidth;
		m_nHeight	= rh.m_nHeight;
		m_nBufferSize		= rh.m_nBufferSize;
		pBuffer = new Pixel[ m_nBufferSize ];
		s_memcpy( pBuffer, m_nBufferSize*sizeof( Pixel ), rh.pBuffer, m_nBufferSize*sizeof(Pixel) ); 
	}

	XAlphaImage & operator=( const XAlphaImage & rh )
	{
		m_nWidth	= rh.m_nWidth;
		m_nHeight	= rh.m_nHeight;

		if( m_nBufferSize < rh.m_nBufferSize )
		{
			delete [] pBuffer;
			pBuffer = new Pixel[ rh.m_nBufferSize ];
			m_nBufferSize		= rh.m_nBufferSize;
		}
		
		s_memcpy( pBuffer, rh.m_nBufferSize*sizeof(Pixel), rh.pBuffer, rh.m_nBufferSize*sizeof(Pixel) ); 

		return *this;
	}

	void Contrast()
	{
		for( size_t x = 0; x < m_nWidth; x++ )
		{
			for( size_t y = 0; y < m_nHeight; y++ )
			{
				if( !pBuffer[ x + y * m_nWidth ].rgba.a ) continue;
				
				int i = pBuffer[ x + y * m_nWidth ].rgba.a;
				
				if( i > 155 ) i = int( i * 1.1f );

				if( i > MAX_VALUE ) i = MAX_VALUE;
				pBuffer[ x + y * m_nWidth ].rgba.a = static_cast< unsigned char >( i );
			}
		}
	}

	inline void LowpassFilter( unsigned char min )
	{
		for( size_t x = 0; x < m_nWidth; x++ )
		{
			for( size_t y = 0; y < m_nHeight; y++ )
			{
				if( pBuffer[ x + y * m_nWidth].rgba.a < min ) pBuffer[ x + y * m_nWidth ].rgba.a = 0;
			}
		}
	}

	void MultiplyAlpha( float f )
	{
		for( size_t x = 0; x < m_nWidth; x++ )
		{
			for( size_t y = 0; y < m_nHeight; y++ )
			{
				if( !pBuffer[ x + y * m_nWidth ].rgba.a ) continue;

				int i = int( pBuffer[ x + y * m_nWidth ].rgba.a * f );
				if( i > MAX_VALUE ) i = MAX_VALUE;
				pBuffer[ x + y * m_nWidth ].rgba.a = static_cast< unsigned char >( i );
			}
		}
	}

	void Clear()
	{
		for( size_t x = 0; x < m_nWidth; x++ )
		{
			for( size_t y = 0; y < m_nHeight; y++ )
			{
				pBuffer[ x + y * m_nWidth ].rgba.a = 0;
			}
		}
	}

	// 알파를 번지게 함. strength 는 0 ~ 10 사이의 값.
	// 한번 호출에 1pixel 씩 번진다. 자연스럽게 하려면 작은 strength 로 여러번 콜해야함.
	void GrowAlpha( unsigned int strength )
	{
		if( !strength ) return;

		size_t size = m_nWidth*m_nHeight;
		Pixel *pTmpBuffer = new Pixel[ size ];
		s_memcpy( pTmpBuffer, sizeof(Pixel)*size, pBuffer, sizeof(Pixel)*size );

		if( strength > 10 ) strength = 10;

		strength = 11 - strength;
		if( strength > 8 ) strength *= 5;

		for( size_t x = 0; x < m_nWidth; x++ )
		{
			for( size_t y = 0; y < m_nHeight; y++ )
			{
				pTmpBuffer[ x + y * m_nWidth ].rgba.a = GetAlphaAvr( x, y, strength );
			}
		}

		for( size_t x = 0; x < m_nWidth; x++ )
		{
			for( size_t y = 0; y < m_nHeight; y++ )
			{
				pBuffer[ x + y * m_nWidth ].rgba.a = pTmpBuffer[ x+y*m_nWidth].rgba.a;
			}
		}

		delete [] pTmpBuffer;
	}

	void FillColor( unsigned char r, unsigned char g, unsigned char b, unsigned char alpha = 0 )
	{
		for( size_t x = 0; x < m_nWidth; x++ )
		{
			for( size_t y = 0; y < m_nHeight; y++ )
			{
				//if( pBuffer[ x + y * m_nWidth ].a )
				{
					pBuffer[ x + y * m_nWidth ].rgba.r = (unsigned char )( r * B / 256 );
					pBuffer[ x + y * m_nWidth ].rgba.g = (unsigned char )( g * B / 256 );
					pBuffer[ x + y * m_nWidth ].rgba.b = (unsigned char )( b * B / 256 );
					if( alpha ) pBuffer[ x + y * m_nWidth ].rgba.a = (unsigned char )( alpha * B / 256 );
				}
			}
		}
	}

	Pixel& rget( const size_t & x, const size_t & y )
	{
		static Pixel dummy;
		if( x >= m_nWidth ) return dummy;
		if( y >= m_nHeight ) return dummy;
		return pBuffer[ x + y * m_nWidth ];
	}

	unsigned char GetAlphaAvr( const size_t & x, const size_t & y, const int & strength )
	{
		if( rget( x  , y   ).rgba.a == (MAX_VALUE) ) return (MAX_VALUE);

		int nR  = rget( x-1, y-1 ).rgba.a;
		    nR += rget( x  , y-1 ).rgba.a;
		    nR += rget( x+1, y-1 ).rgba.a;
		    nR += rget( x-1, y   ).rgba.a;
		    nR += rget( x+1, y   ).rgba.a;
		    nR += rget( x-1, y+1 ).rgba.a;
		    nR += rget( x  , y+1 ).rgba.a;
		    nR += rget( x+1, y+1 ).rgba.a;

		nR = nR/strength;

		nR += rget( x  , y   ).rgba.a;
		
		if( nR > (MAX_VALUE) ) return (MAX_VALUE);
		return static_cast< unsigned char >( nR );
	}

	// 내 밑에 rh 를 갖다붙인 결과를 만든다.
	void Append( const XAlphaImage & rh, int xm = 0, int ym = 0, int nThresold = 9 )
	{
		for( int x = 0; x < (int)m_nWidth; x++ )
		{
			for( int y = 0; y < (int)m_nHeight; y++ )
			{
				if( x + xm < 0 || x + xm >= (int)rh.m_nWidth ) continue;
				if( y + ym < 0 || y + ym >= (int)rh.m_nHeight ) continue;

				Pixel & pl = pBuffer[ x + y * m_nWidth ];
				Pixel pr = rh.pBuffer[ x+xm + (y+ym) * m_nWidth ];
				
				if( pl.rgba.a == (MAX_VALUE) ) continue;	// 내것이 불투명이면 pass
				if( pr.rgba.a == 0	) continue;	// 저쪽이 완전히 투명하면 pass
				//if( pl.a >= nThresold	) continue;

				// 최종 알파는 두개의 알파를 합친 값.
				int a = pl.rgba.a + pr.rgba.a;
				if( a > MAX_VALUE ) 
				{
					pr.rgba.a = MAX_VALUE - pl.rgba.a;
					a = MAX_VALUE;
					
				}

				// 내 비율 :     pl.a / (MAX_VALUE)
				// 니 비율 :     ( (MAX_VALUE) - pl.a) * pr.a / (MAX_VALUE)*(MAX_VALUE) )


				int r = ( pl.rgba.r * pl.rgba.a ) + ( pr.rgba.r * pr.rgba.a );
				int g = ( pl.rgba.g * pl.rgba.a ) + ( pr.rgba.g * pr.rgba.a );
				int b = ( pl.rgba.b * pl.rgba.a ) + ( pr.rgba.b * pr.rgba.a );
				
				//pl.a = a;
				pl.rgba.r = static_cast< unsigned char >( r / (pl.rgba.a+pr.rgba.a) );
				pl.rgba.g = static_cast< unsigned char >( g / (pl.rgba.a+pr.rgba.a) );
				pl.rgba.b = static_cast< unsigned char >( b / (pl.rgba.a+pr.rgba.a) );
				pl.rgba.a = static_cast< unsigned char >( a );
			}
		}
	}

	Pixel* get( const size_t & x, const size_t & y ) { assert( x < m_nWidth ); assert( y < m_nHeight ); return &pBuffer[ x + y * m_nWidth ]; }
	const Pixel* cget( const size_t & x, const size_t & y ) const { assert( x < m_nWidth ); assert( y < m_nHeight ); return &pBuffer[ x + y * m_nWidth ]; }

	void SetColor( const size_t & x, const size_t & y, const int & r, const int & g, const int & b )
	{
		Pixel *p = get( x, y );
		p->rgba.r = static_cast< unsigned char >( ( r * B / 256 ) );
		p->rgba.g = static_cast< unsigned char >( ( g * B / 256 ) );
		p->rgba.b = static_cast< unsigned char >( ( b * B / 256 ) );
	}
	void SetAlpha( const size_t & x, const size_t & y, const int & a )	{ get( x, y )->rgba.a = static_cast< unsigned char >( ( a * B / 256 ) );	}

	const unsigned char GetAlpha( const size_t & x, const size_t & y ) const 	{ return static_cast< unsigned char >( cget( x, y )->rgba.a * 256 / B );	}
	const unsigned char GetRed( const size_t & x, const size_t & y ) const 		{ return static_cast< unsigned char >( cget( x, y )->rgba.r * 256 / B );	}
	const unsigned char GetGreen( const size_t & x, const size_t & y ) const 	{ return static_cast< unsigned char >( cget( x, y )->rgba.g * 256 / B );	}
	const unsigned char GetBlue( const size_t & x, const size_t & y ) const		{ return static_cast< unsigned char >( cget( x, y )->rgba.b * 256 / B );	}

	const size_t GetWidth() const { return m_nWidth; }
	const size_t GetHeight() const { return m_nHeight; }

	const Pixel* GetBuffer() const { return pBuffer; }

	void SetSize( size_t width, size_t height ) { if( m_nBufferSize < width*height ) throw "ERROR";  m_nWidth = width; m_nHeight = height; }

private:

	size_t m_nWidth;
	size_t m_nHeight;
	size_t m_nBufferSize;

	Pixel* pBuffer;
};
