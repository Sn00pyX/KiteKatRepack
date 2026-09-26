
#include "../../include/renderer/XFreeType.h"
#include <vector>
#include <ft2build.h>

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include FT_FREETYPE_H
#include FT_TRUETYPE_IDS_H

struct XFreeType::Imp
{
	Imp()
	{
		m_bCompatibleMode = true;
		m_library = NULL;

		m_FontRed = m_FontGreen = m_FontBlue = 0;
		m_BackRed = m_BackGreen = m_BackBlue = 0;
		m_EffectRed = m_EffectGreen = m_EffectBlue = 0;
	};

	void set_compatible_mode( bool bFlag )
	{
		m_bCompatibleMode = bFlag;
	}

	bool get_compatible_mode() const
	{
		return m_bCompatibleMode;
	}

	bool IsInitialized() const { return m_library != NULL; }

	bool Init()
	{
		m_FontRed=m_FontGreen=m_FontBlue=255;
		m_BackRed=m_BackGreen=m_BackBlue=200;
		m_EffectRed=0;
		m_EffectGreen=0;
		m_EffectBlue=0;

		if( FT_Init_FreeType( &m_library ) ) return false;

		return true;
	}

	bool DeInit()
	{
		if( !m_library ) return false;

		for( std::vector< XAlphaImage* >::iterator it =  m_vImageList.begin(); it != m_vImageList.end(); ++it )
		{
			delete (*it);
		}

		for(std::vector< FaceTag* >::iterator it = m_vFaceList.begin(); it != m_vFaceList.end(); ++it )
		{
			FT_Done_Face( (*it)->face );
			delete (*it);
		}

		if( FT_Done_FreeType( m_library ) ) return false;

		m_library = NULL;

		return true;
	};

	bool selecUnicodeCharmap( FT_Face face )
	{
		for( int i = 0; i < face->num_charmaps; ++i )
		{
			if( face->charmaps[i]->encoding == FT_ENCODING_UNICODE )
			{
				if( FT_Select_Charmap( face, face->charmaps[i]->encoding ) ) return false;
				return true;
			}
		}
		return false;
	}


	bool LoadFont( const char *szAlias, const void *szBuffer, size_t nBufferSize )
	{
		FT_Face face;

		if( FT_New_Memory_Face( m_library, (FT_Byte*)szBuffer, (FT_Long)nBufferSize,  0, &face ) )
		{	
			return false;
		}

		FaceTag *pTag = new FaceTag;
		pTag->bCharsetSelected = selecUnicodeCharmap( face );
		pTag->face = face;
		pTag->strAlias = szAlias;
		m_vFaceList.push_back( pTag );

		return true;
	}

	bool LoadFont( const char *szAlias, const char *szFilePath )
	{
		FT_Face face;

		if( FT_New_Face( m_library, szFilePath,  0, &face ) )
		{	
			return false;
		}

		FaceTag *pTag = new FaceTag;
		pTag->bCharsetSelected = selecUnicodeCharmap( face );
		pTag->face = face;
		pTag->strAlias = szAlias;
		m_vFaceList.push_back( pTag );

		return true;
	}

	int get_x_margin( int nFontSize, int ft )
	{
		int nXAdd = 0;
		if( ft == FT_GROW_LV1   || ft == FT_GROW_LV2 || ft == FT_GROW_LV3 ||
			ft == FT_STROKE_LV0 || ft == FT_STROKE_LV1 || ft == FT_STROKE_LV2 || ft == FT_STROKE_LV3 ) nXAdd = 1;
		return nXAdd;
	}

	int get_y_margin( int nFontSize, int ft )
	{
		if( m_bCompatibleMode )
		{
			int nYAdd = (int)(nFontSize*0.25f);
			if( nYAdd < 3 ) nYAdd = 3;
			return nYAdd;
		}

		int nYAdd = (int)(nFontSize*0.25f + 0.5f);
		return nYAdd;
	}

