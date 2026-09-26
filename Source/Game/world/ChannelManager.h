#pragma once

#include <vector>

#include <geometry/X2DBasicTypes.h>
#include <mmo/ArType.h>
#include <mmo/ArOption.h>


struct ChannelManager
{
	enum
	{
		TYPE_NOTHING			= 0,
		TYPE_USER_LIMIT			= 1,
		TYPE_DUNGEON			= 2,
	};

	static int GetChannelNum( unsigned char layer );
	static int GetChannelId( unsigned char layer );

	static void EnterPlayerToLayer( unsigned char layer );
	static void LeavePlayerFromLayer( unsigned char layer );
	static int GetPlayerCountInLayer( unsigned char layer );

	static int GetChannelId( AR_UNIT x, AR_UNIT y );
	static unsigned char GetProperLayer( AR_UNIT x, AR_UNIT y );
	static int GetProperChannel( int channel_id );
	static int GetMinChannelNo( int channel_id );
	static int GetMaxChannelNo( int channel_id );

	static unsigned char GetLayerOfChannel( int channel_id, int channel_num );
	static int GetChannelType( int channel_id );

	static bool GetLayersOfChannel( int channel_id, std::vector< unsigned char > & vLayers );
	static bool GetRegionOfChannel( int channel_id, X2D::Box< AR_UNIT > & bxRegion );

	static void RegisterChannelInfo( int channel_id, const X2D::Box< AR_UNIT > & channel_region, int channel_type );
	static void RegisterUserLimitChannelInfo( int channel_id, int proper_user, int max_channel_num );

};