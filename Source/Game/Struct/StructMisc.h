#pragma once

#include <vector>

#include <mmo/ArType.h>
#include <toolkit/c_fixed.h>

#include "CreatureBase.h"
#include "GameContent.h"

struct StructState
{
	enum StateCode
	{
		INC_ITEM_CHANCE		= 1020,		// 아이템 드랍율 증가
		HUNTING_CREATURE_CARD	= 2051,	// 헌팅 크리쳐 카드
		INC_BLOCK_CHANCE	= 1021,
		ADD_ENERGY			= 4504,

		STAMINA_SAVE		= 4003,
		SUPER_SAVE_0		= 4102,		// 성장의 물약(구 슈퍼 세이버) - Lv 1	~ Lv 120
		SUPER_SAVE_1		= 4110,		// 성장의 물약(구 슈퍼 세이버) - Lv 121	~ Lv 130
		SUPER_SAVE_2		= 4111,		// 성장의 물약(구 슈퍼 세이버) - Lv 131	~ Lv 140
		SUPER_SAVE_3		= 4112,		// 성장의 물약(구 슈퍼 세이버) - Lv 141	~ Lv 150
		SUPER_SAVE_4		= 4113,		// 성장의 물약(구 슈퍼 세이버) - Lv 121	~ Lv 150(SUPER_SAVE_1 ~ SUPER_SAVE_3 통합 버전)
		SUPER_SAVE_5		= 4116,		// 성장의 물약(구 슈퍼 세이버) - Lv 151 ~ Lv 170
		SUPER_SAVE_6		= 4119,		// 성장의 물약(구 슈퍼 세이버) - Lv 171 ~ Lv 180
		SUPER_SAVE_7		= 4120,		// 성장의 물약(구 슈퍼 세이버) - Lv 171 ~ Lv 180

		FRENZY				= 4503,		// 몬스터 프렌지

		BLESS_OF_GODDESS	= 4510,		// 여신의 축복(기부 보상)
		//STIGMA_OF_CRIME		= 4512,		// 죄의 낙인(Good 성향 중 PK 페널티) - 기부 시스템 리뉴얼 이후로 사용하지 않음

		NEMESIS_FOR_AUTO	= 5997,		// 오토 전용 천벌
		NEMESIS				= 5999,		// 천벌

		SLEEP				= 6005,		// 수면
		STUN				= 6006,		// 스턴
		HOLD				= 6007,		// 홀드
		FEAR				= 6008,		// 공포
		FROZEN				= 6009,		// 빙결
		TOTAL_FROZEN		= 314113,	// 빙결(피격 시 지속효과가 해제될 확률이 조정된 빙결)
		SEAL				= 999990,	// 공간봉인
		SHINE_WALL			= 999991,	// 샤인 월			공격과 이동이 봉쇄당한다. 데미지를 입으면 해제된다.
		STONECURSE			= 6012,		// 석화(피격 불가)
		MUTE				= 6013,		// 침묵
		HIDE				= 6016,		// 은신
		STONECURSE_MORTAL	= 6019,		// 석화(피격 가능)
		FALL_FROM_SUMMON	= 9001,		// 낙상				공격과 이동이 봉쇄당한다.
		GAIA_MEMBER_SHIP	= 9004,
		PCBANG_MEMBER_SHIP	= 9005,		// 아이콘만 있고 효과 없음
		PCBANG_PREMIUM_MEMBER_SHIP	= 9006,		// 최대 HP/MP 10% 증폭 효과 있음
		CARELESSNESS		= 9007,		// 방심				공격과 이동이 봉쇄당한다. 앉은 상태에서 피격 시

