#pragma once

#include <vector>
#include <string>

#include <toolkit/ILock.h>
#include <mmo/ArMove.h>
#include <mmo/ArType.h>
#include <toolkit/KHash.h>

#include "GameDBManager.h"
#include "StructGold.h"
#include "StructPlayer.h"


class GuildManager
{
public:

	enum
	{
		MAX_GUILD_MEMBER			= 200,
		PERMISSION_SET_TYPE_COUNT	= 6,	// 멤버 권한 등급 종류 수
	};

	enum _PERMISSION
	{
		PERMISSION_NONE			= 0,	// 권한 없음(길드 소속이 아니거나 권한 정보가 부여되지 않은 경우)

		PERMISSION_MEMBER_LEAST	= 1,	// 길드 멤버 권한 중 최소 권한
		PERMISSION_MEMBER_MAX	= 6,	// 길드 멤버 권한 중 최대 권한
		PERMISSION_LEADER		= 7,	// 길드장 권한
	};

	enum _PERMIT_REQUIRED_ACTION
	{
		PRA_SET_PERMISSION_NAME				= 1 << 0,	// 길드 권한 등급의 이름 변경
		PRA_MEMBER_INVITE					= 1 << 1,	// 길드원 초대
		PRA_MEMBER_KICK						= 1 << 2,	// 길드원 추방
		PRA_GRANT_REVOKE_PERMISSION			= 1 << 3,	// 길드원 승급/강등
		PRA_DUNGEON_MANAGEMENT				= 1 << 4,	// 던전 관리
		PRA_REQUEST_DUNGEON_RAID			= 1 << 5,	// 던전 레이드 신청
		PRA_ATTACK_TEAM_CREATE				= 1 << 6,	// 공격대 결성
		PRA_ATTACK_TEAM_JOIN				= 1 << 7,	// 공격대 참가
		PRA_NOTICE							= 1 << 8,	// 길드 공지
		PRA_MEMBER_MEMO						= 1 << 9,	// 길드원 메모 작성/수정
		PRA_ALLIANCE_MANAGEMENT				= 1 << 10,	// 길드 연합 관리(마스터 길드용)
		PRA_ALLIANCE_JOIN					= 1 << 11,	// 길드 연합 가입(길마 대행 연합 가입)
		PRA_ALLIANCE_LEAVE					= 1 << 12,	// 길드 연합 탈퇴(길마 대행 연합 탈퇴)
		PRA_UPDATE_GUILD_ICON				= 1 << 13,	// 길드 아이콘 변경
		PRA_ADVERTISE_MANAGEMENT			= 1 << 14,	// 길드 광고 관리
		PRA_BUFF							= 1 << 15,
	};

	enum _ADVERTISE_TYPE
	{
		ADV_TYPE_NONE			= 0,		// 광고 안 함
		ADV_TYPE_LIST_ONLY		= 1,		// 길드 목록에만 공개
		ADV_TYPE_NORMAL			= 2,		// 길드 목록 + 일반 광고 등록
		ADV_TYPE_PREMIUM		= 3			// 길드 목록 + 프리미엄 광고 등록
	};

	struct GuildMemberTag
	{
		bool						bIsOnline;
		PlayerUID		sid;
		int							nLevel;			// 오프라인 멤버 참조용
		int							nJobId;			// 오프라인 멤버 참조용
		char						nPermission;
		std::string					strMemo;
#ifdef USE_CLIENT97
		int							nPoint;
		int							nTotalPoint;

		GuildMemberTag(PlayerUID _sid, int lv, int job, char permission, const char* pszMemo,int points, int totalpoints)
			: sid(_sid)
			, nLevel(lv)
			, nJobId(job)
			, nPermission(permission)
			, bIsOnline(false)
			, strMemo(pszMemo)
			, nPoint(points)
			, nTotalPoint(totalpoints)
		{
		}

#else
		GuildMemberTag(PlayerUID _sid, int lv, int job, char permission, const char* pszMemo)
			: sid(_sid)
			, nLevel(lv)
			, nJobId(job)
			, nPermission(permission)
			, bIsOnline(false)
			, strMemo(pszMemo)
		{
		}

#endif

