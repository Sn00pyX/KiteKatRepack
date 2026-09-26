
#include <toolkit/XEnv.h>
#include <toolkit/XRandom.h>
#include <toolkit/XConsole.h>

#include "PartyManager.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "DB_Commands.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "GameDBUtil.h"
#include "DB_Commands.h"
#include "HuntaholicManager.h"
#include "GuildManager.h"
#include "PartyMatchingManager.h"

static PartyManager _inst;


PartyManager & PartyManager::GetInstance()
{
	return _inst;
}

bool PartyManager::Init()
{
	InitUserDbConnection( m_DBConn.connection );
	m_DBConn.CreateCommand( m_DBConn.command );

	SCOPED_ENV( "stat.loading_party_info" );

	m_nMaxPartyID = 0;

	// DB 파티 리스트 주욱 읽어옴(m_IntfLock을 안 걸었는데, 로딩 과정 중에는 다른 쓰레드에서 PartyManager에 접근할 일이 없다고 간주하고 처리)
	loadPartyMemberTagList();		// Character 테이블에서 먼저 파티 소속 아바타 정보를 읽어서 PartyMemberTag 벡터 생성
	loadPartyList();				// 파티 정보 로딩 및 생성된 PartyMemberTag 벡터에서 해당 파티에 맞는 벡터 복사 및 소거
	finishLoading();				// 쓰고 남은 떨거지 청소(떨거지가 있다면 해당 파티 ID인 캐릭터는 있으나 Party 는 없는 것)

	return true;
}

bool PartyManager::DeInit()
{
	SCOPED_ENV( "stat.saving_party_info" );

	// 쿼리 다 완결될때까지 대기
	while( !m_lQueryList.empty() )
	{
		Sleep( 100 );
	}

	return true;
}

const PartyManager::_PARTY_TYPE PartyManager::GetPartyType( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo )
	{
		assert( 0 );
		return TYPE_UNKNOWN;
	}

	return pInfo->ePartyType;
}

bool PartyManager::IsLinkedParty( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	return ( pInfo->pPartyLinkInfo != NULL );
}

bool PartyManager::IsAttackTeamParty( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	return _isAttackTeamParty( pInfo );
}

bool PartyManager::IsBattleArenaTeamParty( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	return _isBattleArenaTeamParty( pInfo );
}

bool PartyManager::IsJoinableLinkedParty( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	if( !pInfo->pPartyLinkInfo ) return false;

	return static_cast< int >( pInfo->pPartyLinkInfo->vPartyList.size() ) < pInfo->pPartyLinkInfo->nMaxPartyCount;
}

int PartyManager::GetLinkedPartyLeadPartyID( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return 0;

	if( pInfo->pPartyLinkInfo )
		return pInfo->pPartyLinkInfo->nLeadPartyID;

	return 0;
}

bool PartyManager::IsExistAttackTeam( int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	if( m_hshAttackTeamID.has( nGuildID ) )
		return true;

	return false;
}

int PartyManager::GetAttackTeamLeadPartyIDByGuildID( int nGuildID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AttackTeamInfo * pAttackTeamInfo = NULL;

	m_hshAttackTeamID.lookup( nGuildID, pAttackTeamInfo );

	if( !pAttackTeamInfo )
		return 0;

	return pAttackTeamInfo->nLeadPartyID;
}

bool PartyManager::MakeAttackTeam( int nPartyID, int nGuildID, int nMaxGuildParty, bool bIsRaidParty )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	if( pInfo->pPartyLinkInfo )
		return false;

	if( bIsRaidParty )
	{
		if( pInfo->ePartyType != TYPE_RAID_ATTACKTEAM )
		{
			assert( 0 );
			pInfo->ePartyType = TYPE_RAID_ATTACKTEAM;
		}
	}
	else if( pInfo->ePartyType != TYPE_SIEGE_ATTACKTEAM )
	{
		assert( 0 );
		pInfo->ePartyType = TYPE_SIEGE_ATTACKTEAM;
	}

	if( makeAttackTeam( pInfo, nGuildID, nMaxGuildParty ) )
	{
		Push( new DB_UpdatePartyInfo( pInfo->nPartyID, pInfo->nLeaderSID, pInfo->eShareMode, pInfo->ePartyType, pInfo->nPartyID ) );

		return true;
	}

	return false;
}

bool PartyManager::JoinAttackTeam( int nGuildID, int nPartyID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	if( pInfo->pPartyLinkInfo ) return false;

	AttackTeamInfo * pAttackTeamInfo = NULL;
	
	m_hshAttackTeamID.lookup( nGuildID, pAttackTeamInfo );

	if( !pAttackTeamInfo )
		return false;

	if( joinLinkedParty( pAttackTeamInfo, pInfo ) )
	{
		Push( new DB_UpdatePartyInfo( pInfo->nPartyID, pInfo->nLeaderSID, pInfo->eShareMode, pInfo->ePartyType, pInfo->pPartyLinkInfo->nLeadPartyID ) );

		return true;
	}

	return false;
}

int PartyManager::GetAttackTeamGuildID( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return 0;

	if( !_isAttackTeamParty( pInfo ) ) return 0;

	return static_cast< AttackTeamInfo * >( pInfo->pPartyLinkInfo )->nGuildID;
}

bool PartyManager::MakeBattleArenaTeam( int nLeadPartyID, int nMaxPartyCount )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo *pInfo = getPartyInfo( nLeadPartyID );
	if( !pInfo ) return false;

	if( pInfo->pPartyLinkInfo )
		return false;

	if( pInfo->ePartyType != TYPE_BATTLE_ARENA_TEAM &&
		pInfo->ePartyType != TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		return false;

	// 배틀 아레나 파티 관련 내용이므로 DB 업데이트를 따로 하지 않음
	return makeLinkedParty( pInfo, nMaxPartyCount );
}

