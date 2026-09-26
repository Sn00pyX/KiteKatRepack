
#include <network/XIOCPConnection.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XRandom.h>
#include <logging/FileLog.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/XQuadScanner.h>

#include "ErrorCode/ErrorCode.h"
#include "LogClient/LogClient.h"

#include "GameProc.h"
#include "StructItem.h"
#include "StructMonster.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructNPC.h"
#include "StructPlayer.h"
#include "StructFieldProp.h"
#include "StructSkill.h"
#include "GameContent.h"
#include "ItemCollector.h"
#include "DB_Commands.h"
#include "GameMessage.h"
#include "LuaVM.h"
#include "SendMessage.h"
#include "Extern.h"
#include "ChannelManager.h"
#include "DungeonManager.h"
#include "StructWorldLocation.h"


// 테이밍 세팅
bool SetTamer( struct StructMonster *pMonster, struct StructPlayer *pPlayer, struct StructSkill *pSkill )
{
	if( pPlayer->GetTamingTarget() ) return false;

	const ItemBase::ItemCode BASIC_CREATURE_CARD	= 0;
	const ItemBase::ItemCode RARE_CREATURE_CARD		= 0;
	const ItemBase::ItemCode COMMON_CREATURE_CARD	= 0;
	const ItemBase::ItemCode UNCOMMON_CREATURE_CARD	= 0;

	// HP 100%인지 체크
	if( pMonster->GetHP() != pMonster->GetMaxHP() ) return false;

	// 이미 테이밍 되어 있으면 KIN
	if( pMonster->GetTamer() )	return false;

	// 플레이어가 아니라면 KIN
	if( !pPlayer->IsPlayer() ) return false;

	// 이미 다른 몬스터를 테이밍 중일 경우 테이밍 불가
	AR_HANDLE hTamingTarget = pPlayer->GetTamingTarget();
	if( hTamingTarget ) return false;

	// 테이밍 안되는 녀석
	ItemBase::ItemCode hCardCode = pMonster->GetTameItemCode();
	if( !hCardCode ) return false;

	// 테이밍 카드를 가지고 있는지 조사
	StructItem *pItem = pPlayer->FindEmptySummonCard( hCardCode );

	if( !pItem ) return false;

	// 세팅
	pMonster->SetTamer( pPlayer->GetHandle(), pSkill );

	pPlayer->SetTamingTarget( pMonster->GetHandle() );
	pItem->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_TAMING );

	// 방송
	BroadcastTamingMessage( pPlayer, pMonster, TS_SC_TAMING_INFO::START );

	return true;
}

// 테이밍 풀림
void ClearTamer( struct StructMonster *pMonster, bool bBroadcastMsg )
{
	if( !pMonster->GetTamer() ) return;

	StructPlayer::iterator it = StructPlayer::get( pMonster->GetTamer() );

	if( (*it) )
	{
		if( bBroadcastMsg )
			BroadcastTamingMessage( (*it), pMonster, TS_SC_TAMING_INFO::CLEAR );

		(*it)->SetTamingTarget( 0 );

		// 테이밍 중이던 소환수 카드의 테이밍 상태 해제
		StructItem *pItem = NULL;
		while( pItem = (*it)->FindTamingSummonCard( pMonster->GetTameItemCode() ) )
		{
			pItem->SetInstanceFlagOff( ItemInstance::ITEM_FLAG_TAMING );
		}
	}
	pMonster->SetTamer( 0, NULL );
}

