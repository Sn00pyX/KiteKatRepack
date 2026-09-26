#pragma once

#include "LuaVM.h"

int SCRIPT_Warp( struct lua_State *L );
int SCRIPT_ForceWarp( struct lua_State *L );
int SCRIPT_RecallPlayer( struct lua_State *L );
int SCRIPT_SetSpeed( struct lua_State *L );
int SCRIPT_Stat( struct lua_State *L );
int SCRIPT_SaveAllPlayer( struct lua_State *L );
int SCRIPT_SavePlayer( struct lua_State *L );
int SCRIPT_HasItem( struct lua_State *L );
int SCRIPT_GetPlayerFlag( struct lua_State *L );
int SCRIPT_SetPlayerFlag( struct lua_State *L );
int SCRIPT_RemovePlayerFlag( struct lua_State *L );
int SCRIPT_GetAccountFlag( struct lua_State *L );
int SCRIPT_SetAccountFlag( struct lua_State *L );
int SCRIPT_RemoveAccountFlag( struct lua_State *L );
int SCRIPT_GetTargetValue( struct lua_State *L );
int SCRIPT_GetHandleValue( struct lua_State *L );
int SCRIPT_GetValue( struct lua_State *L );
int SCRIPT_SetValue( struct lua_State *L );
int SCRIPT_EchoValue( struct lua_State *L );
int SCRIPT_GetCreatureValue( struct lua_State *L );
int SCRIPT_GetMainCreatureValue( struct lua_State *L );
int SCRIPT_SetCreatureValue( struct lua_State *L );
int SCRIPT_ChangeCreatureName( struct lua_State *L );
int SCRIPT_SetMainCreatureValue( struct lua_State *L );
int SCRIPT_EchoMainCreatureValue( struct lua_State *L );
int SCRIPT_GetCreatureHandle( struct lua_State *L );
int SCRIPT_AddValue( struct lua_State *L );
int SCRIPT_AddExpJP( struct lua_State *L );
int SCRIPT_IsChangeableJob( struct lua_State *L );
int SCRIPT_GetAllValue( struct lua_State *L );
int SCRIPT_Message( struct lua_State *L );

int SCRIPT_AddState( struct lua_State *L );
int SCRIPT_GetStateLevel( struct lua_State *L );
int SCRIPT_RemoveState( struct lua_State *L );
int SCRIPT_AddCreatureState( struct lua_State *L );
int SCRIPT_RemoveCreatureState( struct lua_State *L );
int SCRIPT_AddEventState( struct lua_State *L );
int SCRIPT_RemoveEventState( struct lua_State *L );
int SCRIPT_ClearEventState( struct lua_State *L );
int SCRIPT_GetEventStateList( struct lua_State *L );
int SCRIPT_CastWorldState( struct lua_State *L );

int SCRIPT_Kick( struct lua_State *L );
int SCRIPT_ForceUnregisterAccount( struct lua_State *L );
int SCRIPT_CreatureEvolution( struct lua_State *L );
int SCRIPT_CreatureEnhance( struct lua_State *L );
int SCRIPT_GetSummonNameId( struct lua_State *L );
int SCRIPT_ResetSummonSkill( struct lua_State *L );

int SCRIPT_ShowCreatureFarmWindow( struct lua_State *L );
int SCRIPT_GetSummonList( struct lua_State *L );
int SCRIPT_GetFarmedSummonList( struct lua_State *L );
int SCRIPT_GetFarmedSummonCount( struct lua_State *L );
int SCRIPT_GetFarmedSummonValue( struct lua_State *L );
int SCRIPT_GetFarmedSummonLevel( struct lua_State *L );
int SCRIPT_GetSummonRate( struct lua_State *L );
int SCRIPT_FarmSummon( struct lua_State *L );
int SCRIPT_RegainSummon( struct lua_State *L );
int SCRIPT_NurseSummon( struct lua_State *L );

int SCRIPT_GetMainTitle( struct lua_State *L );
int SCRIPT_SetMainTitle( struct lua_State *L );
int SCRIPT_GetSubTitle( struct lua_State *L );
int SCRIPT_SetSubTitle( struct lua_State *L );
int SCRIPT_OpenTitle( struct lua_State *L );
int SCRIPT_AchieveTitle( struct lua_State *L );
int SCRIPT_UpdateTitleCondition( struct lua_State *L );
int SCRIPT_SetRemainTitleTime( struct lua_State *L );

int SCRIPT_GetBaseSkillLevel( struct lua_State *L );
int SCRIPT_LearnSkill( struct lua_State *L );
int SCRIPT_LearnAllSkill( struct lua_State *L );
int SCRIPT_LearnCreatureAllSkill( struct lua_State *L );
int SCRIPT_CreatureLearnSkill( struct lua_State *L );
int SCRIPT_ResetSkill( struct lua_State *L );
int SCRIPT_SetJob( struct lua_State *L );
int SCRIPT_ResetJob( struct lua_State *L );
int SCRIPT_SetRace( struct lua_State *L );
int SCRIPT_GetUserCountNear( struct lua_State *L );

int SCRIPT_GetProperChannelNo( struct lua_State *L );
int SCRIPT_GetMinChannelNo( struct lua_State *L );
int SCRIPT_GetMaxChannelNo( struct lua_State *L );
int SCRIPT_GetUserCountInChannel( struct lua_State *L );
int SCRIPT_GetLayerOfChannel( struct lua_State *L );
int SCRIPT_EquipSummonCard( struct lua_State *L );
int SCRIPT_ShowChannelSet( struct lua_State *L );

