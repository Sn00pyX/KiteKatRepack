
#include <toolkit/XConsole.h>
#include <toolkit/XEnv.h>
#include <mmo/ArcadiaServer.h>

#include "StructSkillProp.h"
#include "GameAllocator.h"
#include "StructCreature.h"
#include "StructSummon.h"
#include "StructNPC.h"
#include "StructMonster.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "StructSkill.h"


StructSkillProp* StructSkillProp::Create( AR_HANDLE caster, StructSkill* pSkill, int nMagicPoint, float fHateRatio )
{
	StructSkillProp* pProp = new StructSkillProp( caster, pSkill, nMagicPoint, fHateRatio );
	
	return pProp;
}

StructSkillProp::~StructSkillProp()
{
	FreeMiscHandle( m_hHandle );

#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructSkillProp" );
#endif
}

StructSkillProp::StructSkillProp( AR_HANDLE caster, StructSkill* pSkill, int nMagicPoint, float fHateRatio )
		: m_hCaster( caster ), m_Skill( *pSkill ), m_nOwnerMagicPoint( nMagicPoint ), m_fHateRatio( fHateRatio )
{
	m_hHandle = AllocMiscHandle( this );

	m_nArObjectType = ArObject::STATIC_OBJECT;

	m_bFired = false;
	m_bProcessEnd = false;
	m_bIsRemovePended = false;

	const SkillBase *pSkillBase = m_Skill.GetSkillBase();

	switch( pSkillBase->GetSkillEffectType() )
	{
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE_OLD:
		{
			INIT_AREA_EFFECT_MAGIC_DAMAGE();
			break;
		}
	case SkillBase::EF_AREA_EFFECT_HEAL:
		{
			INIT_AREA_EFFECT_HEAL();
			break;
		}
	case SkillBase::EF_AREA_EFFECT_HEAL_BY_FIELD_PROP:
		{
			INIT_AREA_EFFECT_HEAL_BY_FIELD_PROP();
			break;
		}
	// 신규로 추가된 유형의 일반형 초기화 함수 사용 유형
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE:
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL:
		{
			INIT_SKILL_PROP_PARAMETER( ( m_Skill.GetVar(6) + m_Skill.GetVar(7) * m_Skill.GetRequestedSkillLevel() ) * 100, m_Skill.GetVar(8) * 100 );
			break;
		}
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL_T2:
		{
			INIT_SKILL_PROP_PARAMETER( ( m_Skill.GetVar(12) + m_Skill.GetVar(13) * m_Skill.GetRequestedSkillLevel() ) * 100, m_Skill.GetVar(14) * 100 );
			break;
		}
		// 트랩
	case SkillBase::EF_TRAP_PHYSICAL_DAMAGE:
	case SkillBase::EF_TRAP_MAGICAL_DAMAGE:
	case SkillBase::EF_TRAP_MULTIPLE_PHYSICAL_DAMAGE:
	case SkillBase::EF_TRAP_MULTIPLE_MAGICAL_DAMAGE:
		{
			// 0.3초 주기로 반경 내에 대상이 있는지 없는지 검사하도록 초기화
			INIT_SKILL_PROP_PARAMETER( ( m_Skill.GetVar(3) + m_Skill.GetVar(4) * m_Skill.GetRequestedSkillLevel() ) * 100, 30 );
			break;
		}
	default:
		assert( 0 );
	}

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructSkillProp" );
#endif
}

bool StructSkillProp::ProcDelete()
{
	delete this;

	return true;
}

void StructSkillProp::PendRemove()
{
	if( !m_bProcessEnd && !m_bIsRemovePended )
	{
		m_bIsRemovePended = true;
	}
}

