#pragma once 

#include "../toolkit/ILock.h"
#include <audiere/audiere.h>
#include "LowFilter.h"
//#include "basic_source.h"
//#include "types.h"

///ps추가
namespace xaudiere  
{// 네임스페이스 ********************************************************************************************************************

	struct Tag {
		Tag(const std::string& k, const std::string& v, const std::string& t) {
			key = k;
			value = v;
			type = t;
		}

		std::string key;
		std::string value;
		std::string type;
	};

	class BasicSource : public audiere::RefImplementation <audiere::SampleSource> 
	{
	public:
		BasicSource(){ m_repeat = false; m_frequency = -1;}
		virtual ~BasicSource(){;}
		virtual int  ADR_CALL read(int frame_count, void* buffer){return 0;}
		virtual bool ADR_CALL isSeekable()                  { return false; }
		virtual int  ADR_CALL getLength()                   { return 0;     }
		virtual void ADR_CALL setPosition(int /*position*/) {               }
		virtual int  ADR_CALL getPosition()                 { return 0;     }


		bool ADR_CALL getRepeat()                   { return m_repeat; }
		void ADR_CALL setRepeat(bool repeat)        { m_repeat = repeat; }

		int ADR_CALL getTagCount()              { return int(m_tags.size()); }
		const char* ADR_CALL getTagKey(int i)   { return m_tags[i].key.c_str(); }
		const char* ADR_CALL getTagValue(int i) { return m_tags[i].value.c_str(); }
		const char* ADR_CALL getTagType(int i)  { return m_tags[i].type.c_str(); }

		virtual int doRead(int frame_count, void* buffer){return 0;}


		/// ps
		virtual void  SetLowPassFrequency(int frequency) 
		{
			//크리티컬 섹션  - main 쓰레드와  오디에르내부 엔진 쓰레드가 m_frequency 같이 사용하기때문에 
			m_CriticalSection.Lock();
			m_frequency = frequency;
			m_CriticalSection.UnLock();
		}
		virtual int   GetLowPassFrequency()              { return m_frequency;}

	protected:
		void addTag(const Tag& t) {
			m_tags.push_back(t);
		}

		void addTag(const std::string& k, const std::string& v, const std::string& t) {
			addTag(Tag(k, v, t));
		}

	protected:
		bool                  m_repeat;
		int                   m_frequency;
		XCriticalSection      m_CriticalSection;

		std::vector<Tag>      m_tags;
	};



	class WAVInputStream : public  xaudiere::BasicSource
	{

	public:
		WAVInputStream();
		virtual ~WAVInputStream(){;}

		bool    initialize(audiere::FilePtr file);	
		int     doRead(int frame_count, void* buffer);
		void    ADR_CALL getFormat( int& channel_count, int& sample_rate, audiere::SampleFormat& sample_format);
		void    ADR_CALL reset();
		bool    ADR_CALL isSeekable();
		int     ADR_CALL getLength();
		void    ADR_CALL setPosition(int position);
		int     ADR_CALL getPosition();
		int     ADR_CALL read(int frame_count, void* buffer);
	private:
		bool findFormatChunk();
		bool findDataChunk();
		bool skipBytes(int size); 
	private:
		audiere::FilePtr        m_file;

		// from format chunk
		int                     m_channel_count;
		int                     m_sample_rate;
		audiere::SampleFormat   m_sample_format;

		// from data chunk
		int  m_data_chunk_location; ///< bytes
		int  m_data_chunk_length;   ///< in frames

		int  m_frames_left_in_chunk;

		CLowFilter             m_LowFilter;
	};  


	/// 해주기   ps: 이것을 WAVInputStream의 멤버함수로 넣어버리기 .. 또는 xaudirer.h로 옮긴다
	inline int GetFrameSize(audiere::SampleSource* source)
	{
		int channel_count, sample_rate;
		audiere::SampleFormat    sample_format;
		source->getFormat(channel_count, sample_rate, sample_format);

		switch (sample_format) 
		{
		case audiere::SF_U8:    return 1*channel_count;
		case audiere::SF_S16:   return 2*channel_count;
		default:                return 0;
		}	
	}


} // 네임스페이스 **********************************************************************************************************


