#pragma once

#include <string>

#include <toolkit/XFlag.h>

#include "ItemBase.h"

/*
	ItemBase

	by Faruna 2005-03-29

*/

struct QuestBase
{
	typedef int			QuestCode;
	typedef XFlag<int>	BaseFlag;

	enum
	{
		MAX_VALUE_NUMBER	= 12,
		MAX_OPTIONAL_REWARD	= 6,
		MAX_FOREQUEST		= 3,
		MAX_KEY_VALUE		= 3,

	};

	QuestCode		nCode;

	// 텍스트 아이디
	int				nQuestTextId;			// 퀘스트 이름
	int				nSummaryTextId;			// 퀘스트 요약
	int				nStatusTextId;			// 퀘스트 상태 (다이얼로그 표시용)
	
	// 수행 제한
	enum		LIMIT_FLAG
	{
		LIMIT_DEVA		= 1,			// 데바
		LIMIT_ASURA		= 2,			// 아수라
		LIMIT_GAIA		= 3,			// 가이아

		LIMIT_FIGHTER	= 4,			// 전사
		LIMIT_HUNTER	= 5,			// 헌터
		LIMIT_MAGICIAN	= 6,			// 마법사
		LIMIT_SUMMONER	= 7,			// 소환사		
	};

	int			nBeginTime;				// 수락 가능 제한 시작 시각(1일 중 시/분/초 데이터를 초 단위로 환산한 값)
	int			nEndTime;				// 수락 가능 제한 종료 시각(1일 중 시/분/초 데이터를 초 단위로 환산한 값)

	int			nLimitLevel;			// 제한 최소 레벨
	int			nLimitJobLevel;			// 제한 최소 잡 레벨
	int			nLimitMaxLevel;			// 제한 최대 레벨
	int			nLimitMaxJobLevel;		// 제한 최대 잡 레벨

	BaseFlag	LimitFlag;
	int			nLimitJob;				// 직업 제한
	char		nJobDepth;				// 직업 차수( 비트셋형태, 0차 직업 = 1, 1차 직업 = 2, 2차 직업 = 4, 3차 직업 = 8 )
	int			nLimitFavor;			// 호감도 제한

	bool		bIsRepeatable;			// 반복 가능 여부

	int			nInvokeCondition;		// 발동 조건
	int			nInvokeValue;			// 발동 값

	enum		TIME_LIMIT_TYPE
	{
		TIME_LIMIT_TYPE_PERMANENT			= 0,
		TIME_LIMIT_TYPE_DECREASE_ON_GAME	= 1,
		TIME_LIMIT_TYPE_DECREASE_ALWAYS		= 2
	};
	TIME_LIMIT_TYPE		eTimeLimitType;			// 수행 시간 제한 종류
	int					nTimeLimit;				// 수행 제한 시간(단위: 초)

	enum		QuestTypeFlag
	{
		TYPE_FLAG_MISC				= ( 1 << 0 ),
		TYPE_FLAG_KILL_TOTAL		= ( 1 << 1 ),
		TYPE_FLAG_KILL_INDIVIDUAL	= ( 1 << 2 ),
		TYPE_FLAG_COLLECT			= ( 1 << 3 ),
		TYPE_FLAG_HUNT_ITEM			= ( 1 << 4 ),
		TYPE_FLAG_LEARN_SKILL		= ( 1 << 5 ),
		TYPE_FLAG_UPGRADE_ITEM		= ( 1 << 6 ),
		TYPE_FLAG_CONTACT			= ( 1 << 7 ),
		TYPE_FLAG_JOB_LEVEL			= ( 1 << 8 ),
		TYPE_FLAG_PARAMETER			= ( 1 << 9 ),
		TYPE_FLAG_RANDOM_KILL_INDIVIDUAL	= ( 1 << 10 ),
		TYPE_FLAG_RANDOM_COLLECT	= ( 1 << 11 ),
		TYPE_FLAG_HUNT_ITEM_FROM_ANY_MONSTSERS	= ( 1 << 12 ),
		TYPE_FLAG_KILL_PLAYER		= ( 1 << 13 ),

