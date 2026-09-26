
#include <mmo/ArRegion.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "BattleArenaInstance.h"
#include "DB_Commands.h"
#include "PartyManager.h"
#include "BattleArenaManager.h"
#include "NPCProc.h"
#include "GameProc.h"
#include "StructSummon.h"
#include "ThreadPlayerHelper.h"


static void	LeaveParty( int nPartyID, StructPlayer* pPlayer )
{
	// 파티장이면서 멤버가 1명만 남아있거나 다른 파티멤버에게 자동 인계가 실패하면 파티 해산
	// 그 외의 모든 경우에는 그냥 파티 탈퇴
	if( PartyManager::GetInstance().IsLeader( nPartyID, pPlayer->GetPlayerUID() ) )
	{
		if( PartyManager::GetInstance().GetMemberCount( nPartyID ) == 1 || !PartyManager::GetInstance().AutoPromote( nPartyID, false ) )
		{
			BroadcastPartyDestroy( nPartyID, true );

			LOG::Log11N4S( LM_PARTY_DESTROY,
				pPlayer->GetAccountID(), pPlayer->GetSID(),
				nPartyID, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
				1,
				0, 0, 0, 0,
				pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
				PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS,
				"", LOG::STR_NTS );

			PartyManager::GetInstance().DestroyParty( nPartyID );
			return;
		}
		else
		{
			PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "PROMOTE|%d|%s|",
				nPartyID, PartyManager::GetInstance().GetLeaderDisplayName( nPartyID ).c_str() );
		}
	}

	BroadcastPartyLeave( pPlayer, true );

	LOG::Log11N4S( LM_PARTY_LEAVE,
		pPlayer->GetAccountID(), pPlayer->GetSID(), nPartyID,
		pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
		1,
		0, 0, 0, 0,
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
		PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS,
		"", LOG::STR_NTS );

	PartyManager::GetInstance().LeaveParty( nPartyID, pPlayer->GetPlayerUID() );
}

BattleArenaInstance::BattleArenaInstance( const BattleArenaBaseServer* pArenaBase,
										 ThreadSafeIntMap* pPartyToArenaMap,
										 ThreadSafeIntMap* pPartyToInstanceMap,
										 unsigned char nInstanceNo )
	: m_pArenaBase( pArenaBase )
	, m_pPartyToArenaMap( pPartyToArenaMap )
	, m_pPartyToInstanceMap( pPartyToInstanceMap )
	, m_nInstanceNo( nInstanceNo )
	, m_csBattle( "BattleArenaInstance::csBattle" )
{
	m_bStart = false;
	m_bEnd = false;
	m_nWinTeamNo = 0;
	m_bObjectiveScoreReached = false;
	::memset( m_aTeamInfo, 0, sizeof( m_aTeamInfo ) );
	::memset( m_anPartyID, 0, sizeof( m_anPartyID ) );

	for( int i = 0; i < GameRule::BATTLE_ARENA_MAX_TEAM_COUNT; ++i )
	{
		m_vPlayerInfo[i].reserve( GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM );
	}
}

void	BattleArenaInstance::Init( _BATTLE_GRADE _eGrade, AR_TIME _nStartTime )
{
	m_eGrade = _eGrade;

	SetDefaultTimeByStartTime( _nStartTime );

	m_vPendedEnterPlayerList.clear();

	m_bStart = false;
	m_bEnd = false;
	m_nWinTeamNo = INVALID_BATTLE_ARENA_TEAM_NO;
	m_eRewardType = ART_UNKNOWN;
	m_bObjectiveScoreReached = false;

	// 팀별 데이터 초기화는 pArenaBase->nTeamCount 개만 해도 되지만 만일의 경우를 위해서...(현재는 다 2이므로 어떻게 해도 같음)
	for( int nTeamNo = 0; nTeamNo < GameRule::BATTLE_ARENA_MAX_TEAM_COUNT; ++nTeamNo )
	{
		m_aTeamInfo[ nTeamNo ].nTotalKillCount = 0;
		m_aTeamInfo[ nTeamNo ].nTotalDeathCount = 0;
		m_aTeamInfo[ nTeamNo ].nScore = 0;
		m_aTeamInfo[ nTeamNo ].fKillScoreRate = 1;

		std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			pPlayerInfoList->clear();
		}
	}

	m_mapInstanceFlag.clear();

	memset( m_anPartyID, 0, sizeof( m_anPartyID ) );

	m_vPendedMonsterRespawn.clear();
	m_vRespawnedMonster.clear();

	m_vPendedFieldPropRespawn.clear();
	m_vRespawnedFieldProp.clear();

	m_vRespawnedArenaBlockerFieldProp.clear();

	m_vPendedScript.clear();
}

void	BattleArenaInstance::SetDefaultTimeByStartTime( AR_TIME nStartTime )
{
	m_nStartTime = nStartTime;
	if( nStartTime != INFINITE_TIME )
	{
		m_nQuickJoinLimitTime = m_nStartTime + m_pArenaBase->nQuickJoinLimitTime;
		m_nEndTime = m_nStartTime + m_pArenaBase->nBattleDuration;
		m_nForceQuitTime = m_nEndTime + GameRule::BATTLE_ARENA_FORCE_QUIT_DELAY;
	}
	else
	{
		m_nQuickJoinLimitTime = INFINITE_TIME;
		m_nEndTime = INFINITE_TIME;
		m_nForceQuitTime = INFINITE_TIME;
	}
}

ArcadiaLock	BattleArenaInstance::LockWholeInstance() const
{
	// 데드락 체크
	assert( !m_csBattle.IsLockedByCurrentThread() );

	return _LockWholeBattleArena( m_pArenaBase, m_nInstanceNo );
}

void	BattleArenaInstance::_initInstance()
{
	// 경기 생성 시점에 생성됐다가 경기 시작 시점에 사라져야 하는 프랍 리젠(시작 시점까지 경기장 내부로 못 들어가게 막는 프랍)
	for( std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO* >::const_iterator it = m_pArenaBase->vArenaBlockerFieldPropRespawnList.begin();
		it != m_pArenaBase->vArenaBlockerFieldPropRespawnList.end();
		++it )
	{
		const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO* pRespawn = (*it);

		StructFieldProp* pProp = StructFieldProp::Create( this, pRespawn, pRespawn->x, pRespawn->y, m_nInstanceNo );
		if( pProp == NULL )
		{
			FILELOG( "Unknown field prop is to be respawned in battle arena. ArenaID(%d), PropID(%d)", m_pArenaBase->nID, pRespawn->nPropId );
			_cprint( "Unknown field prop is to be respawned in battle arena. ArenaID(%d), PropID(%d)\n", m_pArenaBase->nID, pRespawn->nPropId );
			continue;
		}

		m_vRespawnedArenaBlockerFieldProp.push_back( pProp );
	}

	// 팀별 파티 결성 완료
	// 참여자에게 팀별 상세 정보 및 경기 시작 카운트(시작 시점) 방송
	BattleArenaManager::BroadcastBattleArenaBattleInfoMessage( this );

	LOG::Log11N4S( LM_BATTLE_ARENA_CREATE, 0, 0,
		m_pArenaBase->nID, m_nInstanceNo, m_eGrade,
		_getTeamMemberCount(0), _getTeamMemberCount(1),
		0, 0, 0, 0, "", LOG::STR_NTS, "", LOG::STR_NTS,
		"ARENA", LOG::STR_NTS,
		"", LOG::STR_NTS );
}

void	BattleArenaInstance::_onProcess()
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 현재 진행 중인 각 경기에 대한 개별 처리
	// - 현재 진행 중인 방이 목표 점수 도달된 경우에는 종료 및 승리 처리
	//   * 다른 조건보다 최우선적으로 처리되어야 승리 시 종료 처리를 즉각 하지 않고 여기서 하는 것으로 인한 피해를 줄일 수 있음(지역락 때문에...)
	// - 현재 진행 중인 방의 종료 조건 체크 및 종료 처리
	//   * 각 방에 대해 확정됐던 승리 처리 외의 다른 처리 이전에 종료 처리 먼저 해줘야 불필요하게 종료될 방에 대한 처리를 하는 걸 조금이라도 줄일 수 있음
	// - 시작된 경기의 경우
	//   * 오프라인 상태로 변경된 후 제한 시간 이내에 재접속하지 않은 유저 이탈 처리
	//   * 입장 예정 시간이 다 된 유저를 강제로 입장 예약(락 문제 때문에 여기서 바로 못 함)
	//   * 몬스터 리젠
	//   * 프랍 리젠
	// - 아직 시작되지 않은 경기의 경우
	//   * 시작 시간이 다 된 경우
	//     v 시작 상태로 변경
	//     v 시작 시점에 제거해야 하는 프랍(아레나 내부 진입 금지용 프랍) 제거
	//     v 경기 중에 사용될 모든 몬스터, 프랍 리젠
	//     v 시작 스크립트 실행(락 문제 때문에 여기서 바로 못 하고 BattleArenaManager::onProcess에서 경기가 시작된 인스턴스를 확인해서 처리해 줌)

	AR_TIME tCurrent = GetArTime();

	_ARENA_END_TYPE eEndType = GetFinishType( tCurrent );
	if( eEndType != AET_UNKNOWN )
	{
		_finishBattle( eEndType );
		return;
	}

	RemoveAbsentAndOfflineLimitExpiredPlayer( tCurrent );

	// 시작된 경기의 경우
	if( m_bStart )
	{
		// 몬스터 리젠
		_procMonsterRespawn();

		// 프랍 리젠
		_procFieldPropRespawn();
	}
	// 아직 시작되지 않은 경기의 경우
	else
	{
		TryStart( tCurrent );
	}
}

_ARENA_END_TYPE	BattleArenaInstance::GetFinishType( AR_TIME tCurrent )	const
{
	// { 현재 진행 중인 방이 목표 점수 도달된 경우에는 종료 및 승리 처리
	if( m_bObjectiveScoreReached )
	{
		return AET_MAX_SCORE_REACHED;
	}
	// } 현재 진행 중인 방이 목표 점수 도달된 경우에는 종료 및 승리 처리

	// { 현재 진행 중인 방의 종료 조건 체크 및 종료 처리

	// 유저 모두 내쫓고 제거해야 하는 방( bEnd && tCurrent >= nForceQuitTime )은 방 인스턴스를 제거하고
	// 인스턴스 번호 관련 처리 등이 필요하며, csBattle을 걸지 않은 상태로 처리해야 할 내용도 있기 때문에
	// (사실 배열에 할당된 인스턴스 메모리를 해제하지 않고 재활용하는 형태라 없지만 -_ -;)
	// 여기서 처리하지 않고 BattleArenaManager::onProcess에서 처리함

	// 연습 경기와 선택 경기의 방 파괴 처리 조건이 다른 부분이 있어서 따로 함
	// 연습 경기
	if( m_pArenaBase->IsExerciseGameArena() )
	{
		// 연습 경기는 방 인스턴스 생성 후 첫 번째 유저가 진입해서 ReadyState를 세팅하기 전까지는 방을 파괴하지 않음
		if( _getFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE ).empty() == false )
		{
			// 경기 시작 전에는 첫 번째 팀 유저가 아무도 없다면 경기 파괴
			// 경기 시작 후에는 아무 팀이든 한 명도 없으면 무조건 경기 파괴
			for( int nTeamNo = 0; nTeamNo < m_pArenaBase->nTeamCount; ++nTeamNo )
			{
				const std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
				if( pPlayerInfoList == NULL )
					continue;

				if( !pPlayerInfoList->empty() )
					continue;

				if( !m_bStart && nTeamNo )
					continue;

				// 경기는 끝났어도 방이 바로 파괴되면 안되고 GameRule::BATTLE_ARENA_FORCE_QUIT_DELAY 시간만큼 기다려준 후에 유저들 다 내쫓고 방 폭파해야 함
				return AET_ONE_TEAM_NO_MEMBER;
			}
		}
	}
	// 선택 경기
	else
	{
		// 한 쪽 팀의 인원이 모두 나간 방은 경기 종료
		for( int nTeamNo = 0; nTeamNo < m_pArenaBase->nTeamCount; ++nTeamNo )
		{
			const std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
			if( pPlayerInfoList == NULL )
				continue;

			if( !pPlayerInfoList->empty() )
				continue;

			// 경기는 끝났어도 방이 바로 파괴되면 안되고 GameRule::BATTLE_ARENA_FORCE_QUIT_DELAY 시간만큼 기다려준 후에 유저들 다 내쫓고 방 폭파해야 함
			return AET_ONE_TEAM_NO_MEMBER;
		}
	}

	// 시작한 후에 경기 시간 만료된 방 종료 처리
	if( m_bStart && !m_bEnd && tCurrent >= m_nEndTime )
	{
		// 경기는 끝났어도 방이 바로 파괴되면 안되고 GameRule::BATTLE_ARENA_FORCE_QUIT_DELAY 시간만큼 기다려준 후에 유저들 다 내쫓고 방 폭파해야 함
		return AET_TIME_OVER;
	}
	// 이외의 조건으로 경기가 종료되어야 하는 경우(라고 해봐야 한 팀의 누적 획득 AP가 pArenaBase->nObjectivePoint 이상이 되는 경우밖에 없음)에는
	// 해당 이벤트가 발생하는 시점에 즉시 체크해서 경기 종료 처리가 발생됨

	// } 현재 진행 중인 방의 종료 조건 체크 및 종료 처리
	return AET_UNKNOWN;
}

void	BattleArenaInstance::RemoveAbsentAndOfflineLimitExpiredPlayer( AR_TIME tCurrent )
{
	// 오프라인 상태로 변경된 후 제한 시간 이내에 재접속하지 않은 유저 이탈 처리
	// 입장 예정 시간이 다 된 유저를 강제로 입장 예약(락 문제 때문에 여기서 바로 못 함)
	for( int nTeamNo = 0; nTeamNo < m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		// vPlayerInfo 순회하면서 즉시 _removePlayer를 호출하면 vPlayerInfo.erase 가 호출되면서 it가 무효화되기 때문에
		// 따로 수집했다가 vPlayerInfo 순회 다 하고 나서 일괄 삭제
		std::vector< PlayerUID > vOfflineLimitExpired;
		std::vector< StructPlayer* > vAbsentPlayer;

		std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin(); it != pPlayerInfoList->end(); ++it )
			{
				PlayerInfo & pi = (*it);
				StructPlayer* pPlayer = pi.pPlayer;

				// 오프라인 유저면 이탈 제한 시간 체크
				if( pPlayer == NULL )
				{
					// 이탈 시점이 체크되어 있어야 함. 체크 안 된 상태로 오프라인이 되었다면 어디선가 pi.pPlayer = NULL 을 잘못하고 있다는 이야기.
					// 잘못된 상태라도 일단 그냥 방치하면 즉시 방에서 튕기는 정도의 버그만 나타나니 일단 별다른 조치 없이 진행
					assert( pi.nOfflineLimitTime );

					if( pi.nOfflineLimitTime <= tCurrent )
					{
						vOfflineLimitExpired.push_back( pi.nPlayerUID );
					}
				}
				// 온라인 유저면 처리해야 할 내용 처리
				// * 유저가 오프라인 상태라면 입장 관련해서는 아무것도 안 함(이 상태를 유지시켜서 있다가라도 재접하면 바로 안으로 날아 들어가게 해 줌)
				else
				{
					// 잠수 신고 응답 제한 시간 처리
					if( pi.bWaitingAbsenceCheck && pi.nLastAbsenceCheckTime <= tCurrent )
					{
						vAbsentPlayer.push_back( pPlayer );

						// 쫓겨났으면 다른 처리는 안 함
						continue;
					}

					// 입장 대기 상태의 유저가 입장 예정 시각이 지났다면 입장 처리
					if( pi.nPendedEnterTime && pi.nPendedEnterTime <= tCurrent )
					{
						m_vPendedEnterPlayerList.push_back( pPlayer );

						// 입장 예약 처리 및 중복해서 입장 처리가 발생하지 않도록 입장 예정 시점 초기화
						pi.nPendedEnterTime = 0;
					}
				}
			}
		}

		for( std::vector< PlayerUID >::const_iterator it = vOfflineLimitExpired.begin(); it != vOfflineLimitExpired.end(); ++it )
		{
			// _removePlayer로 인해 vPlayerInfo의 iterator가 무효화되는 문제를 피하기 위해 매 번 PlayerInfo를 검색...;;
			PlayerUID nPlayerUID = (*it);
			PlayerInfo* pPlayerInfo = _getPlayerInfo( nPlayerUID, nTeamNo );

			// 페널티 부여(연습 경기라면 페널티 없음)
			if( !m_pArenaBase->IsExerciseGameArena() )
			{
				// 유저가 오프라인이니 DB()에 직접 오프라인 유저 페널티 처리(DB_Login과 순서 꼬일 수도 있는데... 답이 없음 ` `;;)
				// 구조적으로 오프라인 유저에 대한 데이터 변경 DB 처리가 있을 때 로그인과 순서가 엇갈리지 않게 하려면
				// 별도의 매니저가 오프라인 유저에 대한 DB 처리들을 PlayerUID와 같은 고유 기준값을 이용해 리스트를 관리하고,
				// 로그인 시도 시에 동일한 PlayerUID에 해당하는 DB 작업들이 아직 남아있을 경우 DB_Login을 나중에 처리되도록
				// 대기시켜야 함... 아레나 페널티 따위 때문에 구현하긴 좀...;;
				++pPlayerInfo->nPenaltyCount;
				time_t tCurrent = time( NULL );
				DB().Push( new DB_UpdateBattleArenaPenalty( pPlayerInfo->nPlayerUID,
					tCurrent + GameRule::GetBattleArenaBlockDuration( pPlayerInfo->nPenaltyCount ),
					pPlayerInfo->nPenaltyCount,
					tCurrent + GameRule::GetBattleArenaPenaltyDuration( pPlayerInfo->nPenaltyCount ) ) );

				// 오프라인 상태에서 페널티를 받았을 때 경기가 진행되는 상태라면 재접속 시 페널티 부여 안내 메시지 출력을 위해 PlayerUID를 리스트에 추가
				BattleArenaManager::Instance().AddOfflinePenaltyReceivedPlayer( nPlayerUID );
				// 경기 종료 시 BattleArenaManager::RemoveOfflinePenaltyReceivedPlayer를 호출하기 위해 목록 따로 보관
				m_vOfflinePenaltyReceivedPlayerUIDList.push_back( nPlayerUID );
			}

			// 한 팀이 완전히 비워져서 경기가 취소되어야 하는 경우는 다음 _onProcess가 실행되는 시점(약 1초 후)에 처리되도록 함
			// * 너무 방 파괴되어야 하는 코드가 여기저기 붙는 것 같아서 -_ -;;
			_removePlayer( nPlayerUID, ALT_DISCONNECT );
		}

		for( std::vector< StructPlayer* >::iterator it = vAbsentPlayer.begin(); it != vAbsentPlayer.end(); ++it )
		{
			StructPlayer* pPlayer = (*it);

			// 잠수 신고 응답 제한 시간 초과이니 페널티 먹이고 경기에서 추방
			_removePlayer( pPlayer, ALT_ABSENT );

			// 페널티 부여(연습 경기라면 페널티 없음)
			if( !m_pArenaBase->IsExerciseGameArena() )
			{
				pPlayer->IncBattleArenaPenalty();
				pPlayer->DBQuery( new DB_UpdateBattleArenaPenalty( pPlayer ) );

				BattleArenaManager::SendBattleArenaPenaltyMessage( pPlayer );
			}

			BattleArenaQuitFunctor qf( ALT_ABSENT, m_pArenaBase, m_nInstanceNo );
			qf( pPlayer );
		}
	}
}

