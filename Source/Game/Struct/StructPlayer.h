#pragma once

#include <vector>
#include <string>
#include <ctime>

#include <mmo/ArObject.h>
#include <toolkit/TimeSyncer.h>
#include <toolkit/KHash.h>
#include <toolkit/ILock.h>
#include <toolkit/XPropertySet.h>

#include "StructCreature.h"
#include "ItemInstance.h"
#include "StructInventory.h"
#include "StructItem.h"
#include "StructGold.h"
#include "StructQuestManager.h"
#include "GameMessage.h"
#include "SendMessage.h"
#include "GameDBManager.h"
#include "PlayerData.h"
#include "UID.h"
#include "StructTitleManager.h"
#include "StructTitle.h"

#include "..\Community\PartyMatching.h"


struct PlayerDeleter : public ArSchedulerObject
{
	virtual void onProcess( int nThreadIdx );
};

struct StructPlayer : public StructCreature, XPropertySet, StructInventory::InventoryEventReceiver, StructQuest::QuestEventHandler, StructTitle::TitleEventHandler
{
	ITERATOR_DEFINITION( StructPlayer );

	static iterator get( AR_HANDLE handle )
	{	
		iterator It;
		GameObject *p = *GameObject::get( handle );
		if( p ) 
		{
			if( p->IsPlayer() ) It.set( p );
		}
		return It;
	}

	virtual bool					IsSkillCastable()
	{
		if( IsRiding() || HasRidingState() ) return false;

		return StructCreature::IsSkillCastable();
	}
	virtual bool					IsMagicCastable()
	{
		if( IsRiding() || HasRidingState() ) return false;

		return StructCreature::IsMagicCastable();
	}

	// 시스템 
	static void						EraseReturnLobbyConnection( int conn_id );
	int								SetReturnLobbyConnection( class XIOCPConnection * pConn );
	static void						EraseFromAutoAccount( int account_id );
	static void						AddToAutoAccountList( int account_id );
	static void						ClearAutoAccountList();
	static bool						IsAutoAccount( int account_id );
	static void						BindProperty();
	static StructPlayer*			AllocPlayer();
	static AR_HANDLE				FindPlayer( const char *szName );
	static void						EraseFromPlayerList( const char *szName );
	static bool						RegisterAccount( struct IStreamSocketConnection * pConn, const char *szName );
	static void						UnRegisterAccount( const char *szName );
	static IStreamSocketConnection *		GetConnectionByAccount( const char * szAccount );
	static const bool				IsRegisteredAccount( const char * szAccount );
	static const bool				SetRegisteredConnectionByAccount( const char *szAccount, struct IStreamSocketConnection * pConn );
	static void						DoEachPlayer( ArObjectFunctor & _fo );
	void							DoEachPartyPlayer( ArObjectFunctor & _fo );
	void							DoEachGuildPlayer( ArObjectFunctor & _fo );
	static void						DoPlayer( AR_HANDLE handle, ArObjectFunctor & _fo );
	static volatile LONG &			GetPlayerCount();

	// 플레이어 이름 - UID 저장용 캐쉬(타 매니저에서 PlayerUID만 가지고있고 플레이어 이름을 중복해서 보관하지 않고 아래의 함수를 통해 얻어서 사용하도록 함)
	static const char *				GetPlayerName( const PlayerUID nPlayerUID );
	static void						RegisterPlayerName( const PlayerUID nPlayerUID, const char * pszName );
	static void						ClearPlayerName();

	// UID 할당용
	static ItemUID	GetMaxItemUID();
	static void						SetMaxItemUID( ItemUID id );	

	static void						SetMaxSummonSID( int sid );
	static void						SetMaxFarmSID( int sid );
	static int						AllocSummonSID();
	static int						AllocFarmSID();

	static void						SetMaxPetSID( int sid );
	static int						AllocPetSID();

	bool							IsPlayer() const	{ return true; }
	AR_HANDLE						GetHandle() const	{ return m_hHandle;	}
	PlayerUID						GetPlayerUID() const		{ return m_nUID;	}
	int								GetAccountID() const;
	const char *					GetAccountName() const;
	virtual int						GetSID() const		{ return GetPlayerUID(); }
	void							Save( bool bOnlyCharacter = false );
	virtual void					OnUpdate();

	AR_HANDLE						GetTarget() const	{ return m_hTarget; }
	void							SetTarget( AR_HANDLE target );

	struct SKILL_FUNCTOR			{ virtual void onSkill( const StructSkill* pSkill ) {} };
	void							EnumSummonPassiveSkill( SKILL_FUNCTOR & _fo ) const;
	void							EnumSummonAmplifyPassiveSkill( SKILL_FUNCTOR & _fo ) const;

	bool							CanLogoutNow();

	// 스탯	
	virtual const CreatureStat &	GetBaseStat() const;
	int						GetPermission() const	{ return m_nPermission; }
	bool					IsChatBlock() const		{ return GetArTime() / 100 < (unsigned) m_nChatBlockTime; }
	int						GetChatBlockTime() const	{ return std::max( m_nChatBlockTime - (int)( GetArTime() / 100 ), 0 ); }
	int						GetAdvChatCount() const		{ return m_nAdvChatCount; }
	void					SetChatBlockTime( int nBlockTime ) { m_nChatBlockTime = GetArTime() / 100 + nBlockTime; }
	virtual const char*		GetName() const										{ return m_szName; }
	const char *			GetAlias( const bool bRawAlias = false ) const		{ return ( bRawAlias || ( m_bUseAlias && strlen( m_szAlias ) > 0 ) ) ? m_szAlias : m_szName; }
	virtual int				GetCreatureGroup() const							{ return ( m_nChangingGroup == CREATURE_NONE ) ? CREATURE_HUMAN : m_nChangingGroup; }

	bool					ChangeName( const char * szNewName, const bool bRemoveFromFriendAndDenial, const bool bIgnoreNameChangeCount, const AR_HANDLE hItem );
	void					SetName( const char *szName );
	void					OnChangeName( const char * szName, const bool bRemoveFromFriendAndDenial, const bool bIgnoreNameChangeCount, const bool bSuccess );
	void					SetAlias( const char * szNewAlias )					{ s_strcpy( m_szAlias, _countof( m_szAlias ), szNewAlias ); if( strlen( szNewAlias ) > 0 ) s_strcat( m_szAlias, _countof( m_szAlias ), "*" ); SendPropertyMessage( this, GetHandle(), "alias", m_szAlias ); }
	void					SetUseAlias( bool bUseAlias )						{ m_bUseAlias = bUseAlias; }
	bool					IsUsingAlias() const								{ return m_bUseAlias; }
	virtual unsigned char	GetRace() const				{ return m_nRace; }
	int						GetSex() const				{ return m_nSex; }
	void					SetCreatureSpeed( unsigned char speed );
	void					SetWalk( bool bOn )			{ m_bWalk = bOn; }
	bool					IsWalking() const			{ return m_bWalk; }
	virtual void			AddExp( __int64 exp, __int64 jp, bool bApplyStamina = true );
	__int64					GetDeadEXPPenalty();
	
	// 직업군
	virtual bool			IsHunter() const;
	virtual bool			IsFighter() const;
	virtual bool			IsMagician() const;
	virtual bool			IsSummoner() const;

	bool					IsChangeableJob( const int nTargetJob ) const;
	bool					ChangeJob( const _SKILL_RESET_METHOD eMethod, const int nTargetJob );
	bool					ResetJob( const int nTargetJobDepth );
	bool					SetRace( const _SKILL_RESET_METHOD eMethod, const unsigned char nTargetRace );

	// 특성
	virtual int				GetTalentPoint() const	{ return m_nTalentPoint;	}
	virtual void			SetTalentPoint( int nTalentPoint )	{	m_nTalentPoint = nTalentPoint;	SendPropertyMessage( this, GetHandle(), "tp", GetTalentPoint() );	}

	int						GetCharisma() const		{ return m_nCharisma;		}
	
	virtual int				GetMoveSpeed() const;

	// 혼돈 관련
	int						GetMaxChaos() const		{ return m_nMaxChaos; }
	int						GetChaos() const		{ return  m_nChaos; }
	//void					SetChaos( int nChaos )		{ m_nChaos = nChaos <= m_nMaxChaos ? nChaos : m_nMaxChaos; }
	void					AddChaos( int nChaos );

	// 퀘스트 관련
	bool					StartQuest( QuestBase::QuestCode code, int nStartQuestID, bool bForce = false );
	const bool				AddPendingQuest( const QuestBase::QuestCode code, int nStartID );
	bool					EndQuest( QuestBase::QuestCode code, int nSelectRewardIdx = -1 );
	bool					DropQuest( QuestBase::QuestCode code );
	const bool				ResetFinishedQuest( QuestBase::QuestCode code );
	int						GetQuestProgress( QuestBase::QuestCode code ) const;	// -1 : 아무것도 아님      0 : 수락가능       1 : 수행중      2 : 종료가능       255 : 이미종료
	const int				GetQuestStartTextID( QuestBase::QuestCode code ) const;
	inline const size_t		DoEachActiveQuest( StructQuestManager::QuestFunctor & _fo )			{ return m_QuestManager.DoEachActiveQuest( _fo ); }
	inline const size_t		DoEachPendingQuest( StructQuestManager::QuestFunctor & _fo )		{ return m_QuestManager.DoEachPendingQuest( _fo ); }
	inline const size_t		GetActiveQuestCount()		{ return m_QuestManager.GetActiveQuestCount(); }
	int						GetLastAcceptQuest()		{ return m_nLastAcceptQuest; }

	const bool				IsStartableQuest( QuestBase::QuestCode code ) const;
	const bool				IsPendingQuest( const QuestBase::QuestCode code ) const			{ return m_QuestManager.IsPendingQuest( code ); }
	const bool				IsInProgressQuest( QuestBase::QuestCode code ) const;
	const bool				IsFinishableQuest( QuestBase::QuestCode code ) const;
	const bool				IsTakeableQuestItem( QuestBase::QuestCode code ) const;
	const time_t			GetRemainQuestCoolTime( QuestBase::QuestCode code, time_t tCurrent ) const	{ return m_QuestManager.GetRemainQuestCoolTime( code, tCurrent ); }

	const bool				CheckFinishableQuestAndGetQuestStruct( QuestBase::QuestCode code, StructQuest** pQuest ) const; 

	inline StructQuest *	FindQuest( QuestBase::QuestCode code ) const			{ return m_QuestManager.FindQuest( code ); }
	
	void					GetQuestByMonster( int nMonsterId, std::vector< StructQuest* > & vQuest, int type );
	void					UpdateQuestStatusByMonsterKill( int nMonsterId );

