
#include <toolkit/XEnv.h>
#include <toolkit/XRandom.h>
#include <toolkit/XSTLUtil.h>
#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "GuildManager.h"
#include "PartyManager.h"
#include "StructPlayer.h"
#include "DB_Commands.h"
#include "GameMessage.h"
#include "SendMessage.h"
#include "GameDBUtil.h"
#include "DB_Commands.h"
#include "DungeonManager.h"
#include "GameRule.h"

static GuildManager _inst;


GuildManager::GuildManager()
	: m_QueryLock( "GuildManager::m_QueryLock" )
	, m_IntfLock( "GuildManager::m_IntfLock" )
{
	m_nMaxGuildId = 0;
	m_nMaxAllianceId = 0;
}

GuildManager::~GuildManager()
{
}

GuildManager & GuildManager::GetInstance()
{
	return _inst;
}

bool GuildManager::Init()
{
	InitUserDbConnection( m_DBConn.connection );
	if( m_DBConn.CreateCommand( m_DBConn.command ) == false )
	{
		return false;
	}

	SCOPED_ENV( "stat.loading_guild_info" );

	m_nMaxGuildId = 0;
	m_nMaxAllianceId = 0;

	// DB 길드 리스트 주욱 읽어옴
	loadGuildMemberTagList();	// Character 테이블에서 먼저 길드 소속 아바타 정보를 읽어서 GuildMemberTag 벡터 생성
	loadAllianceList();			// 연합 정보 로딩
	loadGuildList();			// 길드 정보 로딩 및 생성된 GuildMemberTag 벡터에서 해당 길드에 맞는 벡터 복사 및 소거
	finishLoading();			// 쓰고 남은 떨거지 청소(떨거지가 있다면 해당 길드 ID인 캐릭터는 있으나 Guild 는 없는 것)

	return true;
}

bool GuildManager::DeInit()
{
	SCOPED_ENV( "stat.saving_guild_info" );

	// 쿼리 다 완결될때까지 대기
	while( !m_lQueryList.empty() )
	{
		Sleep( 100 );
	}

	return true;
}

int	 GuildManager::GetMemberCount( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	return static_cast< int >( pInfo->vMemberNameList.size() );
}

int GuildManager::GetGuildID( const char *szGuildName )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( szGuildName );
	if( !pInfo ) return 0;

	return pInfo->nGuildId;
}

bool GuildManager::SetGuildAllianceBlockTime( int nGuildID, time_t tAllianceBlockTime )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	pInfo->tAllianceBlockTime = tAllianceBlockTime;

	Push( new DB_SetGuildAllianceBlockTime( nGuildID, tAllianceBlockTime ) );

	return true;
}

bool GuildManager::IsInGuildAllianceBlockTime( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	return ( pInfo->tAllianceBlockTime > time( NULL ) );
}

const std::string GuildManager::GetMemberMemo( const int nGuildID, const PlayerUID nPlayerUID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return "";

	for( std::vector< GuildMemberTag >::const_iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( it->sid != nPlayerUID )
			continue;

		return it->strMemo;
	}

	return "";
}

const bool GuildManager::SetMemberMemo( const int nGuildID, const char * pszPlayerName, const char * pszMemo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( _stricmp( StructPlayer::GetPlayerName( it->sid ), pszPlayerName ) )
			continue;

		it->strMemo = pszMemo;

		Push( new DB_SetGuildMemberMemo( it->sid, pszMemo ) );
		return true;
	}

	return false;
}

GuildManager::GuildInfo * GuildManager::makeGuild( int nGuildID, const char *szGuildName, const char *szGuildNotice, const char *szGuildURL, const char *szIconFileName, int nIconSize,
				const char *szBannerFileName, int nBannerSize, _ADVERTISE_TYPE eAdvertiseType, const char *szAdvertiseComment, time_t tAdvertiseEnd, const bool bRecruiting,
				const short nMinRecruitLevel, const short nMaxRecruitLevel, const int name_changed,
				const int dungeon_id, const time_t dungeon_block_time, const StructGold & gold, const int chaos, time_t alliance_block_time, const int donation_point,
				const char * permission_name_1, const int permission_set_1, const char * permission_name_2, const int permission_set_2,
				const char * permission_name_3, const int permission_set_3, const char * permission_name_4, const int permission_set_4,
				const char * permission_name_5, const int permission_set_5, const char * permission_name_6, const int permission_set_6,
				const int grade, const int points)
{
	// 길드 구조체 생성
	GuildInfo *pInfo = allocGuild( nGuildID, szGuildName,
		szGuildNotice, szGuildURL, szIconFileName, nIconSize, szBannerFileName, nBannerSize,
		eAdvertiseType, szAdvertiseComment, tAdvertiseEnd,
		bRecruiting, nMinRecruitLevel, nMaxRecruitLevel,
		name_changed, dungeon_id, dungeon_block_time,
		gold, chaos, alliance_block_time, donation_point,
		permission_name_1, permission_set_1,
		permission_name_2, permission_set_2,
		permission_name_3, permission_set_3,
		permission_name_4, permission_set_4,
		permission_name_5, permission_set_5,
		permission_name_6, permission_set_6,
		grade, points);

	if( pInfo == NULL )
	{
		return NULL;
	}

	if( registerGuild( pInfo ) == false )
	{
		delete pInfo;
		return NULL;
	}

	return pInfo;
}

GuildManager::GuildInfo*	GuildManager::allocGuild( int nGuildID, const char *szGuildName, const char *szGuildNotice, const char *szGuildURL, const char *szIconFileName, int nIconSize,
					   const char *szBannerFileName, int nBannerSize, _ADVERTISE_TYPE eAdvertiseType, const char *szAdvertiseComment, time_t tAdvertiseEnd, const bool bRecruiting,
					   const short nMinRecruitLevel, const short nMaxRecruitLevel, const int name_changed,
					   const int dungeon_id, const time_t dungeon_block_time, const StructGold & gold, const int chaos, time_t alliance_block_time, const int donation_point,
					   const char * permission_name_1, const int permission_set_1, const char * permission_name_2, const int permission_set_2,
					   const char * permission_name_3, const int permission_set_3, const char * permission_name_4, const int permission_set_4,
					   const char * permission_name_5, const int permission_set_5, const char * permission_name_6, const int permission_set_6,
						const int grade, const int points)
{
	// 길드 구조체 생성
	GuildInfo *pInfo = new GuildInfo;
	pInfo->strGuildName = szGuildName;
	pInfo->strNotice = szGuildNotice;
	pInfo->strURL = szGuildURL;
	pInfo->nGuildId = nGuildID;
	pInfo->nGuildPassword = XRandom();
	pInfo->nLeaderSID = 0;

	pInfo->strIconFileName = szIconFileName;
	pInfo->nIconSize = nIconSize;

	pInfo->strBannerFileName = szBannerFileName;
	pInfo->nBannerSize = nBannerSize;

	pInfo->eAdvertiseType = eAdvertiseType;
	pInfo->strAdvertiseComment = szAdvertiseComment;
	pInfo->tAdvertiseEnd = tAdvertiseEnd;

	pInfo->bRecruiting = bRecruiting;
	pInfo->nMinRecruitLevel = nMinRecruitLevel;
	pInfo->nMaxRecruitLevel = nMaxRecruitLevel;

	pInfo->nNameChanged = name_changed;
	pInfo->nDungeonId = dungeon_id;
	pInfo->tDungeonBlockTime = dungeon_block_time;
	pInfo->nGold = gold;
	pInfo->nChaos = chaos;
	pInfo->pAllianceInfo = NULL;
	pInfo->tAllianceBlockTime = alliance_block_time;
	pInfo->nDonationPoint = donation_point;

	pInfo->aPermissionName[ 0 ] = permission_name_1;
	pInfo->aPermissionSet[ 0 ] = permission_set_1;
	pInfo->aPermissionName[ 1 ] = permission_name_2;
	pInfo->aPermissionSet[ 1 ] = permission_set_2;
	pInfo->aPermissionName[ 2 ] = permission_name_3;
	pInfo->aPermissionSet[ 2 ] = permission_set_3;
	pInfo->aPermissionName[ 3 ] = permission_name_4;
	pInfo->aPermissionSet[ 3 ] = permission_set_4;
	pInfo->aPermissionName[ 4 ] = permission_name_5;
	pInfo->aPermissionSet[ 4 ] = permission_set_5;
	pInfo->aPermissionName[ 5 ] = permission_name_6;
	pInfo->aPermissionSet[ 5 ] = permission_set_6;
	pInfo->nGrade = grade;
	pInfo->nPoint = points;

	return pInfo;
}

