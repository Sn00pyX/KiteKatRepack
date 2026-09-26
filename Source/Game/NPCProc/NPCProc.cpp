
#include <mmo/ArObject.h>
#include <mmo/ArTime.h>
#include <toolkit/XEnv.h>
#include <mmo/ArScheduler.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XConsole.h>
#include <toolkit/XRandom.h>

#include "StructMonster.h"
#include "extern.h"
#include "GameMessage.h"
#include "NPCProc.h"
#include "GameContent.h"
#include "GameProc.h"

volatile LONG g_nDropRespawn = 0;
volatile LONG g_nRespawnObjectCnt = 0;
volatile LONG g_nRespawnTryCnt = 0;
volatile LONG g_nRespawnCnt = 0;

struct RespawnObjectBase : ArSchedulerObject, StructMonster::MonsterDeleteHandler
{
	RespawnObjectBase( const char * name = "RespawnObjectBase::m_CS" )
		: m_CS( name )
	{
		InterlockedIncrement( &g_nRespawnObjectCnt );
		lastDeadTime = 0;
		m_nMaxRespawnNum = 0;
	}

	virtual GameContent::AREA_BOX getRegenPosition() = 0;
	virtual unsigned getMonsterID() const = 0;
	virtual GameContent::MONSTER_RESPAWN_BASE_INFO & getMonsterRespawnInfo() = 0;
	virtual bool isNeedToAdd() const = 0;
	virtual const char* getProcessName() const = 0;

	virtual void onMonsterDelete( struct StructMonster * pMonster );
	virtual void onProcess( int nThreadIdx );
	virtual bool ProcDelete()
	{
		delete this;
		return true;
	}

	unsigned int				m_nMaxRespawnNum;		// 동적으로 조절되는 최대 몹 리젠 수
	std::vector< AR_HANDLE >	m_vRespawnedMonster;	// 리젠되어 있는 몬스터 핸들(리젠되어 있는 놈을 감소처리 하기 위함)

	AR_TIME						lastDeadTime;
	XCriticalSection			m_CS;
};

struct RespawnObject : RespawnObjectBase
{
	RespawnObject( const GameContent::MONSTER_RESPAWN_INFO & rh )
		: info( rh )
		, RespawnObjectBase( "RespawnObject::m_CS" )
	{
		m_nMaxRespawnNum = info.prespawn_count;
	}

	virtual GameContent::AREA_BOX getRegenPosition();
	virtual unsigned getMonsterID() const;
	virtual GameContent::MONSTER_RESPAWN_BASE_INFO & getMonsterRespawnInfo();
	virtual bool isNeedToAdd() const;
	virtual const char* getProcessName() const;

	GameContent::MONSTER_RESPAWN_INFO			info;
};

struct RandomRespawnObject : RespawnObjectBase
{
	RandomRespawnObject( const GameContent::RANDOM_MONSTER_RESPAWN_INFO & rh )
		: info( rh )
		, RespawnObjectBase( "RandomRespawnObject::m_CS" )
	{
		m_nMaxRespawnNum = info.prespawn_count;
	}

	virtual GameContent::AREA_BOX getRegenPosition();
	virtual unsigned getMonsterID() const;
	virtual GameContent::MONSTER_RESPAWN_BASE_INFO & getMonsterRespawnInfo();
	virtual bool isNeedToAdd() const;
	virtual const char* getProcessName() const;

	GameContent::RANDOM_MONSTER_RESPAWN_INFO	info;
};

