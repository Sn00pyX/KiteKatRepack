#pragma once

#include <mmo/ArType.h>

#include "ItemBase.h"
#include "CreatureBase.h"
#include "GameRule.h"

struct SkillBase
{
	enum
	{
		MAX_SKILL_LEVEL				= 50,
		MAX_TOGGLE_GROUP			= 20,
		MAX_SKILL_VALUE				= 20,
		TOGGLE_REFRESH_TIME			= 500,	// 토글형 스킬은 5초마다 다시 걸어줌
		TOGGLE_LIVE_TIME			= 1100,	// 토글형 스킬로 발동된 지속효과는 11초간 유지됨
	};

	enum
	{
		ST_INVALID									= 0,
		ST_VALID									= 1,
		ST_SYSTEM									= 2,
	};
	
	enum	// 분산 타입
	{
		DISTRIBUTION_TYPE_NO_LIMIT			= 0,
		DISTRIBUTION_TYPE_DISTRIBUTE		= 1,
		DISTRIBUTION_TYPE_RANDOM			= 2,
		DISTRIBUTION_TYPE_SEQUENTIAL_TARGET	= 3,
		DISTRIBUTION_TYPE_SEQUENTIAL_CASTER	= 4,
	};

	enum	// 체인 대상 선정 방식
	{
		CHAIN_TYPE_DISTANCE				= 0,
		CHAIN_TYPE_RANDOM				= 1,
		CHAIN_TYPE_HP_LOSS				= 2,
		CHAIN_TYPE_HP_LOSS_PERCENTAGE	= 3,
	};

	enum // 대상 제한
	{
		SKILL_EFFECT_TARGET_LIMIT_ANY_BODY	= 0,
		SKILL_EFFECT_TARGET_LIMIT_NOT_ENEMY	= 1,
		SKILL_EFFECT_TARGET_LIMIT_ONLY_ALLY	= 2,
		SKILL_EFFECT_TARGET_LIMIT_ONLY_ENEMY = 3,
	};

	enum
	{
		REGION_TYPE_DIRECTION				= 0,	// 일자형
		REGION_TYPE_ARC_CIRCLE				= 1,	// 부채꼴형
		REGION_TYPE_CROSS					= 2,	// 십자가형
	};

	enum	// effect type
	{
		EF_MISC											= 0,

		EF_RESPAWN_MONSTER_NEAR							= 2,	// 몬스터 전용 스킬 유형

		EF_PARAMETER_INC								= 3,
		EF_PARAMETER_AMP								= 4,

		EF_RESPAWN_MONSTER_RANDOMLY						= 5,	// 랜덤 소환 주문서용 스킬 유형
		EF_RESPAWN_MONSTER_WITH_DIFF_CODE				= 6,	// 소환 주문서용 스킬 유형

		EF_PHYSICAL_SINGLE_DAMAGE_T1					= 101,
		EF_PHYSICAL_MULTIPLE_DAMAGE_T1					= 102,
		EF_PHYSICAL_SINGLE_DAMAGE_T2					= 103,
		EF_PHYSICAL_MULTIPLE_DAMAGE_T2					= 104,		
		EF_PHYSICAL_DIRECTIONAL_DAMAGE					= 105,
		EF_PHYSICAL_SINGLE_DAMAGE_T3					= 106,
		EF_PHYSICAL_MULTIPLE_DAMAGE_T3					= 107,
		EF_PHYSICAL_MULTIPLE_DAMAGE_TRIPLE_ATTACK_OLD	= 108,
		EF_PHYSICAL_SINGLE_REGION_DAMAGE_OLD			= 111,
		EF_PHYSICAL_MULTIPLE_REGION_DAMAGE_OLD			= 112,
		EF_PHYSICAL_SINGLE_SPECIAL_REGION_DAMAGE_OLD	= 113,
		EF_PHYSICAL_SINGLE_DAMAGE_WITH_SHIELD			= 117,
		EF_PHYSICAL_ABSORB_DAMAGE						= 121,
		EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE_OLD	= 122,
		EF_PHYSICAL_SINGLE_DAMAGE_ADD_ENERGY_OLD		= 125,	// 30003번 유형으로 대체
		EF_PHYSICAL_SINGLE_DAMAGE_KNOCKBACK_OLD			= 131,
		EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_OLD	= 132,
		EF_PHYSICAL_SINGLE_DAMAGE_WITHOUT_WEAPON_RUSH_KNOCK_BACK	= 151,
		EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK_OLD	= 152,


		EF_MAGIC_SINGLE_DAMAGE_T1_OLD					= 201,	// 폐기 -> 231로 전환
		EF_MAGIC_MULTIPLE_DAMAGE_T1_OLD					= 202,	// 폐기 -> 232로 전환
		EF_MAGIC_SINGLE_DAMAGE_T2_OLD					= 203,	// 폐기 -> 231로 전환
		EF_MAGIC_MULTIPLE_DAMAGE_T2_OLD					= 204,	// 폐기 -> 232로 전환
		EF_MAGIC_MULTIPLE_DAMAGE_T3_OLD					= 205,
		EF_MAGIC_MULTIPLE_DAMAGE_T1_DEAL_SUMMON_HP_OLD	= 206,	// 폐기 -> 233으로 전환
		EF_MAGIC_SINGLE_REGION_DAMAGE_OLD				= 211,	// 폐기 -> 261로 전환
		EF_MAGIC_MULTIPLE_REGION_DAMAGE_OLD				= 212,	// 폐기 -> 263으로 전환
		EF_MAGIC_SPECIAL_REGION_DAMAGE_OLD				= 213,	// 폐기 -> 262로 전환
		EF_MAGIC_MULTIPLE_REGION_DAMAGE_T2_OLD			= 214,	// 기존에 없던 것 -> 263으로 추가
		EF_MAGIC_ABSORB_DAMAGE_OLD						= 221,	// 폐기 -> 235로 전환

		EF_MAGIC_SINGLE_DAMAGE								= 231,
		EF_MAGIC_MULTIPLE_DAMAGE							= 232,
		EF_MAGIC_MULTIPLE_DAMAGE_DEAL_SUMMON_HP				= 233,
		EF_MAGIC_SINGLE_DAMAGE_OR_DEATH						= 234,
		EF_MAGIC_DAMAGE_WITH_ABSORB_HP_MP					= 235,
		EF_MAGIC_SINGLE_PERCENT_DAMAGE						= 236,
		EF_MAGIC_SINGLE_PERCENT_MANABURN					= 237,
		EF_MAGIC_SINGLE_PERCENT_OF_MAX_MP_MANABURN			= 238,
		EF_MAGIC_SINGLE_DAMAGE_ADD_RANDOM_STATE				= 239,
		EF_MAGIC_SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE	= 240,	// 1체마법공격: 특정 지속효과 부여시 발동
		EF_MAGIC_MULTIPLE_DAMAGE_AT_ONCE					= 241,	// 1체마법공격: 한방 연타

