
#include <cstdio>
#include <vector>

#include <network/XIOCP.h>
#include <network/XIOCPAcceptor.h>
#include <network/XNetworkUtil.h>
#include <toolkit/XFlag.h>

#include <toolkit/XConsole.h>
#include <toolkit/myDC.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/TimeSyncer.h>

#include <toolkit/XRandom.h>
#include <framework/ArcadiaFramework.h>

#include <toolkit/XEnv.h>
#include <logging/FileLog.h>
#include <cipher/XStrZlibWithSimpleCipherUtil.h>

#include "GameMessage.h"
#include "StructItem.h"
#include "StructMonster.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "Extern.h"
#include "Constant.h"
#include "GameContent.h"
#include "ItemCollector.h"
#include "GameDBManager.h"
#include "DB_Commands.h"
#include "StructEventItemManager.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "StructWorldLocation.h"
#include "HuntaholicManager.h"
#include "DungeonManager.h"
#include "NPCProc.h"
#include "ScheduledCommandManager.h"
#include "RoamingManager.h"
#include "CompeteManager.h"
#include "RankingManager.h"
#include "XSecuritySolutionManager.h"
#include "InstanceDungeonManager.h"
#include "BattleArenaManager.h"
#include "Community/PartyMatchingManager.h"

XCriticalSection				g_Lock;

std::vector< StructPlayer* >	g_vPlayer;
std::vector< StructItem* >		g_vItem;
//std::vector< StructMonster* >		g_vNPC;
std::vector< StructSummon* >	g_vSummon;

XIOCP			*				g_pIOCP;
ArcadiaServer	*				g_pArcadia;

extern ArcadiaServer *			g_pArcadia;

int								g_nDrawMode;

void ChangeDrawMode( int nMode )
{
	g_nDrawMode = nMode;
}
void thread_com_init_func( int nThreadNum )
{
	HRESULT hr = ::CoInitialize( NULL );
	assert( hr == S_OK );
}

void LoadResource()
{
	FILELOG( "Start loading...\n" );

	try
	{
		// DB 연결
		std::string user_connection_string = "Provider=sqloledb;Persist Security Info=False;Network Library=dbmssocn;Initial Catalog=";
		user_connection_string += ENV().GetString( "db.user.name", "Arcadia" );
		user_connection_string += ";Data Source=";
		user_connection_string += ENV().GetString( "db.user.server", "" );
		user_connection_string += ",";
		user_connection_string += ENV().GetString( "db.user.port", "1433" );

		/*
		std::string billing_connection_string = "Provider=sqloledb;Persist Security Info=False;Network Library=dbmssocn;Initial Catalog=";
		billing_connection_string += ENV().GetString( "db.billing.name" );
		billing_connection_string += ";Data Source=";
		billing_connection_string += ENV().GetString( "db.billing.server");
		billing_connection_string += ",";
		billing_connection_string += ENV().GetString( "db.billing.port", "1433" );
		*/

		int nThreadCount = ENV().GetInt( "db.thread", ENV().GetInt( "db.user.thread", 4 ) );
			
		DB().InitDBStatusObserver();
		
		XEnvStruct temp_loader;
		temp_loader.LoadFromFile( "GameServer.eop", "*", true );
		DB().InitGameDB(	nThreadCount,
							ENV().GetString( "db.user.connection_string", user_connection_string.c_str() ).c_str(),
							temp_loader.GetString( "db.user.account", "" ).c_str(),
							XStrZlibWithSimpleCipherUtil::Decrypt( temp_loader.GetString( "db.user._password", "" ).c_str() ).c_str() );

		DB().StartThread( nThreadCount );

	}
	catch( XException & ex )
	{
		FILELOG( "ERROR: %s\n", ex.what() );
		_cprint( "ERROR: %s\n", ex.what() );
	}

	_cprint( "Loading max Item index...  " );
	DB().Push( new DB_OnStartUp() );
	while( ( DB().GetActiveThreadCount() + DB().GetWaitingWorkCount() ))
	{
		Sleep( 0 );
	}
	FILELOG( "Loading max Item index...  (%I64d)  ok\n", StructPlayer::GetMaxItemUID()  );
	_cprint( "(%I64d)  ok\n", StructPlayer::GetMaxItemUID() );

	extern myDC g_DC;
	extern HWND hWnd;
	g_DC.Init( hWnd );

	FILELOG( "Loading Complete.\n" );
}

void DeInitGame()
{
	InstanceDungeonManager::Instance().DeInit();
	RankingManager::Instance().DeInit();
	CompeteManager::Instance().DeInit();
	RoamingManager::Instance().DeInit();
	ScheduledCommandManager::Instance().DeInit();
	BattleArenaManager::Instance().DeInit();
	HuntaholicManager::Instance().DeInit();
	DungeonManager::Instance().DeInit();
	ItemCollector::Instance().DeInit();
	GuildManager::GetInstance().DeInit();
	PartyManager::GetInstance().DeInit();
	StructEventItemManager::GetInstance().DeInit();
	WorldLocationManager::Instance().DeInit();
	CPartyMatchingManager::GetInstance()->Destroy();

	// 플레이어 이름 캐쉬 초기화
	StructPlayer::ClearPlayerName();

	g_pArcadia->SetObjectPriority( &ItemCollector::Instance(), ArSchedulerObject::UPDATE_PRIORITY_IDLE );

	g_pIOCP->EndThreadPool();
	g_pIOCP->DeInit();

	// 보안 솔루션은 IOCP 이벤트가 더 이상 발생하지 않게 된 이후에 역초기화해야 함(역초기화 이후에 클라/서버 인증 요청 들어오면 대략 난감)
	XSecuritySolutionManager::Instance().Deinit();

	extern myDC g_DC;
	g_DC.DeInit();

	Sleep( 200 );

	unsigned x;

	for( x = 0; x < g_vItem.size(); x++ )
	{	
		StructItem::PendFreeItem( g_vItem[x] );
	}
/*
	for( x = 0; x < g_vNPC.size(); x++ )
	{
		//g_pArcadia->DeleteObject( g_vNPC[x] );
	}
*/
	for( x = 0; x < g_vPlayer.size(); x++ )
	{
		g_pArcadia->DeleteObject( g_vPlayer[x] );
	}

	StructItem::DeInitItemSystem();
	StructCreature::DeInitCreatureSystem();

	g_pArcadia->DeInit();
}

void GameProc()
{	
	extern void Draw();
	Draw();
}