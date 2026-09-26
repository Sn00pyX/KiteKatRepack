
#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "BattleArenaWithWaitQueue.h"
#include "PartyManager.h"
#include "DB_Commands.h"
#include "BattleArenaManager.h"


BattleArenaWithWaitQueue::BattleArenaWithWaitQueue( const BattleArenaBaseServer* pArenaBase, ThreadSafeIntMap* pPartyToArenaMap )
	: BattleArena( pArenaBase, pPartyToArenaMap )
{
}

BattleArenaWithWaitQueue::~BattleArenaWithWaitQueue()
{
}

_BATTLE_GRADE	BattleArenaWithWaitQueue::GetPlayableLongestWaitingGrade()	const
{
	_BATTLE_GRADE nPlayableLongestWaitingGrade = BG_INVALID;
	AR_TIME tEarliestWaitStart = INFINITE_TIME;
	for( int nGrade = 0; nGrade < MAX_BATTLE_GRADE; ++nGrade )
	{
		const std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >* pWaitQueue = _getWaitQueue( static_cast< _BATTLE_GRADE >( nGrade ) );
		if( pWaitQueue == NULL )
			continue;

		if( pWaitQueue->empty() )
			continue;

		AR_TIME tWaitStart = pWaitQueue->front().second;
		if( tWaitStart < tEarliestWaitStart && pWaitQueue->size() >= m_pArenaBase->nMinMember )
		{
			tEarliestWaitStart = tWaitStart;
			nPlayableLongestWaitingGrade = static_cast< _BATTLE_GRADE >( nGrade );
		}
	}

	return nPlayableLongestWaitingGrade;
}

unsigned short	BattleArenaWithWaitQueue::_joinWaitQueue( StructPlayer* pPlayer, _BATTLE_GRADE eGrade )
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	assert( eGrade != BG_INVALID );

	if( _isFull() )
		return RESULT_LIMIT_MAX;

	int nCurrentPartyID = pPlayer->GetPartyID();
	if( nCurrentPartyID && PartyManager::GetInstance().GetPartyType( nCurrentPartyID ) != PartyManager::TYPE_NORMAL_PARTY )
		return RESULT_NOT_ACTABLE_IN_SIEGE_OR_RAID;

	// 현재 진행 중인 경기의 체크
	for( unsigned int nInstanceNo = 1; nInstanceNo <= MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA; ++nInstanceNo )
	{
		BattleArenaInstance* pBattleInstance = _getBattleInstance( nInstanceNo );
		if( pBattleInstance == NULL )
		{
			continue;
		}

		THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

		if( pBattleInstance->m_eGrade == eGrade && !pBattleInstance->m_bStart && !pBattleInstance->m_bEnd )
		{
			// 방에 참여될 때는 가명 사용 여부 설정
			pPlayer->SetUseAlias( true );

			unsigned short nResult = pBattleInstance->_addPlayer( pPlayer );

			// 최근에 만들어진 방에 참여가 성공했다면 대기열에 넣을 필요가 없으므로 여기서 성공으로 리턴
			if( nResult == RESULT_SUCCESS )
				return RESULT_SUCCESS;

			// 여기 도달한 경우는 최근에 만들어진 방에 인원 초과였던 경우가 대부분이며,
			// 다른 원인으로 참여 실패했다고 해도 기존의 경기에는 참여 안 하고 대기열로 가면 별 문제 없으므로
			// 별다른 처리 없이 그냥 넘어가서 대기열에 현재 유저를 추가함

			// 설정됐던 가명 사용 여부 해제
			pPlayer->SetUseAlias( false );
		}
	}

	std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >* pWaitQueue = _getWaitQueue( eGrade );
	if( pWaitQueue == NULL )
	{
		return RESULT_LIMIT_MAX;
	}

	pWaitQueue->push_back( PlayerWaitInfo( pPlayer, GetArTime() ) );

	// 대기열에서 대기중인 유저들에게 대기 중인 유저 수 갱신 방송
	BattleArenaManager::BroadcastWaitUserCountMessage( this, eGrade );

	LOG::Log11N4S( LM_BATTLE_ARENA_JOIN_WAIT_QUEUE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(),
		m_pArenaBase->nID, 0, eGrade, pWaitQueue->size(),
		0, 0, 0, 0, 0,
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
		"", LOG::STR_NTS, "", LOG::STR_NTS );

	// 대기열에 유저를 추가했다면 RESULT_SUCCESS가 아니라 RESULT_PENDING을 리턴함
	// * 이 함수의 리턴값으로 대기열에 들어간 건지 최근에 시작되었던 방에 들어간 건지를 구분하기 위함... 리턴값 상수 이름으로는 다소 모호하지만;;
	return RESULT_PENDING;
}

