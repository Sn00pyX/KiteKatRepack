
#include <vector>

#include <mmo/ArcadiaServer.h>
#include <toolkit/ILock.h>
#include <mmo/ArOption.h>
#include <toolkit/XConsole.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "ScriptItem.h"
#include "StructItem.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "GameProc.h"
#include "GameContent.h"
#include "SendMessage.h"
#include "ScriptCommon.h"
#include "DB_Commands.h"
#include "StructEventItemManager.h"
#include "RandomManager.h"
#include "MixManager.h"

#include <lua/lua.hpp>

extern XCriticalSection						g_Lock;
extern std::vector< StructItem* >			g_vItem;

int SCRIPT_DropItem( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);			// number of arguments

	if( nArgCnt < 4 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DropItem() : invalid argument" );
		return 1;
	}

	int x = (int)lua_tonumber( L, 1 );
	int y = (int)lua_tonumber( L, 2 );
	unsigned char layer = (unsigned char)lua_tonumber( L, 3 );
	int code;

	if( lua_isnumber(L, 4) ) code = (int)lua_tonumber( L, 4 );
	else code = StructItem::GetItemCode( lua_tostring_utf8( L, 4 ).c_str() );

	if( !StructItem::IsValidItemCode( code ) )
	{
		return 0;
	}

	__int64 count = ( nArgCnt >= 5 && lua_isnumber( L, 5 ) ) ? lua_tonumber( L, 5 ) : 1;

	// 이 세 개의 변수는 AllocItem에서 -1로 들어가야 DB에 정의된 기본 값을 사용하도록 되어 있음(구리지만... ㅠㅜ;)
	int nEnhance = ( nArgCnt >= 6 && lua_isnumber( L, 6 ) ) ? lua_tonumber( L, 6 ) : -1;
	int nLevel = ( nArgCnt >= 7 && lua_isnumber( L, 7 ) ) ? lua_tonumber( L, 7 ) : -1;
	int nFlag = ( nArgCnt >= 8 && lua_isnumber( L, 8 ) ) ? lua_tonumber( L, 8 ) : -1;

	// 아이템 생성
	StructItem *pItem = StructItem::AllocItem( 0, code, count, ItemInstance::BY_SCRIPT, nLevel, nEnhance, nFlag );

	// 이벤트로 드랍한 아이템일 경우 이벤트 플래그 설정
	if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_EVENT ) )
	{
		pItem->SetEventDropFlag( true );
	}

	// 좌표 설정
	pItem->SetCurrentXY( x, y );
	pItem->SetCurrentLayer( layer );

	// 지역 락이 중첩해서 걸리는 문제를 방지하기 위해 월드에 등장시키는 걸 ObjectRespawner에게 일임
	ObjectRespawner * pRespawner = new ObjectRespawner( pItem );
	ArcadiaServer::Instance().SetObjectPriority( pRespawner, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	return 0;
}


int SCRIPT_DropGold( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 3 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		lua_pushstring( L, "invalid argument" );
		LUA()->Log( "SCRIPT_DropGold() : invalid argument" );
		return 1;
	}

	int x = (int)lua_tonumber( L, 1 );
	int y = (int)lua_tonumber( L, 2 );
	StructGold cnt( lua_tonumber( L, 3 ) );

	if( cnt <= 0 )
	{
		lua_pushstring( L, "invalid gold argument" );
		LUA()->Log( "SCRIPT_DropGold() : invalid gold argument" );
		return 1;
	}

	// 돈 생성
	StructItem *pItem = StructItem::AllocGold( cnt, ItemInstance::BY_SCRIPT );

	// 좌표 설정
	pItem->SetCurrentXY( x, y );

	// 지역 락이 중첩해서 걸리는 문제를 방지하기 위해 월드에 등장시키는 걸 ObjectRespawner에게 일임
	ObjectRespawner * pRespawner = new ObjectRespawner( pItem );
	ArcadiaServer::Instance().SetObjectPriority( pRespawner, ArSchedulerObject::UPDATE_PRIORITY_HIGHEST );

	return 0;
}

int SCRIPT_GetItemName( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_GetItemName() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem ) lua_pushstring_utf8( L, pItem->GetNameInGame().c_str() );
	else lua_pushstring( L, "" );
	return 1;
}

int SCRIPT_GetItemCode( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_GetItemCode() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem ) lua_pushnumber( L, pItem->GetItemCode() );
	else lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_ItemName( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_ItemName() : invalid argument" );
		return 1;
	}

	int item_code = (int)lua_tonumber( L, 1 );

	lua_pushstring_utf8( L, GameContent::GetString( StructItem::GetItemBase( item_code ).nNameId ) );
	return 1;
}

int SCRIPT_GetWearItemHandle( struct lua_State *L )	// 현재 장착한 아이템 handle 을 얻는다
{
	int nArgCnt = lua_gettop(L);

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer || nArgCnt < 1 )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 인덱스 검사
	int wear_index = lua_tonumber( L, 1 );
	if( wear_index >= ItemBase::MAX_SPARE_ITEM_WEAR )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructItem *pItem = pPlayer->GetWearedItemByAbsolutePos( static_cast< ItemBase::ItemWearType >( wear_index ) );
	if( !pItem )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, pItem->GetHandle() );
	return 1;
}

int SCRIPT_InsertItem( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);

	if( nArgCnt < 2 )
	{
		LUA()->Log( "SCRIPT_InsertItem() : invalid argument" );
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 이 세 개의 변수는 AllocItem에서 -1로 들어가야 DB에 정의된 기본 값을 사용하도록 되어 있음(구리지만... ㅠㅜ;)
	int nEnhance = -1;
	int nLevel = -1;
	int nFlag = -1;

	if( nArgCnt >= 3 )
	{
		if( !lua_isnumber( L, 3 ) )
		{
			LUA()->Log( "SCRIPT_InsertItem() : invalid argument" );
			lua_pushnumber( L, 0 );
			return 1;
		}

		nEnhance = lua_tonumber( L, 3 );
	}
	if( nArgCnt >= 4 )
	{
		if( !lua_isnumber( L, 4 ) )
		{
			LUA()->Log( "SCRIPT_InsertItem() : invalid argument" );
			lua_pushnumber( L, 0 );
			return 1;
		}

		nLevel = lua_tonumber( L, 4 );
	}
	if( nArgCnt >= 5 )
	{
		if( !lua_isnumber( L, 5 ) )
		{
			LUA()->Log( "SCRIPT_InsertItem() : invalid argument" );
			lua_pushnumber( L, 0 );
			return 1;
		}

		nFlag = lua_tonumber( L, 5 );
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	const char * szType = "SCRIPT";

	int nCode = (int)lua_tonumber( L, 1 );
	__int64 nCount = (int)lua_tonumber( L, 2 );

	if( nCount < 1 || nLevel < -1 || nEnhance < -1 )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 아이템 할당
	while( nCode < 0 )
	{
		GameContent::SelectItemIDFromDropGroup( nCode, nCode, nCount );
	}

	if( !nCode )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructItem *pItem = StructItem::AllocItem( 0, nCode, nCount, ItemInstance::BY_SCRIPT, nLevel, nEnhance, nFlag );

	// 아이템 추가
	// _oprint( "F:" );
	// StructItem::AllocItem()으로 생성된 실제 갯수 저장
	nCount = pItem->GetCount();
	StructItem *pNewItem = pPlayer->PushItem( pItem, pItem->GetCount() );
	if( pNewItem == NULL )
	{
		// 돈
		LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), nCount, 0, pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szType, LOG::STR_NTS );
		lua_pushnumber( L, 0 );
	}
	else
	{
		LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewItem->GetCount(), 0, pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szType, LOG::STR_NTS );

		if( pNewItem != pItem )
		{
			// _oprint( "S:" );
			StructItem::PendFreeItem( pItem );
		}

		lua_pushnumber( L, pNewItem->GetHandle() );
	}

	return 1;
}

