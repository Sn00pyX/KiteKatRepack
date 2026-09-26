
#include <cmath>

#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>
#include <dump/XExceptionHandler.h>

#include "GameProc.h"
#include "StructRoamer.h"
#include "RoamingManager.h"
#include "ThreadPlayerHelper.h"


StructRoamer::StructRoamer( const int nID, const StructRoamer::ROAMING_TYPE eRoamingType, const int nMoveSpeed, const HATE_TYPE eHateType, const AR_TIME nRespawnInterval, const int nAttributeFlag, const bool bIsRaidDungeonRoamer )
	: ArObject( MOVABLE_OBJECT )
	, m_nID( nID )
	, m_nMoveSpeed( nMoveSpeed )
	, m_eRoamingType( eRoamingType )
	, m_eHateType( eHateType )
	, m_nRespawnInterval( nRespawnInterval )
	, m_bIsRaidDungeonRoamer( bIsRaidDungeonRoamer )
	, m_eCurrentRoamingDirection( ROAMING_DIRECTION_FORWARD )
	, m_nCurrentRoamingPointIndex( 0 )
	, m_eRoamingStatus( ROAMING_STATUS_IDLE )
	, m_nNextRespawnProcTime( 0 )
	, m_nLastRegenCount( 0 )
{
	m_AttributeFlag.CopyFrom( &nAttributeFlag );
}

StructRoamer::~StructRoamer()
{
	DeInit( true );
}

void StructRoamer::AddCreatureRespawnInfo( const GameContent::ROAMING_CREATURE_RESPAWN_INFO & info )
{
	THREAD_SYNCHRONIZE( m_CS );

	m_vRoamingCreatureRespawnInfo.push_back( info );
}

void StructRoamer::AddRoamingPoint( const ArPosition & pos )
{
	THREAD_SYNCHRONIZE( m_CS );

	if( IsInitialized() || IsInWorld() )
	{
		assert( 0 );
		return;
	}

	m_vRoamingPoint.push_back( pos );

	switch( m_vRoamingPoint.size() )
	{
	case 1:
		// 최초 등록이면 현재 위치로 설정
		SetCurrentXY( pos.x, pos.y );
		break;
	case 2:
		// 2번째 등록이면 방향 설정
		mv.SetDirection( pos );
		break;
	}
}

void StructRoamer::DeleteRespawnedCreature()
{
	THREAD_SYNCHRONIZE( m_CS );

	m_nNextRespawnProcTime = 0;

	for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
	{
		StructCreature *pCreature = static_cast< StructCreature * >( (*it).m_pCreature );
		if( !pCreature )
			continue;

		(*it).m_nNextRespawnTime = 0;
		(*it).m_pCreature = NULL;

		if( pCreature->IsInWorld() )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pCreature ) );

			if( pCreature->IsEnable() )
			{
				pCreature->Disable();

				switch( (*it).m_eCreatureType )
				{
				case ROAMING_CREATURE_MONSTER:
					{
						StructMonster *pMonster = static_cast< StructMonster * >( pCreature );

						// 락 걸려고 대기하는 사이에 없어졌을 수 있음
						if( pMonster->IsInWorld() )
							RemoveMonsterFromWorld( pMonster );

						pMonster->SetDeleteHandler( NULL );
					}
					break;
				case ROAMING_CREATURE_NPC:
					{
						StructNPC *pNPC = static_cast< StructNPC * >( pCreature );

						// 락 걸려고 대기하는 사이에 없어졌을 수 있음
						if( pNPC->IsInWorld() )
							RemoveNPCFromWorld( pNPC );

						pNPC->SetDeadHandler( NULL );
					}
					break;
				default:
					// 뉘시오 -_ -?
					assert( 0 );
					break;
				}
				ArcadiaServer::Instance().DeleteObject( pCreature );
			}
		}
	}
}

