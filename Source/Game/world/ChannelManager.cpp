
#include <toolkit/ILock.h>

#include "ChannelManager.h"


struct _CHANNEL_USE_INFO
{
	_CHANNEL_USE_INFO()
	{
		channel_id = 0;
		channel_num = 0;
	}

	int	channel_id;
	int	channel_num;
};

struct _CHANNEL_INFO
{
	int											channel_id;
	std::vector< const X2D::Box< AR_UNIT > >	regions;
	int											type;

	struct CHANNEL_LAYER_MAP
	{
		CHANNEL_LAYER_MAP( int _num, int _layer ) : channel_num( _num ), layer( _layer ), player_cnt(0) {}

		int channel_num;
		int layer;
		int player_cnt;
	};

	std::vector< CHANNEL_LAYER_MAP >		channel_layer_map;

	union
	{
		struct
		{
			int proper_user;
			int max_num;
		} limit_user;

		// 이후 여러가지 타입이 들어갈 수 있3
	} info;
};

std::vector< _CHANNEL_INFO >				g_vChannelInfo;
_CHANNEL_USE_INFO							g_vChannelUseInfo[MAX_LAYER];
static XCriticalSection						s_csChannelLock;

int ChannelManager::GetChannelNum( unsigned char layer )
{
	return g_vChannelUseInfo[ layer ].channel_num;
}

int ChannelManager::GetChannelId( unsigned char layer )
{
	return g_vChannelUseInfo[ layer ].channel_id;
}

void ChannelManager::EnterPlayerToLayer( unsigned char layer )
{
	THREAD_SYNCRONIZE( &s_csChannelLock );

	if( layer < 1 || layer >= MAX_LAYER )
		return;

	if( !g_vChannelUseInfo[layer].channel_id )
		return;

	int channel_id = g_vChannelUseInfo[layer].channel_id;
	int channel_num = g_vChannelUseInfo[layer].channel_num;

	std::vector< _CHANNEL_INFO >::iterator it;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).channel_id == channel_id )
		{
			std::vector< _CHANNEL_INFO::CHANNEL_LAYER_MAP >::iterator mapit;

			for( mapit = (*it).channel_layer_map.begin(); mapit != (*it).channel_layer_map.end(); ++mapit )
			{
				if( (*mapit).channel_num == channel_num )
				{
					++(*mapit).player_cnt;

					break;
				}
			}

			break;
		}
	}
}

void ChannelManager::LeavePlayerFromLayer( unsigned char layer )
{
	THREAD_SYNCRONIZE( &s_csChannelLock );

	if( layer < 1 || layer >= MAX_LAYER )
		return;

	if( !g_vChannelUseInfo[layer].channel_id )
		return;

	int channel_id = g_vChannelUseInfo[layer].channel_id;
	int channel_num = g_vChannelUseInfo[layer].channel_num;

	std::vector< _CHANNEL_INFO >::iterator it;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).channel_id == channel_id )
		{
			std::vector< _CHANNEL_INFO::CHANNEL_LAYER_MAP >::iterator mapit;

			for( mapit = (*it).channel_layer_map.begin(); mapit != (*it).channel_layer_map.end(); ++mapit )
			{
				if( (*mapit).channel_num == channel_num )
				{
					--(*mapit).player_cnt;

					break;
				}
			}

			break;
		}
	}
}

int ChannelManager::GetPlayerCountInLayer( unsigned char layer )
{
	THREAD_SYNCRONIZE( &s_csChannelLock );

	if( layer < 1 || layer >= MAX_LAYER )
		return 0;

	if( !g_vChannelUseInfo[layer].channel_id )
		return 0;

	int channel_id = g_vChannelUseInfo[layer].channel_id;
	int channel_num = g_vChannelUseInfo[layer].channel_num;

	std::vector< _CHANNEL_INFO >::iterator it;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).channel_id == channel_id )
		{
			std::vector< _CHANNEL_INFO::CHANNEL_LAYER_MAP >::iterator mapit;

			for( mapit = (*it).channel_layer_map.begin(); mapit != (*it).channel_layer_map.end(); ++mapit )
			{
				if( (*mapit).channel_num == channel_num )
				{

					return (*mapit).player_cnt;

					break;
				}
			}

			break;
		}
	}

	return 0;
}

