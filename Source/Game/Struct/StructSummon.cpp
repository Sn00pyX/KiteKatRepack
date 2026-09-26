
#include <mmo/ArcadiaServer.h>
#include <toolkit/XRandom.h>
#include <toolkit/XSTLUtil.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "StructSummon.h"
#include "GameContent.h"
#include "SendMessage.h"
#include "StructItem.h"
#include "StructPlayer.h"
#include "StructMonster.h"
#include "DB_Commands.h"
#include "StructSkill.h"
#include "GameProc.h"
#include "GameMessage.h"
#include "CompeteManager.h"
#include "GameAllocator.h"
#include "LuaVM.h"


void StructSummon::BindProperty()
{
	StructSummon temp( "for bind property" );
	temp.Bind( "handle", (int *)&temp.m_hHandle );
	temp.Bind( "hp", &temp.m_nHP );
	temp.Bind( "mp", &temp.m_nMP );
	temp.Bind( "sp", &temp.m_nSP );
	temp.Bind( "max_hp", &temp.m_nMaxHP );
	temp.Bind( "max_mp", &temp.m_nMaxMP );
	temp.Bind( "max_sp", &temp.m_nMaxSP );
	temp.BindCString( "name", temp.m_szName, _countof( temp.m_szName ) );
	temp.Bind( "exp", &temp.m_nEXP );
	temp.Bind( "jp", &temp.m_nJobPoint );
	temp.Bind( "job_level", &temp.m_nJobLevel );
	temp.Bind( "jlv", &temp.m_nJobLevel );
	temp.Bind( "ev_1_ID", &temp.m_nPrevJobId[0] );
	temp.Bind( "ev_2_ID", &temp.m_nPrevJobId[1] );
	temp.Bind( "ev_1_level", &temp.m_nPrevJobLevel[0] );
	temp.Bind( "ev_2_level", &temp.m_nPrevJobLevel[1] );
	temp.Bind( "job", &temp.m_nJob );
	temp.Bind( "level", &temp.m_nLevel );	
	temp.Bind( "lv", &temp.m_nLevel );	
	temp.Bind( "max_level", &temp.m_nMaxReachedLevel );
	temp.Bind( "str", const_cast< __int64 * >( &temp.m_Stat.strength.get() ) );
	temp.Bind( "agi", const_cast< __int64 * >( &temp.m_Stat.agility.get() ) );
	temp.Bind( "dex", const_cast< __int64 * >( &temp.m_Stat.dexterity.get() ) );
	temp.Bind( "int", const_cast< __int64 * >( &temp.m_Stat.intelligence.get() ) );
	temp.Bind( "luck", const_cast< __int64 * >( &temp.m_Stat.luck.get() ) );
	temp.Bind( "vital", const_cast< __int64 * >( &temp.m_Stat.vital.get() ) );
	temp.Bind( "mental", const_cast< __int64 * >( &temp.m_Stat.mentality.get() ) );
	temp.Bind( "race", &temp.m_nRace );
	temp.Bind( "x", &temp.mv.x );
	temp.Bind( "y", &temp.mv.y );
	temp.Bind( "weight", &temp.m_fWeight );	
	temp.Bind( "max_weight", const_cast< __int64 * >( &temp.m_Attribute.fMaxWeight.get() ) );	
	temp.Bind( "speed", const_cast< __int64 * >( &temp.m_Attribute.fMoveSpeed.get() ) );
	temp.Bind( "attack_point", const_cast< __int64 * >( &temp.m_Attribute.fAttackPointRight.get() ) );
	temp.Bind( "attack_point_right", const_cast< __int64 * >( &temp.m_Attribute.fAttackPointRight.get() ) );
	temp.Bind( "attack_point_left", const_cast< __int64 * >( &temp.m_Attribute.fAttackPointLeft.get() )  );
	temp.Bind( "defence", const_cast< __int64 * >( &temp.m_Attribute.fDefence.get() ) );
	temp.Bind( "magic_def", const_cast< __int64 * >( &temp.m_Attribute.fMagicDefence.get() ) );
	temp.Bind( "magic_pt", const_cast< __int64 * >( &temp.m_Attribute.fMagicPoint.get() ) );
	temp.Bind( "accuracy", const_cast< __int64 * >( &temp.m_Attribute.fAccuracyRight.get() ) );
	temp.Bind( "accuracy", const_cast< __int64 * >( &temp.m_Attribute.fAccuracyRight.get() ) );
	temp.Bind( "accuracy", const_cast< __int64 * >( &temp.m_Attribute.fAccuracyLeft.get() ) );
	temp.Bind( "attack_range", const_cast< __int64 * >( &temp.m_Attribute.fAttackRange.get() ) );
	temp.Bind( "attack_speed", const_cast< __int64 * >( &temp.m_Attribute.fAttackSpeed.get() ) );
	temp.Bind( "avoid", const_cast< __int64 * >( &temp.m_Attribute.fAvoid.get() ) );
	temp.Bind( "block_chance", const_cast< __int64 * >( &temp.m_Attribute.fBlockChance.get() ) );
	temp.Bind( "summon_state", &temp.m_bIsSummoned );
	temp.Bind( "evolution_depth", &temp.m_nTransformLevel );
	// Epic 3 이전까지 진화 시스템에서는 진화시 레벨이 초기화 되므로
	// 따로 계산이 필요했으나 Epic 4부터 현재 레벨을 총 레벨로 보면 됨
	temp.Bind( "total_level", &temp.m_nLevel );
}

StructSummon* StructSummon::AllocSummon( struct StructPlayer* pMaster, unsigned idx )
{
	StructSummon* p;

	struct _myIntializer : GameAllocateFunctor
	{
		_myIntializer( int _idx ) : m_nIdx( _idx ) {}
		
		virtual void operator()( void * pObj, AR_HANDLE handle )
		{
			new (pObj) StructSummon( handle, m_nIdx );
		}

		unsigned m_nIdx;
	};

	AR_HANDLE handle = allocSummonStruct( &p, _myIntializer( idx ) );

	// new (p) StructSummon( handle, idx );

	p->m_pMaster = pMaster;
	p->CalculateStat();

	p->m_nHP = p->m_nMaxHP;
	p->m_nMP = p->m_nMaxMP;

	p->m_nMaxReachedLevel = 1;
	p->m_nJobPoint = 1;

	p->m_fCM = 0;

	// 소환수는 id + 1000 이 직업 ID 임.


	if( p->GetRidingInfo() == SummonBase::RIDING_LENT )
	{
		// 라이딩 스킬 넣어줘야 함.
		p->SetSkill( StructSkill::SKILL_UID_SUMMON_SKILL, StructSkill::SKILL_CREATURE_RIDING, 1, 0 );
	}
	return p;
}

char StructSummon::GetRidingInfo() const
{
	return m_pContentInfo->is_riding_only;
}

void StructSummon::FreeSummon( StructSummon* p )
{
	// _oprint( "DEL SUMMON : %08X\n", p );

	prepareFreeSummonStruct( p );

	p->StructSummon::~StructSummon();

	freeSummonStruct( p );
}

