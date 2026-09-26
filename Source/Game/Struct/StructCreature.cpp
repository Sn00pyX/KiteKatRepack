
#include <network/IConnection.h>
#include <mmo/ArRegion.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XConsole.h>
#include <toolkit/XQuadScanner.h>
#include <toolkit/XRandom.h>
#include <toolkit/XSTLUtil.h>
#include <toolkit/XEnv.h>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "StructCreature.h"
#include "StructPlayer.h"	// 흠 ;;
#include "StructMonster.h"	// 냥... ;;
#include "StructNPC.h"
#include "StructSkill.h"
#include "StructSkillProp.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructItem.h"
#include "StructProc.h"
#include "CreatureBase.h"
#include "Extern.h"
#include "GameAllocator.h"
#include "SendMessage.h"
#include "GameProc.h"
#include "GameContent.h"
#include "GameMessage.h"
#include "ChannelManager.h"
#include "GuildManager.h"
#include "DungeonManager.h"
#include "PartyManager.h"
#include "DB_Commands.h"
#include "HuntaholicManager.h"
#include "Damage.h"

XQuadScanner< StructCreature::QuadTreeItem, AR_UNIT >	*		g_pQuadScanner = NULL;

volatile LONG				s_nMaxSkillUID;
volatile LONG				s_nMaxStateUID;
static std::vector< int >	s_vDisabledStateList;
static XCriticalSection		s_csDisabledStateListLock( "s_csDisabledStateListLock" );
volatile int				g_bIgnoreRandomDamage = 0;

void StructCreature::InitCreatureSystem()
{
	if( !g_pQuadScanner )
		g_pQuadScanner = new XQuadScanner< StructCreature::QuadTreeItem, AR_UNIT >( g_nMapWidth, g_nMapHeight );
}

void StructCreature::DeInitCreatureSystem()
{
	if( g_pQuadScanner )
	{
		delete g_pQuadScanner;
		g_pQuadScanner = NULL;
	}
}

bool StructCreature::QuadTreeItem::IsCanAdd( AR_UNIT x, AR_UNIT y )
{
	if( g_pQuadScanner )
		return !g_pQuadScanner->IsExist( QuadTreeItem::getX( x ), QuadTreeItem::getY( y ) );

	return true;
}

void StructCreature::QuadTreeItem::AddMe()
{
	x = getX( pParent->GetX() );
	y = getX( pParent->GetX() );
	if( g_pQuadScanner )
		g_pQuadScanner->Add( *this );
}

bool StructCreature::QuadTreeItem::RemoveMe()
{
	if( g_pQuadScanner )
	{
		if( g_pQuadScanner->Remove( *this ) ) return true;

		// assert( 0 );
		return false;
	}

	return true;
}

static int applyPercentageVar( const int & base_var, const int & percentage )
{
	return base_var * ( ( 100 - percentage ) / 100 );	// 비율적용
}

template <typename T >
T MIN( const T & lh, const T & rh )
{
	return lh < rh ? lh : rh;
}

template <typename T >
T MAX( const T & lh, const T & rh )
{
	return lh > rh ? lh : rh;
}

StructCreature::StructCreature()
{
	m_nTotalJobPoint		= 0;
	m_nMaxHP = m_nHP		= 1;
	m_nMinHP				= 0;
	m_fMinHP				= 0.0f;
	m_nMaxMP = m_nMP		= 1;
	m_nHPDecPart = m_nMPDecPart = 0;
	m_nRegenHP				= 0;
	m_nRegenHP				= 0;

	m_nMaxEnergy = 0;
	m_nEnergy = 0;
	m_nEnergyStartPos = 0;
	m_nEnergyUpkeepTime = 0;
	m_nEnergyUnconsumptionRate = 0;

	memset( m_arEnergy, 0, sizeof( m_arEnergy ) );

	m_StatusFlag.On( STATUS_FIRST_ENTER );
	m_bNeedToBroadcastStatusFlag = false;

	quadTreeItem.Set( *this );

	m_nUnitExpertLevel	= 0;
	m_fWeight			= 0.0f;

	m_nNextAttackableTime	= 0;
	m_nNextCastableTime		= 0;
	m_nDeadTime			= GetArTime();
	m_nMovableTime		= 0;

	m_nNextAttackMode	= StructCreature::AM_AIMING;
	m_fBowInterval		= 0.0f;

	m_LastAttackTime = 0;

	m_nCriticalCount = 0;

	m_nCastKeep = 100;

	m_fBattleLevel = m_nLevel = m_nMaxReachedLevel = 1;

	m_nEXP = 0;
	m_nLastDecreasedEXP = 0;

	m_fEXPMod = 0.0f;

	m_nJob = 100;
	m_nJobLevel= 0;
	m_nJobPoint= 0;

	m_fItemMod = 1.0f;

	m_fHealRatio = 1.0f;
	m_fMPHealRatio = 1.0f;
	m_fHealRatioByItem = 1.0f;
	m_fMPHealRatioByItem = 1.0f;
	m_fHealRatioByRest = 2.0f;
	m_fMPHealRatioByRest = 2.0f;

	m_nAdditionalHeal = 0;
	m_nAdditionalMPHeal = 0;
	m_nAdditionalHealByItem = 0;
	m_nAdditionalMPHealByItem = 0;
	m_nAdditionalHealByRest = 0;
	m_nAdditionalMPHealByRest = 0;

	m_fHateRatio = 1.0f;

	m_nCurrentStateUID = 0;
	m_nPendedClearStateFlag = 0;

	m_fResistHarmfulState = 0.0f;
	m_mapResistHarmfulState.clear();

	m_nLastUpdateTime = GetArTime();
	m_nLastStateProcTime = GetArTime();

	m_hEnemy = 0;
	m_StatusFlag.On( STATUS_FIRST_ATTACK );

	m_hSkillTarget = NULL;
	m_hTrap = NULL;
	m_pCastSkill = NULL;
	memset( m_anWear, 0, sizeof( m_anWear ) );

	memset( m_nPrevJobId, 0, sizeof( m_nPrevJobId ) );	
	memset( m_nPrevJobLevel, 0, sizeof( m_nPrevJobLevel ) );	

	m_nPendingMoveSpeed = 0;

	m_fDetectHideRange = 0.0f;

	m_StatusFlag.On( STATUS_MOVABLE );
	m_StatusFlag.On( STATUS_ATTACKABLE );
	m_StatusFlag.On( STATUS_SKILL_CASTABLE );
	m_StatusFlag.On( STATUS_MAGIC_CASTABLE );
	m_StatusFlag.On( STATUS_ITEM_USABLE );
	m_StatusFlag.On( STATUS_MORTAL );

	m_StatusFlag.On( STATUS_NEED_TO_CALCULATE_STAT );
	m_nDoubleWeaponMasteryLevel = 0;

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

	m_nLuna = 0;
	m_nAttackLuna = 0;

	memset( &m_ParameterForSummon.AttributeAmplifier, 0, sizeof( m_ParameterForSummon.AttributeAmplifier ) );
}

StructCreature::~StructCreature()
{
	// 스킬 정보 삭제
	std::vector< StructSkill * >::iterator	it;	
	for( it = m_vAllSkillList.begin(); it != m_vAllSkillList.end(); ++it )
	{
		// 메모리 풀 사용하도록 변경
		//delete *it;
		(*it)->FreeSkill();
	}
}

int	StructCreature::GetJobDepth() const
{
	const JobInfo* pJobInfo = GameContent::GetJobInfo( GetJobId() );
	return ( pJobInfo ? pJobInfo->job_depth :0 );
}

void StructCreature::SetMaxSkillUID( int nUID )
{
	s_nMaxSkillUID = nUID;
}

void StructCreature::SetMaxStateUID( int nUID )
{
	s_nMaxStateUID = nUID;
}

void StructCreature::SetDisabledStateList( std::vector< int > & vDisabledStateList )
{
	THREAD_SYNCHRONIZE( s_csDisabledStateListLock );

	s_vDisabledStateList.swap( vDisabledStateList );
}

void StructCreature::AddExp( __int64 exp, __int64 jp, bool bApplyStamina )
{
	m_nEXP			 += ( exp );
	m_nJobPoint		 += jp;

	if( jp > 0 )
	{
		m_nTotalJobPoint += ( jp );
	}

	if( IsLoginComplete() )
		onExpChange();
}

void StructCreature::SetEXP( __int64 exp )
{
	m_nEXP		= exp;

	if( IsLoginComplete() )
		onExpChange();
}

void StructCreature::SetJP( __int64 jp )
{
	m_nJobPoint = jp;

	if( m_nJobPoint < 0 )
		m_nJobPoint = 0;

	if( IsLoginComplete() )
		onJPChange();
}

void StructCreature::regenHPMP( AR_TIME t )
{
	if( t - m_nLastUpdateTime < 300 )
		return;

	//	기존의 100초간 리젠량 표기에서 60초간 리젠량으로 파라미터 룰 변경 (2007-03-24 리뉴얼 적용)
	float f_100 = ( t - m_nLastUpdateTime ) / 6000.0f;

	if( IsAlive() )
	{	
		float pt = 0;

		int prev_hp = GetHP();
		int prev_mp = GetMP();

		if( !IsMagicalImmune() )
		{
			if( m_Attribute.fHPAdd )
			{
				pt =  m_Attribute.fHPAdd * f_100;

				if( pt < 1 ) pt += 1;
				if( pt ) Heal( pt );
			}

			if( m_Attribute.fHPAddByItem )
			{
				pt =  m_Attribute.fHPAddByItem * f_100;

				if( pt < 1 ) pt += 1;
				if( pt ) HealByItem( pt );
			}

			if( m_Attribute.fMPAdd )
			{
				pt = m_Attribute.fMPAdd * f_100;

				if( pt < 1 ) pt += 1;
				if( pt ) MPHeal( pt );
			}

			if( m_Attribute.fMPAddByItem )
			{
				pt = m_Attribute.fMPAddByItem * f_100;

				if( pt < 1 ) pt += 1;
				if( pt ) MPHealByItem( pt );
			}
		}

		// HP 증가
		if( !isHPRegenStopped() )
		{
			pt = 0;
			pt += GetHPRegenPercentage() * GetMaxHP() * 0.01 * f_100;
			pt += GetHPRegenPoint() * f_100;

			if( IsSitDown() )
			{
				// 휴식으로 인한 체력 회복 증폭 및 추가 증폭 패시브 적용
				pt *= m_fHealRatioByRest;
				pt += m_nAdditionalHealByRest;
			}

			pt *= m_fHPRegenMod;

			pt = std::max<float>( pt, 1 );
			pt = std::min<float>( pt, GetMaxHP() - GetHP() );

			if( pt ) AddHP( pt );

			// 버려진 소수 자리 누적(소수 둘째 자리 까지)
			m_nHPDecPart += ( pt - (int)pt ) * 100;
			if( m_nHPDecPart / 100 )
			{
				AddHP( m_nHPDecPart / 100 );
				m_nHPDecPart %= 100;
			}
		}

		// MP 증가
		if( !isMPRegenStopped() )
		{
			pt = 0;
			pt += GetMPRegenPercentage() * GetMaxMP() * 0.01 * f_100;
			pt += GetMPRegenPoint() * f_100;

			if( IsSitDown() )
			{
				// 휴식으로 인한 마나 회복 증폭 및 추가 증폭 패시브 적용
				pt *= m_fMPHealRatioByRest;
			}

			pt *= m_fMPRegenMod;

			pt = std::max<float>( pt, 1 );
			pt = std::min<float>( pt, GetMaxMP() - GetMP() );

			if( pt ) AddMP( pt );

			// 버려진 소수 자리 누적(소수 둘째 자리 까지)
			m_nMPDecPart += ( pt - (int)pt ) * 100;
			if( m_nMPDecPart / 100 )
			{
				AddMP( m_nMPDecPart / 100 );
				m_nMPDecPart %= 100;
			}
		}

		do
		{
			// 바뀐거 없음 패스
			if( prev_hp == GetHP() && prev_mp == GetMP() )
				break;

			m_nRegenHP += GetHP() - prev_hp;
			m_nRegenMP += GetMP() - prev_mp;

			// 만땅 차거나, 3% 이상 변했으면 broadcast
			if( GetMaxHP() == GetHP() || GetMaxMP() == GetMP() || m_nRegenHP * 100 / GetMaxHP() > 3 || m_nRegenMP * 100 / GetMaxHP() > 3 )
			{
				TS_SC_REGEN_HPMP msg;

				msg.handle = GetHandle();
				msg.hp_regen = m_nRegenHP;
				msg.mp_regen = m_nRegenMP;
				msg.hp = GetHP();
				msg.mp = GetMP();

				m_nRegenHP = 0;
				m_nRegenMP = 0;

				if( IsInWorld() )
				{
					ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
				}

				if( IsSummon() )
				{
					StructPlayer * pPlayer = static_cast< StructSummon * >( this )->GetMaster();
					if( pPlayer && pPlayer->IsInWorld() && pPlayer->IsLoginComplete() && !pPlayer->GetLogoutTime() )
					{
						PendMessage( pPlayer, &msg );
					}
				}
			}
		}
		while( false );
	}

	m_nLastUpdateTime = t;
}

void StructCreature::RegenFullHPMP()
{
	int nHP = GetHP();
	int nMP = GetMP();

	if( nHP == GetMaxHP() && nMP == GetMaxMP() )
		return;

	SetHP( GetMaxHP() );
	SetMP( GetMaxMP() );

	TS_SC_REGEN_HPMP msg;

	msg.handle = GetHandle();
	msg.hp_regen = GetMaxHP() - nHP;
	msg.mp_regen = GetMaxMP() - nMP;
	msg.hp = GetHP();
	msg.mp = GetMP();

	m_nRegenHP = 0;
	m_nRegenMP = 0;

	ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
}

struct StateDamage
{
	StateDamage( AR_HANDLE _caster, Elemental::Type _elemental_type, int _base_effect_id, StructState::StateCode _code, unsigned short _level, int _damage_hp, int _damage_mp, bool _final )
		: caster( _caster )
		, elemental_type( _elemental_type )
		, base_effect_id( _base_effect_id )
		, code( _code )
		, level( _level )
		, damage_hp( _damage_hp )
		, damage_mp( _damage_mp )
		, final( _final )
	{}

	AR_HANDLE							caster;
	Elemental::Type	elemental_type;
	int									base_effect_id;
	StructState::StateCode				code;
	unsigned short						level;
	int									damage_hp;
	int									damage_mp;
	bool								final;
};

void StructCreature::procState( AR_TIME t )
{
	STATE_ITERATOR it;

	// EF_ADD_REGION_STATE 유형 지속효과 처리용
	struct ADD_STATE_INFO
	{
		StructState::StateCode		code;
		int							nLevel;
		AR_TIME						nEndTime;
		int							nHate;
		StructCreature *			pTarget;
	} add_state_info;

	// 인스턴스를 보관하는 벡터이므로 Deep copy가 필요한 구조체를 넣으면 안됨
	std::vector< ADD_STATE_INFO > vAddStateInfo;

	std::vector< StructState >					vGoodStateRemover;					// 이름 참 구리다...(두 개의 바이올린을 위한 협주곡은 야밤에 들어도 좋구나...)
	std::vector< StructState::StateCode >		vGoodStates;
	bool										bHasSequantialStateRemover = false;	// 지속효과를 제거하는 지속효과가 두개 이상 있을 수 있고, 순서대로 지우는 녀석이 있을 경우 먼저 처리해줘야 하므로...

	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		assert( (*it).GetLastProcessedTime() > 0 );

		if( (*it).GetEffectType() == StructState::EF_REMOVE_GOOD_STATE )
		{
			AR_TIME nThisFireTime = (*it).GetLastProcessedTime() + (*it).GetFireInterval();

			if( nThisFireTime >= t || nThisFireTime > (*it).GetEndTime() )
				continue;

			if( (*it).GetValue( 5 ) == 0 )				// sequantial remover
			{
				bHasSequantialStateRemover = true;
			}

			vGoodStateRemover.push_back( (*it) );
		}
		else if( !(*it).IsHarmful() && ( (*it).GetTimeType() & StateInfo::ERASE_ON_DEAD ) )	// must not contain remover
		{
			vGoodStates.push_back( (*it).GetCode() );
		}

		if( (*it).GetEffectType() == StructState::EF_ADD_REGION_STATE )
		{
			AR_TIME t = GetArTime();

			if( (*it).GetLastProcessedTime() + (*it).GetValue(1) * 100 > t )
			{
				continue;
			}

			(*it).SetLastProcessedTime( t );

			StructState::StateCode code = static_cast< StructState::StateCode >( (int)(*it).GetValue(0) );
			int nLevel = (*it).GetLevel();
			int nHitRate = (*it).GetValue(8) + nLevel * (*it).GetValue(9);
			AR_TIME end_time = t + ( (*it).GetValue(2) + nLevel * (*it).GetValue(3) ) * 100.0;

			AR_HANDLE hCaster = (*it).GetCaster();
			if( !hCaster )
				continue;

			StructCreature *pCaster = static_cast< StructCreature * >( GameObject::raw_get( hCaster ) );
			if( !pCaster )
				continue;

			if( (*it).GetValue(12) == StructState::USE_TARGET_REGION_TYPE )
			{
				float fEffectLength = (*it).GetValue(4) * GameRule::DEFAULT_UNIT_SIZE;
				int nTargetType = (*it).GetValue(7);

				std::vector< StructCreature * > vTargetList;

				EnumSkillTargetsAndCalcDamage( GetCurrentPosition( t ), GetLayer(), GetCurrentPosition( t ), true, fEffectLength, -1, 0, 0, true, pCaster, (*it).GetValue(5), (*it).GetValue(6), vTargetList, false );

				for( std::vector< StructCreature * >::iterator itTarget = vTargetList.begin() ; itTarget != vTargetList.end() ; ++itTarget )
				{
					StructCreature *pTarget = (*itTarget);
					if( !pTarget || pTarget->IsDead() ) continue;

					// 효과 대상 1 : 동료 & 중립
					if( nTargetType == 1 && IsEnemy( pTarget, true ) )
						continue;

					// 효과 대상 2 : 동료
					else if( nTargetType == 2 && !IsAlly( pTarget ) )
						continue;

					// 효과 대상 3 : 적
					else if( nTargetType == 3 && !IsEnemy( pTarget, true ) )
						continue;

					// 성공률 체크
					if( nHitRate < XRandom( 0, 99 ) )
						continue;

					add_state_info.code = code;
					add_state_info.nLevel = nLevel;
					add_state_info.nEndTime = end_time;
					add_state_info.nHate = (*it).GetValue(10) + nLevel * (*it).GetValue(11);
					add_state_info.pTarget = pTarget;

					vAddStateInfo.push_back( add_state_info );
				}
			}
			else if( (*it).GetValue(12) == StructState::USE_TARGET_OWNER )
			{
				// 성공률 체크
				if( nHitRate < XRandom( 0, 99 ) )
					continue;

				add_state_info.code = code;
				add_state_info.nLevel = nLevel;
				add_state_info.nEndTime = end_time;
				add_state_info.nHate = (*it).GetValue(10) + nLevel * (*it).GetValue(11);
				add_state_info.pTarget = this;

				vAddStateInfo.push_back( add_state_info );
			}
		}
	}

	struct lessGoodStateRemover
	{
		bool operator()(StructState & lh, StructState & rh )
		{
			if( lh.GetValue( 5 ) == 0 && rh.GetValue( 5 ) != 0 )
				return true;

			return false;
		}
	};

	if( vGoodStateRemover.size() )
	{
		if( bHasSequantialStateRemover )
		{
			std::sort( vGoodStateRemover.begin(), vGoodStateRemover.end(), lessGoodStateRemover() );

			std::reverse( vGoodStates.begin(), vGoodStates.end() ); // 뒤에서부터 뽑아야 하니 한번 뒤집어 놓는다.
		}

		for( std::vector< StructState >::iterator iter = vGoodStateRemover.begin(); iter != vGoodStateRemover.end(); ++iter )
		{

			if( !vGoodStates.size() || ( (*iter).GetValue( 0 ) + (*iter).GetValue( 1 ) * (*iter).GetLevel() <= XRandom( 0, 99 ) ) )
				break;

			if( (*iter).GetValue( 5 ) != 0 )
			{
				std::random_shuffle( vGoodStates.begin(), vGoodStates.end() );
			}

			int nRemoveCount = (*iter).GetValue( 3 ) + (*iter).GetValue( 4 ) * (*iter).GetLevel();

			for( int i = 0; i < nRemoveCount; ++i )
			{
				if( !vGoodStates.size() )
					break;

				RemoveState( vGoodStates.back() );

				vGoodStates.pop_back();
			}
		}
	}

	// EF_ADD_REGION_STATE 유형 결과 리스트 처리
	if( !vAddStateInfo.empty() )
	{
		for( std::vector< ADD_STATE_INFO >::iterator it = vAddStateInfo.begin() ; it != vAddStateInfo.end() ; ++it )
		{
			if( (*it).pTarget->IsDead() || !(*it).pTarget->IsInWorld() )
				continue;

			(*it).pTarget->AddState( (*it).code, GetHandle(), (*it).nLevel, t, (*it).nEndTime );
			if( (*it).pTarget->IsMonster() )
				static_cast< StructMonster * >( (*it).pTarget )->AddHate( GetHandle(), (*it).nHate );
		}
	}
}

void StructCreature::procStateDamage( AR_TIME t )
{
	// 직접 DealDamage 를 하면 DealDamage 내부에서 RemoveState() 를 하기도 하기 때문에 낭패.
	// StateDamage는 여기서는 힐의 경우에는 힐량을 데미지로 보관하기도 함.
	std::vector< StateDamage >	vDamageList;
	std::vector< StateDamage >::iterator	vit;

	bool bCreatureNeedNormalPriority = false;
	bool bNeedNormalPriority = false;

	// 뇌기 폭주 처리 필요 여부 조사
	bool bNeedToProcLightningForceCongestion = false;

	struct ByDeadCreature
	{
		ByDeadCreature( const StructCreature* target ) : target( target ) {}
		const bool operator() ( const StructState& state )
		{
			if( target->IsPlayer() || target->IsSummon() )
			{
				StructCreature::iterator itCaster = StructCreature::get( state.GetCaster() );

				if( !(*itCaster) &&
					state.IsHarmful() &&
				    state.GetCode() != StructState::GAIA_MEMBER_SHIP &&
				    state.GetCode() != StructState::NEMESIS &&
				    state.GetCode() != StructState::NEMESIS_FOR_AUTO &&
					state.GetCode() != StructState::CARELESSNESS &&
				    state.GetCode() != StructState::FALL_FROM_SUMMON )
				{
					return true;
				}
			}

			return false;
		}

		const StructCreature* target;
	} byDeadCreature( this );

	RemoveStateIf( byDeadCreature );

	STATE_ITERATOR it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		// 이번에 발동될 시각 미리 계산
		AR_TIME nThisFireTime = (*it).GetLastProcessedTime() + (*it).GetFireInterval();

		if( nThisFireTime >= t || nThisFireTime > (*it).GetEndTime() )
			continue;

		// 뇌기 폭주면 처리 필요 플래그 체크
		if( (*it).GetCode() == StructState::LIGHTNING_FORCE_CONGESTION )
			bNeedToProcLightningForceCongestion = true;

		// 기본 효과가 0이면 데미지를 입히는 효과는 없음
		int nBaseEffectID = (*it).GetBaseEffectID();
		if( !nBaseEffectID )
			continue;

		bNeedNormalPriority = true;

		int nDamageHP = 0;
		int nDamageMP = 0;
		Elemental::Type elem = static_cast< Elemental::Type >( (*it).GetElementalType() );

		switch( nBaseEffectID )
		{
		case StructState::EF_PHYSICAL_STATE_DAMAGE:
		case StructState::EF_PHYSICAL_IGNORE_DEFENCE_STATE_DAMAGE:
		case StructState::EF_MAGICAL_STATE_DAMAGE:
		case StructState::EF_MAGICAL_IGNORE_RESIST_STATE_DAMAGE:
		case StructState::EF_HEAL_HP_BY_MAGIC:
			nDamageHP = (*it).GetBaseDamage();
			nDamageHP = nDamageHP * ( (*it).GetAmplifyBase() + (*it).GetAmplifyPerSkillLevel() * (*it).GetLevel() ) + (*it).GetAddDamageBase() + (*it).GetAddDamagePerSkillLevel() * (*it).GetLevel();
			break;

		case StructState::EF_HEAL_MP_BY_MAGIC:
			nDamageMP = (*it).GetBaseDamage();
			nDamageMP = nDamageMP * ( (*it).GetAmplifyBase() + (*it).GetAmplifyPerSkillLevel() * (*it).GetLevel() ) + (*it).GetAddDamageBase() + (*it).GetAddDamagePerSkillLevel() * (*it).GetLevel();
			break;

		case StructState::EF_HEAL_HP_BY_ITEM:
			nDamageHP = (*it).GetAddDamageBase() + (*it).GetAddDamagePerSkillLevel() * (*it).GetLevel();
			break;

		case StructState::EF_HEAL_MP_BY_ITEM:
			nDamageMP = (*it).GetAddDamageBase() + (*it).GetAddDamagePerSkillLevel() * (*it).GetLevel();
			break;

		case StructState::EF_HEAL_HPMP_BY_ITEM:
			nDamageHP = (*it).GetAddDamageBase() + (*it).GetAddDamagePerSkillLevel() * (*it).GetLevel();
			nDamageMP = nDamageHP;
			break;

		case StructState::EF_HEAL_HPMP_PER_BY_ITEM:
			nDamageHP = ( (*it).GetValue(0) + (*it).GetValue(1) * (*it).GetLevel() ) * GetMaxHP();
			nDamageMP = ( (*it).GetValue(3) + (*it).GetValue(4) * (*it).GetLevel() ) * GetMaxMP();
			break;

		case StructState::EF_PHYSICAL_IGNORE_DEFENCE_PER_STATE_DAMAGE:
			nDamageHP = ( (*it).GetValue(0) + (*it).GetValue(1) * (*it).GetLevel() ) * GetMaxHP();
			nDamageMP = ( (*it).GetValue(2) + (*it).GetValue(3) * (*it).GetLevel() ) * GetMaxMP();
			break;

		default:
			assert( 0 );
			continue;
		}

		if( !nDamageHP && !nDamageMP )
			continue;

		(*it).SetLastProcessedTime( nThisFireTime );

		bool bFinal = nThisFireTime + (*it).GetFireInterval() > (*it).GetEndTime();

		vDamageList.push_back( StateDamage( (*it).GetCaster(), elem, nBaseEffectID, (*it).GetCode(), (*it).GetLevel(), nDamageHP, nDamageMP, bFinal ) );
	}

	// 데미지/힐을 주는 상태이상이 있을 경우
	if( !vDamageList.empty() )
	{
		// priority가 normal 미만인 경우 normal로 설정(단, 로그아웃을 위해 IDLE이 된 경우에는 제외)
		if( GetFinalPriority() < UPDATE_PRIORITY_HIGH && GetFinalPriority() != UPDATE_PRIORITY_IDLE )
			ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGH );
	}

	for( vit = vDamageList.begin(); vit != vDamageList.end(); ++vit )
	{
		StructCreature::iterator cit = StructCreature::get( (*vit).caster );

		switch( (*vit).base_effect_id )
		{
			// HP 데미지 종류는 일단 들어가셈
		case StructState::EF_PHYSICAL_STATE_DAMAGE:
		case StructState::EF_PHYSICAL_IGNORE_DEFENCE_STATE_DAMAGE:
		case StructState::EF_MAGICAL_STATE_DAMAGE:
		case StructState::EF_MAGICAL_IGNORE_RESIST_STATE_DAMAGE:
		case StructState::EF_PHYSICAL_IGNORE_DEFENCE_PER_STATE_DAMAGE:
			{
				// 데미지류는 캐스터 없으면 제거
				if( !(*cit) )
				{
					RemoveState( (*vit).code );
					continue;
				}

				int nFlag = 0;
				StructCreature::_DAMAGE damage;
				switch( (*vit).base_effect_id )
				{
				case StructState::EF_PHYSICAL_IGNORE_DEFENCE_STATE_DAMAGE:
				case StructState::EF_PHYSICAL_IGNORE_DEFENCE_PER_STATE_DAMAGE:
					nFlag |= IGNORE_DEFENCE;
				case StructState::EF_PHYSICAL_STATE_DAMAGE:
					nFlag |= IGNORE_AVOID | IGNORE_BLOCK;

					damage = DealPhysicalStateDamage( (*cit), (*vit).damage_hp, (*vit).elemental_type, 0, 0, nFlag, &m_StateStatePenalty );
					break;

				case StructState::EF_MAGICAL_IGNORE_RESIST_STATE_DAMAGE:
					nFlag |= IGNORE_DEFENCE;
				case StructState::EF_MAGICAL_STATE_DAMAGE:
					nFlag |= IGNORE_BLOCK;

					damage = DealStateMagicalDamage( (*cit), (*vit).damage_hp, (*vit).elemental_type, 0, 0, nFlag, &m_StateStatePenalty );
					break;

				default:
					assert(0);
				}

				std::vector< StructState >::iterator it;

				int total_damage = 0;

				for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
				{
					if( (*it).GetCode() == (*vit).code )
					{
						(*it).AddTotalDamage( damage.nDamage );
						total_damage = (*it).GetTotalDamage();

						break;
					}
				}

				TS_SC_STATE_RESULT msg;

				msg.result_type = TS_SC_STATE_RESULT::STATE_DAMAGE_HP;
				msg.target_value = GetHP();

				msg.caster_handle = (*vit).caster;
				msg.target_handle = GetHandle();

				msg.code = (*vit).code;
				msg.level = (*vit).level;
				msg.value = damage.nDamage;
				msg.final = (*vit).final;
				msg.total_amount = total_damage;

				ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
			}
			break;

		case StructState::EF_HEAL_HP_BY_MAGIC:
		case StructState::EF_HEAL_MP_BY_MAGIC:
		case StructState::EF_HEAL_HP_BY_ITEM:
		case StructState::EF_HEAL_MP_BY_ITEM:
		case StructState::EF_HEAL_HPMP_BY_ITEM:
		case StructState::EF_HEAL_HPMP_PER_BY_ITEM:
			{
				int nHealHP = 0;
				int nHealMP = 0;

				if( IsDead() ) // 죽은 상태에서 힐이 들어갈 여지가 있음
				{
					break;
				}

				if( (*vit).base_effect_id == StructState::EF_HEAL_HP_BY_MAGIC )
				{
					nHealHP = Heal( (*vit).damage_hp );
				}
				else if( (*vit).base_effect_id == StructState::EF_HEAL_MP_BY_MAGIC )
				{
					nHealMP = MPHeal( (*vit).damage_mp );
				}
				else if( (*vit).base_effect_id == StructState::EF_HEAL_HP_BY_ITEM )
				{
					nHealHP = HealByItem( (*vit).damage_hp );
				}
				else if( (*vit).base_effect_id == StructState::EF_HEAL_MP_BY_ITEM )
				{
					nHealMP = MPHealByItem( (*vit).damage_mp );
				}
				else if( (*vit).base_effect_id == StructState::EF_HEAL_HPMP_BY_ITEM ||
					(*vit).base_effect_id == StructState::EF_HEAL_HPMP_PER_BY_ITEM )
				{
					nHealHP = HealByItem( (*vit).damage_hp );
					nHealMP = MPHealByItem( (*vit).damage_mp );
				}
				else
					assert( 0 );	// -┏;? 뉘쇼?

				TS_SC_STATE_RESULT msg;

				int total_heal = 0;
				std::vector< StructState >::iterator it;
				for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
				{
					if( (*it).GetCode() == (*vit).code )
					{
						if( nHealHP )
						{
							(*it).AddTotalDamage( nHealHP );
						}
						else if( nHealMP )
						{
							(*it).AddTotalDamage( nHealMP );
						}
						total_heal = (*it).GetTotalDamage();
						break;
					}
				}

				msg.caster_handle = (*vit).caster;
				msg.target_handle = GetHandle();

				msg.code = (*vit).code;
				msg.level = (*vit).level;

				msg.final = (*vit).final;

				if( nHealHP )
				{
					msg.total_amount = total_heal;
					msg.value = nHealHP;
					msg.target_value = GetHP();
					msg.result_type = TS_SC_STATE_RESULT::STATE_HEAL_HP;

					ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
				}

				if( nHealMP )
				{
					// HP, MP 모두 차는 건 MP 메시지에는 total을 0으로,
					// MP만 차는 건 MP 메시지라도 total을 계산해서 보냄
					if( msg.total_amount )
						msg.total_amount = 0;
					else
						msg.total_amount = total_heal;

					msg.value = nHealMP;
					msg.target_value = GetMP();
					msg.result_type = TS_SC_STATE_RESULT::STATE_HEAL_MP;

					ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
				}
			}
			break;
		}
	}

	// 뇌기 폭주 처리
	if( bNeedToProcLightningForceCongestion )
	{
		do
		{
			StructState *pState = GetState( StructState::LIGHTNING_FORCE_CONGESTION );
			if( !pState )
				break;

			StructCreature::iterator itCaster = StructCreature::get( pState->GetCaster() );
			int damage_hp = pState->GetValue(6) + pState->GetLevel() * pState->GetValue(7);
			int damage_mp = pState->GetValue(8) + pState->GetLevel() * pState->GetValue(9);

			if( !(*itCaster) || GetHP() < damage_hp || GetMP() < damage_mp )
			{
				RemoveState( pState->GetCode() );
				break;
			}

			AR_TIME nThisFireTime = pState->GetLastProcessedTime() + pState->GetFireInterval();

			TS_SC_STATE_RESULT msg;

			msg.caster_handle = pState->GetCaster();
			msg.target_handle = GetHandle();
			msg.final = nThisFireTime + pState->GetFireInterval() > pState->GetEndTime();
			msg.code = StructState::LIGHTNING_FORCE_CONGESTION;
			msg.level = pState->GetLevel();

			pState->SetLastProcessedTime( nThisFireTime );

			if( damage_hp )
			{
				StructCreature::_DAMAGE damage = DealPhysicalStateDamage( (*itCaster), damage_hp, static_cast< Elemental::Type >( pState->GetElementalType() ), 0, 0, 0, &m_StateStatePenalty );
				if( damage.nDamage )
				{
					pState->AddTotalDamage( damage.nDamage );

					msg.value = damage.nDamage;
					msg.target_value = GetHP();
					msg.result_type = TS_SC_STATE_RESULT::STATE_DAMAGE_HP;
					msg.total_amount = pState->GetTotalDamage();

					ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
				}
			}

			if( damage_mp )
			{
				AddMP( -damage_mp );

				msg.value = damage_mp;
				msg.target_value = GetMP();
				msg.result_type = TS_SC_STATE_RESULT::STATE_DAMAGE_MP;
				msg.total_amount = damage_mp;

				ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
			}

		} while( false );
	}
}

