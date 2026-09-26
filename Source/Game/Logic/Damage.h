#pragma once

#include "StructCreature.h"

// DamageMessage를 인자로 데미지를 주도록 바꾸는 방식도 고려했지만
// 당장에 모든 인터페이스를 수정/검증하기 어려운 관계로 이는 추후로 미룬다.
struct DamageMessage
{
	DamageMessage( StructCreature* from, StructCreature* target, const StructCreature::DamageType type, const int ignoreFlag,
	               const Elemental::Type elementalType, const int initialDamage );

	void init( const StructCreature::DamageType type, const int ignoreFlag );

	enum DamageType
	{
		DIRECT_DAMAGE     = 0, // 직접 데미지
		STATE_DAMAGE      = 1, // 지속 효과 데미지
		ADDITIONAL_DAMAGE = 2, // 추가 데미지
		NAKED_DAMAGE      = 3, // 방어력 관련 공식 적용 없는 데미지
	};

	enum AttackType
	{
		PHYSICAL_ATTACK           = 0,
		MAGICAL_ATTACK            = 1,
		PHYSICAL_SKILL_ATTACK     = 2,
		PHYSICAL_LEFT_HAND_ATTACK = 3,
	};

	enum
	{
		DEFENCE           = ( 1 << 0 ), // 방어력 적용 여부
		CRITICAL          = ( 1 << 1 ), // 크리 가능 여부
		BLOCK             = ( 1 << 2 ), // 블럭 가능 여부
		AVOID             = ( 1 << 3 ), // 회피 가능 여부
		RANDOM            = ( 1 << 4 ), // 랜덤 데미지 적용 여부 (5%)
		DAMAGE_REDUCE     = ( 1 << 5 ), // 데미지 리듀스 적용 여부
		DOUBLE			  = ( 1 << 6 ),
		FLAG_COUNT        = 7,
		FLAG_MASK         = ( DEFENCE | CRITICAL | BLOCK | AVOID | RANDOM | DAMAGE_REDUCE | DOUBLE),
	};

	StructCreature*                   from;
	StructCreature*                   target;	
	Elemental::Type                   elementalType;
	int                               flag;
	int                               damage;
	DamageType                        damageType;
	AttackType                        attackType;
	
	StructCreature::DamageType        oldDamageType;
	// 기존 가상 함수 인터페이스와의 호환을 위해 일시적으로 보관
	// 추후 StructCreature::DamageType을 적절하게 대체할 수 있으면 함

	const StructCreature::StateMod*   advantage;
	const StructCreature::StateMod*   penalty;

	int                               criticalBonus;
	int                               accuracyBonus;
};


struct SimulationResult
{
	SimulationResult()
	{
		critical = false;
		missed = false;
		blocked = false;
		perfectBlocked = false;
		doubleattack = false;
	}

	bool critical;
	bool missed;
	bool blocked;
	bool perfectBlocked;
	bool doubleattack;
};

struct CalculationResult
{
	int damage;
	int penetratedDamage;
	int resistedDamage;
	int finalDamage;
	int manaShieldAbsorption;

	SimulationResult simulation;

	CalculationResult()
	{
		damage = 0;
		penetratedDamage = 0;
		resistedDamage = 0;
		finalDamage = 0;

		manaShieldAbsorption = 0;
	}

	template < typename ModifierType >
	void apply( const ModifierType& modifier, const bool applyPenetration )
	{
		damage = modifier.getDamage( damage );

		if ( applyPenetration == true )
		{
			penetratedDamage = modifier.getPenetratedDamage( penetratedDamage );
		}
		else
		{
			penetratedDamage = modifier.getDamage( penetratedDamage );
		}
	}
};


struct DamageModifier
{
	DamageModifier() : inc(0), amp(1.0f) {}

	const int getDamage( const int damage ) const
	{
		return ( damage + inc ) * amp;
	}

	const int getPenetratedDamage( const int penetratedDamage ) const
	{
		// 만약 데미지 증가인 경우라면 관통 데미지도 증가, 그 외에는 그대로 유지
		return ( penetratedDamage + std::max( inc, 0 ) ) * std::max( amp, 1.0f );
	}

	int inc;
	float amp;
};

struct DamageAmplifier
{
	DamageAmplifier() : amp(1.0f) {}

	const int getDamage( const int damage ) const
	{
		return damage * amp;
	}

	const int getPenetratedDamage( const int penetratedDamage ) const
	{
		// 만약 데미지 증가인 경우라면 관통 데미지도 증가, 그 외에는 그대로 유지
		return penetratedDamage * std::max( amp, 1.0f );
	}

	float amp;
};

struct DamageCalculator
{
public:
	DamageMessage::DamageType damageType;
	int originalDamage;
	int flag;

	int targetLevel;
	int attackerLevel;

	bool isImmune;
	bool isIgnoreLevelPenalty;

	int accuracyInc;
	int accuracy;
	int avoid;

	int critRateInc;
	float critRateAmp;		
	int critPower;
	int critical;

	int blockRate;
	int perfectBlockRate;	

	DamageModifier beforeDefence;
	DamageModifier afterDefence;
	DamageModifier statePenalty;
	DamageModifier stateAdvantage;
	DamageAmplifier expertisePenalty;
	DamageAmplifier expertiseAdvantage;
	DamageAmplifier random;
	DamageAmplifier rule;
	DamageAmplifier resist;

	int attackPoint;
	int defence;
	int blockDefence;
	int defenceIgnore;
	float defenceIgnoreRatio;

	int penetration;
	float penetrationRatio;

	float manaShieldRatio;
	int manaShieldLimit;

	DamageCalculator();
	DamageCalculator( const DamageMessage& message );

	void Initiate();
	void Interpret( const DamageMessage& message );
	void Calculate( CalculationResult& result );

	void SimulateDamageCalculation( SimulationResult& result );
	void CalculateActualDamage( CalculationResult& result ) const;

private:	
	const bool SimulateAndCheckImmunity( SimulationResult& result );
	const bool SimulateAndCheckAvoidance( SimulationResult& result );
	const bool SimulateAndCheckBlocking( SimulationResult& result );
	void SimulateCritical( SimulationResult& result );
	void SimulateDouble( SimulationResult& result );
	void SimulateRandomDamage( SimulationResult& result );
	static const int CalculateHitRate( const int levelDiff, const float accuracy, const float avoid, const int bonus );
	static const int CalculateCritRate( const int critRate, const float amplifier, const int bonus );
	
	void CalculateDefenceAppliedDamage( CalculationResult& result ) const;
	void CalculateCriticalDamage( CalculationResult& result ) const;
	void CalculateResistance( CalculationResult& result ) const;
	void CalculateLaterDamageModifier( CalculationResult& result ) const;
	void CalculateManaShieldEffect( CalculationResult& result ) const;
	void CalculatePenetrationEffect( CalculationResult& result ) const;

	static const int CalculateDirectDamageWithDefence( const int damage, const int defence, const int attackerLevel );
	static const int CalculateStateDamageWithDefence( const float damage, const int defence, const int attackPoint );
};