const bool StructRoamer::Init()
{
	if( IsInitialized() )
		return false;

	m_nCurrentRoamingPointIndex = 0;
	m_eCurrentRoamingDirection = ROAMING_DIRECTION_FORWARD;
	SetCurrentXY( m_vRoamingPoint.front().GetX(), m_vRoamingPoint.front().GetY() );

	m_eRoamingStatus = ROAMING_STATUS_ROAMING;

	// 리젠 일체화 적용 그룹의 경우 최초에 리젠 처리가 발생하도록 리젠 처리 시각을 1회 세팅(안 하면 일체화 그룹에는 몹이 리젠되지 않음)
	if( m_nRespawnInterval )
	{
		m_nNextRespawnProcTime = GetArTime();
		m_nLastRegenCount = 0;
	}
	// 리젠 일체화 비적용 그룹의 경우 최초에 리젠 처리가 발생하도록 각각의 리젠 처리 시각을 1회 세팅(안 하면 비일체화 그룹에는 몹이 리젠되지 않음)
	else
	{
		THREAD_SYNCHRONIZE( m_CS );

		for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
		{
			(*it).m_nNextRespawnTime = GetArTime();
		}
	}

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		ArcadiaServer::Instance().AddObject( this );
	}
	ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_HIGH );

	return true;
}

const bool StructRoamer::DeInit( const bool bForceToDeleteEverlastingRoamer )
{
	if( !IsInitialized() )
		return false;

	ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_IDLE );

	{
		THREAD_SYNCHRONIZE( m_csHate );

		for( std::vector< PENDING_HATE_SHARE_INFO * >::const_iterator itHate = m_vPendingHateInfo.begin() ; itHate != m_vPendingHateInfo.end() ; ++itHate )
			delete (*itHate);

		m_vPendingHateInfo.clear();
	}

	DeleteRespawnedCreature();

	if( IsInWorld() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

		ArcadiaServer::Instance().RemoveObject( this );
	}

	// 던전 레이드 로밍 그룹은 DeInit 되어도 유지되어야 하므로 객체를 삭제하지 않음(월드에서는 제거해서 추후 Init 시 AddObject가 가능하도록 유지)
	if( m_bIsRaidDungeonRoamer && !bForceToDeleteEverlastingRoamer )
		return true;

	RoamingManager::Instance().UnregisterRoamingInfo( m_nID );

	ArcadiaServer::Instance().DeleteObject( this );

	return true;
}