		LUNA_CHIP			= 9903,
		SUMMON_GROWING_CRACKER_1X				= 9923,		// 크리처 쑥쑥 크래커: 소환수용 스테미너 세이버 역할(플레이어에게 걸림)
		SUMMON_GROWING_CRACKER_WITH_PENALTY		= 9924,		// 크리처 쑥쑥 크래커: 소환수용 스테미너 세이버 역할(플레이어에게 걸림) - 플레이어보다 레벨 높은 소환수는 보너스 경험치 못 먹음
		SUMMON_GROWING_CRACKER_2X = 9984,		// 크리처 쑥쑥 크래커: 소환수용 스테미너 세이버 역할(플레이어에게 걸림)
		SUMMON_GROWING_CRACKER_3X = 9985,		// 크리처 쑥쑥 크래커: 소환수용 스테미너 세이버 역할(플레이어에게 걸림)
		SUMMON_GROWING_CRACKER_4X = 9986,		// 크리처 쑥쑥 크래커: 소환수용 스테미너 세이버 역할(플레이어에게 걸림)

		BURNING_STYLE		= 12671,
		DUSK_STYLE			= 12673,

		AGILE_STYLE			= 12672,
		SQUALL_OF_ARROW		= 12678,

		FROZEN_SNARE		= 13003,
		EARTH_RESTRICTION	= 13023,

		NIGHTMARE			= 13601,

		PASS_DAMAGE			= 14012,

		PET_SHOVELING_REWARD_INC_MOVE_SPEED		= 15000,		// 펫 삽질 보상(이속 증가)
		PET_SHOVELING_REWARD_DEC_MOVE_SPEED		= 15001,		// 펫 삽질 보상(이속 감소)
		PET_SHOVELING_REWARD_INC_STR_INT		= 15002,		// 펫 삽질 보상(힘/지식 증가)
		PET_SHOVELING_REWARD_DEC_STR_INT		= 15003,		// 펫 삽질 보상(힘/지식 감소)
		PET_SHOVELING_REWARD_INC_AGI_DEX		= 15004,		// 펫 삽질 보상(민첩/정신 증가)
		PET_SHOVELING_REWARD_DEC_AGI_DEX		= 15005,		// 펫 삽질 보상(민첩/정신 감소)
		PET_SHOVELING_REWARD_INC_VIT			= 15006,		// 펫 삽질 보상(체력 증가)
		PET_SHOVELING_REWARD_DEC_VIT			= 15007,		// 펫 삽질 보상(체력 감소)

		LIGHTNING_FORCE_CONGESTION	= 163420,		// 뇌기 폭주

		FUSION_WITH_SUMMON	= 164001,	// 생명의 융합 (아따-_- 영어 이름 구리다~-_-;;;)
		SUMMON_FORM			= 164002,	// 야수 융합

		PROTECTING_FORCE_OF_BEGINNING			= 201085,		// 시작의 보호기운<석화> <데스매치 전용>
		TRACE_OF_FUGITIVE						= 201084,		// 도망자의 기척<은신><데스매치 전용>

		MOVE_SPEED_SLOWDOWN	= 313205,	// 베어로드 비정상 탈퇴로 인한 페널티 디버프

		STATE_FOR_DUNGEON_OWNER_1	= 41102446,
		STATE_FOR_DUNGEON_OWNER_2	= 41102447,
	};

	enum StateGroup
	{
		GROUP_NONE								= 0,
		GROUP_PHYSICAL_BOMB						= 101,
		GROUP_MAGICAL_BOMB						= 102,
		GROUP_ASSASSIN							= 201,
		GROUP_RIDING							= 301,
		GROUP_MAGIC								= 401,		// 마법스킬에 의해 호출된 디버프 - 디스펠, 정화에 의해 제거됨
		GROUP_SKILL								= 402,		// 물리스킬에 의해 호출된 디버프 - 디스펠에 의해 제거됨
		GROUP_POISON							= 403,		// 독 또는 중독으로 정의된 지속데미지 - 해독, 정화에 의해 제거됨
		GROUP_CURSE								= 404,		// 저주로 정의된 디버프, 지속데미지 - 저주해제에 의해 제거됨
		GROUP_DISEASE							= 405,		// 질병으로 정의된 암속성, 디버프, 지속데미지 - 질병치료에 의해 제거됨
		GROUP_WOUND								= 406,		// 외상으로 정의된 지속데미지, 출혈 - 외상치료에 의해 제거됨
	};

