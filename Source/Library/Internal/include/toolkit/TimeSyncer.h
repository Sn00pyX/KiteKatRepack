#pragma once

// 1. node A 에서 node B 가 메세지를 보내고 Echo 가 날아오는데 걸린 시간을 T 라고 하자.
//    noise 를 제거하기 위해 L ms 이상의 T 는 DC 회까지 무시한다. (L:2~300 ,  DC:2)
//    만약 MAX 회 이상 반복되면 이전 데이터부터 버린다.
// 2. T 를 일정회 이상 반복해 평균 AT를 얻는다 (N : 3)
// 3. AT / 2 를 node 간 전송 시간차라고 가정한다.


#include <vector>

class TimeSyncer
{
public:
	TimeSyncer( int L = 200, int DC = 2, int MAX = 10 )
	{
		m_L = L;
		m_DC = DC;
		m_MAX = MAX;

		m_vT.reserve( m_MAX );
	}

	void onEcho( unsigned T )
	{	
		if ( m_vT.size() == m_MAX ) *m_vT.begin() = T;
		else m_vT.push_back( T );
	}

	unsigned GetInterval()
	{
		unsigned AT = 0;

		unsigned CDC = 0;
		size_t size = m_vT.size();

		std::vector< unsigned >::iterator it;
		for ( it = m_vT.begin(); it != m_vT.end(); it++ )
		{
			if ( *it >= (unsigned)m_L )
			{
				if ( CDC++ < (unsigned)m_DC ) continue;
			}

			size++;
			AT += *it;
		}

		AT /= (unsigned int)size;

		return (AT/2);
	}

	size_t GetTestCount()
	{
		return m_vT.size();
	}

private:
	int m_L, m_DC, m_MAX;

	std::vector< unsigned >	m_vT;
};