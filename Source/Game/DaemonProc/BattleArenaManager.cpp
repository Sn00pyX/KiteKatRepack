
#include "BattleArenaManager.h"
#include "BattleArenaForExercise.h"
#include "BattleArenaWithWaitQueue.h"
#include "DB_Commands.h"
#include "PartyManager.h"
#include "ThreadPlayerHelper.h"


BattleArenaManager::BattleArenaManager()
	: m_bInit( false )
{
}

BattleArenaManager::~BattleArenaManager()
{
	for( std::vector< BattleArena* >::const_iterator it = m_vArenaList.begin() ; it != m_vArenaList.end() ; ++it )
	{
		BattleArena* pArena = (*it);
		if( pArena != NULL )
		{
			delete pArena;
		}
	}

	m_vArenaList.clear();
}

unsigned short	BattleArenaManager::RegisterArena( const BattleArenaBaseServer* pArenaBase )
{
	// Init 호출 전에만 RegisterArena 함수가 호출되어야 함
	assert( !m_bInit );

	std::vector< BattleArena* >::const_iterator it = m_vArenaList.begin();
	for( ; it != m_vArenaList.end(); ++it )
	{
		BattleArena* pArena = (*it);
		if( pArenaBase->nID == pArena->m_pArenaBase->nID )
		{
			// 리로딩?
			assert( 0 );
			return RESULT_ALREADY_EXIST;
		}
	}

	if( !pArenaBase->IsExerciseGameArena() )
	{
		m_vArenaList.push_back( new BattleArenaWithWaitQueue( pArenaBase, &m_partyToArenaMap ) );
	}
	else
	{
		m_vArenaList.push_back( new BattleArenaForExercise( pArenaBase, &m_partyToArenaMap ) );
	}

	return RESULT_SUCCESS;
}

bool	BattleArenaManager::Init()
{
	assert( !m_bInit );
	if( m_bInit )
	{
		return false;
	}

	m_bInit = true;

	// 1초에 한 번 onProcess로 충분한지는 잘 모르겠지만...
	ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_HIGH );
	return true;
}

bool	BattleArenaManager::DeInit()
{
	assert( m_bInit );
	if( !m_bInit )
	{
		return false;
	}

	m_bInit = false;

	ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_IDLE );
	return true;
}

void	BattleArenaManager::RaiseUpdatePriority()
{
	assert( m_bInit );
	if( !m_bInit )
		return;

	ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_HIGHEST );
}

void	BattleArenaManager::DownUpdatePriority()
{
	if( !m_bInit )
		return;

	ArcadiaServer::Instance().SetObjectPriority( this, UPDATE_PRIORITY_HIGH );
}

unsigned short	BattleArenaManager::JoinWaitQueue( StructPlayer* pPlayer, int nArenaID )
{
	unsigned short nResult = checkJoinable( pPlayer, false );
	if( nResult != RESULT_SUCCESS )
		return nResult;

	_BATTLE_GRADE eGrade = GetBattleArenaGrade( pPlayer->GetLevel() );
	if( eGrade == BG_INVALID )
		return RESULT_NOT_ENOUGH_LEVEL;

	BattleArena* pArena = getArena( nArenaID );
	if( pArena == NULL )
		return RESULT_INVALID_ARGUMENT;

	// 예외적으로 루키 등급 유저는 16:16 경기에 참여할 수 없음
	if( eGrade == BG_ROOKIE && pArena->m_pArenaBase->nMaxMember == 32 )
		return RESULT_NOT_ENOUGH_LEVEL;

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	// 대기열에 들어갈 때 일반 파티에 가입된 상태일 수도 있기 때문에 pPlayer->SetUseAlias를 여기서 호출하지 않고
	// _joinWaitQueue 안에서 상황에 따라 호출함

	// 현재 아레나에 참여 시도를 하긴 하니까 일단 BattleArenaID 세팅(참여 실패할 경우엔 다시 0 으로 돌려줘야 함)
	pPlayer->SetBattleArenaID( pArena->m_pArenaBase->nID );

	nResult = pArena->_joinWaitQueue( pPlayer, eGrade );

	// 최근에 시작되었던 방에 바로 진입해버렸다면 여기서 추가 처리 중지하고 바로 리턴
	if( nResult == RESULT_SUCCESS )
	{
		BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( pPlayer->GetBattleArenaInstanceNo() );

		assert( pBattleInstance );
		THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

		// 경기에 대한 정보 방송
		SendBattleArenaBattleInfoMessage( pPlayer, pBattleInstance );
		SendBattleArenaBattleStatusMessage( pPlayer, pBattleInstance );

		// 현재 스코어 정보도 보내 줌
		SendBattleArenaScoreMessage( pPlayer, pBattleInstance );

		// 다른 유저들에게 현재 유저가 새로 참여했음을 알려 줌
		BroadcastBattleArenaJoinBattleMessage( pBattleInstance, pPlayer );

		return RESULT_SUCCESS;
	}

	// 최근에 시작되었던 경기에 바로 진입한 것도 아니고 대기열에 추가되지도 못했다면 바로 리턴(대기열 등록 실패)
	if( nResult != RESULT_PENDING )
	{
		// 미리 세팅했던 ArenaID 초기화
		pPlayer->SetBattleArenaID( 0 );
		// 가명 사용 여부 설정도 초기화
		pPlayer->SetUseAlias( false );

		// 실패했을 때는 Caller 측에서 TS_SC_RESULT 패킷으로 클라한테 알려주고,
		// 성공했을 때에는 여기서 상황따라 서로 다른 패킷으로 알려주는 게 일관성 없지만...;;
		// * RESULT 패킷 송신과 그 외에 부가 동작에 따른 패킷 방송 순서를 맞출 방법이 없음 ㅠㅜ;
		//   게다가 성공 시 전달해야 할 정보들이 csArena를 풀기 전에 얻어야 할 내용들이 대부분임.
		return nResult;
	}

	// 일단 대기열에 추가는 성공했으니 클라에게 알려 줌
	SendBattleArenaJoinQueueMessage( pPlayer, pArena, eGrade );

	return RESULT_SUCCESS;
}

unsigned short	BattleArenaManager::QuickJoin( StructPlayer* pPlayer )
{
	unsigned short nResult = checkJoinable( pPlayer, false );
	if( nResult != RESULT_SUCCESS )
		return nResult;

	_BATTLE_GRADE eGrade = GetBattleArenaGrade( pPlayer->GetLevel() );
	if( eGrade == BG_INVALID )
		return RESULT_NOT_ENOUGH_LEVEL;

	// 빠른 참여는 기본적으로 현재 시작되어 있는 경기들에 대해 먼저 참여 체크
	do
	{
		unsigned int nMaxBattleInstancePriority = 0;
		int nCandidateArenaID = 0;
		unsigned char nCandidateInstanceNo = 0;

		for( std::vector< BattleArena* >::const_iterator it = m_vArenaList.begin() ; it != m_vArenaList.end() ; ++it )
		{
			const BattleArena* pArena = (*it);

			// 연습 경기는 빠른 참여 대상 제외
			if( pArena->m_pArenaBase->IsExerciseGameArena() )
				continue;

			// 루키 등급의 유저는 16:16 경기에 참여 불가
			if( eGrade == BG_ROOKIE && pArena->m_pArenaBase->nMaxMember == 32 )
				continue;

			THREAD_SYNCHRONIZE( pArena->m_csArena );

			// 현재 진행 중인 경기의 체크
			for( unsigned int nInstanceNo = 1; nInstanceNo <= MAX_BATTLE_ARENA_INSTANCE_NO_PER_ARENA; ++nInstanceNo )
			{
				const BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );
				if( pBattleInstance == NULL )
				{
					continue;
				}

				THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

				if( pBattleInstance->m_eGrade != eGrade )
					continue;

				unsigned int nPriority = pBattleInstance->_getQuickJoinPriority();
				if( nPriority > nMaxBattleInstancePriority )
				{
					nMaxBattleInstancePriority = nPriority;
					nCandidateArenaID = pBattleInstance->m_pArenaBase->nID;
					nCandidateInstanceNo = nInstanceNo;
				}
			}
		}

		// 최우선순위의 방이 없었다면 진행 중인 경기에 대한 참여 시도는 실패로 패스
		// 검색된 방이 있었다면 참여를 시도
		// * 왜 시도냐믄 csArena/csBattle을 풀었다가 다시 걸기 때문에 그 사이에 참여할 수 없는 상태가 될 가능성이 있기 때문
		// * csArena/csBattle을 풀었다 다시 걸기 때문에 모든 방들의 우선순위가 참여 시점에는 뒤바뀌어 있을 수도 있는데
		//   그런 차이는 일단 무시함. 그거 다 챙기려면 전 경기에 동시에 락 걸고 검사하고 참여 처리까지 끝내고 락 풀어야되는데
		//   성능상으로 보나 개념상으로 보나 잠깐 사이에 생기는 점수 차이 방지하자고 하는 짓 치곤 미친 짓이다 -_ -;
		if( !nCandidateArenaID )
			break;

		BattleArena* pArena = getArena( nCandidateArenaID );

		// pArena에 대한 NULL 체크를 안 했다. 서버 종료될 때 말고는 NULL이 될 수가 없다;;
		THREAD_SYNCHRONIZE( pArena->m_csArena );

		BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nCandidateInstanceNo );

		// 검색됐던 방이 사라졌다는 건 있을 수 없는데(이것도 pArena처럼 서버 종료될 떄 말고는 NULL이 될 수가 없음)
		// 딴데서도 다 하던 체크니까 그냥 해 줌... -_ -;;
		if( !pBattleInstance )
			// 만에 하나라도 방이 사라졌다면 위에서 2, 3, 4 순위 등을 미리 뽑아놓지는 않는 관계로 다시 체크함...
			// 위에서 2, 3, 4 순위 뽑아놓는다고 해도 결국은 재수없으면 미리 뽑아 둔 방 모두 참여 불가 상태가 될
			// 가능성 자체를 무시할 수는 없기 때문에 다 소용없다고 판단. 참여 못하는 경우가 정말 드물거라 가정하고
			// 비효율적인 거 알지만 일단 그냥 continue 해서 do 문부터 다시 함.
			break;

		// 빠른 입장의 경우는 별 의미는 없지만 pPlayer->SetBattleArenaID 를 호출하지 않고 _addPlayer를 호출함

		THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

		if( pBattleInstance->m_eGrade == eGrade && pBattleInstance->_isQuickJoinAble() )
		{
			// 가명 사용 여부 설정
			pPlayer->SetUseAlias( true );

			unsigned short nResult = pBattleInstance->_addPlayer( pPlayer );

			// 참여에 실패했으면 앞뒤 없이 닥치고 다른 방 찾기
			if( nResult != RESULT_SUCCESS )
			{
				// 가명 사용 여부 설정 리셋
				pPlayer->SetUseAlias( false );
				break;
			}

			// 참여에 성공했으니 늦게나마 ArenaID 세팅
			pPlayer->SetBattleArenaID( nCandidateArenaID );

			// 그리고 경기에 대한 정보 방송
			SendBattleArenaBattleInfoMessage( pPlayer, pBattleInstance );
			SendBattleArenaBattleStatusMessage( pPlayer, pBattleInstance );

			// 현재 스코어 정보도 보내 줌
			SendBattleArenaScoreMessage( pPlayer, pBattleInstance );

			// 다른 유저들에게 현재 유저가 새로 참여했음을 알려 줌
			BroadcastBattleArenaJoinBattleMessage( pBattleInstance, pPlayer );

			// 그리고 QuickJoin은 성공으로 종료 'ㅠ '
			return RESULT_SUCCESS;
		}
	} while( false );

	// 진행 중이던 경기에 참여를 못 한 경우에는 대기열들 중 참여할 대기열을 검색
	{
		BattleArenaWithWaitQueue* pLongestWaitingArena = NULL;
		AR_TIME tEarliestWaitStart = INFINITE_TIME;

		for( std::vector< BattleArena* >::iterator it = m_vArenaList.begin(); it != m_vArenaList.end(); ++it )
		{
			BattleArena* pArena = (*it);

			// 연습 경기는 빠른 참여 대상 제외
			if( pArena->m_pArenaBase->IsExerciseGameArena() )
				continue;

			// 루키 등급의 유저는 16:16 경기에 참여 불가
			if( eGrade == BG_ROOKIE && pArena->m_pArenaBase->nMaxMember == 32 )
				continue;

			THREAD_SYNCHRONIZE( pArena->m_csArena );

			BattleArenaWithWaitQueue* pCurrentArena = static_cast< BattleArenaWithWaitQueue* >( pArena );
			AR_TIME tWaitStart = pCurrentArena->_getWaitStartTime( eGrade );
			if( tWaitStart == 0 )
				continue;

			if( tWaitStart < tEarliestWaitStart )
			{
				tEarliestWaitStart = tWaitStart;
				pLongestWaitingArena = pCurrentArena;
			}
		}

		// 참여할 대기열이 선택되지 않았다면 빠른 참여는 실패 'ㅠ ';
		if( !pLongestWaitingArena )
			return RESULT_NOT_EXIST;

		THREAD_SYNCHRONIZE( pLongestWaitingArena->m_csArena );

		// 현재 아레나에 참여 시도를 하긴 하니까 일단 BattleArenaID 세팅(참여 실패할 경우엔 다시 0 으로 돌려줘야 함)
		// * 빠른 입장이어도 대기열 대기는 ArenaID를 먼저 세팅해 줌
		pPlayer->SetBattleArenaID( pLongestWaitingArena->m_pArenaBase->nID );

		nResult = pLongestWaitingArena->_joinWaitQueue( pPlayer, eGrade );

		// 여기서 최근에 시작된 경기에 바로 진입하는 경우도 있을 수 있나... '- ';?
		if( nResult == RESULT_SUCCESS )
		{
			BattleArenaInstance* pBattleInstance = pLongestWaitingArena->_getBattleInstance( pPlayer->GetBattleArenaInstanceNo() );

			THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

			// 경기에 대한 정보 방송
			SendBattleArenaBattleInfoMessage( pPlayer, pBattleInstance );
			SendBattleArenaBattleStatusMessage( pPlayer, pBattleInstance );

			// 현재 스코어 정보도 보내 줌
			SendBattleArenaScoreMessage( pPlayer, pBattleInstance );

			// 다른 유저들에게 현재 유저가 새로 참여했음을 알려 줌
			BroadcastBattleArenaJoinBattleMessage( pBattleInstance, pPlayer );

			return RESULT_SUCCESS;
		}
		// 대기열에 대기 성공했다면 정보 보내주고 성공으로 종료
		else if( nResult == RESULT_PENDING )
		{
			SendBattleArenaJoinQueueMessage( pPlayer, pLongestWaitingArena, eGrade );

			return RESULT_SUCCESS;
		}
		// 최근에 시작되었던 경기에 바로 진입한 것도 아니고 대기열에 추가되지도 못했다면 바로 리턴(대기열 등록 실패)
		else
		{
			// 미리 세팅했던 ArenaID 초기화
			pPlayer->SetBattleArenaID( 0 );

			// 가명 사용 여부 리셋
			pPlayer->SetUseAlias( false );
		}
	}

	// 여기까지 온 건 대기열 선택까지 됐는데 대기열 참여(_joinWaitQueue)조차 실패한 경우...
	// JoinWaitQueue와 마찬가지로 실패했을 때는 Caller 측에서 TS_SC_RESULT 패킷으로 클라한테 알려주고,
	// 성공했을 때에는 여기서 상황따라 서로 다른 패킷으로 알려주는 게 일관성 없지만...;;
	// * RESULT 패킷 송신과 그 외에 부가 동작에 따른 패킷 방송 순서를 맞출 방법이 없음 ㅠㅜ;
	//   게다가 성공 시 전달해야 할 정보들이 csArena를 풀기 전에 얻어야 할 내용들이 대부분임.

	return nResult;
}