	// 호칭 관련
	inline const TitleBaseServer *GetMainTitle()				{ return m_pMainTitle; }
	inline const TitleBaseServer *GetSubTitle( int nIndex )		{ return m_pSubTitle[ nIndex ]; }
	inline const AR_TIME	GetRemainTitleTime()				{ return m_tRemainTitleTime; }

	bool					IsUsableTitle( int nCode );
	bool					SetMainTitle( int nCode );
	bool					SetSubTitle( int nIndex, int nCode );
	bool					OpenTitle( int nCode );
	bool					AchieveTitle( int nCode );
	bool					BookmarkTitle( int nCode );
	void					SetRemainTitleTime( int nRemainTitleTime )	{ m_tRemainTitleTime = nRemainTitleTime; }

	void					UpdateTitleCondition( const int nTitleCondition, const __int64 nCount )												{ m_TitleManager.UpdateTitleCondition( nTitleCondition, nCount ); }
	void					UpdateTitleConditionByMonsterKill( const int nMonsterID, const int nTamingCode )									{ m_TitleManager.UpdateTitleConditionByMonsterKill( nMonsterID, nTamingCode ); }
	void					UpdateTitleConditionByItemGet( ItemBase::ItemCode nItemCode, const __int64 & nCount )								{ m_TitleManager.UpdateTitleConditionByItemGet( nItemCode, nCount ); }
	void					UpdateTitleConditionByItemEquip( ItemBase::ItemCode nItemCode, const bool bEquip )									{ m_TitleManager.UpdateTitleConditionByItemEquip( nItemCode, bEquip ); }
	void					UpdateTitleConditionByItemUse( ItemBase::ItemCode nItemCode )														{ m_TitleManager.UpdateTitleConditionByItemUse( nItemCode ); }
	void					UpdateTitleConditionByItemCreateByMixing( std::vector< std::pair< StructItem *, __int64 > > & vCreatedItem )		{ m_TitleManager.UpdateTitleConditionByItemCreateByMixing( vCreatedItem ); }
	void					UpdateTitleConditionBySummonEnhance( const int nTamingCode, const char nRate, const int nEnhance )					{ m_TitleManager.UpdateTitleConditionBySummonEnhance( nTamingCode, nRate, nEnhance ); }
	void					UpdateTitleConditionBySummonTame( const int nTamingCode, const char nRate, const bool bSuccess )					{ m_TitleManager.UpdateTitleConditionBySummonTame( nTamingCode, nRate, bSuccess ); }
	void					UpdateTitleConditionBySummonEquip( const int nTamingCode, const char nRate, const int nEnhance, const int nInc )	{ m_TitleManager.UpdateTitleConditionBySummonEquip( nTamingCode, nRate, nEnhance, nInc ); }
	void					UpdateTitleConditionBySummonMount( const int nTamingCode, const bool bMount )										{ m_TitleManager.UpdateTitleConditionBySummonMount( nTamingCode, bMount ); }
	void					UpdateTitleConditionBySummonPutOnBelt( std::vector< int > & vBeltCode )												{ m_TitleManager.UpdateTitleConditionBySummonPutOnBelt( vBeltCode ); }
	void					UpdateTitleConditionByPKOn( const bool bPKOn )																		{ m_TitleManager.UpdateTitleConditionByPKOn( bPKOn ); }
	void					UpdateTitleConditionByDungeonSiegeStart( const int nDungeonID, const bool bIsAttackTeam )							{ m_TitleManager.UpdateTitleConditionByDungeonSiegeStart( nDungeonID, bIsAttackTeam ); }
	void					UpdateTitleConditionByDungeonSiegeEnd( const int nDungeonID, const bool bIsAttackTeam, const bool bResult )			{ m_TitleManager.UpdateTitleConditionByDungeonSiegeEnd( nDungeonID, bIsAttackTeam, bResult ); }
	void					UpdateTitleConditionByGold()																						{ m_TitleManager.UpdateTitleConditionByGold(m_nGold); }

	inline const bool		IsAchievedTitle( int nCode ){ return m_TitleManager.IsAchievedTitle( nCode ); }
	inline const size_t		GetTitleCount()				{ return m_TitleManager.GetTitleCount(); }
	inline const size_t		GetTitleConditionCount()	{ return m_TitleManager.GetTitleConditionCount(); }
	inline const size_t		DoEachTitle( StructTitleManager::TitleFunctor & _fo )	{ return m_TitleManager.DoEachTitle( _fo ); }
	inline const size_t		DoEachTitleCondition( StructTitleManager::TitleConditionFunctor & _fo )	{ return m_TitleManager.DoEachTitleCondition( _fo ); }

	// 아이템/재화 관련
	const StructGold		GetGold() const					{ return m_nGold;		}
	const StructGold		GetStorageGold() const			{ return m_nStorageGold;		}
	const unsigned short	ChangeStorageGold( const StructGold & gold );
	const unsigned short	ChangeGold( const StructGold & gold );

	inline const int			GetBaseModelId( int nIdx ) const						{ return m_nBaseModelId[nIdx]; }
	inline void					SetBaseModelId( int nIdx, int nModelId )				{ m_nBaseModelId[nIdx] = nModelId; }
	inline const int			GetFaceTextureId() const								{ return m_nFaceTextureId; }
	inline const int			GetHairColorIndex() const								{ return m_nHairColorIndex; }
	inline void					SetHairColorIndex( const int nHairColorIndex )			{ m_nHairColorIndex = nHairColorIndex; }
	inline const unsigned int	GetHairColorRGB() const									{ return m_nHairColorRGB; }
	inline void					SetHairColorRGB( const unsigned int nHairColorRGB )		{ m_nHairColorRGB = nHairColorRGB; }

	inline const unsigned int	GetHideEquipFlag() const								{ return m_nHideEquipFlag; }
	inline void					SetHideEquipFlag( const unsigned int nHideEquipFlag )	{ m_nHideEquipFlag = nHideEquipFlag; }

	const char *				GetClientInfo() const									{ return m_strClientInfo.c_str(); }
	void						SetClientInfo( const char* pValue )						{ m_strClientInfo = pValue; }
	const char *				GetQuickSlot() const									{ return m_strQuickSlot.c_str(); }
	void						SetQuickSlot( const char* pValue )						{ m_strQuickSlot = pValue; }
	const char *				GetCurrentKey() const									{ return m_strCurrentKey.c_str(); }
	void						SetCurrentKey( const char* pValue )						{ m_strCurrentKey = pValue; }
	const char *				GetSavedKey() const										{ return m_strSavedKey.c_str(); }
	void						SetSavedKey( const char* pValue )						{ m_strSavedKey = pValue; }

	virtual bool			IsUsingBow() const			{ return ( m_anWear[ ItemBase::WEAR_WEAPON ] ? m_anWear[ ItemBase::WEAR_WEAPON ]->IsBow() : false ); }
	virtual bool			IsUsingCrossBow() const		{ return ( m_anWear[ ItemBase::WEAR_WEAPON ] ? m_anWear[ ItemBase::WEAR_WEAPON ]->IsCrossBow() : false ); }
	virtual bool			TranslateWearPosition( ItemBase::ItemWearType & pos, struct StructItem* pItem, std::vector< int > * vpOverlappItemList = NULL );
	//bool					IsPopable( struct StructItem* pItem, unsigned count = 1 );

	// 에테리얼 내구도 관련
	virtual void			ProcEtherealDurabilityConsumption( const bool bIsAttacker, const DamageType eDamageType, const int nDamage );

	// 헌터홀릭 관련
	const int				GetHuntaholicPoint() const				{ return m_nHuntaholicPoint; }
	// SetHuntaholicPoint는 HuntaholicManager에서 HuntaholicInfo::csInstanceDungeon, InstanceDungeon::csInstance 걸고 호출해야 함
	void					SetHuntaholicPoint( const int nPoint )	{ m_nHuntaholicPoint = std::min( std::max( nPoint, 0 ), GameRule::HUNTAHOLIC_MAX_OWNABLE_POINT ); SendPropertyMessage( this, GetHandle(), "huntaholicpoint", GetHuntaholicPoint() ); }
	const int				GetHuntaholicEnterableCount() const		{ return m_nHuntaholicEnterableCount; }
	void					SetHuntaholicEnterableCount( const int nEnterableCount );
	void					AddHuntaholicEnterableCount( const int nEnterableCountInc );
	const time_t			GetNextHuntaholicEnterableCountRefill() const	{ return m_tNextHuntaholicEnterableCountRefill;	}

	// 충전된 에테리얼 스톤 내구도
	const int				GetEtherealStoneDurability() const								{ return m_nEtherealStoneDurability; }
	void					SetEtherealStoneDurability( const int nDurability )				{ m_nEtherealStoneDurability = std::min( std::max( nDurability, 0 ), GameRule::MAX_ETHEREAL_STONE_DURABILITY ); SendPropertyMessage( this, GetHandle(), "ethereal_stone", GetEtherealStoneDurability() ); }
	void					AddEtherealStoneDurability( const int nAdd )					{ SetEtherealStoneDurability( GetEtherealStoneDurability() + nAdd ); }

	// 아이템 관련
	bool					IsMixable( StructItem * pItem );
	bool					IsErasable( StructItem * pItem );
	bool					IsDecomposable( StructItem * pItem );
	bool					IsRandomizable( StructItem * pItem );
	bool					IsSellable( StructItem * pItem );
	bool					IsTradable( StructItem * pItem );
	bool					IsDropable( StructItem * pItem );

	bool					IsTakeable( AR_HANDLE ItemHandle, const __int64 & cnt = 0 );
	struct StructItem*		PushItem( struct StructItem *pItem, const __int64 & cnt, bool bSkipUpdateItemToDB = false );
	struct StructItem*		PopItem( struct StructItem *pItem, const __int64 & cnt, bool bSkipUpdateItemToDB = false );
	struct StructItem*		DropItem( struct StructItem *pItem, const __int64 & count = 1 );
	void					ChangeItemPosition( bool is_storage, AR_HANDLE hItem_01, AR_HANDLE hItem_02 );
	unsigned short			ArrangeItem( const bool bIsStorage );
	struct StructItem*		onAfterChangeItemProperty( struct StructItem *pItem );

