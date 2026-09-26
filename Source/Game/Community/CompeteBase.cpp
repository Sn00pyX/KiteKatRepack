
#include <mmo/ArcadiaServer.h>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "CompeteBase.h"
#include "GameRule.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "SendMessage.h"


volatile LONG CompeteInfoBase::nMaxUsedCompeteID = 1;


void assembleCompeteEndMessage( struct TS_SC_COMPETE_END * pMsg, const unsigned char nCompeteType, const unsigned char nEndType, const char * szWinner, const char * szLoser )
{
	pMsg->compete_type = nCompeteType;
	pMsg->end_type = nEndType;
	s_strcpy( pMsg->winner, _countof( pMsg->winner ), szWinner );
	s_strcpy( pMsg->loser, _countof( pMsg->loser ), szLoser );
}

/********************* PlayerCompeteInfo *********************/
void PlayerCompeteInfo::OnAnswerRequest( const _COMPETE_ANSWER_TYPE & eAnswer )
{
	StructPlayer * pRequester = GetRequesterAsPlayer();
	StructPlayer * pRequestee = GetRequesteeAsPlayer();

	// 거절 처리
	if( eAnswer != COMPETE_ANSWER_TYPE_ACCEPT )
	{
		pRequestee->SetCompeteID( 0 );
		pRequester->SetCompeteID( 0 );

		SendCompeteAnswer( pRequester, GetCompeteType(), eAnswer, pRequestee->GetName() );
	}
	// 수락 처리(카운트 다운 시작)
	else
	{
		eState = COMPETE_STATE_COUNTDOWN;
		tNextStateChange = time( NULL ) + GameRule::COMPETE_COUNTDOWN_DURATION;
		posCompeteCenter = pRequester->GetPos();

		pRequestee->PendClearStateWithFlag( StateInfo::ERASE_ON_COMPETE_START );
		pRequester->PendClearStateWithFlag( StateInfo::ERASE_ON_COMPETE_START );

		SendCompeteCountdown( pRequestee, GetCompeteType(), pRequester->GetName(), pRequester->GetHandle() );
		SendCompeteCountdown( pRequester, GetCompeteType(), pRequestee->GetName(), pRequestee->GetHandle() );
	}

	LOG::Log11N4S( LM_COMPETE_ANSWER, pRequestee->GetAccountID(), pRequestee->GetSID(), pRequestee->GetX(), pRequestee->GetY(), pRequestee->GetLayer(),
		pRequester->GetX(), pRequester->GetY(), pRequester->GetLayer(), 0, eAnswer, nID,
		pRequestee->GetAccountName(), LOG::STR_NTS, pRequestee->GetName(), LOG::STR_NTS, pRequester->GetAccountName(), LOG::STR_NTS, pRequester->GetName(), LOG::STR_NTS );
}

void PlayerCompeteInfo::OnAnswerTimeout()
{
	StructPlayer * pRequestee = GetRequesteeAsPlayer();
	if( pRequestee )
	{
		pRequestee->SetCompeteID( 0 );
		SendCompeteAnswer( pRequestee, GetCompeteType(), COMPETE_ANSWER_TYPE_REJECT_BY_TIMEOUT, "" );
	}
	StructPlayer * pRequester = GetRequesterAsPlayer();
	if( pRequester )
	{
		pRequester->SetCompeteID( 0 );
		SendCompeteAnswer( pRequester, GetCompeteType(), COMPETE_ANSWER_TYPE_REJECT_BY_TIMEOUT, ( pRequestee ) ? pRequestee->GetName() : "" );
	}

	if( pRequestee )
	{
		LOG::Log11N4S( LM_COMPETE_ANSWER, pRequestee->GetAccountID(), pRequestee->GetSID(), pRequestee->GetX(), pRequestee->GetY(), pRequestee->GetLayer(),
			( pRequester ) ? pRequester->GetX() : 0, ( pRequester ) ? pRequester->GetY() : 0, ( pRequester ) ? pRequester->GetLayer() : 0, 0, COMPETE_ANSWER_TYPE_REJECT_BY_TIMEOUT, nID,
			pRequestee->GetAccountName(), LOG::STR_NTS, pRequestee->GetName(), LOG::STR_NTS,
			( pRequester ) ? pRequester->GetAccountName() : "", LOG::STR_NTS, ( pRequester ) ? pRequester->GetName() : "", LOG::STR_NTS );
	}
}