// 테이밍
bool ProcTame( StructMonster *pMonster )
{
	// 테이밍 된놈이 아님
	if( !pMonster->GetTamer() ) return false;

	StructPlayer::iterator it = StructPlayer::get( pMonster->GetTamer() );
	StructPlayer *pTamer = *it;	

	// 테이머가 존재하지 않음
	if( !pTamer || pTamer->IsDead() )
	{
		BroadcastTamingMessage( NULL, pMonster, TS_SC_TAMING_INFO::FAILED );
		return false;
	}

	// 너무 멀다면 테이밍 실패
	if( pMonster->GetPos().GetDistance( pTamer->GetPos() ) > RANGE_LIMIT ) 
	{
		ClearTamer( pMonster, false );
		BroadcastTamingMessage( pTamer, pMonster, TS_SC_TAMING_INFO::FAILED );
		return false;
	}

	// 테이밍 안되는 녀석
	ItemBase::ItemCode nTameItemCode = pMonster->GetTameItemCode();
	if( !nTameItemCode ) 
	{
		ClearTamer( pMonster, false );
		BroadcastTamingMessage( pTamer, pMonster, TS_SC_TAMING_INFO::FAILED );
		return false;
	}

	// 테이밍 카드를 가지고 있는지 조사
	StructItem *pItem = pTamer->FindTamingSummonCard( nTameItemCode );
	if( !pItem )
	{
		_cprint( "ProcTame: A summon card used for taming is lost. [%s:%s]\n", pTamer->GetAccountName(), pTamer->GetName() );
		FILELOG( "ProcTame: A summon card used for taming is lost. [%s:%s]", pTamer->GetAccountName(), pTamer->GetName() );

		ClearTamer( pMonster, false );
		BroadcastTamingMessage( pTamer, pMonster, TS_SC_TAMING_INFO::FAILED );
		return false;
	}

	const StructSkill *pSkill = pMonster->GetTamingSkill();

	if( !pSkill )
	{
		assert( 0 );
		ClearTamer( pMonster, false );
		BroadcastTamingMessage( pTamer, pMonster, TS_SC_TAMING_INFO::FAILED );
		return false;
	}

	bool bCardProtected = false;
	StructItem * pMirrorOfTamingCard = NULL;
	// 소모 순서 기준에 따라 순서대로 아이템 코드별 보유템 검색
	(	( pMirrorOfTamingCard = pTamer->FindItem( (ItemBase::ItemCode) ItemBase::ITEM_CODE_MIRROR_OF_TAMING_CARD_ON_TEST ) ) ||
		( pMirrorOfTamingCard = pTamer->FindItem( (ItemBase::ItemCode) ItemBase::ITEM_CODE_MIRROR_OF_TAMING_CARD ) ) ||
		( pMirrorOfTamingCard = pTamer->FindItem( (ItemBase::ItemCode) ItemBase::ITEM_CODE_MIRROR_OF_TAMING_CARD_TRADABLE ) ) );

	// 거울을 소지하고 있는 경우(만일을 대비한 체크를 함께 진행)
	if( pMirrorOfTamingCard && pMirrorOfTamingCard->IsInInventory() && pMirrorOfTamingCard->GetOwnerHandle() == pTamer->GetHandle() && pMirrorOfTamingCard->GetCount() >= 1 )
	{
		const ItemUID		nItemUID				= pMirrorOfTamingCard->GetItemUID();
		const __int64					nItemEnhanceAndLevel	= pMirrorOfTamingCard->GetItemEnhance() * 100 + pMirrorOfTamingCard->GetItemLevel();
		const ItemBase::ItemCode		nItemCode				= pMirrorOfTamingCard->GetItemCode();
		const __int64					nItemCount				= pMirrorOfTamingCard->GetCount();
		const bool						bDecreaseCount			= pMirrorOfTamingCard->GetItemBase().nType != ItemBase::TYPE_USE;

		// 성공이든 실패든 일단 거울은 박살남(박살 성공 여부에 따라 보호 여부를 최종적으로 결정)
		if( pTamer->EraseItem( pMirrorOfTamingCard, 1 ) )
		{
			bCardProtected = true;

			LOG::Log11N4S( LM_ITEM_USE, pTamer->GetAccountID(), pTamer->GetSID(), nItemEnhanceAndLevel, nItemCode, 1, ( bDecreaseCount ) ? nItemCount - 1 : nItemCount, 0, 0, 0, 0, nItemUID, pTamer->GetAccountName(), LOG::STR_NTS, pTamer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

			SendChatMessage( false, CHAT_ITEM, "@SYSTEM", pTamer, "@243" );
		}
	}

	// 확률 계산
	c_fixed10 fTameProbability = pMonster->GetTamePercetage();
	fTameProbability *= ( pSkill->GetVar( 0 ) * pMonster->GetTamingSkillLevel() + pSkill->GetVar( 1 ) * pSkill->GetEnhance() + 1 ) * 100;

	StructItem* pTestItem = pTamer->GetWearedItem(ItemBase::WEAR_FACE);
	if (pTestItem && pTestItem->GetItemCode() == 2016074) // Crystal Glasses
		fTameProbability += 1;

	pTestItem = pTamer->GetWearedItem(ItemBase::WEAR_WEAPON);
	if (pTestItem && pTestItem->GetItemCode() == 103732) // Crystal Sword
		fTameProbability += 1;

	pTestItem = pTamer->GetWearedItem(ItemBase::WEAR_EMBLEM);
	if (pTestItem && pTestItem->GetItemCode() != 0) // Emblems
		fTameProbability += 1;

	pTestItem = pTamer->GetWearedItem(ItemBase::WEAR_SPARE_SHIELD);
	if (pTestItem && pTestItem->GetItemCode() != 950130) // Emblems
		fTameProbability += 1;

	// 소수 자리 전체까지 확률 처리가 가능하도록 c_fixed10::get 함수를 사용한 값에 대해 랜덤 값 범위에 c_fixed10::FACTOR를 곱한 값을 비교
	if( !pTamer->GetAlwaysPassMix() && fTameProbability.get() < static_cast< int >( XRandom( 1, 1000000 ) ) )
	{
		LOG::Log11N4S( LM_CHARACTER_TAMING, pTamer->GetAccountID(), pTamer->GetSID(), 0, nTameItemCode, pMonster->GetMonsterId(), pItem->GetCount(), (bCardProtected)?pItem->GetCount():pItem->GetCount()-1, 0, pTamer->GetX(), pTamer->GetY(), pItem->GetItemUID(), pTamer->GetAccountName(), LOG::STR_NTS, pTamer->GetName(), LOG::STR_NTS, "", 0, "FAIL", LOG::STR_NTS );

		if( !bCardProtected )
			pTamer->EraseItem( pItem, 1 );

		ClearTamer( pMonster, false );
		BroadcastTamingMessage( pTamer, pMonster, TS_SC_TAMING_INFO::FAILED );

		if( pMonster->GetTameCode() )
		{
			pTamer->UpdateTitleConditionBySummonTame( pMonster->GetTameCode(), GameContent::GetSummonInfo( pMonster->GetTameCode() )->rate, false );
		}

		return false;
	}	

	// nSummonCode는 크리처의 종류 (기존 or 몬스터 크리처) 에 따라 다른 루틴에 의해 결정된다.
	int nSummonCode = ( pMonster->IsMonsterCreatureTame() )? pMonster->GetMonsterCreatureTameCode() : pMonster->GetTameCode();
	int nCode = pItem->GetItemCode();

	LOG::Log11N4S( LM_CHARACTER_TAMING, pTamer->GetAccountID(), pTamer->GetSID(), 0, nTameItemCode, pMonster->GetMonsterId(), pItem->GetCount(), pItem->GetCount()-1, 0, pTamer->GetX(), pTamer->GetY(), pItem->GetItemUID(), pTamer->GetAccountName(), LOG::STR_NTS, pTamer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );
	
	// 성공을 했으면 빈카를 지우고 봉카를 넣어준다. (봉인 되면 중첩불가능이 되므로 새로 넣어줘야 한다.)
	pTamer->EraseItem( pItem, 1 );
	pItem = StructItem::AllocItem( 0, nCode, 1, ItemInstance::BY_TAMING );
	pItem->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_SUMMON );
	pItem->SetSummonCode( nSummonCode );
	StructItem *pNewCard = pTamer->PushItem( pItem, pItem->GetCount() );
	if( pNewCard )
	{
		LOG::Log11N4S( LM_ITEM_TAKE, pTamer->GetAccountID(), pTamer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewCard->GetCount(), 0, pTamer->GetGold().GetRawData(), pTamer->GetX(), pTamer->GetY(), pItem->GetItemUID(), pTamer->GetAccountName(), LOG::STR_NTS, pTamer->GetName(), LOG::STR_NTS, "", 0, "TAMING", LOG::STR_NTS );
	}

	if( pNewCard != pItem )
	{
		StructItem::PendFreeItem( pItem );
	}

	// 정보 보냄
	SendItemMessage( pTamer, pItem );

	BroadcastTamingMessage( pTamer, pMonster, TS_SC_TAMING_INFO::SUCCESS );

	ClearTamer( pMonster, false );

	pTamer->UpdateTitleConditionBySummonTame( nSummonCode, GameContent::GetSummonInfo( nSummonCode )->rate, true );

	return true;
}