void StructCreature::OnUpdate()
{
	AR_TIME t = GetArTime();

	assert( !IsInWorld() || ArcadiaServer::Instance().IsLocked( this ) );

	if( isNeedToCalculateStat() )
	{
		CalculateStat();
		m_StatusFlag.Off( STATUS_NEED_TO_CALCULATE_STAT );
	}

	// { energy process
	int nEnergyCnt = GetEnergyCount();
	if( nEnergyCnt )
	{
		int pos, cnt;

		cnt = 0;

		for( pos = m_nEnergyStartPos; m_arEnergy[pos] < t; pos = ++pos % GameRule::ENERGY_MAX )
		{
			if( !m_arEnergy[pos] || cnt == nEnergyCnt )
				break;

			++cnt;
		}

		if( cnt )
			RemoveEnergy( cnt );
	}
	// } energy process

	regenHPMP( t );

	// 장비로 인해 지속적으로 거는 지속 효과 적용
	// 동작 방식으로 보아 유사 오오라로 취급. 단, 오오라와 같이 주변에 대상에게도 지속효과를 건다면 AddState를 직접 호출해서는 안 됨
	if( m_vPendStateListByItem.size() > 0 )
	{
		// m_vPendStateListByItem는 AddState 안에서 CalculateStat을 호출하면 초기화되었다가 다시 세팅되는데,
		// 대개는 해당 벡터의 내용이 그대로 다시 채워지기 때문에 문제가 없지만 vector 순회 중에 포함된 데이터가 변경되는 건
		// 위험한 상태이기 때문에 m_vPendStateListByItem를 복사한 vPendStateListByItem를 만들어서 순회함
		// * AddState가 반드시 호출된다는 보장도 없고, 호출되더라도 CalculateStat이 호출되지 않는 경우도 있기 때문에
		//   m_vPendStateListByItem와 vPendStateListByItem를 swap 시키거나 m_vPendStateListByItem.clear()를 쓰면 안 됨(다음번 OnUpdate에서 바보됨)
		std::vector< std::pair< AR_TIME, StructState > > vPendStateListByItem( m_vPendStateListByItem );

		for( std::vector< std::pair< AR_TIME, StructState > >::iterator it = vPendStateListByItem.begin(); it != vPendStateListByItem.end(); ++it )
		{
			StructState & State = (*it).second;

			if( (*it).first + SkillBase::TOGGLE_REFRESH_TIME <= t )
			{
				AddState( State.GetCode(), State.GetCaster(), State.GetLevel(), t, t + State.GetEndTime() - State.GetStartTime(), State.IsAura(), State.GetStateValue(), State.GetStateStringValue(), State.IsByEvent() );
				(*it).first = t;
			}
		}
	}

	// 오오라 적용
	if( m_vAura.size() > 0 )
	{
		std::vector< std::pair<StructSkill *, int> >::iterator it;
		for( it = m_vAura.begin(); it != m_vAura.end(); )
		{
			if( (*it).first->GetAuraRefreshTime() + SkillBase::TOGGLE_REFRESH_TIME <= t )
			{
				if( !onProcAura( (*it).first, (*it).second ) )
				{
					TS_SC_AURA msg;

					msg.caster = GetHandle();
					msg.skill_id = (*it).first->GetSkillId();

					msg.status = false;

					if( IsPlayer() )
					{
						PendMessage( this, &msg );
					}
					else if( IsSummon() )
					{
						StructSummon * pSummon = static_cast< StructSummon * >( this );
						if( pSummon->GetMaster() && pSummon->IsInWorld() )
							PendMessage( pSummon->GetMaster(), &msg );
					}

					it = m_vAura.erase( it );
					continue;
				}
				(*it).first->SetAuraRefreshTime( t );
			}

			++it;
		}
	}

	// 이하는 월드에 있을 때만 처리되어야 하는 내용들(방송을 동반하므로 -_ -)
	if( !IsInWorld() )
		return;

	if( m_nPendedClearStateFlag )
	{
		int nClearStateFlag = m_nPendedClearStateFlag;
		m_nPendedClearStateFlag = 0;

		removeStateWithFlag( nClearStateFlag );
	}

	if( m_vStateList.size() && ClearExpiredState( t ) )
	{
		CalculateStat();
	}

	if( IsFeared() && ( !IsMoving( GetArTime() ) || !isFearMoving() ) )
	{
		StructState * pState = GetState( StructState::FEAR );

		if( pState )
		{
			int nMoveSpeedAdd = pState->GetValue( 1 );

			ArPosition newPos;

			float theta = ( XRandom() % 628 ) / 100.0f;

			newPos.x = GetX() + sin( theta ) * 120;
			newPos.y = GetY() + cos( theta ) * 120;

			if( !isFearMoving() )
			{
				m_StatusFlag.On( STATUS_MOVING_BY_FEAR );

				AR_HANDLE hCaster = pState->GetCaster();
				StructCreature::iterator itCaster = StructCreature::get( hCaster );
				StructCreature * pCaster = (*itCaster);
				if( pCaster )
				{
					ArPosition caster_pos = pCaster->GetPos();
					ArPosition my_pos = GetCurrentPosition( GetArTime() );
					AR_UNIT distance = my_pos.GetDistance( caster_pos );

					if( distance > 0 )
					{
						newPos.x = my_pos.x + ( caster_pos.x - my_pos.x ) * 120 / distance;
						newPos.y = my_pos.y + ( caster_pos.y - my_pos.y ) * 120 / distance;
					}
				}
			}

			if( newPos.x > g_nMapWidth ) newPos.x = g_nMapWidth;
			if( newPos.y > g_nMapWidth ) newPos.y = g_nMapWidth;
			if( newPos.x < 0 ) newPos.x = 0;
			if( newPos.y < 0 ) newPos.y = 0;

			if( GameRule::bMonsterCollisionToLine == false || GameContent::CollisionToLine( GetX(), GetY(), newPos.x, newPos.y ) == false )
			{
				if( IsInWorld() && GetRealMoveSpeed() )
				{
					ArcadiaServer::Instance().SetMove( this, GetPos(), newPos, GetRealMoveSpeed() + nMoveSpeedAdd / 7, true, GetArTime() );
				}
			}
		}
	}

	if( IsNeedToBroadcastStatusFlag() )
	{
		SetNeedToBroadcastStatusFlag( false );

		BroadcastStatusMessage( this );
	}
}

bool StructCreature::onProcAura( struct StructSkill *pSkill, int nRequestedSkillLevel )
{
	pSkill->SetRequestedSkillLevel( nRequestedSkillLevel );
	bool ret = pSkill->ProcAura();
	pSkill->SetRequestedSkillLevel( 0 );

	return ret;
}

StructState * StructCreature::GetState( StructState::StateCode code )
{
	StructState * pRtn = NULL;

	STATE_ITERATOR it;
	for( it = m_vStateList.begin() ; it != m_vStateList.end() ; ++it )
	{
		if( (*it).GetCode() == code ) return &(*it);
	}
	return NULL;
}

const StructState * StructCreature::GetState( StructState::StateCode code ) const
{
	StructState * pRtn = NULL;

	STATE_CONST_ITERATOR it;
	for( it = m_vStateList.begin() ; it != m_vStateList.end() ; ++it )
	{
		if( (*it).GetCode() == code ) return &(*it);
	}
	return NULL;
}

int StructCreature::AllocStateUID( int & nDBEnable )
{
	int nUID = 0;

	if( s_vDisabledStateList.size() )
	{
		THREAD_SYNCHRONIZE( s_csDisabledStateListLock );

		if( s_vDisabledStateList.size() )
		{
			// 사용한 UID가 재사용되도록 유도하기 위해 위해 반환된 값을 먼저 사용한다.
			// UID가 재사용될 수 있다는 것은 현재 DB의 Enable 값이 false라는 의미이다.
			nUID = s_vDisabledStateList.back();
			nDBEnable = StructState::UNUSED_IN_DB;

			s_vDisabledStateList.pop_back();

			return nUID;
		}
	}

	nUID = InterlockedIncrement( &s_nMaxStateUID );
	nDBEnable = StructState::NOT_IN_DB;

	return nUID;
}

void StructCreature::DeallocStateUID( const int nUID )
{
#ifdef _DEBUG
	if( nUID <= 0 )
	{
		assert( 0 );
		return;
	}
#endif

	THREAD_SYNCHRONIZE( s_csDisabledStateListLock );

	s_vDisabledStateList.push_back( nUID );
}

unsigned short StructCreature::AddState( StructState::StateCode code, AR_HANDLE caster, int level, AR_TIME start_time, AR_TIME end_time, bool bIsAura, int nStateValue, const char * szStateValue, const bool bByEvent, const int nAllocatedUID, int nDBEnable )
{
	SetNeedToUpdateState( true );
	// 사망시 사라지지 않는 효과는 죽은채로도 걸 수 있도록 변경
	// 사망 상태에서 접속시 더블 플러스, 무한 스테 세이버 효과를 받지 못 하는 문제가 있었음. 2007-05-18
	//if( IsDead() )
	//	return RESULT_NOT_ACTABLE;

	const StateInfo * pStateInfo = GameContent::GetStateInfo( code );

	if( !pStateInfo )
	{
		GameObject *pObj = GameObject::raw_get( caster );
		if( pObj && pObj->IsPlayer() && static_cast< StructPlayer * >(pObj)->pConnection )
		{
			static_cast< StructPlayer * >(pObj)->pConnection->Close();
		}
		return RESULT_NOT_EXIST;
	}

	// 지속 효과 적용 대상 플래그 체크
	// ( 우선 현재 지속효과는 플레이어, 몬스터, 소환수에게만 가능하다. )
	if( !( pStateInfo->uf_avatar && IsPlayer() ) &&
		!( pStateInfo->uf_monster && IsMonster() ) &&
		!( pStateInfo->uf_summon && IsSummon() ) )
		return RESULT_ACCESS_DENIED;

	STATE_ITERATOR it;

	// 플레이어일 경우 체크되는 조건 검사(이벤트에 의한 강제 지속효과의 경우 체크 안 함)
	if( IsPlayer() && !bByEvent )
	{
		StructPlayer * pPlayer = static_cast< StructPlayer * >( this );

		// 데스매치에서 제한된 지속효과는 걸 수 없음
		if( pPlayer->IsInDeathmatch() && pStateInfo->state_time_type & StateInfo::NOT_ACTABLE_ON_DEATHMATCH )
		{
			return RESULT_NOT_ACTABLE_IN_DEATHMATCH;
		}

		if( pPlayer->IsInStartedCompete( true ) && pStateInfo->state_time_type & StateInfo::NOT_ACTABLE_IN_COMPETE )
		{
			return RESULT_NOT_IN_COMPETE;
		}
	}

	// 사망시 사라지는 효과는 시체한테는 걸 수 없음(이벤트에 의한 강제 지속효과의 경우 체크 안 함)
	if( IsDead() && ( pStateInfo->state_time_type & StateInfo::ERASE_ON_DEAD || pStateInfo->state_time_type & StateInfo::ERASE_ON_RESURRECT ) && !bByEvent )
	{
		return RESULT_ACCESS_DENIED;
	}

	// 일어설 때 사라지는 효과는 일어선 대상에게 걸 수 없음(이벤트에 의한 강제 지속효과의 경우 체크 안 함)
	if( !IsSitDown() && ( pStateInfo->state_time_type & StateInfo::ERASE_ON_STAND_UP ) && !bByEvent )
	{
		return RESULT_NOT_ACTABLE_ON_STAND_UP;
	}

	if( IsMonster() )
	{
		StructMonster * pMonster = static_cast< StructMonster * >( this );

		// 보스에게 사용 불가 지속 효과인지 판별에도 state_time_type이 사용된다. 이름 바꿔야할 듯
		if( ( pStateInfo->state_time_type & StateInfo::NOT_ACTABLE_TO_BOSS ) && pMonster->IsBossMonster() )
		{
			return RESULT_LIMIT_TARGET;
		}

		// 던전 코어, 오토 트랩은 공포에 걸리지 않아야 함(돌아댕기면 이상하니까...)
		if( code == StructState::FEAR && ( pMonster->IsDungeonConnector() || pMonster->IsAutoTrap() ) )
		{
			return RESULT_LIMIT_TARGET;
		}

		if( pMonster->IsRunaway() )
		{
			return RESULT_LIMIT_TARGET;
		}
	}

	// 지속효과가 행동 불능을 야기시키는 경우 스킬 캔슬, 탑승 크리처에서 떨어짐
	if( code == StructState::SLEEP || code == StructState::NIGHTMARE || code == StructState::SEAL ||
		code == StructState::SHINE_WALL || code == StructState::FEAR || code == StructState::STUN ||
		pStateInfo->effect_type == StructState::EF_TRANSFORMATION || 
		( pStateInfo->effect_type == StructState::EF_MEZZ && ( pStateInfo->fValue[0] || pStateInfo->fValue[1] || pStateInfo->fValue[2] || pStateInfo->fValue[3] ) ) )
	{
		if( IsUsingSkill() )
			CancelSkill();

		// 자신이 크리처 탑승 상태의 플레이어 혹은 플레이어를 태운 소환수였으면 낙상 처리
		StructPlayer * pThisPlayer = NULL;

		if( IsPlayer() )
			pThisPlayer = static_cast< StructPlayer * >( this );
		else if( IsSummon() )
		{
			pThisPlayer = static_cast< StructSummon * >( this )->GetMaster();

			// 탑승 상태이긴 했는데 얻어맞은 소환수가 아닌 다른 놈을 탑승 중이었으면 낙상 처리 안 함
			if( pThisPlayer && pThisPlayer->GetRideObject() != this )
				pThisPlayer = NULL;
		}

		if( pThisPlayer )
		{
			// 행동 불능 지속효과를 건 주체가 낙상을 유발한 것으로 처리(데미지가 들어가야 하기 때문)
			StructCreature::iterator itCaster = StructCreature::get( caster );
			StructCreature * pCaster = (*itCaster);

			// 행동 불능 지속효과를 건 주체가 없으면 낙상 처리 안 함(몬스터나 자신이어도 괜찮음. 아예 없지만 않으면 됨)
			if( pCaster )
			{
				if( pThisPlayer )
				{
					if( pThisPlayer->IsRiding() || pThisPlayer->HasRidingState() )
					{
						pThisPlayer->UnMount( StructPlayer::UNMOUNT_FALL, pCaster );

						// 낙상 발생 후 사망 가능. 사망시 사라지는 효과는 시체한테는 걸 수 없음(이벤트에 의한 강제 지속효과의 경우 체크 안 함)
						if( IsDead() && ( pStateInfo->state_time_type & StateInfo::ERASE_ON_DEAD || pStateInfo->state_time_type & StateInfo::ERASE_ON_RESURRECT ) && !bByEvent )
						{
							return RESULT_ACCESS_DENIED;
						}
					}
					else if( pThisPlayer->IsSitDown() )
					{
						pThisPlayer->StandUp();
						BroadcastStatusMessage( pThisPlayer );

						AR_TIME t = GetArTime();
						pThisPlayer->AddState( StructState::CARELESSNESS, 0, 1, t, t + 300 );
					}
				}
			}
		}
	}

	// 지속효과가 크리처 탑승 효과일 경우 은신 해제됨
	if( pStateInfo->effect_type == StructState::EF_RIDING && IsHiding() )
	{
		RemoveState( StructState::HIDE, GameRule::MAX_STATE_LEVEL );
		RemoveState( StructState::TRACE_OF_FUGITIVE, GameRule::MAX_STATE_LEVEL );
	}

	if( code == StructState::FEAR )
	{
		m_StatusFlag.Off( STATUS_MOVING_BY_FEAR );
	}

	StructCreature::iterator itCaster;
	itCaster = StructCreature::get( caster );

	int base_damage = 0;

	if( (*itCaster) )
	{
		switch( pStateInfo->base_effect_id )
		{
		case StructState::EF_PHYSICAL_STATE_DAMAGE:
		case StructState::EF_PHYSICAL_IGNORE_DEFENCE_STATE_DAMAGE:
		case StructState::EF_PHYSICAL_IGNORE_DEFENCE_PER_STATE_DAMAGE:
			base_damage = (*itCaster)->GetAttackPointRight( static_cast< Elemental::Type >(pStateInfo->elemental_type), true, true );
			break;
		case StructState::EF_MAGICAL_STATE_DAMAGE:
		case StructState::EF_MAGICAL_IGNORE_RESIST_STATE_DAMAGE:
		case StructState::EF_HEAL_HP_BY_MAGIC:
		case StructState::EF_HEAL_MP_BY_MAGIC:
			base_damage = (*itCaster)->GetMagicPoint( static_cast< Elemental::Type >(pStateInfo->elemental_type), false, true );
			break;
		}
	}

	bool bNotErasable = pStateInfo->state_time_type & StateInfo::NOT_ERASABLE;

	std::vector< StructState::StateCode > vDeleteStateCode;
	bool bAlreadyExist = false;

	for( it = m_vStateList.begin() ; it != m_vStateList.end() ; ++it )
	{
		bool bIsDuplicatedGroup = false;
		if( code == (*it).GetCode() )
		{
			bAlreadyExist = true;
			bIsDuplicatedGroup = true;

			// 중첩 가능 횟수(reiteration_count)가 2회 이상일 경우, 중첩이 가능한 지속효과로 판단한다.
			// 중첩 횟수는 사실상 레벨과 동일한 개념이며, 기존의 지속효과에 새로운 지속효과의 레벨을 더해 중첩을 표현한다.
			// 따라서 한 번에 더해지는 중첩 횟수는 개념상 1회이며, 이에 따라 중첩 가능한 지속효과들은 1레벨 단위로만 걸릴 수 있다.
			if( pStateInfo->reiteration_count >= 2 )
				level = std::min( static_cast< int >( pStateInfo->reiteration_count ), (*it).GetLevel() + level );
		}
		else
		{
			for( int i = 0 ; i < 3 ; ++i )
			{
				if( (*it).IsDuplicatedGroup( pStateInfo->duplicate_group[i] ) )
				{
					bIsDuplicatedGroup = true;
					break;
				}
			}
		}

		if( bIsDuplicatedGroup )
		{
			// 이벤트에 의한 강제 지속효과의 경우 무조건 기존 버프 삭제(동일한 지속효과가 있었다고 해도 업데이트 아니고 강제로 삭제)
			// 이벤트에 의한 강제 지속효과가 아니고 기존에 중복되어 있던 지속효과가 이벤트에 의한 강제 지속효과일 경우는 아무것도 못 함
			if( bByEvent )
			{
				bAlreadyExist = false;
				vDeleteStateCode.push_back( (*it).GetCode() );
				continue;
			}
			else if( (*it).IsByEvent() )
			{
				return RESULT_ALREADY_EXIST;
			}

			bool bNotErasableCur = (*it).GetTimeType() & StateInfo::NOT_ERASABLE;

			// 기존, 새 지속효과 모두 소멸 안 됨 플래그 없거나 있음
			if( bNotErasable == bNotErasableCur )
			{
				// 그룹 같고 둘다 플래그 걸려 있으면 같은 지속효과여야 함.
				// 만일 같지 않은 지속효과가 추가되어야 할 경우가 생기면
				// 플래그 있을 때와 없을 때 분리해야 함
				if( ( (*it).GetLevel() > level ) || ( (*it).GetLevel() == level && (*it).GetEndTime() > end_time ) )
				{
					return RESULT_ALREADY_EXIST;
				}

				// 같은 코드이면 기존의 지속효과를 업데이트 시키도록 해야 함(토글형의 갱신 문제)
				if( code == (*it).GetCode() )
				{
					continue;
				}

				vDeleteStateCode.push_back( (*it).GetCode() );
			}
			// 새 지속효과에만 소멸 안 됨 플래그 있음
			else if( bNotErasable )
			{
				vDeleteStateCode.push_back( (*it).GetCode() );
				continue;
			}
			// 기존 지속효과에만 소멸 안 됨 플래그 있음
			else
			{
				return RESULT_ALREADY_EXIST;
			}
		}
	}

	for( std::vector< StructState::StateCode >::iterator dit = vDeleteStateCode.begin() ; dit != vDeleteStateCode.end() ; ++dit )
	{
		RemoveState( *dit );
	}

	if( bAlreadyExist )
	{
		for( it = m_vStateList.begin() ; it != m_vStateList.end() ; ++it )
		{
			if( code == (*it).GetCode() )
			{
				// 변화가 있다면 스탯 재계산 및 방송
				if( (*it).AddState( caster, level, start_time, end_time, base_damage, bIsAura ) )
				{
					CalculateStat();
					onUpdateState( (*it), false );

					onAfterAddState( (*it) );
				}
				else if( !bIsAura )
				{
					onUpdateState( (*it), false );
				}

				break;
			}
		}

		return RESULT_SUCCESS;
	}

	int nUID = -1;

	// EventState 지속효과나 로그아웃 시 삭제되는 지속효과와 같이 저장이 필요없는 지속효과는 UID를 할당하지 않는다. 
	// 여기서 UID가 부여되지 않는다면 UID의 존재 여부로 지속효과의 저장 필요 여부를 판단할 수 있어 효율적이다.
	if( ( !IsPlayer() && !IsSummon() ) ||
		bByEvent ||
		pStateInfo->state_time_type & StateInfo::ERASE_ON_LOGOUT )
	{
		nUID = -1;
	}
	else
	{
	// UID 할당이 필요한 경우에도 미리 할당된 UID를 사용한다면 새로 발급할 필요가 없다.
	// 실제 저장되지 않고 만료되는 지속효과가 많기에 대부분의 UID 발급이 무의미하지만, Save에 의해 이미 부여된 지속효과의 UID가 변경되는 경우는 발생해서는 안 된다.
	// 이는 UID가 클라이언트로 방송될 때 handle로 활용되기 때문인데 handle이라고 해서 반드시 고유값일 필요는 없고 변경만 발생하지 않으면 문제가 없다.
		// 예를 들어 위와 같이 DB에 저장이 필요없는 경우 UID를 -1로 일괄 부여하더라도 UID가 변경되지 않기 때문에 클라이언트에서 문제 없이 처리할 수 있다.
		if( nAllocatedUID )
			nUID = nAllocatedUID;
		else
			nUID = StructCreature::AllocStateUID( nDBEnable );
	}

	m_vStateList.push_back( StructState( code, nUID, caster, level, start_time, end_time, base_damage, bIsAura, nStateValue, szStateValue, bByEvent, nDBEnable ) );

	CalculateStat();

	onUpdateState( m_vStateList.back(), false );

	if( IsMonster() && !IsMovable() )
	{
		if( m_Attribute.fAttackRange < GameRule::MAX_ATTACK_RANGE * GameRule::ATTACK_RANGE_UNIT )
		{
			m_Attribute.fAttackRange = GameRule::MAX_ATTACK_RANGE * GameRule::ATTACK_RANGE_UNIT;
		}
	}

	onAfterAddState( m_vStateList.back() );

	return RESULT_SUCCESS;
}

void StructCreature::PendAddState( StructState::StateCode code, AR_HANDLE caster, int level, AR_TIME start_time, AR_TIME end_time, bool bIsAura, int nStateValue, const char * szStateValue, const bool bByEvent )
{
	m_vPendStateList.push_back( StructState( code, -1, caster, level, start_time, end_time, -1, bIsAura, nStateValue, szStateValue, bByEvent, StructState::NOT_IN_DB ) );
}

void StructCreature::PendAddStateByItem( StructState::StateCode code, AR_HANDLE caster, int level, AR_TIME start_time, AR_TIME end_time, bool bIsAura, int nStateValue, const char * szStateValue, const bool bByEvent )
{
	m_vPendStateListByItem.push_back( std::make_pair( 0, StructState( code, -1, caster, level, start_time, end_time, -1, bIsAura, nStateValue, szStateValue, bByEvent, StructState::NOT_IN_DB ) ) );
}

void StructCreature::onUpdateState( StructState & state, bool bIsExpire )
{
	BroadcastStateMessage( this, &state, bIsExpire );
}

void StructCreature::RemoveState( StructState::StateCode code, int state_level, const bool bByEvent )
{
	bool bFlag = false;

	STATE_ITERATOR it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); )
	{
		if( (*it).GetCode() == code && (*it).GetLevel() <= state_level && ( !(*it).IsByEvent() || bByEvent ) )
		{
			bFlag = true;			
			break;
		}

		++it;
	}

	if( !bFlag ) return;

	if( (*it).GetLevel() <= state_level ) 
	{
		StructState state = (*it);

		onUpdateState( (*it), true );
		m_vStateList.erase( it );
		CalculateStat();

		onAfterRemoveState( state );
	}
}

template< typename Predicate >
void StructCreature::RemoveStateIf( Predicate& predicate, std::vector< StructState >* result, const bool bByDead )
{
	std::vector< StructState > removedStates;
	std::vector< StructState >::iterator it;
	std::vector< StructState >::iterator trail;

	// 필요한 경우 삭제된 state들을 반환하기 위해 우선 스왑
	if( result != NULL )
	{
		removedStates.swap( *result );
	}

	// predicate이 true인 값은 removedStates로 이동시키고 false인 값은 m_vStateList에 남겨둠
	for ( it = m_vStateList.begin(), trail = it; it != m_vStateList.end(); ++it )
	{
		if ( predicate( *it ) )
		{
			removedStates.push_back( *it );
		}
		else
		{
			if( trail != it )
			{
				*trail = *it;
			}
			++trail;
		}
	}

	m_vStateList.resize( trail - m_vStateList.begin() );

	// 삭제 대상인 녀석들을 대상으로 사후 처리
	for ( it = removedStates.begin(); it != removedStates.end(); ++it )
	{
		onUpdateState( (*it), true );
		onAfterRemoveState( *it, bByDead );
	}

	// 삭제된 지속효과가 하나라도 있다면 CalculateStat을 호출한다.
	if( removedStates.size() )
	{
		CalculateStat();
	}

	// 삭제된 state들을 추가한 vector를 반환해주자
	if( result != NULL )
	{
		removedStates.swap( *result );
	}
}

void StructCreature::RemoveStatesOnDamaged()
{	
	RemoveStateIf( StateFlagChecker( StateInfo::ERASE_ON_DAMAGED ) );
}

void StructCreature::DecreaseState( StructState::StateCode code, const unsigned char level, const bool bByEvent )
{
	struct CheckStateCode
	{
		CheckStateCode( StructState::StateCode code, bool byEvent ) : code( code ), byEvent( byEvent ) {}
		const bool operator () ( const StructState & state ) const
		{
			if( state.GetCode() != code || state.IsByEvent() ^ byEvent )
				return false;

			return true;
		}
		StructState::StateCode code;
		bool byEvent;
	} checkStateCode( code, bByEvent );
	DecreaseStateIf( checkStateCode, level );
}

template< typename Predicate >
void StructCreature::DecreaseStateIf( Predicate& predicate, const unsigned char level, std::vector< StructState >* result )
{
	bool bDecreased = false; 
	std::vector< StructState > removedStates;
	std::vector< StructState >::iterator it;
	std::vector< StructState >::iterator trail;

	// 필요한 경우 삭제된 state들을 반환하기 위해 우선 스왑
	if( result != NULL )
	{
		removedStates.swap( *result );
	}

	// predicate이 true인 값은 지속 레벨을 감소시키고 만약 레벨이 0이 된다면 removedStates로 이동시킨다.
	for ( it = m_vStateList.begin(), trail = it; it != m_vStateList.end(); ++it )
	{
		if ( predicate( *it ) )
		{
			bDecreased = true;

			(*it).SetLevel( (*it).GetLevel() > level ? (*it).GetLevel() - level : 0 );

			// 감소된 지속효과 레벨이 0이면 삭제
			if( (*it).GetLevel() == 0 )
			{
				removedStates.push_back( *it );
				continue;
			}

			// 삭제되지 않은 지속효과라면 변경된 레벨을 방송
			onUpdateState( *it );
		}

		if( trail != it )
		{
			*trail = *it;
		}
		++trail;
	}

	m_vStateList.resize( trail - m_vStateList.begin() );

	// 삭제 대상인 녀석들을 대상으로 사후 처리
	for ( it = removedStates.begin(); it != removedStates.end(); ++it )
	{
		onUpdateState( (*it), true );
		onAfterRemoveState( *it );
	}

	// 감소되었거나 삭제된 지속효과가 하나라도 있다면 CalculateStat을 호출한다.
	if( bDecreased || removedStates.size() )
	{
		CalculateStat();
	}

	// 삭제된 state들을 추가한 vector를 반환해주자
	if( result != NULL )
	{
		removedStates.swap( *result );
	}
}

