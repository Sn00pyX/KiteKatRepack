#pragma once

#include "GameMessage.h"

extern __declspec( thread ) struct StructPlayer * s_pCurrentPlayer;

class ThreadPlayerHelper
{
public:
	ThreadPlayerHelper( struct StructPlayer * pPlayer )
	{
		m_pPrevPlayer = s_pCurrentPlayer;
		s_pCurrentPlayer = pPlayer;
	}

	~ThreadPlayerHelper()
	{
		s_pCurrentPlayer = m_pPrevPlayer;
	}

private:
	struct StructPlayer * m_pPrevPlayer;
};