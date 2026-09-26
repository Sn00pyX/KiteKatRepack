#include "RoamingManager.h"
#include "ChannelManager.h"
#include "DungeonManager.h"

#include <set>


RoamingManager & RoamingManager::Instance()
{
	static RoamingManager _instance;

	return _instance;
}

RoamingManager::~RoamingManager()
{
	// 초기화 되어 있지 않으면(또는 DeInit 되어 있으면) DeInit에서 아무 일도 안 함
	DeInit();
}

const bool RoamingManager::RegisterRoamingInfo( const int nID, const StructRoamer::ROAMING_TYPE eType, const int nMoveSpeed, const StructRoamer::HATE_TYPE eHateType, const AR_TIME nRespawnInterval, const int nAttributeFlag, int nInitialX, int ninitialY )
{
	if( m_bInitialized )
		return false;

	THREAD_SYNCHRONIZE( m_RoamerLock );

	for( std::vector< StructRoamer * >::iterator it = m_vRoamer.begin() ; it != m_vRoamer.end() ; ++it )
	{
		if( (*it)->GetRoamingID() == nID )
			return false;
	}

	int channel_id = ChannelManager::GetChannelId( nInitialX, ninitialY );

	std::vector< unsigned char > vLayers;
	int nDungeonID = 0;
	if( channel_id )
	{
		// 레이어가 나뉘는 채널들의 경우 모든 레이어에 모두 StructRoamer를 추가해야 하지만
		// 던전의 경우 던전 시즈 레이어에는 추가하면 안 됨
		ChannelManager::GetLayersOfChannel( channel_id, vLayers );

		nDungeonID = DungeonManager::Instance().GetDungeonID( nInitialX, ninitialY );
		if( nDungeonID )
		{
			std::vector< unsigned char >::iterator itErase = std::find( vLayers.begin(), vLayers.end(), DungeonManager::DUNGEON_SIEGE_LAYER );
			if( itErase != vLayers.end() )
			{
				vLayers.erase( itErase );
			}
		}
	}
	else
	{
		// 일반 필드는 0번 레이어만 추가
		vLayers.push_back( 0 );
	}

	for( std::vector< unsigned char >::iterator lit = vLayers.begin() ; lit != vLayers.end() ; ++lit )
	{
		unsigned char nLayer = (*lit);

		StructRoamer *pRoamer = new StructRoamer( nID, eType, nMoveSpeed, eHateType, nRespawnInterval, nAttributeFlag, nDungeonID && nLayer && nLayer != DungeonManager::DUNGEON_SIEGE_LAYER );
		pRoamer->SetCurrentLayer( *lit );

		m_vRoamer.push_back( pRoamer );
	}

	if( nDungeonID )
	{
		DungeonManager::Instance().RegisterDungeonRoamerInfo( nDungeonID, nID );
	}

	return true;
}

const bool RoamingManager::UnregisterRoamingInfo( const int nID )
{
	if( !m_bInitialized )
		return false;

	THREAD_SYNCHRONIZE( m_RoamerLock );

	for( std::vector< StructRoamer * >::iterator it = m_vRoamer.begin() ; it != m_vRoamer.end() ; ++it )
	{
		if( (*it)->GetRoamingID() == nID )
		{
			m_vRoamer.erase( it );
			return true;
		}
	}

	return false;
}

const bool RoamingManager::RegisterRoamingPointInfo( const int nRoamingID, const int nPosX, const int nPosY )
{
	THREAD_SYNCHRONIZE( m_RoamerLock );

	bool bSuccess = false;

	for( std::vector< StructRoamer * >::iterator it = m_vRoamer.begin() ; it != m_vRoamer.end() ; ++it )
	{
		if( (*it)->GetRoamingID() == nRoamingID )
		{
			(*it)->AddRoamingPoint( ArPosition( nPosX, nPosY ) );
			bSuccess = true;
			// break; 하지 않고 동일한 ID에 layer만 다른 StructRoamer를 모두 처리해야 함
		}
	}

	return bSuccess;
}

