#pragma once

#include <vector>

#include <toolkit/ILock.h>

#include "StructRoamer.h"
#include "GameContent.h"


struct RoamingManager
{
public:
	static RoamingManager & Instance();
	virtual ~RoamingManager();

	const bool			RegisterRoamingInfo( const int nID, const StructRoamer::ROAMING_TYPE eType, const int nMoveSpeed, const StructRoamer::HATE_TYPE eHateType, const AR_TIME nRespawnInterval, const int nAttributeFlag, int nInitialX, int ninitialY );
	const bool			UnregisterRoamingInfo( const int nID );
	const size_t		GetRoamingInfoCount()	{ return m_vRoamer.size(); }

	const bool			RegisterRoamingPointInfo( const int nRoamingID, const int nPosX, const int nPosY );
	const size_t		GetRoamingPointInfoCount( const int nRoamingID = -1 );

	const bool			AddRoamingCreatureRespawnInfo( const int nRoamingID, const GameContent::ROAMING_CREATURE_RESPAWN_INFO & info );

	const bool			Init();
	const bool			DeInit();

	void				InitRoamer( const int nRoamingID, const int nLayer = -1 );
	void				DeInitRoamer( const int nRoamingID, const int nLayer = -1 );

private:
	RoamingManager()
		: m_RoamerLock( "RoamingManager::m_RoamerLock" )
		, m_bInitialized( false )
	{}

private:
	XCriticalSection						m_RoamerLock;

	bool									m_bInitialized;
	std::vector< StructRoamer * >			m_vRoamer;
};
