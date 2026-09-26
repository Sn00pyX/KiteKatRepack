
#pragma once

#include <mmo/ArcadiaServer.h>

#include "BattleArenaCommon.h"
#include "ThreadSafeIntMap.h"
#include "BattleArenaBase.h"
#include "StructMonster.h"
#include "StructFieldProp.h"
#include "StructPlayer.h"



class	BattleArenaInstance : public StructMonster::MonsterDeleteHandler, public StructFieldProp::FieldPropDeleteHandler
{
public:

	// 플레이어의 경기 내 플레이 데이터
	struct PlayerInfo
	{
		PlayerInfo( StructPlayer* _pPlayer )
			: pPlayer( _pPlayer )
			, nPlayerUID( _pPlayer->GetPlayerUID() )
			, nPartyID( _pPlayer->GetPartyID() )
			, nJobID( _pPlayer->GetJobId() )
			, nPenaltyCount( _pPlayer->GetBattleArenaPenaltyCount() )
			, nPendedEnterTime( 0 )
			, nOfflineLimitTime( 0 )
			, bWaitingAbsenceCheck( false )
			, nLastAbsenceCheckTime( 0 )
			, nKillCount( 0 )
			, nDeathCount( 0 )
			, nPropActivateCount( 0 )
			, nTotalGainAPByKill( 0 )
			, nTotalGainAPByProp( 0 )
			, nTotalGainAPByResult( 0 )
			, bIsMVP( false )
		{}

		StructPlayer*	pPlayer;			// 유저가 온라인 상태일 때만 NULL이 아님
		PlayerUID		nPlayerUID;
		int				nPartyID;
		int				nJobID;

		int				nPenaltyCount;		// 오프라인 상태로 경기가 종료될 경우 페널티 카운트를 참조할 수 없기 때문에 사본 보관(오프라인이 되는 시점과 재접 시점에 추가 갱신)

		// 자동 입장될 시점을 기록해 두었다가 입장되는 시점에 0으로 설정(중복해서 입장 처리를 하지 않도록 하는데에도 사용)
		AR_TIME			nPendedEnterTime;
		// 플레이어가 접속 종료로 인해 로그아웃된 이후에 경기에서 강제 이탈되기까지 기다려줄 시점(직접 캐릭터 선택화면으로 가거나 하면 즉시 페널티 먹이고 PlayerInfo 삭제함)
		AR_TIME			nOfflineLimitTime;
		// 잠수 신고를 받아 응답을 기다리는 중인지 여부
		bool			bWaitingAbsenceCheck;
		// 마지막으로 잠수 신고를 당한 시점(각 유저별 잠수 신고 응답 제한 시간, 쿨타임 계산용)
		AR_TIME			nLastAbsenceCheckTime;
		short			nKillCount;
		short			nDeathCount;
		short			nPropActivateCount;
		int				nTotalGainAPByKill;
		int				nTotalGainAPByProp;
		int				nTotalGainAPByResult;
		// 경기가 종료된 후에 MVP 였는지 여부(MVP가 한 경기에서 여러 명이 존재할 수 있다는 기획 내용에 따라 각 플레이어의 속성으로 처리함)
		bool			bIsMVP;
	};

public:

	BattleArenaInstance( const BattleArenaBaseServer* pArenaBase,
						ThreadSafeIntMap* pPartyToArenaMap,
						ThreadSafeIntMap* pPartyToInstanceMap,
						unsigned char nInstanceNo );

	void				Init( _BATTLE_GRADE eGrade, AR_TIME nStartTime );
	void				SetDefaultTimeByStartTime( AR_TIME nStartTime );

	ArcadiaLock			LockWholeInstance() const;

	// { csBattle을 걸고 호출해야 하는 함수들

	void				_initInstance();

	// 현재 경기에 대한 내부 처리를 진행하며 경기를 지속해야 하는 경우에는 true를, 경기를 종료시켜서 제거해야 하는 경우에는 false를 리턴함
	void				_onProcess();

	// nPartyID 에 해당하는 파티의 팀 번호를 얻음.
	// 정상 리턴값의 범위는 [ 0 ~ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ) 이며,
	// 리턴값이 GameRule::BATTLE_ARENA_MAX_TEAM_COUNT 라면 현재 경기에 참여되어 있지 않은 파티임을 의미함.
	int					_getTeamNo( int nPartyID ) const;
	int					_getTeamMemberCount( int nTeamNo )	const;

	bool				_isQuickJoinAble()	const;
	unsigned int		_getQuickJoinPriority() const;