bool StructCreature::ClearExpiredState( AR_TIME t )
{
	struct CheckAndProcessExpiredState {
		CheckAndProcessExpiredState( StructCreature& creature, AR_TIME time )
		: creature( creature ), isExpiredStateExist( false ), time( time ) {}

		const bool operator () ( StructState& state )
		{
			bool erase = false;

			// 이벤트에 의해 부여된 지속효과가 아닐 경우에만 만료 처리 체크 함
			if( !state.IsByEvent() )
			{
				if( state.ClearExpiredState( time ) )  
				{
					isExpiredStateExist = true;
					erase = true;
				}
			}

			return erase;
		}

		StructCreature& creature;
		bool isExpiredStateExist;
		AR_TIME time;
	} checkAndProcessExpiredState( *this, t );
	std::vector< StructState > removedStates;

	RemoveStateIf( checkAndProcessExpiredState, &removedStates );

	bool returnValue = checkAndProcessExpiredState.isExpiredStateExist;

	for ( std::vector< StructState >::iterator it = removedStates.begin(); it != removedStates.end(); ++it )
	{
		// { 추가 작업
		if( (*it).GetCode() == StructState::ADD_ENERGY )
		{
			AddEnergy();
		}
		else if( (*it).GetEffectType() == StructState::EF_PROVOKE && IsMonster() )
		{
			StructMonster *pMonster = static_cast< StructMonster * >( this );

			pMonster->SetNeedToFindEnemy();
		}

		// 지속효과 소멸 시 로그 남겨야 하는 경우 로그 남김
		if( it->IsLogRequiredOnExpiration() )
		{
			StructPlayer * pPlayer = NULL;
			StructSummon * pSummon = NULL;

			if( IsPlayer() )
			{
				pPlayer = static_cast< StructPlayer * >( this );
			}
			else if( IsSummon() )
			{
				pSummon = static_cast< StructSummon * >( this );
				pPlayer = pSummon->GetMaster();
			}

			if( pPlayer && pPlayer->IsLogin() && !pPlayer->IsDeleteRequested() )
			{
				if( pSummon )
				{
					LOG::Log11N4S( LM_STATE_EXPIRATION, pPlayer->GetAccountID(), pPlayer->GetSID(), it->GetCode(), it->GetLevel(),
						pSummon->GetSID(), pSummon->GetSummonCode(), pSummon->GetLevel(), 0, pSummon->GetX(), pSummon->GetY(), pSummon->GetParentCard()->GetItemUID(),
						pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, pSummon->GetName(), LOG::STR_NTS, "", 0 );
				}
				else
				{
					LOG::Log11N4S( LM_STATE_EXPIRATION, pPlayer->GetAccountID(), pPlayer->GetSID(), it->GetCode(), it->GetLevel(),
						0, 0, 0, 0, 0, pPlayer->GetX(), pPlayer->GetY(),
						pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
				}
			}
		}
	}

	return returnValue;
}


void StructCreature::RestoreRemovedStateByDead()
{
	// 사망시 없어진 지속효과를 시작/종료 시각 조정하여 다시 부여
	AR_TIME t = GetArTime();
	for( STATE_ITERATOR it = m_vStateListRemovedByDeath.begin() ; it != m_vStateListRemovedByDeath.end() ; )
	{
		const StructState & state = (*it);

		// 이미 걸려있는 지속효과면 패스
		if( GetState( state.GetCode() ) || ( it->GetTimeType() & StateInfo::ERASE_ON_RESURRECT ) )
		{
			++it;
			continue;
		}

		if( state.GetEndTime() == (AR_TIME)-1 )
		{
			AddState( state.GetCode(), state.GetCaster(), state.GetLevel(), state.GetStartTime(), -1, state.IsAura(), state.GetStateValue(), state.GetStateStringValue(), state.IsByEvent(), state.GetUID(), state.GetDBEnable() );
		}
		else
		{
			AR_TIME time_difference = GetArTime() - GetDeadTime();

			AddState( state.GetCode(), state.GetCaster(), state.GetLevel(), state.GetStartTime() + time_difference, state.GetEndTime() + time_difference, state.IsAura(), state.GetStateValue(), state.GetStateStringValue(), state.IsByEvent(), state.GetUID(), state.GetDBEnable() );
		}

		it = m_vStateListRemovedByDeath.erase( it );
	}

	ClearRemovedStateByDead();
}

void StructCreature::ClearRemovedStateByDead()
{
	for( std::vector< StructState >::iterator it = m_vStateListRemovedByDeath.begin(); it != m_vStateListRemovedByDeath.end(); ++it )
	{
		(*it).Expire( this );
	}

	m_vStateListRemovedByDeath.clear();
}

void StructCreature::RemoveAllAura()
{
	if( m_vAura.size() )
	{
		std::vector< std::pair<StructSkill *, int> >::iterator it;
		for( it = m_vAura.begin(); it != m_vAura.end(); ++it )
		{
			TS_SC_AURA msg;

			msg.caster = GetHandle();
			msg.skill_id = (*it).first->GetSkillId();

			msg.status = false;

			if( IsPlayer() )
			{
				PendMessage( this, &msg );
			}
			else if( IsSummon() )
			{
				PendMessage( static_cast< StructSummon * >( this )->GetMaster(), &msg );
			}
		}
		m_vAura.clear();
	}
}

void StructCreature::RemoveAllStateByDeadOrLogout()
{
	// 소환수 역소환에 의해 호출되는 함수로 실제로는 죽는 것과 관련이 없는 함수이다. 따라서 bByDead를 false로 넘긴다.
	// 또한 소환수의 경우는 부활 시 지속효과 복구 기능이 구현되어 있지 않으며 사용하지도 않는다.
	RemoveStateIf( StateFlagChecker( StateInfo::ERASE_ON_DEAD | StateInfo::ERASE_ON_LOGOUT ) );
}

void StructCreature::RemoveAllStateByQuittingHuntaholic()
{
	RemoveStateIf( StateFlagChecker( StateInfo::ERASE_ON_QUIT_HUNTAHOLIC ) );
}

void StructCreature::RemoveStateByEnteringDeathmatch()
{
	struct DeathMatchFlagChecker
	{
		const bool operator() ( const StructState& state )
		{
			return ( !( state.GetTimeType() & StateInfo::NOT_ERASABLE_ON_ENTER_DEATHMATCH ) ) && !state.IsAura() && !state.IsByEvent();
		}
	} deathMatchFlagChecker;

	RemoveStateIf( deathMatchFlagChecker );
}

void StructCreature::RemoveAllStateByQuittingDeathmatch()
{
	RemoveStateIf( StateFlagChecker( StateInfo::ERASE_ON_QUIT_DEATHMATCH ) );
}

struct BattleArenaStateChecker
{
	static const int REMOVE_FLAG = ( StateInfo::ERASE_ON_DEAD | StateInfo::ERASE_ON_QUIT_BATTLE_ARENA );

	const bool operator()( const StructState & state )
	{
		return ( state.GetTimeType() & REMOVE_FLAG ) != 0;
	}
};

void StructCreature::RemoveAllStateByQuittingBattleArena()
{
	BattleArenaStateChecker basc;

	RemoveStateIf( basc );
}

bool StructCreature::AddAimer( AR_HANDLE aimer )
{
	if( std::find( m_vAimerList.begin(), m_vAimerList.end(), aimer ) != m_vAimerList.end() )
		return false;

	m_vAimerList.push_back( aimer );

	return true;
}

bool StructCreature::RemoveAimer( AR_HANDLE aimer )
{
	std::vector< AR_HANDLE >::iterator it;

	it = std::find( m_vAimerList.begin(), m_vAimerList.end(), aimer );

	if( it == m_vAimerList.end() )
		return false;

	vector_fast_erase( &m_vAimerList, it );

	return true;
}

void StructCreature::ReleaseAimerList()
{
	std::vector< AR_HANDLE >::iterator it;

	for( it = m_vAimerList.begin(); it != m_vAimerList.end(); it++ )
	{
		StructPlayer::iterator itPlayer = StructPlayer::get( *it );
		StructPlayer *pObj = *itPlayer;

		if( pObj )
		{
			pObj->SetTarget( 0 );
		}
	}

	m_vAimerList.clear();
}

bool StructCreature::IsUsingSkill()
{
	return !!m_pCastSkill;
}

bool StructCreature::CancelSkill()
{
	if( !IsUsingSkill() )
		return false;

	if( !m_pCastSkill->Cancel() )
		return false;

	m_pCastSkill = NULL;
	// _oprint("[%s] 스킬 캔슬\n", GetName() );

	return true;
}
bool StructCreature::OnCompleteSkill()
{
	if( !IsUsingSkill() ) return false;

	m_pCastSkill = NULL;
	// _oprint("[%s] 스킬 시전 완료\n", GetName() );

	return true;
}

const unsigned short StructCreature::CastSkill( int nSkillId, int nSkillLevel, AR_HANDLE target_handle, const ArPosition & pos, unsigned char layer, AR_HANDLE hCastItem )
{
	ArPosition target_pos = pos;

	if( IsUsingSkill() )
		return RESULT_NOT_ACTABLE;

	if( IsPlayer() && static_cast< StructPlayer * >( this )->IsUsingStorage() )
		return RESULT_NOT_ACTABLE;

	StructSkill* pSkill = getSkill( nSkillId );
	if( !pSkill ) return RESULT_NO_SKILL;

	GameObject::iterator it = StructCreature::get( target_handle );
	StructCreature *pSkillTarget = NULL;

	if( *it )
	{
		if( (*it)->IsCreature() ) pSkillTarget = static_cast< StructCreature * >( *it );
	}

	// { 대상에게 사용가능한지 체크
	if( pSkill->GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_MASTER )
	{
		if( !IsSummon() ) 
		{
			assert( 0 );
			return RESULT_NOT_ACTABLE;
		}

		StructSummon* pSummon = static_cast< StructSummon* >( this );
		if( pSummon->GetMaster() != pSkillTarget )
		{
			return RESULT_NOT_ACTABLE;
		}
	}
	else if( pSkill->GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_SELF_WITH_MASTER )
	{
		if( !IsSummon() ) 
		{
			assert( 0 );
			return RESULT_NOT_ACTABLE;
		}

		StructSummon* pSummon = static_cast< StructSummon* >( this );
		if( pSkillTarget != this && pSkillTarget != pSummon->GetMaster() )
		{
			return RESULT_NOT_ACTABLE;
		}
	}

	// 자신에게 사용할 수 없는 스킬일 경우 체크
	if( pSkill->GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET_EXCEPT_CASTER &&
		pSkillTarget == this )
	{
		return RESULT_NOT_ACTABLE;
	}

	if( pSkillTarget )
	{
		if( !pSkillTarget->IsInWorld() )
			return RESULT_NOT_ACTABLE;

		//		??-_-??
		//		if( ( pSkill->GetSkillBase()->IsHarmful() && pSkill->IsNeedTarget() && !IsEnemy( pSkillTarget ) ) || ( !pSkill->GetSkillBase()->IsHarmful() && IsEnemy( pSkillTarget ) ) )
		//		{
		//			return false;
		//		}

		AR_TIME t = GetArTime();
		ArPosition enemyPosition = pSkillTarget->GetCurrentPosition( t );
		ArPosition myPosition = GetCurrentPosition( t );
		AR_UNIT enemy_distance = myPosition.GetDistance( enemyPosition ) - GetUnitSize()/2 - pSkillTarget->GetUnitSize()/2;;

		float range_mod = 1.2f;

		if( pSkillTarget->IsMoving() )
		{
			range_mod = 1.5f;

		}

		if( pSkill->GetCastRange() == -1 )
		{
			if( enemy_distance > GetRealAttackRange() * range_mod )
			{
				return RESULT_TOO_FAR;
			}
		}
		else if( enemy_distance > pSkill->GetCastRange() * GameRule::DEFAULT_UNIT_SIZE * range_mod )
		{
			return RESULT_TOO_FAR;
		}

		if( pSkill->GetSkillBase()->IsValidToCorpse() )
		{
			if( !pSkillTarget->IsDead() )
				// _cprint( "스킬 시전 실패 : 살아있는 대상에게 사용 불가\n" );
				return RESULT_NOT_ACTABLE;

			// 죽었더라도 내구도가 다한 소환수는 살릴 수 없음
			else if( pSkillTarget->IsSummon() )
			{
				StructItem *pCardItem = static_cast< StructSummon * >( pSkillTarget )->GetParentCard();

				if( pCardItem && pCardItem->GetMaxEtherealDurability() && !pCardItem->GetCurrentEtherealDurability() )
					return RESULT_NOT_ACTABLE;
			}
		}

		if( pSkillTarget == this || ( pSkillTarget->IsSummon() && static_cast< StructSummon * >( pSkillTarget )->GetMaster() == this ) )
		{
			if( !pSkill->IsUsable( SkillBase::USE_SELF ) )
				return RESULT_NOT_ACTABLE;
		}
		else if( IsAlly( pSkillTarget ) ) 
		{
			if( !pSkill->IsUsable( SkillBase::USE_ALLY ) )
				return RESULT_NOT_ACTABLE;
		}
		else if( IsEnemy( pSkillTarget ) )
		{
			if( !pSkill->IsUsable( SkillBase::USE_ENEMY ) )
				return RESULT_NOT_ACTABLE;
		}
		else
		{
			if( !pSkill->IsUsable( SkillBase::USE_NEUTRAL ) )
				return RESULT_NOT_ACTABLE;
		}

		if( pSkillTarget->IsPlayer() && !pSkill->IsUseableOnAvatar() ) 
		{
			// _cprint( "스킬 시전 실패 : 아바타에게 사용 불가\n" );
			return RESULT_NOT_ACTABLE;
		}
		if( pSkillTarget->IsMonster() && !pSkill->IsUseableOnMonster() )
		{
			// _cprint( "스킬 시전 실패 : 몬스터에게 사용 불가\n" );
			return RESULT_NOT_ACTABLE;
		}
		if( pSkillTarget->IsSummon() && !pSkill->IsUseableOnSummon() )
		{
			// _cprint( "스킬 시전 실패 : 소환수에게 사용 불가\n" );
			return RESULT_NOT_ACTABLE;
		}

		target_pos = pSkillTarget->GetCurrentPosition( GetArTime() );
	}
	// }
	else
	{
		if( nSkillId == StructSkill::SKILL_RETURN_FEATHER && IsPlayer() )
		{
			if( static_cast< StructPlayer * >( this )->IsInSiegeOrRaidDungeon() )
			{
				// _cprintf" 스킬 시전 실패 : 귀환의 깃털은 레이드/시즈 던전 안에서는 사용 불가\n")
				return RESULT_NOT_ACTABLE;
			}
			// 숨겨진 던전 안에서는 귀환의 깃털 스킬을 사용할 수 없도록 되어 있었으나, 사용 가능한 크루 아이템의 추가로 여기서의 체크 제거
			// * 체크를 제거해도 실제로 layer를 별도의 manager에 의해 관리하거나 하는 영역이 아니기 때문에 시스템상 별다른 문제는 없을 걸로 예상됨
			if( static_cast< StructPlayer * >( this )->IsInInstanceDungeon() )
			{
				// _cprintf" 스킬 시전 실패 : 귀환의 깃털은 인스턴스 던전 안에서는 사용 불가\n")
				return RESULT_NOT_ACTABLE_IN_INSTANCE_DUNGEON;
			}
		}
	}

	if( !m_vInterruptedSkill.empty() )
	{
		for( std::vector< int >::iterator it = m_vInterruptedSkill.begin() ; it != m_vInterruptedSkill.end() ; ++it )
		{
			if( (*it) == nSkillId )
			{
				return RESULT_NOT_ACTABLE;
			}
		}
	}

	if( !m_vAllowedSkill.empty() )
	{
		bool bActable = true;
		for( std::vector< std::set< int > >::iterator it = m_vAllowedSkill.begin(); bActable && it != m_vAllowedSkill.end(); it++ )
		{
			if( (*it).find( pSkill->GetSkillId() ) == (*it).end() )
			{
				return RESULT_NOT_ACTABLE;
			}
		}
	}

	SetDirection( target_pos );

	m_pCastSkill = pSkill;
	// _oprint("[%s] 스킬 시전 m_pSkill 설정\n", GetName() );

	int nErrorCode = m_pCastSkill->Cast( nSkillLevel, target_handle, target_pos, layer, hCastItem );

	if( nErrorCode != RESULT_SUCCESS )
	{
		// _oprint("[%s] 스킬 시전 실패\n", GetName() );
		m_pCastSkill = NULL;
		return nErrorCode;
	}

	// _oprint("[%s] 스킬 시전 성공\n", GetName() );
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	// C급 보스 이상의 몬스터가 스킬 시전에 성공한 경우 로그 남김
	if( IsMonster() && static_cast< StructMonster * >( this )->GetMonsterType() >= MonsterBase::MONSTER_TYPE_HIGHEST_1_STAR )
	{
		StructMonster * pMonster = static_cast< StructMonster * >( this );

		if( pSkillTarget )
		{
			StructPlayer * pPlayer = NULL;

			if( pSkillTarget->IsPlayer() )
			{
				pPlayer = static_cast< StructPlayer * >( pSkillTarget );
			}
			else if( pSkillTarget->IsSummon() )
			{
				pPlayer = static_cast< StructSummon * >( pSkillTarget )->GetMaster();
			}

			if( pPlayer )
			{
				LOG::Log11N4S( LM_MONSTER_SKILL_CAST, pPlayer->GetAccountID(), pPlayer->GetSID(), pSkillTarget->GetX(), pSkillTarget->GetY(), pSkillTarget->GetLayer(),
					pMonster->GetMonsterId(), pMonster->GetX(), pMonster->GetY(), pMonster->GetLayer(),
					nSkillId, nSkillLevel,
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
					pSkillTarget->GetName(), LOG::STR_NTS, pMonster->GetName(), LOG::STR_NTS );
			}
			else
			{
				LOG::Log11N4S( LM_MONSTER_SKILL_CAST, 0, 0, pSkillTarget->GetX(), pSkillTarget->GetY(), pSkillTarget->GetLayer(),
					pMonster->GetMonsterId(), pMonster->GetX(), pMonster->GetY(), pMonster->GetLayer(),
					nSkillId, nSkillLevel,
					"", 0, "", 0,
					pSkillTarget->GetName(), LOG::STR_NTS, pMonster->GetName(), LOG::STR_NTS );
			}
		}
		else
		{
			LOG::Log11N4S( LM_MONSTER_SKILL_CAST, 0, 0, 0, 0, 0,
				pMonster->GetMonsterId(), pMonster->GetX(), pMonster->GetY(), pMonster->GetLayer(),
				nSkillId, nSkillLevel,
				"", 0, "", 0,
				"", 0, pMonster->GetName(), LOG::STR_NTS );
		}
	}

	return RESULT_SUCCESS;
}

// 두 벡터의 각도차이
static short getDegreeDiff( const ArMoveVector & mv1, const ArMoveVector & mv2 )
{
	short d1 = mv1.GetDegree();
	short d2 = mv2.GetDegree();

	short diff = ( d1 > d2 ? d1 - d2 : d2 - d1 );
	if( diff < 0 ) diff = 0 - diff;

	return diff;
}

bool StructCreature::IsBackOf( const StructCreature & target )
{
	short diff = getDegreeDiff( target.GetMv(), GetMv() );	

	return ( diff < 180 + 90 / 2 && 180 - 90 / 2 );
}

bool StructCreature::IsSideOf( const StructCreature & target )
{
	short diff = getDegreeDiff( target.GetMv(), GetMv() );

	return	( diff < 90  + 90 / 2 && 90  - 90 / 2 ) || 
		( diff < 270 + 90 / 2 && 270 - 90 / 2 );
}

StructSkill * StructCreature::GetSkill( int nSkillId ) const
{
	std::vector< StructSkill * >::const_iterator	it;
	for( it = m_vAllSkillList.begin(); it != m_vAllSkillList.end(); ++it )
	{
		if( (*it)->GetSkillId() == nSkillId ) return *it;
	}

	return NULL;
}

const StructSkill * StructCreature::GetSkillByEffectType( const int nEffectTypeID ) const
{
	std::vector< StructSkill * >::const_iterator	it;
	for( it = m_vAllSkillList.begin(); it != m_vAllSkillList.end(); ++it )
	{
		if( (*it)->GetSkillBase()->GetEffectType() == nEffectTypeID ) return *it;
	}

	return NULL;
}

StructSkill * StructCreature::getSkill( int nSkillId ) const
{
	std::vector< StructSkill * >::const_iterator	it;
	for( it = m_vAllSkillList.begin(); it != m_vAllSkillList.end(); ++it )
	{
		if( (*it)->GetSkillId() == nSkillId ) return *it;
	}

	return NULL;
}

StructSkill * StructCreature::SetSkill( int skill_uid, int skill_id, int skill_level, AR_TIME remain_cool_time )
{
	int nPrevLevel;

	if( !GameContent::GetSkillBase( skill_id ) ) return NULL;

	StructSkill *pSkill = getSkill( skill_id );
	if( !pSkill ) 
	{	
		// 메모리 풀 사용하도록 변경
		//pSkill = new StructSkill( this, skill_uid, skill_id );
		pSkill = StructSkill::AllocSkill( this, skill_uid, skill_id );

		m_vAllSkillList.push_back( pSkill );
		if( pSkill->IsPassiveSkill() ) m_vPassiveSkillList.push_back( pSkill );
		else m_vActiveSkillList.push_back( pSkill );
	}

	nPrevLevel = pSkill->GetBaseSkillLevel();	
	pSkill->SetBaseSkillLevel( skill_level );
	
	// remain_cool_time이 가끔 음수인 경우가 있는데 여기서 DBCoolTime을 0으로 보정하지 않고 그대로 음수로 저장을 하면,
	// 로그아웃 시점의 remain_cool_time이 0이라고 하더라도 저장이 되며 0 또는 양수의 정상 범위 값으로 변한다.
	if( remain_cool_time ) pSkill->SetRemainCoolTime( remain_cool_time );
	pSkill->SetDBCoolTime( remain_cool_time );

	CheckAndSetEnhanceSkill( pSkill );

	return pSkill;
}

void StructCreature::CheckAndSetEnhanceSkill( StructSkill* skill )
{
	// 강화 스킬은 강화의 대상이 되지 않도록 의도하였다. 강화 자체가 재귀적으로 이루어지는 것이 아니라 최대 한 단계만 참조하기 떄문.
	// 다시 말해 강화 스킬1을 강화하는 스킬2가 있다고 해도 스킬1에 의해 강화되는 스킬은 스킬2에 아무런 영향을 받지 않는다.
	if ( skill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ENHANCE_SKILL )
	{
		for( std::vector< StructSkill* >::iterator it = m_vAllSkillList.begin(); it != m_vAllSkillList.end(); ++it )
		{
			if ( skill->GetSkillBase()->GetEnhancedSkillId() == (*it)->GetSkillId() )
			{
				assert( (*it)->GetSkillBase()->GetSkillEffectType() != SkillBase::EF_ENHANCE_SKILL );
				(*it)->SetEnhanceSkill( skill );
			}
		}
	}
	else
	{
		for( std::vector< StructSkill* >::iterator it = m_vPassiveSkillList.begin(); it != m_vPassiveSkillList.end(); ++it )
		{
			if( (*it)->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ENHANCE_SKILL &&
			    (*it)->GetSkillBase()->GetEnhancedSkillId() == skill->GetSkillId() )
			{
				skill->SetEnhanceSkill( *it );
			}		
		}
	}
}

void StructCreature::RegisterSkill( int skill_id, int skill_level, AR_TIME remain_cool_time, int skill_tree_id )
{
	int	nJobId = GetJobId();
	
	// JP, TP 소비
	__int64 nNeedJP = GameContent::GetNeedJpForSkillLevelUp( skill_id, skill_level, skill_tree_id );
	int nNeedTP = GameContent::GetNeedTpForSkillLevelUp( skill_id, skill_level );

	if( nNeedJP > GetJobPoint() || nNeedTP > GetTalentPoint() )
	{
		return;
	}

	if( nNeedJP > 0 )	m_nJobPoint -= nNeedJP;
	if( nNeedTP > 0 )	
	{
		// TP 사용은 로그로 찍어준다.
		StructPlayer * pClient = NULL;
		if( IsPlayer() )
		{
			pClient = static_cast< StructPlayer * >( this );
		}
		else if( IsSummon() )
		{
			pClient = static_cast< StructPlayer * >( static_cast< StructSummon * >( this )->GetMaster() );
		}
		
		if( pClient )
		{
			LOG::Log11N4S( LM_CHARACTER_USE_TP, pClient->GetAccountID(), 
				pClient->GetSID(), nJobId, 
				GetJobLevel(), skill_id,
				skill_level, nNeedJP,
				nNeedTP, GetTalentPoint() - nNeedTP, 
				0, 0,
				pClient->GetAccountName(), LOG::STR_NTS, 
				pClient->GetName(), LOG::STR_NTS,
				"", 0,
				"ByLearnSkill", LOG::STR_NTS );
		}

		SetTalentPoint( GetTalentPoint() - nNeedTP );
	}

	if( m_nJobPoint < 0 ) m_nJobPoint = 0;
	// 여기서 렙업할 일은 없고, 렙업할 일이 없다는 것은 calculatestat호출 될 일이 없다는 것이므로, 여기선 lock을 걸지 않는다.
	// JP 방송은 onExpChange에서 하고 TP 방송은 SetTalentPoint에서 이미 했다.
	onExpChange();
	// 

	int nSkillUID = 0;
	int nPrevLevel = GetBaseSkillLevel( skill_id );
	bool bIsNewSkill = false;

	if( !GetSkill( skill_id ) )
	{
		bIsNewSkill = true;
		nSkillUID = InterlockedIncrement( &s_nMaxSkillUID );
	}

	StructSkill *pSkill = SetSkill( nSkillUID, skill_id, skill_level, remain_cool_time );

	if( pSkill )
	{
		if( IsPlayer() )
		{
			StructPlayer * pPlayer = static_cast< StructPlayer * >( this );
			LOG::Log11N4S( LM_CHARACTER_LEARN_SKILL, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, skill_id, skill_level, nNeedJP, GetJobPoint(), nSkillUID, nNeedTP, GetTalentPoint(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		}
		else if( IsSummon() )
		{
			StructSummon * pSummon = static_cast< StructSummon * >( this );
			__int64 nCardUID = 0;

			if( pSummon->GetParentCard() )
			{
				nCardUID = pSummon->GetParentCard()->GetItemUID();
			}
			LOG::Log11N4S( LM_SUMMON_LEARN_SKILL, pSummon->GetMaster()->GetAccountID(), pSummon->GetMaster()->GetSID(), pSummon->GetSID(), skill_id, skill_level, nNeedJP, m_nJobPoint, nSkillUID, nNeedTP, GetTalentPoint(), nCardUID, pSummon->GetMaster()->GetAccountName(), LOG::STR_NTS, pSummon->GetMaster()->GetName(), LOG::STR_NTS, pSummon->GetName(), LOG::STR_NTS, "", 0 );
		}


		if( pSkill->IsPassiveSkill() )
		{
			CalculateStat();
		}

		onRegisterSkill( pSkill->GetSkillUID(), skill_id, skill_level, bIsNewSkill );
	}
}

void StructCreature::EnumState( StructCreature::STATE_FUNCTOR & _fo )
{	
	std::vector< StructState >::iterator	it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		_fo.onState( &(*it) );
	}
}

void StructCreature::EnumSkill( StructCreature::SKILL_FUNCTOR & _fo ) const
{
	AR_TIME t = GetArTime();
	std::vector< StructSkill * >::const_iterator	it;
	for( it = m_vAllSkillList.begin(); it != m_vAllSkillList.end(); ++it )
	{
		if( (*it)->GetSkillUID() > 0 )
			_fo.onSkill( (*it)->GetSkillUID(), (*it)->GetSkillId(), (*it)->GetBaseSkillLevel(), (*it)->GetCurrentSkillLevel(), (*it)->GetSkillCoolTime(), (*it)->GetRemainCoolTime( t ) );
	}
}

StructSkill*	StructCreature::GetCurrentPassiveSkill( int skill_id ) const
{
	std::vector< StructSkill * >::const_iterator	it;
	for( it = m_vPassiveSkillList.begin(); it != m_vPassiveSkillList.end(); ++it )
	{
		if( (*it)->GetSkillId() == skill_id ) return (*it);
	}

	return NULL;
}

int	StructCreature::GetCurrentPassiveSkillLevel( int skill_id ) const
{
	StructSkill* pSkill = GetCurrentPassiveSkill( skill_id );
	if( pSkill ) return pSkill->GetCurrentSkillLevel();
	return 0;
}

// skillLevelBase에서 skillLevelTarget까지 필요한 JP, TP 반환
const LearningCost GetSumOfSkillLearningCost( const StructCreature* creature, const int skillId, const int skillLevelBase, const int skillLevelTarget )
{
	int jobId[GameRule::MAX_JOB_DEPTH];
	int jobDepth = creature->GetJobDepth();

	for( int i = 0; i < jobDepth; ++i )
	{
		jobId[i] = creature->GetPrevJobId(i);
	}

	jobId[ jobDepth ] = creature->GetJobId();

	LearningCost cost;

	for( int skillLevel = skillLevelTarget; skillLevel > skillLevelBase; skillLevel-- )
	{
		int i = 0;

		__int64 JPPerLv = -1;
		while( JPPerLv == -1 && i <= jobDepth )
		{
			if( creature->IsSummon() )
			{
				SummonBase *pBase = GameContent::GetSummonInfo( jobId[i++] );
				if( !pBase )
					continue;

				for( int j = 0 ; j < GameRule::MAX_SUMMON_SKILL_TREE && JPPerLv == -1 ; ++j )
				{
					if( !pBase->skill_tree_id[j] )
						continue;
					JPPerLv = GameContent::GetNeedJpForSkillLevelUp( skillId, skillLevel, pBase->skill_tree_id[j] );
				}
			}
			else if( creature->IsPlayer() )
			{
				const JobInfo *pInfo = GameContent::GetJobInfo( jobId[i++] );
				if( !pInfo || !pInfo->skill_tree_id )
					continue;

				JPPerLv = GameContent::GetNeedJpForSkillLevelUp( skillId, skillLevel, pInfo->skill_tree_id );
			}
			else
				break;
		}
		if( JPPerLv != -1 )
		{
			cost.jp += JPPerLv;
		}

		int TPPerLv = GameContent::GetNeedTpForSkillLevelUp( skillId, skillLevel );
		if( TPPerLv != -1 )
		{
			cost.tp += TPPerLv;
		}
	}

	return cost;
}

const int GetAllowedMaxSkillLevel( const StructCreature* creature, const StructSkill* skill, const int remainingMaxJobDepth )
{
	int allowedSkillLevel = 0;
	int maxJobDepth = std::min( creature->GetJobDepth(), remainingMaxJobDepth );

	for( int checkedJobDepth = 0; checkedJobDepth <= maxJobDepth; checkedJobDepth++ )
	{
		int jobId = ( checkedJobDepth == creature->GetJobDepth() ) ? creature->GetJobId() : creature->GetPrevJobId( checkedJobDepth );
		if( creature->IsPlayer() )
		{
			const JobInfo *pInfo = GameContent::GetJobInfo( jobId );
			if( pInfo )
			{
				allowedSkillLevel = std::max( allowedSkillLevel, GameContent::GetAllowedMaxSkillLevel( pInfo->skill_tree, skill->GetSkillId() ) );
			}
		}
		else if( creature->IsSummon() )
		{
			const SummonBase *pBase = GameContent::GetSummonInfo( jobId );
			if( pBase )
			{
				for( int i = 0 ; i < GameRule::MAX_SUMMON_SKILL_TREE ; ++i )
				{
					if( !pBase->skill_tree_id[ i ] )
						continue;
					allowedSkillLevel = std::max( allowedSkillLevel, GameContent::GetAllowedMaxSkillLevel( pBase->skill_tree[ i ], skill->GetSkillId() ) );
				}
			}
		}
	}

	return allowedSkillLevel;
}


template< typename SkillPredicate >
void StructCreature::turnOffAuraOnSkillReset( const SkillPredicate& predicate )
{
	typedef std::pair<StructSkill *, int> Aura;

	// TODO : lambda 쓰면 한 줄로 처리 가능하니 차후 바꾸자
	struct RemainingAuraCriteria
	{
		RemainingAuraCriteria( const SkillPredicate& pred ) : predicate( pred ) {}
		const SkillPredicate& predicate;

		const bool operator() ( const Aura& aura ) const { return !predicate( aura.first ); }
	};

	std::vector< Aura >::iterator removedItems =
		std::stable_partition( m_vAura.begin(), m_vAura.end(), RemainingAuraCriteria( predicate ) );

	for( std::vector< Aura >::iterator it = removedItems; it != m_vAura.end(); ++it )
	{
		SendAuraMsessage( this, it->first->GetSkillId() );
	}

	m_vAura.erase( removedItems, m_vAura.end() );
}

template< typename SkillPredicate >
LearningCost StructCreature::removeSkillFromList( const SkillPredicate& predicate )
{
	// TODO : lambda 쓰면 한 줄로 처리 가능하니 차후 바꾸자
	struct RemainingSkillCriteria
	{
		RemainingSkillCriteria( const SkillPredicate& pred ) : predicate( pred ) {}
		const SkillPredicate& predicate;

		const bool operator() ( const StructSkill* skill ) const {
			return ( skill->GetSkillUID() < 0 ) || !predicate( skill );
		}
	};

	std::vector< StructSkill* >::iterator removedSkills =
		std::stable_partition( m_vAllSkillList.begin(), m_vAllSkillList.end(), RemainingSkillCriteria( predicate ) );

	LearningCost returnedSkillCost;

	for( std::vector< StructSkill* >::iterator it = removedSkills; it != m_vAllSkillList.end(); ++it )
	{
		StructSkill* removedSkill = *it;

		if( removedSkill->IsPassiveSkill() )
		{
			m_vPassiveSkillList.erase( std::find( m_vPassiveSkillList.begin(), m_vPassiveSkillList.end(), removedSkill ) );
		}
		else
		{
			m_vActiveSkillList.erase( std::find( m_vActiveSkillList.begin(), m_vActiveSkillList.end(), removedSkill ) );
		}

		returnedSkillCost += GetSumOfSkillLearningCost( this, removedSkill->GetSkillId(), 0, removedSkill->GetBaseSkillLevel() );

		onRemoveSkill( removedSkill );

		removedSkill->FreeSkill();
	}

	m_vAllSkillList.erase( removedSkills, m_vAllSkillList.end() );

	return returnedSkillCost;
}

// 아래 함수에서는 클라쪽으로 스킬 메시지를 보내는 처리를 따로 하지는 않는다.
// 이는 메시지 처리를 가급적 한꺼번에 처리하도록 하기 위함이다
template< typename SkillPredicate >
LearningCost StructCreature::RemoveSkill( const SkillPredicate& predicate )
{
	LearningCost returnSkillCost;

	if( IsUsingSkill() )
	{
		CancelSkill();
	}

	turnOffAuraOnSkillReset( predicate );
	returnSkillCost = removeSkillFromList( predicate );	

	return returnSkillCost;
}

// TODO: RegisterSkill과 중복되는 코드가 잔뜩 있으나 일단은 구현부터 해놓고 나중에 리팩토링
const bool StructCreature::UpdateSkillLevel( StructSkill* skill, int targetLevel )
{
	if( targetLevel < 0 )
	{
		return false;
	}

	// JP, TP 소비
	LearningCost cost;	
	int prevLevel = skill->GetBaseSkillLevel();

	if( prevLevel > targetLevel )
	{
		cost = GetSumOfSkillLearningCost( this, skill->GetSkillId(), targetLevel, skill->GetBaseSkillLevel() );

		cost.jp = -cost.jp;
		cost.tp = -cost.tp;
	}
	else
	{
		cost = GetSumOfSkillLearningCost( this, skill->GetSkillId(), skill->GetBaseSkillLevel(), targetLevel );

		if( cost.jp > GetJobPoint() || cost.tp > GetTalentPoint() )
		{
			return false;
		}
	}
	
	if( cost.jp != 0 )	SetJP( std::max<__int64>( GetJobPoint() - cost.jp, 0 ) ) ;
	if( cost.tp != 0 )
	{
		if( IsPlayer() )
		{
			StructPlayer * pPlayer = static_cast< StructPlayer * >( this );

			if( cost.tp > 0 )
			{
				LOG::Log11N4S( LM_CHARACTER_GAIN_TP, pPlayer->GetAccountID(), 
					pPlayer->GetSID(), skill->GetSkillId(), 
					pPlayer->GetPrevJobId( pPlayer->GetJobDepth() - 1 ), pPlayer->GetJobId(),
					pPlayer->GetPrevJobLevel( pPlayer->GetJobDepth() - 1 ), pPlayer->GetJobLevel(),
					pPlayer->GetTalentPoint(), pPlayer->GetTalentPoint() - cost.tp, 
					skill->GetBaseSkillLevel(), targetLevel,
					pPlayer->GetAccountName(), LOG::STR_NTS, 
					pPlayer->GetName(), LOG::STR_NTS,
					"", 0,
					"ByUpdateSkill", LOG::STR_NTS );
			}
			else
			{
				LOG::Log11N4S( LM_CHARACTER_USE_TP, pPlayer->GetAccountID(), 
					pPlayer->GetSID(), pPlayer->GetJobId(), 
					GetJobLevel(), skill->GetSkillId(),
					skill->GetBaseSkillLevel(), cost.jp,
					pPlayer->GetTalentPoint(), pPlayer->GetTalentPoint() - cost.tp, 
					targetLevel, 0,
					pPlayer->GetAccountName(), LOG::STR_NTS, 
					pPlayer->GetName(), LOG::STR_NTS,
					"", 0,
					"ByUpdateSkill", LOG::STR_NTS );
			}
		}
		SetTalentPoint( GetTalentPoint() - cost.tp );
	}

	skill->SetBaseSkillLevel( targetLevel );

	onRegisterSkill( skill->GetSkillUID(), skill->GetSkillId(), targetLevel, false );

	return true;
}

// 현재 스킬 트리에서 허용된 레벨 외의 범위에 있는 스킬들의 레벨을 재조정, 스킬 리셋에서 사용한다
void StructCreature::AdjustOverflowedSkillLevel( const int remainingMaxJobDepth )
{
	for( std::vector< StructSkill* >::iterator it = m_vAllSkillList.begin(); it != m_vAllSkillList.end(); ++it )
	{
		StructSkill* skill = *it;
		if( skill->GetSkillUID() < 0 )
		{	// 시스템 스킬인 경우는 패스
			continue;
		}

		int allowedSkillLevel = GetAllowedMaxSkillLevel( this, skill, remainingMaxJobDepth );

		if( allowedSkillLevel < skill->GetBaseSkillLevel() )
		{
			UpdateSkillLevel( skill, allowedSkillLevel );
		}
	}
}



bool StructCreature::ResetSkill( const _SKILL_RESET_METHOD eMethod, const int jobDepth, bool doResetRandomSkill )
{
	if( jobDepth < 0 || jobDepth > GetJobDepth() )
	{
		return false;
	}

	__int64 nPrevJP = GetJobPoint();
	int nPrevTP = GetTalentPoint();	

	// TODO: 차후 람다로 교체
	struct NotOwnedByRemainingSkillTree
	{
		NotOwnedByRemainingSkillTree( const StructCreature* creature, const int remainingMaxJobDepth, bool resetRandomSkill )
			: creature( creature ), remainingMaxJobDepth( remainingMaxJobDepth ), resetRandomSkill( resetRandomSkill ) {}

		const bool operator() ( const StructSkill* skill ) const
		{
			// 만약 크리쳐 랜덤 스킬일 경우 삭제하지 않는다. (밑에서 skill_level만 0으로 만들어준다.)
			// 단, 강제 초기화 일 경우 스킬 다 날려줌. (resetRandomSkill == true)
			if( !resetRandomSkill && creature->IsSummon() && skill->IsRandomSkill() )
				return false;
			// 남아 있어야하는 스킬 트리에서 허용 레벨이 0 -> 삭제 대상
			return GetAllowedMaxSkillLevel( creature, skill, remainingMaxJobDepth ) == 0; 
		}
		const StructCreature* creature;
		int remainingMaxJobDepth;
		bool resetRandomSkill;
	};

	// 스킬 초기화 후 포인트 반납
	LearningCost returnedSkillCost = RemoveSkill( NotOwnedByRemainingSkillTree( this, jobDepth-1, doResetRandomSkill ) );
	SetJP( GetJobPoint() + returnedSkillCost.jp );
	if( returnedSkillCost.tp )
	{
		if( IsPlayer() )
		{
			StructPlayer * pPlayer = static_cast< StructPlayer * >( this );
			LOG::Log11N4S( LM_CHARACTER_GAIN_TP, pPlayer->GetAccountID(), 
				pPlayer->GetSID(), 0, 
				pPlayer->GetPrevJobId( pPlayer->GetJobDepth() - 1 ), pPlayer->GetJobId(),
				pPlayer->GetPrevJobLevel( pPlayer->GetJobDepth() - 1 ), pPlayer->GetJobLevel(),
				pPlayer->GetTalentPoint(), pPlayer->GetTalentPoint() + returnedSkillCost.tp, 
				0, 0,
				pPlayer->GetAccountName(), LOG::STR_NTS, 
				pPlayer->GetName(), LOG::STR_NTS,
				"", 0,
				"ByResetSkill", LOG::STR_NTS );
		}
		SetTalentPoint( GetTalentPoint() + returnedSkillCost.tp );
	}

	// 크리쳐의 경우 남아 있는 랜덤 스킬들을 0으로 업데이트 시켜줌. 여기서 0으로 업데이트 된 애들 스킬 포인트 반납
	if( IsSummon() )
	{
		// 남아있는 랜덤 스킬들의 경우 0으로 업데이트 시켜준다.
		for( std::vector< StructSkill* >::iterator it = m_vAllSkillList.begin() ; it != m_vAllSkillList.end() ; ++it )
			UpdateSkillLevel( (*it), 0 );	
	}

	if( jobDepth > 0 )
	{
		// 현재 스킬 트리로는 찍을 수 없는 일부 스킬이 남아 있을 수 있다.
		// 이 경우는 스킬 리스트에서 남아 있는 녀석들을 대상으로 스킬 레벨을 업데이트하자
		AdjustOverflowedSkillLevel( jobDepth-1 );
	}

	CalculateStat();
	onResetSkill( jobDepth );

	if( IsPlayer() )
	{
		const StructPlayer * pPlayer = static_cast< const StructPlayer * >( this );
		LOG::Log11N4S( LM_SKILL_RESET, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), 0, eMethod, nPrevJP, nPrevTP, GetJobPoint(), GetTalentPoint(), 0, 0, 0,
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}
	else if( IsSummon() )
	{
		const StructSummon * pSummon = static_cast< const StructSummon * >( this );
		const StructPlayer * pPlayer = pSummon->GetMaster();
		// pPlayer == NULL 일 가능성이 작지만 있으므로 pPlayer 참조 부분에 모두 NULL 체크해서 대체 값이 존재하도록 처리해야 함
		LOG::Log11N4S( LM_SKILL_RESET,
			( pPlayer ) ? pPlayer->GetAccountID() : 0,
			( pPlayer ) ? pPlayer->GetPlayerUID() : 0,
			pSummon->GetSummonSID(), eMethod, nPrevJP, nPrevTP, GetJobPoint(), GetTalentPoint(), 0, 0, 0,
			( pPlayer ) ? pPlayer->GetAccountName() : "", LOG::STR_NTS,
			( pPlayer ) ? pPlayer->GetName() : "", LOG::STR_NTS,
			pSummon->GetName(), LOG::STR_NTS, "", 0 );
	}

	return true;
}



__int64 StructCreature::GetAllSkillJP() const
{
	int nJobId[GameRule::MAX_JOB_DEPTH];
	int nJobDepth = GetJobDepth();

	int i;

	for( i = 0; i < nJobDepth; ++i )
	{
		nJobId[i] = GetPrevJobId(i);
	}

	nJobId[i] = GetJobId();

	__int64 nJP = 0;

	// 지금 가지고 있는 스킬들 배우는데 필요한 잡포 계산
	for( std::vector< StructSkill * >::const_iterator it = m_vAllSkillList.begin() ; it != m_vAllSkillList.end() ; it++ )
	{
		StructSkill *pSkill = (*it);

		if( (*it)->GetSkillUID() == StructSkill::SKILL_UID_ITEM_SKILL ||
			(*it)->GetSkillUID() == StructSkill::SKILL_UID_PROP_SKILL ||
			(*it)->GetSkillUID() == StructSkill::SKILL_UID_SUMMON_SKILL ||
			(*it)->GetSkillUID() == StructSkill::SKILL_UID_MONSTER_SKILL ||
			(*it)->GetSkillUID() == StructSkill::SKILL_UID_PET_SKILL )
			continue;

		for( int nSkillLevel = pSkill->GetBaseSkillLevel() ; nSkillLevel > 0 ; nSkillLevel-- )
		{
			int jp = -1;

			i = 0;

			while( jp == -1 && i <= nJobDepth )
			{
				if( IsSummon() )
				{
					SummonBase *pBase = GameContent::GetSummonInfo( nJobId[i++] );
					if( !pBase )
						continue;

					for( int j = 0 ; j < GameRule::MAX_SUMMON_SKILL_TREE && jp == -1 ; ++j )
					{
						if( !pBase->skill_tree_id[j] )
							continue;

						jp = GameContent::GetNeedJpForSkillLevelUp( pSkill->GetSkillId(), nSkillLevel, pBase->skill_tree_id[j] );
					}
				}
				else if( IsPlayer() )
				{
					const JobInfo *pInfo = GameContent::GetJobInfo( nJobId[i++] );
					if( !pInfo || !pInfo->skill_tree_id )
						continue;

					jp = GameContent::GetNeedJpForSkillLevelUp( pSkill->GetSkillId(), nSkillLevel, pInfo->skill_tree_id );
				}
				else
					break;
			}

			if( jp != -1 )
			{
				nJP += jp;
			}
		}
	}

	return nJP;
}

__int64 StructCreature::GetAllJobLevelJP() const
{
	__int64 nJP = 0;
	int nJobLevel = GetJobLevel();
	int nJobDepth = GetJobDepth();

	for( int i = 1 ; i < nJobLevel ; ++i )
	{
		nJP += GameContent::GetNeedJpForJobLevelUp( i, nJobDepth );
	}

	return nJP;
}

int StructCreature::IsLearnableSkill( int nSkillID, int nSkillLevel, int *nSkillTreeID )
{
	return RESULT_ACCESS_DENIED;
}

int StructCreature::GetBaseSkillLevel( int skill_id ) const
{
	StructSkill *pSkill = getSkill( skill_id );
	if( !pSkill ) return 0;
	return pSkill->GetBaseSkillLevel();
}

int StructCreature::GetAddedSkillLevel( const StructSkill *const pSkill ) const
{
	if( pSkill->IsLimitedAddedSkill() ) return 0;

	std::vector< std::pair< int, int > >::const_iterator it;

	int skill_id = pSkill->GetSkillId();
	int skill_type = 0;
	int addedSkillLevel = 0;

	// 스킬의 타입을 정의
	if( pSkill->GetSkillBase()->IsHarmful() )		skill_type += StructState::FLAG_HARMFUL;
	else											skill_type += StructState::FLAG_HELPFUL;
	if( pSkill->GetSkillBase()->IsPassive() )		skill_type += StructState::FLAG_PASSIVE;
	else											skill_type += StructState::FLAG_ACTIVE;
	if( pSkill->GetSkillBase()->IsPhysicalSkill() )	skill_type += StructState::FLAG_PHYSICAL;
	else											skill_type += StructState::FLAG_MAGICAL;

	for( it = m_vAddedSkillBySkillId.begin(); it != m_vAddedSkillBySkillId.end(); it++ )
	{
		if( (*it).first == skill_id )
		{
			addedSkillLevel += (*it).second;
			break;
		}
	}

	for( it = m_vAddedSkillBySkillType.begin(); it != m_vAddedSkillBySkillType.end(); it++ )
	{
		if( (*it).first & skill_type )
		{
			addedSkillLevel += (*it).second;
		}
	}

	if( addedSkillLevel < 0 && addedSkillLevel < this->GetBaseSkillLevel( skill_id ) )
		addedSkillLevel = -this->GetBaseSkillLevel( skill_id );

	return addedSkillLevel;
}

int StructCreature::GetCurrentSkillLevel( int skill_id ) const
{
	StructSkill *pSkill = getSkill( skill_id );
	if( !pSkill ) return 0;
	return pSkill->GetCurrentSkillLevel();
}

void StructCreature::AddRemainCoolTime( const int skill_id, const int nInc, const float fAmp )
{
	StructPlayer * pClient = NULL;

	if( IsPlayer() )
	{
		pClient = static_cast< StructPlayer * >( this );
	}
	else if( IsSummon() )
	{
		pClient = static_cast< StructSummon * >( this )->GetMaster();
	}

	if( skill_id < 0 )
	{
		struct _mySkillFunctor : SKILL_POINTER_FUNCTOR
		{
			_mySkillFunctor( const bool _bExceptRulerOfTime, const int _nInc, const float _fAmp ) : tCurrentTime( GetArTime() ), bExceptRulerOfTime( _bExceptRulerOfTime ), nInc( _nInc ), fAmp( _fAmp ) {}

			virtual void onSkill( StructSkill *pSkill )
			{
				// 은총 스킬은 전직업 공통 스킬로 시스템 스킬의 의도로 들어갔기 때문에 쿨타임 제어에 영향을 받지 않아야 한다.
				if( pSkill->GetSkillId() == StructSkill::SKILL_GRACE ) return;

				// 시간의 지배자의 쿨타임 값을 변경하지 않아야하는 경우는 변경하지 않도록 한다.
				if( bExceptRulerOfTime && pSkill->GetSkillId() == StructSkill::SKILL_RULER_OF_TIME ) return;

				if( pSkill->CheckCoolTime( tCurrentTime ) )
				{
					return;
				}

				// 현재는 증가와 증폭이 동시에 적용되지 않는다.
				// ArTime은 1/100초
				pSkill->SetRemainCoolTime( pSkill->GetRemainCoolTime( tCurrentTime ) * fAmp + nInc * 100 );

				vSkill.push_back( pSkill->GetSkillId() );
			}

			AR_TIME tCurrentTime;
			std::vector< int > vSkill;
			bool bExceptRulerOfTime;
			int nInc;
			float fAmp;
		} foCooldown( ( m_pCastSkill && m_pCastSkill->GetSkillId() == StructSkill::SKILL_RULER_OF_TIME ), nInc, fAmp );

		EnumActiveSkill( foCooldown );

		SendSkillMessage( pClient, this, foCooldown.vSkill );
	}
	else
	{
		// 은총 스킬은 전직업 공통 스킬로 시스템 스킬의 의도로 들어갔기 때문에 쿨타임 제어에 영향을 받지 않아야 한다.
		if( skill_id == StructSkill::SKILL_GRACE ) return;

		AR_TIME tCurrentTime = GetArTime();

		StructSkill *pSkill = GetSkill( skill_id );

		if( !pSkill || pSkill->GetSkillUID() <= 0 || pSkill->CheckCoolTime( tCurrentTime ) ) return;

		// 현재는 증가와 증폭이 동시에 적용되지 않는다.
		// ArTime은 1/100초
		pSkill->SetRemainCoolTime( pSkill->GetRemainCoolTime( tCurrentTime ) * fAmp + nInc * 100 );

		SendSkillMessage( pClient, this, skill_id );
	}
}

AR_TIME	StructCreature::GetRemainCoolTime( int skill_id ) const
{
	AR_TIME t = GetArTime();

	if( ENV().GetInt( "game.no_skill_cooltime", 0 ) )
		return 0;

	StructSkill *pSkill = getSkill( skill_id );
	if( !pSkill ) return 0;
	return pSkill->GetRemainCoolTime( t );
}

AR_TIME	StructCreature::GetTotalCoolTime( int skill_id ) const
{
	AR_TIME t = GetArTime();

	if( ENV().GetInt( "game.no_skill_cooltime", 0 ) )
		return 0;

	StructSkill *pSkill = getSkill( skill_id );
	if( !pSkill ) return 0;
	return pSkill->GetSkillCoolTime();
}

const __int64 StructCreature::GetBulletCount() const
{
	//return 1;	// 임시
	StructItem * pBullet = GetWearedItem( ItemBase::WEAR_BULLET );

	return ( pBullet && pBullet->IsBullet() ) ? pBullet->GetCount() : 10;
}

void StructCreature::BindSkillCard( struct StructItem *pItem, const bool bSkipDBUpdate )
{
	StructSkill *pSkill = getSkill( pItem->GetSkillId() );
	if( !pSkill ) return;

	pSkill->SetEnhance( pItem->GetItemEnhance() );

	pItem->SetBindTarget( this, bSkipDBUpdate );

	SendSkillCardInfo( pItem );

	// 다만 스킬 카드가 2개 이상일 경우 나머지 카드들은 빼서 뒤로 넣어준다
	if( pItem->GetCount() > 1 )
	{
		StructPlayer * pClient;
		if( IsPlayer() )
		{
			pClient = static_cast< StructPlayer * >( this );
		}
		else if( IsSummon() )
		{
			pClient = static_cast< StructPlayer * >( static_cast< StructSummon* >( this )->GetMaster() );
		}

		StructItem * pItemOriginal = pItem;
		int nCount = pItemOriginal->GetCount();
		pItemOriginal->SetCount( 1 );
		SendItemMessage( pClient, pItemOriginal );

		pItem = StructItem::AllocItem( 0, pItem->GetItemCode(), 1, ItemInstance::BY_DIVIDE );
		pItem->CopyFrom( pItemOriginal );
		pItem->SetBindTarget( NULL );
		pItem->SetCount( nCount - 1 );
		pItem->SetIdx( 0 );
		pClient->PushItem( pItem, nCount - 1 );

		// 아이템 추적을 위해 분리되면 로그로 기록해준다.
		LOG::Log11N4S( LM_ITEM_DIVIDE, pClient->GetAccountID(), 
			pClient->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), 
			pItem->GetItemCode(), pItem->GetCount(),
			pItemOriginal->GetCount(), pClient->GetGold().GetRawData(),
			0, 0,
			pItemOriginal->GetItemUID(), pItem->GetItemUID(),
			pClient->GetAccountName(), LOG::STR_NTS, 
			pClient->GetName(), LOG::STR_NTS,
			"", 0,
			"CARD_BIND", LOG::STR_NTS );
	}	
}

void StructCreature::UnBindSkillCard( struct StructItem *pItem )
{
	// 해제는 할 수 있어야 한다.
	StructSkill *pSkill = getSkill( pItem->GetSkillId() );
	if( pSkill )
		pSkill->SetEnhance( 0 );

	pItem->SetBindTarget( NULL );

	SendSkillCardInfo( pItem );

	// 스킬 카드 해제 시 같은 카드들이 인벤에 있나 보고 있다면 가져와서 합쳐준다.
	StructPlayer * pClient = NULL;
	if( IsPlayer() )	
	{
		pClient = static_cast< StructPlayer * >( this );
	}
	if( IsSummon() )
	{
		pClient = static_cast< StructSummon * >( this )->GetMaster();
	}

	StructItem *pJoinableItem = pClient->GetInventory()->FindJoinablePair( pItem );
	if( pJoinableItem )
	{
		int nCount = pJoinableItem->GetCount();
		pClient->PopItem( pJoinableItem, nCount );
		pItem->SetCount( nCount + 1 );
		SendItemMessage( pClient, pItem );

		// 아이템 추적을 위해 합쳐지면 로그로 기록해준다.
		LOG::Log11N4S( LM_ITEM_JOIN, pClient->GetAccountID(), 
			pClient->GetSID(), pJoinableItem->GetItemEnhance() * 100 + pJoinableItem->GetItemLevel(), 
			pJoinableItem->GetItemCode(), pJoinableItem->GetCount(),
			pItem->GetCount(), pClient->GetGold().GetRawData(),
			0, 0,
			pItem->GetItemUID(), pJoinableItem->GetItemUID(),
			pClient->GetAccountName(), LOG::STR_NTS, 
			pClient->GetName(), LOG::STR_NTS,
			"", 0,
			"CARD_UNBIND", LOG::STR_NTS );
	}
}

AR_HANDLE StructCreature::GetTrapHandle()
{
	if( !m_hTrap )
		return NULL;

	GameObject *pObj = GameObject::raw_get( m_hTrap );
	if( !pObj || !pObj->IsSkillProp() || !pObj->IsInWorld() || pObj->IsDeleteRequested() )
	{
		m_hTrap = NULL;
		return NULL;
	}

	return m_hTrap;
}

bool StructCreature::IsWearShield() const
{
	StructItem * pShield = GetWearedItem( ItemBase::WEAR_SHIELD );
	return pShield ? pShield->GetItemClass() == ItemBase::CLASS_SHIELD : false;
}

bool StructCreature::IsWeared( AR_HANDLE ItemHandle )
{
	StructItem *pItem = StructItem::FindItem( ItemHandle );
	if( !pItem ) return false;

	for( int i = 0; i < ItemBase::MAX_SPARE_ITEM_WEAR; ++i )
	{
		if( m_anWear[i] == pItem ) return true;
	}

	return false;
}

bool StructCreature::IsWeared( ItemBase::ItemCode code )
{

	for( int i = 0; i < ItemBase::MAX_SPARE_ITEM_WEAR; ++i )
	{
		if( m_anWear[i] )
		{
			if( m_anWear[i]->GetItemCode() == code )
				return true;
		}
	}

	return false;
}

bool StructCreature::TranslateWearPosition( ItemBase::ItemWearType & pos, struct StructItem *pItem, std::vector< int > * vpOverlappItemList )
{
	// 장비품 아님
	if( pItem->GetWearType() == ItemBase::WEAR_CANTWEAR ) return false;

	// 파기되었는지?
	if( !pItem->IsWearable() ) return false;

	// 유니트 익스퍼트 스킬이 있으면 랭크 무시하고 장착 가능하다고 함.
	int nMyLevel = m_nUnitExpertLevel > GetLevel() ? m_nUnitExpertLevel : GetLevel();

	if( pItem->GetLevelLimit() > nMyLevel ) return false;

	// 장비 아이템에 레벨 제한 설정이 별도로 걸려 있을 경우 해당 레벨 제한을 확인
	// 최소 레벨 체크
	if( pItem->GetItemBase().nMinLevel && GetLevel() < pItem->GetItemBase().nMinLevel )
		return false;
	// 최대 레벨 체크
	if( pItem->GetItemBase().nMaxLevel && GetLevel() > pItem->GetItemBase().nMaxLevel )
		return false;

	return true;
}

unsigned short StructCreature::putonItem( ItemBase::ItemWearType pos, struct StructItem * pItem )
{
	assert( pos < ItemBase::MAX_ITEM_WEAR );

	assert( !m_anWear[ pos ] );

	m_anWear[ pos ] = pItem;

	// floyd 2008. 7..3
	// http://bug.nflavor.com/view.php?id=3380 관련 문제를 처리하다 수정
	// StructSummon::putonItem에서 처리하면 굳이 IsSummon() 확인 할 필요가 없다.
	// if( IsSummon() )
	// {
	//	pItem->SetOwnSummonInfo( GetHandle(), GetSID() );
	// }

	pItem->SetWearInfo( pos );

	pItem->SetBindedCreatureHandle( GetHandle() );

	pItem->TurnOnUpdateFlag();

	if( ( pItem->IsBow() || pItem->IsCrossBow() ) && pos < ItemBase::MAX_ITEM_WEAR )
	{
		m_nNextAttackMode	= StructCreature::AM_AIMING;
	}

	if( IsPlayer() )
	{
		SendItemWearInfoMessage( static_cast< StructPlayer* >( this ), this, m_anWear[ pos ] );
	}
	else if( IsSummon() )
	{
		SendItemWearInfoMessage( static_cast< StructPlayer* >( static_cast< StructSummon * >(this)->GetMaster() ), this, m_anWear[ pos ] );
	}

	return RESULT_SUCCESS;
}

unsigned short StructCreature::putoffItem( ItemBase::ItemWearType pos )
{
	assert( pos < ItemBase::MAX_ITEM_WEAR );

	m_anWear[ pos ]->SetWearInfo( static_cast< ItemBase::ItemWearType >( -1 ) );

	m_anWear[ pos ]->SetBindedCreatureHandle( NULL );

	m_anWear[ pos ]->TurnOnUpdateFlag();

	if( IsPlayer() )
	{
		SendItemWearInfoMessage( static_cast< StructPlayer* >( this ), this, m_anWear[ pos ] );
	}
	else if( IsSummon() )
	{
		SendItemWearInfoMessage( static_cast< StructPlayer* >( static_cast< StructSummon * >(this)->GetMaster() ), this, m_anWear[ pos ] );
	}

	m_anWear[ pos ] = NULL;

	return RESULT_SUCCESS;
}

void StructCreature::PutonSet( struct StructItem* pItemList[] )
{
	int idx = 0;

	// 일단 다 벗자
	for( idx = 0; idx < ItemBase::MAX_ITEM_WEAR; ++idx )
	{
		if( m_anWear[idx] ) putoffItem( static_cast< ItemBase::ItemWearType >( idx ) );
	}

	// 다 입자
	for( idx = 0; idx < ItemBase::MAX_ITEM_WEAR; ++idx )
	{
		if( ! ( pItemList[idx] ) ) continue;

		Puton( static_cast< ItemBase::ItemWearType >( idx ), pItemList[idx] );		
	}

	// 스탯 재계산
	CalculateStat();
}

unsigned short StructCreature::Puton( ItemBase::ItemWearType pos, struct StructItem * pItem )
{
	if( !pItem->IsInInventory() )
	{
		// _cprint( "StructCreature::Puton() :: 장비 불가 : (%I64d)\n", pItem->GetItemUID() );
		return RESULT_ACCESS_DENIED;
	}

	if( pItem->GetWearInfo() != ItemBase::WEAR_NONE )
	{
		return RESULT_NOT_ACTABLE;
	}

	std::vector< int > vOverlappItemList;

	if( !TranslateWearPosition( pos, pItem, &vOverlappItemList ) )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 장착 위치에 무언가 입고 있다면 벗는다
	for( std::vector< int >::iterator it = vOverlappItemList.begin(); it != vOverlappItemList.end(); it++ )
	{
		putoffItem( static_cast< ItemBase::ItemWearType >( *it ) );

		if( m_anWear[ *it ] ) return RESULT_NOT_ACTABLE; // 벗기 실패. (목걸이 같은 경우)
	}

	// 장착
	return putonItem( pos, pItem );
}

unsigned short StructCreature::Putoff( ItemBase::ItemWearType pos )
{	
	// 두손무기는 오른손이므로..
	if( pos == ItemBase::WEAR_TWOHAND ) pos = ItemBase::WEAR_WEAPON;

	// 예외처리 #3. 투슬롯 반지를 장착해야 하는데 이미 오른쪽에 반지를 입고 있으면 벗는다
	if( pos == ItemBase::WEAR_TWOFINGER_RING )
	{
		pos = ItemBase::WEAR_RING;
	}

	// 검증
	if( ( pos >= ItemBase::MAX_SPARE_ITEM_WEAR && pos != ItemBase::WEAR_TWOHAND ) || pos < 0 )
	{
		// _cprint( "StructCreature::Putoff() :: 잘못된 탈착위치 : %d\n", pos );
		return RESULT_NOT_ACTABLE;
	}

	// 입고있는 상태가 아니라면 실패
	ItemBase::ItemWearType absolute_pos = GetAbsoluteWearPos( pos );
	if( absolute_pos == static_cast< ItemBase::ItemWearType >( -1 ) )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 장착된 가방을 해제하려 할 때 최대 무게치보다 소지량이 커지면 불가
	if( pos == ItemBase::WEAR_BAG_SLOT )
	{
		if( GetMaxWeight() < GetWeight() )
		{
			return RESULT_TOO_HEAVY;
		}

		const ItemBaseServer & current_bag_base = m_anWear[ absolute_pos ]->GetItemBase();
		c_fixed10 current_bag_capacity;

		for( int i = 0 ; i < ItemBase::MAX_OPTION_NUMBER ; ++i )
		{
			if( current_bag_base.nOptType[ i ] != ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )
				continue;

			current_bag_capacity += current_bag_base.fOptVar1[ i ];
		}

		// 가방에는 소울스톤 세공이 불가능하므로 소울스톤 중에 소지량 증가 성능이 있는 경우를 체크할 필요는 없음

		if( current_bag_base.pvEffectList && !current_bag_base.pvEffectList->empty() )
		{
			for( std::vector< EffectInfo * >::const_iterator it = current_bag_base.pvEffectList->begin() ; it != current_bag_base.pvEffectList->end() ; ++it )
			{
				EffectInfo * pEffect = (*it);
				if( pEffect->nMinLevel && GetLevel() < pEffect->nMinLevel ) continue;
				if( pEffect->nMaxLevel && GetLevel() > pEffect->nMaxLevel ) continue;

				if( pEffect->eType != EffectInfo::EFFECT_TYPE_OPTIONAL )
					continue;

				if( pEffect->fValue[ 0 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )	current_bag_capacity += pEffect->fValue[ 1 ];
				if( pEffect->fValue[ 3 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )	current_bag_capacity += pEffect->fValue[ 4 ];
				if( pEffect->fValue[ 6 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )	current_bag_capacity += pEffect->fValue[ 7 ];
				if( pEffect->fValue[ 9 ] == ITEM_EFFECT_PASSIVE::CARRY_WEIGHT )	current_bag_capacity += pEffect->fValue[ 10 ];
			}
		}

		current_bag_capacity *= m_fItemMod;

		if( GetMaxWeight() - current_bag_capacity < GetWeight() )
		{
			return RESULT_TOO_HEAVY;
		}
	}

	// 벗자
	return putoffItem( absolute_pos );
}

void StructCreature::ProcByAttack( StructCreature *pTarget, const int nDamage, const bool bIsAttacking, const DWORD nAttackType, const int nElementalType )
{
	std::vector< _ATTACK_TAG > *pvProc;
	std::vector< StructProc * > vActableProc;

	if( bIsAttacking )	pvProc = &m_vProcByAttack;
	else				pvProc = &m_vProcByBeingAttacked;

	std::vector< _ATTACK_TAG >::const_iterator it;
	for( it = pvProc->begin(); it != pvProc->end(); ++it )
	{
		// 타겟이 존재하지 않는 경우에도 정상 호출이 되어야 하는 경우가 있으므로 우선 검사한다.
		if( (*it).CheckProcByAttack( GetHPPercentage(), pTarget ? pTarget->GetHPPercentage() : -1, nAttackType, nElementalType ) )
		{
			vActableProc.push_back( (*it).proc->Clone() );
		}
	}

	std::vector< StructProc * >::iterator procIt;
	for( procIt = vActableProc.begin(); procIt != vActableProc.end(); ++procIt )
	{
		(*procIt)->Proc( this, pTarget, nDamage, bIsAttacking );
		delete (*procIt);
	}
}

void StructCreature::ProcBySkillId( StructCreature *pTarget, const int nDamage, const bool bIsAttacking, const int nSkillId )
{
	std::map< int, std::vector< _PROC_TAG > >::iterator foundIt = m_mapProcBySkillId.find( nSkillId );
	if( foundIt == m_mapProcBySkillId.end() ) return;

	std::vector< _PROC_TAG > *pvProc = &(*foundIt).second;
	std::vector< StructProc * > vActableProc;

	std::vector< _PROC_TAG >::const_iterator it;
	for( it = pvProc->begin(); it != pvProc->end(); ++it )
	{
		// 타겟이 존재하지 않는 경우에도 정상 호출이 되어야 하는 경우가 있으므로 우선 검사한다.
		if( (*it).CheckProc( GetHPPercentage(), pTarget ? pTarget->GetHPPercentage() : -1 ) )
		{
			vActableProc.push_back( (*it).proc->Clone() );
		}
	}

	std::vector< StructProc * >::iterator procIt;
	for( procIt = vActableProc.begin(); procIt != vActableProc.end(); ++procIt )
	{
		(*procIt)->Proc( this, pTarget, nDamage, bIsAttacking );
		delete (*procIt);
	}
}

void StructCreature::ProcByKill( StructCreature *pTarget, const bool bIsKilling )
{
	std::vector< _KILL_TAG > *pvProc;
	std::vector< StructProc * > vActableProc;

	if( bIsKilling )	pvProc = &m_vProcByKill;
	else				pvProc = &m_vProcByDead;

	std::vector< _KILL_TAG >::const_iterator it;
	for( it = pvProc->begin(); it != pvProc->end(); ++it )
	{
		// 타겟이 존재하지 않는 경우에도 정상 호출이 되어야 하는 경우가 있으므로 우선 검사한다.
		if( (*it).CheckProcByDead( GetHPPercentage(), pTarget ? pTarget->GetHPPercentage() : -1, GetLevel() - pTarget->GetLevel(), GetMPPercentage() ) )
		{
			vActableProc.push_back( (*it).proc->Clone() );
		}
	}

	std::vector< StructProc * >::iterator procIt;
	for( procIt = vActableProc.begin(); procIt != vActableProc.end(); ++procIt )
	{
		(*procIt)->Proc( this, pTarget, 0, bIsKilling );
		delete (*procIt);
	}
}

void StructCreature::ProcByCritical( StructCreature *pTarget, const int nDamage, const bool bIsAttacking, const DWORD nAttackType, const int nElementalType )
{
	std::vector< _ATTACK_TAG > *pvProc;
	std::vector< StructProc * > vActableProc;

	if( bIsAttacking )	pvProc = &m_vProcByCriticalAttack;
	else				pvProc = &m_vProcByBeingCriticalAttacked;

	std::vector< _ATTACK_TAG >::const_iterator it;
	for( it = pvProc->begin(); it != pvProc->end(); ++it )
	{
		// 타겟이 존재하지 않는 경우에도 정상 호출이 되어야 하는 경우가 있으므로 우선 검사한다.
		if( (*it).CheckProcByAttack( GetHPPercentage(), pTarget ? pTarget->GetHPPercentage() : -1, nAttackType, nElementalType ) )
		{
			vActableProc.push_back( (*it).proc->Clone() );
		}
	}

	std::vector< StructProc * >::iterator procIt;
	for( procIt = vActableProc.begin(); procIt != vActableProc.end(); ++procIt )
	{
		(*procIt)->Proc( this, pTarget, nDamage, bIsAttacking );
		delete (*procIt);
	}
}

void StructCreature::ProcByAvoid( StructCreature *pFrom, const DWORD nAttackType, const int nElementalType )
{
	std::vector< StructProc * > vActableProc;

	std::vector< _ATTACK_TAG >::const_iterator it;
	for( it = m_vProcByAvoid.begin(); it != m_vProcByAvoid.end(); ++it )
	{
		// 타겟이 존재하지 않는 경우에도 정상 호출이 되어야 하는 경우가 있으므로 우선 검사한다.
		if( (*it).CheckProcByAttack( GetHPPercentage(), pFrom ? pFrom->GetHPPercentage() : -1, nAttackType, nElementalType ) )
		{
			vActableProc.push_back( (*it).proc->Clone() );
		}
	}

	std::vector< StructProc * >::iterator procIt;
	for( procIt = vActableProc.begin(); procIt != vActableProc.end(); ++procIt )
	{
		(*procIt)->Proc( this, pFrom, 0, false );
		delete (*procIt);
	}
}

void StructCreature::ProcByBlock( StructCreature *pTarget, const int nDamage, const DWORD nAttackType, const int nElementalType )
{
	std::vector< StructProc * > vActableProc;

	std::vector< _ATTACK_TAG >::const_iterator it;
	for( it = m_vProcByBlock.begin(); it != m_vProcByBlock.end(); ++it )
	{
		// 타겟이 존재하지 않는 경우에도 정상 호출이 되어야 하는 경우가 있으므로 우선 검사한다.
		if( (*it).CheckProcByAttack( GetHPPercentage(), pTarget ? pTarget->GetHPPercentage() : -1, nAttackType, nElementalType ) )
		{
			vActableProc.push_back( (*it).proc->Clone() );
		}
	}

	std::vector< StructProc * >::iterator procIt;
	for( procIt = vActableProc.begin(); procIt != vActableProc.end(); ++procIt )
	{
		(*procIt)->Proc( this, pTarget, nDamage, false );
		delete (*procIt);
	}
}

void StructCreature::ProcByPerfectBlock( StructCreature *pTarget, const DWORD nAttackType, const int nElementalType )
{
	std::vector< StructProc * > vActableProc;

	std::vector< _ATTACK_TAG >::const_iterator it;
	for( it = m_vProcByPerfectBlock.begin(); it != m_vProcByPerfectBlock.end(); ++it )
	{
		// 타겟이 존재하지 않는 경우에도 정상 호출이 되어야 하는 경우가 있으므로 우선 검사한다.
		if( (*it).CheckProcByAttack( GetHPPercentage(), pTarget ? pTarget->GetHPPercentage() : -1, nAttackType, nElementalType ) )
		{
			vActableProc.push_back( (*it).proc->Clone() );
		}
	}

	std::vector< StructProc * >::iterator procIt;
	for( procIt = vActableProc.begin(); procIt != vActableProc.end(); ++procIt )
	{
		(*procIt)->Proc( this, pTarget, 0, false );
		delete (*procIt);
	}
}

void StructCreature::Attack( StructCreature *pTarget, AR_TIME t, AR_TIME attack_interval, struct StructCreature::_ATTACK_INFO * arDamage, bool & bIsDoubleAttack )
{
	if( !t ) t = GetArTime();

	// 공격 시간 설정
	//SetNextAttackableTime( m_LastAttackTime == 0 ? t : m_LastAttackTime + attack_interval );
	SetNextAttackableTime(t + attack_interval);
	m_LastAttackTime = t;
	// 초기화

	int nHate = 0;
	int nAttackCount = 1;

	bIsDoubleAttack = false;

	if( IsUsingDoubleWeapon() )
		nAttackCount *= 2;		// 아픔 두배

	if( XRandom() % 100 < m_Attribute.fDoubleAttackRatio )
	{
		bIsDoubleAttack = true;
		nAttackCount *= 2;		// 아픔 두배
	}

	for( int i = 0; i < nAttackCount; ++i )
	{
		bool bLeftHandAttack = IsUsingDoubleWeapon() && i % 2; // 양손 공격이고 두번째 공격이면 왼손 공격입니다아~

		int prev_target_hp = pTarget->GetHP();
		int prev_target_mp = pTarget->GetMP();
		int prev_hp = GetHP();
		int prev_mp = GetMP();

		int nCriticalBonus = 0;

		if( m_nCriticalCount > 0 )
		{
			nCriticalBonus = 50;
		}

		if( bLeftHandAttack )
		{
			arDamage[i].SetDamageInfo( pTarget->DealPhysicalNormalLeftHandDamage( this, GetAttackPointLeft(), Elemental::TYPE_NONE, 0, nCriticalBonus ) );
		}
		else
		{
			arDamage[i].SetDamageInfo( pTarget->DealPhysicalNormalDamage( this, GetAttackPointRight(), Elemental::TYPE_NONE, 0, nCriticalBonus ) );
		}

		if( arDamage[i].bCritical )
		{
			++m_nCriticalCount;

			if( m_nCriticalCount >= 3 )
			{
				m_nCriticalCount = 0;
			}
		}

		bool bSuccess = !arDamage[i].bMiss;

		if( bSuccess )
		{
			// 소울스톤 내구도 감소
			StructItem *pItem = m_anWear[ ItemBase::WEAR_RIGHTHAND ];
			if( bLeftHandAttack )
			{
				pItem = m_anWear[ ItemBase::WEAR_LEFTHAND ];
			}

			if( pItem && pItem->GetCurrentEndurance() > 0 && pItem->GetUsingSocketCount() > 0 )
			{
				int nPrevEndurance = pItem->GetCurrentEndurance();
				pItem->SetCurrentEndurance( pItem->GetCurrentEndurance() - GameRule::ENDURANCE_REDUCE_ON_ATTACK );
				if( GameRule::GetDecreasedEndurancePoint( nPrevEndurance, pItem->GetCurrentEndurance() ) > 0 )
				{
					if( IsPlayer() )
					{
						SendItemMessage( static_cast< StructPlayer * >( this ), pItem );
					}
					else if( IsSummon() )
					{
						StructPlayer *pMaster = static_cast< StructSummon * >( this )->GetMaster();
						if( pMaster && pMaster->IsLogin() && pMaster->IsInWorld() )
						{
							SendItemMessage( pMaster, pItem );
						}
					}
				}

				if( pItem->GetCurrentEndurance() == 0 )
					SetNeedCalculateStat();
			}
		}

		// 여기서 한번 더 보정
		arDamage[i].nDamage = prev_target_hp - pTarget->GetHP();
		arDamage[i].mp_damage = prev_target_mp - pTarget->GetMP();
		arDamage[i].attacker_damage = prev_hp - GetHP();
		arDamage[i].attacker_mp_damage = prev_mp - GetMP();

		arDamage[i].target_hp = pTarget->GetHP();
		arDamage[i].target_mp = pTarget->GetMP();
		arDamage[i].attacker_hp = GetHP();
		arDamage[i].attacker_mp = GetMP();

		nHate += arDamage[i].nDamage;

		onAttack( pTarget, arDamage[i].nDamage );

		// 명중
		if( bSuccess )
		{
			// 추가타인 경우는 각종 발동이 일어나지 않게 하려는 의도
			// 이도에 추가타가 붙은 경우, 오른손, 왼손, 오른손 추가타, 왼손 추가타 순서이다.
			if( !bIsDoubleAttack || ( bIsDoubleAttack && i < nAttackCount / 2 ) )
				OnAttack( pTarget, arDamage[i].nDamage, StructState::NormalAttack, Elemental::TYPE_NONE );
		}

		// 크리티컬/블럭/퍼펙트블럭/회피
		if( arDamage[i].bCritical )		OnCritical( pTarget, arDamage[i].nDamage, StructState::NormalAttack, Elemental::TYPE_NONE );
		if( arDamage[i].bBlock )		pTarget->OnBlock( this, arDamage[i].nDamage, StructState::NormalAttack, Elemental::TYPE_NONE );
		if( arDamage[i].bPerfectBlock )	pTarget->OnPerfectBlock( this, StructState::NormalAttack, Elemental::TYPE_NONE );
		if( arDamage[i].bMiss )			pTarget->OnAvoid( this, StructState::NormalAttack, Elemental::TYPE_NONE );
	}

	if( pTarget->IsMonster() )
	{
		bool bRange = ( IsUsingBow() || IsUsingCrossBow() ) && IsPlayer();

		std::pair< float, int > HateMod = GetHateMod( 3, true );

		nHate += HateMod.second;
		nHate *= HateMod.first;

		if( bRange )
		{
			static_cast< StructMonster * >( pTarget )->AddHate( GetHandle(), m_fHateRatio * nHate * m_RangeStateAdvantage.fHate * pTarget->m_RangeStatePenalty.fHate );
		}
		else
		{
			static_cast< StructMonster * >( pTarget )->AddHate( GetHandle(), m_fHateRatio * nHate * m_NormalStateAdvantage.fHate * pTarget->m_NormalStatePenalty.fHate );
		}
	}
	else if( pTarget->IsNPC() )
	{
		static_cast< StructNPC * >( pTarget )->SetAttacker( pTarget );
	}

	return;
}


void StructCreature::broadcastAttackMessage( StructCreature *pTarget, struct StructCreature::_ATTACK_INFO * arDamage, int tm, int delay, bool bIsDoubleAttack, bool bIsAiming, bool bEndAttack, bool bCancelAttack )
{
	char buf[ sizeof( TS_ATTACK_EVENT ) + sizeof( TS_ATTACK_EVENT::ATTACK_INFO ) * 4 ]; // 최대 4개라서 이리 했음. 나중에 또 추가되면 또 늘려야 함.

	TS_ATTACK_EVENT * pMsg = new ( buf ) TS_ATTACK_EVENT();

	int nAttackCount = 1;

	if( bEndAttack || bCancelAttack )
		nAttackCount = 0;

	if( !IsFormChanged() )
	{
		if( IsUsingDoubleWeapon() )
			nAttackCount *= 2;

		if( bIsDoubleAttack )
			nAttackCount *= 2;
	}

	pMsg->size += sizeof( TS_ATTACK_EVENT::ATTACK_INFO ) * nAttackCount;

	pMsg->attack_flag = 0;

	if( !IsFormChanged() )
	{
		if( bIsDoubleAttack )					pMsg->attack_flag |= TS_ATTACK_EVENT::ATTACK_FLAG_DOUBLE_ATTACK;
		if( IsUsingDoubleWeapon() )				pMsg->attack_flag |= TS_ATTACK_EVENT::ATTACK_FLAG_DOUBLE_WEAPON;
		if( IsUsingBow() && IsPlayer() )		pMsg->attack_flag |= TS_ATTACK_EVENT::ATTACK_FLAG_BOW;
		if( IsUsingCrossBow() && IsPlayer() )	pMsg->attack_flag |= TS_ATTACK_EVENT::ATTACK_FLAG_CROSS_BOW;
	}

	pMsg->attack_action = TS_ATTACK_EVENT::ATTACK_ATTACK;
	if( bIsAiming )				pMsg->attack_action = TS_ATTACK_EVENT::ATTACK_AIMING;
	else if( bEndAttack )		pMsg->attack_action = TS_ATTACK_EVENT::ATTACK_END;
	else if( bCancelAttack )	pMsg->attack_action = TS_ATTACK_EVENT::ATTACK_CANCEL;

	pMsg->attack_speed		= tm;
	pMsg->attack_delay		= delay;
	pMsg->count = nAttackCount;
	pMsg->attacker_handle = GetHandle();

	if( pTarget )
	{
		pMsg->target_handle		= pTarget->GetHandle();		// 타겟
	}
	else
	{
		pMsg->target_handle		= 0;		// 타겟
	}


	TS_ATTACK_EVENT::ATTACK_INFO * pAttackInfo = reinterpret_cast< TS_ATTACK_EVENT::ATTACK_INFO * >( pMsg + 1 );

	for( int i = 0; i < nAttackCount; ++i )
	{
		pAttackInfo[i].damage				= arDamage[i].nDamage;
		pAttackInfo[i].mp_damage			= arDamage[i].mp_damage;
		pAttackInfo[i].attacker_damage		= arDamage[i].attacker_damage;
		pAttackInfo[i].attacker_mp_damage	= arDamage[i].attacker_mp_damage;

		pAttackInfo[i].target_hp			= arDamage[i].target_hp;
		pAttackInfo[i].target_mp			= arDamage[i].target_mp;
		pAttackInfo[i].attacker_hp			= arDamage[i].attacker_hp;
		pAttackInfo[i].attacker_mp			= arDamage[i].attacker_mp;

		pAttackInfo[i].flag = 0;

		// 플래그 설정
		if( arDamage[i].bPerfectBlock	) pAttackInfo[i].flag |= TS_ATTACK_EVENT::FLAG_PERFECT_BLOCK;
		if( arDamage[i].bBlock			) pAttackInfo[i].flag |= TS_ATTACK_EVENT::FLAG_BLOCK;
		if( arDamage[i].bMiss			) pAttackInfo[i].flag |= TS_ATTACK_EVENT::FLAG_MISS;
		if( arDamage[i].bCritical		) pAttackInfo[i].flag |= TS_ATTACK_EVENT::FLAG_CRITICAL;	

		// 루프 펼치기 신공. 이게 더 빠르거든. -_-
		pAttackInfo[i].elemental_damage[0] = arDamage[i].elemental_damage[0];
		pAttackInfo[i].elemental_damage[1] = arDamage[i].elemental_damage[1];
		pAttackInfo[i].elemental_damage[2] = arDamage[i].elemental_damage[2];
		pAttackInfo[i].elemental_damage[3] = arDamage[i].elemental_damage[3];
		pAttackInfo[i].elemental_damage[4] = arDamage[i].elemental_damage[4];
		pAttackInfo[i].elemental_damage[5] = arDamage[i].elemental_damage[5];
		pAttackInfo[i].elemental_damage[6] = arDamage[i].elemental_damage[6];
	}
	// 방송
	ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), pMsg );
}

int StructCreature::GetCriticalDamage( int damage, float critical_amp, int critical_bonus )
{
	// 크리티컬
	if( XRandom( 0, 99 ) <= ( critical_amp * GetCritical() + critical_bonus ) )
	{
		return ( damage * ( GetCriticalPower() / 100.0f ) );
	}

	return 0;
}

bool StructCreature::StartAttack( AR_HANDLE target, bool bNeedFastReaction )
{
	if( IsDead() )
	{
		return false;
	}

	if( IsAttacking() )
	{
		return false;
	}

	m_hEnemy = target;
	m_StatusFlag.Off( STATUS_ATTACK_STARTED );

	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	if( ( IsUsingBow() || IsUsingCrossBow() ) && IsPlayer() )
	{
		m_nNextAttackMode = StructCreature::AM_AIMING;
	}

	if( bNeedFastReaction ) onAttackAndSkillProcess();

	return true;
}

void StructCreature::CancelAttack()
{

	if( ( IsUsingBow() || IsUsingCrossBow() ) && IsPlayer() )
	{
		if( m_nNextAttackMode == StructCreature::AM_ATTACK )
		{
			m_nNextAttackMode = StructCreature::AM_AIMING;
			// 조준 시간 초기화.
			SetNextAttackableTime( GetArTime() );
		}
	}

	if( isAttackStarted() )
	{

		StructCreature::iterator it = StructCreature::get( m_hEnemy );
		StructCreature * pCreature = (*it);

		_ATTACK_INFO info[4];

		broadcastAttackMessage( pCreature, info, 0, 0, false, false, false, true );
	}

	m_hEnemy = 0;

	m_StatusFlag.On( STATUS_FIRST_ATTACK );
}
void StructCreature::EndAttack()
{
	if( ( IsUsingBow() || IsUsingCrossBow() ) && IsPlayer() )
	{
		if( m_nNextAttackMode == StructCreature::AM_ATTACK )
		{
			m_nNextAttackMode = StructCreature::AM_AIMING;
			// 조준 시간 초기화.
			SetNextAttackableTime( GetArTime() );
		}
	}

	if( isAttackStarted() )
	{

		StructCreature::iterator it = StructCreature::get( m_hEnemy );
		StructCreature * pCreature = (*it);

		_ATTACK_INFO info[4];

		if( ( IsPlayer() || IsSummon() ) && pCreature )
			broadcastAttackMessage( pCreature, info, 0, 0, false, false, true );
	}

	m_hEnemy = 0;

	m_StatusFlag.On( STATUS_FIRST_ATTACK );
}

void StructCreature::ProcessAttack()
{
	if( IsAttacking() )
	{
		processAttack();
	}
}

void StructCreature::onAttackAndSkillProcess()
{
	if( IsUsingSkill() )
	{
		processSkill();
	}
	else if( IsAttacking() )
	{
		processAttack();
	}

}

void StructCreature::processSkill()
{
	// m_pCastSkill 멤버가 지역락의 보호를 받아야 하지만 StructSkill::ProcSkill은 지역락을 걸지 않고 호출해야 하므로...
	StructSkill * pCastSkill = NULL;
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		pCastSkill = m_pCastSkill;
	}

	if( pCastSkill )
	{
		pCastSkill->ProcSkill();
		return;
	}
}

void StructCreature::OnAttack( StructCreature *pTarget, const int nDamage, const DWORD nAttackType, const int nElementalType, const int nSkillId )
{
	ProcByAttack( pTarget, nDamage, true, nAttackType, nElementalType );
	if( pTarget )
		pTarget->ProcByAttack( this, nDamage, false, nAttackType, nElementalType );

	// 스킬 아이디를 기준으로 핸들러 호출
	// 현재는 특정 스킬 피격 시에 대한 유형은 존재하지 않음
	if( nSkillId )
	{
		ProcBySkillId( pTarget, nDamage, true, nSkillId );
	}
	else
	{
		if (IsEnemy(pTarget) && m_nAttackLuna != 0 && pTarget->GetLuna() == 0)
		{
			pTarget->AddState(StructState::LUNA_CHIP, GetHandle(), 1, GetArTime(), GetArTime() + 18000);
		}
	}
}

void StructCreature::OnKill( StructCreature *pTarget )
{
	ProcByKill( pTarget, true );
	if( pTarget )
		pTarget->ProcByKill( this, false );
}

void StructCreature::OnCritical( StructCreature *pTarget, const int nDamage, const DWORD nAttackType, const int nElementalType )
{
	ProcByCritical( pTarget, nDamage, true, nAttackType, nElementalType );
	if( pTarget )
		pTarget->ProcByCritical( this, nDamage, false, nAttackType, nElementalType );

	// 기존 유형 유지
	ProcessAddHPMPOnCritical();
}

void StructCreature::OnAvoid( StructCreature *pFrom, const DWORD nAttackType, const int nElementalType )
{
	ProcByAvoid( pFrom, nAttackType, nElementalType );
}

void StructCreature::OnBlock( StructCreature *pFrom, const int nDamage, const DWORD nAttackType, const int nElementalType )
{
	ProcByBlock( pFrom, nDamage, nAttackType, nElementalType );

	// 기존 유형 유지
	AR_TIME t = GetArTime();
	for( std::vector< StateReflectInfo >::iterator it = m_vStateReflectInfo.begin() ; it != m_vStateReflectInfo.end() ; ++it )
	{
		if( !pFrom->IsNPC() )
			pFrom->AddState( (*it).nCode, GetHandle(), (*it).nLevel, t, t + (*it).nDuration );
	}
}

void StructCreature::OnPerfectBlock( StructCreature *pFrom, const DWORD nAttackType, const int nElementalType )
{
	ProcByPerfectBlock( pFrom, nAttackType, nElementalType );
}

void StructCreature::processAttack()
{
	AR_TIME	t = GetArTime();

	// 행동 못하면 KIN
	if( !IsAttackable() )
	{
		return;
	}

	bool bIsUsingBow		= IsUsingBow();
	bool bIsUsingCrossBow	= IsUsingCrossBow();

	// 공격 가능 시간 안되었다면 KIN
	if( GetNextAttackableTime() > t ) 
	{
		return;
	}

	// 활일 경우
	if( ( bIsUsingBow || bIsUsingCrossBow ) && IsPlayer() )
	{
		// 화살 없으면 KIN
		if( GetBulletCount() < 1 )
		{
			EndAttack();
			return;
		}
	}

	// 적 포인터를 얻어옴
	StructCreature::iterator it = StructCreature::get( m_hEnemy );
	StructCreature *pEnemy = *it;

	// 근처 범위 Lock
	ArcadiaAutoLock _lock;

	if( pEnemy )
	{
		_lock.set( ArcadiaServer::Instance().LockObjects( this, pEnemy ), __FILE__, __LINE__ );
	}
	else
	{
		if( IsPlayer() )
		{
			SendCantAttackMsg( static_cast< StructPlayer * >( this ), GetHandle(), m_hEnemy, RESULT_NOT_EXIST );
		}
		else if( IsSummon() )
		{
			SendCantAttackMsg( static_cast< StructSummon * >( this )->GetMaster(), GetHandle(), m_hEnemy, RESULT_NOT_EXIST );
		}

		EndAttack();

		return;
	}

	if( IsDead() )
	{
		CancelAttack();
		return;
	}

	// 이동중이라면 KIN
	if( IsMoving( t ) )
	{
		return;
	}

	// 공격중이 아니라면 KIN
	if( !IsAttacking() ) 
	{
		return;
	}

	// 적이 유효하지 않다면 KIN
	if( !pEnemy || !IsEnemy( pEnemy ) || pEnemy->IsDead() || !pEnemy->IsInWorld() || !IsVisibleRegion( GetRX(), GetRY(), pEnemy->GetRX(), pEnemy->GetRY() ) )
	{
		if( IsPlayer() )
			SendCantAttackMsg( static_cast< StructPlayer * >( this ), GetHandle(), m_hEnemy, RESULT_NOT_EXIST );
		else if( IsSummon() )
			SendCantAttackMsg( static_cast< StructSummon * >( this )->GetMaster(), GetHandle(), m_hEnemy, RESULT_NOT_EXIST );

		EndAttack();
		return;
	}

	if( isFirstAttack() )
	{
		pEnemy->OnUpdate();

		m_StatusFlag.Off( STATUS_FIRST_ATTACK );
	}

	if( pEnemy->IsDead() )
	{
		if( IsPlayer() )
			SendCantAttackMsg( static_cast< StructPlayer * >( this ), GetHandle(), m_hEnemy, RESULT_NOT_EXIST );
		else if( IsSummon() )
			SendCantAttackMsg( static_cast< StructSummon * >( this )->GetMaster(), GetHandle(), m_hEnemy, RESULT_NOT_EXIST );

		EndAttack();
		return;
	}

	// 내위치, 상대위치, 거리 얻자	
	ArPosition	enemyPosition	= pEnemy->GetCurrentPosition( t );
	ArPosition	myPosition		= GetCurrentPosition( t );
	AR_UNIT		distance		= myPosition.GetDistance( enemyPosition );
	distance -=  ( pEnemy->GetUnitSize()/2 + GetUnitSize()/2 );
	AR_UNIT		attack_range	= GetRealAttackRange();

	// 타겟 방향을 향하도록..
	SetDirection( enemyPosition );

	{
		if( pEnemy->IsMoving() )
			attack_range *= 1.5f;
		else
			attack_range *= 1.2f;
	}

	// 변수
	_ATTACK_INFO Damages[4];
	bool bIsDoubleAttack = false;

	int nAttackTime = GetAttackInterval();		// 순수 공격에만 소요되는 시간
	int nAttackInterval = GetAttackInterval();	// 다음번 공격까지의 interval

	if (m_LastAttackTime == 0)
	{
		m_LastAttackTime = t;
	}
	
	// 이번에 수행할 공격 모드 저장
	int nCurrentAttackMode = m_nNextAttackMode;

	// 사거리에 못미친다면..
	if( distance > attack_range )
	{
		onCantAttack( pEnemy->GetHandle(), t );
		return;
	}

	// 활이라면..?
	if( ( bIsUsingBow || bIsUsingCrossBow ) && IsPlayer() )
	{
		// 장전 처리를 해야 한다면
		if( nCurrentAttackMode == StructCreature::AM_AIMING ) 
		{
			nAttackTime = GetBowAttackInterval() * GameRule::BOW_AIMING_TIME_RATIO;
			// 장전시간 설정
			nAttackInterval = nAttackTime;

			SetNextAttackableTime( t + nAttackInterval );

			// 다음번에는 공격하세~
			m_nNextAttackMode	= StructCreature::AM_ATTACK;
		}
		else
		{
			nAttackTime = GetBowAttackInterval() * ( 1.0f - GameRule::BOW_AIMING_TIME_RATIO );
			nAttackInterval = nAttackTime + GetBowInterval();

			// 다음번에는 장전하세~
			m_nNextAttackMode	= StructCreature::AM_AIMING;
		}
	} else nCurrentAttackMode = StructCreature::AM_ATTACK;

	// 공격 모드라면 두들겨 패자~~
	if( nCurrentAttackMode == StructCreature::AM_ATTACK )
	{
		if( ( bIsUsingBow || bIsUsingCrossBow ) && IsPlayer() )
		{
			static_cast< StructPlayer * >( this )->EraseBullet( 1 );
		}

		SetMovableTime( t + nAttackTime );

		Attack( pEnemy, t, nAttackInterval, Damages, bIsDoubleAttack );
	}

	m_StatusFlag.On( STATUS_ATTACK_STARTED );

	if( IsFormChanged() )
	{
		// sum all damage info

		int nAttackCount = 1;
		if( IsUsingDoubleWeapon() ) nAttackCount = 2;

		if( bIsDoubleAttack ) nAttackCount *= 2;

		for( int i = 1; i < nAttackCount; ++i )
		{
			// damage는 더하고
			Damages[0].nDamage += Damages[i].nDamage;
			Damages[0].mp_damage += Damages[i].mp_damage;
			Damages[0].attacker_damage += Damages[i].attacker_damage;
			Damages[0].attacker_mp_damage += Damages[i].attacker_mp_damage;

			// 체력등의 결과는 최종 버전으로
			Damages[0].target_hp = Damages[i].target_hp;
			Damages[0].target_mp = Damages[i].target_mp;
			Damages[0].attacker_hp = Damages[i].attacker_hp;
			Damages[0].attacker_mp = Damages[i].attacker_mp;

			// 플래그는 적당히 합치고...
			Damages[0].bPerfectBlock	|=	Damages[i].bPerfectBlock;
			Damages[0].bBlock			|=	Damages[i].bBlock;
			Damages[0].bMiss			&=	Damages[i].bMiss;			// 미스는 모두 미스일때만
			Damages[0].bCritical		|=	Damages[i].bCritical;


			// 루프 펼치기 신공. 이게 더 빠르거든. -_-
			Damages[0].elemental_damage[0] += Damages[i].elemental_damage[0];
			Damages[0].elemental_damage[1] += Damages[i].elemental_damage[1];
			Damages[0].elemental_damage[2] += Damages[i].elemental_damage[2];
			Damages[0].elemental_damage[3] += Damages[i].elemental_damage[3];
			Damages[0].elemental_damage[4] += Damages[i].elemental_damage[4];
			Damages[0].elemental_damage[5] += Damages[i].elemental_damage[5];
			Damages[0].elemental_damage[6] += Damages[i].elemental_damage[6];
		}
	}

	// 메세지 방송
	if( !IsFormChanged() || nCurrentAttackMode != StructCreature::AM_AIMING )
		broadcastAttackMessage( pEnemy, Damages, nAttackTime * 10, ( GetNextAttackableTime() - t ) * 10, bIsDoubleAttack, nCurrentAttackMode == StructCreature::AM_AIMING );
}

unsigned short StructCreature::onItemUseEffect( struct StructCreature *pCaster, struct StructItem* pItem, int type, c_fixed10 var1, c_fixed10 var2, const char *szParameter )
{
	unsigned short nRet = RESULT_SUCCESS;

	switch( type )
	{
	case ITEM_EFFECT_INSTANT::INC_HP		:
		{
			int prev_hp = GetHP();
			HealByItem( var1 );
			BroadcastHPMPMsg( this, GetHP() - prev_hp, 0 );
			break;
		}
	case ITEM_EFFECT_INSTANT::INC_MP		:
		{
			int prev_mp = GetMP();
			MPHealByItem( var1 );
			BroadcastHPMPMsg( this, 0, GetMP() - prev_mp );

			break;
		}
	case ITEM_EFFECT_INSTANT::INC_HP_PERCENT:
		{
			int prev_hp = GetHP();
			HealByItem( var1 * GetMaxHP() );
			BroadcastHPMPMsg( this, GetHP() - prev_hp, 0 );

			break;
		}

	case ITEM_EFFECT_INSTANT::INC_MP_PERCENT:
		{
			int prev_mp = GetMP();
			MPHealByItem( var1 * GetMaxMP() );
			BroadcastHPMPMsg( this, 0, GetMP() - prev_mp );

			break;
		}
	case ITEM_EFFECT_INSTANT::INC_STAMINA	:
		{
			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}
			static_cast< StructPlayer * >( this )->AddStamina( var1 );

			break;
		}	

	case ITEM_EFFECT_INSTANT::RESURECTION	:	
		{
			nRet = RESULT_ACCESS_DENIED;
			// 자신에게는 못씀
			if( pItem->GetOwnerHandle() == GetHandle() )
				break;

			// 안죽었으면 못씀
			if( IsAlive() ) break;

			// 죽었더라도 내구도가 다한 소환수는 살릴 수 없음
			if( IsSummon() )
			{
				StructItem *pCardItem = static_cast< StructSummon * >( this )->GetParentCard();

				if( pCardItem && pCardItem->GetMaxEtherealDurability() && !pCardItem->GetCurrentEtherealDurability() )
					break;
			}

			if( !pCaster->IsAlly( this ) )
				break;

			int prev_hp = GetHP();
			AddHP( GetMaxHP() * 0.2f );
			BroadcastHPMPMsg( this, GetHP() - prev_hp, 0 );

			ClearRemovedStateByDead();

			if( IsPlayer() )
			{
				StructPlayer *pPlayer = static_cast< StructPlayer * >(this);
				StructPlayer *pCastPlayer = ( pCaster->IsPlayer() ) ? static_cast< StructPlayer * >( pCaster ) : NULL;

				if( pPlayer->IsCompeteDead() )
					pPlayer->SetCompeteDead( false );

				LOG::Log11N4S( LM_CHARACTER_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(),
					CRT_ITEM, ( pCastPlayer ) ? pCastPlayer->GetAccountID() : 0, ( pCastPlayer ) ? pCastPlayer->GetPlayerUID() : 0, 0, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), 0, pPlayer->GetEXP(),
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, ( pCastPlayer ) ? pCastPlayer->GetAccountName() : "", LOG::STR_NTS, ( pCastPlayer ) ? pCastPlayer->GetName() : "" , LOG::STR_NTS );
			}
			else if( IsSummon() )
			{
				StructSummon *pSummon = static_cast< StructSummon * >(this);
				StructPlayer *pPlayer = static_cast< StructPlayer * >(pSummon->GetMaster());
				LOG::Log11N4S( LM_SUMMON_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(), pSummon->GetSID(), 0, 0, 0, 0, 0, 0, 0, pSummon->GetEXP(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
			}

			nRet = RESULT_SUCCESS;

			break;
		}

	case ITEM_EFFECT_INSTANT::WARP			:	
		{
			break;
		}

	case ITEM_EFFECT_INSTANT::SKILL			:	// 스킬 호출
		{
			AR_HANDLE hTarget = GetHandle();

			// 귀환/복귀의 깃털 스킬일 경우 타겟을 아이템 핸들로 대체(소켓에 이동할 좌표가 들어있으므로)
			if( var1 == StructSkill::SKILL_RETURN_FEATHER || var1 == StructSkill::SKILL_RETURN_BACK_FEATHER )
				hTarget = pItem->GetHandle();

			nRet = pCaster->CastSkill( var1, var2, hTarget, ArPosition(), GetLayer(), pItem->GetHandle() );
			break;
		}

	case ITEM_EFFECT_INSTANT::ADD_STATE			:	// 지속효과 호출
	case ITEM_EFFECT_INSTANT::ADD_STATE_EX		:	// 확장 지속효과 호출
		{
			StructState::StateCode eCode = static_cast< StructState::StateCode >( ( type == ITEM_EFFECT_INSTANT::ADD_STATE ) ? pItem->GetStateCode() : (int)var1 );

			if( IsPlayer() )
			{
				StructPlayer *pPlayer = static_cast< StructPlayer * >( this );
				const StateInfo * pStateInfo = GameContent::GetStateInfo( eCode );

				if( !pStateInfo )
				{
					nRet = RESULT_NOT_ACTABLE;
					break;
				}

				if( pStateInfo->effect_type == StructState::EF_RIDING )
				{
					if( pPlayer->IsRiding() || pPlayer->HasRidingState() || !pPlayer->IsMountable( true ) )
					{
						nRet = RESULT_ACCESS_DENIED;
						break;
					}
				}

				if( eCode == StructState::STAMINA_SAVE )
				{
					// 스테미너 효과가 더블 플러스 PC방에서 적용되지 않는 경우는 스테미너 세이버(비매품 포함)는 프리미엄 PC방에서는 사용 불가
					if( pPlayer->GetPCBangMode() == GameRule::PCBANG_PREMIUM_BONUS && !GameRule::bApplyStaminaBonusInPremiumPCBang )
					{
						nRet = RESULT_ALREADY_STAMINA_SAVED;
						break;
					}

					// 게임 중독 방지 시스템 관련 스테미너 세이버 사용 방지 체크
					if( pPlayer->IsGameTimeLimited() )
					{
						if( pPlayer->GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
						{
							nRet = RESULT_GAMETIME_HARMFUL_STAMINA_SAVER;
							break;
						}
						else if( pPlayer->GetContinuousPlayTime() >= GameRule::nMaxHealthyGameTime )
						{
							nRet = RESULT_GAMETIME_TIRED_STAMINA_SAVER;
							break;
						}
					}

					// 스태미너 세이버 효과가 있는 상태라면 사용 불가
					if( pPlayer->GetState( StructState::STAMINA_SAVE ) )
					{
						nRet = RESULT_ALREADY_STAMINA_SAVED;
						break;
					}
				}

				if( eCode == StructState::SUPER_SAVE_0 ||
					eCode == StructState::SUPER_SAVE_1 ||
					eCode == StructState::SUPER_SAVE_2 ||
					eCode == StructState::SUPER_SAVE_3 )
				{
					// 게임 중독 방지 시스템 관련 성장의 물약(구 슈퍼 세이버) 사용 방지 체크
					if( pPlayer->IsGameTimeLimited() )
					{
						if( pPlayer->GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
						{
							nRet = RESULT_GAMETIME_HARMFUL_SUPER_SAVER;
							break;
						}
						else if( pPlayer->GetContinuousPlayTime() >= GameRule::nMaxHealthyGameTime )
						{
							nRet = RESULT_GAMETIME_TIRED_SUPER_SAVER;
							break;
						}
					}

					// 성장의 물약(구 슈퍼 세이버)는 중첩 불가
					if( pPlayer->GetState( eCode ) )
					{
						nRet = RESULT_ALREADY_SUPER_SAVER;
						break;
					}
				}
			}

			AR_TIME t = GetArTime();
			int nLevel = ( type == ITEM_EFFECT_INSTANT::ADD_STATE ) ? pItem->GetStateLevel() : var2;
			nRet = AddState( static_cast< StructState::StateCode >( eCode ), pItem->GetOwnerHandle(), nLevel, t, t + pItem->GetStateTime()*100 );
			break;
		}

	case ITEM_EFFECT_INSTANT::REMOVE_STATE			:	// 지속효과 제거
		{
			RemoveState( (StructState::StateCode)pItem->GetStateCode(), pItem->GetStateLevel() );
			break;
		}

	case ITEM_EFFECT_INSTANT::TOGGLE_STATE			:
		{
			StructItem *pWearingRideItem = GetWearedItem( ItemBase::WEAR_RIDE_ITEM );

			// 내리기는 탑승시 사용한 아이템(장착되어 있음)으로만 가능
			if( GetState( static_cast< StructState::StateCode >( pItem->GetStateCode() ) ) && pWearingRideItem == pItem )
			{
				RemoveState( (StructState::StateCode)pItem->GetStateCode(), pItem->GetStateLevel() );
				break;
			}
			else
			{
				if( IsPlayer() )
				{
					StructPlayer *pPlayer = static_cast< StructPlayer * >( this );
					if( pPlayer->IsRiding() || pPlayer->HasRidingState() || !pPlayer->IsMountable( true ) )
					{
						const StateInfo * pStateInfo = GameContent::GetStateInfo( pItem->GetStateCode() );

						if( pStateInfo->effect_type == StructState::EF_RIDING )
						{
							nRet = RESULT_ACCESS_DENIED;
							break;
						}
					}

					if( pWearingRideItem )
					{
						if( pWearingRideItem != pItem && ( pPlayer->Putoff( ItemBase::WEAR_RIDE_ITEM ) != RESULT_SUCCESS || pPlayer->Puton( ItemBase::WEAR_RIDE_ITEM, pItem ) != RESULT_SUCCESS ) )
						{
							nRet = RESULT_ACCESS_DENIED;
							break;
						}
					}
					else if( pPlayer->Puton( ItemBase::WEAR_RIDE_ITEM, pItem ) != RESULT_SUCCESS )
					{
						nRet = RESULT_ACCESS_DENIED;
						break;
					}
				}

				AR_TIME t = GetArTime();
				nRet = AddState( (StructState::StateCode)pItem->GetStateCode(), pItem->GetOwnerHandle(), pItem->GetStateLevel(), t, -1, true );
				break;
			}
		}

	case ITEM_EFFECT_INSTANT::GENERATE_ITEM			:	// 아이템 생성
		{
			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if( var1 == 0 )
			{
				StructPlayer * pPlayer = static_cast< StructPlayer * >( this );
				if( pPlayer->ChangeGold( pPlayer->GetGold() + var2.GetAsInt64() ) != RESULT_SUCCESS )
				{
					nRet = RESULT_TOO_MUCH_MONEY;
					break;
				}

				LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 0, 0, var2, pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "IGEN", LOG::STR_NTS );
			}
			else
			{
				StructPlayer * pPlayer = static_cast< StructPlayer * >( this );

				// 일반 아이템 생성이면 그 아이템을 var2개 만큼 생성, 드랍 그룹 아이템 생성이면 var2 횟수만큼 드랍 그룹 반복 선택
				int nGenCount = ( var1 >= 0 ) ? 1 : var2;
				//bool bIsBlank = true;

				for( int i = 0 ; i < nGenCount ; ++i )
				{
					ItemBase::ItemCode nItemID = var1;
					__int64 nItemCount = var2;

					while( nItemID < 0 )
					{
						GameContent::SelectItemIDFromDropGroup( nItemID, nItemID, nItemCount );
					}

					if( nItemID )
					{
						//bIsBlank = true;

						StructItem *pItem = StructItem::AllocItem( 0, nItemID, nItemCount, ItemInstance::BY_ITEM );

						// 아이템 획득 메시지를 보여주기 위해 그냥 보내주기
						StructItem *pNewItem = static_cast< StructPlayer *>( this )->PushItem( pItem, pItem->GetCount() );	

						if( pNewItem )
						{
							SendResult( this, TM_CS_TAKE_ITEM, RESULT_SUCCESS, pNewItem->GetHandle() );
							LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewItem ? pNewItem->GetCount() : 0, 0, 0, pPlayer->GetX(), pPlayer->GetY(), pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "IGEN", LOG::STR_NTS );
						}

						if( pNewItem != pItem ) 
						{
							StructItem::PendFreeItem( pItem );
						}
					}
					else
					{
						LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 0, 0, 0, pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "IGEN", LOG::STR_NTS );
					}
				}

				// 2008.12.02 꽝 처리 일시적으로 제거(2008년 크리스마스 이벤트 관련)
				// 아이템 사용함으로 인해 아이템이 하나도 안 나온 경우에만 꽝 메시지 출력
				// 아이템 하나에서 드랍그룹 2개 이상 사용 시에 아이템을 1개는 얻었고 나머지 드랍 그룹에서는 꽝이 나와도 꽝은 아니므로...
				//if( bIsBlank )
				//{
				//	PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@6696" );
				//}
			}

			break;
		}

	case ITEM_EFFECT_INSTANT::INC_GOLD:
		{
			StructPlayer * pPlayer = static_cast< StructPlayer * >( this );
			StructGold nIncGold( XRandom( var1, var2 ) );

			if( pPlayer->ChangeGold( pPlayer->GetGold() + nIncGold ) != RESULT_SUCCESS )
			{
				nRet = RESULT_TOO_MUCH_MONEY;
				break;
			}

			LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 0, 0, nIncGold.GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "IGEN", LOG::STR_NTS );
			break;
		}

	case ITEM_EFFECT_INSTANT::INC_HUNTAHOLIC_POINT:
		{
			StructPlayer * pPlayer = static_cast< StructPlayer * >( this );
			int nIncHuntaholicPoint( XRandom( var1, var2 ) );

			pPlayer->SetHuntaholicPoint( pPlayer->GetHuntaholicPoint() + nIncHuntaholicPoint );

			LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, ItemBase::ITEM_CODE_HUNTAHOLIC_POINT_TICKET, nIncHuntaholicPoint, pPlayer->GetHuntaholicPoint(), 0, pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "IGEN", LOG::STR_NTS );

			break;
		}

	case ITEM_EFFECT_INSTANT::RESET_SKILL:	// 리트레이닝 포션
		{
			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if( GetJobDepth() < var1 )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			nRet = ResetSkill( SRM_ITEM ) ? RESULT_SUCCESS : RESULT_ACCESS_DENIED;
			break;
		}
	case ITEM_EFFECT_INSTANT::RESET_JOB:
		{
			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if( GetJobDepth() < var1 )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			nRet = static_cast< StructPlayer * >( this )->ResetJob( 0 ) ? RESULT_SUCCESS : RESULT_ACCESS_DENIED;
			break;
		}

	case ITEM_EFFECT_INSTANT::RESET_SUMMON_SKILL:
		{
			if( !IsSummon() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			// 랜덤 스킬 초기화 유형은 랜덤스킬을 가진 소울 크리처에만 사용 가능하다.
			// IsSoulCreature 함수의 인자를 위해 기본형의 소환수 코드를 가져온다.
			StructSummon * pSummon = static_cast< StructSummon * >( this );
			int nBasicSummonCode = pSummon->GetSummonCode();
			if( pSummon->GetTransformLevel() == SummonBase::EVOLVE_GROWTH || pSummon->GetTransformLevel() == SummonBase::EVOLVE_EVOLVE )
			{
				nBasicSummonCode = pSummon->GetPrevJobId( 0 );
			}

			if( GameContent::IsSoulCreature( nBasicSummonCode ) )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if( !IsInWorld() || IsDead() ||
				static_cast< StructSummon * >( this )->GetMaster() != pCaster )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			nRet = ResetSkill( SRM_ITEM, 0, true ) ? RESULT_SUCCESS : RESULT_ACCESS_DENIED;

			break;
		}

	case ITEM_EFFECT_INSTANT::FORCE_RESET_SUMMON_SKILL:
		{
			if( !IsSummon() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			// 랜덤 스킬 초기화 유형은 랜덤스킬을 가진 소울 크리처에만 사용 가능하다.
			// IsSoulCreature 함수의 인자를 위해 기본형의 소환수 코드를 가져온다.
			StructSummon * pSummon = static_cast< StructSummon * >( this );
			int nBasicSummonCode = pSummon->GetSummonCode();
			if( pSummon->GetTransformLevel() == SummonBase::EVOLVE_GROWTH || pSummon->GetTransformLevel() == SummonBase::EVOLVE_EVOLVE )
			{
				nBasicSummonCode = pSummon->GetPrevJobId( 0 );
			}
			if( !GameContent::IsSoulCreature( nBasicSummonCode ) )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if( !IsInWorld() || IsDead() ||
				static_cast< StructSummon * >( this )->GetMaster() != pCaster )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			nRet = ResetSkill( SRM_ITEM, 0, true ) ? RESULT_SUCCESS : RESULT_ACCESS_DENIED;

			break;
		}

	case ITEM_EFFECT_INSTANT::RECALL:
		{
			if( !IsPlayer() || !pCaster->IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer * pThisPlayer = static_cast< StructPlayer * >( this );
			StructPlayer * pCastPlayer = static_cast< StructPlayer * >( pCaster );

			// 시크루트는 들어가도 아무것도 못하므로 그냥 놔두기
			if( pCastPlayer->IsInSiegeOrRaidDungeon() )
			{
				nRet = RESULT_NOT_ACTABLE_HERE;
				break;
			}

			if( pCastPlayer->IsInInstanceDungeon() )
			{
				nRet = RESULT_NOT_ACTABLE_IN_INSTANCE_DUNGEON;
				break;
			}

			char szBuf[255];
			s_sprintf( szBuf, _countof( szBuf ), "recall_feather( %f, %f, %d )", pCastPlayer->GetX(), pCastPlayer->GetY(), pCastPlayer->GetLayer() );

			if( var1 == 1.0f ) // 한 사람만 호출
			{
				// 시체거나, 파티원이 아니거나 창고 사용 중, 혹은 자신이면 사용 실패
				if( pThisPlayer->IsDead() || !pThisPlayer->GetPartyID() || pThisPlayer->GetPartyID() != pCastPlayer->GetPartyID()
					|| pThisPlayer->IsUsingStorage() || pCastPlayer == pThisPlayer )
				{
					nRet = RESULT_ACCESS_DENIED;
					break;
				}

				// 일반 던전일 경우 레벨 제한 체크
				if( pCastPlayer->IsInDungeon() )
				{
					int nDungeonID = DungeonManager::Instance().GetDungeonID( pCastPlayer->GetX(), pCastPlayer->GetY() );
					if( nDungeonID && DungeonManager::IsRestrictedToEnter( DungeonManager::Instance().GetDungeonLevel( nDungeonID ), pThisPlayer->GetLevel() ) )
					{
						// 이 곳으로 소환하기엔 레벨이 딸립니다.
						PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pCastPlayer, "@563\v#@player_name@#\v%s", pThisPlayer->GetName() );
						PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pThisPlayer, "@563\v#@player_name@#\v%s", pThisPlayer->GetName() );
						nRet = RESULT_ACCESS_DENIED;
						break;
					}
				}

				pThisPlayer->SetFixedDialogTrigger( szBuf );
				pThisPlayer->SetNonNPCDialog( true );

				SendWindowMessage( pThisPlayer, "recall_feather_confirm_window", pCastPlayer->GetName(), szBuf );

				LOG::Log11N4S( LM_ITEM_RECALL_FEATHER_REQUEST, pCastPlayer->GetAccountID(), pCastPlayer->GetSID(), pCastPlayer->GetPartyID(),
					pCastPlayer->GetX(), pCastPlayer->GetY(), pCastPlayer->GetLayer(),
					pThisPlayer->GetAccountID(), pThisPlayer->GetSID(), pThisPlayer->GetX(), pThisPlayer->GetY(), pThisPlayer->GetLayer(),
					pCastPlayer->GetAccountName(), LOG::STR_NTS, pCastPlayer->GetName(), LOG::STR_NTS, pThisPlayer->GetAccountName(), LOG::STR_NTS, pThisPlayer->GetName(), LOG::STR_NTS );
			}
			else	// 파티 전체 호출, 실제 var1 은 8이어야 함. ItemDB 참조.
			{
				if( !pCastPlayer->GetPartyID() )
				{
					nRet = RESULT_ACCESS_DENIED;
					break;
				}

				struct RecallFeatherCheckFunctor : PartyManager::PartyFunctor
				{
					RecallFeatherCheckFunctor( StructPlayer * pCaster, StructItem *pItem ) : m_pCaster( pCaster ), m_pItem( pItem ), m_nResult( RESULT_SUCCESS ) {};

					virtual bool operator()( AR_HANDLE handle )
					{
						if( m_nResult != RESULT_SUCCESS )
							return false;

						StructPlayer::iterator pit = StructPlayer::get( handle );
						StructPlayer *pPlayer = *pit;

						if( pPlayer == m_pCaster )
							return true;

						if( pPlayer->IsInEventmap() )
						{
							m_nResult = RESULT_TARGET_IN_EVENTMAP;
							return false;
						}

						if( HuntaholicManager::Instance().GetHuntaholicID( pPlayer->GetPos() ) )
						{
							m_nResult = RESULT_TARGET_IN_HUNTAHOLIC;
							return false;
						}

						if( !pPlayer || !pPlayer->GetPartyID() || pPlayer->GetPartyID() != m_pCaster->GetPartyID() || pPlayer->IsUsingStorage() )
						{
							m_nResult = RESULT_NOT_ACTABLE;
							return false;
						}

						if( m_pCaster->IsInDungeon() )
						{
							int nDungeonID = DungeonManager::Instance().GetDungeonID( m_pCaster->GetX(), m_pCaster->GetY() );
							if( nDungeonID && DungeonManager::IsRestrictedToEnter( DungeonManager::Instance().GetDungeonLevel( nDungeonID ), pPlayer->GetLevel() ) )
							{
								// 이 곳으로 소환하기엔 레벨이 딸립니다.
								PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pCaster, "@563\v#@player_name@#\v%s", pPlayer->GetName() );
								PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@563\v#@player_name@#\v%s", pPlayer->GetName() );
								m_nResult = RESULT_ACCESS_DENIED;
								return false;
							}
						}

						return true;
					}
					StructPlayer * m_pCaster;
					StructItem *m_pItem;
					int m_nResult;
				} foChecker( pCastPlayer, pItem );

				size_t nCount = PartyManager::GetInstance().DoEachMember( pCastPlayer->GetPartyID(), foChecker );

				// 아이템을 사용한 플레이어를 제외하고 한 명 이상의 플레이어가 로그인되어 있어야 한다.(최소 두 명)
				if( nCount < 2 )
				{
					nRet = RESULT_NOT_EXIST;
					break;
				}

				if( foChecker.m_nResult != RESULT_SUCCESS )
				{
					nRet = foChecker.m_nResult;
					break;
				}

				struct RecallFeatherFunctor : PartyManager::PartyFunctor
				{
					RecallFeatherFunctor( StructPlayer * pCaster, const char * _szTrigger ) : m_pCaster( pCaster ), szTrigger( _szTrigger ) {};

					virtual bool operator()( AR_HANDLE handle )
					{
						StructPlayer::iterator pit = StructPlayer::get( handle );
						StructPlayer *pPlayer = *pit;

						if( !pPlayer || pPlayer->IsDead() || pPlayer == m_pCaster
							|| !pPlayer->GetPartyID() || pPlayer->GetPartyID() != m_pCaster->GetPartyID() || pPlayer->IsUsingStorage() )
							return true;

						pPlayer->SetFixedDialogTrigger( szTrigger );
						pPlayer->SetNonNPCDialog( true );

						SendWindowMessage( pPlayer, "recall_feather_confirm_window", m_pCaster->GetName(), szTrigger );

						LOG::Log11N4S( LM_ITEM_RECALL_FEATHER_REQUEST, m_pCaster->GetAccountID(), m_pCaster->GetSID(), m_pCaster->GetPartyID(),
							m_pCaster->GetX(), m_pCaster->GetY(), m_pCaster->GetLayer(),
							pPlayer->GetAccountID(), pPlayer->GetSID(), pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
							m_pCaster->GetAccountName(), LOG::STR_NTS, m_pCaster->GetName(), LOG::STR_NTS, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS );

						return true;
					}

					StructPlayer * m_pCaster;
					const char * szTrigger;
				} fo( pCastPlayer, szBuf );

				PartyManager::GetInstance().DoEachMember( pCastPlayer->GetPartyID(), fo );
			}
			break;
		}

	case ITEM_EFFECT_INSTANT::ADD_IMMORAL_POINT:
		{
			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer *pPlayer = static_cast< StructPlayer * >(this);

			// 모럴 상태에서는 아이템을 사용할 수 없으며, 이모럴일 때에는 이모럴 수치가 깎일 수치보다 적다고 해도 그냥 적용
			if( pPlayer->GetImmoralPoint() <= 0 )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			pPlayer->SetImmoralPoint( std::max( 0.0, static_cast< double >( pPlayer->GetImmoralPoint() ) + var1 ) );
			BroadcastStatusMessage( pPlayer );
		}
		break;

	case ITEM_EFFECT_INSTANT::SET_IMMORAL_POINT:
		{
			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer *pPlayer = static_cast< StructPlayer * >(this);

			// 세팅하려는 수치보다 이모럴 포인트가 같거나 낮은 상태에서는 아이템을 사용할 수 없음
			if( pPlayer->GetImmoralPoint() <= var1 )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			pPlayer->SetImmoralPoint( std::max( 0.0, double(var1) ) );
			BroadcastStatusMessage( pPlayer );
		}
		break;

	case ITEM_EFFECT_INSTANT::RENAME_SUMMON:
		{
			if( !IsSummon() || static_cast< StructSummon * >(this)->GetRidingInfo() == SummonBase::RIDING_LENT )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructSummon *pSummon = static_cast< StructSummon * >( this );
			StructPlayer *pPlayer = pSummon->GetMaster();

			// pPlayer->GetSubSummon 테스트는 2마리의 크리처가 소환되어 있는지 확인하기 위함.
			// 2마리 소환 상태에서는 사용 불가능
			if( !pSummon->IsInWorld() || !pPlayer || pPlayer != pCaster || !pPlayer->IsInWorld() || ( pPlayer->GetSubSummon() && pPlayer->GetSubSummon()->IsInWorld() ) )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			// 이름 미지정(szParameter가 NULL이 되는 경우는 없지만...) 또는 길이 부족
			int code_page = ENV().GetInt( "CodePage", CP_ACP );
			if( !szParameter || strlen( szParameter ) < 4 )
			{
				nRet = RESULT_INVALID_TEXT;
				break;
			}
			// 같은 이름 사용 불가
			else if( !strncmp( szParameter, pSummon->GetName(), std::max( strlen( szParameter ), strlen( pSummon->GetName() ) ) ) )
			{
				nRet = RESULT_INVALID_TEXT;
				break;
			}
			else if( !GameRule::IsValidName( code_page, szParameter, (int)strlen( szParameter ) + 1, 4, 18 ) || // IsValidName에서 소환수 이름은 최대 길이 19 Byte이므로 버퍼 제한 18로 고정
				GameContent::IsBannedWord( code_page, szParameter ) )
			{
				nRet = RESULT_INVALID_TEXT;
				break;
			}

			LOG::Log11N4S( LM_SUMMON_CHANGE_NAME, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), pSummon->GetSID(), 0, 0, 0, 0, 0, 0, 0, pSummon->GetParentCard()->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, pSummon->GetName(), LOG::STR_NTS, szParameter, LOG::STR_NTS );

			pSummon->SetName( szParameter );

			// 각종 UI에 이름 변경을 알려주기 위함
			SendPropertyMessage( pPlayer, pSummon->GetHandle(), "name", pSummon->GetName() );

			// 주변 사람들에게 바뀐 이름 보이게 하기 위함
			TS_SC_CHANGE_NAME msg;
			msg.handle = pSummon->GetHandle();
			s_strcpy( msg.name, _countof( msg.name ), szParameter );

			ArcadiaServer::Instance().Broadcast( pSummon->GetRX(), pSummon->GetRY(), pSummon->GetLayer(), &msg );	

			// 소환수가 월드에 있을 때만 처리되므로 SetNeedCalculateStat으로만 처리하면 됨.
			// 없을 경우에는 CalculateStat 바로 호출해도 됨.
			pSummon->SetNeedCalculateStat();
		}
		break;

	case ITEM_EFFECT_INSTANT::RENAME_CHARACTER:
		{
			if( !pCaster->IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer * pCastPlayer = static_cast< StructPlayer * >( pCaster );

			if( !pCastPlayer->ChangeName( szParameter, var1, true, pItem->GetHandle() ) )
			{
				// 아이템을 사용하지 못했다는 내용에 대한 시스템 메시지를 출력하지 않도록 정의되지 않은 오류 코드를 반환
				nRet = RESULT_UNKNOWN;
				break;
			}
		}
		break;

	case ITEM_EFFECT_INSTANT::RENAME_PET:
		{
			if( !IsPet() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPet *pPet = static_cast< StructPet * >( this );
			StructPlayer *pPlayer = pPet->GetMaster();

			if( !pPet->IsInWorld() || !pPlayer || pPlayer != pCaster || !pPlayer->IsInWorld() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if( !pPet->GetParentCage() ||  pPet->GetParentCage()->IsExpireItem() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			// 이름 미지정(szParameter가 NULL이 되는 경우는 없지만...) 또는 길이 부족
			int code_page = ENV().GetInt( "CodePage", CP_ACP );
			if( !szParameter || strlen( szParameter ) < 4 )
			{
				nRet = RESULT_INVALID_TEXT;
				break;
			}
			// 같은 이름 사용 불가
			else if( !strncmp( szParameter, pPet->GetName(), std::max( strlen( szParameter ), strlen( pPet->GetName() ) ) ) )
			{
				nRet = RESULT_INVALID_TEXT;
				break;
			}
			else if( !GameRule::IsValidName( code_page, szParameter, (int)strlen( szParameter ) + 1, 4, 18 ) || // IsValidName에서 펫 이름은 최대 길이 19 Byte이므로 버퍼 제한 18로 고정
				GameContent::IsBannedWord( code_page, szParameter ) )
			{
				nRet = RESULT_INVALID_TEXT;
				break;
			}

			LOG::Log11N4S( LM_PET_CHANGE_NAME, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), pPet->GetSID(), 0, 0, 0, 0, 0, 0, 0, pPet->GetParentCage()->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, pPet->GetName(), LOG::STR_NTS, szParameter, LOG::STR_NTS );

			nRet = pPet->ChangeName( szParameter, true );
		}
		break;

	case ITEM_EFFECT_INSTANT::WARP_TO_SPECIAL_POSITION:
		{
			if( !pCaster->IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer *pCastPlayer = static_cast< StructPlayer * >( pCaster );

			unsigned char layer = 0;

			int current_channel = ChannelManager::GetChannelId( pCastPlayer->GetX(), pCastPlayer->GetY() );
			int target_channel = ChannelManager::GetChannelId( var1, var2 );

			if( current_channel && current_channel == target_channel )
			{
				layer = pCastPlayer->GetLayer();
			}
			else if( target_channel )
			{
				layer = ChannelManager::GetProperLayer( var1, var2 );
			}

			pCastPlayer->PendWarp( var1, var2, layer );
			ArcadiaServer::Instance().SetObjectPriority( pCastPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
		}
		break;

	case ITEM_EFFECT_INSTANT::WARP_TO_PLAYER:
		{
			if( !pCaster->IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer *pCastPlayer = static_cast< StructPlayer * >( pCaster );

			if( !szParameter )
			{
				nRet = RESULT_INVALID_ARGUMENT;
				break;
			}

			StructPlayer::iterator pit = StructPlayer::get( StructPlayer::FindPlayer( szParameter ) );
			StructPlayer *pTarget = *pit;

			if( !pTarget || !pTarget->IsInWorld() || !pTarget->IsLogin() || !pTarget->IsLoginComplete() || pTarget == this )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if( pTarget->IsInSecroute() )
			{
				nRet = RESULT_TARGET_IN_SECROUTE;
				break;
			}

			if( pTarget->IsInSiegeOrRaidDungeon() )
			{
				nRet = RESULT_TARGET_IN_SIEGE_OR_RAID;
				break;
			}

			// 값 2의 값이 0이면 숨겨진 던전 내의 대상을 향해 워프 불가, 1이면(0 외의 경우) 가능
			assert( var2 == 0 || var2 == 1 );
			if( var2 == 0 && pTarget->IsInSecretDungeon() )
			{
				nRet = RESULT_TARGET_IN_SECRET_DUNGEON;
				break;
			}

			if( pTarget->IsInInstanceDungeon() )
			{
				nRet = RESULT_TARGET_IN_INSTANCE_DUNGEON;
				break;
			}

			if( pTarget->IsInDungeon() )
			{
				int nDungeonID = DungeonManager::Instance().GetDungeonID( pTarget->GetX(), pTarget->GetY() );
				if( nDungeonID && DungeonManager::IsRestrictedToEnter( DungeonManager::Instance().GetDungeonLevel( nDungeonID ), pCaster->GetLevel() ) )
				{
					// 이 곳으로 워프하기엔 레벨이 딸립니다.
					PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pTarget, "@563\v#@player_name@#\v%s", pCaster->GetName() );
					PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", static_cast< StructPlayer* >( pCaster ), "@563\v#@player_name@#\v%s", pCaster->GetName() );
					nRet = RESULT_ACCESS_DENIED;
					break;
				}
			}
			
			if( pTarget->IsInDeathmatch() )
			{
				nRet = RESULT_TARGET_IN_DEATHMATCH;
				break;
			}

			if( pTarget->IsInEventmap() )
			{
				nRet = RESULT_TARGET_IN_EVENTMAP;
				break;
			}

			if( pTarget->IsInBattleArena() )
			{
				nRet = RESULT_TARGET_IN_BATTLE_ARENA;
				break;
			}

			if( HuntaholicManager::Instance().GetHuntaholicID( pTarget->GetPos() ) )
			{
				nRet = RESULT_TARGET_IN_HUNTAHOLIC;
				break;
			}

			// 파티원, 길드원, 길드 연합원만 가능
			int nPartyID = pTarget->GetPartyID();
			if( !nPartyID || nPartyID != pCastPlayer->GetPartyID() )
			{
				int nGuildId = pTarget->GetGuildID();
				int nCasterGuildID = pCastPlayer->GetGuildID();
				if( !nGuildId || nGuildId != nCasterGuildID )
				{
					int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildId );
					if( !nAllianceID || nAllianceID != GuildManager::GetInstance().GetAllianceID( nCasterGuildID ) )
					{
						nRet = RESULT_ACCESS_DENIED;
						break;
					}
				}
			}

			pCastPlayer->PendWarp( pTarget->GetX(), pTarget->GetY(), pTarget->GetLayer() );
			ArcadiaServer::Instance().SetObjectPriority( pCastPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
		}
		break;

	case ITEM_EFFECT_INSTANT::SUMMON_PET:
		{
			if( !pCaster->IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPet *pPet = pItem->GetPetStruct();
			if( !pItem->IsPetCage() || !pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_CONTAIN_PET ) )
			{
				nRet = RESULT_NOT_ACTABLE;
				break;
			}

			StructPlayer *pCastPlayer = static_cast< StructPlayer * >( pCaster );

			// 테이밍 되어있는데 펫 구조체가 없을 경우 새로 할당
			if( !pPet )
			{
				// 자동 테이밍 처리 가능 아이템 체크(GetSummonCode를 펫 우리의 경우 자동 테이밍 종류 펫으로 사용)
				if( !pItem->GetSummonCode() )
				{
					nRet = RESULT_NOT_ACTABLE;
					break;
				}

				pPet = AllocNewPet( pCastPlayer, pItem, pItem->GetSummonCode() );

				// 잘못된 펫 코드일 가능성 높음
				if( !pPet )
				{
					assert( 0 );
					nRet = RESULT_NOT_ACTABLE;
					break;
				}

				// 클라한테 추가 펫 정보 송신
				pCastPlayer->AddPet( pPet, false );

				// DB에 신규 펫 정보 추가, 펫 우리 아이템 업데이트
				pPet->DBQuery( new DB_InsertPet( pPet ) );
				pItem->DBQuery( new DB_UpdateItem( pItem ) );
			}

			if( !pCastPlayer->IsInWorld() || pCastPlayer->IsDead() || pCastPlayer->IsUsingStorage() ||
				pCastPlayer->IsBoothOpen() || pCastPlayer->IsBoothWatching() || pCastPlayer->IsTrading() ||
				pCastPlayer->IsUsingSkill() || pCastPlayer->IsRiding() || pCastPlayer->HasRidingState() )
			{
				nRet = RESULT_NOT_ACTABLE;
				break;
			}

			if( pPet->IsNameChanged() )
			{
				// 이미 소환된 펫이 있으면 역소환
				if( pCastPlayer->GetSummonedPet() )
				{
					// 사용된 아이템의 펫과 같다면 역소환만 함
					bool bUnsummonOnly = false;
					if( pCastPlayer->GetSummonedPet() == pItem->GetPetStruct() )
						bUnsummonOnly = true;

					// 역소환
					pCastPlayer->UnSummonPet();

					if( bUnsummonOnly )
						break;
				}

				// 이름 변경 불가면 바로 소환
				pCastPlayer->SummonPet( pItem->GetPetStruct() );
			}
			else
			{
				// 이름을 변경해야 하면 이름 변경 메시지 송신
				TS_SC_SHOW_SET_PET_NAME msg;

				msg.handle = pPet->GetHandle();

				PendMessage( pCastPlayer, &msg );
			}
		}
		break;

	case ITEM_EFFECT_INSTANT::ADD_CASH:	// 인포인트 아이템, 캐시 증가
		{
			if( pItem->GetItemCode() != ItemBase::ITEM_CODE_INGAME_POINT &&
				pItem->GetItemCode() != ItemBase::ITEM_CODE_INGAME_POINT2 &&
				pItem->GetItemCode() != ItemBase::ITEM_CODE_INGAME_POINT3 )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer *pPlayer = static_cast< StructPlayer * >( this );
			pPlayer->DBQuery( new DB_InsertCash( pPlayer, pPlayer->GetAccountName(), pPlayer->GetAccountID(), pItem->GetItemCode(), pItem->GetItemUID() ) );
		}
		break;

	case ITEM_EFFECT_INSTANT::HAIR_DYEING:
		{
			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer * pPlayer = static_cast< StructPlayer * >( this );

			// 아이템의 추가 성능에 헤어 스타일 변경 성능이 포함되어 있을 경우에는 아무 체크 하지 않음(헤어 스타일 변경 성능에서 필요한 체크 다 함)
			// 아이템의 추가 성능에 헤어 스타일 변경 성능이 포함되어 있지 않은 경우에는 같은 색상으로 변경하려는 거면 실패
			int nIdx = 0;
			const ItemBaseServer & itemBase = pItem->GetItemBase();
			for( nIdx = 0 ; nIdx < ItemBase::MAX_OPTION_NUMBER ; ++nIdx )
			{
				if( itemBase.nOptType[ nIdx ] != ITEM_EFFECT_INSTANT::SET_HAIR_STYLE )
					continue;

				break;
			}

			// 헤어 스타일 변경 성능이 포함되어 있지 않았을 경우
			if( nIdx >= ItemBase::MAX_OPTION_NUMBER )
			{
				// 현재와 동일한 색으로 변경하려고 하는 경우 실패
				if( pPlayer->GetHairColorIndex() == var1 && pPlayer->GetHairColorRGB() == var2 )
				{
					nRet = RESULT_ACCESS_DENIED;
					break;
				}
			}

			pPlayer->SetHairColorIndex( var1 );
			pPlayer->SetHairColorRGB( var2 );

			// 헤어 스타일 변경 패킷 방송 및 DB 저장
			BroadcastHairInfo( pPlayer );
			pPlayer->DBQuery( new DB_UpdateCharacterHair( pPlayer ) );
		}
		break;

	case ITEM_EFFECT_INSTANT::SET_HAIR_STYLE:
		{
			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer * pPlayer = static_cast< StructPlayer * >( this );

			// 아이템의 추가 성능에 염색 성능이 포함되어 있을 경우에는 같은 스타일/색상으로 변경하려는 거면 실패
			// 아이템의 추가 성능에 염색 성능이 포함되어 있지 않은 경우에는 같은 스타일로 변경하려는 거면 실패
			int nIdx = ItemBase::MAX_OPTION_NUMBER - 1;
			const ItemBaseServer & itemBase = pItem->GetItemBase();
			for( ; nIdx >= 0 ; --nIdx )
			{
				if( itemBase.nOptType[ nIdx ] != ITEM_EFFECT_INSTANT::HAIR_DYEING )
					continue;

				if( itemBase.fOptVar1[ nIdx ] == pPlayer->GetHairColorIndex() && itemBase.fOptVar2[ nIdx ] == pPlayer->GetHairColorRGB() &&
					var1 == pPlayer->GetBaseModelId( 0 ) )
				{
					nRet = RESULT_ACCESS_DENIED;
				}

				break;
			}

			// 윗 단계의 체크 중에 오류 코드가 세팅된 경우 처리 중지(오류 코드가 세팅되지 않았다면 nRet가 초기값인 RESULT_SUCCESS인 상태가 됨)
			if( nRet != RESULT_SUCCESS )
				break;

			// 염색 성능이 없는 헤어 스타일 변경 아이템일 경우 현재 스타일로 변경하려는 건지 체크
			if( nIdx < 0 && var1 == pPlayer->GetBaseModelId( 0 ) )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			pPlayer->SetBaseModelId( 0, var1 );

			// 염색 성능이 없는 헤어 스타일 변경 아이템일 경우에만 방송 및 DB 저장(염색 성능이 있는 아이템이면 염색 성능 처리 후에 저장 및 방송됨)
			if( nIdx < 0 )
			{
				BroadcastHairInfo( pPlayer );
				pPlayer->DBQuery( new DB_UpdateCharacterHair( pPlayer ) );
			}
		}
		break;

	case ITEM_EFFECT_INSTANT::SET_SKIN_COLOR:
		{
			if( !IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			StructPlayer * pPlayer = static_cast< StructPlayer * >( this );

			// 같은 색상으로 변경하려는 경우 실패
			if( pPlayer->GetSkinColor() == var1 )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			// 색깔 바꾸고 방송하고 디비 저장.
			pPlayer->SetSkinColor( var1 );
			BroadcastSkinInfo( pPlayer );
			pPlayer->DBQuery( new DB_UpdateCharacterSkin( pPlayer ) );	
		}
		break;

	case ITEM_EFFECT_INSTANT::CAST_WORLD_STATE:
		if( !pCaster->IsPlayer() )
		{
			nRet = RESULT_ACCESS_DENIED;
			break;
		}

		if ( !static_cast< StructPlayer* >( pCaster )->PendWorldState( (StructState::StateCode)pItem->GetStateCode(), pItem->GetStateLevel(), pItem->GetStateTime() * 100, var1 * 100, pItem->GetItemCode() ) )
		{
			nRet = RESULT_COOL_TIME;
		}

		break;

	case ITEM_EFFECT_INSTANT::CAST_PARTY_STATE:
		{
			if( !pCaster->IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if ( !static_cast< StructPlayer* >( pCaster )->PendPartyState( (StructState::StateCode)pItem->GetStateCode(), pItem->GetStateLevel(), pItem->GetStateTime() * 100, var1 * 100, pItem->GetItemCode() ) )
			{
				nRet = RESULT_COOL_TIME;
			}
			break;
		}

	case ITEM_EFFECT_INSTANT::CAST_GUILD_STATE:
		{
			if( !pCaster->IsPlayer() )
			{
				nRet = RESULT_ACCESS_DENIED;
				break;
			}

			if ( !static_cast< StructPlayer* >( pCaster )->PendGuildState( (StructState::StateCode)pItem->GetStateCode(), pItem->GetStateLevel(), pItem->GetStateTime() * 100, var1 * 100, pItem->GetItemCode() ) )
			{
				nRet = RESULT_COOL_TIME;
			}
			break;
		}

	default:	break;
	}

	return nRet;
}

bool StructCreature::TurnOnAura( struct StructSkill * pSkill )
{
	int nToggleGroup = pSkill->GetSkillBase()->GetToggleGroup();

	if( nToggleGroup > SkillBase::MAX_TOGGLE_GROUP || nToggleGroup < 0 )
	{
		assert( 0 );
		return false;
	}

	TS_SC_AURA msg;

	bool bExistSameGroup = false;

	std::vector< std::pair<StructSkill *, int> >::iterator it;

	for( it = m_vAura.begin(); it != m_vAura.end(); ++it )
	{
		if( (*it).first->GetSkillBase()->GetToggleGroup() != 0 && (*it).first->GetSkillBase()->GetToggleGroup() == nToggleGroup )
		{
			msg.caster = GetHandle();
			msg.skill_id = (*it).first->GetSkillId();

			msg.status = false;

			if( IsPlayer() )
			{
				PendMessage( this, &msg );
			}
			else if( IsSummon() )
			{
				PendMessage( static_cast< StructSummon * >( this )->GetMaster(), &msg );
			}
			(*it) = std::pair<StructSkill *,int>( pSkill, pSkill->GetRequestedSkillLevel() );
			bExistSameGroup = true;
			break;
		}
	}

	if( !bExistSameGroup )
		m_vAura.push_back( std::pair<StructSkill *,int>( pSkill, pSkill->GetRequestedSkillLevel() ) );

	msg.caster = GetHandle();
	msg.skill_id = pSkill->GetSkillId();

	msg.status = true;

	if( IsPlayer() )
	{
		PendMessage( this, &msg );
	}
	else if( IsSummon() )
	{
		PendMessage( static_cast< StructSummon * >( this )->GetMaster(), &msg );
	}
	return true;
}

bool StructCreature::IsActiveAura( struct StructSkill * pSkill )
{
	std::vector< std::pair<StructSkill *, int> >::iterator it;

	for( it = m_vAura.begin(); it != m_vAura.end(); ++it )
	{
		if( (*it).first == pSkill )
		{
			return true;
		}
	}

	return false;
}

bool StructCreature::TurnOffAura( struct StructSkill * pSkill )
{
	std::vector< std::pair<StructSkill *, int> >::iterator it;

	for( it = m_vAura.begin(); it != m_vAura.end(); ++it )
	{
		if( (*it).first == pSkill )
		{
			TS_SC_AURA msg;

			msg.caster = GetHandle();
			msg.skill_id = (*it).first->GetSkillId();

			msg.status = false;

			if( IsPlayer() )
			{
				PendMessage( this, &msg );
			}
			else if( IsSummon() )
			{
				PendMessage( static_cast< StructSummon * >( this )->GetMaster(), &msg );
			}
			vector_fast_erase( &m_vAura, it );
			return true;
		}
	}

	return false;
}

void StructCreature::ToggleAura( struct StructSkill * pSkill )
{
	if( !TurnOffAura( pSkill ) )
		TurnOnAura( pSkill );
}


void StructCreature::onDead( StructCreature *pFrom, bool decreaseEXPOnDead )
{
	// 캐스팅 취소
	if( IsUsingSkill() )
	{
		CancelSkill();
	}

	// 죽었으니 이동 정지
	if( IsMoving() ) 
	{
		ArPosition pos = GetCurrentPosition( m_nDeadTime );
		ArcadiaServer::Instance().SetMove( this, pos, pos, 0, true, GetArTime() );

		if( IsPlayer() && static_cast< StructPlayer * >( this )->GetRideObject() )
		{
			ArcadiaServer::Instance().SetMove( static_cast< StructPlayer * >( this )->GetRideObject(), pos, pos, 0, true, GetArTime() );
		}
	}

	if( IsAttacking() )
	{
		EndAttack();
	}

	// 오라 제거~
	RemoveAllAura();

	RemoveAllHate();

	removeStateByDead();

	// 경험치 감소 여부에 관계없이 마지막에 감소되었던 경험치를 초기화하여 의도하지 않은 동작을 막는다.
	// 만약 경험치 감소가 일어난다면 이후에 새로운 값이 설정될 것이며 경험치 감소가 일어나지 않는다하더라도 최종적으로는 감소된 경험치가 0으로 설정된다.
	SetLastDecreasedEXP( 0 );
}

void StructCreature::RemoveAllHate()
{
	// { Hate 초기화
	std::vector< AR_HANDLE >::iterator it;

	std::vector< AR_HANDLE > vEnemyList = m_vEnemyList;

	for( it = vEnemyList.begin(); it != vEnemyList.end(); ++it )
	{
		StructCreature::iterator itMonster = StructCreature::get( *it );

		if( (*itMonster) && (*itMonster)->IsMonster() )
		{
			StructMonster * pMonster = static_cast< StructMonster * >( *itMonster );

			pMonster->RemoveHate( GetHandle(), pMonster->GetHate( GetHandle() ) );
		}
	}

	{
		THREAD_SYNCHRONIZE( m_csEnemy );

		m_vEnemyList.clear();
	}
	// }
}

bool StructCreature::IsEnemy( StructCreature* pTarget, bool bIncludeHiding )
{
	if( !pTarget )
		return false;

	if( !pTarget->IsInWorld() )
		return false;

	// 나 자신이 무적이라고 해서 적을 적이라 부르지 못하는 건 아님 2012-05-17 Synastry
	//if( IsInvincible() || !isMortal() )
	//	return false;

	if( pTarget->IsInvincible() || !pTarget->isMortal() )
		return false;

	if( !bIncludeHiding && !IsVisible( pTarget ) )
		return false;

	if( IsAlly( pTarget ) )
		return false;

	if( pTarget->IsPet() )
		return false;

	return true;
}

bool StructCreature::IsAlly( StructCreature* pTarget )
{
	return false;
}

const c_fixed10 StructCreature::GetAttackPointRightWithoutWeapon( Elemental::Type type, bool bPhysical, bool bBad ) const
{
	if( bPhysical )
	{
		if( bBad )
			return GetAttackPointRightWithoutWeapon() * m_BadPhysicalElementalSkillStateMod[type].fPhysicalDamage;

		return GetAttackPointRightWithoutWeapon() * m_GoodPhysicalElementalSkillStateMod[type].fPhysicalDamage;
	}

	if( bBad )
		return GetAttackPointRightWithoutWeapon() * m_BadMagicalElementalSkillStateMod[type].fPhysicalDamage;

	return GetAttackPointRightWithoutWeapon() * m_GoodMagicalElementalSkillStateMod[type].fPhysicalDamage;
}

int	StructCreature::GetAttackPointRight( Elemental::Type type, bool bPhysical, bool bBad )
{
	if( bPhysical )
	{
		if( bBad )
			return GetAttackPointRight() * m_BadPhysicalElementalSkillStateMod[type].fPhysicalDamage;

		return GetAttackPointRight() * m_GoodPhysicalElementalSkillStateMod[type].fPhysicalDamage;
	}

	if( bBad )
		return GetAttackPointRight() * m_BadMagicalElementalSkillStateMod[type].fPhysicalDamage;

	return GetAttackPointRight() * m_GoodMagicalElementalSkillStateMod[type].fPhysicalDamage;
}

int	StructCreature::GetMagicPoint( Elemental::Type type, bool bPhysical, bool bBad )
{
	if( bPhysical )
	{
		if( bBad )
			return m_Attribute.fMagicPoint * m_BadPhysicalElementalSkillStateMod[type].fMagicalDamage;

		return m_Attribute.fMagicPoint * m_GoodPhysicalElementalSkillStateMod[type].fMagicalDamage;
	}

	if( bBad )
		return m_Attribute.fMagicPoint * m_BadMagicalElementalSkillStateMod[type].fMagicalDamage;

	return m_Attribute.fMagicPoint * m_GoodMagicalElementalSkillStateMod[type].fMagicalDamage;
}

float StructCreature::GetMagicalHateMod( Elemental::Type type, bool bPhysical, bool bBad )
{
	if( bPhysical )
	{
		if( bBad )
			return m_BadPhysicalElementalSkillStateMod[type].fHate;

		return m_GoodPhysicalElementalSkillStateMod[type].fHate;
	}

	if( bBad )
		return m_BadMagicalElementalSkillStateMod[type].fHate;

	return m_GoodMagicalElementalSkillStateMod[type].fHate;
}

std::pair< float, int> StructCreature::GetHateMod( int nHateModType, bool bIsHarmful )
{
	float	fAmpValue = 1.0f;
	int		nIncValue = 0;

	for( std::vector< HateModifier >::iterator it = m_vHateMod.begin(); it != m_vHateMod.end(); ++it )
	{
		bool bApply = false;

		if( ( bIsHarmful && (*it).bIsApplyToHarmful ) || ( !bIsHarmful && (*it).bIsApplyToHelpful ) )
		{
			if( ( nHateModType == 1 && (*it).bIsApplyToPhysicalSkill )
				|| ( nHateModType == 2 && (*it).bIsApplyToMagicalSkill )
				|| ( nHateModType == 3 && (*it).bIsApplyToPhysicalAttack ) )
			{
				bApply = true;
			}
		}

		if( bApply )
		{
			fAmpValue += (*it).fAmpValue;
			nIncValue += (*it).nIncValue;
		}
	}

	return std::pair< float, int >( fAmpValue, nIncValue );
}

unsigned char StructCreature::GetRealMoveSpeed() const
{
	int RealMoveSpeed = GetMoveSpeed()/7;
	if( RealMoveSpeed > UCHAR_MAX )
		RealMoveSpeed = UCHAR_MAX;

	return RealMoveSpeed;
}

unsigned char StructCreature::GetRealRidingSpeed() const
{
	int RealRidingSpeed = GetRidingSpeed()/7;
	if( RealRidingSpeed > UCHAR_MAX )
		RealRidingSpeed = UCHAR_MAX;

	return RealRidingSpeed;
}

float StructCreature::GetManaCostRatio( Elemental::Type type, bool bPhysical, bool bBad )
{
	if( bPhysical )
	{
		if( bBad )
			return m_BadPhysicalElementalSkillStateMod[type].fManaCostRatio;

		return m_GoodPhysicalElementalSkillStateMod[type].fManaCostRatio;
	}

	if( bBad )
		return m_BadMagicalElementalSkillStateMod[type].fManaCostRatio;

	return m_GoodMagicalElementalSkillStateMod[type].fManaCostRatio;
}

float StructCreature::GetCastingMod( Elemental::Type type, bool bPhysical, bool bBad, AR_TIME nOriginalCoolTime )
{
	if( bPhysical )
	{
		if( bBad )
		{
			if( m_BadPhysicalElementalSkillStateMod[type].nCastingSpeedApplyTime >= nOriginalCoolTime )
			{
				return m_BadPhysicalElementalSkillStateMod[type].fCastingSpeed;
			}
			else
			{
				return 1.0f;
			}
		}

		if( m_GoodPhysicalElementalSkillStateMod[type].nCastingSpeedApplyTime >= nOriginalCoolTime )
		{
			return m_GoodPhysicalElementalSkillStateMod[type].fCastingSpeed;
		}
		else
		{
			return 1.0f;
		}
	}

	if( bBad )
	{
		if( m_BadMagicalElementalSkillStateMod[type].nCastingSpeedApplyTime >= nOriginalCoolTime )
		{
			return m_BadMagicalElementalSkillStateMod[type].fCastingSpeed;
		}
		else
		{
			return 1.0f;
		}
	}

	if( m_GoodMagicalElementalSkillStateMod[type].nCastingSpeedApplyTime >= nOriginalCoolTime )
	{
		return m_GoodMagicalElementalSkillStateMod[type].fCastingSpeed;
	}

	return 1.0f;
}

float StructCreature::GetCoolTimeMod( Elemental::Type type, bool bPhysical, bool bBad )
{
	if( bPhysical )
	{
		if( bBad )
			return m_BadPhysicalElementalSkillStateMod[type].fCooltime;

		return m_GoodPhysicalElementalSkillStateMod[type].fCooltime;
	}

	if( bBad )
		return m_BadMagicalElementalSkillStateMod[type].fCooltime;

	return m_GoodMagicalElementalSkillStateMod[type].fCooltime;
}

bool StructCreature::SetPendingMove( const std::vector< ArPosition > & newPos, unsigned char speed )
{
	if( HasPendingMove() )
	{
		return false;
	}

	m_PendingMovePos = newPos;
	m_nPendingMoveSpeed = speed;

	m_StatusFlag.On( STATUS_MOVE_PENDED );

	return true;
}

void StructCreature::AddHateToEnemyList( AR_HANDLE handle, int pt )
{
	std::vector< AR_HANDLE >::iterator it;
	std::vector< AR_HANDLE > vInvalidEnemy;
	std::vector< AR_HANDLE > vEnemy;

	{
		THREAD_SYNCHRONIZE( m_csEnemy );

		for( it = m_vEnemyList.begin(); it != m_vEnemyList.end(); ++it )
		{
			StructCreature::iterator itEnemy = StructCreature::get( (*it) );

			if( !(*itEnemy) || !(*itEnemy)->IsMonster() )
			{
				vInvalidEnemy.push_back( (*it) );
				continue;
			}

			if( (*itEnemy)->GetPos().GetDistance( GetPos() ) > GameRule::VISIBLE_RANGE )
				continue;
			else if( static_cast< StructMonster * >(*itEnemy)->GetStatus() == StructMonster::STATUS_NORMAL )
				continue;

			vEnemy.push_back( *it );
		}
	}

	for( it = vEnemy.begin(); it != vEnemy.end(); ++it )
	{
		StructCreature::iterator itEnemy = StructCreature::get( (*it) );

		if( (*itEnemy) && (*itEnemy)->IsMonster() )
			static_cast< StructMonster *>( (*itEnemy) )->AddHate( handle, m_fHateRatio * pt );
	}

	for( it = vInvalidEnemy.begin(); it != vInvalidEnemy.end(); ++it )
	{
		RemoveFromEnemyList( (*it) );
	}
}

bool StructCreature::AddToEnemyList( AR_HANDLE handle )
{
	std::vector< AR_HANDLE >::iterator it;

	THREAD_SYNCHRONIZE( m_csEnemy );

	for( it = m_vEnemyList.begin(); it != m_vEnemyList.end(); ++it )
	{
		if( (*it) == handle )
			return false;
	}

	m_vEnemyList.push_back( handle );

	return true;
}

bool StructCreature::RemoveFromEnemyList( AR_HANDLE handle )
{
	std::vector< AR_HANDLE >::iterator it;

	THREAD_SYNCHRONIZE( m_csEnemy );

	for( it = m_vEnemyList.begin(); it != m_vEnemyList.end(); ++it )
	{
		if( (*it) == handle )
		{
			vector_fast_erase( &m_vEnemyList, it );
			return true;
		}
	}

	return false;
}

void StructCreature::AddEnergy( int nEnergy )
{
	if( m_nEnergy >= m_nMaxEnergy )
		return;

	AR_TIME nKeepTime = GetArTime() + m_nEnergyUpkeepTime;

	for( int i = 0; i < nEnergy; ++i )
	{
		int pos = m_nEnergy + m_nEnergyStartPos + i;
	if( pos >= GameRule::ENERGY_MAX )
	{
		pos -= GameRule::ENERGY_MAX;
	}

		m_arEnergy[pos] = nKeepTime;

	++m_nEnergy;

		if( m_nEnergy >= m_nMaxEnergy )
			break;
	}

	onEnergyChange();
}

void StructCreature::RemoveEnergy( int nEnergy )
{
	// 일정 확률로 기공을 소모하지 않는다.
	if( XRandom( 0, 99 ) < GetEnergyUnconsumptionRate() )
		return;

	int nRemoveCnt = std::min( nEnergy, m_nEnergy );

	if( nRemoveCnt < 1 )
		return;

	m_nEnergy -= nRemoveCnt;

	m_nEnergyStartPos += nRemoveCnt;

	if( m_nEnergyStartPos >= GameRule::ENERGY_MAX )
	{
		m_nEnergyStartPos -= GameRule::ENERGY_MAX;
	}

	onEnergyChange();
}

ItemBase::ItemClass StructCreature::GetArmorClass()
{
	StructItem * pArmor = GetWearedItem( ItemBase::WEAR_ARMOR );

	if( !pArmor )
		return ItemBase::CLASS_ETC;

	return pArmor->GetItemClass();
}

ItemBase::ItemClass StructCreature::GetWeaponClass()
{
	StructItem * pWeapon = GetWearedItem( ItemBase::WEAR_WEAPON );

	if( !pWeapon )
		return ItemBase::CLASS_ETC;

	if( IsUsingDoubleWeapon() )
	{
		StructItem * pLeftWeapon = GetWearedItem( ItemBase::WEAR_LEFTHAND );

		if( pWeapon->GetItemClass() == ItemBase::CLASS_ONEHAND_SWORD && pLeftWeapon->GetItemClass() == ItemBase::CLASS_ONEHAND_SWORD )
		{
			return ItemBase::CLASS_DOUBLE_SWORD;
		}
		else if( pWeapon->GetItemClass() == ItemBase::CLASS_DAGGER && pLeftWeapon->GetItemClass() == ItemBase::CLASS_DAGGER )
		{
			return ItemBase::CLASS_DOUBLE_DAGGER;
		}
		else if( pWeapon->GetItemClass() == ItemBase::CLASS_ONEHAND_AXE && pLeftWeapon->GetItemClass() == ItemBase::CLASS_ONEHAND_AXE )
		{
			return ItemBase::CLASS_DOUBLE_AXE;
		}
	}

	return pWeapon->GetItemClass();
}

unsigned short StructCreature::putoffItem( StructCreature * pCreature, ItemBase::ItemWearType pos )
{
	return pCreature->putoffItem( pos );
}

int StructCreature::onDamage( StructCreature * pFrom, Elemental::Type elementalType, DamageType damageType, int nDamage, bool bCritical, bool bDouble)
{
	float fReflect = 0.0f;

	if ((damageType == DT_NORMAL_PHYSICAL_DAMAGE) || (damageType == DT_NORMAL_PHYSICAL_LEFT_HAND_DAMAGE)) 
	{
		fReflect = m_fPassivePhysicalReflect;
	}
	else if ((damageType == DT_NORMAL_MAGICAL_DAMAGE) || (damageType == DT_NORMAL_PHYSICAL_SKILL_DAMAGE)) 
	{
		fReflect = m_fPassiveMagicalReflect;
	}

	if (fReflect <= 0.0f) {
		if (m_Attribute.fDamageReduce >= 1)
		{
			fReflect = m_Attribute.fDamageReduce + (fReflect * 0.5);
			if (fReflect > 0.5)
			{
				fReflect = 0.5;
			}
			nDamage = (1.0 - fReflect) * nDamage;
		}
	}

	if (bDouble != false) 
	{
		nDamage *= 2;
	}


	if (m_Attribute.fDamageReducePerHP > 0)
	{
		float hpRatio = m_nHP / m_nMaxHP;
		if (hpRatio < 0.5f)
		{
			hpRatio = (1.0f - hpRatio * 2.0f) * m_Attribute.fDamageReducePerHP;
			if (hpRatio > 0)
			{
				if (hpRatio > 0.5f)
				{
					hpRatio = 0.6f;
				}
				nDamage = (1.0f - hpRatio) * nDamage;
			}
		}
	}

	// 일반 공격의 하보크 처리는 Attack 함수에서 처리(블럭, 퍼펙트 블럭, 미스 판정 여부를 알아야 함)
	// 불사신 효과는 데미지는 받지만 죽지 않는 상태이므로 패스데미지/데미지 반사 등의 처리가 모두 된 이후에 실제 데미지가 들어가지 않도록 한다.
	int nMinHP = m_fMinHP * GetMaxHP();
	if( nMinHP < m_nMinHP )
		nMinHP = m_nMinHP;
	
	if( m_nHP <= nMinHP )
		nDamage = 0;
	else if( m_nHP - nDamage < nMinHP )
		nDamage = m_nHP - nMinHP;

	return nDamage;
}

void StructCreature::EnumActiveSkill( SKILL_POINTER_FUNCTOR & _fo )
{
	std::vector< StructSkill * >::const_iterator it;

	for( it = m_vActiveSkillList.begin(); it != m_vActiveSkillList.end(); ++it )
	{
		_fo.onSkill( (*it) );
	}
}

void StructCreature::EnumPassiveSkill( SKILL_POINTER_FUNCTOR & _fo )
{
	std::vector< StructSkill * >::const_iterator it;

	for( it = m_vPassiveSkillList.begin(); it != m_vPassiveSkillList.end(); ++it )
	{
		_fo.onSkill( (*it) );
	}
}

void StructCreature::EnumAddedSkill( ADDED_SKILL_FUNCTOR & _fo ) const
{
	std::vector< std::pair< int, int > >::const_iterator it;

	for( it = m_vAddedSkillBySkillId.begin(); it != m_vAddedSkillBySkillId.end(); it++ )
	{
		_fo.onSkill( (*it).first, false, (*it).second );
	}

	for( it = m_vAddedSkillBySkillType.begin(); it != m_vAddedSkillBySkillType.end(); it++ )
	{
		_fo.onSkill( (*it).first, true, (*it).second );
	}
}

void StructCreature::removeStateByDead()
{
	ClearRemovedStateByDead();

	std::vector< StructState > removedStates;
	RemoveStateIf( StateFlagChecker( StateInfo::ERASE_ON_DEAD ), &removedStates, true );

	// m_vStateListRemovedByDeath 변수는 StructCreature에 선언되어있지만 StructPlayer 밖에 사용할 수가 없다.
	if( IsPlayer() )
	{
		for ( std::vector< StructState >::iterator it = removedStates.begin(); it != removedStates.end(); ++it )
		{
			// 크루 아이템을 통한 부활시 없어진 지속효과 복구를 위해 임시 보관
			// 해당 리스트에서 StructState를 다시 꺼내서 걸 경우 시작 시간, 끝 시간을 조정해야 함
			if( !it->IsHarmful() )
			{
				(*it).SaveByDead( this );

				m_vStateListRemovedByDeath.push_back( *it );
			}
			else
			{
				(*it).Expire( this );
			}
		}
	}
}

void StructCreature::removeStateWithFlag( const int & nFlag )
{
	RemoveStateIf( StateFlagChecker( nFlag ) );
}

void StructCreature::processPendingMove()
{
	// { pending move
	do
	{
		if( HasPendingMove() && GetMovableTime() < GetArTime() )
		{
			m_StatusFlag.Off( STATUS_MOVE_PENDED );

			if( IsActable() && IsMovable() )
			{
				AR_TIME tm = GetArTime();
				ArPosition pos = GetCurrentPosition( tm );

				ArcadiaServer::Instance().SetMultipleMove( this, pos, m_PendingMovePos, m_nPendingMoveSpeed, true, tm );

				if( IsPlayer() && static_cast< StructPlayer * >( this )->IsRiding() )
				{
					ArcadiaServer::Instance().SetMultipleMove( static_cast< StructPlayer * >( this )->GetRideObject(), pos, m_PendingMovePos, m_nPendingMoveSpeed, true, tm );
				}

				if( IsSummon() )
				{
					ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );
				}
			}
		}
	} while( false );
	// } pending move
}

void StructCreature::procMoveSpeedChangement()
{
	if( !IsMoving() )
		return;

	ArPosition pos = GetCurrentPosition( GetArTime() );

	// 이동 불가가 되었다면 현재 위치에 정지
	if( !IsMovable() )
	{
		ArcadiaServer::Instance().SetMove( this, pos, pos, 0, true, GetArTime() );
	}
	// 이동 중이었고 이동 가능 상태라면 현재 이동 속도와 스텟상의 이동 속도가 다를 경우 다시 이동 시킴
	else if( GetSpeed() != GetRealMoveSpeed() )
	{
		const std::vector< ArMoveVector::MOVE_INFO > & vMoveVector( GetTargetPosList() );
		std::vector< ArPosition > vMovePos;

		for( std::vector< ArMoveVector::MOVE_INFO >::const_iterator it = vMoveVector.begin() ; it != vMoveVector.end() ; ++it )
			vMovePos.push_back( (*it).end );

		ArcadiaServer::Instance().SetMultipleMove( this, pos, vMovePos, GetRealMoveSpeed(), true, GetArTime() );
	}
}

void StructCreature::PrepareRemoveExhaustiveSkillStateMod( bool bPhysical, bool bHarmful, int nElementalType, AR_TIME nOriginalCastingDelay )
{
	if( bPhysical )
	{
		if( bHarmful )
		{
			m_BadPhysicalElementalSkillStateMod[ nElementalType ].vExhaustiveStateCodeForDelete = m_BadPhysicalElementalSkillStateMod[ nElementalType ].vExhaustiveStateCode;
		}
		else
		{
			m_GoodPhysicalElementalSkillStateMod[ nElementalType ].vExhaustiveStateCodeForDelete = m_GoodPhysicalElementalSkillStateMod[ nElementalType ].vExhaustiveStateCode;
		}
	}
	else
	{
		if( bHarmful )
		{
			m_BadMagicalElementalSkillStateMod[ nElementalType ].vExhaustiveStateCodeForDelete = m_BadMagicalElementalSkillStateMod[ nElementalType ].vExhaustiveStateCode;
		}
		else
		{
			m_GoodMagicalElementalSkillStateMod[ nElementalType ].vExhaustiveStateCodeForDelete = m_GoodMagicalElementalSkillStateMod[ nElementalType ].vExhaustiveStateCode;
		}
	}
}

void StructCreature::RemoveExhaustiveSkillStateMod( bool bPhysical, bool bHarmful, int nElementalType, AR_TIME nOriginalCastingDelay )
{
	if( bPhysical )
	{
		if( bHarmful )
		{
			removeExhaustiveSkillStateMod( &m_BadPhysicalElementalSkillStateMod[ nElementalType ], nOriginalCastingDelay );
		}
		else
		{
			removeExhaustiveSkillStateMod( &m_GoodPhysicalElementalSkillStateMod[ nElementalType ], nOriginalCastingDelay );
		}
	}
	else
	{
		if( bHarmful )
		{
			removeExhaustiveSkillStateMod( &m_BadMagicalElementalSkillStateMod[ nElementalType ], nOriginalCastingDelay );
		}
		else
		{
			removeExhaustiveSkillStateMod( &m_GoodMagicalElementalSkillStateMod[ nElementalType ], nOriginalCastingDelay );
		}
	}

	CalculateStat();
}

void StructCreature::removeExhaustiveSkillStateMod( ElementalSkillStateMod *pSkillStateMod, AR_TIME nOriginalCastingDelay )
{
	std::vector< StructState::StateCode > vEraseList = pSkillStateMod->vExhaustiveStateCodeForDelete;

	std::vector< StructState::StateCode >::iterator it;

	for( it = vEraseList.begin() ; it != vEraseList.end() ; ++it )
	{
		StructState *pState = GetState( *it );
		if( !pState )
		{
			continue;
		}

		if( pState->GetEffectType() == StructState::EF_ADD_PARAMETER_ON_SKILL && pState->GetValue( 11 ) && pState->GetValue( 11 ) * 100 < nOriginalCastingDelay )
		{
			continue;
		}

		RemoveState( *it, pState->GetLevel() );
	}
}

void StructCreature::RemoveGoodState( int state_level )
{
	std::vector< StructState::StateCode > vEraseStateList;

	STATE_ITERATOR it;
	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		if( (*it).IsHarmful() ) continue;
		if( !( (*it).GetTimeType() & StateInfo::ERASE_ON_DEAD ) ) continue;

		vEraseStateList.push_back( (*it).GetCode() );
	}

	for( std::vector< StructState::StateCode >::iterator itState = vEraseStateList.begin(); itState != vEraseStateList.end(); ++itState )
	{
		RemoveState( (*itState), state_level );
	}
}

int StructCreature::damage( StructCreature * pFrom, int nDamage, bool decreaseEXPOnDead )
{
	if( IsDead() ) return 0;

	// 은신 해제
	if( IsHiding() )
	{
		RemoveState( StructState::HIDE, GameRule::MAX_STATE_LEVEL );
		RemoveState( StructState::TRACE_OF_FUGITIVE, GameRule::MAX_STATE_LEVEL );
	}

	m_nHP = ( m_nHP > nDamage ? m_nHP - nDamage : 0 );
	if( IsDead() ) 
	{	
		// 죽었다면 죽은 시간 세팅
		SetDeadTime( GetArTime() );
		onDead( pFrom, decreaseEXPOnDead );
	}
	return nDamage;
}

void StructCreature::ProcessAddHPMPOnCritical()
{
	if( m_vAddHPMPOnCritical.empty() )
		return;

	for( std::vector< AddHPMPOnCriticalInfo >::iterator it = m_vAddHPMPOnCritical.begin() ; it != m_vAddHPMPOnCritical.end() ; ++it )
	{
		if( XRandom( 0, 99 ) < (*it).nActivationRate )
		{
			AddHP( (*it).nAddHP );
			AddMP( (*it).nAddMP );
		}
	}
}

bool StructCreature::IsVisible( StructCreature *pTarget )
{
	AR_TIME t = GetArTime();
	ArPosition pos = GetCurrentPosition( t );

	if( !pTarget->IsInvisible() && ( !pTarget->IsHiding() || pTarget->GetCurrentPosition( t ).GetDistance( pos ) < m_fDetectHideRange ) )
		return true;

	return false;
}

void StructCreature::onProcess( int nThreadIdx )
{
	AR_TIME t = GetArTime();

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		for( std::vector< StructState >::iterator it = m_vPendStateList.begin(); it != m_vPendStateList.end(); ++it )
		{
			AddState( (*it).GetCode(), (*it).GetCaster(), (*it).GetLevel(), t, t + (*it).GetEndTime() - (*it).GetStartTime(), (*it).IsAura(), (*it).GetStateValue(), (*it).GetStateStringValue(), (*it).IsByEvent() );
		}

		m_vPendStateList.clear();

		// 지속효과 적용 (여기서 사망하실수도 있는지라 가장 늦게 적용해야함 -.-)
		if( m_vStateList.size() && m_nLastStateProcTime + 100 < t )
		{
			procStateDamage( t );
			procState( t );
			m_nLastStateProcTime = t;
		}
	}
}

void StructCreature::onHPChange( int nPrevHP )
{
	if( nPrevHP == 0 && GetHP() > 0 )
	{
		removeStateWithFlag( StateInfo::ERASE_ON_RESURRECT );
	}
}

// 범용적인 부활 처리, 추후에 기존의 부활들도 이 함수를 통하도록 수정
bool StructCreature::Resurrect( const _CHARACTER_RESURRECTION_TYPE eResurrectType, const int nIncHP, const int nIncMP, const __int64 nRecoveryEXP, const bool bIsRestoreRemovedStateByDead )
{
	if( !IsDead() || ( !IsSummon() && !IsPlayer() ) )
	{
		return false;
	}

	// 최소 1 회복
	AddHP( std::max( nIncHP, 1 ) );
	AddMP( nIncMP );
	SetEXP( GetEXP() + nRecoveryEXP );

	if( bIsRestoreRemovedStateByDead )	RestoreRemovedStateByDead();
	else								ClearRemovedStateByDead();

	if( IsPlayer() )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer * >( this );
		pPlayer->Save( true );

		if( pPlayer->IsCompeteDead() )
			pPlayer->SetCompeteDead( false );

		LOG::Log11N4S( LM_CHARACTER_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(), eResurrectType, 0, 0, 0, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), nRecoveryEXP, pPlayer->GetEXP(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "" , LOG::STR_NTS );
	}
	else if( IsSummon() )
	{
		StructSummon * pSummon = static_cast< StructSummon * >( this );
		StructItem *pCard = pSummon->GetParentCard();
		if( pCard && pCard->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON_DURABILITY ) )
		{
			pCard->SetCurrentEtherealDurability( pCard->GetCurrentEtherealDurability() + 1 );
			if( pSummon->GetMaster() )
				SendItemMessage( pSummon->GetMaster(), pCard );

			pCard->SetInstanceFlagOff( ItemInstance::ITEM_FLAG_SUMMON_DURABILITY );
		}

		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );

		StructPlayer *pPlayer = static_cast< StructPlayer * >(pSummon->GetMaster());
		LOG::Log11N4S( LM_SUMMON_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(), pSummon->GetSID(), 0, 0, 0, 0, 0, (nRecoveryEXP > 0), nRecoveryEXP, pSummon->GetEXP(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
	}

	BroadcastHPMPMsg( this, nIncHP, nIncMP );

	return true;
}

bool StructCreature::ResurrectByState()
{
	if( !IsDead() || ( !IsSummon() && !IsPlayer() ) )
	{
		return false;
	}

	StructState * pResurrectState = NULL;

	for( std::vector< StructState >::iterator it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		if( (*it).GetEffectType() == StructState::EF_RESURRECTION )
		{
			if( !pResurrectState || pResurrectState->GetLevel() < (*it).GetLevel() )
			{
				pResurrectState = &(*it);
			}
		}
	}

	if( !pResurrectState )
	{
		return false;
	}

	// resurrect~

	int nIncHP = ( pResurrectState->GetValue( 0 ) + pResurrectState->GetValue( 1 ) * pResurrectState->GetLevel() ) * GetMaxHP();
	int nIncMP = ( pResurrectState->GetValue( 2 ) + pResurrectState->GetValue( 3 ) * pResurrectState->GetLevel() ) * GetMaxMP();
	__int64 nRecoveryEXP = GetLastDecreasedEXP() * ( pResurrectState->GetValue( 4 ) + pResurrectState->GetValue( 5 ) * pResurrectState->GetLevel() );

	SetEXP( GetEXP() + nRecoveryEXP );

	AddHP( nIncHP );
	AddMP( nIncMP );

	ClearRemovedStateByDead();

	if( IsPlayer() )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer * >( this );
		pPlayer->Save( true );

		if( pPlayer->IsCompeteDead() )
			pPlayer->SetCompeteDead( false );

		LOG::Log11N4S( LM_CHARACTER_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(), CRT_STATE, 0, 0, 0, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), nRecoveryEXP, pPlayer->GetEXP(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "" , LOG::STR_NTS );

	}
	else if( IsSummon() )
	{
		StructSummon * pSummon = static_cast< StructSummon * >( this );
		StructItem *pCard = pSummon->GetParentCard();
		if( pCard && pCard->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON_DURABILITY ) )
		{
			pCard->SetCurrentEtherealDurability( pCard->GetCurrentEtherealDurability() + 1 );
			if( pSummon->GetMaster() )
				SendItemMessage( pSummon->GetMaster(), pCard );

			pCard->SetInstanceFlagOff( ItemInstance::ITEM_FLAG_SUMMON_DURABILITY );
		}

		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );

		StructPlayer *pPlayer = static_cast< StructPlayer * >(pSummon->GetMaster());
		LOG::Log11N4S( LM_SUMMON_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(), pSummon->GetSID(), 0, 0, 0, 0, 0, (nRecoveryEXP > 0), nRecoveryEXP, pSummon->GetEXP(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
	}

	BroadcastHPMPMsg( this, nIncHP, nIncMP );

	return true;
}



