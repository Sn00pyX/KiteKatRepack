
#include <ctime>

#include <toolkit/ILock.h>
#include <toolkit/XRandom.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>

#include "StructWorldLocation.h"
#include "StructPlayer.h"
#include "GameMessage.h"
#include "SendMessage.h"


static XCriticalSection s_LocationCS;

unsigned int GetCurrentTimeIdx()
{
	time_t t = time( NULL );

	struct tm local_tm;
	localtime_s( &local_tm, &t );

	if( local_tm.tm_hour >= 12 )
		local_tm.tm_hour -= 12;

	if( local_tm.tm_hour >= 4 && ( local_tm.tm_hour < 8 || ( local_tm.tm_hour == 8 && local_tm.tm_min < 30 ) ) )
	{
		return StructWorldLocation::TIME_DAYTIME;
	}

	if( local_tm.tm_hour < 4 && ( local_tm.tm_hour > 2 || ( local_tm.tm_hour == 2 && local_tm.tm_min >= 30 ) ) )
	{
		return StructWorldLocation::TIME_DAWN;
	}

	if( local_tm.tm_hour < 10 && ( local_tm.tm_hour > 8 || ( local_tm.tm_hour == 8 && local_tm.tm_min >= 30 ) ) )
	{
		return StructWorldLocation::TIME_EVENING;
	}
	
	return StructWorldLocation::TIME_NIGHT;
}

WorldLocationManager::WorldLocationManager()
{
}

WorldLocationManager::~WorldLocationManager()
{
}

bool WorldLocationManager::Init()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_LOW );

	return true;
}

bool WorldLocationManager::DeInit()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );

	return true;
}

WorldLocationManager & WorldLocationManager::Instance()
{
	static WorldLocationManager _inst;
	return _inst;
}

void WorldLocationManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "WorldLocationManager" );

	AR_TIME t = GetArTime();

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "WorldLocationManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	unsigned time_idx = GetCurrentTimeIdx();

	THREAD_SYNCRONIZE( &s_LocationCS );

	std::vector< StructWorldLocation >::iterator it;

	for( it = m_vWorldLocation.begin(); it != m_vWorldLocation.end(); ++it )
	{
		if( !(*it).last_changed_time || (*it).last_changed_time + (*it).weather_change_time < t )
		{
			(*it).last_changed_time = t;

			unsigned current_weather = (*it).current_weather;

			unsigned char ratio = XRandom( 0, 99 );

			for( unsigned i = 0; i < StructWorldLocation::WEATHER_MAX; ++i )
			{
				if( ratio < (*it).weather_ratio[time_idx][i] )
				{
					(*it).current_weather = i;

					if( (*it).current_weather != current_weather )
					{
						// broadcasting
						std::vector< StructPlayer * >::iterator itPlayer;

						TS_SC_WEATHER_INFO msg;

						msg.region_id = (*it).idx;
						msg.weather_id = (*it).current_weather;

						for( itPlayer = (*it).m_vIncludeClient.begin(); itPlayer != (*it).m_vIncludeClient.end(); ++itPlayer )
						{
							PendMessage( (*itPlayer), &msg );
						}
					}

					break;
				}

				ratio -= (*it).weather_ratio[time_idx][i];
			}
		}
	}
	

}

const StructWorldLocation * WorldLocationManager::AddToLocation( unsigned int idx, struct StructPlayer * pPlayer )
{
	if( pPlayer->GetLocationId() )
	{
		RemoveFromLocation( pPlayer );
	}

	std::vector< StructWorldLocation >::iterator it;

	THREAD_SYNCRONIZE( &s_LocationCS );

	for( it = m_vWorldLocation.begin(); it != m_vWorldLocation.end(); ++it )
	{
		if( (*it).idx == idx )
		{
			(*it).m_vIncludeClient.push_back( pPlayer );

			TS_SC_WEATHER_INFO msg;

			msg.region_id = idx;
			msg.weather_id = (*it).current_weather;

			PendMessage( pPlayer, &msg );

			return &(*it);
		}
	} 

	return NULL;
}

bool WorldLocationManager::RemoveFromLocation( struct StructPlayer * pPlayer )
{
	std::vector< StructWorldLocation >::iterator it;

	THREAD_SYNCRONIZE( &s_LocationCS );

	for( it = m_vWorldLocation.begin(); it != m_vWorldLocation.end(); ++it )
	{
		if( (*it).idx == pPlayer->GetLocationId() )
		{
			std::vector< struct StructPlayer * >::iterator itPlayer;

			for( itPlayer = (*it).m_vIncludeClient.begin(); itPlayer != (*it).m_vIncludeClient.end(); ++itPlayer )
			{
				if( (*itPlayer) == pPlayer )
				{
					(*it).m_vIncludeClient.erase( itPlayer );
					break;
				}
			}

			return true;
		}
	}

	return false;
}

