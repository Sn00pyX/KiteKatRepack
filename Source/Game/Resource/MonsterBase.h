#pragma once

#include <toolkit/c_fixed.h>

#include "SkillBase.h"


struct MonsterBase
{
	MonsterBase();

	enum		ATTACK_TYPE_FLAG
	{
		AT_FIRST_ATTACK			= ( 1 << 0 ),
		AT_GROUP_FIRST_ATTACK	= ( 1 << 1 ),
		AT_RESPONSE_CASTING		= ( 1 << 2 ),
		AT_RESPONSE_RACE		= ( 1 << 3 ),
		AT_RESPONSE_BATTLE		= ( 1 << 4 ),
	};


	enum
	{
		MONSTER_TYPE_HUNTAHOLIC_1LV		= 1,
		MONSTER_TYPE_HUNTAHOLIC_2LV		= 2,
		MONSTER_TYPE_HUNTAHOLIC_3LV		= 3,
		MONSTER_TYPE_HUNTAHOLIC_BOSS	= 4,
		MONSTER_TYPE_MIDDLE_1_STAR		= 5,
		MONSTER_TYPE_MIDDLE_2_STAR		= 6,
		MONSTER_TYPE_MIDDLE_3_STAR		= 7,
		MONSTER_TYPE_MIDDLE_4_STAR		= 8,
		MONSTER_TYPE_HIGH_1_STAR		= 9,
		MONSTER_TYPE_HIGH_2_STAR		= 10,
		MONSTER_TYPE_HIGH_3_STAR		= 11,
		MONSTER_TYPE_HIGH_4_STAR		= 12,
		MONSTER_TYPE_HIGHEST_1_STAR		= 13,
		MONSTER_TYPE_HIGHEST_2_STAR		= 14,
		MONSTER_TYPE_HIGHEST_3_STAR		= 15,
		MONSTER_TYPE_HIGHEST_4_STAR		= 16,
	};

	int			uid;					// ID

	int			monster_group;			// 계열
	int			monster_type;			// 타입
	int			monster_grade_icon;		// 몬스터 등급 ICON id

	int			name_id;				// 이름

	// 몬스터 종족 ID
	enum
	{
		MONSTER_RACE_DUNGEON_CORE		= 20000,	// 던전 코어
		MONSTER_RACE_DUNGEON_CONNECTOR	= 20001,	// 던전 결계석
	};

	int			race;					// 종족
	int			group;					// 계열
	int			affiliation;				// 세부계열
		
	int			transform_level;		// 변신레벨
	int			level;					// 레벨

	float		size;
	float		scale;

	int			walk_type;				// 보행 타입
	int			visible_range;
	int			chase_range;
	int			magic_type;				// 마법 속성
	int			attack_type;

	int			hp;
	int			mp;
	int			attack_point;
	int			magic_point;
	int			defence;
	int			magic_defence;
	int			attack_speed;
	int			attack_speed_type;		// 공속 타입
	int			casting_speed;
	int			accuracy;
	int			avoid;
	int			magic_accuracy;
	int			magic_avoid;

	int			taming_code;			// 테이밍 되는 ID
	int			creature_taming_code_group;		// 크리쳐 그룹 ( 소울 크리쳐 몬스터 테이밍 테이블의 그룹 아이디 )

	c_fixed10	taming_percentage;		// 테이밍 확률
	float		taming_exp_adjust;		// 테이밍 경험치 조정
	
	int			exp;					// 부여 EXP
	int			exp_2;
	int			jp;						// 부여 JP
	int			jp_2;

	int			stat_id;				// 스탯 ID	
	int			ability;				// 특수능력
	int			walk_speed;				// 걷기 속도
	int			run_speed;				// 뛰기 속도
	int			attack_range;			// 사정 거리
	int			hidesense_range;		// 은신 감지 거리

	enum
	{
		FIGHT_TYPE_NORMAL				= 0,
		FIGHT_TYPE_ENVIRONMENT			= 1,
		FIGHT_TYPE_DUNGEON_CONNECTOR	= 2,
		FIGHT_TYPE_BOSS					= 3,
		FIGHT_TYPE_AGENT				= 4,
		FIGHT_TYPE_AUTO_TRAP			= 5,
		FIGHT_TYPE_TRAP					= 6,
		FIGHT_TYPE_NOT_MOVABLE			= 7,
		FIGHT_TYPE_RUNAWAY				= 8,
		FIGHT_TYPE_IMMORTAL_DUMMY		= 9,
	};

	int			fight_type;