	enum StateBaseEffect
	{
		EF_NONE									= 0,	// 기본 효과 없음
		EF_PHYSICAL_STATE_DAMAGE				= 1,	// 물리 지속 데미지
		EF_PHYSICAL_IGNORE_DEFENCE_STATE_DAMAGE	= 2,	// 방어 무시 물리 지속 데미지
		EF_MAGICAL_STATE_DAMAGE					= 3,	// 마법 지속 데미지
		EF_MAGICAL_IGNORE_RESIST_STATE_DAMAGE	= 4,	// 마방 무시 마법 지속 데미지

		EF_PHYSICAL_IGNORE_DEFENCE_PER_STATE_DAMAGE = 6,	// 방어 무시 물리 % 지속 데미지(지속효과의 추가 컬럼 사용함)

		EF_HEAL_HP_BY_MAGIC						= 11,	// 마법 HP 회복
		EF_HEAL_MP_BY_MAGIC						= 12,	// 마법 MP 회복
		EF_HEAL_SP_BY_MAGIC						= 13,	// 마법 SP 회복
		EF_HEAL_HP_BY_ITEM						= 21,	// 아이템 HP 회복
		EF_HEAL_MP_BY_ITEM						= 22,	// 아이템 MP 회복
		EF_HEAL_SP_BY_ITEM						= 23,	// 아이템 SP 회복
		EF_HEAL_HPMP_BY_ITEM					= 24,	// 아이템 HP, MP 회복
		EF_HEAL_HPMP_PER_BY_ITEM				= 25,	// 아이템 HP, MP % 회복(지속효과의 추가 컬럼 사용함)

		EF_POISON								= 51,	// 중독
		EF_VENOM								= 52,	// 맹독
		EF_BLOODY								= 53,	// 출혈
		EF_SERIOUS_BLOODY						= 54,	// 대량 출혈
	};