bool	GuildManager::registerGuild( GuildManager::GuildInfo* pInfo )
{
	// 해쉬에 등록
	if( m_hshGuildID.add( pInfo->nGuildId, pInfo ) == NULL )
	{
		return false;
	}

	if( m_hshGuildName.add( pInfo->strGuildName.c_str(), pInfo ) == NULL )
	{
		int nHas = (m_hshGuildName.has( pInfo->strGuildName.c_str() ) ? 1 : 0);
		FILELOG( "(register) KHash add failed GuildID: %d, GuildName: %s(has: %d)", pInfo->nGuildId, pInfo->strGuildName.c_str(), nHas );
		_cprint( "(register) KHash add failed GuildID: %d, GuildName: %s(has: %d)\n", pInfo->nGuildId, pInfo->strGuildName.c_str(), nHas );

		m_hshGuildID.erase( pInfo->nGuildId );
		return false;
	}

	m_vGuildList.push_back( pInfo );
	return true;
}

bool	GuildManager::TryMakeGuild( const char *szGuildName, StructPlayer * pLeader, const bool bEnrollInGuildList, const bool bRecruiting )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( szGuildName );
	if( pInfo ) return false;

	const int nGuildId = allocGuildId();
	const short nMinRecruitLevel = 1;
	const short nMaxRecruitLevel = GameRule::nMaxLevel;

	pInfo = allocGuild( nGuildId, szGuildName, "", "", "", 0, "", 0, ( bEnrollInGuildList ) ? ADV_TYPE_LIST_ONLY : ADV_TYPE_NONE, "", 0,
		bRecruiting, nMinRecruitLevel, nMaxRecruitLevel,
		1, 0, 0, StructGold( 0 ), 0, 0, 0,
		GameContent::GetString( 1890 ), 0, GameContent::GetString( 1889 ), 0,
		GameContent::GetString( 1888 ), 0, GameContent::GetString( 1887 ), 0,
		GameContent::GetString( 1886 ), 0, GameContent::GetString( 1885 ), 0,1,0 );
	if( pInfo == NULL )
	{
		return false;
	}

	pInfo->nLeaderSID = pLeader->GetPlayerUID();

	// DB 에 추가(길마 권한 설정을 Character 테이블에 해주는 것도 InsertGuild에서 함)
	Push( new DB_InsertGuild( pInfo, szGuildName, nGuildId, pLeader->GetPlayerUID(), pLeader->GetHandle(), ( bEnrollInGuildList ) ? ADV_TYPE_LIST_ONLY : ADV_TYPE_NONE,
		bRecruiting, nMinRecruitLevel, nMaxRecruitLevel,
		GameContent::GetString( 1890 ), GameContent::GetString( 1889 ), GameContent::GetString( 1888 ),
		GameContent::GetString( 1887 ), GameContent::GetString( 1886 ), GameContent::GetString( 1885 ) ) );

	joinGuild( pInfo, pLeader );

	LOG::Log11N4S( LM_GUILD_CREATE, pLeader->GetAccountID(), pLeader->GetSID(), 0, pLeader->GetGuildID(), 0, 0, 0, 0, 0, 0, 0, pLeader->GetAccountName(), LOG::STR_NTS, pLeader->GetName(), LOG::STR_NTS, pInfo->strGuildName.c_str(), LOG::STR_NTS, "", 0 );

	return true;
}

bool GuildManager::onInsertGuildSuccessed( GuildInfo* pInfo, AR_HANDLE hLeader )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	if( registerGuild( pInfo ) == false )
	{
		onInsertGuildFailed( pInfo );
		return false;
	}

	StructPlayer* pLeader = *StructPlayer::get( hLeader );
	if( pLeader != NULL )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pLeader ) );

		if( pInfo->nLeaderSID == pLeader->GetPlayerUID() )
		{
			std::string strMsg;
			strMsg += "@601\v#@guild_name@#\v";
			strMsg += pInfo->strGuildName.c_str();
			strMsg += "\v#@user_name@#\v";
			strMsg += pLeader->GetName();

			SendGlobalChatMessage( CHAT_GLOBAL, "@GUILD", strMsg.c_str(), static_cast<unsigned int>( strMsg.size() ) );

			SendGuildNotify( pLeader );

			SendGuildInfo( pLeader );
		}
		else
		{
			_cprint( "%s(%s) is not Guild leader(%d)\n", pLeader->GetName(), pLeader->GetPlayerUID(), pInfo->nLeaderSID );
			FILELOG( "%s(%s) is not Guild leader(%d)", pLeader->GetName(), pLeader->GetPlayerUID(), pInfo->nLeaderSID );
		}
	}

	return true;
}

void	GuildManager::onInsertGuildFailed( GuildInfo* pInfo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	LOG::Log11N4S( LM_GUILD_DESTROY,
		0, 0, 0,
		pInfo->nGuildId,
		0, 0, 0, 0, 0, 0, 0,
		"DBError", LOG::STR_NTS, "DBError", LOG::STR_NTS, pInfo->strGuildName.c_str(), LOG::STR_NTS,
		"", 0 );

	struct myGuildClearFunctor : GuildFunctor
	{
		myGuildClearFunctor()
		{}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( !pPlayer ) return true;

			pPlayer->SetGuildPermission( PERMISSION_NONE );
			pPlayer->SetGuildBlockTime( 0 );
			pPlayer->SetPrevGuildID( 0 );
			pPlayer->SetGuildID( 0 );

			return true;
		}
	} _fo;


	// 온라인 플레이어들 길드 정보 클리어 및 길드 재가입 제한 시간 룰 적용
	doEachMember( pInfo, _fo );

	// 모든 길드원 관련 DB에 길드 재가입 제한 시간 룰 적용
	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin() ; it != pInfo->vMemberNameList.end() ; ++it )
	{
		Push( new DB_SetGuildBlockTime( it->sid, 0 ) );
		Push( new DB_SetGuild( it->sid, 0, 0 ) );
	}

	// DB 에서 길드 제거
	Push( new DB_DeleteGuild( pInfo->nGuildId ) );

	// 길드 정보 지움
	delete pInfo;
}

const std::string GuildManager::GetGuildNotice( const int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return "";

	return pInfo->strNotice;
}

const bool GuildManager::SetGuildNotice( const int nGuildID, const char * pszNotice )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	pInfo->strNotice = pszNotice;

	// | 문자는 길드 정보 방송 시 토큰 구분자로 사용되므로 해당 문자는 사용할 수 없음
	for( size_t nIdx = pInfo->strNotice.find( '|' ) ; nIdx != std::string::npos ; nIdx = pInfo->strNotice.find( '|', nIdx ) )
	{
		pInfo->strNotice[ nIdx ] = ' ';
	}

	Push( new DB_SetGuildNotice( nGuildID, pszNotice ) );

	return true;
}

const std::string GuildManager::GetGuildURL( const int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return "";

	return pInfo->strURL;
}

const bool GuildManager::SetGuildURL( const int nGuildID, const char * pszURL )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	pInfo->strURL = pszURL;

	Push( new DB_SetGuildURL( nGuildID, pszURL ) );

	return true;
}

int GuildManager::GetRaidDungeonID( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	return pInfo->nDungeonId;
}

bool GuildManager::SetRaidDungeonID( int nGuildID, int nDungeonID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	if( nDungeonID == 0 )
	{
		if( !pInfo->nDungeonId )
		{
			return false;
		}
	}
	else if( pInfo->nDungeonId ) return false;

	Push( new DB_SetGuildDungeonID( nGuildID, nDungeonID ) );

	pInfo->nDungeonId = nDungeonID;

	return true;
}

bool GuildManager::SetDungeonBlockTime( int nGuildID, time_t tDungeonBlockTime )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	Push( new DB_SetGuildDungeonBlockTime( nGuildID, tDungeonBlockTime ) );

	pInfo->tDungeonBlockTime = tDungeonBlockTime;

	return true;
}