	int get_font_height( int nFontSize, int ft )
	{	
		return int( nFontSize * 1.4f + 2 );
		/*
		if( m_bCompatibleMode )
		{
			return nFontSize*0.9f + get_y_margin( nFontSize, ft )*2;
		}

		return nFontSize*1.5f + 2;
		*/
	}
	
	const XAlphaImage * Draw( const char *szAlias, const wchar_t* wszString, size_t len, int* pWidth, int* pHeight, int nFontSize = 11, int ft = FT_PLAIN, int y_offset = 0, bool bUnderline = false, bool bBackGround = false, bool bOnlySize = false, bool bHinting = false )
	{
		FaceTag* pTag = getFace( szAlias );
		if( !pTag ) return NULL;

		if( FT_Set_Char_Size( pTag->face, 0, 90*(nFontSize-1), 72, 72) ) return NULL;
		//if( FT_Select_Charmap( pTag->face, pTag->face->charmaps[0]->encoding ) ) return NULL;

		std::vector< FT_UInt >	vGlyphIndex;

		// glyph index 리스트를 얻는다
		getGlyphList( pTag, wszString, len, &vGlyphIndex );

		// 크기정보를 얻음
		getTextInfo( pTag, nFontSize, ft, vGlyphIndex, pWidth, pHeight, bHinting );

		if( bOnlySize ) return NULL;

		XAlphaImage *pImage = getSurface( *pWidth, *pHeight );

		drawGlyphList( pImage, pTag, nFontSize, ft, vGlyphIndex, y_offset, bHinting );

		applyEffect( pImage, nFontSize, ft, bUnderline, bBackGround );
		
		return pImage;
	}

	void	SetFontColor( int r, int g, int b, bool bAdjustEffectColor = true ) 
	{
		m_FontRed = r;
		m_FontGreen = g;
		m_FontBlue = b;		

		if( bAdjustEffectColor )
		{
			m_EffectRed = 255 - m_FontRed;
			m_EffectBlue = 255 - m_FontBlue;
			m_EffectGreen = 255 - m_FontGreen;
		}
	}

	void	SetBackGroundColor( int r, int g, int b ) 
	{
		m_BackRed = r;
		m_BackGreen = g;
		m_BackBlue = b;		
	}

	void	SetEffectColor( int r, int g, int b ) 
	{
		m_EffectRed = r;
		m_EffectGreen = g;
		m_EffectBlue = b;		
	}

	int CalcCarretPos( const char *szAlias, int nFontSize, int ft, const wchar_t* wszString, int x, bool bHinting )
	{
		FaceTag* pTag = getFace( szAlias );
		if( !pTag ) return -1;

		size_t len = wcslen( wszString );
		if( !len ) return -1;

		if( FT_Set_Char_Size( pTag->face, 0, 90*(nFontSize-1), 72, 72) ) return NULL;
		//if( FT_Select_Charmap( pTag->face, pTag->face->charmaps[0]->encoding ) ) return NULL;

		std::vector< FT_UInt >	vGlyphIndex;

		// glyph index 리스트를 얻는다
		getGlyphList( pTag, wszString, len, &vGlyphIndex );

		int nXMargin = get_x_margin( nFontSize, ft );
		//int nYMargin = get_y_margin( nFontSize, ft );

		int nPenStartX = get_pen_start_x( nFontSize, nXMargin );
		int nCurrentX = nPenStartX;

		FT_GlyphSlot  slot = pTag->face->glyph;
		
		for( size_t i = 0; i < vGlyphIndex.size(); ++i )
		{
			if( FT_Load_Glyph( pTag->face, vGlyphIndex[i], bHinting ? FT_LOAD_FORCE_AUTOHINT : FT_LOAD_NO_HINTING ) ) continue;

			if( x >= nCurrentX && x <= nCurrentX + (slot->advance.x >> 6 ) ) return int( i );

			/* increment pen position */
			nCurrentX += slot->advance.x >> 6;
		}

		return -1;
	}

private:

