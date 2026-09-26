
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "CommunityLoader.h"
#include "GuildManager.h"
#include "GameDBUtil.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "DB_Commands.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbAlliance : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbAlliance)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, alliance_sid, sizeof(alliance_sid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adVarWChar, szAllianceName, _countof(szAllianceName) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, leader_sid, sizeof(leader_sid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, max_alliance_cnt, sizeof(max_alliance_cnt), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, name_changed, sizeof(name_changed), FALSE)

   
END_ADO_BINDING()

	int alliance_sid;
	wchar_t szAllianceName[61];
	int leader_sid;
	int max_alliance_cnt;
	int name_changed;
};

struct dbGuild : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbGuild)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, guild_sid, sizeof(guild_sid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adVarWChar, szGuildName, _countof(szGuildName) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adVarWChar, szGuildNotice, _countof(szGuildNotice) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adVarWChar, szGuildURL, _countof(szGuildURL) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adVarChar, szIconFileName, _countof(szIconFileName) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, icon_size, sizeof(icon_size), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(7, adVarChar, szBannerFileName, _countof(szBannerFileName) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, nBannerSize, sizeof(nBannerSize), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(9, adTinyInt, nAdvertiseType, sizeof(nAdvertiseType), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, adDBTimeStamp, tAdvertiseEnd, sizeof( tAdvertiseEnd ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, adVarWChar, szAdvertiseComment, _countof(szAdvertiseComment) - 1, FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(12, adBoolean, bRecruiting, sizeof( bRecruiting ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(13, adSmallInt, nMinRecruitLevel, sizeof(nMinRecruitLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, adSmallInt, nMaxRecruitLevel, sizeof(nMaxRecruitLevel), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(15, adInteger, name_changed, sizeof(name_changed), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(16, adInteger, dungeon_id, sizeof(dungeon_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(17, adBigInt, dungeon_block_time, sizeof(dungeon_block_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(18, adBigInt, gold, sizeof(gold), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(19, adInteger, chaos, sizeof(chaos), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(20, adInteger, alliance_id, sizeof(alliance_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(21, adBigInt, alliance_block_time, sizeof(alliance_block_time), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(22, adInteger, donation_point, sizeof(donation_point), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(23, adVarWChar, permission_name[ 0 ], _countof(permission_name[ 0 ]) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(24, adInteger, permission_set[ 0 ], sizeof(permission_set[ 0 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(25, adVarWChar, permission_name[ 1 ], _countof(permission_name[ 1 ]) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(26, adInteger, permission_set[ 1 ], sizeof(permission_set[ 1 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(27, adVarWChar, permission_name[ 2 ], _countof(permission_name[ 2 ]) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(28, adInteger, permission_set[ 2 ], sizeof(permission_set[ 2 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(29, adVarWChar, permission_name[ 3 ], _countof(permission_name[ 3 ]) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(30, adInteger, permission_set[ 3 ], sizeof(permission_set[ 3 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(31, adVarWChar, permission_name[ 4 ], _countof(permission_name[ 4 ]) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(32, adInteger, permission_set[ 4 ], sizeof(permission_set[ 4 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(33, adVarWChar, permission_name[ 5 ], _countof(permission_name[ 5 ]) - 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(34, adInteger, permission_set[ 5 ], sizeof(permission_set[ 5 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(35, adInteger, grade, sizeof(grade), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(36, adInteger, point, sizeof(point), FALSE)

END_ADO_BINDING()

	int guild_sid;
	wchar_t szGuildName[61];
	wchar_t szGuildNotice[256];
	wchar_t szGuildURL[129];

	char szIconFileName[256];
	int icon_size;

	char szBannerFileName[256];
	int nBannerSize;

	unsigned char nAdvertiseType;
	wchar_t szAdvertiseComment[256];
	DBTIMESTAMP tAdvertiseEnd;

	bool bRecruiting;
	short nMinRecruitLevel;
	short nMaxRecruitLevel;

	int name_changed;
	int dungeon_id;
	time_t dungeon_block_time;
	__int64 gold;
	int chaos;
	int alliance_id;
	time_t alliance_block_time;
	int donation_point;
	wchar_t permission_name[ GuildManager::PERMISSION_SET_TYPE_COUNT ][ 33 ];
	int permission_set[ GuildManager::PERMISSION_SET_TYPE_COUNT ];
	int grade;
	int point;
};

bool GuildLoader::onProcess( int nThreadNum )
{
	if( !GuildManager::GetInstance().Init() )
	{
		FILELOG( "Guild load error!" );
		_cprint( "Guild load error!\n" );
		return false;
	}
	return true;
}

void GuildManager::LoadGuildIconInfo( int nGuildID )
{
	DBConnection db;
	InitUserDbConnection( db.connection );
	if( db.CreateCommand( db.command ) == false )
	{
		throw XException( "ADO COMMAND_PTR CREATE ERROR!" );
	}

	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_guild_icon_info" );

	db.command->Parameters->Append( db.command->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( nGuildID ), nGuildID ) );

	_RecordsetPtr rs;

	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		rs = db.command->Execute(NULL,NULL,adCmdStoredProc);
		helper.end( "smp_read_guild_icon_info" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "smp_read_guild_icon_info" );
		LogDBError( e, "GuildManager", "smp_read_guild_icon_info" );

		std::string strError = "GameDB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	if( rs->State != adStateClosed && !rs->EndOfFile )
	{
		if( updateGuildInfo( nGuildID, (_bstr_t) rs->Fields->GetItem("icon")->Value, rs->Fields->GetItem("icon_size")->Value ) )
		{
			char szBuf[255];
			s_sprintf( szBuf, _countof( szBuf ), "GICON_UPDATE|%d", nGuildID );
			SendGlobalChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", szBuf, static_cast<unsigned int>( strlen( szBuf ) ) );
		}
	}
}

void GuildManager::LoadGuildBannerInfo( int nGuildID )
{
	DBConnection db;
	InitUserDbConnection( db.connection );
	if( db.CreateCommand( db.command ) == false )
	{
		throw XException( "ADO COMMAND_PTR CREATE ERROR!" );
	}

	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_guild_banner_info" );

	db.command->Parameters->Append( db.command->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, sizeof( nGuildID ), nGuildID ) );

	_RecordsetPtr rs;

	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		rs = db.command->Execute(NULL,NULL,adCmdStoredProc);
		helper.end( "smp_read_guild_banner_info" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "smp_read_guild_banner_info" );
		LogDBError( e, "GuildManager", "smp_read_guild_banner_info" );

		std::string strError = "GameDB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	if( rs->State != adStateClosed && !rs->EndOfFile )
	{
		if( updateGuildBannerInfo( nGuildID, (_bstr_t) rs->Fields->GetItem("banner")->Value, rs->Fields->GetItem("banner_size")->Value ) )
		{
			char szBuf[255];
			s_sprintf( szBuf, _countof( szBuf ), "GBANNER_UPDATE|%d", nGuildID );
			SendGlobalChatMessage( CHAT_GUILD_SYSTEM, "@GUILD", szBuf, static_cast<unsigned int>( strlen( szBuf ) ) );
		}
	}
}

void GuildManager::loadAllianceList()
{
	_ConnectionPtr		ConnPtr = NULL;
	InitUserDbConnection( ConnPtr );

	_RecordsetPtr		pRstAlliance = NULL;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.
	
	dbAlliance emprs;       // C++ Class object

	pRstAlliance.CreateInstance( __uuidof(Recordset) );

	pRstAlliance->Open("Alliance", 
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdTable);

	pRstAlliance->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );

	picRs->BindToRecordset(&emprs);

	int cnt = 0;
	
	while( pRstAlliance->State != adStateClosed && !pRstAlliance->EndOfFile )
	{	
		// max 길드 ID 수정
		if( m_nMaxAllianceId < emprs.alliance_sid )	m_nMaxAllianceId = emprs.alliance_sid;

		char szAllianceName[61];

		int code_page = ENV().GetInt( "CodePage", CP_ACP );
		WideCharToMultiByte( code_page, 0, emprs.szAllianceName, 61, szAllianceName, 61, NULL, NULL );
		// 길드 생성
		makeAlliance( emprs.alliance_sid, emprs.leader_sid, szAllianceName, emprs.max_alliance_cnt, emprs.name_changed );

		pRstAlliance->MoveNext();
		cnt++;		
	}

	picRs->Release();
	pRstAlliance->Close();	

	_cprint( "Total %d Alliance loaded.\n", cnt );
	FILELOG( "Total %d Alliance loaded.", cnt );
	
	return;
}

void GuildManager::loadGuildMemberTagList()
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

		db.command->CommandType = adCmdText;
		db.command->CommandText = _bstr_t( "dbo.smp_read_guild_member_list" );
		db.command->CommandTimeout = 10*60;	// 10분

		_RecordsetPtr rs;
		helper.start();
		rs = db.command->Execute( NULL, NULL, adCmdText );
		helper.end( "smp_read_guild_member_list" );

		int nLoadingGuildID = 0;
		std::vector< GuildMemberTag > *pvLoadingGuildMemberTagList = NULL;

		while( rs->State != adStateClosed && !rs->EndOfFile )
		{
			_bstr_t name = rs->Fields->GetItem("name")->Value;

			PlayerUID sid = rs->Fields->GetItem("player_id")->Value;
			int guild_id = rs->Fields->GetItem("guild_id")->Value;
			int level = rs->Fields->GetItem("lv")->Value;
			int job = rs->Fields->GetItem("job")->Value;
			char permission = rs->Fields->GetItem("permission")->Value;
			std::string memo = static_cast< const char * >( _bstr_t( rs->Fields->GetItem("memo")->Value.bstrVal ) );
			int points = rs->Fields->GetItem("point")->Value;
			int totalpoints = rs->Fields->GetItem("total_point")->Value;

			// 최초 로딩이거나 이전 로딩된 데이터의 길드 ID가 현재 로드된 길드 ID와 다를 경우 해쉬에서 새로 검색
			if( !nLoadingGuildID || nLoadingGuildID != guild_id )
			{
				// 기존 해쉬에 없던 길드 ID면 벡터 새로 생성
				if( !m_hshGuildMemberTagList.lookup( guild_id, pvLoadingGuildMemberTagList ) )
				{
					pvLoadingGuildMemberTagList = new std::vector< GuildMemberTag >;
					m_hshGuildMemberTagList.add( guild_id, pvLoadingGuildMemberTagList );
				}
				nLoadingGuildID = guild_id;
			}

			assert( nLoadingGuildID == guild_id && pvLoadingGuildMemberTagList );

			StructPlayer::RegisterPlayerName( sid, static_cast< const char* >( name ) );
#ifdef USE_CLIENT97
			pvLoadingGuildMemberTagList->push_back( GuildManager::GuildMemberTag( sid, level, job, permission, memo.c_str(), points, totalpoints ) );
#else
			pvLoadingGuildMemberTagList->push_back(GuildManager::GuildMemberTag(sid, level, job, permission, memo.c_str()));
#endif
			rs->MoveNext();
		}
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "smp_read_guild_member_list" );
		LogDBError( e, "GuildManager", "smp_read_guild_member_list" );

		std::string strError = "GameDB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}
}

int GuildManager::_getGuildMemberTagList( const int guild_id, std::vector< GuildManager::GuildMemberTag > & result )
{
	std::vector< GuildManager::GuildMemberTag > *pvMemberTagList = NULL;
	if( !m_hshGuildMemberTagList.lookup( guild_id, pvMemberTagList ) )
	{
		_cprint( "Guild info with no member detected: guild_id(%d)\n", guild_id );
		FILELOG( "Guild info with no member detected: guild_id(%d)", guild_id );
		return 0;
	}

	// 벡터를 복사함
	// GuildMemberTag의 복사 생성자 호출이 벡터 길이만큼 이루어 짐
	// 그!러!나! DB에 매번 Select 하는 짓거리보단 나음 -_ -;
	result = *pvMemberTagList;

	// 길드 리더 인덱스 찾기
	int leader_index = 0;
	for( std::vector< GuildManager::GuildMemberTag >::iterator it = pvMemberTagList->begin() ; it != pvMemberTagList->end() ; ++it, ++leader_index )
	{
		if( (*it).nPermission == PERMISSION_LEADER )
			break;
	}
	if( leader_index == pvMemberTagList->size() )	// leader_sid에 해당하는 Character 데이터 없음
	{
		_cprint( "Guild info with no leader detected: guild_id(%d)\n", guild_id );
		FILELOG( "Guild info with no leader detected: guild_id(%d)", guild_id );

		leader_index = -1;
	}

	// 사용된 GuildMemberTag 리스트는 제거
	m_hshGuildMemberTagList.erase( guild_id );
	pvMemberTagList->clear();
	delete pvMemberTagList;

	return leader_index;
}

int GuildManager::getUserIndexInGuild( const int guild_id, const PlayerUID user_sid )
{
	THREAD_SYNCRONIZE( m_IntfLock );

	GuildInfo *pInfo = getGuildInfo( guild_id );
	if( !pInfo ) return -1;

	int nIndex = 0;
	for( std::vector< GuildManager::GuildMemberTag >::iterator it = pInfo->vMemberNameList.begin() ; it != pInfo->vMemberNameList.end() ; ++it, ++nIndex )
	{
		if( (*it).sid == user_sid )
			return nIndex;
	}

	return -1;
}

void GuildManager::loadGuildList()
{
	_ConnectionPtr		ConnPtr = NULL;
	InitUserDbConnection( ConnPtr );

	_RecordsetPtr		pRstGuild = NULL;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.
	
	dbGuild emprs;       // C++ Class object

	pRstGuild.CreateInstance( __uuidof(Recordset) );
	pRstGuild->Open("Guild", 
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdTable);

	pRstGuild->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );

	picRs->BindToRecordset(&emprs);

	int cnt = 0;

	while( pRstGuild->State != adStateClosed && !pRstGuild->EndOfFile )
	{
		// 잘못된 길드 정보일 경우 수정을 위해 DB 업뎃이 필요한지를 나타내는 플래그
		bool bNeedToUpdateGuildInfo = false;

		// max 길드 ID 수정
		if( m_nMaxGuildId < emprs.guild_sid )
			m_nMaxGuildId = emprs.guild_sid;

		char szGuildName[ 61 ];
		char szGuildURL[ 128 ];
		char szGuildNotice[ 256 ];
		char szAdvertiseComment[ 256 ];
		char szPermissionName[ GuildManager::PERMISSION_SET_TYPE_COUNT ][ 33 ];

		int code_page = ENV().GetInt( "CodePage", CP_ACP );
		WideCharToMultiByte( code_page, 0, emprs.szGuildName, -1, szGuildName, _countof( szGuildName ), NULL, NULL );
		WideCharToMultiByte( code_page, 0, emprs.szGuildURL, -1, szGuildURL, _countof( szGuildURL ), NULL, NULL );
		WideCharToMultiByte( code_page, 0, emprs.szGuildNotice, -1, szGuildNotice, _countof( szGuildName ), NULL, NULL );
		WideCharToMultiByte( code_page, 0, emprs.szAdvertiseComment, -1, szAdvertiseComment, _countof( szAdvertiseComment ), NULL, NULL );
		for( int i = 0 ; i < GuildManager::PERMISSION_SET_TYPE_COUNT ; ++i )
			WideCharToMultiByte( code_page, 0, emprs.permission_name[ i ], -1, szPermissionName[ i ], _countof( szPermissionName[ i ] ), NULL, NULL );

		struct tm tmAdvertiseEnd;
		tmAdvertiseEnd.tm_year = emprs.tAdvertiseEnd.year - 1900;
		tmAdvertiseEnd.tm_mon = emprs.tAdvertiseEnd.month - 1;
		tmAdvertiseEnd.tm_mday = emprs.tAdvertiseEnd.day;
		tmAdvertiseEnd.tm_hour = emprs.tAdvertiseEnd.hour;
		tmAdvertiseEnd.tm_min = emprs.tAdvertiseEnd.minute;
		tmAdvertiseEnd.tm_sec = emprs.tAdvertiseEnd.second;
		tmAdvertiseEnd.tm_isdst = -1;
		time_t tAdvertiseEnd = mktime( &tmAdvertiseEnd );

		// 길드 생성
		GuildInfo *pInfo = makeGuild( emprs.guild_sid, szGuildName, szGuildNotice, szGuildURL, emprs.szIconFileName, emprs.icon_size,
			emprs.szBannerFileName, emprs.nBannerSize, static_cast< GuildManager::_ADVERTISE_TYPE >( emprs.nAdvertiseType ), szAdvertiseComment, tAdvertiseEnd,
			emprs.bRecruiting, emprs.nMinRecruitLevel, emprs.nMaxRecruitLevel, emprs.name_changed,
			emprs.dungeon_id, emprs.dungeon_block_time, emprs.gold, emprs.chaos, emprs.alliance_block_time, emprs.donation_point,
			szPermissionName[ 0 ], emprs.permission_set[ 0 ], szPermissionName[ 1 ], emprs.permission_set[ 1 ],
			szPermissionName[ 2 ], emprs.permission_set[ 2 ], szPermissionName[ 3 ], emprs.permission_set[ 3 ],
			szPermissionName[ 4 ], emprs.permission_set[ 4 ], szPermissionName[ 5 ], emprs.permission_set[ 5 ],
			emprs.grade, emprs.point);
		if( pInfo == NULL )
		{
			FILELOG( "ERROR: makeGuild failed, GuildID: %d, GuildName: %s", emprs.guild_sid, szGuildName );
			_cprint( "ERROR: makeGuild failed, GuildID: %d, GuildName: %s\n", emprs.guild_sid, szGuildName );

			pRstGuild->MoveNext();
			continue;
		}

		// 멤버 정보 얻어온다
		int leader_index = _getGuildMemberTagList( emprs.guild_sid, pInfo->vMemberNameList );

		// 길드원 없다면 길드 삭제
		if( pInfo->vMemberNameList.empty() )
		{
			DestroyGuild( pInfo->nGuildId );
			pRstGuild->MoveNext();
			continue;
		}

		// 길마가 없는 길드는 첫 번째 멤버를 그냥 길마로 변경
		if( leader_index == -1 )
		{
			leader_index = 0;
			bNeedToUpdateGuildInfo = true;
		}

		pInfo->nLeaderSID = pInfo->vMemberNameList[ leader_index ].sid;
		pInfo->nLeaderLevel = pInfo->vMemberNameList[ leader_index ].nLevel;

		if( emprs.alliance_id )
		{
			AllianceInfo * pAllianceInfo = getAllianceInfo( emprs.alliance_id );

			if( !pAllianceInfo )
			{
				Push( new DB_SetGuildAllianceID( emprs.guild_sid, 0 ) );
			}
			else
			{
				pAllianceInfo->vGuildList.push_back( pInfo );
			}

			pInfo->pAllianceInfo = pAllianceInfo;
		}

		if( bNeedToUpdateGuildInfo )
		{
			Push( new DB_SetGuildLeader( pInfo->nGuildId, pInfo->nLeaderSID ) );
		}

		pRstGuild->MoveNext();
		cnt++;		
	}

	for( std::vector< AllianceInfo * >::iterator it = m_vAllianceList.begin(); it != m_vAllianceList.end(); )
	{
		if( (*it)->vGuildList.size() == 0 )
		{
			Push( new DB_DeleteAllianceInfo( (*it)->nAllianceID ) );

			delete (*it);

			it = m_vAllianceList.erase( it );

			continue;
		}

		bool bLeaderFound = false;
		
		for( std::vector< GuildInfo * >::iterator itGuild = (*it)->vGuildList.begin(); itGuild != (*it)->vGuildList.end(); ++itGuild )
		{
			if( (*it)->nLeadGuildID == (*itGuild)->nGuildId )
			{
				bLeaderFound = true;
			}
		}

		if( !bLeaderFound )
		{
			for( std::vector< GuildInfo * >::iterator itGuild = (*it)->vGuildList.begin(); itGuild != (*it)->vGuildList.end(); ++itGuild )
			{
				Push( new DB_SetGuildAllianceID( (*itGuild)->nGuildId, 0 ) );

				(*itGuild)->pAllianceInfo = NULL;
			}

			Push( new DB_DeleteAllianceInfo( (*it)->nAllianceID ) );

			delete (*it);

			it = m_vAllianceList.erase( it );
		}

		++it;
	}

	picRs->Release();
	pRstGuild->Close();	

	_cprint( "Total %d Guild loaded.\n", cnt );
	FILELOG( "Total %d Guild loaded.", cnt );
	
	return;
}

void GuildManager::finishLoading()
{
	// 로딩에 사용되는 GuildMemberTag 가 남아있으면 제거
	KHash< std::vector< GuildMemberTag > *, hashPr_mod_int >::node *pValueNode = NULL;
	bool bQuitFlag = m_hshGuildMemberTagList.get_first_node( pValueNode );
	while( bQuitFlag )
	{
		_cprint( "Guild info does not exist: guild_id(%d)\n", pValueNode->key );
		FILELOG( "Guild info does not exist: guild_id(%d)", pValueNode->key );

		pValueNode->value->clear();
		delete pValueNode->value;

		bQuitFlag = m_hshGuildMemberTagList.get_next_node( pValueNode );
	}

	m_hshGuildMemberTagList.clear();
}
