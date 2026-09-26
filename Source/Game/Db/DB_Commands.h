#pragma once

#include <string>

#include <mmo/ArType.h>

#include "GameDBManager.h"
#include "PlayerData.h"
#include "ItemInstance.h"
#include "AuctionBase.h"
#include "StructMisc.h"
#include "GuildManager.h"
#include "VersionDefs.h"

struct StructPlayer;
struct StructItem;
struct StructSummon;
struct StructPet;

struct TS_MESSAGE;

struct DB_CharacterUpdateInfo
{
	int	IN_SID;
	int IN_PARTY_SID;
	int IN_GUILD_SID;
	int IN_PREV_GUILD_SID;
	int IN_X;
	int IN_Y;
	int IN_Z;
	int IN_LAYER;
	int IN_LV;
	int IN_MAX_REACHED_LEVEL;
	__int64 IN_EXP;
	__int64	IN_LAST_DECREASED_EXP;
	int	IN_HP;
	int	IN_MP;
	int	IN_STAMINA;
	int	IN_JOB;
	int	IN_JOB_DEPTH;
	int	IN_JLV;
	__int64	IN_JP;
	__int64	IN_TOTAL_JP;
	int IN_TALENT_POINT;
	int IN_JOB_0;
	int	IN_JOB_1;
	int	IN_JOB_2;
	int	IN_JLV_0;
	int	IN_JLV_1;
	int	IN_JLV_2;
	c_fixed10	IN_IP;
	int IN_CHA;
	int	IN_PKC;
	int	IN_DKC;
	int IN_HUNTAHOLIC_POINT;
	int IN_HUNTAHOLIC_ENTER_COUNT;
	int	IN_ETHEREAL_STONE_DURABILITY;
	__int64	IN_GOLD;
	int IN_CHAOS;
	unsigned long IN_SKIN_COLOR;
	std::string IN_FLAG_LIST;
	int IN_SUMMON[6];
	__int64 IN_BELT[8];
	int IN_MAIN_SUMMON;
	int IN_SUB_SUMMON;
	int IN_REMAIN_SUMMON_TIME;
	int	IN_PET;
	int IN_MAIN_TITLE;
	int IN_SUB_TITLE[5];
	int IN_REMAIN_TITLE_TIME;
	int IN_ARENA_POINT;
	int IN_ARENA_MVP_COUNT;
	int IN_ARENA_RECORD[ 3 ][ 2 ];
	int IN_REMAIN_CHAT_BLOCK_TIME;
	int IN_ADV_CHAT_COUNT;
	int	IN_AUTO_USED;
	int IN_PKMODE;
	int IN_PLAY_TIME_POINT;
	int IN_RX;
	int IN_RY;
	int IN_HX;
	int IN_HY;
};

struct DB_SummonInfo
{
	int IN_SID;
	int IN_ACCOUNT_ID;
	int IN_OWNDER_ID;
	int IN_CODE;
	_bstr_t IN_NAME;
	int IN_LV;
	int IN_JLV;
	int IN_MAX_LEVEL;
	__int64 IN_EXP;
	int IN_JP;
	__int64 IN_LAST_DECREASED_EXP;
	int IN_HP;
	int IN_MP;
	int IN_SP;
	int IN_FP;
	int IN_PREV_LEVEL_01;
	int IN_PREV_LEVEL_02;
	int IN_PREV_ID_01;
	int IN_PREV_ID_02;
	int IN_TRANSFORM;
};

void	DateToString( DATE date, char* buffer, size_t size );

struct DB_OnStartUp				: GameDBManager::DBProc
{
	DB_OnStartUp() : DBProc( "DB_OnStartUp" )
	{
	}

	virtual bool onProcess( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

private:

	bool callStartupHandler( DBConnection & db );
	bool connectToBillingServer( DBConnection & db );
	bool readMaxItemIndex( DBConnection & db );
	bool readMaxSkillIndex( DBConnection & db );
	bool readMaxStateIndex( DBConnection & db );
	bool readDisabledStateList( DBConnection & db );
	bool readMaxSummonIndex( DBConnection & db );
	bool readMaxPetIndex( DBConnection & db );
	bool readMaxFarmIndex( DBConnection & db );
	bool readMaxTitleIndex( DBConnection & db );
	bool readMaxTitleConditionIndex( DBConnection & db );
	bool removeDeletedCharacter( DBConnection & db );
	bool readMaxRandomOptionIndex( DBConnection & db );

	bool proc( DBConnection & db );
};

struct UpdateOTPCompletionFunctor
{
	virtual ~UpdateOTPCompletionFunctor()				{}

	virtual void onComplete( const bool bSuccess )		= NULL;
	virtual void onEnd()								{ delete this; }
};

struct UpdateOTPCF_PendMessage : public UpdateOTPCompletionFunctor
{
	UpdateOTPCF_PendMessage( StructPlayer * pPlayer, const TS_MESSAGE * pMsg );
	virtual ~UpdateOTPCF_PendMessage();

	virtual void onComplete( const bool bSuccess );

	StructPlayer * m_pPlayer;
	TS_MESSAGE * m_pMsg;
};

struct UpdateOTPCF_SendOpenURL : public UpdateOTPCompletionFunctor
{
	UpdateOTPCF_SendOpenURL( StructPlayer * pPlayer, const char * pszURL, const bool bWaitForEventScene, const int nWidth = 0, const int nHeight = 0 );

	virtual void onComplete( const bool bSuccess );

	StructPlayer *		m_pPlayer;
	const std::string	m_strURL;
	const bool			m_bWaitForEventScene;
	const int			m_nWidth;
	const int			m_nHeight;
};

struct DB_UpdateOneTimePassword				: GameDBManager::DBProc
{
	DB_UpdateOneTimePassword( int nOwnerId, int nPassWord, UpdateOTPCompletionFunctor * pFo )
		: DBProc( "DB_UpdateOneTimePassword", true )
		, m_nOwnerId( nOwnerId )
		, m_nPassWord( nPassWord )
		, m_pFo( pFo )
	{
		assert( pFo );
	}

	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception )			{};

	virtual void onEnd( bool bIsCancel );

private:

	int m_nOwnerId, m_nPassWord;
	UpdateOTPCompletionFunctor * m_pFo;
};

struct DB_UpdateCharacter				: GameDBManager::DBProc
{
	DB_UpdateCharacter( struct StructPlayer * _pPlayer, bool bOnlyCharacter = false );

	virtual bool onProcess( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

private:

	bool		updateFavorInfo( DBConnection & db );
	bool		updateCharacterInfo( DBConnection & db );
	bool		updateItemCoolTime( DBConnection & db );

	bool		updateAccountFlagList( DBConnection & db );

	bool		m_bOnlyCharacter;
	struct		StructPlayer *pPlayer;

	struct		DB_CharacterUpdateInfo		CharacterInfo;
	
	std::vector< FAVOR_INFO >				vFavorInfo;

	// 계정 저장 정보
	std::string								m_strAccountFlagList;
	int										m_nAccountID;
};

struct DB_UpdateCharacterHair			: GameDBManager::DBProc
{
	DB_UpdateCharacterHair( StructPlayer * pPlayer )
		: DBProc( "DB_UpdateCharacterHair" )
		, m_pPlayer( pPlayer )
	{}

	virtual bool onProcess( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

private:
	StructPlayer *	m_pPlayer;
};

struct DB_UpdateCharacterSkin			: GameDBManager::DBProc
{
	DB_UpdateCharacterSkin( StructPlayer * pPlayer )
		: DBProc( "DB_UpdateCharacterSkin" )
		, m_pPlayer( pPlayer )
	{}

	virtual bool onProcess( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

private:
	StructPlayer * m_pPlayer;
};

struct DB_UpdateCharacterSex			: GameDBManager::DBProc
{
	DB_UpdateCharacterSex( StructPlayer * pPlayer )
		: DBProc( "DB_UpdateCharacterSex" )
		, m_pPlayer( pPlayer )
	{}

	virtual bool onProcess( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

private:
	StructPlayer * m_pPlayer;
};

struct DB_UpdateCharacterHideEquipFlag	: GameDBManager::DBProc
{
	DB_UpdateCharacterHideEquipFlag( StructPlayer * pPlayer )
		: DBProc( "DB_UpdateCharacterHideEquipFlag" )
		, m_pPlayer( pPlayer )
	{}

	virtual bool onProcess( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

private:
	StructPlayer *	m_pPlayer;
};

// 캐릭터 단위 이벤트 관련
struct DB_UpdatePlayTimePoint			: GameDBManager::DBProc
{
	DB_UpdatePlayTimePoint( StructPlayer *pPlayer, int nPlayTimeInMin, int nPoint ) : DBProc( "DB_UpdatePlayTimePoint" ), m_pPlayer( pPlayer ), m_nPlayTimeInMin( nPlayTimeInMin ), m_nPoint( nPoint )
	{
	}

	virtual bool onProcess( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

private:
	StructPlayer *	m_pPlayer;
	int				m_nPlayTimeInMin;
	int				m_nPoint;
};


// DB 플레이타임 이벤트
struct DB_UpdateTimeBasedEvent			: GameDBManager::DBProc
{
	DB_UpdateTimeBasedEvent( StructPlayer *pPlayer )
		: DBProc( "DB_UpdateTimeBasedEvent" )
		, m_pPlayer( pPlayer )
	{
	}