void	BattleArenaInstance::TryStart( AR_TIME tCurrent )
{
	// 아직 준비 중인 연습 경기에서는 아무것도 안 함
	if( m_nStartTime == INFINITE_TIME )
	{
		assert( m_pArenaBase->IsExerciseGameArena() );
		return;
	}

	// 시작 시간이 아직 되지 않은 경우 시작 전에 미리 입장하려던 유저 입장 처리 외에는 아무것도 안 함
	if( m_nStartTime > tCurrent )
		return;

	// 시작 시간이 되었으니 경기 시작 상태로 변경
	m_bStart = true;
	// 프랍 상태값 초기화(0으로 길이가 있는 스트링으로 변경해야 _activateProp 등등이 제대로 동작함 - 일종의 안전장치 'ㅠ '?)
	_setFlag( FLAG_BATTLE_ARENA_PROP_STATE, "0" );

	// 경기 중에 사용될 모든 몬스터, 프랍 리젠
	_initialRespawn();

	// 경기 시작 후 경기장 내부에서 카운트다운 완료 시점에 제거해야 하는 프랍(아레나 내부 진입 금지용 프랍) 제거
	_clearArenaBlockerFieldProps();

	LOG::Log11N4S( LM_BATTLE_ARENA_START, 0, 0,
		m_pArenaBase->nID, m_nInstanceNo, m_eGrade,
		_getTeamMemberCount(0), _getTeamMemberCount(1),
		0, 0, 0, 0, "", LOG::STR_NTS, "", LOG::STR_NTS,
		m_pArenaBase->IsExerciseGameArena() ? "EXERCISE" : "ARENA", LOG::STR_NTS,
		"", LOG::STR_NTS );

	// 시작 스크립트 실행
	BattleArenaRunEventScript( m_pArenaBase, BASET_START_INSTANCE, m_nInstanceNo, NULL );
}

int		BattleArenaInstance::_getTeamNo( int nPartyID ) const
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	if( nPartyID != 0 )
	{
		for( int nTeamNo = 0; nTeamNo < m_pArenaBase->nTeamCount; ++nTeamNo )
		{
			for( int i = 0; i < GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM; ++i )
			{
				if( m_anPartyID[ nTeamNo ][ i ] == nPartyID )
				{
					return nTeamNo;
				}
			}
		}
	}

	return INVALID_BATTLE_ARENA_TEAM_NO;
}

int		BattleArenaInstance::_getTeamMemberCount( int nTeamNo )	const
{
	const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
	return static_cast< int >( pPlayerInfoList->size() );
}

const std::vector< BattleArenaInstance::PlayerInfo >*	BattleArenaInstance::_getPlayerInfoList( int nTeamNo )	const
{
	assert( m_csBattle.IsLockedByCurrentThread() );
	assert( 0 <= nTeamNo && nTeamNo < GameRule::BATTLE_ARENA_MAX_TEAM_COUNT );

	const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = NULL;
	if( 0 <= nTeamNo && nTeamNo < GameRule::BATTLE_ARENA_MAX_TEAM_COUNT )
	{
		pPlayerInfoList = &m_vPlayerInfo[nTeamNo];
	}

	return pPlayerInfoList;
}

std::vector< BattleArenaInstance::PlayerInfo >*	BattleArenaInstance::_getPlayerInfoList( int nTeamNo )
{
	assert( m_csBattle.IsLockedByCurrentThread() );
	assert( 0 <= nTeamNo && nTeamNo < GameRule::BATTLE_ARENA_MAX_TEAM_COUNT );

	std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = NULL;
	if( 0 <= nTeamNo && nTeamNo < GameRule::BATTLE_ARENA_MAX_TEAM_COUNT )
	{
		pPlayerInfoList = &m_vPlayerInfo[nTeamNo];
	}

	return pPlayerInfoList;
}

bool	BattleArenaInstance::_isQuickJoinAble()	const
{
	// 근데 빠른 입장으로 입장할 수 없는 방이라면 즉시 0으로 리턴(경기 진행 상태가 아닌 방, 빠른 입장 제한 시간 초과)
	AR_TIME tCurrent = GetArTime();
	if( !m_bStart || m_bEnd || tCurrent >= m_nQuickJoinLimitTime )
		return false;

	// 각 팀의 점수 중 빠른 입장 제한 점수 이상인 팀이 있는 경우 체크
	for( int nTeamNo = 0; nTeamNo < m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		if( m_aTeamInfo[ nTeamNo ].nScore >= m_pArenaBase->nQuickJoinLimitPoint )
			return false;
	}

	return true;
}

unsigned int	BattleArenaInstance::_getQuickJoinPriority() const
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 빠른 입장을 위한 인스턴스별 가중치를 계산
	// 진행 중인 경기들에 한해서 우선 순위가 높은 조건을 윗 자리 수 값으로 계산해서
	// 대/소 비교를 통해 빠른 입장의 우선 순위가 높은 방이 큰 값을 반환하도록 함

	// 근데 빠른 입장으로 입장할 수 없는 방이라면 즉시 0으로 리턴(경기 진행 상태가 아닌 방, 빠른 입장 제한 시간 초과)
	if( _isQuickJoinAble() == false )
	{
		return 0;
	}

	AR_TIME tCurrent = GetArTime();
	unsigned int nPriority = 0;

	// * 이하는 복잡해서 일단 두 팀만 있다는 전제 하에 작성한 코드임
	assert( m_pArenaBase->nTeamCount == 2 );

	// 양 팀의 인원 수가 맞지 않는 경우
	int anMemberCount[] = { _getTeamMemberCount(0), _getTeamMemberCount(1) };
	if( anMemberCount[ 0 ] != anMemberCount[ 1 ] )
	{
		// 우선 순위 조건 1. 양 진영의 총 인원 수가 적은 방(31 ~ 26 비트)
		assert( GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT < ( 1 << 6 ) );		// 양 진영 인원 수 적음의 수치화 결과가 최대 64보다 작아야 함(6비트로 표현)
		nPriority += ( GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT - anMemberCount[ 0 ] - anMemberCount[ 1 ] ) << 26;

		// 우선 순위 조건 2. 양 진영의 인원 수 차이가 높은 방(25 ~ 21 비트) 
		assert( GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM < ( 1 << 5 ) );
		nPriority += ( anMemberCount[ 0 ] > anMemberCount[ 1 ] ) ? ( anMemberCount[ 0 ] - anMemberCount[ 1 ] ) : ( anMemberCount[ 1 ] - anMemberCount[ 0 ] ) << 21;
	}

	// 우선 순위 조건 3. 입장 가능 시간이 적게 남은 방(20 ~ 15 비트: 총 시간 대비 지난 시간 비율을 2로 나눈 값(0~50)으로 저장)
	nPriority += ( ( tCurrent - m_nStartTime ) * 100 / m_pArenaBase->nQuickJoinLimitTime / 2 ) << 15;

	// 우선 순위 조건 4. (공통: 모든 조건이 같은 방) 슬래터 > 빙고 > 클래식(14 ~ 13 비트)
	assert( m_pArenaBase->eType >= 0 && m_pArenaBase->eType <= 3 );
	nPriority += ( m_pArenaBase->eType ) << 13;

	// 우선 순위 조건 5. (공통: 모든 조건이 같은 방) 16:16 > 8:8 > 4:4 > 2:2(12 ~ 10 비트)
	int nWholePlayerSizePriority = 0;
	if( m_pArenaBase->nMaxMember >= 32 )			nWholePlayerSizePriority = 3;
	else if( m_pArenaBase->nMaxMember >= 16 )		nWholePlayerSizePriority = 2;
	else if( m_pArenaBase->nMaxMember >= 8 )		nWholePlayerSizePriority = 1;

	nPriority += nWholePlayerSizePriority << 10;

	return nPriority;
}

unsigned short	BattleArenaInstance::_addPlayer( StructPlayer* pPlayer, int nTeamNo )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 대기열 참여를 통해 현재 경기에 참가(최근에 생성된 시작 전의 경기 참가 또는 새 경기 생성 후 참가)하는 경우 혹은 연습 경기 참여(Case 1)와
	// 빠른 입장을 통해 플레이 중이던 현재 경기에 입장하는 경우(Case 2)와
	// 연습 경기에 초대를 받아 참여한 경우(Case 3)
	assert( !pPlayer->GetBattleArenaInstanceNo() &&
		( ( pPlayer->GetBattleArenaID() == m_pArenaBase->nID && !m_bStart ) ||	// Case 1
		( !pPlayer->GetBattleArenaID() && m_bStart ) ) );						// Case 2

	// _addPlayer Caller 측에서 먼저 pPlayer가 참여 가능한 상태임을 체크해서 보장해야 하지만 만에 하나의 경우를 위해...
	int nPrevPartyID = pPlayer->GetPartyID();
	assert( !nPrevPartyID || PartyManager::GetInstance().GetPartyType( nPrevPartyID ) == PartyManager::TYPE_NORMAL_PARTY );

	const size_t nMaxMemberPerTeam = static_cast< size_t >( m_pArenaBase->nMaxMember / m_pArenaBase->nTeamCount );

	// nTeamNo 파라미터가 INVALID_BATTLE_ARENA_TEAM_NO인 경우에는 멤버 수가 가장 적은 팀 검색(만일 동일 인원 상태였던 둘 이상의 팀이 있다면 50% 랜덤)
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		nTeamNo = GetLeastMemberCountTeam();
	}

	// 빈 자리가 없었다면 참여 불가
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO ||
		_getTeamMemberCount( nTeamNo ) >= nMaxMemberPerTeam )
		return RESULT_LIMIT_MAX;


	// 현재 파티에 가입중이라면 파티 탈퇴
	if( nPrevPartyID )
	{
		LeaveParty( nPrevPartyID, pPlayer );

		// 파티 해산 또는 탈퇴 처리가 제대로 안됐다면 그냥 참여 자체를 실패로 처리
		if( pPlayer->GetPartyID() )
			return RESULT_ALREADY_EXIST;
	}

	unsigned short shResult = FixSameAlias( pPlayer );
	if( shResult != RESULT_SUCCESS )
	{
		return shResult;
	}

	std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
	if( pPlayerInfoList == NULL )
	{
		return RESULT_LIMIT_MAX;
	}

	// 들어갈 팀에 참가(자리가 빈 파티가 있다면 빈 파티로, 없다면 새 파티 생성)
	for( int nSlotInTeam = 0; nSlotInTeam < GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM; ++nSlotInTeam )
	{
		int & nPartyID = m_anPartyID[ nTeamNo ][ nSlotInTeam ];
		if( nPartyID <= 0 )
		{
			nPartyID = CreateBattleArenaParty( nTeamNo, pPlayer );
			if( nPartyID <= 0 )
			{
				return RESULT_LIMIT_MAX;
			}

			int nLeadParty = 0;
			if( nSlotInTeam != 0 )
			{
				nLeadParty = m_anPartyID[ nTeamNo ][ 0 ];
			}

			if( LinkBattleArenaParty( nLeadParty, nPartyID ) == false )
			{
				// 아놔...;; 뭐여 이건;; 일단 파티 해산;
				PartyManager::GetInstance().DestroyParty( nPartyID );
				nPartyID = 0;

				return RESULT_UNKNOWN;
			}

			std::string strPartyName = PartyManager::GetInstance().GetPartyName( nPartyID );
			PartyManager::_PARTY_TYPE ePartyType = PartyManager::GetInstance().GetPartyType( nPartyID );

			// 클라에 방송하기로는 파티 타입을 무조건 연습 경기용이 아닌 실제 경기용 파티로 방송
			PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "CREATE|%s|%s|%d|",
				strPartyName.c_str(), pPlayer->GetAlias(), PartyManager::TYPE_BATTLE_ARENA_TEAM );

			LOG::Log11N4S( LM_PARTY_CREATE, pPlayer->GetAccountID(), pPlayer->GetSID(),
				nPartyID, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
				5,
				ePartyType,
				0, 0, 0,
				pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetAlias(), LOG::STR_NTS,
				strPartyName.c_str(), LOG::STR_NTS,
				"", LOG::STR_NTS );

			// 새로 생성된 파티 ID와 현재 아레나 ID를 연결
			m_pPartyToArenaMap->Set( nPartyID, m_pArenaBase->nID );
			// 새로 생성된 파티 ID와 현재 인스턴스 번호를 연결
			m_pPartyToInstanceMap->Set( nPartyID, m_nInstanceNo );
		}
		else
		{
			// 파티가 해산되어버렸거나 최대 인원(팀당 아니고 파티 시스템의 최대 인원 = 8명)이 다 차 있으면 Join이 실패함
			if( !PartyManager::GetInstance().JoinParty( nPartyID, pPlayer ) )
				continue;

			PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "JOIN|%s|%s|",
				PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), pPlayer->GetAlias() );
		}

		SendPartyInfo( pPlayer );
		SendLinkedPartyInfo( pPlayer );
		BroadcastPartyMemberInfo( nPartyID, pPlayer );
		BroadcastLinkedPartyMemberInfo( nPartyID, pPlayer );
		SendSummonInfo( pPlayer );
		BroadcastSummonInfo( nPartyID, pPlayer );

		LOG::Log11N4S( LM_PARTY_JOIN,
			pPlayer->GetAccountID(), pPlayer->GetSID(), nPartyID,
			pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
			1,
			0, 0, 0, 0,
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
			PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS,
			"", LOG::STR_NTS );

		break;
	}

	pPlayer->SetBattleArenaInstanceNo( m_nInstanceNo );
	pPlayerInfoList->push_back( PlayerInfo( pPlayer ) );

	// 경기 상태에 따라 입장 예정 시점을 세팅
	AR_TIME tCurrent = GetArTime();
	if( !m_bStart && tCurrent < m_nStartTime )
	{
		// 시작 전의 경기라면 경기 시작 시점에 자동 입장
		pPlayerInfoList->back().nPendedEnterTime = m_nStartTime;
	}
	else
	{
		// 시작된 후의 경기라면 현재 시점보다 지정 기간 이후에 자동 입장(현재는 10초 고정)
		pPlayerInfoList->back().nPendedEnterTime = tCurrent + GameRule::BATTLE_ARENA_QUICK_JOIN_COUNTDOWN;
	}

	LOG::Log11N4S( LM_BATTLE_ARENA_JOIN_BATTLE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(),
		m_pArenaBase->nID, m_nInstanceNo, m_eGrade,
		nTeamNo, _getTeamMemberCount(nTeamNo), pPlayer->GetPartyID(),
		0, 0, 0,
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
		"", LOG::STR_NTS,
		pPlayer->GetAlias(), LOG::STR_NTS );

	return RESULT_SUCCESS;
}