	enum
	{
		EF_MISC									= 0,
		EF_PARAMETER_INC						= 1,
		EF_PARAMETER_AMP						= 2,
		EF_PARAMETER_INC_WHEN_EQUIP_SHIELD		= 3,
		EF_PARAMETER_AMP_WHEN_EQUIP_SHIELD		= 4,
		EF_PARAMETER_INC_WHEN_EQUIP				= 5,	// 특정 장비 장착시 파라미터 증가
		EF_PARAMETER_AMP_WHEN_EQUIP				= 6,	// 특정 장비 장착시 파라미터 증폭
		EF_PARAMETER_MULTI						= 7,
		EF_PARAMETER_SYNCHRONIZATION			= 8,	// 두 개 이상의 파라미터 수치를 동기화 시킨다.
		EF_DOUBLE_ATTACK						= 21,
		EF_ADDITIONAL_DAMAGE_ON_ATTACK			= 22,
		EF_AMP_ADDITIONAL_DAMAGE_ON_ATTACK		= 23,
		EF_ADDITIONAL_DAMAGE_ON_SKILL			= 24,
		EF_AMP_ADDTIONAL_DAMAGE_ON_SKILL		= 25,
		EF_ADD_STATE_ON_ATTACK_OLD				= 26,
		EF_ADD_HP_ON_ATTACK						= 27,
		EF_AMP_RECEIVE_DAMAGE					= 30,
		EF_ADD_HP_MP_ON_CRITICAL				= 34,
		EF_HEALING_AMPLIFY						= 35,
		EF_ADD_STATE_ON_ATTACK					= 36,	// 공격 시 일정 확률로 지속 효과를 부가한다.
		EF_ADD_STATE_BY_SELF_ON_ATTACK			= 37,	// 공격 시 일정 확률로 자신에게 지속 효과를 부가한다.
		EF_ADD_STATE_ON_BEING_ATTACKED			= 38,	// 피격 시 일정 확률로 지속 효과를 부가한다.
		EF_ADD_STATE_BY_SELF_ON_BEING_ATTACKED	= 39,	// 피격 시 일정 확률로 자신에게 지속 효과를 부가한다.
		EF_STEAL_WITH_REGEN_STOP				= 40,	// 공격 시 확률적으로 HP또는 MP 스틸이 적용되지만 HP/MP 리젠을 중지시킬 수 있는 유형
		EF_ABSORB								= 41,
		EF_STEAL								= 42,
		EF_DAMAGE_REFLECT_PERCENT				= 43,
		EF_DAMAGE_REFLECT						= 44,
		EF_DAMAGE_REFLECT_WHEN_EQUIP_SHIELD		= 45,
		EF_DAMAGE_REDUCE_WITH_RACE_BY_PERCENT	= 46,
		EF_DAMAGE_REDUCE_WITH_RACE_BY_VALUE		= 47,
		//EF_DAMAGE_REDUCE						= 48,	// DB에서 제거됨(2차 스킬 리뉴얼)
		EF_MANA_SHIELD							= 49,
		EF_HEAL									= 61,
		EF_REGEN_ADD							= 62,
		EF_HEAL_BY_ITEM							= 63,
		EF_HEAL_HPMP_PER						= 64,	// HP, MP % 회복률
		EF_FORCE_CHIP							= 71,
		EF_SOUL_CHIP							= 72,
		EF_HEALING_CHIP							= 73,
		EF_LUNAR_CHIP							= 74,	// 71, 72, 73 번 유형 혼합형
		EF_NEUTRALIZE							= 81,
		EF_MEZZ									= 82,
		EF_PROVOKE								= 83,	// 도발(걸려있는 동안만 헤이트 작동)
		EF_DECREASE_STATE_EFFECT				= 84,
		EF_INC_HATE								= 85,
		EF_AMP_HATE								= 86,
		EF_SKILL_INTERRUPTION					= 87,
		EF_ADD_REGION_STATE						= 88,
		EF_MP_COST_INC							= 91,
		EF_ADD_PARAMETER_ON_NORMAL_ATTACK		= 92,
		EF_ADD_PARAMETER_ON_SKILL				= 93,
		EF_KNOCKBACK_ON_DEFAULT_ATTACK			= 94,
		EF_REMOVE_GOOD_STATE					= 97,
		EF_TRANSFORMATION						= 104,
		EF_DEAL_DAMAGE							= 105,
		EF_AMPLIFY_CASTING_TIME					= 106,
		EF_RESURRECTION							= 109,
		EF_DETECT_HIDE							= 111,
		EF_CREATURE_PARAMETER_AMP				= 112,
		EF_CHANGING_FORM						= 114,
		EF_CHANGING_GROUP						= 115,
		EF_HUNTING_TRAINING						= 116,	// 계열별 데미지 증감
		EF_PARAMETER_INC_TO_ACTIVE_SUMMON		= 117,	// 소환된 소환수의 파라미터 증가
		EF_RIDING								= 200,
		EF_AMP_AND_INC_ITEM_CHANCE				= 201,
		EF_ADD_SKILL							= 203,
		EF_ATTACK_LUNA							= 204,
		EF_RESIST_HARMFUL_STATE					= 2001,	// 해로운 지속효과에 대한 추가 저항
		EF_ADD_REDUCE_DAMAGE_ON_ATTACK          = 3021,
		EF_DAMAGE_REDUCE_BY_PERCENT_AND_VALUE   = 3031,
		EF_AMP_DAMAGE_BASED_ATTACK_POINT		= 3062,
		EF_INC_DOT_BASED_PARAM					= 3071,
		EF_IMMORTALIZE							= 3111,
		EF_INFINITE_SUMMON_TIME					= 3121,		
		EF_RUSH_REGION_ADDON					= 3131,
		EF_ADD_STATE_BY_SELF_ON_KILL						= 3201,	// 대상이 죽었을 경우 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_CRITICAL_ATTACK						= 3202,	// 크리티컬 공격 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_CRITICAL_ATTACK				= 3203,	// 크리티컬 공격 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_BEING_CRITICAL_ATTACKED				= 3204,	// 크리티컬 피격 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_BEING_CRITICAL_ATTACKED		= 3205,	// 크리티컬 피격 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_AVOID								= 3206,	// 회피 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_AVOID						= 3207,	// 회피 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_BLOCK								= 3208,	// 블럭 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_BLOCK						= 3209,	// 블럭 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_PERFECT_BLOCK						= 3210,	// 퍼펙트 블럭 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_PERFECT_BLOCK				= 3211,	// 퍼펙트 블럭 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_INCREASE_ENERGY_UNCONSUMPTION_RATE				= 3301,	// 일정 확률로 기공을 소모하지 않는다.
		EF_ADD_STATE_BY_SELF_ON_DEAD						= 3311,	// 사망 시 일정 확률로 자신에게 지속 효과를 부가.
		EF_AUTO_RESURRECTION_AFTER_REMOVE_STATE				= 3321,	// 해당 지속효과가 제거될 때 자동 부활
		EF_AUTO_RESURRECTION_AFTER_REMOVE_STATE2			= 3322,	// 해당 지속효과가 제거될 때 자동 부활
		EF_ADD_STATE_BY_EQUIP_ITEM							= 10001, // 아이템을 장착함으로써 지속 효과를 받음 / 아이템 효과 전용
		EF_ADD_PARTY_STATE_BY_EQUIP_ITEM					= 10002,
	};

