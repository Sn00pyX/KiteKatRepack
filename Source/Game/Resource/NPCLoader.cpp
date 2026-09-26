
#include <toolkit/XConsole.h>
#include <toolkit/Khash.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "StructNPC.h"
#include "GameDBUtil.h"
#include "GameProc.h"
#include "NPCBase.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

volatile int g_nCurrentLocalFlag = GameContent::LOCAL_INFO_AMERICA;
static std::vector< NPCEventPeriodBase > s_vNPCEventPeriodList;

struct dbNPC : public CADORecordBinding, public NPCBase
{
	DBTIMESTAMP dtBeginOfPeriod;
	DBTIMESTAMP dtEndOfPeriod;

BEGIN_ADO_BINDING(dbNPC)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, id, sizeof(id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, text_id, sizeof(text_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, name_text_id, sizeof(name_text_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, x, sizeof(x), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adInteger, y, sizeof(y), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, z, sizeof(z), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, face, sizeof(face), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, adInteger, local_flag, sizeof(local_flag), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(32, adInteger, roaming_id, sizeof(roaming_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(33, adInteger, standard_walk_speed, sizeof(standard_walk_speed), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(34, adInteger, standard_run_speed, sizeof(standard_run_speed), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(35, adInteger, walk_speed, sizeof(walk_speed), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(36, adInteger, run_speed, sizeof(run_speed), FALSE)
	
	ADO_VARIABLE_LENGTH_ENTRY4(37, adTinyInt, attackable, sizeof(attackable), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(38, adTinyInt, offensive_type, sizeof(offensive_type), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(39, adInteger, spawn_type, sizeof(spawn_type), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(40, adInteger, chase_range, sizeof(chase_range), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(41, adInteger, regen_time, sizeof(regen_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(42, adInteger, level, sizeof(level), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(43, adInteger, stat_id, sizeof(stat_id), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(44, adInteger, attack_range, sizeof(attack_range), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(45, adInteger, attack_speed_type, sizeof(attack_speed_type), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(46, adInteger, hp, sizeof(hp), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(47, adInteger, mp, sizeof(mp), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(48, adInteger, attack_point, sizeof(attack_point), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(49, adInteger, magic_point, sizeof(magic_point), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(50, adInteger, defence, sizeof(defence), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(51, adInteger, magic_defence, sizeof(magic_defence), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(52, adInteger, attack_speed, sizeof(attack_speed), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(53, adInteger, magic_speed, sizeof(magic_speed), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(54, adInteger, accuracy, sizeof(accuracy), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(55, adInteger, avoid, sizeof(avoid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(56, adInteger, magic_accuracy, sizeof(magic_accuracy), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(57, adInteger, magic_avoid, sizeof(magic_avoid), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(58, adVarChar, ai_script, _countof(ai_script), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(59, adVarChar, contact_script, _countof(contact_script), FALSE)
END_ADO_BINDING()
};

struct dbNPCEventPeriod : public CADORecordBinding, public NPCEventPeriodBase
{
	DBTIMESTAMP dtBeginOfPeriod;
	DBTIMESTAMP dtEndOfPeriod;
	int local_flag;

BEGIN_ADO_BINDING(dbNPCEventPeriod)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, id, sizeof(id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, local_flag, sizeof(local_flag), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adDBTimeStamp, dtBeginOfPeriod, sizeof(dtBeginOfPeriod), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adDBTimeStamp, dtEndOfPeriod, sizeof(dtEndOfPeriod), FALSE)

END_ADO_BINDING()

};

void onNPCEventPeriodData( dbNPCEventPeriod * emprs )
{
	if( !( emprs->local_flag & g_nCurrentLocalFlag ) )
		return;

	struct tm tmPeriod;

	tmPeriod.tm_year = emprs->dtBeginOfPeriod.year - 1900;
	tmPeriod.tm_mon = emprs->dtBeginOfPeriod.month - 1;
	tmPeriod.tm_mday = emprs->dtBeginOfPeriod.day;
	tmPeriod.tm_hour = emprs->dtBeginOfPeriod.hour;
	tmPeriod.tm_min = emprs->dtBeginOfPeriod.minute;
	tmPeriod.tm_sec = emprs->dtBeginOfPeriod.second;
	tmPeriod.tm_isdst = -1;
	emprs->begin_of_period = mktime( &tmPeriod );

	if( emprs->begin_of_period == -1 )
	{
		assert( 0 );
		return;
	}

	tmPeriod.tm_year = emprs->dtEndOfPeriod.year - 1900;
	tmPeriod.tm_mon = emprs->dtEndOfPeriod.month - 1;
	tmPeriod.tm_mday = emprs->dtEndOfPeriod.day;
	tmPeriod.tm_hour = emprs->dtEndOfPeriod.hour;
	tmPeriod.tm_min = emprs->dtEndOfPeriod.minute;
	tmPeriod.tm_sec = emprs->dtEndOfPeriod.second;
	tmPeriod.tm_isdst = -1;
	emprs->end_of_period = mktime( &tmPeriod );

	if( emprs->end_of_period == -1 )
	{
		assert( 0 );
		return;
	}

	static const int nPeriodicalNPCAdjustment = ENV().GetInt( "game.periodical_npc_adjustment" );
	if( nPeriodicalNPCAdjustment )
	{
		if( emprs->begin_of_period > nPeriodicalNPCAdjustment )
			emprs->begin_of_period -= nPeriodicalNPCAdjustment;
		else
			emprs->begin_of_period = 0;

		if( emprs->end_of_period > nPeriodicalNPCAdjustment )
			emprs->end_of_period -= nPeriodicalNPCAdjustment;
		else
			emprs->end_of_period = 0;
	}

	// 이미 없어졌어야 할 NPC면 로딩 안 함
	if( time( NULL ) >= emprs->end_of_period )
		return;

	s_vNPCEventPeriodList.push_back( *emprs );
}

void onNPCData( dbNPC * emprs )
{
	bool bFind = false;

	// s_vNPCEventPeriodList 에 등록된 NPC들은 이미 국가플래그 검증이 끝났으므로 그대로 날짜세팅만 해서 넣어준다.
	// 즉, s_vNPCEventPeriodList에 NPC가 등록 된 경우 그 ID를 가진 NPC는 기존 NPC 리소스 국가 플래그에 영향을 받지 않는다.
	std::vector< NPCEventPeriodBase >::iterator it;
	for( it = s_vNPCEventPeriodList.begin() ; it != s_vNPCEventPeriodList.end() ; ++it )
	{
		if( emprs->id == (*it).id )
		{
			emprs->begin_of_period = (*it).begin_of_period;
			emprs->end_of_period = (*it).end_of_period;
			emprs->is_periodic = true;
			s_vNPCEventPeriodList.erase( it );
			bFind = true;
			break;
		}
	}

	if( !bFind )
	{
		if( emprs->local_flag & g_nCurrentLocalFlag )
			return;

		emprs->is_periodic = false;
		emprs->begin_of_period = 0;
		emprs->end_of_period = 0;
	}

	// 본 테섭 사용 불가 플래그의 경우 항상 체크해준다. (기존 NPCResource의 플래그에 따라 NPCEventPeriodResource에서 온 NPC들도 영향을 받음.
	if( ( emprs->local_flag & GameContent::LOCAL_EXCLUDE_TEST_SERV ) && !ENV().GetInt( "game.ServiceServer" ) )
		return;
	if( ( emprs->local_flag & GameContent::LOCAL_EXCLUDE_SERVICE_SERV ) && ENV().GetInt( "game.ServiceServer" ) )
		return;
	
	GameContent::RegisterNPCInfo( *emprs );
}

static bool LoadNPCEventPeriodResource()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbNPCEventPeriod >( "NPCEventPeriodResource", ConnPtr, onNPCEventPeriodData );

	_cprint( "Total %d NPCEventPeriod data loaded...\n", cnt );
	FILELOG( "Total %d NPCEventPeriod data loaded...", cnt );

	return true;
}

static bool LoadNPCResource()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbNPC >( "NPCResource", ConnPtr, onNPCData );

	_cprint( "Total %d NPC loaded...\n", cnt );
	FILELOG( "Total %d NPC loaded...", cnt );

	if( s_vNPCEventPeriodList.size() > 0 )
	{
		_cprint( "!! %d of NPC Event Period data are unused...\n", static_cast< int >( s_vNPCEventPeriodList.size() ) );
		FILELOG( "!! %d of NPC Event Period data are unused...", static_cast< int >( s_vNPCEventPeriodList.size() ) );
	}

	return true;
}

bool NPCLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;

	try
	{
		helper.start();
		LoadNPCEventPeriodResource();
		helper.end( "NPCLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "NPCLoader" );
		LogDBError( e, "NPCLoader", "LoadNPCEventPeriodResource()" );

		std::string strError = "NPC RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	try
	{
		helper.start();
		LoadNPCResource();
		helper.end( "NPCLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "NPCLoader" );
		LogDBError( e, "NPCLoader", "LoadNPCResource()" );

		std::string strError = "NPC RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;

}