	enum
	{
		TRIGGER_TYPE_NONE = 0,
		TRIGGER_TYPE_HP_BELOW_ONCE = 1,
		TRIGGER_TYPE_HP_BELOW_ALWAYS = 2,
		TRIGGER_TYPE_TIME_ONCE = 3,
		TRIGGER_TYPE_TIME_ALWAYS = 4,
		TRIGGER_TYPE_TIME_ALWAYS_INCLUDE_START = 5,
	};

	enum
	{
		NUMBER_OF_TRIGGERS_PER_ROW		= 6,
		NUMBER_OF_SKILLS_PER_ROW		= 6,
		NUMBER_OF_ITEM_DROPS_PER_ROW	= 10,
	};

	struct		MONSTER_TRIGGER
	{
		int		id;
		float	value[2];
		char	script[255];
	};

	const std::vector< MONSTER_TRIGGER > * trigger_list;

	int			gold_drop_percentage;	// 돈 떨어트릴 확률
	int			gold_min;				// 돈 최소
	int			gold_max;				// 돈 최대
	int			gold_min_2;				// 돈 최소
	int			gold_max_2;				// 돈 최대

	int			chaos_drop_percentage;	// 혼돈 떨어뜨릴 확률
	int			chaos_min;				// 혼돈 최소
	int			chaos_max;				// 혼돈 최대
	int			chaos_min_2;			// 혼돈 최소
	int			chaos_max_2;			// 혼돈 최대

	struct MONSTER_ITEM_DROP_INFO
	{
		int		item_id;
		int		percentage;		// 드랍 확률. 100000000 일경우 100%
		short	min_count;
		short	max_count;
		short	min_level;
		short	max_level;
	};

	const std::vector< MONSTER_ITEM_DROP_INFO > * item_drop_list;

	struct MONSTER_SKILL_INFO
	{
		int		skill_id;
		int		skill_lv;
		float	skill_probability;
	};

	const std::vector< MONSTER_SKILL_INFO > * skill_info_list;

	char script_on_dead[ 256 ];
};

struct SummonBase
{
	SummonBase()
		: uid( 0 )
		, type( 0 )
		, name_id( 0 )
		, card_id( 0 )
		, magic_type( 0 )
		, rate( 0 )
		, stat_id( 0 )
		, size( 1.0f )
		, scale( 1.0f )
		, riding_speed( 0 )
		, run_speed( 0 )
		, is_riding_only( 0 )
		, attack_range( 0 )
		, evolve_type( 0 )
		, evolve_target( 0 )
	{
		for( int i = 0; i < GameRule::MAX_SUMMON_SKILL_TREE ; ++i )
		{
			skill_tree_id[ i ] = 0;
			skill_tree[ i ] = NULL;
		}
	}

	enum		SUMMON_RATE
	{
		RATE_BASIC				= 0,
		RATE_COMMON				= 1,
		RATE_UNCOMMON			= 2,
		RATE_RARE				= 3,
		RATE_UNIQUE				= 4,
		RATE_EPIC				= 5,
		RATE_EXTRA				= 99,
	};

	enum		SUMMON_EVOLVE_TYPE
	{
		EVOLVE_NORMAL			= 1,	// 기본형
		EVOLVE_GROWTH			= 2,	// 성장형
		EVOLVE_EVOLVE			= 3,	// 진화형
	};

	int			uid;					// ID

	int			type;					// 계열
	int			name_id;				// 이름

	int			card_id;				// 대응되는 소환수 카드

	int			magic_type;				// 마법 속성

	int			rate;
	int			stat_id;				// 스탯 ID	

	float		size;
	float		scale;

	int			riding_speed;			// 라이딩 속도
	int			run_speed;				// 뛰기 속도

	enum	// 라이딩 전용 필드
	{
		CANT_RIDING		= 0,
		RIDING_LENT		= 1,
	};

	char		is_riding_only;			// 라이딩 전용
	int			attack_range;			// 사정 거리

	int			evolve_type;
	int			evolve_target;

	int			skill_tree_id[ GameRule::MAX_SUMMON_SKILL_TREE ];

	std::string strScript;

	std::vector< SkillTree > *skill_tree[ GameRule::MAX_SUMMON_SKILL_TREE ];
};

struct PetBase
{
	PetBase()
		: uid( 0 )
		, type( 0 )
		, name_id( 0 )
		, cage_id( 0 )
		, skill_tree_id( 0 )
		, attribute_flag( 0 )
		, size( 1.0f )
		, scale( 1.0f )
	{}