time_t GuildManager::GetDungeonBlockTime( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return NULL;

	if( pInfo->tDungeonBlockTime && pInfo->tDungeonBlockTime < time( NULL ) )
	{
		pInfo->tDungeonBlockTime = 0;
		Push( new DB_SetGuildDungeonBlockTime( nGuildID, 0 ) );
	}

	return pInfo->tDungeonBlockTime;
}

bool GuildManager::Promote( int nGuildID, struct StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	Push( new DB_SetGuildLeader( nGuildID, pPtr->GetPlayerUID() ) );

	StructPlayer * pPrevLeader = NULL;
	AR_HANDLE hPrevLeader = StructPlayer::FindPlayer( StructPlayer::GetPlayerName( pInfo->nLeaderSID ) );
	if( hPrevLeader )
	{
		StructPlayer::iterator it = StructPlayer::get( hPrevLeader );
		pPrevLeader = (*it);

		if( !SetPermission( nGuildID, pPrevLeader->GetName(), PERMISSION_MEMBER_LEAST ) )
			return false;
	}

	if( !SetPermission( nGuildID, pPtr->GetName(), PERMISSION_LEADER ) )
	{
		if( pPrevLeader )
			SetPermission( nGuildID, pPrevLeader->GetName(), PERMISSION_LEADER );
		return false;
	}
	
	pInfo->nLeaderLevel = pPtr->GetLevel();
	pInfo->nLeaderSID = pPtr->GetPlayerUID();

	return true;
}

unsigned short GuildManager::Promote( const int nGuildID, const PlayerUID nNewLeaderUID )
{
	// 공대 파티에 소속되어 있으면 길드장 인계를 할 수 없음
	if( PartyManager::GetInstance().IsExistAttackTeam( nGuildID ) )
	{
		return RESULT_ACCESS_DENIED;
	}

	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return RESULT_NOT_EXIST;

	// 이미 길드장이면 인계 불가
	if( nNewLeaderUID == pInfo->nLeaderSID )
	{
		return RESULT_ALREADY_EXIST;
	}

	int nNewLeaderLevel = 0;

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin() ; it != pInfo->vMemberNameList.end() ; ++it )
	{
		if( (*it).sid == nNewLeaderUID )
		{
			nNewLeaderLevel = (*it).nLevel;
			break;
		}
	}

	// 길드원이 아니면 길드장이 될 수 없음
	if( !nNewLeaderLevel )
	{
		return RESULT_NOT_ACTABLE;
	}

	Push( new DB_SetGuildLeader( nGuildID, nNewLeaderUID ) );

	pInfo->nLeaderLevel = nNewLeaderLevel;
	pInfo->nLeaderSID = nNewLeaderUID;

	return RESULT_SUCCESS;
}

const bool GuildManager::SetPermission( const int nGuildID, const char * szPlayerName, const char nPermission )
{
	if( nPermission < PERMISSION_MEMBER_LEAST || nPermission > PERMISSION_LEADER )
	{
		assert( 0 );
		return false;
	}

	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo )
	{
		assert( 0 );
		return false;
	}

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin() ; it != pInfo->vMemberNameList.end() ; ++it )
	{
		if( _stricmp( StructPlayer::GetPlayerName( (*it).sid ), szPlayerName ) )
			continue;

		if( (*it).nPermission == nPermission )
			return false;

		(*it).nPermission = nPermission;

		if( (*it).bIsOnline )
		{
			for( std::vector< AR_HANDLE >::iterator pit = pInfo->vOnlineList.begin() ; pit != pInfo->vOnlineList.end() ; ++pit )
			{
				StructPlayer * pPlayer = *StructPlayer::get( StructPlayer::FindPlayer( szPlayerName ) );

				if( !pPlayer )
					continue;

				pPlayer->SetGuildPermission( nPermission );
				break;
			}
		}

		Push( new DB_SetGuildPermission( (*it).sid, nPermission ) );
	}

	return true;
}

const char GuildManager::GetPermission( const int nGuildID, const char * szPlayerName ) const
{
	char nPermission = PERMISSION_NONE;

	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo )
		return nPermission;

	std::vector< GuildMemberTag >::const_iterator it;

	for( it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( _stricmp( StructPlayer::GetPlayerName( it->sid ), szPlayerName ) )
			continue;

		nPermission = it->nPermission;
		break;
	}

	return nPermission;
}

const bool GuildManager::IsPermitted( const int nGuildID, const char nPermission, const _PERMIT_REQUIRED_ACTION ePRA ) const
{
	if( !nGuildID )
		return false;

	if( nPermission == PERMISSION_LEADER )
		return true;

	if( nPermission < PERMISSION_MEMBER_LEAST || nPermission > PERMISSION_MEMBER_MAX )
	{
		assert( 0 );
		return false;
	}

	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	const GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo )
		return false;

	return ( pInfo->aPermissionSet[ nPermission - 1 ] & static_cast< int >( ePRA ) ) != 0;
}

void GuildManager::SetPermissionName( const int nGuildID, const char nPermission, const char * pszName )
{
	if( nPermission < PERMISSION_MEMBER_LEAST || nPermission > PERMISSION_MEMBER_MAX || !pszName || strlen( pszName ) < 4 )
	{
		assert( 0 );
		return;
	}

	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo )
	{
		assert( 0 );
		return;
	}

	pInfo->aPermissionName[ nPermission - 1 ] = pszName;

	Push( new DB_SetGuildPermissionName( nGuildID, nPermission, pszName ) );
}

const std::string GuildManager::GetPermissionName( const int nGuildID, const char nPermission ) const
{
	if( nPermission < PERMISSION_MEMBER_LEAST || nPermission > PERMISSION_MEMBER_MAX )
	{
		assert( 0 );
		return "";
	}

	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	const GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo )
	{
		assert( 0 );
		return 0;
	}

	return pInfo->aPermissionName[ nPermission - 1 ];
}

void GuildManager::SetPermissionSet( const int nGuildID, const char nPermission, const int nPermissionSet )
{
	if( nPermission < PERMISSION_MEMBER_LEAST || nPermission > PERMISSION_MEMBER_MAX )
	{
		assert( 0 );
		return;
	}

	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo )
	{
		assert( 0 );
		return;
	}

	pInfo->aPermissionSet[ nPermission - 1 ] = nPermissionSet;

	Push( new DB_SetGuildPermissionSet( nGuildID, nPermission, nPermissionSet ) );
}

const int GuildManager::GetPermissionSet( const int nGuildID, const char nPermission ) const
{
	if( nPermission < PERMISSION_MEMBER_LEAST || nPermission > PERMISSION_MEMBER_MAX )
	{
		assert( 0 );
		return 0;
	}

	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	const GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo )
	{
		assert( 0 );
		return 0;
	}

	return pInfo->aPermissionSet[ nPermission - 1 ];
}

const PlayerUID GuildManager::GetLeaderSID( const int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	return pInfo->nLeaderSID;
}

int GuildManager::GetTotalLevel( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	int nTotalLevel = 0;
	std::vector< AR_HANDLE >::iterator it;
	for( it = pInfo->vOnlineList.begin(); it != pInfo->vOnlineList.end(); ++it )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( *it ) );
		if( !pPlayer ) continue;

		nTotalLevel += pPlayer->GetLevel();
	}

	return nTotalLevel;
}

int  GuildManager::GetMaxLevel( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	int nMaxLevel = 0;
	std::vector< AR_HANDLE >::iterator it;
	for( it = pInfo->vOnlineList.begin(); it != pInfo->vOnlineList.end(); ++it )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( *it ) );
		if( !pPlayer ) continue;

		if( nMaxLevel < pPlayer->GetLevel() ) nMaxLevel = pPlayer->GetLevel();
	}

	return nMaxLevel;
}

int  GuildManager::GetMinLevel( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	int nMinLevel = 999;
	std::vector< AR_HANDLE >::iterator it;
	for( it = pInfo->vOnlineList.begin(); it != pInfo->vOnlineList.end(); ++it )
	{
		StructPlayer *pPlayer = static_cast< StructPlayer * >( GameObject::raw_get( *it ) );
		if( !pPlayer ) continue;

		if( nMinLevel > pPlayer->GetLevel() ) nMinLevel = pPlayer->GetLevel();
	}

	return nMinLevel;
}


