#include "Damage.h"

#include "StructCreature.h"

extern volatile int g_bIgnoreRandomDamage;

DamageMessage::DamageMessage( StructCreature* from, StructCreature* target, const StructCreature::DamageType type,
                              const int ignoreFlag, const Elemental::Type elementalType, const int initialDamage )
: from( from ), target( target ), flag( 0 ), elementalType( elementalType ), damage( initialDamage > 0 ? initialDamage : 0 ),
  oldDamageType( type ), advantage( NULL ), penalty( NULL ), criticalBonus( 0 ), accuracyBonus( 0 )
{
	init( type, ignoreFlag );	
}

void DamageMessage::init( const StructCreature::DamageType type, const int ignoreFlag )
{
	// 이하 flag의 비트 연산을 위해 일시적으로 선언한 상수들
	enum
	{
		DAMAGE_TYPE_SHIFT = FLAG_COUNT,
		DAMAGE_TYPE_MASK  = ( 0x3               << FLAG_COUNT ),
		DAMAGE_TYPE_BIT   = 2,

		DIRECT            = ( DIRECT_DAMAGE     << DAMAGE_TYPE_SHIFT ),
		STATE             = ( STATE_DAMAGE      << DAMAGE_TYPE_SHIFT ),
		ADDITIONAL        = ( ADDITIONAL_DAMAGE << DAMAGE_TYPE_SHIFT ),
	};

	enum
	{
		ATTACK_TYPE_SHIFT  = FLAG_COUNT + DAMAGE_TYPE_BIT,
		ATTACK_TYPE_MASK   = ( 0x3                       << ATTACK_TYPE_SHIFT ),

		PHYSICAL           = ( PHYSICAL_ATTACK           << ATTACK_TYPE_SHIFT ),
		MAGICAL            = ( MAGICAL_ATTACK            << ATTACK_TYPE_SHIFT ),
		PHYSICAL_SKILL     = ( PHYSICAL_SKILL_ATTACK     << ATTACK_TYPE_SHIFT ),
		PHYSICAL_LEFT_HAND = ( PHYSICAL_LEFT_HAND_ATTACK << ATTACK_TYPE_SHIFT ),		
	};

	const static int FlagMap[9] =
	{
		DEFENCE | CRITICAL | BLOCK | AVOID | RANDOM | DIRECT     | PHYSICAL          , // DT_NORMAL_PHYSICAL_DAMAGE
		DEFENCE | CRITICAL         | AVOID | RANDOM | DIRECT     | MAGICAL           , // DT_NORMAL_MAGICAL_DAMAGE
		DEFENCE | CRITICAL | BLOCK | AVOID | RANDOM | DIRECT     | PHYSICAL_SKILL    , // DT_NORMAL_PHYSICAL_SKILL_DAMAGE
		DEFENCE                                     | ADDITIONAL | PHYSICAL          , // DT_ADDITIONAL_DAMAGE
		DEFENCE | CRITICAL | BLOCK | AVOID | RANDOM | DIRECT     | PHYSICAL_LEFT_HAND, // DT_NORMAL_PHYSICAL_LEFT_HAND_DAMAGE
		DEFENCE                                     | ADDITIONAL | PHYSICAL_LEFT_HAND, // DT_ADDITIONAL_LEFT_HAND_DAMAGE
		DEFENCE                                     | ADDITIONAL | MAGICAL           , // DT_ADDITIONAL_MAGICAL_DAMAGE
		DEFENCE | CRITICAL                          | STATE      | MAGICAL           , // DT_STATE_MAGICAL_DAMAGE
		DEFENCE | CRITICAL                          | STATE      | PHYSICAL          , // DT_STATE_PHYSICAL_DAMAGE
	};

	flag = FlagMap[type];

	if ( ignoreFlag & StructCreature::IGNORE_AVOID )
	{
		flag &= ~AVOID;
	}

	if ( ignoreFlag & StructCreature::IGNORE_DEFENCE )
	{
		flag &= ~DEFENCE;
	}

	if ( ignoreFlag & StructCreature::IGNORE_BLOCK )
	{
		flag &= ~BLOCK;
	}

	if ( ignoreFlag & StructCreature::IGNORE_CRITICAL )
	{
		flag &= ~CRITICAL;
	}

	if (ignoreFlag & StructCreature::FORCE_DOUBLE)
	{
		flag |= DOUBLE;
	}

	attackType = static_cast<AttackType>( ( flag & ATTACK_TYPE_MASK ) >> ATTACK_TYPE_SHIFT );
	damageType = static_cast<DamageType>( ( flag & DAMAGE_TYPE_MASK ) >> DAMAGE_TYPE_SHIFT );
	flag &= FLAG_MASK;
}