/*
 * StructCreature가 데미지를 받아 이를 반영하는 과정은 크게 4단계로 나뉜다.
 *
 *
 * 1. 데미지 계산에 필요한 각종 정보들을 수집하는 단계
 *
 *  인자로 주어진 DamageMessage를 해석, 데미지를 주고 받는 StructCreature로 부터 데미지 계산에 필요한 수치들을 수집한다.
 *  이 과정에 수행 순서에 의존하는 로직이 들어가면 부적절하다. 만약 필요하다면 DamageCalculator를 수정, 3번 단계에서
 *  해당 로직을 수행하도록 해야 한다.
 *
 *
 * 2. 데미지 계산에 영향을 줄 수 있는 외적 요인들을 반영하는 단계
 *
 *  크리티컬, 회피 시뮬레이션과 같이 순수 계산 과정에서는 예상하기 힘든, 데미지 계산에 영향을 줄 수 있는 외적 요인들을
 *  반영하는 단계이다.
 *
 *
 * 3. 실제 적용되는 데미지를 계산하는 단계
 *
 *  위에서 수집된 정보들과 시뮬레이션 결과를 기반으로 실제로 적용되는 데미지과 효과 적용 단계에 필요한 결과를 계산한다.
 *
 *
 * 4. 데미지로 인한 효과를 적용하는 단계
 *
 *  계산의 결과 값을 실제로 StructCreature에 반영한다.
 *
 *
 * 계산 과정을 굳이 이렇게 분리한 까닭은 아래와 같다.
 *
 *  - 대부분의 코드가 계산 순서에 심하게 의존하고 있으며, 이로 인해 계산에 영향을 줄 수 있는 다양한 요인들이 서로 엉켜 있다.
 *  - 이러한 환경 하에서는 계산 과정 전반에 영향을 줄 수 있는 요인을 추가하기가 까다롭다.
 *  - 또한 외부 요인에 의해 계산 결과가 바뀔 수 있으므로 자동화된 테스트의 도입이 불가능에 가깝다.
 *
 * 이러한 이유로 순수한 계산 로직과 외적 요인들을 분리, 실제 계산에는 최소한의 복잡도만이 노출되도록 구조를 변경하였다.
 *
 *
 * TODO:
 *
 * 현재는 외부에 노출되는 인터페이스를 바꾸지 않는 한도 내에서만 수정하였기에 구조적인 측면에서 일부 부적절한 면이 있다. 
 * 최종적으로는 데미지를 입히기를 원하는 호출부에서 DamageMessage를 직접 생성하여 이용하는 것이 적절할 것이다. (이게 바로
 * 데미지 구조체에 Message라는 단어를 사용한 이유이다.)
 *
 * 이러면 호출부는 DamageMessage 내부에 직접 각종 연산의 (크리, 회피, 데미지 리듀스, 관통 등의 다양한 요소들) 적용 여부나
 * 해당 데미지에만 적용되는 보너스 등의 고유한 정보들을 세팅하여 StructCreature에 질의를 날리고, 이러면 StructCreature는
 * 이 값과 대상, 공격자의 파라메터들을 토대로 DamageCalculator를 생성하여 계산된 결과를 자신과 상대에게 반영하도록 하는
 * 그림이 나올 것이다.
 *
 */