void StructSummon::onSPChange()
{
	if( GetMaster() )
		SendSPMsg( GetMaster(), this );
}

void StructSummon::AddExp( __int64 exp, __int64 jp, bool bApplyStamina, bool bForce )
{
	__int64 exp_limit = 0;

	// exp나 jp가 크면 float은 오류가 너무 많이 나므로 double로 계산 해줌
	exp *= static_cast< double >( m_fEXPMod + 1.0f );
	jp *= static_cast< double >( m_fEXPMod + 1.0f );

	if( !bForce && m_pMaster  )
		exp = std::min( exp, static_cast< __int64 >( GameRule::GetSummonEXPLimit( GetLevel() ) ) );

	// 획득 가능 한계 경험치를 체크하여 더 이상 경험치 획득 불가능 상태면 획득 취소
	switch( GetTransformLevel() )
	{
	case SummonBase::EVOLVE_GROWTH:
		exp_limit = GameContent::GetNeedSummonExp( GameRule::GROWTH_SUMMON_MAX_LEVEL ) - 1;

		if( exp_limit <= m_nEXP )
			return;

		else if( GameContent::GetNeedSummonExp( GameRule::GROWTH_SUMMON_MAX_LEVEL - 1 ) < m_nEXP )
		{
			exp = std::min( __int64(exp), exp_limit - m_nEXP );
		}
		break;
	case SummonBase::EVOLVE_EVOLVE:
		exp_limit = GameContent::GetNeedSummonExp( GameRule::EVOLVE_SUMMON_MAX_LEVEL ) - 1;

		if( exp_limit <= m_nEXP )
			return;

		else if( GameContent::GetNeedSummonExp( GameRule::EVOLVE_SUMMON_MAX_LEVEL - 1 ) < m_nEXP )
		{
			exp = std::min( __int64( exp ), exp_limit - m_nEXP );
		}
		break;
	default:
		exp_limit = GameContent::GetNeedSummonExp( GameRule::NORMAL_SUMMON_MAX_LEVEL ) - 1;

		if( exp_limit <= m_nEXP )
			return;

		else if( GameContent::GetNeedSummonExp( GameRule::NORMAL_SUMMON_MAX_LEVEL - 1 ) < m_nEXP )
		{
			exp = std::min( __int64( exp ), exp_limit - m_nEXP );
		}
		break;

	}
	StructCreature::AddExp( exp, jp );
}

void StructSummon::onExpChange()
{
	int nLevel = 1;
	int nJP = 0;

	int nMaxLevel = 0;
	int nEvolvableLevel = 0;

	switch( GetTransformLevel() )
	{
	case SummonBase::EVOLVE_NORMAL:
		nMaxLevel = GameRule::NORMAL_SUMMON_MAX_LEVEL;
		nEvolvableLevel = GameRule::NORMAL_SUMMON_EVOLVE_LEVEL;
		break;
	case SummonBase::EVOLVE_GROWTH:
		nMaxLevel = GameRule::GROWTH_SUMMON_MAX_LEVEL;
		nEvolvableLevel = GameRule::GROWTH_SUMMON_EVOLVE_LEVEL;
		break;
	case SummonBase::EVOLVE_EVOLVE:
		nMaxLevel = GameRule::EVOLVE_SUMMON_MAX_LEVEL;
		nEvolvableLevel = nMaxLevel;
		break;
	}

	// 레벨업 처리 전에 m_nMaxReachedLevel(기존 도달 최대 레벨-렙다 후 렙업시 jp획득 방지용)을 확인
	if( m_nMaxReachedLevel < m_nLevel )
		m_nMaxReachedLevel = m_nLevel;

	while( true )
	{
		if( nLevel >= nMaxLevel ) break;

		// 레벨업에 필요한 경험치 테이블 검사로 레벨업 가능한 모든 구간을 처리
		__int64 nNeedExp = GameContent::GetNeedSummonExp( nLevel );

		if( nNeedExp == 0 )
			break;

		if( nNeedExp > m_nEXP )
			break;

		++nLevel;

		if( m_nMaxReachedLevel < nLevel )
		{
			++nJP;

			// 오버브리드의 경우에는 JP를 1개 더 줌
			if( nLevel > nEvolvableLevel )
				++nJP;
		}
	}

	if( GetMaster() )
		SendExpMsg( GetMaster(), this );

	if( nLevel && nLevel != m_nLevel )
	{
		__int64 nCardUID = 0;

		if( GetParentCard() )
		{
			nCardUID = GetParentCard()->GetItemUID();
		}

		LOG::Log11N4S( LM_SUMMON_LEVEL_UP, GetMaster()->GetAccountID(), GetMaster()->GetSID(), GetSID(), m_nLevel, nLevel, GetPrevJobLevel( 0 ), GetPrevJobLevel( 1 ), GetJobId(), m_nJobPoint, ( nLevel > m_nLevel ) ? nJP : 0, nCardUID, GetMaster()->GetAccountName(), LOG::STR_NTS, GetMaster()->GetName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, GetSummonName(), LOG::STR_NTS );

		if( nLevel > m_nLevel )
		{
			int prev_hp = GetHP();
			int prev_mp = GetMP();

			m_nJobLevel = m_nLevel = nLevel;
			m_nJobPoint += nJP;

			if( m_nMaxReachedLevel < m_nLevel )
				m_nMaxReachedLevel = m_nLevel;

			CalculateStat();

			if( !IsDead() )
			{
				m_nHP = m_nMaxHP;
				m_nMP = m_nMaxMP;
			}

			if( IsInWorld() )
			{
				BroadcastHPMPMsg( this, m_nHP - prev_hp, m_nMP - prev_mp );
			}
			else if( GetMaster() )
			{
				SendHPMPMsg( GetMaster(), this, m_nHP - prev_hp, m_nMP - prev_mp );
			}

			if( GetMaster() )
				SendPropertyMessage( GetMaster(), GetHandle(), "jp", m_nJobPoint );
		}
		else
		{
			m_nJobLevel = m_nLevel = nLevel;

			CalculateStat();
		}

		DBQuery( new DB_UpdateSummon( this ) );

		if( GetParentCard() && GetMaster() )
			SendItemMessage( GetMaster(), GetParentCard() );

		if( IsInWorld() )
			BroadcastLevelMsg( this );
		if( GetMaster() )
			SendLevelMsg( GetMaster(), this );
	}
}

void StructSummon::onEnergyChange()
{
	TS_SC_ENERGY msg;
	msg.handle = GetHandle();
	msg.energy = GetEnergyCount();

	if( IsInWorld() )
	{
	ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
}
	else if( GetMaster() )
	{
		PendMessage( GetMaster(), &msg );
	}
}

const CreatureStat &	StructSummon::GetBaseStat() const
{
	return GameContent::GetStatInfo( m_pContentInfo->stat_id );
}


const char*		StructSummon::GetSummonName()
{
	return GameContent::GetString( m_pContentInfo->name_id );
}

