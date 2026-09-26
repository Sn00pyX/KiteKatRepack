#pragma once

#include <toolkit/c_fixed.h>


namespace ITEM_EFFECT_INSTANT
{
	enum {
		NONE					= 0,	// 없음

		INC_HP					= 1,	// HP 회복	현재 HP를 [+X] 만큼 회복 시킨다.
		INC_MP					= 2,	// MP 회복	현재 MP를 [+X] 만큼 회복 시킨다.
		WARP					= 3,	// 캐릭터 이동_귀환	캐릭터를세이브 포인트로 이동시킨다.
		RESURECTION				= 4,	// 부활
		SKILL					= 5,	// 스킬 호출
		ADD_STATE				= 6,	// 지속효과 호출
		REMOVE_STATE			= 7,	// 지속효과 제거
		TOGGLE_STATE			= 8,
		ADD_IMMORAL_POINT		= 41,
		SET_IMMORAL_POINT		= 42,
		WARP_TO_SPECIAL_POSITION	= 43,
		INC_STAMINA				= 80,	// 스태미너 증가
		SUMMON_PET				= 90,	// 펫 소환/해제
		GENERATE_ITEM			= 94,
		INC_HP_PERCENT			= 101,	// HP 회복	현재 HP를 대상의 최대 HP의 [X%]만큼 회복 시킨다.
		INC_MP_PERCENT			= 102,	// MP 회복	현재 HP를 대상의 최대 MP의 [X%]만큼 회복 시킨다.
		INC_GOLD				= 103,	// 루피 증가(최소/최대값 사이 랜덤)
		INC_HUNTAHOLIC_POINT	= 104,	// 헌터홀릭 포인트 증가(최소/최대값 사이 랜덤)
		RECALL					= 112,	// 소환 깃털(파티원 1명/전체 자신 위치로 소환)
		RESET_SKILL				= 113,	// 스킬 초기화
		RESET_JOB				= 118,	// 직업 초기화
		RENAME_SUMMON			= 115,	// 크리처 이름 붙이기
		RESET_SUMMON_SKILL		= 116,	// 소환수용 스킬 초기화
		FORCE_RESET_SUMMON_SKILL= 125,	// 소환수용 스킬 초기화 (랜덤스킬까지 같이 초기화)
		WARP_TO_PLAYER			= 117,	// 유저에게 워프
		RENAME_CHARACTER		= 119,	// 캐릭터 이름 변경
		RENAME_PET				= 120,	// 펫 이름 변경
		ADD_CASH				= 121,	// 캐시 증가
		HAIR_DYEING				= 122,	// 헤어 염색
		SET_HAIR_STYLE			= 123,	// 헤어 스타일 변경
		ADD_STATE_EX			= 124,	// 확장 지속효과 호출 - 값1/2에 지정된 ID와 레벨에 해당하는 지속효과를 아이템 DB 컬럼에 지정된 지속시간만큼 걸기(하나의 아이템으로 지속효과 여러 개 걸기)
		SET_SKIN_COLOR			= 132,	// 피부 색상 변경
		CAST_WORLD_STATE		= 127,	// 월드 버프 부여
		CAST_PARTY_STATE		= 134,	// 파티원 버프 부여
		CAST_GUILD_STATE		= 135,	// 길드원 버프 부여
		CREATURE_INC_HP			= 140,
		CREATURE_INC_MP			= 141,

	};
};


// * 아이템 성능
namespace ITEM_EFFECT_CHARM
{
	enum {
		NONE					= 0,	// 없음

		INC_MAX_STAMINA			= 81,	// 최대 스태미너 증가
		STAMINA_REGEN_TENT		= 82,	// 스테미너 리젠률 120%로 고정(텐트 효과)
		INC_RIDING_SPEED		= 83,	// 크리처 탑승시 이속 증가
		INC_SPEED				= 84,	// 이속 증가
		INC_STAMINA_REGEN		= 85,	// 스테미너 리젠률 증가
		INC_MAX_WEIGHT			= 86,	// 최대 소지량 증가
		INC_GAIA_MEMBERSHIP		= 89,	// 가이아 상인 연합권
		FAIRY_POTION			= 114,	// 요정의 병
		AUTO_RECOVER_HP			= 125,	// HP 자동 회복
		AUTO_RECOVER_MP			= 126,	// MP 자동 회복
	};
};

namespace ITEM_EFFECT_PASSIVE
{
	enum {
		NONE = 0,	// 없음