int		BattleArenaInstance::GetLeastMemberCountTeam()	const
{
	int nTeamNo = INVALID_BATTLE_ARENA_TEAM_NO;
	size_t nLeastMemberCount = UINT_MAX;

	for( int t = 0; t < m_pArenaBase->nTeamCount; ++t )
	{
		size_t nMemberCount = _getTeamMemberCount( t );
		if( nMemberCount < nLeastMemberCount )
		{
			nLeastMemberCount = nMemberCount;
			nTeamNo = t;
		}
		// 동일 인원인 팀이 확인되면 50% 확률로 팀을 변경
		else if( nMemberCount == nLeastMemberCount )
		{
			if( XRandom( 1, 100 ) <= 50 )
			{
				nTeamNo = t;
			}
		}
	}

	return nTeamNo;
}

unsigned short	BattleArenaInstance::FixSameAlias( StructPlayer* pPlayer )	const
{
	// 가명 사용 유저의 경우에 중복되는 가명을 가진 다른 유저가 이미 경기에 참여되어 있다면, 가명 뒤에 숫자 붙여 줌
	if( pPlayer->IsUsingAlias() && strlen( pPlayer->GetAlias( true ) ) > 0 )
	{
		std::string strAlias = pPlayer->GetAlias();
		// 중복 검사 시에는 뒤에 붙은 *을 떼 줘야 함
		strAlias.resize( strAlias.size() - 1 );

		// 이번에 참여하는 유저의 가명을 포함하고, 그 뒤에 숫자가 붙어있는 유저의 가명 목록 추출
		bool bSameAliasExist = false;
		std::vector< int > vUsedDuplicatedAliasIndices;

		for( int t = 0; t < m_pArenaBase->nTeamCount; ++t )
		{
			const std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( t );
			if( pPlayerInfoList != NULL )
			{
				for( std::vector< PlayerInfo >::const_iterator it = pPlayerInfoList->begin(); it != pPlayerInfoList->end(); ++it )
				{
					const PlayerInfo & pi = (*it);

					std::string strOtherAlias;
					if( pi.pPlayer )
					{
						if( !pi.pPlayer->IsUsingAlias() || !strlen( pi.pPlayer->GetAlias( true ) ) )
							continue;

						strOtherAlias = pi.pPlayer->GetAlias();
					}
					else
					{
						strOtherAlias = PartyManager::GetInstance().GetMemberDisplayName( pi.nPartyID, pi.nPlayerUID );

						// strOtherAlias.empty() == true 일 가능성은 없어 보이지만...
						if( strOtherAlias.empty() || strOtherAlias.at( strOtherAlias.size() - 1 ) != '*' )
							continue;
					}

					// 확인된 가명의 뒤에는 *이 반드시 붙어 있어야 하므로 *을 떼 줌
					if( strOtherAlias.at( strOtherAlias.size() - 1 ) != '*' )
					{
						// 가명 사용 중인 유저의 가명만 받아서 여기로 왔는데 뒤에 *이 없다고;?
						assert( 0 );
						continue;
					}

					strOtherAlias.resize( strOtherAlias.size() - 1 );

					if( _strnicmp( strOtherAlias.c_str(), strAlias.c_str(), strAlias.size() ) )
						continue;

					// 동일한 부분을 제외한 부분이 겹치는 Alias에 의해 생성될 수 있는 숫자값인지 체크
					strOtherAlias = strOtherAlias.substr( strAlias.size() );

					// 완전히 동일한 가명인 경우
					if( strOtherAlias.empty() )
					{
						bSameAliasExist = true;
					}
					// 2 byte 이하의 추가 내용이 있는 경우 숫자인지 체크
					else if( strOtherAlias.size() <= 2 )
					{
						if( !isdigit( strOtherAlias.at( 0 ) ) || strOtherAlias.at( 0 ) == '0' || ( strOtherAlias.size() == 2 && !isdigit( strOtherAlias.at( 1 ) ) ) )
							continue;

						int nUsedIndex = atoi( strOtherAlias.c_str() );
						if( !nUsedIndex || nUsedIndex >= 100 || std::find( vUsedDuplicatedAliasIndices.begin(), vUsedDuplicatedAliasIndices.end(), nUsedIndex ) != vUsedDuplicatedAliasIndices.end() )
						{
							// 숫자라고 검사 다 해서 변환하고 보니 정상적인 범위의 값이 아니거나 이미 확인됐던 값이라면
							// 로직 상이나 어떤 부분에 버그가 있을 가능성이 높으므로 일단 패스
							assert( 0 );
							return RESULT_INVALID_TEXT;
						}

						vUsedDuplicatedAliasIndices.push_back( nUsedIndex );
					}
					// else 에 해당하는 2 byte 넘게 추가 내용이 있는 경우는 중복 별명 아님
				}
			}
		}

		if( bSameAliasExist )
		{
			// 추출된 중복 가명 목록 인덱스에 해당하지 않는 가장 작은 인덱스 번호 찾아서 유저 가명 뒤에 붙여 줌
			// * vUsedDuplicatedAliasIndices를 정렬해두고 검사하면 이렇게 for문을 다 돌 필요도 없이 로직을 간소화시킬 수 있지만
			//   vUsedDuplicatedAliasIndices의 최대 길이가 32인 점을 감안하면 정렬해가며 처리하는 것 자체가 사치에 가깝다고 보여서
			//   그냥 무식하게 for문 돌림(어차피 루프 몇 번 안 돌고 빠져 나와야 정상)
			for( int i = 1; i < 100; ++i )
			{
				if( std::find( vUsedDuplicatedAliasIndices.begin(), vUsedDuplicatedAliasIndices.end(), i ) != vUsedDuplicatedAliasIndices.end() )
					continue;

				char szIndex[ 3 ];
				s_sprintf( szIndex, _countof( szIndex ), "%d", i );
				strAlias += szIndex;

				// 최대 길이가 넘친다는 건 뭔가 잘못됐다는 건데 -_ -;?
				if( strAlias.size() > 17 )
				{
					assert( 0 );
					return RESULT_INVALID_TEXT;
				}

				// 여기서 strAlias에 *은 안 붙임(아래의 pPlayer->SetAlias()에서 알아서 붙음)
				break;
			}

			// 새로 세팅된 가명으로 유저의 가명을 변경
			pPlayer->DBQuery( new DB_ChangeCharacterAlias( pPlayer, strAlias.c_str(), false ) );
			pPlayer->SetAlias( strAlias.c_str() );

			// 클라에게 가명이 강제로 변경되었음을 TS_SC_RESULT, request_msg_id = TM_CS_CHANGE_ALIAS로 방송함
			// * 우측 하단 알림 팝업 창에 메시지를 띄워야 하는데 서버에서는 그 쪽에 메시지를 임의로 띄울 수 없어서...
			SendResult( pPlayer, TM_CS_CHANGE_ALIAS, RESULT_ALREADY_EXIST );
		}
	}

	return RESULT_SUCCESS;
}

int		BattleArenaInstance::CreateBattleArenaParty( int nTeamNo, StructPlayer* pPlayer )
{
	int nPartyID = 0;
	char szPartyName[ 64 ] = { 0, };

	// 파티 타입 결정
	PartyManager::_PARTY_TYPE ePartyType = ( m_pArenaBase->IsExerciseGameArena() && !m_bStart ) ?
										PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM : PartyManager::TYPE_BATTLE_ARENA_TEAM;

	// 생성할 파티 이름을 "연합군<~~~>", "연합군_00<~~~>", "연합군_01<~~~>" 이런 식으로 번호 증가시키며 시도
	// * 기존에 결성되었던 파티들 중 누군가가 해산되면 뒤의 파티들이 앞으로 당겨지기 때문에 nSlotInTeam만으로는 파티 이름을 확정적으로 결정할 수 없음
	const int nMaxPartyCountPerTeam = ( ( m_pArenaBase->nMaxMember / m_pArenaBase->nTeamCount ) - 1 ) / PartyManager::MAX_PARTY_MEMBER + 1;
	for( int nPartyNameIndex = 0; nPartyNameIndex < nMaxPartyCountPerTeam; ++nPartyNameIndex )
	{
		if( nPartyNameIndex == 0 )
		{
			s_sprintf( szPartyName, _countof( szPartyName ), "%s<%d%db>",
				GameContent::GetString( GameRule::GetBattleArenaTeamNameStringID( nTeamNo, false ) ),
				m_pArenaBase->nID,
				m_nInstanceNo );
		}
		else
		{
			s_sprintf( szPartyName, _countof( szPartyName ), "%s_%02d<%d%db>",
				GameContent::GetString( GameRule::GetBattleArenaTeamNameStringID( nTeamNo, false ) ),
				nPartyNameIndex - 1,
				m_pArenaBase->nID,
				m_nInstanceNo );
		}

		// 파티 생성 성공하면 루프 중단
		nPartyID = PartyManager::GetInstance().MakeParty( szPartyName, pPlayer->GetPlayerUID(), ePartyType );
		if( nPartyID > 0 )
		{
			break;
		}
	}

	if( nPartyID > 0 )
	{
		// 새로 생성된 파티에 가입시킴
		if( !PartyManager::GetInstance().JoinParty( nPartyID, pPlayer ) )
		{
			// 파장이 가입 실패했으면 뭐... 없애야지 -_ -;;
			PartyManager::GetInstance().DestroyParty( nPartyID );
			nPartyID = 0;
		}
	}

	return nPartyID;
}

bool	BattleArenaInstance::LinkBattleArenaParty( int nLeaderPartyID, int nJoinPartyID )
{
	// 배틀 아레나 링크 파티에 가입 또는 생성
	if( nLeaderPartyID == 0 )
	{
		const int nMaxPartyCountPerTeam = ( ( m_pArenaBase->nMaxMember / m_pArenaBase->nTeamCount ) - 1 ) / PartyManager::MAX_PARTY_MEMBER + 1;
		if( !PartyManager::GetInstance().MakeBattleArenaTeam( nJoinPartyID, nMaxPartyCountPerTeam ) )
		{
			return false;
		}
	}
	else
	{
		if( !PartyManager::GetInstance().JoinBattleArenaTeam( nLeaderPartyID, nJoinPartyID ) )
		{
			return false;
		}
	}

	return true;
}

unsigned short	BattleArenaInstance::_removePlayer( StructPlayer * pPlayer, _ARENA_LEAVE_TYPE eLeaveType )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 현재 경기에 참여해 있는 유저만 탈퇴 가능
	assert( pPlayer->GetBattleArenaID() == m_pArenaBase->nID && pPlayer->GetBattleArenaInstanceNo() == m_nInstanceNo );

	PlayerUID nPlayerUID = pPlayer->GetPlayerUID();
	return _removePlayer( nPlayerUID, eLeaveType );
}

unsigned short BattleArenaInstance::_removePlayer( PlayerUID nPlayerUID, _ARENA_LEAVE_TYPE eLeaveType )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// _removePlayer( StructPlayer * pPlayer, _ARENA_LEAVE_TYPE eLeaveType ) 함수는 참여 정보 삭제와 파티 이탈 처리가 따로 가능하지만
	// 이 함수에서는 유저 참여 정보가 없으면 어느 팀/파티에 소속되어 있던 오프라인 유저인지를 알 수 없기 때문에(그렇다고 모든 파티를 다 돌긴 좀 -_ -;)
	// 유저 참여 정보가 반드시 있어야만 정상적으로 이탈 처리가 됨

	// 팀 번호 확인 및 유저 참여 정보 제거, 파티 관련 이탈 처리
	for( int nTeamNo = 0; nTeamNo < m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin(); it != pPlayerInfoList->end(); ++it )
			{
				if( (*it).nPlayerUID != nPlayerUID )
					continue;

				// 삭제전 정보 저장
				PlayerInfo removePlayerInfo = (*it);
				// 유저 참여 정보 제거
				pPlayerInfoList->erase( it );

				return LeavePlayer( &removePlayerInfo, nTeamNo, eLeaveType );
			}
		}
	}

	// 없는 유저에 대해서 _removePlayer를 시도했다는 건 어디선가 _removePlayer를 두 번 이상 호출하는 상태
	assert( 0 );

	return RESULT_NOT_EXIST;
}

void	BattleArenaInstance::_deleteBattleArenaPartyPlayer( int nPartyID, StructPlayer* pPlayer )
{
	int nTeamNo = _getTeamNo( nPartyID );
	if( nTeamNo != INVALID_BATTLE_ARENA_TEAM_NO )
	{
		LeaveBattleArenaParty( nTeamNo, nPartyID, pPlayer->GetPlayerUID(), pPlayer, pPlayer->GetAlias(), ALT_UNKNOWN );
	}
}

void	BattleArenaInstance::_removeAllPlayer( _ARENA_LEAVE_TYPE eLeaveType, ArObjectFunctor& functor )
{
	for( int nTeamNo = 0; nTeamNo < m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin(); it != pPlayerInfoList->end(); )
			{
				// 삭제전 정보 저장
				PlayerInfo outPlayerInfo = (*it);
				// 유저 참여 정보 제거
				it = pPlayerInfoList->erase( it );

				LeavePlayer( &outPlayerInfo, nTeamNo, eLeaveType );

				StructPlayer* pPlayer = outPlayerInfo.pPlayer;
				if( pPlayer )
				{
					functor( pPlayer );
				}
			}
		}

		assert( pPlayerInfoList->empty() );
		pPlayerInfoList->clear();
	}

#ifdef	_DEBUG
	for( int nTeamNo = 0; nTeamNo < m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		// 모든 파티 해산
		for( int i = 0; i < GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM; ++i )
		{
			int nPartyID = m_anPartyID[ nTeamNo ][ i ];

			if( !nPartyID )
				continue;

			assert( 0 );
		}
	}
#endif
}

unsigned short	BattleArenaInstance::LeavePlayer( PlayerInfo* pPlayerInfo, int nTeamNo, _ARENA_LEAVE_TYPE eLeaveType )
{
	// 정상적으로 파티에 소속된 상태에서 이탈되는 경우 처리
	int nPartyID = pPlayerInfo->nPartyID;
	PlayerUID nPlayerUID = pPlayerInfo->nPlayerUID;
	StructPlayer* pPlayer = pPlayerInfo->pPlayer;

	// 오프라인 유저는 이름을 파티 멤버 정보에서 얻어 와서 사용함
	std::string strNameAlias;
	if( pPlayer != NULL )
	{
		strNameAlias = pPlayer->GetAlias();
	}
	else
	{
		strNameAlias = PartyManager::GetInstance().GetMemberDisplayName( nPartyID, nPlayerUID );
	}

	unsigned short shResult = RESULT_UNKNOWN;
	if( nPartyID )
	{
		shResult = LeaveBattleArenaParty( nTeamNo, nPartyID, nPlayerUID, pPlayer, strNameAlias.c_str(), eLeaveType );
	}
	else
	{
		// 파티에 소속되지 않은 상태에서 여기 들어온 경우(비정상 루트로 파티에서만 이탈된 경우)는 조금 다르게 처리함

		// 현재 팀의 파티들에 대한 검증(실제로 없는 파티일 가능성도 있으므로)
		for( int i = 0; i < GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM; ++i )
		{
			int nCheckPartyID = m_anPartyID[ nTeamNo ][ i ];
			if( nCheckPartyID && !PartyManager::GetInstance().IsBattleArenaTeamParty( nCheckPartyID ) )
			{
				// 파티 ID와 아레나 ID의 연결 정보 삭제
				m_pPartyToArenaMap->Remove( nCheckPartyID );
				// 파티 ID와 인스턴스 번호의 연결 정보 삭제
				m_pPartyToInstanceMap->Remove( nCheckPartyID );

				for( int nPartyIndex = i + 1; nPartyIndex < GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM; ++nPartyIndex )
				{
					m_anPartyID[ nTeamNo ][ nPartyIndex - 1 ] = m_anPartyID[ nTeamNo ][ nPartyIndex ];
				}
				// 팀 내에 맨 마지막에 있던 슬롯은 반드시 0이 되어야 함(위의 for 루프에서 맨 마지막 팀의 슬롯을 0으로 만들지 못함)
				m_anPartyID[ nTeamNo ][ GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM - 1 ] = 0;
			}
		}

		// 현재 팀의 파티가 비어있어서 경기가 종료되어야 하는지 체크해서 리턴
		shResult = ( m_anPartyID[ nTeamNo ][ 0 ] ) ? RESULT_SUCCESS : RESULT_NOT_ENOUGH_BULLET;
	}

	// 나머지 유저들에게 이탈되었다는 것을 알려 줌
	BattleArenaManager::BroadcastBattleArenaLeaveBattleMessageWithLog( this,
		pPlayerInfo, pPlayer,
		m_nWinTeamNo, nTeamNo,
		strNameAlias.c_str(), eLeaveType );

	if( pPlayer != NULL )
	{
		// 자신에게 이탈되었다는 것을 알려 줌
		BattleArenaManager::SendBattleArenaLeaveMessage( pPlayer, eLeaveType );

		// 경기 이탈에 성공했으니 캐릭터에게서 가명 사용 여부 해제하고 BattleArena 정보 제거
		pPlayer->SetUseAlias( false );
		pPlayer->SetBattleArenaID( 0 );
		pPlayer->SetBattleArenaInstanceNo( 0 );
	}

	return shResult;
}

