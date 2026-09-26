
#include <toolkit/XENV.h>
#include <mmo/ArcadiaServer.h>
#include <dump/XExceptionHandler.h>

#include "StructNPC.h"
#include "GameContent.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "GameProc.h"
#include "StructSummon.h"
#include "StructPlayer.h"
#include "StructMonster.h"
#include "StructRoamer.h"
#include "GameAllocator.h"
#include "StructSkill.h"
#include "ThreadPlayerHelper.h"

extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;

ArPosition StructNPC::getNonDuplicateAttackPos( StructCreature * pEnemy )
{
	AR_TIME t = GetArTime();
	ArPosition targetPos = pEnemy->GetCurrentPosition( t );

	AR_UNIT dist = GetUnitSize() / 2 + pEnemy->GetUnitSize() / 2 + GetRealAttackRange();

	float angle = 3.1415f * 2 * 0.1f;
	float fMod = (float) XRandom( -1, 3 );

	angle *= fMod == 0 ? 1.0f : fMod;

	ArPosition myPosition = GetCurrentPosition( t );

	AR_UNIT distance = myPosition.GetDistance( targetPos );

	targetPos.x += cos( angle ) * (float) dist;
	targetPos.y += sin( angle ) * (float) dist;

	return targetPos;
}

AR_HANDLE StructNPC::GetEnemyUID()
{
	return m_hEnemy;
}

void StructNPC::AI_processAttack( AR_TIME t )
{
	AR_HANDLE		enemy_handle = GetEnemyUID();
	StructCreature* pEnemy = static_cast< StructCreature* >( GameObject::raw_get( enemy_handle ) );
	
	// 주적이 없거나 적이 죽었거나 적이 유효하지 않으면 다음 적을 찾자.
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		if( !pEnemy || pEnemy->GetHandle() != enemy_handle || pEnemy->IsDead() || !pEnemy->IsInWorld() )
		{
			comeBackHome( false );
			// 한타임 쉬고, 다음 턴에 어택들어가주자.
			return;
		}
	}

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( this, pEnemy ) );

		if( isFirstAttack() )
		{
			pEnemy->OnUpdate();

			m_StatusFlag.Off( STATUS_FIRST_ATTACK );
		}
	}

	//AI_processAttack( pEnemy, t );
}

void StructNPC::comeBackHome( bool bInvincible )
{
	if( !IsMovable() )
		return;

	// 정상상태로~
	SetStatus( StructNPC::STATUS_NORMAL );

	m_hEnemy = NULL;

	m_nLastEnemyDistance = 0;
	m_bComeBackHome = true;

	// 귀환
	std::vector< ArPosition > newPos;
	newPos.push_back( ArPosition( m_RespawnX, m_RespawnY, 0 ) );

	if( bInvincible )
	{
		SetInvincible( true );
		SetPendingMove( newPos, GetRealMoveSpeed() * 2 );
	}
	else
	{
		SetPendingMove( newPos, GetRealMoveSpeed() );
	}
}

void StructNPC::processDead( AR_TIME t )
{
	bool bDelete = false;
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		// 업데이트 주기를 5초로 수정.
		ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_NORMAL );

		// 시체유지시간만큼 지났으면 없애버리자
		if( GetDeadTime() + GameRule::GetCorpseHoldTime() < t )
		{
			ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_IDLE );

			// 월드에서 제거한다.
			if( IsInWorld() )
			{
				RemoveNPCFromWorld( this );
			}

			bDelete = true;
		}
	}

	// no other lock~
	if( bDelete && m_pDeadHandler )
	{
		m_pDeadHandler->onNPCDead( this );
	}

}