		GuildMemberTag(GuildMemberTag* src)
			: sid(src->sid)
			, nLevel(src->nLevel)
			, nJobId(src->nJobId)
			, nPermission(src->nPermission)
			, bIsOnline(src->bIsOnline)
			, strMemo(src->strMemo)
#ifdef USE_CLIENT97
			, nPoint(src->nPoint)
			, nTotalPoint(src->nTotalPoint)
#endif
		{
		}
	};

	struct GuildFunctor
	{
		virtual bool operator()( AR_HANDLE handle ) = 0;
	};

	struct GuildMemberTagFunctor
	{
		virtual bool operator()( GuildMemberTag * pMemberTag ) = 0;
	};

	struct GuildListFunctor
	{
		virtual bool operator()( int nGuildID, const char * szGuildName, const char * szGuildLeaderName, int nGuildLeaderLevel, int nGuildMemberCount, int nRaidDungeonID ) = 0;
	};

	enum
	{
		DEFAULT_MAX_ALLIANCE_CNT	= 3,
	};

	struct GuildInfo;

	struct AllianceInfo
	{
		AllianceInfo()
		{
			nAllianceID = 0;
			nLeadGuildID = 0;
			nAlliancePassword = 0;
			nMaxAllianceCnt = 0;
			nNameChanged = 0;

#ifdef _MEM_USAGE_DEBUG
			XSEH::IncreaseAllocCount( "AllianceInfo" );
#endif
		}

		~AllianceInfo()
		{
#ifdef _MEM_USAGE_DEBUG
			XSEH::DecreaseAllocCount( "AllianceInfo" );
#endif
		}

		int							nAllianceID;
		int							nLeadGuildID;
		std::string					strAllianceName;
		int							nAlliancePassword;
		int							nMaxAllianceCnt;
		int							nNameChanged;
		std::vector< GuildManager::GuildInfo * >	vGuildList;
	};

	struct GuildInfo
	{
		GuildInfo()
		{
			nGuildPassword = 0;
			nGuildId = 0;

			nLeaderSID = 0;
			nLeaderLevel = 0;

			nIconSize = 0;

			nBannerSize = 0;

			eAdvertiseType = ADV_TYPE_NONE;
			tAdvertiseEnd = 0;

			bRecruiting = false;
			nMinRecruitLevel = 0;
			nMaxRecruitLevel = 0;

			nNameChanged = 0;
			nDungeonId = 0;
			tDungeonBlockTime = 0;
			nChaos = 0;
			pAllianceInfo = NULL;
			tAllianceBlockTime = 0;
			nDonationPoint = 0;
			nGrade = 0;
			nPoint = 0;

			::memset( aPermissionSet, 0, sizeof( aPermissionSet ) );

#ifdef _MEM_USAGE_DEBUG
			XSEH::IncreaseAllocCount( "GuildInfo" );
#endif
		}

		~GuildInfo()
		{
#ifdef _MEM_USAGE_DEBUG
			XSEH::DecreaseAllocCount( "GuildInfo" );
#endif
		}

		int								nGuildPassword;
		int								nGuildId;
		std::string						strGuildName;

		std::string						strNotice;
		std::string						strURL;

		int								nLeaderSID;
		int								nLeaderLevel;

		std::string						strIconFileName;	// game015_0000005849_100326_111217.jpg 형식 - 서버명_길드SID(10자리)_년월일_시분초.jpg
		int								nIconSize;

		std::string						strBannerFileName;
		int								nBannerSize;

		_ADVERTISE_TYPE					eAdvertiseType;
		time_t							tAdvertiseEnd;
		std::string						strAdvertiseComment;

		bool							bRecruiting;
		short							nMinRecruitLevel;
		short							nMaxRecruitLevel;

		int								nNameChanged;
		int								nDungeonId;
		time_t							tDungeonBlockTime;
		StructGold						nGold;
		int								nChaos;
		std::vector< GuildMemberTag >	vMemberNameList;
		std::vector< AR_HANDLE >		vOnlineList;
		AllianceInfo *					pAllianceInfo;
		time_t							tAllianceBlockTime;
		int								nDonationPoint;
		std::string						aPermissionName[ PERMISSION_SET_TYPE_COUNT ];
		int								aPermissionSet[ PERMISSION_SET_TYPE_COUNT ];
		int								nGrade;
		int								nPoint;
	};

	static GuildManager & GetInstance();

	bool				Init();
	bool				DeInit();

	int					GetGuildID( const char *szGuildName );

