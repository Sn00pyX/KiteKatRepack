
#include <map>

#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XRandom.h>
#include <logging/FileLog.h>
#include <toolkit/XStringUtil.h>

#include "LogClient/LogClient.h"

#include "StructMonster.h"
#include "Constant.h"
#include "GameContent.h"
#include "SendMessage.h"
#include "StructItem.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructRoamer.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "Extern.h"
#include "GameRule.h"
#include "GameProc.h"
#include "GameMessage.h"
#include "StructEventItemManager.h"
#include "DungeonManager.h"
#include "InstanceDungeonManager.h"
#include "StructSkill.h"
#include "GameAllocator.h"
#include "LuaVM.h"
#include "ThreadPlayerHelper.h"


#ifdef _MONSTER_DEBUG
#define _monster_debug_output _oprint
#else
#define _monster_debug_output 
#endif

XCriticalSection	m_csHate( "m_csHate" );

struct takePriority
{
	StructItem::ITEM_PICKUP_ORDER ItemPickupOrder;
};


void StructMonster::BindProperty()
{
	StructMonster temp( 0, 0 );
	temp.Bind( "hp", &temp.m_nHP );
	temp.Bind( "mp", &temp.m_nMP );
	temp.Bind( "max_hp", &temp.m_nMaxHP );
	temp.Bind( "max_mp", &temp.m_nMaxMP );
	temp.Bind( "level", &temp.m_nLevel );
	temp.Bind( "lv", &temp.m_nLevel );
	temp.Bind( "str", const_cast< __int64 * >( &temp.m_Stat.strength.get() ) );
	temp.Bind( "agi", const_cast< __int64 * >( &temp.m_Stat.agility.get() ) );
	temp.Bind( "dex", const_cast< __int64 * >( &temp.m_Stat.dexterity.get() ) );
	temp.Bind( "int", const_cast< __int64 * >( &temp.m_Stat.intelligence.get() ) );
	temp.Bind( "luck", const_cast< __int64 * >( &temp.m_Stat.luck.get() ) );
	temp.Bind( "vital", const_cast< __int64 * >( &temp.m_Stat.vital.get() ) );
	temp.Bind( "mental", const_cast< __int64 * >( &temp.m_Stat.mentality.get() ) );
	temp.Bind( "x", &temp.mv.x );
	temp.Bind( "y", &temp.mv.y );
	temp.Bind( "layer", &temp.layer );
	temp.Bind( "speed", const_cast< __int64 * >( &temp.m_Attribute.fMoveSpeed.get() ) );
	temp.Bind( "critical", const_cast< __int64 * >( &temp.m_Attribute.fCritical.get() ) );
	temp.Bind( "critical_power", const_cast< __int64 * >( &temp.m_Attribute.fCriticalPower.get() ) );
	temp.Bind( "attack_point", const_cast< __int64 * >( &temp.m_Attribute.fAttackPointRight.get() ) );
	temp.Bind( "attack_right",		const_cast< __int64 * >( &temp.m_Attribute.fAttackPointRight.get() ) );
	temp.Bind( "attack_left",		const_cast< __int64 * >( &temp.m_Attribute.fAttackPointLeft.get() ) );
	temp.Bind( "defence", const_cast< __int64 * >( &temp.m_Attribute.fDefence.get() ) );
	temp.Bind( "block_defence",		const_cast< __int64 * >( &temp.m_Attribute.fBlockDefence.get() ) );
	temp.Bind( "magic_pt", const_cast< __int64 * >( &temp.m_Attribute.fMagicPoint.get() ) );
	temp.Bind( "magic_def", const_cast< __int64 * >( &temp.m_Attribute.fMagicDefence.get() ) );
	temp.Bind( "accuracy", const_cast< __int64 * >( &temp.m_Attribute.fAccuracyRight.get() ) );
	temp.Bind( "accuracy_left", const_cast< __int64 * >( &temp.m_Attribute.fAccuracyLeft.get() ) );
	temp.Bind( "avoid", const_cast< __int64 * >( &temp.m_Attribute.fAvoid.get() ) );
	temp.Bind( "magic_avoid", const_cast< __int64 * >( &temp.m_Attribute.fMagicAvoid.get() ) );
	temp.Bind( "block_chance", const_cast< __int64 * >( &temp.m_Attribute.fBlockChance.get() ) );
	temp.Bind( "speed", const_cast< __int64 * >( &temp.m_Attribute.fMoveSpeed.get() ) );
	temp.Bind( "attack_range", const_cast< __int64 * >( &temp.m_Attribute.fAttackRange.get() ) );
	temp.Bind( "attack_speed", const_cast< __int64 * >( &temp.m_Attribute.fAttackSpeed.get() ) );
	temp.Bind( "perfect_block", const_cast< __int64 * >( &temp.m_Attribute.fPerfectBlock.get() ) );
	temp.Bind( "physical_ignore", const_cast< __int64 * >( &temp.m_Attribute.fPhysicalDefIgnore.get() ) );
	temp.Bind( "physical_ignore_ratio", const_cast< __int64 * >( &temp.m_Attribute.fPhysicalDefIgnoreRatio.get() ) );
	temp.Bind( "magical_ignore", const_cast< __int64 * >( &temp.m_Attribute.fMagicalDefIgnore.get() ) );
	temp.Bind( "magical_ignore_ratio", const_cast< __int64 * >( &temp.m_Attribute.fMagicalDefIgnoreRatio.get() ) );
	temp.Bind( "physical_penetration", const_cast< __int64 * >( &temp.m_Attribute.fPhysicalPenetration.get() ) );
	temp.Bind( "physical_penetration_ratio", const_cast< __int64 * >( &temp.m_Attribute.fPhysicalPenetrationRatio.get() ) );
	temp.Bind( "magical_penetration", const_cast< __int64 * >( &temp.m_Attribute.fMagicalPenetration.get() ) );
	temp.Bind( "magical_penetration_ratio", const_cast< __int64 * >( &temp.m_Attribute.fMagicalPenetrationRatio.get() ) );
}


StructMonster* StructMonster::AllocMonster( unsigned idx, unsigned char difficulty )
{
	StructMonster* p;

	struct _myIntializer : GameAllocateFunctor
	{
		_myIntializer( int _idx, unsigned char _difficulty ) : m_nIdx( _idx ), m_nDifficulty( _difficulty ) {}
		
		virtual void operator()( void * pObj, AR_HANDLE handle )
		{
			new (pObj) StructMonster( handle, m_nIdx, m_nDifficulty );
		}

		unsigned m_nIdx;
		unsigned char m_nDifficulty;
	};

	AR_HANDLE handle = allocMonsterStruct( &p, _myIntializer( idx, difficulty ) );

	// new (p) StructMonster( handle, idx );

	if( !p->GetMonsterId() )
	{
		freeMonsterStruct( p );
		FILELOG( "Invalid Monster Id : %d", idx );
		return NULL;
	}

	for( std::vector< MonsterBase::MONSTER_SKILL_INFO >::const_iterator it = p->m_pContentInfo->skill_info_list->begin() ; it != p->m_pContentInfo->skill_info_list->end() ; ++it )
	{
		if( (*it).skill_id )
		{
			p->SetSkill( StructSkill::SKILL_UID_MONSTER_SKILL, (*it).skill_id, (*it).skill_lv, 0 );
		}
	}

	p->CalculateStat();

	p->m_nHP = p->m_nMaxHP;
	p->m_nMP = p->m_nMaxMP;

	return p;
}
void StructMonster::FreeMonster( StructMonster* p )
{
	// _oprint( "DEL MONSTER : %08X\n", p );

	// { 임시
	/*
	extern std::vector< StructNPC* >		g_vNPC;
	std::vector< StructNPC* >::iterator it;
	for( it = g_vNPC.begin(); it != g_vNPC.end(); it++ )
	{
		if( *it != p ) continue;
		g_vNPC.erase( it );
		break;
	}
	*/
	// }

	prepareFreeMonsterStruct( p );

	p->StructMonster::~StructMonster();

	freeMonsterStruct( p );
}

StructMonster::StructMonster( AR_HANDLE handle, unsigned idx, unsigned char difficulty )
{
	m_hHandle = handle;

	m_nRegenTime = GetArTime();
	m_bRespawnByScript = false;

	m_nArObjectType = ArObject::MOVABLE_OBJECT;
	//SetTag( static_cast< GameObject* >( this ) );

	m_nStatus = STATUS_NORMAL;
	
	m_bComeBackHome = false;
	if ( idx != 0 )
	{
		// idx == 0은 임시 객체임을 의미함 
		SetMonsterInfo( idx );
		m_vTriggerFlag.reserve( m_pContentInfo->trigger_list->size() );
		for( size_t nCount = 0 ; nCount < m_pContentInfo->trigger_list->size() ; ++nCount )
			m_vTriggerFlag.push_back( 0 );
	}

	m_nLastTrackTime = 0;
	m_nTotalDamage = 0;
	m_nStatus = 0;

	m_nGenerateCode = BY_NONE;

	m_pDeleteHandler = NULL;
	m_nLifeTime = 0;
	m_nDungeonId = 0;
	m_bIsDungeonRaidMonster = false;
	m_bIsDungeonOwnerGuardian = false;
	m_bIsDungeonSiegerGuardian = false;

	m_nInstanceRespawnID = 0;

	m_hFirstAttacker	= 0;
	m_nFirstAttackTime	= 0;
	m_nHateCheckTime	= 0;

	m_vHateModifierByState.clear();
	m_bNeedToFindEnemy = false;

	m_hTamer			= 0;
	m_nTamedTime		= INFINITE_TIME;

	m_RespawnX = m_RespawnY = 0;
	m_nMaxHate = 0;

	m_bTamedSuccess = false;
	m_nTamingSkillLevel = 0;
	m_pTamingSkill = NULL;

	m_nLastEnemyDistance = 0;
	m_nLastHateUpdateTime = 0;

	// 임시 : 랜덤하게 다른방향 보게 하자.. -_-;
	mv.face = (float)XRandom() / 100;	

	m_bIsWandering = true;

#ifndef _USE_NEW_ROAMING_ONLY
	m_pWayPointInfo = NULL;
	m_nWayPointIdx = 0;
#endif
	m_pRoamer = NULL;

	m_bContinueAttack = false;

	m_nDifficulty = difficulty;
}

StructMonster::~StructMonster()
{
	_monster_debug_output( "DELETE : %I64d\n", GetHandle() );
}

bool StructMonster::ProcDelete()
{
	FreeMonster( this );

	return true;
}

bool StructMonster::SetMonsterInfo( unsigned idx )
{
	m_nLevel = GameContent::GetMonsterInfo( idx )->level;
	m_pContentInfo = GameContent::GetMonsterInfo( idx );
	m_Stat = GameContent::GetStatInfo( m_pContentInfo->stat_id );

	return true;
}

int StructMonster::GetMonsterId()
{
	return m_pContentInfo->uid;
}

const char*		StructMonster::GetName() const
{
	return GameContent::GetString( m_pContentInfo->name_id );
}

unsigned char StructMonster::GetRace() const
{
	return CREATURE_ETC;
}

float StructMonster::GetScale() const
{
	return m_pContentInfo->scale;
}

float StructMonster::GetSize() const
{
	return m_pContentInfo->size;
}

StructMonster::MONSTER_STATUS StructMonster::GetStatus() const
{
	return static_cast< MONSTER_STATUS >( m_nStatus );
}