	// 플레이어를 경기에 최초 추가(PlayerInfo 추가됨)
	unsigned short		_addPlayer( StructPlayer* pPlayer, int nTeamNo = INVALID_BATTLE_ARENA_TEAM_NO );
	// 플레이어를 경기에서 제거(PlayerInfo 삭제됨)
	unsigned short		_removePlayer( StructPlayer* pPlayer, _ARENA_LEAVE_TYPE eLeaveType );		// 온라인이었던(혹은 온라인 상태인) 플레이어 처리용
	unsigned short		_removePlayer( PlayerUID nPlayerUID, _ARENA_LEAVE_TYPE eLeaveType );		// _playerLeave를 통해 이미 로그아웃돼서 StructPlayer 구조체가 없는 플레이어 처리용
	void				_deleteBattleArenaPartyPlayer( int nPartyID, StructPlayer* pPlayer );	// 배틀 아레나 파티 멤버를 삭제한다.

	void				_removeAllPlayer( _ARENA_LEAVE_TYPE eLeaveType, ArObjectFunctor& functor );

	// 플레이어가 경기에 참여되어 있던 상태에서 이탈만 한 경우(접속 끊김 - 캐릭터 선택 화면으로 이동하는 건 해당 안 됨)
	unsigned short		_disconnectPlayer( StructPlayer* pPlayer );
	// 플레이어가 경기에 이미 참여되어 있었는데 다시 입장하는 경우(제한 시간 내 재접)
	unsigned short		_reconnectPlayer( StructPlayer* pPlayer, ArPosition* pPosStart, unsigned char* pnLayer );
	// _reconnectPlayer가 성공했던 유저가 로그인 시 데이터 로드 시점과 클라이언트에 방송 시점 차로 인해 처리하지 못했던 것을 후 처리해주는 함수
	void				_reconnectPlayerPostProc( StructPlayer* pPlayer ) const;

	unsigned short		_pendEnterWhileCountdown( StructPlayer* pPlayer );
	// nPropIndex는 0은 없음을, 그 외의 값이 실제 인덱스를 의미함
	unsigned short		_activateProp( StructPlayer* pPlayer, int nPropIndex );
	_PROP_STATE			_getPropState( int nPropIndex ) const;
	void				_setPropState( int nPropIndex, _PROP_STATE ePropState );
	bool				_isCastablePropForTeam( const StructFieldProp* pProp, int nTeamNo ) const;

	unsigned short		_onPlayerDead( StructPlayer* pPlayer, StructPlayer * pKiller );

	unsigned short		_requestAbsenceCheck( StructPlayer* pPlayer, AR_HANDLE hCheckTarget );
	unsigned short		_answerAbsenceCheck( StructPlayer* pPlayer, bool bSuccess );

	unsigned short		_setReadyExerciseGame( StructPlayer* pPlayer, bool bReady );

	size_t				_doEachPlayer( ArObjectFunctor& fo );
	size_t				_doEachPlayerInTeam( int nTeamNo, ArObjectFunctor& fo );

	// 바로 밑에 있지만 여기서 미리 함수 선언하려고 구조체도 선언만 먼저 해 둠
	PlayerInfo*			_getPlayerInfo( PlayerUID nPlayerUID, int nTeamNo = INVALID_BATTLE_ARENA_TEAM_NO );
	const PlayerInfo*	_getPlayerInfo( PlayerUID nPlayerUID, int nTeamNo = INVALID_BATTLE_ARENA_TEAM_NO ) const;

	const std::vector< PlayerInfo >*	_getPlayerInfoList( int nTeamNo )	const;
	std::vector< PlayerInfo >*			_getPlayerInfoList( int nTeamNo );

	// 인스턴스 던전 플래그 설정
	// empty value 값 = 빈 문자열
	void				_setFlag( const char* pszName, const char* pszValue );
	const std::string &	_getFlag( const char* pszName ) const;
	bool				_delFlag( const char* pszName );

	// 스크립트 실행 예약(아레나 내부 지역락 문제 때문에 사용)
	void				_pendScript( StructPlayer* pExecutor, enum _BATTLE_ARENA_SCRIPT_EXEC_TYPE eExecType, const char* pszScript );
	void				_procPendedScript();

	void				_initialRespawn();
	void				_procMonsterRespawn();
	void				_procFieldPropRespawn();

	void				_clearMonster();
	void				_clearFieldProps();
	void				_clearArenaBlockerFieldProps();
	void				_clearItems();

	// 진행 중이던 경기의 종료 및 보상 지급, 인스턴스 최종 해제(이 때 유저 강제로 내쫓음) 처리 타이밍 설정
	void				_finishBattle( _ARENA_END_TYPE eEndType );
	// 인스턴스를 해제하기 직전에 모든 파티 해산, 유저 강제 퇴장 등
	void				_wrapUpInstance( _ARENA_LEAVE_TYPE eLeaveType );