void PlayerCompeteInfo::OnEndCountdown()
{
	eState = COMPETE_STATE_COMPETING;
	tNextStateChange = time( NULL ) + GameRule::COMPETE_DURATION;

	StructPlayer * pRequestee = GetRequesteeAsPlayer();
	StructPlayer * pRequester = GetRequesterAsPlayer();

	SendCompeteStart( pRequestee, COMPETE_TYPE_VS_PLAYER, GetCompetitorName( true ) );
	SendCompeteStart( pRequester, COMPETE_TYPE_VS_PLAYER, GetCompetitorName( false ) );

	// 대련 유저 2명 각각 로그를 각각 남김
	if( pRequestee )
	{
		LOG::Log11N4S( LM_COMPETE_BEGIN, pRequestee->GetAccountID(), pRequestee->GetSID(), pRequestee->GetX(), pRequestee->GetY(), pRequestee->GetLayer(),
			( pRequester ) ? pRequester->GetX() : 0, ( pRequester ) ? pRequester->GetY() : 0, ( pRequester ) ? pRequester->GetLayer() : 0, 0, 0, nID,
			pRequestee->GetAccountName(), LOG::STR_NTS, pRequestee->GetName(), LOG::STR_NTS,
			( pRequester ) ? pRequester->GetAccountName() : "", LOG::STR_NTS, ( pRequester ) ? pRequester->GetName() : "", LOG::STR_NTS );
	}
	if( pRequester )
	{
		LOG::Log11N4S( LM_COMPETE_BEGIN, pRequester->GetAccountID(), pRequester->GetSID(), pRequester->GetX(), pRequester->GetY(), pRequester->GetLayer(),
			( pRequestee ) ? pRequestee->GetX() : 0, ( pRequestee ) ? pRequestee->GetY() : 0, ( pRequestee ) ? pRequestee->GetLayer() : 0, 0, 0, nID,
			pRequester->GetAccountName(), LOG::STR_NTS, pRequester->GetName(), LOG::STR_NTS,
			( pRequestee ) ? pRequestee->GetAccountName() : "", LOG::STR_NTS, ( pRequestee ) ? pRequestee->GetName() : "", LOG::STR_NTS );
	}
}

void PlayerCompeteInfo::OnRetire( const void * pRetirer, const _COMPETE_END_TYPE & eEndType )
{
	StructPlayer * pWinPlayer = NULL;
	StructPlayer * pRetirePlayer = NULL;
	if( *static_cast< const AR_HANDLE * >( pRetirer ) == *static_cast< const AR_HANDLE * >( GetRequestee() ) )
	{
		pWinPlayer = GetRequesterAsPlayer();
		pRetirePlayer = GetRequesteeAsPlayer();
	}
	else
	{
		assert( *static_cast< const AR_HANDLE * >( pRetirer ) == *static_cast< const AR_HANDLE * >( GetRequester() ) );

		pWinPlayer = GetRequesteeAsPlayer();
		pRetirePlayer = GetRequesterAsPlayer();
	}

	TS_SC_COMPETE_END msg;
	assembleCompeteEndMessage( &msg, GetCompeteType(), eEndType, pWinPlayer->GetName(), pRetirePlayer->GetName() );
	PendMessage( pWinPlayer, &msg );
	PendMessage( pRetirePlayer, &msg );

	pWinPlayer->SetCompeteID( 0 );
	pRetirePlayer->SetCompeteID( 0 );

	// 대련 종료 시 버프 제거 처리 추가(대련 종료 후 디버프에 의한 사망 방지)
	if( eState == COMPETE_STATE_COMPETING )
	{
		pWinPlayer->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
		StructSummon * pSummon = pWinPlayer->GetMainSummon();
		if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
		pSummon = pWinPlayer->GetSubSummon();
		if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );

		pRetirePlayer->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
		pSummon = pRetirePlayer->GetMainSummon();
		if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
		pSummon = pRetirePlayer->GetSubSummon();
		if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
	}

	// 대련 유저 2명 각각 로그를 각각 남김
	LOG::Log11N4S( LM_COMPETE_END, pWinPlayer->GetAccountID(), pWinPlayer->GetSID(), pWinPlayer->GetX(), pWinPlayer->GetY(), pWinPlayer->GetLayer(),
		pRetirePlayer->GetX(), pRetirePlayer->GetY(), pRetirePlayer->GetLayer(), true, eEndType, nID,
		pWinPlayer->GetAccountName(), LOG::STR_NTS, pWinPlayer->GetName(), LOG::STR_NTS, pRetirePlayer->GetAccountName(), LOG::STR_NTS, pRetirePlayer->GetName(), LOG::STR_NTS );

	LOG::Log11N4S( LM_COMPETE_END, pRetirePlayer->GetAccountID(), pRetirePlayer->GetSID(), pRetirePlayer->GetX(), pRetirePlayer->GetY(), pRetirePlayer->GetLayer(),
		pWinPlayer->GetX(), pWinPlayer->GetY(), pWinPlayer->GetLayer(), false, eEndType, nID,
		pRetirePlayer->GetAccountName(), LOG::STR_NTS, pRetirePlayer->GetName(), LOG::STR_NTS, pWinPlayer->GetAccountName(), LOG::STR_NTS, pWinPlayer->GetName(), LOG::STR_NTS );
}

