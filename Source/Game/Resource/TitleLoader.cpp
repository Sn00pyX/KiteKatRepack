
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "TitleBase.h"
#include "StructTitleManager.h"
#include "DBPerformanceTracker.h"


struct dbTitle : public CADORecordBinding, public TitleBaseServer
{
	static void onTitleInfo( dbTitle * emprs );

	_decimal_variant	opt_var1[8];
	_decimal_variant	opt_var2[8];

	int					effect_id;
	int					local_flag;
	DBTIMESTAMP			dtBeginOfPeriod;
	DBTIMESTAMP			dtEndOfPeriod;

BEGIN_ADO_BINDING( dbTitle )

	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, nID, sizeof( nID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, nNameID, sizeof( nNameID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adTinyInt, nRate, sizeof( nRate ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	6, adSmallInt, nOptType[0], sizeof(nOptType[0]), FALSE )
	ADO_NUMERIC_ENTRY2(			7, adDecimal, opt_var1[0],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			8, adDecimal, opt_var2[0],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	9, adSmallInt, nOptType[1], sizeof(nOptType[1]), FALSE )
	ADO_NUMERIC_ENTRY2(			10, adDecimal, opt_var1[1],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			11, adDecimal, opt_var2[1],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	12, adSmallInt, nOptType[2], sizeof(nOptType[2]), FALSE )
	ADO_NUMERIC_ENTRY2(			13, adDecimal, opt_var1[2],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			14, adDecimal, opt_var2[2],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	15, adSmallInt, nOptType[3], sizeof(nOptType[3]), FALSE )
	ADO_NUMERIC_ENTRY2(			16, adDecimal, opt_var1[3],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			17, adDecimal, opt_var2[3],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	18, adSmallInt, nOptType[4], sizeof(nOptType[4]), FALSE )
	ADO_NUMERIC_ENTRY2(			19, adDecimal, opt_var1[4],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			20, adDecimal, opt_var2[4],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	21, adSmallInt, nOptType[5], sizeof(nOptType[5]), FALSE )
	ADO_NUMERIC_ENTRY2(			22, adDecimal, opt_var1[5],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			23, adDecimal, opt_var2[5],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	24, adSmallInt, nOptType[6], sizeof(nOptType[6]), FALSE )
	ADO_NUMERIC_ENTRY2(			25, adDecimal, opt_var1[6],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			26, adDecimal, opt_var2[6],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	27, adSmallInt, nOptType[7], sizeof(nOptType[7]), FALSE )
	ADO_NUMERIC_ENTRY2(			28, adDecimal, opt_var1[7],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			29, adDecimal, opt_var2[7],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4( 30, adInteger, effect_id, sizeof( effect_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 31, adInteger, local_flag, sizeof( local_flag ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 32, adBoolean, bIsPeriodic, sizeof(bIsPeriodic), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 33, adDBTimeStamp, dtBeginOfPeriod, sizeof(dtBeginOfPeriod), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 34, adDBTimeStamp, dtEndOfPeriod, sizeof(dtEndOfPeriod), FALSE)

END_ADO_BINDING()
};

struct dbTitleCondition : public CADORecordBinding, public TitleCondition
{
	static void onTitleConditionInfo( dbTitleCondition * emprs );

	int nConditionType;
	__int64 nCount;
	int nGroupID;
	bool bIsArchieveCondition;

BEGIN_ADO_BINDING( dbTitleCondition )

	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, nTitleID, sizeof( nTitleID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, nGroupID, sizeof( nGroupID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, nConditionType, sizeof( nConditionType ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adBigInt, nCount, sizeof( nCount ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adBoolean, bIsArchieveCondition, sizeof( bIsArchieveCondition ), FALSE )

END_ADO_BINDING()
};

struct dbTitleConditionType : public CADORecordBinding, public TitleConditionType
{
	static void onTitleConditionTypeInfo( dbTitleConditionType * emprs );

BEGIN_ADO_BINDING( dbTitleConditionType )

	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, nID, sizeof( nID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, nCategory, sizeof( nCategory ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, nValue[0], sizeof( nValue[0] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, nValue[1], sizeof( nValue[1] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, nValue[2], sizeof( nValue[2] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adBoolean, bIsSet, sizeof( bIsSet ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adBoolean, bSkipDBUpdate, sizeof( bSkipDBUpdate ), FALSE )

END_ADO_BINDING()
};

void dbTitle::onTitleInfo( dbTitle * emprs )
{
	// 국가 코드 체크
	extern volatile int g_nCurrentLocalFlag;
	if( emprs->local_flag & g_nCurrentLocalFlag )
	{
		return;
	}

	for( int x = 0; x < TitleBase::MAX_OPTION_NUMBER; ++x )
	{
		emprs->fOptVar1[x].set( emprs->opt_var1[x].getMultipleInteger( 10000 ) );
		emprs->fOptVar2[x].set( emprs->opt_var2[x].getMultipleInteger( 10000 ) );
	}

	if( emprs->effect_id )
		emprs->pvEffectList = GameContent::GetEffectInfoVector( emprs->effect_id );
	else
		emprs->pvEffectList = NULL;

	if( emprs->bIsPeriodic )
	{
		struct tm tmPeriod;

		tmPeriod.tm_year = emprs->dtBeginOfPeriod.year - 1900;
		tmPeriod.tm_mon = emprs->dtBeginOfPeriod.month - 1;
		tmPeriod.tm_mday = emprs->dtBeginOfPeriod.day;
		tmPeriod.tm_hour = emprs->dtBeginOfPeriod.hour;
		tmPeriod.tm_min = emprs->dtBeginOfPeriod.minute;
		tmPeriod.tm_sec = emprs->dtBeginOfPeriod.second;
		tmPeriod.tm_isdst = -1;
		emprs->tBeginOfPeriod = mktime( &tmPeriod );

		if( emprs->tBeginOfPeriod == -1 )
		{
			assert( 0 );
			emprs->tBeginOfPeriod = 0;
		}

		tmPeriod.tm_year = emprs->dtEndOfPeriod.year - 1900;
		tmPeriod.tm_mon = emprs->dtEndOfPeriod.month - 1;
		tmPeriod.tm_mday = emprs->dtEndOfPeriod.day;
		tmPeriod.tm_hour = emprs->dtEndOfPeriod.hour;
		tmPeriod.tm_min = emprs->dtEndOfPeriod.minute;
		tmPeriod.tm_sec = emprs->dtEndOfPeriod.second;
		tmPeriod.tm_isdst = -1;
		emprs->tEndOfPeriod = mktime( &tmPeriod );

		if( emprs->tEndOfPeriod == -1 )
		{
			assert( 0 );
			emprs->tEndOfPeriod = 0;
		}
	}
	else
	{
		emprs->tBeginOfPeriod = 0;
		emprs->tEndOfPeriod = 0;
	}

	TitleBaseServer * pTitleBase = StructTitleManager::RegisterTitleBase( new TitleBaseServer( *emprs ) );
}

void dbTitleCondition::onTitleConditionInfo( dbTitleCondition * emprs )
{
	static int nLastID = 0;
	static int nLastGroupID = 0;
	static TitleCondition * pLastTitleCondition = NULL;
	static std::set< int > setConditionType;

	if( nLastID != emprs->nTitleID )
	{
		// TitleCondition과 관련된 TitleConditionType을 연결해준다.
		for( std::set< int >::const_iterator it = setConditionType.begin(); it != setConditionType.end(); it++ )
		{
			StructTitleManager::RelateTitleCondition( (*it), pLastTitleCondition );
		}

		nLastID = emprs->nTitleID;
		nLastGroupID = 0;
		setConditionType.clear();

		if( !emprs->nTitleID )
			return;

		pLastTitleCondition = StructTitleManager::RegisterTitleCondition( new TitleCondition( *emprs ) );
	}

	// 데이터가 단 하나도 없다면 nLastID와 nID 모두가 0이며 의미없는 데이터이다.
	if( !emprs->nTitleID )
		return;

	// 그룹 ID는 순차적이어야 하며, 상한을 둔다.
	assert( emprs->nGroupID <= nLastGroupID + 1 && emprs->nGroupID <= GameRule::TITLE_CONDITION_MAX_GROUP );

	if( emprs->bIsArchieveCondition )
	{
		pLastTitleCondition->vTitleAchieveConditionInfo.push_back( TitleCondition::TITLE_CONDITION_INFO( emprs->nConditionType, emprs->nCount, emprs->nGroupID ) );
	}
	else
	{
		pLastTitleCondition->vTitleOpenConditionInfo.push_back( TitleCondition::TITLE_CONDITION_INFO( emprs->nConditionType, emprs->nCount, emprs->nGroupID ) );
	}

	nLastGroupID = emprs->nGroupID;
	setConditionType.insert( emprs->nConditionType );
}

void dbTitleConditionType::onTitleConditionTypeInfo( dbTitleConditionType * emprs )
{
	StructTitleManager::RegisterTitleConditionType( new TitleConditionType( *emprs ) );
}

static bool LoadTitleData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t condition_type_cnt = LoadDbResource< dbTitleConditionType >( "SELECT * FROM dbo.TitleConditionTypeResource ORDER BY category", ConnPtr, dbTitleConditionType::onTitleConditionTypeInfo, adCmdText );
	{
		dbTitleConditionType dbDummy;
		dbDummy.nID = 0;
		dbDummy.nCategory = 0;

		dbTitleConditionType::onTitleConditionTypeInfo( &dbDummy );
	}

	size_t condition_cnt = LoadDbResource< dbTitleCondition >( "SELECT * FROM dbo.TitleConditionResource ORDER BY title_id, group_id", ConnPtr, dbTitleCondition::onTitleConditionInfo, adCmdText );
	{
		dbTitleCondition dbDummy;
		dbDummy.nTitleID = 0;

		dbTitleCondition::onTitleConditionInfo( &dbDummy );
	}

	size_t cnt = LoadDbResource< dbTitle >( "TitleResource", ConnPtr, dbTitle::onTitleInfo );

	_cprint( "Total %d Title table loaded...\n", cnt );
	FILELOG( "Total %d Title table loaded...", cnt );
	_cprint( "Total %d Title Condition table loaded...\n", condition_cnt );
	FILELOG( "Total %d Title Condition table loaded...", condition_cnt );
	_cprint( "Total %d Title Condition Type table loaded...\n", condition_type_cnt );
	FILELOG( "Total %d Title Condition Type table loaded...", condition_type_cnt );

	return true;
}

bool TitleLoader::onProcess( int nThreadNum )
{
	HRESULT hr = S_OK;

	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadTitleData();
		helper.end( "TitleLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "TitleLoader" );
		LogDBError( e, "TitleLoader", "LoadTitleData()" );

		std::string strError = "Title RESOURCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}