#include <string.h>
//#include "debug.h"

#include "../../include/sound/input_xwav.h"
#include "../../include/sound/xaudiere.h"
//#include "utility.h"

//ps추가
namespace xaudiere 
{// 네임스페이스 ********************************************************************************************************************



static inline bool IsValidSampleSize(u32 size)
{
	return (size == 8 || size == 16);
}


WAVInputStream::WAVInputStream()
{
	m_file = 0;

	m_channel_count = 0;
	m_sample_rate   = 0;
	m_sample_format = audiere::SF_U8;  // reasonable default?

	m_data_chunk_location = 0;
	m_data_chunk_length   = 0;

	m_frames_left_in_chunk = 0;
}



bool WAVInputStream::initialize(audiere::FilePtr file) 
{
	m_file = file;

	// read the RIFF header
	char riff_id[4];
	u8   riff_length_buffer[4];
	char riff_datatype[4];

	u32 size = 0;
	size += file->read(riff_id, 4);
	size += file->read(riff_length_buffer, 4);
	size += file->read(riff_datatype, 4);

	int riff_length = read32_le(riff_length_buffer);

	if (size != 12 || memcmp(riff_id, "RIFF", 4) != 0 || riff_length == 0 || memcmp(riff_datatype, "WAVE", 4) != 0) 
	{	
		// so we don't destroy the file
		m_file = 0;
		return false;
	}


	
	if (findFormatChunk() && findDataChunk()) 
	{		
		m_LowFilter.SetChanelIndex( m_channel_count);
		m_LowFilter.SetSamplingRate( m_sample_rate); 
		return true;
	}
	else
	{
		m_file = 0;
		return false;
	}
}


void WAVInputStream::getFormat( int& channel_count,	int& sample_rate, audiere::SampleFormat& sample_format)
{
	channel_count = m_channel_count;
	sample_rate   = m_sample_rate;
	sample_format = m_sample_format;
}

template< typename T >
static inline T _min2( const T& a, const T& b ) 
{
	return a < b ? a : b;
}

int WAVInputStream::doRead(int frame_count, void* buffer) 
{
	if (m_frames_left_in_chunk == 0) 
	{
		return 0;
	}

	const int frames_to_read = _min2(frame_count, m_frames_left_in_chunk);
	const int frame_size = m_channel_count * GetSampleSize(m_sample_format);
	const int bytes_to_read = frames_to_read * frame_size;
	const int read = m_file->read(buffer, bytes_to_read);

	//크리티컬 섹션  - main 쓰레드와  오디에르내부 엔진 쓰레드가 m_frequency 같이 사용하기때문에 
	m_CriticalSection.Lock();
	if( m_frequency != -1)  //로우필터 걸어주기 
	{
		m_LowFilter.SetCut( m_frequency);

		if( m_sample_format == audiere::SF_U8)
			m_LowFilter.DoLowPassFilter( (unsigned char*)buffer, (unsigned char*)buffer, read);
		else if( m_sample_format == audiere::SF_S16)
			m_LowFilter.DoLowPassFilter( (short*)buffer, (short*)buffer, read);
	}
	m_CriticalSection.UnLock();

	const int frames_read = read / frame_size;

#if WORDS_BIGENDIAN
	if (m_sample_format == SF_S16) {
	  // make little endian into host endian
	  u8* out = (u8*)buffer;
	  for (int i = 0; i < frames_read * m_channel_count; ++i) {
		std::swap(out[0], out[1]);
		out += 2;
	  }
	}
#endif

	// assume that if we didn't get a full read, we're done
	if (read != bytes_to_read)
	{
		m_frames_left_in_chunk = 0;
		return frames_read;
	}


	m_frames_left_in_chunk -= frames_read;
	return frames_read;
}


void  WAVInputStream::reset()
{
	// seek to the beginning of the data chunk
	m_frames_left_in_chunk = m_data_chunk_length;
	m_file->seek(m_data_chunk_location, audiere::File::BEGIN);
}



bool WAVInputStream::isSeekable() 
{
	return true;
}


int WAVInputStream::getLength() 
{
	return m_data_chunk_length;
}


void WAVInputStream::setPosition(int position) 
{
	int frame_size = m_channel_count * GetSampleSize(m_sample_format);
	m_frames_left_in_chunk = m_data_chunk_length - position;
	m_file->seek(m_data_chunk_location + position * frame_size, audiere::File::BEGIN);
}


int WAVInputStream::getPosition() 
{
	return m_data_chunk_length - m_frames_left_in_chunk;
}


bool WAVInputStream::findFormatChunk() 
{
	// seek to just after the RIFF header
	m_file->seek(12, audiere::File::BEGIN);

	// search for a format chunk
	for (;;)
	{
		char chunk_id[4];
		u8   chunk_length_buffer[4];

		int size         = m_file->read(chunk_id, 4);
		size            += m_file->read(chunk_length_buffer, 4);
		u32 chunk_length = read32_le(chunk_length_buffer);

		// if we couldn't read enough, we're done
		if (size != 8) 
		{
			return false;
		}

		// if we found a format chunk, excellent!
		if (memcmp(chunk_id, "fmt ", 4) == 0 && chunk_length >= 16)
		{
			// read format chunk
			u8 chunk[16];
			size = m_file->read(chunk, 16);

			// could we read the entire format chunk?
			if (size < 16) 
			{
				return false;
			}

			chunk_length -= size;

			// parse the memory into useful information
			u16 format_tag         = read16_le(chunk + 0);
			u16 channel_count      = read16_le(chunk + 2);
			u32 samples_per_second = read32_le(chunk + 4);
			//u32 bytes_per_second   = read32_le(chunk + 8);
			//u16 block_align        = read16_le(chunk + 12);
			u16 bits_per_sample    = read16_le(chunk + 14);

			// format_tag must be 1 (WAVE_FORMAT_PCM)
			// we only support mono and stereo
			if (format_tag != 1 || channel_count > 2 ||  !IsValidSampleSize(bits_per_sample))
			{				
				return false;
			}

			// skip the rest of the chunk
			if (!skipBytes(chunk_length)) 
			{
				// oops, end of stream
				return false;
			}

			// figure out the sample format
			if (bits_per_sample == 8) 
			{
				m_sample_format = audiere::SF_U8;
			}
			else if (bits_per_sample == 16) 
			{
				m_sample_format = audiere::SF_S16;
			} else 
			{
				return false;
			}

			// store the other important .wav attributes
			m_channel_count = channel_count;
			m_sample_rate   = samples_per_second;
			return true;
		} 
		else 
		{
			// skip the rest of the chunk
			if (!skipBytes(chunk_length)) 
			{
				// oops, end of stream
				return false;
			}
		}
	}
}


bool WAVInputStream::findDataChunk() 
{
	// seek to just after the RIFF header
	m_file->seek(12, audiere::File::BEGIN);

	// search for a data chunk
	while (true) 
	{
		char chunk_id[4];
		u8   chunk_length_buffer[4];

		int size = m_file->read(chunk_id, 4);
		size    += m_file->read(chunk_length_buffer, 4);
		u32 chunk_length = read32_le(chunk_length_buffer);

		// if we couldn't read enough, we're done
		if (size != 8)
		{		
			return false;
		}

		// if we found a data chunk, excellent!
		if (memcmp(chunk_id, "data", 4) == 0) 
		{
			// calculate the frame size so we can truncate the data chunk
			int frame_size = m_channel_count * GetSampleSize(m_sample_format);

			m_data_chunk_location  = m_file->tell();
			m_data_chunk_length    = chunk_length / frame_size;
			m_frames_left_in_chunk = m_data_chunk_length;
			return true;
		} 
		else 
		{
			// skip the rest of the chunk
			if (!skipBytes(chunk_length)) 
			{			
				return false; // oops, end of stream
			}
		}
	}
}


bool   WAVInputStream::skipBytes(int size) 
{
	return m_file->seek(size, audiere::File::CURRENT);
}



int  WAVInputStream::read(int frame_count, void* buffer)
{
    if (m_repeat) 
	{
		const int frame_size = GetFrameSize(this);
		// the main read loop:
		u8* out = (u8*)buffer;
		int frames_left = frame_count;
		while (frames_left > 0) 
		{

			// read some frames.  if we can't read anything, reset the stream
			// and try again.
			int frames_read = doRead(frames_left, out);
			if (frames_read == 0) 
			{
				reset();
				frames_read = doRead(frames_left, out);

				// if we still can't read anything, we're done
				if (frames_read == 0) 
				{
					break;
				}
			}
			frames_left -= frames_read;
			out += frames_read * frame_size;
		}
		return frame_count - frames_left;
	} 
	else 
	{
		return doRead(frame_count, buffer);
	}
}





}// 네임스페이스 ********************************************************************************************************************