void StructSkillProp::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "StructSkillProp" );

	AR_TIME current_time = GetArTime();

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "StructSkillProp(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = current_time;

	if( m_bProcessEnd )
	{
		return;
	}

	// Lock
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

	if( current_time > m_Info.m_nEndTime || m_bIsRemovePended )
	{
		m_bProcessEnd = true;
		// 해당 플레이어가 트랩을 설치할 수 있도록 현재 설치한 트랩 핸들 초기화

		StructCreature::iterator itPlayer = StructCreature::get( m_hCaster );
		StructCreature *pCaster = static_cast< StructCreature * >( *itPlayer );
		if( pCaster && pCaster->GetTrapHandle() == m_hHandle )
		{
			pCaster->SetTrapHandle( NULL );
		}
		ArcadiaServer::Instance().RemoveObject( this );
		ArcadiaServer::Instance().DeleteObject( this );

		return;
	}

	if( current_time < m_Info.m_nLastFireTime + m_Info.m_nInterval )
	{
		// 아직 다음 시전 시간 아님
		return;
	}

	m_Info.m_nLastFireTime = current_time;

	// 시전자 찾기
	GameObject::iterator git = GameObject::get( m_hCaster );
	StructCreature *pCaster = static_cast< StructCreature * >( *git );
	if( !pCaster || pCaster->IsSummon() && static_cast< StructSummon * >( pCaster )->GetMaster() == NULL )
	{
		m_bProcessEnd = true;
		// 해당 플레이어가 트랩을 설치할 수 있도록 현재 설치한 트랩 핸들 초기화 불필요(이미 유저가 없음)
		ArcadiaServer::Instance().RemoveObject( this );
		ArcadiaServer::Instance().DeleteObject( this );
		return;
	}

	m_Skill.m_targetPos = GetPos();
	m_Skill.m_targetLayer = GetLayer();

	m_Skill.m_vResultList.clear();
	m_Skill.m_nTargetCount = 0;

	switch( m_Skill.GetSkillBase()->GetSkillEffectType() )
	{
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE_OLD:
		{
			FIRE_AREA_EFFECT_MAGIC_DAMAGE_OLD( pCaster );
			break;
		}
	case SkillBase::EF_AREA_EFFECT_HEAL:
		{
			FIRE_AREA_EFFECT_HEAL( pCaster );
			break;
		}
	case SkillBase::EF_AREA_EFFECT_HEAL_BY_FIELD_PROP:
		{
			FIRE_AREA_EFFECT_HEAL_BY_FIELD_PROP( pCaster );
			break;
		}
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE:
		{
			FIRE_AREA_EFFECT_MAGIC_DAMAGE( pCaster );
			break;
		}
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL:
		{
			FIRE_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL( pCaster );
			break;
		}
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL_T2:
		{
			FIRE_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL_T2( pCaster );
			break;
		}
	case SkillBase::EF_TRAP_PHYSICAL_DAMAGE:
	case SkillBase::EF_TRAP_MAGICAL_DAMAGE:
		{
			FIRE_TRAP_DAMAGE( pCaster );
			break;
		}
	case SkillBase::EF_TRAP_MULTIPLE_PHYSICAL_DAMAGE:
	case SkillBase::EF_TRAP_MULTIPLE_MAGICAL_DAMAGE:
		{
			FIRE_TRAP_MULTIPLE_DAMAGE( pCaster );
			break;
		}
	default:
		{
			assert( 0 );
			break;
		}
	}

	// 본래는 스킬 사용 이후 관련 로직들이 일괄적으로 적용되야 하겠지만
	// 수정 시의 영향력을 평가하기가 어려워 당장은 Hate, Havok만 적용한다.
	// 이에 따른 다소의 코드 중복은 양해 바람
	for( std::vector< SkillResult >::iterator it = m_Skill.m_vResultList.begin(); it != m_Skill.m_vResultList.end(); ++it )
	{
		if( it->GetType() == SkillResult::DAMAGE || it->GetType() == SkillResult::MAGIC_DAMAGE ||  it->GetType() == SkillResult::DAMAGE_WITH_KNOCK_BACK ||
			it->GetType() == SkillResult::ADD_HP || it->GetType() == SkillResult::ADD_MP || it->GetType() == SkillResult::ADD_HP_MP_SP || it->GetType() == SkillResult::ADD_STATE ||
			it->GetType() == SkillResult::CHAIN_DAMAGE || it->GetType() == SkillResult::CHAIN_MAGIC_DAMAGE || it->GetType() == SkillResult::CHAIN_HEAL )
		{
			StructCreature::iterator cit = StructCreature::get( it->damage.hTarget );
			StructCreature * pDealTarget = (*cit);
			m_Skill.ApplyHateAndHavokFromSkillResult( pCaster, pDealTarget, *it );
		}
	}

	if( m_Skill.m_nTargetCount )
		m_Skill.broadcastSkillMessage( GetRX(), GetRY(), GetLayer(), 0, 0, TS_SC_SKILL::REGION_FIRE );
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StructSkillProp::INIT_AREA_EFFECT_MAGIC_DAMAGE()
{
	m_Info.m_nStartTime = GetArTime();
	m_Info.m_nEndTime = m_Info.m_nStartTime + ( m_Skill.GetVar(3) + m_Skill.GetVar(4) * m_Skill.GetRequestedSkillLevel() ) * 100;
	m_Info.m_nInterval = m_Skill.GetVar(6) * 100;
	m_Info.m_nLastFireTime = 0;
}