bool WorldLocationManager::SendWeatherInfo( unsigned int idx, struct StructPlayer * pPlayer )
{
	std::vector< StructWorldLocation >::iterator it;

	THREAD_SYNCRONIZE( &s_LocationCS );

	for( it = m_vWorldLocation.begin(); it != m_vWorldLocation.end(); ++it )
	{
		if( (*it).idx == idx )
		{
			TS_SC_WEATHER_INFO msg;

			msg.region_id = idx;
			msg.weather_id = (*it).current_weather;

			PendMessage( pPlayer, &msg );

			return true;
		}
	}

	return false;
}

const unsigned char WorldLocationManager::GetLocationType( const unsigned int idx )
{
	std::vector< StructWorldLocation >::iterator it;

	THREAD_SYNCRONIZE( &s_LocationCS );

	for( it = m_vWorldLocation.begin(); it != m_vWorldLocation.end(); ++it )
	{
		if( (*it).idx == idx )
			return (*it).location_type;
	}

	return StructWorldLocation::LOCATION_ETC;
}

const ItemBase::ItemCode WorldLocationManager::GetShovelableItem( const unsigned int idx )
{
	ItemBase::ItemCode nCode = 0;

	THREAD_SYNCRONIZE( &s_LocationCS );

	for( std::vector< StructWorldLocation >::iterator it = m_vWorldLocation.begin(); it != m_vWorldLocation.end(); ++it )
	{
		if( (*it).idx == idx )
		{
			nCode = (*it).shovelable_item;
			break;
		}
	}

	return nCode;
}

const int WorldLocationManager::GetShovelableMonster( const unsigned int idx )
{
	std::vector< int > * pvMonsterID = NULL;

	if( !m_hsMonsterID.lookup( idx, pvMonsterID ) )
		return 0;

	int nCode = 0;
	
	int nCount = 0;
	do
	{
		nCode = (*pvMonsterID)[ XRandom( 0, static_cast< int >( pvMonsterID->size() ) - 1 ) ];

		// 몬스터 등급 체계 변경에 따른 수정, 삽질은 현재 사용하지 않는 시스템이지만 추후 문제가 생긴다면 변경 필요
		MonsterBase *pMonsterBase = GameContent::GetMonsterInfo( nCode );
		if( pMonsterBase->fight_type != MonsterBase::FIGHT_TYPE_NORMAL ||
			pMonsterBase->monster_type != MonsterBase::MONSTER_TYPE_MIDDLE_1_STAR )
		{
			nCode = 0;
		}

	} while( !nCode && ++nCount < 5 );

	return nCode;
}

void WorldLocationManager::RegisterWorldLocation( unsigned int idx, unsigned char location_type, unsigned int time_id, unsigned int weather_id, unsigned char ratio, AR_TIME weather_change_time, const int shovelable_item )
{
	std::vector< StructWorldLocation >::iterator it;

	StructWorldLocation * pLocation = NULL;

	for( it = m_vWorldLocation.begin(); it != m_vWorldLocation.end(); ++it )
	{
		if( (*it).idx == idx )
		{
			pLocation = &(*it);
			break;
		}
	}

	if( pLocation == NULL )
	{
		m_vWorldLocation.push_back( StructWorldLocation() );

		pLocation = &m_vWorldLocation.back();

		pLocation->idx = idx;
		pLocation->location_type = location_type;
		pLocation->weather_change_time = weather_change_time;
	}

	pLocation->weather_ratio[time_id][weather_id] = ratio;
	pLocation->shovelable_item = shovelable_item;
}

void WorldLocationManager::RegisterMonsterLocation( const unsigned int idx, unsigned int monster_id )
{
	std::vector< int > * pvMonsterID = NULL;

	if( !m_hsMonsterID.lookup( idx, pvMonsterID ) )
	{
		pvMonsterID = new std::vector< int >;
		m_hsMonsterID.add( idx, pvMonsterID );
	}

	for( std::vector< int >::iterator it = pvMonsterID->begin() ; it != pvMonsterID->end() ; ++it )
	{
		if( (*it) == monster_id )
		{
			return;
		}
	}

	pvMonsterID->push_back( monster_id );
}