	virtual bool onProcess( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

private:
	StructPlayer *	m_pPlayer;
};


struct DB_CheckCharacterName	: GameDBManager::DBProc
{
	DB_CheckCharacterName( const char * _szCharacterName, struct IStreamSocketConnection* _pConnection ) : DBProc( "DB_CheckCharacterName", true ), pConnection( _pConnection )
	{
		szCharacterName = _szCharacterName;
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

private:
	struct		IStreamSocketConnection *pConnection;
	_bstr_t		szCharacterName;

};

struct DB_ChangeCharacterName	: GameDBManager::DBProc
{
	DB_ChangeCharacterName( const char * szCharacterName, struct StructPlayer * pPlayer, const bool bRemoveFromFriendAndDenial, const bool bIgnoreNameChangeCount, const AR_HANDLE & hNameChangeItem )
		: DBProc( "DB_ChangeCharacterName", true )
		, m_pPlayer( pPlayer )
		, m_szCharacterName( szCharacterName )
		, m_bRemoveFromFriendAndDenial( bRemoveFromFriendAndDenial )
		, m_bIgnoreNameChangeCount( bIgnoreNameChangeCount )
		, m_hNameChangeItem( hNameChangeItem )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

private:
	struct StructPlayer *	m_pPlayer;
	_bstr_t					m_szCharacterName;
	bool					m_bRemoveFromFriendAndDenial;
	bool					m_bIgnoreNameChangeCount;
	AR_HANDLE				m_hNameChangeItem;
};

struct DB_ChangeCharacterAlias	: GameDBManager::DBProc
{
	DB_ChangeCharacterAlias( struct StructPlayer * pPlayer, const char * szAlias, bool bByUserRequest )
		: DBProc( "DB_ChangeCharacterAlias", true )
		, m_pPlayer( pPlayer )
		, m_szAlias( szAlias )
		, m_bByUserRequest( bByUserRequest )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

private:
	struct StructPlayer *	m_pPlayer;
	_bstr_t					m_szAlias;
	bool					m_bByUserRequest;
};

struct DB_UpdateRace	: GameDBManager::DBProc
{
	DB_UpdateRace( struct StructPlayer * pPlayer )
		: DBProc( "DB_UpdateRace" )
		, m_pPlayer( pPlayer )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

private:
	struct StructPlayer *	m_pPlayer;
};

struct DB_Login					: GameDBManager::DBProc
{
	DB_Login( const char * _szCharacterName, int account_id, struct StructPlayer *pPlayer, char nPCBangMode, int nEventCode, int nAge, AR_TIME nContinuousPlayTime, AR_TIME nContinuousLogoutTime, struct IStreamSocketConnection* _pConnection )
		: DBProc( "DB_Login", true )
		, sid( -1 )
		, pConnection( _pConnection )
		, nAccountID( account_id )
		, m_pPlayer( pPlayer )
		, m_nPCBangMode( nPCBangMode )
		, m_nEventCode( nEventCode )
		, m_nAge( nAge )
		, m_nContinuousPlayTime( nContinuousPlayTime )
		, m_nContinuousLogoutTime( nContinuousLogoutTime )
		, logout_duration( 0 )
		, bWasInDeathmatch( false )
		, bBattleArenaReconnectSucceed( false )
		, bWasInBattleArena( false )
		, bBattleArenaOfflinePenaltyReceived( false )
	{
		s_strcpy( szCharacterName, _countof( szCharacterName ), _szCharacterName );
		memset( bindSummon, 0, sizeof(bindSummon) );		
		memset( beltSlot, 0, sizeof(beltSlot) );
		vFarmedSummonCard.reserve( GameRule::FARM_MAX_COUNT * 2 );

		mainSummon = 0;
		subSummon = 0;
		remainSummonTime = 0;
		pet = 0;
		main_title = 0;
		::memset( sub_title, 0, sizeof( sub_title ) );
		remain_title_time = 0;
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

private:

	bool					readFavorList( DBConnection & db );
	bool					readMaxQuestId( DBConnection & db );
	bool					readQuestList( DBConnection & db );
	bool					readQuestCoolTimeList( DBConnection & db );
	bool					readTitleList( DBConnection & db );
	bool					readTitleConditionList( DBConnection & db);
	const unsigned short	readCharacterInfo( DBConnection & db );
	bool					readPlayTime( DBConnection & db );
	bool					readItemList( DBConnection & db );
	bool					readSummonList( DBConnection & db );
	bool					readFarmedSummonInfoList( DBConnection & db );
	bool					readPetList( DBConnection & db );
	bool					readEquipItemList( DBConnection & db );
	bool					readStateInfo( DBConnection & db, StructSummon * pSummon );
	bool					setBeltSlotInfo();
	bool					readItemCoolTime( DBConnection & db );
	bool					readRandomQuestList( DBConnection & db );
	bool					readFriendsList( DBConnection & db );
	bool					readFriendOfsList( DBConnection & db );
	bool					readDenialsList( DBConnection & db );
	bool					readDenialOfsList( DBConnection & db );
	bool					readEventAreaEnterCount( DBConnection & db );
	bool					readRandomOptionList( DBConnection & db );
	bool					_readClientInfo( DBConnection & db );
	void					_readAccountFlag( DBConnection & db );

	void					getRandomOption( int _SID, ItemInstance::RANDOM_OPTION * _pRandomOption );

	struct					IStreamSocketConnection *pConnection;
	struct					StructPlayer *m_pPlayer;
	char					szCharacterName[31];
	char					m_nPCBangMode;
	int						m_nEventCode;
	int						m_nAge;
	AR_TIME					m_nContinuousPlayTime;
	AR_TIME					m_nContinuousLogoutTime;
	int						nAccountID;
	int						sid;

	int						bindSummon[ 6 ];
	int						mainSummon;
	int						subSummon;
	int						remainSummonTime;
	int						pet;
	int						main_title;
	int						sub_title[5];
	int						remain_title_time;
	int						logout_duration;
	AR_TIME					item_cool_time[20];

	ItemUID	beltSlot[ 8 ];

	bool										bWasInDeathmatch;
	bool										bBattleArenaReconnectSucceed;
	bool										bWasInBattleArena;
	bool										bBattleArenaOfflinePenaltyReceived;
	std::vector< struct StructItem * >			vFarmedSummonCard;
	std::vector< ItemInstance::RANDOM_OPTION >	vRandomOptionList;
};


struct DB_UpdateClientInfo		: GameDBManager::DBProc
{
	DB_UpdateClientInfo( struct StructPlayer * pPlayer, void *pPtr ) : DBProc( "DB_UpdateClientInfo" ), m_pPlayer( pPlayer )
	{
		s_memcpy( szValue, sizeof( szValue ), pPtr, sizeof( szValue ) - 1 );
		szValue[ _countof( szValue ) - 1 ] = '\0';
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	struct StructPlayer *	m_pPlayer;
	char					szValue[4096];
};

struct DB_UpdateQuickSlot		: GameDBManager::DBProc
{
	DB_UpdateQuickSlot( struct StructPlayer * pPlayer, void *pPtr ) : DBProc( "DB_UpdateQuickSlot" ), m_pPlayer( pPlayer )
	{
		s_memcpy( szValue, sizeof( szValue ), pPtr, sizeof( szValue ) - 1 );
		szValue[ _countof( szValue ) - 1 ] = '\0';
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	struct StructPlayer *	m_pPlayer;
	char					szValue[4096];
};

struct DB_UpdateCurrentKey		: GameDBManager::DBProc
{
	DB_UpdateCurrentKey( struct StructPlayer * pPlayer, void *pPtr ) : DBProc( "DB_UpdateCurrentKey" ), m_pPlayer( pPlayer )
	{
		s_memcpy( szValue, sizeof( szValue ), pPtr, sizeof( szValue ) - 1 );
		szValue[ _countof( szValue ) - 1 ] = '\0';
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	struct StructPlayer *	m_pPlayer;
	char					szValue[4096];
};

struct DB_UpdateSavedKey		: GameDBManager::DBProc
{
	DB_UpdateSavedKey( struct StructPlayer * pPlayer, void *pPtr ) : DBProc( "DB_UpdateSavedKey" ), m_pPlayer( pPlayer )
	{
		s_memcpy( szValue, sizeof( szValue ), pPtr, sizeof( szValue ) - 1 );
		szValue[ _countof( szValue ) - 1 ] = '\0';
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	struct StructPlayer *	m_pPlayer;
	char					szValue[4096];
};

struct DB_Logout				: GameDBManager::DBProc
{
	DB_Logout( struct StructPlayer * pPlayer ) : DBProc( "DB_Logout" ), m_pPlayer( pPlayer )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );
private:

	struct StructPlayer * m_pPlayer;

};


struct DB_UpdateItemOwner		: GameDBManager::DBProc
{
	DB_UpdateItemOwner( struct StructItem * pItem ) : DBProc( "DB_UpdateItemOwner" ), m_pItem( pItem )	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );	

	struct StructItem * m_pItem;
};

struct DB_UpdateItemCode		: GameDBManager::DBProc
{
	DB_UpdateItemCode( struct StructItem * pItem ) : DBProc( "DB_UpdateItemCode" ), m_pItem( pItem ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	struct StructItem *	m_pItem;
};

struct DB_InsertSummon		: GameDBManager::DBProc
{
	DB_InsertSummon( struct StructPlayer * pMaster, int _sid, int _account_id, int _owner_id, int _code, __int64 _card_uid, const char * szName, int _sp, int _hp, int _mp, __int64 _jp )
		: DBProc( "DB_InsertSummon" ), m_pMaster(pMaster), sid(_sid), account_id(_account_id), owner_id(_owner_id), code(_code), card_uid(_card_uid), strName(szName), sp(_sp), hp(_hp), mp(_mp), jp(_jp) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

private:

	bool proc( DBConnection & db );	

	StructPlayer * m_pMaster;

	int sid;
	int account_id;
	int owner_id;
	int code;
	__int64 card_uid;
	_bstr_t strName;
	int sp;
	int hp;
	int mp;
	__int64 jp;
};

struct DB_ReadStorageList		: GameDBManager::DBProc
{
	DB_ReadStorageList( struct StructPlayer * pPlayer, const bool bReload ) : DBProc( "DB_ReadStorageList", true ), m_pPlayer( pPlayer ), m_bReload( bReload ) {}

	bool readSummonStateInfo( DBConnection & db, StructSummon * pSummon );
	bool readStorageSummonList( DBConnection & db, std::vector<StructSummon *> & vSummonList );
	bool readStoragePetList( DBConnection & db, std::vector<StructPet *> & vPetList );
	bool readStorageRandomOptionList( DBConnection & db, std::vector< ItemInstance::RANDOM_OPTION > & vRandomOptionList );
	void getRandomOption( std::vector< ItemInstance::RANDOM_OPTION > & vRandomOptionList, int _SID, ItemInstance::RANDOM_OPTION *_pRandomOption );
	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:

