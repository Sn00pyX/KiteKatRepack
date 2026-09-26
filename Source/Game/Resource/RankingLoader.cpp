
#include <ctime>

#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "RankingManager.h"


struct dbRankingInfo : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbRankingInfo)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, ranking_id, sizeof( ranking_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adDBTimeStamp, next_settling_time, sizeof( next_settling_time ), FALSE )
END_ADO_BINDING()

	int				ranking_id;
	DBTIMESTAMP		next_settling_time;
};

struct dbRankingScore : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbRankingScore)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, ranking_id, sizeof( ranking_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, owner_id, sizeof( owner_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adVarChar, player_name, _countof( player_name ), FALSE )
	ADO_NUMERIC_ENTRY2(			4, adDecimal, dv_score, 18, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adBoolean, is_valid, sizeof( is_valid ), FALSE )
END_ADO_BINDING()

	int					ranking_id;
	int					owner_id;
	char				player_name[ 31 ];
	_decimal_variant	dv_score;
	bool				is_valid;
};

void onRankingInfo( dbRankingInfo * emprs )
{
	struct tm next_settling_time;

	next_settling_time.tm_year = emprs->next_settling_time.year - 1900;
	next_settling_time.tm_mon = emprs->next_settling_time.month - 1;
	next_settling_time.tm_mday = emprs->next_settling_time.day;
	next_settling_time.tm_hour = emprs->next_settling_time.hour;
	next_settling_time.tm_min = emprs->next_settling_time.minute;
	next_settling_time.tm_sec = emprs->next_settling_time.second;
	next_settling_time.tm_isdst = -1;

	time_t tNextSettling = mktime( &next_settling_time );

	_cprint( "RankingInfo: id(%d), next_settling_time(%04d-%02d-%02d %02d:%02d:%02d, %04d-%02d-%02d %02d:%02d:%02d,%d = %I64d)\n", emprs->ranking_id,
		emprs->next_settling_time.year, emprs->next_settling_time.month, emprs->next_settling_time.day,
		emprs->next_settling_time.hour, emprs->next_settling_time.minute, emprs->next_settling_time.second,
		next_settling_time.tm_year + 1900, next_settling_time.tm_mon + 1, next_settling_time.tm_mday,
		next_settling_time.tm_hour, next_settling_time.tm_min, next_settling_time.tm_sec,
		next_settling_time.tm_isdst, tNextSettling );
	FILELOG( "RankingInfo: id(%d), next_settling_time(%04d-%02d-%02d %02d:%02d:%02d, %04d-%02d-%02d %02d:%02d:%02d,%d = %I64d)", emprs->ranking_id,
		emprs->next_settling_time.year, emprs->next_settling_time.month, emprs->next_settling_time.day,
		emprs->next_settling_time.hour, emprs->next_settling_time.minute, emprs->next_settling_time.second,
		next_settling_time.tm_year + 1900, next_settling_time.tm_mon + 1, next_settling_time.tm_mday,
		next_settling_time.tm_hour, next_settling_time.tm_min, next_settling_time.tm_sec,
		next_settling_time.tm_isdst, tNextSettling );

	if( tNextSettling == -1 )
		tNextSettling = 0;

	RankingManager::Instance().SetNextSettlingTime( static_cast< RankingManager::_RANKING_TYPE >( emprs->ranking_id ), tNextSettling );
}

void onRankingScore( dbRankingScore * emprs )
{
	c_fixed10 fScore;
	fScore.set( emprs->dv_score.getMultipleInteger( 10000 ) );

	RankingManager::Instance().SetRankingScore( static_cast< RankingManager::_RANKING_TYPE >( emprs->ranking_id ), emprs->owner_id, emprs->player_name, fScore, emprs->is_valid );
}

bool RankingLoader::onProcess( int nThreadNum )
{
	_ConnectionPtr		ConnPtr = NULL;

	InitUserDbConnection( ConnPtr );

	size_t nRankingInfoCnt = LoadDbResource< dbRankingInfo >( "RankingInfo", ConnPtr, onRankingInfo );
	size_t nRankingScoreCnt = LoadDbResource< dbRankingScore >( "smp_read_ranking_score", ConnPtr, onRankingScore, adCmdStoredProc );

	_cprint( "Total %d RankingInfo, %d RankingScore loaded...\n", nRankingInfoCnt, nRankingScoreCnt );
	FILELOG( "Total %d RankingInfo, %d RankingScore loaded...", nRankingInfoCnt, nRankingScoreCnt );

	return true;
}
