
#include <toolkit/ILock.h>

#include "DBAllocator.h"
#include "GameDBManager.h"
#include "DB_Commands.h"
// 2009. 8. 11 FLOYD 보안비밀번호 2차 수정
// #include "DB_CheckSecurityNo.h"


static XCriticalSection s_csDBHeapLock( "s_csDBHeapLock" );

XMemoryPool & GetDBHeap()
{
	static XMemoryPool heap(
		COMPILE_TIME_SIZE_COMPARE( DB_OnStartUp,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateOneTimePassword,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateCharacter,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateCharacterHair,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateCharacterSkin, 
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateCharacterHideEquipFlag,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdatePlayTimePoint,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateTimeBasedEvent,
		COMPILE_TIME_SIZE_COMPARE( DB_CheckCharacterName,
		COMPILE_TIME_SIZE_COMPARE( DB_ChangeCharacterName,
		COMPILE_TIME_SIZE_COMPARE( DB_ChangeCharacterAlias,
		COMPILE_TIME_SIZE_COMPARE( DB_Login,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateClientInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateQuickSlot,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateCurrentKey,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateSavedKey,
		COMPILE_TIME_SIZE_COMPARE( DB_Logout,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateItemOwner,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertSummon,
		COMPILE_TIME_SIZE_COMPARE( DB_ReadStorageList,
		COMPILE_TIME_SIZE_COMPARE( DB_ReadCommercialStorageList,
		COMPILE_TIME_SIZE_COMPARE( DB_TakeOutCommercialItem,
		COMPILE_TIME_SIZE_COMPARE( DB_GetCommercialStorageInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertItem,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertFavor,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateItem,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateSummon,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertPet,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdatePet,
		COMPILE_TIME_SIZE_COMPARE( DB_ChangePetName,
		COMPILE_TIME_SIZE_COMPARE( DB_CharacterList,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteCharacter,
		COMPILE_TIME_SIZE_COMPARE( DB_CreateCharacter,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteParty,
		COMPILE_TIME_SIZE_COMPARE( DB_SetParty,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertParty,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdatePartyInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertSkill,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateStorageGold,
		COMPILE_TIME_SIZE_COMPARE( DB_ResetSkill,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteSkill,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateSkill,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteGuild,
		COMPILE_TIME_SIZE_COMPARE( DB_ChangeGuildName,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuild,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildBlockTime,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateGuildDonationPoint,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateGuildGradePoint,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateGuildMemberPoint,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertEventAreaEnterCount,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateEventAreaEnterCount,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertFriend,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteFriend,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertGuild,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildGold,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildAllianceID,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildAllianceBlockTime,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertAlliance,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateAllianceInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_ChangeAllianceName,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteAllianceInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildLeader,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildDungeonID,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildDungeonBlockTime,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildNotice,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildURL,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildPermission,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildPermissionName,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildPermissionSet,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildMemberMemo,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildAdvertiseTypeAndTime,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildAdvertiseComment,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildRecruiting,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildIcon,
		COMPILE_TIME_SIZE_COMPARE( DB_SetGuildBanner,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertQuest,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertRandomQuest,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateRandomQuest,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateQuest,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteQuest,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertQuestCoolTime,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateQuestCoolTime,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteQuestCoolTime,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertTitle,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateTitle,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateEventItemDropInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateEventItemSupplyInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateDungeonRaidTime,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateDungeon,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateDungeonOwnerGuild,
		COMPILE_TIME_SIZE_COMPARE( DB_ClearDungeonRaidRecord,
		COMPILE_TIME_SIZE_COMPARE( DB_EraseUserFromAutoAccountList,
		COMPILE_TIME_SIZE_COMPARE( DB_ClearAutoAccountList,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateLaunchedScheduledCommand,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertAuctionInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateAuctionBiddingInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteAuctionInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertNewItemAndKeepingInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteItemKeepingInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteAuctionInfoAndInsertItemKeepingInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateItemAuctionKeepingID,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertAutoAuctionRegistrationInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_SetRankingScore,
		COMPILE_TIME_SIZE_COMPARE( DB_InvalidateRankingScore,
		COMPILE_TIME_SIZE_COMPARE( DB_SettleRanking,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertGlobalVariable,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateGlobalVariable,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteGlobalVariable,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertCash,
		COMPILE_TIME_SIZE_COMPARE( DB_InsertFarmInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateNursingTime,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteFarmInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateBattleArenaPenalty,
		COMPILE_TIME_SIZE_COMPARE( DB_SetRandomOption,
		COMPILE_TIME_SIZE_COMPARE( DB_DeleteRandomOption,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateItemAwakenSID,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateItemRandomOptionSID,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateRandomOptionOwnerInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_ReadAccountAuthorityInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_SetAccountAuthorityInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_CloseAccountAuthorityInfo,
		COMPILE_TIME_SIZE_COMPARE( DB_UpdateItemCode,
		0 
		) ) ) ) ) ) ) ) ) ) ) ) ) )
		) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) )
		) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) )
		) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) ) )
		// 뭐 나도 이렇게 몰아서 괄호 닫고 싶진 않았는데...
		// 재귀 템플릿화에 능한 누군가가 좀 고쳐주셈;;
#ifdef _DEBUG
		, 100, 2, false );
#else
		, 500, 6, false );
#endif

	return heap;
}

void InitDBHeap()
{
	// 싱글톤 초기화. (안해주면 난리남)
	GetDBHeap();
	//
}

struct GameDBManager::DBProc * allocDBProcStruct()
{
	THREAD_SYNCHRONIZE( s_csDBHeapLock );

	struct GameDBManager::DBProc * pHeapBlock = reinterpret_cast< struct GameDBManager::DBProc * >( GetDBHeap().Alloc() );

	return pHeapBlock;
}

void prepareFreeDBProcStruct( struct GameDBManager::DBProc * pStruct )
{
	THREAD_SYNCHRONIZE( s_csDBHeapLock );

	GetDBHeap().PrepareFree( pStruct );
}

void freeDBProcStruct( struct GameDBManager::DBProc * pStruct )
{
	THREAD_SYNCHRONIZE( s_csDBHeapLock );

	GetDBHeap().Free( pStruct );
}