void StructMonster::SetStatus( MONSTER_STATUS status )
{
	if( m_nStatus == STATUS_DEAD )
	{
		return;
	}

	if( status != m_nStatus && status != STATUS_DEAD && status != STATUS_NORMAL && m_nStatus == STATUS_NORMAL )
	{
		ResetTriggerCondition();
	}

	if( m_nStatus != status )
	{
		m_nStatus = status;

		BroadcastStatusMessage( this );
	}
	else
	{
		m_nStatus = status;
	}

}

bool StructMonster::IsFirstAttacker() const
{
	return ( m_pContentInfo->attack_type & MonsterBase::AT_FIRST_ATTACK );
}

bool StructMonster::IsGroupRevenger() const
{
	return ( m_pContentInfo->attack_type & MonsterBase::AT_RESPONSE_RACE );
}

bool StructMonster::IsCastRevenger() const
{
	return ( m_pContentInfo->attack_type & MonsterBase::AT_RESPONSE_CASTING );
}

bool StructMonster::IsBattleRevenger() const
{
	return ( m_pContentInfo->attack_type & MonsterBase::AT_RESPONSE_BATTLE );
}

int StructMonster::GetMonsterGroup() const
{
	return m_pContentInfo->monster_group;
}

bool StructMonster::IsGroupFirstAttacker() const
{
	return ( m_pContentInfo->attack_type & MonsterBase::AT_GROUP_FIRST_ATTACK );
}

bool StructMonster::IsTameable() const
{
	return !!m_pContentInfo->taming_code;
}

const c_fixed10 StructMonster::GetTamePercetage() const
{
	float fTamePercentage = 0;

	switch (GetMonsterCreatureTameItemCode())
	{
		case 540200:
			fTamePercentage = GameRule::fTamingSuccessRate2;
			break;
		case 540201:
			fTamePercentage = GameRule::fTamingSuccessRate3;
			break;
		case 540202:
			fTamePercentage = GameRule::fTamingSuccessRate4;
			break;
		case 540203:
			fTamePercentage = GameRule::fTamingSuccessRate5;
			break;
	}

	return fTamePercentage > m_pContentInfo->taming_percentage ? fTamePercentage : m_pContentInfo->taming_percentage;
}

float StructMonster::GetTameExpAdjust() const
{
	return m_pContentInfo->taming_exp_adjust;
}

int StructMonster::GetTameCode() const
{
	return m_pContentInfo->taming_code;
}


bool StructMonster::IsMonsterCreatureTame() const
{
	if( m_pContentInfo->creature_taming_code_group )
		return true;

	return false;
}

int StructMonster::GetMonsterCreatureTameGroupCode() const
{
	if( IsMonsterCreatureTame() )
	{
		return m_pContentInfo->creature_taming_code_group;
	}

	return 0;
}


int StructMonster::GetMonsterCreatureTameCode() const
{
	if( IsMonsterCreatureTame() )
	{
		return GameContent::GetMonsterCreatureTameCode( GetMonsterCreatureTameGroupCode() );
	}

	return 0;
}

int StructMonster::GetMonsterCreatureTameItemCode() const
{
	if( IsMonsterCreatureTame() )
	{
		// 무조건 하나의 그룹에 있는 몬스터들은 무조건 동일한 크리쳐 카드를 써야 한다.
		return static_cast< ItemBase::ItemCode >( GameContent::GetSummonInfo( GetMonsterCreatureTameCode() )->card_id );
	}

	return 0;
}

ItemBase::ItemCode StructMonster::GetTameItemCode() const
{
	if( !m_pContentInfo->taming_code && !m_pContentInfo->creature_taming_code_group ) 
		return 0;

	// taming_code와 creature_taming_code_group 가 전부 값이 존재하면 안됨. 하나는 0이어야 함
	if( m_pContentInfo->taming_code && m_pContentInfo->creature_taming_code_group ) 
		return 0;

	// 기존 크리쳐 카드
	if( m_pContentInfo->taming_code )
	{
		return static_cast< ItemBase::ItemCode >( GameContent::GetSummonInfo( m_pContentInfo->taming_code )->card_id );
	}
	
	// 모든 몬스터 크리쳐 카드 ( 소울 테이밍 카드 )
	if( m_pContentInfo->creature_taming_code_group )
	{
		return GetMonsterCreatureTameItemCode();
	}

	return 0;
}

int StructMonster::AddHate( AR_HANDLE handle, int pt, bool bBroadcast, bool bProcRoamingMonster )
{
	if( IsDead() || IsEnvironmentMonster() )
		return 0;

	AR_TIME t = GetArTime();

	StructCreature::iterator it = StructCreature::get( handle );
	if( !(*it) ) return 0;

	if( !IsEnemy( (*it) ) )
		return 0;

	int nHate = addHate( handle, pt )->nHate;

	// 동일 로밍 그룹 몬스터 헤이트 분배
	if( bProcRoamingMonster && GetRoamer() )
	{
		GetRoamer()->PendHateShare( GetHandle(), handle, m_fHateRatio * pt );
	}

	if( pt < 0 )
	{
		findNextEnemy();
	}
	// 동족 인식 방송을 해야 하는 놈들이라도 그룹 로밍 몹이면 동족 인식 처리 안 함
	else if( bBroadcast && IsGroupRevenger() && !GetRoamer() )
	{
		// 자. 동족 몹들아~ 반격하자아~
		std::vector< AR_HANDLE > vResult;
		ArcadiaServer::Instance().EnumMovableObject( GetPos(), GetLayer(), GetFirstAttackRange() * 0.8f, &vResult, false, true );

		for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
		{
			StructCreature* pCreature = static_cast< StructCreature* >( StructCreature::raw_get( *it ) );

			if( !pCreature )
				continue;

			if( pCreature->IsMonster() && pCreature != this )
			{
				if( static_cast< StructMonster * >( pCreature )->GetMonsterGroup() == GetMonsterGroup() )
				{
					static_cast< StructMonster * >( pCreature )->AddHate( handle, m_fHateRatio * pt, false );
				}
			}
		}
	}

	return nHate;
}

int StructMonster::RemoveHate( AR_HANDLE handle, int pt )
{
	_HATE_TAG * pHateTag = getHateTag( handle, GetArTime() );

	if( pHateTag )
	{
		if( pt < pHateTag->nHate )
		{
			pHateTag->nHate -= pt;

			if( handle == m_hEnemy && !IsDead() )
			{
				findNextEnemy();
			}

			return pHateTag->nHate;

		}
		else
		{
			removeFromHateList( handle );

			
			if( handle == m_hEnemy )
			{
				// 문제 생기나?
				m_hEnemy = 0;
				findNextEnemy();
			}

			return 0;
		}
	}

	return -1;
}

int	StructMonster::GetHate( AR_HANDLE handle )
{
	_HATE_TAG * pHateTag = getHateTag( handle );
	if( pHateTag ) return pHateTag->nHate;
	return 0;
}

_HATE_TAG * StructMonster::getHateTag( AR_HANDLE handle, AR_TIME t )
{
	THREAD_SYNCRONIZE( &m_csHate );

	_HATE_TAG * pHateTag = NULL;

	std::vector< _HATE_TAG >::reverse_iterator it;
	for( it = m_vHateList.rbegin(); it != m_vHateList.rend(); it++ )
	{
		if( (*it).uid == handle )
		{
			pHateTag = &(*it);

			if( t )
				pHateTag->nTime = t;
			break;
		}
	}

	return pHateTag;
}

_DAMAGE_TAG * StructMonster::getDamageTag( AR_HANDLE handle, AR_TIME t )
{
	_DAMAGE_TAG * pDamageTag = NULL;

	std::vector< _DAMAGE_TAG >::iterator it;
	for( it = m_vDamageList.begin(); it != m_vDamageList.end();  )
	{
		if( (*it).uid == handle )
		{
			pDamageTag = &(*it);

			if( t )
				pDamageTag->nTime = t;
			break;
		}

		// 5분이 지나도록 공방이 없었다면 damage list 에서 제거
		if( (*it).nTime + 30000 < t )
		{
			m_nTotalDamage -= (*it).nDamage;
			it = m_vDamageList.erase( it );
			continue;
		}

		it++;
	}

	return pDamageTag;
}

_HATE_TAG * StructMonster::addHate( AR_HANDLE handle, int nHate )
{
	_HATE_TAG * pHateTag = NULL;
	AR_TIME t = GetArTime();

	pHateTag = getHateTag( handle, t );

	if( !pHateTag )
	{
		THREAD_SYNCRONIZE( &m_csHate );

		m_vHateList.push_back( _HATE_TAG( t, handle, 0 ) );
		pHateTag = &m_vHateList.back();

		StructCreature::iterator it = StructCreature::get( handle );

		(*it)->AddToEnemyList( GetHandle() );
	}

	pHateTag->nHate += nHate;
	if( pHateTag->nHate < 0 )
		pHateTag->nHate = 0;

	pHateTag->nLastMaxHate = pHateTag->nHate;

	if( !pHateTag->bIsActive )
	{
		pHateTag->bIsActive = true;
		
		int nFrenzyLev = 0;
		AR_TIME nFrenzyTime = 0;

		if( pHateTag->nBadAttackCount == 1 )
		{
			nFrenzyLev = 1;
			nFrenzyTime = 300;
		}
		else
		{
			nFrenzyLev = 2;
			nFrenzyTime = 500;
		}

		AddState( StructState::FRENZY, GetHandle(), nFrenzyLev, t, t + nFrenzyTime );
	}


	if( !IsAttacking() )
	{
		m_nMaxHate = pHateTag->nHate;

		StructCreature::iterator it = StructCreature::get( handle );

		StructCreature * pTarget = (*it);

		if( pTarget )
		{
			//pTarget->OnUpdate();
			StartAttack( handle, false );
		}
	}
	else if( m_hEnemy != handle && pHateTag->nHate > m_nMaxHate )
	{
		m_nMaxHate = pHateTag->nHate;
		m_hEnemy = handle;
	}
	else if( m_hEnemy == handle )
	{
		m_nMaxHate = pHateTag->nHate;
	}

	return pHateTag;
}

_DAMAGE_TAG * StructMonster::addDamage( AR_HANDLE handle, int nDamage )
{
	_DAMAGE_TAG * pDamageTag = NULL;
	AR_TIME t = GetArTime();

	pDamageTag = getDamageTag( handle, t );

	if( !pDamageTag )
	{
		m_vDamageList.push_back( _DAMAGE_TAG( t, handle, 0 ) );
		pDamageTag = &m_vDamageList.back();
	}

	m_nTotalDamage += nDamage;
	pDamageTag->nDamage += nDamage;

	return pDamageTag;
}

void StructMonster::clearHateList()
{
	THREAD_SYNCRONIZE( &m_csHate );

	std::vector< _HATE_TAG >::iterator it;
	
	for( it = m_vHateList.begin(); it != m_vHateList.end(); it++ )
	{
		StructCreature::iterator itTarget = StructCreature::get( (*it).uid );

		if( *itTarget )
		{
			( *itTarget )->RemoveFromEnemyList( GetHandle() );
		}

	}

	m_vHateList.clear();
}

