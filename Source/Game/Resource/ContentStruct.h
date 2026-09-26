#pragma once

#include <vector>
#include <string>

#include <mmo/ArType.h>
#include <mmo/ArObject.h>

#include "ItemBase.h"
#include "MonsterBase.h"
#include "SkillBase.h"


struct JobInfo
{
	JobInfo() : skill_tree( NULL )
	{};

	enum
	{
		FIGHTER		= 1,
		HUNTER		= 2,
		MAGICIAN	= 3,
		SUMMONER	= 4,
	};

	enum
	{
		GAIA		= 3,
		DEVA		= 4,
		ASURA		= 5,
	};

	enum
	{
		GAIA_BASIC_JOB		= 100,
		DEVA_BASIC_JOB		= 200,
		ASURA_BASIC_JOB		= 300,
	};

	int				id;
	int				text_id;
	int				stat_id;
	int				skill_tree_id;
	char			job_class;
	char			job_depth;
	short			up_lv;
	short			up_jlv;
	short			availble_job[4];
	std::vector< SkillTree > *skill_tree;
};

struct JobLevelBonus
{
	int				job_id;
	int				strength[4];
	int				vital[4];
	int				dexterity[4];
	int				agility[4];
	int				intelligence[4];
	int				mentality[4];
	int				luck[4];
};

struct SummonLevelBonus
{
	int				id;
	c_fixed10		strength;
	c_fixed10		vitality;
	c_fixed10		dexterity;
	c_fixed10		agility;
	c_fixed10		intelligence;
	c_fixed10		mentality;
	c_fixed10		luck;
};

struct StateInfo
{
	enum
	{
		TYPE_NORMAL		= 0,
		TYPE_DUPLICATE	= 1,
		TYPE_DEPENDENCE	= 2,
	};

	enum
	{
		MAX_STATE_VALUE		= 20,
	};

	int		id;
	int		name_id;
	int		tooltip_id;

	enum _ATTRIBUTE_FLAG
	{
		ERASE_ON_DEAD						= (1 << 0),
		ERASE_ON_LOGOUT						= (1 << 1),
		TIME_DECREASE_ON_LOGOUT				= (1 << 2),

		NOT_ACTABLE_TO_BOSS					= (1 << 3),
		NOT_ERASABLE						= (1 << 4),
		ERASE_ON_REQUEST					= (1 << 5),
		ERASE_ON_DAMAGED					= (1 << 6),
		ERASE_ON_RESURRECT					= (1 << 7),
		ERASE_ON_QUIT_HUNTAHOLIC			= (1 << 8),

		ERASE_ON_QUIT_DEATHMATCH			= (1 << 9),
		NOT_ERASABLE_ON_ENTER_DEATHMATCH	= (1 << 10),
		NOT_ACTABLE_ON_DEATHMATCH			= (1 << 11),
		ERASE_ON_COMPETE_START				= (1 << 12),
		NOT_ACTABLE_IN_COMPETE				= (1 << 13),

		ONLY_FOR_WORLD_STATE				= (1 << 14),

		ERASE_ON_QUIT_BATTLE_ARENA			= (1 << 15),
		ERASE_ON_STAND_UP					= (1 << 16),
	};

	bool	is_harmful;
	int		state_time_type;
	int		state_group;

	int		duplicate_group[3];

	char	uf_avatar;
	char	uf_summon;
	char	uf_monster;

	unsigned short	reiteration_count;
	int		base_effect_id;
	int		fire_interval;
	int		elemental_type;
	c_fixed10	amplify_base;
	c_fixed10	amplify_per_skl;
	int		add_damage_base;	
	int		add_damage_per_skl;

	int		effect_type;
	c_fixed10	fValue[ MAX_STATE_VALUE ];

	// StateResource 테이블에서 로드되지 않고 서버 설정/명령어에 따라 세팅됨
	bool	is_log_required_on_expiration;
};

struct EnhanceInfo
{
	int			nSID;

	enum
	{
		EF_WEAPON		= 0,
		EF_ARMOR		= 1,
		EF_SHIELD		= 2,
		EF_HELM			= 3,
		EF_GLOVE		= 4,
		EF_BOOTS		= 5,
		EF_BELT			= 6,
		EF_MANTLE		= 7,
		EF_ACCESSORY	= 8,
		EF_SKILL		= 9,
	};
	XFlag< int >	Flag;

	enum
	{
		RESULT_FAIL				= 1,
		RESULT_SKILL_CARD_FAIL	= 2,
		RESULT_ACCESSORY_FAIL	= 3,
		RESULT_DO_NOTHING		= 4,
	};
	int					nFailResult;
	int					nMaxEnhance;
	int					nLocalFlag;
	ItemBase::ItemCode	nNeedItemCode;

	c_fixed10			fPercentage[25];
};