	// } csBattle을 걸고 호출해야 하는 함수들

	// * BattleArenaInstance의 멤버로 있는 락을 함수 caller 측에서 걸고 함수를 호출하게 하는 이유는
	//   1. 하나의 BattleArenaInstance에 대해 락이 걸린 채로 여러 개의 작업을 연달아 처리해야 할 때가 있음
	//   2. BattleArenaInstance 메모리 삭제 처리 시 csBattle의 잠금 상태를 BattleArenaInstance의 소유자 레벨(BattleArena 또는 BattleArenaManager)에서 제어해야 하므로

	// { 타 클래스에서 호출하는 핸들러 함수들(어떤 락도 걸리지 않은 채로 호출됨)
	virtual void	onMonsterDelete( StructMonster* pMonster );
	virtual void	onFieldPropDelete( StructFieldProp* pProp );
	// } 타 클래스에서 호출하는 핸들러 함수들(어떤 락도 걸리지 않은 채로 호출됨)

private:

	_ARENA_END_TYPE	GetFinishType( AR_TIME tCurrent )	const;
	void			RemoveAbsentAndOfflineLimitExpiredPlayer( AR_TIME tCurrent );
	void			TryStart( AR_TIME tCurrent );

	int				GetLeastMemberCountTeam()	const;
	unsigned short	FixSameAlias( StructPlayer* pPlayer )	const;

	int				CreateBattleArenaParty( int nTeamNo, StructPlayer* pPlayer );
	bool			LinkBattleArenaParty( int nLeaderPartyID, int nJoinPartyID );

	unsigned short	LeavePlayer( PlayerInfo* pPlayerInfo, int nTeamNo, _ARENA_LEAVE_TYPE eLeaveType );
	unsigned short	LeaveBattleArenaParty( int nTeamNo,
										int nPartyID,
										PlayerUID nPlayerUID,
										StructPlayer* pPlayer,
										const char* pAlias,
										_ARENA_LEAVE_TYPE eLeaveType );

public:

	// { 인스턴스 생성 시 한 번 세팅되면 대개 바뀌지 않는 정보들 - 접근 시 락 필요 없음
	const BattleArenaBaseServer*			m_pArenaBase;
	ThreadSafeIntMap*						m_pPartyToArenaMap;
	ThreadSafeIntMap*						m_pPartyToInstanceMap;

	// 방 번호. Layer 번호로도 사용되며, 각 BattleArena 별로 방 번호를 따로 배정하므로 서로 다른 BattleArena와는 중복된 번호가 있을 수 있음
	// 방 번호 값은 1 이상 MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA 이하의 값만 발생될 수 있음
	unsigned char							m_nInstanceNo;
	// 랭크별 등급(1개의 BattleArena에 대해서도 서로 다른 랭크의 게임이 동시에 진행되므로 BattleArenaInstance의 속성이 됨)
	_BATTLE_GRADE							m_eGrade;

	// nStartTime은 경기가 생성된 후에 입장 카운트 다운이 종료되면서 실제 경기가 시작되는 시점(몹/프랍 리젠도 여기서 함)
	AR_TIME									m_nStartTime;
	AR_TIME									m_nQuickJoinLimitTime;
	AR_TIME									m_nEndTime;					// 경기 종료 시점에 실제 종료 시점 기준으로 값이 한 번 변경되기 때문에 락이 아예 필요없진 않지만 값 변경은 bEnd의 상태를 기준으로 이루어지므로 bEnd가 락에 의해 보호되고 있어서 이건 락 안 걸어도 크게 문제 없음
	AR_TIME									m_nForceQuitTime;				// 경기 종료 시점에 실제 종료 시점 기준으로 값이 한 번 변경되기 때문에 락이 아예 필요없진 않지만 값 변경은 bEnd의 상태를 기준으로 이루어지므로 bEnd가 락에 의해 보호되고 있어서 이건 락 안 걸어도 크게 문제 없음
	// } 인스턴스 생성 시 한 번 세팅되면 대개 바뀌지 않는 정보들 - 접근 시 락 필요 없음

	// { 런타임에 변경되는 것들 - 접근 시 csBattle을 걸어야 함
	// (지역 락 -> csArena -> csBattle -> PartyManager::m_IntfLock)
	mutable XCriticalSection				m_csBattle;

