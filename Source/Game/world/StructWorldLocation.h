#pragma once

#include <vector>

#include <mmo/ArSchedulerObject.h>
#include <toolkit/KHash.h>

#include "ItemBase.h"

struct StructWorldLocation
{
	StructWorldLocation()
	{
		location_type = 0;
		idx = 0;
		memset( &weather_ratio, 0, sizeof( weather_ratio ) );

		current_weather = 0;
		weather_change_time = 0;
		last_changed_time = 0;

		shovelable_item = 0;
	}

	enum
	{
		TIME_DAWN		= 0,
		TIME_DAYTIME	= 1,
		TIME_EVENING	= 2,
		TIME_NIGHT		= 3,
		TIME_MAX		= 4,
	};

	enum
	{
		WEATHER_CLEAR		= 0,
		WEATHER_LIGHT_RAINY	= 1,
		WEATHER_RAINY		= 2,
		WEATHER_HEAVY_RAINY	= 3,
		WEATHER_LIGHT_SNOWY	= 4,
		WEATHER_SNOWY		= 5,
		WEATHER_HEAVY_SNOWY	= 6,
		WEATHER_MAX			= 7,
	};

	unsigned int idx;

	enum
	{
		LOCATION_ETC						= 0,
		LOCATION_TOWN						= 1,
		LOCATION_FIELD						= 2,
		LOCATION_NON_PK_FIELD				= 3,
		LOCATION_DUNGEON					= 4,
		LOCATION_BATTLE_FIELD				= 5,
		LOCATION_EVENTMAP					= 7,
		LOCATION_HUNTAHOLIC_LOBBY			= 8,
		LOCATION_HUNTAHOLIC_DUNGEON			= 9,
		LOCATION_MARICAT_MARKET				= 10,
		LOCATION_DEATHMATCH					= 11,
		LOCATION_SECRET_DUNGEON				= 12,
		LOCATION_FIELD_WITHOUT_LENSFLARE	= 13,
		LOCATION_INSTANCE_DUNGEON			= 14,
		LOCATION_BATTLE_ARENA				= 15,
		LOCATION_RAMADAN_PRAYER_HALL		= 16,
	};

	enum
	{
		LOCATION_ID_ABADON				= 110900,
		LOCATION_ID_SECROUTE_1			= 130100,
		LOCATION_ID_SECROUTE_2			= 130101,
		LOCATION_ID_SECROUTE_AUCTION	= 130107,
	};

	unsigned char location_type;

	unsigned char weather_ratio[TIME_MAX][WEATHER_MAX];
	unsigned char current_weather;

	AR_TIME weather_change_time;
	AR_TIME last_changed_time;

	// 삽질 관련
	ItemBase::ItemCode		shovelable_item;		// 삽질해서 나올 수 있는 아이템(음수일 경우 드랍 그룹 ID로 동작)

	std::vector< struct StructPlayer * >		m_vIncludeClient;
};

struct WorldLocationManager : ArSchedulerObject
{
	bool Init();
	bool DeInit();

	static WorldLocationManager & Instance();

	virtual void onProcess( int nThreadIdx );

	const						StructWorldLocation * AddToLocation( unsigned int idx, struct StructPlayer * pPlayer );
	bool						RemoveFromLocation( struct StructPlayer * pPlayer );
	bool						SendWeatherInfo( unsigned int idx, struct StructPlayer * pPlayer );

	const unsigned char			GetLocationType( const unsigned int idx );

	const ItemBase::ItemCode	GetShovelableItem( const unsigned int idx );
	const int					GetShovelableMonster( const unsigned int idx );

	void						RegisterWorldLocation( unsigned int idx, unsigned char location_type, unsigned int time_id, unsigned int weather_id, unsigned char ratio, AR_TIME weather_change_time, const int shovelable_item );
	void						RegisterMonsterLocation( const unsigned int idx, unsigned int monster_id );

protected:
	WorldLocationManager();
	~WorldLocationManager();

private:
	std::vector< StructWorldLocation > m_vWorldLocation;

	KHash< std::vector< int > *, hashPr_mod_int >		m_hsMonsterID;			// 특정 지역에 배정된 몬스터 ID 리스트(삽질로 몬스터 리젠 시 사용)
};