#include <mmo/ArcadiaServer.h>

#include <toolkit/XEnv.h>
#include <toolkit/XConsole.h>
#include <toolkit/XRandom.h>
#include <logging/FileLog.h>
#include <toolkit/XSTLUtil.h>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "PartyManager.h"
#include "GuildManager.h"
#include "StructSkill.h"
#include "StructBase.h"
#include "GameContent.h"
#include "GameProc.h"
#include "NPCProc.h"
#include "GameAllocator.h"
#include "GameMessage.h"
#include "SendMessage.h"
#include "StructMonster.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructSkillProp.h"
#include "ChannelManager.h"
#include "DungeonManager.h"
#include "HuntaholicManager.h"
#include "InstanceDungeonManager.h"
#include "BattleArenaManager.h"
#include "StructNPC.h"
#include "StructFieldProp.h"
#include "DB_Commands.h"
#include "StructWorldLocation.h"
#include "Extern.h"

const int PREDICTION_AIMING_TIME = 200;		// 예측사격


StructSkill * StructSkill::AllocSkill( struct StructCreature * pOwner, int uid, int id )
{
	StructSkill *pObj = allocSkillStruct();
	if( !pObj )
		return NULL;

	new (pObj) StructSkill( pOwner, uid, id );

	return pObj;
}

void StructSkill::FreeSkill()
{
	prepareFreeSkillStruct( this );
	StructSkill::~StructSkill();
	freeSkillStruct( this );
}

// Add Skill Result Functions
inline void AddSkillDamageResult( std::vector< SkillResult > * pvList, unsigned char type, char damage_type, const StructCreature::_DAMAGE_INFO & damage_info, AR_HANDLE handle )
{
	SkillResult skill_result;

	skill_result.damage.type = type;
	skill_result.damage.damage_type = damage_type;

	skill_result.damage.flag = 0;
	if( damage_info.bCritical	)	skill_result.damage.flag |= SkillResult::CRITICAL;
	if( damage_info.bBlock		)	skill_result.damage.flag |= SkillResult::BLOCK;
	if( damage_info.bMiss		)	skill_result.damage.flag |= SkillResult::MISS;
	if( damage_info.bPerfectBlock)	skill_result.damage.flag |= SkillResult::PERFECT_BLOCK;

	skill_result.damage.damage = damage_info.nDamage;
	skill_result.damage.hTarget = handle;
	skill_result.damage.target_hp = damage_info.target_hp;

	skill_result.damage.elemental_damage[0] = damage_info.elemental_damage[0];
	skill_result.damage.elemental_damage[1] = damage_info.elemental_damage[1];
	skill_result.damage.elemental_damage[2] = damage_info.elemental_damage[2];
	skill_result.damage.elemental_damage[3] = damage_info.elemental_damage[3];
	skill_result.damage.elemental_damage[4] = damage_info.elemental_damage[4];
	skill_result.damage.elemental_damage[5] = damage_info.elemental_damage[5];
	skill_result.damage.elemental_damage[6] = damage_info.elemental_damage[6];

	pvList->push_back( skill_result );
}

inline void AddSkillResult( std::vector< SkillResult > * pvList, bool bIsSuccess, int nSuccessType , AR_HANDLE handle )
{
	SkillResult skill_result;

	skill_result.result.type = SkillResult::RESULT;
	skill_result.result.bResult = bIsSuccess;
	skill_result.result.hTarget = handle;
	skill_result.result.success_type = nSuccessType;

	pvList->push_back( skill_result );
}

inline void AddSkillDamageWithKnockBackResult( std::vector< SkillResult > * pvList, unsigned char type, char damage_type, const StructCreature::_DAMAGE_INFO & damage_info, AR_HANDLE handle, AR_UNIT x, AR_UNIT y, AR_TIME knock_back_time )
{
	SkillResult skill_result;

	skill_result.damage_kb.type = type;
	skill_result.damage_kb.damage_type = damage_type;

	skill_result.damage_kb.flag = 0;
	if( damage_info.bCritical	)	skill_result.damage_kb.flag |= SkillResult::CRITICAL;
	if( damage_info.bBlock		)	skill_result.damage_kb.flag |= SkillResult::BLOCK;
	if( damage_info.bMiss		)	skill_result.damage_kb.flag |= SkillResult::MISS;
	if( damage_info.bPerfectBlock)	skill_result.damage_kb.flag |= SkillResult::PERFECT_BLOCK;

	skill_result.damage_kb.damage = damage_info.nDamage;
	skill_result.damage_kb.hTarget = handle;
	skill_result.damage_kb.target_hp = damage_info.target_hp;

	skill_result.damage_kb.elemental_damage[0] = damage_info.elemental_damage[0];
	skill_result.damage_kb.elemental_damage[1] = damage_info.elemental_damage[1];
	skill_result.damage_kb.elemental_damage[2] = damage_info.elemental_damage[2];
	skill_result.damage_kb.elemental_damage[3] = damage_info.elemental_damage[3];
	skill_result.damage_kb.elemental_damage[4] = damage_info.elemental_damage[4];
	skill_result.damage_kb.elemental_damage[5] = damage_info.elemental_damage[5];
	skill_result.damage_kb.elemental_damage[6] = damage_info.elemental_damage[6];

	skill_result.damage_kb.x = x;
	skill_result.damage_kb.y = y;
	skill_result.damage_kb.knock_back_time = knock_back_time;
	skill_result.damage_kb.speed = GameRule::DEFAULT_KNOCK_BACK_SPEED;

	pvList->push_back( skill_result );
}

inline void AddChainSkillDamageResult( std::vector< SkillResult > * pvList, unsigned char type, char damage_type, const StructCreature::_DAMAGE_INFO & damage_info, AR_HANDLE handle, AR_HANDLE from_handle )
{
	SkillResult skill_result;

	skill_result.damage.type = type;
	skill_result.damage.damage_type = damage_type;

	skill_result.damage.flag = 0;
	if( damage_info.bCritical	)	skill_result.damage.flag |= SkillResult::CRITICAL;
	if( damage_info.bBlock		)	skill_result.damage.flag |= SkillResult::BLOCK;
	if( damage_info.bMiss		)	skill_result.damage.flag |= SkillResult::MISS;
	if( damage_info.bPerfectBlock)	skill_result.damage.flag |= SkillResult::PERFECT_BLOCK;

	skill_result.damage.damage = damage_info.nDamage;
	skill_result.damage.hTarget = handle;
	skill_result.damage.target_hp = damage_info.target_hp;

	skill_result.damage.elemental_damage[0] = damage_info.elemental_damage[0];
	skill_result.damage.elemental_damage[1] = damage_info.elemental_damage[1];
	skill_result.damage.elemental_damage[2] = damage_info.elemental_damage[2];
	skill_result.damage.elemental_damage[3] = damage_info.elemental_damage[3];
	skill_result.damage.elemental_damage[4] = damage_info.elemental_damage[4];
	skill_result.damage.elemental_damage[5] = damage_info.elemental_damage[5];
	skill_result.damage.elemental_damage[6] = damage_info.elemental_damage[6];

	skill_result.chain_damage.hFrom = from_handle;

	pvList->push_back( skill_result );
}

struct SKILL_TARGET_FUNCTOR
{
	virtual bool onCreature( StructSkill* pSkill, AR_TIME t, struct StructCreature* pCaster, struct StructCreature* pTarget ) = 0;
};

static 	int chip_limit_level[8] = { 20, 50, 80, 100, 120, 150, 180, 200 };

struct STATE_SKILL_FUNCTOR : public SKILL_TARGET_FUNCTOR
{
	STATE_SKILL_FUNCTOR( std::vector< SkillResult > * p, StructState::StateCode _nForcedCode = StructState::StateCode( 0 ) ) : pvList( p ), nForcedCode( _nForcedCode ) {}

	bool onCreature( StructSkill* pSkill, AR_TIME t, struct StructCreature* pCaster, struct StructCreature* pTarget )
	{
		bool bResult = true;

		// 나쁜 상태이상 걸때는 확률체크 한다
		if( pSkill->GetSkillBase()->IsHarmful() )
		{
			if( pSkill->GetSkillId() == StructSkill::SKILL_FORCE_CHIP || pSkill->GetSkillId() == StructSkill::SKILL_SOUL_CHIP || pSkill->GetSkillId() == StructSkill::SKILL_LUNA_CHIP )
			{
				// 포스칩 소울칩 우선 예외처리

				if( chip_limit_level[ pSkill->GetRequestedSkillLevel() - 1 ] < pTarget->GetLevel() )
				{
					int ratio = 100 - ( pTarget->GetLevel() - chip_limit_level[ pSkill->GetRequestedSkillLevel() - 1 ] ) * 10;

					if( ratio < XRandom() % 100 ) bResult = false;
				}
			}
			else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_REGION_STATE ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_BY_SELF_COST ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_REGION_STATE_BY_SELF_COST ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_BY_TARGET_TYPE ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATES_WITH_EACH_DIFF_LV ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATES_WITH_EACH_DIFF_LV_DURATION ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_STEP_BY_STEP ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_TO_CASTER_AND_TARGET ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_RANDOM_STATE ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_RANDOM_REGION_STATE )
			{
				int accuracy_bonus = 0;
				accuracy_bonus = pSkill->GetHitBonus( pSkill->GetEnhance(), pCaster->GetLevel() - pTarget->GetLevel() );
				// 실패~
				if( 50 + pCaster->GetMagicAccuracy() - pTarget->GetMagicAvoid() + accuracy_bonus < XRandom()%100 ) bResult = false;
			}
			else
			{
				if( XRandom() % 100 > pSkill->GetProbabilityOnHit( pSkill->GetRequestedSkillLevel() ) ) bResult = false;
			}

			if( bResult && XRandom() % 100 < pTarget->GetResistStateRate( static_cast< StructState::StateCode >( pSkill->GetStateId() ) ) * 100 )
			{
				bResult = false;
			}
		}

		if( bResult )
		{
			AR_TIME end_time = 0;
			int nEndTime = pSkill->GetStateSecond( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() );

			// TODO : 일단 - 인 시간은 1분으로 고정
			// -1이면 오오라
			if( nEndTime / 100 == -1 ) end_time = AR_TIME(-1);
			if( nEndTime < 0 ) end_time = t + 6000;
			else end_time = t + nEndTime;

			if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_REGION_STATE ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_BY_SELF_COST ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_REGION_STATE_BY_SELF_COST ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_TO_CASTER_AND_TARGET )
			{
				int count = 5;

				if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_BY_SELF_COST ||
					pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_REGION_STATE_BY_SELF_COST )
					count = 0;

				int nLevel = pSkill->GetStateLevel( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() );

				for( int i = -1; i < count; ++i )
				{
					StructState::StateCode code;
					if( i < 0 ) code = (StructState::StateCode)pSkill->GetStateId();
					else code = (StructState::StateCode)( (int)pSkill->GetVar( i ) );

					if( !code ) continue;

					bResult = pTarget->AddState( code, pCaster->GetHandle(), nLevel, t, end_time ) == RESULT_SUCCESS;
				}

				if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_TO_CASTER_AND_TARGET )
				{
					for( int i = 0; i < count; ++i )
					{
						StructState::StateCode code = (StructState::StateCode)( (int)pSkill->GetVar( i + count ) );

						if( !code ) continue;

						bResult = pCaster->AddState( code, pCaster->GetHandle(), nLevel, t, end_time ) == RESULT_SUCCESS;
					}
				}
			}
			else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_BY_TARGET_TYPE )
			{
				int count = 5;

				// 계열별 지속효과 부여 스킬이므로 지정 계열에 속하는지 체크
				bResult = false;
				for( int i = -1 ; i < count ; ++i )
				{
					if( pTarget->GetCreatureGroup() == pSkill->GetVar( 7 + i ) )
					{
						bResult = true;
						break;
					}
				}

				if( bResult )
				{
					int nLevel = pSkill->GetStateLevel( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() );

					for( int i = -1 ; i < count ; ++i )
					{
						StructState::StateCode code;
						if( i < 0 ) code = (StructState::StateCode)pSkill->GetStateId();
						else code = (StructState::StateCode)( (int)pSkill->GetVar( i ) );

						if( !code ) continue;

						bResult = pTarget->AddState( code, pCaster->GetHandle(), nLevel, t, end_time ) ==  RESULT_SUCCESS;
					}
				}
			}
			else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATES_WITH_EACH_DIFF_LV )
			{
				int count = 3;

				for( int i = -1 ; i < count ; ++i )
				{
					StructState::StateCode code = (StructState::StateCode)0;
					int nLevel = 0;
					if( i < 0 )
					{
						code = (StructState::StateCode)pSkill->GetStateId();
						nLevel = pSkill->GetStateLevel( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() );
					}
					else
					{
						code = (StructState::StateCode)( (int)pSkill->GetVar( i ) );
						nLevel = pSkill->GetVar( 3 + ( i * 3 ) ) + ( pSkill->GetVar( 3 + ( i * 3 ) + 1 ) * pSkill->GetRequestedSkillLevel() ) + ( pSkill->GetVar( 3 + ( i * 3 ) + 2 ) * pSkill->GetEnhance() );
					}

					if( !code || !nLevel ) continue;

					bResult = pTarget->AddState( code, pCaster->GetHandle(), nLevel, t, end_time ) == RESULT_SUCCESS;
				}
			}
			else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATES_WITH_EACH_DIFF_LV_DURATION )
			{
				for( int i = 0 ; i < 2 ; ++i )
				{
					StructState::StateCode code = (StructState::StateCode)0;
					int nLevel = 0;
					if( i < 0 )
					{
						code = (StructState::StateCode)pSkill->GetStateId();
						nLevel = pSkill->GetStateLevel( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() );
					}
					else
					{
						code = (StructState::StateCode)((int)pSkill->GetVar(0));
						nLevel = pSkill->GetVar(1) + ( pSkill->GetVar(2) * pSkill->GetRequestedSkillLevel() ) + ( pSkill->GetVar(3) * pSkill->GetEnhance() );
						end_time = t + 100 * ( pSkill->GetVar(4) + ( pSkill->GetVar(5) * pSkill->GetRequestedSkillLevel() ) + ( pSkill->GetVar(6) * pSkill->GetEnhance() ) );
					}

					if( !code || !nLevel || end_time <= t ) continue;

					bResult = pTarget->AddState( code, pCaster->GetHandle(), nLevel, t, end_time ) == RESULT_SUCCESS;
				}
			}
			else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_STEP_BY_STEP )
			{
				bResult = false;
				end_time = t + 100 * ( pSkill->GetVar(15) + pSkill->GetVar(16) * pSkill->GetRequestedSkillLevel() + pSkill->GetVar(17) * pSkill->GetEnhance() );

				int i;

				for( i = 9 ; i >= 0; i-=3 )
					if( pSkill->GetVar(i) != 0 )
						break;

				for( i-=3; i >= 0; i-=3 )
				{
					// 순차적이므로 데이터가 무조건 차 있어야 한다.
					if( pSkill->GetVar( i ) == 0 ) assert( 0 );

					StructState::StateCode code = static_cast< StructState::StateCode >( static_cast< int >( pSkill->GetVar(i) ) );
					if( pTarget->GetState( code ) )
					{
						if( pSkill->GetVar(i+3) == 0 ) assert( 0 );

						if( pSkill->GetVar(i+1) + pSkill->GetVar(i+2) * pSkill->GetRequestedSkillLevel() > XRandom( 0, 99 ) )
						{
							bResult = pTarget->AddState( static_cast< StructState::StateCode >( static_cast< int >( pSkill->GetVar(i+3) ) ), pCaster->GetHandle(), pSkill->GetRequestedSkillLevel(), t, end_time ) == RESULT_SUCCESS;
							if( bResult ) pTarget->RemoveState( code, GameRule::MAX_STATE_LEVEL );
						}
						else
							bResult = false;

						break;
					}
				}

				// 어떠한 버프도 안 걸려있다면 첫번째로 부여될 버프를 건다.
				if( i < 0 )
				{
					if( pSkill->GetVar(1) + pSkill->GetVar(2) * pSkill->GetRequestedSkillLevel() > XRandom( 0, 99 ) )
						bResult = pTarget->AddState( static_cast< StructState::StateCode >( static_cast< int >( pSkill->GetVar(0) ) ),
						pCaster->GetHandle(), pSkill->GetRequestedSkillLevel(), t, end_time ) == RESULT_SUCCESS;
					else
						bResult = false;
				}
			}
			else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_RANDOM_STATE ||
				pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_RANDOM_REGION_STATE )
			{
				StructState::StateCode code = static_cast< StructState::StateCode >( pSkill->GetStateId() );
				int nLevel = pSkill->GetStateLevel( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() );

				if( nForcedCode )
				{
					code = nForcedCode;
				}
				else
				{
					int count;
					for( count = 6; count >= 2; --count )	if( pSkill->GetVar( count - 2 ) ) break;

					int index = XRandom( -1, count - 2 );
					if( index != -1 ) code = static_cast< StructState::StateCode >( static_cast< int >( pSkill->GetVar(index) ) );
				}

				bResult = pTarget->AddState( code, pCaster->GetHandle(), nLevel, t, end_time ) ==  RESULT_SUCCESS;
			}
			else if( pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_SINGLE_DAMAGE_ADD_RANDOM_STATE || pSkill->GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE_ADD_RANDOM_STATE)
			{
				std::vector< int > vStateID;
				vStateID.reserve( 7 );
				vStateID.push_back( pSkill->GetStateId() );
				for( int i = 0 ; i < 6 ; ++i )
				{
					if( pSkill->GetVar( 6 + i ) )
						vStateID.push_back( pSkill->GetVar( 6 + i ) );
				}

				if( !vStateID.empty() )
				{
					StructState::StateCode code = static_cast< StructState::StateCode >( vStateID[ XRandom( 0, static_cast< int >( vStateID.size() ) - 1 ) ] );

					bResult = pTarget->AddState( code,
						pCaster->GetHandle(), pSkill->GetStateLevel( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() ),
						t, end_time ) == RESULT_SUCCESS;
				}
			}
			else
			{
				if( pSkill->GetSkillBase()->GetEffectType() != SkillBase::EF_TOGGLE_AURA &&
					pSkill->GetSkillBase()->GetEffectType() != SkillBase::EF_TOGGLE_DIFFERENTIAL_AURA )
				{
					// 아래 유형의 스킬들은 자기 자신에게는 지속효과를 걸면 안 됨
					// 흡수 등을 위해 스킬 타겟에 자신이 포함되지만 지속효과의 부여 대상은 아닌 경우
					if( pTarget != pCaster || (
						pSkill->GetSkillBase()->GetSkillEffectType() != SkillBase::EF_PHYSICAL_ABSORB_DAMAGE &&
						pSkill->GetSkillBase()->GetSkillEffectType() != SkillBase::EF_MAGIC_ABSORB_DAMAGE_OLD &&
						pSkill->GetSkillBase()->GetSkillEffectType() != SkillBase::EF_MAGIC_DAMAGE_WITH_ABSORB_HP_MP &&
						pSkill->GetSkillBase()->GetSkillEffectType() != SkillBase::EF_ADD_HP_MP_BY_ABSORB_HP_MP &&
						pSkill->GetSkillBase()->GetSkillEffectType() != SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_ABSORB ) )
					{
						bResult = pTarget->AddState( (StructState::StateCode)pSkill->GetStateId(),
							pCaster->GetHandle(), pSkill->GetStateLevel( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() ),
							t, end_time ) == RESULT_SUCCESS;
					}
				}
			}

			if( pSkill->GetSkillId() != StructSkill::SKILL_FORCE_CHIP && pSkill->GetSkillId() != StructSkill::SKILL_SOUL_CHIP && pSkill->GetSkillId() != StructSkill::SKILL_LUNA_CHIP )
			{
				if( pTarget->IsMonster() && pTarget->IsEnemy( pCaster, true ) )
				{
					StructMonster * pMonster = static_cast< StructMonster *>( pTarget );

					pMonster->AddHate( pCaster->GetHandle(), 1 );
				}
				else if( pTarget->IsNPC() && pTarget->IsEnemy( pCaster ) )
				{
					StructNPC * pNPC = static_cast< StructNPC *>( pTarget );

					pNPC->SetAttacker( pCaster );
				}
			}
		}

		AddSkillResult( pvList, bResult, SkillResult::ADD_STATE, pTarget->GetHandle() );

		return true;
	}

	std::vector< SkillResult > * pvList;
	StructState::StateCode nForcedCode;
};

struct REMOVE_GOOD_STATE_SKILL_FUNCTOR : public SKILL_TARGET_FUNCTOR
{
	REMOVE_GOOD_STATE_SKILL_FUNCTOR( std::vector< SkillResult > * p ) : pvList( p ) {}

	bool onCreature( StructSkill* pSkill, AR_TIME t, struct StructCreature* pCaster, struct StructCreature* pTarget )
	{
		int nStateLevel = pSkill->GetVar(1) * pSkill->GetRequestedSkillLevel() + pSkill->GetVar(2) * pSkill->GetEnhance();

		int bResult = true;
		int accuracy_bonus = pSkill->GetVar(9) + pSkill->GetVar(10) * pSkill->GetRequestedSkillLevel();
		// 실패~
		if( accuracy_bonus <= XRandom()%100 ) bResult = false;

		if( pSkill->GetVar( 8 ) )
		{
			if( bResult ) pTarget->RemoveGoodState( nStateLevel );
			AddSkillResult( pvList, bResult, SkillResult::REMOVE_STATE, pTarget->GetHandle() );
		}
		else
		{
			if( bResult ) pTarget->RemoveState( (StructState::StateCode)((int)pSkill->GetVar(0)), nStateLevel );
			AddSkillResult( pvList, bResult, SkillResult::REMOVE_STATE, pTarget->GetHandle() );

			for( int nIndex = 3 ; nIndex < 8 && pSkill->GetVar(nIndex) ; nIndex++ )
			{
				if( bResult ) pTarget->RemoveState( (StructState::StateCode)(int)pSkill->GetVar(nIndex), nStateLevel );
				AddSkillResult( pvList, bResult, SkillResult::REMOVE_STATE, pTarget->GetHandle() );
			}
		}

		return true;
	}

	std::vector< SkillResult > * pvList;
};

struct REMOVE_BAD_STATE_SKILL_FUNCTOR : public SKILL_TARGET_FUNCTOR
{
	REMOVE_BAD_STATE_SKILL_FUNCTOR( std::vector< SkillResult > * p ) : pvList( p ) {}

	bool onCreature( StructSkill* pSkill, AR_TIME t, struct StructCreature* pCaster, struct StructCreature* pTarget )
	{
		int nStateLevel = pSkill->GetVar(1) + pSkill->GetVar(2) * pSkill->GetRequestedSkillLevel() + pSkill->GetVar(3) * pSkill->GetEnhance();
		pTarget->RemoveState( (StructState::StateCode)(int)pSkill->GetVar(0), nStateLevel );
		AddSkillResult( pvList, true, SkillResult::REMOVE_STATE, pTarget->GetHandle() );


		for( int nIndex = 4 ; nIndex < 9 && pSkill->GetVar(nIndex) ; nIndex++ )
		{
			pTarget->RemoveState( (StructState::StateCode)(int)pSkill->GetVar(nIndex), nStateLevel );
			AddSkillResult( pvList, true, SkillResult::REMOVE_STATE, pTarget->GetHandle() );
		}

		return true;
	}

	std::vector< SkillResult > * pvList;
};

struct REMOVE_STATE_GROUP_SKILL_FUNCTOR : public SKILL_TARGET_FUNCTOR
{
	REMOVE_STATE_GROUP_SKILL_FUNCTOR( std::vector< SkillResult > * p, StructCreature * _pOwner ) : pvList( p ), pOwner( _pOwner ), nGroupID( 0 ) {}

	bool onCreature( StructSkill* pSkill, AR_TIME t, struct StructCreature* pCaster, struct StructCreature* pTarget )
	{
		int nRatio = pSkill->GetVar(0) + pSkill->GetVar(1) * pSkill->GetRequestedSkillLevel() + pSkill->GetVar(10) * pSkill->GetEnhance();

		if( XRandom( 1, 100 ) > nRatio )
			return false;

		int nRemoveCount = pSkill->GetVar(3) + pSkill->GetVar(4) * pSkill->GetRequestedSkillLevel() + pSkill->GetVar(11) * pSkill->GetEnhance();
		if( nRemoveCount <= 0 )
			return false;

		int nGroupList[5] = { pSkill->GetVar(5), pSkill->GetVar(6), pSkill->GetVar(7), pSkill->GetVar(8), pSkill->GetVar(9) };

		typedef std::pair< StructState::StateCode, AR_TIME > REMOVE_INFO;
		std::vector< REMOVE_INFO > vRemoveInfoList;

		const std::vector< StructState > &vStateList = pTarget->GetStateList();
		for( std::vector< StructState >::const_iterator it = vStateList.begin() ; it != vStateList.end() && nRemoveCount > static_cast< int >( vRemoveInfoList.size() ) ; ++it )
		{
			if( ( nGroupList[0] && (*it).GetStateGroup() == nGroupList[0] ) || ( nGroupList[1] && (*it).GetStateGroup() == nGroupList[1] ) || ( nGroupList[2] && (*it).GetStateGroup() == nGroupList[2] ) ||
				( nGroupList[3] && (*it).GetStateGroup() == nGroupList[3] ) || ( nGroupList[4] && (*it).GetStateGroup() == nGroupList[4] ) )
			{
				if( (*it).GetTimeType() & StateInfo::ERASE_ON_DEAD )
					vRemoveInfoList.push_back( REMOVE_INFO( (*it).GetCode(), (*it).GetEndTime() ) );
			}
		}

		if( nRemoveCount < static_cast< int >( vRemoveInfoList.size() ) )
		{
			if( pSkill->GetVar(2) == 0 )
			{
				std::random_shuffle( vRemoveInfoList.begin(), vRemoveInfoList.end() );
			}
			else if( pSkill->GetVar(2) == 1 )
			{
				struct RemoveInfoCompare
				{
					bool operator()( REMOVE_INFO &RemoveInfo1, REMOVE_INFO &RemoveInfo2 )
					{
						return RemoveInfo2.second > RemoveInfo1.second;
					}
				};

				std::sort( vRemoveInfoList.begin(), vRemoveInfoList.end(), RemoveInfoCompare() );
			}
			else if( pSkill->GetVar(2) == 2 )
			{
				struct RemoveInfoCompare
				{
					bool operator()( REMOVE_INFO &RemoveInfo1, REMOVE_INFO &RemoveInfo2 )
					{
						return RemoveInfo2.second <= RemoveInfo1.second;
					}
				};

				std::sort( vRemoveInfoList.begin(), vRemoveInfoList.end(), RemoveInfoCompare() );
			}

			vRemoveInfoList.erase( vRemoveInfoList.begin() + nRemoveCount, vRemoveInfoList.end() );
		}

		for( std::vector< REMOVE_INFO >::iterator rit = vRemoveInfoList.begin() ; rit != vRemoveInfoList.end() ; ++rit )
		{
			pTarget->RemoveState( (*rit).first );
		}

		AR_TIME t = GetArTime();

		AR_TIME end_time = 0;

		int nEndTime = pSkill->GetStateSecond( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() );

		if( nEndTime < 0 ) end_time = t + 6000;
		else end_time = t + nEndTime;

		if( pSkill->GetStateId() )
		{
			pTarget->AddState( (StructState::StateCode)pSkill->GetStateId(), 
				pOwner->GetHandle(), 
				pSkill->GetStateLevel( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() ), 
				t, end_time, true );
		}

		for( int i = 12; i < 15; ++i )
		{
			if( pSkill->GetVar(i) )
			{
				pTarget->AddState( (StructState::StateCode) (int) pSkill->GetVar(i), 
					pOwner->GetHandle(), 
					pSkill->GetStateLevel( pSkill->GetRequestedSkillLevel(), pSkill->GetEnhance() ), 
					t, end_time, true );
			}
		}

		return true;
	}

	std::vector< SkillResult > *	pvList;
	StructCreature *				pOwner;
	int nGroupID;
};

struct HEALING_SKILL_FUNCTOR : public SKILL_TARGET_FUNCTOR
{
	HEALING_SKILL_FUNCTOR( std::vector< SkillResult > * p, bool bIsByItem = false ) : pvList( p ), m_bIsByItem( bIsByItem ), nResult( 0 ) {}

	bool onCreature( StructSkill* pSkill, AR_TIME t, struct StructCreature* pCaster, struct StructCreature* pTarget )
	{
		int slv = pSkill->GetRequestedSkillLevel();
		nResult =
			pCaster->GetMagicPoint( (Elemental::Type)pSkill->GetSkillBase()->GetElementalType(), pSkill->GetSkillBase()->IsPhysicalSkill(), pSkill->GetSkillBase()->IsHarmful() ) * ( pSkill->GetVar(0) + pSkill->GetVar(1) * slv )
			+ pSkill->GetVar(2) + pSkill->GetVar(3) * slv
			+ pSkill->GetVar(6) * pSkill->GetEnhance()
			+ pTarget->GetMaxHP() * ( pSkill->GetVar(4) + pSkill->GetVar(5) * slv + pSkill->GetVar(7) * pSkill->GetEnhance() );

		if( pTarget->IsDead() ) nResult = 0;
		else if( m_bIsByItem )
			nResult = pTarget->HealByItem( nResult );
		else
			nResult = pTarget->Heal( nResult );

		SkillResult skill_result;

		skill_result.add_hp.type = SkillResult::ADD_HP;
		skill_result.add_hp.hTarget = pTarget->GetHandle();
		skill_result.add_hp.nIncHP = nResult;
		skill_result.add_hp.target_hp = pTarget->GetHP();

		pvList->push_back( skill_result );

		return true;
	}

	std::vector< SkillResult > * pvList;

	int nResult;
	bool m_bIsByItem;
};

struct RECOVERY_MP_SKILL_FUNCTOR : public SKILL_TARGET_FUNCTOR
{
	RECOVERY_MP_SKILL_FUNCTOR( std::vector< SkillResult > * p, bool bIsByItem = false ) : pvList( p ), m_bIsByItem( bIsByItem ), nResult( 0 ) {}

	bool onCreature( StructSkill* pSkill, AR_TIME t, struct StructCreature* pCaster, struct StructCreature* pTarget )
	{
		int slv = pSkill->GetRequestedSkillLevel();
		int enhance = pSkill->GetEnhance();
		nResult =
			pCaster->GetMagicPoint((Elemental::Type)pSkill->GetSkillBase()->GetElementalType(), pSkill->GetSkillBase()->IsPhysicalSkill(), pSkill->GetSkillBase()->IsHarmful() ) * ( pSkill->GetVar(0) + slv*pSkill->GetVar(1) )
			+ pSkill->GetVar(2) + slv*pSkill->GetVar(3) + enhance*pSkill->GetVar( 6 ) +
			+ pTarget->GetMaxMP() * ( pSkill->GetVar(4) + slv*pSkill->GetVar(5)  + enhance*pSkill->GetVar(7) );

		if( pTarget->IsDead() ) nResult = 0;
		else if( m_bIsByItem )
			nResult = pTarget->MPHealByItem( nResult );
		else
			nResult = pTarget->MPHeal( nResult );

		SkillResult skill_result;

		skill_result.add_hp.type = SkillResult::ADD_MP;
		skill_result.add_hp.hTarget = pTarget->GetHandle();
		skill_result.add_hp.nIncHP = nResult;
		skill_result.add_hp.target_hp = pTarget->GetMP();

		pvList->push_back( skill_result );

		return true;
	}

	std::vector< SkillResult > * pvList;

	int nResult;
	bool m_bIsByItem;
};

// Struct Skill Members

StructSkill::StructSkill( struct StructCreature* pOwner, int uid, int id )
{
	m_pOwner = pOwner;

	m_fRushFace = 0;
	m_nRushDamage = 0;
	m_targetLayer = 0;

	m_nSkillUID = uid;
	m_nSkillId = id;

	m_nEnhance = 0;
	m_nSkillLevel= 0;
	m_nSkillLevelAdd = 0;
	m_bNeedUpdateToDB = false;
	m_bMultiple	= false;
	m_hCastItem = 0;

	m_nAuraMPDecTime = 0;

	m_nNextCoolTime = 0;
	m_nDBCoolTime = 0;

	m_nAuraRefreshTime = 0;

	m_Modifier = NULL;

	bindSkillInfo( id );

	Init();

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructSkill" );
#endif
}

StructSkill::~StructSkill()
{
#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructSkill" );
#endif
}

StructSkill::StructSkill( const StructSkill & rhs )
{
	m_pOwner = rhs.m_pOwner;

	m_nSkillUID = rhs.m_nSkillUID;
	m_nSkillId = rhs.m_nSkillId;

	m_fRushFace = 0;
	m_nRushDamage = 0;

	m_nEnhance = rhs.m_nEnhance;
	m_nSkillLevel = rhs.m_nSkillLevel;
	m_nSkillLevelAdd = rhs.m_nSkillLevelAdd;
	m_pSkillBase = rhs.m_pSkillBase;
	m_bMultiple = rhs.m_bMultiple;
	m_hCastItem = rhs.m_hCastItem;

	m_nRequestedSkillLevel = rhs.m_nRequestedSkillLevel;

	m_bNeedUpdateToDB = false;
	m_nAuraMPDecTime = 0;

	m_nNextCoolTime = 0;
	m_nDBCoolTime = 0;

	m_nErrorCode = RESULT_SUCCESS;
	m_Status = STATUS_IDLE;
	m_nCastTime = 0;
	m_nCastingDelay = 0;
	m_nFireTime = 0;

	m_nCurrentFire = 0;
	m_nTotalFire = 0;
	m_nTargetCount = 1;
	m_nFireCount = 1;

	m_Modifier = NULL;

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructSkill", false );
#endif
}

const bool StructSkill::Cancel()
{
	if( IsPassiveSkill() ) return false;

	switch( m_Status )
	{
	case STATUS_CAST:
		{
			if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ACTIVATE_FIELD_PROP ||
				GetSkillBase()->GetSkillEffectType() == SkillBase::EF_REGION_HEAL_BY_FIELD_PROP ||
				GetSkillBase()->GetSkillEffectType() == SkillBase::EF_AREA_EFFECT_HEAL_BY_FIELD_PROP )
			{
				StructFieldProp::iterator it = StructFieldProp::get( m_hTarget );

				StructFieldProp * pProp = static_cast< StructFieldProp * >( *it );

				if( pProp )
					pProp->CancelCast();
			}

			broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), m_pOwner->GetLayer(), 0, 0, TS_SC_SKILL::CANCEL );

			break;
		}

	case STATUS_COMPLETE:
	case STATUS_FIRE:
		{
			if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ACTIVATE_FIELD_PROP ||
				GetSkillBase()->GetSkillEffectType() == SkillBase::EF_REGION_HEAL_BY_FIELD_PROP ||
				GetSkillBase()->GetSkillEffectType() == SkillBase::EF_AREA_EFFECT_HEAL_BY_FIELD_PROP )
			{
				StructFieldProp::iterator it = StructFieldProp::get( m_hTarget );

				StructFieldProp * pProp = static_cast< StructFieldProp * >( *it );

				if( pProp )
					pProp->CancelCast();
			}

			broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), m_pOwner->GetLayer(), 0, 0, TS_SC_SKILL::COMPLETE );

			break;
		}

	default:
		{
			assert( 0 );
			break;
		}
	}

	Init();

	// 펫 삽질의 경우 삽질 상태 초기화
	if( m_pOwner->IsPet() && GetSkillId() == SKILL_SHOVELING )
	{
		static_cast< StructPet * >( m_pOwner )->SetShovelingStatus( StructPet::SHOVELING_STATUS_IDLE );
		BroadcastStatusMessage( m_pOwner );
	}

	return true;
}

const bool StructSkill::IsRandomSkill() const
{
	// 현재 이 루틴은 소환수에만 적용됨 (일반 캐릭터는 랜덤 스킬을 배울 수 없음.)
	if( !m_pOwner->IsSummon() )
		return false;

	// 크리쳐가 진화를 한 후 강화를 하면 일반형으로 돌아오는 데 이때 예전 진화했을 때 배웠던 스킬을 기억하려면 
	// 이 크리쳐가 진화가능한 3종의 종류 스킬트리를 다 찾아야 한다.
	StructSummon * pSummon = static_cast < StructSummon * >( m_pOwner );
	int SummonID[3];

	if( pSummon->GetTransformLevel() == SummonBase::EVOLVE_NORMAL )
	{
		SummonID[0] = pSummon->GetJobId();
		SummonID[1] = pSummon->GetEvolveTarget();
		SummonID[2] = GameContent::GetSummonInfo( pSummon->GetEvolveTarget() )->evolve_target;
	}
	else if( pSummon->GetTransformLevel() == SummonBase::EVOLVE_GROWTH )
	{
		SummonID[0] = pSummon->GetPrevJobId( 0 );
		SummonID[1] = pSummon->GetJobId();
		SummonID[2] = pSummon->GetEvolveTarget();
	}
	else if( pSummon->GetTransformLevel() == SummonBase::EVOLVE_EVOLVE )
	{
		SummonID[0] = pSummon->GetPrevJobId( 0 );
		SummonID[1] = pSummon->GetPrevJobId( 1 );
		SummonID[2] = pSummon->GetJobId();
	}
	else
	{
		_cprint( "Invalid Summon Info on Summon[%d]\n", pSummon->GetSummonCode() );
		FILELOG( "Invalid Summon Info on Summon[%d]", pSummon->GetSummonCode() );
	}

	// 일반, 성장, 진화형에 대한 스킬트리를 전체 검사
	for( int i = 0 ; i < SummonBase::EVOLVE_EVOLVE ; ++i )
	{
		SummonBase *pBase = GameContent::GetSummonInfo( SummonID[i] );
		if( !pBase )
			continue;

		for( int j = 0 ; j < GameRule::MAX_SUMMON_SKILL_TREE ; ++j )
		{
			if( !pBase->skill_tree[j] )
				continue;

			for( std::vector< SkillTree >::iterator it = (*pBase->skill_tree[j]).begin() ; it != (*pBase->skill_tree[j]).end() ; ++it )
			{
				if( it->skill_group_id )
				{
					for( int i = 0 ; i < GameRule::MAX_RANDOM_SKILL_COUNT ; ++i )
					{
						int nSkillID = GameContent::GetRandomSkillIDFromGroupID( it->skill_group_id, i );
						if( !nSkillID )
							break;
						if( nSkillID == m_nSkillId )
							return true;
					}
				}
			}
		}
	}

	return false;
}

AR_TIME StructSkill::GetSkillCoolTime() const
{
	AR_TIME nCoolTime = GetCoolTime( GetRequestedSkillLevel(), ( m_pOwner->IsSummon() ) ? GetRequestedSkillLevel() : GetEnhance() );
	nCoolTime *= m_pOwner->GetCoolTimeMod( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	return nCoolTime * m_pOwner->GetCoolTimeSpeed();
}

void StructSkill::assembleMessage( struct TS_SC_SKILL *pResultMsg, int buf_len, int nType, int cost_hp, int cost_mp )
{
	char *pResultData = reinterpret_cast< char* >( pResultMsg+1 );
	buf_len -= sizeof( *pResultMsg );

	pResultMsg->type		= nType;
	pResultMsg->layer		= m_targetLayer;
	pResultMsg->x			= m_targetPos.x;
	pResultMsg->y			= m_targetPos.y;
	pResultMsg->z			= m_targetPos.z;	
	pResultMsg->caster		= m_pOwner->GetHandle();
	pResultMsg->skill_id	= GetSkillId();
	pResultMsg->skill_level	= GetRequestedSkillLevel();
	pResultMsg->target		= m_hTarget;
	pResultMsg->hp_cost		= cost_hp;
	pResultMsg->mp_cost		= cost_mp;
	pResultMsg->caster_hp	= m_pOwner->GetHP();
	pResultMsg->caster_mp	= m_pOwner->GetMP();

	switch( nType )
	{
	case TS_SC_SKILL::CASTING:
	case TS_SC_SKILL::CASTING_UPDATE:
		{
			pResultMsg->cast.tm = GetCastingTime();
			pResultMsg->cast.nErrorCode = m_nErrorCode;
			break;
		}
	case TS_SC_SKILL::CANCEL:
	case TS_SC_SKILL::COMPLETE:
		// 할 것 없음
		break;
	case TS_SC_SKILL::FIRE:
	case TS_SC_SKILL::REGION_FIRE:
		{
			pResultMsg->fire.bMultiple = m_bMultiple;
			pResultMsg->fire.target_count = m_nTargetCount;
			pResultMsg->fire.fire_count = m_nFireCount;
			pResultMsg->fire.count = static_cast< unsigned short >( m_vResultList.size() );
			pResultMsg->fire.range = m_fRange;

			if( pResultMsg->fire.count )
			{
				std::vector< SkillResult >::iterator it;
				for( it = m_vResultList.begin(); it != m_vResultList.end(); ++it )
				{
					switch( (*it).GetType() )
					{
					case SkillResult::DAMAGE:
					case SkillResult::MAGIC_DAMAGE:
						s_memcpy( pResultData, buf_len, &(*it), sizeof( SkillResult::DamageType ) );
						pResultData += sizeof( SkillResult::DamageType );
						buf_len -= sizeof( SkillResult::DamageType );
						break;

					case SkillResult::DAMAGE_WITH_KNOCK_BACK:
						s_memcpy( pResultData, buf_len, &(*it), sizeof( SkillResult::DamageWithKnockBackType ) );
						pResultData += sizeof( SkillResult::DamageWithKnockBackType );
						buf_len -= sizeof( SkillResult::DamageWithKnockBackType );
						break;

					case SkillResult::RUSH:
						s_memcpy( pResultData, buf_len, &(*it), sizeof( SkillResult::RushType ) );
						pResultData += sizeof( SkillResult::RushType );
						buf_len -= sizeof( SkillResult::RushType );
						break;

					case SkillResult::RESULT:
						s_memcpy( pResultData, buf_len, &(*it), sizeof( SkillResult::ResultType ) );
						pResultData += sizeof( SkillResult::ResultType );
						buf_len -= sizeof( SkillResult::ResultType );
						break;

					case SkillResult::ADD_HP:
					case SkillResult::ADD_MP:
						s_memcpy( pResultData, buf_len, &(*it), sizeof( SkillResult::AddHPType ) );
						pResultData += sizeof( SkillResult::AddHPType );
						buf_len -= sizeof( SkillResult::AddHPType );
						break;

					case SkillResult::REBIRTH:
						s_memcpy( pResultData, buf_len, &(*it), sizeof( SkillResult::RebirthType ) );
						pResultData += sizeof( SkillResult::RebirthType );
						buf_len -= sizeof( SkillResult::RebirthType );
						break;

					case SkillResult::ADD_HP_MP_SP:
						s_memcpy( pResultData, buf_len, &(*it), sizeof( SkillResult::AddHPMPSPType ) );
						pResultData += sizeof( SkillResult::AddHPMPSPType );
						buf_len -= sizeof( SkillResult::AddHPMPSPType );
						break;

					case SkillResult::CHAIN_DAMAGE:
					case SkillResult::CHAIN_MAGIC_DAMAGE:
						s_memcpy( pResultData, buf_len, &(*it), sizeof( SkillResult::ChainDamageType ) );
						pResultData += sizeof( SkillResult::ChainDamageType );
						buf_len -= sizeof( SkillResult::ChainDamageType );
						break;

					case SkillResult::CHAIN_HEAL:
						s_memcpy( pResultData, buf_len, &(*it), sizeof( SkillResult::ChainDamageType ) );
						pResultData += sizeof( SkillResult::ChainHealType );
						buf_len -= sizeof( SkillResult::ChainHealType );
						break;
					}
				}
			}

			pResultMsg->size += sizeof(SkillResult)*pResultMsg->fire.count;	

			break;
		};
	}
}

void StructSkill::SendSkillCastFailMessage( ArObject * pObject, AR_HANDLE caster, AR_HANDLE target, unsigned short skill_id, char skill_lv, const ArPosition & pos, int error_code )
{
	TS_SC_SKILL msg;

	msg.type		= TS_SC_SKILL::CASTING;
	msg.layer		= 0;
	msg.x			= pos.x;
	msg.y			= pos.y;
	msg.z			= pos.z;	
	msg.caster		= caster;
	msg.skill_id	= skill_id;
	msg.skill_level	= skill_lv;
	msg.target		= target;
	msg.hp_cost		= 0;
	msg.mp_cost		= 0;
	msg.caster_hp	= 0;
	msg.caster_mp	= 0;

	msg.cast.tm = 0;
	msg.cast.nErrorCode = error_code;

	PendMessage( pObject, &msg );
}

void StructSkill::sendSkillMessage( int nType )
{
	if( !m_pOwner->IsPlayer() )
		return;

	char *pBuf = NULL;
	char buf[1024];
	int buf_len = 0;
	if( sizeof(SkillResult) * 1 + sizeof( TS_SC_SKILL ) < sizeof( buf ) )
	{
		pBuf = buf;
		buf_len = sizeof( buf );
	}
	else
	{
		buf_len = sizeof(SkillResult) * 1 + sizeof( TS_SC_SKILL );
		pBuf = new char[ buf_len ];
	}

	TS_SC_SKILL *pResultMsg = new (buf) TS_SC_SKILL();

	assembleMessage( pResultMsg, buf_len, nType, 0, 0 );
	PendMessage( m_pOwner, pResultMsg );

	if( pBuf != buf )
		delete [] pBuf;
}

void StructSkill::broadcastSkillMessage( int rx, int ry, unsigned char layer, int cost_hp, int cost_mp, int nType )
{
	char *pBuf = NULL;
	char szBuf[ SENDMSG_BUFFER_SIZE ] = "";
	int buf_len = 0;
	if( sizeof( SkillResult ) * m_vResultList.size() + sizeof( TS_SC_SKILL ) < SENDMSG_BUFFER_SIZE )
	{
		pBuf = szBuf;
		buf_len = sizeof( szBuf );
	}
	else
	{
		buf_len = sizeof( SkillResult ) * static_cast< int >( m_vResultList.size() ) + sizeof( TS_SC_SKILL );
		pBuf = new char[ buf_len ];
	}

	TS_SC_SKILL *pResultMsg = new (pBuf) TS_SC_SKILL();

	assembleMessage( pResultMsg, buf_len, nType, cost_hp, cost_mp );
	ArcadiaServer::Instance().Broadcast( rx, ry, layer, pResultMsg );

	if( pBuf != szBuf )
		delete [] pBuf;
}

void StructSkill::broadcastSkillMessage( int rx1, int ry1, int rx2, int ry2, unsigned char layer, int cost_hp, int cost_mp, int nType )
{
	char *pBuf = NULL;
	char szBuf[ SENDMSG_BUFFER_SIZE ] = "";
	int buf_len = 0;
	if( sizeof( SkillResult ) * m_vResultList.size() + sizeof( TS_SC_SKILL ) < SENDMSG_BUFFER_SIZE )
	{
		pBuf = szBuf;
		buf_len = sizeof( szBuf );
	}
	else
	{
		buf_len = sizeof( SkillResult ) * static_cast< int >( m_vResultList.size() ) + sizeof( TS_SC_SKILL );
		pBuf = new char[ buf_len ];
	}

	TS_SC_SKILL *pResultMsg = new (pBuf) TS_SC_SKILL();

	assembleMessage( pResultMsg, buf_len, nType, cost_hp, cost_mp );
	ArcadiaServer::Instance().Broadcast( rx1, ry1, rx2, ry2, layer, pResultMsg );

	if( pBuf != szBuf )
		delete [] pBuf;
}

void StructSkill::broadcastSkillMessage( int rx, int ry, unsigned int range, unsigned char layer, int cost_hp, int cost_mp, int nType )
{
	char *pBuf = NULL;
	char szBuf[ SENDMSG_BUFFER_SIZE ] = "";
	int buf_len = 0;
	if( sizeof( SkillResult ) * m_vResultList.size() + sizeof( TS_SC_SKILL ) < SENDMSG_BUFFER_SIZE )
	{
		pBuf = szBuf;
		buf_len = sizeof( szBuf );
	}
	else
	{
		buf_len = sizeof( SkillResult ) * static_cast< int >( m_vResultList.size() ) + sizeof( TS_SC_SKILL );
		pBuf = new char[ buf_len ];
	}

	TS_SC_SKILL *pResultMsg = new (pBuf) TS_SC_SKILL();

	assembleMessage( pResultMsg, buf_len, nType, cost_hp, cost_mp );
	ArcadiaServer::Instance().BroadcastToSpecificRegion( rx, ry, range, layer, pResultMsg );

	if( pBuf != szBuf )
		delete [] pBuf;
}

int StructSkill::Cast( int nSkillLevel, AR_HANDLE handle, const ArPosition & pos, unsigned char layer, AR_HANDLE hCastItem )
{
	assert( m_Status == STATUS_IDLE );

	AR_TIME current_time = GetArTime();
	AR_TIME delay = -1;

	SetRequestedSkillLevel( std::min( nSkillLevel, GetCurrentSkillLevel() ) );
	m_Status = STATUS_CAST;
	m_hCastItem = hCastItem;

	int mana_cost = 0;

	do
	{
		// 스킬 쿨타임 체크
		if( !CheckCoolTime( current_time ) && !m_hCastItem )
		{
			m_nErrorCode = RESULT_COOL_TIME;
			break;
		}

		if( GetSkillBase()->IsNeedWeapon() )
		{
			// 스킬에 적합한 무기인지?
			if( IsUseableWeapon( ItemBase::CLASS_SHIELD ) )
			{
				if( !m_pOwner->IsWearShield() )
				{
					m_nErrorCode = RESULT_LIMIT_WEAPON;
					break;
				}
			}
			else if( !IsUseableWeapon( m_pOwner->GetWeaponClass() ) )
			{
				m_nErrorCode = RESULT_LIMIT_WEAPON;
				break;
			}
		}

		// 테이밍 관련 체크
		if( GetSkillId() == SKILL_CREATURE_TAMING || GetSkillId() == SKILL_ITEM_CREATURE_TAMING_SCROLL || GetSkillId() == SKILL_PET_TAMING )
		{
			if( !m_pOwner->IsPlayer() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			StructCreature::iterator itMonster = StructCreature::get( handle );
			StructCreature * pCreature = (*itMonster);

			if( !pCreature || !pCreature->IsMonster() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			StructMonster * pMonster = static_cast< StructMonster * >( pCreature );

			// 테이밍 안되는 녀석
			ItemBase::ItemCode nTameItemCode = pMonster->GetTameItemCode();
			if( !nTameItemCode )
			{
				m_nErrorCode = RESULT_NOT_TAMABLE;
				break;
			}

			// 이미 테이밍 되어 있으면 KIN
			if( pMonster->GetTamer() )
			{
				m_nErrorCode = RESULT_TARGET_ALREADY_BEING_TAMED;
				break;
			}

			// 체력이 100%가 아닌 대상은 테이밍 불가
			if( pMonster->GetHP() != pMonster->GetMaxHP() )
			{
				m_nErrorCode = RESULT_NOT_ENOUGH_TARGET_HP;
				break;
			}

			if( GetSkillId() == SKILL_CREATURE_TAMING || GetSkillId() == SKILL_ITEM_CREATURE_TAMING_SCROLL )
			{
				// 빈 카드 없으면 테이밍 불가
				if( !static_cast< StructPlayer * >( m_pOwner )->FindEmptySummonCard( nTameItemCode ) )
				{
					if( pMonster->IsMonsterCreatureTame() )
						m_nErrorCode = RESULT_NOT_ENOUGH_SOUL_TAMING_CARD;
					else
						m_nErrorCode = RESULT_NOT_ENOUGH_SUMMON_CARD;
					break;
				}

				// 이미 다른 몬스터를 테이밍 중일 경우 테이밍 불가
				AR_HANDLE hTamingTarget = static_cast< StructPlayer * >( m_pOwner )->GetTamingTarget();
				if( hTamingTarget )
				{
					m_nErrorCode = RESULT_ALREADY_TAMING;
					break;
				}
			}
			else if( GetSkillId() == SKILL_PET_TAMING )
			{
				// 환경 몬스터만 가능
				if( !pMonster->IsEnvironmentMonster() )
				{
					m_nErrorCode = RESULT_NOT_ACTABLE;
					break;
				}

				// 펫으로 테이밍 가능 환경 몬스터에게만 가능
				if( !pMonster->IsEnvironmentMonster() || !nTameItemCode || ( !pMonster->IsMonsterCreatureTame() && !pMonster->GetTameCode() ) 
					|| StructItem::GetItemBase( nTameItemCode ).nGroup != ItemBase::GROUP_PET_CAGE )
				{
					m_nErrorCode = RESULT_NOT_TAMABLE;
					break;
				}

				// 테이밍 가능 우리 보유 체크
				if( !static_cast< StructPlayer * >( m_pOwner )->FindEmptyPetCage( nTameItemCode ) )
				{
					m_nErrorCode = RESULT_NOT_ACTABLE;
					break;
				}
			}
		}
		else if( GetSkillId() == SKILL_SHOVELING )
		{
			if( !m_pOwner->IsPet() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}
		}
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_SUMMON )
		{
			if( !m_pOwner->IsPlayer() || static_cast< StructPlayer * >( m_pOwner )->GetSubSummon() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			if( !static_cast< StructPlayer * >( m_pOwner )->IsSummonable() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			// { 소환시 처리 해줄게 좀 있네.-_-;
			StructItem *pItem = StructItem::FindItem( handle );

			// 카드가 없거나 카드의 내구도가 다 함
			if( !pItem || !pItem->IsSummonCard() || pItem->GetOwnerHandle() != m_pOwner->GetHandle() || ( pItem->GetItemEnhance() && !pItem->GetCurrentEtherealDurability() ) ) 
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			if( m_pOwner->IsPlayer() )
			{
				StructPlayer * pPlayer = static_cast< StructPlayer * >( m_pOwner );
				bool bFind = false;

				for( int i = 0; i < 6; ++i )
				{
					if( pItem == pPlayer->GetSummonCardAt( i ) )
					{
						bFind = true;
						break;
					}
				}

				if( !bFind )
				{
					m_nErrorCode = RESULT_NOT_ACTABLE;
					break;
				}
			}

			StructSummon *pSummon = pItem->GetSummonStruct();

			if( !pSummon )
			{
				m_nErrorCode = RESULT_NOT_EXIST;
				break;
			}

			if( pSummon->IsInWorld() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			ArPosition pos = m_pOwner->GetCurrentPosition( GetArTime() );
			pSummon->SetCurrentXY( pos.x, pos.y );
			pSummon->SetCurrentLayer( m_pOwner->GetLayer() );
			pSummon->StopMove();

			int prevRX = GetRegionX( pos.x );
			int prevRY = GetRegionX( pos.y );

			int nPosPickCount = 0;
			do
			{
				// ArObject::AddNoise 함수를 사용하지 않고 고정 위치(pos) 기준 사각형 내부(pos +- StructSummon::SUMMON_SPAWN_LENGTH)에서 랜덤하게 좌표 추출
				// * 대부분의 내용은 AddNoise 함수와 유사하게 동작하지만 랜덤하게 이동된 위치에서 다시 추가 이동하지 않고 원점 기준으로 반복해서 좌표를 추출한다는 것만 다름

				m_targetPos.x = pos.x + ( XRandom() % ( StructSummon::SUMMON_SPAWN_LENGTH * 2 ) ) - StructSummon::SUMMON_SPAWN_LENGTH;
				// 지역락 때문에 RX가 바뀌지 않는게 가장 좋지만 진입 불가 영역이 현재 Region에 너무 크게 걸쳐있으면 소환이 거의 불가능하게 되기 때문에
				// 1칸 까지는 이동을 허용함(VisibleRange 락 걸 때 2칸 여유 주기 때문에 일단 괜찮을 듯)
				if( abs( prevRX - (int)GetRegionX( m_targetPos.x ) ) > 1 )
					m_targetPos.x = pos.x;

				m_targetPos.y = pos.y + ( XRandom() % ( StructSummon::SUMMON_SPAWN_LENGTH * 2 ) ) - StructSummon::SUMMON_SPAWN_LENGTH;
				// 지역락 때문에 RX가 바뀌지 않는게 가장 좋지만 진입 불가 영역이 현재 Region에 너무 크게 걸쳐있으면 소환이 거의 불가능하게 되기 때문에
				// 1칸 까지는 이동을 허용함(VisibleRange 락 걸 때 2칸 여유 주기 때문에 일단 괜찮을 듯)
				if( abs( prevRY - (int)GetRegionY( m_targetPos.y ) ) > 1 )
					m_targetPos.y = pos.y;

				++nPosPickCount;
			}
			while( (
					( pos.x == m_targetPos.x && pos.y == m_targetPos.y ) ||						// 캐릭터랑 완전히 겹쳤으면 다시 좌표 선정
					//pos.GetDistance( m_targetPos ) < StructSummon::SUMMON_SPAWN_MIN_LENGTH ||	// 2미터 이상 떨어져서 나오게 한다.
					GameContent::CollisionToLine( pos.x, pos.y, m_targetPos.x, m_targetPos.y )	// 주인 시야에 들어와야 한다.
					) &&
					nPosPickCount < 10 );														// 좌표 선정 재시도는 10번까지만

			// 유효한 좌표를 100번 이내에 선정하지 못했다면 주인하고 겹침
			if( nPosPickCount >= 10 )
			{
				m_targetPos = pos;
			}

			pSummon->SetCurrentXY( m_targetPos.x, m_targetPos.y );
			m_targetLayer = pSummon->GetLayer();

			// }
		}
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_UNSUMMON ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_UNSUMMON_AND_ADD_STATE )
		{
			if( !m_pOwner->IsPlayer() || !static_cast< StructPlayer * >( m_pOwner )->GetMainSummon() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}
		}
		// 헌터홀릭 입장 가능 조건 체크가 필요한 스킬
		else if( GetSkillId() == StructSkill::SKILL_WARP_TO_HUNTAHOLIC_LOBBY )
		{
			m_nErrorCode = IsHuntaholicLobbyEnterableOwner( GetTargetHuntaholicID() );

			if( m_nErrorCode != RESULT_SUCCESS )
				break;
		}
		// 데스매치 입장 가능 조건 체크가 필요한 스킬
		else if( GetSkillId() == StructSkill::SKILL_RANKED_DEATHMATCH_ENTER || GetSkillId() == StructSkill::SKILL_FREED_DEATHMATCH_ENTER )
		{
			m_nErrorCode = IsInstanceGameEnterableOwner();

			if( m_nErrorCode != RESULT_SUCCESS )
				break;
		}
		// 보유 루피를 체크 해야하는 스킬
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_LOTTO )
		{
			StructPlayer *pPlayer = NULL;

			if( m_pOwner->IsPlayer() )
				pPlayer = static_cast< StructPlayer * >(m_pOwner);
			else if( m_pOwner->IsSummon() )
				pPlayer = static_cast< StructSummon * >(m_pOwner)->GetMaster();

			if( !pPlayer || pPlayer->GetGold() < StructGold( GetVar( 0 ) + GetVar( 1 ) * GetRequestedSkillLevel() ) )
			{
				m_nErrorCode = RESULT_NOT_ENOUGH_MONEY;
				break;
			}
		}
		// 대상 몬스터의 그룹을 가려서 사용해야 하는 스킬
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_SINGLE_DAMAGE_OR_DEATH )
		{
			StructCreature::iterator it = StructCreature::get( handle );

			if( !(*it) || !(*it)->IsMonster() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			StructMonster *pTarget = static_cast< StructMonster * >(*it);

			// Skill 모든 계열
			if( GameRule::USE_SKILL_ALL_OBJECT != GetVar( 8 )  )
			{
				if( GetVar( 8 ) != pTarget->GetCreatureGroup() )
				{
					m_nErrorCode = RESULT_LIMIT_RACE;
					break;
				}
			}
			
			// 보스 몬스터에겐 100% 실패
			if( pTarget->IsBossMonster() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			bool bIsInvalidTarget = false;
			if( GetVar( 9 ) != 0 && GetVar( 9 ) + GetVar( 16 ) * GetRequestedSkillLevel() > (float)pTarget->GetHP()/pTarget->GetMaxHP()*100 ) bIsInvalidTarget = true;
			if( GetVar( 10 ) != 0 && GetVar( 10 ) + GetVar( 17 ) * GetRequestedSkillLevel() < (float)pTarget->GetHP()/pTarget->GetMaxHP()*100 ) bIsInvalidTarget = true;

			for( int i = 11 ; i < 15 && !bIsInvalidTarget ; ++i )
			{
				if( GetVar( i ) == 0 ) continue;

				if( !pTarget->GetState( StructState::StateCode( static_cast< int >( GetVar( i ) ) ) ) )
				{
					bIsInvalidTarget = true;
					break;
				}
			}

			if( bIsInvalidTarget )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}
		}
		// 대상의 크리처 그룹을 가려서 사용해야 하는 스킬(여러 종류 그룹 가능)
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_STATE_BY_TARGET_TYPE )
		{
			StructCreature::iterator it = StructCreature::get( handle );

			if( !(*it) )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}
			int nTargetCreatureGroup = (*it)->GetCreatureGroup();

			bool bIsInvalidTarget = true;
			for( int i = -1 ; i < 5 ; ++i )
			{
				if( nTargetCreatureGroup == GetVar( i + 7 ) || nTargetCreatureGroup == CREATURE_ALL )
				{
					bIsInvalidTarget = false;
					break;
				}
			}

			if( bIsInvalidTarget )
			{
				m_nErrorCode = RESULT_LIMIT_RACE;
				break;
			}
		}
		// 방패 공격
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITH_SHIELD )
		{
			if( !m_pOwner->IsWearShield() )
				break;
		}
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK_OLD ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITHOUT_WEAPON_RUSH_KNOCK_BACK ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK )
		{
			StructCreature::iterator it = StructCreature::get( handle );
			if( !(*it) )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			ArPosition myPos = m_pOwner->GetCurrentPosition( current_time );
			ArPosition targetPos = (*it)->GetCurrentPosition( current_time );

			AR_UNIT nMinDistance;
			switch( GetSkillBase()->GetSkillEffectType() )
			{
			case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK_OLD:
			case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITHOUT_WEAPON_RUSH_KNOCK_BACK:
				nMinDistance = GetVar(3) * GameRule::DEFAULT_UNIT_SIZE;
				break;
			case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH:
			case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK:
				nMinDistance = GetVar(4) * GameRule::DEFAULT_UNIT_SIZE;
				break;
			default:
				nMinDistance = 0;
				break;
			}

			if( myPos.GetDistance( targetPos ) < nMinDistance )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			if( GameRule::bMonsterPathFinding && GameContent::CollisionToLine( myPos.x, myPos.y, targetPos.x, targetPos.y ) )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}
		}
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ACTIVATE_FIELD_PROP ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_REGION_HEAL_BY_FIELD_PROP ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_AREA_EFFECT_HEAL_BY_FIELD_PROP )
		{
			if( !m_pOwner->IsPlayer() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			StructFieldProp::iterator it = StructFieldProp::get( handle );

			if( !(*it) )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			if( !(*it)->IsFieldProp() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			StructFieldProp * pProp = static_cast< StructFieldProp * >( *it );

			if( pProp->GetFieldPropBase()->nActivateSkillId != GetSkillId() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			if( pProp->GetRemainCount() < 1 )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			if( !pProp->IsCastable( static_cast< StructPlayer * >( m_pOwner ) ) )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			delay = pProp->GetCastingDelay();
		}
		// 특정 지속효과가 타겟에게 걸려 있어야만 사용할 수 있는 스킬
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE )
		{
			StructCreature::iterator it = StructCreature::get( handle );
			StructCreature * pTarget = (*it);

			if( !pTarget )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			bool bCastable = false;
			for( int i = 0 ; i < 4 ; ++i )
			{
				if( pTarget->GetState( StructState::StateCode( static_cast< int >( GetVar( i ) ) ) ) )
				{
					bCastable = true;
					break;
				}
			}

			if( !bCastable )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}
		}
		// 몬스터가 아니면 사용할 수 없는 스킬(몬스터 전용)
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_RESPAWN_MONSTER_NEAR )
		{
			if( !m_pOwner->IsMonster() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}
		}
		else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_RESPAWN_MONSTER_RANDOMLY || GetSkillBase()->GetSkillEffectType() == SkillBase::EF_RESPAWN_MONSTER_WITH_DIFF_CODE )
		{
			if( m_pOwner->IsPlayer() && reinterpret_cast< StructPlayer * >( m_pOwner )->IsInBattleField() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}
		}

		int nHP = m_pOwner->GetHP();
		int nMP = m_pOwner->GetMP();
		int nEnergy = m_pOwner->GetEnergyCount();
		int nCurrentHPPercentage = m_pOwner->GetMaxHP() ? nHP * 100 / m_pOwner->GetMaxHP() : 0;
		int nCurrentMPPercentage = m_pOwner->GetMaxMP() ? nMP * 100 / m_pOwner->GetMaxMP() : 0;

		if( ( GetSkillBase()->GetEffectType() == SkillBase::EF_TOGGLE_AURA ||
			GetSkillBase()->GetEffectType() == SkillBase::EF_TOGGLE_DIFFERENTIAL_AURA ) && m_pOwner->IsActiveAura( this ) )
		{
			mana_cost = 0;
		}
		else
		{
			mana_cost = CalculateMPCost( m_pOwner->GetSkillCostModifier() );
		}

		int nNeedHP = GetNeedHP();
		if( nNeedHP > 0 )
		{
			if( nCurrentHPPercentage < nNeedHP ) { m_nErrorCode = RESULT_NOT_ENOUGH_HP; break; }
		}
		else if( nNeedHP < 0 )
		{
			if( nCurrentHPPercentage > 0 - nNeedHP ) { m_nErrorCode = RESULT_NOT_ACTABLE; break; }
		}

		if( nCurrentMPPercentage < GetNeedMP() ) { m_nErrorCode = RESULT_NOT_ENOUGH_MP; break; }

		if( nHP - 1 < CalculateHPCost( m_pOwner->GetSkillCostModifier() ) ) { m_nErrorCode = RESULT_NOT_ENOUGH_HP; break; }
		if( nMP < mana_cost ) { m_nErrorCode = RESULT_NOT_ENOUGH_MP; break; }

		if( nEnergy < static_cast< int >( GetCostEnergy( GetRequestedSkillLevel() ) ) ) { m_nErrorCode = RESULT_NOT_ENOUGH_ENERGY; break; }

		if( m_pOwner->GetLevel() < GetNeedLevel() ) { m_nErrorCode = RESULT_NOT_ENOUGH_LEVEL; break; }
		if( m_pOwner->GetJobPoint() < GetCostJP(GetRequestedSkillLevel(), GetEnhance() ) ) { m_nErrorCode = RESULT_NOT_ENOUGH_JP; break; }

		int nCostEXP = GetCostEXP( GetRequestedSkillLevel(), GetEnhance() );
		if( nCostEXP )
		{
			if( m_pOwner->GetEXP() < nCostEXP ) { m_nErrorCode = RESULT_NOT_ENOUGH_EXP; break; }
			__int64 nMinimumEXP = GameContent::GetNeedExp( m_pOwner->GetLevel() - 1 );
			if( m_pOwner->GetEXP() - nCostEXP < nMinimumEXP ) { m_nErrorCode = RESULT_NOT_ENOUGH_EXP; break; }
		}

		StructCreature::iterator itTarget = StructCreature::get( handle );
		StructCreature *pTarget = (*itTarget);

		// 시전 대상이 소모해야 할 수치를 따로 지정하는 스킬에 대해서 체크
		if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_STATE_BY_SELF_COST || GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_REGION_STATE_BY_SELF_COST )
		{
			float fCostMP = 0.0f;

			float fCostHP = GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();
			float fCostSP = GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
			float fCostEnergy = GetVar(6) + GetVar(7) * GetRequestedSkillLevel() + GetVar(8) * GetEnhance();
			if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_STATE_BY_SELF_COST )
				fCostMP = GetVar(9) + GetVar(10) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();

			// 사실 이거 스킬DB에서 제어해서 타겟 없으면 들어오지도 않음..
			if( !pTarget || !pTarget->IsSummon() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			// 이 유형들의 소모 수치들은 시전 대상이 지정된 수치를 가지고 있는지를 체크
			if( fCostHP && pTarget->GetHP() < fCostHP )										{ m_nErrorCode = RESULT_NOT_ENOUGH_HP; break; }
			if( fCostSP && static_cast< StructSummon * >(pTarget)->GetSP() < fCostSP )		{ m_nErrorCode = RESULT_NOT_ENOUGH_SP; break; }
			// 기공 소모는 -1이면 현재 1개라도 있기만 하면 모두 소모로 처리 됨
			if( fCostEnergy > 0 && pTarget->GetEnergyCount() < fCostEnergy )				{ m_nErrorCode = RESULT_NOT_ENOUGH_ENERGY; break; }
			else if( fCostEnergy == -1 && pTarget->GetEnergyCount() == 0 )					{ m_nErrorCode = RESULT_NOT_ENOUGH_ENERGY; break; }
			if( fCostMP && pTarget->GetMP() < fCostMP )										{ m_nErrorCode = RESULT_NOT_ENOUGH_MP; break; }
		}

		if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T1_DEAL_SUMMON_HP_OLD ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_HP_MP_BY_SUMMON_DAMAGE ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_HP_MP_BY_SUMMON_DEAD ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_HP_MP_BY_STEAL_SUMMON_HP_MP ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_DEAL_SUMMON_HP )
		{
			if( !m_pOwner->IsPlayer() ||
				!static_cast< StructPlayer * >( m_pOwner )->GetMainSummon() )
			{
				m_nErrorCode = RESULT_NOT_ENOUGH_HP;
				break;
			}

			int decHP = 0;
			int decMP = 0;

			if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T1_DEAL_SUMMON_HP_OLD )
				decHP = GetVar( 6 ) + GetVar( 7 ) * GetRequestedSkillLevel() + GetVar( 8 ) * GetEnhance();

			if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_DEAL_SUMMON_HP )
				decHP = GetVar( 9 ) + GetVar( 10 ) * GetRequestedSkillLevel() + GetVar( 11 ) * GetEnhance();

			if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_HP_MP_BY_SUMMON_DAMAGE )
				decHP = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon()->GetMaxHP() * GetVar( 10 );

			if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ADD_HP_MP_BY_STEAL_SUMMON_HP_MP )
			{
				decHP = GetVar( 0 ) + GetVar( 1 ) * GetRequestedSkillLevel() + GetVar( 4 ) * GetEnhance();
				decMP = GetVar( 2 ) + GetVar( 3 ) * GetRequestedSkillLevel() + GetVar( 5 ) * GetEnhance();
			}

			if( static_cast< StructPlayer * >( m_pOwner )->GetMainSummon()->GetHP() < decHP + 1 )
			{
				m_nErrorCode = RESULT_NOT_ENOUGH_HP;
				break;
			}
			if( static_cast< StructPlayer * >( m_pOwner )->GetMainSummon()->GetMP() < decMP )
			{
				m_nErrorCode = RESULT_NOT_ENOUGH_MP;
				break;
			}
		}

		// 펫 삽질인 경우 Status 바꾸고 메시지 송신
		if( m_pOwner->IsPet() && GetSkillId() == SKILL_SHOVELING )
		{
			static_cast< StructPet * >( m_pOwner )->SetShovelingStatus( StructPet::SHOVELING_STATUS_SEARCH );
			BroadcastStatusMessage( m_pOwner );
		}

		// { 아이템 소모
		ItemBase::ItemCode cost_item_code = GetCostItemCode();

		StructPlayer *pInvenPlayer = NULL;
		if( m_pOwner->IsPlayer() ) pInvenPlayer = static_cast< StructPlayer* >( m_pOwner );
		if( m_pOwner->IsSummon() ) pInvenPlayer = static_cast< StructSummon* >( m_pOwner )->GetMaster();

		// 화살 소모인경우
		if( pInvenPlayer )
		{
			if( cost_item_code == ItemBase::ITEM_CODE_WEARED_BULLET )
			{
				StructItem * pCostItem = NULL;

				pCostItem = pInvenPlayer->GetWearedItem( ItemBase::WEAR_BULLET );
				if( !pCostItem || !pCostItem->IsBullet() )
				{
					m_nErrorCode = RESULT_NOT_ENOUGH_BULLET;
					break;
				}

				// 화살통이면 예상 소모 개수도 0
				int nCostItemCount = 0;
				if( pCostItem->GetItemClass() != ItemBase::CLASS_QUIVER )
					nCostItemCount = GetCostItemCount( GetRequestedSkillLevel() );				

				if( !pCostItem || pCostItem->GetCount() < nCostItemCount )
				{
					m_nErrorCode = RESULT_NOT_ENOUGH_BULLET;
					break;
				}
			}
			else if( cost_item_code )
			{
				StructItem  * pCostItem = pInvenPlayer->FindItem( cost_item_code );

				if( !pCostItem || pCostItem->GetCount() < GetCostItemCount( GetRequestedSkillLevel() ) )
				{
					{ m_nErrorCode = RESULT_NOT_ENOUGH_ITEM; break; }
				}
			}
			
			if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_STATE_BY_USING_ITEM )
			{
				if( GetRequestedSkillLevel() > 0 && GetRequestedSkillLevel() < 20 )
				{
					ItemBase::ItemCode requiredItemCode = GetVar( 0 );
					__int64 requiredItemCount = GetVar( GetRequestedSkillLevel() );
					StructItem* costItem = pInvenPlayer->FindItem( requiredItemCode );

					if ( !costItem || requiredItemCount > costItem->GetCount() )
					{
						m_nErrorCode = RESULT_NOT_ENOUGH_ITEM;
						break;
					}
				}
				else
				{
					m_nErrorCode = RESULT_NOT_ACTABLE;
					break;
				}
			}
		}
		// }

		if( GetNeedStateId() )
		{
			StructState * pState = m_pOwner->GetState( StructState::StateCode( GetNeedStateId() ) );

			if( !pState || pState->GetLevel() < GetNeedStateLevel() )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			if( NeedStateExhaust() == SkillBase::NEED_STATE_EXHAUSE_ALL )
			{
				m_pOwner->RemoveState( pState->GetCode() );
			}
			else if( NeedStateExhaust() == SkillBase::NEED_STATE_EXHAUSE_SPECIFIC )
			{
				m_pOwner->DecreaseState( pState->GetCode(), GetNeedStateLevel() );
			}
		}

		if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ACTIVATE_FIELD_PROP ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_REGION_HEAL_BY_FIELD_PROP ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_AREA_EFFECT_HEAL_BY_FIELD_PROP )
		{
			StructFieldProp::iterator it = StructFieldProp::get( handle );

			StructFieldProp * pProp = static_cast< StructFieldProp * >( *it );

			if( !(*it) || !(*it)->IsFieldProp() || !IsVisibleRegion( m_pOwner->GetRX(), m_pOwner->GetRY(), (*it)->GetRX(), (*it)->GetRY() ) )
			{
				m_nErrorCode = RESULT_NOT_ACTABLE;
				break;
			}

			pProp->Cast();
		}

		// 비용 소비
		m_pOwner->SetMP( nMP - mana_cost * GameRule::SKILL_CAST_COST );

		AR_TIME nOriginalCastingDelay = delay;

		if( delay == -1 )
		{
			// 시전시간
			nOriginalCastingDelay = delay = GetCastDelay( GetRequestedSkillLevel(), GetEnhance() );

			// 시전속도 적용
			// * 단, 아이템으로 시전된 스킬이면 시전 속도에 영향 안받음
			if( GetSkillUID() >= 0 && !hCastItem )
			{
				delay /= ( std::max( m_pOwner->GetCastingSpeed(), 1 ) / 100.0f );
				delay *= m_pOwner->GetCastingMod( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful(), nOriginalCastingDelay );
			}
		}

		if( GetSkillBase()->GetSkillEffectType() != SkillBase::EF_SUMMON )
		{
			m_targetPos = pos;
			m_targetLayer = layer;
		}
		m_hTarget = handle;
		m_nCastTime = current_time;
		m_nCastingDelay = nOriginalCastingDelay;
		m_nFireTime = current_time + delay;

	} while( false );

	int nErrorCode = m_nErrorCode;

	if( nErrorCode != RESULT_SUCCESS )
	{
		Init();
	}
	else
	{
		broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), m_pOwner->GetLayer(), 0, mana_cost * GameRule::SKILL_CAST_COST, TS_SC_SKILL::CASTING );

		if( GetSkillBase()->IsHarmful() && !GetSkillBase()->IsPhysicalSkill() )
		{
			// 캐스트 반격
			StructCreature::iterator itMonster = StructCreature::get( handle );
			StructCreature * pCreature = (*itMonster);

			if( pCreature && pCreature->IsMonster() && static_cast< StructMonster * >( pCreature )->IsCastRevenger() )
			{
				static_cast< StructMonster * >( pCreature )->AddHate( m_pOwner->GetHandle(), 1 );
			}
		}
	}

	return nErrorCode;
}

void StructSkill::Init()
{
	m_nErrorCode = RESULT_SUCCESS;
	m_Status = STATUS_IDLE;
	m_nCastTime = 0;
	m_nCastingDelay = 0;
	m_nFireTime = 0;
	m_nRequestedSkillLevel = 0;
	m_hTarget = 0;
	m_targetPos = ArPosition();
	m_targetLayer = 0;
	m_hCastItem = 0;
	m_nCurrentFire = 0;
	m_nTotalFire = 0;
	m_nTargetCount = 1;
	m_nFireCount = 1;
	m_vResultList.clear();
}

void StructSkill::onDamage( int nDamage )
{
	// 현재는 캐스트 캔슬/지연 지원
	if( m_Status != STATUS_CAST )
		return;

	if( nDamage > m_pOwner->GetMaxHP() * GameRule::SKILL_CANCEL_RATE )
	{
		int nCastKeep = m_pOwner->GetCastKeep();
		if( !nCastKeep )
			nCastKeep = 100;

		float fMod = ( 100 / nCastKeep ) * ( 1 + ( 0.4f * static_cast<int>( nDamage / m_pOwner->GetMaxHP() * 10 ) ) );
		if( GetCastCancelType() == SkillBase::IT_DELAY )
		{
			AR_TIME delay = 100;
			switch( GetCastCancelLevel() )
			{
			case SkillBase::IL_LOW:		delay = GameRule::CASTING_DELAY_LOW; break;
			case SkillBase::IL_MEDIUM:	delay = GameRule::CASTING_DELAY_MEDIUM; break;
			case SkillBase::IL_HIGH:	delay = GameRule::CASTING_DELAY_HIGH; break;
			}

			// 딜레이~~!
			delay *= fMod;
			m_nFireTime += delay;

			sendSkillMessage( TS_SC_SKILL::CASTING_UPDATE );
		}
		else if( GetCastCancelType() == SkillBase::IT_CANCEL )
		{
			int nCancelRate = 0;

			switch( GetCastCancelLevel() )
			{
			case SkillBase::IL_LOW:		nCancelRate = GameRule::CASTING_CANCEL_LOW; break;
			case SkillBase::IL_MEDIUM:	nCancelRate = GameRule::CASTING_CANCEL_MEDIUM; break;
			case SkillBase::IL_HIGH:	nCancelRate = GameRule::CASTING_CANCEL_HIGH; break;
			}

			nCancelRate *= fMod;
			// 일정 확률로 캔슬~
			if( XRandom() % 100 < nCancelRate ) m_pOwner->CancelSkill();
		}
	}
}

void StructSkill::bindSkillInfo( int skill_id )
{
	m_pSkillBase = GameContent::GetSkillBase( skill_id );

	int nEffectType = m_pSkillBase->GetEffectType();

	if( nEffectType == SkillBase::EF_PHYSICAL_MULTIPLE_DAMAGE_T3 ||
		nEffectType == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T1_OLD ||
		nEffectType == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T2_OLD ||
		nEffectType == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T1_DEAL_SUMMON_HP_OLD ||
		nEffectType == SkillBase::EF_MAGIC_MULTIPLE_REGION_DAMAGE_OLD ||
		nEffectType == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITHOUT_WEAPON_RUSH_KNOCK_BACK ||
		nEffectType == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK_OLD ||
		nEffectType == SkillBase::EF_PHYSICAL_MULTIPLE_DAMAGE_TRIPLE_ATTACK_OLD ||

		// 신규 연타 유형 - 2006/10/12
		nEffectType == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE ||
		nEffectType == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_DEAL_SUMMON_HP ||
		nEffectType == SkillBase::EF_MAGIC_MULTIPLE_REGION_DAMAGE ||

		// 신규 연타 유형 - 2007/03/24
		nEffectType == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH ||
		nEffectType == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK ||
		nEffectType == SkillBase::EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE ||

		// 신규 연타 유형 - 2007/05/09
		nEffectType == SkillBase::EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE_KNOCKBACK ||

		// 신규 연타 유형 - 2007/07/04
		nEffectType == SkillBase::EF_PHYSICAL_REALTIME_MULTIPLE_REGION_DAMAGE
		)
	{
		m_bMultiple = true;
	}
}

bool StructSkill::CheckCoolTime( AR_TIME t ) const
{
	if( GameRule::bIgnoreSkillCoolTime ) return true;

	return t > m_nNextCoolTime;
}

void StructSkill::FireSkill( struct StructCreature * pTarget, bool bIsSuccess )
{
	AR_TIME t = m_nFireTime;

	m_vResultList.clear();

	// 은신 해제
	if( m_pOwner->IsHiding() )
	{
		m_pOwner->RemoveState( StructState::HIDE, GameRule::MAX_STATE_LEVEL );
		m_pOwner->RemoveState( StructState::TRACE_OF_FUGITIVE, GameRule::MAX_STATE_LEVEL );
	}

	int skillEffectType = GetSkillBase()->GetSkillEffectType();
	switch( skillEffectType )
	{

	case SkillBase::EF_ADD_STATE:
	case SkillBase::EF_ADD_STATE_BY_TARGET_TYPE:
	case SkillBase::EF_ADD_STATE_TO_CASTER_AND_TARGET:
	case SkillBase::EF_ADD_RANDOM_STATE:
	case SkillBase::EF_ADD_STATE_BY_USING_ITEM:
		{
			// 지속효과 추가
			STATE_SKILL_FUNCTOR mySkillFunctor( &m_vResultList );

			process_target( t, mySkillFunctor, pTarget );
			break;
		}
	case SkillBase::EF_ADD_REGION_STATE:
	case SkillBase::EF_ADD_RANDOM_REGION_STATE:
		{
			ADD_REGION_STATE( pTarget );
			break;
		}
	case SkillBase::EF_ADD_STATE_BY_SELF_COST:
		{
			ADD_STATE_BY_SELF_COST( pTarget );
			break;
		}
	case SkillBase::EF_ADD_REGION_STATE_BY_SELF_COST:
		{
			ADD_REGION_STATE_BY_SELF_COST( pTarget );
			break;
		}
	case SkillBase::EF_ADD_STATE_STEP_BY_STEP:
		{
			ADD_STATE_STEP_BY_STEP( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_DIRECTIONAL_DAMAGE:
		{
			// 방향성 공격
			PHYSICAL_DIRECTIONAL_DAMAGE( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_T1:
		{
			// 공격력 증가 물리공격
			SINGLE_PHYSICAL_DAMAGE_T1( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_T2:
		{
			// 공격력 증폭 물리공격
			SINGLE_PHYSICAL_DAMAGE_T2( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_T3:
		{
			// 무기공격력 무시 물리공격
			SINGLE_PHYSICAL_DAMAGE_T3( pTarget );		
			break;
		}
	case SkillBase::EF_PHYSICAL_MULTIPLE_DAMAGE_T1:
		{
			// 공격력 증폭 물리공격
			MULTIPLE_PHYSICAL_DAMAGE_T1( pTarget );		
			break;
		}
	case SkillBase::EF_PHYSICAL_MULTIPLE_DAMAGE_T2:
		{
			MULTIPLE_PHYSICAL_DAMAGE_T2( pTarget );		
			break;
		}
	case SkillBase::EF_PHYSICAL_MULTIPLE_DAMAGE_T3:
		{
			MULTIPLE_PHYSICAL_DAMAGE_T3( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_ABSORB_DAMAGE:
		{
			// 공격력 증가 물리공격 : 흡수
			SINGLE_PHYSICAL_DAMAGE_ABSORB( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_OLD:
		{
			// 공격력 증가 물리공격 : 타겟 중심 범위
			PHYSICAL_SINGLE_REGION_DAMAGE_OLD( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_MULTIPLE_REGION_DAMAGE_OLD:
		{
			// 공격력 증가 물리공격 : 연타 : 타겟 중심 범위
			PHYSICAL_MULTIPLE_REGION_DAMAGE_OLD( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE_OLD:
		{
			// 공격력 증가 물리공격 : 연타 : 타겟 중심 특수 범위
			PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_SPECIAL_REGION_DAMAGE_OLD:
		{
			// 공격력 증가 물리공격 : 특수 범위
			PHYSICAL_SPECIAL_REGION_DAMAGE( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_KNOCKBACK_OLD:
		{
			// 공격력 증가 물리공격 : 대상밀어내기
			SINGLE_PHYSICAL_DAMAGE_T1( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_ADD_ENERGY_OLD:
		{
			// 공격력 증폭 물리공격 : 기공 충전
			SINGLE_PHYSICAL_DAMAGE_T2_ADD_ENERGY( pTarget );
		}
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_OLD:
		{
			// 공격력 증가 물리 공격 : 밀어내기 : 타겟 중심 범위
			PHYSICAL_SINGLE_REGION_DAMAGE_OLD( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITHOUT_WEAPON_RUSH_KNOCK_BACK:
		{
			// 돌진 : 무기공격력 무시 물리공격 : 밀어내기
			SINGLE_PHYSICAL_DAMAGE_T3( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK_OLD:
		{
			// 돌진 : 공격력 증가 물리공격 : 밀어내기
			SINGLE_PHYSICAL_DAMAGE_T1( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_MULTIPLE_DAMAGE_TRIPLE_ATTACK_OLD:
		{
			// 공격력 증가 물리공격 : 한방연타 - 연타횟수 3단계 적용
			MULTIPLE_PHYSICAL_DAMAGE_T4( pTarget );
			break;
		}
	case SkillBase::EF_REMOVE_BAD_STATE:
		{
			// 나쁜 지속효과 제거
			REMOVE_BAD_STATE_SKILL_FUNCTOR mySkillFunctor( &m_vResultList );
			process_target( t, mySkillFunctor, pTarget );
			break;
		}
	case SkillBase::EF_REMOVE_GOOD_STATE:
		{
			// 좋은 지속효과 제거
			REMOVE_GOOD_STATE_SKILL_FUNCTOR mySkillFunctor( &m_vResultList );
			process_target( t, mySkillFunctor, pTarget );
			break;
		}
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE_OLD:
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE:
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL:
	case SkillBase::EF_AREA_EFFECT_MAGIC_DAMAGE_AND_HEAL_T2:
	case SkillBase::EF_AREA_EFFECT_HEAL:
		{
			MAKE_AREA_EFFECT_PROP( pTarget, false );
			break;
		}
	case SkillBase::EF_TRAP_PHYSICAL_DAMAGE:
	case SkillBase::EF_TRAP_MAGICAL_DAMAGE:
	case SkillBase::EF_TRAP_MULTIPLE_PHYSICAL_DAMAGE:
	case SkillBase::EF_TRAP_MULTIPLE_MAGICAL_DAMAGE:
		{
			MAKE_AREA_EFFECT_PROP( pTarget, true );
			break;
		}
	case SkillBase::EF_CREATE_ITEM:
		{
			CREATE_ITEM( pTarget, bIsSuccess );
			break;
		}
	case SkillBase::EF_ACTIVATE_FIELD_PROP:
		{
			ACTIVATE_FIELD_PROP();
			break;
		}
	case SkillBase::EF_REGION_HEAL_BY_FIELD_PROP:
		{
			REGION_HEAL_BY_FIELD_PROP();
			break;
		}
	case SkillBase::EF_AREA_EFFECT_HEAL_BY_FIELD_PROP:
		{
			MAKE_AREA_EFFECT_PROP_BY_FIELD_PROP( false );
			break;
		}
	case SkillBase::EF_ADD_HP:
		{
			// HP 증가
			HEALING_SKILL_FUNCTOR mySkillFunctor( &m_vResultList );
			mySkillFunctor.nResult = 0;
			process_target( t, mySkillFunctor, pTarget );
			break;
		}
	case SkillBase::EF_ADD_MP:
		{
			RECOVERY_MP_SKILL_FUNCTOR mySkillFunctor( &m_vResultList );
			mySkillFunctor.nResult = 0;
			process_target( t, mySkillFunctor, pTarget );
			break;
		}
	case SkillBase::EF_ADD_HP_BY_ITEM:
		{
			// HP 증가
			HEALING_SKILL_FUNCTOR mySkillFunctor( &m_vResultList, true );
			mySkillFunctor.nResult = 0;
			process_target( t, mySkillFunctor, pTarget );
			break;
		}
	case SkillBase::EF_ADD_MP_BY_ITEM:
		{
			// MP 증가
			RECOVERY_MP_SKILL_FUNCTOR mySkillFunctor( &m_vResultList, true );
			mySkillFunctor.nResult = 0;
			process_target( t, mySkillFunctor, pTarget );
			break;
		}
	case SkillBase::EF_RESURRECTION:
		{
			// 부활
			SKILL_RESURRECTION( pTarget );
			break;
		}
	case SkillBase::EF_ADD_HP_MP:
	case SkillBase::EF_ADD_HP_MP_BY_SUMMON_DAMAGE:
	case SkillBase::EF_ADD_HP_MP_BY_SUMMON_DEAD:
	case SkillBase::EF_ADD_HP_MP_BY_STEAL_SUMMON_HP_MP:
	case SkillBase::EF_ADD_HP_MP_WITH_LIMIT_PERCENT:
		{
			SKILL_ADD_HP_MP( pTarget );
			break;
		}
	case SkillBase::EF_ADD_REGION_HP_MP:
		{
			SKILL_ADD_REGION_HP_MP( pTarget );
			break;
		}
	case SkillBase::EF_ADD_REGION_HP:
		{
			SKILL_ADD_REGION_HP( pTarget );
			break;
		}
	case SkillBase::EF_ADD_REGION_MP:
		{
			SKILL_ADD_REGION_MP( pTarget );
			break;
		}

	case SkillBase::EF_MAGIC_SINGLE_DAMAGE_T1_OLD:
		{
			if( !GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET ) assert( 0 );

			SINGLE_MAGICAL_DAMAGE_T1( pTarget );
			break;
		}

	case SkillBase::EF_MAGIC_SINGLE_DAMAGE_T2_OLD:
		{
			if( !GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET ) assert( 0 );

			SINGLE_MAGICAL_DAMAGE_T2( pTarget );
			break;
		}

		// 위의 두 유형을 대체한 새 유형
	case SkillBase::EF_MAGIC_SINGLE_DAMAGE:
	case SkillBase::EF_MAGIC_SINGLE_DAMAGE_ADD_RANDOM_STATE:
	case SkillBase::EF_MAGIC_SINGLE_DAMAGE_WITH_PHYSICAL_DAMAGE:
		{
			if( !GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET ) assert( 0 );

			SINGLE_MAGICAL_DAMAGE( pTarget );
			break;
		}

	case SkillBase::EF_MAGIC_SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE:
		{
			SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE( pTarget );
			break;
		}

	case SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T1_OLD:
	case SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T1_DEAL_SUMMON_HP_OLD:
		{
			if( !GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET ) assert( 0 );

			MULTIPLE_MAGICAL_DAMAGE_T1( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_AT_ONCE:
		{
			if( !GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET ) assert( 0 );

			MULTIPLE_MAGICAL_DAMAGE_AT_ONCE( pTarget );
			break;
		}

		// 위의 유형을 각각 대체한 새 유형
	case SkillBase::EF_MAGIC_MULTIPLE_DAMAGE:
	case SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_DEAL_SUMMON_HP:
		{
			if( !GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET ) assert( 0 );

			MULTIPLE_MAGICAL_DAMAGE( pTarget );
			break;
		}

		// 재수 없으면 즉사하는 새 유형
	case SkillBase::EF_MAGIC_SINGLE_DAMAGE_OR_DEATH:
		{
			if( !GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET ) assert( 0 );

			SINGLE_MAGICAL_DAMAGE_OR_DEATH( pTarget );
			break;
		}

		// 데미지 주고 HP/MP 흡수하는 새 유형
	case SkillBase::EF_MAGIC_DAMAGE_WITH_ABSORB_HP_MP:
		{
			SINGLE_MAGICAL_DAMAGE_WITH_ABSORB( pTarget );
			break;
		}

		// HP/MP 소모 후 HP/MP 흡수하는 새 유형
	case SkillBase::EF_ADD_HP_MP_BY_ABSORB_HP_MP:
		{
			ADD_HP_MP_BY_ABSORB_HP_MP( pTarget );
			break;
		}

	case SkillBase::EF_MAGIC_SINGLE_PERCENT_DAMAGE:
		{
			SINGLE_MAGICAL_TARGET_HP_PERCENT_DAMAGE( pTarget );
			break;
		}

	case SkillBase::EF_MAGIC_SINGLE_PERCENT_MANABURN:
	case SkillBase::EF_MAGIC_SINGLE_PERCENT_OF_MAX_MP_MANABURN:
		{
			SINGLE_MAGICAL_MANABURN( pTarget );
			break;
		}

	case SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T2_OLD:
		{
			if( !GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET ) assert( 0 );

			MULTIPLE_MAGICAL_DAMAGE_T2( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T3_OLD:
		{
			MULTIPLE_MAGICAL_DAMAGE_T3( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE_OLD:
		{
			MAGIC_SINGLE_REGION_DAMAGE_OLD( pTarget );
			break;
		}
		// 범위마법공격: 표준 신규 스킬 유형 - 위의 유형 대체
	case SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE:
	case SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE_USING_CORPSE:
	case SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE_ADD_RANDOM_STATE:
		{
			MAGIC_SINGLE_REGION_DAMAGE( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE_BY_SUMMON_DEAD:
		{
			MAGIC_SINGLE_REGION_DAMAGE_BY_SUMMON_DEAD( pTarget );
			break;
		}
	case SkillBase::EF_REGION_TAUNT:
		{
			REGION_TAUNT( pTarget );
			break;
		}
	case SkillBase::EF_TAUNT:
		{
			TAUNT( pTarget );
			break;
		}
	case SkillBase::EF_REMOVE_HATE:
		{
			REMOVE_HATE( pTarget );
			break;
		}
	case SkillBase::EF_REGION_REMOVE_HATE:
	case SkillBase::EF_REGION_REMOVE_HATE_OF_TARGET:
		{
			REGION_REMOVE_HATE( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_MULTIPLE_REGION_DAMAGE_AT_ONCE:
		{
			MAGIC_MULTIPLE_REGION_DAMAGE_AT_ONCE( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_MULTIPLE_REGION_DAMAGE_OLD:
		{
			MAGIC_MULTIPLE_REGION_DAMAGE_OLD( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_MULTIPLE_REGION_DAMAGE_T2_OLD:
		{
			MAGIC_MULTIPLE_REGION_DAMAGE_T2( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_SPECIAL_REGION_DAMAGE_OLD:
		{
			MAGIC_SPECIAL_REGION_DAMAGE_OLD( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_SPECIAL_REGION_DAMAGE:
		{
			MAGIC_SPECIAL_REGION_DAMAGE( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITH_SHIELD:
		{
			SINGLE_PHYSICAL_DAMAGE_WITH_SHIELD( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_MULTIPLE_REGION_DAMAGE:
		{
			MAGIC_MULTIPLE_REGION_DAMAGE( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_REGION_PERCENT_DAMAGE:
		{
			MAGIC_SINGLE_REGION_PERCENT_DAMAGE( pTarget );
			break;
		}
	case SkillBase::EF_MAGIC_ABSORB_DAMAGE_OLD:
		{
			MAGIC_ABSORB_DAMAGE( pTarget );
			break;
		}
	case SkillBase::EF_TOGGLE_AURA:
	case SkillBase::EF_TOGGLE_DIFFERENTIAL_AURA:
		{
			TOGGLE_AURA( pTarget );
			break;
		}
	case SkillBase::EF_SUMMON:
		{	
			if( m_pOwner->IsPlayer() ) Summon();
			break;
		}
	case SkillBase::EF_UNSUMMON:
		{
			if( m_pOwner->IsPlayer() ) UnSummon();
			break;
		}
	case SkillBase::EF_UNSUMMON_AND_ADD_STATE:
		{
			if( m_pOwner->IsPlayer() ) UnSummonAndAddState();
			break;
		}
	case SkillBase::EF_CORPSE_ABSORB:
		{
			CORPSE_ABSORB( pTarget );
			break;
		}
	case SkillBase::EF_CORPSE_EXPLOSION:
		{
			CORPSE_EXPLOSION( pTarget );
			break;
		}
		// 신규 유형 - 대체 구 유형:	EF_PHYSICAL_SINGLE_DAMAGE_T1, EF_PHYSICAL_SINGLE_DAMAGE_T2, EF_PHYSICAL_SINGLE_DAMAGE_T3,
		//								EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK_OLD, EF_PHYSICAL_SINGLE_DAMAGE_KNOCKBACK_OLD
		// 신규 추가 유형:				EF_PHYSICAL_SINGLE_DAMAGE_RUSH
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE:
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH:
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK:
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_KNOCKBACK:
		{
			PHYSICAL_SINGLE_DAMAGE( pTarget );
			break;
		}
		// 신규 유형 - 대체 구 유형:	EF_PHYSICAL_ABSORB_DAMAGE
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_ABSORB:
		{
			PHYSICAL_SINGLE_DAMAGE_ABSORB( pTarget );
			break;
		}
		// 신규 유형 - 대체 구 유형:	EF_PHYSICAL_SINGLE_DAMAGE_ADD_ENERGY_OLD
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_ADD_ENERGY:
		{
			PHYSICAL_SINGLE_DAMAGE_ADD_ENERGY( pTarget );
			break;
		}
		// 신규 유형 - 대체 구 유형:	EF_PHYSICAL_SINGLE_REGION_DAMAGE_OLD, EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_OLD
		// 신규 추가 유형:				EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_SELF, EF_PHYSICAL_SINGLE_REGION_DAMAGE_WITH_CAST_CANCEL
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE:
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK:
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_SELF:
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_WITH_CAST_CANCEL:
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_ADDING_MAGICAL_DAMAGE:
		{
			PHYSICAL_SINGLE_REGION_DAMAGE( pTarget );
			break;
		}
		// 신규 유형 - 대체 구 유형:	EF_PHYSICAL_MULTIPLE_DAMAGE_T3
		// 신규 추가 유형:				EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE_KNOCKBACK
	case SkillBase::EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE:
	case SkillBase::EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE_KNOCKBACK:
		{
			PHYSICAL_REALTIME_MULTIPLE_DAMAGE( pTarget );
			break;
		}
		// 신규 추가 유형:				EF_PHYSICAL_REALTIME_MULTIPLE_REGION_DAMAGE
	case SkillBase::EF_PHYSICAL_REALTIME_MULTIPLE_REGION_DAMAGE:
		{
			PHYSICAL_REALTIME_MULTIPLE_REGION_DAMAGE( pTarget );
			break;
		}
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE:
		{
			SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE( pTarget );
			break;
		}
		// 신규 유형 - 대체 구 유형:	EF_PHYSICAL_MULTIPLE_DAMAGE_T1
	case SkillBase::EF_PHYSICAL_MULTIPLE_DAMAGE:
		{
			PHYSICAL_MULTIPLE_DAMAGE( pTarget );
			break;
		}
		// 신규 유형 - 대체 구 유형:	EF_PHYSICAL_MULTIPLE_DAMAGE_TRIPLE_ATTACK_OLD
	case SkillBase::EF_PHYSICAL_MULTIPLE_DAMAGE_TRIPLE_ATTACK:
		{
			PHYSICAL_MULTIPLE_DAMAGE_TRIPLE_ATTACK( pTarget );
			break;
		}
		// 신규 유형 - 대체 구 유형:	EF_PHYSICAL_MULTIPLE_REGION_DAMAGE_OLD, EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE_OLD
		// 신규 추가 유형:				EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE_SELF
	case SkillBase::EF_PHYSICAL_MULTIPLE_REGION_DAMAGE:
	case SkillBase::EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE:
	case SkillBase::EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE_SELF:
		{
			PHYSICAL_MULTIPLE_REGION_DAMAGE( pTarget );
			break;
		}
		// 신규 추가 유형 - 대체 구 유형:	EF_PHYSICAL_SINGLE_SPECIAL_REGION_DAMAGE_OLD
	case SkillBase::EF_PHYSICAL_SINGLE_SPECIAL_REGION_DAMAGE:
		{
			PHYSICAL_SINGLE_SPECIAL_REGION_DAMAGE( pTarget );
			break;
		}

		// 신규 추가 유형:				EF_RESURRECTION_WITH_RECOVER
	case SkillBase::EF_RESURRECTION_WITH_RECOVER:
		{
			SKILL_RESURRECTION_WITH_RECOVER( pTarget );
			break;
		}

		// 신규 추가 유형:				EF_REMOVE_STATE_GROUP
	case SkillBase::EF_REMOVE_STATE_GROUP:
		{
			REMOVE_STATE_GROUP_SKILL_FUNCTOR mySkillFunctor( &m_vResultList, m_pOwner );
			process_target( t, mySkillFunctor, pTarget );
			break;
		}

	case SkillBase::EF_PHYSICAL_CHAIN_DAMAGE:
		{
			PHYSICAL_CHAIN_DAMAGE( pTarget );
			break;
		}

	case SkillBase::EF_MAGIC_CHAIN_DAMAGE:
		{
			MAGIC_CHAIN_DAMAGE( pTarget );
			break;
		}

	case SkillBase::EF_CHAIN_HEAL:
		{
			CHAIN_HEAL( pTarget );
			break;
		}

	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_DEMINISHED_HP_MP:
		{
			PHYSICAL_SINGLE_DAMAGE_DEMINISHED_HP_MP( pTarget );
			break;
		}

	case SkillBase::EF_INC_SKILL_COOL_TIME:
		{
			INC_SKILL_COOL_TIME( pTarget );
			break;
		}

	case SkillBase::EF_AMP_SKILL_COOL_TIME:
		{
			AMP_SKILL_COOL_TIME( pTarget );
			break;
		}

	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_PROP_REMAIN_MP:
		{
			PHYSICAL_SINGLE_DAMAGE_PROP_REMAIN_MP( pTarget );
			break;
		}

	case SkillBase::EF_REPLENISH_ENERGY_HP_MP:
		{
			REPLENISH_ENERGY_HP_MP();
			break;
		}

		// 신규 추가 유형:				EF_LOTTO
	case SkillBase::EF_LOTTO:
		{
			SKILL_LOTTO();
			break;
		}

		// 신규 추가 유형:				EF_CASTING_CANCEL_WITH_ADD_STATE
	case SkillBase::EF_CASTING_CANCEL_WITH_ADD_STATE:
		{
			CASTING_CANCEL_WITH_ADD_STATE( pTarget );
			break;
		}

		// 신규 추가 유형:				EF_RESPAWN_MONSTER_NEAR
	case SkillBase::EF_RESPAWN_MONSTER_NEAR:
		{
			RESPAWN_NEAR_MONSTER();
			break;
		}	

		// 신규 추가 유형:				EF_RESPAWN_MONSTER_RANDOMLY
		// 신규 추가 유형:				EF_RESPAWN_MONSTER_WITH_DIFF_CODE
	case SkillBase::EF_RESPAWN_MONSTER_RANDOMLY:
	case SkillBase::EF_RESPAWN_MONSTER_WITH_DIFF_CODE:
		{
			RESPAWN_MONSTER();
			break;
		}



	default:
		{
			switch( GetSkillId() )
			{
			case SKILL_TOWN_PORTAL:
			case SKILL_RETURN:						TOWN_PORTAL(); break;

			case SKILL_RANKED_DEATHMATCH_ENTER:
			case SKILL_FREED_DEATHMATCH_ENTER:		INSTANCE_GAME_ENTER(); break;

			case SKILL_WARP_TO_HUNTAHOLIC_LOBBY:	WARP_TO_HUNTAHOLIC_LOBBY(); break;

			case SKILL_INSTANCE_GAME_EXIT:			INSTANCE_GAME_EXIT(); break;

			case SKILL_RETURN_FEATHER:				RETURN_FEATHER(); break;
			case SKILL_RETURN_BACK_FEATHER:			RETURN_BACK_FEATHER(); break;
			case SKILL_CREATURE_TAMING:				
			case SKILL_ITEM_CREATURE_TAMING_SCROLL:	CREATURE_TAMING( pTarget ); break;
			case SKILL_PET_TAMING:					PET_TAMING( pTarget ); break;
			case SKILL_SHOVELING:					SHOVELING(); break;
			case SKILL_GAIA_FORCE_SAVING:			ADD_ENERGY(); break;
			default:								break;
			}
		}
	}

	// 헤이트, 하보크 추가
	{
		std::vector< StructCreature * > vNeedStateList;
		std::vector< SkillResult > vResultList = std::vector< SkillResult >( m_vResultList );

		for( std::vector< SkillResult >::iterator it = vResultList.begin(); it != vResultList.end(); ++it )
		{

			if( (*it).GetType() == SkillResult::DAMAGE || (*it).GetType() == SkillResult::MAGIC_DAMAGE ||  (*it).GetType() == SkillResult::DAMAGE_WITH_KNOCK_BACK ||
				(*it).GetType() == SkillResult::ADD_HP || (*it).GetType() == SkillResult::ADD_MP || (*it).GetType() == SkillResult::ADD_HP_MP_SP || (*it).GetType() == SkillResult::ADD_STATE ||
				(*it).GetType() == SkillResult::CHAIN_DAMAGE || (*it).GetType() == SkillResult::CHAIN_MAGIC_DAMAGE || (*it).GetType() == SkillResult::CHAIN_HEAL )
			{
				StructCreature::iterator cit = StructCreature::get( (*it).damage.hTarget );
				StructCreature * pDealTarget = (*cit);

				if( pDealTarget && pDealTarget->IsAlive() )
				{
					if( GetSkillBase()->GetSkillEffectType() != SkillBase::EF_ADD_STATE &&
						GetSkillBase()->GetSkillEffectType() != SkillBase::EF_ADD_REGION_STATE &&
						GetSkillBase()->GetSkillEffectType() != SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITH_SHIELD &&
						GetSkillBase()->GetSkillEffectType() != SkillBase::EF_ADD_STATE_BY_SELF_COST &&
						GetSkillBase()->GetSkillEffectType() != SkillBase::EF_ADD_REGION_STATE_BY_SELF_COST &&
						GetSkillBase()->GetSkillEffectType() != SkillBase::EF_ADD_STATE_TO_CASTER_AND_TARGET &&
						GetSkillBase()->GetSkillEffectType() != SkillBase::EF_ADD_RANDOM_STATE &&
						GetSkillBase()->GetSkillEffectType() != SkillBase::EF_ADD_RANDOM_REGION_STATE &&
						GetSkillBase()->GetSkillEffectType() != SkillBase::EF_REMOVE_STATE_GROUP &&
						GetStateId() )
					{
						//if( ( (*it).GetType() == SkillResult::DAMAGE || (*it).GetType() == SkillResult::MAGIC_DAMAGE || (*it).GetType() == SkillResult::DAMAGE_WITH_KNOCK_BACK ) && (*it).damage.flag ^ SkillResult::MISS )
						if( (*it).damage.flag ^ SkillResult::MISS )
						{
							// 지속효과 추가
							vNeedStateList.push_back( pDealTarget );
						}
					}

					ApplyHateAndHavokFromSkillResult( m_pOwner, pDealTarget, *it );
				}
			}

			if( GetSkillUID() >= 0 )
			{
				// 추가 데미지와 추가 데미지의 속성이 아닌, 스킬의 데미지와 스킬의 속성이 핸들러에 영향을 미친다.
				// 핸들러는 무조건 호출이 되고 데미지가 없어서 효과가 호출이 안 되는 경우는 핸들러 내부에서 처리한다.

				int nDamage = 0;
				
				// 스킬 결과 타입이 데미지를 주는 경우에만 데미지를 인자로 넘겨준다.
				if( (*it).GetType() == SkillResult::DAMAGE || (*it).GetType() == SkillResult::MAGIC_DAMAGE || (*it).GetType() == SkillResult::DAMAGE_WITH_KNOCK_BACK || (*it).GetType() == SkillResult::CHAIN_DAMAGE || (*it).GetType() == SkillResult::CHAIN_MAGIC_DAMAGE ) 
					nDamage = (*it).GetDamage();
				
				DWORD nAttackType = 0;
				nAttackType |= GetSkillBase()->IsHarmful()			? StructState::Harmful			: StructState::Helpful;
				nAttackType	|= GetSkillBase()->IsPhysicalSkill()	? StructState::PhysicalSkill	: StructState::MagicalSkill;

				int nElementalType = GetSkillBase()->GetElementalType();

				// 성공/회피/크리티컬/블럭/퍼펙트블럭
				if( (*it).damage.flag ^ SkillResult::MISS )				m_pOwner->OnAttack( pTarget, nDamage, nAttackType, nElementalType, GetSkillId() );
				else													pTarget->OnAvoid( m_pOwner, nAttackType, nElementalType );
				if( (*it).damage.flag & SkillResult::CRITICAL )			m_pOwner->OnCritical( pTarget, nDamage, nAttackType, nElementalType );
				if( (*it).damage.flag & SkillResult::BLOCK )			pTarget->OnBlock( m_pOwner, nDamage, nAttackType, nElementalType );
				if( (*it).damage.flag & SkillResult::PERFECT_BLOCK )	pTarget->OnPerfectBlock( m_pOwner, nAttackType, nElementalType );
			}
		}

		STATE_SKILL_FUNCTOR myStateSkillFunctor( &vResultList );

		for( std::vector< StructCreature * >::iterator it = vNeedStateList.begin(); it != vNeedStateList.end(); ++it )
		{
			process_target( t, myStateSkillFunctor, (*it) );
		}
	}

	// 지속 효과 중 스킬 사용시 효과를 주고 소모성인 녀석들 제거
	int nElementalType = GetSkillBase()->GetElementalType();
	m_pOwner->RemoveExhaustiveSkillStateMod( GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful(), nElementalType, GetOriginalCastingDelay() );

	// C급 보스 이상의 몬스터가 스킬을 발사한 경우 로그 남김
	if( m_pOwner->IsMonster() && static_cast< StructMonster * >( m_pOwner )->GetMonsterType() >= MonsterBase::MONSTER_TYPE_HIGHEST_1_STAR && bIsSuccess )
	{
		StructMonster * pMonster = static_cast< StructMonster * >( m_pOwner );

		if( pTarget )
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

			if( pPlayer )
			{
				LOG::Log11N4S( LM_MONSTER_SKILL_FIRE, pPlayer->GetAccountID(), pPlayer->GetSID(), pTarget->GetX(), pTarget->GetY(), pTarget->GetLayer(),
					pMonster->GetMonsterId(), pMonster->GetX(), pMonster->GetY(), pMonster->GetLayer(),
					GetSkillId(), GetRequestedSkillLevel(),
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
					pTarget->GetName(), LOG::STR_NTS, pMonster->GetName(), LOG::STR_NTS );
			}
			else
			{
				LOG::Log11N4S( LM_MONSTER_SKILL_FIRE, 0, 0, pTarget->GetX(), pTarget->GetY(), pTarget->GetLayer(),
					pMonster->GetMonsterId(), pMonster->GetX(), pMonster->GetY(), pMonster->GetLayer(),
					GetSkillId(), GetRequestedSkillLevel(),
					"", 0, "", 0,
					pTarget->GetName(), LOG::STR_NTS, pMonster->GetName(), LOG::STR_NTS );
			}
		}
		else
		{
			LOG::Log11N4S( LM_MONSTER_SKILL_FIRE, 0, 0, 0, 0, 0,
				pMonster->GetMonsterId(), pMonster->GetX(), pMonster->GetY(), pMonster->GetLayer(),
				GetSkillId(), GetRequestedSkillLevel(),
				"", 0, "", 0,
				"", 0, pMonster->GetName(), LOG::STR_NTS );
		}
	}
}

void StructSkill::ApplyHateAndHavokFromSkillResult( StructCreature* source, StructCreature* target, const SkillResult& result )
{
	if( !source->IsMonster() && GetSkillBase()->GetEffectType() != SkillBase::EF_REMOVE_HATE && GetSkillBase()->GetEffectType() != SkillBase::EF_REGION_REMOVE_HATE && GetSkillBase()->GetEffectType() != SkillBase::EF_REGION_REMOVE_HATE_OF_TARGET )
	{
		int pt = 0;

		if( result.GetType() == SkillResult::DAMAGE || result.GetType() == SkillResult::MAGIC_DAMAGE ||  result.GetType() == SkillResult::DAMAGE_WITH_KNOCK_BACK || result.GetType() == SkillResult::CHAIN_DAMAGE || result.GetType() == SkillResult::CHAIN_MAGIC_DAMAGE )
		{
			pt = result.GetDamage();
		}
		else if( result.GetType() == SkillResult::ADD_HP || result.GetType() == SkillResult::ADD_MP || result.GetType() == SkillResult::CHAIN_HEAL )
		{
			pt = result.add_hp.nIncHP;
		}
		else if( result.GetType() == SkillResult::ADD_HP_MP_SP )
		{
			pt = result.add_hp_mp_sp.nIncHP;
			pt += result.add_hp_mp_sp.nIncMP;
			pt += result.add_hp_mp_sp.nIncSP;
		}

		int nAddHate = source->GetHateRatio() * GetHatePoint( GetRequestedSkillLevel(), pt, GetEnhance() ) * source->GetMagicalHateMod( (Elemental::Type) GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

		std::pair< float, int > HateMod = source->GetHateMod( ( GetSkillBase()->IsPhysicalSkill() ) ? 1 : 2, GetSkillBase()->IsHarmful() );

		nAddHate += HateMod.second;
		nAddHate *= HateMod.first;

		// Hate 수치를 먹긴 해야 하는데 0으로 나오면 데미지는 주지 않고 디버프만 거는 스킬이므로 최소한 1의 hate를 줌
		if( !nAddHate )
			++nAddHate;

		if( target->IsMonster() )
		{
			StructMonster *pMonster = static_cast< StructMonster * >( target );

			pMonster->AddHate( source->GetHandle(), nAddHate );
		}
		else if( target->IsNPC() )
		{
			StructNPC * pNPC = static_cast< StructNPC * >( target );

			pNPC->SetAttacker( source );
		}
		else if( !target->IsEnemy( source ) && !source->IsEnemy( target ) )
		{
			target->AddHateToEnemyList( source->GetHandle(), nAddHate );
		}

		if (source->IsEnemy(target) && (source->GetAttackLuna() != 0) && (target->GetLuna() == 0))
		{
			//&&
			//(cVar3 = (**(*param_1 + 0x240))(param_1, param_2), cVar3 != '\0')) 
			//uVar8 = GetArTime();
			//uVar7 = (***param_1)(param_1);
			target->AddState(StructState::LUNA_CHIP, source->GetHandle(), 1, GetArTime(), GetArTime() + 18000);
		}
	}
}

void StructSkill::ProcSkill()
{
	if( m_Status == STATUS_IDLE )
		return;

	AR_TIME t = GetArTime();

	StructCreature *pTarget = NULL;
	GameObject * pRawTarget = NULL;

	GameObject::iterator itSkillTarget;

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pOwner ) );

		if( m_pOwner->IsDead() )
		{
			m_pOwner->CancelSkill();
			return;
		}

		itSkillTarget = GameObject::get( m_hTarget );

		pRawTarget = *itSkillTarget;

		if( m_Status != STATUS_COMPLETE && pRawTarget )
		{
			if( pRawTarget->IsCreature() )
			{
				pTarget = static_cast< StructCreature * >( *itSkillTarget );

				// 시야 거리 밖으로 나갔으면 스킬 캔슬
				if( pTarget->GetPos().GetDistance( m_pOwner->GetPos() ) > GameRule::VISIBLE_RANGE )
				{
					m_pOwner->CancelSkill();
					return;
				}
				// 대상이 죽었으면 시전 불가
				if( pTarget->IsDead() && !GetSkillBase()->IsValidToCorpse() )
				{
					m_pOwner->CancelSkill();
					return;
				}
				// 시체 전용 스킬인데 대상이 살아있으면(캐스팅 중에 부활할 수도 있다) 중지
				if( !pTarget->IsDead() && GetSkillBase()->IsValidToCorpse() )
				{
					m_pOwner->CancelSkill();
					return;
				}
			}
			else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ACTIVATE_FIELD_PROP ||
				GetSkillBase()->GetSkillEffectType() == SkillBase::EF_REGION_HEAL_BY_FIELD_PROP ||
				GetSkillBase()->GetSkillEffectType() == SkillBase::EF_AREA_EFFECT_HEAL_BY_FIELD_PROP )
			{
				// 있을 리가 없지만 -_ -;
				if( !m_pOwner->IsPlayer() || !pRawTarget->IsFieldProp() )
				{
					m_pOwner->CancelSkill();
					return;
				}

				StructFieldProp * pProp = static_cast< StructFieldProp * >( pRawTarget );
				if( !pProp || !IsVisibleRegion( m_pOwner->GetRX(), m_pOwner->GetRY(), pProp->GetRX(), pProp->GetRY() ) ||
					!pProp->IsUsable( static_cast< StructPlayer * >( m_pOwner ) ) )
				{
					m_pOwner->CancelSkill();
					return;
				}
			}
		}

		if( m_hTarget && !pRawTarget )
		{
			m_pOwner->CancelSkill();
			return;
		}

		// 펫 삽질일 경우 추가 처리
		if( m_pOwner->IsPet() && GetSkillId() == SKILL_SHOVELING && m_Status == STATUS_CAST )
		{
			StructPet *pPet = static_cast< StructPet * >( m_pOwner );
			StructPlayer *pMaster = pPet->GetMaster();

			// 주인과 일정 거리 이상 멀어지면 중지
			if( !pMaster || !pMaster->IsInWorld() || pMaster->GetPos().GetDistance( m_pOwner->GetPos() ) > GameRule::PET_SHOVELING_LIMIT_DISTANCE_FROM_MASTER )
			{
				m_pOwner->CancelSkill();
				return;
			}

			// 단계 변화 처리
			switch( pPet->GetShovelingStatus() )
			{
			case StructPet::SHOVELING_STATUS_SEARCH:
				if( t >= m_nCastTime + GameRule::PET_SHOVELING_SEARCH_DURATION )
				{
					// 이동 경로 설정
					std::vector< ArPosition > vPath;
					ArPosition pos = m_pOwner->GetPos();
					while( vPath.size() < 2 )
					{
						int nDistance = ( GameRule::PET_SHOVELING_LIMIT_ROAMING_DISTANCE / 2 );
						pos.x += ( XRandom() % nDistance - nDistance/2 );
						pos.y += ( XRandom() % nDistance - nDistance/2 );

						vPath.push_back( pos );
					}

					m_pOwner->SetMultipleMove( vPath, GameRule::PET_SHOVELING_APPROACH_SPEED );
					static_cast< StructPet * >( m_pOwner )->SetShovelingStatus( StructPet::SHOVELING_STATUS_APPROACH );
					BroadcastStatusMessage( m_pOwner );
				}
				break;
			case StructPet::SHOVELING_STATUS_APPROACH:
				if( t >= m_nCastTime + GameRule::PET_SHOVELING_SEARCH_DURATION + GameRule::PET_SHOVELING_APPROACH_DURATION )
				{
					static_cast< StructPet * >( m_pOwner )->SetShovelingStatus( StructPet::SHOVELING_STATUS_DIG );
					BroadcastStatusMessage( m_pOwner );
				}
				break;
			default:
				assert( 0 );
				break;
			}
		}

		if( t < m_nFireTime )
			return;

		if( m_Status == STATUS_CAST )
		{
			int nElementalType = GetSkillBase()->GetElementalType();

			m_pOwner->PrepareRemoveExhaustiveSkillStateMod( GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful(), nElementalType, GetOriginalCastingDelay() );

			// 캐스팅 중이면
			m_Status = STATUS_FIRE;
		}
	}

	ArcadiaAutoLock _lock;

	if( pRawTarget && pRawTarget->IsInWorld() )
	{
		ArPosition top_left, bottom_right;

		const SkillBase *pSkillBase = GetSkillBase();
		int nEnhance = GetEnhance();
		int nSLV = GetRequestedSkillLevel();

		unsigned int rx1 = m_pOwner->GetRX();
		unsigned int ry1 = m_pOwner->GetRY();
		short layer1 = m_pOwner->GetLayer();
		unsigned int rx2 = pRawTarget->GetRX();
		unsigned int ry2 = pRawTarget->GetRY();
		short layer2 = pRawTarget->GetLayer();

		// 최초에는 락이 걸리지 않은 기본 값으로 락 핸들을 -1을 지정함(Unlock의 영향을 받지 않음)
		ArcadiaLock _region_lock( -1 );

		do
		{
			ArcadiaServer::Instance().UnLock( &_region_lock );

			rx1 = m_pOwner->GetRX();
			ry1 = m_pOwner->GetRY();
			layer1 = m_pOwner->GetLayer();
			rx2 = pRawTarget->GetRX();
			ry2 = pRawTarget->GetRY();
			layer2 = pRawTarget->GetLayer();

			short lock_layer = ( layer1 == layer2 ) ? layer1 : -1;

			top_left.x = std::min( m_pOwner->GetX(), pRawTarget->GetX() );
			top_left.y = std::min( m_pOwner->GetY(), pRawTarget->GetY() );
			bottom_right.x = std::max( m_pOwner->GetX(), pRawTarget->GetX() );
			bottom_right.y = std::max( m_pOwner->GetY(), pRawTarget->GetY() );

			if( ( pSkillBase->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_KNOCKBACK_OLD ||
				pSkillBase->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_OLD ||
				pSkillBase->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITHOUT_WEAPON_RUSH_KNOCK_BACK ||
				pSkillBase->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK_OLD ) &&
				( !pRawTarget->IsCreature() || static_cast< StructCreature * >(pRawTarget)->IsKnockbackable() ) )
			{
				float fKnockBackRange = pSkillBase->GetVar(5) + nSLV * pSkillBase->GetVar(6) + pSkillBase->GetVar(10) * nEnhance;
				fKnockBackRange *= GameRule::DEFAULT_UNIT_SIZE;

				top_left.x = std::min( top_left.x, pRawTarget->GetX() - fKnockBackRange );
				top_left.y = std::min( top_left.y, pRawTarget->GetY() - fKnockBackRange );
				bottom_right.x = std::max( bottom_right.x, pRawTarget->GetX() + fKnockBackRange );
				bottom_right.y = std::max( bottom_right.y, pRawTarget->GetY() + fKnockBackRange );
			}
			else if( pSkillBase->GetSkillEffectType() == SkillBase::EF_PHYSICAL_CHAIN_DAMAGE ||
				pSkillBase->GetSkillEffectType() == SkillBase::EF_MAGIC_CHAIN_DAMAGE ||
				pSkillBase->GetSkillEffectType() == SkillBase::EF_CHAIN_HEAL )
			{
				// 대상의 범위는 시전자를 기준으로 시전 범위의 두 배이다.
				// 시전자와 대상은 무조건 락 안에 들어간다.
				float fChainRange = pSkillBase->GetCastRange() * 2 * GameRule::DEFAULT_UNIT_SIZE;

				top_left.x = m_pOwner->GetX() - fChainRange;
				top_left.y = m_pOwner->GetY() - fChainRange;
				bottom_right.x = m_pOwner->GetX() + fChainRange;
				bottom_right.y = m_pOwner->GetY() + fChainRange;
			}

			_region_lock = ArcadiaServer::Instance().LockArea( GetRegionX( top_left.x ), GetRegionY( top_left.y ), GetRegionX( bottom_right.x ), GetRegionY( bottom_right.y ), lock_layer );
		}
		while( rx1 != m_pOwner->GetRX() || ry1 != m_pOwner->GetRY() || layer1 != m_pOwner->GetLayer() ||
			rx2 != pRawTarget->GetRX() || ry2 != pRawTarget->GetRY() || layer2 != pRawTarget->GetLayer() );

		_lock.set( _region_lock, __FILE__, __LINE__ );

		// 락 걸고 난 이후에 타겟이 월드에 없다면 스킬 캔슬
		if( !pRawTarget->IsInWorld() )
		{
			m_pOwner->CancelSkill();
			m_hTarget = 0;
			broadcastSkillMessage( rx1, ry1, m_pOwner->GetLayer(), 0, 0, TS_SC_SKILL::CANCEL );
			return;
		}
	}
	// 역소환 관련 처리가 StructPlayer::UnSummon를 직접 호출하는 게 아니라 StructPlayer::PendUnSummon 함수로 모두 대체되었으므로
	// 소환수가 있는 리전을 포함하는 락을 걸 필요 없이 역소환 스킬 시전자(소환수 주인) 시야 범위 내에만 락을 걸면 됨
	// * 스킬이야 여기서 락 범위 예측해서 조정하면 되지만 사망 시 역소환같은 건 락 범위를 미리 조절할 방법이 없으므로
	//   역소환 처리 자체가 해당 쓰레드에서 바로 처리되지 못하고 Pend만 걸어두고 StructPlayer::onProcess에서 처리하도록 해야 함
	// * EF_UNSUMMON_AND_ADD_STATE의 지속효과 방송 때문에 시야 범위 락은 필요
	/*else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_UNSUMMON ||
		GetSkillBase()->GetSkillEffectType() == SkillBase::EF_UNSUMMON_AND_ADD_STATE )
	{
		// exception process when unsummon
		ArPosition top_left, bottom_right;

		StructSummon * pMainSummon = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon();

		if( !m_pOwner->IsPlayer() || !pMainSummon || !pMainSummon->IsInWorld() )
		{
			m_pOwner->CancelSkill();
			return;
		}

		top_left.x = std::min( m_pOwner->GetX(), pMainSummon->GetX() );
		top_left.y = std::min( m_pOwner->GetY(), pMainSummon->GetY() );
		bottom_right.x = std::max( m_pOwner->GetX(), pMainSummon->GetX() );
		bottom_right.y = std::max( m_pOwner->GetY(), pMainSummon->GetY() );

		StructSummon * pSubSummon = static_cast< StructPlayer * >( m_pOwner )->GetSubSummon();

		if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_UNSUMMON_AND_ADD_STATE &&
			pSubSummon && pSubSummon->IsInWorld() )
		{
			top_left.x = std::min( top_left.x, pSubSummon->GetX() );
			top_left.y = std::min( top_left.y, pSubSummon->GetY() );
			bottom_right.x = std::max( bottom_right.x, pSubSummon->GetX() );
			bottom_right.y = std::max( bottom_right.y, pSubSummon->GetY() );
		}

		_lock.set( ArcadiaServer::Instance().LockArea( GetRegionX( top_left.x ), GetRegionY( top_left.y ), GetRegionX( bottom_right.x ), GetRegionY( bottom_right.y ) ), __FILE__, __LINE__ );

		pMainSummon = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon();

		if( !pMainSummon || !pMainSummon->IsInWorld() )
		{
			m_pOwner->CancelSkill();
			return;
		}
	}*/
	// 지면 타겟 스킬인 경우 시전자와 지면 타겟 지점 기준으로 지역 락 범위 증가
	else if( GetSkillBase()->IsNeedTarget() == 2 )
	{
		// 최초에는 락이 걸리지 않은 기본 값으로 락 핸들을 -1을 지정함(Unlock의 영향을 받지 않음)
		ArcadiaLock _region_lock( -1 );

		unsigned int rx1 = m_pOwner->GetRX();
		unsigned int ry1 = m_pOwner->GetRY();
		unsigned int rx2 = m_targetPos.GetRX();
		unsigned int ry2 = m_targetPos.GetRY();
		short layer = m_pOwner->GetLayer();

		do
		{
			ArcadiaServer::Instance().UnLock( &_region_lock );

			rx1 = m_pOwner->GetRX();
			ry1 = m_pOwner->GetRY();
			layer = m_pOwner->GetLayer();

			_region_lock = ArcadiaServer::Instance().LockArea( rx1, ry1, rx2, ry2, layer );
		}
		while( rx1 != m_pOwner->GetRX() || ry1 != m_pOwner->GetRY() || layer != m_pOwner->GetLayer() );

		_lock.set( _region_lock, __FILE__, __LINE__ );
	}
	else
	{
		_lock.set( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pOwner ), __FILE__, __LINE__ );
	}

	if( pTarget && !pTarget->IsInWorld() )
	{
		m_pOwner->CancelSkill();
		m_hTarget = 0;
		broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), m_pOwner->GetLayer(), 0, 0, TS_SC_SKILL::CANCEL );
		return;
	}

	// 필드 프랍 사용 스킬의 경우 위의 조건으로 프랍이 월드에서 없어졌는지 체크가 불가능(pTarget은 pRawTarget이 Creature일 때만 세팅)
	if( ( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_ACTIVATE_FIELD_PROP ||
		GetSkillBase()->GetSkillEffectType() == SkillBase::EF_REGION_HEAL_BY_FIELD_PROP ||
		GetSkillBase()->GetSkillEffectType() == SkillBase::EF_AREA_EFFECT_HEAL_BY_FIELD_PROP ) &&
		( !pRawTarget || !pRawTarget->IsInWorld() ) )
	{
		m_pOwner->CancelSkill();
		m_hTarget = 0;
		broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), m_pOwner->GetLayer(), 0, 0, TS_SC_SKILL::CANCEL );
		return;
	}

	if( m_Status == STATUS_FIRE )
	{
		bool bSuccess = true;

		int cost_hp = 0;
		int cost_mp = 0;
		int cost_exp = 0;
		int cost_jp = 0;

		if( !m_bMultiple || m_nCurrentFire == 0 )
		{
			// 아이템으로 스킬 사용시에는 쿨타임을 세팅하지 않음
			if( !m_hCastItem )
			{
				AR_TIME next_cool_time = GetSkillCoolTime();
				SetRemainCoolTime( next_cool_time );

				if( GetSkillBase()->GetCoolTimeGroupId() )
				{
					struct _mySkillFunctor : StructCreature::SKILL_POINTER_FUNCTOR
					{
						_mySkillFunctor( int _cool_time_group_id, AR_TIME _next_cool_time ) : cool_time_group_id( _cool_time_group_id ), next_cool_time( _next_cool_time ) {}

						virtual void onSkill( StructSkill* pSkill )
						{

							if( pSkill->GetSkillBase()->GetCoolTimeGroupId() == cool_time_group_id )
								pSkill->SetRemainCoolTime( next_cool_time );
						}

						int cool_time_group_id;
						AR_TIME next_cool_time;
					} _fo( GetSkillBase()->GetCoolTimeGroupId(), next_cool_time );

					m_pOwner->EnumActiveSkill( _fo );
				}
			}

			StructPlayer * pClient = NULL;

			if( m_pOwner->IsPlayer() )
			{
				pClient = static_cast< StructPlayer * >( m_pOwner );
			}
			else if( m_pOwner->IsSummon() )
			{
				pClient = static_cast< StructSummon * >( m_pOwner )->GetMaster();
			}

			if( pClient )
			{
				SendSkillMessage( pClient, m_pOwner, GetSkillId() );
			}

			// 비용 소비
			if( ( GetSkillBase()->GetEffectType() == SkillBase::EF_TOGGLE_AURA ||
				GetSkillBase()->GetEffectType() == SkillBase::EF_TOGGLE_DIFFERENTIAL_AURA ) && m_pOwner->IsActiveAura( this ) )
			{
				cost_mp = 0;
			}
			else
			{
				cost_mp = CalculateMPCost( m_pOwner->GetSkillCostModifier() );
				cost_mp -= cost_mp * GameRule::SKILL_CAST_COST;
			}

			cost_hp = CalculateHPCost( m_pOwner->GetSkillCostModifier() );
			cost_exp = GetCostEXP( GetRequestedSkillLevel(), GetEnhance() );
			cost_jp = GetCostJP( GetRequestedSkillLevel(), GetEnhance() );

			if( cost_hp > 0 ) m_pOwner->SetHP( m_pOwner->GetHP() - cost_hp );
			if( cost_mp > 0 ) m_pOwner->SetMP( m_pOwner->GetMP() - cost_mp );
			if( cost_exp > 0 ) m_pOwner->SetEXP( m_pOwner->GetEXP() - cost_exp );
			if( cost_jp > 0 ) m_pOwner->SetJP( m_pOwner->GetJobPoint() - cost_jp );

			m_pOwner->RemoveEnergy( static_cast< int >( GetCostEnergy( GetRequestedSkillLevel() ) ) );

			// 크리처 피 빼야하는 스킬들
			if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_T1_DEAL_SUMMON_HP_OLD )
			{
				int decHP = GetVar( 6 ) + GetVar( 7 ) * GetRequestedSkillLevel() + GetVar( 8 ) * GetEnhance();

				if( m_pOwner->IsPlayer() ||
					static_cast< StructPlayer * >( m_pOwner )->GetMainSummon() )
				{
					StructSummon * pSummon = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon();

					int nPrevHP = pSummon->GetHP();
					pSummon->AddHP( 0 - std::min( pSummon->GetHP() - 1, decHP ) );

					BroadcastHPMPMsg( pSummon, pSummon->GetHP() - nPrevHP, 0 );
				}
			}
			else if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_MULTIPLE_DAMAGE_DEAL_SUMMON_HP )
			{
				int decHP = GetVar( 9 ) + GetVar( 10 ) * GetRequestedSkillLevel() + GetVar( 11 ) * GetEnhance();

				if( m_pOwner->IsPlayer() ||
					static_cast< StructPlayer * >( m_pOwner )->GetMainSummon() )
				{
					StructSummon * pSummon = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon();

					int nPrevHP = pSummon->GetHP();
					pSummon->AddHP( 0 - std::min( pSummon->GetHP() - 1, decHP ) );

					BroadcastHPMPMsg( pSummon, pSummon->GetHP() - nPrevHP, 0 );
				}
			}

			ItemBase::ItemCode cost_item_code = GetCostItemCode();
			__int64 nItemCostCount = GetCostItemCount( GetRequestedSkillLevel() );

			StructPlayer *pInvenPlayer = NULL;
			if( m_pOwner->IsPlayer() ) pInvenPlayer = static_cast< StructPlayer* >( m_pOwner );
			if( m_pOwner->IsSummon() ) pInvenPlayer = static_cast< StructSummon* >( m_pOwner )->GetMaster();

			if( pInvenPlayer )
			{
				if( cost_item_code )
				{				
					if( cost_item_code == ItemBase::ITEM_CODE_WEARED_BULLET )
					{
						bSuccess = pInvenPlayer->EraseBullet( nItemCostCount );
					}
					else
					{
						StructItem * pCostItem = NULL;

						pCostItem = pInvenPlayer->FindItem( cost_item_code );

						if( pCostItem && pCostItem->GetCount() >= nItemCostCount )
						{
							pInvenPlayer->EraseItem( pCostItem, nItemCostCount );
						}
						else
						{
							bSuccess  = false;
						}
					}
				}

				if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_STATE_BY_USING_ITEM )
				{
					if( GetRequestedSkillLevel() > 0 && GetRequestedSkillLevel() < 20 )
					{
						ItemBase::ItemCode requiredItemCode = GetVar( 0 );
						__int64 requiredItemCount = GetVar( GetRequestedSkillLevel() );
						StructItem* costItem = pInvenPlayer->FindItem( requiredItemCode );

						if ( costItem && requiredItemCount <= costItem->GetCount() )
						{
							pInvenPlayer->EraseItem( costItem, requiredItemCount );
						}
						else
						{
							bSuccess  = false;
						}
					}
					else
					{
						bSuccess  = false;				
					}
				}
			}
		}

		FireSkill( pTarget, bSuccess );

		if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_CHAIN_DAMAGE ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_MAGIC_CHAIN_DAMAGE ||
			GetSkillBase()->GetSkillEffectType() == SkillBase::EF_CHAIN_HEAL )
		{
			broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), GetCastRange() * 2 * GameRule::DEFAULT_UNIT_SIZE, m_pOwner->GetLayer(), cost_hp, cost_mp, TS_SC_SKILL::FIRE );
		}
		else if( pTarget )
		{
			broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), pTarget->GetRX(), pTarget->GetRY(), m_pOwner->GetLayer(), cost_hp, cost_mp, TS_SC_SKILL::FIRE );
		}
		else
		{		
			broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), m_pOwner->GetLayer(), cost_hp, cost_mp, TS_SC_SKILL::FIRE );
		}

		if( !m_bMultiple || m_nCurrentFire == m_nTotalFire )
		{
			m_nFireTime += GetCommonDelay();

			m_Status = STATUS_COMPLETE;
			m_hCastItem = 0;

			// 소울스톤 내구도 감소
			for( int pos = 0 ; pos < 2 ; ++pos )
			{
				const ItemBase::ItemWearType WeaponIndices[ 2 ] = { ItemBase::WEAR_RIGHTHAND, ItemBase::WEAR_LEFTHAND };

				ItemBase::ItemWearType idx = WeaponIndices[ pos ];

				StructItem *pItem = m_pOwner->GetWearedItem( idx );
				if( pItem && pItem->GetCurrentEndurance() > 0 && pItem->GetUsingSocketCount() > 0 )
				{
					pItem->SetCurrentEndurance( pItem->GetCurrentEndurance() - GameRule::ENDURANCE_REDUCE_ON_SKILL );
					if( m_pOwner->IsPlayer() )
						SendItemMessage( static_cast< StructPlayer * >( m_pOwner ), pItem );

					if( pItem->GetCurrentEndurance() == 0 )
						m_pOwner->SetNeedCalculateStat();
				}
			}
		}
	}

	if( m_Status == STATUS_COMPLETE )
	{
		// 스킬 시전할 때 이미 시간이 좀 흘렀을테니, 한번 더 체크해주자.
		if( GetArTime() < m_nFireTime )
			return;

		// 스킬 시전 완료~
		if( pTarget )
		{
			broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), pTarget->GetRX(), pTarget->GetRY(), m_pOwner->GetLayer(), 0, 0, TS_SC_SKILL::COMPLETE );
		}
		else
		{		
			broadcastSkillMessage( m_pOwner->GetRX(), m_pOwner->GetRY(), m_pOwner->GetLayer(), 0, 0, TS_SC_SKILL::COMPLETE );
		}

		m_pOwner->OnCompleteSkill();

		Init();
	}
}

void StructSkill::process_target( AR_TIME t, struct SKILL_TARGET_FUNCTOR & functor, struct StructCreature * pTarget )
{
	switch( GetSkillBase()->GetSkillTargetType() )
	{		
	case SkillBase::TARGET_TARGET:
	case SkillBase::TARGET_TARGET_EXCEPT_CASTER:
		{
			if( GetSkillBase()->GetSkillTargetType() == SkillBase::TARGET_TARGET_EXCEPT_CASTER &&
				pTarget == m_pOwner )
			{
				break;
			}

			if( IsNeedTarget() == SkillBase::NEED_TARGET_TARGET )
			{
				// 대상에게 사용가능하다면
				if( pTarget ) functor.onCreature( this, t, m_pOwner, pTarget );
			}
			else if( IsNeedTarget() == SkillBase::NEED_TARGET_SELF )
			{
				// 아니면 스스로에게 사용
				functor.onCreature( this, t, m_pOwner, m_pOwner );
			}
			break;
		}
	case SkillBase::TARGET_REGION:
	case SkillBase::TARGET_REGION_WITH_TARGET:
	case SkillBase::TARGET_REGION_WITHOUT_TARGET:
		{
			if( pTarget ) functor.onCreature( this, t, m_pOwner, pTarget );
			break;
		}
	case SkillBase::TARGET_PARTY: 
		{
			if( !m_pOwner->IsPlayer() || !pTarget->IsPlayer() ) break;

			StructPlayer* pPlayer = static_cast< StructPlayer* >( m_pOwner );
			StructPlayer* pTargetPlayer = static_cast< StructPlayer* >( pTarget );

			// 파티가 있는데 대상이 내 파티가 아니면 즐
			if( pPlayer != pTargetPlayer && pPlayer->GetPartyID() && pPlayer->GetPartyID() != pTargetPlayer->GetPartyID() ) break;

			struct myPartyFunctor : public PartyManager::PartyFunctor
			{
				myPartyFunctor( StructSkill *_pSkill, AR_TIME _t, StructCreature *_pOwner, SKILL_TARGET_FUNCTOR &_functor ) : pSkill( _pSkill ), t( _t ), pOwner( _pOwner ), partyFunctor( _functor ) {};

				virtual bool operator()( AR_HANDLE handle )
				{
					StructCreature::iterator it = StructCreature::get( handle );
					if( !pSkill || !(*it) || !(*it)->IsPlayer() )
						return false;

					// 적용 거리 체크
					ArPosition pos = pOwner->GetPos();
					if( pSkill->GetFireRange() < pos.GetDistance( (*it)->GetPos() ) )
						return false;

					return partyFunctor.onCreature( pSkill, t, pOwner, (*it) );
				}

			private:
				struct SKILL_TARGET_FUNCTOR &partyFunctor;
				StructSkill *pSkill;
				StructCreature *pOwner;
				AR_TIME t;
			} fo( this, t, m_pOwner, functor );

			// 파티가 없으면 자기 자신만 타겟
			if( pPlayer->GetPartyID() == 0 )
			{
				fo( pPlayer->GetHandle() );
			}
			// 있으면 파티가 타겟
			else
			{
				m_nTargetCount = static_cast<int>( PartyManager::GetInstance().DoEachMember( pPlayer->GetPartyID(), fo ) );
			}

			break;
		}
	case SkillBase::TARGET_GUILD:
		{
			if( !m_pOwner->IsPlayer() || !pTarget->IsPlayer() ) break;

			StructPlayer* pPlayer = static_cast< StructPlayer* >( m_pOwner );
			StructPlayer* pTargetPlayer = static_cast< StructPlayer* >( pTarget );

			// 길드가 있는데 대상이 내 길드가 아니면 즐
			if( pPlayer != pTargetPlayer && pPlayer->GetGuildID() && pPlayer->GetGuildID() != pTargetPlayer->GetGuildID() ) break;

			struct myGuildFunctor : public GuildManager::GuildFunctor
			{
				myGuildFunctor( StructSkill *_pSkill, AR_TIME _t, StructCreature *_pOwner, SKILL_TARGET_FUNCTOR &_functor ) : pSkill( _pSkill ), t( _t ), pOwner( _pOwner ), guildFunctor( _functor ) {};

				virtual bool operator()( AR_HANDLE handle )
				{
					StructCreature::iterator it = StructCreature::get( handle );
					if( !pSkill || !(*it) || !(*it)->IsPlayer() )
						return false;

					// 적용 거리 체크
					ArPosition pos = pOwner->GetPos();
					if( pSkill->GetFireRange() < pos.GetDistance( (*it)->GetPos() ) )
						return false;

					return guildFunctor.onCreature( pSkill, t, pOwner, (*it) );
				}

			private:
				struct SKILL_TARGET_FUNCTOR &guildFunctor;
				StructSkill *pSkill;
				StructCreature *pOwner;
				AR_TIME t;
			} fo( this, t, m_pOwner, functor );

			// 길드가 없으면 자기 자신만 타겟
			if( pPlayer->GetGuildID() == 0 )
			{
				fo( pPlayer->GetHandle() );
			}
			// 있으면 길드가 타겟
			else
			{
				m_nTargetCount = static_cast<int>( GuildManager::GetInstance().DoEachMember( pPlayer->GetGuildID(), fo ) );
			}

			break;
		}
	case SkillBase::TARGET_ATTACKTEAM: 
		{
			if( !m_pOwner->IsPlayer() || !pTarget->IsPlayer() ) break;

			StructPlayer* pPlayer = static_cast< StructPlayer* >( m_pOwner );
			StructPlayer* pTargetPlayer = static_cast< StructPlayer* >( pTarget );

			// 파티가 있는데 대상이 내 파티가 아니면 즐
			if( pPlayer != pTargetPlayer && pPlayer->GetPartyID() && pPlayer->GetPartyID() != pTargetPlayer->GetPartyID() ) break;

			struct myPartyFunctor : public PartyManager::PartyFunctor
			{
				myPartyFunctor( StructSkill *_pSkill, AR_TIME _t, StructCreature *_pOwner, SKILL_TARGET_FUNCTOR &_functor ) : pSkill( _pSkill ), t( _t ), pOwner( _pOwner ), partyFunctor( _functor ) {};

				virtual bool operator()( AR_HANDLE handle )
				{
					StructCreature::iterator it = StructCreature::get( handle );
					if( !pSkill || !(*it) || !(*it)->IsPlayer() )
						return false;

					// 적용 거리 체크
					ArPosition pos = pOwner->GetPos();
					if( pSkill->GetFireRange() < pos.GetDistance( (*it)->GetPos() ) )
						return false;

					return partyFunctor.onCreature( pSkill, t, pOwner, (*it) );
				}

			private:
				struct SKILL_TARGET_FUNCTOR &partyFunctor;
				StructSkill *pSkill;
				StructCreature *pOwner;
				AR_TIME t;
			} fo( this, t, m_pOwner, functor );

			// 파티/공대가 있으면 파티가 타겟
			int nPartyID = pPlayer->GetPartyID();
			int nAttackTeamLeadPartyID = PartyManager::GetInstance().GetLinkedPartyLeadPartyID( pPlayer->GetPartyID() );
			if( nAttackTeamLeadPartyID )
			{
				m_nTargetCount = static_cast<int>( PartyManager::GetInstance().DoEachLinkedPartyMember( nAttackTeamLeadPartyID, fo ) );
			}
			else if( nPartyID )
			{
				m_nTargetCount = static_cast<int>( PartyManager::GetInstance().DoEachMember( nPartyID, fo ) );
			}
			// 파티/공대가 없으면 자기 자신만 타겟
			else
			{
				fo( pPlayer->GetHandle() );
				m_nTargetCount = 1;
			}

			break;
		}
	case SkillBase::TARGET_SUMMON:
		{
			StructSummon *pSummon = NULL;

			if( pTarget->IsSummon() && pTarget->IsInWorld() )
				pSummon = static_cast< StructSummon * >( pTarget );
			else if( pTarget->IsPlayer() && static_cast< StructPlayer * >( pTarget )->GetMainSummon() && static_cast< StructPlayer * >( pTarget )->GetMainSummon()->IsInWorld() )
				pSummon = static_cast< StructSummon * >( static_cast< StructPlayer * >( pTarget )->GetMainSummon() );

			if( !pSummon )
			{
				break;
			}

			ArPosition pos = pSummon->GetPos();
			if( GetFireRange() < pos.GetDistance( pTarget->GetPos() ) )
				break;

			functor.onCreature( this, t, m_pOwner, pSummon );
			break;
		}
	case SkillBase::TARGET_PARTY_SUMMON:
		{
			if( !m_pOwner->IsPlayer() ) break;

			StructPlayer* pPlayer = static_cast< StructPlayer* >( m_pOwner );

			struct myPartyFunctor : public PartyManager::PartyFunctor
			{
				myPartyFunctor( StructSkill *_pSkill, AR_TIME _t, StructCreature *_pOwner, SKILL_TARGET_FUNCTOR &_functor ) : pSkill( _pSkill ), t( _t ), pOwner( _pOwner ), partyFunctor( _functor ) {};

				virtual bool operator()( AR_HANDLE handle )
				{
					StructCreature::iterator it = StructCreature::get( handle );
					if( !pSkill || !(*it) || !(*it)->IsPlayer() || !static_cast< StructPlayer *>(*it)->GetMainSummon() || !static_cast< StructPlayer *>(*it)->GetMainSummon()->IsInWorld() )
						return false;

					// 적용 거리 체크
					ArPosition pos = pOwner->GetPos();
					if( pSkill->GetFireRange() < pos.GetDistance( static_cast< StructPlayer *>(*it)->GetMainSummon()->GetPos() ) )
						return false;

					return partyFunctor.onCreature( pSkill, t, pOwner, static_cast< StructPlayer * >(*it)->GetMainSummon() );
				}

			private:
				struct SKILL_TARGET_FUNCTOR &partyFunctor;
				StructSkill *pSkill;
				StructCreature *pOwner;
				AR_TIME t;
			} fo( this, t, m_pOwner, functor );

			// 파티가 없으면 자기 자신만 타겟
			if( pPlayer->GetPartyID() == 0 )
			{
				fo( pPlayer->GetHandle() );
			}
			// 있으면 파티가 타겟
			else
			{
				m_nTargetCount = static_cast<int>( PartyManager::GetInstance().DoEachMember( pPlayer->GetPartyID(), fo ) );
			}

			break;
		}
	case SkillBase::TARGET_SELF_WITH_SUMMON:
		{
			StructPlayer *pTargetPlayer = NULL;

			if( pTarget->IsPlayer() )
				pTargetPlayer = static_cast< StructPlayer * >( pTarget );
			else if( pTarget->IsSummon() && static_cast< StructSummon * >( pTarget )->GetMaster() )
				pTargetPlayer = static_cast< StructSummon * >( pTarget )->GetMaster();

			if( !pTargetPlayer || !pTargetPlayer->IsInWorld() )
				break;

			functor.onCreature( this, t, m_pOwner, pTargetPlayer );

			ArPosition pos = m_pOwner->GetPos();
			StructSummon *pSummon = pTargetPlayer->GetMainSummon();
			if( pSummon && pSummon->IsInWorld() && pSummon->IsAlive() && GetFireRange() >= pos.GetDistance( pSummon->GetPos() ) )
				functor.onCreature( this, t, m_pOwner, pSummon );

			pSummon = pTargetPlayer->GetSubSummon();
			if( pSummon && pSummon->IsInWorld() && pSummon->IsAlive() && GetFireRange() >= pos.GetDistance( pSummon->GetPos() ) )
				functor.onCreature( this, t, m_pOwner, pTargetPlayer->GetSubSummon() );

			break;
		}
	case SkillBase::TARGET_PARTY_WITH_SUMMON:
		{
			if( !( m_pOwner->IsPlayer() || m_pOwner->IsSummon() ) || !( pTarget->IsPlayer() || pTarget->IsSummon() ) ) break;

			StructPlayer* pPlayer = NULL;
			if( m_pOwner->IsPlayer() )
				pPlayer = static_cast< StructPlayer * >( m_pOwner );
			else if( m_pOwner->IsSummon() )
				pPlayer = static_cast< StructSummon * >( m_pOwner )->GetMaster();

			if( !pPlayer )
				break;

			StructPlayer* pTargetPlayer = NULL;

			if( pTarget->IsPlayer() )
				pTargetPlayer = static_cast< StructPlayer * >( pTarget );
			else if( pTarget->IsSummon() )
				pTargetPlayer = static_cast< StructSummon * >( pTarget )->GetMaster();

			if( !pTargetPlayer )
				break;

			// 파티가 있는데 대상이 내 파티가 아니면 즐
			if( pPlayer != pTargetPlayer && pPlayer->GetPartyID() && pPlayer->GetPartyID() != pTargetPlayer->GetPartyID() ) break;

			struct myPartyFunctor : public PartyManager::PartyFunctor
			{
				myPartyFunctor( StructSkill *_pSkill, AR_TIME _t, StructCreature *_pOwner, SKILL_TARGET_FUNCTOR &_functor ) : pSkill( _pSkill ), t( _t ), pOwner( _pOwner ), partyFunctor( _functor ) {};

				virtual bool operator()( AR_HANDLE handle )
				{
					StructCreature::iterator it = StructCreature::get( handle );
					StructCreature * pPartyTarget = (*it);
					if( !pSkill || !pPartyTarget || !pPartyTarget->IsInWorld() )
						return false;

					bool bResult = false;

					// { 파티원 처리
					// 적용 거리 체크
					ArPosition pos = pOwner->GetPos();
					if( pSkill->GetFireRange() >= pos.GetDistance( pPartyTarget->GetPos() ) )
					{
						bResult = partyFunctor.onCreature( pSkill, t, pOwner, pPartyTarget );
					}
					// } 파티원 처리

					// { 파티원 소환수 처리
					pPartyTarget = static_cast< StructPlayer * >( pPartyTarget )->GetMainSummon();
					if( pPartyTarget && pPartyTarget->IsInWorld() &&
						pSkill->GetFireRange() >= pos.GetDistance( pPartyTarget->GetPos() ) )	// 적용 거리 체크
					{
						bResult |= partyFunctor.onCreature( pSkill, t, pOwner, pPartyTarget );
					}
					// } 파티원 소환수 처리

					return bResult;
				}

			private:
				struct SKILL_TARGET_FUNCTOR &partyFunctor;
				StructSkill *pSkill;
				StructCreature *pOwner;
				AR_TIME t;
			} fo( this, t, m_pOwner, functor );

			// 파티가 없으면 자기 자신만 타겟
			if( pPlayer->GetPartyID() == 0 )
			{
				fo( pPlayer->GetHandle() );
			}
			// 있으면 파티가 타겟
			else
			{
				m_nTargetCount = static_cast<int>( PartyManager::GetInstance().DoEachMember( pPlayer->GetPartyID(), fo ) );
			}

			break;
		}
	case SkillBase::TARGET_MASTER:
		{
			if( !m_pOwner->IsSummon() ) break;

			StructPlayer *pTargetPlayer = static_cast< StructSummon * >( m_pOwner )->GetMaster();
			if( !pTargetPlayer || !pTargetPlayer->IsInWorld() ) break;

			functor.onCreature( this, t, m_pOwner, pTargetPlayer );
			break;
		}
	case SkillBase::TARGET_SELF_WITH_MASTER:
		{
			if( !m_pOwner->IsSummon() || !m_pOwner->IsInWorld() ) break;

			functor.onCreature( this, t, m_pOwner, m_pOwner );

			StructPlayer *pTargetPlayer = static_cast< StructSummon * >( m_pOwner )->GetMaster();
			if( !pTargetPlayer || !pTargetPlayer->IsInWorld() ) break;

			if( pTargetPlayer->GetPos().GetDistance( m_pOwner->GetPos() ) > GameRule::VISIBLE_RANGE )
				break;

			functor.onCreature( this, t, m_pOwner, pTargetPlayer );
			break;
		}
	}

	return;
}

int StructSkill::CalculateMPCost( const SkillCostModifier& modifier )
{
	int absCost = GetCostMP( GetRequestedSkillLevel(), GetEnhance() );
	float percentCost = GetCostMPPercent( GetRequestedSkillLevel() ) * m_pOwner->GetMP() / 100;
	float costRatio = m_pOwner->GetManaCostRatio( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	float costBeforeModify = ( absCost + percentCost ) * costRatio;

	SkillCostModifier appliedModifier( m_CostModifier );
	appliedModifier.add( modifier );

	return appliedModifier.calculateMPCost( costBeforeModify );
}

int StructSkill::CalculateHPCost( const SkillCostModifier& modifier )
{
	int absCost = GetCostHP( GetRequestedSkillLevel() );
	float percentCost = GetCostHPPercent( GetRequestedSkillLevel() ) * m_pOwner->GetMaxHP() / 100;
	float costBeforeModify = absCost + percentCost;

	SkillCostModifier appliedModifier( m_CostModifier );
	appliedModifier.add( modifier );

	return appliedModifier.calculateHPCost( costBeforeModify );
}

struct lessByDistantFromTarget : public std::binary_function< StructCreature *, StructCreature *, bool >
{
	lessByDistantFromTarget( const ArPosition * _pTarget ) : pTarget( _pTarget ) {}

	result_type operator() ( first_argument_type a, second_argument_type b )
	{
		return pTarget->GetDistance( a->GetPos() ) < pTarget->GetDistance( b->GetPos() );
	}

	const ArPosition * pTarget;
};

struct RegionTester
{
	virtual void Init( const ArPosition & OriginalPos, const ArPosition & TargetPos, const float RegionProperty ) = 0;
	virtual bool IsInRegion( const ArPosition & pos ) = 0;
};
struct CircleRegionTester : public RegionTester
{
	void Init( const ArPosition & OriginalPos, const ArPosition & TargetPos, const float RegionProperty )
	{
	}

	bool IsInRegion( const ArPosition & pos )
	{
		return true;
	}
};

struct ArcCircleRegionTester : public RegionTester
{
	void Init( const ArPosition & OriginalPos, const ArPosition & TargetPos, const float RegionProperty )
	{
		float _V1x = TargetPos.x - OriginalPos.x;
		float _V1y = TargetPos.y - OriginalPos.y;

		// normalize 해서 저장한다.
		float m = sqrt( _V1x * _V1x + _V1y * _V1y );

		if( m == 0 )
		{
			V1x = 1;
			V1y = 0;
		}
		else
		{
			V1x = _V1x / m;
			V1y = _V1y / m;
		}

		x = OriginalPos.x;
		y = OriginalPos.y;

		fCos = cos( RegionProperty );
	}

	bool IsInRegion( const ArPosition & pos )
	{
		float _V2x = pos.x - x;
		float _V2y = pos.y - y;

		float V2x, V2y;

		float m = sqrt( _V2x * _V2x + _V2y * _V2y );

		if( m == 0 )
			return true;

		V2x = _V2x / m;
		V2y = _V2y / m;

		return fCos <= (V1x * V2x + V1y * V2y);
	}

	float V1x;	// 기준->타겟의 노말 벡터
	float V1y;	// 기준->타겟의 노말 벡터

	float x;	// 기준점
	float y;	// 기준점

	float fCos;	// 기준 Cosine 값
};

struct DirectionRegionTester : public RegionTester
{
	void Init( const ArPosition & OriginalPos, const ArPosition & TargetPos, const float RegionProperty )
	{
		dx = TargetPos.x - OriginalPos.x;
		dy = TargetPos.y - OriginalPos.y;
		c = dy * OriginalPos.x - dx * OriginalPos.y;

		denominator = sqrt( dx * dx + dy * dy );
		thickness = RegionProperty * GameRule::DEFAULT_UNIT_SIZE / 2;

		float _V1x = TargetPos.x - OriginalPos.x;
		float _V1y = TargetPos.y - OriginalPos.y;

		// normalize 해서 저장한다.
		float m = sqrt( _V1x * _V1x + _V1y * _V1y );
		V1x = _V1x / m;
		V1y = _V1y / m;

		ori_x = OriginalPos.x;
		ori_y = OriginalPos.y;

		/*
		x = TargetPos.y - OriginalPos.y;
		y = OriginalPos.x - TargetPos.x;
		c = 0 - x * OriginalPos.x - y * OriginalPos.y;

		denominator = sqrt( x * x + y * y );
		thickness = RegionProperty * GameRule::DEFAULT_UNIT_SIZE / 2;

		float _V1x = TargetPos.x - OriginalPos.x;
		float _V1y = TargetPos.y - OriginalPos.y;

		// normalize 해서 저장한다.
		float m = sqrt( _V1x * _V1x + _V1y * _V1y );
		V1x = _V1x / m;
		V1y = _V1y / m;

		ori_x = OriginalPos.x;
		ori_y = OriginalPos.y;
		*/
	}

	// 점과 직선사이의 거리 공식
	// d = │ap+bq+c│/√( a²+b²)

	bool IsInRegion( const ArPosition & pos )
	{
		float dist = abs( -dy * pos.x + dx * pos.y + c ) / denominator ;
		if( thickness > dist  )
		{
			float _V2x = pos.x - ori_x;
			float _V2y = pos.y - ori_y;

			float V2x, V2y;

			float m = sqrt( _V2x * _V2x + _V2y * _V2y );
			V2x = _V2x / m;
			V2y = _V2y / m;

			return 0 <= (V1x * V2x + V1y * V2y);
		}
		/*
		if( thickness > ( abs( x * pos.x + y * pos.y + c ) / denominator ) )
		{
		float _V2x = pos.x - ori_x;
		float _V2y = pos.y - ori_y;

		float V2x, V2y;

		float m = sqrt( _V2x * _V2x + _V2y * _V2y );
		V2x = _V2x / m;
		V2y = _V2y / m;

		return 0 <= (V1x * V2x + V1y * V2y);
		}
		*/

		return false;
	}

	float V1x;	// 기준->타겟의 노말 벡터
	float V1y;	// 기준->타겟의 노말 벡터

	float ori_x;	// 기준점
	float ori_y;	// 기준점

	/*
	float x;	// x 변이
	float y;	// y 변이
	*/
	float dx;
	float dy;
	float c;	// 상수 -_-
	float thickness; // 직선 두께
	float denominator; // sqrt( x * x + y * y ) 미리 계산해놓은 것.
};

struct CrossRegionTester : public RegionTester
{
	void Init( const ArPosition & OriginalPos, const ArPosition & TargetPos, const float RegionProperty )
	{
		x1 = TargetPos.y - OriginalPos.y;
		y1 = OriginalPos.x - TargetPos.x;
		c1 = 0 - x1 * OriginalPos.x - y1 * OriginalPos.y;

		x2 = y1;
		y2 = -x1;
		c2 = 0 - x2 * OriginalPos.x - y2 * OriginalPos.y;


		denominator = sqrt( x1 * x1 + y1 * y1 ); // 두 직선에 대해 같이 사용할 수 있음
		thickness = RegionProperty * GameRule::DEFAULT_UNIT_SIZE / 2;
	}

	// 점과 직선사이의 거리 공식
	// d = │ap+bq+c│/√( a²+b²)

	bool IsInRegion( const ArPosition & pos )
	{
		return ( thickness > ( abs( x1 * pos.x + y1 * pos.y + c1 ) / denominator ) ) || ( thickness > ( abs( x2 * pos.x + y2 * pos.y + c2 ) / denominator ) );
	}

	// OriginalPos와 TargetPos를 가로지르는 직선
	float x1;	// x 변이
	float y1;	// y 변이
	float c1;	// 상수 -_-

	// OriginalPos를 지나고 위 직선에 직교하는 직선
	float x2;
	float y2;
	float c2;

	float thickness; // 직선 두께
	float denominator; // sqrt( x * x + y * y ) 미리 계산해놓은 것.
};

// nRegionType 이 -1 이면 기본 원형
int EnumSkillTargetsAndCalcDamage( const ArPosition & _OriginalPos, unsigned char layer, const ArPosition & _TargetPos, bool bTargetOrigin, const float fEffectLength, const int nRegionType, const float fRegionProperty, const int nOriginalDamage, const bool bIncludeOriginalPos, StructCreature * pCaster, const int nDistributeType, const int nTargetMax, /*out*/ std::vector< StructCreature * > & vTargetList, bool bEnemyOnly )
{
	int nResult = nOriginalDamage;

	// 타겟 기준인지, 아닌지
	const ArPosition & OriginalPos = bTargetOrigin ? _TargetPos : _OriginalPos;
	const ArPosition & TargetPos = bTargetOrigin ? _OriginalPos : _TargetPos;

	std::vector< AR_HANDLE > vList;

	{
		ArcadiaServer::Instance().EnumMovableObject( OriginalPos, layer, fEffectLength, &vList );
	}

	vTargetList.clear();
	vTargetList.reserve( vList.size() );

	RegionTester * pTester = NULL;

	ArcCircleRegionTester		RegionTester_ArcCircle;
	DirectionRegionTester		RegionTester_Direction;
	CrossRegionTester			RegionTester_Cross;
	CircleRegionTester			RegionTester_Circle;

	if( nRegionType == SkillBase::REGION_TYPE_ARC_CIRCLE )
	{
		pTester = &RegionTester_ArcCircle;
	}
	else if( nRegionType == SkillBase::REGION_TYPE_DIRECTION )
	{
		pTester = &RegionTester_Direction;
	}
	else if( nRegionType == SkillBase::REGION_TYPE_CROSS )
	{
		pTester = &RegionTester_Cross;
	}
	else
	{
		pTester = &RegionTester_Circle;
	}

	pTester->Init( OriginalPos, TargetPos, fRegionProperty );

	int nTargetCount = 0;
	int nAllyCount = 0;

	// { 공격할 애들 enumeration
	for( std::vector< AR_HANDLE >::iterator itRawTargetList = vList.begin(); itRawTargetList != vList.end(); ++itRawTargetList )
	{
		bool bIsAlly = false;
		GameObject *pObj = GameObject::raw_get( *itRawTargetList );

		if( !pObj ) continue;
		if( !pObj->IsCreature() || pObj->IsPet() ) continue;

		StructCreature * pTarget = static_cast< StructCreature * > ( pObj );

		if( pTarget->IsDead() ) continue;

		if( !pCaster->IsEnemy( pTarget, true ) )
		{
			if( bEnemyOnly )
				continue;

			bIsAlly = true;
		}


		ArPosition current_pos = pObj->GetCurrentPosition( GetArTime() );

		if( !pTester->IsInRegion( current_pos ) )
		{
			continue;
		}

		if( !bIncludeOriginalPos && OriginalPos == current_pos )
		{
			continue;
		}

		if( bIsAlly )
		{
			vTargetList.insert( vTargetList.begin(), pTarget );
			++nAllyCount;
		}
		else
		{
			vTargetList.push_back( pTarget );
			++nTargetCount;
		}
	}
	// }

	// { 분산 타입 처리
	if( nDistributeType == SkillBase::DISTRIBUTION_TYPE_SEQUENTIAL_TARGET )
	{
		std::sort( vTargetList.begin() + nAllyCount, vTargetList.end(), lessByDistantFromTarget( &_TargetPos ) );
	}
	else if( nDistributeType == SkillBase::DISTRIBUTION_TYPE_SEQUENTIAL_CASTER )
	{
		std::sort( vTargetList.begin() + nAllyCount, vTargetList.end(), lessByDistantFromTarget( &_OriginalPos ) );
	}

	if( nDistributeType == SkillBase::DISTRIBUTION_TYPE_RANDOM ||
		nDistributeType == SkillBase::DISTRIBUTION_TYPE_SEQUENTIAL_TARGET ||
		nDistributeType == SkillBase::DISTRIBUTION_TYPE_SEQUENTIAL_CASTER )
	{
		if( nTargetCount > nTargetMax )
			vTargetList.resize( nAllyCount + nTargetMax );
	}

	if( nDistributeType == SkillBase::DISTRIBUTION_TYPE_DISTRIBUTE )
	{
		if( nTargetCount > nTargetMax )
			nResult = nResult * nTargetMax / nTargetCount;
	}
	// }

	return nResult;
}

int EnumChainSkillTargetsAndCalcDamage( const float fEffectLength, const float fChainLength, const int nChainType, const bool bIncludeTarget, StructCreature * pCaster, StructCreature * & pCurrentTarget, const int nChainMax, /*out*/ std::vector< StructCreature * > & vTargetList, const int nTargetType )
{
	AR_TIME t = GetArTime();

	std::vector< AR_HANDLE > vList;
	{
		// 연쇄 이동 방식은 대상을 타고 여러 번 이동해가므로 범위를 예측하기 어렵다.
		// 따라서 대상의 전체 목록은 시전자를 기준으로 제한한다.
		ArcadiaServer::Instance().EnumMovableObject( pCaster->GetCurrentPosition( t ), pCaster->GetLayer(), fEffectLength, &vList );
	}

	vTargetList.clear();
	vTargetList.reserve( vList.size() );

	int nCount = 0;
	std::vector< AR_HANDLE >::iterator it = std::find( vList.begin(), vList.end(), pCurrentTarget->GetHandle() );
	if( it == vList.end() ) return false;

	vector_fast_erase( &vList, it );
	if( bIncludeTarget )
	{
		vTargetList.push_back( pCurrentTarget );
		++nCount;
	}

	while( nCount < nChainMax )
	{
		std::vector< AR_HANDLE >::iterator itExpectedTarget;
		StructCreature * pExpectedTarget = NULL;
		AR_UNIT fMinLength = FLT_MAX;
		int nMaxHPLoss = -1;
		int nMaxHPLossPercentage = -1;

		// { 공격할 애들 enumeration
		for( it = vList.begin(); vList.empty() == false && it != vList.end(); ++it )
		{
			GameObject *pObj = GameObject::raw_get( *it );

			if( !pObj ) continue;
			if( !pObj->IsCreature() || pObj->IsPet() ) continue;

			StructCreature * pTarget = static_cast< StructCreature * > ( pObj );

			if( pTarget->IsDead() ) continue;

			// 효과 대상 1 : 동료 & 중립
			if( nTargetType == 1 && pCaster->IsEnemy( pTarget, true ) )
				continue;

			// 효과 대상 2 : 동료
			else if( nTargetType == 2 && !pCaster->IsAlly( pTarget ) )
				continue;

			// 효과 대상 3 : 적
			else if( nTargetType == 3 && !pCaster->IsEnemy( pTarget, true ) )
				continue;

			float fLength = pTarget->GetCurrentPosition( t ).GetDistance( pCurrentTarget->GetCurrentPosition( t ) );
			if( fLength >= fChainLength )
			{
				continue;
			}

			// { 체인 대상 선정 방식
			if( nChainType == SkillBase::CHAIN_TYPE_DISTANCE )
			{
				if( fLength < fMinLength )
				{
					itExpectedTarget = it;
					pExpectedTarget = pTarget;
					fMinLength = fLength;
				}
			}
			else if( nChainType == SkillBase::CHAIN_TYPE_RANDOM )
			{
				itExpectedTarget = it;
				pExpectedTarget = pTarget;
				break;
			}
			else if( nChainType == SkillBase::CHAIN_TYPE_HP_LOSS )
			{
				if( nMaxHPLoss < pTarget->GetMaxHP() - pTarget->GetHP() )
				{
					itExpectedTarget = it;
					pExpectedTarget = pTarget;
					nMaxHPLoss = pTarget->GetMaxHP() - pTarget->GetHP();
				}
			}
			else if( nChainType == SkillBase::CHAIN_TYPE_HP_LOSS_PERCENTAGE )
			{
				if( nMaxHPLossPercentage < 100 - pTarget->GetHPPercentage() )
				{
					itExpectedTarget = it;
					pExpectedTarget = pTarget;
					nMaxHPLossPercentage = 100 - pTarget->GetHPPercentage();
				}
			}
		}
		// }

		// 더 이상 대상이 없다
		if( pExpectedTarget == NULL ) return nCount;

		StructCreature *pCurrentTarget = pExpectedTarget;
		vector_fast_erase( &vList, itExpectedTarget );
		vTargetList.push_back( pCurrentTarget );
		++nCount;
	}

	return nCount;
}

bool StructSkill::ProcAura()
{
	AR_TIME t = GetArTime();

	StructSummon *pSummon = NULL;

	if( GetAuraMPDecTime() < t + GameRule::DEFAULT_MP_DEC_TIME )
	{
		float fDecMP = GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();

		if( m_pOwner->GetMP() < fDecMP )
		{
			return false;
		}

		m_pOwner->SetMP( m_pOwner->GetMP() - fDecMP );
		SetAuraMPDecTime( t );

		if( m_pOwner->IsInWorld() )
			BroadcastHPMPMsg( m_pOwner, 0, m_pOwner->GetMP() - fDecMP );

		if( m_pOwner->IsSummon() )
		{
			pSummon = static_cast< StructSummon * >( m_pOwner );
			if( pSummon->GetMaster() && pSummon->IsInWorld() )
			{
				SendHPMPMsg( pSummon->GetMaster(), m_pOwner, 0, m_pOwner->GetMP() - fDecMP );
			}
		}
	}

	if( !m_pOwner->IsInWorld() )
		return true;

	std::vector< AR_HANDLE > vList;
	ArcadiaServer::Instance().EnumMovableObject( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), GetFireRange(), &vList, true, true );

	// 대상 없으면 KIN
	if( vList.empty() ) return true;

	AR_TIME et = t + SkillBase::TOGGLE_LIVE_TIME;

	std::vector< AR_HANDLE >::iterator it;
	StructCreature *pTarget;
	int target_type = GetSkillBase()->GetSkillTargetType();

	bool bApplySummon = false;

	for( it = vList.begin(); it != vList.end(); ++it )
	{
		GameObject * pObj = GameObject::raw_get( *it );
		if( !pObj || pObj->IsPet() ) continue;
		pTarget = static_cast< StructCreature* >( pObj );

		// 아니면 KIN
		if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_TOGGLE_DIFFERENTIAL_AURA )
		{
			// 시전자가 소환수이면 무조건 자신에게 건다
			if( pSummon ) 
			{
				pTarget = pSummon;
			}
			else if( pTarget->IsSummon() )
			{
				if( static_cast< StructPlayer* >( m_pOwner ) != static_cast< StructSummon* >( pTarget )->GetMaster() ) continue;
			}
			else continue;

			if( GetVar(5) )
			{
				pTarget->PendAddState( (StructState::StateCode) (int) GetVar(5), 
					m_pOwner->GetHandle(), 
					GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
					t, et, true );
			}
			if( GetVar(6) )
			{
				pTarget->PendAddState( (StructState::StateCode) (int) GetVar(6), 
					m_pOwner->GetHandle(), 
					GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
					t, et, true );
			}
			if( GetVar(7) )
			{
				pTarget->PendAddState( (StructState::StateCode) (int) GetVar(7), 
					m_pOwner->GetHandle(), 
					GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
					t, et, true );
			}

			bApplySummon = true;

			// 분할 토글형 스킬은 반드시 타겟이 한 마리이기 때문에 지속효과가 걸린다면 break 처리한다.
			break;
		}
		else if( target_type == SkillBase::TARGET_TARGET ) 
		{
			if( pTarget != m_pOwner )
				continue;
		}
		else if( pTarget->IsSummon() )
		{
			if(    target_type != SkillBase::TARGET_PARTY_SUMMON
				&& target_type != SkillBase::TARGET_PARTY_WITH_SUMMON
				&& target_type != SkillBase::TARGET_SUMMON
				&& target_type != SkillBase::TARGET_CREATURE_TYPE_NONE
				&& target_type != SkillBase::TARGET_CREATURE_TYPE_FIRE	
				&& target_type != SkillBase::TARGET_CREATURE_TYPE_WATER	
				&& target_type != SkillBase::TARGET_CREATURE_TYPE_WIND	
				&& target_type != SkillBase::TARGET_CREATURE_TYPE_EARTH	
				&& target_type != SkillBase::TARGET_CREATURE_TYPE_LIGHT	
				&& target_type != SkillBase::TARGET_CREATURE_TYPE_DARK
				&& target_type != SkillBase::TARGET_SELF_WITH_SUMMON
				&& target_type != SkillBase::TARGET_SELF_WITH_MASTER ) continue;

			if( target_type == SkillBase::TARGET_PARTY_SUMMON || target_type == SkillBase::TARGET_PARTY_WITH_SUMMON )
			{
				if( static_cast< StructPlayer* >( m_pOwner )->GetPartyID() )
				{
					if( static_cast< StructPlayer* >( m_pOwner )->GetPartyID() != static_cast< StructSummon* >( pTarget )->GetMaster()->GetPartyID() ) continue;
				}
				else
				{
					if( static_cast< StructPlayer* >( m_pOwner ) != static_cast< StructSummon* >( pTarget )->GetMaster() ) continue;
				}
			}
			else if( target_type == SkillBase::TARGET_SELF_WITH_MASTER )
			{
				// 현재 타겟이 소환수 타입이니 자신 아니면 안 걸림
				if( m_pOwner != pTarget ) continue;
			}
			else
			{
				if( static_cast< StructPlayer* >( m_pOwner ) != static_cast< StructSummon* >( pTarget )->GetMaster() ) continue;

				if( target_type == SkillBase::TARGET_CREATURE_TYPE_NONE  && static_cast< StructSummon* >( pTarget )->GetSummonElementalType() != Elemental::TYPE_NONE ) continue;
				if( target_type == SkillBase::TARGET_CREATURE_TYPE_FIRE	 && static_cast< StructSummon* >( pTarget )->GetSummonElementalType() != Elemental::TYPE_FIRE ) continue;
				if( target_type == SkillBase::TARGET_CREATURE_TYPE_WATER && static_cast< StructSummon* >( pTarget )->GetSummonElementalType() != Elemental::TYPE_WATER ) continue;
				if( target_type == SkillBase::TARGET_CREATURE_TYPE_WIND	 && static_cast< StructSummon* >( pTarget )->GetSummonElementalType() != Elemental::TYPE_WIND ) continue;
				if( target_type == SkillBase::TARGET_CREATURE_TYPE_EARTH && static_cast< StructSummon* >( pTarget )->GetSummonElementalType() != Elemental::TYPE_EARTH ) continue;
				if( target_type == SkillBase::TARGET_CREATURE_TYPE_LIGHT && static_cast< StructSummon* >( pTarget )->GetSummonElementalType() != Elemental::TYPE_LIGHT ) continue;
				if( target_type == SkillBase::TARGET_CREATURE_TYPE_DARK  && static_cast< StructSummon* >( pTarget )->GetSummonElementalType() != Elemental::TYPE_DARK ) continue;
			}
		}
		else if( pTarget->IsPlayer() )
		{
			if( target_type != SkillBase::TARGET_PARTY &&
				target_type != SkillBase::TARGET_PARTY_WITH_SUMMON &&
				target_type != SkillBase::TARGET_SELF_WITH_SUMMON &&
				target_type != SkillBase::TARGET_MASTER &&
				target_type != SkillBase::TARGET_SELF_WITH_MASTER ) continue;

			if( target_type == SkillBase::TARGET_PARTY || target_type == SkillBase::TARGET_PARTY_WITH_SUMMON )
			{
				StructPlayer * pOwnPlayer = NULL;

				if( m_pOwner->IsPlayer() ) pOwnPlayer = static_cast< StructPlayer* >( m_pOwner );
				if( m_pOwner->IsSummon() ) pOwnPlayer = static_cast< StructSummon* >( m_pOwner )->GetMaster();

				if( !pOwnPlayer ) continue;

				if( !pOwnPlayer->IsInParty() && pTarget != m_pOwner ) continue;
				if( pOwnPlayer ->GetPartyID() != static_cast< StructPlayer* >( pTarget )->GetPartyID() ) continue;
			}
			else if( target_type == SkillBase::TARGET_SELF_WITH_SUMMON )
			{
				if( pTarget != m_pOwner ) continue;
			}
			else if( target_type == SkillBase::TARGET_SELF_WITH_MASTER )
			{
				if( pTarget != static_cast< StructSummon * >( m_pOwner )->GetMaster() ) continue;
			}
		}
		else
		{
			continue;
		}

		pTarget->PendAddState( (StructState::StateCode)GetStateId(), 
			m_pOwner->GetHandle(), 
			GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
			t, et, true );

		if( GetVar(3) )
		{
			pTarget->PendAddState( (StructState::StateCode) (int) GetVar(3), 
				m_pOwner->GetHandle(), 
				GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
				t, et, true );
		}

		if( GetVar(4) )
		{
			pTarget->PendAddState( (StructState::StateCode) (int) GetVar(4), 
				m_pOwner->GetHandle(), 
				GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
				t, et, true );
		}
	}

	if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_TOGGLE_DIFFERENTIAL_AURA )
	{
		if( !bApplySummon )
			return false;		// 적용된 크리처 없으면 토글 해제


		if( pSummon ) 
			pTarget = pSummon->GetMaster();
		else 
			pTarget = m_pOwner;

		pTarget->PendAddState( (StructState::StateCode)GetStateId(), 
			m_pOwner->GetHandle(), 
			GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
			t, et, true );

		if( GetVar(3) )
		{
			pTarget->PendAddState( (StructState::StateCode) (int) GetVar(3), 
				m_pOwner->GetHandle(), 
				GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
				t, et, true );
		}

		if( GetVar(4) )
		{
			pTarget->PendAddState( (StructState::StateCode) (int) GetVar(4), 
				m_pOwner->GetHandle(), 
				GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
				t, et, true );
		}
	}


	return true;
}

void StructSkill::SINGLE_MAGICAL_DAMAGE_T1( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDamage = 0;

	nDamage = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();

	int elemental_type = GetSkillBase()->GetElementalType();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}

void StructSkill::SINGLE_MAGICAL_DAMAGE_T2( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDamage = 0;

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	nDamage = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance() );
	int nMax = nMagicPoint + GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	nDamage = std::min( nDamage, nMax );

	int elemental_type = GetSkillBase()->GetElementalType();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}

void StructSkill::SINGLE_MAGICAL_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDamage = m_pOwner->GetMagicPoint( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();
	nDamage += GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	if( GetSkillBase()->GetEffectType() == SkillBase::EF_MAGIC_SINGLE_DAMAGE_WITH_PHYSICAL_DAMAGE )
	{
		int nPhysicalDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
		nPhysicalDamage *= GetVar(6) + GetVar(7) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
		nPhysicalDamage += GetVar(8) + GetVar(9) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();
		nDamage += nPhysicalDamage;
	}

	int elemental_type = GetSkillBase()->GetElementalType();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}

void StructSkill::MULTIPLE_MAGICAL_DAMAGE_AT_ONCE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	int nDamage = ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) * nMagicPoint + GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	int nCount = GetVar(6) + GetVar(7)*GetRequestedSkillLevel();	

	int elemental_type = GetSkillBase()->GetElementalType();

	// 데미지
	for( int i = 0; i < nCount; ++i )
	{
		StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}

	m_nFireCount = nCount;
	// 연타 횟수 만큼 행동 불가
	m_nFireTime += GetVar(8) * nCount * 100;

	return;
}

void StructSkill::MULTIPLE_MAGICAL_DAMAGE_T2( struct StructCreature* pTarget )
{

	if( !pTarget ) return;

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	int nDamage = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance() );

	if( m_nCurrentFire == 0 )
	{
		// { 횟수 계산
		m_nTotalFire = GetVar(2) + GetVar(3)*GetRequestedSkillLevel();
		// }
		m_nCurrentFire = 1;
	}
	else
	{
		++m_nCurrentFire;
	}

	int elemental_type = GetSkillBase()->GetElementalType();

	// 데미지
	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	m_nFireTime += GetVar(4) * 100;

	return;
}

void StructSkill::MULTIPLE_MAGICAL_DAMAGE_T1( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDamage = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	if( m_nCurrentFire == 0 )
	{
		// { 횟수 계산
		m_nTotalFire = GetVar(2) + GetVar(3)*GetRequestedSkillLevel();
		// }
		m_nCurrentFire = 1;
	}
	else
	{
		++m_nCurrentFire;
	}

	int elemental_type = GetSkillBase()->GetElementalType();

	// 데미지
	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	m_nFireTime += GetVar(4) * 100;

	return;
}

void StructSkill::SINGLE_MAGICAL_DAMAGE_OR_DEATH( struct StructCreature* pTarget )
{
	if( !pTarget || !pTarget->IsMonster() ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	// 대상이 해당 종족에 맞는지 다시 체크(Cast에서 이미 한 번 체크하게 되어있음)
	// 99:CREATURE_ALL(모든 크리처 종족 대상)이면 체크 안함
	if( GetVar( 8 ) != CREATURE_ALL && GetVar( 8 ) != static_cast< StructMonster * >(pTarget)->GetCreatureGroup() )
		return;

	// 보스 몬스터에겐 100% 실패
	if( static_cast< StructMonster * >(pTarget)->IsBossMonster() ) return;

	if( GetVar( 9 ) != 0 && GetVar( 9 ) + GetVar( 16 ) * GetRequestedSkillLevel() > (float)pTarget->GetHP()/pTarget->GetMaxHP()*100 ) return;
	if( GetVar( 10 ) != 0 && GetVar( 10 ) + GetVar( 17 ) * GetRequestedSkillLevel() < (float)pTarget->GetHP()/pTarget->GetMaxHP()*100 ) return;
	for( int i = 11; i <= 14; i++ )
	{
		if( GetVar( i ) == 0 ) continue;
		StructState::StateCode nCode = StructState::StateCode( static_cast< int >( GetVar( i ) ) );
		if( !pTarget->GetState( nCode ) ) return;
	}

	float fDeathProbability = ( GetVar( 6 ) + GetVar( 7 ) * GetRequestedSkillLevel() ) * 100;

	// 죽을래 살래
	if( XRandom( 1, 10000 ) < (int)fDeathProbability )
	{
		// 죽었네
		int nDamage = pTarget->GetHP() * 2;
		StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ), StructCreature::IGNORE_DEFENCE | StructCreature::IGNORE_AVOID | StructCreature::IGNORE_BLOCK );

		// TARGET_DEAD 로 변경 적용할 것. 이 경우 아이템을 드랍 등등이 보이지 않는 문제 발생함.
		//AddSkillResult( &m_vResultList, true, SkillResult::TARGET_DEAD, pTarget->GetHandle() );
		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}
	else
	{
		// 살았으면 뎀쥐만~
		int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
		int nDamage = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) + GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

		StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		if( GetVar( 15 ) )
		{
			for( int i = 11; i <= 14; i++ )
			{
				if( GetVar( i ) == 0 ) continue;
				StructState::StateCode nCode = StructState::StateCode( static_cast< int >( GetVar( i ) ) );
				if( pTarget->GetState( nCode ) ) 
					pTarget->RemoveState( nCode, GameRule::MAX_STATE_LEVEL );
			}
		}

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}

	return;
}

void StructSkill::MULTIPLE_MAGICAL_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	int nDamage = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) + GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	if( m_nCurrentFire == 0 )
	{
		// { 횟수 계산
		m_nTotalFire = GetVar(6) + GetVar(7)*GetRequestedSkillLevel();
		// }
		m_nCurrentFire = 1;
	}
	else
	{
		++m_nCurrentFire;
	}

	int elemental_type = GetSkillBase()->GetElementalType();

	// 데미지
	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	m_nFireTime += GetVar(8) * 100;

	return;
}

void StructSkill::MULTIPLE_MAGICAL_DAMAGE_T3( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDamage = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	// { 횟수 계산
	int nCount = GetVar(2) + GetVar(3)*GetRequestedSkillLevel();
	// }

	int elemental_type = GetSkillBase()->GetElementalType();

	// 데미지
	for( int i = 0; i < nCount; ++i )
	{

		StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}
	m_nFireCount = nCount;
	m_nFireTime += GetVar(4) * nCount * 100;

	return;
}

void StructSkill::SINGLE_PHYSICAL_DAMAGE_ABSORB( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDamage = 0;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );			// 기본적으로, 모든 스킬은 오늘손 장비의 영향을 받는다. 이도류 스킬은 제외.

	// 데미지
	int elemental_type = GetSkillBase()->GetElementalType();
	nDamage = nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	float fHPMod = GetVar(4) * GetEnhance();
	float fMPMod = GetVar(5) * GetEnhance();

	// 흡수
	int nAddHP = Damage.nDamage * GetVar(2) + Damage.nDamage * fHPMod;
	int nAddMP = Damage.nDamage * GetVar(3) + Damage.nDamage * fMPMod;
	m_pOwner->AddHP( nAddHP );
	m_pOwner->AddMP( nAddMP );

	SkillResult skill_result;

	skill_result.add_hp.type = SkillResult::ADD_HP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nAddHP;
	skill_result.add_hp.target_hp = m_pOwner->GetHP();

	m_vResultList.push_back( skill_result );

	skill_result.add_hp.type = SkillResult::ADD_MP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nAddMP;
	skill_result.add_hp.target_hp = m_pOwner->GetMP();

	m_vResultList.push_back( skill_result );

	return;
}

void StructSkill::SINGLE_MAGICAL_DAMAGE_WITH_ABSORB( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	// 데미지 처리
	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	int nDamage = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) + GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	// 흡수
	int nAddHP = Damage.nDamage * ( GetVar(6) + GetVar(7) * GetRequestedSkillLevel() + GetVar(8) * GetEnhance() );
	int nAddMP = Damage.nDamage * ( GetVar(9) + GetVar(10) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance() );
	m_pOwner->AddHP( nAddHP );
	m_pOwner->AddMP( nAddMP );

	SkillResult skill_result;

	skill_result.add_hp.type = SkillResult::ADD_HP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nAddHP;
	skill_result.add_hp.target_hp = m_pOwner->GetHP();

	m_vResultList.push_back( skill_result );

	skill_result.add_hp.type = SkillResult::ADD_MP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nAddMP;
	skill_result.add_hp.target_hp = m_pOwner->GetMP();

	m_vResultList.push_back( skill_result );
}

void StructSkill::ADD_HP_MP_BY_ABSORB_HP_MP( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// 데미지 처리
	int nHPDamage = ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() );
	int nMPDamage = ( GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance() );

	pTarget->AddHP( 0 - nHPDamage );
	pTarget->AddMP( 0 - nMPDamage );

	SkillResult skill_result;

	skill_result.add_hp.type = SkillResult::ADD_HP;
	skill_result.add_hp.hTarget = pTarget->GetHandle();
	skill_result.add_hp.nIncHP = 0 - nHPDamage;
	skill_result.add_hp.target_hp = pTarget->GetHP();

	m_vResultList.push_back( skill_result );

	skill_result.add_hp.type = SkillResult::ADD_MP;
	skill_result.add_hp.hTarget = pTarget->GetHandle();
	skill_result.add_hp.nIncHP = 0 - nMPDamage;
	skill_result.add_hp.target_hp = pTarget->GetMP();

	m_vResultList.push_back( skill_result );

	// 흡수
	int nHPAbsorb = ( GetVar(9) + GetVar(10) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance() );
	int nMPAbsorb = ( GetVar(6) + GetVar(7) * GetRequestedSkillLevel() + GetVar(8) * GetEnhance() );

	m_pOwner->AddHP( nHPAbsorb );
	m_pOwner->AddMP( nMPAbsorb );

	skill_result.add_hp.type = SkillResult::ADD_HP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nHPAbsorb;
	skill_result.add_hp.target_hp = m_pOwner->GetHP();

	m_vResultList.push_back( skill_result );

	skill_result.add_hp.type = SkillResult::ADD_MP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nMPAbsorb;
	skill_result.add_hp.target_hp = m_pOwner->GetMP();

	m_vResultList.push_back( skill_result );
}

void StructSkill::SINGLE_MAGICAL_TARGET_HP_PERCENT_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// 보스급 몬스터 이상이면 실패
	if( !pTarget->IsMonster() || static_cast< StructMonster * >(pTarget)->IsBossMonster() ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	int nDamage = ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) * pTarget->GetHP();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ), StructCreature::IGNORE_DEFENCE | StructCreature::IGNORE_CRITICAL );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
}

void StructSkill::SINGLE_MAGICAL_MANABURN( StructCreature *pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	int nMPBurn = 0;

	switch( GetSkillBase()->GetEffectType() )
	{
	case SkillBase::EF_MAGIC_SINGLE_PERCENT_MANABURN:
		nMPBurn = GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();
		break;
	case SkillBase::EF_MAGIC_SINGLE_PERCENT_OF_MAX_MP_MANABURN:
		nMPBurn = pTarget->GetMaxMP() * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() );
		break;
	default:
		assert( 0 );
		return;
	}

	int nDamage = GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ), StructCreature::IGNORE_DEFENCE );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	pTarget->AddMP( -nMPBurn );

	SkillResult skill_result;

	skill_result.add_hp.type = SkillResult::ADD_MP;
	skill_result.add_hp.hTarget = pTarget->GetHandle();
	skill_result.add_hp.nIncHP = -nMPBurn;
	skill_result.add_hp.target_hp = pTarget->GetMP();

	m_vResultList.push_back( skill_result );
}

void StructSkill::SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE( struct StructCreature* pTarget )
{
	bool bFirable = false;
	for( int i = 0 ; i < 4 ; ++i )
	{
		StructState::StateCode nCode = StructState::StateCode( static_cast< int >( GetVar( i ) ) );
		if( pTarget->GetState( nCode ) )
		{
			if( GetVar( 4 ) )
				pTarget->RemoveState( nCode, GameRule::MAX_STATE_LEVEL );

			AddSkillResult( &m_vResultList, true, SkillResult::REMOVE_STATE, pTarget->GetHandle() );
			bFirable = true;
			break;
		}
	}

	if( !bFirable )
	{
		AddSkillResult( &m_vResultList, false, SkillResult::REMOVE_STATE, pTarget->GetHandle() );
		return;
	}

	Elemental::Type elemental_type = (Elemental::Type)GetSkillBase()->GetElementalType();

	int nDamage = 0;
	if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_BY_CONSUMING_TARGETS_STATE )
		nDamage = m_pOwner->GetAttackPointRight( elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	else
		nDamage = m_pOwner->GetMagicPoint( elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	nDamage = ( GetVar( 5 ) + GetVar( 6 ) * GetRequestedSkillLevel() + GetVar( 9 ) * GetEnhance() ) * nDamage + ( GetVar( 7 ) + GetVar( 8 ) * GetRequestedSkillLevel() + GetVar( 10 ) * GetEnhance() );

	StructCreature::_DAMAGE_INFO Damage;
	if( GetSkillBase()->IsPhysicalSkill() )
		Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );
	else
		Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, ( GetSkillBase()->IsPhysicalSkill() ) ? SkillResult::DAMAGE : SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
}

void StructSkill::MAGIC_SINGLE_REGION_DAMAGE_BY_SUMMON_DEAD( struct StructCreature* pTarget )
{
	AR_TIME t = GetArTime();

	if( !m_pOwner->IsPlayer() ) return;

	StructSummon * pSummon = NULL;

	pSummon = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon();

	if( !pSummon || pSummon->IsDead() || pSummon == pTarget )
	{
		AddSkillResult( &m_vResultList, false, SkillResult::SUMMON_DEAD );

		return;
	}

	ArPosition targetPos = pSummon->GetCurrentPosition( t );

	int nSummonMaxHP = pSummon->GetMaxHP();

	pSummon->damage( m_pOwner, pSummon->GetHP(), false );

	ArcadiaServer::Instance().SetObjectPriority( pSummon, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	StructCreature::_DAMAGE_INFO Damage;
	Damage.nDamage = pSummon->GetHP();
	Damage.target_hp = pSummon->GetHP();

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, Elemental::TYPE_NONE, Damage, pSummon->GetHandle() );

	int nDamage = nSummonMaxHP * ( GetVar( 0 ) + GetVar( 1 ) * GetRequestedSkillLevel() + GetVar( 2 ) * GetEnhance() );

	int elemental_type = GetSkillBase()->GetElementalType();

	float fEffectLength = ( GetVar(7) + GetVar(8) * GetRequestedSkillLevel() ) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), targetPos, true, fEffectLength, -1, 0, nDamage, true, m_pOwner, GetVar( 9 ), GetVar( 10 ) + GetVar( 11 ) * GetRequestedSkillLevel(), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
	// }
}

void StructSkill::MAGIC_SINGLE_REGION_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	if( GetSkillBase()->GetEffectType() == SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE_USING_CORPSE )
	{
		// 몬스터에게만 사용 가능
		if( !pTarget->IsMonster() || !pTarget->IsDead() ) return;

		RemoveMonsterFromWorld( static_cast< StructMonster * >( pTarget ) );
	}

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	int nDamage = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) + GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	int elemental_type = GetSkillBase()->GetElementalType();

	float effectLength = GetSkillBase()->GetEffectType() == SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE_ADD_RANDOM_STATE ? GetVar(12) : GetVar(9);
	float distributeType = GetSkillBase()->GetEffectType() == SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE_ADD_RANDOM_STATE ? GetVar(13) : GetVar(10);
	float targetMax = GetSkillBase()->GetEffectType() == SkillBase::EF_MAGIC_SINGLE_REGION_DAMAGE_ADD_RANDOM_STATE ? GetVar(14) : GetVar(11);

	float fEffectLength = effectLength * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, distributeType, targetMax, vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
	// }
}

void StructSkill::ACTIVATE_FIELD_PROP()
{
	StructFieldProp::iterator it = StructFieldProp::get( m_hTarget );

	if( !(*it) || !(*it)->IsFieldProp() )
	{
		return;
	}

	StructFieldProp * pProp = static_cast< StructFieldProp * >( *it );

	bool bRet = pProp->UseProp( static_cast< StructPlayer * >( m_pOwner ) );

	AddSkillResult( &m_vResultList, bRet );
}

void StructSkill::REGION_HEAL_BY_FIELD_PROP()
{
	StructFieldProp::iterator it = StructFieldProp::get( m_hTarget );

	if( !(*it) || !(*it)->IsFieldProp() )
	{
		return;
	}

	StructFieldProp * pProp = static_cast< StructFieldProp * >( *it );

	// UseProp에서 프랍이 삭제될 수 있으므로 위치는 미리 저장
	ArPosition posTarget = pProp->GetPos();
	unsigned char nTargetLayer = pProp->GetLayer();

	bool bRet = pProp->UseProp( static_cast< StructPlayer * >( m_pOwner ) );

	AddSkillResult( &m_vResultList, bRet );

	// 프랍 사용에 실패했으면 추가 처리도 일어나지 않음
	if( !bRet )
	{
		return;
	}

	int nHPHeal = GetVar(0) + GetVar(1) * GetRequestedSkillLevel();
	float fHPHeal = GetVar(2) + GetVar(3) * GetRequestedSkillLevel();

	m_fRange = GetVar( 4 ) * GameRule::DEFAULT_UNIT_SIZE;
	int nTargetType = GetVar( 5 );

	std::vector< AR_HANDLE > vResult;
	ArcadiaServer::Instance().EnumMovableObject( posTarget, nTargetLayer, m_fRange, &vResult );

	for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
	{
		StructCreature::iterator itCreature = StructCreature::get( (*it) );

		StructCreature * pCreature = (*itCreature);

		if( !pCreature || pCreature->IsPet() || pCreature->IsDead() )
			continue;

		switch( nTargetType )
		{
		case SkillBase::SKILL_EFFECT_TARGET_LIMIT_NOT_ENEMY:
			if( m_pOwner->IsEnemy( pCreature, true ) )
				continue;
			break;
		case SkillBase::SKILL_EFFECT_TARGET_LIMIT_ONLY_ALLY:
			if( !m_pOwner->IsAlly( pCreature ) )
				continue;
			break;
		case SkillBase::SKILL_EFFECT_TARGET_LIMIT_ONLY_ENEMY:
			if( !m_pOwner->IsEnemy( pCreature, true ) )
				continue;
			break;
		}

		int nIncHP = pCreature->Heal( nHPHeal + fHPHeal * pCreature->GetMaxHP() );

		SkillResult skill_result;

		skill_result.add_hp.type = SkillResult::ADD_HP;
		skill_result.add_hp.hTarget = pCreature->GetHandle();
		skill_result.add_hp.target_hp = pCreature->GetHP();
		skill_result.add_hp_mp_sp.nIncHP = nIncHP;

		m_vResultList.push_back( skill_result );
	}
}

void StructSkill::MAKE_AREA_EFFECT_PROP_BY_FIELD_PROP( bool bIsTrap )
{
	StructFieldProp::iterator it = StructFieldProp::get( m_hTarget );

	if( !(*it) || !(*it)->IsFieldProp() )
	{
		return;
	}

	StructFieldProp * pProp = static_cast< StructFieldProp * >( *it );

	// UseProp에서 프랍이 삭제될 수 있으므로 위치는 미리 저장
	ArPosition posTarget = pProp->GetPos();
	unsigned char nTargetLayer = pProp->GetLayer();

	bool bRet = pProp->UseProp( static_cast< StructPlayer * >( m_pOwner ) );

	AddSkillResult( &m_vResultList, bRet );

	// 프랍 사용에 실패했으면 추가 처리도 일어나지 않음
	if( !bRet )
	{
		return;
	}

	// 트랩은 1인당 동시 1개만 설치 가능(새로 설치하려면 기존꺼 제거)
	if( bIsTrap && m_pOwner->GetTrapHandle() )
	{
		StructSkillProp *pTrap = static_cast< StructSkillProp * >( GameObject::raw_get( m_pOwner->GetTrapHandle() ) );
		if( pTrap )
		{
			pTrap->PendRemove();
		}
	}

	int nMagicPoint = m_pOwner->GetMagicPoint( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	float fHateRatio = m_pOwner->GetHateRatio();
	// 스킬 프랍 생성
	ArObject *pPtr = StructSkillProp::Create( m_pOwner->GetHandle() , this, nMagicPoint, fHateRatio );

	if( bIsTrap && pPtr )
	{
		m_pOwner->SetTrapHandle( pPtr->GetHandle() );
	}

	pPtr->SetCurrentXY( posTarget.x, posTarget.y );
	pPtr->SetCurrentLayer( nTargetLayer );

	// 추가후 스케쥴러에 등록
	ArcadiaServer::Instance().AddObject( pPtr );
	ArcadiaServer::Instance().SetObjectPriority( pPtr, ArSchedulerObject::UPDATE_PRIORITY_HIGH );
	return;
}

void StructSkill::CREATE_ITEM(  struct StructCreature* pTarget, bool bSuccess )
{
	if( bSuccess )
	{
		StructPlayer *pInvenPlayer = NULL;
		if( m_pOwner->IsPlayer() ) pInvenPlayer = static_cast< StructPlayer* >( m_pOwner );
		if( m_pOwner->IsSummon() ) pInvenPlayer = static_cast< StructSummon* >( m_pOwner )->GetMaster();

		// TODO : LOCK 해야되는거 아닌가? by Testors
		for( int i = 0; i < 3; ++i )
		{
			int nItemCode = GetVar( 4*i ) + GetVar( 4*i+1 )*GetRequestedSkillLevel();
			__int64 nItemCount = GetVar( 4*i+2 ) + GetVar( 4*i+3 )*GetRequestedSkillLevel();

			while( nItemCode < 0 )
			{
				GameContent::SelectItemIDFromDropGroup( nItemCode, nItemCode, nItemCount );
			}

			if( nItemCode )
			{
				StructItem *pItem = StructItem::AllocItem( 0, nItemCode, nItemCount, ItemInstance::BY_SKILL );
				StructItem *pNewItem = pInvenPlayer->PushItem( pItem, pItem->GetCount() );

				if( pNewItem )
				{
					LOG::Log11N4S( LM_ITEM_TAKE, pInvenPlayer->GetAccountID(), pInvenPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewItem ? pNewItem->GetCount() : 0, 0, 0, pInvenPlayer->GetX(), pInvenPlayer->GetY(), pItem->GetItemUID(), pInvenPlayer->GetAccountName(), LOG::STR_NTS, pInvenPlayer->GetName(), LOG::STR_NTS, "", 0, "IGEN", LOG::STR_NTS );
				}

				if( pNewItem != pItem )
				{
					StructItem::PendFreeItem( pItem );
				}
			}
		}
	}

	SkillResult skill_result;
	skill_result.result.type = SkillResult::RESULT;
	skill_result.result.bResult = bSuccess;
	m_vResultList.push_back( skill_result );
}

void StructSkill::PHYSICAL_SINGLE_REGION_DAMAGE_OLD( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance();
	// }

	float fEffectLength = GetVar(2) * GameRule::DEFAULT_UNIT_SIZE;

	if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_OLD )
		fEffectLength += GetVar(7) * GetEnhance() * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;
	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, GetVar(3), GetVar(11), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	float fRange = 0;
	AR_TIME knock_back_time = 0;

	// { 넉백 스킬의 경우
	if( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_OLD )
	{
		fRange = GetVar(5) + GetVar(6) * GetRequestedSkillLevel() + GetVar(7) * GetEnhance();
		fRange *= GameRule::DEFAULT_UNIT_SIZE;

		knock_back_time = ( GetVar(8) + GetVar(9) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance() ) * 100;
	}
	// } 넉백 스킬의 경우

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		if( !Damage.bBlock && !Damage.bMiss && !Damage.bPerfectBlock && GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_OLD && !( pDealTarget->IsMonster() && static_cast< StructMonster * >( pDealTarget )->IsBossMonster() ) )
		{
			// 넉백 스킬의 경우
			AFFECT_KNOCK_BACK( pDealTarget, fRange, knock_back_time );
			AddSkillDamageWithKnockBackResult( &m_vResultList, SkillResult::DAMAGE_WITH_KNOCK_BACK, elemental_type, Damage, pDealTarget->GetHandle(), pDealTarget->GetPos().x, pDealTarget->GetPos().y, knock_back_time );
		}
		else
		{
			AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
		}
	}
	// }
}

void StructSkill::PHYSICAL_MULTIPLE_REGION_DAMAGE_OLD( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(8) * GetEnhance();
	// }

	// { 연타 횟수
	int nCount = GetVar( 2 ) + GetVar(3) * GetRequestedSkillLevel();
	// }

	float fEffectLength = ( GetVar(5) + GetVar(9) * GetEnhance() ) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, GetVar( 6 ), GetVar( 7 ), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( int i = 0; i < nCount; ++i )
	{
		for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
		{
			StructCreature* pDealTarget = (*itTargetList);

			StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

			AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
		}
	}
	// }

	m_nFireTime += GetVar(4) * nCount * 100;
}

// 타겟 중심 특수범위 다중 데미지(실시간 연타가 아닌 1개의 SkillResult로 여러대 치는 놈)
void StructSkill::PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel();
	// }

	// { 연타 횟수
	int nCount = GetVar( 10 ) + GetVar( 11 ) * GetRequestedSkillLevel();
	// }

	float fEffectLength = GetVar(8) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), GetVar( 6 ), fEffectLength, GetVar( 5 ), GetVar( 9 ), nDamage, GetVar( 7 ), m_pOwner, GetVar( 2 ), GetVar( 3 ), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( int i = 0; i < nCount; ++i )
	{
		for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
		{
			StructCreature* pDealTarget = (*itTargetList);

			StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

			AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
		}
	}
	// }

	m_nFireCount = nCount;
	m_nFireTime += GetVar(4) * nCount * 100;
}

void StructSkill::PHYSICAL_SPECIAL_REGION_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance();
	// }

	float fEffectLength = GetVar(8) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), !!GetVar(6), fEffectLength, GetVar(5), GetVar(9), nDamage, GetVar(7), m_pOwner, GetVar( 2 ), GetVar( 3 ), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
	// }
}

void StructSkill::ADD_REGION_STATE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	AR_TIME t = GetArTime();

	float fEffectLength = GetVar( 5 ) * GameRule::DEFAULT_UNIT_SIZE;

	int nTargetType = GetVar( 6 );
	int nRandomType	= GetVar( 7 );
	StructState::StateCode nForcedCode = StructState::StateCode( 0 );

	if( nRandomType == StructState::COMMON_RANDOM_STATE )
	{
		nForcedCode = static_cast< StructState::StateCode >( GetStateId() );

		int count;
		for( count = 6; count >= 2; --count )	if( GetVar( count - 2 ) ) break;

		int index = XRandom( -1, count - 2 );
		if( index != -1 ) nForcedCode = static_cast< StructState::StateCode >( static_cast< int >( GetVar(index) ) );
	}

	std::vector< AR_HANDLE > vResult;
	m_fRange = fEffectLength;

	STATE_SKILL_FUNCTOR mySkillFunctor( &m_vResultList, nForcedCode );

	ArcadiaServer::Instance().EnumMovableObject( pTarget->GetCurrentPosition( t ), pTarget->GetLayer(), fEffectLength, &vResult );

	m_nTargetCount = 0;

	for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
	{
		StructCreature::iterator itCreature = StructCreature::get( (*it) );

		StructCreature * pCreature = (*itCreature);

		if( !pCreature || pCreature->IsPet() )
			continue;

		if( pCreature->IsPlayer() && !IsUseableOnAvatar() ) 
		{
			continue;
		}
		if( pCreature->IsMonster() && !IsUseableOnMonster() )
		{
			continue;
		}
		if( pCreature->IsSummon() && !IsUseableOnSummon() )
		{
			continue;
		}

		if( nTargetType == 1 )
		{
			if( m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}
		else if( nTargetType == 2 )
		{
			if( !m_pOwner->IsAlly( pCreature ) )
				continue;
		}
		else if( nTargetType == 3 )
		{
			if( !m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}

		mySkillFunctor.onCreature( this, t, m_pOwner, pCreature );

		++m_nTargetCount;
	}
}

void StructSkill::ADD_STATE_BY_SELF_COST( struct StructCreature* pTarget )
{
	if( !pTarget || !pTarget->IsSummon() )
		return;

	float fCostHP = GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();
	float fCostSP = GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	float fCostEnergy = GetVar(6) + GetVar(7) * GetRequestedSkillLevel() + GetVar(8) * GetEnhance();
	float fCostMP = GetVar(9) + GetVar(10) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();

	if( pTarget->GetHP() < fCostHP )
		return;
	if( static_cast< StructSummon * >(pTarget)->GetSP() < fCostSP )
		return;
	if( fCostEnergy > 0 && pTarget->GetEnergyCount() < fCostEnergy )
		return;
	if( pTarget->GetMP() < fCostMP )
		return;

	pTarget->AddHP( -fCostHP );
	static_cast< StructSummon * >(pTarget)->AddSP( -fCostSP );
	if( fCostEnergy > 0 )
		pTarget->RemoveEnergy( fCostEnergy );
	else if( fCostEnergy == -1 )
		// fCostEnergy가 -1이면 모든 기공 소모
		pTarget->RemoveEnergy( pTarget->GetEnergyCount() );
	pTarget->AddMP( -fCostMP );

	AR_TIME t = GetArTime();

	STATE_SKILL_FUNCTOR mySkillFunctor( &m_vResultList );

	mySkillFunctor.onCreature( this, t, m_pOwner, pTarget );
}

void StructSkill::ADD_REGION_STATE_BY_SELF_COST( struct StructCreature* pTarget )
{
	if( !pTarget || !pTarget->IsSummon() )
		return;

	float fCostHP = GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();
	float fCostSP = GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	float fCostEnergy = GetVar(6) + GetVar(7) * GetRequestedSkillLevel() + GetVar(8) * GetEnhance();

	float fEffectLength = GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;

	int nTargetType = GetVar(10);

	AR_TIME t = GetArTime();

	std::vector< AR_HANDLE > vResult;
	m_fRange = fEffectLength;

	STATE_SKILL_FUNCTOR mySkillFunctor( &m_vResultList );

	ArcadiaServer::Instance().EnumMovableObject( pTarget->GetCurrentPosition( t ), pTarget->GetLayer(), fEffectLength, &vResult );

	m_nTargetCount = 0;

	for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
	{
		StructCreature::iterator itCreature = StructCreature::get( (*it) );

		StructCreature * pCreature = (*itCreature);

		if( !m_pOwner->IsPlayer() || !pCreature || !pCreature->IsSummon() || !static_cast< StructSummon * >(pCreature)->GetMaster()->GetPartyID() ||
			static_cast< StructSummon * >(pCreature)->GetMaster()->GetPartyID() != static_cast< StructPlayer * >(m_pOwner)->GetPartyID() )
			continue;

		if( nTargetType == 1 )
		{
			if( m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}
		else if( nTargetType == 2 )
		{
			if( !m_pOwner->IsAlly( pCreature ) )
				continue;
		}
		else if( nTargetType == 3 )
		{
			if( !m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}

		if( pCreature->GetHP() < fCostHP )
			continue;
		if( static_cast< StructSummon * >(pCreature)->GetSP() < fCostSP )
			continue;
		if( pCreature->GetEnergyCount() < fCostEnergy )
			continue;

		pCreature->AddHP( -fCostHP );
		static_cast< StructSummon * >(pCreature)->AddSP( -fCostSP );
		pCreature->RemoveEnergy( -fCostEnergy );

		mySkillFunctor.onCreature( this, t, m_pOwner, pCreature );

		++m_nTargetCount;
	}
}

void StructSkill::ADD_STATE_STEP_BY_STEP( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	for( int i = 9; i >= 0; i-=3 )
	{
		if( this->GetVar( i ) == 0 ) continue;

		if( pTarget->GetState( static_cast< StructState::StateCode >( static_cast< int >( this->GetVar( i ) ) ) ) )
		{
			pTarget->RemoveState( static_cast< StructState::StateCode >( static_cast< int >( this->GetVar( i ) ) ), GameRule::MAX_STATE_LEVEL );

			int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
			int nDamage = nMagicPoint * ( GetVar(12) + GetVar(13) * GetRequestedSkillLevel() + GetVar(14) * GetEnhance() );

			int elemental_type = GetSkillBase()->GetElementalType();

			StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

			AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
		}
		else
		{
			AR_TIME t = GetArTime();

			STATE_SKILL_FUNCTOR mySkillFunctor( &m_vResultList );

			mySkillFunctor.onCreature( this, t, m_pOwner, pTarget );
		}

		return;
	}

}

// 타겟이 파티원이면 파티원 주변의 몹을 파티원에게 끌어다 준다.
// 타겟이 몹이면 몹 주변 몹을 전부 시전자한테 끌어온다.
void StructSkill::REGION_TAUNT( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nHate = GetVar( 0 ) + GetVar( 1 ) * GetRequestedSkillLevel() + GetVar( 7 ) * GetEnhance();

	float fEffectLength = ( GetVar( 2 ) + GetVar( 8 ) * GetEnhance() ) * GameRule::DEFAULT_UNIT_SIZE;

	int nRatio = GetVar( 5 ) + GetVar( 6 ) * GetRequestedSkillLevel() + GetVar( 9 ) * GetEnhance();

	m_fRange = fEffectLength;

	// 어그로 받을 대상은 기본적으로 시전자
	StructCreature *pHateReceiver = m_pOwner;

	int nPartyID = 0;
	if( m_pOwner->IsPlayer() )
		nPartyID = static_cast< StructPlayer * >(m_pOwner)->GetPartyID();
	else if( m_pOwner->IsSummon() )
		nPartyID = static_cast< StructSummon * >(m_pOwner)->GetMaster()->GetPartyID();

	// 타겟이 유저이면서 같은 파티원이면 어그로 받는 대상이 타겟이 됨
	if( pTarget->IsPlayer() && static_cast< StructPlayer * >(pTarget)->GetPartyID() &&
		static_cast< StructPlayer * >(pTarget)->GetPartyID() == nPartyID )
	{
		pHateReceiver = pTarget;
	}

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nHate = EnumSkillTargetsAndCalcDamage( pHateReceiver->GetCurrentPosition( t ), pHateReceiver->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nHate, true, m_pOwner, GetVar( 3 ), GetVar( 4 ), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 헤이트 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		bool bSuccess = XRandom() % 100 < nRatio;

		if( bSuccess && pDealTarget->IsEnemy( pHateReceiver, true ) && pDealTarget->IsMonster() )
		{
			std::pair< float, int > HateMod = m_pOwner->GetHateMod( ( GetSkillBase()->IsPhysicalSkill() ) ? 1 : 2, GetSkillBase()->IsHarmful() );

			nHate += HateMod.second;
			nHate *= HateMod.first;

			static_cast< StructMonster * >( pDealTarget )->AddHate( pHateReceiver->GetHandle(), pHateReceiver->GetHateRatio() * nHate );
		}

		AddSkillResult( &m_vResultList, bSuccess, SkillResult::ADD_HATE, pDealTarget->GetHandle() );
	}
}

void StructSkill::TAUNT( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nHate = GetVar( 0 ) + GetVar( 1 ) * GetRequestedSkillLevel() + GetVar( 7 ) * GetEnhance();

	int nRatio = GetVar( 5 ) + GetVar( 6 ) * GetRequestedSkillLevel() + GetVar( 9 ) * GetEnhance();

	bool bSuccess = XRandom() % 100 < nRatio;

	if( bSuccess && pTarget->IsEnemy( m_pOwner ) && pTarget->IsMonster() )
	{
		std::pair< float, int > HateMod = m_pOwner->GetHateMod( ( GetSkillBase()->IsPhysicalSkill() ) ? 1 : 2, GetSkillBase()->IsHarmful() );

		nHate += HateMod.second;
		nHate *= HateMod.first;

		static_cast< StructMonster * >( pTarget )->AddHate( m_pOwner->GetHandle(), m_pOwner->GetHateRatio() * nHate );
	}

	AddSkillResult( &m_vResultList, bSuccess, SkillResult::ADD_HATE, pTarget->GetHandle() );
}

void StructSkill::REMOVE_HATE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	if( !pTarget->IsMonster() )
		return;

	float fHateRemoveRate = GetVar( 0 ) + GetVar( 1 ) * GetRequestedSkillLevel() + GetVar( 7 ) * GetEnhance();

	int nSuccessRate = GetVar( 5 ) + GetVar( 6 ) * GetRequestedSkillLevel() + GetVar( 9 ) * GetEnhance();

	bool bSuccess = XRandom( 0, 99 ) < nSuccessRate;

	if( bSuccess && m_pOwner->IsEnemy( pTarget ) )
	{
		StructMonster *pMonster = static_cast< StructMonster * >( pTarget );

		int nHate = pMonster->GetHate( m_pOwner->GetHandle() );

		pMonster->RemoveHate( m_pOwner->GetHandle(), nHate * fHateRemoveRate / 100.0f );
	}

	AddSkillResult( &m_vResultList, bSuccess, SkillResult::ADD_HATE, pTarget->GetHandle() );
}

void StructSkill::REGION_REMOVE_HATE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;
	StructCreature *pCaster = NULL;

	switch( GetSkillBase()->GetEffectType() )
	{
	case SkillBase::EF_REGION_REMOVE_HATE:
		pCaster = m_pOwner;
		break;
	case SkillBase::EF_REGION_REMOVE_HATE_OF_TARGET:
		pCaster = pTarget;
		break;
	}	

	if( GetVar( 10 ) != 0 && GetVar( 13 ) + GetVar( 14 ) * GetRequestedSkillLevel() > XRandom( 0, 99 ) )
	{
		AR_TIME t = GetArTime();
		pTarget->AddState( static_cast< StructState::StateCode >( static_cast< int >( GetVar( 10 ) ) ), m_pOwner->GetHandle(), GetVar( 11 ) + GetVar( 12 ) * GetRequestedSkillLevel(), t, t + 100 * ( GetVar( 15 ) + ( GetVar( 16 ) * GetRequestedSkillLevel() ) ) );
	}

	float fHateRemoveRate = GetVar( 0 ) + GetVar( 1 ) * GetRequestedSkillLevel() +  GetVar( 7 ) * GetEnhance();
	float fEffectLength = ( GetVar( 2 ) + GetVar( 8 ) * GetEnhance() ) * GameRule::DEFAULT_UNIT_SIZE;

	int nSuccessRate = GetVar( 5 ) + GetVar( 6 ) * GetRequestedSkillLevel() + GetVar( 9 ) * GetEnhance();

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	EnumSkillTargetsAndCalcDamage( pCaster->GetCurrentPosition( t ), pCaster->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, 0, true, pCaster, GetVar( 3 ), GetVar( 4 ), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 헤이트 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		if( !(*itTargetList)->IsMonster() )
			continue;

		StructMonster *pMonster = static_cast< StructMonster * >( *itTargetList );

		bool bSuccess = XRandom( 0, 99 ) < nSuccessRate;

		if( bSuccess && pCaster->IsEnemy( pMonster ) )
		{
			int nHate = pMonster->GetHate( pCaster->GetHandle() );

			pMonster->RemoveHate( pCaster->GetHandle(), nHate * fHateRemoveRate / 100.0f );
		}

		AddSkillResult( &m_vResultList, bSuccess, SkillResult::ADD_HATE, pMonster->GetHandle() );
	}
}

void StructSkill::MAGIC_SINGLE_REGION_DAMAGE_OLD( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	// }

	float fEffectLength = GetVar(2) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, GetVar( 3 ), GetVar( 4 ), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
	// }
}

void StructSkill::MAGIC_MULTIPLE_REGION_DAMAGE_AT_ONCE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	int nDamage = ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) * nMagicPoint + GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	int nCount = GetVar(6) + GetVar(7)*GetRequestedSkillLevel();	

	int elemental_type = GetSkillBase()->GetElementalType();

	float effectLength = GetVar(9);
	float distributeType = GetVar(10);
	float targetMax = GetVar(11);

	float fEffectLength = effectLength * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, distributeType, targetMax, vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( int i = 0; i < nCount; ++i )
	{
		for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
		{
			StructCreature* pDealTarget = (*itTargetList);

			StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

			AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
		}
	}
	m_nFireCount = nCount;
	m_nFireTime += GetVar(8) * nCount * 100;
}

void StructSkill::MAGIC_MULTIPLE_REGION_DAMAGE_OLD( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(8) * GetEnhance();
	// }

	if( m_nCurrentFire == 0 )
	{
		// { 횟수 계산
		m_nTotalFire = GetVar(2) + GetVar(3) * GetRequestedSkillLevel();
		// }
		m_nCurrentFire = 1;
	}
	else
	{
		++m_nCurrentFire;
	}

	float fEffectLength = ( GetVar(5) + GetVar(9) * GetEnhance() ) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, GetVar(6), GetVar(7), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
	// }

	m_nFireTime += GetVar(4) * 100;
}

void StructSkill::MAGIC_MULTIPLE_REGION_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage =  ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) * m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) + GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	// }

	if( m_nCurrentFire == 0 )
	{
		// { 횟수 계산
		m_nTotalFire = GetVar(6) + GetVar(7) * GetRequestedSkillLevel();
		// }
		m_nCurrentFire = 1;
	}
	else
	{
		++m_nCurrentFire;
	}

	float fEffectLength = GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, GetVar(10), GetVar(11), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
	// }

	m_nFireTime += GetVar(8) * 100;
}

void StructSkill::MAGIC_SINGLE_REGION_PERCENT_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = 0;	// 대상별 상대치로 계산되어야 하므로 데미지 적용 루프에서 계산한다
	// }

	float fEffectLength = GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, GetVar( 10 ), GetVar( 11 ), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		// 보스급 몬스터 이상이면 실패
		if( !pDealTarget->IsMonster() || static_cast< StructMonster * >(pDealTarget)->IsBossMonster() )
		{
			--m_nTargetCount;
			continue;
		}

		int nTargetDamage = ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) * pDealTarget->GetHP();

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nTargetDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ), StructCreature::IGNORE_DEFENCE );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
	// }
}

void StructSkill::MAGIC_MULTIPLE_REGION_DAMAGE_T2( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(8) * GetEnhance();
	// }

	// { 횟수 계산
	int nCount = GetVar(2) + GetVar(3) * GetRequestedSkillLevel();
	// }

	float fEffectLength = ( GetVar(5) + GetVar(9) * GetEnhance() ) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, GetVar(6), GetVar(7), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( int i = 0; i < nCount; ++i )
	{
		for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
		{
			StructCreature* pDealTarget = (*itTargetList);

			StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

			AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
		}
	}
	// }
	m_nFireCount = nCount;
	m_nFireTime += GetVar(4) * nCount * 100;
}

void StructSkill::MAGIC_SPECIAL_REGION_DAMAGE_OLD( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// { 데미지 계산
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance();
	// }

	float fEffectLength = GetVar(8) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), !!GetVar(6), fEffectLength, GetVar(5), GetVar(9), nDamage, GetVar(7), m_pOwner, GetVar(2), GetVar(3), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
	// }
}

void StructSkill::MAGIC_SPECIAL_REGION_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	int nDamage = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) + GetVar(3) + GetVar(4) * GetRequestedSkillLevel();

	int elemental_type = GetSkillBase()->GetElementalType();
	// }

	float fEffectLength = GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), !!GetVar(6), fEffectLength, GetVar(5), GetVar(8), nDamage, GetVar(7), m_pOwner, GetVar(10), GetVar(11), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
	// }
}

void StructSkill::SINGLE_PHYSICAL_DAMAGE_WITH_SHIELD( struct StructCreature* pTarget )
{
	if( !pTarget || !m_pOwner->IsWearShield() ) return;

	// 데미지가 스텟에 의한 공격력만 계산하고 거기에 증가다 -┏
	int nAttackPoint = GameRule::GetAttackPoint( m_pOwner->GetStrength(), m_pOwner->GetLevel(), m_pOwner->GetFCM() );
	int nDamage = nAttackPoint + GetVar(0) + ( GetVar(1) * GetRequestedSkillLevel() ) + ( GetVar(4) * GetEnhance() );
	int elemental_type = GetSkillBase()->GetElementalType();

	int nCastingCancelRate = GetVar(2) + ( GetVar(3) * GetRequestedSkillLevel() ) + ( GetVar(5) * GetEnhance() );

	// 캐스팅 캔슬 성공~
	StructSkill *pCancelSkill = pTarget->GetCastSkill();
	if( pCancelSkill && pCancelSkill->m_Status == STATUS_CAST && XRandom( 0, 99 ) < nCastingCancelRate )
	{
		pTarget->CancelSkill();

		AR_TIME t = GetArTime();
		pTarget->AddState( static_cast< StructState::StateCode >(GetStateId()), m_pOwner->GetHandle(), GetRequestedSkillLevel(), t, t + GetVar(7) + ( GetVar(8) * GetRequestedSkillLevel() ) );
	}

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}

void StructSkill::PHYSICAL_DIRECTIONAL_DAMAGE( struct StructCreature * pTarget )
{
	if( !pTarget ) return;

	int nDamage = 0;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	// 데미지
	int elemental_type = GetSkillBase()->GetElementalType();
	nDamage = nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	// 방향성 체크
	int nAccAdd = 0;
	if( m_pOwner->IsBackOf( *pTarget ) ) nAccAdd = GetVar( 2 );
	else if( m_pOwner->IsSideOf( *pTarget ) ) nAccAdd = GetVar( 3 );
	else nAccAdd = GetVar( 4 );

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ) + nAccAdd, GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}

void StructSkill::SINGLE_PHYSICAL_DAMAGE_T1( struct StructCreature * pTarget )
{
	if( !pTarget ) return;

	int nDamage = 0;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	if( GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK_OLD )
	{
		if( m_nCurrentFire == 0 )
		{
			m_nCurrentFire = 1;
			m_nTotalFire = 2;

			if( !PHYSICAL_DAMAGE_RUSH( pTarget, &m_nRushDamage ) )
			{
				m_nCurrentFire = 2;
			}

			return;
		}
		else
		{
			ArcadiaServer::Instance().MoveObject( m_pOwner, m_RushPos, m_fRushFace );
			nDamage += m_nRushDamage;
			++m_nCurrentFire;
		}
	}
	// 데미지
	int elemental_type = GetSkillBase()->GetElementalType();
	nDamage += nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	if( !Damage.bBlock && !Damage.bMiss && !Damage.bPerfectBlock
		&& ( GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_KNOCKBACK_OLD
		|| GetSkillBase()->GetSkillEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK_OLD )
		&& !( pTarget->IsMonster() && static_cast< StructMonster * >( pTarget )->IsBossMonster() ) )
	{
		float fRange = GetVar(5) + GetVar(6) * GetRequestedSkillLevel() + GetVar(7) * GetEnhance();
		fRange *= GameRule::DEFAULT_UNIT_SIZE;

		AR_TIME knock_back_time = ( GetVar(8) + GetVar(9) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance() ) * 100;

		AFFECT_KNOCK_BACK( pTarget, fRange, knock_back_time );
		AddSkillDamageWithKnockBackResult( &m_vResultList, SkillResult::DAMAGE_WITH_KNOCK_BACK, elemental_type, Damage, pTarget->GetHandle(), pTarget->GetPos().x, pTarget->GetPos().y, knock_back_time );
	}
	else
	{
		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}

	return;
}

void StructSkill::SINGLE_PHYSICAL_DAMAGE_T2( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDamage = 0;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	nDamage = nAttackPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance() );
	int nMax = nAttackPoint + GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	nDamage = std::min( nDamage, nMax );

	int elemental_type = GetSkillBase()->GetElementalType();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}

void StructSkill::SINGLE_PHYSICAL_DAMAGE_T2_ADD_ENERGY( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDamage = 0;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	nDamage = nAttackPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance() );
	int nMax = nAttackPoint + GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	nDamage = std::min( nDamage, nMax );

	int elemental_type = GetSkillBase()->GetElementalType();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	for( int i = 0 ; i < GetVar(6) + GetVar(7) * GetRequestedSkillLevel() + GetVar(8) * GetEnhance() ; ++i )
		m_pOwner->AddEnergy();

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}

void StructSkill::SINGLE_PHYSICAL_DAMAGE_T3( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDamage = 0;

	int nAttackPoint = m_pOwner->GetAttackPointRightWithoutWeapon( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	if( GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITHOUT_WEAPON_RUSH_KNOCK_BACK )
	{
		if( m_nCurrentFire == 0 )
		{
			m_nCurrentFire = 1;
			m_nTotalFire = 2;

			if( !PHYSICAL_DAMAGE_RUSH( pTarget, &m_nRushDamage ) )
			{
				m_nCurrentFire = 2;
			}

			return;
		}
		else
		{
			ArcadiaServer::Instance().MoveObject( m_pOwner, m_RushPos, m_fRushFace );
			nDamage += m_nRushDamage;

			++m_nCurrentFire;
		}
	}

	nDamage += nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance();

	int elemental_type = GetSkillBase()->GetElementalType();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	if( !Damage.bBlock && !Damage.bMiss && !Damage.bPerfectBlock && GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_WITHOUT_WEAPON_RUSH_KNOCK_BACK
		&& !( pTarget->IsMonster() && static_cast< StructMonster * >( pTarget )->IsBossMonster() ) )
	{
		float fRange = GetVar(5) + GetVar(6) * GetRequestedSkillLevel() + GetVar(7) * GetEnhance();
		fRange *= GameRule::DEFAULT_UNIT_SIZE;

		AR_TIME knock_back_time = ( GetVar(8) + GetVar(9) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance() ) * 100;

		AFFECT_KNOCK_BACK( pTarget, fRange, knock_back_time );
		AddSkillDamageWithKnockBackResult( &m_vResultList, SkillResult::DAMAGE_WITH_KNOCK_BACK, elemental_type, Damage, pTarget->GetHandle(), pTarget->GetPos().x, pTarget->GetPos().y, knock_back_time );
	}
	else
	{
		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}

	return;
}

void StructSkill::MULTIPLE_PHYSICAL_DAMAGE_T1( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	int nDamage = nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
	int nCount = GetVar(2) + GetVar(3) * GetRequestedSkillLevel();

	int elemental_type = GetSkillBase()->GetElementalType();

	// 데미지
	for( int i = 0; i < nCount; ++i )
	{		
		StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}
	m_nFireCount = nCount;
	m_nFireTime += GetVar(4) * nCount * 100;

	return;
}

void StructSkill::MULTIPLE_PHYSICAL_DAMAGE_T2( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	int nDamage = nAttackPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance() );
	int nMax = nAttackPoint + GetVar(2) + GetVar(3) * GetRequestedSkillLevel();
	nDamage = std::min( nDamage, nMax );
	int nCount = GetVar(2) + GetVar(3)*GetRequestedSkillLevel();	

	// 데미지
	for( int i = 0; i < nCount; ++i )
	{
		int elemental_type = GetSkillBase()->GetElementalType();
		StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, GetSkillBase()->GetElementalType(), Damage, pTarget->GetHandle() );
	}

	m_nFireCount = nCount;
	// 연타 횟수 만큼 행동 불가
	m_nFireTime += GetVar(4) * nCount * 100;

	return;
}

void StructSkill::MULTIPLE_PHYSICAL_DAMAGE_T3( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	int nDamage = nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	if( m_nCurrentFire == 0 )
	{
		m_nTotalFire = GetVar(2) + GetVar(3) * GetRequestedSkillLevel();
		m_nCurrentFire = 1;
	}
	else
	{
		++m_nCurrentFire;
	}

	int elemental_type = GetSkillBase()->GetElementalType();

	// 데미지
	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	m_nFireTime += GetVar(4) * 100;

	return;
}

void StructSkill::MULTIPLE_PHYSICAL_DAMAGE_T4( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nAttackPoint = m_pOwner->GetAttackPointRight( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	int nDamage = nAttackPoint + GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	if( m_nCurrentFire == 0 )
	{
		m_nTotalFire = std::min( (int)( ( GetRequestedSkillLevel() - 1 ) / GetVar(2) + 1 ), 3 );
		m_nCurrentFire = 1;
	}
	else
	{
		++m_nCurrentFire;
	}

	int elemental_type = GetSkillBase()->GetElementalType();

	int nCount = GetVar( m_nCurrentFire + 5 );

	// 데미지
	for( int i = 0; i < nCount; ++i )
	{
		elemental_type = GetSkillBase()->GetElementalType();
		StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}

	m_nFireCount = nCount;

	// 연타 횟수 만큼 행동 불가
	m_nFireTime += GetVar( 9 ) + m_nCurrentFire - 1;

	return;
}

void StructSkill::MAGIC_ABSORB_DAMAGE( struct StructCreature* pTarget )
{
	// 실제마법공격력 = 마법공격력 + [값1] + (SLv*[값2]), HP흡수량 = (데미지*[값3]) + (데미지*EP*[값5]), MP흡수량 = (데미지*[값4]) + (데미지*EP*[값6])
	if( !pTarget ) return;

	int nDamage = 0;

	nDamage = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) + GetVar(0) + GetVar(1) * GetRequestedSkillLevel();
	// + GetEnhance() * GetVar(2);

	int elemental_type = GetSkillBase()->GetElementalType();
	int nRealDamage = 0;

	nRealDamage = pTarget->GetHP();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	nRealDamage = std::min( nRealDamage, Damage.nDamage );
	// HP흡수량 = (데미지*[값3]) + (데미지*EP*[값5]), MP흡수량 = (데미지*[값4]) + (데미지*EP*[값6])
	int nHPAbsorb = GetVar( 2 ) * nRealDamage + GetVar( 4 ) * Damage.nDamage * GetEnhance();
	int nMPAbsorb = GetVar( 3 ) * nRealDamage + GetVar( 5 ) * Damage.nDamage * GetEnhance();

	m_pOwner->AddHP( nHPAbsorb );
	m_pOwner->AddMP( nMPAbsorb );

	SkillResult skill_result;

	skill_result.add_hp.type = SkillResult::ADD_HP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nHPAbsorb;
	skill_result.add_hp.target_hp = m_pOwner->GetHP();

	m_vResultList.push_back( skill_result );

	skill_result.add_hp.type = SkillResult::ADD_MP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nMPAbsorb;
	skill_result.add_hp.target_hp = m_pOwner->GetMP();

	m_vResultList.push_back( skill_result );
}

void StructSkill::CORPSE_ABSORB( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// 몬스터에게만 사용 가능
	if( !pTarget->IsMonster() || !pTarget->IsDead() ) return;

	RemoveMonsterFromWorld( static_cast< StructMonster * >( pTarget ) );

	int nHP = GetVar(0) + GetRequestedSkillLevel()*GetVar(1) + GetEnhance()*GetVar(4);
	int nMP = GetVar(2) + GetRequestedSkillLevel()*GetVar(3) + GetEnhance()*GetVar(5);

	StructCreature *pHealTarget = m_pOwner;

	if( GetVar(6) && m_pOwner->IsPlayer() )
		pHealTarget = static_cast< StructSummon * >( static_cast< StructPlayer * >(m_pOwner)->GetMainSummon() );

	nHP = pHealTarget->Heal( nHP );
	nMP = pHealTarget->MPHeal( nMP );

	SkillResult skill_result;

	skill_result.add_hp_mp_sp.type = SkillResult::ADD_HP_MP_SP;
	skill_result.add_hp_mp_sp.hTarget = pHealTarget->GetHandle();
	skill_result.add_hp_mp_sp.target_hp = pHealTarget->GetHP();
	skill_result.add_hp_mp_sp.nIncHP = nHP;
	skill_result.add_hp_mp_sp.nIncMP = nMP;
	skill_result.add_hp_mp_sp.nIncSP = 0;
	skill_result.add_hp_mp_sp.target_mp = pHealTarget->GetMP();

	m_vResultList.push_back( skill_result );

	return;
}

void StructSkill::CORPSE_EXPLOSION( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// 몬스터에게만 사용 가능
	if( !pTarget->IsMonster() || !pTarget->IsDead() ) return;

	int elemental_type = GetVar( 6 );
	int nDamage = GetVar(0) + GetVar(1) * GetRequestedSkillLevel();
	// }

	float fEffectLength = ( GetVar( 2 ) + GetVar( 5 ) * GetEnhance() ) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), true, fEffectLength, -1, 0, nDamage, true, m_pOwner, GetVar(3), GetVar(4), vTargetList );

	RemoveMonsterFromWorld( static_cast< StructMonster * >( pTarget ) );

	m_nTargetCount = 0;

	// { 모든 타겟에 데미지 적용하세~
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );

		++m_nTargetCount;
	}
	// }

	return;
}

const unsigned short StructSkill::IsInstanceGameEnterableOwner() const
{
	StructPlayer * pPlayer = static_cast< StructPlayer * >( m_pOwner );

	// 배틀 아레나에 참여 중일 경우 추가 체크/처리
	int nArenaID = pPlayer->GetBattleArenaID();
	if( nArenaID )
	{
		// 이미 시작된 경기(혹은 아직 시작되진 않았지만 연습 경기)에 참여 중인 경우에는 입장 불가
		// * 위의 경우들은 파티에 참여 중
		int nPartyID = pPlayer->GetPartyID();
		if( nPartyID )
		{
			assert( PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) );

			return RESULT_NOT_ACTABLE_IN_BATTLE_ARENA;
		}

		// 아직 파티에 소속은 아닌데 아레나ID가 세팅되어 있다면 대기열에 대기 중인 경우. 대기열 자동 탈퇴시켜 줌.
		BattleArenaManager::Instance().QuitGame( pPlayer, false, false, ALT_ENTER_HUNTAHOLIC );

		// 대기열 이탈 처리도 제대로 안 됐다면 실패 처리
		if( pPlayer->GetBattleArenaID() )
		{
			// 근데 그럼 이거 무슨 경우지;?
			assert( 0 );

			return RESULT_NOT_ACTABLE_IN_BATTLE_ARENA;
		}
	}

	return RESULT_SUCCESS;
}

// 인스턴스 게임 입장
void StructSkill::INSTANCE_GAME_ENTER()
{
	if( !m_pOwner->IsPlayer() )
		return;

	unsigned short nErrorCode = IsInstanceGameEnterableOwner();

	if( nErrorCode != RESULT_SUCCESS )
	{
		AddSkillResult( &m_vResultList, false, nErrorCode );
		return;
	}

	StructPlayer *pPlayer = static_cast< StructPlayer* >( m_pOwner );

	// 입장시 상태 저장(좌표, HP/MP)
	pPlayer->StoreCurrentStatesOnEnterInstanceGame( true );

	switch( GetSkillId() )
	{
	case SKILL_RANKED_DEATHMATCH_ENTER:
		pPlayer->PendWarpToDeathmatch( 1 );
		break;
	case SKILL_FREED_DEATHMATCH_ENTER:
		pPlayer->PendWarpToDeathmatch( 2 );
		break;
	default:
		// 아무 결과 메시지도 없이 씹어버리기
		return;
	}

	AddSkillResult( &m_vResultList, true, RESULT_SUCCESS );
}

void StructSkill::INSTANCE_GAME_EXIT()
{
	if( !m_pOwner->IsPlayer() )
		return;

	StructPlayer *pPlayer = static_cast< StructPlayer* >( m_pOwner ) ;

	ArPosition pos;
	pPlayer->GetPositionOnEnterInstanceGame( &pos );

	unsigned short nErrorCode = RESULT_SUCCESS;

	unsigned char layer = 0;
	int current_channel = ChannelManager::GetChannelId( pPlayer->GetX(), pPlayer->GetY() );
	int target_channel = ChannelManager::GetChannelId( pos.x, pos.y );

	if( current_channel && current_channel == target_channel )
	{
		layer = pPlayer->GetLayer();
	}
	else if( target_channel )
	{
		layer = ChannelManager::GetProperLayer( pos.x, pos.y );
	}

	pPlayer->PendWarp( pos.x, pos.y, layer );
	pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );

	AddSkillResult( &m_vResultList, nErrorCode == RESULT_SUCCESS, nErrorCode );
}

const int StructSkill::GetTargetHuntaholicID() const
{
	if( GetSkillId() != SKILL_WARP_TO_HUNTAHOLIC_LOBBY )
		return 0;

	// 현재는 어떠한 조건에도 무조건 베어로드로 ㄱㄱ임
	return 10000;
}

const unsigned short StructSkill::IsHuntaholicLobbyEnterableOwner( const int nHuntaholicID ) const
{
	StructPlayer * pPlayer = static_cast< StructPlayer * >( m_pOwner );

	// 배틀 아레나에 참여 중일 경우 추가 체크/처리
	int nArenaID = pPlayer->GetBattleArenaID();
	if( nArenaID )
	{
		// 이미 시작된 경기(혹은 아직 시작되진 않았지만 연습 경기)에 참여 중인 경우에는 입장 불가
		// * 위의 경우들은 파티에 참여 중
		int nPartyID = pPlayer->GetPartyID();
		if( nPartyID )
		{
			assert( PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) );

			return RESULT_NOT_ACTABLE_IN_BATTLE_ARENA;
		}

		// 아직 파티에 소속은 아닌데 아레나ID가 세팅되어 있다면 대기열에 대기 중인 경우. 대기열 자동 탈퇴시켜 줌.
		BattleArenaManager::Instance().QuitGame( pPlayer, false, false, ALT_ENTER_HUNTAHOLIC );

		// 대기열 이탈 처리도 제대로 안 됐다면 실패 처리
		if( pPlayer->GetBattleArenaID() )
		{
			// 근데 그럼 이거 무슨 경우지;?
			assert( 0 );

			return RESULT_NOT_ACTABLE_IN_BATTLE_ARENA;
		}
	}

	AR_TIME t = GetArTime();

	// 이미 헌터홀릭 로비 또는 던전 안에 있으면 입장 워프 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pPlayer->GetPos() ) )
		return RESULT_NOT_ACTABLE_IN_HUNTAHOLIC;

	// PK On 상태면 입장 불가
	if( pPlayer->IsPKOn() || pPlayer->IsPKOning() )
		return RESULT_PK_LIMIT;

	ArPosition posLobby( HuntaholicManager::Instance().GetLobbyPosition( nHuntaholicID ) );

	// 데이터에 없는 놈이라면 처리 불가
	if( !posLobby.GetX() || !posLobby.GetY() )
		return RESULT_NOT_EXIST;

	unsigned char nLayer = HuntaholicManager::Instance().GetProperLobbyLayer( nHuntaholicID, pPlayer->GetLevel() );

	if( nLayer == GameRule::HUNTAHOLIC_UNUSABLE_LOBBY_LAYER )
		return RESULT_NOT_ENOUGH_LEVEL;

	// 중독 방지 시간으로 인한 입장 제한 처리
	if( pPlayer->IsGameTimeLimited() && pPlayer->GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
		return RESULT_GAMETIME_LIMITED;

	return RESULT_SUCCESS;
}

void StructSkill::WARP_TO_HUNTAHOLIC_LOBBY()
{
	if( !m_pOwner->IsPlayer() )
		return;

	int nTargetHuntaholicID = GetTargetHuntaholicID();

	unsigned short nErrorCode = IsHuntaholicLobbyEnterableOwner( nTargetHuntaholicID );

	if( nErrorCode == RESULT_SUCCESS )
		nErrorCode = static_cast< StructPlayer * >( m_pOwner )->PendWarpToHuntaholicLobby( nTargetHuntaholicID );

	AddSkillResult( &m_vResultList, nErrorCode == RESULT_SUCCESS, nErrorCode );
}

// 타운 포탈
void StructSkill::TOWN_PORTAL()
{
	if( m_pOwner->IsPlayer() )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer* >( m_pOwner );
		ArPosition pos;
		pPlayer->GetLastTownPosition( &pos );

		// 귀환 지점 설정 전 유효성 검사
		AR_UNIT targetX = pos.x, targetY = pos.y;
		bool bValidPos = true;
		unsigned short try_cnt = 0;
		while( GameContent::IsBlocked( targetX, targetY ) )
		{
			targetX = XRandom( pos.x - 60, pos.x + 60 );
			targetY = XRandom( pos.y - 60, pos.y + 60 );

			if( ++try_cnt > 300 )
			{
				bValidPos = false;
				break;
			}
		}

		// 유효하지 않은 지점으로 이동시켜주느니 그냥 제자리로 이동시켜준다.
		if( !bValidPos )
		{
			_cprint( "Fail To Warp With Given Position [%d, %d], PlayerSID[%d] (TownPortal)\n", pos.x, pos.y, pPlayer->GetPlayerUID() );
			FILELOG( "Fail To Warp With Given Position [%d, %d], PlayerSID[%d] (TownPortal)", pos.x, pos.y, pPlayer->GetPlayerUID() );
			targetX = pPlayer->GetX();
			targetY = pPlayer->GetY();
		}

		unsigned char layer = 0;

		int current_channel = ChannelManager::GetChannelId( m_pOwner->GetX(), m_pOwner->GetY() );
		int target_channel = ChannelManager::GetChannelId( targetX, targetY );

		if( current_channel && current_channel == target_channel )
		{
			layer = pPlayer->GetLayer();
		}
		else if( target_channel )
		{
			layer = ChannelManager::GetProperLayer( targetX, targetY );
		}

		pPlayer->PendWarp( targetX, targetY, layer );
		pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );
		ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
	}
}

// 귀환의 깃털
void StructSkill::RETURN_FEATHER()
{
	if( m_pOwner->IsPlayer() )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer* >( m_pOwner );

		ArPosition curPos = pPlayer->GetCurrentPosition( GetArTime() );
		pPlayer->SetMove( curPos, 0 );

		StructItem *pItem = StructItem::FindItem( m_hTarget );
		if( !pItem )
			return;

		// 귀환/복귀의 깃털은 캐스팅 완료로 스킬 시전시 아이템이 소모됨.
		if( !pPlayer->EraseItem( pItem, 1 ) )
			return;

		// 복귀의 깃털에 세팅될 좌표를 세팅하고 해당 아이템을 지급
		StructItem *pBackItem = StructItem::AllocItem( 0, ItemBase::ITEM_CODE_RETURN_BACK_FEATHER, 1, ItemInstance::BY_ITEM, -1, -1, -1, curPos.x, curPos.y, pPlayer->GetLayer(), pPlayer->GetLocationId() );
		pPlayer->PushItem( pBackItem, pBackItem->GetCount() );

		// 여기부터는 일반 타운 포탈이랑 동일
		TOWN_PORTAL();

		// 테섭이면 귀환 깃털 써도 저장 안 함
		if( ENV().GetInt( "game.ServiceServer" ) )
			pPlayer->Save();
	}
}

// 복귀의 깃털
void StructSkill::RETURN_BACK_FEATHER()
{
	if( m_pOwner->IsPlayer() )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer* >( m_pOwner );

		ArPosition pos;
		unsigned char layer = 0;

		StructItem *pItem = StructItem::FindItem( m_hTarget );
		if( !pItem )
			return;

		pos.x = pItem->GetSocketCode( 0 );
		pos.y = pItem->GetSocketCode( 1 );
		layer = pItem->GetSocketCode( 2 );

		pPlayer->PendWarp( pos.x, pos.y, layer );
		pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );
		ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

		// 귀환/복귀의 깃털은 캐스팅 완료로 스킬 시전시 아이템이 소모됨.
		pPlayer->EraseItem( pItem, 1 );

		pPlayer->Save();
	}
}

void StructSkill::ADD_ENERGY()
{
	for( int nAddEnergyCount = 0; nAddEnergyCount < GetRequestedSkillLevel() && m_pOwner->GetEnergyCount() < m_pOwner->GetMaxEnergyCount(); ++nAddEnergyCount )
		m_pOwner->AddEnergy();

	AddSkillResult( &m_vResultList, true );
}

void StructSkill::CASTING_CANCEL_WITH_ADD_STATE( struct StructCreature* pTarget )
{
	int nCastingCancelRate = GetVar( 1 ) + GetVar( 2 ) * GetRequestedSkillLevel() + GetVar( 3 ) * GetEnhance();
	bool bSuccess = false;

	// 캐스팅 캔슬 성공~
	StructSkill *pCancelSkill = pTarget->GetCastSkill();
	if( pCancelSkill && pCancelSkill->m_Status == STATUS_CAST )
	{
		// 캔슬될 스킬의 타입을 구분, 물리/마법
		switch( static_cast< int >( GetVar( 0 ) ) )
		{
		case SkillBase::SC_PHYSICAL:	if( pCancelSkill->GetSkillBase()->IsPhysicalSkill() )	bSuccess = true; break;
		case SkillBase::SC_MAGICAL:		if( pCancelSkill->GetSkillBase()->IsMagicalSkill() )	bSuccess = true; break;
		case SkillBase::SC_EVERY:																bSuccess = true; break;
		default:																				bSuccess = false; break;
		}

		if( bSuccess && XRandom( 0, 99 ) < nCastingCancelRate ) 
		{
			pTarget->CancelSkill();

			if( XRandom( 0, 99 ) < GetVar(11) + GetVar(12) * GetRequestedSkillLevel() + GetVar(13) * GetEnhance() )
			{
				AR_TIME t = GetArTime();
				pTarget->AddState( static_cast< StructState::StateCode >( static_cast< int >( GetVar(4) ) ), m_pOwner->GetHandle(), GetVar(5) + GetVar(6) * GetRequestedSkillLevel() + GetVar(7) * GetEnhance(), t, t + GetVar(8) + GetVar(9) * GetEnhance() );
			}
		}
	}

	AddSkillResult( &m_vResultList, bSuccess, 0, pTarget->GetHandle() );
}

void StructSkill::RESPAWN_NEAR_MONSTER()
{
	// 몬스터 전용 스킬 유형임(respawn_near_monster 스크립트 함수 대용)
	if( !m_pOwner->IsMonster() )
		return;

	ArPosition pos = m_pOwner->GetCurrentPosition( GetArTime() );

	if( GameContent::IsBlocked( pos.GetX(), pos.GetY() ) )
		return;

	for( int nRespawnTypeIndex = 0 ; nRespawnTypeIndex < 3 ; ++nRespawnTypeIndex )
	{
		int nMonsterID = GetVar( nRespawnTypeIndex * 6 + 0 );
		int nRespawnCount = GetVar( nRespawnTypeIndex * 6 + 1 ) + GetVar( nRespawnTypeIndex * 6 + 2 ) * GetRequestedSkillLevel();
		bool bShareEnemyOnRespawn = GetVar( nRespawnTypeIndex * 6 + 3 );
		AR_HANDLE hInitialEnemy = ( bShareEnemyOnRespawn && m_pOwner->IsAttacking() ) ? m_pOwner->GetEnemyHandle() : 0;
		AR_TIME nLifeTime = GetVar( nRespawnTypeIndex * 6 + 4 ) + GetVar( nRespawnTypeIndex * 6 + 5 ) * GetRequestedSkillLevel();
		if( !nLifeTime )
			nLifeTime = INFINITE_TIME;

		// 헌터홀릭 던전 내부에서 몬스터가 스킬을 통해 몬스터를 소환한 경우에는 직접 리젠 처리하지 않고 HuntaholicManager에 몹 리젠 처리 요청
		int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( pos );
		int nInstanceDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( pos );

		if( nHuntaholicID && HuntaholicManager::Instance().IsHuntaholicDungeon( pos ) )
		{
			HuntaholicManager::Instance().PendRespawnMonster( nHuntaholicID, pos.x, pos.y, m_pOwner->GetLayer(), nMonsterID, nRespawnCount, nLifeTime, hInitialEnemy );
		}
		else if( nInstanceDungeonID )
		{
			InstanceDungeonManager::Instance().PendRespawnMonster( nInstanceDungeonID, pos.x, pos.y, m_pOwner->GetLayer(), nMonsterID, nRespawnCount, nLifeTime, hInitialEnemy );
		}
		else
		{
			for( int nProcCount = 0 ; nProcCount < nRespawnCount ; ++nProcCount )
			{
				// 락 문제로 일단 시전자 위치에 리젠시키고 바로 랜덤 범위 내 위치로 이동 시킴
				StructMonster * pMob = respawnMonster( pos.x, pos.y, m_pOwner->GetLayer(), nMonsterID, true, 0, NULL, false );
				// 리스폰 안 될 경우 체크 (대부분 MonsterID가 잘못 들어간 경우가 많다.)
				if( !pMob )
				{
					_cprint( "Failed to respawn a monster by monster skill(summoning): CasterMonsterID[%d] SummonedMonsterID[%d] SkillId[%d]\n", 
						static_cast< StructMonster *>( m_pOwner )->GetMonsterId(), nMonsterID, GetSkillId() );
					FILELOG( "Failed to respawn a monster by monster skill(summoning): CasterMonsterID[%d] SummonedMonsterID[%d] SkillId[%d]", 
						static_cast< StructMonster *>( m_pOwner )->GetMonsterId(), nMonsterID, GetSkillId() );
					break;
				}

				// 랜덤 이동 위치 설정
				AR_UNIT x, y;
				x = XRandom( pos.x - 60, pos.x + 60 );
				y = XRandom( pos.y - 60, pos.y + 60 );
				if( GameContent::CollisionToLine( pos.x, pos.y, x, y ) )
				{
					x = pos.x;
					y = pos.y;
				}

				ArPosition targetPos( x, y );
				pMob->SetMove( targetPos, pMob->GetRealMoveSpeed() * 3 );

				pMob->SetRespawnByScript();		// 선공 몹의 경우 바로 적 인식하도록 함
				ArcadiaServer::Instance().SetObjectPriority( pMob, ArObject::UPDATE_PRIORITY_HIGHEST );

				StructMonster * pOnwerMonster = static_cast< StructMonster * >( m_pOwner );
				if( pOnwerMonster->IsDungeonRaidMonster() )
				{
					int nDungeonID = DungeonManager::Instance().GetDungeonID( x, y );

					if( nDungeonID )
					{
						pMob->SetDungeonRaidMonster();
						pMob->CalculateStat();
						DungeonManager::Instance().AddToDungeonRaidMonster( pMob, nDungeonID, m_pOwner->GetLayer() );
					}
				}

				if( hInitialEnemy )
				{
					pMob->AddHate( hInitialEnemy, 1 );
				}

				// 시간 제한 몬스터 소환의 경우 시간 제한 설정 적용
				if( nLifeTime )
					pMob->SetLifeTime( nLifeTime );
			}
		}
	}

	AddSkillResult( &m_vResultList, true );
}

void StructSkill::RESPAWN_MONSTER()
{
	ArPosition pos = m_pOwner->GetCurrentPosition( GetArTime() );

	if( GameContent::IsBlocked( pos.GetX(), pos.GetY() ) )
		return;

	int nMonsterID = 0;

	switch( GetSkillBase()->GetEffectType() )
	{
	case SkillBase::EF_RESPAWN_MONSTER_RANDOMLY:
		nMonsterID = GameContent::GetRandomMonsterID( GetVar( 0 ) );
		break;
	case SkillBase::EF_RESPAWN_MONSTER_WITH_DIFF_CODE:
		if( GetRequestedSkillLevel() >= 1 && GetRequestedSkillLevel() <= 20 )
			nMonsterID = GetVar( GetRequestedSkillLevel() - 1 );
		break;
	}

	if( !nMonsterID )
		return;

	// 헌터홀릭 던전 내부에서 스킬을 통해 몬스터를 소환한 경우에는 직접 리젠 처리하지 않고 HuntaholicManager에 몹 리젠 처리 요청
	int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( pos );
	int nInstanceDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( pos );
	if( nHuntaholicID && HuntaholicManager::Instance().IsHuntaholicDungeon( pos ) )
	{
		HuntaholicManager::Instance().PendRespawnMonster( nHuntaholicID, pos.x, pos.y, m_pOwner->GetLayer(), nMonsterID, 1, INFINITE_TIME, 0 );
	}
	else if( nInstanceDungeonID )
	{
		InstanceDungeonManager::Instance().PendRespawnMonster( nInstanceDungeonID, pos.x, pos.y, m_pOwner->GetLayer(), nMonsterID, 1, INFINITE_TIME, 0 );
	}
	else
	{
		// 락 문제로 일단 시전자 위치에 리젠
		StructMonster * pMob = respawnMonster( pos.x, pos.y, m_pOwner->GetLayer(), nMonsterID, true, 0, NULL, false );

		if( !pMob ) return;

		pMob->SetRespawnByScript();		// 선공 몹의 경우 바로 적 인식하도록 함
		ArcadiaServer::Instance().SetObjectPriority( pMob, ArObject::UPDATE_PRIORITY_HIGHEST );
	}

	AddSkillResult( &m_vResultList, true );
}

// 크리쳐 테이밍
void StructSkill::CREATURE_TAMING( struct StructCreature* pTarget )
{
	// 타겟 없으면 KIN
	if( !pTarget ) return;

	// 몬스터에게만 걸린다~
	if( !pTarget->IsMonster() ) return;

	StructMonster *pMonster = static_cast< StructMonster * >( pTarget );

	// 플레이어만 쓸수 있는거야~!
	if( !m_pOwner->IsPlayer() ) return;

	if( m_hCastItem )
	{
		StructItem * pCastItem = StructItem::FindItem( m_hCastItem );
		if( !pCastItem || ( pCastItem->GetOwnerHandle() && pCastItem->GetOwnerHandle() != m_pOwner->GetHandle() ) || !pCastItem->IsInInventory() )
			return;

		if( !static_cast< StructPlayer * >( m_pOwner )->EraseItem( pCastItem, 1 ) )
			return;
	}

	bool bResult = SetTamer( pMonster, static_cast< StructPlayer* >( m_pOwner ), this );

	// 테이밍 성공~
	StructPlayer *pOwner = static_cast< StructPlayer * >( m_pOwner );
	if( bResult )
	{
		pMonster->AddHate( pOwner->GetHandle(), 1 );
	}

	AddSkillResult( &m_vResultList, bResult );
}

// 펫 테이밍
void StructSkill::PET_TAMING( struct StructCreature* pTarget )
{
	// 타겟 없으면 KIN
	if( !pTarget ) return;

	// 플레이어가 몬스터에게만 사용 가능
	if( !m_pOwner->IsPlayer() || !pTarget->IsMonster() )
		return;

	StructPlayer *pPlayer = static_cast< StructPlayer * >( m_pOwner );
	StructMonster *pMonster = static_cast< StructMonster * >( pTarget );

	// 펫으로 테이밍 가능 환경 몬스터에게만 가능
	ItemBase::ItemCode nTameItemCode = pMonster->GetTameItemCode();
	if( !pMonster->IsEnvironmentMonster() || ( !pMonster->GetTameCode() && !pMonster->IsMonsterCreatureTame() ) || !nTameItemCode
		|| StructItem::GetItemBase( nTameItemCode ).nGroup != ItemBase::GROUP_PET_CAGE )
		return;

	// 테이밍 가능 우리 보유 체크
	StructItem *pItem = pPlayer->FindEmptyPetCage( nTameItemCode );
	if( !pItem )
		return;

	// 테이밍 성공 확률 체크
	float fProbability = pMonster->GetTamePercetage() * 100;
	bool bResult = false;

	if( fProbability >= XRandom( 1, 100 ) )
	{
		// 테이밍 성공
		StructPet *pPet = NULL;

		if( pMonster->GetTameCode() )
		{
			pPet = AllocNewPet( pPlayer, pItem, pMonster->GetTameCode() );
		}

		if( !pPet )
		{
			bResult = false;
		}
		else
		{
			// 클라한테 추가 펫 정보 송신
			pPlayer->AddPet( pPet, false );

			// DB에 신규 펫 정보 추가, 펫 우리 아이템 업데이트
			pPet->DBQuery( new DB_InsertPet( pPet ) );
			pItem->DBQuery( new DB_UpdateItem( pItem ) );

			bResult = true;
		}
	}
	else
	{
		// 테이밍 실패(우리 삭제)
		pPlayer->EraseItem( pItem, 1 );
	}
	pPlayer->Save();

	AddSkillResult( &m_vResultList, bResult );
}

void StructSkill::SHOVELING()
{
	if( !m_pOwner->IsPet() )
		return;

	StructPet *pPet = static_cast< StructPet * >( m_pOwner );

	static_cast< StructPet * >( m_pOwner )->SetShovelingStatus( StructPet::SHOVELING_STATUS_IDLE );
	BroadcastStatusMessage( m_pOwner );

	if( !pPet->GetMaster() || !pPet->GetMaster()->IsLogin() || !pPet->GetMaster()->IsInWorld() )
		return;

	// 성공 여부 확률 체크
	int nProbability = XRandom( 1, 100 );
	bool bIsRarePet = pPet->IsRare();
	if( ( !bIsRarePet && nProbability > GameRule::PET_SHOVELING_SUCCESS_PERCENT_NORMAL ) ||
		( bIsRarePet && nProbability > GameRule::PET_SHOVELING_SUCCESS_PERCENT_RARE ) )
	{
		AddSkillResult( &m_vResultList, false );
		return;
	}

	int nRewardTypeSeed = XRandom( 1, 100 );
	GameRule::PET_SHOVELING_REWARD_TYPE nRewardType = GameRule::PET_SHOVELING_REWARD_UNKNOWN;
	for( int i = 0 ; i < GameRule::PET_SHOVELING_REWARD_TYPE_COUNT ; ++i )
	{
		if( nRewardTypeSeed <= GameRule::PET_SHOVELING_REWARD_TYPE_PERCENT[ i ] )
		{
			nRewardType = static_cast< GameRule::PET_SHOVELING_REWARD_TYPE >( i );
			break;
		}
	}

	if( nRewardType == GameRule::PET_SHOVELING_REWARD_UNKNOWN )
	{
		// 보상 확률 총 합이 100보다 적음(아무 보상도 선택되지 않았음)
		assert( 0 );

		AddSkillResult( &m_vResultList, false );
		return;
	}

	// SkillResult 송신용 임시 변수
	bool bSuccess = false;
	int nResultType = NULL;
	AR_HANDLE hTarget = NULL;

	switch( nRewardType )
	{
	case GameRule::PET_SHOVELING_REWARD_ITEM:
		{
			nResultType = SkillResult::CREATE_ITEM;

			int nLocationID = GameContent::GetLocationId( pPet->GetX(), pPet->GetY() );
			if( !nLocationID )
				break;

			ItemBase::ItemCode nCode = WorldLocationManager::Instance().GetShovelableItem( nLocationID );
			__int64 nCount = 1;

			while( nCode < 0 )
				GameContent::SelectItemIDFromDropGroup( nCode, nCode, nCount );

			if( !nCode )
				break;

			StructItem *pNewItem = StructItem::AllocItem( 0, nCode, nCount, ItemInstance::BY_SHOVELING );
			if( !pNewItem )
				break;

			if( !pPet->GetMaster()->PushItem( pNewItem, pNewItem->GetCount() ) )
			{
				StructItem::PendFreeItem( pNewItem );
				break;
			}

			bSuccess = true;
			hTarget = pNewItem->GetHandle();
		}
		break;
	case GameRule::PET_SHOVELING_REWARD_STATE:
		{
			nResultType = SkillResult::ADD_STATE;

			StructState::StateCode nStateCode = static_cast< StructState::StateCode >( GameRule::GetPetShovelingRewardStateCode() );

			AR_TIME t = GetArTime();
			// 보상 지속효과 부여에 실패 가능(타 지속효과와 중첩 그룹 문제 혹은 현재 더 강한 동종의 지속효과 보유 중)
			bSuccess = pPet->GetMaster()->AddState( nStateCode, NULL, 1, t, t + GameRule::PET_SHOVELING_REWARD_STATE_DURATION ) != RESULT_SUCCESS;

			hTarget = pPet->GetMaster()->GetHandle();
		}
		break;
	case GameRule::PET_SHOVELING_REWARD_MONSTER:
		{
			nResultType = SkillResult::RESPAWN_MONSTER;

			int nLocationID = GameContent::GetLocationId( pPet->GetX(), pPet->GetY() );
			if( !nLocationID )
				break;

			int nMonsterID = WorldLocationManager::Instance().GetShovelableMonster( nLocationID );

			// 레어 펫일 경우 테이밍 가능한 야생 펫 소환 확률 체크
			if( pPet->IsRare() && XRandom( 1, 100 ) <= GameRule::PET_SHOVELING_REWARD_MONSTER_PET_PERCENT )
			{
				nMonsterID = 50001;		// 테이밍 가능한 야생 펫의 결정 방식이 결정된 후에 변경할 것
			}

			if( !nMonsterID )
				break;

			// 몬스터 생성
			StructMonster *pMonster = StructMonster::AllocMonster( nMonsterID );
			if( !pMonster )
				break;

			pMonster->SetGenerateCode( StructMonster::BY_SHOVELING );
			pMonster->SetCurrentXY( pPet->GetX(), pPet->GetY() );

			AddMonsterToWorld( pMonster );

			bSuccess = true;
			hTarget = pMonster->GetHandle();
		}
		break;
	default:
		assert( 0 );
		break;
	}

	AddSkillResult( &m_vResultList, bSuccess, nResultType, hTarget );
}

ArPosition GetMovableKnockBackPosition( ArPosition & OriginalPos, ArPosition & TargetPos )
{
	if( GameContent::IsBlocked( TargetPos.x, TargetPos.y ) )
		return OriginalPos;

	return TargetPos;
}

bool StructSkill::AFFECT_RUSH_OLD( struct StructCreature * pTarget, float * pfRushDistance, ArPosition * pRushPos, float * pface )
{

	AR_TIME t = GetArTime();

	ArPosition original_pos = m_pOwner->GetCurrentPosition( t );

	int nDelay = original_pos.GetDistance( pTarget->GetCurrentPosition( t ) ) / ( (float) GameRule::DEFAULT_RUSH_SPEED / SPEED_UNIT );

	ArPosition target_pos = pTarget->GetCurrentPosition( t + nDelay );

	// TODO : 벡터 클래스 따로 하나 만든 다음, 벡터 연산은 그걸 통해서 하도록 한다.
	// 노말벡터 구한 다음 거리만큼 곱한다.
	float x, y, m, face;

	x = target_pos.x - original_pos.x;
	y = target_pos.y - original_pos.y;

	face = atan2( y, x );

	m = sqrt( x * x + y * y );

	if( m <= ( m_pOwner->GetUnitSize() + pTarget->GetUnitSize() ) / 2 )
	{
		return false;
	}

	x /= m;
	y /= m;

	m -= ( m_pOwner->GetUnitSize() + pTarget->GetUnitSize() ) / 2;

	ArPosition pos;

	pos.x = original_pos.x + x * m;
	pos.y = original_pos.y + y * m;

	if( GameContent::CollisionToLine( original_pos.x, original_pos.y, pos.x, pos.y ) )
		return false;

	m_RushPos = pos;
	m_fRushFace = face;

	*pfRushDistance = m;
	*pRushPos = pos;
	*pface = face;

	m_nFireTime += m / ( (float) GameRule::DEFAULT_RUSH_SPEED / SPEED_UNIT ) + 20;

	return true;
}

int StructSkill::AFFECT_KNOCK_BACK( struct StructCreature * pTarget, float fRange, AR_TIME knock_back_time )
{
	ArPosition caster_pos = m_pOwner->GetCurrentPosition( GetArTime() );
	ArPosition OriginalPos = pTarget->GetCurrentPosition( GetArTime() );

	// 노말벡터 구한 다음 거리만큼 곱한다.
	float x, y, m;

	x = OriginalPos.x - caster_pos.x;
	y = OriginalPos.y - caster_pos.y;

	m = sqrt( x * x + y * y );

	if( m > 0.0f )
	{
		x /= m;
		y /= m;
	}
	else
	{
		x = 1.0f;
		y = 0.0f;
	}

	ArPosition pos;

	pos.x = OriginalPos.x + x * fRange;
	pos.y = OriginalPos.y + y * fRange;

	AR_TIME next_movable_time = pTarget->GetMovableTime();

	if( pTarget->IsKnockbackable() )
	{
		next_movable_time = (std::max)( next_movable_time, GetArTime() + knock_back_time );

		ArPosition newPos = GetMovableKnockBackPosition( OriginalPos, pos );

		ArcadiaServer::Instance().MoveObject( pTarget, newPos, pTarget->GetFace() );
		pTarget->SetMovableTime( next_movable_time );

		// 타겟이 크리처 탑승 상태의 플레이어 혹은 플레이어를 태운 소환수였으면 낙상 처리
		StructPlayer * pTargetPlayer = NULL;

		if( pTarget->IsPlayer() )
			pTargetPlayer = static_cast< StructPlayer * >( pTarget );
		else if( pTarget->IsSummon() )
		{
			pTargetPlayer = static_cast< StructSummon * >( pTarget )->GetMaster();

			// 탑승 상태이긴 했는데 얻어맞은 소환수가 아닌 다른 놈을 탑승 중이었으면 낙상 처리 안 함
			if( pTargetPlayer && pTargetPlayer->GetRideObject() != pTarget )
				pTargetPlayer = NULL;
		}

		if( pTargetPlayer )
		{
			if( pTargetPlayer->IsRiding() || pTargetPlayer->HasRidingState() )
				pTargetPlayer->UnMount( StructPlayer::UNMOUNT_FALL, m_pOwner );
			else if( pTargetPlayer->IsSitDown() )
			{
				pTargetPlayer->StandUp();
				BroadcastStatusMessage( pTargetPlayer );

				AR_TIME t = GetArTime();
				pTargetPlayer->AddState( StructState::CARELESSNESS, 0, 1, t, t + 300 );
			}
		}
	}

	return next_movable_time;
}

void StructSkill::TOGGLE_AURA( struct StructCreature* pTarget )
{
	m_pOwner->ToggleAura( this );

	// 캐스터에게 보내는 SkillResult는 맨 아래에서 Add하고 함수를 빠져 나가고,
	// 타겟 조사 루프에서는 자신을 제외한 타겟들만 추가시킴

	switch( GetSkillBase()->GetSkillTargetType() )
	{
	case SkillBase::TARGET_PARTY: 
		{
			if( !m_pOwner->IsPlayer() ) break;

			StructPlayer* pPlayer = static_cast< StructPlayer* >( m_pOwner );

			// 파티 없으면 KIN
			if( pPlayer->GetPartyID() == 0 ) break;

			struct myPartyFunctor : public PartyManager::PartyFunctor
			{
				myPartyFunctor( StructSkill *_pSkill, StructCreature *_pOwner, std::vector< SkillResult > *_pvResultList )
					: pSkill( _pSkill ), pOwner( _pOwner ), pvResultList( _pvResultList ), t( GetArTime() )
				{
					if( pOwner )
					{
						posCaster.x = pOwner->GetX();
						posCaster.y = pOwner->GetY();
					}
				}

				virtual bool operator()( AR_HANDLE handle )
				{
					StructCreature *pPartyTarget = static_cast< StructCreature * >(StructCreature::raw_get( handle ));
					if( !pSkill || !pPartyTarget || !pPartyTarget->IsCreature() )
						return false;

					// 시전자는 아래에서 처리, 여기선 제외. 우선 성공으로 가정
					if( pPartyTarget == pOwner )
						return true;

					// 오오라 유효 범위 밖이면 버리기
					if( posCaster.GetDistance( pPartyTarget->GetCurrentPosition( t ) ) > pSkill->GetValidRange() )
						return false;

					AddSkillResult( pvResultList, true, 0, handle );

					return true;
				}

			private:
				StructSkill *pSkill;
				StructCreature *pOwner;
				ArPosition posCaster;
				std::vector< SkillResult > *pvResultList;
				AR_TIME t;
			} _fo( this, m_pOwner, &m_vResultList );

			PartyManager::GetInstance().DoEachMember( pPlayer->GetPartyID(), _fo );

			break;
		}
	}

	// 자기 자신에 대한 SkillResult 처리
	AddSkillResult( &m_vResultList, true, 0, m_pOwner->GetHandle() );

	return;
}

void StructSkill::MAKE_AREA_EFFECT_PROP( struct StructCreature* pTarget, bool bIsTrap )
{
	// 어디다 터트릴 것인지 위치를 얻는다.
	ArPosition pos = m_targetPos;

	if( pTarget )
	{
		pos = pTarget->GetCurrentPosition( GetArTime() + PREDICTION_AIMING_TIME );
	}

	if( pos.x < 0 || pos.y < 0 || pos.x > g_nMapWidth || pos.y > g_nMapHeight ) return;

	// _cprint( "SKILL PROP ADDED\n" );

	// 트랩은 1인당 동시 1개만 설치 가능(새로 설치하려면 기존꺼 제거)
	if( bIsTrap && m_pOwner->GetTrapHandle() )
	{
		StructSkillProp *pTrap = static_cast< StructSkillProp * >( GameObject::raw_get( m_pOwner->GetTrapHandle() ) );
		if( pTrap )
		{
			pTrap->PendRemove();
		}
	}

	int nMagicPoint = m_pOwner->GetMagicPoint( (Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	float fHateRatio = m_pOwner->GetHateRatio();
	// 스킬 프랍 생성
	ArObject *pPtr = StructSkillProp::Create( m_pOwner->GetHandle() , this, nMagicPoint, fHateRatio );

	if( bIsTrap && pPtr )
	{
		m_pOwner->SetTrapHandle( pPtr->GetHandle() );
	}

	pPtr->SetCurrentXY( pos.x, pos.y );
	pPtr->SetCurrentLayer( m_pOwner->GetLayer() );

	// 추가후 스케쥴러에 등록
	ArcadiaServer::Instance().AddObject( pPtr );
	ArcadiaServer::Instance().SetObjectPriority( pPtr, ArSchedulerObject::UPDATE_PRIORITY_HIGH );
	return;
}

bool StructSkill::PHYSICAL_DAMAGE_RUSH( struct StructCreature* pTarget, int *pnAdditionalDamage )
{
	SkillResult result;
	result.rush.type = SkillResult::RUSH;

	result.rush.hTarget = m_pOwner->GetHandle();

	ArPosition RushPos;

	float face;
	float fDistance;

	*pnAdditionalDamage = 0;

	if( !AFFECT_RUSH_OLD( pTarget, &fDistance, &RushPos, &face ) )
	{
		result.rush.bResult = false;

		m_vResultList.push_back( result );
		return false;
	}

	*pnAdditionalDamage = ( fDistance / GameRule::DEFAULT_UNIT_SIZE ) * GetVar(2);

	result.rush.bResult = true; 
	result.rush.x = RushPos.x;
	result.rush.y = RushPos.y;
	result.rush.speed = GameRule::DEFAULT_RUSH_SPEED;

	m_vResultList.push_back( result );

	return true;
}

void StructSkill::SKILL_RESURRECTION( struct StructCreature* pTarget )
{
	if( !pTarget || !pTarget->IsDead() ) return;

	// 내구도가 다한 크리처는 부활이 되지 않아야 한다.
	if( pTarget->IsSummon() )
	{
		StructItem *pCard = static_cast< StructSummon * >( pTarget )->GetParentCard();

		if( pCard )
		{
			if( pCard->GetItemEnhance() && !pCard->GetCurrentEtherealDurability() )
				return;
		}
	}

	int nIncHP = pTarget->GetMaxHP() * GetVar(0) * GetRequestedSkillLevel();
	int nIncMP = pTarget->GetMaxMP() * GetVar(1) * GetRequestedSkillLevel();
	__int64 nRecoveryEXP = 0;

	if( pTarget->IsPlayer() || pTarget->IsSummon() )
	{
		nRecoveryEXP = pTarget->GetLastDecreasedEXP() * ( GetVar(2) * GetRequestedSkillLevel() + GetVar(3) * GetEnhance() );

		pTarget->SetEXP( pTarget->GetEXP() + nRecoveryEXP );
	}

	pTarget->AddHP( nIncHP );
	pTarget->AddMP( nIncMP );

	pTarget->ClearRemovedStateByDead();

	if( pTarget->IsPlayer() )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer * >( pTarget );
		pPlayer->Save( true );

		if( pPlayer->IsCompeteDead() )
			pPlayer->SetCompeteDead( false );

		StructPlayer *pCaster = ( m_pOwner->IsPlayer() ) ? static_cast< StructPlayer * >( m_pOwner ) : NULL;
		LOG::Log11N4S( LM_CHARACTER_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(),
			CRT_SKILL, ( pCaster ) ? pCaster->GetAccountID() : 0, ( pCaster ) ? pCaster->GetPlayerUID() : 0, 0, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), nRecoveryEXP, pPlayer->GetEXP(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, ( pCaster ) ? pCaster->GetAccountName() : "", LOG::STR_NTS, ( pCaster ) ? pCaster->GetName() : "" , LOG::STR_NTS );
	}
	else if( pTarget->IsSummon() )
	{
		StructSummon * pSummon = static_cast< StructSummon * >( pTarget );
		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );

		StructPlayer *pPlayer = static_cast< StructPlayer * >(pSummon->GetMaster());
		LOG::Log11N4S( LM_SUMMON_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(), pSummon->GetSID(), 0, 0, 0, 0, 0, (nRecoveryEXP > 0), nRecoveryEXP, pSummon->GetEXP(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
	}

	SkillResult result;

	result.rebirth.type = SkillResult::REBIRTH;
	result.rebirth.nIncHP = nIncHP;
	result.rebirth.nIncMP = nIncMP;
	result.rebirth.nRecoveryEXP = nRecoveryEXP;
	result.rebirth.target_hp = pTarget->GetHP();
	result.rebirth.target_mp = pTarget->GetMP();
	result.rebirth.hTarget = pTarget->GetHandle();

	m_vResultList.push_back( result );
}

void StructSkill::SKILL_RESURRECTION_WITH_RECOVER( struct StructCreature* pTarget )
{
	if( !pTarget || !pTarget->IsDead() ) return;

	// 내구도가 다한 크리처는 부활이 되지 않아야 한다.
	if( pTarget->IsSummon() )
	{
		StructItem *pCard = static_cast< StructSummon * >( pTarget )->GetParentCard();

		if( pCard )
		{
			if( pCard->GetItemEnhance() && !pCard->GetCurrentEtherealDurability() )
				return;
		}
	}

	int nIncHP = pTarget->GetMaxHP() * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(9) * GetEnhance() );
	int nIncMP = pTarget->GetMaxMP() * ( GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance() );
	__int64 nRecoveryEXP = 0;

	if( pTarget->IsPlayer() || pTarget->IsSummon() )
	{
		nRecoveryEXP = pTarget->GetLastDecreasedEXP() * ( GetVar(4) + GetVar(5) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance() );

		pTarget->SetEXP( pTarget->GetEXP() + nRecoveryEXP );
	}

	pTarget->AddHP( nIncHP );
	pTarget->AddMP( nIncMP );

	pTarget->ClearRemovedStateByDead();

	if( pTarget->IsPlayer() )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer * >( pTarget );
		pPlayer->Save( true );

		if( pPlayer->IsCompeteDead() )
			pPlayer->SetCompeteDead( false );

		StructPlayer *pCaster = ( m_pOwner->IsPlayer() ) ? static_cast< StructPlayer * >( m_pOwner ) : NULL;
		LOG::Log11N4S( LM_CHARACTER_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(),
			CRT_SKILL_WITH_RECOVER, ( pCaster ) ? pCaster->GetAccountID() : 0, ( pCaster ) ? pCaster->GetPlayerUID() : 0, 0, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), nRecoveryEXP, pPlayer->GetEXP(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, ( pCaster ) ? pCaster->GetAccountName() : "", LOG::STR_NTS, ( pCaster ) ? pCaster->GetName() : "" , LOG::STR_NTS );
	}
	else if( pTarget->IsSummon() )
	{
		StructSummon * pSummon = static_cast< StructSummon * >( pTarget );
		pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );

		StructPlayer *pPlayer = static_cast< StructPlayer * >(pSummon->GetMaster());
		LOG::Log11N4S( LM_SUMMON_RESURRECTION, pPlayer->GetAccountID(), pPlayer->GetSID(), pSummon->GetSID(), 0, 0, 0, 0, 0, (nRecoveryEXP > 0), nRecoveryEXP, pSummon->GetEXP(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
	}

	SkillResult result;

	result.rebirth.type = SkillResult::REBIRTH;
	result.rebirth.nIncHP = nIncHP;
	result.rebirth.nIncMP = nIncMP;
	result.rebirth.nRecoveryEXP = nRecoveryEXP;
	result.rebirth.target_hp = pTarget->GetHP();
	result.rebirth.target_mp = pTarget->GetMP();
	result.rebirth.hTarget = pTarget->GetHandle();

	m_vResultList.push_back( result );
}

void StructSkill::SKILL_LOTTO()
{
	StructPlayer *pPlayer = NULL;

	if( m_pOwner->IsPlayer() )
		pPlayer = static_cast< StructPlayer * >(m_pOwner);
	else if( m_pOwner->IsSummon() )
		pPlayer = static_cast< StructSummon * >(m_pOwner)->GetMaster();

	// 한 번 더 소지금 체크
	StructGold nSkillCost = GetVar( 0 ) + GetVar( 1 ) * GetRequestedSkillLevel();
	if( !pPlayer || pPlayer->GetGold() < nSkillCost )
		return;

	int nCum = 0;
	int nKey = XRandom( 1, 10000 );
	float rate( -1 );

	for( int i=0; i<5; ++i )
	{
		nCum += ( GetVar( i*3+2 ) + GetVar( i*3+3 ) * GetRequestedSkillLevel() ) * 100;

		if( nKey > nCum ) continue;

		rate += GetVar( i*3+4 );

		break;
	}

	StructGold nGold( rate * nSkillCost.GetRawData() );
	StructGold nPrevGold = pPlayer->GetGold();

	if( pPlayer->GetGold() + nGold > GameRule::MAX_GOLD_FOR_INVENTORY )
		pPlayer->ChangeGold( GameRule::MAX_GOLD_FOR_INVENTORY );
	else if( pPlayer->GetGold() + nGold < 0 )
		pPlayer->ChangeGold( StructGold( 0 ) );
	else
		pPlayer->ChangeGold( pPlayer->GetGold() + nGold );

	LOG::Log11N4S( LM_SKILL_LOTTO, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(), ( m_pOwner->IsPlayer() ) ? 1 : 0, m_pOwner->GetSID(), GetSkillId(), GetRequestedSkillLevel(), GetEnhance(),
		nPrevGold.GetRawData(), nSkillCost.GetRawData(), ( nGold + nSkillCost ).GetRawData(), pPlayer->GetGold().GetRawData(),
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, m_pOwner->GetName(), LOG::STR_NTS, "", 0 );
}

void StructSkill::SKILL_ADD_REGION_HP_MP( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nIncHP = 0;
	int nIncMP = 0;

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	float fEffectLength = GetVar( 10 ) * GameRule::DEFAULT_UNIT_SIZE;

	int nTargetType = GetVar( 11 );

	std::vector< AR_HANDLE > vResult;
	m_fRange = fEffectLength;

	m_nTargetCount = 0;

	AR_TIME t = GetArTime();

	ArcadiaServer::Instance().EnumMovableObject( pTarget->GetCurrentPosition( t ), pTarget->GetLayer(), fEffectLength, &vResult );

	for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
	{
		StructCreature::iterator itCreature = StructCreature::get( (*it) );

		StructCreature * pCreature = (*itCreature);

		if( !pCreature || pCreature->IsPet() || pCreature->IsDead() )
			continue;

		if( nTargetType == 1 )
		{
			if( m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}
		else if( nTargetType == 2 )
		{
			if( !m_pOwner->IsAlly( pCreature ) )
				continue;
		}
		else if( nTargetType == 3 )
		{
			if( !m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}

		switch( static_cast< int >( GetVar(12) ) )
		{
		case 1:
			if( !pCreature->IsPlayer() )
				continue;
			break;
		case 2:
			if( !pCreature->IsSummon() )
				continue;
			break;
		case 3:
			if( !pCreature->IsPlayer() && !pCreature->IsSummon() )
				continue;
			break;
		}

		nIncHP = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() )
			+ GetVar(2) * GetRequestedSkillLevel()
			+ pCreature->GetMaxHP() * GetVar(3) * GetRequestedSkillLevel()
			+ GetVar(4) * GetEnhance();

		nIncMP = nMagicPoint * ( GetVar(5) + GetVar(6) * GetRequestedSkillLevel() )
			+ GetVar(7) * GetRequestedSkillLevel()
			+ pCreature->GetMaxMP() * GetVar(8) * GetRequestedSkillLevel()
			+ GetVar(9) * GetEnhance();

		nIncHP = pCreature->Heal( nIncHP );
		nIncMP = pCreature->MPHeal( nIncMP );

		SkillResult skill_result;

		skill_result.add_hp_mp_sp.type = SkillResult::ADD_HP_MP_SP;
		skill_result.add_hp_mp_sp.hTarget = pCreature->GetHandle();
		skill_result.add_hp_mp_sp.target_hp = pCreature->GetHP();
		skill_result.add_hp_mp_sp.target_mp = pCreature->GetMP();
		skill_result.add_hp_mp_sp.nIncHP = nIncHP;
		skill_result.add_hp_mp_sp.nIncMP = nIncMP;
		skill_result.add_hp_mp_sp.nIncSP = 0;

		m_vResultList.push_back( skill_result );

		++m_nTargetCount;
	}
}

void StructSkill::SKILL_ADD_REGION_HP( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nIncHP = 0;

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	float fEffectLength = GetVar( 10 ) * GameRule::DEFAULT_UNIT_SIZE;

	int nTargetType = GetVar( 11 );

	std::vector< AR_HANDLE > vResult;
	m_fRange = fEffectLength;

	AR_TIME t = GetArTime();

	ArcadiaServer::Instance().EnumMovableObject( pTarget->GetCurrentPosition( t ), pTarget->GetLayer(), fEffectLength, &vResult );

	for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
	{
		StructCreature::iterator itCreature = StructCreature::get( (*it) );

		StructCreature * pCreature = (*itCreature);

		if( !pCreature || pCreature->IsPet() || pCreature->IsDead() )
			continue;

		if( nTargetType == 1 )
		{
			if( m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}
		else if( nTargetType == 2 )
		{
			if( !m_pOwner->IsAlly( pCreature ) )
				continue;
		}
		else if( nTargetType == 3 )
		{
			if( !m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}

		nIncHP = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() )
			+ GetVar(2) + GetVar(3) * GetRequestedSkillLevel()
			+ pCreature->GetMaxHP() * ( GetVar(4) + GetVar(5) * GetRequestedSkillLevel() + GetVar(7) * GetEnhance() )
			+ GetVar(6) * GetEnhance();

		nIncHP = pCreature->Heal( nIncHP );

		SkillResult skill_result;

		skill_result.add_hp.type = SkillResult::ADD_HP;
		skill_result.add_hp.hTarget = pCreature->GetHandle();
		skill_result.add_hp.target_hp = pCreature->GetHP();
		skill_result.add_hp_mp_sp.nIncHP = nIncHP;

		m_vResultList.push_back( skill_result );
	}
}

void StructSkill::SKILL_ADD_REGION_MP( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nIncMP = 0;

	int nMagicPoint = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	float fEffectLength = GetVar( 10 ) * GameRule::DEFAULT_UNIT_SIZE;

	int nTargetType = GetVar( 11 );

	std::vector< AR_HANDLE > vResult;
	m_fRange = fEffectLength;

	AR_TIME t = GetArTime();

	ArcadiaServer::Instance().EnumMovableObject( pTarget->GetCurrentPosition( t ), pTarget->GetLayer(), fEffectLength, &vResult );

	m_nTargetCount = 0;

	for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
	{
		StructCreature::iterator itCreature = StructCreature::get( (*it) );

		StructCreature * pCreature = (*itCreature);

		if( !pCreature || pCreature->IsPet() || pCreature->IsDead() )
			continue;

		if( nTargetType == 1 )
		{
			if( m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}
		else if( nTargetType == 2 )
		{
			if( !m_pOwner->IsAlly( pCreature ) )
				continue;
		}
		else if( nTargetType == 3 )
		{
			if( !m_pOwner->IsEnemy( pCreature, true ) )
				continue;
		}

		nIncMP = nMagicPoint * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() )
			+ GetVar(2) + GetVar(3) * GetRequestedSkillLevel()
			+ pCreature->GetMaxMP() * ( GetVar(4) + GetVar(5) * GetRequestedSkillLevel() + GetVar(7) * GetEnhance() )
			+ GetVar(6) * GetEnhance();

		nIncMP = pCreature->MPHeal( nIncMP );

		SkillResult skill_result;

		skill_result.add_hp.type = SkillResult::ADD_MP;
		skill_result.add_hp.hTarget = pCreature->GetHandle();
		skill_result.add_hp.target_hp = pCreature->GetMP();
		skill_result.add_hp_mp_sp.nIncHP = nIncMP;

		m_vResultList.push_back( skill_result );

		++m_nTargetCount;
	}
}

void StructSkill::SKILL_ADD_HP_MP( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nDecHP = 0;
	int nDecMP = 0;

	if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_HP_MP_BY_SUMMON_DEAD )
	{
		if( !m_pOwner->IsPlayer() ) return;

		StructSummon * pSummon = NULL;

		pSummon = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon();

		if( !pSummon || pSummon->IsDead() || pSummon == pTarget )
		{
			AddSkillResult( &m_vResultList, false, SkillResult::SUMMON_DEAD );

			return;
		}

		nDecHP = pSummon->GetHP();

		pSummon->damage( m_pOwner, nDecHP, false );

		ArcadiaServer::Instance().SetObjectPriority( pSummon, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

		StructCreature::_DAMAGE_INFO Damage;
		Damage.nDamage = nDecHP;
		Damage.target_hp = pSummon->GetHP();

		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, Elemental::TYPE_NONE, Damage, pSummon->GetHandle() );
	}
	else if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_HP_MP_BY_SUMMON_DAMAGE )
	{
		if( !m_pOwner->IsPlayer() ) return;

		StructSummon * pSummon = NULL;

		pSummon = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon();

		if( !pSummon || pSummon->IsDead() )
		{
			AddSkillResult( &m_vResultList, false, SkillResult::SUMMON_DEAD );

			return;
		}

		nDecHP = pSummon->GetMaxHP() * GetVar(10);

		pSummon->AddHP( 0 - nDecHP );

		SkillResult skill_result;

		skill_result.add_hp.type = SkillResult::ADD_HP;
		skill_result.add_hp.hTarget = pSummon->GetHandle();
		skill_result.add_hp.target_hp = pSummon->GetMP();
		skill_result.add_hp.nIncHP = 0 - nDecHP;

		m_vResultList.push_back( skill_result );
	}
	else if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_HP_MP_BY_STEAL_SUMMON_HP_MP )
	{
		if( !m_pOwner->IsPlayer() ) return;

		StructSummon * pSummon = NULL;

		nDecHP = GetVar( 0 ) + GetVar( 1 ) * GetRequestedSkillLevel() + GetVar( 4 ) * GetEnhance();
		nDecMP = GetVar( 2 ) + GetVar( 3 ) * GetRequestedSkillLevel() + GetVar( 5 ) * GetEnhance();

		pSummon = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon();

		if( !pSummon || pSummon->IsDead() )
		{
			AddSkillResult( &m_vResultList, false, SkillResult::SUMMON_DEAD );

			return;
		}

		pSummon->AddHP( 0 - nDecHP );
		pSummon->AddMP( 0 - nDecMP );


		SkillResult skill_result;

		skill_result.add_hp_mp_sp.type = SkillResult::ADD_HP_MP_SP;
		skill_result.add_hp_mp_sp.hTarget = pSummon->GetHandle();
		skill_result.add_hp_mp_sp.target_hp = pSummon->GetHP();
		skill_result.add_hp_mp_sp.target_mp = pSummon->GetMP();
		skill_result.add_hp_mp_sp.nIncHP = 0 - nDecHP;
		skill_result.add_hp_mp_sp.nIncMP = 0 - nDecMP;
		skill_result.add_hp_mp_sp.nIncSP = 0;

		m_vResultList.push_back( skill_result );
	}

	int nIncHP = 0;
	int nIncMP = 0;

	if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_HP_MP_BY_STEAL_SUMMON_HP_MP )
	{
		nIncHP = nDecHP * GetVar( 6 ) + GetVar( 7 ) * GetEnhance();
		nIncMP = nDecMP * GetVar( 6 ) + GetVar( 7 ) * GetEnhance();
	}
	else if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_HP_MP_WITH_LIMIT_PERCENT )
	{
		int nHealLimitHP = ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance() ) * pTarget->GetMaxHP();
		int nHealLimitMP = ( GetVar(3) + GetVar(4) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance() ) * pTarget->GetMaxMP();

		if( nHealLimitHP > pTarget->GetHP() )
			nIncHP = nHealLimitHP - pTarget->GetHP();

		if( nHealLimitMP > pTarget->GetMP() )
			nIncMP = nHealLimitMP - pTarget->GetMP();
	}
	else
	{
		nIncHP = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) * ( GetVar(0) + GetVar(1) * GetRequestedSkillLevel() )
			+ GetVar(2) * GetRequestedSkillLevel()
			+ pTarget->GetMaxHP() * GetVar(3) * GetRequestedSkillLevel()
			+ GetVar(4) * GetEnhance();

		nIncMP = m_pOwner->GetMagicPoint((Elemental::Type)GetSkillBase()->GetElementalType(), GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() ) * ( GetVar(5) + GetVar(6) * GetRequestedSkillLevel() )
			+ GetVar(7) * GetRequestedSkillLevel()
			+ pTarget->GetMaxMP() * GetVar(8) * GetRequestedSkillLevel()
			+ GetVar(9) * GetEnhance();

		if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_HP_MP ||
			GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_HP_MP_BY_SUMMON_DEAD )
		{
			nIncHP += GetVar(10);
			nIncMP += GetVar(11);
		}

		if( GetSkillBase()->GetEffectType() == SkillBase::EF_ADD_HP_MP_BY_SUMMON_DEAD )
		{
			nIncHP += pTarget->GetMaxHP() * GetVar(12);
			nIncMP += pTarget->GetMaxMP() * GetVar(13);
		}
	}

	// 아래로 진입하는 경우는 확인하진 못했지만, 만에 하나라도 의도된 동작은 아니므로 사망 여부 체크하도록 수정
	if( !pTarget->IsDead() )
	{
		nIncHP = pTarget->Heal( nIncHP );
		nIncMP = pTarget->MPHeal( nIncMP );

		SkillResult skill_result;

		skill_result.add_hp_mp_sp.type = SkillResult::ADD_HP_MP_SP;
		skill_result.add_hp_mp_sp.hTarget = pTarget->GetHandle();
		skill_result.add_hp_mp_sp.target_hp = pTarget->GetHP();
		skill_result.add_hp_mp_sp.target_mp = pTarget->GetMP();
		skill_result.add_hp_mp_sp.nIncHP = nIncHP;
		skill_result.add_hp_mp_sp.nIncMP = nIncMP;
		skill_result.add_hp_mp_sp.nIncSP = 0;

		m_vResultList.push_back( skill_result );
	}
}

void StructSkill::Summon()
{
	// 소환은....스킬이 아니다.ㅜ.ㅡ
	StructItem *pItem = StructItem::FindItem( m_hTarget );

	// 카드가 없거나 카드의 내구도가 다 함
	if( !pItem || !pItem->IsSummonCard() || ( pItem->GetItemEnhance() && !pItem->GetCurrentEtherealDurability() ) ) 
		return;

	StructSummon *pSummon = pItem->GetSummonStruct();

	// 소환수 없으면 KIN
	if( !pSummon || pSummon->GetMaster() != m_pOwner )
		return;

	if( !pSummon->IsInWorld() )
	{
		static_cast< StructPlayer * >( m_pOwner )->Summon( pSummon, m_targetPos );
	}
}

void StructSkill::UnSummonAndAddState()
{
	if( !m_pOwner->IsPlayer() )
		return;

	StructSummon * pMainSummon = static_cast< StructPlayer * >( m_pOwner )->GetMainSummon();

	if( !pMainSummon )
		return;

	int nMainSummonCode = pMainSummon->GetSummonCode();
	std::string szMainSummonName;
	XStringUtil::Format( szMainSummonName, "%s|%d", pMainSummon->GetName(), pMainSummon->GetEnhance() );

	StructSummon * pSubSummon = static_cast< StructPlayer * >( m_pOwner )->GetSubSummon();

	if( !GetVar( 6 ) )
	{
		if( pSubSummon )
		{
			static_cast< StructPlayer * >( m_pOwner )->PendUnSummon( pSubSummon );
		}

		static_cast< StructPlayer * >( m_pOwner )->PendUnSummon( pMainSummon );
		ArcadiaServer::Instance().SetObjectPriority( m_pOwner, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
	}

	AR_TIME t = GetArTime();

	AR_TIME end_time = 0;

	int nEndTime = GetStateSecond( GetRequestedSkillLevel(), GetEnhance() );

	if( nEndTime < 0 ) end_time = t + 6000;
	else end_time = t + nEndTime;

	for( int i = 0; i < 6; ++i )
	{
		if( !GetVar( i ) )
			break;

		const StateInfo * pStateInfo = GameContent::GetStateInfo( GetVar( i ) );

		if( pStateInfo && pStateInfo->effect_type == StructState::EF_CHANGING_FORM )
		{
			m_pOwner->AddState( (StructState::StateCode) (int) GetVar(i), 
				m_pOwner->GetHandle(), 
				GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
				t, end_time, false, nMainSummonCode, szMainSummonName.c_str() );
		}
		else
		{
			m_pOwner->AddState( (StructState::StateCode) (int) GetVar(i), 
				m_pOwner->GetHandle(), 
				GetStateLevel( GetRequestedSkillLevel(), GetEnhance() ), 
				t, end_time, false );
		}
	}
}

void StructSkill::UnSummon()
{
	if( !m_pOwner->IsPlayer() )
		return;

	// 소환은....스킬이 아니다.ㅜ.ㅡ
	StructItem *pItem = StructItem::FindItem( m_hTarget );

	// 카드 없음
	if( !pItem || !pItem->IsSummonCard() ) 
		return;

	StructSummon *pSummon = pItem->GetSummonStruct();

	// 소환수 없으면 KIN
	if( !pSummon || pSummon->GetMaster() != m_pOwner )
		return;

	static_cast< StructPlayer * >( m_pOwner )->PendUnSummon( pSummon );
	ArcadiaServer::Instance().SetObjectPriority( m_pOwner, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
}

bool StructSkill::AFFECT_RUSH( struct StructCreature * pTarget, float * pfRushDistance, ArPosition * pRushPos, float * pface, float fSpeed )
{

	AR_TIME t = GetArTime();

	ArPosition original_pos = m_pOwner->GetCurrentPosition( t );

	int nDelay = original_pos.GetDistance( pTarget->GetCurrentPosition( t ) ) / ( fSpeed / SPEED_UNIT );

	ArPosition target_pos = pTarget->GetCurrentPosition( t + nDelay );

	// TODO : 벡터 클래스 따로 하나 만든 다음, 벡터 연산은 그걸 통해서 하도록 한다.
	// 노말벡터 구한 다음 거리만큼 곱한다.
	float x, y, m, face;

	x = target_pos.x - original_pos.x;
	y = target_pos.y - original_pos.y;

	face = atan2( y, x );

	m = sqrt( x * x + y * y );

	if( m <= ( m_pOwner->GetUnitSize() + pTarget->GetUnitSize() ) / 2 )
	{
		return false;
	}

	x /= m;
	y /= m;

	m -= ( m_pOwner->GetUnitSize() + pTarget->GetUnitSize() ) / 2;

	ArPosition pos;

	pos.x = original_pos.x + x * m;
	pos.y = original_pos.y + y * m;

	if( GameContent::CollisionToLine( original_pos.x, original_pos.y, pos.x, pos.y ) )
		return false;

	m_RushPos = pos;
	m_fRushFace = face;

	*pfRushDistance = m;
	*pRushPos = pos;
	*pface = face;

	m_nFireTime += m / ( fSpeed / SPEED_UNIT ) + 20;

	return true;
}

void StructSkill::PHYSICAL_SINGLE_DAMAGE( struct StructCreature * pTarget )
{
	if( !pTarget ) return;

	// 돌진 처리
	if( GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH ||
		GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK )
	{
		if( m_nCurrentFire == 0 )
		{
			m_nCurrentFire = 1;
			m_nTotalFire = 2;

			if( !RUSH( pTarget, GetVar(5) ) )
			{
				m_nCurrentFire = 2;
			}

			return;
		}
		else
		{
			ArcadiaServer::Instance().MoveObject( m_pOwner, m_RushPos, m_fRushFace );
			++m_nCurrentFire;
		}
	}

	// 데미지 처리
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	switch( GetSkillBase()->GetEffectType() )
	{
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE:
		nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
		nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();
		break;
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH:
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK:
		nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel();
		nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel();
		break;
	case SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_KNOCKBACK:
		nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance();
		nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();
		break;
	default:
		// 어느 유형인데 이쪽으로 오셨는가...?
		assert( 0 );
		return;
	}

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	// 넉백 처리
	if( !Damage.bBlock && !Damage.bMiss && !Damage.bPerfectBlock
		&& ( GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_KNOCKBACK
		|| GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_DAMAGE_RUSH_KNOCKBACK )
		&& !( pTarget->IsMonster() && static_cast< StructMonster * >( pTarget )->IsBossMonster() ) )
	{
		float fRange = GetVar(8) + GetVar(9) * GetRequestedSkillLevel();
		fRange *= GameRule::DEFAULT_UNIT_SIZE;

		AR_TIME knock_back_time = ( GetVar(10) + GetVar(11) * GetRequestedSkillLevel() ) * 100;

		AFFECT_KNOCK_BACK( pTarget, fRange, knock_back_time );
		AddSkillDamageWithKnockBackResult( &m_vResultList, SkillResult::DAMAGE_WITH_KNOCK_BACK, elemental_type, Damage, pTarget->GetHandle(), pTarget->GetPos().x, pTarget->GetPos().y, knock_back_time );
	}
	else
		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}

void StructSkill::PHYSICAL_SINGLE_DAMAGE_ABSORB( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );			// 기본적으로, 모든 스킬은 오늘손 장비의 영향을 받는다. 이도류 스킬은 제외.

	// 데미지
	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
	nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	// 흡수
	int nAddHP = Damage.nDamage * GetVar(4);
	int nAddMP = Damage.nDamage * GetVar(5);
	m_pOwner->AddHP( nAddHP );
	m_pOwner->AddMP( nAddMP );

	SkillResult skill_result;

	skill_result.add_hp.type = SkillResult::ADD_HP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nAddHP;
	skill_result.add_hp.target_hp = m_pOwner->GetHP();

	m_vResultList.push_back( skill_result );

	skill_result.add_hp.type = SkillResult::ADD_MP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = nAddMP;
	skill_result.add_hp.target_hp = m_pOwner->GetMP();

	m_vResultList.push_back( skill_result );

	return;
}

void StructSkill::PHYSICAL_SINGLE_DAMAGE_ADD_ENERGY( struct StructCreature * pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	// 데미지
	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
	nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	int nEnergyCount = GetVar(4) + GetVar(5) * GetRequestedSkillLevel();

	for( int i = 0 ; i < nEnergyCount ; ++i )
		m_pOwner->AddEnergy();

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}

bool StructSkill::RUSH( struct StructCreature* pTarget, float fSpeed )
{
	if( !pTarget ) return false;

	SkillResult result;
	result.rush.type = SkillResult::RUSH;
	result.rush.hTarget = m_pOwner->GetHandle();

	ArPosition RushPos;

	float face;
	float fDistance;

	if( !AFFECT_RUSH( pTarget, &fDistance, &RushPos, &face, fSpeed ) )
	{
		result.rush.bResult = false;

		m_vResultList.push_back( result );
		return false;
	}

	result.rush.bResult = true; 
	result.rush.x = RushPos.x;
	result.rush.y = RushPos.y;
	result.rush.speed = fSpeed;

	m_vResultList.push_back( result );

	return true;
}

void StructSkill::PHYSICAL_SINGLE_REGION_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// 데미지 계산 + 유형별 파라미터 설정
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	// 특수 범위 관련
	bool	bTargetOrigin		= true;		// 기본 타겟 중심
	int		nRegionType			= -1;		// 기본 원형
	float	fRegionProperty		= 0.0f;		// 기본 범위 유형 속성값 없음

	// 넉백 관련
	float	fKnockbackRange		= 0.0f;		// 넉백 거리
	AR_TIME	nKnockbackTime		= 0;		// 넉백에 의한 행동 불가 시간

	// 캐스팅 캔슬 관련
	int		nCastingCancelRate	= 0;		// 캐스팅 캔슬 확률(100 = 100%)

	switch( GetSkillBase()->GetEffectType() )
	{
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE:
		nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
		nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();
		break;
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_SELF:
		bTargetOrigin = false;		// break 쓰면 데미지 증폭/증가 안되요;;
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK:
		nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel();
		nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel();
		// 이 유형들은 특수범위 지정이 가능
		nRegionType = static_cast< int >( GetVar(7) );
		fRegionProperty = GetVar(8);
		// 넉백 속성 설정
		fKnockbackRange = ( GetVar(9) + GetVar(10) * GetRequestedSkillLevel() ) * GameRule::DEFAULT_UNIT_SIZE;
		nKnockbackTime = ( GetVar(11) + GetVar(12) * GetRequestedSkillLevel() ) * 100;
		break;
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_WITH_CAST_CANCEL:
		nCastingCancelRate = GetVar(12) + GetVar(13) * GetRequestedSkillLevel() + GetVar(14) * GetEnhance();
		break;
	case SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_ADDING_MAGICAL_DAMAGE:
		{
			nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
			nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();
			int nMagicalDamage = m_pOwner->GetMagicPoint( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
			nMagicalDamage *= GetVar(12) + GetVar(13) * GetRequestedSkillLevel() + GetVar(14) * GetEnhance();
			nDamage += nMagicalDamage;
			nDamage += GetVar(15) + GetVar(16) * GetRequestedSkillLevel() + GetVar(17) * GetEnhance();
			break;
		}
	default:
		// 어느 유형인데 이쪽으로 오셨는가...?
		assert( 0 );
		return;
	}

	float fEffectLength = GetVar(4) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;
	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), bTargetOrigin, fEffectLength, nRegionType, fRegionProperty, nDamage, true, m_pOwner, GetVar(5), GetVar(6), vTargetList );

	m_nTargetCount = static_cast<int>( vTargetList.size() );

	// 데미지 적용
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		if( !Damage.bBlock && !Damage.bMiss && !Damage.bPerfectBlock )
		{
			if( ( GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK
				|| GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_KNOCKBACK_SELF )
				&& !( pDealTarget->IsMonster() && static_cast< StructMonster * >( pDealTarget )->IsBossMonster() ) )
			{
				// 넉백 적용
				AFFECT_KNOCK_BACK( pDealTarget, fKnockbackRange, nKnockbackTime );
				AddSkillDamageWithKnockBackResult( &m_vResultList, SkillResult::DAMAGE_WITH_KNOCK_BACK, elemental_type, Damage, pDealTarget->GetHandle(), pDealTarget->GetPos().x, pDealTarget->GetPos().y, nKnockbackTime );
			}
			else if( GetSkillBase()->GetEffectType() == SkillBase::EF_PHYSICAL_SINGLE_REGION_DAMAGE_WITH_CAST_CANCEL )
			{
				StructSkill *pCancelSkill = pDealTarget->GetCastSkill();
				if( pCancelSkill && pCancelSkill->m_Status == STATUS_CAST && XRandom( 0, 99 ) < nCastingCancelRate )
				{
					pDealTarget->CancelSkill();
				}

				AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
			}
			else
			{
				AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
			}
		}
		else
		{
			AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
		}
	}
}

void StructSkill::PHYSICAL_REALTIME_MULTIPLE_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int nEffectType = GetSkillBase()->GetSkillEffectType();

	int elemental_type = GetSkillBase()->GetElementalType();

	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + ( ( nEffectType == SkillBase::EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE ) ? ( GetVar(10) * GetEnhance() ) : 0 );
	nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + ( ( nEffectType == SkillBase::EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE ) ? ( GetVar(11) * GetEnhance() ) : 0 );

	if( m_nCurrentFire == 0 )
	{
		m_nTotalFire = GetVar(4) + GetVar(5) * GetRequestedSkillLevel();
		m_nCurrentFire = 1;
	}
	else
	{
		++m_nCurrentFire;
	}

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	if( m_nCurrentFire == m_nTotalFire && nEffectType == SkillBase::EF_PHYSICAL_REALTIME_MULTIPLE_DAMAGE_KNOCKBACK
		&& !Damage.bBlock && !Damage.bMiss && !Damage.bPerfectBlock
		&& !( pTarget->IsMonster() && static_cast< StructMonster * >( pTarget )->IsBossMonster() ) )
	{
		float fRange = GetVar(8) + GetVar(9) * GetRequestedSkillLevel();
		fRange *= GameRule::DEFAULT_UNIT_SIZE;

		AR_TIME knock_back_time = ( GetVar(10) + GetVar(11) * GetRequestedSkillLevel() ) * 100;

		AFFECT_KNOCK_BACK( pTarget, fRange, knock_back_time );
		AddSkillDamageWithKnockBackResult( &m_vResultList, SkillResult::DAMAGE_WITH_KNOCK_BACK, elemental_type, Damage, pTarget->GetHandle(), pTarget->GetPos().x, pTarget->GetPos().y, knock_back_time );
	}
	else
	{
		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}

	m_nFireTime += GetVar(6) * 100;

	return;
}

void StructSkill::PHYSICAL_REALTIME_MULTIPLE_REGION_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
	nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();

	if( m_nCurrentFire == 0 )
	{
		m_nTotalFire = GetVar(7) + GetVar(8) * GetRequestedSkillLevel();
		m_nCurrentFire = 1;
	}
	else
	{
		++m_nCurrentFire;
	}

	bool	bTargetOrigin		= true;		
	int		nRegionType			= -1;		
	float	fRegionProperty		= 0.0f;		

	float fEffectLength = GetVar(4) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), bTargetOrigin, fEffectLength, nRegionType, fRegionProperty, nDamage, true, m_pOwner, GetVar(5), GetVar(6), vTargetList );

	m_nTargetCount = static_cast< int >( vTargetList.size() );

	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		if( pDealTarget->IsDead() )
			continue;

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}

	m_nFireTime += GetVar(9) * 100;

	return;
}

void StructSkill::PHYSICAL_MULTIPLE_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
	nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();

	int nCount = GetVar(4) + GetVar(5) * GetRequestedSkillLevel();

	// 데미지
	for( int i = 0; i < nCount; ++i )
	{
		StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}

	m_nFireCount = nCount;

	// 연타 횟수 만큼 행동 불가
	m_nFireTime += GetVar(6) * nCount * 100;

	return;
}

void StructSkill::PHYSICAL_MULTIPLE_DAMAGE_TRIPLE_ATTACK( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
	nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();

	int nStep = std::min( (int)( ( GetRequestedSkillLevel() - 1 ) / GetVar(4) + 1 ), 3 );

	int nCount = GetVar( nStep + 4 );

	// 데미지
	for( int i = 0; i < nCount; ++i )
	{
		StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );
	}

	m_nFireCount = nCount;

	// 연타 횟수 만큼 행동 불가
	m_nFireTime += GetVar( 8 ) * nCount * 100;

	return;
}

void StructSkill::PHYSICAL_MULTIPLE_REGION_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	// 유형별 파라미터 세팅
	int		nCount				= 0;		// 연타 횟수
	bool	bTargetOrigin		= true;	// 기본 타겟 중심
	int		nRegionType			= -1;		// 기본 원형
	float	fRegionProperty		= 0.0f;		// 기본 범위 유형 속성값 없음
	AR_TIME	nFireInterval		= 0;		// 연타간 딜레이

	switch( GetSkillBase()->GetEffectType() )
	{
	case SkillBase::EF_PHYSICAL_MULTIPLE_REGION_DAMAGE:
		nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(10) * GetEnhance();
		nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();

		nCount = GetVar(7) + GetVar(8) * GetRequestedSkillLevel();

		nFireInterval = GetVar(9) * 100;
		break;
	case SkillBase::EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE_SELF:
		bTargetOrigin = false;		// break 쓰면 데미지 증폭/증가 안되요;;
	case SkillBase::EF_PHYSICAL_MULTIPLE_SPECIAL_REGION_DAMAGE:
		nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();
		nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(12) * GetEnhance();

		nCount = GetVar(9) + GetVar(10) * GetRequestedSkillLevel();

		nFireInterval = GetVar(13) * 100;

		nRegionType = static_cast< int >( GetVar(7) );
		fRegionProperty = GetVar(8);
		break;
	default:
		// 어느 유형인데 이쪽으로 오셨는가...?
		assert( 0 );
		return;
	}

	float fEffectLength = GetVar(4) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), bTargetOrigin, fEffectLength, nRegionType, fRegionProperty, nDamage, true, m_pOwner, GetVar(5), GetVar(6), vTargetList );

	m_nTargetCount = static_cast< int >( vTargetList.size() );

	// 데미지 적용
	for( int i = 0; i < nCount; ++i )
	{
		for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
		{
			StructCreature* pDealTarget = (*itTargetList);

			if( pDealTarget->IsDead() )
				continue;

			StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

			AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
		}
	}

	m_nFireTime += nFireInterval * nCount;
}

void StructSkill::PHYSICAL_SINGLE_SPECIAL_REGION_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(13) * GetEnhance();
	nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(11) * GetEnhance();

	float fEffectLength = GetVar(4) * GameRule::DEFAULT_UNIT_SIZE;

	m_fRange = fEffectLength;

	std::vector< StructCreature * > vTargetList;
	std::vector< StructCreature * >::iterator itTargetList;

	AR_TIME t = GetArTime();

	nDamage = EnumSkillTargetsAndCalcDamage( m_pOwner->GetCurrentPosition( t ), m_pOwner->GetLayer(), pTarget->GetCurrentPosition( t ), GetVar(8), fEffectLength, GetVar(7), GetVar(10), nDamage, GetVar(9), m_pOwner, GetVar(5), GetVar(6), vTargetList );

	m_nTargetCount = static_cast< int >( vTargetList.size() );

	// 데미지 적용
	for( itTargetList = vTargetList.begin(); itTargetList != vTargetList.end(); ++itTargetList )
	{
		StructCreature* pDealTarget = (*itTargetList);

		StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

		AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pDealTarget->GetHandle() );
	}
}

void StructSkill::PHYSICAL_CHAIN_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	int nOriginalDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	nOriginalDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();
	nOriginalDamage += GetVar(3) * GetEnhance();

	int nChainCount = GetVar(4) + GetVar(5) * GetRequestedSkillLevel();
	int nTargetType = GetVar(12);

	int nAdditionalChainDamage = nOriginalDamage;
	nAdditionalChainDamage *= GetVar(6) + GetVar(7) * GetRequestedSkillLevel();

	float fEffectLength = GetCastRange() * 2 * GameRule::DEFAULT_UNIT_SIZE;
	float fChainLength = GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;
	bool bIsOverlapped = GetVar(10);
	int nChainType = GetVar(11);

	m_fRange = fEffectLength;


	std::vector< StructCreature * > vTargetList;

	AR_TIME t = GetArTime();

	int nCount = 0;
	int nDamage = nOriginalDamage;
	StructCreature *pChainTarget = pTarget;
	AR_HANDLE hFrom = m_pOwner->GetHandle();

	do
	{
		// 첫 대상은 무조건 목록에 포함한다. bIncludeTarget = !nCount
		int result = EnumChainSkillTargetsAndCalcDamage( fEffectLength, fChainLength, nChainType, !nCount, m_pOwner, pChainTarget, nChainCount, vTargetList, nTargetType );
		if( !result ) break;

		// 데미지 적용
		for( int i = 0; i < result; ++i )
		{
			StructCreature* pDealTarget = vTargetList[i];

			StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

			AddChainSkillDamageResult( &m_vResultList, SkillResult::CHAIN_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle(), hFrom );

			nDamage += nAdditionalChainDamage;

			pChainTarget = pDealTarget;
			hFrom = pDealTarget->GetHandle();
		}

		nCount += result;
	} while( bIsOverlapped && nCount < nChainCount );

	m_nTargetCount = nCount;
}

void StructSkill::MAGIC_CHAIN_DAMAGE( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	int nOriginalDamage = m_pOwner->GetMagicPoint( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	nOriginalDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();
	nOriginalDamage += GetVar(3) * GetEnhance();

	int nChainCount = GetVar(4) + GetVar(5) * GetRequestedSkillLevel();
	int nTargetType = GetVar(12);

	int nAdditionalChainDamage = nOriginalDamage;
	nAdditionalChainDamage *= GetVar(6) + GetVar(7) * GetRequestedSkillLevel();

	float fEffectLength = GetCastRange() * 2 * GameRule::DEFAULT_UNIT_SIZE;
	float fChainLength = GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;
	bool bIsOverlapped = GetVar(10);
	int nChainType = GetVar(11);

	m_fRange = fEffectLength;


	std::vector< StructCreature * > vTargetList;

	AR_TIME t = GetArTime();

	int nCount = 0;
	int nDamage = nOriginalDamage;
	StructCreature *pChainTarget = pTarget;
	AR_HANDLE hFrom = m_pOwner->GetHandle();

	do
	{
		// 첫 대상은 무조건 목록에 포함한다. bIncludeTarget = !nCount
		int result = EnumChainSkillTargetsAndCalcDamage( fEffectLength, fChainLength, nChainType, !nCount, m_pOwner, pChainTarget, nChainCount, vTargetList, nTargetType );
		if( !result ) break;

		// 데미지 적용
		for( int i = 0; i < result; ++i )
		{
			StructCreature* pDealTarget = vTargetList[i];

			StructCreature::_DAMAGE_INFO Damage = pDealTarget->DealMagicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pDealTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

			AddChainSkillDamageResult( &m_vResultList, SkillResult::CHAIN_MAGIC_DAMAGE, elemental_type, Damage, pDealTarget->GetHandle(), hFrom );

			nDamage += nAdditionalChainDamage;

			pChainTarget = pDealTarget;
			hFrom = pDealTarget->GetHandle();
		}

		nCount += result;
	} while( bIsOverlapped && nCount < nChainCount );

	m_nTargetCount = nCount;
}

void StructSkill::CHAIN_HEAL( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	int elemental_type = GetSkillBase()->GetElementalType();

	int nOriginalDamage = m_pOwner->GetMagicPoint( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );
	nOriginalDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(2) * GetEnhance();
	nOriginalDamage += GetVar(3) * GetEnhance();

	int nChainCount = GetVar(4) + GetVar(5) * GetRequestedSkillLevel();
	int nTargetType = GetVar(12);

	int nAdditionalChainDamage = nOriginalDamage;
	nAdditionalChainDamage *= GetVar(6) + GetVar(7) * GetRequestedSkillLevel();

	float fEffectLength = GetCastRange() * 2 * GameRule::DEFAULT_UNIT_SIZE;
	float fChainLength = GetVar(9) * GameRule::DEFAULT_UNIT_SIZE;
	bool bIsOverlapped = GetVar(10);
	int nChainType = GetVar(11);

	m_fRange = fEffectLength;


	std::vector< StructCreature * > vTargetList;

	AR_TIME t = GetArTime();

	int nCount = 0;
	int nDamage = nOriginalDamage;
	StructCreature *pChainTarget = pTarget;
	AR_HANDLE hFrom = m_pOwner->GetHandle();

	do
	{
		// 첫 대상은 무조건 목록에 포함한다. bIncludeTarget = !nCount
		int result = EnumChainSkillTargetsAndCalcDamage( fEffectLength, fChainLength, nChainType, !nCount, m_pOwner, pChainTarget, nChainCount, vTargetList, nTargetType );
		if( !result ) break;

		// 힐 적용
		for( int i = 0; i < result; ++i )
		{
			StructCreature* pDealTarget = vTargetList[i];

			int nIncHP = pDealTarget->Heal( nDamage );

			SkillResult skill_result;

			skill_result.chain_heal.type = SkillResult::CHAIN_HEAL;
			skill_result.chain_heal.hTarget = pDealTarget->GetHandle();
			skill_result.chain_heal.target_hp = pDealTarget->GetHP();
			skill_result.chain_heal.nIncHP = nIncHP;
			skill_result.chain_heal.hFrom = hFrom;

			m_vResultList.push_back( skill_result );

			nDamage += nAdditionalChainDamage;

			pChainTarget = pDealTarget;
			hFrom = pDealTarget->GetHandle();
		}

		nCount += result;
	} while( bIsOverlapped && nCount < nChainCount );

	m_nTargetCount = nCount;
}

void StructSkill::PHYSICAL_SINGLE_DAMAGE_DEMINISHED_HP_MP( struct StructCreature * pTarget )
{
	if( !pTarget ) return;

	// 데미지 처리
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(4) * GetEnhance();
	nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(5) * GetEnhance();

	float DiminishedHPPercentage = 1.0f - (static_cast<float>(pTarget->GetHP()) / pTarget->GetMaxHP());
	float DiminishedMPPercentage = 1.0f - (static_cast<float>(pTarget->GetMP()) / pTarget->GetMaxMP());

	nDamage += (DiminishedHPPercentage / (GetVar(6) + GetVar(7))) * ((GetVar(8) + GetVar(9)) + (GetVar(10) + GetVar(11)));
	nDamage += (DiminishedMPPercentage / (GetVar(12) + GetVar(13))) * ((GetVar(14) + GetVar(15)) + (GetVar(16) + GetVar(17)));

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;
}


void StructSkill::PHYSICAL_SINGLE_DAMAGE_PROP_REMAIN_MP( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	// 데미지 처리
	int elemental_type = GetSkillBase()->GetElementalType();
	int nDamage = m_pOwner->GetAttackPointRight( (Elemental::Type)elemental_type, GetSkillBase()->IsPhysicalSkill(), GetSkillBase()->IsHarmful() );

	nDamage *= GetVar(0) + GetVar(1) * GetRequestedSkillLevel() + GetVar(18) * GetEnhance();
	nDamage += GetVar(2) + GetVar(3) * GetRequestedSkillLevel() + GetVar(19) * GetEnhance();

	int cost = std::min<int>( ( GetVar(6) + GetVar(7) * GetRequestedSkillLevel() ) * m_pOwner->GetMP(), m_pOwner->GetMP() );
	nDamage += ( GetVar(4) + GetVar(5) * GetRequestedSkillLevel() ) * cost;

	m_pOwner->AddMP( -cost );

	StructCreature::_DAMAGE_INFO Damage = pTarget->DealPhysicalSkillDamage( m_pOwner, nDamage, (Elemental::Type)elemental_type, GetHitBonus( GetEnhance(), m_pOwner->GetLevel() - pTarget->GetLevel() ), GetCriticalBonus( GetRequestedSkillLevel() ) );

	AddSkillDamageResult( &m_vResultList, SkillResult::DAMAGE, elemental_type, Damage, pTarget->GetHandle() );

	return;	
}

void StructSkill::INC_SKILL_COOL_TIME( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	if( GetVar( 0 ) )
	{
		// 시간의 지배자 이외의 스킬에서 모든 스킬의 쿨타임을 변경할 수 없다.
		// 이와 같이 하는 이유는 모든 스킬의 쿨타임을 변경하는 스킬이 서로 상호적으로 영향을 준다면 예기치 못 한 상황이 발생할 수 있으며, 이 상황에 대해 고려되어있지 않기 때문이다.
		// 추후 모든 스킬의 쿨타임을 변경하는 액티브 스킬은 유형에서 제외하여 비유형화하고 스킬 ID에 의해 예외 처리를 해야할 필요가 있다.
		assert( GetSkillId() != StructSkill::SKILL_RULER_OF_TIME );
		pTarget->AddRemainCoolTime( -1, GetVar( 1 ) + GetVar( 2 ) * GetRequestedSkillLevel(), 1.0f );
	}

	for( int i = 1; i < 6; ++i )
	{
		if( !GetVar( i * 3 ) ) break;

		pTarget->AddRemainCoolTime( GetVar( i * 3 ), GetVar( i * 3 + 1 ) + GetVar( i * 3 + 2 ) * GetRequestedSkillLevel(), 1.0f );
	}
}

void StructSkill::AMP_SKILL_COOL_TIME( struct StructCreature* pTarget )
{
	if( !pTarget ) return;

	if( GetVar( 0 ) )
	{
		// 시간의 지배자 이외의 스킬에서 모든 스킬의 쿨타임을 변경할 수 없다.
		// 이와 같이 하는 이유는 모든 스킬의 쿨타임을 변경하는 스킬이 서로 상호적으로 영향을 준다면 예기치 못 한 상황이 발생할 수 있으며, 이 상황에 대해 고려되어있지 않기 때문이다.
		// 추후 모든 스킬의 쿨타임을 변경하는 액티브 스킬은 유형에서 제외하여 비유형화하고 스킬 ID에 의해 예외 처리를 해야할 필요가 있다.
		assert( GetSkillId() != StructSkill::SKILL_RULER_OF_TIME );
		pTarget->AddRemainCoolTime( -1, 0, GetVar( 1 ) + GetVar( 2 ) * GetRequestedSkillLevel() );
	}

	for( int i = 1; i < 6; ++i )
	{
		if( !GetVar( i * 3 ) ) break;

		pTarget->AddRemainCoolTime( GetVar( i * 3 ), 0, GetVar( i * 3 + 1 ) + GetVar( i * 3 + 2 ) * GetRequestedSkillLevel() );
	}
}

void StructSkill::REPLENISH_ENERGY_HP_MP()
{
	int energyCount = 0;
	while (m_pOwner->GetEnergyCount() < m_pOwner->GetMaxEnergyCount()) {
		m_pOwner->AddEnergy();
		energyCount++;
	}

	int ReplenishedHPValue = GetVar( 0 ) + GetRequestedSkillLevel() * GetVar( 1 );
	c_fixed10 ReplenishedHPPercent = GetVar( 2 ) + GetRequestedSkillLevel() * GetVar( 3 );
	c_fixed10 ReplenishedMPPercent = GetVar( 6 ) + GetRequestedSkillLevel() * GetVar( 7 );

	int ReplenishedHP = energyCount * ( ReplenishedHPValue + ReplenishedHPPercent * m_pOwner->GetMaxHP() );
	int ReplenishedMP = energyCount * ( ReplenishedHPValue + ReplenishedHPPercent * m_pOwner->GetMaxHP() );

	m_pOwner->AddHP( ReplenishedHP );
	m_pOwner->AddMP( ReplenishedMP );

	SkillResult skill_result;

	skill_result.add_hp.type = SkillResult::ADD_HP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = ReplenishedHP;
	skill_result.add_hp.target_hp = m_pOwner->GetHP();

	m_vResultList.push_back( skill_result );

	skill_result.add_hp.type = SkillResult::ADD_MP;
	skill_result.add_hp.hTarget = m_pOwner->GetHandle();
	skill_result.add_hp.nIncHP = ReplenishedMP;
	skill_result.add_hp.target_hp = m_pOwner->GetMP();

	m_vResultList.push_back( skill_result );
}