void StructRoamer::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "StructRoamer" );

	AR_TIME t = GetArTime();

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "StructRoamer(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;
	ThreadPlayerHelper TPHelper( NULL );

	{
		THREAD_SYNCHRONIZE( m_CS );

		if( GetPriority() != ArSchedulerObject::UPDATE_PRIORITY_HIGH )
		{
			assert( 0 );
			ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_HIGH );
		}

		ArPosition currentPos;
		float fFace;

		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( this ) );

			currentPos = GetCurrentPosition( t );
			fFace = GetFace();
		}

		// 리젠 처리
		if( !m_nRespawnInterval || ( m_eRoamingStatus == ROAMING_STATUS_ROAMING && m_nNextRespawnProcTime && m_nNextRespawnProcTime <= t ) )
		{
			for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; /* 루프에서 ++it 처리 */ )
			{
				StructCreature *pCreature = static_cast< StructCreature * >( (*it).m_pCreature );
				if( pCreature )
				{
					++it;
					continue;
				}

				// 개별 리젠인데 몹 리젠해야할 시간이 지정되어 있지 않거나 아직 시간이 안 됐으면 패스
				if( ( !m_nRespawnInterval && ( !(*it).m_nNextRespawnTime || (*it).m_nNextRespawnTime > t ) ) )
				{
					++it;
					continue;
				}

				switch( (*it).m_eCreatureType )
				{
				case ROAMING_CREATURE_MONSTER:
					{
						StructMonster *pMonster = StructMonster::AllocMonster( (*it).m_nCreatureID );

						if( !pMonster )
						{
							assert( 0 );
							++it;
							continue;
						}

						(*it).m_pCreature = pMonster;

						pMonster->SetGenerateCode( StructMonster::BY_RESPAWN );
						ArPosition pos = getCurrentRespawnObjectPosition( currentPos, fFace, (*it).m_nAngle, (*it).m_nDistance );
						pMonster->SetCurrentXY( pos.x, pos.y );
						pMonster->SetCurrentLayer( GetLayer() );
						pMonster->SetWandering( false );

						pMonster->SetRoamer( this );
						pMonster->SetDeleteHandler( this );

						ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pMonster ) );
						AddMonsterToWorld( pMonster );

						// 이동 중이었어야 했다면 이동 처리
						if( IsMoving() )
						{
							ArPosition targetPos = getCurrentRespawnObjectPosition( GetCurrentRoamingTargetPosition(), fFace, (*it).m_nAngle, (*it).m_nDistance );
							ArcadiaServer::Instance().SetMove( pMonster, pos, targetPos, m_nMoveSpeed, false, 0 );
						}
					}
					break;

				case ROAMING_CREATURE_NPC:
					{
						NPCBase & npcBase( GameContent::GetNPCInfo( (*it).m_nCreatureID ) );

						if( !npcBase.id )
						{
							assert( 0 );
							++it;
							continue;
						}

						if( npcBase.is_periodic )
						{
							time_t tCurrent = time( NULL );
							// 리젠 기간이 만료된 NPC가 리젠 대기 중이라면 삭제
							if( npcBase.end_of_period <= tCurrent )
							{
								it = m_vRoamingCreatureRespawnInfo.erase( it );
								continue;
							}

							// 리젠 기간이 아니면 패스
							if( npcBase.begin_of_period > tCurrent )
							{
								++it;
								continue;
							}
						}

						StructNPC * pNPC = new StructNPC( &npcBase );
						(*it).m_pCreature = pNPC;

						ArPosition pos = getCurrentRespawnObjectPosition( currentPos, fFace, (*it).m_nAngle, (*it).m_nDistance );
						pNPC->SetCurrentXY( pos.x, pos.y );
						pNPC->SetCurrentLayer( GetLayer() );

						pNPC->SetRoamer( this );
						pNPC->SetDeadHandler( this );

						ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pNPC ) );
						AddNPCToWorld( pNPC );

						// 이동 중이었어야 했다면 이동 처리
						if( IsMoving() )
						{
							ArPosition targetPos = getCurrentRespawnObjectPosition( GetCurrentRoamingTargetPosition(), fFace, (*it).m_nAngle, (*it).m_nDistance );
							ArcadiaServer::Instance().SetMove( pNPC, pos, targetPos, m_nMoveSpeed, false, 0 );
						}
					}
					break;

				default:
					// 뉘시오 -_ -?
					assert( 0 );
					break;
				}

				++it;
			}

			m_nNextRespawnProcTime = 0;
			m_nLastRegenCount = 0;
		}

		// 이동 처리
		if( IsMoving() )
			processWalk( t );
		else
			processRoaming( t, currentPos, fFace );
	}

	// Hate 공유 처리(m_csHate를 사용하므로 m_CS를 걸고 호출하면 안 됨)
	processHateSharing();
}

void StructRoamer::onMonsterDelete( struct StructMonster * pMonster )
{
	const bool bDeleteByTimer = pMonster->IsLifeTimeOver();
	bool bNeedToDeleteRoamer = false;

	{
		THREAD_SYNCHRONIZE( m_CS );

		for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
		{
			if( (*it).m_pCreature == pMonster )
			{
				// 몬스터는 processDead에서 ArcadiaServer::Instance().DeleteObject를 호출해 줌(NPC와의 일관성 따위... -_ -;)
				(*it).m_pCreature = NULL;
				// 레이드 던전 내에서 로밍 몬스터가 죽은 경우에는 리젠될 시각을 0으로 세팅하여 리젠 처리가 발생하지 않게 함(개별 리젠 설정의 그룹 로밍 레이드 몹 리젠 처리 방지)
				(*it).m_nNextRespawnTime = ( m_bIsRaidDungeonRoamer ) ? 0 : ( ( m_nRespawnInterval || !(*it).m_nRespawnInterval ) ? 0 : GetArTime() + (*it).m_nRespawnInterval );

				// 개별 리젠 처리 로밍 그룹에서 리젠 간격이 0으로 되어 있거나 타이머에 의해 삭제된 몬스터는 리젠 안 시킴
				if( ( !m_nRespawnInterval && !(*it).m_nRespawnInterval ) || bDeleteByTimer )
				{
					m_vRoamingCreatureRespawnInfo.erase( it );

					if( m_vRoamingCreatureRespawnInfo.empty() )
						bNeedToDeleteRoamer = true;
				}

				break;
			}
		}
	}

	if( bNeedToDeleteRoamer )
		DeInit( false );
}

