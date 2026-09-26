#pragma once


#include <mmo/ArObject.h>
#include <toolkit/ILock.h>

#include "StructBase.h"
#include "GameType.h"
#include "ItemInstance.h"
#include "StructGold.h"
#include "GameDBManager.h"

#include <list>
#include <time.h>


struct StructItem : GameObject
{
	struct ITEM_PICKUP_ORDER
	{
		ITEM_PICKUP_ORDER()
		{
			memset(this, 0, sizeof(ITEM_PICKUP_ORDER));
		}

		AR_HANDLE					hPlayer[ 3 ];
		int							nPartyID[ 3 ];
	};

	// PreviousUID에 따른 정렬을 쉽게 하기 위해 필요한 구조체
	struct INDEX_FINDER
	{
			StructItem *				pItem;
			INDEX_FINDER( StructItem * _pItem ) : pItem( _pItem ){}

			bool operator ==( ItemUID uid )
			{
				return uid == pItem->GetPreviousUID();
			}
	};

	enum _TARGET_TYPE
	{
		TARGET_TYPE_PLAYER		= 0,
		TARGET_TYPE_SUMMON		= 1,
		TARGET_TYPE_MONSTER		= 2,
		TARGET_TYPE_NPC			= 3,
		TARGET_TYPE_UNKNOWN		= 4
	};

	static StructItem*  AllocItem(	ItemUID uid,
									ItemBase::ItemCode code,
									const __int64 & cnt = 1,
									ItemInstance::GenerateCode info = ItemInstance::BY_UNKNOWN,
									int level = -1,
									int	enhance = -1,
									int flag = -1,
									ItemBase::ItemCode socket_0 = 0,
									ItemBase::ItemCode socket_1 = 0,
									ItemBase::ItemCode socket_2 = 0,
									ItemBase::ItemCode socket_3 = 0,
									int awaken_sid = 0,
									int identified_sid = 0,
									int remain_time = -1,
									const unsigned char & elemental_effect_type = 0,
									const time_t & elemental_effect_expire_time = 0,
									const int elemental_effect_attack_point = 0,
									const int elemental_effect_magic_point = 0,
									const ItemBase::ItemCode appearance_code = 0,
									int summon_code = 0,
									int effect_id = 0);
	static StructItem*			FindItem( AR_HANDLE handle );
	static bool					SwapDura(ItemBase::ItemCode& code, int& effect_id);
	static StructItem*			AllocGold(	const StructGold & gold, ItemInstance::GenerateCode gcode = ItemInstance::BY_UNKNOWN );

	static void					InitItemSystem();
	static void					DeInitItemSystem();
	
#define PendFreeItem( p ) PendFreeItem_( p, __FILE__, __LINE__ )
	static void					PendFreeItem_( StructItem* p, const char* function, const int line );		// 지워 달라고 요청. (모았다 주기적으로 한꺼번에 delete 한다)

	friend struct StructPlayer;

	virtual bool				IsDeleteable();

	static bool					RegisterItemBase( ItemBaseServer & base );
	static ItemBase::ItemCode	GetItemCode( const char *szItemName );
	static bool					IsValidItemCode( ItemBase::ItemCode code );

	static bool					RegisterConvertingInfo( ItemBase::ItemCode _key, ItemBase::ItemCode _value );

	AR_HANDLE					GetHandle() const					{ return m_hHandle; }

