#pragma once

#include <mmo/ArType.h>

#include "GameMessage.h"

#define SENDMSG_BUFFER_SIZE	16384

extern struct StructPlayer *GetCurrentThreadPlayer();

/*
void RegisterIOCPConnection( struct IConnection *pConnection );
bool LockIOCPConnection( struct IConnection *pConnection );
void UnLockIOCPConnection( struct IConnection *pConnection );
bool UnregisterIOCPConnection( struct IConnection *pConnection );
*/

void InitMessageLogger();
void DeInitMessageLogger();
void SetMessagingInfo( unsigned short msg_id, __int64 msg_size, bool bIsSendMessage );

void SendResult( const struct ArObject *pObj, unsigned short msg, unsigned short result, int value = 0 );
inline void SendResult( const struct ArObject *pObj, unsigned short msg, unsigned short result, AR_HANDLE value )	{ SendResult( pObj, msg, result, (int)value ); }
void PendStream( const struct ArObject *pObj, const void *pBuffer, unsigned size );
void PendMessage( const struct ArObject *pObj, const struct TS_MESSAGE * pMessage );

// 아래 두 함수는 느리다. 왜냐하면 pConnection 의 유효성 검사를 동반하기 때문이다.
void SendResult( struct IStreamSocketConnection *pConnection, unsigned short msg, unsigned short result, int value = 0 );
void PendMessage( struct IStreamSocketConnection *pConn, const struct TS_MESSAGE * pMessage );
void PendString( struct IStreamSocketConnection *pConnection, const char *szStr );

// 로긴 관련
void SendTimeSync( struct StructPlayer * pPlayer );
void SendCharacterList( struct IStreamSocketConnection * pConn, struct LOBBY_CHARACTER_INFO *pInfo, unsigned short nCount, unsigned short nLastLoginIndex );
void SendLoginResult( struct IStreamSocketConnection * pConn, struct StructPlayer * pClient, const char nPCBangMode, const unsigned short nResult );
void SendAccountResultWithAuth( struct IStreamSocketConnection * pConn, const char *szAccount, int nError );
void SendDisconnectDesc( struct IStreamSocketConnection * pConn, const TS_SC_DISCONNECT_DESC::_DISCONNECT_TYPE eDisconnectionDesc );

void SendEnterMsg( const ArObject * pClient, const ArObject * pObj );
void SendLeaveMsg( const ArObject * pClient, const ArObject * pObj );
void SendMoveMsg( const ArObject * pClient, const ArObject * pObj );
void SendForceMoveMsg( const ArObject * pClient, const ArObject * pObj );

void SendGameTime( const ArObject * pClient );

// 귓속말

// 채팅
void SendChatMessage( bool bLock, int nChatType, const char *szSenderName, struct StructPlayer *pTarget, const char *szString, unsigned len = 0 );
void SendChatMessage( bool bLock, int nChatType, const char *szSenderName, AR_HANDLE hTarget, const char *szString, unsigned len = 0 );
void SendChatMessage( bool bLock, int nChatType, const char *szSenderName, const char *szTarget, const char *szString, unsigned len = 0 );
//void SendChatMessage( int nChatType, const char *szSenderName, struct StructPlayer *pTarget, const char *szString, unsigned len = 0 );
void PrintfChatMessage( bool bLock, int nChatType, const char *szSenderName, struct StructPlayer *pTarget, const char *szString, ... );
//void PrintfChatMessage( int nChatType, const char *szSenderName, struct StructPlayer *pTarget, const char *szString, ... );

// 퀘스트
void SendQuestMessage( int nChatType, struct StructPlayer *pTarget, const char *szString, ... );

// 이하 락됨
void PrintfPartyChatMessage( int nChatType, int nPartyID, const char *szString, ... );
void PrintfPartyChatMessage( int nPartyID, const char *szString, ... );
void PrintfGuildChatMessage( int nChatType, const char * szSender, int nGuildID, const char *szString, ... );
void PrintfGuildChatMessage( int nChatType, int nGuildID, const char *szString, ... );
void PrintfGuildChatMessage( int nGuildID, const char *szString, ... );
void PrintfAllianceChatMessage( int nChatType, int nAllianceID, const char * szSender, const char * szString, ... );
void PrintfAllianceChatMessage( int nAllianceID, const char * szSender, const char * szString, ... );
void PrintfAllianceChatMessage( int nAllianceID, const char * szString, ... );
void SendLocalChatMessage( int nChatType, AR_HANDLE handle, const char *szString, unsigned len );
void SendGlobalChatMessage( int nChatType, const char *szSenderName, const char *szString, unsigned len = 0 );
void SendGuildNotify( struct StructPlayer *pPlayer );
void SendLocalChatMessage( int ChatType, unsigned rx, unsigned ry, unsigned char layer, const char *szSenderName, const char *szString, unsigned len );
void BroadcastChatMessage( int ChatType, unsigned rx, unsigned ry, unsigned char layer, const char *szSenderName, const char *szString, unsigned len );
void PrintfLocalChatMessage( bool bLock, int ChatType, unsigned rx, unsigned ry, unsigned char layer, const char *szSenderName, const char * szString, ... );
void SendYellChatMessage( int nChatType, AR_HANDLE handle, const char *szString, unsigned len );
void PrintfGlobalChatMessage( int nChatType, const char *szSenderName, const char *szString, ... );