bool PartyManager::JoinBattleArenaTeam( int nLeadPartyID, int nPartyID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo * pLeadPartyInfo = getPartyInfo( nLeadPartyID );
	if( !pLeadPartyInfo ) return false;

	PartyLinkInfo * pPartyLinkInfo = pLeadPartyInfo->pPartyLinkInfo;
	if( !pPartyLinkInfo )
		return false;

	if( pLeadPartyInfo->ePartyType != TYPE_BATTLE_ARENA_TEAM &&
		pLeadPartyInfo->ePartyType != TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		return false;

	PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	// 배틀 아레나 파티 관련 내용이므로 DB 업데이트를 따로 하지 않음
	return joinLinkedParty( pPartyLinkInfo, pInfo );
}

bool PartyManager::ChangeBattleArenaExerciseTeamType( int nLeadPartyID, bool bToExerciseTeam )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo * pLeadPartyInfo = getPartyInfo( nLeadPartyID );
	if( !pLeadPartyInfo ) return false;

	if( !_isBattleArenaTeamParty( pLeadPartyInfo ) )
		return false;

	PartyLinkInfo * pPartyLinkInfo = pLeadPartyInfo->pPartyLinkInfo;
	if( !pPartyLinkInfo )
		return false;

	_PARTY_TYPE eSourceType = ( bToExerciseTeam ) ? TYPE_BATTLE_ARENA_TEAM : TYPE_BATTLE_ARENA_EXERCISE_TEAM;
	_PARTY_TYPE eTargetType = ( bToExerciseTeam ) ? TYPE_BATTLE_ARENA_EXERCISE_TEAM : TYPE_BATTLE_ARENA_TEAM;

	for( std::vector< PartyInfo * >::iterator it = pPartyLinkInfo->vPartyList.begin() ; it != pPartyLinkInfo->vPartyList.end() ; ++it )
	{
		PartyInfo * pInfo = (*it);

		// 타입이 다른 파티 정보가 나왔으면 지금까지 바꾼 파티들 다 되돌려주고 실패
		if( pInfo->ePartyType != eSourceType )
		{
			assert( 0 );

			for( std::vector< PartyInfo * >::iterator itPrev = pPartyLinkInfo->vPartyList.begin() ; itPrev != it ; ++itPrev )
				(*itPrev)->ePartyType = eSourceType;

			return false;
		}

		pInfo->ePartyType = eTargetType;
	}

	return true;
}

int PartyManager::GetPartyPassword( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return -1;

	return pInfo->nPartyPassword;
}

int PartyManager::GetLinkedPartyPassword( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return -1;

	if( !pInfo->pPartyLinkInfo ) return -1;

	return pInfo->pPartyLinkInfo->nPassword;
}

bool PartyManager::IsSomeoneInSiegeOrRaidDungeon( int nPartyID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo * pInfo = getPartyInfo( nPartyID );

	if( !pInfo || !_isAttackTeamParty( pInfo ) ) return false;

	// 던전 내부에 한 명이라도 있으면 공대파티 파괴 불가능
	struct myPartyCheckerFunctor : PartyManager::PartyFunctor
	{
		myPartyCheckerFunctor() : m_bSomeoneInDungeon(false) {}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;

			if( pPlayer && pPlayer->IsInSiegeOrRaidDungeon() )
				m_bSomeoneInDungeon = true;

			return true;
		}

		bool m_bSomeoneInDungeon;
	} _foChecker;

	for( std::vector< PartyInfo * >::iterator it = pInfo->pPartyLinkInfo->vPartyList.begin() ; it != pInfo->pPartyLinkInfo->vPartyList.end() && !_foChecker.m_bSomeoneInDungeon ; ++it )
	{
		doEachMember( (*it), _foChecker );
	}

	return _foChecker.m_bSomeoneInDungeon;
}

int PartyManager::MakeParty( const char *szPartyName, int nLeaderSID, const _PARTY_TYPE & ePartyType )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo *pInfo = getPartyInfo( szPartyName );
	if( pInfo ) return 0;

	int nPartyID = allocPartyID();
	if( nPartyID <= 0 )
	{
		return 0;
	}

	pInfo = makeParty( nPartyID, szPartyName, nLeaderSID, ITEM_SHARE_MONOPOLY, ePartyType );
	if( pInfo == NULL )
	{
		return 0;
	}

	// DB에 추가해야 하는 타입의 파티인 경우에만 DB에 추가
	if( isDBUpdateRequiredType( ePartyType ) )
		Push( new DB_InsertParty( pInfo->strPartyName.c_str(), pInfo->nPartyID, pInfo->nLeaderSID, pInfo->ePartyType ) );

	return pInfo->nPartyID;
}

bool PartyManager::DestroyParty( int nPartyID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return destroyParty( getPartyInfo( nPartyID ) );
}

int	PartyManager::GetMemberCount( int nPartyID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return 0;

	return static_cast< int >( pInfo->vMemberNameList.size() );
}

int PartyManager::GetPartyID( const char * szPartyName ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( szPartyName );
	if( !pInfo ) return 0;

	return pInfo->nPartyID;
}

std::string PartyManager::GetPartyName( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return "";

	return pInfo->strPartyName;
}

bool PartyManager::IsLeader( int nPartyID, PlayerUID nPlayerUID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	return pInfo->nLeaderSID == nPlayerUID;
}

bool PartyManager::IsMember( int nPartyID, PlayerUID nPlayerUID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	std::vector< PartyMemberTag >::const_iterator it;

	for( it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( (*it).sid == nPlayerUID )
			return true;
	}

	return false;
}

std::string	PartyManager::GetMemberDisplayName( int nPartyID, PlayerUID nPlayerUID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return "";

	std::vector< PartyMemberTag >::const_iterator it;

	for( it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( (*it).sid == nPlayerUID )
			return (*it).strDisplayName;
	}

	return "";
}

PlayerUID PartyManager::GetMemberUID( int nPartyID, const char * szPlayerName ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return 0;

	std::vector< PartyMemberTag >::const_iterator it;

	for( it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( (*it).strName == szPlayerName )
			return (*it).sid;
	}

	return 0;
}

bool PartyManager::Promote( int nPartyID, struct StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	// 시즈/레이드 공대 파티에서는 파티장 인계 불가능(길드 내 권한 시스템이 추가됨에 따라 수정되어야 했던 부분인듯 한데...;;)
	if( _isAttackTeamParty( pInfo ) )
		return false;

	PlayerUID nNewLeaderUID = pPtr->GetPlayerUID();

	// 현재 파장에게 인계하려는 시도면 패스
	if( nNewLeaderUID == pInfo->nLeaderSID )
	{
		assert( 0 );
		return false;
	}

	const PartyMemberTag * pMemberTag = NULL;
	for( std::vector< PartyMemberTag >::const_iterator it = pInfo->vMemberNameList.begin() ; it != pInfo->vMemberNameList.end() ; ++it )
	{
		const PartyMemberTag & memberTag = (*it);
		if( memberTag.sid == nNewLeaderUID )
		{
			pMemberTag = &memberTag;
			break;
		}
	}

	// 파티 멤버가 아닌 유저에게 인계 시도?
	if( !pMemberTag )
	{
		assert( 0 );
		return false;
	}

	if( isDBUpdateRequiredType( pInfo->ePartyType ) )
		Push( new DB_UpdatePartyInfo( nPartyID, pMemberTag->sid, pInfo->eShareMode, pInfo->ePartyType, 0 ) );

	pInfo->strLeaderName = pMemberTag->strName;
	pInfo->strLeaderDisplayName = pMemberTag->strDisplayName;
	pInfo->nLeaderSID = pMemberTag->sid;
	pInfo->nLeaderJobId = pMemberTag->nJobId;

	CPartyMatchingManager::GetInstance()->ChangeMaster(*StructPlayer::get(StructPlayer::FindPlayer(pInfo->strLeaderName.c_str())), pPtr);

	return true;
}