unsigned short	BattleArenaManager::QuitGame( StructPlayer* pPlayer, bool bByKick, bool bAvoidPenalty, _ARENA_LEAVE_TYPE eLeaveType )
{
	int nArenaID = pPlayer->GetBattleArenaID();
	if( !nArenaID )
		return RESULT_NOT_EXIST;

	BattleArena* pArena = getArena( nArenaID );
	if( !pArena )
		return RESULT_INVALID_ARGUMENT;

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();

	if( nInstanceNo == 0 )
	{
		return pArena->_leaveWaitQueue( pPlayer, eLeaveType );
	}

	BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 읭?
	if( !pBattleInstance )
	{
		assert( 0 );
		return RESULT_INVALID_ARGUMENT;
	}

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	eLeaveType = ( bByKick ) ? ALT_EXERCISE_PARTY_KICK : ALT_USER_REQUEST;
	unsigned short nResult = pBattleInstance->_removePlayer( pPlayer, eLeaveType );

	// 경기 이탈에 실패했다면 오류 코드 리턴
	if( nResult != RESULT_SUCCESS && nResult != RESULT_NOT_ENOUGH_BULLET )
		return nResult;

	// 경기 참여 상태에서 직접 이탈했으므로 페널티 부여(이미 종료된 경기나 연습 경기라면 페널티 없음)
	if( !pBattleInstance->m_bEnd && !pBattleInstance->m_pArenaBase->IsExerciseGameArena() && !bAvoidPenalty )
	{
		pPlayer->IncBattleArenaPenalty();
		pPlayer->DBQuery( new DB_UpdateBattleArenaPenalty( pPlayer ) );

		SendBattleArenaPenaltyMessage( pPlayer );
	}

	BattleArenaQuitFunctor qf( eLeaveType, pBattleInstance->m_pArenaBase, nInstanceNo );
	qf( pPlayer );

	// nResult가 RESULT_NOT_ENOUGH_BULLET 인 경우라면 다음 번 BattleArenaManager::onProcess 에서 경기 종료 처리가 될 거니까 지금은 별 처리 안 함

	return RESULT_SUCCESS;
}