	// 외부에 있는 유저를 아레나 내부로 워프시켜야 할 때 락 관련 이슈로 인해 대개의 경우 워프시켜야 하는 시점에 직접 처리할 수 없으므로 이 벡터에 넣어뒀다가
	// BattleArenaManager::onProcess에서 모아서 각 인스턴스에 대한 처리가 진행될 때마다 해당 유저들에 맞는 지역락을 걸어주고 워프시키도록 처리
	std::vector< StructPlayer* >			m_vPendedEnterPlayerList;

	// 현재 경기에서 오프라인 시간 제한으로 퇴장되며 페널티를 받은 유저의 PlayerUID 목록
	// * 경기 종료 시 BattleArenaManager::RemoveOfflinePenaltyReceivedPlayer을 호출해야 함
	std::vector< PlayerUID >				m_vOfflinePenaltyReceivedPlayerUIDList;

	bool									m_bStart;						// 경기 시작 여부(입장 처리 완료 및 최초 몹/프랍 리젠 처리, 입장 제한 프랍 제거 처리 여부와 동일)
	bool									m_bEnd;						// 경기 종료 여부(경기 종료 후 인스턴스 삭제 처리를 BattleArenaManager::onProcess에서 하기 위해 사용되는 플래그 - 몹/프랍 리젠 방지용으로도 쓰임)
	int										m_nWinTeamNo;				// 승리팀
	_ARENA_REWARD_TYPE						m_eRewardType;				// 보상 타입
	bool									m_bObjectiveScoreReached;		// 목표 점수 도달 여부(플레이어 킬로 점수를 쌓았을 때 목표 점수가 도달되면 바로 _finishBattle을 호출하면 락 문제가 생겨서 별도 플래그 사용)

	struct TeamInfo
	{
		short		nTotalKillCount;	// 각 팀별 킬 수 총합
		short		nTotalDeathCount;	// 각 팀별 데스 수 총합
		short		nScore;				// 각 팀별 현재 점수
		c_fixed10	fKillScoreRate;		// 각 팀별 킬에 따른 점수 추가 시 적용 배율(개인용 실시간 AP 지급에는 관여하지 않으며, 최초 구현에서는 슬로터 타입 전용)
	};
	TeamInfo								m_aTeamInfo[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ];

	std::map< std::string, std::string >	m_mapInstanceFlag;												// 경기 인스턴스별 플래그 데이터

	// 한 팀에 8인 이상이 참여하는 경기의 경우 각 팀의 PartyID가 단일 파티가 아닌 공대 파티일 수 있으므로 시스템 전반에서의 처리를 공대 기준으로 해야 함
	// PartyID에 따른 팀 구분은 _getTeamNo 함수 구현 참조
	int										m_anPartyID[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ][ GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM ];

	// * 유저가 접속 끊김에 의해 이탈해도 이 기록은 삭제하지 않고 보존하여
	//   유저가 로그아웃했다가 제한 시간 이내에 재접속하지 않았을 경우 이탈 처리하는 데 사용함.
	//   BattleArenaManager::onProcess에서 시간 초과로 인한 이탈 처리 발생 시 페널티 부여와 함께 해당 유저의 참여 정보 제거.
	//   하지만 유저가 자의적으로 이탈하는 경우(캐릭터 선택화면으로, 워프 등)에는 이탈 즉시 페널티 부여 및 참여 정보 제거.
	// 유저의 플레이 데이터를 팀별로 구분해서 보관(팀별로는 구분하지만 팀 내의 파티별로는 구분하지 않음)
	std::vector< PlayerInfo >						m_vPlayerInfo[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ];

	typedef std::pair< const BATTLE_ARENA_MONSTER_RESPAWN*, AR_TIME > PendedMonsterRespawnInfo;
	std::vector< PendedMonsterRespawnInfo >			m_vPendedMonsterRespawn;
	std::vector< StructMonster* >					m_vRespawnedMonster;

	typedef std::pair< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO*, AR_TIME > PendedFieldPropRespawnInfo;
	std::vector< PendedFieldPropRespawnInfo >		m_vPendedFieldPropRespawn;
	std::vector< StructFieldProp* >					m_vRespawnedFieldProp;

	std::vector< StructFieldProp* >					m_vRespawnedArenaBlockerFieldProp;

	struct PendedScript
	{
		PendedScript( StructPlayer* _pExecutor, enum _BATTLE_ARENA_SCRIPT_EXEC_TYPE _eExecType, const std::string& _strScript )
			: pExecutor( _pExecutor )
			, eExecType( _eExecType )
			, strScript( _strScript )
		{}

		StructPlayer*							pExecutor;
		enum _BATTLE_ARENA_SCRIPT_EXEC_TYPE		eExecType;
		std::string								strScript;
	};
	std::vector< PendedScript >					m_vPendedScript;
	// } 런타임에 변경되는 것들 - 접근 시 csBattle을 걸어야 함
};