	bool proc( DBConnection & db );	

	struct StructPlayer *	m_pPlayer;
	bool					m_bReload;
};

struct DB_ReadCommercialStorageList		: GameDBManager::DBProc
{
	DB_ReadCommercialStorageList	( struct StructPlayer * pPlayer ) : DBProc( "DB_ReadCommercialStorageList", true ), m_pPlayer( pPlayer ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:

	bool proc( DBConnection & db );	

	struct StructPlayer * m_pPlayer;
};

struct DB_TakeOutCommercialItem		: GameDBManager::DBProc
{
	DB_TakeOutCommercialItem( struct StructPlayer * pPlayer, unsigned int item_uid, const int & item_count )
		: DBProc( "DB_TakeOutCommercialItem", true )
		, m_pPlayer( pPlayer )
		, m_unItemCID( item_uid )
		, m_nCount( item_count )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:

	bool proc( DBConnection & db );

	unsigned int		  m_unItemCID;
	int					  m_nCount;
	struct StructPlayer * m_pPlayer;
};

struct DB_GetCommercialStorageInfo		: GameDBManager::DBProc
{
	DB_GetCommercialStorageInfo	( struct StructPlayer * pPlayer, bool bIsProcessingLogin ) : DBProc( "DB_GetCommercialStorageInfo", true ), m_pPlayer( pPlayer ), m_bIsProcessingLogin( bIsProcessingLogin ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:

	struct StructPlayer * m_pPlayer;
	bool m_bIsProcessingLogin;
};

struct DB_ReadAccountAuthorityInfo		: GameDBManager::DBProc
{
	DB_ReadAccountAuthorityInfo ( struct StructPlayer * pPlayer ) : DBProc( "DB_ReadAccountAuthorityInfo", true ), m_pPlayer( pPlayer ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

	bool syncronizeWithBillingDBForSecroute( DBConnection & db );

private:

	struct StructPlayer * m_pPlayer;
};

struct DB_SetAccountAuthorityInfo		: GameDBManager::DBProc
{
	DB_SetAccountAuthorityInfo ( struct StructPlayer * pPlayer, int authority_type, int duration ) : DBProc( "DB_SetAccountAuthorityInfo", true ), m_pPlayer( pPlayer ), m_nAuthorityType( authority_type ), m_nDuration( duration ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:

	struct StructPlayer * m_pPlayer;
	int	m_nAuthorityType;
	int m_nDuration;
};

struct DB_CloseAccountAuthorityInfo		: GameDBManager::DBProc
{
	DB_CloseAccountAuthorityInfo ( struct StructPlayer * pPlayer, int authority_type ) : DBProc( "DB_SetAccountAuthorityInfo", true ), m_pPlayer( pPlayer ), m_nAuthorityType( authority_type ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:

	struct StructPlayer * m_pPlayer;
	int	m_nAuthorityType;
};

struct DB_InsertItem		: GameDBManager::DBProc
{
	DB_InsertItem( struct StructItem * pItem );

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

	static bool insertItemToDB( DBConnection & db , ItemUID uid, int owner_id, int account_id, int summon_id, int auction_id, int keeping_id, int code, ItemUID previous_uid, int flag, const __int64 & cnt, int level, int enhance, int ehtereal_durabtility, int endurance, int socket_0, int socket_1, int socket_2, int socket_3, int gcode, int remain_time, unsigned char elemental_effect_type, time_t elemental_effect_expire_time, int elemental_effect_attack_point, int elemental_effect_magic_point, const ItemBase::ItemCode appearance_code, int summon_code, int effect_id );

private:

	bool proc( DBConnection & db );

	struct StructItem *m_pItem;
};

struct DB_InsertFavor		: GameDBManager::DBProc
{
	DB_InsertFavor( struct StructPlayer * pPlayer, int favor_id, int favor ) : DBProc( "DB_InsertFavor" ), m_pPlayer( pPlayer ), m_nFavorId( favor_id ), m_nFavor( favor ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

private:

	bool proc( DBConnection & db );

	struct StructPlayer * m_pPlayer;
	int m_nFavorId;
	int m_nFavor;
};

struct DB_UpdateItem		: GameDBManager::DBProc
{
	DB_UpdateItem( struct StructItem * pItem, const bool bCountOnly = false ) : DBProc( "DB_UpdateItem" ), m_pItem( pItem ), m_bCountOnly( bCountOnly )	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

	bool update_item( DBConnection & db, struct StructItem * pItem );
	bool update_item_count( DBConnection & db, ItemUID uid, const __int64 & count );

private:
	bool proc( DBConnection & db );

	struct StructItem *		m_pItem;
	bool					m_bCountOnly;
};

struct DB_UpdateSummon		: GameDBManager::DBProc
{
	DB_UpdateSummon( struct StructSummon * pSummon );

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	struct StructSummon * m_pSummon;
	struct DB_SummonInfo SummonInfo;
};

struct DB_InsertPet		: GameDBManager::DBProc
{
	DB_InsertPet( struct StructPet * pPet );

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	struct StructPet * m_pPet;

	int				sid;
	int				account_id;
	int				owner_id;

	__int64			cage_uid;

	int				code;
	bool			name_changed;
	_bstr_t			name;
};

struct DB_UpdatePet		: GameDBManager::DBProc
{
	DB_UpdatePet( struct StructPet * pPet ) : DBProc( "DB_UpdatePet" ), m_pPet( pPet ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	struct StructPet * m_pPet;
};

struct DB_ChangePetName		: GameDBManager::DBProc
{
	DB_ChangePetName( struct StructPet * pPet )
		: DBProc( "DB_ChangePetName" )
		, m_pPet( pPet )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	struct StructPet *	m_pPet;
};

struct DB_CharacterList					: GameDBManager::DBProc
{
	DB_CharacterList( int account_id, struct IStreamSocketConnection* _pConnection ) : DBProc( "DB_CharacterList", true ), nAccountID( account_id ),  pConnection( _pConnection )
	{
	}

	virtual void				onEnd( bool bIsCancel )						{ delete this; };

	virtual bool				onProcess( DBConnection & db );

	virtual const std::string	getDebugInfo();
	virtual void				onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		IStreamSocketConnection *pConnection;
	int			nAccountID;
};


struct DB_DeleteCharacter					: GameDBManager::DBProc
{
	DB_DeleteCharacter( const char * _szAccountName, const char * _szCharacterName, int _nAccountID, struct IStreamSocketConnection* _pConnection )
		: DBProc( "DB_DeleteCharacter", true ), pConnection( _pConnection ), nAccountID( _nAccountID ), szCharacterName( _szCharacterName )
	{
		s_strcpy( szAccountName, _countof( szAccountName ), _szAccountName );
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		IStreamSocketConnection *pConnection;

	int			nAccountID;
	char		szAccountName[GameRule::MAX_ACCOUNT_LEN + 1];
	_bstr_t		szCharacterName;
};


struct DB_CreateCharacter					: GameDBManager::DBProc
{
	DB_CreateCharacter( struct IStreamSocketConnection* _pConnection,
						const char * _szCharacterName,
						const char * _szAccountName,
						int	account_id,
						int race,
						int sex,
						unsigned long skin_color,
						int model_00,
						int model_01,
						int model_02,
						int model_03,
						int model_04,
						int hair_color_index,
						unsigned int nHairColorRGB,
						int texture_id,
						int _wear_item
						) : DBProc( "DB_CreateCharacter", true ), nAccountID( account_id ), nRace( race ), nSex( sex ), nSkinColor( skin_color ), nHairColorIndex( hair_color_index ), nHairColorRGB (nHairColorRGB), nTextureId( texture_id ), wear_item( _wear_item ), pConnection( _pConnection ), nOwnerID( 0 )
	{

		szCharacterName = _szCharacterName;

		s_strcpy( szAccountName, _countof( szAccountName ), _szAccountName );

		nModelId[0] = model_00;
		nModelId[1] = model_01;
		nModelId[2] = model_02;
		nModelId[3] = model_03;
		nModelId[4] = model_04;
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

	static bool setDefaultClientInfo( DBConnection & db, int nOwnerID );

private:
	bool insertCharacter( DBConnection & db );
	bool insertBasicItem( DBConnection & db );

	struct		IStreamSocketConnection *pConnection;
	_bstr_t		szCharacterName;
	char		szAccountName[GameRule::MAX_ACCOUNT_LEN + 1];

	int			nAccountID;
	int			nOwnerID;
	int			nRace;
	int			nSex;
	unsigned long		nSkinColor;
	int			nModelId[5];
	int			nHairColorIndex;
	int			nHairColorRGB;
	int			nTextureId;
	int			wear_item;
};

struct DB_DeleteParty			: GameDBManager::DBProc
{
	DB_DeleteParty( int nPartyID ) : DBProc( "DB_DeleteParty" ) , m_nPartyID( nPartyID )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	int			m_nPartyID;
};

struct DB_SetParty			: GameDBManager::DBProc
{
	DB_SetParty( const char *_szCharacterName, int nPartyID ) : DBProc( "DB_SetParty" ) , m_nPartyID( nPartyID )
	{
		m_szCharacterName = _szCharacterName;
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	int			m_nPartyID;
	_bstr_t		m_szCharacterName;
};

struct DB_InsertParty			: GameDBManager::DBProc
{
	DB_InsertParty( const char *_szPartyName, int nPartySID, int nLeaderSID, int nPartyType ) : DBProc( "DB_InsertParty" ) , m_nPartySID( nPartySID ), m_nLeaderSID( nLeaderSID ), m_nPartyType( nPartyType )
	{
		m_szPartyName = _szPartyName;
	}

	bool proc( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	int			m_nPartySID;
	int			m_nLeaderSID;
	_bstr_t		m_szPartyName;
	int			m_nPartyType;
};


struct DB_UpdatePartyInfo			: GameDBManager::DBProc
{
	DB_UpdatePartyInfo( int nPartyID, int nLeaderId, int nShareMode, int nPartyType, int nLeadPartyID ) : DBProc( "DB_UpdatePartyInfo" ) , m_nPartyID( nPartyID ), m_nLeaderId( nLeaderId ), m_nShareMode( nShareMode ), m_nPartyType( nPartyType ), m_nLeadPartyID( nLeadPartyID )
	{
	}

