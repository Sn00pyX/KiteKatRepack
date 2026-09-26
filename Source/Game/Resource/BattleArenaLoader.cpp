
#include <logging/FileLog.h>
#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "ContentLoader.h"
#include "BattleArenaManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"


std::vector< BattleArenaBaseServer * > & GetBattleArenaVector()
{
	static std::vector< BattleArenaBaseServer * > vBattleArenaInfo;
	return vBattleArenaInfo;
}

struct BeginAreaInfo
{
	BeginAreaInfo( int _nArenaID )
		: nArenaID( _nArenaID )
	{
		vPointList.assign( GameRule::BATTLE_ARENA_MAX_TEAM_COUNT, std::vector< X2D::Point< AR_UNIT > >() );
	}

	int nArenaID;
	// BattleArenaBeginAreaResource에서 로드된 점들을 모아놨다가 BattleArenaBaseServer::apBeginAreaList에
	// X2D::Polygon으로 변환해서 넣기 위한 버퍼
	std::vector< std::vector< X2D::Point< AR_UNIT > > >	vPointList;
};

std::vector< BeginAreaInfo > & GetBeginAreaVector()
{
	static std::vector< BeginAreaInfo > vBeginAreaInfo;
	return vBeginAreaInfo;
}

struct dbBattleArena : public CADORecordBinding, BattleArenaBaseServer
{
BEGIN_ADO_BINDING(dbBattleArena)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, nID, sizeof( nID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, eType, sizeof( eType ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adTinyInt, nTeamCount, sizeof( nTeamCount ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adTinyInt, nMinMember, sizeof( nMinMember ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adTinyInt, nMaxMember, sizeof( nMaxMember ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, nObjectivePoint, sizeof( nObjectivePoint ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, nMinPlayDurationForReward, sizeof( nMinPlayDurationForReward ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, nArenaRegionLeft, sizeof( nArenaRegionLeft ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, nArenaRegionTop, sizeof( nArenaRegionTop ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, nArenaRegionRight, sizeof( nArenaRegionRight ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 11, adInteger, nArenaRegionBottom, sizeof( nArenaRegionBottom ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 12, adInteger, nCountdownDuration, sizeof( nCountdownDuration ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 13, adInteger, nBattleDuration, sizeof( nBattleDuration ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 14, adInteger, nQuickJoinLimitPoint, sizeof( nQuickJoinLimitPoint ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 15, adInteger, nQuickJoinLimitTime, sizeof( nQuickJoinLimitTime ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 16, adInteger, nScorePerKill, sizeof( nScorePerKill ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 17, adInteger, nAPPerKill, sizeof( nAPPerKill ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 18, adInteger, nMaxAPByKill, sizeof( nMaxAPByKill ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 19, adInteger, nAPPerPropActivate, sizeof( nAPPerPropActivate ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 20, adInteger, nMaxAPByPropActivate, sizeof( nMaxAPByPropActivate ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 21, adInteger, nRewardAPWin[ 0 ], sizeof( nRewardAPWin[ 0 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 22, adInteger, nRewardAPWin[ 1 ], sizeof( nRewardAPWin[ 1 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 23, adInteger, nRewardAPWin[ 2 ], sizeof( nRewardAPWin[ 2 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 24, adInteger, nRewardAPDraw[ 0 ], sizeof( nRewardAPDraw[ 0 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 25, adInteger, nRewardAPDraw[ 1 ], sizeof( nRewardAPDraw[ 1 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 26, adInteger, nRewardAPDraw[ 2 ], sizeof( nRewardAPDraw[ 2 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 27, adInteger, nRewardAPLose[ 0 ], sizeof( nRewardAPLose[ 0 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 28, adInteger, nRewardAPLose[ 1 ], sizeof( nRewardAPLose[ 1 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 29, adInteger, nRewardAPLose[ 2 ], sizeof( nRewardAPLose[ 2 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 30, adInteger, nMinAPForMVP, sizeof( nMinAPForMVP ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 31, adInteger, nStartPos1X, sizeof( nStartPos1X ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 32, adInteger, nStartPos1Y, sizeof( nStartPos1Y ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 33, adInteger, nStartPos2X, sizeof( nStartPos2X ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 34, adInteger, nStartPos2Y, sizeof( nStartPos2Y ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 35, adVarChar, szScriptOnCreate, _countof( szScriptOnCreate ) - 1, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 36, adVarChar, szScriptOnEnter, _countof( szScriptOnEnter ) - 1, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 37, adVarChar, szScriptOnStart, _countof( szScriptOnStart ) - 1, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 38, adVarChar, szScriptOnEnd, _countof( szScriptOnEnd ) - 1, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 39, adVarChar, szScriptOnLeave, _countof( szScriptOnLeave ) - 1, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 40, adVarChar, szScriptOnDestroy, _countof( szScriptOnDestroy ) - 1, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 41, adVarChar, szScriptOnKill, _countof( szScriptOnKill ) - 1, FALSE )
END_ADO_BINDING()

	int nStartPos1X, nStartPos1Y;
	int nStartPos2X, nStartPos2Y;

	char szScriptOnCreate[ 256 ];
	char szScriptOnEnter[ 256 ];
	char szScriptOnStart[ 256 ];
	char szScriptOnEnd[ 256 ];
	char szScriptOnLeave[ 256 ];
	char szScriptOnDestroy[ 256 ];
	char szScriptOnKill[ 256 ];
};

struct dbBattleArenaFieldPropRespawn : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbBattleArenaFieldPropRespawn)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, arena_id, sizeof(arena_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, respawn_id, sizeof(respawn_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, local_flag, sizeof(local_flag), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adBoolean, is_blocker, sizeof(is_blocker), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, prop_id, sizeof(prop_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, x, sizeof(x), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, y, sizeof(y), FALSE )
	ADO_NUMERIC_ENTRY2( 8, adDecimal, offset_x, 10, 8, FALSE )
	ADO_NUMERIC_ENTRY2( 9, adDecimal, offset_y, 10, 8, FALSE )
	ADO_NUMERIC_ENTRY2( 10, adDecimal, offset_z, 10, 8, FALSE )
	ADO_NUMERIC_ENTRY2( 11, adDecimal, around_x, 10, 8, FALSE )
	ADO_NUMERIC_ENTRY2( 12, adDecimal, around_y, 10, 8, FALSE )
	ADO_NUMERIC_ENTRY2( 13, adDecimal, around_z, 10, 8, FALSE )
	ADO_NUMERIC_ENTRY2( 14, adDecimal, scale_x, 10, 8, FALSE )
	ADO_NUMERIC_ENTRY2( 15, adDecimal, scale_y, 10, 8, FALSE )
	ADO_NUMERIC_ENTRY2( 16, adDecimal, scale_z, 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 17, adBoolean, is_height_locked, sizeof(is_height_locked), FALSE )
	ADO_NUMERIC_ENTRY2( 18, adDecimal, lock_height, 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 19, adInteger, prop_index_in_arena, sizeof(prop_index_in_arena), FALSE )
END_ADO_BINDING()

	int arena_id;
	int respawn_id;
	int local_flag;
	bool is_blocker;
	int prop_id;
	int x;
	int y;
	_decimal_variant offset_x;
	_decimal_variant offset_y;
	_decimal_variant offset_z;
	_decimal_variant around_x;
	_decimal_variant around_y;
	_decimal_variant around_z;
	_decimal_variant scale_x;
	_decimal_variant scale_y;
	_decimal_variant scale_z;
	bool is_height_locked;
	_decimal_variant lock_height;
	int prop_index_in_arena;
};

struct dbBattleArenaMonsterRespawn : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbBattleArenaMonsterRespawn)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, arena_id, sizeof(arena_id), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, respawn_id, sizeof(respawn_id), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, region_left, sizeof(region_left), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, region_top, sizeof(region_top), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, region_right, sizeof(region_right), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, region_bottom, sizeof(region_bottom), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, monster_id, sizeof(monster_id), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, respawn_count, sizeof(respawn_count), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, period, sizeof(period), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adBoolean, wandering, sizeof(wandering), FALSE ) 
	ADO_NUMERIC_ENTRY2( 11, adDecimal, face_direction, 10, 8, FALSE )
END_ADO_BINDING()

	int arena_id;
	int respawn_id;
	int region_left;
	int region_top;
	int region_right;
	int region_bottom;
	int monster_id;
	int respawn_count;
	int period;
	bool wandering;
	_decimal_variant face_direction;
};

struct dbBattleArenaBeginArea : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbBattleArenaBeginArea)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, arena_id, sizeof(arena_id), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, team_no, sizeof(team_no), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, point_id, sizeof(point_id), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, x, sizeof(x), FALSE ) 
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, y, sizeof(y), FALSE ) 
END_ADO_BINDING()

	int arena_id;
	int team_no;
	int point_id;
	int x;
	int y;
};


void onBattleArenaInfo( dbBattleArena * emprs )
{
	emprs->nArenaRegionLeft = GetRegionX( emprs->nArenaRegionLeft );
	emprs->nArenaRegionTop = GetRegionY( emprs->nArenaRegionTop );
	emprs->nArenaRegionRight = GetRegionX( emprs->nArenaRegionRight ) + 1;
	emprs->nArenaRegionBottom = GetRegionY( emprs->nArenaRegionBottom ) + 1;

	emprs->posStart[ 0 ].x = emprs->nStartPos1X;
	emprs->posStart[ 0 ].y = emprs->nStartPos1Y;
	emprs->posStart[ 1 ].x = emprs->nStartPos2X;
	emprs->posStart[ 1 ].y = emprs->nStartPos2Y;

	if( strlen( emprs->szScriptOnCreate ) > 0 && strcmp( emprs->szScriptOnCreate, "0" ) )
		emprs->strScriptOnCreate	= emprs->szScriptOnCreate;
	if( strlen( emprs->szScriptOnEnter ) > 0 && strcmp( emprs->szScriptOnEnter, "0" ) )
		emprs->strScriptOnEnter		= emprs->szScriptOnEnter;
	if( strlen( emprs->szScriptOnStart ) > 0 && strcmp( emprs->szScriptOnStart, "0" ) )
		emprs->strScriptOnStart		= emprs->szScriptOnStart;
	if( strlen( emprs->szScriptOnEnd ) > 0 && strcmp( emprs->szScriptOnEnd, "0" ) )
		emprs->strScriptOnEnd		= emprs->szScriptOnEnd;
	if( strlen( emprs->szScriptOnLeave ) > 0 && strcmp( emprs->szScriptOnLeave, "0" ) )
		emprs->strScriptOnLeave		= emprs->szScriptOnLeave;
	if( strlen( emprs->szScriptOnDestroy ) > 0 && strcmp( emprs->szScriptOnDestroy, "0" ) )
		emprs->strScriptOnDestroy	= emprs->szScriptOnDestroy;
	if( strlen( emprs->szScriptOnKill ) > 0 && strcmp( emprs->szScriptOnKill, "0" ) )
		emprs->strScriptOnKill		= emprs->szScriptOnKill;

	memset( const_cast< X2D::Polygon< AR_UNIT > ** >( emprs->apBeginAreaList ), 0, sizeof( emprs->apBeginAreaList ) );

	GetBattleArenaVector().push_back( new BattleArenaBaseServer( *emprs ) );
}

void onBattleArenaFieldPropInfo( dbBattleArenaFieldPropRespawn * emprs )
{
	std::vector< BattleArenaBaseServer * > & vArenaBaseList = GetBattleArenaVector();

	for( std::vector< BattleArenaBaseServer * >::iterator it = vArenaBaseList.begin() ; it != vArenaBaseList.end() ; ++it )
	{
		BattleArenaBaseServer * pArenaBase = (*it);

		if( pArenaBase->nID != emprs->arena_id )
			continue;

		// blocker 프랍은 prop_index_in_arena 가 항상 0 이어야 함
		assert( !emprs->is_blocker || !emprs->prop_index_in_arena );

		// 중복 prop_index_in_arena 체크(0은 프랍 번호 없음이기 때문에 체크 안 함)
		if( emprs->prop_index_in_arena )
		{
			for( std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >::const_iterator it = pArenaBase->vArenaBlockerFieldPropRespawnList.begin() ;
				it != pArenaBase->vArenaBlockerFieldPropRespawnList.end() ; ++it )
			{
				if( (*it)->nPropIndexInArena == emprs->prop_index_in_arena )
				{
					assert( 0 );

					_cprint( "Prop respawn data with duplicated prop index(%d) in BattleArena(%d) is detected.", emprs->prop_index_in_arena, pArenaBase->nID );
					FILELOG( "Prop respawn data with duplicated prop index(%d) in BattleArena(%d) is detected.\n", emprs->prop_index_in_arena, pArenaBase->nID );
					return;
				}
			}

			for( std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >::const_iterator it = pArenaBase->vFieldPropRespawnList.begin() ;
				it != pArenaBase->vFieldPropRespawnList.end() ; ++it )
			{
				if( (*it)->nPropIndexInArena == emprs->prop_index_in_arena )
				{
					assert( 0 );

					_cprint( "Prop respawn data with duplicated prop index(%d) in BattleArena(%d) is detected.", emprs->prop_index_in_arena, pArenaBase->nID );
					FILELOG( "Prop respawn data with duplicated prop index(%d) in BattleArena(%d) is detected.\n", emprs->prop_index_in_arena, pArenaBase->nID );
					return;
				}
			}
		}

		BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * pFieldPropResapwnInfo = new BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO(
			emprs->prop_id, emprs->x, emprs->y, emprs->offset_z.getFloat(),
			emprs->around_x.getFloat(), emprs->around_y.getFloat(), emprs->around_z.getFloat(),
			emprs->scale_x.getFloat(), emprs->scale_y.getFloat(), emprs->scale_z.getFloat(),
			emprs->is_height_locked, emprs->lock_height.getFloat(), emprs->prop_index_in_arena );

		if( emprs->is_blocker )
			pArenaBase->vArenaBlockerFieldPropRespawnList.push_back( pFieldPropResapwnInfo );
		else
			pArenaBase->vFieldPropRespawnList.push_back( pFieldPropResapwnInfo );

		return;
	}

	// emprs->arena_id 에 해당하는 BattleArenaBaseServer 가 GetBattleArenaVector() 에 등록되어 있지 않음
	assert( 0 );
}

void onBattleArenaMonsterRespawnInfo( dbBattleArenaMonsterRespawn * emprs )
{
	std::vector< BattleArenaBaseServer * > & vArenaBaseList = GetBattleArenaVector();

	for( std::vector< BattleArenaBaseServer * >::iterator it = vArenaBaseList.begin() ; it != vArenaBaseList.end() ; ++it )
	{
		BattleArenaBaseServer * pArenaBase = (*it);

		if( pArenaBase->nID != emprs->arena_id )
			continue;

		BATTLE_ARENA_MONSTER_RESPAWN * pMonsterRespawn = new BATTLE_ARENA_MONSTER_RESPAWN;
		pMonsterRespawn->nID = emprs->respawn_id;
		pMonsterRespawn->bxArea.Set( emprs->region_left, emprs->region_top, emprs->region_right, emprs->region_bottom );
		pMonsterRespawn->nMonsterID = emprs->monster_id;
		pMonsterRespawn->nCount = emprs->respawn_count;
		pMonsterRespawn->nPeriod = emprs->period;
		pMonsterRespawn->bWandering = emprs->wandering;
		pMonsterRespawn->fFace = emprs->face_direction.getFloat();

		pArenaBase->vMonsterRespawnList.push_back( pMonsterRespawn );

		return;
	}

	// emprs->arena_id 에 해당하는 BattleArenaBaseServer 가 GetBattleArenaVector() 에 등록되어 있지 않음
	assert( 0 );
}

void onBattleArenaBeginAreaInfo( dbBattleArenaBeginArea * emprs )
{
	std::vector< BeginAreaInfo > & vBeginAreaList = GetBeginAreaVector();

	BeginAreaInfo * pBeginAreaInfo = NULL;

	for( std::vector< BeginAreaInfo >::iterator it = vBeginAreaList.begin() ; it != vBeginAreaList.end() ; ++it )
	{
		BeginAreaInfo & beginAreaInfo = (*it);

		if( beginAreaInfo.nArenaID != emprs->arena_id )
			continue;

		pBeginAreaInfo = &beginAreaInfo;
		break;
	}

	// 새로운 arena_id 에 대한 BeginAreaInfo가 로드된 경우 새로운 BeginAreaInfo 추가
	if( !pBeginAreaInfo )
	{
		GetBeginAreaVector().push_back( BeginAreaInfo( emprs->arena_id ) );
		pBeginAreaInfo = &GetBeginAreaVector().back();
	}

	// DB 상에서 로드된 team_no는 1-based index 이므로 1 뺀 값을 BeginAreaInfo::vPointList의 인덱스로 사용
	int nTeamNo = emprs->team_no - 1;
	if( nTeamNo < 0 || nTeamNo >= pBeginAreaInfo->vPointList.size() )
	{
		// 잘못된 team_no가 DB 상에 입력되었음
		assert( 0 );
		return;
	}

	std::vector< X2D::Point< AR_UNIT > > & vPointList = pBeginAreaInfo->vPointList[ nTeamNo ];
	vPointList.push_back( X2D::Point< AR_UNIT >( emprs->x, emprs->y ) );
}

bool BattleArenaLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		_ConnectionPtr ContentDBConnPtr = NULL;
		InitContentDbConnection( ContentDBConnPtr );
		_ConnectionPtr UserDBConnPtr = NULL;
		InitUserDbConnection( UserDBConnPtr );

		helper.start();
		size_t nLoadedBattleArena = LoadDbResource< dbBattleArena >( "BattleArenaResource", ContentDBConnPtr, onBattleArenaInfo );
		size_t nLoadedBattleArenaFieldProp = LoadDbResource< dbBattleArenaFieldPropRespawn >( "BattleArenaFieldPropResource", ContentDBConnPtr, onBattleArenaFieldPropInfo );
		size_t nLoadedBattleArenaMonster = LoadDbResource< dbBattleArenaMonsterRespawn >( "BattleArenaMonsterRespawnResource", ContentDBConnPtr, onBattleArenaMonsterRespawnInfo );
		size_t nLoadedBattleArenaBeginAreaPoint = LoadDbResource< dbBattleArenaBeginArea >( "BattleArenaBeginAreaResource", ContentDBConnPtr, onBattleArenaBeginAreaInfo );
		helper.end( "BattleArenaLoader" );

		size_t nLoadedBattleArenaBeginArea = 0;
		for( std::vector< BattleArenaBaseServer * >::iterator itBase = GetBattleArenaVector().begin() ; itBase != GetBattleArenaVector().end() ; ++itBase )
		{
			BattleArenaBaseServer * pArenaBase = (*itBase);

			// BeginAreaInfo 구조체에 로드된 BeginArea 정보를 X2D::Polygon으로 변환해서 BattleArenaBaseServer::apBeginAreaList 에 넣어줌
			size_t nFoundBeginAreaCount = 0;
			for( std::vector< BeginAreaInfo >::iterator it = GetBeginAreaVector().begin() ; it != GetBeginAreaVector().end() ; ++it )
			{
				const BeginAreaInfo & beginAreaInfo = (*it);

				if( beginAreaInfo.nArenaID != pArenaBase->nID )
					continue;

				for( int nTeamNo = 0 ; nTeamNo < pArenaBase->nTeamCount ; ++nTeamNo )
				{
					const std::vector< X2D::Point< AR_UNIT > > & vPointList = beginAreaInfo.vPointList[ nTeamNo ];

					// 유효한 팀 번호지만 시작 영역 데이터가 없는 경우
					if( vPointList.empty() )
					{
						assert( 0 );
						continue;
					}

					X2D::Polygon< AR_UNIT > * pPolygon = new X2D::Polygon< AR_UNIT >();
					if( !pPolygon->Set( vPointList.begin(), vPointList.end() ) )
					{
						assert( 0 );
						nLoadedBattleArenaBeginAreaPoint -= vPointList.size();
						delete pPolygon;
						continue;
					}

					if( pPolygon->IsClockWise() )
					{
						assert( 0 );
						pPolygon->Reverse();
					}

					const_cast< X2D::Polygon< AR_UNIT > *& >( pArenaBase->apBeginAreaList[ nTeamNo ] ) = pPolygon;

					++nFoundBeginAreaCount;
				}
				// 유효하지 않은 팀에 대한 시작 영역 데이터가 DB 상에 입력되었는지 체크(이건 안 해도 게임 돌아가는데 지장 없음)
				for( int nTeamNo = pArenaBase->nTeamCount ; nTeamNo < GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ; ++nTeamNo )
				{
					if( !beginAreaInfo.vPointList[ nTeamNo ].empty() )
					{
						assert( 0 );
						break;
					}
				}

				nLoadedBattleArenaBeginArea += nFoundBeginAreaCount;
				break;
			}
			if( !nFoundBeginAreaCount && !pArenaBase->IsExerciseGameArena() )
			{
				// 현재 pArenaBase->nID에 해당하는 BeginAreaInfo가 아예 없는 경우
				assert( 0 );
			}

			if( BattleArenaManager::Instance().RegisterArena( pArenaBase ) != RESULT_SUCCESS )
			{
				assert( 0 );

				--nLoadedBattleArena;

				nLoadedBattleArenaFieldProp -= pArenaBase->vFieldPropRespawnList.size();
				nLoadedBattleArenaFieldProp -= pArenaBase->vArenaBlockerFieldPropRespawnList.size();
				nLoadedBattleArenaMonster -= pArenaBase->vMonsterRespawnList.size();
				nLoadedBattleArenaBeginArea -= nFoundBeginAreaCount;

				// onBattleArenaFieldPropInfo, onBattleArenaMonsterRespawnInfo 에서 로드된 내용이 들어있는
				// vFieldPropRespawnList, vArenaBlockerFieldPropRespawnList, vMonsterRespawnList 삭제
				for( std::vector< const BATTLE_ARENA_MONSTER_RESPAWN * >::const_iterator it = pArenaBase->vMonsterRespawnList.begin() ; it != pArenaBase->vMonsterRespawnList.end() ; ++it )
					delete (*it);
				pArenaBase->vMonsterRespawnList.clear();

				for( std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >::const_iterator it = pArenaBase->vFieldPropRespawnList.begin() ; it != pArenaBase->vFieldPropRespawnList.end() ; ++it )
					delete (*it);
				pArenaBase->vFieldPropRespawnList.clear();

				for( std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >::const_iterator it = pArenaBase->vArenaBlockerFieldPropRespawnList.begin() ; it != pArenaBase->vArenaBlockerFieldPropRespawnList.end() ; ++it )
					delete (*it);
				pArenaBase->vArenaBlockerFieldPropRespawnList.clear();

				for( int nTeamNo = 0 ; nTeamNo < pArenaBase->nTeamCount ; ++nTeamNo )
				{
					const X2D::Polygon< AR_UNIT > * pPolygon = pArenaBase->apBeginAreaList[ nTeamNo ];
					if( !pPolygon )
						continue;

					nLoadedBattleArenaBeginAreaPoint -= pPolygon->Size();
					delete pPolygon;
					const_cast< X2D::Polygon< AR_UNIT > *& >( pArenaBase->apBeginAreaList[ nTeamNo ] ) = NULL;
				}

				delete pArenaBase;

				continue;
			}
		}

		GetBattleArenaVector().clear();
		GetBeginAreaVector().clear();

		_cprint( "Total %d BattleArena info loaded...\n", nLoadedBattleArena );
		FILELOG( "Total %d BattleArena info loaded...", nLoadedBattleArena );
		_cprint( "Total %d BattleArena Field Prop info loaded...\n", nLoadedBattleArenaFieldProp );
		FILELOG( "Total %d BattleArena Field Prop info loaded...", nLoadedBattleArenaFieldProp );
		_cprint( "Total %d BattleArena Monster Respawn info loaded...\n", nLoadedBattleArenaMonster );
		FILELOG( "Total %d BattleArena Monster Respawn info loaded...", nLoadedBattleArenaMonster );
		_cprint( "Total %d(%dpts) BattleArena Begin Area info loaded...\n", nLoadedBattleArenaBeginArea, nLoadedBattleArenaBeginAreaPoint );
		FILELOG( "Total %d(%dpts) BattleArena Begin Area info loaded...", nLoadedBattleArenaBeginArea, nLoadedBattleArenaBeginAreaPoint );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "BattleArenaLoader" );
		LogDBError( e, "BattleArenaLoader", "BattleArenaLoader::onProcess()" );

		std::string strError = "BATTLEARENA RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}
