
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "RandomManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbAwaken : public CADORecordBinding, public _AWAKEN_INFO
{
BEGIN_ADO_BINDING(dbAwaken)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, nSid, sizeof(nSid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, nType, sizeof(nType), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, Level[0].nValue1, sizeof(Level[0].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, Level[0].nValue2, sizeof(Level[0].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, Level[1].nValue1, sizeof(Level[1].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, Level[1].nValue2, sizeof(Level[1].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, Level[2].nValue1, sizeof(Level[2].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, Level[2].nValue2, sizeof(Level[2].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, Level[3].nValue1, sizeof(Level[3].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, adInteger, Level[3].nValue2, sizeof(Level[3].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, adInteger, Level[4].nValue1, sizeof(Level[4].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, adInteger, Level[4].nValue2, sizeof(Level[4].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(13, adInteger, Level[5].nValue1, sizeof(Level[5].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, adInteger, Level[5].nValue2, sizeof(Level[5].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, adInteger, Level[6].nValue1, sizeof(Level[6].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(16, adInteger, Level[6].nValue2, sizeof(Level[6].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(17, adInteger, Level[7].nValue1, sizeof(Level[7].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(18, adInteger, Level[7].nValue2, sizeof(Level[7].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(19, adInteger, Level[8].nValue1, sizeof(Level[8].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(20, adInteger, Level[8].nValue2, sizeof(Level[8].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(21, adInteger, Level[9].nValue1, sizeof(Level[9].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(22, adInteger, Level[9].nValue2, sizeof(Level[9].nValue2), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(23, adInteger, Level[10].nValue1, sizeof(Level[10].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(24, adInteger, Level[10].nValue2, sizeof(Level[10].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(25, adInteger, Level[11].nValue1, sizeof(Level[11].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(26, adInteger, Level[11].nValue2, sizeof(Level[11].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(27, adInteger, Level[12].nValue1, sizeof(Level[12].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(28, adInteger, Level[12].nValue2, sizeof(Level[12].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(29, adInteger, Level[13].nValue1, sizeof(Level[13].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(30, adInteger, Level[13].nValue2, sizeof(Level[13].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(31, adInteger, Level[14].nValue1, sizeof(Level[14].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(32, adInteger, Level[14].nValue2, sizeof(Level[14].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(33, adInteger, Level[15].nValue1, sizeof(Level[15].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(34, adInteger, Level[15].nValue2, sizeof(Level[15].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(35, adInteger, Level[16].nValue1, sizeof(Level[16].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(36, adInteger, Level[16].nValue2, sizeof(Level[16].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(37, adInteger, Level[17].nValue1, sizeof(Level[17].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(38, adInteger, Level[17].nValue2, sizeof(Level[17].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(39, adInteger, Level[18].nValue1, sizeof(Level[18].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(40, adInteger, Level[18].nValue2, sizeof(Level[18].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(41, adInteger, Level[19].nValue1, sizeof(Level[19].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(42, adInteger, Level[19].nValue2, sizeof(Level[19].nValue2), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(43, adInteger, Level[20].nValue1, sizeof(Level[20].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(44, adInteger, Level[20].nValue2, sizeof(Level[20].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(45, adInteger, Level[21].nValue1, sizeof(Level[21].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(46, adInteger, Level[21].nValue2, sizeof(Level[21].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(47, adInteger, Level[22].nValue1, sizeof(Level[22].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(48, adInteger, Level[22].nValue2, sizeof(Level[22].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(49, adInteger, Level[23].nValue1, sizeof(Level[23].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(50, adInteger, Level[23].nValue2, sizeof(Level[23].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(51, adInteger, Level[24].nValue1, sizeof(Level[24].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(52, adInteger, Level[24].nValue2, sizeof(Level[24].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(53, adInteger, Level[25].nValue1, sizeof(Level[25].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(54, adInteger, Level[25].nValue2, sizeof(Level[25].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(55, adInteger, Level[26].nValue1, sizeof(Level[26].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(56, adInteger, Level[26].nValue2, sizeof(Level[26].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(57, adInteger, Level[27].nValue1, sizeof(Level[27].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(58, adInteger, Level[27].nValue2, sizeof(Level[27].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(59, adInteger, Level[28].nValue1, sizeof(Level[28].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(60, adInteger, Level[28].nValue2, sizeof(Level[28].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(61, adInteger, Level[29].nValue1, sizeof(Level[29].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(62, adInteger, Level[29].nValue2, sizeof(Level[29].nValue2), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(63, adInteger, Level[30].nValue1, sizeof(Level[30].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(64, adInteger, Level[30].nValue2, sizeof(Level[30].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(65, adInteger, Level[31].nValue1, sizeof(Level[31].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(66, adInteger, Level[31].nValue2, sizeof(Level[31].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(67, adInteger, Level[32].nValue1, sizeof(Level[32].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(68, adInteger, Level[32].nValue2, sizeof(Level[32].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(69, adInteger, Level[33].nValue1, sizeof(Level[33].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(70, adInteger, Level[33].nValue2, sizeof(Level[33].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(71, adInteger, Level[34].nValue1, sizeof(Level[34].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(72, adInteger, Level[34].nValue2, sizeof(Level[34].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(73, adInteger, Level[35].nValue1, sizeof(Level[35].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(74, adInteger, Level[35].nValue2, sizeof(Level[35].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(75, adInteger, Level[36].nValue1, sizeof(Level[36].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(76, adInteger, Level[36].nValue2, sizeof(Level[36].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(77, adInteger, Level[37].nValue1, sizeof(Level[37].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(78, adInteger, Level[37].nValue2, sizeof(Level[37].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(79, adInteger, Level[38].nValue1, sizeof(Level[38].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(80, adInteger, Level[38].nValue2, sizeof(Level[38].nValue2), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(81, adInteger, Level[39].nValue1, sizeof(Level[39].nValue1), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(82, adInteger, Level[39].nValue2, sizeof(Level[39].nValue2), FALSE)

END_ADO_BINDING()
};

struct dbAwakenProbabilities : public CADORecordBinding, public _AWAKEN_PROBABILITIES_INFO
{
BEGIN_ADO_BINDING(dbAwakenProbabilities)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, nSid, sizeof(nSid), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, nLevel, sizeof(nLevel), FALSE)
	ADO_NUMERIC_ENTRY2(3, adDecimal, probabilities, 10, 3, FALSE)
END_ADO_BINDING()

	_decimal_variant	probabilities;
};

struct dbRandomItemOption : public CADORecordBinding, public RandomItemOptionInfo
{
BEGIN_ADO_BINDING(dbRandomItemOption)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, optionID, sizeof(optionID), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, subID, sizeof(subID), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, optionType, sizeof(optionType), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, optionValue1, sizeof(optionValue1), FALSE)

	ADO_NUMERIC_ENTRY2(5, adDecimal, _minValue2, 10, 1, FALSE)
	ADO_NUMERIC_ENTRY2(6, adDecimal, _maxValue2, 10, 1, FALSE)
	ADO_NUMERIC_ENTRY2(7, adDecimal, _extremeRate, 4, 2, FALSE)
END_ADO_BINDING()

	_decimal_variant	_minValue2;
	_decimal_variant	_maxValue2;
	_decimal_variant	_extremeRate;
};

struct dbRandomItemPreset: public CADORecordBinding, public RandomItemPresetInfo
{
BEGIN_ADO_BINDING(dbRandomItemPreset)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, presetID, sizeof(presetID), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, optionID[0], sizeof(optionID[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, optionSucRate[0], sizeof(optionSucRate[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, optionID[1], sizeof(optionID[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, optionSucRate[1], sizeof(optionSucRate[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, optionID[2], sizeof(optionID[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adInteger, optionSucRate[2], sizeof(optionSucRate[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, optionID[3], sizeof(optionID[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, optionSucRate[3], sizeof(optionSucRate[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, adInteger, optionID[4], sizeof(optionID[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, adInteger, optionSucRate[4], sizeof(optionSucRate[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, adInteger, optionID[5], sizeof(optionID[5]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(13, adInteger, optionSucRate[5], sizeof(optionSucRate[5]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, adInteger, optionID[6], sizeof(optionID[6]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, adInteger, optionSucRate[6], sizeof(optionSucRate[6]), FALSE)
//	ADO_VARIABLE_LENGTH_ENTRY4(16, adInteger, optionID[7], sizeof(optionID[7]), FALSE)
//	ADO_VARIABLE_LENGTH_ENTRY4(17, adInteger, optionSucRate[7], sizeof(optionSucRate[7]), FALSE)
//	ADO_VARIABLE_LENGTH_ENTRY4(18, adInteger, optionID[8], sizeof(optionID[8]), FALSE)
//	ADO_VARIABLE_LENGTH_ENTRY4(19, adInteger, optionSucRate[8], sizeof(optionSucRate[8]), FALSE)
//	ADO_VARIABLE_LENGTH_ENTRY4(20, adInteger, optionID[9], sizeof(optionID[9]), FALSE)
//	ADO_VARIABLE_LENGTH_ENTRY4(21, adInteger, optionSucRate[9], sizeof(optionSucRate[9]), FALSE)
END_ADO_BINDING()
};

void onAwaKenData( dbAwaken * emprs )
{
	RandomManager::RegisterAwakenInfo( *emprs );
}

void onAwakenProbabilitiesData( dbAwakenProbabilities * emprs )
{
	emprs->fProbabilities.set( emprs->probabilities.getMultipleInteger( 10000 ) );

	RandomManager::RegisterAwakenProbabilitiesInfo( *emprs );
}

void onRandomItemOptionData( dbRandomItemOption * emprs )
{
	emprs->minValue2.set( emprs->_minValue2.getMultipleInteger( 10000 ) );
	emprs->maxValue2.set( emprs->_maxValue2.getMultipleInteger( 10000 ) );
	emprs->extremeRate.set( emprs->_extremeRate.getMultipleInteger( 10000 ) );

	RandomManager::RegisterRandomItemOptionInfo( *emprs );
}

void onRandomItemPresetData( dbRandomItemPreset * emprs )
{
	RandomManager::RegisterRandomItemPresetInfo( *emprs );
}

static bool LoadAwakenResource()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );
								 
	size_t nAwakenCnt = LoadDbResource< dbAwaken >( "AwakenResource", ConnPtr, onAwaKenData );
	size_t nAwakenProbabilityCnt = LoadDbResource< dbAwakenProbabilities >( "AwakenProbabilitiesResource", ConnPtr, onAwakenProbabilitiesData );

	_cprint( "%d Awaken info loading complete...\n", nAwakenCnt );
	FILELOG( "%d Awaken info loading complete...", nAwakenCnt );
	_cprint( "%d AwakenProbability info loading complete...\n", nAwakenProbabilityCnt );
	FILELOG( "%d AwakenProbability info loading complete...", nAwakenProbabilityCnt );

	return true;
}

static bool LoadRandomItemResource()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t nRandomItemOptionCnt = LoadDbResource< dbRandomItemOption >( "RandomItemOptionResource", ConnPtr, onRandomItemOptionData );
	size_t nRandomItemPresetCnt = LoadDbResource< dbRandomItemPreset >( "RandomItemPresetResource", ConnPtr, onRandomItemPresetData );

	// 유효성 검사

	_cprint( "Total %d Random item option info loaded...\n", nRandomItemOptionCnt );
	FILELOG( "Total %d Random item option info loaded...", nRandomItemOptionCnt );
	_cprint( "Total %d Random item preset info loaded...\n", nRandomItemPresetCnt );
	FILELOG( "Total %d Random item preset info loaded...", nRandomItemPresetCnt );

	return true;
}

bool AwakenLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadAwakenResource();
		helper.end( "AwakenLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "AwakenLoader" );
		LogDBError( e, "AwakenLoader", "LoadAwakenResource()" );

		std::string strError = "AWAKEN RESOURCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}

bool RandomItemLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadRandomItemResource();
		helper.end( "RandomItemLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "RandomItemLoader" );
		LogDBError( e, "RandomItemLoader", "LoadRandomItemResource()" );

		std::string strError = "RANDOM ITEM RESOURCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}
