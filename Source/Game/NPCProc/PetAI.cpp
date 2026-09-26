
#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>

#include "StructPet.h"
#include "GameProc.h"
#include "StructPlayer.h"
#include "ThreadPlayerHelper.h"

//#include "GameContent.h"

void StructPet::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "StructPet" );

	AR_TIME t = GetArTime();

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "StructPet(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;
	ThreadPlayerHelper TPHelper( GetMaster() );

	if( !IsInWorld() ) 
	{
		return;
	}

	bool bIsMoving = ArObject::IsMoving();
	if( bIsMoving )
	{	
		// 움직이고 있으면 움직임 연산
		processWalk( t );
		m_nLastProcessTime = t;
	}

	// 삽질 중이면 이동 처리도 하고 스킬 처리도 함께 해야 함
	if( !bIsMoving || GetShovelingStatus() != SHOVELING_STATUS_IDLE )
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
		else if( m_nLastProcessTime + 500 < t )
		{	
			ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );
			m_nLastProcessTime = 0;
		}
	}

	// StructCreature를 상속받은 클래스는 모두 호출을 해야하지만 현재는 굳이 호출할 필요가 없다.
	// StructCreature::onProcess( nThreadIdx );
}

// 이동처리 - 이동중인 NPC 시간 t 만큼 가 REGION 을 옮길경우 통지해준다.
void StructPet::processWalk( AR_TIME t )
{
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
