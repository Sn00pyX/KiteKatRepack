#pragma once

#include <dsound.h>
#include "XWave.h"

class XDSoundBuffer
{
public:

	XDSoundBuffer( IDirectSoundBuffer8*	pBuffer, bool bStereo, IDirectSound8* pDS );
	virtual ~XDSoundBuffer();

	bool Play( bool bIsLoop = false );
	bool Play( float x, float y, float z = 0.0f, bool bIsLoop = false );
	bool Stop();
	bool Init3D();
	bool SetMinMax( float fMin, float fMax );

	bool IsPlaying();
	bool IsStereo()		{ return m_bIsStereo; }
	bool Is3DBuffer()	{ return !!m_p3D;	}

	bool SetVolume( int nVolume );
	int  GetVolume();

	XDSoundBuffer * DuplicateBuffer();
	
protected:
	
	bool					m_bIsStereo;

	IDirectSound8*			m_pDS;
	IDirectSound3DBuffer8*	m_p3D;
	IDirectSoundBuffer8*	m_pBuffer;
};

class XDSound
{
public:

	XDSound();
	virtual ~XDSound();

	bool Init( HWND hWnd, bool bUse3D = true, bool bGlobal = false, bool bIsStereo = true, unsigned freq = 44100, unsigned bit = 16 );
	bool DeInit();

	XDSoundBuffer * CreateBuffer( XWave * pWave, bool bUse3D = false );

	bool			SetPosition( float x, float y, float z = 0.0f );
	bool			GetPosition( float * x, float * y, float * z );

	bool			SetOrientation( float fx, float fy, float fz, float tx, float ty, float tz );

	bool			SetRollOffFactor( float x );
	bool			GetRollOffFactor( float * x );

	bool			SetVolume( int nVolume );
	int				GetVolume();

	bool			IsUsing3D()			{ return m_bUse3D; }

protected:

	float						m_fx, m_fy, m_fz;
	bool						m_bUse3D;
	bool						m_bGlobal;
	IDirectSound3DListener8*	m_p3DListener;
	IDirectSound8*				m_pDS;
	IDirectSoundBuffer8*		m_pPrimaryBuffer;
};