unsigned short	BattleArenaManager::QuitGame( int nPartyID, PlayerUID nPlayerUID, bool bByKick, _ARENA_LEAVE_TYPE eLeaveType )
{
	if( !nPartyID || !PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
		return RESULT_ACCESS_DENIED;

	int nArenaID = m_partyToArenaMap.Get( nPartyID );

	BattleArena* pArena = getArena( nArenaID );
	if( !pArena )
	{
		return RESULT_NOT_EXIST;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	int nInstanceNo = pArena->m_partyToInstanceMap.Get( nPartyID );

	// 방이 없거나 이미 해산됐다면...
	if( nInstanceNo == 0 )
	{
		return pArena->_leaveWaitQueue( nPlayerUID, eLeaveType );
	}

	BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 이건 또 좀 이상하지만 여튼 체크...
	if( pBattleInstance == NULL )
		// 또 즐
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	// 체크하려는 파티의 팀 번호 얻기
	int nTeamNo = pBattleInstance->_getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		// 파티 ID 기준으로 아레나를 찾았는데 해당 파티가 소속된 팀은 없는 경우
		// m_partyToArenaMap, m_partyToInstanceMap 이 락을 따로 쓰지 않아서 생길 가능성이 있긴 함(csBattle 걸기 직전에 파티 해산)
		return RESULT_NOT_EXIST;
	}

	BattleArenaInstance::PlayerInfo* pPlayerInfo = pBattleInstance->_getPlayerInfo( nPlayerUID, nTeamNo );
	if( !pPlayerInfo )
		return RESULT_NOT_EXIST;

	// 온라인인 유저는 이 함수가 아니라 QuitGame( StructPlayer* pPlayer, bool bByKick ) 함수로 이탈 처리를 시도해야 함
	if( pPlayerInfo->pPlayer )
		return RESULT_INVALID_ARGUMENT;

	unsigned short nResult = pBattleInstance->_removePlayer( nPlayerUID, (bByKick) ? ALT_EXERCISE_PARTY_KICK : ALT_DISCONNECT );

	// 경기 이탈에 실패했다면 오류 코드 리턴
	if( nResult != RESULT_SUCCESS && nResult != RESULT_NOT_ENOUGH_BULLET )
		return nResult;

	return RESULT_SUCCESS;
}

unsigned short	BattleArenaManager::EnterArenaWhileCountdown( StructPlayer* pPlayer )
{
	int nArenaID = pPlayer->GetBattleArenaID();
	if( !nArenaID )
		return RESULT_NOT_EXIST;

	BattleArena* pArena = getArena( nArenaID );
	if( !pArena )
		return RESULT_INVALID_ARGUMENT;

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();

	if( !nInstanceNo )
		return RESULT_NOT_EXIST;

	BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return RESULT_INVALID_ARGUMENT;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_pendEnterWhileCountdown( pPlayer );
}

unsigned short	BattleArenaManager::OnPlayerDead( StructPlayer* pPlayer, StructPlayer* pKiller )
{
	// 킬러가 없으면 아무것도 안 함
	if( !pKiller )
		return RESULT_SUCCESS;

	int nArenaID = pPlayer->GetBattleArenaID();
	if( !nArenaID )
		return RESULT_NOT_EXIST;

	BattleArena* pArena = getArena( nArenaID );
	if( !pArena )
		return RESULT_INVALID_ARGUMENT;

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();

	if( !nInstanceNo )
		return RESULT_NOT_EXIST;

	BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 읭?
	if( !pBattleInstance )
	{
		assert( 0 );
		return RESULT_INVALID_ARGUMENT;
	}

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_onPlayerDead( pPlayer, pKiller );
}

unsigned short BattleArenaManager::OnDisconnect( StructPlayer* pPlayer )
{
	int nArenaID = pPlayer->GetBattleArenaID();
	if( !nArenaID )
		return RESULT_NOT_EXIST;

	BattleArena* pArena = getArena( nArenaID );
	// 으잉?
	if( !pArena )
	{
		assert( 0 );
		return RESULT_INVALID_ARGUMENT;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();

	// 대기열에서 대기중이었던 유저면 대기열 이탈 처리만 해 줌
	if( nInstanceNo == 0 )
	{
		return pArena->_leaveWaitQueue( pPlayer, ALT_DISCONNECT );
	}

	// 경기 내부에 있었으면 경기에서 오프라인 처리
	BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 읭?
	if( !pBattleInstance )
	{
		assert( 0 );
		return RESULT_INVALID_ARGUMENT;
	}

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_disconnectPlayer( pPlayer );
}

unsigned short	BattleArenaManager::OnReconnect( StructPlayer* pPlayer, ArPosition* pPosStart, unsigned char* pnLayer )
{
	// 재접속 시에는 배틀 아레나 관련 정보가 세팅이 되어 있지 않기 때문에 파티 ID로 다 찾아야 함
	int nPartyID = pPlayer->GetPartyID();

	if( !nPartyID || !PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
		return RESULT_NOT_EXIST;

	// smp_login_character 직후에 smp_set_party가 실행되어 해당 캐릭터의 party_id를 0으로 바꿔주게 되면
	// 로그인 시점(지금 여기 롸잇 나우)에는 pPlayer->GetPartyID() != 0 이지만 실제 멤버는 아닌 상태가
	// 발생할 수도 있으므로 여기서 실제 멤버인지 한 번 체크하고 아니면 그냥 패스
	if( !PartyManager::GetInstance().IsMember( nPartyID, pPlayer->GetPlayerUID() ) )
		// 여기서 그냥 return 해버리고나면 SendLoginResult 함수 안에서 PartyManager::onLogin을 호출해서 알아서 캐릭터의 파티ID가 리셋됨
		return RESULT_NOT_EXIST;

	int nArenaID = m_partyToArenaMap.Get( nPartyID );
	if( !nArenaID )
		return RESULT_NOT_EXIST;

	BattleArena* pArena = getArena( nArenaID );
	if( !pArena )
	{
		// m_partyToArenaMap에 nArenaID가 없던 것도 아니고 아예 잘못된 nArenaID 가 들어있었다는 얘기가 되므로 있어서는 안되는 상황
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	int nInstanceNo = pArena->m_partyToInstanceMap.Get( nPartyID );

	// 방이 없거나 이미 해산됐다면...
	if( !nInstanceNo )
		// 즐
		return RESULT_NOT_EXIST;

	BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 이건 또 좀 이상하지만 여튼 체크...
	if( !pBattleInstance )
		// 또 즐
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	// 경기가 끝난 후에 재접속하는 경우는 그냥 경기에서 이탈시킴
	if( pBattleInstance->m_bEnd )
	{
		// 필요한 경우에는 여기서 ALT_GAME_OVER 대신 ALT_DISCONNECT를 쓰는 것도 개념상 가능할 듯
		pBattleInstance->_removePlayer( pPlayer, ALT_GAME_OVER );

		// 이외의 처리는 여기서 직접 해줘도 되지만 어차피 DB_Login::readCharacterInfo 안에서 OnReconnect를 호출한 시점에는
		// 캐릭터의 정보가 모두 다 로드된 상태가 아닌지라 완전히 처리는 못 해줌(Ex. 버프 삭제)
		// 그래서 그냥 재접속 실패로 리턴해버려서 이탈 관련 나머지 처리는 readCharacterInfo에서 알아서 하게 해야 함
		return RESULT_NOT_EXIST;
	}

	unsigned short nResult = pBattleInstance->_reconnectPlayer( pPlayer, pPosStart, pnLayer );

	// 뭬야? 방도 있고 아레나 파티 소속이기도 한데 경기에 재참여는 실패;?
	if( nResult != RESULT_SUCCESS )
	{
		assert( 0 );

		// 무슨 상황인지도 불명확하니 일단 경기에서 해당 유저 제거
		unsigned short nRemoveResult = pBattleInstance->_removePlayer( pPlayer, ALT_UNKNOWN );

		// 캐릭터의 PlayerInfo가 아예 없는 경우
		if( nRemoveResult != RESULT_SUCCESS && nRemoveResult != RESULT_NOT_ENOUGH_BULLET )
		{
			assert( nRemoveResult == RESULT_NOT_EXIST );

			pBattleInstance->_deleteBattleArenaPartyPlayer( nPartyID, pPlayer );
			return RESULT_UNKNOWN;
		}

		// 여기선 BattleArenaQuitFunctor 처리 안해주고 어차피 nResult != RESULT_SUCCESS 이기 때문에
		// 경기장 밖으로 쫓겨나는 처리가 DB_Login 쪽에서 이루어 짐
	}

	return nResult;
}

void BattleArenaManager::OnReconnectPostProc( StructPlayer* pPlayer )
{
	int nArenaID = pPlayer->GetBattleArenaID();

	if( !nArenaID )
		return;

	BattleArena* pArena = getArena( nArenaID );

	// 으잉?
	if( !pArena )
	{
		assert( 0 );
		return;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );
	if( !pBattleInstance )
	{
		assert( 0 );
		return;
	}

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	// 경기가 끝난 후에 재접속하는 경우는 그냥 경기에서 이탈시킴
	if( pBattleInstance->m_bEnd )
	{
		// 필요한 경우에는 여기서 ALT_GAME_OVER 대신 ALT_DISCONNECT를 쓰는 것도 개념상 가능할 듯
		pBattleInstance->_removePlayer( pPlayer, ALT_GAME_OVER );

		// 이외의 처리는 여기서 직접 해줘도 되지만 어차피 DB_Login::readCharacterInfo 안에서 OnReconnect를 호출한 시점에는
		// 캐릭터의 정보가 모두 다 로드된 상태가 아닌지라 완전히 처리는 못 해줌(Ex. 버프 삭제)
		// 그래서 그냥 재접속 실패로 리턴해버려서 이탈 관련 나머지 처리는 readCharacterInfo에서 알아서 하게 해야 함
		return;
	}

	pBattleInstance->_reconnectPlayerPostProc( pPlayer );
}

size_t BattleArenaManager::DoEachPlayerInBattleInstance( int nArenaID, unsigned char nInstanceNo, ArObjectFunctor& fo )
{
	if( !nArenaID || !IsValidBattleArenaInstanceNo( nInstanceNo ) )
	{
		assert( 0 );
		return 0;
	}

	BattleArena* pArena = getArena( nArenaID );
	if( !pArena )
	{
		assert( 0 );
		return RESULT_INVALID_ARGUMENT;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_doEachPlayer( fo );
}

size_t BattleArenaManager::DoEachPlayerInBattleTeam( int nArenaID, unsigned char nInstanceNo, int nTeamNo, ArObjectFunctor& fo )
{
	if( !nArenaID || !IsValidBattleArenaInstanceNo( nInstanceNo ) )
	{
		assert( 0 );
		return 0;
	}

	BattleArena* pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return RESULT_INVALID_ARGUMENT;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_doEachPlayerInTeam( nTeamNo, fo );
}

unsigned short BattleArenaManager::ActivateBattleInstanceProp( StructPlayer* pPlayer, int nPropIndex )
{
	int nArenaID = pPlayer->GetBattleArenaID();
	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	if( !nArenaID || !IsValidBattleArenaInstanceNo( nInstanceNo ) || nPropIndex < 1 || nPropIndex > 9 )
	{
		assert( 0 );
		return RESULT_INVALID_ARGUMENT;
	}

	BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return RESULT_INVALID_ARGUMENT;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	BattleArenaInstance* pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_activateProp( pPlayer, nPropIndex );
}

unsigned short BattleArenaManager::RequestAbsenceCheck( StructPlayer* pPlayer, AR_HANDLE hCheckTarget )
{
	int nArenaID = pPlayer->GetBattleArenaID();
	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	if( !nArenaID || !IsValidBattleArenaInstanceNo( nInstanceNo ) )
	{
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_requestAbsenceCheck( pPlayer, hCheckTarget );
}

unsigned short BattleArenaManager::AnswerAbsenceCheck( StructPlayer* pPlayer, bool bSuccess )
{
	int nArenaID = pPlayer->GetBattleArenaID();
	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	if( !nArenaID || !IsValidBattleArenaInstanceNo( nInstanceNo ) )
	{
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	BattleArena* pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_answerAbsenceCheck( pPlayer, bSuccess );
}

unsigned short BattleArenaManager::CreateExerciseGame( StructPlayer * pPlayer )
{
	// 지역락 걸지 않고 호출해야 함
	assert( !ArcadiaServer::Instance().IsLocked( pPlayer ) );

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pPlayer ) );

		unsigned short nResult = checkJoinable( pPlayer, true );

		if( nResult != RESULT_SUCCESS )
			return nResult;
	}

	// 연습 경기의 경기 타입은 '슬로터 8:8'로 고정되어 있는 미리 지정된 아레나 ID 값을 사용함
	BattleArena* pArena = getArena( GameRule::BATTLE_ARENA_EXERCISE_GAME_ARENA_ID );

	// 고정 ID에 해당하는 연습 경기용 아레나 ID가 없는 경우...!?
	if( !pArena )
	{
		assert( 0 );
		return RESULT_ACCESS_DENIED;
	}

	// 새로운 경기가 생성되었을 때 부여될 인스턴스 번호
	unsigned char nNewInstanceNo = INVALID_BATTLE_ARENA_INSTANCE_NO;

	{
		THREAD_SYNCHRONIZE( pArena->m_csArena );

		if( pArena->_isFull() )
			return RESULT_LIMIT_MAX;

		nNewInstanceNo = pArena->_allocNewInstanceNo();
		if( nNewInstanceNo == INVALID_BATTLE_ARENA_INSTANCE_NO )
		{
			// 위에서는 _isFull() == false 였는데 새 방 번호 할당은 실패;? 정상은 아니지만 일단 방을 생성할 수 없다는 걸로 끝냄
			return RESULT_LIMIT_MAX;
		}

		// 경기 생성(연습 경기는 등급 구분이 없이 기본 등급(BG_ROOKIE = 0)만 처리함)
		unsigned short nErrorCode = pArena->_createNewBattle( BG_ROOKIE, nNewInstanceNo );

		/* 로그 남겨야 할만한 오류 코드들은 로그 출력 및 성공 시 추가 뒷처리
		// 현재는 따로 하는 일이 없어서 그냥 주석 처리
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
		로그 남겨야 할만한 오류 코드들은 로그 출력 및 성공 시 추가 뒷처리 */

		// 플레이어 추가에 실패했다면 나머지 처리 중지
		if( nErrorCode != RESULT_SUCCESS )
		{
			pArena->_freeInstanceNo( nNewInstanceNo );
			return nErrorCode;
		}

		// 여기서 csArena가 풀리게 하는 이유는 스크립트 실행을 위한 지역락을 방금 할당된 레이어에만 걸기 위해서
		// 단, 여기서 csArena를 풀었다가 지역락 걸고 다시 csArena를 걸기 전에 BattleArenaManager::onProcess -> BattleArenaInstance::_onProcess가
		// 실행되면 방에 아무도 없기 때문에 한쪽 팀 전원 이탈로 방이 파괴될 수 있는데,
		// 연습 경기의 경우에만 ReadyState가 없으면 _onProcess가 아무런 일도 하지 않게 되어있고, 이로 인해 방이 파괴되지 않음
		// 다만 최초 1명이 경기에 참여된 이후에는 방이 파괴될 조건 체크(연습 경기는 사실상 첫 번째 팀이 0명인 경우만)가 이루어져야 하므로 
		// ReadyState를 0으로라도 세팅해줘야 함
	}

	{
		ARCADIA_LOCK( pArena->LockWholeArena( nNewInstanceNo ) );

		THREAD_SYNCHRONIZE( pArena->m_csArena );

		// csArena를 풀었다가 다시 거는 사이에 방이 파괴된 경우. 본래 파괴되지 않도록 보호되어야 하는데 왜 파괴되는지 확인해서 수정해야 함.
		if( !pArena->_isOccupiedInstanceNo( nNewInstanceNo ) )
		{
			assert( 0 );
			return RESULT_UNKNOWN;
		}

		BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nNewInstanceNo );

		if( !pBattleInstance )
		{
			// 위에서 _isOccupiedInstanceNo 체크를 했기 때문에 여기 올 수가 없는데;?
			assert( 0 );
			pArena->_freeInstanceNo( nNewInstanceNo );
			return RESULT_LIMIT_MAX;
		}

		{
			THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

			// 어딘가 다른쪽에서 이 경기의 레디 플래그를 손댄 경우
			if( !pBattleInstance->_getFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE ).empty() )
			{
				// ... 고쳐야지;;
				assert( 0 );
				pArena->_freeInstanceNo( nNewInstanceNo );
				return RESULT_UNKNOWN;
			}

			// 레디 플래그 세팅~(_setReadyExerciseGame 함수를 사용해서 복잡한 처리를 할 필요도 없이 레디 안 된 상태인 0으로 세팅)
			pBattleInstance->_setFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE, "0" );

			// 경기가 시작됐다면 스크립트 실행
			// 지역락은 이미 걸려있고(경기를 생성한 유저 위치에 걸려있지만) csArena, csBattle도 걸린 상태
			BattleArenaRunEventScript( pArena->m_pArenaBase, BASET_CREATE_INSTANCE, nNewInstanceNo, NULL );

			// 플레이어가 참여 중인 아레나의 ID 값을 세팅해 줌(BattleArenaInstance::_addPlayer 호출을 준비하는 정도의 의미)
			pPlayer->SetBattleArenaID( GameRule::BATTLE_ARENA_EXERCISE_GAME_ARENA_ID );

			// 연습 경기는 가명이 사용되지 않음(다른데서 SetUseAlias( true )가 호출된 채로 여기에 들어오면 안 됨)
			assert( !pPlayer->IsUsingAlias() );

			// 경기를 생성한 유저를 첫 번째 팀의 첫 번째 유저로 추가(파티장이 됨)
			unsigned short nErrorCode = pBattleInstance->_addPlayer( pPlayer, 0 );

			if( nErrorCode != RESULT_SUCCESS )
			{
				// 이것도 보통은 일어날 수 없는 상황인데...;?
				assert( 0 );
				pPlayer->SetBattleArenaID( 0 );

				// 일단 BASET_CREATE_INSTANCE 실행했으니 BASET_DESTROY_INSTANCE도 짝으로...
				BattleArenaRunEventScript( pArena->m_pArenaBase, BASET_DESTROY_INSTANCE, nNewInstanceNo, NULL );

				pArena->_freeInstanceNo( nNewInstanceNo );
				return nErrorCode;
			}

			// 참여자에게 팀별 상세 정보 및 경기 시작 카운트(시작 시점: 시간값이 무조건 INFINITE_TIME 이겠지만...;;) 방송
			SendBattleArenaBattleInfoMessage( pPlayer, pBattleInstance );
		}

		// 경기 생성 및 첫 번째 팀의 파티장도 추가됐으니 연습 경기 생성 처리는 완료
	}

	// 경기가 진행 중일 때는 BattleArenaManager의 update priority를 최상으로 높여 줌
	// * UPDATE_PRIORITY_HIGH 정도여도 다 괜찮은데 빙고/킬 스크립트 예약/실행 딜레이가 너무 심해질 소지가 있어서;;
	RaiseUpdatePriority();

	return RESULT_SUCCESS;
}

unsigned short BattleArenaManager::JoinExerciseGameOpponentLeader( StructPlayer * pPlayer, int nInviterPartyID )
{
	// 배틀 아레나 연습 경기용 파티에만 참여 가능(시작된 이후에는 파티 타입이 TYPE_BATTLE_ARENA_TEAM 으로 변경되므로 이 체크만으로도 어느정도 경기 상태를 포함해서 걸러짐)
	if( !nInviterPartyID || PartyManager::GetInstance().GetPartyType( nInviterPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		return RESULT_NOT_EXIST;

	int nArenaID = m_partyToArenaMap.Get( nInviterPartyID );

	if( !nArenaID )
		return RESULT_NOT_EXIST;

	BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		// m_partyToArenaMap에 nArenaID가 없던 것도 아니고 아예 잘못된 nArenaID 가 들어있었다는 얘기가 되므로 있어서는 안되는 상황
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	// 이하는 2개의 팀만 참여하는 경기 타입에 종속적인 코드를 포함하고 있음
	if( pArena->m_pArenaBase->nTeamCount != 2 )
	{
		assert( 0 );
		return RESULT_DB_ERROR;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	if( pPlayer->GetBattleArenaID() != 0 )
	{
		// 에러 코드가 모호하다.
		return RESULT_NOT_EXIST;
	}

	int nInstanceNo = pArena->m_partyToInstanceMap.Get( nInviterPartyID );

	// 방이 없거나 이미 해산됐다면...
	if( !nInstanceNo )
		// 즐
		return RESULT_NOT_EXIST;

	BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 이건 또 좀 이상하지만 여튼 체크...
	if( !pBattleInstance )
		// 또 즐
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	// 경기가 시작된 이후나 끝난 상태라면 참여 불가
	if( pBattleInstance->m_bStart || pBattleInstance->m_bEnd )
		return RESULT_NOT_ACTABLE;

	// 초대 팀 상대편 팀에 아무도 없어야 함
	int nInviterTeamNo = pBattleInstance->_getTeamNo( nInviterPartyID );
	if( nInviterTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		// 응;? 초대 걸어놓고 나가서 초대자 파티가 해산된 경우라고 봐야 하나;? 어쨌든 정상 여부를 확인할 방법이 없으니 실패 처리.
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	// 초대한 팀 상대 팀에 파티가 결성되어 있다면 실패(두 명 이상에게 초대를 보내고 한 쪽이 먼저 수락하고 다른 쪽이 나중에 수락한 경우)
	int nJoinTeamNo = 1 - nInviterTeamNo;
	if( pBattleInstance->m_anPartyID[ nJoinTeamNo ][ 0 ] )
		return RESULT_ALREADY_EXIST;

	// _addPlayer 호출 전에 ArenaID 세팅
	pPlayer->SetBattleArenaID( pArena->m_pArenaBase->nID );

	// 연습 경기에 참여할 때는 가명 기능을 사용할 수 없음(다른데에서 SetUseAlias( true )가 호출됐던 채로 여기로 들어오면 안 됨)
	assert( !pPlayer->IsUsingAlias() );

	// 해당 팀에 유저 추가(파티 결성 등도 함께 처리)
	unsigned short nResult = pBattleInstance->_addPlayer( pPlayer, nJoinTeamNo );

	if( nResult != RESULT_SUCCESS )
	{
		pPlayer->SetBattleArenaID( 0 );
		return nResult;
	}

	// 그리고 경기에 대한 정보 방송
	SendBattleArenaBattleInfoMessage( pPlayer, pBattleInstance );

	// 다른 유저들에게 현재 유저가 새로 참여했음을 알려 줌
	BroadcastBattleArenaJoinBattleMessage( pBattleInstance, pPlayer );

	return RESULT_SUCCESS;
}

unsigned short BattleArenaManager::JoinExerciseGameMember( StructPlayer * pPlayer, int nPartyID )
{
	// 배틀 아레나 연습 경기용 파티에만 참여 가능(시작된 이후에는 파티 타입이 TYPE_BATTLE_ARENA_TEAM 으로 변경되므로 이 체크만으로도 어느정도 경기 상태를 포함해서 걸러짐)
	if( !nPartyID || PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		return RESULT_NOT_EXIST;

	int nArenaID = m_partyToArenaMap.Get( nPartyID );

	if( !nArenaID )
		return RESULT_NOT_EXIST;

	assert( nArenaID == GameRule::BATTLE_ARENA_EXERCISE_GAME_ARENA_ID );
	BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		// m_partyToArenaMap에 nArenaID가 없던 것도 아니고 아예 잘못된 nArenaID 가 들어있었다는 얘기가 되므로 있어서는 안되는 상황
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	if( pPlayer->GetBattleArenaID() != 0 )
	{
		return RESULT_NOT_EXIST;
	}

	unsigned char nInstanceNo = pArena->m_partyToInstanceMap.Get( nPartyID );
	if( !nInstanceNo )
		// 즐
		return RESULT_NOT_EXIST;

	BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 이건 또 좀 이상하지만 여튼 체크...
	if( !pBattleInstance )
		// 또 즐
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	// 경기가 시작된 이후나 끝난 상태라면 참여 불가
	if( pBattleInstance->m_bStart || pBattleInstance->m_bEnd )
		return RESULT_NOT_ACTABLE;

	// 참여할 팀 번호 확인
	int nTeamNo = pBattleInstance->_getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		// 응;? 초대 걸어놓고 나가서 초대자 파티가 해산된 경우라고 봐야 하나;? 어쨌든 정상 여부를 확인할 방법이 없으니 실패 처리.
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	// 참여할 팀의 레디 상태 체크(초대한 후에 레디되었으면 그 이후에 수락 누른 유저는 참여 실패)
	std::string strReady = pBattleInstance->_getFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE );
	if( atoi( strReady.c_str() ) & ( 1 << nTeamNo ) )
		return RESULT_ACCESS_DENIED;

	// 플레이어가 참여 중인 아레나의 ID 값을 세팅해 줌(BattleArenaInstance::_addPlayer 호출을 준비하는 정도의 의미)
	pPlayer->SetBattleArenaID( pArena->m_pArenaBase->nID );

	// 연습 경기에 참여할 때는 가명 기능을 사용할 수 없음(다른데에서 SetUseAlias( true )가 호출됐던 채로 여기로 들어오면 안 됨)
	assert( !pPlayer->IsUsingAlias() );

	unsigned short nResult = pBattleInstance->_addPlayer( pPlayer, nTeamNo );

	if( nResult != RESULT_SUCCESS )
	{
		pPlayer->SetBattleArenaID( 0 );
		return nResult;
	}

	// 그리고 경기에 대한 정보 방송
	SendBattleArenaBattleInfoMessage( pPlayer, pBattleInstance );

	// 다른 유저들에게 현재 유저가 새로 참여했음을 알려 줌
	BroadcastBattleArenaJoinBattleMessage( pBattleInstance, pPlayer );

	return RESULT_SUCCESS;
}

unsigned short BattleArenaManager::SetReadyExerciseGame( StructPlayer * pPlayer, bool bReady )
{
	// 배틀 아레나 연습 경기용 파티에 속한 상태에서만 레디 상태 변경 가능(시작된 이후에는 파티 타입이 TYPE_BATTLE_ARENA_TEAM 으로 변경되므로 이 체크만으로도 어느정도 경기 상태를 포함해서 걸러짐)
	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID || PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		return RESULT_NOT_EXIST;

	int nArenaID = pPlayer->GetBattleArenaID();

	if( !nArenaID )
		return RESULT_NOT_EXIST;

	assert( nArenaID == GameRule::BATTLE_ARENA_EXERCISE_GAME_ARENA_ID && nArenaID == pPlayer->GetBattleArenaID() );
	BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		// m_partyToArenaMap에 nArenaID가 없던 것도 아니고 아예 잘못된 nArenaID 가 들어있었다는 얘기가 되므로 있어서는 안되는 상황
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	assert( nInstanceNo == pArena->m_partyToInstanceMap.Get( nPartyID ) );

	if( !nInstanceNo )
		// 즐
		return RESULT_NOT_EXIST;

	BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 이건 또 좀 이상하지만 여튼 체크...
	if( !pBattleInstance )
		// 또 즐
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_setReadyExerciseGame( pPlayer, bReady );
}

unsigned short BattleArenaManager::StartExerciseGame( StructPlayer * pPlayer )
{
	// 배틀 아레나 연습 경기용 파티에 속한 상태에서만 경기 시작 시도 가능(시작된 이후에는 파티 타입이 TYPE_BATTLE_ARENA_TEAM 으로 변경되므로 이 체크만으로도 어느정도 경기 상태를 포함해서 걸러짐)
	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID || PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		return RESULT_NOT_EXIST;

	int nArenaID = m_partyToArenaMap.Get( nPartyID );

	if( !nArenaID )
		return RESULT_NOT_EXIST;

	if( nArenaID != GameRule::BATTLE_ARENA_EXERCISE_GAME_ARENA_ID || nArenaID != pPlayer->GetBattleArenaID() )
		return RESULT_ACCESS_DENIED;

	BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		// m_partyToArenaMap에 nArenaID가 없던 것도 아니고 아예 잘못된 nArenaID 가 들어있었다는 얘기가 되므로 있어서는 안되는 상황
		assert( 0 );
		return RESULT_NOT_EXIST;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	assert( nInstanceNo == pArena->m_partyToInstanceMap.Get( nPartyID ) );

	if( !nInstanceNo )
		// 즐
		return RESULT_NOT_EXIST;

	BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 이건 또 좀 이상하지만 여튼 체크...
	if( !pBattleInstance )
		// 또 즐
		return RESULT_NOT_EXIST;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	// 경기가 시작된 이후나 끝난 상태라면 변경 불가
	if( pBattleInstance->m_bStart || pBattleInstance->m_bEnd )
		return RESULT_NOT_ACTABLE;

	// 시작은 해당 경기의 첫 번째 팀의 첫 번째 파티의 파티장만이 가능
	if( nPartyID != pBattleInstance->m_anPartyID[ 0 ][ 0 ] || !PartyManager::GetInstance().IsLeader( nPartyID, pPlayer->GetPlayerUID() ) )
		return RESULT_ACCESS_DENIED;

	std::string strReady = pBattleInstance->_getFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE );
	int nReadyState = atoi( strReady.c_str() );

	// 전 팀의 레디 상태 체크
	for( int nTeamNo = 0 ; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		if( !( nReadyState & ( 1 << nTeamNo ) ) )
			return RESULT_NOT_READY;
	}

	// 파티 타입을 TYPE_BATTLE_ARENA_EXERCISE_TEAM 에서 TYPE_BATTLE_ARENA_TEAM 으로 변경해 줌
	for( int nTeamNo = 0 ; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		if( !PartyManager::GetInstance().ChangeBattleArenaExerciseTeamType( pBattleInstance->m_anPartyID[ nTeamNo ][ 0 ], false ) )
		{
			// 중간에 실패했으면 모두 롤백 'ㅠ ';
			for( int nPrevTeamNo = 0 ; nPrevTeamNo < nTeamNo ; ++nPrevTeamNo )
			{
				PartyManager::GetInstance().ChangeBattleArenaExerciseTeamType( pBattleInstance->m_anPartyID[ nPrevTeamNo ][ 0 ], true );
			}

			return RESULT_INVALID_ARGUMENT;
		}
	}

	// 경기 시작
	// BattleArenaInstance의 시작 및 그와 관련된 모든 시간값만 바꿔주고,
	// 참여자 전원이 입장되도록 입장 시각 세팅해주고,
	// 나머지는 onProcess에서 시작처리가 다 이루어지게만 함
	AR_TIME tCurrent = GetArTime();
	pBattleInstance->SetDefaultTimeByStartTime( tCurrent );

	for( int nTeamNo = 0 ; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = pBattleInstance->_getPlayerInfoList( nTeamNo );
		for( std::vector< BattleArenaInstance::PlayerInfo >::iterator it = pPlayerInfoList->begin() ; it != pPlayerInfoList->end() ; ++it )
		{
			(*it).nPendedEnterTime = tCurrent;
		}
	}

	BroadcastBattleArenaBattleInfoMessage( pBattleInstance );

	return RESULT_SUCCESS;
}

bool BattleArenaManager::IsReadyExerciseTeam( int nPartyID ) const
{
	if( !nPartyID || PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		return false;

	int nArenaID = m_partyToArenaMap.Get( nPartyID );

	if( nArenaID != GameRule::BATTLE_ARENA_EXERCISE_GAME_ARENA_ID )
	{
		assert( 0 );
		return false;
	}

	const BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		// m_partyToArenaMap에 nArenaID가 없던 것도 아니고 아예 잘못된 nArenaID 가 들어있었다는 얘기가 되므로 있어서는 안되는 상황
		assert( 0 );
		return false;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	int nInstanceNo = pArena->m_partyToInstanceMap.Get( nPartyID );

	// 방이 없거나 이미 해산됐다면...
	if( !nInstanceNo )
		// 즐
		return false;

	const BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	// 이건 또 좀 이상하지만 여튼 체크...
	if( !pBattleInstance || pBattleInstance->m_bEnd )
		// 또 즐
		return false;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	// 체크하려는 파티의 팀 번호 얻기
	int nTeamNo = pBattleInstance->_getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		// 파티 ID 기준으로 아레나를 찾았는데 해당 파티가 소속된 팀은 없는 경우
		// m_partyToArenaMap, m_partyToInstanceMap 이 락을 따로 쓰지 않아서 생길 가능성이 있긴 함(csBattle 걸기 직전에 파티 해산)
		return false;
	}

	std::string strReady = pBattleInstance->_getFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE );
	int nReadyState = atoi( strReady.c_str() );

	return !!( nReadyState & ( 1 << nTeamNo ) );
}

bool BattleArenaManager::HasOpponent( StructPlayer * pPlayer ) const
{
	int nPartyID = pPlayer->GetPartyID();

	if( !nPartyID || !PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
		return false;

	int nArenaID = pPlayer->GetBattleArenaID();
	if( !nArenaID )
		return false;

	const BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return false;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	int nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	if( !nInstanceNo )
		return false;

	const BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
	{
		assert( 0 );
		return false;
	}

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	int nTeamNo = pBattleInstance->_getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		assert( 0 );
		return false;
	}

	int nOpponentTeamNo = 1 - nTeamNo;
	return (pBattleInstance->_getTeamMemberCount( nOpponentTeamNo ) != 0);
}

bool BattleArenaManager::IsMoreMemberInvitable( StructPlayer * pPlayer ) const
{
	int nPartyID = pPlayer->GetPartyID();

	if( !nPartyID || PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		return false;

	int nArenaID = pPlayer->GetBattleArenaID();
	if( !nArenaID )
		return false;

	const BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return false;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	int nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	if( !nInstanceNo )
		return false;

	const BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
	{
		assert( 0 );
		return false;
	}

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	int nTeamNo = pBattleInstance->_getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		assert( 0 );
		return false;
	}

	return ( pBattleInstance->m_pArenaBase->nMaxMember / pBattleInstance->m_pArenaBase->nTeamCount ) >= pBattleInstance->_getTeamMemberCount( nTeamNo );
}

bool BattleArenaManager::SetBattleInstanceFlag( int nArenaID, unsigned char nInstanceNo, const char * pszName, const char * pszValue )
{
	if( !nArenaID || !nInstanceNo )
		return false;

	BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return false;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return false;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	pBattleInstance->_setFlag( pszName, pszValue );
	return true;
}

std::string BattleArenaManager::GetBattleInstanceFlag( int nArenaID, unsigned char nInstanceNo, const char * pszName ) const
{
	if( !nArenaID || !nInstanceNo )
		return "";

	const BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return "";
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	const BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return "";

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_getFlag( pszName );
}

int BattleArenaManager::GetTeamNo( const StructPlayer * pPlayer ) const
{
	int nArenaID = pPlayer->GetBattleArenaID();
	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	int nPartyID = pPlayer->GetPartyID();

	if( !nArenaID || !IsValidBattleArenaInstanceNo( nInstanceNo ) || !nPartyID )
		return INVALID_BATTLE_ARENA_TEAM_NO;

	const BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
		return INVALID_BATTLE_ARENA_TEAM_NO;

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	const BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return INVALID_BATTLE_ARENA_TEAM_NO;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	return pBattleInstance->_getTeamNo( nPartyID );
}

bool BattleArenaManager::IsOpponent( const StructPlayer * pPlayer, const StructPlayer * pTarget ) const
{
	int nPartyID = pPlayer->GetPartyID();
	int nTargetPartyID = pTarget->GetPartyID();

	if( !nPartyID || !nTargetPartyID ||
		PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_TEAM ||
		PartyManager::GetInstance().GetPartyType( nTargetPartyID ) != PartyManager::TYPE_BATTLE_ARENA_TEAM )
	{
		return false;
	}

	int nArenaID = pPlayer->GetBattleArenaID();
	int nInstanceNo = pPlayer->GetBattleArenaInstanceNo();

	if( !nArenaID || !nInstanceNo || nArenaID != pTarget->GetBattleArenaID() || nInstanceNo != pTarget->GetBattleArenaInstanceNo() )
		return false;

	const BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return false;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	const BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return false;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	if( !pBattleInstance->m_bStart || pBattleInstance->m_bEnd )
		return false;

	int nTeamNo = pBattleInstance->_getTeamNo( nPartyID );
	int nTargetTeamNo = pBattleInstance->_getTeamNo( nTargetPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO || nTargetTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		return false;

	return nTeamNo != nTargetTeamNo;
}

bool BattleArenaManager::GetRevivePosition( const StructPlayer * pPlayer, ArPosition & posRevive ) const
{
	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID )
		return false;

	int nArenaID = pPlayer->GetBattleArenaID();
	if( !nArenaID )
		return false;

	const BattleArena * pArena = getArena( nArenaID );
	if( !pArena )
		return false;

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	unsigned char nInstanceNo = pPlayer->GetBattleArenaInstanceNo();
	if( !nInstanceNo )
		return false;

	const BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );
	if( !pBattleInstance )
		return false;

	THREAD_SYNCHRONIZE2( pBattleInstance->m_csBattle );

	int nTeamNo = pBattleInstance->_getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		return false;

	posRevive = pArena->m_pArenaBase->posStart[ nTeamNo ];
	return true;
}

bool BattleArenaManager::IsCastablePropForTeam( int nArenaID, unsigned char nInstanceNo, const StructFieldProp * pProp, int nPartyID ) const
{
	if( !nArenaID || !nInstanceNo )
		return false;

	const BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return false;
	}

	THREAD_SYNCHRONIZE( pArena->m_csArena );

	const BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

	if( !pBattleInstance )
		return false;

	THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

	// 진행 중이지 않은 경기는 프랍 사용 불가
	if( !pBattleInstance->m_bStart || pBattleInstance->m_bEnd )
		return false;

	int nTeamNo = pBattleInstance->_getTeamNo( nPartyID );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		return false;

	return pBattleInstance->_isCastablePropForTeam( pProp, nTeamNo );
}

// 다각형과 점의 포함관계 체크에서 CCW 체크 이상 빠르고 간단한 로직을 못 봤음. 그래서 일단 이걸로 ㄱㄱ싱.
// 단, 이걸 쓰기 위해서는 다각형이 아래의 조건을 만족해야 함
//  1. 볼록 다각형이어야 함
//     (둔각인 내각이 생기면 다각형의 각 선분에 대해 점이 왼쪽 혹은 오른쪽에 있다는 판정만으로 포함관계를 확인할 수 없음)
//  2. 원점이 왼쪽 위인 2차원 좌표계(그림판 또는 포샵)에서 봤을 때 각 꼭지점 목록이 CCW 방향으로 이어져야 함
//     (순서가 반대가 되면 내외부 판정이 반대로 나옴)

// CheckCCW: (x1, y1) - (x2, y2) 선분(벡터)과 (x2, y2) - (ptx, pty) 선분(벡터)의 내적을 계산해 줌
// 리턴값이 0 이면 두 선분이 나란함을 의미하며,
// 0 보다 작은 경우는 (x1, y1) - (x2, y2) 선분의 진행 방향 기준으로 왼쪽에 (ptx, pty)가 있다는 것을,
// 0 보다 큰 경우는 반대로 오른쪽에 (ptx, pty)가 있다는 것을 의미함
AR_UNIT CheckCCW( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2, AR_UNIT ptx, AR_UNIT pty )
{
	return ( x2 - x1 ) * ( pty - y1 ) - ( y2 - y1 ) * ( ptx - x1 );
}

// IsPointIncludedCCW: CheckCCW만을 이용해서 다각형과 점 사이의 포함 관계 체크
// 모든 선분에 대해 지정된 점이 한 방향(CCW로 각 변이 진행되는 다각형의 경우 왼쪽)에 있다면 다각형 내부에 있다는 것이 됨
// 단, 이 로직의 전제 조건은 다각형이 볼록 다각형이라는 것(오목 다각형은 도형 내부의 점인데 특정 변의 오른쪽에 점이 위치하기도 함)
bool IsPointIncludedCCW( const X2D::Polygon< AR_UNIT > * pPolygon, const ArPosition & pt, bool bIncludeOnEdge )
{
	if( !pPolygon )
	{
		assert( 0 );
		return false;
	}

	const size_t nVertexCount = pPolygon->Size();
	if( !nVertexCount )
	{
		assert( 0 );
		return false;
	}

	const X2D::Point< AR_UNIT > * pPointList = pPolygon->GetRawPoint();
	for( size_t i = 0 ; i < nVertexCount ; ++i )
	{
		const X2D::Point< AR_UNIT > & lineBegin = pPointList[ i ];
		const X2D::Point< AR_UNIT > & lineEnd = pPointList[ ( i + 1 ) % nVertexCount ];

		AR_UNIT fCCW = CheckCCW( lineBegin.x, lineBegin.y, lineEnd.x, lineEnd.y, pt.x, pt.y );
		if( bIncludeOnEdge && fCCW > 0 )
			return false;
		if( !bIncludeOnEdge && fCCW >= 0 )
			return false;
	}

	return true;
}

// GetCollidedPointCCW: CheckCCW를 응용한 두 선분 사이의 충돌점 계산
// 두 선분(다각형의 각 변과 이동 경로)의 각 방향에 대한 내적 값을 계산해서 정규화 시키면 서로의 내적 값이 0.0 ~ 1.0 이면 두 선분이 교차하는 것이 됨
bool GetCollidedPointCCW( const X2D::Polygon< AR_UNIT > * pPolygon, const ArPosition & ptBegin, const ArPosition & ptEnd, ArPosition & ptCollision )
{
	if( !pPolygon )
	{
		assert( 0 );
		return false;
	}

	const size_t nVertexCount = pPolygon->Size();
	if( !nVertexCount )
	{
		assert( 0 );
		return false;
	}

	const X2D::Point< AR_UNIT > * pPointList = pPolygon->GetRawPoint();
	for( size_t i = 0 ; i < nVertexCount ; ++i )
	{
		const X2D::Point< AR_UNIT > & lineBegin = pPointList[ i ];
		const X2D::Point< AR_UNIT > & lineEnd = pPointList[ ( i + 1 ) % nVertexCount ];

		// 두 선분이 각각 포함되는 AABB가 서로 겹치지 않는다면 충돌 체크 불필요
		if( std::min( lineBegin.x, lineEnd.x ) > std::max( ptBegin.x, ptEnd.x ) ||
			std::max( lineBegin.x, lineEnd.x ) < std::min( ptBegin.x, ptEnd.x ) ||
			std::min( lineBegin.y, lineEnd.y ) > std::max( ptBegin.y, ptEnd.y ) ||
			std::max( lineBegin.y, lineEnd.y ) < std::min( ptBegin.y, ptEnd.y ) )
		{
			continue;
		}

		// 교차 체크
		AR_UNIT numeratorAB = ( lineBegin.y - ptBegin.y ) * ( ptEnd.x - ptBegin.x ) - ( lineBegin.x - ptBegin.x ) * ( ptEnd.y - ptBegin.y );
		AR_UNIT denominator = ( lineEnd.x - lineBegin.x ) * ( ptEnd.y - ptBegin.y ) - ( lineEnd.y - lineBegin.y ) * ( ptEnd.x - ptBegin.x );

		// 두 선분의 기울기가 동일
		if( !denominator )
		{
			// 공선(위상도 같음, 두 선분을 무한 연장하면 같은 선이 됨)
			if( !numeratorAB )
			{
				// 게다가 이미 AABB가 겹친다는 것도 확인했기 때문에 충돌 점이 생기는 게 아니라 겹치는 선분 영역이 발생됨
				// 번거로우니 이번 선분의 시작점을 그냥 교차점으로 반환 -ㅠ -
				ptCollision = ptBegin;
				return true;
			}

			// 평행선(절대 만나지 않음)
			continue;
		}

		AR_UNIT numeratorCD = ( lineBegin.y - ptBegin.y ) * ( lineEnd.x - lineBegin.x ) - ( lineBegin.x - ptBegin.x ) * ( lineEnd.y - lineBegin.y );

		// numeratorCD == 0.0f 인 경우는 ptBegin이 [lineBegin, lineEnd]인 변과 맞닿아 있었다는 이야기가 됨
		// 이건 충돌점으로 뽑아내면 안되고 무시해야 함
		if( numeratorCD == 0.0f )
			continue;

		if( denominator < 0 )
		{
			denominator *= -1;
			numeratorAB *= -1;
			numeratorCD *= -1;
		}

		float factorAB = ( (float)numeratorAB ) / denominator;
		float factorCD = ( (float)numeratorCD ) / denominator;

		if( factorCD >= 0.0f && factorCD <= 1.0f )
		{
			if( factorAB >= 0.0f && factorAB <= 1.0f )
			{
				X2D::Point< AR_UNIT > ptCol = lineBegin + ( ( lineEnd - lineBegin ) * factorAB );
				ptCollision.x = ptCol.x;
				ptCollision.y = ptCol.y;
				return true;
			}
		}
	}

	return false;
}

bool BattleArenaManager::ValidateMovableRoute( int nArenaID, unsigned char nInstanceNo, int nPartyID, const ArPosition & posStart, std::vector< ArPosition > & vMoveInfo ) const
{
	if( !nArenaID || !nInstanceNo || vMoveInfo.empty() )
		return false;

	const BattleArena * pArena = getArena( nArenaID );

	if( !pArena )
	{
		assert( 0 );
		return false;
	}

	// csArena, csBattle 걸고 체크해야 할 것만 체크하고 락 해제
	// * 이동 경로 조정 대상으로 확인된 경우의 처리는 락 없이 할 수 있지만 부하가 높은 편이므로.
	int nTeamNo = 0;
	{
		THREAD_SYNCHRONIZE( pArena->m_csArena );

		const BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nInstanceNo );

		if( !pBattleInstance )
			return false;

		THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

		if( pBattleInstance->m_bStart )
			return false;

		nTeamNo = pBattleInstance->_getTeamNo( nPartyID );
		// 소속 팀이 없는 경우;? 아레나 내부에 있는데;?
		if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
		{
			assert( 0 );
			return false;
		}
	}

	// { 시작 전의 경기장 내에서 이동임이 확인되었으므로 이동 경로 체크 및 시작 영역을 벗어나려는 경우 경로 제한

	// * 출발점은 경로 조정에서 제외됨

	// 충돌 체크 대상이 될 폴리곤 선정(시작 영역 정보가 없는 경우에는 아무 체크도 안 함)
	const X2D::Polygon< AR_UNIT > * pBeginArea = pArena->m_pArenaBase->apBeginAreaList[ nTeamNo ];
	if( !pBeginArea )
		return false;

	// 출발점의 시작 영역 내외부 판정
	bool bPrevPosIsInside = IsPointIncludedCCW( pBeginArea, posStart, true );

	// 출발점이 영역 외부라면 직선으로 영역 내부로 이동하는 것만 허용(가다 멈추더라도 어차피 영역쪽으로 점점 가까워지니까...)
	if( !bPrevPosIsInside )
	{
		if( vMoveInfo.size() != 1 || !IsPointIncludedCCW( pBeginArea, vMoveInfo.front(), false ) )
		{
			vMoveInfo.clear();
			return true;
		}

		// 여기 오면 한 방에 영역 내부로 이동을 시도하는 경우이므로 별도의 검사 없이 넘겨 줌
		return false;
	}

	// 출발점이 영역 내부이므로 이후 이동 경로 지점이 영역 밖으로 나간 점이 있는지 체크
	for( std::vector< ArPosition >::iterator it = vMoveInfo.begin() ; it != vMoveInfo.end() ; ++it )
	{
		const ArPosition & prevPos = ( it == vMoveInfo.begin() ) ? posStart : ( *( it - 1 ) );
		ArPosition & nextPos = (*it);

		// 선분의 도착점이 외부로 나가는 경우만 경로 재조정(경로 최초 출발점부터 현재 체크 선분의 출발점까지 모두 도형 안쪽이었던 상태)
		if( IsPointIncludedCCW( pBeginArea, nextPos, false ) )
		{
			// 시작 영역 내부로의 이동이라면 별다른 처리 없이 다음 이동 지점 체크
			continue;
		}

		// 시작 영역 외부로의 이동이므로 경로를 잘라냄(도착점 보정)
		ArPosition posCollision;
		if( GetCollidedPointCCW( pBeginArea, prevPos, nextPos, posCollision ) )
		{
			// 충돌은 했는데 prevPos와 같은 점이 충돌점이라고 나왔으면 그냥 nextPos부터 다 삭제
			if( prevPos == posCollision )
			{
				vMoveInfo.erase( it, vMoveInfo.end() );
			}
			else
			{
				// 충돌점이 확인됐으면 nextPos를 충돌점으로 변경해서 경로를 잘라 내고 이후 이동 경로를 모두 제거
				nextPos = posCollision;
				vMoveInfo.erase( it + 1, vMoveInfo.end() );
			}
		}
		else
		{
			// prevPos가 영역 안에 있었고, nextPos가 영역 밖으로 나갔는데 충돌이 감지되지 않은 경우
			// 경계선 위에 서 있던 채로 밖으로 나가는 경우에 최초 출발점의 내외부 판정에서는 
			// 일단 이번 선분의 시작 점에서 경로가 종료되도록 조정
			vMoveInfo.erase( it, vMoveInfo.end() );
		}

		return true;
	}

	// } 시작 전의 경기장 내에서 이동임이 확인되었으므로 이동 경로 체크 및 시작 영역을 벗어나려는 경우 경로 제한

	// 여기까지 왔다는 건 중간에 경로 보정이 발생하지 않았다는 의미이므로 충돌하지 않았다는 걸로 반환
	return false;
}