		EF_MAGIC_SINGLE_REGION_DAMAGE				= 261,
		EF_MAGIC_SPECIAL_REGION_DAMAGE				= 262,
		EF_MAGIC_MULTIPLE_REGION_DAMAGE				= 263,
		EF_MAGIC_REGION_PERCENT_DAMAGE				= 264,
		EF_MAGIC_SINGLE_REGION_DAMAGE_USING_CORPSE	= 265,

		EF_ADD_HP_MP_BY_ABSORB_HP_MP				= 266,

		EF_MAGIC_SINGLE_REGION_DAMAGE_BY_SUMMON_DEAD= 267,

		EF_MAGIC_SINGLE_REGION_DAMAGE_ADD_RANDOM_STATE=268,

		EF_MAGIC_MULTIPLE_REGION_DAMAGE_AT_ONCE		= 269,

		EF_AREA_EFFECT_MAGIC_DAMAGE					= 271,
		EF_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL		= 272,
		EF_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL_T2		= 273,

		EF_ADD_STATE								= 301,
		EF_ADD_REGION_STATE							= 302,

		EF_CASTING_CANCEL_WITH_ADD_STATE			= 304,

		EF_ADD_STATE_BY_SELF_COST					= 305,
		EF_ADD_REGION_STATE_BY_SELF_COST			= 306,
		EF_ADD_STATE_BY_TARGET_TYPE					= 307,

		EF_ADD_STATES_WITH_EACH_DIFF_LV				= 308,
		EF_ADD_STATES_WITH_EACH_DIFF_LV_DURATION	= 309,
		EF_ADD_STATE_STEP_BY_STEP					= 310,
		EF_ADD_STATE_TO_CASTER_AND_TARGET			= 311,
		EF_ADD_RANDOM_STATE							= 312,
		EF_ADD_RANDOM_REGION_STATE					= 313,
		EF_ADD_STATE_BY_USING_ITEM					= 314,

		EF_AREA_EFFECT_MAGIC_DAMAGE_OLD				= 352,	// 폐기 -> 271로 전환
		EF_AREA_EFFECT_HEAL							= 353,

		EF_TRAP_PHYSICAL_DAMAGE						= 381,
		EF_TRAP_MAGICAL_DAMAGE						= 382,
		EF_TRAP_MULTIPLE_PHYSICAL_DAMAGE			= 383,
		EF_TRAP_MULTIPLE_MAGICAL_DAMAGE				= 384,

		EF_REMOVE_BAD_STATE							= 401,
		EF_REMOVE_GOOD_STATE						= 402,
		EF_ADD_HP									= 501,
		EF_ADD_MP									= 502,
		EF_RESURRECTION								= 504,
		EF_ADD_HP_MP								= 505,
		EF_ADD_HP_MP_BY_SUMMON_DAMAGE				= 506,
		EF_ADD_HP_MP_BY_SUMMON_DEAD					= 507,
		EF_ADD_REGION_HP_MP							= 508,
		EF_ADD_HP_BY_ITEM							= 509,
		EF_ADD_MP_BY_ITEM							= 510,
		EF_CORPSE_ABSORB							= 511,
		EF_ADD_HP_MP_BY_STEAL_SUMMON_HP_MP			= 512,
		EF_ADD_HP_MP_WITH_LIMIT_PERCENT				= 513,

		EF_ADD_REGION_HP							= 521,
		EF_ADD_REGION_MP							= 522,

		EF_SUMMON									= 601,
		EF_UNSUMMON									= 602,
		EF_UNSUMMON_AND_ADD_STATE					= 605,

		EF_TOGGLE_AURA								= 701,
		EF_TOGGLE_DIFFERENTIAL_AURA					= 702, // 이름 애매하구로...-_-

		EF_TAUNT									= 900,
		EF_REGION_TAUNT								= 901,
		EF_REMOVE_HATE								= 902,
		EF_REGION_REMOVE_HATE						= 903,
		EF_REGION_REMOVE_HATE_OF_TARGET				= 904,

		EF_CORPSE_EXPLOSION							= 1001,

		EF_CREATE_ITEM								= 9001,
		EF_ACTIVATE_FIELD_PROP						= 9501,
		EF_REGION_HEAL_BY_FIELD_PROP				= 9502,
		EF_AREA_EFFECT_HEAL_BY_FIELD_PROP			= 9503,

		// passive
		//EF_DOUBLE_WEAPON							= 10011,	// 사용되지 않는 유형(없어짐)
		EF_WEAPON_MASTERY							= 10001,
		EF_BATTLE_PARAMTER_INCREASE					= 10002,
		EF_BLOCK_INCREASE							= 10003,
		EF_ATTACK_RANGE_INCREASE					= 10004,
		EF_RESISTANCE_INCREASE						= 10005,
		EF_MAGIC_REGISTANCE_INCREASE				= 10006,
		EF_SPECIALIZE_ARMOR							= 10007,
		EF_INCREASE_BASE_ATTRIBUTE					= 10008,
		EF_INCREASE_EXTENSION_ATTRIBUTE				= 10009,	// 화살 데미지 감소 옵션 추가해야됨

		EF_SPECIALIZE_ARMOR_AMP						= 10010,	// 직업의상 특화: 증폭
		EF_AMPLIFY_BASE_ATTRIBUTE					= 10011,	// 기본 능력치 증폭

		EF_MAGIC_TRAINING							= 10012,
		EF_HUNTING_TRAINING							= 10013,	// 계열별 데미지 증감
		EF_BOW_TRAINING								= 10014,
		EF_INCREASE_STAT							= 10015,
		EF_AMPLIFY_STAT								= 10016,

		EF_INCREASE_HP_MP							= 10021,	// HP/MP 최대치/회복량 증가
		EF_AMPLIFY_HP_MP							= 10022,	// HP/MP 최대치/회복량 증폭
		EF_HEALING_AMPLIFY							= 10023,
		EF_HEALING_AMPLIFY_BY_ITEM					= 10024,	// 아이템으로 인한 회복량 증폭
		EF_HEALING_AMPLIFY_BY_REST					= 10025,	// 앉아있을 때 HP, MP 리젠 증폭
		EF_HATE_AMPLIFY								= 10026,	// 위협수준 증폭

