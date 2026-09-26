
#include <WTYPES.h>
#include <assert.h>
//#include <string>

#include "../../../include/toolkit/ILock.h"


#include <miles/mss.h>

#include "../../../include/sound/miles/PStreamReader.h"
#include "../../../include/sound/miles/ISoundBufferInfoTable.h"
#include "../../../include/sound/miles/PSoundBufferManager.h"
#include "../../../include/sound/miles/PStreamManager.h"
#include "../../../include/sound/miles/PSoundManager.h"
#include "../../../include/sound/miles/PSoundFilterManager.h"







struct CHANNEL_ENTRY
{
   C8         *name;   
   MSS_MC_SPEC channel_spec;
};

const int CHANNEL_ENTRY_COUNT = 15;

static CHANNEL_ENTRY   g_Channel_entries[ CHANNEL_ENTRY_COUNT] = 
{
   { "Use Windows speaker configuration", MSS_MC_USE_SYSTEM_CONFIG   },
   { "Mono",                              MSS_MC_MONO                },
   { "Stereo headphones",                 MSS_MC_HEADPHONES          },
   { "Stereo speakers",                   MSS_MC_STEREO              },
   { "Dolby Surround",                    MSS_MC_DOLBY_SURROUND      },
   { "SRS Circle Surround",               MSS_MC_SRS_CIRCLE_SURROUND },
   { "4.0 discrete",                      MSS_MC_40_DISCRETE         },
   { "5.1 discrete",                      MSS_MC_51_DISCRETE         },
   { "6.1 discrete",                      MSS_MC_61_DISCRETE         },
   { "7.1 discrete",                      MSS_MC_71_DISCRETE         },
   { "8.1 discrete",                      MSS_MC_81_DISCRETE         },
   { "DirectSound 3D hardware support",   MSS_MC_DIRECTSOUND3D       },
   { "Creative Labs EAX 2 (TM)",          MSS_MC_EAX2                },
   { "Creative Labs EAX 3 (TM)",          MSS_MC_EAX3                },
   { "Creative Labs EAX 4 (TM)",          MSS_MC_EAX4                },
};


static HDIGDRIVER	     g_hDig;

static  XCriticalSection g_cs;
static  PStreamReader*   g_pStream[16]  = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
static  PStreamReader*   g_pInputStream = NULL;



//*** pSoundBuffer매니저의 Sample
TSAMPLE    PSoundBufferManager::m_tSamples[ MAX_SOUND_SAMPLE];		




PSoundManager::PSoundManager(void)
{
	for ( int i=0; i<MAX_SOUND_BUFFER; ++i)
	{		
		m_pBuffer[i] = NULL;
	}

	for ( int i=MAX_SOUND_BUFFER-1;  i>=0; --i)    // Free Buffer 은 뒤에서 부터 삽입한다. - 그다지 상관은 없지만 ㅡ_ㅡ;;
	{	
		m_vecFreeBuffer.push_back( i);
	}	

	/*//	listener position vector
	m_posX = 0.0f;
	m_posY = 0.0f;
	m_posZ = 0.0f;

	//	orientation face vector
	m_faceX = 0.0f;
	m_faceY = 0.0f;
	m_faceZ = 0.0f;

	//	orientation up vector
	m_upX = 0.0f;
	m_upY = 0.0f;
	m_upZ = 0.0f;
	*/
	
	m_fBGM = 1.0f;
	m_fSFX = 1.0f;

	m_hDig = NULL;
}

PSoundManager::~PSoundManager(void)
{
	//m_pBuffer
	for ( int i=0; i<MAX_SOUND_BUFFER; ++i)
	{		
		if ( m_pBuffer[i] == NULL)
			continue;

		delete m_pBuffer[i];
	}


	//m_hDig
	AIL_close_digital_driver( m_hDig);
	AIL_shutdown();
}

//초기화 
bool PSoundManager::Initialize(class ISoundBufferInfoTable *pBufferTable, const char *szPathRedist, HWND hWnd, const char *szChannelName, unsigned freq, int bit, unsigned flag)
{
	m_pBufferTable = pBufferTable;

	int i = 0;
	for ( i=0; i<CHANNEL_ENTRY_COUNT; ++i)
	{
		if ( !strcmp( szChannelName, g_Channel_entries[i].name))
			break;
	}
	
	if ( i >= CHANNEL_ENTRY_COUNT)
	{
		assert( 0 && "PSoundManager::Initialize함수 szChannelName에 해당하는 채널엔트리가 없습니다!!");
		return false;
	}

	if ( szPathRedist == NULL)
	{
		AIL_set_redist_directory(".\\redist" );
	}
	else
	{
		AIL_set_redist_directory( szPathRedist);
	}

    AIL_startup();
	m_hDig = AIL_open_digital_driver( freq, 16, g_Channel_entries[i].channel_spec, flag);	
	if ( m_hDig == NULL)
	{
		assert(  0 && "PSoundManager::Initialize함수 dig생성실패:" &&  AIL_last_error());
		return false;
	}	
	
	g_hDig = m_hDig;
	AIL_set_DirectSound_HWND( m_hDig, hWnd);
	AIL_set_digital_master_volume_level(m_hDig,  DEF_MASTER_VOLUME);	
	AIL_set_file_callbacks( PSoundManager::AIL_file_open_callback, PSoundManager::AIL_file_close_callback, 
		                    PSoundManager::AIL_file_seek_callback, PSoundManager::AIL_file_read_callback);	
	
	
	if ( AIL_room_type( m_hDig) == -1)
		assert( 0 && " PSoundManager::Initialize  - SetRoomType를 지원하지 않습니다!!!");
	
	//*** 사운드 필터 초기화.
	SOUND_FILTER().Initialize();

	return true;
}

// 버퍼 생성 
int PSoundManager::Load( const char *szFileName, PStreamReader* pStream )
{
	//리턴값은 버퍼 인덱스가 되게한다. -실패시 -1 리턴
	int nIndex = bHaveSoundBuffer( szFileName);  
	if ( nIndex != -1)
	{
		return nIndex;
	}

	if ( m_vecFreeBuffer.size() <= 0)
	{
		assert( 0 && "함수 PSoundManager::Load -  사용할수 있는 Free Buffer이 없습니다.");
		return -1;
	}

	nIndex = m_vecFreeBuffer.back();
	m_vecFreeBuffer.pop_back();

	m_pBuffer[ nIndex] = new PSoundBufferManager( m_pBufferTable, pStream, m_hDig, &m_fBGM, &m_fSFX , nIndex);
	m_hashSound.add( szFileName, nIndex);
	return nIndex;
}

int  PSoundManager::Load( const char *szFileName)
{
	PFileStreamReader  FileStream( szFileName);
	return Load( szFileName, &FileStream);
}

