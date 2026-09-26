// 트레이드는 복잡하고 아주 중요한 것이라 GameMessage 에 안넣고 따로 빼놓았음.

#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>

#include "ErrorCode/ErrorCode.h"


#include "GameMessage.h"
#include "SendMessage.h"
#include "StructPlayer.h"
#include "StructItem.h"


static StructPlayer::iterator find_trade_target( StructPlayer *pClient, AR_HANDLE target )
{
	// 타겟 찾는다
	StructPlayer::iterator pit( StructPlayer::get( target ) );
	if( !(*pit) || !(*pit)->IsPlayer() || !(*pit)->IsInWorld() )
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return NULL;
	}

	// 거리 체크
	if( pClient->GetMv().GetDistance( (*pit)->GetMv() ) > g_nRegionSize )
	{
		SendResult( pClient, TM_TRADE, RESULT_TOO_FAR );
		return NULL;
	}

	return pit;
}


void onRequestTrade( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );
	
	StructPlayer::iterator pit = find_trade_target( pClient, pMsg->target_player );
	if( !(*pit) ) return;

	if( (*pit)->IsTrading() || pClient->IsTrading() )
	{
		SendResult( pClient, TM_TRADE, RESULT_ACCESS_DENIED );
		return;
	}

	// PK 룰 및 Demoniac/Bloody 관련 커뮤니티 룰 체크
	if( !pClient->IsTradableWith( (*pit) ) )
	{
		SendResult( pClient, TM_TRADE, RESULT_PK_LIMIT );
		return;
	}

	//if( pClient->IsPKOn() || (*pit)->IsPKOn() )
	//{
	//	if( !pClient->GetPartyID() || ( pClient->GetPartyID() != (*pit)->GetPartyID() ) )
	//	{
	//		SendResult( pClient, TM_TRADE, RESULT_ACCESS_DENIED );
	//		return;
	//	}
	//}

	//if( !GameRule::bIsPKServer && ( pClient->IsDemoniacCharacter() || pClient->IsBloodyCharacter() || (*pit)->IsDemoniacCharacter() || (*pit)->IsBloodyCharacter() ) )
	//{
	//	if( (!pClient->GetPartyID() || ( pClient->GetPartyID() != (*pit)->GetPartyID() ) ) && (!pClient->GetGuildID() || (pClient->GetGuildID() != (*pit)->GetGuildID() ) ) )
	//	{
	//		SendResult( pClient, TM_TRADE, RESULT_PK_LIMIT );
	//		return;
	//	}
	//}

	TS_TRADE msg;
	msg.mode = TS_TRADE::REQUEST_TRADE;
	msg.target_player = pClient->GetHandle();
	PendMessage( (*pit), &msg );
}

void onAcceptTrade( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructPlayer::iterator pit = find_trade_target( pClient, pMsg->target_player );
	if( !(*pit) ) return;

	if( (*pit)->IsTrading() || pClient->IsTrading() )
	{
		SendResult( pClient, TM_TRADE, RESULT_ACCESS_DENIED );
		return;
	}

	if( (*pit)->GetTamingTarget() || pClient->GetTamingTarget() )
	{
		SendResult( pClient, TM_TRADE, RESULT_ACCESS_DENIED );
		return;
	}

	TS_TRADE msg;

	pClient->StartTrade( *pit );
	(*pit)->StartTrade( pClient );

	msg.mode = TS_TRADE::BEGIN_TRADE;
	msg.target_player = pClient->GetHandle();
	PendMessage( (*pit), &msg );
	msg.target_player = (*pit)->GetHandle();
	PendMessage( pClient, &msg );
}

void onRejectTrade( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructPlayer::iterator pit = find_trade_target( pClient, pMsg->target_player );
	if( !(*pit) ) return;
	
	TS_TRADE msg;
	msg.mode = TS_TRADE::REJECT_TRADE;
	msg.target_player = pClient->GetHandle();
	PendMessage( (*pit), &msg );
}

void onCancelTrade( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	if( pClient->IsTrading() )
	{
		StructPlayer::iterator itTradeTarget = StructPlayer::get( pClient->GetTradeTarget() );
		StructPlayer * pTradeTarget = (*itTradeTarget);

		if( pTradeTarget )
			pTradeTarget->CancelTrade( true );

		pClient->CancelTrade( true );
	}
}