	void				ForceChangeGuildName( int nGuildID, const char * szGuildName );
	void				ChangeGuildName( int nGuildID, const char * szGuildName, struct StructPlayer *pPtr );
	void				onChangeGuildName( int nGuildID, const char * szGuildName, struct StructPlayer *pPtr );
	void				onChangeGuildNameFailed( const int nGuildID );

	bool				TryMakeGuild( const char *szGuildName, StructPlayer * pLeader, const bool bEnrollInGuildList, const bool bRecruiting );
	bool				onInsertGuildSuccessed( GuildInfo* pInfo, AR_HANDLE hLeader );
	void				onInsertGuildFailed( GuildInfo* pInfo );
	bool				DestroyGuild( int nGuildID );
	unsigned short		IsDestroyableGuild( const int nGuildID );

	const std::string	GetGuildNotice( const int nGuildID ) const;
	const bool			SetGuildNotice( const int nGuildID, const char * pszNotice );

	const std::string	GetGuildURL( const int nGuildID ) const;
	const bool			SetGuildURL( const int nGuildID, const char * pszURL );

	bool				SetRaidDungeonID( int nGuildID, int nDungeonID );
	int					GetRaidDungeonID( int nGuildID );
	bool				SetDungeonBlockTime( int nGuildID, time_t tDungeonBlockTime );
	time_t				GetDungeonBlockTime( int nGuildID );

	int					GetMemberCount( int nGuildID );

	const std::string	GetMemberMemo( const int nGuildID, const PlayerUID nPlayerUID ) const;
	const bool			SetMemberMemo( const int nGuildID, const char * pszPlayerName, const char * pszMemo );

	bool 				IsExistAlliance( const char * szName );
	bool 				CreateAlliance( int nGuildID, const char * szName );
	void 				ChangeAllianceName( int nAllianceID, const char * szNewName, StructPlayer *pPtr );
	void 				onChangeAllianceName( int nAllianceID, const char * szNewName, StructPlayer *pPtr );
	void 				onChangeAllianceNameFailed( const int nAllianceID );
	bool 				AddToAlliance( int nAllianceID, int nGuildID );
	bool 				LeaveAlliance( int nGuildID );
	int					GetAllianceID( int nGuildID );
	int					GetAllianceLeaderGuildID( int nAllianceID );
	bool				IsAllianceLeader( int nAllianceID, int nGuildID );
	int					GetAlliancePassword( int nAllianceID );
	int					GetMaxAllianceCount( int nAllianceID );
	bool				IncMaxAllianceCount( int nAllianceID );
	int					GetAllianceMemberGuildCount( int nAllianceID );
	int					GetAllianceMemberPlayerCount( int nAllianceID );
	bool				DestroyAlliance( int nAllianceID );
	std::string			GetAllianceName( int nAllianceID );
	std::vector< int >	GetAllianceMemberID( int nAllianceID );
	bool				SetGuildAllianceBlockTime( int nGuildID, time_t tAllianceBlockTime );
	bool				IsInGuildAllianceBlockTime( int nGuildID );


	void				OnChangeCharacterName( int nGuildID, const PlayerUID nPlayerUID, const char * szNewName );
	void				OnChangeCharacterLevel( int nGuildID, const PlayerUID nPlayerUID, const int nLevel );
	void				OnChangeCharacterJob( int nGuildID, const PlayerUID nPlayerUID, const int nJobId );

	void				LoadGuildIconInfo( int nGuildID );
	void				LoadGuildBannerInfo( int nGuildID );

	void				GiveTax( int nGuildID, const StructGold & nTax, int nChaos );
	const StructGold	GetTaxAmount( int nGuildID );
	int					GetTaxChaosAmount( int nGuildID );
	const StructGold	DrawTax( int nGuildID );
	int					DrawTaxChaos( int nGuildID, int nChaos );

	bool				GetMemberInfo( int nGuildID, const char* szPlayerName, GuildManager::GuildMemberTag * pMemberInfo );

	int					GetGuildPassword( int nGuildID );

	std::string			GetGuildName( int nGuildID );

	bool				SetGuildIconInfo( int nGuildID, const char * szIconFileName, int nIconSize );
	const std::string	GetGuildIconFileName( int nGuildID ) const;
	const int			GetGuildIconSize( int nGuildID ) const;
	bool				SetGuildBannerInfo( int nGuildID, const char * szBannerFileName, int nBannerSize );
	const std::string	GetGuildBannerFileName( int nGuildID ) const;
	const int			GetGuildBannerSize( int nGuildID ) const;

