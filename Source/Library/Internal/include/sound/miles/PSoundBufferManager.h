#pragma once

#include <miles/mss.h>

#include "PStreamReader.h"


//#define LEFT_HAND_COORDINATE        
typedef int  TRANSLATE_INT;

const unsigned short MAX_SOUND_BUFFER   = 256;

//const unsigned short MAX_SOUND_SAMPLE   = 40;
const unsigned short MAX_SOUND_SAMPLE   = DEFAULT_DMC;

const float          DEF_MAX_DIST       = 800.0f;
const float          DEF_MIN_DIST       = 5.0f;
const float		     VOLUME_FACTOR      = 1.0f;
const float          DEF_MASTER_VOLUME  = 1.0f;     ///< 0~1.0f
const float          DEF_BUFFER_VOLUME  = 1.0f;     ///< 0~1.0f




/** 게임-오른손좌표,  마일즈-왼손좌표  사용하기때문에 좌표계 알맞게 바꿔주는 구조체
	ISoundManger <-> PSoundManager 이렇게 전달인자 주고받을때만 사용한다.  왜냐면, 만약 나중에 사용하지 않게瑛뻑?최소한만 수정하기위해 */
struct PSoundVector
{
public:
	float    m_fx;
	float    m_fy;
	float    m_fz;

public:
	PSoundVector()
	{
		m_fx = 0.0f;
		m_fy = 0.0f;
		m_fz = 0.0f;
	}
	PSoundVector(float fx, float fy, float fz)
	{
		m_fx = fx;
		m_fy = fy;

//		#ifdef LEFT_HAND_COORDINATE      //위에서 들어오는 fx,fy,fz가 왼손좌표계 기준이라면  m_fz=fz  이고
			m_fz = fz;
/*		#else                            //fx,fy,fz가 오른손좌표계라면   m_fz= -1*fz 로 해준다.
			//m_fz =  -1.0f * fz ;         //이유는  마일즈엔진에 들어가는좌표가 왼손좌표계기준이라서 바꿔주는것임!
			m_fz = fy;
			m_fy = fz;
		#endif*/
	}
	PSoundVector( PSoundVector &Vector)
	{
		m_fx = Vector.m_fx;
		m_fy = Vector.m_fy;
		m_fz = Vector.m_fz;	
	}
	PSoundVector& operator=(PSoundVector &Vector)
	{
		m_fx = Vector.m_fx;
		m_fy = Vector.m_fy;
		m_fz = Vector.m_fz;
		return *this;
	}
};



/// *** Sample 관리 구조체    //데이터 보관이 주목적이고 기타 함수는 최소한만 사용하자!!. 
struct TSAMPLE
{
	HSAMPLE   m_hSample;

	bool      m_bLock;        ///< 이 Sample이  현재 Lock상태인지.	
	int       m_owner;        ///< Owner 버퍼를 가리킴
	int       m_nDefPlayRate;

	TSAMPLE()
	{
		m_owner   = -1;
		m_bLock   = false;
		m_hSample = NULL;
		m_nDefPlayRate = 0;
	}	

	~TSAMPLE()
	{
		Clear();
	}

	void Clear()
	{
		if ( m_hSample)	
		{
			AIL_stop_sample( m_hSample);				
			AIL_release_sample_handle( m_hSample);
			m_hSample = NULL;
		}
	}

	bool Init( int owner, HDIGDRIVER hDig, const char *szFileName, void *pData, int nLength)
	{
		if ( !m_hSample)
			m_hSample = AIL_allocate_sample_handle( hDig);

		if ( !m_hSample)				
			return false;

		m_owner   = owner;		
		AIL_set_named_sample_file( m_hSample, szFileName , pData, nLength, 0);  //wav , mp3 .. 지원. 			
		return true;
	}

	void ResetPlayRate() 
	{		
		if ( m_hSample)	AIL_set_sample_playback_rate( m_hSample, m_nDefPlayRate);
	}

	bool IsOwner( int owner)
	{
		if ( !m_hSample)
			return false;

		return ( m_owner == owner);
	}

	bool IsLock()
	{		
		return m_bLock;
	}

	bool IsPlaying()
	{
		if ( !m_hSample)		return false;

		if ( AIL_sample_status( m_hSample) == SMP_PLAYING)
			return true;

		return false;
	}

	void SetLock( bool bLock){ m_bLock = bLock;}
};



class PSoundBufferManager
{
public:
	PSoundBufferManager( class ISoundBufferInfoTable *pBufferTable, PStreamReader* pStream,  HDIGDRIVER  hDig, float *pfBGM, float *pfSFX, int nIndex);
	~PSoundBufferManager(void);	

	bool   Resume_Sound( int nSampleIndex);
	bool   SetPlayBackRate( int nSampleIndex, int nRate);

	/// Stop는 UnLock까지 수행한다. 
	bool   Stop( int & nSampleIndex);
	bool   Stop();
	/// Pause는 단지 Sound만 멈춤.
	bool   Pause( int nSampleIndex);

	int    Play( PSoundVector &vecPos, int nLoop, float fLVolume, float fRVolume, int nPlayRate, bool bLock, bool bReverb, bool bLowPass );
	int    Play2D( int nLoop,  float fLVolume, float fRVolume, int nPlayRate, bool bLock);
	void   SetVolume(  float fLVolume, float fRVolume);
	void   ResetVolume();
	void   SetPosition_Sample(int nSampleIndex, PSoundVector pos);


	bool   SetMinMaxDist(float fMin, float fMax );
	bool   SetMinMaxDist(int nSampleIndex, float fMin, float fMax );
	bool   SetAutoWet( bool  bAutoSet);


	/// fade
	bool   SetFade( float time, bool bFadeIn);	 
protected:
	int    GetFreeBuffer();	
	void   RemoveAll();

private:

	const PSoundBufferManager& operator=( const PSoundBufferManager& );



protected:
	int             m_nDefPlayRate;
	bool            m_bAutoWet;
	float			m_fMinDist;
	float			m_fMaxDist;
	

	float           m_fBufferVolumeFactor;  ///< 버퍼(파일) 단위로  최대값 조절하는변수             
	float           m_fLVolume;		        ///< 현재 볼륨  마스터VolumeFactor,  BGM, SFX, m_fBufferVolumeFactor  적용이  안되있는 값이다.
	float           m_fRVolume;		        ///< 현재 볼륨  마스터VolumeFactor,  BGM, SFX, m_fBufferVolumeFactor  적용이  안되있는 값이다.

	float           m_fReverbDry;
	float           m_fReverbWet;

	float			m_fCutOff;
	
	bool            m_bSfx;                 ///< sfx인지 bgm 인지 구별하기 위한값. 
	float*          m_pfBGM;
	float*          m_pfSFX;	

	void*			m_pData;	
	int             m_nLength;              ///< 버퍼길이

	
	const int                      m_nBufferIndex;
	std::string                    m_strFileName;
	class ISoundBufferInfoTable *  m_pBufferTable;	
	HDIGDRIVER		               m_hDig;

	//HSAMPLE	      m_aSample[ MAX_SOUND_SAMPLE];		
  
	static TSAMPLE    m_tSamples[ MAX_SOUND_SAMPLE];		
};