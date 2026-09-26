
#include <algorithm>

#include <mmo/ArcadiaServer.h>
#include <toolkit/XRandom.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>
#include <toolkit/XEnv.h>

#include "ErrorCode/ErrorCode.h"

#include "GameAllocator.h"
#include "Extern.h"
#include "StructMonster.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructRoamer.h"
#include "SendMessage.h"
#include "GameContent.h"
#include "GameProc.h"
#include "StructSkill.h"
#include "LuaVM.h"
#include "GameMessage.h"
#include "ThreadPlayerHelper.h"
#include "SchedulingPerformanceTracker.h"


// 몹이 플레이어 따라가는 거리
const int	TRACK_DISTANCE	= 600;
extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;

volatile LONG g_nMobLineCount = 0;
volatile LONG g_nMobPathFindingCount = 0;
volatile LONG g_nMobFindDuplicatePos = 0;
volatile LONG g_nMobRandomMove = 0;
volatile LONG g_nMobFirstLineCount = 0;


ArPosition StructMonster::getNonDuplicateAttackPos( StructCreature * pEnemy )
{
	AR_TIME t = GetArTime();
	ArPosition targetPos = pEnemy->GetCurrentPosition( t );

	AR_UNIT dist = GetUnitSize() / 2 + pEnemy->GetUnitSize() / 2 + GetRealAttackRange();

	float angle = 3.1415f * 2 * 0.1f;
	float fMod = (float) XRandom( -1, 3 );

	angle *= fMod == 0 ? 1.0f : fMod;

	ArPosition myPosition = GetCurrentPosition( t );

	AR_UNIT distance = myPosition.GetDistance( targetPos );

	float cur_angle = 0;

	if( int( distance ) )
	{
		cur_angle = acos( (float) ( myPosition.GetX() - targetPos.GetX() ) / distance );
	}

	if( myPosition.GetY() - targetPos.GetY() < 0 )
	{
		cur_angle = 3.1415f * 2 - cur_angle;
	}
	angle += cur_angle;

	targetPos.x += cos( angle ) * (float) dist;
	targetPos.y += sin( angle ) * (float) dist;

	return targetPos;
}

AR_HANDLE StructMonster::GetEnemyUID()
{
	return m_hEnemy;
}

AR_HANDLE StructMonster::GetMaxDamageDealer() const
{
	AR_HANDLE hMaxDamageDealer = NULL;

	int nMaxDamage = 0;
	for( std::vector< _DAMAGE_TAG >::const_iterator it = m_vDamageList.begin() ; it != m_vDamageList.end() ; ++it )
	{
		if( (*it).nDamage > nMaxDamage )
		{
			StructPlayer::iterator itPlayer = StructPlayer::get( (*it).uid );
			if( !(*itPlayer) )
				continue;

			nMaxDamage = (*it).nDamage;
			hMaxDamageDealer = (*it).uid;
		}
	}

	return hMaxDamageDealer;
}

void StructMonster::OnUpdate()
{
	if( m_bNeedToFindEnemy )
	{
		m_bNeedToFindEnemy = false;
		findNextEnemy();
	}

	StructCreature::OnUpdate();
}

void StructMonster::findNextEnemy()
{
	int nMaxHate = -1;
	AR_HANDLE target = NULL;

	{
		extern XCriticalSection m_csHate;
		THREAD_SYNCHRONIZE( &m_csHate );

		std::vector< _HATE_TAG >::iterator it;
		std::vector< _HATE_MODIFIER_TAG >::iterator mit;
		for( it = m_vHateList.begin(); it != m_vHateList.end(); ++it )
		{
			if( !(*it).bIsActive )
				continue;

			int nHate = (*it).nHate;

			// 헤이트 증가 지속효과에 의한 헤이트 처리
			for( mit = m_vHateModifierByState.begin() ; mit != m_vHateModifierByState.end() ; ++mit )
			{
				if( (*mit).uid == (*it).uid )
				{
					nHate += (*mit).nHate;
					break;
				}
			}

			if( nHate > nMaxHate )
			{
				StructCreature::iterator itTarget = StructCreature::get( (*it).uid );

				if( (*itTarget) && IsVisibleRegion( (*itTarget)->GetRX(), (*itTarget)->GetRY(), GetRX(), GetRY() ) && IsVisible( *itTarget ) )
				{
					nMaxHate = nHate;
					target = (*it).uid;
				}
			}
		}
	}

	if( nMaxHate != -1 )
	{
		m_nMaxHate = nMaxHate;
		m_hEnemy = target;

		// 여기서 타겟에게 OnUpdate를 날렸었으나 지역 락 밖에 있는 애한테 OnUpdate를 날리는
		// 문제를 유발하게 되므로 m_bIsFirstAttack 만 세팅하여 이후 호출되는 AI_processAttack에서
		// 적 중심으로 지역 락 걸고 OnUpdate 호출하도록 프로세스 변경
		// 보고 있는 적 변경 시 공격이 한 턴 지연될 수 있으나 서버 다운보다 백만배 낫다 -┏
		m_StatusFlag.On( STATUS_FIRST_ATTACK );
	}
	else
	{
		comeBackHome( false );
	}
}

void StructMonster::updateHate()
{
	AR_TIME t = GetArTime();

	extern XCriticalSection m_csHate;
	THREAD_SYNCRONIZE( &m_csHate );

	std::vector< _HATE_TAG >::iterator it;

	for( it = m_vHateList.begin(); it != m_vHateList.end(); )
	{
		_HATE_TAG * pHateTag = &(*it);

		if( pHateTag->nHate < 1 )
		{
			StructCreature::iterator itTarget = StructCreature::get( pHateTag->uid );

			if( *itTarget )
			{
				( *itTarget )->RemoveFromEnemyList( GetHandle() );
			}

			it = m_vHateList.erase( it );
			continue;
		}

		if( pHateTag->nTime + 6000 < t )
		{
			int nRemoveHate = ( pow( 0.1f, ( pHateTag->nTime - t ) / 6000.0f * 0.1f - 2.04f ) - 9 ) * pHateTag->nLastMaxHate;

			if( nRemoveHate > pHateTag->nHate )
			{
				StructCreature::iterator itTarget = StructCreature::get( pHateTag->uid );

				if( *itTarget )
				{
					( *itTarget )->RemoveFromEnemyList( GetHandle() );
				}

				it = m_vHateList.erase( it );
				continue;
			}
			else
			{
				pHateTag->nHate -= nRemoveHate;
			}
		}

		++it;
	}
}