StructCreature::_DAMAGE StructCreature::DealDamage( const DamageMessage& message )
{	
	DamageCalculator calculator( message );
	
	CalculationResult result;
	calculator.Calculate( result );

	StructCreature* target = message.target;

	result.finalDamage = target->onDamage( message.from, message.elementalType, message.oldDamageType, result.damage, result.simulation.critical, result.simulation.doubleattack );
	target->damage( message.from, result.finalDamage );
	
	ProcessPostDamageEffect( message, result );

	return CreateDamageStruct( result );
}


// 데미지 계산에 필요한 각종 수치를 모아서 calculator에 반영하는 함수
// 계산 순서에 의존하거나 결과를 예상할 수 없는 로직은 가급적 여기에 넣지 말 것.
void StructCreature::ProvideAttackerInfo( const DamageMessage& message, DamageCalculator& calculator )
{
	StructCreature* target = message.target;
	
	calculator.expertiseAdvantage.amp = 1.0f + m_Expert[target->GetCreatureGroup()].fDamage;

	if( message.damageType == DamageMessage::STATE_DAMAGE && message.elementalType >= 0 && message.elementalType < Elemental::TYPE_COUNT )
	{
		calculator.beforeDefence.amp += m_ElementalStateDamageAmplifier[message.elementalType];
		calculator.beforeDefence.inc += m_ElementalStateDamageAdder[message.elementalType];
	}

	if( message.attackType == DamageMessage::ADDITIONAL_DAMAGE && IsUsingDoubleWeapon() )
	{
		if( message.damageType == DamageMessage::PHYSICAL_LEFT_HAND_ATTACK )
		{
			calculator.rule.amp *= ( 0.44f + m_nDoubleWeaponMasteryLevel * 0.02f );
		}
		else if ( message.damageType == DamageMessage::PHYSICAL_ATTACK )
		{
			calculator.rule.amp *= ( 0.90f + m_nDoubleWeaponMasteryLevel * 0.01f );
		}
	}

	// 방어력/공격력/관통력 관련 정보
	calculator.attackerLevel = GetLevel();
	calculator.critical = GetCritical();
	calculator.critPower = GetCriticalPower();

	if( message.attackType == DamageMessage::MAGICAL_ATTACK )
	{
		calculator.attackPoint = GetMagicPoint();
		calculator.penetration = m_Attribute.fMagicalPenetration;
		calculator.penetrationRatio = m_Attribute.fMagicalPenetrationRatio * 0.01f;
		calculator.defenceIgnore = m_Attribute.fMagicalDefIgnore;
		calculator.defenceIgnoreRatio = m_Attribute.fPhysicalDefIgnoreRatio * 0.01f;
		calculator.accuracy = GetMagicAccuracy();
	}
	else
	{
		calculator.attackPoint = GetAttackPointRight();
		calculator.penetration = m_Attribute.fPhysicalPenetration;
		calculator.penetrationRatio = m_Attribute.fPhysicalPenetrationRatio * 0.01f;
		calculator.defenceIgnore = m_Attribute.fPhysicalDefIgnore;
		calculator.defenceIgnoreRatio = m_Attribute.fPhysicalDefIgnoreRatio * 0.01f;

		if( message.attackType == DamageMessage::PHYSICAL_LEFT_HAND_ATTACK )
		{
			calculator.accuracy = GetAccuracyLeft();
		}
		else
		{
			calculator.accuracy = GetAccuracyRight();
		}
	}

	if ( IsSkillProp() )
	{
		calculator.flag &= ~DamageMessage::BLOCK;
		calculator.damageType = DamageMessage::NAKED_DAMAGE;
	}
	else
	{
		calculator.damageType = message.damageType;
	}

	for ( std::vector< StructSkill* >::iterator it = message.from->m_vSkillRelatedToDamaging.begin();
	      it != message.from->m_vSkillRelatedToDamaging.end(); ++it )
	{
		ApplySkillToCalculation( message, calculator, *it );		
	}
}

