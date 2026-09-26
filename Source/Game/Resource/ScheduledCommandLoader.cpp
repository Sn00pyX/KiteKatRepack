
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "ScheduledCommandManager.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbScheduledCommand : public CADORecordBinding
{
	int sid;
	char type;
	DBTIMESTAMP begin_time;
	DBTIMESTAMP end_time;
	int interval;
	wchar_t command[256];
	bool is_finished;
	//DBTIMESTAMP launched_time;

BEGIN_ADO_BINDING(dbScheduledCommand)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, sid, sizeof( sid ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adTinyInt, type, sizeof( type ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adDBTimeStamp, begin_time, sizeof( begin_time ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adDBTimeStamp, end_time, sizeof( end_time ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, interval, sizeof( interval ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adVarWChar, command, _countof( command ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adBoolean, is_finished, 1, FALSE)
END_ADO_BINDING()
};

static bool onScheduledCommandData( dbScheduledCommand * emprs )
{
	if( emprs->is_finished )
	{
		return false;
	}

	emprs->command[ _countof( emprs->command ) - 1 ] = L'\0';

	struct tm tmBeginTime;
	tmBeginTime.tm_year = emprs->begin_time.year - 1900;
	tmBeginTime.tm_mon = emprs->begin_time.month - 1;
	tmBeginTime.tm_mday = emprs->begin_time.day;
	tmBeginTime.tm_hour = emprs->begin_time.hour;
	tmBeginTime.tm_min = emprs->begin_time.minute;
	tmBeginTime.tm_sec = emprs->begin_time.second;
	tmBeginTime.tm_isdst = -1;
	time_t tBeginTime = mktime( &tmBeginTime );

	struct tm tmEndTime;
	tmEndTime.tm_year = emprs->end_time.year - 1900;
	tmEndTime.tm_mon = emprs->end_time.month - 1;
	tmEndTime.tm_mday = emprs->end_time.day;
	tmEndTime.tm_hour = emprs->end_time.hour;
	tmEndTime.tm_min = emprs->end_time.minute;
	tmEndTime.tm_sec = emprs->end_time.second;
	tmEndTime.tm_isdst = -1;
	time_t tEndTime = mktime( &tmEndTime );

	if( !emprs->interval || tEndTime == -1 || tEndTime < tBeginTime )
	{
		tEndTime = tBeginTime;
		emprs->interval = 0;
	}

	return ScheduledCommandManager::Instance().RegisterScheduledCommand( emprs->sid,
		static_cast< ScheduledCommandManager::_SCHEDULED_COMMAND_TYPE >( emprs->type ),
		tBeginTime, tEndTime, emprs->interval, emprs->command );
}

static bool LoadScheduledCommand()
{
	_ConnectionPtr		ConnPtr = NULL;
	InitUserDbConnection( ConnPtr );

	_RecordsetPtr		pRstScheduledCommand = NULL;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	HRESULT  hr = S_OK;
	
	dbScheduledCommand emprs;       // C++ Class object

	pRstScheduledCommand.CreateInstance( __uuidof(Recordset) );

	pRstScheduledCommand->Open("ScheduledCommand", 
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdTable);

	pRstScheduledCommand->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );

	picRs->BindToRecordset(&emprs);

	int nTotalCnt = 0;
	int nLoadCount = 0;

	while( pRstScheduledCommand->State != adStateClosed && !pRstScheduledCommand->EndOfFile )
	{
		if( onScheduledCommandData( &emprs ) )
			++nLoadCount;

		hr = pRstScheduledCommand->MoveNext();
		++nTotalCnt;
	}

	_cprint( "Total %d/%d ScheduledCommand loaded...\n", nLoadCount, nTotalCnt );
	FILELOG( "Total %d/%d ScheduledCommand loaded...", nLoadCount, nTotalCnt );

	return true;
}

bool ScheduledCommandLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		ScheduledCommandManager::Instance().ClearScheduledCommandList();

		helper.start();
		LoadScheduledCommand();
		helper.end( "ScheduledCommandLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "ScheduledCommandLoader" );
		LogDBError( e, "ScheduledCommandLoader", "LoadScheduledCommand()" );

		std::string strError = "ScheduledCommand RESOURCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}