bool GuildManager::joinGuild( GuildInfo *pInfo, struct StructPlayer *pPtr )
{
	if( !pInfo ) return false;

	// 길드에 최초로 가입한 유저는 길드장
	_PERMISSION ePermission = PERMISSION_MEMBER_LEAST;
	if( pInfo->vMemberNameList.empty() )
		ePermission = PERMISSION_LEADER;

	// 길드 리스트에 추가
	StructPlayer::RegisterPlayerName( pPtr->GetPlayerUID(), pPtr->GetName() );
	pPtr->SetGuildPoint(0);
#ifdef USE_CLIENT97
	pInfo->vMemberNameList.push_back( GuildMemberTag( pPtr->GetPlayerUID(), pPtr->GetLevel(), pPtr->GetJobId(), ePermission, "", pPtr->GetGuildPoint(),0 ) );
#else
	pInfo->vMemberNameList.push_back(GuildMemberTag(pPtr->GetPlayerUID(), pPtr->GetLevel(), pPtr->GetJobId(), ePermission, ""));
#endif

	// 온라인으로 설정
	signAsOnLine( pInfo, pPtr );

	pPtr->SetGuildPermission( ePermission );
	pPtr->SetGuildID( pInfo->nGuildId );
	pPtr->SetPrevGuildID( 0 );

	Push( new DB_SetGuild( pPtr->GetPlayerUID(), pInfo->nGuildId, 0 ) );
	if( ePermission != PERMISSION_MEMBER_LEAST )
		Push( new DB_SetGuildPermission( pPtr->GetPlayerUID(), ePermission ) );

	return true;
}

void GuildManager::OnChangeCharacterName( int nGuildID, const PlayerUID nPlayerUID, const char * szNewName )
{
	PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "CHANGE_NAME|%s|%s|", StructPlayer::GetPlayerName( nPlayerUID ), szNewName );
}

void GuildManager::OnChangeCharacterLevel( int nGuildID, const PlayerUID nPlayerUID, const int nLevel )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return;

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( it->sid == nPlayerUID )
		{
			it->nLevel = nLevel;

			PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "CHANGE_LEVEL|%s|%d|", StructPlayer::GetPlayerName( nPlayerUID ), nLevel );

			break;
		}
	}
}

void GuildManager::OnChangeCharacterJob( int nGuildID, const PlayerUID nPlayerUID, const int nJobId )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return;

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( it->sid == nPlayerUID )
		{
			it->nJobId = nJobId;

			PrintfGuildChatMessage( CHAT_GUILD_SYSTEM, nGuildID, "CHANGE_JOB|%s|%d|", StructPlayer::GetPlayerName( nPlayerUID ), nJobId );

			break;
		}
	}
}

bool GuildManager::JoinGuild( int nGuildID, struct StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return joinGuild( getGuildInfo( nGuildID ), pPtr );
}

int GuildManager::GetTaxChaosAmount( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	return pInfo->nChaos;
}

const StructGold GuildManager::GetTaxAmount( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return StructGold( 0 );

	return pInfo->nGold;
}

const StructGold GuildManager::DrawTax( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return StructGold( 0 );

	StructGold gold = pInfo->nGold;

	pInfo->nGold.SetRawData( 0 );

	Push( new DB_SetGuildGold( nGuildID, pInfo->nGold, pInfo->nChaos ) );

	return gold;

}

int GuildManager::DrawTaxChaos( int nGuildID, int nChaos )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	int chaos = std::max( std::min( pInfo->nChaos, nChaos ), 0 );

	pInfo->nChaos -= chaos;

	Push( new DB_SetGuildGold( nGuildID, pInfo->nGold, pInfo->nChaos ) );

	return chaos;

}

void GuildManager::GiveTax( int nGuildID, const StructGold & nTax, int nChaos )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return;

	pInfo->nGold += nTax;
	pInfo->nChaos += nChaos;

	Push( new DB_SetGuildGold( nGuildID, pInfo->nGold, pInfo->nChaos ) );
}

int GuildManager::GetGuildPassword( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 길드 구조체 얻기
	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return -1;

	return pInfo->nGuildPassword;
}

bool GuildManager::LeaveGuild( int nGuildID, const char *szName )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return leaveGuild( getGuildInfo( nGuildID ), szName );
}

bool GuildManager::leaveGuild( GuildInfo *pInfo, const char *szName )
{
	if( !pInfo ) return false;

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		PlayerUID nPlayerSID = it->sid;

		if( !_stricmp( StructPlayer::GetPlayerName( nPlayerSID ), szName ) )
		{
			time_t guild_block_time = time(NULL) + GameRule::GUILD_REJOIN_BLOCK_DURATION_SEC;
			// 온라인 상태라면 길드 정보 제거
			StructPlayer::iterator pit = StructPlayer::get( StructPlayer::FindPlayer( szName ) );
			StructPlayer *pPtr = *pit;
			if( pPtr ) 
			{
				if( pPtr->IsInSiegeOrRaidDungeon() )
					return false;

				pPtr->SetGuildID( 0 );
				pPtr->SetPrevGuildID( pInfo->nGuildId );
				signAsOffLine( pInfo, pPtr );

				pPtr->SetGuildBlockTime( guild_block_time );
				pPtr->SetGuildPermission( PERMISSION_NONE );
			}

			Push( new DB_SetGuild( nPlayerSID, 0, pInfo->nGuildId ) );
			Push( new DB_SetGuildBlockTime( nPlayerSID, guild_block_time ) );

			// 멤버 리스트에서 제거
			pInfo->vMemberNameList.erase( it );

			return true;
		}
	}

	return false;
}

bool GuildManager::DestroyGuild( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return destroyGuild( getGuildInfo( nGuildID ) );
}

unsigned short GuildManager::IsDestroyableGuild( const int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return isDestroyableGuild( getGuildInfo( nGuildID ) );
}

unsigned short GuildManager::isDestroyableGuild( GuildInfo *pInfo )
{
	if( !pInfo )
	{
		return RESULT_NOT_EXIST;
	}

	// 길드 연합, 공대파티 소속 길드는 해산 불가
	if( pInfo->pAllianceInfo || PartyManager::GetInstance().IsExistAttackTeam( pInfo->nGuildId ) )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 던전 관련
	if( pInfo->nDungeonId )
	{
		// 던전 소유 길드는 해산 불가
		if( DungeonManager::Instance().GetOwnGuildID( pInfo->nDungeonId ) == pInfo->nGuildId )
		{
			return RESULT_NOT_ACTABLE;
		}

		// 관련 던전에 시즈가 진행 중이면 해산 불가
		if( DungeonManager::Instance().IsSiegeBegin( pInfo->nDungeonId ) )
		{
			return RESULT_TARGET_IN_SIEGE_OR_RAID;
		}

		// 레이드 기간이라면 일반 신청 상태이므로 레이드 완료 기록 삭제 후 재시도 할 수 있도록 개별적인 오류 코드를 반환
		if( DungeonManager::Instance().IsDungeonRaidTime( pInfo->nDungeonId ) )
		{
			return RESULT_NOT_ACTABLE_IN_SIEGE_OR_RAID;
		}
		// 던전 시즈 공격자 길드는 해산 불가(레이드 기간이 아닌데 소유자가 아니면 공격자임)
		else
		{
			return RESULT_NOT_ACTABLE;
		}
	}

	return RESULT_SUCCESS;
}

void GuildManager::GetNearMember( struct StructPlayer *pPtr, AR_UNIT distance, std::vector< StructPlayer* > & vList )
{
	AR_TIME t = GetArTime();

	struct myGuildFunctor : GuildManager::GuildFunctor
	{
		myGuildFunctor( const ArPosition & _pos, AR_TIME _t, AR_UNIT _dist, std::vector< StructPlayer* > * pList ) : pos( _pos ), t( _t ), distance( _dist ), pvList( pList )
		{
		}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( !pPlayer || !pPlayer->IsInWorld() ) return true;

			// 멀리 떨어져 있으면 KIN			
			AR_UNIT d = pos.GetDistance( pPlayer->GetPos() );	
			if( d > distance ) return true;

			pvList->push_back( pPlayer );

			return true;
		}

		ArPosition pos;
		AR_TIME t;
		AR_UNIT distance;
		std::vector< StructPlayer* > *pvList;

	} _tmp( pPtr->GetPos(), t, distance, &vList );

	DoEachMember( pPtr->GetGuildID(), _tmp );
}