	bool					EraseBullet( const __int64 & count );
#define EraseItem( pItem, count ) EraseItem_( pItem, count, __FILE__, __LINE__ )
	bool					EraseItem_( struct StructItem *pItem, const __int64 & count = 1, const char* function = NULL, const int line = 0 );
	bool					EraseItemFromStorage( struct StructItem *pItem, const __int64 & count );
	bool					GiveItem( struct StructPlayer *pTarget, AR_HANDLE ItemHandle, const __int64 & count, AR_HANDLE * pResultItemHandle = NULL );
	const size_t			GetItemCount();		// 아이템의 수량이 아닌 아이템의 종류 수(인벤토리 차지 칸 수)이므로 size_t 사용
	const size_t			GetStorageItemCount();
	const __int64			GetItemCount( ItemBase::ItemCode code );
	struct StructItem*		GetItem( unsigned idx );
	struct StructItem*		GetStorageItem( unsigned idx );
	struct StructItem*		FindItem( ItemUID uid ) const;	// 이것 느림. 가급적 AR_HANDLE 로 찾을것.
	struct StructItem*		FindItem( ItemBase::ItemCode code ) const;
	void					FindItem( ItemBase::ItemCode code, std::vector< StructItem * > & vList );
	struct StructItem*		FindStorageItem( ItemBase::ItemCode code ) const;
	struct StructItem*		FindEmptySummonCard( ItemBase::ItemCode code ) const;
	struct StructItem*		FindEmptyPetCage( ItemBase::ItemCode code ) const;
	struct StructItem*		FindTamingSummonCard( ItemBase::ItemCode code ) const;
	struct StructItem*		FindFarmedSummonCard( ItemUID uid ) const;

	unsigned short			UseItem( struct StructItem *pItem, struct StructCreature *pTarget = NULL, const char *szParameter = NULL );
	int						IsUseableItem( struct StructItem *pItem, struct StructCreature * pTarget );
	AR_TIME					GetItemCoolTime( int group_id );

	void					CloseStorage();
	bool					IsLoadingStorage() const	{ return m_nIsUsingStorage == LOADING_STORAGE; }
	bool					IsUsingStorage() const		{ return m_nIsUsingStorage == OPEN_STORAGE; }
	bool					MoveStorageToInventory( struct StructItem *pItem, const __int64 & cnt );
	bool					MoveInventoryToStorage( struct StructItem *pItem, const __int64 & cnt );

	StructInventory *		GetInventory()				{ return &m_Inventory; }
	void					ClearInventory();

	inline void				UpdateWeightWithInventory()	{ m_fWeight = (int)m_Inventory.GetWeight(); }
	
	// 소환수 관련
	void					EquipSummon( AR_HANDLE* pCardHandle );
	void					AddSummon( struct StructSummon* pSummon, bool bSkipUpdateItemToDB = false, bool bSendMsg = true );
	void					RemoveSummon( struct StructSummon* pSummon, bool bSkipUpdateItemToDB = false );
	void					AddSummonToStorage( struct StructSummon* pSummon, bool bSkipUpdateItemToDB );
	void					RemoveSummonFromStorage( struct StructSummon* pSummon, bool bSkipUpdateItemToDB );
	void					RemoveAllSummonFromWorld();
	void					RemoveAllPetFromWorld();
	struct StructSummon*	GetSummon( int summon_sid );
	struct StructSummon*	GetSummon( AR_HANDLE handle );
	struct StructSummon*	GetSummonAt( int slot_index ) const		{ return ( m_aBindSummonCard[slot_index] ? m_aBindSummonCard[slot_index]->GetSummonStruct() : NULL ); }
	struct StructItem*		GetSummonCardAt( int slot_index ) const	{ return m_aBindSummonCard[slot_index]; }
	void					SetSummonCardAt( int slot_index, struct StructItem *pPtr ) { m_aBindSummonCard[slot_index] = pPtr; }
	bool					Summon( struct StructSummon * pSummon, const ArPosition & pos );
	bool					UnSummon( struct StructSummon * pSummon = NULL );
	bool					PendUnSummon( struct StructSummon * pSummon = NULL );
	int						GetPendingUnSummon() const				{ return m_nPendingUnSummon; }
	struct StructSummon*	GetMainSummon() const	{ return m_pMainSummon; }
	struct StructSummon*	GetSubSummon() const	{ return m_pSubSummon; }
	void					SetMainAndSubSummon();
	bool					IsSummonable() const	{ return m_bIsSummonable; }
	int						GetNextUnSummonTime() { return m_nNextUnSummonTime; }
	struct StructItem*		GetBeltSlotCardAt( int belt_slot_idx ) const { return m_aBeltSlotCard[belt_slot_idx]; }
	struct StructItem*		GetEquipmentOnBelt() const;
	void					SetNextUnSummonTime( int nNextUnSummonTime ) { m_nNextUnSummonTime = nNextUnSummonTime; }
	void					SetNameChangeTarget( const int uid )				{ m_nNameChangeTarget = uid; }
	int						GetNameChangeTarget() const							{ return m_nNameChangeTarget; }

	float					GetActiveSummonExpAmp() const		{ return m_fActiveSummonExpAmp;	}
	void					SetActiveSummonExpAmp( float fAmp )	{ m_fActiveSummonExpAmp = fAmp;	}

	bool					GetAlwaysPassMix() const { return GetPermission() >= GameRule::PERMISSION_FOR_GM && GameRule::bGMsNeverFail; }

	// 소환수 농장 관련
	struct FARMED_SUMMON_INFO
	{
		FARMED_SUMMON_INFO( const int _farm_sid, struct StructItem *_item, const int _max_level, const bool _is_using_cracker, const bool _is_cash, const time_t _registration_time, const int _duration, const time_t _nursing_time )
			: farm_sid( _farm_sid )
			, item( _item )
			, max_level( _max_level )
			, is_using_cracker( _is_using_cracker )
			, is_cash( _is_cash )
			, registration_time( _registration_time )
			, duration( _duration )
			, nursing_time( _nursing_time )
		{}

		int					farm_sid;
		struct StructItem *	item;
		int					max_level;
		bool				is_using_cracker;
		bool				is_cash;
		time_t				registration_time;
		int					duration;
		time_t				nursing_time;
	};

	bool					FarmSummon( int nSlot, AR_HANDLE hCardHandle, int nDuration, bool bUseCracker, bool bCash );
	bool					RegainSummon( AR_HANDLE hHandle );
	bool					NurseSummon( AR_HANDLE hHandle );
	void					SetFarmedSummonInfo( int nSlot, FARMED_SUMMON_INFO *pFarmedSummonInfo ) { assert( ( nSlot >= 0 && nSlot < GameRule::FARM_MAX_COUNT ) && ( !m_vFarmedSummonInfo[nSlot] || pFarmedSummonInfo == NULL ) ); m_vFarmedSummonInfo[nSlot] = pFarmedSummonInfo; }
	FARMED_SUMMON_INFO *	GetFarmedSummonInfo( int nSlot ) { assert( nSlot >= 0 && nSlot < GameRule::FARM_MAX_COUNT ); return m_vFarmedSummonInfo[nSlot]; }
	size_t					GetFarmedSummonCount();
	size_t					GetNonCashFarmedSummonCount();
	int						GetFarmedSummonLevel( AR_HANDLE hHandle );

	// 펫 관련
	void					AddPet( struct StructPet* pPet, bool bSkipUpdateItemToDB, bool bSendMsg = true );
	void					RemovePet( struct StructPet* pPet, bool bSkipUpdateItemToDB );
	void					AddPetToStorage( struct StructPet* pPet, bool bSkipUpdateItemToDB );
	void					RemovePetFromStorage( struct StructPet* pPet, bool bSkipUpdateItemToDB );
	unsigned short			SummonPet( struct StructPet* pPet );
	bool					UnSummonPet();								// 펫의 역소환은 소환수같이 즉시 하지 않고 전부 벡터에 보관했다가 StructPlayer:onProcess에서 함(락 문제 발생 소지 즐 -_ -)
	struct StructPet*		GetSummonedPet() const				{ return m_pSummonedPet; }
	struct StructPet*		GetPet( int nPet_sid );
	bool					ProcGlobalChatProcess( const char * pChat, int nChatType );

	// 로그인 / 로그아웃 처리
	void			Login();
	void			LogoutNow( const int nCallerIdx );
	void			LogoutNowWithAccount( const int nCallerIdx );

	void			OnConnectionClosed( struct _CONNECTION_TAG * pTag );
	
	bool			IsLogin() const				{	return m_bIsLogin;	}

	void			SetLastLogoutRequestedTime( const AR_TIME & t )		{ m_nLastLogoutRequestedTime = t; }
	const AR_TIME	GetLastLogoutRequestedTime()						{ return m_nLastLogoutRequestedTime; }

	// 파티 관련
	void			SetPartyID( int nPartyID )			{ m_nPartyID = nPartyID; }
	int				GetPartyID() const					{ return m_nPartyID; }
	bool			IsInParty() const					{ return m_nPartyID != 0; }
	bool			HasItemPriority() const				{ return m_bItemPriority; }
	void			SetItemPriority()					{ m_bItemPriority = true; }
	void			ReleaseItemPriority()				{ m_bItemPriority = false; }
	bool			IsPartyInvitable( StructPlayer * pTarget) const;

	// 길드 관련
	void			SetGuildID( int nGuildId )							{ m_nGuildId = nGuildId; }
	int				GetGuildID() const									{ return m_nGuildId; }
	void			SetPrevGuildID( int nPrevGuildId )					{ m_nPrevGuildId = nPrevGuildId; }
	int				GetPrevGuildID() const								{ return ( m_tGuildBlockTime > time( NULL ) ) ? m_nPrevGuildId : 0; }
	bool			IsInGuild() const									{ return ( m_nGuildId > 0 ); }
	bool			IsGuildInvitable( StructPlayer * pTarget ) const;
	time_t			GetGuildBlockTime()									{ return ( m_tGuildBlockTime > time( NULL ) ) ? m_tGuildBlockTime : 0; }
	void			SetGuildBlockTime( time_t guild_block_time )		{ m_tGuildBlockTime = guild_block_time; }
	const char		GetGuildPermission() const							{ return m_nGuildPermission; }
	void			SetGuildPermission( const char nPermission )		{ m_nGuildPermission = nPermission; }

	// 메신저 관련
	void			AddFriend( const char * szName );
	bool			addFriend( const char * szName );
	void			AddFriendOf( const char * szName );
	void			AddDenial( const char * szName );
	bool			addDenial( const char * szName );
	void			AddDenialOf( const char * szName );
	void			DelFriend( const char * szName );
	bool			delFriend( const char * szName );
	void			DelFriendOf( const char * szName );
	void			DelDenial( const char * szName );
	bool			delDenial( const char * szName );
	void			DelDenialOf( const char * szName );
	bool			IsFriend( const char * szName );
	bool			IsFriendOf( const char * szName );
	bool			IsDenial( const char * szName );
	bool			IsDenialOf( const char * szName );

	const size_t	GetFriendCount() const			{ return m_vFriend.size(); }
	const size_t	GetFriendOfCount() const		{ return m_vFriendOf.size(); }
	const size_t	GetDenialCount() const			{ return m_vDenial.size(); }
	const size_t	GetDenialOfCount() const		{ return m_vDenialOf.size(); }

