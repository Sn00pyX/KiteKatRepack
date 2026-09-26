#pragma once

#include "GameType.h"


#pragma pack( 1 )
#ifdef USE_CLIENT97

struct CreatureStat
{
	 short		stat_id;
	 short		strength;
	 short		vital;
	 short		agility;
	 short		dexterity;
	 short		intelligence;
	 short		mentality;
	 short		luck;
};

#else
struct CreatureStat
{
	short		stat_id;
	short		strength;
	short		vital;
	short		dexterity;
	short		agility;
	short		intelligence;
	short		mentality;
	short		luck;
};


#endif

#ifdef USE_CLIENT97

struct CreatureAttribute
{
	 short		nCritical;					// 크리티컬
	 short		nCriticalPower;				// 크리티컬 비율	( % 단위 )
	 int		nAttackPointRight;			// 공격력(오른손. 메인)
	 int		nAttackPointLeft;			// 공격력(왼손. 보조)
	 int		nDefence;					// 방어력
	 int		nBlockDefence;				// 블록 방어력
	 int		nMagicPoint;				// 마력
	 int		nMagicDefence;				// 마법방어력
	 int		nMaxWeight;					// 소지무게
	 short		nAccuracyRight;				// 명중(오른손. 메인)
	 short		nMagicAccuracy;				// 마명중
	 short		nAvoid;						// 회피
	 short		nMagicAvoid;				// 마법저항력
	 short		nBlockChance;				// 블럭율

	 short		nMoveSpeed;					// 이동속도
	 short		nCoolTimeSpeed;				// 재시전속도	 
	 short		nAttackSpeed;				// 공격속도
	 short		nAttackRange;				// 사거리
	 short		nCastingSpeed;				// 시전속도
	 short		nItemChance;				// 아이템 드랍율
	 short		nAccuracyLeft;				// 명중(왼손. 보조)	 
	 short		nHPRegenPercentage;			// HP 리젠%
	 short		nHPRegenPoint;				// HP 리젠P
	 short		nMPRegenPercentage;			// MP 리젠%
	 short		nMPRegenPoint;				// MP 리젠P
	 short		nPerfectBlock;				// 퍼펙트 블럭%
	 short		nMagicalDefIgnore;			// 마법 방어력 무시P
	 short		nMagicalDefIgnoreRatio;		// 마법 방어력 무시%
	 short		nPhysicalDefIgnore;			// 물리 방어력 무시P
	 short		nPhysicalDefIgnoreRatio;	// 물리 방어력 무시%
//	 short		nMagicalPenetration;		// 마법 관통력P
//	 short		nMagicalPenetrationRatio;	// 마법 관통력%
//	 short		nPhysicalPenetration;		// 물리 관통력P
//	 short		nPhysicalPenetrationRatio;	// 물리 관통력%
}; 
#else

struct CreatureAttribute
{
	short		nCritical;					// 크리티컬
	short		nCriticalPower;				// 크리티컬 비율	( % 단위 )
	int		nAttackPointRight;			// 공격력(오른손. 메인)
	int		nAttackPointLeft;			// 공격력(왼손. 보조)
	int		nDefence;					// 방어력
	int		nBlockDefence;				// 블록 방어력
	int		nMagicPoint;				// 마력
	int		nMagicDefence;				// 마법방어력
	short		nAccuracyRight;				// 명중(오른손. 메인)
	short		nAccuracyLeft;				// 명중(왼손. 보조)
	short		nMagicAccuracy;				// 마명중
	short		nAvoid;						// 회피
	short		nMagicAvoid;				// 마법저항력

	short		nBlockChance;				// 블럭율
	short		nMoveSpeed;					// 이동속도
	short		nAttackSpeed;				// 공격속도
	short		nAttackRange;				// 사거리
	int		nMaxWeight;					// 소지무게
	short		nCastingSpeed;				// 시전속도
	short		nCoolTimeSpeed;				// 재시전속도	 
	short		nItemChance;				// 아이템 드랍율
	short		nHPRegenPercentage;			// HP 리젠%
	short		nHPRegenPoint;				// HP 리젠P
	short		nMPRegenPercentage;			// MP 리젠%
	short		nMPRegenPoint;				// MP 리젠P
	short		nPerfectBlock;				// 퍼펙트 블럭%
	short		nMagicalDefIgnore;			// 마법 방어력 무시P
	short		nMagicalDefIgnoreRatio;		// 마법 방어력 무시%
	short		nPhysicalDefIgnore;			// 물리 방어력 무시P
	short		nPhysicalDefIgnoreRatio;	// 물리 방어력 무시%
	short		nMagicalPenetration;		// 마법 관통력P
	short		nMagicalPenetrationRatio;	// 마법 관통력%
	short		nPhysicalPenetration;		// 물리 관통력P
	short		nPhysicalPenetrationRatio;	// 물리 관통력%
};
#endif
struct CreatureElementalResist
{
	const CreatureElementalResist& operator+= ( const CreatureElementalResist& rhs )
	{
		for( int i = 0; i < Elemental::TYPE_COUNT; i++ )
		{
			nResist[ i ] += rhs.nResist[ i ];
		}

		return *this;
	}

	short		nResist[ Elemental::TYPE_COUNT ];
};

#pragma pack()