int SCRIPT_DeleteItem( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);

	if( nArgCnt < 2 )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 아이템 할당
	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	__int64 count = lua_tonumber( L, 2 );

	StructItem *pItem = StructItem::FindItem( item_handle );
	if( !pItem )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nItemEnhance				= pItem->GetItemEnhance();
	int nItemLevel					= pItem->GetItemLevel();
	ItemBase::ItemCode nCode		= pItem->GetItemCode();
	__int64 nItemCount				= pItem->GetCount();
	ItemUID nItemUID	= pItem->GetItemUID();

	if( pPlayer->EraseItem( pItem, count ) )
	{
		LOG::Log11N4S( LM_ITEM_DELETE, pPlayer->GetAccountID(), pPlayer->GetSID(), nItemEnhance * 100 + nItemLevel, nCode, nItemCount, count, pPlayer->GetGold().GetRawData(), 0, pPlayer->GetX(), pPlayer->GetY(), nItemUID, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "SUCS", LOG::STR_NTS, "SCRIPT", LOG::STR_NTS );
		lua_pushnumber( L, 1 );
	}
	else
	{
		LOG::Log11N4S( LM_ITEM_DELETE, pPlayer->GetAccountID(), pPlayer->GetSID(), nItemEnhance * 100 + nItemLevel, nCode, nItemCount, count, pPlayer->GetGold().GetRawData(), 0, pPlayer->GetX(), pPlayer->GetY(), nItemUID, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "FAIL", LOG::STR_NTS, "SCRIPT", LOG::STR_NTS );
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int	SCRIPT_PutoffItem( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_PutoffItem() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( pPlayer == NULL )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_PutoffItem() : invalid argument" );
		return 1;
	}

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem != NULL )
	{
		if( pPlayer->GetHandle() != pItem->GetOwnerHandle() )
		{
			lua_pushnumber( L, 0 );
			LUA()->Log( "SCRIPT_PutoffItem() : access denied" );
			return 1;
		}

		// Putoff
		unsigned short nRet = pPlayer->Putoff( pItem->GetWearInfo() );
		if( nRet == RESULT_SUCCESS )
		{
			pPlayer->CalculateStat();

			SendResult( pPlayer, TM_CS_PUTOFF_ITEM, nRet );

			TS_WEAR_INFO msg;
			GetWearMsg( pPlayer, msg );
			ArcadiaServer::Instance().Broadcast( pPlayer->GetRX(), pPlayer->GetRY(), pPlayer->GetLayer(), &msg );

			pItem->TurnOnUpdateFlag();
		}

		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_PutoffItem() : pItem is NULL" );
		return 1;
	}

	return 1;

}


int SCRIPT_IsErasableItem( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer || nArgCnt < 1 )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );
	if( !pItem || !pPlayer->IsErasable( pItem ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_InsertGold( struct lua_State *L )
{
	int nArgCnt = lua_gettop(L);

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	bool bAllowPartial = false;
	if( nArgCnt >= 2 )
	{
		if( lua_isboolean( L, 2 ) )
		{
			bAllowPartial = lua_toboolean( L, 2 );
			nArgCnt = 2;
		}
		else
		{
			nArgCnt = 1;
		}
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	// 부분 지급 허용 시 최대 보유량과 추가 보유 가능량 체크 처리
	const StructGold nCurrentGold = pPlayer->GetGold();
	StructGold nAddGold = (__int64)lua_tonumber( L, 1 );
	if( bAllowPartial )
	{
		// 돈을 지급하는 경우에는
		if( nAddGold > 0 )
		{
			// 이미 최대 보유량 이상을 가지고 있으면 지급량을 조절하지 않음으로써
			// ChangeGold의 실패를 유도하고 리턴값을 0을 만듦.
			if( nCurrentGold < GameRule::MAX_GOLD_FOR_INVENTORY )
				nAddGold = std::min( nAddGold, GameRule::MAX_GOLD_FOR_INVENTORY - nCurrentGold );
		}
		// 돈을 차감하는 경우에는
		else
		{
			// 땡전 한 푼 없는 경우에는 차감량을 조절하지 않음으로써
			// ChangeGold의 실패를 유도하고 리턴값을 0을 만듦.
			if( nCurrentGold > 0 )
				nAddGold = std::max( nAddGold, nCurrentGold * -1 );
		}
	}

	// 돈 추가
	if( pPlayer->ChangeGold( nCurrentGold + nAddGold ) == RESULT_SUCCESS )
	{
		LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 0, 0, nAddGold.GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "SUCS", 0, "SCRIPT", LOG::STR_NTS );
		lua_pushnumber( L, 1 );
	}
	else
	{
		LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 0, 0, nAddGold.GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "FAIL", 0, "SCRIPT", LOG::STR_NTS );
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_GetItemLevel( struct lua_State *L )	// handle 에서 아이템 레벨을 얻는다
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_GetItemLevel() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem ) lua_pushnumber( L, pItem->GetItemLevel() );
	else lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_GetItemEnhance( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_GetItemEnhance() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem ) lua_pushnumber( L, pItem->GetItemEnhance() );
	else lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_GetItemGrade( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_GetItemGrade() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem ) lua_pushnumber( L, pItem->GetItemGrade() );
	else lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_GetItemRank( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_GetItemRank() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem ) lua_pushnumber( L, pItem->GetItemRank() );
	else lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_GetItemPrice( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_GetItemPrice() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem ) lua_pushnumber( L, pItem->GetItemBase().nPrice.GetRawData() );
	else lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_RefreshEventDropInfo( struct lua_State *L )
{
	StructEventItemManager::GetInstance().LoadEventItemDropInfo();

	return 0;
}

int SCRIPT_SetItemLevel( struct lua_State *L )	// handle 에서 아이템 레벨을 얻는다
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_SetItemLevel() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemLevel() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem )
	{
		if( ( pPlayer->GetHandle() != pItem->GetOwnerHandle() || pItem->GetWearInfo() == -1 ) && pPlayer->GetPermission() == 0 )
		{
			lua_pushnumber( L, 0 );
			//LUA()->Log( "SCRIPT_SetItemLevel() : access denied" );
			return 1;
		}

		int nPrevLevel = pItem->GetItemLevel();
		pItem->SetItemLevel( lua_tonumber( L, 2 ) );

		lua_pushnumber( L, pItem->GetItemLevel() );

		StructPlayer::iterator it = StructPlayer::get( pItem->GetOwnerHandle() );
		if( *it )
		{
			LOG::Log11N4S( LM_ITEM_SMITH_UPGRADE, (*it)->GetAccountID(), (*it)->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nPrevLevel, pItem->GetItemLevel(), pItem->GetItemEnhance(), pItem->GetItemEnhance(), 0, 0, pItem->GetItemUID(), (*it)->GetAccountName(), LOG::STR_NTS, (*it)->GetName(), LOG::STR_NTS, "", 0, "", 0 );

			(*it)->UpdateQuestStatusByItemUpgrade();
			SendItemMessage( *it, pItem );

			if( pItem->GetWearInfo() != -1 )
			{
				if( pItem->GetOwnSummonHandle() )
				{
					StructCreature::iterator itOwner = StructCreature::get( pItem->GetOwnSummonHandle() );

					if( *itOwner )
					{
						(*itOwner)->CalculateStat();
					}
				}
				else
				{
					(*it)->CalculateStat();
				}
			}
		}

	}
	else
	{
		lua_pushnumber( L, 0 );
	}
	return 1;
}

