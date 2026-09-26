#pragma once

#include <toolkit/c_fixed.h>

#include "ItemEffect.h"


struct TitleBase
{
	enum		
	{
		MAX_OPTION_NUMBER		= 8,
	};

	int			nID;
	int			nNameID;
	char		nRate;
	short		nOptType[MAX_OPTION_NUMBER];
	c_fixed10	fOptVar1[MAX_OPTION_NUMBER];
	c_fixed10	fOptVar2[MAX_OPTION_NUMBER];
	bool		bIsPeriodic;
	time_t		tBeginOfPeriod;
	time_t		tEndOfPeriod;
};

struct TitleBaseServer : public TitleBase
{
	const std::vector< EffectInfo * > *		pvEffectList;
};

struct TitleCondition
{
	enum
	{
		CONDITION_UNDONE		= 0,
		CONDITION_DONE			= 1,
		CONDITION_DONE_AT_FIRST	= 2,
	};

	struct TITLE_CONDITION_INFO
	{
		TITLE_CONDITION_INFO( const int _nConditionType, const __int64 _nCount, const int _nGroupID )
			: nConditionType( _nConditionType )
			, nCount( _nCount )
			, nGroupID( _nGroupID )
		{}

		int nConditionType;
		__int64 nCount;
		int nGroupID;
	};

	enum
	{
		CONDITION_QUEST_START			= 0,
		CONDITION_QUEST_END				= 1,
		CONDITION_QUEST_DROP			= 2,
		CONDITION_QUEST_TIMEOUT			= 3,
		CONDITION_QUEST_START_RANDOMLY	= 4,
	};

	int nTitleID;
	std::vector< TITLE_CONDITION_INFO > vTitleAchieveConditionInfo;
	std::vector< TITLE_CONDITION_INFO > vTitleOpenConditionInfo;
};

struct TitleConditionType
{
	enum TYPE_TITLE_CONDITION
	{
		TYPE_MONSTER_KILL_BY_CODE				= 1001,
		TYPE_MONSTER_KILL_BY_TAMING_CODE		= 1002,
		TYPE_ITEM_GET_BY_CODE					= 2001,
		TYPE_ITEM_EQUIP_BY_CODE					= 2101,
		TYPE_ITEM_USE_BY_CODE					= 2201,
		TYPE_ITEM_MIX_BY_CODE					= 2401,
		TYPE_SUMMON_TAME_BY_CODE				= 3001,
		TYPE_SUMMON_TAME_BY_RATE				= 3002,
		TYPE_SUMMON_EQUIP_BY_CODE				= 3101,
		TYPE_SUMMON_EQUIP_BY_RATE				= 3102,
		TYPE_SUMMON_ENHANCE_BY_CODE				= 3201,
		TYPE_SUMMON_ENHANCE_BY_RATE				= 3202,
		TYPE_SUMMON_MOUNT_BY_CODE				= 3301,
		TYPE_SUMMON_PUT_ON_BELT_WITH_SAME_CODE	= 3801,
		TYPE_ITEM_SUMMON_GET_BY_CODE			= 3901,
		TYPE_ITEM_SUMMON_GET_BY_RATE			= 3902,
		TYPE_QUEST_START						= 4001,
		TYPE_QUEST_END							= 4002,
		//TYPE_INSTANCE_DUNGEON_ENTER				= 5101,	// 외부제어(set_title_condition 스크립트 함수에 의해서만 완료 가능)
		TYPE_DUNGEON_SIEGE_START				= 5201,
		TYPE_DUNGEON_SIEGE_END					= 5202,
		TYPE_CHARACTER_PK_ON					= 6001,
		TYPE_CHARACTER_PLAYER_KILL				= 6002,
		TYPE_CHARACTER_IMMORAL_POINT			= 6101,
		TYPE_CHARACTER_SKILL					= 7001,
		TYPE_CHARACTER_TITLE					= 8001,
		TYPE_CHARACTER_PC_BANG_MODE				= 9000,
		TYPE_CHARACTER_GOLD						= 9001,
		//TYPE_CHARACTER_LEVEL					= 9002,	// 외부제어(set_title_condition 스크립트 함수에 의해서만 완료 가능)
	};

	int nID;
	int nCategory;
	int nValue[3];
	bool bIsSet;
	bool bSkipDBUpdate;

	std::vector< TitleCondition * >		vRelatedCondition;
};