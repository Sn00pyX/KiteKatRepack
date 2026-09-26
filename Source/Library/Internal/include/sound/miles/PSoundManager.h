#pragma once

#include <vector>
#include "../../toolkit/khash.h"

#include <miles/mss.h>

#include "PStreamReader.h"

#include "PSoundBufferManager.h"






class PSoundManager
{
	//필터링 관련 enum
	enum
	{
		VOLUME_RAMP =0,
		FILTER_COUNT
	};



public:
	PSoundManager(void);
	~PSoundManager(void);
	
	bool   Initialize(class ISoundBufferInfoTable *pBufferTable, const char *szPathRedist,  HWND hWnd, const char *szChannelName, 
		             unsigned freq = 44100, int bit = 16, unsigned flag = 0);
		
	int    Load( const char *szFileName, PStreamReader* pStream );	
	int    Load( const char *szFileName);	
	bool   UnLoad(const char *szFileName);

	bool   SetAutoWet( int nBufferIndex ,bool  bAutoSet);
	bool   SetMinMaxDist(int nBufferIndex , float fMin, float fMax );
	bool   SetMinMaxDist(int nBufferIndex, int nSampleIndex , float fMin, float fMax );
	bool   SetPosition( PSoundVector  vecPos);
	bool   SetOrientation( PSoundVector vecFace, PSoundVector vecUp);

	bool   SetMasterVolume( float fVolume);
	float  GetMasterVolume() const;
	bool   SetBufferVolume( int nBufferIndex, float fLVolume, float fRVolume);
	bool   SetBufferVolume( float fLVolume, float fRVolume );

	//bool SetRollOffFactor( float x );
	//bool GetRollOffFactor( float * x );
	//bool SetDopplerFactor( float x );
	//bool GetDopplerFactor( float * x );	

	int    bHaveSoundBuffer(const char *szFileName);

	int    Play2D( int nBufferIndex , int nLoopCount, float fLVolume, float fRVolume, int nPlayRate, bool bLock);
	int    Play( int nBufferIndex , PSoundVector  vecPos,int nLoopCount, float fLVolume, float fRVolume, int nPlayRate, bool bLock, bool bReverb, bool bLowPass );

	bool   StopAll();
	bool   Stop( int nBufferIndex , int nSampleIndex);	
	bool   Stop( const char *szFileName);
	bool   Stop( int nBufferIndex);
	bool   Pause( int nBufferIndex, int nSampleIndex);

	bool   Resume_Sound( int nBufferIndex, int nSampleIndex);
	bool   SetPlayBackRate( int nBufferIndex, int nSampleIndex, int nRate);
	void   SetPosition_Sample( int nBufferIndex, int nSampleIndex, float x, float y, float z);


	/// reverb 관련
    bool SetMasterReverb( float fReverbTime, float fReverb_Predelay, float fEvery_Damping);
	bool SetMasterReverbLevel( float fDryLevel, float fWetLevel);
	bool SetRoomType( int nRoom_Type);
	
	/// bgm, sfx  set하기!!
	void Set_BGM_SFX( float fBGM, float fSFX);

	/// fade
	bool   SetFade_Buffer( const char *szFileName, float time, bool bFadeIn);
protected:		
	static U32 AILCALLBACK AIL_file_open_callback( MSS_FILE const FAR* filename,  UINTa FAR* file_handle);
	static void AILCALLBACK AIL_file_close_callback(UINTa file_handle);
	static S32 AILCALLBACK AIL_file_seek_callback(UINTa file_handle, S32 offset, U32 type);
	static U32 AILCALLBACK AIL_file_read_callback (UINTa file_handle,void FAR* buffer, U32 bytes);



protected:
	std::vector<int>               m_vecFreeBuffer;                 ///< 사용하지 않은 Buffer 인덱스	
	PSoundBufferManager *          m_pBuffer[ MAX_SOUND_BUFFER];
	KHash<int, hashPr_string>      m_hashSound;	
		
	//float					   m_posX, m_posY, m_posZ;     //	listener position vector	
	//float					   m_faceX, m_faceY, m_faceZ;  //	orientation face vector	
	//float					   m_upX, m_upY, m_upZ;        //	orientation up vector	
	
	float                          m_fBGM;
	float                          m_fSFX;

	class ISoundBufferInfoTable *  m_pBufferTable;

	HDIGDRIVER				       m_hDig;	
};