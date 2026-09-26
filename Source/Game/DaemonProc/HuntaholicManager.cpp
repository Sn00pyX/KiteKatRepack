
#include <toolkit/XEnv.h>
#include <dump/XExceptionHandler.h>
#include <mmo/ArcadiaServer.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "HuntaholicManager.h"
#include "SendMessage.h"
#include "PartyManager.h"
#include "RankingManager.h"
#include "NPCProc.h"
#include "GameProc.h"
#include "StructFieldProp.h"
#include "DB_Commands.h"

extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;


struct WarpFunctor : public PartyManager::PartyFunctor
{
	WarpFunctor( const AR_UNIT _x, const AR_UNIT _y, const unsigned char _layer, const AR_TIME _begin_time = 0 )
		: x( _x )
		, y( _y )
		, layer( _layer )
		, begin_time( _begin_time )
	{}

	virtual bool operator()( AR_HANDLE handle )
	{
		StructPlayer::iterator itPlayer = StructPlayer::get( handle );
		StructPlayer * pPlayer = (*itPlayer);

		if( !pPlayer )
			return false;

		PendWarpPlayer( pPlayer );

		if( begin_time )
		{
			TS_SC_HUNTAHOLIC_BEGIN_HUNTING msg;
			msg.begin_time = begin_time;
			PendMessage( pPlayer, &msg );
		}

		return true;
	}

	void PendWarpPlayer( StructPlayer * pPlayer )
	{
		// 랜덤 위치 설정
		AR_UNIT targetX, targetY;
		bool bValidPos = true;
		unsigned short try_cnt = 0;
		do
		{
			targetX = XRandom( x - 60, x + 60 );
			targetY = XRandom( y - 60, y + 60 );

			if( ++try_cnt > 300 )
			{
				bValidPos = false;
				break;
			}
		} while( GameContent::CollisionToLine( x, y, targetX, targetY ) );

		if( !bValidPos )
		{
			targetX = x;
			targetY = y;

			if( GameContent::IsBlocked( targetX, targetY ) )
			{
				FILELOG( "Unable to PendWarpPlayer (%f, %f)", targetX, targetY );
				_cprint( "Unable to PendWarpPlayer (%f, %f)\n", targetX, targetY );
			}
		}

		// 파티 락 걸린 상태로 호출되기도 하는 곳이므로 지역락 걸지 않음
		pPlayer->PendWarp( targetX, targetY, layer );
		ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
	}

	AR_UNIT			x;
	AR_UNIT			y;
	unsigned char	layer;
	AR_TIME			begin_time;
};


// HuntaholicManager::InstanceDungeon
void HuntaholicManager::InstanceDungeon::onMonsterDelete( StructMonster * pMonster )
{
	// 사냥이 종료된 이후면 아무것도 하지 않음
	if( bEnd || nScore >= pHuntaholicInfo->pHuntaholicBase->nMaxPoint )
		return;

	THREAD_SYNCHRONIZE( csInstance );

	// 사냥이 종료된 이후면 아무것도 하지 않음(락 걸고 나서 한 번 더 검사)
	if( bEnd || nScore >= pHuntaholicInfo->pHuntaholicBase->nMaxPoint )
		return;

	AR_TIME t = GetArTime();

	int nRespawnID = pMonster->GetInstanceRespawnID();

	std::vector< StructMonster * >::iterator it = std::find( vRespawnedMonster.begin(), vRespawnedMonster.end(), pMonster );
	// 이미 삭제 처리 된 몬스터면 점수 카운트 및 리젠 진행시키지 않음(사냥 진행 중인데 사망 처리가 2번 들어왔거나 리젠만 되고 vRespawnedMonster에 추가되지 않았거나... -_ -?)
	if( it == vRespawnedMonster.end() )
	{
		assert( 0 );
		return;
	}
	vRespawnedMonster.erase( it );

	// 시간제로 소환되었던 몬스터가 시간 제한에 의해 사라지는 경우에는 점수 가산, 리젠 예약 처리를 하지 않음
	// * 스킬에 의해 소환되었어도 영구 몹이면 포인트 가산 처리 함(단, 리젠은 안 됨. 현재 함수 아래쪽의 리젠 처리 부분 참고)
	if( pMonster->IsLifeTimeOver() )
		return;

	int nMonsterScore = 0;

	switch( pMonster->GetMonsterType() )
	{
	case MonsterBase::MONSTER_TYPE_HUNTAHOLIC_1LV:
		nMonsterScore = GameRule::HUNTAHOLIC_SCORE_FOR_LV1_MONSTER;
		break;
	case MonsterBase::MONSTER_TYPE_HUNTAHOLIC_2LV:
		nMonsterScore = GameRule::HUNTAHOLIC_SCORE_FOR_LV2_MONSTER;
		break;
	case MonsterBase::MONSTER_TYPE_HUNTAHOLIC_3LV:
		nMonsterScore = GameRule::HUNTAHOLIC_SCORE_FOR_LV3_MONSTER;
		break;
	case MonsterBase::MONSTER_TYPE_HUNTAHOLIC_BOSS:
		nMonsterScore = GameRule::HUNTAHOLIC_SCORE_FOR_BOSS_MONSTER;
		break;
	default:
		assert( 0 );
		return;
	}

	// 개인별 점수 추가
	{
		AR_HANDLE hKiller = NULL;

		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pMonster ) );
			hKiller = pMonster->GetMaxDamageDealer();
		}

		if( hKiller )
		{
			_SCORE_TAG * pScoreTag = getScoreTag( hKiller );
			if( !pScoreTag )
			{
				vScoreTag.push_back( _SCORE_TAG( hKiller ) );
				pScoreTag = &vScoreTag.back();
			}

			++( pScoreTag->nKillCount );
			pScoreTag->nScore += nMonsterScore;
		}
	}

	// 방 전체 점수 추가
	++nKillCount;
	nScore += nMonsterScore;

	// 최대 점수 도달 시 처리
	bool bMaxPointReached = nScore >= pHuntaholicInfo->pHuntaholicBase->nMaxPoint;
	if( bMaxPointReached )
	{
		nScore = pHuntaholicInfo->pHuntaholicBase->nMaxPoint;

		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockArea( pHuntaholicInfo->pHuntaholicBase->nDungeonRegionLeft, pHuntaholicInfo->pHuntaholicBase->nDungeonRegionTop,
				pHuntaholicInfo->pHuntaholicBase->nDungeonRegionRight, pHuntaholicInfo->pHuntaholicBase->nDungeonRegionBottom, nInstanceNo ) );

			clearMonsters();
			clearHealingProps();
		}

		struct MaxPointAchievementBroadcaster : public PartyManager::PartyFunctor
		{
			virtual bool operator ()( AR_HANDLE handle )
			{
				StructPlayer::iterator itPlayer = StructPlayer::get( handle );
				StructPlayer * pPlayer = (*itPlayer);

				if( pPlayer )
					PendMessage( pPlayer, &msg );

				return true;
			}

			TS_SC_HUNTAHOLIC_MAX_POINT_ACHIEVED msg;
		} fo;

		PartyManager::GetInstance().DoEachMember( nPartyID, fo );
	}

	// 점수 방송
	{
		TS_SC_HUNTAHOLIC_UPDATE_SCORE msg;

		msg.score = nScore;

		struct ScoreBroadcaster : public PartyManager::PartyFunctor
		{
			ScoreBroadcaster( const HuntaholicManager::InstanceDungeon * pInstance, TS_SC_HUNTAHOLIC_UPDATE_SCORE * pMsg )
				: m_pInstance( pInstance )
				, m_pMsg( pMsg )
			{}

			virtual bool operator ()( AR_HANDLE handle )
			{
				StructPlayer::iterator itPlayer = StructPlayer::get( handle );
				StructPlayer * pPlayer = (*itPlayer);

				if( !pPlayer )
					return false;

				const _SCORE_TAG * pScoreTag = m_pInstance->getScoreTag( handle );
				m_pMsg->kill_count = ( pScoreTag ) ? pScoreTag->nKillCount : 0;

				PendMessage( pPlayer, m_pMsg );

				return true;
			}

			const HuntaholicManager::InstanceDungeon *	m_pInstance;
			TS_SC_HUNTAHOLIC_UPDATE_SCORE *				m_pMsg;
		} fo( this, &msg );

		PartyManager::GetInstance().DoEachMember( nPartyID, fo );
	}

	// 몬스터 리젠 예약(최대 점수 도달 시 또는 스킬에 의해 리젠되어 리젠 정보 ID 값이 설정되지 않은 몬스터는 리젠 예약하지 않음)
	if( !bMaxPointReached && nRespawnID )
	{
		std::vector< const GameContent::HUNTAHOLIC_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = pInstanceBase->vRespawnInfo.begin();
		for( ; itRespawn != pInstanceBase->vRespawnInfo.end() ; ++itRespawn )
		{
			if( (*itRespawn)->nID != nRespawnID )
				continue;

			vPendedMonsterRespawn.push_back( PendedMonsterRespawnInfo( (*itRespawn), t + (*itRespawn)->nPeriod ) );

			break;
		}

		assert( itRespawn != pInstanceBase->vRespawnInfo.end() );
	}
}

void HuntaholicManager::InstanceDungeon::clearMonsters()
{
	// 리젠 대기 중인 몬스터 목록 제거
	vPendedMonsterRespawn.clear();

	// 몬스터 제거 처리(지역 락 필요)
	for( std::vector< StructMonster * >::iterator itMonster = vRespawnedMonster.begin() ; itMonster != vRespawnedMonster.end() ; /* 루프에서 ++itMonster 처리 */ )
	{
		if( (*itMonster)->IsEnable() )
		{
			(*itMonster)->SetDeleteHandler( NULL );

			// 더이상의 스케쥴러 요청을 무시
			(*itMonster)->Disable();

			// 월드에서 제거한다.
			if( (*itMonster)->IsInWorld() )
			{
				RemoveMonsterFromWorld( (*itMonster) );
			}

			// object delete 요청
			ArcadiaServer::Instance().DeleteObject( (*itMonster) );

			itMonster = vRespawnedMonster.erase( itMonster );
		}
		else
		{
			++itMonster;
		}
	}
}

