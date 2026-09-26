
#include <math.h>

// #include <d3dtypes.h>
#include <d3d9.h>
#include <audiere/audiere.h>
#include "../../include/sound/xaudiere.h"



//#include <string>

#include "../../include/kfile/KStream.h"
#include "../../include/kfile/KFileManager.h"
#include "../../include/sound/XStreamReader.h"
#include "../../include/sound/XSoundManager.h"


#include "../../include/sound/GlobalCutOffFreqTable.h"  //해주기 - 나중에 로우필터 각 소리마다 해주게 되면 빼준다. 


const size_t MAX_SOUND = 15;



class C3DSoundInfo
{
public:
	D3DVECTOR m_position;

	C3DSoundInfo()
	{
		m_position.x = 0.0f;
		m_position.y = 0.0f;
		m_position.z = 0.0f;
	}

	C3DSoundInfo(const C3DSoundInfo& obj)
	{
		m_position.x = obj.m_position.x;
		m_position.y = obj.m_position.y;
		m_position.z = obj.m_position.z;
	}

	~C3DSoundInfo() {}

	void Clear()
	{
		m_position.x = 0.0f;
		m_position.y = 0.0f;
		m_position.z = 0.0f;
	}
};

class XDSoundBufferManager
{
public:
	XDSoundBufferManager( audiere::AudioDevicePtr pDevice, const char* szFileName, AudiereStreamAdapter* pStream, int nMasterVolume, const D3DVECTOR* listenerPosition = NULL, const D3DVECTOR* cameraPosition = NULL , bool bStream = false)
	{
		m_fMin = 0.0f;
		m_fMax = 100.0f;

		m_nVolume = 100;
		m_nMasterVolume = nMasterVolume;

		m_pDevice = pDevice;

		if(listenerPosition)
		{
			m_listenerPosition.x = listenerPosition->x;
			m_listenerPosition.y = listenerPosition->y;
			m_listenerPosition.z = listenerPosition->z;
		}

		if(cameraPosition)
		{
			m_cameraPosition.x = cameraPosition->x;
			m_cameraPosition.y = cameraPosition->y;
			m_cameraPosition.z = cameraPosition->z;
		}

		for(int i=0; i<MAX_SOUND; ++i)
		{
			m_vBuffer[i] = NULL;
			m_v3DSoundInfo[i].Clear();
		}

		m_szFileName.assign(szFileName);

		m_vBuffer[0] = xaudiere::OpenSound(m_pDevice, pStream, bStream);    //ps

		m_bUse3D = true;
	}

	~XDSoundBufferManager()
	{
		for(int i=0; i<MAX_SOUND; ++i)             
		{
			if(m_vBuffer[i])
			{
				/*
				if(m_vBuffer[i]->isPlaying())
				{
					m_vBuffer[i]->stop();	
				}
				*/
				
				delete m_vBuffer[i];           

				m_vBuffer[i] = NULL;
				m_v3DSoundInfo[i].Clear();
			}
		}
	}

	bool IsUse3DSound()
	{
		return m_bUse3D;
	}

	void SetUse3DSound( bool bUse3D )
	{
		m_bUse3D = bUse3D;
	}

	int SetVolume( int nVolume, bool bStream )
	{
		int nRtn = -1;
		getFreeBuffer( nRtn, bStream );
		m_nVolume = nVolume;
		return nRtn;
	}

	int Play( int idx, bool bLoop = false )
	{
		if( idx > MAX_SOUND || idx < 0 || m_vBuffer[idx]==NULL )
		{
			return -1;
		}

		m_vBuffer[idx]->setRepeat(bLoop);

		//	volume, panning 지정
		m_vBuffer[idx]->setPan(0);
		m_vBuffer[idx]->setVolume((float)m_nVolume * 0.01f * (float)m_nMasterVolume * 0.01f);

		if( m_vBuffer[idx] )
		{
			m_vBuffer[idx]->play();
		}

		return idx;
	}