unsigned short	BattleArenaInstance::LeaveBattleArenaParty( int nTeamNo,
														   int nPartyID,
														   PlayerUID nPlayerUID,
														   StructPlayer* pPlayer,
														   const char* pAlias,
														   _ARENA_LEAVE_TYPE eLeaveType )
{
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		return RESULT_UNKNOWN;
	}

	int nAccountID = 0;
	AR_UNIT x = 0;
	AR_UNIT y = 0;
	unsigned char nLayer = 0;
	const char* szAccountName = "";
	const char* szPlayerName = pAlias;

	if( pPlayer != NULL )
	{
		nAccountID = pPlayer->GetAccountID();
		x = pPlayer->GetX();
		y = pPlayer->GetY();
		nLayer = pPlayer->GetLayer();
		szAccountName = pPlayer->GetAccountName();
		szPlayerName = pPlayer->GetName();
	}

	// 파티장이면서 멤버가 1명만 남아있거나 다른 파티멤버에게 자동 인계가 실패하면 파티 해산
	// 그 외의 모든 경우에는 그냥 파티 탈퇴
	if( PartyManager::GetInstance().IsLeader( nPartyID, nPlayerUID ) )
	{
		if( PartyManager::GetInstance().GetMemberCount( nPartyID ) == 1 ||
			!PartyManager::GetInstance().AutoPromote( nPartyID, false ) )
		{
			BroadcastPartyDestroy( nPartyID );

			LOG::Log11N4S( LM_PARTY_DESTROY,
				nAccountID, nPlayerUID,
				nPartyID, x, y, nLayer,
				1,
				0, 0, 0, 0,
				szAccountName, LOG::STR_NTS, szPlayerName, LOG::STR_NTS,
				PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS,
				"", LOG::STR_NTS );

			PartyManager::GetInstance().DestroyParty( nPartyID );

			// 파티 ID와 아레나 ID의 연결 정보 삭제
			m_pPartyToArenaMap->Remove( nPartyID );
			// 파티 ID와 인스턴스 번호의 연결 정보 삭제
			m_pPartyToInstanceMap->Remove( nPartyID );

			// 파티가 해산될 경우에는 파티 목록에서 해당 파티ID 제거하되, 현재 팀 내의 뒤쪽에 다른 파티ID를 앞으로 당겨 줌(anPartyID 가 팀 단위로는 중간에 0 인 값이 들어있지 않도록 하기 위함)
			for( int i = 0; i < GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM; ++i )
			{
				if( m_anPartyID[ nTeamNo ][ i ] != nPartyID )
					continue;

				m_anPartyID[ nTeamNo ][ i ] = 0;
				for( int nPartyIndex = i + 1; nPartyIndex < GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM; ++nPartyIndex )
				{
					m_anPartyID[ nTeamNo ][ nPartyIndex - 1 ] = m_anPartyID[ nTeamNo ][ nPartyIndex ];
				}
				break;
			}

			// 팀 내에 맨 마지막에 있던 슬롯은 반드시 0이 되어야 함(위의 for 루프에서 맨 마지막 팀의 슬롯을 0으로 만들지 못함)
			m_anPartyID[ nTeamNo ][ GameRule::BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM - 1 ] = 0;

			// 만일 현재 팀에 더 이상 남은 파티가 없다면 경기를 종료시켜야 하므로 별도의 리턴값으로 리턴
			if( !m_anPartyID[ nTeamNo ][ 0 ] )
			{
				return RESULT_NOT_ENOUGH_BULLET;
			}
		}
		else
		{
			PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "PROMOTE|%d|%s|",
				nPartyID, PartyManager::GetInstance().GetLeaderDisplayName( nPartyID ).c_str() );
		}
	}

	if( eLeaveType != ALT_EXERCISE_PARTY_KICK )
	{
		BroadcastPartyLeave( nPartyID, pAlias );

		LOG::Log11N4S( LM_PARTY_LEAVE,
			nAccountID, nPlayerUID, nPartyID,
			x, y, nLayer,
			1,
			0, 0, 0, 0,
			szAccountName, LOG::STR_NTS, szPlayerName, LOG::STR_NTS,
			PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS,
			"", LOG::STR_NTS );
	}
	else
	{
		PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "KICK|%s|%s|",
			PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), pAlias );
	}

	PartyManager::GetInstance().LeaveParty( nPartyID, nPlayerUID );

	return RESULT_SUCCESS;
}

unsigned short	BattleArenaInstance::_disconnectPlayer( StructPlayer * pPlayer )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 경기가 끝난 후에 접속이 끊기는 경우는 그냥 경기에서 이탈시킴
	if( m_bEnd )
	{
		// 필요한 경우에는 여기서 ALT_GAME_OVER 대신 ALT_DISCONNECT를 쓰는 것도 개념상 가능할 듯
		unsigned short nResult = _removePlayer( pPlayer, ALT_GAME_OVER );

		// 한 팀이 완전히 비워진 경우라도 이탈은 성공인 경우고, 한 팀이 완전히 비워지든 말든 상관없으므로 RESULT_SUCCESS로 바꿔서 결과를 리턴
		if( nResult == RESULT_NOT_ENOUGH_BULLET )
			nResult = RESULT_SUCCESS;

		return nResult;
	}

	PlayerUID nPlayerUID = pPlayer->GetPlayerUID();

	for( int nTeamNo = 0; nTeamNo < m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			for( std::vector< PlayerInfo >::iterator it =pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
			{
				PlayerInfo & pi = (*it);

				if( pi.nPlayerUID != nPlayerUID )
					continue;

				if( !pi.pPlayer )
				{
					assert( 0 );
					return RESULT_NOT_EXIST;
				}
				assert( pi.pPlayer == pPlayer );

				// 경기장 내부에 있었을 경우 이탈 스크립트 실행(추가로 접속 종료에 의한 것이 아니라는 파라미터를 세팅해 줌)
				if( pPlayer->IsInBattleArena() )
				{
					// 이탈 스크립트 실행(추가로 접속 종료에 의한 것이라는 파라미터를 세팅해 줌)
					std::vector< std::string > vTagReplacement;
					vTagReplacement.push_back( "#@disconnect@#" );
					vTagReplacement.push_back( "1" );

					ThreadPlayerHelper TPHelper( pPlayer );
					BattleArenaRunEventScript( m_pArenaBase, BASET_LEAVE_INSTANCE, m_nInstanceNo, pPlayer, &vTagReplacement );
				}

				pi.pPlayer = NULL;
				pi.nPenaltyCount = pPlayer->GetBattleArenaPenaltyCount();
				pi.nOfflineLimitTime = GetArTime() + GameRule::nBattleArenaReconnectWaitDuration;

				// 잠수 신고 응답 대기 상태였으면 해제
				if( pi.bWaitingAbsenceCheck )
				{
					pi.bWaitingAbsenceCheck = false;
					// 대신 다시 접속하면 바로 다시 잠수 신고 받을 수 있게 이전 잠수 신고 시점을 0 으로 초기화
					pi.nLastAbsenceCheckTime = 0;
				}

				// 연습 경기일 경우이고 시작 전이었다면 팀의 레디 상태 초기화
				if( m_pArenaBase->IsExerciseGameArena() && !m_bStart )
				{
					std::string strReady = _getFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE );
					int nReadyState = atoi( strReady.c_str() );

					nReadyState &= ~( 1 << nTeamNo );
					XStringUtil::Format( strReady, "%d", nReadyState );

					_setFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE, strReady.c_str() );

					BattleArenaManager::BroadcastBattleArenaExerciseReadyState( this );
				}

				// 다른 유저들에게 현재 유저가 접속이 끊겼음을 알려 줌
				BattleArenaManager::BroadcastBattleArenaDisconnectBattleMessage( this, pPlayer );

				// 일단 실제로 이탈은 아니고 접속 종료지만 처리가 제대로 됐다는 표시라도 해주기 위해
				// (그리고 향후 불필요한 추가 처리 발생하지 않게 하기 위해) 각 ID 값 초기화
				pPlayer->SetBattleArenaID( 0 );
				pPlayer->SetBattleArenaInstanceNo( 0 );

				// 죽은 상태에서 접속 종료 처리를 해버리면 재접속 시 경기장 밖으로 쫓겨나게 되는 경우에 죽은 상태로 밖에서 로그인되기 때문에 여기서 부활시켜줌
				if( pPlayer->IsDead() && pPlayer->IsInBattleArena() )
				{
					// 시작 지점으로 이동시켜 줄 필요는 없음(재접속 시 상황에 따라 처리될테니)
					// 아레나 내부에서 부활 시에는 모든 지속효과를 복구하지만 이 시점에서는 복구를 해주더라도 퇴장 처리에서 결국 모두 삭제되기 때문에 복구해주지 않는다.
					pPlayer->Resurrect( CRT_BATTLE_ARENA, pPlayer->GetMaxHP(), pPlayer->GetMaxMP(), 0, false );
				}

				return RESULT_SUCCESS;
			}
		}
	}

	return RESULT_NOT_EXIST;
}

unsigned short	BattleArenaInstance::_reconnectPlayer( StructPlayer* pPlayer, ArPosition* pPosStart, unsigned char* pnLayer )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	int nPartyID = pPlayer->GetPartyID();
	PlayerUID nPlayerUID = pPlayer->GetPlayerUID();
	int nTeamNo = _getTeamNo( nPartyID );
	if( nTeamNo != INVALID_BATTLE_ARENA_TEAM_NO )
	{
		std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin(); it != pPlayerInfoList->end(); ++it )
			{
				PlayerInfo& pi = (*it);
				if( pi.nPlayerUID != nPlayerUID )
					continue;

				if( pi.pPlayer )
				{
					assert( 0 );
					return RESULT_ALREADY_EXIST;
				}

				// 가명 사용 여부 다시 세팅
				if( !m_pArenaBase->IsExerciseGameArena() )
					pPlayer->SetUseAlias( true );

				// 플레이어에게 아레나 정보 다시 세팅
				pPlayer->SetBattleArenaID( m_pArenaBase->nID );
				pPlayer->SetBattleArenaInstanceNo( m_nInstanceNo );

				// PlayerInfo에 유저 온라인 상태로 변경 및 온라인 상태에 따라 갱신되어야 하는 내용 갱신
				pi.pPlayer = pPlayer;
				pi.nPenaltyCount = pPlayer->GetBattleArenaPenaltyCount();
				pi.nOfflineLimitTime = 0;

				// OnReconnect -> _reconnectPlayer 가 호출되는 시점은 캐릭터 로그인으로 로드된 정보가
				// 클라이언트에 방송되기 전이므로 여기서 패킷을 방송해도 소용 없음
				// 스크립트 실행 도 마찬가지로 방송을 동반하는 스크립트가 실행되면
				// 클라이언트에서 정상적으로 처리되지 않을 가능성이 높음
				// 따라서 이 지점에서 처리해야 할 각종 패킷 방송 및 스크립트 실행을
				// OnReconnectPostProc -> _reconnectPlayerPostProc 에서 처리되도록 분리하고,
				// 클라로 로그인 시 방송되어야 할 기본적인 정보를 방송한 후에 해당 처리를 함

				// 다시 참여한 경기로 진입해야 할 좌표 세팅(해당 팀 시작 지점)
				if( pPosStart )
					*pPosStart = m_pArenaBase->posStart[ nTeamNo ];
				if( pnLayer )
					*pnLayer = m_nInstanceNo;

				return RESULT_SUCCESS;
			}
		}
	}

	return RESULT_ACCESS_DENIED;
}

void	BattleArenaInstance::_reconnectPlayerPostProc( StructPlayer * pPlayer ) const
{
	int nTeamNo = _getTeamNo( pPlayer->GetPartyID() );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		assert( 0 );
		return;
	}

	// 다시 참여된 경기 정보 보내 줌
	BattleArenaManager::SendBattleArenaBattleInfoMessage( pPlayer, this );
	BattleArenaManager::SendBattleArenaBattleStatusMessage( pPlayer, this );

	// 시작되기 전의 연습 경기이고, 팀의 리더라면 레디 상태를 보내 줌
	if( m_pArenaBase->IsExerciseGameArena() && !m_bStart )
	{
		int nPartyID = pPlayer->GetPartyID();
		if( nPartyID == m_anPartyID[ nTeamNo ][ 0 ] && PartyManager::GetInstance().IsLeader( nPartyID, pPlayer->GetPlayerUID() ) )
			BattleArenaManager::BroadcastBattleArenaExerciseReadyState( this );
	}

	// 현재 스코어 정보도 보내 줌
	BattleArenaManager::SendBattleArenaScoreMessage( pPlayer, this );

	// 다른 유저들에게 현재 유저가 재접속했음을 알려 줌
	BattleArenaManager::BroadcastBattleArenaReconnectBattleMessage( this, pPlayer );

	// 입장 스크립트 실행(추가로 재접속에 의한 것이라는 파라미터를 세팅해 줌)
	std::vector< std::string > vTagReplacement;
	vTagReplacement.push_back( "#@reconnect@#" );
	vTagReplacement.push_back( "1" );

	ThreadPlayerHelper TPHelper( pPlayer );
	BattleArenaRunEventScript( m_pArenaBase, BASET_ENTER_INSTANCE, m_nInstanceNo, pPlayer, &vTagReplacement );
}

unsigned short BattleArenaInstance::_pendEnterWhileCountdown( StructPlayer * pPlayer )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 입장 처리가 발생하면 페널티를 받게 되는 경우를 미리 필터링
	unsigned short nErrorCode = RESULT_SUCCESS;
	// 사망 상태로는 이용 불가
	if( pPlayer->IsDead() )						nErrorCode = RESULT_NOT_ENOUGH_HP;
	// 거래 중엔 이용 불가
	else if( pPlayer->IsTrading() )				nErrorCode = RESULT_NOT_ACTABLE_WHILE_TRADING;
	// 베어로드, 데스매치, 인던에서는 이용 불가
	else if( pPlayer->IsInHuntaholic() )		nErrorCode = RESULT_NOT_ACTABLE_IN_HUNTAHOLIC;
	else if( pPlayer->IsInDeathmatch() )		nErrorCode = RESULT_NOT_ACTABLE_IN_DEATHMATCH;
	else if( pPlayer->IsInInstanceDungeon() )	nErrorCode = RESULT_NOT_ACTABLE_IN_INSTANCE_DUNGEON;

	if( nErrorCode != RESULT_SUCCESS )
		return nErrorCode;

	int nTeamNo = _getTeamNo( pPlayer->GetPartyID() );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		return RESULT_NOT_EXIST;

	// 이미 끝난 경기면 못 들어감(시작된 후에는 들어갈 수 있음 - 빠른 입장으로 입장 카운트 중 난입)
	if( m_bEnd )
		return RESULT_NOT_ACTABLE;

	PlayerUID nPlayerUID = pPlayer->GetPlayerUID();

	PlayerInfo * pPlayerInfo = _getPlayerInfo( nPlayerUID, nTeamNo );
	if( !pPlayerInfo )
	{
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	// 이미 입장된 경우나 혹은 예정 시점이 경과한 경우(이 함수가 호출된 직후에 BattleArenaInstance::_onProcess가 호출되면서 처리될 예정)에는 패스
	AR_TIME tCurrent = GetArTime();
	if( !pPlayerInfo->nPendedEnterTime || pPlayerInfo->nPendedEnterTime <= tCurrent )
		return RESULT_ACCESS_DENIED;

	// 입장 예정 시점만 당겨놓고, 실제 입장 처리는 BattleArenaInstance::_onProcess에서 일어나도록 놔둠
	pPlayerInfo->nPendedEnterTime = tCurrent;
	return RESULT_SUCCESS;
}

