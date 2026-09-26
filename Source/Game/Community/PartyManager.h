#pragma once

#include <vector>
#include <string>

#include <toolkit/ILock.h>
#include <mmo/ArMove.h>
#include <mmo/ArType.h>
#include <toolkit/KHash.h>

#include "UID.h"
#include "GameDBManager.h"
#include "ADOConnection.h"


class PartyManager
{
public:
	enum
	{
		MAX_PARTY_MEMBER = 8,
	};

	enum _PARTY_TYPE
	{
		TYPE_UNKNOWN						= -1,

		TYPE_NORMAL_PARTY					= 0,
		TYPE_RAID_ATTACKTEAM				= 1,
		TYPE_SIEGE_ATTACKTEAM				= 2,
		TYPE_HUNTAHOLIC_PARTY				= 3,
		TYPE_BATTLE_ARENA_TEAM				= 4,	// 배틀 아레나 시스템에 의해 자동 생성된 파티(유저가 파티원 구성 변경 불가)
		TYPE_BATTLE_ARENA_EXERCISE_TEAM		= 5,	// 배틀 아레나 연습 경기를 위해 생성된 파티(유저가 파티원 구성 변경 가능)
	};

	enum _ITEM_SHARE_MODE
	{
		ITEM_SHARE_MONOPOLY		= 0,
		ITEM_SHARE_RANDOM		= 1,
		ITEM_SHARE_LINEAR		= 2,
	};

	struct PartyMemberTag
	{
		bool		bIsOnline;
		int			sid;
		std::string strName;
		std::string strDisplayName;
		int			nLevel;		// 오프라인 멤버 참조용
		int			nJobId;		// 오프라인 멤버 참조용

		PartyMemberTag( int _sid, const char *szName, const char *szDisplayName, int lv, int job )
			: sid( _sid ), strName( szName ), strDisplayName( szDisplayName ), nLevel( lv ), nJobId( job ), bIsOnline( false )
		{
		}
	};

	struct PartyFunctor
	{
		virtual bool operator()( AR_HANDLE handle ) = 0;
	};
	struct PartyMemberTagFunctor
	{
		virtual bool operator()( PartyMemberTag * pMemberTag ) = 0;
	};
	struct LinkedPartyFunctor
	{
		virtual bool operator()( const int nPartyID ) = 0;
	};

	static PartyManager & GetInstance();

	struct PartyInfo;

	struct PartyLinkInfo
	{
		virtual ~PartyLinkInfo()	{}

		std::vector< PartyInfo * >	vPartyList;
		int							nLeadPartyID;
		int							nMaxPartyCount;
		int							nPassword;
	};

	struct AttackTeamInfo : public PartyLinkInfo
	{
		int							nGuildID;
	};

	struct PartyInfo
	{
		PartyInfo()
		{
			nPartyPassword = 0;
			nPartyID = 0;
			nLeaderSID = 0;
			eShareMode = ITEM_SHARE_MONOPOLY;
			ePartyType = TYPE_NORMAL_PARTY;
			nLeaderJobId = 0;

			nLastItemAcquirerIdx = 0;

			pPartyLinkInfo = NULL;

#ifdef _MEM_USAGE_DEBUG
			XSEH::IncreaseAllocCount( "PartyInfo" );
#endif
		}

		~PartyInfo()
		{
#ifdef _MEM_USAGE_DEBUG
			XSEH::DecreaseAllocCount( "PartyInfo" );
#endif
		}

		int								nPartyPassword;
		int								nPartyID;
		int								nLeaderSID;
		_ITEM_SHARE_MODE				eShareMode;
		_PARTY_TYPE						ePartyType;
		std::string						strPartyName;
		std::string						strLeaderName;
		std::string						strLeaderDisplayName;
		int								nLeaderJobId;
		std::vector< PartyMemberTag >	vMemberNameList;
		std::vector< AR_HANDLE >		vOnlineList;

		int								nLastItemAcquirerIdx;

		PartyLinkInfo *					pPartyLinkInfo;
	};

	bool Init();
	bool DeInit();

	// 연결된 파티 관련
	const _PARTY_TYPE	GetPartyType( int nPartyID ) const;
	bool				IsLinkedParty( int nPartyID ) const;
	bool				IsAttackTeamParty( int nPartyID ) const;
	bool				IsBattleArenaTeamParty( int nPartyID ) const;
	bool				IsJoinableLinkedParty( int nPartyID ) const;
	int					GetLinkedPartyLeadPartyID( int nPartyID ) const;