unsigned char ChannelManager::GetLayerOfChannel( int channel_id, int channel_num )
{
	std::vector< _CHANNEL_INFO >::iterator it;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).channel_id == channel_id )
		{
			std::vector< _CHANNEL_INFO::CHANNEL_LAYER_MAP >::iterator mapit;

			for( mapit = (*it).channel_layer_map.begin(); mapit != (*it).channel_layer_map.end(); ++mapit )
			{
				if( (*mapit).channel_num == channel_num )
				{

					return (*mapit).layer;

					break;
				}
			}

			break;
		}
	}

	return 0;
}

int ChannelManager::GetChannelId( AR_UNIT x, AR_UNIT y )
{
	X2D::Point< AR_UNIT > pt( x, y );

	std::vector< _CHANNEL_INFO >::const_iterator it;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		std::vector< const X2D::Box< AR_UNIT > >::const_iterator rit;
		for( rit = (*it).regions.begin(); rit != (*it).regions.end(); ++rit )
		{
			if( (*rit).IsInclude( pt ) )
			{
				return (*it).channel_id;
			}
		}
	}

	return 0;
}

int ChannelManager::GetMinChannelNo( int channel_id )
{
	THREAD_SYNCRONIZE( &s_csChannelLock );

	std::vector< _CHANNEL_INFO >::const_iterator it;
	int channel_no = 0;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		std::vector< const X2D::Box< AR_UNIT > >::const_iterator rit;
		for( rit = (*it).regions.begin(); rit != (*it).regions.end(); ++rit )
		{
			if( (*it).channel_id == channel_id )
			{
				if( (*it).type == TYPE_USER_LIMIT )
				{
					std::vector< _CHANNEL_INFO::CHANNEL_LAYER_MAP >::const_iterator mapit;

					for( mapit = (*it).channel_layer_map.begin(); mapit != (*it).channel_layer_map.end(); ++mapit )
					{
						if( !channel_no || (*mapit).channel_num < channel_no )
						{
							channel_no = (*mapit).channel_num;
						}
					}
				}
				break;
			}
		}
	}

	// 채널 관련 리소스가 없었을 경우 1로 반환
	return ( channel_no ) ? channel_no : 1;
}

int ChannelManager::GetMaxChannelNo( int channel_id )
{
	THREAD_SYNCRONIZE( &s_csChannelLock );

	std::vector< _CHANNEL_INFO >::const_iterator it;
	int channel_no = 0;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		std::vector< const X2D::Box< AR_UNIT > >::const_iterator rit;
		for( rit = (*it).regions.begin(); rit != (*it).regions.end(); ++rit )
		{
			if( (*it).channel_id == channel_id )
			{
				if( (*it).type == TYPE_USER_LIMIT )
				{
					std::vector< _CHANNEL_INFO::CHANNEL_LAYER_MAP >::const_iterator mapit;

					for( mapit = (*it).channel_layer_map.begin(); mapit != (*it).channel_layer_map.end(); ++mapit )
					{
						if( !channel_no || (*mapit).player_cnt )
						{
							channel_no = (*mapit).channel_num;
						}
					}
				}
				break;
			}
		}
	}

	// 채널 관련 리소스가 없었을 경우 1로 반환
	return ( channel_no ) ? channel_no : 1;
}

int ChannelManager::GetProperChannel( int channel_id )
{
	THREAD_SYNCRONIZE( &s_csChannelLock );

	std::vector< _CHANNEL_INFO >::iterator it;
	int channel_no = 0;
	int player_cnt = INT_MAX;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).channel_id != channel_id )
			continue;

		if( (*it).type != TYPE_USER_LIMIT )
			break;

		std::vector< _CHANNEL_INFO::CHANNEL_LAYER_MAP >::iterator mapit;

		for( mapit = (*it).channel_layer_map.begin(); mapit != (*it).channel_layer_map.end(); ++mapit )
		{
			if( (*mapit).player_cnt <= (*it).info.limit_user.proper_user )
				return (*mapit).channel_num;

			if( (*mapit).player_cnt < player_cnt )
			{
				player_cnt = (*mapit).player_cnt;
				channel_no = (*mapit).channel_num;
			}
		}

		break;
	}

	return channel_no;
}