DamageCalculator::DamageCalculator()
{
	Initiate();
}

DamageCalculator::DamageCalculator( const DamageMessage& message )
{
	Initiate();
	Interpret( message );
}

void DamageCalculator::Initiate()
{
	originalDamage = 0;
	flag = 0;

	isImmune = false;
	isIgnoreLevelPenalty = false;

	accuracyInc = 0;
	accuracy = 0;
	avoid = 0;
	targetLevel = 0;

	critical = 0;
	critRateInc = 0;
	critRateAmp = 1.0f;
	critPower = 0;

	blockRate = 0;
	perfectBlockRate = 0;

	damageType = DamageMessage::NAKED_DAMAGE;
	
	attackerLevel = 0;	
	attackPoint = 0;
	defence = 0;
	blockDefence = 0;

	penetration = 0;
	penetrationRatio = 0.0f;
	defenceIgnore = 0;
	defenceIgnoreRatio = 0.0f;

	manaShieldRatio = 0.0f;
	manaShieldLimit = 0;
}

void DamageCalculator::Interpret( const DamageMessage& message )
{
	StructCreature* target = message.target;
	StructCreature* from = message.from;

	originalDamage = message.damage;
	accuracyInc = message.accuracyBonus;
	critRateInc = message.criticalBonus;
	flag = message.flag;

	if ( message.advantage != NULL )
	{
		critRateInc += message.advantage->nCritical;
		critRateAmp += message.advantage->fCritical - 1.0f; // 크리율은 덧셈 연산
		stateAdvantage.inc += message.advantage->nDamage;
		stateAdvantage.amp *= message.advantage->fDamage; // 데미지는 곱 연산
	}

	if ( message.penalty != NULL )
	{
		critRateInc += message.penalty->nCritical;
		critRateAmp += message.penalty->fCritical - 1.0f; // 크리율은 덧셈 연산
		statePenalty.inc += message.penalty->nDamage;
		statePenalty.amp *= message.penalty->fDamage; // 데미지는 곱 연산
	}

	target->ProvideTargetInfo( message, *this );
	from->ProvideAttackerInfo( message, *this );
}


void DamageCalculator::Calculate( CalculationResult& result )
{
	SimulateDamageCalculation( result.simulation );
	CalculateActualDamage( result );
}


// CalculateActualDamage와 SimulateDamageCalculation 함수에서는 의도적으로 StructCreature의 멤버를 사용하지 않았는데,
// 이는 계산 과정을 StructCreature로부터 격리함으로써 추후 자동화된 테스트를 보다 쉽게 구현하기 위함이다.
void DamageCalculator::SimulateDamageCalculation( SimulationResult& result )
{
	if( SimulateAndCheckImmunity( result ) ||
	    SimulateAndCheckAvoidance( result ) ||
	    SimulateAndCheckBlocking( result ) )
	{
		return;
	}

	SimulateCritical( result );
	SimulateDouble(result);
	SimulateRandomDamage( result );
}

const bool DamageCalculator::SimulateAndCheckImmunity( SimulationResult& result )
{
	// 면역 체크
	if( isImmune )
	{
		result.missed = true;
		return true;
	}

	return false;
}