unsigned short BattleArenaInstance::_activateProp( StructPlayer * pPlayer, int nPropIndex )
{
	if( !m_bStart || m_bEnd )
		return RESULT_NOT_ACTABLE;

	int nTeamNo = _getTeamNo( pPlayer->GetPartyID() );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		return RESULT_ACCESS_DENIED;

	PlayerInfo * pPlayerInfo = _getPlayerInfo( pPlayer->GetPlayerUID(), nTeamNo );
	if( !pPlayerInfo )
		return RESULT_ACCESS_DENIED;

	_PROP_STATE ePropState = ( nTeamNo == 0 ) ? PS_OWNED_BY_TEAM_0 : PS_OWNED_BY_TEAM_1;

	std::string strPropState = _getFlag( FLAG_BATTLE_ARENA_PROP_STATE );

	if( strPropState.empty() )
		return RESULT_NOT_ACTABLE;

	if( nPropIndex < 1 || nPropIndex > 9 )
	{
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	// Zero-based index로 변환하기 위해 1 뺌
	--nPropIndex;

	int nWholePropState = atoi( strPropState.c_str() );
	int nPropState = ( nWholePropState >> ( nPropIndex * 2 ) ) & 0x3;

	// 프랍이 월드에서 사라진 상태라면 실패(빙고에서만 발생)
	if( nPropState == PS_NOT_EXIST )
		return RESULT_NOT_READY;

	// 이미 현재 팀 소유 프랍이면 실패
	if( nPropState == ePropState )
		return RESULT_ALREADY_EXIST;

	// 지정된 프랍 활성화 상태로 플래그 변경
	int nBitMask = ~( 0x3 << ( nPropIndex * 2 ) );
	nWholePropState = ( nWholePropState & nBitMask ) | ( ePropState << ( nPropIndex * 2 ) );
	// 빙고 타입의 경기에서만 사용되는 빙고 체크 값 제거
	nWholePropState &= 0x00FFFFFF;

	// 변경된 값으로 플래그 세팅
	XStringUtil::Format( strPropState, "%d", nWholePropState );
	_setFlag( FLAG_BATTLE_ARENA_PROP_STATE, strPropState.c_str() );

	// 상태 변경된 채로 참여자들에게 방송
	BattleArenaManager::BroadcastBattleArenaBattleStatusMessage( this );

	if( !m_pArenaBase->IsExerciseGameArena() )
	{
		// 개인별 실시간 AP 지급
		// 프랍 활성화에 의해 얻을 수 있는 최대 AP 제한 적용
		if( pPlayerInfo->nTotalGainAPByProp < m_pArenaBase->nMaxAPByPropActivate )
		{
			int nGainAP = m_pArenaBase->nMaxAPByPropActivate - pPlayerInfo->nTotalGainAPByProp;
			if( nGainAP > m_pArenaBase->nAPPerPropActivate )
				nGainAP = m_pArenaBase->nAPPerPropActivate;

			pPlayer->AddBattleArenaPoint( nGainAP );
			pPlayerInfo->nTotalGainAPByProp += nGainAP;
		}
	}

	++pPlayerInfo->nPropActivateCount;

	// 점수 방송(팀 점수 증가는 프랍 활성화로는 없고 킬로만 발생)
	BattleArenaManager::BroadcastBattleArenaScoreUpdateMessage( this, pPlayer, nTeamNo );

	// 경기 타입별 프랍 동작에 따른 처리
	switch( m_pArenaBase->eType )
	{
	case BAT_SLAUGHTER:
		// 슬래터에서는 0번 프랍 소유권에 따라 경험치 배율을 변경해 줌
		if( !nPropIndex )
		{
			if( nTeamNo == 0 )
			{
				m_aTeamInfo[ 0 ].fKillScoreRate = 2;
				m_aTeamInfo[ 1 ].fKillScoreRate = 1;
			}
			else if( nTeamNo == 1 )
			{
				m_aTeamInfo[ 0 ].fKillScoreRate = 1;
				m_aTeamInfo[ 1 ].fKillScoreRate = 2;
			}
			else
			{
				// 두 팀 이상이 참가하는 슬래터 경기 타입은 기획되어 있지 않음
				assert( 0 );
			}

			// 참여자들에게 필요한 채팅 방송
			std::string strMessage;
			XStringUtil::Format( strMessage, "@2488\v#@arena_team@#\v@%d\v#@player_name@#\v%s",
				GameRule::GetBattleArenaTeamNameStringID( nTeamNo, true ), pPlayer->GetAlias() );

			for( int _nTeamNo = 0 ; _nTeamNo < m_pArenaBase->nTeamCount ; ++_nTeamNo )
			{
				std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( _nTeamNo );
				if( pPlayerInfoList != NULL )
				{
					for( std::vector< PlayerInfo >::const_iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
					{
						const PlayerInfo & pi = (*it);
						if( !pi.pPlayer )
							continue;

						SendChatMessage( false, CHAT_CENTER_NOTICE, "@NOTICE", pi.pPlayer, strMessage.c_str() );
						if( nTeamNo == _nTeamNo )
							SendChatMessage( false, CHAT_BATTLE, "@SYSTEM", pi.pPlayer, "@2489" );
					}
				}
			}
		}
		break;
	case BAT_BINGO:
		{
			// 동작 시킨 빙고 프랍과 연결된 빙고 라인 체크
			static const unsigned char BINGO_PROP_INDICES[ 8 ][ 3 ] = {
				{ 0, 1, 2 },	// Line 0
				{ 3, 4, 5 },	// Line 1
				{ 6, 7, 8 },	// Line 2
				{ 0, 3, 6 },	// Line 3
				{ 1, 4, 7 },	// Line 4
				{ 2, 5, 8 },	// Line 5
				{ 0, 4, 8 },	// Line 6
				{ 2, 4, 6 }		// Line 7
			};
			static const unsigned char INVALID_LINE_INDEX = 0xFF;
			static const unsigned char BINGO_RELATED_LINE_INDICES[ 9 ][ 4 ] = {
				{ 0, 3, 6, INVALID_LINE_INDEX },
				{ 0, 4, INVALID_LINE_INDEX, INVALID_LINE_INDEX },
				{ 0, 5, 7, INVALID_LINE_INDEX },
				{ 1, 3, INVALID_LINE_INDEX, INVALID_LINE_INDEX },
				{ 1, 4, 6, 7 },
				{ 1, 5, INVALID_LINE_INDEX, INVALID_LINE_INDEX },
				{ 2, 3, 7, INVALID_LINE_INDEX },
				{ 2, 4, INVALID_LINE_INDEX, INVALID_LINE_INDEX },
				{ 2, 5, 6, INVALID_LINE_INDEX }
			};

			std::vector< int > vBingoLines;
			for( int nCheckLineIndex = 0 ; nCheckLineIndex < 4 ; ++nCheckLineIndex )
			{
				int nCurrentLineIndex = BINGO_RELATED_LINE_INDICES[ nPropIndex ][ nCheckLineIndex ];
				if( nCurrentLineIndex == INVALID_LINE_INDEX )
					break;

				int nCheckPropIndex;
				for( nCheckPropIndex = 0 ; nCheckPropIndex < 3 ; ++nCheckPropIndex )
				{
					int nCurrentPropIndex = BINGO_PROP_INDICES[ nCurrentLineIndex ][ nCheckPropIndex ];
					if( ( ( nWholePropState >> ( nCurrentPropIndex * 2 ) ) & 0x3 ) != ePropState )
						break;
				}

				// 현재 체크한 라인이 빙고 달성된 경우
				if( nCheckPropIndex >= 3 )
				{
					vBingoLines.push_back( nCurrentLineIndex );
				}
			}

			// 달성된 빙고가 없다면 추가 처리 스킵
			if( vBingoLines.empty() )
				break;

			// 빙고 달성에 따른 스크립트 처리(지역락 문제를 유발할 수 있기 때문에 여기서 직접 실행하지 못하고 예약만 함)
			std::string strBingoScript;
			XStringUtil::Format( strBingoScript, "on_battle_arena_bingo( %d, %d, %d, %d )", m_pArenaBase->nID, (int)m_nInstanceNo, nTeamNo, (int)vBingoLines.size() );
			_pendScript( pPlayer, BASET_BINGO, strBingoScript.c_str() );

			// 빙고에 참여한 프랍/라인 상태 변경
			std::vector< int > vPropIndicesToBeRemoved;
			for( std::vector< int >::const_iterator it = vBingoLines.begin() ; it != vBingoLines.end() ; ++it )
			{
				int nLineIndex = (*it);

				// 프랍 상태 변경(프랍 없음)
				for( int nCheckPropIndex = 0 ; nCheckPropIndex < 3 ; ++nCheckPropIndex )
				{
					int nCurrentPropIndex = BINGO_PROP_INDICES[ nLineIndex ][ nCheckPropIndex ];
					nWholePropState &= ~( 0x3 << ( nCurrentPropIndex * 2 ) );

					// 빙고 참여 프랍 인덱스 보관(프랍 제거 처리용)
					if( std::find( vPropIndicesToBeRemoved.begin(), vPropIndicesToBeRemoved.end(), nCurrentPropIndex ) == vPropIndicesToBeRemoved.end() )
						vPropIndicesToBeRemoved.push_back( nCurrentPropIndex );
				}

				// 빙고 라인 세팅
				nWholePropState |= 1 << ( nLineIndex + 24 );
			}

			// 변경된 값으로 플래그 세팅
			XStringUtil::Format( strPropState, "%d", nWholePropState );
			_setFlag( FLAG_BATTLE_ARENA_PROP_STATE, strPropState.c_str() );

			// 상태 변경된 채로 참여자들에게 방송
			BattleArenaManager::BroadcastBattleArenaBattleStatusMessage( this );

			// 빙고 라인 번호 플래그 제거(향후 플래그값 방송에서 다시 빙고가 된 것처럼 방송될 가능성 방지)
			nWholePropState &= 0x00FFFFFF;
			XStringUtil::Format( strPropState, "%d", nWholePropState );
			_setFlag( FLAG_BATTLE_ARENA_PROP_STATE, strPropState.c_str() );
			// 이 값으로 다시 즉시 방송할 필요는 없음

			// 빙고 참여 프랍 제거
			// * 특정 빙고 프랍이 동작해서 빙고가 완성되었을 때 지역락은 마지막에 발동된 프랍과 시전자 주변만 걸려있지만
			//   같은 레이어 상에 있는 유저들은 csArena, csBattle을 걸지 않은 상태로 vPlayerInfo 에서 빠져나가지 못하므로
			//   (StructPlayer::LogoutNow -> BattleArenaManager::OnDisconnect)
			//   패킷 방송 대상(프랍의 TS_LEAVE를 받을 유저들)이 변할 가능성은 없음.
			//   이거 믿고 위에서도 BroadcastBattleArenaBattleStatusMessage 같은 거 막 호출함 -_ -ㅋ;
			for( std::vector< int >::const_iterator it = vPropIndicesToBeRemoved.begin() ; it != vPropIndicesToBeRemoved.end() ; ++it )
			{
				// DB 상에서 지정한 프랍 번호는 1 ~ 9 가 되고, 0 은 번호를 갖지 않는 프랍을 의미함
				// vPropIndicesToBeRemoved에 있는 프랍 번호는 0 ~ 8 이므로 1을 더해서 사용해야 함
				int nRemovePropIndex = (*it) + 1;

				bool bRemoved = false;

				for( std::vector< StructFieldProp * >::iterator itProp = m_vRespawnedFieldProp.begin() ; itProp != m_vRespawnedFieldProp.end() ; /* 루프에서 ++itProp 처리 */ )
				{
					StructFieldProp * pProp = (*itProp);

					const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * pRespawn = static_cast< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >( pProp->GetRespawnInfo() );

					if( pRespawn->nPropIndexInArena != nRemovePropIndex )
					{
						++itProp;
						continue;
					}

					ArcadiaServer::Instance().RemoveObject( pProp );
					StructFieldProp::PendFreeFieldProp( pProp );

					bRemoved = true;
					break;
				}

				assert( bRemoved );
			}

			// 참여자들에게 필요한 채팅 방송
			std::string strMessage;
			XStringUtil::Format( strMessage, "@2490\v#@arena_team@#\v@%d\v#@player_name@#\v%s",
				GameRule::GetBattleArenaTeamNameStringID( nTeamNo, true ), pPlayer->GetAlias() );

			for( int _nTeamNo = 0 ; _nTeamNo < m_pArenaBase->nTeamCount ; ++_nTeamNo )
			{
				std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( _nTeamNo );
				if( pPlayerInfoList != NULL )
				{
					for( std::vector< PlayerInfo >::const_iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
					{
						const PlayerInfo & pi = (*it);
						if( !pi.pPlayer )
							continue;

						SendChatMessage( false, CHAT_CENTER_NOTICE, "@NOTICE", pi.pPlayer, strMessage.c_str() );
						if( nTeamNo == _nTeamNo )
							SendChatMessage( false, CHAT_BATTLE, "@SYSTEM", pi.pPlayer, "@2491" );
					}
				}
			}

		}
		break;
	}

	return RESULT_SUCCESS;
}

_PROP_STATE BattleArenaInstance::_getPropState( int nPropIndex ) const
{
	if( nPropIndex < 1 || nPropIndex > 9 )
	{
		assert( 0 );
		return PS_NOT_EXIST;
	}

	// Zero-based index로 변환하기 위해 1 뺌
	--nPropIndex;

	std::string strPropState = _getFlag( FLAG_BATTLE_ARENA_PROP_STATE );
	int nPropState = atoi( strPropState.c_str() );

	return static_cast< _PROP_STATE >( ( nPropState >> ( nPropIndex * 2 ) ) & 0x3 );
}

void BattleArenaInstance::_setPropState( int nPropIndex, _PROP_STATE ePropState )
{
	if( nPropIndex < 1 || nPropIndex > 9 )
	{
		assert( 0 );
		return;
	}

	// Zero-based index로 변환하기 위해 1 뺌
	--nPropIndex;

	std::string strPropState = _getFlag( FLAG_BATTLE_ARENA_PROP_STATE );
	int nPropState = atoi( strPropState.c_str() );

	// 지정된 프랍 활성화 상태로 플래그 변경
	int nBitMask = ~( 0x3 << ( nPropIndex * 2 ) );
	nPropState = ( nPropState & nBitMask ) | ( ePropState << ( nPropIndex * 2 ) );

	// 변경된 값으로 플래그 세팅
	XStringUtil::Format( strPropState, "%d", nPropState );
	_setFlag( FLAG_BATTLE_ARENA_PROP_STATE, strPropState.c_str() );
}

bool BattleArenaInstance::_isCastablePropForTeam( const StructFieldProp * pProp, int nTeamNo ) const
{
	// 지정된 프랍이 해당 아레나에서 경기에 영향을 주는 타입으로 리젠된 프랍인지 체크
	if( std::find( m_vRespawnedFieldProp.begin(), m_vRespawnedFieldProp.end(), pProp ) == m_vRespawnedFieldProp.end() )
		return false;

	const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * pRespawn = static_cast< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >( pProp->GetRespawnInfo() );

	if( pRespawn->nPropIndexInArena < 1 || pRespawn->nPropIndexInArena > 9 )
		return false;

	// 이미 해당 프랍을 지정된 팀에서 소유 중인지 체크
	_PROP_STATE ePropState = _getPropState( pRespawn->nPropIndexInArena );
	switch( nTeamNo )
	{
	case 0:	if( ePropState == PS_OWNED_BY_TEAM_0 ) return false; break;
	case 1: if( ePropState == PS_OWNED_BY_TEAM_1 ) return false; break;
	default:
		assert( 0 );
		return false;
	}

	return true;
}

unsigned short BattleArenaInstance::_onPlayerDead( StructPlayer * pPlayer, StructPlayer * pKiller )
{
	// 이미 목표 점수를 도달한 팀이 있다면 플레이어 킬에 대해 아무런 처리도 하지 않음
	// * 플레이어 킬 외에 점수 획득 수단이 없으므로 여기서만 체크하면 되지만,
	//   향후 점수 획득 수단이 늘어나면 각 점수 획득 가능 위치마다 이 체크를 해 줘야 함
	if( m_bObjectiveScoreReached )
		return RESULT_LIMIT_MAX;

	int nTeamNo = _getTeamNo( pPlayer->GetPartyID() );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		return RESULT_NOT_OWN;

	int nKillerTeamNo = _getTeamNo( pKiller->GetPartyID() );
	if( nKillerTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		return RESULT_NOT_OWN;

	// 같은 팀원한테 죽은 상황이면 아무것도 안 함
	if( nTeamNo == nKillerTeamNo )
	{
		// 근데 팀킬이 일반적으로 불가능해야 함;;
		assert( 0 );
		return RESULT_SUCCESS;
	}

	// 죽은 사람은 데스 카운트 증가를 위해 PlayerInfo가 필요
	PlayerInfo * pDeadPlayerInfo = _getPlayerInfo( pPlayer->GetPlayerUID(), nTeamNo );
	if( !pDeadPlayerInfo )
		return RESULT_NOT_OWN;

	// 킬러의 경우 킬 카운트 및 획득 총점 누적을 위해 PlayerInfo가 필요
	PlayerInfo * pKillerPlayerInfo = _getPlayerInfo( pKiller->GetPlayerUID(), nKillerTeamNo );
	if( !pKillerPlayerInfo )
		return RESULT_NOT_OWN;

	// 진행 중인 경기가 아니면 아무것도 안 함(성공으로 리턴하지는 않지만 별 의미 없음)
	if( !m_bStart || m_bEnd )
		return RESULT_NOT_READY;

	// 죽은 사람 팀의 데스 카운트 증가 및 킬러 팀/유저 킬 카운트, 점수 가산 처리
	++m_aTeamInfo[ nTeamNo ].nTotalDeathCount;
	++m_aTeamInfo[ nKillerTeamNo ].nTotalKillCount;
	m_aTeamInfo[ nKillerTeamNo ].nScore += m_aTeamInfo[ nKillerTeamNo ].fKillScoreRate * m_pArenaBase->nScorePerKill;
	if( m_aTeamInfo[ nKillerTeamNo ].nScore > m_pArenaBase->nObjectivePoint )
		m_aTeamInfo[ nKillerTeamNo ].nScore = m_pArenaBase->nObjectivePoint;

	// 킬러 AP 지급(연습 경기면 안 함)
	if( !m_pArenaBase->IsExerciseGameArena() )
	{
		// 개인별 실시간 AP 지급
		// 킬에 의해 얻을 수 있는 최대 AP 제한 적용
		if( pKillerPlayerInfo->nTotalGainAPByKill < m_pArenaBase->nMaxAPByKill )
		{
			int nGainAP = m_pArenaBase->nMaxAPByKill - pKillerPlayerInfo->nTotalGainAPByKill;
			if( nGainAP > m_pArenaBase->nAPPerKill )
				nGainAP = m_pArenaBase->nAPPerKill;

			pKiller->AddBattleArenaPoint( nGainAP );
			pKillerPlayerInfo->nTotalGainAPByKill += nGainAP;
		}
	}

	++pDeadPlayerInfo->nDeathCount;
	++pKillerPlayerInfo->nKillCount;

	// 점수 방송
	BattleArenaManager::BroadcastBattleArenaScoreUpdateMessage( this, pKiller, nKillerTeamNo, pPlayer, nTeamNo );

	// 유저 킬 시 스크립트 실행(지역락 문제를 유발할 수 있기 때문에 여기서 직접 실행하지 못하고 예약만 함)
	if( !m_pArenaBase->strScriptOnKill.empty() )
	{
		std::string strScript = m_pArenaBase->strScriptOnKill;

		XStringUtil::ReplaceFormat( strScript, "#@instance_no@#", "%d", (int)m_nInstanceNo );
		XStringUtil::ReplaceFormat( strScript, "#@dead_player_name@#", "%s", pPlayer->GetName() );

		_pendScript( pKiller, BASET_KILL_PLAYER, strScript.c_str() );
	}

	// 경기에 참여 중인 모든 유저들에게 방송
	std::string strNotice;
	XStringUtil::Format( strNotice, "@2439\v#@kill_player_name@#\v%s\v#@death_player_name@#\v%s", pKiller->GetAlias(), pPlayer->GetAlias() );
	for( int nTeamNo = 0 ; nTeamNo < m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			for( std::vector< PlayerInfo >::const_iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
			{
				const PlayerInfo & pi = (*it);

				if( pi.pPlayer )
					SendChatMessage( false, CHAT_CENTER_NOTICE, "@NOTICE", pi.pPlayer, strNotice.c_str() );
			}
		}
	}

	// 목표 점수 도달 체크
	if( m_aTeamInfo[ nKillerTeamNo ].nScore >= m_pArenaBase->nObjectivePoint )
	{
		// 여기서 바로 _finishBattle을 호출하지 않고 _onProcess에서 _finishBattle이 호출될 수 있도록 플래그만 세팅
		// * 지역 락 문제 때문에 특정 유저 종속적인 락을 걸고 실행되는 코드에서 _finishBattle을 호출할 수 없음
		//   이로 인해 플래그만 세팅되고 나서 승리 처리 전에 경기가 파토나는 경우가 있을 수 있는데,
		//   이미 시작된 경기에 대해 _finishBattle 함수를 통해 종료되는 처리는 모두 BattleArenaInstance::_onProcess 안에 있으므로
		//   _onProcess 안에서만 종료 조건 체크 순서에 주의하면 로직상 크게 문제가 발생할 사례는 없을 것으로 생각됨.
		//   단, 거꾸로 악용해서 모든 유저가 이탈하는 바람에 파토날 걸로 확정된 방에서
		//   _onProcess 들어오기 전에 킬해서 목표 점수를 도달하면 목표 점수 도달로 끝나게 됨.
		//   근데 이 정도는 악용도 쉽지 않거니와 소소한 여흥거리 정도로 봐줘도 문제 없다고 보임.
		m_bObjectiveScoreReached = true;
	}

	return RESULT_SUCCESS;
}

unsigned short BattleArenaInstance::_requestAbsenceCheck( StructPlayer * pPlayer, AR_HANDLE hCheckTarget )
{
	int nPartyID = pPlayer->GetPartyID();
	int nTeamNo = _getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		return RESULT_NOT_EXIST;

	// 자기 자신에게는 잠수 신고 불가
	if( pPlayer->GetHandle() == hCheckTarget )
		return RESULT_LIMIT_TARGET;

	std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
	if( pPlayerInfoList != NULL )
	{
		// 잠수 신고는 같은 팀원 끼리만 가능하므로 신고자의 팀 내에서만 대상 검색
		for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
		{
			PlayerInfo & pi = (*it);

			if( !pi.pPlayer || pi.pPlayer->GetHandle() != hCheckTarget )
				continue;

			// 현재 잠수 신고 당한 상태 체크
			if( pi.bWaitingAbsenceCheck )
				return RESULT_ALREADY_EXIST;

			AR_TIME tCurrent = GetArTime();

			// 잠수 신고 쿨타임 체크(nLastAbsenceCheckTime를 신고 응답 대기 상태가 아닐 때에는 다음 체크 가능 시점으로 사용함)
			if( pi.nLastAbsenceCheckTime > tCurrent )
				return RESULT_COOL_TIME;

			// 잠수 신고 체크 시작
			pi.bWaitingAbsenceCheck = true;
			pi.nLastAbsenceCheckTime = tCurrent + GameRule::BATTLE_ARENA_ABSENCE_CHECK_ANSWER_LIMIT;

			BattleArenaManager::SendBattleArenaAbsenceCheck( pi.pPlayer, pi.nLastAbsenceCheckTime );

			// 팀원들에게 누군가가 누군가에게 잠수 신고를 당했다는 내용 방송
			PrintfLinkedPartyChatMessage( false, CHAT_BATTLE, "@SYSTEM", nPartyID,
				"@2443\v#@report_player@#\v%s\v#@dive_player@#\v%s",
				pPlayer->GetAlias(), pi.pPlayer->GetAlias() );

			return RESULT_SUCCESS;
		}
	}

	return RESULT_LIMIT_TARGET;
}

unsigned short BattleArenaInstance::_answerAbsenceCheck( StructPlayer * pPlayer, bool bSuccess )
{
	int nPartyID = pPlayer->GetPartyID();
	int nTeamNo = _getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		return RESULT_NOT_EXIST;

	PlayerInfo * pPlayerInfo = _getPlayerInfo( pPlayer->GetPlayerUID(), nTeamNo );

	if( !pPlayerInfo )
		return RESULT_NOT_EXIST;

	// 잠수 신고 응답 대기 상태가 아닌 경우
	if( !pPlayerInfo->bWaitingAbsenceCheck )
		return RESULT_ACCESS_DENIED;

	// 이미 응답 제한 시간이 초과된 경우는 체크하지 않음(onProcess에서 이탈 처리가 되기 전에 응답이 왔다는 거니까)

	if( bSuccess )
	{
		// 응답이 성공한 경우에는 해제 및 쿨타임 세팅
		pPlayerInfo->bWaitingAbsenceCheck = false;
		pPlayerInfo->nLastAbsenceCheckTime = GetArTime() + GameRule::BATTLE_ARENA_ABSENCE_CHECK_COOLTIME;

		// 잠수 신고를 해제했다는 내용을 팀원들에게 방송
		PrintfLinkedPartyChatMessage( false, CHAT_BATTLE, "@SYSTEM", nPartyID, "@2444\v#@player_name@#\v%s", pPlayer->GetAlias() );
	}
	else
	{
		// 응답이 실패한 경우에는 여기서 직접 접속 종료 처리를 해버리지 않고 시간 제한을 앞당겨서 다음 onProcess에서 튕겨나가게 처리
		pPlayerInfo->nLastAbsenceCheckTime = 0;
	}

	return RESULT_SUCCESS;
}

unsigned short BattleArenaInstance::_setReadyExerciseGame( StructPlayer * pPlayer, bool bReady )
{
	// 경기가 시작된 이후나 끝난 상태라면 변경 불가
	if( m_bStart || m_bEnd )
		return RESULT_NOT_ACTABLE;

	// 팀 번호 확인
	int nPartyID = pPlayer->GetPartyID();
	int nTeamNo = _getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		// 응;?
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	// 레디 상태 변경은 해당 팀의 첫 번째 파티의 파티장만이 가능
	if( nPartyID != m_anPartyID[ nTeamNo ][ 0 ] || !PartyManager::GetInstance().IsLeader( nPartyID, pPlayer->GetPlayerUID() ) )
		return RESULT_ACCESS_DENIED;

	// 레디를 하려고 할 때 해당 팀의 멤버 수가 조건에 맞는지 체크
	if( m_pArenaBase->nMinMember / m_pArenaBase->nTeamCount > _getTeamMemberCount( nTeamNo ) )
		return RESULT_LIMIT_MIN;
	if( m_pArenaBase->nMaxMember / m_pArenaBase->nTeamCount < _getTeamMemberCount( nTeamNo ) )
		return RESULT_LIMIT_MAX;

	std::string strReady = _getFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE );
	int nReadyState = atoi( strReady.c_str() );

	// 이미 요청한 레디 상태에 해당한다면 아무것도 안 함
	if( static_cast< bool >( nReadyState & ( 1 << nTeamNo ) ) == bReady )
		return RESULT_SUCCESS;

	nReadyState ^= ( 1 << nTeamNo );
	XStringUtil::Format( strReady, "%d", nReadyState );

	_setFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE, strReady.c_str() );

	BattleArenaManager::BroadcastBattleArenaExerciseReadyState( this );

	return RESULT_SUCCESS;
}

size_t BattleArenaInstance::_doEachPlayer( ArObjectFunctor & fo )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// fo 내부에서 지역락 걸면 안 됨

	size_t nCount = 0;
	for( int nTeamNo = 0 ; nTeamNo < m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		nCount += _doEachPlayerInTeam( nTeamNo, fo );
	}

	return nCount;
}

size_t BattleArenaInstance::_doEachPlayerInTeam( int nTeamNo, ArObjectFunctor & fo )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 이 함수를 실행하는 do_each_player_in_battle_arena_instance, do_each_player_in_battle_arena_team 함수는
	// 아레나 영역 전체에 지역락을 건 상태로만 실행되어야 함
	assert( ArcadiaServer::Instance().IsLocked( m_pArenaBase->nArenaRegionLeft, m_pArenaBase->nArenaRegionTop, m_pArenaBase->nArenaRegionRight, m_pArenaBase->nArenaRegionBottom, m_nInstanceNo ) );

	// fo 내부에서 지역락 걸면 안 됨

	size_t nCount = 0;
	std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
	if( pPlayerInfoList != NULL )
	{
		for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
		{
			PlayerInfo & pi = (*it);
			StructPlayer * pPlayer = pi.pPlayer;

			// 오프라인이거나 입장 대기 중 또는 경기장 외부인 유저는 스크립트 실행 대상 제외(지역 락 이슈)
			if( !pPlayer || pi.nPendedEnterTime || !pPlayer->IsInBattleArena() )
				continue;

			fo( pPlayer );
			++nCount;
		}
	}

	return nCount;
}