bool StructMonster::removeFromHateList( AR_HANDLE handle )
{
	THREAD_SYNCRONIZE( &m_csHate );

	bool bSuccess = false;
	std::vector< _HATE_TAG >::iterator it;
	
	for( it = m_vHateList.begin(); it != m_vHateList.end(); it++ )
	{
		if( (*it).uid == handle )
		{
			m_vHateList.erase( it );
			bSuccess = true;
			break;
		}
	}

	if( bSuccess )
	{
		StructCreature::iterator itTarget = StructCreature::get( handle );

		if( *itTarget )
		{
			( *itTarget )->RemoveFromEnemyList( GetHandle() );
		}
	}

	return bSuccess;

}

bool StructMonster::removeFromDamageList( AR_HANDLE handle )
{
	bool bSuccess = false;
	std::vector< _DAMAGE_TAG >::iterator it;
	
	for( it = m_vDamageList.begin(); it != m_vDamageList.end(); it++ )
	{
		if( (*it).uid == handle )
		{
			m_nTotalDamage -= (*it).nDamage;
			m_vDamageList.erase( it );
			bSuccess = true;
			break;
		}
	}

	return bSuccess;
}

int	StructMonster::onDamage( StructCreature * pFrom, Elemental::Type elementalType, DamageType damageType, int nDamage, bool bCritical, bool bDouble)
{
	nDamage = std::min( GetHP(), nDamage );

	if( !nDamage )
		return StructCreature::onDamage( pFrom, elementalType, damageType, nDamage, bCritical, bDouble );

	AR_TIME t = GetArTime();
	int nHate = nDamage;

	if( !m_hFirstAttacker ) 
	{
		if( pFrom->IsSummon() )
		{
			if( static_cast< StructSummon * >( pFrom )->GetMaster() )
			{
				m_hFirstAttacker = static_cast< StructSummon * >( pFrom )->GetMaster()->GetHandle();
			}
			// 주인 없는 소환수가 친 경우 선공자 처리 안 함(어차피 소환수 곧 없어지고 몹은 제자리로...)
		}
		else
		{
			m_hFirstAttacker = pFrom->GetHandle();
		}
		m_nFirstAttackTime = t;
	}

	bool bIsTamer = false;

	// Damage 증가
	if( pFrom->IsSummon() )
	{
		// 소환수가 때린거라면 실제로는 주인이 데미지를 입힌걸로 처리.
		StructSummon * pSummon = static_cast< StructSummon* >( pFrom );

		if( pSummon->GetMaster() )
		{
			addDamage( pSummon->GetMaster()->GetHandle(), nDamage );

			// 테이머의 소환수가 때린거라면 테이밍 유지시간 증가
			if( GetTamer() == pSummon->GetMaster()->GetHandle() ) bIsTamer = true;
		}
	}
	else
	{
		addDamage( pFrom->GetHandle(), nDamage );

		// 테이머가 때린거라면 테이밍 유지시간 증가
		if( GetTamer() == pFrom->GetHandle() ) bIsTamer = true;
	}

	// 테이밍 유지시간 증가
	if( bIsTamer ) m_nTamedTime = t + GameRule::TAMING_INTERVAL;

	return StructCreature::onDamage( pFrom, elementalType, damageType, nDamage, bCritical, bDouble );
}

bool StructMonster::StartAttack( AR_HANDLE target, bool bNeedFastReaction )
{
	if( StructCreature::StartAttack( target, bNeedFastReaction ) )
	{
		SetStatus( StructMonster::STATUS_ATTACK );

		return true;
	}

	return false;
}

AR_HANDLE StructMonster::GetTamer() const
{
	return m_hTamer;
}

void StructMonster::SetTamer( AR_HANDLE handle, StructSkill* pTamingSkill )
{
	m_hTamer = handle;
	m_nTamedTime = GetArTime() + GameRule::TAMING_INTERVAL;
	m_pTamingSkill = pTamingSkill;
	// FireSkill 이후 RequestSkillLevel은 초기화 되는데 크리처 테이밍은 전투후에 다시 이 정보가 필요하므로 스킬레벨만 따로 데이터 보관.
	if( pTamingSkill )
		m_nTamingSkillLevel = pTamingSkill->GetRequestedSkillLevel();
	else
		m_nTamingSkillLevel = 0;
}

int StructMonster::GetLevel() const
{
	return m_pContentInfo->level;
}

const CreatureStat &	StructMonster::GetBaseStat() const
{
	return GameContent::GetStatInfo( m_pContentInfo->stat_id );
}

static void addChaos( StructCreature *pCorpse, StructPlayer *pPlayer, float chaos )
{
	// 더이상 저장할 수 없으면 즐
	if( pPlayer->GetMaxChaos() <= pPlayer->GetChaos() )
		return;

	// 멀리 떨어져 있으면 KIN
	AR_TIME t = GetArTime();
	AR_UNIT distance = pCorpse->GetCurrentPosition( t ).GetDistance( pPlayer->GetCurrentPosition( t ) );	
	if( distance > RANGE_LIMIT ) return;

	TS_SC_GET_CHAOS msg;

	int nChaos = GameRule::GetIntValueByRandom< int >( chaos );

	msg.hPlayer = pPlayer->GetHandle();
	msg.hCorpse = pCorpse->GetHandle();
	msg.nChaos = nChaos;
	msg.nBonusType = TS_SC_GET_CHAOS::CHAOS_BONUS_NONE;
	msg.nBonusPercent = 0;
	msg.nBonus = 0;

	// 중독 방지 시스템 관련 제한 체크
	AR_TIME nContinuousPlayTime = pPlayer->GetContinuousPlayTime();
	if( pPlayer->IsGameTimeLimited() && nContinuousPlayTime >= GameRule::nMaxHealthyGameTime )
	{
		chaos = GameRule::GetGameTimeLimitPenalty( nContinuousPlayTime ) * chaos;
	}
	else
	{
		switch( pPlayer->GetPCBangMode() )
		{
		case GameRule::PCBANG_ALLY_BONUS:
			msg.nBonus = GameRule::fAllyPCBangChaosBonusRate * nChaos;
			msg.nBonusType = TS_SC_GET_CHAOS::CHAOS_BONUS_PCBANG;
			msg.nBonusPercent = GameRule::fAllyPCBangChaosBonusRate * 100;
			break;
		case GameRule::PCBANG_PREMIUM_BONUS:
			msg.nBonus = GameRule::fPremiumPCBangChaosBonusRate * nChaos;
			msg.nBonusType = TS_SC_GET_CHAOS::CHAOS_BONUS_PREMIUM_PCBANG;
			msg.nBonusPercent = GameRule::fPremiumPCBangChaosBonusRate * 100;
			break;
		default:
			break;
		}

		// 보너스 획득 라크가 0이면 보너스 메시지도 안 띄움
		if( !msg.nBonus )
		{
			msg.nBonusType = TS_SC_GET_CHAOS::CHAOS_BONUS_NONE;
		}
		msg.nChaos += msg.nBonus;
	}

	// 라크 획득이 음수일 경우 중단
	if( chaos <= 0.0f )
		return;

	ArcadiaServer::Instance().Broadcast( pCorpse->GetRX(), pCorpse->GetRY(),pCorpse->GetLayer(), &msg );

	pPlayer->AddChaos( msg.nChaos );

	LOG::Log11N4S( LM_LAC_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, msg.nChaos, pPlayer->GetChaos(), 0, 0, 0, 0, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	
}

static void addChaos( StructCreature *pCorpse, int nPartyID, float chaos )
{
	struct myPartyFunctor1 : PartyManager::PartyFunctor
	{
		myPartyFunctor1()
		{
			t = GetArTime();
		}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( !pPlayer || !pPlayer->IsInWorld() ) return true;

			// 멀리 떨어져 있으면 KIN			
			AR_UNIT distance = pos.GetDistance( pPlayer->GetPos() );	
			if( distance > RANGE_LIMIT ) return true;

			vPlayers.push_back( pPlayer );

			return true;
		}

		ArPosition pos;
		AR_TIME t;
		std::vector< StructPlayer * > vPlayers;
	} _tmp;

	_tmp.pos = pCorpse->GetCurrentPosition( GetArTime() );

	PartyManager::GetInstance().DoEachMember( nPartyID, _tmp );

	std::vector< StructPlayer * >::iterator it;

	float fSharedChaos = chaos / _tmp.vPlayers.size();

	for( it = _tmp.vPlayers.begin(); it != _tmp.vPlayers.end(); ++it )
	{
		addChaos( pCorpse, (*it), fSharedChaos );
	}
}