	virtual bool				IsItem() const						{ return true;												}
	inline bool					IsBow() const						{ return ( GetItemBase().nClass == ItemBase::CLASS_LIGHT_BOW || GetItemBase().nClass == ItemBase::CLASS_HEAVY_BOW );	}
	inline bool					IsCrossBow() const					{ return GetItemBase().nClass == ItemBase::CLASS_CROSSBOW;	}
	// Bullet 류는 화살 뿐만 아니라 화살통을 포함한다.
	inline bool					IsBullet() const					{ return GetItemBase().nGroup == ItemBase::GROUP_BULLET;	}
	inline bool					IsBelt() const						{ return GetItemBase().nGroup == ItemBase::GROUP_BELT;		}
	inline bool					IsEquipment() const					{ return GetItemBase().nType == ItemBase::TYPE_ARMOR;		}
	inline bool					IsCard() const						{ return GetItemBase().nType == ItemBase::TYPE_CARD;		}
	inline bool					IsCharm() const						{ return GetItemBase().nType == ItemBase::TYPE_CHARM;		}
	inline bool					IsCollection() const				{ return GetItemBase().nType == ItemBase::TYPE_COLLECTION;	}
	inline bool					IsWeapon() const					{ return GetItemGroup() == ItemBase::GROUP_WEAPON;			}	
	inline bool					IsAccessory() const					{ return GetItemGroup() == ItemBase::GROUP_ACCESSORY;		}
	inline bool					IsItemCard() const					{ return GetItemGroup() == ItemBase::GROUP_ITEMCARD;		}
	inline bool					IsWarpItem() const					{ return GetItemBase().Flag.IsOn( ItemBase::FLAG_WARP );	}
	inline bool					IsSummonCard() const				{ return GetItemGroup() == ItemBase::GROUP_SUMMONCARD;		}
	inline bool					IsPetCage() const					{ return GetItemGroup() == ItemBase::GROUP_PET_CAGE;		}
	inline bool					IsSkillCard() const					{ return GetItemGroup() == ItemBase::GROUP_SKILLCARD;		}
	inline bool					IsSpellCard() const					{ return GetItemGroup() == ItemBase::GROUP_SPELLCARD;		}
	inline bool					IsCube() const						{ return GetItemBase().nType == ItemBase::TYPE_CUBE;		}
	inline bool					IsSupplyItem() const				{ return GetItemBase().nType == ItemBase::TYPE_SUPPLY;		}
	inline bool					IsUsingItem() const					{ return GetItemBase().nType == ItemBase::TYPE_USE || GetItemBase().nType == ItemBase::TYPE_USE_CARD;			}
	inline bool					IsStrikeCube() const				{ return GetItemGroup() == ItemBase::GROUP_STRIKE_CUBE;		}
	inline bool					IsDefenceCube() const				{ return GetItemGroup() == ItemBase::GROUP_DEFENCE_CUBE;	}
	inline bool					IsSkillCube() const					{ return GetItemGroup() == ItemBase::GROUP_SKILL_CUBE;		}
	inline bool					IsArtifact() const					{ return GetItemGroup() == ItemBase::GROUP_ARTIFACT;		}
	inline bool					IsEquipmentOnBelt() const			{ return GetItemGroup() == ItemBase::GROUP_EQUIPMENT_ON_BELT;	}
	inline bool					IsFarmPass() const					{ return GetItemBase().nClass == ItemBase::CLASS_FARM_PASS;	}
	inline bool					IsRefreshStone() const				{ return GetItemCode() == 800000; }	// 하드코드 -_-
	inline bool					IsEtherealStone() const				{ return GetItemBase().nClass == ItemBase::CLASS_ETHEREAL_STONE;}
	inline bool					IsTwoHandItem()	const				{ return GetItemBase().WearType == ItemBase::WEAR_TWOHAND && GetItemBase().nClass != ItemBase::CLASS_CROSSBOW; }
	inline bool					IsDonatable() const					{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_DONATE );		}
	inline bool					IsNeedTarget() const				{ return GetBaseFlag().IsOn( ItemBase::FLAG_TARGET_USE );		}
	inline bool					IsCashItem() const 					{ return GetBaseFlag().IsOn( ItemBase::FLAG_CASHITEM );			}
	inline bool					IsTradable() const 					{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_TRADE );		}	// 뭐가 좀;;
	inline bool					IsDropable() const 					{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_DROP );		}	// 역시 좀;;
	inline bool					IsUsableMoving() const				{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_MOVING );	}	// 으음;;
	inline bool					IsUsableSit() const					{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_SIT );	}	// 으음;;
	inline bool					IsUsableInSiegeOrRaid() const		{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_IN_RAID_SIEGE );		}	// 아... 막 가는거다;;
	inline bool					IsUsableInSecroute() const			{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_IN_SECROUTE );		}	// 젠장 왜 사용 불가 플래그냐고...
	inline bool					IsUsableInEventmap() const			{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_IN_EVENTMAP );		}	// 역시 긍정 문으로 바꾸는 것이...;;
	inline bool					IsUsableInHuntaholic() const		{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_IN_HUNTAHOLIC );		}	// ...
	inline bool					IsUsableInOnlyHuntaholic() const	{ return GetBaseFlag().IsOn( ItemBase::FLAG_USABLE_IN_ONLY_HUNTAHOLIC );	}
	inline bool					IsUsableInDeathmatch() const		{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_IN_DEATHMATCH );		}
	inline bool					IsUsableInOnlyDeathmatch() const	{ return GetBaseFlag().IsOn( ItemBase::FLAG_USABLE_IN_ONLY_DEATHMATCH );	}
	inline bool					IsErasable() const					{ return !GetBaseFlag().IsOn( ItemBase::FLAG_NOT_ERASABLE );				}
	inline bool					IsUsableOnOnlySit() const			{ return GetBaseFlag().IsOn( ItemBase::FLAG_USABLE_ON_ONLY_SIT );			}
	inline bool					IsUsableInSecretDungeon() const		{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_IN_SECRET_DUNGEON );	}
	inline bool					IsUsableInBattleArena() const		{ return !GetBaseFlag().IsOn( ItemBase::FLAG_CANT_USE_IN_BATTLE_ARENA );	}
	inline bool					IsRandomizable() const				{ return GetBaseFlag().IsOn( ItemBase::FLAG_RANDOMIZABLE );	}
	// 중첩기준: 중첩 가능한 플래그, 소환수가 들어 있지 않고, 스킬카드면 장비되있지 않아야 함.
	bool						IsJoinable() const					
	{ 
		return GetBaseFlag().IsOn( ItemBase::FLAG_JOIN ) 
			&& !GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_SUMMON )
			&& !( IsSkillCard() && ( GetBindedPlayerSID() || GetBindedSummonSID() ) );	
	}

	void						CopyFrom( StructItem* pFrom );
	const bool					IsReplaceable( const ItemBase::ItemCode code, const bool reset_remain_time );
	const char*					GetName() const;
	std::string					GetNameInGame();	// 게임상 이름 반환 (게임에서 못 보는 이름인 '< >' 태그 삭제된 이름으로 반환)

	inline void					SetPickupOrder( const ITEM_PICKUP_ORDER& ItemPickupOrder )	{ s_memcpy( &m_ItemPickupOrder, sizeof( m_ItemPickupOrder ), &ItemPickupOrder, sizeof( m_ItemPickupOrder ) ); }
	inline const ITEM_PICKUP_ORDER&	GetPickupOrder() const	{ return m_ItemPickupOrder; }

	// ItemUID 는 DB 에 저장할 필요가 있을때 발급된다.
	// 이는 플레이어의 인벤토리에 포함될때를 의미하며 해서 StructPlayer 에서 할당해준다.
	bool						SetItemUID( ItemUID uid );
	ItemUID		GetItemUID() const					{ return m_Instance.UID;			}

	bool						ChangeItemCode( const ItemBase::ItemCode code );

	// PreviuosUID는 아이템이 StructInventory 구조체 하에서 관리될 때 벡터형태의 메모리 구조의 단점을 어느정도 커버하기위해 사용된다.
	// 구체적으로는 발급은 StructInventory::push 로직내에서 처리되고, 데이터의 관리는 StructInventory내의 각 함수들에 의해 이루어진다.
	inline void					SetPreviousUID( ItemUID uid )		{ m_Instance.PreviousUID = uid;		}
	inline ItemUID				GetPreviousUID()					{ return m_Instance.PreviousUID;	}

	inline AR_HANDLE			GetOwnerHandle() const				{ return m_Instance.OwnerHandle;	}
	inline void					SetOwnerInfo( AR_HANDLE handle, int UID, int nAccountID )				{ m_Instance.OwnerHandle = handle; m_Instance.nOwnerUID = UID; m_nAccountID = nAccountID; memset( &m_ItemPickupOrder, 0, sizeof( m_ItemPickupOrder ) );	}
	inline int					GetOwnerUID() const					{ return m_Instance.nOwnerUID;		}
	inline int					GetAccountID() const				{ return m_nAccountID;		}
	inline int					GetAuctionID() const				{ return m_Instance.nAuctionID;		}
	inline void					SetAuctionID( int nAuctionID )		{ m_Instance.nAuctionID = nAuctionID;	}
	inline int					GetItemKeepingID() const			{ return m_Instance.nItemKeepingID;		}
	inline void					SetItemKeepingID( int nItemKeepingID )		{ m_Instance.nItemKeepingID = nItemKeepingID;	}

	inline void					SetIdx( int nIdx )					{ m_Instance.nIdx = nIdx;	}
	inline int					GetIdx() const						{ return m_Instance.nIdx;	}
	inline bool					IsVirtualItem() const				{ return m_bIsVirtualItem;	}
	inline void					SetVirtualItem()					{ m_bIsVirtualItem = true;	}
	inline void					SetOwnSummonInfo( AR_HANDLE handle, int UID )				{ m_Instance.OwnSummonHandle = handle; m_Instance.nOwnSummonUID = UID; }
	inline int					GetOwnSummonUID() const				{ return m_Instance.nOwnSummonUID;	}
	inline AR_HANDLE			GetOwnSummonHandle() const			{ return m_Instance.OwnSummonHandle; }

	inline bool					IsInStorage() const					{ return ( GetAccountID() && !GetOwnerUID() && GetItemCode() != 0 ); } // gold 아이템은 안친다.-_-
	inline bool					IsInInventory() const				{ return ( !GetAccountID() && GetOwnerUID() ); }

	inline void						SetWearInfo( ItemBase::ItemWearType wear_info, const bool bSkipDBUpdate = false )
	{
		m_Instance.nWearInfo = wear_info;

		if( !bSkipDBUpdate )
			TurnOnUpdateFlag();
	}
	inline ItemBase::ItemWearType	GetWearInfo() const									{ return m_Instance.nWearInfo;					}	// 1000 이상일경우 소환수에 장착된것임.

	void						SetBindedCreatureHandle( AR_HANDLE target ) { m_hBindedTarget = target;	}
	AR_HANDLE					GetBindedCreatureHandle() const				{ return m_hBindedTarget;	}
	
	ItemBase::ItemCode			GetItemCode() const							{ return m_Instance.Code;						}
	const ItemBaseServer &		GetItemBase() const							{ return *m_pItemBase;							}
	float						GetWeight(const __int64& cnt) const			{ return cnt* GetItemBase().fWeight; }
	float						GetWeight() const							{ return GetCount()* GetItemBase().fWeight; }
	ItemBase::ITEM_TYPE			GetItemType() const							{ return static_cast< ItemBase::ITEM_TYPE >( GetItemBase().nType ); }
	ItemBase::ItemClass			GetItemClass() const						{ return GetItemBase().nClass;					}
	bool						IsWearable() const;
	ItemBase::ItemWearType		GetWearType() const							{ return GetItemBase().WearType;				}
	int							GetMaxSocketCount() const;
	int							GetUsingSocketCount() const;
	ItemBase::ItemCode			GetSocketCode( unsigned idx ) const	{ return m_Instance.Socket[idx];	}
	void						SetSocketCode( unsigned idx, ItemBase::ItemCode code )		{ m_Instance.Socket[idx] = code;	}

	void						SetSummonCode( int _nSummonCode ) { m_Instance.nSummonCode = _nSummonCode; }
	int							GetSummonCode() const;

	int							GetStateCode() const				{ return m_pItemBase->nStateCode;	}
	int							GetStateLevel() const				{ return m_pItemBase->nStateLevel;	}
	AR_TIME						GetStateTime() const				{ return m_pItemBase->nStateTime;	}
	AR_TIME						GetCoolTime() const					{ return m_pItemBase->nCoolTime * 100;	}
	int							GetCoolTimeGroup() const			{ return m_pItemBase->nCoolTimeGroup;	}

	// 소환수 카드 전용
	void						SetSummonSID( int sid, const bool bSkipDBUpdate = false )
	{
		if( !bSkipDBUpdate )
			TurnOnUpdateFlag();

		m_Instance.Socket[0] = sid;
	}
	int							GetSummonSID() const								{ return static_cast< int >( m_Instance.Socket[0] );	}
	void						SetSummonStruct( struct StructSummon* pSummon )		{ m_pSummon = pSummon; }
	struct StructSummon*		GetSummonStruct() const								{ return m_pSummon; }

	// 펫 우리 전용
	void						SetPetSID( const int sid )					{ m_Instance.Socket[0] = sid; TurnOnUpdateFlag();		}
	const int					GetPetSID() const							{ return static_cast< int >( m_Instance.Socket[0] );	}
	void						SetPetCode( const int code )				{ m_Instance.Socket[1] = code; TurnOnUpdateFlag();		}
	const int					GetPetCode() const							{ return static_cast< int >( m_Instance.Socket[1] );	}
	void						SetPetStruct( struct StructPet * pPet )		{ m_pPet = pPet; }
	struct StructPet *			GetPetStruct() const						{ return m_pPet; }

	void						SetEventDropFlag( bool bFlag )		{ m_bIsEventDrop = bFlag; }
	bool						GetEventDropFlag() const			{ return m_bIsEventDrop; }

	// (스킬카드일경우) 누구에게 바인딩 되었는지?
	int							GetSkillId() const					{ return m_pItemBase->nSkillID; } // TODO : 스킬카드일경우 스킬 번호 알려줘야함
	bool						IsBinded() const					{ return !!m_hBindedTarget; }
	void						SetBindTarget( struct StructCreature *pTarget, const bool bSkipDBUpdate = false );
	int							GetBindedPlayerSID() const		{ return static_cast< int >( m_Instance.Socket[0] );	}	
	int							GetBindedSummonSID() const		{ return static_cast< int >( m_Instance.Socket[1] );	}	

	bool						IsGold() const				{ return !GetItemCode();			}
	void						SetCount( const __int64 & c )		{ m_Instance.nCount = c; TurnOnUpdateFlag();	}
	const __int64				GetCount() const			{ return m_Instance.nCount;			}
	unsigned char				GetItemGrade() const		{ return GetItemBase().nGrade;		}
	int							GetItemRank() const			{ return GetItemBase().nRank;		}
	int							GetItemLevel() const		{ return m_Instance.nLevel % 100;			}
	void						SetItemLevel( int level )	{ m_Instance.nLevel = level; TurnOnUpdateFlag();	}
	int							GetItemEnhance() const		{ return m_Instance.nEnhance;		}
	void						SetItemEnhance( int enhance );
	int							GetEnhanceChance() const	{ return m_Instance.nLevel / 100;	}	
	
	// 에테리얼 내구도 관련
	const int					ProcEtherealDurabilityConsumption( const int nBaseConsumption, const c_fixed10 & fConsumeRate, const c_fixed10 & fEnvironmentalConsumeRate );
	const int					GetCurrentEtherealDurability() const					{ return m_Instance.nCurrentEtherealDurability; }
	void						SetCurrentEtherealDurability( const int nDurability )	{ m_Instance.nCurrentEtherealDurability = std::min( std::max( nDurability, 0 ), GetMaxEtherealDurability() ); TurnOnUpdateFlag(); }
	void						AddCurrentEtherealDurability( const int nAdd )
	{
		// 오버 플로우 체크
		__int64 nNewEtherealDurability = GetCurrentEtherealDurability();
		nNewEtherealDurability += nAdd;
		if( nNewEtherealDurability > INT_MAX )
		{
			assert( 0 );
			nNewEtherealDurability = INT_MAX;
		}
		SetCurrentEtherealDurability( int( nNewEtherealDurability ) );
	}
	const int					GetMaxEtherealDurability() const;

	int							GetCurrentEndurance() const	{ return m_Instance.nCurrentEndurance; }
	void						SetCurrentEndurance( int n );
	int							GetMaxEndurance() const;

	// 레벨제한
	int							GetLevelLimit() const;
	// 권장레벨 
	int							GetRecommendLevel() const;
	AR_UNIT						GetItemAttackRange() const	{ return AR_UNIT( GetItemBase().nRange ); }
	int							GetItemGroup() const		{ return GetItemBase().nGroup; }

	ItemBase::BaseFlag			GetBaseFlag() const			{ return GetItemBase().Flag; }
	inline ItemInstance::InstanceFlag &	GetInstanceFlag()	{ return m_Instance.Flag;			}
	inline const ItemInstance::InstanceFlag &	GetInstanceFlag() const	{ return m_Instance.Flag;			}
	void						SetInstanceFlagOn( int idx );
	void						SetInstanceFlagOff( int idx );

	bool						IsQuestItem()				{ return GetBaseFlag().IsOn( ItemBase::FLAG_QUEST ); }
	bool						IsQuestDistributeItem()		{ return GetBaseFlag().IsOn( ItemBase::FLAG_QUEST_DISTRIBUTE ); }

	inline ItemInstance::GenerateCode	GetGenerateCode() const		{ return m_Instance.GenerateInfo;	}

	// 기간제 아이템 관련
	bool						IsExpireItem() const;
	time_t						GetExpireTime() const		{ return m_Instance.tExpire; } // 단위 : 초
	void						SetRemainTime( time_t t )	{ m_Instance.tExpire = t; }

	// 기간제 속성 이펙트 관련
	const unsigned short		SetElementalEffect( const unsigned char & cType, const time_t & tExpire );
	const unsigned short		SetElementalEffectAttackPoint( const int nAttackPoint );
	const unsigned short		SetElementalEffectMagicPoint( const int nMagicPoint );
	void						ClearElementalEffect();
	// tElementalEffectExpire가 0일 경우는 무제한
	inline const unsigned char		GetElementalEffectType() const				{ return ( ( !m_Instance.tElementalEffectExpire || m_Instance.tElementalEffectExpire >= time( NULL ) ) ? m_Instance.cElementalEffectType : 0 ); }
	const time_t &					GetElementalEffectExpireTime() const		{ return m_Instance.tElementalEffectExpire; }
	const int						GetElementalEffectAttackPoint() const		{ return m_Instance.nElementalEffectAttackPoint; }
	const int						GetElementalEffectMagicPoint() const		{ return m_Instance.nElementalEffectMagicPoint; }

	// 형상 변환 관련
	const ItemBase::ItemCode		GetAppearanceCode() const										{ return m_Instance.nAppearanceCode; }
	const bool						SetAppearanceCode( const ItemBase::ItemCode nAppearanceCode )	{ m_Instance.nAppearanceCode = nAppearanceCode; return true; }