void StructRoamer::onNPCDead( struct StructNPC * pNPC )
{
	bool bNeedToDeleteRoamer = false;

	{
		THREAD_SYNCHRONIZE( m_CS );

		for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
		{
			if( (*it).m_pCreature == pNPC )
			{
				ArcadiaServer::Instance().DeleteObject( (*it).m_pCreature );
				(*it).m_pCreature = NULL;
				(*it).m_nNextRespawnTime = ( m_nRespawnInterval || !(*it).m_nRespawnInterval ) ? 0 : GetArTime() + (*it).m_nRespawnInterval;

				// 개별 리젠 처리 로밍 그룹에서 리젠 간격이 0으로 되어 있으면 죽어도 리젠 안 시킴
				if( !m_nRespawnInterval && !(*it).m_nRespawnInterval )
				{
					m_vRoamingCreatureRespawnInfo.erase( it );

					if( m_vRoamingCreatureRespawnInfo.empty() )
						bNeedToDeleteRoamer = true;
				}

				break;
			}
		}
	}

	if( bNeedToDeleteRoamer )
		DeInit( false );
}

bool StructRoamer::IsMovable()
{
	THREAD_SYNCHRONIZE( m_CS );

	if( m_eRoamingStatus == ROAMING_STATUS_IDLE )
		return false;

	return isMovable();
}

void StructRoamer::PauseRoaming()
{
	THREAD_SYNCHRONIZE( m_CS );

	if( m_eRoamingStatus == ROAMING_STATUS_PAUSED )
	{
		assert( 0 );
		return;
	}

	AR_TIME t = GetArTime();
	ArPosition pos = GetCurrentPosition( t );
	float fFace = GetFace();

	m_eRoamingStatus = ROAMING_STATUS_PAUSED;

	StopMove();
	ArcadiaServer::Instance().SetMove( this, pos, pos, 0, false, 0, false );

	for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
	{
		StructCreature *pCreature = (*it).m_pCreature;

		if( !pCreature || !pCreature->IsInWorld() )
			continue;

		ArPosition returnPos = getCurrentRespawnObjectPosition( pos, fFace, (*it).m_nAngle, (*it).m_nDistance );

		if( pCreature->IsMonster() )
			static_cast< StructMonster * >( pCreature )->SetReturnPosition( returnPos.x, returnPos.y );
		else if( pCreature->IsNPC() )
			static_cast< StructNPC * >( pCreature )->SetReturnPosition( returnPos.x, returnPos.y );

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pCreature ) );
		pCreature->StopMove();
		ArcadiaServer::Instance().SetMove( pCreature, returnPos, returnPos, 0, false, 0 );
	}
}

const ArPosition StructRoamer::GetCurrentRoamingTargetPosition()
{
	return m_vRoamingPoint[ m_nCurrentRoamingPointIndex ];
}

const ArPosition StructRoamer::GetNextRoamingTargetPosition()
{
	size_t nNextRoamingTargetIndex = getNextRoamingTargetIndex();

	return m_vRoamingPoint[ nNextRoamingTargetIndex ];
}

void StructRoamer::PendHateShare( const AR_HANDLE hRequester, const AR_HANDLE hHateTarget, const int nHate, const int eApplyHateType )
{
	if( !( eApplyHateType & m_eHateType ) )
		return;

	THREAD_SYNCHRONIZE( m_csHate );

	m_vPendingHateInfo.push_back( new PENDING_HATE_SHARE_INFO( hRequester, hHateTarget, nHate ) );
}

const bool StructRoamer::isMovable()
{
	for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
	{
		StructCreature *pCreature = (*it).m_pCreature;

		if( !pCreature || !pCreature->IsInWorld() )
			continue;

		// 전투 중이거나, 이동 불가면 이동 불가
		if( pCreature->GetEnemyHandle() || !pCreature->IsMovable() )
			return false;
	}

	return true;
}

void StructRoamer::processWalk( AR_TIME t )
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

		ArcadiaServer::Instance().onRegionChange( this,
													t - lastStepTime,
													!tmp_mv.IsMoving() );
	}
}