void StructNPC::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "StructNPC" );

	AR_TIME t = GetArTime();

	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "StructNPC(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;
	ThreadPlayerHelper TPHelper( NULL );

	{
		switch( GetStatus() )
		{
		case StructNPC::STATUS_DEAD:
			{	// 죽은 녀석이면 시체 사라지는 처리만 하면 된다.
				processDead( t );
				return;
			}
			break;

		case StructNPC::STATUS_NORMAL:
			{
				if( GetNPCBase()->is_periodic && GetNPCBase()->end_of_period <= time( NULL ) )
				{
					ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

					onDead( NULL, false );
					SetDeadTime( 0 );

					RemoveNPCFromWorld( this );
					return;
				}

				if( GetRoamer() )
				{
					if( GetPriority() != ArSchedulerObject::UPDATE_PRIORITY_HIGH )
					{
						ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGH );
					}
				}
				else
				{
					if( !HasPendingMove() && GetPriority() > ArObject::UPDATE_PRIORITY_NORMAL )
					{
						ArcadiaServer::Instance().SetObjectPriority( this, ArObject::UPDATE_PRIORITY_NORMAL );
					}
				}

				if( m_nLastUpdateTime + 3000 < t || HasPendingMove() )
				{
					ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

					// 시간대 제한 진행 퀘스트의 수락 가능 조건이 변경되었을 수 있으므로 30초에 한 번쯤 주변에 업데이트 날려 줌
					SetNeedToBroadcastStatusFlag();

					OnUpdate();

					if( IsDead() ) return;
				}

				// 움직이고 있지 않다면 이리저리 돌아다니게 처리
				if( IsActable() && !m_bComeBackHome ) processFirstAttack( t );
				// if( IsActable() && IsMovable() && !m_bComeBackHome ) processMove( t );
			}
			break;

		case StructNPC::STATUS_TRACKING:
		case StructNPC::STATUS_FIND_ATTACK_POS:
		case StructNPC::STATUS_ATTACK:
			{	// 전투중이면 전투 AI 처리
				{
					ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

					if( m_nLastUpdateTime + 50 < t ) OnUpdate();
				}

				if( IsActable() )
				{
					if( IsDead() ) return; // 업데이트 안에서 뒤지는 경우가 생기는구나.-_-

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

	if( IsDead() ) return;

	if( HasPendingMove() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		processPendingMove();
	}

	// 움직이고 있으면 움직임 연산
	if( ArObject::IsMoving() && IsMovable() )
	{
		processWalk( t );
	}

	// StructCreature를 상속받은 클래스는 모두 호출을 해야하지만 현재는 굳이 호출할 필요가 없다.
	// StructCreature::onProcess( nThreadIdx );
}

void StructNPC::processWalk( AR_TIME t )
{
	ArMoveVector tmp_mv;
	// 시간만큼 이동해 본다.
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		tmp_mv = GetMv();
	}
	tmp_mv.Step( t );
		
	// 만약 이동했는데 Region 변경이 일어났거나 혹은 이동이 멈추었다면
	// 그것을 ArcadiaServer 에게 통지해준다.
	if( tmp_mv.GetRX() != GetRX() || tmp_mv.GetRY() != GetRY() || !tmp_mv.IsMoving() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithSpecificRegion( this, tmp_mv.GetRX(), tmp_mv.GetRY() ) );

		if( m_bComeBackHome && !IsBattleMode() && !tmp_mv.IsMoving() )
		{
			m_bComeBackHome = false;

			if( IsAttackableNPC() )
				SetInvincible( false );
		}

		ArcadiaServer::Instance().onRegionChange( this,
													t - lastStepTime,
													!tmp_mv.IsMoving() );
	}
}

bool StructNPC::IsFirstAttacker() const
{
	return m_pBaseInfo->offensive_type;
}

