// by Testors

/*

	note :

		1. 한 사운드는 동시에 15개 까지밖에 소리가 안나니 참고.
		2. 사운드는 최대 XMAX_SOUND_INDEX 개까지 쓸수 있는데 더 이상 쓰고싶으면 고칠것.



	예제 참조.

	unsigned short ERROR_SOUND_IDX = 0x00;

	XSoundManager sound;
	
	sound.Init( hWnd );							// 초기화
	
	sound.SetPosition( 100, 100, 0 );			// 청자 위치 설정
	
	sound.SetOrientation( 0, 1, 0, 0, 0, 1 );	// 청자 방향 설정
	
	// 웨이브 로딩
	sound.Load( ERROR_SOUND_IDX, "C:/WINDOWS/MEDIA/Windows XP Error.wav" );

	// 해당 웨이브에 거리계수 설정
	sound.SetMinMax( ERROR_SOUND_IDX, 100, 500 );

	// 재생 ( 왼쪽 스피커에서 사운드가 들리게 된다 )
	sound.Play( ERROR_SOUND_IDX, 50, 100, 0 );


 */

#pragma once

#include <audiere/audiere.h>
//#include <d3dtypes.h>


#include "../kfile/KStream.h"
#include "../kfile/KFileManager.h"
#include "XStreamReader.h"

const unsigned short XMAX_SOUND_INDEX = 256;


//struct AudiereStreamAdapter;
struct AudiereStreamAdapter : public audiere::RefImplementation<audiere::File>
{
public:
	AudiereStreamAdapter( KStream * p )
	{
		pStream = p;
	};

	~AudiereStreamAdapter()
	{
		if(pStream)
		{
			delete pStream;
		}
	};

	bool IsValid()
	{
		return pStream ? true : false;
	}

	virtual int ADR_CALL read(void* buffer, int size)
	{ 
		return (int)pStream->Read( buffer, size );
	};

	virtual bool ADR_CALL seek(int position, SeekMode mode)
	{ 
		KStream::enum_seek_origin nOrigin = KStream::seekCur;

		if( mode == SEEK_CUR ) nOrigin = KStream::seekCur;
		if( mode == SEEK_END ) nOrigin = KStream::seekEnd;
		if( mode == SEEK_SET ) nOrigin = KStream::seekSet;
		
		pStream->Seek( position, nOrigin );
	
		return true; 
	};

	virtual int ADR_CALL tell() 
	{ 
		return (int)pStream->Tell();
	};

	KStream *pStream;
};


class XSoundManager
{
public:
	XSoundManager();
	virtual ~XSoundManager();

	bool Init( void* hWnd, bool bGlobal = false, bool bUse3D = true, bool bIsStereo = true, unsigned freq = 44100, unsigned bit = 16 );
	bool DeInit();

	bool Load( unsigned short idx, const char *szFileName, AudiereStreamAdapter * pStream, bool bUse3D = true , bool bStream = true);
	bool Load( unsigned short idx, const char *szFileName, bool bUse3D = true , bool bStream = true);
	bool UnLoad( unsigned short idx );

	bool SetMinMax( unsigned short idx, float fMin, float fMax );
	bool SetListenerPosition( float x, float y, float z = 0.0f );
	bool SetCameraPosition( float x, float y, float z = 0.0f );

	bool SetRollOffFactor( float x );
	bool GetRollOffFactor( float * x );

	bool SetVolume( int nVolume );
	int  GetVolume() const;

	/// Play() 는 play_id 를 반환한다. -1 이라면 재생실패
	int  Play( unsigned short idx, bool bUse3D = true, bool bLoop = false, int nVolume = 0 , bool bStream = true);
	int  Play( unsigned short idx, float x, float y, float z = 0.0f, bool bLoop = false, int nVolume = 0, bool bStream = true );
	bool Stop( unsigned short idx, int play_id );



	//ps	
	bool  SetLowFilter( int idx, int play_id, int  frequency);
	//해주기 - ps 거리에 따른 로우필터 자동 set하기 위한 기울기관련 변수set하는 함수 
	

protected:

	bool m_bUse3D;

	D3DVECTOR m_listenerPosition;
	D3DVECTOR m_cameraPosition;

	int	m_nVolume;

	class XDSoundBufferManager*	m_pBuffer[XMAX_SOUND_INDEX];
	audiere::AudioDevicePtr		m_pDevice;


	//해주기 - ps 거리에 따른 로우필터 자동 set하기 위한 기울기관련 변수선언
	
};