void StructMonster::comeBackHome( bool bInvincible )
{
	if( ( !IsMovable() || IsFeared() ) && !IsAutoTrap() )
		return;

	// 정상상태로~
	SetStatus( StructMonster::STATUS_NORMAL );

	m_nLastHateUpdateTime = GetArTime();

	updateHate();

	m_hEnemy = NULL;
	m_nMaxHate = 0;
	m_nLastEnemyDistance = 0;

	// 몬스터가 원 위치한다는 것은 플레이어가 사실 상 이 몬스터 테이밍에 대한 관심을 끊었다는 이야기도 되므로 테이머 초기화
	ClearTamer( this );

	// 귀환. 단, 도망형일 경우 아무 일도 하지 않음.
	if( !IsRunaway() )
	{
		m_bComeBackHome = true;

		std::vector< ArPosition > newPos;
		newPos.push_back( ArPosition( m_RespawnX, m_RespawnY, 0 ) );

		if( bInvincible )
		{
			SetInvincible( true );
		}

		SetPendingMove( newPos, GetRealMoveSpeed() * 2 );
	}
}

void StructMonster::AI_processAttack( StructCreature * pEnemy, AR_TIME t )
{
	if( !IsInWorld() ) return;

	if( ( IsAutoTrap() || ( IsAgent() && ( !pEnemy->IsPlayer() || !static_cast< StructPlayer * >( pEnemy )->IsAutoUsed() ) ) ) && pEnemy->GetEnemyHandle() != GetHandle() )
	{
		comeBackHome( false );
		return;
	}

	// 적이 가시 범위 내인가..?
	if( !IsVisibleRegion( GetRX(), GetRY(), pEnemy->GetRX(), pEnemy->GetRY() ) || pEnemy->GetLayer() != GetLayer() )
	{	
		comeBackHome( false );
		return;
	}

	// 좌표 관련 미리 계산
	ArPosition enemyPosition = pEnemy->GetCurrentPosition( t );
	ArPosition myPosition = GetCurrentPosition( t );
	AR_UNIT enemy_distance = myPosition.GetDistance( enemyPosition );

	// 공격 사정거리 밖이면 쫒아가자
	_HATE_TAG * pHateTag = getHateTag( pEnemy->GetHandle() );
	bool bFindNextEnemy = false;

	if( pHateTag && pHateTag->nTime + 2500 < t )
	{
		bFindNextEnemy = true;
	}

	if( !IsDungeonRaidMonster() &&
		( ( bFindNextEnemy && pEnemy->IsMoving() && enemy_distance > GameRule::MONSTER_TRACKING_RANGE_BY_TIME ) || ( m_nLastEnemyDistance && enemy_distance > GameRule::MONSTER_TRACKING_RANGE && enemy_distance > m_nLastEnemyDistance ) )
		)
	{
		// 치고 빠지다니. 나쁜 녀석~

		_HATE_TAG * pHateTag = getHateTag( pEnemy->GetHandle(), t );

		if( pHateTag )
		{
			pHateTag->bIsActive = false;
			++pHateTag->nBadAttackCount;


			pHateTag->nHate -= std::max( int( pHateTag->nHate * 0.5f), 100 );
		}

		findNextEnemy();

		return;
	}

	m_nLastEnemyDistance = enemy_distance;

	AR_UNIT gap = GetUnitSize()/2 + pEnemy->GetUnitSize()/2;

	// 도망형 몬스터의 경우는 반대로 도망을 간다
	if( IsRunaway() )
	{
		if( GetStatus() != StructMonster::STATUS_RUNAWAY )
		{
			if( IsInWorld() && GetRealMoveSpeed() && IsMovable() )
			{
				std::vector< ArPosition > vNewPos;
				ArPosition currentPos( GetX(), GetY() );
				for( int i = 0; i < 3; ++i )
				{
					ArPosition newPos;

					float theta = ( XRandom() % 628 ) / 100.0f;

					newPos.x = currentPos.x + sin( theta ) * GameRule::RUNAWAY_RANGE;
					newPos.y = currentPos.y + cos( theta ) * GameRule::RUNAWAY_RANGE;

					if( newPos.x > g_nMapWidth ) newPos.x = g_nMapWidth;
					if( newPos.y > g_nMapWidth ) newPos.y = g_nMapWidth;
					if( newPos.x < 0 ) newPos.x = 0;
					if( newPos.y < 0 ) newPos.y = 0;

					if( GameRule::bMonsterCollisionToLine && GameContent::CollisionToLine( currentPos.x, currentPos.y, newPos.x, newPos.y ) == true )
						continue;

					currentPos = newPos;
					vNewPos.push_back( newPos );
				}

				if( vNewPos.empty() == false )
				{
					SetStatus( StructMonster::STATUS_RUNAWAY );
					ArcadiaServer::Instance().SetMultipleMove( this, GetCurrentPosition( t ), vNewPos, (int) ( (float) GetRealMoveSpeed() * 1.3f ), true, GetArTime() );
				}
			}
		}
		else
		{
			if( IsMoving( t ) )
			{
				return;
			}
			else
			{
				comeBackHome( false );
			}
		}

		return;
	}

	ArPosition targetPosition = enemyPosition;

	if( pEnemy->IsMoving() )
	{
		AR_TIME nModTime = 15; // 게임 룰로 뺍시다아~

		targetPosition = pEnemy->GetCurrentPosition( t + nModTime );
		if( GameContent::IsBlocked( targetPosition.x, targetPosition.y ) )
		{
			comeBackHome( false );
			return;
		}
	}
	else
	{
		if( findAttackablePosition( myPosition, targetPosition, enemy_distance, gap + GetRealAttackRange() ) == false )
		{
			// 실패시 적 위치에..
			if( GameContent::IsBlocked( targetPosition.x, targetPosition.y ) )
			{
				comeBackHome( false );
				return;
			}
		}
	}

	bool bStraightLine = true;
	if( GameRule::bMonsterPathFinding )
	{
		if( GameRule::bLogMonsterPathFinding )
			InterlockedIncrement( &g_nMobLineCount );

		if( GameContent::CollisionToLine( myPosition.x, myPosition.y, targetPosition.x, targetPosition.y ) )
		{
			bStraightLine = false;
		}
	}

	// 스킬 처리 ( 스킬, Trigger는 따로 캐스팅 사거리가 없기때문에 StraightLine이기만 하면 쓸수 있다고 한다. -ㅁ-)
	if( bStraightLine && GetNextCastableTime() <= t )
	{
		// 스킬 처리를 할 때 다음 스킬 사용 가능 시간을 세팅하여 단기간에 여러번 스킬 처리를 하는 것 방지.
		// 스킬 처리를 사용하는 시간 간격도 AttackInterval로 사용한다. (공격 속도가 빠르면 스킬 확률이 낮아도 시도를 많이해서 빨라진다. 시전속도로 바꿔야하나?)
		SetNextCastableTime( t + GetAttackInterval() );

		// 스킬 시전 조건 체크 1 (MonsterSkillResource 트리거 조건 확인)
		int nTriggerIndex = 0;
		for( std::vector< MonsterBase::MONSTER_TRIGGER >::const_iterator it = m_pContentInfo->trigger_list->begin() ; it != m_pContentInfo->trigger_list->end() ; ++it, ++nTriggerIndex )
		{
			const MonsterBase::MONSTER_TRIGGER & trigger = (*it);
			if( CheckTriggerCondition( trigger, nTriggerIndex ) )
			{
				char szScript[255];

				s_sprintf( szScript, _countof( szScript ), "%s( %u, %u, %d, %d, %d, %d, %d )", trigger.script, GetHandle(), pEnemy->GetHandle(), nTriggerIndex, myPosition.x, myPosition.y, GetLayer(), IsDungeonRaidMonster() );

				// 스크립트 성공/실패에 따라 함수를 종료할지 계속할지 체크
				SetContinueAttack( false );

				LUA()->RunString( szScript );

				if( !IsNeedToContinueAttack() )
				{
					// 스킬 시전 스크립트가 성공했을 경우 이동 중이었다면 제자리에 정지(Lua 파일의 스크립트 함수 trigger가 몬스터 스킬 처리)
					if( IsMoving() && strstr( trigger.script, "trigger" ) )
					{
						// 파라미터로 넘어온 t를 사용해도 되지만 트리거를 실행하는데 소요된 시간 등을 고려해서 새로 시간값을 받아 사용
						AR_TIME tCurrent = GetArTime();
						ArPosition stopPosition = GetCurrentPosition( tCurrent );
						ArcadiaServer::Instance().SetMove( this, myPosition, stopPosition, 0, true, tCurrent );
					}

					// 트리거 스킬 성공했으니까 공격 바로 하지 못하게 한 타임 쉬자. (아니면 스킬쓰자마자 바로 때려서 보기에 좀 이상하고 난이도도 확 높아짐.)
					SetNextAttackableTime( t + GetAttackInterval() );
					return;
				}

				SetContinueAttack( false );
				break;
			}
		}

		// 스킬 시전 조건 체크 2
		for( std::vector< MonsterBase::MONSTER_SKILL_INFO >::const_iterator it = m_pContentInfo->skill_info_list->begin() ; it != m_pContentInfo->skill_info_list->end() ; ++it )
		{
			const MonsterBase::MONSTER_SKILL_INFO & skill_info = (*it);

			if( skill_info.skill_id && skill_info.skill_probability )
			{
				if( XRandom( 0, 10000 ) < skill_info.skill_probability * 10000 )
				{
					AR_HANDLE hSkillTarget = m_hEnemy;

					StructSkill* pSkill = getSkill( skill_info.skill_id );

					if( pSkill )
					{
						if( !pSkill->GetSkillBase()->IsHarmful() )
							hSkillTarget = GetHandle();

						// 스킬 사용 못하면 패스
						if( ( pSkill->GetSkillBase()->IsPhysicalSkill() && !IsSkillCastable() ) || ( pSkill->GetSkillBase()->IsMagicalSkill() && !IsMagicCastable() ) )
							continue;

						if( CastSkill( skill_info.skill_id, skill_info.skill_lv, hSkillTarget, enemyPosition, GetLayer() ) == RESULT_SUCCESS )
						{
							// 스킬 시전 처리가 성공했을 경우 이동 중이었다면 제자리에 정지
							if( IsMoving() )
							{
								// 파라미터로 넘어온 t를 사용해도 되지만 트리거를 실행하는데 소요된 시간 등을 고려해서 새로 시간값을 받아 사용
								AR_TIME tCurrent = GetArTime();
								ArPosition stopPosition = GetCurrentPosition( tCurrent );
								ArcadiaServer::Instance().SetMove( this, myPosition, stopPosition, 0, true, tCurrent );
							}

							// 스킬 캐스팅 성공했으니까 공격 바로 하지 못하게 한 타임 쉬자. (아니면 스킬쓰자마자 바로 때려서 보기에 좀 이상하고 난이도도 확 높아짐.)
							SetNextAttackableTime( t + GetAttackInterval() );
							return;
						}
					}
				}
			}
		}
	}

	// 이동을 해야 하는지 검사
	if( ( pEnemy->IsMoving( t ) && ( enemy_distance - gap > GetRealAttackRange() * 1.5f ) ) || ( !pEnemy->IsMoving( t ) && ( enemy_distance - gap > GetRealAttackRange() * 1.2f ) ) || !bStraightLine )
	{
		if( IsMoving() && !pEnemy->IsMoving( t ) )
		{
			if( GetTargetPos().GetDistance( enemyPosition ) - gap <= GetRealAttackRange() * 1.2f )
			{
				return;
			}
		}

		if( !IsActable() || !IsMovable() || GetMovableTime() > t )
			return;

		// 길찾기
		std::vector< ArPosition > vMoveInfo;
		if( GameRule::bMonsterPathFinding && !bStraightLine )
		{
			if( GameRule::bLogMonsterPathFinding )
				InterlockedIncrement( &g_nMobPathFindingCount );

			if( GameContent::FindPath( myPosition.x, myPosition.y, targetPosition.x, targetPosition.y, vMoveInfo ) == false )
			{
				// ?
			}
		}

		// 추적상태로 변경
		SetStatus( StructMonster::STATUS_TRACKING );

		/*		if( targetPosition.GetDistance( myPosition ) < GetRealAttackRange() )
		{
		return;
		}
		*/
		ArPosition homePosition( m_RespawnX, m_RespawnY );
		AR_UNIT track_distance = myPosition.GetDistance( homePosition );

		// 너무 멀리 쫓아간경우에는 회귀
		if( !IsDungeonRaidMonster() && track_distance > GetChaseRange() )
		{
			comeBackHome( true );
			return;
		}

		if( m_nLastTrackTime + 50 < t )
		{
			m_nLastTrackTime = t;

			float fMod = XRandom( 0, 9 );

			fMod /= 100.0f;

			fMod += 1;

			if( IsMovable() )
			{
				if( !GameRule::bMonsterPathFinding || bStraightLine )
				{
					ArcadiaServer::Instance().SetMove( this, myPosition, targetPosition, (int) ( (float) GetRealMoveSpeed() * fMod ), true, t );
				}
				else if( !vMoveInfo.empty() )
				{
					ArcadiaServer::Instance().SetMultipleMove( this, myPosition, vMoveInfo, ( (float) GetRealMoveSpeed() * fMod ), true, t );
				}
				else // 몬스터 길찾기를 해야하나, 목표지점까지 갈 수 있는 방법이 없을 때.
				{
					comeBackHome( false );
					return;
				}
			}
		}
	}
	else
	{
		if( GetStatus() == StructMonster::STATUS_FIND_ATTACK_POS && IsMoving( t ) )
			return;

		if( !pEnemy->IsMoving( t ) && 
			( GetStatus() == StructMonster::STATUS_TRACKING || ( pEnemy->GetEnemyCount() < 6 && XRandom( 0, 99 ) < GameRule::MONSTER_FIND_ATTACK_POS_RATIO ) ) )
		{
			std::vector< AR_HANDLE > vList;
			std::vector< AR_HANDLE >::iterator it;
			ArcadiaServer::Instance().EnumMovableObject( myPosition, GetLayer(), GetUnitSize() / 2, &vList );

			for( it = vList.begin(); it != vList.end(); ++it )
			{
				if( ( ::IsMonster( (*it) ) && (*it) > GetHandle() ) || ( (GetStatus() == StructMonster::STATUS_TRACKING) && pEnemy->IsPlayer() &&  static_cast< StructPlayer * >( pEnemy )->GetHandle() == (*it) ) )
				{
					ArPosition attack_pos = getNonDuplicateAttackPos( pEnemy );

					if( GameRule::bLogMonsterPathFinding )
						InterlockedIncrement( &g_nMobFindDuplicatePos );

					if( GameRule::bMonsterPathFinding )
					{
						if( GameContent::CollisionToLine( myPosition.x, myPosition.y, attack_pos.x, attack_pos.y ) )
						{
							continue;
						}
					}
					else
					{
						if( GameContent::IsBlocked( attack_pos.x, attack_pos.y ) )
							continue;
					}

					// 안겹치게 설수 있다면 거기에서 선다.
					if( IsMovable() )
					{
						SetStatus( StructMonster::STATUS_FIND_ATTACK_POS );
						ArcadiaServer::Instance().SetMove( this, myPosition, attack_pos, GetRealMoveSpeed() , true, GetArTime() );
						return;
					}
				}
			}
		}
		int nPrevStatus = GetStatus();

		// 공격 상태로 변경
		SetStatus( StructMonster::STATUS_ATTACK );

		// 통상공격 처리
		if( bStraightLine && GetNextAttackableTime() <= t )
		{
			if( !IsAttackable() || !pEnemy || !IsEnemy( pEnemy ) || pEnemy->IsDead() )
				return;

			// 공격 처리가 시작되기 전에 이동 중이라면 제자리에 정지
			if( IsMoving() )
			{
				ArPosition stopPosition = GetCurrentPosition( t + 10 );
				ArcadiaServer::Instance().SetMove( this, myPosition, stopPosition, 0, true, GetArTime() );
			}

			// 두들겨 패자~~ 랄라~

			_ATTACK_INFO Damages[4];
			bool bDoubleAttack = false;
			Attack( pEnemy, t, GetAttackInterval(), Damages, bDoubleAttack );


			float fAttackMotionRatio;

			switch( m_pContentInfo->attack_speed_type )
			{
			case 0: // 공속 느림
				fAttackMotionRatio = 0.5f;
				break;
			case 1:
				fAttackMotionRatio = 0.6f;
				break;
			case 2:
				fAttackMotionRatio = 0.7f;
				break;
			case 3:
				fAttackMotionRatio = 1.0f;
				break;
			default:
				fAttackMotionRatio = 0.5f;
				break;
			}

			SetMovableTime( t + GetAttackInterval() * fAttackMotionRatio );

			broadcastAttackMessage( pEnemy, Damages, GetAttackInterval()*10, ( GetNextAttackableTime() - t ) * 10, bDoubleAttack, false );
		}

		if( pEnemy->IsMoving( t ) && nPrevStatus == StructMonster::STATUS_TRACKING )
		{
			SetStatus( StructMonster::STATUS_TRACKING );
		}
	}
}