size_t GuildManager::DoEachGuild( GuildListFunctor & _fo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	size_t cnt = 0;
	for( std::vector< GuildInfo * >::iterator it = m_vGuildList.begin(); it != m_vGuildList.end(); ++it )
	{
		_fo( (*it)->nGuildId, (*it)->strGuildName.c_str(), StructPlayer::GetPlayerName( (*it)->nLeaderSID ), (*it)->nLeaderLevel, static_cast<int>( (*it)->vMemberNameList.size() ), (*it)->nDungeonId );
		++cnt;
	}

	return cnt;
}

size_t GuildManager::DoEachAllianceGuild( int nAllianceID, GuildListFunctor & _fo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pAllianceInfo = getAllianceInfo( nAllianceID );

	if( !pAllianceInfo )
	{
		return 0;
	}

	size_t cnt = 0;
	for( std::vector< GuildInfo * >::iterator it = pAllianceInfo->vGuildList.begin(); it != pAllianceInfo->vGuildList.end(); ++it )
	{
		// GuildListFunctor 에서 true를 반환해야만 카운트 - DoEachAllianceGuild 함수의 반환값은 처리 성공 길드 개수
		if( _fo( (*it)->nGuildId, (*it)->strGuildName.c_str(), StructPlayer::GetPlayerName( (*it)->nLeaderSID ), (*it)->nLeaderLevel, static_cast<int>( (*it)->vMemberNameList.size() ), (*it)->nDungeonId ) )
		{
			++cnt;
		}
	}

	return cnt;
}

size_t GuildManager::DoEachMember( int nGuildID, GuildFunctor & _fo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	return doEachMember( pInfo, _fo );
}

size_t GuildManager::DoEachMemberTag( int nGuildID, GuildMemberTagFunctor & _fo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	return doEachMemberTag( pInfo, _fo );
}

size_t GuildManager::doEachMember( GuildInfo * pInfo, GuildFunctor & _fo )
{
	size_t cnt = 0;	

	std::vector< AR_HANDLE >::iterator it;
	for( it = pInfo->vOnlineList.begin(); it != pInfo->vOnlineList.end(); ++it )
	{
		_fo( *it );
		++cnt;
	}

	return cnt;
}

size_t GuildManager::doEachMemberTag( GuildInfo * pInfo, GuildMemberTagFunctor & _fo )
{
	size_t cnt = 0;

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin() ; it != pInfo->vMemberNameList.end() ; ++it )
	{
		_fo( &(*it) );
		++cnt;
	}

	return cnt;
}

bool GuildManager::onLogin( int nGuildID, struct StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo * pGuildInfo = getGuildInfo( nGuildID );

	if( !pGuildInfo )
	{
		pPtr->SetGuildID( 0 );

		return false;
	}

	return signAsOnLine( getGuildInfo( nGuildID ), pPtr );
}

bool GuildManager::onLogout( int nGuildID, struct StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return signAsOffLine( getGuildInfo( nGuildID ), pPtr );
}

std::string GuildManager::GetGuildName( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return "";

	return pInfo->strGuildName;
}

bool GuildManager::SetGuildIconInfo( int nGuildID, const char * szIconFileName, int nIconSize )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	if( pInfo->strIconFileName != szIconFileName || pInfo->nIconSize != nIconSize )
	{
		pInfo->strIconFileName = szIconFileName;
		pInfo->nIconSize = nIconSize;

		Push( new DB_SetGuildIcon( nGuildID, szIconFileName, nIconSize ) );

		char szBuf[255];
		s_sprintf( szBuf, _countof( szBuf ), "GICON_UPDATE|%d", nGuildID );
		SendGlobalChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", szBuf, static_cast<unsigned int>( strlen( szBuf ) ) );

		return true;
	}

	return false;
}

const std::string GuildManager::GetGuildIconFileName( int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return "";

	return pInfo->strIconFileName;
}

const int GuildManager::GetGuildIconSize( int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	return pInfo->nIconSize;
}

bool GuildManager::SetGuildBannerInfo( int nGuildID, const char * szBannerFileName, int nBannerSize )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	if( pInfo->strBannerFileName != szBannerFileName || pInfo->nBannerSize != nBannerSize )
	{
		pInfo->strBannerFileName = szBannerFileName;
		pInfo->nBannerSize = nBannerSize;

		Push( new DB_SetGuildBanner( nGuildID, szBannerFileName, nBannerSize ) );

		char szBuf[255];
		s_sprintf( szBuf, _countof( szBuf ), "GBANNER_UPDATE|%d", nGuildID );
		SendGlobalChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", szBuf, static_cast<unsigned int>( strlen( szBuf ) ) );

		return true;
	}

	return false;
}

const std::string GuildManager::GetGuildBannerFileName( int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return "";

	return pInfo->strBannerFileName;
}

const int GuildManager::GetGuildBannerSize( int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	return pInfo->nBannerSize;
}

const GuildManager::_ADVERTISE_TYPE GuildManager::GetGuildAdvertiseType( const int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return ADV_TYPE_NONE;

	return pInfo->eAdvertiseType;
}

const time_t GuildManager::GetGuildAdvertiseEndTime( const int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return 0;

	return ( pInfo->eAdvertiseType == ADV_TYPE_NONE || pInfo->eAdvertiseType == ADV_TYPE_LIST_ONLY ) ? 0 : pInfo->tAdvertiseEnd;
}

const bool GuildManager::SetGuildAdvertise( const int nGuildID, const _ADVERTISE_TYPE eType, const time_t tEnd )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	pInfo->eAdvertiseType = eType;
	pInfo->tAdvertiseEnd = ( eType == ADV_TYPE_NONE || eType == ADV_TYPE_LIST_ONLY ) ? 0 : tEnd;

	Push( new DB_SetGuildAdvertiseTypeAndTime( nGuildID, eType, tEnd ) );
	return true;
}

const std::string GuildManager::GetGuildAdvertiseComment( const int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return "";

	return pInfo->strAdvertiseComment;
}

const bool GuildManager::SetGuildAdvertiseComment( const int nGuildID, const char * pszComment )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	pInfo->strAdvertiseComment = pszComment;

	Push( new DB_SetGuildAdvertiseComment( nGuildID, pszComment ) );
	return true;
}

const bool GuildManager::IsRecruitingGuild( const int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	return pInfo->bRecruiting;
}

const bool GuildManager::SetGuildRecruiting( const int nGuildID, const bool bRecruiting, const short nMinRecruitLevel, const short nMaxRecruitLevel, const bool bSkipDBUpdate )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	// 이미 변경하고자 하는 상태와 같다면 패스
	if( pInfo->bRecruiting == bRecruiting )
		return true;

	pInfo->bRecruiting = bRecruiting;
	pInfo->nMinRecruitLevel = nMinRecruitLevel;
	pInfo->nMaxRecruitLevel = nMaxRecruitLevel;

	// DB 업데이트 실패 시 메모리상의 정보만 원상복귀시키기 위해서 DB 갱신을 Skip 할 수 있음
	if( !bSkipDBUpdate )
		Push( new DB_SetGuildRecruiting( nGuildID, bRecruiting, nMinRecruitLevel, nMaxRecruitLevel ) );

	return true;
}

const bool GuildManager::IsLeader( int nGuildID, const PlayerUID nPlayerUID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	return pInfo->nLeaderSID == nPlayerUID;
}

const bool GuildManager::IsMember( int nGuildID, const char *szPlayerName ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const GuildInfo * pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	std::vector< GuildMemberTag >::const_iterator it;

	for( it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( !_stricmp( StructPlayer::GetPlayerName( it->sid ), szPlayerName ) ) return true;
	}

	return false;
}

bool GuildManager::GetMemberInfo( int nGuildID, const char* szPlayerName, GuildManager::GuildMemberTag * pMemberInfo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	std::vector< GuildMemberTag >::iterator it;

	for( it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( !_stricmp( StructPlayer::GetPlayerName( it->sid ), szPlayerName ) )
		{
			*pMemberInfo = (*it);
			return true;
		}
	}

	return false;
}

