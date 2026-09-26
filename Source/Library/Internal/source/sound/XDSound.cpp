#include "../../include/sound/XDSound.h"

const int MAX_VOLUME = 5000;

static bool setVolume( IDirectSoundBuffer8 *pBuffer, int nVolume )
{
	if( !pBuffer ) return false;

	long lVolume;

	if( nVolume ) lVolume = (long)( 0 - MAX_VOLUME*((100 - nVolume)/100.0f) );
	else lVolume = -10000;

	return !FAILED( pBuffer->SetVolume( lVolume ) );
}

static int getVolume( IDirectSoundBuffer8 *pBuffer )
{
	if( !pBuffer ) return 0;

	long nVolume;
	pBuffer->GetVolume( &nVolume );

	nVolume = (int)( 100 + (nVolume / (float)MAX_VOLUME) * 100.0f );

	if( nVolume < 0 ) nVolume = 0;

	return nVolume;
}

XDSound::XDSound()
{
	m_fx = m_fy = m_fz = 0;
	m_bUse3D = false;
	m_bGlobal = false;
	m_pPrimaryBuffer = NULL;
	m_pDS = NULL;
	m_p3DListener = NULL;
}

XDSound::~XDSound()
{
	DeInit();
}

bool XDSound::Init( HWND hWnd, bool bUse3D, bool bGlobal, bool bIsStereo, unsigned freq, unsigned bit )
{
	WAVEFORMATEX	waveFormat;
	DSBUFFERDESC	bufferInfo;

	m_bGlobal = bGlobal;
	m_bUse3D = bUse3D;

	if( FAILED( DirectSoundCreate8( NULL, &m_pDS, NULL ) ) )
	{
		return false;
	}

	if( FAILED( m_pDS->SetCooperativeLevel( hWnd, DSSCL_PRIORITY ) ) ) return false;

	memset( &waveFormat, 0, sizeof(waveFormat) );
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = (unsigned short)(1 + bIsStereo );
	waveFormat.nSamplesPerSec = freq;
	waveFormat.wBitsPerSample = (unsigned short)bit;
	waveFormat.nBlockAlign = (unsigned short)((waveFormat.wBitsPerSample / 8) * waveFormat.nChannels);
	waveFormat.nAvgBytesPerSec = (unsigned short)waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

	memset( &bufferInfo, 0, sizeof(bufferInfo) );
	bufferInfo.dwSize = sizeof(bufferInfo);
	bufferInfo.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_PRIMARYBUFFER;
	if( m_bUse3D ) bufferInfo.dwFlags |= DSBCAPS_CTRL3D;
	bufferInfo.dwBufferBytes = 0;
	bufferInfo.lpwfxFormat = NULL;


	if( FAILED( m_pDS->CreateSoundBuffer( &bufferInfo, (LPDIRECTSOUNDBUFFER*)&m_pPrimaryBuffer, NULL ) ) )
	{
		m_pDS->Release();
		m_pDS = NULL;
		return false;
	}

	if( m_bUse3D )
	{
		m_pPrimaryBuffer->QueryInterface( IID_IDirectSound3DListener8, (LPVOID*)&m_p3DListener );

		if( !m_p3DListener )
		{
			m_pDS->Release();
			m_pDS = NULL;

			m_pPrimaryBuffer->Release();
			m_pPrimaryBuffer = NULL;

			return false;
		}

		D3DVECTOR v;
		m_p3DListener->GetPosition( &v );

		m_fx = v.x;
		m_fy = v.y;
		m_fz = v.z;
	}
	
	return true;
};

bool XDSound::DeInit()
{
	if( m_pDS ) m_pDS->Release();
	m_pDS = NULL;

	return true;
}

bool XDSound::SetVolume( int nVolume )
{
	return setVolume( m_pPrimaryBuffer, nVolume );
}

int XDSound::GetVolume()
{
	return getVolume( m_pPrimaryBuffer );
}

bool XDSound::SetPosition( float x, float y, float z )
{
	if( !m_p3DListener ) return false;

	m_fx = x;
	m_fy = y;
	m_fz = z;

	return !FAILED( m_p3DListener->SetPosition( x, y, z, DS3D_IMMEDIATE ) );
}

bool XDSound::GetPosition( float * x, float * y, float * z )
{
	if( !m_p3DListener ) return false;

	*x = m_fx;
	*y = m_fy;
	*z = m_fz;

	return true;
}

bool XDSound::SetOrientation( float fx, float fy, float fz, float tx, float ty, float tz )
{
	return !FAILED( m_p3DListener->SetOrientation( fx, fy, fz, tx, ty, tz, DS3D_IMMEDIATE ) );
}

bool XDSound::SetRollOffFactor( float x )
{
	if( !m_p3DListener ) return false;

	return !FAILED( m_p3DListener->SetRolloffFactor( x, DS3D_IMMEDIATE ) );
}

bool XDSound::GetRollOffFactor( float * x )
{
	if( !m_p3DListener ) return false;

	return !FAILED( m_p3DListener->GetRolloffFactor( x ) );
}

