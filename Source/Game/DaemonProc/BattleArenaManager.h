
#pragma once

#include "BattleArena.h"


// 아래와 같은 구성도로 아레나 데이터가 이루어져서 처리됨
//
// BattleArena의 인스턴스 수 까지는 서버 로딩 중 채워진 정적 데이터가 되며,
// BattleArenaInstance와 BattleArena 멤버 중 BattleArenaInstance들과 관련된 변수들만 유저가 게임을 생성/종료함에 따라 증가/감소됨
//
//                                              BattleArenaManager
//                      ┌────────────────────────────────┼─────────── ...
//                 BattleArena                      BattleArena          ...
//          ┌───────────┼────────...                    ...
// BattleArenaInstance BattleArenaInstance ...
//
// * 데이터 접근 순서와 락의 순서는 지역 락을 최우선으로 하고 위에서부터 아래로만 허용함
//   아래쪽의 클래스가 위쪽의 클래스의 멤버를 참조하기 위해 락을 걸거나 하면 데드락이 됨


// StructPlayer::SetUseAlias를 BattleArenaManager에서 사용하는 방식
//
// - 연습 경기는 어떤 루트로 참여하더라도 SetUseAlias( true )를 호출하지 않은 상태(IsUsingAlias() == false)로 참여돼야 함
// - 대기열에 대기시킬 때에도 SetUseAlias( true )를 호출하지 않은 상태(IsUsingAlias() == false)로 대기열에 들어가야 함
//   단, 향후 alias를 사용하는 다른 시스템이 추가되고 대기열에 대기된 상태로 해당 시스템을 이용할 수 있다면
//   (대신 경기장 내에 진입할 때는 해당 시스템의 이용이 중단돼야 함 - 가명 룰 중복 때문) 대기열에 있다고 해서 무조건 false라고 가정하면 안됨
//   이 가정은 BattleArenaManager::QuickJoin 함수에서 현재 진행 중인 경기에 참여시도를 하기 위해 BattleArenaInstance::_addPlayer를 호출해 보도록
//   되어 있는데, 이 때 SetUseAlias( true ) 를 막무가내로 호출해보고 실패하면 다시 SetUseAlias( false )로 덮는 형태로 반영되어 있음
//   수정이 필요하게 되면 _addPlayer 호출 전에 IsUsingAlias()의 결과값을 보관했다가 실패하면 해당 결과값 상태로 되돌려줘야 함
//   그리고 QuickJoin에서 진행 중인 경기 참여에 실패해서 대기열에 대기되는 시점에는 기존의 설정을 그대로 유지시켜주기 위해
//   SetUseAlias를 아예 호출하지 않아야 함(대신 BattleArenaWithWaitQueue::_createNewBattle 에서 _addPlayer 하기 직전에 SetUseAlias( true ) 호출하게 되어 있음)
// - 연습 경기 외의 경기(BattleArenaWithWaitQueue에 소속된 경기)에 BattleArenaInstance::_addPlayer 를 호출하기 전에 SetUseAlias( true )가 되어 있어야 함
//   파티에 참여되면서 온갖 정보가 클라이언트들(기존 파티 참여자 및 자신)로 방송되는데, 이 때 가명이 사용되어야 하기 때문
// - BattleArenaInstance::_addPlayer 를 호출했다가 실패하는 경우에는 Caller 측에서 SetUseAlias( false )를 호출해 줘야 함
// - BattleArenaInstance::_removePlayer 를 통해서 경기에서 이탈되는 경우에는 SetUseAlias( false )를 _removePlayer Caller 측에서 호출하지 않아도 됨
//   * _removePlayer 안에서 SetUseAlias( false )를 호출하도록 되어 있음
// - BattleArenaInstance::_wrapUpInstance 를 통해서 유저가 경기에서 이탈되는 경우에도 _wrapUpInstance Caller 측(BattleArenaManager::onProcess)에서
//   SetUseAlias( false )를 호출해 줄 필요는 없음
//   * _wrapUpInstance 안에서 SetUseAlias( false )를 호출하도록 되어 있음
// 간단히, 정리하면 아래와 같음
// 1. 대기열에 있을 때는 노 터치(현재는 alias를 쓰는 다른 시스템이나 일반 파티가 없으므로 무조건 false 취급)
// 2. _addPlayer 호출 전에는 연습 경기가 아닐 때만 Caller 측에서 SetUseAlias( true ) 호출, 실패하면 다시 false 혹은 원래 설정으로 복귀시켜 줌(연습 경기는 기획상 가명 사용 불가)
// 3. 경기에서 이탈하는 경우(_removePlayer, _wrapUpInstance)에는 Caller 쪽에서 신경 쓸 필요 없고, 무조건 SetUseAlias( false )가 됨