		EF_INCREASE_SUMMON_HP_MP_SP					= 10031,	// 편성된 소환수의 HP/MP/SP 최대치/회복량 증가
		EF_AMPLIFY_SUMMON_HP_MP_SP					= 10032,	// 편성된 소환수의 HP/MP/SP 최대치/회복량 증폭
		EF_BELT_ON_PARAMETER_INC					= 10035,	// 소환수 카드 스텟 중 벨트 장착시 스텟(힘/체/지능 등등) 증가 패시브
		EF_BELT_ON_ATTRIBUTE_INC					= 10036,	// 소환수 카드 스텟 중 벨트 장착시 기본 속성(공/방/회피 등등) 증가 패시브
		EF_BELT_ON_ATTRIBUTE_EX_INC					= 10037,	// 소환수 카드 스텟 중 벨트 장착시 확장 속성(HP/MP, 블럭률, 크리율 등등) 증가 패시브
		EF_BELT_ON_ATTRIBUTE_EX2_INC				= 10038,	// 소환수 카드 스텟 중 벨트 장착시 확장 속성(시전, 캐스팅, 화살 데미지 감소 등등) 증가 패시브

		EF_UNIT_EXPERT								= 10041,

		EF_BELT_ON_PARAMETER_AMP					= 10042,	// 크리처 : 카드 스탯 : 증폭
		EF_BELT_ON_ATTRIBUTE_AMP					= 10043,	// 크리처 : 카드 기본능력치 : 증폭
		EF_BELT_ON_ATTRIBUTE_EX_AMP					= 10044,	// 크리처 : 카드 확장능력치 : 증폭
		EF_BELT_ON_ATTRIBUTE_EX2_AMP				= 10045,	// 크리처 : 카드 확장능력치 : 증폭

		EF_SUMMON_ITEM_EXPERT						= 10046,	// 크리처 : 아이템 성능 적용률 지정

		EF_ADD_STATE_ON_ATTACK						= 10048,	// 공격 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_ATTACK				= 10049,	// 공격 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_BEING_ATTACKED				= 10050,	// 피격 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_BEING_ATTACKED		= 10051,	// 피격 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_KILL						= 10052,	// 대상이 죽었을 경우 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_CRITICAL_ATTACK						= 10053,	// 크리티컬 공격 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_CRITICAL_ATTACK				= 10054,	// 크리티컬 공격 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_BEING_CRITICAL_ATTACKED				= 10055,	// 크리티컬 피격 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_BEING_CRITICAL_ATTACKED		= 10056,	// 크리티컬 피격 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_AVOID								= 10057,	// 회피 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_AVOID						= 10058,	// 회피 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_BLOCK								= 10059,	// 블럭 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_BLOCK						= 10060,	// 블럭 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_STATE_ON_PERFECT_BLOCK						= 10061,	// 퍼펙트 블럭 시 일정 확률로 지속 효과를 부가, 패시브.
		EF_ADD_STATE_BY_SELF_ON_PERFECT_BLOCK				= 10062,	// 퍼펙트 블럭 시 일정 확률로 자신에게 지속 효과를 부가, 패시브.
		EF_ADD_ENERGY_ON_ATTACK								= 32262,	// 공격 시 기공 생성
		EF_ADD_ENERGY_ON_BEING_ATTACKED						= 32263,	// 피격 시 기공 생성
		EF_INC_SKILL_COOL_TIME_ON_ATTACK					= 10063,	// 공격 시 일정 확률로 지정된 스킬의 쿨타임을 증가
		EF_INC_SKILL_COOL_TIME_ON_BEING_ATTACKED			= 10064,	// 피격 시 일정 확률로 지정된 스킬의 쿨타임을 증가
		EF_INC_SKILL_COOL_TIME_ON_KILL						= 10065,	// 대상이 죽었을 경우 일정 확률로 지정된 스킬의 쿨타임을 증가
		EF_INC_SKILL_COOL_TIME_ON_CRITICAL_ATTACK			= 10066,	// 크리티컬 공격 시 일정 확률로 지정된 스킬의 쿨타임을 증가
		EF_INC_SKILL_COOL_TIME_ON_BEING_CRITICAL_ATTACKED	= 10067,	// 크리티컬 피격 시 일정 확률로 지정된 스킬의 쿨타임을 증가
		EF_INC_SKILL_COOL_TIME_ON_AVOID						= 10068,	// 회피 시 일정 확률로 지정된 스킬의 쿨타임을 증가
		EF_INC_SKILL_COOL_TIME_ON_BLOCK						= 10069,	// 블럭 시 일정 확률로 지정된 스킬의 쿨타임을 증가
		EF_INC_SKILL_COOL_TIME_ON_PERFECT_BLOCK				= 10070,	// 퍼펙트 블럭 시 일정 확률로 지정된 스킬의 쿨타임을 증가
		EF_INC_SKILL_COOL_TIME_ON_SKILL_OF_ID				= 32281,	// 특정 스킬 시전 시 일정 확률로 지정된 스킬의 쿨타임을 증가

		// 3월 23일부터 신규 정리 유형(기존 유형 대체로 추가됨)
		EF_PHYSICAL_SINGLE_DAMAGE						= 30001,	// 물리 공격
		EF_PHYSICAL_SINGLE_DAMAGE_ABSORB				= 30002,	// 물리 공격: 흡수
		EF_PHYSICAL_SINGLE_DAMAGE_ADD_ENERGY			= 30003,	// 물리 공격: 명중시 기공 생성
		EF_PHYSICAL_SINGLE_DAMAGE_RUSH					= 30004,	// 물리 공격: 돌진
		EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK		= 30005,	// 물리 공격: 돌진: 넉백
		EF_PHYSICAL_SINGLE_DAMAGE_KNOCKBACK				= 30006,	// 물리 공격: 넉백

		EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK		= 30007,	// 물리 공격: 넉백: 타겟 중심 범위
		EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_SELF	= 30008,	// 물리 공격: 넉백: 술자 중심 범위

		EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE			= 30009,	// 물리 공격: 실시간 연타
		EF_PHYSICAL_MULTIPLE_DAMAGE_TRIPLE_ATTACK		= 30010,	// 물리 공격: 한 방 연타 - 연타 횟수 3단계 적용

		EF_PHYSICAL_SINGLE_REGION_DAMAGE						= 30011,	// 물리 공격: 타겟 중심 범위
		EF_PHYSICAL_MULTIPLE_REGION_DAMAGE						= 30012,	// 물리 공격: 한 방 연타: 타겟 중심 범위
		EF_PHYSICAL_SINGLE_SPECIAL_REGION_DAMAGE				= 30013,	// 물리 공격: 특수 범위
		EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE				= 30014,	// 물리 공격: 특수 범위: 한 방 연타: 타겟 중심 범위
		EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE_SELF			= 30015,	// 물리 공격: 특수 범위: 한 방 연타: 술자 중심 범위
		EF_PHYSICAL_MULTIPLE_DAMAGE								= 30016,	// 물리 공격: 한 방 연타
		EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE_KNOCKBACK			= 30017,	// 물리 공격: 실시간 연타: 마지막 타격시 넉백
		EF_PHYSICAL_REALTIME_MULTIPLE_REGION_DAMAGE				= 30018,	// 물리 공격: 실시간 연타: 타겟 중심 범위
		EF_PHYSICAL_SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE	= 30019,	// 물리 공격: 특정 지속효과 부여시 발동
		EF_PHYSICAL_SINGLE_REGION_DAMAGE_ADDING_MAGICAL_DAMAGE	= 30020,	// 물리 공격: 타겟 중심 범위: 마법공격력 추가