	const _ADVERTISE_TYPE	GetGuildAdvertiseType( const int nGuildID ) const;
	const time_t			GetGuildAdvertiseEndTime( const int nGuildID ) const;
	const bool				SetGuildAdvertise( const int nGuildID, const _ADVERTISE_TYPE eType, const time_t tEnd );
	const std::string		GetGuildAdvertiseComment( const int nGuildID ) const;
	const bool				SetGuildAdvertiseComment( const int nGuildID, const char * pszComment );

	const bool			IsRecruitingGuild( const int nGuildID ) const;
	const bool			SetGuildRecruiting( const int nGuildID, const bool bRecruiting, const short nMinRecruitLevel, const short nMaxRecruitLevel, const bool bSkipDBUpdate = false );

	const bool			IsLeader( int nGuildID, const PlayerUID nPlayerUID ) const;
	const bool			IsMember( int nGuildID, const char *szPlayerName ) const;

	bool				Promote( int nGuildID, struct StructPlayer *pPtr );
	// 아래는 GM툴 용 강제 길마 변경(조건 검사 부분이 위의 Promote와 조금 다름
	unsigned short		Promote( const int nGuildID, const PlayerUID nNewLeaderUID );

	const bool			SetPermission( const int nGuildID, const char * szPlayerName, const char nPermission );
	const char			GetPermission( const int nGuildID, const char * szPlayerName ) const;
	const bool			IsPermitted( const int nGuildID, const char nPermission, const _PERMIT_REQUIRED_ACTION ePRA ) const;

	void				SetPermissionName( const int nGuildID, const char nPermission, const char * pszName );
	const std::string	GetPermissionName( const int nGuildID, const char nPermission ) const;
	void				SetPermissionSet( const int nGuildID, const char nPermission, const int nPermissionSet );
	const int			GetPermissionSet( const int nGuildID, const char nPermission ) const;

	int					GetMaxLevel( int nGuildID );
	int					GetMinLevel( int nGuildID );
	int					GetTotalLevel( int nGuildID );

	const PlayerUID	GetLeaderSID( const int nGuildID );

	bool				JoinGuild( int nGuildID, struct StructPlayer *pPtr );
	bool				LeaveGuild( int nGuildID, const char *szName );

	bool				onLogin( int nGuildID, struct StructPlayer *pPtr );
	bool				onLogout( int nGuildID, struct StructPlayer *pPtr );

	size_t				DoEachGuild( GuildListFunctor & _fo );
	size_t				DoEachAllianceGuild( int nAllianceID, GuildListFunctor & _fo );

	size_t				DoEachMember( int nGuildID, GuildFunctor & _fo );
	size_t				DoEachMemberTag( int nGuildID, GuildMemberTagFunctor & _fo );
	void  				GetNearMember( struct StructPlayer *pPtr, AR_UNIT distance, std::vector< StructPlayer* > & vList );
	void  				GetOfflineMember( int nGuildID, std::vector< GuildMemberTag > & vList );
	bool				IsExistGuild( const char *szGuildName );

	bool				AddGuildDonationPoint( const int nGuildID, const int nPoint );
	bool				Buff(int nGuildID, int points, int* pnGrade, int* pnPoint, StructPlayer* pPtr, int* pnGuildPoint, int* pnGuildTotalPoint);
	bool				Donate(int nGuildID, int points, int* pnGrade, int* pnPoint, StructPlayer* pPtr, int* pnGuildPoint, int* pnGuildTotalPoint);
	bool				Upgrade(int nGuildID, int* pnGrade, int* pnPoint, StructPlayer* pPtr, int* pnGuildPoint, int* pnGuildTotalPoint);

	void				Push( GameDBManager::DBProc* pWork );
	void				onEndQuery();

	const GuildInfo *		getGuildInfo( const char *szGuildName ) const;
	GuildInfo *				getGuildInfo( const char *szGuildName );
	const GuildInfo *		getGuildInfo( const int nGuildID ) const;
	GuildInfo *				getGuildInfo( const int nGuildID );

	inline XCriticalSection& GetLock()
	{
		return m_IntfLock;
	}