bool PSoundManager::UnLoad(const char *szFileName)  
{
	int nBufferIndex = bHaveSoundBuffer( szFileName);

	if ( m_pBuffer[ nBufferIndex] == NULL)
	{
		m_hashSound.erase( szFileName);

		size_t nSize = m_vecFreeBuffer.size();
		for ( size_t i=0; i<nSize; ++i)
		{
			if ( nBufferIndex == m_vecFreeBuffer[i])
				return false;			
		}		

		m_vecFreeBuffer.push_back( nBufferIndex);  
		return false; 
	}

	delete m_pBuffer[ nBufferIndex];

	m_pBuffer[ nBufferIndex] = NULL;
	m_vecFreeBuffer.push_back( nBufferIndex);
	m_hashSound.erase( szFileName);
	return true;
}

bool PSoundManager::SetAutoWet( int nBufferIndex ,bool  bAutoSet)
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return false; 

	return  m_pBuffer[ nBufferIndex]->SetAutoWet( bAutoSet);
}

bool PSoundManager::SetMinMaxDist(int nBufferIndex , float fMin, float fMax )
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return false; 

	return  m_pBuffer[ nBufferIndex]->SetMinMaxDist( fMin, fMax);
}

bool PSoundManager::SetMinMaxDist(int nBufferIndex, int nSampleIndex , float fMin, float fMax )
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return false; 

	return  m_pBuffer[ nBufferIndex]->SetMinMaxDist( nSampleIndex, fMin, fMax);
}

bool PSoundManager::SetPosition( PSoundVector  vecPos )
{
	if( m_hDig == NULL ) return false;

	AIL_set_listener_3D_position( m_hDig, vecPos.m_fx, vecPos.m_fy, vecPos.m_fz);
	return true;
}

bool PSoundManager::SetOrientation( PSoundVector vecFace, PSoundVector vecUp)
{
	if( m_hDig == NULL ) return false;

	AIL_set_listener_3D_orientation( m_hDig, vecFace.m_fx, vecFace.m_fy, vecFace.m_fy, vecUp.m_fx, vecUp.m_fy, vecUp.m_fz);
	return true;
}

bool PSoundManager::SetMasterVolume( float fVolume)
{
	if( m_hDig == NULL ) return false;

	AIL_set_digital_master_volume_level( m_hDig, fVolume);
	return true;
}

bool PSoundManager::SetBufferVolume( int nBufferIndex,  float fLVolume, float fRVolume)
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return false;

	m_pBuffer[ nBufferIndex]->SetVolume( fLVolume, fRVolume);
	return true;
}

bool PSoundManager::SetBufferVolume( float fLVolume, float fRVolume )
{
	for( int nBufferIndex = 0; nBufferIndex < MAX_SOUND_BUFFER; ++nBufferIndex )
	{
		if( m_pBuffer[nBufferIndex] )
			m_pBuffer[nBufferIndex]->SetVolume( fLVolume, fRVolume);
	}

	return true;
}

float  PSoundManager::GetMasterVolume() const
{
	if( m_hDig == NULL ) return 0.0f;

	return  AIL_digital_master_volume_level(m_hDig);
}


//해당 파일의  버퍼가 생성되 있는지 여부 검사. 
int PSoundManager::bHaveSoundBuffer(const char *szFileName)
{
	// 리턴값 - 생성되 있다면 인덱스 ,  아직 생성안되있다면 -1리턴 
	int nIndex = -1;
	m_hashSound.lookup( szFileName, nIndex );   
	return nIndex;                              //해쉬에 아직 등록이 안되있는것이라면 nIndex를 건드리지 않기때문에 -1
}


// Sample의  x,y,z는 현재 Listener의 위치 얻어서 set한다. 
int  PSoundManager::Play2D( int nBufferIndex, int nLoopCount, float fLVolume, float fRVolume, int nPlayRate, bool bLock)
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return -1;

	//리턴값이 Sample인덱스가 되게한다. 실패시 -1리턴	

	return m_pBuffer[ nBufferIndex]->Play2D(nLoopCount, fLVolume, fRVolume, nPlayRate, bLock);
}


int  PSoundManager::Play( int nBufferIndex, PSoundVector vecPos, int nLoopCount, float fLVolume, float fRVolume, int nPlayRate, bool bLock, bool bReverb, bool bLowPass ) 
{	
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return -1;

	//리턴값이 Sample인덱스가 되게한다. 실패시 -1리턴
	return m_pBuffer[ nBufferIndex]->Play( vecPos, nLoopCount, fLVolume, fRVolume, nPlayRate, bLock, bReverb, bLowPass );   //리턴값은 Sample Index 	 
}

bool PSoundManager::StopAll()
{
	for( int x = 0; x < MAX_SOUND_BUFFER; ++x )
	{
		if( m_pBuffer[x] ) m_pBuffer[x]->Stop();
	}
	
	return true;
}

bool PSoundManager::Stop( int nBufferIndex , int nSampleIndex)
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return false;

	return m_pBuffer[ nBufferIndex]->Stop( nSampleIndex);
}

bool PSoundManager::Stop( int nBufferIndex)
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
	{	
		return false;
	}

	return m_pBuffer[ nBufferIndex]->Stop();
}

bool PSoundManager::Stop( const char *szFileName)
{
	int nIndex = -1;
	if ( m_hashSound.lookup( szFileName, nIndex) == false)
	{
		return false;
	}

	if ( m_pBuffer[ nIndex] == NULL)
	{
		assert( 0 && "PSoundManager::Stop( const char *szFileName) 뭔가잘못榮募?.");
		return false;
	}

	return m_pBuffer[ nIndex]->Stop();
}

bool PSoundManager::Pause( int nBufferIndex, int nSampleIndex)
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return false;

	return m_pBuffer[ nBufferIndex]->Pause( nSampleIndex);
}

bool PSoundManager::Resume_Sound( int nBufferIndex, int nSampleIndex)
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return false;

	return m_pBuffer[ nBufferIndex]->Resume_Sound( nSampleIndex);
}

bool PSoundManager::SetPlayBackRate( int nBufferIndex, int nSampleIndex, int nRate)
{
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return false;

	return m_pBuffer[ nBufferIndex]->SetPlayBackRate( nSampleIndex, nRate);
}

void PSoundManager::SetPosition_Sample( int nBufferIndex, int nSampleIndex, float x, float y, float z)
{	
	if ( m_pBuffer[ nBufferIndex] == NULL)
		return;

	PSoundVector sv(x,y,z);
	return m_pBuffer[ nBufferIndex]->SetPosition_Sample( nSampleIndex, sv);
}

//reverb 관련
bool PSoundManager::SetMasterReverb( float fReverbTime, float fReverb_Predelay, float fEvery_Damping)
{
	if ( m_hDig == NULL) return false;

	AIL_set_digital_master_reverb( m_hDig, fReverbTime, fReverb_Predelay, fEvery_Damping);
	return true;
}