void HuntaholicManager::InstanceDungeon::onFieldPropDelete( StructFieldProp * pProp )
{
	// 사냥이 종료된 이후면 아무것도 하지 않음
	if( bEnd )
		return;

	THREAD_SYNCHRONIZE( csInstance );

	// 사냥이 종료된 이후면 아무것도 하지 않음(락 걸고 나서 한 번 더 검사)
	if( bEnd )
		return;

	std::vector< StructFieldProp * >::iterator itErase = std::find( vRespawnedHealingProp.begin(), vRespawnedHealingProp.end(), pProp );
	// 이미 삭제된 프랍이면 아무 처리 안 함(삭제처리가 2번 일어났거나 리젠되기만 하고 vRespawnedHealingProp에는 추가되지 않았거나 -_ -?)
	if( itErase == vRespawnedHealingProp.end() )
	{
		assert( 0 );
		return;
	}

	vRespawnedHealingProp.erase( itErase );
	vPendedHealingPropRespawn.push_back( PendedHealingPropRespawnInfo( pProp->GetRespawnInfo(), GetArTime() + pProp->GetFieldPropBase()->nRegenTime ) );
}

void HuntaholicManager::InstanceDungeon::clearHealingProps()
{
	// 리젠 대기 중인 힐링 프랍 제거
	vPendedHealingPropRespawn.clear();

	// 힐링 프랍 제거 처리(지역 락 필요)
	for( std::vector< StructFieldProp * >::iterator itHealingProp = vRespawnedHealingProp.begin() ; itHealingProp != vRespawnedHealingProp.end() ; ++itHealingProp )
	{
		// 해당 프랍이 ProcDelete에서 다시 리젠되도록 vPendedHealingPropRespawn에 추가되지 않도록 DeleteHandler를 제거하고 삭제함
		(*itHealingProp)->SetDeleteHandler( NULL );

		// 유저가 사용함으로 인해서 RemoveObject/PendFreeFieldProp은 걸려 있지만 아직 onFieldPropDelete가 호출되지 않은 경우에는 아무것도 하지 않음
		if( !(*itHealingProp)->IsInWorld() )
			continue;

		ArcadiaServer::Instance().RemoveObject( (*itHealingProp) );
		StructFieldProp::PendFreeFieldProp( (*itHealingProp) );
	}

	vRespawnedHealingProp.clear();
}

void HuntaholicManager::InstanceDungeon::broadcastInstanceDungeonInfoToMembers() const
{
	TS_HUNTAHOLIC_INSTANCE_INFO info;
	HuntaholicManager::assembleInstanceInfo( this, &info );

	struct InstanceDungeonInfoBroadcaster : public PartyManager::PartyFunctor
	{
		InstanceDungeonInfoBroadcaster( const TS_HUNTAHOLIC_INSTANCE_INFO & info )
			: m_InstanceInfo( info )
		{}

		virtual bool operator ()( AR_HANDLE handle )
		{
			StructPlayer::iterator itPlayer = StructPlayer::get( handle );
			StructPlayer * pPlayer = (*itPlayer);

			if( pPlayer )
				SendHuntaholicInstanceInfo( pPlayer, m_InstanceInfo );

			return true;
		}

		const TS_HUNTAHOLIC_INSTANCE_INFO & m_InstanceInfo;
	} fo( info );

	PartyManager::GetInstance().DoEachMember( nPartyID, fo );
}

const unsigned short HuntaholicManager::InstanceDungeon::joinInstanceDungeon( StructPlayer * pPlayer, const char * pszPassword )
{
	// 이미 시작된 방이면 조인 불가
	if( tBeginTime )
		return RESULT_COOL_TIME;

	// 최대 멤버 수 제한 걸렸으면 조인 불가
	if( PartyManager::GetInstance().GetMemberCount( nPartyID ) >= nMaxMemberCount )
		return RESULT_LIMIT_MAX;

	// 레벨대가 맞지 않는 방에는 조인 불가
	if( !pInstanceBase->IsProperLevel( pPlayer->GetLevel() ) )
		return RESULT_LIMIT_TARGET;

	// 비번이 지정되어있는데 맞지 않으면 조인 불가
	if( strlen( szPassword ) && strcmp( szPassword, pszPassword ) )
		return RESULT_INVALID_PASSWORD;

	// 파티에 조인 실패하면 방에도 조인 불가
	if( !PartyManager::GetInstance().JoinParty( nPartyID, pPlayer ) )
		return RESULT_UNKNOWN;

	broadcastInstanceDungeonInfoToMembers();

	SendPartyInfo( pPlayer );

	BroadcastPartyMemberInfo( nPartyID, pPlayer );

	SendTimeSync( pPlayer );

	LOG::Log11N4S( LM_PARTY_JOIN, pPlayer->GetAccountID(), pPlayer->GetSID(), nPartyID, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), 1, 0, 0, 0, 0,
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

	LOG::Log11N4S( LM_HUNTAHOLIC_JOIN, pPlayer->GetAccountID(), pPlayer->GetSID(), pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, nInstanceNo, nMaxMemberCount, !!szPassword[ 0 ], PartyManager::GetInstance().GetMemberCount( nPartyID ), pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

	return RESULT_SUCCESS;
}

const unsigned short HuntaholicManager::InstanceDungeon::leaveInstanceDungeon( StructPlayer * pPlayer )
{
	// 파티가 이미 해산된 후에 leaveInstanceDungeon이 호출되는 경우라면(endHunting 처리) 걍 패스
	if( !pPlayer->IsInParty() )
		return RESULT_SUCCESS;

	// 현재 인스턴스에 소속되지 않은 플레이어가 나가겠다고 하면 탈퇴 불가
	if( !PartyManager::GetInstance().IsMember( nPartyID, pPlayer->GetPlayerUID() ) )
	{
		assert( 0 );
		return RESULT_NOT_OWN;
	}

	// 파장이 나가려고 하면
	if( PartyManager::GetInstance().IsLeader( nPartyID, pPlayer->GetPlayerUID() ) )
	{
		// 다음 파장 될 사람이 없거나, 다른 파티원이 남아 있긴 한데
		// 죄다 오프라인이면 파티 해산(베어로드 파티에는 오프라인 멤버가 있으면 안되지만...)
		if( PartyManager::GetInstance().GetMemberCount( nPartyID ) == 1 || !PartyManager::GetInstance().AutoPromote( nPartyID, true ) )
		{
			LOG::Log11N4S( LM_PARTY_DESTROY, pPlayer->GetAccountID(), pPlayer->GetSID(), nPartyID, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), 1, 0, 0, 0, 0,
				pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

			BroadcastPartyDestroy( nPartyID );

			PartyManager::GetInstance().DestroyParty( nPartyID );

			return RESULT_SUCCESS;
		}

		// 새 파장 정보 얻기
		StructPlayer::iterator itNewLeader = StructPlayer::get( StructPlayer::FindPlayer( PartyManager::GetInstance().GetLeaderName( nPartyID ).c_str() ) );
		StructPlayer * pNewLeader = (*itNewLeader);

		// 인계는 됐는데 어쩐 일인지 새 파장이 없다... 오프라인이거나... 해산해버리자 -_ -;
		if( !pNewLeader )
		{
			LOG::Log11N4S( LM_PARTY_DESTROY, pPlayer->GetAccountID(), pPlayer->GetSID(), nPartyID, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), 1, 0, 0, 0, 0,
				pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

			BroadcastPartyDestroy( nPartyID );

			PartyManager::GetInstance().DestroyParty( nPartyID );

			return RESULT_SUCCESS;
		}

		// 다른 사람에게 자동 인계가 처리됐으면 파장 변경 방송 처리
		PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "PROMOTE|%d|%s|", nPartyID, pNewLeader->GetName() );

		LOG::Log11N4S( LM_PARTY_PROMOTE, pPlayer->GetAccountID(), pPlayer->GetSID(), nPartyID, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
			pNewLeader->GetAccountID(), pNewLeader->GetSID(), pNewLeader->GetX(), pNewLeader->GetY(), pNewLeader->GetLayer(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, pNewLeader->GetAccountName(), LOG::STR_NTS, pNewLeader->GetName(), LOG::STR_NTS );
	}

	BroadcastPartyLeave( pPlayer );

	PartyManager::GetInstance().LeaveParty( nPartyID, pPlayer->GetPlayerUID() );

	LOG::Log11N4S( LM_PARTY_LEAVE, pPlayer->GetAccountID(), pPlayer->GetSID(), nPartyID, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), 1, 0, 0, 0, 0,
		pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

	broadcastInstanceDungeonInfoToMembers();

	return RESULT_SUCCESS;
}

const unsigned short HuntaholicManager::InstanceDungeon::beginHunting()
{
	if( bBegin )
		return RESULT_ALREADY_EXIST;

	bBegin = true;

	// 몬스터 리젠 시키고~
	for( std::vector< const GameContent::HUNTAHOLIC_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = pInstanceBase->vRespawnInfo.begin() ; itRespawn != pInstanceBase->vRespawnInfo.end() ; ++itRespawn )
	{
		for( int nIndex = 0 ; nIndex < (*itRespawn)->nCount ; ++nIndex )
		{
			int nRespawnX;
			int nRespawnY;

			int nRespawnTryCount = 0;

			do
			{
				nRespawnX = XRandom( static_cast< int >( (*itRespawn)->bxArea.GetLeft() ), static_cast< int >( (*itRespawn)->bxArea.GetRight() ) );
				nRespawnY = XRandom( static_cast< int >( (*itRespawn)->bxArea.GetTop() ), static_cast< int >( (*itRespawn)->bxArea.GetBottom() ) );

				if( ++nRespawnTryCount > 10 )
				{
					FILELOG( "Unable to respawn monster in huntaholic dungeon. HuntaholicID(%d), InstanceID(%d), RespawnID(%d)", pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, (*itRespawn)->nID );
					_cprint( "Unable to respawn monster in huntaholic dungeon. HuntaholicID(%d), InstanceID(%d), RespawnID(%d)\n", pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, (*itRespawn)->nID );

					nRespawnTryCount = 0;
					break;
				}
			}
			while( GameContent::IsBlocked( nRespawnX, nRespawnY ) );

			if( !nRespawnTryCount )
				continue;

			StructMonster * pMob = respawnMonster( nRespawnX, nRespawnY, nInstanceNo, (*itRespawn)->nMonsterID, (*itRespawn)->bWandering, 0, this );
			if( !pMob )
			{
				FILELOG( "Unknown monster is to be respawned in huntaholic dungeon. HuntaholicID(%d), InstanceID(%d), RespawnID(%d)", pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, (*itRespawn)->nID );
				_cprint( "Unknown monster is to be respawned in huntaholic dungeon. HuntaholicID(%d), InstanceID(%d), RespawnID(%d)\n", pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, (*itRespawn)->nID );

				continue;
			}

			pMob->SetInstanceRespawnID( (*itRespawn)->nID );

			vRespawnedMonster.push_back( pMob );
		}
	}

	// 프랍 리젠 시키고~
	for( std::vector< const GameContent::FIELD_PROP_RESPAWN_INFO * >::const_iterator itHealingProp = pInstanceBase->vHealingPropInfo.begin() ; itHealingProp != pInstanceBase->vHealingPropInfo.end() ; ++itHealingProp )
	{
		StructFieldProp * pProp = NULL;
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockWithVisibleRange( GetRegionX( (*itHealingProp)->x ), GetRegionY( (*itHealingProp)->y ), nInstanceNo ) );

			pProp = StructFieldProp::Create( this, (*itHealingProp), (*itHealingProp)->x, (*itHealingProp)->y, nInstanceNo );
		}

		if( pProp )
		{
			vRespawnedHealingProp.push_back( pProp );
		}
	}

	// 모두 입장~
	WarpFunctor beginWarpFo( pHuntaholicInfo->pHuntaholicBase->posDungeon.x, pHuntaholicInfo->pHuntaholicBase->posDungeon.y, nInstanceNo, GetArTime() );
	PartyManager::GetInstance().DoEachMember( nPartyID, beginWarpFo );

	LOG::Log11N4S( LM_HUNTAHOLIC_BEGIN, 0, 0, pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, nInstanceNo, nMaxMemberCount, !!szPassword[ 0 ], PartyManager::GetInstance().GetMemberCount( nPartyID ), 0, 0, 0,
		"", 0, "", 0, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

	return RESULT_SUCCESS;
}