bool StructMonster::ResetTriggerCondition()
{
	size_t nTriggerCount = m_pContentInfo->trigger_list->size();
	for( size_t idx = 0 ; idx < nTriggerCount ; ++idx )
	{
		switch( m_pContentInfo->trigger_list->at( idx ).id )
		{
		case MonsterBase::TRIGGER_TYPE_NONE:
			break;

		case MonsterBase::TRIGGER_TYPE_HP_BELOW_ONCE:
			if( GetHPPercentage() >= m_pContentInfo->trigger_list->at( idx ).value[0] )
			{
				m_vTriggerFlag[idx] = 0;
			}

			break;

		case MonsterBase::TRIGGER_TYPE_HP_BELOW_ALWAYS:
			break;

		case MonsterBase::TRIGGER_TYPE_TIME_ONCE:
			m_vTriggerFlag[idx] = GetArTime();
			break;

		case MonsterBase::TRIGGER_TYPE_TIME_ALWAYS:
			m_vTriggerFlag[idx] = GetArTime();
			break;

		case MonsterBase::TRIGGER_TYPE_TIME_ALWAYS_INCLUDE_START:
			m_vTriggerFlag[idx] = 0;
			break;

		default:
			break;
		}
	}

	return true;
}

bool StructMonster::CheckTriggerCondition( const MonsterBase::MONSTER_TRIGGER & trigger, const int nTriggerIndex )
{
	AR_TIME t = GetArTime();

	switch( trigger.id )
	{
	case MonsterBase::TRIGGER_TYPE_NONE:
		return false;

	case MonsterBase::TRIGGER_TYPE_HP_BELOW_ONCE:
		if( !m_vTriggerFlag[ nTriggerIndex ] && GetHPPercentage() <= trigger.value[ 0 ] )
		{
			m_vTriggerFlag[ nTriggerIndex ] = 1;
			return true;
		}
		return false;

	case MonsterBase::TRIGGER_TYPE_HP_BELOW_ALWAYS:
		if( GetHPPercentage() <= trigger.value[ 0 ] && XRandom( 1, 10000 ) <= trigger.value[ 1 ] * 100 )
		{
			return true;
		}
		return false;

	case MonsterBase::TRIGGER_TYPE_TIME_ONCE:
		if( m_vTriggerFlag[ nTriggerIndex ] && m_vTriggerFlag[ nTriggerIndex ] + trigger.value[ 0 ] * 100 < GetArTime() )
		{
			m_vTriggerFlag[ nTriggerIndex ] = 0;
			return true;
		}
		return false;

	case MonsterBase::TRIGGER_TYPE_TIME_ALWAYS:
		if( m_vTriggerFlag[ nTriggerIndex ] + trigger.value[ 0 ] * 100 < GetArTime() )
		{
			m_vTriggerFlag[ nTriggerIndex ] = t;
			return true;
		}
		return false;

	case MonsterBase::TRIGGER_TYPE_TIME_ALWAYS_INCLUDE_START:
		if( !m_vTriggerFlag[ nTriggerIndex ] || m_vTriggerFlag[ nTriggerIndex ] + trigger.value[ 0 ] * 100 < GetArTime() )
		{
			m_vTriggerFlag[ nTriggerIndex ] = t;
			return true;
		}
		return false;

	default:
		return false;

	}
}

