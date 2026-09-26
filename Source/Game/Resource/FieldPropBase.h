#pragma once

#include <string>

#include <toolkit/XFlag.h>

#include "ItemBase.h"

struct FieldPropBase
{
	FieldPropBase()
		: nPropId( 0 )
		, nPropTextId( 0 )
		, nType( 0 )
		, nLocalFlag( -1 )
		, nCastingTime( 0 )
		, nUseCount( 0 )
		, nRegenTime( 0 )
		, nLifeTime( 0 )
		, nMinLevel( 0 )
		, nMaxLevel( 0 )
		, nLimit( 0 )
		, nLimitJobId( 0 )
		, nActivateSkillId( 0 )
	{
		nActivateId[ 0 ] = nActivateId[ 1 ] = 0;
		memset( nActivateValue, 0, sizeof( nActivateValue ) );
		memset( drop_info, 0, sizeof( drop_info ) );
	}

	int			nPropId;
	int			nPropTextId;
	int			nType;
	int			nLocalFlag;
	AR_TIME		nCastingTime;
	int			nUseCount;
	AR_TIME		nRegenTime;
	AR_TIME		nLifeTime;
	int			nMinLevel;
	int			nMaxLevel;

	enum		LIMIT_FLAG
	{
		LIMIT_DEVA		= (1 << 2),		// 데바
		LIMIT_ASURA		= (1 << 3),		// 아수라
		LIMIT_GAIA		= (1 << 4),		// 가이아

		LIMIT_FIGHTER	= (1 << 10),	// 전사
		LIMIT_HUNTER	= (1 << 11),	// 헌터
		LIMIT_MAGICIAN	= (1 << 12),	// 마법사
		LIMIT_SUMMONER	= (1 << 13),	// 소환사		
	};

	enum		ACTIVATE_CHECK_TYPE
	{
		CHECK_TYPE_ITEM = 1,
		CHECK_TYPE_QUEST,
		CHECK_TYPE_SKILL,
		CHECK_TYPE_WEAR,
		CHECK_TYPE_SUMMON,
		CHECK_TYPE_PROP,
		CHECK_TYPE_MEMBER,
		CHECK_TYPE_EXIST_NEAR_MOB = 11,
		CHECK_TYPE_NON_EXIST_NEAR_MOB = 12,
		CHECK_TYPE_OWN_TACTICAL_POSITION = 13,
		CHECK_TYPE_NOT_OWNED_BATTLE_ARNEA_PROP = 14,	// 배틀 아레나 프랍을 사용하려는 유저와 현재 프랍의 소유 상태가 서로 다른 팀일 때만 가능
	};

	int			nLimit;					// 사용제한

	int			nLimitJobId;

	int			nActivateId[2];
	int			nActivateValue[2][2];

	int			nActivateSkillId;

	struct DROP_ITEM_INFO
	{
		ItemBase::ItemCode	code;
		int					ratio;
		int					min_count;
		int					max_count;
		int					min_level;
		int					max_level;
	};

	DROP_ITEM_INFO drop_info[2];

	std::string	strScript;
};

struct FieldPropSwitchingData
{
	int				prop_id;
	int				switched_prop_id;
};