void StructRoamer::processRoaming( AR_TIME t, const ArPosition & currentPos, const float & fFace )
{
	// 소속 객체가 전투 중이거나 제 위치에 멈춰 있지 않으면 아무 것도 안 함
	for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
	{
		StructCreature *pCreature = (*it).m_pCreature;

		if( pCreature && pCreature->IsInWorld() )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pCreature ) );

			if( pCreature->IsMoving() || pCreature->GetEnemyHandle() )
				return;

			// 이동 중도 전투 중도 아닌 놈이 좌표가 있어야 할 곳과 다르다면 이동하라고 시킴
			ArPosition posTarget = getCurrentRespawnObjectPosition( currentPos, fFace, (*it).m_nAngle, (*it).m_nDistance );
			ArPosition posCurrent = pCreature->GetCurrentPosition( t );
			if( !( posCurrent == posTarget ) )
			{
				ArcadiaServer::Instance().SetMove( pCreature, posCurrent, posTarget, m_nMoveSpeed, false, 0 );
				return;
			}
		}
	}

	// 일단 이동 가능한 상태이므로 이동 재개 처리만 하면 됨
	switch( m_eRoamingStatus )
	{
	case ROAMING_STATUS_ROAMING:
		{
			// 모두 현재 목표 지점으로 이동을 마친 상태이므로 다음 목표 지점을 향해 대형 방향 전환
			SetDirection( GetNextRoamingTargetPosition() );

			ArPosition pos = GetCurrentRoamingTargetPosition();
			float fNewFace = GetFace();

			for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
			{
				StructCreature *pCreature = (*it).m_pCreature;

				if( !pCreature || !pCreature->IsInWorld() )
					continue;

				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pCreature ) );

				// 락 거는 동안 월드에서 사라졌을 수 있음
				if( !pCreature->IsInWorld() )
					continue;

				ArcadiaServer::Instance().SetMove( pCreature, pCreature->GetPos(), getCurrentRespawnObjectPosition( pos, fNewFace, (*it).m_nAngle, (*it).m_nDistance ), m_nMoveSpeed * GameRule::ROAMING_ROATING_MOVE_SPEED_RATE, false, 0 );
			}

			m_eRoamingStatus = ROAMING_STATUS_ROTATING;
		}
		break;

	case ROAMING_STATUS_ROTATING:
		{
			// 대형 전환이 완료되었으므로 다음 목표 지점으로 이동 시작
			proceedRoamingTargetIndex();
		}
		// break 없이 이어서 일시 정지 상태에서 로밍 재시작과 동일한 처리

	case ROAMING_STATUS_PAUSED:
		{
			// 모두 이동 가능한 상태이므로 로밍 재시작
			ArPosition pos = GetCurrentRoamingTargetPosition();
			int nNeedToProcRespawnCount = 0;

			{
				ARCADIA_LOCK( ArcadiaServer::Instance().LockArea( GetPos().GetRX(), GetPos().GetRY(), pos.GetRX(), pos.GetRY() ) );

				ArcadiaServer::Instance().SetMove( this, GetPos(), pos, m_nMoveSpeed, false, 0, false );

				float fFace = GetFace();
				bool bNeedToRegenFullHPMP = m_AttributeFlag.IsOn( ATTRIBUTE_HEAL_HP_ON_COMEBACKHOME ) && m_nRespawnInterval;

				for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
				{
					StructCreature *pCreature = (*it).m_pCreature;

					if( !pCreature )
					{
						++nNeedToProcRespawnCount;
						continue;
					}

					// 등장 기간이 만료된 NPC가 월드에서만 삭제되고 개체 자체는 삭제 처리 대기 중일 수 있으므로 그냥 건너 뜀
					if( !pCreature->IsInWorld() )
					{
						continue;
					}

					int nMaxHP = pCreature->GetMaxHP();
					int nCurrentHP = pCreature->GetHP();
					if( bNeedToRegenFullHPMP && nCurrentHP < nMaxHP )
					{
						pCreature->RegenFullHPMP();
					}

					ArcadiaServer::Instance().SetMove( pCreature, pCreature->GetPos(), getCurrentRespawnObjectPosition( pos, fFace, (*it).m_nAngle, (*it).m_nDistance ), m_nMoveSpeed, false, 0 );
				}
			}

			// 레이드 던전 로밍 몬스터는 다시 리젠되지 않는다.
			if( m_nRespawnInterval && ( m_nLastRegenCount != nNeedToProcRespawnCount ) && !m_bIsRaidDungeonRoamer )
			{
				// 그룹형 리젠이면 마지막에 죽는 녀석의 리스폰 시각에 맞춰 리젠타임이 조정된다.
				// (예: 리젠 타임이 1시간 일 경우, 첫 번째 녀석이 0시에 죽고 두번째 녀석이 0시 30분에 죽었으면 다음 리젠 시각 1시 30분에 두마리 다 리젠.)
				m_nNextRespawnProcTime = t + m_nRespawnInterval;
				m_nLastRegenCount = nNeedToProcRespawnCount;
			}

			m_eRoamingStatus = ROAMING_STATUS_ROAMING;
		}
		break;


	default:
		break;
	}
}