	enum
	{
		FLAG_STR								= ( 1 << 0 ),
		FLAG_VIT								= ( 1 << 1 ),
		FLAG_AGI								= ( 1 << 2 ),
		FLAG_DEX								= ( 1 << 3 ),
		FLAG_INT								= ( 1 << 4 ),
		FLAG_MEN								= ( 1 << 5 ),
		FLAG_LUK								= ( 1 << 6 ),
		FLAG_ATTACK_POINT						= ( 1 << 7 ),
		FLAG_MAGIC_POINT						= ( 1 << 8 ),
		FLAG_DEFENCE							= ( 1 << 9 ),
		FLAG_MAGIC_DEFENCE						= ( 1 << 10 ),
		FLAG_ATTACK_SPEED						= ( 1 << 11 ),
		FLAG_CAST_SPEED							= ( 1 << 12 ),
		FLAG_MOVE_SPEED							= ( 1 << 13 ),
		FLAG_ACCURACY							= ( 1 << 14 ),
		FLAG_MAGIC_ACCURACY						= ( 1 << 15 ),
		FLAG_CRITICAL							= ( 1 << 16 ),
		FLAG_BLOCK								= ( 1 << 17 ),
		FLAG_BLOCK_DEFENCE						= ( 1 << 18 ),
		FLAG_AVOID								= ( 1 << 19 ),
		FLAG_MAGIC_RESISTANCE					= ( 1 << 20 ),
		FLAG_MAX_HP								= ( 1 << 21 ),
		FLAG_MAX_MP								= ( 1 << 22 ),
		FLAG_MAX_SP								= ( 1 << 23 ),
		FLAG_HP_REGEN_ADD						= ( 1 << 24 ),
		FLAG_MP_REGEN_ADD						= ( 1 << 25 ),
		FLAG_SP_REGEN_ADD						= ( 1 << 26 ),
		FLAG_HP_REGEN_RATIO						= ( 1 << 27 ),
		FLAG_MP_REGEN_RATIO						= ( 1 << 28 ),
		FLAG_CRITICAL_DAMAGE_BACKUP				= ( 1 << 29 ),
		FLAG_MAX_WEIGHT							= ( 1 << 30 ),
		FLAG_DAMAGE_REDUCE						= ( 1 << 31 ),
	};

