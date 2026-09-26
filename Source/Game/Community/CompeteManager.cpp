
#include <string>

#include <mmo/ArcadiaServer.h>
#include <toolkit/XEnv.h>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "CompeteManager.h"
#include "StructSummon.h"
#include "SendMessage.h"
#include "GameRule.h"
#include "PartyManager.h"
#include "GuildManager.h"


CompeteManager::~CompeteManager()
{
	THREAD_SYNCHRONIZE( m_csCompete );

	for( std::vector< CompeteInfoBase * >::const_iterator it = m_vCompeteInfo.begin() ; it != m_vCompeteInfo.end() ; ++it )
	{
		delete (*it);
	}
	m_vCompeteInfo.clear();
}

void CompeteManager::Init()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_HIGH );
}

void CompeteManager::DeInit()
{
	ArcadiaServer::Instance().SetObjectPriority( this, ArSchedulerObject::UPDATE_PRIORITY_IDLE );
}

const unsigned short CompeteManager::RequestCompeteToPlayer( struct StructPlayer * pRequester, struct StructPlayer * pRequestee )
{
	THREAD_SYNCHRONIZE( m_csCompete );

	// 도전자가 신청 가능한 상태인지 확인
	unsigned int nCompeteID = pRequester->GetCompeteID();
	if( nCompeteID )
	{
		CompeteInfoBase * pCompeteInfo = getCompeteInfo( nCompeteID );
		if( pCompeteInfo )
		{
			if( pCompeteInfo->eState == COMPETE_STATE_REQUESTING )
				return RESULT_WAITING_COMPETE_REQUEST_ANSWER;
			else
				return RESULT_ALREADY_IN_COMPETE;
		}

		assert( 0 );
		pRequester->SetCompeteID( 0 );
	}

	if( !( pRequester->IsInField() || pRequester->IsInBattleField() ) || pRequester->IsInDeathmatch() || pRequester->IsInRamadanPrayerHall() )
		return RESULT_NOT_IN_COMPETIBLE_PLACE;

	// 대상이 신청 가능한 상태인지 확인
	nCompeteID = pRequestee->GetCompeteID();
	if( nCompeteID )
	{
		CompeteInfoBase * pCompeteInfo = getCompeteInfo( nCompeteID );
		if( pCompeteInfo )
		{
			if( pCompeteInfo->eState == COMPETE_STATE_REQUESTING )
				return RESULT_TARGET_WAITING_COMPETE_REQUEST_ANSWER;
			else
				return RESULT_TARGET_ALREADY_IN_COMPETE;
		}

		assert( 0 );
		pRequestee->SetCompeteID( 0 );
	}

	if( !( pRequestee->IsInField() || pRequestee->IsInBattleField() ) || pRequestee->IsInDeathmatch() || pRequestee->IsInRamadanPrayerHall() )
		return RESULT_TARGET_NOT_IN_COMPETIBLE_PLACE;

	// 대련 신청 시점의 거리 체크
	if( pRequestee->GetPos().GetDistance( pRequester->GetPos() ) > GameRule::COMPETE_RING_SEMIDIAMETER )
	{
		return RESULT_TOO_FAR;
	}

	// 대련 정보 추가
	AR_HANDLE hRequester = pRequester->GetHandle();
	AR_HANDLE hRequestee = pRequestee->GetHandle();
	nCompeteID = insertCompeteInfo( COMPETE_TYPE_VS_PLAYER, &hRequestee, &hRequester, COMPETE_STATE_REQUESTING, time( NULL ) + GameRule::COMPETE_ANSWER_WAITTING_DURATION );

	pRequester->SetCompeteID( nCompeteID );
	pRequestee->SetCompeteID( nCompeteID );

	SendCompeteRequest( pRequestee, COMPETE_TYPE_VS_PLAYER, pRequester->GetName() );

	LOG::Log11N4S( LM_COMPETE_REQUEST, pRequester->GetAccountID(), pRequester->GetSID(), pRequester->GetX(), pRequester->GetY(), pRequester->GetLayer(),
		pRequestee->GetX(), pRequestee->GetY(), pRequestee->GetLayer(), 0, 0, nCompeteID,
		pRequester->GetAccountName(), LOG::STR_NTS, pRequester->GetName(), LOG::STR_NTS, pRequestee->GetAccountName(), LOG::STR_NTS, pRequestee->GetName(), LOG::STR_NTS );

	return RESULT_SUCCESS;
}

