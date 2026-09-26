
#include "LogClient/LogClient.h"

#include "GameMessage.h"
#include "SendMessage.h"
#include "BattleArenaManager.h"
#include "BattleArenaWithWaitQueue.h"
#include "StructPlayer.h"
#include "PartyManager.h"


void	BattleArenaManager::assemblePlayerScoreFromPlayerInfo( TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* pPlayerScore,
															  const BattleArenaInstance::PlayerInfo* pPlayerInfo )
{
	if( pPlayerInfo->pPlayer )
	{
		s_strcpy( pPlayerScore->szName, _countof( pPlayerScore->szName ), pPlayerInfo->pPlayer->GetAlias() );
	}
	else
	{
		s_strcpy( pPlayerScore->szName, _countof( pPlayerScore->szName ), PartyManager::GetInstance().GetMemberDisplayName( pPlayerInfo->nPartyID, pPlayerInfo->nPlayerUID ).c_str() );
	}

	pPlayerScore->nKillCount = pPlayerInfo->nKillCount;
	pPlayerScore->nDeathCount = pPlayerInfo->nDeathCount;
	pPlayerScore->nPropActivateCount = pPlayerInfo->nPropActivateCount;
	pPlayerScore->nTotalGainAP = pPlayerInfo->nTotalGainAPByKill + pPlayerInfo->nTotalGainAPByProp + pPlayerInfo->nTotalGainAPByResult;
}

void	BattleArenaManager::BroadcastBattleArenaMessage( const BattleArenaInstance* pBattleInstance, const TS_MESSAGE* pMsg )
{
	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = pBattleInstance->_getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			std::vector< BattleArenaInstance::PlayerInfo >::const_iterator it = pPlayerInfoList->begin();
			for( ; it != pPlayerInfoList->end(); ++it )
			{
				const BattleArenaInstance::PlayerInfo& pi = (*it);
				StructPlayer* pPlayer = pi.pPlayer;
				if( pPlayer != NULL )
				{
					PendMessage( pPlayer, pMsg );
				}
			}
		}
	}
}

void	BattleArenaManager::SendBattleArenaJoinQueueMessage( const StructPlayer* pPlayer, const BattleArena* pArena, _BATTLE_GRADE eGrade )
{
	TS_SC_BATTLE_ARENA_JOIN_QUEUE msg;

	msg.nArenaID = pArena->m_pArenaBase->nID;
	msg.eGrade = eGrade;

	PendMessage( pPlayer, &msg );
}

void	BattleArenaManager::BroadcastWaitUserCountMessage( const BattleArena* pArena, _BATTLE_GRADE eGrade )
{
	TS_SC_BATTLE_ARENA_UPDATE_WAIT_USER_COUNT msg;

	const BattleArenaWithWaitQueue* pArenaWithWaitQueue = static_cast< const BattleArenaWithWaitQueue* >( pArena );
	if( pArenaWithWaitQueue != NULL )
	{
		const std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >* pWaitQueue = pArenaWithWaitQueue->_getWaitQueue( eGrade );
		if( pWaitQueue != NULL )
		{
			msg.nWaitUserCount = static_cast< int >( pWaitQueue->size() );

			std::vector< BattleArenaWithWaitQueue::PlayerWaitInfo >::const_iterator it = pWaitQueue->begin();
			for( ; it != pWaitQueue->end() ; ++it )
			{
				const BattleArenaWithWaitQueue::PlayerWaitInfo& pwi = (*it);
				StructPlayer* pPlayer = pwi.first;
				if( pPlayer != NULL )
				{
					PendMessage( pPlayer, &msg );
				}
			}
		}
	}
}

void	BattleArenaManager::SendBattleArenaLeaveMessage( const StructPlayer* pPlayer, _ARENA_LEAVE_TYPE eLeaveType )
{
	TS_SC_BATTLE_ARENA_LEAVE msg;

	msg.eLeaveType = eLeaveType;
	s_strcpy( msg.szName, _countof( msg.szName ), pPlayer->GetAlias() );

	PendMessage( pPlayer, &msg );
}

