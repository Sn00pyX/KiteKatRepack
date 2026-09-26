
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "FieldPropBase.h"
#include "FieldPropManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbFieldProp : public CADORecordBinding, public FieldPropBase
{
	dbFieldProp() { script_string[0] = '\0'; }

	char							limit_deva;
	char							limit_asura;
	char							limit_gaia;

	char							limit_fighter;
	char							limit_hunter;
	char							limit_magician;
	char							limit_summoner;

	char							script_string[256];

	_decimal_variant				dv_drop_percentage[2];

BEGIN_ADO_BINDING( dbFieldProp )
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, nPropId, sizeof(nPropId), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, nPropTextId, sizeof(nPropTextId), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, nType, sizeof(nType), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, nLocalFlag, sizeof(nLocalFlag), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, nCastingTime, sizeof(nCastingTime), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adInteger, nUseCount, sizeof(nUseCount), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, nRegenTime, sizeof(nRegenTime), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, nLifeTime, sizeof(nLifeTime), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(12, adInteger, nMinLevel, sizeof(nMinLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(13, adInteger, nMaxLevel, sizeof(nMaxLevel), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(14, adBoolean, limit_deva, sizeof(limit_deva), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, adBoolean, limit_asura, sizeof(limit_asura), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(16, adBoolean, limit_gaia, sizeof(limit_gaia), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(17, adBoolean, limit_fighter, sizeof(limit_fighter), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(18, adBoolean, limit_hunter, sizeof(limit_hunter), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(19, adBoolean, limit_magician, sizeof(limit_magician), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(20, adBoolean, limit_summoner, sizeof(limit_summoner), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(21, adInteger, nLimitJobId, sizeof(nLimitJobId), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(22, adInteger, nActivateId[0], sizeof(nActivateId[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(23, adInteger, nActivateValue[0][0], sizeof(nActivateValue[0][0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(24, adInteger, nActivateValue[0][1], sizeof(nActivateValue[0][1]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(25, adInteger, nActivateId[1], sizeof(nActivateId[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(26, adInteger, nActivateValue[1][0], sizeof(nActivateValue[1][0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(27, adInteger, nActivateValue[1][1], sizeof(nActivateValue[1][1]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(28, adInteger, nActivateSkillId, sizeof(nActivateSkillId), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(29, adInteger, drop_info[0].code, sizeof(drop_info[0].code), FALSE)
	ADO_NUMERIC_ENTRY2(		   30, adDecimal, dv_drop_percentage[0],	10,8,FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(31, adInteger, drop_info[0].min_count, sizeof(drop_info[0].min_count), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(32, adInteger, drop_info[0].max_count, sizeof(drop_info[0].max_count), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(33, adInteger, drop_info[0].min_level, sizeof(drop_info[0].min_level), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(34, adInteger, drop_info[0].max_level, sizeof(drop_info[0].max_level), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(35, adInteger, drop_info[1].code, sizeof(drop_info[1].code), FALSE)
	ADO_NUMERIC_ENTRY2(		   36, adDecimal, dv_drop_percentage[1],	10,8,FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(37, adInteger, drop_info[1].min_count, sizeof(drop_info[1].min_count), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(38, adInteger, drop_info[1].max_count, sizeof(drop_info[1].max_count), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(39, adInteger, drop_info[1].min_level, sizeof(drop_info[1].min_level), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(40, adInteger, drop_info[1].max_level, sizeof(drop_info[1].max_level), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(41, adVarChar, script_string, _countof(script_string), FALSE)
END_ADO_BINDING()
};

static void onFieldPropData( dbFieldProp * emprs )
{
	emprs->nCastingTime *= 100;
	emprs->nRegenTime *= 100;
	emprs->nLifeTime *= 100;

	emprs->nLimit = 0;
	if( emprs->limit_asura )	emprs->nLimit	|= FieldPropBase::LIMIT_ASURA;
	if( emprs->limit_gaia )		emprs->nLimit	|= FieldPropBase::LIMIT_GAIA;
	if( emprs->limit_deva )		emprs->nLimit	|= FieldPropBase::LIMIT_DEVA;

	if( emprs->limit_hunter )	emprs->nLimit	|= FieldPropBase::LIMIT_HUNTER;
	if( emprs->limit_fighter )	emprs->nLimit	|= FieldPropBase::LIMIT_FIGHTER;
	if( emprs->limit_magician ) emprs->nLimit	|= FieldPropBase::LIMIT_MAGICIAN;
	if( emprs->limit_summoner ) emprs->nLimit	|= FieldPropBase::LIMIT_SUMMONER;

	emprs->strScript = emprs->script_string;

	emprs->drop_info[0].ratio = emprs->dv_drop_percentage[0].getFloat() * 100000000;
	emprs->drop_info[1].ratio = emprs->dv_drop_percentage[1].getFloat() * 100000000;

	FieldPropManager::GetInstance().RegisterFieldPropBase( *emprs );
}

bool FieldPropLoader::onProcess( int nThreadNum )
{
	HRESULT  hr = S_OK;
	DBPerformanceTrackHelper helper;
	try
	{
		_ConnectionPtr		ConnPtr = NULL;

		InitContentDbConnection( ConnPtr );

		helper.start();
		size_t cnt = LoadDbResource< dbFieldProp >( "FieldPropResource", ConnPtr, onFieldPropData );
		helper.end( "FieldPropLoader" );

		_cprint( "Total %d Field Prop loaded...\n", cnt );
		FILELOG( "Total %d Field Prop loaded...", cnt );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "FieldPropLoader" );
		LogDBError( e, "FieldPropLoader", "FieldPropResource" );

		std::string strError = "FIELD PROP RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}
