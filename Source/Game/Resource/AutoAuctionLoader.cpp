
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "AuctionManager.h"

struct dbAutoAuction : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbAutoAuction)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, id, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, item_id, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, auctionseller_name_id, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adBigInt, price, 8, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adBoolean, secroute_apply, 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, local_flag, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(7, adDBTimeStamp, auction_enrollment_time, sizeof(auction_enrollment_time), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adBoolean, repeat_apply, 1, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, repeat_term, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, adSmallInt, auctiontime_type, 2, FALSE)
   
END_ADO_BINDING()

	int						id;
	ItemBase::ItemCode		item_id;
	int						auctionseller_name_id;
	__int64					price;
	bool					secroute_apply;
	int						local_flag;
	DBTIMESTAMP				auction_enrollment_time;
	bool					repeat_apply;
	int						repeat_term;		// 반복 주기(단위: 일)
	short					auctiontime_type;
};

static void onAutoAuctionInfo( dbAutoAuction * emprs )
{
	// 국가 코드 체크
	extern volatile int g_nCurrentLocalFlag;
	if( emprs->local_flag & g_nCurrentLocalFlag )
	{
		return;
	}

	// 이미 등록된 자동 등록 경매 정보인지 체크
	if( AuctionManager::Instance().IsRegisteredAutoAuction( emprs->id ) )
	{
		return;
	}

	struct tm tmEnrollmentTime;
	tmEnrollmentTime.tm_year = emprs->auction_enrollment_time.year - 1900;
	tmEnrollmentTime.tm_mon = emprs->auction_enrollment_time.month - 1;
	tmEnrollmentTime.tm_mday = emprs->auction_enrollment_time.day;
	tmEnrollmentTime.tm_hour = emprs->auction_enrollment_time.hour;
	tmEnrollmentTime.tm_min = emprs->auction_enrollment_time.minute;
	tmEnrollmentTime.tm_sec = emprs->auction_enrollment_time.second;
	tmEnrollmentTime.tm_isdst = -1;
	time_t tEnrollmentTime = mktime( &tmEnrollmentTime );

	time_t tDuration = 0;

	switch( emprs->auctiontime_type )
	{
	case TS_CS_AUCTION_REGISTER::DURATION_LONGTERM:
		tDuration = GameRule::AUCTION_DURATION_LONGTERM;
		break;
	case TS_CS_AUCTION_REGISTER::DURATION_MIDTERM:
		tDuration = GameRule::AUCTION_DURATION_MIDTERM;
		break;
	case TS_CS_AUCTION_REGISTER::DURATION_SHORTTERM:
		tDuration = GameRule::AUCTION_DURATION_SHORTTERM;
		break;
	// 경매 기간 지정 중 기타 방식
	case 0:
		tDuration = GameRule::AUCTION_DURATION_SHORTTERM;
		break;
	default:
		tDuration = GameRule::AUCTION_DURATION_SHORTTERM;
		break;
	}

	AuctionManager::Instance().AddAutoAuctionInfo( emprs->id, GameContent::GetString( emprs->auctionseller_name_id ), emprs->item_id, 1,
		emprs->price, StructGold( 0 ), tEnrollmentTime, tDuration, emprs->secroute_apply, emprs->repeat_apply, emprs->repeat_term * 86400 );
}

bool AutoAuctionLoader::onProcess( int nThreadNum )
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t nTotalCnt = 0;

	nTotalCnt += LoadDbResource< dbAutoAuction >( "AutoAuctionResource", ConnPtr, onAutoAuctionInfo );

	_cprint( "Total %d/%d AutoAuctionResource loaded.\n", AuctionManager::Instance().GetAutoAuctionResouceCount(), nTotalCnt );
	FILELOG( "Total %d/%d AutoAuctionResource loaded.", AuctionManager::Instance().GetAutoAuctionResouceCount(), nTotalCnt );
	
	return true;
}
