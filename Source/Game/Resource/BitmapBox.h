#pragma once

class BitmapBox
{
public:
	BitmapBox( int width, int height )
	{
		m_fAttrLength = 0;
		m_nWidth = width;
		m_nHeight = height;

		m_pData = new unsigned char*[height];
		for( int i = 0; i < height; ++i )
		{
			m_pData[i] = new unsigned char[ width ];
			memset( m_pData[i], 0, sizeof( unsigned char ) * width );
		}
	}

	~BitmapBox()
	{
		for( int i = 0; i < m_nHeight; ++i )
		{
			delete [] m_pData[i];
		}

		delete [] m_pData;
	}

	inline bool IsOn( int width, int height )
	{
		unsigned char & c = m_pData[height][(width/8)];
		return !!( ((1<<(unsigned char)(width%8))) & c );
	}

	inline void SetOn( int width, int height )
	{
		unsigned char & c = m_pData[height][(width/8)];
		unsigned char f = (1<<(width%8));
		c |= f;
	}

	inline void SetOff( int width, int height )
	{
		unsigned char & c = m_pData[height][(width/8)];
		unsigned char f = (1<<(width%8));
		c &= (~f);
	}

	inline void Set( int width, int height, bool bIsTrue )
	{
		if( bIsTrue ) SetOn( width, height );
		else SetOff( width, height );
	}

	inline int GetWidth()	{ return m_nWidth; }
	inline int GetHeight()	{ return m_nHeight; }
	inline float GetAttrLength()	{ return m_fAttrLength; }

	float m_fAttrLength;
	int	m_nWidth,m_nHeight;
	unsigned char **m_pData;
};