void GuildManager::GetOfflineMember( int nGuildID, std::vector< GuildMemberTag > & vList )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return;

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( !(*it).bIsOnline ) vList.push_back( *it );
	}
}

const GuildManager::GuildInfo * GuildManager::getGuildInfo( const int nGuildID ) const
{
	GuildInfo * p = NULL;

	m_hshGuildID.lookup( nGuildID, p );

	return p;
}

GuildManager::GuildInfo * GuildManager::getGuildInfo( const int nGuildID )
{
	GuildInfo * p = NULL;

	m_hshGuildID.lookup( nGuildID, p );

	return p;
}

const GuildManager::GuildInfo * GuildManager::getGuildInfo( const char *szGuildName ) const
{
	GuildInfo * p = NULL;

	m_hshGuildName.lookup( szGuildName, p );

	return p;
}

GuildManager::GuildInfo * GuildManager::getGuildInfo( const char *szGuildName )
{
	GuildInfo * p = NULL;

	m_hshGuildName.lookup( szGuildName, p );

	return p;
}

bool GuildManager::signAsOnLine( GuildInfo *pInfo, struct StructPlayer *pPtr )
{
	if( !pInfo ) return false;

	pInfo->vOnlineList.push_back( pPtr->GetHandle() );

	if( pInfo->nLeaderSID == pPtr->GetPlayerUID() )
	{
		pInfo->nLeaderLevel = pPtr->GetLevel();
	}

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( (*it).sid == pPtr->GetPlayerUID() )
		{
			(*it).bIsOnline = true;
			break;
		}
	}

	return true;
}

bool GuildManager::signAsOffLine( GuildInfo *pInfo, struct StructPlayer *pPtr )
{
	if( !pInfo ) return false;

	for( std::vector< AR_HANDLE >::iterator it = pInfo->vOnlineList.begin(); it != pInfo->vOnlineList.end(); ++it )
	{
		if( *it == pPtr->GetHandle() )
		{
			pInfo->vOnlineList.erase( it );
			break;
		}
	}

	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( (*it).sid == pPtr->GetPlayerUID() )
		{
			(*it).bIsOnline = false;
			break;
		}
	}

	return false;	
}

void GuildManager::Push( GameDBManager::DBProc* pWork )
{
	THREAD_SYNCRONIZE( m_QueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}


void GuildManager::onEndQuery()
{
	THREAD_SYNCRONIZE( m_QueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}

int GuildManager::allocGuildId()
{
	return InterlockedIncrement( &m_nMaxGuildId );
}

int GuildManager::allocAllianceId()
{
	return InterlockedIncrement( &m_nMaxAllianceId );
}

void GuildManager::ForceChangeGuildName( int nGuildID, const char * szGuildName )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( szGuildName );
	if( pInfo )
	{
		return;
	};

	pInfo = getGuildInfo( nGuildID );
	if( !pInfo )
	{
		return;
	}

	Push( new DB_ChangeGuildName( szGuildName, nGuildID, NULL ) );
}

void GuildManager::ChangeGuildName( int nGuildID, const char * szGuildName, struct StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( szGuildName );
	if( pInfo )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPtr, "@18" );

		return;
	};

	pInfo = getGuildInfo( nGuildID );
	if( !pInfo )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPtr, "@130" );

		return;
	}

	if( pInfo->nNameChanged )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPtr, "@130" );

		return;
	}

	pInfo->nNameChanged = 1;

	Push( new DB_ChangeGuildName( szGuildName, nGuildID, pPtr ) );
}

void GuildManager::onChangeGuildName( int nGuildID, const char * szGuildName, struct StructPlayer *pPtr )
{
	{
		THREAD_SYNCRONIZE( m_IntfLock );

		GuildInfo *pInfo = getGuildInfo( nGuildID );
		if( !pInfo )
		{
			return;
		}

		// 이전 길드 이름 삭제
		m_hshGuildName.erase( pInfo->strGuildName.c_str() );

		pInfo->strGuildName = szGuildName;

		// 새 길드 이름 추가
		if( m_hshGuildName.add( pInfo->strGuildName.c_str(), pInfo ) == NULL )
		{
			int nHas = (m_hshGuildName.has( pInfo->strGuildName.c_str() ) ? 1 : 0);
			FILELOG( "(ChangeName) KHash add failed GuildID: %d, GuildName: %s(has: %d)", pInfo->nGuildId, pInfo->strGuildName.c_str(), nHas );
			_cprint( "(ChangeName) KHash add failed GuildID: %d, GuildName: %s(has: %d)\n", pInfo->nGuildId, pInfo->strGuildName.c_str(), nHas );
		}
	}

	char buf[256];

	s_sprintf( buf, _countof( buf ), "CHANGE_GUILD_NAME|%d|%s|", nGuildID, szGuildName );

	SendGlobalChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", buf, static_cast<unsigned int>( strlen(buf) ) );

	if( pPtr )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPtr, "@131" );

		LOG::Log11N4S( LM_GUILD_CHANGE_NAME, pPtr->GetAccountID(), pPtr->GetSID(), 0, nGuildID, 0, 0, 0, 0, 0, 0, 0, pPtr->GetAccountName(), LOG::STR_NTS, pPtr->GetName(), LOG::STR_NTS, szGuildName, LOG::STR_NTS, "", 0 );
	}
	else
	{
		LOG::Log11N4S( LM_GUILD_CHANGE_NAME, 0, 0, 0, nGuildID, 0, 0, 0, 0, 0, 0, 0, "GM", LOG::STR_NTS, "GM", LOG::STR_NTS, szGuildName, LOG::STR_NTS, "", 0 );
	}
}

void GuildManager::onChangeGuildNameFailed( const int nGuildID )
{
	THREAD_SYNCHRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );

	if( !pInfo ) return;

	pInfo->nNameChanged = 0;
}

bool GuildManager::destroyGuild( GuildInfo *pInfo )
{
	if( !pInfo ) return false;

	if( isDestroyableGuild( pInfo ) != RESULT_SUCCESS )
	{
		// 아래의 처리로 추가 처리는 가능하지만 이 함수까지 들어오기 전에 처리할 것들은 미리 처리했다고 가정
		return false;

		// 던전 소유 중인 길드는 해산 불가
		//if( pInfo->nDungeonId )
		//{
		//	if( DungeonManager::Instance().GetOwnGuildID( pInfo->nDungeonId ) == pInfo->nGuildId )
		//		return false;

		//	// 레이드 기간 중 레이드 도전 길드일 경우
		//	if( DungeonManager::Instance().IsDungeonRaidTime( pInfo->nDungeonId ) )
		//	{
		//		DungeonManager::Instance().ClearRaidRecord( pInfo->nDungeonId, pInfo->nGuildId );
		//		pInfo->nDungeonId = 0;
		//		// 아래에서 길드 해산되므로 dungeon_id 바뀌는 DB 업데이트 제낌
		//	}

		//	// 레이드 종료 후 최고 기록 길드여서 공격자 길드로 선정된 길드일 경우 레이드 기록 제거
		//	if( DungeonManager::Instance().GetRaidGuildID( pInfo->nDungeonId ) == pInfo->nGuildId )
		//	{
		//		DungeonManager::Instance().ClearRaidGuildID( pInfo->nDungeonId );
		//	}
		//}
	}

	struct myGuildFunctor : GuildFunctor
	{
		myGuildFunctor()
			: tGuildBlockTime( time( NULL ) + GameRule::GUILD_REJOIN_BLOCK_DURATION_SEC )
		{}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( !pPlayer ) return true;
			pPlayer->SetGuildBlockTime( tGuildBlockTime );
			pPlayer->SetPrevGuildID( pPlayer->GetGuildID() );
			pPlayer->SetGuildID( 0 );

			return true;
		}

		time_t tGuildBlockTime;
	} _fo;

	// 온라인 플레이어들 길드 정보 클리어 및 길드 재가입 제한 시간 룰 적용
	doEachMember( pInfo, _fo );

	// 모든 길드원 관련 DB에 길드 재가입 제한 시간 룰 적용
	time_t tGuildBlockTime = time(NULL) + GameRule::GUILD_REJOIN_BLOCK_DURATION_SEC;
	for( std::vector< GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin() ; it != pInfo->vMemberNameList.end() ; ++it )
	{
		Push( new DB_SetGuildBlockTime( it->sid, tGuildBlockTime ) );
		Push( new DB_SetGuild( it->sid, 0, pInfo->nGuildId ) );
	}

	// DB 에서 길드 제거
	Push( new DB_DeleteGuild( pInfo->nGuildId ) );
	
	m_hshGuildName.erase( pInfo->strGuildName.c_str() );
	m_hshGuildID.erase( pInfo->nGuildId );

	for( std::vector< GuildInfo * >::iterator it = m_vGuildList.begin(); it != m_vGuildList.end(); ++it )
	{
		if( (*it) == pInfo )
		{
			vector_fast_erase( &m_vGuildList, it );
			break;
		}
	}

	// 길드 정보 지움
	delete pInfo;

	return true;
}