	enum
	{
		FLAG_ET_NONE_RESIST						= ( 1 << 0 ),
		FLAG_ET_FIRE_RESIST						= ( 1 << 1 ),
		FLAG_ET_WATER_RESIST					= ( 1 << 2 ),
		FLAG_ET_WIND_RESIST						= ( 1 << 3 ),
		FLAG_ET_EARTH_RESIST					= ( 1 << 4 ),
		FLAG_ET_LIGHT_RESIST					= ( 1 << 5 ),
		FLAG_ET_DARK_RESIST						= ( 1 << 6 ),
		FLAG_ET_ATTACK_RANGE					= ( 1 << 8 ),
		FLAG_ET_PERFECT_BLOCK					= ( 1 << 9 ),
		FLAG_ET_IGNORE_PHYSICAL_DEFENCE			= ( 1 << 10 ),
		FLAG_ET_IGNORE_MAGICAL_DEFENCE			= ( 1 << 11 ),
		FLAG_ET_PHYSICAL_PENETRATION			= ( 1 << 12 ),
		FLAG_ET_MAGICAL_PENETRATION				= ( 1 << 13 ),
		FLAG_ET_NONE_DAMAGE						= ( 1 << 14 ),
		FLAG_ET_FIRE_DAMAGE						= ( 1 << 15 ),
		FLAG_ET_WATER_DAMAGE					= ( 1 << 16 ),
		FLAG_ET_WIND_DAMAGE						= ( 1 << 17 ),
		FLAG_ET_EARTH_DAMAGE					= ( 1 << 18 ),
		FLAG_ET_LIGHT_DAMAGE					= ( 1 << 19 ),
		FLAG_ET_DARK_DAMAGE						= ( 1 << 20 ),
		FLAG_ET_NONE_ADDITIONAL_DAMAGE			= ( 1 << 21 ),
		FLAG_ET_FIRE_ADDITIONAL_DAMAGE			= ( 1 << 22 ),
		FLAG_ET_WATER_ADDITIONAL_DAMAGE			= ( 1 << 23 ),
		FLAG_ET_WIND_ADDITIONAL_DAMAGE			= ( 1 << 24 ),
		FLAG_ET_EARTH_ADDITIONAL_DAMAGE			= ( 1 << 25 ),
		FLAG_ET_LIGHT_ADDITIONAL_DAMAGE			= ( 1 << 26 ),
		FLAG_ET_DARK_ADDITIONAL_DAMAGE			= ( 1 << 27 ),
		FLAG_CRITICAL_DAMAGE					= ( 1 << 28 ),
		FLAG_HP_REGEN_STOP						= ( 1 << 29 ),
		FLAG_MP_REGEN_STOP						= ( 1 << 30 ),
	};

	enum
	{
		FLAG_EQUIP_ONEHAND_SWORD				= ( 1 << 0 ),
		FLAG_EQUIP_TWOHAND_SWORD				= ( 1 << 1 ),
		FLAG_EQUIP_DAGGER						= ( 1 << 2 ),
		FLAG_EQUIP_TWOHAND_SPEAR				= ( 1 << 3 ),
		FLAG_EQUIP_TWOHAND_AXE					= ( 1 << 4 ),
		FLAG_EQUIP_ONEHAND_MACE					= ( 1 << 5 ),
		FLAG_EQUIP_TWOHAND_MACE					= ( 1 << 6 ),
		FLAG_EQUIP_HEAVY_BOW					= ( 1 << 7 ),
		FLAG_EQUIP_LIGHT_BOW					= ( 1 << 8 ),
		FLAG_EQUIP_CROSSBOW						= ( 1 << 9 ),
		FLAG_EQUIP_ONEHAND_STAFF				= ( 1 << 10 ),
		FLAG_EQUIP_TWOHAND_STAFF				= ( 1 << 11 ),
		FLAG_EQUIP_DOUBLE_SWORD					= ( 1 << 12 ),
		FLAG_EQUIP_DOUBLE_DAGGER				= ( 1 << 13 ),
	};

	enum
	{
		NormalAttack							= 1 << 0,
		PhysicalSkill							= 1 << 1,
		MagicalSkill							= 1 << 2,
		Helpful									= 1 << 3,
		Harmful									= 1 << 4,
	};

