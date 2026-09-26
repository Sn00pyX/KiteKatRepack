#pragma once


#include "input_xwav.h"   //ps : wave  input 스트림 새로만든것. 





//ps추가
namespace xaudiere 
{// 네임스페이스 ********************************************************************************************************************

typedef unsigned __int8  u8;
typedef signed   __int8  s8;
typedef unsigned __int16 u16;
typedef signed   __int16 s16;
typedef unsigned __int32 u32;
typedef signed   __int32 s32;
typedef unsigned __int64 u64;
typedef signed   __int64 s64;





struct TInOutStream
{
	xaudiere::BasicSource*     m_pInput;
	audiere::OutputStreamPtr   m_OutPtr;
	
				

public:		
	TInOutStream()
	{
		m_pInput = NULL;
	}
	~TInOutStream()
	{
		if( (m_OutPtr.get() != NULL) && (m_OutPtr.get()->isPlaying()))
		{
			m_OutPtr->stop();   // ps: 오디에르 엔진 내부적으로 m_pOutPut 이  m_pInput 을 가지고 있기때문에 
		                        //     m_pOutPut 만 없애주면 오디에르에서 m_pInput 알아서 없앤다. 			
		}
	}

	
	void operator=(TInOutStream *pInOut)
	{
		m_pInput = pInOut->m_pInput;
		m_OutPtr = pInOut->m_OutPtr;		
	}

	virtual void  ADR_CALL play()                  	 { m_OutPtr->play();}
	virtual void  ADR_CALL stop()                  	 { m_OutPtr->stop();}
	virtual bool  ADR_CALL isPlaying()             	 { return m_OutPtr->isPlaying();}
	virtual void  ADR_CALL reset()                 	 { m_OutPtr->reset();}
	virtual void  ADR_CALL setRepeat(bool repeat)  	 { m_OutPtr->setRepeat(repeat);}
	virtual bool  ADR_CALL getRepeat()             	 { return m_OutPtr->getRepeat();}
	virtual void  ADR_CALL setVolume(float volume) 	 { m_OutPtr->setVolume(volume);}
	virtual float ADR_CALL getVolume()            	 { return m_OutPtr->getVolume();}
	virtual void  ADR_CALL setPan(float pan)       	 { m_OutPtr->setPan(pan);}
	virtual float ADR_CALL getPan()               	 { return m_OutPtr->getPan();}
	virtual void  ADR_CALL setPitchShift(float shift){ m_OutPtr->setPitchShift(shift);}
	virtual float ADR_CALL getPitchShift()           { return m_OutPtr->getPitchShift();}
	virtual bool  ADR_CALL isSeekable()              { return m_OutPtr->isSeekable();}
	virtual int   ADR_CALL getLength()               { return m_OutPtr->getLength();}
	virtual void  ADR_CALL setPosition(int position) { m_OutPtr->setPosition(position);}
	virtual int   ADR_CALL getPosition()             { return m_OutPtr->getPosition();}

	
	virtual void  SetLowPassFrequency(int frequency) { m_pInput->SetLowPassFrequency(frequency);}
	virtual int   GetLowPassFrequency()              { return m_pInput->GetLowPassFrequency();  }
};




template<typename T>
static T* TryInputStream(const audiere::FilePtr& file) 
{
	// initialize should never close the file
	T* source = new T();
	if (source->initialize(file)) 
	{
	  return source;
	} 
	else 
	{
	  delete source;
	  return 0;
	}
}

inline u16 read16_le(const u8* b) 
{
	return b[0] + (b[1] << 8);
}

inline u16 read16_be(const u8* b) 
{
	return (b[0] << 8) + b[1];
}

inline u32 read32_le(const u8* b)
{
	return read16_le(b) + (read16_le(b + 2) << 16);
}

inline u32 read32_be(const u8* b) 
{
	return (read16_be(b) << 16) + read16_be(b + 2);
}


//해주기 1. 
inline xaudiere::BasicSource*   OpenSampleSource( const audiere::FilePtr& file,  audiere::FileFormat file_format)
{
	//해주기 1. 여기서 wav파일인지 mp3파일인지  구분하는 코드집어넣기 	
	

	return TryInputStream< WAVInputStream>(file);
}

	
inline xaudiere::TInOutStream*  OpenSound( const audiere::AudioDevicePtr& device, const audiere::FilePtr& file, 
	                                     bool streaming = false, audiere::FileFormat file_format = audiere::FF_AUTODETECT)
{
	TInOutStream  *ptInOutStream = new TInOutStream;	


	if( streaming == false)       //Buffer이면  오디에르 라이브러리 만들어서 open한다. 
	{
		ptInOutStream->m_pInput = NULL;
		ptInOutStream->m_OutPtr = audiere::OpenSound( device, file, false, file_format);
	}
	else if( streaming == true)   // 스트림으로 만드는것이라면(로우필터사용하려면 스트림으로 만들어야됨) .. (스트림 wav파일만지원)
	{
		ptInOutStream->m_pInput   = xaudiere::OpenSampleSource(file, file_format);		
		ptInOutStream->m_OutPtr   = audiere::OpenSound(device, ptInOutStream->m_pInput, streaming);    
	}                                                 


	if( ptInOutStream->m_OutPtr == NULL )
	{
		delete ptInOutStream;
		return NULL;
	}

	return ptInOutStream;  
}
	







}// 네임스페이스 ******************************************************************************************************************** 














