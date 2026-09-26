#pragma once

#include <mmo/ArType.h>

#include "QuestBase.h"

struct QuestInstance
{
	typedef LONG	QuestID;	// 해당 플레이어에 대해서만 UNIQUE함이 보장된다.

	enum
	{
		MAX_STATUS = 6,
		MAX_RANDOM_VALUE = 3,
	};

	enum	QUEST_PROGRESS
	{
		NOT_STARTED	= 0,
		IN_PROGRESS	= 1,
		FINISHABLE	= 2,
		FAIL		= 100,
		FINISHED	= 255,
	};

	QuestInstance()
	{
		nId = 0;
		Code = 0;
		nProgress = NOT_STARTED;

		for( int i = 0; i < MAX_STATUS; ++i )
		{
			nStatus[i] = 0;
		}

		for( int i = 0; i < MAX_RANDOM_VALUE; ++i )
		{
			nRandomKey[i] = nRandomValue[i] = 0;
		}
	}

	QuestID					nId;

	QuestBase::QuestCode	Code;

	int						nStatus[MAX_STATUS];

	AR_TIME					nTimeLimit;

	QUEST_PROGRESS			nProgress;
	int						nStartID;
	int						nRandomKey[MAX_RANDOM_VALUE];
	int						nRandomValue[MAX_RANDOM_VALUE];
};