void StructMonster::AI_processAttack( AR_TIME t )
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

	AR_HANDLE		enemy_handle = GetEnemyUID();
	StructCreature* pEnemy = static_cast< StructCreature* >( GameObject::raw_get( enemy_handle ) );

	// 주적이 없거나 적이 죽었거나 적이 유효하지 않으면 다음 적을 찾자.
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		if( !pEnemy || pEnemy->GetHandle() != enemy_handle || pEnemy->IsDead() || !pEnemy->IsInWorld() || !IsVisible( pEnemy ) ||
			( pEnemy->IsPlayer() && static_cast< StructPlayer * >( pEnemy )->IsInBattleField() ) ||
			( pEnemy->IsSummon() && static_cast< StructSummon * >( pEnemy )->GetMaster()->IsInBattleField() ) )
		{
			removeFromHateList( enemy_handle );
			findNextEnemy();
			// 한타임 쉬고, 다음 턴에 어택들어가주자.
			return;
		}
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( this, pEnemy ) );

	if( isFirstAttack() )
	{
		pEnemy->OnUpdate();

		m_StatusFlag.Off( STATUS_FIRST_ATTACK );
	}

	// 상태 OnUpdate로 갱신하고 한 번 더 체크~
	if( pEnemy->IsDead() )
	{
		removeFromHateList( enemy_handle );
		findNextEnemy();
		// 한타임 쉬고, 다음 턴에 어택들어가주자.
		return;
	}

	AI_processAttack( pEnemy, t );
}

