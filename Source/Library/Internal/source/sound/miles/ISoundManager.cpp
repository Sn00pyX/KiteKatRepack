
#include <WTYPES.h>
#include "../../../include/sound/miles/PStreamReader.h"

#include "../../../include/kfile/KStream.h"
#include "../../../include/kfile/KFileManager.h"

#include "../../../include/toolkit/safe_function.h"

#include "../../../include/sound/miles/PStreamReader.h"

#include "../../../include/sound/miles/ISoundBufferInfoTable.h"
#include "../../../include/sound/miles/ISoundManager.h"
#include "../../../include/sound/miles/PSoundManager.h"
#include "../../../include/sound/miles/PStreamManager.h"
#include "../../../include/sound/miles/PSoundBufferManager.h"


//==============================================================================

struct StreamAdapter : public PStreamReader    // 이 cpp에서만 사용하는 구조체라서  cpp에 선언!
{
	StreamAdapter( KStream * p, const char *szFileName )
	{
		pStream = p;
		s_strcpy( szName, _countof( szName ), szFileName);
	}

	bool IsValid()
	{
		return pStream ? true : false;
	}

	size_t	Read( void * p, size_t len )
	{
		return pStream->Read( p, len );
	}
	bool	Seek( long pos, int cur )
	{
		KStream::enum_seek_origin nOrigin = KStream::seekCur;

		if ( cur == SEEK_CUR ) nOrigin = KStream::seekCur;
		if ( cur == SEEK_END ) nOrigin = KStream::seekEnd;
		if ( cur == SEEK_SET ) nOrigin = KStream::seekSet;

		pStream->Seek( pos, nOrigin );

		return true;
	}

	bool	IsEOF()
	{
		return pStream->Eos();
	}

	size_t	Tell()
	{
		return pStream->Tell();
	}

	size_t	GetLength()
	{
		return pStream->GetLength();
	}

	const char*  GetName()
	{
		return szName;
	}

	~StreamAdapter()
	{
		KFileManager::Instance().DeleteStream( pStream );
	}

	KStream *pStream;
	char     szName[256];
};

//******************************************************************************

ISoundManager::ISoundManager(void): m_bMute_Buffer(false), m_bMute_Stream(false)
{
	m_pSoundMng  = new PSoundManager;
	m_pStreamMng = new PStreamManager; 	
}

ISoundManager::~ISoundManager(void)
{
	RemoveAll();
}
	

bool  ISoundManager::Initialize( ISoundBufferInfoTable *pBufferTable, const char *szPathRedist,void* hWnd, const char *szChannelName, unsigned freq, int bit, unsigned flag)
{	
	bool  bSoundMng  =  m_pSoundMng->Initialize( pBufferTable, szPathRedist, (HWND)hWnd, szChannelName, freq, bit, flag);		
	bool  bStreamMng =  m_pStreamMng->Initialize( pBufferTable);		
	return ( bSoundMng && bStreamMng);
}

void  ISoundManager::RemoveAll()
{
	if ( m_pStreamMng)  delete m_pStreamMng;
	if ( m_pSoundMng)   delete m_pSoundMng;
}

//버퍼단위이다. 
bool  ISoundManager::UnLoad( const char *szFileName)
{
	return m_pSoundMng->UnLoad( szFileName);
}

//**** Buffer단위  Min,Max 설정 
bool  ISoundManager::SetMinMaxDist( int nBufferIndex, float fMin, float fMax)
{
	return m_pSoundMng->SetMinMaxDist( nBufferIndex, fMin, fMax);
}

//**** sample 단위
bool ISoundManager::SetMinMaxDist( int nBufferIndex, int nSampleIndex, float fMin, float fMax )
{
	return m_pSoundMng->SetMinMaxDist( nBufferIndex, nSampleIndex, fMin, fMax);
}

bool ISoundManager::SetAutoWet( int nBufferIndex ,bool  bAutoSet)
{
	return m_pSoundMng->SetAutoWet( nBufferIndex, bAutoSet);
}


bool  ISoundManager::SetPosition( float x, float y, float z)
{
	PSoundVector sv(x,y,z);
	return m_pSoundMng->SetPosition( sv );
}

bool  ISoundManager::SetOrientation( float face_x, float face_y, float face_z, float up_x, float up_y, float up_z)
{
	PSoundVector face( face_x, face_y, face_z);
	PSoundVector up( up_x, up_y, up_z);
	return m_pSoundMng->SetOrientation( face, up );
}


void  ISoundManager::SetMute_Buffer( bool bFlag )
{
	m_bMute_Buffer = bFlag;
}