const unsigned short CompeteManager::AnswerRequestFromPlayer( struct StructPlayer * pRequestee, const _COMPETE_ANSWER_TYPE & eAnswerType )
{
	THREAD_SYNCHRONIZE( m_csCompete );

	// 수락 가능 상태인지 확인
	unsigned int nCompeteID = pRequestee->GetCompeteID();
	if( !nCompeteID )
		return RESULT_NOT_IN_COMPETE;

	CompeteInfoBase * pCompeteInfo = getCompeteInfo( nCompeteID );
	if( !pCompeteInfo )
	{
		pRequestee->SetCompeteID( 0 );
		return RESULT_NOT_EXIST;
	}

	if( pCompeteInfo->eState != COMPETE_STATE_REQUESTING )
	{
		return RESULT_ALREADY_IN_COMPETE;
	}

	// 수락 여부 변수 유효성 체크(이상한 값일 경우 유저 거부로 처리)
	switch( eAnswerType )
	{
	case COMPETE_ANSWER_TYPE_ACCEPT:
	case COMPETE_ANSWER_TYPE_REJECT_BY_USER:
	case COMPETE_ANSWER_TYPE_REJECT_BY_OPTION:
		break;
	default:
		const_cast< _COMPETE_ANSWER_TYPE >( eAnswerType ) = COMPETE_ANSWER_TYPE_REJECT_BY_USER;
		break;
	}

	// 최초 대련 신청자 유효성 체크
	unsigned short nErrorCode = pCompeteInfo->Validate( false, false, true );
	if( nErrorCode != RESULT_SUCCESS )
	{
		// 유저의 장소 이동으로 인한 무효화 상태가 아니라면 비정상적인 경우
		assert( nErrorCode == RESULT_NOT_IN_COMPETIBLE_PLACE || nErrorCode == RESULT_TARGET_NOT_IN_COMPETIBLE_PLACE );
		deleteCompeteInfo( nCompeteID );
		return nErrorCode;
	}

	pCompeteInfo->OnAnswerRequest( eAnswerType );

	// 거절이었을 경우 대련 정보 제거
	if( eAnswerType != COMPETE_ANSWER_TYPE_ACCEPT )
		deleteCompeteInfo( nCompeteID );

	return RESULT_SUCCESS;
}

const unsigned short CompeteManager::RetireCompeteWithPlayer( struct StructPlayer * pRetirer, const _COMPETE_END_TYPE & eEndType )
{
	THREAD_SYNCHRONIZE( m_csCompete );

	unsigned int nCompeteID = pRetirer->GetCompeteID();
	if( !nCompeteID )
		return RESULT_NOT_IN_COMPETE;

	CompeteInfoBase * pCompeteInfo = getCompeteInfo( nCompeteID );
	if( !pCompeteInfo )
	{
		pRetirer->SetCompeteID( 0 );
		return RESULT_NOT_EXIST;
	}

	// 유효성 체크에서 실패할 경우 종료 처리가 완료되었으므로 Retire 함수는 성공...이랄까?
	_COMPETE_END_TYPE eReason = COMPETE_END_BY_LOGOUT;
	bool bRequesterWin = true;
	if( pCompeteInfo->Validate( true, true, true, &eReason, &bRequesterWin ) != RESULT_SUCCESS )
	{
		const bool bRequesteeRetired = ( static_cast< PlayerCompeteInfo * >( pCompeteInfo )->GetRequesteeAsPlayer() == pRetirer );
		StructPlayer * pCompetor = ( bRequesteeRetired ) ? static_cast< PlayerCompeteInfo * >( pCompeteInfo )->GetRequesterAsPlayer() : static_cast< PlayerCompeteInfo * >( pCompeteInfo )->GetRequesteeAsPlayer();
		const bool bCompetorWin = ( bRequesteeRetired ) ? bRequesterWin : !bRequesterWin;

		// 대련 종료 시 버프 제거 처리 추가(대련 종료 후 디버프에 의한 사망 방지)
		if( pCompeteInfo->eState == COMPETE_STATE_COMPETING )
		{
			pRetirer->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
			StructSummon * pSummon = pRetirer->GetMainSummon();
			if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
			pSummon = pRetirer->GetSubSummon();
			if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
		}

		// 대련 유저 2명 각각 로그를 각각 남김
		LOG::Log11N4S( LM_COMPETE_END, pRetirer->GetAccountID(), pRetirer->GetSID(), pRetirer->GetX(), pRetirer->GetY(), pRetirer->GetLayer(),
			( pCompetor ) ? pCompetor->GetX() : 0, ( pCompetor ) ? pCompetor->GetY() : 0, ( pCompetor ) ? pCompetor->GetLayer() : 0, !bCompetorWin, eReason, pCompeteInfo->nID,
			pRetirer->GetAccountName(), LOG::STR_NTS, pRetirer->GetName(), LOG::STR_NTS,
			( pCompetor ) ? pCompetor->GetAccountName() : "", LOG::STR_NTS, ( pCompetor ) ? pCompetor->GetName() : "", LOG::STR_NTS );

		if( pCompetor )
		{
			// 대련 종료 시 버프 제거 처리 추가(대련 종료 후 디버프에 의한 사망 방지)
			if( pCompeteInfo->eState == COMPETE_STATE_COMPETING )
			{
				pCompetor->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
				StructSummon * pSummon = pCompetor->GetMainSummon();
				if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
				pSummon = pCompetor->GetSubSummon();
				if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
			}

			LOG::Log11N4S( LM_COMPETE_END, pCompetor->GetAccountID(), pCompetor->GetSID(), pCompetor->GetX(), pCompetor->GetY(), pCompetor->GetLayer(),
				pRetirer->GetX(), pRetirer->GetY(), pRetirer->GetLayer(), bCompetorWin, eReason, pCompeteInfo->nID,
				pCompetor->GetAccountName(), LOG::STR_NTS, pCompetor->GetName(), LOG::STR_NTS,
				pRetirer->GetAccountName(), LOG::STR_NTS, pRetirer->GetName(), LOG::STR_NTS );
		}

		deleteCompeteInfo( nCompeteID );
		return RESULT_SUCCESS;
	}

	AR_HANDLE hRetirer = pRetirer->GetHandle();
	pCompeteInfo->OnRetire( &hRetirer, eEndType );

	deleteCompeteInfo( nCompeteID );

	return RESULT_SUCCESS;
}