const unsigned short HuntaholicManager::InstanceDungeon::quitHunting( StructPlayer * pPlayer, const bool bNeedToReward, const HuntaholicManager::_HUNTING_RESULT eResult, int * pnSuppliedRewardPoint )
{
	unsigned short nErrorCode = leaveInstanceDungeon( pPlayer );
	if( nErrorCode != RESULT_SUCCESS )
		return nErrorCode;

	if( bNeedToReward )
	{
		int nPrevPoint = pPlayer->GetHuntaholicPoint();
		bool bSuccess = ( eResult == HUNTING_RESULT_SUCCESS );
		int nGainPoint = ( bSuccess ) ? ceil( pInstanceBase->fPointAdvantage * nScore ) : 0;

		// 게임 중독 방지 시스템에 의한 획득 보상 페널티 적용
		if( pPlayer->IsGameTimeLimited() )
		{
			nGainPoint = GameRule::GetGameTimeLimitPenalty( pPlayer->GetContinuousPlayTime() ) * nGainPoint;
		}

		InstanceDungeon::_SCORE_TAG * pScoreTag = getScoreTag( pPlayer->GetHandle() );

		// 성공 시 성공 보상 및 포인트 지급
		if( bSuccess )
		{
			// AddExp는 onExpChange에서 레벨업 방송이 있으므로 지역 락 걸어야 함
			{
				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

				pPlayer->SetHuntaholicPoint( nPrevPoint + nGainPoint );
				pPlayer->AddExp( pInstanceBase->nRewardExp, pInstanceBase->nRewardJp, false );

				if( pInstanceBase->nRewardSuccessItemCode )
				{
					StructItem * pRewardSuccessItem = StructItem::AllocItem( 0, static_cast< ItemBase::ItemCode >( pInstanceBase->nRewardSuccessItemCode ), pInstanceBase->nRewardSuccessItemCount, ItemInstance::BY_HUNTAHOLIC );

					if( pRewardSuccessItem->IsJoinable() )
					{
						PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@254\v#@item_name@#\v@%d\v#@item_num@#\v%d", pRewardSuccessItem->GetItemBase().nNameId, pRewardSuccessItem->GetCount() );
					}
					else
					{
						PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@253\v#@item_name@#\v@%d", pRewardSuccessItem->GetItemBase().nNameId );
					}

					StructItem * pNewItem = pPlayer->PushItem( pRewardSuccessItem, pRewardSuccessItem->GetCount() );
					if( pNewItem )
					{
						LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pRewardSuccessItem->GetItemEnhance() * 100 + pRewardSuccessItem->GetItemLevel(), pRewardSuccessItem->GetItemCode(), pRewardSuccessItem->GetCount(), pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "HUNTAHOLIC", LOG::STR_NTS );
					}

					if( pNewItem != pRewardSuccessItem )
					{
						StructItem::PendFreeItem( pRewardSuccessItem );
					}
				}
			}

			if( pnSuppliedRewardPoint )
				*pnSuppliedRewardPoint = nGainPoint;
		}
		// 사망에 의한 실패 및 중동 방지에 의한 실패일 경우에는 실패 보상 지급
		else if( ( eResult == HUNTING_RESULT_FAILED_BY_DEATH || eResult == HUNTING_RESULT_FAILED_BY_GAMETIME_LIMIT ) && pInstanceBase->nRewardFailItemCode )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

			StructItem * pRewardFailItem = StructItem::AllocItem( 0, static_cast< ItemBase::ItemCode >( pInstanceBase->nRewardFailItemCode ), pInstanceBase->nRewardFailItemCount, ItemInstance::BY_HUNTAHOLIC );

			if( pRewardFailItem->IsJoinable() )
			{
				PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@254\v#@item_name@#\v@%d\v#@item_num@#\v%d", pRewardFailItem->GetItemBase().nNameId, pRewardFailItem->GetCount() );
			}
			else
			{
				PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@253\v#@item_name@#\v@%d", pRewardFailItem->GetItemBase().nNameId );
			}

			StructItem * pNewItem = pPlayer->PushItem( pRewardFailItem, pRewardFailItem->GetCount() );
			if( pNewItem )
			{
				LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pRewardFailItem->GetItemEnhance() * 100 + pRewardFailItem->GetItemLevel(), pRewardFailItem->GetItemCode(), pRewardFailItem->GetCount(), pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "HUNTAHOLIC", LOG::STR_NTS );
			}

			if( pNewItem != pRewardFailItem )
			{
				StructItem::PendFreeItem( pRewardFailItem );
			}
		}
		// 성공도 아니고 사망에 의한 실패도 아니라면 보상을 전혀 지급하지 않고 페널티로 디버프 부여 및 잔여 입장 횟수를 1회 추가 차감
		else
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

			pPlayer->AddHuntaholicEnterableCount( -1 );
			AR_TIME t = GetArTime();
			pPlayer->AddState( StructState::MOVE_SPEED_SLOWDOWN, pPlayer->GetHandle(), 1, t, t + GameRule::HUNTAHOLIC_QUITTING_PENALTY_DEBUFF_TIME );
		}

		SendHuntaholicHuntingScore( pPlayer, pHuntaholicInfo->pHuntaholicBase->nID, ( pScoreTag ) ? pScoreTag->nKillCount : 0, ( pScoreTag ) ? pScoreTag->nScore : 0,
			nKillCount, nScore, pInstanceBase->fPointAdvantage, nGainPoint, eResult );

		LOG::Log11N4S( LM_HUNTAHOLIC_QUIT, pPlayer->GetAccountID(), pPlayer->GetSID(), pHuntaholicInfo->pHuntaholicBase->nID, nInstanceNo, eResult, nGainPoint, pPlayer->GetHuntaholicPoint(), pInstanceBase->nRewardExp, pPlayer->GetEXP(), pInstanceBase->nRewardJp, pPlayer->GetJobPoint(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );
	}
	else
	{
		LOG::Log11N4S( LM_HUNTAHOLIC_QUIT, pPlayer->GetAccountID(), pPlayer->GetSID(), pHuntaholicInfo->pHuntaholicBase->nID, nInstanceNo, HUNTING_RESULT_NOREWARD, 0, pPlayer->GetHuntaholicPoint(), 0, pPlayer->GetEXP(), 0, pPlayer->GetJobPoint(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );
	}

	return RESULT_SUCCESS;
}

const unsigned short HuntaholicManager::InstanceDungeon::endHunting()
{
	if( bEnd )
		return RESULT_NOT_EXIST;

	bEnd = true;

	// 리젠되어 있는 몹/프랍 모두 제거
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockArea( pHuntaholicInfo->pHuntaholicBase->nDungeonRegionLeft, pHuntaholicInfo->pHuntaholicBase->nDungeonRegionTop,
			pHuntaholicInfo->pHuntaholicBase->nDungeonRegionRight, pHuntaholicInfo->pHuntaholicBase->nDungeonRegionBottom, nInstanceNo ) );

		// 리젠 대기 중인 몬스터 데이터 제거
		{
			THREAD_SYNCHRONIZE( pHuntaholicInfo->csPendedRespawn );

			for( std::vector< GameContent::PENDED_RESPAWN_INFO * >::iterator itRespawn = pHuntaholicInfo->vPendedRespawn.begin() ; itRespawn != pHuntaholicInfo->vPendedRespawn.end() ; /* 루프에서 ++itRespawn 처리 */ )
			{
				if( (*itRespawn)->layer == nInstanceNo )
				{
					delete (*itRespawn);
					itRespawn = pHuntaholicInfo->vPendedRespawn.erase( itRespawn );
				}
				else
					++itRespawn;
			}
		}

		clearMonsters();
		clearHealingProps();
	}

	struct MemberHandleCollector : public PartyManager::PartyFunctor
	{
		virtual bool operator()( AR_HANDLE handle )
		{
			vMember.push_back( handle );
			return true;
		}

		std::vector< AR_HANDLE >	vMember;
	} fo;

	PartyManager::GetInstance().DoEachMember( nPartyID, fo );

	BroadcastPartyDestroy( nPartyID );

	LOG::Log11N4S( LM_PARTY_DESTROY, 0, 0, nPartyID, 0, 0, 0, 2, 0, 0, 0, 0,
		"", 0, "", 0, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

	PartyManager::GetInstance().DestroyParty( nPartyID );

	// endHunting 함수는 모든 유저가 나가거나 타임아웃으로 동시에 모두 쫓겨나야 할 때만 호출되므로(개별 탈퇴는 quitHunting) 실패 시에도 페널티 없음(사망 실패와 동일하게 처리)
	_HUNTING_RESULT eResult = ( pHuntaholicInfo->pHuntaholicBase->nObjectivePoint > nScore ) ? HUNTING_RESULT_FAILED_BY_DEATH : HUNTING_RESULT_SUCCESS;
	WarpFunctor endWarpFo( pHuntaholicInfo->pHuntaholicBase->posLobby.x, pHuntaholicInfo->pHuntaholicBase->posLobby.y, pInstanceBase->nID );
	for( std::vector< AR_HANDLE >::const_iterator itPlayer = fo.vMember.begin() ; itPlayer != fo.vMember.end() ; ++itPlayer )
	{
		StructPlayer::iterator it = StructPlayer::get( (*itPlayer) );
		StructPlayer * pPlayer = (*it);

		if( !pPlayer )
			continue;

		// 파티 해산 이후에 호출되는 quitHunting 안에서 호출되는 leaveInstanceDungeon은 파티 관련 처리는 아무것도 하지 않으므로 보상 지급&결과 방송 처리만 됨
		int nSuppliedRewardPoint = 0;
		quitHunting( pPlayer, true, ( pPlayer->IsDead() ) ? HUNTING_RESULT_FAILED_BY_DEATH : eResult, &nSuppliedRewardPoint );

		// 성공해서 보상을 지급받았다면 지급된 보상을 랭킹에 적용
		if( nSuppliedRewardPoint )
		{
			RankingManager::Instance().AddRankingScore( RankingManager::RANKING_TYPE_HUNTAHOLIC_THIS_MONTH, pPlayer->GetPlayerUID(), pPlayer->GetName(), nSuppliedRewardPoint, true );
			RankingManager::Instance().AddRankingScore( RankingManager::RANKING_TYPE_HUNTAHOLIC_TOTAL, pPlayer->GetPlayerUID(), pPlayer->GetName(), nSuppliedRewardPoint, true );
		}

		// 로비로 돌려보냄
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

			endWarpFo.PendWarpPlayer( pPlayer );
		}
	}

	return RESULT_SUCCESS;
}