	struct FriendDenialFunctor
	{
		virtual const bool operator()( std::string & strName )		= 0;
	};
	// 별로 이러고 싶진 않지만 락도 안 걸고 m_vDenial을 돌아주는 함수의 인터페이스를 외부에 노출하지 않으면 해결이 안 됨 -_ -;
	friend void SendFriendsList( struct StructPlayer *pClient );
	friend void SendStatusMessageToFriendOfPlayer( struct StructPlayer *pClient, bool bIsLogin );

	const size_t	DoEachFriend( FriendDenialFunctor & fo );
	const size_t	DoEachFriendOf( FriendDenialFunctor & fo );
	const size_t	DoEachDenial( FriendDenialFunctor & fo );
	const size_t	DoEachDenialOf( FriendDenialFunctor & fo );

	// 플레이어 변수 ( named flag 라고 보면 됨 )
	std::string		GetFlag( const char *szKey ) const;
	const bool		RemoveFlag( const char *szKey );
	const bool		IsExistFlag( const char *szKey ) const;
	void			SetFlag( const char *szKey, const char *szData );
	void			EnumFlag( std::string & strValueList );

	// 플레이어 어카운트 변수
	std::string		GetAccountFlag( const char *szKey ) const;
	void			SetAccountFlag( const char *szKey, const char *szData );
	const bool		RemoveAccountFlag( const char* szKey );
	void			EnumAccountFlag( std::string & strValueList );

	// 노점 관련
	enum BOOTH_STATUS
	{
		IS_NOT_BOOTH	= 0,
		SELL_BOOTH		= 1,
		BUY_BOOTH		= 2,
	};

	enum STORAGE_STATUS
	{
		CLOSE_STORAGE	= 0,
		LOADING_STORAGE	= 1,
		OPEN_STORAGE	= 2,
	};

	struct			BOOTH_OPEN_ITEM_INFO
	{
		AR_HANDLE	handle;
		int			cnt;
		__int64		gold;
	};

	struct			BOOTH_ITEM_INFO
	{
		StructItem* pItem;
		int			cnt;
		__int64		gold;
	};

	struct			BOOTH_ITEM_BUY_INFO
	{
		int			nIdx;
		int			nCount;
	};

	bool			OpenBooth( const char * szBoothName, const std::vector< StructPlayer::BOOTH_OPEN_ITEM_INFO > & vItemList, bool IsBuyBooth );
	bool			CloseBooth();
	BOOTH_STATUS	GetBoothStatus()			{ return m_BoothStatus; }
	const char *	GetBoothName();
	const std::vector< StructPlayer::BOOTH_ITEM_INFO > & GetBoothItemList();

	void			WatchBooth( StructPlayer * pBooth );
	void			StopWatchBooth();
	bool			IsBoothWatching() const		{ return ( m_pBoothOpener != NULL ); }


	bool			BuyFromBooth( StructPlayer * pBuyer, std::vector<BOOTH_ITEM_BUY_INFO> & vBuyInfo );
	bool			SellToBooth( StructPlayer * pSeller, AR_HANDLE hItem, int nIdx, const __int64 & nCnt );
	static XCriticalSection*	GetBoothLock();

	bool			IsBoothOpen()
	{
		return GetBoothStatus() != IS_NOT_BOOTH;
	}

	virtual bool IsActable()
	{
		if( IsBoothOpen() ) return false;

		return StructCreature::IsActable();
	}

	virtual bool IsMovable()
	{
		if( IsBoothOpen() ) return false;

		return StructCreature::IsMovable();
	}

	virtual bool IsAttackable()
	{
		if( IsRiding() || HasRidingState() ) return false;
		
		return StructCreature::IsAttackable();
	}

	const unsigned short IsBoothOpenable();
	bool IsMountable( bool bByOnlyLocation )
	{
		if( !bByOnlyLocation )
		{
			if( !IsActable() || IsSitDown() || IsUsingSkill() || IsAttacking() || IsSitDown() )
				return false;
		}

		if( IsInDungeon() || IsInSecretDungeon() || IsInInstanceDungeon() || IsInBattleArena() || IsInRamadanPrayerHall() ) return false;

		return true;
	}

	bool IsItemPickable()
	{
		if( !IsActable() || IsSitDown() || IsUsingSkill() || IsAttacking() ) return false;
		return true;
	}

	bool IsSitdownable()
	{
		if( !IsActable() || IsSitDown() || IsUsingSkill() || IsRiding() || HasRidingState() ) return false;
		return true;
	}

	virtual bool IsItemUseable()
	{
		if( IsBoothOpen() ) return false;

		return StructCreature::IsItemUseable();
	}

	virtual bool IsItemWearable()
	{
		if( IsBoothOpen() ) return false;

		return StructCreature::IsItemWearable();
	}

	// 테이밍 관련
	AR_HANDLE		GetTamingTarget() const { return m_hTamingTarget; }
	void			SetTamingTarget( AR_HANDLE target ) { m_hTamingTarget = target; }

	// 앉기/서기
	void			SitDown()			{ m_bIsSitDown = true; }
	void			StandUp()			{ m_bIsSitDown = false; onStandUp(); }
	virtual bool	IsSitDown() const	{ return m_bIsSitDown; }

	// 모드
	void			SetBattleModeOn()		{ m_bIsBattleMode = true; }
	void			SetBattleModeOff()		{ m_bIsBattleMode = false; }
	virtual bool	IsBattleMode() const	{ return m_bIsBattleMode; }

	// 라이딩 관련
	// HasRidingState -> 아이템(탈것)으로 인한 라이딩
	// IsRiding -> 내 소환수(크리처)의 패시브 스킬에 의한 라이딩
	bool					HasRidingState()	{ return !!m_nRidingStateCode; }
	bool					IsRiding()	{ return m_nRideIdx != MOUNT_NOTHING; }
	inline bool				IsApplyingFasterSpeedInRiding() { return m_bUseFasterSpeedInRiding; }
	AR_HANDLE				GetRideHandle();
	struct StructSummon *	GetRideObject()  const;
	int						GetUnMountProbabilityOnDamage();
	int						GetUnMountProbabilityOnCriticalDamage();

	enum
	{
		UNMOUNT_NORMAL = 0,
		UNMOUNT_FALL = 1,
		UNMOUNT_UNSUMMON = 2,
	};
	void					MountSummon( AR_HANDLE handle );
	void					UnMount( const char flag = UNMOUNT_NORMAL, StructCreature * pCauser = NULL );


	bool			IsPendWarp() const		{ return m_nPendWarpX >= 0 && m_nPendWarpY >= 0; }
	void			ProcessWarp();

	enum CONDITION_INFO
	{
		CONDITION_GOOD		= 0,
		CONDITION_AVERAGE	= 1,
		CONDITION_BAD		= 2,
	};
	// 피로도 관련
	int				GetStamina() const		{ return m_nStamina; }
	void			AddStamina( int nStamina );
	int				GetStaminaRegenRate();
	CONDITION_INFO	GetCondition();
	int				GetLogoutDuration() const						{ return m_nLogoutDuration; }
	void			SetLogoutDuration( int nLogoutDuration )		{ m_nLogoutDuration = nLogoutDuration; }

	// 벨트 슬롯 관련
	bool			PutOnBelt( int pos, struct StructItem * pItem );
	void			PutOffBelt( int pos );		
	
	// 지역/날씨 정보 관련
	int				GetLocationId() const	{ return m_nWorldLocationId; }
	void			ChangeLocation( AR_UNIT x, AR_UNIT y, bool bByRequest = false, bool bBroadcast = true );
	const bool		IsInTown() const;
	const bool		IsInField() const;
	const bool		IsInBattleField() const;
	const bool		IsInEventmap() const;
	const bool		IsUsingTent() const	{ return m_bUsingTent; }
	const bool		IsInSiegeOrRaidDungeon() const;
	const bool		IsInSiegeDungeon() const;
	const bool		IsInDungeon() const;
	const bool		IsInSecretDungeon() const;
	const bool		IsInHuntaholic() const;
	const bool		IsInInstanceDungeon() const;
	const bool		IsInDeathmatch() const;
	const bool		IsInBattleArena() const;
	const bool		IsInRamadanPrayerHall() const;
	const bool		IsGaiaMember() const	{ return m_bIsGaiaMember; }
	AR_TIME			GetGaiaMemberRemainTime();
	void			SetGaiaMemberRemainTime( AR_TIME valid_time );
	void			SetSetSecrouteFreePass( bool bIsPremiumUser, int nRestSecond );
	void			SetAccountAuthority( int AuthorityType, int Duration );

	// PK 관련
	bool			IsPKOn()			{ return m_bIsPK; }
	void			SetPKOn()			{ m_bIsPK = true;	UpdateTitleConditionByPKOn( true );		}
	void			SetPKOff()			{ m_bIsPK = false;	UpdateTitleConditionByPKOn( false );	}
	bool			IsPKOning()			{ return !!m_nTurnOnPkModeTime; }
	bool			IsPKOffing()		{ return !!m_nTurnOffPkModeTime; }

	const c_fixed10 &	GetImmoralPoint() const		{ return m_fImmoralPoint; }
	void				SetImmoralPoint( const c_fixed10 & fIP );

	int				GetPKC()			{ return m_nPKC; }
	void			IncPKC();
	int				GetDKC()			{ return m_nDKC; }
	void			IncDKC();

	const bool		IsBloodyCharacter() const	{ return ( m_fImmoralPoint >= GameRule::MORAL_LIMIT ); }
	const bool		IsDemoniacCharacter() const	{ return ( m_fImmoralPoint >= GameRule::CRIME_LIMIT ); }
	const bool		IsDungeonOriginalOwner() const;
	const bool		IsDungeonOriginalSieger() const;
	bool			IsInSecroute() const;

	virtual bool	IsEnemy( StructCreature* pTarget, bool bIncludeHiding = false );
	virtual bool	IsAlly( StructCreature* pTarget );
	virtual bool	IsVisible( StructCreature *pTarget );

	const bool		IsInPKField() const;
	const bool		TurnOnPkMode( const bool bImmediateOn = false );
	const bool		TurnOffPkMode( const bool bImmediateOff = false );

	// 스킬 관련
	virtual int		GetAllSkillTP() const;
	virtual int		IsLearnableSkill( int nSkillID, int nSkillLevel, int *nSkillTreeID );

	void				UpdateQuestStatusByItemEnhance( ItemBase::ItemCode code = 0 )
	{
		// 그닥 아름다운 방식은 아니지만 어차피 이벤트 없이는 그게 그거
		// 당장 조금 예쁜 코드 짜기보다는 나중에 리팩토링할 생각을 합시다
		m_QuestManager.UpdateQuestStatusByItemEnhance( &m_Inventory, code );
	}