	int Play( int idx, float x, float y, float z = 0.0f, bool bLoop = false )
	{
		if( idx > MAX_SOUND || idx < 0 || m_vBuffer[idx]==NULL ) return -1;

		m_vBuffer[idx]->setRepeat(bLoop);

		m_v3DSoundInfo[idx].m_position.x = x;
		m_v3DSoundInfo[idx].m_position.y = y;
		m_v3DSoundInfo[idx].m_position.z = z;

		Simulate3D(idx);

		if(m_vBuffer[idx]!=NULL)
		{
			m_vBuffer[idx]->play();
		}

		return idx;
	}

	bool Stop( int idx )
	{
		if( idx > MAX_SOUND || idx < 0 || m_vBuffer[idx]==NULL ) return false;

		m_vBuffer[idx]->stop();
		return true;
	}

	bool SetMinMax( float fMin, float fMax )
	{
		m_fMin = fMin;
		m_fMax = fMax;

		return true;
	}


	xaudiere::TInOutStream* getFreeBuffer( int & idx, bool bStream )  
	{
		int nCnt = 0;
		int nFreeIdx = -1;

		for( int i = 0; i < MAX_SOUND; ++i )
		{
			if(m_vBuffer[i]==NULL)
			{
				if(nFreeIdx==-1)
				{
					nFreeIdx = i;
				}
				continue;
			}

			nCnt++;

			if( m_vBuffer[i]->isPlaying() ) continue;

			idx = i;
			return m_vBuffer[i];
		}

		if( nCnt >= MAX_SOUND ) return NULL;
		if( nFreeIdx==-1 ) return NULL;
		
		
		
		if( bStream)
			m_vBuffer[nFreeIdx] = getFreeBuffer_Stream();
		else
			m_vBuffer[nFreeIdx] = getFreeBuffer_Buffer();


		m_v3DSoundInfo[nFreeIdx].Clear();

		idx = nFreeIdx;

		return m_vBuffer[nFreeIdx];
	}


	xaudiere::TInOutStream* getFreeBuffer_Buffer()  //ps InputBuffer
	{		
		AudiereStreamAdapter stream(KFileManager::Instance().CreateStreamFromResource( m_szFileName.c_str() ));
		stream.ref();
		return xaudiere::OpenSound(m_pDevice, &stream, false);	
	}

	xaudiere::TInOutStream* getFreeBuffer_Stream()   //ps InputStream
	{
		AudiereStreamAdapter *pStream = new AudiereStreamAdapter(KFileManager::Instance().CreateStreamFromResource( m_szFileName.c_str() ));				
		return xaudiere::OpenSound(m_pDevice, pStream, true);
	}

	void OnChangeListenerPosition(float x, float y, float z)
	{
		m_listenerPosition.x = x;
		m_listenerPosition.y = y;
		m_listenerPosition.z = z;

		//	play 중인 sound volume / panning 다시 계산
		for(int i=0; i<MAX_SOUND; ++i)
		{
			if(m_vBuffer[i])
			{
				if(m_vBuffer[i]->isPlaying())
				{
					Simulate3D(i);
				}
			}
		}
	}

	void OnChangeCameraPosition(float x, float y, float z)
	{
		m_cameraPosition.x = x;
		m_cameraPosition.y = y;
		m_cameraPosition.z = z;

		//	play 중인 sound volume / panning 다시 계산
		for(int i=0; i<MAX_SOUND; ++i)
		{
			if(m_vBuffer[i])
			{
				if(m_vBuffer[i]->isPlaying())
				{
					Simulate3D(i);
				}
			}
		}
	}