void HuntaholicManager::InstanceDungeon::procMonsterRespawn( const AR_TIME & tCurrent )
{
	for( std::vector< PendedMonsterRespawnInfo >::iterator itRespawn = vPendedMonsterRespawn.begin() ; itRespawn != vPendedMonsterRespawn.end() ; /* 루프에서 ++itRespawn 처리 */ )
	{
		if( (*itRespawn).second > tCurrent )
		{
			++itRespawn;
			continue;
		}

		int nRespawnX;
		int nRespawnY;

		int nRespawnTryCount = 0;

		do
		{
			nRespawnX = XRandom( static_cast< int >( (*itRespawn).first->bxArea.GetLeft() ), static_cast< int >( (*itRespawn).first->bxArea.GetRight() ) );
			nRespawnY = XRandom( static_cast< int >( (*itRespawn).first->bxArea.GetTop() ), static_cast< int >( (*itRespawn).first->bxArea.GetBottom() ) );

			if( ++nRespawnTryCount > 10 )
			{
				FILELOG( "Unable to respawn monster in huntaholic dungeon. HuntaholicID(%d), InstanceID(%d), RespawnID(%d)", pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, (*itRespawn).first->nID );
				_cprint( "Unable to respawn monster in huntaholic dungeon. HuntaholicID(%d), InstanceID(%d), RespawnID(%d)\n", pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, (*itRespawn).first->nID );

				nRespawnTryCount = 0;
				break;
			}
		}
		while( GameContent::IsBlocked( nRespawnX, nRespawnY ) );

		if( !nRespawnTryCount )
			continue;

		StructMonster * pMob = respawnMonster( nRespawnX, nRespawnY, nInstanceNo, (*itRespawn).first->nMonsterID, (*itRespawn).first->bWandering, 0, this );
		if( !pMob )
		{
			FILELOG( "Unknown monster is to be respawned in huntaholic dungeon. HuntaholicID(%d), InstanceID(%d), RespawnID(%d)", pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, (*itRespawn).first->nID );
			_cprint( "Unknown monster is to be respawned in huntaholic dungeon. HuntaholicID(%d), InstanceID(%d), RespawnID(%d)\n", pHuntaholicInfo->pHuntaholicBase->nID, pInstanceBase->nID, (*itRespawn).first->nID );

			continue;
		}

		pMob->SetInstanceRespawnID( (*itRespawn).first->nID );

		vRespawnedMonster.push_back( pMob );

		itRespawn = vPendedMonsterRespawn.erase( itRespawn );
	}
}

void HuntaholicManager::InstanceDungeon::procHealingPropRespawn( const AR_TIME & tCurrent )
{
	for( std::vector< PendedHealingPropRespawnInfo >::iterator itRespawn = vPendedHealingPropRespawn.begin() ; itRespawn != vPendedHealingPropRespawn.end() ; /* 루프에서 ++itRespawn 처리 */ )
	{
		if( (*itRespawn).second > tCurrent )
		{
			++itRespawn;
			continue;
		}

		StructFieldProp * pProp = NULL;
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockWithVisibleRange( GetRegionX( (*itRespawn).first->x ), GetRegionY( (*itRespawn).first->y ), nInstanceNo ) );

			pProp = StructFieldProp::Create( this, (*itRespawn).first, (*itRespawn).first->x, (*itRespawn).first->y, nInstanceNo );
		}

		if( pProp )
		{
			vRespawnedHealingProp.push_back( pProp );
		}

		itRespawn = vPendedHealingPropRespawn.erase( itRespawn );
	}
}

HuntaholicManager::InstanceDungeon::_SCORE_TAG * HuntaholicManager::InstanceDungeon::getScoreTag( const AR_HANDLE & hPlayer )
{
	for( std::vector< _SCORE_TAG >::iterator it = vScoreTag.begin() ; it != vScoreTag.end() ; ++it )
	{
		if( (*it).hOwner != hPlayer )
			continue;

		return &(*it);
	}

	return NULL;
}

const HuntaholicManager::InstanceDungeon::_SCORE_TAG * HuntaholicManager::InstanceDungeon::getScoreTag( const AR_HANDLE & hPlayer ) const
{
	for( std::vector< _SCORE_TAG >::const_iterator it = vScoreTag.begin() ; it != vScoreTag.end() ; ++it )
	{
		if( (*it).hOwner != hPlayer )
			continue;

		return &(*it);
	}

	return NULL;
}


// HuntaholicManager::HuntaholicInfo
const unsigned char HuntaholicManager::HuntaholicInfo::GetNewInstanceNo() const
{
	unsigned char nInstanceNo = 1;

	while( stInstanceNo.find( nInstanceNo ) != stInstanceNo.end() && nInstanceNo < 0xFF )
		++nInstanceNo;

	return nInstanceNo;
}

void HuntaholicManager::HuntaholicInfo::ProcPendedMonsterRespawn()
{
	// [지역 락 -> csPEndedRespawn / csInstanceDungeon -> HuntaholicInfo::csInstance -> 지역 락] 지역 락으로 인한 데드락 발생 방지용 락 전략
	std::vector< GameContent::PENDED_RESPAWN_INFO * > _vPendedRespawn;
	{
		THREAD_SYNCHRONIZE( csPendedRespawn );

		if( vPendedRespawn.empty() )
			return;

		_vPendedRespawn.swap( vPendedRespawn );
	}

	THREAD_SYNCHRONIZE( csInstanceDungeon );

	for( std::vector< GameContent::PENDED_RESPAWN_INFO * >::const_iterator itRespawn = _vPendedRespawn.begin() ; itRespawn != _vPendedRespawn.end() ; ++itRespawn )
	{
		GameContent::PENDED_RESPAWN_INFO * pRespawn = (*itRespawn);

		for( std::vector< InstanceDungeon * >::iterator itInstance = vInstanceDungeon.begin() ; itInstance != vInstanceDungeon.end() ; ++itInstance )
		{
			InstanceDungeon * pInstance = (*itInstance);

			if( pInstance->nInstanceNo != pRespawn->layer )
				continue;

			// 몹이 리젠되어야 할 방인데 아직 시작되지 않은 상태이거나 끝난 이후면 리젠시키지 않음
			if( !pInstance->bBegin || pInstance->bEnd || pInstance->nScore >= pHuntaholicBase->nMaxPoint )
				break;

			THREAD_SYNCHRONIZE( pInstance->csInstance );

			// 몹이 리젠되어야 할 방인데 아직 시작되지 않은 상태이거나 끝난 이후면 리젠시키지 않음(락 걸고 나서 한 번 더 검사)
			if( !pInstance->bBegin || pInstance->bEnd || pInstance->nScore >= pHuntaholicBase->nMaxPoint )
				break;

			for( int nCountIdx = 0 ; nCountIdx < pRespawn->respawnCount ; ++nCountIdx )
			{
				// 랜덤 위치 설정
				AR_UNIT x, y;
				bool bValidPos = true;
				unsigned short try_cnt = 0;
				do
				{
					x = XRandom( pRespawn->x - 60, pRespawn->x + 60 );
					y = XRandom( pRespawn->y - 60, pRespawn->y + 60 );

					if( ++try_cnt > 300 )
					{
						bValidPos = false;
						break;
					}
				} while( GameContent::CollisionToLine( pRespawn->x, pRespawn->y, x, y ) );

				if( !bValidPos )
				{
					x = pRespawn->x;
					y = pRespawn->y;

					if( GameContent::IsBlocked( x, y ) == true )
					{
						FILELOG( "Unable to respawn monster in huntaholic dungeon. (respawn pos) HuntaholicID(%d), x(%d), y(%d), layer(%d), MonsterID(%d)", pHuntaholicBase->nID, pRespawn->x, pRespawn->y, pRespawn->layer, pRespawn->monsterId );
						_cprint( "Unable to respawn monster in huntaholic dungeon. (respawn pos) HuntaholicID(%d), x(%d), y(%d), layer(%d), MonsterID(%d)", pHuntaholicBase->nID, pRespawn->x, pRespawn->y, pRespawn->layer, pRespawn->monsterId );
					}
				}

				{
					ARCADIA_LOCK( ArcadiaServer::Instance().LockWithVisibleRange( ::GetRegionX( x ), ::GetRegionY( y ), pRespawn->layer ) );

					StructMonster * pMob = respawnMonster( x, y, pRespawn->layer, pRespawn->monsterId, true, 0, pInstance );
					if( !pMob )
					{
						FILELOG( "Unknown monster is to be respawned in huntaholic dungeon by pending. HuntaholicID(%d), x(%d), y(%d), layer(%d), MonsterID(%d)", pHuntaholicBase->nID, pRespawn->x, pRespawn->y, pRespawn->layer, pRespawn->monsterId );
						_cprint( "Unknown monster is to be respawned in huntaholic dungeon by pending. HuntaholicID(%d), x(%d), y(%d), layer(%d), MonsterID(%d)\n", pHuntaholicBase->nID, pRespawn->x, pRespawn->y, pRespawn->layer, pRespawn->monsterId );

						break;
					}

					if( pRespawn->initialEnemy )
						pMob->AddHate( pRespawn->initialEnemy, 1 );

					pMob->SetLifeTime( pRespawn->lifeTime );

					pInstance->vRespawnedMonster.push_back( pMob );
				}
			}
		}

		delete pRespawn;
	}
}