struct _PROP_CONTACT_SCRIPT_INFO
{
	struct _FUNCTION_LIST
	{
		enum
		{
			TRIGER_INIT = 0,
			TRIGER_CONTACT
		};

		int trigger_id;
		std::string stdFunction;
	};

	enum
	{
		NPC = 0,
		PROP,
	};

	int							prop_id;
	float						x,y;
	int							prop_type;
	int							model_info;
	std::vector< _FUNCTION_LIST >	vFunctionList;
};

struct SetItemEffectInfo
{
	SetItemEffectInfo()
		: nSetPartID( 0 )
		, pvEffectList( NULL )
	{
		memset( nBaseType, 0, sizeof( nBaseType ) );
		memset( nOptType, 0, sizeof( nOptType ) );
		for( int i = 0 ; i < MAX_OPTION_NUMBER ; ++i )
		{
			fBaseVar1[ i ].set( 0 );
			fBaseVar2[ i ].set( 0 );
			fOptVar1[ i ].set( 0 );
			fOptVar2[ i ].set( 0 );
		}
	}

	// nSetID는 SetItemEffectInfo의 벡터에서 1개만 가지고 있으면 됨(KHash의 Key로 보관)
	int nSetPartID;

	enum
	{
		MAX_OPTION_NUMBER = 4
	};

	short		nBaseType[MAX_OPTION_NUMBER];
	c_fixed10	fBaseVar1[MAX_OPTION_NUMBER];
	c_fixed10	fBaseVar2[MAX_OPTION_NUMBER];

	short		nOptType[MAX_OPTION_NUMBER];
	c_fixed10	fOptVar1[MAX_OPTION_NUMBER];
	c_fixed10	fOptVar2[MAX_OPTION_NUMBER];

	const std::vector< EffectInfo * > *		pvEffectList;
};

struct DeathmatchInfo
{
	DeathmatchInfo( const int _id, const int _deathmatch_type, const int _min_level, const int _max_level, const std::vector< ArPosition >& vPosition )
		: id( _id )
		, deathmatch_type( _deathmatch_type )
		, min_level( _min_level )
		, max_level( _max_level )
		, m_vPosition( vPosition )
	{ }

	int							id;
	int							deathmatch_type;
	int							min_level;
	int							max_level;
	std::vector< ArPosition >	m_vPosition;
};

struct SecretDungeonInfo
{
	SecretDungeonInfo( const int _id, const ArPosition _position )
		: id( _id )
		, position( _position )
	{ }

	int							id;
	ArPosition					position;
};

struct CreatureEnhanceInfo
{
	CreatureEnhanceInfo( const float _stat_amplify, const short _card_durability, const short _slot_amount, const short _jp_addition )
		: stat_amplify( _stat_amplify )
		, card_durability( _card_durability )
		, slot_amount( _slot_amount )
		, jp_addition( _jp_addition )
	{ }

	float stat_amplify;
	short card_durability;
	short slot_amount;
	short jp_addition;
};

struct CreatureFarmInfo
{
	CreatureFarmInfo( const char _rate, const char _form, const char _enhance_level, const char _ticket_count )
		: rate( _rate )
		, form( _form )
		, enhance_level( _enhance_level )
		, ticket_count( _ticket_count )
	{ }

	char rate;
	char form;
	char enhance_level;
	char ticket_count;
};

struct MonsterCreatureInfo
{
	int nSid;
	int nUseCode;
	int arSummonCode[ GameRule::MAX_MONSTER_CREATURE_COUNT ];
};


struct SummonRandomSkillInfo
{
	int nSid;
	int nUseCode;
	int arSummonSkillID[ GameRule::MAX_RANDOM_SKILL_COUNT ];
};

#pragma pack( 1 )

struct _MARKET_INFO
{
	_MARKET_INFO( const char *_szMarketName ) : strMarketName( _szMarketName ) {}

	struct _MARKET_TAG
	{
		_MARKET_TAG( int _index, ItemBase::ItemCode _code, const StructGold & _price, const int _huntaholic_point, const int _arena_point )
			: index( _index )
			, code( _code )
			, price( _price )
			, huntaholic_point( _huntaholic_point )
			, arena_point( _arena_point )
		{}

		int						index;					// 상점 물품 인덱스
		ItemBase::ItemCode		code;					// 상점 물품 아이템 코드
		StructGold				price;					// 상점 판매가 - 루피(플레이어 입장 구매가)
		int						huntaholic_point;		// 상점 판매가 - 헌터홀릭 포인트(플레이어 입장 구매가)
		int						arena_point;			// 상점 판매가 - 아레나 포인트(플레이어 입장 구매가)
	};



	std::string					strMarketName;
	std::vector< _MARKET_TAG >	vItemList;
};

#pragma pack()