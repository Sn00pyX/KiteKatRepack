#pragma once

// 여러가지 상수들.




// * 생물 계열
//   모든 생물(플레이어/몬스터/소환수) 는 이중 하나의 계열이다.
//
enum	CREATURE_TYPE
{
	CREATURE_NONE			= -1,	// 설정되지 않음
	CREATURE_ALL			= 99,	// 모두
	CREATURE_ETC			= 0,	// 기타
	CREATURE_BEAST			= 1,	// 야수
	CREATURE_SEMIHUMAN		= 2,	// 아인
	CREATURE_ELEMENTAL		= 3,	// 정령
	CREATURE_ANGEL			= 4,	// 천사
	CREATURE_DEVIL			= 5,	// 악마
	CREATURE_MECHA			= 6,	// 메카
	CREATURE_DRAGON			= 7,	// 드래곤
	CREATURE_UNDEAD			= 8,	// 언데드
	CREATURE_HUMAN			= 9,	// 인간
	MAX_CREATURE_TYPE_NUMBER
};




// * 종족
const int		RACE_DEVA				= 91;	// 데바
const int		RACE_ASURA				= 92;	// 아수라

const int		RACE_ANT				= 11;	// 앤트
const int		RACE_CHICKEN			= 12;	// 닭
const int		RACE_STONE_TURTLE		= 13;	// 바위거북이

const int		RACE_SIREN				= 32;	// 사이렌

const int		RACE_WHITE_DRAGON		= 73;	// 화이트드래곤

const int		RACE_GOBLIN_SKELECTON	= 81;	// 고블린 스켈렉톤

// * 직업
const int		JOB_HUNTER				= 0;	// -_-

// 속성
namespace Elemental
{
	enum Type
	{	
		TYPE_NONE		= 0,		// 무속성
		TYPE_FIRE		= 1,		// 화속성
		TYPE_WATER		= 2,		// 수속성
		TYPE_WIND		= 3,		// 풍속성
		TYPE_EARTH		= 4,		// 토속성
		TYPE_LIGHT		= 5,		// 명속성
		TYPE_DARK		= 6,		// 암속성			

		TYPE_COUNT					// 갯수
	};
};