BattleArenaInstance::PlayerInfo * BattleArenaInstance::_getPlayerInfo( PlayerUID nPlayerUID, int nTeamNo )
{
	// vPlayerInfo[ 팀 번호 ].reserve( GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM ) 가 되어있지 않으면
	// 이 함수에서 리턴한 포인터의 주소가 무효화되는 경우가 발생될 수 있음(vPlayerInfo 내부 버퍼의 reallocation이 발생할 수 있으므로)
	// 그래서 꼭 vPlayerInfo[ 팀 번호 ].reserve( GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM ) 를 BattleArenaInstance 생성자에서 해 줌

	// 팀 번호가 지정되어 있지 않으면 모든 팀 검색
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		for( int _nTeamNo = 0 ; _nTeamNo < m_pArenaBase->nTeamCount ; ++_nTeamNo )
		{
			std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( _nTeamNo );
			if( pPlayerInfoList != NULL )
			{
				for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
				{
					if( (*it).nPlayerUID == nPlayerUID )
						return &(*it);
				}
			}
		}
	}
	// 팀 번호가 지정되어 있으면 해당 팀 내에서만 검색
	else
	{
		std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
			{
				if( (*it).nPlayerUID == nPlayerUID )
					return &(*it);
			}
		}
	}

	return NULL;
}

const BattleArenaInstance::PlayerInfo * BattleArenaInstance::_getPlayerInfo( PlayerUID nPlayerUID, int nTeamNo ) const
{
	// vPlayerInfo[ 팀 번호 ].reserve( GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM ) 가 되어있지 않으면
	// 이 함수에서 리턴한 포인터의 주소가 무효화되는 경우가 발생될 수 있음(vPlayerInfo 내부 버퍼의 reallocation이 발생할 수 있으므로)
	// 그래서 꼭 vPlayerInfo[ 팀 번호 ].reserve( GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM ) 를 BattleArenaInstance 생성자에서 해 줌

	// 팀 번호가 지정되어 있지 않으면 모든 팀 검색
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		for( int _nTeamNo = 0 ; _nTeamNo < m_pArenaBase->nTeamCount ; ++_nTeamNo )
		{
			const std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( _nTeamNo );
			if( pPlayerInfoList != NULL )
			{
				for( std::vector< PlayerInfo >::const_iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
				{
					if( (*it).nPlayerUID == nPlayerUID )
						return &(*it);
				}
			}
		}
	}
	// 팀 번호가 지정되어 있으면 해당 팀 내에서만 검색
	else
	{
		const std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			for( std::vector< PlayerInfo >::const_iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
			{
				if( (*it).nPlayerUID == nPlayerUID )
					return &(*it);
			}
		}
	}

	return NULL;
}

void	BattleArenaInstance::_setFlag( const char* pszName, const char* pszValue )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	m_mapInstanceFlag[ pszName ] = pszValue;
}

const std::string&	BattleArenaInstance::_getFlag( const char* pszName ) const
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	static const std::string strEmpty;

	std::map< std::string, std::string >::const_iterator it = m_mapInstanceFlag.find( pszName );
	if( it == m_mapInstanceFlag.end() )
		return strEmpty;

	return it->second;
}

bool	BattleArenaInstance::_delFlag( const char* pszName )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	return (m_mapInstanceFlag.erase( pszName ) != 0);
}

void BattleArenaInstance::_pendScript( StructPlayer * pExecutor, enum _BATTLE_ARENA_SCRIPT_EXEC_TYPE eExecType, const char * pszScript )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	m_vPendedScript.push_back( PendedScript( pExecutor, eExecType, pszScript ) );
}

void BattleArenaInstance::_procPendedScript()
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	if( m_vPendedScript.empty() )
		return;

	// 혹시라도 스크립트에 의해 다시 _pendScript가 호출되면 vPendedScript에 대한 iterator가 무효화될 수 있으니 swap시킨 사본(?) 사용
	std::vector< PendedScript > _vPendedScript;
	_vPendedScript.swap( m_vPendedScript );

	for( std::vector< PendedScript >::const_iterator it = _vPendedScript.begin() ; it != _vPendedScript.end() ; ++it )
	{
		const PendedScript & pendedScript = (*it);

		ThreadPlayerHelper TPHelper( pendedScript.pExecutor );
		BattleArenaRunScriptWithLog( m_pArenaBase, pendedScript.eExecType, m_nInstanceNo, pendedScript.strScript, pendedScript.pExecutor );
	}
}

