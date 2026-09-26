
#pragma once

#include "BattleArena.h"

#include "ErrorCode/ErrorCode.h"


// 연습 경기용 아레나(WaitQueue 관련 함수들을 사용하면 안 됨)
class	BattleArenaForExercise : public BattleArena
{
public:

	BattleArenaForExercise( const BattleArenaBaseServer* pArenaBase, ThreadSafeIntMap* pPartyToArenaMap );
	virtual ~BattleArenaForExercise();

	// { csArena를 걸고 호출해야 하는 함수들
	virtual unsigned short	_joinWaitQueue( StructPlayer* pPlayer, _BATTLE_GRADE eGrade );
	virtual unsigned short	_leaveWaitQueue( StructPlayer* pPlayer, _ARENA_LEAVE_TYPE eLeaveType );
	virtual unsigned short	_leaveWaitQueue( PlayerUID nPlayerUID, _ARENA_LEAVE_TYPE eLeaveType );

	virtual unsigned char	_tryStartNewBattle();
	virtual unsigned short	_createNewBattle( _BATTLE_GRADE eGrade, unsigned char nNewInstanceNo );
	// } csArena를 걸고 호출해야 하는 함수들

};