void StructRoamer::processHateSharing()
{
	// m_csHate와 지역 락, m_CS 와의 데드락 발생 가능성을 배제하기 위해서 버퍼를 두고 복제된 내용만 락 안 걸고 처리해야 함
	// 어그로 공유 메시지 큐와 같은 개념이므로 처리 로직에는 문제 없음
	std::vector< PENDING_HATE_SHARE_INFO * > vHateProcBuffer;

	{
		THREAD_SYNCHRONIZE( m_csHate );

		if( m_vPendingHateInfo.empty() )
			return;

		vHateProcBuffer = m_vPendingHateInfo;
		m_vPendingHateInfo.clear();
	}

	{
		THREAD_SYNCHRONIZE( m_CS );

		for( std::vector< PENDING_HATE_SHARE_INFO * >::iterator itHate = vHateProcBuffer.begin() ; itHate != vHateProcBuffer.end() ; ++itHate )
		{
			PENDING_HATE_SHARE_INFO * pPendingHate = (*itHate);

			for( std::vector< ROAMING_CREATURE_INFO >::iterator it = m_vRoamingCreatureRespawnInfo.begin() ; it != m_vRoamingCreatureRespawnInfo.end() ; ++it )
			{
				if( !(*it).m_pCreature || (*it).m_pCreature->GetHandle() == pPendingHate->m_hRequester || (*it).m_pCreature->GetHandle() == pPendingHate->m_hHateTarget || !(*it).m_pCreature->IsMonster() || !(*it).m_pCreature->IsInWorld() || (*it).m_pCreature->IsDead() )
					continue;

				StructMonster *pMonster = static_cast< StructMonster * >( (*it).m_pCreature );

				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pMonster ) );

				if( !pMonster->IsInWorld() || pMonster->IsDead() )
					continue;

				// 멍하니 있던 애면 Hate주고 적을 인식시켜야 함
				bool bNeedToFindEnemy = pMonster->GetStatus() == StructMonster::STATUS_NORMAL;

				pMonster->AddHate( pPendingHate->m_hHateTarget, pPendingHate->m_nHate, true, false );

				if( bNeedToFindEnemy )
					pMonster->SetNeedToFindEnemy();
			}

			delete pPendingHate;
		}
	}
}

const size_t StructRoamer::getNextRoamingTargetIndex() const
{
	size_t nNextRoamingTargetIndex = 0;
	size_t nRoaminPointCount = m_vRoamingPoint.size();

	// 일단 로밍 타겟 지점이 1개 이하면 이동이 안 일어남 -_ -
	if( nRoaminPointCount <= 1 )
		return 0;

	// 전방 진행 중
	if( m_eCurrentRoamingDirection == ROAMING_DIRECTION_FORWARD )
	{
		// 최종 점을 초과하려는 경우
		if( m_nCurrentRoamingPointIndex >= nRoaminPointCount - 1 )
		{
			switch( m_eRoamingType )
			{
			case ROAMING_TYPE_ROUND:
				// 순환 타입(ROAMING_TYPE_ROUND)이면 0번 위치가 다음 목표 지점
				nNextRoamingTargetIndex = 0;
				break;

			case ROAMING_TYPE_GO_BACK:
				// 왕복 타입(ROAMING_TYPE_GO_BACK)이면 역방향의 다음 지점 지정
				nNextRoamingTargetIndex = nRoaminPointCount - 2;
				break;

			default:
				// 당췌 어떤 로밍을 하고 계신게요 -_ -?
				assert( 0 );
				break;
			}
		}
		// 문제 없으면 걍 다음 지점으로
		else
		{
			nNextRoamingTargetIndex = m_nCurrentRoamingPointIndex + 1;
		}
	}
	else if( m_eCurrentRoamingDirection == ROAMING_DIRECTION_BACKWARD )
	{
		// 시작 점을 초과하려는 경우
		if( m_nCurrentRoamingPointIndex == 0 )
		{
			switch( m_eRoamingType )
			{
			case ROAMING_TYPE_ROUND:
				// 순환 타입(ROAMING_TYPE_ROUND)인데 어떻게 BACKWARD로 진행하셨수 -_ -?
				assert( 0 );
				break;

			case ROAMING_TYPE_GO_BACK:
				// 왕복 타입(ROAMING_TYPE_GO_BACK)이면 순방향의 다음 지점 지정
				nNextRoamingTargetIndex = 1;
				break;

			default:
				// 당췌 어떤 로밍을 하고 계신게요 -_ -?
				assert( 0 );
				break;
			}
		}
		// 문제 없으면 걍 다음 지점으로
		else
		{
			nNextRoamingTargetIndex = m_nCurrentRoamingPointIndex - 1;
		}
	}

	assert( nNextRoamingTargetIndex >= 0 && nNextRoamingTargetIndex < nRoaminPointCount );

	return nNextRoamingTargetIndex;
}

