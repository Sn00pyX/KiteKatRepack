
#include "LogClient/LogClient.h"

#include "BattleArenaForExercise.h"


BattleArenaForExercise::BattleArenaForExercise( const BattleArenaBaseServer* pArenaBase, ThreadSafeIntMap* pPartyToArenaMap )
	: BattleArena( pArenaBase, pPartyToArenaMap )
{
}

BattleArenaForExercise::~BattleArenaForExercise()
{
}

// { csArena를 걸고 호출해야 하는 함수들
unsigned short	BattleArenaForExercise::_joinWaitQueue( StructPlayer* pPlayer, _BATTLE_GRADE eGrade )
{
	return RESULT_NOT_ACTABLE;
}

unsigned short	BattleArenaForExercise::_leaveWaitQueue( StructPlayer* pPlayer, _ARENA_LEAVE_TYPE eLeaveType )
{
	return RESULT_NOT_ACTABLE;
}

unsigned short	BattleArenaForExercise::_leaveWaitQueue( PlayerUID nPlayerUID, _ARENA_LEAVE_TYPE eLeaveType )
{
	return RESULT_NOT_ACTABLE;
}

unsigned char	BattleArenaForExercise::_tryStartNewBattle()
{
	// 연습 경기는 이하의 처리(대기열 상태 체크 후 새 경기 시작)를 하지 않음
	assert( m_pArenaBase->IsExerciseGameArena() );
	return INVALID_BATTLE_ARENA_INSTANCE_NO;
}

unsigned short	BattleArenaForExercise::_createNewBattle( _BATTLE_GRADE eGrade, unsigned char nNewInstanceNo )
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	if( IsValidBattleArenaInstanceNo( nNewInstanceNo ) )
	{
		BattleArenaInstance*& pBattleInstance = m_apBattleList[ nNewInstanceNo ];
		// 처음 해당 방 번호에 경기가 생성되는 경우 BattleArenaInstance 객체를 할당(이후는 같은 방 번호의 경우 기존에 할당받았던 객체를 재사용함)
		// * 연습 경기는 시간에 의해 자동으로 경기가 시작되지 않고 양 팀의 파티장이 준비 완료 후 최초 경기 생성자가 경기 시작 버튼을 눌러야 시작됨
		if( pBattleInstance == NULL )
		{
			pBattleInstance = new BattleArenaInstance( m_pArenaBase, m_pPartyToArenaMap, &m_partyToInstanceMap, nNewInstanceNo );
		}

		THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );
		pBattleInstance->Init( eGrade, INFINITE_TIME );

		// 연습 경기는 대기열 등을 통해 현재 경기에 참여할 유저의 목록이나 정보가 따로 없기 때문에 인스턴스만 생성하고 그 외에는 아무것도 하지 않

		LOG::Log11N4S( LM_BATTLE_ARENA_CREATE, 0, 0, m_pArenaBase->nID, nNewInstanceNo, eGrade,
			pBattleInstance->_getTeamMemberCount(0), pBattleInstance->_getTeamMemberCount(1),
			0, 0, 0, 0, "", LOG::STR_NTS, "", LOG::STR_NTS,
			"EXERCISE", LOG::STR_NTS, "", LOG::STR_NTS );

		return RESULT_SUCCESS;
	}

	return RESULT_UNKNOWN;
}
