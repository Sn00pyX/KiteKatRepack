
#pragma once

#include "BattleArena.h"


// 대기열에 의해 운영되는 아레나
class	BattleArenaWithWaitQueue : public BattleArena
{
public:

	typedef std::pair< StructPlayer*, AR_TIME >	PlayerWaitInfo;

public:

	BattleArenaWithWaitQueue( const BattleArenaBaseServer* pArenaBase, ThreadSafeIntMap* pPartyToArenaMap );
	virtual ~BattleArenaWithWaitQueue();

	// { csArena를 걸고 호출해야 하는 함수들
	// 대기열에 유저가 추가되면 RESULT_PENDING을, 최근에 시작된 경기에 바로 진입되었을 경우에는 RESULT_SUCCESS을 각각 리턴함
	virtual unsigned short	_joinWaitQueue( StructPlayer* pPlayer, _BATTLE_GRADE eGrade );
	virtual unsigned short	_leaveWaitQueue( StructPlayer* pPlayer, _ARENA_LEAVE_TYPE eLeaveType );
	virtual unsigned short	_leaveWaitQueue( PlayerUID nPlayerUID, _ARENA_LEAVE_TYPE eLeaveType );

	virtual unsigned char	_tryStartNewBattle();
	virtual unsigned short	_createNewBattle( _BATTLE_GRADE eGrade, unsigned char nNewInstanceNo );

	AR_TIME									_getWaitStartTime( _BATTLE_GRADE eGrade )	const;
	std::vector< PlayerWaitInfo >*			_getWaitQueue( _BATTLE_GRADE eGrade );
	const std::vector< PlayerWaitInfo >*	_getWaitQueue( _BATTLE_GRADE eGrade )	const;
	// } csArena를 걸고 호출해야 하는 함수들

private:

	_BATTLE_GRADE			GetPlayableLongestWaitingGrade()	const;

private:

	// 대기열은 자료구조상 std::deque이 가장 적절하지만 방이 생성되어 앞에서부터 대기열에 있던 n명의 유저가 빠져나갈 때
	// n명 뒤로 대기열에 남아야 할 유저가 많이 있을 경우가 거의 없다고 예상되며,
	// 대기열의 순회에 있어서는 vector가 훨씬 유리하기 때문에 std::deque 대신 std::vector를 사용함.
	std::vector< PlayerWaitInfo >	m_vWaitQueue[ MAX_BATTLE_GRADE ];

};