		ATTACK_POINT = 11,	// 공격력 조정
		MAGIC_POINT = 12,	// 마력 조정
		ACCURACY = 13,	// 정확도 조정
		ATTACK_SPEED = 14,	// 공속 조정
		DEFENCE = 15,	// 방어력 조정
		MAGIC_DEFENCE = 16,	// 마법 방어력 조정
		AVOID = 17,	// 회피율 조정
		MOVE_SPEED = 18,	// 이동속도 조정
		BLOCK_CHANGE = 19,	// 블럭율 조정
		CARRY_WEIGHT = 20,	// 소지량 조정
		BLOCK_DEFENCE = 21,	// 블록방어력 조정
		CASTING_SPEED = 22,
		MAGIC_ACCURACY = 23,
		MAGIC_AVOID = 24,
		COOLTIME_SPEED = 25,
		BELT_SLOT = 26,
		MAX_CHAOS = 27,
		MAX_HP = 30,
		MAX_MP = 31,
		BOW_INTERVAL = 34,
		MP_REGEN_POINT = 33,
		TAMED_ITEM = 95,
		INC_PARAMETER_A = 96,
		INC_PARAMETER_B = 97,
		AMP_PARAMETER_A = 98,
		AMP_PARAMETER_B = 99,
		INC_SOCKET_COUNT_A = 130,
		INC_SOCKET_COUNT_B = 131,
		APPLY_EFFECT = 133,
		INC_DOUBLE_ATTACK = 140,
		INC_COOLTIME_SPEED = 141,
		INC_CASTING_SPEED = 142,
		RESIST_NEGATIVE = 143,
		INC_SUMMON_FCM = 144,
		INC_BELTSLOT_AMP = 145,
	};
};

struct EffectInfo
{
	enum
	{
		NUMBER_OF_VALUE			= 20	// fValue 배열 원소 수
	};

	EffectInfo()
		: nOrdinalID( 0 )
		, nMinLevel( 0 )
		, nMaxLevel( 0 )
		, eType( EFFECT_TYPE_UNKNOWN )
		, nEffectID( 0 )
		, nEffectLevel( 0 )
	{
		for( int i = 0 ; i < NUMBER_OF_VALUE ; ++i )
			fValue[ i ].set( 0 );
	}
	EffectInfo( const EffectInfo & rhs )
		: nOrdinalID( rhs.nOrdinalID )
		, nMinLevel( rhs.nMinLevel )
		, nMaxLevel( rhs.nMaxLevel )
		, eType( rhs.eType )
		, nEffectID( rhs.nEffectID )
		, nEffectLevel( rhs.nEffectLevel )
	{
		for( int i = 0 ; i < NUMBER_OF_VALUE ; ++i )
			fValue[ i ].set( rhs.fValue[ i ].get() );
	}

	enum _EFFECT_TYPE
	{
		EFFECT_TYPE_UNKNOWN		= 0,	// 기본값(기능 없음)

		EFFECT_TYPE_BASIC		= 1,	// 아이템: 기본 성능
		EFFECT_TYPE_OPTIONAL	= 2,	// 아이템: 추가 성능
		EFFECT_TYPE_STATE		= 3,	// 지속효과 성능
		EFFECT_TYPE_ENHANCE		= 4,	// 아이템: 강화 성능
	};

	// ID는 EffectInfo *의 벡터가 하나만 가지고 있으면 됨
	int				nOrdinalID;			// 리로딩 또는 중첩 로딩 체크를 위해 필요함

	int				nMinLevel;
	int				nMaxLevel;
	_EFFECT_TYPE	eType;
	short			nEffectID;
	unsigned short	nEffectLevel;
	c_fixed10		fValue[ NUMBER_OF_VALUE ];
};

struct EnhanceEffectInfo
{
	enum
	{
		NUMBER_OF_VALUE_ENHANCE			= 25	// fValue 배열 원소 수
	};

	EnhanceEffectInfo()
		: nID( 0 )
		, nSubID( 0 )
		, nEnhanceEffectID( 0 )
	{
		for( int i = 0 ; i < NUMBER_OF_VALUE_ENHANCE ; ++i )
			fValue[ i ].set( 0 );
	}

	int					nID;
	short				nSubID;		// 사실 subID가 꼭 필요하지는 않으나 기획팀과 데이터 일관성을 유지하기 위해 보관
	short				nEnhanceEffectID;
	c_fixed10			fValue[NUMBER_OF_VALUE_ENHANCE];
};