StructNPC *g_pCurrentNPC;

struct StructNPC * GetCurrentInitializingNPC()
{
	return g_pCurrentNPC;
}

// 월드에서 Player를 제거
void RemovePlayerFromWorld( struct StructPlayer * pPlayer )
{
	pPlayer->onRemoveFromWorld();

	ArcadiaServer::Instance().RemoveObject( pPlayer );
}

// 월드에 NPC를 추가
void AddNPCToWorld( struct StructNPC * pNPC )
{
	ArcadiaServer::Instance().AddObject( pNPC );

	g_pCurrentNPC = pNPC;
}

// 월드에서 NPC를 제거
void RemoveNPCFromWorld( struct StructNPC * pMob )
{
	pMob->onRemoveFromWorld();

	ArcadiaServer::Instance().RemoveObject( pMob );
}

// 월드에 아이템을 떨어트림
void AddItemToWorld( struct StructItem * pItem )
{
	ArcadiaServer::Instance().AddObject( pItem );

	pItem->SetDropTime( GetArTime() );

	ItemCollector::Instance().RegisterItem( pItem );
}

// 월드에 필드프랍을 생성
void AddFieldPropToWorld( struct StructFieldProp * pProp )
{
	ArcadiaServer::Instance().AddObject( pProp );
}