		TYPE_FLAG_KILL				= ( TYPE_FLAG_KILL_TOTAL | TYPE_FLAG_KILL_INDIVIDUAL | TYPE_FLAG_RANDOM_KILL_INDIVIDUAL ),
		TYPE_FLAG_ALL				= 0xffffffff,
	};

	enum		QuestType
	{
		QUEST_MISC							= 100,		// 그때 그때 다른 퀘스트, 스크립트로 동작
		QUEST_KILL_TOTAL					= 101,		// 처치 : 통합
		QUEST_KILL_INDIVIDUAL				= 102,		// 처치 : 각각
		QUEST_COLLECT						= 103,		// 수집
		QUEST_HUNT_ITEM						= 106,		// 수집 : 사냥
		QUEST_HUNT_ITEM_FROM_ANY_MONSTERS	= 107,		// 수집 : 사냥(어떠한 몬스터든)
		QUEST_KILL_PLAYER					= 108,		// 처치 : 유저
		QUEST_HUNT_ITEM_DROP_PENALTY		= 109,		// 수집 : 사냥(어떠한 몬스터든 드롭페널티포함)
		QUEST_LEARN_SKILL					= 201,		// 스킬 배우기
		QUEST_UPGRADE_ITEM					= 301,		// 아이템 레벨업
		QUEST_ENHANCE_ITEM					= 302,		// 아이템 강화
		QUEST_CONTACT						= 401,		// 만나기
		QUEST_JOB_LEVEL						= 501,		// 잡레벨 올리기
		QUEST_PARAMETER						= 601,		// 파라미터 설정
		QUEST_EXTERNAL_CONTROL				= 701,		// 외부제어(set_quest_status 스크립트 함수에 의해서만 완료 가능)
		QUEST_RANDOM_KILL_INDIVIDUAL		= 901,		// 랜덤 : 처치 : 각각
		QUEST_RANDOM_COLLECT				= 902,		// 랜덤 : 수집
	};

	enum
	{
		QUEST_PARAMETER_CHAOS	= 99,
	};

	QuestType	nType;

	int			nValue[MAX_VALUE_NUMBER];

	int			nDropGroupId;				// 드랍 그룹 아이디

	int			nQuestDifficulty;				// 난이도

	struct Reward
	{
		ItemBase::ItemCode	nItemCode;
		int					nLevel;
		int					nQuantity;

	};

	// 보상
	int			nFavor;
	__int64		nExp;
	int			nJP;
	int			nHuntaholicPoint;
	StructGold	nGold;
	Reward		DefaultReward;
	Reward		OptionalReward[MAX_OPTIONAL_REWARD];

	// 먼저 수행해야 할 퀘스트들
	int			nForeQuest[MAX_FOREQUEST];
	bool		bForeCheckType;	// 0 : and  1 : or

	int			nCoolTime;		// 완료 후 재수락 쿨타임(단위: 초, 0 : 사용 안 함, 음수 : 일일 단위 초기화)
	int			nDropCoolTime;	// 수락/포기 후 재수락 쿨타임(단위: 초)
};

struct QuestBaseServer : public QuestBase
{
	enum
	{
		FAVOR_GROUP_ID_CONTACT = 999,
	};
	int			nLimitFavorGroupId;
	int			nFavorGroupId;
	int			nHateGroupId;
	std::string strAcceptScript;
	std::string strClearScript;
	std::string strDropScript;
};

struct QuestLink
{
	int						nNPCId;
	QuestBase::QuestCode	code;

	bool					bLF_Start;
	bool					bLF_Progress;
	bool					bLF_End;

	int						nStartTextId;			// 퀘스트 시작시 NPC 대화
	int						nInProgressTextId;		// 퀘스트 진행시 NPC 대화
	int						nEndTextId;				// 퀘스트 종료시 NPC 대화

};

enum
{
	QUEST_IS_STARTABLE		= 0,
	QUEST_IS_IN_PROGRESS	= 1,
	QUEST_IS_FINISHABLE		= 2,
	QUEST_IS_IN_COOL_TIME	= 3,
};