	bool proc( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	int			m_nPartyID;
	int			m_nLeaderId;
	int			m_nShareMode;
	int			m_nPartyType;
	int			m_nLeadPartyID;
};

struct DB_InsertSkill			: GameDBManager::DBProc
{
	DB_InsertSkill( struct StructCreature * pCreature, int nUID, int nOwnerID, int nNeedJP, int nSummonID, int nSkillID, int nSkillLevel, int nCoolTime ) : DBProc( "DB_InsertSkill" ) , m_pCreature( pCreature ), m_nUID( nUID ), m_nOwnerID( nOwnerID) , m_nJpDec( nNeedJP ), m_nSummonID( nSummonID) , m_nSkillID( nSkillID) , m_nSkillLevel( nSkillLevel) , m_nCoolTime( nCoolTime) 
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	struct StructCreature * m_pCreature;

	int	m_nUID;
	int m_nOwnerID;
	int m_nJpDec;
	int m_nSummonID;
	int m_nSkillID;
	int m_nSkillLevel;
	int m_nCoolTime;    
};

struct DB_UpdateStorageGold		: GameDBManager::DBProc
{
	DB_UpdateStorageGold( struct StructPlayer *pPlayer ) : DBProc( "DB_UpdateStorageGold" ), m_pPlayer( pPlayer )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

	struct StructPlayer *m_pPlayer;
};

struct DB_ResetSkill			: GameDBManager::DBProc
{
	DB_ResetSkill( struct StructCreature * pCreature, int nOwnerID, int nSummonID ) : DBProc( "DB_ResetSkill" ) , m_pCreature( pCreature ), m_nOwnerID( nOwnerID ), m_nSummonID( nSummonID )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	struct StructCreature * m_pCreature;
	int m_nOwnerID;
	int m_nSummonID;
};

struct DB_DeleteSkill			: GameDBManager::DBProc
{
	DB_DeleteSkill( struct StructCreature * pCreature, int nSID ) : DBProc( "DB_DeleteSkill" ) , m_pCreature( pCreature ), m_nSID( nSID)
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	struct StructCreature * m_pCreature;

	int m_nSID;
};

struct DB_UpdateSkill			: GameDBManager::DBProc
{
	DB_UpdateSkill( struct StructCreature * pCreature, int nSID, int nSkillLevel, int nCoolTime ) : DBProc( "DB_UpdateSkill" ) , m_pCreature( pCreature ), m_nSID( nSID), m_nSkillLevel( nSkillLevel) , m_nCoolTime( nCoolTime)
	{
	}

	bool UpdateSkill( DBConnection & db, int nSID, int nSkillLevel, int nCoolTime );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	struct StructCreature * m_pCreature;

	int m_nSID;
	int m_nSkillLevel;
	int m_nCoolTime;    
};

struct DB_UpdateState			: GameDBManager::DBProc
{
	DB_UpdateState( struct StructCreature * pCreature, struct StructState * pState )
		: DBProc( "DB_UpdateState" )
		, m_pCreature( pCreature )
		, m_pPlayer( NULL )
		, m_pSummon( NULL )
		, m_nOwnerID( 0 )
		, m_nSummonID( 0 )
		, m_bInsert( !pState->IsInDB() )
		, t( GetArTime() )
	{
		SetCommonStateDBParameters( pState );
		
		pState->SetLastSavedTime( t );
	}

	void SetCommonStateDBParameters( StructState * pState );
	bool InsertState( DBConnection & db );
	bool UpdateState( DBConnection & db );
	bool proc( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	struct StructCreature * m_pCreature;
	struct StructPlayer * m_pPlayer;
	struct StructSummon * m_pSummon;
	int m_nSID;
	int m_nOwnerID;
	int m_nSummonID;
	int m_nCode;
	unsigned short m_nLevel;
	int m_nDuration;
	int m_nRemainTime;
	int m_nBaseDamage;
	int m_nRemainFireTime;
	int m_nStateValue;
	std::string m_strStateStringValue;
	int m_nEnable;
	bool m_bInsert;
	AR_TIME t;
};

struct DB_DeleteGuild			: GameDBManager::DBProc
{
	DB_DeleteGuild( int nGuildId ) : DBProc( "DB_DeleteGuild" ) , m_nGuildId( nGuildId )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	int			m_nGuildId;
};


struct DB_ChangeGuildName			: GameDBManager::DBProc
{
	DB_ChangeGuildName( const char *_szNewName, int nGuildId, StructPlayer * pPlayer )
		: DBProc( "DB_ChangeGuildName", true )
		, m_nGuildId( nGuildId )
		, m_szNewName( _szNewName )
		, m_pPlayer( pPlayer )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	int				m_nGuildId;
	_bstr_t			m_szNewName;
	StructPlayer *	m_pPlayer;
};

struct DB_SetGuild			: GameDBManager::DBProc
{
	DB_SetGuild( const PlayerUID & nPlayerId, int nGuildId, int nPrevGuildId ) : DBProc( "DB_SetGuild" ), m_nPlayerId( nPlayerId ), m_nGuildId( nGuildId ), m_nPrevGuildId( nPrevGuildId )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	int			m_nPlayerId;
	int			m_nGuildId;
	int			m_nPrevGuildId;
};

struct DB_SetGuildBlockTime			: GameDBManager::DBProc
{
	DB_SetGuildBlockTime( const PlayerUID & nPlayerId, time_t guild_block_time ) : DBProc( "DB_SetGuildBlockTime" ), m_nPlayerId( nPlayerId ), m_tGuildBlockTime( guild_block_time )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	int			m_nPlayerId;
	time_t		m_tGuildBlockTime;
};

struct DB_UpdateGuildDonationPoint			: GameDBManager::DBProc
{
	DB_UpdateGuildDonationPoint( const int nGuildID, const int nPoint ) : DBProc( "DB_UpdateGuildDonationPoint" ), m_nGuildID( nGuildID ), m_nPoint( nPoint )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int		m_nGuildID;
	int		m_nPoint;
};

struct DB_UpdateGuildGradePoint : GameDBManager::DBProc
{
	DB_UpdateGuildGradePoint(const int nGuildID, const int nGrade, const int nPoint) : DBProc("DB_UpdateGuildGradePoint"), m_nGuildID(nGuildID), m_nGrade(nGrade), m_nPoint(nPoint)
	{
	}

	virtual void onEnd(bool bIsCancel) { delete this; };

	virtual bool onProcess(DBConnection& db);

private:
	bool proc(DBConnection& db);

	int		m_nGuildID;
	int		m_nGrade;
	int		m_nPoint;
};

struct DB_UpdateGuildMemberPoint : GameDBManager::DBProc
{
	DB_UpdateGuildMemberPoint(const int nPlayerSID, const int nGuildPoint, const int nGuildTotalPoint) : DBProc("DB_UpdateGuildMemberPoint"), m_nPlayerSID(nPlayerSID), m_nGuildPoint(nGuildPoint), m_nGuildTotalPoint(nGuildTotalPoint)
	{
	}

	virtual void onEnd(bool bIsCancel) { delete this; };

	virtual bool onProcess(DBConnection& db);

private:
	bool proc(DBConnection& db);

	AR_HANDLE	m_nPlayerSID;
	int			m_nGuildPoint;
	int			m_nGuildTotalPoint;
};

struct DB_InsertEventAreaEnterCount			: GameDBManager::DBProc
{
	DB_InsertEventAreaEnterCount( StructPlayer * pPlayer, const int nEventAreaID, const int nEnterCount );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

	StructPlayer *				m_pPlayer;
	PlayerUID					m_nPlayerID;
	int							m_nEventAreaID;
	int							m_nEnterCount;
};

struct DB_UpdateEventAreaEnterCount			: GameDBManager::DBProc
{
	DB_UpdateEventAreaEnterCount( StructPlayer * pPlayer, const int nEventAreaID, const int nEnterCount );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

	StructPlayer *				m_pPlayer;
	PlayerUID					m_nPlayerID;
	int							m_nEventAreaID;
	int							m_nEnterCount;
};

struct DB_InsertFriend			: GameDBManager::DBProc
{
	DB_InsertFriend( const char *szFriendID, bool bIsDenial, struct StructPlayer * pPlayer );

	bool proc( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:

	_bstr_t m_strOwnerID;
	_bstr_t m_strFriendID;
	bool m_bIsDenial;
	struct StructPlayer * m_pPlayer;
};

struct DB_DeleteFriend			: GameDBManager::DBProc
{
	DB_DeleteFriend( const char * szOwnerID, const char *szFriendID, bool bIsDenial, struct StructPlayer * pPlayer );

	bool proc( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:

	_bstr_t m_strOwnerID;
	_bstr_t m_strFriendID;
	bool m_bIsDenial;
	struct StructPlayer * m_pPlayer;
};

struct DB_InsertGuild			: GameDBManager::DBProc
{
	DB_InsertGuild( GuildManager::GuildInfo* pInfo, const char * pszGuildName, const int nGuildSID, const int nLeaderSID, AR_HANDLE hLeader, const char nAdvertiseType,
		const bool bRecruiting, const short nMinRecruitLevel, const short nMaxRecruitLevel,
		const char * pszPermissionName1, const char * pszPermissionName2, const char * pszPermissionName3,
		const char * pszPermissionName4, const char * pszPermissionName5, const char * pszPermissionName6 )

