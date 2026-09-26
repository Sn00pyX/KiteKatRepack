
#include <toolkit/XSTLUtil.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>
#include <dump/XExceptionHandler.h>

#include "FieldPropManager.h"
#include "ChannelManager.h"


FieldPropManager::~FieldPropManager()
{
	for( std::vector< GameContent::FIELD_PROP_RESPAWN_INFO * >::const_iterator it = m_vRespawnInfo.begin() ; it != m_vRespawnInfo.end() ; ++it )
	{
		delete (*it);
	}
}

const FieldPropBase & FieldPropManager::GetFieldPropBase( int nPropId )
{
	static FieldPropBase propBase;

	size_t idx = 0;

	if( !m_hsFieldPropId.lookup( nPropId, idx ) )
	{
		assert( 0 );
		return propBase;
	}

	return m_vFieldPropBase[idx];
}

void FieldPropManager::RegisterFieldPropBase( const FieldPropBase & base )
{
	// 이미 존재하면 수정
	if( m_hsFieldPropId.has( base.nPropId ) )
	{
		size_t idx;
		m_hsFieldPropId.lookup( base.nPropId, idx );
		m_vFieldPropBase[idx] = base;

		return;
	}

	m_vFieldPropBase.push_back( base );

	m_hsFieldPropId.add( base.nPropId, m_vFieldPropBase.size()-1 );
}

void FieldPropManager::onFieldPropDelete( struct StructFieldProp * pProp )
{
	if( pProp->IsExpireObject() )
	{
		THREAD_SYNCRONIZE( m_ExpireCS );

		std::vector< struct StructFieldProp * >::iterator it;

		for( it = m_vExpireObject.begin(); it != m_vExpireObject.end(); ++it )
		{
			if( (*it) == pProp )
			{
				vector_fast_erase( &m_vExpireObject, it );
				break;
			}
		}
	}

	{
		THREAD_SYNCRONIZE( m_CS );

		m_vRespawnList.push_back( GameContent::REGEN_INFO( pProp->GetRespawnInfo(), GetArTime() + pProp->GetFieldPropBase()->nRegenTime ) );
	}
}


void FieldPropManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "FieldPropManager" );

	AR_TIME t = GetArTime();

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "FieldPropManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	std::vector< GameContent::REGEN_INFO > vRegenInfo;

	{
		THREAD_SYNCRONIZE( m_CS );

		// 서버 최초 로딩 시 수 많은 프랍 리젠 정보가 대부분 push_back/erase에 의해 옮겨지는 증상이 발생하여
		// 처리 시간이 상당히 오리 걸리는 증상이 보여 기본적으로 리젠 대상 벡터(vRegenInfo)로 다 옮기고
		// 리젠되지 않을 놈들만 다시 대기 리스트(m_vRegenInfo)로 옮기도록 함
		vRegenInfo.swap( m_vRespawnList );

		for( std::vector< GameContent::REGEN_INFO >::iterator it = vRegenInfo.begin(); it != vRegenInfo.end(); )
		{
			if( (*it).tNextRegen < t )
			{
				++it;
			}
			else
			{
				m_vRespawnList.push_back( *it );

				it = vRegenInfo.erase( it );
			}
		}
	}

	for( std::vector< GameContent::REGEN_INFO >::const_iterator it = vRegenInfo.begin(); it != vRegenInfo.end(); ++it )
	{
		// 본래 ArcadiaServer::Lock을 이용하려면 대상이 워프되었는지 확인을 위해 Lock 이후에 좌표를
		// 한 번 검사해야 하지만(LockObjectWithVisibleRange 구현 참조) 프랍은 움직일 일이 없으므로 무시
		ARCADIA_LOCK( ArcadiaServer::Instance().LockWithVisibleRange( GetRegionX( (*it).pRespawnInfo->x ), GetRegionY( (*it).pRespawnInfo->y ), (*it).pRespawnInfo->layer ) );

		StructFieldProp * pProp = StructFieldProp::Create( this, (*it).pRespawnInfo );

		if( pProp->IsExpireObject() )
		{
			THREAD_SYNCRONIZE( m_ExpireCS );

			m_vExpireObject.push_back( pProp );
		}
	}

	std::vector< StructFieldProp * > vDeleteList;
	{
		THREAD_SYNCRONIZE( m_ExpireCS );

		for( std::vector< StructFieldProp * >::const_iterator it = m_vExpireObject.begin(); it != m_vExpireObject.end(); ++it )
		{
			if( (*it)->GetRegenTime() + (*it)->GetFieldPropBase()->nLifeTime < t )
			{
				vDeleteList.push_back( (*it) );
			}
		}
	}

	if( vDeleteList.size() )
	{
		for( std::vector< StructFieldProp * >::iterator it = vDeleteList.begin(); it != vDeleteList.end(); ++it )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockWithVisibleRange( (*it)->GetRX(), (*it)->GetRY(), (*it)->GetLayer() ) );

			if( (*it)->IsInWorld() && !(*it)->bIsDeleteRequested )
			{
				// 무조건 월드에 리젠되었고 그 후에 프랍 유효 시간(life_time)이
				// 만료되서 바로 삭제되는 경우에 들어오므로 RemoveObject도 함께 해야 함
				ArcadiaServer::Instance().RemoveObject( (*it) );
				StructFieldProp::PendFreeFieldProp( (*it) );
			}
		}
	}
}

void FieldPropManager::RegisterFieldProp( const GameContent::FIELD_PROP_RESPAWN_INFO & prop )
{
	THREAD_SYNCRONIZE( m_CS );

	int nPropId = prop.nPropId;

	// 필드 프랍 바꿔치기 적용
	for( std::vector< FieldPropSwitchingData >::iterator it = m_vFieldPropSwitching.begin() ; it != m_vFieldPropSwitching.end() ; ++it )
	{
		if( (*it).prop_id == prop.nPropId )
		{
			nPropId = (*it).switched_prop_id;
			break;
		}
	}

	// 국가별 프랍 숨김 설정 적용
	extern volatile int g_nCurrentLocalFlag;
	if( GetFieldPropBase( nPropId ).nLocalFlag & g_nCurrentLocalFlag )
	{
		return;
	}

	int channel_id = ChannelManager::GetChannelId( prop.x, prop.y );

	std::vector< unsigned char > vLayers;
	ChannelManager::GetLayersOfChannel( channel_id, vLayers );

	for( std::vector< unsigned char >::iterator lit = vLayers.begin(); lit != vLayers.end(); ++lit )
	{
		GameContent::FIELD_PROP_RESPAWN_INFO * pPropInfo = new GameContent::FIELD_PROP_RESPAWN_INFO( prop );

		pPropInfo->nPropId = nPropId;
		pPropInfo->layer = (*lit);

		m_vRespawnInfo.push_back( pPropInfo );
		m_vRespawnList.push_back( GameContent::REGEN_INFO( pPropInfo, GetArTime() + GetFieldPropBase( pPropInfo->nPropId ).nRegenTime )  );
	}
}

void FieldPropManager::RegisterFieldPropSwitchingData( const FieldPropSwitchingData & switching )
{
	m_vFieldPropSwitching.push_back( switching );
}

FieldPropManager & FieldPropManager::GetInstance()
{
	static FieldPropManager _inst;
	return _inst;
}


bool FieldPropManager::Init()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	return true;
}

bool FieldPropManager::DeInit()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );

	return true;
}