// 몬스터가 월드에 아이템을 떨어트림
void MonsterDropItemToWorld( struct StructCreature *pCreature, struct StructItem * pItem )
{
	TS_SC_ITEM_DROP_INFO info;
	info.monster_handle = pCreature->GetHandle();
	info.item_handle = pItem->GetHandle();

	ArcadiaServer::Instance().Broadcast( pItem->GetRX(), pItem->GetRY(), pItem->GetLayer(), &info );

	AddItemToWorld( pItem );
}


// 월드에서 아이템을 제거함
const bool RemoveItemFromWorld( struct StructItem * pItem )
{
	if( !ItemCollector::Instance().UnregisterItem( pItem ) )
		return false;

	ArcadiaServer::Instance().RemoveObject( pItem );

	pItem->SetDropTime( 0 );

	return true;
}

// 월드에 몬스터를 추가
void AddMonsterToWorld( struct StructMonster * pMob )
{
	pMob->quadTreeItem.Set( *pMob );
	pMob->quadTreeItem.AddMe();
	pMob->SetReturnPosition( pMob->GetX(), pMob->GetY() );

	ArcadiaServer::Instance().AddObject( pMob );

	// enter 이펙트 끄자
	pMob->SetFirstEnter( false );
}

// 월드에서 몬스터를 제거
void RemoveMonsterFromWorld( struct StructMonster * pMob )
{
	if( !pMob->IsDead() ) pMob->quadTreeItem.RemoveMe();

	pMob->onRemoveFromWorld();

	ArcadiaServer::Instance().RemoveObject( pMob );
}