		: DBProc( "DB_InsertGuild" ), m_pInfo( pInfo ), m_szGuildName( pszGuildName ), m_nGuildSID( nGuildSID ), m_nLeaderSID( nLeaderSID ), m_hLeader( hLeader ), m_nAdvertiseType( nAdvertiseType ),
		m_bRecruiting( bRecruiting ), m_nMinRecruitLevel( nMinRecruitLevel ), m_nMaxRecruitLevel( nMaxRecruitLevel ),
		m_szPermissionName1( pszPermissionName1 ), m_szPermissionName2( pszPermissionName2 ), m_szPermissionName3( pszPermissionName3 ),
		m_szPermissionName4( pszPermissionName4 ), m_szPermissionName5( pszPermissionName5 ), m_szPermissionName6( pszPermissionName6 )
	{
		// 광고 기간은 있는 DB에 설정되지 않기 때문에 길드 생성 시 광고가 등록되는 광고 타입이 들어오면 안 됨
		// ADV_TYPE_NONE = 0, ADV_TYPE_LIST_ONLY = 1
		if( m_nAdvertiseType != 0 && m_nAdvertiseType != 1 )
	{
			assert( 0 );
			m_nAdvertiseType = 1;
		}
	}

	bool proc( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	GuildManager::GuildInfo*	m_pInfo;
	int			m_nGuildSID;
	_bstr_t		m_szGuildName;
	int			m_nLeaderSID;
	AR_HANDLE	m_hLeader;
	char		m_nAdvertiseType;
	bool		m_bRecruiting;
	short		m_nMinRecruitLevel;
	short		m_nMaxRecruitLevel;
	_bstr_t		m_szPermissionName1;
	_bstr_t		m_szPermissionName2;
	_bstr_t		m_szPermissionName3;
	_bstr_t		m_szPermissionName4;
	_bstr_t		m_szPermissionName5;
	_bstr_t		m_szPermissionName6;
};

struct DB_SetGuildGold			: GameDBManager::DBProc
{
	DB_SetGuildGold( int nGuildId, const StructGold & nGold, int nChaos ) : DBProc( "DB_SetGuildGold" ) , m_nGuildId( nGuildId ), m_nGold( nGold ), m_nChaos( nChaos )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	int			m_nGuildId;
	StructGold	m_nGold;
	int			m_nChaos;
};

struct DB_SetGuildAllianceID			: GameDBManager::DBProc
{
	DB_SetGuildAllianceID( int nGuildId, int nAllianceID ) : DBProc( "DB_SetGuildAllianceID" ) , m_nGuildId( nGuildId ), m_nAllianceID( nAllianceID )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	int			m_nGuildId;
	int			m_nAllianceID;
};

struct DB_SetGuildAllianceBlockTime		: GameDBManager::DBProc
{
	DB_SetGuildAllianceBlockTime( int nGuildId, time_t tAllianceBlockTime ) : DBProc( "DB_SetGuildAllianceBlockTime" ), m_nGuildId( nGuildId ), m_tAllianceBlockTime( tAllianceBlockTime )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int			m_nGuildId;
	time_t		m_tAllianceBlockTime;
};

struct DB_InsertAlliance			: GameDBManager::DBProc
{
	DB_InsertAlliance( int nAllianceId, int nLeadGuildId, const char * szName, int nMaxAllianceCnt ) : DBProc( "DB_InsertAlliance" ), m_nAllianceId( nAllianceId ), m_nLeadGuildId( nLeadGuildId ), m_strName( szName ), m_nMaxAllianceCnt( nMaxAllianceCnt )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int			m_nAllianceId;
	int			m_nLeadGuildId;
	_bstr_t		m_strName;
	int			m_nMaxAllianceCnt;
};

struct DB_UpdateAllianceInfo		: GameDBManager::DBProc
{
	DB_UpdateAllianceInfo( int nAllianceId, int nMaxAllianceCnt ) : DBProc( "DB_UpdateAllianceInfo" ), m_nAllianceId( nAllianceId ), m_nMaxAllianceCnt( nMaxAllianceCnt )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int		m_nAllianceId;
	int		m_nMaxAllianceCnt;
};

struct DB_ChangeAllianceName			: GameDBManager::DBProc
{
	DB_ChangeAllianceName( const char *_szNewName, int nAllianceId, StructPlayer * pPlayer )
		: DBProc( "DB_ChangeAllianceName", true )
		, m_nAllianceId( nAllianceId )
		, m_szNewName( _szNewName )
		, m_pPlayer( pPlayer )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	int				m_nAllianceId;
	_bstr_t			m_szNewName;
	StructPlayer *	m_pPlayer;
};

struct DB_DeleteAllianceInfo		: GameDBManager::DBProc
{
	DB_DeleteAllianceInfo( int nAllianceId ) : DBProc( "DB_UpdateAllianceInfo" ), m_nAllianceId( nAllianceId )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int		m_nAllianceId;
};

struct DB_SetGuildLeader			: GameDBManager::DBProc
{
	DB_SetGuildLeader( int nGuildId, int nLeaderId ) : DBProc( "DB_SetGuildLeader" ), m_nGuildId( nGuildId ), m_nLeaderId( nLeaderId )
	{
	}

	bool proc( DBConnection & db );

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	int			m_nGuildId;
	int			m_nLeaderId;
};

struct DB_SetGuildDungeonID				: GameDBManager::DBProc
{
	DB_SetGuildDungeonID( int nGuildID, int nDungeonID )
		: DBProc( "DB_SetGuildDungeonBlockTime" )
		, m_nGuildID( nGuildID )
		, m_nDungeonID( nDungeonID )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:

	int			m_nGuildID;
	int			m_nDungeonID;
};

struct DB_SetGuildDungeonBlockTime		: GameDBManager::DBProc
{
	DB_SetGuildDungeonBlockTime( int nGuildId, time_t tDungeonBlockTime ) : DBProc( "DB_SetGuildDungeonBlockTime" ), m_nGuildId( nGuildId ), m_tDungeonBlockTime( tDungeonBlockTime )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int			m_nGuildId;
	time_t		m_tDungeonBlockTime;
};

struct DB_SetGuildNotice				: GameDBManager::DBProc
{
	DB_SetGuildNotice( const int nGuildID, const char * pszNotice )
		: DBProc( "DB_SetGuildNotice" )
		, m_nGuildID( nGuildID )
		, m_szNotice( pszNotice )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int				m_nGuildID;
	_bstr_t			m_szNotice;
};

struct DB_SetGuildURL					: GameDBManager::DBProc
{
	DB_SetGuildURL( const int nGuildID, const char * pszURL )
		: DBProc( "DB_SetGuildURL" )
		, m_nGuildID( nGuildID )
		, m_szURL( pszURL )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int				m_nGuildID;
	_bstr_t			m_szURL;
};

struct DB_SetGuildPermission			: GameDBManager::DBProc
{
	DB_SetGuildPermission( const PlayerUID nPlayerUID, const char nPermission )
		: DBProc( "DB_SetGuildPermission" )
		, m_nPlayerUID( nPlayerUID )
		, m_nPermission( nPermission )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	PlayerUID					m_nPlayerUID;
	char						m_nPermission;
};

struct DB_SetGuildPermissionName			: GameDBManager::DBProc
{
	DB_SetGuildPermissionName( const int nGuildID, const char nPermission, const char * pszName )
		: DBProc( "DB_SetGuildPermissionName" )
		, m_nGuildID( nGuildID )
		, m_nPermission( nPermission )
		, m_szName( pszName )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int			m_nGuildID;
	char		m_nPermission;
	_bstr_t		m_szName;
};

struct DB_SetGuildPermissionSet			: GameDBManager::DBProc
{
	DB_SetGuildPermissionSet( const int nGuildID, const char nPermission, const int nPermissionSet )
		: DBProc( "DB_SetGuildPermissionSet" )
		, m_nGuildID( nGuildID )
		, m_nPermission( nPermission )
		, m_nPermissionSet( nPermissionSet )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int			m_nGuildID;
	char		m_nPermission;
	int			m_nPermissionSet;
};

struct DB_SetGuildMemberMemo			:GameDBManager::DBProc
{
	DB_SetGuildMemberMemo( const int nPlayerUID, const char * pszMemo )
		: DBProc( "DB_SetGuildMemberMemo" )
		, m_nPlayerUID( nPlayerUID )
		, m_szMemo( pszMemo )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	PlayerUID		m_nPlayerUID;
	_bstr_t			m_szMemo;
};

struct DB_SetGuildAdvertiseTypeAndTime	: GameDBManager::DBProc
{
	DB_SetGuildAdvertiseTypeAndTime( const int nGuildID, unsigned char nAdvertiseType, time_t tAdvertiseEnd )
		: DBProc( "DB_SetGuildAdvertiseTypeAndTime" )
		, m_nGuildID( nGuildID )
		, m_nAdvertiseType( nAdvertiseType )
		, m_tAdvertiseEnd( tAdvertiseEnd )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int				m_nGuildID;
	unsigned char	m_nAdvertiseType;
	time_t			m_tAdvertiseEnd;
};

struct DB_SetGuildAdvertiseComment		: GameDBManager::DBProc
{
	DB_SetGuildAdvertiseComment( const int nGuildID, const char * pszComment )
		: DBProc( "DB_SetGuildAdvertiseComment" )
		, m_nGuildID( nGuildID )
		, m_szComment( pszComment )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int				m_nGuildID;
	_bstr_t			m_szComment;
};

struct DB_SetGuildRecruiting		: GameDBManager::DBProc
{
	DB_SetGuildRecruiting( const int nGuildID, const bool bRecruiting, const short nMinRecruitLevel, const short nMaxRecruitLevel )
		: DBProc( "DB_SetGuildRecruiting", true )
		, m_nGuildID( nGuildID )
		, m_bRecruiting( bRecruiting )
		, m_nMinRecruitLevel( nMinRecruitLevel )
		, m_nMaxRecruitLevel( nMaxRecruitLevel )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; };

	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

private:
	int				m_nGuildID;
	bool			m_bRecruiting;
	short			m_nMinRecruitLevel;
	short			m_nMaxRecruitLevel;
};

struct DB_SetGuildIcon						: GameDBManager::DBProc
{
	DB_SetGuildIcon( int nGuildID, const char *szIconFileName, int nIconSize ) : DBProc( "DB_SetGuildIcon" ), m_nGuildID( nGuildID ), m_nIconSize( nIconSize )
	{
		m_szIconFileName = szIconFileName;
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int				m_nGuildID;
	_bstr_t			m_szIconFileName;
	int				m_nIconSize;
};

struct DB_SetGuildBanner						: GameDBManager::DBProc
{
	DB_SetGuildBanner( int nGuildID, const char *szBannerFileName, int nBannerSize ) : DBProc( "DB_SetGuildBanner" ), m_nGuildID( nGuildID ), m_nBannerSize( nBannerSize )
	{
		m_szBannerFileName = szBannerFileName;
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; };