void	BattleArenaManager::SendBattleArenaLeaveMessage( const StructPlayer* pPlayer, const char* pszLeaverName, _ARENA_LEAVE_TYPE eLeaveType )
{
	TS_SC_BATTLE_ARENA_LEAVE msg;

	msg.eLeaveType = eLeaveType;
	s_strcpy( msg.szName, _countof( msg.szName ), pszLeaverName );

	PendMessage( pPlayer, &msg );
}

void	BattleArenaManager::SendBattleArenaBattleInfoMessage( const StructPlayer* pPlayer, const BattleArenaInstance* pBattleInstance )
{
	char szBuffer[ 2048 ] = { 0, };

	TS_SC_BATTLE_ARENA_BATTLE_INFO* pMsg = new ( szBuffer ) TS_SC_BATTLE_ARENA_BATTLE_INFO();

	pMsg->nArenaID = pBattleInstance->m_pArenaBase->nID;

	{
		const BattleArenaInstance::PlayerInfo* pPlayerInfo = pBattleInstance->_getPlayerInfo( pPlayer->GetPlayerUID() );
		if( pPlayerInfo == NULL )
		{
			assert( 0 );
			return;
		}

		pMsg->nForceEnterTime = pPlayerInfo->nPendedEnterTime;
	}

	pMsg->nStartTime = pBattleInstance->m_nStartTime;
	pMsg->eGrade = pBattleInstance->m_eGrade;
	pMsg->nEndTime = pBattleInstance->m_nEndTime;
	pMsg->nPlayerCountPerTeam[ 0 ] = (unsigned char)pBattleInstance->_getTeamMemberCount(0);
	pMsg->nPlayerCountPerTeam[ 1 ] = (unsigned char)pBattleInstance->_getTeamMemberCount(1);

	// 패킷 크기 조정
	pMsg->size += ( pMsg->nPlayerCountPerTeam[ 0 ] + pMsg->nPlayerCountPerTeam[ 1 ] ) * sizeof( TS_SC_BATTLE_ARENA_BATTLE_INFO::PlayerInfo );
	if( pMsg->size > _countof( szBuffer ) )
	{
		assert( 0 );
		return;
	}

	TS_SC_BATTLE_ARENA_BATTLE_INFO::PlayerInfo* pPlayerInfo = reinterpret_cast< TS_SC_BATTLE_ARENA_BATTLE_INFO::PlayerInfo* >( pMsg + 1 );

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = pBattleInstance->_getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			std::vector< BattleArenaInstance::PlayerInfo >::const_iterator it = pPlayerInfoList->begin();
			for( ; it != pPlayerInfoList->end(); ++it )
			{
				const BattleArenaInstance::PlayerInfo& pi = (*it);
				StructPlayer* pPlayer = pi.pPlayer;

				pPlayerInfo->handle = ( pPlayer != NULL ) ? pPlayer->GetHandle() : NULL;
				pPlayerInfo->nJobID = pi.nJobID;

				if( pPlayer != NULL )
					s_strcpy( pPlayerInfo->szName, _countof( pPlayerInfo->szName ), pPlayer->GetAlias() );
				else
					s_strcpy( pPlayerInfo->szName, _countof( pPlayerInfo->szName ), PartyManager::GetInstance().GetMemberDisplayName( pi.nPartyID, pi.nPlayerUID ).c_str() );

				++pPlayerInfo;
			}
		}
	}

	PendMessage( pPlayer, pMsg );
}

void	BattleArenaManager::BroadcastBattleArenaExerciseReadyState( const BattleArenaInstance* pBattleInstance )
{
	TS_SC_BATTLE_ARENA_EXERCISE_READY_STATUS msg;

	msg.nReadyState = atoi( pBattleInstance->_getFlag( FLAG_BATTLE_ARENA_EXERCISE_GAME_READY_STATE ).c_str() );

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = pBattleInstance->_getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			std::vector< BattleArenaInstance::PlayerInfo >::const_iterator it = pPlayerInfoList->begin();
			for( ; it != pPlayerInfoList->end(); ++it )
			{
				const BattleArenaInstance::PlayerInfo& pi = (*it);
				StructPlayer* pPlayer = pi.pPlayer;
				if( pPlayer != NULL )
				{
					PendMessage( pi.pPlayer, &msg );
				}
			}
		}
	}
}