static void addEXP( StructCreature *pCorpse, StructPlayer *pPlayer, float exp, float jp )
{
	if( pPlayer->IsDead() )
		return;

	float fJP = jp;

	// { ImmoralPoint 보정
	if( pPlayer->GetImmoralPoint() > 0 && pCorpse->GetLevel() >= pPlayer->GetLevel() )
	{
		float fIPDec = -1;
		if( pPlayer->GetImmoralPoint() >= GameRule::CRIME_LIMIT )
		{
			fIPDec = -0.5f;
		}
		if( pPlayer->GetPartyID() )
		{
			fIPDec *= ( 2 / (float)( PartyManager::GetInstance().GetMemberCount( pPlayer->GetPartyID() ) + 1 ) );
		}

		bool bIsNeedToSendStatus = false;

		if( pPlayer->GetImmoralPoint() >= GameRule::CRIME_LIMIT && ( pPlayer->GetImmoralPoint() + fIPDec ) < GameRule::CRIME_LIMIT )
		{
			bIsNeedToSendStatus = true;
		}
		else if( pPlayer->GetImmoralPoint() >= GameRule::MORAL_LIMIT && ( pPlayer->GetImmoralPoint() + fIPDec ) < GameRule::MORAL_LIMIT )
		{
			bIsNeedToSendStatus = true;
		}

		pPlayer->SetImmoralPoint( pPlayer->GetImmoralPoint() + fIPDec );

		// 모럴 포인트 개념 추가로 음수 이모럴 포인트도 유효
		//if( pPlayer->GetImmoralPoint() <= 0 )
		//{
		//	bIsNeedToSendStatus = true;
		//	pPlayer->SetImmoralPoint( 0 );
		//}

		if( bIsNeedToSendStatus )
		{
			BroadcastStatusMessage( pPlayer );
		}
	}
	// }

	/*	개인 몫으로 분배된 경험치에, 잡힌 몬스터가 분배받을자의 레벨보다 낮으면 레벨차 패널티를 부가한다.
		몬스터LV < 플레이어LV 이면,
		플레이어가 받을 EXP = 분배된 EXP * { 1- ((플레이어LV-몬스터Lv) * 0.05) }
		플레이어가 받을 JP = 분배된 JP * { 1- ((플레이어LV-몬스터Lv) * 0.05) }*/
#ifdef _EXPDEBUG
	_cprint( "경험치 계산! : %s Lv%d    %s Lv%d\n", pCorpse->GetName(), pCorpse->GetLevel(), pPlayer->GetName(), pPlayer->GetLevel() );
	_cprint( "경험치 1 : %f,%f (%s)\n", exp, fJP, pPlayer->GetName() );
#endif

	if( pPlayer->GetLevel() > pCorpse->GetLevel() )
	{
		exp = exp * ( 1.0f - ( pPlayer->GetLevel() - pCorpse->GetLevel() ) * 0.05 );
		fJP = fJP * ( 1.0f - ( pPlayer->GetLevel() - pCorpse->GetLevel() ) * 0.05 );
	}

#ifdef _EXPDEBUG
	_cprint( "경험치 2 : %f,%f (%s)\n", exp, fJP, pPlayer->GetName() );
#endif
/* 보너스 jp 사라짐
	float fBonusJP = 0.0f;
	if( pPlayer->GetJobLevel() >= fJP*2 ) fBonusJP = fJP * 2;
	else fBonusJP = pPlayer->GetJobLevel();

#ifdef _EXPDEBUG
	_cprint( "경험치 3 : %f,%f (%s)\n", exp, fJP, pPlayer->GetName() );
#endif

	if( pPlayer->GetLevel() > pCorpse->GetLevel() )
	{
		int nGap = pPlayer->GetLevel() - pCorpse->GetLevel();
		if( nGap >= 10 ) fBonusJP = 0;
		else if( 10 > nGap && nGap >= 3 ) 
		{
			fBonusJP = fBonusJP * ( 1 - ( nGap - 2 )  * 0.1f );
		}
	}

	fJP += fBonusJP;
*/
#ifdef _EXPDEBUG
	_cprint( "경험치 4 : %f,%f (%s)\n", exp, fJP, pPlayer->GetName() );
#endif 

	// 멀리 떨어져 있으면 KIN
	AR_TIME t = GetArTime();
	AR_UNIT distance = pCorpse->GetCurrentPosition( t ).GetDistance( pPlayer->GetCurrentPosition( t ) );	
	if( distance > RANGE_LIMIT ) return;

	if( exp < 1 ) exp = 1;
	if( fJP < 0 ) fJP = 0;

	// 테이밍 한 플레이어면 경험치 조정
	if( pCorpse->IsMonster() && static_cast< StructMonster* >( pCorpse )->GetTamer() == pPlayer->GetHandle() && static_cast< StructMonster* >( pCorpse )->IsTamingSuccessed() )
	{
		exp *= static_cast< StructMonster* >( pCorpse )->GetTameExpAdjust();
		fJP *= static_cast< StructMonster* >( pCorpse )->GetTameExpAdjust();
	}

	float fEXPMod = GameRule::fEXPRate;

	exp = fEXPMod * exp;
	fJP = fEXPMod * fJP;

	__int64 nEXP = GameRule::GetIntValueByRandom< __int64 >( exp );
	__int64 nJP = GameRule::GetIntValueByRandom< __int64 >( fJP );
	pPlayer->AddExp( nEXP, nJP );

#ifdef _EXPDEBUG
	_cprint( "경험치 5 : %d,%d (%s)\n", nEXP, nJP, pPlayer->GetName() );
#endif

	// 힐러, 버퍼, 디버퍼 등, 공헌도를 따질 수 없는 직업을 위해 경험치 배분을 기준으로 Kill 체크를 한다.
	// 소환수 경험치 배분도 동일한 룰에 의해 시야 거리 밖이면 핸들러를 호출하지 않도록 한다.
	ArPosition pos				= pPlayer->GetCurrentPosition( t );
	StructSummon *pMainSummon	= pPlayer->GetMainSummon();
	StructSummon *pSubSummon	= pPlayer->GetSubSummon();
	
	pPlayer->OnKill( pCorpse );
	if( pMainSummon && pos.GetDistance( pMainSummon->GetCurrentPosition( t ) ) <= GameRule::VISIBLE_RANGE )
		pMainSummon->OnKill( pCorpse );
	if( pSubSummon && pos.GetDistance( pSubSummon->GetCurrentPosition( t ) ) <= GameRule::VISIBLE_RANGE )
		pSubSummon->OnKill( pCorpse );
}

static void addEXP( StructCreature *pCorpse, int nPartyID, int exp, int jp )
{
	struct myPartyFunctor1 : PartyManager::PartyFunctor
	{
		myPartyFunctor1()
		{
			t = GetArTime();
			nMaxLevel = 0;
			nTotalLevel = 0;
			nCount = 0;
			nTotalCount = 0;
			nMinLevel = GameRule::MAX_LEVEL;
		}

		virtual bool operator()( AR_HANDLE handle )
		{
			++nTotalCount;

			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( !pPlayer || !pPlayer->IsInWorld() ) return true;

			// 멀리 떨어져 있으면 KIN		
			if( layer != pPlayer->GetLayer() )
				return true;

			AR_UNIT distance = pos.GetDistance( pPlayer->GetPos() );	
			if( distance > RANGE_LIMIT ) return true;

			pOneManPlayer = pPlayer; // One Man Party 처리를 위해

			if( nMaxLevel < pPlayer->GetLevel() ) nMaxLevel = pPlayer->GetLevel();
			if( nMinLevel > pPlayer->GetLevel() ) nMinLevel = pPlayer->GetLevel();
			nTotalLevel += pPlayer->GetLevel();

			nCount++;

			return true;
		}

		ArPosition pos;
		unsigned char layer;
		AR_TIME t;
		int nTotalCount;
		int nCount;
		int nMaxLevel;
		int nMinLevel;
		int nTotalLevel;
		StructPlayer * pOneManPlayer;
	} _tmp;
	_tmp.pos = pCorpse->GetCurrentPosition( GetArTime() );
	_tmp.layer = pCorpse->GetLayer();

	PartyManager::GetInstance().DoEachMember( nPartyID, _tmp );

	// 파티원이 없으면
	if( _tmp.nCount < 1 )
	{
		return;
	}

	// 적용되는 파티원이 혼자이면..
	if( _tmp.nCount < 2 )
	{
		addEXP( pCorpse, _tmp.pOneManPlayer, exp, jp );
		return;
	}

	struct myPartyFunctor : PartyManager::PartyFunctor
	{
		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( !pPlayer ) return true;

			// 파티원 개인에게 돌아갈 경험치 = ( ( 개인Lv / 전 파티원 Lv 총합 ) * 파티에 분배된 경험치) * 레벨차보정팩터
			// 파티별로 할당된 경험치가 파티원들에게 분배된다. 이때는 데미지량, 공헌도에 관계없이 파티원의 LV에 따라 분배한다. 힐러, 버퍼, 디버퍼등, 공헌도를 따질 수 없는 직업이 다수 존재하기 때문이다.
			float fT = ( (float)pPlayer->GetLevel() / nTotalLevel );
			float fEXP = nSharedEXP * fT;
			float fJP  = nSharedJP * fT;

			// 파티내 최고레벨 플레이어보다 15레벨 이상 낮은 레벨의 플레이어는 10레벨 단위로 10%씩의 경험치 패널티를 받는다. (100레벨차부터는 100% 패널티로 경험치를 전혀 얻지 못한다.)
			// 레벨차보정팩터 = 1 - [ { (파티내 최고레벨 아바타의 Lv - 자신의 Lv)*0.1} * 0.1] 단, 레벨차 보정팩터가 0이하일경우 0으로 고정, 1이상일경우 1로 고정한다.	
			if (pPlayer->GetLevel() < GameRule::PartyExpPenaltyLevelCutoff)
			{
				float fLevelPenalty = 1 - int(((nMaxLevel - GameRule::PartyExpPenaltyLevel) - pPlayer->GetLevel()) * 0.1f) * 0.1f;
				fLevelPenalty = std::max(0.0f, std::min(1.0f, fLevelPenalty));
				fEXP *= fLevelPenalty;
				fJP *= fLevelPenalty;
			}

			// fPartyEXPRate[ n ] = 파티원이 n+2명일때 적용 되는 계수 (예: 파티원 2명 -> fPartEXPRate[0] 참조)
			if( nMembers > 1 )
			{
				fEXP	*= GameRule::fPartyEXPRate[ nMembers - 2];
				fJP		*= GameRule::fPartyEXPRate[ nMembers - 2];
			}

			if( fEXP < 1 ) fEXP = 1;
			
			addEXP( pCorpse, pPlayer, fEXP, fJP );

			return true;
		}

		StructCreature *pCorpse;
		int nMaxLevel;
		int nTotalLevel;
		int nSharedEXP;
		int nSharedJP;
		int nMembers;
	} _fo;

	int nLevelDiff = _tmp.nMaxLevel - _tmp.nMinLevel;

	if (_tmp.nMinLevel < GameRule::PartyExpPenaltyLevelCutoff)
	{
		if (_tmp.nTotalCount + 40 > nLevelDiff && nLevelDiff >= _tmp.nTotalCount + GameRule::PartyExpPenaltyLevel)
		{
			float fLevelPenalty = 1 - pow((nLevelDiff - _tmp.nCount - GameRule::PartyExpPenaltyLevel), 1.1) * 0.02f;

			exp *= fLevelPenalty;
			jp *= fLevelPenalty;
		}
		else if (nLevelDiff >= _tmp.nTotalCount + 40)
		{
			exp = 0;
			jp = 0;
		}
	}

	_fo.nMaxLevel	= _tmp.nMaxLevel;
	_fo.nTotalLevel	= _tmp.nTotalLevel;
	_fo.nSharedEXP	= exp * ( 1 + _tmp.nCount * _tmp.nCount * 2 * 0.01 );
	_fo.nSharedJP	= jp * ( 1 + _tmp.nCount * _tmp.nCount * 2 * 0.01 );
	_fo.pCorpse		= pCorpse;
	_fo.nMembers	= _tmp.nCount;

	PartyManager::GetInstance().DoEachMember( nPartyID, _fo );
}