bool  ISoundManager::SetMasterVolume( int nVolume )
{
	float fVolume = (float)nVolume / 100.0f;	
	return m_pSoundMng->SetMasterVolume(fVolume);
}

bool ISoundManager::SetBufferVolume( int nBufferIndex, int nLVolume, int nRVolume)
{
	float fLVolume = (float)nLVolume / 100.0f;
	float fRVolume = (float)nRVolume / 100.0f;

	return m_pSoundMng->SetBufferVolume( nBufferIndex, fLVolume, fRVolume);
}

bool ISoundManager::SetBufferVolume( int nLVolume, int nRVolume)
{
	float fLVolume = (float)nLVolume / 100.0f;
	float fRVolume = (float)nRVolume / 100.0f;

	return m_pSoundMng->SetBufferVolume( fLVolume, fRVolume);
}

void ISoundManager::SetSfxVolume( int nVolume )
{
	float fBGM = (float)nVolume / 100.0f;
	float fSFX = (float)nVolume / 100.0f;

	m_pSoundMng->Set_BGM_SFX( fBGM, fSFX);
}

int   ISoundManager::GetMasterVolume()
{
	float   fVolume = m_pSoundMng->GetMasterVolume();
	int     nVolume = (int)( fVolume * 100.0f);
	return  nVolume;
}

//Sample 단위이다. 
void  ISoundManager::Stop_AllSound()
{
	m_pSoundMng->StopAll();
}

void  ISoundManager::Stop_Sound(int nBufferIndex, int nSampleIndex)
{
	m_pSoundMng->Stop( nBufferIndex, nSampleIndex);	
}

//Buffer 단위이다. 
void ISoundManager::Stop_Sound( const char *szFileName)
{
	m_pSoundMng->Stop( szFileName);
}

void ISoundManager::Stop_Sound( int nBufferIndex)
{
	m_pSoundMng->Stop( nBufferIndex);
}

bool ISoundManager::Pause_Sound( int nBufferIndex, int nSampleIndex)
{
	return m_pSoundMng->Pause( nBufferIndex, nSampleIndex);
}

bool  ISoundManager::Play_Sound2D(bool bSampleLock, TSoundKey *pSoundKey, const char * pName , int nLoopCount , int nLVolume, int nRVolume, int nPlayRate)   //위치를 Listener의  x,y,z로 set한다.
{	
	if ( m_bMute_Buffer)	
		return false;
	
	float fLVolume = (float)nLVolume / 100.0f;
	float fRVolume = (float)nRVolume / 100.0f;

	TSoundKey tSoundKey;

	tSoundKey.m_strFileName = pName;
	tSoundKey.m_nBufferIndex = PreLoad( pName);  //PreLoad 호출해서 버퍼 인덱스 얻어온다.

	if ( tSoundKey.m_nBufferIndex == -1)
		return false;
	
	tSoundKey.m_nSampleIndex = m_pSoundMng->Play2D( tSoundKey.m_nBufferIndex, nLoopCount, fLVolume, fRVolume, nPlayRate, bSampleLock);

	if ( pSoundKey != NULL)
	{
		pSoundKey->m_strFileName  = tSoundKey.m_strFileName;
		pSoundKey->m_nBufferIndex = tSoundKey.m_nBufferIndex;
		pSoundKey->m_nSampleIndex = tSoundKey.m_nSampleIndex;
	}
	
	return true;
}

bool  ISoundManager::Play_Sound(bool bSampleLock, TSoundKey *pSoundKey, const char * pName, float x, float y, float z, int nLoopCount, int nLVolume, int nRVolume, int nPlayRate, bool bReverb, bool bLowPass )
{	
	if ( m_bMute_Buffer)	
		return false;

	float fLVolume = (float)nLVolume / 100.0f;
	float fRVolume = (float)nRVolume / 100.0f;


	TSoundKey tSoundKey;

	tSoundKey.m_strFileName = pName;
	tSoundKey.m_nBufferIndex = PreLoad( pName);   //PreLoad 호출해서 버퍼 인덱스 얻어온다.

	if ( tSoundKey.m_nBufferIndex == -1)
		return false;

	PSoundVector pos(x, y, z);
	tSoundKey.m_nSampleIndex = m_pSoundMng->Play( tSoundKey.m_nBufferIndex, pos, nLoopCount, fLVolume, fRVolume, nPlayRate, bSampleLock, bReverb, bLowPass ); 

	if ( tSoundKey.m_nSampleIndex == -1)
		return false; 

	if ( pSoundKey != NULL)
	{
		pSoundKey->m_strFileName  = tSoundKey.m_strFileName;
		pSoundKey->m_nBufferIndex = tSoundKey.m_nBufferIndex;
		pSoundKey->m_nSampleIndex = tSoundKey.m_nSampleIndex;
	}

	return true;
}