	// 시즈/레이드 공대 파티 관련(AttackTeam)
	bool				IsExistAttackTeam( int nGuildID ) const;
	int					GetAttackTeamLeadPartyIDByGuildID( int nGuildID ) const;
	bool				MakeAttackTeam( int nPartyID, int nGuildID, int nMaxGuildParty, bool bIsRaidParty );
	bool				JoinAttackTeam( int nGuildID, int nPartyID );
	int					GetAttackTeamGuildID( int nPartyID ) const;

	// 배틀 아레나 파티 관련(BattleArenaTeam)
	bool				MakeBattleArenaTeam( int nLeadPartyID, int nMaxPartyCount );
	bool				JoinBattleArenaTeam( int nLeadPartyID, int nPartyID );
	bool				ChangeBattleArenaExerciseTeamType( int nLeadPartyID, bool bToExerciseTeam );

	int  GetPartyPassword( int nPartyID ) const;
	int  GetLinkedPartyPassword( int nPartyID ) const;

	bool IsSomeoneInSiegeOrRaidDungeon( int nPartyID );

	int MakeParty( const char *szPartyName, int nLeaderSID, const _PARTY_TYPE & ePartyType );
	//bool DestroyParty( const char *szPartyName );
	bool DestroyParty( int nPartyID );
	int	 GetMemberCount( int nPartyID );

	int GetPartyID( const char * szPartyName ) const;
	std::string GetPartyName( int nPartyID ) const;

	bool IsLeader( int nPartyID, PlayerUID nPlayerUID ) const;
	bool IsMember( int nPartyID, PlayerUID nPlayerUID ) const;
	std::string	GetMemberDisplayName( int nPartyID, PlayerUID nPlayerUID ) const;
	PlayerUID	GetMemberUID( int nPartyID, const char * szPlayerName ) const;

	bool Promote( int nPartyID, struct StructPlayer *pPtr );
	bool AutoPromote( int nPartyID, const bool bOnlyOnlinePlayer );

	void OnChangeCharacterName( int nPartyID, PlayerUID nPlayerUID, const char * szNewName );
	void OnChangeCharacterLevel( int nPartyID, PlayerUID nPlayerUID, const int nLevel );
	void OnChangeCharacterJob( int nPartyID, PlayerUID nPlayerUID, const int nJobId );

	int  GetMaxLevel( int nPartyID );
	int  GetMinLevel( int nPartyID );
	int  GetTotalLevel( int nPartyID );
	std::string	GetLeaderName( int nPartyID ) const;
	std::string	GetLeaderDisplayName( int nPartyID ) const;
	PlayerUID	GetLeaderSID( int nPartyID ) const;

	bool JoinParty( int nPartyID, struct StructPlayer *pPtr );
	bool LeaveParty( int nPartyID, PlayerUID nPlayerUID );

	bool onLogin( int nPartyID, struct StructPlayer *pPtr );
	bool onLogout( int nPartyID, struct StructPlayer *pPtr );

	const _ITEM_SHARE_MODE	GetShareMode( int nPartyID );
	void					SetShareMode( int nPartyID, const _ITEM_SHARE_MODE & eShareMode );
	struct StructPlayer *	GetNextItemAcquirer( struct StructPlayer * pFinder, const std::vector< struct StructPlayer * > & vPlayers );

	std::string GetLinkedPartyInfo( int nPartyID ) const;
	std::string GetSummonInfo( int nPartyID ) const;
	std::string GetLinkedPartySummonInfo( int nPartyID ) const;
	size_t DoEachLinkedPartyMember( int nPartyID, PartyFunctor & _fo );
	size_t DoEachAttackTeamMemberByGuildId( int nGuildID, PartyFunctor & _fo );
	size_t DoEachMember( int nPartyID, PartyFunctor & _fo );
	size_t DoEachMember( int nPartyID, PartyFunctor & _fo ) const;
	size_t DoEachMemberTag( int nPartyID, PartyMemberTagFunctor & _fo );
	size_t DoEachParty( int nPartyID, LinkedPartyFunctor & _fo );
	void   GetNearMember( struct StructPlayer *pPtr, AR_UNIT distance, std::vector< StructPlayer* > & vList ) const;
	void   GetOfflineMember( int nPartyID, std::vector< PartyMemberTag > & vList ) const;

	void Push( GameDBManager::DBProc* pWork );
	void onEndQuery();

