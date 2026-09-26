
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "CommunityLoader.h"
#include "PartyManager.h"
#include "GameDBUtil.h"
#include "DB_Commands.h"
#include "GuildManager.h"
#include "DungeonManager.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbParty : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbParty)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, party_sid, sizeof(party_sid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adVarWChar, szPartyName, _countof(szPartyName) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, leader_sid, sizeof(leader_sid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, share_mode, sizeof(share_mode), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, party_type, sizeof(party_type), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, lead_party_id, sizeof(lead_party_id), FALSE)
   
END_ADO_BINDING()

	int party_sid;
	wchar_t szPartyName[61];
	int leader_sid;
	int share_mode;
	int party_type;
	int lead_party_id;
};

bool PartyLoader::onProcess( int nThreadNum )
{
	if( !PartyManager::GetInstance().Init() )
	{
		_cprint( "Party load error!\n" );
		FILELOG( "Party load error!" );
		return false;
	}
	return true;
}

int get_guild_id( _ConnectionPtr & connection, int leader_sid )
{
	_CommandPtr command;
	if( CreateDBCommand( command, connection ) == false )
	{
		throw XException( "ADO COMMAND_PTR CREATE ERROR!" );
	}

	command->CommandType = adCmdStoredProc;
	command->CommandText = _bstr_t( "dbo.smp_get_guild_id" );

	command->Parameters->Append( command->CreateParameter( "IN_PLAYER_SID", adInteger, adParamInput, sizeof( leader_sid ), leader_sid ) );
	command->Parameters->Append( command->CreateParameter( "OUT_GUILD_ID", adInteger, adParamOutput, 4, 0 ) );

	_RecordsetPtr rs;

	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		rs = command->Execute( NULL, NULL, adCmdStoredProc );
		helper.end( "dbo.smp_get_guild_id" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "dbo.smp_get_guild_id" );
		LogDBError( e, "get_guild_id()", "dbo.smp_get_guild_id" );

		std::string strError = "GameDB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	if( command->Parameters->Item[ "OUT_GUILD_ID" ]->Value.vt != VT_NULL )
	{
		return command->Parameters->Item[ "OUT_GUILD_ID" ]->Value;
	}

	return 0;
}

void PartyManager::loadPartyMemberTagList()
{
	DBPerformanceTrackHelper helper;
	try
	{
		DBConnection db;
		InitUserDbConnection( db.connection );
		if( db.CreateCommand( db.command ) == false )
		{
			throw XException( "ADO COMMAND_PTR CREATE ERROR!" );
		}

		db.command->CommandType = adCmdStoredProc;
		db.command->CommandText = _bstr_t( "dbo.smp_load_party_member_info" );

		_RecordsetPtr rs;

		helper.start();
		rs = db.command->Execute( NULL, NULL, adCmdStoredProc );
		helper.end( "dbo.smp_load_party_member_info" );

		int nLoadingPartyID = 0;
		std::vector< PartyMemberTag > *pvLoadingPartyMemberTagList = NULL;

		while( rs->State != adStateClosed && !rs->EndOfFile )
		{
			int sid, party_id, level, job;
			_bstr_t name = rs->Fields->GetItem("name")->Value;

			sid = rs->Fields->GetItem("sid")->Value;
			party_id = rs->Fields->GetItem("party_id")->Value;
			level = rs->Fields->GetItem("lv")->Value;
			job = rs->Fields->GetItem("job")->Value;

			// 최초 로딩이거나 이전 로딩된 데이터의 길드 ID가 현재 로드된 길드 ID와 다를 경우 해쉬에서 새로 검색
			if( !nLoadingPartyID || nLoadingPartyID != party_id )
			{
				// 기존 해쉬에 없던 길드 ID면 벡터 새로 생성
				if( !m_hshPartyMemberTagList.lookup( party_id, pvLoadingPartyMemberTagList ) )
				{
					pvLoadingPartyMemberTagList = new std::vector< PartyMemberTag >;
					m_hshPartyMemberTagList.add( party_id, pvLoadingPartyMemberTagList );
				}
				nLoadingPartyID = party_id;
			}

			assert( nLoadingPartyID == party_id && pvLoadingPartyMemberTagList );

			// DB에 저장된 파티원 정보는 가명 기능을 사용하지 않으므로 별도로 DB에서 Character.alias를 로드하지 않고 name으로 동일하게 세팅
			pvLoadingPartyMemberTagList->push_back( PartyManager::PartyMemberTag( sid, name, name, level, job ) );
			rs->MoveNext();
		}
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "dbo.smp_load_party_member_info" );
		LogDBError( e, "PartyManager", "dbo.smp_load_party_member_info" );

		std::string strError = "GameDB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}
}

