#include <oledb.h>

#include <logging/FileLog.h>
#include <toolkit/XConsole.h>

#include "ContentLoader.h"
#include "RoamingManager.h"
#include "GameDBUtil.h"



struct dbRoaming : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbRoaming)

	ADO_VARIABLE_LENGTH_ENTRY4(	1, adInteger, id, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	2, adInteger, roaming_type, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	3, adInteger, move_speed, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	4, adInteger, hate_type, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	5, adInteger, respawn_interval, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	6, adInteger, special_ability, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	7, adInteger, x, 4, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	8, adInteger, y, 4, FALSE )
   
END_ADO_BINDING()

	int		id;
	int		roaming_type;
	int		move_speed;
	int		hate_type;
	int		respawn_interval;
	int		special_ability;
	int		x;
	int		y;
};

struct dbRoamingPoint : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbRoamingPoint)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, roaming_id, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, point_id, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, x, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, y, 4, FALSE)
   
END_ADO_BINDING()

	int		roaming_id;
	int		point_id;
	int		x;
	int		y;
};

static void onRoamingInfo( dbRoaming * emprs )
{
	RoamingManager::Instance().RegisterRoamingInfo( emprs->id, static_cast< StructRoamer::ROAMING_TYPE >( emprs->roaming_type ), emprs->move_speed / 7, static_cast< StructRoamer::HATE_TYPE >( emprs->hate_type ), emprs->respawn_interval, emprs->special_ability, emprs->x, emprs->y );
}

static void onRoamingPointInfo( dbRoamingPoint * emprs )
{
	// point_id는 0부터 차례대로 들어와야 함을 확인
	//assert( emprs->point_id == RoamingManager::Instance().GetRoamingPointInfoCount( emprs->roaming_id ) );

	RoamingManager::Instance().RegisterRoamingPointInfo( emprs->roaming_id, emprs->x, emprs->y );
}

bool RoamingLoader::onProcess( int nThreadNum )
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t nRoamingCnt = LoadDbResource< dbRoaming >( "SELECT R.id, R.roaming_type, R.move_speed, R.hate_type, R.respawn_interval, R.special_ability, P.x, P.y FROM RoamingResource R, RoamingPointResource P WHERE R.id = P.roaming_id AND P.point_id = 0 ORDER BY R.id", ConnPtr, onRoamingInfo, adCmdText );
	size_t nRoamingPointCnt = LoadDbResource< dbRoamingPoint >( "SELECT * FROM RoamingPointResource ORDER BY roaming_id, point_id", ConnPtr, onRoamingPointInfo, adCmdText );
	size_t nLoadedRoamingCnt = RoamingManager::Instance().GetRoamingInfoCount();
	size_t nLoadedRoamingPointCnt = RoamingManager::Instance().GetRoamingPointInfoCount();

	_cprint( "Total %d/%d RoamingResource, %d/%d RoamingPointResource loaded.\n", nLoadedRoamingCnt, nRoamingCnt, nLoadedRoamingPointCnt, nRoamingPointCnt );
	FILELOG( "Total %d/%d RoamingResource, %d/%d RoamingPointResource loaded.", nLoadedRoamingCnt, nRoamingCnt, nLoadedRoamingPointCnt, nRoamingPointCnt );

	return true;
}
