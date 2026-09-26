
#include <toolkit/XEnv.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>
#include <dump/XException.h>

#include "ContentLoader.h"
#include "QuestBase.h"
#include "StructQuest.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbQuest : public CADORecordBinding, public QuestBaseServer
{
	dbQuest() { script_drop_string[0] = '\0'; script_accept_string[0] = '\0'; script_clear_string[0] = '\0'; }

	char							limit_deva;
	char							limit_asura;
	char							limit_gaia;

	char							limit_fighter;
	char							limit_hunter;
	char							limit_magician;
	char							limit_summoner;

	char							is_repeatable;

	__int64							gold;

	char							script_accept_string[513];
	char							script_clear_string[513];
	char							script_drop_string[513];

BEGIN_ADO_BINDING(dbQuest)

   	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, nCode, sizeof(nCode), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, nQuestTextId, sizeof(nQuestTextId), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, nSummaryTextId, sizeof(nSummaryTextId), FALSE)
   	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, nStatusTextId, sizeof(nStatusTextId), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, nBeginTime, sizeof(nBeginTime), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, nEndTime, sizeof(nEndTime), FALSE)	

	ADO_VARIABLE_LENGTH_ENTRY4(7, adInteger, nLimitLevel, sizeof(nLimitLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, nLimitJobLevel, sizeof(nLimitJobLevel), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, nLimitMaxLevel, sizeof(nLimitMaxLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, adInteger, nLimitMaxJobLevel, sizeof(nLimitMaxJobLevel), FALSE)	

	ADO_VARIABLE_LENGTH_ENTRY4(11, adBoolean, limit_deva, sizeof(limit_deva), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, adBoolean, limit_asura, sizeof(limit_asura), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(13, adBoolean, limit_gaia, sizeof(limit_gaia), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(14, adBoolean, limit_fighter, sizeof(limit_fighter), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, adBoolean, limit_hunter, sizeof(limit_hunter), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(16, adBoolean, limit_magician, sizeof(limit_magician), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(17, adBoolean, limit_summoner, sizeof(limit_summoner), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(18, adInteger, nLimitJob, sizeof(nLimitJob), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(19, adTinyInt, nJobDepth, sizeof(nJobDepth), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(20, adInteger, nLimitFavorGroupId, sizeof(nLimitFavorGroupId), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(21, adInteger, nLimitFavor, sizeof(nLimitFavor), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(22, adBoolean, is_repeatable, sizeof(is_repeatable), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(23, adInteger, nInvokeCondition, sizeof(nInvokeCondition), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(24, adInteger, nInvokeValue, sizeof(nInvokeValue), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(25, adInteger, eTimeLimitType, sizeof( eTimeLimitType ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(26, adInteger, nTimeLimit, sizeof( nTimeLimit ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(27, adInteger, nType, sizeof(nType), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(28, adInteger, nValue[0], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(29, adInteger, nValue[1], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(30, adInteger, nValue[2], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(31, adInteger, nValue[3], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(32, adInteger, nValue[4], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(33, adInteger, nValue[5], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(34, adInteger, nValue[6], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(35, adInteger, nValue[7], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(36, adInteger, nValue[8], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(37, adInteger, nValue[9], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(38, adInteger, nValue[10], 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(39, adInteger, nValue[11], 4, FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(40, adInteger, nDropGroupId, sizeof(nDropGroupId), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(41, adInteger, nQuestDifficulty, sizeof(nQuestDifficulty), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(42, adInteger, nFavorGroupId, sizeof(nFavorGroupId), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(43, adInteger, nHateGroupId, sizeof(nHateGroupId), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(44, adInteger, nFavor, sizeof(nFavor), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(45, adBigInt, nExp, sizeof(nExp), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(46, adInteger, nJP, sizeof(nJP), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(47, adInteger, nHuntaholicPoint, sizeof(nHuntaholicPoint), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(48, adBigInt, gold, sizeof(gold), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(49, adInteger, DefaultReward.nItemCode, sizeof(DefaultReward.nItemCode), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(50, adInteger, DefaultReward.nLevel, sizeof(DefaultReward.nLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(51, adInteger, DefaultReward.nQuantity, sizeof(DefaultReward.nQuantity), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(52, adInteger, OptionalReward[0].nItemCode, sizeof(OptionalReward[0].nItemCode), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(53, adInteger, OptionalReward[0].nLevel, sizeof(OptionalReward[0].nLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(54, adInteger, OptionalReward[0].nQuantity, sizeof(OptionalReward[0].nQuantity), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(55, adInteger, OptionalReward[1].nItemCode, sizeof(OptionalReward[1].nItemCode), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(56, adInteger, OptionalReward[1].nLevel, sizeof(OptionalReward[1].nLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(57, adInteger, OptionalReward[1].nQuantity, sizeof(OptionalReward[1].nQuantity), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(58, adInteger, OptionalReward[2].nItemCode, sizeof(OptionalReward[2].nItemCode), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(59, adInteger, OptionalReward[2].nLevel, sizeof(OptionalReward[2].nLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(60, adInteger, OptionalReward[2].nQuantity, sizeof(OptionalReward[2].nQuantity), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(61, adInteger, OptionalReward[3].nItemCode, sizeof(OptionalReward[3].nItemCode), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(62, adInteger, OptionalReward[3].nLevel, sizeof(OptionalReward[3].nLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(63, adInteger, OptionalReward[3].nQuantity, sizeof(OptionalReward[3].nQuantity), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(64, adInteger, OptionalReward[4].nItemCode, sizeof(OptionalReward[4].nItemCode), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(65, adInteger, OptionalReward[4].nLevel, sizeof(OptionalReward[4].nLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(66, adInteger, OptionalReward[4].nQuantity, sizeof(OptionalReward[4].nQuantity), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(67, adInteger, OptionalReward[5].nItemCode, sizeof(OptionalReward[5].nItemCode), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(68, adInteger, OptionalReward[5].nLevel, sizeof(OptionalReward[5].nLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(69, adInteger, OptionalReward[5].nQuantity, sizeof(OptionalReward[5].nQuantity), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(70, adInteger, nForeQuest[0], sizeof(nForeQuest[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(71, adInteger, nForeQuest[1], sizeof(nForeQuest[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(72, adInteger, nForeQuest[2], sizeof(nForeQuest[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(73, adBoolean, bForeCheckType, sizeof(bForeCheckType), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(74, adVarChar, script_accept_string, _countof(script_accept_string), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(75, adVarChar, script_clear_string, _countof(script_clear_string), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(76, adVarChar, script_drop_string, _countof(script_drop_string), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(80, adInteger, nCoolTime, sizeof(nCoolTime), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(81, adInteger, nDropCoolTime, sizeof(nDropCoolTime), FALSE)
END_ADO_BINDING()

};

struct dbQuestLink : public CADORecordBinding, public QuestLink
{
BEGIN_ADO_BINDING(dbQuestLink)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, nNPCId, sizeof(nNPCId), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, code, sizeof(code), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(3, adBoolean, bLF_Start, sizeof(bLF_Start), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adBoolean, bLF_Progress, sizeof(bLF_Progress), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adBoolean, bLF_End, sizeof(bLF_End), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, nStartTextId, sizeof(nStartTextId), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adInteger, nInProgressTextId, sizeof(nInProgressTextId), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, nEndTextId, sizeof(nEndTextId), FALSE)

END_ADO_BINDING()
};

struct dbRandomPool : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbRandomPool)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, group_id, sizeof(group_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, quest_target_id, sizeof(quest_target_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, target_level, sizeof(target_level), FALSE)
END_ADO_BINDING()

	int group_id;
	int quest_target_id;
	int target_level;
};
static void onQuestData( dbQuest * emprs )
{
	emprs->LimitFlag.Clear();

	if( emprs->limit_asura )	emprs->LimitFlag.On( QuestBase::LIMIT_ASURA );
	if( emprs->limit_gaia )		emprs->LimitFlag.On( QuestBase::LIMIT_GAIA );
	if( emprs->limit_deva )		emprs->LimitFlag.On( QuestBase::LIMIT_DEVA );

	if( emprs->limit_hunter )	emprs->LimitFlag.On( QuestBase::LIMIT_HUNTER );
	if( emprs->limit_fighter )	emprs->LimitFlag.On( QuestBase::LIMIT_FIGHTER );
	if( emprs->limit_magician ) emprs->LimitFlag.On( QuestBase::LIMIT_MAGICIAN );
	if( emprs->limit_summoner ) emprs->LimitFlag.On( QuestBase::LIMIT_SUMMONER );

	emprs->bIsRepeatable = emprs->is_repeatable;

	emprs->nGold.SetRawData( emprs->gold );

	emprs->strAcceptScript = emprs->script_accept_string;
	emprs->strClearScript = emprs->script_clear_string;
	emprs->strDropScript = emprs->script_drop_string;

	if( strlen( emprs->script_accept_string ) < 3 ) emprs->strAcceptScript = "";
	if( strlen( emprs->script_clear_string ) < 3 ) emprs->strClearScript = "";
	if( strlen( emprs->script_drop_string ) < 3 ) emprs->strDropScript = "";
	
	StructQuest::RegisterQuestBase( *emprs );
}


static bool LoadQuestData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbQuest >( "QuestResource", ConnPtr, onQuestData );

	_cprint( "Total %d Quest loaded...\n", cnt );
	FILELOG( "Total %d Quest info loaded...", cnt );

	return true;
}

static void onQuestLinkData( dbQuestLink * emprs )
{
	if( emprs->nNPCId )
		GameContent::RegisterQuestLink( *emprs );
	else
		GameContent::RegisterQuestTextId( emprs->code, emprs->nStartTextId, emprs->nInProgressTextId, emprs->nEndTextId );
}


static bool LoadQuestLinkData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbQuestLink >( "QuestLinkResource", ConnPtr, onQuestLinkData );

	_cprint( "Total %d QuestLink loaded...\n", cnt );
	FILELOG( "Total %d QuestLink info loaded...", cnt );

	return true;
}

static void onRandomPoolData( dbRandomPool * emprs )
{
	GameContent::RegisterRandomPoolInfo( emprs->group_id, emprs->quest_target_id, emprs->target_level );
}

static bool LoadRandomPoolData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbRandomPool >( "RandomPoolResource", ConnPtr, onRandomPoolData );

	_cprint( "Total %d RandomPool loaded...\n", cnt );
	FILELOG( "Total %d RandomPool info loaded...", cnt );

	return true;
}

bool QuestLoader::onProcess( int nThreadNum )
{
	HRESULT  hr = S_OK;
	DBPerformanceTrackHelper helper;
	try
	{
		GameContent::ClearQuestLink();

		helper.start();
		LoadQuestData();
		LoadQuestLinkData();
		LoadRandomPoolData();
		helper.end( "QuestLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "QuestLoader" );
		LogDBError( e, "QuestLoader", "QuestLoader::onProcess()" );

		std::string strError = "QUEST RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}