const bool DamageCalculator::SimulateAndCheckAvoidance( SimulationResult& result )
{
	// 회피 무시가 아닌경우 회피 체크 (추가/지속 데미지는 회피 없음)
	if( ( flag & DamageMessage::AVOID ) && avoid > 0 )
	{
		int hitRate = CalculateHitRate( isIgnoreLevelPenalty ? 0 : attackerLevel - targetLevel, accuracy, avoid, accuracyInc );

		if( XRandom( 0, 99 ) > hitRate )
		{
			result.missed = true;
			return true;
		}
	}

	return false;
}

const bool DamageCalculator::SimulateAndCheckBlocking( SimulationResult& result )
{
	if( ( flag & DamageMessage::BLOCK ) && ( flag & DamageMessage::DEFENCE ) )
	{
		if( XRandom( 0, 99 ) < blockRate )
		{
			if( XRandom( 0, 99 ) < perfectBlockRate )
			{
				result.perfectBlocked = true;
				return true;
			}
			else
			{
				result.blocked = true;
			}
		}
	}

	return false;
}

void DamageCalculator::SimulateCritical( SimulationResult& result )
{
	// 크리티컬
	if( flag & DamageMessage::CRITICAL )
	{
		int critRate = CalculateCritRate( critical, critRateAmp, critRateInc );

		if( XRandom( 0, 99 ) <= critRate )
		{
			result.critical = true;
		}
	}
}

void DamageCalculator::SimulateDouble(SimulationResult& result)
{
	// 크리티컬
	if (flag & DamageMessage::DOUBLE)
	{
		result.doubleattack = true;
	}
}

void DamageCalculator::SimulateRandomDamage( SimulationResult& result )
{
	if( ( flag & DamageMessage::RANDOM ) && !g_bIgnoreRandomDamage )
	{
		// 현재 게산 방식에서는 관통된 데미지 값과 일반 데미지 값을 함께 구한 뒤 이 값에 관통 파라메터를 적용한다.
		// 이 때 관통된 데미지 값에도 동일한 %의 랜덤 변위량이 적용되어야 하기 때문에 따로 계산한 뒤 둘에 곱한다.
		random.amp += XRandom( -5000, 5000 ) / 100000.0f; // 변위량은 5%
	}
}



const int DamageCalculator::CalculateHitRate( const int levelDiff, const float accuracy, const float avoid, const int bonus )
{
	return 7 + std::max( 10, 88 + levelDiff * 2 ) * ( accuracy / avoid ) + bonus;
}

const int DamageCalculator::CalculateCritRate( const int critRate, const float amplifier, const int bonus )
{
	return amplifier * critRate + bonus;
}



// 실제 적용되는 데미지를 계산하는 함수
// 계산 순서에 의존적인 로직들은 전부 여기로 격리시킬 것.
void DamageCalculator::CalculateActualDamage( CalculationResult& result ) const
{
	// 면역/회피/퍼펙트 블럭 당한 경우 더 볼 필요 없이 제로 데미지 반환
	if ( result.simulation.missed || result.simulation.perfectBlocked )
	{
		result.damage = 0;
		return;
	}

	result.damage = originalDamage;
	
	// 사냥 전문화 처리
	result.apply( expertiseAdvantage, false );
	result.apply( expertisePenalty, false );

	// 방어력 이전 데미지 변경 요인 적용
	result.apply( beforeDefence, false );

	CalculateDefenceAppliedDamage( result );

	result.apply( random, false );

	CalculateCriticalDamage( result );	
	CalculateResistance( result );

	// 방어력 이후 데미지 변경 요인 적용
	result.apply( afterDefence, true );

	// 게임 룰 (pvp, 양손 무기 전문화) 등에 의한 데미지 변경 요인 적용
	result.apply( rule, false );

	// 지속 효과에 의한 데미지 변경 요인 적용
	result.apply( statePenalty, true );
	result.apply( stateAdvantage, true );

	CalculateLaterDamageModifier( result );

	CalculateManaShieldEffect( result );
	CalculatePenetrationEffect( result );
}