void	BattleArenaManager::BroadcastBattleArenaBattleInfoMessage( const BattleArenaInstance* pBattleInstance )
{
	char szBuffer[ 2048 ] = { 0, };

	TS_SC_BATTLE_ARENA_BATTLE_INFO * pMsg = new ( szBuffer ) TS_SC_BATTLE_ARENA_BATTLE_INFO();

	pMsg->nArenaID = pBattleInstance->m_pArenaBase->nID;
	// pMsg->nForceEnterTime = 0; 각 유저마다 따로 세팅되어 방송해야 함
	pMsg->nStartTime = pBattleInstance->m_nStartTime;
	pMsg->eGrade = pBattleInstance->m_eGrade;
	pMsg->nEndTime = pBattleInstance->m_nEndTime;
	pMsg->nPlayerCountPerTeam[ 0 ] = (unsigned char)pBattleInstance->_getTeamMemberCount(0);
	pMsg->nPlayerCountPerTeam[ 1 ] = (unsigned char)pBattleInstance->_getTeamMemberCount(1);

	// 패킷 크기 조정
	pMsg->size += ( pMsg->nPlayerCountPerTeam[ 0 ] + pMsg->nPlayerCountPerTeam[ 1 ] ) * sizeof( TS_SC_BATTLE_ARENA_BATTLE_INFO::PlayerInfo );
	if( pMsg->size > _countof( szBuffer ) )
	{
		assert( 0 );
		return;
	}

	TS_SC_BATTLE_ARENA_BATTLE_INFO::PlayerInfo* pPlayerInfo = reinterpret_cast< TS_SC_BATTLE_ARENA_BATTLE_INFO::PlayerInfo* >( pMsg + 1 );

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = pBattleInstance->_getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			std::vector< BattleArenaInstance::PlayerInfo >::const_iterator it = pPlayerInfoList->begin();
			for( ; it != pPlayerInfoList->end(); ++it )
			{
				const BattleArenaInstance::PlayerInfo& pi = (*it);
				StructPlayer* pPlayer = pi.pPlayer;

				pPlayerInfo->handle = ( pPlayer ) ? pPlayer->GetHandle() : NULL;
				pPlayerInfo->nJobID = pi.nJobID;
				s_strcpy( pPlayerInfo->szName, _countof( pPlayerInfo->szName ), PartyManager::GetInstance().GetMemberDisplayName( pi.nPartyID, pi.nPlayerUID ).c_str() );
				++pPlayerInfo;
			}
		}
	}

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = pBattleInstance->_getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			std::vector< BattleArenaInstance::PlayerInfo >::const_iterator it = pPlayerInfoList->begin();
			for( ; it != pPlayerInfoList->end(); ++it )
			{
				const BattleArenaInstance::PlayerInfo& pi = (*it);
				StructPlayer* pPlayer = pi.pPlayer;
				if( pPlayer )
				{
					pMsg->nForceEnterTime = pi.nPendedEnterTime;
					PendMessage( pPlayer, pMsg );
				}
			}
		}
	}
}

void	BattleArenaManager::SendBattleArenaBattleStatusMessage( const StructPlayer* pPlayer, const BattleArenaInstance* pBattleInstance )
{
	TS_SC_BATTLE_ARENA_BATTLE_STATUS msg;

	msg.nStatus = atoi( pBattleInstance->_getFlag( FLAG_BATTLE_ARENA_PROP_STATE ).c_str() );

	PendMessage( pPlayer, &msg );
}

void	BattleArenaManager::BroadcastBattleArenaBattleStatusMessage( const BattleArenaInstance* pBattleInstance )
{
	TS_SC_BATTLE_ARENA_BATTLE_STATUS msg;

	msg.nStatus = atoi( pBattleInstance->_getFlag( FLAG_BATTLE_ARENA_PROP_STATE ).c_str() );

	BroadcastBattleArenaMessage( pBattleInstance, &msg );
}