	GuildManager();
	virtual ~GuildManager();

protected:
	void			loadAllianceList();
	void			loadGuildMemberTagList();
	int				_getGuildMemberTagList( const int guild_id, std::vector< GuildManager::GuildMemberTag > & result );
	int				getUserIndexInGuild( const int guild_id, const PlayerUID user_sid );
	void			loadGuildList();
	void			finishLoading();
	bool			updateGuildInfo( int nGuildID, const char * szIconFileName, int nIconSize );
	bool			updateGuildBannerInfo( int nGuildID, const char * szBannerFileName, int nBannerSize );

	AllianceInfo * makeAlliance( int nAllianceId, int nLeadGuildId, const char * szName, int nMaxAllianceCnt, int nNameChanged );
	AllianceInfo * getAllianceInfo( int nAllianceId );
	AllianceInfo * getAllianceInfo( const char * szName );

	unsigned short isDestroyableGuild( GuildInfo *pInfo );
	bool leaveGuild( GuildInfo *pInfo, const char *szName );
	bool joinGuild( GuildInfo *pInfo, struct StructPlayer *pPtr );
	bool destroyGuild( GuildInfo *pInfo );
	GuildInfo * makeGuild( int nGuildID, const char *szGuildName, const char *szGuildNotice, const char *szGuildURL, const char *szIconFileName, int nIconSize,
		const char *szBannerFileName, int nBannerSize, _ADVERTISE_TYPE eAdvertiseType, const char *szAdvertiseComment, time_t tAdvertiseEnd, const bool bRecruiting,
		const short nMinRecruitLevel, const short nMaxRecruitLevel, const int name_changed,
		const int dungeon_id, const time_t dungeon_block_time, const StructGold & gold, const int chaos, time_t alliance_block_time, const int donation_point,
		const char * permission_name_1, const int permission_set_1, const char * permission_name_2, const int permission_set_2,
		const char * permission_name_3, const int permission_set_3, const char * permission_name_4, const int permission_set_4,
		const char * permission_name_5, const int permission_set_5, const char * permission_name_6, const int permission_set_6,
		const int grade, const int points);
	GuildInfo*	allocGuild( int nGuildID, const char *szGuildName, const char *szGuildNotice, const char *szGuildURL, const char *szIconFileName, int nIconSize,
		const char *szBannerFileName, int nBannerSize, _ADVERTISE_TYPE eAdvertiseType, const char *szAdvertiseComment, time_t tAdvertiseEnd, const bool bRecruiting,
		const short nMinRecruitLevel, const short nMaxRecruitLevel, const int name_changed,
		const int dungeon_id, const time_t dungeon_block_time, const StructGold & gold, const int chaos, time_t alliance_block_time, const int donation_point,
		const char * permission_name_1, const int permission_set_1, const char * permission_name_2, const int permission_set_2,
		const char * permission_name_3, const int permission_set_3, const char * permission_name_4, const int permission_set_4,
		const char * permission_name_5, const int permission_set_5, const char * permission_name_6, const int permission_set_6,
		const int grade, const int points);
	bool	registerGuild( GuildInfo* pInfo );
	int allocGuildId();
	int allocAllianceId();

	size_t doEachMember( GuildInfo *pInfo, GuildFunctor & _fo );
	size_t doEachMemberTag( GuildInfo *pInfo, GuildMemberTagFunctor & _fo );

	bool signAsOnLine( GuildInfo *pInfo, struct StructPlayer *pPtr );
	bool signAsOffLine( GuildInfo *pInfo, struct StructPlayer *pPtr );

	volatile LONG								m_nMaxGuildId;
	volatile LONG								m_nMaxAllianceId;
	XCriticalSection							m_QueryLock;
	mutable XCriticalSection					m_IntfLock;

	KHash< GuildInfo*, hashPr_string_nocase >	m_hshGuildName;
	KHash< GuildInfo*, hashPr_mod_int >			m_hshGuildID;

	KHash< std::vector< GuildMemberTag > *, hashPr_mod_int >			m_hshGuildMemberTagList;	// 로딩에만 사용

	std::vector< GuildInfo* >					m_vGuildList;
	std::vector< AllianceInfo* >				m_vAllianceList;

	std::list< GameDBManager::DBProc* >			m_lQueryList;

	DBConnection								m_DBConn;
};