	enum
	{
		FLAG_ASURA								= 1 << 0,
		FLAG_DEVA								= 1 << 1,
		FLAG_GAIA								= 1 << 2,
		FLAG_ALL								= 1 << 3,
	};

	enum
	{
		FLAG_HELPFUL							= 1 << 0,
		FLAG_HARMFUL							= 1 << 1,
		FLAG_ACTIVE								= 1 << 2,
		FLAG_PASSIVE							= 1 << 3,
		FLAG_PHYSICAL							= 1 << 4,
		FLAG_MAGICAL							= 1 << 5,
	};

	enum
	{
		COMMON_RANDOM_STATE						= 0,
		EACH_RANDOM_STATE						= 1,
	};

	// 실시간으로 지속효과 부여할 대상 설정을 위한 상수
	// 효과 대상은 범위 스킬과 공용으로 사용되는 값이기 때문에 단일 개체를 대상으로 하는 유형을 추가하기에는 적합하지 않아
	// 효과 대상 자체를 사용하지 않는 별도의 상수를 추가
	enum
	{
		USE_TARGET_REGION_TYPE	= 0,	// < 실시간 지속효과 부여 > : 분산 타입, 한계 개체 수, 효과대상을 참조하여 범위 지속효과 부여
		USE_TARGET_OWNER		= 1,	// < 실시간 지속효과 부여 > : 실시간 지속효과 부여 유형을 가진 스스로에게만 지속효과 부여
	};

	enum
	{
		UNUSED_IN_DB		= 0,
		USED_IN_DB			= 1,
		USED_IN_DB_BY_DEAD	= 2,
		NOT_IN_DB			= -1,
	};

	StructState( StateCode code, int uid, AR_HANDLE caster, unsigned short level, AR_TIME start_time, AR_TIME end_time, int base_damage, bool bIsAura, int nStateValue, const char * szStateValue, const bool bByEvent, int nDBEnable, const bool bHideTime = false, const bool bInvisible = false );
	StructState();

	virtual ~StructState()
	{
	}

	void					Save( struct StructCreature * pCreature, const int nEnable );
	void					SaveByDead( struct StructCreature * pCreature );
	void					Expire( struct StructCreature * pCreature );

	inline bool				IsNeedDisplay() const
	{
		return true;
	}

	bool					IsAura() const								{ return m_bAura;	}
	bool					IsHolded() const							{ return !!GetRemainDuration(); }
	void					HoldRemainDuration();
	void					ReleaseRemainDuration();

	int						GetStateValue() const						{ return m_nStateValue; }
	const char *			GetStateStringValue() const					{ return m_szStateValue; }
	
	bool					AddState( AR_HANDLE caster, unsigned short level, AR_TIME start_time, AR_TIME end_time, int base_damage, bool bIsAura );
	inline int				GetUID() const								{ return m_nUID;		}
	inline void				SetUID( int uid )							{ m_nUID = uid;			}
	inline void				SetLevel( unsigned short level )			{ m_nLevel = level;	}
	inline void				SetStartTime( AR_TIME t )					{ m_nStartTime = t;	}
	inline void				SetEndTime( AR_TIME t )						{ m_nEndTime = t;	}
	inline AR_TIME			GetFullDuration() const						{ assert( m_nEndTime != (AR_TIME)-1 ); return m_nLostDurationByLoading + ( m_nEndTime - m_nStartTime ); }
	inline void				SetLastProcessedTime( AR_TIME t )			{ m_nLastProcessedTime = t;	}
	inline void				SetEnable( int nEnable )					{ m_nEnable = nEnable; }
	inline void				SetDBEnable( int nDBEnable )				{ m_nDBEnable = nDBEnable; }
	inline void				SetLastSavedTime( AR_TIME t )				{ m_nLastSavedTime = t; }
	inline unsigned short	GetLevel() const							{ return m_nLevel;	}
	inline StateCode		GetCode() const								{ return m_nCode;		}
	inline AR_TIME			GetStartTime() const						{ return m_nStartTime;	}
	inline AR_TIME			GetEndTime() const							{ return m_nEndTime;	}
	inline AR_TIME			GetRemainDuration() const					{ return m_nRemainDuration;	}
	inline AR_TIME			GetLastProcessedTime() const				{ return m_nLastProcessedTime;	}
	inline AR_HANDLE		GetCaster() const							{ return m_hCaster;	}
	inline int				GetEnable() const							{ return m_nEnable; }
	inline int				GetDBEnable() const							{ return m_nDBEnable; }
	inline AR_TIME			GetLastSavedTime() const					{ return m_nLastSavedTime; }
	inline bool				IsSavable() const							{ return m_nUID > 0; }
	inline bool				IsReusable() const							{ return m_nDBEnable == UNUSED_IN_DB; }
	inline bool				IsInDB() const								{ return m_nDBEnable != NOT_IN_DB; }