void StructSkillProp::FIRE_AREA_EFFECT_MAGIC_DAMAGE_OLD( struct StructCreature * pCaster )
{
	std::vector< AR_HANDLE > vResult;
	vResult.reserve( 30 );

	float fRange = m_Skill.GetVar(5) * GameRule::DEFAULT_UNIT_SIZE;

	// { 데미지 계산
	int nDamage = m_nOwnerMagicPoint + m_Skill.GetVar(0) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(1) + m_Skill.GetEnhance() * m_Skill.GetVar(7);
	nDamage *= m_Skill.GetVar(2) + m_Skill.GetEnhance() * m_Skill.GetVar(8);
	// }

	// 근처 유닛 리스트를 vResult에 긁어옴
	ArcadiaServer::Instance().EnumMovableObject( GetPos(), GetLayer(), fRange, &vResult );
	
	StructCreature *pCreature;
	std::vector< AR_HANDLE >::iterator it;

	SkillResult skill_result;

	for( it = vResult.begin(); it != vResult.end(); ++it )
	{
		// 크리쳐 핸들 얻는다
		pCreature = static_cast< StructCreature * >( GameObject::raw_get( *it ) );
		if( !pCreature || pCreature->IsPet() ) continue;

		if( !pCaster->IsEnemy( pCreature, true ) )
			continue;

		if( pCreature->IsDead() )
			continue;


		int flag = 0;
		int elemental_type = m_Skill.GetSkillBase()->GetElementalType();
		// 데미지 주기
		StructCreature::_DAMAGE Damage = pCreature->DealMagicalDamage( m_Skill.m_pOwner, nDamage, (Elemental::Type) elemental_type, m_Skill.GetCriticalBonus( m_Skill.GetRequestedSkillLevel() ) );

		if( Damage.bCritical	)	flag |= SkillResult::CRITICAL;
		if( Damage.bBlock		)	flag |= SkillResult::BLOCK;
		if( Damage.bMiss		)	flag |= SkillResult::MISS;
		if( Damage.bPerfectBlock)	flag |= SkillResult::PERFECT_BLOCK;

		skill_result.damage.type = SkillResult::MAGIC_DAMAGE;
		skill_result.damage.damage_type = elemental_type;
		skill_result.damage.damage = Damage.nDamage;
		skill_result.damage.hTarget = pCreature->GetHandle();
		skill_result.damage.flag = flag;
		skill_result.damage.target_hp = pCreature->GetHP();

		m_Skill.m_vResultList.push_back( skill_result );

		++m_Skill.m_nTargetCount;

		if( !Damage.bMiss && m_Skill.m_pOwner->GetPos().GetDistance( GetPos() ) <= GameRule::VISIBLE_RANGE )
		{
			if( pCreature->IsMonster() )
			{
				int nHate = Damage.nDamage;

				std::pair< float, int > HateMod = pCaster->GetHateMod( ( m_Skill.GetSkillBase()->IsPhysicalSkill() ) ? 1 : 2, m_Skill.GetSkillBase()->IsHarmful() );

				nHate += HateMod.second;
				nHate *= HateMod.first;

				static_cast< StructMonster * >( pCreature )->AddHate( m_Skill.m_pOwner->GetHandle(), m_fHateRatio * nHate );
			}
			else if( pCreature->IsNPC() )
				static_cast< StructNPC * >( pCreature )->SetAttacker( m_Skill.m_pOwner );
		}
	}
}

void StructSkillProp::INIT_AREA_EFFECT_HEAL()
{
	m_Info.m_nStartTime = GetArTime();
	m_Info.m_nEndTime = m_Info.m_nStartTime + ( m_Skill.GetVar(7) + m_Skill.GetVar(8) * m_Skill.GetRequestedSkillLevel() ) * 100;
	m_Info.m_nInterval = m_Skill.GetVar(10) * 100;
	m_Info.m_nLastFireTime = 0;
}

void StructSkillProp::FIRE_AREA_EFFECT_HEAL( struct StructCreature * pCaster )
{
	std::vector< AR_HANDLE > vResult;

	AR_UNIT fRange = m_Skill.GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;
	m_Skill.m_fRange = fRange;

	int nHPHeal = m_Skill.GetVar(0) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(1);
	int nMPHeal = m_Skill.GetVar(2) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(3);
	int nSPHeal = m_Skill.GetVar(4) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(5);
	nHPHeal *= m_Skill.GetVar(11) * m_Skill.GetEnhance() + 1;
	nMPHeal *= m_Skill.GetVar(11) * m_Skill.GetEnhance() + 1;
	nSPHeal *= m_Skill.GetVar(11) * m_Skill.GetEnhance() + 1;

	// 근처 유닛 리스트를 vResult에 긁어옴
	ArcadiaServer::Instance().EnumMovableObject( GetPos(), GetLayer(), fRange, &vResult );
	
	StructCreature *pCreature;
	std::vector< AR_HANDLE >::iterator it;

	int nTargetLimit = m_Skill.GetVar(6);
	SkillResult skill_result;

	for( it = vResult.begin(); it != vResult.end(); ++it )
	{
		// 크리쳐 핸들 얻는다
		pCreature = static_cast< StructCreature * >( GameObject::raw_get( *it ) );
		if( !pCreature || pCreature->IsPet() ) continue;

		if( nTargetLimit == SkillBase::SKILL_EFFECT_TARGET_LIMIT_NOT_ENEMY && pCaster->IsEnemy( pCreature, true ) )
			continue;

		if( nTargetLimit == SkillBase::SKILL_EFFECT_TARGET_LIMIT_ONLY_ALLY && !pCaster->IsAlly( pCreature ) )
			continue;

		if( pCreature->IsDead() )
			continue;

		// HEAL~
		nHPHeal = pCreature->Heal( nHPHeal );
		nMPHeal = pCreature->MPHeal( nMPHeal );

		if( pCreature->IsSummon() )
		{
			StructSummon * pSummon = static_cast< StructSummon * >( pCreature );
			pSummon->SetSP( pSummon->GetSP() + nSPHeal );
		}

		skill_result.add_hp_mp_sp.type = SkillResult::ADD_HP_MP_SP;
		skill_result.add_hp_mp_sp.hTarget = pCreature->GetHandle();
		skill_result.add_hp_mp_sp.target_hp = pCreature->GetHP();
		skill_result.add_hp_mp_sp.target_mp = pCreature->GetMP();
		skill_result.add_hp_mp_sp.nIncHP = nHPHeal;
		skill_result.add_hp_mp_sp.nIncMP = nMPHeal;
		skill_result.add_hp_mp_sp.nIncSP = nSPHeal;

		m_Skill.m_vResultList.push_back( skill_result );

		++m_Skill.m_nTargetCount;
	}
}