bool PartyManager::AutoPromote( int nPartyID, const bool bOnlyOnlinePlayer )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return false;

	// 시즈/레이드 공대 파티에서는 파티장 인계 불가능(길드 내 권한 시스템이 추가됨에 따라 수정되어야 했던 부분인듯 한데...;;)
	if( pInfo->pPartyLinkInfo && ( pInfo->ePartyType == TYPE_RAID_ATTACKTEAM || pInfo->ePartyType == TYPE_SIEGE_ATTACKTEAM ) )
		return false;

	PartyMemberTag * pNewLeaderTag = NULL;

	for( std::vector< PartyMemberTag >::iterator it = pInfo->vMemberNameList.begin() ; it != pInfo->vMemberNameList.end() ; ++it )
	{
		PartyMemberTag & memberTag = (*it);

		// 필요하다면 온라인 유저 체크
		if( bOnlyOnlinePlayer && !memberTag.bIsOnline )
			continue;

		// 현재 리더면 패스
		if( memberTag.sid == pInfo->nLeaderSID )
			continue;

		pNewLeaderTag = &memberTag;
	}

	if( !pNewLeaderTag )
		return false;

	if( isDBUpdateRequiredType( pInfo->ePartyType ) )
		Push( new DB_UpdatePartyInfo( nPartyID, pNewLeaderTag->sid, pInfo->eShareMode, pInfo->ePartyType, 0 ) );

	pInfo->strLeaderName = pNewLeaderTag->strName;
	pInfo->strLeaderDisplayName = pNewLeaderTag->strDisplayName;
	pInfo->nLeaderSID = pNewLeaderTag->sid;
	pInfo->nLeaderJobId = pNewLeaderTag->nJobId;

	CPartyMatchingManager::GetInstance()->ChangeMaster(*StructPlayer::get(StructPlayer::FindPlayer(pInfo->strLeaderName.c_str())), *StructPlayer::get(StructPlayer::FindPlayer(pNewLeaderTag->strName.c_str())));

	return true;
}

void PartyManager::OnChangeCharacterName( int nPartyID, PlayerUID nPlayerUID, const char * szNewName )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return;

	for( std::vector< PartyMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		PartyMemberTag & memberTag = (*it);

		if( memberTag.sid == nPlayerUID )
		{
			std::string strPrevDisplayName = memberTag.strDisplayName;
			bool bDisplayNameChanged = false;

			// 실명이 다른 사람들에게 보여지고 있는 경우 표시 이름 변경 처리도 필요
			if( memberTag.strName == memberTag.strDisplayName )
			{
				bDisplayNameChanged = true;
				memberTag.strDisplayName = szNewName;

				// 파장이었으면 파장 표시 이름 갱신
				if( pInfo->nLeaderSID == nPlayerUID )
					pInfo->strLeaderDisplayName = szNewName;
			}

			// 파장이었으면 파장 이름 갱신
			if( pInfo->nLeaderSID == nPlayerUID )
				pInfo->strLeaderName = szNewName;

			memberTag.strName = szNewName;

			if( bDisplayNameChanged )
				PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "CHANGE_NAME|%s|%s|", strPrevDisplayName.c_str(), szNewName );

			break;
		}
	}
}

void PartyManager::OnChangeCharacterLevel( int nPartyID, PlayerUID nPlayerUID, const int nLevel )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return;

	for( std::vector< PartyMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( (*it).sid == nPlayerUID )
		{
			(*it).nLevel = nLevel;
			PrintfPartyChatMessage( CHAT_PARTY_SYSTEM, nPartyID, "CHANGE_LEVEL|%s|%d|", (*it).strDisplayName.c_str(), nLevel );
			break;
		}
	}
}

void PartyManager::OnChangeCharacterJob( int nPartyID, PlayerUID nPlayerUID, const int nJobId )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return;

	for( std::vector< PartyMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( (*it).sid == nPlayerUID )
		{
			(*it).nJobId = nJobId;
			PrintfLinkedPartyChatMessage( false, CHAT_PARTY_SYSTEM, "@PARTY", nPartyID, "CHANGE_JOB|%s|%d|", (*it).strDisplayName.c_str(), nJobId );
			break;
		}
	}
}