void StructMonster::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "StructMonster" );

	SchedulingPerformanceHelper helper;
	if( GameRule::bLogSchedulingStatus )
	{
		helper.start();
	}

	AR_TIME t = GetArTime();

	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "StructMonster(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;
	ThreadPlayerHelper TPHelper( NULL );

	// 타이머 걸려서 사라져야 될 놈이면 월드에서 없앤 후에 사망 처리가 발생하여 월드에서 사라지고 삭제 시 발생해야 할 처리(onMonsterDelete)가 이루어지도록 함
	if( IsLifeTimeOver() && IsAlive() && GetStatus() != STATUS_DEAD )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );		

		if( IsInWorld() )
		{
			// 스킬 사용 중이었으면 스킬 캔슬
			CancelSkill();

			// 제자리에 정지시키고(여기서 건 지역락 이후 아래의 processDead에서 거는 지역락에 도달했을 때 좌표가 달라져 있는 현상을 방지하기 위함)
			ArPosition pos = GetCurrentPosition( t );
			ArcadiaServer::Instance().SetMove( this, pos, pos, 0, true, t, false );

			// 월드에서 제거
			RemoveMonsterFromWorld( this );
		}

		// 시체 없애는 처리로 인해 몬스터가 즉시 없어지도록 사망 시각을 세팅
		SetHP( 0 );
		SetDeadTime( t - GameRule::GetCorpseHoldTime() );
		SetStatus( STATUS_DEAD );
	}

	{
		switch( GetStatus() )
		{
		case StructMonster::STATUS_DEAD:
			{	// 죽은 녀석이면 시체 사라지는 처리만 하면 된다.
				processDead( t );
				return;
			}
			break;

		case StructMonster::STATUS_NORMAL:
			{
				if( GetRoamer() )
				{
					if( GetPriority() != ArSchedulerObject::UPDATE_PRIORITY_HIGH )
					{
						ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGH );
					}
				}
				else
				{
					if( !HasPendingMove() )
					{
						if( GetPriority() > ArSchedulerObject::UPDATE_PRIORITY_NORMAL )
						{
							ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );
						}
					}
					else if( GetPriority() > ArSchedulerObject::UPDATE_PRIORITY_HIGH )
					{
						ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGH );
					}
				}

				if( m_nLastUpdateTime + 3000 < t || HasPendingMove() )
				{
					ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
					OnUpdate();

					if( IsDead() ) break;

					if( GetMaxHP() == GetHP() )
					{
						clearHateList();
					}
				}

				// 움직이고 있지 않다면 이리저리 돌아다니게 처리
				if( IsInWorld() && IsActable() && ( IsDungeonRaidMonster() || !m_bComeBackHome ) ) processFirstAttack( t );
				if( IsInWorld() && IsActable() && IsMovable() && !m_bComeBackHome ) processMove( t );
			}
			break;

		case StructMonster::STATUS_TRACKING:
		case StructMonster::STATUS_FIND_ATTACK_POS:
		case StructMonster::STATUS_ATTACK:
		case StructMonster::STATUS_RUNAWAY:
			{	// 전투중이면 전투 AI 처리

				{
					ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

					if( m_nLastUpdateTime + 50 < t ) OnUpdate();
				}

				if( IsActable() )
				{
					if( IsDead() ) break; // 업데이트 안에서 뒤지는 경우가 생기는구나.-_-

					if( GetRoamer() && !GetRoamer()->IsPaused() )
					{
						GetRoamer()->PauseRoaming();
						GetRoamer()->PendHateShare( GetHandle(), GetEnemyUID(), 1, StructRoamer::HATE_TYPE_SHARE_FIRST | StructRoamer::HATE_TYPE_FULL_SHARE );

						break;
					}

					AI_processAttack( t );
				}
			}
			break;
		}
	}

	if( IsDead() ) 
	{
		if( GameRule::bLogSchedulingStatus )
		{
			helper.end();
			g_MonsterPerformanceTracker.addUpThisResult( helper );
		}
		return;
	}

	if( HasPendingMove() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		if( IsInWorld() ) processPendingMove();
	}

	if( m_nLastHateUpdateTime < t + 6000 )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		m_nLastHateUpdateTime = t;

		updateHate();
	}

	// 테이밍 유효 시간이 지났다면 테이밍 풀림
	if( m_nTamedTime != INFINITE_TIME && m_nTamedTime < t )
	{
		ClearTamer( this );
	}

	if( ArObject::IsMoving() && IsMovable() )
	{
		// 움직이고 있으면 움직임 연산
		processWalk( t );
	}

	StructCreature::onProcess( nThreadIdx );
	if( GameRule::bLogSchedulingStatus )
	{
		helper.end();
		g_MonsterPerformanceTracker.addUpThisResult( helper );
	}
}

