#pragma once
#include <memory.h>
#include <math.h>



#define   M_PI         3.14
#define   SHRT_MIN    (-32768)        /* minimum (signed) short value */
#define   SHRT_MAX      32767         /* maximum (signed) short value */
#define   INT16_MIN    SHRT_MIN
#define   INT16_MAX    SHRT_MAX


/// ps추가
class CLowFilter
{
    struct BWBuffer
	{
    	double   x[2];
    	double   y[2];
    };
	double m_CutOffFreq;
	double m_ShortMaxDenom;
	double m_InputSamplingRate;

	double m_a0;
	double m_a1;
	double m_a2;
	double m_b1;
	double m_b2;
	double m_c;	

	BWBuffer*   chBuf[256];
	int         m_nInputChanelIndex;
	int         m_nChIndex;

public:

	CLowFilter(void):m_CutOffFreq(0.0), m_InputSamplingRate(0.0), m_nInputChanelIndex(0), m_nChIndex(0)
	{
		m_a0 = 0;
		m_a1 = 0;
		m_a2 = 0;
		m_b1 = 0;
		m_b2 = 0;
		m_c = 0;

		memset( chBuf, 0, sizeof( chBuf ) );

		m_ShortMaxDenom = 1.0 / double(SHRT_MAX);
	}

	~CLowFilter(void)
	{
		for (int i = 0; i < m_nInputChanelIndex; i++) 
		{
			delete chBuf[i];   		
		}
	}
	
	void SetSamplingRate(double inputSamplingRate)
	{
		m_InputSamplingRate = inputSamplingRate;
	}

	void SetChanelIndex( int nChanel)
	{
		m_nChIndex =0;

		m_nInputChanelIndex = nChanel;
	    for (int i = 0; i < m_nInputChanelIndex; i++) 
		{
    		chBuf[i] = new BWBuffer;
    		chBuf[i]->x[0] = chBuf[i]->x[1] = 0.0;
			chBuf[i]->y[0] = chBuf[i]->y[1] = 0.0;
		}
	}

	bool SetCut(double fHz)
	{
		if( m_InputSamplingRate == 0.0)
			return false;

		m_CutOffFreq = fHz;
		m_c  = 1.0 / tan(M_PI * m_CutOffFreq / m_InputSamplingRate);
		m_a0 = 1.0 / (1.0 + sqrt(2.0) * m_c + m_c * m_c);
		m_a1 = 2.0 * m_a0;
		m_a2 = m_a0;
		m_b1 = 2 * (1.0 - m_c * m_c) * m_a0;
		m_b2 = (1.0 - sqrt(2.0) * m_c + m_c * m_c) * m_a0;


		/* 하이패스는 이렇게만 바꿔준다.
	    cutOffFreq = f;
    
		C = tan(M_PI * cutOffFreq / inputSamplingRate);

		a0 = 1.0 / (1.0 + sqrt(2.0) * C + C * C);
		a1 = -2.0 * a0;
		a2 = a0;

		b1 = 2 * (C * C - 1.0) * a0;
		b2 = (1.0 - sqrt(2.0) * C + C * C) * a0;
		*/

		return true;
	}	


	bool  DoLowPassFilter( short *pSource , short *pDest, int nByteSize)
	{
		
		int nSize = nByteSize / 2;

		if( (m_CutOffFreq == 0.0) || (m_nInputChanelIndex ==0))
		{
			s_memcpy( pDest, nByteSize, pSource, sizeof(short)*nSize);
			return false;
		}
		


		for( int i =0; i<nSize ; ++i)
		{
			double inputSample;
			double outputSample;			 
			

			inputSample   = pSource[i] * m_ShortMaxDenom;
			BWBuffer* buf = chBuf[m_nChIndex++];
			outputSample  = m_a0 * inputSample + m_a1 * buf->x[0] + m_a2 * buf->x[1] - m_b1 * buf->y[0] - m_b2 * buf->y[1];

			if (outputSample < -1.0)
				pDest[i] = INT16_MIN;
			else if (outputSample > 1.0)
				pDest[i] = INT16_MAX;
			else
				pDest[i] = short(outputSample	* INT16_MAX);


			buf->x[1] = buf->x[0];
			buf->x[0] = inputSample;
			buf->y[1] = buf->y[0];
			buf->y[0] = outputSample;

			m_nChIndex %= m_nInputChanelIndex;
		}
		
		return true;
	}


	bool  DoLowPassFilter( unsigned char *pSource , unsigned char *pDest, int nByteSize)
	{		

		if( (m_CutOffFreq == 0.0) || (m_nInputChanelIndex ==0))
		{
			s_memcpy( pDest, nByteSize, pSource, sizeof(short)*nByteSize);
			return false;
		}
		


		for( int i =0; i<nByteSize ; ++i)
		{
			double inputSample;
			double outputSample;			 
			

			inputSample   = pSource[i] * m_ShortMaxDenom;
			BWBuffer* buf = chBuf[m_nChIndex++];
			outputSample  = m_a0 * inputSample + m_a1 * buf->x[0] + m_a2 * buf->x[1] - m_b1 * buf->y[0] - m_b2 * buf->y[1];

			if (outputSample < -1.0)
				pDest[i] = (unsigned char)INT16_MIN;
			else if (outputSample > 1.0)
				pDest[i] = (unsigned char)INT16_MAX;
			else
				pDest[i] = (unsigned char)(outputSample	* INT16_MAX);	// short(outputSample	* INT16_MAX);


			buf->x[1] = buf->x[0];
			buf->x[0] = inputSample;
			buf->y[1] = buf->y[0];
			buf->y[0] = outputSample;

			m_nChIndex %= m_nInputChanelIndex;
		}
		return true;
	}

};