void StructCreature::ProvideTargetInfo( const DamageMessage& message, DamageCalculator& calculator )
{
	StructCreature* from = message.from;

	calculator.expertisePenalty.amp = 1.0f - m_Expert[from->GetCreatureGroup()].fAvoid;

	// TODO: 아래 수식을 실행하면 calculator.resist.amp는 무조건 1.0f로 나옴.
	// 당장은 동작을 그대로 옮기나, 버그일 가능성이 크니 차후 수정
	calculator.resist.amp -= GetElementalResist( message.elementalType ) / 300;

	calculator.targetLevel = GetLevel();
	calculator.manaShieldLimit = GetMP();

	if( message.attackType == DamageMessage::MAGICAL_ATTACK ) 
	{
		calculator.defence = GetMagicDefence();
		calculator.avoid = GetMagicAvoid();
		calculator.isImmune = IsMagicalImmune();
		calculator.manaShieldRatio = m_fMagicalDamageManaShieldAbsorbRatio;
	}
	else
	{
		calculator.defence = GetDefence();
		calculator.blockDefence = calculator.defence + GetBlockDefence();
		calculator.avoid = GetAvoid();
		calculator.blockRate = GetBlockChance();
		calculator.perfectBlockRate = m_Attribute.fPerfectBlock;
		calculator.isImmune = IsPhysicalImmune();
		calculator.manaShieldRatio = m_fPhysicalDamageManaShieldAbsorbRatio;
	}

	if ( !IsWearShield() && !IsSummon() )
	{
		calculator.flag &= ~DamageMessage::BLOCK;
	}

	if( ( IsPlayer() || IsSummon() ) && this != from )
	{
		if( from->IsPlayer() )
		{
			calculator.rule.amp *= GameRule::fPVPDamageRateForPlayer;
		}
		else if( from->IsSummon() )
		{
			calculator.rule.amp *= GameRule::fPVPDamageRateForSummon;
		}
	}	

	if ( message.flag & DamageMessage::DAMAGE_REDUCE )
	{
		StructCreature* target = message.target;

		calculator.statePenalty.inc -= CalculateDamageReduceSum( message, m_vDamageReduceValueInfo );
		calculator.statePenalty.amp -= CalculateDamageReduceSum( message, m_vDamageReducePercentInfo );
	}

	// 플레이어 전용
	// 앉은 상태에서 피격 시 모든 공격이 크리티컬로 터진다.
	if( IsPlayer() )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer * >( this );

		if( pPlayer->GetBattleArenaID() )
			calculator.isIgnoreLevelPenalty = true;
		if( pPlayer->IsSitDown() )
			calculator.critRateInc = 100;
	}
}