StructSummon * AllocNewSummon( struct StructPlayer* pMaster, struct StructItem* pCard )
{
	StructSummon * pSummon = StructSummon::AllocSummon( pMaster, pCard->GetSummonCode() );
	pSummon->SetSummonSID( StructPlayer::AllocSummonSID() );
	pSummon->SetLevel( 1 );
	pSummon->CalculateStat();
	pSummon->SetParentCard( pCard );
	if( pSummon->GetParentCard() )
		pSummon->SetJP( pSummon->GetJobPoint() + GameContent::GetCreatureEnhanceInfo( pSummon->GetParentCard()->GetItemEnhance() )->jp_addition );

	if( XRandom() % 100 < 1 )
	{
		pSummon->SetName( GameContent::GetUniqueName( pSummon->GetCreatureGroup() ).c_str() );
	}
	else
	{
		pSummon->SetName( GameContent::GetSummonName().c_str() );
	}

	pCard->SetSummonSID( pSummon->GetSummonSID() );
	pCard->SetSummonStruct( pSummon );

	return pSummon;
}

// 월드에 소환수를 추가
void AddSummonToWorld( struct StructSummon * pSummon )
{
	pSummon->quadTreeItem.Set( *pSummon );
	pSummon->quadTreeItem.AddMe();

	pSummon->SetFirstEnter( true );

	ArcadiaServer::Instance().AddObject( pSummon );
	pSummon->SetSummonFlag( true );

	// enter 이펙트 끄자
	pSummon->SetFirstEnter( false );

	ArcadiaServer::Instance().SetObjectPriority( pSummon, ArObject::UPDATE_PRIORITY_NORMAL );
}

// 월드에서 소환수를 제거
void RemoveSummonFromWorld( struct StructSummon * pSummon )
{	
	ArcadiaServer::Instance().SetObjectPriority( pSummon, ArObject::UPDATE_PRIORITY_IDLE );

	pSummon->quadTreeItem.RemoveMe();

	pSummon->onRemoveFromWorld();

	ArcadiaServer::Instance().RemoveObject( pSummon );
}

struct StructPet * AllocNewPet( struct StructPlayer* pMaster, struct StructItem* pCage, const int nPetCode )
{
	StructPet * pPet = StructPet::AllocPet( pMaster, nPetCode );

	if( !pPet )
		return NULL;

	pPet->SetPetSID( StructPlayer::AllocPetSID() );
	pPet->SetParentCage( pCage );
	pPet->SetName( pPet->GetNameID() );
	pPet->SetNameChanged( !pPet->IsRare() );
	pPet->SetLoginComplete();

	pCage->SetPetSID( pPet->GetPetSID() );
	pCage->SetPetCode( nPetCode );
	pCage->SetPetStruct( pPet );
	pCage->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_CONTAIN_PET );

	return pPet;
}

// 월드에 펫을 추가
void AddPetToWorld( struct StructPet * pPet )
{
	pPet->quadTreeItem.Set( *pPet );
	pPet->quadTreeItem.AddMe();

	pPet->SetFirstEnter( true );

	ArcadiaServer::Instance().AddObject( pPet );
	pPet->SetSummonFlag( true );

	// enter 이펙트 끄자
	pPet->SetFirstEnter( false );

	ArcadiaServer::Instance().SetObjectPriority( pPet, ArObject::UPDATE_PRIORITY_NORMAL );
}

// 월드에서 펫을 제거
void RemovePetFromWorld( struct StructPet * pPet )
{
	ArcadiaServer::Instance().SetObjectPriority( pPet, ArObject::UPDATE_PRIORITY_IDLE );

	pPet->quadTreeItem.RemoveMe();

	pPet->onRemoveFromWorld();

	ArcadiaServer::Instance().RemoveObject( pPet );
}

// 플레이어 스탯 저장
void SavePlayer( struct StructPlayer *pPlayer )
{
	pPlayer->Save();;
}