StructMonster * respawnMonster( unsigned x, unsigned y, unsigned char layer, unsigned id, bool is_wandering, int way_point_id, StructMonster::MonsterDeleteHandler * pDeleteHandler, bool bNeedLock, unsigned char difficulty, float face )
{
	// 몬스터 생성
	StructMonster *pMob = StructMonster::AllocMonster( id, difficulty );

	if( !pMob )
	{
		return NULL;
	}
	
	// 생성코드 설정
	pMob->SetGenerateCode( StructMonster::BY_RESPAWN );

	// 좌표 설정
	pMob->SetCurrentXY( x, y );
	pMob->SetCurrentLayer( layer );
	pMob->SetFace( face );

	pMob->SetWandering( is_wandering );

	// 리스폰 갯수 설정
	pMob->SetDeleteHandler( pDeleteHandler );

#ifndef _USE_NEW_ROAMING_ONLY
	if( way_point_id )
	{
		pMob->SetWayPointInfo( GameContent::GetWayPoint( way_point_id ) );
	}
#endif

	// lock 을 한 후 몬스터 추가

	ArcadiaAutoLock lock;

	if( bNeedLock )
		lock.set( ArcadiaServer::Instance().LockObjectWithVisibleRange( pMob ), __FILE__, __LINE__ );

	AddMonsterToWorld( pMob );

	if( way_point_id )
	{
		ArcadiaServer::Instance().SetObjectPriority( pMob, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );
	}

	InterlockedIncrement( &g_nRespawnCnt );

	return pMob;
}

void RespawnObjectBase::onMonsterDelete( struct StructMonster * pMonster )
{
	THREAD_SYNCRONIZE( &m_CS );

	GameContent::MONSTER_RESPAWN_BASE_INFO & info = getMonsterRespawnInfo();

	assert( info.count == m_vRespawnedMonster.size() );
	assert( info.count > 0 );

	lastDeadTime = GetArTime();

	bool bNeedToStartRespawn = ( info.count == m_nMaxRespawnNum );

	--info.count;

	// 리젠되어 있는 몬스터 리스트에서 해당 몬스터를 삭제
	std::vector< AR_HANDLE >::iterator itErase = std::find( m_vRespawnedMonster.begin(), m_vRespawnedMonster.end(), pMonster->GetHandle() );
	if( itErase != m_vRespawnedMonster.end() )
	{
		m_vRespawnedMonster.erase( itErase );
	}

	// 시간제로 소환되었던 몬스터가 시간 제한에 의해 사라지는 경우에는 리젠 처리를 하지 않음
	if( pMonster->IsLifeTimeOver() )
	{
		return;
	}

	// 몹이 한 마리 죽을 때마다 동적으로 최대 리젠 몹 수를 증가시킴
	if( isNeedToAdd() )
	{
		++m_nMaxRespawnNum;
	}

	if( bNeedToStartRespawn )
	{
		ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );
	}
}