void PlayerCompeteInfo::OnCompeteTimeout()
{
	StructPlayer * pRequestee = GetRequesteeAsPlayer();
	StructPlayer * pRequester = GetRequesterAsPlayer();

	if( pRequestee )
	{
		pRequestee->SetCompeteID( 0 );
		SendCompeteEnd( pRequestee, COMPETE_TYPE_VS_PLAYER, COMPETE_END_BY_TIMEOUT, pRequestee->GetName(), pRequestee->GetName() );

		// 대련 종료 시 버프 제거 처리 추가(대련 종료 후 디버프에 의한 사망 방지)
		if( eState == COMPETE_STATE_COMPETING )
		{
			pRequestee->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
			StructSummon * pSummon = pRequestee->GetMainSummon();
			if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
			pSummon = pRequestee->GetSubSummon();
			if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
		}
	}

	if( pRequester )
	{
		pRequester->SetCompeteID( 0 );
		SendCompeteEnd( pRequester, COMPETE_TYPE_VS_PLAYER, COMPETE_END_BY_TIMEOUT, pRequestee->GetName(), pRequestee->GetName() );

		// 대련 종료 시 버프 제거 처리 추가(대련 종료 후 디버프에 의한 사망 방지)
		if( eState == COMPETE_STATE_COMPETING )
		{
			pRequester->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
			StructSummon * pSummon = pRequester->GetMainSummon();
			if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
			pSummon = pRequester->GetSubSummon();
			if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
		}
	}

	// 대련 유저 2명 각각 로그를 각각 남김
	if( pRequestee )
	{
		LOG::Log11N4S( LM_COMPETE_END, pRequestee->GetAccountID(), pRequestee->GetSID(), pRequestee->GetX(), pRequestee->GetY(), pRequestee->GetLayer(),
			( pRequester ) ? pRequester->GetX() : 0, ( pRequester ) ? pRequester->GetY() : 0, ( pRequester ) ? pRequester->GetLayer() : 0, false, COMPETE_END_BY_TIMEOUT, nID,
			pRequestee->GetAccountName(), LOG::STR_NTS, pRequestee->GetName(), LOG::STR_NTS,
			( pRequester ) ? pRequester->GetAccountName() : "", LOG::STR_NTS, ( pRequester ) ? pRequester->GetName() : "", LOG::STR_NTS );
	}
	if( pRequester )
	{
		LOG::Log11N4S( LM_COMPETE_END, pRequester->GetAccountID(), pRequester->GetSID(), pRequester->GetX(), pRequester->GetY(), pRequester->GetLayer(),
			( pRequestee ) ? pRequestee->GetX() : 0, ( pRequestee ) ? pRequestee->GetY() : 0, ( pRequestee ) ? pRequestee->GetLayer() : 0, false, COMPETE_END_BY_TIMEOUT, nID,
			pRequester->GetAccountName(), LOG::STR_NTS, pRequester->GetName(), LOG::STR_NTS,
			( pRequestee ) ? pRequestee->GetAccountName() : "", LOG::STR_NTS, ( pRequestee ) ? pRequestee->GetName() : "", LOG::STR_NTS );
	}
}