void	BattleArenaManager::SendBattleArenaScoreMessage( const StructPlayer* pPlayer, const BattleArenaInstance* pBattleInstance )
{
	char szBuffer[ 2048 ] = { 0, };

	TS_SC_BATTLE_ARENA_BATTLE_SCORE* pMsg = new ( szBuffer ) TS_SC_BATTLE_ARENA_BATTLE_SCORE();

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		assert( nTeamNo < _countof( pMsg->aTeamScore ) );

		pMsg->aTeamScore[ nTeamNo ].nTotalKillCount = pBattleInstance->m_aTeamInfo[ nTeamNo ].nTotalKillCount;
		pMsg->aTeamScore[ nTeamNo ].nTotalDeathCount = pBattleInstance->m_aTeamInfo[ nTeamNo ].nTotalDeathCount;
		pMsg->aTeamScore[ nTeamNo ].nScore = pBattleInstance->m_aTeamInfo[ nTeamNo ].nScore;
	}

	TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* pPlayerScore = reinterpret_cast< TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* >( pMsg + 1 );

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		pMsg->nPlayerScoreCount += static_cast< unsigned char >( pBattleInstance->_getTeamMemberCount(nTeamNo) );
	}

	pMsg->size += sizeof( TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore ) * pMsg->nPlayerScoreCount;
	if( pMsg->size > _countof( szBuffer ) )
	{
		assert( 0 );
		return;
	}

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = pBattleInstance->_getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			std::vector< BattleArenaInstance::PlayerInfo >::const_iterator it = pPlayerInfoList->begin();
			for( ; it != pPlayerInfoList->end(); ++it )
			{
				const BattleArenaInstance::PlayerInfo& pi = (*it);

				assemblePlayerScoreFromPlayerInfo( pPlayerScore, &pi );

				++pPlayerScore;
			}
		}
	}

	PendMessage( pPlayer, pMsg );
}

void	BattleArenaManager::BroadcastBattleArenaScoreUpdateMessage( const BattleArenaInstance* pBattleInstance, const StructPlayer* pPlayer, int nTeamNo )
{
	char szBuffer[ 2048 ] = { 0, };

	TS_SC_BATTLE_ARENA_BATTLE_SCORE * pMsg = new ( szBuffer ) TS_SC_BATTLE_ARENA_BATTLE_SCORE();

	for( int i = 0; i < pBattleInstance->m_pArenaBase->nTeamCount; ++i )
	{
		assert( i < _countof( pMsg->aTeamScore ) );

		pMsg->aTeamScore[ i ].nTotalKillCount = pBattleInstance->m_aTeamInfo[ i ].nTotalKillCount;
		pMsg->aTeamScore[ i ].nTotalDeathCount = pBattleInstance->m_aTeamInfo[ i ].nTotalDeathCount;
		pMsg->aTeamScore[ i ].nScore = pBattleInstance->m_aTeamInfo[ i ].nScore;
	}

	const BattleArenaInstance::PlayerInfo* pPlayerInfo = ( pPlayer ) ? pBattleInstance->_getPlayerInfo( pPlayer->GetPlayerUID(), nTeamNo ) : NULL;
	if( pPlayerInfo != NULL )
	{
		pMsg->nPlayerScoreCount = 1;
		pMsg->size += sizeof( TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore );

		TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* pPlayerScore = reinterpret_cast< TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* >( pMsg + 1 );

		assemblePlayerScoreFromPlayerInfo( pPlayerScore, pPlayerInfo );
	}

	BroadcastBattleArenaMessage( pBattleInstance, pMsg );
}