	virtual bool onProcess( DBConnection & db );

private:
	int				m_nGuildID;
	_bstr_t			m_szBannerFileName;
	int				m_nBannerSize;
};

struct DB_InsertQuest				: GameDBManager::DBProc
{
	DB_InsertQuest( struct StructPlayer * pPlayer, int owner_id, int id, int code, int start_id, int status1, int status2, int status3, int status4, int status5, int status6, int remain_time, int progress )
		: DBProc( "DB_InsertQuest", true )
		, m_pPlayer( pPlayer )
		, m_nOwnerID( owner_id )
		, m_nID( id )
		, m_nCode( code )
		, m_nStartID( start_id )
		, m_nStatus1( status1 )
		, m_nStatus2( status2 )
		, m_nStatus3( status3 )
		, m_nStatus4( status4 )
		, m_nStatus5( status5 )
		, m_nStatus6( status6 )
		, m_nRemainTime( remain_time )
		, m_nProgress( progress )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		StructPlayer *m_pPlayer;

	int m_nOwnerID;
	int m_nID;
	int m_nCode;
	int m_nStartID;
	int m_nStatus1;
	int m_nStatus2;
	int m_nStatus3;
	int m_nStatus4;
	int m_nStatus5;
	int m_nStatus6;
	int m_nRemainTime;
	int m_nProgress;
};

struct DB_InsertRandomQuest				: GameDBManager::DBProc
{
	DB_InsertRandomQuest( struct StructPlayer * pPlayer, int owner_id, int code, int key1, int key2, int key3, int value1, int value2, int value3 )
		: DBProc( "DB_InsertRandomQuest", true ), m_pPlayer( pPlayer ), m_nOwnerID( owner_id ), m_nCode( code ), m_nKey1( key1 ), m_nKey2( key2 ), m_nKey3( key3 ), m_nValue1( value1 ), m_nValue2( value2 ), m_nValue3( value3 )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		StructPlayer *m_pPlayer;

	int m_nOwnerID;
	int m_nCode;
	int m_nKey1;
	int m_nKey2;
	int m_nKey3;
	int m_nValue1;
	int m_nValue2;
	int m_nValue3;
};

struct DB_UpdateRandomQuest				: GameDBManager::DBProc
{
	DB_UpdateRandomQuest( struct StructPlayer * pPlayer, int owner_id, int code, int key1, int key2, int key3, int value1, int value2, int value3, char cIsDropped )
		: DBProc( "DB_UpdateRandomQuest", true ), m_pPlayer( pPlayer ), m_nOwnerID( owner_id ), m_nCode( code ), m_nKey1( key1 ), m_nKey2( key2 ), m_nKey3( key3 ), m_nValue1( value1 ), m_nValue2( value2 ), m_nValue3( value3 ), m_cDropped( cIsDropped )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct	StructPlayer *m_pPlayer;

	int m_nOwnerID;
	int m_nCode;
	int m_nKey1;
	int m_nKey2;
	int m_nKey3;
	int m_nValue1;
	int m_nValue2;
	int m_nValue3;
	char m_cDropped;
};

struct DB_UpdateQuest				: GameDBManager::DBProc
{
	DB_UpdateQuest( struct StructPlayer * pPlayer, int owner_id, int id, int status1, int status2, int status3, int status4, int status5, int status6, int remain_time, int progress )
		: DBProc( "DB_UpdateQuest", true )
		, m_pPlayer( pPlayer )
		, m_nOwnerID( owner_id )
		, m_nID( id )
		, m_nStatus1( status1 )
		, m_nStatus2( status2 )
		, m_nStatus3( status3 )
		, m_nStatus4( status4 )
		, m_nStatus5( status5 )
		, m_nStatus6( status6 )
		, m_nRemainTime( remain_time )
		, m_nProgress( progress )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	static bool update_quest( DBConnection & db, int nOwnerID, int nID, int nStatus1, int nStatus2, int nStatus3, int nStatus4, int nStatus5, int nStatus6, int nRemainTime, int nProgress );
	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		StructPlayer *m_pPlayer;

	int m_nOwnerID;
	int m_nID;
	int m_nStatus1;
	int m_nStatus2;
	int m_nStatus3;
	int m_nStatus4;
	int m_nStatus5;
	int m_nStatus6;
	int m_nRemainTime;
	int m_nProgress;
};

struct DB_DeleteQuest				: GameDBManager::DBProc
{
	DB_DeleteQuest( struct StructPlayer * pPlayer, int owner_id, int id ) : DBProc( "DB_DeleteQuest", true ), m_pPlayer( pPlayer ), m_nOwnerID( owner_id ), m_nID( id )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct StructPlayer * m_pPlayer;
	int m_nOwnerID;
	int m_nID;
};

struct DB_InsertQuestCoolTime			: GameDBManager::DBProc
{
	DB_InsertQuestCoolTime( struct StructPlayer * pPlayer, int owner_id, int code, time_t cool_time, int progress )
		: DBProc( "DB_InsertQuestCoolTime", true )
		, m_pPlayer( pPlayer )
		, m_nOwnerID( owner_id )
		, m_nCode( code )
		, m_tCoolTime( cool_time )
		, m_nProgress( progress )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }
	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		StructPlayer *m_pPlayer;

	int m_nOwnerID;
	int m_nCode;
	time_t m_tCoolTime;
	int m_nProgress;
};

struct DB_UpdateQuestCoolTime			: GameDBManager::DBProc
{
	DB_UpdateQuestCoolTime( struct StructPlayer * pPlayer, int owner_id, int code, time_t cool_time, int progress )
		: DBProc( "DB_UpdateQuestCoolTime", true )
		, m_pPlayer( pPlayer )
		, m_nOwnerID( owner_id )
		, m_nCode( code )
		, m_tCoolTime( cool_time )
		, m_nProgress( progress )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	static bool update_quest_cool_time( DBConnection & db, int nOwnerID, int nCode, time_t tCoolTime, int nProgress );
	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		StructPlayer *m_pPlayer;

	int m_nOwnerID;
	int m_nCode;
	time_t m_tCoolTime;
	int m_nProgress;
};

struct DB_DeleteQuestCoolTime			: GameDBManager::DBProc
{
	DB_DeleteQuestCoolTime( struct StructPlayer * pPlayer, int owner_id, int code )
		: DBProc( "DB_DeleteQuestCoolTime", true )
		, m_pPlayer( pPlayer )
		, m_nOwnerID( owner_id )
		, m_nCode( code )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }
	virtual bool onProcess( DBConnection & db );
	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct StructPlayer * m_pPlayer;
	int m_nOwnerID;
	int m_nCode;
};

struct DB_InsertTitle				: GameDBManager::DBProc
{
	DB_InsertTitle( struct StructPlayer * pPlayer, int nSID, int nOwnerID, int nCode, int nStatus )
		: DBProc( "DB_InsertTitle", true )
		, m_pPlayer( pPlayer )
		, m_nSID( nSID )
		, m_nOwnerID( nOwnerID )
		, m_nCode( nCode )
		, m_nStatus( nStatus )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		StructPlayer *m_pPlayer;
	int m_nSID;
	int m_nOwnerID;
	int m_nCode;
	int m_nStatus;
};

struct DB_UpdateTitle				: GameDBManager::DBProc
{
	DB_UpdateTitle( struct StructPlayer * pPlayer, int nSID, int nStatus )
		: DBProc( "DB_UpdateTitle", true )
		, m_pPlayer( pPlayer )
		, m_nSID( nSID )
		, m_nStatus( nStatus )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		StructPlayer *m_pPlayer;
	int m_nSID;
	int m_nStatus;
};

struct DB_InsertTitleCondition			: GameDBManager::DBProc
{
	DB_InsertTitleCondition( struct StructPlayer * pPlayer, int nSID, int nOwnerID, int nType, __int64 nCount )
		: DBProc( "DB_InsertTitle", true )
		, m_pPlayer( pPlayer )
		, m_nSID( nSID )
		, m_nOwnerID( nOwnerID )
		, m_nType( nType )
		, m_nCount( nCount )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		StructPlayer *m_pPlayer;
	int m_nSID;
	int m_nOwnerID;
	int m_nType;
	__int64 m_nCount;
};

struct DB_UpdateTitleCondition			: GameDBManager::DBProc
{
	DB_UpdateTitleCondition( struct StructPlayer * pPlayer, int nSID, __int64 nCount )
		: DBProc( "DB_UpdateTitle", true )
		, m_pPlayer( pPlayer )
		, m_nSID( nSID )
		, m_nCount( nCount )
	{
	}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:
	bool proc( DBConnection & db );