	void OnChangeMasterVolume(int nVolume, bool bUse3D)
	{
		m_nMasterVolume = nVolume;

		//	play 중인 sound volume / panning 다시 계산
		if(bUse3D)
		{
			for( int i = 0; i < MAX_SOUND; ++i )
			{
				if(m_vBuffer[i]!=NULL)
				{
					if(m_vBuffer[i]->isPlaying())
					{
						Simulate3D(i);
					}
				}
			}
		}
		else
		{
			for( int i = 0; i < MAX_SOUND; ++i )
			{
				if(m_vBuffer[i]!=NULL)
				{
					if(m_vBuffer[i]->isPlaying())
					{
						m_vBuffer[i]->setPan(0);
						m_vBuffer[i]->setVolume((float)m_nVolume * 0.01f * (float)m_nMasterVolume * 0.01f );
					}
				}
			}
		}
	}

	float GetSquareOfDistance(float x1, float y1, float z1, float x2, float y2, float z2) const
	{
		float x, y, z;

		x = x1 - x2;
		y = y1 - y2;
		z = z1 - z2;

		return x * x + y * y + z * z;
	}

	float GetPanning(float posX, float posY, float posZ) const
	{
		//	Z 성분은 무시
		float cameraViewX, cameraViewY;
		cameraViewX = m_listenerPosition.x - m_cameraPosition.x;
		cameraViewY = m_listenerPosition.y - m_cameraPosition.y;

		//	새로운 카메라 뷰 계산(90도 회전)
		float newCameraViewX, newCameraViewY;

		newCameraViewX = cameraViewX * cos(90.0f) - cameraViewY * sin(90.0f);
		newCameraViewY = cameraViewX * sin(90.0f) + cameraViewY * cos(90.0f);

		//	Normalize
		float size1 = sqrt(pow(newCameraViewX, 2) + pow(newCameraViewY, 2));

		if(size1==0.0f)
		{
			return 0.0f;
		}
		else
		{
			newCameraViewX = newCameraViewX / size1; 
			newCameraViewY = newCameraViewY / size1;
		}

		//	사운드위치 - 카메라 위치
		float newPosX, newPosY;

		newPosX = posX - m_cameraPosition.x;
		newPosY = posY - m_cameraPosition.y;

		//	Normalize
		float size2 = sqrt(pow(newPosX, 2) + pow(newPosY, 2));

		if(size2==0.0f)
		{
			return 0.0f;
		}
		else
		{
			newPosX = newPosX / size2; 
			newPosY = newPosY / size2; 
		}

		//	return -cos(theta)
		return (newCameraViewX * newPosX + newCameraViewY * newPosY) * -1;
	}

	void Simulate3D(int idx)
	{
		D3DVECTOR soundPosition = m_v3DSoundInfo[idx].m_position;
		
		float squareOfDistance = GetSquareOfDistance(m_listenerPosition.x, m_listenerPosition.y, m_listenerPosition.z, 
												     soundPosition.x, soundPosition.y, soundPosition.z);


		float distance = sqrt(squareOfDistance);
		float distanceFactor = 0;

		if(distance > m_fMax)
		{
			distanceFactor = 0.0f;
		}
		else if(distance < m_fMin)
		{
			distanceFactor = 1.0f;
		}
		else
		{
			distanceFactor = pow(((m_fMax + m_fMin - distance) / (m_fMax - m_fMin)), 2);
		}

		float objVolume = (m_nVolume * 0.01f) * (m_nMasterVolume * 0.01f) * distanceFactor;

		if(objVolume<0.0f)
		{
			objVolume = 0.0f;
		}
		else if(objVolume>1.0f)
		{
			objVolume = 1.0f;
		}
		
		m_vBuffer[idx]->setVolume(objVolume);

		float fpan = GetPanning(soundPosition.x, soundPosition.y, soundPosition.z); 
		m_vBuffer[idx]->setPan(fpan);


		//해주기 - 거리에 따른 자동 로우필터를 각 소리마다 set하게 되면 이부분 알맞게 변경해준다.
		SetDistForLowFilter(idx, distance);
	}