void	BattleArenaManager::BroadcastBattleArenaScoreUpdateMessage( const BattleArenaInstance* pBattleInstance,
																   const StructPlayer* pPlayer1, int nTeamNo1,
																   const StructPlayer* pPlayer2, int nTeamNo2 )
{
	char szBuffer[ 2048 ] = { 0, };

	TS_SC_BATTLE_ARENA_BATTLE_SCORE * pMsg = new ( szBuffer ) TS_SC_BATTLE_ARENA_BATTLE_SCORE();

	for( int i = 0; i < pBattleInstance->m_pArenaBase->nTeamCount; ++i )
	{
		assert( i < _countof( pMsg->aTeamScore ) );

		pMsg->aTeamScore[ i ].nTotalKillCount = pBattleInstance->m_aTeamInfo[ i ].nTotalKillCount;
		pMsg->aTeamScore[ i ].nTotalDeathCount = pBattleInstance->m_aTeamInfo[ i ].nTotalDeathCount;
		pMsg->aTeamScore[ i ].nScore = pBattleInstance->m_aTeamInfo[ i ].nScore;
	}

	TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* pPlayerScore = reinterpret_cast< TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* >( pMsg + 1 );

	// 누가 누굴 죽인 경우 두 명의 점수 정보를 동시에 갱신해야 하므로 두 명짜리 함수도 넣었음 ` `;
	// pPlayer1
	{
		const BattleArenaInstance::PlayerInfo* pPlayerInfo = ( pPlayer1 ) ? pBattleInstance->_getPlayerInfo( pPlayer1->GetPlayerUID(), nTeamNo1 ) : NULL;
		if( pPlayerInfo != NULL )
		{
			++pMsg->nPlayerScoreCount;
			pMsg->size += sizeof( TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore );

			assemblePlayerScoreFromPlayerInfo( pPlayerScore, pPlayerInfo );
			++pPlayerScore;
		}
	}

	// pPlayer2
	{
		const BattleArenaInstance::PlayerInfo* pPlayerInfo = ( pPlayer2 ) ? pBattleInstance->_getPlayerInfo( pPlayer2->GetPlayerUID(), nTeamNo2 ) : NULL;
		if( pPlayerInfo != NULL )
		{
			++pMsg->nPlayerScoreCount;
			pMsg->size += sizeof( TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore );

			assemblePlayerScoreFromPlayerInfo( pPlayerScore, pPlayerInfo );
			++pPlayerScore;
		}
	}

	BroadcastBattleArenaMessage( pBattleInstance, pMsg );
}

void	BattleArenaManager::BroadcastBattleArenaJoinBattleMessage( const BattleArenaInstance* pBattleInstance, const StructPlayer* pPlayer )
{
	int nTeamNo = pBattleInstance->_getTeamNo( pPlayer->GetPartyID() );
	if( nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO )
	{
		assert( 0 );
		return;
	}

	TS_SC_BATTLE_ARENA_JOIN_BATTLE msg;

	msg.nTeamNo = nTeamNo;
	msg.handle = pPlayer->GetHandle();
	msg.nJobID = pPlayer->GetJobId();
	s_strcpy( msg.szName, _countof( msg.szName ), pPlayer->GetAlias() );

	BroadcastBattleArenaMessage( pBattleInstance, &msg );
}