XDSoundBuffer * XDSound::CreateBuffer( XWave * pWave, bool bUse3D )
{
	if( !m_pDS ) return NULL;

	IDirectSoundBuffer8 * pBuffer = NULL;
	DSBUFFERDESC desc;

	char			*pt1 = NULL;
	DWORD			size1;
	int				size_byte = 0;
	DWORD			flag = DSBLOCK_ENTIREBUFFER;

	if( !pWave->ReadWaveInfo() ) return NULL;

	size_byte = pWave->GetSize() * pWave->GetWaveFormat().nBlockAlign;

	memset( &desc, 0, sizeof(desc) );

	desc.dwSize = sizeof(desc);
	if( m_bGlobal ) desc.dwFlags |= DSBCAPS_GLOBALFOCUS;
	if( bUse3D ) desc.dwFlags |= DSBCAPS_CTRL3D;
	desc.dwBufferBytes = pWave->GetSize();
	desc.lpwfxFormat = (LPWAVEFORMATEX)&pWave->GetWaveFormat();

	if( FAILED( m_pDS->CreateSoundBuffer( &desc, (LPDIRECTSOUNDBUFFER*)&pBuffer, NULL ) ) ) return NULL;
	
	// DSERR_BADFORMAT
	if( pBuffer->Lock( 0, size_byte, (void**)&pt1, (DWORD*)&size1, NULL, NULL, flag ) == DSERR_BUFFERLOST )
	{
		pBuffer->Restore();
		if( pBuffer->Lock( 0, size_byte, (void**)&pt1, (DWORD*)&size1, NULL, NULL, flag ) == DSERR_BUFFERLOST )
		{
			pBuffer->Release();
			return false;
		}
	}

	if( !pWave->ReadWaveData( pt1, size1 ) )
	{
		pBuffer->Release();
		return false;
	}

	XDSoundBuffer * pNewBuffer = new XDSoundBuffer( pBuffer, pWave->IsStereo(), m_pDS );
	if( bUse3D ) pNewBuffer->Init3D();

	return pNewBuffer;
}

XDSoundBuffer * XDSoundBuffer::DuplicateBuffer()
{
	IDirectSoundBuffer8 * pNewDSBuffer;
	m_pDS->DuplicateSoundBuffer( (LPDIRECTSOUNDBUFFER)m_pBuffer, (LPDIRECTSOUNDBUFFER*) &pNewDSBuffer );
	
	XDSoundBuffer * pNewBuffer = new XDSoundBuffer( pNewDSBuffer, m_bIsStereo, m_pDS );
	if( Is3DBuffer() )
	{
		float fMin, fMax;
		m_p3D->GetMinDistance( &fMin );
		m_p3D->GetMaxDistance( &fMax );
		pNewBuffer->Init3D();
		pNewBuffer->SetMinMax( fMin, fMax );
	}

	return pNewBuffer;
}


XDSoundBuffer::XDSoundBuffer( IDirectSoundBuffer8*	pBuffer, bool bStereo, IDirectSound8* pDS )
{
	m_pBuffer = pBuffer;
	m_p3D = NULL;
	m_pDS = pDS;
	m_bIsStereo = bStereo;
}

XDSoundBuffer::~XDSoundBuffer()
{
	m_pBuffer->Release();
}

bool XDSoundBuffer::Play( bool bIsLoop )
{
	if( FAILED( m_pBuffer->SetCurrentPosition( 0 ) ) ) return false;

	return !FAILED( m_pBuffer->Play( 0, 0, ( bIsLoop ? DSBPLAY_LOOPING  : 0 ) ) );
}

bool XDSoundBuffer::Stop()
{
	return !FAILED( m_pBuffer->Stop() );
}

bool XDSoundBuffer::Init3D()
{
	if( FAILED( m_pBuffer->QueryInterface( IID_IDirectSound3DBuffer8, (LPVOID*)&m_p3D ) ) )	return false;

	return true;
}

bool XDSoundBuffer::SetMinMax( float fMin, float fMax )
{
	if( !m_p3D ) return false;

	m_p3D->SetMinDistance( fMin, DS3D_IMMEDIATE );
	m_p3D->SetMaxDistance( fMax, DS3D_IMMEDIATE );

	return true;
}

bool XDSoundBuffer::Play( float x, float y, float z, bool bIsLoop )
{
	if( m_p3D )	m_p3D->SetPosition( x, y, z, DS3D_IMMEDIATE );

	return Play( bIsLoop );
}

bool XDSoundBuffer::IsPlaying()
{
	DWORD dwStatus;
	m_pBuffer->GetStatus( &dwStatus );
	return ( dwStatus & DSBSTATUS_PLAYING );
}

bool XDSoundBuffer::SetVolume( int nVolume )
{
	return setVolume( m_pBuffer, nVolume );
}

int XDSoundBuffer::GetVolume()
{
	return getVolume( m_pBuffer );
}