void StructMonster::regenHPMP( AR_TIME t )
{
	int nHP = GetHP();
	int nMP = GetMP();

	m_nRegenHP = GetMaxHP() - nHP;
	m_nRegenMP = GetMaxMP() - nMP;

	if( GetMonsterBase()->fight_type == MonsterBase::FIGHT_TYPE_AUTO_TRAP && ( m_nRegenHP || m_nRegenMP ) )
		RegenFullHPMP();
	else
		StructCreature::regenHPMP( t );
}

void StructMonster::processDead( AR_TIME t )
{
	// no other lock~
	if( m_pDeleteHandler )
	{
		m_pDeleteHandler->onMonsterDelete( this );
		m_pDeleteHandler = NULL;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

	// 업데이트 주기를 5초로 수정.
	ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_NORMAL );

	// 시체유지시간만큼 지났으면 없애버리자
	if( GetDeadTime() + GameRule::GetCorpseHoldTime() < t )
	{
		if( IsEnable() )
		{
			// 더이상의 스케쥴러 요청을 무시
			Disable();

			// 월드에서 제거한다.
			if( IsInWorld() )
			{
				RemoveMonsterFromWorld( this );
			}

			// object delete 요청
			ArcadiaServer::Instance().DeleteObject( this );
		}
	}
}


// enemyPosition 에 있는 적을 쫓아가서 떄리기 위해서 이동해야할 목표를 다시 enemyPosition 에 설정해 넘겨준다.
// 지금은 그냥 쫓아가지만, 나중에는 장애물 등을 피해가는 길찾기가 적용 되어야 한다.
bool StructMonster::findAttackablePosition( const ArPosition & myPosition, ArPosition & enemyPosition, AR_UNIT distance, AR_UNIT gap )
{
	ArPosition duplicateEnemyPosition = enemyPosition;
	AR_UNIT walk_length = distance - gap;

	if( walk_length < 0 )
	{
		return false;
	}

	assert( walk_length >= 0 );
	assert( int( distance ) );

	float v = walk_length / (float)distance;	// 가야할 비율
	v += 0.1f; // 사거리보다 10% 정도 더 가자

	enemyPosition.x -= myPosition.x;
	enemyPosition.y -= myPosition.y;
	enemyPosition.z -= myPosition.z;

	enemyPosition.x *= v;
	enemyPosition.y *= v;
	enemyPosition.z *= v;

	enemyPosition.x += myPosition.x;
	enemyPosition.y += myPosition.y;
	enemyPosition.z += myPosition.z;

	if( GameContent::IsBlocked( enemyPosition.x, enemyPosition.y ) )
	{
		// 복구
		enemyPosition = duplicateEnemyPosition;
		return false;
	}

	return true;
}

