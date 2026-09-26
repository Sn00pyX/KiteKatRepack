
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "DungeonManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbDungeon : public CADORecordBinding
{
	int id;
	int local_flag;
	int type;
	int dungeon_level;

	int raid_start_x;
	int raid_start_y;
	int siege_start_x;
	int siege_start_y;
	int siege_defence_x;
	int siege_defence_y;

	int connector_id;
	int connector_x;			// connector_pos_x
	int connector_y;			// connector_pos_y
	int connector_direction;

	int core_id;
	int core_x;					// core_pos_x
	int core_y;					// core_pos_y
	float core_offset_z;
	float core_around_x;
	float core_around_y;
	float core_around_z;
	float core_scale_x;
	float core_scale_y;
	float core_scale_z;

	bool core_is_height_locked;
	float core_lock_height;

	int boss01_id;
	int boss02_id;

	int raid_start_time;		// raid_opening_time
	int raid_end_time;			// raid_closing_time
	int start_time;				// siege_opening_time
	int end_time;				// siege_closing_time

	int map_x;					// seamless_x
	int map_y;					// seamless_y

	int num_party_guild;
	int num_party_raid;

	_decimal_variant			dv_core_offset_z;
	_decimal_variant			dv_core_around_x;
	_decimal_variant			dv_core_around_y;
	_decimal_variant			dv_core_around_z;
	_decimal_variant			dv_core_scale_x;
	_decimal_variant			dv_core_scale_y;
	_decimal_variant			dv_core_scale_z;

	_decimal_variant			dv_core_lock_height;

BEGIN_ADO_BINDING(dbDungeon)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, id, sizeof(id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, local_flag, sizeof(local_flag), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, type, sizeof(type), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, dungeon_level, sizeof(dungeon_level), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, raid_start_x, sizeof(raid_start_x), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, raid_start_y, sizeof(raid_start_y), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adInteger, siege_start_x, sizeof(siege_start_x), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, siege_start_y, sizeof(siege_start_y), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, siege_defence_x, sizeof(siege_defence_x), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, adInteger, siege_defence_y, sizeof(siege_defence_y), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(11, adInteger, connector_id, sizeof(connector_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, adInteger, connector_x, sizeof(connector_x), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(13, adInteger, connector_y, sizeof(connector_y), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, adInteger, connector_direction, sizeof(connector_direction), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(15, adInteger, core_id, sizeof(core_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(16, adInteger, core_x, sizeof(core_x), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(17, adInteger, core_y, sizeof(core_y), FALSE)
	ADO_NUMERIC_ENTRY2(		   20, adDecimal, dv_core_offset_z, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(		   21, adDecimal, dv_core_around_x, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(		   22, adDecimal, dv_core_around_y, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(		   23, adDecimal, dv_core_around_z, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(		   24, adDecimal, dv_core_scale_x, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(		   25, adDecimal, dv_core_scale_y, 10, 8, FALSE)
	ADO_NUMERIC_ENTRY2(		   26, adDecimal, dv_core_scale_z, 10, 8, FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(27, adChar, core_is_height_locked, sizeof(core_is_height_locked), FALSE)
	ADO_NUMERIC_ENTRY2(		   28, adDecimal, dv_core_lock_height, 10, 8, FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(29, adInteger, boss01_id, sizeof(boss01_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(30, adInteger, boss02_id, sizeof(boss02_id), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(31, adInteger, raid_start_time, sizeof(raid_start_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(32, adInteger, raid_end_time, sizeof(raid_end_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(33, adInteger, start_time, sizeof(start_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(34, adInteger, end_time, sizeof(end_time), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(35, adInteger, map_x, sizeof(map_x), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(36, adInteger, map_y, sizeof(map_y), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(37, adInteger, num_party_guild, sizeof(num_party_guild), FALSE)
	// 38번 컬럼 용병 파티 관련 내용으로 사용하지 않음
	ADO_VARIABLE_LENGTH_ENTRY4(39, adInteger, num_party_raid, sizeof(num_party_raid), FALSE)
END_ADO_BINDING()
};

struct dbTacticalPosition : public CADORecordBinding
{
	int dungeon_id;
	int id;
	int x;
	int y;
	int prop_id;

BEGIN_ADO_BINDING(dbTacticalPosition)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, dungeon_id, sizeof(dungeon_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, id, sizeof(id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, x, sizeof(x), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, y, sizeof(y), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, prop_id, sizeof(prop_id), FALSE)
END_ADO_BINDING()
};

void onTacticalPositionData( dbTacticalPosition * emprs )
{
#ifdef _DEBUG
	static int nPrevDungeonID = emprs->dungeon_id;
	static int nPrevTacticalPositionID = 0;

	if( nPrevDungeonID != emprs->dungeon_id )
	{
		nPrevDungeonID = emprs->dungeon_id;
		nPrevTacticalPositionID = 0;
	}

	assert( ++nPrevTacticalPositionID == emprs->id );
#endif

	DungeonManager::Instance().RegisterTacticalPositionInfo( emprs->dungeon_id, emprs->id, emprs->x, emprs->y, emprs->prop_id );
}

void onDungeonData( dbDungeon * emprs )
{
	extern float g_fMapLength;

	X2D::Box< AR_UNIT > box( X2D::Point< AR_UNIT >( emprs->map_x * g_fMapLength, emprs->map_y * g_fMapLength ),
		X2D::Point< AR_UNIT >( ( emprs->map_x + 1 ) * g_fMapLength, ( emprs->map_y + 1 ) * g_fMapLength ) );

	// 해당 국가에 맞는 던전 데이터만 로딩해야 함
	extern volatile int g_nCurrentLocalFlag;
	if( ( g_nCurrentLocalFlag & emprs->local_flag ) != g_nCurrentLocalFlag )
		return;

	int owner_guild_id = 0;
	int raid_guild_id = 0;
	int best_raid_time = 0;
	int last_dungeon_siege_finish_time = 0;
	int last_dungeon_raid_wrap_up_time = 0;
	int tax_rate = 0;

	// 던전 타입이 시즈일 경우에만 던전 소유 정보 로딩
	if( emprs->type == DungeonManager::DUNGEON_TYPE_SIEGE )
	{
		DBConnection db;
		InitUserDbConnection( db.connection );
		if( db.CreateCommand( db.command ) == false )	throw XException( "onDungeonData : CreateInstance(command) error" );

		db.command->CommandType = adCmdStoredProc;
		db.command->CommandText = _bstr_t( "dbo.smp_read_dungeon" );

		db.command->Parameters->Append( db.command->CreateParameter( "IN_DUNGEON_ID", adInteger, adParamInput, 4, emprs->id  ) );

		_RecordsetPtr pRS = db.command->Execute(NULL,NULL,adCmdStoredProc);

		if( pRS->State == adStateClosed || pRS->EndOfFile )
		{
			_cprint( "!!!!Dungeon info loading Error!!!! -- No Instance Data --\n" );
			FILELOG( "!!!!Dungeon info loading Error!!!! -- No Instance Data --" );
			return;
		}
		owner_guild_id = pRS->Fields->Item["owner_guild_id"]->Value;
		raid_guild_id = pRS->Fields->Item["raid_guild_id"]->Value;
		best_raid_time = pRS->Fields->Item["best_raid_time"]->Value;
		last_dungeon_siege_finish_time = pRS->Fields->Item["last_dungeon_siege_finish_time"]->Value;
		last_dungeon_raid_wrap_up_time = pRS->Fields->Item["last_dungeon_raid_wrap_up_time"]->Value;
		tax_rate = pRS->Fields->Item["tax_rate"]->Value;
	}

	emprs->core_offset_z = emprs->dv_core_offset_z.getFloat();
	emprs->core_around_x = emprs->dv_core_around_x.getFloat();
	emprs->core_around_y = emprs->dv_core_around_y.getFloat();
	emprs->core_around_z = emprs->dv_core_around_z.getFloat();
	emprs->core_scale_x = emprs->dv_core_scale_x.getFloat();
	emprs->core_scale_y = emprs->dv_core_scale_y.getFloat();
	emprs->core_scale_z = emprs->dv_core_scale_z.getFloat();
	emprs->core_lock_height = emprs->dv_core_lock_height.getFloat();

	DungeonManager::Instance().RegisterDungeonInfo( emprs->id, emprs->dungeon_level, emprs->type,
		emprs->raid_start_x, emprs->raid_start_y, emprs->siege_start_x, emprs->siege_start_y, emprs->siege_defence_x, emprs->siege_defence_y,
		emprs->connector_id, emprs->connector_x, emprs->connector_y, emprs->core_id, emprs->core_x, emprs->core_y,
		emprs->core_offset_z, emprs->core_around_x, emprs->core_around_y, emprs->core_around_z, emprs->core_scale_x,
		emprs->core_scale_y, emprs->core_scale_z, emprs->core_is_height_locked, emprs->core_lock_height,
		emprs->boss01_id, emprs->boss02_id, emprs->raid_start_time, emprs->raid_end_time, emprs->start_time, emprs->end_time,
		box, owner_guild_id, raid_guild_id, best_raid_time, last_dungeon_siege_finish_time, last_dungeon_raid_wrap_up_time,
		tax_rate, emprs->num_party_guild, emprs->num_party_raid );
}

void DungeonManager::LoadRaidInfo( int dungeon_id, int last_dungeon_siege_time )
{
	DBConnection db;
	InitUserDbConnection( db.connection );
	if( db.CreateCommand( db.command ) == false )	throw XException( "LoadRaidInfo : CreateInstance(command) error" );

	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_dungeon_raid_time" );
	
	db.command->Parameters->Append( db.command->CreateParameter( "IN_DUNGEON_ID", adInteger, adParamInput, 4, dungeon_id ) );
	db.command->Parameters->Append( db.command->CreateParameter( "IN_LAST_DUNGEON_SIEGE_TIME", adInteger, adParamInput, 4, last_dungeon_siege_time ) );

	_RecordsetPtr pRS = db.command->Execute(NULL,NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		int guild_id = pRS->Fields->Item["guild_id"]->Value;
		int record = pRS->Fields->Item["record"]->Value;
		int raid_end_time = pRS->Fields->Item["raid_time"]->Value;

		RegisterDungeonRaidInfo( dungeon_id, guild_id, record, raid_end_time );

		pRS->MoveNext();
	}
}

static bool LoadTacticalPositionData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	LoadDbResource< dbTacticalPosition >( "SELECT * FROM dbo.TacticalPositionResource ORDER BY dungeon_id, tactical_pos_id", ConnPtr, onTacticalPositionData, adCmdText );

	_cprint( "Dungeon Tactical Position info loading complete...\n" );
	FILELOG( "Dungeon Tactical Position info loading complete..." );

	return true;
}

static bool LoadDungeonData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	LoadDbResource< dbDungeon >( "DungeonResource", ConnPtr, onDungeonData );

	DungeonManager::Instance().LoadAllRaidInfo();

	_cprint( "Dungeon info loading complete...\n" );
	FILELOG( "Dungeon info loading complete..." );

	return true;
}

bool DungeonLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadDungeonData();
		LoadTacticalPositionData();
		helper.end( "DungeonLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "DungeonLoader" );
		LogDBError( e, "DungeonLoader", "DungeonLoader::onProcess()" );

		std::string strError = "DUNGEON RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}