int  PartyManager::GetMaxLevel( int nPartyID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	PartyInfo *pInfo = getPartyInfo( nPartyID );
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

int  PartyManager::GetMinLevel( int nPartyID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	PartyInfo *pInfo = getPartyInfo( nPartyID );
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

int  PartyManager::GetTotalLevel( int nPartyID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	PartyInfo *pInfo = getPartyInfo( nPartyID );
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

std::string PartyManager::GetLeaderName( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	const PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return "";

	return pInfo->strLeaderName;
}

std::string	PartyManager::GetLeaderDisplayName( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	const PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return "";

	return pInfo->strLeaderDisplayName;
}

PlayerUID PartyManager::GetLeaderSID( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	const PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return 0;

	return pInfo->nLeaderSID;
}

bool PartyManager::JoinParty( int nPartyID, struct StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return joinParty( getPartyInfo( nPartyID ), pPtr );
}

bool PartyManager::LeaveParty( int nPartyID, PlayerUID nPlayerUID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return leaveParty( getPartyInfo( nPartyID ), nPlayerUID );
}

bool PartyManager::onLogin( int nPartyID, struct StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo * pPartyInfo = getPartyInfo( nPartyID );

	if( !pPartyInfo )
	{
		pPtr->SetPartyID( 0 );
		return false;
	}

	// 유저가 오프라인 된 상태에서 기존의 파티가 해산되고 다른 파티가 생성되면서 파티ID가 재사용된 경우에는
	// 실제로는 멤버가 아닌데 해당 파티에 다시 진입하려는 시도를 할 수도 있으므로 실제 멤버인지 체크 추가
	// * 이 증상은 거의 발생하지 않지만, smp_login_character가 실행된 직후에 smp_delete_party가 실행되면
	//   이미 DB에서 읽어들인 데이터는 party_id != 0 이 되지만 실제로 DB 상의 party_id = 0 이 되어있고,
	//   PartyManager::onLogin 함수가 호출되기 전에 다른 쓰레드에서 해당 파티ID를 사용해 새 파티가 생성되는
	//   아주 절묘한 타이밍에는 발생할 수 있음... 거의 없음 -_ -;;;
	//   그래도 유저 1회 로그인당 1번밖에 실행 안되는 함수이니 처리해주기...
	PlayerUID nPlayerUID = pPtr->GetPlayerUID();
	for( std::vector< PartyMemberTag >::const_iterator it = pPartyInfo->vMemberNameList.begin() ; it != pPartyInfo->vMemberNameList.end() ; ++it )
	{
		if( (*it).sid != nPlayerUID )
			continue;

		// 멤버라는 거 알려주는 걸 변수 딱히 따로 선언하기 뭣해서 기냥 nPlayerUID를 차용 'ㅠ 'ㅋ
		nPlayerUID = -1;
		break;
	}

	// 멤버가 아니네 'ㅠ ';
	if( nPlayerUID != -1 )
	{
		pPtr->SetPartyID( 0 );
		return false;
	}

	if( pPartyInfo->vMemberNameList.size() > MAX_PARTY_MEMBER )
	{
		bool bResult = false;
		if( pPartyInfo->nLeaderSID == pPtr->GetPlayerUID() )
		{
			bResult = destroyParty( pPartyInfo );
		}
		else
		{
			bResult = leaveParty( pPartyInfo, pPtr->GetPlayerUID() );
		}

		// 탈퇴/해산이 불가능한 경우에는 그냥 로그인시켜 줌(어차피 배열도 아니고 벡터로 처리하므로 인덱싱 오류같은 건 안 생기므로... 클라에선 좀 문제될지도 ``;)
		if( bResult )
		{
			pPtr->SetPartyID( 0 );

			return false;
		}
	}
	
	return signAsOnLine( pPartyInfo, pPtr );
}

bool PartyManager::onLogout( int nPartyID, struct StructPlayer *pPtr )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	return signAsOffLine( getPartyInfo( nPartyID ), pPtr );
}

const PartyManager::_ITEM_SHARE_MODE PartyManager::GetShareMode( int nPartyID )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return ITEM_SHARE_MONOPOLY;

	return pInfo->eShareMode;
}

void PartyManager::SetShareMode( int nPartyID, const _ITEM_SHARE_MODE & eShareMode )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return;

	pInfo->eShareMode = eShareMode;

	int nLeadPartyID = ( pInfo->pPartyLinkInfo ) ? pInfo->pPartyLinkInfo->nLeadPartyID : 0;

	if( isDBUpdateRequiredType( pInfo->ePartyType ) )
		Push( new DB_UpdatePartyInfo( nPartyID, pInfo->nLeaderSID, pInfo->eShareMode, pInfo->ePartyType, nLeadPartyID ) );
}

struct StructPlayer * PartyManager::GetNextItemAcquirer( struct StructPlayer * pFinder, const std::vector< struct StructPlayer * > & vPlayers )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	// 파티 구조체 얻기
	PartyInfo *pInfo = getPartyInfo( pFinder->GetPartyID() );
	if( !pInfo ) return NULL;

	switch( pInfo->eShareMode )
	{
	case ITEM_SHARE_MONOPOLY:
		// 각자 분배(습득자 독점)면 습득자가 획득자
		return pFinder;

	case ITEM_SHARE_RANDOM:
		// 랜덤 분배
		return vPlayers[ XRandom() % static_cast< int >( vPlayers.size() ) ];

	case ITEM_SHARE_LINEAR:
		// 순차 분배
		{
			int nMemberCount = static_cast< int >( pInfo->vMemberNameList.size() );
			if( pInfo->nLastItemAcquirerIdx == -1 )
			{
				pInfo->nLastItemAcquirerIdx = nMemberCount - 1;
			}

			int nThisItemAcquirerIdx = ( pInfo->nLastItemAcquirerIdx + 1 ) % nMemberCount;

			while( nThisItemAcquirerIdx != pInfo->nLastItemAcquirerIdx )
			{
				if( !pInfo->vMemberNameList[ nThisItemAcquirerIdx ].bIsOnline )
				{
					nThisItemAcquirerIdx = ( nThisItemAcquirerIdx + 1 ) % nMemberCount;
					continue;
				}

				for( std::vector< struct StructPlayer * >::const_iterator it = vPlayers.begin() ; it != vPlayers.end() ; ++it )
				{
					if( (*it)->GetSID() == pInfo->vMemberNameList[ nThisItemAcquirerIdx ].sid )
					{
						pInfo->nLastItemAcquirerIdx = nThisItemAcquirerIdx;
						return (*it);
					}
				}

				nThisItemAcquirerIdx = ( nThisItemAcquirerIdx + 1 ) % nMemberCount;
			}
		}
		break;
	}

	return pFinder;
}

