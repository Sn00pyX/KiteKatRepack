#pragma once

#include <vector>

#include "../../toolkit/khash.h"

#include <miles/mss.h>
#include "PStreamReader.h"

// 스트림으로 Play 한다. -  주로 BGM에서 이용한다!!!



class PStreamManager
{
	struct TStreamData
	{
		HSTREAM           m_hStream;
		PStreamReader*    m_pStreamReader;
		std::string       m_strFileName;		 
		float             m_fBufferVolumeFactor;  
		float             m_fLVolume;
		float             m_fRVolume;
		bool              m_bSfx;

	public:
		TStreamData()
		{
			m_hStream      = NULL;
			m_pStreamReader= NULL;
			
			m_fBufferVolumeFactor = 1.0f;
			m_fLVolume            = 1.0f;
			m_fRVolume            = 1.0f;

			m_bSfx                = false;  //디폴트가 BGM이 되게하자. 
		}
		~TStreamData()
		{
			if ( m_hStream != NULL)
			{
				AIL_close_stream( m_hStream);
			}
			if ( m_pStreamReader != NULL)
			{
				delete m_pStreamReader;
			}
		}
	};
	


public:
	PStreamManager(void);
	~PStreamManager(void);
	
	bool   Initialize(class ISoundBufferInfoTable *pBufferTable);	
	int    GetFreeStream( const char *szFileName);
	int    GetStreamIndex( const char *szFileName );
	bool   IsPlayStream( const char *szFileName );
	int    Play( const char *szFileName, PStreamReader*  pReader , int nLoop ,float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func);	
	int    PlayBgm( const char *szFileName, PStreamReader*  pReader , int nLoop ,float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func);	
	int    Play( const char *szFullPathName, const char *szFileName , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func);	
	int    PlayBgm( const char *szFullPathName, const char *szFileName , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func);	
	int    Play( int nIndex , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func);	
	int    PlayBgm( int nIndex , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func);	
	bool   UnLoad(const char *szFileName);
	void   Set_BGM_SFX( float fBGM, float fSFX);
	void   Set_BGM( float fBGM );
	void   Set_SFX( float fSFX );
	bool   SetPlayBackRate( const char *szFileName, int nRate);
	bool   Stop( const char *szFileName);
	
	bool   SetFade( const char* szFileName, float time, bool bFadeIn);
	void   UnLoadAllStream();
protected:
	void   RemoveAll();
	bool   SetVolume( int nIndex, float fLVolume, float fRVolume);
	bool   SetReverbLevel( int nIndex, float fDry, float fWet);	


protected:
	KHash<int, hashPr_string>      m_HashSound;	
	std::vector<TStreamData*>      m_vecStreamData;	
	float                          m_fBGM;
	float                          m_fSFX;	
	ISoundBufferInfoTable         *m_pBufferTable;
	HDIGDRIVER				       m_hDig;
};