void BattleArenaInstance::_initialRespawn()
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 몬스터 리젠
	for( std::vector< const BATTLE_ARENA_MONSTER_RESPAWN * >::const_iterator it = m_pArenaBase->vMonsterRespawnList.begin() ; it != m_pArenaBase->vMonsterRespawnList.end() ; ++it )
	{
		const BATTLE_ARENA_MONSTER_RESPAWN * pRespawn = (*it);

		// 리젠 마리 수 만큼 반복
		for( int nIndex = 0 ; nIndex < pRespawn->nCount ; ++nIndex )
		{
			int nRespawnX = XRandom( static_cast< int >( pRespawn->bxArea.GetLeft() ), static_cast< int >( pRespawn->bxArea.GetRight() ) );
			int nRespawnY = XRandom( static_cast< int >( pRespawn->bxArea.GetTop() ), static_cast< int >( pRespawn->bxArea.GetBottom() ) );

			if( GameContent::IsBlocked( nRespawnX, nRespawnY ) == true )
			{
				FILELOG( "Unable to respawn monster in battle arena. ArenaID(%d), RespawnID(%d)", m_pArenaBase->nID, pRespawn->nID );
				_cprint( "Unable to respawn monster in battle arena. ArenaID(%d), RespawnID(%d)\n", m_pArenaBase->nID, pRespawn->nID );
			}

			StructMonster * pMob = respawnMonster( nRespawnX, nRespawnY, m_nInstanceNo, pRespawn->nMonsterID, pRespawn->bWandering, 0, this, false, 0, pRespawn->fFace );
			if( !pMob )
			{
				FILELOG( "Unknown monster is to be respawned in battle arena. ArenaID(%d), RespawnID(%d)", m_pArenaBase->nID, pRespawn->nID );
				_cprint( "Unknown monster is to be respawned in battle arena. ArenaID(%d), RespawnID(%d)\n", m_pArenaBase->nID, pRespawn->nID );
				continue;
			}

			pMob->SetInstanceRespawnID( pRespawn->nID );

			m_vRespawnedMonster.push_back( pMob );
		}
	}

	// 프랍 리젠
	bool bStatusBroadcastRequired = false;
	for( std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >::const_iterator it = m_pArenaBase->vFieldPropRespawnList.begin() ; it != m_pArenaBase->vFieldPropRespawnList.end() ; ++it )
	{
		const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * pRespawn = (*it);

		StructFieldProp * pProp = StructFieldProp::Create( this, pRespawn, pRespawn->x, pRespawn->y, m_nInstanceNo );

		if( !pProp )
		{
			FILELOG( "Unknown field prop is to be respawned in battle arena. ArenaID(%d), PropID(%d)", m_pArenaBase->nID, pRespawn->nPropId );
			_cprint( "Unknown field prop is to be respawned in battle arena. ArenaID(%d), PropID(%d)\n", m_pArenaBase->nID, pRespawn->nPropId );
			continue;
		}

		m_vRespawnedFieldProp.push_back( pProp );

		// 경기 내 프랍 상태 갱신
		if( pRespawn->nPropIndexInArena )
		{
			bStatusBroadcastRequired = true;
			_setPropState( pRespawn->nPropIndexInArena, PS_NEUTRAL );
		}
	}

	// 프랍 상태가 변경되었다면 상태 방송
	if( bStatusBroadcastRequired )
		BattleArenaManager::BroadcastBattleArenaBattleStatusMessage( this );
}

void BattleArenaInstance::_procMonsterRespawn()
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	AR_TIME tCurrent = GetArTime();

	// 몹이 리젠되어야 할 방인데 끝난 이후면 리젠시키지 않음
	if( m_bEnd )
	{
		// 끝난 방인데 왜 _procMonsterRespawn가 호출되었는지 확인해서 제거
		assert( 0 );
		m_vPendedMonsterRespawn.clear();
		return;
	}

	for( std::vector< PendedMonsterRespawnInfo >::iterator it = m_vPendedMonsterRespawn.begin() ; it != m_vPendedMonsterRespawn.end() ; /* 루프에서 ++it 처리 */ )
	{
		const PendedMonsterRespawnInfo & respawnInfo = (*it);
		const BATTLE_ARENA_MONSTER_RESPAWN * pRespawn = respawnInfo.first;

		if( respawnInfo.second > tCurrent )
		{
			++it;
			continue;
		}

		// 경기장 내부에서 몬스터 리젠은 이동 불가 영역에 관계 없이 어디든 리스폰이 되어야 함(프랍과 겹쳐서 화형대 지킴이 몬스터 리젠을 위해)
		int nRespawnX = XRandom( static_cast< int >( pRespawn->bxArea.GetLeft() ), static_cast< int >( pRespawn->bxArea.GetRight() ) );
		int nRespawnY = XRandom( static_cast< int >( pRespawn->bxArea.GetTop() ), static_cast< int >( pRespawn->bxArea.GetBottom() ) );

		if( GameContent::IsBlocked( nRespawnX, nRespawnY ) == true )
		{
			FILELOG( "Unable to respawn monster in battle arena. ArenaID(%d), RespawnID(%d)", m_pArenaBase->nID, pRespawn->nID );
			_cprint( "Unable to respawn monster in battle arena. ArenaID(%d), RespawnID(%d)\n", m_pArenaBase->nID, pRespawn->nID );
		}

		// 게임 진행 중에 죽은 각 몬스터에 대한 리젠 처리이기 때문에 pRespawn->nCount 로 저장되어 있는 몬스터 마리 수는 무시함
		StructMonster * pMob = respawnMonster( nRespawnX, nRespawnY, m_nInstanceNo, pRespawn->nMonsterID, pRespawn->bWandering, 0, this, false, 0, pRespawn->fFace );
		if( !pMob )
		{
			FILELOG( "Unknown monster is to be respawned in battle arena. ArenaID(%d), RespawnID(%d)", m_pArenaBase->nID, pRespawn->nID );
			_cprint( "Unknown monster is to be respawned in battle arena. ArenaID(%d), RespawnID(%d)\n", m_pArenaBase->nID, pRespawn->nID );

			it = m_vPendedMonsterRespawn.erase( it );
			continue;
		}

		pMob->SetInstanceRespawnID( pRespawn->nID );

		m_vRespawnedMonster.push_back( pMob );

		it = m_vPendedMonsterRespawn.erase( it );
	}
}

void BattleArenaInstance::_procFieldPropRespawn()
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	AR_TIME tCurrent = GetArTime();

	bool bStatusBroadcastRequired = false;
	for( std::vector< PendedFieldPropRespawnInfo >::iterator itRespawn = m_vPendedFieldPropRespawn.begin() ; itRespawn != m_vPendedFieldPropRespawn.end() ; /* 루프에서 ++itRespawn 처리 */ )
	{
		if( (*itRespawn).second > tCurrent )
		{
			++itRespawn;
			continue;
		}

		const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * pRespawn = (*itRespawn).first;

		StructFieldProp * pProp = StructFieldProp::Create( this, pRespawn, pRespawn->x, pRespawn->y, m_nInstanceNo );

		if( !pProp )
		{
			FILELOG( "Unknown field prop is to be respawned in battle arena. ArenaID(%d), PropID(%d)", m_pArenaBase->nID, pRespawn->nPropId );
			_cprint( "Unknown field prop is to be respawned in battle arena. ArenaID(%d), PropID(%d)\n", m_pArenaBase->nID, pRespawn->nPropId );

			itRespawn = m_vPendedFieldPropRespawn.erase( itRespawn );
			continue;
		}

		m_vRespawnedFieldProp.push_back( pProp );

		itRespawn = m_vPendedFieldPropRespawn.erase( itRespawn );

		// 경기 내 프랍 상태 갱신
		if( pRespawn->nPropIndexInArena )
		{
			bStatusBroadcastRequired = true;
			_setPropState( pRespawn->nPropIndexInArena, PS_NEUTRAL );
		}
	}

	// 프랍 상태가 변경되었다면 상태 방송
	if( bStatusBroadcastRequired )
		BattleArenaManager::BroadcastBattleArenaBattleStatusMessage( this );
}

void BattleArenaInstance::_clearMonster()
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 리젠 대기 중인 몬스터 목록 제거
	m_vPendedMonsterRespawn.clear();

	// 몬스터 제거 처리(지역 락 필요)
	for( std::vector< StructMonster * >::iterator itMonster = m_vRespawnedMonster.begin() ; itMonster != m_vRespawnedMonster.end() ; /* 루프에서 ++itMonster 처리 */ )
	{
		StructMonster * pMob = (*itMonster);

		if( !pMob->IsEnable() )
		{
			++itMonster;
			continue;
		}

		pMob->SetDeleteHandler( NULL );

		// 더이상의 스케쥴러 요청을 무시
		pMob->Disable();

		// 월드에서 제거한다.
		if( pMob->IsInWorld() )
		{
			RemoveMonsterFromWorld( pMob );
		}

		// object delete 요청
		ArcadiaServer::Instance().DeleteObject( pMob );

		itMonster = m_vRespawnedMonster.erase( itMonster );
	}

	m_vRespawnedMonster.clear();
}

void BattleArenaInstance::_clearFieldProps()
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 리젠 대기 중인 힐링 프랍 제거
	m_vPendedFieldPropRespawn.clear();

	// 필드 프랍 제거 처리(지역 락 필요)
	for( std::vector< StructFieldProp * >::iterator itFieldProp = m_vRespawnedFieldProp.begin() ; itFieldProp != m_vRespawnedFieldProp.end() ; ++itFieldProp )
	{
		StructFieldProp * pProp = (*itFieldProp);

		// 해당 프랍이 ProcDelete에서 다시 리젠되도록 vPendedFieldPropRespawn에 추가되지 않도록 DeleteHandler를 제거하고 삭제함
		pProp->SetDeleteHandler( NULL );

		// 유저가 사용함으로 인해서 RemoveObject/PendFreeFieldProp은 걸려 있지만 아직 onFieldPropDelete가 호출되지 않은 경우에는 아무것도 하지 않음
		if( !pProp->IsInWorld() )
			continue;

		ArcadiaServer::Instance().RemoveObject( pProp );
		StructFieldProp::PendFreeFieldProp( pProp );
	}

	m_vRespawnedFieldProp.clear();
}

void BattleArenaInstance::_clearArenaBlockerFieldProps()
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 필드 프랍 제거 처리(지역 락 필요)
	for( std::vector< StructFieldProp * >::iterator itFieldProp = m_vRespawnedArenaBlockerFieldProp.begin() ; itFieldProp != m_vRespawnedArenaBlockerFieldProp.end() ; ++itFieldProp )
	{
		StructFieldProp * pProp = (*itFieldProp);

		// 해당 프랍이 ProcDelete에서 다시 리젠되도록 vPendedFieldPropRespawn에 추가되지 않도록 DeleteHandler를 제거하고 삭제함
		// * 아레나 입장 제한 프랍은 원래 DeleteHandler 세팅하지 않으니 이미 NULL이겠지만...
		pProp->SetDeleteHandler( NULL );

		// 유저가 사용함으로 인해서 RemoveObject/PendFreeFieldProp은 걸려 있지만 아직 onFieldPropDelete가 호출되지 않은 경우에는 아무것도 하지 않음
		// * 아레나 입장 제한 프랍은 유저가 사용할 수 있는 프랍이 아니라서 이럴 일은 없지만...
		if( !pProp->IsInWorld() )
			continue;

		ArcadiaServer::Instance().RemoveObject( pProp );
		StructFieldProp::PendFreeFieldProp( pProp );
	}

	m_vRespawnedArenaBlockerFieldProp.clear();
}

void BattleArenaInstance::_clearItems()
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	struct DroppedItemCollector : public ArRegionFunctor, ArObjectFunctor
	{
		// ArObjectFunctor
		virtual void operator()( ArObject *pObj ) const
		{
			if( !static_cast< GameObject * >( pObj )->IsItem() )
				return;

			vItemList.push_back( static_cast< StructItem * >( pObj ) );
		}

		// ArRegionFunctor
		virtual void operator()( const struct ArRegion * pRegion )
		{
			// DoEachStaticOjbect 안에서 ArObjectFunctor가 RemoveStaticObject를 호출하면
			// 해당 ArRegion의 m_vStaticObject 순회하던 게 박살나므로 포인터만 모음
			pRegion->DoEachStaticObject( *this );
		}

		mutable std::vector< StructItem * >		vItemList;
	} droppedItemCollector;

	ArcadiaServer::Instance().DoEachRegion( m_pArenaBase->nArenaRegionLeft, m_pArenaBase->nArenaRegionTop,
		m_pArenaBase->nArenaRegionRight, m_pArenaBase->nArenaRegionBottom, m_nInstanceNo, droppedItemCollector );

	for( std::vector< StructItem * >::iterator it = droppedItemCollector.vItemList.begin() ; it != droppedItemCollector.vItemList.end() ; ++it )
	{
		StructItem * pItem = (*it);

		// 월드에서 제거(ItemCollector::onProcess에서 삭제 중인 아이템이었을 경우 실패함)
		if( !RemoveItemFromWorld( pItem ) )
			continue;

		StructItem::PendFreeItem( pItem );
	}
}

