
#pragma once


#include <mmo/ArObject.h>
#include <mmo/ArcadiaServer.h>

#include "BattleArenaBase.h"


const int				INVALID_BATTLE_ARENA_TEAM_NO			= -1;
const unsigned char		INVALID_BATTLE_ARENA_INSTANCE_NO		= 255;
const unsigned char		MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA	= 250;

extern const char*		FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE;
extern const char*		FLAG_BATTLE_ARENA_PROP_STATE;

// 스크립트 실행 시 실행 유형 코드(LM_BATTLE_ARENA_PROCESS 로그 생성 시 사용됨)
enum _BATTLE_ARENA_SCRIPT_EXEC_TYPE
{
	BASET_UNKNOWN				= 0,

	BASET_CREATE_INSTANCE		= 1,
	BASET_ENTER_INSTANCE		= 2,
	BASET_START_INSTANCE		= 3,
	BASET_LEAVE_INSTANCE		= 4,
	BASET_END_INSTANCE			= 5,
	BASET_DESTROY_INSTANCE		= 6,

	BASET_KILL_PLAYER			= 7,
	BASET_BINGO					= 8,
};


// 방에 있는 유저들 집에 보내기용 Functor
class	BattleArenaQuitFunctor : public ArObjectFunctor
{
public:

	BattleArenaQuitFunctor( _ARENA_LEAVE_TYPE eLeaveType,
							const BattleArenaBaseServer* pArenaBase,
							unsigned char nInstanceNo );

	virtual void	operator()( ArObject* pObj )	const;

private:

	_ARENA_LEAVE_TYPE				m_eLeaveType;
	const BattleArenaBaseServer*	m_pArenaBase;
	unsigned char					m_nInstanceNo;

};


// 아무일 안하는 Functor
class	BattleArenaNULLFunctor : public ArObjectFunctor
{
public:

	BattleArenaNULLFunctor()
	{
	}

	virtual void	operator()( ArObject* pObj )	const
	{
	}

};


bool			IsValidBattleArenaInstanceNo( unsigned char nInstanceNo );

_BATTLE_GRADE	GetBattleArenaGrade( int nLevel );
bool			IsValidBattleArenaGrade( _BATTLE_GRADE eGrade );


// 직접 호출해서 사용하지 말 것. 이 함수 대신 BattleArena::LockWholeArena, BattleArenaInstance::LockWholeInstance 함수를 사용할 것.
ArcadiaLock		_LockWholeBattleArena( const BattleArenaBaseServer* pArenaBase, unsigned short nLayer = -1 );


// 아레나 이벤트별 스크립트에 대한 태그 처리 결과 반환
std::string		GetBattleArenaEventScript( const BattleArenaBaseServer* pArenaBase,
										  _BATTLE_ARENA_SCRIPT_EXEC_TYPE eExecType,
										  unsigned char nInstanceNo,
										  const std::vector< std::string >* pvTagReplacement = NULL );

void			BattleArenaRunScriptWithLog( const BattleArenaBaseServer* pArenaBase,
											_BATTLE_ARENA_SCRIPT_EXEC_TYPE eExecType,
											unsigned char nInstanceNo,
											const std::string& strScript,
											const StructPlayer* pExecutor );

// 각 상황에 맞는 지역락 -> csArena -> csBattle을 걸고 호출해야 함
// * 스크립트 안에서 호출하는 함수가 BattleArenaManager의 멤버 함수를 호출하는 경우 csArena/csBattle을 중첩으로 걸 수도 있으나,
//   CRITICAL_SECTION의 특성에 따라 중첩해서 거는 것이 허용됨.
//   단, csArena/csBattle을 걸고 BattleArenaRunEventScript를 호출하는 경우에는 반드시 현재 걸려있는 csArena/csBattle만 걸어야 함.
//   csArena/csBattle은 실제로 1개가 아니고 각 아레나별/인스턴스별로 여러 개가 생성되어 있기 때문에
//   BattleArenaRunEventScript 함수를 호출하는 시점에 걸려있던 csArena/csBattle과 이름은 같지만 다른 다른 인스턴스를
//   스크립트 안에서 걸게 되면 데드락을 유발하게 됨(이름은 같지만 서로 다른 락을 규칙성 없이 교차해서 걸게 되므로)
void			BattleArenaRunEventScript( const BattleArenaBaseServer* pArenaBase,
										  _BATTLE_ARENA_SCRIPT_EXEC_TYPE eExecType,
										  unsigned char nInstanceNo,
										  const StructPlayer* pExecutor,
										  const std::vector< std::string >* pvTagReplacement = NULL );