void onConfirmTrade( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructPlayer::iterator pit = find_trade_target( pClient, pClient->GetTradeTarget() );
	StructPlayer * pTradeTarget = (*pit);
	if( pTradeTarget == NULL )
	{
		// 거리를 벋어났을 경우
		pit = StructPlayer::get( pClient->GetTradeTarget() );
		pTradeTarget = (*pit);
		if( pTradeTarget )
		{
			SendResult( pTradeTarget, TM_TRADE, RESULT_ACCESS_DENIED );
			pTradeTarget->CancelTrade();
		}

		pClient->CancelTrade( true );
		return;
	}

	// 뭔가 이상
	if( !pTradeTarget || !pTradeTarget->IsTrading() || !pClient->IsTrading() ||
		!pTradeTarget->IsTradeFreezed() || !pClient->IsTradeFreezed() ||
		pTradeTarget->GetTradeTarget() != pClient->GetHandle() )
	{
		if( pTradeTarget )
			pTradeTarget->CancelTrade( true );

		pClient->CancelTrade( true );
		return;
	}

	// 이미 최종확인 눌렀으면 KIN
	if( pClient->IsTradeConfirm() ) return;

	pClient->ConfirmTrade();

	// 최종확인 누른거 방송
	TS_TRADE msg;
	msg.mode = TS_TRADE::CONFIRM_TRADE;
	msg.target_player = pClient->GetHandle();
	PendMessage( pClient, &msg );
	PendMessage( pTradeTarget, &msg );

	// 상대방은 최종확인 안눌렀으면 KIN
	if( !pTradeTarget->IsTradeConfirm() ) return;

	// 거래 완료될 경우의 무게 검사
	if( !pTradeTarget->CheckTradeWeight() || !pClient->CheckTradeWeight() )
	{
		pClient->CancelTrade();
		pTradeTarget->CancelTrade();

		SendResult( pClient, TM_TRADE, RESULT_TOO_HEAVY );
		SendResult( pTradeTarget, TM_TRADE, RESULT_TOO_HEAVY );		
		return;
	}

	if( !pTradeTarget->CheckTradeItem() || !pClient->CheckTradeItem() )
	{
		pClient->CancelTrade();
		pTradeTarget->CancelTrade();

		SendResult( pClient, TM_TRADE, RESULT_ACCESS_DENIED );
		SendResult( pTradeTarget, TM_TRADE, RESULT_ACCESS_DENIED );

		return;
	}

	// 거래 완료될 경우의 소지 금액 오버 플로우 검사
	bool bExceedGoldTradeTarget = pTradeTarget->GetGold() + pClient->GetTradeGold() > GameRule::MAX_GOLD_FOR_INVENTORY;
	bool bExceedGold = pClient->GetGold() + pTradeTarget->GetTradeGold() > GameRule::MAX_GOLD_FOR_INVENTORY;
	if( bExceedGoldTradeTarget || bExceedGold )
	{
		pClient->CancelTrade();
		pTradeTarget->CancelTrade();

		if( bExceedGoldTradeTarget )
		{
			SendResult( pClient, TM_TRADE, RESULT_TOO_MUCH_MONEY, pTradeTarget->GetHandle() );
			SendResult( pTradeTarget, TM_TRADE, RESULT_TOO_MUCH_MONEY, pTradeTarget->GetHandle() );
		}
		if( bExceedGold )
		{
			SendResult( pClient, TM_TRADE, RESULT_TOO_MUCH_MONEY, pClient->GetHandle() );
			SendResult( pTradeTarget, TM_TRADE, RESULT_TOO_MUCH_MONEY, pClient->GetHandle() );
		}
		return;
	}

	if( pTradeTarget->IsTrading() && pClient->IsTrading() &&
		pTradeTarget->IsTradeFreezed() && pClient->IsTradeFreezed() &&
		pClient->GetTradeTarget() == pTradeTarget->GetHandle() &&
		pTradeTarget->GetTradeTarget() == pClient->GetHandle() )
	{
		if( pClient->ProcessTrade() )
		{
			msg.mode = TS_TRADE::PROCESS_TRADE;
			msg.target_player = pClient->GetHandle();
			PendMessage( pTradeTarget, &msg );
			PendMessage( pClient, &msg );
		}
	}

	pTradeTarget->CancelTrade();
	pClient->CancelTrade();
}

void onFreezeTrade( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructPlayer::iterator pit = find_trade_target( pClient, pClient->GetTradeTarget() );
	StructPlayer * pTradeTarget = (*pit);
	if( pTradeTarget == NULL )
	{
		// 거리를 벋어났을 경우
		pit = StructPlayer::get( pClient->GetTradeTarget() );
		pTradeTarget = (*pit);
		if( pTradeTarget )
		{
			SendResult( pTradeTarget, TM_TRADE, RESULT_ACCESS_DENIED );
			pTradeTarget->CancelTrade();
		}

		pClient->CancelTrade( true );
		return;
	}

	pClient->FreezeTrade();

	TS_TRADE msg;

	msg.mode = TS_TRADE::FREEZE_TRADE;
	msg.target_player = pClient->GetHandle();
	PendMessage( pTradeTarget, &msg );
	PendMessage( pClient, &msg );
}