	// 스크립트 관련
	void				UpdateQuestStatusByItemUpgrade();
	void				GetLastTownPosition( ArPosition * pPos ) const;
	void				StoreCurrentStatesOnEnterInstanceGame( bool bIncludeHPMP );
	void				GetPositionOnEnterInstanceGame( ArPosition * pPos ) const;
	void				RestoreStatesOnLeaveInstanceGame( bool bIncludeHPMP );
	void				OpenStorage( bool bStart = true );
	void				SetContactNPCHandle( AR_HANDLE handle )		{ SetLastContact( "npc", handle ); }
	AR_HANDLE			GetContactNPCHandle() const					{ return GetLastContactLong( "npc" ); }
	void				SetNonNPCDialog( bool bNonNPCDialog )		{ m_bNonNPCDialog = bNonNPCDialog; }
	bool				IsNonNPCDialog()							{ return m_bNonNPCDialog; }
	void				SetDialogTitle( const char *szString, int type = 0 );
	std::string			GetDialogTitle() const { return m_strDialogTitle; }
	void				SetDialogText( const char *szString );
	void				ClearDialogMenu();
	void				SetSpecialDialogMenu( const char * szString );
	bool				IsSpecialDialogMenu( const char * szString );
	void				ClearSpecialDialogMenu();
	void				SetFixedDialogTrigger( const char *szString );
	bool				IsFixedDialogTrigger( const char * szTrigger );
	void				ClearFixedDialogTrigger();

	void				AddDialogMenu( const char *szString, const char *szTrigger );
	void				AddTrigger( const char *szTrigger );
	void				SetTrigger( const char *szTrigger );
	inline bool			HasDialog()			{ return ( !m_strDialogTitle.empty() || !m_strDialogText.empty() ); }
	const bool			SetupQuestDialog( const QuestBase::QuestCode nQuestCode, const int nTextID, const int nDialogType = 0 );
	void				ShowDialog();
	bool				IsValidTrigger( const char *szTrigger );
	void				SetLastContactMarket( const char *szMarketName )	{ SetLastContact( "market", szMarketName ); }
	std::string			GetLastContactMarket() const						{ return GetLastContactStr( "market" ); }	

	void					PendWarp( int x, int y, unsigned char layer, int nInstanceDungeonType = -1, int isForceWarp = 0 );
	const unsigned short	PendWarpToHuntaholicLobby( const int nHuntaholicID );
	void					PendWarpToDeathmatch( int instance_game_type );


	void				SetLastContact( const char * szKey, const char * szValue );
	void				SetLastContact( const char * szKey, int nValue );
	std::string			GetLastContactStr( const char * szKey ) const;
	int					GetLastContactLong( const char * szKey ) const;

	// 시간 관련. (int 는 초단위, AR_TIME 은 1/100 초 단위)
	int				GetLoginDuration() const	{ return ( m_nLogoutTime ? ( ( m_nLogoutTime - m_nLoginTime ) / 100 ) : ( ( GetArTime() - m_nLoginTime ) / 100 ) ); }
	AR_TIME			GetLoginTime() const		{ return m_nLoginTime; }
	AR_TIME			GetLogoutTime()	const		{ return m_nLogoutTime;	}
	int				GetLoginCount() const		{ return m_nLoginCount; }
	int				GetTotalPlayTime()			{ return m_nTotalPlayTime + GetLoginDuration(); }
	void			SetTotalPlayTime( int nTotalPlayTime )		{ m_nTotalPlayTime = nTotalPlayTime; }

	AR_TIME			GetLogoutTimer() const { return m_nLogoutTimer; }
	void			SetLogoutTimer( AR_TIME tm ) { m_nLogoutTimer = tm; }

	// 프로퍼티 Set 이므로 프로퍼티 메시지 송신이 유발되며, 따라서 락이 잡힌 상태에서만 사용해야 함
	void			SetContinuousPlayTime( AR_TIME continuous_play_time );
	const AR_TIME	GetContinuousPlayTime()													{ return ( IsGameTimeLimited() ) ? m_nContinuousPlayTime : 0; }

	void			SetContinuousLogoutTime( AR_TIME continuous_logout_time )				{ m_nContinuousLogoutTime = continuous_logout_time; }
	const AR_TIME	GetContinuousLogoutTime()												{ return ( IsGameTimeLimited() ) ? m_nContinuousLogoutTime : 0; }

	void			SetLastContinuousPlayTimeProcTime( AR_TIME last_playtime_proc_time )	{ m_nLastContinuousPlayTimeProcTime = last_playtime_proc_time; }
	const AR_TIME	GetLastContinuousPlayTimeProcTime()										{ return ( IsGameTimeLimited() ) ? m_nLastContinuousPlayTimeProcTime : 0; }

	// 호감도
	void			AddFavor( int favor_id, int favor );
	void			SetFavor( int favor_id, int favor, bool bDBJob = true );
	int				GetFavor( int favor_id ) const;

	// DB 쿼리 관련
	void			DBQuery( struct GameDBManager::DBProc *pWork );
	void			onEndQuery();

	// 전투 관련	
	virtual int		onDamage( StructCreature * pFrom, Elemental::Type elementalType, DamageType damageType, int nDamage, bool bCritical, bool bDouble);
	void			ProcImmoralPoint( StructPlayer * pKiller, int nChaos, bool bDecreaseProc = true );

	// 대련 관련
	const unsigned int	GetCompeteID() const							{ return m_nCompeteID; }
	void				SetCompeteID( const unsigned int nCompeteID )	{ m_nCompeteID = nCompeteID; SetNeedToBroadcastStatusFlag(); }
	const bool		IsCompeteDead() const								{ return m_StatusFlag.IsOn( STATUS_COMPETE_DEAD ); }
	void			SetCompeteDead( const bool bCompeteDead )			{ m_StatusFlag.Set( STATUS_COMPETE_DEAD, bCompeteDead ); }
	const bool		IsInStartedCompete( const bool includeCounting ) const;

	const bool		ResurrectByCompete();
	const bool		ResurrectByDeathmatch();

	// 배틀 아레나 관련
	int						GetBattleArenaID() const											{ return m_nArenaID; }
	void					SetBattleArenaID( const int nArenaID )								{ m_nArenaID = nArenaID; }
	unsigned char			GetBattleArenaInstanceNo() const									{ return m_nArenaInstanceNo; }
	void					SetBattleArenaInstanceNo( const unsigned char nArenaInstanceNo )	{ m_nArenaInstanceNo = nArenaInstanceNo; }
	time_t					GetBattleArenaBlockTime() const										{ return m_tArenaBlock; }
	AR_TIME					GetBattleArenaBlockTimeLeft() const									{ time_t t = time( NULL ); return ( m_tArenaBlock > t ) ? ( m_tArenaBlock - t ) * 100 : 0;  }
	void					SetBattleArenaBlockTime( const time_t tArenaBlock )					{ m_tArenaBlock = tArenaBlock; }
	int						GetBattleArenaPenaltyCount() const									{ return m_nArenaPenaltyCount; }
	void					SetBattleArenaPenaltyCount( const int nPenaltyCount )				{ m_nArenaPenaltyCount = nPenaltyCount; }
	time_t					GetBattleArenaPenaltyDecTime() const								{ return ( m_nArenaPenaltyCount ) ? m_tArenaPenaltyDecrease : 0; }
	void					SetBattleArenaPenaltyDecTime( const time_t tPenaltyDec )			{ m_tArenaPenaltyDecrease = tPenaltyDec; } 
	void					IncBattleArenaPenalty();
	int						GetBattleArenaPoint() const											{ return m_nArenaPoint; }
	void					SetBattleArenaPoint( const int nArenaPoint )						{ m_nArenaPoint = std::min( std::max( nArenaPoint, 0 ), GameRule::BATTLE_ARENA_MAX_OWNABLE_POINT ); SendPropertyMessage( this, GetHandle(), "ap", GetBattleArenaPoint() ); }
	void					AddBattleArenaPoint( const int nArenaPoint )						{ SetBattleArenaPoint( GetBattleArenaPoint() + nArenaPoint ); }
	int						GetBattleArenaMVPCount() const										{ return m_nArenaMVPCount; }
	void					AddBattleArenaMVPCount()											{ ++m_nArenaMVPCount; }
	int						GetBattleArenaRecord( _BATTLE_ARENA_TYPE eType, bool bWin ) const	{ assert( eType >= 0 && eType < _countof( m_anArenaRecord ) ); return m_anArenaRecord[ eType ][ ( bWin ) ? 0 : 1 ]; }
	void					AddBattleArenaRecord( _BATTLE_ARENA_TYPE eType, bool bWin )			{ assert( eType >= 0 && eType < _countof( m_anArenaRecord ) ); ++m_anArenaRecord[ eType ][ ( bWin ) ? 0 : 1 ]; }

	// 오토 관련
	bool			IsAutoUsed()						{ return m_bAutoUsed; }
	bool			IsMoveReq()							{ return m_bMoveReq; }
	void			SetMoveReq( bool bMoveReq )			{ m_bMoveReq = bMoveReq; }
	void			SetAutoUsed( bool AutoUsed = true)	{ m_bAutoUsed = AutoUsed; }
	bool			IsWarpEnded()						{ return m_bWarpEnded; }
	void			SetWarpEnded( bool WarpEnded = true ) { m_bWarpEnded = WarpEnded; }
	int				GetRegionUpdateNeedCount() const	{ return m_nRegionUpdateNeedCount; }
	void			IncRegionUpdateNeedCount()			{ InterlockedIncrement( &m_nRegionUpdateNeedCount ); }
	void			DecRegionUpdateNeedCount()			{ InterlockedDecrement( &m_nRegionUpdateNeedCount ); }
	int				GetInvalidScriptTriggerCount() const	{ return m_nInvalidScriptTriggerCount; }
	void			IncInvalidScriptTriggerCount()		{ InterlockedIncrement( &m_nInvalidScriptTriggerCount ); }

	// 트레이드
	bool			IsTradableWith( StructPlayer * pTarget ) const;
	void			StartTrade( StructPlayer * pTarget );
	bool			IsTrading() const  { return !!m_hTradeTarget; }
	bool			IsTradeFreezed() const { return m_bTradeFreezed; }
	bool			IsTradeConfirm() const { return m_bTradeAccepted; }	
	AR_HANDLE		GetTradeTarget() const { return m_hTradeTarget; }
	const StructGold	GetTradeGold() const	{ return m_nTradeGold; }
	bool			AddItemToTradeWindow( StructItem * pItem, const __int64 & count );
	bool			ModifyItemCountInTradeWindow( StructItem * pItem, const __int64 & count );
	bool			RemoveItemFromTradeWindow( StructItem * pItem, const __int64 & count );
	bool			RemoveItemByQuittingDeathmatch();
	void			AddGoldToTradeWindow( const StructGold & gold );
	void			FreezeTrade();
	void			ConfirmTrade();
	bool			CheckTradeWeight();
	bool			CheckTradeItem();
	bool			ProcessTrade();
	void			CancelTrade( bool bIsNeedBroadcast = false );	

