
#include <toolkit/XMemoryPool.h>

#include "StructProc.h"
#include "GameAllocator.h"
#include "StructSkill.h"


const bool _PROC_TAG::CheckProc( const int _hp_rate, const int _target_hp_rate ) const
{
	if( XRandom() % 100 >= ratio ) return false;

	if( min_hp && _hp_rate < min_hp ) return false;
	if( max_hp && _hp_rate > max_hp ) return false;
	if( target_min_hp && ( _target_hp_rate == -1 || _target_hp_rate < target_min_hp ) ) return false;
	if( target_max_hp && ( _target_hp_rate == -1 || _target_hp_rate >= target_max_hp ) ) return false;

	return true;
}

const bool _ATTACK_TAG::CheckProcByAttack( const int _hp_rate, const int _target_hp_rate, const DWORD _attack_type, const int _elemental_type ) const
{
	if( !CheckProc( _hp_rate, _target_hp_rate ) ) return false;

	if( ( attack_type & _attack_type ) != _attack_type ) return false;
	if( elemental_type != 99 && elemental_type != _elemental_type ) return false;

	return true;
}

const bool _KILL_TAG::CheckProcByDead( const int _hp_rate, const int _target_hp_rate, const int _diff_level, const int _mp_rate ) const
{
	if( !CheckProc( _hp_rate, _target_hp_rate ) ) return false;

	if( max_diff_level != -1 && _diff_level > max_diff_level ) return false;
	if( min_mp_rate && _mp_rate < min_mp_rate ) return false;

	return true;
}

void * StructProc::operator new( size_t nAllocSize )
{
	assert( GetProcHeap().GetBlockSize() >= nAllocSize );

	StructProc * pBlock = allocProcStruct();
	if( !pBlock )
		throw std::bad_alloc( "StructProc alloc failed." );

	return pBlock;
}

void * StructProc::operator new[]( size_t nArraySize )
{
	assert( 0 );
	throw std::bad_alloc( "StructProc: XMemoryPool does not support allocating an array." );
}

void StructProc::operator delete( void * pBlock )
{
	StructProc * pFreeBlock = reinterpret_cast< StructProc * >( pBlock );

	prepareFreeProcStruct( pFreeBlock );

	pFreeBlock->~StructProc();

	freeProcStruct( pFreeBlock );
}

void StructProc::operator delete[]( void * pBlock )
{
	assert( 0 );
	throw std::exception( "StructProc: XMemoryPool does not support de-allocating an array." );
}

void StructStateProc::Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking )
{
	AR_TIME t = GetArTime();
	StructCreature *pStateTarget = NULL;
	if( bIsAttacking )	pStateTarget = target == ATTACKER ? pOwner : pTarget;
	else				pStateTarget = target == ATTACKER ? pTarget : pOwner;
	if( !pStateTarget ) return;

	pStateTarget->AddState( code, pOwner->GetHandle(), level, t, t + duration );

	pOwner->AddMP( -cost_mp );
}

void StructAbsorbProc::Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking )
{
	if( !nDamage ) return;

	StructCreature *pAbsorbTarget = NULL;
	if( bIsAttacking )	pAbsorbTarget = target == ATTACKER ? pOwner : pTarget;
	else				pAbsorbTarget = target == ATTACKER ? pTarget : pOwner;
	if( !pAbsorbTarget ) return;

	if( hp_absorb_ratio ) pAbsorbTarget->AddHP( hp_absorb_ratio * nDamage );
	if( mp_absorb_ratio ) pAbsorbTarget->AddMP( mp_absorb_ratio * nDamage );
}

void StructHealProc::Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking )
{
	StructCreature *pIncTarget = NULL;
	if( bIsAttacking )	pIncTarget = target == ATTACKER ? pOwner : pTarget;
	else				pIncTarget = target == ATTACKER ? pTarget : pOwner;
	if( !pIncTarget ) return;

	if( hp_inc ) pIncTarget->AddHP( hp_inc );
	if( mp_inc ) pIncTarget->AddMP( mp_inc );
}

void StructStealProc::Proc( StructCreature *pOwner, StructCreature *pTarget, const int damage, const bool bIsAttacking )
{
	StructCreature *pStealer = NULL;
	StructCreature *pStealTarget = NULL;
	if( bIsAttacking )
	{
		pStealer = target == ATTACKER ? pOwner : pTarget;
		pStealTarget = target == ATTACKER ? pTarget : pOwner;
	}
	else
	{
		pStealer = target == ATTACKER ? pTarget : pOwner;
		pStealTarget = target == ATTACKER ? pOwner : pTarget;
	}
	if( !pStealer || !pStealTarget ) return;

	if( hp_steal )
	{
		int nHP = std::min( pStealTarget->GetHP(), hp_steal );

		pStealer->AddHP( nHP );
		pStealTarget->damage( pOwner, nHP );
	}
	if( mp_steal )
	{
		int nMP = std::min( pStealTarget->GetMP(), mp_steal );

		pStealer->AddMP( nMP );
		pStealTarget->AddMP( 0 - nMP );
	}
}

void StructEnergyProc::Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking )
{
	StructCreature *pEnergyTarget = NULL;
	if( bIsAttacking )	pEnergyTarget = target == ATTACKER ? pOwner : pTarget;
	else				pEnergyTarget = target == ATTACKER ? pTarget : pOwner;
	if( !pEnergyTarget ) return;

	pEnergyTarget->AddEnergy( energy_count );
}

void StructCooldownProc::Proc( StructCreature *pOwner, StructCreature *pTarget, const int nDamage, const bool bIsAttacking )
{
	StructCreature *pCooldownTarget = NULL;
	if( bIsAttacking )	pCooldownTarget = target == ATTACKER ? pOwner : pTarget;
	else				pCooldownTarget = target == ATTACKER ? pTarget : pOwner;
	if( !pCooldownTarget ) return;

	if( is_all_skill )	pCooldownTarget->AddRemainCoolTime( -1, inc, 1.0f );
	if( skill_id1 )		pCooldownTarget->AddRemainCoolTime( skill_id1, inc1, 1.0f );
	if( skill_id2 )		pCooldownTarget->AddRemainCoolTime( skill_id2, inc2, 1.0f );
}