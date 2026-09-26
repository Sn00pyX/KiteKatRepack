
#include <oledberr.h>

#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"



struct dbString : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbString)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adVarChar, name, _countof(name), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, string_id, sizeof(string_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adVarWChar, value, _countof(value) - 1, FALSE)
   
END_ADO_BINDING()


	char	name[65];
	int		string_id;	
	wchar_t	value[3072];
};

static void onStringInfo( dbString * emprs )
{
	char value[3072];

	WideCharToMultiByte( ENV().GetInt( "CodePage", CP_ACP ), 0, emprs->value, 3072, value, 3072, 0, FALSE );

	GameContent::RegisterStringInfo( emprs->string_id, value );
}

static void onStringLocalInfo( dbString * emprs )
{
	// 이미 로드되어 있는 데이터면 무시
	if( strcmp( GameContent::GetString( emprs->string_id ), "<NULL>" ) )
	{
		_cprint( "Already loaded resource loaded from [StringLocalResource]. id(%d)\n", emprs->string_id );
		FILELOG( "Already loaded resource loaded from [StringLocalResource]. id(%d)", emprs->string_id );
		return;
	}

	onStringInfo( emprs );
}

bool StringLoader::onProcess( int nThreadNum )
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = 0;
	cnt += LoadDbResource< dbString >( "StringResource", ConnPtr, onStringInfo );

	_cprint( "Total %d String loaded.\n", cnt );
	FILELOG( "Total %d String loaded...", cnt );

	DBPerformanceTrackHelper helper;
	try
	{
		cnt = 0;
		helper.start();
		cnt = LoadDbResource< dbString >( "StringLocalResource", ConnPtr, onStringLocalInfo );
		helper.end( "StringLoader" );

		_cprint( "Total %d String(local) info loaded...\n", cnt );
		FILELOG( "Total %d String(local) info loaded...", cnt );
	}
	catch( _com_error &e )
	{
		if( e.Error() != DB_E_NOTABLE )
		{
			helper.end( e.Error(), "StringLoader" );
			LogDBError( e, "StringLoader", "StringLoader::onProcess()" );
			throw;
		}
	}
	catch( ... )
	{
		throw;
	}

	return true;
}

