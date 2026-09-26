
#pragma once

#include "BattleArenaInstance.h"


class	BattleArena
{
public:

	BattleArena( const BattleArenaBaseServer* pArenaBase, ThreadSafeIntMap* pPartyToArenaMap );
	virtual ~BattleArena();

	ArcadiaLock						LockWholeArena( unsigned short nLayer = -1 )	const;

	// { csArena를 걸고 호출해야 하는 함수들
	unsigned char					_allocNewInstanceNo();
	void							_freeInstanceNo( unsigned char nInstanceNo );
	bool							_isOccupiedInstanceNo( unsigned char nInstanceNo )	const;

	bool							_isFull() const;

	virtual unsigned short			_joinWaitQueue( StructPlayer* pPlayer, _BATTLE_GRADE eGrade )			= 0;
	virtual unsigned short			_leaveWaitQueue( StructPlayer* pPlayer, _ARENA_LEAVE_TYPE eLeaveType )	= 0;
	virtual unsigned short			_leaveWaitQueue( PlayerUID nPlayerUID, _ARENA_LEAVE_TYPE eLeaveType )	= 0;

	virtual unsigned char			_tryStartNewBattle()													= 0;
	virtual unsigned short			_createNewBattle( _BATTLE_GRADE eGrade, unsigned char nNewInstanceNo )	= 0;

	BattleArenaInstance*			_getBattleInstance( unsigned char nInstanceNo );
	const BattleArenaInstance*		_getBattleInstance( unsigned char nInstanceNo )	const;
	// } csArena를 걸고 호출해야 하는 함수들

public:

	const BattleArenaBaseServer*	m_pArenaBase;
	ThreadSafeIntMap*				m_pPartyToArenaMap;
	ThreadSafeIntMap				m_partyToInstanceMap;

	// BattleArena의 멤버를 보호하기 위한 락(지역 락 -> csArena -> csBattle -> PartyManager::m_IntfLock)
	mutable XCriticalSection		m_csArena;

	// { 런타임에 수시로 변경되는 것들 - 접근 시 csArena를 걸어야 함

	// ( 배열 인덱스 )에 해당하는 방 번호의 사용 여부(0번 방 번호는 사용하지 않으므로 0번 인덱스도 사용 안 함)
	bool							m_aInstanceNoUseFlag[ MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA + 1 ];
	// aInstanceNoUseFlag 안에서 가장 작은(인덱스가 1에 가까운) 비어있는 슬롯 인덱스(새 방 번호를 발급할 때는 nLeastFreeInstanceSlot을 바로 지급함)
	unsigned char					m_nLeastFreeInstanceSlot;
	// 활성화 상태의 방 목록을 빠르게 순회할 수 있도록 별도로 InstanceNo 보관
	std::vector< unsigned char >	m_vActiveLayerList;

	// 인스턴스 리스트(방 번호 - 1 번 배열 요소에 인스턴스 포인터가 있음)
	// * 한 번 할당된 BattleArenaInstance는 경기 종료 시에도 삭제하지 않고 재활용함
	BattleArenaInstance*			m_apBattleList[ MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA + 1 ];

	// } 런타임에 수시로 변경되는 것들 - 접근 시 csArena를 걸어야 함
};