std::string PartyManager::GetLinkedPartyInfo( int nPartyID ) const
{
	char buf[1024];

	std::string strResult;
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo || !pInfo->pPartyLinkInfo ) return strResult;

	if( _isAttackTeamParty( pInfo ) )
	{
		int nRaidDungeonID = GuildManager::GetInstance().GetRaidDungeonID( static_cast< AttackTeamInfo * >( pInfo->pPartyLinkInfo )->nGuildID );
		s_sprintf( buf, _countof( buf ), "RAID_INFO|%d|%d|%d|%d|", pInfo->pPartyLinkInfo->nLeadPartyID,
			pInfo->pPartyLinkInfo->nMaxPartyCount, nRaidDungeonID + 70000000, pInfo->ePartyType == TYPE_RAID_ATTACKTEAM );
	}
	// 기존의 시즈/레이드용 공대 파티 외의 파티는 모두 아래의 형식으로 보내지만,
	// 차후에라도 BattleArenaTeamParty 외의 타입이 추가되고 아래의 정보 외에 추가 정보를 보내야 하게 되면
	// 아래의 내용이 else 로 분류되지 않고 else if ... else if ... else 로 갈라져야 함
	else
	{
		// 배틀 아레나 연습 경기용 파티의 경우 경기 시작과 함께 실제 경기용 파티로 변경되므로 클라에는 무조건 실제 경기용 파티 타입으로 방송
		s_sprintf( buf, _countof( buf ), "LPINFO|%d|%d|%d|", pInfo->pPartyLinkInfo->nLeadPartyID, pInfo->pPartyLinkInfo->nMaxPartyCount,
			( pInfo->ePartyType != TYPE_BATTLE_ARENA_EXERCISE_TEAM ) ? pInfo->ePartyType : TYPE_BATTLE_ARENA_TEAM );
	}
	strResult = buf;

	for( std::vector< PartyInfo * >::iterator it = pInfo->pPartyLinkInfo->vPartyList.begin(); it != pInfo->pPartyLinkInfo->vPartyList.end(); ++it )
	{
		int nSubPartyID = (*it)->nPartyID;

		if( nPartyID == nSubPartyID )
			continue;

		std::string strMemberInfo;
		// 멤버당 최대 정보 길이를 예상해보면 34 Bytes 쯤 되니 미리 공간을 할당해둬서 그나마 빈번한 메모리 재할당을 방지
		strMemberInfo.reserve( (*it)->vMemberNameList.size() * 34 );
		int nPartyLevel = 0;
		for( std::vector< PartyManager::PartyMemberTag >::iterator itMember = (*it)->vMemberNameList.begin() ; itMember != (*it)->vMemberNameList.end() ; ++itMember )
		{
			PartyManager::PartyMemberTag *pMemberTag = &(*itMember);
			bool bIsProc = false;

			// 파티장의 레벨을 파티의 대표 레벨로 설정
			if( pMemberTag->sid == (*it)->nLeaderSID )
				nPartyLevel = pMemberTag->nLevel;

			do
			{
				if( !pMemberTag->bIsOnline )
					break;

				StructPlayer::iterator pit = StructPlayer::get( StructPlayer::FindPlayer( pMemberTag->strName.c_str() ) );
				StructPlayer * pPlayer = *pit;
				if( !pPlayer )
					break;

				char buf[512];
				s_sprintf( buf, _countof( buf ), "%u|%s|%d|%d|", pPlayer->GetHandle(),
					pMemberTag->strDisplayName.c_str(), pPlayer->GetJobId(), pPlayer->GetHPPercentage() != 0 );
				strMemberInfo += buf;

				bIsProc = true;

			} while( false );

			if( !bIsProc )
			{
				char buf[512];
				s_sprintf( buf, _countof( buf ), "0|%s|%d|1|", pMemberTag->strDisplayName.c_str(), pMemberTag->nJobId );
				strMemberInfo += buf;
			}
		}

		s_sprintf( buf, _countof( buf ), "%d|%s|%s|%d|%d|%d|", (*it)->nPartyID, (*it)->strPartyName.c_str(), (*it)->strLeaderDisplayName.c_str(),
		         nPartyLevel, (*it)->nLeaderJobId, (*it)->vMemberNameList.size() );
		strResult += buf;

		strResult += strMemberInfo;
	}

	return strResult;
}

std::string PartyManager::GetSummonInfo( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return "";

	return "SINFO|" + getSummonInfo( nPartyID );
}

std::string PartyManager::GetLinkedPartySummonInfo( int nPartyID ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return "";

	if( !pInfo->pPartyLinkInfo ) return "";

	std::string strResult = "SINFO|";

	for( std::vector< PartyInfo * >::iterator it = pInfo->pPartyLinkInfo->vPartyList.begin(); it != pInfo->pPartyLinkInfo->vPartyList.end(); ++it )
	{
		strResult += getSummonInfo( (*it)->nPartyID );
	}

	return strResult;
}

size_t PartyManager::DoEachLinkedPartyMember( int nPartyID, PartyFunctor & _fo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return 0;

	if( !pInfo->pPartyLinkInfo ) return 0;

	size_t nRet = 0;

	for( std::vector< PartyInfo * >::iterator it = pInfo->pPartyLinkInfo->vPartyList.begin(); it != pInfo->pPartyLinkInfo->vPartyList.end(); ++it )
	{
		nRet += doEachMember( (*it), _fo );
	}

	return nRet;
}

size_t PartyManager::DoEachAttackTeamMemberByGuildId( int nGuildID, PartyFunctor & _fo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	AttackTeamInfo * pAttackTeamInfo = NULL;
	
	m_hshAttackTeamID.lookup( nGuildID, pAttackTeamInfo );

	if( !pAttackTeamInfo )
		return 0;

	int nRet = 0;

	for( std::vector< PartyInfo * >::iterator it = pAttackTeamInfo->vPartyList.begin(); it != pAttackTeamInfo->vPartyList.end(); ++it )
	{
		nRet += static_cast<int>( doEachMember( (*it), _fo ) );
	}

	return nRet;
}

size_t PartyManager::DoEachMember( int nPartyID, PartyFunctor & _fo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return 0;

	return doEachMember( pInfo, _fo );
}

size_t PartyManager::DoEachMember( int nPartyID, PartyFunctor & _fo ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return 0;

	return doEachMember( pInfo, _fo );
}

size_t PartyManager::DoEachMemberTag( int nPartyID, PartyMemberTagFunctor & _fo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return 0;

	return doEachMemberTag( pInfo, _fo );
}

size_t PartyManager::DoEachParty( int nPartyID, LinkedPartyFunctor & _fo )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	PartyInfo *pInfo = getPartyInfo( nPartyID );
	if( !pInfo || !pInfo->pPartyLinkInfo ) return 0;

	return doEachParty( pInfo->pPartyLinkInfo, _fo );
}

void PartyManager::GetNearMember( struct StructPlayer *pPtr, AR_UNIT distance, std::vector< StructPlayer* > & vList ) const
{
	AR_TIME t = GetArTime();

	struct myPartyFunctor : PartyManager::PartyFunctor
	{
		myPartyFunctor( const ArPosition & _pos, unsigned char _layer, AR_TIME _t, AR_UNIT _dist, std::vector< StructPlayer* > * pList ) : pos( _pos ), layer( _layer ), t( _t ), distance( _dist ), pvList( pList )
		{
		}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( !pPlayer || !pPlayer->IsInWorld() ) return true;

			// 멀리 떨어져 있으면 KIN	
			if( layer != pPlayer->GetLayer() )
				return true;

			AR_UNIT d = pos.GetDistance( pPlayer->GetPos() );	
			if( d > distance ) return true;

			pvList->push_back( pPlayer );

			return true;
		}

		ArPosition pos;
		unsigned char layer;
		AR_TIME t;
		AR_UNIT distance;
		std::vector< StructPlayer* > *pvList;

	} _tmp( pPtr->GetPos(), pPtr->GetLayer(), t, distance, &vList );

	DoEachMember( pPtr->GetPartyID(), _tmp );
}

void PartyManager::GetOfflineMember( int nPartyID, std::vector< PartyMemberTag > & vList ) const
{
	THREAD_SYNCRONIZE( m_IntfLock );

	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return;

	for( std::vector< PartyMemberTag >::const_iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( !(*it).bIsOnline ) vList.push_back( *it );
	}
}