void BattleArenaInstance::_finishBattle( _ARENA_END_TYPE eEndType )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	if( m_bEnd )
	{
		assert( 0 );
		return;
	}

	// 아직 시작되지 않은 경기(카운트다운 중)의 bStart 판정 때문에 여기서 bStart도 true로 그냥 덮어 줌
	m_bStart = true;

	m_bEnd = true;
	// 실제 경기 종료 처리가 발생한 시점을 기준으로 nEndTime, nForceQuitTime을 갱신(nEndTime은 별 의미는 없고 nForceQuitTime이 의미가 실제로 있음)
	m_nEndTime = GetArTime();
	m_nForceQuitTime = m_nEndTime + GameRule::BATTLE_ARENA_FORCE_QUIT_DELAY;

	// 입장을 하려고 대기중이던 유저가 있다면 취소시킴
	// 경기 종료/이탈 시 해제해줘야 할 버프도 해제(여기서 해야 점수판 보다 디버프로 죽는 일을 방지할 수 있음. 단, 경기 내에 진입하지 않은 유저는 제외)
	// * 지속 효과 말고 스킬 프랍에 의해 입을 수 있는 피해는 피아 판정에 의해 차단될 걸로 예상됨
	{
		for( int nTeamNo = 0 ; nTeamNo < m_pArenaBase->nTeamCount ; ++nTeamNo )
		{
			std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
			if( pPlayerInfoList != NULL )
			{
				for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
				{
					PlayerInfo & pi = (*it);

					if( pi.nPendedEnterTime )
						pi.nPendedEnterTime = 0;
					else if( pi.pPlayer )
						pi.pPlayer->RemoveAllStateByQuittingBattleArena();
				}
			}
		}

		m_vPendedEnterPlayerList.clear();
	}

	// 리젠되어 있던 모든 객체 제거(월드에 드랍되어 있던 아이템은 점수판 보다가 주워 먹을 수도 있으니 여기서 삭제하지 않고 _wrapUpInstance에서 삭제함)
	_clearMonster();
	_clearFieldProps();
	_clearArenaBlockerFieldProps();

	// 경기 중에 오프라인 시간 제한으로 페널티를 받았던 유저 목록을 초기화
	for( std::vector< PlayerUID >::const_iterator it = m_vOfflinePenaltyReceivedPlayerUIDList.begin() ; it != m_vOfflinePenaltyReceivedPlayerUIDList.end() ; ++it )
	{
		BattleArenaManager::Instance().RemoveOfflinePenaltyReceivedPlayer( (*it) );
	}
	m_vOfflinePenaltyReceivedPlayerUIDList.clear();

	// 각 유저에게 걸려있던 버프 모두 제거(해로운 버프 받은 상태로 점수판 보다 죽으면 대략 난감하니까...)
	struct StateRemover : public ArObjectFunctor
	{
		virtual void operator()( ArObject *pObj ) const
		{
			StructPlayer * pPlayer = static_cast< StructPlayer * >( pObj );

			// 아직 경기장 내에 진입하지 않았다면 아무것도 안 함
			if( !pPlayer->IsInBattleArena() )
				return;

			// 지속효과 제거
			pPlayer->RemoveAllStateByQuittingBattleArena();

			if( pPlayer->GetMainSummon() )
				pPlayer->GetMainSummon()->RemoveAllStateByQuittingBattleArena();
			if( pPlayer->GetSubSummon() )
				pPlayer->GetSubSummon()->RemoveAllStateByQuittingBattleArena();
		}
	} foStateRemover;
	_doEachPlayer( foStateRemover );

	// 두 팀만 존재하는 경기에서는 승/패/무승부 3가지만 있지만 여러 팀이 참여하는 걸 고려하면 팀간 순위가 필요해짐
	// 여기 걸리게 되면 승패 보상 지급에 대한 기획을 추가(순위별 보상으로 변경되어야 함)해서 해당 내용대로 구현을 변경해야 함
	if( m_pArenaBase->nTeamCount > 2 )
	{
		assert( 0 );

		// 보상이고 뭐고 없어. 집에 가! 기획자를 욕해!!
		BattleArenaManager::BroadcastBattleArenaFinalScoreMessage( this );
		BattleArenaManager::BroadcastBattleArenaResultWithLog( this, eEndType, m_eRewardType, INVALID_BATTLE_ARENA_TEAM_NO, m_aTeamInfo[ 0 ].nScore, m_aTeamInfo[ 1 ].nScore, m_nEndTime - m_nStartTime );

		// 방 폭파 타이밍을 당겨서 다음 _onProcess에서 바로 방 파괴되면서 모두 쫓아내도록 처리
		m_nForceQuitTime = m_nEndTime;

		return;
	}

	// 종료 스크립트 실행
	BattleArenaRunEventScript( m_pArenaBase, BASET_END_INSTANCE, m_nInstanceNo, NULL );

	// { 보상 지급이 필요 없는 경우나 보상의 배율 조정 필요한 경우 처리
	c_fixed10 fRewardRate = 1;

	// 한 팀의 멤버가 모두 나가 경기가 종료되는 경우에는 보상 없음
	if( eEndType == AET_ONE_TEAM_NO_MEMBER )
	{
		m_eRewardType = ART_NO_REWARD_BY_NO_MEMBER;
		BattleArenaManager::BroadcastBattleArenaResultWithLog( this, eEndType, m_eRewardType, INVALID_BATTLE_ARENA_TEAM_NO, m_aTeamInfo[ 0 ].nScore, m_aTeamInfo[ 1 ].nScore, m_nEndTime - m_nStartTime );
		return;
	}

	// 최소 경기 시간이 아직 경과하지 않은 경우 보상 없음
	if( m_nEndTime - m_nStartTime < m_pArenaBase->nMinPlayDurationForReward )
	{
		m_eRewardType = ART_NO_REWARD_BY_MIN_TIME;
		BattleArenaManager::BroadcastBattleArenaResultWithLog( this, eEndType, m_eRewardType, INVALID_BATTLE_ARENA_TEAM_NO, m_aTeamInfo[ 0 ].nScore, m_aTeamInfo[ 1 ].nScore, m_nEndTime - m_nStartTime );
		return;
	}

	// 한 팀의 인원 체크
	m_eRewardType = ART_FULL_REWARD;
	for( int nTeamNo = 0 ; nTeamNo < m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		size_t nMemberCount = _getTeamMemberCount( nTeamNo );

		// 한 팀의 인원이 0 이라면 보상 없음
		if( !nMemberCount )
		{
			// eEndType != AET_ONE_TEAM_NO_MEMBER 인데 팀이 0 명인 경우가 있다...?
			assert( 0 );
			BattleArenaManager::BroadcastBattleArenaResultWithLog( this, eEndType, ART_NO_REWARD_BY_NO_MEMBER, INVALID_BATTLE_ARENA_TEAM_NO, m_aTeamInfo[ 0 ].nScore, m_aTeamInfo[ 1 ].nScore, m_nEndTime - m_nStartTime );
			return;
		}

		// 한 팀의 인원이 최대 인원의 50% 미만이라면 보상 절반
		if( nMemberCount < ( m_pArenaBase->nMaxMember / m_pArenaBase->nTeamCount / 2 ) )
		{
			m_eRewardType = ART_HALF_REWARD_BY_FEW_MEMBER;
			fRewardRate = 0.5f;
		}
	}

	// 연습 경기라면 보상 없음(근데 MVP 등은 보여주기 위해 보상 배율만 조정함)
	if( m_pArenaBase->IsExerciseGameArena() )
	{
		m_eRewardType = ART_NO_REWARD_BY_EXERCISE;
		fRewardRate = 0;
	}
	// } 보상 지급이 필요 없는 경우나 보상의 배율 조정 필요한 경우 처리

	// 각 팀별 순위 처리(이 부분은 2개 이상의 팀의 순위 가르기에 대해서도 동일하게 동작함)
	// 큰 숫자가 낮은 등수(0: 1등, 1: 2등, ...)
	int anTeamRank[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ] = { 0, };
	bool abHasDrawTeam[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ] = { false, };
	for( int nTeamNo = 1 ; nTeamNo < m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		// 현재 팀 이전 팀과의 스코어 비교 및 순위 처리
		// * 첫 번째 팀은 비교 대상 팀이 없으므로 nTeamNo를 1부터 시작함
		for( int nAnotherTeamNo = 0 ; nAnotherTeamNo < nTeamNo ; ++nAnotherTeamNo )
		{
			// 현재 팀이 이전 팀보다 점수가 높다면 이전 팀의 순위 하나 낮춤(숫자는 커짐)
			if( m_aTeamInfo[ nAnotherTeamNo ].nScore < m_aTeamInfo[ nTeamNo ].nScore )
			{
				++anTeamRank[ nAnotherTeamNo ];
			}
			// 같다면 이전 팀의 순위를 하나 낮추고 현재 팀의 순위도 하나 낮춤(동점 시 순위를 하향 평준화. 1위 보상을 여러 팀이 먹으면 안되니까)
			else if( m_aTeamInfo[ nAnotherTeamNo ].nScore == m_aTeamInfo[ nTeamNo ].nScore  )
			{
				++anTeamRank[ nAnotherTeamNo ];
				++anTeamRank[ nTeamNo ];
				abHasDrawTeam[ nAnotherTeamNo ] = true;
				abHasDrawTeam[ nTeamNo ] = true;
			}
			// 이전 팀이 더 높다면 현재 팀의 등 수를 하나 늘림
			else
			{
				++anTeamRank[ nTeamNo ];
			}
		}
	}

	// MVP 관련 처리 및 승패 보상 AP 지급은 연습 경기에서는 발생하지 않음
	if( !m_pArenaBase->IsExerciseGameArena() )
	{
		// 팀별 점수 확정 및 MVP 선정을 위한 최고 점수 확인(승패 보상 AP 포함 점수로 산정)
		int nHighestPrivateScore = 0;
		int anTeamScore[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ] = { 0, };
		for( int nTeamNo = 0 ; nTeamNo < m_pArenaBase->nTeamCount ; ++nTeamNo )
		{
			// * 2팀 이상이 경기에 참여하게 되면 이 부분도 수정해야 함
			// 승리 팀이면 팀 점수를 승자 점수로 세팅
			if( !anTeamRank[ nTeamNo ] )
				anTeamScore[ nTeamNo ] = m_pArenaBase->nRewardAPWin[ m_eGrade ];
			// 비긴 팀이면 팀 점수를 비긴 점수로 세팅
			else if( abHasDrawTeam[ nTeamNo ] )
				anTeamScore[ nTeamNo ] = m_pArenaBase->nRewardAPDraw[ m_eGrade ];
			// 진 팀~
			else
				anTeamScore[ nTeamNo ] = m_pArenaBase->nRewardAPLose[ m_eGrade ];

			std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
			if( pPlayerInfoList != NULL )
			{
				for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
				{
					PlayerInfo & pi = (*it);

					// 오프라인인 유저는 실제로 승패 보상 AP는 획득 못하지만 MVP 선정 시에는 팀 승패 보상 AP도 포함한 점수로 뽑음
					int nPenaltyCount = ( pi.pPlayer ) ? pi.pPlayer->GetBattleArenaPenaltyCount() : pi.nPenaltyCount;
					// 여기서 pi.nTotalGainAPByResult에 팀 보상AP에 페널티를 반영한 값을 미리 저장하고, 실제 지급 처리 시 MVP, 보상률 적용함
					pi.nTotalGainAPByResult = ( nPenaltyCount >= GameRule::BATTLE_ARENA_HALF_REWARD_PENALTY_COUNT ) ? anTeamScore[ nTeamNo ] / 2 : anTeamScore[ nTeamNo ];
					int nPrivateScore = pi.nTotalGainAPByKill + pi.nTotalGainAPByProp + pi.nTotalGainAPByResult;

					if( nPrivateScore > nHighestPrivateScore )
						nHighestPrivateScore = nPrivateScore;
				}
			}
		}

		// MVP 확정 및 승패 보상 지급, 전적 처리
		for( int nTeamNo = 0 ; nTeamNo < m_pArenaBase->nTeamCount ; ++nTeamNo )
		{
			std::vector< PlayerInfo >* pPlayerInfoList = _getPlayerInfoList( nTeamNo );
			if( pPlayerInfoList != NULL )
			{
				for( std::vector< PlayerInfo >::iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
				{
					PlayerInfo & pi = (*it);

					int nPrivateScore = pi.nTotalGainAPByKill + pi.nTotalGainAPByProp + pi.nTotalGainAPByResult;

					// MVP 여부 확정(온/오프라인 여부와 무관하게 MVP는 결정됨)
					pi.bIsMVP = ( nHighestPrivateScore >= m_pArenaBase->nMinAPForMVP && nHighestPrivateScore == nPrivateScore );

					// 온라인인 유저만 실질적인 승패 보상 지급 및 전적 처리
					if( !pi.pPlayer )
						continue;

					c_fixed10 fMVPRate = ( pi.bIsMVP ) ? GameRule::BATTLE_ARENA_MVP_REWARD_RATE : c_fixed10( 1 );
					// 승패 결과 및 MVP 등에 따른 최종 획득 AP 보관
					pi.nTotalGainAPByResult = fRewardRate * fMVPRate * pi.nTotalGainAPByResult;

					// 지급
					pi.pPlayer->AddBattleArenaPoint( pi.nTotalGainAPByResult );

					// { 전적 처리

					// MVP
					if( pi.bIsMVP )
						pi.pPlayer->AddBattleArenaMVPCount();

					// 승리
					if( anTeamScore[ nTeamNo ] == m_pArenaBase->nRewardAPWin[ m_eGrade ] )
					{
						pi.pPlayer->AddBattleArenaRecord( m_pArenaBase->eType, true );
					}
					// 패배
					else if( anTeamScore[ nTeamNo ] == m_pArenaBase->nRewardAPLose[ m_eGrade ] )
					{
						pi.pPlayer->AddBattleArenaRecord( m_pArenaBase->eType, false );
					}

					// } 전적 처리
				}
			}
		}
	}

	// 최종적인 점수 방송 및 경기 결과 방송
	for( int nTeamNo = 0 ; nTeamNo < m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		if( anTeamRank[ nTeamNo ] )
			continue;

		m_nWinTeamNo = nTeamNo;
		break;
	}
	BattleArenaManager::BroadcastBattleArenaFinalScoreMessage( this );
	BattleArenaManager::BroadcastBattleArenaResultWithLog( this, eEndType, m_eRewardType, m_nWinTeamNo, m_aTeamInfo[ 0 ].nScore, m_aTeamInfo[ 1 ].nScore, m_nEndTime - m_nStartTime );

	// 줄 거 다 줬으니 이제 점수 판이나 보다가 쫓겨나든 알아서 나가든 유저가 알아서 하면 됨
}

void	BattleArenaInstance::_wrapUpInstance( _ARENA_LEAVE_TYPE eLeaveType )
{
	// 락 체크
	assert( m_csBattle.IsLockedByCurrentThread() );

	// 인스턴스 해제 스크립트 실행
	ThreadPlayerHelper TPHelper( NULL );
	BattleArenaRunEventScript( m_pArenaBase, BASET_DESTROY_INSTANCE, m_nInstanceNo, NULL );

	// 유저들 집으로 보내줄 버스 준비
	BattleArenaQuitFunctor qf( eLeaveType, m_pArenaBase, m_nInstanceNo );

	// { 각 팀별 뒷정리

	// 경기장 내부에 있는 유저용 이탈 스크립트 준비(추가로 접속 종료에 의한 것이 아니라는 파라미터를 세팅해 줌)
	std::vector< std::string > vTagReplacement;
	vTagReplacement.push_back( "#@disconnect@#" );
	vTagReplacement.push_back( "0" );

	_removeAllPlayer( eLeaveType, qf );

	// } 각 팀별 뒷정리

	// 경기 인스턴스별 플래그 데이터 초기화
	m_mapInstanceFlag.clear();

	// 입장을 하려고 대기중이던 유저가 있다면 취소시킴
	m_vPendedEnterPlayerList.clear();

	// 리젠되어 있는 몬스터, 프랍, 아이템 등의 처리(몬스터, 프랍은 원래 있으면 안 되는데 혹시 모르니까... 월드에 드랍되어 있던 아이템은 이제 안뇽~)
	_clearMonster();
	_clearFieldProps();
	_clearArenaBlockerFieldProps();
	_clearItems();

	LOG::Log11N4S( LM_BATTLE_ARENA_DESTROY,
		0, 0,
		m_pArenaBase->nID, m_nInstanceNo, m_eGrade,
		_getTeamMemberCount(0), _getTeamMemberCount(1), eLeaveType,
		0, 0, 0,
		"", LOG::STR_NTS, "", LOG::STR_NTS,
		m_pArenaBase->IsExerciseGameArena() ? "EXERCISE" : "ARENA", LOG::STR_NTS,
		"", LOG::STR_NTS );
}

void BattleArenaInstance::onMonsterDelete( StructMonster * pMonster )
{
	// 경기가 종료된 이후면 아무것도 하지 않음
	if( m_bEnd )
		return;

	THREAD_SYNCHRONIZE( m_csBattle );

	// 경기가 종료된 이후면 아무것도 하지 않음(락 걸고 다시 한 번 체크)
	if( m_bEnd )
		return;

	std::vector< StructMonster * >::iterator it = std::find( m_vRespawnedMonster.begin(), m_vRespawnedMonster.end(), pMonster );
	// 이미 삭제 처리 된 몬스터면 리젠 진행시키지 않음(경기 진행 중인데 사망 처리가 2번 들어왔거나 리젠만 되고 vRespawnedMonster에 추가되지 않았거나... -_ -?)
	if( it == m_vRespawnedMonster.end() )
	{
		assert( 0 );
		return;
	}
	m_vRespawnedMonster.erase( it );

	// 시간제로 소환되었던 몬스터가 시간 제한에 의해 사라지는 경우에는 리젠 예약 처리를 하지 않음
	if( pMonster->IsLifeTimeOver() )
		return;

	int nRespawnID = pMonster->GetInstanceRespawnID();
	if( nRespawnID )
	{
		std::vector< const BATTLE_ARENA_MONSTER_RESPAWN * >::const_iterator itRespawn;
		for( itRespawn = m_pArenaBase->vMonsterRespawnList.begin() ; itRespawn != m_pArenaBase->vMonsterRespawnList.end() ; ++itRespawn )
		{
			const BATTLE_ARENA_MONSTER_RESPAWN * pRespawn = (*itRespawn);
			if( pRespawn->nID != nRespawnID )
				continue;

			// 리젠 간격이 0 인 경우에는 즉시 리젠이 아니라 한 번 죽으면 다시 리젠시키지 않음
			if( !pRespawn->nPeriod )
				break;

			m_vPendedMonsterRespawn.push_back( PendedMonsterRespawnInfo( pRespawn, GetArTime() + pRespawn->nPeriod ) );

			break;
		}

		assert( itRespawn != m_pArenaBase->vMonsterRespawnList.end() );
	}
}

void BattleArenaInstance::onFieldPropDelete( StructFieldProp * pProp )
{
	// 경기가 종료된 이후면 아무것도 하지 않음
	if( m_bEnd )
		return;

	THREAD_SYNCHRONIZE( m_csBattle );

	// 경기가 종료된 이후면 아무것도 하지 않음(락 걸고 다시 한 번 체크)
	if( m_bEnd )
		return;

	// 프랍 번호를 가지고 있는 프랍이 없어지는 경우라면 반드시 프랍 상태가 먼저 PS_NOT_EXIST로 변경되어 있어야 함
	const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * pRespawn = static_cast< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >( pProp->GetRespawnInfo() );
	if( pRespawn->nPropIndexInArena )
	{
		if( _getPropState( pRespawn->nPropIndexInArena ) != PS_NOT_EXIST )
		{
			assert( 0 );

			_setPropState( pRespawn->nPropIndexInArena, PS_NOT_EXIST );
			BattleArenaManager::BroadcastBattleArenaBattleStatusMessage( this );
		}
	}

	std::vector< StructFieldProp * >::iterator it = std::find( m_vRespawnedFieldProp.begin(), m_vRespawnedFieldProp.end(), pProp );
	// 이미 삭제 처리 된 프랍이면 리젠 진행시키지 않음(경기 진행 중인데 삭제 처리가 2번 들어왔거나 리젠만 되고 vRespawnedFieldProp에 추가되지 않았거나... -_ -?)
	if( it == m_vRespawnedFieldProp.end() )
	{
		assert( 0 );
		return;
	}
	m_vRespawnedFieldProp.erase( it );

	// 리젠 간격이 0 인 프랍은 한 번 사라지면 다시 리젠시키지 않음
	if( !pProp->GetFieldPropBase()->nRegenTime )
		return;

	m_vPendedFieldPropRespawn.push_back( PendedFieldPropRespawnInfo( pRespawn, GetArTime() + pProp->GetFieldPropBase()->nRegenTime ) );
}