int SCRIPT_SetItemEnhance( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || lua_isnumber(L, 2) < 0 )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemEnhance() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	int enhance = lua_tonumber( L, 2 );
	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemEnhance() : invalid argument" );
		return 1;
	}

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem )
	{
		bool bIsNeedToCalculate = false;
		if( pPlayer->GetHandle() != pItem->GetOwnerHandle() && pPlayer->GetPermission() == 0 )
		{
			lua_pushnumber( L, 0 );
			LUA()->Log( "SCRIPT_SetItemEnhance() : access denied" );
			return 1;
		}

		int nPrevEnhance = pItem->GetItemEnhance();

		if( pItem->IsSummonCard() )
		{
			if( enhance < 0 || enhance > GameRule::MAX_SUMMON_ENHANCE )
			{
				lua_pushnumber( L, 0 );
				LUA()->Log( "SCRIPT_SetItemEnhance() : invalid argument" );
				return 1;
			}

			pItem->SetItemEnhance( enhance );
			pItem->SetCurrentEtherealDurability( GameContent::GetCreatureEnhanceInfo( enhance )->card_durability );

			StructSummon *pSummon = pItem->GetSummonStruct();
			if( pSummon )
			{
				if( nPrevEnhance > lua_tonumber( L, 2 ) )
				{
					pSummon->ResetSkill( StructCreature::SRM_SUMMON_ENHANCE );
					// 플레이어 업데이트 여부
					for( int nBindIndex = 0 ; nBindIndex < 6 ; ++nBindIndex )
					{
						if( pPlayer->GetBeltSlotCardAt( nBindIndex ) == pItem )
						{
							bIsNeedToCalculate = true;
							break;
						}
					}
				}
				else
				{
					pSummon->SetJP( pSummon->GetJobPoint() + GameContent::GetCreatureEnhanceInfo( enhance )->jp_addition - GameContent::GetCreatureEnhanceInfo( nPrevEnhance )->jp_addition );
					pSummon->CalculateStat();
				}

				pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );
			}
		}
		else
		{
			pItem->SetItemEnhance( enhance );
		}

		lua_pushnumber( L, pItem->GetItemEnhance() );

		StructPlayer::iterator it = StructPlayer::get( pItem->GetOwnerHandle() );
		if( *it )
		{
			LOG::Log11N4S( LM_ITEM_SMITH_UPGRADE, (*it)->GetAccountID(), (*it)->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetItemLevel(), pItem->GetItemLevel(), nPrevEnhance, pItem->GetItemEnhance(), 0, 0, pItem->GetItemUID(), (*it)->GetAccountName(), LOG::STR_NTS, (*it)->GetName(), LOG::STR_NTS, "", 0, "", 0 );

			(*it)->UpdateQuestStatusByItemUpgrade();
			pItem = (*it)->onAfterChangeItemProperty( pItem );

			if( bIsNeedToCalculate )
				(*it)->CalculateStat();

			if( pItem->GetWearInfo() != ItemBase::WEAR_NONE )
			{
				if( pItem->GetOwnSummonHandle() )
				{
					StructCreature::iterator itOwner = StructCreature::get( pItem->GetOwnSummonHandle() );

					if( *itOwner )
					{
						(*itOwner)->CalculateStat();
					}
				}
				else
				{
					(*it)->CalculateStat();
				}
			}
		}
	}
	else
	{
		lua_pushnumber( L, 0 );
	}
	return 1;
}


int SCRIPT_SetItemEnhanceFail( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemEnhanceFail() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( pPlayer == NULL )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemEnhanceFail() : invalid argument" );
		return 1;
	}

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem != NULL )
	{
		if( pPlayer->GetHandle() != pItem->GetOwnerHandle() )
		{
			lua_pushnumber( L, 0 );
			LUA()->Log( "SCRIPT_SetItemEnhanceFail() : access denied" );
			return 1;
		}

		if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) == true ) 
		{
			lua_pushnumber( L, 0 );
			LUA()->Log( "SCRIPT_SetItemEnhanceFail() : ItemInstance::ITEM_FLAG_FAILED is on" );
			return 1;
		}

		if( pItem->IsEquipment() == true )
		{
			int	nItemGroup = pItem->GetItemGroup();

			if( nItemGroup >= ItemBase::GROUP_WEAPON && nItemGroup <= ItemBase::GROUP_MANTLE )
			{
				// On ( ItemInstance::ITEM_FLAG_FAILED )
				MixManager::procEnhanceFail( pPlayer, pItem, EnhanceInfo::RESULT_FAIL );
				SendMixResult( pPlayer, NULL, 0 );

				// Putoff
				unsigned short nRet = pPlayer->Putoff( pItem->GetWearInfo() );

				if( nRet == RESULT_SUCCESS )
				{
					pPlayer->CalculateStat();

					SendResult( pPlayer, TM_CS_PUTOFF_ITEM, nRet );

					TS_WEAR_INFO msg;
					GetWearMsg( pPlayer, msg );
					ArcadiaServer::Instance().Broadcast( pPlayer->GetRX(), pPlayer->GetRY(), pPlayer->GetLayer(), &msg );
				}

				pItem->TurnOnUpdateFlag();
				
				lua_pushnumber( L, pItem->GetItemEnhance() );

				StructPlayer::iterator it = StructPlayer::get( pItem->GetOwnerHandle() );
				LOG::Log11N4S( LM_ITEM_SMITH_UPGRADE, (*it)->GetAccountID(), (*it)->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetItemLevel(), pItem->GetItemLevel(), pItem->GetItemEnhance(), -1, 0, 0, pItem->GetItemUID(), (*it)->GetAccountName(), LOG::STR_NTS, (*it)->GetName(), LOG::STR_NTS, "", 0, "", 0 );

			}
			else
			{
				lua_pushnumber( L, 0 );
				LUA()->Log( "SCRIPT_SetItemEnhanceFail() : nItemGroup >= GROUP_WEAPON && nItemGroup <= GROUP_MANTLE" );
				return 1;
			}
		}
		else
		{
			lua_pushnumber( L, 0 );
			LUA()->Log( "SCRIPT_SetItemEnhanceFail() : pItem is not equipment item" );
			return 1;
		}
	}
	else
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemEnhanceFail() : pItem is NULL" );
		return 1;
	}

	return 1;
}

int SCRIPT_GetItemNameId( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_GetItemNameId() : invalid argument" );
		return 1;
	}

	int item_code = (int)lua_tonumber( L, 1 );

	lua_pushnumber( L, StructItem::GetItemBase( item_code ).nNameId );
	return 1;

}

int SCRIPT_StopEventDrop( struct lua_State *L )
{
	StructEventItemManager::GetInstance().StopEventDrop();
	return 0;
}

int SCRIPT_StartEventDrop( struct lua_State *L )
{
	StructEventItemManager::GetInstance().StartEventDrop();
	return 0;
}

