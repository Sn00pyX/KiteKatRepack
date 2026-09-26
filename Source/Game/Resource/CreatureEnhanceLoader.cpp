
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbCreatureEnhance : public CADORecordBinding
{
	short enhance_level;
	_decimal_variant stat_amplify;
	short card_durability;
	short slot_amount;
	short jp_addition;

BEGIN_ADO_BINDING(dbCreatureEnhance)
	ADO_VARIABLE_LENGTH_ENTRY4(	1,	adSmallInt,	enhance_level,		sizeof( enhance_level ),	FALSE )
	ADO_NUMERIC_ENTRY2(			2,	adDecimal,	stat_amplify,	4,	3,							FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	3,	adSmallInt,	card_durability,	sizeof( card_durability ),	FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	4,	adSmallInt,	slot_amount,		sizeof( slot_amount ),		FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	5,	adSmallInt,	jp_addition,		sizeof( jp_addition ),		FALSE )
END_ADO_BINDING()
};


void onCreatureEnhanceData( dbCreatureEnhance * emprs )
{
	// 크리처 강화 정보 등록
	GameContent::RegisterCreatureEnhanceInfo( emprs->enhance_level, emprs->stat_amplify.getFloat(), emprs->card_durability, emprs->slot_amount, emprs->jp_addition );
}

static bool LoadCreatureEnhanceResource()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t nCnt = LoadDbResource< dbCreatureEnhance >( "CreatureEnhance", ConnPtr, onCreatureEnhanceData );

	_cprint( "Total %d creature enhance info loading complete...\n", nCnt );
	FILELOG( "Total %d creature enhance info loading complete...", nCnt );

	return true;
}

bool CreatureEnhanceLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadCreatureEnhanceResource();
		helper.end( "CreatureEnhanceLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "CreatureEnhanceLoader" );
		LogDBError( e, "CreatureEnhanceLoader", "LoadCreatureEnhanceResource()" );

		std::string strError = "CREATURE ENHANCE RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}