	struct FaceTag
	{
		FT_Face		face;
		bool		bCharsetSelected;
		std::string strAlias;
	};

	int get_pen_start_x( int nFontSize, int ft ) { return (int)(nFontSize*0.05f) + get_x_margin( nFontSize, ft ); }
	int get_pen_start_y( int nFontSize, int ft ) { return (int)(nFontSize*0.95f) + get_y_margin( nFontSize, ft ); }

	void applyEffect( XAlphaImage *pImage, int nFontSize, int ft, bool bUnderline, bool bBackground = false )
	{
		XAlphaImage *pBuffer     = NULL;
		XAlphaImage *pBackGround = NULL;


		if( bBackground )
		{
			pBackGround = getSurface( pImage->GetWidth(), pImage->GetHeight(), pImage );
			pBackGround->FillColor( static_cast< unsigned char >( m_BackRed ),
				static_cast< unsigned char >( m_BackGreen ),
				static_cast< unsigned char >( m_BackBlue ),
				180 );
		}

		int nXMargin = get_x_margin( nFontSize, ft );
		int nYMargin = get_y_margin( nFontSize, ft );
		int nShadowOffset = 0 - nFontSize / 20 - 1;

		if( ft == FT_STROKE_LV1 || ft == FT_STROKE_LV2 || ft == FT_STROKE_LV3 || ft == FT_STROKE_LV0 ||
			ft == FT_SHADOW_LV1 || ft == FT_SHADOW_LV2 || ft == FT_SHADOW_LV3 )
		{
			pBuffer = getSurface( pImage->GetWidth(), pImage->GetHeight(), pImage, pBackGround );
			*pBuffer = *pImage;
		}
		
		pImage->FillColor( static_cast< unsigned char >( m_FontRed ),
			static_cast< unsigned char >( m_FontGreen ),
			static_cast< unsigned char >( m_FontBlue ) );

		if( ft == FT_BOLD ) pImage->MultiplyAlpha( 1.8f );
//		if( ft == FT_GROW_LV1 ) pImage->GrowAlpha( 1 );
		if( ft == FT_GROW_LV2 ) pImage->GrowAlpha( 4 );
		if( ft == FT_GROW_LV3 ) { pImage->GrowAlpha( 3 ); pImage->GrowAlpha( 3 ); }
		if( ft == FT_SHADOW_LV1 || ft == FT_SHADOW_LV2 || ft == FT_SHADOW_LV3 )
		{
			if( ft == FT_SHADOW_LV2 ) { pBuffer->GrowAlpha( 6 ); }
			if( ft == FT_SHADOW_LV3 ) { pBuffer->GrowAlpha( 4 ); pBuffer->GrowAlpha( 4 ); }
			pBuffer->FillColor( static_cast< unsigned char >( m_EffectRed ),
				static_cast< unsigned char >( m_EffectGreen ),
				static_cast< unsigned char >( m_EffectBlue ) );
			pImage->Append( *pBuffer, nShadowOffset, nShadowOffset );
		}
		if( ft == FT_STROKE_LV1 || ft == FT_STROKE_LV2 || ft == FT_STROKE_LV3 || ft == FT_STROKE_LV0 )
		{	
			if( ft == FT_STROKE_LV0 ) { pImage->MultiplyAlpha( 1.2f ); pBuffer->GrowAlpha( 10 ); pBuffer->LowpassFilter( 170 ); pBuffer->MultiplyAlpha( 0.8f ); }
			if( ft == FT_STROKE_LV1 ) { pBuffer->GrowAlpha( 10 ); }
			if( ft == FT_STROKE_LV2 ) { pBuffer->GrowAlpha( 10 ); pBuffer->GrowAlpha( 5 ); }
			if( ft == FT_STROKE_LV3 ) { pBuffer->GrowAlpha( 10 ); pBuffer->GrowAlpha( 10 ); }
			pBuffer->FillColor( static_cast< unsigned char >( m_EffectRed ),
				static_cast< unsigned char >( m_EffectGreen ),
				static_cast< unsigned char >( m_EffectBlue ) );
			pImage->Append( *pBuffer, 0, 0, 256 );
		}

		if( bUnderline )
		{
			for( size_t i = nXMargin; i +nXMargin< pImage->GetWidth(); ++i )
			{
				pImage->SetColor( i, pImage->GetHeight() - nYMargin - nShadowOffset - 1, m_FontRed, m_FontGreen, m_FontBlue );
				pImage->SetAlpha( i, pImage->GetHeight() - nYMargin - nShadowOffset - 1, 255 );				
			}
		}

		// DEBUG
		/*
		for( int x = 0; x < pImage->GetWidth(); ++x ) 
		{
			pImage->SetAlpha( x, 0, 255 );
			pImage->SetColor( x, 0, m_FontRed, m_FontGreen, m_FontBlue );
			pImage->SetAlpha( x, pImage->GetHeight()-1, 255 );
			pImage->SetColor( x, pImage->GetHeight()-1, m_FontRed, m_FontGreen, m_FontBlue );
		}
		for( int y = 0; y < pImage->GetHeight(); ++y )
		{
			pImage->SetAlpha( 0, y, 255 );
			pImage->SetColor( 0, y, m_FontRed, m_FontGreen, m_FontBlue );
			pImage->SetAlpha( pImage->GetWidth()-1, y, 255 );
			pImage->SetColor( pImage->GetWidth()-1, y, m_FontRed, m_FontGreen, m_FontBlue );
		}
		*/
		
		if( bBackground )
		{
			pImage->Append( *pBackGround, 0, 0, 255 );
		}
	}