	struct		StructPlayer *m_pPlayer;
	int m_nSID;
	__int64 m_nCount;
};

struct DB_UpdateEventItemDropInfo	: GameDBManager::DBProc
{
	DB_UpdateEventItemDropInfo( int sid, int remain_time, int count ) :  DBProc( "DB_UpdateEventItemDropInfo" ), m_nSID( sid ), m_nRemainTime( remain_time ), m_nCount( count ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	int m_nSID;
	int m_nRemainTime;
	int m_nCount;

};

struct DB_UpdateEventItemSupplyInfo	: GameDBManager::DBProc
{
	DB_UpdateEventItemSupplyInfo( int sid, int left_count ) :  DBProc( "DB_UpdateEventItemSupplyInfo" ), m_nSID( sid ), m_nLeftCount( left_count ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	bool proc( DBConnection & db );

	int m_nSID;
	int m_nLeftCount;

};

struct DB_UpdateDungeonRaidTime	: GameDBManager::DBProc
{
	DB_UpdateDungeonRaidTime( int dungeon_id, int guild_id, int raid_time, int record ) :  DBProc( "DB_UpdateDungeonRaidTime" ), nDungeonID( dungeon_id ), nGuildID( guild_id ), nRaidTime( raid_time ), nRecord( record ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	int nDungeonID;
	int nGuildID;
	int nRaidTime;
	int nRecord;
};

struct DB_UpdateDungeon	: GameDBManager::DBProc
{
	DB_UpdateDungeon( int dungeon_id, int own_guild_id, int raid_guild_id, int best_raid_time, int dungeon_siege_finish_time, int dungeon_raid_wrap_up_time, int tax_rate ) :  DBProc( "DB_UpdateDungeon" ), nDungeonID( dungeon_id ), nOwnGuildID( own_guild_id ), nRaidGuildID( raid_guild_id ), nBestRaidTime( best_raid_time ), nDungeonSiegeFinishTime( dungeon_siege_finish_time ), nDungeonRaidWrapUpTime( dungeon_raid_wrap_up_time ), nTaxRate( tax_rate ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	int nDungeonID;
	int nOwnGuildID;
	int nRaidGuildID;
	int nBestRaidTime;
	int nDungeonSiegeFinishTime;
	int nDungeonRaidWrapUpTime;
	int nTaxRate;

};

struct DB_UpdateDungeonOwnerGuild	: GameDBManager::DBProc
{
	DB_UpdateDungeonOwnerGuild( const int & dungeon_id, const int & owner_guild_id )
		: DBProc( "DB_UpdateDungeonOwnerGuild" )
		, nDungeonID( dungeon_id )
		, nOwnerGuildID( owner_guild_id )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	int nDungeonID;
	int nOwnerGuildID;
};

struct DB_ClearDungeonRaidRecord	: GameDBManager::DBProc
{
	DB_ClearDungeonRaidRecord( int dungeon_id, int guild_id = 0 ) :  DBProc( "DB_ClearDungeonRaidRecord" ), nDungeonID( dungeon_id ), nGuildID( guild_id ) {}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	int nDungeonID;
	int nGuildID;
};

struct DB_EraseUserFromAutoAccountList : GameDBManager::DBProc
{
	DB_EraseUserFromAutoAccountList( int account_id ) : DBProc( "DB_EraseUserFromAutoAccountList", true ), m_nAccountID( account_id ) {}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

	virtual void onFail( const _com_error & exception );

private:

	int m_nAccountID;

	bool proc( DBConnection & db );
};

struct DB_ClearAutoAccountList : GameDBManager::DBProc
{
	DB_ClearAutoAccountList() : DBProc( "DB_ClearAutoAccountList" ) {}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:

	bool proc( DBConnection & db );
};

struct DB_UpdateLaunchedScheduledCommand : GameDBManager::DBProc
{
	DB_UpdateLaunchedScheduledCommand( const int nSID, const time_t tLastLaunched, const bool bFinished ) : DBProc( "DB_UpdateLaunchedScheduledCommand" )
		, m_nSID( nSID )
		, m_tLastLaunched( tLastLaunched )
		, m_bFinished( bFinished )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	int		m_nSID;
	time_t	m_tLastLaunched;
	bool	m_bFinished;
};

// 경매 정보에 링크된 아이템 정보에 해당하는 아이템이 DB에
// 있을 경우 변경 정보 : 해당 아이템의 account_id, owner_id, auction_id, keeping_id
// 없을 경우 변경 정보 : 새로운 아이템 데이터 추가 및 기존 아이템의 count 감소
struct DB_InsertAuctionInfo : GameDBManager::DBProc
{
	DB_InsertAuctionInfo( const AuctionInfo *pAuctionInfo, const ItemUID nOriginalItemUID = 0, const __int64 nOriginalItemCount = 0 );

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	AuctionInfo							m_AuctionInfo;

	const ItemUID						m_nItemUID;
	const ItemBase::ItemCode			m_nItemCode;
	const int							m_nItemFlag;
	const __int64						m_nItemCount;
	const int							m_nItemLevel;
	const int							m_nItemEnhance;
	const int							m_nItemEhterealDurability;
	const int							m_nItemEndurance;
	const ItemInstance::GenerateCode	m_nItemGenCode;
	const int							m_nItemRemainTime;
	const unsigned char					m_cItemElementalEffectType;
	const time_t						m_tItemElementalEffectExpire;
	const int							m_nItemElementalEffectAttackPoint;
	const int							m_nItemElementalEffectMagicPoint;
	const ItemBase::ItemCode			m_nItemAppearanceCode;

	const ItemUID			m_nOriginalItemUID;
	const __int64						m_nOriginalItemCount;
	
};

struct DB_UpdateAuctionBiddingInfo : GameDBManager::DBProc
{
	DB_UpdateAuctionBiddingInfo( const AuctionUID nAuctionUID, const std::vector< PlayerUID > & vBidderUID, const StructGold & nHighestBiddingPrice, const PlayerUID nHighestBidderUID, const std::string& szHighestBidderName ) : DBProc( "DB_UpdateAuctionBiddingInfo" )
		, m_nAuctionUID( nAuctionUID )
		, m_nHighestBiddingPrice( nHighestBiddingPrice )
		, m_nHighestBidderUID( nHighestBidderUID )
		, m_szHighestBidderName( szHighestBidderName )
	{
		if( !vBidderUID.empty() )
		{
			if( !vBidderUID.empty() )
			{
				char szBuf[12];

				s_sprintf( szBuf, _countof( szBuf ), "%d", vBidderUID.front() );
				m_szBidderList += szBuf;
				for( std::vector< PlayerUID >::const_iterator it = vBidderUID.begin() + 1 ; it != vBidderUID.end() ; ++it )
				{
					s_sprintf( szBuf, _countof( szBuf ), "|%d", (*it) );
					m_szBidderList += szBuf;
				}
			}
		}
	}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	AuctionUID									m_nAuctionUID;
	std::string									m_szBidderList;
	StructGold									m_nHighestBiddingPrice;
	PlayerUID									m_nHighestBidderUID;
	std::string									m_szHighestBidderName;
};

// AuctionInfo 레코드를 삭제하고 해당 경매에 연관된 아이템의 auction_id 를 0 으로 만듦(소유자 정보가 완전히 없어짐)
struct DB_DeleteAuctionInfo : GameDBManager::DBProc
{
	DB_DeleteAuctionInfo( const AuctionUID nAuctionUID ) : DBProc( "DB_DeleteAuctionInfo" )
		, m_nAuctionUID( nAuctionUID )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	const AuctionUID m_nAuctionUID;
};

struct DB_InsertNewItemAndKeepingInfo : GameDBManager::DBProc
{
	DB_InsertNewItemAndKeepingInfo( const ItemKeepingInfo *pItemKeeping );

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	// 멤버인 pItem을 NULL로 세팅하여 참조를 방지하기 위해 const가 아님
	ItemKeepingInfo						m_ItemKeepingInfo;

	const ItemUID						m_nItemUID;
	const ItemBase::ItemCode			m_nItemCode;
	const int							m_nItemFlag;
	const __int64						m_nItemCount;
	const int							m_nItemLevel;
	const int							m_nItemEnhance;
	const int							m_nItemEtherealDurability;
	const int							m_nItemEndurance;
	const ItemInstance::GenerateCode	m_nItemGenCode;
	const int							m_nItemRemainTime;
	const unsigned char					m_cItemElementalEffectType;
	const time_t						m_tItemElementalEffectExpire;
	const int							m_nItemElementalEffectAttackPoint;
	const int							m_nItemElementalEffectMagicPoint;
	const ItemBase::ItemCode			m_nItemAppearanceCode;
};

// ItemKeeping 레코드를 삭제하고 연관된 아이템의 소유권을 변경함(PlayerUID는 필요없고 아이템 파괴/반환 여부만 인자로 넘겨줌)
struct DB_DeleteItemKeepingInfo : GameDBManager::DBProc
{
	DB_DeleteItemKeepingInfo( ItemKeepingUID nItemKeepingUID, bool bReturnItemToOwner, ItemUID nPrevOwnedItemSID ) : DBProc( "DB_DeleteItemKeepingInfo" )
		, m_nItemKeepingUID( nItemKeepingUID )
		, m_bReturnItemToOwner( bReturnItemToOwner )
		, m_nPrevOwnedItemSID( nPrevOwnedItemSID )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	ItemKeepingUID						m_nItemKeepingUID;
	bool								m_bReturnItemToOwner;
	ItemUID								m_nPrevOwnedItemSID;
};

// 경매 낙찰/유찰 등 경매 데이터 삭제 및 아이템 보관 정보가 함께 추가되어야 하는 경우 사용
// (2개를 따로 할 경우 Transaction 실패시 아이템 손실/복사 발생)
struct DB_DeleteAuctionInfoAndInsertItemKeepingInfo : GameDBManager::DBProc
{
	DB_DeleteAuctionInfoAndInsertItemKeepingInfo( const AuctionUID nAuctionUID, const ItemKeepingInfo *pItemKeeping );

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	AuctionUID							m_nAuctionUID;
	ItemKeepingInfo						m_ItemKeepingInfo;
	__int64								m_nItemUID;
};

struct DB_UpdateItemAuctionKeepingID : GameDBManager::DBProc
{
	DB_UpdateItemAuctionKeepingID( ItemUID nItemUID, AuctionUID nAuctionUID, ItemKeepingUID nItemKeepingUID ) : DBProc( "DB_UpdateItemAuctionKeepingID" )
		, m_nItemUID( nItemUID )
		, m_nAuctionUID( nAuctionUID )
		, m_nItemKeepingUID( nItemKeepingUID )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection & db );

private:
	ItemUID								m_nItemUID;
	AuctionUID							m_nAuctionUID;
	ItemKeepingUID						m_nItemKeepingUID;
};

struct DB_InsertAutoAuctionRegistrationInfo : GameDBManager::DBProc
{
	DB_InsertAutoAuctionRegistrationInfo( int nAutoAuctionID, time_t tScheduledRegisterTime, time_t tRegisteredTime )
		: DBProc( "DB_InsertAutoAuctionRegistrationInfo" )
		, m_nAutoAuctionID( nAutoAuctionID )
		, m_tScheduledRegisterTime( tScheduledRegisterTime )
		, m_tRegisteredTime( tRegisteredTime )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection &db );

private:
	int			m_nAutoAuctionID;
	time_t		m_tScheduledRegisterTime;
	time_t		m_tRegisteredTime;
};

struct DB_SetRankingScore : GameDBManager::DBProc
{
	DB_SetRankingScore( const int nRankingType, const PlayerUID nPlayerUID, const c_fixed10 & fScore )
		: DBProc( "DB_SetRankingScore" )
		, m_nRankingType( nRankingType )
		, m_nPlayerUID( nPlayerUID )
		, m_fScore( fScore )
	{}