const unsigned short PlayerCompeteInfo::Validate( const bool bNotifyEndOnError, const bool bIncludeRequestee, const bool bIncludeRequester, _COMPETE_END_TYPE * peError, bool * pbRequesterWin )
{
	StructPlayer * pCheckTarget = NULL;
	StructPlayer * pOpponent = NULL;

	// 1회째 체크는 Requestee, 2회째 체크는 Requester
	for( int i = 0 ; i < 2 ; ++i )
	{
		if( i == 0 )
		{
			if( !bIncludeRequestee )
				continue;

			pCheckTarget = GetRequesteeAsPlayer();
			pOpponent = GetRequesterAsPlayer();
		}
		if( i == 1 )
		{
			if( !bIncludeRequester )
				continue;

			pCheckTarget = GetRequesterAsPlayer();
			pOpponent = GetRequesteeAsPlayer();
		}

		if( !pCheckTarget )
		{
			if( pOpponent )
			{
				if( bNotifyEndOnError )
					SendCompeteEnd( pOpponent, GetCompeteType(), COMPETE_END_BY_LOGOUT, pOpponent->GetName(), pOpponent->GetName() );
				pOpponent->SetCompeteID( 0 );
			}
			if( pCheckTarget )
			{
				if( bNotifyEndOnError )
					SendCompeteEnd( pCheckTarget, GetCompeteType(), COMPETE_END_BY_LOGOUT, pCheckTarget->GetName(), pCheckTarget->GetName() );
				pCheckTarget->SetCompeteID( 0 );
			}

			if( peError ) *peError = COMPETE_END_BY_LOGOUT;
			if( pbRequesterWin ) *pbRequesterWin = ( pCheckTarget == GetRequesteeAsPlayer() );
			return ( pCheckTarget == GetRequesteeAsPlayer() ) ? RESULT_LIMIT_TARGET : RESULT_NOT_EXIST;
		}
		if( pCheckTarget->GetCompeteID() != nID )
		{
			if( pOpponent )
			{
				if( bNotifyEndOnError )
					SendCompeteEnd( pOpponent, GetCompeteType(), COMPETE_END_BY_LOGOUT, pOpponent->GetName(), pOpponent->GetName() );
				pOpponent->SetCompeteID( 0 );
			}
			if( pCheckTarget )
			{
				if( bNotifyEndOnError )
					SendCompeteEnd( pCheckTarget, GetCompeteType(), COMPETE_END_BY_LOGOUT, pCheckTarget->GetName(), pCheckTarget->GetName() );
				pCheckTarget->SetCompeteID( 0 );
			}

			if( peError ) *peError = COMPETE_END_BY_LOGOUT;
			if( pbRequesterWin ) *pbRequesterWin = ( pCheckTarget == GetRequesteeAsPlayer() );
			return ( pCheckTarget == GetRequesteeAsPlayer() ) ? RESULT_TARGET_ALREADY_IN_COMPETE : RESULT_ALREADY_IN_COMPETE;
		}
		if( !pCheckTarget->IsInField() && !pCheckTarget->IsInBattleField() )
		{
			if( bNotifyEndOnError )
			{
				TS_SC_COMPETE_END msg;
				assembleCompeteEndMessage( &msg, GetCompeteType(), COMPETE_END_BY_ENTERING_SAFETY_ZONE, pOpponent->GetName(), pCheckTarget->GetName() );

				PendMessage( pCheckTarget, &msg );
				PendMessage( pOpponent, &msg );
			}

			pCheckTarget->SetCompeteID( 0 );
			pOpponent->SetCompeteID( 0 );

			if( peError ) *peError = COMPETE_END_BY_ENTERING_SAFETY_ZONE;
			if( pbRequesterWin ) *pbRequesterWin = ( pCheckTarget == GetRequesteeAsPlayer() );
			return ( pCheckTarget == GetRequesteeAsPlayer() ) ? RESULT_TARGET_NOT_IN_COMPETIBLE_PLACE : RESULT_NOT_IN_COMPETIBLE_PLACE;
		}
		// 대련 진행 단계에서 대련 시작점으로부터의 거리 체크
		if( ( eState == COMPETE_STATE_COUNTDOWN || eState == COMPETE_STATE_COMPETING ) &&
			( posCompeteCenter.x || posCompeteCenter.y ) && pCheckTarget->GetPos().GetDistance( posCompeteCenter ) > GameRule::COMPETE_RING_SEMIDIAMETER )
		{
			if( bNotifyEndOnError )
			{
				TS_SC_COMPETE_END msg;
				assembleCompeteEndMessage( &msg, GetCompeteType(), COMPETE_END_BY_RINGOUT, pOpponent->GetName(), pCheckTarget->GetName() );

				PendMessage( pCheckTarget, &msg );
				PendMessage( pOpponent, &msg );
			}

			pCheckTarget->SetCompeteID( 0 );
			pOpponent->SetCompeteID( 0 );

			if( peError ) *peError = COMPETE_END_BY_RINGOUT;
			if( pbRequesterWin ) *pbRequesterWin = ( pCheckTarget == GetRequesteeAsPlayer() );
			return ( pCheckTarget == GetRequesteeAsPlayer() ) ? RESULT_TARGET_NOT_IN_COMPETIBLE_PLACE : RESULT_NOT_IN_COMPETIBLE_PLACE;
		}
	}

	// 신청 수락 단계에서의 거리 체크(Requestee/Requester의 2회 체크가 불필요함)
	if( eState == COMPETE_STATE_REQUESTING )
	{
		if( pCheckTarget->GetPos().GetDistance( pOpponent->GetPos() ) > GameRule::COMPETE_RING_SEMIDIAMETER )
		{
			if( bNotifyEndOnError )
			{
				TS_SC_COMPETE_END msg;
				assembleCompeteEndMessage( &msg, GetCompeteType(), COMPETE_END_BY_RINGOUT, pOpponent->GetName(), pCheckTarget->GetName() );

				PendMessage( pCheckTarget, &msg );
				PendMessage( pOpponent, &msg );
			}

			pCheckTarget->SetCompeteID( 0 );
			pOpponent->SetCompeteID( 0 );

			if( peError ) *peError = COMPETE_END_BY_RINGOUT;
			return ( pCheckTarget == GetRequesteeAsPlayer() ) ? RESULT_TARGET_NOT_IN_COMPETIBLE_PLACE : RESULT_NOT_IN_COMPETIBLE_PLACE;
		}
	}


	return RESULT_SUCCESS;
}