	void drawGlyphList( XAlphaImage *pImage, FaceTag* pTag, int nFontSize, int ft, const std::vector< FT_UInt > & vGlyphIndex, int y_offset = 0, bool bHinting = false )
	{
		FT_GlyphSlot			slot = pTag->face->glyph;  /* a small shortcut */

		int pen_x = get_pen_start_x( nFontSize, ft );
		int pen_y = get_pen_start_y( nFontSize, ft );

		for ( size_t n = 0; n < vGlyphIndex.size(); n++ )
		{	
			/* load glyph image into the slot (erase previous one) */
			if( FT_Load_Glyph( pTag->face, vGlyphIndex[n], bHinting ? FT_LOAD_FORCE_AUTOHINT : FT_LOAD_NO_HINTING ) ) continue;

			/* convert to an anti-aliased bitmap */
			if( FT_Render_Glyph( pTag->face->glyph, ft_render_mode_normal ) ) continue;

			/* now, draw to our target surface */
			draw_bitmap( pImage, &slot->bitmap,
							pen_x + slot->bitmap_left,
							pen_y - slot->bitmap_top+1 + y_offset );

			/* increment pen position */
			pen_x += slot->advance.x >> 6;
			pen_y += slot->advance.y >> 6; /* not useful for now */
		}
	}

	void draw_bitmap( XAlphaImage *pImg, FT_Bitmap*  bitmap, FT_Int x, FT_Int y )
	{
		FT_Int  i, j, p, q;
		FT_Int  x_max = x + bitmap->width;
		FT_Int  y_max = y + bitmap->rows;

		FT_Int	image_width  = (FT_Int)pImg->GetWidth();
		FT_Int	iamge_height = (FT_Int)pImg->GetHeight();

		for ( i = x, p = 0; i < x_max; i++, p++ )
		{
			for ( j = y, q = 0; j < y_max; j++, q++ )
			{
				if ( i >= image_width || j >= iamge_height ) continue;

				if( i < 0 || j < 0 || bitmap->buffer[q * bitmap->width + p] == 0 ) continue;
				//if( i < 0 || j < 0 ) continue;

				pImg->SetAlpha( i, j, bitmap->buffer[q * bitmap->width + p] );
			}
		}
	}