// 몬스터가 거닐 새로운 위치를 찾는다.
// 나중에는 해당 몬스터의 활동반경을 벗어나지 않도록 조절해 줘야한다.
// 물론 장애물들을 피해가는 길찾기도 추가 되어야함.
bool StructMonster::getMovePosition( ArPosition & newPos )
{
	newPos.x = GetX() + XRandom()%120 - 60;
	newPos.y = GetY() + XRandom()%120 - 60;

	if( newPos.x > g_nMapWidth ) newPos.x = g_nMapWidth;
	if( newPos.y > g_nMapHeight ) newPos.y = g_nMapHeight;
	if( newPos.x < 0 ) newPos.x = 0;
	if( newPos.y < 0 ) newPos.y = 0;

	if( GameRule::bLogMonsterPathFinding )
		InterlockedIncrement( &g_nMobRandomMove );

	// 전투 시작전에는 항상 유효한 좌표에 있게하자. 아니면 전투를 할 수 없다.
	if( GameRule::bMonsterCollisionToLine && GameContent::CollisionToLine( GetX(), GetY(), newPos.x, newPos.y ) )
	{
#ifdef	_DEBUG
		AR_UNIT newX = 0, newY = 0;
		if( GameContent::GetIntersectPoint( GetX(), GetY(), newPos.x, newPos.y, newX, newY ) == true )
		{
			newPos.x = newX;
			newPos.y = newY;
			return true;
		}
#endif

		return false;
	}

	return true;
}

// 이동처리 - 이동중인 NPC 시간 t 만큼 가 REGION 을 옮길경우 통지해준다.
void StructMonster::processWalk( AR_TIME t )
{
	ArMoveVector tmp_mv;
	// 시간만큼 이동해 본다.
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		tmp_mv = GetMv();
	}
	tmp_mv.Step( t );

#ifndef _USE_NEW_ROAMING_ONLY
	if( GetStatus() == STATUS_NORMAL && !m_bComeBackHome && m_pWayPointInfo )
	{
		SetReturnPosition( tmp_mv.GetX(), tmp_mv.GetY() );

		if( m_nWayPointIdx < 0 && m_pWayPointInfo->way_point_type == GameContent::WAY_POINT_CIRCULAR )
		{
			m_nWayPointIdx = 0 - static_cast< int >( tmp_mv.GetWayPointCount() );
		}
		else
		{
			m_nWayPointIdx = static_cast< int >( m_pWayPointInfo->vWayPoint.size() ) - 1 - static_cast< int >( tmp_mv.GetWayPointCount() );
		}

	}
#endif

	// 만약 이동했는데 Region 변경이 일어났거나 혹은 이동이 멈추었다면
	// 그것을 ArcadiaServer 에게 통지해준다.
	if( tmp_mv.GetRX() != GetRX() || tmp_mv.GetRY() != GetRY() || !tmp_mv.IsMoving() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithSpecificRegion( this, tmp_mv.GetRX(), tmp_mv.GetRY() ) );

		if( !IsInWorld() )
			return;

		if( m_bComeBackHome && !IsBattleMode() && !tmp_mv.IsMoving() )
		{
			if( IsInvincible() )
			{
				SetInvincible( false );
			}

			m_bComeBackHome = false;
		}

		// _oprint( "MR : (%d,%d)->(%d,%d) %d\n", (int)mv.x, (int)mv.y, (int)tmp_mv.x, (int)tmp_mv.y, tmp_mv.IsMoving() );

		ArcadiaServer::Instance().onRegionChange( this,
			t - lastStepTime,
			!tmp_mv.IsMoving() );
	}
}