	//PS: 로우필터 set하는 함수 
	bool  SetLowFilter( int idx, int  frequency)
	{		
		if( (m_vBuffer[idx] == NULL) || (m_vBuffer[idx]->m_pInput == NULL))
			return false;

		m_vBuffer[idx]->m_pInput->SetLowPassFrequency( frequency);

		return true;
	}

private:
	//해주기 - 거리에 따른 로우필터 set하는 함수 만들기 private 로 
	//해주기 - 거리에 따른 자동 로우필터를 각 소리마다 set하게 되면 이부분 알맞게 변경해준다.
	void SetDistForLowFilter( int idx,  float fDist )
	{
		GCutOffFreqTable *pCutOff = GCutOffFreqTable::GetCutOffFreqTable();   //싱글톤 접근자.

		int freq = pCutOff->GetCutOffFreq( fDist);
		SetLowFilter( idx, freq );		
	}


private:
	D3DVECTOR					m_listenerPosition;
	D3DVECTOR					m_cameraPosition;

	float						m_fMin;
	float						m_fMax;
	int							m_nVolume;
	int							m_nMasterVolume;

	bool						m_bUse3D;

	std::string					m_szFileName;

	audiere::AudioDevicePtr		m_pDevice;
	xaudiere::TInOutStream*	    m_vBuffer[MAX_SOUND];                   //ps
	C3DSoundInfo				m_v3DSoundInfo[MAX_SOUND];	            //사운드 버퍼들의 위치정보 가지고 있다. 
};

XSoundManager::XSoundManager()
{
	m_bUse3D = true;
	m_nVolume = 100;
	memset( m_pBuffer, 0, sizeof(m_pBuffer) );
}

XSoundManager::~XSoundManager()
{
	DeInit();
}

bool XSoundManager::Init( void* hWnd, bool bGlobal, bool bUse3D, bool bIsStereo, unsigned freq, unsigned bit )
{
	m_pDevice = audiere::OpenDevice();

	if( !m_pDevice )
	{
		return false;
	}

	return true;
}

bool XSoundManager::DeInit()
{
	if( !m_pDevice )
	{
		return false;
	}

	for( size_t i = 0; i < XMAX_SOUND_INDEX; ++i ) 
	{
		if( m_pBuffer[i] )
		{		
			delete m_pBuffer[i];		
		}
	}

	if( m_pDevice )
	{
		m_pDevice = NULL;
	}
	
	memset( m_pBuffer, 0, sizeof(m_pBuffer) );

	m_bUse3D = true;
	m_nVolume = 100;

	return true;
}

bool XSoundManager::Load( unsigned short idx, const char* szFileName, AudiereStreamAdapter* pStream, bool bUse3D ,bool bStream )
{
	if( m_pBuffer[idx] ) return false;

	if(bUse3D)
	{
		m_pBuffer[idx] = new XDSoundBufferManager(m_pDevice, szFileName, pStream, m_nVolume, &m_listenerPosition, &m_cameraPosition, bStream);
	}
	else
	{
		m_pBuffer[idx] = new XDSoundBufferManager(m_pDevice, szFileName, pStream, m_nVolume);
	}

	return ( m_pBuffer[idx] ? true : false );
}

bool XSoundManager::Load( unsigned short idx, const char *szFileName, bool bUse3D  , bool bStream)
{
	AudiereStreamAdapter stream(KFileManager::Instance().CreateStreamFromResource( szFileName ));
	return Load( idx, szFileName, &stream, bUse3D , bStream);
}

bool XSoundManager::UnLoad( unsigned short idx )
{
	if( !m_pBuffer[idx] ) return false;

	delete m_pBuffer[idx];
	m_pBuffer[idx] = NULL;
	return true;
}

bool XSoundManager::SetMinMax( unsigned short idx, float fMin, float fMax )
{
	if( !m_pBuffer[idx] ) return false;

	return m_pBuffer[idx]->SetMinMax( fMin, fMax );
}