void RespawnObjectBase::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, getProcessName() );

	AR_TIME t = GetArTime();

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "%s(0x%08X)", getProcessName(), (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	THREAD_SYNCRONIZE( &m_CS );

	GameContent::MONSTER_RESPAWN_BASE_INFO & info = getMonsterRespawnInfo();

	// 리젠 처리를 해야 하는 경우
	if( info.count < m_nMaxRespawnNum )
	{
		InterlockedIncrement( &g_nRespawnTryCnt );

		// 리스폰 인터벌이 안 지났으면 아무 것도 안 하고 끝남
		AR_TIME check_time = lastDeadTime + info.interval;
		if( lastDeadTime && check_time > t )
			return;

		int inc_cnt = std::min( m_nMaxRespawnNum - info.count, info.inc );

		bool bFirstRegen = false;
		// 첫 실행일경우
		if( !lastDeadTime ) 
		{
			lastDeadTime = t;
			inc_cnt = m_nMaxRespawnNum;
			bFirstRegen = true;
		}

		if( inc_cnt > 0 )
		{
			unsigned try_cnt = 0;

			// { TODO : inc_cnt 만큼 몬스터 생성.
			for( int i = 0 ; i < inc_cnt ; ++i )
			{
				GameContent::AREA_BOX regenBox = getRegenPosition();
				int x, y;
				int monster_id = getMonsterID();

				do
				{
					x = XRandom( regenBox.left, regenBox.right );
					y = XRandom( regenBox.top, regenBox.bottom );

					if( bFirstRegen && i == 0 && ++try_cnt > 500 )
					{
						InterlockedIncrement( &g_nDropRespawn );
						// 포기!
						ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );
						ArcadiaServer::Instance().DeleteObject( this );
						FILELOG( "Unable to respawn monster group. RespawnID(%d), MonsterID(%d), box(%d,%d-%d,%d)", info.id, monster_id, (int)regenBox.left, (int)regenBox.top, (int)regenBox.right, (int)regenBox.bottom );
						_cprint( "Unable to respawn monster group. RespawnID(%d), MonsterID(%d), box(%d,%d-%d,%d)\n", info.id, monster_id, (int)regenBox.left, (int)regenBox.top, (int)regenBox.right, (int)regenBox.bottom );

						return;
					}
				} while( GameContent::IsBlocked( x, y ) );
				
				StructMonster * pMob = respawnMonster( x, y, info.layer, monster_id, info.is_wandering, info.way_point_id, this );

				// Arcadia DB에 없는 몬스터 ID가 지정된 경우에는 pMob == NULL이 됨
				if( pMob )
				{
					if( info.dungeon_id )
					{
						pMob->SetDungeonId( info.dungeon_id );
					}

					m_vRespawnedMonster.push_back( pMob->GetHandle() );
				}

				++info.count;
			}
			// }
		}

		// 최대 리젠 수 만큼 리젠되었으면 priority를 낮춰서 몬스터 수 감소 처리를 하거나 놀도록 변경
		if( info.count == m_nMaxRespawnNum )
		{
			// 리젠되어 있는 몬스터 감소 처리가 불필요한 경우에는 IDLE 상태로 바꿈
			// 1 마리만 리젠, 던전 몬스터, 로밍 몬스터의 경우, 감소되어야 하는 수치만큼만 리젠된 경우
			if( m_nMaxRespawnNum == 1 || m_nMaxRespawnNum == info.prespawn_count || info.dungeon_id || info.way_point_id )
			{
				ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );
			}
			else
			{
				ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_LOW );
			}
		}
	}
	// 리젠되어 있는 몬스터 제거 처리를 해야하는 경우(던전 몬스터, 싱글 로밍 몬스터 리젠은 제외)
	else if( !info.dungeon_id && !info.way_point_id )
	{
		// 줄일 필요가 없다면 IDLE 상태로 변경만 함
		if( info.prespawn_count == m_nMaxRespawnNum )
		{
			ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );
			return;
		}

		// 현재 리젠되어 있는 몬스터 수가 최소 리젠 수보다 적다면 버그 있음
		if( m_nMaxRespawnNum < info.prespawn_count )
		{
			assert( 0 );

			// 다음 onProcess 들어올 때 최소 허용 수와 차이 만큼을 리젠되도록 조정함
			m_nMaxRespawnNum = info.prespawn_count;

			ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

			return;
		}

		// 마지막으로 몬스터가 리젠된 후로 지정 시간이 지났으면 몬스터의 수를 최소 리젠 수 만큼으로 줄임
		if( !lastDeadTime || lastDeadTime + info.interval + GameRule::MONSTER_RESPAWN_DECREASE_INTERVAL > t )
			return;

		// XMemoryPool 구조상 나중에 리젠된 놈이 삭제되는 게 메모리 풀 내에서 사용되는 블럭의 밀집도를 유지시키는데 유리함
		unsigned int nCountToDelete = m_nMaxRespawnNum - info.prespawn_count;
		while( nCountToDelete )
		{
			StructCreature::iterator pit = StructCreature::get( m_vRespawnedMonster.back() );
			StructCreature * pCreature = (*pit);

			StructMonster * pMonster = ( pCreature && pCreature->IsMonster() ) ? static_cast< StructMonster * >( pCreature ) : NULL;

			// 없거나 잘못된 핸들 값일 경우, 사망 처리 대기 중인 경우, 다른 놈한테 리젠된 놈인 면 버그에 의한 것임
			if( !pMonster || pMonster->GetDeleteHandler() != this )
			{
				assert( 0 );

				--m_nMaxRespawnNum;
				--info.count;

				--nCountToDelete;

				m_vRespawnedMonster.pop_back();
				continue;
			}

			// 몬스터 주변에 클라이언트가 있으면 중지(다음 프로세스에서 다시 체크)
			if( pMonster->bIsNearClient )
			{
				break;
			}

			// 몬스터 리젠해야 할 수와 현재 리젠되어 있는 수 감소 처리(이 이후에 잘못되는 경우는 발생하면 안 됨)
			--m_nMaxRespawnNum;
			--info.count;

			// 줄여야 할 몬스터 수 감소 처리(루프 탈출 조건)
			--nCountToDelete;

			m_vRespawnedMonster.pop_back();

			{
				ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pMonster ) );

				// 몬스터 사망 처리 중이거나 처리된 놈은 아무것도 하지 않음
				if( pMonster->IsEnable() )
				{
					pMonster->SetDeleteHandler( NULL );

					// 더이상의 스케쥴러 요청을 무시
					pMonster->Disable();

					// 월드에서 제거한다.
					if( pMonster->IsInWorld() )
					{
						RemoveMonsterFromWorld( pMonster );
					}

					// object delete 요청
					ArcadiaServer::Instance().DeleteObject( pMonster );
				}
				else
				{
					// 사망 처리 중인 놈은 지역 락이 걸려 있었어야 하므로 여기 들어올 수 없어야 정상
					assert( 0 );
				}
			}
		}

		// 몬스터 줄일 만큼 줄였으면 프로세스 돌지 않도록 IDLE 상태로 변경(전투 중인 몬스터가 있고 대신 없앨 놈이 없었다면 다음 프로세스 때 다시 체크해야 함)
		if( !nCountToDelete )
		{
			ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );
		}
	}
}