bool PSoundManager::SetMasterReverbLevel( float fDryLevel, float fWetLevel)
{
	if ( m_hDig == NULL) return false;
	
	AIL_set_digital_master_reverb_levels( m_hDig, fDryLevel, fWetLevel);
	return true;
}

bool PSoundManager::SetRoomType( int nRoom_Type)
{
	if ( m_hDig == NULL) return false;

	AIL_set_room_type( m_hDig, nRoom_Type);
	return true;
}

void PSoundManager::Set_BGM_SFX( float fBGM, float fSFX)
{
	m_fBGM = fBGM;
	m_fSFX = fSFX;

	//버퍼마다 돌면서 해주기 
	for ( int i=0; i<MAX_SOUND_BUFFER  ; ++i)
	{
		if ( m_pBuffer[i] != NULL)
			m_pBuffer[i]->ResetVolume();
	}
}

//fade
bool PSoundManager::SetFade_Buffer( const char *szFileName, float time, bool bFadeIn)
{
	int nIndex = -1;
	if ( m_hashSound.lookup( szFileName, nIndex) == false)
	{
		return false;
	}

	if ( m_pBuffer[ nIndex] == NULL)
	{
		assert( 0 && "PSoundManager::SetFade_Buffer   뭔가잘못榮募?.");
		return false;
	}

	return m_pBuffer[ nIndex]->SetFade( time, bFadeIn);
}



//// ***********************  마일즈  CALL BACK 함수 *************************************************************

U32 AILCALLBACK  PSoundManager::AIL_file_open_callback( MSS_FILE const FAR* filename,  UINTa FAR* file_handle)
{
	//	입력 Stream 이 NULL 이면 실패
	if (g_pInputStream==NULL)
	{
		return 0;
	}

	//	비어있는 FileHandle 을 찾는다
	int idx = -1;
	for (int i=0; i<16; i++)
	{
		if (g_pStream[i]==NULL)
		{
			idx = i;
			break;
		}
	}

	if (idx==-1 || idx==16)
	{
		return 0;
	}

	//	비어있는 FileHandle 에 pStream assign
	g_pStream[idx] = g_pInputStream;	
	*file_handle = idx;
	return true;
}

void AILCALLBACK PSoundManager::AIL_file_close_callback(UINTa FileHandle)
{
	if (g_pStream[ FileHandle])
	{
		g_pStream[ FileHandle] = NULL;		
	}
}

S32 AILCALLBACK  PSoundManager::AIL_file_seek_callback(UINTa FileHandle, S32 offset, U32 type)
{
	if (g_pStream[FileHandle])
	{
		switch(type)
		{
		case AIL_FILE_SEEK_BEGIN :
			g_pStream[FileHandle]->Seek( offset, SEEK_SET);			
			break;

		case AIL_FILE_SEEK_CURRENT :
			g_pStream[FileHandle]->Seek( offset, SEEK_CUR);			
			break;

		case AIL_FILE_SEEK_END :			
			g_pStream[FileHandle]->Seek( offset, SEEK_END);		
			break;
		}

		return static_cast< S32 >( g_pStream[FileHandle]->Tell() );
	}
	else
	{
		return -1;
	}
}

U32 AILCALLBACK  PSoundManager::AIL_file_read_callback (UINTa FileHandle,void FAR* buffer, U32 bytes)
{
	if (g_pStream[FileHandle])
	{
		return (U32)g_pStream[FileHandle]->Read(buffer, bytes);
	}
	else
	{
		return 0;
	}
}







//*********************************************    PStreamManager ***************************************************************

PStreamManager::PStreamManager(void): m_fBGM( 1.0f), m_fSFX( 1.0f)
{
	m_pBufferTable = NULL;
}


PStreamManager::~PStreamManager(void)
{
	RemoveAll();
}


bool  PStreamManager::Initialize(class ISoundBufferInfoTable *pBufferTable)
{
	m_pBufferTable = pBufferTable;	
    m_hDig = g_hDig;

	if ( m_hDig == NULL)
		return false;

	return true;
}


void  PStreamManager::RemoveAll()
{
	size_t nSize = m_vecStreamData.size();
	
	for ( size_t i=0; i<nSize; ++i)
	{
		delete  m_vecStreamData[ i];
	}
	
	m_vecStreamData.clear();
	m_HashSound.clear();
}

int    PStreamManager::GetFreeStream( const char *szFileName)
{
	int nIndex = -1;
	m_HashSound.lookup( szFileName, nIndex );  

	if (nIndex != -1)
	{
		HSAMPLE sample = AIL_stream_sample_handle( m_vecStreamData[ nIndex]->m_hStream);	
		AIL_set_sample_processor( sample, SP_FILTER_0, 0 );	

		int state = AIL_stream_status(m_vecStreamData[ nIndex]->m_hStream);
		if ( (state == SMP_STOPPED) || (state == SMP_DONE))		
			return nIndex;	
		else
		{
			AIL_pause_stream( m_vecStreamData[ nIndex]->m_hStream, 1);   //스트림은 같은소리를 동시에 1개이상 Play시키지 않게하자!!
			return nIndex;
		}
	}
	return -1;
}

int PStreamManager::GetStreamIndex( const char *szFileName )
{
	int nIndex = -1;
	m_HashSound.lookup( szFileName, nIndex ); 
	return nIndex;
}

bool PStreamManager::IsPlayStream( const char *szFileName )
{
	int nIndex = -1;
	m_HashSound.lookup( szFileName, nIndex );  

	if (nIndex != -1)
	{
		int state = AIL_stream_status(m_vecStreamData[ nIndex]->m_hStream);
		if ( state == SMP_PLAYING )
			return true;
	}
	return false;
}

int    PStreamManager::Play( const char *szFileName, PStreamReader*  pReader ,int nLoop , float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func)
{
	HSTREAM  stream;
	float    fDry = 1.0f;
	float    fWet = 1.0f;
	
	
	{
		THREAD_SYNCRONIZE(g_cs);
		g_pInputStream = pReader;		
		stream = AIL_open_stream( m_hDig, szFileName, 0 );
		if ( stream == NULL)
		{
			//char *szErr = AIL_last_error();
			delete pReader;
			return -1;
		}
		g_pInputStream = NULL;
	}

	if ( stream == 0 )
	{
		return -1;
	}	



	TStreamData *pData           = new TStreamData;
	pData->m_strFileName         = szFileName;
	pData->m_pStreamReader       = pReader;
	pData->m_hStream             = stream;		
	pData->m_fLVolume            = fLVolume;
	pData->m_fRVolume            = fRVolume;

	if( m_pBufferTable ) m_pBufferTable->GetSoundBufferData( szFileName, NULL, NULL, &pData->m_fBufferVolumeFactor, &fDry, &fWet, &pData->m_bSfx);
	//SD님의 요청으로 Stream류는 Wet적용이 안됨
//	else
//	{
		fDry = 1.0f;
		fWet = 0.0f;
//	}


	int nIndex = static_cast< int >( m_vecStreamData.size() );
	m_vecStreamData.push_back( pData);

	SetVolume( nIndex , fLVolume, fRVolume);
	SetReverbLevel(nIndex , fDry, fWet);

	AIL_set_stream_loop_count( stream, nLoop);	
	AIL_start_stream( stream );
	

	if ( CallBack_Func != NULL)
	{
		AIL_register_stream_callback( stream, CallBack_Func);  //스트림 Play가 끝났을때 호출되는 컬백함수 등록
	}

	m_HashSound.add( szFileName, nIndex);
	return nIndex;
}