int SCRIPT_StartEventDungeonDroprate( struct lua_State *L )
{
	if( StructEventItemManager::GetInstance().IsStartingDungeonDropRateEvent() )
	{
		LUA()->Log( "Dungeon droprate event has already been started." );
		return 0;
	}

	StructEventItemManager::GetInstance().StartDungeonDropRateEvent();
	LUA()->Log( "Dungeon droprate event has successfully been started." );
	return 0;
}

int SCRIPT_StopEventDungeonDroprate( struct lua_State *L )
{
	if( !StructEventItemManager::GetInstance().IsStartingDungeonDropRateEvent() )
	{
		LUA()->Log( "Dungeon droprate event has not been started yet." );
		return 0;
	}

	StructEventItemManager::GetInstance().StopDungeonDropRateEvent();
	LUA()->Log( "Dungeon droprate event has successfully been stopped." );
	return 0;
}

int SCRIPT_RefreshEventDungeonDroprate( struct lua_State *L )
{
	StructEventItemManager::GetInstance().LoadEventDungeonDroprateInfo();
	LUA()->Log( "Dungeon droprate event has successfully been refreshed." );
	return 0;
}

int SCRIPT_RefreshEventSupplyInfo( struct lua_State *L )
{
	StructEventItemManager::GetInstance().LoadEventItemSupplyInfo();
	return 0;
}

int SCRIPT_StartEventSupply( struct lua_State *L )
{
	StructEventItemManager::GetInstance().StartEventSupply();
	return 0;
}

int SCRIPT_StopEventSupply( struct lua_State *L )
{
	StructEventItemManager::GetInstance().StopEventSupply();
	return 0;
}

int SCRIPT_EventSupply( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1)  )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_EventSupply() : invalid argument" );
		return 1;
	}

	ItemBase::ItemCode nItemCode = static_cast< ItemBase::ItemCode >( lua_tonumber( L, 1 ) );

	// 아이템 수량 처리
	int nMinCount = 1;
	if( n >= 2 )
		nMinCount = (int)lua_tonumber( L, 2 );
	int nMaxCount = nMinCount;
	if( n >= 3 )
		nMaxCount = (int)lua_tonumber( L, 3 );

	// 아이템 플래그 파라미터 처리
	int nFlag = ItemInstance::ITEM_FLAG_NONE;
	if( n >= 4 )
		nFlag = (int)lua_tonumber( L, 4 );

	StructEventItemManager::GetInstance().SetActiveSupply( nItemCode, nMinCount, nMaxCount, nFlag );

	return 0;
}

int SCRIPT_SupplyEventItem( struct lua_State *L )
{
	StructPlayer::iterator pit = getPlayer( L, 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SupplyEventItem() : invalid argument" );
		return 1;
	}

	EVENT_ITEM_INFO EventSupply = StructEventItemManager::GetInstance().GetActiveSupply( time( NULL ) );

	if( EventSupply.code && GameContent::IsValidItemCode( EventSupply.code ) )
	{
		StructItem *pItem = StructItem::AllocItem( 0, EventSupply.code, XRandom( EventSupply.min_count, EventSupply.max_count ), ItemInstance::BY_SCRIPT, -1, -1, EventSupply.flag );

		pItem->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_EVENT );

		// DB에 아이템 추가는 StructPlayer::onAdd 에서 이루어짐
		StructItem * pNewItem = pPlayer->PushItem( pItem, pItem->GetCount() );
		if( pNewItem )
		{
			LOG::Log11N4S( LM_ITEM_TAKE, 
				pPlayer->GetAccountID(), pPlayer->GetSID(), 
				pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), 
				pItem->GetItemCode(), pItem->GetCount(), 
				pNewItem->GetCount(), 0, 
				0, pPlayer->GetX(), 
				pPlayer->GetY(), pItem->GetItemUID(), 
				pPlayer->GetAccountName(), LOG::STR_NTS, 
				pPlayer->GetName(), LOG::STR_NTS, 
				"", 0, 
				"EVENT_ITEM", LOG::STR_NTS );
		}

		int nFlag = 0;
		pItem->GetInstanceFlag().CopyTo( &nFlag, sizeof( nFlag ) );
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "Event", "%20s\t%20u\t%20d\t%20I64d\t%20d\t%20u", pPlayer->GetName(), pPlayer->GetSID(), pItem->GetItemCode(), pItem->GetItemUID(), pItem->GetCount(), nFlag );
		if( GameRule::bBroadcastEventItemPickup )
		{
			PrintfGlobalChatMessage( CHAT_NOTICE, "@NOTICE", "@955\v#@picker_name@#\v%s\v#@item_name@#\v@%d", pPlayer->GetName(), pItem->GetItemBase().nNameId );
		}
	}

	return 0;
}

int SCRIPT_FindItem( struct lua_State * L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_FindItem() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_FindItem() : invalid argument" );
		return 1;
	}

	int item_code = (int)lua_tonumber( L, 1 );

	StructItem * pItem = pPlayer->FindItem( (ItemBase::ItemCode) item_code );

	if( pItem )
	{
		lua_pushnumber( L, pItem->GetCount() );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_GetItemHandle( struct lua_State * L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemHandle() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemHandle() : invalid argument" );
		return 1;
	}

	int item_code = (int)lua_tonumber( L, 1 );

	StructItem * pItem = pPlayer->FindItem( (ItemBase::ItemCode) item_code );

	if( pItem )
	{
		lua_pushnumber( L, pItem->GetHandle() );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_GetItemHandleList( struct lua_State * L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemHandleList() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemHandleList() : invalid argument" );
		return 1;
	}

	int item_code = (int)lua_tonumber( L, 1 );

	std::vector< StructItem * > vList;
	std::vector< StructItem * >::const_iterator it;
	pPlayer->FindItem( (ItemBase::ItemCode) item_code, vList );

	int count = 0;
	lua_newtable( L );

	for( it = vList.begin(); it != vList.end(); ++it )
	{
		lua_pushnumber( L, ++count );
		lua_pushnumber( L, (*it)->GetHandle() );
		lua_rawset( L, -3 );
	}

	lua_pushstring( L, "n" );
	lua_pushnumber( L, count );
	lua_rawset( L, -3 );

	return 1;
}

int SCRIPT_GetItemEtherealDurability( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetItemEtherealDurability() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetItemEtherealDurability() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( !pItem || ( pItem->GetOwnerHandle() != pPlayer->GetHandle() && pPlayer->GetPermission() == 0 ) )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetItemEtherealDurability() : invalid argument" );
		return 1;
	}

	lua_pushnumber( L, pItem->GetCurrentEtherealDurability() );
	return 1;
}

int SCRIPT_SetItemEtherealDurability( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		LUA()->Log( "SCRIPT_SetItemEtherealDurability() : invalid argument" );
		return 0;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_SetItemEtherealDurability() : invalid argument" );
		return 0;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( !pItem || ( pItem->GetOwnerHandle() != pPlayer->GetHandle() && pPlayer->GetPermission() == 0 ) )
	{
		LUA()->Log( "SCRIPT_SetItemEtherealDurability() : invalid argument" );
		return 0;
	}

	int nEtherealDurability = lua_tonumber( L, 2 );
	if( nEtherealDurability < 0 || nEtherealDurability > GameRule::MAX_ETHEREAL_DURABILITY )
	{
		LUA()->Log( "SCRIPT_SetItemEtherealDurability() : invalid argument" );
		return 0;
	}

	pItem->SetCurrentEtherealDurability( nEtherealDurability );

	// 아이템 주인에게 아이템 메시지 방송
	if( pItem->GetOwnerHandle() == pPlayer->GetHandle() )
	{
		SendItemMessage( pPlayer, pItem );
	}
	else
	{
		StructPlayer::iterator itOwner = StructPlayer::get( pItem->GetOwnerHandle() );
		if( (*itOwner) )
			SendItemMessage( (*itOwner), pItem );
	}

	return 0;
}