int StructSummon::GetSummonNameId()
{
	return m_pContentInfo->name_id;
}

const char*		StructSummon::GetName() const
{
	return m_szName;
}

int StructSummon::GetSummonCode()
{
	return m_pContentInfo->uid;
}

int StructSummon::GetSummonType()
{
	if( m_nChangingGroup != CREATURE_NONE )
		return m_nChangingGroup;

	return m_pContentInfo->type;
}

int StructSummon::GetSummonElementalType()
{
	return m_pContentInfo->magic_type;
}

bool StructSummon::SetSummonInfo( unsigned idx )
{
	m_pContentInfo = GameContent::GetSummonInfo( idx );

	m_nTransformLevel = m_pContentInfo->evolve_type;

	m_nJob = idx;

	return true;
}

int StructSummon::GetJobDepth() const
{
	return GetTransformLevel()-1;
}

void StructSummon::SetName( const char *szName )
{
	s_strcpy( m_szName, _countof( m_szName ), szName );
}

StructSummon::StructSummon( AR_HANDLE handle, unsigned idx )
{
	m_hHandle = handle;

	m_nAccountId = 0;
	m_nLastProcessTime = 0;

	m_nArObjectType = ArObject::MOVABLE_OBJECT;
	//SetTag( static_cast< GameObject* >( this ) );

	m_nTransformLevel = 0;

	SetSummonInfo( idx );

	m_nStatus = 0;

	m_nSP = m_nMaxSP = 1000;
	m_fMaxSPAmplifier = 0.0f;
	m_fSPRegenAddAmplifier = 0.0f;

	m_bIsSummoned = false;

	m_bIsRidable = true;
	
	m_pMaster = 0;
	m_pItem = NULL;

	m_nSID = 0;

	m_nJobLevel = m_nLevel = m_nMaxReachedLevel = 1;

	m_cSlotIndex = 0;

	m_nMaxItemWear = 2;

	memset( m_szName, 0, sizeof(m_szName) );

	//ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_NORMAL );

	// 임시 : 랜덤하게 다른방향 보게 하자.. -_-;
	mv.face = (float)XRandom() / 100;
	m_bIsBattleMode = false;

	m_nLastCantAttackTime = 0;
	m_nSPRegenAdd = 0;

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructSummon" );
#endif
}

StructSummon::~StructSummon()
{
	if( IsInWorld() ) 
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		RemoveSummonFromWorld( this );
	}

#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructSummon" );
#endif
}

bool StructSummon::ProcDelete()
{
	FreeSummon( this );

	return true;
}

bool StructSummon::IsUsingBow() const
{
	for( int i = 0 ; i < m_nMaxItemWear ; ++i )
	{
		if( m_anWear[i] && m_anWear[i]->IsBow() )
			return true;
	}

	return false;
}

bool StructSummon::IsUsingCrossBow() const
{
	for( int i = 0 ; i < m_nMaxItemWear ; ++i )
	{
		if( m_anWear[i] && m_anWear[i]->IsCrossBow() )
			return true;
	}

	return false;
}

bool StructSummon::TranslateWearPosition( ItemBase::ItemWearType & pos, struct StructItem* pItem, std::vector< int > * vpOverlappItemList)
{
	struct StructItem* pWearedItem;
	if( !StructCreature::TranslateWearPosition( pos, pItem, vpOverlappItemList ) ) return false;

	
	if( !pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_CARD ) ) return false;
	

	if( pos > ItemBase::MAX_SPARE_ITEM_WEAR || pos < 0 )
	{
		return false;
	}

	StructItem* pCard = this->GetParentCard();
	if (!pCard) return false;

	int nEffectID = pCard->GetEffectID();
	int nEndingPos = nEffectID ? 7 + (nEffectID - 1000) : m_nMaxItemWear;


	// 날아온 포지션은 쌩깐다.
	if( pos == (ItemBase::ItemWearType) ItemBase::MAX_SPARE_ITEM_WEAR)
	{
		int startPos = 0;
		int endPos = StructSummon::SUMMON_MAX_NON_ARTIFACT_ITEM_WEAR;

		// 악세사리 아이템은 3번째 슬롯만 장착할 수 있다.
		if( pItem->IsAccessory() )
		{
			startPos = StructSummon::SUMMON_MAX_NON_ARTIFACT_ITEM_WEAR;
			endPos = std::min( m_nMaxItemWear, static_cast< int >( StructSummon::SUMMON_MAX_ACCESSORY_ITEM_WEAR ) );
		}

		// 아티팩트 아이템은 4번째 슬롯부터 장착할 수 있다.
		if( pItem->IsArtifact() )
		{
			// 같은 계열의 소환수일 때만 장착 가능 (단, 아티팩트의 소환수코드가 없으면 모든 크리쳐용 아티팩트)
			int nSummonCode = pItem->GetSummonCode();
			if( nSummonCode )
			{
				int nJobId = GetPrevJobId( 0 );
				if( !nJobId ) nJobId = GetJobId();
				if( nJobId != nSummonCode )
					return false;
			}

			startPos = StructSummon::SUMMON_MAX_ACCESSORY_ITEM_WEAR;
			endPos = m_nMaxItemWear;
		}

		startPos = 0;
		endPos = 2 + GetEnhance();

		if (nEffectID)
			endPos = 7 + nEffectID - 1000;

		for( int i = startPos; i < endPos; ++i )
		{
			pWearedItem = m_anWear[i];

			if( !pItem->IsArtifact() && !pItem->IsAccessory() && pWearedItem && pWearedItem->GetItemGroup() == pItem->GetItemGroup() )
				return false;

			if( pWearedItem == pItem )
				return false;

			// slots 0 and 1 are for weapon and armor
			// slot 2 is for accessory
			// slot 3-4 are for artifacts
			// slot 5-6 are for weapon and armor
			// if we are upgraded then we can also do the next 7 rows


			if (pItem->IsAccessory())
			{
				if ((i != 2) && (i != 8 && i != 10 && i != 12)) continue;
			}
			else if (pItem->IsArtifact())
			{
				if (i != 3 && i != 4 && i != 9 && i != 11) continue;
			}
			else
			{
				if (i != 0 && i != 1 && i != 5 && i != 6 && i != 7 && i != 13) continue;
			}

			if( !pWearedItem && pos == ItemBase::MAX_SPARE_ITEM_WEAR)
			{
				pos = (ItemBase::ItemWearType) i;
				return true;
			}
		}

		if( pos == ItemBase::MAX_SPARE_ITEM_WEAR)
		{
			return false;
		}
	}
	else if( pos < 7 && pos >= m_nMaxItemWear )
	{
		return false;
	}
	else if (pos >= nEndingPos)
	{
		return false;
	}
	else if( vpOverlappItemList )
	{
		// 장착 위치에 무언가 입고 있다면
		if( m_anWear[ pos ] )
		{
			vpOverlappItemList->push_back( pos );
		}
	}

	return true;
}


unsigned char	StructSummon::GetRace() const
{
	return CREATURE_ETC;
}