		EF_PHYSICAL_SINGLE_REGION_DAMAGE_WITH_CAST_CANCEL	= 30030,	// 물리 공격: 타겟 중심 범위: 성공시 캐스팅 캔슬

		EF_RESURRECTION_WITH_RECOVER					= 30501,	// 부활: HP, MP, EXP 복구
		EF_REMOVE_STATE_GROUP							= 30601,	// 지속효과 그룹 제거
		EF_LOTTO										= 30701,	// 루피 뻥튀기 스킬

		EF_WEAPON_TRAINING								= 31001,	// 무기 수련: 증폭
		EF_AMPLIFY_BASE_ATTRIBUTE_OLD					= 31002,	// 기본 능력치 증폭
		EF_AMPLIFY_EXT_ATTRIBUTE						= 31003,	// 확장 능력치 증폭

		EF_AMPLIFY_EXP_FOR_SUMMON						= 32001,	// 크리처 경험치 획득량 증폭

		EF_ENHANCE_SKILL								= 32011,	// 강화 스킬

		EF_MAGIC_SINGLE_DAMAGE_WITH_PHYSICAL_DAMAGE		= 32021,	// 1체 마법 공격 : 표준 + 물리 공격

		// TODO : 작명 센스가 구리다. 조금 더 나은 이름을 찾자.
		EF_INC_DAMAGE_BY_TARGET_STATE					= 32031,	// 공격 대상에게 걸려 있는 지속 효과 조건이 일치하면 입히는 피해량 증가
		EF_AMP_DAMAGE_BY_TARGET_STATE					= 32032,	// 공격 대상에게 걸려 있는 지속 효과 조건이 일치하면 입히는 피해량 증폭
		EF_TRANFER_HEALING								= 32051,	// 회복 주문 사용시 자신에게 일정 수치의 회복량 이전
		EF_PHYSICAL_CHAIN_DAMAGE						= 32061,	// 연쇄 물리 공격
		EF_MAGIC_CHAIN_DAMAGE							= 32062,	// 연쇄 마법 공격
		EF_CHAIN_HEAL									= 32063,	// 연쇄 회복
		EF_PHYSICAL_SINGLE_DAMAGE_DEMINISHED_HP_MP      = 32141,    // 물리 공격: 대상의 소모된 HP/MP에 비례하여 추가 데미지
		EF_MODIFY_SKILL_COST							= 32171,	// 스킬 사용 소모 수치 감소
		EF_RESIST_HARMFUL_STATE							= 32183,	// 해로운 지속효과에 대한 추가 저항
		EF_INC_SKILL_COOL_TIME							= 32191,	// 재사용 시간 증감 (증가형)
		EF_AMP_SKILL_COOL_TIME							= 32192,	// 재사용 시간 증감 (증폭형)
		EF_INC_DAMAGE_INC_CRIT_RATE_BY_TARGET_HP_RATIO	= 32201,	// 입히는 피해량 증가/크리티컬 확률 증가 (생명력 % 조건)
		EF_AMP_DAMAGE_INC_CRIT_RATE_BY_TARGET_HP_RATIO	= 32202,	// 입히는 피해량 증폭/크리티컬 확률 증가 (생명력 % 조건)
		EF_ABSORB_DAMAGE                                = 32211,    // 데미지의 일부를 HP/MP로 회복
		EF_STEAL_HP_MP                                  = 32212,    // 적의 HP/MP의 일부를 스틸
		EF_PHYSICAL_SINGLE_DAMAGE_PROP_REMAIN_MP        = 32251,    // 물리 공격: 자신의 남은 MP를 반영하여 추가 데미지 입힘
		EF_REPLENISH_ENERGY_HP_MP                       = 32261,    // 기공을 최대치로 충전하고 그에 비례하여 HP/MP 회복
		EF_INCREASE_ENERGY_UNCONSUMPTION_RATE			= 32264,	// 일정 확률로 기공을 소모하지 않는다.

		EF_INC_PARAM_AMPLIFY_HEAL						= 32271,	// 파라메터 증가 + 자신의 HP/MP 회복률 증폭
		EF_AMP_PARAM_AMPLIFY_HEAL						= 32272,	// 파라메터 증폭 + 자신의 HP/MP 회복률 증폭

		EF_INC_PARAM_BY_STATE							= 32291,	// 설정한 지속 효과 중 1개 이상 걸리면 파라메터 증가
		EF_AMP_PARAM_BY_STATE							= 32292,	// 설정한 지속 효과 중 1개 이상 걸리면 파라메터 증폭

		EF_INC_PARAM_BASED_PARAM					= 32301,	// 원본 비트셋 총합의 일정 비율만큼 파라미터 증가
		EF_INC_SUMMON_PARAM_BASED_PARAM				= 32302,	// 아바타 원본 비트셋 총합의 일정 비율 만큼 크리처 파라미터 증가
		EF_INC_SUMMON_PARAM_BASED_SUMMON_PARAM		= 32303,	// 크리처 원본 비트셋 총합의 일정 비율 만큼 크리처 파라미터 증가
		EF_INC_PARAM_BASED_SUMMON_PARAM				= 32304,	// 크리처 원본 비트셋 총합의 일정 비율 만큼 아바타 파라미터 증가
	};

	enum	// interrupt type
	{
		IT_ABSOLUTE						= 0,	// 방해, 캔슬을 당하지 않는 스킬
		IT_DELAY						= 1,	// 캐스팅 방해를 받는 스킬
		IT_CANCEL						= 2,	// 캐스팅캔슬을 당하는 스킬
	};

	enum	// interrupt level
	{
		IL_LOW							= 0,	// 낮음 : 0.2초의 캐스팅 감소 혹은 20%의 확률로 캐스팅이 캔슬됨
		IL_MEDIUM						= 1,	// 보통 : 0.5초의 캐스팅 감소 혹은 50%의 확률로 캐스팅이 캔슬됨
		IL_HIGH							= 2,	// 높음 :   1초의 캐스팅 감소 혹은 100%의 확률로 캐스팅이 캔슬됨
	};

	enum	// skill class/category
	{
		SC_PHYSICAL						= 1,
		SC_MAGICAL						= 2,
		SC_EVERY						= 99,
	};

	enum
	{
		TF_ADD_STATE					= 1 << 1,
	};

