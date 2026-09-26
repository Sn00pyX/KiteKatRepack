
#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>

#include "StructSummon.h"
#include "GameProc.h"
#include "StructPlayer.h"
#include "ThreadPlayerHelper.h"
#include "SchedulingPerformanceTracker.h"

void StructSummon::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "StructSummon" );

	SchedulingPerformanceHelper helper;
	if( GameRule::bLogSchedulingStatus )
	{
		helper.start();
	}

	AR_TIME t = GetArTime();

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "StructSummon(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;
	ThreadPlayerHelper TPHelper( GetMaster() );

	if( !IsInWorld() ) 
	{
		return;
	}

	// 죽은 녀석
	if( IsDead() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( GetMaster() ) );

		if( GetDeadTime() + GameRule::GetDeadSummonHoldTime() < t )
		{
			if( IsEnable() )
			{
				ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );

				// 월드에서 제거한다.
				GetMaster()->PendUnSummon( this );
				//RemoveSummonFromWorld( this );
			}
		}

		return;
	}

	if( ArObject::IsMoving() )
	{	
		// 움직이고 있으면 움직임 연산
		processWalk( t );
		m_nLastProcessTime = t;
	}
	else
	{
		// 전투 처리	
		if( IsAttacking() || IsUsingSkill() )
		{
			onAttackAndSkillProcess();
			m_nLastProcessTime = t;
		}
		else
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );

			if( !IsInWorld() )
			{
				return;
			}

			if( HasPendingMove() )
			{
				processPendingMove();
				m_nLastProcessTime = t;
			}
			else if( m_nLastProcessTime == 0 )
			{
				m_nLastProcessTime = t;
			}
			else if( m_nLastProcessTime + 500 < t && m_nLastStateProcTime + 500 < t )
			{	
				ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );
				m_nLastProcessTime = 0;
			}
		}
	}

	StructPlayer *pMaster = GetMaster();
	StructPlayer::iterator itMaster = pMaster;
	if( pMaster && pMaster->IsInWorld() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( this, pMaster ) );

		const ParameterForSummon & ParameterForSummon = pMaster->GetParameterForSummon();

		if( memcmp( &m_ParameterForSummon, &ParameterForSummon, sizeof m_ParameterForSummon ) )
		{
			m_ParameterForSummon = pMaster->GetParameterForSummon();
			SetNeedCalculateStat();
		}
	}

	StructCreature::onProcess( nThreadIdx );
	if( GameRule::bLogSchedulingStatus )
	{
		helper.end();
		g_SummonPerformanceTracker.addUpThisResult( helper );
	}
}

// 이동처리 - 이동중인 NPC 시간 t 만큼 가 REGION 을 옮길경우 통지해준다.
void StructSummon::processWalk( AR_TIME t )
{
	if( GetMaster()->GetRideObject() == this )	// 얘는 플레이어가 업데이트 쳐 준다.
		return;
	
	ArMoveVector tmp_mv;
	// 시간만큼 이동해 본다.
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( this ) );
		tmp_mv = GetMv();
	}
	tmp_mv.Step( t );

	//_oprint( "SR : (%d,%d)->(%d,%d) %d\n", (int)mv.x, (int)mv.y, (int)tmp_mv.x, (int)tmp_mv.y, tmp_mv.IsMoving( t ) );
		
	// 만약 이동했는데 Region 변경이 일어났거나 혹은 이동이 멈추었다면
	// 그것을 ArcadiaServer 에게 통지해준다.
	if( tmp_mv.GetRX() != GetRX() || tmp_mv.GetRY() != GetRY() || !tmp_mv.IsMoving() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithSpecificRegion( this, tmp_mv.GetRX(), tmp_mv.GetRY() ) );

		if( IsMoving() && IsInWorld() )
		{
			ArcadiaServer::Instance().onRegionChange( this,
													t - lastStepTime,
													!tmp_mv.IsMoving( t ) );
		}
	}
}