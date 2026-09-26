
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "LocationLoader.h"
#include "StructWorldLocation.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbLocation : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbLocation)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, idx, sizeof(idx), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adTinyInt, location_type, sizeof(location_type), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, adInteger, time_idx, sizeof(time_idx), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, adInteger, weather_id, sizeof(weather_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, adTinyInt, weather_change_time, sizeof(weather_change_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, adTinyInt, weather_ratio, sizeof(weather_ratio), FALSE)
   
END_ADO_BINDING()

	unsigned int idx;
	unsigned char location_type;
	unsigned int time_idx;
	unsigned int weather_id;
	unsigned char weather_change_time;
	unsigned char weather_ratio;
	int shovelable_item;
};

bool LocationLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		loadLocationList();
		helper.end( "LocationLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "LocationLoader" );
		LogDBError( e, "LocationLoader", "loadLocationList()" );

		std::string strError = "WORLD LOCATION RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	if( !WorldLocationManager::Instance().Init() )
	{
		FILELOG( "location/weather info load error!" );
		_cprint( "location/weather info load error!\n" );
		return false;
	}

	return true;
}

void LocationLoader::loadLocationList()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbLocation >( "WorldLocation", ConnPtr, onLocationInfo );

	_cprint( "Total %d Location info loaded...\n", cnt );
	FILELOG( "Total %d Location info loaded...", cnt );

}

void LocationLoader::onLocationInfo( dbLocation * emprs )
{
	WorldLocationManager::Instance().RegisterWorldLocation( emprs->idx, emprs->location_type, emprs->time_idx, emprs->weather_id, emprs->weather_ratio, emprs->weather_change_time * 6000, /*emprs->shovelable_item*/0 );
}