const float StructCreature::CalculateDamageReduceSum( const DamageMessage& message, const std::vector< DamageReduceInfo >& reduceInfo )
{
	std::vector< DamageReduceInfo >::const_iterator dit;
	const int targetGroup = message.from->GetCreatureGroup();
	float sum = 0.0f;

	for( dit = reduceInfo.begin() ; dit != reduceInfo.end() ; ++dit )
	{
		if( dit->IsAppliableCreatureGroup( targetGroup ) && dit->ratio > XRandom() % 100 )
		{
			switch ( message.attackType )
			{
			case DamageMessage::MAGICAL_ATTACK:
				sum += dit->magical_skill_reduce;
				break;
			case DamageMessage::PHYSICAL_ATTACK:
			case DamageMessage::PHYSICAL_LEFT_HAND_ATTACK:
				sum += dit->physical_reduce;
				break;
			case DamageMessage::PHYSICAL_SKILL_ATTACK:
				sum += dit->physical_skill_reduce;
				break;
			default:
				assert( false );
			}
		}
	}

	return sum;
}



void StructCreature::ApplySkillToCalculation( const DamageMessage& message, DamageCalculator& calculator, StructSkill* skill )
{
	StructCreature* target = message.target;
	StructCreature* from = message.from;

	int effectType = skill->GetSkillBase()->GetEffectType();
	switch ( effectType )
	{
	case SkillBase::EF_INC_DAMAGE_BY_TARGET_STATE:
	case SkillBase::EF_AMP_DAMAGE_BY_TARGET_STATE:
	{
		bool isAndOp = ( skill->GetVar( 12 ) == 1 );
		bool apply = isAndOp;

		for ( int i = 0; i < 12; i++ )
		{
			StructState::StateCode id = (StructState::StateCode)(int)skill->GetVar( i );

			if( id == 0 )
			{
				break;
			}

			if( target->GetState( id ) != NULL )
			{
				if( isAndOp == false )
				{
					apply = true;
					break;
				}
			}
			else if( isAndOp == true )
			{
				apply = false;
				break;
			}
		}

		if( apply == true )
		{
			c_fixed10 damage;

			if( message.damageType == DamageMessage::DIRECT_DAMAGE )
			{
				switch( message.attackType )
				{
				case DamageMessage::PHYSICAL_ATTACK:
				case DamageMessage::PHYSICAL_LEFT_HAND_ATTACK:
					damage = skill->GetVar( 14 ) + skill->GetVar( 15 ) * skill->GetCurrentSkillLevel();
					break;
				case DamageMessage::PHYSICAL_SKILL_ATTACK:
					damage = skill->GetVar( 16 ) + skill->GetVar( 17 ) * skill->GetCurrentSkillLevel();
					break;
				case DamageMessage::MAGICAL_ATTACK:
					damage = skill->GetVar( 18 ) + skill->GetVar( 19 ) * skill->GetCurrentSkillLevel();
					break;
				}
			}

			if( effectType == SkillBase::EF_INC_DAMAGE_BY_TARGET_STATE )
			{
				calculator.afterDefence.inc += damage;
			}
			else 
			{
				calculator.afterDefence.amp += damage;
			}
		}
		break;
	}
	case SkillBase::EF_INC_DAMAGE_INC_CRIT_RATE_BY_TARGET_HP_RATIO:
	case SkillBase::EF_AMP_DAMAGE_INC_CRIT_RATE_BY_TARGET_HP_RATIO:
	{
		c_fixed10 condition = skill->GetVar( 0 ) + skill->GetVar( 1 ) * skill->GetCurrentSkillLevel();
		c_fixed10 HPPercentage = ( static_cast< c_fixed10 > ( target->GetHP() ) * 100 ) / target->GetMaxHP();

		if( HPPercentage <= condition )
		{
			bool targetIsInProperGroup = false;

			for ( int i = 11; i < 16; i++ )
			{	
				int requiredGroup = skill->GetVar( i );

				if( requiredGroup == CREATURE_ALL || requiredGroup == target->GetCreatureGroup() )
				{
					targetIsInProperGroup = true;
					break;
				}
				else if( requiredGroup == CREATURE_NONE )
				{
					break;
				}
			}

			if( targetIsInProperGroup == true && XRandom( 0, 99 ) < skill->GetVar( 10 ) )
			{
				c_fixed10 damage;
				
				if( message.damageType == DamageMessage::DIRECT_DAMAGE )
				{
					switch( message.attackType )
					{
					case DamageMessage::PHYSICAL_ATTACK:
					case DamageMessage::PHYSICAL_LEFT_HAND_ATTACK:
						damage = skill->GetVar( 2 ) + skill->GetVar( 3 ) * skill->GetCurrentSkillLevel();
						break;					
					case DamageMessage::PHYSICAL_SKILL_ATTACK:
						damage = skill->GetVar( 4 ) + skill->GetVar( 5 ) * skill->GetCurrentSkillLevel();
						break;
					case DamageMessage::MAGICAL_ATTACK:
						damage = skill->GetVar( 6 ) + skill->GetVar( 7 ) * skill->GetCurrentSkillLevel();
						break;
					}
				}

				if( effectType == SkillBase::EF_INC_DAMAGE_INC_CRIT_RATE_BY_TARGET_HP_RATIO )
				{
					calculator.afterDefence.inc += damage;
				}
				else 
				{
					calculator.afterDefence.amp += damage;
				}

				calculator.critRateInc += skill->GetVar( 8 ) + skill->GetVar( 9 ) * skill->GetCurrentSkillLevel();
			}
		}
		break;
	}
	default:
		// 엉뚱한/미구현 스킬이 m_vSkillRelatedToDamaging에 등록된 상황이다!
		assert( false );
		break;
	}
}