void SendPartyChatMessage( int nChatType, const char *szSender, int nPartyID, const char *szString, unsigned len = 0 );
void SendPartyChatMessage( const char *szSender, int nPartyID, const char *szString, unsigned len = 0 );

void SendGuildChatMessage( int nChatType, const char *szSender, int nGuildID, const char *szString, unsigned len = 0 );
void SendGuildChatMessage( const char *szSender, int nGuildID, const char *szString, unsigned len = 0 );

void BroadcastTamingMessage( struct StructPlayer *pPlayer, struct StructCreature *pMonster, int mode );

void SendShowCreateGuild( struct StructPlayer *pPlayer );
void SendShowCreateAlliance( struct StructPlayer *pPlayer );

// 아이템
void fillItemBaseInfo( struct TS_ITEM_BASE_INFO *pInfo, struct StructItem * pItem );
void fillItemInfo( struct TS_ITEM_INFO *pInfo, struct StructItem *pItem );
void SendItemMessage( struct StructPlayer * pPlayer, struct StructItem *pItem );
void SendStorageItemDestroyMessage( struct StructPlayer * pPlayer, struct StructItem *pItem );
void SendItemDestroyMessage( struct StructPlayer * pPlayer, struct StructItem *pItem );
void SendItemCountMessage( struct StructPlayer * pPlayer, struct StructItem *pItem );
void SendDropResult( struct StructPlayer * pClient, AR_HANDLE ItemHandle, bool bIsSuccess );
void SendTakeResult( struct StructPlayer * pClient, struct StructItem *pItem );
void SendItemList( struct StructPlayer * pClient, bool bIsStorage );
void SendTradeCancelMessage( struct StructPlayer * pClient );
void SendMixResult( struct StructPlayer * pClient, AR_HANDLE *pHandle, unsigned count, unsigned int type = 0 );
void SendDecomposeResult( struct StructPlayer * pClient, TS_SC_DECOMPOSE_RESULT::DECOMPOSE_INFO * pItemResults, unsigned int count );
void SendSkillCardInfo( struct StructItem *pItem );
void SendShowSoulStoneCraftWindow( struct StructPlayer * pPlayer );
void SendShowSoulStoneRepairWindow( struct StructPlayer * pPlayer );
void SendJobInfo( struct StructPlayer *pPlayer, struct StructCreature *pCreature );
void SendStorageInfo( struct StructPlayer * pClient );
void SendOpenStorageMessage( struct StructPlayer *pPlayer );
void SendBeltSlotInfo( struct StructPlayer *pPlayer );
void SendCreatureEquipMessage( const StructPlayer *pPlayer, bool bShowDialog );
void SendWarpMessage( const StructPlayer *pPlayer );
void SendItemCoolTimeInfo( struct StructPlayer *pPlayer );