unsigned short	BattleArenaWithWaitQueue::_leaveWaitQueue( StructPlayer* pPlayer, _ARENA_LEAVE_TYPE eLeaveType )
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	// 대기열에 등록하던 시점과 대기열에서 이탈하는 시점 사이에 캐릭터의 레벨이 변경되었을 가능성이 있기 때문에
	// 여기서 GetBattleArenaGrade( pPlayer ) 에 의해 얻어진 아레나 등급만을 믿으면 안 됨.
	// 하지만 대기 시점의 등급과 동일할 가능성이 높기 때문에 가장 먼저 현재 레벨에 해당하는 등급을 검사함.
	_BATTLE_GRADE eGrade = GetBattleArenaGrade( pPlayer->GetLevel() );
	if( eGrade == BG_INVALID )
	{
		// 레벨 다운 BG_INVALID 가 올수 있다.
		eGrade = BG_ROOKIE;
	}

	_BATTLE_GRADE aeGrades[ MAX_BATTLE_GRADE ] = { eGrade, ( eGrade == BG_ROOKIE ) ? BG_GROW : BG_ROOKIE, ( eGrade == BG_MAJOR ) ? BG_GROW : BG_MAJOR };

	for( int i = 0; i < MAX_BATTLE_GRADE; ++i )
	{
		std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >* pWaitQueue = _getWaitQueue( aeGrades[ i ] );
		if( pWaitQueue != NULL )
		{
			for( std::vector< PlayerWaitInfo >::iterator it = pWaitQueue->begin(); it != pWaitQueue->end(); ++it )
			{
				StructPlayer* pTemp = (*it).first;
				if( pTemp != pPlayer )
					continue;

				BattleArenaManager::SendBattleArenaLeaveMessage( pPlayer, eLeaveType );

				pWaitQueue->erase( it );

				// 대기열에서 대기중인 유저들에게 대기 중인 유저 수 갱신 방송
				BattleArenaManager::BroadcastWaitUserCountMessage( this, aeGrades[ i ] );

				pPlayer->SetUseAlias( false );
				pPlayer->SetBattleArenaID( 0 );

				LOG::Log11N4S( LM_BATTLE_ARENA_LEAVE_WAIT_QUEUE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(),
					m_pArenaBase->nID, 0, aeGrades[ i ], pWaitQueue->size(),
					0, 0, 0, 0, 0,
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
					"", LOG::STR_NTS, "", LOG::STR_NTS );

				return RESULT_SUCCESS;
			}
		}
	}

	return RESULT_NOT_EXIST;
}

