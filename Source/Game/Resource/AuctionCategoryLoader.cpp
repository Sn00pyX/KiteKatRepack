#include <oledb.h>

#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "AuctionManager.h"

struct dbAuctionCategory : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbAuctionCategory)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, category_id, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, sub_category_id, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, name_id, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, local_flag, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, item_group, 4, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, item_class, 4, FALSE)
   
END_ADO_BINDING()

	int						category_id;
	int						sub_category_id;
	int						name_id;
	int						local_flag;
	int						item_group;
	int						item_class;
};

static void onAuctionCategoryInfo( dbAuctionCategory * emprs )
{
	// 국가 코드 체크
	extern volatile int g_nCurrentLocalFlag;
	if( emprs->local_flag & g_nCurrentLocalFlag )
	{
		return;
	}

	// 값 규칙 체크
	// sub_category_id가 SUB_CATEGORY_ALL 이 아니려면 category_id도 CATEGORY_SPECIAL이 아니어야 함
	if( emprs->sub_category_id != SUB_CATEGORY_ALL && emprs->category_id == CATEGORY_SPECIAL )
	{
		assert( 0 );
		return;
	}

	// 중복 카테고리인지 체크는 별도로 하지 않고 RegisterAuctionCategory에서 false 리턴
	if( !AuctionManager::Instance().RegisterAuctionCategory( emprs->category_id, emprs->sub_category_id, emprs->item_group, emprs->item_class ) )
	{
		assert( 0 );
		return;
	}
}

bool AuctionCategoryLoader::onProcess( int nThreadNum )
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t nTotalCnt = LoadDbResource< dbAuctionCategory >( "SELECT * FROM AuctionCategoryResource ORDER BY category_id, sub_category_id", ConnPtr, onAuctionCategoryInfo, adCmdText );
	AuctionManager::Instance().FinalizeAuctionCategory();	// 로딩이 끝났으면 그에 맞게 카테고리 벡터를 생성해야 함
	size_t nCategoryCnt = AuctionManager::Instance().GetAuctionItemCategoryCount();

	_cprint( "Total %d/%d AuctionCategoryResource loaded.\n", nCategoryCnt, nTotalCnt );
	FILELOG( "Total %d/%d AuctionCategoryResource loaded.", nCategoryCnt, nTotalCnt );
	
	return true;
}