int SCRIPT_GetMaxItemEtherealDurability( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetMaxItemEtherealDurability() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetMaxItemEtherealDurability() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( !pItem || ( pItem->GetOwnerHandle() != pPlayer->GetHandle() && pPlayer->GetPermission() == 0 ) )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetMaxItemEtherealDurability() : invalid argument" );
		return 1;
	}

	lua_pushnumber( L, pItem->GetMaxEtherealDurability() );
	return 1;
}

int SCRIPT_GetItemSoulStoneEndurance( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetItemSoulStoneEndurance() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetItemSoulStoneEndurance() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( !pItem || ( pItem->GetOwnerHandle() != pPlayer->GetHandle() && pPlayer->GetPermission() == 0 ) || pItem->GetUsingSocketCount() == 0 )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetItemSoulStoneEndurance() : invalid argument" );
		return 1;
	}

	lua_pushnumber( L, pItem->GetCurrentEndurance() );
	return 1;
}

int SCRIPT_GetMaxItemSoulStoneEndurance( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetItemSoulStoneEndurance() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetItemSoulStoneEndurance() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( !pItem || ( pItem->GetOwnerHandle() != pPlayer->GetHandle() && pPlayer->GetPermission() == 0 ) || pItem->GetUsingSocketCount() == 0 )
	{
		lua_pushnumber( L, -1 );
		LUA()->Log( "SCRIPT_GetItemSoulStoneEndurance() : invalid argument" );
		return 1;
	}

	lua_pushnumber( L, pItem->GetMaxEndurance() );
	return 1;
}