class	BattleArenaManager : public ArSchedulerObject
{
protected:

	BattleArenaManager();
	virtual ~BattleArenaManager();

public:

	static BattleArenaManager&	Instance()
	{
		static BattleArenaManager _inst;
		return _inst;
	}

	unsigned short		RegisterArena( const BattleArenaBaseServer* pArenaBase );

	bool				Init();
	bool				DeInit();
	void				RaiseUpdatePriority();
	void				DownUpdatePriority();

	unsigned short		checkJoinable( StructPlayer* pPlayer, bool bIgnorePenalty );

	unsigned short		JoinWaitQueue( StructPlayer* pPlayer, int nArenaID );
	unsigned short		QuickJoin( StructPlayer* pPlayer );

	unsigned short		QuitGame( StructPlayer* pPlayer, bool bByKick, bool bAvoidPenalty, _ARENA_LEAVE_TYPE eLeaveType );
	unsigned short		QuitGame( int nPartyID, PlayerUID nPlayerUID, bool bByKick, _ARENA_LEAVE_TYPE eLeaveType );

	unsigned short		EnterArenaWhileCountdown( StructPlayer* pPlayer );

	unsigned short		OnPlayerDead( StructPlayer* pPlayer, StructPlayer* pKiller );
	unsigned short		OnDisconnect( StructPlayer* pPlayer );
	unsigned short		OnReconnect( StructPlayer* pPlayer, ArPosition* pPosStart, unsigned char* pnLayer );
	void				OnReconnectPostProc( StructPlayer* pPlayer );

	size_t				DoEachPlayerInBattleInstance( int nArenaID, unsigned char nInstanceNo, ArObjectFunctor& fo );
	size_t				DoEachPlayerInBattleTeam( int nArenaID, unsigned char nInstanceNo, int nTeamNo, ArObjectFunctor& fo );

	// 빙고/슬래터 프랍 동작
	unsigned short		ActivateBattleInstanceProp( StructPlayer* pPlayer, int nPropIndex );

	// 잠수 신고 관련
	unsigned short		RequestAbsenceCheck( StructPlayer* pPlayer, AR_HANDLE hCheckTarget );
	unsigned short		AnswerAbsenceCheck( StructPlayer* pPlayer, bool bSuccess );

	// 연습 경기 관련
	unsigned short		CreateExerciseGame( StructPlayer* pPlayer );
	unsigned short		JoinExerciseGameOpponentLeader( StructPlayer* pPlayer, int nInviterPartyID );
	unsigned short		JoinExerciseGameMember( StructPlayer* pPlayer, int nPartyID );
	unsigned short		SetReadyExerciseGame( StructPlayer* pPlayer, bool bReady );
	unsigned short		StartExerciseGame( StructPlayer* pPlayer );
	bool				IsReadyExerciseTeam( int nPartyID ) const;
	bool				HasOpponent( StructPlayer* pPlayer ) const;
	bool				IsMoreMemberInvitable( StructPlayer* pPlayer ) const;

	// 경기 관련 공통
	bool				SetBattleInstanceFlag( int nArenaID, unsigned char nInstanceNo, const char* pszName, const char* pszValue );
	std::string			GetBattleInstanceFlag( int nArenaID, unsigned char nInstanceNo, const char* pszName ) const;
	int					GetTeamNo( const StructPlayer* pPlayer ) const;
	bool				IsOpponent( const StructPlayer* pPlayer, const StructPlayer* pTarget ) const;
	bool				GetRevivePosition( const StructPlayer* pPlayer, ArPosition& posRevive ) const;
	bool				IsCastablePropForTeam( int nArenaID, unsigned char nInstanceNo, const StructFieldProp* pProp, int nPartyID ) const;
	// 경기장 내에서 경기 시작 전에 시작 영역 밖으로 벗어나지 못하도록 제한하기 위해 onMoveRequest에서 사용
	// 함수 내부에서 vMoveInfo가 수정 없이 그대로 리턴되는 경우는 false, vMoveInfo가 조정된 경우는 true를 리턴
	bool				ValidateMovableRoute( int nArenaID, unsigned char nInstanceNo, int nPartyID, const ArPosition& posStart, std::vector< ArPosition >& vMoveInfo ) const;