int PStreamManager::PlayBgm( const char *szFileName, PStreamReader*  pReader , int nLoop ,float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func)
{
	HSTREAM  stream;
	float    fDry = 1.0f;
	float    fWet = 1.0f;

	{
		THREAD_SYNCRONIZE(g_cs);
		g_pInputStream = pReader;		
		stream = AIL_open_stream( m_hDig, szFileName, 0 );
		if ( stream == NULL)
		{
			//char *szErr = AIL_last_error();
			delete pReader;
			return -1;
		}
		g_pInputStream = NULL;
	}

	if ( stream == 0 ) return -1;	

	TStreamData *pData           = new TStreamData;
	pData->m_strFileName         = szFileName;
	pData->m_pStreamReader       = pReader;
	pData->m_hStream             = stream;		
	pData->m_fLVolume            = fLVolume;
	pData->m_fRVolume            = fRVolume;

	pData->m_fBufferVolumeFactor = 1.0f;
	fDry						 = 1.0f;
	fWet						 = 0.0f;
	pData->m_bSfx				 = false;

	int nIndex = static_cast< int >( m_vecStreamData.size() );
	m_vecStreamData.push_back( pData);

	SetVolume( nIndex , fLVolume, fRVolume);
	SetReverbLevel(nIndex , fDry, fWet);

	AIL_set_stream_loop_count( stream, nLoop);	
	AIL_start_stream( stream );
	

	if ( CallBack_Func != NULL)
	{
		AIL_register_stream_callback( stream, CallBack_Func);  //스트림 Play가 끝났을때 호출되는 컬백함수 등록
	}

	m_HashSound.add( szFileName, nIndex);
	return nIndex;
}

int    PStreamManager::Play(const char *szFullPathName, const char *szFileName , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func)
{
	PFileStreamReader  *pFileStream = new PFileStreamReader( szFullPathName);
	return Play( szFileName, pFileStream, nLoop, fLVolume, fRVolume,  CallBack_Func);
}

int    PStreamManager::PlayBgm(const char *szFullPathName, const char *szFileName , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func)
{
	PFileStreamReader  *pFileStream = new PFileStreamReader( szFullPathName);
	return PlayBgm( szFileName, pFileStream, nLoop, fLVolume, fRVolume,  CallBack_Func);
}

int    PStreamManager::Play( int nIndex , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func)
{
	if ( (nIndex == -1) || (static_cast<size_t>(nIndex) >= m_vecStreamData.size()))
	{
		assert( 0 && "PStreamManager::Play( int nIndex , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func)함수 nIndex 잘못들어옴!!");
		return -1;
	}

	AIL_start_stream( m_vecStreamData[ nIndex]->m_hStream);
	AIL_set_stream_loop_count( m_vecStreamData[ nIndex]->m_hStream, nLoop);	
	SetVolume( nIndex , fLVolume, fRVolume);
	return nIndex;
}

int    PStreamManager::PlayBgm( int nIndex , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func)
{
	if ( (nIndex == -1) || (static_cast<size_t>(nIndex) >= m_vecStreamData.size()))
	{
		assert( 0 && "PStreamManager::Play( int nIndex , int nLoop, float fLVolume, float fRVolume, AILSTREAMCB  CallBack_Func)함수 nIndex 잘못들어옴!!");
		return -1;
	}

	m_vecStreamData[ nIndex]->m_bSfx = false;
	AIL_start_stream( m_vecStreamData[ nIndex]->m_hStream);
	AIL_set_stream_loop_count( m_vecStreamData[ nIndex]->m_hStream, nLoop);	
	SetVolume( nIndex , fLVolume, fRVolume);
	return nIndex;
}

bool   PStreamManager::UnLoad(const char *szFileName)
{
	int nIndex = -1;
	m_HashSound.lookup( szFileName, nIndex );  
	
	if ( (nIndex == -1) || ( static_cast<size_t>(nIndex) >= m_vecStreamData.size()))
		return false;

	delete  m_vecStreamData[ nIndex];   //구조체 소멸자에서  마일즈 AIL_close_stream 호출함!!
	m_vecStreamData.erase(  m_vecStreamData.begin() + nIndex);
	

	//*** Hash에 있는 정보를 다시 set하자. 	
	m_HashSound.clear();
	int size = static_cast< int >( m_vecStreamData.size() );
	for ( int i=0; i<size; ++i)
	{
		m_HashSound.add( m_vecStreamData[i]->m_strFileName.c_str(), i);
	}

	return true;
}

void PStreamManager::UnLoadAllStream()
{
	RemoveAll();
}

bool   PStreamManager::SetVolume( int nIndex, float fLVolume, float fRVolume)
{
	if ( static_cast<size_t>(nIndex) >= m_vecStreamData.size())
		return false;

	TStreamData* pStreamData = m_vecStreamData[ nIndex];
	pStreamData->m_fLVolume = fLVolume;
	pStreamData->m_fRVolume = fRVolume;

	float fSFX_BGM       = m_vecStreamData[ nIndex]->m_bSfx ? m_fSFX : m_fBGM;
	float fLResultVolume = fLVolume * VOLUME_FACTOR * pStreamData->m_fBufferVolumeFactor * fSFX_BGM;
	float fRResultVolume = fRVolume * VOLUME_FACTOR * pStreamData->m_fBufferVolumeFactor * fSFX_BGM;	
	
	HSAMPLE sample = AIL_stream_sample_handle ( pStreamData->m_hStream);
	AIL_set_sample_volume_levels( sample, fLResultVolume, fRResultVolume);

	return true;
}

bool   PStreamManager::SetReverbLevel( int nIndex, float fDry, float fWet)
{
	if ( static_cast<size_t>(nIndex) >= m_vecStreamData.size())
		return false;

	TStreamData* pStreamData = m_vecStreamData[ nIndex];
	
	HSAMPLE sample = AIL_stream_sample_handle ( pStreamData->m_hStream);
	AIL_set_sample_reverb_levels( sample, fDry, fWet);

	return true;
}

void PStreamManager::Set_BGM_SFX( float fBGM, float fSFX)
{
	m_fBGM = fBGM;
	m_fSFX = fSFX;

	int nSize = static_cast< int >( m_vecStreamData.size() );

	for ( int i=0; i<nSize; ++i)
	{
		SetVolume( i, m_vecStreamData[i]->m_fLVolume, m_vecStreamData[i]->m_fRVolume);
	}
}