	// 신규 추가 필드 관련(데미지 관련)
	inline int				GetBaseDamage() const						{ return m_nBaseDamage;	}
	inline int				GetBaseEffectID() const						{ return m_pInfo->base_effect_id;	}
	inline AR_TIME			GetFireInterval() const						{ return GameContent::GetStateInfo( static_cast<int>(m_nCode) )->fire_interval * 100;		}
	inline int				GetElementalType() const					{ return m_pInfo->elemental_type;	}
	inline c_fixed10		GetAmplifyBase() const						{ return m_pInfo->amplify_base;		}
	inline c_fixed10		GetAmplifyPerSkillLevel() const				{ return m_pInfo->amplify_per_skl;	}
	inline int				GetAddDamageBase() const					{ return m_pInfo->add_damage_base;		}
	inline int				GetAddDamagePerSkillLevel() const			{ return m_pInfo->add_damage_per_skl;	}

	int						GetEffectType() const;
	c_fixed10				GetValue( int idx ) const;
	bool					IsHarmful() const;
	bool					ClearExpiredState( AR_TIME t );
	int						GetStateGroup() const;
	bool					IsDuplicatedGroup( int nGroupID ) const;

	void					SetState( StateCode code, int uid, AR_HANDLE caster, unsigned short level, AR_TIME duration, AR_TIME remain_time, AR_TIME last_fire_time, int base_damage, int state_value, const char * szStateValue, int nEnable );
	int						GetTimeType() const;

	inline void				AddTotalDamage( int damage )	{ m_nTotalDamage += damage; }
	inline int				GetTotalDamage() const			{ return m_nTotalDamage; }

	inline const bool		IsLogRequiredOnExpiration() const	{ return m_pInfo->is_log_required_on_expiration; }

	inline const bool		IsByEvent() const					{ return m_bByEvent; }

protected:

	// friend struct StatePool;

	void init( int uid, int code );

	int						m_nUID;	
	StateCode				m_nCode;
	unsigned short			m_nLevel;
	int						m_nBaseDamage;
	AR_HANDLE				m_hCaster;
	AR_TIME					m_nStartTime;
	AR_TIME					m_nEndTime;
	AR_TIME					m_nLostDurationByLoading;
	AR_TIME					m_nRemainDuration;	// 일반적으로는 사용하지 않으며 지속효과 일시정지(Hold/Release) 기능에만 사용
	AR_TIME					m_nLastProcessedTime;
	const struct StateInfo* m_pInfo;
	int						m_nTotalDamage;
	bool					m_bAura;
	int						m_nStateValue;
	char					m_szStateValue[33];
	int						m_nEnable;

	int						m_nDBEnable;
	AR_TIME					m_nLastSavedTime;
	bool					m_bByEvent;			// 이벤트에 의해 걸린 지속효과(다른 모든 동일 그룹, 코드의 지속효과를 레벨/잔여시간과 관계없이 덮어 씀. 일반적인 RemoveState로 삭제되지도 않음.)
};