#if defined USE_ITEMEFFECT_96 || defined USE_CLIENT97 || defined USE_CLIENT96
	const int						GetEffectID() const												{ return m_Instance.nEffectID; }
#else
	const int						GetEffectID() const { return 0; }
#endif
	// 이하는 구현을 위해 존재하는 인터페이스임.

	void						DBQuery( GameDBManager::DBProc *pWork );
	void						onEndQuery();
	volatile size_t				GetDBQueryCount()	{ return m_lQueryList.size(); }

	inline bool					IsNeedUpdateToDB()			{ return ( m_nDBUpdateFlag != 0 );	}
	inline void					TurnOffDbUpdateFlag()		{ InterlockedExchange( &m_nDBUpdateFlag, 0 ); }
	inline void					TurnOnUpdateFlag()			{ InterlockedIncrement( &m_nDBUpdateFlag );	}
	
	inline void					SetDropTime( AR_TIME t )	{ m_nDropTime = t;						}
	inline AR_TIME				GetDropTime()				{ return m_nDropTime;					}

	static ItemBaseServer &		GetItemBase( ItemBase::ItemCode c );

	// 세트 아이템 전용
	int GetSetItemID()			{ return m_pItemBase->nSetID; }
	int GetSetItemPartFlag()	{ return m_pItemBase->nSetPartFlag; }

	// 아이템 랜덤 옵션 관련 함수들 (각성, 아이템옵션랜덤화)
	inline int			GetRandomOptionType( ItemInstance::RANDOM_TYPE eType, int nIndex ) const	{ return m_Instance.RandomOption[ eType ].OptionInfo[ nIndex ].nType; }
	inline c_fixed10	GetRandomOptionValue1( ItemInstance::RANDOM_TYPE eType, int nIndex ) const	{ return m_Instance.RandomOption[ eType ].OptionInfo[ nIndex ].fValue1; }
	inline c_fixed10	GetRandomOptionValue2( ItemInstance::RANDOM_TYPE eType, int nIndex ) const	{ return m_Instance.RandomOption[ eType ].OptionInfo[ nIndex ].fValue2; }
	inline int			GetRandomSID( ItemInstance::RANDOM_TYPE eType ) const						{ return m_Instance.RandomOption[ eType ].nSID; }
	inline void			SetRandomSID( ItemInstance::RANDOM_TYPE eType, int nSID )					{ m_Instance.RandomOption[ eType ].nSID = nSID; }
	inline bool			IsRandomOptionItem( ItemInstance::RANDOM_TYPE eType ) const					{ return !!m_Instance.RandomOption[ eType ].nSID; }
	bool 				SetRandomOption( ItemInstance::RANDOM_TYPE eType, ItemInstance::RANDOM_OPTION & RandomOption );
	ItemInstance::RANDOM_OPTION*		GetRandomOption( ItemInstance::RANDOM_TYPE eType );

	inline int			GetAwakenOptionType( int nIndex ) const										{ return GetRandomOptionType( ItemInstance::AWAKEN, nIndex ); }
	inline c_fixed10	GetAwakenOptionValue1( int nIndex ) const									{ return GetRandomOptionValue1( ItemInstance::AWAKEN, nIndex ); }
	inline c_fixed10	GetAwakenOptionValue2( int nIndex ) const									{ return GetRandomOptionValue2( ItemInstance::AWAKEN, nIndex ); }
	inline int			GetAwakenSID() const														{ return GetRandomSID( ItemInstance::AWAKEN ); }
	inline void			SetAwakenSID( int nSID )													{ return SetRandomSID( ItemInstance::AWAKEN, nSID ); }
	inline bool			IsAwaken() const															{ return IsRandomOptionItem( ItemInstance::AWAKEN ); }
	inline bool 		SetAwakenOption( ItemInstance::RANDOM_OPTION & AwakenOption )				{ return SetRandomOption( ItemInstance::AWAKEN, AwakenOption ); }

	inline int			GetIdentifiedOptionType( int nIndex ) const									{ return GetRandomOptionType( ItemInstance::IDENTIFIED, nIndex ); }
	inline c_fixed10	GetIdentifiedOptionValue1( int nIndex ) const								{ return GetRandomOptionValue1( ItemInstance::IDENTIFIED, nIndex ); }
	inline c_fixed10	GetIdentifiedOptionValue2( int nIndex ) const								{ return GetRandomOptionValue2( ItemInstance::IDENTIFIED, nIndex ); }
	inline int			GetIdentifiedSID() const													{ return GetRandomSID( ItemInstance::IDENTIFIED ); }
	inline void			SetIdentifiedSID( int nSID )												{ return SetRandomSID( ItemInstance::IDENTIFIED, nSID ); }
	inline bool			IsIdentified() const														{ return IsRandomOptionItem( ItemInstance::IDENTIFIED ); }
	inline bool 		SetIdentifiedOption( ItemInstance::RANDOM_OPTION & IdentifiedOption )		{ return SetRandomOption( ItemInstance::IDENTIFIED, IdentifiedOption ); }

protected:
	static void					deletePendingItem();
	static void					freeItem( StructItem* p );

	// ArScheduler 에서 StructItem 을 지우기 위해 존재함.
	// alloc/free 메커니즘에 관한 확실한 이해 전에는 수정 말것!
	virtual bool				ProcDelete();	

	AR_HANDLE					m_hHandle;	
	ItemInstance				m_Instance;

	int							m_nAccountID;

	unsigned					m_unInventoryIndex;	// 리스트에서의 위치

	bool						m_bIsEventDrop;
	bool						m_bIsVirtualItem;	
	volatile long				m_nDBUpdateFlag;
	ItemBaseServer*				m_pItemBase;

	struct StructSummon *		m_pSummon;
	struct StructPet *			m_pPet;

	AR_HANDLE					m_hBindedTarget;	// 스킬카드일경우

	AR_TIME						m_nDropTime;

	ITEM_PICKUP_ORDER			m_ItemPickupOrder;	// 아이템 집는 순서.

	XCriticalSection			m_bQueryLock;
	std::list< GameDBManager::DBProc* >	m_lQueryList;

	const char*					m_deleteFunction;
	int							m_deleteLine;

	StructItem( AR_HANDLE handle );
	virtual ~StructItem();
};