bool  ISoundManager::Play_SoundFromFile2D(bool bSampleLock, TSoundKey *pSoundKey, const char * pName , int nLoopCount , int nLVolume, int nRVolume, int nPlayRate)  //위치를 Listener의  x,y,z로 set한다.
{	
	if ( m_bMute_Buffer)	
		return false;

	float fLVolume = (float)nLVolume / 100.0f;
	float fRVolume = (float)nRVolume / 100.0f;


	TSoundKey  tSoundKey;

	tSoundKey.m_strFileName = pName;
	tSoundKey.m_nBufferIndex =  m_pSoundMng->Load( pName);  //PreLoad 호출해서 버퍼 인덱스 얻어온다.

	if ( tSoundKey.m_nBufferIndex == -1)
		return false;
	
	tSoundKey.m_nSampleIndex = m_pSoundMng->Play2D( tSoundKey.m_nBufferIndex, nLoopCount, fLVolume,fRVolume, nPlayRate, bSampleLock);

	if ( pSoundKey != NULL)
	{
		pSoundKey->m_strFileName  = tSoundKey.m_strFileName;
		pSoundKey->m_nBufferIndex = tSoundKey.m_nBufferIndex;
		pSoundKey->m_nSampleIndex = tSoundKey.m_nSampleIndex;
	}
	
	return true;
}

int   ISoundManager::PreLoad( const char * pName)
{
	//1. 일단 해당파일 이름의 버퍼가 있는지 검사
	int nIndex = m_pSoundMng->bHaveSoundBuffer( pName);


	//2. 아직 Load 안榮摸? Load한다.  
	if ( nIndex == -1)
	{
		StreamAdapter file( KFileManager::Instance().CreateStreamFromResource( pName ), pName );
		if ( file.pStream == NULL)
		{
			//assert( 0 && "ISoundManager::PreLoad 함수.  파일매니저에서" && pName && " 파일을 찾을수 없습니다!!");
			return -1;
		}
		nIndex = m_pSoundMng->Load( pName, &file);
	}


	//3. 리턴값은 버퍼의 인덱스가 되게한다. 	
	return nIndex;
}


//reverb 관련
bool ISoundManager::SetMasterReverb( float fReverbTime, float fReverb_Predelay, float fEvery_Damping)
{
	return m_pSoundMng->SetMasterReverb( fReverbTime, fReverb_Predelay, fEvery_Damping);
}

bool ISoundManager::SetMasterReverbLevel( int nDryLevel, int nWetLevel)
{
	float  fDryLevel =  (float)nDryLevel / 100.0f;
	float  fWetLevel =  (float)nWetLevel / 100.0f;
	return m_pSoundMng->SetMasterReverbLevel( fDryLevel, fWetLevel);
}

bool ISoundManager::SetRoomType( int nRoom_Type)
{
	return m_pSoundMng->SetRoomType( nRoom_Type);
}


// BGM,  SFX  값 셋하기!!
void  ISoundManager::Set_BGM_SFX( int nBGM, int nSFX)
{
	float fBGM = (float)nBGM / 100.0f;
	float fSFX = (float)nSFX / 100.0f;

	m_pSoundMng->Set_BGM_SFX( fBGM, fSFX);
	m_pStreamMng->Set_BGM_SFX( fBGM, fSFX);
}

//fade
bool   ISoundManager::SetFade_Buffer( const char *szFileName, int time, bool bFadeIn)
{	
	return m_pSoundMng->SetFade_Buffer( szFileName, (float)time, bFadeIn);
}

bool ISoundManager::Resume_Sound( int nBufferIndex, int nSampleIndex)
{
	return m_pSoundMng->Resume_Sound( nBufferIndex, nSampleIndex);
}


bool ISoundManager::SetPlayBackRate( int nBufferIndex, int nSampleIndex, int nRate)
{
	return m_pSoundMng->SetPlayBackRate( nBufferIndex, nSampleIndex, nRate);
}

void ISoundManager::SetPosition_Sample( int nBufferIndex, int nSampleIndex, float x, float y, float z)
{
	return m_pSoundMng->SetPosition_Sample( nBufferIndex, nSampleIndex, x, y, z);
}






//   **********   Stream  Play 관련 *************
void  ISoundManager::SetMute_Stream( bool bFlag )
{
	m_bMute_Stream = bFlag;
}