void StructSkillProp::INIT_AREA_EFFECT_HEAL_BY_FIELD_PROP()
{
	m_Info.m_nStartTime = GetArTime();
	m_Info.m_nEndTime = m_Info.m_nStartTime + ( m_Skill.GetVar(7) + m_Skill.GetVar(8) * m_Skill.GetRequestedSkillLevel() ) * 100;
	m_Info.m_nInterval = m_Skill.GetVar(5) * 100;
	m_Info.m_nLastFireTime = 0;
}

void StructSkillProp::FIRE_AREA_EFFECT_HEAL_BY_FIELD_PROP( struct StructCreature * pCaster )
{
	std::vector< AR_HANDLE > vResult;

	AR_UNIT fRange = m_Skill.GetVar(4) * GameRule::DEFAULT_UNIT_SIZE;
	m_Skill.m_fRange = fRange;

	c_fixed10 fHPHeal = m_Skill.GetVar(0) + m_Skill.GetVar(1) * m_Skill.GetRequestedSkillLevel();
	c_fixed10 fMPHeal = m_Skill.GetVar(2) + m_Skill.GetVar(3) * m_Skill.GetRequestedSkillLevel();

	// 근처 유닛 리스트를 vResult에 긁어옴
	ArcadiaServer::Instance().EnumMovableObject( GetPos(), GetLayer(), fRange, &vResult );
	
	StructCreature *pCreature;
	std::vector< AR_HANDLE >::iterator it;

	int nTargetLimit = m_Skill.GetVar(6);
	SkillResult skill_result;

	for( it = vResult.begin(); it != vResult.end(); ++it )
	{
		// 크리쳐 핸들 얻는다
		pCreature = static_cast< StructCreature * >( GameObject::raw_get( *it ) );
		if( !pCreature || pCreature->IsPet() ) continue;

		switch( nTargetLimit )
		{
		case SkillBase::SKILL_EFFECT_TARGET_LIMIT_NOT_ENEMY:
			if( pCaster->IsEnemy( pCreature, true ) )
				continue;
			break;
		case SkillBase::SKILL_EFFECT_TARGET_LIMIT_ONLY_ALLY:
			if( !pCaster->IsAlly( pCreature ) )
				continue;
			break;
		case SkillBase::SKILL_EFFECT_TARGET_LIMIT_ONLY_ENEMY:
			if( !pCaster->IsEnemy( pCreature ) )
				continue;
			break;
		}

		if( pCreature->IsDead() )
			continue;

		// HEAL~
		int nHPHeal = pCreature->Heal( fHPHeal * pCreature->GetMaxHP() );
		int nMPHeal = pCreature->MPHeal( fMPHeal * pCreature->GetMaxMP() );

		skill_result.add_hp_mp_sp.type = SkillResult::ADD_HP_MP_SP;
		skill_result.add_hp_mp_sp.hTarget = pCreature->GetHandle();
		skill_result.add_hp_mp_sp.target_hp = pCreature->GetHP();
		skill_result.add_hp_mp_sp.target_mp = pCreature->GetMP();
		skill_result.add_hp_mp_sp.nIncHP = nHPHeal;
		skill_result.add_hp_mp_sp.nIncMP = nMPHeal;
		skill_result.add_hp_mp_sp.nIncSP = 0;

		m_Skill.m_vResultList.push_back( skill_result );

		++m_Skill.m_nTargetCount;
	}
}

void StructSkillProp::INIT_SKILL_PROP_PARAMETER( AR_TIME nDuration, AR_TIME nInterval )
{
	m_Info.m_nStartTime = GetArTime();
	m_Info.m_nEndTime = m_Info.m_nStartTime + nDuration;
	m_Info.m_nInterval = nInterval;
	m_Info.m_nLastFireTime = 0;
}