	enum
	{
		TARGET_MISC										= 0,
		TARGET_TARGET									= 1,
		TARGET_REGION_WITH_TARGET						= 2,
		TARGET_REGION_WITHOUT_TARGET					= 3,
		TARGET_REGION									= 4,
		TARGET_TARGET_EXCEPT_CASTER						= 6,
		TARGET_PARTY									= 21,
		TARGET_GUILD									= 22,
		TARGET_ATTACKTEAM								= 23,
		TARGET_SUMMON									= 31,
		TARGET_PARTY_SUMMON								= 32,
		TARGET_REGION_NEAR_MAIN_SUMMON_WITHOUT_TARGET	= 44,	// 메인 크리처 주변 범위(크리처 자신 제외)
		TARGET_SELF_WITH_SUMMON							= 45,	// 자신과 자신의 크리처 모두
		TARGET_PARTY_WITH_SUMMON						= 51,	// 파티원 전원 + 파티원 전원의 크리처
		TARGET_MASTER									= 101,	// 자신의 소환자(소환수 전용)
		TARGET_SELF_WITH_MASTER							= 102,	// 자신과 자신의 소환자(소환수 전용)
		TARGET_CREATURE_TYPE_NONE						= 201,
		TARGET_CREATURE_TYPE_FIRE						= 202,
		TARGET_CREATURE_TYPE_WATER						= 203,
		TARGET_CREATURE_TYPE_WIND						= 204,
		TARGET_CREATURE_TYPE_EARTH						= 205,
		TARGET_CREATURE_TYPE_LIGHT						= 206,
		TARGET_CREATURE_TYPE_DARK						= 207,
	};

	//사용가능/불가/강제 공격 처리
	enum
	{
		USE_SELF = 0,	//자신
		USE_ALLY,		//파티원
		USE_GUILD,		//길드원
		USE_NEUTRAL,	//중립
		USE_PURPLE,		//보라
		USE_ENEMY,		//적
	};

	enum
	{
		NEED_TARGET_SELF	=	0,
		NEED_TARGET_TARGET	=	1,
		NEED_TARGET_GROUND	=	2,
	};

	enum
	{
		NEED_STATE_EXHAUSE_NONE		= 0,	// 스킬 시전 시 필요 지속효과, 소모하지 않음
		NEED_STATE_EXHAUSE_ALL		= 1,	// 스킬 시전 시 필요 지속효과, 모두 소모
		NEED_STATE_EXHAUSE_SPECIFIC	= 2,	// 스킬 시전 시 필요 지속효과, 필요 지속 레벨만큼만 소모
	};

	int			GetID() const				{ return uid; }
	int			GetNameID() const			{ return name_id; }

	bool		IsValid() const				{ return is_valid != ST_INVALID; }      //구현 여부
	bool		IsSystemSkill() const		{ return is_valid == ST_SYSTEM; }	    //귀환 스킬등, 존재하지만 스킬창에 보여지면 안 되는 것

	bool		IsPassive() const			{ return !is_active; }		//패시브, 액티브 인가?
	bool		IsPhysicalSkill() const		{ return !is_spell_act; }	//물리스킬, 마법 인가?
	bool		IsMagicalSkill() const		{ return !!is_spell_act;	}
	bool		IsHarmful() const			{ return !!is_harmful; }
	char		IsNeedTarget() const		{ return is_need_target; }//타겟이 필요한가?
	bool		IsValidToCorpse() const		{ return !!is_corpse; }     //시체가 필요한가?
	bool		IsToggle() const            { return !!is_toggle; }     //토글형 인가?

	int			GetCastRange() const		{ return cast_range; }
	int			GetValidRange()	const		{ return valid_range; }

	int			GetCastCancelType() const		{ return cancel_type;	}
	int			GetCastCancelLevel() const		{ return cancel_level;	}

	int			GetToggleGroup() const		{ return toggle_group; }
	int			GetSkillTargetType() const	{ return target; }
	int			GetSkillEffectType() const	{ return effect_type; }
	int			GetElementalType() const	{ return elemental; }
	bool		CheckEffectType( int nFlag ) const	{ return !!(effect_flag & nFlag); }

	bool		IsNeedShield() const		{ return !!vf_shield_only; }
	bool		IsNeedWeapon() const		{ return !vf_is_not_need_weapon; }
	bool		IsUseableWeapon( ItemBase::ItemClass c ) const
	{
		switch( c )
		{
		case ItemBase::CLASS_ONEHAND_SWORD	:	if( vf_one_hand_sword ) return true; break;
		case ItemBase::CLASS_TWOHAND_SWORD	:	if( vf_two_hand_sword ) return true; break;
		case ItemBase::CLASS_DOUBLE_DAGGER	:	if( vf_double_dagger  ) return true; break;
		case ItemBase::CLASS_DOUBLE_SWORD	:	if( vf_double_sword	  )	return true; break;
		case ItemBase::CLASS_DAGGER			:	if( vf_dagger         ) return true; break;
		case ItemBase::CLASS_TWOHAND_SPEAR	:	if( vf_spear          ) return true; break;
		case ItemBase::CLASS_TWOHAND_AXE	:	if( vf_axe            ) return true; break;
		case ItemBase::CLASS_ONEHAND_AXE	:	if( vf_one_hand_axe   ) return true; break;
		case ItemBase::CLASS_DOUBLE_AXE		:	if( vf_double_axe     ) return true; break;
		case ItemBase::CLASS_ONEHAND_MACE	:	if( vf_one_hand_mace  ) return true; break;
		case ItemBase::CLASS_TWOHAND_MACE	:	if( vf_two_hand_mace  ) return true; break;
		case ItemBase::CLASS_LIGHT_BOW		:	if( vf_lightbow       ) return true; break;
		case ItemBase::CLASS_HEAVY_BOW		:	if( vf_heavybow		  ) return true; break;
		case ItemBase::CLASS_CROSSBOW		:	if( vf_crossbow       ) return true; break;
		case ItemBase::CLASS_ONEHAND_STAFF	:	if( vf_one_hand_staff ) return true; break;
		case ItemBase::CLASS_TWOHAND_STAFF	:	if( vf_two_hand_staff ) return true; break;
		case ItemBase::CLASS_SHIELD         :   if( vf_shield_only    ) return true; break;
		default: return false;
		}
		return false;
	}

	bool		IsUseableOnAvatar() const		{ return !!tf_avatar; }
	bool		IsUseableOnMonster() const		{ return !!tf_monster; }
	bool		IsUseableOnSummon() const		{ return !!tf_summon; }

	bool		IsLimitedAddedSkill() const		{ return !!skill_lvup_limit; }