__int64	StructSummon::GetDeadEXPPenalty()
{
	return GameContent::GetNeedSummonExp( GetLevel() ) * ( 0.15 / ( GetLevel() - 1 ) + 0.0005 );
}

bool StructSummon::IsEXPLimitReached()
{
	__int64 exp_limit = 0;

	exp_limit = GameContent::GetNeedSummonExp( GameRule::MAX_SUMMON_LEVEL ) - 1;

	return ( exp_limit <= m_nEXP );
}

void StructSummon::onDead( StructCreature *pFrom, bool decreaseEXPOnDead )
{
	StructPlayer * pMaster = GetMaster();

	if( pMaster->IsRiding() && pMaster->GetRideObject() == this )
	{
		pMaster->UnMount( StructPlayer::UNMOUNT_FALL, pFrom );
	}

	StructCreature::onDead( pFrom, decreaseEXPOnDead );

	StructPlayer * pKiller = NULL;

	if( pFrom->IsPlayer() )
	{
		pKiller = static_cast< StructPlayer * >( pFrom );
	}
	else if( pFrom->IsSummon() )
	{
		pKiller = static_cast< StructSummon * >( pFrom )->GetMaster();
	}

	char * szKillerType = "UNKN";
	int nKillerID = 0;

	if( pFrom->IsPlayer() )		{ szKillerType = "PLYR"; nKillerID = static_cast< StructPlayer* >( pFrom )->GetPlayerUID(); }
	if( pFrom->IsMonster() )	{ szKillerType = "MNST"; nKillerID = static_cast< StructMonster* >( pFrom )->GetMonsterId(); }
	if( pFrom->IsSummon() )		{ szKillerType = "SUMN"; nKillerID = static_cast< StructSummon* >( pFrom )->GetSummonSID(); }	

	__int64 nLostExp = 0;

	// 대련 중에 범위 스킬 등으로 플레이어가 죽은 직후 소환수가 같이 죽으면
	// 주인의 CompeteID는 0이지만 대련 중의 사망이었으므로 페널티 안 받게 주인이 대련 사망 상태인지도 체크해야 함
	if( ( !pMaster->IsInBattleArena() && !pMaster->IsInDeathmatch() && !pMaster->IsInBattleField() && !pMaster->IsInStartedCompete( true ) && !( pMaster->IsDead() && pMaster->IsCompeteDead() ) ) || !pKiller )
	{
		if( pKiller && pKiller != pMaster )
		{
			pMaster->ProcImmoralPoint( pKiller, 0, false );
		}

		if( GetLevel() > 5 && pFrom->IsMonster() && !pMaster->IsInSiegeOrRaidDungeon() && !pMaster->IsInBattleField() && decreaseEXPOnDead )
		{
			nLostExp = GetDeadEXPPenalty();

			SetEXP( GetEXP() - nLostExp );
		}

		// 장착 중인 아이템의 에테리얼 내구도 감소 처리(던전 시즈/레이드는 예외)
		if( !pMaster->IsInSiegeOrRaidDungeon() )
		{
			// 몬스터에게 죽은 경우나 주인에 의한 자살일 때 장착 중인 아이템의 에테리얼 내구도 감소 처리
			if( !pKiller || pKiller == pMaster )
			{
				bool bNeedToCalculateStat = false;

				for( int i = 0 ; i < m_nMaxItemWear ; ++i )
				{
					StructItem * pItem = m_anWear[ i ];

					if( !pItem || !pItem->GetMaxEtherealDurability() || !pItem->GetCurrentEtherealDurability() )
						continue;

					// 최대 내구도의 10% 감소(곱하기 상수화하면 소수자리 오차 발생하므로 나누기 정수 연산으로 적용함)
					int nDecrement = pItem->GetMaxEtherealDurability() / 10;

					pItem->AddCurrentEtherealDurability( nDecrement * -1 );
					SendItemMessage( pMaster, pItem );

					bNeedToCalculateStat |= ( pItem->GetCurrentEtherealDurability() == 0 );
				}

				if( bNeedToCalculateStat )
					CalculateStat();
			}

			// 몬스터에게 죽은 경우나 주인 이외의 플레이어에 의해 죽었을 때 강화 크리처의 내구도 감소
			GetParentCard()->SetInstanceFlagOff( ItemInstance::ITEM_FLAG_SUMMON_DURABILITY );
			if( !pKiller || pKiller != pMaster ) 
			{
				if( GetParentCard()->GetItemEnhance() != 0 )
				{
					GetParentCard()->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_SUMMON_DURABILITY );
					GetParentCard()->SetCurrentEtherealDurability( GetParentCard()->GetCurrentEtherealDurability() - 1 );
					SendItemMessage( this->GetMaster(), GetParentCard() );
				}
			}
		}
	}

	SetLastDecreasedEXP( nLostExp );

	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	LOG::Log11N4S( LM_SUMMON_DEATH, pMaster->GetAccountID(), pMaster->GetSID(), GetSID(), nKillerID, nLostExp, 0, 0, 0, GetX(), GetY(), GetEXP(), pMaster->GetAccountName(), LOG::STR_NTS, pMaster->GetName(), LOG::STR_NTS, GetName(), LOG::STR_NTS, szKillerType, LOG::STR_NTS );
}

void StructSummon::onJPChange()
{
	SendPropertyMessage( this->GetMaster(), GetHandle(), "jp", GetJobPoint() );
}

void StructSummon::onRegisterSkill( int skill_uid, int skill_id, int skill_level, bool is_new_skill )
{
	if( is_new_skill )
	{
		// 새로 배우는거면 insert
		DBQuery( new DB_InsertSkill( this, skill_uid, 0, 0/*JP*/, GetSummonSID(), skill_id, skill_level, 0 ) );
	}
	else
	{
		// 아니면 update
		DBQuery( new DB_UpdateSkill( this, skill_uid, skill_level, GetRemainCoolTime( skill_id ) ) );
	}

	::SendSkillMessage( GetMaster(), this, skill_id );
}