bool GuildManager::IsExistGuild( const char *szGuildName )
{
	return getGuildInfo( szGuildName );
}

bool GuildManager::AddGuildDonationPoint( const int nGuildID, const int nPoint )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	pInfo->nDonationPoint += nPoint;

	Push( new DB_UpdateGuildDonationPoint( nGuildID, pInfo->nDonationPoint ) );

	return true;
}

bool GuildManager::updateGuildInfo( int nGuildID, const char * szIconFileName, int nIconSize )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	if( pInfo->strIconFileName != szIconFileName || pInfo->nIconSize != nIconSize )
	{
		pInfo->strIconFileName = szIconFileName;
		pInfo->nIconSize = nIconSize;

		return true;
	}

	return false;
}

bool GuildManager::updateGuildBannerInfo( int nGuildID, const char * szBannerFileName, int nBannerSize )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	if( pInfo->strBannerFileName != szBannerFileName || pInfo->nBannerSize != nBannerSize )
	{
		pInfo->strBannerFileName = szBannerFileName;
		pInfo->nBannerSize = nBannerSize;

		return true;
	}

	return false;
}

bool GuildManager::IsExistAlliance( const char * szName )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return !!getAllianceInfo( szName );
}
	
bool GuildManager::CreateAlliance( int nGuildID, const char * szName )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	if( pInfo->pAllianceInfo ) return false;

	if( getAllianceInfo( szName ) ) return false;

	AllianceInfo * pAllianceInfo = makeAlliance( allocAllianceId(), pInfo->nGuildId, szName, DEFAULT_MAX_ALLIANCE_CNT, 1 );

	if( !pAllianceInfo ) return false;

	pInfo->pAllianceInfo = pAllianceInfo;

	pAllianceInfo->vGuildList.push_back( pInfo );

	Push( new DB_InsertAlliance( pAllianceInfo->nAllianceID, pAllianceInfo->nLeadGuildID, pAllianceInfo->strAllianceName.c_str(), pAllianceInfo->nMaxAllianceCnt ) );
	Push( new DB_SetGuildAllianceID( nGuildID, pAllianceInfo->nAllianceID ) );

	return true;
}

void GuildManager::ChangeAllianceName( int nAllianceID, const char * szNewName, StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

	if( !pInfo ) return;

	if( !pInfo->strAllianceName.compare( szNewName ) )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPtr, "@118" );
		return;
	}

	if( pInfo->nNameChanged )
	{
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPtr, "@130" );
		return;
	}

	pInfo->nNameChanged = 1;

	Push( new DB_ChangeAllianceName( szNewName, nAllianceID, pPtr ) );
}

void GuildManager::onChangeAllianceName( int nAllianceID, const char * szNewName, StructPlayer *pPtr )
{
	{
		THREAD_SYNCRONIZE( m_IntfLock );

		AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

		if( !pInfo ) return;

		pInfo->strAllianceName = szNewName;
	}

	SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pPtr, "@131" );
}

void GuildManager::onChangeAllianceNameFailed( const int nAllianceID )
{
	THREAD_SYNCHRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

	if( !pInfo ) return;

	pInfo->nNameChanged = 0;
}

bool GuildManager::AddToAlliance( int nAllianceID, int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo ) return false;

	if( pInfo->pAllianceInfo ) return false;

	AllianceInfo * pAllianceInfo = getAllianceInfo( nAllianceID );

	if( pAllianceInfo->nMaxAllianceCnt <= static_cast< int >( pAllianceInfo->vGuildList.size() ) ) return false;

	pInfo->pAllianceInfo = pAllianceInfo;
	pAllianceInfo->vGuildList.push_back( pInfo );

	Push( new DB_SetGuildAllianceID( nGuildID, nAllianceID ) );

	// 연합 가입한 길드가 연합 마스터 길드보다 더 나중까지 레이드 신청 기간 제한에 걸려 있을 경우 해당 설정으로 변경
	if( pInfo->tDungeonBlockTime )
	{
		for( std::vector< GuildManager::GuildInfo * >::iterator it = pAllianceInfo->vGuildList.begin() ; it != pAllianceInfo->vGuildList.end() ; ++it )
		{
			if( (*it)->nGuildId == pAllianceInfo->nLeadGuildID )
			{
				time_t tDungeonBlockTime = pInfo->tDungeonBlockTime;
				if( (*it)->tDungeonBlockTime && (*it)->tDungeonBlockTime > tDungeonBlockTime )
				{
					tDungeonBlockTime = (*it)->tDungeonBlockTime;
				}

				break;
			}
		}
	}

	return true;
}

bool GuildManager::LeaveAlliance( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo || !pInfo->pAllianceInfo ) return false;

	if( pInfo->pAllianceInfo->nLeadGuildID == pInfo->nGuildId ) return false;

	time_t alliance_block_time = time(NULL) + GameRule::ALLIANCE_REJOIN_BLOCK_DURATION_SEC;

	for( std::vector< GuildInfo * >::iterator it = pInfo->pAllianceInfo->vGuildList.begin(); it != pInfo->pAllianceInfo->vGuildList.end(); ++it )
	{
		if( (*it) == pInfo )
		{
			pInfo->pAllianceInfo->vGuildList.erase( it );
			pInfo->tAllianceBlockTime = alliance_block_time;
			break;
		}
	}

	Push( new DB_SetGuildAllianceID( nGuildID, 0 ) );
	Push( new DB_SetGuildAllianceBlockTime( nGuildID, alliance_block_time ) );

	pInfo->pAllianceInfo = NULL;

	return true;
}

int GuildManager::GetAlliancePassword( int nAllianceID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

	if( !pInfo ) return 0;

	return pInfo->nAlliancePassword;
}

int GuildManager::GetAllianceLeaderGuildID( int nAllianceID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );
	if( !pInfo ) return 0;

	return pInfo->nLeadGuildID;
}

int GuildManager::GetAllianceID( int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( nGuildID );
	if( !pInfo || !pInfo->pAllianceInfo ) return 0;

	return pInfo->pAllianceInfo->nAllianceID;
}

bool GuildManager::IsAllianceLeader( int nAllianceID, int nGuildID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );
	if( !pInfo ) return false;

	return pInfo->nLeadGuildID == nGuildID;
}

std::vector< int > GuildManager::GetAllianceMemberID( int nAllianceID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	std::vector< int > vAllianceMemberID;

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

	if( !pInfo ) return vAllianceMemberID;

	for( std::vector< GuildInfo * >::iterator it = pInfo->vGuildList.begin(); it != pInfo->vGuildList.end(); ++it )
	{
		vAllianceMemberID.push_back( (*it)->nGuildId );
	}

	return vAllianceMemberID;
	
}

std::string GuildManager::GetAllianceName( int nAllianceID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

	if( !pInfo ) return "";

	return pInfo->strAllianceName;
}

int GuildManager::GetMaxAllianceCount( int nAllianceID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

	if( !pInfo ) return 0;

	return pInfo->nMaxAllianceCnt;
}

bool GuildManager::IncMaxAllianceCount( int nAllianceID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

	if( !pInfo ) return false;

	++(pInfo->nMaxAllianceCnt);

	Push( new DB_UpdateAllianceInfo( pInfo->nAllianceID, pInfo->nMaxAllianceCnt ) );

	return true;
}

int GuildManager::GetAllianceMemberGuildCount( int nAllianceID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

	if( !pInfo ) return 0;

	return static_cast< int >( pInfo->vGuildList.size() );
}