int SCRIPT_GetItemEndurance( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemEndurance() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemEndurance() : invalid argument" );
		return 1;
	}

	int item_code = (int)lua_tonumber( L, 1 );

	StructItem * pItem = pPlayer->FindItem( (ItemBase::ItemCode) item_code );

	if( pItem )
	{
		lua_pushnumber( L, pItem->GetCurrentEndurance() );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_SetItemEndurance( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_SetItemEndurance() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemEndurance() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem )
	{
		pItem->SetCurrentEndurance( (int)lua_tonumber( L, 2 ) );

		SendItemMessage( pPlayer, pItem );
		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_GetItemSocketCode( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemSocketCode() : invalid argument" );
		return 1;
	}

	int socket_index = lua_tonumber( L, 2 );

	if( socket_index < 0 || socket_index >= ItemBase::MAX_SOCKET_NUMBER )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemSocketCode() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemSocketCode() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( !pItem || ( pItem->GetOwnerHandle() != pPlayer->GetHandle() && pPlayer->GetPermission() == 0 ) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemSocketCode() : invalid argument" );
		return 1;
	}

	lua_pushnumber( L, pItem->GetSocketCode( socket_index ) );
	return 1;
}

int SCRIPT_SetItemSocketCode( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 3 || !lua_isnumber(L, 1) || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_SetItemSocketCode() : invalid argument" );
		return 1;
	}

	int socket_index = lua_tonumber( L, 2 );

	if( socket_index < 0 || socket_index >= ItemBase::MAX_SOCKET_NUMBER )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemSocketCode() : invalid argument" );
		return 1;
	}

	int socket_code = (int)lua_tonumber( L, 3 );
	ItemBase SocketBase = StructItem::GetItemBase( socket_code );
	if( SocketBase.nCode == 0 || SocketBase.nType != ItemBase::TYPE_SOULSTONE )
	{
		lua_pushnumber( L, 0 );
		//LUA()->Log( "SCRIPT_SetItemSocketCode() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 4 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemSocketCode() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem )
	{
		pItem->SetSocketCode( socket_index, socket_code );
		pItem->SetCurrentEndurance( pItem->GetCurrentEndurance() + SocketBase.nEndurance );

		SendItemMessage( pPlayer, pItem );
		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_SetItemElement( struct lua_State *L )
{
	// SCRIPT_SetItemElement
	// 1: 아이템핸들 2: 속성이펙트스톤 아이템코드

	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_SetItemElement() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemElement() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	int element_code = lua_tonumber( L, 2 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( !pItem )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemElement() : invalid argument" );
		return 1;
	}

	// 두 번째 인자는 속성이펙트스톤이어야 한다.
	const ItemBaseServer * pBase = &StructItem::GetItemBase( element_code );
	if( !pBase->nCode || pBase->nClass != ItemBase::CLASS_ELEMENT_EFFECT )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemElement() : invalid argument" );
		return 1;
	}

	unsigned char cElementalType = pBase->fOptVar1[ 0 ].GetAsInt64();
	if( !cElementalType ) // cElementalType == 0 -> 무기속성이펙트제거
	{
		if( pItem->GetElementalEffectType() && pItem->GetElementalEffectExpireTime() )
		{
			pPlayer->RemoveFromElementalEffectedItemList( pItem );
		}
		pItem->ClearElementalEffect();
	}
	else
	{
		time_t tExpire = pBase->fOptVar2[ 0 ] ? time( NULL ) + pBase->fOptVar2[ 0 ] : 0;

		if( pItem->SetElementalEffect( cElementalType, tExpire ) != RESULT_SUCCESS )
		{
			lua_pushnumber( L, 0 );
			LUA()->Log( "SCRIPT_SetItemElement() : fail to insert elemental effect" );
			return 1;
		}

		// 만약 영구 속성이 부여된 아이템이라면 만료 처리를 하지 않기 위해 Elemental Effected Item List에 추가해서는 안 될 뿐더러 행여나 존재할 경우에는 제거를 해줘야 한다.
		if( tExpire )
			pPlayer->AddToElementalEffectedItemList( pItem );
		else
			pPlayer->RemoveFromElementalEffectedItemList( pItem );
	}

	pItem->TurnOnUpdateFlag();
	SendItemMessage( pPlayer, pItem );

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_SetItemElementParameter( struct lua_State *L )
{
	// SCRIPT_SetItemElementParameter
	// 1: 아이템핸들 2: 타입(공격력:1/마력:2) 3: 수치

	int n = lua_gettop(L);			// number of arguments

	if( n < 3 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		lua_pushstring( L, "" );
		LUA()->Log( "SCRIPT_SetItemElementParameter() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 4 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemElementParameter() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	StructItem *pItem = StructItem::FindItem( item_handle );

	if( !pItem )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemElementParameter() : invalid argument" );
		return 1;
	}

	// type이 1이면 공격력, 2이면 마력 증가
	int type = lua_tonumber( L, 2 );
	if( type != 1 && type != 2 )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemElementParameter() : invalid argument" );
		return 1;
	}

	int point = lua_tonumber( L, 3 );
	if( point < 0 )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemElementParameter() : invalid argument" );
		return 1;
	}

	if( type == 1 )
	{
		pItem->SetElementalEffectAttackPoint( point );
	}
	else
	{
		pItem->SetElementalEffectMagicPoint( point );
	}

	pItem->TurnOnUpdateFlag();
	SendItemMessage( pPlayer, pItem );

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_GetItemAppearanceCode( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemAppearanceCode() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_GetItemAppearanceCode() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem )
	{
		lua_pushnumber( L, pItem->GetAppearanceCode() );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}

int SCRIPT_SetItemAppearanceCode( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemAppearanceCode() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_SetItemAppearanceCode() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );

	StructItem *pItem = StructItem::FindItem( item_handle );

	if( pItem && pItem->SetAppearanceCode( (int)lua_tonumber( L, 2 ) ) )
	{
		SendItemMessage( pPlayer, pItem );
		pItem->TurnOnUpdateFlag();

		// 방송
		TS_WEAR_INFO msg;
		GetWearMsg( pPlayer, msg );
		ArcadiaServer::Instance().Broadcast( pPlayer->GetRX(), pPlayer->GetRY(), pPlayer->GetLayer(), &msg );	

		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}


int SCRIPT_InsertAwakenOption( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	// ItemHandle, Min, Max, Player
	if( n < 3 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_InsertAwakenOption() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 4 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_InsertAwakenOption() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	StructItem *pItem = StructItem::FindItem( item_handle );
	if( pItem )
	{
		if( pItem->IsAwaken() )
		{
			lua_pushnumber( L, 0 );
			return 1;
		}

		int arrRate[ 40 ] = {0, };
		RandomManager::GetAwakenProbabilitiesInfo( arrRate, _countof( arrRate ) );

		int nMinAwakenCount = (int)lua_tonumber( L, 2 );
		int nMaxAwakenCount = (int)lua_tonumber( L, 3 );
		int nAwakenCount = XRandom( nMinAwakenCount, nMaxAwakenCount );

		if( nAwakenCount > ItemInstance::MAX_AWAKEN_NUMBER )
			nAwakenCount = ItemInstance::MAX_AWAKEN_NUMBER;

		ItemInstance::RANDOM_OPTION kAwakenOption;
		kAwakenOption.nRandomType = ItemInstance::AWAKEN;

		int nType = 0;
		int nBitSet = 0;
		int nAwakenData = 0;
		for( int nOptionCnt = 0 ; nOptionCnt < nAwakenCount ; )
		{
			int nKey = XRandom( 1, 100000 );
			bool bCheckResult = true;

			for( int nIndex = 0 ; nIndex < 40 ; ++nIndex )
			{
				if( nKey > arrRate[ nIndex ] )	continue;

				int nMaxTypeCount = RandomManager::GetMaxAwakenCount();
				int nOptionType = XRandom( 1, nMaxTypeCount );
				nAwakenData = RandomManager::GetAwakenInfoValue( nIndex, nOptionType, nBitSet, nType );
				if( !nAwakenData )	return false;

				if( !RandomManager::CheckMaxAwakenOption( &kAwakenOption, nBitSet, nType, nAwakenData ) )
				{
					bCheckResult = false;
				}
				break;
			}

			if( bCheckResult )
			{
				kAwakenOption.OptionInfo[ nOptionCnt ].nType = nType;
				kAwakenOption.OptionInfo[ nOptionCnt ].fValue1 = nBitSet;
				kAwakenOption.OptionInfo[ nOptionCnt ].fValue2 = nAwakenData;

				++nOptionCnt;
			}
		}

		kAwakenOption.nSID = RandomManager::AllocRandomSID();
		if( !pItem->SetAwakenOption( kAwakenOption ) )	return 1;

		pItem->DBQuery( new DB_UpdateItemAwakenSID( pItem ) );
		pItem->DBQuery( new DB_SetRandomOption( pPlayer, pItem, kAwakenOption ) );

		SendItemMessage( pPlayer, pItem );

		AR_HANDLE tmp = pItem->GetHandle();
		SendMixResult( pPlayer, &tmp, 1, 1 );

		// Awaken Result
		char szAwakenValue[128] = { 0, };
		char szAwakenData[128] = { 0, };
		s_itoa( pItem->GetAwakenOptionValue1( 4 ), szAwakenValue, _countof( szAwakenValue ), 10 );
		s_itoa( pItem->GetAwakenOptionValue2( 4 ), szAwakenData, _countof( szAwakenData ), 10 );
		LOG::Log11N4S( LM_INSERT_AWAKEN_RESULT_BY_SCRIPT, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetAwakenOptionValue1(0), pItem->GetAwakenOptionValue2(0), pItem->GetAwakenOptionValue1(1), pItem->GetAwakenOptionValue2(1), pItem->GetAwakenOptionValue1(2), pItem->GetAwakenOptionValue2(2), pItem->GetAwakenOptionValue1(3), pItem->GetAwakenOptionValue2(3), pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, szAwakenValue, LOG::STR_NTS, szAwakenData, LOG::STR_NTS );

		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}


int SCRIPT_IdentifyItemForRandomOption( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	// 1:ItemHandle, 2:PresetID
	if( n < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_IdentifyItemForRandomOption() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 3 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_IdentifyItemForRandomOption() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	int nID = (int)lua_tonumber( L, 2 );

	StructItem *pItem = StructItem::FindItem( item_handle );
	if( pItem )
	{
		if( !pPlayer->IsRandomizable( pItem ) )
		{		
			lua_pushnumber( L, 0 );
			return 1;
		}

		ItemInstance::RANDOM_OPTION kOption;
		kOption.nSID = RandomManager::AllocRandomSID();
		kOption.nRandomType = ItemInstance::IDENTIFIED;
		if( !RandomManager::PickRandomOptionByPreset( &kOption, nID ) )
		{
			lua_pushnumber( L, 0 );
			return 1;
		}

		pItem->SetIdentifiedOption( kOption );
		int nLogCnt = 0;
		for( ; nLogCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++nLogCnt )
		{
			if( !kOption.OptionInfo[ nLogCnt ].nType )
			{
				break;
			}
		}

		// 랜덤 옵션 두개마다 로그 하나씩 기록. (실제 옵션 값은 소수점도 기록하기 위해 한자리 올려서 기록한다. 예: 15.6->156 )
		for( int i = 0 ; i < nLogCnt ; i+=2 )
		{
			LOG::Log11N4S( LM_INSERT_RANDOM_OPTION, pPlayer->GetAccountID(), pPlayer->GetSID(), kOption.OptionInfo[ i ].nType, kOption.OptionInfo[ i ].fValue1, kOption.OptionInfo[ i ].fValue2.get()/1000, kOption.OptionInfo[ i + 1 ].nType, kOption.OptionInfo[ i + 1 ].fValue1, kOption.OptionInfo[ i + 1].fValue2.get()/1000, 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "ByScript", LOG::STR_NTS );
		}

		// DB업뎃
		pItem->DBQuery( new DB_UpdateItemRandomOptionSID( pItem ) );
		pItem->DBQuery( new DB_SetRandomOption( pPlayer, pItem, kOption ) );

		SendItemMessage( pPlayer, pItem );
		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}


int SCRIPT_DeleteAwakenOption( struct lua_State *L )
{
	int n = lua_gettop(L);			// number of arguments

	// ItemHandle, Player
	if( n < 1 || !lua_isnumber(L, 1) )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_DeleteAwakenOption() : invalid argument" );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, 2 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		LUA()->Log( "SCRIPT_DeleteAwakenOption() : invalid argument" );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	StructItem *pItem = StructItem::FindItem( item_handle );
	if( pItem )
	{
		if( !pItem->IsAwaken() )
		{
			lua_pushnumber( L, 0 );
			return 1;
		}

		int nAwakenSID = pItem->GetAwakenSID();
		pItem->SetAwakenSID( 0 );

		pItem->DBQuery( new DB_UpdateItemAwakenSID( pItem ) );
		pItem->DBQuery( new DB_DeleteRandomOption( pItem, nAwakenSID ) );

		SendItemMessage( pPlayer, pItem );

		AR_HANDLE tmp = pItem->GetHandle();
		SendMixResult( pPlayer, &tmp, 1, 1 );

		// Awaken Result
		char szAwakenValue[128] = { 0, };
		char szAwakenData[128] = { 0, };
		s_itoa( pItem->GetAwakenOptionValue1( 4 ), szAwakenValue, _countof( szAwakenValue ), 10 );
		s_itoa( pItem->GetAwakenOptionValue2( 4 ), szAwakenData, _countof( szAwakenData ), 10 );
		LOG::Log11N4S( LM_DELETE_AWAKEN_RESULT_BY_SCRIPT, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetAwakenOptionValue1(0), pItem->GetAwakenOptionValue2(0), pItem->GetAwakenOptionValue1(1), pItem->GetAwakenOptionValue2(1), pItem->GetAwakenOptionValue1(2), pItem->GetAwakenOptionValue2(2), pItem->GetAwakenOptionValue1(3), pItem->GetAwakenOptionValue2(3), pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, szAwakenValue, LOG::STR_NTS, szAwakenData, LOG::STR_NTS );

		lua_pushnumber( L, 1 );
	}
	else
	{
		lua_pushnumber( L, 0 );
	}

	return 1;
}



int SCRIPT_ClearInventory( struct lua_State *L )
{
	StructPlayer::iterator pit = StructPlayer::iterator( GetCurrentThreadPlayer() );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		FILELOG( "SCRIPT_ClearInventory pPlayer == NULL" );
		_cprint( "SCRIPT_ClearInventory pPlayer == NULL\n" );

		lua_pushnumber( L, 0 );
		return 1;
	}


	int nArgCnt = lua_gettop(L);

	if( nArgCnt != 0 )
	{
		FILELOG( "SCRIPT_ClearInventory ArgCnt != 0. PlayerID[%s]", pPlayer->GetName() );
		_cprint( "SCRIPT_ClearInventory ArgCnt != 0. PlayerID[%s]\n", pPlayer->GetName() );

		lua_pushnumber( L, 0 );
		return 1;
	}


	if( pPlayer->GetPermission() != GameRule::PERMISSION_FOR_GM )
	{
		FILELOG( "SCRIPT_ClearInventory pPlayer[%s][%d] != PERMISSION_FOR_GM", pPlayer->GetName(), pPlayer->GetPermission() );
		_cprint( "SCRIPT_ClearInventory pPlayer[%s][%d] != PERMISSION_FOR_GM\n", pPlayer->GetName(), pPlayer->GetPermission() );

		lua_pushnumber( L, 0 );
		return 1;
	}


	pPlayer->ClearInventory();
	FILELOG( "Execute SCRIPT_ClearInventory pPlayer[%s][%d]", pPlayer->GetName(), pPlayer->GetPermission() );

	lua_pushnumber( L, 1 );
	return 1;
}

int SCRIPT_InsertSummonBySummonID( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_InsertSummonBySummonID() : invalid argument" );
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nSummonCode = (int)lua_tonumber( L, 1 );

	SummonBase* pBase = GameContent::GetSummonInfo( nSummonCode );
	if( pBase->uid != nSummonCode )
	{
		LUA()->Log( "SCRIPT_InsertSummonBySummonID() : invalid summon code" );
		lua_pushnumber( L, 0 );
		return 1;
	}
	int nItemCode = pBase->card_id;

	// SummonCode가 성장, 진화형으로 오면 기본형 카드로 뽑는다 (진화를 제대로 하려면 Alloc이 된 상태여야하기에.)
	if( pBase->evolve_type == SummonBase::EVOLVE_GROWTH )
	{
		nSummonCode = pBase->uid - 1;		
	}
	else if( pBase->evolve_type == SummonBase::EVOLVE_EVOLVE )
	{
		nSummonCode = pBase->uid - 2;
	}

	StructItem *pItem = StructItem::AllocItem( 0, nItemCode, 1, ItemInstance::BY_SCRIPT );
	pItem->SetSummonCode( nSummonCode );
	pItem->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_SUMMON );
	StructItem *pNewItem = pPlayer->PushItem( pItem, pItem->GetCount() );
	if( pNewItem != pItem )
	{
		LUA()->Log( "SCRIPT_InsertSummonBySummonID() : fail to push item" );
		StructItem::PendFreeItem( pItem );
	}

	if( pNewItem == NULL )
	{
		LUA()->Log( "SCRIPT_InsertSummonBySummonID() : invalid summon code new item." );
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_pushnumber( L, pNewItem->GetHandle() );
	return 1;
}

int SCRIPT_InsertSummonByMonsterID( struct lua_State *L )
{
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 1 )
	{
		LUA()->Log( "SCRIPT_InsertSummonByMonsterID() : invalid argument" );
		lua_pushnumber( L, 0 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nMonsterCode = (int)lua_tonumber( L, 1 );

	MonsterBase* pBase = GameContent::GetMonsterInfo( nMonsterCode );
	if( pBase->uid != nMonsterCode || ( !pBase->creature_taming_code_group && !pBase->taming_code ) )
	{
		LUA()->Log( "SCRIPT_InsertSummonByMonsterID() : invalid monster code" );
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nSummonCode = 0;
	if( pBase->creature_taming_code_group ) // 소울테이밍 카드로 넣는 크리쳐
	{
		nSummonCode = GameContent::GetMonsterCreatureTameCode( pBase->creature_taming_code_group );
	}
	else
	{
		nSummonCode = pBase->taming_code;
	}

	SummonBase* pSummonBase = GameContent::GetSummonInfo( nSummonCode );
	int nItemCode = pSummonBase->card_id;

	StructItem *pItem = StructItem::AllocItem( 0, nItemCode, 1, ItemInstance::BY_SCRIPT );
	pItem->SetSummonCode( nSummonCode );
	pItem->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_SUMMON );
	StructItem *pNewItem = pPlayer->PushItem( pItem, pItem->GetCount() );
	if( pNewItem != pItem )
	{
		LUA()->Log( "SCRIPT_InsertSummonByMonsterID() : fail to push item" );
		StructItem::PendFreeItem( pItem );
	}

	lua_pushnumber( L, pNewItem->GetHandle() );
	return 1;
}

int SCRIPT_ChangeItemCode( struct lua_State *L )
{
	// 1. 아이템 핸들 2. 바꿀 아이템 코드
	// PlayerLock 필요 (지역락)
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		LUA()->Log( "SCRIPT_ChangeItemCode() : invalid argument" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_ChangeItemCode() : invalid player" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	ItemBase::ItemCode code = (ItemBase::ItemCode)lua_tonumber( L, 2 );

	StructItem *pItem = StructItem::FindItem( item_handle );
	if( !pItem )
	{
		LUA()->Log( "SCRIPT_ChangeItemCode() : invalid handle" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	ItemBaseServer * base = &StructItem::GetItemBase( code );
	if( base->nCode == 0 )
	{
		LUA()->Log( "SCRIPT_ChangeItemCode() : invalid code" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	int nOriginalCode = pItem->GetItemCode();
	if( pItem->ChangeItemCode( code ) )
	{
		LOG::Log11N4S( LM_ITEM_CONVERT, pPlayer->GetAccountID(), 
			pPlayer->GetPlayerUID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), 
			nOriginalCode, code,
			pItem->GetCount(), 0,
			0, 0,
			0, pItem->GetItemUID(),
			"", 0, "", 0,
			"", 0, "ITEM_CONVERT_BY_SCRIPT", LOG::STR_NTS );
		pPlayer->onAfterChangeItemProperty( pItem );

		if( pItem->GetWearInfo() != ItemBase::WEAR_NONE )
		{
			pPlayer->CalculateStat();
		}

		lua_pushnumber( L, 1 );
		return 1;
	}

	lua_pushnumber( L, 0 );
	return 1;	
}

int SCRIPT_GetItemIdentifiedOption( struct lua_State *L )
{
	// 1. 아이템 핸들 2. 인덱스 (랜덤 옵션 위치 번호 ex; 1 - 첫 번째 랜덤 옵션 반환)
	// 반환은 루아 테이블로 한다. (테이블구성: opt1, opt2, val)
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 2 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) )
	{
		LUA()->Log( "SCRIPT_GetItemIdentifiedOption() : invalid argument" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_GetItemIdentifiedOption() : invalid player" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	int index = (ItemBase::ItemCode)lua_tonumber( L, 2 );

	if( index < 1 || index > ItemInstance::MAX_RANDOM_OPTION_NUMBER )
	{
		LUA()->Log( "SCRIPT_GetItemIdentifiedOption() : invalid index number" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	StructItem *pItem = StructItem::FindItem( item_handle );
	if( !pItem )
	{
		LUA()->Log( "SCRIPT_GetItemIdentifiedOption() : invalid handle" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	int nType = pItem->GetRandomOptionType( ItemInstance::IDENTIFIED, index-1 );
	// 해당 인덱스의 랜덤 옵션이 존재하면 넣기.
	if( nType )
	{
		lua_newtable( L );

		lua_pushnumber( L, 1 );
		lua_pushnumber( L, nType );
		lua_rawset( L, -3 );

		lua_pushnumber( L, 2 );
		lua_pushnumber( L, pItem->GetRandomOptionValue1( ItemInstance::IDENTIFIED, index-1 ) );
		lua_rawset( L, -3 );

		lua_pushnumber( L, 3 );
		lua_pushnumber( L, pItem->GetRandomOptionValue2( ItemInstance::IDENTIFIED, index-1 ) );
		lua_rawset( L, -3 );

		// 이거 안넣어주면 다운되나?
 		lua_pushstring( L, "n" );
 		lua_pushnumber( L, 4 );
 		lua_rawset( L, -3 );

		return 1;
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int SCRIPT_SetItemIdentifiedOption( struct lua_State *L )
{
	// 1. 아이템 핸들 2.index 3.opt1 4.opt2 5.val
	// 반환정보: 성공:1 실패:0, -1(오류에 의한 실패)
	// 이 스크립트는 SID가 이미 존재하는 랜덤 옵션 아이템에 대해서 추가적인 값을 세팅할때만 사용한다.
	int nArgCnt = lua_gettop( L );

	if( nArgCnt < 5 || !lua_isnumber( L, 1 ) || !lua_isnumber( L, 2 ) || !lua_isnumber( L, 3 ) || !lua_isnumber( L, 4 ) )
	{
		LUA()->Log( "SCRIPT_SetItemIdentifiedOption() : invalid argument" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	StructPlayer::iterator pit = getPlayer( L, nArgCnt + 1 );
	StructPlayer *pPlayer = *pit;

	if( !pPlayer )
	{
		LUA()->Log( "SCRIPT_SetItemIdentifiedOption() : invalid player" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	AR_HANDLE item_handle = (AR_HANDLE)lua_tonumber( L, 1 );
	int index = (ItemBase::ItemCode)lua_tonumber( L, 2 );
	int opt1 = lua_tonumber( L, 3 );
	double opt2 = lua_tonumber( L, 4 );
	double val = lua_tonumber( L, 5 );

	if( index < 1 || index > ItemInstance::MAX_RANDOM_OPTION_NUMBER )
	{
		LUA()->Log( "SCRIPT_SetItemIdentifiedOption() : invalid index number" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	StructItem *pItem = StructItem::FindItem( item_handle );
	if( !pItem )
	{
		LUA()->Log( "SCRIPT_SetItemIdentifiedOption() : invalid handle" );
		lua_pushnumber( L, -1 );
		return 1;
	}

	ItemInstance::RANDOM_OPTION *pOption = pItem->GetRandomOption( ItemInstance::IDENTIFIED );
	if( pItem->GetIdentifiedSID() && pOption )
	{
		// 로그남기기용 변수
		int nOriginalType = pOption->OptionInfo[index-1].nType;
		int dOriginalValue1 = pOption->OptionInfo[index-1].fValue1 * 10;
		int dOriginalValue2 = pOption->OptionInfo[index-1].fValue2 * 10;

		pOption->OptionInfo[index-1].nType = opt1;
		pOption->OptionInfo[index-1].fValue1 = opt2;
		pOption->OptionInfo[index-1].fValue2 = val;
		pItem->SetIdentifiedOption( *pOption );
		pItem->DBQuery( new DB_SetRandomOption( pPlayer, pItem, *pOption ) );
		pPlayer->onAfterChangeItemProperty( pItem );
		if( pItem->GetWearInfo() != ItemBase::WEAR_NONE )
			pPlayer->CalculateStat();

		LOG::Log11N4S( LM_UPDATE_RANDOM_OPTION, 
			pPlayer->GetAccountID(), pPlayer->GetSID(), 
			index, pItem->GetItemCode(),
			nOriginalType, dOriginalValue1, 
			dOriginalValue2, pOption->OptionInfo[index-1].nType,
			pOption->OptionInfo[index-1].fValue1.get()/1000, pOption->OptionInfo[index-1].fValue2.get()/1000,
			pItem->GetItemUID(), 
			pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, 
			"", 0, "ByScript", LOG::STR_NTS );

		lua_pushnumber( L, 1 );
		return 1;
	}

	lua_pushnumber( L, 0 );
	return 1;
}

int	SCRIPT_PickItemInDropGroup( struct lua_State *L )
{
	// Input: 1번째 - 드랍그룹 ID (드랍그룹 ID가 아니면 그냥 nCount = 1로 해당 코드 반환)
	// Output: LuaTable (1:코드, 2:개수)
	int nArgCnt = lua_gettop(L);

	if( nArgCnt < 1 || !lua_isnumber( L, 1 ) )
	{
		LUA()->Log( "SCRIPT_PickItemInDropGroup() : invalid argument" );
		lua_pushnumber( L, 0 );
		return 1;
	}

	int nCode = (int)lua_tonumber( L, 1 );
	__int64 nCount = 1;

	// 아이템 할당
	while( nCode < 0 )
	{
		GameContent::SelectItemIDFromDropGroup( nCode, nCode, nCount );
	}

	if( !nCode )
	{
		lua_pushnumber( L, 0 );
		return 1;
	}

	lua_newtable( L );

	lua_pushnumber( L, 1 );
	lua_pushnumber( L, nCode );
	lua_rawset( L, -3 );

	lua_pushnumber( L, 2 );
	lua_pushnumber( L, nCount );
	lua_rawset( L, -3 );

	lua_pushstring( L, "n" );
	lua_pushnumber( L, 3 );
	lua_rawset( L, -3 );

	return 1;
}