__int64 StructSummon::getJPAfterSkillReset()
{
	int nJP = m_nMaxReachedLevel;

	// 강화 크리처의 추가 잡포인트
	if( GetParentCard() )
		nJP += GameContent::GetCreatureEnhanceInfo( GetParentCard()->GetItemEnhance() )->jp_addition;

	// 오버 브리드 레벨 계산
	switch( GetTransformLevel() )
	{
	case SummonBase::EVOLVE_EVOLVE:
		{
			// 2차 진화시 오버브리드 JP 처리
			int nEvolveLevel = GetPrevJobLevel( 1 );
			nJP += ( nEvolveLevel > GameRule::GROWTH_SUMMON_EVOLVE_LEVEL ) ? nEvolveLevel - GameRule::GROWTH_SUMMON_EVOLVE_LEVEL : 0;
			// 1차 진화시 오버브리드 JP 처리
			nEvolveLevel = GetPrevJobLevel( 0 );
			nJP += ( nEvolveLevel > GameRule::NORMAL_SUMMON_EVOLVE_LEVEL ) ? nEvolveLevel - GameRule::NORMAL_SUMMON_EVOLVE_LEVEL : 0;
		}
		break;
	case SummonBase::EVOLVE_GROWTH:
		{
			// 1차 진화시 오버브리드 JP 처리
			int nEvolveLevel = GetPrevJobLevel( 0 );
			nJP += ( nEvolveLevel > GameRule::NORMAL_SUMMON_EVOLVE_LEVEL ) ? nEvolveLevel - GameRule::NORMAL_SUMMON_EVOLVE_LEVEL : 0;
			// 현재 오버브리드 진행 중일 경우 JP 처리
			nJP += ( m_nMaxReachedLevel > GameRule::GROWTH_SUMMON_EVOLVE_LEVEL ) ? m_nMaxReachedLevel - GameRule::GROWTH_SUMMON_EVOLVE_LEVEL : 0;
		}
		break;
	case SummonBase::EVOLVE_NORMAL:
		{
			// 현재 오버브리드 진행 중일 경우 JP 처리
			nJP += ( m_nMaxReachedLevel > GameRule::NORMAL_SUMMON_EVOLVE_LEVEL ) ? m_nMaxReachedLevel - GameRule::NORMAL_SUMMON_EVOLVE_LEVEL : 0;
		}
		break;
	default:
		break;
	}

	return nJP;
}

int	StructSummon::IsLearnableSkill( int nSkillID, int nSkillLevel, int *nSkillTreeID )
{
	int nResult = RESULT_ACCESS_DENIED;

	// 우선 전 진화 단계에서 배울 수 있는 스킬 트리로 한번 검색해주고.
	for( int i = 0 ; i < 2 && ( nResult == RESULT_ACCESS_DENIED || nResult == RESULT_LIMIT_MAX || nResult == RESULT_NOT_ENOUGH_JOB_LEVEL ) ; ++i )
	{
		int nSummonID = GetPrevJobId( i );
		if( !nSummonID )
			continue;

		const SummonBase *pBase = GameContent::GetSummonInfo( nSummonID );
		if( !pBase )
			continue;
		int nLevel = GetPrevJobLevel( i );

		for( int j = 0 ; j < 5 ; ++j )
		{
			if( !pBase->skill_tree_id[ j ] )
				continue;

			nResult = GameContent::isLearnableSkill( this, nSkillID, nSkillLevel, pBase->skill_tree_id[ j ], nLevel );
			if( nResult == RESULT_SUCCESS )
			{
				*nSkillTreeID = pBase->skill_tree_id[ j ];
				break;
			}
		}
	}

	// 전 진화단계에서 못 찾았다면 현재 소환수 코드로 한번 더 검색 (단 ERROR 조건이 밑 3개가 아니라면 검사할 필요가 없음.)
	if( nResult == RESULT_ACCESS_DENIED || nResult == RESULT_LIMIT_MAX || nResult == RESULT_NOT_ENOUGH_JOB_LEVEL )
	{
		const SummonBase *pBase = GameContent::GetSummonInfo( GetJobId() );
		if( !pBase )
			return nResult;

		int nLevel = GetJobLevel();

		for( int i = 0 ; i < 5 ; ++i )
		{
			if( !pBase->skill_tree_id[ i ] )
				continue;

			nResult = GameContent::isLearnableSkill( this, nSkillID, nSkillLevel, pBase->skill_tree_id[ i ], nLevel );
			if( nResult == RESULT_SUCCESS )
			{
				*nSkillTreeID = pBase->skill_tree_id[ i ];
				break;
			}
		}
	}

	return nResult;
}

void StructSummon::onResetSkill( const int jobDepth )
{
	GetMaster()->Save();

	::SendSkillMessage( GetMaster(), this, -1, true );
}

void StructSummon::onRemoveSkill( StructSkill* removedSkill )
{
	DBQuery( new DB_DeleteSkill( this, removedSkill->GetSkillUID() ) );
}

void StructSummon::procMoveSpeedChangement()
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
	else
	{
		unsigned char nSpeed = GetRealMoveSpeed();

		// 라이딩 소환수여서 주인이 타고 있는 상태라면 라이딩 이속 적용
		StructPlayer * pMaster = GetMaster();
		if( pMaster && pMaster->IsRiding() && pMaster->GetRideObject() == this )
		{
			nSpeed = pMaster->GetRealRidingSpeed();
		}
		// 라이딩 상태가 아니라면 주인과 이속 싱크 상태(onMoveRequest에서 pMsg->speed_sync 처리 참조)에 대한 처리를 해야 하지만
		// 현재 이동 중인 속도가 싱크에 의한 이속인지 아닌지 알 수가 없으므로 여기서는 그냥 스텟상 고유 이속으로 이동되도록 함
		// (이 위치는 이동 중에 버프가 걸리거나 풀리는 경우만이므로 그런 이속 증폭의 혜택을 못 받는 걸로 무마!)

		if( GetSpeed() != nSpeed )
		{
			const std::vector< ArMoveVector::MOVE_INFO > & vMoveVector( GetTargetPosList() );
			std::vector< ArPosition > vMovePos;

			for( std::vector< ArMoveVector::MOVE_INFO >::const_iterator it = vMoveVector.begin() ; it != vMoveVector.end() ; ++it )
				vMovePos.push_back( (*it).end );

			ArcadiaServer::Instance().SetMultipleMove( this, pos, vMovePos, nSpeed, true, GetArTime() );
		}
	}
}

void StructSummon::onAfterRemoveState( StructState & state, const bool bByDead )
{
	// 부활에 의한 지속효과 복구 기능을 구현하려면 bByDead가 true일 경우에 만료 처리를 하지 않고 DB에 저장하도록 해야한다.
	// StructPlayer의 onAfterRemoveState 참조
	state.Expire( this );

	SetNeedToUpdateState();

	StructCreature::onAfterRemoveState( state );
}

void StructSummon::onUpdateState( StructState & state, bool bIsExpire )
{
	if( GetMaster() && GetMaster()->IsInWorld() )
	{
		SendStateMessage( GetMaster(), GetHandle(), &state, bIsExpire );
	}

	StructCreature::onUpdateState( state, bIsExpire );

	if( state.GetBaseEffectID() && this->GetPriority() < ArSchedulerObject::UPDATE_PRIORITY_HIGH )
	{
		ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGH );
	}
}

void StructSummon::onChangeProperty( const std::string & strKey, const char *data )
{
	const char *szKey = strKey.c_str();

	if( !_stricmp( szKey, "lv" ) || !_stricmp( szKey, "level" ) )
	{
		// m_nLevel은 이미 증가된 상태이므로 잡 포인트 획득 및 기타 처리가
		// 정상적으로 onExpChange에서 이루어지지 않게 됨
		// 명령어에 의해서만 프로퍼티가 변경되므로 그냥 1에서 업한 것처럼 처리
		m_nLevel = 0;
		SetEXP( ( atoi(data) > 1 ) ? GameContent::GetNeedSummonExp( atoi(data) - 1 ) : 0 );

		return;
	}
	else if( !_stricmp( szKey, "exp" ) )
	{
		onExpChange();

		return;
	}
	else if( !_stricmp( szKey, "hp" ) )
	{
		BroadcastHPMPMsg( this, atoi( data ), 0 );

		return;
	}
	else if( !_stricmp( szKey, "mp" ) )
	{
		BroadcastHPMPMsg( this, 0, atoi( data ) );

		return;
	}

	SendPropertyMessage( GetMaster(), GetHandle(), szKey, GetAsString( szKey ).c_str() );
}