void PStreamManager::Set_BGM( float fBGM )
{
	m_fBGM = fBGM;

	int nSize = static_cast< int >( m_vecStreamData.size() );
	for ( int i=0; i<nSize; ++i)
	{
		SetVolume( i, m_vecStreamData[i]->m_fLVolume, m_vecStreamData[i]->m_fRVolume);
	}
}

void PStreamManager::Set_SFX( float fSFX )
{
	m_fSFX = fSFX;

	int nSize = static_cast< int >( m_vecStreamData.size() );
	for ( int i=0; i<nSize; ++i)
	{
		SetVolume( i, m_vecStreamData[i]->m_fLVolume, m_vecStreamData[i]->m_fRVolume);
	}
}

bool PStreamManager::SetPlayBackRate( const char *szFileName, int nRate)
{
	int nIndex = -1;
	m_HashSound.lookup( szFileName, nIndex );  

	if ( (nIndex == -1) || (static_cast<size_t>(nIndex) >= m_vecStreamData.size()))
		return false;

	HSAMPLE sample = AIL_stream_sample_handle ( m_vecStreamData[ nIndex]->m_hStream);
	AIL_set_sample_playback_rate( sample, nRate);
	return true;
}


bool PStreamManager::Stop( const char *szFileName)
{
	int nIndex = -1;
	m_HashSound.lookup( szFileName, nIndex );  

	if ( (nIndex == -1) || (static_cast<size_t>(nIndex) >= m_vecStreamData.size()))
		return false;

	AIL_pause_stream( m_vecStreamData[ nIndex]->m_hStream, 1);	
	//AIL_set_stream_position( m_vecStreamData[ nIndex]->m_hStream, 0);
	return true;
}

bool  PStreamManager::SetFade( const char *szFileName, float time, bool bFadeIn)
{
	HPROVIDER filter = SOUND_FILTER().GetFilter("Volume Ramp Filter");

	if ( !filter)
		return false;

	int nIndex = -1;
	if ( m_HashSound.lookup( szFileName, nIndex))
	{
		// fade
		HSAMPLE sample = AIL_stream_sample_handle( m_vecStreamData[ nIndex]->m_hStream);			

		AIL_set_sample_processor( sample, SP_FILTER_0, 0 );	
		AIL_set_sample_processor( sample, SP_FILTER_0, filter );	
		
		if ( bFadeIn)
		{
			float ramp_time  = 0.0f;
			float ramp_level = 0.0f;
			AIL_sample_stage_property( sample, SP_FILTER_0, "Ramp Time", 0, &ramp_time , 0);
			AIL_sample_stage_property( sample, SP_FILTER_0, "Ramp To"  , 0, &ramp_level, 0);	

			ramp_time  = time;
			ramp_level = 1.0f;
			AIL_sample_stage_property( sample, SP_FILTER_0, "Ramp Time", 0, &ramp_time , 0);
			AIL_sample_stage_property( sample, SP_FILTER_0, "Ramp To"  , 0, &ramp_level, 0);	
		}
		else
		{
			float ramp_time  = 0.0f;
			float ramp_level = 1.0f;
			AIL_sample_stage_property( sample, SP_FILTER_0, "Ramp Time", 0, &ramp_time , 0);
			AIL_sample_stage_property( sample, SP_FILTER_0, "Ramp To"  , 0, &ramp_level, 0);	

			ramp_time  = time;
			ramp_level = 0.0f;
			AIL_sample_stage_property( sample, SP_FILTER_0, "Ramp Time", 0, &ramp_time , 0);
			AIL_sample_stage_property( sample, SP_FILTER_0, "Ramp To"  , 0, &ramp_level, 0);	
		}
		return true;
	}

	return false;
}

//*********************************************    PSoundBufferManager ***************************************************************

PSoundBufferManager::PSoundBufferManager(ISoundBufferInfoTable *pBufferTable, PStreamReader* pStream,  HDIGDRIVER  hDig, float *pfBGM, float *pfSFX, int nIndex)
:m_fLVolume( DEF_BUFFER_VOLUME)
,m_fRVolume( DEF_BUFFER_VOLUME)
,m_pData(NULL)
,m_pfBGM( pfBGM)
,m_pfSFX( pfSFX)
,m_fMaxDist(DEF_MAX_DIST)   // <== 여기서부터는 Sound Buffer 테이블에서 읽어오는값. 
,m_fMinDist(DEF_MIN_DIST)
,m_fBufferVolumeFactor(1.0f)
,m_fReverbDry(1.0f)
,m_fReverbWet(1.0f)
,m_bSfx( 1)
,m_nBufferIndex( nIndex)
,m_fCutOff(1.0f)
{
	//*** 06.08.07  Sample Static로 변경작업.
	/*for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		m_aSample[i] = NULL;
	}*/

	m_bAutoWet     = true;   //해주기 - 임시 나중에 이값을 외부에서 가져오든지 ..하게끔.. 
	m_hDig         = hDig;	
	m_nLength      = static_cast< int >( pStream->GetLength() );
	m_strFileName  = pStream->GetName();
	m_pBufferTable = pBufferTable;
	
	if ( m_pBufferTable)
	{		
		m_pBufferTable->GetSoundBufferData( m_strFileName.data(), &m_fMaxDist, &m_fMinDist, &m_fBufferVolumeFactor, &m_fReverbDry, &m_fReverbWet, &m_bSfx);	
	}
	//else
	//{		
	//	m_fMaxDist			      = DEF_MAX_DIST;
	//	m_fMinDist            = DEF_MIN_DIST;
	//	m_fBufferVolumeFactor = 1.0f;
	//	m_fReverbDry          = 1.0f;
	//	m_fReverbWet          = 1.0f;
	//	m_bSfx                = true;
	//}

	if( !m_pBufferTable )
	{
		m_fReverbDry = 1.0f;
		m_fReverbWet = 0.0f;
		m_fCutOff	 = 1.0f;
	}


	{
		THREAD_SYNCRONIZE(g_cs);
		g_pInputStream = pStream;		
		m_pData = AIL_file_read( (const char*)pStream, NULL);    //여기서 CALLBACK 함수 등록한것 호출함. 		
		g_pInputStream = NULL;
	}
	
	//*** 06.08.07  Sample Static로 변경작업.
	////*** 어차피 GetFreeBuffer에서 알아서함!!
	//m_aSample[0] = AIL_allocate_sample_handle(m_hDig);		
	//if (m_aSample[0])
	//{		
	//	if ( AIL_set_named_sample_file( m_aSample[0], m_strFileName.data() , m_pData, pStream->GetLength(), 0) == false)  //wav , mp3 .. 지원. 
	//	{
	//		char *pErr = AIL_last_error();
	//		OutputDebugString( pErr);
	//		assert( 0 &&  "PSoundBufferManager::PSoundBufferManager 함수 AIL_set_named_sample_file 부분 에러");
	//	}
	//	AIL_set_sample_3D_distances(m_aSample[0], m_fMaxDist, m_fMinDist, m_bAutoWet);		
	//	m_nDefPlayRate = AIL_sample_playback_rate( m_aSample[0]);
	//}	
}