int    ISoundManager::Play_Stream( const char *szFileName , int nLoop, int nLVolume, int  nRVolume, AILSTREAMCB  CallBack_Func)
{
	if ( m_bMute_Stream)	
		return false;

	float fLVolume = (float)nLVolume / 100.0f;
	float fRVolume = (float)nRVolume / 100.0f;
	
	int nIndex = m_pStreamMng->GetFreeStream( szFileName);
	if ( nIndex != -1)
	{
		m_pStreamMng->Play( nIndex, nLoop, fLVolume, fRVolume, CallBack_Func);
		return nIndex;
	}

	StreamAdapter *pfile = new StreamAdapter( KFileManager::Instance().CreateStreamFromResource( szFileName ), szFileName );
	
	if ( pfile->pStream == NULL)
	{
		delete pfile;
		//assert( 0 && "ISoundManager::Play_Stream 부분");
		_oprint("ISoundManager::Play_Stream 부분  %s 파일이 없음\n", szFileName);
		return -1;
	}

	return m_pStreamMng->Play( szFileName,pfile, nLoop, fLVolume, fRVolume,  CallBack_Func);	
}

int ISoundManager::Play_Bgm_Stream( const char *szFileName , int nLoop, int nLVolume, int  nRVolume, AILSTREAMCB  CallBack_Func )
{
	if ( m_bMute_Stream)	
		return false;

	float fLVolume = (float)nLVolume / 100.0f;
	float fRVolume = (float)nRVolume / 100.0f;
	
	int nIndex = m_pStreamMng->GetFreeStream( szFileName);
	if ( nIndex != -1)
	{
		m_pStreamMng->Play( nIndex, nLoop, fLVolume, fRVolume, CallBack_Func);
		return nIndex;
	}

	StreamAdapter *pfile = new StreamAdapter( KFileManager::Instance().CreateStreamFromResource( szFileName ), szFileName );
	
	if ( pfile->pStream == NULL)
	{
		delete pfile;
		//assert( 0 && "ISoundManager::Play_Stream 부분");
		_oprint("ISoundManager::Play_Stream 부분  %s 파일이 없음\n", szFileName);
		return -1;
	}

	return m_pStreamMng->PlayBgm( szFileName, pfile, nLoop, fLVolume, fRVolume,  CallBack_Func );	
}

int ISoundManager::GetStreamIndex( const char *szFileName )
{
	return m_pStreamMng->GetStreamIndex( szFileName);
}

int    ISoundManager::Play_StreamFromFile( const char *szFullPathName, const char *szFileName , int nLoop, int nLVolume, int  nRVolume, AILSTREAMCB  CallBack_Func)
{	
	if ( m_bMute_Stream)	
		return false;

	float fLVolume = (float)nLVolume / 100.0f;
	float fRVolume = (float)nRVolume / 100.0f;

	int nIndex = m_pStreamMng->GetFreeStream( szFileName);
	if ( nIndex != -1)
	{
		m_pStreamMng->Play( nIndex, nLoop, fLVolume, fRVolume, CallBack_Func);
		return nIndex;
	}

	return m_pStreamMng->Play( szFullPathName, szFileName, nLoop, fLVolume, fRVolume, CallBack_Func);	
}

bool   ISoundManager::UnLoad_Stream(const char *szFileName)
{
	return m_pStreamMng->UnLoad( szFileName);
}

void   ISoundManager::UnLoadAllStream()
{
	m_pStreamMng->UnLoadAllStream();
}

bool   ISoundManager::SetPlayBackRate_Stream( const char *szFileName, int nRate)
{
	return m_pStreamMng->SetPlayBackRate( szFileName, nRate);
}

bool   ISoundManager::Stop_Stream(const char *szFileName)
{
	return m_pStreamMng->Stop( szFileName);
}

bool   ISoundManager::SetFade_Stream( const char *szFileName, int time, bool bFadeIn)
{	
	return m_pStreamMng->SetFade( szFileName, (float)time, bFadeIn);
}

void ISoundManager::SetBgmSfxVolume( int nBgnVolume, int nSfxVolume )
{
	float fBGM = (float)nBgnVolume / 100.0f;
	float fSFX = (float)nSfxVolume / 100.0f;

	m_pStreamMng->Set_BGM_SFX( fBGM, fSFX);
}

void ISoundManager::SetBgmVolume( int nVolume )
{
	float fBGM = (float)nVolume / 100.0f;

	m_pStreamMng->Set_BGM( fBGM );
}

void ISoundManager::SetEnvVolume( int nVolume )
{
	float fSFX = (float)nVolume / 100.0f;

	m_pStreamMng->Set_SFX( fSFX );
}

bool ISoundManager::IsPlayStream( const char *szFileName )
{
	return m_pStreamMng->IsPlayStream( szFileName );
}