void StructSkillProp::FIRE_AREA_EFFECT_MAGIC_DAMAGE( struct StructCreature * pCaster )
{
	std::vector< StructCreature * > vResult;
	vResult.reserve( 30 );

	float fRange = m_Skill.GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;

	// { 데미지 계산
	int nDamage = m_nOwnerMagicPoint * ( m_Skill.GetVar(0) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(1) + m_Skill.GetEnhance() * m_Skill.GetVar(2) ) + m_Skill.GetVar(3) + m_Skill.GetVar(4) * m_Skill.GetRequestedSkillLevel() + m_Skill.GetVar(5) * m_Skill.GetEnhance();
	nDamage *= m_Skill.GetVar(10) + m_Skill.GetVar(11) * m_Skill.GetEnhance();
	// }

	// 근처 유닛 리스트를 vResult에 긁어옴
	nDamage = EnumSkillTargetsAndCalcDamage( GetPos(), GetLayer(), GetPos(), false, fRange, -1, 0, nDamage, true, pCaster, m_Skill.GetVar(16), m_Skill.GetVar(17), vResult );
	
	StructCreature *pCreature;
	std::vector< StructCreature * >::iterator it;

	SkillResult skill_result;

	AR_TIME t = GetArTime();

	for( it = vResult.begin(); it != vResult.end(); ++it )
	{
		// 크리쳐 핸들 얻는다
		pCreature = *it;
		if( !pCreature || pCreature->IsPet() ) continue;

		if( !pCaster->IsEnemy( pCreature, true ) )
			continue;

		if( pCreature->IsDead() )
			continue;

		int flag = 0;
		int elemental_type = m_Skill.GetSkillBase()->GetElementalType();
		// 데미지 주기
		StructCreature::_DAMAGE Damage = pCreature->DealMagicalDamage( m_Skill.m_pOwner, nDamage, (Elemental::Type) elemental_type, m_Skill.GetHitBonus( m_Skill.GetEnhance(), pCaster->GetLevel() - pCreature->GetLevel() ), m_Skill.GetCriticalBonus( m_Skill.GetRequestedSkillLevel() ), 0, pCreature->GetMagicalSkillStatePenalty() );

		if( Damage.bCritical	)	flag |= SkillResult::CRITICAL;
		if( Damage.bBlock		)	flag |= SkillResult::BLOCK;
		if( Damage.bMiss		)	flag |= SkillResult::MISS;
		if( Damage.bPerfectBlock)	flag |= SkillResult::PERFECT_BLOCK;

		skill_result.damage.type = SkillResult::MAGIC_DAMAGE;
		skill_result.damage.damage_type = elemental_type;
		skill_result.damage.damage = Damage.nDamage;
		skill_result.damage.hTarget = pCreature->GetHandle();
		skill_result.damage.flag = flag;
		skill_result.damage.target_hp = pCreature->GetHP();

		m_Skill.m_vResultList.push_back( skill_result );

		++m_Skill.m_nTargetCount;

		if( !Damage.bMiss )
		{
			if( pCreature->IsMonster() )
			{
				int nHate = Damage.nDamage;

				std::pair< float, int > HateMod = pCaster->GetHateMod( ( m_Skill.GetSkillBase()->IsPhysicalSkill() ) ? 1 : 2, m_Skill.GetSkillBase()->IsHarmful() );

				nHate += HateMod.second;
				nHate *= HateMod.first;

				static_cast< StructMonster * >( pCreature )->AddHate( m_Skill.m_pOwner->GetHandle(), m_fHateRatio * nHate );
			}
			else if( pCreature->IsNPC() )
				static_cast< StructNPC * >( pCreature )->SetAttacker( m_Skill.m_pOwner );

			if( !Damage.bMiss && pCreature->IsAlive() )
			{
				StructState::StateCode nStateCode = static_cast< StructState::StateCode >(m_Skill.GetStateId());

				if( nStateCode )
				{
					int nLevel = m_Skill.GetStateLevel( m_Skill.GetRequestedSkillLevel(), m_Skill.GetEnhance() );

					AR_TIME nDuration = m_Skill.GetStateSecond( m_Skill.GetRequestedSkillLevel(), m_Skill.GetEnhance() );
					pCreature->AddState( nStateCode, pCaster->GetHandle(), nLevel, t, t + nDuration );
				}
			}
		}
	}

	// 확률로 상태이상 부여
}

void StructSkillProp::FIRE_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL( struct StructCreature * pCaster )
{
	std::vector< AR_HANDLE > vResult;
	vResult.reserve( 30 );

	float fRange = m_Skill.GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;

	// { 데미지 계산
	int nDamage = m_nOwnerMagicPoint * ( m_Skill.GetVar(0) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(1) + m_Skill.GetEnhance() * m_Skill.GetVar(2) );
	nDamage *= m_Skill.GetVar(10) + m_Skill.GetEnhance() * m_Skill.GetVar(11);

	int nHPHeal = m_Skill.GetVar(3) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(4) + m_Skill.GetEnhance() * m_Skill.GetVar(5);
	// }

	// 근처 유닛 리스트를 vResult에 긁어옴
	ArcadiaServer::Instance().EnumMovableObject( GetPos(), GetLayer(), fRange, &vResult );
	
	StructCreature *pCreature;
	std::vector< AR_HANDLE >::iterator it;
	
	for( it = vResult.begin(); it != vResult.end(); ++it )
	{
		SkillResult skill_result;

		// 크리쳐 핸들 얻는다
		pCreature = static_cast< StructCreature * >( GameObject::raw_get( *it ) );
		if( !pCreature || pCreature->IsPet() ) continue;

		if( pCreature->IsDead() )
			continue;

		// 적이면 뎀쥐 드셈
		if( pCaster->IsEnemy( pCreature, true ) )
		{
			int flag = 0;
			int elemental_type = m_Skill.GetSkillBase()->GetElementalType();
			// 데미지 주기
			StructCreature::_DAMAGE Damage = pCreature->DealMagicalDamage( m_Skill.m_pOwner, nDamage, (Elemental::Type) elemental_type, m_Skill.GetCriticalBonus( m_Skill.GetRequestedSkillLevel() ) );

			if( Damage.bCritical	)	flag |= SkillResult::CRITICAL;
			if( Damage.bBlock		)	flag |= SkillResult::BLOCK;
			if( Damage.bMiss		)	flag |= SkillResult::MISS;
			if( Damage.bPerfectBlock)	flag |= SkillResult::PERFECT_BLOCK;

			skill_result.damage.type = SkillResult::MAGIC_DAMAGE;
			skill_result.damage.damage_type = elemental_type;
			skill_result.damage.damage = Damage.nDamage;
			skill_result.damage.hTarget = pCreature->GetHandle();
			skill_result.damage.flag = flag;
			skill_result.damage.target_hp = pCreature->GetHP();

			m_Skill.m_vResultList.push_back( skill_result );

			++m_Skill.m_nTargetCount;

			if( !Damage.bMiss )
			{
				if( pCreature->IsMonster() )
				{
					int nHate = Damage.nDamage;

					std::pair< float, int > HateMod = pCaster->GetHateMod( ( m_Skill.GetSkillBase()->IsPhysicalSkill() ) ? 1 : 2, m_Skill.GetSkillBase()->IsHarmful() );

					nHate += HateMod.second;
					nHate *= HateMod.first;

					static_cast< StructMonster * >( pCreature )->AddHate( m_Skill.m_pOwner->GetHandle(), m_fHateRatio * nHate );
				}
				else if( pCreature->IsNPC() )
					static_cast< StructNPC * >( pCreature )->SetAttacker( m_Skill.m_pOwner );
			}
		}
		// 아군이면 힐 드셈
		else if( pCaster->IsAlly( pCreature ) )
		{
			nHPHeal = pCreature->Heal( nHPHeal );

			skill_result.add_hp_mp_sp.type = SkillResult::ADD_HP;
			skill_result.add_hp_mp_sp.hTarget = pCreature->GetHandle();
			skill_result.add_hp_mp_sp.target_hp = pCreature->GetHP();
			skill_result.add_hp_mp_sp.target_mp = pCreature->GetMP();
			skill_result.add_hp_mp_sp.nIncHP = nHPHeal;
			skill_result.add_hp_mp_sp.nIncMP = 0;
			skill_result.add_hp_mp_sp.nIncSP = 0;

			m_Skill.m_vResultList.push_back( skill_result );

			++m_Skill.m_nTargetCount;
		}
	}
}

