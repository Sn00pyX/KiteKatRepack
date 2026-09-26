#pragma once

#include <vector>

#include <toolkit/ILock.h>
#include <toolkit/XFlag.h>

#include "GameRule.h"
#include "GameContent.h"
#include "StructMonster.h"
#include "StructNPC.h"


struct StructRoamer : public ArObject, StructMonster::MonsterDeleteHandler, StructNPC::NPCDeadHandler
{
public:
	enum ROAMING_TYPE
	{
		ROAMING_TYPE_ROUND			= 0,
		ROAMING_TYPE_GO_BACK		= 1,
	};

	enum HATE_TYPE
	{
		HATE_TYPE_INDIVIDUAL		= 1 << 0,
		HATE_TYPE_SHARE_FIRST		= 1 << 1,
		HATE_TYPE_FULL_SHARE		= 1 << 2,
	};

	enum ROAMING_CREATURE_TYPE
	{
		ROAMING_CREATURE_MONSTER	= 0,
		ROAMING_CREATURE_NPC		= 1,
	};

protected:
	enum ROAMING_DIRECTION
	{
		ROAMING_DIRECTION_FORWARD		= 0,
		ROAMING_DIRECTION_BACKWARD		= 1
	};

	enum ROAMING_STATUS
	{
		ROAMING_STATUS_IDLE			= 0,
		ROAMING_STATUS_ROAMING		= 1,
		ROAMING_STATUS_ROTATING		= 2,
		ROAMING_STATUS_PAUSED		= 3,
	};

	struct ROAMING_CREATURE_INFO : public GameContent::ROAMING_CREATURE_RESPAWN_INFO
	{
		ROAMING_CREATURE_INFO( const ROAMING_CREATURE_TYPE eCreatureType, const int nCreatureID, const AR_TIME nRespawnInterval, const int nAngle, const AR_UNIT nDistance )
			: GameContent::ROAMING_CREATURE_RESPAWN_INFO( eCreatureType, nCreatureID, nRespawnInterval, nAngle, nDistance )
			, m_pCreature( NULL )
			, m_nNextRespawnTime( 0 )
		{}

		ROAMING_CREATURE_INFO( const ROAMING_CREATURE_RESPAWN_INFO & info )
			: ROAMING_CREATURE_RESPAWN_INFO( info )
			, m_pCreature( NULL )
			, m_nNextRespawnTime( 0 )
		{}

		StructCreature *			m_pCreature;
		AR_TIME						m_nNextRespawnTime;
	};

	struct PENDING_HATE_SHARE_INFO
	{
		PENDING_HATE_SHARE_INFO( const AR_HANDLE hRequester, const AR_HANDLE hHateTarget, const int nHate )
			: m_hRequester( hRequester )
			, m_hHateTarget( hHateTarget )
			, m_nHate( nHate )
		{}

		AR_HANDLE		m_hRequester;
		AR_HANDLE		m_hHateTarget;
		int				m_nHate;
	};

public:
	StructRoamer( const int nID, const ROAMING_TYPE eRoamingType, const int nMoveSpeed, const HATE_TYPE eHateType, const AR_TIME nRespawnInterval, const int nAttributeFlag, const bool bIsRaidDungeonRoamer );
	~StructRoamer();

	virtual bool				ProcDelete()		{ delete this; return true; }
	virtual bool				IsRoamer() const	{ return true; }

	const int					GetRoamingID()		{ return m_nID; }

	void						AddCreatureRespawnInfo( const GameContent::ROAMING_CREATURE_RESPAWN_INFO & info );
	const size_t				GetCreatureRespawnInfoCount()				{ return m_vRoamingCreatureRespawnInfo.size(); }

	void						AddRoamingPoint( const ArPosition & pos );
	const size_t				GetRoamingPointCount()						{ return m_vRoamingPoint.size(); }

	void						DeleteRespawnedCreature();

	const bool					Init();
	const bool					DeInit( const bool bForceToDeleteEverlastingRoamer );
	const bool					IsInitialized()								{ return GetPriority() == UPDATE_PRIORITY_HIGH; }

	virtual void				onProcess( int nThreadIdx );

	virtual void				onMonsterDelete( struct StructMonster * pMonster );
	virtual void				onNPCDead( struct StructNPC * pNPC );

	virtual bool				IsMovable();
	const bool					IsRaidDungeonRoamer() const					{ return m_bIsRaidDungeonRoamer; }
	const bool					IsPaused()									{ return m_eRoamingStatus == ROAMING_STATUS_PAUSED; }
	void						PauseRoaming();

	const ArPosition			GetCurrentRoamingTargetPosition();
	const ArPosition			GetNextRoamingTargetPosition();

	void						PendHateShare( const AR_HANDLE hRequester, const AR_HANDLE hHateTarget, const int nHate, const int eApplyHateType = HATE_TYPE_FULL_SHARE );

protected:
	const bool					isMovable();
	void						processWalk( AR_TIME t );
	void						processRoaming( AR_TIME t, const ArPosition & currentPos, const float & fFace );
	void						processHateSharing();

	const size_t				getNextRoamingTargetIndex() const;
	void						proceedRoamingTargetIndex();

	static const ArPosition		getCurrentRespawnObjectPosition( const ArPosition & currentPos, const float & fFace, const int & nAngle, const AR_UNIT & nDistance );

private:
	const int										m_nID;
	const int										m_nMoveSpeed;

	ROAMING_TYPE									m_eRoamingType;
	HATE_TYPE										m_eHateType;
	AR_TIME											m_nRespawnInterval;		// 그룹 로밍 전체 리젠 처리 지연 시간(0일 경우 개별 리젠 처리 적용)

	enum _ATTRIBUTE_FLAG_IDX
	{
		ATTRIBUTE_HEAL_HP_ON_COMEBACKHOME			= 1,	// 전투 종료 후 원위치로 돌아갔을 때 HP를 최대치로 회복시킬 것인지 여부
	};
	XFlag< int, 1 >									m_AttributeFlag;		// 특성 플래그

	ROAMING_DIRECTION								m_eCurrentRoamingDirection;
	size_t											m_nCurrentRoamingPointIndex;
	ROAMING_STATUS									m_eRoamingStatus;
	const bool										m_bIsRaidDungeonRoamer;			// DeInit되어도 삭제되지 않아야 하는 로망 그룹 데이터 여부(레이드 던전 내부 등)

	// 이하 3개의 멤버를 보호(m_CS -> 지역 락 순서여야 함)
	XCriticalSection								m_CS;

	std::vector< ArPosition >						m_vRoamingPoint;
	std::vector< ROAMING_CREATURE_INFO >			m_vRoamingCreatureRespawnInfo;
	AR_TIME											m_nNextRespawnProcTime;
	int												m_nLastRegenCount;		// 그룹 단체 리젠일 경우 마지막 리젠 시간 카운팅하기 위한 변수

	// m_vPendingHateInfo를 보호(지역 락 -> m_csHate 순서여야 함)
	XCriticalSection								m_csHate;
	std::vector< PENDING_HATE_SHARE_INFO * >		m_vPendingHateInfo;
};