	int			GetCoolTimeGroupId() const				{ return cool_time_group_id; }
	int			GetProbabilityOnHit( int slv ) const	{ return probability_on_hit + slv * probability_inc_by_slv; }
	int			GetStateId() const						{ return state_id; }
	int			GetStateLevel( int skill_lv, int enhance_lv ) const		{ return state_level_base + int( state_level_per_skl*skill_lv ) + int(state_level_per_enhance*enhance_lv); }
	int			GetStateSecond( int skill_lv, int enhance_lv ) const	{ return int( state_second+state_second_per_skl*(skill_lv) ) + int(state_second_per_enhance*enhance_lv); }

	int			GetCostEXP( int skill_lv, int enhance ) const	{ return cost_exp + cost_exp_per_enhance*enhance; }
	int			GetCostJP( int skill_lv, int enhance ) const	{ return cost_jp + cost_jp_per_enhance*enhance; }
	int			GetCostItemCode() const				{ return cost_item; }
	const __int64	GetCostItemCount( int skill_lv ) const			{ return cost_item_count + ( skill_lv * cost_item_count_per_skl ); }
	int			GetCostHP( int skill_lv ) const					{ return cost_hp + cost_hp_per_skl*(skill_lv); }
	float		GetCostHPPercent( int skill_lv ) const			{ return cost_hp_per + cost_hp_per_skl_per * skill_lv; }
	int			GetCostMP( int skill_lv, int enhance ) const	{ return cost_mp + cost_mp_per_skl*(skill_lv) + cost_mp_per_enhance*(enhance); }
	float		GetCostMPPercent( int skill_lv ) const			{ return cost_mp_per + cost_mp_per_skl_per * skill_lv; }
	c_fixed10	GetCostEnergy( int skill_lv ) const	{ return cost_energy + cost_energy_per_skl*(skill_lv); }

	int			GetNeedLevel() const		{ return need_level; }
	int			GetNeedHP() const			{ return need_hp; }
	int			GetNeedMP() const			{ return need_mp; }

	AR_UNIT		GetFireRange() const		{ return AR_UNIT(valid_range * GameRule::DEFAULT_UNIT_SIZE); }

	AR_TIME		GetCoolTime( int skill_lv, int enhance ) const		{ return AR_TIME( GetCoolTimeAsFloat( skill_lv, enhance ) * 100.0f ); }
	AR_TIME		GetCommonDelay() const					{ return AR_TIME( GetCommonDelayAsFloat() * 100.0f );   }
	AR_TIME		GetCastDelay( int skill_lv, int enhance ) const		{ return AR_TIME( GetCastDelayAsFloat( skill_lv, enhance ) * 100.0f ); }

	float		GetCoolTimeAsFloat( int skill_lv, int enhance ) const		{ if( GameRule::bIgnoreSkillCoolTime ) return 0; return delay_cooltime * ( delay_cooltime_mod_per_enhance * enhance + 1.0f ) + delay_cooltime_per_skl * skill_lv; }
	float		GetCommonDelayAsFloat() const					{ return delay_common;   }
	float		GetCastDelayAsFloat( int skill_lv, int enhance ) const		{ return ( delay_cast + delay_cast_per_skl * (skill_lv) ) * ( delay_cast_mod_per_enhance * enhance + 1.0f ); }

	int			GetHitBonus( int enhance, int level_diff ) const	{ return hit_bonus + enhance * hit_bonus_enhace + level_diff * percentage; }

	c_fixed10	GetVar( int idx ) const		{ return var[idx]; }
	c_fixed10	GetVar1() const				{ return var[0]; }
	c_fixed10	GetVar2() const				{ return var[1]; }
	c_fixed10	GetVar3() const				{ return var[2]; }
	c_fixed10	GetVar4() const				{ return var[3]; }

	int			GetHateMod() const			{ return int(hate_mod); }

	int			GetCriticalBonus( int skill_lv ) const	{ return critical_bonus + critical_bonus_per_skl * skill_lv; }

	// 강화 단계에 따른 수치 ========================
	
	// 카드/스킬 공통
	// MP 소모
	// 캐스팅 증가율
	int GetCastDelayByEnhance( int nEnhance ) const	{ return static_cast<int>( delay_cast_mod_per_enhance * 100.f ) * nEnhance; }
	
	// ===================
	// 공격력 증가(물리)
	int GetPowerByEnhance( int nEnhance ) const			{ return static_cast<int>( nEnhance * var[4] ); };
	// 공격력 증가(물리 1타)
	int GetPowerOneByEnhance( int nEnhance ) const		{ return static_cast<int>( nEnhance * var[5] ); };
	// 공격력 증가(물리 방향)
	int GetPowerDirectByEnhance( int nEnhance ) const	{ return static_cast<int>( nEnhance * var[5] ); };
	// 공격력 증가(마법)
	int GetMagicByEnhance( int nEnhance ) const			{ return static_cast<int>( nEnhance * var[2] ); };
	// 공격력 증가(마법 1타)
	int GetMagicOneByEnhance( int nEnhance ) const		{ return static_cast<int>( nEnhance * var[5] ); };
	// 공격력 증가(마법 방향)
	int GetMagicAreaByEnhance( int nEnhance ) const		{ return static_cast<int>( nEnhance * var[4] ); };
	
	// 흡수율 HP
	int GetAbsorptionHPByEnhance( int nEnhance ) const	{ return static_cast<int>( var[4] * 100.f ) * nEnhance; };
	// 흡수율 MP
	int GetAbsorptionMPByEnhance( int nEnhance ) const	{ return static_cast<int>( var[5] * 100.f ) * nEnhance; };
	// 마법 흡수율 HP
	int GetMagicAbsorptionHPByEnhance( int nEnhance ) const	{ return static_cast<int>( var[4] * 100.f ) * nEnhance; };
	// 마법 흡수율 MP
	int GetMagicAbsorptionMPByEnhance( int nEnhance ) const	{ return static_cast<int>( var[5] * 100.f ) * nEnhance; };

	// 공격력 증폭(물리)
	int GetIncreaseByEnhance( int nEnhance ) const	{ return static_cast<int>( var[4] * 100.f ) * nEnhance; };
	// 공격력 증폭(물리 1타)
	int GetIncreaseOneByEnhance( int nEnhance ) const	{ return static_cast<int>( var[5] * 100.f ) * nEnhance; };

	// 지속효과 성공률(물리 저주 계열)
	int GetStateSuccessByEnhance( int nEnhance ) const	{ return static_cast<int>( nEnhance * var[5] ); };
	// 지속효과 연타 공격력 강화
	int GetStatePowerOneByEnhance( int nEnhance ) const	{ return static_cast<int>( nEnhance * var[7] ); };
	// 지속효과 마법 공격력 강화
	int GetStateMagicPowerByEnhance( int nEnhance ) const	{ return static_cast<int>( nEnhance * var[2] ); };

	// 버프/디버프 성능
	int GetStatePowerByEnhance( int nEnhance ) const	{ return static_cast<int>( nEnhance * var[2] ); };