int GuildManager::GetAllianceMemberPlayerCount( int nAllianceID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pInfo = getAllianceInfo( nAllianceID );

	if( !pInfo ) return 0;

	int nCount = 0;

	for( std::vector< GuildManager::GuildInfo * >::iterator it = pInfo->vGuildList.begin() ; it != pInfo->vGuildList.end() ; ++it )
	{
		nCount += static_cast< int >( (*it)->vMemberNameList.size() );
	}

	return nCount;
}

bool GuildManager::DestroyAlliance( int nAllianceID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AllianceInfo *pAllianceInfo = getAllianceInfo( nAllianceID );

	if( pAllianceInfo->vGuildList.size() > 1 ) return false;

	GuildInfo *pLeadGuildInfo = getGuildInfo( pAllianceInfo->nLeadGuildID );

	if( !pLeadGuildInfo ) return false;

	pLeadGuildInfo->pAllianceInfo = NULL;

	time_t tAllianceBlockTime = time( NULL ) + GameRule::ALLIANCE_REJOIN_BLOCK_DURATION_SEC;

	for( std::vector< AllianceInfo * >::iterator it = m_vAllianceList.begin(); it != m_vAllianceList.end(); ++it )
	{
		if( (*it) == pAllianceInfo )
		{
			// 연합 소속 길드들에게 연합 재가입 대기 시간 세팅
			for( std::vector< GuildInfo * >::iterator git = (*it)->vGuildList.begin() ; git != (*it)->vGuildList.end() ; ++git )
			{
				(*git)->tAllianceBlockTime = tAllianceBlockTime;
				Push( new DB_SetGuildAllianceBlockTime( (*git)->nGuildId, tAllianceBlockTime ) );
			}

			m_vAllianceList.erase( it );
			break;
		}
	}

	Push( new DB_DeleteAllianceInfo( pAllianceInfo->nAllianceID ) );
	Push( new DB_SetGuildAllianceID( pLeadGuildInfo->nGuildId, 0 ) );

	delete pAllianceInfo;

	return true;
}

bool GuildManager::Buff(int nGuildID, int points, int* pnGrade, int* pnPoint, StructPlayer* pPtr, int* pnGuildPoint, int* pnGuildTotalPoint)
{
	THREAD_SYNCRONIZE(m_IntfLock);

	GuildInfo* pInfo = getGuildInfo(nGuildID);

	if (!pInfo)
		return false;

	if (points != 1 && points != 10)
		return false;

	if (pPtr->GetGuildPoint() < points)
	{
		::SendChatMessage(false, CHAT_NOTICE, "@NOTICE", pPtr->GetHandle(), "@799");
		return false;
	}

	pPtr->SetGuildPoint(pPtr->GetGuildPoint() - points);

	int level = 0;

	if (pPtr->GetGuildPermission() == PERMISSION_LEADER) {
		level = pInfo->nGrade * 15;
	}
	else {
		level = pInfo->nGrade * 10;
	}
	if (points == 10) {
		level += 50;
	}
	
	pPtr->PendAddState((StructState::StateCode)6523, pPtr->GetHandle(), level, GetArTime(), GetArTime() + (GameRule::nGuildBuffMinute * 6000));

	*pnGrade = pInfo->nGrade;
	*pnPoint = pInfo->nPoint;
	*pnGuildPoint = pPtr->GetGuildPoint();
	*pnGuildTotalPoint = pPtr->GetTotalGuildPoint();

	Push(new DB_UpdateGuildMemberPoint(pPtr->GetHandle(), pPtr->GetGuildPoint(), pPtr->GetTotalGuildPoint()));

	return true;
}

bool GuildManager::Donate(int nGuildID, int points, int* pnGrade, int* pnPoint, StructPlayer* pPtr, int* pnGuildPoint, int* pnGuildTotalPoint)
{
	THREAD_SYNCRONIZE(m_IntfLock);

	GuildInfo* pInfo = getGuildInfo(nGuildID);

	if (!pInfo) 
		return false;

	StructGold nGold = pPtr->GetGold();
	int nDonateGold = GameRule::nGuildDonateGold;
	
	if (points == 100)
		nDonateGold *= 10;
	else if (points == 1000)
		nDonateGold *= 100;
	else if (points != 10)
		return false;

	if (nGold < nDonateGold)
	{
		::SendChatMessage(false, CHAT_NOTICE, "@NOTICE", pPtr->GetHandle(), "@765");
		return false;
	}

	pInfo->nPoint += points;
	pPtr->SetGuildPoint(pPtr->GetGuildPoint() + points);
	pPtr->SetTotalGuildPoint(pPtr->GetTotalGuildPoint() + points);
	
	nGold -= nDonateGold;

	if ((pPtr->GetGold() != nGold) && (nGold <= GameRule::MAX_GOLD_FOR_INVENTORY) && (nGold > 0))
	{
		pPtr->ChangeGold(nGold);
		::SendGoldChaosUpdateMsg(pPtr);
		pPtr->UpdateTitleConditionByGold();

	}

	*pnGrade = pInfo->nGrade;
	*pnPoint = pInfo->nPoint;
	*pnGuildPoint = pPtr->GetGuildPoint();
	*pnGuildTotalPoint = pPtr->GetTotalGuildPoint();

	Push(new DB_UpdateGuildGradePoint(nGuildID, pInfo->nGrade, pInfo->nPoint));
	Push(new DB_UpdateGuildMemberPoint(pPtr->GetPlayerUID(), pPtr->GetGuildPoint(), pPtr->GetTotalGuildPoint()));


	std::vector< GuildMemberTag >::iterator it;
	for (it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it)
	{
		if (pPtr->GetSID() == it->sid)
		{
#ifdef USE_CLIENT97
			it->nPoint = pPtr->GetGuildPoint();
			it->nTotalPoint = pPtr->GetTotalGuildPoint();
#endif
			break;
		}
	}

	return true;
}

bool GuildManager::Upgrade(int nGuildID, int* pnGrade, int* pnPoint, StructPlayer* pPtr, int* pnGuildPoint, int* pnGuildTotalPoint)
{
	static int costs[10] = { 0, 5000, 10000, 20000, 40000, 80000, 160000, 320000, 640000, 1280000 };

	THREAD_SYNCRONIZE(m_IntfLock);

	GuildInfo* pInfo = getGuildInfo(nGuildID);

	if (!pInfo)
		return false;


	if (pInfo->nGrade >= 10)
		return false;

	int cost = costs[pInfo->nGrade];

	if (pInfo->nPoint < cost)
		return false;

	pInfo->nPoint -= cost;
	pInfo->nGrade++;

	*pnGrade = pInfo->nGrade;
	*pnPoint = pInfo->nPoint;
	*pnGuildPoint = pPtr->GetGuildPoint();
	*pnGuildTotalPoint = pPtr->GetTotalGuildPoint();

	Push(new DB_UpdateGuildGradePoint(nGuildID, pInfo->nGrade, pInfo->nPoint));

	return true;
}

GuildManager::AllianceInfo * GuildManager::makeAlliance( int nAllianceId, int nLeadGuildId, const char * szName, int nMaxAllianceCnt, int nNameChaned )
{
	AllianceInfo *pInfo = new AllianceInfo;
	pInfo->nAllianceID = nAllianceId;
	pInfo->nAlliancePassword = XRandom();
	pInfo->nLeadGuildID = nLeadGuildId;
	pInfo->strAllianceName = szName;
	pInfo->nMaxAllianceCnt = nMaxAllianceCnt;
	pInfo->nNameChanged = nNameChaned;

	// 해쉬에 등록
	m_vAllianceList.push_back( pInfo );

	return pInfo;
}

GuildManager::AllianceInfo * GuildManager::getAllianceInfo( int nAllianceId )
{
	for( std::vector< AllianceInfo * >::iterator it = m_vAllianceList.begin(); it != m_vAllianceList.end(); ++it )
	{
		if( (*it)->nAllianceID == nAllianceId )
		{
			return (*it);
		}
	}

	return NULL;
}

GuildManager::AllianceInfo * GuildManager::getAllianceInfo( const char * szName )
{
	for( std::vector< AllianceInfo * >::iterator it = m_vAllianceList.begin(); it != m_vAllianceList.end(); ++it )
	{
		if( !_stricmp( (*it)->strAllianceName.c_str(), szName ) )
		{
			return (*it);
		}
	}

	return NULL;
}