void StructMonster::calcPartyContribute( StructCreature * pKiller, std::vector< VIRTUAL_PARTY > &vPartyContribute )
{
	if( m_vDamageList.empty() ) return;

	struct PARTY_DAMAGE
	{
		PARTY_DAMAGE()
		{
			nDamage = 0;
			nLevel = 0;
		}

		PARTY_DAMAGE( int _damage, int _level ) : nDamage( _damage ), nLevel( _level ) { }
		int	nDamage;
		int nLevel;
	};
	// 공헌도 맵
	AR_TIME									t = GetArTime();
	AR_HANDLE								hKiller = 0;
	std::vector< VIRTUAL_PARTY >::iterator	itVector;
	std::map< int, PARTY_DAMAGE >			mapPartyDamageContribute;
	std::map< int, PARTY_DAMAGE >::iterator	itMap;
	int										nTamerPartyID = 0;
	int										nFirstAttackPartyID = 0;
	int										nLastAttackPartyID = 0;	
	float									fMaxDamageAdd = 0.1f;	// 최다 뎀지 보너스

	vPartyContribute.reserve( m_vDamageList.size() );

	// 1분 이상 지났으면 선공 보너스 사라짐
	if( m_nFirstAttackTime + GameRule::MAX_FIRST_ATTACK_BONUS_TIME < t )
	{
		fMaxDamageAdd = 0.4f;		// 선공 보너스를 최다 뎀지 입힌 파티에게로..
		m_hFirstAttacker = NULL;
	}

	// 막타 넣은사람 찾기
	if( pKiller->IsSummon() )
	{
		StructPlayer * pPlayer = static_cast< StructSummon* >( pKiller )->GetMaster();
		if( pPlayer )
		{
			hKiller = pPlayer->GetHandle();
			nLastAttackPartyID = pPlayer->GetPartyID();
		}
	}
	else if( pKiller->IsPlayer() ) 
	{
		hKiller = pKiller->GetHandle();
		nLastAttackPartyID = static_cast< StructPlayer* >( pKiller )->GetPartyID();
	}

	// mapPartyDamageContribute 구성
	std::vector< _DAMAGE_TAG >::iterator it;
	for( it = m_vDamageList.begin(); it != m_vDamageList.end(); it++ )	
	{
		StructCreature::iterator pit( StructCreature::get( (*it).uid ) );
		StructCreature *pCreature = *pit;

		if( !pCreature ) continue;

		StructPlayer *pPlayer = ( pCreature->IsPlayer() ? static_cast< StructPlayer * >( pCreature ) : NULL );
		StructSummon *pSummon = ( pCreature->IsSummon() ? static_cast< StructSummon * >( pCreature ) : NULL );

		if( !pPlayer && !pSummon ) continue;

		if( !pPlayer ) pPlayer = pSummon->GetMaster();

		// 파티 데미지 계산
		if( pPlayer->IsInParty() ) 
		{
			// 테이머 파티 저장
			if( GetTamer() == pPlayer->GetHandle() ) nTamerPartyID = pPlayer->GetPartyID();

			itMap = mapPartyDamageContribute.find( pPlayer->GetPartyID() );

			if( itMap == mapPartyDamageContribute.end() ) 
			{
				mapPartyDamageContribute[ pPlayer->GetPartyID() ] = PARTY_DAMAGE( (*it).nDamage, pPlayer->GetLevel() );
			}
			else 
			{
				itMap->second.nDamage += (*it).nDamage;

				if( itMap->second.nLevel < pPlayer->GetLevel() ) itMap->second.nLevel = pPlayer->GetLevel();
			}

			// 선공 파티 저장
			if( pPlayer->GetHandle() == m_hFirstAttacker ) 
			{
				nFirstAttackPartyID = pPlayer->GetPartyID();
			}
		}
		else
		{	
			for( itVector = vPartyContribute.begin(); itVector != vPartyContribute.end(); ++itVector )
			{
				if( (*itVector).hPlayer == pPlayer->GetHandle() )
				{
					(*itVector).nDamage += (*it).nDamage;
					break;
				}
			}

			if( itVector == vPartyContribute.end() ) vPartyContribute.push_back( VIRTUAL_PARTY( pPlayer->GetHandle(), (*it).nDamage, pPlayer->GetLevel() ) );
		}
	}

	for( itMap = mapPartyDamageContribute.begin(); itMap != mapPartyDamageContribute.end(); ++itMap )
	{
		vPartyContribute.push_back( VIRTUAL_PARTY( itMap->first, itMap->second.nDamage, itMap->second.nLevel ) );
	}
	
	// TODO: 게임 종료한 사용자의 경우 여기로 떨어질 수 있으므로, 여기서 바로 RETURN할 수 있도록.
	// assert( vPartyContribute.size() != 0 );

	// 뎀지순으로 정렬
	std::sort( vPartyContribute.begin(), vPartyContribute.end(), VIRTUAL_PARTY::greaterByDamage );

	for( itVector = vPartyContribute.begin(); itVector != vPartyContribute.end(); ++itVector )
	{
		float fContribute = 0.0f;

		// { 테이머 파티 셋팅
		if( nTamerPartyID != 0 && nTamerPartyID == (*itVector).nPartyID )
		{
			(*itVector).bTamer = true;
		}
		// )

		// 공헌도 = ( 데미지양 / 그 몬스터가 입은 총 데미지양) * 0.5
		if( m_nTotalDamage ) fContribute = ( (float) (*itVector).nDamage / m_nTotalDamage ) * 0.5f;

		// 최다 데미지 보너스
		if( itVector == vPartyContribute.begin() ) fContribute += fMaxDamageAdd;

		// { 선공 보너스 - 공헌도 0.3 추가
		if( nFirstAttackPartyID != 0 )
		{
			if( (*itVector).nPartyID == nFirstAttackPartyID ) fContribute += 0.3f;
		}
		else if( m_hFirstAttacker && (*itVector).hPlayer == m_hFirstAttacker ) fContribute += 0.3f;
		// }

		// { 막타 보너스 - 공헌도 0.1 추가
		if( nLastAttackPartyID != 0 )
		{
			if( (*itVector).nPartyID == nLastAttackPartyID ) fContribute += 0.1f;	// 파티
		}
		else
		{
			if( (*itVector).hPlayer == hKiller ) fContribute += 0.1f;				// 솔로
		}
		// }

		(*itVector).fContribute = fContribute;
	}
}

void StructMonster::procEXP( StructCreature *pKiller, std::vector< VIRTUAL_PARTY > & vPartyContribute )
{
	if( m_vDamageList.empty() ) return;

	// 	최종EXP = 기본EXP * [ 1 + { (친사람수 -1) * 0.2} ] (단, 6명까지만.) // deleted
	int nEXP = m_pContentInfo->exp;
	int nJP = m_pContentInfo->jp;

	std::vector< VIRTUAL_PARTY >::iterator	itVector;
	for( itVector = vPartyContribute.begin(); itVector != vPartyContribute.end(); ++itVector )
	{
		// { 공헌도에 따른 경험치/JP 계산
		int nSharedEXP = nEXP * (*itVector).fContribute;
		float fSharedJP = nJP * (*itVector).fContribute;
		if( nSharedEXP < 1 ) nSharedEXP = 1;	// EXP 는 최소 1
		// }

		// { 테이밍 보너스
		if( (*itVector).hPlayer )
		{
			if( (*itVector).hPlayer == GetTamer() ) 
			{
				nSharedEXP	*= GetTameExpAdjust();
				fSharedJP	*= GetTameExpAdjust();
			}
		}
		else
		{
			if( (*itVector).bTamer )
			{
				nSharedEXP	*= GetTameExpAdjust();
				fSharedJP	*= GetTameExpAdjust();
			}
		}
		// }

		if( (*itVector).hPlayer )
		{
			StructPlayer::iterator pit( StructPlayer::get( (*itVector).hPlayer ) );
			if( *pit )
			{
				addEXP( this, *pit, nSharedEXP, fSharedJP );	// 솔로일경우
			}
		}
		else
		{
			addEXP( this, (*itVector).nPartyID, nSharedEXP, fSharedJP );	// 파티일경우
		}
	}
}
void StructMonster::procDropChaos( StructCreature *pKiller, std::vector< VIRTUAL_PARTY > & vPartyContribute, float fDropRatePenalty, float fPCBangDropRateBonus )
{
	if( m_pContentInfo->chaos_drop_percentage * GameRule::fChaosDropRate * fDropRatePenalty * fPCBangDropRateBonus > XRandom() % 100 )
	{
		c_fixed10 fGameTimeLimitPenalty;
		fGameTimeLimitPenalty.set( 0 );

		THREAD_SYNCRONIZE( &m_csHate );

		// 중독 방지 시스템에 의해 몬스터 공격자가 모두 해로운 게임 시간이면 드랍 안 함
		// 페널티 수치를 공격자 내 최고 수치로 적용(드랍율은 변동 없이 드랍량만 변경)
		for( std::vector< struct _HATE_TAG >::iterator it = m_vHateList.begin() ; it != m_vHateList.end() ; ++it )
		{
			if( (*it).uid )
			{
				StructCreature::iterator pit = StructCreature::get( (*it).uid );
				if( *pit )
				{
					StructPlayer *pPlayer = NULL;

					if( (*pit)->IsPlayer() )
						pPlayer = static_cast< StructPlayer * >( *pit );
					else if( (*pit)->IsSummon() )
						pPlayer = static_cast< StructSummon * >( *pit )->GetMaster();

					if( !pPlayer || !pPlayer->IsLoginComplete() || pPlayer->GetLogoutTime() || !pPlayer->IsInWorld() )
						continue;

					if( pPlayer->IsGameTimeLimited() )
					{
						c_fixed10 fCurrentPlayerPenalty = GameRule::GetGameTimeLimitPenalty( pPlayer->GetContinuousPlayTime() );

						if( fGameTimeLimitPenalty < fCurrentPlayerPenalty )
						{
							fGameTimeLimitPenalty = fCurrentPlayerPenalty;
						}
					}
					else
					{
						fGameTimeLimitPenalty.set( 10000 );
						break;
					}
				}
			}
		}

		// 원샷 킬 하면 킬러의 Hate가 Add되기 전에 StructMonster::onDead 가 불려서 이쪽으로 와버림
		// ( m_vHateList.empty() == true )가 되므로 그냥 겸사겸사 킬러에 대해서 한 번 더 검사
		if( fGameTimeLimitPenalty == 0 )
		{
			if( pKiller )
			{
				StructPlayer *pPlayer = NULL;

				if( pKiller->IsPlayer() )
					pPlayer = static_cast< StructPlayer * >( pKiller );
				else if( pKiller->IsSummon() )
					pPlayer = static_cast< StructSummon * >( pKiller )->GetMaster();

				if( pPlayer && pPlayer->IsLoginComplete() && !pPlayer->GetLogoutTime() && pPlayer->IsInWorld() )
				{
					if( pPlayer->IsGameTimeLimited() )
					{
						c_fixed10 fCurrentPlayerPenalty = GameRule::GetGameTimeLimitPenalty( pPlayer->GetContinuousPlayTime() );

						if( fGameTimeLimitPenalty < fCurrentPlayerPenalty )
						{
							fGameTimeLimitPenalty = fCurrentPlayerPenalty;
						}
					}
					else
					{
						fGameTimeLimitPenalty.set( 10000 );
					}
				}
			}
		}

		if( fGameTimeLimitPenalty == 0 )
		{
			return;
		}

		// 여기까지는 int 절사
		float chaos = (int)( fGameTimeLimitPenalty * XRandom( m_pContentInfo->chaos_min, m_pContentInfo->chaos_max ) );

		if( GetDungeonId() )
		{
			int nTaxRate = 0;
			int nGuildID = 0;

			DungeonManager::Instance().GetTaxRate( GetDungeonId(), &nTaxRate, &nGuildID );
			int dungeon_tax = chaos * nTaxRate / 100.0f;

			if( nGuildID )
				GuildManager::GetInstance().GiveTax( nGuildID, StructGold( 0 ), dungeon_tax );

			chaos -= dungeon_tax;
		}

		std::vector< VIRTUAL_PARTY >::iterator	itVector;

		for( itVector = vPartyContribute.begin(); itVector != vPartyContribute.end(); ++itVector )
		{
			float fSharedChaos = chaos * (*itVector).fContribute;

			if( (*itVector).hPlayer )
			{
				StructPlayer::iterator pit( StructPlayer::get( (*itVector).hPlayer ) );
				if( *pit ) addChaos( this, *pit, fSharedChaos );	// 솔로일경우
			}
			else
			{
				addChaos( this, (*itVector).nPartyID, fSharedChaos );
			}
		}
	}
}