void StructNPC::processFirstAttack( AR_TIME t )
{
	if( !IsAttackableNPC() )
		return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

	// 죽었으면 스탑~
	if( GetStatus() == StructNPC::STATUS_DEAD )
		return;

	if( GetStatus() == StructNPC::STATUS_NORMAL )
	{
		std::vector< AR_HANDLE >	vResult;
		ArcadiaServer::Instance().EnumMovableObject( GetPos(), GetLayer(), GameRule::AGGRESIVE_MONSTER_REACTION_RANGE, &vResult, true, true );

		AR_UNIT distance = GameRule::AGGRESIVE_MONSTER_REACTION_RANGE + 1;
		StructCreature * pTarget = NULL;

		for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
		{
			StructCreature* pCreature = static_cast< StructCreature* >( StructCreature::raw_get( *it ) );
			if( !pCreature )
				continue;


			if( IsEnemy( pCreature ) )
			{
				if( IsFirstAttacker() )
				{
					AR_UNIT _distance = GetPos().GetDistance( pCreature->GetCurrentPosition( t ) );

					if( pCreature->IsSummon() && _distance > GameRule::AGGRESIVE_MONSTER_REACTION_RANGE / 2 )
					{
						// 소환수는 절반.
						continue;
					}

					if( _distance < distance )
					{
						distance = _distance;
						pTarget = pCreature;
					}
				}
				else
				{
					if( pCreature->IsAttacking() )
					{
						pTarget = pCreature;
						break;
					}
					else if( pCreature->IsUsingSkill() )
					{
						AR_HANDLE target = pCreature->GetCastSkill()->GetTargetHandle();

						if( target )
						{
							StructCreature* pSkillTarget = static_cast< StructCreature* >( StructCreature::raw_get( target ) );

							if( pSkillTarget && ( pSkillTarget->IsPlayer() || pSkillTarget->IsSummon() ) && pSkillTarget->IsEnemy( pCreature ) )
							{
								pTarget = pCreature;
								break;
							}
						}

					}
				}
			}
		}

		if( pTarget && pTarget->IsSummon() )
		{
			for( std::vector< AR_HANDLE >::iterator it = vResult.begin(); it != vResult.end(); ++it )
			{
				if( static_cast< StructSummon * >( pTarget )->GetMaster()->GetHandle() == (*it) )
				{
					pTarget = static_cast< StructSummon * >( pTarget )->GetMaster();

					break;
				}
			}
		}

		if( pTarget )
		{
			SetAttacker( pTarget );
		}
	}
}

bool StructNPC::StartAttack( AR_HANDLE target, bool bNeedFastReaction )
{
	if( StructCreature::StartAttack( target, bNeedFastReaction ) )
	{
		SetStatus( StructNPC::STATUS_ATTACK );

		return true;
	}

	return false;
}