	bool			ResurrectByPotion();

	const bool		ClearExpiredItem();
	const bool		ClearExpiredElementalEffect();

	void			AddToElementalEffectedItemList( StructItem * pItem );
	void			RemoveFromElementalEffectedItemList( StructItem * pItem );

	int				GetRideSpeedModifier() const { return m_nRideModifier; }

	// PC방 프리미엄 서비스 용
	void			SetPCBangMode( char nPCBangMode );
	char			GetPCBangMode();

	// 계정단위 이벤트 코드
	void			SetEventCode( int nEventCode )		{ m_nEventCode = nEventCode; }
	int				GetEventCode() const				{ return m_nEventCode; }

	// 로그인 시간과 보상 관련 (플레이 포인트, 시간제 이벤트)
	void			SetPlayTime( int nPlayTime )					{ m_nPlayTime = nPlayTime; }
	int				GetPlayTime()									{ return m_nPlayTime; }
	void			SetLastPlayTimeUpdateTime( AR_TIME t )			{ m_nLastPlayTimeUpdateTime = t; }
	AR_TIME			GetLastPlayTimeUpdateTime()						{ return m_nLastPlayTimeUpdateTime; }
	void			SetLastTimeBasedEventTimeScript( AR_TIME t )	{ m_nLastTimeBasedEventTimeScript = t; }
	AR_TIME			GetLastTimeBasedEventTimeScript()				{ return m_nLastTimeBasedEventTimeScript; }
	void			SetLastTimeBasedEventTimeDB( AR_TIME t )		{ m_nLastTimeBasedEventTimeDB = t; }
	AR_TIME			GetLastTimeBasedEventTimeDB()					{ return m_nLastTimeBasedEventTimeDB; }

	// 경매 관련
	void			SetNextAuctionUsableTime( AR_TIME t )			{ m_nNextAuctionUsableTime = t; }
	AR_TIME			GetNextAuctionUsableTime()						{ return m_nNextAuctionUsableTime; }

	// 게임 중독 시스템 용(유저 나이도 관련)
	bool			IsGameTimeLimited();
	void			SetAge( int nAge )					{ m_nAge = nAge; }
	const int		GetAge() const						{ return m_nAge; }

	// 이벤트 영역 진입 횟수 데이터
	const int		GetEventAreaEnterCount( const int nEventAreaID ) const;
	void			IncEventAreaEnterCount( const int nEventAreaID );
	void			SetEventAreaEnterCount( const int nEventAreaID, const int nEnterCount );

	void			InsertEventAreaID( const int nEventAreaID );
	bool			DeleteEventAreaID( const int nEventAreaID );

	// 이벤트용 전 월드 지속효과 부여 관련
	struct EVENT_STATE
	{
		EVENT_STATE()
			: eCode( static_cast< StructState::StateCode >( 0 ) )
			, nLevel( 0 )
			, nStateValue( 0 )
			, strStateValue( "" )
		{}

		EVENT_STATE( const StructState::StateCode _eCode, const int _nLevel, const int _nStateValue, const char * _szStateValue )
			: eCode( _eCode )
			, nLevel( _nLevel )
			, nStateValue( _nStateValue )
			, strStateValue( _szStateValue )
		{}

		const bool operator ==( const EVENT_STATE & rhs ) const
		{
			return ( eCode == rhs.eCode && nLevel == rhs.nLevel && nStateValue == rhs.nStateValue && strStateValue == rhs.strStateValue );
		}

		StructState::StateCode	eCode;
		int						nLevel;
		int						nStateValue;
		std::string				strStateValue;
	};

	// 이벤트용 전 월드 지속효과 부여 관련(지역 락, g_PlayerListLock 걸고 호출하면 안 됨)
	static const bool		AddEventState( const StructState::StateCode eCode, const int nLevel, const int nStateValue, const char * szStateValue );
	static const bool		RemoveEventState( const StructState::StateCode eCode );
	static void				ClearEventState();
	static void				GetEventStateList( std::vector< struct EVENT_STATE > & vList );
	void					ProcEventState();

	// 크루 아이템을 이용한 지속효과로서 중첩 사용이 불가능하다.
	const bool				PendWorldState( const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode );
	const bool				CastWorldState( const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode );

	// 크루 아이템을 이용한 지속효과로서 중첩 사용이 불가능하다. (파티원 용)
	const bool PendPartyState(const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode);
	const bool CastPartyState(const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode);

	// 크루 아이템을 이용한 지속효과로서 중첩 사용이 불가능하다. (길드원 용)
	const bool PendGuildState(const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode);
	const bool CastGuildState(const StructState::StateCode code, const int level, const AR_TIME time, const AR_TIME cooltime, const int itemcode);

	// 
	const bool				AddDungeonState();
	const bool				RemoveDungeonState();

	// 기타 구현용
	void			SendCharacterInfo();

	int				m_nGuildPoint;
	int				m_nGuildTotalPoint;

	const int GetGuildPoint() { return m_nGuildPoint; }
	void SetGuildPoint(int point) { m_nGuildPoint = point; }

	const int GetTotalGuildPoint() { return m_nGuildTotalPoint; }
	void SetTotalGuildPoint(int point) { m_nGuildTotalPoint = point; }

	TimeSyncer		m_TS;
	struct IStreamSocketConnection*	pConnection;	

	void			SetAccount( const int nAccountID, const char *szAccount );
	static ItemUID	allocItemUID();

	void			SetSkinColor( DWORD skin_color )	{ m_nSkinColor = skin_color; }
	unsigned long	GetSkinColor() const				{ return m_nSkinColor; }

	void			RequestSecurityNo( int nMode );
	
protected:
	const bool		clearExpiredItem( const std::vector< struct StructItem * > & vExpireItemList, bool bStorage );

	virtual unsigned short	putonItem( ItemBase::ItemWearType pos, struct StructItem * pItem );	// insert absolute position
	virtual unsigned short	putoffItem( ItemBase::ItemWearType pos );	// insert absolute position

	void					procDecreaseEXPAndDropItem( StructPlayer * pKiller, struct StructItem ** pDropWearItem, struct StructItem ** pDropInvenItem );

	void					addToBoothWatcher( StructPlayer * pWatcher );
	void					removeFromBoothWatcher( StructPlayer * pWatcher );

	inline const bool		isInLocationType( unsigned char nLocationType ) const;

private:
	void					setLocation( int idx )								{ m_nWorldLocationId = idx; }

	virtual const char*		getClassName()										{ return "StructPlayer"; }

	virtual void			onProcess( int nThreadIdx );
	
	virtual void			onHPChange( int nPrevHP );
	virtual void			onMPChange( int nPrevMP );
	virtual void			onChangeProperty( const std::string & strKey, const char *data );
	virtual void			onBeforeChangeProperty( const std::string & strKey, const char *data );

	virtual void			onExpChange();
	virtual void			onEnergyChange();

	virtual void			onCantAttack( AR_HANDLE target, AR_TIME t );
	virtual void			onDead( StructCreature *pFrom, bool decreaseEXPOnDead = true );
	virtual void			onJobLevelUp();
	virtual void			onJPChange();
	virtual void			onRegisterSkill( int skill_uid, int skill_id, int skill_level, bool is_new_skill );	

	// 스킬 초기화 관련
	virtual __int64			getJPAfterSkillReset()								{ return GetJobPoint() + GetAllSkillJP() + GetAllJobLevelJP(); }
	virtual int				getTPAfterSkillReset()								{ return GetTalentPoint() + GetAllSkillTP(); }
	virtual void			onResetSkill( const int jobDepth );
	virtual void			onRemoveSkill( StructSkill* removedSkill );

	// 직업 초기화 관련
	__int64					getJPAfterJobReset( int nTargetJobDepth );

	// 전체 특성 포인트 중 기본으로 지급 받은 포인트를 제외한 특성 포인트만 남긴다.
	// 다시 전직을 하면 지급을 받는다.
	int						getTPAfterJobReset( int nJobDepth )					{ return m_nJobDepth >= GameRule::MIN_TALENT_POINT_JOB_DEPTH ? GetTalentPoint() - GameRule::DEFAULT_TALENT_POINT : GetTalentPoint(); }
	void					onBeforeResetJob()									{};
	void					onAfterResetJob();

	void					onBeforeResetRace()									{};
	void					onAfterResetRace();

	virtual void			procMoveSpeedChangement();

	virtual void			onAfterAddState( StructState & state );
	virtual void			onAfterRemoveState( StructState & state, const bool bByDead = false );

	// 경험치 관련 루틴
	void					applyPenaltyByImmoralPoint( Experience& result );
	void					applyLimitBySummonLevel( Experience& result );
	void					applyPenaltyByGameTimeLimit( Experience& result );
	const Experience		getPCBangBonus( const Experience& input );
	const Experience		getStaminaBonus( const Experience& input, const __int64 gainExp );
	const Experience		getSuperSaveBonus( const Experience& input );
	const Experience		getInGameBonus( const Experience& input );
	void					distributeExpToSummons( const Experience& input, const __int64 summonStaminaSaveBonus );
	void					applyGameRuleLimit( Experience& result );
	
	void					openStorage();
	
	void					clearTradeInfo();

	void					broadcastBoothInfo();

	
	void					updateQuestStatus( StructQuest* pQuest );
	void					onStartQuest( StructQuest* pQuest );
	void					onEndQuest( StructQuest* pQuest );
	void					onDropQuest( StructQuest* pQuest );

	void					checkSummonEffectOnPlayer();

	bool					unSummon( struct StructSummon * pSummon );
	void					procPendingUnSummon();
	bool					unSummonPet( struct StructPet * pPet );
	void					procPendingUnSummonPet();

	bool					eraseItem( StructItem *pItem, const __int64 & count );
	struct StructItem*		popItem( struct StructItem *pItem, const __int64 & count = 1, bool bSkipUpdateItemToDB = false );

	const bool				procAutoRecover();

	void					onStandUp();

	bool					m_bDeadDialogProc;
	bool					m_bIsLogin;
	bool					m_bHasLogined;
	AR_HANDLE				m_hHandle;

	AR_HANDLE				m_hTamingTarget;

	// DB 에 있는 값들
	char					m_szAccountName[GameRule::MAX_ACCOUNT_LEN + 1];
	int						m_nAccountID;
	int						m_nChatBlockTime;
	int						m_nPermission;
	int						m_nHuntaholicPoint;
	int						m_nHuntaholicEnterableCount;
	int						m_nEtherealStoneDurability;
	time_t					m_tNextHuntaholicEnterableCountRefill;
	StructGold				m_nGold;
	PlayerUID				m_nUID;	
	char					m_szName[31];
	int						m_nPartyID;
	bool					m_bItemPriority;
	int						m_nPrevPartyBroadcastedHP;
	int						m_nPrevPartyBroadcastedMP;
	int						m_nGuildId;
	int						m_nPrevGuildId;
	char					m_nGuildPermission;
	int						m_nSex;	
	int						m_nTalentPoint;
	int						m_nCharisma;
	int						m_nLoginCount;
	unsigned long			m_nSkinColor;
	int						m_nBaseModelId[5];	
	int						m_nFaceTextureId;
	int						m_nHairColorIndex;
	unsigned int			m_nHairColorRGB;