void StructMonster::procDropGold( const ArPosition & pos, StructCreature * pKiller, struct takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, float fDropRatePenalty, float fPCBangDropRateBonus )
{
	float fGoldDropRate = GameRule::fGoldDropRate * fDropRatePenalty * fPCBangDropRateBonus;

	if( m_pContentInfo->gold_drop_percentage * fGoldDropRate > XRandom() % 100 )
	{
		c_fixed10 fGameTimeLimitPenalty;
		fGameTimeLimitPenalty.set( 0 );

		THREAD_SYNCRONIZE( &m_csHate );

		// 중독 방지 시스템에 의해 몬스터 공격자가 모두 해로운 게임 시간이면 드랍 안 함
		// 페널티 수치를 공격자 내 최고 수치로 적용(드랍율은 변동 없이 드랍량만 변경)
		for( std::vector< struct _HATE_TAG >::iterator it = m_vHateList.begin() ; it != m_vHateList.end() ; ++it )
		{
			if( (*it).uid )
			{
				StructCreature::iterator pit = StructCreature::get( (*it).uid );
				if( *pit )
				{
					StructPlayer *pPlayer = NULL;

					if( (*pit)->IsPlayer() )
						pPlayer = static_cast< StructPlayer * >( *pit );
					else if( (*pit)->IsSummon() )
						pPlayer = static_cast< StructSummon * >( *pit )->GetMaster();

					if( !pPlayer || !pPlayer->IsLoginComplete() || pPlayer->GetLogoutTime() || !pPlayer->IsInWorld() )
						continue;

					if( pPlayer->IsGameTimeLimited() )
					{
						c_fixed10 fCurrentPlayerPenalty = GameRule::GetGameTimeLimitPenalty( pPlayer->GetContinuousPlayTime() );

						if( fGameTimeLimitPenalty < fCurrentPlayerPenalty )
						{
							fGameTimeLimitPenalty = fCurrentPlayerPenalty;
						}
					}
					else
					{
						fGameTimeLimitPenalty.set( 10000 );
						break;
					}
				}
			}
		}

		// 원샷 킬 하면 킬러의 Hate가 Add되기 전에 StructMonster::onDead 가 불려서 이쪽으로 와버림
		// ( m_vHateList.empty() == true )가 되므로 그냥 겸사겸사 킬러에 대해서 한 번 더 검사
		if( fGameTimeLimitPenalty == 0 )
		{
			if( pKiller )
			{
				StructPlayer *pPlayer = NULL;

				if( pKiller->IsPlayer() )
					pPlayer = static_cast< StructPlayer * >( pKiller );
				else if( pKiller->IsSummon() )
					pPlayer = static_cast< StructSummon * >( pKiller )->GetMaster();

				if( pPlayer && pPlayer->IsLoginComplete() && !pPlayer->GetLogoutTime() && pPlayer->IsInWorld() )
				{
					if( pPlayer->IsGameTimeLimited() )
					{
						c_fixed10 fCurrentPlayerPenalty = GameRule::GetGameTimeLimitPenalty( pPlayer->GetContinuousPlayTime() );

						if( fGameTimeLimitPenalty < fCurrentPlayerPenalty )
						{
							fGameTimeLimitPenalty = fCurrentPlayerPenalty;
						}
					}
					else
					{
						fGameTimeLimitPenalty.set( 10000 );
					}
				}
			}
		}

		if( fGameTimeLimitPenalty == 0 )
		{
			return;
		}

		StructGold gold( fGameTimeLimitPenalty * XRandom( m_pContentInfo->gold_min, m_pContentInfo->gold_max ) );

		if( gold < 0 )							gold.SetRawData( 1 );
		if( gold > GameRule::MAX_GOLD_DROP )	gold = GameRule::MAX_GOLD_DROP;
		
		if( GetDungeonId() )
		{
			int nTaxRate = 0;
			int nGuildID = 0;

			DungeonManager::Instance().GetTaxRate( GetDungeonId(), &nTaxRate, &nGuildID );
			StructGold dungeon_tax( gold.GetRawData() * nTaxRate / 100.0f );

			if( nGuildID )
				GuildManager::GetInstance().GiveTax( nGuildID, dungeon_tax, 0 );

			gold -= dungeon_tax;
		}

		if( gold >= 0 )
		{
			StructItem* pItem = StructItem::AllocGold( gold, ItemInstance::BY_MONSTER );
			ArPosition valid_pos;
			if( GameContent::GetValidRandomPos( pos, valid_pos, ITEM_DROP_LENGTH ) == false )
			{
				if( pKiller )
				{
					_cprint( "gold drop valid pos failed. set killer pos." );
					FILELOG( "gold drop valid pos failed. set killer pos." );

					valid_pos = pKiller->GetPos();
					if( GameContent::IsBlocked( valid_pos.x, valid_pos.y ) == true )
					{
						_cprint( "killer pos is blocked." );
						FILELOG( "killer pos is blocked." );
					}
				}
				else
				{
					_cprint( "gold drop valid pos failed. killer == NULL" );
					FILELOG( "gold drop valid pos failed. killer == NULL\n" );

					valid_pos = pos;
				}
			}

			pItem->SetCurrentXY( valid_pos.x, valid_pos.y );
			pItem->SetCurrentLayer( GetLayer() );
			pItem->SetPickupOrder( pPriority->ItemPickupOrder );
			MonsterDropItemToWorld( this, pItem );
		}
	}
}

void StructMonster::dropItem( const ArPosition & pos, StructCreature * pKiller, struct takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, ItemBase::ItemCode code, const __int64 & count, int level, bool bIsEventItem, int nFlagIndex )
{
	if( !count )
	{
		FILELOG( "dropItem: count was 0. (x: %f, y: %f, code: %d, Killer: %s)", pos.GetX(), pos.GetY(), code, pKiller->GetName() );
		return;
	}

	// 중독 방지 시스템에 의해 몬스터 공격자가 모두 해로운 게임 시간이면 드랍 안 함
	bool bHarmfulUsersOnly = true;
	for( std::vector< struct _HATE_TAG >::iterator it = m_vHateList.begin() ; it != m_vHateList.end() ; ++it )
	{
		if( (*it).uid )
		{
			StructCreature::iterator pit = StructCreature::get( (*it).uid );
			if( *pit )
			{
				StructPlayer *pPlayer = NULL;

				if( (*pit)->IsPlayer() )
					pPlayer = static_cast< StructPlayer * >( *pit );
				else if( (*pit)->IsSummon() )
					pPlayer = static_cast< StructSummon * >( *pit )->GetMaster();

				if( !pPlayer || !pPlayer->IsLoginComplete() || pPlayer->GetLogoutTime() || !pPlayer->IsInWorld() )
					continue;

				if( !pPlayer->IsGameTimeLimited() || pPlayer->GetContinuousPlayTime() < GameRule::nMaxTiredGameTime )
				{
					bHarmfulUsersOnly = false;
					break;
				}
			}
		}
	}

	// 원샷 킬 하면 킬러의 Hate가 Add되기 전에 StructMonster::onDead 가 불려서 이쪽으로 와버림
	// ( m_vHateList.empty() == true )가 되므로 그냥 겸사겸사 킬러에 대해서 한 번 더 검사
	if( bHarmfulUsersOnly )
	{
		if( pKiller )
		{
			StructPlayer *pPlayer = NULL;

			if( pKiller->IsPlayer() )
				pPlayer = static_cast< StructPlayer * >( pKiller );
			else if( pKiller->IsSummon() )
				pPlayer = static_cast< StructSummon * >( pKiller )->GetMaster();

			if( pPlayer && pPlayer->IsLoginComplete() && !pPlayer->GetLogoutTime() && pPlayer->IsInWorld()
				&& ( !pPlayer->IsGameTimeLimited() || pPlayer->GetContinuousPlayTime() < GameRule::nMaxTiredGameTime ) )
			{
				bHarmfulUsersOnly = false;
			}
		}
	}

	if( bHarmfulUsersOnly )
	{
		return;
	}

	StructItem* pItem = StructItem::AllocItem( 0, code, count, ItemInstance::BY_MONSTER, level );
	ArPosition valid_pos;
	if( GameContent::GetValidRandomPos( pos, valid_pos, ITEM_DROP_LENGTH ) == false )
	{
		if( pKiller )
		{
			_cprint( "item drop valid pos failed. set killer pos." );
			FILELOG( "item drop valid pos failed. set killer pos." );

			valid_pos = pKiller->GetPos();
			if( GameContent::IsBlocked( valid_pos.x, valid_pos.y ) == true )
			{
				_cprint( "killer pos is blocked." );
				FILELOG( "killer pos is blocked." );
			}
		}
		else
		{
			_cprint( "item drop valid pos failed. killer == NULL" );
			FILELOG( "item drop valid pos failed. killer == NULL\n" );

			valid_pos = pos;
		}
	}

	pItem->SetCurrentXY( valid_pos.x, valid_pos.y );
	pItem->SetCurrentLayer( GetLayer() );
	pItem->SetPickupOrder( pPriority->ItemPickupOrder );
	
	// nFlag가 유효한 플래그라면 해당 플래그를 세팅
	if( nFlagIndex >= 0 && nFlagIndex < pItem->GetInstanceFlag().TOTAL_BITS )
		pItem->SetInstanceFlagOn( nFlagIndex );

	if( bIsEventItem ) 
	{
		pItem->SetEventDropFlag( true );
		pItem->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_EVENT );
	}
	MonsterDropItemToWorld( this, pItem );
}

static bool checkDrop( int code, int percentage, StructCreature * pKiller, float fDropRatePenalty, float fPCBangDropRateBonus )
{
	StructPlayer *pPlayer = NULL;

	if( pKiller->IsPlayer() )
		pPlayer = static_cast< StructPlayer * >( pKiller );
	else if( pKiller->IsSummon() )
		pPlayer = static_cast< StructSummon * >( pKiller )->GetMaster();

	float fCreatureCardMod = 1.0f;

	// 아이템 챤스 적용
	float fMod = ( 1.0f + pPlayer->GetItemChance() * 0.01f );

	if( code > 0 )
	{
		// { 소환수 카드 드랍율 증가 처리
		if( StructItem::GetItemBase( code ).nGroup == ItemBase::GROUP_SUMMONCARD )
		{
			fCreatureCardMod = pKiller->GetCreatureCardChance();
		}

		// 퀘스트 아이템인 경우(보통 몬스터가 드랍하지 않지만 간혹 사용됨 - 평소에도 주는 잡템 모으기) 드랍율 페널티 미적용
		// 데스매치 전용 아이템은 데스매치에서만 드랍되며 레벨에 상관없이 드랍이 되어야 함.
		if( StructItem::GetItemBase( code ).Flag.IsOn( ItemBase::FLAG_QUEST ) )
		{
			fDropRatePenalty = 1.0f;
		}
	}

	// 데스매치 내에 몬스터를 잡는 경우 레벨 패널티를 받지 않음
	if( pPlayer && pPlayer->IsInDeathmatch() )
		fDropRatePenalty = 1.0f;

	// 확률 계산 후 KIN

	float fItemDropRate = GameRule::fItemDropRate;

	if( XRandom( 1, 100000000 ) > ( percentage * fMod * fItemDropRate * fDropRatePenalty * fPCBangDropRateBonus ) * fCreatureCardMod ) return false;

	return true;
}

void StructMonster::dropItemGroup( const ArPosition & pos, StructCreature * pKiller, struct takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, int nDropGroupID, const __int64 & count, int level, int nFlagIndex )
{
	std::map< ItemBase::ItemCode, __int64 > mapDropItem;

	for( int i = 0 ; i < static_cast< int >( count ) ; ++i )
	{
		ItemBase::ItemCode nItemID = nDropGroupID;
		__int64 nItemCount = 1;

		do
		{
			GameContent::SelectItemIDFromDropGroup( nItemID, nItemID, nItemCount );
		} while( nItemID < 0 );

		// 꽝이 아니고 아이템이 선택된 경우에만 드랍 대상으로 추가
		if( nItemID > 0 )
		{
			std::map< ItemBase::ItemCode, __int64 >::iterator it = mapDropItem.find( nItemID );
			if( it == mapDropItem.end() )
			{
				mapDropItem.insert( std::make_pair( nItemID, nItemCount ) );
			}
			else
			{
				(*it).second += nItemCount;
			}
		}
	}

	for( std::map< ItemBase::ItemCode, __int64 >::const_iterator it = mapDropItem.begin() ; it != mapDropItem.end() ; ++it )
	{
		dropItem( pos, pKiller, pPriority, vPartyContribute, (*it).first, (*it).second, level, false, nFlagIndex );
	}
}