void SaveAllPlayer()
{
	struct _PlayerSave : ArObjectFunctor
	{
		virtual void operator()( ArObject *pObj ) const
		{
			SavePlayer( static_cast< StructPlayer* >( pObj ) );
		}
	} _fo;

	StructPlayer::DoEachPlayer( _fo );
}

void warpBeginSummon( struct StructPlayer *pPlayer, StructSummon * pSummon )
{
	if( !pSummon || !pSummon->GetSummonFlag() || !pSummon->IsInWorld() )
		return;

	if( pSummon->IsUsingSkill() )
	{
		pSummon->CancelSkill();
	}

	RemoveSummonFromWorld( pSummon );
}

void WarpBegin( struct StructPlayer *pPlayer )
{
	// TODO : 가끔 플레이어가 월드밖에 있을때가 있는듯.. (워프 두번 시도인가..?)

	if( pPlayer->IsUsingSkill() )
	{
		pPlayer->CancelSkill();
	}
	if( pPlayer->IsTrading() )
	{
		StructPlayer::iterator itTradeTarget = StructPlayer::get( pPlayer->GetTradeTarget() );
		StructPlayer * pTradeTarget = (*itTradeTarget);

		if( pTradeTarget )
			pTradeTarget->CancelTrade( true );

		pPlayer->CancelTrade( true );
	}
	if( pPlayer->IsInWorld() )
	{
		RemovePlayerFromWorld( pPlayer );
	}

	StructSummon * pSummon = NULL;
	if( pSummon = pPlayer->GetMainSummon() )
		warpBeginSummon( pPlayer, pSummon );

	if( pSummon = pPlayer->GetSubSummon() )
		warpBeginSummon( pPlayer, pSummon );

	StructPet *pPet = pPlayer->GetSummonedPet();
	if( pPet && pPet->GetSummonFlag() && pPet->IsInWorld() )
	{
		if( pPet->IsUsingSkill() )
			pPet->CancelSkill();

		RemovePetFromWorld( pPlayer->GetSummonedPet() );
	}
}

void warpEndSummon( struct StructPlayer *pPlayer, const struct ArPosition * pPosition, unsigned char layer, StructSummon * pSummon, bool bInDungeon )
{
	if( !pSummon || !pSummon->GetSummonFlag() )
		return;

	pSummon->SetCurrentXY( pPosition->x, pPosition->y );
	pSummon->SetCurrentLayer( layer );
	pSummon->StopMove();
	pSummon->SetFirstEnter( true );

	if( pSummon != pPlayer->GetRideObject() )
	{
		ArPosition valid_pos;
		if( GameContent::GetValidRandomPos( *pPosition, valid_pos, StructSummon::SUMMON_SPAWN_LENGTH ) == false )
		{
			valid_pos = pPlayer->GetPos();
		}

		pSummon->SetCurrentXY( valid_pos.x, valid_pos.y );
	}

	ArcadiaServer::Instance().AddObject( pSummon );
	pSummon->SetFirstEnter( false );

	ArPosition pos = pSummon->GetCurrentPosition( GetArTime() );
	ArcadiaServer::Instance().SetMove( pSummon, pos, pos, 0, true, GetArTime() );
}

