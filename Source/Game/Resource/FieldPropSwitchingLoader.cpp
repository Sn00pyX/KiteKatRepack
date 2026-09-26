
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "FieldPropManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbFieldPropSwitching : public CADORecordBinding, public FieldPropSwitchingData
{
	int				local_flag;
	int				server_idx;

BEGIN_ADO_BINDING( dbFieldPropSwitching )
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, local_flag, sizeof(local_flag), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, server_idx, sizeof(server_idx), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, prop_id, sizeof(prop_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, switched_prop_id, sizeof(switched_prop_id), FALSE)
END_ADO_BINDING()
};

static void onFieldPropSwitchingData( dbFieldPropSwitching * emprs )
{
	// 국가별 프랍 바꿔치기 설정 적용 체크
	extern volatile int g_nCurrentLocalFlag;
	if( emprs->local_flag & g_nCurrentLocalFlag )
	{
		return;
	}

	// 서버 인덱스별 설정 적용 체크
	if( emprs->server_idx != ENV().GetInt( "auth.server_idx" ) )
	{
		return;
	}

	FieldPropManager::GetInstance().RegisterFieldPropSwitchingData( *emprs );
}

bool FieldPropSwitchingLoader::onProcess( int nThreadNum )
{
	HRESULT  hr = S_OK;
	DBPerformanceTrackHelper helper;
	try
	{
		_ConnectionPtr		ConnPtr = NULL;

		InitContentDbConnection( ConnPtr );

		helper.start();
		size_t cnt = LoadDbResource< dbFieldPropSwitching >( "FieldPropSwitchingResource", ConnPtr, onFieldPropSwitchingData );
		helper.end( "FieldPropSwitchingLoader" );

		_cprint( "Total %d/%d Field Prop Switching loaded...\n", FieldPropManager::GetInstance().GetFieldPropSwitchingDataCount(), cnt );
		FILELOG( "Total %d/%d Field Prop Switching loaded...", FieldPropManager::GetInstance().GetFieldPropSwitchingDataCount(), cnt );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "FieldPropSwitchingLoader" );
		LogDBError( e, "FieldPropSwitchingLoader", "FieldPropSwitchingResource" );

		std::string strError = "FIELD PROP SWITCHING RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}