void StructMonster::procDropItem( const ArPosition & pos, StructCreature * pKiller, takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, float fDropRatePenalty, float fPCBangDropRateBonus )
{
	if( m_pContentInfo && m_pContentInfo->item_drop_list )
	{
		if( StructEventItemManager::GetInstance().IsStartingDungeonDropRateEvent() )
		{
			int nDungeonID = DungeonManager::Instance().GetDungeonID( GetX(), GetY() );
			if( !nDungeonID )
			{
				nDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( ArPosition( GetX(), GetY() ) );
			}

			if( nDungeonID )
			{
				fPCBangDropRateBonus *= StructEventItemManager::GetInstance().GetEventDungeonDropRate( nDungeonID );
			}
		}

		for( std::vector< MonsterBase::MONSTER_ITEM_DROP_INFO >::const_iterator it = m_pContentInfo->item_drop_list->begin() ; it != m_pContentInfo->item_drop_list->end() ; ++it )
		{
			const MonsterBase::MONSTER_ITEM_DROP_INFO & item_drop = (*it);

			if( !item_drop.item_id ) continue;

			if( !checkDrop( item_drop.item_id, item_drop.percentage, pKiller, fDropRatePenalty, fPCBangDropRateBonus ) ) continue;

			// 갯수
			__int64 item_count = XRandom( item_drop.min_count, item_drop.max_count );
			if( item_count < item_drop.min_count || item_count > item_drop.max_count )
			{
				_cprint( "StructMonster::procDropItem(...) : invalid item count! [%d][%d]\n", item_drop.item_id, item_count );
				FILELOG( "StructMonster::procDropItem(...) : invalid item count! [%d][%d]", item_drop.item_id, item_count );
				continue;
			}

			// 레벨
			int item_level = XRandom( item_drop.min_level, item_drop.max_level );

			// drop group
			if( item_drop.item_id < 0 )
			{
				dropItemGroup( pos, pKiller, pPriority, vPartyContribute, item_drop.item_id, item_count, item_level, ItemInstance::ITEM_FLAG_NONE );
			}
			else
			{	
				dropItem( pos, pKiller, pPriority, vPartyContribute, item_drop.item_id, item_count, item_level, false, ItemInstance::ITEM_FLAG_NONE );
			}
		}

		// 이벤트 드랍
		ItemBase::ItemCode nEventItemCode = StructEventItemManager::GetInstance().GetActiveDrop( GetArTime(), fDropRatePenalty );
		if( nEventItemCode && GameContent::IsValidItemCode( nEventItemCode ) )
		{
			dropItem( pos, pKiller, pPriority, vPartyContribute, nEventItemCode, 1, 1, true, ItemInstance::ITEM_FLAG_NONE );
			
			StructPlayer *pPlayer = NULL;
			if( pKiller )
			{
				if( pKiller->IsPlayer() )
					pPlayer = static_cast< StructPlayer * >( pKiller );
				else if( pKiller->IsSummon() )
					pPlayer = static_cast< StructSummon * >( pKiller )->GetMaster();
			}

			if( pPlayer )
			{
				LOG::Log11N4S( LM_ITEM_EVENT_DROP, pPlayer->GetAccountID(), 
					pPlayer->GetSID(), 0,
					nEventItemCode, 1,
					this->GetMonsterId(), this->GetX(),
					this->GetY(), 0, 
					0, 0, 
					pPlayer->GetAccountName(), LOG::STR_NTS, 
					pPlayer->GetName(), LOG::STR_NTS, 
					"", 0, 
					"", 0 );
			}
			else
			{
				LOG::Log11N4S( LM_ITEM_EVENT_DROP, 0, 
					0, 0,
					nEventItemCode, 1,
					this->GetMonsterId(), this->GetX(),
					this->GetY(), 0, 
					0, 0, 
					"", 0, 
					"", 0, 
					"", 0, 
					"", 0 );
			}
		}
	}

	// 퀘스트 및 호칭 처리
	procQuestAndTitle( pos, pKiller, pPriority, vPartyContribute, fDropRatePenalty );
}

void StructMonster::onDead( StructCreature * pKiller, bool decreaseEXPOnDead )
{
	std::vector< VIRTUAL_PARTY > vPartyContribute;

	StructCreature::onDead( pKiller, decreaseEXPOnDead );

	SetStatus( STATUS_DEAD );
	_monster_debug_output( "DEAD : %I64d\n", GetHandle() );

	// 스캐너에서 제거
	quadTreeItem.RemoveMe();

	m_bTamedSuccess = false;

	// 테이밍 처리
	if( GetTamer() ) 
	{
		StructPlayer::iterator it = StructPlayer::get( GetTamer() );
		if( *it ) m_bTamedSuccess = ProcTame( this );
	}

	calcPartyContribute( pKiller, vPartyContribute );

	// 경치 분배(몬스터 DB에 획득 경험치가 이상일 때만경험치 적용 처리 하도록 변경)
	if( GetMonsterBase()->exp > 0 )
	{
		procEXP( pKiller, vPartyContribute );
	}

	StructPlayer* pKillPlayer = NULL;
	if( pKiller != NULL )
	{
		if( pKiller->IsPlayer() )
		{
			pKillPlayer = static_cast< StructPlayer* >( pKiller );
		}
		else if( pKiller->IsSummon() )
		{
			StructSummon* pKillSummon = static_cast< StructSummon* >( pKiller );
			if( pKillSummon != NULL )
			{
				pKillPlayer = pKillSummon->GetMaster();
			}
		}
	}

	// 사망 시 스크립트 실행할 내용이 있다면 스크립트 실행(실행 위치, Lua에 전달할 파라미터 재결정 필요)
	if( GetMonsterBase()->script_on_dead[ 0 ] )
	{
		ThreadPlayerHelper TPHepler( pKillPlayer );

		char szScriptBuffer[ 256 ];
		s_strcpy( szScriptBuffer, _countof( szScriptBuffer ), GetMonsterBase()->script_on_dead );

		char szElementBuffer[ 32 ];
		s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%d", (int)GetX() );
		XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@pos_x@#", szElementBuffer );
		s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%d", (int)GetY() );
		XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@pos_y@#", szElementBuffer );
		s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%d", (int)GetLayer() );
		XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@pos_layer@#", szElementBuffer );		
		s_sprintf( szElementBuffer, _countof( szElementBuffer ), "%u", GetHandle() );
		XStringUtil::Replace( szScriptBuffer, _countof( szScriptBuffer ), "#@monster_handle@#", szElementBuffer );

		LUA()->RunString( szScriptBuffer );
	}

	// 보스급 몬스터는 사망 시 로그 남기자.
	if( GetMonsterType() >= MonsterBase::MONSTER_TYPE_HIGHEST_1_STAR )
	{
		int nDungeonID = 0;
		if( pKillPlayer->IsInDungeon() )
		{
			nDungeonID = DungeonManager::Instance().GetDungeonID( pKillPlayer->GetX(), pKillPlayer->GetY() );
		}
		else if( pKillPlayer->IsInInstanceDungeon() )
		{
			nDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( ArPosition( pKillPlayer->GetX(), pKillPlayer->GetY() ) );
		}

		LOG::Log11N4S( LM_MONSTER_KILL, pKillPlayer->GetAccountID(),
			pKillPlayer->GetPlayerUID(), pKillPlayer->GetPartyID(),
			GetMonsterId(), nDungeonID,
			GetX(), GetY(),
			pKillPlayer->GetX(), pKillPlayer->GetY(),
			GetLayer(), 0,
			pKillPlayer->GetAccountName(), LOG::STR_NTS, 
			pKillPlayer->GetName(), LOG::STR_NTS, 
			"", 0, 
			"", 0 );
	}

	// 테이밍 안되었다면 드랍
	if( !m_bTamedSuccess )
	{
		std::sort( vPartyContribute.begin(), vPartyContribute.end(), VIRTUAL_PARTY::greaterByContribute );

		ArPosition pos = GetCurrentPosition( GetArTime() );

		takePriority Priority;
		std::vector< VIRTUAL_PARTY >::iterator itContribute;

		// { 획득 우선순위 생성
		int nCount = 0;

		for( itContribute = vPartyContribute.begin(); itContribute != vPartyContribute.end(); ++itContribute )
		{
			// 레이드 보스 몬스터의 경우는 파티가 바로 해산되므로 소유권을 없애야 함
			if( IsDungeonRaidMonster() && GetMonsterType() >= MonsterBase::MONSTER_TYPE_HIGHEST_1_STAR )
			{
				Priority.ItemPickupOrder.hPlayer[nCount] = 0;
				Priority.ItemPickupOrder.nPartyID[nCount] = 0;
			}
			else if( (*itContribute).hPlayer )
			{
				Priority.ItemPickupOrder.hPlayer[nCount] = (*itContribute).hPlayer;
				Priority.ItemPickupOrder.nPartyID[nCount] = 0;
			}
			else
			{
				Priority.ItemPickupOrder.hPlayer[nCount] = 0;
				Priority.ItemPickupOrder.nPartyID[nCount] = (*itContribute).nPartyID;
			}

			if( ++nCount >= 3 )
			{
				break;
			}
		}
		// }

		// 최고 우선순위 보유자 정보 적용
		itContribute = vPartyContribute.begin();

		int nMaxPriorityLevel = ( vPartyContribute.empty() ) ? 0 : (*itContribute).nLevel;
		float fMaxContribute = ( vPartyContribute.empty() ) ? 0.0f : (*itContribute).fContribute;

		float fDropRatePenalty = 1.0f;
		int nLevelDiff = nMaxPriorityLevel - GetLevel();

		if( nLevelDiff >= 10 )
		{
			fDropRatePenalty = std::max( 1 - ( nLevelDiff - 10 ) * 0.2f, 0.0f );
		}

		// 더블 플러스 PC방 혜택 적용 여부 체크
		bool bPremiumPCBang = false;

		// 어그로 정보 자체가 없을 경우(한 방 사살 -_ -;) 킬러 기준 적용
		if( vPartyContribute.empty() )
		{
			StructPlayer *pKillPlayer = NULL;
			if( pKiller->IsPlayer() )
				pKillPlayer = static_cast< StructPlayer * >( pKiller );
			else if( pKiller->IsSummon() )
				pKillPlayer = static_cast< StructSummon * >( pKiller )->GetMaster();

			bPremiumPCBang = pKillPlayer && pKillPlayer->GetPCBangMode() == GameRule::PCBANG_PREMIUM_BONUS;
		}
		// 최고 우선순위가 플레이어에게 있었을 경우
		else if( (*itContribute).hPlayer )
		{
			StructPlayer::iterator pit = StructPlayer::get( (*itContribute).hPlayer );
			bPremiumPCBang = (*pit) && static_cast< StructPlayer * >( (*pit) )->GetPCBangMode() == GameRule::PCBANG_PREMIUM_BONUS;
		}
		// 최고 우선순위가 파티에게 있었을 경우
		else if( (*itContribute).nPartyID )
		{
			// 파티 멤버 중 더블 플러스 PC방 멤버 찾기
			struct partyPCBangBonusChecker : public PartyManager::PartyFunctor
			{
				partyPCBangBonusChecker( const ArPosition & posKiller )
					: m_posKiller( posKiller )
					, m_bPremiumPCBang( false )
				{}

				virtual bool operator()( AR_HANDLE handle )
				{
					if( m_bPremiumPCBang )
						return true;

					StructPlayer::iterator pit = StructPlayer::get( handle );
					StructPlayer *pPlayer = *pit;
					if( !pPlayer || !pPlayer->IsInWorld() ) return true;

					// 멀리 떨어져 있으면 KIN
					if( m_posKiller.GetDistance( pPlayer->GetPos() ) > RANGE_LIMIT ) return true;

					// 파티 멤버 중 더블 플러스 PC방 유저 있는지 찾기
					if( pPlayer->GetPCBangMode() == GameRule::PCBANG_PREMIUM_BONUS )
						m_bPremiumPCBang = true;

					return true;
				}

				const ArPosition &	m_posKiller;
				bool				m_bPremiumPCBang;
			} _fo( GetPos() );

			PartyManager::GetInstance().DoEachMember( (*itContribute).nPartyID, _fo );

			bPremiumPCBang = _fo.m_bPremiumPCBang;
		}

		c_fixed10 difficultyBonus = GameRule::GetDifficultyBonus( m_nDifficulty );

		float fGoldDropRateBonus = 1.0f * difficultyBonus;
		float fItemDropRateBonus = 1.0f * difficultyBonus;
		float fChaosDropRateBonus = 1.0f * difficultyBonus;

		if( bPremiumPCBang )
		{
			fGoldDropRateBonus = GameRule::fPremiumPCBangGoldBonusDropRate;
			fItemDropRateBonus = GameRule::fPremiumPCBangItemBonusDropRate;
			fChaosDropRateBonus = GameRule::fPremiumPCBangChaosBonusDropRate;
		}

		procDropChaos( pKiller, vPartyContribute, fDropRatePenalty, fChaosDropRateBonus );

		// 1라크 모으기 퀘스트
		if( pKiller->IsPlayer() && static_cast< StructPlayer * >( pKiller )->GetChaos() < 1 && static_cast< StructPlayer * >( pKiller )->GetQuestProgress( StructQuest::QUEST_CODE_CHAOS_COLLECT ) == QuestInstance::IN_PROGRESS )
		{
			addChaos( this, static_cast< StructPlayer * >( pKiller ), 1.0f );
		}

		if( !IsDungeonRaidMonster() || ( GetMonsterType() >= MonsterBase::MONSTER_TYPE_HIGHEST_1_STAR ) )
		{
			// 돈 떨굼
			procDropGold( pos, pKiller, &Priority, vPartyContribute, fDropRatePenalty, fGoldDropRateBonus );

			// 아템 떨굼
			procDropItem( pos, pKiller, &Priority, vPartyContribute, fDropRatePenalty, fItemDropRateBonus );
		}
	}

	// { Hate List 제거 처리

	std::vector< _HATE_TAG >::iterator it;

	StructCreature::onDead( pKiller,decreaseEXPOnDead );

	for( it = m_vHateList.begin(); it != m_vHateList.end(); ++it )
	{
		StructCreature::iterator itEnemy = StructCreature::get( (*it).uid );

		if( *itEnemy )
		{
			(*itEnemy)->RemoveFromEnemyList( GetHandle() );
		}
	}
	// }
}