void StructCreature::ProcessPostDamageEffect( const DamageMessage& message, const CalculationResult& result )
{
	ApplyManaShieldEffect( message, result );
	InterruptSpellCast( message, result );

	if( !result.simulation.missed )
	{
		RemoveStateOnDamage( message, result );
		ConsumeSoulStoneDurabilityOnDamage( message, result );
		ConsumeEtherealDurabilityOnDamage( message, result );
	}

	ReflectDamage( message, result );
}

void StructCreature::ApplyManaShieldEffect( const DamageMessage& message, const CalculationResult& result )
{
	StructCreature* target = message.target;

	if ( result.manaShieldAbsorption > 0 )
	{
		target->AddMP( -result.manaShieldAbsorption );

		BroadcastHPMPMsg( message.target, 0, result.manaShieldAbsorption );
	}
}

void StructCreature::InterruptSpellCast( const DamageMessage& message, const CalculationResult& result )
{
	StructCreature* target = message.target;

	// 캐스팅 캔슬 처리
	if( target->IsUsingSkill() )
	{
		target->m_pCastSkill->onDamage( result.damage );
	}
}

void StructCreature::RemoveStateOnDamage( const DamageMessage& message, const CalculationResult& result )
{
	StructCreature* target = message.target;

	target->RemoveStatesOnDamaged();

	target->RemoveState( StructState::SLEEP,		GameRule::MAX_STATE_LEVEL );
	target->RemoveState( StructState::NIGHTMARE,	GameRule::MAX_STATE_LEVEL );

	// 맞으면 샤인월 해제된다
	if( message.attackType != DamageMessage::MAGICAL_ATTACK )
		target->RemoveState( StructState::SHINE_WALL,	GameRule::MAX_STATE_LEVEL );

	// 맞으면 일정 확률로 빙결 해제
	const StructState::StateCode nFrozen[] = { StructState::FROZEN, StructState::TOTAL_FROZEN };
	for( int i = 0; i < 2; ++i )
	{
		StructState * pFrozen = target->GetState( nFrozen[i] );
		if( pFrozen )
		{
			int nRatio = ( pFrozen->GetValue(4) + pFrozen->GetLevel() * pFrozen->GetValue(5) ) * 100.0f;

			if( XRandom() % 100 < nRatio )
				target->RemoveState( nFrozen[i],		GameRule::MAX_STATE_LEVEL );
		}
	}
}

void StructCreature::ConsumeSoulStoneDurabilityOnDamage( const DamageMessage& message, const CalculationResult& result )
{
	StructCreature* target = message.target;

	// 소울스톤 내구도 감소
	for( int pos = 0 ; pos < 5 ; ++pos )
	{
		static const ItemBase::ItemWearType WeaponIndices[ 5 ] = { ItemBase::WEAR_ARMOR, ItemBase::WEAR_HELM, ItemBase::WEAR_GLOVE, ItemBase::WEAR_BOOTS, ItemBase::WEAR_MANTLE };

		ItemBase::ItemWearType idx = WeaponIndices[ pos ];

		StructItem *pItem = target->m_anWear[ idx ];
		if( pItem && pItem->GetCurrentEndurance() > 0 && pItem->GetUsingSocketCount() > 0 )
		{
			int nPrevEndurance = pItem->GetCurrentEndurance();
			pItem->SetCurrentEndurance( pItem->GetCurrentEndurance() - GameRule::ENDURANCE_REDUCE_ON_DAMAGE );
			if( target->IsPlayer() && GameRule::GetDecreasedEndurancePoint( nPrevEndurance, pItem->GetCurrentEndurance() ) > 0 )
				SendItemMessage( static_cast< StructPlayer * >( target ), pItem );

			if( pItem->GetCurrentEndurance() == 0 )
				target->SetNeedCalculateStat();
		}
	}
}

void StructCreature::ConsumeEtherealDurabilityOnDamage( const DamageMessage& message, const CalculationResult& result )
{
	StructCreature* from = message.from;
	StructCreature* target = message.target;

	// 에테리얼 내구도 감소(평타, 평타-왼손, 물리스킬, 마법스킬, 물리지속, 마법지속 데미지일 때만 적용)
	if( message.damageType != DamageMessage::ADDITIONAL_DAMAGE )
	{
		// 공격자가 자신이 아니고 반사막에 의한 공격이 아닐 경우 에테리얼 내구도 소모 적용
		if( from != target && !from->IsProcessingReflectDamage() )
		{
			// 공격자의 에테리얼 내구도 소모는 공격자가 준 데미지 기준으로 적용
			from->ProcEtherealDurabilityConsumption( true, message.oldDamageType, message.damage );
		}

		// 피격자의 에테리얼 내구도 소모는 피격자가 받은 데미지 기준으로 적용
		target->ProcEtherealDurabilityConsumption( false, message.oldDamageType, result.damage );
	}
}

void StructCreature::ReflectDamage( const DamageMessage& message, const CalculationResult& result )
{
	StructCreature* from = message.from;
	StructCreature* target = message.target;

	const std::vector< StructCreature::DamageReflectInfo > reflectInfo = target->m_vDamageReflectInfo;

	// 두 케릭이 서로 반사막 켜고 피케하면 연속 반사 되는 걸 방지하기 위해 IsProcessingReflectDamage() 사용
	// 이 부분은 상대방을 죽일 수도 있으므로 상대방에게 적용시키는 모든 동작의 맨 마지막이 되어야 함
	// Ex. 지속효과 부여 -> 반사 데미지 처리시 사망 : 부여된 지속효과 자동 제거 (O)
	//     반사 데미지 처리시 사망 -> 지속효과 부여 : 사망 후 지속효과가 부여됨 (X)
	if( !target->IsProcessingReflectDamage() && !from->IsProcessingReflectDamage() && !reflectInfo.empty() )
	{
		target->m_StatusFlag.On( STATUS_PROCESSING_REFELCT );

		for( std::vector< DamageReflectInfo >::const_iterator it = reflectInfo.begin(); it != reflectInfo.end(); ++it )
		{
			if( XRandom( 0, 100 ) < it->fire_ratio &&
			    from->GetCurrentPosition( GetArTime() ).GetDistance( target->GetCurrentPosition( GetArTime() ) ) <=
			    it->range * GameRule::DEFAULT_UNIT_SIZE )
			{
				int nPrevHP = from->GetHP();
				int nPrevMP = from->GetMP();
				int nDamageFlag = IGNORE_AVOID | IGNORE_BLOCK | IGNORE_CRITICAL;
				
				if( it->bIgnoreDefence )
					nDamageFlag |= IGNORE_DEFENCE;

				// 직접적으로 받은 데미지 유형일 경우만 반사막 일반 데미지 작동(일반 물리 공격, 왼손 물리 공격, 마법 공격)
				if( message.damageType == DamageMessage::DIRECT_DAMAGE )
				{
					if ( it->nReflectDamage )
					{
						from->DealAdditionalMagicalDamage( target, it->nReflectDamage, it->type, 0, 0, nDamageFlag );
					}

					int nReflectDamage = 0;

					switch( message.attackType )
					{
					case DamageMessage::PHYSICAL_ATTACK:
					case DamageMessage::PHYSICAL_LEFT_HAND_ATTACK:
						// 물리 데미지 비율 반사는 물리 데미지일 경우만 작동(반사도 물리 데미지임)
						nReflectDamage = it->fPhysicalReflectRatio * result.finalDamage;
						if ( nReflectDamage )
						{
							from->DealPhysicalDamage( target, nReflectDamage, it->type, 0, 0, nDamageFlag );
						}
						break;
					case DamageMessage::PHYSICAL_SKILL_ATTACK:
						// 물리 스킬 데미지 비율 반사는 물리 스킬 데미지일 경우만 작동(반사도 물리 스킬 데미지)
						nReflectDamage = it->fPhysicalSkillReflectRatio * result.finalDamage;
						if ( nReflectDamage )
						{
							from->DealPhysicalSkillDamage( target, nReflectDamage, it->type, 0, 0, nDamageFlag );
						}
						break;
					case DamageMessage::MAGICAL_ATTACK:
						// 마법 스킬 데미지 비율 반사는 마법 스킬 데미지일 경우만 작동(반사도 마법 스킬 데미지)
						nReflectDamage = it->fMagicalReflectRatio * result.finalDamage;
						if ( nReflectDamage )
						{
							from->DealMagicalSkillDamage( target, nReflectDamage, it->type, 0, 0, nDamageFlag );
						}
						break;
					}
				}				

				// 반사막에 의한 데미지 표시를 위해 방송 후 한 번 더 보냄
				BroadcastHPMPMsg( from, from->GetHP() - nPrevHP, from->GetMP() - nPrevMP );

				if( target->IsPlayer() )
				{
					SendHPMPMsg( static_cast< StructPlayer * >( target ), from, from->GetHP() - nPrevHP, from->GetMP() - nPrevMP, true );
				}
				else if( target->IsSummon() && static_cast< StructSummon * >( target )->GetMaster() )
				{
					SendHPMPMsg( static_cast< StructSummon * >( target )->GetMaster(), from, from->GetHP() - nPrevHP, from->GetMP() - nPrevMP, true );
				}

				if( from->IsPlayer() )
				{
					SendHPMPMsg( static_cast< StructPlayer * >( from ), from, from->GetHP() - nPrevHP, from->GetMP() - nPrevMP, true );
				}
				else if( from->IsSummon() && static_cast< StructSummon * >( from )->GetMaster() )
				{
					SendHPMPMsg( static_cast< StructSummon * >( from )->GetMaster(), from, from->GetHP() - nPrevHP, from->GetMP() - nPrevMP, true );
				}
			}
		}

		target->m_StatusFlag.Off( STATUS_PROCESSING_REFELCT );
	}
}


StructCreature::_DAMAGE StructCreature::CreateDamageStruct( const CalculationResult& result )
{
	_DAMAGE damage;

	damage.nDamage = result.finalDamage;
	damage.nResistedDamage = result.resistedDamage;
	damage.bBlock = result.simulation.blocked;
	damage.bCritical = result.simulation.critical;
	damage.bMiss = result.simulation.missed;
	damage.bPerfectBlock = result.simulation.perfectBlocked;

	return damage;
}




StructCreature::_DAMAGE_INFO StructCreature::DealMagicalSkillDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag )
{
	_DAMAGE_INFO damage_info;
	DamageMessage message( pFrom, this, DT_NORMAL_MAGICAL_DAMAGE, nFlag, elemental_type, nDamage );
	
	message.penalty = &m_MagicalSkillStatePenalty;
	message.flag |= DamageMessage::DAMAGE_REDUCE;
	message.criticalBonus = critical_bonus + pFrom->m_BadMagicalElementalSkillStateMod[elemental_type].nCritical;
	message.accuracyBonus = accuracy_bonus + pFrom->m_BadMagicalElementalSkillStateMod[elemental_type].nPhysicalAccuracy;

	damage_info.SetDamage( DealDamage( message ) );

	ProcessAdditionalDamage( message, DT_ADDITIONAL_MAGICAL_DAMAGE, pFrom->m_vMagicalSkillAdditionalDamage, damage_info );

	damage_info.target_hp = GetHP();

	return damage_info;
}

StructCreature::_DAMAGE_INFO StructCreature::DealPhysicalNormalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag )
{
	_DAMAGE_INFO damage_info;
	DamageMessage message( pFrom, this, DT_NORMAL_PHYSICAL_DAMAGE, nFlag, elemental_type, nDamage );
	
	bool bRange = ( pFrom->IsUsingBow() || pFrom->IsUsingCrossBow() ) && pFrom->IsPlayer();
	
	message.advantage = bRange ? &pFrom->m_RangeStateAdvantage : &pFrom->m_NormalStateAdvantage;
	message.penalty = bRange ? &m_RangeStatePenalty : &m_NormalStatePenalty;
	message.flag |= DamageMessage::DAMAGE_REDUCE;
	message.criticalBonus = critical_bonus;
	message.accuracyBonus = accuracy_bonus;

	damage_info.SetDamage( DealDamage( message ) );

	ProcessAdditionalDamage( message, DT_ADDITIONAL_DAMAGE,
		bRange ? pFrom->m_vRangeAdditionalDamage : pFrom->m_vNormalAdditionalDamage, damage_info );

	damage_info.target_hp = GetHP();

	return damage_info;
}

StructCreature::_DAMAGE_INFO StructCreature::DealPhysicalNormalLeftHandDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag )
{
	_DAMAGE_INFO damage_info;
	DamageMessage message( pFrom, this, DT_NORMAL_PHYSICAL_LEFT_HAND_DAMAGE, nFlag, elemental_type, nDamage );

	message.advantage = &pFrom->m_NormalStateAdvantage;
	message.penalty = &m_NormalStatePenalty;
	message.flag |= DamageMessage::DAMAGE_REDUCE;
	message.criticalBonus = critical_bonus;
	message.accuracyBonus = accuracy_bonus;

	damage_info.SetDamage( DealDamage( message ) );

	ProcessAdditionalDamage( message, DT_ADDITIONAL_LEFT_HAND_DAMAGE, pFrom->m_vNormalAdditionalDamage, damage_info );

	damage_info.target_hp = GetHP();

	return damage_info;
}

StructCreature::_DAMAGE_INFO StructCreature::DealPhysicalSkillDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag )
{
	_DAMAGE_INFO damage_info;
	DamageMessage message( pFrom, this, DT_NORMAL_PHYSICAL_SKILL_DAMAGE, nFlag, elemental_type, nDamage );

	message.penalty = &m_PhysicalSkillStatePenalty;
	message.flag |= DamageMessage::DAMAGE_REDUCE;
	message.criticalBonus = critical_bonus + pFrom->m_BadPhysicalElementalSkillStateMod[elemental_type].nCritical;
	message.accuracyBonus = accuracy_bonus + pFrom->m_BadPhysicalElementalSkillStateMod[elemental_type].nPhysicalAccuracy;

	damage_info.SetDamage( DealDamage( message ) );

	ProcessAdditionalDamage( message, DT_ADDITIONAL_DAMAGE, pFrom->m_vPhysicalSkillAdditionalDamage, damage_info );

	damage_info.target_hp = GetHP();

	return damage_info;
}


void StructCreature::ProcessAdditionalDamage( const DamageMessage& sourceMessage, const DamageType additionalDamageType, const std::vector< AdditionalDamageInfo >& additionalDamage, _DAMAGE_INFO& damageInfo )
{
	if( !damageInfo.bMiss && !damageInfo.bPerfectBlock )
	{
		StructCreature* target = sourceMessage.target;

		// 추가 데미지 처리		 
		std::vector< AdditionalDamageInfo >::const_iterator it;
		for( it = additionalDamage.begin(); it != additionalDamage.end(); ++it )
		{
			if( ( it->require_type == 99 || it->require_type == sourceMessage.elementalType ) && it->ratio > XRandom() % 100 )
			{		
				int damage = 0;

				if( it->nDamage )
				{
					damage = it->nDamage;
				}
				else
				{
					damage = it->fDamage * damageInfo.nDamage;
				}

				DamageMessage message( sourceMessage.from, sourceMessage.target, additionalDamageType, 0, it->type, damage );

				damage = target->DealDamage( message ).nDamage;
				if ( it->type >= 0 && it->type < Elemental::TYPE_COUNT )
				{
					damageInfo.elemental_damage[ it->type ] += damage;
				}
				damageInfo.nDamage += damage;
			}
		}
	}
}


StructCreature::_DAMAGE StructCreature::DealDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, DamageType damageType, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage )
{
	_DAMAGE_INFO damage_info;
	DamageMessage message( pFrom, this, damageType, nFlag, elemental_type, nDamage );

	message.advantage = damage_advantage;
	message.penalty = damage_penalty;
	message.criticalBonus = critical_bonus;
	message.accuracyBonus = accuracy_bonus;
	
	return DealDamage( message );
}


// 이하 함수들은 인터페이스 차원의 호환을 위해 남겨두었으나, 이런 류의 복붙 래퍼 함수가 굳이 필요한지는 의문이다.
// TODO : 추후 리팩토링을 통해 인터페이스를 적절하게 정리하도록 하자.
// 데미지를 리턴함
StructCreature::_DAMAGE StructCreature::DealMagicalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage )
{
	return DealDamage( pFrom, nDamage, elemental_type, DT_NORMAL_MAGICAL_DAMAGE, accuracy_bonus, critical_bonus, nFlag, damage_penalty, damage_advantage );
}

// 데미지를 리턴함
StructCreature::_DAMAGE StructCreature::DealPhysicalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage )
{
	return DealDamage( pFrom, nDamage, elemental_type, DT_NORMAL_PHYSICAL_DAMAGE, accuracy_bonus, critical_bonus, nFlag, damage_penalty, damage_advantage );
}

// 데미지를 리턴함
StructCreature::_DAMAGE StructCreature::DealStateMagicalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage )
{
	return DealDamage( pFrom, nDamage, elemental_type, DT_STATE_MAGICAL_DAMAGE, accuracy_bonus, critical_bonus, nFlag, damage_penalty, damage_advantage );
}

// 데미지를 리턴함
StructCreature::_DAMAGE StructCreature::DealPhysicalStateDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage )
{
	return DealDamage( pFrom, nDamage, elemental_type, DT_STATE_PHYSICAL_DAMAGE, accuracy_bonus, critical_bonus, nFlag, damage_penalty, damage_advantage );
}

// 데미지를 리턴함
StructCreature::_DAMAGE StructCreature::DealPhysicalLeftHandDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage )
{
	return DealDamage( pFrom, nDamage, elemental_type, DT_NORMAL_PHYSICAL_LEFT_HAND_DAMAGE, accuracy_bonus, critical_bonus, nFlag, damage_penalty, damage_advantage );
}

// 데미지를 리턴함
StructCreature::_DAMAGE StructCreature::DealAdditionalMagicalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage )
{
	return DealDamage( pFrom, nDamage, elemental_type, DT_ADDITIONAL_MAGICAL_DAMAGE, accuracy_bonus, critical_bonus, nFlag, damage_penalty, damage_advantage );
}

// 데미지를 리턴함
StructCreature::_DAMAGE StructCreature::DealAdditionalDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage )
{
	return DealDamage( pFrom, nDamage, elemental_type, DT_ADDITIONAL_DAMAGE, accuracy_bonus, critical_bonus, nFlag, damage_penalty, damage_advantage );
}

// 데미지를 리턴함
StructCreature::_DAMAGE StructCreature::DealAdditionalLeftHandDamage( StructCreature *pFrom, int nDamage, Elemental::Type elemental_type, int accuracy_bonus, int critical_bonus, int nFlag, const StateMod * damage_penalty, const StateMod * damage_advantage )
{
	return DealDamage( pFrom, nDamage, elemental_type, DT_ADDITIONAL_LEFT_HAND_DAMAGE, accuracy_bonus, critical_bonus, nFlag, damage_penalty, damage_advantage );
}