void onAddItem( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructPlayer::iterator pit = find_trade_target( pClient, pClient->GetTradeTarget() );
	StructPlayer * pTradeTarget = (*pit);
	if( pTradeTarget == NULL )
	{
		// 거리를 벋어났을 경우
		pit = StructPlayer::get( pClient->GetTradeTarget() );
		pTradeTarget = (*pit);
		if( pTradeTarget )
		{
			SendResult( pTradeTarget, TM_TRADE, RESULT_ACCESS_DENIED );
			pTradeTarget->CancelTrade();
		}

		pClient->CancelTrade( true );
		return;
	}

	if( pClient->IsTradeFreezed() ) return;

	StructItem *pItem = pClient->FindItem( pMsg->item_info.uid );

	if( 0 > pMsg->item_info.count )
	{
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Add Trade Bug [%s:%s]", pClient->GetAccountName(), pClient->GetName() );

		GameRule::RegisterBlockAccount( pClient->GetAccountName() );
		//if( pClient->pConnection && pClient->pConnection->IsConnected() )
		//	pClient->pConnection->Close();

		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}

	if( !pItem || pItem->GetCount() < pMsg->item_info.count )
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}

	// 장착된건 KIN
	if( !pClient->IsTradable( pItem ) )
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_ACTABLE );
		return;
	}

	if( pClient->AddItemToTradeWindow( pItem, pMsg->item_info.count ) )
	{
		TS_TRADE msg;
		msg.mode = TS_TRADE::ADD_ITEM;
		fillItemInfo( &msg.item_info, pItem );
		msg.item_info.count = pMsg->item_info.count;
		msg.target_player = pClient->GetHandle();
		PendMessage( pTradeTarget, &msg );
		PendMessage( pClient, &msg );
	}
	else
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}
}

void onRemoveItem( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructPlayer::iterator pit = find_trade_target( pClient, pClient->GetTradeTarget() );
	StructPlayer * pTradeTarget = (*pit);
	if( pTradeTarget == NULL )
	{
		// 거리를 벋어났을 경우
		pit = StructPlayer::get( pClient->GetTradeTarget() );
		pTradeTarget = (*pit);
		if( pTradeTarget )
		{
			SendResult( pTradeTarget, TM_TRADE, RESULT_ACCESS_DENIED );
			pTradeTarget->CancelTrade();
		}

		pClient->CancelTrade( true );
		return;
	}

	if( pClient->IsTradeFreezed() ) return;

	StructItem *pItem = pClient->FindItem( pMsg->item_info.uid );

	if( !pItem )
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}

	if( pClient->RemoveItemFromTradeWindow( pItem, pMsg->item_info.count ) )
	{
		TS_TRADE msg;
		msg.mode = TS_TRADE::REMOVE_ITEM;
		fillItemInfo( &msg.item_info, pItem );
		msg.item_info.count = pMsg->item_info.count;
		msg.target_player = pClient->GetHandle();
		PendMessage( pTradeTarget, &msg );
		PendMessage( pClient, &msg );
	}
	else
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}
}

void onAddGold( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructPlayer::iterator pit = find_trade_target( pClient, pClient->GetTradeTarget() );
	StructPlayer * pTradeTarget = (*pit);
	if( pTradeTarget == NULL )
	{
		// 거리를 벋어났을 경우
		pit = StructPlayer::get( pClient->GetTradeTarget() );
		pTradeTarget = (*pit);
		if( pTradeTarget )
		{
			SendResult( pTradeTarget, TM_TRADE, RESULT_ACCESS_DENIED );
			pTradeTarget->CancelTrade();
		}

		pClient->CancelTrade( true );
		return;
	}

	if( pClient->IsTradeFreezed() ) return;

	if( 0 > pMsg->item_info.count )
	{
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Add gold Trade Bug [%s:%s]", pClient->GetAccountName(), pClient->GetName() );

		GameRule::RegisterBlockAccount( pClient->GetAccountName() );
		//if( pClient->pConnection && pClient->pConnection->IsConnected() )
		//	pClient->pConnection->Close();

		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}

	if( pClient->GetGold() < pMsg->item_info.count )
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}

	pClient->AddGoldToTradeWindow( pMsg->item_info.count );

	TS_TRADE msg;
	msg.mode = TS_TRADE::ADD_GOLD;
	msg.item_info.count = pMsg->item_info.count;
	msg.target_player = pClient->GetHandle();
	PendMessage( pTradeTarget, &msg );
	PendMessage( pClient, &msg );
}