	XAlphaImage* getSurface( size_t nWidth, size_t nHeight, XAlphaImage *pExcept1 = NULL, XAlphaImage *pExcept2 = NULL)
	{
		for( std::vector< XAlphaImage* >::iterator it =  m_vImageList.begin(); it != m_vImageList.end(); ++it )
		{
			if( (*it)->GetWidth() < nWidth || (*it)->GetHeight() < nHeight || (*it) == pExcept1 || (*it) == pExcept2 ) continue;
			(*it)->Clear();
			(*it)->SetSize( nWidth, nHeight );
			return (*it);
		}

		XAlphaImage *pImg = NULL;
	
		pImg = new XAlphaImage( nWidth, nHeight );

		m_vImageList.push_back( pImg );
		return pImg;
	}

	FaceTag* getFace( const char *szAlias )
	{
		for(std::vector< FaceTag* >::iterator it = m_vFaceList.begin(); it != m_vFaceList.end(); ++it )
		{
			if( _stricmp( szAlias, (*it)->strAlias.c_str() ) ) continue;
			return (*it);
		}

		if( m_vFaceList.empty() ) return NULL;

		return m_vFaceList.front();
	}

	void getGlyphList( FaceTag* pTag, const wchar_t* wszString, size_t len, std::vector< FT_UInt > *pList )
	{
		// 폰트의 코드페이지를 얻어낸다
		int nCodePage = 0;
		if( pTag->bCharsetSelected == false )
		{
			for( int i = 0; i < pTag->face->num_charmaps; ++i )
			{
				if( pTag->face->charmaps[i]->encoding == FT_ENCODING_SJIS		)	{ nCodePage = 932; FT_Select_Charmap( pTag->face, pTag->face->charmaps[i]->encoding ); break; }
				if( pTag->face->charmaps[i]->encoding == FT_ENCODING_GB2312	)		{ nCodePage = 936; FT_Select_Charmap( pTag->face, pTag->face->charmaps[i]->encoding ); break; }
				if( pTag->face->charmaps[i]->encoding == FT_ENCODING_BIG5		)	{ nCodePage = 950; FT_Select_Charmap( pTag->face, pTag->face->charmaps[i]->encoding ); break; }
				if( pTag->face->charmaps[i]->encoding == FT_ENCODING_WANSUNG	)	{ nCodePage = 949; FT_Select_Charmap( pTag->face, pTag->face->charmaps[i]->encoding ); break; }
				if( pTag->face->charmaps[i]->encoding == FT_ENCODING_JOHAB	)		{ nCodePage = 1361; FT_Select_Charmap( pTag->face, pTag->face->charmaps[i]->encoding ); break; }
			}
		}

		for ( size_t n = 0; n < len; n++ )
		{
			int code_point = wszString[n];

			// UTF16-BE 로 변환
			if( nCodePage )
			{
				if( ( code_point >= 0xAC00 && code_point <= 0xD7A3 ) || ( code_point >= 0x3130 && code_point <= 0x318F ) )
				{
					WideCharToMultiByte( nCodePage, 0, &wszString[n], 1, (LPSTR)&code_point, sizeof(code_point), "*", NULL ); 
					std::swap( *((char*)( &code_point ) + 0), *((char*)( &code_point ) + 1) );
				}
			}

			/* retrieve glyph index from character code */
			pList->push_back( FT_Get_Char_Index( pTag->face, code_point ) );
		}
	}