bool BattleArenaManager::IsOfflinePenaltyReceivedPlayer( PlayerUID nPlayerUID )
{
	THREAD_SYNCHRONIZE( m_csOfflinePenaltyReceivedPlayer );

	std::vector< PlayerUID >::iterator it = std::find( m_vOfflinePenaltyReceivedPlayerUIDList.begin(), m_vOfflinePenaltyReceivedPlayerUIDList.end(), nPlayerUID );

	if( it != m_vOfflinePenaltyReceivedPlayerUIDList.end() )
	{
		m_vOfflinePenaltyReceivedPlayerUIDList.erase( it );
		return true;
	}

	return false;
}

void BattleArenaManager::AddOfflinePenaltyReceivedPlayer( PlayerUID nPlayerUID )
{
	THREAD_SYNCHRONIZE( m_csOfflinePenaltyReceivedPlayer );

	if( std::find( m_vOfflinePenaltyReceivedPlayerUIDList.begin(), m_vOfflinePenaltyReceivedPlayerUIDList.end(), nPlayerUID ) != m_vOfflinePenaltyReceivedPlayerUIDList.end() )
	{
		assert( 0 );
		return;
	}

	m_vOfflinePenaltyReceivedPlayerUIDList.push_back( nPlayerUID );
}

void BattleArenaManager::RemoveOfflinePenaltyReceivedPlayer( PlayerUID nPlayerUID )
{
	THREAD_SYNCHRONIZE( m_csOfflinePenaltyReceivedPlayer );

	std::vector< PlayerUID >::iterator it = std::find( m_vOfflinePenaltyReceivedPlayerUIDList.begin(), m_vOfflinePenaltyReceivedPlayerUIDList.end(), nPlayerUID );

	if( it != m_vOfflinePenaltyReceivedPlayerUIDList.end() )
		m_vOfflinePenaltyReceivedPlayerUIDList.erase( it );
}