void StructSummon::DBQuery( GameDBManager::DBProc *pWork )
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}

void StructSummon::onEndQuery()
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}

bool StructSummon::IsDeleteable()
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	// 스케쥴러에 등록되어 있으면 삭제 불가 && 다른 쓰레드에서 참조중이면 삭제 불가
	if( !GameObject::IsDeleteable() ) return false;

	// DB 쿼리 미결된게 있다면 삭제 불가
	if( !m_lQueryList.empty() ) return false;

	return true;
}

bool StructSummon::IsEnemy( StructCreature* pTarget, bool bIncludeHiding )
{
	return GetMaster()->IsEnemy( pTarget, bIncludeHiding );
}

bool StructSummon::IsVisible( StructCreature *pTarget )
{
	StructPlayer * pMaster = GetMaster();
	return ( pMaster ) ? pMaster->IsVisible( pTarget ) : StructCreature::IsVisible( pTarget );
}

bool StructSummon::IsSkillCastable()
{
	if( IsRidable() && GetMaster() && GetMaster()->GetRideObject() == this ) return false;

	return StructCreature::IsSkillCastable();
}

bool StructSummon::IsMagicCastable()
{
	if( IsRidable() && GetMaster() && GetMaster()->GetRideObject() == this ) return false;

	return StructCreature::IsMagicCastable();
}

ItemBase::ItemWearType StructSummon::GetAbsoluteWearPos( ItemBase::ItemWearType & pos )
{
	return pos;
	if( pos < ItemBase::MAX_ITEM_WEAR && pos >= 0 )
	{
		for( int i = 0; i < ItemBase::MAX_ITEM_WEAR; ++i )
		{
			if( !m_anWear[i] ) continue;

			ItemBase::ItemWearType type = m_anWear[i]->GetWearType();

			if( type == pos )
			{
				return static_cast< ItemBase::ItemWearType >( i );
			}

			if( type == ItemBase::WEAR_TWOFINGER_RING && pos == ItemBase::WEAR_RING )
			{
				return static_cast< ItemBase::ItemWearType >( i );
			}

			if( type == ItemBase::WEAR_TWOHAND && pos == ItemBase::WEAR_WEAPON )
			{
				return static_cast< ItemBase::ItemWearType >( i );
			}

			if( type == ItemBase::WEAR_SUMMON_ONLY && i == pos )
			{
				return static_cast< ItemBase::ItemWearType >( i );
			}
		}
	}

	return static_cast< ItemBase::ItemWearType >( -1 );
}

struct StructItem* StructSummon::GetWearedItem( ItemBase::ItemWearType idx ) const
{
	if( idx < ItemBase::MAX_ITEM_WEAR && idx >= 0 )
	{
		for( int i = 0; i < ItemBase::MAX_ITEM_WEAR; ++i )
		{
			if( !m_anWear[i] ) continue;

			ItemBase::ItemWearType type = m_anWear[i]->GetWearType();

			if( type == idx )
			{
				return m_anWear[i];
			}

			if( type == ItemBase::WEAR_TWOFINGER_RING && idx == ItemBase::WEAR_RING )
			{
				return m_anWear[i];
			}

			if( type == ItemBase::WEAR_TWOHAND && idx == ItemBase::WEAR_WEAPON )
			{
				return m_anWear[i];
			}

			if( type == ItemBase::WEAR_SUMMON_ONLY && i == idx )
			{
				return m_anWear[i];
			}
		}
	}

	return NULL;
}

void StructSummon::ProcEtherealDurabilityConsumption( const bool bIsAttacker, const DamageType eDamageType, const int nDamage )
{
	// 실제 소모량 = (기본 소모량) * (소모율) * (상황에 따른 소모율)

	// 기본 소모량
	const int nBaseConsumption = GameRule::GetEtherealDurabilityBaseConsumption( bIsAttacker, eDamageType );

	// 기본 소모량이 0이면 소모율과 상황에 다른 소모율이 몇이어도 소모되지 않음
	if( !nBaseConsumption )
		return;

	// 소모율(계산값)
	c_fixed10 fConsumeRate( GameRule::GetEtherealDurabilityConsumeRate( GetLevel(), GetJobId(), bIsAttacker, nDamage ) );

	// 상황에 따른 보정치(기본값 100%)
	c_fixed10 fEnvironmentalConsumeRate( 1 );

	StructPlayer * pMaster = GetMaster();
	// 상태에 따른 소모율 계산(소환수 자신은 관계 없고 주인의 상태를 따름)
	if( pMaster && pMaster->IsInWorld() )
	{
		// 대련 상대끼리나 데스매치 존, 배틀아레나 안에서는 5%(대련 상대가 아닌 대상에게 피격당할 경우 대련 상태가 풀리므로 별도 체크 안 함)
		// 본래대로라면 대련이 시작된 이후, 즉 IsInStartedCompete() 함수로 체크해야 하나 매 공격마다 체크하기엔 부하가 있으므로 일단 예외로 둠
		if( pMaster->GetCompeteID() || pMaster->IsInDeathmatch() || pMaster->IsInBattleArena() )
			fEnvironmentalConsumeRate = GameRule::ETHEREAL_DURABILITY_ENVIRONMENTAL_CONSUME_RATE_ON_PVP;
		// PK On 상태면 200%
		else if( pMaster->IsPKOn() || pMaster->IsPKOffing() )
			fEnvironmentalConsumeRate = GameRule::ETHEREAL_DURABILITY_ENVIRONMENTAL_CONSUME_RATE_ON_PK;
	}

	// 장착된 아이템에 에테리얼 내구도 감소 적용
	bool bNeedToCalculateStat = false;
	for( int i = 0 ; i < m_nMaxItemWear ; ++i )
	{
		static __declspec( thread ) StructItem * pItemForDebugging;
		StructItem * pItem = m_anWear[ i ];

		pItemForDebugging = pItem;

		// 아이템이 없거나 공격 시인데 무기가 아니거나 피격 시인데 무기면 패스
		if( !pItem || bIsAttacker != pItem->IsWeapon() )
			continue;

		if( !pItem->ProcEtherealDurabilityConsumption( nBaseConsumption, fConsumeRate, fEnvironmentalConsumeRate ) )
			continue;

		if( pMaster && pMaster->IsInWorld() )
			SendItemMessage( pMaster, pItem );

		// 새로 파괴된 아이템이 있으면 스텟 재 계산
		if( !pItem->GetCurrentEtherealDurability() )
		{
			StructPlayer *pPlayer = GetMaster();
			if( pPlayer )
			{
				LOG::Log11N4S( LM_ITEM_ETHEREAL_EXHAUST, 
					pPlayer->GetAccountID(), pPlayer->GetSID(), // N1, N2
					pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), // N3, N4
					GetSID(), 0, 0, pItem->GetMaxEtherealDurability(), // N5, N6, N7, N8
					GetX(), GetY(), pItem->GetItemUID(), // N9, N10, N11
					pPlayer->GetAccountName(), LOG::STR_NTS,
					pPlayer->GetName(), LOG::STR_NTS,
					GetName(), LOG::STR_NTS,
					bIsAttacker?"ATTACKING":"BEING_ATTACKED", LOG::STR_NTS );
			}
			bNeedToCalculateStat = true;
		}
	}

	// 스텟 재계산이 필요할 경우 재계산
	if( bNeedToCalculateStat )
		CalculateStat();
}