void StructSkillProp::FIRE_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL_T2( struct StructCreature * pCaster )
{
	std::vector< StructCreature * > vResult;
	vResult.reserve( 30 );

	float fRange = m_Skill.GetVar(15) * GameRule::DEFAULT_UNIT_SIZE;

	// { 데미지 계산
	int nHPHeal = m_nOwnerMagicPoint * ( m_Skill.GetVar(0) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(1) + m_Skill.GetEnhance() * m_Skill.GetVar(2) ) + m_Skill.GetVar(3) + m_Skill.GetVar(4) * m_Skill.GetRequestedSkillLevel() + m_Skill.GetVar(5) * m_Skill.GetEnhance();
	int nDamage = m_nOwnerMagicPoint * ( m_Skill.GetVar(6) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(7) + m_Skill.GetEnhance() * m_Skill.GetVar(8) ) + m_Skill.GetVar(9) + m_Skill.GetVar(10) * m_Skill.GetRequestedSkillLevel() + m_Skill.GetVar(11) * m_Skill.GetEnhance();

	nDamage *= m_Skill.GetVar(18) + m_Skill.GetEnhance() * m_Skill.GetVar(19);
	// }

	// 근처 유닛 리스트를 vResult에 긁어옴

	nDamage = EnumSkillTargetsAndCalcDamage( GetPos(), GetLayer(), GetPos(), false, fRange, -1, 0, nDamage,
		true, pCaster, m_Skill.GetVar(16), m_Skill.GetVar(17), vResult, false );

	StructCreature *pCreature;

	AR_TIME t = GetArTime();

	for( std::vector< StructCreature * >::iterator it = vResult.begin(); it != vResult.end(); ++it )
	{
		SkillResult skill_result;

		// 크리쳐 핸들 얻는다
		pCreature = (*it);

		if( pCreature->IsDead() )
			continue;

		// 적이면 뎀쥐 드셈
		if( pCaster->IsEnemy( pCreature, true ) )
		{
			int flag = 0;
			int elemental_type = m_Skill.GetSkillBase()->GetElementalType();
			// 데미지 주기
			StructCreature::_DAMAGE Damage = pCreature->DealMagicalDamage( m_Skill.m_pOwner, nDamage, (Elemental::Type) elemental_type, m_Skill.GetCriticalBonus( m_Skill.GetRequestedSkillLevel() ) );

			if( Damage.bCritical	)	flag |= SkillResult::CRITICAL;
			if( Damage.bBlock		)	flag |= SkillResult::BLOCK;
			if( Damage.bMiss		)	flag |= SkillResult::MISS;
			if( Damage.bPerfectBlock)	flag |= SkillResult::PERFECT_BLOCK;

			skill_result.damage.type = SkillResult::MAGIC_DAMAGE;
			skill_result.damage.damage_type = elemental_type;
			skill_result.damage.damage = Damage.nDamage;
			skill_result.damage.hTarget = pCreature->GetHandle();
			skill_result.damage.flag = flag;
			skill_result.damage.target_hp = pCreature->GetHP();

			m_Skill.m_vResultList.push_back( skill_result );

			++m_Skill.m_nTargetCount;

			if( !Damage.bMiss && pCreature->IsAlive() )
			{
				StructState::StateCode nStateCode = static_cast< StructState::StateCode >(m_Skill.GetStateId());

				if( nStateCode )
				{
					int nLevel = m_Skill.GetStateLevel( m_Skill.GetRequestedSkillLevel(), m_Skill.GetEnhance() );

					AR_TIME nDuration = m_Skill.GetStateSecond( m_Skill.GetRequestedSkillLevel(), m_Skill.GetEnhance() );
					pCreature->AddState( nStateCode, pCaster->GetHandle(), nLevel, t, t + nDuration );
				}

				if( pCreature->IsMonster() )
				{
					int nHate = Damage.nDamage;

					std::pair< float, int > HateMod = pCaster->GetHateMod( ( m_Skill.GetSkillBase()->IsPhysicalSkill() ) ? 1 : 2, m_Skill.GetSkillBase()->IsHarmful() );

					nHate += HateMod.second;
					nHate *= HateMod.first;

					static_cast< StructMonster * >( pCreature )->AddHate( m_Skill.m_pOwner->GetHandle(), m_fHateRatio * nHate );
				}
				else if( pCreature->IsNPC() )
					static_cast< StructNPC * >( pCreature )->SetAttacker( m_Skill.m_pOwner );
			}
		}
		// 아군이면 힐 드셈
		else if( pCaster->IsAlly( pCreature ) )
		{
			nHPHeal = pCreature->Heal( nHPHeal );

			skill_result.add_hp_mp_sp.type = SkillResult::ADD_HP;
			skill_result.add_hp_mp_sp.hTarget = pCreature->GetHandle();
			skill_result.add_hp_mp_sp.target_hp = pCreature->GetHP();
			skill_result.add_hp_mp_sp.target_mp = pCreature->GetMP();
			skill_result.add_hp_mp_sp.nIncHP = nHPHeal;
			skill_result.add_hp_mp_sp.nIncMP = 0;
			skill_result.add_hp_mp_sp.nIncSP = 0;

			m_Skill.m_vResultList.push_back( skill_result );

			++m_Skill.m_nTargetCount;
		}
	}
}

