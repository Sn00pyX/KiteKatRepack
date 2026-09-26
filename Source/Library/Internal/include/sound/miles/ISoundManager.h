#pragma once

#include <string>

#include <miles/mss.h>

//해주기 - 마일즈는 왼손좌표계 사용하기 때문에 .. 좌표 넘겨 받을때  오른손 좌표계로 바꿔서 받아야 한다. ( z값에 -1 곱하기)
//좌표넘겨받을때 왼손좌표계로 받을것인지 아니면 어떻게 할건지 결정하기
//아니면 ..    오른손좌표계를 왼손좌표계로 바꿔주는 백터 구조체 만들어서 PSoundManager부터는 그 백터 이용해서 좌표처리한다. 

struct  TSoundKey
{
	std::string     m_strFileName;
	int             m_nBufferIndex; 
	int             m_nSampleIndex;
};



class ISoundManager
{
public:
	ISoundManager(void);
	~ISoundManager(void);

	bool Initialize( class ISoundBufferInfoTable *pBufferTable, const char *szPathRedist, void* hWnd, const char *szChanelName,
		            unsigned freq = 44100, int bit = 16, unsigned flag = 0);
	bool UnLoad( const char *szFileName);

	bool SetAutoWet( int nBufferIndex ,bool  bAutoSet);
	/// 버퍼단위
	bool SetMinMaxDist( int nBufferIndex, float fMin, float fMax );
	/// sample 단위
	bool SetMinMaxDist( int nBufferIndex, int nSampleIndex, float fMin, float fMax );
	bool SetPosition( float x, float y, float z = 0.0f );
	bool SetOrientation( float face_x, float face_y, float face_z = 0.0f, float up_x = 0.0f, float up_y = 0.0f, float up_z = 0.0f);

	void SetMute_Buffer( bool bFlag );	
	bool SetMasterVolume( int nVolume );
	int  GetMasterVolume();
	bool SetBufferVolume( int nBufferIndex, int nLVolume, int nRVolume);
	bool SetBufferVolume( int nLVolume, int nRVolume);
	void SetSfxVolume( int nVolume );

	void Stop_AllSound();
	void Stop_Sound( int nBufferIndex);
	void Stop_Sound( const char *szFileName);
	void Stop_Sound( int nBufferIndex, int nSampleIndex);
	bool Pause_Sound( int nBufferIndex, int nSampleIndex);

	/** Play시킬때  pSoundKey이 NULL이 아니면 자동으로  Sample을  Lock을건다. 
		따라서 pSoundKey이 NULL이 아니면 UnLock도 사용자가 해줘야됨. */
	bool Play_Sound2D(bool bSampleLock, TSoundKey *pSoundKey, const char * pName, int nLoopCount, int nLVolume=100, int nRVolume=100, int nPlayRate = 0); //위치를 Listener의  x,y,z로 set한다.
	bool Play_Sound(bool bSampleLock, TSoundKey *pSoundKey, const char * pName, float x, float y, float z, int nLoopCount, int nLVolume, int nRVolume, int nPlayRate, bool bReverb, bool bLowPass);	   //StreamAdapter이용
	bool Play_SoundFromFile2D(bool bSampleLock, TSoundKey *pSoundKey, const char * pName , int nLoopCount , int nLVolume, int nRVolume=100, int nPlayRate = 0) ;  // PFileStreamReader 이용 
	

	bool Resume_Sound( int nBufferIndex, int nSampleIndex);
	bool SetPlayBackRate( int nBufferIndex, int nSampleIndex, int nRate);
	void SetPosition_Sample( int nBufferIndex, int nSampleIndex, float x, float y, float z);

	//reverb 관련	
    bool SetMasterReverb( float fReverbTime, float fReverb_Predelay, float fEvery_Damping);
	bool SetMasterReverbLevel( int nDryLevel, int nWetLevel);
	bool SetRoomType( int nRoom_Type);
	
	/// bgm, sfx  set하기!!
	void Set_BGM_SFX( int nBGM, int nSFX);	

	//fade
	bool   SetFade_Buffer ( const char *szFileName, int time, bool bFadeIn);

public:
	int  PreLoad( const char * pName);
protected:	
	void RemoveAll();	
	





//   **********   Stream  Play 관련 *************
public:	
	void   SetMute_Stream( bool bFlag );
	int    Play_Stream(         const char *szFileName , int nLoop , int nLVolume=100, int nRVolume=100, AILSTREAMCB  CallBack_Func =NULL);	
	int    Play_Bgm_Stream( const char *szFileName , int nLoop , int nLVolume=100, int nRVolume=100, AILSTREAMCB  CallBack_Func =NULL );
	int    Play_StreamFromFile( const char *szFullPathName, const char *szFileName , int nLoop , int nLVolume=100, int nRVolume=100, AILSTREAMCB  CallBack_Func =NULL);	
	bool   UnLoad_Stream(const char *szFileName);
	void   UnLoadAllStream();
	bool   SetPlayBackRate_Stream( const char *szFileName, int nRate);
	bool   Stop_Stream(const char *szFileName);
	bool   SetFade_Stream ( const char *szFileName, int time, bool bFadeIn);

	void   SetBgmSfxVolume( int nBgnVolume, int nSfxVolume );
	void   SetBgmVolume( int nVolume );
	void   SetEnvVolume( int nVolume );
	bool   IsPlayStream( const char *szFileName );

	int    GetStreamIndex( const char *szFileName );

	bool   GetMuteBuffer() { return m_bMute_Buffer; }
	bool   GetMuteStream() { return m_bMute_Stream; }
protected:
	class PSoundManager*   m_pSoundMng;   	
	class PStreamManager*  m_pStreamMng;

	bool			       m_bMute_Buffer;
	bool                   m_bMute_Stream;

};