const bool CompeteManager::IsStarted( const unsigned int nCompeteID, const bool bIncludeCounting ) const
{
	THREAD_SYNCHRONIZE( m_csCompete );

	CompeteInfoBase * pCompeteInfo = getCompeteInfo( nCompeteID );
	if( !pCompeteInfo )
		return false;

	return ( ( bIncludeCounting && pCompeteInfo->eState == COMPETE_STATE_COUNTDOWN ) || pCompeteInfo->eState == COMPETE_STATE_COMPETING );
}

void CompeteManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "CompeteManager" );

	extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;
	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "CompeteManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = GetArTime();

	THREAD_SYNCHRONIZE( m_csCompete );

	time_t tCurrent = time( NULL );

	for( std::vector< CompeteInfoBase * >::iterator it = m_vCompeteInfo.begin() ; it != m_vCompeteInfo.end() ; /* 루프에서 ++it */ )
	{
		// onProcess의 호출 주기인 1초마다 한 번씩 Validate를 무조건 함(부하 발생이 크다면 조정 필요 - 수락 이전 단계에서는 체크가 간소화될 수 있음)
		_COMPETE_END_TYPE eReason = COMPETE_END_BY_LOGOUT;
		bool bRequesterWin = false;
		if( (*it)->Validate( true, true, true, &eReason, &bRequesterWin ) != RESULT_SUCCESS )
		{
			// 플레이어 대련 종료 로그 남기기
			if( (*it)->GetCompeteType() == COMPETE_TYPE_VS_PLAYER )
			{
				StructPlayer * pRequestee = static_cast< PlayerCompeteInfo * >( *it )->GetRequesteeAsPlayer();
				StructPlayer * pRequester = static_cast< PlayerCompeteInfo * >( *it )->GetRequesterAsPlayer();

				// 대련 유저 2명 각각 로그를 각각 남김
				// * 신청 단계에서 Validate 실패는 승자 없이 무조건 false로 남겨야 함
				if( pRequestee )
				{
					// 대련 종료 시 버프 제거 처리 추가(대련 종료 후 디버프에 의한 사망 방지)
					if( (*it)->eState == COMPETE_STATE_COMPETING )
					{
						pRequestee->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
						StructSummon * pSummon = pRequestee->GetMainSummon();
						if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
						pSummon = pRequestee->GetSubSummon();
						if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
					}

					LOG::Log11N4S( LM_COMPETE_END, pRequestee->GetAccountID(), pRequestee->GetSID(), pRequestee->GetX(), pRequestee->GetY(), pRequestee->GetLayer(),
						( pRequester ) ? pRequester->GetX() : 0, ( pRequester ) ? pRequester->GetY() : 0, ( pRequester ) ? pRequester->GetLayer() : 0, ( (*it)->eState != COMPETE_STATE_REQUESTING ) ? !bRequesterWin : false, eReason, (*it)->nID,
						pRequestee->GetAccountName(), LOG::STR_NTS, pRequestee->GetName(), LOG::STR_NTS,
						( pRequester ) ? pRequester->GetAccountName() : "", LOG::STR_NTS, ( pRequester ) ? pRequester->GetName() : "", LOG::STR_NTS );
				}
				if( pRequester )
				{
					// 대련 종료 시 버프 제거 처리 추가(대련 종료 후 디버프에 의한 사망 방지)
					if( (*it)->eState == COMPETE_STATE_COMPETING )
					{
						pRequester->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
						StructSummon * pSummon = pRequester->GetMainSummon();
						if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
						pSummon = pRequester->GetSubSummon();
						if( pSummon && pSummon->IsInWorld() ) pSummon->PendClearStateWithFlag( StateInfo::ERASE_ON_DEAD );
					}

					LOG::Log11N4S( LM_COMPETE_END, pRequester->GetAccountID(), pRequester->GetSID(), pRequester->GetX(), pRequester->GetY(), pRequester->GetLayer(),
						( pRequestee ) ? pRequestee->GetX() : 0, ( pRequestee ) ? pRequestee->GetY() : 0, ( pRequestee ) ? pRequestee->GetLayer() : 0, ( (*it)->eState != COMPETE_STATE_REQUESTING ) ? bRequesterWin : false, eReason, (*it)->nID,
						pRequester->GetAccountName(), LOG::STR_NTS, pRequester->GetName(), LOG::STR_NTS,
						( pRequestee ) ? pRequestee->GetAccountName() : "", LOG::STR_NTS, ( pRequestee ) ? pRequestee->GetName() : "", LOG::STR_NTS );
				}
			}

			delete (*it);
			it = m_vCompeteInfo.erase( it );
			continue;
		}

		if( (*it)->tNextStateChange == NOT_SCHEDULED_TIME || (*it)->tNextStateChange > tCurrent )
		{
			++it;
			continue;
		}

		switch( (*it)->eState )
		{
		case COMPETE_STATE_REQUESTING:
			// 응답 대기 시간 초과 처리
			(*it)->OnAnswerTimeout();

			delete (*it);
			it = m_vCompeteInfo.erase( it );
			break;
		case COMPETE_STATE_COUNTDOWN:
			// 카운트 다운 완료 처리(대련 시작 됨)
			(*it)->OnEndCountdown();
			++it;
			break;
		case COMPETE_STATE_COMPETING:
			// 대련 시간 초과 처리
			(*it)->OnCompeteTimeout();

			delete (*it);
			it = m_vCompeteInfo.erase( it );
			break;
		}
	}
}