unsigned short	BattleArenaWithWaitQueue::_leaveWaitQueue( PlayerUID nPlayerUID, _ARENA_LEAVE_TYPE eLeaveType )
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	_BATTLE_GRADE aeGrades[ MAX_BATTLE_GRADE ] = { BG_ROOKIE, BG_GROW, BG_MAJOR };

	for( int i = 0; i < MAX_BATTLE_GRADE; ++i )
	{
		std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >* pWaitQueue = _getWaitQueue( aeGrades[ i ] );
		if( pWaitQueue != NULL )
		{
			for( std::vector< PlayerWaitInfo >::iterator it = pWaitQueue->begin(); it != pWaitQueue->end(); ++it )
			{
				StructPlayer* pPlayer = (*it).first;
				if( pPlayer->GetPlayerUID() != nPlayerUID )
					continue;

				BattleArenaManager::SendBattleArenaLeaveMessage( pPlayer, eLeaveType );

				pWaitQueue->erase( it );

				// 대기열에서 대기중인 유저들에게 대기 중인 유저 수 갱신 방송
				BattleArenaManager::BroadcastWaitUserCountMessage( this, aeGrades[ i ] );

				pPlayer->SetUseAlias( false );
				pPlayer->SetBattleArenaID( 0 );

				LOG::Log11N4S( LM_BATTLE_ARENA_LEAVE_WAIT_QUEUE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(),
					m_pArenaBase->nID, 0, aeGrades[ i ], pWaitQueue->size(),
					0, 0, 0, 0, 0,
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
					"", LOG::STR_NTS, "", LOG::STR_NTS );

				return RESULT_SUCCESS;
			}
		}
	}

	return RESULT_NOT_EXIST;
}

unsigned char	BattleArenaWithWaitQueue::_tryStartNewBattle()
{
	// 인스턴스를 더 생성할 수 없다면 아무 체크도 하지 않고 넘어감
	if( _isFull() )
		return INVALID_BATTLE_ARENA_INSTANCE_NO;

	// 새로운 경기가 생성되었을 때 부여될 인스턴스 번호
	// * 새로운 경기 시작을 시도할 때마다 필요하면 새로 할당하고, 이전에 할당했던 번호가 쓰이지 않고 남아 있으면 다음 등급 체크에서 그대로 이용함
	//   그리고 등급별 새 경기 시작 처리가 모두 종료된 후에는 마지막으로 할당했던 번호가 쓰이지 않고 남아 있으면 해제함
	_BATTLE_GRADE eGrade = GetPlayableLongestWaitingGrade();
	if( eGrade == BG_INVALID )
	{
		return INVALID_BATTLE_ARENA_INSTANCE_NO;
	}

	unsigned char nNewInstanceNo = _allocNewInstanceNo();
	if( nNewInstanceNo == INVALID_BATTLE_ARENA_INSTANCE_NO )
	{
		// 현재 체크 등급 이하의 등급에서 방이 새로 생성되면서 풀이 된 경우에만 여기로 들어와야 정상이고 그 외에는 로직 버그일 가능성이 높음
		// * 루프 도는 사이에 어떤 경기가 종료되어서 _isFull() == false인 상황으로 바뀌는 경우는 csArena 때문에 없음
		assert( _isFull() );
		// 한 번 _isFull() == true가 되면 나머지 등급에 대해서는 방을 추가로 생성할 수 없으므로 break;
		return INVALID_BATTLE_ARENA_INSTANCE_NO;
	}

	unsigned short nErrorCode = _createNewBattle( eGrade, nNewInstanceNo );
	if( nErrorCode == RESULT_SUCCESS )
	{
		return nNewInstanceNo;
	}
	else
	{
		// 로그 남겨야 할만한 오류 코드들은 로그 출력 및 성공 시 추가 뒷처리
		switch( nErrorCode )
		{
			// 로그 남길 리턴값들
		case RESULT_LIMIT_MAX:
			// 대기열 풀이면 방 만드는 시도 자체 이전에 대기열에 유저가 등록되는 액션 자체를 차단해야 하지 않나?
			break;
		case RESULT_ALREADY_EXIST:
			// _addPlayer에서 기존에 가입되어 있던 파티에서 탈퇴시키는 처리 실패
			break;
		case RESULT_UNKNOWN:
			// 뭐여;;
			break;

			// 일반적인 실패 케이스
		case RESULT_LIMIT_MIN:
		case RESULT_NOT_ENOUGH_BULLET:
			break;

			// 새로운 경기 생성 성공
		case RESULT_SUCCESS:
			break;

		default:
			// 예상 못 한 리턴값을 넘기는 곳을 찾아서 경우에 따라 로그를 남기거나 아무것도 안하는 그룹 case 들에 추가할 것
			assert( 0 );
			break;
		}

		if( nNewInstanceNo != INVALID_BATTLE_ARENA_INSTANCE_NO )
			_freeInstanceNo( nNewInstanceNo );
	}

	return INVALID_BATTLE_ARENA_INSTANCE_NO;
}

