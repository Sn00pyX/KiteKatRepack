
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "DB_Commands.h"
#include "AuctionManager.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbAutoAuctionRegistrationInfo : public CADORecordBinding
{
	int				sid;						// 1
	int				auto_auction_id;			// 2
	DBTIMESTAMP		scheduled_register_time;	// 3
	DBTIMESTAMP		registered_time;			// 4

BEGIN_ADO_BINDING(dbAutoAuctionRegistrationInfo)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, sid, sizeof( sid ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, auto_auction_id, sizeof( auto_auction_id ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adDBTimeStamp, scheduled_register_time, sizeof( scheduled_register_time ), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adDBTimeStamp, registered_time, sizeof( registered_time ), FALSE)
END_ADO_BINDING()
};

bool AutoAuctionRegistrationInfoLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		AuctionManager::Instance().ClearAutoAuctionRegistrationInfo();

		helper.start();
		loadAutoAuctionRegistrationInfo();
		helper.end( "AutoAuctionRegistrationInfoLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "AutoAuctionRegistrationInfoLoader" );
		LogDBError( e, "AutoAuctionRegistrationInfoLoader", "loadAutoAuctionRegistrationInfo()" );

		std::string strError = "AutoAuctionRegistrationInfo USER DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}

bool AutoAuctionRegistrationInfoLoader::loadAutoAuctionRegistrationInfo()
{
	_ConnectionPtr		ConnPtr = NULL;
	InitUserDbConnection( ConnPtr );

	_RecordsetPtr		pRstItemInfo = NULL;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	HRESULT  hr = S_OK;

	dbAutoAuctionRegistrationInfo emprs;       // C++ Class object

	pRstItemInfo.CreateInstance( __uuidof(Recordset) );

	pRstItemInfo->Open( "AutoAuctionRegistrationInfo",
		_variant_t((IDispatch *)ConnPtr,true),
		adOpenForwardOnly, adLockReadOnly, adCmdTable );

	pRstItemInfo->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );

	picRs->BindToRecordset(&emprs);

	int nTotalCount = 0;
	int nLoadCount = 0;

	for( ; pRstItemInfo->State != adStateClosed && !pRstItemInfo->EndOfFile; pRstItemInfo->MoveNext() )
	{
		struct tm tmRegisteredTime;
		tmRegisteredTime.tm_year = emprs.registered_time.year - 1900;
		tmRegisteredTime.tm_mon = emprs.registered_time.month - 1;
		tmRegisteredTime.tm_mday = emprs.registered_time.day;
		tmRegisteredTime.tm_hour = emprs.registered_time.hour;
		tmRegisteredTime.tm_min = emprs.registered_time.minute;
		tmRegisteredTime.tm_sec = emprs.registered_time.second;
		tmRegisteredTime.tm_isdst = -1;
		time_t tRegisteredTime = mktime( &tmRegisteredTime );

		if( AuctionManager::Instance().SetLastAutoRegisteredTime( emprs.auto_auction_id, tRegisteredTime ) )
		{
			++nLoadCount;
		}

		++nTotalCount;
	}

	_cprint( "Total %d/%d AutoAuctionRegistrationInfo loaded...\n", nLoadCount, nTotalCount );
	FILELOG( "Total %d/%d AutoAuctionRegistrationInfo loaded...", nLoadCount, nTotalCount );

	return true;
}