// HuntaholicManager
HuntaholicManager::~HuntaholicManager()
{
	if( IsInitialized() )
		DeInit();

	ClearHuntaholicInfo();
}

HuntaholicManager & HuntaholicManager::Instance()
{
	static HuntaholicManager _instance;

	return _instance;
}

const bool HuntaholicManager::Init()
{
	if( GameRule::bDisableHuntaholic )
		return false;

	if( IsInitialized() )
		return false;

	ArcadiaServer::Instance().SetObjectPriority( &HuntaholicManager::Instance(), ArSchedulerObject::UPDATE_PRIORITY_HIGH );

	return true;
}

const bool HuntaholicManager::DeInit()
{
	if( !IsInitialized() )
		return false;

	ArcadiaServer::Instance().SetObjectPriority( &HuntaholicManager::Instance(), ArSchedulerObject::UPDATE_PRIORITY_IDLE );

	// 진행 중이던 헌터홀릭 던전이 있는 상태에서 종료 또는 뒷 처리를 해야할 경우 여기서 하면 됨
	for( std::vector< HuntaholicInfo * >::iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		// 리젠 대기 중인 몬스터 데이터 제거
		{
			THREAD_SYNCHRONIZE( (*it)->csPendedRespawn );

			for( std::vector< GameContent::PENDED_RESPAWN_INFO * >::const_iterator itRespawn = (*it)->vPendedRespawn.begin() ; itRespawn != (*it)->vPendedRespawn.end() ; ++itRespawn )
			{
				delete (*itRespawn);
			}

			(*it)->vPendedRespawn.clear();
		}

		// 삭제되어야 할 던전 정보를 즉시 삭제하기 위해서 모으는 벡터
		std::vector< InstanceDungeon * > vPendedInstanceDungeonToBeDeleted;
		{
			THREAD_SYNCHRONIZE( (*it)->csInstanceDungeon );

			// 삭제 대기 중인 던전들을 즉시 삭제할 목록에 추가 후 대기 리스트 초기화
			vPendedInstanceDungeonToBeDeleted.swap( (*it)->vPendedInstanceDungeonToBeDeleted );
			(*it)->stInstanceNo.clear();

			for( std::vector< InstanceDungeon * >::iterator itInstance = (*it)->vInstanceDungeon.begin() ; itInstance != (*it)->vInstanceDungeon.end() ; /* 루프에서 ++itInstance 처리 */ )
			{
				InstanceDungeon * pInstance = (*itInstance);

				THREAD_SYNCHRONIZE( pInstance->csInstance );

				if( pInstance->bBegin && !pInstance->bEnd )
				{
					pInstance->endHunting();

					itInstance = (*it)->vInstanceDungeon.erase( itInstance );
				}
				else
				{
					// 사냥 시작한 던전 아니면 파티 박살내고 방 정보 바로 삭제해버림(이쯤 되면 메모리 누수만 안 생기면 어떻게 되도 별 상관없다 -_ -; 어차피 서버 종료되는 중...)
					PartyManager::GetInstance().DestroyParty( (*itInstance)->nPartyID );

					++itInstance;
				}

				vPendedInstanceDungeonToBeDeleted.push_back( pInstance );
			}
		}

		for( std::vector< InstanceDungeon * >::const_iterator itInstance = vPendedInstanceDungeonToBeDeleted.begin() ; itInstance != vPendedInstanceDungeonToBeDeleted.end() ; ++itInstance )
		{
			// 삭제 전에 락을 한 번 걸었다 풀어서 다른 프로세스가 인스턴스 정보를 사용 중일 가능성을 낮춘 후에 삭제
			// 그래봐야 여기서 거는 락 때문에 기다리고 있는 쓰레드가 생기면 말짱 꽝이지만,
			// 어차피 서버 내려가고 있는데 그딴 거 알 게 뭐삼! 걍 지워!(서버 종료되면서 불필요한 덤프가 남는 주 원인이 될 수도 있음... ``;)
			{
				THREAD_SYNCHRONIZE( (*itInstance)->csInstance );
			}
			delete (*itInstance);
		}
	}

	return true;
}

const bool HuntaholicManager::IsInitialized() const
{
	return GetFinalPriority() != ArSchedulerObject::UPDATE_PRIORITY_IDLE;
}

const bool HuntaholicManager::ClearHuntaholicInfo()
{
	if( IsInitialized() )
		return false;

	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		// beginHunting에서 몬스터/프랍 리젠 정보를 사용하지만 ClearHuntaholicInfo가 호출되는 상황이면 beginHunting이 처리 중일 수는 없으므로 락 안 걸고 삭제
		for( std::vector< const GameContent::HUNTAHOLIC_INSTANCE_BASE * >::const_iterator itInstance = (*it)->pHuntaholicBase->vInstanceBase.begin() ; itInstance != (*it)->pHuntaholicBase->vInstanceBase.end() ; ++itInstance )
		{
			for( std::vector< const GameContent::HUNTAHOLIC_MONSTER_RESPAWN_INFO * >::const_iterator itRespawn = (*itInstance)->vRespawnInfo.begin() ; itRespawn != (*itInstance)->vRespawnInfo.end() ; ++itRespawn )
			{
				delete (*itRespawn);
			}

			for( std::vector< const GameContent::FIELD_PROP_RESPAWN_INFO * >::const_iterator itHealingProp = (*itInstance)->vHealingPropInfo.begin() ; itHealingProp != (*itInstance)->vHealingPropInfo.end() ; ++itHealingProp )
			{
				delete (*itHealingProp);
			}

			delete (*itInstance);
		}

		delete (*it);
	}
	m_vHuntaholicInfo.clear();

	return true;
}

const bool HuntaholicManager::RegisterHuntaholicBase( const GameContent::HUNTAHOLIC_BASE * pHuntaholicBase )
{
	assert( !IsInitialized() );

#ifdef _DEBUG
	// 디버그 모드일 때만 이미 등록된 헌터홀릭 정보인지 확인
	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		if( (*it)->pHuntaholicBase->nID == pHuntaholicBase->nID )
			return false;
	}
#endif

	m_vHuntaholicInfo.push_back( new HuntaholicInfo( pHuntaholicBase ) );

	return true;
}

const int HuntaholicManager::GetHuntaholicID( const ArPosition & pos ) const
{
	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		unsigned int nRegionX = pos.GetRX();
		unsigned int nRegionY = pos.GetRY();

		if( ( nRegionX >= (*it)->pHuntaholicBase->nDungeonRegionLeft && nRegionX <= (*it)->pHuntaholicBase->nDungeonRegionRight &&
			nRegionY >= (*it)->pHuntaholicBase->nDungeonRegionTop && nRegionY <= (*it)->pHuntaholicBase->nDungeonRegionBottom ) ||
			( nRegionX >= (*it)->pHuntaholicBase->nLobbyRegionLeft && nRegionX <= (*it)->pHuntaholicBase->nLobbyRegionRight &&
			nRegionY >= (*it)->pHuntaholicBase->nLobbyRegionTop && nRegionY <= (*it)->pHuntaholicBase->nLobbyRegionBottom ) )
			return (*it)->pHuntaholicBase->nID;
	}

	return 0;
}

const bool HuntaholicManager::IsHuntaholicLobby( const ArPosition & pos ) const
{
	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		unsigned int nRegionX = pos.GetRX();
		unsigned int nRegionY = pos.GetRY();

		if( nRegionX >= (*it)->pHuntaholicBase->nLobbyRegionLeft && nRegionX <= (*it)->pHuntaholicBase->nLobbyRegionRight &&
			nRegionY >= (*it)->pHuntaholicBase->nLobbyRegionTop && nRegionY <= (*it)->pHuntaholicBase->nLobbyRegionBottom )
			return true;
	}

	return false;
}

const bool HuntaholicManager::IsHuntaholicDungeon( const ArPosition & pos ) const
{
	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		unsigned int nRegionX = pos.GetRX();
		unsigned int nRegionY = pos.GetRY();

		if( nRegionX >= (*it)->pHuntaholicBase->nDungeonRegionLeft && nRegionX <= (*it)->pHuntaholicBase->nDungeonRegionRight &&
			nRegionY >= (*it)->pHuntaholicBase->nDungeonRegionTop && nRegionY <= (*it)->pHuntaholicBase->nDungeonRegionBottom )
			return true;
	}

	return false;
}

const ArPosition HuntaholicManager::GetLobbyPosition( const int nHuntaholicID ) const
{
	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		if( (*it)->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		return (*it)->pHuntaholicBase->posLobby;
	}

	assert( 0 );
	return ArPosition( 0, 0 );
}

const unsigned char HuntaholicManager::GetProperLobbyLayer( const int nHuntaholicID, const int nLevel ) const
{
	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		if( (*it)->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		return (*it)->pHuntaholicBase->GetProperLobbyLayer( nLevel );
	}

	assert( 0 );
	return GameRule::HUNTAHOLIC_UNUSABLE_LOBBY_LAYER;
}

const size_t HuntaholicManager::GetFreeInstanceCount( const int nHuntaholicID ) const
{
	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		if( (*it)->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		THREAD_SYNCHRONIZE( (*it)->csInstanceDungeon );

		return (*it)->GetFreeInstanceCount();
	}

	assert( 0 );
	return 0;
}

