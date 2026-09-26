#pragma once


/** 사운드 파일에 대한 각종 정보를 가지고 있는 클래스
	인터페이스 클래스이며  사용자가 이 클래스 상속받아서  사운드 엔진에 넘겨줘야된다. */
class ISoundBufferInfoTable
{
public:
	ISoundBufferInfoTable(){;}
	virtual ~ISoundBufferInfoTable(){;}	

	/// 해당 파일명에 대한 사운드파일 정보를 out 해준다!!   // fVolume, fReverbDry, fReverbWet 등의 범위는 1.0f~ 0.0f
	virtual bool GetSoundBufferData( const char *szFileName, float *pfMaxDist, float *pfMinDist, 
		                             float *pfVolume,  float *pfReverbDry, float *pfReverbWet, bool *pbSfx) = 0;

	virtual bool GetGameDryWetVolume( float *pfReverbDry, float *pfReverbWet )
	{ 
		*pfReverbDry = 1.0f;
		*pfReverbWet = 0.0f;
		
		assert(false); 
		return false; 
	}
	
	virtual bool GetGameLowPassCutOff( float *pfCutOff )
	{ 
		*pfCutOff = 1.0f;
		
		assert(false); 
		return false; 
	} 
};


//file name  bBGM Max Dist	Min Dist	 최대 volume	 reverb wet값