void WarpEnd( struct StructPlayer *pPlayer, const struct ArPosition * pPosition, unsigned char layer )
{
	if( !pPlayer->GetSID() || !strlen( pPlayer->GetName() ) )
	{
		FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "BugReport", "Use No Character Login Bug [%s]", pPlayer->GetAccountName() );
		GameRule::RegisterBlockAccount( pPlayer->GetAccountName() );
		if( pPlayer->pConnection && pPlayer->pConnection->IsConnected() )
			pPlayer->pConnection->Close();
		else
		{
			StructPlayer::UnRegisterAccount( pPlayer->GetAccountName() );

		}
		return;
	}

	LOG::Log11N4S( LM_CHARACTER_WARP, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, pPlayer->GetLayer(), layer, pPlayer->GetX(), pPlayer->GetY(), pPosition->x, pPosition->y, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

	int nChannelID = ChannelManager::GetChannelId( pPosition->GetX(), pPosition->GetY() );

	if( layer != pPlayer->GetLayer() )
	{
		if( ChannelManager::GetChannelId( pPlayer->GetX(), pPlayer->GetY() ) && pPlayer->GetLayer() )
		{
			ChannelManager::LeavePlayerFromLayer( pPlayer->GetLayer() );
		}

		if( nChannelID && layer )
		{
			ChannelManager::EnterPlayerToLayer( layer );
		}
	}

	pPlayer->SetCurrentXY( pPosition->x, pPosition->y );
	pPlayer->SetCurrentLayer( layer );
	pPlayer->StopMove();

	pPlayer->SetWarpEnded( true );

	SendWarpMessage( pPlayer );	

	StructSummon * pSummon = NULL;
	if( pSummon = pPlayer->GetMainSummon() )
		warpEndSummon( pPlayer, pPosition, layer, pSummon, DungeonManager::Instance().GetDungeonID( pPosition->x, pPosition->y ) != 0 );

	if( pSummon = pPlayer->GetSubSummon() )
		warpEndSummon( pPlayer, pPosition, layer, pSummon, DungeonManager::Instance().GetDungeonID( pPosition->x, pPosition->y ) != 0 );

	StructPet *pPet = pPlayer->GetSummonedPet();
	if( pPet )
	{
		ArPosition valid_pos;
		if( GameContent::GetValidRandomPos( *pPosition, valid_pos, StructPet::PET_SPAWN_MIN_LENGTH ) == false )
		{
			valid_pos = pPlayer->GetPos();
		}

		pPet->SetCurrentXY( valid_pos.x, valid_pos.y );
		pPet->SetCurrentLayer( layer );
		pPet->StopMove();

		pPet->SetFirstEnter( true );

		ArcadiaServer::Instance().AddObject( pPet );
		pPet->SetFirstEnter( false );

		ArPosition pos = pPet->GetCurrentPosition( GetArTime() );
		ArcadiaServer::Instance().SetMove( pPet, pos, pos, 0, true, GetArTime() );
	}

	pPlayer->SetInvincible( true );

	if( pPlayer->GetMainSummon() )
	{
		pPlayer->GetMainSummon()->SetInvincible( true );
	}

	if( pPlayer->GetSubSummon() )
	{
		pPlayer->GetSubSummon()->SetInvincible( true );
	}

	pPlayer->SetFirstEnter( true );
	ArcadiaServer::Instance().AddObject( pPlayer );	
	pPlayer->SetFirstEnter( false );

	ArPosition pos = pPlayer->GetCurrentPosition( GetArTime() );
	ArcadiaServer::Instance().SetMove( pPlayer, pos, pos, 0, true, GetArTime() );

	SendPropertyMessage( pPlayer, pPlayer->GetHandle(), "channel", ChannelManager::GetChannelNum( layer ) );

	pPlayer->ChangeLocation( pPlayer->GetX(), pPlayer->GetY() );

	// 탑승 상태로 탑승 불가 지역으로 워프하는 경우 탑승 해제 처리
	// * IsMountable() 함수가 캐릭터의 현재 WorldLocation 정보에 의존적인 처리여서 ChangeLocation 아래에서 처리함
	//   뭣하면 ChangeLocation 함수 안쪽으로 넣어도 무방하지 않나 싶은데,
	//   일단은 기존에 있던 코드에서 큰 변경을 만들지 않기 위해 처리되던 부분에서 위치만 변경함(ChannelManager 참조할 때는 ChangeLocation 위에서 처리했음)
	if( ( pPlayer->IsRiding() || pPlayer->HasRidingState() ) && !pPlayer->IsMountable( false ) )
	{
		pPlayer->UnMount();
	}

	pPlayer->Save( true );
}