	// 획복량/회복률
	int GetRecoveryByEnhance( int nEnhance ) const { return static_cast<int>( nEnhance * var[6] ); };
	int GetRecoveryPerByEnhance( int nEnhance ) const { return static_cast<int>( var[7] * 100.f ) * nEnhance; };

	// 부활시 EXP 복구률
	int GetExpIncreaseByRevival( int nEnhance ) const	{ return static_cast<int>( var[3] * 100.f ) * nEnhance; };

	// HP 흡수량
	int GetAbsorptionHPPointByEnhance( int nEnhance ) const	{ return static_cast<int>( nEnhance * var[4] ); };
	// MP 흡수량
	int GetAbsorptionMPPointByEnhance( int nEnhance ) const	{ return static_cast<int>( nEnhance * var[5] ); };

	// 테이밍 성공률
	int GetTamingSuccessByEnhance( int nEnhance ) const	{ return static_cast<int>( var[1] * 100.f ) * nEnhance; };
	// 초당 mp 소모
	int GetCostMPPerSecondByEnhance( int nEnhance ) const { return static_cast<int>( nEnhance * var[2] ); };

	// ==============================================

	int				GetNeedStateId() const { return need_state_id; }
	unsigned char	GetNeedStateLevel() const { return need_state_level; }
	char			NeedStateExhaust() const { return need_state_exhaust; }


	int		GetNeedJobPoint( int skill_lv ) const
	{
		if( skill_lv > MAX_SKILL_LEVEL )
			skill_lv = MAX_SKILL_LEVEL;

		int jp = need_jp[ skill_lv - 1 ];
		if( jp < 0 ) jp = 0;

		return jp;
	}

	int		GetNeedTalentPoint( int skill_lv ) const
	{
		if( skill_lv > MAX_SKILL_LEVEL )
			skill_lv = MAX_SKILL_LEVEL;

		int tp = need_jp[ skill_lv - 1 ];
		if( tp > 0 ) tp = 0;

		return -tp;
	}

	void	SetNeedJobPoint( int skill_lv, int jp )
	{
		if( skill_lv > MAX_SKILL_LEVEL ) return;
		need_jp[ skill_lv - 1 ] = jp;
	}

	//사용가능/불가/강제 공격 처리 - 가져다 쓰는게 애매하다..
	char IsUsable( char nUseIndex ) const
	{
		switch( nUseIndex )
		{
		case USE_SELF    : return uf_self;     //자신
		case USE_ALLY	 : return uf_party;	   //파티원
		case USE_GUILD   : return uf_guild;	   //길드원
		case USE_NEUTRAL : return uf_neutral;  //중립
		case USE_PURPLE  : return uf_purple;   //보라
		case USE_ENEMY   : return uf_enemy;	   //적
		}
		return 0; //사용 원천 불가
	}

	// 효과 타입
	int		GetEffectType()	const { return effect_type; }
	int		GetEnhancedSkillId() const { return skill_enchant_link_id; }

	// 헤이트 비율
	int		GetHatePoint( int lv, int point, int enhance ) const
	{
		// -1 : 데미지와 무관하게 일정값이 적용 ([헤이트], [SLv:헤이트] 수치로 적용)
		// 0 : 헤이트 부가 안함
		// 0 : 데미지, 힐량과 비례하여 값을 비율로 적용 (부가할 헤이트 = 데미지 * [헤이트비율]) (옆의 두 값은 무시한다)
		if( hate_mod == 0 ) return 0;

		if( hate_mod < 0 ) return (int)( hate_basic + lv * hate_slv + enhance * hate_enhance );

		return (int)( hate_basic + point * hate_mod );
	}
	
protected:

	int			uid;					// id
	int			name_id;

	char		is_valid;				// 구현 여부

	char		elemental;				// 무속성 0	화속성 1 수속성 2 풍속성 3 토속성 4 명속성 5 암속성 6


	char		is_active;				// 액티브 스킬이면 1
	char		is_spell_act;			// 전투기술이면 0, 마법이면 1
	char		is_harmful;				// 피해를 주는 스킬이면 1
	char		is_need_target;			// 셀프0 타겟1 지면2
	char		is_corpse;				// 시체에게 사용가능?
	char		is_toggle;				// 토글형 스킬?

	char		cancel_type;			// 캐스팅 캔슬 타입
	char		cancel_level;			// 캐스팅 캔슬 위험도
	
	int			toggle_group;			// 토글 그룹

	int			cast_range;				// 캐스트 가능 거리
	int			valid_range;			// 유효 적용 거리, 캐스트 시전중 타겟과의 거리가 valid_range 보다 커지면, 시전 취소

	int			cost_exp;
	int			cost_exp_per_enhance;
	int			cost_jp;
	int			cost_jp_per_enhance;
	int			cost_hp;
	int			cost_hp_per_skl;
	int			cost_mp;				// mp
	int			cost_mp_per_skl;		// 스킬레벨당 mp 
	int			cost_mp_per_enhance;	// 스킬레벨당 mp 
	float		cost_hp_per;			// 소모 hp(퍼센트)
	float		cost_hp_per_skl_per;	// 스킬 렙당 소모 hp(퍼센트)
	float		cost_mp_per;			// 소모 mp(퍼센트)
	float		cost_mp_per_skl_per;	// 스킬 렙당 소모 mp(퍼센트)
	c_fixed10	cost_energy;
	c_fixed10	cost_energy_per_skl;
	int			cost_item;
	int			cost_item_count;	
	int			cost_item_count_per_skl;	

	int			need_level;
	int			need_hp;
	int			need_mp;
	int			need_state_id;
	unsigned char		need_state_level;
	char		need_state_exhaust;

	char		vf_one_hand_sword;
	char		vf_two_hand_sword;
	char		vf_double_sword;
	char		vf_dagger;
	char		vf_double_dagger;
	char		vf_spear;
	char		vf_axe;
	char		vf_one_hand_axe;
	char		vf_double_axe;
	char		vf_one_hand_mace;
	char		vf_two_hand_mace;
	char		vf_lightbow;
	char		vf_heavybow;
	char		vf_crossbow;
	char		vf_one_hand_staff;
	char		vf_two_hand_staff;
	char		vf_shield_only;
	char		vf_is_not_need_weapon;

	float		delay_cast;
	float		delay_cast_per_skl;
	float		delay_cast_mod_per_enhance;
	float		delay_common;
	float		delay_cooltime;
	float		delay_cooltime_per_skl;
	float		delay_cooltime_mod_per_enhance;

	// 자신	파티원	길드원	중립	보라	적	아바타	소환수	몬스터

	int			cool_time_group_id;
	char		uf_self;
	char		uf_party;
	char		uf_guild;
	char		uf_neutral;
	char		uf_purple;
	char		uf_enemy;

	char		tf_avatar;
	char		tf_summon;
	char		tf_monster;
	char		skill_lvup_limit;

