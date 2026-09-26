#pragma once

#include "XStreamReader.h"
#include <windows.h>

struct XWave
{
	XWave( XStreamReader *pStreamReader );
	XWave( const char* szFileName		);
	virtual ~XWave();

	bool					ReadWaveInfo();
	bool					ReadWaveData( void * p, unsigned len );

	const WAVEFORMATEX &	GetWaveFormat()	{ return m_WaveFormat;	}
	unsigned				GetSize()		{ return m_unSize;		}
	bool					IsStereo();

private:

	XWave( const XWave& );
	XWave& operator=( const XWave& );

	void init();

	bool			m_bIsMyStream;
	XStreamReader * m_pStream;

	unsigned		m_unSize;
	WAVEFORMATEX	m_WaveFormat;

	class CWaveFile * m_pWaveFile;
};