void StructRoamer::proceedRoamingTargetIndex()
{
	size_t nNextRoamingTargetIndex = 0;
	size_t nRoaminPointCount = m_vRoamingPoint.size();

	// 일단 로밍 타겟 지점이 1개 이하면 이동이 안 일어남 -_ -
	if( nRoaminPointCount <= 1 )
		return;

	// 전방 진행 중
	if( m_eCurrentRoamingDirection == ROAMING_DIRECTION_FORWARD )
	{
		// 최종 점을 초과하려는 경우
		if( m_nCurrentRoamingPointIndex >= nRoaminPointCount - 1 )
		{
			switch( m_eRoamingType )
			{
			case ROAMING_TYPE_ROUND:
				// 순환 타입(ROAMING_TYPE_ROUND)이면 0번 위치가 다음 목표 지점
				nNextRoamingTargetIndex = 0;
				break;

			case ROAMING_TYPE_GO_BACK:
				// 왕복 타입(ROAMING_TYPE_GO_BACK)이면 역순 진행으로 변경
				m_eCurrentRoamingDirection = ROAMING_DIRECTION_BACKWARD;
				nNextRoamingTargetIndex = nRoaminPointCount - 2;
				break;

			default:
				// 당췌 어떤 로밍을 하고 계신게요 -_ -?
				assert( 0 );
				break;
			}
		}
		// 문제 없으면 걍 다음 지점으로
		else
		{
			nNextRoamingTargetIndex = m_nCurrentRoamingPointIndex + 1;
		}
	}
	else if( m_eCurrentRoamingDirection == ROAMING_DIRECTION_BACKWARD )
	{
		// 시작 점을 초과하려는 경우
		if( m_nCurrentRoamingPointIndex == 0 )
		{
			switch( m_eRoamingType )
			{
			case ROAMING_TYPE_ROUND:
				// 순환 타입(ROAMING_TYPE_ROUND)인데 어떻게 BACKWARD로 진행하셨수 -_ -?
				assert( 0 );
				break;

			case ROAMING_TYPE_GO_BACK:
				// 왕복 타입(ROAMING_TYPE_GO_BACK)이면 순 진행으로 변경
				m_eCurrentRoamingDirection = ROAMING_DIRECTION_FORWARD;
				nNextRoamingTargetIndex = 1;
				break;

			default:
				// 당췌 어떤 로밍을 하고 계신게요 -_ -?
				assert( 0 );
				break;
			}
		}
		// 문제 없으면 걍 다음 지점으로
		else
		{
			nNextRoamingTargetIndex = m_nCurrentRoamingPointIndex - 1;
		}
	}

	assert( nNextRoamingTargetIndex >= 0 && nNextRoamingTargetIndex < nRoaminPointCount );

	m_nCurrentRoamingPointIndex = nNextRoamingTargetIndex;
}

const ArPosition StructRoamer::getCurrentRespawnObjectPosition( const ArPosition & currentPos, const float & fFace, const int & nAngle, const AR_UNIT & nDistance )
{
	float fCreatureFace = fFace + static_cast< float >( nAngle ) * 3.141592f / 180;

	return ArPosition( currentPos.x - ( cos( fCreatureFace ) * nDistance ), currentPos.y - ( sin( fCreatureFace ) * nDistance ) );
}