// 일반화된 다이얼로그
int SCRIPT_DialogGeneral( struct lua_State *L );
int SCRIPT_DialogSpecial( struct lua_State *L );

// 숨겨진 던전
int SCRIPT_EnterSecretDungeon( struct lua_State *L );
int SCRIPT_WarpToSecretDungeon( struct lua_State *L );
// 인스턴스 던전
int SCRIPT_EnterInstanceDungeon( struct lua_State *L );
int SCRIPT_EnterOtherInstanceDungeon( struct lua_State *L );
int SCRIPT_SpecialMenu(struct lua_State* L);
int SCRIPT_WarpToInstanceDungeon( struct lua_State *L );
int SCRIPT_LeaveInstanceDungeon( struct lua_State *L );
int SCRIPT_QuitInstanceDungeon( struct lua_State *L );
int SCRIPT_GetInstanceDungeonID( struct lua_State *L );
int SCRIPT_SendMissionTitle( struct lua_State *L );
int SCRIPT_SendMissionReward( struct lua_State *L );
int SCRIPT_SendMissionObjective( struct lua_State *L );
int SCRIPT_SendMissionObjectiveProgress( struct lua_State *L );

int SCRIPT_WarpToDungeon( struct lua_State *L );
int SCRIPT_WarpToStoredPosition( struct lua_State *L );
int SCRIPT_BeginDungeonRaid( struct lua_State *L );
int SCRIPT_ShowDungeonStone( struct lua_State *L );
int SCRIPT_RespawnGuardian( struct lua_State *L );
int SCRIPT_RequestDungeonRaid( struct lua_State *L );
int SCRIPT_CancelDungeonRaid( struct lua_State *L );
int SCRIPT_ChangeTacticalPositionOwner( struct lua_State *L );
int SCRIPT_DropDungeonOwnership( struct lua_State *L );
int SCRIPT_GetOwnDungeonID( struct lua_State *L );
int SCRIPT_GetSiegeDungeonID( struct lua_State *L );
int SCRIPT_ChangeDungeonOwner( struct lua_State *L );
int SCRIPT_ClearDungeonCoreGuardian( struct lua_State *L );
int SCRIPT_IsEnterableSiegeDungeon( struct lua_State *L );
int SCRIPT_IsInSiegeDungeon( struct lua_State *L );
int SCRIPT_GetOwnGuildName( struct lua_State *L );
int SCRIPT_GetDungeonRelation( struct lua_State *L );
int SCRIPT_GetTaxRate( struct lua_State *L );
int SCRIPT_SetTaxRate( struct lua_State *L );
int SCRIPT_GetTaxAmount( struct lua_State *L );
int SCRIPT_DrawTax( struct lua_State *L );
int SCRIPT_GetTaxChaosAmount( struct lua_State *L );
int SCRIPT_DrawTaxChaos( struct lua_State *L );

int SCRIPT_SetHuntaholicPoint( struct lua_State *L );

int SCRIPT_WarpToRevivePosition( struct lua_State *L );

int SCRIPT_RecallFeather( struct lua_State *L );

int SCRIPT_SetAutoUser( struct lua_State *L );
int SCRIPT_SetAutoAccount( struct lua_State *L );
int SCRIPT_ClearAutoAccount( struct lua_State *L );

int SCRIPT_SendGoldChaosUpdate( struct lua_State *L );

int SCRIPT_ShowDonationMenu( struct lua_State *L );

int SCRIPT_GetCreatureNameID( struct lua_State *L );
int SCRIPT_ShowCreatureNameChangeBox( struct lua_State *L );

int SCRIPT_SetPCBangUser( struct lua_State *L );
int SCRIPT_SetContinuousPlayTime( struct lua_State *L );

int SCRIPT_ShowAuctionWindow( struct lua_State *L );
int SCRIPT_CancelAuctionBySeller( struct lua_State *L );

int SCRIPT_ShowHuntaholicLobbyWindow( struct lua_State *L );

int SCRIPT_GetEventAreaEnterCount( struct lua_State *L );
int SCRIPT_SetEventAreaEnterCount( struct lua_State *L );

int SCRIPT_SetPKMode( struct lua_State *L );
int SCRIPT_Kill( struct lua_State *L );
int SCRIPT_KillTarget( struct lua_State *L );

int SCRIPT_PrivateNotice( struct lua_State *L );

int SCRIPT_GetBattleArenaBlockTime( struct lua_State *L );
int SCRIPT_SetBattleArenaBlockTime( struct lua_State *L );
int SCRIPT_GetBattleArenaPenaltyCount( struct lua_State *L );
int SCRIPT_SetBattleArenaPenaltyCount( struct lua_State *L );
int SCRIPT_GetBattleArenaNextPenaltyDecTime( struct lua_State *L );
int SCRIPT_SetBattleArenaNextPenaltyDecTime( struct lua_State *L );
int SCRIPT_GetBattleArenaTeamNo( struct lua_State *L );

int SCRIPT_SetSecrouteFreePass( struct lua_State *L );

int SCRIPT_SetChatBlockTime( struct lua_State *L );
int SCRIPT_SetInvisible( struct lua_State *L );
int SCRIPT_Rebirth( struct lua_State *L );
int SCRIPT_SetLevel( struct lua_State *L );
int SCRIPT_Heal( struct lua_State *L );
int SCRIPT_SetAccountAuthority( struct lua_State *L );
