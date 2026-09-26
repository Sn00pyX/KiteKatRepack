
#include <mmo/ArcadiaServer.h>


#include "StructCreature.h"
#include "StructPlayer.h"	// 흠 ;;
#include "StructNPC.h"
#include "StructMonster.h"
#include "StructSkill.h"
#include "StructSkillProp.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructItem.h"
#include "StructProc.h"
#include "CreatureBase.h"
#include "SendMessage.h"
#include "GameContent.h"

// 현재 CalculateStat 관련 로직이 Struct~~에 들어가 있는 당위성은 멤버 변수에 접근하기 편하다는 점 말고는 아무 것도 없다
// 최종적으로는 CalculateStat과 Struct~~를 구분 짓는 것이 좋겠지만 워낙에 덩치가 커서 당장은 힘들다
// 우선 이 둘을 물리적인 cpp파일로 구분하여 코드 파악 과정을 조금 더 간결하게 해봅시다

// --- 초기화 관련 루틴 ---
void StructCreature::CalculateStat()
{
	int prev_max_hp = GetMaxHP();
	int prev_max_mp = GetMaxMP();
	int prev_hp = GetHP();
	int prev_mp = GetMP();

	std::vector< std::pair< int, int > > prevAddedSkillBySkillId;
	std::vector< std::pair< int, int > > prevAddedSkillBySkillType;
	prevAddedSkillBySkillId.swap( m_vAddedSkillBySkillId );
	prevAddedSkillBySkillType.swap( m_vAddedSkillBySkillType );

	// 일단 모든것 초기화
	m_nLuna = 0;
	m_fHPRegenMod = 1.0f;
	m_fMPRegenMod = 1.0f;
	m_nMaxHP = 0;
	m_nMinHP = 0;
	m_nMinHP = 0.0f;
	m_fMaxHPAmplifier = 0.0f;
	m_nMaxMP = 0;
	m_fMaxMPAmplifier = 0.0f;

	m_fEXPMod = 0.0f;

	m_fHealRatio = 1.0f;
	m_fMPHealRatio = 1.0f;
	m_fHealRatioByItem = 1.0f;
	m_fMPHealRatioByItem = 1.0f;
	m_fHealRatioByRest = 10.0f;
	m_fMPHealRatioByRest = 2.0f;

	m_nAdditionalHeal = 0;
	m_nAdditionalMPHeal = 0;
	m_nAdditionalHealByItem = 0;
	m_nAdditionalMPHealByItem = 0;
	m_nAdditionalHealByRest = 0;
	m_nAdditionalMPHealByRest = 0;

	m_fHateRatio = 1.0f;
	m_fSummonFCM = 0;
	m_fResistHarmfulState = 0.0f;
	m_mapResistHarmfulState.clear();

	m_nCastKeep = 100;
	m_fBattleLevel = m_nLevel;
	m_fBowInterval = 0.0f;
	if( IsSummon() )
		m_nMaxEnergy = 5;
	else
		m_nMaxEnergy = 0;
	m_nEnergyUpkeepTime = GameRule::DEFAULT_ENERGY_UPKEEP_TIME;
	m_nEnergyUnconsumptionRate = 0;

	//m_nAttackPointRightByWeapon = 0;
	//m_nAttackPointRightByWeaponMastery = 0;
	//m_nAttackPointLeftByWeapon = 0;
	//m_nAttackPointLeftByWeaponMastery = 0;
	m_fAttackPointRightWithoutWeapon = 0;
	m_fAttackPointLeftWithoutWeapon = 0;

	m_fCreatureCardChance = 1.0f;

	m_vPendStateListByItem.clear();

	m_fPhysicalDamageManaShieldAbsorbRatio = 0.0f;
	m_fMagicalDamageManaShieldAbsorbRatio = 0.0f;

	m_vAmplifyPassiveSkillList.clear();

	m_nChangingGroup = CREATURE_NONE;

	m_vInterruptedSkill.clear();
	m_vAllowedSkill.clear();

	m_vAddedSkillBySkillId.clear();
	m_vAddedSkillBySkillType.clear();

	m_vAddHPMPOnCritical.clear();
	initProc();

	m_vDamageReducePercentInfo.clear();
	m_vDamageReduceValueInfo.clear();

	m_SkillCostModifier.init();

	m_NormalStateAdvantage.Init();
	m_RangeStateAdvantage.Init();
	m_NormalStatePenalty.Init();
	m_RangeStatePenalty.Init();
	m_PhysicalSkillStatePenalty.Init();
	m_MagicalSkillStatePenalty.Init();
	m_StateStatePenalty.Init();
	m_fPassivePhysicalReflect = 0; 

	for( int i = 0; i < Elemental::TYPE_COUNT; ++i )
	{
		m_GoodPhysicalElementalSkillStateMod[i].Init();
		m_BadPhysicalElementalSkillStateMod[i].Init();
		m_GoodMagicalElementalSkillStateMod[i].Init();
		m_BadMagicalElementalSkillStateMod[i].Init();
	}

	m_nElementalEffectAttackPointRight = 0;
	m_nElementalEffectAttackPointLeft = 0;
	m_nElementalEffectMagicPoint = 0;

	m_StatusFlag.On( STATUS_MOVABLE );
	m_StatusFlag.On( STATUS_ATTACKABLE );
	m_StatusFlag.On( STATUS_SKILL_CASTABLE );
	m_StatusFlag.On( STATUS_MAGIC_CASTABLE );
	m_StatusFlag.On( STATUS_ITEM_USABLE );
	m_StatusFlag.On( STATUS_MORTAL );

	m_StatusFlag.Off( STATUS_FEARED );
	m_StatusFlag.Off( STATUS_FORM_CHANGED );
	m_StatusFlag.Off( STATUS_MOVE_SPEED_FIXED );
	m_StatusFlag.Off( STATUS_HIDING );
	AR_UNIT fPrevDetectHideRange = m_fDetectHideRange;
	m_fDetectHideRange = 0.0f;

	m_StatusFlag.Off( STATUS_HP_REGEN_STOPPED );
	m_StatusFlag.Off( STATUS_MP_REGEN_STOPPED );

	m_nDoubleWeaponMasteryLevel = 0;

	// 지속효과에 의한 증가량 계산용
	CreatureStatServer			stateStat;
	CreatureAttributeServer		stateAttr;

	// 어트리뷰트와 레지스트는 0으로 설정하고..
	memset( &stateStat, 0, sizeof( stateStat ) );
	memset( &stateAttr, 0, sizeof( stateAttr ) );
	memset( &m_StatByState, 0, sizeof( m_StatByState ) );
	memset( &m_StatAmplifier, 0, sizeof( m_StatAmplifier ) );
	memset( &m_AttributeByState, 0, sizeof( m_AttributeByState ) );
	memset( &m_AttributeAmplifier, 0, sizeof( m_AttributeAmplifier ) );
	memset( &m_Attribute, 0, sizeof( m_Attribute ) );
	memset( &m_Resist, 0, sizeof( m_Resist ) );
	memset( &m_ResistAmplifier, 0, sizeof( m_ResistAmplifier ) );
	memset( &m_Expert, 0, sizeof( m_Expert ) );
	memset( &m_ElementalStateDamageAdder, 0, sizeof( m_ElementalStateDamageAdder ) );
	memset( &m_ElementalStateDamageAmplifier, 0, sizeof( m_ElementalStateDamageAmplifier ) );

	m_vNormalAdditionalDamage.clear();
	m_vRangeAdditionalDamage.clear();
	m_vPhysicalSkillAdditionalDamage.clear();
	m_vMagicalSkillAdditionalDamage.clear();
	m_vDamageReflectInfo.clear();
	m_vStateReflectInfo.clear();
	m_vHateMod.clear();
	m_vSetItemEffect.clear();
	m_vSynchronizedParameter.clear();

	m_vSkillRelatedToDamaging.clear();

	for ( std::vector< StructSkill* >::iterator it = m_vAllSkillList.begin(); it != m_vAllSkillList.end(); ++it )
	{
		(*it)->GetCostModifier().init();
	}

	// 우선 기본 스탯 얻어온다	
	m_Stat = GetBaseStat();

	onBeforeCalculateStat();

	// 세트 아이템에 의한 성능을 체크 합니다.
	checkAdditionalItemEffect();

	// 스탯을 변경시킬 수 있는 아이템 효과 적용한다.
	// 호칭의 성능 역시 아이템 성능으로 동작하기 때문에 여기서 처리한다.
	applyStatByItem();
	applyStatByTitle();
	applyStatByPassiveSkill();

	// Job Level Bonus 적용
	applyJobLevelBonus();

	stateStat = m_Stat;
	// 스탯을 변경시킬 수 있는 상태이상 효과 적용하고
	applyStatByState();

	accumulateStatDiffByState( stateStat ); // 지속 효과에 의한 스탯 증폭치 누적

	onApplyStat();				// 증폭 전에 처리해야 할 것들(벨트 장착 관련)

	amplifyStatByItem();
	amplifyStatByTitle();
	amplifyStatByPassiveSkill();

	// 지속효과에 의한 증폭치 추가 전에 미리 스텟 계산해 봄
	stateStat = m_Stat;
	getAmplifiedStatByAmplifier( &stateStat );

	// 증폭도 적용시키고
	amplifyStatByState();

	// 스텟 증폭치 처리
	getAmplifiedStatByAmplifier( &m_Stat );

	// 스탯 동기화
	onSynchronizeParameter( true );
	accumulateStatDiffByState( stateStat ); // 지속 효과에 의한 스탯 증폭치 누적

	// 현재 계산 방식에서는 파라미터 기반 계산은 두번 이루어지는데, 두 번 모두 이 계산 이전의 파라미터를 참조해야 한다.
	// 헌데 지금의 구조를 유지하며 계산을 수행하려면 임시 변수에 저장해놨다가 이를 계산에 사용하는 방법 밖에 없다.
	m_StatBeforeParamBasedCalc = m_Stat;
	calcParamBasedStat();

	// 스탯 적용이 끝났음을 알림
	onAfterApplyStat();

	// 0 이하로 떨어진 값들 정리
	finalizeStat();
	// !!! 이 밑으로는 스탯이 변경되면 안됨.

	// hp/mp 설정
	m_nMaxHP += GameRule::GetMaxHP( GetVital(), GetBattleLevel(), GetFCM() );
	m_nMaxMP += GameRule::GetMaxMP( GetIntelligence(), GetBattleLevel(), GetFCM() );	

	// 스탯에 따른 어트리뷰트 적용한 뒤에..
	calcAttribute( &m_Attribute );	

	// 어트리뷰트 계산 직후에 할 일: 벨트 장착 패시브 중 어트리뷰트에 영향 주는 녀석 처리
	onAfterCalculateAttributeByStat();

	// 아이템빨 적용
	applyItemEffect();

	// 호칭 성능 적용
	applyTitleEffect();

	// 그 다음에 패시브빨을 먹인다, 벨트 장착 크리처 패시브 중 스텟이 아닌 파라미터 증가도 여기서 적용
	applyPassiveSkillEffect();

	stateAttr = m_Attribute;

	// 상태이상빨 적용한다.
	applyStateEffect();

	accumulateAttributeDiffByState( stateAttr ); // 지속 효과에 의한 어트리뷰트 증폭치 누적

	//패시브 스킬에 의한 증폭 효과를 적용 합니다.
	applyPassiveSkillAmplifyEffect();

	onApplyAttributeAdjustment();	// 증폭 전에 처리해야 할 파라미터 보정치(NPC, Monster 적용)

	// 파라미터 증폭 적용
	stateAttr = m_Attribute;
	getAmplifiedAttributeByAmplifier( &stateAttr );

	applyStateAmplifyEffect();

	getAmplifiedAttributeByAmplifier( &m_Attribute );
	m_fAttackPointRightWithoutWeapon = m_AttributeAmplifier.fAttackPointRight * m_fAttackPointRightWithoutWeapon;
	m_fAttackPointLeftWithoutWeapon = m_AttributeAmplifier.fAttackPointLeft * m_fAttackPointLeftWithoutWeapon;

	// 스탯 이 외의 파라미터 동기화
	onSynchronizeParameter( false );

	accumulateAttributeDiffByState( stateAttr ); // 지속 효과에 의한 어트리뷰트 증폭치 누적

	// 현재 계산 방식에서는 파라미터 기반 계산은 두번 이루어지는데, 두 번 모두 이 계산 이전의 파라미터를 참조해야 한다.
	// 헌데 지금의 구조를 유지하며 계산을 수행하려면 임시 변수에 저장해놨다가 이를 계산에 사용하는 방법 밖에 없다.
	m_AttrBeforeParamBasedCalc = m_Attribute;
	calcParamBasedAttribute();
	applyParamBasedStateEffect();
	applyParamBasedItemEffect();

	// 속성 이펙트 추가 성능(공격력, 마력) 적용(어떠한 증폭도 적용되지 않아야 함)
	m_Attribute.fAttackPointRight += m_nElementalEffectAttackPointRight;
	m_Attribute.fAttackPointLeft += m_nElementalEffectAttackPointLeft;
	m_Attribute.fMagicPoint += m_nElementalEffectMagicPoint;

	// 양손 무기에 따른 보정 처리 한다.
	applyDoubleWeaponEffect();

	// HP, MP 최대치 조정
	m_nMaxHP *= 1.0f + m_fMaxHPAmplifier;
	m_nMaxMP *= 1.0f + m_fMaxMPAmplifier;

	// 저항 증폭 처리
	getAmplifiedResistByAmplifier( &m_Resist );

	// onModifyStatAndAttribute와 나눈 이후는, 여기서 각 파생 struct별 최종 파라미터 정리를 하는데, 그때 max_hp가 바뀔 수 있기 때문에,
	// hp 줄이기 전에 이것은 호출되어야 하고, onModifyStatAndAttribute는 최종 파라미터 전송등의 용도로 사용되어야 하기 때문이다.
	onCompleteCalculateStat();

	// 공속은 최소 10이다.
	if( m_Attribute.fAttackSpeed < 10 ) m_Attribute.fAttackSpeed = 10;

	// 이속이 최소 10이다.
	if( m_Attribute.fMoveSpeed < 10 ) m_Attribute.fMoveSpeed = 10;

	// 크리티컬은 100 이 최대
	if( m_Attribute.fCritical > 100 ) m_Attribute.fCritical = 100;

	// 회피가 음수가 되어선 안 된다.
	if( m_Attribute.fAvoid < 0 ) m_Attribute.fAvoid = 0;
	if( m_Attribute.fMagicAvoid < 0 ) m_Attribute.fMagicAvoid = 0;

	// MAX 치가 작아질경우도 있을수 있으므로 줄여준다
	AddHP( 0 );
	AddMP( 0 );

	// 기공은 최대가 10개
	if( m_nMaxEnergy > GameRule::ENERGY_MAX )
		m_nMaxEnergy = GameRule::ENERGY_MAX;

	// 라이딩 중이고 그 라이딩 특성이 '현재 속도가 라이딩 속도를 초과할 시 현재 속도로 적용'을 반영한다면
	if( IsPlayer() && m_Attribute.fRidingSpeed && static_cast< StructPlayer* >( this )->IsApplyingFasterSpeedInRiding() )
	{
		if( m_Attribute.fMoveSpeed > m_Attribute.fRidingSpeed )
			m_Attribute.fRidingSpeed = m_Attribute.fMoveSpeed;
	}

	// 핸들러 호출
	onModifyStatAndAttribute();

	if( isSkillModified( &prevAddedSkillBySkillType, &m_vAddedSkillBySkillType ) ||
		isSkillModified( &prevAddedSkillBySkillId, &m_vAddedSkillBySkillId ) )
		onModifySkill();

	if( fPrevDetectHideRange != m_fDetectHideRange )
		onModifyDetectRange();

	if( IsInWorld() && ( prev_max_hp != GetMaxHP() || prev_max_mp != GetMaxMP() || prev_hp != GetHP() || prev_mp != GetMP() ) )
	{
		BroadcastHPMPMsg( this, GetHP() - prev_hp, GetMP() - prev_mp );
	}
	else if( IsSummon() && !IsInWorld() && ( prev_max_hp != GetMaxHP() || prev_max_mp != GetMaxMP() || prev_hp != GetHP() || prev_mp != GetMP() ) )
	{
		StructPlayer * pPlayer = static_cast< StructSummon * >( this )->GetMaster();

		if( pPlayer )
			SendHPMPMsg( pPlayer, this, GetHP() - prev_hp, GetMP() - prev_mp );
	}
}

void StructCreature::initProc()
{
	// 소멸자에서 delete를 하면, 복사 연산자의 호출 시에 문제가 발생하여 Deep copy를 해야하므로 직접 delete
	std::vector< _ATTACK_TAG >::iterator attackIt;
	std::map< int, std::vector< _PROC_TAG > >::iterator mapIt;
	// std::vector< _PROC_TAG >에 대한 iterator를 멤버로 선언하면 _Clean_up_iter_debug에서 멈춰버린다. 원인은 아직 알 수 없음.
	
	for( attackIt = m_vProcByAttack.begin(); attackIt != m_vProcByAttack.end(); ++attackIt )								delete (*attackIt).proc;
	for( attackIt = m_vProcByBeingAttacked.begin(); attackIt != m_vProcByBeingAttacked.end(); ++attackIt )					delete (*attackIt).proc;
	for( std::vector< _KILL_TAG >::iterator killIt = m_vProcByKill.begin(); killIt != m_vProcByKill.end(); ++killIt )		delete (*killIt).proc;
	for( std::vector< _KILL_TAG >::iterator killIt = m_vProcByDead.begin(); killIt != m_vProcByDead.end(); ++killIt )		delete (*killIt).proc;
	for( attackIt = m_vProcByCriticalAttack.begin(); attackIt != m_vProcByCriticalAttack.end(); ++attackIt )				delete (*attackIt).proc;
	for( attackIt = m_vProcByBeingCriticalAttacked.begin(); attackIt != m_vProcByBeingCriticalAttacked.end(); ++attackIt )	delete (*attackIt).proc;
	for( attackIt = m_vProcByAvoid.begin(); attackIt != m_vProcByAvoid.end(); ++attackIt )									delete (*attackIt).proc;
	for( attackIt = m_vProcByBlock.begin(); attackIt != m_vProcByBlock.end(); ++attackIt )									delete (*attackIt).proc;
	for( attackIt = m_vProcByPerfectBlock.begin(); attackIt != m_vProcByPerfectBlock.end(); ++attackIt )					delete (*attackIt).proc;
	for( mapIt = m_mapProcBySkillId.begin(); mapIt != m_mapProcBySkillId.end(); ++mapIt )
	{
		for( std::vector< _PROC_TAG >::iterator procIt = (*mapIt).second.begin(); procIt != (*mapIt).second.end(); ++procIt )
		{
			delete (*procIt).proc;
		}
	}

	m_vProcByAttack.clear();
	m_vProcByBeingAttacked.clear();
	m_vProcByKill.clear();
	m_vProcByDead.clear();
	m_vProcByCriticalAttack.clear();
	m_vProcByBeingCriticalAttacked.clear();
	m_vProcByAvoid.clear();
	m_vProcByBlock.clear();
	m_vProcByPerfectBlock.clear();
	m_mapProcBySkillId.clear();
}

void StructCreature::calcAttribute( CreatureAttributeServer * pAttribute )
{
	pAttribute->fCriticalPower	=  80;
	pAttribute->fCritical		+= GameRule::GetCritical( GetLuck() );

	if( IsUsingBow() || IsUsingCrossBow() )
	{
		pAttribute->fAttackPointRight	+= GameRule::GetBowAttackPoint( GetAgility(), GetDexterity(), GetBattleLevel(), GetFCM() );	
	}
	else
	{
		pAttribute->fAttackPointRight	+= GameRule::GetAttackPoint( GetStrength(), GetBattleLevel(), GetFCM() );	

		if( IsUsingDoubleWeapon() )
			pAttribute->fAttackPointLeft	+= GameRule::GetAttackPoint( GetStrength(), GetBattleLevel(), GetFCM() );	
	}

	pAttribute->fMagicPoint		+= GameRule::GetMagicPoint( GetIntelligence(), GetBattleLevel(), GetFCM() );
	pAttribute->fItemChance		+= GameRule::GetItemChance( GetLuck() );
	pAttribute->fDefence		+= GameRule::GetDefence( GetBattleLevel(), GetVital(), GetFCM() );;
	pAttribute->fMagicDefence	+= GameRule::GetMagicDefence( GetBattleLevel(), GetMentality(), GetFCM() );
	pAttribute->fMaxWeight		+= GameRule::GetMaxWeight( GetLevel(), GetStrength() );

	pAttribute->fAccuracyRight		+= GameRule::GetAccuracy( GetDexterity(), GetBattleLevel(), GetFCM() );

	if( IsUsingDoubleWeapon() )
		pAttribute->fAccuracyLeft		+= GameRule::GetAccuracy( GetDexterity(), GetBattleLevel(), GetFCM() );

	pAttribute->fMagicAccuracy	+= GameRule::GetMagicAccuracy( GetMentality(), GetDexterity(), GetBattleLevel(), GetFCM() );
	pAttribute->fAvoid			+= GameRule::GetAvoid( GetAgility(), GetBattleLevel(), GetFCM() );
	pAttribute->fMagicAvoid		+= GameRule::GetMagicAvoid( GetMentality(), GetBattleLevel() , GetFCM());

	pAttribute->fAttackSpeedRight	+= GameRule::GetAttackSpeed( GetAgility() );	// 공격속도	(2010-05-07 리뉴얼 적용)

	if( IsUsingDoubleWeapon() )
		pAttribute->fAttackSpeedLeft	+= GameRule::GetAttackSpeed( GetAgility() );	// 공격속도	(2010-05-07 리뉴얼 적용)

	if( !isMoveSpeedFixed() )
	{
		pAttribute->fMoveSpeed		+= GameRule::GetBaseMoveSpeed();	// 기본 이동속도	(2007-03-24 리뉴얼 적용)
	}
	pAttribute->fCastingSpeed	+= GameRule::GetCastingTime( GetDexterity(), GetMentality(), GetBattleLevel() );
	pAttribute->fCoolTimeSpeed	= 100;
	pAttribute->fHPRegenPercentage	+= GameRule::GetHPRegenPercentage( GetFCM() );
	pAttribute->fHPRegenPoint		+= GameRule::GetHPRegenPoint( GetBattleLevel() );
	pAttribute->fMPRegenPercentage	+= GameRule::GetMPRegenPercentage( GetFCM() );
	pAttribute->fMPRegenPoint		+= GameRule::GetMPRegenPoint( GetBattleLevel(), GetMentality() );
	pAttribute->fPerfectBlock		+= GameRule::GetPerfectBlock();

	// 사거리가 없으면 기본사거리 적용
	if( !pAttribute->fAttackRange ) pAttribute->fAttackRange	= GameRule::DEFAULT_ATTACK_RANGE * GameRule::ATTACK_RANGE_UNIT;
}


void StructSummon::calcAttribute( CreatureAttributeServer* pAttribute )
{
	StructCreature::calcAttribute( pAttribute );

	int nCorrection = 0;

	if( GetMaster() )
	{
		// (2007-03-24 리뉴얼 적용)
		nCorrection = ( 2 * GetMaster()->GetLevel() - GetLevel() ) / GetLevel();
	}
	
	pAttribute->fAccuracyRight		+= nCorrection;

	if( IsUsingDoubleWeapon() )
		pAttribute->fAccuracyLeft	+= nCorrection;

	pAttribute->fMagicAccuracy		+= nCorrection;
}

// --- 스탯 및 능력치 연산에 관련된 패시브 스킬들 적용하는 루틴 ---
void StructCreature::applyStatByPassiveSkill()
{
	if( m_vPassiveSkillList.empty() ) return;

	std::vector< StructSkill *>::iterator it;

	for( it = m_vPassiveSkillList.begin(); it != m_vPassiveSkillList.end(); ++it )
	{
		StructSkill * pSkill = (*it);

		// 디버프, 랜덤 스킬초기화로 인해 현재 스킬 레벨이 0 이하인 경우 효과가 적용되지 않는다.
		if( pSkill->GetCurrentSkillLevel() <= 0 )
			continue;

		if( pSkill->GetSkillBase()->IsNeedWeapon() )
		{
			// 스킬에 적합한 무기인지?
			if( pSkill->IsUseableWeapon( ItemBase::CLASS_SHIELD ) )
			{
				if( !IsWearShield() )
					continue;
			}
			else if( !pSkill->IsUseableWeapon( GetWeaponClass() ) )
			{
				continue;
			}
		}

		if ( pSkill->GetSkillBase()->GetEffectType() == SkillBase::EF_PARAMETER_INC ) 
		{
			incParameter( pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), true );
			incParameter( pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), true );
			incParameter( pSkill->GetVar( 12 ), pSkill->GetVar( 13 ) + pSkill->GetVar( 14 ) * pSkill->GetCurrentSkillLevel(), true );
			incParameter( pSkill->GetVar( 15 ), pSkill->GetVar( 16 ) + pSkill->GetVar( 17 ) * pSkill->GetCurrentSkillLevel(), true );
		}
		else if ( pSkill->GetSkillBase()->GetEffectType() == SkillBase::EF_PARAMETER_AMP )
		{
			m_vAmplifyPassiveSkillList.push_back( pSkill );
		}
	}
}

void StructCreature::amplifyStatByPassiveSkill()
{
	if( m_vAmplifyPassiveSkillList.empty() ) return;

	std::vector< StructSkill *>::iterator it;

	for( it = m_vAmplifyPassiveSkillList.begin(); it != m_vAmplifyPassiveSkillList.end(); ++it )
	{
		StructSkill * pSkill = (*it);

		// 디버프, 랜덤 스킬초기화로 인해 현재 스킬 레벨이 0 이하인 경우 효과가 적용되지 않는다.
		if( pSkill->GetCurrentSkillLevel() <= 0 )
			continue;

		if ( pSkill->GetSkillBase()->GetEffectType() == SkillBase::EF_PARAMETER_AMP ) 
		{
			ampParameter( pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), true );
			ampParameter( pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), true );
			ampParameter( pSkill->GetVar( 12 ), pSkill->GetVar( 13 ) + pSkill->GetVar( 14 ) * pSkill->GetCurrentSkillLevel(), true );
			ampParameter( pSkill->GetVar( 15 ), pSkill->GetVar( 16 ) + pSkill->GetVar( 17 ) * pSkill->GetCurrentSkillLevel(), true );
		}
	}
}

void StructCreature::applyPassiveSkillEffect()
{
	if( m_vPassiveSkillList.empty() ) return;

	std::vector< StructSkill *>::iterator it;

	for( it = m_vPassiveSkillList.begin(); it != m_vPassiveSkillList.end(); ++it )
	{
		StructSkill * pSkill = (*it);

		// 디버프, 랜덤 스킬초기화로 인해 현재 스킬 레벨이 0 이하인 경우 효과가 적용되지 않는다.
		if( pSkill->GetCurrentSkillLevel() <= 0 )
			continue;

		if( pSkill->GetSkillBase()->IsNeedWeapon() )
		{
			// 스킬에 적합한 무기인지?
			if( pSkill->IsUseableWeapon( ItemBase::CLASS_SHIELD ) )
			{
				if( !IsWearShield() )
					continue;
			}
			else if( !pSkill->IsUseableWeapon( GetWeaponClass() ) )
			{
				continue;
			}
		}

		applyPassiveSkillEffect( pSkill );
	}
}

void StructSummon::applyPassiveSkillEffect()
{
	StructCreature::applyPassiveSkillEffect();

	m_nMaxHP += m_ParameterForSummon.nMaxHP;
	m_nMaxMP += m_ParameterForSummon.nMaxMP;
	m_nMaxSP += m_ParameterForSummon.nMaxSP;
	m_Attribute.fHPRegenPoint += m_ParameterForSummon.Attribute.fHPRegenPoint;
	m_Attribute.fMPRegenPoint += m_ParameterForSummon.Attribute.fHPRegenPoint;
	m_nSPRegenAdd += m_ParameterForSummon.nSPRegenAdd;
}

void StructCreature::applyPassiveSkillEffect( struct StructSkill * pSkill )
{
	switch( pSkill->GetSkillBase()->GetSkillEffectType() )
	{
		//case SkillBase::EF_DOUBLE_WEAPON:
		//	{
		//		// 눈물을 머금고...따로 처리.ㅜ.ㅜ
		//	}
		//	break;

	case SkillBase::EF_PARAMETER_INC:
		{
			incParameter( pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), false );
			incParameter( pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), false );
			incParameter2( pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
			incParameter2( pSkill->GetVar( 9 ), pSkill->GetVar( 10 ) + pSkill->GetVar( 11 ) * pSkill->GetCurrentSkillLevel() );
			incParameter( pSkill->GetVar( 12 ), pSkill->GetVar( 13 ) + pSkill->GetVar( 14 ) * pSkill->GetCurrentSkillLevel(), false );
			incParameter( pSkill->GetVar( 15 ), pSkill->GetVar( 16 ) + pSkill->GetVar( 17 ) * pSkill->GetCurrentSkillLevel(), false );
			break;
		}
	case SkillBase::EF_WEAPON_MASTERY:
		{
			StructItem * pWeapon = GetWearedItem( ItemBase::WEAR_WEAPON );

			if( pSkill->GetSkillId() == StructSkill::SKILL_ADV_WEAPON_EXPERT )	// 전문 무기 숙련은 2랭크 이상에만 적용되게 예외처리
			{
				if( !pWeapon || pWeapon->GetItemRank() < 2 )
				{
					break;
				}
			}

			c_fixed10 fAP = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fSpeed = pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fAccuracy = pSkill->GetVar(4) + pSkill->GetVar(5) * pSkill->GetCurrentSkillLevel();

			m_Attribute.fAttackPointRight += fAP;
			m_Attribute.fAttackSpeedRight += fSpeed;
			m_Attribute.fAccuracyRight += fAccuracy;

			//m_nAttackPointRightByWeaponMastery	+= nAP;

			if( IsUsingDoubleWeapon() )
			{
				m_Attribute.fAttackPointLeft += fAP;
				m_Attribute.fAttackSpeedLeft += fSpeed;
				m_Attribute.fAccuracyLeft += fAccuracy;

				//m_nAttackPointLeftByWeaponMastery += nAP;
			}

			m_Attribute.fMagicPoint += pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel();
			m_Attribute.fCritical += pSkill->GetVar( 8 ) + pSkill->GetVar( 9 ) * pSkill->GetCurrentSkillLevel();
		}
		break;

	case SkillBase::EF_BATTLE_PARAMTER_INCREASE:
		{
			c_fixed10 fAP = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fDefence = pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fMP = pSkill->GetVar(4) + pSkill->GetVar(5) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fMD = pSkill->GetVar(6) + pSkill->GetVar(7) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fAvoid = pSkill->GetVar(8) + pSkill->GetVar(9) * pSkill->GetCurrentSkillLevel();

			m_Attribute.fAttackPointRight += fAP;
			m_fAttackPointRightWithoutWeapon += fAP;

			if( IsUsingDoubleWeapon() )
			{
				m_Attribute.fAttackPointLeft += fAP;
				m_fAttackPointLeftWithoutWeapon += fAP;
			}

			m_Attribute.fDefence += fDefence;
			m_Attribute.fMagicPoint += fMP;
			m_Attribute.fMagicDefence += fMD;
			m_Attribute.fAvoid += fAvoid;
		}
		break;

	case SkillBase::EF_BLOCK_INCREASE:
		{
			m_Attribute.fBlockChance += pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			m_Attribute.fBlockDefence += pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
		}
		break;

	case SkillBase::EF_ATTACK_RANGE_INCREASE:
		{
			m_Attribute.fAttackRange += ( pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel() ) * GameRule::ATTACK_RANGE_UNIT;
		}
		break;

	case SkillBase::EF_RESISTANCE_INCREASE:
		{
			m_Attribute.fStunResistance += pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			m_Attribute.fMoveSpeedDecreaseResistance += pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
		}
		break;

	case SkillBase::EF_INCREASE_BASE_ATTRIBUTE:
		{
			int nSkillLevel = pSkill->GetCurrentSkillLevel();

			m_Attribute.fAttackPointRight += pSkill->GetVar( 0 ) * nSkillLevel;
			m_fAttackPointRightWithoutWeapon += pSkill->GetVar( 0 ) * nSkillLevel;
			if( IsUsingDoubleWeapon() )
			{
				m_Attribute.fAttackPointLeft		+= pSkill->GetVar( 0 ) * nSkillLevel;
				m_fAttackPointLeftWithoutWeapon		+= pSkill->GetVar( 0 ) * nSkillLevel;
			}

			m_Attribute.fDefence += pSkill->GetVar( 1 ) * nSkillLevel;

			m_Attribute.fMagicPoint			+= pSkill->GetVar( 2 ) * nSkillLevel;
			m_Attribute.fMagicDefence		+= pSkill->GetVar( 3 ) * nSkillLevel;

			m_Attribute.fAttackSpeedRight	+= pSkill->GetVar( 4 ) * nSkillLevel;
			if( IsUsingDoubleWeapon() )
			{
				m_Attribute.fAttackSpeedLeft	+= pSkill->GetVar( 4 ) * nSkillLevel;
			}

			if( !isMoveSpeedFixed() )		// fRidingSpeed 수치가 생기면서 라이딩 중에도 기본 속도에 관여할 수 있게 변경 ( 2013-12-20 )
			{
				m_Attribute.fMoveSpeed			+= pSkill->GetVar( 5 ) * nSkillLevel;
			}

			m_Attribute.fAccuracyRight		+= pSkill->GetVar( 6 ) * nSkillLevel;
			if( IsUsingDoubleWeapon() )
			{
				m_Attribute.fAccuracyRight	+= pSkill->GetVar( 6 ) * nSkillLevel;
			}

			m_Attribute.fMagicAccuracy		+= pSkill->GetVar( 7 ) * nSkillLevel;

			m_Attribute.fAvoid				+= pSkill->GetVar( 8 ) * nSkillLevel;
			m_Attribute.fMagicAvoid			+= pSkill->GetVar( 9 ) * nSkillLevel;
		}
		break;

	case SkillBase::EF_AMPLIFY_BASE_ATTRIBUTE_OLD:
	case SkillBase::EF_AMPLIFY_BASE_ATTRIBUTE:
		{
			int nSkillLevel = pSkill->GetCurrentSkillLevel();

			m_AttributeAmplifier.fAttackPointRight += pSkill->GetVar(0) * nSkillLevel;
			m_AttributeAmplifier.fAttackSpeedRight += pSkill->GetVar(4) * nSkillLevel;
			m_AttributeAmplifier.fAccuracyRight += pSkill->GetVar(6) * nSkillLevel;

			if( IsUsingDoubleWeapon() )
			{
				m_AttributeAmplifier.fAttackPointLeft += pSkill->GetVar(0) * nSkillLevel;
				m_AttributeAmplifier.fAttackSpeedLeft += pSkill->GetVar(4) * nSkillLevel;
				m_AttributeAmplifier.fAccuracyLeft += pSkill->GetVar(6) * nSkillLevel;
			}

			m_AttributeAmplifier.fDefence += pSkill->GetVar(1) * nSkillLevel;
			m_AttributeAmplifier.fMagicPoint += pSkill->GetVar(2) * nSkillLevel;
			m_AttributeAmplifier.fMagicDefence += pSkill->GetVar(3) * nSkillLevel;
			m_AttributeAmplifier.fMoveSpeed += pSkill->GetVar(5) * nSkillLevel;
			m_AttributeAmplifier.fMagicAccuracy += pSkill->GetVar(7) * nSkillLevel;
			m_AttributeAmplifier.fAvoid += pSkill->GetVar(8) * nSkillLevel;
			m_AttributeAmplifier.fMagicAvoid += pSkill->GetVar(9) * nSkillLevel;
		}
		break;

	case SkillBase::EF_INCREASE_EXTENSION_ATTRIBUTE:
		{
			int nSkillLevel = pSkill->GetCurrentSkillLevel();

			m_Attribute.fHPRegenPercentage		+= pSkill->GetVar( 0 ) * nSkillLevel;
			m_Attribute.fHPRegenPoint			+= pSkill->GetVar( 1 ) * nSkillLevel;
			m_Attribute.fMPRegenPercentage		+= pSkill->GetVar( 2 ) * nSkillLevel;
			m_Attribute.fMPRegenPoint			+= pSkill->GetVar( 3 ) * nSkillLevel;
			m_Attribute.fBlockChance			+= pSkill->GetVar( 4 ) * nSkillLevel;
			m_Attribute.fBlockDefence			+= pSkill->GetVar( 5 ) * nSkillLevel;
			m_Attribute.fCritical				+= pSkill->GetVar( 6 ) * nSkillLevel;
			m_Attribute.fCriticalPower			+= pSkill->GetVar( 7 ) * nSkillLevel;
			m_Attribute.fCastingSpeed			+= pSkill->GetVar( 8 ) * nSkillLevel;
			// 쿨타임 스피드는 작아야 빠른거다(-┏ 이러고도 스피드냐;;)
			m_Attribute.fCoolTimeSpeed			-= pSkill->GetVar( 9 ) * nSkillLevel;
			// 화살 데미지 감소율??
		}
		break;

	case SkillBase::EF_MAGIC_REGISTANCE_INCREASE:
		{
			int nElemental;
			int nElementalInc;

			for( int i = 0; i < 4; ++i )
			{
				nElemental = pSkill->GetVar( 3 * i);
				nElementalInc = pSkill->GetVar( 3 * i + 1 ) + pSkill->GetVar( 3 * i + 2 ) * pSkill->GetCurrentSkillLevel();

				m_Resist.nResist[ nElemental ] += nElementalInc;
			}
		}
		break;

	case SkillBase::EF_SPECIALIZE_ARMOR:
		{
			if( GetArmorClass() != pSkill->GetVar( 0 ) )
				break;

			m_Attribute.fAccuracyRight += pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel();

			if( IsUsingDoubleWeapon() )
				m_Attribute.fAccuracyLeft += pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel();

			m_Attribute.fAvoid += pSkill->GetVar( 3 ) + pSkill->GetVar( 4 ) * pSkill->GetCurrentSkillLevel();
			m_Attribute.fMPRegenPercentage += pSkill->GetVar( 5 ) + pSkill->GetVar( 6 ) * pSkill->GetCurrentSkillLevel();
		}
		break;
	case SkillBase::EF_SPECIALIZE_ARMOR_AMP:
		{
			if( GetArmorClass() != pSkill->GetVar( 0 ) )
				break;

			m_AttributeAmplifier.fAttackSpeedRight += pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel();
			if( IsUsingDoubleWeapon() )
				m_AttributeAmplifier.fAttackSpeedLeft += pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel();

			m_AttributeAmplifier.fCritical += pSkill->GetVar( 3 ) + pSkill->GetVar( 4 ) * pSkill->GetCurrentSkillLevel();

			m_fMaxHPAmplifier += pSkill->GetVar( 5 ) + pSkill->GetVar( 6 ) * pSkill->GetCurrentSkillLevel();
			m_fMaxMPAmplifier += pSkill->GetVar( 11 ) + pSkill->GetVar( 12 ) * pSkill->GetCurrentSkillLevel();
			m_AttributeAmplifier.fHPRegenPercentage += pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel();
			m_AttributeAmplifier.fMPRegenPercentage += pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel();
		}
		break;

		// 삭제된 유형
	case SkillBase::EF_MAGIC_TRAINING:
		{
			int nCastSpeed = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fCoolTime = pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
			int nCastKeep = pSkill->GetVar(4) + pSkill->GetVar(5) * pSkill->GetCurrentSkillLevel();

			m_Attribute.fCastingSpeed += nCastSpeed;
			m_Attribute.fCoolTimeSpeed -= m_Attribute.fCoolTimeSpeed * fCoolTime;
			m_nCastKeep += nCastKeep;
		}
		break;

	case SkillBase::EF_HUNTING_TRAINING:
		{
			for( int i = 0; i < 3; i++ )
			{
				int nCreatureType = pSkill->GetVar( i * 5 );

				c_fixed10 fDamage = pSkill->GetVar( i * 5 + 1 ) + pSkill->GetVar( i * 5 + 2 ) * pSkill->GetCurrentSkillLevel();
				c_fixed10 fAvoid = pSkill->GetVar( i * 5 + 3 ) + pSkill->GetVar( i * 5 + 4 ) * pSkill->GetCurrentSkillLevel();

				if( nCreatureType == CREATURE_ALL )
				{
					for( int i = 0 ; i < MAX_CREATURE_TYPE_NUMBER ; ++i )
					{
						m_Expert[ i ].fDamage += fDamage;
						m_Expert[ i ].fAvoid += fAvoid;
					}
				}
				else if( nCreatureType >= 0 && nCreatureType < MAX_CREATURE_TYPE_NUMBER )
				{
					m_Expert[ nCreatureType ].fDamage += fDamage;
					m_Expert[ nCreatureType ].fAvoid += fAvoid;
				}
				else
				{
					assert( 0 && "CREATURE DATA TYPE ERROR!" );
				}
			}

		}
		break;

	case SkillBase::EF_BOW_TRAINING:
		{
			// TODO : 나중에 처리
		}
		break;

	case SkillBase::EF_INCREASE_HP_MP:
		{
			int nMaxHPInc = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			int nMaxMPInc = pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
			int nHPRegenInc = pSkill->GetVar(6) + pSkill->GetVar(7) * pSkill->GetCurrentSkillLevel();
			int nMPRegenInc = pSkill->GetVar(8) + pSkill->GetVar(9) * pSkill->GetCurrentSkillLevel();

			m_nMaxHP += nMaxHPInc;
			m_nMaxMP += nMaxMPInc;
			m_Attribute.fHPRegenPoint += nHPRegenInc;
			m_Attribute.fMPRegenPoint += nMPRegenInc;
		}
		break;

	case SkillBase::EF_AMPLIFY_HP_MP:
		{
			m_vAmplifyPassiveSkillList.push_back( pSkill );
		}
		break;

	case SkillBase::EF_HEALING_AMPLIFY:
		{
			c_fixed10 fHPHealRatio = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fMPHealRatio = pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();

			m_fHealRatio += fHPHealRatio;
			m_fMPHealRatio += fMPHealRatio;
		}
		break;

	case SkillBase::EF_HEALING_AMPLIFY_BY_ITEM:
		{
			c_fixed10 fHPHealRatioByItem = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fMPHealRatioByItem = pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();

			m_fHealRatioByItem += fHPHealRatioByItem;
			m_fMPHealRatioByItem += fMPHealRatioByItem;
		}
		break;

	case SkillBase::EF_HEALING_AMPLIFY_BY_REST:
		{
			c_fixed10 fHPHealRatioByRest = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fMPHealRatioByRest = pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();

			m_fHealRatioByRest += fHPHealRatioByRest;
			m_fMPHealRatioByRest += fMPHealRatioByRest;
		}
		break;

	case SkillBase::EF_HATE_AMPLIFY:
		{
			m_fHateRatio += pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
		}
		break;


	// 코드량이 지나치게 많지만 가독성 때문에 우선 펼쳐둔다.
	case SkillBase::EF_ADD_STATE_ON_ATTACK:
	case SkillBase::EF_ADD_STATE_BY_SELF_ON_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pSkill->GetSkillBase()->GetEffectType() )
			{
			case SkillBase::EF_ADD_STATE_ON_ATTACK:			nTarget = StructProc::ATTACKEE;	break;
			case SkillBase::EF_ADD_STATE_BY_SELF_ON_ATTACK:	nTarget = StructProc::ATTACKER;	break;
			}

			m_vProcByAttack.push_back( _ATTACK_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 12 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByAttack.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pSkill->GetVar( 0 ) ), pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * pSkill->GetCurrentSkillLevel(), ( pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel() ) * 100, nTarget, pSkill->GetVar( 13 ) );
		}
		break;

	case SkillBase::EF_ADD_STATE_ON_BEING_ATTACKED:
	case SkillBase::EF_ADD_STATE_BY_SELF_ON_BEING_ATTACKED:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pSkill->GetSkillBase()->GetEffectType() )
			{
			case SkillBase::EF_ADD_STATE_ON_BEING_ATTACKED:			nTarget = StructProc::ATTACKER;	break;
			case SkillBase::EF_ADD_STATE_BY_SELF_ON_BEING_ATTACKED:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByBeingAttacked.push_back( _ATTACK_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 12 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByBeingAttacked.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pSkill->GetVar( 0 ) ), pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * pSkill->GetCurrentSkillLevel(), ( pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel() ) * 100, nTarget, pSkill->GetVar( 13 ) );
		}
		break;

	case SkillBase::EF_ADD_STATE_BY_SELF_ON_KILL:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			m_vProcByKill.push_back( _KILL_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), 0, 0, pSkill->GetVar( 18 ), pSkill->GetVar( 19 ), NULL ) );
			m_vProcByKill.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pSkill->GetVar( 0 ) ), pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * pSkill->GetCurrentSkillLevel(), ( pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel() ) * 100, StructProc::ATTACKER, pSkill->GetVar( 13 ) );
		}
		break;

	case SkillBase::EF_ADD_STATE_ON_CRITICAL_ATTACK:
	case SkillBase::EF_ADD_STATE_BY_SELF_ON_CRITICAL_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pSkill->GetSkillBase()->GetEffectType() )
			{
			case SkillBase::EF_ADD_STATE_ON_CRITICAL_ATTACK:			nTarget = StructProc::ATTACKEE;	break;
			case SkillBase::EF_ADD_STATE_BY_SELF_ON_CRITICAL_ATTACK:	nTarget = StructProc::ATTACKER;	break;
			}

			m_vProcByCriticalAttack.push_back( _ATTACK_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 12 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByCriticalAttack.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pSkill->GetVar( 0 ) ), pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * pSkill->GetCurrentSkillLevel(), ( pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel() ) * 100, nTarget, pSkill->GetVar( 13 ) );
		}
		break;

	case SkillBase::EF_ADD_STATE_ON_BEING_CRITICAL_ATTACKED:
	case SkillBase::EF_ADD_STATE_BY_SELF_ON_BEING_CRITICAL_ATTACKED:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pSkill->GetSkillBase()->GetEffectType() )
			{
			case SkillBase::EF_ADD_STATE_ON_BEING_CRITICAL_ATTACKED:			nTarget = StructProc::ATTACKER;	break;
			case SkillBase::EF_ADD_STATE_BY_SELF_ON_BEING_CRITICAL_ATTACKED:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByBeingCriticalAttacked.push_back( _ATTACK_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 12 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByBeingCriticalAttacked.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pSkill->GetVar( 0 ) ), pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * pSkill->GetCurrentSkillLevel(), ( pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel() ) * 100, nTarget, pSkill->GetVar( 13 ) );
		}
		break;

	case SkillBase::EF_ADD_STATE_ON_AVOID:
	case SkillBase::EF_ADD_STATE_BY_SELF_ON_AVOID:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pSkill->GetSkillBase()->GetEffectType() )
			{
			case SkillBase::EF_ADD_STATE_ON_AVOID:			nTarget = StructProc::ATTACKER;	break;
			case SkillBase::EF_ADD_STATE_BY_SELF_ON_AVOID:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByAvoid.push_back( _ATTACK_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 12 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByAvoid.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pSkill->GetVar( 0 ) ), pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * pSkill->GetCurrentSkillLevel(), ( pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel() ) * 100, nTarget, pSkill->GetVar( 13 ) );
		}
		break;

	case SkillBase::EF_ADD_STATE_ON_BLOCK:
	case SkillBase::EF_ADD_STATE_BY_SELF_ON_BLOCK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pSkill->GetSkillBase()->GetEffectType() )
			{
			case SkillBase::EF_ADD_STATE_ON_BLOCK:			nTarget = StructProc::ATTACKER;	break;
			case SkillBase::EF_ADD_STATE_BY_SELF_ON_BLOCK:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByBlock.push_back( _ATTACK_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 12 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByBlock.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pSkill->GetVar( 0 ) ), pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * pSkill->GetCurrentSkillLevel(), ( pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel() ) * 100, nTarget, pSkill->GetVar( 13 ) );
		}
		break;

	case SkillBase::EF_ADD_STATE_ON_PERFECT_BLOCK:
	case SkillBase::EF_ADD_STATE_BY_SELF_ON_PERFECT_BLOCK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pSkill->GetSkillBase()->GetEffectType() )
			{
			case SkillBase::EF_ADD_STATE_ON_PERFECT_BLOCK:			nTarget = StructProc::ATTACKER;	break;
			case SkillBase::EF_ADD_STATE_BY_SELF_ON_PERFECT_BLOCK:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByPerfectBlock.push_back( _ATTACK_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 12 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByPerfectBlock.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pSkill->GetVar( 0 ) ), pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * pSkill->GetCurrentSkillLevel(), ( pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel() ) * 100, nTarget, pSkill->GetVar( 13 ) );
		}
		break;

	case SkillBase::EF_ADD_ENERGY_ON_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 12 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByAttack.back().proc = new StructEnergyProc( StructProc::ATTACKER, pSkill->GetVar( 0 ) + pSkill->GetVar( 1 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_ADD_ENERGY_ON_BEING_ATTACKED:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 8 ) && weapon_class != pSkill->GetVar( 9 ) && weapon_class != pSkill->GetVar( 10 ) && weapon_class != pSkill->GetVar( 11 ) )
				break;

			m_vProcByBeingAttacked.push_back( _ATTACK_TAG( pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 12 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByBeingAttacked.back().proc = new StructEnergyProc( StructProc::ATTACKEE, pSkill->GetVar( 0 ) + pSkill->GetVar( 1 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_INC_SKILL_COOL_TIME_ON_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 11 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 11 ) && weapon_class != pSkill->GetVar( 12 ) )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 13 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByAttack.back().proc = new StructCooldownProc( StructProc::ATTACKER, pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_INC_SKILL_COOL_TIME_ON_BEING_ATTACKED:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 11 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 11 ) && weapon_class != pSkill->GetVar( 12 ) )
				break;

			m_vProcByBeingAttacked.push_back( _ATTACK_TAG( pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 13 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByBeingAttacked.back().proc = new StructCooldownProc( StructProc::ATTACKEE, pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_INC_SKILL_COOL_TIME_ON_KILL:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 11 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 11 ) && weapon_class != pSkill->GetVar( 12 ) )
				break;

			m_vProcByKill.push_back( _KILL_TAG( pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), 0, 0, pSkill->GetVar( 18 ), pSkill->GetVar( 19 ), NULL ) );
			m_vProcByKill.back().proc = new StructCooldownProc( StructProc::ATTACKER, pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_INC_SKILL_COOL_TIME_ON_CRITICAL_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 11 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 11 ) && weapon_class != pSkill->GetVar( 12 ) )
				break;

			m_vProcByCriticalAttack.push_back( _ATTACK_TAG( pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 13 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByCriticalAttack.back().proc = new StructCooldownProc( StructProc::ATTACKER, pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_INC_SKILL_COOL_TIME_ON_BEING_CRITICAL_ATTACKED:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 11 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 11 ) && weapon_class != pSkill->GetVar( 12 ) )
				break;

			m_vProcByBeingCriticalAttacked.push_back( _ATTACK_TAG( pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 13 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByBeingCriticalAttacked.back().proc = new StructCooldownProc( StructProc::ATTACKEE, pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_INC_SKILL_COOL_TIME_ON_AVOID:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 11 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 11 ) && weapon_class != pSkill->GetVar( 12 ) )
				break;

			m_vProcByAvoid.push_back( _ATTACK_TAG( pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 13 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByAvoid.back().proc = new StructCooldownProc( StructProc::ATTACKEE, pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_INC_SKILL_COOL_TIME_ON_BLOCK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 11 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 11 ) && weapon_class != pSkill->GetVar( 12 ) )
				break;

			m_vProcByBlock.push_back( _ATTACK_TAG( pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 13 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByBlock.back().proc = new StructCooldownProc( StructProc::ATTACKEE, pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_INC_SKILL_COOL_TIME_ON_PERFECT_BLOCK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pSkill->GetVar( 11 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pSkill->GetVar( 11 ) && weapon_class != pSkill->GetVar( 12 ) )
				break;

			m_vProcByPerfectBlock.push_back( _ATTACK_TAG( pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), pSkill->GetVar( 13 ), pSkill->GetVar( 18 ), NULL ) );
			m_vProcByPerfectBlock.back().proc = new StructCooldownProc( StructProc::ATTACKEE, pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
		}
		break;

	case SkillBase::EF_INC_SKILL_COOL_TIME_ON_SKILL_OF_ID:
		{
			for( int i = 11; i < 14; ++i )
			{
				int nSkillId = pSkill->GetVar( i );
				if( !nSkillId ) break;
				
				std::map< int, std::vector< _PROC_TAG > >::const_iterator it = m_mapProcBySkillId.find( nSkillId );

				if( it == m_mapProcBySkillId.end() )
				{
					m_mapProcBySkillId.insert( std::make_pair( nSkillId, std::vector< _PROC_TAG >() ) );
				}

				std::vector< _PROC_TAG > & vProc = m_mapProcBySkillId[ nSkillId ];
				vProc.push_back( _PROC_TAG( pSkill->GetVar( 9 ) + pSkill->GetVar( 10 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 14 ), pSkill->GetVar( 15 ), pSkill->GetVar( 16 ), pSkill->GetVar( 17 ), NULL ) );
				vProc.back().proc = new StructCooldownProc( StructProc::ATTACKER, pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
			}
		}
		break;

	case SkillBase::EF_WEAPON_TRAINING:
		{
			StructItem * pWeapon = GetWearedItem( ItemBase::WEAR_WEAPON );

			if( !pWeapon )
				break;

			c_fixed10 fAttackPoint = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fAttackSpeed = pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fAccuracy = pSkill->GetVar(4) + pSkill->GetVar(5) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fMagicPoint = pSkill->GetVar(6) + pSkill->GetVar(7) * pSkill->GetCurrentSkillLevel();
			c_fixed10 fCritical = pSkill->GetVar(8) + pSkill->GetVar(9) * pSkill->GetCurrentSkillLevel();

			m_AttributeAmplifier.fAttackPointRight += fAttackPoint;
			m_AttributeAmplifier.fAttackSpeedRight += fAttackSpeed;
			m_AttributeAmplifier.fAccuracyRight += fAccuracy;

			//m_nAttackPointRightByWeaponMastery	+= nAP;

			if( IsUsingDoubleWeapon() )
			{
				m_AttributeAmplifier.fAttackPointLeft += fAttackPoint;
				m_AttributeAmplifier.fAttackSpeedLeft += fAttackSpeed;
				m_AttributeAmplifier.fAccuracyLeft += fAccuracy;

				//m_nAttackPointLeftByWeaponMastery += nAP;
			}

			m_AttributeAmplifier.fMagicPoint += fMagicPoint;
			m_AttributeAmplifier.fCritical += fCritical;
		}
		break;

	case SkillBase::EF_AMPLIFY_EXT_ATTRIBUTE:
		{
			int nSkillLevel = pSkill->GetCurrentSkillLevel();

			m_AttributeAmplifier.fHPRegenPercentage += pSkill->GetVar(0) * nSkillLevel;
			m_AttributeAmplifier.fHPRegenPoint += pSkill->GetVar(1) * nSkillLevel;
			m_AttributeAmplifier.fMPRegenPercentage += pSkill->GetVar(2) * nSkillLevel;
			m_AttributeAmplifier.fMPRegenPoint += pSkill->GetVar(3) * nSkillLevel;
			m_AttributeAmplifier.fBlockChance += pSkill->GetVar(4) * nSkillLevel;
			m_AttributeAmplifier.fBlockDefence += pSkill->GetVar(5) * nSkillLevel;
			m_AttributeAmplifier.fCritical += pSkill->GetVar(6) * nSkillLevel;
			m_AttributeAmplifier.fCriticalPower += pSkill->GetVar(7) * nSkillLevel;
			m_AttributeAmplifier.fCastingSpeed += pSkill->GetVar(8) * nSkillLevel;
			m_AttributeAmplifier.fCoolTimeSpeed += pSkill->GetVar(9) * nSkillLevel;

			// 캐스팅 보전 따위... 그냥 곱해
			m_nCastKeep *= pSkill->GetVar(10) * nSkillLevel;

			// 화살 데미지 감소율: 미구현
		}
		break;

	case SkillBase::EF_INC_DAMAGE_BY_TARGET_STATE:
	case SkillBase::EF_AMP_DAMAGE_BY_TARGET_STATE:
		{
			m_vSkillRelatedToDamaging.push_back( pSkill );
		}
		break;

	case SkillBase::EF_MODIFY_SKILL_COST:
		// StructSkill 내부의 스킬 비용을 변경시키는 용도로 쓰는 변수를 통해 스킬 비용을 수정한다.
		// CalculateStat에서 모든 스킬에 대해 CostModifier를 Reset한 뒤 여기에서 이를 재설정한다.
		// 마음에 드는 방법은 아니나 지금과 같이 상태를 매 번 수동으로 업데이트하는 구조 위에서는 별 도리가 없다.
		// 이 방법 아니면 매 번 스킬 시전할 때마다 StructCreature의 PassiveSkillList를 뒤져서 재계산해야 하는 것이다.
		{
			float HPInc = pSkill->GetVar( 0 ) + pSkill->GetVar( 1 ) * pSkill->GetCurrentSkillLevel();
			float MPInc = pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * pSkill->GetCurrentSkillLevel();
			float HPAmp = pSkill->GetVar( 5 ) + pSkill->GetVar( 6 ) * pSkill->GetCurrentSkillLevel();
			float MPAmp = pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel();
			bool applyToAllSkill = ( pSkill->GetVar( 9 ) == 1 );

			if ( applyToAllSkill == true )
			{
				m_SkillCostModifier.add( MPInc, MPAmp, HPInc, HPAmp );
			}
			else
			{
			for ( std::vector< StructSkill* >::iterator it = m_vAllSkillList.begin(); it != m_vAllSkillList.end(); ++it )
			{
					int skillID = (*it)->GetSkillId();

					for ( int i = 10; i < 20; i++ )
					{
						if ( skillID == pSkill->GetVar( i ) )
						{
							(*it)->GetCostModifier().add( MPInc, MPAmp, HPInc, HPAmp );
						}
					}
				}
			}
		}
		break;

	case SkillBase::EF_RESIST_HARMFUL_STATE:
		{
			for( int i = 0; i < 6; ++i )
			{
				StructState::StateCode code = static_cast< StructState::StateCode >( (int)pSkill->GetVar( i * 3 ) );
				c_fixed10 rate = pSkill->GetVar( i * 3 + 1 ) + pSkill->GetVar( i * 3 + 2 ) * pSkill->GetCurrentSkillLevel();

				if( !code ) continue;

				std::map< StructState::StateCode, c_fixed10 >::iterator it = m_mapResistHarmfulState.find( code );
				if( it == m_mapResistHarmfulState.end() )
				{
					m_mapResistHarmfulState.insert( std::make_pair< StructState::StateCode, c_fixed10 >( code, rate ) );
				}
				else
				{
					(*it).second += rate;
				}
			}
		}
		break;

	case SkillBase::EF_INC_DAMAGE_INC_CRIT_RATE_BY_TARGET_HP_RATIO:
	case SkillBase::EF_AMP_DAMAGE_INC_CRIT_RATE_BY_TARGET_HP_RATIO:
		{
			m_vSkillRelatedToDamaging.push_back( pSkill );
		}
		break;

	case SkillBase::EF_ABSORB_DAMAGE:
		{
			if( pSkill->GetVar(8) != ItemBase::CLASS_EVERY_WEAPON )
			{
				ItemBase::ItemClass weapon_class = GetWeaponClass();

				if( !weapon_class )
				{
					break;
				}

				if( weapon_class != pSkill->GetVar(8) && weapon_class !=  pSkill->GetVar(9) &&
					weapon_class != pSkill->GetVar(10) && weapon_class !=  pSkill->GetVar(11) )
					break;
			}

			float fHPAbsorbRatio = pSkill->GetVar(0) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(1);
			float fMPAbsorbRatio = pSkill->GetVar(2) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(3);
			int nRatio = pSkill->GetVar(6) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(7);
			int nAttackType = pSkill->GetVar(4);

			if( !fHPAbsorbRatio && !fMPAbsorbRatio )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( nRatio, 0, 0, 0, 0, nAttackType, 99, NULL ) );
			m_vProcByAttack.back().proc = new StructAbsorbProc( StructProc::ATTACKER, fHPAbsorbRatio, fMPAbsorbRatio );
		}
		break;

	case SkillBase::EF_STEAL_HP_MP:
		{
			if( pSkill->GetVar(8) != ItemBase::CLASS_EVERY_WEAPON )
			{
				ItemBase::ItemClass weapon_class = GetWeaponClass();

				if( !weapon_class )
				{
					break;
				}

				if( weapon_class != pSkill->GetVar(8) && weapon_class != pSkill->GetVar(9) &&
					weapon_class != pSkill->GetVar(10) && weapon_class != pSkill->GetVar(11) )
					break;
			}

			int nHPSteal = pSkill->GetVar(0) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(1);
			int nMPSteal = pSkill->GetVar(2) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(3);
			int nRatio = pSkill->GetVar(6) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(7);
			int nAttackType = pSkill->GetVar(4);

			if( !nHPSteal && !nMPSteal )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( nRatio, 0, 0, 0, 0, nAttackType, 99, NULL ) );
			m_vProcByAttack.back().proc = new StructStealProc( StructProc::ATTACKER, nHPSteal, nMPSteal );
		}
		break;

	case SkillBase::EF_INCREASE_ENERGY_UNCONSUMPTION_RATE:
		{
			m_nEnergyUnconsumptionRate += pSkill->GetVar( 0 ) + pSkill->GetVar( 1 ) * pSkill->GetCurrentSkillLevel() + pSkill->GetVar( 2 ) * pSkill->GetEnhance();
		}
		break;

	case SkillBase::EF_INC_PARAM_AMPLIFY_HEAL:
		{
			incParameter( pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), false );
			incParameter( pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), false );
			incParameter2( pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
			incParameter2( pSkill->GetVar( 9 ), pSkill->GetVar( 10 ) + pSkill->GetVar( 11 ) * pSkill->GetCurrentSkillLevel() );

			m_fHealRatio += pSkill->GetVar( 12 ) + pSkill->GetVar( 13 ) * pSkill->GetCurrentSkillLevel();
			m_fMPHealRatio += pSkill->GetVar( 14 ) + pSkill->GetVar( 15 ) * pSkill->GetCurrentSkillLevel();

			if( pSkill->GetVar( 16 ) != 0 ) // 아이템 적용
			{
				m_fHealRatioByItem += pSkill->GetVar( 12 ) + pSkill->GetVar( 13 ) * pSkill->GetCurrentSkillLevel();
				m_fMPHealRatioByItem += pSkill->GetVar( 14 ) + pSkill->GetVar( 15 ) * pSkill->GetCurrentSkillLevel();
			}
			break;
		}

	case SkillBase::EF_INC_PARAM_BY_STATE:
		{
			bool exist = false;
			for ( int i = 12; i < 20; i++ )
			{
				StructState::StateCode stateCode = (StructState::StateCode)(int) pSkill->GetVar( i );

				if ( (int)stateCode == 0 )
				{
					break;
				}
				else if ( GetState( stateCode ) != NULL )
				{
					exist = true;
					break;
				}
			}

			if( exist == true )
			{
				incParameter( pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), false );
				incParameter( pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), false );
				incParameter2( pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
				incParameter2( pSkill->GetVar( 9 ), pSkill->GetVar( 10 ) + pSkill->GetVar( 11 ) * pSkill->GetCurrentSkillLevel() );
			}
			break;
		}

	case SkillBase::EF_AMP_PARAM_AMPLIFY_HEAL:
	case SkillBase::EF_AMP_PARAM_BY_STATE:
		{
			m_vAmplifyPassiveSkillList.push_back( pSkill );
		}

	case SkillBase::EF_MISC:
		{
			switch( pSkill->GetSkillId() )
			{
			case StructSkill::SKILL_AMORY_UNIT:
				{
					m_vAmplifyPassiveSkillList.push_back( pSkill );
				}
				break;

				// SKILL_ARMOR_MASTERY
			case StructSkill::SKILL_INCREASE_ENERGY:
				{
					m_nMaxEnergy += pSkill->GetCurrentSkillLevel();
				}
				break;
			case StructSkill::SKILL_ARMOR_MASTERY:
			case StructSkill::SKILL_GAIA_ARMOR_MASTERY:
				if( GetWearedItem( ItemBase::WEAR_ARMOR ) )
				{
					m_Attribute.fDefence += pSkill->GetVar(0) * pSkill->GetCurrentSkillLevel();
				}
				break;

			case StructSkill::SKILL_LIFE_OF_DARKNESS:
				{
					m_Resist.nResist[ Elemental::TYPE_DARK ] += pSkill->GetVar(0) * pSkill->GetCurrentSkillLevel();
				}
				break;

			default:
				break;

			}
		}
		break;

	default:
		break;
	}
}

void StructPlayer::applyPassiveSkillEffect( struct StructSkill * pSkill )
{
	switch( pSkill->GetSkillBase()->GetSkillEffectType() )
	{
	case SkillBase::EF_INCREASE_SUMMON_HP_MP_SP:
		{
			m_ParameterForSummon.nMaxHP += pSkill->GetVar(0) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(1);
			m_ParameterForSummon.nMaxMP += pSkill->GetVar(2) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(3);
			m_ParameterForSummon.nMaxSP += pSkill->GetVar(4) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(5);
			m_ParameterForSummon.Attribute.fHPRegenPoint += pSkill->GetVar(6) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(7);
			m_ParameterForSummon.Attribute.fMPRegenPoint += pSkill->GetVar(8) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(9);
			m_ParameterForSummon.nSPRegenAdd += pSkill->GetVar(10) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(11);
		}
		break;

	case SkillBase::EF_INC_SUMMON_PARAM_BASED_PARAM:
		{
			c_fixed10 sum = CalcStatSum( pSkill->GetVar(12) ); // 스탯 증가 분은 스탯만 참조한다
			c_fixed10 coefficient = pSkill->GetVar(1) + pSkill->GetVar(2) * pSkill->GetCurrentSkillLevel();
			incParameter( pSkill->GetVar(0), sum * coefficient, true, &m_ParameterForSummon );

			sum += CalcAttributeSum( pSkill->GetVar(12), pSkill->GetVar(13) );
			incParameter( pSkill->GetVar(0), sum * coefficient, false, &m_ParameterForSummon );

			coefficient = pSkill->GetVar(4) + pSkill->GetVar(5) * pSkill->GetCurrentSkillLevel();
			incParameter2( pSkill->GetVar(3), sum * coefficient, &m_ParameterForSummon );
			
			sum = CalcStatSum( pSkill->GetVar(14) ); // 스탯 증가 분은 스탯만 참조한다
			coefficient = pSkill->GetVar(7) + pSkill->GetVar(8) * pSkill->GetCurrentSkillLevel();
			incParameter( pSkill->GetVar(6), sum * coefficient, true, &m_ParameterForSummon );

			sum += CalcAttributeSum( pSkill->GetVar(14), pSkill->GetVar(15) );
			incParameter( pSkill->GetVar(6), sum * coefficient, false, &m_ParameterForSummon );

			coefficient = pSkill->GetVar(10) + pSkill->GetVar(11) * pSkill->GetCurrentSkillLevel();
			incParameter2( pSkill->GetVar(9), sum * coefficient, &m_ParameterForSummon );
		}
		break;

	case SkillBase::EF_INC_SUMMON_PARAM_BASED_SUMMON_PARAM:
		{
			StructSummon *pSummon = GetMainSummon();
			if( !pSummon )
				break;

			c_fixed10 sum = pSummon->CalcStatSum( pSkill->GetVar(12), pSummon->GetStatBeforeParamBasedCalc() ); // 스탯 증가 분은 스탯만 참조한다
			c_fixed10 coefficient = pSkill->GetVar(1) + pSkill->GetVar(2) * pSkill->GetCurrentSkillLevel();
			incParameter( pSkill->GetVar(0), sum * coefficient, true, &m_ParameterForSummon );

			sum += pSummon->CalcAttributeSum( pSkill->GetVar(12), pSkill->GetVar(13), pSummon->GetAttrBeforeParamBasedCalc() );
			incParameter( pSkill->GetVar(0), sum * coefficient, false, &m_ParameterForSummon );

			coefficient = pSkill->GetVar(4) + pSkill->GetVar(5) * pSkill->GetCurrentSkillLevel();
			incParameter2( pSkill->GetVar(3), sum * coefficient, &m_ParameterForSummon );

			sum = pSummon->CalcStatSum( pSkill->GetVar(14), pSummon->GetStatBeforeParamBasedCalc() ); // 스탯 증가 분은 스탯만 참조한다
			coefficient = pSkill->GetVar(7) + pSkill->GetVar(8) * pSkill->GetCurrentSkillLevel();
			incParameter( pSkill->GetVar(6), sum * coefficient, true, &m_ParameterForSummon );

			sum += pSummon->CalcAttributeSum( pSkill->GetVar(14), pSkill->GetVar(15), pSummon->GetAttrBeforeParamBasedCalc() );
			incParameter( pSkill->GetVar(6), sum * coefficient, false, &m_ParameterForSummon );
			
			coefficient = pSkill->GetVar(10) + pSkill->GetVar(11) * pSkill->GetCurrentSkillLevel();
			incParameter2( pSkill->GetVar(9), sum * coefficient, &m_ParameterForSummon );
		}
		break;

	case SkillBase::EF_AMPLIFY_SUMMON_HP_MP_SP:
		{
			float fMaxHPInc = pSkill->GetVar(0) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(1);
			float fMaxMPInc = pSkill->GetVar(2) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(3);
			float fMaxSPInc = pSkill->GetVar(4) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(5);
			float fHPRegenInc = pSkill->GetVar(6) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(7);
			float fMPRegenInc = pSkill->GetVar(8) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(9);
			float fSPRegenInc = pSkill->GetVar(10) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(11);

			m_ParameterForSummon.fMaxHPAmplifier += fMaxHPInc;
			m_ParameterForSummon.fMaxMPAmplifier += fMaxMPInc;
			m_ParameterForSummon.fMaxSPAmplifier += fMaxSPInc;
			m_ParameterForSummon.AttributeAmplifier.fHPRegenPoint += fHPRegenInc;
			m_ParameterForSummon.AttributeAmplifier.fMPRegenPoint += fMPRegenInc;
			m_ParameterForSummon.fSPRegenAddAmplifier += fSPRegenInc;
		}
		break;

	case SkillBase::EF_AMPLIFY_EXP_FOR_SUMMON:
		{
			m_fActiveSummonExpAmp += pSkill->GetVar(0) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(1);
			m_fDeactiveSummonExpAmp += pSkill->GetVar(2) + pSkill->GetCurrentSkillLevel() * pSkill->GetVar(3);
		}
		break;

	case SkillBase::EF_MISC:
		{
			if( pSkill->GetSkillId() == StructSkill::SKILL_TECHNICAL_CREATURE_CONTROL )
			{
				m_nDoubleSummonTime += pSkill->GetCurrentSkillLevel() * pSkill->GetVar(0) * 100;
				break;
			}
		}
	default:
		StructCreature::applyPassiveSkillEffect( pSkill );
		break;
	}
}

void StructCreature::applyPassiveSkillAmplifyEffect()
{
	if( m_vAmplifyPassiveSkillList.empty() ) return;

	std::vector< StructSkill *>::iterator it;

	for( it = m_vAmplifyPassiveSkillList.begin(); it != m_vAmplifyPassiveSkillList.end(); ++it )
	{
		StructSkill * pSkill = (*it);

		applyPassiveSkillAmplifyEffect( pSkill );
	}
}

void StructSummon::applyPassiveSkillAmplifyEffect()
{
	StructCreature::applyPassiveSkillAmplifyEffect();

	m_fMaxHPAmplifier += m_ParameterForSummon.fMaxHPAmplifier;
	m_fMaxMPAmplifier += m_ParameterForSummon.fMaxMPAmplifier;
	m_fMaxSPAmplifier += m_ParameterForSummon.fMaxSPAmplifier;
	m_AttributeAmplifier.fHPRegenPoint += m_ParameterForSummon.AttributeAmplifier.fHPRegenPoint;
	m_AttributeAmplifier.fMPRegenPoint += m_ParameterForSummon.AttributeAmplifier.fMPRegenPoint;
	m_fSPRegenAddAmplifier += m_ParameterForSummon.fSPRegenAddAmplifier;
}

void StructCreature::applyPassiveSkillAmplifyEffect( struct StructSkill * pSkill )
{
	bool bFighterArmor = false;
	bool bHunterArmor = false;
	bool bMagicianArmor = false;
	bool bSummonerArmor = false;

	ItemBase::ItemClass armor_class = GetArmorClass();

	if( armor_class == ItemBase::CLASS_FIGHTER_ARMOR ) bFighterArmor = true;
	else if( armor_class == ItemBase::CLASS_HUNTER_ARMOR ) bHunterArmor = true;
	else if( armor_class == ItemBase::CLASS_MAGICIAN_ARMOR ) bMagicianArmor = true;
	else if( armor_class == ItemBase::CLASS_SUMMONER_ARMOR ) bSummonerArmor = true;

	if( pSkill->GetSkillId() == StructSkill::SKILL_AMORY_UNIT && bFighterArmor )
	{
		m_AttributeAmplifier.fDefence += pSkill->GetVar( 0 );
	}

	switch( pSkill->GetSkillBase()->GetSkillEffectType() )
	{
	case SkillBase::EF_PARAMETER_AMP:
		{
			ampParameter( pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), false );
			ampParameter( pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), false );
			ampParameter2( pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
			ampParameter2( pSkill->GetVar( 9 ), pSkill->GetVar( 10 ) + pSkill->GetVar( 11 ) * pSkill->GetCurrentSkillLevel() );
			ampParameter( pSkill->GetVar( 12 ), pSkill->GetVar( 13 ) + pSkill->GetVar( 14 ) * pSkill->GetCurrentSkillLevel(), false );
			ampParameter( pSkill->GetVar( 15 ), pSkill->GetVar( 16 ) + pSkill->GetVar( 17 ) * pSkill->GetCurrentSkillLevel(), false );
			break;
		}
	case SkillBase::EF_AMPLIFY_HP_MP:
		{
			float fMaxHPInc = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
			float fMaxMPInc = pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
			float fHPRegenInc = pSkill->GetVar(6) + pSkill->GetVar(7) * pSkill->GetCurrentSkillLevel();
			float fMPRegenInc = pSkill->GetVar(8) + pSkill->GetVar(9) * pSkill->GetCurrentSkillLevel();

			m_fMaxHPAmplifier += fMaxHPInc;
			m_fMaxMPAmplifier += fMaxMPInc;
			m_AttributeAmplifier.fHPRegenPercentage += fHPRegenInc;
			m_AttributeAmplifier.fMPRegenPercentage += fMPRegenInc;
		}
		break;

	case SkillBase::EF_AMP_PARAM_AMPLIFY_HEAL:
		{
			ampParameter( pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), false );
			ampParameter( pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), false );
			ampParameter2( pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
			ampParameter2( pSkill->GetVar( 9 ), pSkill->GetVar( 10 ) + pSkill->GetVar( 11 ) * pSkill->GetCurrentSkillLevel() );

			m_fHealRatio += pSkill->GetVar( 12 ) + pSkill->GetVar( 13 ) * pSkill->GetCurrentSkillLevel();
			m_fMPHealRatio += pSkill->GetVar( 14 ) + pSkill->GetVar( 15 ) * pSkill->GetCurrentSkillLevel();

			if( pSkill->GetVar( 16 ) != 0 ) // 아이템 적용
			{
				m_fHealRatioByItem += pSkill->GetVar( 12 ) + pSkill->GetVar( 13 ) * pSkill->GetCurrentSkillLevel();
				m_fMPHealRatioByItem += pSkill->GetVar( 14 ) + pSkill->GetVar( 15 ) * pSkill->GetCurrentSkillLevel();
			}
			break;
		}
	case SkillBase::EF_AMP_PARAM_BY_STATE:
		{
			bool exist = false;
			for ( int i = 12; i < 20; i++ )
			{
				StructState::StateCode stateCode = (StructState::StateCode)(int) pSkill->GetVar( i );

				if ( (int)stateCode == 0 )
				{
					break;
				}
				if ( GetState( stateCode ) != NULL )
				{
					exist = true;
					break;
				}
			}

			if( exist == true )
			{
				ampParameter( pSkill->GetVar( 0 ), pSkill->GetVar( 1 ) + pSkill->GetVar( 2 ) * pSkill->GetCurrentSkillLevel(), false );
				ampParameter( pSkill->GetVar( 3 ), pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * pSkill->GetCurrentSkillLevel(), false );
				ampParameter2( pSkill->GetVar( 6 ), pSkill->GetVar( 7 ) + pSkill->GetVar( 8 ) * pSkill->GetCurrentSkillLevel() );
				ampParameter2( pSkill->GetVar( 9 ), pSkill->GetVar( 10 ) + pSkill->GetVar( 11 ) * pSkill->GetCurrentSkillLevel() );
			}
			break;
		}

	default:
		break;
	}
}

void StructCreature::calcParamBasedStat()
{
	if( m_vPassiveSkillList.empty() ) return;

	std::vector< StructSkill *>::iterator it;

	for( it = m_vPassiveSkillList.begin(); it != m_vPassiveSkillList.end(); ++it )
	{
		StructSkill* pSkill = *it;

		switch( pSkill->GetSkillBase()->GetSkillEffectType() )
		{
		case SkillBase::EF_INC_PARAM_BASED_SUMMON_PARAM:
			{
				// IsPlayer 등의 타입 파악 함수가 존재하는 시점에서 이미 DIP는 깨진 것이나 다름없다.
				// 어차피 깨질대로 깨진 상속 구조에 DIP를 억지로 맞추기 위해 코드의 응집성을 낮추기보다는 당장은 구현에 집중하고
				// 추후 리팩토링을 통해 관련 구현을 StructSkill쪽으로 넘기는 방향을 추구하는 것이 나아 보인다.
				StructCreature* summon = IsPlayer() ? static_cast< StructPlayer* >( this )->GetMainSummon() : NULL;
				if ( summon != NULL )
				{
					c_fixed10 sum = summon->CalcStatSum( pSkill->GetVar(12) );
					c_fixed10 coefficient = pSkill->GetVar(1) + pSkill->GetVar(2) * pSkill->GetCurrentSkillLevel();
					incParameter( pSkill->GetVar(0), sum * coefficient, true );

					sum = summon->CalcStatSum( pSkill->GetVar(14) );
					coefficient = pSkill->GetVar(7) + pSkill->GetVar(8) * pSkill->GetCurrentSkillLevel();
					incParameter( pSkill->GetVar(6), sum * coefficient, true );
				}
				break;
			}
		case SkillBase::EF_INC_PARAM_BASED_PARAM:
			{
				c_fixed10 sum = CalcStatSum( pSkill->GetVar(12), m_StatBeforeParamBasedCalc ); // 스탯 증가 분은 스탯만 참조한다
				c_fixed10 coefficient = pSkill->GetVar(1) + pSkill->GetVar(2) * pSkill->GetCurrentSkillLevel();
				incParameter( pSkill->GetVar(0), sum * coefficient, true );

				sum = CalcStatSum( pSkill->GetVar(14), m_StatBeforeParamBasedCalc ); // 스탯 증가 분은 스탯만 참조한다
				coefficient = pSkill->GetVar(7) + pSkill->GetVar(8) * pSkill->GetCurrentSkillLevel();
				incParameter( pSkill->GetVar(6), sum * coefficient, true );

				break;
			}
		}
	}
}

void StructSummon::calcParamBasedStat()
{
	StructCreature::calcParamBasedStat();

	m_Stat += m_ParameterForSummon.Stat;
}

void StructCreature::calcParamBasedAttribute()
{
	// 스킬 하나가 파라메터에 기반하여 성능이 바뀌기 때문에 어쩔 수 없이 분리
	// TODO: 차후 스탯 적용 시점과 아이템 효과 적용 시점을 분리할 때 이 함수도 합쳐 볼 방안을 찾아본다

	if( m_vPassiveSkillList.empty() ) return;

	std::vector< StructSkill *>::iterator it;

	for( it = m_vPassiveSkillList.begin(); it != m_vPassiveSkillList.end(); ++it )
	{
		StructSkill* pSkill = *it;

		switch( pSkill->GetSkillBase()->GetSkillEffectType() )
		{
		case SkillBase::EF_INC_PARAM_BASED_PARAM:
			{
				c_fixed10 sum = CalcStatSum( pSkill->GetVar(12), m_StatBeforeParamBasedCalc )
					+ CalcAttributeSum( pSkill->GetVar(12), pSkill->GetVar(13), m_AttrBeforeParamBasedCalc );

				c_fixed10 coefficient = pSkill->GetVar(1) + pSkill->GetVar(2) * pSkill->GetCurrentSkillLevel();
				incParameter( pSkill->GetVar(0), sum * coefficient, false );

				coefficient = pSkill->GetVar(4) + pSkill->GetVar(5) * pSkill->GetCurrentSkillLevel();
				incParameter2( pSkill->GetVar(3), sum * coefficient);

				sum = CalcStatSum( pSkill->GetVar(14), m_StatBeforeParamBasedCalc )
					+ CalcAttributeSum( pSkill->GetVar(14), pSkill->GetVar(15), m_AttrBeforeParamBasedCalc );

				coefficient = pSkill->GetVar(7) + pSkill->GetVar(8) * pSkill->GetCurrentSkillLevel();
				incParameter( pSkill->GetVar(6), sum * coefficient, false );

				coefficient = pSkill->GetVar(10) + pSkill->GetVar(11) * pSkill->GetCurrentSkillLevel();
				incParameter2( pSkill->GetVar(9), sum * coefficient);

				break;
			}
		case SkillBase::EF_INC_PARAM_BASED_SUMMON_PARAM:
			{
				// IsPlayer 등의 타입 파악 함수가 존재하는 시점에서 이미 DIP는 깨진 것이나 다름없다.
				// 어차피 깨질대로 깨진 상속 구조에 DIP를 억지로 맞추기 위해 코드의 응집성을 낮추기보다는 당장은 구현에 집중하고
				// 추후 리팩토링을 통해 관련 구현을 StructSkill쪽으로 넘기는 방향을 추구하는 것이 나아 보인다.
				StructCreature* summon = IsPlayer() ? static_cast< StructPlayer* >( this )->GetMainSummon() : NULL;
				if ( summon != NULL )
				{
					c_fixed10 sum = summon->CalcStatSum( pSkill->GetVar(12) )
						+ summon->CalcAttributeSum( pSkill->GetVar(12), pSkill->GetVar(13) );

					c_fixed10 coefficient = pSkill->GetVar(1) + pSkill->GetVar(2) * pSkill->GetCurrentSkillLevel();
					incParameter( pSkill->GetVar(0), sum * coefficient, false );

					coefficient = pSkill->GetVar(4) + pSkill->GetVar(5) * pSkill->GetCurrentSkillLevel();
					incParameter2( pSkill->GetVar(3), sum * coefficient);

					sum = summon->CalcStatSum( pSkill->GetVar(14) )
						+ summon->CalcAttributeSum( pSkill->GetVar(14), pSkill->GetVar(15) );

					coefficient = pSkill->GetVar(7) + pSkill->GetVar(8) * pSkill->GetCurrentSkillLevel();
					incParameter( pSkill->GetVar(6), sum * coefficient, false );

					coefficient = pSkill->GetVar(10) + pSkill->GetVar(11) * pSkill->GetCurrentSkillLevel();
					incParameter2( pSkill->GetVar(9), sum * coefficient);
				}
				break;
			}
		}
	}
}

void StructSummon::calcParamBasedAttribute()
{
	StructCreature::calcParamBasedAttribute();

	m_Attribute += m_ParameterForSummon.Attribute;
	m_fAttackPointRightWithoutWeapon += m_ParameterForSummon.fAttackPointRightWithoutWeapon;

	if( IsUsingDoubleWeapon() )
	{
		m_Attribute.fAttackPointLeft += m_ParameterForSummon.Attribute.fAttackPointRight;
		m_fAttackPointLeftWithoutWeapon += m_ParameterForSummon.Attribute.fAttackPointRight;
		m_Attribute.fAttackSpeedLeft += m_ParameterForSummon.Attribute.fAttackSpeedRight;
		m_Attribute.fAccuracyLeft += m_ParameterForSummon.Attribute.fAccuracyRight;
	}

	if( !isMoveSpeedFixed() )
	{
		m_Attribute.fMoveSpeed += m_ParameterForSummon.Attribute.fMoveSpeed;
	}

	if( IsUsingBow() || IsUsingCrossBow() )
		// 사정거리 단위, 내부적으로는 기획 DB 에서 ATTACK_RANGE_UNIT 를 곱한 값으로 처리된다.
		m_Attribute.fAttackRange += m_ParameterForSummon.Attribute.fAttackRange * GameRule::ATTACK_RANGE_UNIT;

	m_Resist += m_ParameterForSummon.Resist;
}

// --- 스탯 및 능력치 연산에 관련된 지속 효과들 적용하는 루틴 ---
void StructCreature::applyStateEffect()
{
	if( m_vStateList.empty() ) return;

	std::vector< std::pair< int, int > > vDecreaseList;

	STATE_ITERATOR it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		if( (*it).GetEffectType() == StructState::EF_DECREASE_STATE_EFFECT )
		{
			int nDecreaseLevel = (*it).GetValue(0) + (*it).GetValue(1) * (*it).GetLevel();

			for( int i = 2 ; i < 11 && (*it).GetValue(i) ; ++i )
			{
				vDecreaseList.push_back( std::pair< int, int >( (int)(*it).GetValue(i), nDecreaseLevel ) );
			}
		}
	}

	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		int nOriginalLevel = (*it).GetLevel();

		for( std::vector< std::pair< int, int > >::iterator itDecrease = vDecreaseList.begin() ; itDecrease != vDecreaseList.end() ; ++itDecrease )
		{
			if( (*itDecrease).first == (*it).GetCode() )
			{
				int nLevel = nOriginalLevel - (*itDecrease).second;
				if( nLevel < 0 )
				{
					(*itDecrease).second = -1 * nLevel;
					nLevel = 0;
				}

				(*it).SetLevel( nLevel );

				break;
			}
		}

		if( (*it).GetLevel() > 0 )
		{
			applyState( *it );
		}

		(*it).SetLevel( nOriginalLevel );
	}
}

void StructCreature::applyState( StructState & state )
{
	// _applyEffectForState 함수와 대부분 내용 중첩되므로 수정 사항 있으면 같이 수정할 것
	switch( state.GetEffectType() )
	{
	case StructState::EF_PARAMETER_INC:
		incParameter( state.GetValue( 0 ), state.GetValue( 1 ) + state.GetValue( 2 ) * state.GetLevel(), false );
		incParameter( state.GetValue( 3 ), state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel(), false );
		incParameter2( state.GetValue( 6 ), state.GetValue( 7 ) + state.GetValue( 8 ) * state.GetLevel() );
		incParameter2( state.GetValue( 9 ), state.GetValue( 10 ) + state.GetValue( 11 ) * state.GetLevel() );
		incParameter( state.GetValue( 12 ), state.GetValue( 13 ) + state.GetValue( 14 ) * state.GetLevel(), false );
		incParameter( state.GetValue( 15 ), state.GetValue( 16 ) + state.GetValue( 17 ) * state.GetLevel(), false );

		break;
	case StructState::EF_PARAMETER_INC_WHEN_EQUIP_SHIELD:
		if( IsWearShield() )
		{
			incParameter( state.GetValue( 0 ), state.GetValue( 1 ) + state.GetValue( 2 ) * state.GetLevel(), false );
			incParameter( state.GetValue( 3 ), state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel(), false );
			incParameter2( state.GetValue( 6 ), state.GetValue( 7 ) + state.GetValue( 8 ) * state.GetLevel() );
			incParameter2( state.GetValue( 9 ), state.GetValue( 10 ) + state.GetValue( 11 ) * state.GetLevel() );
		}

		break;

	case StructState::EF_PARAMETER_AMP_WHEN_EQUIP_SHIELD:
		if( IsWearShield() )
		{
			ampParameter( state.GetValue( 0 ), state.GetValue( 1 ) + state.GetValue( 2 ) * state.GetLevel(), false );
			ampParameter( state.GetValue( 3 ), state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel(), false );
			ampParameter2( state.GetValue( 6 ), state.GetValue( 7 ) + state.GetValue( 8 ) * state.GetLevel() );
			ampParameter2( state.GetValue( 9 ), state.GetValue( 10 ) + state.GetValue( 11 ) * state.GetLevel() );
		}

		break;
	case StructState::EF_PARAMETER_INC_WHEN_EQUIP:
	case StructState::EF_PARAMETER_AMP_WHEN_EQUIP:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();

			DWORD nWeaponBitFlag = 0;

			if( !weapon_class )
			{
				break;
			}
			switch( weapon_class )
			{
			case ItemBase::CLASS_ONEHAND_SWORD:		nWeaponBitFlag = StructState::FLAG_EQUIP_ONEHAND_SWORD;				break;
			case ItemBase::CLASS_TWOHAND_SWORD:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_SWORD;				break;
			case ItemBase::CLASS_DAGGER:			nWeaponBitFlag = StructState::FLAG_EQUIP_DAGGER;					break;
			case ItemBase::CLASS_TWOHAND_SPEAR:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_SPEAR;				break;
			case ItemBase::CLASS_TWOHAND_AXE:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_AXE;				break;
			case ItemBase::CLASS_ONEHAND_MACE:		nWeaponBitFlag = StructState::FLAG_EQUIP_ONEHAND_MACE;				break;
			case ItemBase::CLASS_TWOHAND_MACE:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_MACE;				break;
			case ItemBase::CLASS_HEAVY_BOW:			nWeaponBitFlag = StructState::FLAG_EQUIP_HEAVY_BOW;					break;
			case ItemBase::CLASS_LIGHT_BOW:			nWeaponBitFlag = StructState::FLAG_EQUIP_LIGHT_BOW;					break;
			case ItemBase::CLASS_CROSSBOW:			nWeaponBitFlag = StructState::FLAG_EQUIP_CROSSBOW;					break;
			case ItemBase::CLASS_ONEHAND_STAFF:		nWeaponBitFlag = StructState::FLAG_EQUIP_ONEHAND_STAFF;				break;
			case ItemBase::CLASS_TWOHAND_STAFF:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_STAFF;				break;
			case ItemBase::CLASS_DOUBLE_SWORD:		nWeaponBitFlag = StructState::FLAG_EQUIP_DOUBLE_SWORD;				break;
			case ItemBase::CLASS_DOUBLE_DAGGER:		nWeaponBitFlag = StructState::FLAG_EQUIP_DOUBLE_DAGGER;				break;
			}

			if( !( ((DWORD)state.GetValue(0)) & nWeaponBitFlag ) )
				break;

			if( state.GetEffectType() == StructState::EF_PARAMETER_INC_WHEN_EQUIP )
			{
				incParameter( state.GetValue( 1 ), state.GetValue( 2 ) + state.GetLevel() * state.GetValue( 3 ), ( state.GetValue( 1 ) <= StructState::FLAG_LUK ) ? true : false );
				incParameter( state.GetValue( 4 ), state.GetValue( 5 ) + state.GetLevel() * state.GetValue( 6 ), ( state.GetValue( 4 ) <= StructState::FLAG_LUK ) ? true : false );
				incParameter2( state.GetValue( 7 ), state.GetValue( 8 ) + state.GetLevel() * state.GetValue( 9 ) );
			}
			else if( state.GetEffectType() == StructState::EF_PARAMETER_AMP_WHEN_EQUIP )
			{
				ampParameter( state.GetValue( 1 ), state.GetValue( 2 ) + state.GetLevel() * state.GetValue( 3 ), ( state.GetValue( 1 ) <= StructState::FLAG_LUK ) ? true : false );
				ampParameter( state.GetValue( 4 ), state.GetValue( 5 ) + state.GetLevel() * state.GetValue( 6 ), ( state.GetValue( 4 ) <= StructState::FLAG_LUK ) ? true : false );
				ampParameter2( state.GetValue( 7 ), state.GetValue( 8 ) + state.GetLevel() * state.GetValue( 9 ) );
			}
		}
		break;

	case StructState::EF_DOUBLE_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();

			if( !weapon_class )
			{
				break;
			}
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON )
			{
				if( weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
					break;
			}

			m_Attribute.fDoubleAttackRatio += state.GetValue(0) + state.GetLevel() * state.GetValue(1);
		}
		break;

	case StructState::EF_ADDITIONAL_DAMAGE_ON_ATTACK:
		{
			if( state.GetValue( 11 ) == 0 || state.GetValue( 11 ) == 99 )
				m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), Elemental::TYPE_NONE, (Elemental::Type) (int) state.GetValue( 8 ), state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel(), 0 ) );
			if( state.GetValue( 11 ) == 1 || state.GetValue( 11 ) == 99 )
				m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), Elemental::TYPE_NONE, (Elemental::Type) (int) state.GetValue( 8 ), state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel(), 0 ) );
		}
		break;

	case StructState::EF_AMP_ADDITIONAL_DAMAGE_ON_ATTACK:
		{
			if( state.GetValue( 11 ) == 0 || state.GetValue( 11 ) == 99 )
				m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), Elemental::TYPE_NONE, (Elemental::Type) (int) state.GetValue( 8 ), 0, state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel() ) );
			if( state.GetValue( 11 ) == 1 || state.GetValue( 11 ) == 99 )
				m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), Elemental::TYPE_NONE, (Elemental::Type) (int) state.GetValue( 8 ), 0, state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel() ) );
		}
		break;

	case StructState::EF_ADDITIONAL_DAMAGE_ON_SKILL:
		{
			if( state.GetValue( 11 ) == 0 || state.GetValue( 11 ) == 99 )
				m_vPhysicalSkillAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), (Elemental::Type) (int) state.GetValue( 5 ), (Elemental::Type) (int) state.GetValue( 8 ), state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel(), 0 ) );
			if( state.GetValue( 11 ) == 1 || state.GetValue( 11 ) == 99 )
				m_vMagicalSkillAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), (Elemental::Type) (int) state.GetValue( 5 ), (Elemental::Type) (int) state.GetValue( 8 ), state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel(), 0 ) );
		}
		break;

	case StructState::EF_AMP_ADDTIONAL_DAMAGE_ON_SKILL:
		{
			if( state.GetValue( 11 ) == 0 || state.GetValue( 11 ) == 99 )
				m_vPhysicalSkillAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), (Elemental::Type) (int) state.GetValue( 5 ), (Elemental::Type) (int) state.GetValue( 8 ), 0, state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel() ) );
			if( state.GetValue( 11 ) == 1 || state.GetValue( 11 ) == 99 )
				m_vMagicalSkillAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), (Elemental::Type) (int) state.GetValue( 5 ), (Elemental::Type) (int) state.GetValue( 8 ), 0, state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel() ) );
		}
		break;

	case StructState::EF_ADD_STATE_ON_ATTACK_OLD:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), 0, 0, 0, 0, StructState::NormalAttack, 99, NULL ) );
			m_vProcByAttack.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, StructProc::ATTACKEE, 0 );
		}
		break;

	// 코드량이 지나치게 많지만 가독성 때문에 우선 펼쳐둔다.
	case StructState::EF_ADD_STATE_ON_ATTACK:
	case StructState::EF_ADD_STATE_BY_SELF_ON_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( state.GetEffectType() )
			{
			case StructState::EF_ADD_STATE_ON_ATTACK:			nTarget = StructProc::ATTACKEE;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_ATTACK:	nTarget = StructProc::ATTACKER;	break;
			}

			StructStateProc *pProc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, nTarget, state.GetValue( 13 ) );
			m_vProcByAttack.push_back( _ATTACK_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), state.GetValue( 14 ), state.GetValue( 15 ), state.GetValue( 16 ), state.GetValue( 17 ), state.GetValue( 12 ), state.GetValue( 18 ), pProc ) );
		}
		break;

	case StructState::EF_ADD_STATE_ON_BEING_ATTACKED:
	case StructState::EF_ADD_STATE_BY_SELF_ON_BEING_ATTACKED:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( state.GetEffectType() )
			{
			case StructState::EF_ADD_STATE_ON_BEING_ATTACKED:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_BEING_ATTACKED:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByBeingAttacked.push_back( _ATTACK_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), state.GetValue( 14 ), state.GetValue( 15 ), state.GetValue( 16 ), state.GetValue( 17 ), state.GetValue( 12 ), state.GetValue( 18 ), NULL ) );
			m_vProcByBeingAttacked.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, nTarget, state.GetValue( 13 ) );
		}
		break;

	case StructState::EF_AMP_DAMAGE_BASED_ATTACK_POINT:
		{
			c_fixed10 damage = 0;

			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 14 ) != ItemBase::CLASS_EVERY_WEAPON &&
			    weapon_class != state.GetValue( 14 ) && 
			    weapon_class != state.GetValue( 15 ) &&
			    weapon_class != state.GetValue( 16 ) &&
			    weapon_class != state.GetValue( 17 ) )
				break;

			if ( state.GetValue( 4 ) == 0 || state.GetValue( 4 ) == 99 )
			{
				damage += GetAttackPointRight();
			}
			if ( state.GetValue( 4 ) == 1 || state.GetValue( 4 ) == 99 )
			{
				damage += GetMagicPoint();
			}

			damage *= state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel();

			c_fixed10 procRate = state.GetValue( 5 ) + state.GetValue( 6 ) * state.GetLevel();

			AdditionalDamageInfo additionalDamage( procRate, (Elemental::Type)99, (Elemental::Type)(int)state.GetValue( 7 ), damage, 0 );

			if ( state.GetValue( 10 ) == 1 )
			{
				m_vNormalAdditionalDamage.push_back( additionalDamage );
			}
			if ( state.GetValue( 11 ) == 1 )
			{
				m_vRangeAdditionalDamage.push_back( additionalDamage );
			}
			if ( state.GetValue( 12 ) == 1 )
			{
				m_vPhysicalSkillAdditionalDamage.push_back( additionalDamage );
			}
			if ( state.GetValue( 13 ) == 1 )
			{
				m_vMagicalSkillAdditionalDamage.push_back( additionalDamage );
			}
		}
		break;

	case StructState::EF_ADD_STATE_BY_SELF_ON_KILL:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			m_vProcByKill.push_back( _KILL_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), state.GetValue( 14 ), state.GetValue( 15 ), 0, 0, state.GetValue( 18 ), state.GetValue( 19 ), NULL ) );
			m_vProcByKill.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, StructProc::ATTACKER, state.GetValue( 13 ) );
		}
		break;

	case StructState::EF_ADD_STATE_BY_SELF_ON_DEAD:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			m_vProcByDead.push_back( _KILL_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), 0, 0, state.GetValue( 16 ), state.GetValue( 17 ), state.GetValue( 18 ), state.GetValue( 19 ), NULL ) );
			m_vProcByDead.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, StructProc::ATTACKEE, state.GetValue( 13 ) );
		}
		break;

	case StructState::EF_ADD_STATE_ON_CRITICAL_ATTACK:
	case StructState::EF_ADD_STATE_BY_SELF_ON_CRITICAL_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( state.GetEffectType() )
			{
			case StructState::EF_ADD_STATE_ON_CRITICAL_ATTACK:			nTarget = StructProc::ATTACKEE;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_CRITICAL_ATTACK:	nTarget = StructProc::ATTACKER;	break;
			}

			m_vProcByCriticalAttack.push_back( _ATTACK_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), state.GetValue( 14 ), state.GetValue( 15 ), state.GetValue( 16 ), state.GetValue( 17 ), state.GetValue( 12 ), state.GetValue( 18 ), NULL ) );
			m_vProcByCriticalAttack.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, nTarget, state.GetValue( 13 ) );
		}
		break;

	case StructState::EF_ADD_STATE_ON_BEING_CRITICAL_ATTACKED:
	case StructState::EF_ADD_STATE_BY_SELF_ON_BEING_CRITICAL_ATTACKED:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( state.GetEffectType() )
			{
			case StructState::EF_ADD_STATE_ON_BEING_CRITICAL_ATTACKED:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_BEING_CRITICAL_ATTACKED:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByBeingCriticalAttacked.push_back( _ATTACK_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), state.GetValue( 14 ), state.GetValue( 15 ), state.GetValue( 16 ), state.GetValue( 17 ), state.GetValue( 12 ), state.GetValue( 18 ), NULL ) );
			m_vProcByBeingCriticalAttacked.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, nTarget, state.GetValue( 13 ) );
		}
		break;

	case StructState::EF_ADD_STATE_ON_AVOID:
	case StructState::EF_ADD_STATE_BY_SELF_ON_AVOID:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( state.GetEffectType() )
			{
			case StructState::EF_ADD_STATE_ON_AVOID:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_AVOID:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByAvoid.push_back( _ATTACK_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), state.GetValue( 14 ), state.GetValue( 15 ), state.GetValue( 16 ), state.GetValue( 17 ), state.GetValue( 12 ), state.GetValue( 18 ), NULL ) );
			m_vProcByAvoid.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, nTarget, state.GetValue( 13 ) );
		}
		break;

	case StructState::EF_ADD_STATE_ON_BLOCK:
	case StructState::EF_ADD_STATE_BY_SELF_ON_BLOCK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( state.GetEffectType() )
			{
			case StructState::EF_ADD_STATE_ON_BLOCK:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_BLOCK:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByBlock.push_back( _ATTACK_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), state.GetValue( 14 ), state.GetValue( 15 ), state.GetValue( 16 ), state.GetValue( 17 ), state.GetValue( 12 ), state.GetValue( 18 ), NULL ) );
			m_vProcByBlock.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, nTarget, state.GetValue( 13 ) );
		}
		break;

	case StructState::EF_ADD_STATE_ON_PERFECT_BLOCK:
	case StructState::EF_ADD_STATE_BY_SELF_ON_PERFECT_BLOCK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON && weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( state.GetEffectType() )
			{
			case StructState::EF_ADD_STATE_ON_PERFECT_BLOCK:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_PERFECT_BLOCK:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByPerfectBlock.push_back( _ATTACK_TAG( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel(), state.GetValue( 14 ), state.GetValue( 15 ), state.GetValue( 16 ), state.GetValue( 17 ), state.GetValue( 12 ), state.GetValue( 18 ), NULL ) );
			m_vProcByPerfectBlock.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) state.GetValue( 0 ) ), state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel(), ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() ) * 100, nTarget, state.GetValue( 13 ) );
		}
		break;

	case StructState::EF_ADD_HP_ON_ATTACK:
		{
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON )
			{
				ItemBase::ItemClass weapon_class = GetWeaponClass();

				if( !weapon_class )
				{
					break;
				}

				if( weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
					break;
			}

			int nHPInc = state.GetValue(0) + state.GetLevel() * state.GetValue(1);
			int nMPInc = state.GetValue(2) + state.GetLevel() * state.GetValue(3);
			int nRatio = state.GetValue(6) + state.GetLevel() * state.GetValue(7);

			if( !nHPInc && !nMPInc )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( nRatio, 0, 0, 0, 0, StructState::NormalAttack, 99, NULL ) );
			m_vProcByAttack.back().proc = new StructHealProc( StructProc::ATTACKER, nHPInc, nMPInc );
		}
		break;

	case StructState::EF_ABSORB:
		{
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON )
			{
				ItemBase::ItemClass weapon_class = GetWeaponClass();

				if( !weapon_class )
				{
					break;
				}

				if( weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
					break;
			}

			float fHPAbsorbRatio = state.GetValue(0) + state.GetLevel() * state.GetValue(1);
			float fMPAbsorbRatio = state.GetValue(2) + state.GetLevel() * state.GetValue(3);
			int nRatio = state.GetValue(6) + state.GetLevel() * state.GetValue(7);

			if( !fHPAbsorbRatio && !fMPAbsorbRatio )
				break;

			StructAbsorbProc *pProc = new StructAbsorbProc( StructProc::ATTACKER, fHPAbsorbRatio, fMPAbsorbRatio );
			m_vProcByAttack.push_back( _ATTACK_TAG( nRatio, 0, 0, 0, 0, StructState::NormalAttack, 99, pProc ) );
		}
		break;

	case StructState::EF_STEAL:
	case StructState::EF_STEAL_WITH_REGEN_STOP:
		{
			if( state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON )
			{
				ItemBase::ItemClass weapon_class = GetWeaponClass();

				if( !weapon_class )
				{
					break;
				}

				if( weapon_class != state.GetValue( 8 ) && weapon_class != state.GetValue( 9 ) && weapon_class != state.GetValue( 10 ) && weapon_class != state.GetValue( 11 ) )
					break;
			}

			int nHPSteal = state.GetValue(0) + state.GetLevel() * state.GetValue(1);
			int nMPSteal = state.GetValue(2) + state.GetLevel() * state.GetValue(3);
			int nRatio = state.GetValue(6) + state.GetLevel() * state.GetValue(7);

			if( !nHPSteal && !nMPSteal )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( nRatio, 0, 0, 0, 0, StructState::NormalAttack, 99, NULL ) );
			m_vProcByAttack.back().proc = new StructStealProc( StructProc::ATTACKER, nHPSteal, nMPSteal );

			if( state.GetEffectType() == StructState::EF_STEAL_WITH_REGEN_STOP )
			{
				if( state.GetValue( 12 ) )
					m_StatusFlag.On( STATUS_HP_REGEN_STOPPED );
				if( state.GetValue( 13 ) )
					m_StatusFlag.On( STATUS_MP_REGEN_STOPPED );
			}
		}
		break;

	case StructState::EF_DAMAGE_REFLECT_PERCENT:
		{
			m_vDamageReflectInfo.push_back( DamageReflectInfo( state.GetValue( 6 ) + state.GetLevel() * state.GetValue( 7 ), state.GetValue( 9 ) * GameRule::DEFAULT_UNIT_SIZE,
				(Elemental::Type) (int) state.GetValue( 8 ), 0,
				state.GetValue( 0 ) + state.GetLevel() * state.GetValue( 1 ),
				state.GetValue( 2 ) + state.GetLevel() * state.GetValue( 3 ),
				state.GetValue( 4 ) + state.GetLevel() * state.GetValue( 5 ),
				state.GetValue( 10 ) ) );
		}
		break;

	case StructState::EF_DAMAGE_REFLECT:
		{
			m_vDamageReflectInfo.push_back( DamageReflectInfo( state.GetValue( 6 ) + state.GetLevel() * state.GetValue( 7 ), GameRule::REFLECT_RANGE,
				(Elemental::Type) (int) state.GetValue( 8 ), state.GetValue( 0 ) + state.GetLevel() * state.GetValue( 1 ),
				0.0f, 
				0.0f,
				0.0f,
				state.GetValue( 9 ) ) );
		}
		break;

	case StructState::EF_DAMAGE_REFLECT_WHEN_EQUIP_SHIELD:
		{
			if( IsWearShield() )
			{
				m_vStateReflectInfo.push_back( StateReflectInfo( static_cast< StructState::StateCode >( (int)state.GetValue( 0 ) ), state.GetValue( 3 ) + state.GetLevel() * state.GetValue( 4 ), state.GetValue( 1 ) + state.GetLevel() * state.GetValue( 2 ) ) );
			}
		}
		break;

	case StructState::EF_REGEN_ADD:
		{
			m_Attribute.fHPRegenPoint += state.GetValue(0) + state.GetLevel() * state.GetValue(1);
			m_Attribute.fMPRegenPoint += state.GetValue(2) + state.GetLevel() * state.GetValue(3);
		}
		break;

	case StructState::EF_AMP_RECEIVE_DAMAGE:
		{
			// ApplyType(근거리/원거리 설정)은 일반 공격만 적용된다
			int nApplyType = state.GetValue( 0 );
			if( nApplyType == 1 )
			{
				m_NormalStatePenalty.fDamage += ( state.GetValue(7) + state.GetLevel() * state.GetValue(8) );
			}
			else if( nApplyType == 2 )
			{
				m_RangeStatePenalty.fDamage += ( state.GetValue(7) + state.GetLevel() * state.GetValue(8) );
			}
			else if( nApplyType == 99 )
			{
				m_NormalStatePenalty.fDamage += ( state.GetValue(7) + state.GetLevel() * state.GetValue(8) );
				m_RangeStatePenalty.fDamage += ( state.GetValue(7) + state.GetLevel() * state.GetValue(8) );
			}

			m_StateStatePenalty.fDamage += ( state.GetValue(3) + state.GetLevel() * state.GetValue(4) );
			m_PhysicalSkillStatePenalty.fDamage += ( state.GetValue(5) + state.GetLevel() * state.GetValue(6) );
			m_MagicalSkillStatePenalty.fDamage += ( state.GetValue(9) + state.GetLevel() * state.GetValue(10) );
		}
		break;

	case StructState::EF_INC_HATE:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();

			if( !weapon_class )
				break;

			if( weapon_class != state.GetValue( 6 ) && state.GetValue( 6 ) != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != state.GetValue( 7 ) && state.GetValue( 7 ) != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != state.GetValue( 8 ) && state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != state.GetValue( 9 ) && state.GetValue( 9 ) != ItemBase::CLASS_EVERY_WEAPON )
				break;

			m_vHateMod.push_back( HateModifier( state.GetValue(10), state.GetValue(11), 0, state.GetValue(0) + state.GetValue(1) * state.GetLevel() ) );
		}
		break;

	case StructState::EF_AMP_HATE:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();

			if( !weapon_class )
				break;

			if( weapon_class != state.GetValue( 6 ) && state.GetValue( 6 ) != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != state.GetValue( 7 ) && state.GetValue( 7 ) != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != state.GetValue( 8 ) && state.GetValue( 8 ) != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != state.GetValue( 9 ) && state.GetValue( 9 ) != ItemBase::CLASS_EVERY_WEAPON )
				break;

			m_vHateMod.push_back( HateModifier( state.GetValue(10), state.GetValue(11), state.GetValue(0) + state.GetValue(1) * state.GetLevel(), 0 ) );
		}
		break;

	case StructState::EF_FORCE_CHIP:
		{
			c_fixed10 fNormalStatePenalty = ( state.GetValue(0) + state.GetValue(1) * state.GetLevel() );
			c_fixed10 fRangeStatePenalty = ( state.GetValue(0) + state.GetValue(1) * state.GetLevel() );
			c_fixed10 fPhysicalSkillStatePenalty = ( state.GetValue(2) + state.GetValue(3) * state.GetLevel() );

			if( IsPlayer() || IsSummon() )
			{
				fNormalStatePenalty = fNormalStatePenalty * 0.25f;
				fRangeStatePenalty = fRangeStatePenalty * 0.25f;
				fPhysicalSkillStatePenalty = fPhysicalSkillStatePenalty * 0.25f;
			}

			m_NormalStatePenalty.fDamage += fNormalStatePenalty;
			m_RangeStatePenalty.fDamage += fRangeStatePenalty;
			m_PhysicalSkillStatePenalty.fDamage += fPhysicalSkillStatePenalty;
		}
		break;

	case StructState::EF_SOUL_CHIP:
		{
			c_fixed10 fMagicalSkillStatePenalty = ( state.GetValue(2) + state.GetLevel() * state.GetValue(3) );

			if( IsPlayer() || IsSummon() )
			{
				fMagicalSkillStatePenalty = fMagicalSkillStatePenalty * 0.25f;
			}

			m_MagicalSkillStatePenalty.fDamage += fMagicalSkillStatePenalty;
		}
		break;

	case StructState::EF_HEALING_CHIP:
		{
			m_fHealRatio += ( state.GetValue(2) + state.GetLevel() * state.GetValue(3) );
		}
		break;

	case StructState::EF_LUNAR_CHIP:
		{
			m_nLuna = 1;
			c_fixed10 fNormalStatePenalty = ( state.GetValue(0) + state.GetValue(1) * state.GetLevel() );
			c_fixed10 fRangeStatePenalty = ( state.GetValue(0) + state.GetValue(1) * state.GetLevel() );
			c_fixed10 fPhysicalSkillStatePenalty = ( state.GetValue(2) + state.GetValue(3) * state.GetLevel() );
			c_fixed10 fMagicalSkillStatePenalty = ( state.GetValue(2) + state.GetValue(3) * state.GetLevel() );
			c_fixed10 fStateStatePenalty = ( state.GetValue(12) + state.GetValue(13) * state.GetLevel() );

			if( IsPlayer() || IsSummon() )
			{
				fNormalStatePenalty = fNormalStatePenalty * 0.25f;
				fRangeStatePenalty = fRangeStatePenalty * 0.25f;
				fPhysicalSkillStatePenalty = fPhysicalSkillStatePenalty * 0.25f;
				fMagicalSkillStatePenalty = fMagicalSkillStatePenalty * 0.25f;
				fStateStatePenalty = fStateStatePenalty * 0.25f;
			}

			m_NormalStatePenalty.fDamage += fNormalStatePenalty;
			m_RangeStatePenalty.fDamage += fRangeStatePenalty;
			m_PhysicalSkillStatePenalty.fDamage += fPhysicalSkillStatePenalty;
			m_MagicalSkillStatePenalty.fDamage += fMagicalSkillStatePenalty;
			m_StateStatePenalty.fDamage += fStateStatePenalty;
		}
		break;

	case StructState::EF_MP_COST_INC:
		{
			float fCostReduce = state.GetValue(0) + state.GetLevel() * state.GetValue(1);
			int nElementalType = state.GetValue(5);

			if( nElementalType != 99 )
			{
				if( state.GetValue( 10 ) == 99 || state.GetValue( 10 ) == 1 )
				{
					if( state.GetValue( 11 ) == 99 || state.GetValue( 11 ) == 0 )
					{
						m_GoodPhysicalElementalSkillStateMod[nElementalType].fManaCostRatio += fCostReduce;
					}

					if( state.GetValue( 11 ) == 99 || state.GetValue( 11 ) == 1 )
					{
						m_BadPhysicalElementalSkillStateMod[nElementalType].fManaCostRatio += fCostReduce;
					}
				}

				if( state.GetValue( 10 ) == 99 || state.GetValue( 10 ) == 2 )
				{
					if( state.GetValue( 11 ) == 99 || state.GetValue( 11 ) == 0 )
					{
						m_GoodMagicalElementalSkillStateMod[nElementalType].fManaCostRatio += fCostReduce;
					}

					if( state.GetValue( 11 ) == 99 || state.GetValue( 11 ) == 1 )
					{
						m_BadMagicalElementalSkillStateMod[nElementalType].fManaCostRatio += fCostReduce;
					}
				}
			}
			else
			{
				for( int i = 0; i < Elemental::TYPE_COUNT; ++i )
				{
					if( state.GetValue( 10 ) == 99 || state.GetValue( 10 ) == 1 )
					{
						if( state.GetValue( 11 ) == 99 || state.GetValue( 11 ) == 0 )
						{
							m_GoodPhysicalElementalSkillStateMod[i].fManaCostRatio += fCostReduce;
						}

						if( state.GetValue( 11 ) == 99 || state.GetValue( 11 ) == 1 )
						{
							m_BadPhysicalElementalSkillStateMod[i].fManaCostRatio += fCostReduce;
						}
					}

					if( state.GetValue( 10 ) == 99 || state.GetValue( 10 ) == 2 )
					{
						if( state.GetValue( 11 ) == 99 || state.GetValue( 11 ) == 0 )
						{
							m_GoodMagicalElementalSkillStateMod[i].fManaCostRatio += fCostReduce;
						}

						if( state.GetValue( 11 ) == 99 || state.GetValue( 11 ) == 1 )
						{
							m_BadMagicalElementalSkillStateMod[i].fManaCostRatio += fCostReduce;
						}
					}
				}
			}
		}
		break;

	case StructState::EF_ADD_PARAMETER_ON_NORMAL_ATTACK:
		{
			if( state.GetValue( 9 ) == 99 || state.GetValue( 9 ) == 1 )
			{
				m_RangeStateAdvantage.fDamage += state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel();
				m_RangeStateAdvantage.fCritical += state.GetValue( 3 ) * state.GetLevel();
				m_RangeStateAdvantage.nCritical += state.GetValue( 4 ) * state.GetLevel();
				m_RangeStateAdvantage.fHate += state.GetValue( 5 ) + state.GetValue( 6 ) * state.GetLevel();
			}

			if( state.GetValue( 9 ) == 99 || state.GetValue( 9 ) == 0 )
			{
				m_NormalStateAdvantage.fDamage += state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel();
				m_NormalStateAdvantage.fCritical += state.GetValue( 3 ) * state.GetLevel();
				m_NormalStateAdvantage.nCritical += state.GetValue( 4 ) * state.GetLevel();
				m_NormalStateAdvantage.fHate += state.GetValue( 6 ) * state.GetLevel();
			}
		}
		break;

	case StructState::EF_ADD_PARAMETER_ON_SKILL:
		{
			float fDamage = state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel();
			float fMagicDamage = state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel();
			int nCritical = state.GetValue( 4 ) * state.GetLevel();
			int nElementalType = state.GetValue( 5 );
			float fHate = state.GetValue( 6 ) * state.GetLevel();
			float fCoolTime = state.GetValue( 7 ) * state.GetLevel();
			bool bExhaustive = state.GetValue( 8 );
			int nApplySkillType = state.GetValue( 9 );
			int nApplyToHarmful = state.GetValue( 10 );
			AR_TIME nCastingSpeedApplyTime = state.GetValue( 11 ) * 100;
			int fCastingSpeed = state.GetValue( 12 ) + state.GetValue( 13 ) * state.GetLevel();

			if( nElementalType != 99 )
			{
				if( nApplySkillType == 1 || nApplySkillType == 99 )
				{
					if( nApplyToHarmful == 1 || nApplyToHarmful == 99 )
					{
						m_BadPhysicalElementalSkillStateMod[nElementalType].apply( fDamage, fMagicDamage, nCritical, fHate, fCoolTime, nCastingSpeedApplyTime, fCastingSpeed, bExhaustive, state.GetCode() );
					}
					if( nApplyToHarmful == 0 || nApplyToHarmful == 99 )
					{
						m_GoodPhysicalElementalSkillStateMod[nElementalType].apply( fDamage, fMagicDamage, nCritical, fHate, fCoolTime, nCastingSpeedApplyTime, fCastingSpeed, bExhaustive, state.GetCode() );
					}
				}
				if( nApplySkillType == 2 || nApplySkillType == 99 )
				{
					if( nApplyToHarmful == 1 || nApplyToHarmful == 99 )
					{
						m_BadMagicalElementalSkillStateMod[nElementalType].apply( fDamage, fMagicDamage, nCritical, fHate, fCoolTime, nCastingSpeedApplyTime, fCastingSpeed, bExhaustive, state.GetCode() );
					}
					if( nApplyToHarmful == 0 || nApplyToHarmful == 99 )
					{
						m_GoodMagicalElementalSkillStateMod[nElementalType].apply( fDamage, fMagicDamage, nCritical, fHate, fCoolTime, nCastingSpeedApplyTime, fCastingSpeed, bExhaustive, state.GetCode() );
					}
				}
			}
			else
			{
				for( int i = 0; i < Elemental::TYPE_COUNT; ++i )
				{
					if( nApplySkillType == 1 || nApplySkillType == 99 )
					{
						if( nApplyToHarmful == 1 || nApplyToHarmful == 99 )
						{
							m_BadPhysicalElementalSkillStateMod[i].apply( fDamage, fMagicDamage, nCritical, fHate, fCoolTime, nCastingSpeedApplyTime, fCastingSpeed, bExhaustive, state.GetCode() );
						}
						if( nApplyToHarmful == 0 || nApplyToHarmful == 99 )
						{
							m_GoodPhysicalElementalSkillStateMod[i].apply( fDamage, fMagicDamage, nCritical, fHate, fCoolTime, nCastingSpeedApplyTime, fCastingSpeed, bExhaustive, state.GetCode() );
						}

					}
					if( nApplySkillType == 2 || nApplySkillType == 99 )
					{
						if( nApplyToHarmful == 1 || nApplyToHarmful == 99 )
						{
							m_BadMagicalElementalSkillStateMod[i].apply( fDamage, fMagicDamage, nCritical, fHate, fCoolTime, nCastingSpeedApplyTime, fCastingSpeed, bExhaustive, state.GetCode() );
						}
						if( nApplyToHarmful == 0 || nApplyToHarmful == 99 )
						{
							m_GoodMagicalElementalSkillStateMod[i].apply( fDamage, fMagicDamage, nCritical, fHate, fCoolTime, nCastingSpeedApplyTime, fCastingSpeed, bExhaustive, state.GetCode() );
						}
					}
				}
			}
		}
		break;

		//조작 관련 상태 이상
	case StructState::EF_MEZZ:
		// 모든 조작 불가
		if( state.GetValue(0) )
		{
			m_StatusFlag.Off( STATUS_MOVABLE );
			m_StatusFlag.Off( STATUS_ATTACKABLE );
			m_StatusFlag.Off( STATUS_SKILL_CASTABLE );
			m_StatusFlag.Off( STATUS_MAGIC_CASTABLE );
			m_StatusFlag.Off( STATUS_ITEM_USABLE );
		}

		// 이동 불가
		if( state.GetValue(1) )
			m_StatusFlag.Off( STATUS_MOVABLE );

		// 공격 불가
		if( state.GetValue(2) )
			m_StatusFlag.Off( STATUS_ATTACKABLE );

		// 물리 스킬 시전 불가
		if( state.GetValue(3) )
			m_StatusFlag.Off( STATUS_SKILL_CASTABLE );

		// 마법 스킬 시전 불가
		if( state.GetValue(4) )
			m_StatusFlag.Off( STATUS_MAGIC_CASTABLE );

		// 피격 불가
		if( state.GetValue(5) )
			m_StatusFlag.Off( STATUS_MORTAL );

		// HP 감소
		m_fMaxHPAmplifier -= state.GetValue(8) + state.GetValue(9) * state.GetLevel();
		m_nMaxHP -= state.GetValue(6) + state.GetValue(7) * state.GetLevel();

		// MP 감소
		m_fMaxMPAmplifier -= state.GetValue(12) + state.GetValue(13) * state.GetLevel();
		m_nMaxMP -= state.GetValue(10) + state.GetValue(11) * state.GetLevel();

		break;

	case StructState::EF_TRANSFORMATION:
		// 이동 불가
		if( state.GetValue(1) )
			m_StatusFlag.Off( STATUS_MOVABLE );

		// 일반 공격 불가
		if( state.GetValue(2) )
			m_StatusFlag.Off( STATUS_ATTACKABLE );

		// (물리)스킬 사용 불가
		if( state.GetValue(3) == 1 )
			m_StatusFlag.Off( STATUS_SKILL_CASTABLE );

		// 마법 사용 불가
		if( state.GetValue(3) == 2 )
			m_StatusFlag.Off( STATUS_MAGIC_CASTABLE );

		// 마법/스킬 모두 사용 불가
		if( state.GetValue(3) == 3 )
		{
			m_StatusFlag.Off( STATUS_SKILL_CASTABLE );
			m_StatusFlag.Off( STATUS_MAGIC_CASTABLE );
		}

		// 피격 불가
		if( state.GetValue(4) )
			m_StatusFlag.Off( STATUS_MORTAL );

		// 아이템 사용 불가
		if( state.GetValue(5) )
			m_StatusFlag.Off( STATUS_ITEM_USABLE );

		incParameter2( state.GetValue(6), state.GetValue(7) + state.GetValue(8) * state.GetLevel() );
		incParameter2( state.GetValue(9), state.GetValue(10) + state.GetValue(11) * state.GetLevel() );

		break;

	case StructState::EF_ADD_HP_MP_ON_CRITICAL:
		m_vAddHPMPOnCritical.push_back( AddHPMPOnCriticalInfo( state.GetValue(0) + state.GetValue(1) * state.GetLevel(), state.GetValue(2) + state.GetValue(3) * state.GetLevel(), state.GetValue(4) + state.GetValue(5) * state.GetLevel() ) );
		break;

	case StructState::EF_HEALING_AMPLIFY:
		{
			c_fixed10 fHPHealRatio = state.GetValue(0) + state.GetValue(1) * state.GetLevel();
			c_fixed10 fMPHealRatio = state.GetValue(2) + state.GetValue(3) * state.GetLevel();

			m_fHealRatio += fHPHealRatio;
			m_fMPHealRatio += fMPHealRatio;

			m_nAdditionalHeal += state.GetValue(4) + state.GetValue(5) * state.GetLevel();
			m_nAdditionalMPHeal += state.GetValue(6) + state.GetValue(7) * state.GetLevel();
		}
		break;

	case StructState::EF_DETECT_HIDE:
		m_fDetectHideRange += ( state.GetValue(0) + state.GetLevel() * state.GetValue(1) ) * GameRule::DEFAULT_UNIT_SIZE;
		break;

	case StructState::EF_SKILL_INTERRUPTION:
		for( int i = 0 ; i < 12 && state.GetValue( i ) ; ++i )
			m_vInterruptedSkill.push_back( state.GetValue( i ) );
		break;

	case StructState::EF_DAMAGE_REDUCE_WITH_RACE_BY_PERCENT:
		m_vDamageReducePercentInfo.push_back( DamageReduceInfo( state.GetValue(6), state.GetValue(0) + state.GetLevel() * state.GetValue(1), state.GetValue(2) + state.GetLevel() * state.GetValue(3),
			state.GetValue(4) + state.GetLevel() * state.GetValue(5), state.GetValue(7), state.GetValue(8), state.GetValue(9), state.GetValue(10), state.GetValue(11) ) );
		break;

	case StructState::EF_DAMAGE_REDUCE_WITH_RACE_BY_VALUE:
		m_vDamageReduceValueInfo.push_back( DamageReduceInfo( state.GetValue(6), state.GetValue(0) + state.GetLevel() * state.GetValue(1), state.GetValue(2) + state.GetLevel() * state.GetValue(3),
			state.GetValue(4) + state.GetLevel() * state.GetValue(5), state.GetValue(7), state.GetValue(8), state.GetValue(9), state.GetValue(10), state.GetValue(11) ) );
		break;

	case StructState::EF_MANA_SHIELD:
		{
			int nTargetType = state.GetValue(4);
			if( nTargetType == 1 || nTargetType == 99 )
				m_fPhysicalDamageManaShieldAbsorbRatio += state.GetValue(0) + state.GetLevel() * state.GetValue(1);
			if( nTargetType == 2 || nTargetType == 99 )
				m_fMagicalDamageManaShieldAbsorbRatio += state.GetValue(0) + state.GetLevel() * state.GetValue(1);
		}
		break;

	case StructState::EF_AMP_AND_INC_ITEM_CHANCE:
		// nItemChance == 100 이면 아이템 드랍 체크시 해당 아이템의 드랍율이 2배로 적용됨(드랍율 % 증폭 개념 변수임)
		// GetValue( 0 ) + GetValue( 1 ) * GetLevel() == 1.0 -> 2배 증폭(nItemChance = 100 이 되야 함)
		// 정수 처리 특성상 결과값의 소수 2째 자리 미만은 전부 버려짐
		m_Attribute.fItemChance += ( state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel() ) * 100;
		m_fEXPMod += state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel();
		break;

	case StructState::EF_CHANGING_GROUP:
		m_nChangingGroup = state.GetValue(0);
		break;	

	case StructState::EF_HUNTING_TRAINING:
		{
			for( int i = 0; i < 3; i++ )
			{
				int nCreatureType = state.GetValue( i * 5 );

				c_fixed10 fDamage = state.GetValue( i * 5 + 1 ) + state.GetValue( i * 5 + 2 ) * state.GetLevel();
				c_fixed10 fAvoid = state.GetValue( i * 5 + 3 ) + state.GetValue( i * 5 + 4 ) * state.GetLevel();

				if( nCreatureType == CREATURE_ALL )
				{
					for( int i = 0 ; i < MAX_CREATURE_TYPE_NUMBER ; ++i )
					{
						m_Expert[ i ].fDamage += fDamage;
						m_Expert[ i ].fAvoid += fAvoid;
					}
				}
				else if( nCreatureType >= 0 && nCreatureType < MAX_CREATURE_TYPE_NUMBER )
				{
					m_Expert[ nCreatureType ].fDamage += fDamage;
					m_Expert[ nCreatureType ].fAvoid += fAvoid;
				}
				else
				{
					assert( 0 && "CREATURE DATA TYPE ERROR!" );
				}
			}

		}
		break;

	case StructState::EF_RESIST_HARMFUL_STATE:
		{
			for( int i = 0; i < 6; ++i )
			{
				StructState::StateCode code = static_cast< StructState::StateCode >( (int)state.GetValue( i * 3 ) );
				c_fixed10 rate = state.GetValue( i * 3 + 1 ) + state.GetValue( i * 3 + 2 ) * state.GetLevel();

				if( !code ) continue;

				std::map< StructState::StateCode, c_fixed10 >::iterator it = m_mapResistHarmfulState.find( code );
				if( it == m_mapResistHarmfulState.end() )
				{
					m_mapResistHarmfulState.insert( std::make_pair< StructState::StateCode, c_fixed10 >( code, rate ) );
				}
				else
				{
					(*it).second += rate;
				}
			}
		}
		break;

	case StructState::EF_ADD_REDUCE_DAMAGE_ON_ATTACK:
		{
			if( state.GetValue( 7 ) == 0 || state.GetValue( 7 ) == 99 )
				m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 3 ) + state.GetValue( 4 ) * state.GetLevel(), Elemental::TYPE_NONE, (Elemental::Type) (int) state.GetValue( 5 ), state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel(), 0 ) );
			if( state.GetValue( 7 ) == 1 || state.GetValue( 7 ) == 99 )
				m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( state.GetValue( 3 ) + state.GetValue( 4 ) * state.GetLevel(), Elemental::TYPE_NONE, (Elemental::Type) (int) state.GetValue( 5 ), state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel(), 0 ) );
			m_vDamageReduceValueInfo.push_back( DamageReduceInfo( state.GetValue(15) + state.GetLevel() * state.GetValue(16), state.GetValue(9) + state.GetLevel() * state.GetValue(10), state.GetValue(11) + state.GetLevel() * state.GetValue(12),
				state.GetValue(13) + state.GetLevel() * state.GetValue(14), state.GetValue(17), state.GetValue(18), state.GetValue(19), state.GetValue(19), state.GetValue(19) ) );
			// TODO: 유형 체크에 20번 값을 중복하여 넣은 것은 뒤의 두 값은 사용되지 않기 때문. 동작에는 차이가 없을 것이나 차후 리뷰가 필요할 듯
		}
		break;

	case StructState::EF_DAMAGE_REDUCE_BY_PERCENT_AND_VALUE:
		{
			m_vDamageReducePercentInfo.push_back( DamageReduceInfo( state.GetValue(6), state.GetValue(0) + state.GetLevel() * state.GetValue(1), state.GetValue(2) + state.GetLevel() * state.GetValue(3),
				state.GetValue(4) + state.GetLevel() * state.GetValue(5), state.GetValue(7), state.GetValue(8), state.GetValue(9), state.GetValue(9), state.GetValue(9) ) );
			m_vDamageReduceValueInfo.push_back( DamageReduceInfo( state.GetValue(16), state.GetValue(10) + state.GetLevel() * state.GetValue(11), state.GetValue(12) + state.GetLevel() * state.GetValue(13),
				state.GetValue(14) + state.GetLevel() * state.GetValue(15), state.GetValue(17), state.GetValue(18), state.GetValue(19), state.GetValue(19), state.GetValue(19) ) );
			// TODO: 유형 체크에 20번 값을 중복하여 넣은 것은 뒤의 두 값은 사용되지 않기 때문. 동작에는 차이가 없을 것이나 차후 리뷰가 필요할 듯
		}
		break;

	case StructState::EF_INCREASE_ENERGY_UNCONSUMPTION_RATE:
		{
			m_nEnergyUnconsumptionRate += state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel();
		}
		break;

	case StructState::EF_MISC:
		{
			c_fixed10 fValue1 = state.GetValue(0) + state.GetValue(1) * state.GetLevel();
			c_fixed10 fValue2 = state.GetValue(2) + state.GetValue(3) * state.GetLevel();
			c_fixed10 fValue3 = state.GetValue(4) + state.GetValue(5) * state.GetLevel();
			c_fixed10 fValue4 = state.GetValue(6) + state.GetValue(7) * state.GetLevel();
			c_fixed10 fValue5 = state.GetValue(8) + state.GetValue(9) * state.GetLevel();
			c_fixed10 fValue6 = state.GetValue(10) + state.GetValue(11) * state.GetLevel();

			switch( state.GetCode() )
			{
				// 일단 메즈 스킬들
			case StructState::NIGHTMARE:
				m_Attribute.fMagicDefence += fValue1;
			case StructState::STUN:
			case StructState::SLEEP:
			case StructState::SEAL:
			case StructState::SHINE_WALL:
			case StructState::CARELESSNESS:
			case StructState::FALL_FROM_SUMMON:
				m_StatusFlag.Off( STATUS_ATTACKABLE );
				m_StatusFlag.Off( STATUS_SKILL_CASTABLE );
				m_StatusFlag.Off( STATUS_MAGIC_CASTABLE );
				m_StatusFlag.Off( STATUS_ITEM_USABLE );
			case StructState::HOLD:
			case StructState::EARTH_RESTRICTION:
			case StructState::FROZEN_SNARE:
				m_StatusFlag.Off( STATUS_MOVABLE );
				break;

			case StructState::STONECURSE:
				m_StatusFlag.Off( STATUS_MORTAL );
			case StructState::STONECURSE_MORTAL:
				m_StatusFlag.On( STATUS_HP_REGEN_STOPPED );
				m_StatusFlag.On( STATUS_MP_REGEN_STOPPED );
			case StructState::FROZEN:
			case StructState::TOTAL_FROZEN:
				m_AttributeAmplifier.fDefence += fValue1;
				m_AttributeAmplifier.fMagicDefence += fValue2;
				m_StatusFlag.Off( STATUS_MOVABLE );
				m_StatusFlag.Off( STATUS_ATTACKABLE );
				m_StatusFlag.Off( STATUS_SKILL_CASTABLE );
				m_StatusFlag.Off( STATUS_ITEM_USABLE );
			case StructState::MUTE:
				m_StatusFlag.Off( STATUS_MAGIC_CASTABLE );
				break;

			case StructState::FEAR:
				m_StatusFlag.On( STATUS_FEARED );
				break;

			case StructState::PROTECTING_FORCE_OF_BEGINNING:
				{
					m_StatusFlag.Off( STATUS_MORTAL );
					m_StatusFlag.Off( STATUS_ATTACKABLE );

					std::set< int > allowedSkillSet;
					allowedSkillSet.insert( 4001 );
					allowedSkillSet.insert( 4002 );
					m_vAllowedSkill.push_back( allowedSkillSet );
				}
				break;

				// 아이템 드랍 확률 증가(2007-10-30 삭제 -> 지속효과 유형 201번으로 대체)
				//case StructState::INC_ITEM_CHANCE:
				//	m_Attribute.fItemChance += fValue1;
				//	break;

			case StructState::HUNTING_CREATURE_CARD:
				m_fCreatureCardChance += fValue1;
				break;

				// 블럭챤스 증가
			case StructState::INC_BLOCK_CHANCE:
				m_Attribute.fBlockChance += fValue1;
				break;

				// 은신, 도망자의 기척<은신><데스매치 전용>
			case StructState::HIDE:
			case StructState::TRACE_OF_FUGITIVE:
				m_StatusFlag.On( STATUS_HIDING );
				m_StatusFlag.Off( STATUS_ATTACKABLE );

				m_AttributeAmplifier.fMoveSpeed += fValue1;
				break;

			case StructState::FRENZY:
				{
					m_AttributeAmplifier.fAttackSpeedRight += fValue1;
					if( IsUsingDoubleWeapon() )
						m_AttributeAmplifier.fAttackSpeedLeft += fValue1;

					// fRidingSpeed 수치가 생기면서 라이딩 중에도 기본 속도에 관여할 수 있게 변경 ( 2013-12-20 )
					m_AttributeAmplifier.fMoveSpeed += fValue2;
					m_Attribute.fCritical = 100;

					break;
				}

			case StructState::BURNING_STYLE:
				{
					m_AttributeAmplifier.fAttackPointRight += fValue1;

					if( IsUsingDoubleWeapon() )
						m_AttributeAmplifier.fAttackPointLeft += fValue1;

					m_AttributeAmplifier.fCritical += fValue2;
					m_AttributeAmplifier.fDefence += fValue3;
					m_AttributeAmplifier.fAvoid += fValue4;
					m_fHateRatio += fValue5;

					break;
				}

			case StructState::DUSK_STYLE:
				{
					m_AttributeAmplifier.fAccuracyRight += fValue1;

					if( IsUsingDoubleWeapon() )
						m_AttributeAmplifier.fAccuracyRight += fValue1;

					m_AttributeAmplifier.fCastingSpeed += fValue2;
					m_fMaxHPAmplifier += fValue3;
					m_fHateRatio += fValue4;

					break;
				}

			case StructState::AGILE_STYLE:
				{
					m_AttributeAmplifier.fMoveSpeed += fValue1;
					m_AttributeAmplifier.fAvoid += fValue2;
					//m_Attribute.fAvoid += fValue2;
					m_AttributeAmplifier.fAttackPointRight += fValue3;

					if( IsUsingDoubleWeapon() )
						m_AttributeAmplifier.fAttackPointLeft += fValue3;

					break;
				}

			case StructState::LIGHTNING_FORCE_CONGESTION:
				{
					m_AttributeAmplifier.fAttackPointRight += fValue1;
					m_fAttackPointRightWithoutWeapon *= fValue1;	// 귀찮아서 나중에 증폭 안하고 그냥...;;
					m_AttributeAmplifier.fAttackSpeedRight += fValue2;
					if( IsUsingDoubleWeapon() )
					{
						m_AttributeAmplifier.fAttackPointLeft += fValue1;
						m_fAttackPointLeftWithoutWeapon *= fValue1;
						m_AttributeAmplifier.fAttackSpeedLeft += fValue2;
					}
					m_fResistHarmfulState += fValue3;

					break;
				}

			}

		}
	case StructState::EF_ATTACK_LUNA:
	{
		m_nAttackLuna = 1;
		break;
	}
	}
}

void StructPlayer::applyState( StructState & state )
{
	if( state.GetEffectType() == StructState::EF_RIDING )
	{
		if( !isMoveSpeedFixed() )
		{
			m_Attribute.fRidingSpeed = state.GetValue( 0 );
			
			bool bUseFasterMoveSpeed = state.GetValue( 8 );
			m_bUseFasterSpeedInRiding = bUseFasterMoveSpeed;
		}

		m_nRidingStateCode = state.GetCode();

		return;
	}

	switch( state.GetEffectType() )
	{
	case StructState::EF_CHANGING_FORM:
		{
			// 값 1과 2가 둘 다 0이면 자신의 본래 이속을 따름
			if( state.GetValue( 0 ).get() || state.GetValue( 1 ).get() )
			{
				m_Attribute.fMoveSpeed = state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel();
				m_StatusFlag.On( STATUS_MOVE_SPEED_FIXED );
			}

			m_StatusFlag.On( STATUS_FORM_CHANGED );
			m_StatusFlag.Set( STATUS_SKILL_CASTABLE, state.GetValue( 2 ) );
			m_StatusFlag.Set( STATUS_MAGIC_CASTABLE, state.GetValue( 2 ) );
			m_bIsSummonable = state.GetValue( 3 );
			m_nMaxHP *= 1 + ( state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel() );
			m_nMaxMP *= 1 + ( state.GetValue( 6 ) + state.GetValue( 7 ) * state.GetLevel() );
			break;
		}

	case StructState::EF_PARAMETER_INC_TO_ACTIVE_SUMMON:
		incParameter( state.GetValue( 0 ), state.GetValue( 1 ) + state.GetValue( 2 ) * state.GetLevel(), true, &m_ParameterForSummon );
		incParameter( state.GetValue( 0 ), state.GetValue( 1 ) + state.GetValue( 2 ) * state.GetLevel(), false, &m_ParameterForSummon );
		incParameter( state.GetValue( 3 ), state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel(), true, &m_ParameterForSummon );
		incParameter( state.GetValue( 3 ), state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel(), false, &m_ParameterForSummon );
		incParameter2( state.GetValue( 6 ), state.GetValue( 7 ) + state.GetValue( 8 ) * state.GetLevel(), &m_ParameterForSummon );
		incParameter2( state.GetValue( 9 ), state.GetValue( 10 ) + state.GetValue( 11 ) * state.GetLevel(), &m_ParameterForSummon );
		incParameter( state.GetValue( 12 ), state.GetValue( 13 ) + state.GetValue( 14 ) * state.GetLevel(), true, &m_ParameterForSummon );
		incParameter( state.GetValue( 12 ), state.GetValue( 13 ) + state.GetValue( 14 ) * state.GetLevel(), false, &m_ParameterForSummon );
		incParameter( state.GetValue( 15 ), state.GetValue( 16 ) + state.GetValue( 17 ) * state.GetLevel(), true, &m_ParameterForSummon );
		incParameter( state.GetValue( 15 ), state.GetValue( 16 ) + state.GetValue( 17 ) * state.GetLevel(), false, &m_ParameterForSummon );
		break;

	case StructState::EF_AMP_AND_INC_ITEM_CHANCE:
		m_fActiveSummonExpAmp += state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel();
		StructCreature::applyState( state );
		break;

	case StructState::EF_IMMORTALIZE:
		{
			if( m_nMinHP < state.GetValue( 0 ) )
				m_nMinHP = state.GetValue( 0 );
			if( m_fMinHP < state.GetValue( 1 ) )
				m_fMinHP = state.GetValue( 1 );

			break;
		}
	case StructState::EF_INFINITE_SUMMON_TIME:
		{
			m_bIsInfiniteSummonTime = true;

			if( GetSubSummon() )
			{
				TS_SC_UNSUMMON_NOTICE msg;

				msg.summon_handle = m_pSubSummon->GetHandle();
				msg.unsummon_duration = 0;

				PendMessage( this, &msg );
			}

			break;
		}

	case StructState::EF_MISC:
		{
			switch( state.GetCode() )
			{
			case StructState::STAMINA_SAVE:
				m_bStaminaSave = true;
				break;

			case StructState::SUPER_SAVE_0:
				if( GameRule::anSuperSaveLevelMinLimit[ 0 ] <= GetLevel() && GetLevel() <= GameRule::anSuperSaveLevelMaxLimit[ 0 ] )
					m_bSuperSave = true;
				break;
			case StructState::SUPER_SAVE_1:
				if( GameRule::anSuperSaveLevelMinLimit[ 1 ] <= GetLevel() && GetLevel() <= GameRule::anSuperSaveLevelMaxLimit[ 1 ] )
					m_bSuperSave = true;
				break;
			case StructState::SUPER_SAVE_2:
				if( GameRule::anSuperSaveLevelMinLimit[ 2 ] <= GetLevel() && GetLevel() <= GameRule::anSuperSaveLevelMaxLimit[ 2 ] )
					m_bSuperSave = true;
				break;
			case StructState::SUPER_SAVE_3:
				if( GameRule::anSuperSaveLevelMinLimit[ 3 ] <= GetLevel() && GetLevel() <= GameRule::anSuperSaveLevelMaxLimit[ 3 ] )
					m_bSuperSave = true;
				break;
			case StructState::SUPER_SAVE_4:
				if( GameRule::anSuperSaveLevelMinLimit[ 4 ] <= GetLevel() && GetLevel() <= GameRule::anSuperSaveLevelMaxLimit[ 4 ] )
					m_bSuperSave = true;
				break;
			case StructState::SUPER_SAVE_5:
				if( GameRule::anSuperSaveLevelMinLimit[ 5 ] <= GetLevel() && GetLevel() <= GameRule::anSuperSaveLevelMaxLimit[ 5 ] )
					m_bSuperSave = true;
				break;
			case StructState::SUPER_SAVE_6:
				if( GameRule::anSuperSaveLevelMinLimit[ 6 ] <= GetLevel() && GetLevel() <= GameRule::anSuperSaveLevelMaxLimit[ 6 ] )
					m_bSuperSave = true;
				break;

			case StructState::SUMMON_GROWING_CRACKER_1X:
				m_eSummonStaminaSaveType = SUMMON_STAMINA_SAVE_WITHOUT_PENALTY;
				break;

			case StructState::SUMMON_GROWING_CRACKER_WITH_PENALTY:
				m_eSummonStaminaSaveType = SUMMON_STAMINA_SAVE_WITH_PENALTY;
				break;

			case StructState::PASS_DAMAGE:
				m_fPassDamageRatio = state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel();
				break;

			case StructState::FUSION_WITH_SUMMON:
				m_nMaxHP += m_nMaxHP * ( state.GetValue( 0 ) + state.GetValue( 1 ) * state.GetLevel() );
				m_nMaxMP += m_nMaxMP * ( state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel() );
				m_bIsSummonable = false;
				break;

			default:
				StructCreature::applyState( state );
				break;
			}
		}
		break;

	default:
		StructCreature::applyState( state );
		break;
	}

}

void StructMonster::applyState( StructState & state )
{
	switch( state.GetEffectType() )
	{
	case StructState::EF_PROVOKE:
		m_vHateModifierByState.push_back( _HATE_MODIFIER_TAG( state.GetCaster(), state.GetValue(0) + state.GetLevel() * state.GetValue(1) ) );
		m_bNeedToFindEnemy = true;
		break;

	default:
		StructCreature::applyState( state );
		break;
	}
}

void StructSummon::applyState( StructState & state )
{
	switch( state.GetEffectType() )
	{
	case StructState::EF_HEAL:
		{
			m_nSPAdd += state.GetValue(4) + state.GetLevel() * state.GetValue(5);
		}
		break;
	case StructState::EF_REGEN_ADD:
		{
			m_nSPRegenAdd += state.GetValue(4) + state.GetLevel() * state.GetValue(5);
		}
		break;
	case StructState::EF_HEAL_BY_ITEM:
		{
			m_nSPAdd += state.GetValue(4) + state.GetLevel() * state.GetValue(5);
		}
		break;
	 }

	StructCreature::applyState( state );
}

void StructCreature::applyStateAmplifyEffect()
{
	if( m_vStateList.empty() ) return;

	std::vector< std::pair< int, int > > vDecreaseList;

	STATE_ITERATOR it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		if( (*it).GetEffectType() == StructState::EF_DECREASE_STATE_EFFECT )
		{
			int nDecreaseLevel = (*it).GetValue(0) + (*it).GetValue(1) * (*it).GetLevel();

			for( int i = 2 ; i < 11 && (*it).GetValue(i) ; ++i )
			{
				vDecreaseList.push_back( std::pair< int, int >( (int)(*it).GetValue(i), nDecreaseLevel ) );
			}
		}
	}

	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		int nOriginalLevel = (*it).GetLevel();

		for( std::vector< std::pair< int, int > >::iterator itDecrease = vDecreaseList.begin() ; itDecrease != vDecreaseList.end() ; ++itDecrease )
		{
			if( (*itDecrease).first == (*it).GetCode() )
			{
				int nLevel = nOriginalLevel - (*itDecrease).second;
				if( nLevel < 0 )
				{
					(*itDecrease).second = -1 * nLevel;
					nLevel = 0;
				}

				(*it).SetLevel( nLevel );

				break;
			}
		}

		if( (*it).GetLevel() > 0 )
		{
			applyStateAmplify( *it );
		}

		(*it).SetLevel( nOriginalLevel );
	}
}

void StructCreature::applyStateAmplify( StructState & state )
{
	switch( state.GetEffectType() )
	{
	case StructState::EF_PARAMETER_AMP:
		ampParameter( state.GetValue( 0 ), state.GetValue( 1 ) + state.GetValue( 2 ) * state.GetLevel(), false );
		ampParameter( state.GetValue( 3 ), state.GetValue( 4 ) + state.GetValue( 5 ) * state.GetLevel(), false );
		ampParameter2( state.GetValue( 6 ), state.GetValue( 7 ) + state.GetValue( 8 ) * state.GetLevel() );
		ampParameter2( state.GetValue( 9 ), state.GetValue( 10 ) + state.GetValue( 11 ) * state.GetLevel() );
		ampParameter( state.GetValue( 12 ), state.GetValue( 13 ) + state.GetValue( 14 ) * state.GetLevel(), false );
		ampParameter( state.GetValue( 15 ), state.GetValue( 16 ) + state.GetValue( 17 ) * state.GetLevel(), false );
		break;

	case StructState::EF_MISC:
		{
			switch( state.GetCode() )
			{
			case StructState::SQUALL_OF_ARROW:
				m_StatusFlag.Set( STATUS_MOVABLE, !state.GetValue( 1 ) );

				if( GetWeaponClass() == state.GetValue(0) )
				{
					m_AttributeAmplifier.fAttackSpeedRight += ( state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel() );

					if( IsUsingDoubleWeapon() )
						m_AttributeAmplifier.fAttackSpeedLeft += ( state.GetValue( 2 ) + state.GetValue( 3 ) * state.GetLevel() );
				}
				break;
			}
		}
	}
}

void StructCreature::applyStatByState()
{
	if( m_vStateList.empty() ) return;

	std::vector< std::pair< int, int > > vDecreaseList;

	STATE_ITERATOR it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		if( (*it).GetEffectType() == StructState::EF_DECREASE_STATE_EFFECT )
		{
			int nDecreaseLevel = (*it).GetValue(0) + (*it).GetValue(1) * (*it).GetLevel();

			for( int i = 2 ; i < 11 && (*it).GetValue(i) ; ++i )
			{
				vDecreaseList.push_back( std::pair< int, int >( (int)(*it).GetValue(i), nDecreaseLevel ) );
			}
		}
	}

	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		int nOriginalLevel = (*it).GetLevel();
		
		for( std::vector< std::pair< int, int > >::iterator itDecrease = vDecreaseList.begin() ; itDecrease != vDecreaseList.end() ; ++itDecrease )
		{
			if( (*itDecrease).first == (*it).GetCode() )
			{
				int nLevel = nOriginalLevel - (*itDecrease).second;
				if( nLevel < 0 )
				{
					(*itDecrease).second = -1 * nLevel;
					nLevel = 0;
				}

				(*it).SetLevel( nLevel );

				break;
			}
		}

		if( (*it).GetLevel() > 0 )
		{
			switch( (*it).GetEffectType() )
			{
			case StructState::EF_PARAMETER_INC:
				{
					incParameter( (*it).GetValue( 0 ), (*it).GetValue( 1 ) + (*it).GetValue( 2 ) * (*it).GetLevel(), true );
					incParameter( (*it).GetValue( 3 ), (*it).GetValue( 4 ) + (*it).GetValue( 5 ) * (*it).GetLevel(), true );
					incParameter( (*it).GetValue( 12 ), (*it).GetValue( 13 ) + (*it).GetValue( 14 ) * (*it).GetLevel(), true );
					incParameter( (*it).GetValue( 15 ), (*it).GetValue( 16 ) + (*it).GetValue( 17 ) * (*it).GetLevel(), true );
				}
				break;
			case StructState::EF_PARAMETER_INC_WHEN_EQUIP_SHIELD:
				if( IsWearShield() )
				{
					incParameter( (*it).GetValue( 0 ), (*it).GetValue( 1 ) + (*it).GetValue( 2 ) * (*it).GetLevel(), true );
					incParameter( (*it).GetValue( 3 ), (*it).GetValue( 4 ) + (*it).GetValue( 5 ) * (*it).GetLevel(), true );
				}
				break;
			case StructState::EF_ADD_SKILL:
				{
					addSkill( (*it).GetValue( 0 ), (*it).GetValue( 1 ) + (*it).GetValue( 2 ) * (*it).GetLevel(), (*it).GetValue( 18 ), false );
					addSkill( (*it).GetValue( 3 ), (*it).GetValue( 4 ) + (*it).GetValue( 5 ) * (*it).GetLevel(), (*it).GetValue( 18 ), false );
					addSkill( (*it).GetValue( 6 ), (*it).GetValue( 7 ) + (*it).GetValue( 8 ) * (*it).GetLevel(), (*it).GetValue( 18 ), false );
					addSkill( (*it).GetValue( 9 ), (*it).GetValue( 10 ) + (*it).GetValue( 11 ) * (*it).GetLevel(), (*it).GetValue( 18 ), false );
					addSkill( (*it).GetValue( 12 ), (*it).GetValue( 13 ) + (*it).GetValue( 14 ) * (*it).GetLevel(), (*it).GetValue( 18 ), false );

					addSkill( (*it).GetValue( 15 ), (*it).GetValue( 16 ) + (*it).GetValue( 17 ) * (*it).GetLevel(), (*it).GetValue( 18 ), true );
				}
				break;
			case StructState::EF_PARAMETER_SYNCHRONIZATION:
				{
					synchronizeParameter( (*it).GetValue( 0 ), (*it).GetValue( 1 ) + (*it).GetValue( 2 ) * (*it).GetLevel() );
				}
				break;
			}
		}

		(*it).SetLevel( nOriginalLevel );
	}
}

void StructPlayer::applyStatByState()
{
	if( m_vStateList.empty() ) return;

	StructCreature::applyStatByState();

	std::vector< std::pair< int, int > > vDecreaseList;

	STATE_ITERATOR it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		int nOriginalLevel = (*it).GetLevel();

		for( std::vector< std::pair< int, int > >::iterator itDecrease = vDecreaseList.begin() ; itDecrease != vDecreaseList.end() ; ++itDecrease )
		{
			if( (*itDecrease).first == (*it).GetCode() )
			{
				int nLevel = nOriginalLevel - (*itDecrease).second;
				if( nLevel < 0 )
				{
					(*itDecrease).second = -1 * nLevel;
					nLevel = 0;
				}

				(*it).SetLevel( nLevel );

				break;
			}
		}

		if( (*it).GetLevel() > 0 )
		{
			switch( (*it).GetEffectType() )
			{
			case StructState::EF_CREATURE_PARAMETER_AMP:
				{
					m_nDoubleSummonTime += (*it).GetLevel() * (*it).GetValue(4) * 100;
					break;
				}
			}
		}
		(*it).SetLevel( nOriginalLevel );
	}
}

void StructSummon::applyStatByState()
{
	StructCreature::applyStatByState();

	// 이하는 주인에게 부여된 소환수의 스텟에 영향 주는 지속효과 처리임
	if( !GetMaster() )
		return;
}

void StructCreature::amplifyStatByState()
{
	if( m_vStateList.empty() ) return;

	std::vector< std::pair< int, int > > vDecreaseList;

	STATE_ITERATOR it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		if( (*it).GetEffectType() == StructState::EF_DECREASE_STATE_EFFECT )
		{
			int nDecreaseLevel = (*it).GetValue(0) + (*it).GetValue(1) * (*it).GetLevel();

			for( int i = 2 ; i < 11 && (*it).GetValue(i) ; ++i )
			{
				vDecreaseList.push_back( std::pair< int, int >( (int)(*it).GetValue(i), nDecreaseLevel ) );
			}
		}
	}

	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		int nOriginalLevel = (*it).GetLevel();

		for( std::vector< std::pair< int, int > >::iterator itDecrease = vDecreaseList.begin() ; itDecrease != vDecreaseList.end() ; ++itDecrease )
		{
			if( (*itDecrease).first == (*it).GetCode() )
			{
				int nLevel = nOriginalLevel - (*itDecrease).second;
				if( nLevel < 0 )
				{
					(*itDecrease).second = -1 * nLevel;
					nLevel = 0;
				}
				
				(*it).SetLevel( nLevel );

				break;
			}
		}

		if( (*it).GetLevel() > 0 )
		{
			switch( (*it).GetEffectType() )
			{
			case StructState::EF_PARAMETER_AMP:
				{
					ampParameter( (*it).GetValue( 0 ), (*it).GetValue( 1 ) + (*it).GetValue( 2 ) * (*it).GetLevel(), true );
					ampParameter( (*it).GetValue( 3 ), (*it).GetValue( 4 ) + (*it).GetValue( 5 ) * (*it).GetLevel(), true );
					ampParameter( (*it).GetValue( 12 ), (*it).GetValue( 13 ) + (*it).GetValue( 14 ) * (*it).GetLevel(), true );
					ampParameter( (*it).GetValue( 15 ), (*it).GetValue( 16 ) + (*it).GetValue( 17 ) * (*it).GetLevel(), true );
				}
				break;
			}
		}

		(*it).SetLevel( nOriginalLevel );
	}
}

void StructCreature::applyParamBasedStateEffect()
{
	// 지속 효과 하나가 파라메터에 기반하여 성능이 바뀌기 때문에 어쩔 수 없이 분리
	// TODO: 차후 스탯 적용 시점과 아이템 효과 적용 시점을 분리할 때 이 함수도 합쳐 볼 방안을 찾아본다
	if( m_vStateList.empty() ) return;

	std::vector< std::pair< int, int > > vDecreaseList;

	STATE_ITERATOR it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		if( it->GetEffectType() == StructState::EF_DECREASE_STATE_EFFECT )
		{
			int nDecreaseLevel = it->GetValue(0) + it->GetValue(1) * it->GetLevel();

			for( int i = 2 ; i < 11 && it->GetValue(i) ; ++i )
			{
				vDecreaseList.push_back( std::pair< int, int >( (int)(*it).GetValue(i), nDecreaseLevel ) );
			}
		}
	}

	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		int nOriginalLevel = it->GetLevel();

		for( std::vector< std::pair< int, int > >::iterator itDecrease = vDecreaseList.begin() ; itDecrease != vDecreaseList.end() ; ++itDecrease )
		{
			if( itDecrease->first == it->GetCode() )
			{
				int nLevel = nOriginalLevel - itDecrease->second;
				if( nLevel < 0 )
				{
					itDecrease->second = -1 * nLevel;
					nLevel = 0;
				}

				it->SetLevel( nLevel );

				break;
			}
		}

		if( it->GetLevel() > 0 )
		{
			switch( it->GetEffectType() )
			{
			case StructState::EF_INC_DOT_BASED_PARAM:
				{
					for ( int i = 0; i < 2; i++ )
					{
						int varIndex = i * 6;
						int sumIndex = i * 2 + 12;

						c_fixed10 sum = CalcStatSum( it->GetValue(sumIndex), m_StatBeforeParamBasedCalc )
							+ CalcAttributeSum( it->GetValue(sumIndex), it->GetValue(sumIndex+1), m_AttrBeforeParamBasedCalc );
						int type = it->GetValue(varIndex);
						c_fixed10 adder = it->GetValue(varIndex+1) + it->GetValue(varIndex+2) * it->GetLevel();
						c_fixed10 amplifier = it->GetValue(varIndex+3) + it->GetValue(varIndex+4) * it->GetLevel();

						if ( type == 99 )
						{
							for ( int j = 0; j < Elemental::TYPE_COUNT; j++ )
							{
								m_ElementalStateDamageAdder[j] += sum * adder;
								m_ElementalStateDamageAmplifier[j] += sum * amplifier;
							}
						}
						else
						{
							m_ElementalStateDamageAdder[type] += sum * adder;
							m_ElementalStateDamageAmplifier[type] += sum * amplifier;				
						}
					}

					break;
				}
			}
		}

		it->SetLevel( nOriginalLevel );
	}
}


// --- 스탯 및 능력치 연산에 관련된 아이템 효과를 적용하는 루틴 ---
void StructCreature::applyItemEffect()
{
	int i, x;

	// { 유니트 익스퍼트
	StructSkill *pUnitExpert = NULL;
	if( ( pUnitExpert = GetCurrentPassiveSkill( StructSkill::SKILL_UNIT_EXPERT_LV6 ) ) ) ;
	else if( ( pUnitExpert	= GetCurrentPassiveSkill( StructSkill::SKILL_UNIT_EXPERT_LV5 ) ) ) ;
	else if( ( pUnitExpert	= GetCurrentPassiveSkill( StructSkill::SKILL_UNIT_EXPERT_LV4 ) ) ) ;
	else if( ( pUnitExpert	= GetCurrentPassiveSkill( StructSkill::SKILL_UNIT_EXPERT_LV3 ) ) ) ;
	else pUnitExpert = GetCurrentPassiveSkill( StructSkill::SKILL_UNIT_EXPERT_LV2 );

	m_nUnitExpertLevel = 0;
	if( pUnitExpert ) m_nUnitExpertLevel = GameRule::GetItemLevelLimitByRank( pUnitExpert->GetVar( 0 ) );
	// }

	for( i = 0; i < ItemBase::MAX_ITEM_WEAR; ++i )
	{
		if( !m_anWear[i] ) continue;

		// 장착 불가는 효과 없음
		if( !IsWearable( (ItemBase::ItemWearType)i, m_anWear[i] ) ) continue;

		float fItemRatio = 1.0f;

		// 유니트 익스퍼트로 입은 아이템.
		if( m_anWear[i]->GetLevelLimit() > GetLevel() && m_anWear[i]->GetLevelLimit() <= m_nUnitExpertLevel ) fItemRatio = 0.4f;

		const ItemBaseServer & base = m_anWear[i]->GetItemBase();
		for( x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
		{
			if( !base.nBaseType[x] ) continue;			

			onItemWearEffect( m_anWear[i], true, base.nBaseType[x], base.fBaseVar1[x], base.fBaseVar2[x], fItemRatio );
		}

		for( x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
		{
			if( !base.nOptType[x] ) continue;

			onItemWearEffect( m_anWear[i], false, base.nOptType[x], base.fOptVar1[x], base.fOptVar2[x], fItemRatio );
		}

		int nEnhance = m_anWear[i]->GetItemEnhance();
		if( nEnhance >= 1 )
		{
			const std::vector< EnhanceEffectInfo * > * pvEffectList = base.pvEnhanceEffectList;
			if( pvEffectList )
			{
				for( auto it = pvEffectList->begin() ; it != pvEffectList->end() ; ++it )
				{
					c_fixed10 fAddPoint = 0;
					for( int iEnhance = 0 ; iEnhance < nEnhance ; ++iEnhance )
					{
						fAddPoint += (*it)->fValue[iEnhance];
					}

					onItemWearEffect( m_anWear[i], false, (*it)->nEnhanceEffectID, fAddPoint, 0, fItemRatio );
				}
			}
		}

		_applyEffect( m_anWear[ i ], base.pvEffectList, fItemRatio );

		int nSocketCount = m_anWear[i]->GetMaxSocketCount();
		for( x = 0 ; x < nSocketCount ; ++x )
		{
			ItemBase::ItemCode nSocketCode = m_anWear[i]->GetSocketCode( x );

			if( !nSocketCode )
				continue;

			const ItemBaseServer & SocketBase = StructItem::GetItemBase( nSocketCode );

			// 레벨 제한 적용 가능 검사
			if( GetLevel() < SocketBase.nMinLevel || GetLevel() > SocketBase.nMaxLevel )
				continue;

			// 소켓 내구도 검사
			if( !m_anWear[i]->GetCurrentEndurance() )
				continue;

			for( int k = 0; k < ItemBase::MAX_OPTION_NUMBER; ++k )
			{
				if( !SocketBase.nBaseType[k] ) continue;

				onItemWearEffect( m_anWear[i], true, SocketBase.nBaseType[k], SocketBase.fBaseVar1[k], SocketBase.fBaseVar2[k], fItemRatio );
			}

			for( int k = 0; k < ItemBase::MAX_OPTION_NUMBER; ++k )
			{
				if( !SocketBase.nOptType[k] ) continue;

				onItemWearEffect( m_anWear[i], false, SocketBase.nOptType[k], SocketBase.fOptVar1[k], SocketBase.fOptVar2[k], fItemRatio );
			}

			_applyEffect( m_anWear[ i ], SocketBase.pvEffectList, fItemRatio );
		}

		// 각성 여부 확인
		if( m_anWear[i]->IsAwaken() )
		{
			for( int nCnt = 0 ; nCnt < ItemInstance::MAX_AWAKEN_NUMBER ; ++nCnt )
			{
				if( !m_anWear[i]->GetAwakenOptionType( nCnt ) )
					break;

				if( m_anWear[i]->GetAwakenOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::APPLY_EFFECT )
				{
					_applyEffect( m_anWear[ i ], GameContent::GetEffectInfoVector( m_anWear[i]->GetAwakenOptionValue1( nCnt ) ), fItemRatio );
				}
				else
				{
					onItemWearEffect( m_anWear[i], false, m_anWear[i]->GetAwakenOptionType( nCnt ), m_anWear[i]->GetAwakenOptionValue1( nCnt ), m_anWear[i]->GetAwakenOptionValue2( nCnt ), fItemRatio );
				}
			}
		}

		// 랜덤화 여부 확인
		if( m_anWear[i]->IsIdentified() )
		{
			for( int nCnt = 0 ; nCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++nCnt )
			{
				if( !m_anWear[i]->GetIdentifiedOptionType( nCnt ) )
					break;

				if( m_anWear[i]->GetIdentifiedOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::APPLY_EFFECT )
				{
					_applyEffect( m_anWear[ i ], GameContent::GetEffectInfoVector( m_anWear[i]->GetIdentifiedOptionValue1( nCnt ) ), fItemRatio );
				}
				else
				{
					onItemWearEffect( m_anWear[i], false, m_anWear[i]->GetIdentifiedOptionType( nCnt ), m_anWear[i]->GetIdentifiedOptionValue1( nCnt ), m_anWear[i]->GetIdentifiedOptionValue2( nCnt ), fItemRatio );
				}
			}
		}
	}

	for( std::vector< SetItemEffectInfo * >::const_iterator itSetItemEffectInfo = m_vSetItemEffect.begin() ; itSetItemEffectInfo != m_vSetItemEffect.end() ; ++itSetItemEffectInfo )
	{
		for( int nOptionIndex = 0 ; nOptionIndex < ItemBase::MAX_OPTION_NUMBER ; ++nOptionIndex )
		{
			if( !(*itSetItemEffectInfo)->nBaseType[nOptionIndex] ) continue;

			onItemWearEffect( NULL, true, (*itSetItemEffectInfo)->nBaseType[nOptionIndex], (*itSetItemEffectInfo)->fBaseVar1[nOptionIndex], (*itSetItemEffectInfo)->fBaseVar2[nOptionIndex], 1.0f );
		}

		for( int nOptionIndex = 0 ; nOptionIndex < ItemBase::MAX_OPTION_NUMBER ; ++nOptionIndex )
		{
			if( !(*itSetItemEffectInfo)->nOptType[nOptionIndex] ) continue;

			onItemWearEffect( NULL, false, (*itSetItemEffectInfo)->nOptType[nOptionIndex], (*itSetItemEffectInfo)->fOptVar1[nOptionIndex], (*itSetItemEffectInfo)->fOptVar2[nOptionIndex], 1.0f );
		}

		_applyEffect( NULL, (*itSetItemEffectInfo)->pvEffectList, 1.0f );
	}
}

void StructCreature::_applyEffect( struct StructItem * pItem, const std::vector< EffectInfo * > * pvEffectList, float fRatio )
{
	if( !pvEffectList || pvEffectList->empty() )
		return;

	int nEnhance = ( pItem ) ? pItem->GetItemEnhance() : 0;

	for( std::vector< EffectInfo * >::const_iterator it = pvEffectList->begin() ; it != pvEffectList->end() ; ++it )
	{
		EffectInfo * pEffect = (*it);
		if( pEffect->nMinLevel && GetLevel() < pEffect->nMinLevel ) continue;
		if( pEffect->nMaxLevel && GetLevel() > pEffect->nMaxLevel ) continue;

		switch( pEffect->eType )
		{
		case EffectInfo::EFFECT_TYPE_BASIC:
			if( pEffect->fValue[ 0 ] )	onItemWearEffect( pItem, true, pEffect->fValue[ 0 ], pEffect->fValue[ 1 ], pEffect->fValue[ 2 ], fRatio );
			if( pEffect->fValue[ 3 ] )	onItemWearEffect( pItem, true, pEffect->fValue[ 3 ], pEffect->fValue[ 4 ], pEffect->fValue[ 5 ], fRatio );
			if( pEffect->fValue[ 6 ] )	onItemWearEffect( pItem, true, pEffect->fValue[ 6 ], pEffect->fValue[ 7 ], pEffect->fValue[ 8 ], fRatio );
			if( pEffect->fValue[ 9 ] )	onItemWearEffect( pItem, true, pEffect->fValue[ 9 ], pEffect->fValue[ 10 ], pEffect->fValue[ 11 ], fRatio );
			break;

		case EffectInfo::EFFECT_TYPE_OPTIONAL:
			if( pEffect->fValue[ 0 ] )	onItemWearEffect( pItem, false, pEffect->fValue[ 0 ], pEffect->fValue[ 1 ], pEffect->fValue[ 2 ], fRatio );
			if( pEffect->fValue[ 3 ] )	onItemWearEffect( pItem, false, pEffect->fValue[ 3 ], pEffect->fValue[ 4 ], pEffect->fValue[ 5 ], fRatio );
			if( pEffect->fValue[ 6 ] )	onItemWearEffect( pItem, false, pEffect->fValue[ 6 ], pEffect->fValue[ 7 ], pEffect->fValue[ 8 ], fRatio );
			if( pEffect->fValue[ 9 ] )	onItemWearEffect( pItem, false, pEffect->fValue[ 9 ], pEffect->fValue[ 10 ], pEffect->fValue[ 11 ], fRatio );
			break;

		case EffectInfo::EFFECT_TYPE_STATE:
			_applyEffectForState( pEffect );
			break;

		case EffectInfo::EFFECT_TYPE_ENHANCE:
			if( nEnhance >= 1 )
			{
				for( int nIndex = 0 ; nIndex < 2 ; ++nIndex )
				{
					if( !pEffect->fValue[ nIndex * 5 ] )
						continue;

					float fAddPoint = 0.0f;

					if( nEnhance > 0 )		fAddPoint += std::min( nEnhance, 4 ) * pEffect->fValue[ nIndex + 1 ];
					if( nEnhance > 4 )		fAddPoint += std::min( nEnhance - 4, 4 ) * pEffect->fValue[ nIndex + 2 ];
					if( nEnhance > 8 )		fAddPoint += std::min( nEnhance - 8, 4 ) * pEffect->fValue[ nIndex + 3 ];
					if( nEnhance > 12 )		fAddPoint += std::min( nEnhance - 12, 8 ) * pEffect->fValue[ nIndex + 4 ];

					onItemWearEffect( pItem, false, pEffect->fValue[ nIndex ], (int)fAddPoint, 0, fRatio );
				}
			}
			break;

		default:
			assert( 0 );
			break;
		}
	}
}

void StructCreature::_applyEffectForState( const EffectInfo * pEffect )
{
	// applyState 함수와 대부분 내용 중첩되므로 수정 사항 있으면 같이 수정할 것
	// 파라미터 증가/증폭 모두 한 번에 적용함(증폭은 어차피 모아서 마지막에 적용하므로 여기서 해도 별 상관 없으며, 애초에 아이템 적용으로 인한 증가/증폭은 한 번에 처리 됨)
	switch( pEffect->nEffectID )
	{
	case StructState::EF_PARAMETER_INC:
		incParameter( pEffect->fValue[ 0 ], pEffect->fValue[ 1 ] + pEffect->fValue[ 2 ] * pEffect->nEffectLevel, false );
		incParameter( pEffect->fValue[ 3 ], pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel, false );
		incParameter2( pEffect->fValue[ 6 ], pEffect->fValue[ 7 ] + pEffect->fValue[ 8 ] * pEffect->nEffectLevel );
		incParameter2( pEffect->fValue[ 9 ], pEffect->fValue[ 10 ] + pEffect->fValue[ 11 ] * pEffect->nEffectLevel );
		incParameter( pEffect->fValue[ 12 ], pEffect->fValue[ 13 ] + pEffect->fValue[ 14 ] * pEffect->nEffectLevel, false );
		incParameter( pEffect->fValue[ 15 ], pEffect->fValue[ 16 ] + pEffect->fValue[ 17 ] * pEffect->nEffectLevel, false );

		break;
	case StructState::EF_PARAMETER_INC_WHEN_EQUIP_SHIELD:
		if( IsWearShield() )
		{
			incParameter( pEffect->fValue[ 0 ], pEffect->fValue[ 1 ] + pEffect->fValue[ 2 ] * pEffect->nEffectLevel, false );
			incParameter( pEffect->fValue[ 3 ], pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel, false );
			incParameter2( pEffect->fValue[ 6 ], pEffect->fValue[ 7 ] + pEffect->fValue[ 8 ] * pEffect->nEffectLevel );
			incParameter2( pEffect->fValue[ 9 ], pEffect->fValue[ 10 ] + pEffect->fValue[ 11 ] * pEffect->nEffectLevel );
		}

		break;

	case StructState::EF_PARAMETER_AMP_WHEN_EQUIP_SHIELD:
		if( IsWearShield() )
		{
			ampParameter( pEffect->fValue[ 0 ], pEffect->fValue[ 1 ] + pEffect->fValue[ 2 ] * pEffect->nEffectLevel, false );
			ampParameter( pEffect->fValue[ 3 ], pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel, false );
			ampParameter2( pEffect->fValue[ 6 ], pEffect->fValue[ 7 ] + pEffect->fValue[ 8 ] * pEffect->nEffectLevel );
			ampParameter2( pEffect->fValue[ 9 ], pEffect->fValue[ 10 ] + pEffect->fValue[ 11 ] * pEffect->nEffectLevel );
		}

		break;
	case StructState::EF_PARAMETER_INC_WHEN_EQUIP:
	case StructState::EF_PARAMETER_AMP_WHEN_EQUIP:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();

			DWORD nWeaponBitFlag = 0;

			if( !weapon_class )
			{
				break;
			}
			switch( weapon_class )
			{
			case ItemBase::CLASS_ONEHAND_SWORD:		nWeaponBitFlag = StructState::FLAG_EQUIP_ONEHAND_SWORD;				break;
			case ItemBase::CLASS_TWOHAND_SWORD:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_SWORD;				break;
			case ItemBase::CLASS_DAGGER:			nWeaponBitFlag = StructState::FLAG_EQUIP_DAGGER;					break;
			case ItemBase::CLASS_TWOHAND_SPEAR:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_SPEAR;				break;
			case ItemBase::CLASS_TWOHAND_AXE:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_AXE;				break;
			case ItemBase::CLASS_ONEHAND_MACE:		nWeaponBitFlag = StructState::FLAG_EQUIP_ONEHAND_MACE;				break;
			case ItemBase::CLASS_TWOHAND_MACE:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_MACE;				break;
			case ItemBase::CLASS_HEAVY_BOW:			nWeaponBitFlag = StructState::FLAG_EQUIP_HEAVY_BOW;					break;
			case ItemBase::CLASS_LIGHT_BOW:			nWeaponBitFlag = StructState::FLAG_EQUIP_LIGHT_BOW;					break;
			case ItemBase::CLASS_CROSSBOW:			nWeaponBitFlag = StructState::FLAG_EQUIP_CROSSBOW;					break;
			case ItemBase::CLASS_ONEHAND_STAFF:		nWeaponBitFlag = StructState::FLAG_EQUIP_ONEHAND_STAFF;				break;
			case ItemBase::CLASS_TWOHAND_STAFF:		nWeaponBitFlag = StructState::FLAG_EQUIP_TWOHAND_STAFF;				break;
			case ItemBase::CLASS_DOUBLE_SWORD:		nWeaponBitFlag = StructState::FLAG_EQUIP_DOUBLE_SWORD;				break;
			case ItemBase::CLASS_DOUBLE_DAGGER:		nWeaponBitFlag = StructState::FLAG_EQUIP_DOUBLE_DAGGER;				break;
			}

			if( !( ((DWORD)pEffect->fValue[0]) & nWeaponBitFlag ) )
				break;

			if( pEffect->nEffectID == StructState::EF_PARAMETER_INC_WHEN_EQUIP )
			{
				incParameter( pEffect->fValue[ 1 ], pEffect->fValue[ 2 ] + pEffect->nEffectLevel * pEffect->fValue[ 3 ], ( pEffect->fValue[ 1 ] <= StructState::FLAG_LUK ) ? true : false );
				incParameter( pEffect->fValue[ 4 ], pEffect->fValue[ 5 ] + pEffect->nEffectLevel * pEffect->fValue[ 6 ], ( pEffect->fValue[ 4 ] <= StructState::FLAG_LUK ) ? true : false );
				incParameter2( pEffect->fValue[ 7 ], pEffect->fValue[ 8 ] + pEffect->nEffectLevel * pEffect->fValue[ 9 ] );
			}
			else if( pEffect->nEffectID == StructState::EF_PARAMETER_AMP_WHEN_EQUIP )
			{
				ampParameter( pEffect->fValue[ 1 ], pEffect->fValue[ 2 ] + pEffect->nEffectLevel * pEffect->fValue[ 3 ], ( pEffect->fValue[ 1 ] <= StructState::FLAG_LUK ) ? true : false );
				ampParameter( pEffect->fValue[ 4 ], pEffect->fValue[ 5 ] + pEffect->nEffectLevel * pEffect->fValue[ 6 ], ( pEffect->fValue[ 4 ] <= StructState::FLAG_LUK ) ? true : false );
				ampParameter2( pEffect->fValue[ 7 ], pEffect->fValue[ 8 ] + pEffect->nEffectLevel * pEffect->fValue[ 9 ] );
			}
		}
		break;

	case StructState::EF_DOUBLE_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();

			if( !weapon_class )
			{
				break;
			}
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON )
			{
				if( weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
					break;
			}

			m_Attribute.fDoubleAttackRatio += pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1];
		}
		break;

	case StructState::EF_ADDITIONAL_DAMAGE_ON_ATTACK:
		{
			if( pEffect->fValue[ 11 ] == 0 || pEffect->fValue[ 11 ] == 99 )
				m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, Elemental::TYPE_NONE, (Elemental::Type) (int) pEffect->fValue[ 8 ], pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel, 0 ) );
			if( pEffect->fValue[ 11 ] == 1 || pEffect->fValue[ 11 ] == 99 )
				m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, Elemental::TYPE_NONE, (Elemental::Type) (int) pEffect->fValue[ 8 ], pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel, 0 ) );
		}
		break;

	case StructState::EF_AMP_ADDITIONAL_DAMAGE_ON_ATTACK:
		{
			if( pEffect->fValue[ 11 ] == 0 || pEffect->fValue[ 11 ] == 99 )
				m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, Elemental::TYPE_NONE, (Elemental::Type) (int) pEffect->fValue[ 8 ], 0, pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel ) );
			if( pEffect->fValue[ 11 ] == 1 || pEffect->fValue[ 11 ] == 99 )
				m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, Elemental::TYPE_NONE, (Elemental::Type) (int) pEffect->fValue[ 8 ], 0, pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel ) );
		}
		break;

	case StructState::EF_ADDITIONAL_DAMAGE_ON_SKILL:
		{
			if( pEffect->fValue[ 11 ] == 0 || pEffect->fValue[ 11 ] == 99 )
				m_vPhysicalSkillAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, (Elemental::Type) (int) pEffect->fValue[ 5 ], (Elemental::Type) (int) pEffect->fValue[ 8 ], pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel, 0 ) );
			if( pEffect->fValue[ 11 ] == 1 || pEffect->fValue[ 11 ] == 99 )
				m_vMagicalSkillAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, (Elemental::Type) (int) pEffect->fValue[ 5 ], (Elemental::Type) (int) pEffect->fValue[ 8 ], pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel, 0 ) );
		}
		break;

	case StructState::EF_AMP_ADDTIONAL_DAMAGE_ON_SKILL:
		{
			if( pEffect->fValue[ 11 ] == 0 || pEffect->fValue[ 11 ] == 99 )
				m_vPhysicalSkillAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, (Elemental::Type) (int) pEffect->fValue[ 5 ], (Elemental::Type) (int) pEffect->fValue[ 8 ], 0, pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel ) );
			if( pEffect->fValue[ 11 ] == 1 || pEffect->fValue[ 11 ] == 99 )
				m_vMagicalSkillAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, (Elemental::Type) (int) pEffect->fValue[ 5 ], (Elemental::Type) (int) pEffect->fValue[ 8 ], 0, pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel ) );
		}
		break;

	case StructState::EF_ADD_STATE_ON_ATTACK_OLD:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, 0, 0, 0, 0, StructState::NormalAttack, 99, NULL ) );
			m_vProcByAttack.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, StructProc::ATTACKEE, 0 );
		}
		break;

	// 코드량이 지나치게 많지만 가독성 때문에 우선 펼쳐둔다.
	case StructState::EF_ADD_STATE_ON_ATTACK:
	case StructState::EF_ADD_STATE_BY_SELF_ON_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pEffect->nEffectID )
			{
			case StructState::EF_ADD_STATE_ON_ATTACK:			nTarget = StructProc::ATTACKEE;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_ATTACK:	nTarget = StructProc::ATTACKER;	break;
			}

			m_vProcByAttack.push_back( _ATTACK_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, pEffect->fValue[ 14 ], pEffect->fValue[ 15 ], pEffect->fValue[ 16 ], pEffect->fValue[ 17 ], pEffect->fValue[ 12 ], pEffect->fValue[ 18 ], NULL ) );
			m_vProcByAttack.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, nTarget, pEffect->fValue[ 13 ] );
		}
		break;

	case StructState::EF_ADD_STATE_ON_BEING_ATTACKED:
	case StructState::EF_ADD_STATE_BY_SELF_ON_BEING_ATTACKED:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pEffect->nEffectID )
			{
			case StructState::EF_ADD_STATE_ON_BEING_ATTACKED:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_BEING_ATTACKED:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByBeingAttacked.push_back( _ATTACK_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, pEffect->fValue[ 14 ], pEffect->fValue[ 15 ], pEffect->fValue[ 16 ], pEffect->fValue[ 17 ], pEffect->fValue[ 12 ], pEffect->fValue[ 18 ], NULL ) );
			m_vProcByBeingAttacked.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, nTarget, pEffect->fValue[ 13 ] );
		}
		break;

	case StructState::EF_AMP_DAMAGE_BASED_ATTACK_POINT:
		{
			c_fixed10 damage = 0;

			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 14 ] != ItemBase::CLASS_EVERY_WEAPON &&
			    weapon_class != pEffect->fValue[ 14 ] && 
			    weapon_class != pEffect->fValue[ 15 ] &&
			    weapon_class != pEffect->fValue[ 16 ] &&
			    weapon_class != pEffect->fValue[ 17 ] )
				break;

			if ( pEffect->fValue[ 4 ] == 0 || pEffect->fValue[ 4 ] == 99 )
			{
				damage += GetAttackPointRight();
			}
			if ( pEffect->fValue[ 4 ] == 1 || pEffect->fValue[ 4 ] == 99 )
			{
				damage += GetMagicPoint();
			}

			damage *= pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel;

			c_fixed10 procRate = pEffect->fValue[ 5 ] + pEffect->fValue[ 6 ] * pEffect->nEffectLevel;

			AdditionalDamageInfo additionalDamage( procRate, (Elemental::Type)99, (Elemental::Type)(int)pEffect->fValue[ 7 ], damage, 0 );

			if ( pEffect->fValue[ 10 ] == 1 )
			{
				m_vNormalAdditionalDamage.push_back( additionalDamage );
			}
			if ( pEffect->fValue[ 11 ] == 1 )
			{
				m_vRangeAdditionalDamage.push_back( additionalDamage );
			}
			if ( pEffect->fValue[ 12 ] == 1 )
			{
				m_vPhysicalSkillAdditionalDamage.push_back( additionalDamage );
			}
			if ( pEffect->fValue[ 13 ] == 1 )
			{
				m_vMagicalSkillAdditionalDamage.push_back( additionalDamage );
			}
		}
		break;

	case StructState::EF_ADD_STATE_BY_SELF_ON_KILL:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			m_vProcByKill.push_back( _KILL_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, pEffect->fValue[ 14 ], pEffect->fValue[ 15 ], 0, 0, pEffect->fValue[ 18 ], pEffect->fValue[ 19 ], NULL ) );
			m_vProcByKill.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, StructProc::ATTACKER, pEffect->fValue[ 13 ] );
		}
		break;

	case StructState::EF_ADD_STATE_BY_SELF_ON_DEAD:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			m_vProcByDead.push_back( _KILL_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, 0, 0, pEffect->fValue[ 16 ], pEffect->fValue[ 17 ], pEffect->fValue[ 18 ], pEffect->fValue[ 19 ], NULL ) );
			m_vProcByDead.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, StructProc::ATTACKEE, pEffect->fValue[ 13 ] );
		}
		break;

	case StructState::EF_ADD_STATE_ON_CRITICAL_ATTACK:
	case StructState::EF_ADD_STATE_BY_SELF_ON_CRITICAL_ATTACK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pEffect->nEffectID )
			{
			case StructState::EF_ADD_STATE_ON_CRITICAL_ATTACK:			nTarget = StructProc::ATTACKEE;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_CRITICAL_ATTACK:	nTarget = StructProc::ATTACKER;	break;
			}

			m_vProcByCriticalAttack.push_back( _ATTACK_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, pEffect->fValue[ 14 ], pEffect->fValue[ 15 ], pEffect->fValue[ 16 ], pEffect->fValue[ 17 ], pEffect->fValue[ 12 ], pEffect->fValue[ 18 ], NULL ) );
			m_vProcByCriticalAttack.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, nTarget, pEffect->fValue[ 13 ] );
		}
		break;

	case StructState::EF_ADD_STATE_ON_BEING_CRITICAL_ATTACKED:
	case StructState::EF_ADD_STATE_BY_SELF_ON_BEING_CRITICAL_ATTACKED:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pEffect->nEffectID )
			{
			case StructState::EF_ADD_STATE_ON_BEING_CRITICAL_ATTACKED:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_BEING_CRITICAL_ATTACKED:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByBeingCriticalAttacked.push_back( _ATTACK_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, pEffect->fValue[ 14 ], pEffect->fValue[ 15 ], pEffect->fValue[ 16 ], pEffect->fValue[ 17 ], pEffect->fValue[ 12 ], pEffect->fValue[ 18 ], NULL ) );
			m_vProcByBeingCriticalAttacked.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, nTarget, pEffect->fValue[ 13 ] );

		}
		break;

	case StructState::EF_ADD_STATE_ON_AVOID:
	case StructState::EF_ADD_STATE_BY_SELF_ON_AVOID:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pEffect->nEffectID )
			{
			case StructState::EF_ADD_STATE_ON_AVOID:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_AVOID:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByAvoid.push_back( _ATTACK_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, pEffect->fValue[ 14 ], pEffect->fValue[ 15 ], pEffect->fValue[ 16 ], pEffect->fValue[ 17 ], pEffect->fValue[ 12 ], pEffect->fValue[ 18 ], NULL ) );
			m_vProcByAvoid.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, nTarget, pEffect->fValue[ 13 ] );

		}
		break;

	case StructState::EF_ADD_STATE_ON_BLOCK:
	case StructState::EF_ADD_STATE_BY_SELF_ON_BLOCK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pEffect->nEffectID )
			{
			case StructState::EF_ADD_STATE_ON_BLOCK:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_BLOCK:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByBlock.push_back( _ATTACK_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, pEffect->fValue[ 14 ], pEffect->fValue[ 15 ], pEffect->fValue[ 16 ], pEffect->fValue[ 17 ], pEffect->fValue[ 12 ], pEffect->fValue[ 18 ], NULL ) );
			m_vProcByBlock.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, nTarget, pEffect->fValue[ 13 ] );

		}
		break;

	case StructState::EF_ADD_STATE_ON_PERFECT_BLOCK:
	case StructState::EF_ADD_STATE_BY_SELF_ON_PERFECT_BLOCK:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON && weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;

			StructProc::TargetType nTarget = StructProc::ATTACKER;

			switch( pEffect->nEffectID )
			{
			case StructState::EF_ADD_STATE_ON_PERFECT_BLOCK:			nTarget = StructProc::ATTACKER;	break;
			case StructState::EF_ADD_STATE_BY_SELF_ON_PERFECT_BLOCK:	nTarget = StructProc::ATTACKEE;	break;
			}

			m_vProcByPerfectBlock.push_back( _ATTACK_TAG( pEffect->fValue[ 6 ] + pEffect->fValue[ 7 ] * pEffect->nEffectLevel, pEffect->fValue[ 14 ], pEffect->fValue[ 15 ], pEffect->fValue[ 16 ], pEffect->fValue[ 17 ], pEffect->fValue[ 12 ], pEffect->fValue[ 18 ], NULL ) );
			m_vProcByPerfectBlock.back().proc = new StructStateProc( static_cast< StructState::StateCode >( (int) pEffect->fValue[ 0 ] ), pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel, ( pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel ) * 100, nTarget, pEffect->fValue[ 13 ] );

		}
		break;

	case StructState::EF_ADD_HP_ON_ATTACK:
		{

			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON )
			{
			ItemBase::ItemClass weapon_class = GetWeaponClass();

				if( !weapon_class )
				{
					break;
				}

				if( weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
				break;
			}

			int nHPInc = pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1];
			int nMPInc = pEffect->fValue[2] + pEffect->nEffectLevel * pEffect->fValue[3];
			int nRatio = pEffect->fValue[6] + pEffect->nEffectLevel * pEffect->fValue[7];

			if( !nHPInc && !nMPInc )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( nRatio, 0, 0, 0, 0, StructState::NormalAttack, 99, NULL ) );
			m_vProcByAttack.back().proc = new StructHealProc( StructProc::ATTACKER, nHPInc, nMPInc );
		}
		break;

	case StructState::EF_ABSORB:
		{
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON )
			{
				ItemBase::ItemClass weapon_class = GetWeaponClass();

				if( !weapon_class )
				{
					break;
				}

				if( weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
					break;
			}

			float fHPAbsorbRatio = pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1];
			float fMPAbsorbRatio = pEffect->fValue[2] + pEffect->nEffectLevel * pEffect->fValue[3];
			int nRatio = pEffect->fValue[6] + pEffect->nEffectLevel * pEffect->fValue[7];

			if( !fHPAbsorbRatio && !fMPAbsorbRatio )
				break;

			StructAbsorbProc *pProc = new StructAbsorbProc( StructProc::ATTACKER, fHPAbsorbRatio, fMPAbsorbRatio );
			m_vProcByAttack.push_back( _ATTACK_TAG( nRatio, 0, 0, 0, 0, StructState::NormalAttack, 99, pProc ) );
		}
		break;

	case StructState::EF_STEAL:
	case StructState::EF_STEAL_WITH_REGEN_STOP:
		{
			if( pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON )
			{
				ItemBase::ItemClass weapon_class = GetWeaponClass();

				if( !weapon_class )
				{
					break;
				}

				if( weapon_class != pEffect->fValue[ 8 ] && weapon_class != pEffect->fValue[ 9 ] && weapon_class != pEffect->fValue[ 10 ] && weapon_class != pEffect->fValue[ 11 ] )
					break;
			}

			int nHPSteal = pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1];
			int nMPSteal = pEffect->fValue[2] + pEffect->nEffectLevel * pEffect->fValue[3];
			int nRatio = pEffect->fValue[6] + pEffect->nEffectLevel * pEffect->fValue[7];

			if( !nHPSteal && !nMPSteal )
				break;

			m_vProcByAttack.push_back( _ATTACK_TAG( nRatio, 0, 0, 0, 0, StructState::NormalAttack, 99, NULL ) );
			m_vProcByAttack.back().proc = new StructStealProc( StructProc::ATTACKER, nHPSteal, nMPSteal );

			if( pEffect->nEffectID == StructState::EF_STEAL_WITH_REGEN_STOP )
			{
				if( pEffect->fValue[ 12 ] )
					m_StatusFlag.On( STATUS_HP_REGEN_STOPPED );
				if( pEffect->fValue[ 13 ] )
					m_StatusFlag.On( STATUS_MP_REGEN_STOPPED );
			}
		}
		break;

	case StructState::EF_DAMAGE_REFLECT_PERCENT:
		{
			m_vDamageReflectInfo.push_back( DamageReflectInfo( pEffect->fValue[ 6 ] + pEffect->nEffectLevel * pEffect->fValue[ 7 ], pEffect->fValue[ 9 ] * GameRule::DEFAULT_UNIT_SIZE,
				(Elemental::Type) (int) pEffect->fValue[ 8 ], 0,
				pEffect->fValue[ 0 ] + pEffect->nEffectLevel * pEffect->fValue[ 1 ],
				pEffect->fValue[ 2 ] + pEffect->nEffectLevel * pEffect->fValue[ 3 ],
				pEffect->fValue[ 4 ] + pEffect->nEffectLevel * pEffect->fValue[ 5 ],
				pEffect->fValue[ 10 ] ) );

			m_fPassivePhysicalReflect += pEffect->fValue[2] - (pEffect->fValue[3] * pEffect->nEffectLevel);
			m_fPassiveMagicalReflect += pEffect->fValue[4] - (pEffect->fValue[5] * pEffect->nEffectLevel);
		}
		break;

	case StructState::EF_DAMAGE_REFLECT:
		{
			m_vDamageReflectInfo.push_back( DamageReflectInfo( pEffect->fValue[ 6 ] + pEffect->nEffectLevel * pEffect->fValue[ 7 ], GameRule::REFLECT_RANGE,
				(Elemental::Type) (int) pEffect->fValue[ 8 ], pEffect->fValue[ 0 ] + pEffect->nEffectLevel * pEffect->fValue[ 1 ],
				m_fPassivePhysicalReflect,
				0.0,
				m_fPassiveMagicalReflect,
				pEffect->fValue[ 9 ] ) );
		}
		break;

	case StructState::EF_DAMAGE_REFLECT_WHEN_EQUIP_SHIELD:
		{
			if( IsWearShield() )
			{
				m_vStateReflectInfo.push_back( StateReflectInfo( static_cast< StructState::StateCode >( (int)pEffect->fValue[ 0 ] ), pEffect->fValue[ 3 ] + pEffect->nEffectLevel * pEffect->fValue[ 4 ], pEffect->fValue[ 1 ] + pEffect->nEffectLevel * pEffect->fValue[ 2 ] ) );
			}
		}
		break;

	case StructState::EF_REGEN_ADD:
		{
			m_Attribute.fHPRegenPoint += pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1];
			m_Attribute.fMPRegenPoint += pEffect->fValue[2] + pEffect->nEffectLevel * pEffect->fValue[3];
		}
		break;

	case StructState::EF_AMP_RECEIVE_DAMAGE:
		{
			// ApplyType(근거리/원거리 설정)은 일반 공격만 적용된다
			int nApplyType = pEffect->fValue[ 0 ];
			if( nApplyType == 1 )
			{
				m_NormalStatePenalty.fDamage += ( pEffect->fValue[7] + pEffect->nEffectLevel * pEffect->fValue[8] );
			}
			else if( nApplyType == 2 )
			{
				m_RangeStatePenalty.fDamage += ( pEffect->fValue[7] + pEffect->nEffectLevel * pEffect->fValue[8] );
			}
			else if( nApplyType == 99 )
			{
				m_NormalStatePenalty.fDamage += ( pEffect->fValue[7] + pEffect->nEffectLevel * pEffect->fValue[8] );
				m_RangeStatePenalty.fDamage += ( pEffect->fValue[7] + pEffect->nEffectLevel * pEffect->fValue[8] );
			}

			m_StateStatePenalty.fDamage += ( pEffect->fValue[3] + pEffect->nEffectLevel * pEffect->fValue[4] );
			m_PhysicalSkillStatePenalty.fDamage += ( pEffect->fValue[5] + pEffect->nEffectLevel * pEffect->fValue[6] );
			m_MagicalSkillStatePenalty.fDamage += ( pEffect->fValue[9] + pEffect->nEffectLevel * pEffect->fValue[10] );
		}
		break;

	case StructState::EF_INC_HATE:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();

			if( !weapon_class )
				break;

			if( weapon_class != pEffect->fValue[ 6 ] && pEffect->fValue[ 6 ] != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != pEffect->fValue[ 7 ] && pEffect->fValue[ 7 ] != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != pEffect->fValue[ 8 ] && pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != pEffect->fValue[ 9 ] && pEffect->fValue[ 9 ] != ItemBase::CLASS_EVERY_WEAPON )
				break;

			m_vHateMod.push_back( HateModifier( pEffect->fValue[10], pEffect->fValue[11], 0, pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel ) );
		}
		break;

	case StructState::EF_AMP_HATE:
		{
			ItemBase::ItemClass weapon_class = GetWeaponClass();

			if( !weapon_class )
				break;

			if( weapon_class != pEffect->fValue[ 6 ] && pEffect->fValue[ 6 ] != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != pEffect->fValue[ 7 ] && pEffect->fValue[ 7 ] != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != pEffect->fValue[ 8 ] && pEffect->fValue[ 8 ] != ItemBase::CLASS_EVERY_WEAPON &&
				weapon_class != pEffect->fValue[ 9 ] && pEffect->fValue[ 9 ] != ItemBase::CLASS_EVERY_WEAPON )
				break;

			m_vHateMod.push_back( HateModifier( pEffect->fValue[10], pEffect->fValue[11], pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel, 0 ) );
		}
		break;

	case StructState::EF_FORCE_CHIP:
		{
			c_fixed10 fNormalStatePenalty = ( pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel );
			c_fixed10 fRangeStatePenalty = ( pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel );
			c_fixed10 fPhysicalSkillStatePenalty = ( pEffect->fValue[2] + pEffect->fValue[3] * pEffect->nEffectLevel );

			if( IsPlayer() || IsSummon() )
			{
				fNormalStatePenalty = fNormalStatePenalty * 0.25f;
				fRangeStatePenalty = fRangeStatePenalty * 0.25f;
				fPhysicalSkillStatePenalty = fPhysicalSkillStatePenalty * 0.25f;
			}

			m_NormalStatePenalty.fDamage += fNormalStatePenalty;
			m_RangeStatePenalty.fDamage += fRangeStatePenalty;
			m_PhysicalSkillStatePenalty.fDamage += fPhysicalSkillStatePenalty;
		}
		break;

	case StructState::EF_SOUL_CHIP:
		{
			c_fixed10 fMagicalSkillStatePenalty = ( pEffect->fValue[2] + pEffect->nEffectLevel * pEffect->fValue[3] );

			if( IsPlayer() || IsSummon() )
			{
				fMagicalSkillStatePenalty = fMagicalSkillStatePenalty * 0.25f;
			}

			m_MagicalSkillStatePenalty.fDamage += fMagicalSkillStatePenalty;
		}
		break;

	case StructState::EF_HEALING_CHIP:
		{
			m_fHealRatio += ( pEffect->fValue[2] + pEffect->nEffectLevel * pEffect->fValue[3] );
		}
		break;

	case StructState::EF_LUNAR_CHIP:
		{
			c_fixed10 fNormalStatePenalty = ( pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel );
			c_fixed10 fRangeStatePenalty = ( pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel );
			c_fixed10 fPhysicalSkillStatePenalty = ( pEffect->fValue[2] + pEffect->fValue[3] * pEffect->nEffectLevel );
			c_fixed10 fMagicalSkillStatePenalty = ( pEffect->fValue[2] + pEffect->fValue[3] * pEffect->nEffectLevel );
			c_fixed10 fStateStatePenalty = ( pEffect->fValue[12] + pEffect->fValue[13] * pEffect->nEffectLevel );

			if( IsPlayer() || IsSummon() )
			{
				fNormalStatePenalty = fNormalStatePenalty * 0.25f;
				fRangeStatePenalty = fRangeStatePenalty * 0.25f;
				fPhysicalSkillStatePenalty = fPhysicalSkillStatePenalty * 0.25f;
				fMagicalSkillStatePenalty = fMagicalSkillStatePenalty * 0.25f;
				fStateStatePenalty = fStateStatePenalty * 0.25f;
			}

			m_NormalStatePenalty.fDamage += fNormalStatePenalty;
			m_RangeStatePenalty.fDamage += fRangeStatePenalty;
			m_PhysicalSkillStatePenalty.fDamage += fPhysicalSkillStatePenalty;
			m_MagicalSkillStatePenalty.fDamage += fMagicalSkillStatePenalty;
			m_StateStatePenalty.fDamage += fStateStatePenalty;
		}
		break;

	case StructState::EF_MP_COST_INC:
		{
			float fCostReduce = pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1];
			int nElementalType = pEffect->fValue[5];

			if( nElementalType != 99 )
			{
				if( pEffect->fValue[ 10 ] == 99 || pEffect->fValue[ 10 ] == 1 )
				{
					if( pEffect->fValue[ 11 ] == 99 || pEffect->fValue[ 11 ] == 0 )
					{
						m_GoodPhysicalElementalSkillStateMod[nElementalType].fManaCostRatio += fCostReduce;
					}

					if( pEffect->fValue[ 11 ] == 99 || pEffect->fValue[ 11 ] == 1 )
					{
						m_BadPhysicalElementalSkillStateMod[nElementalType].fManaCostRatio += fCostReduce;
					}
				}

				if( pEffect->fValue[ 10 ] == 99 || pEffect->fValue[ 10 ] == 2 )
				{
					if( pEffect->fValue[ 11 ] == 99 || pEffect->fValue[ 11 ] == 0 )
					{
						m_GoodMagicalElementalSkillStateMod[nElementalType].fManaCostRatio += fCostReduce;
					}

					if( pEffect->fValue[ 11 ] == 99 || pEffect->fValue[ 11 ] == 1 )
					{
						m_BadMagicalElementalSkillStateMod[nElementalType].fManaCostRatio += fCostReduce;
					}
				}
			}
			else
			{
				for( int i = 0; i < Elemental::TYPE_COUNT; ++i )
				{
					if( pEffect->fValue[ 10 ] == 99 || pEffect->fValue[ 10 ] == 1 )
					{
						if( pEffect->fValue[ 11 ] == 99 || pEffect->fValue[ 11 ] == 0 )
						{
							m_GoodPhysicalElementalSkillStateMod[i].fManaCostRatio += fCostReduce;
						}

						if( pEffect->fValue[ 11 ] == 99 || pEffect->fValue[ 11 ] == 1 )
						{
							m_BadPhysicalElementalSkillStateMod[i].fManaCostRatio += fCostReduce;
						}
					}

					if( pEffect->fValue[ 10 ] == 99 || pEffect->fValue[ 10 ] == 2 )
					{
						if( pEffect->fValue[ 11 ] == 99 || pEffect->fValue[ 11 ] == 0 )
						{
							m_GoodMagicalElementalSkillStateMod[i].fManaCostRatio += fCostReduce;
						}

						if( pEffect->fValue[ 11 ] == 99 || pEffect->fValue[ 11 ] == 1 )
						{
							m_BadMagicalElementalSkillStateMod[i].fManaCostRatio += fCostReduce;
						}
					}
				}
			}
		}
		break;

	case StructState::EF_ADD_PARAMETER_ON_NORMAL_ATTACK:
		{
			if( pEffect->fValue[ 9 ] == 99 || pEffect->fValue[ 9 ] == 1 )
			{
				m_RangeStateAdvantage.fDamage += pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel;
				m_RangeStateAdvantage.fCritical += pEffect->fValue[ 3 ] * pEffect->nEffectLevel;
				m_RangeStateAdvantage.nCritical += pEffect->fValue[ 4 ] * pEffect->nEffectLevel;
				m_RangeStateAdvantage.fHate += pEffect->fValue[ 5 ] + pEffect->fValue[ 6 ] * pEffect->nEffectLevel;
			}

			if( pEffect->fValue[ 9 ] == 99 || pEffect->fValue[ 9 ] == 0 )
			{
				m_NormalStateAdvantage.fDamage += pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel;
				m_NormalStateAdvantage.fCritical += pEffect->fValue[ 3 ] * pEffect->nEffectLevel;
				m_NormalStateAdvantage.nCritical += pEffect->fValue[ 4 ] * pEffect->nEffectLevel;
				m_NormalStateAdvantage.fHate += pEffect->fValue[ 6 ] * pEffect->nEffectLevel;
			}
		}
		break;

	case StructState::EF_ADD_PARAMETER_ON_SKILL:
		assert( 0 );
		break;

		//조작 관련 상태 이상
	case StructState::EF_MEZZ:
		// 모든 조작 불가
		if( pEffect->fValue[0] )
		{
			m_StatusFlag.Off( STATUS_MOVABLE );
			m_StatusFlag.Off( STATUS_ATTACKABLE );
			m_StatusFlag.Off( STATUS_SKILL_CASTABLE );
			m_StatusFlag.Off( STATUS_MAGIC_CASTABLE );
			m_StatusFlag.Off( STATUS_ITEM_USABLE );
		}

		// 이동 불가
		if( pEffect->fValue[1] )
			m_StatusFlag.Off( STATUS_MOVABLE );

		// 공격 불가
		if( pEffect->fValue[2] )
			m_StatusFlag.Off( STATUS_ATTACKABLE );

		// 물리 스킬 시전 불가
		if( pEffect->fValue[3] )
			m_StatusFlag.Off( STATUS_SKILL_CASTABLE );

		// 마법 스킬 시전 불가
		if( pEffect->fValue[4] )
			m_StatusFlag.Off( STATUS_MAGIC_CASTABLE );

		// 피격 불가
		if( pEffect->fValue[5] )
			m_StatusFlag.Off( STATUS_MORTAL );

		// HP 감소
		m_fMaxHPAmplifier -= pEffect->fValue[8] + pEffect->fValue[9] * pEffect->nEffectLevel;
		m_nMaxHP -= pEffect->fValue[6] + pEffect->fValue[7] * pEffect->nEffectLevel;

		// MP 감소
		m_fMaxMPAmplifier -= pEffect->fValue[12] + pEffect->fValue[13] * pEffect->nEffectLevel;
		m_nMaxMP -= pEffect->fValue[10] + pEffect->fValue[11] * pEffect->nEffectLevel;

		break;

	case StructState::EF_TRANSFORMATION:
		assert( 0 );
		break;

	case StructState::EF_ADD_HP_MP_ON_CRITICAL:
		m_vAddHPMPOnCritical.push_back( AddHPMPOnCriticalInfo( pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel, pEffect->fValue[2] + pEffect->fValue[3] * pEffect->nEffectLevel, pEffect->fValue[4] + pEffect->fValue[5] * pEffect->nEffectLevel ) );
		break;

	case StructState::EF_HEALING_AMPLIFY:
		{
			c_fixed10 fHPHealRatio = pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel;
			c_fixed10 fMPHealRatio = pEffect->fValue[2] + pEffect->fValue[3] * pEffect->nEffectLevel;

			m_fHealRatio += fHPHealRatio;
			m_fMPHealRatio += fMPHealRatio;

			m_nAdditionalHeal += pEffect->fValue[4] + pEffect->fValue[5] * pEffect->nEffectLevel;
			m_nAdditionalMPHeal += pEffect->fValue[6] + pEffect->fValue[7] * pEffect->nEffectLevel;
		}
		break;

	case StructState::EF_DETECT_HIDE:
		m_fDetectHideRange += ( pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1] ) * GameRule::DEFAULT_UNIT_SIZE;
		break;

	case StructState::EF_SKILL_INTERRUPTION:
		for( int i = 0 ; i < 12 && pEffect->fValue[ i ] ; ++i )
			m_vInterruptedSkill.push_back( pEffect->fValue[ i ] );
		break;

	case StructState::EF_DAMAGE_REDUCE_WITH_RACE_BY_PERCENT:
		m_vDamageReducePercentInfo.push_back( DamageReduceInfo( pEffect->fValue[6], pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1], pEffect->fValue[2] + pEffect->nEffectLevel * pEffect->fValue[3],
			pEffect->fValue[4] + pEffect->nEffectLevel * pEffect->fValue[5], pEffect->fValue[7], pEffect->fValue[8], pEffect->fValue[9], pEffect->fValue[10], pEffect->fValue[11] ) );
		break;

	case StructState::EF_DAMAGE_REDUCE_WITH_RACE_BY_VALUE:
		m_vDamageReduceValueInfo.push_back( DamageReduceInfo( pEffect->fValue[6], pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1], pEffect->fValue[2] + pEffect->nEffectLevel * pEffect->fValue[3],
			pEffect->fValue[4] + pEffect->nEffectLevel * pEffect->fValue[5], pEffect->fValue[7], pEffect->fValue[8], pEffect->fValue[9], pEffect->fValue[10], pEffect->fValue[11] ) );
		break;

	case StructState::EF_MANA_SHIELD:
		{
			int nTargetType = pEffect->fValue[4];
			if( nTargetType == 1 || nTargetType == 99 )
				m_fPhysicalDamageManaShieldAbsorbRatio += pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1];
			if( nTargetType == 2 || nTargetType == 99 )
				m_fMagicalDamageManaShieldAbsorbRatio += pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1];
		}
		break;

	case StructState::EF_AMP_AND_INC_ITEM_CHANCE:
		// nItemChance == 100 이면 아이템 드랍 체크시 해당 아이템의 드랍율이 2배로 적용됨(드랍율 % 증폭 개념 변수임)
		// GetValue( 0 ) + GetValue( 1 ) * GetLevel() == 1.0 -> 2배 증폭(nItemChance = 100 이 되야 함)
		// 정수 처리 특성상 결과값의 소수 2째 자리 미만은 전부 버려짐
			m_Attribute.fItemChance += ( pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel ) * 100;
			m_fEXPMod += pEffect->fValue[ 2 ] + pEffect->fValue[ 3 ] * pEffect->nEffectLevel;
			if( this->IsPlayer() )
			{
				StructPlayer *pPlayer = static_cast< StructPlayer * >( this );
				pPlayer->SetActiveSummonExpAmp( pPlayer->GetActiveSummonExpAmp() + pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel );
			}
		break;

	case StructState::EF_CHANGING_GROUP:
		m_nChangingGroup = pEffect->fValue[ 0 ];
		break;	

	case StructState::EF_HUNTING_TRAINING:
		{
			for( int i = 0; i < 3; i++ )
			{
				int nCreatureType = pEffect->fValue[ i * 5 ];

				c_fixed10 fDamage = pEffect->fValue[ i * 5 + 1 ] + pEffect->fValue[ i * 5 + 2 ] * pEffect->nEffectLevel;
				c_fixed10 fAvoid = pEffect->fValue[ i * 5 + 3 ] + pEffect->fValue[ i * 5 + 4 ] * pEffect->nEffectLevel;

				if( nCreatureType == CREATURE_ALL )
				{
					for( int i = 0 ; i < MAX_CREATURE_TYPE_NUMBER ; ++i )
					{
						m_Expert[ i ].fDamage += fDamage;
						m_Expert[ i ].fAvoid += fAvoid;
					}
				}
				else if( nCreatureType >= 0 && nCreatureType < MAX_CREATURE_TYPE_NUMBER )
				{
					m_Expert[ nCreatureType ].fDamage += fDamage;
					m_Expert[ nCreatureType ].fAvoid += fAvoid;
				}
				else
				{
					assert( 0 && "CREATURE DATA TYPE ERROR!" );
				}
			}

		}
		break;

	case StructState::EF_RESIST_HARMFUL_STATE:
		{
			for( int i = 0; i < 6; ++i )
			{
				StructState::StateCode code = static_cast< StructState::StateCode >( (int)pEffect->fValue[ i * 3 ] );
				c_fixed10 rate = pEffect->fValue[ i * 3 + 1 ] + pEffect->fValue[ i * 3 + 2 ] * pEffect->nEffectLevel;

				if( !code ) continue;

				std::map< StructState::StateCode, c_fixed10 >::iterator it = m_mapResistHarmfulState.find( code );
				if( it == m_mapResistHarmfulState.end() )
				{
					m_mapResistHarmfulState.insert( std::make_pair< StructState::StateCode, c_fixed10 >( code, rate ) );
				}
				else
				{
					(*it).second += rate;
				}
			}
		}
		break;

	case StructState::EF_ADD_REDUCE_DAMAGE_ON_ATTACK:
		{
			if( pEffect->fValue[7] == 0 || pEffect->fValue[7] == 99 )
				m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[3] + pEffect->fValue[4] * pEffect->nEffectLevel, Elemental::TYPE_NONE, (Elemental::Type) (int) pEffect->fValue[5], pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel, 0 ) );
			if( pEffect->fValue[7] == 1 || pEffect->fValue[7] == 99 )
				m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( pEffect->fValue[3] + pEffect->fValue[4] * pEffect->nEffectLevel, Elemental::TYPE_NONE, (Elemental::Type) (int) pEffect->fValue[5], pEffect->fValue[0] + pEffect->fValue[1] * pEffect->nEffectLevel, 0 ) );
			m_vDamageReduceValueInfo.push_back( DamageReduceInfo( pEffect->fValue[15] * pEffect->nEffectLevel * pEffect->fValue[16], pEffect->fValue[9] + pEffect->nEffectLevel * pEffect->fValue[10], pEffect->fValue[11] + pEffect->nEffectLevel * pEffect->fValue[12],
				pEffect->fValue[13] + pEffect->nEffectLevel * pEffect->fValue[14], pEffect->fValue[17], pEffect->fValue[18], pEffect->fValue[19], pEffect->fValue[19], pEffect->fValue[19] ) );
			// TODO: 유형 체크에 20번 값을 중복하여 넣은 것은 뒤의 두 값은 사용되지 않기 때문. 동작 자체에는 차이가 없을 것이나 리뷰가 필요할 듯 함
		}
		break;

	case StructState::EF_DAMAGE_REDUCE_BY_PERCENT_AND_VALUE:
		{
			m_vDamageReducePercentInfo.push_back( DamageReduceInfo( pEffect->fValue[6], pEffect->fValue[0] + pEffect->nEffectLevel * pEffect->fValue[1], pEffect->fValue[2] + pEffect->nEffectLevel * pEffect->fValue[3],
				pEffect->fValue[4] + pEffect->nEffectLevel * pEffect->fValue[5], pEffect->fValue[7], pEffect->fValue[8], pEffect->fValue[9], pEffect->fValue[9], pEffect->fValue[9] ) );
			m_vDamageReduceValueInfo.push_back( DamageReduceInfo( pEffect->fValue[16], pEffect->fValue[10] + pEffect->nEffectLevel * pEffect->fValue[11], pEffect->fValue[12] + pEffect->nEffectLevel * pEffect->fValue[13],
				pEffect->fValue[14] + pEffect->nEffectLevel * pEffect->fValue[15], pEffect->fValue[17], pEffect->fValue[18], pEffect->fValue[19], pEffect->fValue[19], pEffect->fValue[19] ) );
			// TODO: 유형 체크에 10, 20번 값을 중복하여 넣은 것은 뒤의 두 값은 사용되지 않기 때문. 동작 자체에는 차이가 없을 것이나 리뷰가 필요할 듯 함
		}
		break;

	case StructState::EF_INCREASE_ENERGY_UNCONSUMPTION_RATE:
		{
			m_nEnergyUnconsumptionRate += pEffect->fValue[ 0 ] + pEffect->fValue[ 1 ] * pEffect->nEffectLevel;
		}
		break;

	case StructState::EF_ADD_STATE_BY_EQUIP_ITEM:
		{
			AR_TIME t = GetArTime();

			for( int i = 0; i < 4; ++i )
			{
				int nStateCode = pEffect->fValue[ 3 * i + 0 ];
				int nStateLevel = pEffect->fValue[ 3 * i + 1 ];

				if ( nStateCode != 0 )
				{
					// 장비하고 있는 동안 지속적으로 걸어주는 지속효과이므로 기본적으로 오오라의 성격을 따른다.
					PendAddStateByItem( (StructState::StateCode)nStateCode, GetHandle(), nStateLevel, t, t + SkillBase::TOGGLE_LIVE_TIME, true, 0, "", false );
				}				
			}
		}
		break;

	case StructState::EF_MISC:
		assert( 0 );
		break;
	}
}

void StructPlayer::_applyEffectForState( const EffectInfo * pEffect )
{
	switch( pEffect->nEffectID )
	{
	case StructState::EF_PARAMETER_INC_TO_ACTIVE_SUMMON:
		incParameter( pEffect->fValue[ 0 ], pEffect->fValue[ 1 ] + pEffect->fValue[ 2 ] * pEffect->nEffectLevel, true, &m_ParameterForSummon );
		incParameter( pEffect->fValue[ 0 ], pEffect->fValue[ 1 ] + pEffect->fValue[ 2 ] * pEffect->nEffectLevel, false, &m_ParameterForSummon );
		incParameter( pEffect->fValue[ 3 ], pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel, true, &m_ParameterForSummon );
		incParameter( pEffect->fValue[ 3 ], pEffect->fValue[ 4 ] + pEffect->fValue[ 5 ] * pEffect->nEffectLevel, false, &m_ParameterForSummon );
		incParameter2( pEffect->fValue[ 6 ], pEffect->fValue[ 7 ] + pEffect->fValue[ 8 ] * pEffect->nEffectLevel, &m_ParameterForSummon );
		incParameter2( pEffect->fValue[ 9 ], pEffect->fValue[ 10 ] + pEffect->fValue[ 11 ] * pEffect->nEffectLevel, &m_ParameterForSummon );
		incParameter( pEffect->fValue[ 12 ], pEffect->fValue[ 13 ] + pEffect->fValue[ 14 ] * pEffect->nEffectLevel, true, &m_ParameterForSummon );
		incParameter( pEffect->fValue[ 12 ], pEffect->fValue[ 13 ] + pEffect->fValue[ 14 ] * pEffect->nEffectLevel, false, &m_ParameterForSummon );
		incParameter( pEffect->fValue[ 15 ], pEffect->fValue[ 16 ] + pEffect->fValue[ 17 ] * pEffect->nEffectLevel, true, &m_ParameterForSummon );
		incParameter( pEffect->fValue[ 15 ], pEffect->fValue[ 16 ] + pEffect->fValue[ 17 ] * pEffect->nEffectLevel, false, &m_ParameterForSummon );
		break;
	}

	StructCreature::_applyEffectForState( pEffect );
}

void StructSummon::_applyEffectForState( const EffectInfo * pEffect )
{
	switch( pEffect->nEffectID )
	{
	case StructState::EF_HEAL:
		{
			m_nSPAdd += pEffect->fValue[ 4 ] + pEffect->nEffectLevel * pEffect->fValue[ 5 ];
		}
		break;
	case StructState::EF_REGEN_ADD:
		{
			m_nSPRegenAdd += pEffect->fValue[ 4 ] + pEffect->nEffectLevel * pEffect->fValue[ 5 ];
		}
		break;
	case StructState::EF_HEAL_BY_ITEM:
		{
			m_nSPAdd += pEffect->fValue[ 4 ] + pEffect->nEffectLevel * pEffect->fValue[ 5 ];
		}
		break;
	 }

	StructCreature::_applyEffectForState( pEffect );
}

void StructCreature::_applySkillByEffect( const std::vector< EffectInfo * > * pvEffectList )
{
	if( !pvEffectList || pvEffectList->empty() )
		return;

	for( std::vector< EffectInfo * >::const_iterator it = pvEffectList->begin(); it != pvEffectList->end(); it++ )
	{
		if( (*it)->eType != EffectInfo::EFFECT_TYPE_STATE )
			continue;

		if( (*it)->nMinLevel && GetLevel() < (*it)->nMinLevel ) continue;
		if( (*it)->nMaxLevel && GetLevel() > (*it)->nMaxLevel ) continue;

		if( (*it)->nEffectID == StructState::EF_ADD_SKILL )
		{
			addSkill( (*it)->fValue[ 0 ], (*it)->fValue[ 1 ] + (*it)->fValue[ 2 ] * (*it)->nEffectLevel, (*it)->fValue[ 18 ], false );
			addSkill( (*it)->fValue[ 3 ], (*it)->fValue[ 4 ] + (*it)->fValue[ 5 ] * (*it)->nEffectLevel, (*it)->fValue[ 18 ], false );
			addSkill( (*it)->fValue[ 6 ], (*it)->fValue[ 7 ] + (*it)->fValue[ 8 ] * (*it)->nEffectLevel, (*it)->fValue[ 18 ], false );
			addSkill( (*it)->fValue[ 9 ], (*it)->fValue[ 10 ] + (*it)->fValue[ 11 ] * (*it)->nEffectLevel, (*it)->fValue[ 18 ], false );
			addSkill( (*it)->fValue[ 12 ], (*it)->fValue[ 13 ] + (*it)->fValue[ 14 ] * (*it)->nEffectLevel, (*it)->fValue[ 18 ], false );

			addSkill( (*it)->fValue[ 15 ], (*it)->fValue[ 16 ] + (*it)->fValue[ 17 ] * (*it)->nEffectLevel, (*it)->fValue[ 18 ], true );
		}
	}
}

void StructCreature::applyStatByItem()
{
	int i, x;

	for( i = 0; i < ItemBase::MAX_ITEM_WEAR; ++i )
	{
		if( !m_anWear[i] ) continue;

		// 장착 불가는 효과 없음
		if( !IsWearable( (ItemBase::ItemWearType)i, m_anWear[i] ) ) continue;

		const ItemBaseServer & base = m_anWear[i]->GetItemBase();

		for( x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
		{
			if( !base.nOptType[x] ) continue;

			if( base.nOptType[x] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )
			{
				incParameter( base.fOptVar1[x], base.fOptVar2[x], true );
			}
		}

		// 스킬의 변화도 스탯에 영향을 미치므로 함께 변경한다.
		_applyStatByEffect( base.pvEffectList );
		_applySkillByEffect( base.pvEffectList );

		for( x = 0 ; x < m_anWear[i]->GetMaxSocketCount() ; ++x )
		{
			ItemBase::ItemCode nSocketCode = m_anWear[i]->GetSocketCode( x );

			if( !nSocketCode )
				continue;

			const ItemBaseServer & SocketBase = StructItem::GetItemBase( nSocketCode );

			// 레벨 제한 적용 가능 검사
			if( GetLevel() < SocketBase.nMinLevel || GetLevel() > SocketBase.nMaxLevel )
				continue;

			// 소켓 내구도 검사
			if( !m_anWear[i]->GetCurrentEndurance() )
				continue;

			for( int k = 0; k < ItemBase::MAX_OPTION_NUMBER; ++k )
			{
				if( !SocketBase.nOptType[k] ) continue;

				if( SocketBase.nOptType[k] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )
				{
					incParameter( SocketBase.fOptVar1[k], SocketBase.fOptVar2[k], true );

				}
			}

			// 스킬의 변화도 스탯에 영향을 미치므로 함께 변경한다.
			_applyStatByEffect( SocketBase.pvEffectList );
			_applySkillByEffect( SocketBase.pvEffectList );
		}

		// 각성 여부 확인
		if( m_anWear[i]->IsAwaken() )
		{
			for( int nCnt = 0 ; nCnt < ItemInstance::MAX_AWAKEN_NUMBER ; ++nCnt )
			{
				if( !m_anWear[i]->GetAwakenOptionType( nCnt ) )
					break;

				if( m_anWear[i]->GetAwakenOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )
				{
					incParameter( m_anWear[i]->GetAwakenOptionValue1( nCnt ), m_anWear[i]->GetAwakenOptionValue2( nCnt ), true );
				}
				else if( m_anWear[i]->GetAwakenOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::APPLY_EFFECT )
				{
					// 스킬의 변화도 스탯에 영향을 미치므로 함께 변경한다.
					const std::vector< EffectInfo * > *pEffectVector = GameContent::GetEffectInfoVector( m_anWear[i]->GetAwakenOptionValue1( nCnt ) );
					_applyStatByEffect( pEffectVector );
					_applySkillByEffect( pEffectVector );
				}
			}
		}

		// 랜덤화 여부 확인
		if( m_anWear[i]->IsIdentified() )
		{
			for( int nCnt = 0 ; nCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++nCnt )
			{
				if( !m_anWear[i]->GetIdentifiedOptionType( nCnt ) )
					break;

				if( m_anWear[i]->GetIdentifiedOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )
				{
					incParameter( m_anWear[i]->GetIdentifiedOptionValue1( nCnt ), m_anWear[i]->GetIdentifiedOptionValue2( nCnt ), true );
				}
				else if( m_anWear[i]->GetIdentifiedOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::APPLY_EFFECT )
				{
					// 스킬의 변화도 스탯에 영향을 미치므로 함께 변경한다.
					const std::vector< EffectInfo * > *pEffectVector = GameContent::GetEffectInfoVector( m_anWear[i]->GetIdentifiedOptionValue1( nCnt ) );
					_applyStatByEffect( pEffectVector );
					_applySkillByEffect( pEffectVector );
				}
			}
		}
	}

	for( std::vector< SetItemEffectInfo * >::const_iterator itSetItemEffectInfo = m_vSetItemEffect.begin(); itSetItemEffectInfo != m_vSetItemEffect.end(); ++itSetItemEffectInfo )
	{
		for( x = 0 ; x < ItemBase::MAX_OPTION_NUMBER ; ++x )
		{
			if( !(*itSetItemEffectInfo)->nOptType[x] ) continue;

			if( (*itSetItemEffectInfo)->nOptType[x] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )
			{
				incParameter( (*itSetItemEffectInfo)->fOptVar1[x], (*itSetItemEffectInfo)->fOptVar2[x], true );
			}
		}

		// 스킬의 변화도 스탯에 영향을 미치므로 함께 변경한다.
		_applyStatByEffect( (*itSetItemEffectInfo)->pvEffectList );
		_applySkillByEffect( (*itSetItemEffectInfo)->pvEffectList );
	}
}

void StructCreature::_applyStatByEffect( const std::vector< EffectInfo * > * pvEffectList, float fMod )
{
	if( !pvEffectList || pvEffectList->empty() )
		return;

	for( std::vector< EffectInfo * >::const_iterator it = pvEffectList->begin() ; it != pvEffectList->end() ; ++it )
	{
		if( (*it)->eType != EffectInfo::EFFECT_TYPE_OPTIONAL )
			continue;

		if( (*it)->nMinLevel && GetLevel() < (*it)->nMinLevel ) continue;
		if( (*it)->nMaxLevel && GetLevel() > (*it)->nMaxLevel ) continue;

		if (fMod <= 1 || fMod >= 2)
		{
			fMod = 1;
		}

		if( (*it)->fValue[ 0 ] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )	incParameter( (*it)->fValue[ 1 ], (*it)->fValue[ 2 ] * fMod, true );
		if( (*it)->fValue[ 3 ] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )	incParameter( (*it)->fValue[ 4 ], (*it)->fValue[ 5 ] * fMod, true );
		if( (*it)->fValue[ 6 ] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )	incParameter( (*it)->fValue[ 7 ], (*it)->fValue[ 8 ] * fMod, true );
		if( (*it)->fValue[ 9 ] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )	incParameter( (*it)->fValue[ 10 ], (*it)->fValue[ 11 ] * fMod, true );
	}
}

void StructCreature::amplifyStatByItem()
{
	for( int i = 0; i < ItemBase::MAX_ITEM_WEAR; ++i )
	{
		if( !m_anWear[i] ) continue;

		// 장착 불가는 효과 없음
		if( !IsWearable( (ItemBase::ItemWearType)i, m_anWear[i] ) ) continue;

		const ItemBaseServer & base = m_anWear[i]->GetItemBase();

		for( int x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
		{
			if( !base.nOptType[x] ) continue;

			if( base.nOptType[x] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )
			{
				ampParameter( base.fOptVar1[x], base.fOptVar2[x], true );
			}
		}

		_amplifyStatByEffect( base.pvEffectList );

		for( int x = 0 ; x < m_anWear[i]->GetMaxSocketCount() ; ++x )
		{
			ItemBase::ItemCode nSocketCode = m_anWear[i]->GetSocketCode( x );

			if( !nSocketCode )
				continue;

			const ItemBaseServer & SocketBase = StructItem::GetItemBase( nSocketCode );

			// 레벨 제한 적용 가능 검사
			if( GetLevel() < SocketBase.nMinLevel || GetLevel() > SocketBase.nMaxLevel )
				continue;

			// 소켓 내구도 검사
			if( !m_anWear[i]->GetCurrentEndurance() )
				continue;

			for( int x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
			{
				if( !SocketBase.nOptType[x] ) continue;

				if( SocketBase.nOptType[x] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )
				{
					ampParameter( SocketBase.fOptVar1[x], SocketBase.fOptVar2[x], true );
				}
			}

			_amplifyStatByEffect( SocketBase.pvEffectList );
		}

		// 각성 여부 확인
		if( m_anWear[i]->IsAwaken() )
		{
			for( int nCnt = 0 ; nCnt < ItemInstance::MAX_AWAKEN_NUMBER ; ++nCnt )
			{
				if( !m_anWear[i]->GetAwakenOptionType( nCnt ) )
					break;

				if( m_anWear[i]->GetAwakenOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )
				{
					ampParameter( m_anWear[i]->GetAwakenOptionValue1( nCnt ), m_anWear[i]->GetAwakenOptionValue2( nCnt ), true );
				}
				else if( m_anWear[i]->GetAwakenOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::APPLY_EFFECT )
				{
					_amplifyStatByEffect( GameContent::GetEffectInfoVector( m_anWear[i]->GetAwakenOptionValue1( nCnt ) ) );
				}
			}
		}

		// 랜덤화 여부 확인
		if( m_anWear[i]->IsIdentified() )
		{
			for( int nCnt = 0 ; nCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++nCnt )
			{
				if( !m_anWear[i]->GetIdentifiedOptionType( nCnt ) )
					break;

				if( m_anWear[i]->GetIdentifiedOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )
				{
					ampParameter( m_anWear[i]->GetIdentifiedOptionValue1( nCnt ), m_anWear[i]->GetIdentifiedOptionValue2( nCnt ), true );
				}
				else if( m_anWear[i]->GetIdentifiedOptionType( nCnt ) == ITEM_EFFECT_PASSIVE::APPLY_EFFECT )
				{
					_amplifyStatByEffect( GameContent::GetEffectInfoVector( m_anWear[i]->GetIdentifiedOptionValue1( nCnt ) ) );
				}
			}
		}
	}

	for( std::vector< SetItemEffectInfo * >::const_iterator itSetItemEffectInfo = m_vSetItemEffect.begin(); itSetItemEffectInfo != m_vSetItemEffect.end(); ++itSetItemEffectInfo )
	{
		for( int x = 0 ; x < ItemBase::MAX_OPTION_NUMBER ; ++x )
		{
			if( !(*itSetItemEffectInfo)->nOptType[x] ) continue;

			if( (*itSetItemEffectInfo)->nOptType[x] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )
			{
				ampParameter( (*itSetItemEffectInfo)->fOptVar1[x], (*itSetItemEffectInfo)->fOptVar2[x], true );
			}
		}

		_amplifyStatByEffect( (*itSetItemEffectInfo)->pvEffectList );
	}
}

void StructCreature::_amplifyStatByEffect( const std::vector< EffectInfo * > * pvEffectList )
{
	if( !pvEffectList || pvEffectList->empty() )
		return;

	for( std::vector< EffectInfo * >::const_iterator it = pvEffectList->begin() ; it != pvEffectList->end() ; ++it )
	{
		if( (*it)->eType != EffectInfo::EFFECT_TYPE_OPTIONAL )
			continue;

		if( (*it)->nMinLevel && GetLevel() < (*it)->nMinLevel ) continue;
		if( (*it)->nMaxLevel && GetLevel() > (*it)->nMaxLevel ) continue;

		if( (*it)->fValue[ 0 ] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )	ampParameter( (*it)->fValue[ 1 ], (*it)->fValue[ 2 ], true );
		if( (*it)->fValue[ 3 ] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )	ampParameter( (*it)->fValue[ 4 ], (*it)->fValue[ 5 ], true );
		if( (*it)->fValue[ 6 ] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )	ampParameter( (*it)->fValue[ 7 ], (*it)->fValue[ 8 ], true );
		if( (*it)->fValue[ 9 ] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )	ampParameter( (*it)->fValue[ 10 ], (*it)->fValue[ 11 ], true );
	}
}

void StructCreature::applyParamBasedItemEffect()
{
	// 지속 효과 하나가 파라메터에 기반하여 성능이 바뀌기 때문에 어쩔 수 없이 분리
	// TODO: 차후 스탯 적용 시점과 아이템 효과 적용 시점을 분리할 때 이 함수도 합쳐 볼 방안을 찾아본다

	// 유닛 익스퍼트는 이미 계산되어 있음
	for( int i = 0; i < ItemBase::MAX_ITEM_WEAR; ++i )
	{
		if( !m_anWear[i] ) continue;

		// 장착 불가는 효과 없음
		if( !IsWearable( (ItemBase::ItemWearType)i, m_anWear[i] ) ) continue;

		float fItemRatio = 1.0f;

		// 유니트 익스퍼트로 입은 아이템.
		if( m_anWear[i]->GetLevelLimit() > GetLevel() && m_anWear[i]->GetLevelLimit() <= m_nUnitExpertLevel ) fItemRatio = 0.4f;

		const ItemBaseServer & base = m_anWear[i]->GetItemBase();

		_applyParamBasedEffect( base.pvEffectList );

		int nSocketCount = m_anWear[i]->GetMaxSocketCount();
		for( int x = 0 ; x < nSocketCount ; ++x )
		{
			ItemBase::ItemCode nSocketCode = m_anWear[i]->GetSocketCode( x );

			if( !nSocketCode )
				continue;

			const ItemBaseServer & SocketBase = StructItem::GetItemBase( nSocketCode );

			// 레벨 제한 적용 가능 검사
			if( GetLevel() < SocketBase.nMinLevel || GetLevel() > SocketBase.nMaxLevel )
				continue;

			// 소켓 내구도 검사
			if( !m_anWear[i]->GetCurrentEndurance() )
				continue;

			_applyParamBasedEffect( SocketBase.pvEffectList );
		}
	}

	for( std::vector< SetItemEffectInfo * >::const_iterator itSetItemEffectInfo = m_vSetItemEffect.begin() ; itSetItemEffectInfo != m_vSetItemEffect.end() ; ++itSetItemEffectInfo )
	{
		_applyParamBasedEffect( (*itSetItemEffectInfo)->pvEffectList );
	}
}

void StructCreature::_applyParamBasedEffect( const std::vector< EffectInfo * > * pvEffectList )
{
	if( !pvEffectList || pvEffectList->empty() )
		return;

	for( std::vector< EffectInfo * >::const_iterator it = pvEffectList->begin() ; it != pvEffectList->end() ; ++it )
	{
		EffectInfo * pEffect = (*it);
		if( pEffect->nMinLevel && GetLevel() < pEffect->nMinLevel ) continue;
		if( pEffect->nMaxLevel && GetLevel() > pEffect->nMaxLevel ) continue;

		if ( pEffect->eType == EffectInfo::EFFECT_TYPE_STATE )
		{
			switch ( pEffect->nEffectID )
			{
			case StructState::EF_INC_DOT_BASED_PARAM:
				{
					c_fixed10 sum = CalcStatSum( pEffect->fValue[12], m_StatBeforeParamBasedCalc )
						+ CalcAttributeSum( pEffect->fValue[12], pEffect->fValue[13], m_AttrBeforeParamBasedCalc );
					int type = pEffect->fValue[0];

					m_ElementalStateDamageAdder[type] += sum * (pEffect->fValue[1] + pEffect->fValue[2] * pEffect->nEffectLevel);
					m_ElementalStateDamageAmplifier[type] += sum * (pEffect->fValue[3] + pEffect->fValue[4] * pEffect->nEffectLevel);

					sum = CalcStatSum( pEffect->fValue[14], m_StatBeforeParamBasedCalc )
						+ CalcAttributeSum( pEffect->fValue[14], pEffect->fValue[15], m_AttrBeforeParamBasedCalc );

					type = pEffect->fValue[6];

					m_ElementalStateDamageAdder[type] += sum * (pEffect->fValue[7] + pEffect->fValue[8] * pEffect->nEffectLevel);
					m_ElementalStateDamageAmplifier[type] += sum * (pEffect->fValue[9] + pEffect->fValue[10] * pEffect->nEffectLevel);
				}
			}
		}
	}
}

void StructCreature::onItemWearEffect( struct StructItem* pItem, bool bIsBaseVar, int type, c_fixed10 var1, c_fixed10 var2, float fRatio )
{
	// 공속의 경우는 페널티 안받도록 수정
	c_fixed10 item_var_penalty = var1;

	if (pItem && type != ITEM_EFFECT_PASSIVE::ATTACK_SPEED)
	{
		item_var_penalty = var1 * fRatio;
		var2 *= fRatio;

		if (bIsBaseVar)
		{
			item_var_penalty += var2 * (pItem->GetItemLevel() - 1);
			//item_var_penalty = GameRule::GetItemValue(item_var_penalty, var1, GetLevel(), pItem->GetItemRank(), pItem->GetItemLevel(), pItem->GetItemBase().nMinLevel);
		}
	}

	// 아이템 사용 효율 설정(공속의 경우는 크리처 아이템 익스퍼트 영향을 안받도록 수정)
	if( type != ITEM_EFFECT_PASSIVE::ATTACK_SPEED )
		item_var_penalty *= m_fItemMod;

	switch( type )
	{	
	case ITEM_EFFECT_PASSIVE::MAGIC_POINT	:
		{
			m_Attribute.fMagicPoint += item_var_penalty;
			break;
		}
	case ITEM_EFFECT_PASSIVE::ATTACK_POINT	:	
		{
			bool bApplyRight = true;
			bool bApplyLeft = false;

			if( IsUsingDoubleWeapon() )
			{
				if( pItem && pItem->IsWeapon() )
				{
					if( pItem->GetWearInfo() == ItemBase::WEAR_LEFTHAND )
					{
						bApplyLeft = true;
						bApplyRight = false;
					}
				}
				else
				{
					bApplyLeft = true;
				}
			}

			if( bApplyLeft )
			{
				m_Attribute.fAttackPointLeft	+= item_var_penalty;
				//m_nAttackPointLeftByWeapon		+= item_var_penalty;
			}

			if( bApplyRight )
			{
				m_Attribute.fAttackPointRight	+= item_var_penalty;
				//m_nAttackPointRightByWeapon		+= item_var_penalty;
			}

			break;
		}
	case ITEM_EFFECT_PASSIVE::BLOCK_DEFENCE :	m_Attribute.fBlockDefence	+= item_var_penalty;		break;
	case ITEM_EFFECT_PASSIVE::DEFENCE		:	m_Attribute.fDefence		+= item_var_penalty;		break;
	case ITEM_EFFECT_PASSIVE::ACCURACY		:
		{
			bool bApplyRight = true;
			bool bApplyLeft = false;

			if( IsUsingDoubleWeapon() )
			{
				if( pItem && pItem->IsWeapon() )
				{
					if(pItem->GetWearInfo() == ItemBase::WEAR_LEFTHAND )
					{
						bApplyLeft = true;
						bApplyRight = false;
					}
				}
				else
				{
					bApplyLeft = true;
				}
			}

			if( bApplyLeft )
			{
				m_Attribute.fAccuracyLeft += item_var_penalty;
			}

			if( bApplyRight )
			{
				m_Attribute.fAccuracyRight += item_var_penalty;
			}

			break;
		}
	case ITEM_EFFECT_PASSIVE::ATTACK_SPEED	:
		{
			// 이도류 일 때는 양손 각 장비를 따로, 손에 착용한게 아닐 경우 양손 모두 적용
			bool bApplyRight = true;
			bool bApplyLeft = false;

			if( IsUsingDoubleWeapon() )
			{
				if( pItem && pItem->IsWeapon() )
				{
					if( pItem->GetWearInfo() == ItemBase::WEAR_LEFTHAND )
					{
						bApplyLeft = true;
						bApplyRight = false;
					}
				}
				else
				{
					bApplyLeft = true;
				}
			}

			if( bApplyLeft )
			{
				m_Attribute.fAttackSpeedLeft += item_var_penalty;
			}

			if( bApplyRight )
			{
				m_Attribute.fAttackSpeedRight += item_var_penalty;
			}

			break;
		}
	case ITEM_EFFECT_PASSIVE::MAGIC_DEFENCE	:	m_Attribute.fMagicDefence += item_var_penalty;			break;
	case ITEM_EFFECT_PASSIVE::AVOID			:	m_Attribute.fAvoid += item_var_penalty;					break;
	case ITEM_EFFECT_PASSIVE::MOVE_SPEED	:
		if( !isMoveSpeedFixed() )	// fRidingSpeed 수치가 생기면서 라이딩 중에도 기본 속도에 관여할 수 있게 변경 ( 2013-12-20 )
		{
			m_Attribute.fMoveSpeed += item_var_penalty;
		}
		break;
	case ITEM_EFFECT_PASSIVE::BLOCK_CHANGE	:	m_Attribute.fBlockChance += item_var_penalty;					break;
	case ITEM_EFFECT_PASSIVE::CARRY_WEIGHT	:	m_Attribute.fMaxWeight += item_var_penalty;						break;
	case ITEM_EFFECT_PASSIVE::CASTING_SPEED	:	m_Attribute.fCastingSpeed += item_var_penalty;					break;
	case ITEM_EFFECT_PASSIVE::MAGIC_ACCURACY:	m_Attribute.fMagicAccuracy += item_var_penalty;					break;
	case ITEM_EFFECT_PASSIVE::MAGIC_AVOID	:	m_Attribute.fMagicAvoid += item_var_penalty;					break;
	case ITEM_EFFECT_PASSIVE::COOLTIME_SPEED:	m_Attribute.fCoolTimeSpeed += item_var_penalty;					break;
	case ITEM_EFFECT_PASSIVE::MAX_HP		:	m_nMaxHP += int( item_var_penalty );							break;
	case ITEM_EFFECT_PASSIVE::MAX_MP		:	m_nMaxMP += int( item_var_penalty );							break;
	case ITEM_EFFECT_PASSIVE::MP_REGEN_POINT:	m_Attribute.fMPRegenPoint += item_var_penalty;					break;

	case ITEM_EFFECT_PASSIVE::BOW_INTERVAL	:	m_fBowInterval = var1;											break;
	case ITEM_EFFECT_PASSIVE::INC_PARAMETER_A:	incParameter( var1, var2, false );								break;
	case ITEM_EFFECT_PASSIVE::INC_PARAMETER_B:	incParameter2( var1, var2 );									break;
	case ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A:	ampParameter( var1, var2, false );								break;
	case ITEM_EFFECT_PASSIVE::AMP_PARAMETER_B:	ampParameter2( var1, var2 );									break;
	
	case ITEM_EFFECT_PASSIVE::INC_DOUBLE_ATTACK:
		if (IsCreature() && (var2 > 0)) 
		{
			m_Attribute.fDoubleAttackRatio += var2;
		}
		break;
	case ITEM_EFFECT_PASSIVE::INC_COOLTIME_SPEED:
		if (IsCreature() && (var2 > 0))
		{
			float amt = var2 / 100.0;
			m_Attribute.fCoolTimeSpeed += (1.0 / ((1.0 - amt) / amt)) * 100.0;
		}
		break;
	case ITEM_EFFECT_PASSIVE::INC_CASTING_SPEED:
		if (IsCreature() && (var2 > 0))
		{
			float amt = var2 / 100.0;
			m_Attribute.fCastingSpeed += (1.0 / ((1.0 - amt) / amt)) * 100.0;
		}
		break;
	case ITEM_EFFECT_PASSIVE::RESIST_NEGATIVE:
		if (IsCreature() && (var2 > 0))
		{
			m_Attribute.fStunResistance += (var2 / 100.0);
		}
		break;
	case ITEM_EFFECT_PASSIVE::INC_SUMMON_FCM:
		if (IsCreature() && (var2 > 0))
		{
			this->m_fSummonFCM = (var2 / 100.0);
		}
		break;
  

	default:	break;
	}
}


void StructPlayer::onItemWearEffect( struct StructItem* pItem, bool bIsBaseVar, int type, c_fixed10 var1, c_fixed10 var2, float fItemRatio )
{
	switch( type )
	{	
	case ITEM_EFFECT_PASSIVE::MAX_CHAOS		:
		if( !pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_NON_CHAOS_STONE ) )
		{
			m_nMaxChaos += int(var1) + int(var2) * ( pItem->GetItemLevel() + 10 * pItem->GetItemEnhance() );
		}
		break;
	default:	StructCreature::onItemWearEffect( pItem, bIsBaseVar, type, var1, var2, fItemRatio );					break;
	}
}

void StructSummon::onItemWearEffect( struct StructItem* pItem, bool bIsBaseVar, int type, c_fixed10 var1, c_fixed10 var2, float fRatio )
{
	if( bIsBaseVar )
	{
		switch( type )
		{
		case ITEM_EFFECT_PASSIVE::ATTACK_POINT: var1 = var1 * m_fBaseAttackPointRatio; var2 = var2 * m_fBaseAttackPointRatio; break;
		case ITEM_EFFECT_PASSIVE::MAGIC_POINT: var1 = var1 * m_fBaseMagicPointRatio; var2 = var2 * m_fBaseMagicPointRatio; break;
		case ITEM_EFFECT_PASSIVE::DEFENCE: var1 = var1 * m_fBaseDefenceRatio; var2 = var2 * m_fBaseDefenceRatio; break;
		case ITEM_EFFECT_PASSIVE::MAGIC_DEFENCE: var1 = var1 * m_fBaseMagicDefenceRatio; var2 = var2 * m_fBaseMagicDefenceRatio; break;
		default: break;
		}
	}

	StructCreature::onItemWearEffect( pItem, bIsBaseVar, type, var1, var2, fRatio );
}

void StructPlayer::applyStatByTitle()
{
	// 주 호칭
	if( GetMainTitle() )
	{
		const TitleBaseServer * base = GetMainTitle();

		for( int i = 0; i < TitleBase::MAX_OPTION_NUMBER; i++ )
		{
			if( !base->nOptType[i] ) continue;

			if( base->nOptType[i] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )
			{
				incParameter( base->fOptVar1[i], base->fOptVar2[i], true );
			}

			// 스킬의 변화도 스탯에 영향을 미치므로 함께 변경한다.
			_applyStatByEffect( base->pvEffectList );
			_applySkillByEffect( base->pvEffectList );
		}
	}

	// 부 호칭
	for( int i = 0; i < GameRule::SUB_TITLE_COUNT; i++ )
	{
		const TitleBaseServer * base = GetSubTitle( i );

		if( !base )
			continue;

		for( int x = 0; x < TitleBase::MAX_OPTION_NUMBER; x++ )
		{
			if( !base->nOptType[x] ) continue;

			// 부 호칭으로 사용 시에는 성능의 10%만을 적용한다.
			if( base->nOptType[x] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )
			{
				incParameter( base->fOptVar1[x], base->fOptVar2[x] * GameRule::SUB_TITLE_RATE, true );
			}
		}
	}
}

void StructPlayer::amplifyStatByTitle()
{
	// 주 호칭
	if( GetMainTitle() )
	{
		const TitleBaseServer * base = GetMainTitle();

		for( int i = 0; i < TitleBase::MAX_OPTION_NUMBER; i++ )
		{
			if( !base->nOptType[i] ) continue;

			if( base->nOptType[i] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )
			{
				ampParameter( base->fOptVar1[i], base->fOptVar2[i], true );
			}

			_amplifyStatByEffect( base->pvEffectList );
		}
	}

	// 부 호칭
	for( int i = 0; i < GameRule::SUB_TITLE_COUNT; i++ )
	{
		const TitleBaseServer * base = GetSubTitle( i );

		if( !base )
			continue;

		for( int x = 0; x < TitleBase::MAX_OPTION_NUMBER; x++ )
		{
			if( !base->nOptType[x] ) continue;

			// 부 호칭으로 사용 시에는 성능의 10%만을 적용한다.
			if( base->nOptType[x] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )
			{
				ampParameter( base->fOptVar1[x], base->fOptVar2[x] * GameRule::SUB_TITLE_RATE, true );
			}
		}
	}
}


void StructPlayer::applyTitleEffect()
{
	// 주 호칭
	if( GetMainTitle() )
	{
		const TitleBaseServer * base = GetMainTitle();

		for( int i = 0; i < TitleBase::MAX_OPTION_NUMBER; i++ )
		{
			if( !base->nOptType[i] ) continue;

			onItemWearEffect( NULL, false, base->nOptType[i], base->fOptVar1[i], base->fOptVar2[i], 1.0f );
		}

		_applyEffect( NULL, base->pvEffectList, 1.0f );
	}

	// 부 호칭
	for( int i = 0; i < GameRule::SUB_TITLE_COUNT; i++ )
	{
		const TitleBaseServer * base = GetSubTitle( i );

		if( !base )
			continue;

		for( int x = 0; x < TitleBase::MAX_OPTION_NUMBER; x++ )
		{
			if( !base->nOptType[x] ) continue;

			onItemWearEffect( NULL, false, base->nOptType[i], base->fOptVar1[i], base->fOptVar2[i], GameRule::SUB_TITLE_RATE );
		}
	}
}

// --- 기타 예외적인 처리 ---
void StructCreature::applyDoubleWeaponEffect()
{
	// T.T 눈물을 머금고 예외처리.
	StructSkill* pSkill = NULL;

	if( IsUsingDoubleWeapon() )
	{

		int nExpertSkillLevel = 1;

		if( GetWearedItem( ItemBase::WEAR_LEFTHAND )->GetItemClass() == ItemBase::CLASS_ONEHAND_SWORD &&
			GetWearedItem( ItemBase::WEAR_RIGHTHAND )->GetItemClass() == ItemBase::CLASS_ONEHAND_SWORD )		// 한손검 이도류
		{
			pSkill = GetCurrentPassiveSkill( StructSkill::SKILL_DUAL_SWORD_EXPERT );

			if( !pSkill )
			{
				pSkill = GetCurrentPassiveSkill( StructSkill::SKILL_TWIN_BLADE_EXPERT );
			}

			if( pSkill )
			{
				nExpertSkillLevel = pSkill->GetCurrentSkillLevel();
			}
		}
		else if( GetWearedItem( ItemBase::WEAR_LEFTHAND )->GetItemClass() == ItemBase::CLASS_DAGGER &&
			GetWearedItem( ItemBase::WEAR_RIGHTHAND )->GetItemClass() == ItemBase::CLASS_DAGGER )		// 쌍단검
		{
			pSkill = GetCurrentPassiveSkill( StructSkill::SKILL_TWIN_BLADE_EXPERT );

			if( pSkill )
			{
				nExpertSkillLevel = pSkill->GetCurrentSkillLevel();
			}
		}
		else if( GetWearedItem( ItemBase::WEAR_LEFTHAND )->GetItemClass() == ItemBase::CLASS_ONEHAND_AXE &&
			GetWearedItem( ItemBase::WEAR_RIGHTHAND )->GetItemClass() == ItemBase::CLASS_ONEHAND_AXE )		// 쌍도끼
		{
			pSkill = GetCurrentPassiveSkill( StructSkill::SKILL_TWIN_AXE_EXPERT );

			if( pSkill )
			{
				nExpertSkillLevel = pSkill->GetCurrentSkillLevel();
			}
		}

		m_nDoubleWeaponMasteryLevel = nExpertSkillLevel;

		c_fixed10 fBase, fAddPerLevel;
		fBase.set( 7500 );
		fAddPerLevel.set( 50 );
		m_Attribute.fAttackSpeedRight			= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_Attribute.fAttackSpeedRight;
		m_Attribute.fAttackSpeedLeft			= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_Attribute.fAttackSpeedLeft;
		m_Attribute.fAttackSpeed				= ( m_Attribute.fAttackSpeedRight + m_Attribute.fAttackSpeedLeft ) * 0.5f;
		m_AttributeByState.fAttackSpeedRight	= ( c_fixed10( 0.75f ) + c_fixed10( 0.005f ) * nExpertSkillLevel ) * m_AttributeByState.fAttackSpeedRight;
		m_AttributeByState.fAttackSpeedLeft		= ( c_fixed10( 0.75f ) + c_fixed10( 0.005f ) * nExpertSkillLevel ) * m_AttributeByState.fAttackSpeedLeft;
		m_AttributeByState.fAttackSpeed			= ( m_AttributeByState.fAttackSpeedRight + m_AttributeByState.fAttackSpeedLeft ) * 0.5f;

		fBase.set( 8900 );
		fAddPerLevel.set( 100 );
		m_Attribute.fAccuracyRight				= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_Attribute.fAccuracyRight;
		m_AttributeByState.fAccuracyRight		= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_AttributeByState.fAccuracyRight;
		m_Attribute.fAccuracyLeft				= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_Attribute.fAccuracyLeft;
		m_AttributeByState.fAccuracyLeft		= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_AttributeByState.fAccuracyLeft;

		fBase.set( 9000 );
		fAddPerLevel.set( 100 );
		m_Attribute.fAttackPointRight			= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_Attribute.fAttackPointRight;
		m_AttributeByState.fAttackPointRight	= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_AttributeByState.fAttackPointRight;
		m_fAttackPointRightWithoutWeapon		= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_fAttackPointRightWithoutWeapon;
		fBase.set( 4400 );
		fAddPerLevel.set( 200 );
		m_Attribute.fAttackPointLeft			= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_Attribute.fAttackPointLeft;
		m_AttributeByState.fAttackPointLeft		= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_AttributeByState.fAttackPointLeft;
		m_fAttackPointLeftWithoutWeapon			= ( fBase + fAddPerLevel * nExpertSkillLevel ) * m_fAttackPointLeftWithoutWeapon;
	}
	else
	{
		m_Attribute.fAttackSpeed				= m_Attribute.fAttackSpeedRight;
		m_AttributeByState.fAttackSpeed			= m_AttributeByState.fAttackSpeedRight;
	}
}

void StructCreature::applyJobLevelBonus()
{
}

void StructPlayer::applyJobLevelBonus()
{
	if( !GetJobId() )
		return;

	int depth = GetJobDepth();
	int jobs[GameRule::MAX_JOB_DEPTH];
	int levels[GameRule::MAX_JOB_DEPTH];

	int i;

	for( i = 0; i < depth; ++i )
	{
		jobs[i] = GetPrevJobId( i );
		levels[i] = GetPrevJobLevel( i );
	}

	jobs[i] = GetJobId();
	levels[i] = GetJobLevel();


	CreatureStat stat = GameContent::GetJobLevelBonus( depth, jobs, levels );

	m_Stat.strength += stat.strength;
	m_Stat.vital += stat.vital;
	m_Stat.dexterity += stat.dexterity;
	m_Stat.agility += stat.agility;
	m_Stat.intelligence += stat.intelligence;
	m_Stat.mentality += stat.mentality;
	m_Stat.luck += stat.luck;
}

void StructSummon::applyJobLevelBonus()
{
	CreatureStat stat = GameContent::GetSummonLevelBonus( GetSummonCode(), GetTransformLevel(), GetLevel() );

	m_Stat.strength += stat.strength;
	m_Stat.vital += stat.vital;
	m_Stat.dexterity += stat.dexterity;
	m_Stat.agility += stat.agility;
	m_Stat.intelligence += stat.intelligence;
	m_Stat.mentality += stat.mentality;
	m_Stat.luck += stat.luck;
}

void StructCreature::synchronizeParameter( const int bitset, const c_fixed10 & rate )
{
	if( bitset == 0 ) return;
	std::vector< std::pair< int, std::pair< c_fixed10, int > > >::iterator it;

	for( it = m_vSynchronizedParameter.begin(); it != m_vSynchronizedParameter.end(); it++ )
	{
		int nBitset = (*it).first;
		if( ( nBitset & bitset ) != 0 )
		{
			c_fixed10 prevRate = (*it).second.first;
			int count = (*it).second.second;

			(*it).first |= bitset;
			(*it).second.first = ( prevRate * count + rate ) / ( count + 1 );
			(*it).second.second++;

			return;
		}
	}

	m_vSynchronizedParameter.push_back( std::make_pair( bitset, std::make_pair( rate, 1 ) ) );
}



void StructCreature::addSkill( const int id, const int added_level, const int skill_type, const bool bRace )
{
	if( id == 0 || added_level == 0 ) return;

	std::vector< std::pair< int, int > >::iterator it;

	if( bRace )
	{
		int race_id = id;

		// 적용 대상이 모든 스킬이거나 종족이 해당된다면 스킬을 증가시킨다.
		if( race_id & StructState::FLAG_ALL
			|| ( (race_id & StructState::FLAG_ASURA) && (GetRace() == JobInfo::ASURA) )
			|| ( (race_id & StructState::FLAG_DEVA) && (GetRace() == JobInfo::DEVA) )
			|| ( (race_id & StructState::FLAG_GAIA) && (GetRace() == JobInfo::GAIA) ) )
		{
			for( it = m_vAddedSkillBySkillType.begin(); it != m_vAddedSkillBySkillType.end(); it++ )
				if( (*it).first == skill_type )
					break;

			if( it == m_vAddedSkillBySkillType.end() )
				m_vAddedSkillBySkillType.push_back( std::make_pair( skill_type, added_level ) );
			else
				(*it).second += added_level;
		}
	}
	else
	{
		// 양수일 경우에는 스킬 ID
		if( id > 0 )
		{
			int skill_id = id;

			for( it = m_vAddedSkillBySkillId.begin(); it != m_vAddedSkillBySkillId.end(); it++ )
				if( (*it).first == skill_id )
					break;

			if( it == m_vAddedSkillBySkillId.end() )
				m_vAddedSkillBySkillId.push_back( std::make_pair( skill_id, added_level ) );
			else
				(*it).second += added_level;
		}
		// 음수일 경우 직업 ID
		else if( id < 0 )
		{
			int job_id = -id;

			// 자신의 직업이나 하위 직업 효과라면 적용
			for( int j = 0; j <= GetJobDepth(); j++ )
			{
				int creatureJobId = GetPrevJobId( j );
				if( !creatureJobId ) creatureJobId = GetJobId();

				if( job_id == creatureJobId )
				{
					for( it = m_vAddedSkillBySkillType.begin(); it != m_vAddedSkillBySkillType.end(); it++ )
						if( (*it).first == skill_type )
							break;

					if( it == m_vAddedSkillBySkillType.end() )
						m_vAddedSkillBySkillType.push_back( std::make_pair( skill_type, added_level ) );
					else
						(*it).second += added_level;

					break;
				}
			}
		}
	}
}

void StructCreature::checkAdditionalItemEffect()
{
	// 세트 아이템, 세트 소울스톤의 세트 착용 정보 수집 맵
	SetPartFlagMap mapSetPartFlag;

	for( int nWearIndex = ItemBase::WEAR_WEAPON ; nWearIndex < ItemBase::MAX_SPARE_ITEM_WEAR; ++nWearIndex )
	{
		StructItem * pEquipment = m_anWear[nWearIndex];

		if( !pEquipment ) continue;

		// 세트 아이템 착용 정보 수집
		if( nWearIndex >= ItemBase::WEAR_WEAPON && nWearIndex < ItemBase::MAX_SPARE_ITEM_WEAR)
		{
			if( pEquipment && pEquipment->GetSetItemID() )
			{
				if( mapSetPartFlag.empty() )
				{
					mapSetPartFlag.insert( std::make_pair( pEquipment->GetSetItemID(), pEquipment->GetSetItemPartFlag() ) );
				}
				else 
				{
					SetPartFlagMap::iterator itSetPartFlag = mapSetPartFlag.find( pEquipment->GetSetItemID() );
					if( itSetPartFlag == mapSetPartFlag.end() )
					{
						mapSetPartFlag.insert( std::make_pair( pEquipment->GetSetItemID(), pEquipment->GetSetItemPartFlag() ) );
					}
					else
					{
						(*itSetPartFlag).second |= pEquipment->GetSetItemPartFlag();
					}
				}
			}
		}

		// 세트 소울스톤 착용 정보 수집
		for( int nSocketIndex = 0 ; nSocketIndex < pEquipment->GetMaxSocketCount() ; ++nSocketIndex )
		{
			ItemBase::ItemCode nSocketCode = pEquipment->GetSocketCode( nSocketIndex );

			if( !nSocketCode )
				continue;

			const ItemBase & SocketBase = StructItem::GetItemBase( nSocketCode );

			// 레벨 제한 적용 가능 검사
			if( GetLevel() < SocketBase.nMinLevel || GetLevel() > SocketBase.nMaxLevel )
				continue;

			// 소켓 내구도 검사
			if( !pEquipment->GetCurrentEndurance() )
				continue;

			if( SocketBase.nSetID )
			{
				if( mapSetPartFlag.empty() )
				{
					mapSetPartFlag.insert( std::make_pair( SocketBase.nSetID, SocketBase.nSetPartFlag ) );
					continue;
				}
				else 
				{
					SetPartFlagMap::iterator itSetPartFlag = mapSetPartFlag.find( SocketBase.nSetID );
					if( itSetPartFlag == mapSetPartFlag.end() )
					{
						mapSetPartFlag.insert( std::make_pair( SocketBase.nSetID, SocketBase.nSetPartFlag ) );
					}
					else
					{
						(*itSetPartFlag).second |= SocketBase.nSetPartFlag;
						continue;
					}
				}
			}
		}

		// 착용 무기 속성 이펙트 추가 성능 적용
		if( pEquipment->IsWeapon() && pEquipment->GetElementalEffectType() )
		{
			if( nWearIndex == ItemBase::WEAR_WEAPON )
				m_nElementalEffectAttackPointRight += pEquipment->GetElementalEffectAttackPoint();
			else
				m_nElementalEffectAttackPointLeft += pEquipment->GetElementalEffectAttackPoint();

			m_nElementalEffectMagicPoint += pEquipment->GetElementalEffectMagicPoint();
		}
	}

	// 세트 아이템과 세트 소울스톤 세트 착용 정보에 따라 적용될 세트 효과를 추후 스텟/파라미터 계산에 사용하기 위해 m_vSetItemEffect에 보관
	for( SetPartFlagMap::const_iterator it = mapSetPartFlag.begin() ; it != mapSetPartFlag.end() ; ++it )
	{
		const std::vector< SetItemEffectInfo * > * pvSetItemEffectInfo = GameContent::GetSetItemEffectInfoVector( (*it).first );

		for( std::vector< SetItemEffectInfo * >::const_iterator itSetItemEffect = pvSetItemEffectInfo->begin() ; itSetItemEffect != pvSetItemEffectInfo->end() ; ++itSetItemEffect )
		{
			if( ( (*it).second & (*itSetItemEffect)->nSetPartID ) == (*itSetItemEffect)->nSetPartID )
			{
				m_vSetItemEffect.push_back( *itSetItemEffect );
			}
		}
	}
}




// --- 이하 핸들러들 ---
void StructCreature::onBeforeCalculateStat()
{
}

void StructPlayer::onBeforeCalculateStat()
{
	m_bIsSummonable = true;
	m_bIsInfiniteSummonTime = false;

	m_nRideModifier = 0;
	m_nSpeedModifier = 0;
	m_nMaxWeightModifier = 0;
	m_fPassDamageRatio = 0.0f;
	m_bStaminaSave = false;
	m_bSuperSave = false;
	m_eSummonStaminaSaveType = SUMMON_STAMINA_SAVE_NONE;
	m_nMaxStamina = GameRule::DEFAULT_MAX_STAMINA;
	m_bUsingTent = false;
	m_nStaminaRegenBonus = 0;
	m_nMaxChaos = 0;
	m_nDoubleSummonTime = GameRule::DEFAULT_DOUBLE_SUMMON_TIME;
	m_vAutoRecoverInfo.clear();
	m_nRidingStateCode = static_cast< StructState::StateCode >( 0 );
	m_bUseFasterSpeedInRiding = false;

	m_fActiveSummonExpAmp = 1.0f;
	m_fDeactiveSummonExpAmp = 0.0f;

	// 무기 사거리 조정
	StructItem * pWeapon = GetWearedItem( ItemBase::WEAR_WEAPON );
	if( pWeapon )
		m_Attribute.fAttackRange	= pWeapon->GetItemAttackRange();

	m_vSummonPassiveSkillList.clear();
	m_vAmplifySummonPassiveSkillList.clear();

	// 벨트 슬롯 결정
	m_nBeltSlotMax = 0;
	m_fBeltSlotAmp = 0;
	StructItem *pItem = m_anWear[ItemBase::WEAR_BELT];
	if( pItem && IsWearable( ItemBase::WEAR_BELT, pItem ) )
	{
		const ItemBaseServer & base = pItem->GetItemBase();

		for( int i = 0; i < ItemBase::MAX_OPTION_NUMBER; ++i )
		{
			if( base.nOptType[i] == ITEM_EFFECT_PASSIVE::BELT_SLOT )
				m_nBeltSlotMax += base.fOptVar1[i];
		}

		// 벨트는 세공이 불가능한 대신 소켓 정보에 벨트 슬롯 정보를 담고 있다.
		m_nBeltSlotMax += pItem->GetSocketCode( 0 );

		if (pItem->IsAwaken())
		{
			for (int i = 0; i < ItemInstance::MAX_RANDOM_OPTION_NUMBER; ++i)
			{
				if (pItem->GetAwakenOptionType(i) == 0)
					break;
				if (pItem->GetAwakenOptionType(i) == 145)
				{
					this->m_fBeltSlotAmp += (pItem->GetAwakenOptionValue2(i) / 100);
				}
			}
		}
	}

	m_nBeltSlotMax = m_nBeltSlotMax > 8 ? 8 : m_nBeltSlotMax;


	s_memcpy( &m_PrevParameterForSummon, sizeof( m_PrevParameterForSummon ), &m_ParameterForSummon, sizeof( m_PrevParameterForSummon ) );

	memset( &m_ParameterForSummon.Stat, 0, sizeof( m_ParameterForSummon.Stat ) );
	memset( &m_ParameterForSummon.Attribute, 0, sizeof( m_ParameterForSummon.Attribute ) );
	memset( &m_ParameterForSummon.Resist, 0, sizeof( m_ParameterForSummon.Resist ) );

	m_ParameterForSummon.nMaxHP = 0;
	m_ParameterForSummon.nMaxMP = 0;
	m_ParameterForSummon.nMaxSP = 0;
	m_ParameterForSummon.nSPRegenAdd = 0;

	m_ParameterForSummon.fAttackPointRightWithoutWeapon = 0.0f;
	m_ParameterForSummon.fMoveSpeed = 0.0f;
	m_ParameterForSummon.fAttackRange = 0.0f;

	m_ParameterForSummon.fMaxHPAmplifier = 0.0f;
	m_ParameterForSummon.fMaxMPAmplifier = 0.0f;
	m_ParameterForSummon.fMaxSPAmplifier = 0.0f;
	m_ParameterForSummon.fSPRegenAddAmplifier = 0.0f;

	memset( &m_ParameterForSummon.AttributeAmplifier, 0, sizeof( m_ParameterForSummon.AttributeAmplifier ) );
}

void StructMonster::onBeforeCalculateStat()
{
	// (2007-03-24 리뉴얼 적용)
	if( !isMoveSpeedFixed() )
	{
		m_Attribute.fMoveSpeed		+= ( m_pContentInfo->run_speed - GameRule::GetBaseMoveSpeed() );
	}
	m_Attribute.fAttackRange	= m_pContentInfo->attack_range;
	m_fDetectHideRange			= m_pContentInfo->hidesense_range;

	// 지속효과 도발에 의한 헤이트 보정치 벡터 초기화
	m_vHateModifierByState.clear();

	c_fixed10 difficultyBonus = GameRule::GetDifficultyBonus( m_nDifficulty );
	m_Stat.strength *= difficultyBonus;
	m_Stat.vital *= difficultyBonus;
	m_Stat.dexterity *= difficultyBonus;
	m_Stat.agility *= difficultyBonus;
	m_Stat.intelligence *= difficultyBonus;
	m_Stat.mentality *= difficultyBonus;
	m_Stat.luck *= difficultyBonus;

	if( m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_IMMORTAL_DUMMY )
		m_StatusFlag.Off( STATUS_MORTAL );
}

void StructSummon::onBeforeCalculateStat()
{
	// { 스탯 초기화
	m_fSoulConnectionRatio = 0.0f;
	m_fRideSpeedMod = 0.9f;
	m_fCM.set( 7000 );
	m_fHPRegenMod = 1.0f;
	m_fMPRegenMod = 1.0f;
	m_fMaxSPAmplifier = 0.0f;
	m_fSPRegenAddAmplifier = 0.0f;

	m_fBaseAttackPointRatio = 0;
	m_fBaseMagicPointRatio = 0;
	m_fBaseDefenceRatio = 0;
	m_fBaseMagicDefenceRatio = 0;

	short nEnhance = 0;
	if( GetParentCard() )
		nEnhance = GetParentCard()->GetItemEnhance();
	float fAmp = 1.0f;
	fAmp += GameContent::GetCreatureEnhanceInfo( nEnhance )->stat_amplify;
	m_Stat.strength *= fAmp;
	m_Stat.vital *= fAmp;
	m_Stat.dexterity *= fAmp;
	m_Stat.agility *= fAmp;
	m_Stat.intelligence *= fAmp;
	m_Stat.mentality *= fAmp;
	m_Stat.luck *= fAmp;

	const StructSkill *pItemExpert = GetSkillByEffectType( SkillBase::EF_SUMMON_ITEM_EXPERT );
	if( pItemExpert && pItemExpert->GetCurrentSkillLevel() > 0 )
	{
		m_fBaseAttackPointRatio = pItemExpert->GetVar( 0 );
		m_fBaseMagicPointRatio = pItemExpert->GetVar( 1 );
		m_fBaseDefenceRatio = pItemExpert->GetVar( 2 );
		m_fBaseMagicDefenceRatio = pItemExpert->GetVar( 3 );
	}

	m_nMaxItemWear = GameContent::GetCreatureEnhanceInfo( nEnhance )->slot_amount;
	//if (this->GetSP() > 0)
		//m_nMaxItemWear = 7 + GetSP();
	m_fItemMod = 1.0f;
	m_vMasterAmplifyPassiveSkillList.clear();
	m_bIsRidable = false;
	m_nUnMountProbabilityOnDamage = GameRule::DEFAULT_UNMOUNT_PROBABILITY_ON_DAMAGE;
	m_nUnMountProbabilityOnCriticalDamage = GameRule::DEFAULT_UNMOUNT_PROBABILITY_ON_CRITICAL_DAMAGE;
	// }

	const StructSkill *pSkill;

	if( GetMaster() )
	{
		// { 크리쳐 마스터리 적용	(2007-03-24 리뉴얼 적용)
		pSkill = GetMaster()->GetCurrentPassiveSkill( StructSkill::SKILL_CREATURE_MASTERY );
		static const c_fixed10 fCMPerLevel( 300, true );
		if( pSkill ) m_fCM += fCMPerLevel * pSkill->GetCurrentSkillLevel();
		// }
	}

	m_nSPRegenAdd = ( GetFCM() * 25 ) + 25;
	m_nSPAdd = 0;

	if( GetMaster() )
	{
		// { 크리처 아바타 레벨차 보정
		// 오버브리드 페널티(오버Lv+크리처Lv-아바타Lv > 10 일 경우 부여되는 페널티) 제거. 2007-04-04
		m_fBattleLevel = GetMaster()->GetLevel() + GameRule::GetSummonLevelPenalty( GetMaster()->GetLevel(), GetLevel() );
		// }
	}
	else
	{
		m_fBattleLevel = GetLevel();
	}

	// 소환수 이속 보정. 상태이상 적용을 위해서 onCompleteCalculateStat()에 있던걸 이쪽으로 옮겨왔음 2006-09-08 by Synastry
	// (2007-03-24 리뉴얼 적용)
	if( !isMoveSpeedFixed() )
		m_Attribute.fMoveSpeed		+= ( m_pContentInfo->run_speed - GameRule::GetBaseMoveSpeed() );

	// 소환수의 공격거리는 DB에 정의된 거리를 따른다
	m_Attribute.fAttackRange	 = m_pContentInfo->attack_range;
}

void StructNPC::onBeforeCalculateStat()
{
	// (2007-03-24 리뉴얼 적용)
	if( !isMoveSpeedFixed() )
	{
		m_Attribute.fMoveSpeed		+= ( m_pBaseInfo->run_speed - GameRule::GetBaseMoveSpeed() );
	}
	m_Attribute.fAttackRange	 = m_pBaseInfo->attack_range;
}

void StructCreature::onAfterApplyStat()
{
}

void StructSummon::onAfterApplyStat()
{
	// { 레벨 별 스텟 페널티 적용
	float stat_penalty = 1.0f;
	
	if( GetMaster() )
		stat_penalty = GameRule::GetSummonStatPenalty( GetMaster()->GetLevel(), GetLevel() );

	m_Stat.strength		*= stat_penalty;
	m_Stat.vital		*= stat_penalty;
	m_Stat.dexterity	*= stat_penalty;
	m_Stat.agility		*= stat_penalty;
	m_Stat.intelligence	*= stat_penalty;
	m_Stat.mentality	*= stat_penalty;
	m_Stat.luck			*= stat_penalty;
	// }

}


void StructCreature::onApplyStat()
{
}

void StructPlayer::onApplyStat()
{
	struct _mySkillFunctor : StructCreature::SKILL_POINTER_FUNCTOR
	{
		_mySkillFunctor( StructPlayer * _pPlayer ) : pPlayer( _pPlayer )
		{
			memset( amplifierCount, 0, sizeof( amplifierCount ) );
			for( int i = 0; i < 7; ++i ) statAmplifier[i] = 0;
		}

		virtual void onSkill( StructSkill* pSkill )
		{
			// 문제가 있어 사용을 하지 않는다. 벨트 리뉴얼 기획을 따르지 않음.
			if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_BELT_ON_PARAMETER_INC )
			{
				pPlayer->m_Stat.strength += pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();
				pPlayer->m_Stat.vital += pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
				pPlayer->m_Stat.agility += pSkill->GetVar(4) + pSkill->GetVar(5) * pSkill->GetCurrentSkillLevel();
				pPlayer->m_Stat.dexterity += pSkill->GetVar(6) + pSkill->GetVar(7) * pSkill->GetCurrentSkillLevel();
				pPlayer->m_Stat.intelligence += pSkill->GetVar(8) + pSkill->GetVar(9) * pSkill->GetCurrentSkillLevel();
				pPlayer->m_Stat.mentality += pSkill->GetVar(10) + pSkill->GetVar(11) * pSkill->GetCurrentSkillLevel();
				pPlayer->m_Stat.luck += pSkill->GetVar(12) + pSkill->GetVar(13) * pSkill->GetCurrentSkillLevel();
			}
			else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_BELT_ON_PARAMETER_AMP )
			{
				float amp;

				for( int i = 0; i < 7; ++i )
				{
					amp = pSkill->GetVar(i*2) + pSkill->GetVar(i*2+1) * pSkill->GetCurrentSkillLevel();
					if( amp )
					{
						statAmplifier[i] += amp;
						++amplifierCount[i];
					}

				}
			}
		}

		StructPlayer * pPlayer;
		c_fixed10 statAmplifier[7];
		int amplifierCount[7];
	} _fo( this );

	for( int i = 0; i < m_nBeltSlotMax; ++i )
	{
		StructItem * pItem = m_aBeltSlotCard[i];

		if( !pItem )
			continue;

		if( pItem->IsSummonCard() )
		{
			StructSummon * pSummon = pItem->GetSummonStruct();

			if( !pSummon )
				continue;

			pSummon->EnumPassiveSkill( _fo );
		}
		else if( pItem->IsEquipmentOnBelt() )
		{
			// 내구도 체크
			if( !pItem->IsWearable() ) continue;

			// 레벨 체크
			if( pItem->GetLevelLimit() > GetLevel() ) continue;
			if( pItem->GetItemBase().nMinLevel && GetLevel() < pItem->GetItemBase().nMinLevel ) continue;
			if( pItem->GetItemBase().nMaxLevel && GetLevel() > pItem->GetItemBase().nMaxLevel ) continue;

			const ItemBaseServer & base = pItem->GetItemBase();

			for( int x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
			{
				if( !base.nOptType[x] ) continue;

				if( base.nOptType[x] == ITEM_EFFECT_PASSIVE::INC_PARAMETER_A )
				{
					incParameter( base.fOptVar1[x], base.fOptVar2[x], true );
				}
				else if( base.nOptType[x] == ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A )
				{
					ampParameter( base.fOptVar1[x], base.fOptVar2[x], true );
				}
			}

			// 스킬의 변화도 스탯에 영향을 미치므로 함께 변경한다.
			_applyStatByEffect( base.pvEffectList, m_fBeltSlotAmp + 1);
			_applySkillByEffect( base.pvEffectList );
			_amplifyStatByEffect( base.pvEffectList );
		}
	}

	for( int i = 0; i < 7; ++i )
	{
		float fAdd = (m_fBeltSlotAmp / 10.0) + 0.3;
		if( _fo.statAmplifier[i] > fAdd ) _fo.statAmplifier[i] = fAdd;
		if( _fo.amplifierCount[i] <= 1 ) _fo.amplifierCount[i] = 0;
	}

	m_StatAmplifier.strength		+= _fo.statAmplifier[0] + _fo.amplifierCount[0] * 0.01;
	m_StatAmplifier.vital			+= _fo.statAmplifier[1] + _fo.amplifierCount[1] * 0.01;
	m_StatAmplifier.agility			+= _fo.statAmplifier[2] + _fo.amplifierCount[2] * 0.01;
	m_StatAmplifier.dexterity		+= _fo.statAmplifier[3] + _fo.amplifierCount[3] * 0.01;
	m_StatAmplifier.intelligence	+= _fo.statAmplifier[4] + _fo.amplifierCount[4] * 0.01;
	m_StatAmplifier.mentality		+= _fo.statAmplifier[5] + _fo.amplifierCount[5] * 0.01;
	m_StatAmplifier.luck			+= _fo.statAmplifier[6] + _fo.amplifierCount[6] * 0.01;
}

void StructSummon::onApplyStat()
{
	struct _mySkillFunctor : StructCreature::SKILL_POINTER_FUNCTOR
	{
		_mySkillFunctor( StructSummon * _pSummon ) : pSummon( _pSummon ) {}

		virtual void onSkill( StructSkill* pSkill )
		{
			if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_INCREASE_STAT )
			{
				int nSkillLevel = pSkill->GetCurrentSkillLevel();

				pSummon->m_Stat.strength					+= pSkill->GetVar( 0 ) + pSkill->GetVar( 1 ) * nSkillLevel;
				pSummon->m_Stat.vital						+= pSkill->GetVar( 2 ) + pSkill->GetVar( 3 ) * nSkillLevel;
				pSummon->m_Stat.agility						+= pSkill->GetVar( 4 ) + pSkill->GetVar( 5 ) * nSkillLevel;
				pSummon->m_Stat.dexterity					+= pSkill->GetVar( 6 ) + pSkill->GetVar( 7 ) * nSkillLevel;
				pSummon->m_Stat.intelligence				+= pSkill->GetVar( 8 ) + pSkill->GetVar( 9 ) * nSkillLevel;
				pSummon->m_Stat.mentality					+= pSkill->GetVar( 10 ) + pSkill->GetVar( 11 ) * nSkillLevel;
				pSummon->m_Stat.luck						+= pSkill->GetVar( 12 ) + pSkill->GetVar( 13 ) * nSkillLevel;
			}
			else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_AMPLIFY_STAT )
			{
				int nSkillLevel = pSkill->GetCurrentSkillLevel();

				pSummon->m_StatAmplifier.strength			+= pSkill->GetVar( 0 ) * nSkillLevel;
				pSummon->m_StatAmplifier.vital				+= pSkill->GetVar( 1 ) * nSkillLevel;
				pSummon->m_StatAmplifier.agility			+= pSkill->GetVar( 2 ) * nSkillLevel;
				pSummon->m_StatAmplifier.dexterity			+= pSkill->GetVar( 3 ) * nSkillLevel;
				pSummon->m_StatAmplifier.intelligence		+= pSkill->GetVar( 4 ) * nSkillLevel;
				pSummon->m_StatAmplifier.mentality			+= pSkill->GetVar( 5 ) * nSkillLevel;
				pSummon->m_StatAmplifier.luck				+= pSkill->GetVar( 6 ) * nSkillLevel;
			}
		}

		StructSummon * pSummon;
	} _fo( this );

	EnumPassiveSkill( _fo );
}

void StructCreature::onAfterCalculateAttributeByStat()
{
}

void StructPlayer::onAfterCalculateAttributeByStat()
{
	// 벨트 장착 카드 속성(스텟 아님) 증가 처리
	if( IsPlayer() )
	{
		struct _mySkillFunctor : StructCreature::SKILL_POINTER_FUNCTOR
		{
			_mySkillFunctor( StructPlayer * _pPlayer ) : pPlayer( _pPlayer )
			{
				for( int i = 0; i < 22; ++i ) attributeAmplifier[22] = 0;
				fMaxHPAmplifier = 0.0;
				fMaxMPAmplifier = 0.0;
				memset( amplifierCount, 0, sizeof( amplifierCount ) );
			}

			virtual void onSkill( StructSkill* pSkill )
			{
				int nSkillLevel = pSkill->GetCurrentSkillLevel();

				// 문제가 있어 사용을 하지 않는다. 벨트 리뉴얼 기획을 따르지 않음.
				if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_BELT_ON_ATTRIBUTE_INC )
				{
					pPlayer->m_Attribute.fAttackPointRight += pSkill->GetVar(0) + pSkill->GetVar(1) * nSkillLevel;
					pPlayer->m_Attribute.fDefence += pSkill->GetVar(2) + pSkill->GetVar(3) * nSkillLevel;
					pPlayer->m_Attribute.fMagicPoint += pSkill->GetVar(4) + pSkill->GetVar(5) * nSkillLevel;
					pPlayer->m_Attribute.fMagicDefence += pSkill->GetVar(6) + pSkill->GetVar(7) * nSkillLevel;

					pPlayer->m_Attribute.fAttackSpeed += pSkill->GetVar(8) + pSkill->GetVar(9) * nSkillLevel;
					pPlayer->m_Attribute.fAttackSpeed += pSkill->GetVar(8) + pSkill->GetVar(9) * nSkillLevel;
					if( pPlayer->IsUsingDoubleWeapon() )
						pPlayer->m_Attribute.fAttackSpeed += pSkill->GetVar(8) + pSkill->GetVar(9) * nSkillLevel;

					if( !pPlayer->isMoveSpeedFixed() ) // fRidingSpeed 수치가 생기면서 라이딩 중에도 기본 속도에 관여할 수 있게 변경 ( 2013-12-20 )
					{
						pPlayer->m_Attribute.fMoveSpeed += pSkill->GetVar(10) + pSkill->GetVar(11) * nSkillLevel;
					}

					pPlayer->m_Attribute.fAccuracyRight += pSkill->GetVar(12) + pSkill->GetVar(13) * nSkillLevel;
					if( pPlayer->IsUsingDoubleWeapon() )
						pPlayer->m_Attribute.fAccuracyLeft += pSkill->GetVar(12) + pSkill->GetVar(13) * nSkillLevel;

					pPlayer->m_Attribute.fMagicAccuracy += pSkill->GetVar(14) + pSkill->GetVar(15) * nSkillLevel;
					pPlayer->m_Attribute.fAvoid += pSkill->GetVar(16) + pSkill->GetVar(17) * nSkillLevel;
					pPlayer->m_Attribute.fMagicAvoid += pSkill->GetVar(18) + pSkill->GetVar(19) * nSkillLevel;
				}
				// 문제가 있어 사용을 하지 않는다. 벨트 리뉴얼 기획을 따르지 않음.
				else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_BELT_ON_ATTRIBUTE_EX_INC )
				{
					pPlayer->m_Attribute.fHPRegenPercentage += pSkill->GetVar(0) + pSkill->GetVar(1) * nSkillLevel;
					pPlayer->m_Attribute.fHPRegenPoint += pSkill->GetVar(2) + pSkill->GetVar(3) * nSkillLevel;
					pPlayer->m_Attribute.fMPRegenPercentage += pSkill->GetVar(4) + pSkill->GetVar(5) * nSkillLevel;
					pPlayer->m_Attribute.fMPRegenPoint += pSkill->GetVar(6) + pSkill->GetVar(7) * nSkillLevel;
					pPlayer->m_Attribute.fBlockChance += pSkill->GetVar(8) + pSkill->GetVar(9) * nSkillLevel;
					pPlayer->m_Attribute.fBlockDefence += pSkill->GetVar(10) + pSkill->GetVar(11) * nSkillLevel;
					pPlayer->m_Attribute.fCritical += pSkill->GetVar(12) + pSkill->GetVar(13) * nSkillLevel;
					pPlayer->m_Attribute.fCriticalPower += pSkill->GetVar(14) + pSkill->GetVar(15) * nSkillLevel;
					pPlayer->m_nMaxHP += pSkill->GetVar(16) + pSkill->GetVar(17) * nSkillLevel;
					pPlayer->m_nMaxMP += pSkill->GetVar(18) + pSkill->GetVar(19) * nSkillLevel;
				}
				// 문제가 있어 사용을 하지 않는다. 벨트 리뉴얼 기획을 따르지 않음.
				else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_BELT_ON_ATTRIBUTE_EX2_INC )
				{
					pPlayer->m_Attribute.fCastingSpeed += pSkill->GetVar(0) + pSkill->GetVar(1) * nSkillLevel;
					pPlayer->m_Attribute.fCoolTimeSpeed += pSkill->GetVar(2) + pSkill->GetVar(3) * nSkillLevel;
					// 캐스팅 보전 비구현
					// 화살 데미지 감소 비구현
				}
				else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_BELT_ON_ATTRIBUTE_AMP )
				{
					float amp;

					for( int i = 0; i < 10; ++i )
					{
						amp = pSkill->GetVar(i*2) + pSkill->GetVar(i*2+1) * nSkillLevel;
						if( amp )
						{
							attributeAmplifier[i] += amp;
							++amplifierCount[i];
						}
					}
				}
				else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_BELT_ON_ATTRIBUTE_EX_AMP )
				{
					float amp;

					for( int i = 0; i < 10; ++i )
					{
						amp = pSkill->GetVar(i*2) + pSkill->GetVar(i*2+1) * nSkillLevel;
						if( amp )
						{
							attributeAmplifier[i+10] += amp;
							++amplifierCount[i+10];
						}
					}
				}
				else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_BELT_ON_ATTRIBUTE_EX2_AMP )
				{
					float amp;

					for( int i = 0; i < 2; ++i )
					{
						amp = pSkill->GetVar(i*2) + pSkill->GetVar(i*2+1) * nSkillLevel;
						if( amp )
						{
							attributeAmplifier[i+20] += amp;
							++amplifierCount[i+20];
						}
					}
					// 캐스팅 보전 비구현
					// 화살 데미지 감소 비구현
				}
			}

			StructPlayer * pPlayer;
			c_fixed10 attributeAmplifier[22];
			float fMaxHPAmplifier;
			float fMaxMPAmplifier;
			int amplifierCount[22];
		} _fo( static_cast< StructPlayer * >(this) );

		for( int i = 0; i < m_nBeltSlotMax; ++i )
		{
			StructItem * pItem = m_aBeltSlotCard[i];

			if( !pItem )
				continue;

			if( pItem->IsSummonCard() )
			{
				StructSummon * pSummon = pItem->GetSummonStruct();

				if( !pSummon )
					continue;

				pSummon->EnumPassiveSkill( _fo );
			}
			else if( pItem->IsEquipmentOnBelt() )
			{
				// 내구도 체크
				if( !pItem->IsWearable() ) continue;

				// 레벨 체크
				if( pItem->GetLevelLimit() > GetLevel() ) continue;
				if( pItem->GetItemBase().nMinLevel && GetLevel() < pItem->GetItemBase().nMinLevel ) continue;
				if( pItem->GetItemBase().nMaxLevel && GetLevel() > pItem->GetItemBase().nMaxLevel ) continue;

				int x;
				float fItemRatio = m_fBeltSlotAmp + 1.0f;

				const ItemBaseServer & base = pItem->GetItemBase();
				
				for( int x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
				{
					if( !base.nBaseType[x] ) continue;

					onItemWearEffect( pItem, true, base.nBaseType[x], base.fBaseVar1[x], base.fBaseVar2[x], fItemRatio );
				}

				for( x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
				{
					if( !base.nOptType[x] ) continue;

					onItemWearEffect( pItem, false, base.nOptType[x], base.fOptVar1[x], base.fOptVar2[x], fItemRatio );
				}

				_applyEffect( pItem, base.pvEffectList, fItemRatio );
			}
		}

		for( int i = 0; i < 22; ++i )
		{
			float fAdd = (m_fBeltSlotAmp / 10.0) + 0.3;
			if( _fo.attributeAmplifier[i] > fAdd) _fo.attributeAmplifier[i] = fAdd;
			if( _fo.amplifierCount[i] <= 1 ) _fo.amplifierCount[i] = 0;
		}

		m_AttributeAmplifier.fAttackPointRight		+= _fo.attributeAmplifier[0] + _fo.amplifierCount[0] * 0.01;
		m_AttributeAmplifier.fDefence				+= _fo.attributeAmplifier[1] + _fo.amplifierCount[1] * 0.01;
		m_AttributeAmplifier.fMagicPoint			+= _fo.attributeAmplifier[2] + _fo.amplifierCount[2] * 0.01;
		m_AttributeAmplifier.fMagicDefence			+= _fo.attributeAmplifier[3] + _fo.amplifierCount[3] * 0.01;
		m_AttributeAmplifier.fAttackSpeed			+= _fo.attributeAmplifier[4] + _fo.amplifierCount[4] * 0.01;
		m_AttributeAmplifier.fAttackSpeedRight		+= _fo.attributeAmplifier[4] + _fo.amplifierCount[4] * 0.01;
		if( IsUsingDoubleWeapon() )
			m_AttributeAmplifier.fAttackSpeedLeft	+= _fo.attributeAmplifier[4] + _fo.amplifierCount[4] * 0.01;
		if( isMoveSpeedFixed() )	// fRidingSpeed 수치가 생기면서 라이딩 중에도 기본 속도에 관여할 수 있게 변경 ( 2013-12-20 )
			m_AttributeAmplifier.fMoveSpeed			+= _fo.attributeAmplifier[5] + _fo.amplifierCount[5] * 0.01;
		m_AttributeAmplifier.fAccuracyRight			+= _fo.attributeAmplifier[6] + _fo.amplifierCount[6] * 0.01;
		if( IsUsingDoubleWeapon() )
			m_AttributeAmplifier.fAccuracyLeft		+= _fo.attributeAmplifier[6] + _fo.amplifierCount[6] * 0.01;
		m_AttributeAmplifier.fMagicAccuracy			+= _fo.attributeAmplifier[7] + _fo.amplifierCount[7] * 0.01;
		m_AttributeAmplifier.fAvoid					+= _fo.attributeAmplifier[8] + _fo.amplifierCount[8] * 0.01;
		m_AttributeAmplifier.fMagicAvoid			+= _fo.attributeAmplifier[9] + _fo.amplifierCount[9] * 0.01;
		m_AttributeAmplifier.fHPRegenPercentage		+= _fo.attributeAmplifier[10] + _fo.amplifierCount[10] * 0.01;
		m_AttributeAmplifier.fHPRegenPoint			+= _fo.attributeAmplifier[11] + _fo.amplifierCount[11] * 0.01;
		m_AttributeAmplifier.fMPRegenPercentage		+= _fo.attributeAmplifier[12] + _fo.amplifierCount[12] * 0.01;
		m_AttributeAmplifier.fMPRegenPoint			+= _fo.attributeAmplifier[13] + _fo.amplifierCount[13] * 0.01;
		m_AttributeAmplifier.fBlockChance			+= _fo.attributeAmplifier[14] + _fo.amplifierCount[14] * 0.01;
		m_AttributeAmplifier.fBlockDefence			+= _fo.attributeAmplifier[15] + _fo.amplifierCount[15] * 0.01;
		m_AttributeAmplifier.fCritical				+= _fo.attributeAmplifier[16] + _fo.amplifierCount[16] * 0.01;
		m_AttributeAmplifier.fCriticalPower			+= _fo.attributeAmplifier[17] + _fo.amplifierCount[17] * 0.01;
		m_fMaxHPAmplifier							+= _fo.attributeAmplifier[18] + _fo.amplifierCount[18] * 0.01;
		m_fMaxMPAmplifier							+= _fo.attributeAmplifier[19] + _fo.amplifierCount[19] * 0.01;
		m_AttributeAmplifier.fCastingSpeed			+= _fo.attributeAmplifier[20] + _fo.amplifierCount[20] * 0.01;
		m_AttributeAmplifier.fCoolTimeSpeed			+= _fo.attributeAmplifier[21] + _fo.amplifierCount[21] * 0.01;
	}
}

void StructMonster::onApplyAttributeAdjustment()
{
	m_nMaxHP						+= m_pContentInfo->hp;
	m_nMaxMP						+= m_pContentInfo->mp;
	m_Attribute.fAttackPointRight	+= m_pContentInfo->attack_point;
	m_Attribute.fAttackPointLeft	+= 0;								// 왼손 공격력은 일단 무조건 0
	m_Attribute.fMagicPoint			+= m_pContentInfo->magic_point;
	m_Attribute.fDefence			+= m_pContentInfo->defence;
	m_Attribute.fMagicDefence		+= m_pContentInfo->magic_defence;
	m_Attribute.fAttackSpeedRight	+= m_pContentInfo->attack_speed;
	m_Attribute.fAttackSpeedLeft	+= m_pContentInfo->attack_speed;	// 왼손 공속은 오른손 공속 보정과 동일하게 적용
	m_Attribute.fCastingSpeed		+= m_pContentInfo->casting_speed;
	m_Attribute.fAccuracyRight		+= m_pContentInfo->accuracy;
	m_Attribute.fAccuracyLeft		+= m_pContentInfo->accuracy;		// 왼손 명중률은 오른손 명중률 보정과 동일하게 적용
	m_Attribute.fAvoid				+= m_pContentInfo->avoid;
	m_Attribute.fMagicAccuracy		+= m_pContentInfo->magic_accuracy;
	m_Attribute.fMagicAvoid			+= m_pContentInfo->magic_avoid;
}

void StructCreature::onApplyAttributeAdjustment()
{
}

void StructNPC::onApplyAttributeAdjustment()
{
	m_nMaxHP						+= m_pBaseInfo->hp;
	m_nMaxMP						+= m_pBaseInfo->mp;
	m_Attribute.fAttackPointRight	+= m_pBaseInfo->attack_point;
	m_Attribute.fAttackPointLeft	+= 0;							// 왼손 공격력은 일단 무조건 0
	m_Attribute.fMagicPoint			+= m_pBaseInfo->magic_point;
	m_Attribute.fDefence			+= m_pBaseInfo->defence;
	m_Attribute.fMagicDefence		+= m_pBaseInfo->magic_defence;
	m_Attribute.fAttackSpeedRight	+= m_pBaseInfo->attack_speed;
	m_Attribute.fAttackSpeedLeft	+= m_pBaseInfo->attack_speed;	// 왼손 공속은 오른손 공속 보정과 동일하게 적용
	m_Attribute.fAccuracyRight		+= m_pBaseInfo->accuracy;
	m_Attribute.fAccuracyLeft		+= m_pBaseInfo->accuracy;		// 왼손 명중률은 오른손 명중률 보정과 동일하게 적용
	m_Attribute.fAvoid				+= m_pBaseInfo->avoid;
	m_Attribute.fMagicAccuracy		+= m_pBaseInfo->magic_accuracy;
	m_Attribute.fMagicAvoid			+= m_pBaseInfo->magic_avoid;
}

void StructCreature::onModifyStatAndAttribute()
{
}

void StructPlayer::onModifyStatAndAttribute()
{
	//SendHPMPMsg( this, this );
	SendStatInfo( this, this );

	SendPropertyMessage( this, GetHandle(), "max_chaos", GetMaxChaos() );
	SendPropertyMessage( this, GetHandle(), "max_stamina", m_nMaxStamina );
}

void StructSummon::onModifyStatAndAttribute()
{
	if( GetMaster() )
	{
		//SendHPMPMsg( GetMaster(), this );
		SendStatInfo( GetMaster(), this );
		SendSPMsg( GetMaster(), this );
	}
}


void StructCreature::onModifySkill()
{
	if( this->IsPlayer() )
		SendAddedSkillMessage( static_cast< StructPlayer * >(this), this );
	else if( this->IsSummon() )
		SendAddedSkillMessage( static_cast< StructSummon * >(this)->GetMaster(), this );
}

void StructCreature::onModifyDetectRange()
{
	if( IsPlayer() )
		SendDetectRangeMessage( static_cast< StructPlayer * >(this), this );
	else if( IsSummon() )
		SendDetectRangeMessage( static_cast< StructSummon * >(this)->GetMaster(), this );
}

void StructPlayer::onCompleteCalculateStat()
{
	if( IsRiding() )
	{
		m_Attribute.fRidingSpeed = GetRideObject()->GetSummonBase()->riding_speed;
	}

	// 부적 적용
	for( std::vector< StructItem * >::iterator it = m_vCharmList.begin(); it != m_vCharmList.end(); ++it )
		applyCharm( *it );

	// 펫 적용
	StructPet * pPet = GetSummonedPet();
	if( pPet )
		m_Attribute.fMaxWeight += pPet->GetAddedWeight();

	// 야수 변신과 같은 이동 속도 고정류(MoveSpeedFixed)는 튼튼한 발굽 같은 RidingModifier의 보정을 받는다고 한다.
	if( !isMoveSpeedFixed() )
		m_Attribute.fMoveSpeed += m_nSpeedModifier;
	else
		m_Attribute.fMoveSpeed += m_nRideModifier;
	m_Attribute.fRidingSpeed += m_nRideModifier;
	m_Attribute.fMaxWeight += m_nMaxWeightModifier;

	if( memcmp( &m_ParameterForSummon, &m_PrevParameterForSummon, sizeof( m_ParameterForSummon ) ) )
	{
		onModifyParameterForSummon();
	}
}

void StructPlayer::onModifyParameterForSummon()
{
	StructSummon *pSummon = GetMainSummon();
	if( pSummon )
	{
		if( pSummon->GetPriority() == ArObject::UPDATE_PRIORITY_NULL ||
			pSummon->GetPriority() == ArObject::UPDATE_PRIORITY_IDLE )
		{
			ArcadiaServer::Instance().SetObjectPriority( pSummon, ArObject::UPDATE_PRIORITY_NORMAL );
		}
	}

	pSummon = GetSubSummon();
	if( pSummon )
	{
		if( pSummon->GetPriority() == ArObject::UPDATE_PRIORITY_NULL ||
			pSummon->GetPriority() == ArObject::UPDATE_PRIORITY_IDLE )
		{
			ArcadiaServer::Instance().SetObjectPriority( pSummon, ArObject::UPDATE_PRIORITY_NORMAL );
		}
	}
}

void StructPlayer::_applyCharm( const int type, const c_fixed10 & var1, const c_fixed10 & var2, struct StructItem * pItem )
{
	switch( type )
	{
	case ITEM_EFFECT_CHARM::INC_SPEED:
		if( m_nSpeedModifier < var1 )
			m_nSpeedModifier = var1;
		break;
	case ITEM_EFFECT_CHARM::INC_MAX_STAMINA:
		if( m_nMaxStamina < var1 )
			m_nMaxStamina = var1;
		break;
	case ITEM_EFFECT_CHARM::STAMINA_REGEN_TENT:
		m_bUsingTent = true;
		break;
	case ITEM_EFFECT_CHARM::INC_STAMINA_REGEN:
		m_nStaminaRegenBonus = var1;
		break;
	case ITEM_EFFECT_CHARM::INC_MAX_WEIGHT:
		if( m_nMaxWeightModifier < var1 )
			m_nMaxWeightModifier = var1;
		break;
	case ITEM_EFFECT_CHARM::INC_RIDING_SPEED:
		if( m_nRideModifier < var1 )
			m_nRideModifier = var1;
		break;
	case ITEM_EFFECT_PASSIVE::INC_PARAMETER_A:
		incParameter(var1, var2, false);
		break;
	case ITEM_EFFECT_PASSIVE::INC_PARAMETER_B:
		incParameter2(var1, var2);
		break;
	case ITEM_EFFECT_PASSIVE::AMP_PARAMETER_A:
		ampParameter(var1, var2, false);
		break;
	case ITEM_EFFECT_PASSIVE::AMP_PARAMETER_B:
		ampParameter(var1, var2, false);
		break;

	case ITEM_EFFECT_CHARM::AUTO_RECOVER_HP:
		{
			int nIdx = pItem->GetIdx();
			std::vector< AUTO_RECOVER_INFO >::iterator it;
			for( it = m_vAutoRecoverInfo.begin() ; it != m_vAutoRecoverInfo.end() ; ++it )
			{
				if( (*it).m_nOrderIndex > nIdx )
					break;
			}
			m_vAutoRecoverInfo.insert( it, AUTO_RECOVER_INFO( RECOVER_TYPE_HP, var1, var2, pItem->GetHandle(), pItem->GetIdx() ) );
		}
		break;
	case ITEM_EFFECT_CHARM::AUTO_RECOVER_MP:
		{
			int nIdx = pItem->GetIdx();
			std::vector< AUTO_RECOVER_INFO >::iterator it;
			for( it = m_vAutoRecoverInfo.begin() ; it != m_vAutoRecoverInfo.end() ; ++it )
			{
				if( (*it).m_nOrderIndex > nIdx )
					break;
			}
			m_vAutoRecoverInfo.insert( it, AUTO_RECOVER_INFO( RECOVER_TYPE_MP, var1, var2, pItem->GetHandle(), pItem->GetIdx() ) );
		}
		break;
	}
}

void StructPlayer::applyCharm( struct StructItem * pItem )
{
	const ItemBaseServer & base = pItem->GetItemBase();

	for( int x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
	{
		if( !base.nOptType[x] ) 
			continue;

		_applyCharm( base.nOptType[ x ], base.fOptVar1[ x ], base.fOptVar2[ x ], pItem );
	}

	if( !base.pvEffectList || base.pvEffectList->empty() )
		return;

	for( std::vector< EffectInfo * >::const_iterator it = base.pvEffectList->begin() ; it != base.pvEffectList->end() ; ++it )
	{
		if( (*it)->eType != EffectInfo::EFFECT_TYPE_BASIC && (*it)->eType != EffectInfo::EFFECT_TYPE_OPTIONAL )
			continue;

		if( (*it)->nMinLevel && GetLevel() < (*it)->nMinLevel ) continue;
		if( (*it)->nMaxLevel && GetLevel() > (*it)->nMaxLevel ) continue;

		_applyCharm( (*it)->fValue[ 0 ], (*it)->fValue[ 1 ], (*it)->fValue[ 2 ], pItem );
		_applyCharm( (*it)->fValue[ 3 ], (*it)->fValue[ 4 ], (*it)->fValue[ 5 ], pItem );
		_applyCharm( (*it)->fValue[ 6 ], (*it)->fValue[ 7 ], (*it)->fValue[ 8 ], pItem );
		_applyCharm( (*it)->fValue[ 9 ], (*it)->fValue[ 10 ], (*it)->fValue[ 11 ], pItem );
	}
}

void StructCreature::onCompleteCalculateStat()
{
}

void StructMonster::onCompleteCalculateStat()
{
	if( IsDungeonRaidMonster() )
	{
		m_Attribute.fMoveSpeed *= 1.5f;
	}
}

void StructSummon::onCompleteCalculateStat()
{
	// { 탑승 가능 소환수인지 확인
	StructSkill * pSkill = GetCurrentPassiveSkill( StructSkill::SKILL_CREATURE_RIDING );

	if( pSkill )
	{
		m_bIsRidable = true;

		m_fRideSpeedMod += pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetCurrentSkillLevel();

		if( m_fRideSpeedMod > 1.0f )
		{
			m_fRideSpeedMod = 1.0f;
		}

		m_nUnMountProbabilityOnDamage -= pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
		m_nUnMountProbabilityOnCriticalDamage -= pSkill->GetVar(2) + pSkill->GetVar(3) * pSkill->GetCurrentSkillLevel();
	}

	m_nMaxSP *= 1.0f + m_fMaxSPAmplifier;
	m_nSPRegenAdd *= 1.0f + m_fSPRegenAddAmplifier;

	if (GetMaster())
	{
		float fSummonFCM = GetMaster()->GetSummonFCM() + 1.0f;
		m_Attribute.fAttackPointRight *= fSummonFCM;
		m_Attribute.fAttackPointLeft *= fSummonFCM;
		m_Attribute.fMagicPoint *= fSummonFCM; 
		m_Attribute.fDefence *= fSummonFCM;
		m_Attribute.fMagicDefence *= fSummonFCM;
	}

	// }
}

// --- 각종 스탯을 계산하는 유틸리티 함수들 ---
void StructCreature::accumulateStatDiffByState( const CreatureStatServer& stateStat )
{
	m_StatByState.strength		+= m_Stat.strength		- stateStat.strength;
	m_StatByState.vital			+= m_Stat.vital			- stateStat.vital;
	m_StatByState.dexterity		+= m_Stat.dexterity		- stateStat.dexterity;
	m_StatByState.agility		+= m_Stat.agility		- stateStat.agility;
	m_StatByState.intelligence	+= m_Stat.intelligence	- stateStat.intelligence;
	m_StatByState.mentality		+= m_Stat.mentality		- stateStat.mentality;
	m_StatByState.luck			+= m_Stat.luck			- stateStat.luck;
}

void StructCreature::accumulateAttributeDiffByState( const CreatureAttributeServer& stateAttr )
{
	m_AttributeByState.fCritical			+= m_Attribute.fCritical			- stateAttr.fCritical;
	m_AttributeByState.fAttackPointRight	+= m_Attribute.fAttackPointRight	- stateAttr.fAttackPointRight;
	m_AttributeByState.fAttackPointLeft		+= m_Attribute.fAttackPointLeft		- stateAttr.fAttackPointLeft;
	m_AttributeByState.fDefence				+= m_Attribute.fDefence				- stateAttr.fDefence;
	m_AttributeByState.fBlockDefence		+= m_Attribute.fBlockDefence		- stateAttr.fBlockDefence;
	m_AttributeByState.fMagicPoint			+= m_Attribute.fMagicPoint			- stateAttr.fMagicPoint;
	m_AttributeByState.fMagicDefence		+= m_Attribute.fMagicDefence		- stateAttr.fMagicDefence;
	m_AttributeByState.fAccuracyRight		+= m_Attribute.fAccuracyRight		- stateAttr.fAccuracyRight;
	m_AttributeByState.fAccuracyLeft		+= m_Attribute.fAccuracyLeft		- stateAttr.fAccuracyLeft;
	m_AttributeByState.fMagicAccuracy		+= m_Attribute.fMagicAccuracy		- stateAttr.fMagicAccuracy;
	m_AttributeByState.fAvoid				+= m_Attribute.fAvoid				- stateAttr.fAvoid;
	m_AttributeByState.fMagicAvoid			+= m_Attribute.fMagicAvoid			- stateAttr.fMagicAvoid;
	m_AttributeByState.fBlockChance			+= m_Attribute.fBlockChance			- stateAttr.fBlockChance;
	m_AttributeByState.fMoveSpeed			+= m_Attribute.fMoveSpeed			- stateAttr.fMoveSpeed;
	m_AttributeByState.fAttackSpeedRight	+= m_Attribute.fAttackSpeedRight	- stateAttr.fAttackSpeedRight;
	m_AttributeByState.fAttackSpeedLeft		+= m_Attribute.fAttackSpeedLeft		- stateAttr.fAttackSpeedLeft;
	m_AttributeByState.fDoubleAttackRatio	+= m_Attribute.fDoubleAttackRatio	- stateAttr.fDoubleAttackRatio;
	m_AttributeByState.fAttackRange			+= m_Attribute.fAttackRange			- stateAttr.fAttackRange;
	m_AttributeByState.fMaxWeight			+= m_Attribute.fMaxWeight			- stateAttr.fMaxWeight;
	m_AttributeByState.fCastingSpeed		+= m_Attribute.fCastingSpeed		- stateAttr.fCastingSpeed;
	m_AttributeByState.fCoolTimeSpeed		+= m_Attribute.fCoolTimeSpeed		- stateAttr.fCoolTimeSpeed;
	m_AttributeByState.fHPRegenPercentage	+= m_Attribute.fHPRegenPercentage	- stateAttr.fHPRegenPercentage;
	m_AttributeByState.fHPRegenPoint		+= m_Attribute.fHPRegenPoint		- stateAttr.fHPRegenPoint;
	m_AttributeByState.fMPRegenPercentage	+= m_Attribute.fMPRegenPercentage	- stateAttr.fMPRegenPercentage;
	m_AttributeByState.fMPRegenPoint		+= m_Attribute.fMPRegenPoint		- stateAttr.fMPRegenPoint;
	m_AttributeByState.fPerfectBlock				+= m_Attribute.fPerfectBlock				- stateAttr.fPerfectBlock;
	m_AttributeByState.fPhysicalDefIgnore			+= m_Attribute.fPhysicalDefIgnore			- stateAttr.fPhysicalDefIgnore;
	m_AttributeByState.fPhysicalDefIgnoreRatio		+= m_Attribute.fPhysicalDefIgnoreRatio		- stateAttr.fPhysicalDefIgnoreRatio;
	m_AttributeByState.fMagicalDefIgnore			+= m_Attribute.fMagicalDefIgnore			- stateAttr.fMagicalDefIgnore;
	m_AttributeByState.fMagicalDefIgnoreRatio		+= m_Attribute.fMagicalDefIgnoreRatio		- stateAttr.fMagicalDefIgnoreRatio;
	m_AttributeByState.fPhysicalPenetration			+= m_Attribute.fPhysicalPenetration			- stateAttr.fPhysicalPenetration;
	m_AttributeByState.fPhysicalPenetrationRatio	+= m_Attribute.fPhysicalPenetrationRatio	- stateAttr.fPhysicalPenetrationRatio;
	m_AttributeByState.fMagicalPenetration			+= m_Attribute.fMagicalPenetration			- stateAttr.fMagicalPenetration;
	m_AttributeByState.fMagicalPenetrationRatio		+= m_Attribute.fMagicalPenetrationRatio		- stateAttr.fMagicalPenetrationRatio;
}

void StructCreature::getAmplifiedStatByAmplifier( CreatureStatServer * pStat )
{
	pStat->strength			+= ( m_StatAmplifier.strength ) * pStat->strength;
	pStat->vital			+= ( m_StatAmplifier.vital ) * pStat->vital;
	pStat->dexterity		+= ( m_StatAmplifier.dexterity ) * pStat->dexterity;
	pStat->agility			+= ( m_StatAmplifier.agility ) * pStat->agility;
	pStat->intelligence		+= ( m_StatAmplifier.intelligence ) * pStat->intelligence;
	pStat->mentality		+= ( m_StatAmplifier.mentality ) * pStat->mentality;
	pStat->luck				+= ( m_StatAmplifier.luck ) * pStat->luck;
}

void StructCreature::getAmplifiedAttributeByAmplifier( CreatureAttributeServer * pAttribute )
{
	pAttribute->fCritical						+= ( m_AttributeAmplifier.fCritical ) * pAttribute->fCritical;
	pAttribute->fCriticalPower					+= ( m_AttributeAmplifier.fCriticalPower ) * pAttribute->fCriticalPower;
	pAttribute->fAttackPointRight				+= ( m_AttributeAmplifier.fAttackPointRight ) * pAttribute->fAttackPointRight;
	pAttribute->fAttackPointLeft				+= ( m_AttributeAmplifier.fAttackPointLeft ) * pAttribute->fAttackPointLeft;
	pAttribute->fDefence						+= ( m_AttributeAmplifier.fDefence ) * pAttribute->fDefence;
	pAttribute->fBlockDefence					+= ( m_AttributeAmplifier.fBlockDefence ) * pAttribute->fBlockDefence;
	pAttribute->fMagicPoint						+= ( m_AttributeAmplifier.fMagicPoint ) * pAttribute->fMagicPoint;
	pAttribute->fMagicDefence					+= ( m_AttributeAmplifier.fMagicDefence ) * pAttribute->fMagicDefence;
	pAttribute->fAccuracyRight					+= ( m_AttributeAmplifier.fAccuracyRight ) * pAttribute->fAccuracyRight;
	pAttribute->fAccuracyLeft					+= ( m_AttributeAmplifier.fAccuracyLeft ) * pAttribute->fAccuracyLeft;
	pAttribute->fMagicAccuracy					+= ( m_AttributeAmplifier.fMagicAccuracy ) * pAttribute->fMagicAccuracy;
	pAttribute->fAvoid							+= ( m_AttributeAmplifier.fAvoid ) * pAttribute->fAvoid;
	pAttribute->fMagicAvoid						+= ( m_AttributeAmplifier.fMagicAvoid ) * pAttribute->fMagicAvoid;
	pAttribute->fBlockChance					+= ( m_AttributeAmplifier.fBlockChance ) * pAttribute->fBlockChance;
	if( !isMoveSpeedFixed() )
	{
		pAttribute->fMoveSpeed					+= ( m_AttributeAmplifier.fMoveSpeed ) * pAttribute->fMoveSpeed;
	}
	pAttribute->fAttackSpeed					+= ( m_AttributeAmplifier.fAttackSpeed ) * pAttribute->fAttackSpeed;
	pAttribute->fAttackRange					+= ( m_AttributeAmplifier.fAttackRange ) * pAttribute->fAttackRange;
	pAttribute->fMaxWeight						+= ( m_AttributeAmplifier.fMaxWeight ) * pAttribute->fMaxWeight;
	pAttribute->fCastingSpeed					+= ( m_AttributeAmplifier.fCastingSpeed ) * pAttribute->fCastingSpeed;
	pAttribute->fCoolTimeSpeed					+= ( m_AttributeAmplifier.fCoolTimeSpeed ) * pAttribute->fCoolTimeSpeed;
	pAttribute->fItemChance						+= ( m_AttributeAmplifier.fItemChance ) * pAttribute->fItemChance;
	pAttribute->fHPRegenPercentage				+= ( m_AttributeAmplifier.fHPRegenPercentage ) * pAttribute->fHPRegenPercentage;
	pAttribute->fHPRegenPoint					+= ( m_AttributeAmplifier.fHPRegenPoint ) * pAttribute->fHPRegenPoint;
	pAttribute->fMPRegenPercentage				+= ( m_AttributeAmplifier.fMPRegenPercentage ) * pAttribute->fMPRegenPercentage;
	pAttribute->fMPRegenPoint					+= ( m_AttributeAmplifier.fMPRegenPoint ) * pAttribute->fMPRegenPoint;

	pAttribute->fAttackSpeedRight				+= ( m_AttributeAmplifier.fAttackSpeedRight ) * pAttribute->fAttackSpeedRight;
	pAttribute->fAttackSpeedLeft				+= ( m_AttributeAmplifier.fAttackSpeedLeft ) * pAttribute->fAttackSpeedLeft;
	pAttribute->fDoubleAttackRatio				+= ( m_AttributeAmplifier.fDoubleAttackRatio ) * pAttribute->fDoubleAttackRatio;
	pAttribute->fStunResistance					+= ( m_AttributeAmplifier.fStunResistance ) * pAttribute->fStunResistance;
	pAttribute->fMoveSpeedDecreaseResistance	+= ( m_AttributeAmplifier.fMoveSpeedDecreaseResistance ) * pAttribute->fMoveSpeedDecreaseResistance;
	pAttribute->fHPAdd							+= ( m_AttributeAmplifier.fHPAdd ) * pAttribute->fHPAdd;
	pAttribute->fMPAdd							+= ( m_AttributeAmplifier.fMPAdd ) * pAttribute->fMPAdd;
	pAttribute->fHPAddByItem					+= ( m_AttributeAmplifier.fHPAddByItem ) * pAttribute->fHPAddByItem;
	pAttribute->fMPAddByItem					+= ( m_AttributeAmplifier.fMPAddByItem ) * pAttribute->fMPAddByItem;
}

void StructCreature::getAmplifiedResistByAmplifier( CreatureElementalResist * pResist )
{
	for( int i = 0; i < Elemental::TYPE_COUNT; ++i )
	{
		pResist->nResist[i] += ( m_ResistAmplifier.fResist[i] ) * pResist->nResist[i];
	}
}

void StructCreature::incParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat, ParameterForSummon *pParam )
{
	if( bStat )
	{
		CreatureStatServer & Stat = pParam ? pParam->Stat : m_Stat;

		if( nBitset & StructState::FLAG_STR )
		{
			Stat.strength += fValue;
		}
		if( nBitset & StructState::FLAG_VIT )
		{
			Stat.vital += fValue;
		}
		if( nBitset & StructState::FLAG_AGI )
		{
			Stat.agility += fValue;
		}
		if( nBitset & StructState::FLAG_DEX )
		{
			Stat.dexterity += fValue;
		}
		if( nBitset & StructState::FLAG_INT )
		{
			Stat.intelligence += fValue;
		}
		if( nBitset & StructState::FLAG_MEN )
		{
			Stat.mentality += fValue;
		}
		if( nBitset & StructState::FLAG_LUK )
		{
			Stat.luck += fValue;
		}
	}
	else
	{
		// pParam의 여부에 따라 반드시 받아야 할 변수
		CreatureAttributeServer & Attribute = pParam ? pParam->Attribute : m_Attribute;
		int & nMaxHP = pParam ? pParam->nMaxHP : m_nMaxHP;
		int & nMaxMP = pParam ? pParam->nMaxMP : m_nMaxMP;
		c_fixed10 & fAttackPointRightWithoutWeapon = pParam ? pParam->fAttackPointRightWithoutWeapon : m_fAttackPointRightWithoutWeapon;

		if( nBitset & StructState::FLAG_ATTACK_POINT )
		{
			Attribute.fAttackPointRight += fValue;
			fAttackPointRightWithoutWeapon += fValue;

			if( IsUsingDoubleWeapon() )
			{
				m_Attribute.fAttackPointLeft += fValue;
				m_fAttackPointLeftWithoutWeapon += fValue;
			}
		}

		if( nBitset & StructState::FLAG_MAGIC_POINT )
		{
			Attribute.fMagicPoint += fValue;
		}

		if( nBitset & StructState::FLAG_DEFENCE )
		{
			Attribute.fDefence += fValue;
		}

		if( nBitset & StructState::FLAG_MAGIC_DEFENCE )
		{
			Attribute.fMagicDefence += fValue;
		}

		if( nBitset & StructState::FLAG_ATTACK_SPEED )
		{
			Attribute.fAttackSpeedRight += fValue;

			if( IsUsingDoubleWeapon() )
				Attribute.fAttackSpeedLeft += fValue;
		}

		if( nBitset & StructState::FLAG_CAST_SPEED )
		{
			Attribute.fCastingSpeed += fValue;
		}

		if( nBitset & StructState::FLAG_MOVE_SPEED )
		{
			if( pParam || ( !isMoveSpeedFixed() ) )	// fRidingSpeed 수치가 생기면서 라이딩 중에도 기본 속도에 관여할 수 있게 변경 ( 2013-12-20 )
			{
				Attribute.fMoveSpeed += fValue;
			}
		}

		if( nBitset & StructState::FLAG_ACCURACY )
		{
			Attribute.fAccuracyRight += fValue;

			if( IsUsingDoubleWeapon() )
				Attribute.fAccuracyLeft += fValue;
		}

		if( nBitset & StructState::FLAG_MAGIC_ACCURACY )
		{
			Attribute.fMagicAccuracy += fValue;
		}

		if( nBitset & StructState::FLAG_CRITICAL )
		{
			Attribute.fCritical += fValue;
		}

		if( nBitset & StructState::FLAG_BLOCK )
		{
			Attribute.fBlockChance += fValue;
		}

		if( nBitset & StructState::FLAG_BLOCK_DEFENCE )
		{
			Attribute.fBlockDefence += fValue;
		}

		if( nBitset & StructState::FLAG_AVOID )
		{
			Attribute.fAvoid += fValue;
		}

		if( nBitset & StructState::FLAG_MAGIC_RESISTANCE )
		{
			Attribute.fMagicAvoid += fValue;
		}

		if( nBitset & StructState::FLAG_MAX_HP )
		{
			nMaxHP += fValue;
		}

		if( nBitset & StructState::FLAG_MAX_MP )
		{
			nMaxMP += fValue;
		}

		if( nBitset & StructState::FLAG_MAX_SP )
		{
			// 소환수에서만 사용
		}

		if( nBitset & StructState::FLAG_HP_REGEN_ADD )
		{
			Attribute.fHPRegenPoint += fValue;
		}

		if( nBitset & StructState::FLAG_MP_REGEN_ADD )
		{
			Attribute.fMPRegenPoint += fValue;
		}

		if( nBitset & StructState::FLAG_SP_REGEN_ADD )
		{
			// 소환수에서만 사용
		}

		if( nBitset & StructState::FLAG_HP_REGEN_RATIO )
		{
			Attribute.fHPRegenPercentage += fValue;
		}

		if( nBitset & StructState::FLAG_MP_REGEN_RATIO )
		{
			Attribute.fMPRegenPercentage += fValue;
		}

		if( nBitset & StructState::FLAG_MAX_WEIGHT )
		{
			Attribute.fMaxWeight += fValue;
		}
	}
}

void StructSummon::incParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat, ParameterForSummon *pParam )
{
	int & nMaxSP = pParam ? pParam->nMaxSP : m_nMaxSP;
	short & nSPRegenAdd = pParam ? pParam->nSPRegenAdd : m_nSPRegenAdd;

	if( !bStat )
	{
		if( nBitset & StructState::FLAG_MAX_SP )
		{
			nMaxSP += fValue;
		}
		if( nBitset & StructState::FLAG_SP_REGEN_ADD )
		{
			nSPRegenAdd += fValue;
		}
	}

	StructCreature::incParameter( nBitset, fValue, bStat, pParam );
}

void StructCreature::ampParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat )
{
	if( bStat )
	{
		if( nBitset & StructState::FLAG_STR )
		{
			m_StatAmplifier.strength += fValue;
		}
		if( nBitset & StructState::FLAG_VIT )
		{
			m_StatAmplifier.vital += fValue;
		}
		if( nBitset & StructState::FLAG_AGI )
		{
			m_StatAmplifier.agility += fValue;
		}
		if( nBitset & StructState::FLAG_DEX )
		{
			m_StatAmplifier.dexterity += fValue;
		}
		if( nBitset & StructState::FLAG_INT )
		{
			m_StatAmplifier.intelligence += fValue;
		}
		if( nBitset & StructState::FLAG_MEN )
		{
			m_StatAmplifier.mentality += fValue;
		}
		if( nBitset & StructState::FLAG_LUK )
		{
			m_StatAmplifier.luck += fValue;
		}
	}
	else
	{
		if( nBitset & StructState::FLAG_ATTACK_POINT )
		{
			m_AttributeAmplifier.fAttackPointRight += fValue;

			if( IsUsingDoubleWeapon() )
				m_AttributeAmplifier.fAttackPointLeft += fValue;

		}

		if( nBitset & StructState::FLAG_MAGIC_POINT )
		{
			m_AttributeAmplifier.fMagicPoint += fValue;
		}

		if( nBitset & StructState::FLAG_DEFENCE )
		{
			m_AttributeAmplifier.fDefence += fValue;
		}

		if( nBitset & StructState::FLAG_MAGIC_DEFENCE )
		{
			m_AttributeAmplifier.fMagicDefence += fValue;
		}

		if( nBitset & StructState::FLAG_ATTACK_SPEED )
		{
			m_AttributeAmplifier.fAttackSpeedRight += fValue;

			if( IsUsingDoubleWeapon() )
				m_AttributeAmplifier.fAttackSpeedLeft += fValue;
		}

		if( nBitset & StructState::FLAG_CAST_SPEED )
		{
			m_AttributeAmplifier.fCastingSpeed += fValue;
		}

		if( nBitset & StructState::FLAG_MOVE_SPEED )
		{
			// fRidingSpeed 수치가 생기면서 라이딩 중에도 기본 속도에 관여할 수 있게 변경 ( 2013-12-20 )
			m_AttributeAmplifier.fMoveSpeed += fValue;
		}

		if( nBitset & StructState::FLAG_ACCURACY )
		{
			m_AttributeAmplifier.fAccuracyRight += fValue;

			if( IsUsingDoubleWeapon() )
				m_AttributeAmplifier.fAccuracyLeft += fValue;
		}

		if( nBitset & StructState::FLAG_MAGIC_ACCURACY )
		{
			m_AttributeAmplifier.fMagicAccuracy += fValue;
		}

		if( nBitset & StructState::FLAG_CRITICAL )
		{
			m_AttributeAmplifier.fCritical += fValue;
		}

		if( nBitset & StructState::FLAG_BLOCK )
		{
			m_AttributeAmplifier.fBlockChance += fValue;
		}

		if( nBitset & StructState::FLAG_BLOCK_DEFENCE )
		{
			m_AttributeAmplifier.fBlockDefence += fValue;
		}

		if( nBitset & StructState::FLAG_AVOID )
		{
			m_AttributeAmplifier.fAvoid += fValue;
		}

		if( nBitset & StructState::FLAG_MAGIC_RESISTANCE )
		{
			m_AttributeAmplifier.fMagicAvoid += fValue;
		}

		if( nBitset & StructState::FLAG_MAX_HP )
		{
			m_fMaxHPAmplifier += fValue;
		}

		if( nBitset & StructState::FLAG_MAX_MP )
		{
			m_fMaxMPAmplifier += fValue;
		}

		if( nBitset & StructState::FLAG_MAX_SP )
		{
			// 소환수에서만 사용
		}

		if( nBitset & StructState::FLAG_HP_REGEN_ADD )
		{
			m_AttributeAmplifier.fHPRegenPoint += fValue;
		}

		if( nBitset & StructState::FLAG_MP_REGEN_ADD )
		{
			m_AttributeAmplifier.fMPRegenPoint += fValue;
		}

		if( nBitset & StructState::FLAG_SP_REGEN_ADD )
		{
			// 소환수에서만 사용
		}

		if( nBitset & StructState::FLAG_HP_REGEN_RATIO )
		{
			m_AttributeAmplifier.fHPRegenPercentage += fValue;
		}

		if( nBitset & StructState::FLAG_MP_REGEN_RATIO )
		{
			m_AttributeAmplifier.fMPRegenPercentage += fValue;
		}

		if( nBitset & StructState::FLAG_MAX_WEIGHT )
		{
			m_AttributeAmplifier.fMaxWeight += fValue;
		}

	}
}

void StructSummon::ampParameter( const int nBitset, const c_fixed10 & fValue, const bool bStat )
{
	if( !bStat )
	{
		if( nBitset & StructState::FLAG_MAX_SP )
		{
			m_fMaxSPAmplifier += float( fValue );
		}
		if( nBitset & StructState::FLAG_SP_REGEN_ADD )
		{
			m_fSPRegenAddAmplifier += float( fValue );
		}
	}

	StructCreature::ampParameter( nBitset, fValue, bStat );
}

void StructCreature::incParameter2( const int nBitset, const c_fixed10 & fValue, ParameterForSummon *pParam )
{
	CreatureAttributeServer & Attribute = pParam ? pParam->Attribute : m_Attribute;
	CreatureElementalResist & Resist = pParam ? pParam->Resist : m_Resist;

	if( nBitset & StructState::FLAG_ET_NONE_RESIST )
	{
		Resist.nResist[ Elemental::TYPE_NONE ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_FIRE_RESIST )
	{
		Resist.nResist[ Elemental::TYPE_FIRE ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_WATER_RESIST )
	{
		Resist.nResist[ Elemental::TYPE_WATER ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_WIND_RESIST )
	{
		Resist.nResist[ Elemental::TYPE_WIND ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_EARTH_RESIST )
	{
		Resist.nResist[ Elemental::TYPE_EARTH ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_LIGHT_RESIST )
	{
		Resist.nResist[ Elemental::TYPE_LIGHT ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_DARK_RESIST )
	{
		Resist.nResist[ Elemental::TYPE_DARK ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_ATTACK_RANGE )
	{
		if( pParam || IsUsingBow() || IsUsingCrossBow() )
			// 사정거리 단위, 내부적으로는 기획 DB 에서 ATTACK_RANGE_UNIT 를 곱한 값으로 처리된다.
			Attribute.fAttackRange += fValue * GameRule::ATTACK_RANGE_UNIT;
	}

	if( !pParam )
	{
		if( nBitset & StructState::FLAG_ET_NONE_ADDITIONAL_DAMAGE )
		{
			m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_NONE, fValue, 0 ) );
			m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_NONE, fValue, 0 ) );	
		}
		if( nBitset & StructState::FLAG_ET_FIRE_ADDITIONAL_DAMAGE )
		{
			m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_FIRE, fValue, 0 ) );
			m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_FIRE, fValue, 0 ) );
		}

		if( nBitset & StructState::FLAG_ET_WATER_ADDITIONAL_DAMAGE )
		{
			m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_WATER, fValue, 0 ) );
			m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_WATER, fValue, 0 ) );
		}

		if( nBitset & StructState::FLAG_ET_WIND_ADDITIONAL_DAMAGE )
		{
			m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_WIND, fValue, 0 ) );
			m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_WIND, fValue, 0 ) );
		}

		if( nBitset & StructState::FLAG_ET_EARTH_ADDITIONAL_DAMAGE )
		{
			m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_EARTH, fValue, 0 ) );
			m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_EARTH, fValue, 0 ) );
		}

		if( nBitset & StructState::FLAG_ET_LIGHT_ADDITIONAL_DAMAGE )
		{
			m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_LIGHT, fValue, 0 ) );
			m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_LIGHT, fValue, 0 ) );
		}

		if( nBitset & StructState::FLAG_ET_DARK_ADDITIONAL_DAMAGE )
		{
			m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_DARK, fValue, 0 ) );
			m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_DARK, fValue, 0 ) );
		}
	}

	if( nBitset & StructState::FLAG_CRITICAL_DAMAGE )
	{
		Attribute.fCriticalPower += fValue;
	}

	if( nBitset & StructState::FLAG_ET_PERFECT_BLOCK )
	{
		Attribute.fPerfectBlock += fValue;
	}

	if( nBitset & StructState::FLAG_ET_IGNORE_PHYSICAL_DEFENCE )
	{
		Attribute.fPhysicalDefIgnore += fValue;
	}

	if( nBitset & StructState::FLAG_ET_IGNORE_MAGICAL_DEFENCE )
	{
		Attribute.fMagicalDefIgnore += fValue;
	}

	if( nBitset & StructState::FLAG_ET_PHYSICAL_PENETRATION )
	{
		Attribute.fPhysicalPenetration += fValue;
	}

	if( nBitset & StructState::FLAG_ET_MAGICAL_PENETRATION )
	{
		Attribute.fMagicalPenetration += fValue;
	}

	if( !pParam )
	{
		if( nBitset & StructState::FLAG_HP_REGEN_STOP )
		{
			m_StatusFlag.On( STATUS_HP_REGEN_STOPPED );
		}

		if( nBitset & StructState::FLAG_MP_REGEN_STOP )
		{
			m_StatusFlag.On( STATUS_MP_REGEN_STOPPED );
		}
	}
}

void StructCreature::ampParameter2( const int nBitset, const c_fixed10 & fValue )
{
	if( nBitset & StructState::FLAG_ET_NONE_RESIST )
	{
		m_ResistAmplifier.fResist[ Elemental::TYPE_NONE ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_FIRE_RESIST )
	{
		m_ResistAmplifier.fResist[ Elemental::TYPE_FIRE ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_WATER_RESIST )
	{
		m_ResistAmplifier.fResist[ Elemental::TYPE_WATER ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_WIND_RESIST )
	{
		m_ResistAmplifier.fResist[ Elemental::TYPE_WIND ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_EARTH_RESIST )
	{
		m_ResistAmplifier.fResist[ Elemental::TYPE_EARTH ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_LIGHT_RESIST )
	{
		m_ResistAmplifier.fResist[ Elemental::TYPE_LIGHT ] += fValue;
	}

	if( nBitset & StructState::FLAG_ET_DARK_RESIST )
	{
		m_ResistAmplifier.fResist[ Elemental::TYPE_DARK ] += fValue;
	}
	if( nBitset & StructState::FLAG_ET_ATTACK_RANGE )
	{
		if( IsUsingBow() || IsUsingCrossBow() )
			m_AttributeAmplifier.fAttackRange += fValue;
	}
	if( nBitset & StructState::FLAG_ET_NONE_ADDITIONAL_DAMAGE )
	{
		m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_NONE, 0, fValue ) );
		m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_NONE, 0, fValue ) );
	}

	if( nBitset & StructState::FLAG_ET_FIRE_ADDITIONAL_DAMAGE )
	{
		m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_FIRE, 0, fValue ) );
		m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_FIRE, 0, fValue ) );
	}

	if( nBitset & StructState::FLAG_ET_WATER_ADDITIONAL_DAMAGE )
	{
		m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_WATER, 0, fValue ) );
		m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_WATER, 0, fValue ) );
	}

	if( nBitset & StructState::FLAG_ET_WIND_ADDITIONAL_DAMAGE )
	{
		m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_WIND, 0, fValue ) );
		m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_WIND, 0, fValue ) );
	}

	if( nBitset & StructState::FLAG_ET_EARTH_ADDITIONAL_DAMAGE )
	{
		m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_EARTH, 0, fValue ) );
		m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_EARTH, 0, fValue ) );
	}

	if( nBitset & StructState::FLAG_ET_LIGHT_ADDITIONAL_DAMAGE )
	{
		m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_LIGHT, 0, fValue ) );
		m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_LIGHT, 0, fValue ) );
	}

	if( nBitset & StructState::FLAG_ET_DARK_ADDITIONAL_DAMAGE )
	{
		m_vNormalAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_DARK, 0, fValue ) );
		m_vRangeAdditionalDamage.push_back( AdditionalDamageInfo( 100, Elemental::TYPE_NONE, Elemental::TYPE_DARK, 0, fValue ) );
	}

	if( nBitset & StructState::FLAG_CRITICAL_DAMAGE )
	{
		m_AttributeAmplifier.fCriticalPower += fValue;
	}

	if( nBitset & StructState::FLAG_ET_PERFECT_BLOCK )
	{
		m_AttributeAmplifier.fPerfectBlock += fValue;
	}

	// 아래 네 가지 파라메터는 증폭에 있어 여타 파라메터들과 다른 정책을 취한다.
	// 즉, 증가와 증폭이 서로 다른 파라메터를 증가시킨다. 이 경우 % 파라메터는 0~100 사이의 값을 가져야 하므로 100을 곱한다.
	if( nBitset & StructState::FLAG_ET_IGNORE_PHYSICAL_DEFENCE )
	{
		m_Attribute.fPhysicalDefIgnoreRatio += fValue * 100;
	}

	if( nBitset & StructState::FLAG_ET_IGNORE_MAGICAL_DEFENCE )
	{
		m_Attribute.fMagicalDefIgnoreRatio += fValue * 100;
	}

	if( nBitset & StructState::FLAG_ET_PHYSICAL_PENETRATION )
	{
		m_Attribute.fPhysicalPenetrationRatio += fValue * 100;
	}

	if( nBitset & StructState::FLAG_ET_MAGICAL_PENETRATION )
	{
		m_Attribute.fMagicalPenetrationRatio += fValue * 100;
	}

	if( nBitset & StructState::FLAG_HP_REGEN_STOP )
	{
		m_StatusFlag.On( STATUS_HP_REGEN_STOPPED );
	}

	if( nBitset & StructState::FLAG_MP_REGEN_STOP )
	{
		m_StatusFlag.On( STATUS_MP_REGEN_STOPPED );
	}
}

void StructCreature::finalizeStat()
{
	if( m_Stat.strength < 0 )		m_Stat.strength = 0;
	if( m_Stat.vital < 0 )			m_Stat.vital = 0;
	if( m_Stat.dexterity < 0 ) 		m_Stat.dexterity = 0;
	if( m_Stat.agility < 0 ) 		m_Stat.agility = 0;
	if( m_Stat.intelligence < 0 )	m_Stat.intelligence = 0;
	if( m_Stat.mentality < 0 ) 		m_Stat.mentality = 0;
	if( m_Stat.luck < 0 ) 			m_Stat.luck = 0;
}

void StructCreature::onSynchronizeParameter( bool bStat )
{
	for( std::vector< std::pair< int, std::pair< c_fixed10, int > > >::const_iterator it = m_vSynchronizedParameter.begin(); it != m_vSynchronizedParameter.end(); it++ )
	{
		int nBitset = (*it).first;
		c_fixed10 sum = 0;
		int count = 0;

		if( bStat )
		{
			if( nBitset & StructState::FLAG_STR ) { ++count; sum += m_Stat.strength; }
			if( nBitset & StructState::FLAG_VIT ) { ++count; sum += m_Stat.vital; }
			if( nBitset & StructState::FLAG_AGI ) { ++count; sum += m_Stat.agility; }
			if( nBitset & StructState::FLAG_DEX ) { ++count; sum += m_Stat.dexterity; }
			if( nBitset & StructState::FLAG_INT ) { ++count; sum += m_Stat.intelligence; }
			if( nBitset & StructState::FLAG_MEN ) { ++count; sum += m_Stat.mentality; }
			if( nBitset & StructState::FLAG_LUK ) { ++count; sum += m_Stat.luck; }

			if( count == 0 ) continue;
			sum /= count;
			sum *= (*it).second.first;

			// 계산된 평균을 incParameter 함수에서 bStat 인자를 true로 주고 호출하는 편이 성능은 떨어지지만
			// 추후에 incParameter를 Override해서 스텟 계산을 다르게 하는 클래스가 생길 경우에도 문제가 발생하지 않음
			// Ex) StructSummon::incParameter에서 bStat == true 일 때 특정 패시브 스킬에 의해 스텟 증가치가 모두 1.5배가 되게 하는 유형의 기능
			// 아래와 같이 해두면 incParameter virtual 함수 호출 부하와 switch문 처리 부하는 피할 수 있지만
			// 위에서 설명된 내용이 추후에라도 추가될 경우 비정상 동작을 하게됨.
			if( nBitset & StructState::FLAG_STR ) m_Stat.strength = sum;
			if( nBitset & StructState::FLAG_VIT ) m_Stat.vital = sum;
			if( nBitset & StructState::FLAG_AGI ) m_Stat.agility = sum;
			if( nBitset & StructState::FLAG_DEX ) m_Stat.dexterity = sum;
			if( nBitset & StructState::FLAG_INT ) m_Stat.intelligence = sum;
			if( nBitset & StructState::FLAG_MEN ) m_Stat.mentality = sum;
			if( nBitset & StructState::FLAG_LUK ) m_Stat.luck = sum;
		}
		else
		{	
			if( nBitset & StructState::FLAG_ATTACK_POINT )		{ ++count; sum += m_Attribute.fAttackPointRight; }
			if( nBitset & StructState::FLAG_MAGIC_POINT)		{ ++count; sum += m_Attribute.fMagicPoint; }
			if( nBitset & StructState::FLAG_DEFENCE )			{ ++count; sum += m_Attribute.fDefence; }
			if( nBitset & StructState::FLAG_MAGIC_DEFENCE )		{ ++count; sum += m_Attribute.fMagicDefence; }
			if( nBitset & StructState::FLAG_ATTACK_SPEED )		{ ++count; sum += m_Attribute.fAttackSpeedRight; }
			if( nBitset & StructState::FLAG_CAST_SPEED )		{ ++count; sum += m_Attribute.fCastingSpeed; }
			if( nBitset & StructState::FLAG_MOVE_SPEED )		{ ++count; sum += m_Attribute.fMoveSpeed; }
			if( nBitset & StructState::FLAG_ACCURACY )			{ ++count; sum += m_Attribute.fAccuracyRight; }
			if( nBitset & StructState::FLAG_MAGIC_ACCURACY )	{ ++count; sum += m_Attribute.fMagicAccuracy; }
			if( nBitset & StructState::FLAG_CRITICAL )			{ ++count; sum += m_Attribute.fCritical; }
			if( nBitset & StructState::FLAG_BLOCK )				{ ++count; sum += m_Attribute.fBlockChance; }
			if( nBitset & StructState::FLAG_BLOCK_DEFENCE )		{ ++count; sum += m_Attribute.fBlockDefence; }
			if( nBitset & StructState::FLAG_AVOID )				{ ++count; sum += m_Attribute.fAvoid; }
			if( nBitset & StructState::FLAG_MAGIC_RESISTANCE )	{ ++count; sum += m_Attribute.fMagicAvoid; }
			if( nBitset & StructState::FLAG_MAX_HP )			{ ++count; sum += m_nMaxHP; }
			if( nBitset & StructState::FLAG_MAX_MP )			{ ++count; sum += m_nMaxMP; }
			if( nBitset & StructState::FLAG_MAX_SP )			{ /* SP는 사용하지 않음. */ }
			if( nBitset & StructState::FLAG_HP_REGEN_ADD )		{ ++count; sum += m_Attribute.fHPRegenPoint; }
			if( nBitset & StructState::FLAG_MP_REGEN_ADD )		{ ++count; sum += m_Attribute.fMPRegenPoint; }
			if( nBitset & StructState::FLAG_SP_REGEN_ADD )		{ /* SP는 사용하지 않음. */ }
			if( nBitset & StructState::FLAG_HP_REGEN_RATIO )	{ ++count; sum += m_Attribute.fHPRegenPercentage; }
			if( nBitset & StructState::FLAG_MP_REGEN_RATIO )	{ ++count; sum += m_Attribute.fMPRegenPercentage; }
			if( nBitset & StructState::FLAG_MAX_WEIGHT )		{ ++count; sum += m_Attribute.fMaxWeight; }
			if (nBitset & StructState::FLAG_DAMAGE_REDUCE)		{ ++count; sum += m_Attribute.fDamageReduce; }

			if( count == 0 ) continue;
			sum /= count;
			sum *= (*it).second.first;

			if( nBitset & StructState::FLAG_ATTACK_POINT )		incParameter( StructState::FLAG_ATTACK_POINT,		sum - m_Attribute.fAttackPointRight, false );
			if( nBitset & StructState::FLAG_MAGIC_POINT)		incParameter( StructState::FLAG_MAGIC_POINT,		sum - m_Attribute.fMagicPoint, false );
			if( nBitset & StructState::FLAG_DEFENCE )			incParameter( StructState::FLAG_DEFENCE,			sum - m_Attribute.fDefence, false );
			if( nBitset & StructState::FLAG_MAGIC_DEFENCE )		incParameter( StructState::FLAG_MAGIC_DEFENCE,		sum - m_Attribute.fMagicDefence, false );
			if( nBitset & StructState::FLAG_ATTACK_SPEED )		incParameter( StructState::FLAG_ATTACK_SPEED,		sum - m_Attribute.fAttackSpeedRight, false );
			if( nBitset & StructState::FLAG_CAST_SPEED )		incParameter( StructState::FLAG_CAST_SPEED,			sum - m_Attribute.fCastingSpeed, false );
			if( nBitset & StructState::FLAG_MOVE_SPEED )		incParameter( StructState::FLAG_MOVE_SPEED,			sum - m_Attribute.fMoveSpeed, false );
			if( nBitset & StructState::FLAG_ACCURACY )			incParameter( StructState::FLAG_ACCURACY,			sum - m_Attribute.fAccuracyRight, false );
			if( nBitset & StructState::FLAG_MAGIC_ACCURACY )	incParameter( StructState::FLAG_MAGIC_ACCURACY,		sum - m_Attribute.fMagicAccuracy, false );
			if( nBitset & StructState::FLAG_CRITICAL )			incParameter( StructState::FLAG_CRITICAL,			sum - m_Attribute.fCritical, false );
			if( nBitset & StructState::FLAG_BLOCK )				incParameter( StructState::FLAG_BLOCK,				sum - m_Attribute.fBlockChance, false );
			if( nBitset & StructState::FLAG_BLOCK_DEFENCE )		incParameter( StructState::FLAG_BLOCK_DEFENCE,		sum - m_Attribute.fBlockDefence, false );
			if( nBitset & StructState::FLAG_AVOID )				incParameter( StructState::FLAG_AVOID,				sum - m_Attribute.fAvoid, false );
			if( nBitset & StructState::FLAG_MAGIC_RESISTANCE )	incParameter( StructState::FLAG_MAGIC_RESISTANCE,	sum - m_Attribute.fMagicAvoid, false );
			if( nBitset & StructState::FLAG_MAX_HP )			m_nMaxHP += sum - m_nMaxHP;
			if( nBitset & StructState::FLAG_MAX_MP )			m_nMaxMP += sum - m_nMaxMP;
			if( nBitset & StructState::FLAG_MAX_SP )			{ /* SP는 사용하지 않음. */ }
			if( nBitset & StructState::FLAG_HP_REGEN_ADD )		incParameter( StructState::FLAG_HP_REGEN_ADD,		sum - m_Attribute.fHPRegenPoint, false );
			if( nBitset & StructState::FLAG_MP_REGEN_ADD )		incParameter( StructState::FLAG_MP_REGEN_ADD,		sum - m_Attribute.fMPRegenPoint, false );
			if( nBitset & StructState::FLAG_SP_REGEN_ADD )		{ /* SP는 사용하지 않음. */ }
			if( nBitset & StructState::FLAG_HP_REGEN_RATIO )	incParameter( StructState::FLAG_HP_REGEN_RATIO,		sum - m_Attribute.fHPRegenPercentage, false );
			if( nBitset & StructState::FLAG_MP_REGEN_RATIO )	incParameter( StructState::FLAG_MP_REGEN_RATIO,		sum - m_Attribute.fMPRegenPercentage, false );
			if( nBitset & StructState::FLAG_MAX_WEIGHT )		incParameter( StructState::FLAG_MAX_WEIGHT,			sum - m_Attribute.fMaxWeight, false );
			if (nBitset & StructState::FLAG_DAMAGE_REDUCE)		incParameter( StructState::FLAG_DAMAGE_REDUCE,		sum - m_Attribute.fDamageReduce, false);
		}
	}
}

c_fixed10 StructCreature::CalcStatSum( const int bitset, const CreatureStatServer& statBeforeCalc )
{
	c_fixed10 sum;

	if( bitset == 0 )
	{
		return sum;
	}

	if( bitset & StructState::FLAG_STR )
	{
		sum += statBeforeCalc.strength;
	}
	if( bitset & StructState::FLAG_VIT )
	{
		sum += statBeforeCalc.vital;
	}
	if( bitset & StructState::FLAG_AGI )
	{
		sum += statBeforeCalc.agility;
	}
	if( bitset & StructState::FLAG_DEX )
	{
		sum += statBeforeCalc.dexterity;
	}
	if( bitset & StructState::FLAG_INT )
	{
		sum += statBeforeCalc.intelligence;
	}
	if( bitset & StructState::FLAG_MEN )
	{
		sum += statBeforeCalc.mentality;
	}
	if( bitset & StructState::FLAG_LUK )
	{
		sum += statBeforeCalc.luck;
	}

	return sum;
}

c_fixed10 StructCreature::CalcAttributeSum( const int bitset1, const int bitset2, const CreatureAttributeServer& attrBeforeCalc )
{
	c_fixed10 sum;

	if( bitset1 == 0 && bitset2 == 0 )
	{
		return sum;
	}

	if( bitset1 & StructState::FLAG_ATTACK_POINT )
	{
		sum += attrBeforeCalc.fAttackPointRight;

		if( IsUsingDoubleWeapon() )
		{
			sum += attrBeforeCalc.fAttackPointLeft;
		}
	}

	if( bitset1 & StructState::FLAG_MAGIC_POINT )
	{
		sum += attrBeforeCalc.fMagicPoint;
	}

	if( bitset1 & StructState::FLAG_DEFENCE )
	{
		sum += attrBeforeCalc.fDefence;
	}

	if( bitset1 & StructState::FLAG_MAGIC_DEFENCE )
	{
		sum += attrBeforeCalc.fMagicDefence;
	}

	if( bitset1 & StructState::FLAG_ATTACK_SPEED )
	{
		sum += attrBeforeCalc.fAttackSpeedRight;

		if( IsUsingDoubleWeapon() )
			sum += attrBeforeCalc.fAttackSpeedLeft;
	}

	if( bitset1 & StructState::FLAG_CAST_SPEED )
	{
		sum += attrBeforeCalc.fCastingSpeed;
	}

	if( bitset1 & StructState::FLAG_MOVE_SPEED )
	{
		sum += attrBeforeCalc.fMoveSpeed;
	}

	if( bitset1 & StructState::FLAG_ACCURACY )
	{
		sum += attrBeforeCalc.fAccuracyRight;

		if( IsUsingDoubleWeapon() )
			sum += attrBeforeCalc.fAccuracyLeft;
	}

	if( bitset1 & StructState::FLAG_MAGIC_ACCURACY )
	{
		sum += attrBeforeCalc.fMagicAccuracy;
	}

	if( bitset1 & StructState::FLAG_CRITICAL )
	{
		sum += attrBeforeCalc.fCritical;
	}

	if( bitset1 & StructState::FLAG_BLOCK )
	{
		sum += attrBeforeCalc.fBlockChance;
	}

	if( bitset1 & StructState::FLAG_BLOCK_DEFENCE )
	{
		sum += attrBeforeCalc.fBlockDefence;
	}

	if( bitset1 & StructState::FLAG_AVOID )
	{
		sum += attrBeforeCalc.fAvoid;
	}

	if( bitset1 & StructState::FLAG_MAGIC_RESISTANCE )
	{
		sum += attrBeforeCalc.fMagicAvoid;
	}

	if( bitset1 & StructState::FLAG_MAX_HP )
	{
		sum += m_nMaxHP;
	}

	if( bitset1 & StructState::FLAG_MAX_MP )
	{
		sum += m_nMaxMP;
	}

	if( bitset1 & StructState::FLAG_MAX_SP )
	{
		// 소환수에서만 사용
	}

	if( bitset1 & StructState::FLAG_HP_REGEN_ADD )
	{
		sum += attrBeforeCalc.fHPRegenPoint;
	}

	if( bitset1 & StructState::FLAG_MP_REGEN_ADD )
	{
		sum += attrBeforeCalc.fMPRegenPoint;
	}

	if( bitset1 & StructState::FLAG_SP_REGEN_ADD )
	{
		// 소환수에서만 사용
	}

	if( bitset1 & StructState::FLAG_HP_REGEN_RATIO )
	{
		sum += attrBeforeCalc.fHPRegenPercentage;
	}

	if( bitset1 & StructState::FLAG_MP_REGEN_RATIO )
	{
		sum += attrBeforeCalc.fMPRegenPercentage;
	}

	if( bitset1 & StructState::FLAG_MAX_WEIGHT )
	{
		sum += attrBeforeCalc.fMaxWeight;
	}

	if( bitset2 & StructState::FLAG_CRITICAL_DAMAGE )
	{
		sum += attrBeforeCalc.fCriticalPower;
	}

	if( bitset2 & StructState::FLAG_ET_PERFECT_BLOCK )
	{
		sum += attrBeforeCalc.fPerfectBlock;
	}

	if( bitset2 & StructState::FLAG_ET_IGNORE_PHYSICAL_DEFENCE )
	{
		sum += attrBeforeCalc.fPhysicalDefIgnore;
	}

	if( bitset2 & StructState::FLAG_ET_IGNORE_MAGICAL_DEFENCE )
	{
		sum += attrBeforeCalc.fMagicalDefIgnore;
	}

	if( bitset2 & StructState::FLAG_ET_PHYSICAL_PENETRATION )
	{
		sum += attrBeforeCalc.fPhysicalPenetration;
	}

	if( bitset2 & StructState::FLAG_ET_MAGICAL_PENETRATION )
	{
		sum += attrBeforeCalc.fMagicalPenetration;
	}

	return sum;
}

c_fixed10 StructSummon::CalcAttributeSum( const int bitset1, const int bitset2, const CreatureAttributeServer& attrBeforeCalc )
{
	c_fixed10 sum;

	if( bitset1 & StructState::FLAG_MAX_SP )
	{
		sum += m_nMaxSP;
	}

	if( bitset1 & StructState::FLAG_SP_REGEN_ADD )
	{
		sum += m_nSPRegenAdd;
	}

	return sum + StructCreature::CalcAttributeSum( bitset1, bitset2, attrBeforeCalc );
}

bool StructCreature::isSkillModified( std::vector< std::pair< int, int > > * pvPrevAddedSkill, std::vector< std::pair< int, int > > * pvAddedSkill )
{
	if( pvPrevAddedSkill->size() != pvAddedSkill->size() ) return true;

	std::vector< std::pair< int, int > >::const_iterator it;
	std::vector< std::pair< int, int > >::const_iterator prevIt;

	for( it = pvAddedSkill->begin(); it != pvAddedSkill->end(); it++ )
	{
		bool bFound = false;

		for( prevIt = pvPrevAddedSkill->begin(); prevIt!= pvPrevAddedSkill->end(); prevIt++ )
		{
			if( (*it).first == (*prevIt).first )
			{
				if( (*it).second == (*prevIt).second )
				{
					bFound = true;
					break;
				}
				else
				{
					return true;
				}
			}
		}

		if( !bFound ) return true;
	}

	return false;
}