void DamageCalculator::CalculateDefenceAppliedDamage( CalculationResult& result ) const
{
	int actualDamage = result.damage;
	int actualDefence = 0;
	
	if ( flag & DamageMessage::DEFENCE ) // 방어력 적용 시
	{
		actualDefence = result.simulation.blocked ? blockDefence : defence;
		actualDefence = ( actualDefence - defenceIgnore ) * ( 1.0f - defenceIgnoreRatio );
		actualDefence = std::max( actualDefence, 0 );
	}

	switch( damageType )
	{
	case DamageMessage::DIRECT_DAMAGE:
		result.damage = CalculateDirectDamageWithDefence( actualDamage, actualDefence, attackerLevel );
		result.penetratedDamage = CalculateDirectDamageWithDefence( actualDamage, 0, attackerLevel );
		break;
	case DamageMessage::STATE_DAMAGE:
		result.damage = CalculateStateDamageWithDefence( actualDamage, actualDefence, attackPoint );
		result.penetratedDamage = CalculateStateDamageWithDefence( actualDamage, 0, attackPoint );
		break;
	case DamageMessage::ADDITIONAL_DAMAGE: // 방어력 미적용인 경우.
	case DamageMessage::NAKED_DAMAGE: // 방어력 미적용인 경우.
		result.penetratedDamage = actualDamage;
		break;
	}
}

void DamageCalculator::CalculateCriticalDamage( CalculationResult& result ) const
{
	if( result.simulation.critical )
	{
		float coefficient = 1.0f + ( critPower / 100.0f );
		result.damage *= coefficient;
		result.penetratedDamage *= coefficient;
	}
}

void DamageCalculator::CalculateResistance( CalculationResult& result ) const
{
	// 저항된 데미지 출력을 위해 따로 보관(아직 사용은 안 함)
	int damageBeforeResist = result.damage;
	result.apply( resist, true );
	result.resistedDamage = damageBeforeResist - result.damage;
}

void DamageCalculator::CalculateLaterDamageModifier( CalculationResult& result ) const
{
	result.damage = std::max( result.damage, 0 );
	// 관통 데미지는 원 데미지보다는 무조건 높아야 한다.
	result.penetratedDamage = std::max( result.penetratedDamage, result.damage );
}

void DamageCalculator::CalculateManaShieldEffect( CalculationResult& result ) const
{
	if( manaShieldRatio > 0.0f )
	{
		result.manaShieldAbsorption = std::min<int>( result.damage * std::min( 1.0f, manaShieldRatio ), manaShieldLimit );
		result.damage -= result.manaShieldAbsorption;
	}
}

void DamageCalculator::CalculatePenetrationEffect( CalculationResult& result ) const
{
	int reducedAmount = result.penetratedDamage - result.damage;
	int penetrationBonus = std::min<int>( reducedAmount * penetrationRatio + penetration, reducedAmount );
		
	result.damage += penetrationBonus;
	// 관통되는 경우에도 마나 쉴드의 소모량이 그대로라면 불공평하므로 관통당한 비율만큼 소모량을 줄임
	if ( reducedAmount > 0 )
	{
		result.manaShieldAbsorption *= static_cast< float >( reducedAmount - penetrationBonus ) / reducedAmount;
	}
}

const int DamageCalculator::CalculateDirectDamageWithDefence( const int damage, const int defence, const int attackerLevel )
{
	int adjustedDamage = std::max( damage, 1 );
	double levelBonus = attackerLevel * 1.7f * std::max( 1 - 0.4 * defence / adjustedDamage , 0.3 );
	double damageWithDefence = adjustedDamage * std::max( 1 - 0.5 * defence / adjustedDamage, 0.05 );
	int actualDamage = levelBonus + damageWithDefence;

	return std::max( actualDamage, 1 );
}

const int DamageCalculator::CalculateStateDamageWithDefence( const float damage, const int defence, const int attackPoint )
{
	int adjustedDefence = std::max( defence, 1 );
	float coefficient = ( defence > attackPoint ) ? 1.0f - ( defence - attackPoint ) / ( adjustedDefence * 2 ) : 1.0f;

	return damage * coefficient;
}