void onModifyCount( StructPlayer* pClient, TS_TRADE* pMsg )
{
	ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pClient ) );

	StructPlayer::iterator pit = find_trade_target( pClient, pClient->GetTradeTarget() );
	StructPlayer * pTradeTarget = (*pit);
	if( pTradeTarget == NULL )
	{
		// 거리를 벋어났을 경우
		pit = StructPlayer::get( pClient->GetTradeTarget() );
		pTradeTarget = (*pit);
		if( pTradeTarget )
		{
			SendResult( pTradeTarget, TM_TRADE, RESULT_ACCESS_DENIED );
			pTradeTarget->CancelTrade();
		}

		pClient->CancelTrade( true );
		return;
	}

	if( pClient->IsTradeFreezed() ) return;

	StructItem *pItem = pClient->FindItem( pMsg->item_info.uid );

	if( 0 > pMsg->item_info.count )
	{
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Add Trade Bug [%s:%s]", pClient->GetAccountName(), pClient->GetName() );

		GameRule::RegisterBlockAccount( pClient->GetAccountName() );
		//if( pClient->pConnection && pClient->pConnection->IsConnected() )
		//	pClient->pConnection->Close();

		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}

	if( !pItem || pItem->GetCount() < pMsg->item_info.count )
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}

	// 장착된건 KIN
	if( !pClient->IsTradable( pItem ) )
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_ACTABLE );
		return;
	}

	if( pClient->ModifyItemCountInTradeWindow( pItem, pMsg->item_info.count ) )
	{
		TS_TRADE msg;
		msg.mode = TS_TRADE::MODIFY_COUNT;
		fillItemInfo( &msg.item_info, pItem );
		msg.item_info.count = pMsg->item_info.count;
		msg.target_player = pClient->GetHandle();
		PendMessage( pTradeTarget, &msg );
		PendMessage( pClient, &msg );
	}
	else
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_EXIST );
		return;
	}
}

const ErrorCode checkUsingStorage( StructPlayer *pClient, TS_TRADE* pMsg )
{
	// 둘 중 하나라도 창고 사용중인 경우 트레이드 불가
	if( pClient->IsUsingStorage() ) return RESULT_NOT_ACTABLE_WHILE_USING_STORAGE;

	StructPlayer::iterator itTradeTarget = StructPlayer::get( pMsg->target_player );
	StructPlayer * pTradeTarget = *itTradeTarget;

	// 일단 클라이언트에서 한번 필터링을 하겠지만, 서버에서도 한번 더 검증해본다
	if( pTradeTarget )
	{
		return pTradeTarget->IsUsingStorage() ? RESULT_TARGET_IS_USING_STORAGE : RESULT_SUCCESS;
	}
	else
	{
		return RESULT_NOT_EXIST;
	}	
}

void onTrade( StructPlayer *pClient, TS_TRADE* pMsg )
{
	if( GameRule::bDisableTrade )
	{
		SendResult( pClient, TM_TRADE, RESULT_NOT_ACTABLE );
		return;
	}

	ErrorCode code = checkUsingStorage( pClient, pMsg );

	if( code != RESULT_SUCCESS )
	{
		SendResult( pClient, pMsg->id, code );
		return;
	}

	// Trading Fix
	AR_TIME t = GetArTime();
	if (pClient->IsMoving(t))
	{
		pClient->CancelTrade();
		return;
	}

	// 자기 자신에게 거래 메시지는 처리 불가
	if( pMsg->target_player == pClient->GetHandle() )
	{
		StructPlayer::iterator itTradeTarget = StructPlayer::get( pClient->GetTradeTarget() );
		StructPlayer * pTradeTarget = (*itTradeTarget);

		if( pTradeTarget )
		{
			pTradeTarget->CancelTrade();
		}

		pClient->CancelTrade();
		return;
	}

	switch( pMsg->mode )
	{
		case TS_TRADE::REQUEST_TRADE :	onRequestTrade( pClient, pMsg );	break;
		case TS_TRADE::ACCEPT_TRADE  :	onAcceptTrade( pClient, pMsg );		break;
		case TS_TRADE::CANCEL_TRADE  :	onCancelTrade( pClient, pMsg );		break;
		case TS_TRADE::REJECT_TRADE  :	onRejectTrade( pClient, pMsg );		break;
		case TS_TRADE::ADD_ITEM	     :	onAddItem( pClient, pMsg );			break;
		case TS_TRADE::REMOVE_ITEM   :	onRemoveItem( pClient, pMsg );		break;
		case TS_TRADE::ADD_GOLD      :	onAddGold( pClient, pMsg );			break;
		case TS_TRADE::FREEZE_TRADE  :	onFreezeTrade( pClient, pMsg );		break;
		case TS_TRADE::CONFIRM_TRADE :	onConfirmTrade( pClient, pMsg );	break;
		case TS_TRADE::MODIFY_COUNT  :  onModifyCount( pClient, pMsg );		break;
	};
}