
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbDeathmatch : public CADORecordBinding
{
	int id;
	int deathmatch_type;
	int min_level;
	int max_level;
	int x1;
	int y1;
	int x2;
	int y2;
	int x3;
	int y3;
	int x4;
	int y4;
	int x5;
	int y5;

BEGIN_ADO_BINDING(dbDeathmatch)
	ADO_VARIABLE_LENGTH_ENTRY4(  1, adInteger, id,				sizeof( id ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  2, adInteger, deathmatch_type,	sizeof( deathmatch_type ),	FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  3, adInteger, min_level,		sizeof( min_level ),		FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  4, adInteger, max_level,		sizeof( max_level ),		FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  5, adInteger, x1,				sizeof( x1 ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  6, adInteger, y1,				sizeof( y1 ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  7, adInteger, x2,				sizeof( x2 ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  8, adInteger, y2,				sizeof( y2 ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  9, adInteger, x3,				sizeof( x3 ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, y3,				sizeof( y3 ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 11, adInteger, x4,				sizeof( x4 ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 12, adInteger, y4,				sizeof( y4 ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 13, adInteger, x5,				sizeof( x5 ),				FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 14, adInteger, y5,				sizeof( y5 ),				FALSE )
END_ADO_BINDING()
};

void onDeathmatchData( dbDeathmatch * emprs )
{
	// 데스매치 정보 등록
	ArPosition position[5] = {	ArPosition( emprs->x1, emprs->y1 ),
								ArPosition( emprs->x2, emprs->y2 ),
								ArPosition( emprs->x3, emprs->y3 ),
								ArPosition( emprs->x4, emprs->y4 ),
								ArPosition( emprs->x5, emprs->y5 ) };

	GameContent::RegisterDeathmatchInfo( emprs->id, emprs->deathmatch_type, emprs->min_level, emprs->max_level, position);
}

static bool LoadDeathmatchResource()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t nCnt = LoadDbResource< dbDeathmatch >( "DeathmatchResource", ConnPtr, onDeathmatchData );

	_cprint( "Total %d deathmatch info loading complete...\n", nCnt );
	FILELOG( "Total %d deathmatch info loading complete...", nCnt );

	return true;
}

bool DeathmatchLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadDeathmatchResource();
		helper.end( "DeathmatchLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "DeathmatchLoader" );
		LogDBError( e, "DeathmatchLoader", "LoadDeathmatchResource()" );

		std::string strError = "DEATHMATCH RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}