void BattleArenaManager::onProcess( int nThreadIdx )
{
	// onProcess가 할 일들
	// - 유효한 경기에 대한 개별 처리
	//   * 해제되어야 할 경기 인스턴스의 뒷처리
	//   * 진행 중인 경기의 개별 처리(BattleArenaInstance::_onProcess)
	//   * 진행 중인 경기에 입장해야 하는 유저 처리
	// - 등급별 각 대기열에 대한 경기 시작 가능성 체크 및 경기 시작 처리

	// 시험적인 락 정책
	// - 지역 락 -> csArena -> csBattle 의 순서로 락을 걸어야 하는데, 지역 락을 먼저 걸고 방 목록을 순회하려면
	//   특정 레이어만 락을 걸지 못하고 전체 레이어에 락을 걸어야 하기 때문에 같은 타입의 경기끼리 지역락 간섭이 심하게 발생할 가능성이 높음.
	//   따라서 아래와 같은 순서로 처리를 시도해 봄.(성능이 별로거나 문제가 있으면 그냥 전체 레이어 걸고 ㄱㄱ싱~)
	//   * csArena 만 걸고 현재 게임이 진행 중인 레이어 목록 수집(로컬 std::vector 인스턴스에)
	//   * 로컬 std::vector 인스턴스인 레이어 목록 순회
	//     v 현재 순회 대상인 레이어에만 지역락
	//     v csArena
	//     v 현재 순회 대상인 레이어에 해당하는 BattleArenaInstance 검색
	//     v csBattle
	//     v csArena, csBattle이 풀렸다가 걸렸기 때문에 그 사이에 경기가 종료되지 않았는지 체크
	//       경기가 종료됐다면 다음 레이어로 진행
	//     v 처리해야 할 내용 처리
	//
	//   이 방식을 사용하면 지역락을 레이어별로 걸고 처리할 수 있지만 csArena를 한 번 풀었다가 다시 걸어야 한다는 문제점 때문에
	//   csArena가 풀렸다가 걸리는 사이에 새로 생성된 방의 경우는 최초 1회의 처리가 누락됨

	// BattleArenaManager의 update priority 조절을 위한 체크
	bool bActiveBattleInstanceExist = false;

	for( std::vector< BattleArena * >::iterator itArena = m_vArenaList.begin() ; itArena != m_vArenaList.end() ; ++itArena )
	{
		BattleArena * pArena = (*itArena);

		// 경기 정보가 유효한 레이어 번호 수집
		std::vector< unsigned char > vActiveLayerList;
		{
			THREAD_SYNCHRONIZE( pArena->m_csArena );

			vActiveLayerList = pArena->m_vActiveLayerList;
		}

		// 유효한 레이어에 대해 레이어별로 지역락부터 다시 걸고 처리
		for( std::vector< unsigned char >::iterator itLayer = vActiveLayerList.begin() ; itLayer != vActiveLayerList.end() ; ++itLayer )
		{
			unsigned char nLayer = (*itLayer);

			// 여기서 사용하는 각종 락(아레나 지역락, csArena, csBattle)이 걸린 상태에서는
			// 아레나 외부에 있는 유저를 아레나 내부로 강제로 워프시키거나 스크립트 처리를 할 수 없기 때문에
			// 내부로 강제 워프시켜야 하는 유저 목록만 따로 수집하고, 모든 락을 푼 시점에 해당 처리를 몰아서 함
			// * pair의 first 멤버 타입이 StructPlayer * 가 아니라 StructPlayer::iterator 인 이유는
			//   아래쪽에서 이 벡터를 처리할 때는 아무런 락도 걸려있지 않은 상태이기 때문에
			//   리스트에 내용을 채울 당시에는 유효하던 StructPlayer * 가 처리 시점에는 무효화되어 있을 수 있기 때문
			std::vector< std::pair< StructPlayer::iterator /* itPlayer */, int /* nTeamNo */ > > vPendedEnterPlayerList;

			{
				ARCADIA_LOCK( pArena->LockWholeArena( nLayer ) );

				// 인스턴스 해제 처리가 필요한 경우에는 실제 해제 처리 전에 스크립트를 먼저 실행해야 하므로
				// 락 풀고 스크립트 실행이 필요한지 여부만 체크할 수 있게 변수를 둠
				// 그리고 스크립트 실행 처리가 완료된 후에 실제 인스턴스 해제 처리를 다시 함
				// * 이 과정에서 한 번 csArena, csBattle을 해제했다 다시 걸게 되는데 어차피 BattleArenaInstance가 해제되지만 않으면 상관없을 뿐더러,
				//   해제 처리는 이 코드(BattleArenaManager::onProcess)에서 밖에 하지 않고 이 코드가 멀티 쓰레드로 동시에 여러개가 실행될 일은 없으므로 문제 발생 가능성은 없다고 예상됨

				{
					THREAD_SYNCHRONIZE( pArena->m_csArena );

					// csArena 풀었다 거는 사이에 방이 파괴된 경우에 대해 체크 필요
					if( !pArena->_isOccupiedInstanceNo( nLayer ) )
						continue;

					BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nLayer );

#ifdef _DEBUG
					if( !pBattleInstance )
					{
						// pArena->_isOccupiedInstanceNo( nLayer ) == true 인데 pArena->apBattleList[ nLayer ] == NULL 인 건 의도된 로직이 아님
						// 어딘가에서 aInstanceNoUseFlag 만 true로 세팅하고 BattleArenaInstance의 인스턴스는 생성하지 않고 있거나, 혹은 다른 문제 사례 확인/수정 필요
						assert( 0 );
						continue;
					}
#endif
					// 활성화 상태의 경기가 있다는 것이 확인됨
					bActiveBattleInstanceExist = true;
					bool bWrapUpInstance = false;

					{
						THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

						// 예약해 둔 스크립트 처리
						// * 끝난 경기라도 밀린 스크립트가 있다면 실행시켜주는 게 맞겠지만, 로직 처리상 문제가 발생한다면 BattleArenaInstance::_onProcess 안으로 넣으면 될듯
						pBattleInstance->_procPendedScript();

						// 종료된 경기면
						if( pBattleInstance->m_bEnd )
						{
							// 경기 종료된 후에 점수판 보여준 상태에서 대기 시간이 다 경과해서 유저 모두 내쫓고 경기 인스턴스를 해제(라고 쓰고 초기화해서 재활용 준비라고 읽음)해야 하는 경우
							if( GetArTime() >= pBattleInstance->m_nForceQuitTime )
							{
								// BattleArenaInstance 내부에서 처리할만한 건 일단 해주셈 'ㅠ '(인스턴스 해제 스크립트도 실행됨)
								pBattleInstance->_wrapUpInstance( ALT_GAME_OVER );

								// 방 관련 처리는 csBattle 풀고 나서 처리~
								bWrapUpInstance = true;
							}

							// 위의 경우 아니면 아무것도 안하고 그냥 멍 때림
						}
						// 종료 전 경기면 일단 _onProcess ㄱㄱ싱
						else
						{
							pBattleInstance->_onProcess();

							// 경기가 _onProcess 안에서 종료되지 않았다면 입장 대기 유저들로 선정된 목록 처리 준비
							if( !pBattleInstance->m_bEnd )
							{
								std::vector< StructPlayer* >::iterator it = pBattleInstance->m_vPendedEnterPlayerList.begin();
								for( ; it != pBattleInstance->m_vPendedEnterPlayerList.end() ; ++it )
								{
									vPendedEnterPlayerList.push_back( std::make_pair(
																		StructPlayer::iterator( *it ),
																		pBattleInstance->_getTeamNo( (*it)->GetPartyID() )
																	) );
								}

								pBattleInstance->m_vPendedEnterPlayerList.clear();
							}
						}
					}

					// 인스턴스가 해제되어야 한다면 해제 처리
					if( bWrapUpInstance )
					{
						// 입장 대기 상태인 유저가 있을리는 없지만 만에 하나의 경우를 위해 모두 제거
						assert( vPendedEnterPlayerList.empty() );
						vPendedEnterPlayerList.clear();

						pArena->_freeInstanceNo( nLayer );

						// 인스턴스 해제 처리가 되었으니 추가적으로 현재 인스턴스에 대한 어떤 처리도 일어나지 않도록 다음 레이어로 진행
						continue;
					}
				}
			}

			// 입장 처리가 필요한 유저들이 실행할 스크립트 준비(추가로 재접속에 의한 것이 아니라는 파라미터를 세팅해 줌)
			std::vector< std::string > vTagReplacement;
			vTagReplacement.push_back( "#@reconnect@#" );
			vTagReplacement.push_back( "0" );
			std::string & strScript = GetBattleArenaEventScript( pArena->m_pArenaBase, BASET_ENTER_INSTANCE, nLayer, &vTagReplacement );

			// 입장 처리가 필요한 유저들에 대한 처리
			// * 이 위치는 지역락, csArena, csBattle 아무것도 걸려있지 않기 때문에 다른 쪽에서 현재 입장하려는 유저들이 입장해야 할 경기가
			//   중간에 종료되는 사태가 발생할 수도 있으며, 이렇게 되면 입장은 했는데 이미 경기는 끝난 상태가 될 수 있음.
			//   다만 그런 경우는 경기 시간 만료(BattleArenaInstance::_onProcess 안에서 발생하므로)가 아닌 최대 점수 도달 혹은 경기 취소의 경우에 해당하며,
			//   어떤 경우든 BattleArenaInstance 자체가 즉시 해제되는 경우는 없으므로(모두 점수판을 보고 있다가 알아서 나가든 1분 지나서 쫓겨나든 함)
			//   큰 문제로 번지지는 않을 거라고 예상됨. 또한, 이번 onProcess 실행 후에 다시 onProcess가 실행될 때는 이미 bEnd == true 라는 것을
			//   각 락을 걸고 처리하는 단계에서 확인 가능하기 때문에, 해당 증상이 장시간 반복되진 않음(즉, 경기 완료되는 순간 1회만 발생함)
			//   여하튼 깔끔하지 않은 처리이지만 락 순서 문제와 지역락에 있어서 아레나 내부에 있지 않은 유저를 핸들링해야 한다는 점에서 깔끔한 답이 안 보이는 상태임... Orz...
			bool bBattleCanceled = false;
			for( std::vector< std::pair< StructPlayer::iterator, int > >::iterator it = vPendedEnterPlayerList.begin() ; it != vPendedEnterPlayerList.end() ; ++it )
			{
				StructPlayer * pPlayer = (*(*it).first);

				// 유저가 락 풀고 다시 거는 사이에 로그아웃해버렸다면 그냥 패스
				// 대개 이 상황이면 BattleArenaInstance::vPlayerInfo[ nTeamNo ].at( ~~ ).pPlayer == NULL 이 되어 있을 가능성이 높음(로그아웃하면서 이탈 처리도 됐을테니까)
				if( !pPlayer->IsLogin() )
					continue;

				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

				// 이용 불가 상태 체크
				unsigned short nErrorCode = RESULT_SUCCESS;

				int nPartyID = pPlayer->GetPartyID();
				int nTeamNo = (*it).second;
				// 위에서 vPendedEnterPlayerList.push_back 할 때 이미 캐릭터가 파티에서 이탈된 상태였던 경우
				// 경기용 파티에 참여된 이후에 정상적인 루트가 아닌 다른 루트에 의해 파티에서 이탈되거나 파티가 해산됐다는 것을 의미함
				// 이 경우에는 파티를 해산시키거나 문제가 되는 루트를 찾아서 수정해야 함(문제 재현하면서 PartyManager::LeaveParty/DestroyParty에 Breakpoint 걸면 잡기 쉬움)
				if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
				{
					assert( 0 );
					nErrorCode = RESULT_UNKNOWN;
				}
				// 락을 풀었다 다시 거는 사이에 팀에서 이탈했거나 파티가 해산된 경우에는 이탈 처리
				// * onProcess 실행 중에 _wrapUpInstance가 다른 곳에서 호출되진 않기 때문에 경기 인스턴스가 완전히 해제되는 경우는 없고
				//   입장 시점이 되는 순간 자진해서 이탈해버린 경우에는 파티 소속이 아닐 수 있음
				// * 이탈 후 다른 파티에 가입했으면 nPartyID != 0 이고 입장하려는 인스턴스의 팀별 파티 ID에 nPartyID가 없겠지만
				//   여기서 BattleArenaInstance에 접근하려면 다시 락을 걸어야 하고, 그런 락 풀었다 걸었다 하며 반복하는 체크는
				//   최종 처리를 락을 푼 상태로 해야 하는 이상 아무런 의미가 없음. 그리고 시간상으로 봤을 때 위에서 락 풀자마자 탈퇴+다른 파티 가입까지 한다는건 인간적으로 무리;;
				//   그래서 nPartyID != 0 와 파티 타입만 체크함
				else if( !nPartyID || !PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
				{
					// 정상적으로 경기가 종료됐다고 해도 락 잠깐 풀었다 거는 사이에 파티까지 해산되기는 쉽지 않음(_wrapUpInstance가 호출되어야 하니까)
					// 그래서 어떤 상황인지 예측 가능한 경우가 없음
					assert( 0 );
					nErrorCode = RESULT_UNKNOWN;
				}
				// 사망 상태로는 이용 불가
				else if( pPlayer->IsDead() )				nErrorCode = RESULT_NOT_ENOUGH_HP;
				// 거래 중엔 이용 불가
				else if( pPlayer->IsTrading() )				nErrorCode = RESULT_NOT_ACTABLE_WHILE_TRADING;
				// 베어로드, 데스매치, 인던에서는 이용 불가
				else if( pPlayer->IsInHuntaholic() )		nErrorCode = RESULT_NOT_ACTABLE_IN_HUNTAHOLIC;
				else if( pPlayer->IsInDeathmatch() )		nErrorCode = RESULT_NOT_ACTABLE_IN_DEATHMATCH;
				else if( pPlayer->IsInInstanceDungeon() )	nErrorCode = RESULT_NOT_ACTABLE_IN_INSTANCE_DUNGEON;

				// 실패 시에는 이탈 처리(페널티 부여도 있음)
				if( nErrorCode != RESULT_SUCCESS )
				{
					// 경기에서 이탈 처리
					THREAD_SYNCHRONIZE( pArena->m_csArena );

					BattleArenaInstance * pBattleInstance = pArena->_getBattleInstance( nLayer );

					// csArena 풀었다 거는 사이에 방이 파괴된 경우에 대해 체크 필요
					if( !pBattleInstance )
					{
						// 입장 타이머가 다 돌아갔는데 죽어 있었거나 할 때 락 풀었다 거는 사이에 방이 폭파돼버리면 여기 걸릴 수 있을지도...
						// * pArena->_isOccupiedInstanceNo( nLayer ) == true 인데 pArena->apBattleList[ nLayer ] == NULL 인 건 의도된 로직이 아님
						//   어딘가에서 aInstanceNoUseFlag 만 true로 세팅하고 BattleArenaInstance의 인스턴스는 생성하지 않고 있거나, 혹은 다른 문제 사례 확인/수정 필요
						assert( !pArena->_isOccupiedInstanceNo( nLayer ) );
						break;
					}

					THREAD_SYNCHRONIZE1( pBattleInstance->m_csBattle );

					// csBattle 풀고 다시 거는 사이에 경기가 종료되어 버렸으면 아무것도 안 함(어차피 끝나고 보상처리도 끝났을텐데 페널티주고 쫓아내서 뭐 함?)
					if( pBattleInstance->m_bEnd )
						break;

					// 한 팀이 완전히 비워져서 경기가 취소되어야 하는 경우는 다음 _onProcess가 실행되는 시점(약 1초 후)에 처리되도록 함
					// * _removePlayer 호출할 때마다 바로 처리해 주면 너무 방 파괴되어야 하는 코드가 여기저기 붙는 것 같아서 -_ -;;
					//   대신 방이 파괴되어야 할 상황이라면 아직 방에 입장시키지 않은 유저들은 그냥 밖에 놔 둠
					bBattleCanceled = ( pBattleInstance->_removePlayer( pPlayer, ALT_FAILED_TO_JOIN_BATTLE ) == RESULT_NOT_ENOUGH_BULLET );

					// 페널티 부여(연습 경기이거나 원인 불명의 상태로 이탈된 경우라면 페널티 없음)
					if( !pArena->m_pArenaBase->IsExerciseGameArena() && nErrorCode != RESULT_UNKNOWN )
					{
						pPlayer->IncBattleArenaPenalty();
						pPlayer->DBQuery( new DB_UpdateBattleArenaPenalty( pPlayer ) );

						SendBattleArenaPenaltyMessage( pPlayer );
					}

					// bBattleCanceled의 값과 관계없이 모든 참여자에 대해 확인해서 페널티를 줘야 하는 경우는 페널티를 줘야 하기 때문에
					// 여기서 break를 쓰면 안되고 무조건 continue가 되어야 함
					continue;
				}

				// 한 팀의 멤버가 모두 이탈되었다는 것이 확인됐으면 입장 처리를 하지 않음
				if( bBattleCanceled )
					continue;

				// 입장 시 실행되어야 할 스크립트 실행
				ThreadPlayerHelper TPHelper( pPlayer );
				BattleArenaRunScriptWithLog( pArena->m_pArenaBase, BASET_ENTER_INSTANCE, nLayer, strScript, pPlayer );

				// 입장 전 좌표 보관
				pPlayer->StoreCurrentStatesOnEnterInstanceGame( false );

				// 입장 워프(DB상의 입장 좌표 기준 +-2 미터씩 랜덤으로 옮겨 줌)
				ArPosition posEnter = pArena->m_pArenaBase->posStart[ nTeamNo ];
				posEnter.x += XRandom( GameRule::DEFAULT_UNIT_SIZE * -2, GameRule::DEFAULT_UNIT_SIZE * 2 );
				posEnter.y += XRandom( GameRule::DEFAULT_UNIT_SIZE * -2, GameRule::DEFAULT_UNIT_SIZE * 2 );

				pPlayer->PendWarp( posEnter.GetX(), posEnter.GetY(), nLayer );
				pPlayer->SetMove( pPlayer->GetCurrentPosition( GetArTime() ), 0 );
				ArcadiaServer::Instance().SetObjectPriority( pPlayer, UPDATE_PRIORITY_HIGHEST );
			}
		}

		// 새로 시작되는 경기에 대해 경기 생성 스크립트를 csBattle, csArena는 풀고, 해당 레이어의 지역락만 다시 걸고 실행하기 위해
		unsigned char nNewInstanceNo = INVALID_BATTLE_ARENA_INSTANCE_NO;
		{
			THREAD_SYNCHRONIZE( pArena->m_csArena );
			nNewInstanceNo = pArena->_tryStartNewBattle();
		}

		if( nNewInstanceNo != INVALID_BATTLE_ARENA_INSTANCE_NO )
		{
			// 경기가 진행 중일 때는 BattleArenaManager의 update priority를 최상으로 높여 줌
			// * UPDATE_PRIORITY_HIGH 정도여도 다 괜찮은데 빙고/킬 스크립트 예약/실행 딜레이가 너무 심해질 소지가 있어서;;
			RaiseUpdatePriority();
			bActiveBattleInstanceExist = true;

			// 경기장 영역 내에 지역락을 걸고 스크립트 실행(csArena, csBattle 중 아무것도 걸리지 않은 상태)
			ARCADIA_LOCK( pArena->LockWholeArena( nNewInstanceNo ) );

			// 방금 생성됐지만, 경기 자체가 파괴된 경우에 스크립트 실행을 방지하기 위해서 csArena는 걸고 방 상태 다시 체크
			THREAD_SYNCHRONIZE( pArena->m_csArena );

			// 근데 이거 가능한 경우가 있나 ` `;? _wrapUpInstance가
			// BattleArenaManager::onProcess (지금 이 함수)의 위쪽에서 실행되기 때문에 사실상 불가능일텐데...
			if( !pArena->_isOccupiedInstanceNo( nNewInstanceNo ) )
				continue;

			BattleArenaRunEventScript( pArena->m_pArenaBase, BASET_CREATE_INSTANCE, nNewInstanceNo, NULL );
		}
	}

	// 활성화 된 경기도 없고 새로 생성된 경기도 없다면 update priority 낮춤
	if( !bActiveBattleInstanceExist )
		DownUpdatePriority();
}