int PartyManager::_getPartyMemberTagList( const int party_id, const int leader_sid, std::vector< PartyManager::PartyMemberTag > & result )
{
	std::vector< PartyManager::PartyMemberTag > *pvMemberTagList = NULL;
	if( !m_hshPartyMemberTagList.lookup( party_id, pvMemberTagList ) )
	{
		_cprint( "Party info with no member detected: party_id(%d)\n", party_id );
		FILELOG( "Party info with no member detected: party_id(%d)", party_id );
		return 0;
	}

	// 파티 리더 인덱스 찾기
	int leader_index = 0;
	for( std::vector< PartyManager::PartyMemberTag >::iterator it = pvMemberTagList->begin() ; it != pvMemberTagList->end() ; ++it, ++leader_index )
	{
		if( (*it).sid == leader_sid )
			break;
	}
	if( leader_index == pvMemberTagList->size() )	// leader_sid에 해당하는 Character 데이터 없음
		leader_index = -1;

	// 벡터를 교환
	result.swap( *pvMemberTagList );

	// 사용된 PartyMemberTag 리스트는 제거
	m_hshPartyMemberTagList.erase( party_id );
	delete pvMemberTagList;

	return leader_index;
}

void PartyManager::loadPartyList()
{
	DBConnection db;
	InitUserDbConnection( db.connection );
	if( db.CreateCommand( db.command ) == false )
	{
		throw XException( "ADO COMMAND_PTR CREATE ERROR!" );
	}

	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_load_party_list" );
	
	_RecordsetPtr pRstParty;

	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		pRstParty = db.command->Execute(NULL, NULL, adCmdText);
		helper.end( "dbo.smp_load_party_list" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "dbo.smp_load_party_list" );
		LogDBError( e, "PartyManager", "dbo.smp_load_party_list" );

		std::string strError = "GameDB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	dbParty emprs;       // C++ Class object

	pRstParty->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );

	picRs->BindToRecordset(&emprs);

	int cnt = 0;
	
	while( pRstParty->State != adStateClosed && !pRstParty->EndOfFile )
	{	
		// max 파티 ID 수정
		if( m_nMaxPartyID < emprs.party_sid )	m_nMaxPartyID = emprs.party_sid;

		char szPartyName[61];

		int code_page = ENV().GetInt( "CodePage", CP_ACP );
		WideCharToMultiByte( code_page, 0, emprs.szPartyName, 61, szPartyName, 61, NULL, NULL );

		// 파티 생성
		PartyInfo *pInfo = makeParty( emprs.party_sid, szPartyName, emprs.leader_sid, static_cast< _ITEM_SHARE_MODE >( emprs.share_mode ), static_cast< _PARTY_TYPE >( emprs.party_type ) );
		if( pInfo == NULL )
		{
			FILELOG( "ERROR: makeParty failed, PartyID: %d, PartyName: %s", emprs.party_sid, szPartyName );
			_cprint( "ERROR: makeParty failed, PartyID: %d, PartyName: %s\n", emprs.party_sid, szPartyName );

			pRstParty->MoveNext();
			continue;
		}

		// 멤버 정보 얻어온다
		int leader_index = _getPartyMemberTagList( emprs.party_sid, emprs.leader_sid, pInfo->vMemberNameList );

		// 1. 파티원 없는 파티이거나
		// 2. 리더가 없는 파티이거나
		// 3. 베어로드 타입의 파티는 삭제
		// 4. 배틀 아레나 관련 파티는 삭제
		// * 이 시점에 파티를 삭제해야 Party 테이블의 데이터와 로딩된 PartyMemberTag가 제대로 삭제 처리되며, DB상의 Character.party_id도 0으로 리셋됨
		if( pInfo->vMemberNameList.empty() ||
			leader_index == -1 ||
			pInfo->ePartyType == TYPE_HUNTAHOLIC_PARTY ||
			pInfo->ePartyType == TYPE_BATTLE_ARENA_TEAM ||
			pInfo->ePartyType == TYPE_BATTLE_ARENA_EXERCISE_TEAM )
		{
			DestroyParty( pInfo->nPartyID );			

			pRstParty->MoveNext();
			continue;
		}

		const std::string & strLeaderName = pInfo->vMemberNameList[ leader_index ].strName;
		pInfo->strLeaderName = strLeaderName;
		// DB에서 로드되는 타입의 파티에서는 가명을 사용하지 않기 때문에 실명을 표시 이름으로 세팅해 줌
		pInfo->strLeaderDisplayName = strLeaderName;
		pInfo->nLeaderJobId = pInfo->vMemberNameList[ leader_index ].nJobId;

		if( emprs.party_type == TYPE_RAID_ATTACKTEAM || emprs.party_type == TYPE_SIEGE_ATTACKTEAM )
		{
			int guild_id = get_guild_id( db.connection, pInfo->nLeaderSID );
			int dungeon_id = GuildManager::GetInstance().GetRaidDungeonID( guild_id );
			bool bIsRaidParty = emprs.party_type == TYPE_RAID_ATTACKTEAM;

			if( !guild_id || !dungeon_id )
			{
				destroyParty( pInfo );

				pRstParty->MoveNext();
				continue;
			}

			if( emprs.lead_party_id != pInfo->nPartyID )
			{
				if( !IsExistAttackTeam( guild_id ) )
				{
					destroyParty( pInfo );

					pRstParty->MoveNext();
					continue;
				}

				PartyInfo * pLeadPartyInfo = getPartyInfo( emprs.lead_party_id );
				if( !pLeadPartyInfo || !pLeadPartyInfo->pPartyLinkInfo || pLeadPartyInfo->ePartyType != pInfo->ePartyType )
				{
					// 이거 현실적으로 있을 수가 없는게, 게임 내에서 공대 파티의 메인 파티가 먼저 결성되고 그 후에 서브 파티가 결성되기 때문에
					// 로딩 시 메인 파티보다 서브 파티가 먼저 로드될 가능성은 거의 없음. 발생했다면 어떤 경우인지 확인하고, 그 상황을 피할 수 없다면
					// 메인 파티보다 먼저 로드된 서브 파티의 목록을 가지고 있다가 모든 파티 로드 후에 추가로 join 시켜줘야 함. 이 처리는 구현되어 있지 않음
					// 그리고 메인 파티는 있었는데 파티 링크 정보만 없는 경우는 메인 파티가 로드는 됐는데 DB 상에서 lead_party_id 가 0 이었다는 이야기가 됨.
					// 정상 상태가 아니니 별도의 대처도 불가능.
					assert( 0 );

					// 현재는 예상 밖의 상황이니 그냥 파티 해산 -,. -;
					destroyParty( pInfo );

					pRstParty->MoveNext();
					continue;
				}

				if( !joinLinkedParty( pLeadPartyInfo->pPartyLinkInfo, pInfo ) )
				{
					// 읭 -_ -;?
					destroyParty( pInfo );

					pRstParty->MoveNext();
					continue;
				}
			}
			else
			{
				int nMaxGuildParty = 0;

				if( bIsRaidParty )
				{
					nMaxGuildParty = DungeonManager::Instance().GetMaxRaidParty( dungeon_id );
				}
				else
				{
					nMaxGuildParty = DungeonManager::Instance().GetMaxGuildParty( dungeon_id );
				}

				if( !makeAttackTeam( pInfo, guild_id, nMaxGuildParty ) )
				{
					destroyParty( pInfo );

					pRstParty->MoveNext();
					continue;
				}
			}
		}
		pRstParty->MoveNext();
		cnt++;		
	}

	picRs->Release();
	pRstParty->Close();	

	_cprint( "Total %d Party loaded.\n", cnt );
	FILELOG( "Total %d Party loaded.", cnt );
	
	return;
}

void PartyManager::finishLoading()
{
	// 로딩에 사용되는 PartyMemberTag 가 남아있으면 제거
	KHash< std::vector< PartyMemberTag > *, hashPr_mod_int >::node *pValueNode = NULL;
	bool bQuitFlag = m_hshPartyMemberTagList.get_first_node( pValueNode );
	while( bQuitFlag )
	{
		_cprint( "Party info does not exist: party_id(%d)\n", pValueNode->key );
		FILELOG( "Party info does not exist: party_id(%d)", pValueNode->key );

		pValueNode->value->clear();
		delete pValueNode->value;

		bQuitFlag = m_hshPartyMemberTagList.get_next_node( pValueNode );
	}

	m_hshPartyMemberTagList.clear();
}