unsigned short StructSummon::putonItem( ItemBase::ItemWearType pos , struct StructItem * pItem )
{
	StructPlayer *pMaster = GetMaster();

	if( !pMaster || !pMaster->IsInWorld() || !pMaster->IsLoginComplete() || !pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_CARD ) )
	{
		return RESULT_ACCESS_DENIED;
	}

	unsigned short nRet = StructCreature::putonItem( pos, pItem );
	if( nRet != RESULT_SUCCESS )
		return nRet;

	// floyd 2008. 7..3
	// http://bug.nflavor.com/view.php?id=3380 관련 문제를 수정하다가 StructCreate::putonItem에서 옮겨옴
	pItem->SetOwnSummonInfo( GetHandle(), GetSID() );

	// 아이템 착용에 의한 스크립트 실행
	if( !pItem->GetItemBase().strScript.empty() )
	{
		std::string strScript( pItem->GetItemBase().strScript );

		const char * pszProcScript = "on_equip_item";
		// 스크립트 내용에 on_equip_item이 있을 경우 파라미터 추가
		if( strstr( strScript.c_str(), pszProcScript ) )
		{
			// on_equip_item( 아이템 코드, 착용자 타입, 착용자 핸들, [소환수인 경우에만]Summon 코드값, [소환수인 경우에만]주인 핸들, 착용 위치 )
			std::string strOnEquipItem;
			XStringUtil::Format( strOnEquipItem, "on_equip_item( %d, %d, %u, %d, %u, %d )",
				pItem->GetItemCode(), StructItem::TARGET_TYPE_SUMMON, GetHandle(), GetSummonCode(), ( GetMaster() ) ? GetMaster()->GetHandle() : 0, pos );

			XStringUtil::Replace( strScript, pszProcScript, strlen( pszProcScript ), strOnEquipItem.c_str(), strOnEquipItem.length() );
		}

		// 일부 스크립트 함수에 의한 기능에서 대화 중인 NPC 핸들 값에 의해 제어되는 부분이 있으므로
		// 아이템에 의한 스크립트 실행 동안은 대화 중인 NPC 핸들을 0으로 설정함
		AR_HANDLE nContactNPCHandle = pMaster->GetContactNPCHandle();
		pMaster->SetContactNPCHandle( 0 );

		LUA()->RunString( strScript.c_str() );

		// 대화 중인 NPC 핸들 값을 원래대로 되돌림
		pMaster->SetContactNPCHandle( nContactNPCHandle );
	}

	// 착용된 아이템은 더 이상 무게에 포함되지 않으므로 뺀다
	if( m_anWear[ pos ] == pItem )
		pMaster->GetInventory()->AddWeightModifier( -pItem->GetWeight() );

	pMaster->UpdateWeightWithInventory();

	pMaster->UpdateQuestStatusByItemUpgrade();

	return RESULT_SUCCESS;
}

unsigned short StructSummon::putoffItem( ItemBase::ItemWearType pos )
{
	StructPlayer *pMaster = GetMaster();

	if( !pMaster || !pMaster->IsInWorld() || !pMaster->IsLoginComplete() )
	{
		return RESULT_ACCESS_DENIED;
	}

	if( !m_anWear[ pos ] )
		return RESULT_NOT_EXIST;

	StructItem *pItem = m_anWear[ pos ];

	// floyd 2008. 7..3
	// http://bug.nflavor.com/view.php?id=3380
	// 크리쳐가 착용했던 장비의 소환수 정보를 지워주지 않아서 드물지만 복잡한 프로세스를 거치면 캐릭터가 장비하고 있는데도, 크리쳐가 입고 있는 것으로 인식하게 됨
	pItem->SetOwnSummonInfo( 0, 0 );
	unsigned short nRet = StructCreature::putoffItem( pos );

	// 착용되지 않은 아이템은 무게에 다시 포함되어야 함
	if( m_anWear[ pos ] == NULL )
		pMaster->GetInventory()->AddWeightModifier( pItem->GetWeight() );

	pMaster->UpdateWeightWithInventory();

	pMaster->UpdateQuestStatusByItemUpgrade();

	return nRet;
}

bool StructSummon::DoEvolution( int nTargetCode )
{
	//ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

	int prev_hp = GetHP();
	int prev_mp = GetMP();

	if( GetMaster()->IsRiding() && GetMaster()->GetRideObject() == this )
		return false;

	int nCurrentCode = m_pContentInfo->uid;
	int nCurrentLevel = GetLevel();

	if( nTargetCode == 0 )
	{
		if( m_pContentInfo->evolve_target != 0 )
			nTargetCode = m_pContentInfo->evolve_target;
		else
			return false;
	}

	SetSummonInfo( nTargetCode );

	CalculateStat();

	GetMaster()->Save();

	TS_SC_SUMMON_EVOLUTION msg;

	msg.summon_handle = GetHandle();			// 소환수 핸들
	msg.card_handle = GetParentCard()->GetHandle();
	s_strcpy( msg.name, _countof( msg.name ), GetName() );	// 소환수 이름
	msg.code = GetSummonCode();

	if( IsInWorld() )
	{
		ArcadiaServer::Instance().Broadcast( GetRX(), GetRY(), GetLayer(), &msg );
	}
	else if( GetMaster() )
	{
		PendMessage( GetMaster(), &msg );
	}

	if( !IsVisibleRegion( GetRX(), GetRY(), GetMaster()->GetRX(), GetMaster()->GetRY() ) )
	{
		PendMessage( GetMaster(), &msg );
	}

	SendStatInfo( GetMaster(), this );	// 소환수 스탯정보 얻어서
										// 보내주자

	SendHPMPMsg( GetMaster(), this, GetHP() - prev_hp, GetMP() - prev_mp );	// HP/MP
	SendLevelMsg( GetMaster(), this );	// 레벨
	SendExpMsg( GetMaster(), this );	// 경치

	SendSPMsg( GetMaster(), this );

	__int64 nCardUID = 0;

	StructItem * pItem = GetParentCard();
	if( pItem )
	{
		nCardUID = pItem->GetItemUID();

		int depth = GetTransformLevel();
		int i;
		for( i = 0; i < depth - 1; ++i )
		{
			pItem->SetSocketCode( i + 1, GetPrevJobLevel( i ) );
		}
		pItem->SetSocketCode( i + 1, GetLevel() );
		pItem->SetSummonCode( GetSummonCode() );
		pItem->DBQuery( new DB_UpdateItem( pItem ) );

		if( GetMaster() )
		{
			SendItemMessage( GetMaster(), GetParentCard() );
		}
	}

	LOG::Log11N4S( LM_SUMMON_EVOLUTION,	GetMaster()->GetAccountID(), 
		GetMaster()->GetSID(), GetSID(), 
		nCurrentCode, nTargetCode, 
		nCurrentLevel, GetPrevJobId( 0 ), 
		GetPrevJobLevel( 0 ), GetPrevJobId( 1 ),
		GetPrevJobLevel( 1 ), nCardUID, 
		GetMaster()->GetAccountName(), LOG::STR_NTS, 
		GetMaster()->GetName(), LOG::STR_NTS, 
		GetName(), LOG::STR_NTS, 
		"", 0 );

	return true;
}