BattleArena * BattleArenaManager::getArena( int nArenaID )
{
	assert( m_bInit );

	for( std::vector< BattleArena * >::iterator it = m_vArenaList.begin() ; it != m_vArenaList.end() ; ++it )
	{
		BattleArena* pArena = (*it);
		if( pArena->m_pArenaBase->nID != nArenaID )
			continue;

		return (*it);
	}

	assert( 0 );
	return NULL;
}

const BattleArena * BattleArenaManager::getArena( int nArenaID ) const
{
	assert( m_bInit );

	for( std::vector< BattleArena * >::const_iterator it = m_vArenaList.begin() ; it != m_vArenaList.end() ; ++it )
	{
		BattleArena* pArena = (*it);
		if( pArena->m_pArenaBase->nID != nArenaID )
			continue;

		return (*it);
	}

	assert( 0 );
	return NULL;
}

unsigned short BattleArenaManager::checkJoinable( StructPlayer * pPlayer, bool bIgnorePenalty )
{
	// { 캐릭터의 상태 체크

	// 캐릭터 페널티 상태 체크
	if( !bIgnorePenalty && pPlayer->GetBattleArenaBlockTime() > time( NULL ) )
		return RESULT_COOL_TIME;

	// 사망 상태로는 이용 불가
	if( pPlayer->IsDead() )
		return RESULT_NOT_ENOUGH_HP;
	// 거래 중엔 이용 불가
	if( pPlayer->IsTrading() )
		return RESULT_NOT_ACTABLE_WHILE_TRADING;

	// 베어로드, 데스매치, 인던에서는 이용 불가
	if( pPlayer->IsInHuntaholic() )
		return RESULT_NOT_ACTABLE_IN_HUNTAHOLIC;
	if( pPlayer->IsInDeathmatch() )
		return RESULT_NOT_ACTABLE_IN_DEATHMATCH;
	if( pPlayer->IsInInstanceDungeon() )
		return RESULT_NOT_ACTABLE_IN_INSTANCE_DUNGEON;

	// 일반 파티 외의 파티에 가입된 상태에서는 이용 불가
	int nPrevPartyID = pPlayer->GetPartyID();
	if( nPrevPartyID && PartyManager::GetInstance().GetPartyType( nPrevPartyID ) != PartyManager::TYPE_NORMAL_PARTY )
		return RESULT_NOT_ACTABLE_IN_SIEGE_OR_RAID;

	// 다른 아레나 대기열에 대기되어 있거나 참여 중인 상태에서는 이용 불가
	if( pPlayer->GetBattleArenaID() )
		return RESULT_ALREADY_EXIST;

	// } 캐릭터의 상태 체크

	return RESULT_SUCCESS;
}