void BroadcastSummonEvolutionMessage( struct StructPlayer * pClient, struct StructSummon *pSummon );
void SendAddSummonMessage( struct StructPlayer * pClient, struct StructSummon *pSummon );
void SendRemoveSummonMessage( struct StructPlayer * pClient, struct StructSummon *pSummon );
void SendAddPetMessage( struct StructPlayer * pClient, struct StructPet *pPet );
void SendRemovePetMessage( struct StructPlayer * pClient, struct StructPet *pPet );
void SendWarpMessage( const StructPlayer *pPlayer );
void SendStatInfo( struct StructPlayer * pClient, struct StructCreature *pCreature );
void SendSPMsg( const StructPlayer *pPlayer, struct StructSummon *pSummon );
void SendTargetMsg( const struct StructPlayer *pPlayer, AR_HANDLE target );
void SendGoldChaosUpdateMsg( const struct StructPlayer *pPlayer );
void SendExpMsg( const StructPlayer *pPlayer, const StructCreature *pCreature );
void SendStateMessage( const struct StructPlayer *pPlayer, AR_HANDLE target_handle, const struct StructState *p, bool bIsCancel = false );
void BroadcastStateMessage( struct StructCreature *pCreature, struct StructState *p, bool bIsCancel = false );
void BroadcastLevelMsg( const struct StructCreature *pPlayer );
void SendLevelMsg( const struct StructPlayer *pPlayer, const StructCreature *pCreature );
void SendPropertyMessage( const struct StructPlayer *pPlayer, AR_HANDLE handle, const char *szPropertyName, const __int64 & value );
void SendPropertyMessage( const struct StructPlayer *pPlayer, AR_HANDLE handle, const char *szPropertyName, const char *szValue );
void BroadcastPropertyMessage( unsigned rx, unsigned ry, unsigned char layer, AR_HANDLE handle, const char *szPropertyName, const __int64 & value );
void SendSkillMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature, int skill_id = -1, bool refresh = false );		// skill_id 가 -1 이면 다보냄.
void SendPetSkillMessage(const struct StructPlayer* pPlayer, const struct StructCreature* pCreature, int skill_id = -1, bool refresh = false);		// skill_id 가 -1 이면 다보냄.
void SendSkillMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature, std::vector< int > vSkill );
void SendSkillResetMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature );
void SendAddedSkillMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature );
void SendSkillLevelMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature );
void BroadcastSkillEffectMessage( const struct StructCreature *pCaster, const struct StructCreature *pTarget, int skill_level, int skill_id, int var1, unsigned char var2, char var3 );
void SendDetectRangeMessage( const struct StructPlayer *pPlayer, const struct StructCreature *pCreature );
void SendCantAttackMsg( const struct StructPlayer *pPlayer, AR_HANDLE attacker_handle, AR_HANDLE target_handle, int reason );
void SendAuraMsessage( const struct StructPlayer *pCreature, const unsigned short skillId, const bool status = false );
void SendAuraMsessage( const struct StructCreature *pCreature, const unsigned short skillId, const bool status = false );

void SendHPMPMsg( const struct StructPlayer * pPlayer, const struct StructCreature * pCreature, int add_hp, short add_mp, bool need_to_display = false );
void BroadcastHPMPMsg( const struct StructCreature *pCreature, int add_hp, short add_mp, bool need_to_display = false );

void GetWearMsg( struct StructCreature *pCreature, struct TS_WEAR_INFO & itemInfoMsg );
void SendWearMessage( struct StructPlayer *pPlayer );
void SendWearMessage( struct StructPlayer *pPlayer, struct StructCreature *pCreature );
void SendTotalItemWearInfoMessage( StructPlayer *pPlayer, StructCreature *pTarget );
void SendItemWearInfoMessage( StructPlayer *pPlayer, StructCreature *pTarget, StructItem *pItem );

// NPC
void SendWindowMessage( struct StructPlayer *pPlayer, const char *szWindow, const char *szArgument, const char *szTrigger );
void SendDialogMessage( struct StructPlayer *pPlayer, AR_HANDLE npc_handle, int type, const char *szTitle, const char *szText, const char *szMenu );
void SendMarketInfo( struct StructPlayer *pPlayer, AR_HANDLE npc_handle, struct _MARKET_INFO *pInfo );

// 메시지 박스
void SendGeneralMessageBox( struct StructPlayer *pPlayer, const char *szText );

void SendLinkedPartyChatMessage( bool bLock, int nChatType, const char *szSender, int nPartyID, const char *szString );
void SendAttackTeamChatMessageByGuildID( bool bLock, int nChatType, const char *szSender, int nGuildID, const char *szString );
void PrintfLinkedPartyChatMessage( bool bLock, int nChatType, const char *szSender, int nPartyID, const char *szString, ... );
void PrintfAttackTeamChatMessageByGuildID( bool bLock, int nChatType, const char *szSender, int nGuildID, const char *szString, ... );

void BroadcastPartyMemberInfo( int nPartyID, struct StructPlayer *pPlayer );
void BroadcastLinkedPartyMemberInfo( int nPartyID, struct StructPlayer *pPlayer );

void BroadcastLinkedPartyInfo( int nPartyID );
// bByBattleArena 부분은 추후에 파티 탈퇴/해산 사유에 대해 방송해야될 케이스가 더 많아지면
// enum 상수로 변경해서 CHAT_PARTY_SYSTEM 타입의 LEAVE, DESTROY에 해당 enum 상수를 넣어
// 방송하도록 수정하는 편이 나을 것으로 생각됨
void BroadcastPartyLeave( struct StructPlayer *pPlayer, bool bByBattleArena = false );
void BroadcastPartyLeave( int nPartyID, const char * pszName, bool bByBattleArena = false );
void BroadcastPartyDestroy( int nPartyID, bool bByBattleArena = false );
void SendLinkedPartyInfo( struct StructPlayer *pPlayer );