void StructMonster::procQuestAndTitle( const ArPosition & pos, StructCreature *pKiller, struct takePriority* pPriority, std::vector< VIRTUAL_PARTY > & vPartyContribute, float fDropRatePenalty )
{
	if( vPartyContribute.empty() ) 
	{
		// assert( 0 );
		return;
	}
	
	VIRTUAL_PARTY & vp_tag = vPartyContribute.front();
	std::vector< struct StructPlayer * > vPlayer;

	// 대상 선택
	if( vp_tag.hPlayer )
	{
		StructPlayer::iterator pit = StructPlayer::get( vp_tag.hPlayer );
		StructPlayer *pPlayer = *pit;

		if( pPlayer ) vPlayer.push_back( pPlayer );
	}
	else
	{
		struct myPartyFunctor : PartyManager::PartyFunctor
		{
			myPartyFunctor( ArPosition p, std::vector< struct StructPlayer * > * pList ) : pos( p ), pList( pList )
			{
			}

			virtual bool operator()( AR_HANDLE handle )
			{
				StructPlayer::iterator pit = StructPlayer::get( handle );
				StructPlayer *pPlayer = *pit;
				if( !pPlayer || !pPlayer->IsInWorld() ) return true;

				// 멀리 떨어져 있으면 KIN			
				AR_UNIT distance = pos.GetDistance( pPlayer->GetPos() );	
				if( distance > RANGE_LIMIT ) return true;

				pList->push_back( pPlayer );
				
				return true;
			}

			ArPosition pos;			
			std::vector< struct StructPlayer * > *pList;
		} _tmp( pos, &vPlayer );

		PartyManager::GetInstance().DoEachMember( vp_tag.nPartyID, _tmp );
	}

	for( std::vector< struct StructPlayer * >::iterator it = vPlayer.begin(); it != vPlayer.end(); ++it )
	{
		// 퀘스트 처리
		// 킬수 처리
		(*it)->UpdateQuestStatusByMonsterKill( GetMonsterId() );

		// 사냥 수집 퀘스트의 경우 드랍그룹 적용
		std::vector< StructQuest* > vQuestList;		
		(*it)->GetQuestByMonster( GetMonsterId(), vQuestList, QuestBase::TYPE_FLAG_HUNT_ITEM | QuestBase::TYPE_FLAG_HUNT_ITEM_FROM_ANY_MONSTSERS );
		for( std::vector< StructQuest* >::iterator it2 = vQuestList.begin(); it2 != vQuestList.end(); ++it2 )
		{
			StructQuest *pQuest = (*it2);
			if( pQuest && pQuest->GetQuestBase().nDropGroupId && !pQuest->IsFinishable() )
			{
				if( pQuest->GetQuestType() == QuestBase::QUEST_HUNT_ITEM_DROP_PENALTY )
				{
					unsigned int	n64Random	= XRandom( 1, 100000000 );
					unsigned int	n64Rate		= 100000000 * fDropRatePenalty;
					if( n64Rate > n64Random )
					{
						dropItemGroup( pos, pKiller, pPriority, vPartyContribute, pQuest->GetQuestBase().nDropGroupId, 1, 1, ItemInstance::ITEM_FLAG_NONE );
					}
				}
				else
				{
					dropItemGroup( pos, pKiller, pPriority, vPartyContribute, pQuest->GetQuestBase().nDropGroupId, 1, 1, ItemInstance::ITEM_FLAG_NONE );
				}
			}
		}

		// 호칭 처리
		(*it)->UpdateTitleConditionByMonsterKill( GetMonsterId(), GetTameCode() );
	}
}

AR_UNIT StructMonster::GetFirstAttackRange() const
{
	return m_pContentInfo->visible_range;
}

bool StructMonster::IsEnemy( StructCreature* pTarget, bool bIncludeHiding )
{
	if( !StructCreature::IsEnemy( pTarget, bIncludeHiding ) )
		return false;

	if( pTarget->IsNPC() )
		return false;

	return true;
}

bool StructMonster::IsAlly( StructCreature* pTarget )
{
	if( pTarget->IsMonster() )
	{
		return true;
	}
	else if( IsOriginalDungeonOwnerGuardian() || IsOriginalDungeonSiegerGuardian() )
	{
		StructPlayer * pPlayer = NULL;

		if( pTarget->IsPlayer() )
		{
			pPlayer = static_cast< StructPlayer * >( pTarget );
		}
		else if( pTarget->IsSummon() )
		{
			pPlayer = static_cast< StructSummon * >( pTarget )->GetMaster();
		}

		if( pPlayer && pPlayer->GetPartyID() )
		{
			int dungeon_id = DungeonManager::Instance().GetDungeonID( GetX(), GetY() );

			if( dungeon_id )
			{
				int guild_id = PartyManager::GetInstance().GetAttackTeamGuildID( pPlayer->GetPartyID() );

				if( guild_id == DungeonManager::Instance().GetOriginalOwnGuildID( dungeon_id ) )
				{
					if( IsOriginalDungeonOwnerGuardian() )
						return true;
				}
				else
				{
					if( IsOriginalDungeonSiegerGuardian() )
						return true;
				}
			}
			
		}

	}
	return StructCreature::IsAlly( pTarget );
}

int StructMonster::GetCreatureGroup() const
{
	if( m_nChangingGroup != CREATURE_NONE ) 
		return m_nChangingGroup;
	
	return m_pContentInfo->group;
}

bool StructMonster::IsAttackable()
{
	if( m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_ENVIRONMENT ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_DUNGEON_CONNECTOR ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_AUTO_TRAP ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_TRAP ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_RUNAWAY ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_IMMORTAL_DUMMY )
		return false;

	return StructCreature::IsAttackable();

}

bool StructMonster::IsEnvironmentMonster() const
{
	return m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_ENVIRONMENT;
}

bool StructMonster::IsDungeonConnector() const
{
	return m_pContentInfo->race == MonsterBase::MONSTER_RACE_DUNGEON_CONNECTOR;
}

bool StructMonster::IsAgent() const
{
	return m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_AGENT;
}

bool StructMonster::IsAutoTrap() const
{
	return m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_AUTO_TRAP;
}

bool StructMonster::IsRunaway() const
{
	return m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_RUNAWAY;
}

bool StructMonster::IsMovable()
{
	if( m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_DUNGEON_CONNECTOR ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_AUTO_TRAP ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_TRAP ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_NOT_MOVABLE ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_IMMORTAL_DUMMY )
		return false;

	return StructCreature::IsMovable();
}

bool StructMonster::IsKnockbackable()
{
	if( m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_DUNGEON_CONNECTOR ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_AUTO_TRAP ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_TRAP ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_NOT_MOVABLE ||
		m_pContentInfo->fight_type == MonsterBase::FIGHT_TYPE_IMMORTAL_DUMMY ||
		IsBossMonster() )
		return false;

	return StructCreature::IsKnockbackable();
}

bool StructMonster::IsSkillCastable()
{
	if( !IsActable() || GetState( StructState::MUTE ) ) return false;
	if( IsSitDown() ) return false;
	if( IsUsingSkill() ) return false;
	return m_StatusFlag.IsOn( STATUS_SKILL_CASTABLE );
}

bool StructMonster::IsMagicCastable()
{
	if( !IsActable() || GetState( StructState::MUTE ) ) return false;
	if( IsSitDown() ) return false;
	if( IsUsingSkill() ) return false;
	return m_StatusFlag.IsOn( STATUS_MAGIC_CASTABLE );
}

bool StructMonster::IsBossMonster() const
{
	if( m_pContentInfo->monster_type >= MonsterBase::MONSTER_TYPE_HIGH_3_STAR )
		return true;

	return false;
}

bool StructMonster::IsDungeonCore() const
{
	return ( m_pContentInfo->race == MonsterBase::MONSTER_RACE_DUNGEON_CORE );
}

AR_UNIT StructMonster::GetChaseRange() const
{
	return m_pContentInfo->chase_range * GameRule::DEFAULT_UNIT_SIZE;
}