unsigned RespawnObject::getMonsterID() const
{
	return info.monster_id;
}

GameContent::MONSTER_RESPAWN_BASE_INFO & RespawnObject::getMonsterRespawnInfo()
{
	return static_cast< GameContent::MONSTER_RESPAWN_BASE_INFO & >(info);
}

bool RespawnObject::isNeedToAdd() const
{
	return m_nMaxRespawnNum < info.max_num;
}

const char *RespawnObject::getProcessName() const
{
	return "RespawnObject";
}

GameContent::AREA_BOX RespawnObject::getRegenPosition()
{
	return GameContent::AREA_BOX( info.left, info.top, info.right, info.bottom );
}

static std::vector< RespawnObjectBase* > & GetRespawnList()
{
	static std::vector< RespawnObjectBase* > vRegenList;
	return vRegenList;
}

void AddRespawnObject( const GameContent::MONSTER_RESPAWN_INFO & info )
{
	if( !info.monster_id ) return;

	// 새 영역정보 생성
	RespawnObject * pRespawn = new RespawnObject( info );
	GetRespawnList().push_back( pRespawn );

	// 스케쥴러에 등록
	ArcadiaServer::Instance().SetObjectPriority( pRespawn, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	return;
}

void AddRandomRespawnObject( const GameContent::RANDOM_MONSTER_RESPAWN_INFO & info )
{
	if( !info.random_area_id || info.monster_list.empty() ) return;

	// 새 영역정보 생성
	RandomRespawnObject * pRespawn = new RandomRespawnObject( info );
	GetRespawnList().push_back( pRespawn );

	// 스케쥴러에 등록
	ArcadiaServer::Instance().SetObjectPriority( pRespawn, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	return;
}

unsigned RandomRespawnObject::getMonsterID() const
{
	int nCum = 0;
	int nKey = XRandom( 1, 100000000 );

	for( std::vector< std::pair< int, int > >::const_iterator it = info.monster_list.begin(); it != info.monster_list.end(); ++it )
	{
		if( !it->first ) break;

		nCum += it->second;

		if( nKey > nCum ) continue;

		return it->first;
	}

	return 0;
}

GameContent::MONSTER_RESPAWN_BASE_INFO & RandomRespawnObject::getMonsterRespawnInfo()
{
	return static_cast< GameContent::MONSTER_RESPAWN_BASE_INFO & >(info);
}

bool RandomRespawnObject::isNeedToAdd() const
{
	return false;
}

const char *RandomRespawnObject::getProcessName() const
{
	return "RandomRespawnObject";
}

GameContent::AREA_BOX RandomRespawnObject::getRegenPosition()
{
	return GameContent::GetRandomRespawnBox( info.random_area_id );
}