	unsigned int			m_nHideEquipFlag;

	std::string				m_strClientInfo;
	std::string				m_strQuickSlot;
	std::string				m_strCurrentKey;
	std::string				m_strSavedKey;

	AR_TIME					m_nLastGlobalChatTime;
	AR_TIME					m_nNextChatPenaltyDecreaseTime;
	int						m_nChatPenalty;
	std::string				m_strLastGlobalChat;
	time_t					m_nLastAdvChatTime;
	int						m_nAdvChatCount;
	volatile LONG			m_nNameChanged;
	bool					m_bAutoUsed;
	time_t					m_tGuildBlockTime;
	bool					m_bMoveReq;
	bool					m_bWarpEnded;
	volatile LONG			m_nRegionUpdateNeedCount;

	AR_HANDLE				m_hTarget;	// 플레이어가 타게팅 하고 있는 Creature Handle
	
	unsigned int			m_nCompeteID;

	int						m_nArenaID;
	unsigned char			m_nArenaInstanceNo;
	time_t					m_tArenaBlock;
	int						m_nArenaPenaltyCount;
	time_t					m_tArenaPenaltyDecrease;
	int						m_nArenaPoint;
	int						m_nArenaMVPCount;
	int						m_anArenaRecord[ 3 /* _BATTLE_ARENA_TYPE */ ][ 2 /* win, lose */ ];

	int						m_nTotalPlayTime;
	bool					m_bUseAlias;
	char					m_szAlias[ 31 ];
	int						m_nCharacterChecksum;

	struct PendedWorldState // 월드 버프는 락 문제로 비동기적으로 검, 현재 서버에는 비동기 관련 작업을 할 기반 자체가 없으므로 일단 우회
	{
		int itemcode;
		int code;
		int level;
		int time;
		int cooltime;
	};

	std::vector< PendedWorldState > m_vPendedWorldStateList;
	std::vector< PendedWorldState > m_vPendedPartyStateList;
	std::vector< PendedWorldState > m_vPendedGuildStateList;

	// 워프 관련
	AR_UNIT					m_nPendWarpX, m_nPendWarpY;	
	unsigned char			m_nPendWarpLayer;
	int						m_nPendWarpInstanceDungeonType;
	int						m_bForceWarp;					// 강제 워프에 사용 (true면 여러가지 입장 검사 안 함)

	int						m_nJobDepth;

	AR_TIME					m_nWarpEndTime;
	AR_TIME					m_nLoginTime;
	AR_TIME					m_nLogoutTime;	
	AR_TIME					m_nLastCantAttackTime;
	AR_TIME					m_nLastSaveTime;
	AR_TIME					m_nLastExpSaveTime;
	AR_TIME					m_nLastProcessTime;
	AR_TIME					m_nLogoutTimer;
	AR_TIME					m_nLastLogoutRequestedTime;		// 로비로 돌아가기/로그아웃 요청 시각(TS_CS_LOGOUT / TS_CS_RETURN_LOBBY 수신 시 검증 용)

	AR_TIME					m_nContinuousPlayTime;					// _CONNECTION_TAG::nContinuousPlayTime을 플레이어에 보관 해두는 변수
	AR_TIME					m_nContinuousLogoutTime;				// _CONNECTION_TAG::nContinuousLogoutTime을 플레이어에 보관 해두는 변수
	AR_TIME					m_nLastContinuousPlayTimeProcTime;		// 위의 변수 2개를 처리하기 위한 타이머

	// 노점
	BOOTH_STATUS							m_BoothStatus;
	std::string								m_strBoothName;
	std::vector< BOOTH_ITEM_INFO >			m_vBoothItem;
	std::vector< StructPlayer * >			m_vBoothWatcher;
	StructPlayer*							m_pBoothOpener;

	bool					m_bIsSitDown;
	bool					m_bIsBattleMode;

	bool					m_bStaminaSave;
	bool					m_bSuperSave;

	enum _SUMMON_STAMINA_SAVE_TYPE
	{
		SUMMON_STAMINA_SAVE_NONE			= 0,
		SUMMON_STAMINA_SAVE_WITHOUT_PENALTY	= 1,
		SUMMON_STAMINA_SAVE_WITH_PENALTY	= 2,
	};
	_SUMMON_STAMINA_SAVE_TYPE	m_eSummonStaminaSaveType;

	bool					m_bWalk;

	// 트레이드
	struct	TradeItemInfo
	{
		TradeItemInfo( AR_HANDLE hItem, __int64 nCount )
			: m_hItem( hItem )
			, m_nCount( nCount )
		{
		}

		AR_HANDLE	m_hItem;
		__int64		m_nCount;
	};

	bool					m_bTradeFreezed;
	bool					m_bTradeAccepted;
	StructGold				m_nTradeGold;
	std::vector< TradeItemInfo >	m_vTradeItemList;
	AR_HANDLE				m_hTradeTarget;
	unsigned short			processTradeGold();
	unsigned short			processTradeItem();


	// 퀘스트
	int						m_nLastAcceptQuest;
	StructQuestManager		m_QuestManager;

	// 호칭
	StructTitleManager		m_TitleManager;

	// 인벤토리
	StructInventory						m_Inventory;
	StructInventory						m_Storage;
	AR_TIME								m_nLastInvenArrangedTime;
	AR_TIME								m_nLastStorageArrangedTime;
	
	AR_TIME								m_nItemCoolTime[ ItemBase::MAX_COOLTIME_GROUP ];

	std::vector< StructItem * >			m_vCharmList;
	std::vector< StructItem * >			m_vElementalEffectedItem;

	// 창고
	volatile int						m_nIsUsingStorage;
	ItemUID								m_nStorageGoldItemID;
	volatile bool						m_bIsStorageRequested;
	volatile bool						m_bIsStorageLoaded;
	StructGold							m_nStorageGold;
	virtual void						onAdd( struct StructInventory* pInventory, struct StructItem* pItem, bool bSkipUpdateItemToDB = false );
	virtual void						onRemove( struct StructInventory* pInventory, struct StructItem* pItem, bool bSkipUpdateItemToDB = false );
	virtual void						onChangeCount( struct StructInventory* pInventory, struct StructItem* pItem, bool bSkipUpdateItemToDB = false );

	// 퀘스트
	virtual void						onProgressChanged( struct StructQuest *pQuest, QuestInstance::QUEST_PROGRESS oldProgress, QuestInstance::QUEST_PROGRESS newProgress );
	virtual void						onStatusChanged( struct StructQuest *pQuest );
	void								setSummonUpdate();

	// 호칭 핸들러
	virtual bool						isReadInfoComplete()	{ return IsReadInfoComplete(); }
	virtual void						onTitleAchieved( StructTitle * pTitle );
	virtual void						onTitleOpened( StructTitle * pTitle );
	// 이하 핸들러는 빈번한 변경이 발생할 수 있으므로 DB Query를 즉시 요청하지 않는다.
	virtual void						onTitleBookmarked( StructTitle * pTitle );
	virtual void						onConditionChanged( struct TitleConditionType * pCondition, const __int64 nCount );

	int									m_nWorldLocationId;
	const struct StructWorldLocation *	m_pWorldLocation;

	// 피케이 관련
	c_fixed10							m_fImmoralPoint;
	int									m_nPKC;
	int									m_nDKC;
	bool								m_bIsPK;
	AR_TIME								m_nTurnOnPkModeTime;
	AR_TIME								m_nTurnOffPkModeTime;

	// 혼돈 관련 파라미터
	int									m_nMaxChaos;
	int									m_nChaos;

	// 피로도 관련 파라미터
	int									m_nMaxStamina;
	int									m_nStamina;
	int									m_nStaminaRegenRate;
	bool								m_bUsingTent;
	int									m_nStaminaRegenBonus;
	int									m_nLogoutDuration;		// 로그아웃 기간(분 단위)

	// 가이아 상인 멤버쉽
	bool								m_bIsGaiaMember;
	AR_TIME								m_nGaiaValidTime;

	// PC방 프리미엄 관련
	char								m_nPCBangMode;

	// 계정단위 이벤트 코드
	int									m_nEventCode;

	// 로그인 시간과 보상 관련 (플레이 포인트, 시간제 이벤트)
	int									m_nPlayTime;
	AR_TIME								m_nLastPlayTimeUpdateTime;
	AR_TIME								m_nLastTimeBasedEventTimeScript;
	AR_TIME								m_nLastTimeBasedEventTimeDB;

	// 경매 관련
	AR_TIME								m_nNextAuctionUsableTime;	// 경매에서 다음 처리 요청 가능한 시각(경매 동작 반복 시간 제한을 두기 위함)

	// 게임 중독 방지 시스템 관련(유저 나이)
	int									m_nAge;
	int									m_nLastMin;

	// 메신저 관련
	std::vector< std::string >			m_vFriend;
	std::vector< std::string >			m_vFriendOf;
	std::vector< std::string >			m_vDenial;
	std::vector< std::string >			m_vDenialOf;

	// 소환수
	std::vector< struct StructSummon* >	m_vSummonList;
	std::vector< struct StructSummon* >	m_vStorageSummonList;
	struct StructItem*					m_aBindSummonCard[ 6 ];
	struct StructItem*					m_aBeltSlotCard[ 8 ];
	int									m_nBeltSlotMax;
	float								m_fBeltSlotAmp;
	bool								m_bIsSummonable;
	bool								m_bIsInfiniteSummonTime;

	struct StructSummon*				m_pMainSummon;
	struct StructSummon*				m_pSubSummon;
	AR_TIME								m_nDoubleSummonTime;
	AR_TIME								m_nNextUnSummonTime;
	AR_TIME								m_nLastBindSummonUpdateTime;
	int									m_nNameChangeTarget;
	volatile LONG						m_nPendingUnSummon;
	enum PENDING_UNSUMMON_TYPE
	{
		UNSUMMON_MAIN					= 1,
		UNSUMMON_SUB					= 2,
	};

	// 소환수 농장 관련
	FARMED_SUMMON_INFO *				m_vFarmedSummonInfo[GameRule::FARM_MAX_COUNT];

	// 펫
	XCriticalSection					m_csPet;		// 지역락 걸린 상태에서 사용해야 함
	std::vector< struct StructPet * >	m_vPetList;
	std::vector< struct StructPet * >	m_vStoragePetList;
	std::vector< struct StructPet * >	m_vPendingUnSummonPetList;
	struct StructPet *					m_pSummonedPet;