unsigned char ChannelManager::GetProperLayer( AR_UNIT x, AR_UNIT y )
{
	THREAD_SYNCRONIZE( &s_csChannelLock );

	X2D::Point< AR_UNIT > pt( x, y );

	std::vector< _CHANNEL_INFO >::const_iterator it;
	int layer = 0;
	int player_cnt = INT_MAX;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).type != TYPE_USER_LIMIT )
			continue;

		std::vector< const X2D::Box< AR_UNIT > >::const_iterator rit;
		for( rit = (*it).regions.begin(); rit != (*it).regions.end(); ++rit )
		{
			if( !(*rit).IsInclude( pt ) )
				continue;

			std::vector< _CHANNEL_INFO::CHANNEL_LAYER_MAP >::const_iterator mapit;

			for( mapit = (*it).channel_layer_map.begin(); mapit != (*it).channel_layer_map.end(); ++mapit )
			{
				if( (*mapit).player_cnt < (*it).info.limit_user.proper_user )
					return (*mapit).layer;

				if( (*mapit).player_cnt < player_cnt )
				{
					player_cnt = (*mapit).player_cnt;
					layer = (*mapit).layer;
				}
			}

		break;
		}
	}

	return layer;
}
int ChannelManager::GetChannelType( int channel_id )
{
	std::vector< _CHANNEL_INFO >::iterator it;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).channel_id == channel_id )
		{
			return (*it).type;
		}
	}

	return TYPE_NOTHING;
}

bool ChannelManager::GetLayersOfChannel( int channel_id, std::vector< unsigned char > & vLayers )
{
	if( channel_id )
	{
		for( std::vector< _CHANNEL_INFO >::const_iterator it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
		{
			if( (*it).channel_id == channel_id )
			{
				if( (*it).type == TYPE_USER_LIMIT )
				{
					std::vector< _CHANNEL_INFO::CHANNEL_LAYER_MAP >::const_iterator mapit;

					for( mapit = (*it).channel_layer_map.begin() ; mapit != (*it).channel_layer_map.end() ; ++mapit )
					{
						vLayers.push_back( (*mapit).layer );
					}
				}
				else if( (*it).type == TYPE_DUNGEON )
				{
					// unsigned char 로 할 경우 255 이후에 0으로 돌아가서 무한히 추가하게 됨
					for( unsigned int nLayer = 0 ; nLayer < MAX_LAYER ; ++nLayer )
					{
						vLayers.push_back( static_cast< unsigned char >( nLayer ) );
					}
				}
				break;
			}
		}
	}

	// 찾은 레이어가 없을 경우 기본 레이어를 넣어 줌
	if( vLayers.empty() )
	{
		vLayers.push_back( 0 );
		return false;
	}

	return true;
}

bool ChannelManager::GetRegionOfChannel( int channel_id, X2D::Box< AR_UNIT > & bxRegion )
{
	bool bFound = false;

	for( std::vector< _CHANNEL_INFO >::const_iterator it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).channel_id != channel_id )
			continue;

		assert( (*it).regions.size() == 1 );

		bFound = true;
		bxRegion = (*it).regions.front();

		break;
	}

	return bFound;
}

void ChannelManager::RegisterChannelInfo( int channel_id, const X2D::Box< AR_UNIT > & channel_region, int channel_type )
{
	std::vector< _CHANNEL_INFO >::iterator it;
	bool find = false;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).channel_id == channel_id )
		{
			find = true;
			(*it).regions.push_back( channel_region );
			break;
		}
	}
	if( !find )
	{
		_CHANNEL_INFO info;
		info.channel_id = channel_id;
		info.regions.push_back( channel_region );
		info.type = channel_type;

		memset( &info.info, 0, sizeof( info.info ) );

		g_vChannelInfo.push_back( info );
	}
}

void ChannelManager::RegisterUserLimitChannelInfo( int channel_id, int proper_user, int max_channel_num )
{
	std::vector< _CHANNEL_INFO >::iterator it;

	for( it = g_vChannelInfo.begin(); it != g_vChannelInfo.end(); ++it )
	{
		if( (*it).channel_id == channel_id )
		{
			(*it).info.limit_user.proper_user = proper_user;
			(*it).info.limit_user.max_num = max_channel_num;

			int channel_cnt = 0;

			for( int i = 1; i < MAX_LAYER; ++i )	// 0번은 기본 layer이다.
			{
				if( channel_cnt > max_channel_num )
					break;

				if( g_vChannelUseInfo[i].channel_id )
					continue;

				g_vChannelUseInfo[i].channel_id = channel_id;
				g_vChannelUseInfo[i].channel_num = ++channel_cnt;

				(*it).channel_layer_map.push_back( _CHANNEL_INFO::CHANNEL_LAYER_MAP( channel_cnt, i ) );

			}

			break;
		}
	}
}