const size_t HuntaholicManager::GetUsedInstanceCount( const int nHuntaholicID, const int nLevel ) const
{
	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		if( (*it)->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		THREAD_SYNCHRONIZE( (*it)->csInstanceDungeon );

		if( (*it)->vInstanceDungeon.empty() )
			return 0;

		size_t nCount = 0;

		for( std::vector< InstanceDungeon * >::const_iterator itInstance = (*it)->vInstanceDungeon.begin() ; itInstance != (*it)->vInstanceDungeon.end() ; ++itInstance )
		{
			if( !nLevel || (*itInstance)->pInstanceBase->IsProperLevel( nLevel ) )
				++nCount;
		}

		return nCount;
	}

	assert( 0 );
	return 0;
}

const size_t HuntaholicManager::GetInstanceList( const int nHuntaholicID, const int nLevel, const int nPage, TS_HUNTAHOLIC_INSTANCE_INFO * pBuffer )
{
	for( std::vector< HuntaholicInfo * >::const_iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		if( (*it)->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		THREAD_SYNCHRONIZE( (*it)->csInstanceDungeon );

		if( (*it)->vInstanceDungeon.size() <= GameRule::HUNTAHOLIC_MAX_INSTANCE_COUNT_PER_PAGE * ( nPage - 1 ) )
			return 0;

		size_t nValidCount = 0;
		size_t nTotalCount = 0;

		for( std::vector< InstanceDungeon * >::const_iterator itInstance = (*it)->vInstanceDungeon.begin() ; itInstance != (*it)->vInstanceDungeon.end() ; ++itInstance )
		{
			// 적정 레벨에 해당되지 않는 방은 카운트도 안 하고 건너 뜀
			if( !(*itInstance)->pInstanceBase->IsProperLevel( nLevel ) )
				continue;

			// 이미 사냥을 시작한 방은 없는셈 쳐버림(시작 버튼을 누르자마자 목록에서는 사라지는 걸로 처리)
			if( (*itInstance)->tBeginTime )
				continue;

			// 선택한 페이지 수 만큼의 개수는 건너 뛰기
			if( ++nTotalCount <= GameRule::HUNTAHOLIC_MAX_INSTANCE_COUNT_PER_PAGE * ( nPage - 1 ) )
				continue;

			assembleInstanceInfo( (*itInstance), pBuffer + nValidCount );

			if( ++nValidCount >= GameRule::HUNTAHOLIC_MAX_INSTANCE_COUNT_PER_PAGE )
				break;
		}

		return nValidCount;
	}

	assert( 0 );
	return 0;
}

const unsigned short HuntaholicManager::CreateInstanceDungeon( const int nHuntaholicID, struct StructPlayer * pPlayer, const char * szName, const int nMaxMember, const char * szPassword )
{
	if( !IsInitialized() )
		return RESULT_ACCESS_DENIED;

	if( !IsHuntaholicLobby( pPlayer->GetPos() ) )
		return RESULT_ACCESS_DENIED;

	if( pPlayer->IsInParty() )
		return RESULT_NOT_ACTABLE;

	if( pPlayer->IsGameTimeLimited() && pPlayer->GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
		return RESULT_GAMETIME_LIMITED;

	// 나쁜 파티 이름
	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, szName, static_cast< int >( strlen( szName ) + 1 ), 1, GameRule::HUNTAHOLIC_MAX_INSTANCE_NAME_LENGTH ) ||
		GameContent::IsBannedWord( code_page, szName ) )
		return RESULT_INVALID_TEXT;

	// 최대 인원 수가 비정상적인 값인 경우
	if( nMaxMember != 4 && nMaxMember != 6 && nMaxMember != 8 )
		return RESULT_INVALID_ARGUMENT;

	for( std::vector< HuntaholicInfo * >::iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		HuntaholicInfo * pHuntaholicInfo = (*it);

		if( pHuntaholicInfo->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		THREAD_SYNCHRONIZE( pHuntaholicInfo->csInstanceDungeon );

		if( pHuntaholicInfo->IsInstanceFull() )
			return RESULT_LIMIT_MAX;

		const GameContent::HUNTAHOLIC_INSTANCE_BASE * pInstanceBase = NULL;
		for( std::vector< const GameContent::HUNTAHOLIC_INSTANCE_BASE * >::const_iterator itInstance = pHuntaholicInfo->pHuntaholicBase->vInstanceBase.begin() ; itInstance != pHuntaholicInfo->pHuntaholicBase->vInstanceBase.end() ; ++itInstance )
		{
			if( (*itInstance)->IsProperLevel( pPlayer->GetLevel() ) )
			{
				pInstanceBase = (*itInstance);
				break;
			}
		}
		if( !pInstanceBase )
			return RESULT_NOT_ACTABLE;

		// 헌터홀릭 시스템용 파티 이름 생성(외부의 파티 이름과 중첩되지 않도록 유저가 지정한 이름 뒤에 추가 문자를 붙임)
		std::string strHuntaholicPartyName;
		int nSuffix;
		int nPartyID = 0;
		for( nSuffix = 0 ; nSuffix < GameRule::HUNTAHOLIC_MAX_INSTANCE_COUNT ; ++nSuffix )
		{
			XStringUtil::Format( strHuntaholicPartyName, "%s<%d_h>", szName, nSuffix );

			nPartyID = PartyManager::GetInstance().MakeParty( strHuntaholicPartyName.c_str(), pPlayer->GetPlayerUID(), PartyManager::TYPE_HUNTAHOLIC_PARTY );
			if( nPartyID > 0 )
				break;
		}

		if( nSuffix >= GameRule::HUNTAHOLIC_MAX_INSTANCE_COUNT )
			return RESULT_ALREADY_EXIST;

		if( !PartyManager::GetInstance().JoinParty( nPartyID, pPlayer ) )
		{
			PartyManager::GetInstance().DestroyParty( nPartyID );
			return RESULT_UNKNOWN;
		}

		// TS_SC_HUNTAHOLIC_BEGIN_HUNTING 메시지에서 사용되는 헌터홀릭 시작 시간 
		SendTimeSync( pPlayer );
		SendPartyInfo( pPlayer );

		LOG::Log11N4S( LM_PARTY_CREATE, pPlayer->GetAccountID(), pPlayer->GetSID(), pPlayer->GetPartyID(), pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), 4, PartyManager::TYPE_HUNTAHOLIC_PARTY, 0, 0, 0,
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, strHuntaholicPartyName.c_str(), LOG::STR_NTS, "", 0 );

		unsigned char nInstanceNo = pHuntaholicInfo->GetNewInstanceNo();
		InstanceDungeon * pInstance = new InstanceDungeon( pHuntaholicInfo, pInstanceBase, nInstanceNo, pPlayer->GetPartyID(), nMaxMember, szPassword );

		pHuntaholicInfo->stInstanceNo.insert( nInstanceNo );
		pHuntaholicInfo->vInstanceDungeon.push_back( pInstance );

		pInstance->broadcastInstanceDungeonInfoToMembers();

		LOG::Log11N4S( LM_HUNTAHOLIC_CREATE, pPlayer->GetAccountID(), pPlayer->GetSID(), nHuntaholicID, pInstance->pInstanceBase->nID, pInstance->nInstanceNo, pInstance->nMaxMemberCount, !!pInstance->szPassword[ 0 ], (*it)->vInstanceDungeon.size(), pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, strHuntaholicPartyName.c_str(), LOG::STR_NTS, szPassword, LOG::STR_NTS );

		return RESULT_SUCCESS;
	}

	assert( 0 );
	return RESULT_NOT_EXIST;
}

