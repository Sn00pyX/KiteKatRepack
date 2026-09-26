#pragma once 



//해주기 - 나중에 로우필터 각 소리마다 set하게 되면 이 파일 빼주기. 





#define  MIN_DIST_CUTOFF_FREQ    22000.0f
#define  MIN_FREQ                40.0f


class  GCutOffFreqTable
{
private:
	float           m_fDistMin;
	float           m_fDistMinData;
	
	float           m_fSlope;  ///< 직선의 방정식 y=ax+b 에서  a값.  즉,기울기를뜻함.
	float           m_fConst;  ///< 직선의 방정식에서 b값.    즉, 뒤에 상수뜻함


public:
	~GCutOffFreqTable(){;}
	static GCutOffFreqTable*   GetCutOffFreqTable()
	{
		static GCutOffFreqTable  CutOff;

		return &CutOff;
	}

	void   SetCutOffFreqRate( float fDMin, float fDist, float fDistLowData)
	{
		m_fDistMin = fDMin;		

		m_fSlope =  (fDistLowData - m_fDistMinData) / (fDist - fDMin);  //기울기 구하기
		m_fConst =  fDistLowData - ( m_fSlope*fDist);                  //상수 구하기       b = y - ax
	}

	int    GetCutOffFreq( float fDist)
	{
		if(  (fDist < 0.0f) || (fDist < m_fDistMin))
		{
			return (int)m_fDistMinData;
		}

		int freq = int( (m_fSlope*fDist) + m_fConst);
		if( freq < (int)MIN_FREQ)
			freq = (int)MIN_FREQ;

		return  freq;
	}


private:
	GCutOffFreqTable()
	{
		m_fDistMin     = 10.0f;
		m_fDistMinData = MIN_DIST_CUTOFF_FREQ;
		m_fSlope       = 0.0f;
		m_fConst       = MIN_DIST_CUTOFF_FREQ;
	}	
};