void	BattleArenaManager::BroadcastBattleArenaLeaveBattleMessageWithLog( const BattleArenaInstance* pBattleInstance,
																		  const BattleArenaInstance::PlayerInfo* pPlayerInfo,
																		  const StructPlayer* pPlayer,
																		  const int nWinTeamNo,
																		  const int nTeamNo,
																		  const char* pszLeaverName,
																		  _ARENA_LEAVE_TYPE eLeaveType )
{
	const char* pAccountName = "";
	const char* pPlayerName = "";
	int nAccountID = 0;

	const char* pTitle = "";
	int nWinType = 2;	// 승리 여부(0: 승리, 1:패배, 2: 무승부, 3 : 불안정 승리)
	if( nTeamNo != INVALID_BATTLE_ARENA_TEAM_NO && nWinTeamNo != INVALID_BATTLE_ARENA_TEAM_NO )
	{
		if( nWinTeamNo == nTeamNo )
		{
			nWinType = 0;
		}
		else
		{
			nWinType = 1;
		}

		if( nWinType == 0 )
		{
			if( pBattleInstance->m_eRewardType != ART_FULL_REWARD )
			{
				nWinType = 3;
			}
		}
	}

	if( pPlayer )
	{
		nAccountID = pPlayer->GetAccountID();
		pAccountName = pPlayer->GetAccountName();
		pPlayerName = pPlayer->GetName();

		const TitleBaseServer* pMainTitle = const_cast< StructPlayer* >( pPlayer )->GetMainTitle();
		if( pMainTitle != NULL )
		{
			pTitle = GameContent::GetString( pMainTitle->nNameID );
		}
	}

	int nTotalGainAP = pPlayerInfo->nTotalGainAPByKill + pPlayerInfo->nTotalGainAPByProp + pPlayerInfo->nTotalGainAPByResult;
	int nTeamMemberCount = (nTeamNo == INVALID_BATTLE_ARENA_TEAM_NO) ? 0 : pBattleInstance->_getTeamMemberCount(nTeamNo);

	LOG::Log11N4S( LM_BATTLE_ARENA_LEAVE_BATTLE,
		nAccountID, pPlayerInfo->nPlayerUID,
		pBattleInstance->m_pArenaBase->nID, pBattleInstance->m_nInstanceNo, pBattleInstance->m_eGrade,
		nTeamNo, nTeamMemberCount, pPlayerInfo->nPartyID, eLeaveType,
		0, 0,
		pAccountName, LOG::STR_NTS, pPlayerName, LOG::STR_NTS,
		"", LOG::STR_NTS,
		pszLeaverName, LOG::STR_NTS );

	LOG::Log11N4S( LM_BATTLE_ARENA_REWARD,
		nAccountID, pPlayerInfo->nPlayerUID,
		pBattleInstance->m_pArenaBase->nID, pBattleInstance->m_nInstanceNo, pBattleInstance->m_eGrade,
		nTotalGainAP, static_cast< int >( pPlayerInfo->bIsMVP ), nWinType, eLeaveType, pPlayerInfo->nKillCount, pPlayerInfo->nPropActivateCount,
		pAccountName, LOG::STR_NTS, pPlayerName, LOG::STR_NTS,
		pTitle, LOG::STR_NTS,
		pszLeaverName, LOG::STR_NTS );

	TS_SC_BATTLE_ARENA_LEAVE msg;

	msg.eLeaveType = eLeaveType;
	s_strcpy( msg.szName, _countof( msg.szName ), pszLeaverName );

	BroadcastBattleArenaMessage( pBattleInstance, &msg );
}

void	BattleArenaManager::BroadcastBattleArenaDisconnectBattleMessage( const BattleArenaInstance* pBattleInstance, const StructPlayer* pPlayer )
{
	TS_SC_BATTLE_ARENA_DISCONNECT_BATTLE msg;

	msg.handle = pPlayer->GetHandle();

	BroadcastBattleArenaMessage( pBattleInstance, &msg );
}

void	BattleArenaManager::BroadcastBattleArenaReconnectBattleMessage( const BattleArenaInstance* pBattleInstance, const StructPlayer* pPlayer )
{
	TS_SC_BATTLE_ARENA_RECONNECT_BATTLE msg;

	s_strcpy( msg.szName, _countof( msg.szName ), pPlayer->GetAlias() );
	msg.newHandle = pPlayer->GetHandle();

	BroadcastBattleArenaMessage( pBattleInstance, &msg );
}

void	BattleArenaManager::BroadcastBattleArenaFinalScoreMessage( const BattleArenaInstance* pBattleInstance )
{
	char szBuffer[ 2048 ] = { 0, };

	TS_SC_BATTLE_ARENA_BATTLE_SCORE* pMsg = new ( szBuffer ) TS_SC_BATTLE_ARENA_BATTLE_SCORE();

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		assert( nTeamNo < _countof( pMsg->aTeamScore ) );

		pMsg->aTeamScore[ nTeamNo ].nTotalKillCount = pBattleInstance->m_aTeamInfo[ nTeamNo ].nTotalKillCount;
		pMsg->aTeamScore[ nTeamNo ].nTotalDeathCount = pBattleInstance->m_aTeamInfo[ nTeamNo ].nTotalDeathCount;
		pMsg->aTeamScore[ nTeamNo ].nScore = pBattleInstance->m_aTeamInfo[ nTeamNo ].nScore;
	}

	TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* pPlayerScore = reinterpret_cast< TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore* >( pMsg + 1 );

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		pMsg->nPlayerScoreCount += static_cast< unsigned char >( pBattleInstance->_getTeamMemberCount( nTeamNo ) );
	}

	pMsg->size += sizeof( TS_SC_BATTLE_ARENA_BATTLE_SCORE::PlayerScore ) * pMsg->nPlayerScoreCount;
	if( pMsg->size > sizeof( szBuffer ) )
	{
		assert( 0 );
		return;
	}

	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount; ++nTeamNo )
	{
		const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = pBattleInstance->_getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			std::vector< BattleArenaInstance::PlayerInfo >::const_iterator it = pPlayerInfoList->begin();
			for( ; it != pPlayerInfoList->end(); ++it )
			{
				const BattleArenaInstance::PlayerInfo& pi = (*it);

				assemblePlayerScoreFromPlayerInfo( pPlayerScore, &pi );

				++pPlayerScore;
			}
		}
	}

	BroadcastBattleArenaMessage( pBattleInstance, pMsg );
}

