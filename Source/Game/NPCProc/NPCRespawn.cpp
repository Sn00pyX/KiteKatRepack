
#include <mmo/ArObject.h>
#include <mmo/ArTime.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XConsole.h>
#include <toolkit/XRandom.h>
#include <mmo/ArScheduler.h>
#include <toolkit/XEnv.h>

#include "extern.h"
#include "GameMessage.h"
#include "GameContent.h"
#include "GameProc.h"
#include "StructNPC.h"

struct NPCRespawnObject : ArSchedulerObject, StructNPC::NPCDeadHandler
{
	NPCRespawnObject()
		: m_CS( "NPCRespawnObject::m_CS" )
	{}

	virtual void onNPCDead( struct StructNPC * pNPC );

	void Init();

	bool ProcDelete()
	{
		delete this;
		return true;
	}

	void AddNPCRespawn( struct StructNPC * pNPC );

	void onProcess( int nThreadIdx );

	std::vector< StructNPC * > m_vNPCRespawnInfo;

	XCriticalSection m_CS;
};

static NPCRespawnObject s_NPCRespawnObject;

void NPCRespawnObject::Init()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );
}

void NPCRespawnObject::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "NPCRespawnObject" );

	AR_TIME t = GetArTime();

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "NPCRespawnObject(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	THREAD_SYNCRONIZE( &m_CS );

	if( m_vNPCRespawnInfo.empty() )
		return;

	time_t tCurrent = time( NULL );

	for( std::vector< StructNPC * >::iterator it = m_vNPCRespawnInfo.begin(); it != m_vNPCRespawnInfo.end();  )
	{
		const NPCBase * pNPCBase = (*it)->GetNPCBase();

		if( pNPCBase->is_periodic )
		{
			// 리젠 기간이 만료된 NPC가 리젠 대기 중이라면 삭제
			if( pNPCBase->end_of_period <= tCurrent )
			{
				ArcadiaServer::Instance().DeleteObject( (*it) );
				it = m_vNPCRespawnInfo.erase( it );
				continue;
			}

			// 리젠 기간이 아니면 패스
			if( pNPCBase->begin_of_period > tCurrent )
			{
				++it;
				continue;
			}
		}

		// 사망 이후 리젠까지 대기 시간 중이라면 패스
		if( (*it)->GetDeadTime() + pNPCBase->regen_time > t )
		{
			++it;
			continue;
		}

		(*it)->SetDeadHandler( this );
		(*it)->SetCurrentXY( pNPCBase->x, pNPCBase->y );
		(*it)->SetHP( (*it)->GetMaxHP() );
		
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( (*it) ) );
			(*it)->SetStatus( StructNPC::STATUS_NORMAL );
			AddNPCToWorld( (*it) );
		}

		it = m_vNPCRespawnInfo.erase( it );
	}
}

void InitNPCRespawnObject()
{
	s_NPCRespawnObject.Init();
}

void AddNPCToRespawnObject( StructNPC * pNPC )
{
	s_NPCRespawnObject.AddNPCRespawn( pNPC );
}

void NPCRespawnObject::AddNPCRespawn( struct StructNPC * pNPC )
{
	THREAD_SYNCRONIZE( &m_CS );

	m_vNPCRespawnInfo.push_back( pNPC );
}

void NPCRespawnObject::onNPCDead( struct StructNPC * pNPC )
{
	AddNPCRespawn( pNPC );
}