	enum _PET_BITFLAG
	{
		RATE_RARE				= 1 << 0,	// 레어 여부
		RATE_SHOVELABLE			= 1 << 1,	// 삽질 가능 여부
		RATE_ITEM_COLLECTABLE	= 1 << 2,	// 아이템 자동 줍기 가능 여부
		RATE_WEIGHT_1000		= 1 << 3,	// 소지량 증가(1000)
		RATE_WEIGHT_2000		= 1 << 4,	// 소지량 증가(2000)
		RATE_WEIGHT_4000		= 1 << 5,	// 소지량 증가(4000)
		RATE_WEIGHT_8000		= 1 << 6,	// 소지량 증가(8000)
	};

	// 소지량을 증가시키는 펫의 성능을 하드코딩하기 위해 필요한 정보들, 플래그의 수가 늘어난다면 코드가 변경되어야 한다.
	enum _PET_BITFLAG_WEIGHT
	{
		RATE_WEIGHT_INDEX		=	3,
		RATE_WEIGHT_MAX			=	RATE_WEIGHT_1000 | RATE_WEIGHT_2000 | RATE_WEIGHT_4000 | RATE_WEIGHT_8000,
	};

	int			uid;					// ID

	int			type;					// 계열
	int			name_id;				// 이름

	int			cage_id;				// 대응되는 소환수 카드
	int			skill_tree_id;			// 스킬 트리 아이디

	int			attribute_flag;			// 속성 플래그

	float		size;
	float		scale;
};

struct DropGroup
{
	DropGroup()
		: uid( 0 )
	{
		memset( drop_item_id, 0, sizeof( drop_item_id ) );
		memset( drop_min_count, 0, sizeof( drop_min_count ) );
		memset( drop_max_count, 0, sizeof( drop_max_count ) );
		memset( drop_percentage, 0, sizeof( drop_percentage ) );
	}

	enum
	{
		MAX_DROP_GROUP		= 10,
	};

	int			uid;
	int			drop_item_id[10];
	int			drop_min_count[10];
	int			drop_max_count[10];
	int			drop_percentage[10];	// 드랍 확률. 100000000 일경우 100%
};

struct _MONSTER_INFO_FILE : public MonsterBase
{
	_MONSTER_INFO_FILE()
	{
		memset( model, 0, sizeof(model) );
		motion_file_id = 0;
		weapon_type = 0;
		nCamera_x = 0;  //카메라 위치 X
		nCamera_y = 0;  //카메라 위치 Y
		nCamera_z = 0;  //카메라 위치 Z
		nTarget_x = 0;  //카메라 타겟 위치 X
		nTarget_y = 0;  //카메라 타겟 위치 Y
		nTarget_z = 0;  //카메라 타겟 위치 Z
		nMaterial = 0;			  // 재질			
		nAttack_motion_speed = 0; // 공격속도		
	}

	char	model[256];
	int     motion_file_id;
	int     weapon_type;
	int		nCamera_x;  //카메라 위치 X
	int		nCamera_y;  //카메라 위치 Y
	int		nCamera_z;  //카메라 위치 Z
	int		nTarget_x;  //카메라 타겟 위치 X
	int		nTarget_y;  //카메라 타겟 위치 Y
	int		nTarget_z;  //카메라 타겟 위치 Z
	int		nMaterial;			  // 재질			
	int		nAttack_motion_speed; // 공격속도		
};

struct _SUMMON_INFO_FILE : public SummonBase
{
	_SUMMON_INFO_FILE()
	{
		walk_type = 0;
		nMaterial				= 0;
		weapon_type = 0;
		nAttack_motion_speed	= 0;	
		nForm					= 0;					
		nEvolve_target			= 0;	

		nCamera_x = 0;  //카메라 위치 X
		nCamera_y = 0;  //카메라 위치 Y
		nCamera_z = 0;  //카메라 위치 Z
		nTarget_x = 0;
		nTarget_y = 0;
		nTarget_z = 0;

		memset( model, 0, sizeof(model) );
		motion_file_id = 0;
		nFaceID					= 0;

		memset( face_file_name, 0, sizeof(face_file_name) );

		card_id					= 0;
	}

	int		walk_type;				// 보행 타입
	int		nMaterial;				//재질				
	int		weapon_type;            //무기 종류         
	int		nAttack_motion_speed;	//공격속도			
	int		nForm;					//형태(진화)		
	int		nEvolve_target;			//진화 대상			

	int		nCamera_x;  //카메라 위치 X
	int		nCamera_y;  //카메라 위치 Y
	int		nCamera_z;  //카메라 위치 Z
	int		nTarget_x;
	int		nTarget_y;
	int		nTarget_z;

	char	model[256];
	int		motion_file_id;
	int		nFaceID;
	char	face_file_name[256];

	int		card_id;
};