	virtual void onEnd( bool bIsCancel );

	virtual bool onProcess( DBConnection &db );

private:
	int			m_nRankingType;
	PlayerUID	m_nPlayerUID;
	c_fixed10	m_fScore;
};

struct DB_InvalidateRankingScore : GameDBManager::DBProc
{
	DB_InvalidateRankingScore( const int nRankingType, const PlayerUID nPlayerUID )
		: DBProc( "DB_InvalidateRankingScore" )
		, m_nRankingType( nRankingType )
		, m_nPlayerUID( nPlayerUID )
	{}

	virtual void onEnd( bool bIsCancel );

	virtual bool onProcess( DBConnection &db );

private:
	int			m_nRankingType;
	PlayerUID	m_nPlayerUID;
};

struct DB_SettleRanking : GameDBManager::DBProc
{
	DB_SettleRanking( const int nRankingType, const time_t & tNextSettling )
		: DBProc( "DB_SettleRanking" )
		, m_nRankingType( nRankingType )
		, m_tNextSettling( tNextSettling )
	{}

	virtual void onEnd( bool bIsCancel );

	virtual bool onProcess( DBConnection &db );

private:
	int			m_nRankingType;
	time_t		m_tNextSettling;
};

struct DB_InsertGlobalVariable : GameDBManager::DBProc
{
	DB_InsertGlobalVariable( const char * pszName, const char * pszValue )
		: DBProc( "DB_InsertGlobalVariable" )
		, m_strName( pszName )
		, m_strValue( pszValue )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection &db );

private:
	std::string m_strName;
	std::string m_strValue;
};

struct DB_UpdateGlobalVariable : GameDBManager::DBProc
{
	DB_UpdateGlobalVariable( const char * pszName, const char * pszValue )
		: DBProc( "DB_UpdateGlobalVariable" )
		, m_strName( pszName )
		, m_strValue( pszValue )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection &db );

private:
	std::string m_strName;
	std::string m_strValue;
};

struct DB_DeleteGlobalVariable : GameDBManager::DBProc
{
	DB_DeleteGlobalVariable( const char * pszName )
		: DBProc( "DB_DeleteGlobalVariable" )
		, m_strName( pszName )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection &db );

private:
	std::string m_strName;
};

struct DB_InsertCash : GameDBManager::DBProc
{
	DB_InsertCash( struct StructPlayer * pPlayer, const char * pszAccountName, const int nAccountID, const ItemBase::ItemCode nItemCode, const ItemUID nItemSID )
		: DBProc( "DB_InsertCash" )
		, m_pPlayer( pPlayer )
		, m_strAccountName( pszAccountName )
		, m_nAccountID( nAccountID )
		, m_nItemCode( nItemCode )
		, m_nItemSID( nItemSID )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection &db );

private:
	bool proc( DBConnection & db );

	struct StructPlayer * m_pPlayer;
	std::string m_strAccountName;
	int m_nAccountID;
	const ItemBase::ItemCode m_nItemCode;
	const ItemUID m_nItemSID;
};

struct DB_InsertFarmInfo : GameDBManager::DBProc
{
	DB_InsertFarmInfo( struct StructPlayer * pPlayer, const int nFarmUID, const char nSlot, struct StructItem * pCard, const int nMaxLevel, const bool bUseCracker, const bool bCash, const time_t tRegistrationTime, int nDuration, const time_t tNursingTime )
		: DBProc( "DB_InsertFarm" )
		, m_pPlayer( pPlayer )
		, m_nFarmUID( nFarmUID )
		, m_nSlot( nSlot )
		, m_pCard( pCard )
		, m_nMaxLevel( nMaxLevel )
		, m_bUseCracker( bUseCracker )
		, m_bCash( bCash )
		, m_tRegistrationTime( tRegistrationTime )
		, m_nDuration( nDuration )
		, m_tNursingTime( tNursingTime )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection &db );

private:
	struct StructPlayer *			m_pPlayer;
	const int						m_nFarmUID;
	const char						m_nSlot;
	struct StructItem *				m_pCard;
	const int						m_nMaxLevel;
	const bool						m_bUseCracker;
	const bool						m_bCash;
	const time_t					m_tRegistrationTime;
	const int						m_nDuration;
	const time_t					m_tNursingTime;
};

struct DB_UpdateNursingTime : GameDBManager::DBProc
{
	DB_UpdateNursingTime( struct StructPlayer * pPlayer, const int nFarmUID, const time_t tNursingTime )
		: DBProc( "DB_UpdateNursingTime" )
		, m_pPlayer( pPlayer )
		, m_nFarmUID( nFarmUID )
		, m_tNursingTime( tNursingTime )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection &db );

private:
	struct StructPlayer *	m_pPlayer;
	const int				m_nFarmUID;
	const time_t			m_tNursingTime;
};

struct DB_DeleteFarmInfo : GameDBManager::DBProc
{
	DB_DeleteFarmInfo( struct StructPlayer * pPlayer, const int nFarmUID )
		: DBProc( "DB_InsertFarm" )
		, m_pPlayer( pPlayer )
		, m_nFarmUID( nFarmUID )
	{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }

	virtual bool onProcess( DBConnection &db );

private:
	struct StructPlayer *	m_pPlayer;
	int						m_nFarmUID;
};

// { 배틀 아레나 관련

// 배틀 아레나 관련 페널티 정보를 업데이트(캐릭터가 로그인 상태일 때와 로그아웃 상태일 때 각각 생성자를 다르게 사용하며, 그에 따라 DB 처리 후 이벤트가 다르게 동작)
struct DB_UpdateBattleArenaPenalty : GameDBManager::DBProc
{
	// 로그인 상태의 플레이어 페널티 정보 업데이트용
	DB_UpdateBattleArenaPenalty( struct StructPlayer * pPlayer );
	// 로그아웃 상태의 플레이어 페널티 정보 업데이트용
	DB_UpdateBattleArenaPenalty( PlayerUID nPlayerUID, time_t tBlockTime, int nPenaltyCount, time_t tPenaltyDecrease );

	virtual bool onProcess( DBConnection & db );
	// 페널티 DB 처리 실패하면 그까이꺼 한 번 봐주지 뭐 -_ -;;
	virtual void onFail( const _com_error & exception )			{}

	virtual void onEnd( bool bIsCancel )						{ delete this; }

protected:
	bool proc( DBConnection & db );

private:
	struct StructPlayer *	m_pPlayer;

	PlayerUID				m_nPlayerUID;
	time_t					m_tBlock;
	int						m_nPenaltyCount;
	time_t					m_tPenaltyDecrease;
};

// } 배틀 아레나 관련

struct DB_SetRandomOption : GameDBManager::DBProc
{
	DB_SetRandomOption( struct StructPlayer * pPlayer, struct StructItem * pItem, ItemInstance::RANDOM_OPTION randomOption )
		: DBProc( "DB_SetRandomOption" )
		, m_pPlayer( pPlayer )
		, m_pItem( pItem )
		, m_RandomOption( randomOption )
	{}

	virtual void onEnd( bool bIsCancel )						{ delete this; };
	virtual bool onProcess( DBConnection & db );

protected:
	bool proc( DBConnection & db );
	

private:
	struct StructPlayer *	m_pPlayer;
	ItemInstance::RANDOM_OPTION m_RandomOption;

public:
	struct StructItem *	m_pItem;
};



struct DB_DeleteRandomOption : GameDBManager::DBProc
{
	DB_DeleteRandomOption( struct StructItem * pItem, int _sid ) 
		: DBProc( "DB_DeleteRandomOption" )
		, m_pItem( pItem )
		, sid(_sid)
		{}

	virtual void onEnd( bool bIsCancel )					{ delete this; }
	virtual bool onProcess( DBConnection &db );

protected:
	bool proc( DBConnection & db );

private:
	struct StructItem *	m_pItem;
	int sid;
};



struct DB_UpdateItemAwakenSID : GameDBManager::DBProc
{
	DB_UpdateItemAwakenSID( struct StructItem *	pItem ) 
		: DBProc( "DB_UpdateItemAwakenSID" )
		, m_pItem( pItem ) {}

	virtual void onEnd( bool bIsCancel )					{ delete this; }
	virtual bool onProcess( DBConnection &db );

protected:
	bool proc( DBConnection & db );

private:
	struct StructItem *		m_pItem;
};



struct DB_UpdateItemRandomOptionSID : GameDBManager::DBProc
{
	DB_UpdateItemRandomOptionSID( struct StructItem * pItem )
		: DBProc( "DB_UpdateRandomOptionSID" )
		, m_pItem( pItem ) {}

	virtual void onEnd( bool bIsCancel )					{ delete this; }
	virtual bool onProcess( DBConnection &db );

protected:
	bool proc( DBConnection & db );

private:
	struct StructItem *		m_pItem;
};



struct DB_UpdateRandomOptionOwnerInfo : GameDBManager::DBProc
{
	DB_UpdateRandomOptionOwnerInfo( struct StructPlayer *	pPlayer,int _sid, int _owner_id, int _account_id ) 
		: DBProc( "DB_UpdateRandomOptionOwnerInfo" )
		,m_pPlayer( pPlayer ), sid(_sid), owner_id(_owner_id), account_id(_account_id) {}

	virtual void onEnd( bool bIsCancel )					{ delete this; }
	virtual bool onProcess( DBConnection &db );

protected:
	bool proc( DBConnection & db );

private:
	struct StructPlayer *	m_pPlayer;
	int sid;
	int owner_id;
	int account_id;
};