PSoundBufferManager::~PSoundBufferManager(void)
{
	RemoveAll();
}



void  PSoundBufferManager::RemoveAll()  
{	
	//*** 06.08.07  Sample Static로 변경작업.
	//for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	//{
	//	if ( m_aSample[i] == NULL)
	//		continue;
	//	
	//	if ( AIL_sample_status( m_aSample[i]) == SMP_PLAYING)
	//	{
	//		AIL_stop_sample( m_aSample[i]);	
	//	}
	//	AIL_release_sample_handle( m_aSample[i]);   //  Sample  Release 시켜준다. 		
	//	m_aSample[i] = NULL;
	//}

	//sample들 clear시킨다.	
	for (int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		m_tSamples[i].Clear();
	}

	if (m_pData)
	{
		AIL_mem_free_lock(m_pData);
	}		
}

//*** Stop는 Unlock까지 동시에 수행. 
bool  PSoundBufferManager::Stop( int & nSampleIndex) 
{
	/*if ( m_aSample[ nSampleIndex] == NULL)
		return false;	

	AIL_stop_sample( m_aSample[ nSampleIndex]);		
	return true;*/

	if ( m_tSamples[ nSampleIndex].IsOwner( m_nBufferIndex))
	{
		AIL_stop_sample( m_tSamples[ nSampleIndex].m_hSample);

		m_tSamples[ nSampleIndex].m_owner = -1;
		m_tSamples[ nSampleIndex].SetLock( false);
		return true;
	}
	return false;
}

bool  PSoundBufferManager::Stop()
{
	/*for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		if ( m_aSample[i] == NULL)
			continue;
	
		AIL_stop_sample( m_aSample[i]);			
	}
	return true;*/

	for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		if ( m_tSamples[i].IsOwner( m_nBufferIndex))
		{
			AIL_stop_sample( m_tSamples[i].m_hSample);	

			m_tSamples[i].m_owner = -1;
			m_tSamples[i].SetLock( false);
		}
	}
	return true;
}

//*** Pause는 단지 Sound만 멈춤.
bool  PSoundBufferManager::Pause( int nSampleIndex)
{
	if ( m_tSamples[ nSampleIndex].IsOwner( m_nBufferIndex))
	{
		AIL_stop_sample( m_tSamples[ nSampleIndex].m_hSample);		
		return true;
	}
	return false;
}

bool  PSoundBufferManager::Resume_Sound( int nSampleIndex)
{
	if ( m_tSamples[ nSampleIndex].IsOwner( m_nBufferIndex))
	{		
		AIL_resume_sample( m_tSamples[ nSampleIndex].m_hSample);			
		return true;
	}
	return false;
}

bool  PSoundBufferManager::SetPlayBackRate( int nSampleIndex, int nRate)
{
	/*if ( m_aSample[ nSampleIndex] == NULL)
		return false;	

	AIL_set_sample_playback_rate( m_aSample[ nSampleIndex], nRate);		
	return true;*/
	
	if ( m_tSamples[ nSampleIndex].IsOwner( m_nBufferIndex))
	{				
		AIL_set_sample_playback_rate( m_tSamples[ nSampleIndex].m_hSample, nRate);		
		return true;
	}
	return false;
}

int   PSoundBufferManager::Play( PSoundVector &vecPos, int nLoop, float fLVolume, float fRVolume, int nPlayRate, bool bLock, bool bReverb, bool bLowPass )
{
	////getFreeBuffer()함수 호출해서 비어있는 Sample인덱스 얻어서 play 시킨다. 
	//int nSampleIndex = GetFreeBuffer();
	//
	//if (  m_aSample[nSampleIndex] == NULL)
	//{
	//	std::string strErr = m_strFileName + " Play실패!\n";
	//	OutputDebugString( strErr.c_str());
	//	return -1;
	//}

	//if ( nSampleIndex == -1)
	//{
	//	return -1;
	//}

	//SetVolume( fLVolume, fRVolume);
	//AIL_set_sample_3D_position( m_aSample[nSampleIndex], vecPos.m_fx, vecPos.m_fy, vecPos.m_fz);	
	//AIL_set_sample_loop_count( m_aSample[nSampleIndex], nLoop);
	//AIL_set_sample_reverb_levels(  m_aSample[ nSampleIndex], m_fReverbDry, m_fReverbWet);
	//if ( nPlayRate != 0)
	//	AIL_set_sample_playback_rate( m_aSample[ nSampleIndex], nPlayRate);
	//AIL_start_sample( m_aSample[ nSampleIndex]);	
	//return nSampleIndex;
	
	int nSampleIndex = GetFreeBuffer();

	if ( nSampleIndex == -1)
	{
#ifdef _DEBUG
		std::string strErr = m_strFileName + " Play실패!\n";
		OutputDebugString( strErr.c_str());
#endif
		return -1;
	}

	if( m_pBufferTable )
	{
		if( bReverb )	m_pBufferTable->GetGameDryWetVolume( &m_fReverbDry, &m_fReverbWet );
		else			
		{
			m_fReverbDry = 1.0f;
			m_fReverbWet = 0.0f;
		}

		if( bLowPass )	m_pBufferTable->GetGameLowPassCutOff( &m_fCutOff );
		else	m_fCutOff = 1.0f;
	}

	m_tSamples[nSampleIndex].SetLock( bLock);
	SetVolume( fLVolume, fRVolume);
	
	AIL_set_sample_3D_position( m_tSamples[nSampleIndex].m_hSample, vecPos.m_fx, vecPos.m_fy, vecPos.m_fz);	
	AIL_set_sample_3D_distances( m_tSamples[ nSampleIndex].m_hSample, m_fMaxDist, m_fMinDist, m_bAutoWet);
	AIL_set_sample_loop_count( m_tSamples[nSampleIndex].m_hSample, nLoop);
	
	AIL_set_sample_reverb_levels( m_tSamples[nSampleIndex].m_hSample, m_fReverbDry, m_fReverbWet);
	AIL_set_sample_low_pass_cut_off( m_tSamples[nSampleIndex].m_hSample, m_fCutOff );

	if ( nPlayRate != 0)
		AIL_set_sample_playback_rate( m_tSamples[nSampleIndex].m_hSample, nPlayRate);

	AIL_start_sample( m_tSamples[nSampleIndex].m_hSample);	
	return nSampleIndex;
}