	// 호칭
	TitleBaseServer *					m_pMainTitle;
	TitleBaseServer *					m_pSubTitle[5];
	AR_TIME								m_tRemainTitleTime;

	AR_TIME								m_nLastStaminaUpdateTime;
	std::vector< struct StructSkill * >		m_vSummonPassiveSkillList;
	std::vector< struct StructSkill * >		m_vAmplifySummonPassiveSkillList;

	float								m_fActiveSummonExpAmp;
	float								m_fDeactiveSummonExpAmp;

	float								m_fPassDamageRatio;

	// 아이템 성능에 의한 스텟 계산시 보정치
	int									m_nRideModifier;
	int									m_nSpeedModifier;

	int									m_nMaxWeightModifier;

	// 자동 회복 기능 관련
	enum _RECOVER_TYPE
	{
		RECOVER_TYPE_HP		= 1,
		RECOVER_TYPE_MP		= 2,
	};
	struct AUTO_RECOVER_INFO
	{
		AUTO_RECOVER_INFO( const _RECOVER_TYPE & eRecoverType, const c_fixed10 & fCondition, const c_fixed10 & fRecoverTo, const AR_HANDLE hItem, const int nOrderIndex )
			: m_eRecoverType( eRecoverType )
			, m_fCondition( fCondition )
			, m_fRecoverTo( fRecoverTo )
			, m_hItem( hItem )
			, m_nOrderIndex( nOrderIndex )
		{}

		_RECOVER_TYPE	m_eRecoverType;
		c_fixed10		m_fCondition;
		c_fixed10		m_fRecoverTo;
		AR_HANDLE		m_hItem;
		int				m_nOrderIndex;
	};

	std::vector< AUTO_RECOVER_INFO >	m_vAutoRecoverInfo;

	static const bool _AutoRecoverInfoArrangeItemPositionPreceder( const AUTO_RECOVER_INFO & lhs, const AUTO_RECOVER_INFO & rhs )
	{
		return lhs.m_nOrderIndex < rhs.m_nOrderIndex;
	}

	enum
	{
		MOUNT_NOTHING = 0,
		MOUNT_ON_MAIN = 1,
		MOUNT_ON_SUB = 2,
	};

	StructState::StateCode				m_nRidingStateCode;
	int									m_nRideIdx;			// 어떤 소환수에 Ride했는지.
	bool								m_bUseFasterSpeedInRiding;		// 라이딩 시 기본 이속이 더 빠르면 기본 이속으로 적용해줄 것인지 여부

	std::vector< FAVOR_INFO >			m_vFavorInfo;

	// 이벤트 영역 관련 데이터(대개 Lua 스크립트와 연결되므로 지역 락에 의해 보호함)
	KHash< int, hashPr_mod_int >		m_hsEventAreaEnterCount;
	std::set< int >						m_stEventAreaID;

	// m_vEventState를 보호(지역 락 -> m_csEventState 순서여야 함)
	static XCriticalSection					s_csEventState;
	static std::vector< EVENT_STATE >		s_vEventState;

	// 이하 구현을 위한 것들
	int									m_nPlayerListIndex;
	int									m_nReturnLobbyConnId;

	// 스크립트
	int									m_nDialogType;
	std::string							m_strDialogTitle;
	std::string							m_strDialogText;
	std::string							m_strDialogMenu;
	std::string							m_strSpecialDialogMenu;
	std::string							m_strFixedDialogTrigger;
	bool								m_bNonNPCDialog;
	volatile LONG						m_nInvalidScriptTriggerCount;

	// 플레이어 변수
	mutable XCriticalSection					m_csHash;
	KHash< std::string, hashPr_string_nocase >	m_hsValue;

	// 플레이어 변수 - 계정 데이터
	mutable XCriticalSection					m_csHashAccount;
	KHash< std::string, hashPr_string_nocase >	m_hsValueAccount;

	// 최근 contact 한 상점이름
	//std::string							m_strLastContactMarket;
	KHash< std::string, hashPr_string_nocase >	m_hsContact;

	// 플레이어 이름 - UID 저장용 캐쉬(타 매니저에서 PlayerUID만 가지고있고 플레이어 이름을 중복해서 보관하지 않고 아래의 함수를 통해 얻어서 사용하도록 함)
	static XCriticalSection														s_csPlayerName;
	static KHash< std::string *, hashPr_mod_basic< PlayerUID > >	s_hsPlayerName;

	StructPlayer( AR_HANDLE handle );
	virtual	~StructPlayer();

	friend struct DB_Login;
	friend struct DB_UpdateCharacter;
	friend struct DB_CreateCharacter;
	friend struct DB_ReadStorageList;
	friend struct DB_ChangeCharacterName;
	friend struct PlayerDeleter;

	// ArScheduler 에서 StructPlayer 를 지우기 위해 존재함.
	// alloc/free 메커니즘에 관한 확실한 이해 전에는 수정 말것!
	virtual bool	ProcDelete();
	virtual bool	IsDeleteable();

	// DB 관련
	XCriticalSection							m_bQueryLock;
	std::list< struct GameDBManager::DBProc* >	m_lQueryList;

	// pc방, 스테미너 등 경험치 획득 방송이후에 추가 방송이 필요한 부분 관련
	enum BONUS_TYPE
	{
		BONUS_PCBANG			= 0,
		BONUS_STAMINA			= 1,
		BONUS_PREMIUM_PCBANG	= 2,
		BONUS_SUMMON_STAMINA	= 3,
		BONUS_SUPER_SAVE		= 4,	// 성장의 물약(구 슈퍼 세이버)
		BONUS_IN_GAME			= 5,	// 게임 내부 요인에 의한 보너스

		MAX_BONUS_TYPE		// 마지막 보너스 인덱스보다 1 크게 되므로 개수로 쓰면 됨
	};

	TS_SC_BONUS_EXP_JP::BONUS_INFO		m_pBonusInfo[ MAX_BONUS_TYPE ];

	inline void setBonusMsg( BONUS_TYPE BonusType, int nBonusRateAsPercent, __int64 nBounsExp, int nBonusJp )
	{
		m_pBonusInfo[ BonusType ].type = BonusType;
		m_pBonusInfo[ BonusType ].rate = nBonusRateAsPercent;
		m_pBonusInfo[ BonusType ].exp = nBounsExp;
		m_pBonusInfo[ BonusType ].jp = nBonusJp;
	}

	inline void clearPendingBonusMsg()	{
		// exp가 -1 이면 보너스 없는 거3~
		for( int i = 0 ; i < MAX_BONUS_TYPE ; i++ )
		{
			m_pBonusInfo[i].exp = -1;
			m_pBonusInfo[i].jp = -1;
		}
	}
	
	void sendBonusExpJpMsg();
	int m_nPlayTimePoint;
	int m_nRX;
	int m_nRY;
	int m_nHX;
	int m_nHY;


	// 스탯 관련 함수 및 변수들을 아래에 정리
private:
	// --- 스탯 및 능력치 연산에 관련된 패시브 스킬들 적용하는 루틴 ---
	virtual void			applyPassiveSkillEffect( struct StructSkill * pSkill );

	// --- 스탯 및 능력치 연산에 관련된 지속 효과들 적용하는 루틴 ---
	virtual void			applyState( StructState & state );
	virtual void			applyStatByState();

	// --- 기타 예외적인 처리 ---
	virtual void			applyJobLevelBonus();

	// --- 스탯 및 능력치 연산에 관련된 아이템 효과를 적용하는 루틴 ---
	virtual void			_applyEffectForState( const EffectInfo * pEffect );
	virtual void			onItemWearEffect( struct StructItem* pItem, bool bIsBaseVar, int type, c_fixed10 var1, c_fixed10 var2, float fItemRatio );

	// --- 스탯 및 능력치 연산에 관련된 호칭 효과를 적용하는 루틴 ---
	virtual void			applyStatByTitle();
	virtual void			amplifyStatByTitle();
	virtual void			applyTitleEffect();

	// --- 이하 핸들러들 ---
	virtual void			onBeforeCalculateStat();	
	virtual void			onApplyStat();
	virtual void			onAfterCalculateAttributeByStat();
	virtual void			onModifyStatAndAttribute();
	virtual	void			onCompleteCalculateStat();
	void					onModifyParameterForSummon();
	void					_applyCharm( const int type, const c_fixed10 & var1, const c_fixed10 & var2, struct StructItem * pItem );
	void					applyCharm( struct StructItem * pItem );

	// party matching
public:
	inline CPartyMatching* GetPartyMatching()
	{
		return m_party_matching;
	}
	inline void SetPartyMatching(CPartyMatching* pPartyMatching)
	{
		m_party_matching = pPartyMatching;
	}
	inline void SetPartyMatchingLeaderName(const char* name)
	{
		m_party_matching_leader_name = name;
	}
	inline std::string& GetPartyMatchingLeaderName()
	{
		return m_party_matching_leader_name;
	}

private:
	CPartyMatching* m_party_matching;
	std::string m_party_matching_leader_name;

	int m_nLastReadSec;
	int m_nReadCount;
	int m_nTotalCount;
	int pFreq[4];
};

struct _CONNECTION_TAG
{
	_CONNECTION_TAG( const char *_szAccountName )
		: pPlayer( NULL )
		, nAccountID( 0 )
		, nVersion( -1 )
		, nLastReadTime( GetArTime() )
		, bAuthByAuthServer( false )
		, nPCBangMode( 0 )
		, nEventCode( 0 )
		, nAge( 0 )
		, nAgeLimitFlags( 0 )
		, nContinuousPlayTime( 0 )
		, nContinuousLogoutTime( 0 )
		, nLastContinuousPlayTimeProcTime( GetArTime() )
		, nConnId( 0 )

		, pSecuritySolutionBuffer( NULL )

		, bStorageSecurityCheck( false )

	{
		s_strcpy( szAccountName, _countof( szAccountName ), _szAccountName );
	}

	char						szAccountName[GameRule::MAX_ACCOUNT_LEN + 1];
	std::vector< std::string >	vCharacterNameList;
	StructPlayer*				pPlayer;
	int							nAccountID;
	int							nVersion;
	AR_TIME						nLastReadTime;
	bool						bAuthByAuthServer;	// Temporary
	char						nPCBangMode;
	int							nEventCode;
	int							nAge;
	int							nAgeLimitFlags;
	AR_TIME						nContinuousPlayTime;
	AR_TIME						nContinuousLogoutTime;
	AR_TIME						nLastContinuousPlayTimeProcTime;
	int							nConnId;
	std::string					strNameToDelete;
	bool						bStorageSecurityCheck;
	void *						pSecuritySolutionBuffer;
};