const size_t RoamingManager::GetRoamingPointInfoCount( const int nRoamingID )
{
	THREAD_SYNCHRONIZE( m_RoamerLock );

	size_t nCount = 0;

	// 동일한 RoamingID에 layer 만 다른 경우 중복으로 카운트되지 않도록 하기 위해 체크된 RoamingID를 보관
	std::set< int > stCheckedRoamingID;

	for( std::vector< StructRoamer * >::iterator it = m_vRoamer.begin() ; it != m_vRoamer.end() ; ++it )
	{
		int nCurrentRoamerID = (*it)->GetRoamingID();

		if( nRoamingID >= 0 )
		{
			// 특정 StructRoamer의 RoamingPoint 개수만 세는 경우
			if( nCurrentRoamerID == nRoamingID )
			{
				nCount += (*it)->GetRoamingPointCount();
				break;
			}
		}
		else
		{
			// 전체 StructRoamer의 RoamingPoint 개수를 세는 경우
			if( stCheckedRoamingID.find( nCurrentRoamerID ) == stCheckedRoamingID.end() )
			{
				nCount += (*it)->GetRoamingPointCount();
				stCheckedRoamingID.insert( nCurrentRoamerID );
			}
		}
	}

	return nCount;
}

const bool RoamingManager::AddRoamingCreatureRespawnInfo( const int nRoamingID, const GameContent::ROAMING_CREATURE_RESPAWN_INFO & info )
{
	THREAD_SYNCHRONIZE( m_RoamerLock );

	bool bSuccess = false;

	for( std::vector< StructRoamer * >::iterator it = m_vRoamer.begin() ; it != m_vRoamer.end() ; ++it )
	{
		if( (*it)->GetRoamingID() == nRoamingID )
		{
			(*it)->AddCreatureRespawnInfo( info );
			bSuccess = true;
			// break; 하지 않고 동일한 ID에 layer만 다른 StructRoamer를 모두 처리해야 함
		}
	}

	return bSuccess;
}

const bool RoamingManager::Init()
{
	if( m_bInitialized )
		return false;

	THREAD_SYNCHRONIZE( m_RoamerLock );

	if( m_bInitialized )
		return false;

	for( std::vector< StructRoamer * >::iterator it = m_vRoamer.begin() ; it != m_vRoamer.end() ; ++it )
	{
		if( !(*it)->GetRoamingPointCount() )
		{
			assert( 0 );
			continue;
		}

		// 던전 레이드용 로밍 객체는 초기화시키지 않음
		if( !(*it)->IsInitialized() && !(*it)->IsRaidDungeonRoamer() )
		{
			(*it)->Init();
		}
	}

	m_bInitialized = true;

	return true;
}

const bool RoamingManager::DeInit()
{
	if( !m_bInitialized )
		return false;

	THREAD_SYNCHRONIZE( m_RoamerLock );

	if( !m_bInitialized )
		return false;

	m_bInitialized = false;

	for( std::vector< StructRoamer * >::iterator it = m_vRoamer.begin() ; it != m_vRoamer.end() ; ++it )
	{
		(*it)->DeInit( true );
	}

	m_vRoamer.clear();

	return true;
}

void RoamingManager::InitRoamer( const int nRoamingID, const int nLayer )
{
	THREAD_SYNCHRONIZE( m_RoamerLock );

	if( !m_bInitialized )
		return;

	for( std::vector< StructRoamer * >::iterator it = m_vRoamer.begin() ; it != m_vRoamer.end() ; ++it )
	{
		if( (*it)->GetRoamingID() == nRoamingID && ( nLayer == -1 || (*it)->GetLayer() == nLayer ) )
			(*it)->Init();
	}

	return;
}

void RoamingManager::DeInitRoamer( const int nRoamingID, const int nLayer )
{
	THREAD_SYNCHRONIZE( m_RoamerLock );

	if( !m_bInitialized )
		return;

	for( std::vector< StructRoamer * >::iterator it = m_vRoamer.begin() ; it != m_vRoamer.end() ; ++it )
	{
		if( (*it)->GetRoamingID() == nRoamingID && ( nLayer == -1 || (*it)->GetLayer() == nLayer ) )
			(*it)->DeInit( false );
	}

	return;
}