int   PSoundBufferManager::Play2D( int nLoop, float fLVolume, float fRVolume, int nPlayRate, bool bLock)
{
	////getFreeBuffer()함수 호출해서 비어있는 Sample인덱스 얻어서 play 시킨다. 
	//int nSampleIndex = GetFreeBuffer();
	//
	//if (  m_aSample[nSampleIndex] == NULL)
	//{
	//	std::string strErr = m_strFileName + " Play2D실패!\n";
	//	OutputDebugString( strErr.c_str());
	//	return -1;
	//}

	//if ( nSampleIndex == -1)
	//{
	//	return -1;
	//}

	//
	//SetVolume( fLVolume, fRVolume);
	//AIL_set_sample_loop_count( m_aSample[nSampleIndex], nLoop);
	//AIL_set_sample_reverb_levels(  m_aSample[ nSampleIndex], m_fReverbDry, m_fReverbWet);

	//if ( nPlayRate != 0)
	//	AIL_set_sample_playback_rate( m_aSample[ nSampleIndex], nPlayRate);
	//AIL_start_sample( m_aSample[ nSampleIndex]);

	//
	//return nSampleIndex;

	int nSampleIndex = GetFreeBuffer();
	
	if ( nSampleIndex == -1)
	{
#ifdef _DEBUG
		std::string strErr = m_strFileName + " Play2D실패!\n";
		OutputDebugString( strErr.c_str());
#endif
		return -1;
	}

	m_tSamples[nSampleIndex].SetLock( bLock);
	SetVolume( fLVolume, fRVolume);
	AIL_set_sample_loop_count(  m_tSamples[nSampleIndex].m_hSample, nLoop);

	//2D사운드는 Reverb와 LowPass가 필요없을듯 3D사운드쪽만 적용시킨다
	AIL_set_sample_reverb_levels( m_tSamples[nSampleIndex].m_hSample, 1.0f, 0.0f );
	AIL_set_sample_low_pass_cut_off( m_tSamples[nSampleIndex].m_hSample, 1.0f );
//	AIL_set_sample_reverb_levels(   m_tSamples[nSampleIndex].m_hSample, m_fReverbDry, m_fReverbWet);
//	AIL_set_sample_low_pass_cut_off( m_tSamples[nSampleIndex].m_hSample, m_fCutOff );

	if ( nPlayRate != 0)
		AIL_set_sample_playback_rate(  m_tSamples[nSampleIndex].m_hSample, nPlayRate);

	AIL_start_sample(  m_tSamples[nSampleIndex].m_hSample);
	return nSampleIndex;
}


/******* GetFreeBuffer 함수 관련 ************
  일단 아래처럼 이렇게 일단하고 나중에 속도 더 빠르게 하고 싶다면 
3~4개정도만 멤버 가질수있는 백터 같은걸 만들어서 
Play시킬때  이 백터에 idx삽입하고  GetFreebuffer할때  
일단 이백터것부터 검사하고 없으면 아래 순서대로...   */
int   PSoundBufferManager::GetFreeBuffer()
{
	//int nFreeSampleIndex = -1;

	//for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	//{
	//	if (m_aSample[i]==NULL)           //Freeid 를 찾고  
	//	{
	//		if (nFreeSampleIndex == -1)
	//		{
	//			nFreeSampleIndex = i;
	//		}
	//		continue;
	//	}	

	//	//  m_vBuffer[i] 중에서 NULL이 아닌것들중세서  현재 play 되고있지 않은것을 검사 하여 리턴
	//	if ( AIL_sample_status( m_aSample[i])==SMP_PLAYING ) 
	//	{
	//		continue;
	//	}		

	//	//여기까지 왔다는 것은 이전에 Load한 Sample중에 playing중이 아닌 Sample을 찾았다는것. 이 Sample이용한다.		
	//	AIL_set_sample_playback_rate(  m_aSample[ i], m_nDefPlayRate);
	//	AIL_set_sample_3D_distances( m_aSample[ i], m_fMaxDist, m_fMinDist, m_bAutoWet);						
	//	return i;
	//}


	//if ( nFreeSampleIndex == -1)
	//{
	//	assert( 0 && " PSoundBufferManager::getFreeBuffer() 함수  Free Sample이 없습니다.!!!");
	//	return -1;
	//}

	//
	////새로운 Sample 생성 
	//m_aSample[ nFreeSampleIndex] = AIL_allocate_sample_handle(m_hDig);

	//if (m_aSample[ nFreeSampleIndex])
	//{
	//	AIL_set_named_sample_file( m_aSample[ nFreeSampleIndex], m_strFileName.c_str() , m_pData, m_nLength, 0);  //wav , mp3 .. 지원. 		
	//	AIL_set_sample_3D_distances( m_aSample[ nFreeSampleIndex], m_fMaxDist, m_fMinDist, m_bAutoWet);			
	//}
	//else
	//{		
	//	char  szErr[128];
	//	sprintf( szErr, "%s 파일 GetFreeBuffer 실패 SampleIndex = %d \n" , m_strFileName.c_str(), nFreeSampleIndex);
	//	OutputDebugString( szErr);
	//}

	//return nFreeSampleIndex;
	
	

	//***** 디버그용 코드 **********************
	/*int nFreeSampleCount = 0;
	for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		if ( m_tSamples[i].IsLock() || m_tSamples[i].IsPlaying())
			continue;

		++nFreeSampleCount;
	}
	_oprint( "사운드 Free Sample 카운트 %d \n", nFreeSampleCount); */
	//******************************************


	int nFreeSampleIndex = -1;

	for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		if ( m_tSamples[i].IsLock() || m_tSamples[i].IsPlaying())
			continue;
		
		if ( nFreeSampleIndex == -1)
			nFreeSampleIndex = i;

		if ( m_tSamples[ nFreeSampleIndex].IsOwner( m_nBufferIndex))
			break;
	}
	
	if ( nFreeSampleIndex == -1)
		return -1;

	if ( m_tSamples[ nFreeSampleIndex].IsOwner( m_nBufferIndex))
	{
		m_tSamples[ nFreeSampleIndex].ResetPlayRate();		
	}
	else
	{
		if ( m_tSamples[ nFreeSampleIndex].Init( m_nBufferIndex, m_hDig, m_strFileName.c_str(), m_pData, m_nLength) == false)
		{
			//char  szErr[128];
			//sprintf( szErr, "%s 파일 GetFreeBuffer 실패 SampleIndex = %d \n" , m_strFileName.c_str(), nFreeSampleIndex);
			//OutputDebugString( szErr);	
			_oprint( "%s 파일 GetFreeBuffer 실패 SampleIndex = %d \n" , m_strFileName.c_str(), nFreeSampleIndex);			
			return -1;
		}		
	}	

	AIL_set_sample_processor(  m_tSamples[nFreeSampleIndex].m_hSample, SP_FILTER_0, 0 );	
	AIL_set_sample_3D_distances( m_tSamples[ nFreeSampleIndex].m_hSample , m_fMaxDist, m_fMinDist, m_bAutoWet);
	return nFreeSampleIndex;
}