bool StructSummon::Reborn()
{
	bool bSuccess = true;

	if( GetMaster()->IsRiding() && GetMaster()->GetRideObject() == this )
		return false;

	// 스킬 리셋부터 하고
	bSuccess = ResetSkill( SRM_SUMMON_ENHANCE );
	if( !bSuccess ) return bSuccess;

	int nTargetCode = m_nPrevJobId[0];
	if( !nTargetCode )
		nTargetCode = m_pContentInfo->uid;

	m_nEXP = 0;
	m_nTransformLevel = 1;
	m_nJobLevel = m_nLevel = m_nMaxReachedLevel = 1;
	m_nPrevJobId[0] = m_nPrevJobId[1] = m_nPrevJobId[2] = 0;
	m_nPrevJobLevel[0] = m_nPrevJobLevel[1] = m_nPrevJobLevel[2] = 0;
	m_nLastDecreasedEXP = 0;

	if( GetParentCard() )
	{
		GetParentCard()->SetInstanceFlagOff( ItemInstance::ITEM_FLAG_SUMMON_DURABILITY );
		for( int i = 1; i < ItemBase::MAX_SOCKET_NUMBER; ++i )
		{
			GetParentCard()->SetSocketCode( i, 0 );
		}
	}

	bSuccess = DoEvolution( nTargetCode );
	if( !bSuccess ) return bSuccess;

	// 크리쳐 강화의 경우 JP 보너스 추가가 있고 레벨이 1로 초기화되기 때문에 JP를 새로 계산해 반납해준다. 
	// 나중에 새로운 JP 계산 시스템이나 한두개의 스킬만 JP를 반납하는 등의 시스템이 생기면 
	// 아래 getJPAfterSkillReset 코드를 통해 고려해줘야한다.
	SetJP( getJPAfterSkillReset() );
	SetTalentPoint( getTPAfterSkillReset() );

	return bSuccess;
}

bool StructSummon::IsAlly( StructCreature *pTarget )
{
	return GetMaster()->IsAlly( pTarget );
}

int StructSummon::onDamage( StructCreature * pFrom, Elemental::Type elementalType, DamageType damageType, int nDamage, bool bCritical, bool bDouble)
{
	StructPlayer * pMaster = GetMaster();

	if( ( pMaster->IsRiding() && pMaster->GetRideObject() == this )
		&& ( damageType == DT_NORMAL_PHYSICAL_DAMAGE || damageType == DT_NORMAL_MAGICAL_DAMAGE || damageType == DT_NORMAL_PHYSICAL_LEFT_HAND_DAMAGE || damageType == DT_NORMAL_PHYSICAL_SKILL_DAMAGE ) )
	{
		if( ( bCritical && XRandom() % 100 <= pMaster->GetUnMountProbabilityOnCriticalDamage() ) || XRandom() % 100 <= pMaster->GetUnMountProbabilityOnDamage() )
		{
			pMaster->UnMount( StructPlayer::UNMOUNT_FALL, pFrom );
		}
	}

	if( m_fSoulConnectionRatio && pMaster )
	{
		int nPassDamage = std::min( int( nDamage * m_fSoulConnectionRatio), pMaster->GetHP() );
		if( nPassDamage )
		{
			pMaster->damage( pFrom, nPassDamage );
			nDamage -= nPassDamage;

			// Master를 중심으로 VisibleRegion에 Lock이 걸려있다는 보장은 없으나 일단 대충;;
			// 이 부분이 문제가 될 경우 주인에게 락을 따로 걸고 데미지를 주기 위한 방법을
			// 강구해야 함
			BroadcastHPMPMsg( pMaster, 0 - nPassDamage, 0 );
		}
	}

	AR_TIME t = GetArTime();
	ArPosition pos = GetCurrentPosition( t );

	if( pMaster && pMaster->IsInStartedCompete( true ) )
	{
		StructPlayer * pAttackPlayer = NULL;
		if( pFrom->IsPlayer() )
			pAttackPlayer = static_cast< StructPlayer * >( pFrom );
		else if( pFrom->IsSummon() )
			pAttackPlayer = static_cast< StructSummon * >( pFrom )->GetMaster();

		if( !pAttackPlayer || pMaster->GetCompeteID() != pAttackPlayer->GetCompeteID() )
		{
			CompeteManager::Instance().RetireCompeteWithPlayer( pMaster, COMPETE_END_BY_INTERRUPT );
		}
	}

	return StructCreature::onDamage( pFrom, elementalType, damageType, nDamage, bCritical, bDouble );

}

int StructSummon::GetCreatureGroup() const
{
	if( m_nChangingGroup != CREATURE_NONE )
		return m_nChangingGroup;

	return m_pContentInfo->type;
}

void StructSummon::onCantAttack( AR_HANDLE target, AR_TIME t )
{
	if( m_nLastCantAttackTime + 100 < t )
	{
		m_nLastCantAttackTime = t;

		SendCantAttackMsg( GetMaster(), GetHandle(), target, RESULT_TOO_FAR );
	}
}

float StructSummon::GetScale() const
{
	return m_pContentInfo->scale;
}

float StructSummon::GetSize() const
{
	return m_pContentInfo->size;
}

int StructSummon::GetTransformLevel() const
{
	return m_pContentInfo->evolve_type;
}

int StructSummon::GetEvolveTarget() const
{
	return m_pContentInfo->evolve_target;
}

unsigned char StructSummon::GetRidingMoveSpeed()
{
	// 튼튼한 발굽 등의 라이딩 속도 증가 아이템이 크리처 라이딩에도 적용 될 수 있게 변경 (2013-12-20)
	return ( m_pContentInfo->riding_speed + GetMaster()->GetRideSpeedModifier() ) / 7.0 * m_fRideSpeedMod;
}

char StructSummon::GetRate() const
{
	return m_pContentInfo->rate;
}