bool XSoundManager::SetListenerPosition( float x, float y, float z )
{
	m_listenerPosition.x = x;
	m_listenerPosition.y = y;
	m_listenerPosition.z = z;

	try
	{
		for(size_t i=0; i<XMAX_SOUND_INDEX; ++i)
		{
			if( m_pBuffer[i] && m_pBuffer[i]->IsUse3DSound() )
			{
				m_pBuffer[i]->OnChangeListenerPosition(x, y, z);
			}
		}
	}
	catch(std::exception&)
	{
		return false;
	}

	return true;
}

bool XSoundManager::SetCameraPosition( float x, float y, float z )
{
	m_cameraPosition.x = x;
	m_cameraPosition.y = y;
	m_cameraPosition.z = z;

	try
	{
		for(size_t i=0; i<XMAX_SOUND_INDEX; ++i)
		{
			if( m_pBuffer[i] && m_pBuffer[i]->IsUse3DSound() )
			{
				m_pBuffer[i]->OnChangeCameraPosition(x, y, z);
			}
		}
	}
	catch(std::exception&)
	{
		return false;
	}

	return true;
}

bool XSoundManager::SetRollOffFactor( float x )
{
	//	RollOff 팩터 무시
	return false;
}

bool XSoundManager::GetRollOffFactor( float * x )
{
	//	RollOff 팩터 무시
	return false;
}

int XSoundManager::Play( unsigned short idx, bool bUse3D, bool bLoop, int nVolume, bool bStream)
{
	if( !m_pBuffer[idx] ) return false;

	int id = m_pBuffer[idx]->SetVolume( nVolume, bStream );

	m_pBuffer[idx]->SetUse3DSound(bUse3D);

	if( bUse3D )
	{
		return m_pBuffer[idx]->Play( id, m_listenerPosition.x, m_listenerPosition.y, m_listenerPosition.z, bLoop);
	}
	else
	{
		return m_pBuffer[idx]->Play( id, bLoop );
	}
}

int XSoundManager::Play( unsigned short idx, float x, float y, float z, bool bLoop, int nVolume, bool bStream)
{
	if( !m_pBuffer[idx] ) return false;

	int id = m_pBuffer[idx]->SetVolume( nVolume, bStream);

	if( id < 0 )
	{
		return -1;
	}
	else
	{
		return m_pBuffer[idx]->Play( id, x, y, z, bLoop);
	}
}

bool XSoundManager::Stop( unsigned short idx, int play_id )
{
	if( !m_pBuffer[idx] ) return false;

	return m_pBuffer[idx]->Stop( play_id );
}

bool XSoundManager::SetVolume(int nVolume)
{
	m_nVolume = nVolume;

	try
	{
		for(size_t i=0; i<XMAX_SOUND_INDEX; i++)
		{
			if( m_pBuffer[i] )
			{
				m_pBuffer[i]->OnChangeMasterVolume(m_nVolume, m_bUse3D);
			}
		}
	}
	catch(std::exception&)
	{
		return false;
	}

	return true;
}

int XSoundManager::GetVolume() const
{
	return m_nVolume;
}

/*
bool XSoundManager::SetWaveVolume( int nVolume )
{
	m_nWaveVolume = nVolume;

	try
	{
		for(size_t i=0; i<XMAX_SOUND_INDEX; i++)
		{
			if( m_pBuffer[i] )
			{
				m_pBuffer[i]->OnChangeWaveVolume(m_nWaveVolume, m_bUse3D);
			}
		}
	}
	catch(std::exception&)
	{
		return false;
	}

	return true;
}

int  XSoundManager::GetWaveVolume() const
{
	return m_nWaveVolume;
}
*/

//해주기- 거리에 따른 자동 로우필터 함수 들어가면 이함수 빼주기. 
bool  XSoundManager::SetLowFilter( int idx, int play_id, int  frequency)
{
	
	if( NULL == m_pBuffer[idx])
		return false;
	return m_pBuffer[idx]->SetLowFilter( play_id, frequency);
}