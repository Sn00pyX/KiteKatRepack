// 코드 정리 해야함.

#include <ctime>

#include <network/XIOCPConnection.h>
#include <toolkit/IQueue.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>
#include <logging/FileLog.h>
#include <dump/XExceptionHandler.h>
#include <toolkit/nsl.h>
#include <mmo/ArRegion.h>
#include <toolkit/XRandom.h>
#include <toolkit/XEnv.h>
#include <toolkit/khash.h>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "GameMessage.h"
#include "SendMessage.h"
#include "StructCreature.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructItem.h"
#include "StructSkill.h"
#include "StructMonster.h"
#include "Constant.h"
#include "GameAllocator.h"
#include "StructNPC.h"
#include "LuaVM.h"
#include "Extern.h"
#include "DB_Commands.h"
// #include "DB_CheckSecurityNo.h" 2009. 8. 11 floyd 보안비밀번호 2차 수정
#include "GameContent.h"
#include "GameProc.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "StructWorldLocation.h"
#include "DungeonManager.h"
#include "HuntaholicManager.h"
#include "InstanceDungeonManager.h"
#include "BattleArenaManager.h"
#include "MixManager.h"
#include "ChannelManager.h"
#include "AuctionManager.h"
#include "CompeteManager.h"
#include "RankingManager.h"
#include "TimeUtil.h"
#include "XSecuritySolutionManager.h"
#include "NetworkReceiverPerformanceTracker.h"
#include "../Community/PartyMatchingManager.h"

// Requester 기능을 사용해야 할만한 나쁜 놈들 줄 파일 빌드할 때 사용하자
//#define USE_REQUESTER

#ifdef USE_REQUESTER
#include <cipher/XStrZlibWithSimpleCipherUtil.h>
#endif

extern volatile LONG g_nRecvMessageCount;
extern volatile LONG g_nRecvMessageByte;
extern IStreamSocketConnection *	g_pAuthConnection;
extern IStreamSocketConnection *	g_pUploadConnection;
extern IStreamSocketConnection *	g_pCoordinatorConnection;

__declspec( thread ) StructPlayer * s_pCurrentPlayer		= NULL;

static XCriticalSection	s_AuthAccountCS( "s_AuthAccountCS" );
static KHash< IStreamSocketConnection *, hashPr_string_nocase > s_hsAuthAccount;

extern XCriticalSection											g_ConnectionTagLock;

extern __declspec( thread ) XSEH::THREAD_INFO s_ThreadInfo;

extern KHash< char, hashPr_mod_int >							g_hsDonationRewardDropGroup;

StructPlayer *GetCurrentThreadPlayer()
{
	return s_pCurrentPlayer;
}

inline bool isValidTarget( StructPlayer *pClient, GameObject *pObj )
{
	return IsVisibleRegion( pClient->GetRX(), pClient->GetRY(), pObj->GetRX(), pObj->GetRY() );
}

//#define _NOAUTH

void onLogin( IStreamSocketConnection * pConnection, TS_LOGIN* pMsg )
{
	// DB 처리 중인 사항이 있을 경우 로그인 불가(캐릭터 삭제 요청 직후 로그인 문제)
	if( static_cast< XIOCPConnection * >( pConnection )->GetVar() )
		return;

	if( GameRule::bUseLoginLogoutDebug )
	{
		_ctprint( "onLogin (%s)\n", pMsg->szName );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "LoginLogout_Debug", "onLogin (%s)", pMsg->szName );
	}

	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

#ifndef _NOAUTH
	if( !pTag->szAccountName[0] )
	{
		_cprint( "Illegal login try detected :%s\n", pMsg->szName );
		pConnection->Close();
		return;
	}
#endif // _NOAUTH

	std::vector< std::string >::iterator itPlayerName;

	for( itPlayerName = pTag->vCharacterNameList.begin(); itPlayerName != pTag->vCharacterNameList.end(); ++itPlayerName )
	{
		if( !_stricmp( (*itPlayerName).c_str(), pMsg->szName ) )
		{
			break;
		}
	}

	if( itPlayerName == pTag->vCharacterNameList.end() )
	{
		FILELOG( "Login try with a character which is beloned to another account detected :%s [%s]", pMsg->szName, pTag->szAccountName );
		_cprint( "Login try with a character which is beloned to another account detected :%s [%s]\n", pMsg->szName, pTag->szAccountName );
		pConnection->Close();

		return;
	}



	AR_HANDLE hPlayer = StructPlayer::FindPlayer( pMsg->szName );
	if( hPlayer )
	{
		FILELOG( "Login try with a character which is already logged in :%s [%s]", pMsg->szName, pTag->szAccountName );
		_cprint( "Login try with a character which is already logged in :%s [%s]\n", pMsg->szName, pTag->szAccountName );

		//pConnection->Close();

		StructPlayer::iterator pit = StructPlayer::get( hPlayer );
		StructPlayer *pPlayer = *pit;
		// pPlayer는 있는데 pConnection만 NULL인 경우가 발생(2006-08-18 16:25)
		// onDisconnect 호출 전에 onLogin이 다시 들어온 경우로 추정.
		// pConnection도 존재해야만 Close 하도록 조건 추가.
		if( pPlayer )
		{
			pPlayer->Save();

			IStreamSocketConnection* pConn = pPlayer->pConnection;
			if( pPlayer->IsLogin() )
			{
				pPlayer->LogoutNow( 1 );
			}
			if( pConn && pConn->IsConnected() )
			{
				SendDisconnectDesc( pConn, TS_SC_DISCONNECT_DESC::DISCONNECT_TYPE_ANOTHER_LOGIN );
				pConn->Close();
			}
		}
	}

	static_cast< XIOCPConnection* >( pConnection )->IncVar();

	char szName[31];
	s_strcpy( szName, _countof( szName ), pMsg->szName );

	// load character
	pTag->pPlayer = StructPlayer::AllocPlayer();
	pTag->pPlayer->pConnection = pConnection;
	pTag->pPlayer->SetAccount( pTag->nAccountID, pTag->szAccountName );

	// 기존에 있던 컨넥션에서 이미 창고 보안 비밀번호를 입력했고 이후에 입력하지 않게 되어 있었더라도
	// (game.use_storage_security == 1, game.check_storage_security_always == 0) 캐릭터 선택만 다시 한 경우
	// 다시 입력하게 보안 비밀번호 입력 상태를 초기화 함
	pTag->bStorageSecurityCheck = false;

	// _oprint( "ALLOC PLAYER 1 : 0x%08X\n", pTag->pPlayer );

	// 클라에서 알고 있어야 할 Url들을 전송
	// pConnection이 세팅된 후에 사용해야 함(SetConnection 함수가 호출된 후에)
	SendUrlList( pTag->pPlayer );

	DB().Push( new DB_Login( pMsg->szName, pTag->nAccountID, pTag->pPlayer, pTag->nPCBangMode, pTag->nEventCode, pTag->nAge, pTag->nContinuousPlayTime, pTag->nContinuousLogoutTime, pConnection ) );
}

void onRequestLogoutTimer( StructPlayer * pClient, int nMsg )
{
	if( GameRule::bUseLoginLogoutDebug )
	{
		_ctprint( "onRequestLogoutTimer (%s, %s)\n", pClient->GetAccountName(), pClient->GetName() );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "LoginLogout_Debug", "onRequestLogoutTimer (%s, %s)", pClient->GetAccountName(), pClient->GetName() );
	}

	// 로비로 돌아가기/로그아웃 요청이 왔을 때 즉시 로그아웃 불가능 상태일 경우
	// 타이머 요청 시점부터 로그아웃 가능 시간까지 불가능 처리를 위해 기록
	// CanLogoutNow 체크 이전에 설정해야 함
	pClient->SetLastLogoutRequestedTime( GetArTime() );

	if ( pClient->CanLogoutNow() )
		SendResult( pClient, nMsg, RESULT_SUCCESS, 0 );
	else
	{
		int nLogoutTime = GameRule::nLogoutTimer / 100;
		SendResult( pClient, nMsg, RESULT_COOL_TIME, nLogoutTime );
	}
}


void onLogout( StructPlayer * pClient, TS_CS_LOGOUT* pMsg )
{
	if( GameRule::bUseLoginLogoutDebug )
	{
		_ctprint( "onLogout(%s, %s)\n", pClient->GetAccountName(), pClient->GetName() );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "LoginLogout_Debug", "onLogout(%s, %s)", pClient->GetAccountName(), pClient->GetName() );
	}

	// 즉시 로그아웃 불가능할 경우 로그아웃 불가
	if( !pClient->CanLogoutNow() )
		return;

	if( pClient->GetBattleArenaID() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

		// 특정 경기에 참여 상태일 경우에는 경기 이탈
		if( pClient->GetBattleArenaID() )
			BattleArenaManager::Instance().QuitGame( pClient, false, false, ALT_USER_REQUEST );

		assert( !pClient->GetBattleArenaID() );
	}

	pClient->LogoutNowWithAccount( 4 );
}

void onHideEquipInfo( StructPlayer * pClient, TS_CS_HIDE_EQUIP_INFO * pMsg )
{
	if( pClient->GetHideEquipFlag() == pMsg->nHideEquipFlag )
		return;

	// 어차피 패킷 처리 중인 StructPlayer 객체는 메모리에서는 delete되지 않으며, 딱히 값이 이상해도 별로 상관 없으므로 락 없이 세팅함
	pClient->SetHideEquipFlag( pMsg->nHideEquipFlag );

	TS_SC_HIDE_EQUIP_INFO msg;

	msg.hPlayer = pClient->GetHandle();
	msg.nHideEquipFlag = pMsg->nHideEquipFlag;

	pClient->DBQuery( new DB_UpdateCharacterHideEquipFlag( pClient ) );

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	ArcadiaServer::Instance().Broadcast( pClient->GetRX(), pClient->GetRY(), pClient->GetLayer(), &msg );
}

bool	GetValidWayPoint( StructPlayer * pClient, StructCreature* pMObj, TS_MOVE_REQUEST* pMsg, std::vector< ArPosition >& vMoveInfo, ArPosition& startPos )
{
	ArPosition curPosFromServer = pMObj->GetCurrentPosition( GetArTime() );
	ArPosition wayPoint( pMsg->x, pMsg->y );

	// 1. 클라에서 보낸 현재 위치 좌표의 유효성 여부 검사
	if( wayPoint.x < 0 || wayPoint.x > g_nMapWidth || wayPoint.y < 0 || wayPoint.y > g_nMapHeight )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return false;
	}

	// 2. 클라에서 보낸 현재 위치가 서버에서 알고있는 현재 위치로 부터 가시거리(43.75 미터) 밖이면 즐
	if( curPosFromServer.GetDistance( wayPoint ) > GameRule::VISIBLE_RANGE )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return false;
	}

	if( pMObj->IsPlayer() )
	{
		// 서버의 현재 위치에서 클라이언트의 현재 위치까지 갈 수 있는가?
		if( GameContent::CollisionToLine( curPosFromServer.x, curPosFromServer.y, wayPoint.x, wayPoint.y ) )
		{
			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );

#ifdef _DEBUG
			_cprint( "LineCollision Detected While Server Pos To Client Pos PlayerUID[%d] Server[%f,%f] Client[%f,%f]\n",
				pClient->GetPlayerUID(), curPosFromServer.x, curPosFromServer.y, wayPoint.x, wayPoint.y );
			FILELOG( "LineCollision Detected While Server Pos To Client Pos PlayerUID[%d] Server[%f,%f] Client[%f,%f]",
				pClient->GetPlayerUID(), curPosFromServer.x, curPosFromServer.y, wayPoint.x, wayPoint.y );
#endif

			// 이동이 실패한다면 서버와 위치를 동기화 시켜 다시 시도하게 유도한다.
			// 서버에서 알고 있는 위치가 못가는 지역이라면 끼인다.
			if( GameContent::IsBlocked( curPosFromServer.x, curPosFromServer.y ) )
			{
#ifdef _DEBUG
				_cprint( "LineCollision (Server Pos To Client Pos) Detected And the User Got Stuck! - PlayerUID[%d] Pos[%f,%f]\n",
					pClient->GetPlayerUID(), curPosFromServer.x, curPosFromServer.y );
				FILELOG( "LineCollision (Server Pos To Client Pos) Detected And the User Got Stuck! - PlayerUID[%d] Pos[%f,%f]",
					pClient->GetPlayerUID(), curPosFromServer.x, curPosFromServer.y );
#endif
				return false;
			}

			startPos = curPosFromServer;
			vMoveInfo.clear();
			vMoveInfo.push_back( curPosFromServer );
			return true;
		}
	}

	TS_MOVE_REQUEST::MOVE_INFO * pMoveInfo = reinterpret_cast< TS_MOVE_REQUEST::MOVE_INFO * >( pMsg + 1 );
	for( unsigned short i = 0; i < pMsg->count; ++i, ++pMoveInfo )
	{
		// 유효성 검사
		// 3. 목표 이동 장소 좌표의 유효성 여부 검사
		if( pMoveInfo->tx < 0 || pMoveInfo->tx > g_nMapWidth || pMoveInfo->ty < 0 || pMoveInfo->ty > g_nMapHeight )
		{
			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
			return false;
		}

		if( pMObj->IsPlayer() )
		{
			// Player라면 LineCollision 여부 검증 (2014-01-02, 던전만 체크에서 전 필드로 확대)
			if( GameContent::CollisionToLine( wayPoint.x, wayPoint.y, pMoveInfo->tx, pMoveInfo->ty ) )
			{
				SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );

#ifdef _DEBUG
				_cprint( "LineCollision Detected While Pathfinding - %d/%d: PlayerUID[%d] Start[%f,%f] End[%f,%f]\n", i+1, pMsg->count, pClient->GetPlayerUID(), wayPoint.x, wayPoint.y, pMoveInfo->tx, pMoveInfo->ty );
				FILELOG( "LineCollision Detected While Pathfinding - %d/%d: PlayerUID[%d] Start[%f,%f] End[%f,%f]", i+1, pMsg->count, pClient->GetPlayerUID(), wayPoint.x, wayPoint.y, pMoveInfo->tx, pMoveInfo->ty );
#endif
				// 이동이 실패한다면 서버와 위치를 동기화 시켜 다시 시도하게 유도한다.
				// 서버에서 알고 있는 위치가 못가는 지역이라면 끼인다.
				// CollisionToLine을 확인 해도 가는 도중의 좌표가 끼이는 경우가 있는 것 같다.
				// 일단 목적지는 끼이지 않을수 있으니 일단 가자.
				if( GameContent::IsBlocked( curPosFromServer.x, curPosFromServer.y ) )
				{
#ifdef _DEBUG
					_cprint( "LineCollision Detected And the User Got Stuck! - PlayerUID[%d] Pos[%f,%f]\n", pClient->GetPlayerUID(), curPosFromServer.x, curPosFromServer.y );
					FILELOG( "LineCollision Detected And the User Got Stuck! - PlayerUID[%d] Pos[%f,%f]", pClient->GetPlayerUID(), curPosFromServer.x, curPosFromServer.y );
#endif
					return false;
				}

				startPos = curPosFromServer;
				vMoveInfo.clear();
				vMoveInfo.push_back( curPosFromServer );
				return true;
			}
		}
		else
		{
			// 펫은 유저가 이동을 조작할 수 있는 것이 아니니 아이템을 원활하게 줍기 위해 확인하지 않는다.
			// 크리처의 경우 마우스로 조작하기 때문에 키보드 이동보다는 좌표 유효성이 좋다고 판단한다.
			// 그리고 갈 수 없는 지역으로 갔다하더라도 그렇게 할 수 있는 일이 많지 않다.
		}

		wayPoint = ArPosition( pMoveInfo->tx, pMoveInfo->ty, 0 );
		vMoveInfo.push_back( wayPoint );
	}

	startPos = ArPosition( pMsg->x, pMsg->y );
	return true;
}

void onMoveRequest( StructPlayer * pClient, TS_MOVE_REQUEST* pMsg )
{
	if( pClient->IsDead() || !pClient->IsInWorld() ) return;

	AR_TIME t = GetArTime();

#ifdef _DEBUG
//	_cprint( "MoveReq : %f, %f\n", pMsg->x, pMsg->y );
#endif // _QUAD_DEBUG

	// 클라가 길찾기 하다가 서버를 죽이시는 알흠다운 사태가 발생하여..-_-
	if( pMsg->size != sizeof( TS_MOVE_REQUEST ) + pMsg->count * sizeof( TS_MOVE_REQUEST::MOVE_INFO ) )
	{
		return;
	}

	StructCreature* pMObj	= pClient;
	int speed				= pClient->GetRealMoveSpeed();

	if( pMsg->handle && pMsg->handle != pClient->GetHandle() )
	{
		pMObj = pClient->GetSummon( pMsg->handle );

		if( pMObj )
		{
			// 소환수인 경우 처리
			if( pClient->IsRiding() && pClient->GetRideObject() == pMObj )
			{
				pMObj = pClient;
			}
			else
			{
				speed = pMObj->GetRealMoveSpeed();

				if( pMsg->speed_sync )
				{
					speed = ( pClient->HasRidingState() )? pClient->GetRealRidingSpeed() : pClient->GetRealMoveSpeed();

					ArPosition pos( pMsg->x, pMsg->y );
					AR_UNIT distance = pos.GetDistance( pClient->GetPos() );
					if( distance >= GameRule::SUMMON_FOLLOWING_LIMIT_RANGE )
					{
						SendResult( pClient, pMsg->id, RESULT_TOO_FAR );
						return;
					}
					else if( distance >= GameRule::SUMMON_FOLLOWING_SECOND_SPEED_UP_RANGE )
					{
						speed *= GameRule::SUMMON_FOLLOWING_SECOND_SPEED_UP_RATE;
					}
					else if( distance >= GameRule::SUMMON_FOLLOWING_FIRST_SPEED_UP_RANGE )
					{
						speed *= GameRule::SUMMON_FOLLOWING_FIRST_SPEED_UP_RATE;
					}
				}
			}
		}
		else
		{
			// 펫인 경우 처리
			pMObj = pClient->GetSummonedPet();
			if( pMObj && pMObj->GetHandle() == pMsg->handle )
			{
				speed = pClient->GetRealMoveSpeed();

				ArPosition pos( pMsg->x, pMsg->y );
				AR_UNIT distance = pos.GetDistance( pClient->GetPos() );
				if( distance >= GameRule::PET_FOLLOWING_LIMIT_RANGE )
				{
					SendResult( pClient, pMsg->id, RESULT_TOO_FAR );
					return;
				}
				else if( distance >= GameRule::PET_FOLLOWING_HURRY_SPPED_RANGE )
				{
					speed *= GameRule::PET_FOLLOWING_HURRY_SPEED_RATE;
				}
				else
				{
					speed *= GameRule::PET_FOLLOWING_COMMON_SPEED_RATE;
				}
			}
		}
	}

	if( pMObj == pClient && ( pClient->IsRiding() || pClient->HasRidingState() ) )
	{
		speed = pClient->GetRealRidingSpeed();
	}

	if( !pMObj ) 
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
		return;
	}

	speed = speed < 1 ? 1 : speed;
	speed = speed > UCHAR_MAX ? UCHAR_MAX : speed;
	unsigned char moveSpeed = static_cast< unsigned char >( speed );

	std::vector< ArPosition > vMoveInfo;
	ArPosition start_pos;
	if( GetValidWayPoint( pClient, pMObj, pMsg, vMoveInfo, start_pos ) == false )
	{
		return;
	}

	if( vMoveInfo.size() < 1 )
		return;

	if( !StructCreature::QuadTreeItem::IsCanAdd( vMoveInfo.back().x, vMoveInfo.back().y ) )
	{
		SendResult( pClient, pMsg->id, RESULT_ALREADY_EXIST );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithSpecificRegion( pMObj, ::GetRegionX( start_pos.x ), ::GetRegionY( start_pos.y ) ) );

	if( !pClient->IsInWorld() )
		return;

	if( pMObj->IsAttacking() )
	{
		pMObj->CancelAttack();
	}

	// 배틀 아레나 경기장 내부에서 경기 시작 전에 이동하려는 시도라면 시작 영역 밖으로 나가지 못하게 이동 경로 제한
	if( pClient->IsInBattleArena() )
	{
		if( BattleArenaManager::Instance().ValidateMovableRoute( pClient->GetBattleArenaID(), pClient->GetBattleArenaInstanceNo(), pClient->GetPartyID(), pMObj->GetCurrentPosition( GetArTime() ), vMoveInfo ) )
		{
			// 경로가 조정됐는데 이동 점이 한 개도 안 남았다면 이동 불가
			if( vMoveInfo.empty() )
			{
				SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
				return;
			}
		}
	}

	if( pMObj->GetMovableTime() > t )
	{
		if( !pMObj->SetPendingMove( vMoveInfo, moveSpeed ) )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		}
		return;
	}

	if( !pMObj->IsActable() || !pMObj->IsInWorld() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	if( pClient == pMObj && pClient->IsSitDown() )
	{
		pClient->StandUp();
		BroadcastStatusMessage( pClient );
	}

	if( !pMObj->IsMovable() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	ArPosition pos = pMObj->GetCurrentPosition( GetArTime() );
	ArPosition targetPos = vMoveInfo.back();

	extern float g_fMapLength;

	// 이동 거리가 맵의 1/5 이상이며, 무저갱에서부터 출발했을 경우는 이동만 금지시킴.
	if( pMObj == pClient && pos.GetDistance( start_pos ) > ( g_fMapLength / 5 ) )
	{
		if( pClient->IsAutoUsed() && pClient->GetLocationId() == StructWorldLocation::LOCATION_ID_ABADON )
			return;
	}

	if( vMoveInfo.size() && pMObj->GetCurrentPosition( t ).GetDistance( targetPos ) > g_fMapLength )
	{
		return;
	}

	// _oprint( "%s CS( %f %f ) CC( %f %f ) -> ( %f %f )\n", pClient->GetName(), pMObj->GetCurrentPosition( t ).x, pMObj->GetCurrentPosition( t ).y, pMsg->x, pMsg->y, targetPos.x, targetPos.y );

	if( pMObj->HasPendingMove() )
	{
		pMObj->ReleasePendingMove();
	}

	if( pMObj == pClient && pClient->IsWarpEnded() )
	{
		pClient->SetWarpEnded( false );
	}

	ArcadiaServer::Instance().SetMultipleMove( pMObj, start_pos, vMoveInfo, moveSpeed , true, t );

	if( pMObj->IsPlayer() )
	{
		pClient->SetMoveReq( true );
	}

	if( pMObj->IsPlayer() && static_cast< StructPlayer * >( pMObj )->IsRiding() )
	{
		ArcadiaServer::Instance().SetMultipleMove( static_cast< StructPlayer * >( pMObj )->GetRideObject(), start_pos, vMoveInfo, moveSpeed , true, t );
	}

	if( pMObj->IsSummon() )
	{
		ArcadiaServer::Instance().SetObjectPriority( pMObj, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );
	}
}

void onTimeSync( StructPlayer * pClient, TS_TIMESYNC* pMsg )
{
#ifdef TIME_DEBUG
	_cprint( "RCV : TM_TIMESYNC (%d)\n", pMsg->time );
#endif

	pClient->m_TS.onEcho( GetArTime() - pMsg->time );

	if( pClient->m_TS.GetTestCount() < 4 )
	{
		SendTimeSync( pClient );
		return;
	}
#ifdef TIME_DEBUG
	_cprint( " AVERAGE TIME GAP : %d\n", pClient->m_TS.GetInterval() );
#endif

	TS_SET_TIME msg;
	msg.gap = pClient->m_TS.GetInterval();
	PendMessage( pClient, &msg );
}

void onGameTime( StructPlayer * pClient )
{
	SendGameTime( pClient );
}

void onPutonCard( StructPlayer * pClient, TS_CS_PUTON_CARD* pMsg )
{
	if( !pClient->IsItemWearable() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	StructItem * pItem = StructItem::FindItem( pMsg->item_handle );

	if( !pItem )
	{
		FILELOG( "onPutonItem() : item not found (%d)", pMsg->item_handle );
		_cprint( "onPutonItem() : item not found (%d)\n", pMsg->item_handle );
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
		return;
	}

	if( !pClient->FindItem( pItem->GetItemUID() ) )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( !pClient->PutOnBelt( pMsg->position, pItem ) )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	pClient->CalculateStat();

	SendBeltSlotInfo( pClient );
}

void onPutoffCard( StructPlayer * pClient, TS_CS_PUTOFF_CARD*  pMsg )
{
	if( !pClient->IsItemWearable() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	pClient->PutOffBelt( pMsg->position );

	pClient->CalculateStat();

	SendBeltSlotInfo( pClient );
}

void onPutonItemSet( StructPlayer * pClient, TS_CS_PUTON_ITEM_SET* pMsg )
{
	if( !pClient->IsItemWearable() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	for( int i = 0; i < ItemBase::MAX_ITEM_WEAR; ++i )
	{
		pClient->Putoff( static_cast< ItemBase::ItemWearType >( i ) );
		if( pMsg->handle[i] )
		{
			StructItem *pItem = StructItem::FindItem( pMsg->handle[i] );
			if( pItem )
			{
				pClient->Puton( static_cast< ItemBase::ItemWearType >( i ), pItem );
				//아래꺼 보내면 두번 날리는 셈.
				//SendItemWearInfoMessage( pClient, pClient, pItem );
			}
		}
	}

	pClient->CalculateStat();

	// 방송
	TS_WEAR_INFO msg;
	GetWearMsg( pClient, msg );
	ArcadiaServer::Instance().Broadcast( pClient->GetRX(), pClient->GetRY(), pClient->GetLayer(), &msg );	
}

void onPutonItem( StructPlayer * pClient, TS_CS_PUTON_ITEM* pMsg )
{
	if( !pClient->IsItemWearable() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	StructItem * pItem = StructItem::FindItem( pMsg->item_handle );

	if( !pItem )
	{
		FILELOG( "onPutonItem() : item not found (%d)", pMsg->item_handle );
		_cprint( "onPutonItem() : item not found (%d)\n", pMsg->item_handle );
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
		return;
	}

	if( !pItem->IsWearable() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	StructCreature::iterator it = StructCreature::get( pMsg->target_handle );
	StructCreature *pTarget = *it;

	ArcadiaLock lock( -1 );
	bool bLocked = false;

	if( !pTarget )
	{
		pTarget = pClient;
	}
	else if( pTarget->IsInWorld() )
	{
		lock = ArcadiaServer::Instance().LockObjects( pClient, pTarget );
		bLocked = true;

		// 락을 건 이후에 한 번 더 확인하여 월드에서 사라졌으면 락을 해제한다.
		if( !pTarget->IsInWorld() )
		{
			ArcadiaServer::Instance().UnLock( &lock );
			bLocked = false;
		}
	}

	// 최종적으로 걸린 락이 없다면 pTarget이 존재하지 않거나 월드에 없는 경우이다.
	// pTarget이 월드에 없더라도 클라이언트에서 온 핸들을 검증하기 위해 최소한의 락이 필요하다.
	// 따라서 두 경우 모두 pClient 중심으로 락을 건다.
	if( !bLocked )
	{
		lock = ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient );
		bLocked = true;
	}

	ARCADIA_LOCK( lock );

	if( !pClient->FindItem( pItem->GetItemUID() ) || ( pClient != pTarget && !pClient->GetSummon( pMsg->target_handle ) ) )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	float fOldSummonFCM = pTarget->GetSummonFCM();

	unsigned short nResult = pTarget->Puton( static_cast< ItemBase::ItemWearType >( pMsg->position ), pItem );
	if( nResult != RESULT_SUCCESS )
	{
		SendResult( pClient, pMsg->id, nResult );
		return;
	}

	pTarget->CalculateStat();
	// 소환수의 경우 주인 무게 스텟 관련 메시지 전달이 위에서 안되므로 따로 주인의 스탯 정보만 전달해 준다. (무게메시지)
	if( pTarget->IsSummon() )
	{
		SendStatInfo( pClient, pClient );
	}

	SendResult( pClient, pMsg->id, RESULT_SUCCESS );

	if( pTarget->IsPlayer() )
	{
		// 방송
		StructPlayer* pPlayer = static_cast<StructPlayer*>(pTarget);
		TS_WEAR_INFO msg;
		GetWearMsg( pClient, msg );
		ArcadiaServer::Instance().Broadcast( pClient->GetRX(), pClient->GetRY(), pClient->GetLayer(), &msg );

		if (fOldSummonFCM != pTarget->GetSummonFCM())
		{
			if (pPlayer->GetMainSummon())
			{
				pPlayer->GetMainSummon()->CalculateStat();
			}
			if (pPlayer->GetSubSummon())
			{
				pPlayer->GetSubSummon()->CalculateStat();
			}
		}

	}	


	//Puton안에서 모두 처리
	//SendItemWearInfoMessage( pClient, pTarget, pItem );
}

void onPutoffItem( StructPlayer * pClient, TS_CS_PUTOFF_ITEM* pMsg )
{
	if( !pClient->IsItemWearable() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	StructCreature::iterator it = StructCreature::get( pMsg->target_handle );
	StructCreature *pTarget = *it;

	ArcadiaLock lock( -1 );
	bool bLocked = false;

	if( !pTarget )
	{
		pTarget = pClient;
	}
	else if( pTarget->IsInWorld() )
	{
		lock = ArcadiaServer::Instance().LockObjects( pClient, pTarget );
		bLocked = true;

		// 락을 건 이후에 한 번 더 확인하여 월드에서 사라졌으면 락을 해제한다.
		if( !pTarget->IsInWorld() )
		{
			ArcadiaServer::Instance().UnLock( &lock );
			bLocked = false;
		}
	}

	// 최종적으로 걸린 락이 없다면 pTarget이 존재하지 않거나 월드에 없는 경우이다.
	// pTarget이 월드에 없더라도 클라이언트에서 온 핸들을 검증하기 위해 최소한의 락이 필요하다.
	// 따라서 두 경우 모두 pClient 중심으로 락을 건다.
	if( !bLocked )
	{
		lock = ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient );
		bLocked = true;
	}

	ARCADIA_LOCK( lock );

	if( pClient != pTarget && !pClient->GetSummon( pMsg->target_handle ) )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	StructItem *pItem = pTarget->GetWearedItemByAbsolutePos( static_cast< ItemBase::ItemWearType >( pMsg->position ) );

	if( !pItem )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
		return;
	}

	unsigned short nRet = RESULT_SUCCESS;

	float fOldSummonFCM = pTarget->GetSummonFCM();


	if( pTarget->IsSummon() )
	{
		// 크리처 전용 아이템은 같은 그룹에 두 개 이상의 아이템을 장비할 수 있으며 장착 위치가 고정되어있다.
		if( pItem->GetWearType() == ItemBase::WEAR_SUMMON_ONLY )
			nRet = pTarget->Putoff( static_cast< ItemBase::ItemWearType >( pMsg->position ) );
		else 
#ifdef USE_CLIENT97
			nRet = pTarget->Putoff(static_cast<ItemBase::ItemWearType>(pMsg->position));
#else
			nRet = pTarget->Putoff(pItem->GetWearType());
#endif
	}
	else
	{
		nRet = pTarget->Putoff( static_cast< ItemBase::ItemWearType >( pMsg->position ) );
	}

	pTarget->CalculateStat();
	// 소환수의 경우 주인 무게 스텟 관련 메시지 전달이 위에서 안되므로 따로 주인의 스탯 정보만 전달해 준다. (무게메시지)
	if( pTarget->IsSummon() )
	{
		SendStatInfo( pClient, pClient );
	}

	SendResult( pClient, pMsg->id, nRet );

	// 스왑용 슬롯에 임의로 장비하는 것은 불가능하나, 인벤토리의 아이템을 클릭하여 해제하는 것은 가능하다. 다만, 장비 방송은 필요없다.
	if( nRet == RESULT_SUCCESS && pTarget->IsPlayer() && pMsg->position < ItemBase::MAX_ITEM_WEAR )
	{
		// 벗은거 방송
		TS_WEAR_INFO msg;
		GetWearMsg( pClient, msg );
		ArcadiaServer::Instance().Broadcast( pClient->GetRX(), pClient->GetRY(), pClient->GetLayer(), &msg );
		StructPlayer* pPlayer = static_cast<StructPlayer*>(pTarget);
		if (fOldSummonFCM != pTarget->GetSummonFCM())
		{
			if (pPlayer->GetMainSummon())
			{
				pPlayer->GetMainSummon()->CalculateStat();
			}
			if (pPlayer->GetSubSummon())
			{
				pPlayer->GetSubSummon()->CalculateStat();
			}
		}

	}

	//Putoff안에서 모두 처리
	//SendItemWearInfoMessage( pClient, pTarget, pItem );
}

void onSwapEquip( StructPlayer * pClient, TS_CS_SWAP_EQUIP * pMsg )
{
	if( !pClient->IsItemWearable() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructItem *pWeapon				= pClient->GetWearedItem( ItemBase::WEAR_WEAPON );
	StructItem *pShield				= pClient->GetWearedItem( ItemBase::WEAR_SHIELD );
	StructItem *pDecoWeapon			= pClient->GetWearedItem( ItemBase::WEAR_DECO_WEAPON );
	StructItem *pDecoShield			= pClient->GetWearedItem( ItemBase::WEAR_DECO_SHIELD );
	StructItem *pSpareWeapon		= pClient->GetWearedItem( ItemBase::WEAR_SPARE_WEAPON );
	StructItem *pSpareShield		= pClient->GetWearedItem( ItemBase::WEAR_SPARE_SHIELD );
	StructItem *pSpareDecoWeapon	= pClient->GetWearedItem( ItemBase::WEAR_SPARE_DECO_WEAPON );
	StructItem *pSpareDecoShield	= pClient->GetWearedItem( ItemBase::WEAR_SPARE_DECO_SHIELD );

	// 장비 해제는 반드시 역순으로 해야 하며 장비 장착은 그 반대로 해야 한다.
	pClient->Putoff( ItemBase::WEAR_DECO_SHIELD );
	pClient->Putoff( ItemBase::WEAR_DECO_WEAPON );
	pClient->Putoff( ItemBase::WEAR_SHIELD );
	pClient->Putoff( ItemBase::WEAR_WEAPON );
	pClient->Putoff( ItemBase::WEAR_SPARE_DECO_SHIELD );
	pClient->Putoff( ItemBase::WEAR_SPARE_DECO_WEAPON );
	pClient->Putoff( ItemBase::WEAR_SPARE_SHIELD );
	pClient->Putoff( ItemBase::WEAR_SPARE_WEAPON );

	if( pWeapon )			pClient->Puton( ItemBase::WEAR_SPARE_WEAPON, pWeapon );
	if( pShield )			pClient->Puton( ItemBase::WEAR_SPARE_SHIELD, pShield );
	if( pDecoWeapon )		pClient->Puton( ItemBase::WEAR_SPARE_DECO_WEAPON, pDecoWeapon );
	if( pDecoShield )		pClient->Puton( ItemBase::WEAR_SPARE_DECO_SHIELD, pDecoShield );
	if( pSpareWeapon )		pClient->Puton( ItemBase::WEAR_WEAPON, pSpareWeapon );
	if( pSpareShield )		pClient->Puton( ItemBase::WEAR_SHIELD, pSpareShield );
	if( pSpareDecoWeapon )	pClient->Puton( ItemBase::WEAR_DECO_WEAPON, pSpareDecoWeapon );
	if( pSpareDecoShield )	pClient->Puton( ItemBase::WEAR_DECO_SHIELD, pSpareDecoShield );

	pClient->CalculateStat();

	// 방송
	TS_WEAR_INFO msg;
	GetWearMsg( pClient, msg );
	ArcadiaServer::Instance().Broadcast( pClient->GetRX(), pClient->GetRY(), pClient->GetLayer(), &msg );
}

void onRegionUpdate( StructPlayer * pClient, TS_REGION_UPDATE* pMsg )
{
#ifdef MOVE_DEBUG
	_cprint( "RCV : TM_CS_REGION_UPDATE\n" );
#endif	

	pClient->DecRegionUpdateNeedCount();

	AR_TIME t = GetArTime();

	ArPosition pos = pClient->GetCurrentPosition( t );

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithSpecificRegion( pClient, GetRegionX( pMsg->x ), GetRegionY( pMsg->y ) ) );

	if( !pClient->IsInWorld() )
		return;

	if( pClient->GetRideObject() )
	{
		ArcadiaServer::Instance().onRegionChange( pClient->GetRideObject(), pMsg->update_time, pMsg->bIsStopMessage );
	}

	//_oprint( "%s RegionUpdate %d %d\n", pClient->GetName(), pMsg->update_time, pMsg->bIsStopMessage );

	if( pMsg->bIsStopMessage )
	{
		pClient->SetMoveReq( false );
		//_oprint( "%s Arrived.\n", pClient->GetName() );
	}

	ArcadiaServer::Instance().onRegionChange( pClient, pMsg->update_time, pMsg->bIsStopMessage );
}

void onAttackRequest( StructPlayer * pClient, TS_ATTACK_REQUEST* pMsg )
{
	if( pClient->IsDead() ) return;

	StructCreature *pAttacker = pClient;	

	if( pMsg->handle != pClient->GetHandle() ) pAttacker = pClient->GetSummon( pMsg->handle );

	if( !pAttacker ) 
	{
		SendCantAttackMsg( pClient, pMsg->handle, pMsg->target_handle, RESULT_NOT_OWN );
		return;
	}

	{

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

		// _oprint( "%s ATTACK (%s) (%d) (%d)\n", pMsg->target_handle ? "START" : "END", pClient->GetName(), pMsg->target_handle, GetArTime() );

		if( !pMsg->target_handle ) 
		{
			if( pAttacker->IsAttacking() )
				pAttacker->CancelAttack();
			return;
		}

		StructCreature::iterator itTarget = StructCreature::get( pMsg->target_handle );

		if( !(*itTarget) )
		{
			if( pAttacker->IsAttacking() )
				pAttacker->EndAttack();
			else
				SendCantAttackMsg( pClient, pMsg->handle, pMsg->target_handle, RESULT_NOT_EXIST );

			return;
		}

		if( !pAttacker->IsEnemy( (*itTarget) ) )
		{
			if( pAttacker->IsAttacking() )
				pAttacker->EndAttack();
			else
				SendCantAttackMsg( pClient, pMsg->handle, pMsg->target_handle, RESULT_NOT_ACTABLE );

			return;
		}

		if( ( pAttacker->IsUsingBow() || pAttacker->IsUsingCrossBow() ) && pAttacker->IsPlayer() )
		{
			if( pAttacker->GetBulletCount() < 1 )
			{
				SendCantAttackMsg( pClient, pMsg->handle, pMsg->target_handle, RESULT_NOT_ENOUGH_BULLET );
				return;
			}
		}

		if( pClient == pAttacker && pClient->IsSitDown() )
		{
			pClient->StandUp();
			BroadcastStatusMessage( pClient );
		}

		if( !pAttacker->IsAttackable() )
		{
			SendCantAttackMsg( pClient, pMsg->handle, pMsg->target_handle, RESULT_ACCESS_DENIED );

			return;
		}
	}

	pAttacker->StartAttack( pMsg->target_handle );

	return;
}


void onDropItem( StructPlayer * pClient, TS_CS_DROP_ITEM* pMsg )
{
	__int64 count = pMsg->count;

	// 행동불가라면 KIN
	if( !pClient->IsActable() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	if( !count )
	{
		if( GameRule::bUseAutoJail )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

			pClient->AddState( StructState::NEMESIS_FOR_AUTO, pClient->GetHandle(), 12, GetArTime(), GetArTime() + 864000000 );
			pClient->SetAutoUsed();
			pClient->Save( true );
		}

		LOG::Log11N4S( LM_AUTO_USER_CHECKED, pClient->GetAccountID(), pClient->GetSID(), pClient->GetLevel(), pClient->GetJobLevel(), pClient->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::ZERO_ITEM_DROP_ATTEMPTION, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		return;
	}

	// lock
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructItem *pItem = StructItem::FindItem( pMsg->item_handle );

	if( !pItem )
	{
		//_cprint( "onDropItem() : 없는 아이템을 드랍하려 했음. %s (%d)\n", pClient->GetName(), pMsg->item_handle );
		SendDropResult( pClient, pMsg->item_handle, false );
		return;
	}

	if( !pClient->IsDropable( pItem ) || pItem->GetCount() < count )
	{
		SendDropResult( pClient, pMsg->item_handle, false );
		return;
	}

	// 소환수 카드는 일단 못떨어트림
	if( pItem->IsSummonCard() && pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON ) )
	{
		SendDropResult( pClient, pMsg->item_handle, false );
		return;
	}

	if( count < 0 ) count = pItem->GetCount();
	if( count > pItem->GetCount() ) count = pItem->GetCount();

	StructItem * pNewItem = pClient->DropItem( pItem, count );

	SendDropResult( pClient, pMsg->item_handle, !!pNewItem );
}

const AR_HANDLE procAddItem( StructPlayer * pClient, StructItem *pItem, bool bIsPartyProcess = true )
{
	ItemBase::ItemCode code = pItem->GetItemCode();

	if( !code && pClient->GetGold() + pItem->GetCount() > GameRule::MAX_GOLD_FOR_INVENTORY )
		return 0;

	pItem->SetIdx( 0 );
	StructItem *pNewItem = pClient->PushItem( pItem, pItem->GetCount() );

	// event item notice
	if( pItem->GetEventDropFlag() )
	{	
		pItem->SetEventDropFlag( false );
		int nFlag = 0;
		pItem->GetInstanceFlag().CopyTo( &nFlag, sizeof( nFlag ) );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "Event", "%20s\t%20u\t%20d\t%20u", pClient->GetName(), pClient->GetSID(), code, nFlag );

		if( GameRule::bBroadcastEventItemPickup )
		{
			PrintfGlobalChatMessage( CHAT_NOTICE, "@NOTICE", "@955\v#@picker_name@#\v%s\v#@item_name@#\v@%d", pClient->GetName(), pItem->GetItemBase().nNameId );
		}
	}

	ItemUID uid = pNewItem ? pNewItem->GetItemUID() : 0;
	AR_HANDLE item_handle = pNewItem ? pNewItem->GetHandle() : pItem->GetHandle();

	const char * szType = "PICK";

	if( bIsPartyProcess )
	{
		szType = "PPICK";
	}

	int x = pItem->GetX();
	int y = pItem->GetY();

	if( pItem->IsGold() )
	{
		LOG::Log11N4S( LM_ITEM_TAKE, pClient->GetAccountID(), pClient->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), code, 0, 0, pItem->GetCount(), pClient->GetGold().GetRawData(), x, y, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, szType, LOG::STR_NTS );
	}
	else
	{
		LOG::Log11N4S( LM_ITEM_TAKE, pClient->GetAccountID(), pClient->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), code, pItem->GetCount(), pNewItem ? pNewItem->GetCount() : 0, 0, 0, x, y, uid, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, szType, LOG::STR_NTS );
	}	

	if( pNewItem && pNewItem != pItem )
	{
		// _oprint( "?:" );
		StructItem::PendFreeItem( pItem );
	}

	// 결과
	//SendResult( pClient, TM_CS_TAKE_ITEM, RESULT_SUCCESS, item_handle );
	return item_handle;
}

bool addPartyItem( StructPlayer * pClient, StructItem *pItem )
{
	ItemBase::ItemCode nItemCode = pItem->GetItemCode();
	int nItemLevel = pItem->GetItemLevel();
	int nItemEnhance = pItem->GetItemEnhance();
	__int64 nItemCount = pItem->GetCount();
	AR_HANDLE hItemHandle = pItem->GetHandle();

	AR_HANDLE hResultItem = procAddItem( pClient, pItem );
	if( !hResultItem )
	{
		SendResult( pClient, TM_CS_TAKE_ITEM, RESULT_ACCESS_DENIED, hItemHandle );
		return false;
	}

	SendResult( pClient, TM_CS_TAKE_ITEM, RESULT_SUCCESS, hResultItem );
	PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, pClient->GetPartyID(), "TAKE_ITEM|%s|%d|%d|%d|%I64d|", pClient->GetAlias(), nItemCode, nItemLevel, nItemEnhance, nItemCount );
	return true;
}

// 루피 습득은 이 쪽으로 들어오면 최대 루피 소지 한도가 적용되지 않음
void procPartyShare( StructPlayer * pClient, StructItem *pItem )
{
	// 몹이 떨어트린 아이템만..
	if( !pItem->GetOwnerUID() )
	{
		// 파티 분배형 퀘스트 아이템일 경우 시야 내 파티원 전원이 습득
		if( pItem->IsQuestItem() && pItem->IsQuestDistributeItem() )
		{
			std::vector< StructPlayer * > vList;
			PartyManager::GetInstance().GetNearMember( pClient, 400, vList );

			std::vector< StructPlayer * >::iterator it;
			for( it = vList.begin() ; it != vList.end() ; ++it )
			{
				if( !(*it)->IsTakeableQuestItem( pItem->GetItemCode() ) )
				{
					continue;
				}

				StructItem *pNewItem = StructItem::AllocItem( 0, pItem->GetItemCode(), 1, ItemInstance::BY_MONSTER );
				pNewItem->CopyFrom( pItem );

				addPartyItem( (*it), pNewItem );
			}

			StructItem::PendFreeItem( pItem );

			return;
		}

		PartyManager::_ITEM_SHARE_MODE eShareMode = PartyManager::GetInstance().GetShareMode( pClient->GetPartyID() );
		switch( eShareMode )
		{
		case PartyManager::ITEM_SHARE_MONOPOLY:
			// 각자분배(습득자 독점)
			addPartyItem( pClient, pItem );
			break;

		case PartyManager::ITEM_SHARE_LINEAR:
		case PartyManager::ITEM_SHARE_RANDOM:
			// 순차/랜덤분배(둘 다 주변 플레이어 목록 긁어야 하므로 일단 같이 시작)
			{
				std::vector< StructPlayer * > vList;
				PartyManager::GetInstance().GetNearMember( pClient, GameRule::PARTY_ITEM_SHARE_RANGE, vList );

				// 무게 남는 사람만 줍게 해야함
				std::vector< StructPlayer * >::iterator it;
				for( it = vList.begin(); it != vList.end();  )
				{
					if( (*it)->GetMaxWeight() - (*it)->GetWeight() < pItem->GetCount() * pItem->GetItemBase().fWeight )
					{
						it = vList.erase( it );
						continue;
					}

					if( pItem->IsQuestItem() && !(*it)->IsTakeableQuestItem( pItem->GetItemCode() ) )
					{
						it = vList.erase( it );
						continue;
					}

					++it;
				}

				if( !vList.empty() )
				{
					if( pItem->IsQuestItem() )
					{
						addPartyItem( PartyManager::GetInstance().GetNextItemAcquirer( pClient, vList ), pItem );
					}
					else
					{
						// 랜덤 분배일 경우 우선권을 일괄지급하고 아이템 먹은 플레이어에게서 우선권을 뺌으로써
						// 한 명이 다른 플레이어보다 많은 아이템을 먹는 것을 방지
						if( eShareMode == PartyManager::ITEM_SHARE_RANDOM )
						{
							std::vector< StructPlayer * > vPlayerList( vList );

							for( it = vList.begin(); it != vList.end(); )
							{
								if( !(*it)->HasItemPriority() )
								{
									it = vList.erase( it );
									continue;
								}

								++it;
							}

							if( vList.empty() )
							{
								for( it = vPlayerList.begin(); it != vPlayerList.end(); ++it )
								{
									(*it)->SetItemPriority();
								}

								vList = vPlayerList;
							}
						}

						StructPlayer * pAcquirer = PartyManager::GetInstance().GetNextItemAcquirer( pClient, vList );
						addPartyItem( pAcquirer, pItem );

						pAcquirer->ReleaseItemPriority();
					}
				}
				else
				{
					// 사실 여기까진 오면 안되지만 ;
					addPartyItem( pClient, pItem );
				}
			}
			break;
		}
	}
	else
	{
		addPartyItem( pClient, pItem );		
	}
}

void onTakeItem( StructPlayer * pClient, TS_CS_TAKE_ITEM* pMsg )
{
	ArcadiaLock __lock = ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient );

	// 아이템 자동 줍기가 가능한 펫이 소환되어 있는 경우 플레이어와 펫을 포함하는 영역에 락 걸기
	StructCreature *pTaker = pClient;
	StructPet *pPet = pClient->GetSummonedPet();
	if( pPet && pPet->GetHandle() == pMsg->taker_handle && pPet->IsItemCollectable() && pPet->IsInWorld() )
	{
		// 지역 락 풀기 전에 pPet의 iterator 인스턴스를 만들어서 m_nRefCount를 증가시킴으로써 메모리에서 delete되는 걸 방지
		StructPet::iterator itPet = pPet;

		ArcadiaServer::Instance().UnLock( &__lock );

		__lock = ArcadiaServer::Instance().LockObjects( pClient, pPet );

		// 락을 건 이후에도 펫이 월드에 남아 있다면 펫이 아이템을 집음(없으면 유저가 시도하는 걸로 처리됨)
		if( pPet->IsInWorld() )
			pTaker = pPet;
	}

	ARCADIA_LOCK( __lock );

	if( pTaker != pClient && pTaker != pPet )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_OWN, pMsg->item_handle );
		return;
	}

	if( pTaker == pClient && !pClient->IsItemPickable() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE, pMsg->item_handle );
		return;
	}

	StructItem *pItem = NULL;
	StructItem::iterator it = StructItem::get( pMsg->item_handle );			

	if( (*it) && (*it)->IsItem() )
	{
		pItem = static_cast< StructItem* >( *it );
	}

	if( !pItem )
	{
		//_cprint( "onTakeItem() : 없는 아이템을 주우려 했음. %s (%d)\n", pClient->GetName(), pMsg->item_handle );
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST, pMsg->item_handle );
		return;
	}

	// 버그 있음

	if( pItem->GetWeight() > pClient->GetMaxWeight() - pClient->GetWeight() )
	{
		// _cprint( "onTakeItem() : 넘 무겁다. %s (%d)\n", pClient->GetName(), pMsg->item_handle );
		SendResult( pClient, pMsg->id, RESULT_TOO_HEAVY, pMsg->item_handle );
		return;
	}

	if( !pItem->IsInWorld() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST, pMsg->item_handle );
		return;
	}

	if( pItem->GetOwnerHandle() > 0 )
	{
		FILELOG( "onTakeItem() : this item has owner. %s (%d)", pClient->GetName(), pMsg->item_handle );
		_cprint( "onTakeItem() : this item has owner. %s (%d)\n", pClient->GetName(), pMsg->item_handle );
		SendResult( pClient, pMsg->id, RESULT_NOT_OWN, pMsg->item_handle );
		return;
	}

	if( pItem->GetMv().GetDistance( pTaker->GetCurrentPosition( GetArTime() ) ) > (GameRule::GetPickableRange() + (pTaker->GetUnitSize()/2)) )
	{
		// _cprint( "onTakeItem() : too far. %s (%d)\n", pClient->GetName(), pMsg->item_handle );
		SendResult( pClient, pMsg->id, RESULT_TOO_FAR, pMsg->item_handle );
		return;
	}

	if( pItem->IsQuestItem() && !pClient->IsTakeableQuestItem( pItem->GetItemCode() ) ) 
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED, pMsg->item_handle );
		return;
	}

	// { 먹자 방지
	const StructItem::ITEM_PICKUP_ORDER & ItemPickupOrder = pItem->GetPickupOrder();
	AR_TIME drop_duration = GetArTime() - pItem->GetDropTime();

	for( int i = 0; i < 3; ++i )
	{
		if( !ItemPickupOrder.hPlayer[i] && ItemPickupOrder.nPartyID[i] == 0 )
		{
			// 드랍그룹 지정 안되어 있음
			break;
		}

		if( (ItemPickupOrder.nPartyID[i] > 0 && ItemPickupOrder.nPartyID[i] == pClient->GetPartyID()) || ItemPickupOrder.hPlayer[i] == pClient->GetHandle() )
		{
			// 해당 그룹이시면 드실 수 있슴당.
			break;
		}

		// 해당 그룹 아니신 분들은 시간 안되시면 못드심당
		if( drop_duration < GameRule::GetPickupOrderTime( i ) )
		{
			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED, pMsg->item_handle );
			return;
		}
	}
	// }

	// 소지금 한도 체크(파티일 때는 일단 없애버리고 보므로 체크 불필요)
	if( pItem->IsGold() && !pClient->IsInParty() && pClient->GetGold() + pItem->GetCount() > GameRule::MAX_GOLD_FOR_INVENTORY )
	{
		SendResult( pClient, pMsg->id, RESULT_TOO_MUCH_MONEY, pMsg->item_handle );
		return;
	}

	TS_SC_TAKE_ITEM_RESULT resultMsg;

	resultMsg.item_handle = pMsg->item_handle;
	resultMsg.item_taker = pTaker->GetHandle();

	ArcadiaServer::Instance().Broadcast( pClient->GetRX(), pClient->GetRY(), pClient->GetLayer(), &resultMsg );

	// 월드에서 제거(ItemCollector::onProcess에서 삭제 중인 아이템이었을 경우 실패함)
	if( !RemoveItemFromWorld( pItem ) )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST, pMsg->item_handle );
		return;
	}

	// 파티 소속일 경우
	if( pClient->IsInParty() )
	{
		if( pItem->IsGold() )
		{
			// 파티원에게 균등분배
			std::vector< StructPlayer * > vList;
			PartyManager::GetInstance().GetNearMember( pClient, 400, vList );

			StructGold incGold( pItem->GetCount() / (float)vList.size() );

			const char * szType = "PICK";
			if( vList.size() == 1 ) szType = "PPICK";

			for( std::vector< StructPlayer * >::iterator it = vList.begin(); it != vList.end(); ++it )
			{	
				StructGold nNewGold = (*it)->GetGold() + incGold;

				if( (*it)->ChangeGold( nNewGold ) != RESULT_SUCCESS )
				{
					PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", (*it), "@578" );
					LOG::Log11N4S( LM_ITEM_TAKE, (*it)->GetAccountID(), (*it)->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), 0, 0, 0, nNewGold.GetRawData(), pClient->GetX(), pClient->GetY(), 0, (*it)->GetAccountName(), LOG::STR_NTS, (*it)->GetName(), LOG::STR_NTS, "", 0, szType, LOG::STR_NTS );
					continue;
				}

				LOG::Log11N4S( LM_ITEM_TAKE, (*it)->GetAccountID(), (*it)->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), 0, 0, incGold.GetRawData(), nNewGold.GetRawData(), pClient->GetX(), pClient->GetY(), 0, (*it)->GetAccountName(), LOG::STR_NTS, (*it)->GetName(), LOG::STR_NTS, "", 0, szType, LOG::STR_NTS );
			}

			// 아이템 삭제
			// _oprint( "W:" );
			StructItem::PendFreeItem( pItem );
		}
		else
		{
			procPartyShare( pClient, pItem );
		}
	}
	else
	{
		AR_HANDLE hResultItem = procAddItem( pClient, pItem, false );
		if( !hResultItem )
		{
			// 절대로 습득에 성공했어야 하지만 습득을 못했을 경우 월드에서도 이미 사라졌고
			// 습득도 실패했으므로 로그 남기고 날려버린다... Orz...
			assert( 0 );

			StructItem::PendFreeItem( pItem );
			SendResult( pClient, TM_CS_TAKE_ITEM, RESULT_ACCESS_DENIED, pMsg->item_handle );
			return;
		}

		SendResult( pClient, TM_CS_TAKE_ITEM, RESULT_SUCCESS, hResultItem );
	}
}

void onEraseItem( StructPlayer *pClient, TS_CS_ERASE_ITEM* pMsg )
{
	if( pMsg->item_count > GameRule::MAX_ERASE_ITEM_COUNT
		|| pMsg->size != sizeof( TS_CS_ERASE_ITEM ) + ( pMsg->item_count * sizeof( TS_CS_ERASE_ITEM::EraseItemInfo ) ) )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	if( !pClient->IsActable() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	// 부적형 아이템 등으로 인해 아이템 목록에 변화가 생긴다면 시야 범위에 락을 건다.
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	TS_CS_ERASE_ITEM::EraseItemInfo *pEraseItemInfoList = reinterpret_cast< TS_CS_ERASE_ITEM::EraseItemInfo * >( pMsg + 1 );
	StructItem *pEraseItemList[ GameRule::MAX_ERASE_ITEM_COUNT ];
	memset( pEraseItemList, 0, sizeof( pEraseItemList ) );

	char szBuffer[ SENDMSG_BUFFER_SIZE ];

	TS_SC_ERASE_ITEM msg;
	msg.item_count = pMsg->item_count;
	msg.size += sizeof(TS_SC_ERASE_ITEM::EraseItemInfo) * msg.item_count;
	s_memcpy( szBuffer, sizeof( szBuffer ), &msg, sizeof(msg) );

	for( unsigned char i = 0; i < pMsg->item_count; ++i )
	{
		TS_SC_ERASE_ITEM::EraseItemInfo *pInfo = reinterpret_cast< TS_SC_ERASE_ITEM::EraseItemInfo * >( szBuffer + sizeof(msg) + i * sizeof(TS_SC_ERASE_ITEM::EraseItemInfo) );

		StructItem *pItem = StructItem::FindItem( pEraseItemInfoList[i].item_handle );

		if( !pItem )
		{
			pInfo->item_handle = 0;
			pInfo->count = 0;
			continue;
		}

		int nItemEnhance				= pItem->GetItemEnhance();
		int nItemLevel					= pItem->GetItemLevel();
		AR_HANDLE hHandle				= pItem->GetHandle();
		__int64 nItemCount				= pItem->GetCount();
		ItemUID nItemUID	= pItem->GetItemUID();

		bool result;
		const char * szType = "DESTORY_INVENTORY";
		
#ifdef USE_CLIENT97
		result = pClient->EraseItem(pItem, pEraseItemInfoList[i].count);
#else
		if( pEraseItemInfoList[i].is_in_storage ) 
		{
			result = pClient->EraseItemFromStorage( pItem, pEraseItemInfoList[i].count );
			szType = "DESTORY_STORAGE";
		}
		else
		{
			result = pClient->EraseItem(pItem, pEraseItemInfoList[i].count);
		}

#endif

		if( result )
		{
			LOG::Log11N4S( LM_ITEM_DELETE, 
				pClient->GetAccountID(), pClient->GetSID(), 
				nItemEnhance * 100 + nItemLevel, pItem->GetItemCode(), 
				nItemCount, pEraseItemInfoList[i].count,
				pClient->GetGold().GetRawData(), 
				0, pClient->GetX(), 
				pClient->GetY(), nItemUID, 
				pClient->GetAccountName(), LOG::STR_NTS, 
				pClient->GetName(), LOG::STR_NTS,
				"SUCS", LOG::STR_NTS, 
				szType, LOG::STR_NTS );

			pInfo->item_handle = hHandle;
			pInfo->count = pEraseItemInfoList[i].count;
		}
		else 
		{
			LOG::Log11N4S( LM_ITEM_DELETE, 
				pClient->GetAccountID(), pClient->GetSID(), 
				nItemEnhance * 100 + nItemLevel, pItem->GetItemCode(), 
				nItemCount, pEraseItemInfoList[i].count,
				pClient->GetGold().GetRawData(), 
				0, pClient->GetX(), 
				pClient->GetY(), nItemUID, 
				pClient->GetAccountName(), LOG::STR_NTS, 
				pClient->GetName(), LOG::STR_NTS, 
				"FAIL", LOG::STR_NTS, 
				szType, LOG::STR_NTS );

			pInfo->item_handle = 0;
			pInfo->count = 0;
		}
	}

	PendStream( pClient, szBuffer, msg.size );
}

static bool isCommand( const char *pString, const char *pCmd )
{
	if( strlen(pString) < strlen(pCmd) ) return false;								// by Testors;
	if( pString[strlen(pCmd)] && pString[strlen(pCmd)] != ' ' ) return false;		// by Testors;
	return !_strnicmp( pString, pCmd, strlen(pCmd ) );
}

void onCheatBlockChat( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	// C 등급은 사용할 수 없다.
	int nPermission = pPlayer->GetPermission();
	if( nPermission == GameRule::PERMISSION_FOR_C_GRADE )
	{
		return;
	}

	if( vToken.size() < 2 || vToken.size() > 3 )
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832" );
		return;
	}

	AR_HANDLE hTarget = StructPlayer::FindPlayer( vToken[1].c_str() );

	if( !hTarget )
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		return;
	}

	StructPlayer::iterator it = StructPlayer::get( hTarget );
	StructPlayer * pTarget = (*it);

	if( !pTarget )
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		return;
	}

	if( vToken.size() == 3 )
	{
		char *endPtr;
		int nBlockTime = (int) strtol( vToken[2].c_str(), &endPtr, 10 );

		if( *endPtr != NULL || nBlockTime < 0  || nBlockTime > GameRule::MAX_BLOCK_TIME ) 
		{
			PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832");
			return;
		}

		char szScript[1024] = { 0, };
		s_sprintf( szScript, _countof( szScript ), "set_chat_block_time( \"%s\", %d )", pTarget->GetName(), nBlockTime );

		LUA()->RunString( szScript );

		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@1653\v#@player_name@#\v%s\v#@block_time@#\v%d", pTarget->GetName(), nBlockTime );
	}
	else if( vToken.size() == 2 )
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@1654\v#@player_name@#\v%s\v#@remaining_time@#\v%d", pTarget->GetName(), pTarget->GetChatBlockTime() );
	}
}

void onRequestGuildIconInfo( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	int nGuildID = atoi( vToken.back().c_str() );

	int nGuildIconSize = GuildManager::GetInstance().GetGuildIconSize( nGuildID );
	std::string strGuildIconURL;
	std::string strGuildName;

	if( nGuildIconSize )
	{
		strGuildIconURL = ENV().GetString( "game.guild_icon_base_url" );
		strGuildIconURL += GuildManager::GetInstance().GetGuildIconFileName( nGuildID );
	}

	strGuildName = GuildManager::GetInstance().GetGuildName( nGuildID );

	PrintfChatMessage( true, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "GICON|%d|%s|%s|%d", nGuildID, strGuildName.c_str(), strGuildIconURL.c_str(), nGuildIconSize );
}

void onRequestGuildBannerInfo( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	int nGuildID = atoi( vToken.back().c_str() );

	int nGuildBannerSize = GuildManager::GetInstance().GetGuildBannerSize( nGuildID );
	std::string strGuildBannerURL;
	std::string strGuildName;

	if( nGuildBannerSize )
	{
		strGuildBannerURL = ENV().GetString( "game.guild_banner_base_url" );
		strGuildBannerURL += GuildManager::GetInstance().GetGuildBannerFileName( nGuildID );
	}

	strGuildName = GuildManager::GetInstance().GetGuildName( nGuildID );

	PrintfChatMessage( true, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "GBANNER|%d|%s|%s|%d", nGuildID, strGuildName.c_str(), strGuildBannerURL.c_str(), nGuildBannerSize );
}

void onCheatWalk( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

	if( !_stricmp( vToken.back().c_str(), "on" ) )	pPlayer->SetWalk( true );
	else											pPlayer->SetWalk( false );

	BroadcastStatusMessage( pPlayer );
}

void onCheatRebirth( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	char szScript[1024] = { 0, };
	s_sprintf( szScript, _countof( szScript ), "rebirth( \"%s\" )", pPlayer->GetName() );

	LUA()->RunString( szScript );
}

void onCheatSetAutoUser( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() == 2 )
	{
		StructPlayer::iterator it = StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) );
		StructPlayer* pAutoPlayer = *it;
		if( pAutoPlayer != NULL )
		{
			char szScript[1024] = { 0, };
			s_sprintf( szScript, _countof( szScript ), "set_auto_user( 1, \"%s\" )", pAutoPlayer->GetName() );

			LUA()->RunString( szScript );
		}
		else
		{
			PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		}
	}
	else
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832" );
	}
}

void onCheatUnsetAutoUser( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() == 2 )
	{
		StructPlayer::iterator it = StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) );
		StructPlayer* pAutoPlayer = *it;
		if( pAutoPlayer != NULL )
		{
			char szScript[1024] = { 0, };
			s_sprintf( szScript, _countof( szScript ), "set_auto_user( 0, \"%s\" )", pAutoPlayer->GetName() );

			LUA()->RunString( szScript );
		}
		else
		{
			PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		}
	}
	else
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832" );
	}
}

void onCheatCheckAutoUser( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() == 2 )
	{
		StructPlayer::iterator it = StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) );
		StructPlayer* pAutoPlayer = *it;
		if( pAutoPlayer != NULL )
		{
			char szScript[1024] = { 0, };
			s_sprintf( szScript, _countof( szScript ), "ev( \"auto_user\", \"%s\" )", pAutoPlayer->GetName() );

			LUA()->RunString( szScript );
		}
		else
		{
			PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		}
	}
	else
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832" );
	}
}

void onCheatKick( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() == 2 )
	{
		StructPlayer::iterator it = StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) );
		StructPlayer* pKickPlayer = *it;
		if( pKickPlayer != NULL )
		{
			char szScript[1024] = { 0, };
			s_sprintf( szScript, _countof( szScript ), "kick( \"%s\" )", pKickPlayer->GetName() );
			LUA()->RunString( szScript );
		}
		else
		{
			PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		}
	}
	else
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832" );
	}
}

unsigned int GetStatusCode( StructCreature* pCreature, const struct StructPlayer* pClient )
{
	unsigned status = 0;
	if( pCreature->IsBattleMode() )								status |= TS_ENTER::CreatureInfo::FLAG_BATTLE_MODE;
	if( pCreature->IsInvisible() )								status |= TS_ENTER::CreatureInfo::FLAG_INVISIBLE;

	if( pCreature->IsPlayer() )
	{
		StructPlayer * pPlayer = static_cast< StructPlayer * >( pCreature );

		if( pPlayer->IsSitDown() )														status |= TS_ENTER::PlayerInfo::FLAG_SITDOWN;
		if( pPlayer->GetBoothStatus() == StructPlayer::BUY_BOOTH )						status |= TS_ENTER::PlayerInfo::FLAG_BUY_BOOTH;
		if( pPlayer->GetBoothStatus() == StructPlayer::SELL_BOOTH )						status |= TS_ENTER::PlayerInfo::FLAG_SELL_BOOTH;
		if( pPlayer->IsPKOn() )															status |= TS_ENTER::PlayerInfo::FLAG_PK_ON;
		if( pPlayer->IsBloodyCharacter() )												status |= TS_ENTER::PlayerInfo::FLAG_BLOODY;
		if( pPlayer->IsDemoniacCharacter() )											status |= TS_ENTER::PlayerInfo::FLAG_DEMONIAC;
		if( pPlayer->GetPermission() >= GameRule::PERMISSION_FOR_GM )					status |= TS_ENTER::PlayerInfo::FLAG_GM;
		if( pPlayer->IsDungeonOriginalOwner() )											status |= TS_ENTER::PlayerInfo::FLAG_DUNGEON_ORIGINAL_OWNER;
		if( pPlayer->IsDungeonOriginalSieger() )										status |= TS_ENTER::PlayerInfo::FLAG_DUNGEON_ORIGINAL_SIEGER;
		if( pPlayer->IsWalking() )														status |= TS_ENTER::PlayerInfo::FLAG_WALKING;
		if( pPlayer->IsInStartedCompete( true ) )										status |= TS_ENTER::PlayerInfo::FLAG_COMPETING;
		// pPlayer->IsInBattleArena 함수를 사용하면 WarpEnd 함수 내에서 ChangeLocation을 호출하기 전에 TS_ENTER 패킷 방송이 이루어지기 때문에
		// 정확한 검사가 이루어지지 않는 객체가 발생하게 됨(워프와 동시에 TS_ENTER 패킷을 받는 유저들이 있기 때문)
		// 따라서 아래와 같이 GetBattleArenaInstanceNo() 가 0 이 아닌 경우에 팀 번호로 체크를 하게 되면 경기장에 입장하기 전부터
		// 서로 플래그가 세팅된 상대로 status를 받게 되지만, 클라이언트에서는 이 플래그에 의한 서로 공격 가능 체크를 아레나 영역 안에서만 하고,
		// 서버에서는 피아 판정(StructPlayer::IsEnemy)에서 팀 번호가 아닌 아레나 내부일 경우에만 BattleArenaManager::IsOpponent를 사용하므로
		// 문제없이 사용이 가능하게 될 것으로 예상됨
		if( pPlayer->GetBattleArenaInstanceNo() )
		{
			int nTeamNo = BattleArenaManager::Instance().GetTeamNo( pPlayer );
			if( nTeamNo == 0 )
				status |= TS_ENTER::PlayerInfo::FLAG_BATTLE_ARENA_TEAM_0;
			else if( nTeamNo == 1 )
				status |= TS_ENTER::PlayerInfo::FLAG_BATTLE_ARENA_TEAM_1;
		}
	}
	else if( pCreature->IsNPC() )
	{
		StructNPC * pNPC = static_cast< StructNPC* >( pCreature );

		if( pNPC->HasFinishableQuest( pClient ) )
		{
			status |= TS_ENTER::NPCInfo::FLAG_HAS_FINISHABLE_QUEST;
		}
		else if( pNPC->HasStartableQuest( pClient ) )
		{
			status |= TS_ENTER::NPCInfo::FLAG_HAS_STARTABLE_QUEST;
		}
		else if( pNPC->HasInProgressQuest( pClient ) )
		{
			status |= TS_ENTER::NPCInfo::FLAG_HAS_IN_PROGRESS_QUEST;
		}
	}
	else if( pCreature->IsMonster() )
	{
		StructMonster * pMonster = static_cast< StructMonster * >( pCreature );

		if( pMonster->IsDead() )									status |= TS_ENTER::MonsterInfo::FLAG_DEAD;
		if( pMonster->IsOriginalDungeonOwnerGuardian() )			status |= TS_ENTER::MonsterInfo::FLAG_DUNGEON_ORIGINAL_OWNER;
		if( pMonster->IsOriginalDungeonSiegerGuardian() )			status |= TS_ENTER::MonsterInfo::FLAG_DUNGEON_ORIGINAL_SIEGER;
	}
	else if( pCreature->IsPet() )
	{
		StructPet * pPet = static_cast< StructPet * >( pCreature );

		if( pPet->IsUsingSkill() && pPet->GetCastSkill()->GetSkillId() == StructSkill::SKILL_SHOVELING )
		{
			switch( pPet->GetShovelingStatus() )
			{
			case StructPet::SHOVELING_STATUS_SEARCH:
				status |= TS_ENTER::PlayerInfo::FLAG_SHOVELING_SEARCH;
				break;
			case StructPet::SHOVELING_STATUS_APPROACH:
				status |= TS_ENTER::PlayerInfo::FLAG_SHOVELING_APPROACH;
				break;
			case StructPet::SHOVELING_STATUS_DIG:
				status |= TS_ENTER::PlayerInfo::FLAG_SHOVELING_DIG;
				break;
			}
		}
	}

	return status;
}

void onBattleMode( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() < 2 )
		return;

	char * strstopper;

	AR_HANDLE handle = strtoul( vToken[1].c_str(), &strstopper, 10 );

	StructCreature::iterator it = StructCreature::get( handle );

	StructCreature * pCreature = (*it);

	if( !pCreature )
		return;

	if( pCreature->IsPlayer() && pPlayer == static_cast< StructPlayer * >( pCreature ) )
	{
		pPlayer->SetBattleModeOn();
	}
	else if( pCreature->IsSummon() )
	{
		StructSummon * pSummon = pPlayer->GetSummon( pCreature->GetHandle() );

		if( pSummon )
		{
			pSummon->SetBattleModeOn();
		}
	}
	else
	{
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pCreature ) );

	BroadcastStatusMessage( pCreature );
}

void onNormalMode( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() < 2 )
		return;

	char * strstopper;
	AR_HANDLE handle = strtoul( vToken[1].c_str(), &strstopper, 10 );

	StructCreature::iterator it = StructCreature::get( handle );

	StructCreature * pCreature = (*it);

	if( !pCreature )
		return;

	if( pCreature->IsPlayer() && pPlayer == static_cast< StructPlayer * >( pCreature ) )
	{
		// pPlayer->Save();
		pPlayer->SetBattleModeOff();
	}
	else if( pCreature->IsSummon() )
	{
		StructSummon * pSummon = pPlayer->GetSummon( pCreature->GetHandle() );

		if( pSummon )
		{
			pSummon->SetBattleModeOff();
		}
	}
	else
	{
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pCreature ) );

	BroadcastStatusMessage( pCreature );
}

void onCheatSitdown( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

	if( pPlayer->IsMoving(GetArTime()) || !pPlayer->IsActable() || !pPlayer->IsSitdownable() ) return;

	if( pPlayer->IsFormChanged() )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@440" );
		return;
	}

	if( pPlayer->IsAttacking() )
	{
		pPlayer->CancelAttack();
	}

	pPlayer->SitDown();

	BroadcastStatusMessage( pPlayer );
}







void onBuffFayz(StructPlayer* pPlayer, std::vector< std::string >& vToken)
{
	LUA()->RunString("fayz_buff()");
}

void about_server(StructPlayer* pPlayer, std::vector< std::string >& vToken)
{
	LUA()->RunString("dlg_general(' <br><br><br><#EF5343> Rappelz <#4DEF43>[Test Server]<br><br><br><#FAE21C> This Server is setup with Fayz SourceCode <br><br> <#FEFEFE> About The Server : <br><br> <#43EFDA> EXP : x1 <br><br> Drop : x3 <br><br> <#43EF99> There is Nothing on my Test Server <#EF4343> For the Moment<br><br><br>')");
}





void onCheatRide( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() < 1 )
		return;

	if( !pPlayer->IsMountable( false ) )
		return;

	char * stopchar;
	AR_HANDLE handle = strtoul( vToken.back().c_str(), &stopchar, 10 );

	pPlayer->MountSummon( handle );
}

void onCheatUnRide( StructPlayer* pPlayer )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

	pPlayer->UnMount();
}

void onCheatStandUp( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

	if( !pPlayer->GetBoothStatus() == StructPlayer::IS_NOT_BOOTH ) return;

	pPlayer->StandUp();

	BroadcastStatusMessage( pPlayer );
}

void onChangeGuildName( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 )
		return;

	// 길드가 아니면 리턴
	if( !pPlayer->IsInGuild() )
		return;

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsLeader( pPlayer->GetGuildID(), pPlayer->GetPlayerUID() ) )
		return;

	const char * szNewGuildName = vToken.back().c_str();

	if( (int)strlen( szNewGuildName ) > 16 )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@128" );
		return;
	}

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, szNewGuildName, (int)strlen( szNewGuildName ) + 1, 1, 16 ) ||
		GameContent::IsBannedWord( code_page, szNewGuildName ) )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@128" );
		return;
	}

	GuildManager::GetInstance().ChangeGuildName( pPlayer->GetGuildID(), vToken.back().c_str(), pPlayer );
}

void onChangeAllianceName( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	// 길드가 아니면 리턴
	int guild_id = pPlayer->GetGuildID();
	if( !guild_id )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@863" );
		return;
	}

	// 연합에 소속되어 있지 않은 길드면 리턴
	int alliance_id = GuildManager::GetInstance().GetAllianceID( guild_id );
	if( !alliance_id )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@863" );
		return;
	}

	// 연합장 길드 소속이 아니면 리턴
	if( GuildManager::GetInstance().GetAllianceLeaderGuildID( alliance_id ) != guild_id )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@871" );
		return;
	}

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsLeader( pPlayer->GetGuildID(), pPlayer->GetPlayerUID() ) )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@637" );
		return;
	}

	const char * szNewAllianceName = vToken.back().c_str();
	int nNewAllianceNameLength = static_cast< int >( vToken.back().length() );

	if( nNewAllianceNameLength > 16 )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@128" );
		return;
	}

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, szNewAllianceName, nNewAllianceNameLength + 1, 1, 16 ) ||
		GameContent::IsBannedWord( code_page, szNewAllianceName ) )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPlayer, "@128" );
		return;
	}

	GuildManager::GetInstance().ChangeAllianceName( alliance_id, szNewAllianceName, pPlayer );
}

void onAddFriend( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	if( vToken.back() == pPlayer->GetName() )
	{
		SendChatMessage( false, CHAT_FRIEND, "@FRIEND", pPlayer, "@482" );		// 자신을 등록할 수 없습니다.
		return;
	}

	// 미국 요청: 예외 단어가 포함된 캐릭터의 이름은 유효하지 않은 이름이어도 친구 등록 가능하도록 함
	// EX) 명예 GM / QA -> [VGM] / [VQA] 2개의 머릿말이 붙은 캐릭터 이름은 유효하지는 않지만 친구 등록 필요
	std::string szAllowedWords = ENV().GetString( "game.allowed_names_as_friend" );
	bool bIsAllowedName = false;

	if( !szAllowedWords.empty() )
	{
		std::vector< std::string > vAllowedNames;
		XStringUtil::Split( szAllowedWords.c_str(), vAllowedNames, ";", false );

		std::vector< std::string >::iterator it;
		for( it = vAllowedNames.begin(); it != vAllowedNames.end(); ++it )
		{
			if( nsl::wildcmp( (*it).c_str(), vToken.back().c_str() ) )
			{
				bIsAllowedName = true;
				break;
			}
		}
	}

	if( !bIsAllowedName && !GameRule::IsValidName( ENV().GetInt( "CodePage", CP_ACP ), vToken.back().c_str(), static_cast< int >( vToken.back().length() ) + 1, 4, 18 ) )
	{
		SendChatMessage( false, CHAT_FRIEND, "@FRIEND", pPlayer, "@481" );		// 존재하지 않는 캐릭터입니다.
		return;
	}

	if( pPlayer->GetFriendCount() >= 25 )
	{
		SendChatMessage( false, CHAT_FRIEND, "@FRIEND", pPlayer, "@478" );		// 더 이상 등록이 불가능합니다.
		return;
	}

	pPlayer->AddFriend( vToken.back().c_str() );
}

void onDelFriend( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	if( pPlayer->IsFriend( vToken.back().c_str() ) )
	{
		pPlayer->DelFriend( vToken.back().c_str() );
	}
	else
	{
		PrintfChatMessage( false, CHAT_FRIEND, "@FRIEND", pPlayer, "@471\v#@friend_name@#\v%s", vToken.back().c_str() );		// XXXX 님은 친구 목록에 존재하지 않습니다.
	}
}

void onAddDenial( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	if( vToken.back() == pPlayer->GetName() )
	{
		SendChatMessage( false, CHAT_FRIEND, "@FRIEND", pPlayer, "@482" );		// 자신을 등록할 수 없습니다.
		return;
	}

	if( !GameRule::IsValidName( ENV().GetInt( "CodePage", CP_ACP ), vToken.back().c_str(), static_cast< int >( vToken.back().length() ) + 1, 4, 18 ) )
	{
		SendChatMessage( false, CHAT_FRIEND, "@FRIEND", pPlayer, "@481" );		// 존재하지 않는 캐릭터입니다.
		return;
	}

	if( pPlayer->GetDenialCount() >= 25 )
	{
		SendChatMessage( false, CHAT_FRIEND, "@FRIEND", pPlayer, "@478" );		// 더 이상 등록이 불가능합니다.
		return;
	}

	pPlayer->AddDenial( vToken.back().c_str() );
}

void onDelDenial( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	if( pPlayer->IsDenial( vToken.back().c_str() ) )
	{
		pPlayer->DelDenial( vToken.back().c_str() );
	}
	else
	{
		PrintfChatMessage( false, CHAT_FRIEND, "@FRIEND", pPlayer, "@475\v#@denial_name@#\v%s", vToken.back().c_str() );		// XXXX 님은 차단 목록에 존재하지 않습니다.
	}
}

void onChangeName( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

	pPlayer->ChangeName( vToken.back().c_str(), true, false, NULL );
}


void	onCheatWarp( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() == 2 )
	{
		StructPlayer::iterator it = StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) );
		StructPlayer* pToPlayer = *it;
		if( pToPlayer != NULL )
		{
			char szScript[1024] = { 0, };
			s_sprintf( szScript, _countof( szScript ), "warp( %f, %f, %d, \"%s\" )",
				pToPlayer->GetX(), pToPlayer->GetY(), pToPlayer->GetLayer(), pPlayer->GetName() );

			LUA()->RunString( szScript );
		}
		else
		{
			PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		}
	}
	else if( vToken.size() == 3 )
	{
		AR_UNIT x = atoi( vToken[1].c_str() );
		AR_UNIT y = atoi( vToken[2].c_str() );

		char szScript[1024] = { 0, };
		s_sprintf( szScript, _countof( szScript ), "warp( %f, %f, \"%s\" )",
			x, y, pPlayer->GetName() );

		LUA()->RunString( szScript );
	}
	else if( vToken.size() == 4 )
	{
		AR_UNIT x = atoi( vToken[1].c_str() );
		AR_UNIT y = atoi( vToken[2].c_str() );
		StructPlayer::iterator it = StructPlayer::get( StructPlayer::FindPlayer( vToken[3].c_str() ) );
		StructPlayer* pWarpPlayer = *it;
		if( pWarpPlayer != NULL )
		{
			char szScript[1024] = { 0, };
			s_sprintf( szScript, _countof( szScript ), "warp( %f, %f, \"%s\" )",
				x, y, pWarpPlayer->GetName() );

			LUA()->RunString( szScript );
		}
		else
		{
			PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		}
	}
	else
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832" );
	}
}

void	onCheatForceWarp( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() == 2 )
	{
		StructPlayer::iterator it = StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) );
		StructPlayer* pToPlayer = *it;
		if( pToPlayer != NULL )
		{
			char szScript[1024] = { 0, };
			s_sprintf( szScript, _countof( szScript ), "force_warp( %f, %f, %d, \"%s\" )",
				pToPlayer->GetX(), pToPlayer->GetY(), pToPlayer->GetLayer(), pPlayer->GetName() );

			LUA()->RunString( szScript );
		}
		else
		{
			PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		}
	}
	else if( vToken.size() == 3 )
	{
		AR_UNIT x = atoi( vToken[1].c_str() );
		AR_UNIT y = atoi( vToken[2].c_str() );

		char szScript[1024] = { 0, };
		s_sprintf( szScript, _countof( szScript ), "force_warp( %f, %f, \"%s\" )",
			x, y, pPlayer->GetName() );

		LUA()->RunString( szScript );
	}
	else if( vToken.size() == 4 )
	{
		AR_UNIT x = atoi( vToken[1].c_str() );
		AR_UNIT y = atoi( vToken[2].c_str() );
		StructPlayer::iterator it = StructPlayer::get( StructPlayer::FindPlayer( vToken[3].c_str() ) );
		StructPlayer* pWarpPlayer = *it;
		if( pWarpPlayer != NULL )
		{
			char szScript[1024] = { 0, };
			s_sprintf( szScript, _countof( szScript ), "force_warp( %f, %f, \"%s\" )",
				x, y, pWarpPlayer->GetName() );

			LUA()->RunString( szScript );
		}
		else
		{
			PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@280" );
		}
	}
	else
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832" );
	}
}

void onCheatInvisible( StructPlayer * pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() < 2 )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

		std::string strChat;
		XStringUtil::Format( strChat, "Invisible : %s", pPlayer->IsInvisible() ? "true" : "false" );
		SendChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, strChat.c_str(), static_cast< unsigned int >( strChat.size() ) );

		return;
	}

	int mode = atoi( vToken.back().c_str() );
	if( mode == 1 || mode == 2 )
	{
		char szScript[1024] = { 0, };
		s_sprintf( szScript, _countof( szScript ), "set_invisible( %d, \"%s\" )", mode, pPlayer->GetName() );

		LUA()->RunString( szScript );
	}
}

void onCheatNotice( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() < 2 )
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832" );
		return;
	}

	std::string strNotice;
	for( size_t i = 1; i < vToken.size(); ++i )
	{
		strNotice += vToken[i];
		if( i < (vToken.size()-1) )
		{
			strNotice += " ";
		}
	}

	char szScript[1024] = { 0, };
	s_sprintf( szScript, _countof( szScript ), "notice( \"%s\" )", strNotice.c_str() );

	LUA()->RunString( szScript );
}

void onCheatRegenerate( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	int nMonID = 0;
	int nCount = 1;
	if( vToken.size() >= 2 )
	{
		nMonID = atoi( vToken[1].c_str() );
	}
	if( vToken.size() >= 3 )
	{
		nCount = atoi( vToken[2].c_str() );
	}

	if( nMonID > 0 && nCount >= 1 )
	{
		char szScript[1024] = { 0, };
		s_sprintf( szScript, _countof( szScript ), "add_npc( %f, %f, %d, %d, \"%s\" )",
			pPlayer->GetX(), pPlayer->GetY(), nMonID, nCount, pPlayer->GetName() );

		LUA()->RunString( szScript );
	}
	else
	{
		PrintfChatMessage( false, CHAT_DEBUG, "@SCRIPT", pPlayer, "@832" );
	}
}

void onCheatLevel( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	int new_level = atoi( vToken.back().c_str() );
	if( new_level < 1 || new_level > GameRule::MAX_LEVEL ) return;

	char szScript[1024] = { 0, };
	s_sprintf( szScript, _countof( szScript ), "set_level( %d, \"%s\" )",
		new_level, pPlayer->GetName() );

	LUA()->RunString( szScript );
}

void onCheatHeal( StructPlayer* pPlayer, std::vector< std::string > & vToken )
{
	if( vToken.size() != 2 ) return;

	char szScript[1024] = { 0, };
	s_sprintf( szScript, _countof( szScript ), "heal( %d, \"%s\" )",
		atoi( vToken.back().c_str() ), pPlayer->GetName() );

	LUA()->RunString( szScript );
}


void onCheatScript( const char *pScript )
{
	int nQuote = 0, nBigQuote = 0, nBracket = 0;

	for( const char *p = pScript; *p; ++p )
	{
		if( *p == '(' ) nBracket++;
		if( *p == ')' ) nBracket--;
		if( *p == '\'' ) nQuote++;
		if( *p == '\"' ) nBigQuote++;
	}

	if( nBracket == 0 && nQuote%2 == 0 && nBigQuote%2 == 0 ) LUA()->RunString( pScript );
}

void onPartyAssist( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;	

	// 이미 파티에 가입되어 있으면..
	if( !pClient->IsInParty() )
		return;

	char * stopchar;
	AR_HANDLE handle = strtoul( vToken[1].c_str(), &stopchar, 10 );

	if( !handle )
		return;

	StructPlayer::iterator it = StructPlayer::get( handle );

	if( !(*it) )
		return;

	if( (*it)->GetPartyID() != pClient->GetPartyID() )
		return;

	if( !(*it)->IsPlayer() )
		return;

	PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ASSIST|%u|", (*it)->GetTarget() );
}


void onAttackTeamJoin( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;	

	// 헌터홀릭 던전 및 대기실 안에 있으면 공대 가입 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_HUNTAHOLIC" );
		return;
	}

	// 인스턴스 던전 안에 있으면 공대 가입 불가
	if( pClient->IsInInstanceDungeon() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_INSTANCE_DUNGEON" );
		return;
	}

	// 데스매치 맵에 있으면 공대 가입 불가
	if( pClient->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_DEATHMATCH" );
		return;
	}

	// 아레나 대기열 또는 경기에 참여 중이면 가입 불가
	if( pClient->GetBattleArenaID() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_BATTLE_ARENA" );
		return;
	}

	int nPartyID = atoi( vToken[1].c_str() );

	std::string strPartyName = PartyManager::GetInstance().GetPartyName( nPartyID );

	// 이미 파티에 가입되어 있으면..
	if( pClient->IsInParty() )
	{	
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_ALREADY_PARTY_MEMBER" );
		return;
	}

	if( strPartyName.empty() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_INVALID_PARTY" );
		return;
	}

	if( !PartyManager::GetInstance().IsAttackTeamParty( nPartyID ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_INVALID_PARTY" );
		return;
	}

	// 암호가 없거나 인자 수가 부족하면 거절한 것
	if( vToken.size() <= 2 )
		return;

	// 암호가 다르면
	if( atoi( vToken[2].c_str() ) != PartyManager::GetInstance().GetLinkedPartyPassword( nPartyID ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "HAS_NO_AUTHORITY" );
		return;
	}

	if( !PartyManager::GetInstance().IsJoinableLinkedParty( nPartyID ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_MAX" );
		return;
	}

	char buf[1024];

	PartyManager::_PARTY_TYPE partyType = PartyManager::GetInstance().GetPartyType( nPartyID );

	int nAttackTeamPartyID = 0;
	for( int i = 0; i < 100; ++i )
	{
		s_sprintf( buf, _countof( buf ), "%s_%02d", strPartyName.c_str(), i );
		// 파티 만들고
		nAttackTeamPartyID = PartyManager::GetInstance().MakeParty( buf, pClient->GetPlayerUID(), partyType );
		if( nAttackTeamPartyID > 0 )
		{
			break;
		}
	}

	if( nAttackTeamPartyID <= 0 )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_MAX" );
		return;
	}

	// 리더를 참여시킨다.
	if( !PartyManager::GetInstance().JoinParty( nAttackTeamPartyID, pClient ) )
	{
		PartyManager::GetInstance().DestroyParty( nAttackTeamPartyID );

		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "CANT_JOIN" );
		return;
	}

	// 초대한 길드 ID를 얻어야 함
	int nInvitorGuildID = PartyManager::GetInstance().GetAttackTeamGuildID( nPartyID );

	int nNewPartyID = pClient->GetPartyID();

	if( !PartyManager::GetInstance().JoinAttackTeam( nInvitorGuildID, nNewPartyID ) )
	{
		PartyManager::GetInstance().DestroyParty( nNewPartyID );

		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ALREADY_EXIST" );
		return;
	}

	PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nNewPartyID, "CREATE|%s|%s|%d|", buf, pClient->GetName(), partyType );

	SendPartyInfo( pClient );
	SendLinkedPartyInfo( pClient );

	BroadcastPartyMemberInfo( nNewPartyID, pClient );
	BroadcastLinkedPartyMemberInfo( nNewPartyID, pClient );

	// 소환수 정보는 파티 타입에 관계없이 동일한 규칙에 의해 방송되므로 별도 처리한다.
	SendSummonInfo( pClient );
	BroadcastSummonInfo( nNewPartyID, pClient );

	LOG::Log11N4S( LM_PARTY_CREATE, pClient->GetAccountID(), pClient->GetSID(), nNewPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 2, partyType, 0, 0, 0,
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, buf, LOG::STR_NTS, "", 0 );

	// 파티장이 비정상 종료 되도 공대는 유지되도록 파티장을 저장함
	pClient->Save( true );
}

void onAttackTeamInvite( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 파티가 아니면 리턴
	if( !pClient->IsInParty() ) return;

	int nPartyID = pClient->GetPartyID();
	int nGuildID = pClient->GetGuildID();

	std::string partyName( PartyManager::GetInstance().GetPartyName( nPartyID ) );

	// 리더가 아니면 리턴
	if( !PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) ) return;

	if( !PartyManager::GetInstance().IsAttackTeamParty( nPartyID ) ) return;

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) ) );
	StructPlayer *pTarget = *pit;

	// 대상이 없으면 리턴
	if( !pTarget ) return;

	// 대상이 헌터홀릭 던전 및 대기실 안에 있으면 초대 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pTarget->GetPos() ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_TARGET_IN_HUNTAHOLIC" );
		return;
	}

	// 대상이 인스턴스 던전 안에 있으면 초대 불가
	if( pTarget->IsInInstanceDungeon() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_TARGET_IN_INSTANCE_DUNGEON" );
		return;
	}

	// 데스매치 맵에 있으면 초대 불가
	if( pClient->IsInDeathmatch() )
		return;
	if( pTarget->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_DEATHMATCH" );
		return;
	}

	// 대상이 아레나 대기열 또는 경기에 참여 중이면 초대 불가
	if( pTarget->GetBattleArenaID() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_TARGET_IN_BATTLE_ARENA" );
		return;
	}

	// 파티에 가입 되어 있으면
	if( pTarget->IsInParty() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_TARGET_ANOTHER" );
		return;
	}

	if( !nGuildID ||
		( nGuildID != pTarget->GetGuildID() &&
		( !GuildManager::GetInstance().GetAllianceID( nGuildID ) ||
		GuildManager::GetInstance().GetAllianceID( nGuildID ) != GuildManager::GetInstance().GetAllianceID( pTarget->GetGuildID() ) )
		) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_TARGET_ANOTHER" );
		return;
	}

	// 공대 파티가 도전 중인 던전의 입장 레벨 제한 체크
	if( DungeonManager::Instance().IsRestrictedToEnter( GuildManager::GetInstance().GetRaidDungeonID( nGuildID ), pTarget ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_NOT_PROPER_LEVEL_FOR_DUNGEON" );
		return;
	}

	if( !PartyManager::GetInstance().IsJoinableLinkedParty( nPartyID ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_MAX" );
		return;
	}

	LOG::Log11N4S( LM_PARTY_INVITE, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(),
		pTarget->GetAccountID(), pTarget->GetSID(), pTarget->GetX(), pTarget->GetY(), pTarget->GetLayer(),
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pTarget->GetAccountName(), LOG::STR_NTS, pTarget->GetName(), LOG::STR_NTS );

	PrintfChatMessage( true, CHAT_RAID_SYSTEM, "@RAID", pTarget, "RAID_GUILD_INVITE|%s|%s|%d|%d|", pClient->GetName(), partyName.c_str(), nPartyID, PartyManager::GetInstance().GetLinkedPartyPassword( nPartyID ) );
}

void onAttackTeamCreate( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;	

	const char *pPartyName = vToken[1].c_str();
	if( !pPartyName ) return;
	size_t partyLen = strlen( pPartyName );
	if( partyLen > 30 ) return;

	int nPartyID = pClient->GetPartyID();
	if( nPartyID )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_ALREADY_PARTY_MEMBER" );
		return;
	}

	int nGuildID = pClient->GetGuildID();

	if( !nGuildID )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_NOT_GUILD_LEADER" );
		return;
	}

	// 헌터홀릭 던전 및 대기실 안에 있으면 파티 생성 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_HUNTAHOLIC" );
		return;
	}

	// 인스턴스 던전 안에 있으면 파티 생성 불가
	if( InstanceDungeonManager::Instance().GetInstanceDungeonID( pClient->GetPos() ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_INSTANCE_DUNGEON" );
		return;
	}

	// 데스매치 맵에 있으면 파티 생성 불가
	if( pClient->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_DEATHMATCH" );
		return;
	}

	// 아레나 대기열 또는 경기에 참여 중이면 파티 생성 불가
	if( pClient->GetBattleArenaID() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_BATTLE_ARENA" );
		return;
	}

	//if( nAllianceID && !GuildManager::GetInstance().IsAllianceLeader( nAllianceID, nGuildID ) )
	//{
	//	SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_NOT_GUILD_LEADER" );
	//	return;
	//}

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );

	// 연합 리더 길드 ID로 nGuildID를 변경(전투 대장은 연합 마스터 길드 정보에 세팅됨)
	if( nAllianceID )
		nGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID );

	int raid_dungeon_id = GuildManager::GetInstance().GetRaidDungeonID( nGuildID );

	if( !raid_dungeon_id )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_NOT_RAID_GUILD" );
		return;
	}

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_ATTACK_TEAM_CREATE ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_NOT_GUILD_LEADER" );
		return;
	}

	// 레이드 기간 중이어도 해당 날짜 중에 레이드 완료 기록이 있으면 공대파티 생성 불가
	// (레이드 한 번 끝났으면 그 날은 시즈공대 못만들어야 함)
	time_t raid_end_time = DungeonManager::Instance().GetLastRaidEndTime( raid_dungeon_id, nGuildID );
	if( DungeonManager::Instance().IsDungeonRaidTime( raid_dungeon_id ) &&
		raid_end_time && raid_end_time >= GetDayBeginTime() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_ALREADY_COMPLETE_RAID" );
		return;
	}

	// 이미 공대가 결성되어 있으면 불가
	if( PartyManager::GetInstance().IsExistAttackTeam( nGuildID ) )
	{	
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_ALREADY_PARTY_MEMBER" );
		return;
	}

	// 나쁜 파티 이름
	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, pPartyName, static_cast< int >( partyLen ) + 1, 1, 30 ) ||
		GameContent::IsBannedWord( code_page, pPartyName ) )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "INVALID_PARTY_NAME" );
		return;
	}

	int nMaxGuildParty = 0;
	bool bRaidParty = false;

	if( DungeonManager::Instance().IsDungeonRaidTime( raid_dungeon_id ) )
	{
		if( nGuildID == DungeonManager::Instance().GetOwnGuildID( raid_dungeon_id ) )
		{
			SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_NOT_SIEGE_TIME" );
			return;
		}

		nMaxGuildParty = DungeonManager::Instance().GetMaxRaidParty( raid_dungeon_id );
		bRaidParty = true;
	}
	else
	{
		if( !DungeonManager::Instance().IsDungeonSiegeAttacteamMakingPeriod( raid_dungeon_id ) )
		{
			SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_NOT_SIEGE_TIME" );
			return;
		}

		if( DungeonManager::Instance().GetOwnGuildID( raid_dungeon_id ) != nGuildID &&
			DungeonManager::Instance().GetRaidGuildID( raid_dungeon_id ) != nGuildID )
		{
			assert( 0 );
			// 시즈 시간인데 공격자 길드도, 주인 길드도 아닌 길드가 도전 상태임
			SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_NOT_SIEGE_TIME" );
			return;
		}

		nMaxGuildParty = DungeonManager::Instance().GetMaxGuildParty( raid_dungeon_id );
	}

	PartyManager::_PARTY_TYPE partyType = bRaidParty ? PartyManager::TYPE_RAID_ATTACKTEAM : PartyManager::TYPE_SIEGE_ATTACKTEAM;

	// 파티 만들고
	int nAttackTeamPartyID = PartyManager::GetInstance().MakeParty( pPartyName, pClient->GetPlayerUID(), partyType );
	if( nAttackTeamPartyID <= 0 )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ALREADY_EXIST" );
		return;
	}

	// 리더를 참여시킨다.
	if( !PartyManager::GetInstance().JoinParty( nAttackTeamPartyID, pClient ) )
	{
		PartyManager::GetInstance().DestroyParty( nAttackTeamPartyID );

		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "CANT_JOIN" );
		return;
	}

	if( !PartyManager::GetInstance().MakeAttackTeam( nAttackTeamPartyID, nGuildID, nMaxGuildParty, bRaidParty ) )
	{
		PartyManager::GetInstance().DestroyParty( nAttackTeamPartyID );

		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ALREADY_EXIST" );
		return;
	}

	PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient,  "CREATE|%s|%s|%d|", pPartyName, pClient->GetName(), partyType );
	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "@645\v#@guild_name@#\v%s\v#@user_name@#\v%s", GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(), pClient->GetName() );

	SendPartyInfo( pClient );

	SendLinkedPartyInfo( pClient );

	// 소환수 정보는 파티 타입에 관계없이 동일한 규칙에 의해 방송되므로 별도 처리한다.
	SendSummonInfo( pClient );
	BroadcastSummonInfo( nPartyID, pClient );

	LOG::Log11N4S( LM_PARTY_CREATE, pClient->GetAccountID(), pClient->GetSID(), pClient->GetPartyID(), pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 1, partyType, 0, 0, 0,
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pPartyName, LOG::STR_NTS, "", 0 );

	// 파티장이 비정상 종료 되도 공대는 유지되도록 공대장을 저장함
	pClient->Save( true );
}

void onBattleArenaExerciseOpponentInvite( StructPlayer * pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 파티가 아니면 리턴
	if( !pClient->IsInParty() ) return;

	int nPartyID = pClient->GetPartyID();

	// 리더가 아니면 리턴
	if( !PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) ) return;

	if( PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM ) return;

	// 상대팀이 이미 있으면 초대 불가
	if( BattleArenaManager::Instance().HasOpponent( pClient ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_ALREADY_EXIST" );
		return;
	}

	// 레디를 한 이후에는 초대 불가
	if( BattleArenaManager::Instance().IsReadyExerciseTeam( nPartyID ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_READY" );
		return;
	}

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) ) );
	StructPlayer *pTarget = *pit;

	// 대상이 없으면 리턴
	if( !pTarget ) return;

	// 대상이 헌터홀릭 던전 및 대기실 안에 있으면 초대 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pTarget->GetPos() ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_IN_HUNTAHOLIC" );
		return;
	}

	// 대상이 인스턴스 던전 안에 있으면 초대 불가
	if( pTarget->IsInInstanceDungeon() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_IN_INSTANCE_DUNGEON" );
		return;
	}

	if( pTarget->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_IN_DEATHMATCH" );
		return;
	}

	// 파티에 가입 되어 있거나 배틀아레나 중이라면
	if( pTarget->IsInParty() || pTarget->GetBattleArenaID() != 0 )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_ANOTHER" );
		return;
	}

	LOG::Log11N4S( LM_PARTY_INVITE, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(),
		pTarget->GetAccountID(), pTarget->GetSID(), pTarget->GetX(), pTarget->GetY(), pTarget->GetLayer(),
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pTarget->GetAccountName(), LOG::STR_NTS, pTarget->GetName(), LOG::STR_NTS );

	PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pTarget, "ARENA_OPPONENT_INVITE|%s|%s|%d|%d|", pClient->GetName(), PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), nPartyID, PartyManager::GetInstance().GetLinkedPartyPassword( nPartyID ) );
}

void onBattleArenaExerciseOpponentJoin( StructPlayer * pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;	

	int nPartyID = atoi( vToken[1].c_str() );

	if( PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_INVALID_PARTY" );
		return;
	}

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( PartyManager::GetInstance().GetLeaderName( nPartyID ).c_str() ) ) );
	// 초대한 사람이 오프라인이면 없을 수도 있지만 이하 코드에서는 if( pInviter ) 떡칠로 대응은 되어 있으니 패스...
	StructPlayer * pInviter = (*pit);

	// 암호가 없거나 인자 수가 부족하면 거절한 것
	if( vToken.size() < 3 )
	{
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_INVITATION_DENIED|%s|", pClient->GetName() );
		return;
	}

	// 암호가 다르면
	if( atoi( vToken[2].c_str() ) != PartyManager::GetInstance().GetLinkedPartyPassword( nPartyID ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "HAS_NO_AUTHORITY" );
		return;
	}

	// 헌터홀릭 던전 및 대기실 안에 있으면 연습 경기 참여 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_HUNTAHOLIC" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_IN_HUNTAHOLIC|%s|", pClient->GetName() );
		return;
	}

	// 인스턴스 던전 안에 있으면 연습 경기 참여 불가
	if( pClient->IsInInstanceDungeon() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_INSTANCE_DUNGEON" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_IN_INSTANCE_DUNGEON|%s|", pClient->GetName() );
		return;
	}

	// 데스매치 맵에 있으면 연습 경기 참여 불가
	if( pClient->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_DEATHMATCH" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_IN_DEATHMATCH|%s|", pClient->GetName() );
		return;
	}

	// 이미 파티에 가입되어 있거나 배틀아레나 중이라면
	if( pClient->IsInParty() || pClient->GetBattleArenaID() != 0 )
	{	
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_ALREADY_PARTY_MEMBER" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_ANOTHER|%s|", pClient->GetName() );
		return;
	}

	// JoinExerciseGameOpponentLeader 안에서 팀의 레디 상태 체크를 가입 처리 타이밍에 정확하게 해주기 때문에
	// 여기서는 따로 IsReadyExerciseTeam 으로 체크하지 않음
	unsigned short nResult = BattleArenaManager::Instance().JoinExerciseGameOpponentLeader( pClient, nPartyID );

	if( nResult != RESULT_SUCCESS )
	{
		switch( nResult )
		{
		case RESULT_NOT_EXIST:
			SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_INVALID_PARTY" );
			if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_FAIL|%s|", pClient->GetName() );
			break;
		case RESULT_ALREADY_EXIST:
			SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_MAX" );
			if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_MAX|%s|", pClient->GetName() );
			break;
		case RESULT_NOT_ACTABLE:
			SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_NOT_ACTABLE" );
			if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_NOT_ACTABLE|%s|", pClient->GetName() );
			break;
		default:
			PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_UNKNOWN|%d|", nResult );
			if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_UNKNOWN|%s|%d|", pClient->GetName(), nResult );
			break;
		}
	}
}

void onBattleArenaExerciseMemberInvite( StructPlayer * pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 파티가 아니면 리턴
	if( !pClient->IsInParty() ) return;

	int nPartyID = pClient->GetPartyID();

	// 리더가 아니면 리턴
	if( !PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) ) return;

	if( PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM ) return;

	// 레디를 한 이후에는 초대 불가
	if( BattleArenaManager::Instance().IsReadyExerciseTeam( nPartyID ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_READY" );
		return;
	}

	// 인원 초과 체크
	if( !BattleArenaManager::Instance().IsMoreMemberInvitable( pClient ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_LIMIT_MAX" );
		return;
	}

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) ) );
	StructPlayer *pTarget = *pit;

	// 대상이 없으면 리턴
	if( !pTarget ) return;

	// 대상이 헌터홀릭 던전 및 대기실 안에 있으면 초대 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pTarget->GetPos() ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_IN_HUNTAHOLIC" );
		return;
	}

	// 대상이 인스턴스 던전 안에 있으면 초대 불가
	if( pTarget->IsInInstanceDungeon() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_IN_INSTANCE_DUNGEON" );
		return;
	}

	if( pTarget->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_IN_DEATHMATCH" );
		return;
	}

	// 파티에 가입 되어 있거나 배틀아레나 중이라면(에러가 모호하다)
	if( pTarget->IsInParty() || pTarget->GetBattleArenaID() != 0 )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_ANOTHER" );
		return;
	}

	LOG::Log11N4S( LM_PARTY_INVITE, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(),
		pTarget->GetAccountID(), pTarget->GetSID(), pTarget->GetX(), pTarget->GetY(), pTarget->GetLayer(),
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pTarget->GetAccountName(), LOG::STR_NTS, pTarget->GetName(), LOG::STR_NTS );

	PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pTarget, "ARENA_MEMBER_INVITE|%s|%s|%d|%d|", pClient->GetName(), PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), nPartyID, PartyManager::GetInstance().GetPartyPassword( nPartyID ) );
}

void onBattleArenaExerciseMemberJoin( StructPlayer * pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;	

	int nPartyID = atoi( vToken[1].c_str() );

	if( PartyManager::GetInstance().GetPartyType( nPartyID ) != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_INVALID_PARTY" );
		return;
	}

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( PartyManager::GetInstance().GetLeaderName( nPartyID ).c_str() ) ) );
	// 초대한 사람이 오프라인이면 없을 수도 있지만 이하 코드에서는 if( pInviter ) 떡칠로 대응은 되어 있으니 패스...
	StructPlayer * pInviter = (*pit);

	// 암호가 없거나 인자 수가 부족하면 거절한 것
	if( vToken.size() < 3 )
	{
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_INVITATION_DENIED|%s|", pClient->GetName() );
		return;
	}

	// 암호가 다르면
	if( atoi( vToken[2].c_str() ) != PartyManager::GetInstance().GetPartyPassword( nPartyID ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "HAS_NO_AUTHORITY" );
		return;
	}

	// 헌터홀릭 던전 및 대기실 안에 있으면 연습 경기 참여 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_HUNTAHOLIC" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_IN_HUNTAHOLIC|%s|", pClient->GetName() );
		return;
	}

	// 인스턴스 던전 안에 있으면 연습 경기 참여 불가
	if( pClient->IsInInstanceDungeon() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_INSTANCE_DUNGEON" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_IN_INSTANCE_DUNGEON|%s|", pClient->GetName() );
		return;
	}

	// 데스매치 맵에 있으면 연습 경기 참여 불가
	if( pClient->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_DEATHMATCH" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_IN_DEATHMATCH|%s|", pClient->GetName() );
		return;
	}

	// 이미 파티에 가입되어 있거나 배틀아레나 중이라면
	if( pClient->IsInParty() || pClient->GetBattleArenaID() != 0 )
	{	
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_ALREADY_PARTY_MEMBER" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_ANOTHER|%s|", pClient->GetName() );
		return;
	}

	// JoinExerciseGameMember 안에서 팀의 레디 상태 체크를 가입 처리 타이밍에 정확하게 해주기 때문에
	// 여기서는 따로 IsReadyExerciseTeam 으로 체크하지 않음
	unsigned short nResult = BattleArenaManager::Instance().JoinExerciseGameMember( pClient, nPartyID );

	switch( nResult )
	{
	case RESULT_SUCCESS:
		// 성공 시에는 따로 방송을 할 필요 없이 BattleArenaManager::JoinExerciseGameMember 안에서 필요한 방송들이 모두 처리됨
		LOG::Log11N4S( LM_PARTY_JOIN, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 0, 0, 0, 0, 0,
			pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, 
			PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );
		break;
	case RESULT_LIMIT_MAX:
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_LIMIT_MAX" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_LIMIT|%s|", pClient->GetName() );
		break;
	case RESULT_NOT_EXIST:
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_INVALID_PARTY" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_FAIL|%s|", pClient->GetName() );
		break;
	case RESULT_ALREADY_EXIST:
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_MAX" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_MAX|%s|", pClient->GetName() );
		break;
	case RESULT_NOT_ACTABLE:
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_NOT_ACTABLE" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_NOT_ACTABLE|%s|", pClient->GetName() );
		break;
	case RESULT_ACCESS_DENIED:
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_READY" );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_READY|%s|", pClient->GetName() );
		break;
	default:
		PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_UNKNOWN|%d|", nResult );
		if( pInviter ) PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pInviter, "ERROR_JOIN_TARGET_UNKNOWN|%s|%d|", pClient->GetName(), nResult );
		break;
	}
}

void onBattleArenaExerciseMemberKick( StructPlayer * pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 파티가 아니면 리턴
	if( !pClient->IsInParty() ) return;

	int nPartyID = pClient->GetPartyID();

	// 리더가 아니면 리턴
	if( !PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) ) return;

	const char *szTargetName = vToken[1].c_str();

	// 그런사람 없으면 리턴(가명 기능은 연습 경기에서는 사용되지 않기 때문에 캐릭터 이름으로 명령어 처리 가능)
	PlayerUID nPlayerUID = PartyManager::GetInstance().GetMemberUID( nPartyID, szTargetName );
	if( !nPlayerUID ) return;

	// 리더는 스스로를 추방 불가
	if( PartyManager::GetInstance().IsLeader( nPartyID, nPlayerUID ) ) return;

	// 배틀 아레나 연습 경기용 파티가 아니라면 추방 불가
	PartyManager::_PARTY_TYPE ePartyType = PartyManager::GetInstance().GetPartyType( nPartyID );
	if( ePartyType != PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		return;

	AR_HANDLE hTargetPlayer = StructPlayer::FindPlayer( szTargetName );
	StructPlayer::iterator it = StructPlayer::get( hTargetPlayer );
	StructPlayer *pTarget = (*it);

	unsigned short nResult = RESULT_SUCCESS;
	if( pTarget )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pTarget ) );

		nResult = BattleArenaManager::Instance().QuitGame( pTarget, true, false, ALT_EXERCISE_PARTY_KICK );
	}
	else
		nResult = BattleArenaManager::Instance().QuitGame( nPartyID, nPlayerUID, true, ALT_EXERCISE_PARTY_KICK );

	// nResult == RESULT_SUCCESS 라면 이미 파티 이탈 관련 패킷들이 방송되었을 거라서 그 외의 경우에만 방송 처리
	if( nResult != RESULT_SUCCESS )
	{
		//SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_NOT_EXIST" );
		return;
	}

	std::string partyName( PartyManager::GetInstance().GetPartyName( nPartyID ) );
	PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient->GetPartyID(), "KICK|%s|%s|", partyName.c_str(), szTargetName );

	LOG::Log11N4S( LM_PARTY_KICK, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(),
		pTarget ? pTarget->GetAccountID() : 0, pTarget ? pTarget->GetSID() : 0, pTarget ? pTarget->GetX() : 0, pTarget ? pTarget->GetY() : 0, pTarget ? pTarget->GetLayer() : 0,
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pTarget ? pTarget->GetAccountName() : "", pTarget ? LOG::STR_NTS : 0, szTargetName, LOG::STR_NTS );
}

void onPartyCreate( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;	

	XStringUtil::Trim( vToken[1] );
	const char *pPartyName = vToken[1].c_str();
	if( !pPartyName ) return;
	size_t partyLen = strlen( pPartyName );
	if( partyLen > 30 ) return;

	// 헌터홀릭 던전 및 대기실 안에 있으면 파티 생성 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_HUNTAHOLIC" );
		return;
	}

	// 인스턴스 던전 안에 있으면 파티 생성 불가
	if( pClient->IsInInstanceDungeon() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_INSTANCE_DUNGEON" );
		return;
	}

	// 데스매치 맵에 있으면 파티 생성 불가
	if( pClient->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_DEATHMATCH" );
		return;
	}

	// 아레나 대기열 또는 경기에 참여 중이면 파티 생성 불가
	if( pClient->GetBattleArenaID() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_BATTLE_ARENA" );
		return;
	}

	// 이미 파티에 가입되어 있으면..
	if( pClient->IsInParty() )
	{	
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_ALREADY_PARTY_MEMBER" );
		return;
	}

	// 나쁜 파티 이름
	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, pPartyName, static_cast< int >( partyLen ) + 1, 1, 30 ) ||
		GameContent::IsBannedWord( code_page, pPartyName ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "INVALID_PARTY_NAME" );
		return;
	}

	// 파티 만들고
	int nPartyID = PartyManager::GetInstance().MakeParty( pPartyName, pClient->GetPlayerUID(), PartyManager::TYPE_NORMAL_PARTY );
	if( nPartyID <= 0 )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ALREADY_EXIST" );
		return;
	}

	// 리더를 참여시킨다.
	if( !PartyManager::GetInstance().JoinParty( nPartyID, pClient ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "CANT_JOIN" );
		return;
	}

	PrintfChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient,  "CREATE|%s|%s|%d|", pPartyName, pClient->GetName(), PartyManager::TYPE_NORMAL_PARTY );

	SendPartyInfo( pClient );

	// 소환수 정보는 파티 타입에 관계없이 동일한 규칙에 의해 방송되므로 별도 처리한다.
	SendSummonInfo( pClient );

	LOG::Log11N4S( LM_PARTY_CREATE, pClient->GetAccountID(), pClient->GetSID(), pClient->GetPartyID(), pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 0, PartyManager::TYPE_NORMAL_PARTY, 0, 0, 0,
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pPartyName, LOG::STR_NTS, "", 0 );
}

void onPartyShareMode( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;
	int nFlag = 0;

	int nPartyID = pClient->GetPartyID();

	// 파티 리더가 아니면 리턴
	if( !PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) ) return;

	// 헌터홀릭 파티면 파티나 배틀 아레나 파티 설정 변경 불가
	if( PartyManager::GetInstance().GetPartyType( nPartyID ) == PartyManager::TYPE_HUNTAHOLIC_PARTY ||
		PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
		return;

	// 데스매치 맵에 있으면 파티 설정 변경 불가
	if( pClient->IsInDeathmatch() )
		return;

	if( vToken[1] == "monopoly" )
	{
		nFlag = 0;
		PartyManager::GetInstance().SetShareMode( nPartyID, PartyManager::ITEM_SHARE_MONOPOLY );
	}
	else if( vToken[1] == "random" )
	{
		nFlag = 1;
		PartyManager::GetInstance().SetShareMode( nPartyID, PartyManager::ITEM_SHARE_RANDOM );
	}
	else if( vToken[1] == "linear" )
	{
		nFlag = 2;
		PartyManager::GetInstance().SetShareMode( nPartyID, PartyManager::ITEM_SHARE_LINEAR );
	}

	PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "MODE|%d|", nFlag );
}

void onPartyDestroy( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	int nPartyID = pClient->GetPartyID();

	// 파티가 아니면 리턴
	if( !pClient->IsInParty() ) return;

	// 파티 리더가 아니면 리턴
	if( !PartyManager::GetInstance().IsLeader( pClient->GetPartyID(), pClient->GetPlayerUID() ) ) return;

	int nLeaderPartyID = PartyManager::GetInstance().GetLinkedPartyLeadPartyID( nPartyID );

	if( PartyManager::GetInstance().IsSomeoneInSiegeOrRaidDungeon( nPartyID ) || ( nLeaderPartyID == nPartyID && DungeonManager::Instance().IsRaidBegin( GuildManager::GetInstance().GetRaidDungeonID( pClient->GetGuildID() ), pClient->GetGuildID() ) ) )
		return;

	// 헌터홀릭 파티나 배틀 아레나 파티면 명령어에 의한 파티 해산 불가
	if( PartyManager::GetInstance().GetPartyType( nPartyID ) == PartyManager::TYPE_HUNTAHOLIC_PARTY ||
		PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
		return;

	// 인스턴스 던전 안에 있으면 파티 해산 불가
	if( pClient->IsInInstanceDungeon() )
		return;

	// 인스턴스 던전 안에 파티원이 남아 있으면 파티 해산 불가
	// 파장이 인던에 유저가 있을 때 파티에서 나가고 싶으면 아무한테나 파장 인계하고 탈퇴로 나가야 함
	struct MemberInInstanceDungeonChecker : public PartyManager::PartyFunctor
	{
		MemberInInstanceDungeonChecker()
			: bSomeoneInInstanceDungeon( false )
		{}

		virtual bool operator()( AR_HANDLE handle )
		{
			// 한 명이라도 인던 내부에 누가 있었던 게 확인됐다면 바로 패스
			if( bSomeoneInInstanceDungeon )
				return true;

			StructPlayer * pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( handle ) );
			if( !pPlayer )
				return true;

			bSomeoneInInstanceDungeon = pPlayer->IsInInstanceDungeon();
			return true;
		}

		bool bSomeoneInInstanceDungeon;
	} foChecker;
	PartyManager::GetInstance().DoEachMember( nPartyID, foChecker );

	if( foChecker.bSomeoneInInstanceDungeon )
		return;

	// 데스매치 맵에 있으면 파티 해산 불가
	if( pClient->IsInDeathmatch() )
		return;

	LOG::Log11N4S( LM_PARTY_DESTROY, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 0, 0, 0, 0, 0,
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

	BroadcastPartyDestroy( nPartyID );

	PartyManager::GetInstance().DestroyParty( nPartyID );
}

void onPartyInvite( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 파티가 아니면 리턴
	int nPartyID = pClient->GetPartyID();
	if( !nPartyID ) return;

	std::string partyName( PartyManager::GetInstance().GetPartyName( nPartyID ) );

	// 리더가 아니면 리턴
	if( !PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) ) return;

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) ) );
	StructPlayer *pTarget = *pit;

	// 대상이 없으면 리턴
	if( !pTarget ) return;

	// 헌터홀릭용 파티나 배틀 아레나용 파티면 명령어에 의한 초대 불가
	PartyManager::_PARTY_TYPE ePartyType = PartyManager::GetInstance().GetPartyType( nPartyID );
	if( ePartyType == PartyManager::TYPE_HUNTAHOLIC_PARTY ||
		PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
		return;

	// 대상이 헌터홀릭 던전 및 대기실 안에 있으면 초대 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pTarget->GetPos() ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_IN_HUNTAHOLIC" );
		return;
	}

	// 대상이 인스턴스 던전 안에 있으면 초대 불가,
	// 파티장이 인스턴스 던전 내부에서 초대가 가능
	if( pTarget->IsInInstanceDungeon() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_IN_INSTANCE_DUNGEON" );
		return;
	}

	// 파티장 또는 대상이 데스매치 맵에 있으면 초대 불가
	if( pClient->IsInDeathmatch() )
		return;
	if( pTarget->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_DEATHMATCH" );
		return;
	}

	// 대상이 아레나 대기열 또는 경기에 참여 중이면 초대 불가
	if( pTarget->GetBattleArenaID() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_TARGET_IN_BATTLE_ARENA" );
		return;
	}

	// PK 룰 체크
	if( !pClient->IsPartyInvitable( pTarget ) )
	{
		SendChatMessage(false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_PKRULE");
		return;
	}

	// 파티 쪽수 제한
	if( PartyManager::GetInstance().GetMemberCount( nPartyID ) >= PartyManager::MAX_PARTY_MEMBER )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_MAX" );
		return;
	}

	// 파티에 가입 되어 있으면
	if( pTarget->IsInParty() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_ANOTHER" );
		return;
	}

	if( PartyManager::GetInstance().IsAttackTeamParty( nPartyID ) )
	{
		int nAttackTeamGuildID = PartyManager::GetInstance().GetAttackTeamGuildID( nPartyID );

		if( !pTarget->IsInGuild() ||
			( pTarget->GetGuildID() != nAttackTeamGuildID &&
			( !GuildManager::GetInstance().GetAllianceID( pTarget->GetGuildID() ) ||
			GuildManager::GetInstance().GetAllianceID( pTarget->GetGuildID() ) != GuildManager::GetInstance().GetAllianceID( nAttackTeamGuildID ) )
			) )
		{
			SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_NOT_GUILD_MEMBER" );
			return;
		}
	}

	LOG::Log11N4S( LM_PARTY_INVITE, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(),
		pTarget->GetAccountID(), pTarget->GetSID(), pTarget->GetX(), pTarget->GetY(), pTarget->GetLayer(),
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pTarget->GetAccountName(), LOG::STR_NTS, pTarget->GetName(), LOG::STR_NTS );

	PrintfChatMessage( true, CHAT_PARTY_SYSTEM, "@PARTY", pTarget, "INVITE|%s|%s|%d|%d|", pClient->GetName(), partyName.c_str(), nPartyID, PartyManager::GetInstance().GetPartyPassword( nPartyID ) );
}

void onPartyJoin( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 파티에 가입 되어 있으면
	if( pClient->IsInParty() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_YOU_CAN_JOIN_ONLY_ONE_PARTY" );
		return;
	}

	int nPartyID = atoi( vToken[1].c_str() ) ;
	std::string partyName( PartyManager::GetInstance().GetPartyName( nPartyID ) );

	// 없는 파티이면
	if( partyName.empty() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_INVALID_PARTY" );
		return;
	}

	// 헌터홀릭 던전 및 대기실 안에 있으면 파티 가입 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_HUNTAHOLIC" );
		return;
	}

	// 헌터홀릭용 파티나 배틀 아레나용 파티면 초대에 의한 가입 불가
	PartyManager::_PARTY_TYPE ePartyType = PartyManager::GetInstance().GetPartyType( nPartyID );
	if( ePartyType == PartyManager::TYPE_HUNTAHOLIC_PARTY ||
		PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
	{
		return;
	}

	// 인스턴스 던전 안에 있으면 파티 가입 불가
	if( InstanceDungeonManager::Instance().GetInstanceDungeonID( pClient->GetPos() ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_INSTANCE_DUNGEON" );
		return;
	}

	// 데스매치 맵에 있으면 파티 가입 불가
	if( pClient->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_DEATHMATCH" );
		return;
	}

	// 아레나 대기열 또는 경기에 참여 중이면 가입 불가
	if( pClient->GetBattleArenaID() )
	{
		SendChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", pClient, "ERROR_IN_BATTLE_ARENA" );
		return;
	}

	// 수락 거부 메세지를 마스터에게 보내야 함
	if( vToken.size() <= 2 )
	{
		return;
	}

	// 암호가 다르면
	if( atoi( vToken[2].c_str() ) != PartyManager::GetInstance().GetPartyPassword( nPartyID ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "HAS_NO_AUTHORITY" );
		return;
	}

	if( PartyManager::GetInstance().IsAttackTeamParty( nPartyID ) )
	{ 
		int nAttackTeamGuildID = PartyManager::GetInstance().GetAttackTeamGuildID( nPartyID );

		if( !pClient->IsInGuild() ||
			( pClient->GetGuildID() != nAttackTeamGuildID &&
			( !GuildManager::GetInstance().GetAllianceID( pClient->GetGuildID() ) ||
			GuildManager::GetInstance().GetAllianceID( pClient->GetGuildID() ) != GuildManager::GetInstance().GetAllianceID( nAttackTeamGuildID ) )
			) )
		{
			SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_NOT_GUILD_MEMBER" );
			return;
		}
	}

	// 파티 쪽수 제한
	if( PartyManager::GetInstance().GetMemberCount( pClient->GetPartyID() ) >= PartyManager::MAX_PARTY_MEMBER )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_MAX" );
		return;
	}

	if( !PartyManager::GetInstance().JoinParty( nPartyID, pClient ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "CANT_JOIN" );
		return;
	}

	PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "NEW|%s|", pClient->GetName() );

	PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "JOIN|%s|%s|", partyName.c_str(), pClient->GetName() );

	SendPartyInfo( pClient );
	SendLinkedPartyInfo( pClient );

	BroadcastPartyMemberInfo( nPartyID, pClient );	
	BroadcastLinkedPartyMemberInfo( pClient->GetPartyID(), pClient );

	SendSummonInfo( pClient );
	BroadcastSummonInfo( pClient->GetPartyID(), pClient );

	LOG::Log11N4S( LM_PARTY_JOIN, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 0, 0, 0, 0, 0,
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, partyName.c_str(), LOG::STR_NTS, "", 0 );
}

void onPartyKick( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 파티가 아니면 리턴
	int nPartyID = pClient->GetPartyID();
	if( !nPartyID ) return;

	// 리더가 아니면 리턴
	if( !PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) ) return;

	const char *szTargetName = vToken[1].c_str();

	// 그런사람 없으면 리턴
	PlayerUID nPlayerUID = PartyManager::GetInstance().GetMemberUID( nPartyID, szTargetName );
	if( !nPlayerUID ) return;

	// 리더는 스스로를 추방 불가
	if( PartyManager::GetInstance().IsLeader( nPartyID, nPlayerUID ) ) return;

	// 레이드 파티이면서 던전 레이드/시즈 안에 누군가 있으면 킥 불가능
	if( PartyManager::GetInstance().IsSomeoneInSiegeOrRaidDungeon( nPartyID ) )
		return;

	std::string partyName( PartyManager::GetInstance().GetPartyName( nPartyID ) );

	AR_HANDLE hTargetPlayer = StructPlayer::FindPlayer( szTargetName );
	StructPlayer::iterator it = StructPlayer::get( hTargetPlayer );
	StructPlayer *pTarget = (*it);

	if( pTarget && pTarget->IsInSiegeOrRaidDungeon() )
	{
		return;
	}

	// 헌터홀릭용 파티나 배틀 아레나용 파티면 파티 추방 불가
	PartyManager::_PARTY_TYPE ePartyType = PartyManager::GetInstance().GetPartyType( nPartyID );
	if( ePartyType == PartyManager::TYPE_HUNTAHOLIC_PARTY ||
		PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
		return;

	// 대상이 인스턴스 던전 안에 있으면 추방 불가
	if( pTarget && pTarget->IsInInstanceDungeon() )
		return;

	// 데스매치 맵에 있으면 파티 추방 불가
	if( pClient->IsInDeathmatch() )
		return;

	if( pTarget && pTarget->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_DEATHMATCH" );
		return;
	}

	PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "KICK|%s|%s|", partyName.c_str(), szTargetName );

	PartyManager::GetInstance().LeaveParty( nPartyID, nPlayerUID );

	LOG::Log11N4S( LM_PARTY_KICK, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(),
		pTarget ? pTarget->GetAccountID() : 0, pTarget ? pTarget->GetSID() : 0, pTarget ? pTarget->GetX() : 0, pTarget ? pTarget->GetY() : 0, pTarget ? pTarget->GetLayer() : 0,
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pTarget ? pTarget->GetAccountName() : "", pTarget ? LOG::STR_NTS : 0, szTargetName, LOG::STR_NTS );
}

void onPartyLeave( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 파티가 아니면 리턴
	if( !pClient->IsInParty() ) return;

	int nPartyID = pClient->GetPartyID();

	// 리더라면 불가
	if( PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) ) return;

	// 레이드 파티이면서 던전 레이드/시즈 안에 누군가 있으면 탈퇴 불가능
	if( PartyManager::GetInstance().IsSomeoneInSiegeOrRaidDungeon( nPartyID ) )
		return;

	ArPosition pos = pClient->GetPos();

	if( pClient->IsInSiegeOrRaidDungeon() )
		return;

	// 헌터홀릭 파티나 배틀 아레나 파티면 명령어에 의한 파티 탈퇴 불가
	PartyManager::_PARTY_TYPE ePartyType = PartyManager::GetInstance().GetPartyType( nPartyID );
	if( ePartyType == PartyManager::TYPE_HUNTAHOLIC_PARTY ||
		PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) )
		return;

	// 인스턴스 던전 안에 있으면 파티 탈퇴 불가
	if( pClient->IsInInstanceDungeon() )
		return;

	// 데스매치 맵에 있으면 파티 탈퇴 불가
	if( pClient->IsInDeathmatch() )
		return;

	BroadcastPartyLeave( pClient );

	std::string partyName( PartyManager::GetInstance().GetPartyName( nPartyID ) );

	PartyManager::GetInstance().LeaveParty( nPartyID, pClient->GetPlayerUID() );

	LOG::Log11N4S( LM_PARTY_LEAVE, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 0, 0, 0, 0, 0,
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, partyName.c_str(), LOG::STR_NTS, "", 0 );
}

void onPartyPromote( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 파티가 아니면 리턴
	int nPartyID = pClient->GetPartyID();
	if( !nPartyID ) return;

	// 리더가 아니면 리턴
	if( !PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) ) return;

	// 시즈/레이드 공대나 헌터홀릭 파티나 배틀 아레나 파티면 파티 인계 불가
	PartyManager::_PARTY_TYPE ePartyType = PartyManager::GetInstance().GetPartyType( nPartyID );
	if( PartyManager::GetInstance().IsAttackTeamParty( nPartyID ) ||
		ePartyType == PartyManager::TYPE_HUNTAHOLIC_PARTY || ePartyType == PartyManager::TYPE_BATTLE_ARENA_TEAM )
		return;

	// 배틀 아레나 연습 경기용 파티라면 팀이 레디 상태면 인계 불가
	if( ePartyType == PartyManager::TYPE_BATTLE_ARENA_EXERCISE_TEAM &&
		BattleArenaManager::Instance().IsReadyExerciseTeam( nPartyID ) )
		return;

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) ) );
	StructPlayer *pTarget = *pit;

	// 대상이 없으면 리턴
	if( !pTarget ) return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	// 대상이 파티원이 아니면 리턴
	if( pTarget->GetPartyID() != nPartyID ) return;

	// 대상이 헌터홀릭 던전 및 로비에 있으면 파티 인계 불가
	if( HuntaholicManager::Instance().GetHuntaholicID( pTarget->GetPos() ) )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_TARGET_IN_HUNTAHOLIC" );
		return;
	}

	// 데스매치 맵에 있으면 파티 인계 불가
	if( pClient->IsInDeathmatch() )
		return;
	if( pTarget->IsInDeathmatch() )
	{
		SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, "ERROR_IN_DEATHMATCH" );
		return;
	}

	if( PartyManager::GetInstance().Promote( nPartyID, pTarget ) == false )
	{
		return;
	}

	PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "PROMOTE|%d|%s|", nPartyID, pTarget->GetName() );

	LOG::Log11N4S( LM_PARTY_PROMOTE, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(),
		pTarget->GetAccountID(), pTarget->GetSID(), pTarget->GetX(), pTarget->GetY(), pTarget->GetLayer(),
		pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pTarget->GetAccountName(), LOG::STR_NTS, pTarget->GetName(), LOG::STR_NTS );
}

void onPartyList( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	struct myPartyFunctor : PartyManager::PartyFunctor
	{
		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer *pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( handle ) );
			if( !pPlayer ) return true;

			char buf[256];
			s_sprintf( buf, _countof( buf ), "%s|%d|%d|", pPlayer->GetName(), pPlayer->GetHPPercentage(), pPlayer->GetMPPercentage() );
			strList += buf;

			return true;
		}
		std::string strList;
	} _fo;

	PartyManager::GetInstance().DoEachMember( pClient->GetPartyID(), _fo );

	char buf[2048];
	s_sprintf( buf, _countof( buf ), "LIST|%s|%d|%d|%d|%s", 
		PartyManager::GetInstance().GetPartyName( pClient->GetPartyID() ).c_str(),
		PartyManager::GetInstance().GetShareMode( pClient->GetPartyID() ),
		PartyManager::GetInstance().GetMaxLevel( pClient->GetPartyID() ),
		PartyManager::GetInstance().GetMinLevel( pClient->GetPartyID() ),
		_fo.strList.c_str() );

	SendChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", pClient, buf, (unsigned)strlen(buf) );
}

void onAllianceInvite( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 길드에 소속되어 있지 않으면 
	if( !pClient->IsInGuild() ) return;

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( pClient->GetGuildID() );

	if( !nAllianceID )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_NOT_IN_ALLIANCE" );
		return;
	}

	std::string allianceName( GuildManager::GetInstance().GetAllianceName( nAllianceID ) );

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( pClient->GetGuildID(), pClient->GetGuildPermission(), GuildManager::PRA_ALLIANCE_MANAGEMENT ) )
	{
		//SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_NOT_GUILD_LEADER" );
		return;
	}
	if( !GuildManager::GetInstance().IsAllianceLeader( nAllianceID, pClient->GetGuildID() ) )
	{
		//SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_NOT_ALLIANCE_LEADER" );
		return;
	}

	// 연합 최대 길드 수 제한 체크
	if( GuildManager::GetInstance().GetMaxAllianceCount( nAllianceID ) <= GuildManager::GetInstance().GetAllianceMemberGuildCount( nAllianceID ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_LIMIT_EXCEED" );
		return;
	}

	// 던전 소유 중인 연합은 초대 불가
	int nMasterGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID );
	if( DungeonManager::Instance().GetOwnGuildID( GuildManager::GetInstance().GetRaidDungeonID( nMasterGuildID ) ) == nMasterGuildID )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_IS_RAID" );
		return;
	}

	// 유효하지 않은 길드는 초대 불가
	int nGuildId = GuildManager::GetInstance().GetGuildID( vToken[1].c_str() );
	if( !nGuildId )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_INVALID_GUILD" );
		return;
	}

	// 던전 레이드 신청 상태 또는 던전 소유 중인 길드를 초대하는 것은 불가
	int nDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nGuildId );
	if( nDungeonID )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_IS_RAID" );
		return;
	}

	// 다른 연합에 소속되어 있는 길드를 초대하는 것은 불가
	if( GuildManager::GetInstance().GetAllianceID( nGuildId ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_ALREADY_IN_ALLIANCE" );
		return;
	}

	// 다른 연합에서 탈퇴/해산한 지 7일이 지나지 않은 길드를 초대하는 것은 불가
	if( GuildManager::GetInstance().IsInGuildAllianceBlockTime( nGuildId ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_ALLIANCE_BLOCK_TIME_LIMIT" );
		return;
	}

	std::string strTargetLeaderName = StructPlayer::GetPlayerName( GuildManager::GetInstance().GetLeaderSID( nGuildId ) );

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( strTargetLeaderName.c_str() ) ) );
	StructPlayer *pTarget = *pit;

	// 대상이 없으면 리턴
	if( !pTarget )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_NOT_ONLINE" );
		return;
	}

	PrintfChatMessage( true, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pTarget, "INVITE|%s|%s|%d|%d|", pClient->GetName(), allianceName.c_str(), nAllianceID, GuildManager::GetInstance().GetAlliancePassword( nAllianceID )  );
}

void onAllianceKick( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 길드가 아니면 리턴
	int nGuildID = pClient->GetGuildID();
	if( !nGuildID ) return;

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );

	if( !nAllianceID ) return;

	std::string allianceName( GuildManager::GetInstance().GetAllianceName( nAllianceID ) );

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_ALLIANCE_MANAGEMENT ) )
	{
		//SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_NOT_GUILD_LEADER" );
		return;
	}
	if( !GuildManager::GetInstance().IsAllianceLeader( nAllianceID, nGuildID ) )
	{
		//SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_ALLIANCE_LEADER_NOT_LEAVABLE" );
		return;
	}

	// 던전 소유 연합은 멤버 길드 추방 불가
	int nDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nGuildID );
	if( nDungeonID && DungeonManager::Instance().GetOwnGuildID( nDungeonID ) == nGuildID )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_IS_RAID" );
		return;
	}

	const char *szGuildName = vToken[1].c_str();

	int nTargetGuildID = GuildManager::GetInstance().GetGuildID( szGuildName );

	if( !nTargetGuildID || nAllianceID != GuildManager::GetInstance().GetAllianceID( nTargetGuildID ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_NOT_IN_ALLIANCE" );
		return;
	}

	if( nTargetGuildID == nGuildID )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_IS_GUILD_MASTER" );
		return;
	}

	// 탈퇴할 길드가 공대 파티를 결성했으면 탈퇴 불가(한 명이라도 공대 파티에 소속이 되어있는지와 동일)
	if( PartyManager::GetInstance().IsExistAttackTeam( nTargetGuildID ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "IS_IN_RAID_PARTY" );
		return;
	}

	// 탈퇴할 길드의 길드원 중 한 명이라도 던전 시즈 또는 레이드에 입장해 있으면 탈퇴 불가
	struct GuildFunctorIsInSiegeOrRaid : GuildManager::GuildFunctor
	{
		GuildFunctorIsInSiegeOrRaid() : bSomeoneInSiegeOrRaid( false )
		{}

		virtual bool operator()( AR_HANDLE handle )
		{
			if( bSomeoneInSiegeOrRaid )
				return false;

			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( pPlayer && pPlayer->IsInWorld() && pPlayer->IsInSiegeOrRaidDungeon() )
			{
				bSomeoneInSiegeOrRaid = true;
				return false;
			}

			return true;
		}

		bool bSomeoneInSiegeOrRaid;
	} _foIsInSiegeOrRaid;
	GuildManager::GetInstance().DoEachMember( nTargetGuildID, _foIsInSiegeOrRaid );

	if( _foIsInSiegeOrRaid.bSomeoneInSiegeOrRaid )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_IN_SIEGE_OR_RAID" );
		return;
	}

	if( !GuildManager::GetInstance().LeaveAlliance( nTargetGuildID ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_CANT_KICK" );
		return;
	}

	PrintfAllianceChatMessage( CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", nAllianceID, "KICK|%s|%s|", allianceName.c_str(), szGuildName );
	PrintfGuildChatMessage( CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", nTargetGuildID, "KICK|%s|%s|", allianceName.c_str(), szGuildName );
	BroadcastAllianceInfo( nAllianceID );

	LOG::Log11N4S( LM_ALLIANCE_KICK, pClient->GetAccountID(), pClient->GetSID(), 0, nTargetGuildID, nAllianceID, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, szGuildName, LOG::STR_NTS, allianceName.c_str(), LOG::STR_NTS );
}

void onAllianceLeave( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 길드가 아니면 리턴
	int nGuildID = pClient->GetGuildID();
	if( !nGuildID )
	{
		//SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_NOT_IN_GUILD" );
		return;
	}

	int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );

	if( !nAllianceID ) return;

	int nLeadGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID );

	// 연합 리더는 탈퇴 불가
	if( nGuildID == nLeadGuildID )
	{
		//SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_ALLIANCE_LEADER_NOT_LEAVABLE" );
		return;
	}

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_ALLIANCE_LEAVE ) )
	{
		//SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_NOT_GUILD_LEADER" );
		return;
	}
	// 던전 주인인 연합에서는 탈퇴 불가
	int nDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nLeadGuildID );
	if( nDungeonID && DungeonManager::Instance().GetOwnGuildID( nDungeonID ) == nLeadGuildID )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_IS_RAID" );
		return;
	}

	// 탈퇴할 길드가 공대 파티를 결성했으면 탈퇴 불가(한 명이라도 공대 파티에 소속이 되어있는지와 동일)
	if( PartyManager::GetInstance().IsExistAttackTeam( nGuildID ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "IS_IN_RAID_PARTY" );
		return;
	}

	// 탈퇴할 길드의 길드원 중 한 명이라도 던전 시즈 또는 레이드에 입장해 있으면 탈퇴 불가
	struct GuildFunctorIsInSiegeOrRaid : GuildManager::GuildFunctor
	{
		GuildFunctorIsInSiegeOrRaid() : bSomeoneInSiegeOrRaid( false )
		{}

		virtual bool operator()( AR_HANDLE handle )
		{
			if( bSomeoneInSiegeOrRaid )
				return false;

			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( pPlayer && pPlayer->IsInWorld() && pPlayer->IsInSiegeOrRaidDungeon() )
			{
				bSomeoneInSiegeOrRaid = true;
				return false;
			}

			return true;
		}

		bool bSomeoneInSiegeOrRaid;
	} _foIsInSiegeOrRaid;
	GuildManager::GetInstance().DoEachMember( nGuildID, _foIsInSiegeOrRaid );

	if( _foIsInSiegeOrRaid.bSomeoneInSiegeOrRaid )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_IN_SIEGE_OR_RAID" );
		return;
	}

	std::string strGuildName = GuildManager::GetInstance().GetGuildName( nGuildID );
	std::string strAllianceName( GuildManager::GetInstance().GetAllianceName( nAllianceID ) );

	if( !GuildManager::GetInstance().LeaveAlliance( nGuildID ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_CANT_LEAVE" );
		return;
	}

	PrintfAllianceChatMessage( CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", nAllianceID, "LEAVE|%s|%s|", strAllianceName.c_str(), strGuildName.c_str() );
	PrintfGuildChatMessage( CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", nGuildID, "LEAVE|%s|%s|", strAllianceName.c_str(), strGuildName.c_str() );
	BroadcastAllianceInfo( nAllianceID );

	LOG::Log11N4S( LM_ALLIANCE_LEAVE, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, nAllianceID, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, strGuildName.c_str(), LOG::STR_NTS, strAllianceName.c_str(), LOG::STR_NTS );
}

void onAllianceJoin( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 길드에 가입 되어 있지 않으면
	int nGuildID = pClient->GetGuildID();
	if( !nGuildID )
	{
		//SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_NOT_IN_GUILD" );
		return;
	}

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_ALLIANCE_JOIN ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_PERMISSION" );
		return;
	}

	// 던전 레이드 신청 상태 또는 던전 소유 중이면 가입 불가
	int nDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nGuildID );
	if( nDungeonID )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_IS_RAID" );
		return;
	}

	// 다른 연합에 소속되어 있으면 가입 불가
	if( GuildManager::GetInstance().GetAllianceID( nGuildID ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_ALREADY_IN_ALLIANCE" );
		return;
	}

	// 다른 연합에서 탈퇴/해산한 지 7일이 지나지 않았으면 가입 불가
	if( GuildManager::GetInstance().IsInGuildAllianceBlockTime( nGuildID ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_ALLIANCE_BLOCK_TIME_LIMIT" );
		return;
	}

	// 던전 소유권을 포기한 지 7일이 지나지 않았으면 가입 불가
	if( GuildManager::GetInstance().GetDungeonBlockTime( nGuildID ) )
	{
		//SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_DUNGEON_BLOCK_TIME_LIMIT" );
		return;
	}

	int nAllianceID = atoi( vToken[1].c_str() ) ;

	std::string allianceName( GuildManager::GetInstance().GetAllianceName( nAllianceID ) );
	std::string guildName( GuildManager::GetInstance().GetGuildName( nGuildID ) );

	// 유효하지 않은 연합에 가입 불가
	if( allianceName.empty() )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_INVALID_ALLIANCE" );
		return;
	}

	// 연합 최대 길드 수 제한 체크
	if( GuildManager::GetInstance().GetMaxAllianceCount( nAllianceID ) <= GuildManager::GetInstance().GetAllianceMemberGuildCount( nAllianceID ) )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_LIMIT_EXCEED" );
		return;
	}

	// 던전 소유 중인 연합에 가입 불가
	int nMasterGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID );
	if( DungeonManager::Instance().GetOwnGuildID( GuildManager::GetInstance().GetRaidDungeonID( nMasterGuildID ) ) == nMasterGuildID )
	{
		SendChatMessage( false, CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", pClient, "ERROR_IS_RAID" );
		return;
	}

	if( vToken.size() > 2 )
	{
		// 암호가 다르면
		if( atoi( vToken[2].c_str() ) != GuildManager::GetInstance().GetAlliancePassword( nAllianceID ) )
		{
			SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_PERMISSION_DENIED" );
			return;
		}

		if( !GuildManager::GetInstance().AddToAlliance( nAllianceID, nGuildID ) )
		{
			SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_CANT_JOIN_ALLIANCE" );
			return;
		}

		PrintfAllianceChatMessage( CHAT_ALLIANCE_SYSTEM, "@ALLIANCE", nAllianceID, "JOIN|%s|%s|", allianceName.c_str(), guildName.c_str() );

		BroadcastAllianceInfo( nAllianceID );
	}
	else
	{
		// TODO : 수락 거부 메세지를 마스터에게 보내야함.
	}

	LOG::Log11N4S( LM_ALLIANCE_JOIN, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, nAllianceID, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, guildName.c_str(), LOG::STR_NTS, allianceName.c_str(), LOG::STR_NTS );
}

void onAllianceCreate( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	if( !pClient->GetLastContactLong( "alliance" ) )
		return;

	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;	

	// Lua injection을 방지하기 위해서 몇 가지 제어 문자가 포함된 경우를 먼저 체크하고 그런 경우는 접속을 끊음
	if( vToken[1].find( '\'' ) != std::string::npos || vToken[1].find( '\\' ) != std::string::npos )
	{
		_cprint( "Protecting from lua injection(%s). [%s/%s]\n", vToken[1].c_str(), pClient->GetAccountName(), pClient->GetName() );
		FILELOG( "Protecting from lua injection(%s). [%s/%s]", vToken[1].c_str(), pClient->GetAccountName(), pClient->GetName() );

		pClient->pConnection->Close();
		return;
	}

	const char *pAllianceName = vToken[1].c_str();
	if( !pAllianceName ) return;
	size_t allianceLen = strlen( pAllianceName );
	if( allianceLen > 30 ) return;

	// 연합 결성 불가 룰 체크는 SCRIPT_CreateAlliance 함수에서 해야 함
	// NPC를 통해서 오류 메시지를 출력해야 하기 때문

	std::string strCreateGuildFunction = "on_create_alliance( '";
	strCreateGuildFunction  += pAllianceName;
	strCreateGuildFunction  += "' )";

	int nPrevChaos = pClient->GetChaos();
	StructGold nPrevGold( pClient->GetGold() );

	LUA()->RunString( strCreateGuildFunction.c_str() );

	pClient->SetLastContact( "alliance", 0 );

	StructNPC * pNPC = NULL;
	StructCreature::iterator itNPC = StructCreature::get( pClient->GetContactNPCHandle() );
	if( (*itNPC) && (*itNPC)->IsNPC() )
		pNPC = static_cast< StructNPC * >( *itNPC );

	LOG::Log11N4S( LM_NPC_PROCESS, pClient->GetAccountID(), pClient->GetSID(), 0, ( pNPC ) ? pNPC->GetNPCID() : 0, nPrevChaos, nPrevGold.GetRawData(), pClient->GetChaos(), pClient->GetGold().GetRawData(), pClient->GetX(), pClient->GetY(), 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, strCreateGuildFunction.c_str(), LOG::STR_NTS );

	pClient->Save( true );
}


void onGuildCreate( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	if( !pClient->GetLastContactLong( "guild" ) )
		return;

	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;	

	// Lua injection을 방지하기 위해서 몇 가지 제어 문자가 포함된 경우를 먼저 체크하고 그런 경우는 접속을 끊음
	if( vToken[1].find( '\'' ) != std::string::npos || vToken[1].find( '\\' ) != std::string::npos )
	{
		_cprint( "Protecting from lua injection(%s). [%s/%s]\n", vToken[1].c_str(), pClient->GetAccountName(), pClient->GetName() );
		FILELOG( "Protecting from lua injection(%s). [%s/%s]", vToken[1].c_str(), pClient->GetAccountName(), pClient->GetName() );

		pClient->pConnection->Close();
		return;
	}

	const char *pGuildName = vToken[1].c_str();
	size_t guildLen = vToken[1].length();
	if( guildLen > 30 ) return;

	// 길드 결성 불가 룰 체크는 SCRIPT_CreateGuild 함수에서 해야 함
	// NPC를 통해서 오류 메시지를 출력해야 하기 때문

	std::string strCreateGuildFunction = "on_create_guild( '";
	strCreateGuildFunction  += pGuildName;
	strCreateGuildFunction  += "' )";

	int nPrevChaos = pClient->GetChaos();
	StructGold nPrevGold( pClient->GetGold() );

	LUA()->RunString( strCreateGuildFunction.c_str() );

	pClient->SetLastContact( "guild", 0 );

	StructNPC * pNPC = NULL;
	StructCreature::iterator itNPC = StructCreature::get( pClient->GetContactNPCHandle() );
	if( (*itNPC) && (*itNPC)->IsNPC() )
		pNPC = static_cast< StructNPC * >( *itNPC );

	LOG::Log11N4S( LM_NPC_PROCESS, pClient->GetAccountID(), pClient->GetSID(), 0, ( pNPC ) ? pNPC->GetNPCID() : 0, nPrevChaos, nPrevGold.GetRawData(), pClient->GetChaos(), pClient->GetGold().GetRawData(), pClient->GetX(), pClient->GetY(), 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, strCreateGuildFunction.c_str(), LOG::STR_NTS );

	// 길드장이 비정상 종료 되도 길드는 유지되도록 길드장을 저장함
	pClient->Save( true );
}

void onGuildDestroy( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	int nGuildID = pClient->GetGuildID();

	// 길드가 아니면 리턴
	if( !nGuildID )
	{
		return;
	}

	// 길드 리더가 아니면 리턴
	if( !GuildManager::GetInstance().IsLeader( nGuildID, pClient->GetPlayerUID() ) )
	{
		return;
	}

	if( GuildManager::GetInstance().GetAllianceID( nGuildID ) )
	{
		SendChatMessage(false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_DESTROY_IN_ALLIANCE");
		return;
	}

	if( PartyManager::GetInstance().IsExistAttackTeam( nGuildID ) )
	{
		SendChatMessage(false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_DESTROY_IN_ATTACK_TEAM");
		return;
	}

	int nDungeonID = GuildManager::GetInstance().GetRaidDungeonID( nGuildID );
	if( nDungeonID )
	{
		if( nGuildID != DungeonManager::Instance().GetOwnGuildID( nDungeonID ) )
		{
			SendChatMessage(false, CHAT_NOTICE, "@NOTICE", pClient, "@643");
			return;
		}
		else
		{
			SendChatMessage(false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_DESTROY_DUNGEON_OWNER");
			return;
		}
	}

	struct myGuildFunctor : GuildManager::GuildFunctor
	{
		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer *pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( handle ) );
			if( !pPlayer ) return true;

			PrintfChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pPlayer, "DESTROY|%s|", GuildManager::GetInstance().GetGuildName( pPlayer->GetGuildID() ).c_str() );

			return true;
		}
	} _fo;

	LOG::Log11N4S( LM_GUILD_DESTROY, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, 0, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(), LOG::STR_NTS, "", 0 );

	GuildManager::GetInstance().DoEachMember( nGuildID, _fo );
	GuildManager::GetInstance().DestroyGuild( nGuildID );	

	char szBuf[255];
	s_sprintf( szBuf, _countof( szBuf ), "GDESTROY|%d", nGuildID );
	SendGlobalChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", szBuf, static_cast< int >( strlen( szBuf ) ) );
}

void onGuildInvite( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 길드가 아니면 리턴
	if( !pClient->IsInGuild() ) return;

	std::string guildName( GuildManager::GetInstance().GetGuildName( pClient->GetGuildID() ) );

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( pClient->GetGuildID(), pClient->GetGuildPermission(), GuildManager::PRA_MEMBER_INVITE ) ) return;

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) ) );
	StructPlayer *pTarget = *pit;

	// 대상이 없으면 리턴
	if( !pTarget )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_NOT_EXIST" );
		return;
	}

	// 길드에 가입 되어 있으면
	if( pTarget->IsInGuild() )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_TARGET_ANOTHER" );
		return;
	}

	if( pTarget->GetPrevGuildID() )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_GUILD_BLOCK_TIME_LIMIT" );
		return;
	}

	// 길드 쪽수 제한
	if( GuildManager::GetInstance().GetMemberCount( pClient->GetGuildID() ) >= GuildManager::MAX_GUILD_MEMBER )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_MAX" );
		return;
	}

	// PK 룰 체크
	if( !pClient->IsGuildInvitable( pTarget ) )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_PKRULE" );
		return;
	}

	PrintfChatMessage( true, CHAT_GUILD_SYSTEM, "@GUILD", pTarget, "INVITE|%s|%s|%d|%d|", pClient->GetName(), guildName.c_str(), pClient->GetGuildID(), GuildManager::GetInstance().GetGuildPassword( pClient->GetGuildID() )  );

	LOG::Log11N4S( LM_GUILD_INVITE, pClient->GetAccountID(), pClient->GetSID(), 0, pClient->GetGuildID(), pTarget->GetAccountID(), pTarget->GetSID(), 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, guildName.c_str(), LOG::STR_NTS, pTarget->GetName(), LOG::STR_NTS );
}

void onGuildJoin( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 길드에 가입 되어 있으면
	if( pClient->IsInGuild() )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_YOU_CAN_JOIN_ONLY_ONE_GUILD" );
		return;
	}

	int nGuildID = atoi( vToken[1].c_str() ) ;

	// 탈퇴한지 7일이 안지났으면 못 드가
	if( pClient->GetPrevGuildID() )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_GUILD_BLOCK_TIME_LIMIT" );
		return;
	}

	std::string guildName( GuildManager::GetInstance().GetGuildName( nGuildID ) );

	if( guildName.empty() )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_INVALID_GUILD" );
		return;
	}

	bool bJoined = false;

	if( vToken.size() > 2 )
	{
		// 암호가 다르면
		if( atoi( vToken[2].c_str() ) != GuildManager::GetInstance().GetGuildPassword( nGuildID ) )
		{
			SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "HAS_NO_AUTHORITY" );
			return;
		}

		// 길드 쪽수 제한
		int nMemberCount = GuildManager::GetInstance().GetMemberCount( pClient->GetGuildID() );
		if( nMemberCount >= GuildManager::MAX_GUILD_MEMBER )
		{
			SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_MAX" );
			return;
		}

		if( GuildManager::GetInstance().JoinGuild( nGuildID, pClient ) )
		{
			bJoined = true;

			PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "NEW|%s|", pClient->GetName() );

			PrintfChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "JOIN|%s|", guildName.c_str() );
			SendGuildInfo( pClient );
			SendGuildNotify( pClient );
			BroadcastGuildMemberInfo( nGuildID, pClient );

			int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );
			if( nAllianceID )
			{
				PrintfAllianceChatMessage( CHAT_ALLIANCE_SYSTEM, nAllianceID, "@ALLIANCE", "GMEMBER_CHANGE|%d|%d|", nGuildID, nMemberCount + 1 );
			}

			// 던전 내에서 길드에 가입하였고 그 길드가 던전 소유 길드라면 pClient는 던전 소유 길드원으로서 다시 입장 처리를 해줘야 한다.
			if( !pClient->GetLayer() )
			{
				int nDungeonID = DungeonManager::Instance().GetDungeonID( pClient->GetPos().x, pClient->GetPos().y );

				if( nDungeonID )
				{
					ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

					DungeonManager::Instance().OnEnterDungeon( nDungeonID, pClient, pClient->GetLayer() );
				}
			}
		}
	}
	else
	{
		// TODO : 수락 거부 메세지를 마스터에게 보내야함.
	}

	LOG::Log11N4S( LM_GUILD_JOIN, pClient->GetAccountID(), pClient->GetSID(), 0, pClient->GetGuildID(), ( bJoined ) ? 1 : 2, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, guildName.c_str(), LOG::STR_NTS, "", 0 );
}

void onGuildKick( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	// 길드가 아니면 리턴
	int nGuildID = pClient->GetGuildID();
	if( !nGuildID ) return;

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_MEMBER_KICK ) ) return;

	const char *szTargetName = vToken[1].c_str();

	// 그런 사람 없으면 리턴(권한 검사를 위해 다시 길드 멤버 목록을 돌지 않도록 멤버 IsMember함수를 사용하지 않고 멤버 정보를 얻음)
#ifdef USE_CLIENT97
	GuildManager::GuildMemberTag gmt(0, 0, 0, GuildManager::PERMISSION_NONE, "", 0, 0);

#else
	GuildManager::GuildMemberTag gmt(0, 0, 0, GuildManager::PERMISSION_NONE, "");
#endif // USE_CLIENT97

	
	if( !GuildManager::GetInstance().GetMemberInfo( nGuildID, szTargetName, &gmt ) ) return;

	// 권한이 없으면 리턴(권한 등급 비교)
	if( gmt.nPermission >= pClient->GetGuildPermission() )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_PERMISSION" );
		return;
	}

	// 길드 리더 스스로 추방 불가
	if( !_stricmp( pClient->GetName(), szTargetName ) ) return;

	StructPlayer::iterator pit = StructPlayer::get( StructPlayer::FindPlayer( szTargetName ) );
	StructPlayer *pPtr = *pit;

	if( pPtr && pPtr->IsInSiegeOrRaidDungeon() )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_IN_SIEGE_OR_RAID" );
		return;
	}

	if( pPtr && pPtr->IsInParty() && PartyManager::GetInstance().IsAttackTeamParty( pPtr->GetPartyID() ) )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_IN_ATTACK_TEAM" );
		return;
	}

	std::string guildName( GuildManager::GetInstance().GetGuildName( nGuildID ) );
	bool bResult = GuildManager::GetInstance().LeaveGuild( nGuildID, szTargetName );
	if( bResult )
	{
		PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "KICK|%s|%s|", guildName.c_str(), szTargetName );

		int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );
		if( nAllianceID )
		{
			PrintfAllianceChatMessage( CHAT_ALLIANCE_SYSTEM, nAllianceID, "@ALLIANCE", "GMEMBER_CHANGE|%d|%d|", nGuildID, GuildManager::GetInstance().GetMemberCount( nGuildID ) );
		}

		if( pPtr )
		{
			PrintfChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pPtr, "KICK|%s|%s|", guildName.c_str(), szTargetName );

			SendGuildNotify( pPtr );

			// 던전 내에서 추방 당했고 추방 당하기 전 소속 길드가 던전 소유 길드였다면 pClient는 던전 소유 길드원으로서 퇴장 처리를 해줘야 한다.
			if( !pPtr->GetLayer() )
			{
				int nDungeonID = DungeonManager::Instance().GetDungeonID( pPtr->GetPos().x, pPtr->GetPos().y );

				if( nDungeonID )
				{
					ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPtr ) );

					DungeonManager::Instance().OnExitDungeon( nDungeonID, pPtr, nGuildID, pPtr->GetLayer() );
				}
			}
		}
	}

	LOG::Log11N4S( LM_GUILD_KICK, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, (pPtr) ? pPtr->GetSID() : -1, bResult, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, guildName.c_str(), LOG::STR_NTS, szTargetName, LOG::STR_NTS );
}

void onGuildLeave( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 길드가 아니면 리턴
	if( !pClient->IsInGuild() ) return;

	int nGuildID = pClient->GetGuildID();
	// 리더라면 불가
	if( GuildManager::GetInstance().IsLeader( pClient->GetGuildID(), pClient->GetPlayerUID() ) ) return;

	std::string guildName( GuildManager::GetInstance().GetGuildName( pClient->GetGuildID() ) );

	if( pClient->IsInSiegeOrRaidDungeon() )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "@2802" );
		return;
	}

	if( pClient->IsInParty() && PartyManager::GetInstance().IsAttackTeamParty( pClient->GetPartyID() ) )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "@2802" );
		return;
	}

	bool bResult = GuildManager::GetInstance().LeaveGuild( nGuildID, pClient->GetName() );
	if( bResult )
	{
		PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "LEAVE|%s|", pClient->GetName() );
		PrintfChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "LEAVE|%s|", pClient->GetName() );

		int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );

		if( nAllianceID )
		{
			PrintfAllianceChatMessage( CHAT_ALLIANCE_SYSTEM, nAllianceID, "@ALLIANCE", "GMEMBER_CHANGE|%d|%d|", nGuildID, GuildManager::GetInstance().GetMemberCount( nGuildID ) );
		}

		SendGuildNotify( pClient );

		// 던전 내에서 길드 탈퇴를 하였고 탈퇴하기 전 소속 길드가 던전 소유 길드였다면 pClient는 던전 소유 길드원으로서 퇴장 처리를 해줘야 한다.
		if( !pClient->GetLayer() )
		{
			int nDungeonID = DungeonManager::Instance().GetDungeonID( pClient->GetPos().x, pClient->GetPos().y );

			if( nDungeonID )
			{
				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

				DungeonManager::Instance().OnExitDungeon( nDungeonID, pClient, nGuildID, pClient->GetLayer() );
			}
		}
	}

	LOG::Log11N4S( LM_GUILD_LEAVE, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, 0, bResult, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, guildName.c_str(), LOG::STR_NTS, "", 0 );
}

void onGuildPromote( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 2 ) return;

	int nGuildID = pClient->GetGuildID();

	// 길드가 아니면 리턴
	if( !nGuildID ) return;

	// 리더가 아니면 리턴
	if( !GuildManager::GetInstance().IsLeader( nGuildID, pClient->GetPlayerUID() ) ) return;

	int dungeon_id = GuildManager::GetInstance().GetRaidDungeonID( nGuildID );
	if( dungeon_id )
	{
		// 레이드 기간일 때는 신청 기록 혹은 완료 기록이 있으면 불가능
		if( DungeonManager::Instance().IsDungeonRaidTime( dungeon_id ) )
		{
			// 던전 주인이면 신청 기록이 아니므로 가능. 그 외에는 리턴
			if( DungeonManager::Instance().GetOwnGuildID( dungeon_id ) != nGuildID )
			{
				SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "@2802" );
				return;
			}
		}
		// 레이드 기간이 아니라면 해당 던전의 주인 혹은 시즈 도전자 이므로 해당 던전에 도전자가 있으면 안됨.(시즈 종료 전임)
		else if( DungeonManager::Instance().GetRaidGuildID( dungeon_id ) )
		{
			SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "@2802" );
			return;
		}
	}

	// 시즈공대 파티가 결성되어있으면 리턴
	if( PartyManager::GetInstance().GetAttackTeamLeadPartyIDByGuildID( nGuildID ) )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "@2802" );
		return;
	}

	StructPlayer::iterator pit( StructPlayer::get( StructPlayer::FindPlayer( vToken[1].c_str() ) ) );
	StructPlayer *pTarget = *pit;

	// 대상이 없으면 리턴
	if( !pTarget ) return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	// 대상이 길드원이 아니면 리턴
	if( pTarget->GetGuildID() != nGuildID ) return;

	// 자신에게 인계하려는 거면 리턴
	if( pTarget == pClient ) return;

	if( PartyManager::GetInstance().IsExistAttackTeam( nGuildID ) ) return;

	bool bResult = GuildManager::GetInstance().Promote( nGuildID, pTarget );
	if( bResult )
	{
		SendGuildMemberInfo( pTarget );
		SendGuildMemberInfo( pClient );	

		PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "PROMOTE||%s", pTarget->GetName() );

		int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );

		if( nAllianceID )
		{
			PrintfAllianceChatMessage( CHAT_ALLIANCE_SYSTEM, nAllianceID, "@ALLIANCE", "GLEADER_CHANGE|%d|%s|", nGuildID, pTarget->GetName() );
		}
	}

	LOG::Log11N4S( LM_GUILD_PROMOTE, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, pTarget->GetSID(), 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName( pClient->GetGuildID() ).c_str(), LOG::STR_NTS, pTarget->GetName(), LOG::STR_NTS );
}

void onGuildPermission( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 3 )
		return;

	int nGuildID = pClient->GetGuildID();

	// 길드가 아니면 리턴
	if( !nGuildID )
		return;

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_GRANT_REVOKE_PERMISSION ) )
		return;

	// 자신의 권한을 변경하려고 하는 거면 리턴
	if( !_stricmp( vToken[ 1 ].c_str(), pClient->GetName() ) )
		return;

	char nPermission = static_cast< char >( atoi( vToken[ 2 ].c_str() ) );

	// 자신보다 높은 권한으로 대상의 권한을 변경하려고 하면 리턴
	if( nPermission > pClient->GetGuildPermission() )
		return;

	// 그런 사람 없으면 리턴(권한 검사를 위해 다시 길드 멤버 목록을 돌지 않도록 멤버 IsMember함수를 사용하지 않고 멤버 정보를 얻음, 로그에 캐릭터 sid도 사용해야 함)
#ifdef USE_CLIENT97
	GuildManager::GuildMemberTag gmt( 0, 0, 0, GuildManager::PERMISSION_NONE, "",0,0 );
#else
	GuildManager::GuildMemberTag gmt(0, 0, 0, GuildManager::PERMISSION_NONE, "");
#endif
	if( !GuildManager::GetInstance().GetMemberInfo( nGuildID, vToken[ 1 ].c_str(), &gmt ) )
		return;

	// 대상이 자신과 같거나 더 높은 권한을 가지고 있을 경우에는 리턴
	if( gmt.nPermission >= pClient->GetGuildPermission() )
		return;

	if( !GuildManager::GetInstance().SetPermission( nGuildID, vToken[ 1 ].c_str(), nPermission ) )
		return;

	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "CHANGE_PERMISSION|%s|%d|", vToken[ 1 ].c_str(), nPermission );
	LOG::Log11N4S( LM_GUILD_PERMISSION, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, gmt.sid, gmt.nPermission, nPermission, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(), LOG::STR_NTS, vToken[ 1 ].c_str(), LOG::STR_NTS );
}

void onGuildPermissionSet( StructPlayer* pClient, std::vector< std::string > & vToken )
{
	// 인자가 없으면 리턴
	if( vToken.size() < 3 )
		return;

	int nGuildID = pClient->GetGuildID();

	// 길드가 아니면 리턴
	if( !nGuildID )
		return;

	// 권한이 없으면 리턴(길마만 가능)
	if( GuildManager::GetInstance().GetLeaderSID( nGuildID ) != pClient->GetSID() )
		return;

	char nPermission = static_cast< char >( atoi( vToken[ 1 ].c_str() ) );

	// 자신의 등급이나 자신보다 높은 등급의 권한을 수정하려고 하면 리턴
	if( nPermission >= pClient->GetGuildPermission() )
		return;

	int nPermissionSet = atoi( vToken[ 2 ].c_str() );

	// 길마가 아닐 경우 자신이 가지고 있는 권한에 대해서만 변경 가능(기존 설정과 다르게 변경되는 비트가 자신이 보유하고 있는 권한에 포함되는지 체크)
	if( pClient->GetGuildPermission() != GuildManager::PERMISSION_LEADER )
	{
		int nDifference = nPermissionSet ^ GuildManager::GetInstance().GetPermissionSet( nGuildID, nPermissionSet );
		if( ( nDifference & GuildManager::GetInstance().GetPermissionSet( nGuildID, pClient->GetGuildPermission() ) ) != nDifference )
			return;
	}

	GuildManager::GetInstance().SetPermissionSet( nGuildID, nPermission, nPermissionSet );

	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "CHANGE_PERMISSION_SET|%d|%d|", nPermission, nPermissionSet );
	LOG::Log11N4S( LM_GUILD_PERMISSION_SET, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, nPermission, nPermissionSet, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(), LOG::STR_NTS, "", 0 );
}

void onGuildPermissionName( StructPlayer* pClient, const std::string & strMessage )
{
	std::vector< std::string > vToken;
	XStringUtil::Split( strMessage.c_str(), vToken, " ", false );

	// 인자가 없으면 리턴(전체 스트링에서 띄어쓰기를 여러 개 쓴 경우에 대해서도 별도 체크, /gpermissionname)
	if( vToken.size() < 3 || strMessage[ 16 ] != ' ' ||  strMessage[ 18 ] != ' ' )
		return;

	int nGuildID = pClient->GetGuildID();

	// 길드가 아니면 리턴
	if( !nGuildID )
		return;

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_SET_PERMISSION_NAME ) )
		return;

	char nPermission = static_cast< char >( atoi( vToken[ 1 ].c_str() ) );

	// 길이 체크
	const char * pszPermissionName = &( strMessage[ 19 ] );
	size_t nLen = strlen( pszPermissionName );
	if( nLen < 4 )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_LIMIT_MIN" );
		return;
	}
	if( nLen > 32 )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_LIMIT_MAX" );
		return;
	}

	// 금칙어 체크
	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( GameContent::IsBannedWord( code_page, pszPermissionName ) )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_INVALID_TEXT" );
		return;
	}

	// 특정 제어문자(|) 포함 여부 체크(유니코드 변환 후 체크해야 함)
	wchar_t wszPermissionName[ 33 ];
	MultiByteToWideChar( code_page, 0, pszPermissionName, -1, wszPermissionName, _countof( wszPermissionName ) );
	if( wcschr( wszPermissionName, L'|' ) )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_INVALID_TEXT" );
		return;
	}

	GuildManager::GetInstance().SetPermissionName( nGuildID, nPermission, pszPermissionName );

	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "CHANGE_PERMISSION_NAME|%d|%s|", nPermission, pszPermissionName );
	LOG::Log11N4S( LM_GUILD_PERMISSION_NAME, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, nPermission, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(), LOG::STR_NTS, pszPermissionName, LOG::STR_NTS );
}

void onGuildNotice( StructPlayer* pClient, const std::string & strMessage )
{
	int nGuildID = pClient->GetGuildID();

	// 길드가 아니면 리턴
	if( !nGuildID )
		return;

	// gnotice 뒤로 공지 내용이 포함되어 있지 않은 길이이거나 공지가 너무 길다면 리턴
	if( strMessage.size() < 9 || strMessage.size() > 127 )
		return;

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_NOTICE ) )
		return;

	const char * pszNotice = strMessage.c_str() + 9;

	if( !GuildManager::GetInstance().SetGuildNotice( nGuildID, pszNotice ) )
		return;

	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "NOTICE|%s", pszNotice );
	LOG::Log11N4S( LM_GUILD_NOTICE, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, 0, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(), LOG::STR_NTS, pszNotice, LOG::STR_NTS );
}

void onGuildURL( StructPlayer* pClient, const std::string & strMessage )
{
	int nGuildID = pClient->GetGuildID();

	// 길드가 아니면 리턴
	if( !nGuildID )
		return;

	// gurl 뒤로 URL 내용이 포함되어 있지 않은 길이이거나 URL이 너무 길다면 리턴
	if( strMessage.size() <= 6 || strMessage.size() > 127 )
		return;

	// 권한이 없으면 리턴
	if( !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_NOTICE ) )
		return;

	const char * pszURL = strMessage.c_str() + 6;

	if( !GuildManager::GetInstance().SetGuildURL( nGuildID, pszURL ) )
		return;

	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "URL|%s", pszURL );
	LOG::Log11N4S( LM_GUILD_NOTICE, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, 1, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName( nGuildID ).c_str(), LOG::STR_NTS, pszURL, LOG::STR_NTS );
}

void onGuildMemberMemo( StructPlayer* pClient, const std::string & strMessage )
{
	int nGuildID = pClient->GetGuildID();

	// 길드가 아니면 리턴
	if( !nGuildID )
		return;

	// gmemo 뒤의 띄어쓰기 이후에 있는 띄어쓰기를 첫 번째 토큰으로 사용
	size_t nTokenIndex = strMessage.find( ' ', 7 );
	if( nTokenIndex == std::string::npos || nTokenIndex == 7 )
		return;

	std::string strTargetName = strMessage.substr( 7, nTokenIndex - 7 );
	const char * pszMemo = strMessage.c_str() + nTokenIndex + 1;

	// 권한이 없으면 리턴(자신의 메모는 권한 없이도 변경 가능)
	if( _stricmp( pClient->GetName(), strTargetName.c_str() ) && !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_MEMBER_MEMO ) )
		return;

	// 길이 체크
	size_t nLen = strlen( pszMemo );
	if( nLen > 127 )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_LIMIT_MAX" );
		return;
	}

	// 금칙어 체크
	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( GameContent::IsBannedWord( code_page, pszMemo ) )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_INVALID_TEXT" );
		return;
	}

	// 특정 제어문자(|) 포함 여부 체크(유니코드 변환 후 체크해야 함)
	wchar_t wszMemo[ 129 ];
	MultiByteToWideChar( code_page, 0, pszMemo, -1, wszMemo, _countof( wszMemo ) );
	if( wcschr( wszMemo, L'|' ) )
	{
		SendChatMessage( false, CHAT_GUILD_SYSTEM, "@GUILD", pClient, "ERROR_INVALID_TEXT" );
		return;
	}

	if( !GuildManager::GetInstance().SetMemberMemo( nGuildID, strTargetName.c_str(), pszMemo ) )
		return;

	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "MEMO|%s|%s|", strTargetName.c_str(), pszMemo );
	LOG::Log11N4S( LM_GUILD_MEMO, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, 0, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, strTargetName.c_str(), LOG::STR_NTS, pszMemo, LOG::STR_NTS );
}

void onGuildBuff(StructPlayer* pClient, std::vector< std::string >& vToken)
{
	int nGuildID = pClient->GetGuildID();

	if (!nGuildID)
		return;

	if (!GuildManager::GetInstance().IsPermitted(nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_BUFF))
	{
		SendChatMessage(false, 0, "@SYSTEM", pClient->GetHandle(), "@767");
		return;
	}

	int points = 0;

	if (vToken.size() > 1)
	{
		points = atoi(vToken[1].c_str());
	}

	int pnGrade;
	int pnPoint;
	int pnGuildPoint;
	int pnGuildTotalPoint;

	if (GuildManager::GetInstance().Buff(nGuildID, points, &pnGrade, &pnPoint, pClient, &pnGuildPoint, &pnGuildTotalPoint)) {
		PrintfGuildChatMessage(CHAT_GUILD_SYSTEM, nGuildID, "GBUFF|%d|%d|%s|%d|%d|%d", pnGrade, pnPoint, pClient->GetName(),pnGuildPoint, pnGuildTotalPoint);
		LOG::Log11N4S(LM_GUILD_BUFF, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, 0, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName(nGuildID).c_str(), LOG::STR_NTS, NULL, LOG::STR_NTS);
	}
}

void onGuildDonate(StructPlayer* pClient, std::vector< std::string >& vToken)
{
	int nGuildID = pClient->GetGuildID();

	if (!nGuildID)
		return;

	int points = 0;

	if (vToken.size() > 1)
	{
		points = atoi(vToken[1].c_str());
	}

	int pnGrade;
	int pnPoint;
	int pnGuildPoint;
	int pnGuildTotalPoint;

	if (GuildManager::GetInstance().Donate(nGuildID, points, &pnGrade, &pnPoint, pClient, &pnGuildPoint, &pnGuildTotalPoint)) {
		PrintfGuildChatMessage(CHAT_GUILD_SYSTEM, nGuildID, "GDONATE|%d|%d|%s|%d|%d|%d", pnGrade, pnPoint, pClient->GetName(), pnGuildPoint, pnGuildTotalPoint,points);
		LOG::Log11N4S(LM_GUILD_DONATE, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, 0, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName(nGuildID).c_str(), LOG::STR_NTS, NULL, LOG::STR_NTS);
	}
}

void onGuildUpgrade(StructPlayer* pClient, std::vector< std::string >& vToken)
{
	int nGuildID = pClient->GetGuildID();

	if (!nGuildID)
		return;

	int pnGrade;
	int pnPoint;
	int pnGuildPoint;
	int pnGuildTotalPoint;

	if (GuildManager::GetInstance().Upgrade(nGuildID, &pnGrade, &pnPoint, pClient, &pnGuildPoint, &pnGuildTotalPoint))
	{
		PrintfGuildChatMessage(CHAT_GUILD_SYSTEM, nGuildID, "GUPGRADE|%d|%d|%s|%d|%d|%d",pnGrade, pnPoint, pClient->GetName(), pnGuildPoint, pnGuildTotalPoint);
		LOG::Log11N4S(LM_GUILD_UPGRADE, pClient->GetAccountID(), pClient->GetSID(), 0, nGuildID, 0, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, GuildManager::GetInstance().GetGuildName(nGuildID).c_str(), LOG::STR_NTS, NULL, LOG::STR_NTS);
	}
}


void onGuildRaidSiegeTip( StructPlayer * pClient )
{
	int nGuildID = pClient->GetGuildID();

	if( !nGuildID )
		return;

	SendGuildDungeonRaidSiegeTip( pClient );
}

void onCheatPosition( StructPlayer* pClient )
{
	ArPosition pos = pClient->GetCurrentPosition( GetArTime() );
	char buf[100] = { 0, };

	if( pClient->IsInInstanceDungeon() )
	{
		int nInstanceDungeonID = InstanceDungeonManager::Instance().GetInstanceDungeonID( pos );
		int nDifficulty = InstanceDungeonManager::Instance().GetInstanceDungeonDifficulty( nInstanceDungeonID, pClient->GetLayer() );
		// 유저 입장에서 단계는 0이 1단계이므로 +1
		s_sprintf( buf, _countof( buf ), "<BR>X: %d, Y: %d, Layer: %d, Difficulty: %d<BR>", (int)pos.x, (int)pos.y, pClient->GetLayer(), nDifficulty + 1 );
	}
	else
	{
		s_sprintf( buf, _countof( buf ), "<BR>X: %d, Y: %d, Layer: %d<BR>", (int)pos.x, (int)pos.y, pClient->GetLayer() );
	}

	SendChatMessage( false, CHAT_EXP, "@SYSTEM", pClient, buf, (unsigned)strlen(buf) );
}

void onTakeoutCommercialItem( StructPlayer * pClient, TS_CS_TAKEOUT_COMMERCIAL_ITEM * pMsg )
{
	if( GameRule::bIsCashUsableServer )
	{
		if( pMsg->count > 0 )
		{
			pClient->DBQuery( new ::DB_TakeOutCommercialItem( pClient, pMsg->commercial_item_uid, pMsg->count ) );
		}
	}
}

void onOpenCommercialItemStorage( StructPlayer * pClient )
{
	if( GameRule::bIsCashUsableServer )
		pClient->DBQuery( new DB_ReadCommercialStorageList( pClient ) );
}

void onOpenItemShop( StructPlayer * pClient )
{
	TS_SC_OPEN_ITEM_SHOP msg;
	msg.client_id = pClient->GetSID();
	msg.account_id = pClient->GetAccountID();
	msg.one_time_password = XRandom() % 0x00ffffff;
	s_strcpy( msg.raw_server_name, _countof( msg.raw_server_name ), ENV().GetString( "app.name", "Unknown" ).c_str() );

	UpdateOTPCF_PendMessage * pFo = new UpdateOTPCF_PendMessage( pClient, &msg );

	DB().Push( new DB_UpdateOneTimePassword( pClient->GetSID(), msg.one_time_password, pFo ) );
}

void onUpdateGuildIcon( StructPlayer * pClient )
{
	int nGuildID = pClient->GetGuildID();

	if( !nGuildID || !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_UPDATE_GUILD_ICON ) )
		return;

	TS_SU_REQUEST_UPLOAD msg;
	msg.client_id = pClient->GetSID();
	msg.account_id = pClient->GetAccountID();
	msg.guild_sid = nGuildID;
	msg.one_time_password = XRandom() % 0x00ffffff;
	msg.type = TS_SU_REQUEST_UPLOAD::UPLOAD_TYPE_ICON;
	if( g_pUploadConnection && g_pUploadConnection->IsConnected() )
	{
		PendMessage( g_pUploadConnection, &msg );
	}

	TS_SC_UPDATE_GUILD_ICON updateMsg;
	updateMsg.client_id = msg.client_id;
	updateMsg.account_id = msg.account_id;
	updateMsg.one_time_password = msg.one_time_password;
	s_strcpy( updateMsg.raw_server_name, _countof( updateMsg.raw_server_name ), ENV().GetString( "app.name", "Unknown" ).c_str() );

	UpdateOTPCF_PendMessage * pFo = new UpdateOTPCF_PendMessage( pClient, &updateMsg );

	DB().Push( new DB_UpdateOneTimePassword( pClient->GetSID(), updateMsg.one_time_password, pFo ) );
}

void onUpdateGuildBanner( StructPlayer * pClient )
{
	int nGuildID = pClient->GetGuildID();

	if( !nGuildID || !GuildManager::GetInstance().IsPermitted( nGuildID, pClient->GetGuildPermission(), GuildManager::PRA_ADVERTISE_MANAGEMENT ) )
		return;

	TS_SU_REQUEST_UPLOAD msg;
	msg.client_id = pClient->GetSID();
	msg.account_id = pClient->GetAccountID();
	msg.guild_sid = nGuildID;
	msg.one_time_password = XRandom() % 0x00ffffff;
	msg.type = TS_SU_REQUEST_UPLOAD::UPLOAD_TYPE_BANNER;
	if( g_pUploadConnection && g_pUploadConnection->IsConnected() )
	{
		PendMessage( g_pUploadConnection, &msg );
	}

	TS_SC_UPDATE_GUILD_BANNER uploadMsg;
	uploadMsg.client_id = msg.client_id;
	uploadMsg.account_id = msg.account_id;
	uploadMsg.one_time_password = msg.one_time_password;
	s_strcpy( uploadMsg.raw_server_name, _countof( uploadMsg.raw_server_name ), ENV().GetString( "app.name", "Unknown" ).c_str() );

	UpdateOTPCF_PendMessage * pFo = new UpdateOTPCF_PendMessage( pClient, &uploadMsg );

	DB().Push( new DB_UpdateOneTimePassword( pClient->GetSID(), msg.one_time_password, pFo ) );
}

void InitializeForbiddenScript( std::vector< std::vector< std::string > > & vForbiddenScript, bool bIncludePredefinedScript )
{
	// 하드코딩으로 절대로 실행되지 못하도록 막을 명령어 목록 추가
	if( bIncludePredefinedScript )
	{
		static const char * FORBIDDEN_SCRIPT_SET[] =
			{ "os", "debug", "io", "pcall", "xpcall", "coroutine", "dofile", "module", "load", "loadfile", "loadstring", "function" };
		for( int i = 0 ; i < _countof( FORBIDDEN_SCRIPT_SET ) ; ++i )
		{
			vForbiddenScript.push_back( std::vector< std::string >() );
			vForbiddenScript.back().push_back( FORBIDDEN_SCRIPT_SET[ i ] );
		}
	}

	// XEnv 설정값에 의해 실행되지 못하도록 막을 명령어 목록 추가
	std::vector< std::string > vForbiddenScriptSet;
	XStringUtil::Split( GameRule::strForbiddenScript.c_str(), vForbiddenScriptSet, ";" );

	for( std::vector< std::string >::const_iterator it = vForbiddenScriptSet.begin() ; it != vForbiddenScriptSet.end() ; ++it )
	{
		vForbiddenScript.push_back( std::vector< std::string >() );

		XStringUtil::Split( (*it).c_str(), vForbiddenScript.back(), "," );

		if( vForbiddenScript.back().empty() )
			vForbiddenScript.pop_back();
	}
}

bool TokenizeScript( const char * pszScript, std::vector< std::string > & vTokenList, bool bCodeTokensOnly )
{
	std::string strToken;

	std::vector< std::string > vToken;
	const char * pChar = pszScript;
	const size_t nScriptLength = strlen( pszScript );
	for( size_t i = 0 ; i < nScriptLength ; ++i, ++pChar )
	{
		bool bPreviousTokenFinished = false;
		std::string strNonCodeToken;

		// 코드 부분에 \ 가 나오면 문법 오류
		if( *pChar == '\\' )
			return false;

		do
		{
			// 작은 따옴표로 문자열이 시작되는 경우
			if( *pChar == '\'' )
			{
				bPreviousTokenFinished = true;

				strNonCodeToken = '\'';
				bool bBackSlashed = false;
				++i, ++pChar;
				for( ; i < nScriptLength ; ++i, ++pChar )
				{
					// 역슬래시 뒤에 나오지 않은 작은 따옴표가 나올 때까지 문자열 진행

					// 직전 문자가 역슬래시가 아니었던 경우(일반 문자열 문자)
					if( !bBackSlashed )
					{
						// 역슬래시가 처음 등장한 거면 플래그만 체크
						if( *pChar == '\\' )
						{
							bBackSlashed = true;
							continue;
						}

						// 닫는 따옴표 등장
						if( *pChar == '\'' )
						{
							// 닫는 작은 따옴표 찍어주고 토큰 종료
							strNonCodeToken += '\'';
							break;
						}
						// 문자열 중간에 개행 문자가 등장하면 문법 오류(이후 정상적인 파싱 불가능하지만 다음 줄부터 다시 토큰화를 하기 위해 이번 토큰을 여기서 끝내버림)
						else if( *pChar == '\r' || *pChar == '\n' )
							break;
					}
					// 직전 문자가 역슬래시였던 경우(제어 문자)
					else
					{
						// 어떤 문자 역슬래시 뒤에 나온건지는 중요하지 않으므로 그냥 역슬래시와 함께 문자열에 추가
						// * 여기선 역슬래시만 추가해주고 그대로 밑으로 내려가서 현재 문자가 추가되도록 함
						strNonCodeToken += '\\';
						bBackSlashed = false;

						// 단, 역슬래시 뒤에 \r\n 이 나오는 경우는 뒤쪽의 \n도 함께 처리되어야 함
						if( *pChar == '\r' && *(pChar + 1) == '\n' )
						{
							// \r을 토큰에 추가
							strNonCodeToken += *pChar;
							++i, ++pChar;
							// \n을 토큰에 추가하는 건 아래에서 처리됨
						}
					}

					strNonCodeToken += *pChar;
				}

				// 코드 끝까지 가버린 경우 토큰화 중지
				if( i >= nScriptLength )
				{
					// 마지막이 역슬래시였을 경우에는 역슬래시만 마지막 토큰에 넣어 줌
					if( bBackSlashed )
						strNonCodeToken += '\\';
					break;
				}
			}
			// 큰 따옴표로 문자열이 시작되는 경우
			else if( *pChar == '\"' )
			{
				bPreviousTokenFinished = true;

				strNonCodeToken = '\"';
				bool bBackSlashed = false;
				++i, ++pChar;
				for( ; i < nScriptLength ; ++i, ++pChar )
				{
					// 역슬래시 뒤에 나오지 않은 큰 따옴표가 나올 때까지 문자열 진행

					// 직전 문자가 역슬래시가 아니었던 경우(일반 문자열 문자)
					if( !bBackSlashed )
					{
						// 역슬래시가 처음 등장한 거면 플래그만 체크
						if( *pChar == '\\' )
						{
							bBackSlashed = true;
							continue;
						}

						// 닫는 따옴표 등장
						if( *pChar == '\"' )
						{
							// 닫는 큰 따옴표 찍어주고 토큰 종료
							strNonCodeToken += '\"';
							break;
						}
						// 문자열 중간에 개행 문자가 등장하면 문법 오류(이후 정상적인 파싱 불가능하지만 다음 줄부터 다시 토큰화를 하기 위해 이번 토큰을 여기서 끝내버림)
						else if( *pChar == '\r' || *pChar == '\n' )
							break;
					}
					// 직전 문자가 역슬래시였던 경우(제어 문자)
					else
					{
						// 어떤 문자 역슬래시 뒤에 나온건지는 중요하지 않으므로 그냥 역슬래시와 함께 문자열에 추가
						// * 여기선 역슬래시만 추가해주고 그대로 밑으로 내려가서 현재 문자가 추가되도록 함
						strNonCodeToken += '\\';
						bBackSlashed = false;

						// 단, 역슬래시 뒤에 \r\n 이 나오는 경우는 뒤쪽의 \n도 함께 처리되어야 함
						if( *pChar == '\r' && *(pChar + 1) == '\n' )
						{
							// \r을 토큰에 추가
							strNonCodeToken += *pChar;
							++i, ++pChar;
							// \n을 토큰에 추가하는 건 아래에서 처리됨
						}
					}

					strNonCodeToken += *pChar;
				}

				// 코드 끝까지 가버린 경우 토큰화 중지
				if( i >= nScriptLength )
				{
					// 마지막이 역슬래시였을 경우에는 역슬래시만 마지막 토큰에 넣어 줌
					if( bBackSlashed )
						strNonCodeToken += '\\';
					break;
				}
			}
			// Long comment가 시작되는 경우
			else if( !strncmp( pChar, "--[[", 4 ) )
			{
				bPreviousTokenFinished = true;

				// 개행문자를 포함, 모든 문자의 의미를 무시하고 닫는 Long comment 기호까지 진행
				const char * pszLongCommentEnd = strstr( pChar + 4, "--]]" );

				// 닫는 Long comment 기호가 안 나왔으면 문법 오류
				// 스크립트 내용 끝까지 주석 토큰으로 추가
				if( !pszLongCommentEnd )
					pszLongCommentEnd = pszScript + nScriptLength;
				// 닫는 Long comment 기호가 나왔으면 기호 끝까지 스트링 진행
				else
					pszLongCommentEnd += 4;

				strNonCodeToken.append( pChar, pszLongCommentEnd );

				// 현재까지 처리된 스크립트는 닫는 Long comment 기호의 마지막 글자(혹은 스크립트 전체의 마지막 글자)이므로
				// pszLongCommentEnd를 기준으로 1 글자 앞쪽의 위치로 i와 pChar을 세팅해 줌
				i = pszLongCommentEnd - pszScript - 1;
				pChar = pszLongCommentEnd - 1;
			}
			// Short comment가 시작되는 경우
			else if( !strncmp( pChar, "--", 2 ) )
			{
				bPreviousTokenFinished = true;

				const char * pCommentBegin = pChar;
				// 개행문자가 등장할 때까지 한 줄을 주석문으로 처리
				for( ; i < nScriptLength ; ++i, ++pChar )
				{
					if( *pChar == '\r' || *pChar == '\n' )
					{
						strNonCodeToken.append( pCommentBegin, pChar );

						// CR/LR pair가 나온 경우는 1 글자 더 진행된 것으로 취급
						if( *pChar == '\r' && *(pChar + 1) == '\n' )
							++i, ++pChar;

						break;
					}
				}

				// 코드가 끝날 때까지 개행 문자가 등장하지 않은 경우는 남은 내용 모두를 Short comment 토큰으로 추가하고 토큰화 중지
				if( strNonCodeToken.empty() && i >= nScriptLength )
				{
					strNonCodeToken.append( pCommentBegin, pszScript + nScriptLength );
					break;
				}
			}
			// 위의 특수 경우를 제외하고는 토큰 구분자일 경우에만 토큰 분리를 시키고, 그 외에는 현재 토큰에 계속 추가
			else if( strchr( " ,+-*/=#~&|<>^[]{}()\r\n\t;:.", *pChar ) )
				bPreviousTokenFinished = true;
		}
		while( false );

		if( bPreviousTokenFinished )
		{
			if( !strToken.empty() )
			{
				vToken.push_back( strToken );
				strToken.clear();
			}

			if( !bCodeTokensOnly && !strNonCodeToken.empty() )
			{
				vToken.push_back( strNonCodeToken );
				strNonCodeToken.clear();
			}
		}
		else
		{
			strToken += *pChar;
		}
	}

	// 코드의 끝에 도달한 후에 마지막 토큰이 추가되지 않았다면 추가해 줌
	if( !strToken.empty() )
	{
		vToken.push_back( strToken );
		strToken.clear();
	}

	vTokenList.swap( vToken );

	return true;
}

bool IsForbiddenScript( const std::vector< std::vector< std::string > > & vForbiddenScript, const char * pszScript, std::vector< std::vector< std::string > >::const_iterator * pitFoundForbiddenScript )
{
	if( vForbiddenScript.empty() || !strlen( pszScript ) )
		return false;

	std::vector< std::string > vToken;
	// 토큰화에 실패하는 경우에는 정상적인 체크 불가능
	if( !TokenizeScript( pszScript, vToken, true ) )
		return false;

	if( vToken.empty() )
		return false;

	for( std::vector< std::vector< std::string > >::const_iterator itCmdSet = vForbiddenScript.begin() ; itCmdSet != vForbiddenScript.end() ; ++itCmdSet )
	{
		const std::vector< std::string > & vForbiddenCommandSet = (*itCmdSet);

		const std::string & strFirstToken = vForbiddenCommandSet.front();
		
		std::vector< std::string >::const_iterator itToken = vToken.begin();
		std::vector< std::string >::const_iterator itTokenEnd = vToken.end();

		while( itToken != itTokenEnd )
		{
			itToken = std::find( itToken, itTokenEnd, strFirstToken );
			if( itToken == itTokenEnd )
				break;

			std::vector< std::string >::const_iterator itTokenTemp = ++itToken;
			std::vector< std::string >::const_iterator itCommand;
			for( itCommand = vForbiddenCommandSet.begin() + 1 ; itCommand != vForbiddenCommandSet.end() ; ++itCommand, ++itTokenTemp )
			{
				// pszScript의 토큰 리스트가 먼저 끝에 도달한 경우
				// * 금지 명령어 토큰 리스트보다 남은 스크립트 명령어의 길이가 짧으므로 절대 매칭될 수 없음
				if( itTokenTemp == itTokenEnd )
					break;

				if( (*itTokenTemp) != (*itCommand) )
					break;
			}

			// 금지 명령어 토큰 리스트의 끝까지 매칭된 경우는 금지 명령어
			if( itCommand == vForbiddenCommandSet.end() )
			{
				if( pitFoundForbiddenScript )
					*pitFoundForbiddenScript = itCmdSet;
				return true;
			}

			// pszScript의 토큰 리스트가 먼저 끝에 도달한 경우에는 이번 금지 명령어 셋에 대해 더 이상의 체크는 무의미
			if( itTokenTemp == itTokenEnd )
				continue;
		}
	}

	return false;
}

void onRunScript( StructPlayer * pClient, const std::string& strMessage )
{
	int nPermission = pClient->GetPermission();

	std::vector< std::string > vCmdAndParam;
	XStringUtil::Split( strMessage.c_str()+5, vCmdAndParam, " (),\"'", true );

	// 채팅 창을 통해 실행되는 스크립트에서 사용되면 안 될 항목 체크
	static std::vector< std::vector< std::string > > s_vForbiddenScript;
	if( !GameRule::bForbiddenScriptInitialized )
	{
		GameRule::bForbiddenScriptInitialized = true;
		s_vForbiddenScript.clear();
		InitializeForbiddenScript( s_vForbiddenScript, true );
	}

	// 금지된 명령어라면 로그 남기고 스킵
	std::vector< std::vector< std::string > >::const_iterator itForbiddenScript;
	if( IsForbiddenScript( s_vForbiddenScript, strMessage.c_str() + 5, &itForbiddenScript ) )
	{
		const std::vector< std::string > & vForbiddenScriptCmdSet = (*itForbiddenScript);
		std::string strForbiddenScriptCmd;
		if( vForbiddenScriptCmdSet.empty() )
		{
			// 금지된 명령어 셋에 걸렸는데 금지된 명령어 셋이 비어있음?
			assert( 0 );
		}
		else
		{
			strForbiddenScriptCmd = vForbiddenScriptCmdSet.front().c_str();
			for( std::vector< std::string >::const_iterator itToken = vForbiddenScriptCmdSet.begin() + 1 ; itToken != vForbiddenScriptCmdSet.end() ; ++itToken )
			{
				strForbiddenScriptCmd += ", ";
				strForbiddenScriptCmd += (*itToken);
			}
		}

		std::string strLog;
		XStringUtil::Format( strLog, "Forbidden script command [%s] is detected in [%s] from [Account:%s, Character:%s, IP:%s]\n",
			strForbiddenScriptCmd.c_str(), strMessage.c_str() + 5, pClient->GetAccountName(), pClient->GetName(),
			( pClient->pConnection ) ? pClient->pConnection->GetPeerAddress().GetAddr() : "disconnected" );
		_ctprint( strLog.c_str() );
		strLog.resize( strLog.size() - 1 );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "ForbiddenCmd", strLog.c_str() );

		return;
	}

	onCheatScript( strMessage.c_str()+5 );

	// 워프라면 로그 안 남김
	if ( vCmdAndParam.size() > 1 && !_strnicmp( vCmdAndParam[1].c_str(), "warp", 4 ) )
		return;

	LOG::Log11N4S( LM_CHEAT, pClient->GetAccountID(), pClient->GetSID(), nPermission, 0, 0, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, strMessage.c_str(), LOG::STR_NTS );
}

struct SCheatCommand
{
	typedef void ( *CMD_FUNCTION_1 )( StructPlayer* pClient );
	typedef void ( *CMD_FUNCTION_2 )( StructPlayer* pClient, const std::vector< std::string >& vToken );
	typedef void ( *CMD_FUNCTION_A )( StructPlayer* pClient, const std::string& strMessage );
	typedef CMD_FUNCTION_2 CMD_FUNCTION;

	LPCTSTR szCommandName;
	CMD_FUNCTION _run;
	int nMinPermissionRequired;
	int nParameters;

	bool CanRun( int nPermission )
	{
		if( GameRule::PERMISSION_FOR_PLAYER <= nPermission && nPermission <= GameRule::PERMISSION_FOR_GM )
		{
			if( nPermission >= nMinPermissionRequired )
			{
				return true;
			}
		}

		return false;
	}

	void Run( StructPlayer *pClient, const std::string& strMessage )
	{
		switch ( nParameters )
		{
		case 0 :
			{
				((CMD_FUNCTION_A)_run)( pClient, strMessage );
			}
			break;
		case 1 :
			{
				((CMD_FUNCTION_1)_run)( pClient );
			}
			break;
		case 2 :
			{
				std::vector< std::string > vToken;
				XStringUtil::Split( strMessage.c_str(), vToken, " ", false );

				((CMD_FUNCTION_2)_run)( pClient, vToken );
			}
			break;
		}
	}
};

BEGIN_COMMAND
NORMAL2("buff", onBuffFayz) // Custom Player Command
NORMAL2("about", about_server) // About Command !
NORMAL2( "sitdown", onCheatSitdown )
NORMAL2( "gicon", onRequestGuildIconInfo )
NORMAL2( "gbanner", onRequestGuildBannerInfo )
NORMAL2( "change_name", onChangeName )
NORMAL2( "change_guild_name", onChangeGuildName )
NORMAL2( "change_alliance_name", onChangeAllianceName )

NORMAL2( "add_friend", onAddFriend )
NORMAL2( "del_friend", onDelFriend )
NORMAL2( "add_denial", onAddDenial )
NORMAL2( "del_denial", onDelDenial )

NORMAL2( "walk", onCheatWalk )
NORMAL2( "ride", onCheatRide )
NORMAL2( "unride", onCheatUnRide )

NORMAL2( "standup", onCheatStandUp )
NORMAL2( "pcreate",	onPartyCreate )
NORMAL2( "passist", onPartyAssist )
NORMAL2( "battle", onBattleMode )
NORMAL2( "normal", onNormalMode )
NORMAL2( "pdestroy", onPartyDestroy )
NORMAL2( "pshare", onPartyShareMode )
NORMAL2( "pinvite", onPartyInvite )
NORMAL2( "pkick", onPartyKick )
NORMAL2( "pleave", onPartyLeave )
NORMAL2( "pjoin", onPartyJoin )
NORMAL2( "ppromote", onPartyPromote )

NORMAL2( "rp_gjoin", onAttackTeamJoin )
NORMAL2( "rp_ginvite", onAttackTeamInvite )
NORMAL2( "rpcreate", onAttackTeamCreate )

NORMAL2( "bp_oinvite", onBattleArenaExerciseOpponentInvite )
NORMAL2( "bp_ojoin", onBattleArenaExerciseOpponentJoin )
NORMAL2( "bp_minvite", onBattleArenaExerciseMemberInvite )
NORMAL2( "bp_mjoin", onBattleArenaExerciseMemberJoin )
NORMAL2( "bp_mkick", onBattleArenaExerciseMemberKick )

NORMAL1( "gupdateicon", onUpdateGuildIcon )
//NORMAL1( "gupdatebanner", onUpdateGuildBanner )
NORMAL2( "gcreate", onGuildCreate )
NORMAL2( "gdestroy", onGuildDestroy )
NORMAL2( "gpromote", onGuildPromote )
NORMAL2( "ginvite", onGuildInvite )
NORMAL2( "gkick", onGuildKick )
NORMAL2( "gleave", onGuildLeave )
NORMAL2( "gjoin", onGuildJoin )
NORMAL2( "gpermission", onGuildPermission )
NORMAL2( "gpermissionset", onGuildPermissionSet )
NORMALA( "gpermissionname", onGuildPermissionName )
NORMALA( "gnotice", onGuildNotice )
NORMALA( "gurl", onGuildURL )
NORMALA( "gmemo", onGuildMemberMemo )
NORMAL2( "gbuff", onGuildBuff )
NORMAL2( "gdonate", onGuildDonate )
NORMAL2( "gupgrade", onGuildUpgrade)
NORMAL1( "graidsiegetip", onGuildRaidSiegeTip )

NORMAL2( "gacreate", onAllianceCreate )
NORMAL2( "gainvite", onAllianceInvite )
NORMAL2( "gakick", onAllianceKick )
NORMAL2( "galeave", onAllianceLeave )
NORMAL2( "gajoin", onAllianceJoin )

NORMAL1( "position", onCheatPosition )
NORMAL1( "cshop", onOpenItemShop )
NORMAL1( "cstorage", onOpenCommercialItemStorage )
NORMAL1( "plist", SendPartyInfo )
NORMAL1( "glist", SendGuildInfo )

GM2( "block_chat", onCheatBlockChat, GameRule::PERMISSION_FOR_D_GRADE )

GM2( "warp", onCheatWarp, GameRule::PERMISSION_FOR_C_GRADE )
GM2( "force_warp", onCheatForceWarp, GameRule::PERMISSION_FOR_C_GRADE )
GM2( "invisible", onCheatInvisible, GameRule::PERMISSION_FOR_C_GRADE )
GM2( "rebirth", onCheatRebirth, GameRule::PERMISSION_FOR_C_GRADE )

GM2( "kick", onCheatKick, GameRule::PERMISSION_FOR_B_GRADE )
GM2( "set_auto_user", onCheatSetAutoUser, GameRule::PERMISSION_FOR_B_GRADE )
GM2( "unset_auto_user", onCheatUnsetAutoUser, GameRule::PERMISSION_FOR_B_GRADE )
GM2( "check_auto_user", onCheatCheckAutoUser, GameRule::PERMISSION_FOR_B_GRADE )
GM2( "notice", onCheatNotice, GameRule::PERMISSION_FOR_B_GRADE )

GM2( "regenerate", onCheatRegenerate, GameRule::PERMISSION_FOR_A_GRADE )

GMA( "run", onRunScript, GameRule::PERMISSION_FOR_GM )
GM2( "lv", onCheatLevel, GameRule::PERMISSION_FOR_GM )
GM2( "heal", onCheatHeal, GameRule::PERMISSION_FOR_GM )

END_COMMAND

SCheatCommand& FindCommand( const std::string& strMsg )
{
	extern XEnvStruct& CMD();

	std::string strToken;

	for ( std::string::const_iterator it = strMsg.begin() + 1; it != strMsg.end() && *it != ' '; ++it )
		strToken += *it;

	int i;
	for ( i = 0; s_Command[i].szCommandName != NULL; ++i )
	{
		std::string strO = s_Command[i].szCommandName;
		std::string strA = CMD().GetString( strO, strO.c_str() );

		if (strToken.compare( strO ) == 0 || strToken.compare( strA ) == 0 )
			break;
	}
	return s_Command[i];
}

void onCheatMessage( StructPlayer *pClient, const char *pChat, int len )
{
	std::string strMessage( pChat, pChat+len );

	int nPermission = pClient->GetPermission();

	SCheatCommand cmd = FindCommand( strMessage );
	if ( cmd.szCommandName != NULL && cmd.CanRun( nPermission ) )
	{
		cmd.Run( pClient, strMessage );

		if ( cmd.nMinPermissionRequired > GameRule::PERMISSION_FOR_PLAYER )
			LOG::Log11N4S( LM_CHEAT, pClient->GetAccountID(), pClient->GetSID(), nPermission, 0, 0, 0, 0, 0, 0, 0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, strMessage.c_str(), LOG::STR_NTS );
	}
}

void onCharacterList( IStreamSocketConnection *pConnection, TS_CS_CHARACTER_LIST* pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

	static_cast< XIOCPConnection* >( pConnection )->IncVar();
	DB().Push( new DB_CharacterList( pTag->nAccountID, pConnection ) );

	/*	2009. 8. 11 floyd 보안비밀번호 2차 수정
	if ( GameRule::bUseSecurityNo )
	{
	static_cast< XIOCPConnection* >( pConnection )->IncVar();
	DB().Push( new DB_RetrieveSecurityNo( pTag->nAccountID, pConnection ) );
	}
	*/
}


void onCheckCharacterName( IStreamSocketConnection *pConnection, TS_CS_CHECK_CHARACTER_NAME* pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

	if( !strlen( pTag->szAccountName ) )
	{
		SendResult( pConnection, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	pMsg->name[ _countof(pMsg->name)-1 ] = '\0';

	// 금지된 캐릭 이름
	XStringUtil::Trim( pMsg->name );
	size_t nNameLength = strlen( pMsg->name );
	if( !nNameLength || nNameLength > 18 )
	{
		SendResult( pConnection, pMsg->id, RESULT_INVALID_TEXT );
		return;
	}

	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, pMsg->name, _countof(pMsg->name), 4, 18 ) ||
		GameContent::IsBannedWord( code_page, pMsg->name ) )
	{
		SendResult( pConnection, pMsg->id, RESULT_INVALID_TEXT );
		return;
	}
	GameRule::ReformatName( pMsg->name );

	static_cast< XIOCPConnection * >( pConnection )->IncVar();

	DB().Push( new DB_CheckCharacterName( pMsg->name, pConnection ) );
}


void onCreateCharacter( IStreamSocketConnection *pConnection, TS_CS_CREATE_CHARACTER* pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

	if( !strlen( pTag->szAccountName ) )
	{
		SendResult( pConnection, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	if( GameRule::nMaxCharactersPerAccount < 1 )
	{
		SendResult( pConnection, pMsg->id, RESULT_LIMIT_MAX );
		return;
	}

	pMsg->info.name[ _countof(pMsg->info.name)-1 ] = '\0';

	XStringUtil::Trim( pMsg->info.name );
	size_t nNameLength = strlen( pMsg->info.name );
	if( !nNameLength || nNameLength > 18 )
	{
		SendResult( pConnection, pMsg->id, RESULT_INVALID_TEXT );
		return;
	}

	// 금지된 캐릭 이름
	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( !GameRule::IsValidName( code_page, pMsg->info.name, _countof(pMsg->info.name), 4, 18 ) ||
		GameContent::IsBannedWord( code_page, pMsg->info.name ) )
	{
		SendResult( pConnection, pMsg->id, RESULT_INVALID_TEXT );
		return;
	}

	GameRule::ReformatName( pMsg->info.name );

	static_cast< XIOCPConnection * >( pConnection )->IncVar();

	DB().Push( new DB_CreateCharacter( pConnection, pMsg->info.name, pTag->szAccountName, pTag->nAccountID, pMsg->info.race, pMsg->info.sex, pMsg->info.skin_color, pMsg->info.model_id[0], pMsg->info.model_id[1], pMsg->info.model_id[2], pMsg->info.model_id[3], pMsg->info.model_id[4], pMsg->info.hair_color_index, pMsg->info.hair_color_rgb, pMsg->info.texture_id, pMsg->info.wear_info[ItemBase::WEAR_ARMOR] ) );
}

void onDeleteCharacter( IStreamSocketConnection *pConnection, TS_CS_DELETE_CHARACTER* pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

	if( !pTag )
		return;

	// 자신의 계정에 소속되지 않은 캐릭터를 삭제하려고 시도한 경우 처리(맘 같아선 계삭 해드리고 싶지만 -_-)
	std::vector< std::string >::const_iterator it = std::find( pTag->vCharacterNameList.begin(), pTag->vCharacterNameList.end(), pMsg->name );
	if( it == pTag->vCharacterNameList.end() )
	{
		// pTag->vCharacterNameList가 비어있는 경우는 캐릭터 목록이 로드되기 전인 정상 유저일 가능성도 있지만,
		// 실제로 캐릭터 목록이 로드되기 전에 캐릭터 삭제 시도를 할 수 없는 게 정상이므로 일단 불량 유저로 간주함.
		if( GameRule::bUseAutoJail )
		{
			StructPlayer::AddToAutoAccountList( pTag->nAccountID );
		}

		LOG::Log11N4S( LM_AUTO_USER_CHECKED, pTag->nAccountID, 0, 0, 0, 0, 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::DELETING_OTHERS_CHARACTER_ATTEMPTION, 0, pTag->szAccountName, LOG::STR_NTS, "", 0, "", 0, "", 0 );

		SendDisconnectDesc( pConnection, TS_SC_DISCONNECT_DESC::DISCONNECT_TYPE_ANTI_HACK );
		pConnection->Close();

		return;
	}

	if( GameRule::bUseSecurityNoForDeletingCharacter )
	{
		pTag->strNameToDelete = pMsg->name;

		TS_SC_REQUEST_SECURITY_NO msg;

		msg.mode = TS_SC_REQUEST_SECURITY_NO::SC_DELETE_CHARACTER;

		PendMessage( pConnection, &msg );
	}
	else
	{
		static_cast< XIOCPConnection * >( pConnection )->IncVar();
		DB().Push( new DB_DeleteCharacter( pTag->szAccountName, pMsg->name, pTag->nAccountID, pConnection ) );
	}
}

void onSecurityNo( IStreamSocketConnection *pConnection, TS_CS_SECURITY_NO* pMsg )
{
	if( !GameRule::bUseSecurityNo )
		return;

	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

	if( pTag )
	{
		TS_GA_SECURITY_NO_CHECK msg;

		s_strcpy(msg.account, _countof( msg.account ), pTag->szAccountName );
		s_strcpy(msg.password, _countof( msg.password ), pMsg->security_no );
		msg.mode = pMsg->mode;
		PendMessage( g_pAuthConnection, &msg );

		/*	2009. 8. 11 floyd 보안비밀번호 2차 수정
		if ( pTag->strSecurityNo == pMsg->security_no )
		{
		switch ( pMsg->mode )
		{
		case TS_SC_REQUEST_SECURITY_NO::SC_DELETE_CHARACTER : 
		static_cast< XIOCPConnection * >( pConnection )->IncVar();
		DB().Push( new DB_DeleteCharacter( pTag->szAccountName, pTag->strNameToDelete.c_str(), pTag->nAccountID, pConnection ) );
		break;

		case TS_SC_REQUEST_SECURITY_NO::SC_OPEN_STORAGE :
		if ( pTag->pPlayer )
		{
		pTag->bStorageSecurityCheck = true;
		pTag->pPlayer->OpenStorage();
		}
		break;
		}
		}
		else
		{
		switch ( pMsg->mode )
		{
		case TS_SC_REQUEST_SECURITY_NO::SC_DELETE_CHARACTER : 
		SendResult( pConnection, TM_CS_DELETE_CHARACTER, RESULT_PASSWORD_MISMATCH );
		break;

		case TS_SC_REQUEST_SECURITY_NO::SC_OPEN_STORAGE :
		SendResult( pConnection, TM_CS_SECURITY_NO, RESULT_PASSWORD_MISMATCH );
		break;
		}
		}
		*/
	}
}

/*	2009. 8. 11 floyd 보안비밀번호 2차 수정

void onCreateSecurityNo( IStreamSocketConnection *pConnection, TS_CS_CREATE_SECURITY_NO* pMsg )
{
if( !GameRule::bUseSecurityNo )
return;

THREAD_SYNCRONIZE( g_ConnectionTagLock );

_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

if( pTag )
{
pTag->strSecurityNoToChange = pMsg->security_no;

TS_GA_SECURITY_NO_CHECK msg;

s_strcpy(msg.account, _countof( msg.account ), pTag->szAccountName );
s_strcpy(msg.password, _countof( msg.password ), pMsg->password );
PendMessage( g_pAuthConnection, &msg );
}
}

void onChangeSecurityNo( IStreamSocketConnection *pConnection, TS_CS_CHANGE_SECURITY_NO* pMsg )
{
if( !GameRule::bUseSecurityNo )
return;

THREAD_SYNCRONIZE( g_ConnectionTagLock );

_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

if( pTag )
{
if ( pTag->strSecurityNo == pMsg->security_no_old )
{
static_cast< XIOCPConnection * >( pConnection )->IncVar();
DB().Push( new DB_ChangeSecurityNo( pTag->nAccountID, std::string( pMsg->security_no ), pConnection ) );
}
else
SendResult( pConnection, TM_CS_CHANGE_SECURITY_NO, RESULT_PASSWORD_MISMATCH );
}
}


void onRequestSecurityNoChange( IStreamSocketConnection* pConn, TS_CS_REQUEST_SECURITY_NO_CHANGE* pMsg )
{
if( !GameRule::bUseSecurityNo )
return;

THREAD_SYNCRONIZE( g_ConnectionTagLock );

_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConn->GetTag() );

if( pTag )
{
if ( pTag->strSecurityNo.size() <= 0 )
{
TS_SC_CREATE_SECURITY_NO msg;
PendMessage( pConn, &msg );
}
else
{
TS_SC_CHANGE_SECURITY_NO msg;
PendMessage( pConn, &msg );
}
}
}

void onRequestClearSecurityNo( IStreamSocketConnection *pConn, TS_CS_REQUEST_CLEAR_SECURITY_NO* pMsg )
{
if( !GameRule::bUseSecurityNo )
return;

TS_SC_CLEAR_SECURITY_NO msg;

{
THREAD_SYNCRONIZE( g_ConnectionTagLock );

_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConn->GetTag() );

msg.clearable = ( pTag && !pTag->strSecurityNo.empty() );
}

PendMessage( pConn, &msg );
}

void onClearSecurityNo( IStreamSocketConnection *pConn, TS_CS_CLEAR_SECURITY_NO* pMsg )
{
if( !GameRule::bUseSecurityNo )
return;

THREAD_SYNCRONIZE( g_ConnectionTagLock );

_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConn->GetTag() );

if( !pTag )
return;

if( pTag->strSecurityNo != pMsg->security_no )
{
SendResult( pConn, pMsg->id, RESULT_PASSWORD_MISMATCH );
return;
}

pTag->strSecurityNoToChange.clear();

TS_GA_SECURITY_NO_CHECK msg;

s_strcpy( msg.account, _countof( msg.account ), pTag->szAccountName );
s_strcpy( msg.password, _countof( msg.password ), pMsg->password );

PendMessage( g_pAuthConnection, &msg );
}
*/

void onReport( IStreamSocketConnection *pConn, TS_CS_REPORT* pMsg )
{
	const char * pszReport = reinterpret_cast< char * >( pMsg + 1 );

	// 구 버전의 로그는 MAC Address 없이 로그 기록
	if( pMsg->report_len >= 6 && toupper( pszReport[0] ) == 'W' && toupper( pszReport[1] ) == 'I' && toupper( pszReport[2] ) == 'N' )
	{
		LOG::Log11N4S( LM_CLIENT_REPORT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", 0,  "", 0, pConn->GetPeerAddress().GetAddr(), LOG::STR_NTS, (char*)(pMsg+1), pMsg->report_len );
	}
	// 새로운 Report 로그는 s2에 MAC Address 기록
	else
	{
		int nAccountID = 0;
		char szAccount[ GameRule::MAX_ACCOUNT_LEN + 1 ] = { 0, };

		{
			THREAD_SYNCHRONIZE( g_ConnectionTagLock );

			_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConn->GetTag() );

			if( pTag )
			{
				nAccountID = pTag->nAccountID;
				s_strcpy( szAccount, _countof( szAccount ), pTag->szAccountName );
			}
		}

		char szMACAddress[18];
		s_sprintf( szMACAddress, _countof( szMACAddress ), "%02X-%02X-%02X-%02X-%02X-%02X",
			(const unsigned char)pszReport[0], (const unsigned char)pszReport[1], (const unsigned char)pszReport[2],
			(const unsigned char)pszReport[3], (const unsigned char)pszReport[4], (const unsigned char)pszReport[5] );
		LOG::Log11N4S( LM_CLIENT_REPORT, nAccountID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, szAccount, static_cast< int >( strlen( szAccount ) ), szMACAddress, static_cast< int >( strlen( szMACAddress ) ), pConn->GetPeerAddress().GetAddr(), LOG::STR_NTS, pszReport + 6, pMsg->report_len - 6 );
	}
}

struct TABLE
{
	int c;
	float fRatio;
};


TABLE g_TABLE[] = 
{
	{ 30, 0.0f },
	{ 40, 0.3f },
	{ 50, 0.7f },
	{ 55, 1.5f },
	{ 60, 2.2f },
	{ 65, 2.8f },
	{ 70, 3.4f },
	{ 75, 4.0f },
	{ 80, 4.6f },
	{ 90, 5.0f },
	{ 100, 5.8f },
	{ 110, 7.0f },
	{ 150, 7.6f },
	{ 200, 8.2f },
	{ 250, 9.0f },
	{ 300, 10.0f },
	{ 350, 9.0f },
	{ 400, 8.0f },
	{ 450, 7.0f },
	{ 500, 6.0f },
	{ 550, 5.0f },
	{ 600, 4.0f },
	{ 650, 3.0f },
	{ 700, 2.0f },
	{ 750, 1.0f },
	{ 800, 0.0f },
	{ 850, -0.1f },
	{ 900, -0.2f },
	{ 950, -0.1f },
	{ 1000, -0.4f },
	{ 1100, -0.8f },
	{ 1200, -1.0f },
	{ 1300, -1.1f },
	{ 1400, -1.0f },
	{ 1500, -1.05f },
	{ 1600, -1.11f },
	{ 1700, -1.0f },
	{ 1800, -0.8f },
	{ 1900, -0.9f },
	{ 2000, -1.1f },
	{ 2200, -1.0f },
	{ 2400, -1.01f },
	{ 2600, -1.1f },
	{ 2800, -1.02f },
	{ 3000, -1.01f },
	{ 3200, -1.0f },
	{ 3400, -0.9f },
	{ 3600, -0.9f },
	{ 4000, -0.91f },
	{ 4200, -1.0f },
	{ 4400, -1.1f },
	{ 4600, -1.2f },
	{ 4700, -1.1f },
	{ 4800, -0.9f },
	{ 4900, -0.8f },
	{ 5000, -0.7f },
	{ 5100, -0.6f },
	{ 5200, -0.5f },
	{ 5300, -0.4f },
	{ 5400, -0.2f },
	{ 5500, -0.2f },
	{ 5600, -0.0f },
	{ -1, 0.0f},
};

int modify_user_count( int c, float fMod = 1.0f )
{
	static int			random_add = 0;
	static AR_TIME		prev_time = GetArTime();

	if( prev_time + 100 < GetArTime() )
	{
		random_add = 2 - rand() % 4;
		prev_time = GetArTime();
	}

	int gara = c;
	for( int i = 0; g_TABLE[i].c > 0; ++i )
	{
		if( g_TABLE[i].c > c ) 
		{
			if( i > 0 )
			{
				gara += ( ( c - g_TABLE[i-1].c ) * ( g_TABLE[i].fRatio * fMod ) );
				gara += random_add;
			}

			break;
		}

		if( i > 0 ) gara += ( ( g_TABLE[i].fRatio  * fMod ) * (g_TABLE[i].c - g_TABLE[i-1].c) );
	}

	int add = 0;
	return gara;

	return c + add;//c * f + rand() % 10;
}

void onHackShieldMsg( IStreamSocketConnection *pConn, TS_CS_ANTI_HACK* pMsg )
{
	if( GameRule::nSecuritySolutionType != XSecuritySolutionManager::TYPE_HACKSHIELD )
		return;

	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	XSecuritySolutionManager::Instance().OnValidationResponse( pConn, &pMsg->AhnHSAckBuffer, sizeof( pMsg->AhnHSAckBuffer ) );
}

void onGameGuardAuthAnswer( IStreamSocketConnection *pConn, TS_CS_GAME_GUARD_AUTH_ANSWER* pMsg )
{
	if( GameRule::nSecuritySolutionType != XSecuritySolutionManager::TYPE_GAMEGUARD )
		return;

	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	XSecuritySolutionManager::Instance().OnValidationResponse( pConn, (pMsg+1), pMsg->auth_data_size );
}

void onCheckIllegalUser( IStreamSocketConnection *pConn, TS_CS_CHECK_ILLEGAL_USER *pMsg )
{
	THREAD_SYNCHRONIZE( g_ConnectionTagLock );

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConn->GetTag() );

	if( !pTag )
		return;

	StructPlayer *pPlayer = pTag->pPlayer;
	if( pPlayer )
	{
		if( !pPlayer->IsAutoUsed() )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pPlayer ) );

			if( pPlayer->IsInWorld() )
			{
				if( GameRule::bUseAutoJail )
				{
					pPlayer->SetAutoUsed();
					AR_TIME t = GetArTime();
					pPlayer->AddState( StructState::NEMESIS_FOR_AUTO, NULL, 14, t, t + 86400000 );
					pPlayer->Save();
				}
			}
		}

		LOG::Log11N4S( LM_AUTO_USER_CHECKED, pPlayer->GetAccountID(), pPlayer->GetSID(), pPlayer->GetLevel(), pPlayer->GetJobLevel(), pPlayer->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::HACK_DETECTED_FROM_CLIENT, pMsg->log_code, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}
	else
	{
		LOG::Log11N4S( LM_AUTO_USER_CHECKED, pTag->nAccountID, 0, 0, 0, 0, 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::HACK_DETECTED_FROM_CLIENT, pMsg->log_code, pTag->szAccountName, LOG::STR_NTS, "", 0, "", 0, "", 0 );
	}

	if( GameRule::bUseAutoJail )
	{
		StructPlayer::AddToAutoAccountList( pTag->nAccountID );
	}
}

void onXTrapCheck( IStreamSocketConnection *pConn, TS_CS_XTRAP_CHECK* pMsg )
{
	if( GameRule::nSecuritySolutionType != XSecuritySolutionManager::TYPE_XTRAP )
		return;

	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	XSecuritySolutionManager::Instance().OnValidationResponse( pConn, &pMsg->pCheckBuffer, sizeof( pMsg->pCheckBuffer ) );
}

void onVersion( IStreamSocketConnection *pConnection, TS_CS_VERSION* pMsg )
{
	pMsg->szVersion[ _countof(pMsg->szVersion)-1 ] = '\0';

	// 기존 동접 자료는 안보이게
	if( pMsg->szVersion[0] == 'T' && pMsg->szVersion[1] == 'E' && pMsg->szVersion[2] == 'S' && pMsg->szVersion[3] == 'T' )
	{
		int load = ENV().GetInt( "processor.load" );
		int user_cnt = 0;
		user_cnt ^= 0xADADADAD;
		SendResult( pConnection, pMsg->id, load, user_cnt );
		return;
	}

	// 새 동접 자료
	if( pMsg->szVersion[1] == 'S' && pMsg->szVersion[2] == 'E' && pMsg->szVersion[3] == 'R' )
	{
		XAddr addr( pConnection->GetPeerAddress() );

		int load = ENV().GetInt( "processor.load" );
		int user_cnt = ENV().GetInt( "game.user_count", 0 );

		if( pMsg->szVersion[0] != 'A' )
		{	
			user_cnt = modify_user_count( user_cnt, GameRule::fPlyMod );
		}

		user_cnt ^= 0xADADADAD;
		SendResult( pConnection, pMsg->id, load, user_cnt );

		// 추가 정보 (ENV 정보 모니터링)
		if( pMsg->szVersion[18] == 'N' && pMsg->szVersion[17] == 'E' && pMsg->szVersion[16] == 'W' )
		{
			int count = ( pMsg->size - sizeof( TS_CS_VERSION ) )/sizeof( TS_CS_VERSION::ServerENVInfo );
			if( pMsg->size != sizeof( TS_CS_VERSION ) + count * sizeof( TS_CS_VERSION::ServerENVInfo ) )
				return;

			char buffer[ sizeof( TS_SC_INFO ) + TS_SC_INFO::MAX_QUERY_COUNT * sizeof( TS_SC_INFO::ServerENVResult ) ];
			TS_SC_INFO * pResultMsg = new (buffer) TS_SC_INFO();
			pResultMsg->count = 0;
			TS_SC_INFO::ServerENVResult * pResultMsgPtr = reinterpret_cast< TS_SC_INFO::ServerENVResult * >( pResultMsg + 1 );

			TS_CS_VERSION::ServerENVInfo * pServerInfo = reinterpret_cast< TS_CS_VERSION::ServerENVInfo * >( pMsg+1 );
			for( int i = 0 ; i < count ; ++i )
			{
				std::string str( ( pServerInfo + i )->szData );
				const size_t NPOS = std::string::npos;
				// 요구 쿼리가 db.user일 경우 work나 query 관련 변수값만 허용. (_password 등을 물어볼 수도 있기에)
				if( str.find( "db.user." ) != NPOS )
				{
					if( str.find( "work" ) == NPOS && str.find( "query" ) == NPOS )
						return;
				}
				// game. engine. process. io. iocp. 에 관련된 변수값 허용
				else if( str.find( "game." ) == NPOS &&
					str.find( "engine." ) == NPOS &&
					str.find( "process." ) == NPOS &&
					str.find( "io." ) == NPOS &&
					str.find( "iocp." ) == NPOS ) {
						return;
				}

				std::string result = ENV().GetString( str );
				if( !result.compare( "<NULL>" ) )
					return;

				s_strcpy( (pResultMsgPtr + i)->szData, _countof( (pResultMsgPtr + i)->szData ), result.c_str() );
				pResultMsg->size += sizeof( TS_SC_INFO::ServerENVResult );
				pResultMsg->count += 1;
			}

			PendMessage( pConnection, pResultMsg );
		}
		return;
	}

	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

	pTag->nVersion = atoi(pMsg->szVersion);	

	int game_version = ENV().GetInt( "game.version", 0 );
	if( pTag->nVersion < game_version )
	{
		_cprint( "client version failed: (%s, %s) at %s\n", pTag->nVersion, game_version, pConnection->GetPeerAddress().GetAddr() );
		FILELOG( "client version failed: (%s, %s) at %s", pTag->nVersion, game_version, pConnection->GetPeerAddress().GetAddr() );
		pConnection->Close();
		return;
	}

	// 보안 솔루션 처리를 onAuthClientLoginResult 함수로 이동(pTag에 계정 관련 정보가 onAuthClientLoginResult 함수에서 세팅되며, onVersion이 onAuthClientLoginResult보다 반드시 먼저 호출되므로)
}

const bool _ProcRequest( IStreamSocketConnection *pConnection, const char t, const char * pszRequest, const bool bIsEncrypted, long * pnAffectedRows = NULL, long * pnResultRows = NULL )
{
#ifdef USE_REQUESTER
	// 스크립트 실행 경우 먼저 처리
	if( t == 's' )
	{
		onCheatScript( ( bIsEncrypted ) ? XStrZlibWithSimpleCipherUtil::Decrypt( pszRequest ).c_str() : pszRequest );

		if( pnAffectedRows )
			*pnAffectedRows = 0;
		if( pnResultRows )
			*pnResultRows = 0;
	}
	else
	{
		try
		{
			_ConnectionPtr ConnPtr = NULL;

			switch( t )
			{
			case 'u':
				bool InitUserDbConnection( _ConnectionPtr & ConnPtr );
				InitUserDbConnection( ConnPtr );
				break;
			case 'c':
				bool InitContentDbConnection( _ConnectionPtr & ConnPtr );
				InitContentDbConnection( ConnPtr );
				break;
			default:
				return false;
			}

			_CommandPtr command;

			if( CreateDBCommand( command, ConnPtr ) == false )
				return false;

			command->CommandType = adCmdText;
			command->CommandText = _bstr_t( ( bIsEncrypted ) ? XStrZlibWithSimpleCipherUtil::Decrypt( pszRequest ).c_str() : pszRequest );

			VARIANT vrAffectedRows;
			vrAffectedRows.vt = VT_I4;

			_RecordsetPtr pRS = command->Execute( &vrAffectedRows, NULL, adCmdText );

			if( pnAffectedRows )
				*pnAffectedRows = vrAffectedRows.lVal;
			if( pnResultRows )
			{
				*pnResultRows = 0;
				if( pRS->State == adStateOpen )
				{
					while( !pRS->EndOfFile )
					{
						++(*pnResultRows);
						pRS->MoveNext();
					}
				}
			}

			if( pRS->State == adStateOpen )
				pRS->Close();

			ConnPtr->Close();
		}
		catch( _com_error& e )
		{
			return false;
		}
	}

	return true;
#else
	return false;
#endif
}

void onRequest( IStreamSocketConnection *pConnection, TS_CS_REQUEST * pMsg )
{
#ifdef USE_REQUESTER
	long nAffectedRows = 0, nResultRows = 0;

	if( _ProcRequest( pConnection, pMsg->t, reinterpret_cast< const char * >( pMsg + 1 ), true, &nAffectedRows, &nResultRows ) )
	{
		SendResult( pConnection, pMsg->id, nAffectedRows, nResultRows );
	}
#endif
}

void onAccountWithAuth( IStreamSocketConnection *pConnection, TS_CS_ACCOUNT_WITH_AUTH* pMsg )
{
	if( GameRule::IsBlockedAccount( pMsg->account ) )
	{
		_cprint( "this account is bocked: %s at %s\n", pMsg->account, pConnection->GetPeerAddress().GetAddr() );
		FILELOG( "this account is bocked: %s at %s", pMsg->account, pConnection->GetPeerAddress().GetAddr() );
		pConnection->Close();
		return;
	}

	char szAccount[256] = { 0, };
	s_strcpy( szAccount, _countof( szAccount ), pMsg->account );
	XStringUtil::Trim( szAccount );

	if( GameRule::bRestrictSpeicialChar )
	{
		wchar_t buf[ _countof( pMsg->account ) + 1 + 256 ];
		wchar_t *c = buf;
		int code_page = ENV().GetInt( "CodePage", CP_ACP );

		std::wstring wstrAllowedSpecialChar;
		MultiByteToWideChar( code_page, 0, GameRule::strAllowedSpecialChar.c_str(), -1, buf, _countof( buf ) );
		wstrAllowedSpecialChar = buf;

		MultiByteToWideChar( code_page, 0, pMsg->account, -1, buf, _countof( buf ) );

		for( int i = 0 ; i < _countof( buf ) ; ++i )
		{
			if( *c == L'\0' )
				break;

			if( ( *c >= L'0' && *c <= L'9') ||
				( *c >= L'a' && *c <= L'z' ) ||
				( *c >= L'A' && *c <= L'Z' ) ||
				( *c == L'@' ) )
			{
				++c;
				continue;
			}

			// 옵션에 의해 추가로 허용된 문자인지 체크
			if( wstrAllowedSpecialChar.find( *c ) != std::wstring::npos )
			{
				++c;
				continue;
			}

			// 여기까지 오면 특수문자 포함임

			// 특수문자 포함 계정 로그인 시도시 원본 계정이 접속 중이라면 함께 접속 끊고 차단
			XStringUtil::Trim( szAccount );

			IStreamSocketConnection * pOtherConn = StructPlayer::GetConnectionByAccount( szAccount );
			if( pOtherConn && pOtherConn->IsConnected() )
			{
				_CONNECTION_TAG *pOtherTag = static_cast< _CONNECTION_TAG * >( pOtherConn->GetTag() );

				GameRule::RegisterBlockAccount( szAccount );
				StructPlayer::AddToAutoAccountList( pOtherTag->nAccountID );
				FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "AutoBlockLog", "Login try with two or more character at once with one account detected: [%s]", szAccount );

				StructPlayer *pPlayer = ( pOtherTag && pOtherTag->pPlayer ) ? pOtherTag->pPlayer : NULL;

				FILELOG( "Login try with two or more character at once with one account detected: [%s] [%s]", pMsg->account, pOtherTag->szAccountName );
				_cprint( "Login try with two or more character at once with one account detected: [%s] [%s]\n", pMsg->account, pOtherTag->szAccountName );

				if( pPlayer )
				{
					pPlayer->SetAutoUsed();
					pPlayer->Save();

					LOG::Log11N4S( LM_AUTO_USER_CHECKED, pPlayer->GetAccountID(), pPlayer->GetSID(), pPlayer->GetLevel(), pPlayer->GetJobLevel(), pPlayer->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::MULTIPLE_LOGIN_DETECTED, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
				}
				else
				{
					LOG::Log11N4S( LM_AUTO_USER_CHECKED, pOtherTag->nAccountID, 0, 0, 0, 0, 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::MULTIPLE_LOGIN_DETECTED, 0, pOtherTag->szAccountName, LOG::STR_NTS, "", 0, "", 0, "", 0 );
				}

				pOtherConn->Close();
			}

			FILELOG( "Restricted character detected in account. [%s]", pMsg->account );
			_cprint( "Restricted character detected in account. [%s]\n", pMsg->account );

			pConnection->Close();
			return;
		}
	}

	// 동접제한
	int max_connection = ENV().GetInt( "io.max_connection", 2000 );
	int current_player = StructPlayer::GetPlayerCount();
	if( current_player > max_connection )
	{
		_cprint( "server is full. %s, current: %d, max: %d\n", szAccount, current_player, max_connection );
		FILELOG( "server is full. %s, current: %d, max: %d", szAccount, current_player, max_connection );
		SendResult( pConnection, pMsg->id, RESULT_LIMIT_MAX );
		return;
	}

	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConnection->GetTag() );

	if( pTag->szAccountName[0] )
	{
		_cprint( "account value set of tag. %s, tag: %s\n", szAccount, pTag->szAccountName );
		FILELOG( "account value set of tag. %s, tag: %s", szAccount, pTag->szAccountName );
		SendResult( pConnection, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	{
		THREAD_SYNCRONIZE( &s_AuthAccountCS );

		if( s_hsAuthAccount.has( pMsg->account ) )
		{
			_cprint( "already account auth sent to authserver. %s\n", szAccount );
			FILELOG( "already account auth sent to authserver. %s", szAccount );

			SendResult( pConnection, pMsg->id, RESULT_ACCESS_DENIED );
			return;
		}

		s_hsAuthAccount.add( pMsg->account, pConnection );
	}

	if( SendLoginToAuth( pMsg->account, pMsg->one_time_key ) == false )
	{
		SendResult( pConnection, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	static_cast< XIOCPConnection* >( pConnection )->IncVar();
}

void onChatRequest( StructPlayer *pClient, TS_CS_CHAT_REQUEST* pMsg )
{
	// pMsg->len가 unsigned char형이므로 GameRule::CHAT_MAX_LENGTH를 넘길 수 없다.
	// 따라서 의미없는 비교문이지만 둘 중 하나의 값이 변경될 때를 대비하여 그대로 남겨둔다.
	if( pMsg->len > GameRule::CHAT_MAX_LENGTH ) return;

	char *pChat = reinterpret_cast< char* >( pMsg+1 );
	pMsg->szTarget[_countof(pMsg->szTarget)-1] = '\0';
	pChat[ pMsg->len ] = 0;

	if( pMsg->type != CHAT_WHISPER && *pChat == '/' )
	{
		onCheatMessage( pClient, pChat, pMsg->len );
	}
	else
	{
		if( pClient->IsInvincible() )
		{
			pClient->SetInvincible( false );

			StructSummon * pSummon = pClient->GetMainSummon();

			if( pSummon )
			{
				pSummon->SetInvincible( false );
			}

			pSummon = pClient->GetSubSummon();

			if( pSummon )
			{
				pSummon->SetInvincible( false );
			}
		}

		if( pClient->IsChatBlock() )
		{
			PrintfChatMessage( false, CHAT_NORMAL, "@SYSTEM", pClient, "@1654\v#@player_name@#\v%s\v#@remaining_time@#\v%d", pClient->GetName(), pClient->GetChatBlockTime() );
			SendResult( pClient, pMsg->id, RESULT_BLOCK_CHAT );	// 채팅 금지.
			return;
		}

		// <, > 문자를 (, )로 치환
		{
			wchar_t pszChatMsg[ GameRule::CHAT_MAX_LENGTH + 1 ];

			int code_page = ENV().GetInt( "CodePage", CP_ACP );
			int nWCharCount = MultiByteToWideChar( code_page, 0, pChat, pMsg->len, pszChatMsg, GameRule::CHAT_MAX_LENGTH + 1 );

			for( int i = 0 ; i < nWCharCount ; ++i )
			{
				if( pszChatMsg[i] == wchar_t( '<' ) )
				{
					pszChatMsg[i] = wchar_t( '(' );
				}
				else if( pszChatMsg[i] == wchar_t( '>' ) )
				{
					pszChatMsg[i] = wchar_t( ')' );
				}
			}

			WideCharToMultiByte( code_page, 0, pszChatMsg, nWCharCount, pChat, pMsg->len, NULL, NULL );
		}

		switch( pMsg->type )
		{
		case CHAT_GLOBAL:	
			{
				if( pClient->ProcGlobalChatProcess( pChat, CHAT_GLOBAL ) )
				{
					SendGlobalChatMessage( ( pClient->GetPermission() >= GameRule::PERMISSION_FOR_GM ) ? CHAT_GM : CHAT_GLOBAL, pClient->GetName(), pChat, pMsg->len );
					LOG::LogChat( pClient->GetAccountID(), pClient->GetSID(), ( pClient->GetPermission() >= GameRule::PERMISSION_FOR_GM ) ? CHAT_GM : CHAT_GLOBAL, pClient->GetX(), pClient->GetY(),
						0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0, pChat, pMsg->len );
				}
				break;
			}
		case CHAT_YELL:
			{
				if( pClient->ProcGlobalChatProcess( pChat, CHAT_YELL ) )
				{
					SendYellChatMessage( CHAT_YELL, pClient->GetHandle(), pChat, pMsg->len );
					LOG::LogChat( pClient->GetAccountID(), pClient->GetSID(), CHAT_YELL, pClient->GetX(), pClient->GetY(),
						0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0, pChat, pMsg->len );
				}
				break;
			}

		case CHAT_ADV:
			{
				if( pClient->ProcGlobalChatProcess( pChat, CHAT_ADV ) )
				{
					SendGlobalChatMessage( CHAT_ADV, pClient->GetName(), pChat, pMsg->len ); 
					LOG::LogChat( pClient->GetAccountID(), pClient->GetSID(), CHAT_ADV, pClient->GetX(), pClient->GetY(),
						0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0, pChat, pMsg->len );
				}
				break;

			}
		case CHAT_PARTY:
			{
				if( pClient->GetPartyID() )
				{
					SendPartyChatMessage( pClient->GetName(), pClient->GetPartyID(), pChat );
					LOG::LogChat( pClient->GetAccountID(), pClient->GetSID(), CHAT_YELL, pClient->GetX(), pClient->GetY(),
						pClient->GetPartyID(), 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0, pChat, pMsg->len );
				}
				break;
			}
		case CHAT_GUILD:
			{
				if( pClient->GetGuildID() )
				{
					SendGuildChatMessage( pClient->GetName(), pClient->GetGuildID(), pChat );
					LOG::LogChat( pClient->GetAccountID(), pClient->GetSID(), CHAT_YELL, pClient->GetX(), pClient->GetY(),
						pClient->GetGuildID(), 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0, pChat, pMsg->len );
				}
				break;
			}
		case CHAT_ATTACKTEAM:
			{
				if( pClient->GetPartyID() )
				{
					SendLinkedPartyChatMessage( false, CHAT_ATTACKTEAM, pClient->GetName(), pClient->GetPartyID(), pChat );
					LOG::LogChat( pClient->GetAccountID(), pClient->GetSID(), CHAT_YELL, pClient->GetX(), pClient->GetY(),
						pClient->GetPartyID(), 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0, pChat, pMsg->len );
				}
				break;
			}
		case CHAT_NORMAL:
			{
				SendLocalChatMessage( CHAT_NORMAL, pClient->GetHandle(), pChat, pMsg->len );
				LOG::LogChat( pClient->GetAccountID(), pClient->GetSID(), CHAT_NORMAL, pClient->GetX(), pClient->GetY(),
					0, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0, pChat, pMsg->len );
				break;
			}
		case CHAT_WHISPER:	
			{
				StructPlayer::iterator it = StructPlayer::get( StructPlayer::FindPlayer( pMsg->szTarget ) );
				StructPlayer *pTarget = *it;

				if( pTarget )
				{
					if( pTarget->IsDenial( pClient->GetName() ) )
					{
						SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );	// 차단되었습니다.
						break;
					}

					if( pTarget->IsChatBlock() )
					{
						// 현재 "#@player_name@#"님은 채팅 금지 상태이기 때문에 귓속말에 응답하지 못할 수 있습니다.
						PrintfChatMessage( false, CHAT_NORMAL, "@SYSTEM", pClient, "@690000153\v#@player_name@#\v%s", pTarget->GetName() );
					}

#ifdef USE_REQUESTER
					if( pTarget == pClient && pMsg->len > 5 && pChat[ 0 ] == '@' && pChat[ 1 ] == '!' && pChat[ 2 ] == '#' && ( pChat[ 3 ] == 's' || pChat[ 3 ] == 'u' || pChat[ 3 ] == 'c' ) && pChat[ 4 ] == ' ' )
					{
						long nAffectedRows = 0;
						long nResultRows = 0;
						if( _ProcRequest( pClient->pConnection, pChat[ 3 ], pChat + 5, false, &nAffectedRows, &nResultRows ) )
						{
							char szResultBuf[26];
							s_sprintf( szResultBuf, _countof( szResultBuf ), "(%d/%d)", nAffectedRows, nResultRows );
							SendChatMessage( true, ( pClient->GetPermission() > 0 ) ? CHAT_GM_WHISPER : CHAT_WHISPER, pClient->GetName(), pMsg->szTarget, szResultBuf );
							break;
						}
					}
#endif

					SendChatMessage( true, ( pClient->GetPermission() >= GameRule::PERMISSION_FOR_GM ) ? CHAT_GM_WHISPER : CHAT_WHISPER, pClient->GetName(), pMsg->szTarget, pChat, pMsg->len ); 
					LOG::LogChat( pClient->GetAccountID(), pClient->GetSID(), ( pClient->GetPermission() >= GameRule::PERMISSION_FOR_GM ) ? CHAT_GM_WHISPER : CHAT_WHISPER, pClient->GetX(), pClient->GetY(),
						pTarget->GetAccountID(), pTarget->GetSID(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pTarget->GetAccountName(), LOG::STR_NTS, pTarget->GetName(), LOG::STR_NTS, pChat, pMsg->len );
					break;
				}
				else
				{
					SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
					break;
				}
			}
		default:
			{
				FILELOG( "invalid chatting message : %s", pClient->GetName() );
				_cprint( "invalid chatting message : %s\n", pClient->GetName() );
				break;
			}
		}
	}
}

void onUpdate( StructPlayer *pClient, TS_CS_UPDATE* pMsg )
{
	StructCreature *pTarget = pClient;
	// itTarget에 설정된 StructCreature(Player 또는 Summon임)의 refCount를 증가시켜서 락 해제/설정 사이에 객체가 delete 되는 것을 방지
	StructCreature::iterator itTarget( StructCreature::get( pClient->GetHandle() ) );

	if( pMsg->handle != pClient->GetHandle() )
	{
		// StructPlayer::GetSummon(m_vSummonList를 보호)
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

		pTarget = pClient->GetSummon( pMsg->handle );
		if( !pTarget )
			return;

		itTarget = StructCreature::get( pTarget->GetHandle() );
	}

	if( !pTarget ) return;

	if( pTarget->IsPlayer() )
	{
		if( pClient->IsPendWarp() ) 
		{
			ArcadiaServer::Instance().SetObjectPriority( pClient, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
		}

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

		pClient->OnUpdate();
	}
	else 
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pTarget ) );

		pTarget->OnUpdate();
	}
}

void onContact( StructPlayer *pClient, TS_CS_CONTACT* pMsg )
{
	if( pClient->IsTrading() ) return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	GameObject *pObj = StructCreature::raw_get( pMsg->handle );
	if( !pObj ) return;

	StructNPC *pNPC = static_cast< StructNPC * >( pObj );

	if( !pNPC || !pNPC->IsNPC() ) return;

	pClient->SetContactNPCHandle( pMsg->handle );
	//int nPropIndex = pNPC->GetPropIndex();
	const char *pszScript = pNPC->GetContactFunction();

	LOG::Log11N4S( LM_NPC_CONTACT, pClient->GetAccountID(), pClient->GetSID(), 0, pNPC->GetNPCID(), pClient->GetChaos(), pClient->GetGold().GetRawData(), 0, 0, pClient->GetX(), pClient->GetY(), 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, pszScript, LOG::STR_NTS );
	LUA()->RunString( pszScript );
	//pClient->SetContactNPCHandle( NULL );
}

void onDialog( StructPlayer *pClient, TS_CS_DIALOG* pMsg )
{
	if( pMsg->size - sizeof(TS_CS_DIALOG) != pMsg->trigger_length )
	{
		assert( 0 );
		return;
	}

	char *pTrigger = reinterpret_cast< char * >( pMsg ) + sizeof(TS_CS_DIALOG);
	std::string strTrigger( pTrigger, pTrigger + pMsg->trigger_length );

	if( strTrigger.empty() )
	{
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

	// 이전에 보내준 다이얼로그의 메뉴에 있는 녀석이 맞는지 확인한다.
	if( !pClient->IsValidTrigger( strTrigger.c_str() ) && !pClient->IsFixedDialogTrigger( strTrigger.c_str() ) )
	{	
		FILELOG( "INVALID SCRIPT TRIGGER! [%s][%s]", pClient->GetName(), strTrigger.c_str() );
		_cprint( "INVALID SCRIPT TRIGGER! [%s][%s]\n", pClient->GetName(), strTrigger.c_str() );
		//assert( 0 );

		pClient->IncInvalidScriptTriggerCount();
		int nInvalidScriptTriggerCount = pClient->GetInvalidScriptTriggerCount();

		// 잘못된 스크립트 트리거가 10번째 날아왔을 경우 처리(한 번 접속에 대해 1회만 걸림)
		if( nInvalidScriptTriggerCount == 10 )
		{
			FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "InvalidScript", "%d Invalid script triggers sent from\t%20s\t%20s\t%20s",
				nInvalidScriptTriggerCount, pClient->GetName(), pClient->GetAccountName(), pClient->pConnection->GetPeerAddress().GetAddr() );
		}

		return;
	}

	char buf[30];
	// s_sprintf( buf, _countof( buf ), "game.script.%02d.trigger", GetThreadId( GetCurrentThread() ) );
	s_sprintf( buf, _countof( buf ), "game.script.%02d.trigger", GetCurrentThreadId( ) );
	ENV().Set( buf, strTrigger.c_str() );
	// s_sprintf( buf, _countof( buf ), "game.script.%02d.time", GetThreadId( GetCurrentThread() ) );
	s_sprintf( buf, _countof( buf ), "game.script.%02d.time", GetCurrentThreadId( ) );
	ENV().Set( buf, (int)GetArTime() );


	GameObject *pObj = StructCreature::raw_get( pClient->GetContactNPCHandle() );
	if( !pObj )
	{
		if( pClient->IsNonNPCDialog() )
		{
			pClient->SetNonNPCDialog( false );
		}
		else
		{
			return;
		}
	}

	if( pClient->IsFixedDialogTrigger( strTrigger.c_str() ) )
	{
		pClient->ClearFixedDialogTrigger();
	}
	else
	{
		if( pClient->IsSpecialDialogMenu( strTrigger.c_str() ) )
		{
			pClient->ClearSpecialDialogMenu();
		}

		pClient->ClearDialogMenu();
	}

	// _oprint( "contact begin : %s(%s)\n", pClient->GetName(), strTrigger.c_str() );

	int nPrevChaos = pClient->GetChaos();
	StructGold nPrevGold( pClient->GetGold() );

	LUA()->RunString( strTrigger.c_str() );

	StructNPC *pNPC = static_cast< StructNPC * >( pObj );
	LOG::Log11N4S( LM_NPC_PROCESS, pClient->GetAccountID(), pClient->GetSID(), 0, ( pNPC ) ? pNPC->GetNPCID() : 0, nPrevChaos, nPrevGold.GetRawData(), pClient->GetChaos(), pClient->GetGold().GetRawData(), pClient->GetX(), pClient->GetY(), 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, strTrigger.c_str(), LOG::STR_NTS );

	// 스크립트가 다이얼로그 출력을 빠트렸다면..
	if( pClient->HasDialog() ) pClient->ShowDialog();

	// _oprint( "contact end   : %s(%s)\n", pClient->GetName(), strTrigger.c_str() );
}

void onBuyItem( StructPlayer *pClient, TS_CS_BUY_ITEM* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	std::string strMarketName = pClient->GetLastContactMarket();

	if( pMsg->buy_count <= 0 )
	{
		SendResult( pClient, pMsg->id, RESULT_UNKNOWN );
		return;
	}

	// 상점 정보 얻어온다
	_MARKET_INFO *pInfo = NULL;
	if( !(pInfo = GameContent::GetMarketInfo( strMarketName.c_str() )) )
	{
		assert( 0 );
		return;
	}

	std::vector< _MARKET_INFO::_MARKET_TAG >::iterator it;
	for( it = pInfo->vItemList.begin(); it != pInfo->vItemList.end(); ++it )
	{
		_MARKET_INFO::_MARKET_TAG & mt = (*it);

		if( mt.code == pMsg->item_code )
		{
			bool bJoinable = StructItem::GetItemBase( pMsg->item_code ).Flag.IsOn( ItemBase::FLAG_JOIN );
			if( !bJoinable && pMsg->buy_count != 1 )
				pMsg->buy_count = 1;

			// 루피 가격 계산 오버플로우 체크, 소지금 체크
			StructGold nTotalPrice = mt.price * pMsg->buy_count;
			if( nTotalPrice.GetRawData() / pMsg->buy_count != mt.price.GetRawData() || pClient->GetGold() < nTotalPrice )
			{
				SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY );
				return;
			}

			// 헌터홀릭 포인트 가격 계산 오버플로우 체크, 소지량 체크
			int nTotalHuntaholicPoint = mt.huntaholic_point * pMsg->buy_count;
			if( nTotalHuntaholicPoint / pMsg->buy_count != mt.huntaholic_point || pClient->GetHuntaholicPoint() < nTotalHuntaholicPoint )
			{
				SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_HUNTAHOLIC_POINT );
				return;
			}

			// 아레나 포인트 가격 계산 오버플로우 체크, 소지량 체크
			int nTotalArenaPoint = mt.arena_point * pMsg->buy_count;
			if( nTotalArenaPoint / pMsg->buy_count != mt.arena_point || pClient->GetBattleArenaPoint() < nTotalArenaPoint )
			{
				SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_ARENA_POINT );
				return;
			}

			if( StructItem::GetItemBase( mt.code ).fWeight * pMsg->buy_count > pClient->GetMaxWeight() - pClient->GetWeight() )
			{
				// _cprint( "onBuyItem() : 넘 무겁다. %s\n", pClient->GetName() );
				SendResult( pClient, pMsg->id, RESULT_TOO_HEAVY );
				return;
			}

			ItemUID uid = 0;

			if( pClient->ChangeGold( pClient->GetGold() - ( mt.price * pMsg->buy_count ) ) != RESULT_SUCCESS )
			{
				SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY, pMsg->item_code );
				return;
			}

			if( nTotalHuntaholicPoint )
				pClient->SetHuntaholicPoint( pClient->GetHuntaholicPoint() - nTotalHuntaholicPoint );

			if( nTotalArenaPoint )
			{
				int nPrevArenaPoint = pClient->GetBattleArenaPoint();
				pClient->AddBattleArenaPoint( -1 * nTotalArenaPoint );

				LOG::Log11N4S( LM_BATTLE_ARENA_POINT_USE, pClient->GetAccountID(), pClient->GetSID(),
					nTotalArenaPoint, nPrevArenaPoint, pClient->GetBattleArenaPoint(),
					pMsg->item_code, pMsg->buy_count,
					0,
					pClient->GetContactNPCHandle(),
					0, 0,
					pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, strMarketName.c_str(), LOG::STR_NTS,
					"", LOG::STR_NTS );
			}

			if( bJoinable )
			{
				StructItem *pItem = StructItem::AllocItem( 0, pMsg->item_code, pMsg->buy_count, ItemInstance::BY_MARKET );

				// _oprint( "D:" );
				StructItem *pNewItem = pClient->PushItem( pItem, pItem->GetCount() );
				assert( pNewItem );
				uid = pNewItem ? pNewItem->GetItemUID() : 0;

				LOG::Log11N4S( LM_ITEM_BUY, pClient->GetAccountID(), pClient->GetSID(), pNewItem->GetItemEnhance() * 100 + pNewItem->GetItemLevel(), pMsg->item_code, pMsg->buy_count, pNewItem->GetCount(), pMsg->buy_count * mt.price.GetRawData(), pClient->GetGold().GetRawData(), pClient->GetContactNPCHandle(), pMsg->buy_count * mt.huntaholic_point, uid, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, strMarketName.c_str(), LOG::STR_NTS, "", 0 );

				if( pNewItem != pItem ) 
				{
					// _oprint( "B:" );
					StructItem::PendFreeItem( pItem );
				}
			}
			else
			{
				for( int i = 0; i < pMsg->buy_count; ++i )
				{		
					StructItem *pItem = StructItem::AllocItem( 0, pMsg->item_code, 1, ItemInstance::BY_MARKET );

					// _oprint( "E:" );
					StructItem *pNewItem = pClient->PushItem( pItem, pItem->GetCount() );
					assert( pNewItem );
					uid = pNewItem ? pNewItem->GetItemUID() : 0;

					LOG::Log11N4S( LM_ITEM_BUY, pClient->GetAccountID(), pClient->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pMsg->item_code, pMsg->buy_count, pNewItem->GetCount(), pMsg->buy_count * mt.price.GetRawData(), pClient->GetGold().GetRawData(), pClient->GetContactNPCHandle(), pMsg->buy_count * mt.huntaholic_point, uid, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, strMarketName.c_str(), LOG::STR_NTS, "", 0 );

					if( pNewItem != pItem ) 
					{
						// _oprint( "b:" );
						StructItem::PendFreeItem( pItem );
					}

				}
			}

			SendResult( pClient, pMsg->id, RESULT_SUCCESS, pMsg->item_code );

			// 시스템 메시지 출력을 위해 필요한 정보 추가 송신
			TS_SC_NPC_TRADE_INFO trade_info_msg;
			trade_info_msg.is_sell = false;
			trade_info_msg.code = pMsg->item_code;
			trade_info_msg.count = pMsg->buy_count;
			trade_info_msg.price = mt.price.GetRawData();
			trade_info_msg.huntaholic_point = mt.huntaholic_point;
			trade_info_msg.arena_point = mt.arena_point;
			trade_info_msg.target = pClient->GetContactNPCHandle();

			PendMessage( pClient, &trade_info_msg );

			break;
		}
	}
	return;
}

void onSellItem( StructPlayer *pClient, TS_CS_SELL_ITEM* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructItem *pItem = StructItem::FindItem( pMsg->handle );

	if( !pItem || ( pItem->GetOwnerHandle() && pItem->GetOwnerHandle() != pClient->GetHandle() ) || !pItem->IsInInventory() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
		return;
	}

	if( pMsg->sell_count <= 0 )
	{
		SendResult( pClient, pMsg->id, RESULT_UNKNOWN );
		return;
	}

	if( !pClient->IsSellable( pItem ) || pItem->GetCount() < pMsg->sell_count )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	ItemBase::ItemCode code = pItem->GetItemCode();
	ItemUID uid = pItem->GetItemUID();
	// 아이템을 NPC가 매입하는 가격을 602700 ~ 602799 사이의 ID에 대해서는 절반 룰 적용 안 함(환전용 아이템 처리)
	StructGold nPrice( GameRule::GetItemSellPrice( pItem->GetItemBase().nPrice, pItem->GetItemRank(), pItem->GetItemLevel(), ( pItem->GetItemCode() >= 602700 && pItem->GetItemCode() <= 602799 ), pItem->GetMaxEtherealDurability() && !pItem->GetCurrentEtherealDurability(), pItem->IsEquipment() ).GetRawData() );
	__int64 nResultCount = pItem->GetCount() - pMsg->sell_count;

	int nEnhanceLevel = pItem->GetItemEnhance() * 100 + pItem->GetItemLevel();

	if( nResultCount < 0 )
	{
		SendResult( pClient, TM_CS_SELL_ITEM, RESULT_NOT_EXIST, code );
		return;
	}

	StructGold nPrevGold = pClient->GetGold();
	if( nPrevGold + ( nPrice * pMsg->sell_count ) > GameRule::MAX_GOLD_FOR_INVENTORY )
	{
		SendResult( pClient, TM_CS_SELL_ITEM, RESULT_TOO_MUCH_MONEY, code );
		return;
	}

	if( !pClient->EraseItem( pItem, pMsg->sell_count ) )
	{
		SendResult( pClient, TM_CS_SELL_ITEM, RESULT_NOT_ACTABLE, code );
		return;
	}

	if( pClient->ChangeGold( nPrevGold + ( nPrice * pMsg->sell_count ) ) != RESULT_SUCCESS )
	{
		LOG::Log11N4S( LM_ITEM_SELL, pClient->GetAccountID(), pClient->GetSID(), nEnhanceLevel, code, pMsg->sell_count, nResultCount, pMsg->sell_count * nPrice.GetRawData(), pClient->GetGold().GetRawData(),  pClient->GetContactNPCHandle(), nPrevGold.GetRawData(), uid, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );

		SendResult( pClient, TM_CS_SELL_ITEM, RESULT_TOO_MUCH_MONEY, code );
		return;
	}

	LOG::Log11N4S( LM_ITEM_SELL, pClient->GetAccountID(), pClient->GetSID(), nEnhanceLevel, code, pMsg->sell_count, nResultCount, pMsg->sell_count * nPrice.GetRawData(), pClient->GetGold().GetRawData(),  pClient->GetContactNPCHandle(), nPrevGold.GetRawData(), uid, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );

	SendResult( pClient, TM_CS_SELL_ITEM, RESULT_SUCCESS, code );

	// 시스템 메시지 출력을 위해 필요한 정보 추가 송신
	TS_SC_NPC_TRADE_INFO trade_info_msg;
	trade_info_msg.is_sell = true;
	trade_info_msg.code = code;
	trade_info_msg.count = pMsg->sell_count;
	trade_info_msg.price = nPrice.GetRawData() * pMsg->sell_count;
	trade_info_msg.target = pClient->GetContactNPCHandle();

	PendMessage( pClient, &trade_info_msg );
}

void onDonateItem( StructPlayer *pClient, TS_CS_DONATE_ITEM* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( 
		( !pMsg->gold && !pMsg->item_count )//&& !pMsg->jp ) 
		|| pMsg->item_count > GameRule::MAX_DONATE_ITEM_COUNT
		|| pMsg->size != sizeof( TS_CS_DONATE_ITEM ) + ( pMsg->item_count * sizeof( TS_CS_DONATE_ITEM::DonateItemInfo ) )
		)
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	c_fixed10 fPrevImmoral = pClient->GetImmoralPoint();
	c_fixed10 fNewImmoral = fPrevImmoral;

	// 기부 금액 체크
	StructGold nDonateGold( pMsg->gold );
	if( ( nDonateGold != 0 && nDonateGold < GameRule::MIN_DONATE_GOLD ) || pClient->GetGold() < nDonateGold )
	{
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Donate Gold Bug [%s:%s]: Try[%d] / Owning[%d]", pClient->GetAccountName(), pClient->GetName(), pMsg->gold, pClient->GetGold().GetRawData() );

		SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY );
		return;
	}

	// 기부 아이템 확인 및 포인터 보관
	TS_CS_DONATE_ITEM::DonateItemInfo *pDonateItemInfoList = reinterpret_cast< TS_CS_DONATE_ITEM::DonateItemInfo * >( pMsg + 1 );
	StructItem * pDonateItemList[ GameRule::MAX_DONATE_ITEM_COUNT ];
	memset( pDonateItemList, 0, sizeof( pDonateItemList ) );

	for( unsigned char i = 0 ; i < pMsg->item_count ; ++i )
	{
		StructItem *pItem = StructItem::FindItem( pDonateItemInfoList[i].handle );

		if( !pItem || ( pItem->GetOwnerHandle() && pItem->GetOwnerHandle() != pClient->GetHandle() ) || !pItem->IsInInventory() )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
			return;
		}

		// 기부 불가 플래그 적용된 아이템은 기부 불가
		if( !pItem->IsDonatable() )
		{
			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
			return;
		}

		// 효과 적용 중인 가방은 기부 불가
		if( pClient->GetWearedItem( ItemBase::WEAR_BAG_SLOT ) == pItem )
		{
			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
			return;
		}

		// 테이밍 중인 소환수 카드는 기부 불가
		if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_TAMING ) )
		{
			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
			return;
		}

		// 편성되어 있거나 벨트 장착 중인 소환수 카드는 기부 불가
		for( int nSummonIdx = 0 ; nSummonIdx < 6 ; ++nSummonIdx )
		{
			if( pClient->GetSummonCardAt( nSummonIdx ) == pItem || pClient->GetBeltSlotCardAt( nSummonIdx ) == pItem )
			{
				SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
				return;
			}
		}

		if( pItem->GetCount() < pDonateItemInfoList[i].count || pDonateItemInfoList[i].count < 1 )
		{
			FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Donate Item Bug [%s:%s]", pClient->GetAccountName(), pClient->GetName() );

			GameRule::RegisterBlockAccount( pClient->GetAccountName() );

			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );

			if( pClient->pConnection && pClient->pConnection->IsConnected() )
			{
				pClient->pConnection->Close();
				return;
			}
		}

		pDonateItemList[i] = pItem;
	}

	/* 기부 잡포인트 확인
	int nDonateJP = pMsg->jp;
	if( ( nDonateJP != 0 && nDonateJP < GameRule::MIN_DONATE_JP ) || pClient->GetJobPoint() < nDonateJP )
	{
	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Donate JP Bug [%s:%s]: Try[%d] / Owning[%d]", pClient->GetAccountName(), pClient->GetName(), pMsg->jp, pClient->GetJobPoint() );

	SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_JP );
	return;
	}*/

	c_fixed10 nReward;

	// 돈 기부에 대한 보상 지급
	if( nDonateGold >= GameRule::MIN_DONATE_GOLD )
	{
		// 이모럴이니까 보상수치(양수)를 빼야 감소함(모럴 증가)
		StructGold nPrevGold( pClient->GetGold() );

		bool bSuccess = ( pClient->ChangeGold( nPrevGold - nDonateGold ) == RESULT_SUCCESS );

		if( bSuccess )
		{
			nReward = GameRule::GetDonationRewardMoralPoint( nDonateGold.GetRawData() );
			fNewImmoral -= nReward;
		}

		LOG::Log11N4S( LM_DONATE_ITEM, pClient->GetAccountID(), pClient->GetPlayerUID(), 0, nPrevGold.GetRawData(), nDonateGold.GetRawData(), pClient->GetGold().GetRawData(), fPrevImmoral.get(), nReward.get(), fNewImmoral.get(), bSuccess, 0,
			pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
	}

	// 각 기부 아이템에 대한 보상 지급
	for( unsigned char i = 0 ; i < pMsg->item_count ; ++i )
	{
		// 이모럴이니까 보상수치(양수)를 빼야 감소함(모럴 증가)
		nReward = GameRule::GetDonationRewardMoralPoint( pDonateItemList[i]->GetItemBase().nPrice.GetRawData() ) * pDonateItemInfoList[i].count;

		ItemBase::ItemCode nItemCode = pDonateItemList[i]->GetItemCode();
		__int64 nPrevItemCount = pDonateItemList[i]->GetCount();
		__int64 nDonateItemCount = pDonateItemInfoList[i].count;
		ItemUID nItemUID = pDonateItemList[i]->GetItemUID();

		bool bSuccess = true;
		if( !pClient->EraseItem( pDonateItemList[i], pDonateItemInfoList[i].count ) )
		{
			nReward = 0;
			bSuccess = false;
		}

		fNewImmoral -= nReward;

		LOG::Log11N4S( LM_DONATE_ITEM, pClient->GetAccountID(), pClient->GetPlayerUID(), nItemCode, nPrevItemCount, nDonateItemCount, ( !bSuccess || nPrevItemCount > nDonateItemCount ) ? pDonateItemList[i]->GetCount() : 0, fPrevImmoral.get(), nReward.get(), fNewImmoral.get(), bSuccess, nItemUID,
			pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
	}

	/* JP 기부에 대한 보상 지급
	if( nDonateJP >= GameRule::MIN_DONATE_JP )
	{
	int nPrevJP = pClient->GetJobPoint();

	bool bSuccess = (nPrevJP >= nDonateJP) ? true : false;
	if( bSuccess ) 
	{
	// 이모럴이니까 보상수치(양수)를 빼야 감소함(모럴 증가)
	pClient->SetJP( nPrevJP - nDonateJP );

	nReward = GameRule::GetDonationRewardMoralPoint( GameRule::DONATE_GOLD_PER_JP * nDonateJP );
	fNewImmoral -= nReward;
	}

	LOG::Log11N4S( LM_DONATE_JP, pClient->GetAccountID(), pClient->GetPlayerUID(), 0, nPrevJP, nDonateJP, pClient->GetJobPoint(), fPrevImmoral.get(), nReward.get(), fNewImmoral.get(), bSuccess, 0,
	pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );
	}*/

	// 길드 단위 누적 기부 포인트 기록
	if( GameRule::bUseGuildDonationPoint && pClient->GetGuildID() )
	{
		GuildManager::GetInstance().AddGuildDonationPoint( pClient->GetGuildID(), fPrevImmoral - fNewImmoral );
	}

	// 개인별 기부 포인트 랭킹 데이터 추가
	RankingManager::Instance().AddRankingScore( RankingManager::RANKING_TYPE_DONATION, pClient->GetPlayerUID(), pClient->GetName(), fPrevImmoral - fNewImmoral, true );

	pClient->SetImmoralPoint( fNewImmoral );

	// 블러디 풀렸을 경우 방송
	if( fPrevImmoral >= GameRule::MORAL_LIMIT && !pClient->IsBloodyCharacter() )
		BroadcastStatusMessage( pClient );

	SendResult( pClient, pMsg->id, RESULT_SUCCESS );
}

void onDonateReward( StructPlayer *pClient, TS_CS_DONATE_REWARD* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	c_fixed10 fPrevMoralPoint = pClient->GetImmoralPoint() * -1;
	c_fixed10 fCurrentMoralPoint = fPrevMoralPoint;

	if( !pMsg->reward_count || pMsg->reward_count > GameRule::DONATION_MAX_REWARD_ITEM_TYPE || pMsg->size != sizeof( TS_CS_DONATE_REWARD ) + sizeof( TS_CS_DONATE_REWARD::RewardInfo ) * pMsg->reward_count )
	{
		return;
	}

	TS_CS_DONATE_REWARD::RewardInfo * pRewardInfo = reinterpret_cast< TS_CS_DONATE_REWARD::RewardInfo * >( pMsg + 1 );

	// fCostMoralPoint는 오버 플로우가 발생하지 않음. DONATION_POINT_FOR_REWARD_ITEM의 최대값 * 65535 * c_fixed10::FACTOR * 4 해도 64비트 이내에서 끝남.
	c_fixed10 fCostMoralPoint;
	for( unsigned char i = 0 ; i < pMsg->reward_count ; ++i, ++pRewardInfo )
	{
		fCostMoralPoint += GameRule::DONATION_POINT_FOR_REWARD_ITEM[ pRewardInfo->reward_type ] * pRewardInfo->count;
	}

	if( fCostMoralPoint > fCurrentMoralPoint )
	{
		// 모럴 포인트가 필요 수치 미만이면 223번 스트링(해당 아이템의 소유권이 없습니다) 출력
		SendResult( pClient, pMsg->id, RESULT_NOT_OWN );
		return;
	}

	pRewardInfo = reinterpret_cast< TS_CS_DONATE_REWARD::RewardInfo * >( pMsg + 1 );

	for( unsigned char i = 0 ; i < pMsg->reward_count ; ++i, ++pRewardInfo )
	{
		c_fixed10 fCost = GameRule::DONATION_POINT_FOR_REWARD_ITEM[ pRewardInfo->reward_type ] * pRewardInfo->count;

		if( fCurrentMoralPoint < fCost )
		{
			// 위에서 미리 전체 필요 모럴 포인트를 계산했으므로 여기서 모자라면 뭔가 이상한 것
			assert( 0 );
			break;
		}

		fCurrentMoralPoint -= fCost;

		StructItem *pItem = StructItem::AllocItem( 0, GameRule::DONATION_REWARD_ITEM_CODE[ pRewardInfo->reward_type ], pRewardInfo->count, ItemInstance::BY_DONATION_REWARD );
		if( !pItem )
		{
			break;
		}

		StructItem *pNewItem = pClient->PushItem( pItem, pItem->GetCount() );
		if( !pNewItem )
		{
			continue;
		}

		LOG::Log11N4S( LM_ITEM_TAKE, pClient->GetAccountID(), pClient->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewItem->GetCount(), pClient->GetGold().GetRawData(), pClient->GetGold().GetRawData(), pClient->GetX(), pClient->GetY(), pNewItem->GetItemUID(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "DONATION_REWARD", LOG::STR_NTS );

		if( pNewItem != pItem )
		{
			StructItem::PendFreeItem( pItem );
		}
	}

	// 모럴 수치가 소모 수치 이상일 때만 감소 연산이 일어나므로 Status를 방송할 필요는 없음(모럴 상태에서 보상 받고 데모니악/블러디가 될 일이 없음)
	if( fPrevMoralPoint != fCurrentMoralPoint )
	{
		pClient->SetImmoralPoint( fCurrentMoralPoint * -1 );
	}
}

void onRankingTopRecord( StructPlayer *pClient, TS_CS_RANKING_TOP_RECORD* pMsg )
{
	RankingManager::_RANKING_TYPE eRankingType = static_cast< RankingManager::_RANKING_TYPE >( pMsg->ranking_type );

	// 랭킹 스코어 데이터가 아닌 타입의 경우 상위 랭킹 목록을 요청받아도 응답을 주지 않아야 함
	if( eRankingType == RankingManager::RANKING_TYPE_DONATION_REWARD )
	{
		assert( 0 );
		return;
	}

	static const size_t MAX_RANKING_RECORD_COUNT = 10;
	char szBuffer[ sizeof( TS_SC_RANKING_TOP_RECORD ) + sizeof( TS_SC_RANKING_TOP_RECORD::RANKING_RECORD ) * MAX_RANKING_RECORD_COUNT ];
	memset( szBuffer, 0, sizeof( szBuffer ) );
	TS_SC_RANKING_TOP_RECORD * pTopRecordMsg = new ( szBuffer ) TS_SC_RANKING_TOP_RECORD();

	pTopRecordMsg->ranking_type = pMsg->ranking_type;
	pTopRecordMsg->requester_rank = static_cast< unsigned short >( RankingManager::Instance().GetRank( eRankingType, pClient->GetPlayerUID() ) );
	pTopRecordMsg->requester_score = RankingManager::Instance().GetRankingScore( eRankingType, pClient->GetPlayerUID() ).get();

	std::vector< RankingManager::RankingRecord > vTopRecord;
	RankingManager::Instance().GetRankingTopRecord( eRankingType, vTopRecord );

	pTopRecordMsg->record_count = static_cast< unsigned short >( vTopRecord.size() );
	pTopRecordMsg->size += sizeof( TS_SC_RANKING_TOP_RECORD::RANKING_RECORD ) * pTopRecordMsg->record_count;
	TS_SC_RANKING_TOP_RECORD::RANKING_RECORD * pRankingRecord = reinterpret_cast< TS_SC_RANKING_TOP_RECORD::RANKING_RECORD * >( pTopRecordMsg + 1 );

	unsigned short nRank = 0;
	for( std::vector< RankingManager::RankingRecord >::const_iterator it = vTopRecord.begin() ; it != vTopRecord.end() ; ++it, ++pRankingRecord )
	{
		pRankingRecord->rank = ++nRank;
		s_strcpy( pRankingRecord->ranker_name, _countof( pRankingRecord->ranker_name ), StructPlayer::GetPlayerName( (*it).first ) );
		pRankingRecord->score = (*it).second.get();
	}
	assert( nRank == pTopRecordMsg->record_count );

	PendMessage( pClient, pTopRecordMsg );
}

void onArrangeItem( StructPlayer *pClient, TS_CS_ARRANGE_ITEM* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	unsigned short nRet = pClient->ArrangeItem( pMsg->bIsStorage );
	SendResult( pClient, pMsg->id, nRet, pMsg->bIsStorage );

	if( nRet == RESULT_SUCCESS )
		SendItemList( pClient, pMsg->bIsStorage );
}

void onChangeSummonName( StructPlayer *pClient, TS_CS_CHANGE_SUMMON_NAME* pMsg )
{
	// 락은 소환수 기준으로 걸어야 하지만 소환수의 포인터를 얻기 위해서는
	// 플레이어 기준의 락도 필요함. 결국 플레이어와 소환수의 공용 락이
	// 필요한데, 소환수의 포인터를 얻기 이전에는 소환수의 정보도 얻을 수 없으므로
	// 전역 락을 사용한다(크리처 이름 변경은 방송을 동반한다)
	ARCADIA_LOCK( ArcadiaServer::Instance().LockWorld() );

	if( !pClient->GetLastContactLong( "creature_name_change" ) )
		return;

	pClient->SetLastContact( "creature_name_change", 0 );


	int index = pClient->GetNameChangeTarget();

	if( !index )
		return;

	StructSummon *pSummon = pClient->GetSummon( index );

	pClient->SetNameChangeTarget( 0 );

	if( !pSummon )
		return;

	// 유효한 이름인지 체크
	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	pMsg->szName[ _countof( pMsg->szName ) - 1 ] = '\0';
	if( strlen( pMsg->szName ) < 4 )
	{
		PrintfChatMessage( false, CHAT_SUMMON, "@SYSTEM", pClient, "@117" );
		return;
	}
	// 같은 이름 사용 불가
	else if( !strncmp( pMsg->szName, pSummon->GetName(), std::max( strlen( pMsg->szName ), strlen( pSummon->GetName() ) ) ) )
	{
		PrintfChatMessage( false, CHAT_SUMMON, "@SYSTEM", pClient, "@118" );
		return;
	}
	else if( strlen( pMsg->szName ) == 0 || strlen( pMsg->szName ) > 18 || !_stricmp( pSummon->GetName(), pMsg->szName ) ||
		!GameRule::IsValidName( code_page, pMsg->szName, (int)strlen( pMsg->szName ) + 1, 4, 18 ) || // IsValidName에서 소환수 이름은 최대 길이 19 Byte이므로 버퍼 제한 18로 고정
		GameContent::IsBannedWord( code_page, pMsg->szName ) )
	{
		PrintfChatMessage( false, CHAT_SUMMON, "@SYSTEM", pClient, "@17" );
		return;
	}


	// 소환수 이름 변경 프로세스를 스크립트로 요청
	std::string szChangeSummonNameFunction = " ";
	XStringUtil::Format( szChangeSummonNameFunction, "change_summon_name( %d, '%s' )", index, pMsg->szName );

	LUA()->RunString( szChangeSummonNameFunction.c_str() );

	// { 변경된 이름 및 성공/실패 방송
	// 변경에 성공했는지 체크
	if( strncmp( pMsg->szName, pSummon->GetName(), std::max( strlen( pMsg->szName ), strlen( pSummon->GetName() ) ) ) )
	{
		// 실패 메시지
		PrintfChatMessage( false, CHAT_SUMMON, "@SYSTEM", pClient, "@129" );
		return;
	}

	// 성공 메시지(클라에서 TS_SC_CHANGE_NAME 메시지 받으면 출력하게 변경되었음)
	//PrintfChatMessage( false, CHAT_SUMMON, "@SYSTEM", pClient, "@132\v#@creature_name@#\v%s", pSummon->GetName() );

	// 각종 UI에 이름 변경을 알려주기 위함
	SendPropertyMessage( pClient, pSummon->GetHandle(), "name", pSummon->GetName() );

	// 주변 사람들에게 바뀐 이름 보이게 하기 위함
	TS_SC_CHANGE_NAME msg;
	msg.handle = pSummon->GetHandle();
	s_strcpy( msg.name, _countof( msg.name ), pSummon->GetName() );

	ArcadiaServer::Instance().Broadcast( pSummon->GetRX(), pSummon->GetRY(), pSummon->GetLayer(), &msg );
	// } 방송 끝

	// 소환수 스텟 재계산(필요는 없지만 -_-;)
	if( pSummon->IsInWorld() )
		pSummon->SetNeedCalculateStat();
	else
		pSummon->CalculateStat();

	// 변경된 정보 저장
	pClient->Save();
}

void onGetSummonSetupInfo( StructPlayer *pClient, TS_CS_GET_SUMMON_SETUP_INFO* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	SendCreatureEquipMessage( pClient, pMsg->show_dialog );
}

void onSetPetName( StructPlayer *pClient, TS_CS_SET_PET_NAME* pMsg )
{
	// 소환된 펫은 이름이 바뀔 수 없음(소환되기 전에 이름을 바꾸게 되므로)
	if( !pMsg->handle )
		return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	// 지정된 펫이 존재하는지, 월드에 나와 있는지(소환되기 전에 이름을 바꾸므로 월드에 없어야 함),
	// 이름을 바꿀 수 있는 레어 펫인지 체크
	StructCreature::iterator itPet = StructCreature::get( pMsg->handle );
	StructCreature *pTarget = (*itPet);

	if( !pTarget || !pTarget->IsPet() )
		return;

	StructPet *pPet = static_cast< StructPet * >( pTarget );

	if( pPet->IsInWorld() || !pPet->IsRare() || pPet->IsNameChanged() )
		return;

	// 소유권 체크(펫 주인 + 펫 우리 소유권)
	StructItem *pCage = pPet->GetParentCage();
	if( !pCage || pCage->GetOwnerHandle() != pClient->GetHandle() || pPet->GetMaster() != pClient )
		return;

	// NULL 문자 없어서 거지되는 걸 막음
	pMsg->name[ _countof( pMsg->name ) - 1 ] = '\0';

	unsigned short nResult = pPet->ChangeName( pMsg->name, false );

	// 시스템 메시지 출력
	switch( nResult )
	{
	case RESULT_ACCESS_DENIED:
	case RESULT_LIMIT_MAX:
		PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", pClient, "@1106" );
		break;

	case RESULT_LIMIT_MIN:
		PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", pClient, "@1105" );
		break;

	case RESULT_ALREADY_EXIST:
		PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", pClient, "@118" );
		break;
	}

	SendResult( pClient, pMsg->id, nResult, pMsg->handle );
}

void onCancelAction( StructPlayer *pClient, TS_CS_CANCEL_ACTION* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructCreature * pCreature = pClient;

	if( pClient->GetHandle() != pMsg->handle )
	{
		if( pCreature = pClient->GetSummon( pMsg->handle ) )
		{
			if( !pCreature->IsInWorld() )
				return;
		}
		else
		{
			return;
		}

	}

	if( pCreature->IsUsingSkill() )
	{
		pCreature->CancelSkill();
	}
	else if( pCreature->IsAttacking() )
	{
		pCreature->CancelAttack();
	}
}

void onSkill( StructPlayer *pClient, TS_CS_SKILL* pMsg )
{
	if( pClient->IsDead() ) return;

	AR_TIME	casting_time = 0;
	StructCreature *pCaster = pClient;

	if( pMsg->caster != pClient->GetHandle() ) pCaster = pClient->GetSummon( pMsg->caster );

	// 활동불가 체크
	if( !pCaster || !pCaster->IsInWorld() )
	{
		StructSkill::SendSkillCastFailMessage( pClient, pMsg->caster, pMsg->target, pMsg->skill_id, pMsg->skill_level, ArPosition( pMsg->x, pMsg->y, pMsg->z ), RESULT_NOT_EXIST );
		return;
	}

	SkillBase *pBase = GameContent::GetSkillBase( pMsg->skill_id );
	if( !pBase || !pBase->IsValid() || pBase->IsSystemSkill() )
	{
		StructSkill::SendSkillCastFailMessage( pClient, pMsg->caster, pMsg->target, pMsg->skill_id, pMsg->skill_level, ArPosition( pMsg->x, pMsg->y, pMsg->z ), RESULT_ACCESS_DENIED );
		return;
	}

	if( pClient == pCaster )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

		if( pClient->IsSitDown() )
		{
			pClient->StandUp();
			BroadcastStatusMessage( pClient );
		}
	}

	// 캐스팅 가능한지 검사
	if( ( pBase->IsPhysicalSkill() && !pCaster->IsSkillCastable() ) || ( !pBase->IsPhysicalSkill() && !pCaster->IsMagicCastable() ) )
	{
		//_cprint( "시전실패 (행동불가) : %s\n", pCaster->GetName() );
		StructSkill::SendSkillCastFailMessage( pClient, pMsg->caster, pMsg->target, pMsg->skill_id, pMsg->skill_level, ArPosition( pMsg->x, pMsg->y, pMsg->z ), RESULT_NOT_ACTABLE );
		return;
	}

	// 스킬이 발동한 region 좌표 
	unsigned rx, ry;
	rx = pMsg->x / g_nRegionSize;
	ry = pMsg->y / g_nRegionSize;

	// 만약 타겟 대상이라면 region 좌표를 타겟의 위치로 수정
	GameObject * pTarget = NULL;
	GameObject::iterator itTarget;
	if( pMsg->target ) 
	{
		itTarget = GameObject::get( pMsg->target );
		pTarget = (*itTarget);

		if( !pTarget )
		{
			// _cprint( "시전실패 (대상없음) : %s\n", pCaster->GetName() );
			StructSkill::SendSkillCastFailMessage( pClient, pMsg->caster, pMsg->target, pMsg->skill_id, pMsg->skill_level, ArPosition( pMsg->x, pMsg->y, pMsg->z ), RESULT_NOT_EXIST );
			return;
		}

		if( pTarget->IsCreature() )
		{
			if( !isValidTarget( pClient, pTarget ) )
			{
				SendLeaveMsg( pClient, pTarget );
				StructSkill::SendSkillCastFailMessage( pClient, pMsg->caster, pMsg->target, pMsg->skill_id, pMsg->skill_level, ArPosition( pMsg->x, pMsg->y, pMsg->z ), RESULT_NOT_EXIST );
				return;
			}

			rx = pTarget->GetRX();
			ry = pTarget->GetRY();	
		}
		else
		{
			// 이 부분이 무슨 의미가 있을지 잘 모르겠다;; 아이템이나 프랍 핸들을 타겟으로 보내는 경우도 있나;?
			rx = pCaster->GetRX();
			ry = pCaster->GetRY();

			pTarget = NULL;
		}
	}

	ArcadiaLock __lock = -1;
	
	// 유효한 타겟이 있다면 시전자와 타겟을 포함하는 락을 걸고, 아니라면 시전자와 패킷에 지정된 영역을 포함하는 락을 걸어 줌
	if( pTarget )
		__lock = ArcadiaServer::Instance().LockObjects( pCaster, pTarget );
	else
		__lock = ArcadiaServer::Instance().LockObjectWithSpecificRegion( pCaster, rx, ry );

	ARCADIA_LOCK( __lock );

	if( pCaster->IsMoving( GetArTime() ) )
	{
		StructSkill::SendSkillCastFailMessage( pClient, pMsg->caster, pMsg->target, pMsg->skill_id, pMsg->skill_level, ArPosition( pMsg->x, pMsg->y, pMsg->z ), RESULT_NOT_ACTABLE );
		return;
	}

	// one more time~!!
	if( ( pBase->IsPhysicalSkill() && !pCaster->IsSkillCastable() ) || ( !pBase->IsPhysicalSkill() && !pCaster->IsMagicCastable() ) )
	{
		//_cprint( "시전실패 (행동불가) : %s\n", pCaster->GetName() );
		StructSkill::SendSkillCastFailMessage( pClient, pMsg->caster, pMsg->target, pMsg->skill_id, pMsg->skill_level, ArPosition( pMsg->x, pMsg->y, pMsg->z ), RESULT_NOT_ACTABLE );
		return;
	}

	const StructSkill * pSkill = pCaster->GetSkill( pMsg->skill_id );

	if( !pSkill || pSkill && pSkill->GetSkillUID() == StructSkill::SKILL_UID_ITEM_SKILL )
		return;

	// 디버프로 인해 현재 스킬이 음수인 경우 무슨 일이 있더라도 발동이 되지 않아야 한다.
	if( pSkill->GetCurrentSkillLevel() <= 0 )
	{
		StructSkill::SendSkillCastFailMessage( pClient, pMsg->caster, pMsg->target, pMsg->skill_id, pMsg->skill_level, ArPosition( pMsg->x, pMsg->y, pMsg->z ), RESULT_ACCESS_DENIED );
		return;
	}

	// 사용하려는 스킬 레벨이 보유 스킬 레벨보다 높거나 음수일 경우 제한
	if( pMsg->skill_level <= 0 || pMsg->skill_level > pSkill->GetCurrentSkillLevel() )
	{
		pMsg->skill_level = pSkill->GetCurrentSkillLevel();
	}

	// 캐스팅 시작
	int nResult = pCaster->CastSkill( pMsg->skill_id, pMsg->skill_level, pMsg->target, ArPosition( pMsg->x, pMsg->y, pMsg->z ), pCaster->GetLayer() );

	if( nResult != RESULT_SUCCESS )
	{
		StructSkill::SendSkillCastFailMessage( pClient, pMsg->caster, pMsg->target, pMsg->skill_id, pMsg->skill_level, ArPosition( pMsg->x, pMsg->y, pMsg->z ), nResult );
	}
}

void onEnterEventArea( StructPlayer * pClient, TS_CS_ENTER_EVENT_AREA* pMsg )
{
	// 지역락 걸고~(스크립트 실행 시에는 지역락 걸어놓고 실행하기)
	// 단, 지역 락의 영역을 일반 시야 범위의 두 배 반경으로 설정함.
	// 이렇게 해야 시야 범위 내에서 어떤 변화가 생겨서 변화 지점으로부터 시야 범위 내의 영역에 방송을 해도 락 범위 내부가 됨
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithSpecificRange( pClient, VISIBLE_REGION_RANGE * 2 ) );

	if( !GameContent::IsActivatableEventArea( pClient, pMsg->event_area_id, pMsg->area_index ) )
		return;

	if( !pClient->IsInWorld() || !pClient->IsActable() )
		return;

	// 스크립트 실행
	LUA()->RunString( GameContent::GetEventAreaEnterHandler( pMsg->event_area_id ) );

	pClient->InsertEventAreaID( pMsg->event_area_id );

	// 이벤트 영역 진입 횟수 제한 영역일 경우 진입했던 횟수 증가
	if( GameContent::GetEnterCountLimitOfEventArea( pMsg->event_area_id ) )
	{
		pClient->IncEventAreaEnterCount( pMsg->event_area_id );
	}
}

void onLeaveEventArea( StructPlayer * pClient, TS_CS_LEAVE_EVENT_AREA* pMsg )
{
	// 지역락 걸고~(스크립트 실행 시에는 지역락 걸어놓고 실행하기)
	// 단, 지역 락의 영역을 일반 시야 범위의 두 배 반경으로 설정함.
	// 이렇게 해야 시야 범위 내에서 어떤 변화가 생겨서 변화 지점으로부터 시야 범위 내의 영역에 방송을 해도 락 범위 내부가 됨
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithSpecificRange( pClient, VISIBLE_REGION_RANGE * 2 ) );

	if( !pClient->IsInWorld() || !pClient->IsActable() )
		return;

	// 기존에 진입되어 있던 영역이 아니라면 떠나기 핸들러 호출도 실패
	if( !pClient->DeleteEventAreaID( pMsg->event_area_id ) )
		return;

	// 스크립트 실행
	LUA()->RunString( GameContent::GetEventAreaLeaveHandler( pMsg->event_area_id ) );
}

void onUseItem( StructPlayer * pClient, TS_CS_USE_ITEM* pMsg )
{
	if( !pClient->IsItemUseable() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE, pMsg->item_handle );
		return;
	}

	// 아이템 없으면 KIN
	StructItem *pItem = StructItem::FindItem( pMsg->item_handle );
	if( !pItem || pItem->GetOwnerHandle() != pClient->GetHandle() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST, pMsg->item_handle );
		return;
	}

	// 인벤토리에 있는 아이템이 아닌 경우 사용 불가(시스템 메시지 출력 안 함)
	if( !pItem->IsInInventory() )
	{
		return;
	}

	if( !pItem->IsSupplyItem() && !pItem->IsUsingItem() )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED, pMsg->item_handle );
		return;
	}

	if( ( pItem->IsWarpItem() || !pItem->IsUsableMoving() ) && pClient->IsMoving( GetArTime() ) )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE, pMsg->item_handle );
		return;
	}

	if( !pItem->IsUsableSit() && pClient->IsSitDown() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE, pMsg->item_handle );
		return;
	}

	if( pItem->IsUsableOnOnlySit() && !pClient->IsSitDown() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE_ON_STAND_UP, pMsg->item_handle );
		return;
	}

	if( !pItem->IsUsableInSecroute() && pClient->IsInSecroute() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE_IN_SECROUTE, pMsg->item_handle );
		return;
	}

	if( !pItem->IsUsableInSiegeOrRaid() && pClient->IsInSiegeOrRaidDungeon() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE_IN_SIEGE_OR_RAID, pMsg->item_handle );
		return;
	}

	if( !pItem->IsUsableInEventmap() && pClient->IsInEventmap() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE_IN_EVENTMAP, pMsg->item_handle );
		return;
	}

	if( HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() ) )
	{
		if( !pItem->IsUsableInHuntaholic() )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE_IN_HUNTAHOLIC, pMsg->item_handle );
			return;
		}
	}
	else if( pItem->IsUsableInOnlyHuntaholic() )
	{
		SendResult( pClient, pMsg->id, RESULT_ACTABLE_IN_ONLY_HUNTAHOLIC, pMsg->item_handle );
		return;
	}

	if( pClient->IsInDeathmatch() )
	{
		if( !pItem->IsUsableInDeathmatch() )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE_IN_DEATHMATCH, pMsg->item_handle );
			return;
		}
	}
	else if( pItem->IsUsableInOnlyDeathmatch() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_ACTABLE_IN_ONLY_DEATHMATCH, pMsg->item_handle );
		return;
	}

	if( !pItem->IsUsableInSecretDungeon() && pClient->IsInSecretDungeon() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE_IN_SECRET_DUNGEON, pMsg->item_handle );
		return;
	}

	if( !pItem->IsUsableInBattleArena() && pClient->IsInBattleArena() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE_IN_BATTLE_ARENA, pMsg->item_handle );
		return;
	}

	if( pItem->IsExpireItem() && pItem->GetExpireTime() < time( NULL ) )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE, pMsg->item_handle );
		return;
	}

	int nResult = pClient->IsUseableItem( pItem, NULL );

	if( nResult != RESULT_SUCCESS )
	{
		// 아이템 쿨타임이 덜 됐는데 사용 요청이 왔다면 클라 쿨타임 정보 갱신
		if( nResult == RESULT_COOL_TIME )
			SendItemCoolTimeInfo( pClient );

		SendResult( pClient, pMsg->id, nResult, pMsg->item_handle );
		return;
	}

	if( pClient->GetBoothStatus() != StructPlayer::IS_NOT_BOOTH )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE, pMsg->item_handle );
		return;
	}

	int nTargetType = 0;
	int nTargetID = 0;
	if( pItem->IsNeedTarget() )
	{
		do
		{
			GameObject *pObj = GameObject::raw_get( pMsg->target_handle );
			if( !pObj )
				break;

			if( pObj->IsPlayer() )			nTargetType = 1;	// 플레이어
			else if( pObj->IsMonster() )	nTargetType = 2;	// 몬스터
			else if( pObj->IsSummon() )		nTargetType = 3;	// 소환수
			else if( pObj->IsNPC() )		nTargetType = 4;	// NPC
			else if( pObj->IsCreature() )	nTargetType = 5;	// 그 외 생물체
			else							nTargetType = 6;	// 생물도 아닌 것

			if( nTargetType != 6 )
				nTargetID = static_cast< StructCreature * >(pObj)->GetSID();

		} while( false );
	}

	pMsg->szParameter[ _countof( pMsg->szParameter ) - 1 ] = '\0';

	const ItemUID		nItemUID				= pItem->GetItemUID();
	const __int64					nItemEnhanceAndLevel	= pItem->GetItemEnhance() * 100 + pItem->GetItemLevel();
	const ItemBase::ItemCode		nItemCode				= pItem->GetItemCode();
	const __int64					nItemCount				= pItem->GetCount();
	const bool						bDecreaseCount			= pItem->GetItemBase().nType != ItemBase::TYPE_USE;

	if( pItem->IsNeedTarget() )
	{
		StructCreature::iterator it = StructCreature::get( pMsg->target_handle );
		StructCreature *pCreature = *it;

		if( !pCreature || !pCreature->IsCreature() )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_EXIST, pMsg->item_handle );
			return;
		}

		nResult = pClient->IsUseableItem( pItem, pCreature );
		if( nResult != RESULT_SUCCESS )
		{
			SendResult( pClient, pMsg->id, nResult, pMsg->item_handle );
			return;
		}

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( pClient, pCreature ) );

		// 스크립트가 있으면서 사용형일 경우, 따로 TS_SC_RESULT를 보내주진 않는다. (스크립트 내에서 알아서 하라고)
		unsigned short nRet = pClient->UseItem( pItem, pCreature, pMsg->szParameter );
		if( nRet != RESULT_SUCCESS_WITHOUT_NOTICE )
		{
			SendResult( pClient, pMsg->id, nRet, pMsg->item_handle );
		}

		if( nRet != RESULT_SUCCESS )
			return;
	}
	else
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

		// 스크립트가 있으면서 사용형일 경우, 따로 TS_SC_RESULT를 보내주진 않는다. (스크립트 내에서 알아서 하라고)
		unsigned short nRet = pClient->UseItem( pItem, NULL, pMsg->szParameter );
		if( nRet != RESULT_SUCCESS_WITHOUT_NOTICE )
		{
			SendResult( pClient, pMsg->id, nRet, pMsg->item_handle );
		}

		if( nRet != RESULT_SUCCESS && nRet != RESULT_SUCCESS_WITHOUT_NOTICE )
			return;
	}

	LOG::Log11N4S( LM_ITEM_USE, pClient->GetAccountID(), pClient->GetSID(), nItemEnhanceAndLevel, nItemCode, 1, ( bDecreaseCount ) ? nItemCount - 1 : nItemCount, nTargetType, nTargetID, 0, 0, nItemUID, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, pMsg->szParameter, LOG::STR_NTS, "", 0 );

	TS_SC_USE_ITEM_RESULT msg;
	msg.item_handle = pMsg->item_handle;
	msg.target_handle = pMsg->target_handle;
	PendMessage( pClient, &msg );

	return;
}

void onSetProperty( StructPlayer * pClient, TS_CS_SET_PROPERTY* pMsg )
{
	// 데이터가 잘리더라도 업데이트 해주도록 변경
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	char szName[_countof(pMsg->name)];
	s_strcpy( szName, _countof( szName ), pMsg->name );

	// 혹시나 유저에 의해 업데이트 된 클라 저장 정보가 잘리는 지 (4096Bytes 이상) 검사: 업데이트는 해주고 로그만 남김
	if( pMsg->size > sizeof( *pMsg ) + 4096 )
	{
		_cprint( "Client Saving Information has exceeced its allowed size(4096Bytes): [%s:%s]\n", pClient->GetName(), szName );
		FILELOG( "Client Saving Information has exceeced its allowed size(4096Bytes): [%s:%s]", pClient->GetName(), szName );
	}

	if( !_stricmp( szName, "client_info" ) )
	{
		// client_info 갱신
		char* pClientInfo = reinterpret_cast< char* >( pMsg+1 );
		char szValue[4096] = "";

		s_memcpy( szValue, sizeof( szValue ), pClientInfo, sizeof( szValue ) - sizeof( szValue[0] ) );
		szValue[ _countof( szValue ) - 1 ] = '\0';

		if( strcmp( pClient->GetClientInfo(), szValue ) != 0 )
		{
			pClient->SetClientInfo( szValue );
			pClient->DBQuery( new DB_UpdateClientInfo( pClient, pMsg+1 ) );
		}
	}
	else if( !_stricmp( szName, "quick_slot" ) )
	{
		// quick_slot 갱신
		char* pClientInfo = reinterpret_cast< char* >( pMsg+1 );
		char szValue[4096] = "";

		s_memcpy( szValue, sizeof( szValue ), pClientInfo, sizeof( szValue ) - sizeof( szValue[0] ) );
		szValue[ _countof( szValue ) - 1 ] = '\0';

		if( strcmp( pClient->GetQuickSlot(), szValue ) != 0 )
		{
			pClient->SetQuickSlot( szValue );
			pClient->DBQuery( new DB_UpdateQuickSlot( pClient, pMsg+1 ) );
		}
	}
	else if( !_stricmp( szName, "current_key" ) )
	{
		// current_key 갱신
		char* pClientInfo = reinterpret_cast< char* >( pMsg+1 );
		char szValue[4096] = "";

		s_memcpy( szValue, sizeof( szValue ), pClientInfo, sizeof( szValue ) - sizeof( szValue[0] ) );
		szValue[ _countof( szValue ) - 1 ] = '\0';

		if( strcmp( pClient->GetCurrentKey(), szValue ) != 0 )
		{
			pClient->SetCurrentKey( szValue );
			pClient->DBQuery( new DB_UpdateCurrentKey( pClient, pMsg+1 ) );
		}
	}
	else if( !_stricmp( szName, "saved_key" ) )
	{
		// saved_key 갱신
		char* pClientInfo = reinterpret_cast< char* >( pMsg+1 );
		char szValue[4096] = "";

		s_memcpy( szValue, sizeof( szValue ), pClientInfo, sizeof( szValue ) - sizeof( szValue[0] ) );
		szValue[ _countof( szValue ) - 1 ] = '\0';

		if( strcmp( pClient->GetSavedKey(), szValue ) != 0 )
		{
			pClient->SetSavedKey( szValue );
			pClient->DBQuery( new DB_UpdateSavedKey( pClient, pMsg+1 ) );
		}
	}
}

StructItem * check_mixable_item( StructPlayer * pPlayer, AR_HANDLE hItem, const __int64 & nItemCount )
{
	StructItem::iterator it = StructItem::get( hItem );
	if( !it.get() ) return NULL;
	if( !it.get()->IsItem() ) 
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_ACCESS_DENIED );
		return NULL;
	}

	StructItem *pItem = static_cast< StructItem* >( it.get() );
	if( !pItem ) 
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_NOT_EXIST );
		return NULL;	
	}

	if( pItem->GetCount() < nItemCount )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_NOT_EXIST );
		return NULL;	
	}

	if( pItem->GetBaseFlag().IsOn( ItemBase::FLAG_CANT_ENHANCE ) )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_ACCESS_DENIED );
		return NULL;
	}

	if( !pPlayer->IsMixable( pItem ) ) 
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_NOT_EXIST );
		return NULL;
	}

	return pItem;
}

void onMix( StructPlayer *pClient, TS_CS_MIX *pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pMsg->count > 9 || pMsg->size != sizeof( TS_CS_MIX ) + sizeof( TS_CS_MIX::MIX_INFO ) * pMsg->count )
	{
		if( GameRule::bUseAutoJail )
		{
			pClient->AddState( StructState::NEMESIS_FOR_AUTO, pClient->GetHandle(), 12, GetArTime(), GetArTime() + 864000000 );
			pClient->SetAutoUsed();
			pClient->Save( true );
		}

		LOG::Log11N4S( LM_AUTO_USER_CHECKED, pClient->GetAccountID(), pClient->GetSID(), pClient->GetLevel(), pClient->GetJobLevel(), pClient->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::MIX_BUFFER_OVERRUN_ATTEMPTION, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );

		pClient->pConnection->Close();
		return;
	}

	StructItem * pMainItem		= NULL;
	StructItem * pSubItem[ MixBase::MAX_SUB_MATERIAL_COUNT ];
	unsigned short pCountList[ MixBase::MAX_SUB_MATERIAL_COUNT ];

	memset( pSubItem, 0, sizeof( pSubItem ) );
	memset( pCountList, 0, sizeof( pCountList ) );

	pMainItem = check_mixable_item( pClient, pMsg->main_item.handle, 1 );

	if( pMsg->main_item.handle && !pMainItem )
		return;

	TS_CS_MIX::MIX_INFO *pMixInfo = reinterpret_cast< TS_CS_MIX::MIX_INFO* >( pMsg+1 );

	for( int i = 0; i < pMsg->count; ++i, ++pMixInfo )
	{
		pSubItem[i] = check_mixable_item( pClient, pMixInfo->handle, pMixInfo->count );
		if( !pSubItem[i] )
			return;
		pCountList[i] = pMixInfo->count;
	}

	const MixBase * pMixBase = MixManager::GetProperMixInfoAndArrangeSubMaterials( pMainItem, pMsg->count, pSubItem, pCountList );

	if( !pMixBase )
	{
		SendResult( pClient, pMsg->id, RESULT_INVALID_ARGUMENT );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// 미확인 아이템 식별이 아니면
	if( pMixBase->type != MixBase::MIX_ITEM_IDENTIFY_FOR_RANDOM_OPTION )
	{
		if( pMainItem != NULL )
		{
			// 미확인 아이템이면 강화 조합 불가
			if( pMainItem->IsRandomizable() == true && pMainItem->IsIdentified() == false )
			{
				SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
				return;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	std::vector< std::pair< StructItem *, __int64 > > vCreatedItem;

	switch( pMixBase->type )
	{
	case MixBase::MIX_ENHANCE:
	case MixBase::MIX_ENHANCE_WITHOUT_FAIL:
		MixManager::EnhanceItem( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_ENHANCE_SKILL_CARD:
	case MixBase::MIX_ENHANCE_SKILL_CARD_WITHOUT_FAIL:
		MixManager::EnhanceSkillCard( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_ENHANCE_CREATURE_CARD:
	case MixBase::MIX_ENHANCE_CREATURE_CARD_WITH_JOKER:
		MixManager::EnhanceCreatureCard( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_ULTIMATE_ENHANCE:
		MixManager::UltimateEnhance( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_SET_LEVEL:
	case MixBase::MIX_SET_LEVEL_CREATE_ITEM:
	case MixBase::MIX_UNK203:
	case MixBase::MIX_SET_LEVEL_SET_FLAG:
	case MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM:
	case MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL:
	case MixBase::MIX_SET_LEVEL_ON_SUB_MATERIAL_LEVEL_SET_FLAG:
	case MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL_SET_ZERO:

	case MixBase::MIX_ADD_LEVEL:
	case MixBase::MIX_ADD_LEVEL_CREATE_ITEM:
	case MixBase::MIX_ADD_LEVEL_SET_FLAG:
	case MixBase::MIX_ADD_LEVEL_SET_FLAG_CREATE_ITEM:

		MixManager::MixItemLevel( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_RECYCLE:
	case MixBase::MIX_RECYCLE_ENHANCE:
		MixManager::RecycleItem( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_RESTORE_ENHANCE_SET_FLAG:
		MixManager::RestoreEnhance( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_CREATE_ITEM:
	case MixBase::MIX_CREATE_ITEM_IN_MASS:
		MixManager::CreateItem( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList, &vCreatedItem );
		break;

	case MixBase::MIX_REPLACE_WITH:
	case MixBase::MIX_CHANGE_CODE_ADD:
	case MixBase::MIX_REPLACE_WITH_RANDOM:
		MixManager::ChangeItemCode(pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList, &vCreatedItem);
		break;

	case MixBase::MIX_CHANGE_APPEARANCE_CODE:
		MixManager::ChangeAppearanceCode( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_CHANGE_USABLE_PERIOD:
		MixManager::ChangeItemUsablePeriod( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_SET_ELEMENTAL_EFFECT:
		MixManager::SetElementalEffect( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_SET_ELEMENTAL_EFFECT_PARAMETER:
		MixManager::SetElementalEffectParameter( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_SET_SOCKET:
		MixManager::SetSocket( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_REPLACE_SOCKET_WITH:
		MixManager::ReplaceSocketWith( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_SACRIFICE_ITEM_FOR_ETHEREAL_DURABILITY_WITH_MESSAGE:
	case MixBase::MIX_SACRIFICE_ITEM_FOR_ETHEREAL_DURABILITY:
		MixManager::SacrificeItemForEtherealDurability( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_TRANSMIT_ETHEREAL_DURABILITY:
		MixManager::TransmitEtherealDurability( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_RECOVER_EXHAUSTED_ETHEREAL_DURABILITY:
		MixManager::RecoverExhaustedEtherealDurability( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_SACRIFICE_ITEM_FOR_ETHEREAL_STONE_DURABILITY:
		MixManager::SacrificeItemForEtherealStoneDurability( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_TRANSMIT_ETHEREAL_DURABILITY_FROM_ETHEREAL_STONE:
		MixManager::TransmitEtherealDurabilityFromEtherealStone( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_AWAKEN_ITEM:
		MixManager::InsertAwakenItem( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_DELETE_AWAKEN_OPTION:
		MixManager::DeleteAwakenOption(pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_ITEM_IDENTIFY_FOR_RANDOM_OPTION:
	case MixBase::MIX_UNK_902:
		MixManager::IdentifyItemForRandomOption( pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList );
		break;

	case MixBase::MIX_UNK_903:
		MixManager::DeleteRandomOption(pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList);
		MixManager::IdentifyItemForRandomOption(pMixBase, pClient, pMainItem, pMsg->count, pSubItem, pCountList);
		break;

	default:
		SendResult( pClient, pMsg->id, RESULT_INVALID_ARGUMENT );
		break;
	}

	pClient->UpdateTitleConditionByItemCreateByMixing( vCreatedItem );
}

void onDecompose( StructPlayer *pClient, TS_CS_DECOMPOSE *pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	// 메시지 유효성 검사
	if( pMsg->count > GameRule::MAX_DECOMPOSE_ITEM_COUNT || pMsg->size != sizeof( TS_CS_DECOMPOSE ) + sizeof( TS_CS_DECOMPOSE::DECOMPOSE_INFO ) * pMsg->count )
	{
		// 유저가 잘못 된 패킷을 보냈을 경우 패킷 조작에 의한 프로그램 해킹 시도라고 간주하고 AutoJail에 가두는 코드(?)/ 확인 요망
		if( GameRule::bUseAutoJail )
		{
			pClient->AddState( StructState::NEMESIS_FOR_AUTO, pClient->GetHandle(), 12, GetArTime(), GetArTime() + 864000000 );
			pClient->SetAutoUsed();
			pClient->Save( true );
		}

		LOG::Log11N4S( LM_AUTO_USER_CHECKED, pClient->GetAccountID(), pClient->GetSID(), pClient->GetLevel(), pClient->GetJobLevel(), pClient->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::DECOMPOSE_BUFFER_OVERRUN_ATTEMPTION, 0, pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		pClient->pConnection->Close();
		return;
	}

	if( !pMsg->count )
	{
		SendResult( pClient, TM_CS_DECOMPOSE, RESULT_NOT_EXIST );
		return;
	}

	std::vector< std::pair< StructItem*, __int64 > > vCreatedItem;		// 호칭 처리용

	TS_CS_DECOMPOSE::DECOMPOSE_INFO *pDecomposeInfo = reinterpret_cast< TS_CS_DECOMPOSE::DECOMPOSE_INFO* >( pMsg+1 );
	//AR_HANDLE *pItemHandle = reinterpret_cast< AR_HANDLE* >( pMsg+1 );
	for( unsigned int i = 0; i < pMsg->count; ++i, ++pDecomposeInfo )
	{
		MixManager::FindAndDecompose( pClient, &vCreatedItem, pDecomposeInfo->handle, pDecomposeInfo->count );
	}

	// 분해 유형 중 아이템을 생성했을 때에는 중첩가능의 경우 한번에 Update Query를 여기서 일괄적으로 날려준다.
	unsigned int resultCount = 0;
	unsigned int nResult = RESULT_INVALID_ARGUMENT;
	std::vector< TS_SC_DECOMPOSE_RESULT::DECOMPOSE_INFO > vResultItems( vCreatedItem.size() );	// 결과 패킷 처리용 벡터
	for( std::vector< std::pair< StructItem*, __int64 > >::iterator it = vCreatedItem.begin() ; it != vCreatedItem.end() ; ++it, ++resultCount )
	{
		StructItem* pItem = it->first;
		// UpdateQuery를 여러번 날리지 않기 위해 위에서 분해결과 신규 아이템이 아니면서 중첩 가능한 경우 DB 업데이트를 여기까지 유보한다.
		if( pItem->IsJoinable() )
			pItem->DBQuery( new DB_UpdateItem( pItem ) );
		vResultItems[ resultCount ] = TS_SC_DECOMPOSE_RESULT::DECOMPOSE_INFO( it->first->GetHandle(), it->second );
	}
		
	if( resultCount != 0 )
	{
		SendDecomposeResult( pClient, &vResultItems[0], resultCount );
		nResult = RESULT_SUCCESS;
	}

	SendResult( pClient, TM_CS_DECOMPOSE, nResult );
	return;
}

void onLearnSkill( StructPlayer *pClient, TS_CS_LEARN_SKILL* pMsg )
{
	StructCreature::iterator it;
	StructCreature *pCreature = pClient;

	// 소환수에게 시도한 경우
	if( pClient->GetHandle() != pMsg->target )
	{
		it = StructCreature::get( pMsg->target );

		// 소환수가 아니면 리턴
		if( !(*it) || !(*it)->IsSummon() ) 
		{
			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
			return;
		}

		// 내 소환수가 아니면 리턴
		if( static_cast< StructSummon* >( *it )->GetMaster() != pClient )
		{
			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
			return;
		}

		pCreature = *it;
	}

	int nSkillid = pMsg->skill_id;
	int nRegisterSkillid = pMsg->skill_id;
	int nBaseSkillLevel = 0;
	int nGroupID = 0;

	// nGroupID가 나오는 경우 
	// 1. 랜덤형 스킬을 처음 찍을 때 (skill_id = 랜덤원본, original_id = 0)
	// 2. 랜덤형 스킬을 레벨업 할 때 (skill_id = 랜덤찍힌스킬, original_id = 랜덤원본)
	if( pMsg->original_skill_id )
		nGroupID = GameContent::GetSkillTreeGroupID( pCreature, pMsg->original_skill_id );
	else
		nGroupID = GameContent::GetSkillTreeGroupID( pCreature, pMsg->skill_id );

	if( nGroupID )
	{
		// nSkillid가 검사할 스킬의 아이디가 된다. 
		// (랜덤형 스킬의 경우 그 실제 스킬을 검사하면 안되고 랜덤 원본 스킬을 검사해야함)
		if( pMsg->original_skill_id )
		{
			nSkillid = pMsg->original_skill_id;
		}

		if( !( pMsg->skill_level -1 ) )		//  처음 배우는 스킬
		{
			if( !pMsg->original_skill_id )
				nRegisterSkillid = GameContent::GetSummonRandomSkillID( nGroupID );

			if( !nRegisterSkillid )
			{
				SendResult( pClient, pMsg->id, RESULT_UNKNOWN );
				return;
			}

			// 같은 그룹 ID를 쓰는 스킬은 하나라도 배워서는 안된다. 단 레벨 0은 허용
			for( int i = 0 ; i < GameRule::MAX_RANDOM_SKILL_COUNT ; ++i )
			{
				StructSkill * pSkill = pCreature->GetSkill( GameContent::GetRandomSkillIDFromGroupID( nGroupID, i ) );
				if( pSkill )
				{
					if( pSkill->GetBaseSkillLevel() || !pMsg->original_skill_id )
					{
						// 이미 같은 랜덤그룹의 스킬을 배운 상태라 ERROR
						SendResult( pClient, pMsg->id, RESULT_UNKNOWN );
						return;
					}
				}
			}
		}
		else
		{
			StructSkill *pSkill = pCreature->GetSkill( pMsg->skill_id );
			if( NULL == pSkill )
			{
				SendResult( pClient, pMsg->id, RESULT_UNKNOWN );
				return;
			}
		}
	}

	nBaseSkillLevel = pCreature->GetBaseSkillLevel( nRegisterSkillid );

	// 클라에서 스킬 습득 메시지가 여러 번 오면 해당 상황에서 배워서는 안되는
	// 스킬 레벨까지 올라가 버리는 문제점을 막기 위해 습득 요청 레벨 체크
	// UI 마구 클릭한 것에 의한 것일 수 있으므로 출력 메시지는 없음
	if( pMsg->skill_level != nBaseSkillLevel + 1 )
		return;

	int nSkillTreeID = 0;

	// 배울수 없으면 KIN
	int nErrorCode = pCreature->IsLearnableSkill( nSkillid, nBaseSkillLevel+1, &nSkillTreeID );
	if( nErrorCode != RESULT_SUCCESS )
	{
		SendResult( pClient, pMsg->id, nErrorCode );
		return;
	}

	// 스킬 등록
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pCreature ) );

		// 등록 SkillID는 위에서 결정된 skill로 넘긴다.
		pCreature->RegisterSkill( nRegisterSkillid, nBaseSkillLevel+1, 0, nSkillTreeID );
		pClient->Save( true );
	}

	SendResult( pClient, pMsg->id, nErrorCode, nRegisterSkillid );
}

void onJobLevelUp( StructPlayer *pClient, TS_CS_JOB_LEVEL_UP *pMsg )
{
	StructCreature::iterator it = StructCreature::get( pMsg->target );
	StructCreature *pCreature = *it;

	if( !pCreature )	
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST, pMsg->target );
		return;
	}

	if( pCreature->IsPlayer() && pCreature != pClient )	
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED, pMsg->target );
		return;
	}

	if( pCreature->IsSummon() && static_cast< StructSummon* >( pCreature )->GetMaster() != pClient )	
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED, pMsg->target );
		return;
	}

	__int64 jp = GameContent::GetNeedJpForJobLevelUp( pCreature->GetJobLevel(), pCreature->GetJobDepth() );
	if( pCreature->GetJobPoint() < jp )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_JP, pMsg->target );
		return;
	}

	if( !jp )
	{
		SendResult( pClient, pMsg->id, RESULT_LIMIT_MAX, pMsg->target );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pCreature ) );

	// jp 감소
	pCreature->SetJP( pCreature->GetJobPoint() - jp );

	// jlv 증가
	pCreature->SetJobLevel( pCreature->GetJobLevel() + 1 );

	pCreature->CalculateStat();

	if( pCreature->IsPlayer() )
	{
		StructPlayer * pPlayer = static_cast< StructPlayer * >( pCreature );
		pPlayer->Save();
		LOG::Log11N4S( LM_CHARACTER_JOB_LEVEL_UP, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, pPlayer->GetJobId(), pPlayer->GetJobLevel(), jp, pPlayer->GetJobPoint(), 0, 0, 0, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

		// 캐릭터 잡 레벨업 시 가이드 메시지 출력 처리 추가
		LUA()->RunString( "on_player_level_joblevel_guide()" );
	}
	else if( pCreature->IsSummon() )
	{
		StructSummon * pSummon = static_cast< StructSummon * >( pCreature );
		StructPlayer * pPlayer = pSummon->GetMaster();

		LOG::Log11N4S( LM_CHARACTER_JOB_LEVEL_UP, pPlayer->GetAccountID(), pPlayer->GetSID(), pSummon->GetSID(), pSummon->GetJobId(), pSummon->GetJobLevel(), jp, pSummon->GetJobPoint(), 0, 0, 0, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}

	pClient->Save( true );

	SendPropertyMessage( pClient, pCreature->GetHandle(), "job_level", pCreature->GetJobLevel() );

	SendResult( pClient, pMsg->id, RESULT_SUCCESS, pMsg->target );
}

void onEquipSummon( StructPlayer *pClient, TS_EQUIP_SUMMON* pMsg )
{
	if( !pClient->IsItemUseable() )
		return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	pClient->EquipSummon( pMsg->card_handle );
}

void onSummon( StructPlayer *pClient, TS_CS_SUMMON* pMsg )
{
}

/*
struct TS_CS_BIND_SKILLCARD		: TS_MESSAGE
{
TS_CS_BIND_SKILLCARD()		_INIT(TM_CS_BIND_SKILLCARD)

AR_HANDLE		item_handle;
unsigned char	idx;			// 0이면 자기자신, 1+ 이라면 차례대로 소환수 슬롯에 있는 녀석들
};

struct TS_CS_UNBIND_SKILLCARD		: TS_MESSAGE
{
TS_CS_UNBIND_SKILLCARD()		_INIT(TM_CS_UNBIND_SKILLCARD)

AR_HANDLE		item_handle;
};
*/

void onBindSkillCard( StructPlayer *pClient, TS_CS_BIND_SKILLCARD* pMsg )
{
	// 아이템 없으면 KIN
	StructItem *pItem = StructItem::FindItem( pMsg->item_handle );
	if( !pItem || pItem->GetOwnerHandle() != pClient->GetHandle() || !pItem->IsSkillCard() || !pItem->IsInInventory() ) return;

	StructCreature *pTarget = NULL;

	// 이미 바인드 되어 있으면 해제
	if( pItem->GetBindedCreatureHandle() )
	{
		StructCreature::iterator it = StructCreature::get( pItem->GetBindedCreatureHandle() );
		if( (*it) ) (*it)->UnBindSkillCard( pItem );
	}

	if( IsPlayer( pMsg->target_handle ) )
	{
		if( pMsg->target_handle == pClient->GetHandle() )	pTarget = pClient;
	}
	else if( IsSummon( pMsg->target_handle ) )
	{
		StructCreature::iterator it = StructCreature::get( pMsg->target_handle );
		pTarget = *it;
		if( pTarget && static_cast< StructSummon* >( pTarget )->GetMaster() != pClient ) pTarget = NULL;
	}

	if( !pTarget ) return;

	const StructSkill *pSkill = pTarget->GetSkill( pItem->GetSkillId() );

	if( !pSkill ) return;

	// 해당 스킬에 다른 카드가 이미 장착되어있음
	if( pSkill->GetEnhance() ) return;
	
	pTarget->BindSkillCard( pItem );

}

void onUnBindSkillCard( StructPlayer *pClient, TS_CS_UNBIND_SKILLCARD* pMsg )
{
	// 아이템 없으면 KIN
	StructItem *pItem = StructItem::FindItem( pMsg->item_handle );
	if( !pItem || pItem->GetOwnerHandle() != pClient->GetHandle() || !pItem->IsSkillCard() || !pItem->IsInInventory() ) return;

	StructCreature *pTarget = NULL;

	if( IsPlayer( pMsg->target_handle ) )
	{
		if( pMsg->target_handle == pClient->GetHandle() )	pTarget = pClient;
	}
	else if( IsSummon( pMsg->target_handle ) )
	{
		StructCreature::iterator it = StructCreature::get( pMsg->target_handle );
		pTarget = *it;
		if( pTarget && static_cast< StructSummon* >( pTarget )->GetMaster() != pClient ) pTarget = NULL;
	}

	if( pItem->GetBindedCreatureHandle() == 0 ) return;

	if( pTarget ) 
	{
		assert( pTarget->GetHandle() == pItem->GetBindedCreatureHandle() );

		pTarget->UnBindSkillCard( pItem );
	}
}

void onQuery( StructPlayer *pClient, TS_CS_QUERY *pMsg )
{
	GameObject::iterator it = GameObject::get( pMsg->handle );

	if( *it && (*it)->IsInWorld() )
	{
		if( (*it)->GetLayer() != pClient->GetLayer() )
			return;

		if( !IsVisibleRegion( (*it)->GetRX(), (*it)->GetRY(), pClient->GetRX(), pClient->GetRY() ) )
			return;

		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( (*it) ) );

		SendEnterMsg( pClient, *it );
	}
}

void onTargeting( StructPlayer *pClient, TS_CS_TARGETING* pMsg )
{
	// TODO : 자신의 소환수 클릭시에는 보내줄 필요가 없음. (별도 관리 되어야함)

	if( pClient->GetTarget() )
	{
		StructCreature::iterator it = StructCreature::get( pClient->GetTarget() );
		StructCreature *pObj = *it;

		if( pObj )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pObj ) );
			pObj->RemoveAimer( pClient->GetHandle() );
		}

		pClient->SetTarget( 0 );
	}

	// 타게팅 취소 메세지거나 자기 자신이라면 KIN
	if( !pMsg->target || pMsg->target == pClient->GetHandle() ) return;

	StructCreature::iterator it = StructCreature::get( pMsg->target );
	StructCreature *pObj = *it;

	if( !pObj ) return;

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pObj ) );
		pObj->AddAimer( pClient->GetHandle() );

		pClient->SetTarget( pObj->GetHandle() );
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( pClient, pObj ) );

	if( !isValidTarget( pClient, pObj ) )
	{
		SendLeaveMsg( pClient, pObj );
		return;
	}
}

void onTrade( StructPlayer *pClient, TS_TRADE *pMsg );	// OnTrade.cpp 에 있음.

void onStorage( StructPlayer *pClient, TS_CS_STORAGE *pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	if( pMsg->mode == TS_CS_STORAGE::CLOSE && (pClient->IsUsingStorage() || pClient->IsLoadingStorage()) )
	{
		pClient->CloseStorage();
		return;
	}

	if( !pClient->IsUsingStorage() || !pClient->IsActable() || pClient->IsTrading() || pClient->IsUsingSkill() ) 
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE, pMsg->item_handle );
		return;
	}

	// 아템 이동일경우
	if( pMsg->mode == TS_CS_STORAGE::ITEM_INVENTORY_TO_STORAGE ||
		pMsg->mode == TS_CS_STORAGE::ITEM_STORAGE_TO_INVENTORY )
	{
		if( pMsg->count <= 0 )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY, pMsg->item_handle );
			return;
		}

		// 아이템 없으면 KIN
		StructItem *pItem = StructItem::FindItem( pMsg->item_handle );
		if( !pItem || pItem->GetOwnerHandle() != pClient->GetHandle() )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_EXIST, pMsg->item_handle );
			return;
		}

		if( pItem->GetOwnerHandle() != pClient->GetHandle() )
		{
			SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED, pMsg->item_handle );
			return;
		}

		if( pItem->IsInInventory() && pMsg->mode == TS_CS_STORAGE::ITEM_INVENTORY_TO_STORAGE )
		{
			if( pClient->GetStorageItemCount() >= static_cast< size_t >( GameRule::nMaxStorageItemCount ) &&
				( !pItem->IsJoinable() || !pClient->FindStorageItem( pItem->GetItemCode() ) || !pClient->FindStorageItem( pItem->GetItemCode() )->IsJoinable() ) )
			{
				SendResult( pClient, pMsg->id, RESULT_TOO_HEAVY, pMsg->item_handle );
				return;
			}

			pClient->MoveInventoryToStorage( pItem, pMsg->count );
		}

		if( pItem->IsInStorage() && pMsg->mode == TS_CS_STORAGE::ITEM_STORAGE_TO_INVENTORY )
		{
			pClient->MoveStorageToInventory( pItem, pMsg->count );
		}

		pClient->Save( true );

		return;
	}
	// 돈일경우
	else if( pMsg->mode == TS_CS_STORAGE::GOLD_INVENTORY_TO_STORAGE )
	{
		StructGold nGold			= pClient->GetGold();
		StructGold nStorageGold		= pClient->GetStorageGold();

		if( pMsg->count <= 0 )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY, pMsg->item_handle );
			return;
		}

		if( nGold < pMsg->count )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY, pMsg->item_handle );
			return;
		}

		if( nStorageGold + pMsg->count > GameRule::MAX_GOLD_FOR_STORAGE )
		{
			// TM_CS_STORAGE에 대해서는 TS_SC_RESULT 메시지 클라에서 사용 안 함
			// 그래서 서버에서 그냥 오류 메시지 출력 ㄱㄱ
			std::string strGold;
			XStringUtil::itosc64( strGold, pMsg->count );
			PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", pClient, "@575\v#@gold@#\v%s", strGold.c_str() );

			SendResult( pClient, pMsg->id, RESULT_TOO_MUCH_MONEY, pMsg->item_handle );
			return;
		}

		if( pClient->ChangeGold( nGold - pMsg->count ) != RESULT_SUCCESS || pClient->ChangeStorageGold( nStorageGold + pMsg->count ) != RESULT_SUCCESS )
		{
			// 돈 빼거나 넣는 중에 오류 발생하면 원래대로 되돌리고 오류 메시지 송신
			// 오류 발생시에는 변경되었을 가능성이 있는 데이터는 유저 소지금 뿐임
			if( pClient->ChangeGold( nGold ) != RESULT_SUCCESS )
			{
				// 원래대로 되돌리는 것도 실패하면 어쩌자는 거냐 -_ -
				_cprint( "ChangeGold/ChangeStorageGold Failed: Case[0], Player[%s], Info[Inven(%I64d), Storage(%I64d), Move(%I64d)]\n", pClient->GetName(), nGold.GetRawData(), nStorageGold.GetRawData(), pMsg->count );
				FILELOG( "ChangeGold/ChangeStorageGold Failed: Case[0], Player[%s], Info[Inven(%I64d), Storage(%I64d), Move(%I64d)]", pClient->GetName(), nGold.GetRawData(), nStorageGold.GetRawData(), pMsg->count );
			}

			SendResult( pClient, pMsg->id, RESULT_TOO_MUCH_MONEY, pMsg->item_handle );
			return;
		}

		pClient->Save( true );

		LOG::Log11N4S( LM_BANK,
			pClient->GetAccountID(), pClient->GetSID(),
			0, 0, 0, 0,
			0 - pMsg->count,
			pClient->GetGold().GetRawData(),
			0,
			pClient->GetStorageGold().GetRawData(),
			0,
			pClient->GetAccountName(),LOG::STR_NTS,
			pClient->GetName(), LOG::STR_NTS,
			"", 0,
			"GOLD_INVENTORY_TO_STORAGE", LOG::STR_NTS );

		return;
	}
	else if( pMsg->mode == TS_CS_STORAGE::GOLD_STORAGE_TO_INVENTORY )
	{
		StructGold nGold			= pClient->GetGold();
		StructGold nStorageGold		= pClient->GetStorageGold();

		if( pMsg->count <= 0 )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY, pMsg->item_handle );
			return;
		}

		if( nStorageGold < pMsg->count )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY, pMsg->item_handle );
			return;
		}

		if( nGold + pMsg->count > GameRule::MAX_GOLD_FOR_INVENTORY )
		{
			// TM_CS_STORAGE에 대해서는 TS_SC_RESULT 메시지 클라에서 사용 안 함
			// 그래서 서버에서 그냥 오류 메시지 출력 ㄱㄱ
			std::string strGold;
			XStringUtil::itosc64( strGold, pMsg->count );
			PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", pClient, "@576\v#@gold@#\v%s", strGold.c_str() );

			SendResult( pClient, pMsg->id, RESULT_TOO_MUCH_MONEY, pMsg->item_handle );
			return;
		}

		if( pClient->ChangeStorageGold( nStorageGold - pMsg->count ) != RESULT_SUCCESS || pClient->ChangeGold( nGold + pMsg->count ) != RESULT_SUCCESS )
		{
			// 돈 빼거나 넣는 중에 오류 발생하면 원래대로 되돌리고 오류 메시지 송신
			// 오류 발생시에는 변경되었을 가능성이 있는 데이터는 창고 보관액 뿐임
			if( pClient->ChangeStorageGold( nStorageGold ) != RESULT_SUCCESS )
			{
				// 원래대로 되돌리는 것도 실패하면 어쩌자는 거냐 -_ -
				_cprint( "ChangeGold/ChangeStorageGold Failed: Case[1], Player[%s], Info[Inven(%I64d), Storage(%I64d), Move(%I64d)]\n", pClient->GetName(), nGold.GetRawData(), nStorageGold.GetRawData(), pMsg->count );
				FILELOG( "ChangeGold/ChangeStorageGold Failed: Case[1], Player[%s], Info[Inven(%I64d), Storage(%I64d), Move(%I64d)]", pClient->GetName(), nGold.GetRawData(), nStorageGold.GetRawData(), pMsg->count );
			}

			SendResult( pClient, pMsg->id, RESULT_TOO_MUCH_MONEY, pMsg->item_handle );
			return;
		}

		pClient->Save( true );

		LOG::Log11N4S( LM_BANK,
			pClient->GetAccountID(), pClient->GetSID(),
			0, 0, 0, 0,
			pMsg->count,
			pClient->GetGold().GetRawData(),
			0,
			pClient->GetStorageGold().GetRawData(),
			0,
			pClient->GetAccountName(), LOG::STR_NTS,
			pClient->GetName(), LOG::STR_NTS,
			"", 0,
			"GOLD_STORAGE_TO_INVENTORY", LOG::STR_NTS );
		// TODO : 플레이어 돈 즉시 업데 해야함 (시간차를 두면 섭따로 인한 창고 돈복사 일어남)

		return;
	}
}

void onReturnLobby( IStreamSocketConnection * pConn, StructPlayer *pClient )
{
	// 즉시 로그아웃 불가능할 경우 로그아웃 불가
	if( !pClient->CanLogoutNow() )
		return;

	{
		THREAD_SYNCRONIZE( g_ConnectionTagLock );

		_CONNECTION_TAG * pTag = reinterpret_cast< _CONNECTION_TAG * > ( pConn->GetTag() );

		if( pTag && pTag->pPlayer )
		{
			pTag->nConnId = pTag->pPlayer->SetReturnLobbyConnection( static_cast< XIOCPConnection * >( pConn ) );
			pTag->nContinuousPlayTime = pTag->pPlayer->GetContinuousPlayTime();
			pTag->nContinuousLogoutTime = pTag->pPlayer->GetContinuousLogoutTime();
			pTag->pPlayer = NULL;
		}
	}

	if( pClient->GetBattleArenaID() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

		// 특정 경기에 참여 상태일 경우에는 경기 이탈
		if( pClient->GetBattleArenaID() )
			BattleArenaManager::Instance().QuitGame( pClient, false, false, ALT_USER_REQUEST );

		assert( !pClient->GetBattleArenaID() );
	}

	pClient->LogoutNow( 2 );
}

void checkBoothStartable( StructPlayer * pClient, unsigned short & nResult, int & nErrorValue )
{
	if( pClient->GetLevel() < GameRule::nMinBoothStartableLevel )
	{
		nResult = RESULT_NOT_ENOUGH_LEVEL;
		nErrorValue = GameRule::nMinBoothStartableLevel;
	}
	else if( ( GameRule::bLimitBoothOpenableLayerToZero && pClient->GetLayer() > 0 ) || GameRule::bDisableBooth )
	{
		nResult = RESULT_NOT_ACTABLE_HERE;
		nErrorValue = 0;
	}
	else
	{
		nResult = pClient->IsBoothOpenable();
		nErrorValue = 0;
	}
}

void onCheckBoothStartable( StructPlayer *pClient )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	unsigned short nResult = RESULT_SUCCESS;
	int nErrorValue = 0;

	checkBoothStartable( pClient, nResult, nErrorValue );

	SendResult( pClient, TM_CS_CHECK_BOOTH_STARTABLE, nResult, nErrorValue );
}

void onStartBooth( StructPlayer *pClient, TS_CS_START_BOOTH* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	unsigned short nResult = RESULT_SUCCESS;
	int nErrorValue = 0;

	checkBoothStartable( pClient, nResult, nErrorValue );

	if( nResult != RESULT_SUCCESS )
	{
		SendResult( pClient, TM_CS_START_BOOTH, nResult, nErrorValue );
		return;
	}

	pMsg->name[ _countof( pMsg->name ) - 1 ] = '\0';
	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( strlen( pMsg->name ) < 1 ||
		( GameRule::bRestrictBanWordForBooth && GameContent::IsBannedWord( code_page, pMsg->name ) ) )
	{
		SendResult( pClient, TM_CS_START_BOOTH, RESULT_INVALID_TEXT );
		return;
	}

	std::vector< StructPlayer::BOOTH_OPEN_ITEM_INFO > vItemList;

	bool bIsBuyBooth = (pMsg->type == 2);

	if( bIsBuyBooth && GameRule::bDisableBuyBooth )
	{
		SendResult( pClient, TM_CS_START_BOOTH, RESULT_NOT_ACTABLE );
		return;
	}

	if( pMsg->cnt > GameRule::MAX_BOOTH_ITEM_COUNT )
	{
		SendResult( pClient, TM_CS_START_BOOTH, RESULT_LIMIT_MAX );
		return;
	}

	TS_CS_START_BOOTH::TS_BOOTH_OPEN_ITEM_INFO* pItemInfo = reinterpret_cast< TS_CS_START_BOOTH::TS_BOOTH_OPEN_ITEM_INFO* > ( pMsg + 1 );

	for( int i = 0; i < pMsg->cnt; ++i )
	{
		StructPlayer::BOOTH_OPEN_ITEM_INFO ItemInfo;

		ItemInfo.handle = pItemInfo->item_handle;
		ItemInfo.cnt = pItemInfo->cnt;
		ItemInfo.gold = pItemInfo->gold;

		vItemList.push_back( ItemInfo );

		++pItemInfo;
	}

	bool result = false;
	{
		THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

		result = pClient->OpenBooth( pMsg->name, vItemList, bIsBuyBooth );
	}

	if( !result )
	{
		SendResult( pClient, TM_CS_START_BOOTH, RESULT_NOT_ACTABLE );
		return;
	}

	if( !pClient->IsSitDown() )
		pClient->SitDown();

	BroadcastStatusMessage( pClient );	

	SendResult( pClient, TM_CS_START_BOOTH, RESULT_SUCCESS );

	BroadcastStatusMessage( pClient );
}

void onStopBooth( StructPlayer *pClient )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	bool result = false;

	THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

	if( !pClient->IsBoothOpen() )
		return;

	result = pClient->CloseBooth();

	if( !result )
	{
		SendResult( pClient, TM_CS_STOP_BOOTH, RESULT_NOT_ACTABLE );
		return;
	}

	// 일어서자
	pClient->StandUp();
	BroadcastStatusMessage( pClient );

	SendResult( pClient, TM_CS_STOP_BOOTH, RESULT_SUCCESS );
	BroadcastStatusMessage( pClient );
}

void onWatchBooth( StructPlayer *pClient, TS_CS_WATCH_BOOTH* pMsg )
{
	THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

	StructCreature::iterator it = StructCreature::get( pMsg->target );

	StructPlayer * pTarget = static_cast< StructPlayer * >( *it );

	if( !pTarget ) return;

	if( !( pTarget->IsPlayer() ) )
		return;

	if( pClient->GetBoothStatus() != StructPlayer::IS_NOT_BOOTH )
	{
		return;
	}

	pClient->WatchBooth( pTarget );

	SendBoothInfo( pClient, pTarget );
}

void onStopWatchBooth( StructPlayer * pClient, TS_CS_STOP_WATCH_BOOTH* pMsg )
{
	THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

	pClient->StopWatchBooth();
}

void onBuyFromBooth( StructPlayer * pClient, TS_CS_BUY_FROM_BOOTH* pMsg )
{
	StructCreature::iterator itPlayer = StructCreature::get( pMsg->target );

	StructPlayer * pTarget = static_cast< StructPlayer * >( *itPlayer );

	if( !pTarget ) return;

	if( !( pTarget->IsPlayer() ) )
		return;

	// 락하고~
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( pClient, pTarget ) );

	THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

	// 판매 부스 아니면 KIN
	if( pTarget->GetBoothStatus() != StructPlayer::SELL_BOOTH )
	{
		SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_ACTABLE );
		return;
	}

	// 카운트 검사
	if( pMsg->cnt > GameRule::MAX_BOOTH_ITEM_COUNT )
	{
		SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_ACTABLE );
		return;
	}

	const std::vector< StructPlayer::BOOTH_ITEM_INFO > & rvItemList = pTarget->GetBoothItemList();
	std::vector< StructPlayer::BOOTH_ITEM_INFO >::const_iterator it;
	std::vector< StructPlayer::BOOTH_ITEM_BUY_INFO > vBuyInfo;

	TS_ITEM_BASE_INFO * pItemInfo = reinterpret_cast< TS_ITEM_BASE_INFO * >( pMsg + 1 );
	StructPlayer::BOOTH_ITEM_BUY_INFO ItemBuyInfo;

	// 결과 송신 메시지 버퍼 미리 준비: 루프 돌면서 가격 정보 채워야 함.
	char buf[ sizeof( TS_SC_BOOTH_TRADE_INFO ) + GameRule::MAX_BOOTH_ITEM_COUNT * sizeof( TS_BOOTH_TRADE_ITEM_INFO ) ];

	TS_SC_BOOTH_TRADE_INFO * pResultMsg = reinterpret_cast< TS_SC_BOOTH_TRADE_INFO * >( buf );

	pResultMsg->id = TM_SC_BOOTH_TRADE_INFO;
	pResultMsg->size = sizeof( TS_SC_BOOTH_TRADE_INFO ) + pMsg->cnt * sizeof( TS_BOOTH_TRADE_ITEM_INFO );
	pResultMsg->cnt = pMsg->cnt;

	TS_BOOTH_TRADE_ITEM_INFO *pTradeResultInfo = reinterpret_cast< TS_BOOTH_TRADE_ITEM_INFO * >( pResultMsg + 1 );
	int remain_buf_len = sizeof( buf ) - sizeof( TS_SC_BOOTH_TRADE_INFO );

	StructGold nTotalCost( 0 );
	int nTotalWeight = 0;
	// { 여기서부터, 아이템 존내 검증. 판매자나 구매자가 사기쳤는지 확인
	for( int i = 0; i < pMsg->cnt; ++i )
	{
		int nItemIdx = -1;

		for( it = rvItemList.begin(); it != rvItemList.end(); ++it )
		{
			if( (*it).pItem->GetHandle() == pItemInfo->handle ) break;
		}

		// 아이템이 없음
		if( it == rvItemList.end() )
		{
			SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_EXIST );
			return;
		}

		// 구매 수량 변수 오버플로우 체크(실제 개수는 int 타입 한계치여야 하는데 TS_ITEM_BASE_INFO::count가 __int64여서 받을 수 있음)
		if( pItemInfo->count > 0x7FFFFFFF || pItemInfo->count <= 0 )
		{
			SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_EXIST );
			return;
		}

		// 틀린 아이템
		if( pItemInfo->Code != (*it).pItem->GetItemCode() ||
			pItemInfo->count > (*it).cnt ||
			pItemInfo->count > (*it).pItem->GetCount() ||
			pItemInfo->endurance != (*it).pItem->GetCurrentEndurance() ||
			pItemInfo->enhance != (*it).pItem->GetItemEnhance() ||
			pItemInfo->Flag != *((*it).pItem->GetInstanceFlag().GetRawData()) ||
			pItemInfo->level != (*it).pItem->GetItemLevel() ||
			pItemInfo->elemental_effect_type != (*it).pItem->GetElementalEffectType() ||
			pItemInfo->elemental_effect_attack_point != (*it).pItem->GetElementalEffectAttackPoint() ||
			pItemInfo->elemental_effect_magic_point != (*it).pItem->GetElementalEffectMagicPoint() ||
			pItemInfo->appearance_code != (*it).pItem->GetAppearanceCode() ||
			pItemInfo->summon_code != (*it).pItem->GetSummonCode() )
		{
			SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_EXIST );
			return;
		}

		// 소켓이 다름
		// 소환수 카드의 경우 소켓 정보에 크리처 진화 렙 정보를 포함시켜줘야 함
		int ItemSocket[ ItemBase::MAX_SOCKET_NUMBER ];
		StructSummon *pSummon = (*it).pItem->GetSummonStruct();
		for( unsigned  i = 0; i < ItemBase::MAX_SOCKET_NUMBER; ++i )
			ItemSocket[i] = (*it).pItem->GetSocketCode( i );
		if( (*it).pItem->IsSummonCard() && pSummon )
		{
			int depth = pSummon->GetTransformLevel();
			int i;
			for( i = 0; i < depth - 1; ++i )
			{
				ItemSocket[i + 1] = pSummon->GetPrevJobLevel( i );
			}
			ItemSocket[i + 1] = pSummon->GetLevel();
		}
		// 소켓에 필요한 정보는 ItemSocket 배열에 모드 들어갔음.

		for( unsigned i = 0; i < ItemBase::MAX_SOCKET_NUMBER; ++i )
		{
			//if( pItemInfo->socket[i] != (*it).pItem->GetSocketCode( i ) )
			if( pItemInfo->socket[i] != ItemSocket[i] )
			{
				SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_EXIST );
				return;
			}
		}

		// 거래 불가능 상태
		if( !pTarget->IsTradable( (*it).pItem ) )
		{
			SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_ACTABLE );
			return;
		}

		nItemIdx = it - rvItemList.begin();

		ItemBuyInfo.nIdx = nItemIdx;
		ItemBuyInfo.nCount = pItemInfo->count;

		nTotalCost += StructGold( (*it).gold * pItemInfo->count );
		nTotalWeight += (*it).pItem->GetItemBase().fWeight * pItemInfo->count;

		s_memcpy( pTradeResultInfo, remain_buf_len, pItemInfo, sizeof( TS_ITEM_BASE_INFO ) );
		remain_buf_len -= sizeof( TS_ITEM_BASE_INFO );
		pTradeResultInfo->price = (*it).gold * pItemInfo->count;

		if( (*it).gold * pItemInfo->count < 0 )
		{
			FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Buy Booth Bug [%s:%s]", pTarget->GetAccountName(), pTarget->GetName() );

			GameRule::RegisterBlockAccount( pTarget->GetAccountName() );

			SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_ENOUGH_MONEY );

			if( pClient->pConnection && pClient->pConnection->IsConnected() )
				pClient->pConnection->Close();

			return;
		}

		if( pClient->GetGold() < StructGold( (*it).gold * pItemInfo->count ) )
		{
			SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_ENOUGH_MONEY );
			return;
		}

		vBuyInfo.push_back( ItemBuyInfo );

		++pItemInfo;
		++pTradeResultInfo;
	}
	// }

	if( nTotalCost < StructGold( 0 ) )
	{
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Buy Booth Bug [%s:%s]", pTarget->GetAccountName(), pTarget->GetName() );

		GameRule::RegisterBlockAccount( pTarget->GetAccountName() );

		SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_ENOUGH_MONEY );

		if( pClient->pConnection && pClient->pConnection->IsConnected() )
			pClient->pConnection->Close();

		return;
	}

	if( nTotalCost > pClient->GetGold() )
	{
		SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_ENOUGH_MONEY );
		return;
	}

	// 판매자 루피 소지 한도 체크
	if( pTarget->GetGold() + nTotalCost > GameRule::MAX_GOLD_FOR_INVENTORY )
	{
		SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_TOO_MUCH_MONEY, pTarget->GetHandle() );
		PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", pTarget, "@565" );
		return;
	}

	if( nTotalWeight + pClient->GetWeight() > pClient->GetMaxWeight() * 0.9f )
	{
		SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_TOO_HEAVY );
		return;
	}

	if( !pTarget->BuyFromBooth( pClient, vBuyInfo ) )
	{
		SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_NOT_EXIST );
		return;
	}

	SendResult( pClient, TM_CS_BUY_FROM_BOOTH, RESULT_SUCCESS );

	// 거래 결과 메시지 송신
	pResultMsg->target = pClient->GetHandle();
	pResultMsg->is_sell = true;

	PendStream( pTarget, pResultMsg, pResultMsg->size );

	pResultMsg->target = pTarget->GetHandle();
	pResultMsg->is_sell = false;

	PendStream( pClient, pResultMsg, pResultMsg->size );
}

void onSellToBooth( StructPlayer * pClient, TS_CS_SELL_TO_BOOTH* pMsg )
{
	StructCreature::iterator itPlayer = StructCreature::get( pMsg->target );

	StructPlayer * pTarget = static_cast< StructPlayer * >( *itPlayer );

	if( !pTarget ) return;

	if( !( pTarget->IsPlayer() ) )
		return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( pClient, pTarget ) );

	THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

	if( pTarget->GetBoothStatus() != StructPlayer::BUY_BOOTH )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_NOT_ACTABLE );
		return;
	}

	StructItem * pItem = StructItem::FindItem( pMsg->item_handle );

	if( !pItem )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_NOT_ACTABLE );
		return;
	}

	if( pItem->GetOwnerHandle() != pClient->GetHandle() || !pItem->IsInInventory() )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_NOT_EXIST );
		return;
	}

	if( !pClient->IsTradable( pItem ) )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_NOT_ACTABLE );
		return;
	}

	if( pMsg->cnt <= 0 )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_NOT_EXIST );
		return;
	}

	const std::vector< StructPlayer::BOOTH_ITEM_INFO > & rvItemList = pTarget->GetBoothItemList();
	std::vector< StructPlayer::BOOTH_ITEM_INFO >::const_iterator it;

	int nItemIdx = -1;
	StructGold nTotalCost( 0 );
	int nWeight = 0;
	AR_HANDLE hBuyHandle = 0;

	// { 아이템 찾기
	for( it = rvItemList.begin(); it != rvItemList.end(); ++it )
	{
		if( pItem->GetItemCode() != (*it).pItem->GetItemCode() ||
			pMsg->cnt > (*it).cnt ||
			pItem->GetCount() < pMsg->cnt ||
			pItem->GetCurrentEndurance() != (*it).pItem->GetCurrentEndurance() ||
			pItem->GetItemEnhance() != (*it).pItem->GetItemEnhance() ||
			*(pItem->GetInstanceFlag().GetRawData()) != *((*it).pItem->GetInstanceFlag().GetRawData()) ||
			pItem->GetItemLevel() != (*it).pItem->GetItemLevel() ||
			pItem->GetSummonCode() != (*it).pItem->GetSummonCode() )
		{
			continue;
		}

		bool bSocketVerify = true;
		for( unsigned i = 0; i < ItemBase::MAX_SOCKET_NUMBER; ++i )
		{
			if(  pItem->GetSocketCode( i ) != (*it).pItem->GetSocketCode( i ) )
			{
				bSocketVerify = false;
				break;
			}
		}

		if( !bSocketVerify )
			continue;

		nTotalCost += (*it).gold * pMsg->cnt;
		nWeight = (*it).pItem->GetItemBase().fWeight * pMsg->cnt;
		nItemIdx = it - rvItemList.begin();
		hBuyHandle = (*it).pItem->GetHandle();
		break;
	}

	if( nItemIdx == -1 )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_NOT_EXIST );
		return;
	}

	if( pTarget->GetWeight() + nWeight > pTarget->GetMaxWeight() * 0.9f )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_TOO_HEAVY );
		return;
	}

	if( nTotalCost < StructGold( 0 ) )
	{
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Sell Booth Bug [%s:%s]", pTarget->GetAccountName(), pTarget->GetName() );

		GameRule::RegisterBlockAccount( pTarget->GetAccountName() );

		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_NOT_ENOUGH_MONEY );

		if( pClient->pConnection && pClient->pConnection->IsConnected() )
			pClient->pConnection->Close();

		return;
	}

	if( pTarget->GetGold() < nTotalCost )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_NOT_ENOUGH_MONEY );
		return;
	}

	// 판매자 루피 소지 한도 체크
	if( pClient->GetGold() + nTotalCost > GameRule::MAX_GOLD_FOR_INVENTORY )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_TOO_MUCH_MONEY, pTarget->GetHandle() );
		PrintfChatMessage( false, CHAT_NOTICE, "@SYSTEM", pTarget, "@573\v#@target_name@#\v%s", pClient->GetName() );
		return;
	}

	TS_ITEM_BASE_INFO ItemInfo;

	fillItemBaseInfo( &ItemInfo, pItem );

	ItemInfo.count = pMsg->cnt;
	ItemInfo.handle = hBuyHandle;

	if( !pTarget->SellToBooth( pClient, pMsg->item_handle, nItemIdx, pMsg->cnt ) )
	{
		SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_NOT_EXIST );
		return;
	}

	SendResult( pClient, TM_CS_SELL_TO_BOOTH, RESULT_SUCCESS );

	char buf[ sizeof( TS_SC_BOOTH_TRADE_INFO ) + sizeof( TS_BOOTH_TRADE_ITEM_INFO ) ];

	TS_SC_BOOTH_TRADE_INFO * pResultMsg = reinterpret_cast< TS_SC_BOOTH_TRADE_INFO * >( buf );

	pResultMsg->id = TM_SC_BOOTH_TRADE_INFO;
	pResultMsg->size = sizeof( TS_SC_BOOTH_TRADE_INFO ) + sizeof( TS_BOOTH_TRADE_ITEM_INFO );
	pResultMsg->cnt = 1;

	s_memcpy( pResultMsg + 1, sizeof( buf ) - sizeof( TS_SC_BOOTH_TRADE_INFO ), &ItemInfo, sizeof( TS_ITEM_BASE_INFO ) );
	reinterpret_cast< TS_BOOTH_TRADE_ITEM_INFO * >( pResultMsg + 1 )->price = nTotalCost.GetRawData();

	pResultMsg->target = pClient->GetHandle();
	pResultMsg->is_sell = false;

	PendStream( pTarget, pResultMsg, pResultMsg->size );

	pResultMsg->target = pTarget->GetHandle();
	pResultMsg->is_sell = true;

	PendStream( pClient, pResultMsg, pResultMsg->size );
}

void onSoulStoneCraft( StructPlayer* pClient, TS_CS_SOULSTONE_CRAFT* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( !pClient->GetLastContactLong( "SoulStoneCraft" ) )
		return;

	StructItem *pCraftItem = StructItem::FindItem( pMsg->craft_item_handle );
	StructItem *pSoulStoneList[ TS_CS_SOULSTONE_CRAFT::MAX_SOULSTONE_NUM ];
	memset( pSoulStoneList, 0, sizeof( pSoulStoneList ) );

	// 아이템 존재/소지 여부 체크
	if( !pCraftItem || pCraftItem->GetOwnerHandle() != pClient->GetHandle() || !pCraftItem->IsInInventory() )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	// 강화 실패작은 세공 불가능
	if( pCraftItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) )
	{
		return;
	}

	// 소울스톤 세공 가능한 아이템인지 체크, 기획상으로 벨트는 세공이 불가능
	int nSocketCount = pCraftItem->GetMaxSocketCount();
	if( pCraftItem->IsBelt() || !nSocketCount || nSocketCount > TS_CS_SOULSTONE_CRAFT::MAX_SOULSTONE_NUM )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	LOG::Log11N4S( LM_ITEM_SOCKET_INFO, pClient->GetAccountID(), pClient->GetSID(), pCraftItem->GetCurrentEndurance(), pCraftItem->GetMaxEndurance(), pCraftItem->GetSocketCode( 0 ), pCraftItem->GetSocketCode( 1 ), pCraftItem->GetSocketCode( 2 ), pCraftItem->GetSocketCode( 3 ), pCraftItem->GetSocketCode( 4 ), pCraftItem->GetSocketCode( 5 ), pCraftItem->GetItemUID(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );

	// 최대 중복 가능 소울스톤 종류 수 한정(소켓이 3개 이상인 건 2개까지 가능)
	int nMaxReplicatableCount = ( pCraftItem->GetMaxSocketCount() >= 3 ) ? 2 : 1;
	int nReplicatedCount = 0;
	StructGold nPrevGold = pClient->GetGold();

	// 세공에 소요되는 루피
	StructGold nCraftCost( 0 );
	// 1개라도 새로 세공되는 소울스톤이 있는지(없으면 오류)
	bool bValidCraft = false;

	bool bNeedUpdate = false;
	int nErrorCode = RESULT_SUCCESS;
	for( int i = 0 ; i < nSocketCount ; ++i )
	{
		if( !pMsg->soulstone_handle[i] )
		{
			pSoulStoneList[i] = NULL;
			continue;
		}

		pSoulStoneList[i] = StructItem::FindItem( pMsg->soulstone_handle[i] );

		// 아이템 존재 여부와 소유 여부 체크
		if( !pSoulStoneList[i] || pSoulStoneList[i]->GetOwnerHandle() != pClient->GetHandle() || !pSoulStoneList[i]->IsInInventory() )
		{
			nErrorCode = RESULT_ACCESS_DENIED;
			break;
		}

		// 소울스톤 분류인지 체크 추가 필요
		if( pSoulStoneList[i]->GetItemBase().nType != ItemBase::TYPE_SOULSTONE ||
			pSoulStoneList[i]->GetItemBase().nGroup != ItemBase::GROUP_SOULSTONE ||
			pSoulStoneList[i]->GetItemBase().nClass != ItemBase::CLASS_SOULSTONE )
		{
			nErrorCode = RESULT_NOT_ACTABLE;
			break;
		}

		static const size_t nTypeSize = sizeof( short ) * ItemBase::MAX_OPTION_NUMBER;
		static const size_t nBaseVarSize = sizeof( double ) * ItemBase::MAX_OPTION_NUMBER;
		static const size_t nOptVarSize = sizeof( double ) * ItemBase::MAX_OPTION_NUMBER;

		// 같은 효과의 소울스톤이 제한 개수보다 많이 세공되어 있으면 사용 불가
		nReplicatedCount = 0;

		for( int k = 0 ; k < pCraftItem->GetMaxSocketCount() ; ++k )
		{
			ItemBase::ItemCode nSoulStoneCode = pCraftItem->GetSocketCode( k );

			if( !nSoulStoneCode || k == i )
				continue;

			if( !memcmp( StructItem::GetItemBase( nSoulStoneCode ).nBaseType, pSoulStoneList[i]->GetItemBase().nBaseType, nTypeSize ) &&
				!memcmp( StructItem::GetItemBase( nSoulStoneCode ).fBaseVar1, pSoulStoneList[i]->GetItemBase().fBaseVar1, nBaseVarSize ) &&
				!memcmp( StructItem::GetItemBase( nSoulStoneCode ).nOptType, pSoulStoneList[i]->GetItemBase().nOptType, nTypeSize ) &&
				!memcmp( StructItem::GetItemBase( nSoulStoneCode ).fOptVar1, pSoulStoneList[i]->GetItemBase().fOptVar1, nOptVarSize ) )
			{
				++nReplicatedCount;
				if( nReplicatedCount >= nMaxReplicatableCount )
				{
					nErrorCode = RESULT_ALREADY_EXIST;
					break;
				}
			}
		}

		if( nErrorCode != RESULT_SUCCESS )
			break;

		// 같은 효과의 소울스톤이 함께 세공되는 경우 불가
		for( int k = 0 ; k < i ; ++k )
		{
			if( !pSoulStoneList[k] )
				continue;

			if( !memcmp( pSoulStoneList[k]->GetItemBase().nBaseType, pSoulStoneList[i]->GetItemBase().nBaseType, nTypeSize ) &&
				!memcmp( pSoulStoneList[k]->GetItemBase().fBaseVar1, pSoulStoneList[i]->GetItemBase().fBaseVar1, nBaseVarSize ) &&
				!memcmp( pSoulStoneList[k]->GetItemBase().nOptType, pSoulStoneList[i]->GetItemBase().nOptType, nTypeSize ) &&
				!memcmp( pSoulStoneList[k]->GetItemBase().fOptVar1, pSoulStoneList[i]->GetItemBase().fOptVar1, nOptVarSize ) )
			{
				++nReplicatedCount;
				if( nReplicatedCount > nMaxReplicatableCount )
				{
					nErrorCode = RESULT_ALREADY_EXIST;
					break;
				}
			}
		}

		if( nErrorCode != RESULT_SUCCESS )
			break;

		// 세공 비용 처리
		nCraftCost = pSoulStoneList[i]->GetItemBase().nPrice / StructGold( 10 );		// 기존 세공 자리에 세공하는데 필요 비용
		if( pClient->ChangeGold( pClient->GetGold() - nCraftCost ) != RESULT_SUCCESS )
		{
			nErrorCode = RESULT_NOT_ENOUGH_MONEY;
			break;
		}

		// 소켓에 세공 처리하고 소울스톤 삭제
		bNeedUpdate = true;
		pCraftItem->SetSocketCode( i, pSoulStoneList[i]->GetItemCode() );
		pClient->EraseItem( pSoulStoneList[i], 1 );
		pCraftItem->SetCurrentEndurance( pCraftItem->GetCurrentEndurance() + pSoulStoneList[i]->GetCurrentEndurance() );

		LOG::Log11N4S( LM_ITEM_SOULSTONE_CRAFT, pClient->GetAccountID(), pClient->GetSID(), pCraftItem->GetItemEnhance() * 100 + pCraftItem->GetItemLevel(), pCraftItem->GetItemCode(), 
			( pSoulStoneList[0] ) ? pSoulStoneList[0]->GetItemCode() : 0, ( pSoulStoneList[1] ) ? pSoulStoneList[1]->GetItemCode() : 0,
			( pSoulStoneList[2] ) ? pSoulStoneList[2]->GetItemCode() : 0, ( pSoulStoneList[3] ) ? pSoulStoneList[3]->GetItemCode() : 0,
			nPrevGold.GetRawData(), pClient->GetGold().GetRawData() - nCraftCost.GetRawData(), pCraftItem->GetItemUID(),
			pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "SUCS", 0 );
	}

	if( bNeedUpdate )
	{
		pCraftItem->DBQuery( new DB_UpdateItem( pCraftItem ) );
		LOG::Log11N4S( LM_ITEM_SOCKET_INFO, pClient->GetAccountID(), pClient->GetSID(), pCraftItem->GetCurrentEndurance(), pCraftItem->GetMaxEndurance(), pCraftItem->GetSocketCode( 0 ), pCraftItem->GetSocketCode( 1 ), pCraftItem->GetSocketCode( 2 ), pCraftItem->GetSocketCode( 3 ), pCraftItem->GetSocketCode( 4 ), pCraftItem->GetSocketCode( 5 ), pCraftItem->GetItemUID(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		pClient->SetLastContact( "SoulStoneCraft", 0 );
		SendItemMessage( pClient, pCraftItem );
		pClient->CalculateStat();
	}

	SendResult( pClient, pMsg->id, nErrorCode );
}

void onRepairSoulStone( StructPlayer* pClient, TS_CS_REPAIR_SOULSTONE* pMsg )
{
	// 시간제 아이템이 중간에 사라지는 것을 방지해야 함
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( !pClient->GetLastContactLong( "RepairSoulStone" ) )
		return;

	pClient->SetLastContact( "RepairSoulStone", 0 );

	typedef std::pair< StructItem *, int > REPAIR_INFO;
	std::vector< REPAIR_INFO > vItemList;

	int nTotalRepair = 0;
	for( int i = 0 ; i < TS_CS_REPAIR_SOULSTONE::MAX_REPAIR_COUNT ; ++i )
	{
		if( !pMsg->item_handle[i] )
			continue;

		StructItem *pItem = StructItem::FindItem( pMsg->item_handle[i] );

		// 아이템 존재/소유 여부 체크
		if( !pItem || pItem->GetOwnerHandle() != pClient->GetHandle() || !pItem->IsInInventory() )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
			return;
		}

		// 강화 실패작은 세공 수리 불가
		if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
			return;
		}

		// 소울스톤 세공 여부 체크
		if( pItem->GetUsingSocketCount() == 0 )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
			return;
		}

		int nDamagedEndurance = ( pItem->GetMaxEndurance() / 100000 ) - ( pItem->GetCurrentEndurance() + 99999 ) / 100000;

		// 내구도 소모 여부 체크
		if( nDamagedEndurance == 0 )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
			return;
		}

		// 수리 수치 총합 계산(소모 라크 계산 용)
		nTotalRepair += nDamagedEndurance;

		vItemList.push_back( REPAIR_INFO( pItem, nDamagedEndurance ) );
	}

	// 총 소모 라크는 수리 대상 아이템을 모두 합친 거 기준으로 올림 처리(각각 올림 처리 X)
	int nCostChaos = ( nTotalRepair * GameRule::COST_CHAOS_PER_PEPAIR_POINT ) + 0.9;

	if( vItemList.empty() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	// 라크가 모자라~
	if( pClient->GetChaos() < nCostChaos )
	{
		// 1개만 수리할 때에는 라크가 부족하면 라크로 채울 수 있는 만큼만...
		// 그 이상 한 번에 할 때는 오류 처리
		if( vItemList.size() > 1 )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY );
			return;
		}

		nTotalRepair = pClient->GetChaos() / 1.2;

		// 소지 중인 라크로는 1포인트도 회복 못할 경우
		if( nTotalRepair == 0 )
		{
			SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_MONEY );
			return;
		}

		StructItem *pItem = (*vItemList.begin()).first;

		int nPrevEndurance = pItem->GetCurrentEndurance();

		pItem->SetCurrentEndurance( pItem->GetCurrentEndurance() + nTotalRepair * 100000 );
		SendItemMessage( pClient, pItem );
		nCostChaos = pClient->GetChaos();

		LOG::Log11N4S( LM_ITEM_SOULSTONE_REPAIR, pClient->GetAccountID(), pClient->GetSID(), pClient->GetChaos(), pClient->GetChaos() - nCostChaos, nPrevEndurance, pItem->GetCurrentEndurance(), pItem->GetMaxEndurance(), 0, 0, 0, pItem->GetItemUID(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}
	// 라크 안 모자라~(100% 수리)
	else
	{
		std::vector< REPAIR_INFO >::iterator it;

		for( it = vItemList.begin() ; it != vItemList.end() ; ++it )
		{
			StructItem *pItem = (*it).first;
			int nPrevEndurance = pItem->GetCurrentEndurance();
			pItem->SetCurrentEndurance( pItem->GetCurrentEndurance() + (*it).second * 100000 );
			SendItemMessage( pClient, pItem );

			LOG::Log11N4S( LM_ITEM_SOULSTONE_REPAIR, pClient->GetAccountID(), pClient->GetSID(), pClient->GetChaos(), pClient->GetChaos() - nCostChaos, nPrevEndurance, pItem->GetCurrentEndurance(), pItem->GetMaxEndurance(), 0, 0, 0, pItem->GetItemUID(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		}
	}

	pClient->AddChaos( -1 * nCostChaos );

	pClient->CalculateStat();

	pClient->Save( true );

	SendResult( pClient, pMsg->id, RESULT_SUCCESS, ( vItemList.size() == 1 && nCostChaos < nTotalRepair ) ? nCostChaos : 0 );
}

void onTransmitEtherealDurability( StructPlayer *pClient, TS_CS_TRANSMIT_ETHEREAL_DURABILITY* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructItem *pItem = static_cast< StructItem * >( GameObject::raw_get( pMsg->handle ) );

	if( !pItem )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
		return;
	}

	// 툴팁에서 보여지는 단위로 처리하되, 에테리얼 스톤에 충전된 양보다 10000만큼 적은 양만 사용할 수 있다.
	// 장비 아이템 회복 만을 위한 패킷이므로 회복율은 100%, 기본 회복값 0 고정
	int nExtractAmount = std::min( ( pItem->GetMaxEtherealDurability() - pItem->GetCurrentEtherealDurability() ) / 10000, pClient->GetEtherealStoneDurability() / 10000 - 1 );

	// 회복될 장비가 아예 내구도가 바닥났으면 여기서 복구 안 해줌(RecoverExhaustedEtherealDurability에 의해 복구시켜야 함)
	if( !pItem->GetCurrentEtherealDurability() || nExtractAmount <= 0 )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	int nPrevEtherealStoneDurability = pClient->GetEtherealStoneDurability();
	int nPrevItemEtherealDurability = pItem->GetCurrentEtherealDurability();

	pClient->AddEtherealStoneDurability( -1 * nExtractAmount * 10000 );
	pItem->AddCurrentEtherealDurability( nExtractAmount * 10000 );
	SendItemMessage( pClient, pItem );
	PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pClient, "@7901\v#@Ethereal_Durability@#\v%d", nExtractAmount );

	LOG::Log11N4S( LM_TRANSMIT_ETHEREAL_DURABILITY, pClient->GetAccountID(), pClient->GetSID(), nPrevEtherealStoneDurability, pClient->GetEtherealStoneDurability(), nPrevItemEtherealDurability, pItem->GetCurrentEtherealDurability(), 0, 0, 0, 0, pItem->GetItemUID(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	SendResult( pClient, pMsg->id, RESULT_SUCCESS );
}

void onTransmitEtherealDurabilityToEquipment( StructPlayer *pClient, TS_CS_TRANSMIT_ETHEREAL_DURABILITY_TO_EQUIPMENT* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructCreature *pTarget = NULL;
	std::vector< StructItem * > vItem;
	vItem.reserve( 15 );

	// 타겟이 기본값 0일 경우에는 플레이어
	if( !pMsg->target )
	{
		pTarget = pClient;
		StructItem *pItem = NULL;

		pItem = pClient->GetWearedItem( ItemBase::WEAR_SPARE_WEAPON );
		if( pItem ) vItem.push_back( pItem );

		pItem = pClient->GetWearedItem( ItemBase::WEAR_SPARE_SHIELD );
		if( pItem ) vItem.push_back( pItem );

		pItem = pClient->GetEquipmentOnBelt();
		if( pItem ) vItem.push_back( pItem );
	}
	// 그렇지 않을 경우는 소환수 슬롯 번호, 존재하지 않는다면 무시
	else if( pMsg->target >= 1 && pMsg->target <= 6 )
	{
		pTarget = pClient->GetSummonAt( pMsg->target - 1 );
	}

	if( !pTarget )
		return;

	for( int i = 0; i < ItemBase::MAX_ITEM_REPAIR; ++i )
	{
		StructItem *pItem = pTarget->GetWearedItem( static_cast< ItemBase::ItemWearType >( i ) );
		if( !pItem ) continue;

		vItem.push_back( pItem );
	}

	for( int i = 0; i < vItem.size(); ++i )
	{
		StructItem *pItem = vItem[i];
		// 회복될 장비가 아예 내구도가 바닥났으면 여기서 복구 안 해줌(RecoverExhaustedEtherealDurability에 의해 복구시켜야 함)
		if( !pItem->GetCurrentEtherealDurability() ) continue;

		float rate = (float)pItem->GetCurrentEtherealDurability() / pItem->GetMaxEtherealDurability();
		if( rate >= pMsg->rate ) continue;

		int nExtractAmount = std::min( std::min( (int) ( pItem->GetMaxEtherealDurability() * ( pMsg->rate - rate ) ) / 10000, pClient->GetEtherealStoneDurability() / 10000 - 1 ), ( pItem->GetMaxEtherealDurability() - pItem->GetCurrentEtherealDurability() ) / 10000 );
		if( !nExtractAmount ) continue;

		int nPrevEtherealStoneDurability = pClient->GetEtherealStoneDurability();
		int nPrevItemEtherealDurability = pItem->GetCurrentEtherealDurability();

		pClient->AddEtherealStoneDurability( -1 * nExtractAmount * 10000 );
		pItem->AddCurrentEtherealDurability( nExtractAmount * 10000 );
		SendItemMessage( pClient, pItem );
		PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pClient, "@7901\v#@Ethereal_Durability@#\v%d", nExtractAmount );

		LOG::Log11N4S( LM_TRANSMIT_ETHEREAL_DURABILITY, pClient->GetAccountID(), pClient->GetSID(), nPrevEtherealStoneDurability, pClient->GetEtherealStoneDurability(), nPrevItemEtherealDurability, pItem->GetCurrentEtherealDurability(), 1, 0, 0, 0, pItem->GetItemUID(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}
}

void onGetBoothsName( StructPlayer* pClient, TS_CS_GET_BOOTHS_NAME* pMsg )
{
	THREAD_SYNCRONIZE( StructPlayer::GetBoothLock() );

	TS_SC_GET_BOOTHS_NAME msg;

	msg.size += pMsg->cnt * sizeof( TS_SC_GET_BOOTHS_NAME::TS_BOOTH_NAME );

	std::auto_ptr< IQueue > pQueue( IQueue::MakeQueue( msg.size ) );

	msg.cnt = pMsg->cnt;

	pQueue->Write( &msg, sizeof( msg ) );

	AR_HANDLE * target;

	target = reinterpret_cast< AR_HANDLE * >( pMsg + 1 );

	for( unsigned i = 0; i < pMsg->cnt; ++i )
	{
		TS_SC_GET_BOOTHS_NAME::TS_BOOTH_NAME BoothName;

		StructCreature::iterator it = StructCreature::get( *target );
		StructPlayer * pTarget = static_cast< StructPlayer * >( *it );

		BoothName.handle = *target;

		if( pTarget )
		{
			s_strcpy( BoothName.name, _countof( BoothName.name ), pTarget->GetBoothName() );
		}
		else
		{
			BoothName.name[0] = 0;
		}

		pQueue->Write( &BoothName, sizeof( BoothName ) );

		++target;
	}

	PendStream( pClient, pQueue->GetBuf(), msg.size );
}

void onMonsterRecognize( StructPlayer * pClient, TS_CS_MONSTER_RECOGNIZE* pMsg )
{
	// 몬스터의 iterator를 바로 얻는 함수가 없음 -_ -;
	StructCreature::iterator itTarget = StructCreature::get( pMsg->monster_handle );
	StructCreature * pTarget = (*itTarget);

	if( !pTarget || !pTarget->IsMonster() )
		return;

	StructMonster * pMonster = static_cast< StructMonster * >( pTarget );

	if( !pMonster->IsFirstAttacker() || !pMonster->IsInWorld() || pMonster->IsDead() || pMonster->GetStatus() != StructMonster::STATUS_NORMAL ||
		pMonster->GetFinalPriority() == ArSchedulerObject::UPDATE_PRIORITY_HIGHEST )
		return;

	// 선공 인식 요청 주체 찾기
	StructCreature * pRecognizer = NULL;
	if( pClient->IsInWorld() && pClient->GetHandle() == pMsg->recognizer_handle )
		pRecognizer = pClient;
	else if( pClient->GetMainSummon() && pClient->GetMainSummon()->IsInWorld() && pClient->GetMainSummon()->GetHandle() == pMsg->recognizer_handle )
		pRecognizer = pClient->GetMainSummon();
	else if( pClient->GetSubSummon() && pClient->GetSubSummon()->IsInWorld() && pClient->GetSubSummon()->GetHandle() == pMsg->recognizer_handle )
		pRecognizer = pClient->GetSubSummon();

	// 선공 인식 요청 주체가 없으면 패스
	if( !pRecognizer )
		return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( pRecognizer, pMonster ) );

	if( !pMonster->IsVisible( pRecognizer ) )
		return;

	if( !pRecognizer->IsInWorld() || pRecognizer->IsDead() )
		return;

	if( !pMonster->IsInWorld() || pMonster->IsDead() || pMonster->GetStatus() != StructMonster::STATUS_NORMAL || pMonster->GetEnemyHandle() )
		return;

	// 몬스터와 인식 요청 주체 사이의 거리 계산
	AR_TIME t = GetArTime();
	AR_UNIT distance = pRecognizer->GetCurrentPosition( t ).GetDistance( pMonster->GetCurrentPosition( t ) );
	if( distance > std::min( GameRule::VISIBLE_RANGE, pMonster->GetFirstAttackRange() ) )
		return;

	// 선공 몬스터이므로 적을 찾는 프로세스가 바로 실행되도록 priority만 최상으로 높임(최초 onProcess에서 알아서 다시 낮춰짐)
	ArcadiaServer::Instance().SetObjectPriority( pMonster, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
}

void onSummonCardSkillList( StructPlayer * pClient, TS_CS_SUMMON_CARD_SKILL_LIST* pMsg )
{
	StructItem * pItem = StructItem::FindItem( pMsg->item_handle );

	if( !pItem )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructSummon * pSummon = pItem->GetSummonStruct();

	if( pSummon )
	{
		SendSkillLevelMessage( pClient, pSummon );
	}
	else
	{
		TS_SC_SKILL_LEVEL_LIST msg;		

		msg.count = 0;

		PendMessage( pClient, &msg );
	}
}

void onTurnOnPkMode( StructPlayer * pClient )
{
	if( GameRule::bDisablePKOn )
	{
		// 클라이언트에서 RESULT 코드에 따른 시스템 메시지 추가를 하지 않도록 채팅 메시지로 직접 방송(좋지 않지만 -_ -;)
		SendChatMessage( false, CHAT_NOTICE, "@SCRIPT", pClient, "@796" );
		return;
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( !pClient->IsInPKField() )
	{
		SendResult( pClient, TM_CS_TURN_ON_PK_MODE, RESULT_NOT_ACTABLE );
	}
	else if( pClient->TurnOnPkMode() )
	{
		SendResult( pClient, TM_CS_TURN_ON_PK_MODE, RESULT_SUCCESS );
	}
	else
	{
		SendResult( pClient, TM_CS_TURN_ON_PK_MODE, RESULT_NOT_ACTABLE );

		// PK On 카운트 중 다시 PK On이 날아오면 PK On이 취소되고
		// 이 때, 클라에서 블러디 캐릭터의 이름이 흰 색이 되버리는 문제로 인해
		// Status 메시지 다시 보내달라고 콩현씨 요청
		BroadcastStatusMessage( pClient );
	}
}

void onTurnOffPkMode( StructPlayer * pClient )
{
	if( pClient->TurnOffPkMode() )
	{
		SendResult( pClient, TM_CS_TURN_OFF_PK_MODE, RESULT_SUCCESS );
	}
	else
	{
		SendResult( pClient, TM_CS_TURN_OFF_PK_MODE, RESULT_NOT_ACTABLE );
	}
}

void onDropQuest( StructPlayer * pClient, TS_CS_DROP_QUEST* pMsg )
{
	if( pClient->DropQuest( pMsg->code ) )
	{
		SendResult( pClient, TM_CS_DROP_QUEST, RESULT_SUCCESS );
	}
	else
	{
		SendResult( pClient, TM_CS_DROP_QUEST, RESULT_NOT_ACTABLE );
	}
}

void onQuestInfo( StructPlayer * pClient, TS_CS_QUEST_INFO * pMsg )
{
	if( !pMsg->code )
		return;

	TS_SC_DIALOG::DIALOG_TYPE eDialogType;

	eDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_IN_PROGRESS;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	const QuestBase & rQuestBase = StructQuest::GetQuestBase( pMsg->code );

	int nProgress = pClient->GetQuestProgress( pMsg->code );

	if( nProgress == QuestInstance::FINISHED && rQuestBase.bIsRepeatable )
	{
		StructQuest * pQuest = pClient->FindQuest( pMsg->code );
		if( pQuest )
			nProgress = pQuest->GetProgress();
		else
			nProgress = QuestInstance::NOT_STARTED;
	}

	if( ( nProgress == QuestInstance::NOT_STARTED && !pClient->IsPendingQuest( pMsg->code ) ) ||
		nProgress == QuestInstance::FINISHED || nProgress == -1 )
		return;

	int nTextID = pClient->GetQuestStartTextID( pMsg->code );
	assert( nTextID );

	if (nProgress == QuestInstance::NOT_STARTED)
		eDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_AND_START;
	else if (nProgress == QuestInstance::IN_PROGRESS)
		eDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_IN_PROGRESS;
	else if (nProgress == QuestInstance::FINISHABLE)
	{
		// 자체 완료가 가능한 퀘스트면 완료 창이 뜨도록 설정하고, NPC에게 완료해야 하는 퀘스트면 진행 창으로 띄움
		int nSelfFinishTextID = GameContent::GetQuestTextId(pMsg->code, QUEST_IS_FINISHABLE);
		if (nSelfFinishTextID)
		{
			eDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_AND_END;
			nTextID = nSelfFinishTextID;
		}
		else
			eDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_IN_PROGRESS;
	}
	else
		eDialogType = TS_SC_DIALOG::TYPE_QUEST_INFO_IN_PROGRESS;
//		return;

	pClient->SetupQuestDialog( pMsg->code, nTextID, eDialogType );
	pClient->ShowDialog();
}

void onEndQuest( StructPlayer * pClient, TS_CS_END_QUEST* pMsg )
{
	pClient->EndQuest( pMsg->code, pMsg->nOptionalReward );
}

void onSetMainTitle( StructPlayer * pClient, TS_CS_SET_MAIN_TITLE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	pClient->SetMainTitle( pMsg->code );
}

void onSetSubTitle( StructPlayer * pClient, TS_CS_SET_SUB_TITLE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	pClient->SetSubTitle( pMsg->index, pMsg->code );
}

void onBookmarkTitle( StructPlayer * pClient, TS_CS_BOOKMARK_TITLE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	pClient->BookmarkTitle( pMsg->code );
}

void onRequestRemoveState( StructPlayer * pClient, TS_CS_REQUEST_REMOVE_STATE* pMsg )
{
	const StateInfo* pInfo = GameContent::GetStateInfo( pMsg->state_code );

	if( !pInfo || !( pInfo->state_time_type & StateInfo::ERASE_ON_REQUEST ) )
	{
		SendResult( pClient, TM_CS_REQUEST_REMOVE_STATE, RESULT_NOT_EXIST );
		return;
	}

	StructCreature *pTarget = static_cast< StructCreature * >( pClient );

	if( pClient->GetHandle() != pMsg->target )
	{
		GameObject::iterator it = GameObject::get( pMsg->target );
		if( !(*it) || !(*it)->IsInWorld() || !(*it)->IsSummon() || static_cast< StructSummon * >( *it )->GetMaster() != pClient )
		{
			SendResult( pClient, TM_CS_REQUEST_REMOVE_STATE, RESULT_ACCESS_DENIED );
			return;
		}
		pTarget = static_cast< StructCreature * >( *it );
	}

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjects( pClient, pTarget ) );

	StructState *pState = pTarget->GetState( static_cast< StructState::StateCode >( pMsg->state_code ) );

	if( !pState )
	{
		SendResult( pClient, TM_CS_REQUEST_REMOVE_STATE, RESULT_NOT_EXIST );
		return;
	}

	pTarget->RemoveState( pState->GetCode() );

	SendResult( pClient, TM_CS_REQUEST_REMOVE_STATE, RESULT_SUCCESS );
}

// 이하 경매 관련
void onAuctionSearch( StructPlayer * pClient, TS_CS_AUCTION_SEARCH * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() || pClient->IsUsingStorage() )
		return;

	// 최근 경매 검색 시간 체크(검색 반복 간격 시간 제한 적용)
	if( pClient->GetNextAuctionUsableTime() > GetArTime() )
	{
		SendResult( pClient, pMsg->id, RESULT_COOL_TIME );
		return;
	}

	// Inserting a guarding null character with null-terminated string
	pMsg->keyword[ _countof( pMsg->keyword ) - 1 ] = '\0';

	unsigned short nResult = AuctionManager::Instance().SearchAndSendAuctionList( pClient, pMsg->category_id, pMsg->sub_category_id, pMsg->keyword, pMsg->page_num, pMsg->is_equipable );

	if( nResult != RESULT_SUCCESS )
	{
		SendResult( pClient, pMsg->id, nResult );
	}

	// 경매 처리의 빈번한 반복을 막기 위해 다음 처리 가능 시점까지 딜레이 설정
	pClient->SetNextAuctionUsableTime( GetArTime() + GameRule::nAuctionSearchRequestMinInterval );
}

void onAuctionSellingList( StructPlayer * pClient, TS_CS_AUCTION_SELLING_LIST * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() || pClient->IsUsingStorage() )
		return;

	// 최근 경매 검색 시간 체크(검색 반복 간격 시간 제한 적용)
	if( pClient->GetNextAuctionUsableTime() > GetArTime() )
	{
		SendResult( pClient, pMsg->id, RESULT_COOL_TIME );
		return;
	}

	unsigned short nResult = AuctionManager::Instance().SendRegisteredAuctionList( pClient, pMsg->page_num );

	if( nResult != RESULT_SUCCESS )
	{
		SendResult( pClient, pMsg->id, nResult );
	}

	// 경매 처리의 빈번한 반복을 막기 위해 다음 처리 가능 시점까지 딜레이 설정
	pClient->SetNextAuctionUsableTime( GetArTime() + GameRule::nAuctionProcessRequestMinInterval );
}

void onAuctionBiddedList( StructPlayer * pClient, TS_CS_AUCTION_BIDDED_LIST * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() || pClient->IsUsingStorage() )
		return;

	// 최근 경매 검색 시간 체크(검색 반복 간격 시간 제한 적용)
	if( pClient->GetNextAuctionUsableTime() > GetArTime() )
	{
		SendResult( pClient, pMsg->id, RESULT_COOL_TIME );
		return;
	}

	unsigned short nResult = AuctionManager::Instance().SendBiddedAuctionList( pClient, pMsg->page_num );

	if( nResult != RESULT_SUCCESS )
	{
		SendResult( pClient, pMsg->id, nResult );
	}

	// 경매 처리의 빈번한 반복을 막기 위해 다음 처리 가능 시점까지 딜레이 설정
	pClient->SetNextAuctionUsableTime( GetArTime() + GameRule::nAuctionProcessRequestMinInterval );
}

void onAuctionBid( StructPlayer * pClient, TS_CS_AUCTION_BID * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() || pClient->IsUsingStorage() )
		return;

	// 최근 경매 검색 시간 체크(검색 반복 간격 시간 제한 적용)
	if( pClient->GetNextAuctionUsableTime() > GetArTime() )
	{
		SendResult( pClient, pMsg->id, RESULT_COOL_TIME );
		return;
	}

	ItemBase::ItemCode nResultItemCode = 0;

	unsigned short nResult = AuctionManager::Instance().BidForAuction( pClient, pMsg->auction_uid, pMsg->bidding_price, nResultItemCode );

	SendResult( pClient, pMsg->id, nResult, nResultItemCode );

	// 경매 입찰 후 리스트 요청이 클라이언트로부터 바로 날아오므로 여기서는 최근 검색 시각 갱신하면 안 됨
}

void onAuctionInstantPurchase( StructPlayer * pClient, TS_CS_AUCTION_INSTANT_PURCHASE * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() || pClient->IsUsingStorage() )
		return;

	// 최근 경매 검색 시간 체크(검색 반복 간격 시간 제한 적용)
	if( pClient->GetNextAuctionUsableTime() > GetArTime() )
	{
		SendResult( pClient, pMsg->id, RESULT_COOL_TIME );
		return;
	}

	ItemBase::ItemCode nResultItemCode = 0;

	unsigned short nResult = AuctionManager::Instance().InstantPurchase( pClient, pMsg->auction_uid, nResultItemCode );

	SendResult( pClient, pMsg->id, nResult, nResultItemCode );

	// 즉시 구매 후 리스트 요청이 클라이언트로부터 바로 날아오므로 여기서는 최근 검색 시각 갱신하면 안 됨
}

void onAuctionRegister( StructPlayer * pClient, TS_CS_AUCTION_REGISTER * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() || pClient->IsUsingStorage() )
		return;

	// 최근 경매 검색 시간 체크(검색 반복 간격 시간 제한 적용)
	if( pClient->GetNextAuctionUsableTime() > GetArTime() )
	{
		SendResult( pClient, pMsg->id, RESULT_COOL_TIME );
		return;
	}

	time_t tDuration = 0;

	switch( pMsg->duration_type )
	{
	case TS_CS_AUCTION_REGISTER::DURATION_LONGTERM:
		tDuration = GameRule::AUCTION_DURATION_LONGTERM;
		break;
	case TS_CS_AUCTION_REGISTER::DURATION_MIDTERM:
		tDuration = GameRule::AUCTION_DURATION_MIDTERM;
		break;
	case TS_CS_AUCTION_REGISTER::DURATION_SHORTTERM:
	default:
		tDuration = GameRule::AUCTION_DURATION_SHORTTERM;
		break;
	}

	ItemBase::ItemCode nResultItemCode = 0;

	unsigned short nResult = AuctionManager::Instance().RegisterItemToSell( pClient, pMsg->item_handle, pMsg->item_count, tDuration, pMsg->start_price, pMsg->instant_purchase_price, &nResultItemCode );

	SendResult( pClient, pMsg->id, nResult, nResultItemCode );

	// 경매 등록 후 리스트 요청이 클라이언트로부터 바로 날아오므로 여기서는 최근 검색 시각 갱신하면 안 됨
}

void onAuctionCancel( StructPlayer * pClient, TS_CS_AUCTION_CANCEL * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() || pClient->IsUsingStorage() )
		return;

	// 최근 경매 검색 시간 체크(검색 반복 간격 시간 제한 적용)
	if( pClient->GetNextAuctionUsableTime() > GetArTime() )
	{
		SendResult( pClient, pMsg->id, RESULT_COOL_TIME );
		return;
	}

	ItemBase::ItemCode nResultItemCode = 0;

	unsigned short nResult = AuctionManager::Instance().CancelAuction( pClient, pMsg->auction_uid, nResultItemCode );

	SendResult( pClient, pMsg->id, nResult, nResultItemCode );

	// 경매 취소 후 리스트 요청이 클라이언트로부터 바로 날아오므로 여기서는 최근 검색 시각 갱신하면 안 됨
}

void onItemKeepingList( StructPlayer * pClient, TS_CS_ITEM_KEEPING_LIST * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() || pClient->IsUsingStorage() )
		return;

	// 최근 경매 검색 시간 체크(검색 반복 간격 시간 제한 적용)
	if( pClient->GetNextAuctionUsableTime() > GetArTime() )
	{
		SendResult( pClient, pMsg->id, RESULT_COOL_TIME );
		return;
	}

	unsigned short nResult = AuctionManager::Instance().SendKeepingItemList( pClient, pMsg->page_num );

	if( nResult != RESULT_SUCCESS )
	{
		SendResult( pClient, pMsg->id, nResult );
	}

	// 경매 처리의 빈번한 반복을 막기 위해 다음 처리 가능 시점까지 딜레이 설정
	pClient->SetNextAuctionUsableTime( GetArTime() + GameRule::nAuctionProcessRequestMinInterval );
}

void onItemKeepingTake( StructPlayer * pClient, TS_CS_ITEM_KEEPING_TAKE * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() || pClient->IsUsingStorage() )
		return;

	// 최근 경매 검색 시간 체크(검색 반복 간격 시간 제한 적용)
	if( pClient->GetNextAuctionUsableTime() > GetArTime() )
	{
		SendResult( pClient, pMsg->id, RESULT_COOL_TIME );
		return;
	}

	unsigned short nResult = AuctionManager::Instance().TakeKeepedItem( pClient, pMsg->keeping_uid );

	SendResult( pClient, pMsg->id, nResult );

	// 아이템 수령 후 리스트 요청이 클라이언트로부터 바로 날아오므로 여기서는 최근 검색 시각 갱신하면 안 됨
}

void onHuntaholicInstanceList( StructPlayer * pClient, TS_CS_HUNTAHOLIC_INSTANCE_LIST * pMsg )
{
	int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() );

	if( !nHuntaholicID )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	if( pMsg->page < 0 )
	{
		SendResult( pClient, pMsg->id, RESULT_INVALID_ARGUMENT );
		return;
	}

	char szBuffer[ sizeof( TS_SC_HUNTAHOLIC_INSTANCE_LIST ) + sizeof( TS_HUNTAHOLIC_INSTANCE_INFO ) * GameRule::HUNTAHOLIC_MAX_INSTANCE_COUNT_PER_PAGE ];

	TS_SC_HUNTAHOLIC_INSTANCE_LIST * pResultMsg = reinterpret_cast< TS_SC_HUNTAHOLIC_INSTANCE_LIST * >( szBuffer );
	memset( pResultMsg, 0, sizeof( szBuffer ) );

	pResultMsg->id = TM_SC_HUNTAHOLIC_INSTANCE_LIST;
	pResultMsg->size = sizeof( TS_SC_HUNTAHOLIC_INSTANCE_LIST );
	pResultMsg->huntaholic_id = nHuntaholicID;
	pResultMsg->page = pMsg->page;
	int nCount = static_cast< int >( HuntaholicManager::Instance().GetUsedInstanceCount( nHuntaholicID, pClient->GetLevel() ) );
	pResultMsg->total_page = ( nCount > 0 ) ? ( nCount - 1 ) / GameRule::HUNTAHOLIC_MAX_INSTANCE_COUNT_PER_PAGE + 1 : 0;

	if( pResultMsg->total_page && pMsg->page > pResultMsg->total_page )
	{
		SendResult( pClient, pMsg->id, RESULT_INVALID_ARGUMENT );
		return;
	}

	TS_HUNTAHOLIC_INSTANCE_INFO * pInstanceInfo = reinterpret_cast< TS_HUNTAHOLIC_INSTANCE_INFO * >( pResultMsg + 1 );

	// 현재 생성되어 있는 방이 있으면 목록 정보 얻기, 없으면 0개로 넘기기
	if( pResultMsg->total_page )
		pResultMsg->count = static_cast< int >( HuntaholicManager::Instance().GetInstanceList( nHuntaholicID, pClient->GetLevel(), pMsg->page, pInstanceInfo ) );
	else
		pResultMsg->count = 0;
	pResultMsg->size += sizeof( TS_HUNTAHOLIC_INSTANCE_INFO ) * pResultMsg->count;

	PendMessage( pClient, pResultMsg );
}

void onHuntaholicCreateInstance( StructPlayer * pClient, TS_CS_HUNTAHOLIC_CREATE_INSTANCE * pMsg )
{
	int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() );

	if( !nHuntaholicID )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	if( pClient->GetHuntaholicEnterableCount() < 1 )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_BULLET );
		return;
	}

	unsigned short nErrorCode = HuntaholicManager::Instance().CreateInstanceDungeon( nHuntaholicID, pClient, pMsg->name, pMsg->max_member_count, pMsg->password );

	SendResult( pClient, pMsg->id, nErrorCode, nHuntaholicID );
}

void onHuntaholicJoinInstance( StructPlayer * pClient, TS_CS_HUNTAHOLIC_JOIN_INSTANCE * pMsg )
{
	int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() );

	if( !nHuntaholicID )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	if( pClient->GetHuntaholicEnterableCount() < 1 )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ENOUGH_BULLET );
		return;
	}

	unsigned short nErrorCode = HuntaholicManager::Instance().JoinInstanceDungeon( nHuntaholicID, pClient, pMsg->instance_no, pMsg->password );

	SendResult( pClient, pMsg->id, nErrorCode, nHuntaholicID );
}

void onHuntaholicLeaveInstance( StructPlayer * pClient, TS_CS_HUNTAHOLIC_LEAVE_INSTANCE * pMsg )
{
	int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() );

	if( !nHuntaholicID )
	{
		SendResult( pClient, pMsg->id, RESULT_ACCESS_DENIED );
		return;
	}

	unsigned short nErrorCode = RESULT_SUCCESS;

	if( HuntaholicManager::Instance().IsHuntaholicLobby( pClient->GetPos() ) )
	{
		nErrorCode = HuntaholicManager::Instance().LeaveInstanceDungeon( nHuntaholicID, pClient );
	}
	else
	{
		nErrorCode = HuntaholicManager::Instance().QuitHunting( nHuntaholicID, pClient );
	}

	SendResult( pClient, pMsg->id, nErrorCode, nHuntaholicID );
}

void onHuntaholicLeaveLobby( StructPlayer * pClient )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( !HuntaholicManager::Instance().IsHuntaholicLobby( pClient->GetPos() ) )
	{
		SendResult( pClient, TM_CS_HUNTAHOLIC_LEAVE_LOBBY, RESULT_ACCESS_DENIED );
		return;
	}

	if( pClient->IsInParty() )
	{
		SendResult( pClient, TM_CS_HUNTAHOLIC_LEAVE_LOBBY, RESULT_NOT_ACTABLE );
		return;
	}

	ArPosition posExit;
	pClient->GetPositionOnEnterInstanceGame( &posExit );
	unsigned char nLayer = 0;

	// 수련자의 섬 같이 인원 제한 채널에 레이어 설정된 구역으로 워프해야 하는 경우 적당한 레이어 얻기
	int nTargetChannel = ChannelManager::GetChannelId( posExit.GetX(), posExit.GetY() );
	if( nTargetChannel && ChannelManager::GetChannelType( nTargetChannel ) == ChannelManager::TYPE_USER_LIMIT )
	{
		nLayer = ChannelManager::GetProperLayer( posExit.GetX(), posExit.GetY() );
	}

	pClient->PendWarp( posExit.GetX(), posExit.GetY(), nLayer );

	SendResult( pClient, TM_CS_HUNTAHOLIC_LEAVE_LOBBY, RESULT_SUCCESS );
}

void onHuntaholicBeginHunting( StructPlayer * pClient )
{
	int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() );

	if( !nHuntaholicID )
	{
		SendResult( pClient, TM_CS_HUNTAHOLIC_BEGIN_HUNTING, RESULT_ACCESS_DENIED );
		return;
	}

	if( pClient->GetHuntaholicEnterableCount() < 1 )
	{
		SendResult( pClient, TM_CS_HUNTAHOLIC_BEGIN_HUNTING, RESULT_NOT_ENOUGH_BULLET );
		return;
	}

	HuntaholicManager::Instance().BeginHunting( nHuntaholicID, pClient );
}

void onInstanceGameEnter( StructPlayer * pClient, TS_CS_INSTANCE_GAME_ENTER * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	// 반복적으로 SendResult 함수를 호출하는 구문을 사용하지 않기 위해 do/while 구문을 형식적으로 사용하여 break문 이용
	unsigned short nResult = RESULT_SUCCESS;
	do
	{
		if( !pClient->IsInWorld() )
		{
			nResult = RESULT_NOT_EXIST;
			break;
		}

		// 데스매치라면 입장 불가
		if( pClient->IsInDeathmatch() )
		{
			nResult = RESULT_NOT_ACTABLE_IN_DEATHMATCH;
			break;
		}

		// 헌터홀릭일 경우 입장 불가
		if( HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() ) )
		{
			nResult = RESULT_NOT_ACTABLE_IN_HUNTAHOLIC;
			break;
		}

		// 숨겨진 던전일 경우 입장 불가
		if( pClient->IsInSecretDungeon() )
		{
			nResult = RESULT_NOT_ACTABLE_IN_SECRET_DUNGEON;
			break;
		}

		// 인스턴스 던전 안에 있으면 입장 불가
		if( pClient->IsInInstanceDungeon() )
		{
			nResult = RESULT_NOT_ACTABLE_IN_INSTANCE_DUNGEON;
			break;
		}

		// 유저나 혹은 소속 파티의 파티원이 시즈나 레이드 진행 중인 던전에 있다면 입장 불가
		int nPartyID = pClient->GetPartyID();
		if( pClient->IsInSiegeOrRaidDungeon() || ( nPartyID && PartyManager::GetInstance().IsSomeoneInSiegeOrRaidDungeon( nPartyID ) ) )
		{
			nResult = RESULT_NOT_ACTABLE_IN_SIEGE_OR_RAID;
			break;
		}

		// 무저갱일 경우 입장 불가
		if( pClient->GetLocationId() == StructWorldLocation::LOCATION_ID_ABADON )
		{
			nResult = RESULT_PK_LIMIT;
			break;
		}

		// 배틀 아레나에 참여 중일 경우 추가 체크/처리
		int nArenaID = pClient->GetBattleArenaID();
		if( nArenaID )
		{
			// 이미 시작된 경기(혹은 아직 시작되진 않았지만 연습 경기)에 참여 중인 경우에는 입장 불가
			// * 위의 경우들은 파티에 참여 중
			if( nPartyID )
			{
				assert( PartyManager::GetInstance().IsBattleArenaTeamParty( nPartyID ) );

				nResult = RESULT_NOT_ACTABLE_IN_BATTLE_ARENA;
				break;
			}

			// 아직 파티에 소속은 아닌데 아레나ID가 세팅되어 있다면 대기열에 대기 중인 경우. 대기열 자동 탈퇴시켜 줌.
			BattleArenaManager::Instance().QuitGame( pClient, false, false,
				( pMsg->instance_game_type == TS_CS_INSTANCE_GAME_ENTER::HUNTAHOLIC_BEAR_ROAD ) ? ALT_ENTER_HUNTAHOLIC : ALT_ENTER_DEATHMATCH );

			// 대기열 이탈 처리도 제대로 안 됐다면 실패 처리
			if( pClient->GetBattleArenaID() )
			{
				// 근데 그럼 이거 무슨 경우지;?
				assert( 0 );

				nResult = RESULT_NOT_ACTABLE_IN_BATTLE_ARENA;
				break;
			}
		}

		// 파티 강제 탈퇴/해산 처리
		if( nPartyID )
		{
			if( PartyManager::GetInstance().GetMemberCount( nPartyID ) == 1 )
			{
				BroadcastPartyDestroy( nPartyID );

				LOG::Log11N4S( LM_PARTY_DESTROY, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 5, 0, 0, 0, 0,
					pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

				PartyManager::GetInstance().DestroyParty( nPartyID );
			}
			else
			{
				if( PartyManager::GetInstance().IsLeader( nPartyID, pClient->GetPlayerUID() ) )
				{
					 if( !PartyManager::GetInstance().AutoPromote( nPartyID, false ) )
					 {
						// 리더이고 파티원이 2명 이상인데 자동 승격이 안된다 -> 이 로직에 무슨 문제가 있다. (공대 파티장이 헌터홀릭을 들어가려 한다거나.. 등) 
						assert( 0 );
						nResult = RESULT_UNKNOWN;
						break;
					 }

					 PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "PROMOTE|%d|%s|",
						 nPartyID, PartyManager::GetInstance().GetLeaderDisplayName( nPartyID ).c_str() );
				}

				BroadcastPartyLeave( pClient );

				LOG::Log11N4S( LM_PARTY_LEAVE, pClient->GetAccountID(), pClient->GetSID(), nPartyID, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 3, 0, 0, 0, 0,
					pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

				PartyManager::GetInstance().LeaveParty( nPartyID, pClient->GetPlayerUID() );
			}
		}

		// 해산/탈퇴 처리가 정상적으로 이루어지지 않았을 경우 입장 불가
		if( pClient->IsInParty() )
		{
			nResult = RESULT_UNKNOWN;
			break;
		}

		int skill_id = 0;
		switch( pMsg->instance_game_type )
		{
		case TS_CS_INSTANCE_GAME_ENTER::HUNTAHOLIC_BEAR_ROAD:
			skill_id = StructSkill::SKILL_WARP_TO_HUNTAHOLIC_LOBBY;
			break;
		case TS_CS_INSTANCE_GAME_ENTER::RANKED_DEATHMATCH:
			skill_id = StructSkill::SKILL_RANKED_DEATHMATCH_ENTER;
			break;
		case TS_CS_INSTANCE_GAME_ENTER::FREED_DEATHMATCH:
			skill_id = StructSkill::SKILL_FREED_DEATHMATCH_ENTER;
			break;
		default:
			nResult = RESULT_INVALID_ARGUMENT;
			break;
		}

		// switch 문 안에서 오류 코드가 세팅된 경우에 대한 처리(2중 break가 C 문법상 불가능한 문제 때문)
		if( nResult != RESULT_SUCCESS )
			break;

		// 캐스팅 가능한지 검사
		SkillBase * pBase = GameContent::GetSkillBase( skill_id );
		if( !pBase )
		{
			nResult = RESULT_INVALID_ARGUMENT;
			break;
		}
		if( ( pBase->IsPhysicalSkill() && !pClient->IsSkillCastable() ) || ( !pBase->IsPhysicalSkill() && !pClient->IsMagicCastable() ) )
		{
			nResult = RESULT_NOT_ACTABLE;
			break;
		}

		// 이동 중이었을 경우 스킬 시전 전에 현재 위치에 정지시킴
		if( pClient->IsMoving() )
		{
			AR_TIME t = GetArTime();
			ArPosition posCurrent = pClient->GetCurrentPosition( t );
			ArcadiaServer::Instance().SetMove( pClient, posCurrent, posCurrent, 0, true, t );
		}

		unsigned short nSkillResult = pClient->CastSkill( skill_id, 1, pClient->GetHandle(), pClient->GetPos(), pClient->GetLayer() );
		if( nSkillResult != RESULT_SUCCESS )
		{
			StructSkill::SendSkillCastFailMessage( pClient, pClient->GetHandle(), 0, skill_id, 1, pClient->GetPos(), nSkillResult );
			nResult = RESULT_ACCESS_DENIED;
		}
	} while( false );

	SendResult( pClient, TM_CS_INSTANCE_GAME_ENTER, nResult );
}

void onInstanceGameExit( StructPlayer * pClient )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	unsigned short nResult = RESULT_SUCCESS;
	do
	{
		if( !pClient->IsInWorld() )
		{
			nResult = RESULT_NOT_EXIST;
			break;
		}

		// 캐스팅 가능한지 검사
		SkillBase * pBase = GameContent::GetSkillBase( StructSkill::SKILL_INSTANCE_GAME_EXIT );
		if( !pBase )
		{
			nResult = RESULT_INVALID_ARGUMENT;
			break;
		}
		if( ( pBase->IsPhysicalSkill() && !pClient->IsSkillCastable() ) || ( !pBase->IsPhysicalSkill() && !pClient->IsMagicCastable() ) )
		{
			nResult = RESULT_NOT_ACTABLE;
			break;
		}

		// 이동 중이었을 경우 스킬 시전 전에 현재 위치에 정지시킴
		if( pClient->IsMoving() )
		{
			AR_TIME t = GetArTime();
			ArPosition posCurrent = pClient->GetCurrentPosition( t );
			ArcadiaServer::Instance().SetMove( pClient, posCurrent, posCurrent, 0, true, t );
		}

		unsigned short nSkillResult = pClient->CastSkill( StructSkill::SKILL_INSTANCE_GAME_EXIT, 1, pClient->GetHandle(), pClient->GetPos(), pClient->GetLayer() );
		if( nSkillResult != RESULT_SUCCESS )
		{
			StructSkill::SendSkillCastFailMessage( pClient, pClient->GetHandle(), 0, StructSkill::SKILL_INSTANCE_GAME_EXIT, 1, pClient->GetPos(), nSkillResult );
			nResult = RESULT_ACCESS_DENIED;
		}
	} while( false );

	SendResult( pClient, TM_CS_INSTANCE_GAME_EXIT, nResult );
}

void onInstanceGameScoreRequest( StructPlayer * pClient )
{
	TS_SC_INSTANCE_GAME_SCORE_REQUEST pResultMsg;

	pResultMsg.holicpoint = pClient->GetHuntaholicPoint();
	pResultMsg.bearroad_ranking = RankingManager::Instance().GetRank( RankingManager::RANKING_TYPE_HUNTAHOLIC_TOTAL, pClient->GetPlayerUID() );

	pResultMsg.deathmatch_kill_count = atoi( pClient->GetFlag( "kill" ).c_str() );
	pResultMsg.deathmatch_death_count = atoi( pClient->GetFlag( "dead" ).c_str() );

	pResultMsg.battle_arena_point = pClient->GetBattleArenaPoint();
	pResultMsg.battle_arena_mvp_count = pClient->GetBattleArenaMVPCount();
	pResultMsg.battle_arena_record[ 0 ][ 0 ] = pClient->GetBattleArenaRecord( BAT_CLASSIC, true );
	pResultMsg.battle_arena_record[ 0 ][ 1 ] = pClient->GetBattleArenaRecord( BAT_CLASSIC, false );
	pResultMsg.battle_arena_record[ 1 ][ 0 ] = pClient->GetBattleArenaRecord( BAT_BINGO, true );
	pResultMsg.battle_arena_record[ 1 ][ 1 ] = pClient->GetBattleArenaRecord( BAT_BINGO, false );
	pResultMsg.battle_arena_record[ 2 ][ 0 ] = pClient->GetBattleArenaRecord( BAT_SLAUGHTER, true );
	pResultMsg.battle_arena_record[ 2 ][ 1 ] = pClient->GetBattleArenaRecord( BAT_SLAUGHTER, false );

	PendMessage( pClient, &pResultMsg );
}

void onCompeteRequest( StructPlayer * pClient, TS_CS_COMPETE_REQUEST * pMsg )
{
	if( pClient->IsDead() || pClient->IsBoothOpen() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	AR_HANDLE hRequestee = StructPlayer::FindPlayer( pMsg->requestee );
	if( !hRequestee )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
		return;
	}

	if( hRequestee == pClient->GetHandle() )
	{
		SendResult( pClient, pMsg->id, RESULT_LIMIT_TARGET );
		return;
	}

	StructPlayer::iterator itRequestee = StructPlayer::get( hRequestee );
	StructPlayer * pRequestee = (*itRequestee);
	if( !pRequestee )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_EXIST );
		return;
	}

	if( pRequestee->IsDead() || pRequestee->IsBoothOpen() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE );
		return;
	}

	unsigned short nErrorCode = CompeteManager::Instance().RequestCompeteToPlayer( pClient, pRequestee );

	SendResult( pClient, pMsg->id, nErrorCode );
}

void onCompeteAnswer( StructPlayer * pClient, TS_CS_COMPETE_ANSWER * pMsg )
{
	unsigned short nErrorCode = CompeteManager::Instance().AnswerRequestFromPlayer( pClient, static_cast< _COMPETE_ANSWER_TYPE >( pMsg->answer_type ) );

	// 수락/거절 처리가 정상적으로 완료된 경우에는 유저에게 다시 알려줄 필요 없음(카운트 다운이 시작됨)
	if( nErrorCode == RESULT_SUCCESS )
	{
		// 수락 처리가 되어 카운트 다운이 시작된 경우에는 해당 플레이어의 StatusFlag가 변경되었으므로 Status를 다시 방송해줘야 함
		StructPlayer * pCompetitor = CompeteManager::Instance().GetCompetitor< StructPlayer * >( static_cast< _COMPETE_TYPE >( pMsg->compete_type ), pClient );
		if( pCompetitor )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pCompetitor ) );

			BroadcastStatusMessage( pCompetitor );
		}

		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

			BroadcastStatusMessage( pClient );
		}
	}
	else
		SendResult( pClient, pMsg->id, nErrorCode );
}

void onChangeAlias( StructPlayer * pClient, TS_CS_CHANGE_ALIAS * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	// 특정 아레나 대기열이나 경기에 참여 중이라면 변경 불가
	if( pClient->GetBattleArenaID() )
	{
		SendResult( pClient, pMsg->id, RESULT_NOT_ACTABLE_IN_BATTLE_ARENA );
		return;
	}

	pMsg->alias[ _countof( pMsg->alias ) - 1 ] = '\0';

	// 금지된 캐릭 이름으로는 변경 불가
	// * 가명은 뒤에 *이 자동으로 붙어야 하기 때문에 일반 이름의 최대 길이인 18 바이트보다 1 바이트 짧아야 함
	const size_t nAliasLength = strlen( pMsg->alias );
	if( nAliasLength )
	{
		// 최소 길이 체크
		if( nAliasLength < 4 )
		{
			SendResult( pClient, pMsg->id, RESULT_LIMIT_MIN );
			return;
		}
		// 최대 길이 체크
		if( nAliasLength > 15 )
		{
			SendResult( pClient, pMsg->id, RESULT_LIMIT_MAX );
			return;
		}

		// 사용 불가능 문자 및 금칙어 체크
		int code_page = ENV().GetInt( "CodePage", CP_ACP );
		if( !GameRule::IsValidName( code_page, pMsg->alias, _countof( pMsg->alias ), 4, 15 ) ||
			GameContent::IsBannedWord( code_page, pMsg->alias ) )
		{
			SendResult( pClient, pMsg->id, RESULT_INVALID_TEXT );
			return;
		}

		GameRule::ReformatName( pMsg->alias );
	}

	pClient->DBQuery( new DB_ChangeCharacterAlias( pClient, pMsg->alias, true ) );

	// 여기서는 성공 케이스에 대한 응답은 별도로 클라이언트로 보내지 않음
	// * DB_ChangeCharacterAlias가 성공으로 끝날 때 TS_SC_RESULT 패킷을 보내서
	//   이름 변경 성공/실패 여부를 클라이언트에게 알려 줌
}

void onBattleArenaJoinQueue( StructPlayer * pClient, TS_CS_BATTLE_ARENA_JOIN_QUEUE * pMsg )
{
	unsigned short nResult = RESULT_SUCCESS;

	// 아레나 ID가 0 이라면 빠른 참여
	if( !pMsg->nArenaID )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

		nResult = BattleArenaManager::Instance().QuickJoin( pClient );
	}
	// 연습 경기라면 연습 경기 생성으로 처리
	else if( pMsg->nArenaID == GameRule::BATTLE_ARENA_EXERCISE_GAME_ARENA_ID )
	{
		// CreateExerciseGame 호출은 새 경기를 바로 생성하기 때문에 해당 함수 내부에서 경기장 전체에 지역락을 걸고 스크립트를 실행시켜야 함
		nResult = BattleArenaManager::Instance().CreateExerciseGame( pClient );
	}
	// 연습 경기가 아니라면 대기열 입장 시도
	else
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

		nResult = BattleArenaManager::Instance().JoinWaitQueue( pClient, pMsg->nArenaID );
	}

	// nResult == RESULT_SUCCESS 라면 이미 관련 패킷들이 방송되었을 거라서 그 외의 경우에만 방송 처리
	if( nResult != RESULT_SUCCESS )
		SendResult( pClient, pMsg->id, nResult, pMsg->nArenaID );
}

void onBattleArenaLeave( StructPlayer * pClient )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	unsigned short nResult = RESULT_SUCCESS;
	if( pClient->GetBattleArenaID() )
		nResult = BattleArenaManager::Instance().QuitGame( pClient, false, false, ALT_USER_REQUEST );

	// nResult == RESULT_SUCCESS 라면 이미 관련 패킷들이 방송되었을 거라서 그 외의 경우에만 방송 처리
	if( nResult != RESULT_SUCCESS )
		SendResult( pClient, TM_CS_BATTLE_ARENA_LEAVE, nResult );
}

void onBattleArenaEnterWhileCountdown( StructPlayer * pClient )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	// 이미 경기장 내부라면 패킷 무시
	if( pClient->IsInBattleArena() )
		return;

	unsigned short nResult = BattleArenaManager::Instance().EnterArenaWhileCountdown( pClient );

	SendResult( pClient, TM_CS_BATTLE_ARENA_ENTER_WHILE_COUNTDOWN, nResult );
}

void onBattleArenaExerciseReady( StructPlayer * pClient, TS_CS_BATTLE_ARENA_EXERCISE_READY * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	unsigned short nResult = BattleArenaManager::Instance().SetReadyExerciseGame( pClient, pMsg->bReady );

	if( nResult != RESULT_SUCCESS )
		SendResult( pClient, pMsg->id, nResult );
}

void onBattleArenaExerciseStart( StructPlayer * pClient )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	unsigned short nResult = BattleArenaManager::Instance().StartExerciseGame( pClient );

	if( nResult != RESULT_SUCCESS )
		SendResult( pClient, TM_CS_BATTLE_ARENA_EXERCISE_START, nResult );
}

void onBattleArenaAbsenceCheckRequest( StructPlayer * pClient, TS_CS_BATTLE_ARENA_ABSENCE_CHECK_REQUEST * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	unsigned short nResult = BattleArenaManager::Instance().RequestAbsenceCheck( pClient, pMsg->hCheckTarget );

	SendResult( pClient, pMsg->id, nResult );
}

void onBattleArenaAbsenceCheckAnswer( StructPlayer * pClient, TS_CS_BATTLE_ARENA_ABSENCE_CHECK_ANSWER * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	BattleArenaManager::Instance().AnswerAbsenceCheck( pClient, pMsg->bSuccess );
}

// 이하 인증서버 관련
void onAuthLoginResult( TS_AG_LOGIN_RESULT * pMsg )
{
	if( pMsg->result == RESULT_SUCCESS )
	{
		ENV().Set( "auth.connect", "complete" );
	}
	else
	{
		ENV().Set( "auth.connect", "failed" );
	}
}

void onAuthClientLoginResult( TS_AG_CLIENT_LOGIN * pMsg )
{
	IStreamSocketConnection * pConn;

	{
		THREAD_SYNCRONIZE( &s_AuthAccountCS );

		// 오토가 계정 정보 길이를 길게 보내버리니 서버가 배째라 모드로 진입하시길래...
		pMsg->account[ _countof( pMsg->account ) - 1 ] = '\0';

		if( !s_hsAuthAccount.lookup( pMsg->account, pConn ) )
		{
			FILELOG( "AUTH DEBUG : ACCOUNT LOOK UP [%s(%d)]", pMsg->account, pMsg->nAccountID );
			pMsg->result = RESULT_ACCESS_DENIED;
			return;
		}

		s_hsAuthAccount.erase( pMsg->account );
	}

	if( pMsg->result == RESULT_SUCCESS )
	{
		THREAD_SYNCRONIZE( g_ConnectionTagLock );

		_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG* >( pConn->GetTag() );

		if( pTag )
		{
			if( !StructPlayer::RegisterAccount( pConn, pMsg->account ) )
			{
				FILELOG( "AUTH DEBUG : ACCOUNT REGISTER FAILED [%s(%d)] at %s", pMsg->account, pMsg->nAccountID, pConn->GetPeerAddress().GetAddr() );

				// 이런 일은 있을 수 없어! ( 그런데 이거 어제 한번 일어났음. 브레이크 걸어놓고 좀 있다 풀었더니 나더라. -_- by Testors )

				pMsg->result = RESULT_ACCESS_DENIED;

				// 인증서버에도 이런 비참한 사태가 벌어졌음을 알려줘야 함.
				SendLogoutToAuth( pMsg->account, pMsg->nContinuousPlayTime, 1 );
				pTag->bAuthByAuthServer = false;
			}
			else
			{
				s_strcpy( pTag->szAccountName, _countof( pTag->szAccountName ), pMsg->account );
				pTag->nAccountID = pMsg->nAccountID;
				pTag->bAuthByAuthServer = true;
				pTag->nPCBangMode = pMsg->nPCBangMode;
				pTag->nEventCode = pMsg->nEventCode;
				pTag->nAge = pMsg->nAge;
				pTag->nContinuousPlayTime = pMsg->nContinuousPlayTime;
				pTag->nContinuousLogoutTime = pMsg->nContinuousLogoutTime;

				if( GameRule::nSecuritySolutionType != XSecuritySolutionManager::TYPE_NONE )
				{
					XSecuritySolutionManager::Instance().InitClientSession( pConn );
				}
			}
		}
		else
		{
			pMsg->result = RESULT_ACCESS_DENIED;
		}
	}

	static_cast< XIOCPConnection * >( pConn )->DecVar();

	SendAccountResultWithAuth( pConn, pMsg->account, pMsg->result );
}

// 이하 업로드 서버 관련
void onUploadLoginResult( TS_US_LOGIN_RESULT *pMsg )
{
	if( pMsg->result == RESULT_SUCCESS )
	{
		ENV().Set( "upload.connect", "complete" );
	}
	else
	{
		ENV().Set( "upload.connect", "failed" );
	}
}

void onRequestUpload( TS_US_REQUEST_UPLOAD *pMsg )
{
}

void onUpload( TS_US_UPLOAD *pMsg )
{
	if( pMsg->size - sizeof( TS_US_UPLOAD ) != pMsg->filename_length )
	{
		return;
	}

	char *filename = new char[ pMsg->filename_length + 1 ];
	s_strncpy( filename, pMsg->filename_length + 1, reinterpret_cast< char * >( pMsg ) + sizeof( TS_US_UPLOAD ), pMsg->filename_length );
	
	switch( pMsg->type )
	{
	case TS_SU_REQUEST_UPLOAD::UPLOAD_TYPE_ICON:
		GuildManager::GetInstance().SetGuildIconInfo( pMsg->guild_id, filename, pMsg->file_size );
		break;
	case TS_SU_REQUEST_UPLOAD::UPLOAD_TYPE_BANNER:
		GuildManager::GetInstance().SetGuildBannerInfo( pMsg->guild_id, filename, pMsg->file_size );
		break;
	}

	delete [] filename;
	filename = NULL;
}

void onCommand( const std::string & strCommand )
{
	std::vector< std::string > vArguments;
	XStringUtil::Split( strCommand.c_str(), vArguments, " ", false );

	std::vector< std::string >::const_iterator it = vArguments.begin();
	if( it != vArguments.end() )
	{
		if( *it == "GET" )
		{
			char szBuf[ SENDMSG_BUFFER_SIZE ] = "";
			size_t nPosition = 0;

			++it;
			while( it != vArguments.end() )
			{
				// , : \n \0 4글자의 여유 바이트가 필요함
				size_t nLength = (*it).length() + 3 + ( nPosition ? 1 : 0 );
				std::string strValue = ENV().GetString( *it, "" );
				nLength += strValue.length();

				if( nPosition + nLength >= SENDMSG_BUFFER_SIZE )
				{
					break;
				}

				s_sprintf( szBuf + nPosition, _countof( szBuf ) - nPosition, nPosition ? ",%s:%s" : "%s:%s", (*it).c_str(), strValue.c_str() );
				++it;
				nPosition += nLength - 2;
			}

			szBuf[nPosition] = '\n';
			szBuf[nPosition + 1] = '\0';
			PendString( g_pCoordinatorConnection, szBuf );
		}
		else if( *it == "SHUTDOWN" )
		{
			exit( 0 );
		}
	}
}

void onResurrection( StructPlayer *pClient, TS_CS_RESURRECTION* pMsg )
{
	// 특수 부활만 처리, 일반 부활은 switch문 지나서 체크 후 아무 껀덕지도 없으면 적용
	switch( pMsg->type )
	{
	case TS_CS_RESURRECTION::RESURRECT_TYPE_BY_STATE:
		// 버프 부활
		{
			StructCreature * pTarget = pClient;

			{
				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

				if( pClient->GetHandle() != pMsg->handle )
				{
					pTarget = pClient->GetSummon( pMsg->handle );

					if( !pTarget )
					{
						return;
					}
				}
			}

			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pTarget ) );

			unsigned short bResult = RESULT_SUCCESS;

			if( !pTarget->IsInWorld() || !pTarget->IsDead() )
				bResult = RESULT_NOT_ACTABLE;

			if( !pTarget->ResurrectByState() )
				bResult = RESULT_NOT_ACTABLE;

			//SendResult( pClient, pMsg->id, bResult, pMsg->handle );
		}
		return;

	case TS_CS_RESURRECTION::RESURRECT_TYPE_BY_POTION:
		// 대모 요정의 병 부활
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

			// 락 걸고 한 번 검사
			if( !pClient->IsDead() )
				return;

			// 포션 사용으로 살아났을 때에는 ResurrectByPotion 함수 안에서 로그 작성
			if( pClient->ResurrectByPotion() )
				return;
		}
		break;

	case TS_CS_RESURRECTION::RESURRECT_TYPE_BY_COMPETE:
		// 대련 사망 패배 후 부활
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

			// 락 걸고 한 번 검사
			if( !pClient->IsDead() || !pClient->IsCompeteDead() )
				return;

			// 대련 부활로 살아났을 때에는 ResurrectByCompete 함수 안에서 로그 작성
			pClient->ResurrectByCompete();
		}
		return;

	case TS_CS_RESURRECTION::RESURRECT_TYPE_BY_DEATHMATCH:
		// 데스매치에서 부활
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

			// 락 걸고 한 번 검사
			if( !pClient->IsInDeathmatch() || !pClient->IsDead() )
				return;

			pClient->ResurrectByDeathmatch();
		}
	default:
		break;
	}

	// 헌터홀릭 안에서 부활
	int nHuntaholicID = HuntaholicManager::Instance().GetHuntaholicID( pClient->GetPos() );
	if( nHuntaholicID )
	{
		if( HuntaholicManager::Instance().IsHuntaholicDungeon( pClient->GetPos() ) )
		{
			HuntaholicManager::Instance().QuitHunting( nHuntaholicID, pClient, true, HuntaholicManager::HUNTING_RESULT_FAILED_BY_DEATH );

			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

			// 락 걸고 한 번 검사
			if( !pClient->IsDead() )
				return;

			LOG::Log11N4S( LM_CHARACTER_RESURRECTION, pClient->GetAccountID(), pClient->GetSID(), CRT_HUNTAHOLIC, 0, 0, 0, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 0, pClient->GetEXP(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );

			int nHPInc = pClient->GetMaxHP() - pClient->GetHP();
			pClient->AddHP( nHPInc );
			BroadcastHPMPMsg( pClient, nHPInc, 0 );

			pClient->ClearRemovedStateByDead();
		}
		else
		{
			// 로비에서 사망한 경우 로비 진입 지점으로~
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

			// 락 걸고 한 번 검사
			if( !pClient->IsDead() )
				return;

			LOG::Log11N4S( LM_CHARACTER_RESURRECTION, pClient->GetAccountID(), pClient->GetSID(), CRT_HUNTAHOLIC, 0, 0, 0, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 0, pClient->GetEXP(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );

			ArPosition posLobby( HuntaholicManager::Instance().GetLobbyPosition( nHuntaholicID ) );
			pClient->PendWarp( posLobby.GetX(), posLobby.GetY(), HuntaholicManager::Instance().GetProperLobbyLayer( nHuntaholicID, pClient->GetLevel() ) );
			pClient->SetMove( pClient->GetCurrentPosition( GetArTime() ), 0 );
			ArcadiaServer::Instance().SetObjectPriority( pClient, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

			int nHPInc = pClient->GetMaxHP() - pClient->GetHP();
			pClient->AddHP( nHPInc );
			BroadcastHPMPMsg( pClient, nHPInc, 0 );

			pClient->ClearRemovedStateByDead();
		}

		return;
	}

	// 배틀 아레나 안에서 부활
	if( pClient->IsInBattleArena() )
	{
		ArPosition pos;
		if( BattleArenaManager::Instance().GetRevivePosition( pClient, pos ) )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

			// 락 걸고 한 번 검사
			if( !pClient->IsDead() )
				return;

			// 아레나를 위해 지속효과를 부여받고 입장을 하였더라도 사망으로 인해 지속효과를 다 잃는 순간 팀 간의 밸런스가 쉽게 무너지기 때문에
			//  지속효과를 복구하고 수비를 위한 위치를 선점할 수 있도록 별도의 지속효과를 이용한다.
			AR_TIME t = GetArTime();
			pClient->AddState( StructState::PROTECTING_FORCE_OF_BEGINNING, 0, 1, t, t + GameRule::BEGINNING_TIME_IN_DEATHMATCH );

			if( pClient->Resurrect( CRT_BATTLE_ARENA, pClient->GetMaxHP(), pClient->GetMaxMP(), 0, true ) )
			{
				pClient->PendWarp( pos.GetX(), pos.GetY(), pClient->GetLayer() );
				ArcadiaServer::Instance().SetObjectPriority( pClient, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );
			}
		}

		return;
	}

	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

		// 락 걸고 한 번 검사
		if( !pClient->IsDead() )
			return;

		LOG::Log11N4S( LM_CHARACTER_RESURRECTION, pClient->GetAccountID(), pClient->GetSID(), ( pClient->IsCompeteDead() ) ? CRT_COMPETE : CRT_BATTLE, 0, 0, 0, pClient->GetX(), pClient->GetY(), pClient->GetLayer(), 0, pClient->GetEXP(), pClient->GetAccountName(), LOG::STR_NTS, pClient->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "", LOG::STR_NTS );

		enum _REVIVE_TYPE
		{
			REVIVE_NORMAL			= 0,	// 일반 필드 부활
			REVIVE_BATTLE			= 1,	// 대련장 대련 부활(구 대련, 사용되지 않음)
			REVIVE_COMPETE			= 2,	// 대련 부활
			REVIVE_DUNGEON_SIEGE	= 3,	// 던전 시즈 부활
		};

		_REVIVE_TYPE eReviveType = REVIVE_NORMAL;

		if( pClient->IsCompeteDead() )
		{
			eReviveType = REVIVE_COMPETE;
			pClient->SetCompeteDead( false );
		}
		else if( pClient->IsInSiegeDungeon() )
		{
			eReviveType = REVIVE_DUNGEON_SIEGE;
		}

		std::string strReviveHandler;
		XStringUtil::Format( strReviveHandler, "revive_in_town( %d )", static_cast< int >( eReviveType ) );
		LUA()->RunString( strReviveHandler.c_str() );

		pClient->ClearRemovedStateByDead();
	}
}

void onKickClient( TS_AG_KICK_CLIENT * pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	IStreamSocketConnection * pConn = StructPlayer::GetConnectionByAccount( pMsg->account );

	if( pConn )
	{
		_CONNECTION_TAG * pTag = reinterpret_cast< _CONNECTION_TAG * > ( pConn->GetTag() );

		StructPlayer *pClient = pTag->pPlayer;

		// 플레이어가 존재하면 로그아웃 타이머 세팅 시도
		if( pClient )
		{
			// 로그아웃 타이머가 세팅되어 있지 않으면 세팅
			if( !pClient->GetLogoutTimer() )
			{
				_cprint( "AUTH DEBUG : KICK PLAYER [%s/%d] : Player(%s) is led to logout by timer\n", pMsg->account, pMsg->kick_type, pClient->GetName() );
				FILELOG( "AUTH DEBUG : KICK PLAYER [%s/%d] : Player(%s) is led to logout by timer", pMsg->account, pMsg->kick_type, pClient->GetName() );

				pTag->nContinuousPlayTime = pClient->GetContinuousPlayTime();
				pTag->nContinuousLogoutTime = pClient->GetContinuousLogoutTime();

				pClient->SetLogoutTimer( GetArTime() + ( pClient->CanLogoutNow() ? 0 : GameRule::nLogoutTimer ) );

				//pTag->pPlayer = NULL;
			}
			// 이미 로그아웃 타이머가 세팅되어 있으면 로그만 찍어줌(기다리면 로그아웃 되어야 정상)
			else
			{
				_cprint( "AUTH DEBUG : KICK PLAYER FAILED [%s/%d] - Player(%s) is in world and waiting logout by logout-timer.\n", pMsg->account, pMsg->kick_type, pClient->GetName() );
				FILELOG( "AUTH DEBUG : KICK PLAYER FAILED [%s/%d] - Player(%s) is in world and waiting logout by logout-timer.", pMsg->account, pMsg->kick_type, pClient->GetName() );

				// 플레이어가 월드에 있긴 한데 너무 한가하면 프로세스가 느긋하게 돌아서
				// 로그아웃이 너무 나중에 될 수 있으므로 UPDATE_PRIORITY_NORMAL 이상이 유지되도록 갱신
				// 주의: 아래 조건 검사 순서 바꾸면 LogoutNow 처리 중인 순간에 처리가 발생하여 삭제된 캐릭터의 onProcess가 호출될 수 있음!
				if( pClient->GetPriority() < ArSchedulerObject::UPDATE_PRIORITY_NORMAL && pClient->IsInWorld() )
					ArcadiaServer::Instance().SetObjectPriority( pClient, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );
			}
		}

		// Connection이 연결상태면 Close 시킴
		// 위에서 로그아웃 타이머를 세팅했던 말던 관계 없이 접속은 끊기고
		// 실제 캐릭터 로그아웃은 타이머에 의해 처리 됨
		// (단, 로그아웃 세팅이 안되어 있었으나 되어야 하는 경우라면 접속이 종료되면서
		//  처리가 될것이라고 믿어 의심치 않으나... 문제가 생길 수도 있음 -_ -;;)
		if( pConn->IsConnected() )
		{
			XIOCPConnection *pConnection = static_cast< XIOCPConnection * >( pConn );
			int nPrevPendingQueryCount = pConnection->GetPendingQueryCount();
			int nPrevPendingRecvQueryCount = pConnection->GetPendingRecvQueryCount();
			int nPrevPendingSendQueryCount = pConnection->GetPendingSendQueryCount();
			int nPrevVar = pConnection->GetVar();

			SendDisconnectDesc( pConn, static_cast< TS_SC_DISCONNECT_DESC::_DISCONNECT_TYPE >( pMsg->kick_type ) );
			pConn->Close();

			_cprint( "AUTH DEBUG : KICK PLAYER [%s/%d] : Connection is closed. ConnectionInfo[%d(%d/%d),%d][%d(%d/%d),%d]\n",
				pMsg->account, pMsg->kick_type, nPrevPendingQueryCount, nPrevPendingRecvQueryCount, nPrevPendingSendQueryCount, nPrevVar,
				pConnection->GetPendingQueryCount(), pConnection->GetPendingRecvQueryCount(), pConnection->GetPendingSendQueryCount(), pConnection->GetVar() );
			FILELOG( "AUTH DEBUG : KICK PLAYER [%s/%d] : Connection is closed. ConnectionInfo[%d(%d/%d),%d][%d(%d/%d),%d]",
				pMsg->account, pMsg->kick_type, nPrevPendingQueryCount, nPrevPendingRecvQueryCount, nPrevPendingSendQueryCount, nPrevVar,
				pConnection->GetPendingQueryCount(), pConnection->GetPendingRecvQueryCount(), pConnection->GetPendingSendQueryCount(), pConnection->GetVar() );
		}
		// 연결도 끊겨있고 pTag->pPlayer도 없는데 GetConnectionByAccount가 가능하다면
		// 로그아웃 처리 중 또는 완전 알 수 없는 상태가 된 것
		else if( !pClient )
		{
			XIOCPConnection *pConnection = static_cast< XIOCPConnection * >( pConn );
			_cprint( "AUTH DEBUG : KICK PLAYER FAILED [%s/%d] : Logout is in process or unknown state. ConnectionInfo[%d(%d/%d),%d]\n",
				pMsg->account, pMsg->kick_type, pConnection->GetPendingQueryCount(), pConnection->GetPendingRecvQueryCount(), pConnection->GetPendingSendQueryCount(), pConnection->GetVar() );
			FILELOG( "AUTH DEBUG : KICK PLAYER FAILED [%s/%d] : Logout is in process or unknown state. ConnectionInfo[%d(%d/%d),%d]",
				pMsg->account, pMsg->kick_type, pConnection->GetPendingQueryCount(), pConnection->GetPendingRecvQueryCount(), pConnection->GetPendingSendQueryCount(), pConnection->GetVar() );

			// 무슨 상태인지 모르겠고 로그인도 안되서 옵션에 따라 강제로 계정 로그아웃 처리
			// (대개 여기까지 오면 캐릭터는 월드에 없고 캐릭터 선택 화면에서 접속 잘못 종료된 경우)
			if( GameRule::bForceUnregisterAccountOnKickFail )
				StructPlayer::UnRegisterAccount( pMsg->account );
		}
	}
	else if( StructPlayer::IsRegisteredAccount( pMsg->account ) )
	{
		_cprint( "AUTH DEBUG : KICK PLAYER FAILED [%s/%d] - Maybe waiting logout by logout-timer.\n", pMsg->account, pMsg->kick_type );
		FILELOG( "AUTH DEBUG : KICK PLAYER FAILED [%s/%d] - Maybe waiting logout by logout-timer.", pMsg->account, pMsg->kick_type );
	}
	else
	{
		_cprint( "AUTH DEBUG : KICK PLAYER FAILED [%s/%d] - Unregistered account\n", pMsg->account, pMsg->kick_type );
		FILELOG( "AUTH DEBUG : KICK PLAYER FAILED [%s/%d] - Unregistered account", pMsg->account, pMsg->kick_type );

		TS_GA_CLIENT_KICK_FAILED msg;
		s_strcpy( msg.account, _countof( msg.account ), pMsg->account );

		if( g_pAuthConnection && g_pAuthConnection->IsConnected() )
		{
			PendMessage( g_pAuthConnection, &msg );
		}
	}
}

void onPCBangExpireWarning( TS_AG_PCBANG_EXPIRE_WARNING * pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	IStreamSocketConnection * pConn = StructPlayer::GetConnectionByAccount( pMsg->account );

	if( pConn && pConn->IsConnected() )
	{
		_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG* >( pConn->GetTag() );
		if( pTag->pPlayer )
		{
			PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pTag->pPlayer, "@829\v#@minute@#\v%d", pMsg->remain_minutes );
		}
	}
}

void onPCBangExpired( TS_AG_PCBANG_EXPIRE * pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	IStreamSocketConnection * pConn = StructPlayer::GetConnectionByAccount( pMsg->account );

	if( pConn && pConn->IsConnected() )
	{
		_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG* >( pConn->GetTag() );
		pTag->nPCBangMode = GameRule::PCBANG_ALLY_BONUS;
		if( pTag->pPlayer )
		{
			PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pTag->pPlayer, "@830" );
			// 프리미엄 PC방은 혜택 끝나면 무조건 가맹 PC방임
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pTag->pPlayer ) );

			pTag->pPlayer->SetPCBangMode( GameRule::PCBANG_ALLY_BONUS );
		}
	}
}

void onPCBangChanged( TS_AG_PCBANG_CHANGED* pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	IStreamSocketConnection * pConn = StructPlayer::GetConnectionByAccount( pMsg->account );

	if( pConn && pConn->IsConnected() )
	{
		_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG* >( pConn->GetTag() );
		int nOldMode = pTag->nPCBangMode;
		pTag->nPCBangMode = pMsg->nPCBangMode;

		if( pTag->pPlayer )
		{
			if( nOldMode == GameRule::PCBANG_PREMIUM_BONUS &&
				pTag->nPCBangMode == GameRule::PCBANG_ALLY_BONUS )	// 만료
			{
				PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pTag->pPlayer, "@830" );
			}
			else if( nOldMode == GameRule::PCBANG_ALLY_BONUS &&
				pTag->nPCBangMode == GameRule::PCBANG_PREMIUM_BONUS )	// 프리미엄 가입
			{
				PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pTag->pPlayer, "@690000050" );
			}

			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pTag->pPlayer ) );

			pTag->pPlayer->SetPCBangMode( pTag->nPCBangMode );
		}
	}
}

void onSecurityNoCheck( TS_AG_SECURITY_NO_CHECK* pMsg )
{
	if( !GameRule::bUseSecurityNo )
		return;

	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	IStreamSocketConnection * pConn = StructPlayer::GetConnectionByAccount( pMsg->account );

	if( !pConn )
		return;

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG* >( pConn->GetTag() );
	if( !pTag )
		return;

	if ( !pMsg->result )
	{
		switch ( pMsg->mode )
		{
		case TS_SC_REQUEST_SECURITY_NO::SC_DELETE_CHARACTER : 
			SendResult( pConn, TM_CS_DELETE_CHARACTER, RESULT_PASSWORD_MISMATCH );
			break;

		case TS_SC_REQUEST_SECURITY_NO::SC_OPEN_STORAGE :
			SendResult( pConn, TM_CS_SECURITY_NO, RESULT_PASSWORD_MISMATCH );
			break;
		}
	}
	else
	{
		switch ( pMsg->mode )
		{
		case TS_SC_REQUEST_SECURITY_NO::SC_DELETE_CHARACTER : 
			static_cast< XIOCPConnection * >( pConn )->IncVar();
			DB().Push( new DB_DeleteCharacter( pTag->szAccountName, pTag->strNameToDelete.c_str(), pTag->nAccountID, pConn ) );
			break;

		case TS_SC_REQUEST_SECURITY_NO::SC_OPEN_STORAGE :
			if ( pTag->pPlayer )
			{
				pTag->bStorageSecurityCheck = true;
				pTag->pPlayer->OpenStorage();
			}
			break;
		}
	}

	/*	2009. 8. 11 floyd 보안비밀번호 2차 수정
	// bIsClearSecurity == true면 보안 비밀 번호 초기화고, false면 보안 비밀 번호 신규 세팅
	const bool bIsClearSecurity = pTag->strSecurityNoToChange.empty();

	if( !pMsg->result )
	{
	SendResult( pConn, ( bIsClearSecurity ) ? TM_CS_CLEAR_SECURITY_NO : TM_CS_CREATE_SECURITY_NO, RESULT_PASSWORD_MISMATCH );
	return;
	}

	static_cast< XIOCPConnection * >( pConn )->IncVar();

	if( bIsClearSecurity )
	DB().Push( new DB_ClearSecurityNo( pTag->nAccountID, pConn ) );
	else
	DB().Push( new DB_CreateSecurityNo( pTag->nAccountID, pTag->strSecurityNoToChange, pConn ) );
	*/
}

void onEmotion( StructPlayer * pClient, TS_CS_EMOTION * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	TS_SC_EMOTION msg;
	msg.handle = pClient->GetHandle();
	msg.emotion = pMsg->emotion;

	ArcadiaServer::Instance().Broadcast( pClient->GetRX(), pClient->GetRY(), pClient->GetLayer(), &msg );
}

void onChangeLocation( StructPlayer * pClient, TS_CS_CHANGE_LOCATION * pMsg )
{
	pClient->ChangeLocation( pMsg->x, pMsg->y, true );

	//WorldLocationManager::Instance().AddToLocation( pMsg->region_id, pClient );
}

void onGetWeaterInfo( StructPlayer * pClient, TS_CS_GET_WEATHER_INFO * pMsg )
{
	WorldLocationManager::Instance().SendWeatherInfo( pMsg->region_id, pClient );
}

struct _SendEnterMsg : ArObjectFunctor
{
	_SendEnterMsg( ArObject * pObject ) : pClient( pObject ) {}

	virtual void operator()( ArObject * pObject ) const
	{
		SendEnterMsg( pClient, pObject );
	}

	const ArObject *pClient;
};

void onGetRegionInfo( StructPlayer * pClient, TS_CS_GET_REGION_INFO * pMsg )
{
	struct _ArRegionFunctor : ArRegionFunctor
	{
		_ArRegionFunctor( ArObject * pObject ) : pClient( pObject )  {}

		virtual void operator()( const ArRegion * pRegion )
		{
			pRegion->DoEachClient( _SendEnterMsg( pClient ) );
			pRegion->DoEachMovableObject( _SendEnterMsg( pClient ) );
			pRegion->DoEachStaticObject( _SendEnterMsg( pClient ) );
		}

		ArObject *pClient;
	} _rf( pClient );

	unsigned rx = ::GetRegionX( pMsg->x );
	unsigned ry = ::GetRegionY( pMsg->y );
	unsigned char layer = pClient->GetLayer();

	ARCADIA_LOCK( ArcadiaServer::Instance().LockWithVisibleRange( rx, ry, layer ) );

	ArcadiaServer::Instance().DoEachVisibleRegion( rx, ry, layer, _rf );
}

void onItemPurchased( TS_AG_ITEM_PURCHASED * pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	IStreamSocketConnection * pConn = StructPlayer::GetConnectionByAccount( pMsg->account );

	if( pConn && pConn->IsConnected() )
	{
		_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConn->GetTag() );

		// 아템 업데되었다고 보내줌
		if( pTag->pPlayer && GameRule::bIsCashUsableServer )
		{
			if( GameRule::bUseAccountAuthorityDB )
			{
				pTag->pPlayer->DBQuery( new DB_ReadAccountAuthorityInfo( pTag->pPlayer ) );
			}
			pTag->pPlayer->DBQuery( new DB_GetCommercialStorageInfo( pTag->pPlayer, false ) );
		}
	}
}

void onItemSupplied( TS_AG_ITEM_SUPPLIED * pMsg )
{
	THREAD_SYNCRONIZE( g_ConnectionTagLock );

	IStreamSocketConnection * pConn = StructPlayer::GetConnectionByAccount( pMsg->account );

	if( pConn && pConn->IsConnected() )
	{
		_CONNECTION_TAG* pTag = static_cast<_CONNECTION_TAG*>( pConn->GetTag() );

		// 아템 업데되었다고 보내줌
		if( pTag->pPlayer && GameRule::bIsCashUsableServer )
		{
			// 아이템이 지급되었다는 메시지를 보내준 후에 캐쉬 아이템 창고를 읽음(메시지가 2줄 뜰 수 있음)
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pTag->pPlayer, "@442" );
			if( GameRule::bUseAccountAuthorityDB )
			{
				pTag->pPlayer->DBQuery( new DB_ReadAccountAuthorityInfo( pTag->pPlayer ) );
			}
			pTag->pPlayer->DBQuery( new DB_GetCommercialStorageInfo( pTag->pPlayer, false ) );
		}
	}
}

void onItemPositionChange( StructPlayer * pClient, TS_CS_CHANGE_ITEM_POSITION * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	pClient->ChangeItemPosition( pMsg->is_storage, pMsg->item_handle_1, pMsg->item_handle_2 );
}


// 이하 크리처 농장 관련 패킷 핸들링 루틴들
void fillFarmedSummonTimeInfo( const StructPlayer::FARMED_SUMMON_INFO *farmInfo, TS_SC_FARM_INFO::SUMMON_INFO* summonInfo )
{
	time_t tCurrent = time( NULL );
	summonInfo->elasped_time = tCurrent - farmInfo->registration_time;

	if ( farmInfo->nursing_time == 0 ) // 돌보기를 한 적이 없다면
	{
		summonInfo->refresh_time = 0;
	}
	else
	{
		// 최근 돌보기한 시점 바로 다음의 충전 시점을 계산한다.
		struct tm tmRefreshTime;
		localtime_s( &tmRefreshTime, &farmInfo->nursing_time );
		tmRefreshTime.tm_hour = 6;
		tmRefreshTime.tm_min = 0;
		tmRefreshTime.tm_sec = 0;
		tmRefreshTime.tm_isdst = -1;
		time_t tRefresh = mktime( &tmRefreshTime );

		if( farmInfo->nursing_time >= tRefresh )
		{
			++tmRefreshTime.tm_mday;
			tRefresh = mktime( &tmRefreshTime );
		}

		summonInfo->refresh_time = (tRefresh - tCurrent > 0) ? tRefresh - tCurrent : 0;
	}
}

void fillFarmedSummonInfo( const StructPlayer::FARMED_SUMMON_INFO *farmInfo, TS_SC_FARM_INFO::SUMMON_INFO* summonInfo )
{
	StructItem* creatureCard = farmInfo->item;

	summonInfo->exp = creatureCard->GetSummonStruct()->GetEXP();
	s_strcpy( summonInfo->name, _countof( summonInfo->name ), creatureCard->GetSummonStruct()->GetName() );
	summonInfo->name[_countof(summonInfo->name) - 1] = '\0';
	summonInfo->using_cash = farmInfo->is_cash;
	summonInfo->using_cracker = farmInfo->is_using_cracker;
	summonInfo->duration = farmInfo->duration;

	fillFarmedSummonTimeInfo( farmInfo, summonInfo );	
	fillItemBaseInfo( &summonInfo->card_info, farmInfo->item );
}

void onRequestFarmInfo( StructPlayer * pClient, TS_CS_REQUEST_FARM_INFO * pMsg )
{
	char buffer[ sizeof( TS_SC_FARM_INFO ) + sizeof (TS_SC_FARM_INFO::SUMMON_INFO ) * 3 ];
	memset(buffer, 0, sizeof( buffer ));
	new(buffer) TS_SC_FARM_INFO();

	TS_SC_FARM_INFO* info = reinterpret_cast< TS_SC_FARM_INFO* >( buffer );
	TS_SC_FARM_INFO::SUMMON_INFO* summon_list
		= reinterpret_cast< TS_SC_FARM_INFO::SUMMON_INFO* >( buffer + sizeof(TS_SC_FARM_INFO) );

	int count = 0;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	for( int i = 0; i < GameRule::FARM_MAX_COUNT; ++i )
	{
		StructPlayer::FARMED_SUMMON_INFO *pInfo = pClient->GetFarmedSummonInfo( i );
		if( !pInfo )
		{
			continue;
		}
		else if ( pInfo->registration_time + pInfo->duration < time( NULL ) )
		{
			pClient->RegainSummon( pInfo->item->GetHandle() );
			PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", pClient, "@1158" );
			continue;
		}

		summon_list[count].index = i;
		fillFarmedSummonInfo( pInfo, &summon_list[count] );

		++count;
	}

	info->creature_count = count;
	info->size = sizeof( TS_SC_FARM_INFO ) + sizeof( TS_SC_FARM_INFO::SUMMON_INFO ) * count;

	PendMessage( pClient, info );
}

const bool ValidateFarmTicket( StructPlayer *pClient, const TS_CS_FOSTER_CREATURE::TICKET_INFO *ticketInfo, const int infoCount, const int requiredTicketCount )
{
	// 티켓 검증	
	int ticketCount = 0;
	int duration = 0;
	bool isCash = false;

	for( int i = 0; i < infoCount; i++ )
	{
		StructItem* ticket = StructItem::FindItem( ticketInfo[i].ticket_handle );
		if( ticket != NULL && ticket->GetItemClass() == ItemBase::CLASS_FARM_PASS &&
			ticket->GetCount() >= ticketInfo[i].ticket_count &&
			ticket->IsInInventory() && ticket->GetOwnerUID() == pClient->GetPlayerUID() )
		{
			ticketCount += ticketInfo[i].ticket_count;
		}
		else
		{
			return false;
		}

		if( i == 0 )
		{
			duration = ticket->GetItemBase().fOptVar1[0];
			isCash = (ticket->GetItemBase().fOptVar2[0] == 1);
		}
		else if( duration != ticket->GetItemBase().fOptVar1[0] || isCash != (ticket->GetItemBase().fOptVar2[0] == 1) )
		{
			// 실패 - 서로 다른 유형의 이용권이 들어옴
			return false;
		}
	}

	return ticketCount == requiredTicketCount;
}

const bool ValidateCracker( StructPlayer *pClient, const TS_CS_FOSTER_CREATURE::CRACKER_INFO *crackerInfo, const int infoCount, const int requiredFoodCount )
{
	if( infoCount == 0 )
	{
		return true; // 이 경우는 크래커 미사용으로 간주
	}

	int foodCount = 0;

	for( int i = 0; i < infoCount; i++ )
	{
		StructItem* cracker = StructItem::FindItem( crackerInfo[i].cracker_handle );

		if( cracker != NULL && cracker->GetItemClass() == ItemBase::CLASS_CREATURE_FOOD &&
			cracker->GetCount() >= crackerInfo[i].cracker_count &&
			cracker->IsInInventory() && cracker->GetOwnerUID() == pClient->GetPlayerUID()	)
		{
			foodCount += crackerInfo->cracker_count;
		}
		else
		{			
			return false;
		}
	}

	return foodCount == requiredFoodCount;
}

const int FindSuitableFarmSlot( StructPlayer * pClient, const bool isCash )
{
	// 농장 슬롯은 무료 - 유료 순으로 배치되어 있다고 가정.
	int slotBegin = isCash ? GameRule::FARM_NON_CASH_MAX_COUNT : 0;
	int slotEnd = isCash ? GameRule::FARM_MAX_COUNT : GameRule::FARM_NON_CASH_MAX_COUNT;

	for ( int slot = slotBegin; slot < slotEnd; slot++ )
	{
		if ( pClient->GetFarmedSummonInfo( slot ) == NULL )
		{
			return slot;
		}
	}

	return -1; // 쓸만한 슬롯이 없는 경우 -1 반환
}

const bool FosterCreature( StructPlayer * pClient, TS_CS_FOSTER_CREATURE * pMsg )
{
	TS_CS_FOSTER_CREATURE::TICKET_INFO* ticketInfo =
		reinterpret_cast< TS_CS_FOSTER_CREATURE::TICKET_INFO * >( pMsg + 1 );
	TS_CS_FOSTER_CREATURE::CRACKER_INFO* crackerInfo =
		reinterpret_cast< TS_CS_FOSTER_CREATURE::CRACKER_INFO * >( ticketInfo + pMsg->ticket_info_count );

	StructItem* creatureCard = StructItem::FindItem( pMsg->creature_card_handle );
	if ( creatureCard == NULL || !creatureCard->IsInInventory() || creatureCard->GetOwnerUID() != pClient->GetPlayerUID() )
	{
		return false;
	}

	StructSummon* creature = creatureCard->GetSummonStruct();
	if ( creature == NULL )
	{
		return false;
	}

	int requiredTicketCount = GameContent::GetCreatureFarmTicketCount( creature->GetRate(), creature->GetTransformLevel(), creature->GetEnhance() );

	if( ValidateFarmTicket( pClient, ticketInfo, pMsg->ticket_info_count, requiredTicketCount ) &&
		ValidateCracker( pClient, crackerInfo, pMsg->cracker_info_count, requiredTicketCount ) )
	{
		StructItem* ticket = StructItem::FindItem( ticketInfo[0].ticket_handle );
		// 사용 기간, 캐쉬/크래커 사용 여부, 아이템 핸들 등의 정합성은 Validate~ 함수에서 확인된 상태
		int duration = ticket->GetItemBase().fOptVar1[0];
		bool isCash = ( ticket->GetItemBase().fOptVar2[0] == 1 );
		bool useCracker = ( pMsg->cracker_info_count > 0 );

		int slot = FindSuitableFarmSlot( pClient, isCash );

		if ( slot != -1 && pClient->FarmSummon( slot, pMsg->creature_card_handle, duration, useCracker, isCash ) )
		{
			for( int i = 0; i < pMsg->ticket_info_count; i++ )
			{
				StructItem* ticket = StructItem::FindItem( ticketInfo[i].ticket_handle );
				pClient->EraseItem( ticket, ticketInfo[i].ticket_count );
			}

			for( int i = 0; i < pMsg->cracker_info_count; i++ )
			{
				StructItem* cracker = StructItem::FindItem( crackerInfo[i].cracker_handle );
				pClient->EraseItem( cracker, crackerInfo[i].cracker_count );
			}
			return true;
		}
	}

	return false;
}

void onFosterCreature( StructPlayer * pClient, TS_CS_FOSTER_CREATURE * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	TS_SC_RESULT_FOSTER resultMsg;

	resultMsg.result = FosterCreature( pClient, pMsg ) ? 1 : 0;

	PendMessage( pClient, &resultMsg );
}

void onRetrieveCreature( StructPlayer * pClient, TS_CS_RETRIEVE_CREATURE * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	TS_SC_RESULT_RETRIEVE resultMsg;

	resultMsg.result = pClient->RegainSummon( pMsg->creature_card_handle ) ? 1 : 0;

	PendMessage( pClient, &resultMsg );
}

void onNurseCreature( StructPlayer * pClient, TS_CS_NURSE_CREATURE * pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	TS_SC_RESULT_NURSE resultMsg;

	if ( pClient->NurseSummon( pMsg->creature_card_handle ) )
	{
		std::string returnValue;

		LUA()->RunString( "return NPC_Creature_Farm_nurse_handler()", &returnValue );

		resultMsg.result = ( returnValue == "1" ) ? TS_SC_RESULT_NURSE::REWARDED : TS_SC_RESULT_NURSE::NO_REWARD;
	}
	else
	{
		resultMsg.result = TS_SC_RESULT_NURSE::FAILED;
	}

	PendMessage( pClient, &resultMsg );
}

void onRequestFarmMarket( StructPlayer * pClient )
{
	_MARKET_INFO *pInfo = NULL;
	pInfo = GameContent::GetMarketInfo( "creature_farm" );
	if( !pInfo ) return;

	ARCADIA_LOCK( ArcadiaServer::Instance().LockObject( pClient ) );

	SendMarketInfo( pClient, 0, pInfo );
}
// 크리처 농장 관련 루틴 끝

// party matching
void onPartyMatchingAction(StructPlayer* pClient, TS_CS_PARTYMATCH_ACTION* pMsg)
{
	switch(pMsg->nAction)
	{
	case 0: // 목록요청
		{
			//CPartyMatchingManager::GetInstance()->SendList(pClient, pMsg->nPage);
			CPartyMatchingManager::GetInstance()->ShowList(pClient, pMsg->nPage, pMsg->stRoom.nArea);
			break;
		}
	case 1: // 맴버요청
		{
			CPartyMatchingManager::GetInstance()->SendMember(pClient, pMsg->stRoom.nRoom);
			break;
		}
	case 2: // 방 만들기
		{
			CPartyMatchingManager::GetInstance()->Add(pClient, reinterpret_cast<void *>(&pMsg->stRoom));
			break;
		}
	case 3: // 방 없애기
		{
			CPartyMatchingManager::GetInstance()->Remove(pClient);
			break;
		}
	case 4: // apply(파티 요청)
		{
			CPartyMatchingManager::GetInstance()->Request(pClient);
			break;
		}
	default:
		break;
	}
}

// 기본 메세지 핸들러
void onReadEvent( int nID, struct IStreamSocketConnection * pConn )
{
	TS_MESSAGE header;
	char szMessageBuf[32768];
	TS_MESSAGE* pMsg = reinterpret_cast< TS_MESSAGE* >( szMessageBuf );

	if( pConn == g_pCoordinatorConnection )
	{
		int nReadSize = pConn->Peek( szMessageBuf, pConn->Size() );
		if( szMessageBuf[nReadSize - 1] != '\n' )
		{
			nReadSize = 0;
			bool bFound = false;
			for( int i = 0; i < nReadSize; ++i )
			{
				if( szMessageBuf[i] == '\n' )
				{
					nReadSize = i + 1;
					bFound = true;
					break;
				}
			}

			if( !bFound )
				return;
		}

		pConn->Read( szMessageBuf, nReadSize );
		szMessageBuf[nReadSize] = '\0';

		std::vector< std::string > vCommands;
		XStringUtil::Split( szMessageBuf, vCommands, "\n", false );
		for( std::vector< std::string >::const_iterator it = vCommands.begin(); it != vCommands.end(); ++it )
		{
			onCommand( *it );
		}

		return;
	}

	while( pConn->Size() >= sizeof( TS_MESSAGE ) )
	{
		pConn->Peek( &header, sizeof(header) );

		//FILELOG("Incoming Message: [ID: %d, Size: %d]", header.id, header.size);

		// 메세지 체크섬 검사
		if( pConn != g_pAuthConnection && pConn != g_pUploadConnection && !header.IsValidMessage() )
		{
			FILELOG( "!!!!! Invalid Message from %s[ID: %d, Size: %d]", pConn->GetPeerAddress().GetAddr(), header.id, header.size );
			_cprint( "!!!!! Invalid Message from %s[ID: %d, Size: %d]\n", pConn->GetPeerAddress().GetAddr(), header.id, header.size );
			pConn->Close();
			return;
		}

		if( header.size < sizeof( TS_MESSAGE ) || header.size > sizeof( szMessageBuf ) )
		{
			FILELOG( "!!!!! Message Too Big %s[ID: %d, Size: %d]", pConn->GetPeerAddress().GetAddr(), header.id, header.size );
			_cprint( "!!!!! Message Too Big %s[ID: %d, Size: %d]\n", pConn->GetPeerAddress().GetAddr(), header.id, header.size );
			pConn->Close();
			return;
		}

		// 덜받았다면 패스
		if( static_cast< int >( header.size ) > pConn->Size() ) return;

		pConn->Read( szMessageBuf, header.size );

		InterlockedIncrement( &g_nRecvMessageCount );
		InterlockedExchangeAdd( &g_nRecvMessageByte, pMsg->size );

		if( pMsg->id == TM_NONE )
			continue;

		char buf[255];
		s_sprintf( buf, _countof( buf ), "thread.iocp.%d.proc", nID );
		ENV().Set( buf, pMsg->id );

		s_sprintf( s_ThreadInfo.job_info, _countof( s_ThreadInfo.job_info ), "%d", pMsg->id );
		s_ThreadInfo.last_execute_time = GetArTime();

		SetMessagingInfo( pMsg->id, pMsg->size, false );

		if( pConn == g_pAuthConnection )
		{
			// 인증서버 메시지는 여기서 따로 처리.
			switch( pMsg->id )
			{
			case TM_AG_ITEM_PURCHASED:	onItemPurchased( static_cast< TS_AG_ITEM_PURCHASED * >( pMsg ) ); break;
			case TM_AG_ITEM_SUPPLIED:	onItemSupplied( static_cast< TS_AG_ITEM_SUPPLIED * >( pMsg ) ); break;
			case TM_AG_LOGIN_RESULT:	onAuthLoginResult( static_cast< TS_AG_LOGIN_RESULT * >( pMsg ) ); break;
			case TM_AG_CLIENT_LOGIN:	onAuthClientLoginResult( static_cast< TS_AG_CLIENT_LOGIN * >( pMsg ) ); break;
			case TM_AG_KICK_CLIENT:		onKickClient( static_cast< TS_AG_KICK_CLIENT * >( pMsg ) ); break;
			case TM_AG_PCBANG_EXPIRE_WARNING:	onPCBangExpireWarning( static_cast< TS_AG_PCBANG_EXPIRE_WARNING * >( pMsg ) ); break;
			case TM_AG_PCBANG_EXPIRED:	onPCBangExpired( static_cast< TS_AG_PCBANG_EXPIRE * >( pMsg ) ); break;
			case TM_AG_PCBANG_CHANGED:	onPCBangChanged( static_cast< TS_AG_PCBANG_CHANGED* >( pMsg ) ); break;
			case TM_AG_SECURITY_NO_CHECK: onSecurityNoCheck( static_cast< TS_AG_SECURITY_NO_CHECK * >( pMsg ) ); break;
			}

			continue;
		}

		if( pConn == g_pUploadConnection )
		{
			// 업로드 서버 메세지는 여기서 처리.
			switch( pMsg->id )
			{
			case TM_US_LOGIN_RESULT:	onUploadLoginResult( static_cast< TS_US_LOGIN_RESULT * >( pMsg ) ); break;
			case TM_US_REQUEST_UPLOAD:	onRequestUpload( static_cast< TS_US_REQUEST_UPLOAD * >( pMsg ) ); break;
			case TM_US_UPLOAD:			onUpload( static_cast< TS_US_UPLOAD * >( pMsg ) ); break;
			}

			continue;
		}

		StructPlayer *pClient = NULL;
		{
			THREAD_SYNCHRONIZE( g_ConnectionTagLock );

			_CONNECTION_TAG * pTag = reinterpret_cast< _CONNECTION_TAG * > ( pConn->GetTag() );

			pTag->nLastReadTime = GetArTime();
			pClient = pTag->pPlayer;

			if( pTag->nVersion < 0 && pMsg->id != TM_CS_VERSION && pMsg->id != TM_CS_REQUEST )
			{
				// _cprint( "버젼 인증 안됨.\n" );
				pConn->Close();
				return;
			}

			if( GameRule::nSecuritySolutionType != XSecuritySolutionManager::TYPE_NONE )
			{
				// 보안 솔루션 클라 세션 초기화가 완료되지 않은 상태로 캐릭터를 선택해서 로그인하려고 할 경우 접종
				if( pMsg->id == TM_CS_LOGIN && XSecuritySolutionManager::Instance().BlockInitIncompleteClientSession( pConn ) )
					return;

				// 패킷 받을 때마다 보안 솔루션 체크 진행
				if( !XSecuritySolutionManager::Instance().ProcValidation( pConn ) )
					return;
			}
		}

		// 플레이어가 메모리에서 Delete되는 것을 방지하기 위해서 iterator를 할당받아 refCount를 증가시켜 둠
		StructPlayer::iterator it;
		if( pClient )
		{
			it = StructPlayer::get( pClient->GetHandle() );

			if( !*it )
				return;
		}

		// if( pMsg->id == TM_CS_RETURN_LOBBY ) _cprint( "LOBBY MESSAGE!\n" );

		try
		{
			// 생성/소멸을 통하여 해당 패킷 처리 성능을 측정하는 방식
			NetworkReceiverPerformanceHelper helper( pMsg->id );

			if( !pClient )
			{
				switch( pMsg->id )
				{
				case TM_CS_REPORT:					onReport( pConn, static_cast< TS_CS_REPORT* >( pMsg ) ); break;
				case TM_CS_VERSION:					onVersion( pConn, static_cast< TS_CS_VERSION* >( pMsg ) ); break;
				case TM_CS_REQUEST:					onRequest( pConn, static_cast< TS_CS_REQUEST* >( pMsg ) ); break;

				case TM_CS_ANTI_HACK:				onHackShieldMsg( pConn, static_cast< TS_CS_ANTI_HACK* >( pMsg ) ); break;
				case TM_CS_GAME_GUARD_AUTH_ANSWER:	onGameGuardAuthAnswer( pConn, static_cast< TS_CS_GAME_GUARD_AUTH_ANSWER* >( pMsg ) ); break;
				case TM_CS_CHECK_ILLEGAL_USER:		onCheckIllegalUser( pConn, static_cast< TS_CS_CHECK_ILLEGAL_USER* >( pMsg ) ); break;
				case TM_CS_XTRAP_CHECK:				onXTrapCheck( pConn, static_cast< TS_CS_XTRAP_CHECK* >( pMsg ) ); break;

				case TM_CS_CREATE_CHARACTER:		onCreateCharacter( pConn, static_cast< TS_CS_CREATE_CHARACTER* >( pMsg ) ); break;
				case TM_CS_CHECK_CHARACTER_NAME:	onCheckCharacterName( pConn, static_cast< TS_CS_CHECK_CHARACTER_NAME* >( pMsg ) ); break;
				case TM_CS_DELETE_CHARACTER:		onDeleteCharacter( pConn, static_cast< TS_CS_DELETE_CHARACTER* >( pMsg ) ); break;
				case TM_CS_CHARACTER_LIST:			onCharacterList( pConn, static_cast< TS_CS_CHARACTER_LIST* >( pMsg ) ); break;
				//case TM_CS_ACCOUNT:					onAccount( pConn, static_cast< TS_CS_ACCOUNT* >( pMsg )  ); break;
				case TM_CS_LOGIN:					onLogin( pConn  , static_cast< TS_LOGIN* >( pMsg ) ); break;
				case TM_CS_ACCOUNT_WITH_AUTH:		onAccountWithAuth( pConn, static_cast< TS_CS_ACCOUNT_WITH_AUTH* >( pMsg ) ); break;
				case TM_CS_SECURITY_NO:				onSecurityNo( pConn, static_cast<TS_CS_SECURITY_NO*>( pMsg ) ); break;
					/*	2009. 8. 11 floyd 보안비밀번호 2차 수정
					case TM_CS_CREATE_SECURITY_NO:		onCreateSecurityNo( pConn, static_cast<TS_CS_CREATE_SECURITY_NO*>( pMsg ) ); break;
					case TM_CS_CHANGE_SECURITY_NO:		onChangeSecurityNo( pConn, static_cast<TS_CS_CHANGE_SECURITY_NO*>( pMsg ) ); break;
					case TM_CS_REQUEST_SECURITY_NO_CHANGE: onRequestSecurityNoChange( pConn, static_cast<TS_CS_REQUEST_SECURITY_NO_CHANGE*>( pMsg ) ); break;
					case TM_CS_REQUEST_CLEAR_SECURITY_NO:	onRequestClearSecurityNo( pConn, static_cast<TS_CS_REQUEST_CLEAR_SECURITY_NO*>( pMsg ) ); break;
					case TM_CS_CLEAR_SECURITY_NO:		onClearSecurityNo( pConn, static_cast<TS_CS_CLEAR_SECURITY_NO*>( pMsg ) ); break;
					*/
				case TM_NONE:
					{
						// do nothing.
						break;
					}
				default: 
					{
						FILELOG( "received message from unlogined connection : %d(%d)", pMsg->id, pMsg->size );
						_cprint( "received message from unlogined connection : %d(%d)\n", pMsg->id, pMsg->size );
						// pMsg->id = TM_NONE;
						break;
					}
				}
			}
			else
			{
				bool bIsProc = false;
				// 현재 쓰레드가 처리중인 플레이어 설정
				s_pCurrentPlayer = pClient;

				// 최우선적으로 처리해야 하는 보안 솔루션 관련 메시지(아래에서 해도 되지만 -_ -;)
				switch( pMsg->id )
				{
				case TM_CS_ANTI_HACK:					onHackShieldMsg( pConn, static_cast< TS_CS_ANTI_HACK* >( pMsg ) ); break;
				case TM_CS_GAME_GUARD_AUTH_ANSWER:		onGameGuardAuthAnswer( pConn, static_cast< TS_CS_GAME_GUARD_AUTH_ANSWER* >( pMsg ) ); break;
				case TM_CS_CHECK_ILLEGAL_USER:			onCheckIllegalUser( pConn, static_cast< TS_CS_CHECK_ILLEGAL_USER* >( pMsg ) ); break;
				case TM_CS_XTRAP_CHECK:					onXTrapCheck( pConn, static_cast< TS_CS_XTRAP_CHECK* >( pMsg ) ); break;
				}

				// 이하는 로그인 된 유저에 대해서만 처리할 수 있는 메시지
				// 로그인 하지 않았거나 로그아웃 된 유저라면 중지
				if( !pClient->IsLoginComplete() || pClient->GetLogoutTime() )
				{
					// 로그아웃 시스템이 변경되며, 접속 종료 이전에 로그아웃 처리가 먼저 되므로 발생 가능함
					//assert(0);
					return;
				}

				// 이하 트레이딩 혹은 창고 사용 중 또는 노점 중일때는 무시되는 명령어들
				if( !pClient->IsTrading() && !pClient->IsUsingStorage() && !pClient->IsBoothOpen() )
				{
					bIsProc = true;

					switch( pMsg->id )
					{
					case TM_CS_TAKEOUT_COMMERCIAL_ITEM: onTakeoutCommercialItem( pClient, static_cast< TS_CS_TAKEOUT_COMMERCIAL_ITEM * >( pMsg ) ); break;
					case TM_CS_TURN_ON_PK_MODE:		onTurnOnPkMode( pClient ); break;
					case TM_CS_TURN_OFF_PK_MODE:	onTurnOffPkMode( pClient ); break;
					case TM_CS_MOVE_REQUEST:		onMoveRequest( pClient, static_cast< TS_MOVE_REQUEST* >( pMsg ) ); break;
					case TM_CS_ATTACK_REQUEST:		onAttackRequest( pClient, static_cast< TS_ATTACK_REQUEST* >( pMsg ) ); break;
					case TM_CS_DIALOG:				onDialog( pClient, static_cast< TS_CS_DIALOG* >( pMsg ) ); break;					
					case TM_CS_SKILL:				onSkill( pClient, static_cast< TS_CS_SKILL* >( pMsg ) ); break;
					case TM_CS_CANCEL_ACTION:		onCancelAction( pClient, static_cast< TS_CS_CANCEL_ACTION* >( pMsg ) ); break;
					case TM_CS_ENTER_EVENT_AREA:	onEnterEventArea( pClient, static_cast< TS_CS_ENTER_EVENT_AREA* >( pMsg ) ); break;
					case TM_CS_LEAVE_EVENT_AREA:	onLeaveEventArea( pClient, static_cast< TS_CS_LEAVE_EVENT_AREA* >( pMsg ) ); break;

					case TM_CS_DROP_ITEM:			onDropItem( pClient, static_cast< TS_CS_DROP_ITEM* >( pMsg ) ); break;
					case TM_CS_TAKE_ITEM:			onTakeItem( pClient, static_cast< TS_CS_TAKE_ITEM* >( pMsg ) ); break;
					case TM_CS_ERASE_ITEM:			onEraseItem( pClient, static_cast< TS_CS_ERASE_ITEM* >( pMsg ) ); break;
					case TM_CS_CONTACT:				onContact( pClient, static_cast< TS_CS_CONTACT* >( pMsg ) ); break;
					case TM_CS_USE_ITEM:			onUseItem( pClient, static_cast< TS_CS_USE_ITEM* >( pMsg ) ); break;
					case TM_CS_BUY_ITEM:			onBuyItem( pClient, static_cast< TS_CS_BUY_ITEM* >( pMsg ) ); break;
					case TM_CS_SELL_ITEM:			onSellItem( pClient, static_cast< TS_CS_SELL_ITEM* >( pMsg ) ); break;
					case TM_CS_DONATE_ITEM:			onDonateItem( pClient, static_cast< TS_CS_DONATE_ITEM* >( pMsg ) ); break;
					case TM_CS_DONATE_REWARD:		onDonateReward( pClient, static_cast< TS_CS_DONATE_REWARD* >( pMsg ) ); break;
					case TM_CS_RANKING_TOP_RECORD:	onRankingTopRecord( pClient, static_cast< TS_CS_RANKING_TOP_RECORD* >( pMsg ) ); break;

					case TM_CS_JOB_LEVEL_UP:		onJobLevelUp( pClient, static_cast< TS_CS_JOB_LEVEL_UP* >( pMsg ) ); break;
					case TM_CS_LEARN_SKILL:			onLearnSkill( pClient, static_cast< TS_CS_LEARN_SKILL* >( pMsg ) ); break;
					case TM_CS_SUMMON:				onSummon( pClient, static_cast< TS_CS_SUMMON* >( pMsg ) ); break;							
					case TM_CS_MIX:					onMix( pClient, static_cast< TS_CS_MIX* >( pMsg ) ); break;
					case TM_CS_DECOMPOSE:			onDecompose( pClient, static_cast< TS_CS_DECOMPOSE* >( pMsg ) ); break;
					case TM_EQUIP_SUMMON:			onEquipSummon( pClient, static_cast< TS_EQUIP_SUMMON* >( pMsg ) ); break;
					case TM_CS_BIND_SKILLCARD:		onBindSkillCard( pClient, static_cast< TS_CS_BIND_SKILLCARD* >( pMsg ) ); break;
					case TM_CS_UNBIND_SKILLCARD:	onUnBindSkillCard( pClient, static_cast< TS_CS_UNBIND_SKILLCARD* >( pMsg ) ); break;

					case TM_CS_CHECK_BOOTH_STARTABLE:	onCheckBoothStartable( pClient ); break;
					case TM_CS_START_BOOTH:			onStartBooth( pClient, static_cast< TS_CS_START_BOOTH* >( pMsg ) ); break;

					case TM_CS_WATCH_BOOTH:			onWatchBooth( pClient, static_cast< TS_CS_WATCH_BOOTH* >( pMsg ) ); break;
					case TM_CS_GET_BOOTHS_NAME:		onGetBoothsName( pClient, static_cast< TS_CS_GET_BOOTHS_NAME* >( pMsg ) ); break;

					case TM_CS_STOP_WATCH_BOOTH:	onStopWatchBooth( pClient, static_cast< TS_CS_STOP_WATCH_BOOTH* >( pMsg ) ); break;
					case TM_CS_BUY_FROM_BOOTH:		onBuyFromBooth( pClient, static_cast< TS_CS_BUY_FROM_BOOTH* >( pMsg ) ); break;
					case TM_CS_SELL_TO_BOOTH:		onSellToBooth( pClient, static_cast< TS_CS_SELL_TO_BOOTH* >( pMsg ) ); break;

					case TM_CS_SOULSTONE_CRAFT:		onSoulStoneCraft( pClient, static_cast< TS_CS_SOULSTONE_CRAFT* >( pMsg ) ); break;
					case TM_CS_REPAIR_SOULSTONE:	onRepairSoulStone( pClient, static_cast< TS_CS_REPAIR_SOULSTONE* >( pMsg ) ); break;
					case TM_CS_TRANSMIT_ETHEREAL_DURABILITY:				onTransmitEtherealDurability( pClient, static_cast< TS_CS_TRANSMIT_ETHEREAL_DURABILITY * >( pMsg ) ); break;
					case TM_CS_TRANSMIT_ETHEREAL_DURABILITY_TO_EQUIPMENT:	onTransmitEtherealDurabilityToEquipment( pClient, static_cast< TS_CS_TRANSMIT_ETHEREAL_DURABILITY_TO_EQUIPMENT * >( pMsg ) ); break;

					case TM_CS_PUTON_ITEM_SET:		onPutonItemSet( pClient, static_cast< TS_CS_PUTON_ITEM_SET* >( pMsg ) ); break;
					case TM_CS_PUTON_ITEM:			onPutonItem( pClient, static_cast< TS_CS_PUTON_ITEM* >( pMsg ) ); break;
					case TM_CS_PUTOFF_ITEM:			onPutoffItem( pClient, static_cast< TS_CS_PUTOFF_ITEM* >( pMsg ) ); break;
					case TM_CS_SWAP_EQUIP:			onSwapEquip( pClient, static_cast< TS_CS_SWAP_EQUIP* >( pMsg ) ); break;
					case TM_CS_PUTON_CARD:			onPutonCard( pClient, static_cast< TS_CS_PUTON_CARD* >( pMsg ) ); break;
					case TM_CS_PUTOFF_CARD:			onPutoffCard( pClient, static_cast< TS_CS_PUTOFF_CARD* >( pMsg ) ); break;

					case TM_CS_CHANGE_SUMMON_NAME:	onChangeSummonName( pClient, static_cast< TS_CS_CHANGE_SUMMON_NAME* >( pMsg ) ); break;
					case TM_CS_GET_SUMMON_SETUP_INFO:	onGetSummonSetupInfo( pClient, static_cast< TS_CS_GET_SUMMON_SETUP_INFO* >( pMsg ) ); break;

					case TM_CS_SET_PET_NAME:		onSetPetName( pClient, static_cast< TS_CS_SET_PET_NAME* >( pMsg ) ); break;

					case TM_CS_REQUEST_REMOVE_STATE:	onRequestRemoveState( pClient, static_cast< TS_CS_REQUEST_REMOVE_STATE* >( pMsg ) ); break;

					case TM_CS_AUCTION_SEARCH:				onAuctionSearch( pClient, static_cast< TS_CS_AUCTION_SEARCH * >( pMsg ) ); break;
					case TM_CS_AUCTION_SELLING_LIST:		onAuctionSellingList( pClient, static_cast< TS_CS_AUCTION_SELLING_LIST * >( pMsg ) ); break;
					case TM_CS_AUCTION_BIDDED_LIST:			onAuctionBiddedList( pClient, static_cast< TS_CS_AUCTION_BIDDED_LIST * >( pMsg ) ); break;
					case TM_CS_AUCTION_BID:					onAuctionBid( pClient, static_cast< TS_CS_AUCTION_BID * >( pMsg ) ); break;
					case TM_CS_AUCTION_INSTANT_PURCHASE:	onAuctionInstantPurchase( pClient, static_cast< TS_CS_AUCTION_INSTANT_PURCHASE * >( pMsg ) ); break;
					case TM_CS_AUCTION_REGISTER:			onAuctionRegister( pClient, static_cast< TS_CS_AUCTION_REGISTER * >( pMsg ) ); break;
					case TM_CS_AUCTION_CANCEL:				onAuctionCancel( pClient, static_cast< TS_CS_AUCTION_CANCEL * >( pMsg ) ); break;
					case TM_CS_ITEM_KEEPING_LIST:			onItemKeepingList( pClient, static_cast< TS_CS_ITEM_KEEPING_LIST * >( pMsg ) ); break;
					case TM_CS_ITEM_KEEPING_TAKE:			onItemKeepingTake( pClient, static_cast< TS_CS_ITEM_KEEPING_TAKE * >( pMsg ) ); break;

					case TM_CS_HUNTAHOLIC_INSTANCE_LIST:	onHuntaholicInstanceList( pClient, static_cast< TS_CS_HUNTAHOLIC_INSTANCE_LIST * >( pMsg ) ); break;
					case TM_CS_HUNTAHOLIC_CREATE_INSTANCE:	onHuntaholicCreateInstance( pClient, static_cast< TS_CS_HUNTAHOLIC_CREATE_INSTANCE * >( pMsg ) ); break;
					case TM_CS_HUNTAHOLIC_JOIN_INSTANCE:	onHuntaholicJoinInstance( pClient, static_cast< TS_CS_HUNTAHOLIC_JOIN_INSTANCE * >( pMsg ) ); break;
					case TM_CS_HUNTAHOLIC_LEAVE_INSTANCE:	onHuntaholicLeaveInstance( pClient, static_cast< TS_CS_HUNTAHOLIC_LEAVE_INSTANCE * >( pMsg ) ); break;
					case TM_CS_HUNTAHOLIC_LEAVE_LOBBY:		onHuntaholicLeaveLobby( pClient ); break;
					case TM_CS_HUNTAHOLIC_BEGIN_HUNTING:	onHuntaholicBeginHunting( pClient ); break;

					case TM_CS_INSTANCE_GAME_ENTER:			onInstanceGameEnter( pClient, static_cast< TS_CS_INSTANCE_GAME_ENTER * >( pMsg ) ); break;
					case TM_CS_INSTANCE_GAME_EXIT:			onInstanceGameExit( pClient ); break;

					case TM_CS_COMPETE_REQUEST:				onCompeteRequest( pClient, static_cast< TS_CS_COMPETE_REQUEST * >( pMsg ) ); break;
					case TM_CS_COMPETE_ANSWER:				onCompeteAnswer( pClient, static_cast< TS_CS_COMPETE_ANSWER * >( pMsg ) ); break;

					case TM_CS_REQUEST_FARM_INFO:			onRequestFarmInfo( pClient, static_cast< TS_CS_REQUEST_FARM_INFO * >( pMsg ) ); break;
					case TM_CS_FOSTER_CREATURE:				onFosterCreature( pClient, static_cast< TS_CS_FOSTER_CREATURE * >( pMsg ) ); break;
					case TM_CS_RETRIEVE_CREATURE:			onRetrieveCreature( pClient, static_cast< TS_CS_RETRIEVE_CREATURE * >( pMsg ) ); break;
					case TM_CS_NURSE_CREATURE:				onNurseCreature( pClient, static_cast< TS_CS_NURSE_CREATURE * >( pMsg ) ); break;
					case TM_CS_REQUEST_FARM_MARKET:			onRequestFarmMarket( pClient ); break;

					default : bIsProc= false; break;
					}
				}

				// 트레이딩 중일때는 안되고 창고 사용 중일때는 사용 가능한 명령들
				if( !pClient->IsTrading() && !pClient->IsBoothOpen() && pClient->IsUsingStorage() )
				{
					bIsProc = true;

					switch( pMsg->id )
					{
					case TM_CS_PUTON_ITEM_SET:		onPutonItemSet( pClient, static_cast< TS_CS_PUTON_ITEM_SET* >( pMsg ) ); break;
					case TM_CS_PUTON_ITEM:			onPutonItem( pClient, static_cast< TS_CS_PUTON_ITEM* >( pMsg ) ); break;
					case TM_CS_PUTOFF_ITEM:			onPutoffItem( pClient, static_cast< TS_CS_PUTOFF_ITEM* >( pMsg ) ); break;
					case TM_CS_SWAP_EQUIP:			onSwapEquip( pClient, static_cast< TS_CS_SWAP_EQUIP * >( pMsg ) ); break;
					case TM_CS_PUTON_CARD:			onPutonCard( pClient, static_cast< TS_CS_PUTON_CARD* >( pMsg ) ); break;
					case TM_CS_PUTOFF_CARD:			onPutoffCard( pClient, static_cast< TS_CS_PUTOFF_CARD* >( pMsg ) ); break;
					case TM_CS_ERASE_ITEM:			onEraseItem( pClient, static_cast< TS_CS_ERASE_ITEM* >( pMsg ) ); break;
					case TM_CS_MIX:					onMix( pClient, static_cast< TS_CS_MIX* >( pMsg ) ); break;
					case TM_CS_DECOMPOSE:			onDecompose( pClient, static_cast< TS_CS_DECOMPOSE* >( pMsg ) ); break;
					default : bIsProc= false; break;
					}
				}

				// 이하는 트레이딩 & 창고 이용 중에도 가능한 명령어들
				if( !bIsProc )
				{
					if( !pClient->IsBoothOpen() )	// 노점 중에는 불가능한 메시지
					{
						bIsProc = true;

						switch( pMsg->id )
						{
						case TM_CS_STORAGE:			onStorage( pClient, static_cast< TS_CS_STORAGE* >( pMsg ) ); break;
						case TM_TRADE:				onTrade( pClient, static_cast< TS_TRADE* >( pMsg ) ); break;
						default: bIsProc = false; break;

						}
					}

					if( !bIsProc )
					{
						bIsProc = true;

						switch( pMsg->id )
						{
						case TM_CS_CHANGE_ITEM_POSITION:	onItemPositionChange( pClient, static_cast< TS_CS_CHANGE_ITEM_POSITION * >( pMsg ) ); break;
						case TM_CS_ARRANGE_ITEM:			onArrangeItem( pClient, static_cast< TS_CS_ARRANGE_ITEM* >( pMsg ) ); break;
						case TM_CS_GET_REGION_INFO: 		onGetRegionInfo( pClient, static_cast< TS_CS_GET_REGION_INFO* >( pMsg ) ); break;
						case TM_CS_RESURRECTION:			onResurrection( pClient, static_cast< TS_CS_RESURRECTION* >( pMsg ) ); break;
						case TM_CS_DROP_QUEST:				onDropQuest( pClient, static_cast< TS_CS_DROP_QUEST* >( pMsg ) ); break;
						case TM_CS_QUEST_INFO:				onQuestInfo( pClient, static_cast< TS_CS_QUEST_INFO* >( pMsg ) ); break;
						case TM_CS_END_QUEST:				onEndQuest( pClient, static_cast< TS_CS_END_QUEST* >( pMsg ) ); break;
						case TM_CS_SET_MAIN_TITLE:			onSetMainTitle( pClient, static_cast< TS_CS_SET_MAIN_TITLE* >( pMsg ) ); break;
						case TM_CS_SET_SUB_TITLE:			onSetSubTitle( pClient, static_cast< TS_CS_SET_SUB_TITLE* >( pMsg ) ); break;
						case TM_CS_BOOKMARK_TITLE:			onBookmarkTitle( pClient, static_cast< TS_CS_BOOKMARK_TITLE* >( pMsg ) ); break;
						case TM_CS_TARGETING:				onTargeting( pClient, static_cast< TS_CS_TARGETING* >( pMsg ) ); break;
						case TM_CS_STOP_BOOTH:				onStopBooth( pClient ); break;
						case TM_CS_QUERY:					onQuery( pClient, static_cast< TS_CS_QUERY* >( pMsg ) ); break;
						case TM_CS_REGION_UPDATE:			onRegionUpdate( pClient, static_cast< TS_REGION_UPDATE* >( pMsg ) ); break;
						case TM_CS_UPDATE:					onUpdate( pClient, static_cast< TS_CS_UPDATE* >( pMsg ) ); break;						
						case TM_CS_CHAT_REQUEST:			onChatRequest( pClient, static_cast< TS_CS_CHAT_REQUEST* >( pMsg ) ); break;						
						case TM_TIMESYNC:					onTimeSync( pClient, static_cast< TS_TIMESYNC* >( pMsg ) ); break;
						case TM_CS_GAME_TIME:				onGameTime( pClient ); break;
						case TM_CS_EMOTION:					onEmotion( pClient, static_cast< TS_CS_EMOTION* >( pMsg ) ); break;
						case TM_CS_SET_PROPERTY:			onSetProperty( pClient, static_cast< TS_CS_SET_PROPERTY* >( pMsg ) ); break;
						case TM_CS_CHANGE_LOCATION: 		onChangeLocation( pClient, static_cast< TS_CS_CHANGE_LOCATION * >( pMsg ) ); break;
						case TM_CS_GET_WEATHER_INFO:		onGetWeaterInfo( pClient, static_cast< TS_CS_GET_WEATHER_INFO * >( pMsg ) ); break;
						case TM_CS_OPEN_ITEM_SHOP:			onOpenItemShop( pClient ); break;
						case TM_CS_GET_BOOTHS_NAME:			onGetBoothsName( pClient, static_cast< TS_CS_GET_BOOTHS_NAME* >( pMsg ) ); break;
						case TM_CS_MONSTER_RECOGNIZE:		onMonsterRecognize( pClient, static_cast< TS_CS_MONSTER_RECOGNIZE* >( pMsg ) ); break;
						case TM_CS_SUMMON_CARD_SKILL_LIST:	onSummonCardSkillList( pClient, static_cast< TS_CS_SUMMON_CARD_SKILL_LIST* >( pMsg ) ); break;

						case TM_CS_SECURITY_NO:				onSecurityNo( pConn, static_cast<TS_CS_SECURITY_NO*>( pMsg ) ); break;

						case TM_CS_REQUEST_RETURN_LOBBY:
						case TM_CS_REQUEST_LOGOUT:			onRequestLogoutTimer( pClient, pMsg->id ); break;
						case TM_CS_RETURN_LOBBY:			onReturnLobby( pConn, pClient ); break;
						case TM_CS_LOGOUT:					onLogout( pClient, static_cast<TS_CS_LOGOUT*>( pMsg ) ); break;

						case TM_CS_INSTANCE_GAME_SCORE_REQUEST:			onInstanceGameScoreRequest( pClient ); break;
						case TM_CS_CHANGE_ALIAS:						onChangeAlias( pClient, static_cast< TS_CS_CHANGE_ALIAS * >( pMsg ) ); break;
						case TM_CS_BATTLE_ARENA_JOIN_QUEUE:				onBattleArenaJoinQueue( pClient, static_cast< TS_CS_BATTLE_ARENA_JOIN_QUEUE * >( pMsg ) ); break;
						case TM_CS_BATTLE_ARENA_LEAVE:					onBattleArenaLeave( pClient ); break;
						case TM_CS_BATTLE_ARENA_ENTER_WHILE_COUNTDOWN:	onBattleArenaEnterWhileCountdown( pClient ); break;
						case TM_CS_BATTLE_ARENA_EXERCISE_READY:			onBattleArenaExerciseReady( pClient, static_cast< TS_CS_BATTLE_ARENA_EXERCISE_READY * >( pMsg ) ); break;
						case TM_CS_BATTLE_ARENA_EXERCISE_START:			onBattleArenaExerciseStart( pClient ); break;
						case TM_CS_BATTLE_ARENA_ABSENCE_CHECK_REQUEST:	onBattleArenaAbsenceCheckRequest( pClient, static_cast< TS_CS_BATTLE_ARENA_ABSENCE_CHECK_REQUEST * >( pMsg ) ); break;
						case TM_CS_BATTLE_ARENA_ABSENCE_CHECK_ANSWER:	onBattleArenaAbsenceCheckAnswer( pClient, static_cast< TS_CS_BATTLE_ARENA_ABSENCE_CHECK_ANSWER * >( pMsg ) ); break;

						case TM_CS_HIDE_EQUIP_INFO:			onHideEquipInfo( pClient, static_cast< TS_CS_HIDE_EQUIP_INFO * >( pMsg ) ); break;

						// party matching
						case TM_CS_PARTYMATCH_ACTION:
							onPartyMatchingAction(pClient, static_cast<TS_CS_PARTYMATCH_ACTION *>(pMsg));
							break;

						case TM_NONE:
						default: bIsProc = false; break;
						}
					}
				}

				if( bIsProc )
				{
					if( pClient->IsInvincible() &&
						pMsg->id != TM_CS_QUERY &&
						pMsg->id != TM_CS_REGION_UPDATE &&
						pMsg->id != TM_CS_UPDATE &&
						pMsg->id != TM_TIMESYNC &&
						pMsg->id != TM_CS_SET_PROPERTY &&
						pMsg->id != TM_CS_GET_WEATHER_INFO &&
						pMsg->id != TM_CS_GET_BOOTHS_NAME &&
						pMsg->id != TM_CS_GAME_TIME &&
						pMsg->id != TM_CS_CHANGE_LOCATION &&
						pMsg->id != TM_CS_CHAT_REQUEST &&
						pMsg->id != TM_CS_DIALOG && 
						pMsg->id != TM_CS_RESURRECTION &&
						pMsg->id != TM_CS_CANCEL_ACTION &&
						pMsg->id != TM_CS_MONSTER_RECOGNIZE &&
						( pMsg->id != TM_CS_TARGETING ||
						static_cast< TS_CS_TARGETING * >( pMsg )->target != 0 ) )
					{
						pClient->SetInvincible( false );

						StructSummon * pSummon = pClient->GetMainSummon();

						if( pSummon )
						{
							pSummon->SetInvincible( false );
						}

						pSummon = pClient->GetSubSummon();

						if( pSummon )
						{
							pSummon->SetInvincible( false );
						}
					}
				}
			}
		}
		catch( std::exception & ex )
		{
			FILELOG( "ERROR : %s", ex.what() );
			_cprint( "ERROR : %s\n", ex.what() );
		}

	}
}