	void getTextInfo( FaceTag* pTag, int nFontSize, int ft, const std::vector< FT_UInt >& vGlyphList, int* pWidth, int* pHeight, bool bHinting )
	{
		int nXMargin = get_x_margin( nFontSize, ft );
		//int nYMargin = get_y_margin( nFontSize, ft );

		int nPenStartX = get_pen_start_x( nFontSize, nXMargin );
		int nCurrentX = nPenStartX;

		FT_GlyphSlot  slot = pTag->face->glyph;
		
		for( size_t i = 0; i < vGlyphList.size(); ++i )
		{
			if( FT_Load_Glyph( pTag->face, vGlyphList[i], bHinting ? FT_LOAD_FORCE_AUTOHINT : FT_LOAD_NO_HINTING ) ) continue;

			/* increment pen position */
			nCurrentX += slot->advance.x >> 6;
		}

		int nWidth  = nCurrentX + nXMargin;
		int nHeight = get_font_height( nFontSize, ft );

		nWidth++;

		if( nWidth % 2 ) nWidth++;
		if( nHeight % 2 ) nHeight++;

		if( pWidth ) *pWidth = nWidth;
		if( pHeight ) *pHeight = nHeight;
	}

	bool						m_bCompatibleMode;

	int							m_FontRed,m_FontGreen,m_FontBlue;
	int							m_BackRed,m_BackGreen,m_BackBlue;
	int							m_EffectRed,m_EffectGreen,m_EffectBlue;

	FT_Library					m_library;
	std::vector< FaceTag* >		m_vFaceList;
	std::vector< XAlphaImage* > m_vImageList;
};








XFreeType::XFreeType()
{
	m_pImpl = new Imp();
}

XFreeType::~XFreeType()
{
	delete m_pImpl;
}

bool XFreeType::IsInitialized() const
{
	return m_pImpl->IsInitialized();
}

bool XFreeType::Init()
{
	return m_pImpl->Init();
}

bool XFreeType::DeInit()
{
	return m_pImpl->DeInit();
}

bool XFreeType::LoadFont( const char *szAlias, const void *szBuffer, size_t nBufferSize )
{
	return m_pImpl->LoadFont( szAlias, szBuffer, nBufferSize );
}

bool XFreeType::LoadFont( const char *szAlias, const char *szFilePath )
{
	return m_pImpl->LoadFont( szAlias, szFilePath );
}

int XFreeType::get_x_margin( int nFontSize, int ft )
{
	return m_pImpl->get_x_margin( nFontSize, ft );
}

int XFreeType::get_y_margin( int nFontSize, int ft )
{
	return m_pImpl->get_y_margin( nFontSize, ft );
}

int XFreeType::get_font_height( int nFontSize, int ft )
{
	return m_pImpl->get_font_height( nFontSize, ft );
}

void XFreeType::set_compatible_mode( bool bFlag )
{
	m_pImpl->set_compatible_mode( bFlag );
}

bool XFreeType::get_compatible_mode() const
{
	return m_pImpl->get_compatible_mode();
}

const XAlphaImage* XFreeType::Draw( const char *szAlias, const wchar_t* wszString, size_t len, int* pWidth, int* pHeight, int nFontSize, int ft, int y_offset, bool bUnderline, bool bBackGround, bool bOnlySize, bool bHinting )
{
	return m_pImpl->Draw( szAlias, wszString, len, pWidth, pHeight, nFontSize, ft, y_offset, bUnderline, bBackGround, bOnlySize, bHinting );
}

void XFreeType::SetFontColor( int r, int g, int b, bool bAdjustEffectColor )
{
	return m_pImpl->SetFontColor( r, g, b, bAdjustEffectColor );
}

void XFreeType::SetBackGroundColor( int r, int g, int b )
{
	return m_pImpl->SetBackGroundColor( r, g, b );
}

void XFreeType::SetEffectColor( int r, int g, int b )
{
	return m_pImpl->SetEffectColor( r, g, b );
}

int XFreeType::CalcCarretPos( const char *szAlias, int nFontSize, int ft, const wchar_t* wszString, int x, bool bHinting )
{
	return m_pImpl->CalcCarretPos( szAlias, nFontSize, ft, wszString, x, bHinting );
}