void PartyManager::Push( GameDBManager::DBProc* pWork )
{
	THREAD_SYNCRONIZE( m_QueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}

void PartyManager::onEndQuery()
{
	THREAD_SYNCRONIZE( m_QueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}

PartyManager::PartyManager()
	: m_QueryLock( "PartyManager::m_QueryLock" )
	, m_IntfLock( "PartyManager::m_IntfLock" )
{
	m_nMaxPartyID = 0;
}

PartyManager::~PartyManager()
{
}

bool PartyManager::leaveParty( PartyInfo *pInfo, PlayerUID nPlayerUID )
{
	if( !pInfo ) return false;

	// 파티장이 파티 탈퇴를 하면 덤프 남김(파티장은 leaveParty 함수가 아니라 destroyParty 함수로 들어가도록 호출부에서 처리되어 있어야 함)
	if( pInfo->nLeaderSID == nPlayerUID )
	{
		assert( 0 );
		XSEH::InvokeUnhandledException( 0xC000000DL );
		return false;
	}

	// 파티원이 1명인데 파티 탈퇴가 시도되면 해산 처리 시도
	// 파티장이 탈퇴를 할 수 있었기에 이런 상황이 발생하는 것이므로 이 경우에는 덤프를 남기지 않고 어떻게든 처리해서 넘김
	if( pInfo->vMemberNameList.size() == 1 )
	{
		assert( 0 );

		switch( pInfo->ePartyType )
		{
		// 공대 관련 파티일 경우에는 파티장 인계처리만 하여 파티 해산을 유저가 시도 할 수 있도록 허용
		// 멤버로서 탈퇴는 가능하지만 해산은 하면 안되는 경우가 많음
		case TYPE_RAID_ATTACKTEAM:
		case TYPE_SIEGE_ATTACKTEAM:
			{
				const PartyMemberTag * pMemberTag = &pInfo->vMemberNameList.front();

				int nLeadPartyID = ( pInfo->pPartyLinkInfo ) ? pInfo->pPartyLinkInfo->nLeadPartyID : 0;

				Push( new DB_UpdatePartyInfo( pInfo->nPartyID, pMemberTag->sid, pInfo->eShareMode, pInfo->ePartyType, nLeadPartyID ) );

				pInfo->strLeaderName = pMemberTag->strName;
				pInfo->strLeaderDisplayName = pMemberTag->strDisplayName;
				pInfo->nLeaderSID = pMemberTag->sid;
				pInfo->nLeaderJobId = pMemberTag->nJobId;
			}
			return false;

		// 일반 파티 또는 헌터홀릭 관련 파티면 해산함(탈퇴 시도가 사냥 종료를 뜻하는 경우가 대부분이므로)
		default:
			return destroyParty( pInfo );
		}
	}

	int nIdx = 0;
	for( std::vector< PartyMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it, ++nIdx )
	{
		if( it->sid == nPlayerUID )
		{
			// 멤버 리스트에서 제거(이름은 아래에서 사용해야 하므로 임시 보관)
			std::string strName = it->strName;
			pInfo->vMemberNameList.erase( it );

			// 온라인 상태라면 제거
			StructPlayer::iterator pit = StructPlayer::get( StructPlayer::FindPlayer( strName.c_str() ) );
			StructPlayer *pPtr = *pit;
			if( pPtr ) 
			{
				pPtr->SetPartyID( 0 );
				signAsOffLine( pInfo, pPtr );
			}
			else
			{
				// DB의 해당 캐릭터의 파티 ID 를 0으로 설정함.
				// 파티 ID는 무조건 저장해 줌
				// * 파티 타입에 따라 DB에 파티 정보를 저장하지 않는 경우도 있지만 파티 데이터가 유지되는 상태에서
				//   유저만 재접속하는 경우에는 해당 파티에 가입된 상태를 유지시켜야 하는 경우가 있기 때문
				Push( new DB_SetParty( strName.c_str(), 0 ) );
			}

			// 순차 분배일 경우 파티 탈퇴 인원 발생 시 다음 아이템 습득할 유저 인덱스가 탈퇴 유저 인덱스보다 뒤면 하나 당겨줌
			if( nIdx < pInfo->nLastItemAcquirerIdx )
				--pInfo->nLastItemAcquirerIdx;

			pInfo->nLastItemAcquirerIdx = pInfo->nLastItemAcquirerIdx % static_cast< int >( pInfo->vMemberNameList.size() );
			CPartyMatchingManager::GetInstance()->SendMemberAsChange(*StructPlayer::get(StructPlayer::FindPlayer(pInfo->strLeaderName.c_str())));

			return true;
		}
	}

	return false;
}

bool PartyManager::joinParty( PartyInfo *pInfo, struct StructPlayer *pPtr )
{
	if( !pInfo ) return false;

	if( pInfo->vMemberNameList.size() >= MAX_PARTY_MEMBER )
		return false;

	// 파티매칭에 등록된 인원제한.
	//if(false == CPartyMatchingManager::GetInstance()->CheckNumber(pInfo->nPartyID))
	//{
	//	return false;
	//}

	// 파티 리스트에 추가
	// * 가명 기능 사용 가능 타입의 파티에만 가명 정보 반영
	pInfo->vMemberNameList.push_back( PartyMemberTag( pPtr->GetPlayerUID(), pPtr->GetName(),
		getDisplayNameForPartyType( pInfo->ePartyType, pPtr ), pPtr->GetLevel(), pPtr->GetJobId() ) );

	// 온라인으로 설정
	signAsOnLine( pInfo, pPtr );

	pPtr->SetPartyID( pInfo->nPartyID );

	pPtr->ReleaseItemPriority();

	// 파티 ID는 무조건 저장해 줌
	// * 파티 타입에 따라 DB에 파티 정보를 저장하지 않는 경우도 있지만 파티 데이터가 유지되는 상태에서
	//   유저만 재접속하는 경우에는 해당 파티에 가입된 상태를 유지시켜야 하는 경우가 있기 때문
	Push( new DB_SetParty( pPtr->GetName(), pInfo->nPartyID ) );
	CPartyMatchingManager::GetInstance()->SendMemberAsChange(*StructPlayer::get(StructPlayer::FindPlayer(pInfo->strLeaderName.c_str())));
	return true;
}

bool PartyManager::destroyParty( PartyInfo *pInfo )
{
	if( !pInfo ) return false;

	CPartyMatchingManager::GetInstance()->Remove(*StructPlayer::get(StructPlayer::FindPlayer(pInfo->strLeaderName.c_str())));

	// 연결된 파티가 있는 파티 해산 시
	if( pInfo->pPartyLinkInfo )
	{
		// 연결된 파티의 마스터 파티가 해산될 경우
		if( pInfo->pPartyLinkInfo->nLeadPartyID == pInfo->nPartyID )
		{
			// 시즈/레이드 공대 파티일 경우
			if( pInfo->ePartyType == TYPE_RAID_ATTACKTEAM || pInfo->ePartyType == TYPE_SIEGE_ATTACKTEAM )
			{
				// destroy all raid party

				for( std::vector< PartyInfo * >::iterator it = pInfo->pPartyLinkInfo->vPartyList.begin(); it != pInfo->pPartyLinkInfo->vPartyList.end(); )
				{
					if( (*it) != pInfo )
					{
						destroyParty( (*it) );

						it = pInfo->pPartyLinkInfo->vPartyList.begin();
					}
					else
					{
						++it;
					}
				}
				m_hshAttackTeamID.erase( static_cast< AttackTeamInfo * >( pInfo->pPartyLinkInfo )->nGuildID );

				delete pInfo->pPartyLinkInfo;

				pInfo->pPartyLinkInfo = NULL;
			}
			// 배틀 아레나 파티인 경우
			else if( pInfo->ePartyType == TYPE_BATTLE_ARENA_TEAM || pInfo->ePartyType == TYPE_BATTLE_ARENA_EXERCISE_TEAM )
			{
				// 마지막 남은 파티인 경우
				if( pInfo->pPartyLinkInfo->vPartyList.size() <= 1 )
				{
					delete pInfo->pPartyLinkInfo;

					pInfo->pPartyLinkInfo = NULL;
				}
				// 현재 파티 외에 남은 파티가 있다면 마스터 파티를 남은 파티 중 첫 번째 파티로 변경
				// * 마스터 파티가 배틀 아레나에선 별 의미가 없으므로 막 바꿔 줌
				else
				{
					// 해산될 파티를 링크 파티 목록에서 제거
					std::vector< PartyInfo * >::iterator it = std::find( pInfo->pPartyLinkInfo->vPartyList.begin(), pInfo->pPartyLinkInfo->vPartyList.end(), pInfo );
					if( it != pInfo->pPartyLinkInfo->vPartyList.end() )
					{
						it = pInfo->pPartyLinkInfo->vPartyList.erase( it );
						pInfo->pPartyLinkInfo->nLeadPartyID = pInfo->pPartyLinkInfo->vPartyList.front()->nPartyID;
					}
					else
					{
						// 멤버 파티가 아닌데 해산하려고 보니 현재 링크 파티 정보로 온 것이므로
						// pPartyLinkInfo 가 엉뚱한 놈으로 세팅된 것이거나, 아니면 두 번 이상 destroyParty가 호출된 경우.
						assert( 0 );
					}

					pInfo->pPartyLinkInfo = NULL;
				}
			}
			else
			{
				// 연결된 파티 정보는 있으나 해당 타입의 파티에 대한 해산 처리 대한 코드 작업이 되지 않은 경우...
				assert( 0 );
			}
		}
		// 멤버 파티의 해산인 경우
		else
		{
			for( std::vector< PartyInfo * >::iterator it = pInfo->pPartyLinkInfo->vPartyList.begin(); it != pInfo->pPartyLinkInfo->vPartyList.end(); ++it)
			{
				if( (*it) == pInfo )
				{
					pInfo->pPartyLinkInfo->vPartyList.erase( it );
					break;
				}
			}

			pInfo->pPartyLinkInfo = NULL;
		}
	}

	struct myPartyFunctor : PartyFunctor
	{
		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( !pPlayer ) return true;
			pPlayer->SetPartyID( 0 );

			return true;
		}
	} _fo;

	// 각 플레이어들 파티 정보 클리어
	doEachMember( pInfo, _fo );

	// DB 에서 파티 제거(DB에 저장되지 않은 타입의 파티여도 이건 처리해 줌으로써
	// Party 테이블의 데이터 삭제는 어차피 이루어지지 않지만
	// Character.party_id == pInfo->nPartyID 인 것들을 0 으로 설정하는 처리를 하도록 함
	Push( new DB_DeleteParty( pInfo->nPartyID ) );
	
	m_hshPartyName.erase( pInfo->strPartyName.c_str() );
	m_hshPartyID.erase( pInfo->nPartyID );

	// 파티 정보 지움
	delete pInfo;

	return true;
}

PartyManager::PartyLinkInfo * PartyManager::makeLinkedParty( PartyInfo * pInfo, int nMaxPartyCount )
{
	if( pInfo->pPartyLinkInfo )
		return NULL;

	PartyLinkInfo * pLinkInfo = new PartyLinkInfo();
	pLinkInfo->vPartyList.push_back( pInfo );
	pLinkInfo->nLeadPartyID = pInfo->nPartyID;
	pLinkInfo->nMaxPartyCount = nMaxPartyCount;
	pLinkInfo->nPassword = XRandom();

	pInfo->pPartyLinkInfo = pLinkInfo;

	return pLinkInfo;
}

bool PartyManager::joinLinkedParty( PartyLinkInfo * pLinkInfo, PartyInfo * pInfo )
{
	if( static_cast< int >( pLinkInfo->vPartyList.size() ) >= pLinkInfo->nMaxPartyCount )
		return false;

	pLinkInfo->vPartyList.push_back( pInfo );

	pInfo->pPartyLinkInfo = pLinkInfo;

	return true;
}

PartyManager::AttackTeamInfo * PartyManager::makeAttackTeam( PartyInfo * pInfo, int nGuildID, int nMaxGuildParty )
{
	if( m_hshAttackTeamID.has( nGuildID ) )
		return NULL;

	AttackTeamInfo * pAttackTeamInfo = new AttackTeamInfo;
	pAttackTeamInfo->nPassword = XRandom();
	pAttackTeamInfo->nLeadPartyID = pInfo->nPartyID;
	pAttackTeamInfo->nGuildID = nGuildID;
	pAttackTeamInfo->nMaxPartyCount = nMaxGuildParty;
	pAttackTeamInfo->vPartyList.push_back( pInfo );

	pInfo->pPartyLinkInfo = pAttackTeamInfo;

	m_hshAttackTeamID.add( pAttackTeamInfo->nGuildID, pAttackTeamInfo );

	return pAttackTeamInfo;
}

PartyManager::PartyInfo * PartyManager::makeParty( int nPartyID, const char *szPartyName, int nLeaderSID, const _ITEM_SHARE_MODE & eShareMode, const _PARTY_TYPE & ePartyType )
{
	if( ePartyType == TYPE_UNKNOWN )
	{
		assert( 0 );
		return NULL;
	}

	// 파티 구조체 생성
	PartyInfo * pInfo = new PartyInfo;
	pInfo->strPartyName = szPartyName;
	pInfo->nPartyID = nPartyID;
	pInfo->nPartyPassword = XRandom();
	pInfo->nLeaderSID = nLeaderSID;
	pInfo->eShareMode = eShareMode;
	pInfo->nLastItemAcquirerIdx = -1;

	pInfo->ePartyType = ePartyType;

	pInfo->pPartyLinkInfo = NULL;

	// 해쉬에 등록
	if( registerParty( pInfo ) == false )
	{
		delete pInfo;
		return NULL;
	}

	return pInfo;
}

bool	PartyManager::registerParty( PartyInfo* pInfo )
{
	if( m_hshPartyID.add( pInfo->nPartyID, pInfo ) == NULL )
	{
		return false;
	}

	if( m_hshPartyName.add( pInfo->strPartyName.c_str(), pInfo ) == NULL )
	{
		int nHas = (m_hshPartyName.has( pInfo->strPartyName.c_str() ) ? 1 : 0);
		FILELOG( "(register) KHash add failed PartyID: %d, PartyName: %s(has: %d)", pInfo->nPartyID, pInfo->strPartyName.c_str(), nHas );
		_cprint( "(register) KHash add failed PartyID: %d, PartyName: %s(has: %d)\n", pInfo->nPartyID, pInfo->strPartyName.c_str(), nHas );

		m_hshPartyID.erase( pInfo->nPartyID );
		return false;
	}

	return true;
}

int PartyManager::allocPartyID()
{
	return InterlockedIncrement( &m_nMaxPartyID );
}

PartyManager::PartyInfo * PartyManager::getPartyInfo( const char *szPartyName )
{
	PartyInfo * p = NULL;

	m_hshPartyName.lookup( szPartyName, p );

	return p;
}

const char * PartyManager::getDisplayNameForPartyType( _PARTY_TYPE ePartyType, struct StructPlayer * pPlayer )
{
	return ( isAliasUsableParty( ePartyType ) ) ? pPlayer->GetAlias() : pPlayer->GetName();
}

const PartyManager::PartyInfo * PartyManager::getPartyInfo( const char *szPartyName ) const
{
	PartyInfo * p = NULL;

	m_hshPartyName.lookup( szPartyName, p );

	return p;
}

PartyManager::PartyInfo * PartyManager::getPartyInfo( int nPartyID )
{
	PartyInfo * p = NULL;

	m_hshPartyID.lookup( nPartyID, p );

	return p;
}

const PartyManager::PartyInfo * PartyManager::getPartyInfo( int nPartyID ) const
{
	PartyInfo * p = NULL;

	m_hshPartyID.lookup( nPartyID, p );

	return p;
}

const std::string PartyManager::getSummonInfo( int nPartyID ) const
{
	const PartyInfo * pInfo = getPartyInfo( nPartyID );
	if( !pInfo ) return "";

	struct myPartyFunctor : PartyFunctor
	{
		myPartyFunctor()	: count( 0 ) {}

		virtual bool operator()( AR_HANDLE handle )
		{
			StructPlayer::iterator pit = StructPlayer::get( handle );
			StructPlayer *pPlayer = *pit;
			if( !pPlayer ) return true;

			AR_HANDLE nMainSummonHandle = pPlayer->GetMainSummon() ? pPlayer->GetMainSummon()->GetHandle() : 0;
			AR_HANDLE nSubSummonHandle = pPlayer->GetSubSummon() ? pPlayer->GetSubSummon()->GetHandle() : 0;

			if( !nMainSummonHandle && !nSubSummonHandle ) return true;

			char buf[50];
			s_sprintf( buf, _countof( buf ), "%u|%u|%u|", handle, nMainSummonHandle, nSubSummonHandle );

			count++;
			strPartyInfo += buf;

			return true;
		}

		int count;
		std::string strPartyInfo;
	} _fo;

	doEachMember( pInfo, _fo );

	std::string strResult;
	XStringUtil::Format( strResult, "%d|%d|%s", nPartyID, _fo.count, _fo.strPartyInfo.c_str() );

	return strResult;
}

size_t PartyManager::doEachMember( PartyInfo * pInfo, PartyFunctor & _fo )
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

size_t PartyManager::doEachMember( const PartyInfo * pInfo, PartyFunctor & _fo ) const
{
	size_t cnt = 0;	

	std::vector< AR_HANDLE >::const_iterator it;
	for( it = pInfo->vOnlineList.begin(); it != pInfo->vOnlineList.end(); ++it )
	{
		_fo( *it );
		++cnt;
	}

	return cnt;
}

size_t PartyManager::doEachMemberTag( PartyInfo * pInfo, PartyMemberTagFunctor & _fo )
{
	size_t cnt = 0;

	for( std::vector< PartyMemberTag >::iterator it = pInfo->vMemberNameList.begin() ; it != pInfo->vMemberNameList.end() ; ++it )
	{
		_fo( &(*it) );
		++cnt;
	}

	return cnt;
}

size_t PartyManager::doEachParty( PartyLinkInfo * pLinkInfo, LinkedPartyFunctor & _fo )
{
	size_t cnt = 0;

	for( std::vector< PartyInfo * >::iterator it = pLinkInfo->vPartyList.begin() ; it != pLinkInfo->vPartyList.end() ; ++it )
	{
		_fo( (*it)->nPartyID );
		++cnt;
	}

	return cnt;
}

bool PartyManager::signAsOnLine( PartyInfo *pInfo, struct StructPlayer *pPtr )
{
	if( !pInfo ) return false;

	pInfo->vOnlineList.push_back( pPtr->GetHandle() );

	if( pInfo->nLeaderSID == pPtr->GetPlayerUID() )
	{
		pInfo->strLeaderName = pPtr->GetName();
		pInfo->strLeaderDisplayName = getDisplayNameForPartyType( pInfo->ePartyType, pPtr );
		pInfo->nLeaderJobId = pPtr->GetJobId();
	}

	for( std::vector< PartyMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( (*it).sid == pPtr->GetPlayerUID() )
		{
			(*it).bIsOnline = true;

			(*it).nJobId = pPtr->GetJobId();
			(*it).nLevel = pPtr->GetLevel();

			break;
		}
	}

	return true;
}

bool PartyManager::signAsOffLine( PartyInfo *pInfo, struct StructPlayer *pPtr )
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

	if( pInfo->nLeaderSID == pPtr->GetPlayerUID() )
	{
		pInfo->nLeaderJobId = pPtr->GetJobId();
	}

	for( std::vector< PartyMemberTag >::iterator it = pInfo->vMemberNameList.begin(); it != pInfo->vMemberNameList.end(); ++it )
	{
		if( (*it).sid == pPtr->GetPlayerUID() )
		{
			(*it).bIsOnline = false;

			(*it).nJobId = pPtr->GetJobId();
			(*it).nLevel = pPtr->GetLevel();

			break;
		}
	}

	return false;	
}