void SendPartyInfo( struct StructPlayer *pPlayer );
void SendPartyInfoByID( struct StructPlayer *pPlayer, const int nPartyID );
void SendPartyPosition( struct StructPlayer *pPlayer );
void BroadcastGuildMemberInfo( int nGuildID, struct StructPlayer *pPlayer );
void SendGuildStaticInfo( struct StructPlayer * pPlayer );
void SendGuildMemberInfo( struct StructPlayer * pPlayer );
void SendGuildInfo( struct StructPlayer *pPlayer );
void SendGuildPermissionInfo( struct StructPlayer *pPlayer );
void SendAllianceInfo( struct StructPlayer *pPlayer );
void BroadcastAllianceInfo( const int nAllianceID );
void SendGuildAdvertiseInfo( struct StructPlayer *pPlayer );
void SendGuildDungeonRaidSiegeTip( struct StructPlayer *pPlayer );

// 파티원에게 소환수 정보 방송
void SendSummonInfo( struct StructPlayer *pPlayer );
void BroadcastSummonInfo( int nPartyID, struct StructPlayer *pPlayer, bool bForce = false );

void SendShowCreatureNameChangeBox( struct StructPlayer *pPlayer, AR_HANDLE hSummon );

// Quest
void SendQuestList( struct StructPlayer *pPlayer );
void SendQuestStatus( struct StructPlayer *pPlayer, struct StructQuest* pQuest );

// Title
void SendTitleList( struct StructPlayer *pPlayer );
void SendTitleConditionList( struct StructPlayer *pPlayer );
void SendRemainTitleTime( struct StructPlayer *pPlayer );
void SendMainTitle( struct StructPlayer *pPlayer );
void SendSubTitle( struct StructPlayer *pPlayer );

// 기타 유틸
void SendNPCStatusInVisibleRange( struct StructPlayer* pPlayer );
void SendStatusMessage( struct StructPlayer * pClient, struct StructCreature* pCreature );
void BroadcastStatusMessage( struct StructCreature* pCreature );
void BroadcastHairInfo( struct StructPlayer * pPlayer );
void BroadcastSkinInfo( struct StructPlayer * pPlayer );
unsigned int GetStatusCode( struct StructCreature* pCreature, const struct StructPlayer * pClient );

// 노점
void SendBoothClosedMessage( struct StructPlayer* pPlayer, AR_HANDLE target );
void SendBoothInfo( struct StructPlayer* pPlayer, StructPlayer* pTarget );		// pTarget이 노점 주인임.

// 경매 정보 송신용 구조체에 값 대입
void fillAuctionMessageBuffer( struct TS_AUCTION_INFO *pBuffer, const class AuctionInfo *pAuctionInfo );

// 인증과 통신
void SendGameServerLoginToAuth( const char * szServerName, const char * szServerScreenshotUrl, unsigned short nServerIdx, const char * szServerIP );
bool SendLoginToAuth( const char * szAccount, __int64 one_time_key );
void SendLogoutToAuth( const char * szAccount, AR_TIME nContinuousPlayTime, const int nCallerIdx );

// 업로드 서버와 통신
void SendGameServerLoginToUpload( const char *szServerName );

// Url 툴
void SendOpenUrl( struct StructPlayer * pPlayer, const char *szUrl, const bool bWaitForEventScene, const int width = 0, const int height = 0 );
void SendUrlList( struct StructPlayer * pPlayer );

// 친구, 차단 관련
void SendFriendsList( struct StructPlayer *pClient );
void SendDenialsList( struct StructPlayer *pClient );
void SendStatusMessageToFriendOfPlayer( struct StructPlayer *pClient, bool bIsLogin );
void SendStatusMessageToFriendOfPlayer( struct StructPlayer *pClient, struct StructPlayer *pTarget, bool bIsLogin );

// 헌터홀릭 관련
void SendHuntaholicInstanceInfo( const struct StructPlayer * pPlayer, const TS_HUNTAHOLIC_INSTANCE_INFO & info );
void SendHuntaholicHuntingScore( const struct StructPlayer * pPlayer, const int nHuntaholicID, const int nPersonalKillCount, const int nPersonalScore, const int nKillCount, const int nScore, const double & fPointAdvantage, const int nGainPoint, const char nResultType );

// 대련 관련
void SendCompeteRequest( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const char * szRequester );
void SendCompeteAnswer( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const unsigned char nAnswerType, const char * szRequestee );
void SendCompeteCountdown( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const char * szCompetitor, const AR_HANDLE hCompetitor );
void SendCompeteStart( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const char * szCompetitor );
// TS_SC_COMPETE_END 메시지는 동일한 내용이 여러 유저에게 보내져야 하므로 각 부분에서 직접 처리하는 것이 효율적임
void SendCompeteEnd( const struct StructPlayer * pPlayer, const unsigned char nCompeteType, const unsigned char nEndType, const char * szWinner, const char * szLoser );