void  PSoundBufferManager::SetVolume(float fLVolume, float fRVolume) 
{	
	//float fSFX_BGM       = 1.0f;
	//float fLResultVolume = 1.0f;
	//float fRResultVolume = 1.0f;
	//
	//fSFX_BGM       = m_bSfx ? *m_pfSFX : *m_pfBGM;  //SFX, BGM
	//m_fLVolume     = fLVolume;
	//m_fRVolume     = fRVolume;

	//fLResultVolume = fLVolume * VOLUME_FACTOR * m_fBufferVolumeFactor * fSFX_BGM;
	//fRResultVolume = fRVolume * VOLUME_FACTOR * m_fBufferVolumeFactor * fSFX_BGM;

	//for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	//{
	//	if ( m_aSample[i] == NULL)
	//		continue;
	//	
	//	AIL_set_sample_volume_levels( m_aSample[i], fLResultVolume, fRResultVolume);
	//}	
	
	float fSFX_BGM       = m_bSfx ? *m_pfSFX : *m_pfBGM;  //SFX, BGM
	m_fLVolume     = fLVolume;
	m_fRVolume     = fRVolume;

	float fLResultVolume = fLVolume * VOLUME_FACTOR * m_fBufferVolumeFactor * fSFX_BGM;
	float fRResultVolume = fRVolume * VOLUME_FACTOR * m_fBufferVolumeFactor * fSFX_BGM;

	for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		if ( m_tSamples[i].IsOwner( m_nBufferIndex))
			AIL_set_sample_volume_levels( m_tSamples[i].m_hSample, fLResultVolume, fRResultVolume);
	}	
}

// BGM, SFX 등을 다시 set했을대  이 함수 호출해서 볼륨 다시계산한다.
void  PSoundBufferManager::ResetVolume() 
{
	SetVolume( m_fLVolume,  m_fRVolume); 
}

void  PSoundBufferManager::SetPosition_Sample(int nSampleIndex, PSoundVector pos)
{
	if ( m_tSamples[ nSampleIndex].IsOwner( m_nBufferIndex))
		AIL_set_sample_3D_position( m_tSamples[ nSampleIndex].m_hSample, pos.m_fx, pos.m_fy, pos.m_fz);	
}

//*** Buffer 단위로 Min, Max 설정한다. 이때 Sample단위로 Min,Max 설정한것들도 모두 버퍼단위 Min,Max로 다시 set하기 때문에 사용할때 유의해야됨!!
bool  PSoundBufferManager::SetMinMaxDist(float fMin, float fMax )
{
	//m_fMinDist = fMin;
	//m_fMaxDist = fMax;

	//for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	//{
	//	if ( m_aSample[i] == NULL)
	//		continue;
	//	
	//	AIL_set_sample_3D_distances(m_aSample[0], m_fMaxDist, m_fMinDist, m_bAutoWet);
	//}	

	//return true;

	m_fMinDist = fMin;
	m_fMaxDist = fMax;

	for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		if ( m_tSamples[i].IsOwner( m_nBufferIndex))
			AIL_set_sample_3D_distances( m_tSamples[i].m_hSample, m_fMaxDist, m_fMinDist, m_bAutoWet);
	}
	return true;
}

//*** Sample 단위로 Min,Max 설정한다. 이때 버퍼단위의 Min,Max는 무시되기때문에 사용할때 유의해야됨!!
bool  PSoundBufferManager::SetMinMaxDist(int nSampleIndex, float fMin, float fMax )
{
	//if ( m_aSample[ nSampleIndex] == NULL)
	//	return false;

	//AIL_set_sample_3D_distances(m_aSample[ nSampleIndex], fMax, fMin, m_bAutoWet);
	//return true;

	if ( m_tSamples[ nSampleIndex].IsOwner( m_nBufferIndex))
		AIL_set_sample_3D_distances( m_tSamples[ nSampleIndex].m_hSample, fMax, fMin, m_bAutoWet);
		
	return true;
}

//*** Buffer 단위로 bAutoSet설정한다. 이때 Sample단위로 Min,Max 설정한것들도 모두 버퍼단위 Min,Max로 다시 set하기 때문에 사용할때 유의해야됨!!
bool  PSoundBufferManager::SetAutoWet( bool  bAutoSet) 
{
	//m_bAutoWet = bAutoSet;

	//for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	//{
	//	if ( m_aSample[i] == NULL)
	//		continue;
	//	
	//	AIL_set_sample_3D_distances(m_aSample[0], m_fMaxDist, m_fMinDist, m_bAutoWet);
	//}	

	//return true;

	m_bAutoWet = bAutoSet;

	for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		if ( m_tSamples[i].IsOwner( m_nBufferIndex))
			AIL_set_sample_3D_distances( m_tSamples[i].m_hSample, m_fMaxDist, m_fMinDist, m_bAutoWet);
	}
	return true;
}


bool  PSoundBufferManager::SetFade( float time, bool bFadeIn)
{
	HPROVIDER filter = SOUND_FILTER().GetFilter("Volume Ramp Filter");

	if ( !filter)
		return false;

	for ( int i=0; i<MAX_SOUND_SAMPLE; ++i)
	{
		if ( m_tSamples[i].IsOwner( m_nBufferIndex)  && m_tSamples[i].IsPlaying())
		{				
			_oprint( "fade %d\n", i);
			AIL_set_sample_processor(  m_tSamples[i].m_hSample, SP_FILTER_0, filter );			
			
			if ( bFadeIn)
			{
				float ramp_time  = 0.0f;
				float ramp_level = 0.0f;
				AIL_sample_stage_property( m_tSamples[i].m_hSample, SP_FILTER_0, "Ramp Time", 0, &ramp_time , 0);
				AIL_sample_stage_property( m_tSamples[i].m_hSample, SP_FILTER_0, "Ramp To"  , 0, &ramp_level, 0);	

				ramp_time  = time;
				ramp_level = 1.0f;
				AIL_sample_stage_property( m_tSamples[i].m_hSample, SP_FILTER_0, "Ramp Time", 0, &ramp_time , 0);
				AIL_sample_stage_property( m_tSamples[i].m_hSample, SP_FILTER_0, "Ramp To"  , 0, &ramp_level, 0);	
			}
			else
			{	
				float ramp_time  = 0.0f;
				float ramp_level = 1.0f;
				AIL_sample_stage_property( m_tSamples[i].m_hSample, SP_FILTER_0, "Ramp Time", 0, &ramp_time , 0);
				AIL_sample_stage_property( m_tSamples[i].m_hSample, SP_FILTER_0, "Ramp To"  , 0, &ramp_level, 0);	

				ramp_time  = time;
				ramp_level = 0.0f;
				AIL_sample_stage_property( m_tSamples[i].m_hSample, SP_FILTER_0, "Ramp Time", 0, &ramp_time , 0);
				AIL_sample_stage_property( m_tSamples[i].m_hSample, SP_FILTER_0, "Ramp To"  , 0, &ramp_level, 0);	
			}
		}			
	}	
	return true;
}