void StructNPC::AI_processAttack( StructCreature * pEnemy, AR_TIME t )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( this, pEnemy ) );

	// 적이 가시 범위 내인가..?
	if( !IsVisibleRegion( GetRX(), GetRY(), pEnemy->GetRX(), pEnemy->GetRY() ) || pEnemy->GetLayer() != GetLayer() )
	{	
		comeBackHome( false );
		return;
	}

	// 공격 사정거리 밖이면 쫒아가자
	ArPosition enemyPosition = pEnemy->GetCurrentPosition( t );
	ArPosition myPosition = GetCurrentPosition( t );
	AR_UNIT enemy_distance = myPosition.GetDistance( enemyPosition );


	if( ( pEnemy->IsMoving() && enemy_distance > GameRule::MONSTER_TRACKING_RANGE_BY_TIME )  ||  ( m_nLastEnemyDistance && enemy_distance > GameRule::MONSTER_TRACKING_RANGE && enemy_distance > m_nLastEnemyDistance ) )
	{
		// 치고 빠지다니. 나쁜 녀석~
		comeBackHome( false );

		return;
	}

	m_nLastEnemyDistance = enemy_distance;

	AR_UNIT gap = GetUnitSize()/2 + pEnemy->GetUnitSize()/2;


	if( ( pEnemy->IsMoving() && ( enemy_distance - gap > GetRealAttackRange() * 1.5f ) ) || ( !pEnemy->IsMoving() && ( enemy_distance - gap > GetRealAttackRange() * 1.2f ) ) )
	{

		if( !IsActable() || !IsMovable() || GetMovableTime() > t )
			return;

		ArPosition targetPosition = enemyPosition;

		if( pEnemy->IsMoving() )
		{
			AR_TIME nModTime = 15; // 게임 룰로 뺍시다아~

			targetPosition = pEnemy->GetCurrentPosition( t + nModTime );
		}
		else
		{
			findAttackablePosition( myPosition, targetPosition, enemy_distance, gap + GetRealAttackRange() );
		}

		// { 길찾기 테스트
		/*
		std::vector< ArPosition > vMoveInfo;
		if( GameRule::bMonsterPathFinding && !GameContent::IsBlocked( targetPosition.x, targetPosition.y ) )
		{
			if( GameContent::FindPath( myPosition.x, myPosition.y, targetPosition.x, targetPosition.y, vMoveInfo ) == false )
			{
				// ?
			}
		}
		*/


		// 추적상태로 변경
		SetStatus( StructNPC::STATUS_TRACKING );

		ArPosition homePosition( m_RespawnX, m_RespawnY );
		AR_UNIT track_distance = myPosition.GetDistance( homePosition );

		if( track_distance > GetChaseRange() )
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

			if( GameContent::IsBlocked( targetPosition.x, targetPosition.y ) )
			{
				return;
			}

			ArcadiaServer::Instance().SetMove( this, GetCurrentPosition( t ), targetPosition, (int) ( (float) GetRealMoveSpeed() * fMod ), true, GetArTime() );
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
				if( ( ::IsMonster( (*it) ) && (*it) > GetHandle() ) || ( pEnemy->IsPlayer() &&  static_cast< StructPlayer * >( pEnemy )->GetHandle() == (*it) ) )
				{
					ArPosition attack_pos = getNonDuplicateAttackPos( pEnemy );

					SetStatus( StructNPC::STATUS_FIND_ATTACK_POS );

					if( GameContent::IsBlocked( attack_pos.x, attack_pos.y ) )
						continue;

					ArcadiaServer::Instance().SetMove( this, myPosition, attack_pos, GetRealMoveSpeed() , true, GetArTime() );
					return;
				}
			}
		}
		// 공격 상태로 변경
		SetStatus( StructNPC::STATUS_ATTACK );

		// 행동시간 체크
		if( GetNextAttackableTime() <= t )
		{
			if( !IsAttackable() || !pEnemy || !IsEnemy( pEnemy ) || pEnemy->IsDead() )
				return;

			// 두들겨 패자~~ 랄라~

			_ATTACK_INFO Damages[4];
			bool bDoubleAttack = false;
			Attack( pEnemy, t, GetAttackInterval(), Damages, bDoubleAttack );


			float fAttackMotionRatio = 0.5f;
/*
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
			}
*/
			SetMovableTime( t + GetAttackInterval() * fAttackMotionRatio );

			broadcastAttackMessage( pEnemy, Damages, GetAttackInterval()*10, ( GetNextAttackableTime() - t ) * 10, bDoubleAttack, false );
		}
	}
}

void StructNPC::findAttackablePosition( const ArPosition & myPosition, ArPosition & enemyPosition, AR_UNIT distance, AR_UNIT gap )
{
	ArPosition duplicateEnemyPosition = enemyPosition;
	AR_UNIT walk_length = distance - gap;
	
	if( walk_length < 0 )
	{
		enemyPosition = myPosition;
		return;
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
		enemyPosition = duplicateEnemyPosition;
	}
}

void StructNPC::SetAttacker( StructCreature * pAttacker )
{
	if( !IsAttackableNPC() )
		return;

	if( !m_hEnemy )
	{
		StartAttack( pAttacker->GetHandle(), false );
	}
}