void StructSkillProp::FIRE_TRAP_DAMAGE( StructCreature *pCaster )
{
	float fFireRange = m_Skill.GetVar(0) * GameRule::DEFAULT_UNIT_SIZE;
	float fDamageRange = m_Skill.GetVar(5) * GameRule::DEFAULT_UNIT_SIZE;

	// 발동 반경 내의 유닛 리스트를 vResult에 긁어옴
	std::vector< StructCreature * > vTarget;

	EnumSkillTargetsAndCalcDamage( GetPos(), GetLayer(), GetPos(), true, fFireRange, -1, 0, 0,
		true, pCaster, SkillBase::DISTRIBUTION_TYPE_NO_LIMIT, 0, vTarget );

	std::vector< StructCreature * >::iterator it;
	for( it = vTarget.begin() ; it != vTarget.end() ; ++it )
	{
		if( (*it) && pCaster->IsEnemy( *it, true ) )
			break;
	}
	if( vTarget.empty() || it == vTarget.end() )
		return;

	// { 데미지 계산
	int elemental_type = m_Skill.GetSkillBase()->GetElementalType();
	int nDamage = m_Skill.GetVar(1) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(2);
	// }

	AR_TIME t = GetArTime();

	// 근처 유닛 리스트를 vTargetList에 긁어오고, 데미지 계산
	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	// 분산 유형 바꿀 것 -> SkillBase::DISTRIBUTION_TYPE_DISTRIBUTE
	nDamage = EnumSkillTargetsAndCalcDamage( GetPos(), GetLayer(), GetPos(), true, fDamageRange, -1, 0, nDamage,
		true, pCaster, SkillBase::DISTRIBUTION_TYPE_NO_LIMIT, m_Skill.GetVar(6), vTargetList );

	// 데미지 주기
	for( itTargetList = vTargetList.begin() ; itTargetList != vTargetList.end() ; ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		switch( m_Skill.GetSkillBase()->GetSkillEffectType() )
		{
		case SkillBase::EF_TRAP_PHYSICAL_DAMAGE:
			{
				StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( pCaster, nDamage, (Elemental::Type)elemental_type, m_Skill.GetHitBonus( m_Skill.GetEnhance(), pCaster->GetLevel() - pDealTarget->GetLevel() ), m_Skill.GetCriticalBonus( m_Skill.GetRequestedSkillLevel() ) );

				AddSkillDamageResult( &m_Skill.m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
			}
			break;
		case SkillBase::EF_TRAP_MAGICAL_DAMAGE:
			{
				StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( pCaster, nDamage, (Elemental::Type)elemental_type, m_Skill.GetHitBonus( m_Skill.GetEnhance(), pCaster->GetLevel() - pDealTarget->GetLevel() ), m_Skill.GetCriticalBonus( m_Skill.GetRequestedSkillLevel() ) );

				AddSkillDamageResult( &m_Skill.m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
			}
			break;
		}

		++m_Skill.m_nTargetCount;

		// 확률로 상태이상 부여
		if( XRandom( 0, 99 ) < m_Skill.GetProbabilityOnHit( m_Skill.GetRequestedSkillLevel() ) )
		{
			int nLevel = m_Skill.GetStateLevel( m_Skill.GetRequestedSkillLevel(), m_Skill.GetEnhance() );
			AR_TIME nDuration = m_Skill.GetStateSecond( m_Skill.GetRequestedSkillLevel(), m_Skill.GetEnhance() );
			StructState::StateCode nStateCode = static_cast< StructState::StateCode >(m_Skill.GetStateId());
			(*itTargetList)->AddState( nStateCode, pCaster->GetHandle(), nLevel, t, t + nDuration );

			AddSkillResult( &m_Skill.m_vResultList, true, SkillResult::ADD_STATE, (*itTargetList)->GetHandle() );
			++m_Skill.m_nTargetCount;
		}
	}

	// 개채를 여기서 삭제하면 RemoveObject의 방송을 위한 락과 여기 onProcess에서의 락이 중첩되어 서버가 멎음
	// m_Info.m_nEndTime을 현재로 세팅하면 onProcess에서 알아서 없애줌
	//m_bProcessEnd = true;
	//ArcadiaServer::Instance().DeleteObject( this );
	m_Info.m_nEndTime = t;

	return;
}

void StructSkillProp::FIRE_TRAP_MULTIPLE_DAMAGE( StructCreature *pCaster )
{
	float fFireRange = m_Skill.GetVar(0) * GameRule::DEFAULT_UNIT_SIZE;
	float fDamageRange = m_Skill.GetVar(5) * GameRule::DEFAULT_UNIT_SIZE;

	if( !m_bFired )
	{
		// 발동 반경 내의 유닛 리스트를 vResult에 긁어옴
		std::vector< StructCreature * > vTarget;

		EnumSkillTargetsAndCalcDamage( GetPos(), GetLayer(), GetPos(), true, fFireRange, -1, 0, 0,
			true, pCaster, SkillBase::DISTRIBUTION_TYPE_NO_LIMIT, 0, vTarget );

		std::vector< StructCreature * >::iterator it;
		for( it = vTarget.begin() ; it != vTarget.end() ; ++it )
		{
			if( (*it) && pCaster->IsEnemy( *it, true ) )
				break;
		}
		if( vTarget.empty() || it == vTarget.end() )
			return;

		AR_TIME t = GetArTime();
		AR_TIME nFireEndTime = t + ( m_Skill.GetVar(7) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(8) ) * 100;

		m_bFired = true;
		m_Info.m_nInterval = m_Skill.GetVar(9) * 100;
		// 밟았으면 지속시간만큼만 지속되고 없어져야 함
		// 밟은 후 지속시간이 10초인데 트랩 지속시간이 1초 남았어도 10초간 지속되며 데미지를 모두 줘야 함
		m_Info.m_nEndTime = nFireEndTime;
	}

	// { 데미지 계산
	int elemental_type = m_Skill.GetSkillBase()->GetElementalType();
	int nDamage = m_Skill.GetVar(1) + m_Skill.GetRequestedSkillLevel() * m_Skill.GetVar(2);
	// }

	AR_TIME t = GetArTime();

	// 근처 유닛 리스트를 vTargetList에 긁어오고, 데미지 계산
	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	nDamage = EnumSkillTargetsAndCalcDamage( GetPos(), GetLayer(), GetPos(), true, fDamageRange, -1, 0, nDamage,
		true, pCaster, SkillBase::DISTRIBUTION_TYPE_DISTRIBUTE, m_Skill.GetVar(6), vTargetList );

	// 데미지 주기
	for( itTargetList = vTargetList.begin() ; itTargetList != vTargetList.end() ; ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		switch( m_Skill.GetSkillBase()->GetSkillEffectType() )
		{
		case SkillBase::EF_TRAP_MULTIPLE_PHYSICAL_DAMAGE:
			{
				StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( pCaster, nDamage, (Elemental::Type)elemental_type, m_Skill.GetHitBonus( m_Skill.GetEnhance(), pCaster->GetLevel() - pDealTarget->GetLevel() ), m_Skill.GetCriticalBonus( m_Skill.GetRequestedSkillLevel() ) );

				AddSkillDamageResult( &m_Skill.m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
			}
			break;
		case SkillBase::EF_TRAP_MULTIPLE_MAGICAL_DAMAGE:
			{
				StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( pCaster, nDamage, (Elemental::Type)elemental_type, m_Skill.GetHitBonus( m_Skill.GetEnhance(), pCaster->GetLevel() - pDealTarget->GetLevel() ), m_Skill.GetCriticalBonus( m_Skill.GetRequestedSkillLevel() ) );

				AddSkillDamageResult( &m_Skill.m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
			}
			break;
		}

		++m_Skill.m_nTargetCount;

		// 확률로 상태이상 부여
		if( XRandom( 0, 99 ) < m_Skill.GetProbabilityOnHit( m_Skill.GetRequestedSkillLevel() ) )
		{
			int nLevel = m_Skill.GetStateLevel( m_Skill.GetRequestedSkillLevel(), m_Skill.GetEnhance() );
			AR_TIME nDuration = m_Skill.GetStateSecond( m_Skill.GetRequestedSkillLevel(), m_Skill.GetEnhance() );
			StructState::StateCode nStateCode = static_cast< StructState::StateCode >(m_Skill.GetStateId());
			(*itTargetList)->AddState( nStateCode, pCaster->GetHandle(), nLevel, t, t + nDuration );

			AddSkillResult( &m_Skill.m_vResultList, true, SkillResult::ADD_STATE, (*itTargetList)->GetHandle() );
			++m_Skill.m_nTargetCount;
		}
	}
}