const unsigned short HuntaholicManager::JoinInstanceDungeon( const int nHuntaholicID, struct StructPlayer * pPlayer, const int nInstanceNo, const char * szPassword )
{
	if( !IsInitialized() )
		return RESULT_ACCESS_DENIED;

	if( !IsHuntaholicLobby( pPlayer->GetPos() ) )
		return RESULT_ACCESS_DENIED;

	if( pPlayer->IsInParty() )
		return RESULT_ALREADY_EXIST;

	if( pPlayer->IsGameTimeLimited() && pPlayer->GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
		return RESULT_GAMETIME_LIMITED;

	for( std::vector< HuntaholicInfo * >::iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		if( (*it)->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		THREAD_SYNCHRONIZE( (*it)->csInstanceDungeon );

		for( std::vector< InstanceDungeon * >::iterator itInstance = (*it)->vInstanceDungeon.begin() ; itInstance != (*it)->vInstanceDungeon.end() ; ++itInstance )
		{
			InstanceDungeon * pInstance = (*itInstance);

			if( pInstance->nInstanceNo != nInstanceNo )
				continue;

			// 이미 시작된 방이면 조인 불가(시작 처리가 아닌 시작 버튼을 누른 시점부터 입장 불가)
			if( pInstance->tBeginTime )
				return RESULT_COOL_TIME;

			THREAD_SYNCHRONIZE( pInstance->csInstance );

			return pInstance->joinInstanceDungeon( pPlayer, szPassword );
		}

		// 유저가 가지고 있는 방 목록이 갱신되지 않은 상태에서 방이 없어지면
		// 그 방에 조인을 시도할 경우 여기까지 오게 됨.(정상적인 경우이므로 assert 없음)
		return RESULT_NOT_EXIST;
	}

	assert( 0 );
	return RESULT_NOT_EXIST;
}

const unsigned short HuntaholicManager::LeaveInstanceDungeon( const int nHuntaholicID, struct StructPlayer * pPlayer )
{
	if( !IsHuntaholicLobby( pPlayer->GetPos() ) )
		return RESULT_ACCESS_DENIED;

	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID )
		return RESULT_NOT_EXIST;

	for( std::vector< HuntaholicInfo * >::iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		HuntaholicInfo * pHuntaholicInfo = (*it);

		if( pHuntaholicInfo->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		THREAD_SYNCHRONIZE( pHuntaholicInfo->csInstanceDungeon );

		for( std::vector< InstanceDungeon * >::iterator itInstance = pHuntaholicInfo->vInstanceDungeon.begin() ; itInstance != pHuntaholicInfo->vInstanceDungeon.end() ; ++itInstance )
		{
			InstanceDungeon * pInstance = (*itInstance);

			if( pInstance->nPartyID != nPartyID )
				continue;

			std::string strPartyName( PartyManager::GetInstance().GetPartyName( pInstance->nPartyID ) );

			// 사냥 시작 후일 경우 탈퇴 불가(QuitHunting 함수를 통해 사냥을 종료하고 나가야 함)
			if( pInstance->bBegin )
				return RESULT_COOL_TIME;

			THREAD_SYNCHRONIZE( pInstance->csInstance );

			// 사냥 시작 후일 경우 탈퇴 불가(락 걸고 한 번 더 검사)
			if( pInstance->bBegin )
				return RESULT_COOL_TIME;

			unsigned short nErrorCode = pInstance->leaveInstanceDungeon( pPlayer );
			if( nErrorCode != RESULT_SUCCESS )
				return nErrorCode;

			LOG::Log11N4S( LM_HUNTAHOLIC_LEAVE, pPlayer->GetAccountID(), pPlayer->GetSID(), nHuntaholicID, pInstance->pInstanceBase->nID, pInstance->nInstanceNo, pInstance->nMaxMemberCount, !!pInstance->szPassword[ 0 ], PartyManager::GetInstance().GetMemberCount( pInstance->nPartyID ), pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
				pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, strPartyName.c_str(), LOG::STR_NTS, "", 0 );

			// 아무도 남지 않은 경우 방 정보 삭제 대기 처리(아직 시작되지도 않았던 방이므로 몹/힐링프랍 없으므로 그냥 방 정보만 삭제하면 됨)
			if( !PartyManager::GetInstance().GetMemberCount( pInstance->nPartyID ) )
			{
				LOG::Log11N4S( LM_HUNTAHOLIC_DESTROY, pPlayer->GetAccountID(), pPlayer->GetSID(), nHuntaholicID, pInstance->pInstanceBase->nID, pInstance->nInstanceNo, pInstance->nMaxMemberCount, !!pInstance->szPassword[ 0 ], 0, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, strPartyName.c_str(), LOG::STR_NTS, "", 0 );

				pHuntaholicInfo->vInstanceDungeon.erase( itInstance );

				pHuntaholicInfo->vPendedInstanceDungeonToBeDeleted.push_back( pInstance );
			}

			return RESULT_SUCCESS;
		}

		// 여기 걸리는 건 방은 예전에 폭파됐는 데 사람은 남았다가 사람이 방에서 나가려고 하는 경우(로비에서 방 가입된 상태로 서버 다운 후 로그인하는 경우)
		return RESULT_NOT_EXIST;
	}

	assert( 0 );
	return RESULT_NOT_EXIST;
}

const unsigned short HuntaholicManager::BeginHunting( const int nHuntaholicID, struct StructPlayer * pPlayer )
{
	if( !IsHuntaholicLobby( pPlayer->GetPos() ) )
		return RESULT_ACCESS_DENIED;

	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID )
		return RESULT_NOT_EXIST;

	for( std::vector< HuntaholicInfo * >::iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		HuntaholicInfo * pHuntaholicInfo = (*it);

		if( pHuntaholicInfo->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		THREAD_SYNCHRONIZE( pHuntaholicInfo->csInstanceDungeon );

		for( std::vector< InstanceDungeon * >::iterator itInstance = pHuntaholicInfo->vInstanceDungeon.begin() ; itInstance != pHuntaholicInfo->vInstanceDungeon.end() ; ++itInstance )
		{
			InstanceDungeon * pInstance = (*itInstance);

			if( pInstance->nPartyID != nPartyID )
				continue;

			if( !PartyManager::GetInstance().IsLeader( pInstance->nPartyID, pPlayer->GetPlayerUID() ) )
				return RESULT_NOT_OWN;

			// 이미 시작 버튼이 눌린 던전은 시작 요청 불가
			if( pInstance->tBeginTime )
				return RESULT_ALREADY_EXIST;

			THREAD_SYNCHRONIZE( pInstance->csInstance );

			// 이미 시작 버튼이 눌린 던전은 시작 요청 불가
			if( pInstance->tBeginTime )
				return RESULT_ALREADY_EXIST;

			// 파티원 중 입장 불가 대상이 있는지 체크
			struct AdmissionChecker : public PartyManager::PartyFunctor
			{
				AdmissionChecker()
					: bAllAdmittable( true )
				{}

				virtual bool operator()( AR_HANDLE handle )
				{
					if( !bAllAdmittable )
						return false;

					StructPlayer::iterator itPlayer = StructPlayer::get( handle );
					StructPlayer * pPlayer = (*itPlayer);

					if( !pPlayer || !HuntaholicManager::Instance().IsHuntaholicLobby( pPlayer->GetPos() ) || !pPlayer->GetHuntaholicEnterableCount() )
					{
						bAllAdmittable = false;
						return false;
					}

					return true;
				}

				bool bAllAdmittable;
			} foAdmissionChecker;

			PartyManager::GetInstance().DoEachMember( pInstance->nPartyID, foAdmissionChecker );

			if( !foAdmissionChecker.bAllAdmittable )
				return RESULT_NOT_ACTABLE;

			// 방 시작 방송
			struct BeginCountdownBroadcaster : public PartyManager::PartyFunctor
			{
				virtual bool operator()( AR_HANDLE handle )
				{
					StructPlayer::iterator itPlayer = StructPlayer::get( handle );
					StructPlayer * pPlayer = (*itPlayer);

					if( !pPlayer )
						return false;

					pPlayer->AddHuntaholicEnterableCount( -1 );

					SendTimeSync( pPlayer );

					PendMessage( pPlayer, &msg );

					return true;
				}

				TS_SC_HUNTAHOLIC_BEGIN_COUNTDOWN msg;
			} foBeginCountdownBroadcaster;

			PartyManager::GetInstance().DoEachMember( pInstance->nPartyID, foBeginCountdownBroadcaster );

			pInstance->tBeginTime = GetArTime() + GameRule::HUNTAHOLIC_DUNGEON_BEGIN_COUNTDOWN_TIME;

			LOG::Log11N4S( LM_HUNTAHOLIC_BEGIN_COUNTDOWN, 0, 0, pHuntaholicInfo->pHuntaholicBase->nID, pInstance->pInstanceBase->nID, pInstance->nInstanceNo, pInstance->nMaxMemberCount, !!pInstance->szPassword[ 0 ], PartyManager::GetInstance().GetMemberCount( pInstance->nPartyID ), 0, 0, 0,
				"", 0, "", 0, PartyManager::GetInstance().GetPartyName( pInstance->nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

			return RESULT_SUCCESS;
		}

		break;
	}

	return RESULT_NOT_EXIST;
}

const unsigned short HuntaholicManager::QuitHunting( const int nHuntaholicID, struct StructPlayer * pPlayer, const bool bNeedToReward, const HuntaholicManager::_HUNTING_RESULT eResult )
{
	int nPartyID = pPlayer->GetPartyID();
	if( !nPartyID )
		return RESULT_ACCESS_DENIED;

	for( std::vector< HuntaholicInfo * >::iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		HuntaholicInfo * pHuntaholicInfo = (*it);

		if( pHuntaholicInfo->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		THREAD_SYNCHRONIZE( pHuntaholicInfo->csInstanceDungeon );

		for( std::vector< InstanceDungeon * >::iterator itInstance = pHuntaholicInfo->vInstanceDungeon.begin() ; itInstance != pHuntaholicInfo->vInstanceDungeon.end() ; /* 루프에서 ++itInstance 처리 */ )
		{
			InstanceDungeon * pInstance = (*itInstance);
			if( pInstance->nPartyID != nPartyID )
			{
				++itInstance;
				continue;
			}

			std::string strPartyName( PartyManager::GetInstance().GetPartyName( pInstance->nPartyID ) );

			{
				THREAD_SYNCHRONIZE( pInstance->csInstance );

				// eResult 가 지정되지 않았을 경우(eResult == HUNTING_RESULT_UNKNOWN) 목표 점수에 도달하지 못했으면 실질적으로 retire 처리
				// bNeedToReward == false 여도 죽었으면 fail_by_death, retire 처리(실제로 bNeedToReward == false이면 eResult는 참조되지도 않음 -_ -)
				_HUNTING_RESULT eFinalResult = eResult;
				if( eFinalResult == HUNTING_RESULT_UNKNOWN )
				{
					if( pPlayer->IsDead() )
						eFinalResult = HUNTING_RESULT_FAILED_BY_DEATH;
					else if( pInstance->nScore < pHuntaholicInfo->pHuntaholicBase->nObjectivePoint )
					{
						if( pPlayer->IsGameTimeLimited() && pPlayer->GetContinuousPlayTime() >= GameRule::nMaxTiredGameTime )
							eFinalResult = HUNTING_RESULT_FAILED_BY_GAMETIME_LIMIT;
						else
							eFinalResult = HUNTING_RESULT_RETIRED;
					}
					else if( !bNeedToReward )
						eFinalResult = HUNTING_RESULT_RETIRED;
					else
						eFinalResult = HUNTING_RESULT_SUCCESS;
				}

				int nSuppliedRewardPoint = 0;
				unsigned short nErrorCode = pInstance->quitHunting( pPlayer, bNeedToReward, eFinalResult, &nSuppliedRewardPoint );

				if( nErrorCode != RESULT_SUCCESS )
					return nErrorCode;

				// 로비로 워프 처리
				{
					ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

					pPlayer->PendWarp( pHuntaholicInfo->pHuntaholicBase->posLobby.x, pHuntaholicInfo->pHuntaholicBase->posLobby.y, pInstance->pInstanceBase->nID );
					ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
				}

				// 성공해서 보상을 지급받았다면 지급된 보상을 랭킹에 적용
				if( nSuppliedRewardPoint )
				{
					RankingManager::Instance().AddRankingScore( RankingManager::RANKING_TYPE_HUNTAHOLIC_THIS_MONTH, pPlayer->GetPlayerUID(), pPlayer->GetName(), nSuppliedRewardPoint, true );
					RankingManager::Instance().AddRankingScore( RankingManager::RANKING_TYPE_HUNTAHOLIC_TOTAL, pPlayer->GetPlayerUID(), pPlayer->GetName(), nSuppliedRewardPoint, true );
				}

				// 아무도 남지 않은 경우 방 정보 삭제 대기 처리
				if( !PartyManager::GetInstance().GetMemberCount( nPartyID ) )
				{
					pInstance->endHunting();

					LOG::Log11N4S( LM_HUNTAHOLIC_DESTROY, pPlayer->GetAccountID(), pPlayer->GetSID(), nHuntaholicID, pInstance->pInstanceBase->nID, pInstance->nInstanceNo, pInstance->nMaxMemberCount, !!pInstance->szPassword[ 0 ], 0, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(),
						pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, strPartyName.c_str(), LOG::STR_NTS, "", 0 );

					itInstance = pHuntaholicInfo->vInstanceDungeon.erase( itInstance );

					pHuntaholicInfo->vPendedInstanceDungeonToBeDeleted.push_back( pInstance );
				}
			}

			break;
		}

		// 로그인 시 헌터홀릭 내에서 파티 중이었을 경우(섭다 등으로 인한 로그아웃 시 처리 안 된 상태) 파티 탈퇴시키고 로비로 이동만이라도 시켜야 함
		if( pPlayer->IsInParty() )
		{
			if( PartyManager::GetInstance().IsLeader( nPartyID, pPlayer->GetPlayerUID() ) )
			{
				LOG::Log11N4S( LM_PARTY_DESTROY, pPlayer->GetAccountID(), pPlayer->GetSID(), nPartyID, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), 10, 0, 0, 0, 0,
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

				BroadcastPartyDestroy( nPartyID );

				PartyManager::GetInstance().DestroyParty( nPartyID );
			}
			else
			{
				BroadcastPartyLeave( pPlayer );

				LOG::Log11N4S( LM_PARTY_LEAVE, pPlayer->GetAccountID(), pPlayer->GetSID(), nPartyID, pPlayer->GetX(), pPlayer->GetY(), pPlayer->GetLayer(), 4, 0, 0, 0, 0,
					pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

				PartyManager::GetInstance().LeaveParty( nPartyID, pPlayer->GetPlayerUID() );
			}

			// 파티 탈퇴가 제대로 되었어야 함
			assert( !PartyManager::GetInstance().IsMember( nPartyID, pPlayer->GetPlayerUID() ) );
		}

		unsigned char nLayer = pHuntaholicInfo->pHuntaholicBase->GetProperLobbyLayer( pPlayer->GetLevel() );

		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

			// quitHunting 에서 로비로 빼는 처리가 안된 경우에도 파탈 처리 제대로 안된 경우처럼 워프
			if( IsHuntaholicDungeon( pPlayer->GetPos() ) && !pPlayer->IsPendWarp() )
			{
				if( nLayer != GameRule::HUNTAHOLIC_UNUSABLE_LOBBY_LAYER )
					pPlayer->PendWarp( pHuntaholicInfo->pHuntaholicBase->posLobby.GetX(), pHuntaholicInfo->pHuntaholicBase->posLobby.GetY(), nLayer );
				else
				{
					ArPosition posReturn;
					pPlayer->GetPositionOnEnterInstanceGame( &posReturn );
					pPlayer->PendWarp( posReturn.GetX(), posReturn.GetY(), 0 );
				}
				ArcadiaServer::Instance().SetObjectPriority( pPlayer, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
			}
		}

		return RESULT_SUCCESS;
	}

	assert( 0 );
	return RESULT_NOT_EXIST;
}

void HuntaholicManager::PendRespawnMonster( const int nHuntaholicID, const AR_UNIT x, const AR_UNIT y, const unsigned char nLayer, const int nMonsterID, const int nRespawnCount, const AR_TIME nLifeTime, const AR_HANDLE hInitialEnemy )
{
	for( std::vector< HuntaholicInfo * >::iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		if( (*it)->pHuntaholicBase->nID != nHuntaholicID )
			continue;

		// 지역 락을 걸고 호출되는 함수이므로 csInstanceDungeon을 걸면 안됨
		{
			THREAD_SYNCHRONIZE( (*it)->csPendedRespawn );

			(*it)->vPendedRespawn.push_back( new GameContent::PENDED_RESPAWN_INFO( x, y, nLayer, nMonsterID, nRespawnCount, true, nLifeTime, hInitialEnemy/* , 0*/ ) );
		}

		return;
	}

	assert( 0 );
}

void HuntaholicManager::onProcess( int nThreadIdx )
{
	char buf[255];
	s_sprintf( buf, _countof( buf ), "thread.scheduler.%d.proc", nThreadIdx );
	ENV().Set( buf, "HuntaholicManager" );

	AR_TIME t = GetArTime();

	s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "HuntaholicManager(0x%08X)", (UINT_PTR)this );
	s_ThreadInfo.last_execute_time = t;

	if( !IsInitialized() )
		return;

	for( std::vector< HuntaholicInfo * >::iterator it = m_vHuntaholicInfo.begin() ; it != m_vHuntaholicInfo.end() ; ++it )
	{
		HuntaholicInfo * pHuntaholic = (*it);

		{
			THREAD_SYNCHRONIZE( pHuntaholic->csInstanceDungeon );

			// 삭제 대기 중인 인스턴스 던전 삭제 처리
			// * 현재 생성되어 있는 방들을 처리하기 전에 이걸 먼저 처리함으로써
			//   이번 onProcess에서 삭제 대기 상태가 된 인스턴스를 다음 onProcess에서 삭제하도록 여유 시간을 발생시킴.
			for( std::vector< InstanceDungeon * >::const_iterator itInstace = pHuntaholic->vPendedInstanceDungeonToBeDeleted.begin() ; itInstace != pHuntaholic->vPendedInstanceDungeonToBeDeleted.end() ; ++itInstace )
			{
				// 삭제 전에 락을 한 번 걸었다 풀어서 다른 프로세스가 인스턴스 정보를 사용 중일 가능성을 낮춘 후에 삭제
				// 그래봐야 여기서 거는 락 때문에 기다리고 있는 쓰레드가 생기면 말짱 꽝이지만,
				// 인스턴스 정보가 vPendedInstanceDungeonToBeDelete 리스트에 추가되고 여기까지 도달하기 전에 어지간한 것들은 다 종료되기에 충분할 것으로 가정
				{
					THREAD_SYNCHRONIZE( (*itInstace)->csInstance );
				}
				pHuntaholic->stInstanceNo.erase( (*itInstace)->nInstanceNo );
				delete (*itInstace);
			}
			pHuntaholic->vPendedInstanceDungeonToBeDeleted.clear();

			// 현재 생성되어 있는 인스턴스 던전 처리
			for( std::vector< InstanceDungeon * >::iterator itInstance = pHuntaholic->vInstanceDungeon.begin() ; itInstance != pHuntaholic->vInstanceDungeon.end() ; /* 루프에서 ++itInstance 처리 */ )
			{
				InstanceDungeon * pInstance = (*itInstance);

				THREAD_SYNCHRONIZE( pInstance->csInstance );

				// 사냥 시작 전 상태인 방
				if( !pInstance->bBegin )
				{
					// 파티가 정상적으로 유지되어 있는지 확인(일종의 유효성 검사, 정상적이면 LeaveInstanceDungeon에서 방이 삭제처리되었어야 함)
					if( !PartyManager::GetInstance().GetMemberCount( pInstance->nPartyID ) )
					{
						assert( 0 );
						PartyManager::GetInstance().DestroyParty( pInstance->nPartyID );

						itInstance = pHuntaholic->vInstanceDungeon.erase( itInstance );

						pHuntaholic->vPendedInstanceDungeonToBeDeleted.push_back( pInstance );

						continue;
					}

					// 시작 버튼이 눌린 상태이고 시작 시각을 넘어갔으면 시작(시작 버튼 누르고 10초 후)
					if( pInstance->tBeginTime && t >= pInstance->tBeginTime )
					{
						pInstance->beginHunting();
					}
				}
				// 사냥 중인 방
				else
				{
					// 사냥이 다 끝났거나 사냥 시간이 만료됐거나 아무도 없는 방 제거
					if( pInstance->bEnd || pInstance->tBeginTime + pHuntaholic->pHuntaholicBase->tHuntingPeriod * 100 < t ||
						!PartyManager::GetInstance().GetMemberCount( pInstance->nPartyID ) )
					{
						pInstance->endHunting();

						itInstance = pHuntaholic->vInstanceDungeon.erase( itInstance );

						pHuntaholic->vPendedInstanceDungeonToBeDeleted.push_back( pInstance );

						continue;
					}

					// 종료 시간 1분 전 미만이고 공지된 적 없는 던전에는 종료 예정 공지
					if( !pInstance->bEndNoticed && pInstance->tBeginTime + pHuntaholic->pHuntaholicBase->tHuntingPeriod * 100 - GameRule::HUNTAHOLIC_DUNGEON_END_NOTICE_TIME < t )
					{
						pInstance->bEndNoticed = true;

						struct DungeonEndNotifier : PartyManager::PartyFunctor
						{
							virtual bool operator()( AR_HANDLE handle )
							{
								StructPlayer::iterator itPlayer = StructPlayer::get( handle );
								StructPlayer * pPlayer = (*itPlayer);

								if( !pPlayer )
									return false;

								PrintfChatMessage( false, CHAT_HUNTAHOLIC_SYSTEM, "@HUNTAHOLIC", pPlayer, "@1111\v#@min@#\v%d", static_cast< int >( GameRule::HUNTAHOLIC_DUNGEON_END_NOTICE_TIME / 6000 ) );
								return true;
							}
						} fo;

						PartyManager::GetInstance().DoEachMember( pInstance->nPartyID, fo );
					}

					// 종료랑 관계 없는 방이면 리젠 처리
					pInstance->procMonsterRespawn( t );
					pInstance->procHealingPropRespawn( t );
				}

				++itInstance;
			}
		}

		// 리젠 대기 상태인 몬스터를 리젠 처리
		// 위의 처리가 끝나고 하는 이유는 종료될 방은 종료시키고 나서 해야 쓸데없는 몹 리젠/삭제가 반복되지 않기 때문
		// 아예 pHuntaholic->csInstanceDungeon 걸기 전에 
		pHuntaholic->ProcPendedMonsterRespawn();
	}
}

void HuntaholicManager::assembleInstanceInfo( const InstanceDungeon * pInstance, TS_HUNTAHOLIC_INSTANCE_INFO * pInfo )
{
	pInfo->instance_no = pInstance->nInstanceNo;
	s_strcpy( pInfo->name, _countof( pInfo->name ), PartyManager::GetInstance().GetPartyName( pInstance->nPartyID ).c_str() );
	pInfo->current_member_count = static_cast< unsigned char >( PartyManager::GetInstance().GetMemberCount( pInstance->nPartyID ) );
	pInfo->max_member_count = pInstance->nMaxMemberCount;
	pInfo->require_password = *pInstance->szPassword;
}