const unsigned int CompeteManager::insertCompeteInfo( const _COMPETE_TYPE & eType, const void * pRequestee, const void * pRequester, const _COMPETE_STATE & eState, const time_t & tNextStateChange )
{
	if( !pRequestee || !pRequester )
		return 0;

	CompeteInfoBase * pCompeteInfo = NULL;

	switch( eType )
	{
	case COMPETE_TYPE_VS_PLAYER:
		pCompeteInfo = new PlayerCompeteInfo( *static_cast< const AR_HANDLE * >( pRequestee ), *static_cast< const AR_HANDLE * >( pRequester ), eState, tNextStateChange );
		break;
	default:
		assert( 0 );
		break;
	}

	if( !pCompeteInfo )
		return 0;

	m_vCompeteInfo.push_back( pCompeteInfo );

	return pCompeteInfo->nID;
}

const bool CompeteManager::deleteCompeteInfo( const unsigned int nCompeteID )
{
	for( std::vector< CompeteInfoBase * >::iterator it = m_vCompeteInfo.begin() ; it != m_vCompeteInfo.end() ; ++it )
	{
		if( (*it)->nID == nCompeteID )
		{
			delete (*it);
			m_vCompeteInfo.erase( it );
			return true;
		}
	}

	return false;
}

CompeteInfoBase * CompeteManager::getCompeteInfo( const unsigned int nCompeteID ) const
{
	for( std::vector< CompeteInfoBase * >::const_iterator it = m_vCompeteInfo.begin() ; it != m_vCompeteInfo.end() ; ++it )
	{
		if( (*it)->nID == nCompeteID )
			return (*it);
	}

	return NULL;
}