	// 오프라인 상태로 페널티를 받은 캐릭터인지 여부 리턴(페널티를 부여한 경기가 진행 중인 상태에서만 true 반환)
	// * 만일 true가 리턴되면 오프라인 페널티 유저 목록에서 nPlayerUID를 제거함(RemoveOfflinePenaltyReceivedPlayer을 따로 호출하지 않아도 됨)
	bool				IsOfflinePenaltyReceivedPlayer( PlayerUID nPlayerUID );
	void				AddOfflinePenaltyReceivedPlayer( PlayerUID nPlayerUID );
	void				RemoveOfflinePenaltyReceivedPlayer( PlayerUID nPlayerUID );

	virtual void		onProcess( int nThreadIdx );

	// BattleArenaManager 멤버 함수에서만 사용할 함수들
private:

	BattleArena*		getArena( int nArenaID );
	const BattleArena*	getArena( int nArenaID ) const;

private:

	bool							m_bInit;
	std::vector< BattleArena* >		m_vArenaList;

	ThreadSafeIntMap				m_partyToArenaMap;

	mutable XCriticalSection		m_csOfflinePenaltyReceivedPlayer;
	std::vector< PlayerUID >		m_vOfflinePenaltyReceivedPlayerUIDList;

	// 패킷 관련 함수들
public:

	static void	assemblePlayerScoreFromPlayerInfo( TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* pPlayerScore, const BattleArenaInstance::PlayerInfo* pPlayerInfo );
	static void	BroadcastBattleArenaMessage( const BattleArenaInstance* pBattleInstance, const TS_MESSAGE* pMsg );

	static void SendBattleArenaJoinQueueMessage( const StructPlayer* pPlayer, const BattleArena* pArena, _BATTLE_GRADE eGrade );
	static void BroadcastWaitUserCountMessage( const BattleArena* pArena, _BATTLE_GRADE eGrade );
	static void SendBattleArenaLeaveMessage( const StructPlayer* pPlayer, _ARENA_LEAVE_TYPE eLeaveType );
	static void SendBattleArenaLeaveMessage( const StructPlayer* pPlayer, const char* pszLeaverName, _ARENA_LEAVE_TYPE eLeaveType );
	static void SendBattleArenaBattleInfoMessage( const StructPlayer* pPlayer, const BattleArenaInstance* pBattleInstance );
	static void BroadcastBattleArenaExerciseReadyState( const BattleArenaInstance* pBattleInstance );
	static void BroadcastBattleArenaBattleInfoMessage( const BattleArenaInstance* pBattleInstance );
	static void SendBattleArenaBattleStatusMessage( const StructPlayer* pPlayer, const BattleArenaInstance* pBattleInstance );
	static void BroadcastBattleArenaBattleStatusMessage( const BattleArenaInstance* pBattleInstance );
	static void SendBattleArenaScoreMessage( const StructPlayer* pPlayer, const BattleArenaInstance* pBattleInstance );
	static void BroadcastBattleArenaScoreUpdateMessage( const BattleArenaInstance* pBattleInstance, const StructPlayer* pPlayer, int nTeamNo );
	static void BroadcastBattleArenaScoreUpdateMessage( const BattleArenaInstance* pBattleInstance, const StructPlayer* pPlayer1, int nTeamNo1, const StructPlayer* pPlayer2, int nTeamNo2 );
	static void BroadcastBattleArenaJoinBattleMessage( const BattleArenaInstance* pBattleInstance, const StructPlayer* pPlayer );
	static void BroadcastBattleArenaLeaveBattleMessageWithLog( const BattleArenaInstance* pBattleInstance, const BattleArenaInstance::PlayerInfo* pPlayerInfo, const StructPlayer* pPlayer, const int nWinTeamNo, const int nTeamNo, const char* pszLeaverName, _ARENA_LEAVE_TYPE eLeaveType );
	static void BroadcastBattleArenaDisconnectBattleMessage( const BattleArenaInstance* pBattleInstance, const StructPlayer* pPlayer );
	static void BroadcastBattleArenaReconnectBattleMessage( const BattleArenaInstance* pBattleInstance, const StructPlayer* pPlayer );
	static void BroadcastBattleArenaFinalScoreMessage( const BattleArenaInstance* pBattleInstance );
	static void BroadcastBattleArenaResultWithLog( const BattleArenaInstance* pBattleInstance, _ARENA_END_TYPE eEndType, _ARENA_REWARD_TYPE eRewardType, int nWinnerTeamNo, int nScore1, int nScore2, AR_TIME nPlayTime );
	static void SendBattleArenaAbsenceCheck( const StructPlayer* pPlayer, AR_TIME nLimitTime );
	static void SendBattleArenaPenaltyMessage( const struct StructPlayer* pPlayer );
};