	PartyInfo * getPartyInfo( const char *szPartyName );
	const PartyInfo * getPartyInfo( const char *szPartyName ) const;
	PartyInfo * getPartyInfo( int nPartyID );
	const PartyInfo * getPartyInfo( int nPartyID ) const;
	inline XCriticalSection& GetLock()
	{
		return m_IntfLock;
	}

	PartyManager();
	virtual ~PartyManager();

protected:

	// 로딩 관련(PartyLoader.cpp)
	void			loadPartyMemberTagList();
	int				_getPartyMemberTagList( const int party_id, const int leader_sid, std::vector< PartyManager::PartyMemberTag > & result );
	void			loadPartyList();
	void			finishLoading();

	bool leaveParty( PartyInfo *pInfo, PlayerUID nPlayerUID );
	bool joinParty( PartyInfo *pInfo, struct StructPlayer *pPtr );
	bool destroyParty( PartyInfo *pInfo );

	// 연결된 파티 관련
	PartyLinkInfo * makeLinkedParty( PartyInfo * pInfo, int nMaxPartyCount );
	bool joinLinkedParty( PartyLinkInfo * pLinkInfo, PartyInfo * pInfo );

	// 시즈/레이드 공대 파티 관련(AttackTeam)
	AttackTeamInfo * makeAttackTeam( PartyInfo * pInfo, int nGuildID, int nMaxGuildParty );

	PartyInfo * makeParty( int nPartyID, const char *szPartyName, int nLeaderSID, const _ITEM_SHARE_MODE & eShareMode, const _PARTY_TYPE & ePartyType );
	bool	registerParty( PartyInfo* pInfo );

	int allocPartyID();
	static bool isDBUpdateRequiredType( _PARTY_TYPE ePartyType )
	{
		return ( ePartyType != TYPE_HUNTAHOLIC_PARTY && ePartyType != TYPE_BATTLE_ARENA_TEAM && ePartyType != TYPE_BATTLE_ARENA_EXERCISE_TEAM );
	}
	static bool isAliasUsableParty( _PARTY_TYPE ePartyType )
	{
		return ( ePartyType == TYPE_BATTLE_ARENA_TEAM );
	}
	static const char * getDisplayNameForPartyType( _PARTY_TYPE ePartyType, struct StructPlayer * pPlayer );

	const std::string getSummonInfo( int nPartyID ) const;

	size_t doEachMember( PartyInfo * pInfo, PartyFunctor & _fo );
	size_t doEachMember( const PartyInfo * pInfo, PartyFunctor & _fo ) const;
	size_t doEachMemberTag( PartyInfo * pInfo, PartyMemberTagFunctor & _fo );
	size_t doEachParty( PartyLinkInfo * pInfo, LinkedPartyFunctor & _fo );

	bool signAsOnLine( PartyInfo *pInfo, struct StructPlayer *pPtr );
	bool signAsOffLine( PartyInfo *pInfo, struct StructPlayer *pPtr );

	// _ 붙은 소문자로 시작하는 함수는 m_IntfLock 걸린 상태에서만 호출해야하는 내부용 함수
	bool _isAttackTeamParty( const PartyInfo * pInfo ) const			{ return ( pInfo->ePartyType == TYPE_RAID_ATTACKTEAM || pInfo->ePartyType == TYPE_SIEGE_ATTACKTEAM ) && pInfo->pPartyLinkInfo; }
	bool _isBattleArenaTeamParty( const PartyInfo * pInfo ) const		{ return ( pInfo->ePartyType == TYPE_BATTLE_ARENA_TEAM || pInfo->ePartyType == TYPE_BATTLE_ARENA_EXERCISE_TEAM ) && pInfo->pPartyLinkInfo; }

	volatile LONG								m_nMaxPartyID;
	XCriticalSection							m_QueryLock;
	mutable XCriticalSection					m_IntfLock;
	KHash< PartyInfo *, hashPr_string_nocase >	m_hshPartyName;
	KHash< PartyInfo *, hashPr_mod_int >		m_hshPartyID;

	KHash< std::vector< PartyMemberTag > *, hashPr_mod_int >		m_hshPartyMemberTagList;	// 로딩에만 사용

	KHash< AttackTeamInfo *, hashPr_mod_int >	m_hshAttackTeamID;
	std::list< GameDBManager::DBProc * >		m_lQueryList;

	DBConnection								m_DBConn;
};