void	BattleArenaManager::BroadcastBattleArenaResultWithLog( const BattleArenaInstance* pBattleInstance,
															  _ARENA_END_TYPE eEndType, _ARENA_REWARD_TYPE eRewardType,
															  int nWinnerTeamNo, int nScore1, int nScore2, AR_TIME nPlayTime )
{
	LOG::Log11N4S( LM_BATTLE_ARENA_END, 0, 0,
		pBattleInstance->m_pArenaBase->nID, pBattleInstance->m_nInstanceNo, pBattleInstance->m_eGrade,
		eEndType, eRewardType,
		nWinnerTeamNo, nScore1, nScore2, nPlayTime,
		"", LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );

	char szBuffer[ 2048 ] = { 0, };

	TS_SC_BATTLE_ARENA_RESULT* pMsg = new ( szBuffer ) TS_SC_BATTLE_ARENA_RESULT();

	pMsg->eEndType = eEndType;
	pMsg->eRewardType = eRewardType;
	pMsg->nWinnerTeamNo = nWinnerTeamNo;

	char* pMVPName = szBuffer + sizeof( TS_SC_BATTLE_ARENA_RESULT );
	for( int nTeamNo = 0; nTeamNo < pBattleInstance->m_pArenaBase->nTeamCount ; ++nTeamNo )
	{
		const std::vector< BattleArenaInstance::PlayerInfo >* pPlayerInfoList = pBattleInstance->_getPlayerInfoList( nTeamNo );
		if( pPlayerInfoList != NULL )
		{
			std::vector< BattleArenaInstance::PlayerInfo >::const_iterator it = pPlayerInfoList->begin();
			for( ; it != pPlayerInfoList->end(); ++it )
			{
				const BattleArenaInstance::PlayerInfo& pi = (*it);

				if( !pi.bIsMVP )
					continue;

				++pMsg->nMVPCount;
				pMsg->size += 19;

				if( pi.pPlayer )
					s_strcpy( pMVPName, 19, pi.pPlayer->GetAlias() );
				else
					s_strcpy( pMVPName, 19, PartyManager::GetInstance().GetMemberDisplayName( pi.nPartyID, pi.nPlayerUID ).c_str() );

				pMVPName += 19;
			}
		}
	}

	BroadcastBattleArenaMessage( pBattleInstance, pMsg );
}

void	BattleArenaManager::SendBattleArenaAbsenceCheck( const StructPlayer* pPlayer, AR_TIME nLimitTime )
{
	TS_SC_BATTLE_ARENA_ABSENCE_CHECK msg;

	msg.nLimitTime = nLimitTime;

	PendMessage( pPlayer, &msg );
}

void	BattleArenaManager::SendBattleArenaPenaltyMessage( const struct StructPlayer* pPlayer )
{
	TS_SC_BATTLE_ARENA_PENALTY_INFO msg;

	msg.nBlockTime = pPlayer->GetBattleArenaBlockTimeLeft();
	msg.nPenaltyCount = pPlayer->GetBattleArenaPenaltyCount();

	time_t tPenaltyDec = pPlayer->GetBattleArenaPenaltyDecTime();
	time_t t = time( NULL );
	if( tPenaltyDec && t < tPenaltyDec )
	{
		msg.nPenaltyCountDecTime = ( tPenaltyDec - t ) * 100;
	}

	PendMessage( pPlayer, &msg );
}
