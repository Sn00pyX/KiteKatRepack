
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbCreatureFarm : public CADORecordBinding
{
	char rate;
	char form;
	char enhance_level;
	char ticket_count;

BEGIN_ADO_BINDING(dbCreatureFarm)
	ADO_VARIABLE_LENGTH_ENTRY4(	1,	adTinyInt,	rate,			sizeof( rate ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	2,	adTinyInt,	form,			sizeof( form ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	3,	adTinyInt,	enhance_level,	sizeof( enhance_level ),	FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	4,	adTinyInt,	ticket_count,	sizeof( ticket_count ),		FALSE )
END_ADO_BINDING()
};


void onCreatureFarmData( dbCreatureFarm * emprs )
{
	// 크리처 강화 정보 등록
	GameContent::RegisterCreatureFarmInfo( emprs->rate, emprs->form, emprs->enhance_level, emprs->ticket_count );
}

static bool LoadCreatureFarmResource()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t nCnt = LoadDbResource< dbCreatureFarm >( "CreatureFarmResource", ConnPtr, onCreatureFarmData );

	_cprint( "Total %d creature farm info loading complete...\n", nCnt );
	FILELOG( "Total %d creature farm info loading complete...", nCnt );

	return true;
}

bool CreatureFarmLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadCreatureFarmResource();
		helper.end( "CreatureFarmLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "CreatureFarmLoader" );
		LogDBError( e, "CreatureFarmLoader", "LoadCreatureFarmResource()" );

		std::string strError = "CREATURE FARM RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}