const char * PlayerCompeteInfo::GetCompetitorName( const bool bRequester ) const
{
	if( bRequester && GetRequesterAsPlayer() )
		return GetRequesterAsPlayer()->GetName();
	else if( !bRequester && GetRequesteeAsPlayer() )
		return GetRequesteeAsPlayer()->GetName();

	return "";
}

const StructPlayer * PlayerCompeteInfo::GetRequesteeAsPlayer() const
{
	return *StructPlayer::get( hRequestee );
}

StructPlayer * PlayerCompeteInfo::GetRequesteeAsPlayer()
{
	return *StructPlayer::get( hRequestee );
}

const StructPlayer * PlayerCompeteInfo::GetRequesterAsPlayer() const
{
	return *StructPlayer::get( hRequester );
}

StructPlayer * PlayerCompeteInfo::GetRequesterAsPlayer()
{
	return *StructPlayer::get( hRequester );
}
/********************* PlayerCompeteInfo *********************/

/********************* PartyCompeteInfo *********************/
const char * PartyCompeteInfo::GetCompetitorName( const bool bRequester ) const
{
	// PartyManager::GetPartyName이 std::string의 인스턴스를 리턴하므로 c_str() 함수의 결과를 이 함수의 리턴값으로 쓸 수 없음
	static __declspec( thread ) char szName[ 31 ];
	s_strcpy( szName, _countof( szName ), PartyManager::GetInstance().GetPartyName( ( bRequester ) ? nRequester : nRequestee ).c_str() );
	return szName;
}
/********************* PartyCompeteInfo *********************/

/********************* GuildCompeteInfo *********************/
const char * GuildCompeteInfo::GetCompetitorName( const bool bRequester ) const
{
	// GuildManager::GetGuildName이 std::string의 인스턴스를 리턴하므로 c_str() 함수의 결과를 이 함수의 리턴값으로 쓸 수 없음
	static __declspec( thread ) char szName[ 31 ];
	s_strcpy( szName, _countof( szName ), GuildManager::GetInstance().GetGuildName( ( bRequester ) ? nRequester : nRequestee ).c_str() );
	return szName;
}
/********************* GuildCompeteInfo *********************/

/********************* AllianceCompeteInfo *********************/
const char * AllianceCompeteInfo::GetCompetitorName( const bool bRequester ) const
{
	// GuildManager::GetAllianceName이 std::string의 인스턴스를 리턴하므로 c_str() 함수의 결과를 이 함수의 리턴값으로 쓸 수 없음
	static __declspec( thread ) char szName[ 31 ];
	s_strcpy( szName, _countof( szName ), GuildManager::GetInstance().GetAllianceName( ( bRequester ) ? nRequester : nRequestee ).c_str() );
	return szName;
}
/********************* AllianceCompeteInfo *********************/