unsigned short	BattleArenaWithWaitQueue::_createNewBattle( _BATTLE_GRADE eGrade, unsigned char nNewInstanceNo )
{
	// 락 체크
	assert( m_csArena.IsLockedByCurrentThread() );

	assert( IsValidBattleArenaInstanceNo( nNewInstanceNo ) );

	std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >* pWaitQueue = _getWaitQueue( eGrade );
	if( pWaitQueue == NULL )
	{
		return RESULT_LIMIT_MIN;
	}

	int nWaitingMemberCount = (int)pWaitQueue->size();
	if( nWaitingMemberCount < m_pArenaBase->nMinMember )
	{
		return RESULT_LIMIT_MIN;
	}

	// 대기열에서 기다리는 유저들 중 실제 참여 가능한 유저 목록 선별(룰 상으로는 모두가 항상 가능해야 하지만 어떤 버그로 인한 증상 등)
	StructPlayer* apJoinablePlayer[ GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT ] = { NULL, };
	int nJoinPlayerCount = 0;
	{
		std::vector< PlayerWaitInfo >::iterator it;
		for( it = pWaitQueue->begin(); it != pWaitQueue->end(); /* 루프에서 ++it 처리 */ )
		{
			StructPlayer* pPlayer = (*it).first;

			// 대기열에 대기했을 때는 가명 사용 여부를 세팅하지 않은 상태이므로 여기에서 pPlayer->IsUsingAlias() == true 이면
			// 어디선가 잘못해서 대기열에 있는 유저에게 SetUseAlias( true )를 호출한 경우임
			assert( !pPlayer->IsUsingAlias() );

			if( pPlayer->GetLogoutTime() != 0 )
			{
				pPlayer->SetBattleArenaID( 0 );
				// 아래의 두 줄은 어차피 게임 내로 진입했던 건 아니니 굳이 할 필요 없지만 혹시나 해서...
				assert( !pPlayer->GetBattleArenaInstanceNo() );
				pPlayer->SetBattleArenaInstanceNo( 0 );

				// 로그아웃 대기 인사람은 삭제한다.
				it = pWaitQueue->erase( it );

				LOG::Log11N4S( LM_BATTLE_ARENA_LEAVE_WAIT_QUEUE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(),
					m_pArenaBase->nID, 0, eGrade, pWaitQueue->size(),
					0, 0, 0, 0, 0,
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
					"", LOG::STR_NTS, "", LOG::STR_NTS );

				continue;
			}

			int nPartyID = pPlayer->GetPartyID();
			// 경기에 진입시키지 않고 대기열에서도 이탈시켜야 하는 유저
			if( nPartyID && PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_NORMAL_PARTY )
			{
				BattleArenaManager::SendBattleArenaLeaveMessage( pPlayer, ALT_PARTICIPATING_SPECIAL_PARTY );

				// 가명 사용 여부는 세팅된 적이 없으므로 해제할 필요도 없음

				// 유저에게 세팅된 BattleArenaID를 0으로 변경해야 하는데, 지역 락을 여기서 걸어줄 수는 없음.
				// 우선은 csArena에 의존해서 유저의 BattleArenaID도 보호한다고 생각하고 처리함.
				pPlayer->SetBattleArenaID( 0 );
				// 아래의 두 줄은 어차피 게임 내로 진입했던 건 아니니 굳이 할 필요 없지만 혹시나 해서...
				assert( !pPlayer->GetBattleArenaInstanceNo() );
				pPlayer->SetBattleArenaInstanceNo( 0 );

				it = pWaitQueue->erase( it );

				LOG::Log11N4S( LM_BATTLE_ARENA_LEAVE_WAIT_QUEUE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(),
					m_pArenaBase->nID, 0, eGrade, pWaitQueue->size(),
					0, 0, 0, 0, 0,
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
					"", LOG::STR_NTS, "", LOG::STR_NTS );

				continue;
			}

			apJoinablePlayer[ nJoinPlayerCount ] = pPlayer;
			++nJoinPlayerCount;

			++it;

			if( nJoinPlayerCount == m_pArenaBase->nMaxMember )
				break;
		}

		// 선별하고 나서 보니 하나의 경기를 시작하기 위해 필요한 최소 인원보다 적다면
		// 새로운 경기 시작은 없었던 일로 하고 다른 유저가 대기열에 들어오길 대기
		if( nJoinPlayerCount < m_pArenaBase->nMinMember )
		{
			// 대기열에서 대기중인 유저들에게 대기 중인 유저 수 갱신 방송
			BattleArenaManager::BroadcastWaitUserCountMessage( this, eGrade );

			return RESULT_LIMIT_MIN;
		}

		// 선별된 유저들로 새로운 경기를 시작할 수 있으므로, 해당 유저들은 대기열에서 제거
		pWaitQueue->erase( pWaitQueue->begin(), it );
	}

	// 랜덤으로 팀 분배
	StructPlayer* apTeamMember[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ][ GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM ] = { 0, };
	{
		char anSlotNo[ GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT ] = { 0, };

		for( int i = 0; i < nJoinPlayerCount; ++i )
		{
			anSlotNo[ i ] = i;
		}

		// 각 유저마다 랜덤 슬롯에 배치
		for( int i = 0; i < nJoinPlayerCount; ++i )
		{
			int nSlotIndex = XRandom( i, nJoinPlayerCount - 1 );
			std::swap( anSlotNo[ i ], anSlotNo[ nSlotIndex ] );
			apTeamMember[ anSlotNo[ i ] % m_pArenaBase->nTeamCount ][ anSlotNo[ i ] / m_pArenaBase->nTeamCount ] = apJoinablePlayer[ i ];
		}
	}

	// 인원 불균형 시 특정 팀이 항상 더 많은 멤버를 가지고 시작하지 않도록 팀의 순서를 바꾸기 처리
	// * 이를 위해 이하의 코드 블럭에서는 팀 번호를 접근할 때에 반드시 anTeamIndex[ i ] 를 거치도록 해야 함
	int anTeamIndex[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ] = { 0, };
	for( int i = 0; i < m_pArenaBase->nTeamCount; ++i )
	{
		anTeamIndex[ i ] = i;
	}

	if( nJoinPlayerCount % m_pArenaBase->nTeamCount )
	{
		for( int i = 0; i < m_pArenaBase->nTeamCount; ++i )
		{
			// 50% 확률로 다음 팀과 팀 순서 뒤집기
			if( XRandom( 1, 100 ) <= 50 )
			{
				std::swap( anTeamIndex[ i ], anTeamIndex[ ( i + 1 ) % m_pArenaBase->nTeamCount ] );
			}
		}
	}

	// 대기열에서 대기중인 유저들에게 대기 중인 유저 수 갱신 방송
	BattleArenaManager::BroadcastWaitUserCountMessage( this, eGrade );

	BattleArenaInstance*& pBattleInstance = m_apBattleList[ nNewInstanceNo ];
	// 처음 해당 방 번호에 경기가 생성되는 경우 BattleArenaInstance 객체를 할당(이후는 같은 방 번호의 경우 기존에 할당받았던 객체를 재사용함)
	if( pBattleInstance == NULL )
	{
		pBattleInstance = new BattleArenaInstance( m_pArenaBase,
												m_pPartyToArenaMap,
												&m_partyToInstanceMap,
												nNewInstanceNo );
	}

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );
	pBattleInstance->Init( eGrade, GetArTime() + m_pArenaBase->nCountdownDuration );

	// 각 팀별 파티 생성 및 참가
	for( int nTeamIndex = 0; nTeamIndex < m_pArenaBase->nTeamCount; ++nTeamIndex )
	{
		int nTeamNo = anTeamIndex[ nTeamIndex ];

		unsigned short nResult = RESULT_SUCCESS;

		for( int nPlayerIndex = 0; nPlayerIndex < GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM; ++nPlayerIndex )
		{
			StructPlayer* pPlayer = apTeamMember[ nTeamNo ][ nPlayerIndex ];
			if( pPlayer == NULL )
			{
				continue;
			}

			// 가명 사용 여부 세팅
			pPlayer->SetUseAlias( true );

			LOG::Log11N4S( LM_BATTLE_ARENA_LEAVE_WAIT_QUEUE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(),
				m_pArenaBase->nID, 0, eGrade, pWaitQueue->size(),
				0, 0, 0, 0, 0,
				pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
				"", LOG::STR_NTS, "", LOG::STR_NTS );

			nResult = pBattleInstance->_addPlayer( pPlayer, nTeamIndex );

			// 성공적으로 경기에 추가되었으면 다음 유저 처리(참여된 유저들에게 서로 방송을 하는 것은 첨여 처리 완료 이후에 함)
			if( nResult == RESULT_SUCCESS )
			{
				apTeamMember[ nTeamNo ][ nPlayerIndex ] = NULL;
				continue;
			}

			// 현재 유저는 방 참여에 실패했고, 해당 유저만 이탈되면 되는 경우
			if( nResult == RESULT_NOT_ACTABLE_IN_SIEGE_OR_RAID ||
				nResult == RESULT_TARGET_IN_SIEGE_OR_RAID ||
				nResult == RESULT_LIMIT_MAX ||
				nResult == RESULT_INVALID_TEXT ||
				nResult == RESULT_ALREADY_EXIST )
			{
				// 페널티 줘야 하는 경우
				if( nResult == RESULT_NOT_ACTABLE_IN_SIEGE_OR_RAID ||
					nResult == RESULT_TARGET_IN_SIEGE_OR_RAID )
				{
					pPlayer->IncBattleArenaPenalty();
					pPlayer->DBQuery( new DB_UpdateBattleArenaPenalty( pPlayer ) );

					BattleArenaManager::SendBattleArenaPenaltyMessage( pPlayer );
				}

				// 참여 유저 목록에서 해당 유저 제거
				apTeamMember[ nTeamNo ][ nPlayerIndex ] = NULL;

				// 자신에게 이탈되었다는 것을 알려 줌
				BattleArenaManager::SendBattleArenaLeaveMessage( pPlayer, ALT_FAILED_TO_JOIN_BATTLE );

				// 유저가 이탈했다는 것을 나머지 참가자에게 방송해야 함(PartyManager::DoEachMember를 쓸 수 없는게, 아직 파티 편성이 다 안 된 상태라서 -_ -;)
				// * 방송 순서는 관계 없으므로 anTeamIndex를 거치지 않고 바로 nTeamNo로 각 팀을 순회
				// * RESULT_INVALID_TEXT 인 경우는 가명 정보가 정상적으로 처리되지 않은 경우이기 때문에 방송을 할 수 없음(_addPlayer 자체가 실패했으니 방송 안해도 무방함)
				if( nResult != RESULT_INVALID_TEXT )
				{
					BattleArenaInstance::PlayerInfo null_pi( pPlayer );
					BattleArenaManager::BroadcastBattleArenaLeaveBattleMessageWithLog( pBattleInstance,
						&null_pi, pPlayer,
						pBattleInstance->m_nWinTeamNo,
						INVALID_BATTLE_ARENA_TEAM_NO,
						pPlayer->GetAlias(),
						ALT_FAILED_TO_JOIN_BATTLE );
				}

				// 이탈된 유저에게 세팅된 정보 제거
				pPlayer->SetUseAlias( false );
				pPlayer->SetBattleArenaID( 0 );

				// 유저 한 명이 이탈했지만 경기 시작에는 문제는 없으므로 nResult를 RESULT_SUCCESS로 다시 바꿔 줌
				nResult = RESULT_SUCCESS;
			}
			// 유저 참가 처리가 실패했는데, 해당 유저만 쫓아내서 될 상황이 아니라면 방 생성 자체를 실패 처리
			else if( nResult == RESULT_UNKNOWN )
				break;
		}

		// 방 생성이 불가능해지는 문제는 없었지만 한 팀이 모두 이탈된 경우인지 체크
		if( nResult == RESULT_SUCCESS )
		{
			if( pBattleInstance->_getTeamMemberCount( nTeamNo ) == 0 )
			{
				nResult = RESULT_NOT_ENOUGH_BULLET;
			}
		}

		// 중간에 복구가 불가능한 형태로 유저 참여가 실패한 경우에는 나머지 유저들에게는 미안하지만 방을 파토냄
		if( nResult != RESULT_SUCCESS )
		{
			BattleArenaNULLFunctor null_functor;
			pBattleInstance->_removeAllPlayer( ALT_FAILED_TO_START_BATTLE, null_functor );

			// 참여 중이던 모든 유저들에게 경기 시작 실패로 대기열에서 이탈되었음을 알려줌
			for( int _nTeamNo = 0; _nTeamNo < m_pArenaBase->nTeamCount; ++_nTeamNo )
			{
				// apTeamMember 중간중간에 NULL 값이 들어있을 수 있지만,
				// 불의의 사고로 이탈된 유저가 이미 있었다면 해당 칸이 NULL이 된 이후에 그 뒤로도 다른 유저가 있을 수 있으므로
				// _nPlayerIndex 순회는 반드시 [ 0 ~ GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM ) 의 범위를 다 해야 함
				for( int _nPlayerIndex = 0; _nPlayerIndex < GameRule::BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM; ++_nPlayerIndex )
				{
					StructPlayer* pPlayer = apTeamMember[ _nTeamNo ][ _nPlayerIndex ];
					if( pPlayer == NULL )
						continue;

					BattleArenaManager::SendBattleArenaLeaveMessage( pPlayer, ALT_FAILED_TO_START_BATTLE );

					pPlayer->SetUseAlias( false );
					pPlayer->SetBattleArenaID( 0 );

					LOG::Log11N4S( LM_BATTLE_ARENA_LEAVE_WAIT_QUEUE, pPlayer->GetAccountID(), pPlayer->GetPlayerUID(),
						m_pArenaBase->nID, 0, eGrade, pWaitQueue->size(),
						0, 0, 0, 0, 0,
						pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS,
						"", LOG::STR_NTS, "", LOG::STR_NTS );
				}
			}

			return nResult;
		}
	}

	pBattleInstance->_initInstance();

	return RESULT_SUCCESS;
}

AR_TIME	BattleArenaWithWaitQueue::_getWaitStartTime( _BATTLE_GRADE eGrade )	const
{
	AR_TIME tWaitStart = 0;
	const std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >* pWaitQueue = _getWaitQueue( eGrade );
	if( pWaitQueue != NULL )
	{
		if( pWaitQueue->empty() == false )
		{
			tWaitStart = pWaitQueue->front().second;
		}
	}

	return tWaitStart;
}

std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >*	BattleArenaWithWaitQueue::_getWaitQueue( _BATTLE_GRADE eGrade )
{
	assert( IsValidBattleArenaGrade( eGrade ) );

	std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >* pWaitQueue = NULL;
	if( IsValidBattleArenaGrade( eGrade ) )
	{
		pWaitQueue = &m_vWaitQueue[eGrade];
	}

	return pWaitQueue;
}

const std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >*	BattleArenaWithWaitQueue::_getWaitQueue( _BATTLE_GRADE eGrade )	const
{
	assert( IsValidBattleArenaGrade( eGrade ) );

	const std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >* pWaitQueue = NULL;
	if( IsValidBattleArenaGrade( eGrade ) )
	{
		pWaitQueue = &m_vWaitQueue[eGrade];
	}

	return pWaitQueue;
}