	short		target;
	short		effect_type;	// 효과 타임
	int			skill_enchant_link_id; // 강화 스킬 id

	int			state_id;
	int			state_level_base;
	float		state_level_per_skl;
	float		state_level_per_enhance;
	AR_TIME		state_second;
	AR_TIME		state_second_per_skl;
	AR_TIME		state_second_per_enhance;
	int			probability_on_hit;
	int			probability_inc_by_slv;

	short		hit_bonus;
	short		hit_bonus_enhace;
	short		percentage;

	float		hate_mod;
	int			hate_basic;
	float		hate_slv;
	float		hate_enhance;

	int			critical_bonus;			// 크리율 증가
	int			critical_bonus_per_skl;	// 레벨당 크리율 증가

	c_fixed10	var[MAX_SKILL_VALUE];

	int			need_jp[ MAX_SKILL_LEVEL ];

	char		effect_flag;

	char		is_projectile;
	float		projectile_speed;
	float		projectile_acceleration;
};


struct SkillTree
{
	int skill_tree_id			;	// 스킬 트리 아이디
	int skill_id				;	// 스킬아이디
	int skill_group_id			;	// SummonRandomSkillResource SID

	int min_skill_lv			;	// 필요 스킬 조건이 적용될 스킬 최소 레벨(전직 후 중첩 조건 발생 방지)
	int max_skill_lv			;	// 스킬 맥스 레벨
	int lv						;	// 스킬 학습 레벨
	int job_lv					;	// 스킬 학습 직업 레벨
	float	jp_ratio			;	// 스킬 JP 소모 비율
	int need_skill_id[3]		;	// 필요 스킬 아이디
	int need_skill_lv[3]		;	// 필요 스킬 레벨
	int min_enhance				;	// 필요 강화 최소치
	int max_enhance				;	// 필요 강화 최대치
};

#pragma pack( 1 )

// 스킬 사용 결과
struct SkillResult
{
	enum
	{
		// type
		DAMAGE					= 0,
		MAGIC_DAMAGE			= 1,
		DAMAGE_WITH_KNOCK_BACK	= 2,
		RESULT					= 10,
		ADD_HP					= 20,			// HP가 소모될 때도 사용
		ADD_MP					= 21,			// MP가 소모될 때도 사용
		ADD_HP_MP_SP			= 22,
		REBIRTH					= 23,
		RUSH					= 30,
		CHAIN_DAMAGE			= 40,
		CHAIN_MAGIC_DAMAGE		= 41,
		CHAIN_HEAL				= 42,
		NOT_USE					= 100,
	};	

	enum
	{
		// ResultType의 SuccessType
		ADD_STATE		= 10,
		REMOVE_STATE	= 11,
		ADD_HATE		= 12,
		TURN_ON			= 21,
		TURN_OFF		= 22,
		SUMMON_DEAD		= 30,
		TARGET_DEAD		= 31,
		CREATE_ITEM		= 40,
		RESPAWN_MONSTER	= 41,
	};

	enum
	{
		// DamageType의 flag
		CRITICAL	= ( 1 << 0 ),
		MISS		= ( 1 << 1 ),
		BLOCK		= ( 1 << 2 ),
		PERFECT_BLOCK = ( 1 << 3 ),
	};

	int			GetType() const			{ return damage.type; }
	AR_HANDLE	GetTargetHandle() const	{ return damage.hTarget; }
	int			GetDamage() const		{ return damage.damage; }

	struct DamageType
	{
		unsigned char type;

		AR_HANDLE	hTarget;
		int			target_hp;

		char		damage_type;
		int			damage;
		int			flag;
		unsigned short elemental_damage[ Elemental::TYPE_COUNT ];
	};

	struct DamageWithKnockBackType
	{
		unsigned char type;

		AR_HANDLE	hTarget;
		int			target_hp;

		char		damage_type;
		int			damage;
		int			flag;
		unsigned short elemental_damage[ Elemental::TYPE_COUNT ];

		AR_UNIT			x, y;
		unsigned char	speed;
		AR_TIME			knock_back_time;
	};

	struct ResultType
	{
		unsigned char type;

		AR_HANDLE	hTarget;
		//int			target_hp;

		bool		bResult;
		int			success_type;
	};

	struct AddHPType	// Add MP 도 동일하게 사용
	{
		unsigned char type;

		AR_HANDLE	hTarget;
		int			target_hp;

		int			nIncHP;
	};

	struct AddHPMPSPType
	{
		unsigned char type;

		AR_HANDLE	hTarget;
		int			target_hp;

		int			nIncHP;
		int			nIncMP;
		int			nIncSP;

		int			target_mp;
	};

	struct RushType
	{
		unsigned char type;

		AR_HANDLE	hTarget;
		//int			target_hp;

		bool		bResult;

		AR_UNIT		x, y;
		float		face;
		unsigned char	speed;
	};

	struct RebirthType
	{
		unsigned char type;

		AR_HANDLE	hTarget;
		int			target_hp;

		int			nIncHP;
		int			nIncMP;
		int			nRecoveryEXP;

		short		target_mp;
	};

	struct ChainDamageType
	{
		unsigned char type;

		AR_HANDLE	hTarget;
		int			target_hp;

		char		damage_type;
		int			damage;
		int			flag;
		unsigned short elemental_damage[ Elemental::TYPE_COUNT ];

		AR_HANDLE	hFrom;
	};

	struct ChainHealType
	{
		unsigned char type;

		AR_HANDLE	hTarget;
		int			target_hp;

		int			nIncHP;

		AR_HANDLE	hFrom;
	};

	// 서버에서 공용으로 사용하기 위한 데이터
	union
	{
		DamageType				damage;
		DamageWithKnockBackType	damage_kb;
		ResultType				result;
		AddHPType				add_hp;
		AddHPMPSPType			add_hp_mp_sp;
		RushType				rush;
		RebirthType				rebirth;
		ChainDamageType			chain_damage;
		ChainHealType			chain_heal;
	};

	SkillResult()
	{	
		memset( this, 0, sizeof( *this ) );
	}
};

struct ATTACK_INFO
{
	short			damage;					// -1 이면 MISS, -2 면 failed, -3이면 너무 멀리 떨어져 있음, -4면 너무 빨리 공격했음 (스핵?)
	unsigned char	flag;					
	unsigned short	elemental_damage[ Elemental::TYPE_COUNT ];	// 추가데미지
	unsigned char	hp_percentage;			// HP %

	ATTACK_INFO()
	{
		damage        = 0;        // -1 이면 MISS, -2 면 failed, -3이면 너무 멀리 떨어져 있음, -4면 너무 빨리 공격했음 (스핵?)
		flag          = 0;
		memset( elemental_damage, 0, sizeof( elemental_damage ) );
		hp_percentage = 0;
	}
};

#pragma pack()