void StructMonster::processFirstAttack( AR_TIME t )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

	// 죽었으면 스탑~
	if( GetStatus() == StructMonster::STATUS_DEAD )
		return;

	// 리젠된지 5초동안은 선공 반응 없음.
	if( m_nRegenTime + 500 > t && !m_bRespawnByScript )
		return;

	if( GetStatus() == StructMonster::STATUS_NORMAL && ( IsAgent() || IsFirstAttacker() || IsBattleRevenger() ) )
	{
		AR_HANDLE target = 0;
		std::vector< StructMonster * > vSameGroup;

		if( IsAgent() )
		{
			std::vector< AR_HANDLE >	vResult;
			ArcadiaServer::Instance().EnumMovableObject( GetPos(), GetLayer(), GetFirstAttackRange(), &vResult, true, true );

			for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
			{
				StructCreature* pCreature = static_cast< StructCreature* >( StructCreature::raw_get( *it ) );

				if( !pCreature )
					continue;

				if( pCreature->IsDead() )
					continue;

				if( !pCreature->IsPlayer() )
					continue;

				StructPlayer * pPlayer = static_cast< StructPlayer * >( pCreature );

				if( !pPlayer->IsAutoUsed() )
					continue;

				pPlayer->AddState( StructState::NEMESIS_FOR_AUTO, 0, 10, t, t + 8640000 );

				target = pPlayer->GetHandle();
				break;
			}
		}
		else
		{
			// Hate AI
			std::vector< AR_HANDLE >	vResult;
			ArPosition myPos = GetCurrentPosition( t );
			ArcadiaServer::Instance().EnumMovableObject( myPos, GetLayer(), GetFirstAttackRange(), &vResult, true, true );


			// 가장 가까이 있는 적부터 검색하기 위해 거리 순으로 정렬한다.
			std::sort( vResult.begin(), vResult.end(),
				[&myPos, t]( AR_HANDLE l, AR_HANDLE r ) -> bool
				{
					StructCreature* pLCreature = static_cast< StructCreature* >( StructCreature::raw_get( l ) );
					StructCreature* pRCreature = static_cast< StructCreature* >( StructCreature::raw_get( r ) );

					if (pLCreature == NULL && pRCreature == NULL)
						return false;

					if (pLCreature == NULL)
						return false;
					if(pRCreature == NULL )
						return true;

					return myPos.GetDistance( pLCreature->GetCurrentPosition( t ) ) < myPos.GetDistance( pRCreature->GetCurrentPosition( t ) );
				}
			);

			for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
			{
				StructCreature* pCreature = static_cast< StructCreature* >( StructCreature::raw_get( *it ) );

				if( !pCreature )
					continue;

				if( pCreature->IsDead() )
					continue;


				if( !IsFirstAttacker() && pCreature->IsMonster() && pCreature->IsAttacking() )
				{
					target = static_cast< StructMonster * >( pCreature )->m_hEnemy;
				}

				if( IsGroupFirstAttacker() && pCreature->IsMonster() && pCreature != this )
				{
					if( static_cast< StructMonster * >( pCreature )->GetMonsterGroup() == GetMonsterGroup() )
					{
						vSameGroup.push_back( static_cast< StructMonster * >( pCreature ) );
					}
				}

				if( IsFirstAttacker() && IsEnemy( pCreature ) )
				{
					ArPosition myPosition( GetCurrentPosition( t ) );
					ArPosition enemyPosition( pCreature->GetCurrentPosition( t ) );
					AR_UNIT _distance = myPosition.GetDistance( enemyPosition );

					if( pCreature->IsSummon() && _distance > GetFirstAttackRange() / 2 )
					{
						// 소환수는 절반.
						continue;
					}

					// 사이에 장애물, 벽 등이 있으면 못 보므로 선공 리스트에 추가하지 않는다
					if( GameRule::bLogMonsterPathFinding )
						InterlockedIncrement( &g_nMobFirstLineCount );

					// 처음 전투 시작시에는 반드시 시야를 확인한다.
					if( GameRule::bMonsterCollisionToLine && GameContent::CollisionToLine( myPosition.x, myPosition.y, enemyPosition.x, enemyPosition.y ) )
						continue;

					target = (*it);
					break;
				}
			}
		}

		if( target )
		{
			if( !IsFirstAttacker() )
			{
				AddHate( target, 1, true );
			}
			else
			{
				AddHate( target, 1, false );

				if( IsGroupFirstAttacker() )
				{
					for( std::vector< StructMonster * >::iterator it = vSameGroup.begin(); it != vSameGroup.end(); ++it )
					{
						(*it)->AddHate( target, 1, false );
					}
				}
			}
		}
	}
}

// 일상적인 상태일때 랜덤하게 이리저리 돌아다니도록 처리해 준다.
void StructMonster::processMove( AR_TIME t )
{
	if( IsDungeonConnector() ) return;

	// 일단 락
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

	// 죽었으면 스탑~
	if( IsDead() )
		return;

#ifndef _USE_NEW_ROAMING_ONLY
	if( m_pWayPointInfo )
	{
		if( IsMoving() )
		{
			return;
		}

		int max_way_point_idx = static_cast< int >( m_pWayPointInfo->vWayPoint.size() ) - 1;

		if( m_nWayPointIdx >= max_way_point_idx )
		{
			if( m_pWayPointInfo->way_point_type == GameContent::WAY_POINT_CIRCULAR )
			{
				m_nWayPointIdx = 0 - max_way_point_idx;
			}
			else
			{
				m_nWayPointIdx = -1;
			}
		}

		int final_target_idx = max_way_point_idx;

		if( m_pWayPointInfo->way_point_type == GameContent::WAY_POINT_CIRCULAR && m_nWayPointIdx < 0 )
		{
			// reverse~
			final_target_idx = 0;
		}

		std::vector< ArPosition > vMoveInfo;

		for( int i = m_nWayPointIdx + 1; i <= final_target_idx; ++i )
		{
			vMoveInfo.push_back( m_pWayPointInfo->vWayPoint[ abs(i) ] );
		}

		AR_TIME t = GetArTime();

		unsigned char speed = m_pWayPointInfo->way_point_speed / 7;

		if( speed == 0 )
		{
			speed = GetRealMoveSpeed();
		}

		ArcadiaServer::Instance().SetMultipleMove( this, GetCurrentPosition( t ), vMoveInfo, speed, true, t );

		return;
	}
#endif

	// 근처에 클라이언트가 아무도 없으면 IDLE 모드로 전환.
	if( !IsDead() && GetStatus() == StructMonster::STATUS_NORMAL && !bIsNearClient && m_vHateList.size() == 0 )
	{
		ClearTamer( this );
		SetStatus( StructMonster::STATUS_NORMAL );
		ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_IDLE );
		return;
	}

	if( !m_bIsWandering )
		return;

	if( !GameRule::bMonsterWandering )
		return;

	int nRandom = XRandom();

	if( GetStatus() == StructMonster::STATUS_NORMAL &&		// 평상모드이고
		IsAlive() &&										// 죽지 않았으며
		!IsMoving() &&										// 이동 중 아니고
		lastStepTime + 200 + nRandom % 500 < t &&			// 이전 이동한지 2 ~ 7초가 지났을때 
		nRandom%3 == 0)										// 33% 확률로 이동
	{
		ArPosition targetPos;
		if( getMovePosition( targetPos ) == true )
		{
			ArcadiaServer::Instance().SetMove( this, GetPos(), targetPos, GetRealMoveSpeed(), true, GetArTime() );
			SetReturnPosition( targetPos.GetX(), targetPos.GetY() );
		}
	}

	return;
}
