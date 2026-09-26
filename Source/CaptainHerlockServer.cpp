
#include <process.h>

#include <network/IAcceptor.h>
#include <network/IConnection.h>

#include <framework/ArcadiaFramework.h>
#include <mmo/ArcadiaServer.h>
#include <mmo/ArTime.h>
#include <logging/FileLog.h>
#include <toolkit/XBossWorker.h>
#include <toolkit/XConsole.h>
#include <toolkit/XEnv.h>
#include <toolkit/XModuleInfo.h>
#include <dump/XExceptionHandler.h>
#include <toolkit/XRandom.h>
#include <toolkit/XStringUtil.h>
#include <network/XNetworkUtil.h>

#include "LogClient/LogClient.h"

#include "LuaVM.h"

#include "ScriptNPC.h"
#include "ScriptItem.h"
#include "ScriptMisc.h"
#include "ScriptCommon.h"
#include "ScriptPlayer.h"
#include "ScriptGuild.h"
#include "ScriptQuest.h"

#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructMonster.h"
#include "StructItem.h"

#include "AuctionManager.h"
#include "DungeonManager.h"
#include "FieldPropManager.h"
#include "ScheduledCommandManager.h"
#include "StructEventItemManager.h"
#include "RoamingManager.h"
#include "HuntaholicManager.h"
#include "CompeteManager.h"
#include "RankingManager.h"
#include "InstanceDungeonManager.h"
#include "BattleArenaManager.h"

#include "GameNetwork.h"
#include "CommunityLoader.h"
#include "ContentLoader.h"
#include "LocationLoader.h"
#include "GameAllocator.h"
#include "DBAllocator.h"
#include "SendMessage.h"
#include "XSecuritySolutionManager.h"

#include "DBPerformanceTracker.h"
#include "SchedulingPerformanceTracker.h"
#include "NetworkReceiverPerformanceTracker.h"
#include "ADOConnection.h"

#include <external_lib_include.h>
#include <internal_lib_all_include.h>


__declspec( thread ) XSEH::THREAD_INFO	s_ThreadInfo;
volatile bool g_bPerformanceTracing = true;
volatile DWORD g_nIntervalTimeToLogPerformance = 10000; // default: 10초마다 기록
bool b_performanceUpdate;

unsigned __stdcall performanceLogFunc( void* );

void	WriteHeapUsageInfo( HANDLE hFile )
{
	DWORD dwWritten = 0;
	std::string strMemoryPoolSize;

	XStringUtil::Format( strMemoryPoolSize, "PlayerHeapSize      : %d kb\r\n", GetPlayerHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "MonsterHeapSize     : %d kb\r\n", GetMonsterHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "ItemHeapSize        : %d kb\r\n", GetItemHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "SummonHeapSize      : %d kb\r\n", GetSummonHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "PetHeapSize         : %d kb\r\n", GetPetHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "SkillHeapSize       : %d kb\r\n", GetSkillHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "QuestHeapSize       : %d kb\r\n", GetQuestHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "TitleHeapSize       : %d kb\r\n", GetTitleHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "TitleConditionHeapSize: %d kb\r\n", GetTitleConditionHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "ProcHeapSize        : %d kb\r\n", GetProcHeapSize() );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
	XStringUtil::Format( strMemoryPoolSize, "DBHeapSize          : %d kb\r\n\r\n", GetDBHeap().GetAllocSize() >> 10 );
	WriteFile( hFile, strMemoryPoolSize.c_str(), static_cast< DWORD >( strMemoryPoolSize.length() ), &dwWritten, NULL );
}

void WINAPI CaptainHerlockExceptionFilter( HANDLE hFile )
{
	g_DBTracker.dump();

	std::string strVersionInfo;
	DWORD dwWritten;

	XModuleInfo cModuleInfo;
	cModuleInfo.Load( ::GetCurrentProcess() );
	char szMyVersion[64] = "";
	const WORD* pFileVerion = cModuleInfo.FileVersion();
	s_sprintf( szMyVersion, _countof( szMyVersion ), "%d.%d.%d.%d", pFileVerion[0], pFileVerion[1], pFileVerion[2], pFileVerion[3] );

	XStringUtil::Format( strVersionInfo,	"Server Version      : v%s\r\n", szMyVersion );
	WriteFile( hFile, strVersionInfo.c_str(), static_cast< DWORD >( strVersionInfo.length() ), &dwWritten, NULL );

	WriteHeapUsageInfo( hFile );
}

volatile int	g_bUseLockDelayLogging = 0;

struct _HerlockIntf : ArcadiaFrameworkIntf
{
	bool onInit( HWND hWnd );
	bool onMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	bool onKeyDown( int nVkCode );
	bool onCommand( const char* szTarget,
					const char* szFrom,
					const char *szFullCmd,
					std::vector< std::string > & vList,
					ArcadiaCommandResultReceiver & receiver );
	bool onDeInit();
	
	void onDrawUser( HDC dc );

} myFrameworkIntf;

unsigned __stdcall initFunc( void* )
{
	XSetThreadName( -1, "GameLoader" );

	CoInitialize(NULL);

	InitGameHeap();
	InitDBHeap();

	XSEH::SetUserFunc( CaptainHerlockExceptionFilter );

	StructItem::InitItemSystem();
	StructCreature::InitCreatureSystem();

	XBossWorker worker;

	void InitArcadia();
	InitArcadia();

	void LoadResource();
	LoadResource();

	ResourceManager::Init();

	MonsterLoader						monster_loader;
	PetLoader							pet_loader;
	ItemLoader							item_loader;
	EventAreaHandlerLoader				event_area_loader;
	MapLoader							map_loader;
	SkillLoader							skill_loader;
	StringLoader						string_loader;
	MixLoader							mix_loader;
	BitmapLoader						bitmap_loader;
	PartyLoader							party_loader;
	GuildLoader							guild_loader;
	ETCLoader							etc_loader;
	QuestLoader							quest_loader;
	LocationLoader						location_loader;
	NPCLoader							npc_loader;
	ChannelLoader						channel_loader;
	FieldPropLoader 					prop_loader;
	FieldPropSwitchingLoader			prop_switching_loader;
	FieldPropDataLoader					prop_data_loader;
	DungeonLoader						dungeon_loader;
	ScheduledCommandLoader				scheduled_command_loader;
	AuctionCategoryLoader				auction_category_loader;
	AutoAuctionLoader					auto_auction_loader;
	AutoAuctionRegistrationInfoLoader	auto_auction_reg_info_loader;
	AuctionLoader						auction_loader;
	RoamingLoader						roaming_loader;
	HuntaholicLoader					huntaholic_loader;
	RankingLoader						ranking_loader;
	DeathmatchLoader					deathmatch_loader;
	CreatureEnhanceLoader				creature_enhance_loader;
	CreatureFarmLoader					creature_farm_loader;
	InstanceDungeonLoader				instance_dungeon_loader;
	TitleLoader							title_loader;
	BattleArenaLoader					battle_arena_loader;
	AwakenLoader						awaken_loader;
	RandomItemLoader					random_item_loader;

	void thread_com_init_func( int nThreadNum );
	// 워커 쓰레드 생성
	worker.StartThread( "GameLoader", 4, THREAD_PRIORITY_NORMAL, thread_com_init_func );

	// 스트링 테이블은 미리 읽어야함
	string_loader.onProcess( 0 );

	worker.Push( &skill_loader  );
	worker.Push( &item_loader );
	worker.Push( &auction_category_loader );
	worker.Push( &event_area_loader );

	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );

	worker.Push( &monster_loader );			// skill_loader 완료 필요
	worker.Push( &bitmap_loader );
	worker.Push( &map_loader );
	worker.Push( &auto_auction_loader );	// auction_category_loader 완료 필요

	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );

	worker.Push( &etc_loader );				// skill_loader 완료 필요
	worker.Push( &quest_loader );
	worker.Push( &location_loader );
	worker.Push( &auto_auction_reg_info_loader );	// auto_auction_loader 완료 필요

	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );

	worker.Push( &guild_loader );
	worker.Push( &channel_loader );		// map_loader 완료 필요
	worker.Push( &dungeon_loader );		// map_loader 완료 필요
	worker.Push( &mix_loader );

	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );
	
	worker.Push( &prop_loader );		// channel_loader 완료 필요
	worker.Push( &roaming_loader );		// channel_loader 완료 필요
	worker.Push( &prop_switching_loader );
	worker.Push( &creature_enhance_loader );

	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );

	worker.Push( &party_loader );
	worker.Push( &prop_data_loader );		// prop_loader, prop_switching_loader 완료 필요
	worker.Push( &pet_loader );
	
	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );

	worker.Push( &auction_loader );		// auction_category_loader, item_loader, skill_loader, state_loader, etc_loader(소환수 로딩 관련 스텟 정보 및 소환수 강화 정보 필요), pet loader, monster_loader 완료 필요
	worker.Push( &scheduled_command_loader );
	worker.Push( &npc_loader );			// string_loader, channel_loader, roaming_loader 완료 필요
	
	worker.Push( &huntaholic_loader );	// channel_loader 완료 필요
	
	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );

	worker.Push( &ranking_loader );
	worker.Push( &deathmatch_loader );
	worker.Push( &creature_farm_loader );
	worker.Push( &instance_dungeon_loader );

	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );

	worker.Push( &title_loader );
	worker.Push( &battle_arena_loader );
	worker.Push( &awaken_loader );
	worker.Push( &random_item_loader );

	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );

	// prop_data_loader의 로딩까지 완료된 후에 동작 시작해야 함
	FieldPropManager::GetInstance().Init();

	StructEventItemManager::GetInstance().Init(); // 이벤트 아이템 매니저도 로딩 함 해주고~
	// 스크립트 로드
	GameContent::LoadScript();

	// 리스폰 정보 처리
	map_loader.InitMapInfo();

	// 초기화 함수 호출
	LUA()->RunString( "on_server_init()" );
	
	while( ( worker.GetActiveThreadCount() + worker.GetWaitingWorkCount() ) ) Sleep( 50 );

	GameContent::AddNPCToWorld();
	GameContent::AddRespawnObjectToWorld();

	worker.EndThread();

	ENV().Set( "game.loading", "complete" );

	DungeonManager::Instance().Init();
	AuctionManager::Instance().Init();		// AutoAuctionRegistrationInfoLoader 완료 전에는 호출되면 안 됨

	RoamingManager::Instance().Init();

	HuntaholicManager::Instance().Init();
	BattleArenaManager::Instance().Init();

	CompeteManager::Instance().Init();

	RankingManager::Instance().Init();

	InstanceDungeonManager::Instance().Init();

	ArcadiaServer::Instance().SetObjectPriority( &g_DBTracker, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	InitMessageLogger();

	return 0;
}

void InitDefaultEnv()
{
	XModuleInfo cModuleInfo;
	cModuleInfo.Load( ::GetCurrentProcess() );
	char szMyVersion[64] = "";
	const WORD* pFileVerion = cModuleInfo.FileVersion();
	s_sprintf( szMyVersion, _countof( szMyVersion ), "%d.%d.%d.%d", pFileVerion[0], pFileVerion[1], pFileVerion[2], pFileVerion[3] );

	ENV().Set( "game.server_version", szMyVersion );
	ENV().SetReadOnly( "game.server_version" );

	ENV().SetDefaultValue( "db.user.name",				"Telecaster" );
	ENV().SetDefaultValue( "db.user.server",			"cadb.nflavor.com" );

	ENV().SetDefaultValue( "db.user.thread_total",		4 ),
	ENV().SetDefaultValue( "db.user.account",			"sa" );
	ENV().SetDefaultValue( "db.user._password",			"" );

	ENV().SetDefaultValue( "db.c.name",					"Arcadia" );
	ENV().SetDefaultValue( "db.c.server",				"cadb.nflavor.com" );
	ENV().SetDefaultValue( "db.c.account",				"sa" );
	ENV().SetDefaultValue( "db.c._password",			"" );

	ENV().Bind( "game.battle_arena_reconnect_wait_duration",	(int*)&GameRule::nBattleArenaReconnectWaitDuration );

	ENV().Bind( "game.mod",								&GameRule::fPlyMod );
	ENV().Bind( "game.user_count",						(int*)&StructPlayer::GetPlayerCount() );
	ENV().Bind( "game.party_exp_rate_0",				&GameRule::fPartyEXPRate[ 0 ] );
	ENV().Bind( "game.party_exp_rate_1",				&GameRule::fPartyEXPRate[ 1 ] );
	ENV().Bind( "game.party_exp_rate_2",				&GameRule::fPartyEXPRate[ 2 ] );
	ENV().Bind( "game.party_exp_rate_3",				&GameRule::fPartyEXPRate[ 3 ] );
	ENV().Bind( "game.party_exp_rate_4",				&GameRule::fPartyEXPRate[ 4 ] );
	ENV().Bind( "game.party_exp_rate_5",				&GameRule::fPartyEXPRate[ 5 ] );
	ENV().Bind( "game.party_exp_rate_6",				&GameRule::fPartyEXPRate[ 6 ] );

	ENV().Bind( "game.item_drop_rate",					&GameRule::fItemDropRate );
	ENV().Bind( "game.no_collision_check",				&GameRule::bIsNoCollisionCheck );
	ENV().Bind( "game.skip_loading_attribute",			&GameRule::bSkipLoadingAttribute );
	ENV().Bind( "game.gold_drop_rate",					&GameRule::fGoldDropRate );
	ENV().Bind( "game.chaos_drop_rate",					&GameRule::fChaosDropRate );
	ENV().Bind( "game.exp_rate",						&GameRule::fEXPRate );
	ENV().Bind( "game.pvp_damage_rate_for_player",		&GameRule::fPVPDamageRateForPlayer );
	ENV().Bind( "game.pvp_damage_rate_for_summon",		&GameRule::fPVPDamageRateForSummon );

	ENV().Bind( "game.stamina_bonus_rate",				&GameRule::fStaminaBonusRate );
	ENV().Bind( "game.super_save_bonus_rate",			&GameRule::fSuperSaveBonusRate );
	ENV().Bind( "game.super_save_level_min_limit_0",	&GameRule::anSuperSaveLevelMinLimit[ 0 ] );
	ENV().Bind( "game.super_save_level_min_limit_1",	&GameRule::anSuperSaveLevelMinLimit[ 1 ] );
	ENV().Bind( "game.super_save_level_min_limit_2",	&GameRule::anSuperSaveLevelMinLimit[ 2 ] );
	ENV().Bind( "game.super_save_level_min_limit_3",	&GameRule::anSuperSaveLevelMinLimit[ 3 ] );
	ENV().Bind( "game.super_save_level_min_limit_4",	&GameRule::anSuperSaveLevelMinLimit[ 4 ] );
	ENV().Bind( "game.super_save_level_min_limit_5",	&GameRule::anSuperSaveLevelMinLimit[ 5 ] );
	ENV().Bind( "game.super_save_level_min_limit_6",	&GameRule::anSuperSaveLevelMinLimit[ 6 ] );
	ENV().Bind( "game.super_save_level_max_limit_0",	&GameRule::anSuperSaveLevelMaxLimit[ 0 ] );
	ENV().Bind( "game.super_save_level_max_limit_1",	&GameRule::anSuperSaveLevelMaxLimit[ 1 ] );
	ENV().Bind( "game.super_save_level_max_limit_2",	&GameRule::anSuperSaveLevelMaxLimit[ 2 ] );
	ENV().Bind( "game.super_save_level_max_limit_3",	&GameRule::anSuperSaveLevelMaxLimit[ 3 ] );
	ENV().Bind( "game.super_save_level_max_limit_4",	&GameRule::anSuperSaveLevelMaxLimit[ 4 ] );
	ENV().Bind( "game.super_save_level_max_limit_5",	&GameRule::anSuperSaveLevelMaxLimit[ 5 ] );
	ENV().Bind( "game.super_save_level_max_limit_6",	&GameRule::anSuperSaveLevelMaxLimit[ 6 ] );

	ENV().Bind("game.guild_donate_gold",				&GameRule::nGuildDonateGold);
	ENV().Bind("game.guild_buff_minute",				&GameRule::nGuildBuffMinute);

	ENV().Bind( "game.ally_pcbang_bonus_rate",					&GameRule::fAllyPCBangBonusRate );
	ENV().Bind( "game.ally_pcbang_chaos_bonus_rate",			&GameRule::fAllyPCBangChaosBonusRate );
	ENV().Bind( "game.premium_pcbang_bonus_rate",				&GameRule::fPremiumPCBangBonusRate );
	ENV().Bind( "game.premium_pcbang_chaos_bonus_rate",			&GameRule::fPremiumPCBangChaosBonusRate );
	ENV().Bind( "game.premium_pcbang_gold_bonus_drop_rate",		&GameRule::fPremiumPCBangGoldBonusDropRate );
	ENV().Bind( "game.premium_pcbang_item_bonus_drop_rate",		&GameRule::fPremiumPCBangItemBonusDropRate );
	ENV().Bind( "game.premium_pcbang_chaos_bonus_drop_rate",	&GameRule::fPremiumPCBangChaosBonusDropRate );
	ENV().Bind( "game.apply_stamina_bonus_in_premium_pcbang",	&GameRule::bApplyStaminaBonusInPremiumPCBang );

	ENV().Bind( "game.use_play_point",							&GameRule::bUsePlayPoint );
	ENV().Bind( "game.play_point_accumulate_term",				&GameRule::nPlayPointAccumulateTerm );
	ENV().Bind( "game.play_point_accumulate_amount",			&GameRule::nPlayPointAccumulateAmount );
	ENV().Bind( "game.premium_pcbang_play_point_bonus_rate",	&GameRule::fPremiumPCBangPlayPointBonusRate );
	ENV().Bind( "game.use_time_based_event_script",				&GameRule::bUseTimeBasedEventScript );
	ENV().Bind( "game.use_time_based_event_db",					&GameRule::bUseTimeBasedEventDB );
	ENV().Bind( "game.term_for_time_based_event_script",		&GameRule::nTermForTimeBasedEventScript );
	ENV().Bind( "game.term_for_time_based_event_db",			&GameRule::nTermForTimeBasedEventDB );

	ENV().Bind( "game.monster_wandering",				&GameRule::bMonsterWandering );
	ENV().Bind( "game.monster_collision_line",			&GameRule::bMonsterCollisionToLine );
	ENV().Bind( "game.monster_pathfinding",				&GameRule::bMonsterPathFinding );
	ENV().Bind( "game.log_monster_pathfinding",			&GameRule::bLogMonsterPathFinding );

	extern volatile LONG g_nMobLineCount;
	extern volatile LONG g_nMobPathFindingCount;
	extern volatile LONG g_nMobFindDuplicatePos;
	extern volatile LONG g_nMobRandomMove;
	extern volatile LONG g_nMobFirstLineCount;

	ENV().Bind( "game.LineCount", (int *) &g_nMobLineCount );
	ENV().Bind( "game.PathFindingCount", (int *) &g_nMobPathFindingCount );
	ENV().Bind( "game.FindDuplicatePos", (int *) &g_nMobFindDuplicatePos );
	ENV().Bind( "game.RandomMove", (int *) &g_nMobRandomMove );
	ENV().Bind( "game.FirstLineCount", (int *) &g_nMobFirstLineCount );

	ENV().Bind( "game.TracingGamePerformance", (bool *) &g_bPerformanceTracing );
	ENV().Bind( "game.GamePerformanceLogingInterval", (int *) &g_nIntervalTimeToLogPerformance );
	
	ENV().Bind( "game.log_scheduling_status",	&GameRule::bLogSchedulingStatus );
	ENV().Bind( "game.ProcessingPlayerCount", (int *) &g_PlayerPerformanceTracker.m_count );
	ENV().Bind( "game.ProcessingSummonCount", (int *) &g_SummonPerformanceTracker.m_count );
	ENV().Bind( "game.ProcessingMonsterCount", (int *) &g_MonsterPerformanceTracker.m_count );
	ENV().Bind( "game.ProcessingPlayerTime", (int *) &g_PlayerPerformanceTracker.m_totalTime );
	ENV().Bind( "game.ProcessingSummonTime", (int *) &g_SummonPerformanceTracker.m_totalTime );
	ENV().Bind( "game.ProcessingMonsterTime", (int *) &g_MonsterPerformanceTracker.m_totalTime );

	ENV().Bind( "game.no_skill_cooltime",				&GameRule::bIgnoreSkillCoolTime );
	ENV().Bind( "game.PKServer",						&GameRule::bIsPKServer );
	ENV().Bind( "game.PKPenaltyLevel",					&GameRule::nPKPenaltyLevel );
	ENV().Bind( "game.disable_pk_on",					&GameRule::bDisablePKOn );
	ENV().Bind( "game.AdultServer",						&GameRule::bIsAdultServer );
	ENV().Bind( "game.restrict_special_char",			&GameRule::bRestrictSpeicialChar );
	ENV().Bind( "game.allowed_special_char",			&GameRule::strAllowedSpecialChar );
	ENV().Bind( "game.AutoOpenOnly",					&GameRule::bAutoOpen );

	ENV().Bind( "game.disable_huntaholic",				&GameRule::bDisableHuntaholic );

	ENV().Bind( "game.use_auto_jail",					&GameRule::bUseAutoJail );

	ENV().Bind( "game.security_solution_type",				&GameRule::nSecuritySolutionType );
	ENV().Bind( "game.period_of_security_solution_check",	(int *)&GameRule::nPeriodOfSecuritySolutionCheck );
	ENV().Bind( "game.security_solution_response_timeout",	(int *)&GameRule::nSecuritySolutionResponseTimeout );
	ENV().Bind( "game.security_solution_exceptional_ip",	&GameRule::strSecuritySolutionExceptionalIP );

	ENV().Bind( "game.disable_dungeon_raid_siege",		&GameRule::bDisableDungeonRaidSiege );
	ENV().Bind( "game.cash_usable_server",				&GameRule::bIsCashUsableServer );
	ENV().Bind( "game.use_account_authority_db",		&GameRule::bUseAccountAuthorityDB );
	ENV().Bind( "game.cash_item_dropable",				&GameRule::bCashItemDropable );
	ENV().Bind( "game.use_auto_trap",					&GameRule::bUseAutoTrap );
	ENV().Bind( "game.max_level",						&GameRule::nMaxLevel );

	ENV().Bind( "game.base_ethereal_durability_consumption_on_normal_attack",	&GameRule::nEtherealDurabilityBaseConsumptionOnNormalAttack );
	ENV().Bind( "game.base_ethereal_durability_consumption_on_skill_attack",	&GameRule::nEtherealDurabilityBaseConsumptionOnSkillAttack );
	ENV().Bind( "game.base_ethereal_durability_consumption_on_damage",			&GameRule::nEtherealDurabilityBaseConsumptionOnDamage );

	ENV().Bind( "game.broadcast_event_item_pickup",		&GameRule::bBroadcastEventItemPickup );
	ENV().Bind( "game.use_guild_donation_point",		&GameRule::bUseGuildDonationPoint );
	ENV().Bind( "game.restrict_banword_for_booth",		&GameRule::bRestrictBanWordForBooth );
	ENV().Bind( "game.min_booth_startable_level",		&GameRule::nMinBoothStartableLevel );
	ENV().Bind( "game.limit_booth_openable_layer_to_zero",		&GameRule::bLimitBoothOpenableLayerToZero );
	ENV().Bind( "game.disable_buy_booth",				&GameRule::bDisableBuyBooth );
	ENV().Bind( "game.disable_booth",					&GameRule::bDisableBooth );
	ENV().Bind( "game.disable_trade",					&GameRule::bDisableTrade );

	ENV().Bind( "game.limit_adv_chat_count",			&GameRule::bLimitAdvChatCount );
	ENV().Bind( "game.min_global_chat_usable_level",	&GameRule::nMinGlobalChatUsableLevel );
	ENV().Bind( "game.max_storage_item_count",			&GameRule::nMaxStorageItemCount );
	ENV().Bind( "game.max_characters_per_account",		&GameRule::nMaxCharactersPerAccount );
	ENV().Bind( "game.auction_search_request_min_interval",		(int *)&GameRule::nAuctionSearchRequestMinInterval );
	ENV().Bind( "game.auction_process_request_min_interval",	(int *)&GameRule::nAuctionProcessRequestMinInterval );

	ENV().Bind( "game.use_security",					&GameRule::bUseSecurityNo );
	ENV().Bind( "game.use_storage_security",			&GameRule::bUseSecurityNoForStorage );
	ENV().Bind( "game.use_delete_security",				&GameRule::bUseSecurityNoForDeletingCharacter );
	ENV().Bind( "game.check_storage_security_always",	&GameRule::bCheckStorageSecurityAlways );

	ENV().Bind( "game.limit_field_logout",				&GameRule::bLimitFieldLogout );
	ENV().Bind( "game.logout_timer",					(int *)&GameRule::nLogoutTimer );
	ENV().Bind( "game.force_unregister_account_on_kick_fail",	&GameRule::bForceUnregisterAccountOnKickFail );

	ENV().Bind( "game.forbidden_script_init",			&GameRule::bForbiddenScriptInitialized );
	ENV().Bind( "game.forbidden_script",				&GameRule::strForbiddenScript );

	ENV().Bind( "game.use_login_logout_debug",			&GameRule::bUseLoginLogoutDebug );
	
	ENV().Bind( "game.log_vulcanus_dungeon",			&GameRule::bLogVulcanusDungeon );

	ENV().Bind( "game.limit_game_time" ,				&GameRule::bLimitGameTime );
	ENV().Bind( "game.max_game_time_limited_age" ,		&GameRule::nMaxGameTimeLimitedAge );
	ENV().Bind( "game.max_healthy_game_time" ,			(int *)&GameRule::nMaxHealthyGameTime );
	ENV().Bind( "game.max_tired_game_time" ,			(int *)&GameRule::nMaxTiredGameTime );

	ENV().Bind( "game.log_required_state_list",			&GameRule::strLogRequiredStateList );
	ENV().Bind( "game.log_required_item_list",			&GameRule::strLogRequiredItemList );

	ENV().Bind( "game.farm_normal_summon_exp",			&GameRule::nFarmNormalSummonEXP );
	ENV().Bind( "game.farm_growth_summon_exp",			&GameRule::nFarmGrowthSummonEXP );
	ENV().Bind( "game.farm_growth_summon_exp",			&GameRule::nFarmEvolveSummonEXP );
	ENV().Bind( "game.farm_growth_summon_exp",			&GameRule::nPremiumFarmNormalSummonEXP );
	ENV().Bind( "game.farm_growth_summon_exp",			&GameRule::nPremiumFarmGrowthSummonEXP );
	ENV().Bind( "game.farm_growth_summon_exp",			&GameRule::nPremiumFarmEvolveSummonEXP );

	ENV().Bind( "game.summon_exp_penalty_level",		&GameRule::SummonExpPenaltyLevel);
	ENV().Bind( "game.party_exp_penalty_level",			&GameRule::PartyExpPenaltyLevel);
	ENV().Bind( "game.party_exp_penalty_level_cutoff",	&GameRule::PartyExpPenaltyLevelCutoff);
	ENV().Bind( "game.taming_success_rate_2",			&GameRule::fTamingSuccessRate2);
	ENV().Bind( "game.taming_success_rate_3",			&GameRule::fTamingSuccessRate3);
	ENV().Bind( "game.taming_success_rate_4",			&GameRule::fTamingSuccessRate4);
	ENV().Bind( "game.taming_success_rate_5",			&GameRule::fTamingSuccessRate5);
	ENV().Bind( "game.gm_never_fail",					&GameRule::bGMsNeverFail);

	extern volatile LONG g_nDropRespawn;
	extern volatile LONG g_nRespawnObjectCnt;
	extern volatile LONG g_nRespawnCnt;
	extern volatile LONG g_nRespawnTryCnt;
	extern volatile int g_nCurrentLocalFlag;
	extern volatile int g_bUseLockDelayLogging;
	extern volatile int g_bIgnoreRandomDamage;
	
	ENV().Bind( "game.monster_drop_respawn",			(int *)	&g_nDropRespawn );
	ENV().Bind( "game.monster_respawn_object_cnt",		(int *)	&g_nRespawnObjectCnt );
	ENV().Bind( "game.monster_respawn_cnt",				(int *)	&g_nRespawnCnt );
	ENV().Bind( "game.monster_respawn_try_cnt",			(int *)	&g_nRespawnTryCnt );
	ENV().Bind( "game.local_flag",						(int *) &g_nCurrentLocalFlag );
	ENV().Bind( "game.bUseLockDelayLogging",			(int *) &g_bUseLockDelayLogging );
	ENV().Bind( "game.ignore_random_damage",			(int *) &g_bIgnoreRandomDamage );

	ENV().Bind( "memory.player_heap_size",				GetPlayerHeapSize );
	ENV().Bind( "memory.monster_heap_size",				GetMonsterHeapSize );
	ENV().Bind( "memory.item_heap_size",				GetItemHeapSize );
	ENV().Bind( "memory.summon_heap_size",				GetSummonHeapSize );

	ENV().Bind("game.HardcoreServer", &GameRule::Hardcore);
	ENV().Bind("game.HardcoreServerXPRate", &GameRule::HardcoreExpRate);
}

void InitLua()
{
	// init lua VM
	_cprint( "LUA initialize...   " );
	if( LuaVM::Inst()->Init() )
	{
		_cprint( "ok\n" );
		FILELOG( "LUA initialize...   ok" );
	}
	else
	{
		_cprint( "Failed\n" );
		FILELOG( "LUA initialize...   failed" );
	}

	/*
	_cprint( "LUA main thread initialize...   " );
	LUA()->InitThread() ? _cprint( "ok\n" ) : _cprint( "Failed\n" );
	*/

	// 스크립트 등록	

	// 아이템 관련
	LUA()->RegisterFunction( "insert_item", 			SCRIPT_InsertItem			);	
//	LUA()->RegisterFunction( "insert_item_target",		SCRIPT_InsertItemTarget		);
//	LUA()->RegisterFunction( "insert_item_near",		SCRIPT_InsertItemNear		);
//	LUA()->RegisterFunction( "insert_item_all",			SCRIPT_InsertItemAll		);
	LUA()->RegisterFunction( "delete_item", 			SCRIPT_DeleteItem			);
	LUA()->RegisterFunction( "putoff_item", 			SCRIPT_PutoffItem			);
	LUA()->RegisterFunction( "is_erasable_item",		SCRIPT_IsErasableItem		);
	LUA()->RegisterFunction( "find_item",				SCRIPT_FindItem				);
	LUA()->RegisterFunction( "drop_item", 				SCRIPT_DropItem				);
	LUA()->RegisterFunction( "drop_gold", 				SCRIPT_DropGold				);
	LUA()->RegisterFunction( "insert_gold", 			SCRIPT_InsertGold			);	
	LUA()->RegisterFunction( "has_item", 				SCRIPT_HasItem				);
	LUA()->RegisterFunction( "get_item_handle",			SCRIPT_GetItemHandle		);	
	LUA()->RegisterFunction( "get_item_handle_list",	SCRIPT_GetItemHandleList	);
	LUA()->RegisterFunction( "get_item_name", 			SCRIPT_GetItemName			);
	LUA()->RegisterFunction( "get_item_code", 			SCRIPT_GetItemCode			);
//	LUA()->RegisterFunction( "get_item_level", 			SCRIPT_GetItemLevel			);
	LUA()->RegisterFunction( "set_item_level", 			SCRIPT_SetItemLevel			);
	LUA()->RegisterFunction( "get_item_level", 			SCRIPT_GetItemLevel			);
	LUA()->RegisterFunction( "set_item_enhance", 		SCRIPT_SetItemEnhance		);
	LUA()->RegisterFunction( "set_item_enhance_fail",	SCRIPT_SetItemEnhanceFail	);
	LUA()->RegisterFunction( "get_item_enhance", 		SCRIPT_GetItemEnhance		);
	LUA()->RegisterFunction( "get_item_grade",	 		SCRIPT_GetItemGrade			);
	LUA()->RegisterFunction( "get_item_rank", 			SCRIPT_GetItemRank			);
	LUA()->RegisterFunction( "get_item_price", 			SCRIPT_GetItemPrice			);
	LUA()->RegisterFunction( "get_item_endurance",		SCRIPT_GetItemEndurance		);
	LUA()->RegisterFunction( "set_item_endurance",		SCRIPT_SetItemEndurance		);
	LUA()->RegisterFunction( "get_socket_info",			SCRIPT_GetItemSocketCode	);
	LUA()->RegisterFunction( "set_socket_info",			SCRIPT_SetItemSocketCode	);
	LUA()->RegisterFunction( "set_item_element",		SCRIPT_SetItemElement		);
	LUA()->RegisterFunction( "set_item_element_parameter",	SCRIPT_SetItemElementParameter );
	LUA()->RegisterFunction( "get_item_name_by_code", 	SCRIPT_ItemName				);
	LUA()->RegisterFunction( "get_item_name_id",		SCRIPT_GetItemNameId		);
	LUA()->RegisterFunction( "get_wear_item_handle", 	SCRIPT_GetWearItemHandle	);
//	LUA()->RegisterFunction( "get_item_in_belt",		SCRIPT_GetItemInBelt);
	LUA()->RegisterFunction( "get_item_appearance_code",SCRIPT_GetItemAppearanceCode	);
	LUA()->RegisterFunction( "set_item_appearance_code",SCRIPT_SetItemAppearanceCode	);
	LUA()->RegisterFunction( "insert_awaken",			SCRIPT_InsertAwakenOption );
	LUA()->RegisterFunction( "delete_awaken",			SCRIPT_DeleteAwakenOption );
	LUA()->RegisterFunction( "identify_item",			SCRIPT_IdentifyItemForRandomOption );
	LUA()->RegisterFunction( "insert_summon_by_summon_id",	SCRIPT_InsertSummonBySummonID );
	LUA()->RegisterFunction( "insert_summon_by_monster_id",	SCRIPT_InsertSummonByMonsterID );
	LUA()->RegisterFunction( "change_item_code",		SCRIPT_ChangeItemCode );
	LUA()->RegisterFunction( "get_item_random_option",	SCRIPT_GetItemIdentifiedOption );
	LUA()->RegisterFunction( "set_item_random_option",	SCRIPT_SetItemIdentifiedOption );
	LUA()->RegisterFunction( "pick_item_in_drop_group",	SCRIPT_PickItemInDropGroup );

	// 에테리얼 내구도 관련
	LUA()->RegisterFunction( "get_item_ethereal_durability",		SCRIPT_GetItemEtherealDurability	);
	LUA()->RegisterFunction( "set_item_ethereal_durability",		SCRIPT_SetItemEtherealDurability	);
	LUA()->RegisterFunction( "get_max_item_ethereal_durability",	SCRIPT_GetMaxItemEtherealDurability	);

	// 소울스톤 관련
	LUA()->RegisterFunction( "show_soulstone_craft_window",			SCRIPT_ShowSoulStoneCraftWindow		);
	LUA()->RegisterFunction( "get_item_soulstone_endurance",		SCRIPT_GetItemSoulStoneEndurance	);
	LUA()->RegisterFunction( "get_max_item_soulstone_endurance",	SCRIPT_GetMaxItemSoulStoneEndurance	);
	LUA()->RegisterFunction( "show_soulstone_repair_window",		SCRIPT_ShowSoulStoneRepairWindow	);

	// 아이템 드랍 이벤트 관련
	LUA()->RegisterFunction( "refresh_event_drop",		SCRIPT_RefreshEventDropInfo	);
	LUA()->RegisterFunction( "start_event_drop",		SCRIPT_StartEventDrop		);
	LUA()->RegisterFunction( "stop_event_drop",			SCRIPT_StopEventDrop		);
//	LUA()->RegisterFunction( "show_event_drop",			SCRIPT_ShowEventDrop		);
//	LUA()->RegisterFunction( "update_event_drop",		SCRIPT_UpdateEventDrop);
//	LUA()->RegisterFunction( "insert_event_drop",		SCRIPT_InsertEventDrop);

	// 던전 드랍율 이벤트 관련
	LUA()->RegisterFunction( "start_event_dungeon_droprate",		SCRIPT_StartEventDungeonDroprate );
	LUA()->RegisterFunction( "stop_event_dungeon_droprate",			SCRIPT_StopEventDungeonDroprate );
	LUA()->RegisterFunction( "refresh_event_dungeon_droprate",		SCRIPT_RefreshEventDungeonDroprate );

	// 아이템 지급 이벤트 관련
	LUA()->RegisterFunction( "event_supply",			SCRIPT_EventSupply			);
	LUA()->RegisterFunction( "refresh_event_supply",	SCRIPT_RefreshEventSupplyInfo	);
	LUA()->RegisterFunction( "start_event_supply",		SCRIPT_StartEventSupply		);
	LUA()->RegisterFunction( "stop_event_supply",		SCRIPT_StopEventSupply		);
	LUA()->RegisterFunction( "supply_event_item",		SCRIPT_SupplyEventItem		);

	// 프로퍼티 관련
	LUA()->RegisterFunction( "get_target_value",		SCRIPT_GetTargetValue		);
	LUA()->RegisterFunction( "gtv",						SCRIPT_GetTargetValue		);
	LUA()->RegisterFunction( "get_handle_value",		SCRIPT_GetHandleValue		);
	LUA()->RegisterFunction( "ghv",						SCRIPT_GetHandleValue		);
	LUA()->RegisterFunction( "get_value", 				SCRIPT_GetValue				);
	LUA()->RegisterFunction( "add_value", 				SCRIPT_AddValue				);
	LUA()->RegisterFunction( "add_exp_jp",				SCRIPT_AddExpJP				);
	LUA()->RegisterFunction( "set_value", 				SCRIPT_SetValue				);
	LUA()->RegisterFunction( "echo_value",				SCRIPT_EchoValue			);
	LUA()->RegisterFunction( "gv",						SCRIPT_GetValue				);
	LUA()->RegisterFunction( "av",						SCRIPT_AddValue				);
	LUA()->RegisterFunction( "sv",						SCRIPT_SetValue				);
	LUA()->RegisterFunction( "ev",						SCRIPT_EchoValue			);
	LUA()->RegisterFunction( "get_all_value", 			SCRIPT_GetAllValue			);		
	LUA()->RegisterFunction( "set_creature_value",		SCRIPT_SetCreatureValue		);
	LUA()->RegisterFunction( "get_creature_value",		SCRIPT_GetCreatureValue		);
	LUA()->RegisterFunction( "change_creature_name",	SCRIPT_ChangeCreatureName	);	
	LUA()->RegisterFunction( "scv",						SCRIPT_SetCreatureValue		);
	LUA()->RegisterFunction( "smcv",					SCRIPT_SetMainCreatureValue	);
	LUA()->RegisterFunction( "gcv",						SCRIPT_GetCreatureValue		);
	LUA()->RegisterFunction( "gmcv",					SCRIPT_GetMainCreatureValue	);
	LUA()->RegisterFunction( "emcv",					SCRIPT_EchoMainCreatureValue);

	// 플레이어 플래그 관련
	LUA()->RegisterFunction( "get_flag", 				SCRIPT_GetPlayerFlag		);
	LUA()->RegisterFunction( "set_flag", 				SCRIPT_SetPlayerFlag		);
	LUA()->RegisterFunction( "del_flag", 				SCRIPT_RemovePlayerFlag		);

	// 계정 플래그 관련
	LUA()->RegisterFunction( "get_account_flag", 		SCRIPT_GetAccountFlag		);
	LUA()->RegisterFunction( "set_account_flag", 		SCRIPT_SetAccountFlag		);
	LUA()->RegisterFunction( "del_account_flag", 		SCRIPT_RemoveAccountFlag		);

	// 대기시간 없이 PK 모드 변경
	LUA()->RegisterFunction( "set_pk_mode", 			SCRIPT_SetPKMode			);

	// 길드 연합 관련
	LUA()->RegisterFunction( "is_alliance_leader",		SCRIPT_IsAllianceLeader		);
	LUA()->RegisterFunction( "increase_max_alliance_member_count",		SCRIPT_IncreaseMaxAllianceMemberCount		);
	LUA()->RegisterFunction( "get_max_alliance_member_count",	SCRIPT_GetMaxAllianceMemberCount		);

	// NPC 대화 및 퀘스트 관련
	LUA()->RegisterFunction( "dlg_title", 				SCRIPT_DialogTitle			);
	LUA()->RegisterFunction( "dlg_text", 				SCRIPT_DialogText			);
	LUA()->RegisterFunction( "dlg_menu", 				SCRIPT_DialogMenu			);
	LUA()->RegisterFunction( "dlg_show", 				SCRIPT_DialogShow			);
	LUA()->RegisterFunction( "quest_text_without_quest_menu",	SCRIPT_QuestTextWithoutQuest);	
	LUA()->RegisterFunction( "dlg_text_without_quest_menu",	SCRIPT_DialogTextWithoutQuest);	
	LUA()->RegisterFunction( "start_quest",				SCRIPT_StartQuest			);
	LUA()->RegisterFunction( "force_start_quest",		SCRIPT_ForceStartQuest		);
	LUA()->RegisterFunction( "add_pending_quest",		SCRIPT_AddPendingQuest		);
	LUA()->RegisterFunction( "end_quest",				SCRIPT_EndQuest				);
	LUA()->RegisterFunction( "drop_quest",				SCRIPT_DropQuest			);
	LUA()->RegisterFunction( "quest_info",				SCRIPT_QuestInfo			);
	LUA()->RegisterFunction( "show_quest_info_without_npc",	SCRIPT_ShowQuestInfoWithoutNPC	);
	LUA()->RegisterFunction( "get_quest_progress",		SCRIPT_GetQuestProgress		);
	LUA()->RegisterFunction( "get_quest_status",		SCRIPT_GetQuestStatus		);
	LUA()->RegisterFunction( "set_quest_status",		SCRIPT_SetQuestStatus		);
	LUA()->RegisterFunction( "get_last_accept_quest",	SCRIPT_GetLastAcceptQuest	);
	LUA()->RegisterFunction( "reset_finished_quest",	SCRIPT_ResetFinishedQuest	);

	// 다이얼로그 관련
	LUA()->RegisterFunction( "dlg_general",				SCRIPT_DialogGeneral		);
	LUA()->RegisterFunction( "dlg_special",				SCRIPT_DialogSpecial		);

	LUA()->RegisterFunction( "kick", 							SCRIPT_Kick					);
	LUA()->RegisterFunction( "force_unregister_account",		SCRIPT_ForceUnregisterAccount	);
	LUA()->RegisterFunction( "env", 							SCRIPT_GetEnv				);
	LUA()->RegisterFunction( "add_npc", 						SCRIPT_AddMonster			);
	LUA()->RegisterFunction( "add_monster",						SCRIPT_AddMonster2);
	LUA()->RegisterFunction( "set_home",						SCRIPT_SetHome);
	LUA()->RegisterFunction( "add_instance_dungeon_monster",	SCRIPT_AddInstanceDungeonMonster );
	LUA()->RegisterFunction( "add_npc_to_world", 				SCRIPT_AddNPC				);
//	LUA()->RegisterFunction( "remove_npc",						SCRIPT_RemoveNPC);
	LUA()->RegisterFunction( "show_npc",						SCRIPT_ShowNPC);
	LUA()->RegisterFunction( "add_field_prop",					SCRIPT_AddFieldProp			);
	LUA()->RegisterFunction( "remove_field_prop",				SCRIPT_RemoveFieldProp);

	LUA()->RegisterFunction( "respawn", 						SCRIPT_AddRespawnInfo		);
	LUA()->RegisterFunction( "respawn_guardian", 				SCRIPT_AddDungeonGuardianRespawnInfo );
	LUA()->RegisterFunction( "respawn_environmental_guardian", 	SCRIPT_AddDungeonEnvironmentalGuardianRespawnInfo );
	LUA()->RegisterFunction( "respawn_rare_mob", 				SCRIPT_AddRareMobRespawnInfo );
	LUA()->RegisterFunction( "respawn_roaming_mob",				SCRIPT_AddRoamingMobRespawnInfo );
	LUA()->RegisterFunction( "raid_respawn", 					SCRIPT_AddRaidRespawnInfo		);
	LUA()->RegisterFunction( "raid_respawn_rare_mob", 			SCRIPT_AddRaidRareMobRespawnInfo);

	// 몬스터 랜덤 리젠
	LUA()->RegisterFunction( "set_random_respawn",		SCRIPT_SetRandomRespawnInfo	);
	LUA()->RegisterFunction( "add_random_area",			SCRIPT_AddRandomAreaInfo	);
	LUA()->RegisterFunction( "add_random_monster",		SCRIPT_AddRandomMonster		);

	// 랜덤 몬스터 그룹
	LUA()->RegisterFunction( "add_random_monster_group",	SCRIPT_AddRandomMonsterGroup	);

	LUA()->RegisterFunction( "kill",					SCRIPT_Kill					);
	LUA()->RegisterFunction( "kill_target",				SCRIPT_KillTarget			);

	// 시스템 날짜와 시간 관련 함수
	LUA()->RegisterFunction( "get_os_time",				SCRIPT_LuaSTDLib_Time		);
	LUA()->RegisterFunction( "get_os_date",				SCRIPT_LuaSTDLib_Date		);
//	LUA()->RegisterFunction( "tt",						SCRIPT_TimeTest				);
	LUA()->RegisterFunction( "get_minute",				SCRIPT_GetMinute			);
//	LUA()->RegisterFunction( "get_os_second",			SCRIPT_GetOsSecond			);

	LUA()->RegisterFunction( "notice", 					SCRIPT_Notice				);
	LUA()->RegisterFunction( "private_notice", 			SCRIPT_PrivateNotice		);
	LUA()->RegisterFunction( "announce",				SCRIPT_Announce				);
	LUA()->RegisterFunction( "whisper", 				SCRIPT_Whisper				);
	LUA()->RegisterFunction( "warp", 					SCRIPT_Warp					);
	LUA()->RegisterFunction( "force_warp",				SCRIPT_ForceWarp			);
	LUA()->RegisterFunction( "recall_player", 			SCRIPT_RecallPlayer			);
	LUA()->RegisterFunction( "is_in_siege_dungeon", 	SCRIPT_IsInSiegeDungeon		);
	LUA()->RegisterFunction( "respawn_guardian_object",	SCRIPT_RespawnGuardian		);	
	LUA()->RegisterFunction( "show_dungeon_stone",	 	SCRIPT_ShowDungeonStone		);
	LUA()->RegisterFunction( "change_tactical_position_owner",	SCRIPT_ChangeTacticalPositionOwner	);
	LUA()->RegisterFunction( "request_dungeon_raid",	SCRIPT_RequestDungeonRaid	);
	LUA()->RegisterFunction( "cancel_dungeon_raid",		SCRIPT_CancelDungeonRaid	);

	// 숨겨진 던전
	LUA()->RegisterFunction( "enter_secret_dungeon",	SCRIPT_EnterSecretDungeon	);
	LUA()->RegisterFunction( "warp_to_secret_dungeon",	SCRIPT_WarpToSecretDungeon	);
	// 인스턴스 던전
	LUA()->RegisterFunction( "enter_instance_dungeon",			SCRIPT_EnterInstanceDungeon		);
	LUA()->RegisterFunction( "enter_other_instance_dungeon",	SCRIPT_EnterOtherInstanceDungeon);
	LUA()->RegisterFunction( "dlg_special_menu",				SCRIPT_SpecialMenu				);
	LUA()->RegisterFunction( "warp_to_instance_dungeon",		SCRIPT_WarpToInstanceDungeon	);
	LUA()->RegisterFunction( "leave_instance_dungeon",			SCRIPT_LeaveInstanceDungeon		);
	LUA()->RegisterFunction( "quit_instance_dungeon",			SCRIPT_QuitInstanceDungeon		);
	LUA()->RegisterFunction( "get_instance_dungeon_id",			SCRIPT_GetInstanceDungeonID		);
	LUA()->RegisterFunction( "get_instance_dungeon_type_id",	SCRIPT_GetInstanceDungeonTypeID	);
	LUA()->RegisterFunction( "set_instance_dungeon_flag",		SCRIPT_SetInstanceDungeonFlag );	
	LUA()->RegisterFunction( "get_instance_dungeon_flag",		SCRIPT_GetInstanceDungeonFlag );
	LUA()->RegisterFunction( "get_alive_instance_respawn_group_monster_count",	SCRIPT_GetAliveInstanceRespawnGroupMonsterCount );
	LUA()->RegisterFunction( "do_each_player_in_instance_dungeon",				SCRIPT_DoEachPlayerInInstanceDungeon );
	// 인스턴스 던전 미션 관련(인던 진행상황 표시용 UI - 미션 UI라고 지칭함)
	// * 미션에 관련된 데이터를 서버가 따로 보관하지는 않으며 InstanceDungeonFlag를 이용해서 스크립트에서 데이터를 관리하며
	//   유저에게 방송만 함으로써 UI 갱신만 시켜주는 것으로, 데이터의 세팅/값 확인 등의 함수 없이 방송 함수만 있음
	LUA()->RegisterFunction( "send_mission_title",						SCRIPT_SendMissionTitle );
	LUA()->RegisterFunction( "send_mission_reward",						SCRIPT_SendMissionReward );
	LUA()->RegisterFunction( "send_mission_objective",					SCRIPT_SendMissionObjective );
	LUA()->RegisterFunction( "send_mission_objective_progress",			SCRIPT_SendMissionObjectiveProgress );
	LUA()->RegisterFunction( "broadcast_mission_title",					SCRIPT_BroadcastMissionTitle );
	LUA()->RegisterFunction( "broadcast_mission_reward",				SCRIPT_BroadcastMissionReward );
	LUA()->RegisterFunction( "broadcast_mission_objective",				SCRIPT_BroadcastMissionObjective );
	LUA()->RegisterFunction( "broadcast_mission_objective_progress",	SCRIPT_BroadcastMissionObjectiveProgress );
	LUA()->RegisterFunction( "broadcast_notice",						SCRIPT_BroadcastNotice );

	// 배틀 아레나 관련
	LUA()->RegisterFunction( "activate_battle_arena_prop",				SCRIPT_ActivateBattleArenaProp );
	LUA()->RegisterFunction( "set_battle_arena_instance_flag",			SCRIPT_SetBattleArenaInstanceFlag );
	LUA()->RegisterFunction( "get_battle_arena_instance_flag",			SCRIPT_GetBattleArenaInstanceFlag );
	LUA()->RegisterFunction( "do_each_player_in_battle_arena_instance",	SCRIPT_DoEachPlayerInBattleArenaInstance );
	LUA()->RegisterFunction( "do_each_player_in_battle_arena_team",		SCRIPT_DoEachPlayerInBattleArenaTeam );
	LUA()->RegisterFunction( "get_battle_arena_block_time",				SCRIPT_GetBattleArenaBlockTime );
	LUA()->RegisterFunction( "set_battle_arena_block_time",				SCRIPT_SetBattleArenaBlockTime );
	LUA()->RegisterFunction( "get_battle_arena_penalty_count",			SCRIPT_GetBattleArenaPenaltyCount );
	LUA()->RegisterFunction( "set_battle_arena_penalty_count",			SCRIPT_SetBattleArenaPenaltyCount );
	LUA()->RegisterFunction( "get_battle_arena_next_penalty_dec_time",	SCRIPT_GetBattleArenaNextPenaltyDecTime );
	LUA()->RegisterFunction( "set_battle_arena_next_penalty_dec_time",	SCRIPT_SetBattleArenaNextPenaltyDecTime );
	LUA()->RegisterFunction( "get_battle_arena_team_no",				SCRIPT_GetBattleArenaTeamNo );

	LUA()->RegisterFunction( "set_secroute_free_pass",					SCRIPT_SetSecrouteFreePass );

	LUA()->RegisterFunction( "set_chat_block_time",						SCRIPT_SetChatBlockTime );
	LUA()->RegisterFunction( "set_invisible",							SCRIPT_SetInvisible );
	LUA()->RegisterFunction( "rebirth",									SCRIPT_Rebirth );
	LUA()->RegisterFunction( "set_level",								SCRIPT_SetLevel );
	LUA()->RegisterFunction( "heal",									SCRIPT_Heal );

	LUA()->RegisterFunction( "enter_dungeon",		 	SCRIPT_WarpToDungeon		);
	LUA()->RegisterFunction( "warp_to_stored_position",	SCRIPT_WarpToStoredPosition	);
	LUA()->RegisterFunction( "begin_dungeon_raid",		SCRIPT_BeginDungeonRaid		);
	LUA()->RegisterFunction( "drop_dungeon_owner_ship",	SCRIPT_DropDungeonOwnership	);
	LUA()->RegisterFunction( "change_dungeon_owner",	SCRIPT_ChangeDungeonOwner	);
	LUA()->RegisterFunction( "clear_dungeon_core_guardian",	SCRIPT_ClearDungeonCoreGuardian	);
	LUA()->RegisterFunction( "get_own_dungeon_id",		SCRIPT_GetOwnDungeonID		);
	LUA()->RegisterFunction( "get_siege_dungeon_id",	SCRIPT_GetSiegeDungeonID	);
	LUA()->RegisterFunction( "stat", 					SCRIPT_Stat					);
	LUA()->RegisterFunction( "setspeed", 				SCRIPT_SetSpeed				);	
	LUA()->RegisterFunction( "saveall",					SCRIPT_SaveAllPlayer		);
	LUA()->RegisterFunction( "shutdown",				SCRIPT_Shutdown				);
	LUA()->RegisterFunction( "delete_block_account",	SCRIPT_DeleteFromBlockAccount );
	LUA()->RegisterFunction( "save", 					SCRIPT_SavePlayer			);
	LUA()->RegisterFunction( "respawn_near_monster",	SCRIPT_RespawnNearMonster);		
	LUA()->RegisterFunction( "monster_skill_cast",		SCRIPT_MonsterSkillCast	);
	LUA()->RegisterFunction( "get_monster_skill_target_position",	SCRIPT_GetMonsterSkillTargetPosition );
	LUA()->RegisterFunction( "set_monster_skill_target_position",	SCRIPT_SetMonsterSkillTargetPosition );
	LUA()->RegisterFunction( "set_next_attackable_time",	SCRIPT_SetNextAttackableTime	);
	LUA()->RegisterFunction( "get_monster_id",			SCRIPT_GetMonsterID	);

	LUA()->RegisterFunction( "db_trace_dump",			SCRIPT_DBTraceDump	);
	
	LUA()->RegisterFunction( "force_monster_proc_dead",	SCRIPT_ForceMonsterProcDead );
	LUA()->RegisterFunction( "open_market", 			SCRIPT_ShowMarket			);
	LUA()->RegisterFunction( "gametime", 				SCRIPT_GetGameTime			);	
	LUA()->RegisterFunction( "refresh", 				SCRIPT_Reload				);
	LUA()->RegisterFunction( "learn_skill", 			SCRIPT_LearnSkill			);
	LUA()->RegisterFunction( "learn_all_skill",			SCRIPT_LearnAllSkill		);
	LUA()->RegisterFunction( "learn_creature_all_skill",SCRIPT_LearnCreatureAllSkill);
	LUA()->RegisterFunction( "reset_skill",				SCRIPT_ResetSkill			);
	LUA()->RegisterFunction( "set_job",					SCRIPT_SetJob				);
	LUA()->RegisterFunction( "reset_job",				SCRIPT_ResetJob				);
	LUA()->RegisterFunction( "set_race",				SCRIPT_SetRace				);
	LUA()->RegisterFunction( "get_base_skill_level", 	SCRIPT_GetBaseSkillLevel	);
	LUA()->RegisterFunction( "show_creature_dialog", 	SCRIPT_ShowCreatureDialog	);
	LUA()->RegisterFunction( "sconv",		 			SCRIPT_Conv					);
	LUA()->RegisterFunction( "message", 				SCRIPT_Message				);
	LUA()->RegisterFunction( "is_changeable_job", 		SCRIPT_IsChangeableJob		);

	LUA()->RegisterFunction( "add_state",		 		SCRIPT_AddState				);
	LUA()->RegisterFunction( "get_state_level",	 		SCRIPT_GetStateLevel		);
	LUA()->RegisterFunction( "remove_state",	 		SCRIPT_RemoveState			);
	LUA()->RegisterFunction( "add_cstate",		 		SCRIPT_AddCreatureState		);
	LUA()->RegisterFunction( "remove_cstate",	 		SCRIPT_RemoveCreatureState	);
	LUA()->RegisterFunction( "add_event_state",	 		SCRIPT_AddEventState		);
	LUA()->RegisterFunction( "remove_event_state",		SCRIPT_RemoveEventState		);
	LUA()->RegisterFunction( "clear_event_state",		SCRIPT_ClearEventState		);
	LUA()->RegisterFunction( "get_event_state_list",	SCRIPT_GetEventStateList	);
	LUA()->RegisterFunction( "cast_world_state",		SCRIPT_CastWorldState		);

	LUA()->RegisterFunction( "set_env",		 			SCRIPT_SetEnv				);
	LUA()->RegisterFunction( "get_env",		 			SCRIPT_GetEnv				);

	LUA()->RegisterFunction( "set_global_variable",		SCRIPT_SetGlobalVariable	);
	LUA()->RegisterFunction( "get_global_variable",		SCRIPT_GetGlobalVariable	);
	LUA()->RegisterFunction( "del_global_variable",		SCRIPT_DeleteGlobalVariable	);
	LUA()->RegisterFunction( "sgv",						SCRIPT_SetGlobalVariable);
	LUA()->RegisterFunction( "ggv",						SCRIPT_GetGlobalVariable);
	LUA()->RegisterFunction( "dgv",						SCRIPT_DeleteGlobalVariable);

	//LUA()->RegisterFunction( "revival_summon",			SCRIPT_ResurrectSummon		);
	LUA()->RegisterFunction( "get_string",				SCRIPT_GetString			);
	LUA()->RegisterFunction( "get_creature_handle",		SCRIPT_GetCreatureHandle	);
	LUA()->RegisterFunction( "open_storage",			SCRIPT_ShowStorage			);
	LUA()->RegisterFunction( "get_user_count_near",		SCRIPT_GetUserCountNear		);
	LUA()->RegisterFunction( "create_guild",			SCRIPT_CreateGuild			);
	LUA()->RegisterFunction( "destroy_guild",			SCRIPT_DestroyGuild			);
	LUA()->RegisterFunction( "show_guild_create",		SCRIPT_ShowCreateGuild		);
	LUA()->RegisterFunction( "show_alliance_create",	SCRIPT_ShowCreateAlliance	);
	LUA()->RegisterFunction( "force_change_guild_name",	SCRIPT_ForceChangeGuildName	);
	LUA()->RegisterFunction( "force_promote_guild_leader",	SCRIPT_ForcePromoteGuildLeader	);
	LUA()->RegisterFunction( "check_valid_guild_name",	SCRIPT_CheckValidGuildName	);
	LUA()->RegisterFunction( "create_alliance",			SCRIPT_CreateAlliance		);
	LUA()->RegisterFunction( "destroy_alliance",		SCRIPT_DestroyAlliance		);
	LUA()->RegisterFunction( "check_valid_alliance_name",	SCRIPT_CheckValidAllianceName	);
	LUA()->RegisterFunction( "get_npc_id",				SCRIPT_GetNPCId				);
	LUA()->RegisterFunction( "get_npc_name",			SCRIPT_GetNPCName			);
	LUA()->RegisterFunction( "get_npc_type",			SCRIPT_GetNPCType			);
	LUA()->RegisterFunction( "get_npc_handle",			SCRIPT_GetNPCHandle			);
	LUA()->RegisterFunction( "creature_evolution",		SCRIPT_CreatureEvolution	);
	LUA()->RegisterFunction( "creature_enhance",		SCRIPT_CreatureEnhance		);
	LUA()->RegisterFunction( "call_lc_In",				SCRIPT_SetCurrentLocationId	);
	LUA()->RegisterFunction( "creature_learn_skill",	SCRIPT_CreatureLearnSkill	);
	LUA()->RegisterFunction( "get_proper_channel_num",	SCRIPT_GetProperChannelNo	);
	LUA()->RegisterFunction( "get_min_channel_num",		SCRIPT_GetMinChannelNo		);
	LUA()->RegisterFunction( "get_max_channel_num",		SCRIPT_GetMaxChannelNo		);
	LUA()->RegisterFunction( "get_user_count_in_channel",SCRIPT_GetUserCountInChannel);
	LUA()->RegisterFunction( "get_layer_of_channel",	SCRIPT_GetLayerOfChannel	);
	LUA()->RegisterFunction( "get_summon_name_id",		SCRIPT_GetSummonNameId		);
	LUA()->RegisterFunction( "equip_summon_card",		SCRIPT_EquipSummonCard		);
	LUA()->RegisterFunction( "show_channel_set",		SCRIPT_ShowChannelSet		);
	LUA()->RegisterFunction( "update_guild_info",		SCRIPT_UpdateGuildInfo		);
	LUA()->RegisterFunction( "update_guild_banner_info",	SCRIPT_UpdateGuildBannerInfo	);
	LUA()->RegisterFunction( "is_guild_leader",			SCRIPT_IsGuildLeader		);
	LUA()->RegisterFunction( "set_guild_block_time",	SCRIPT_SetGuildBlockTime	);
	LUA()->RegisterFunction( "get_guild_block_time",	SCRIPT_GetGuildBlockTime	);
	LUA()->RegisterFunction( "get_guild_name",			SCRIPT_GetGuildName);
	LUA()->RegisterFunction( "get_guild_point",			SCRIPT_GetGuildPoint);
	LUA()->RegisterFunction( "add_guild_point",			SCRIPT_AddGuildPoint);
	LUA()->RegisterFunction( "gnotice",					SCRIPT_GuildNotice);
	
	LUA()->RegisterFunction( "get_own_guild_name",		SCRIPT_GetOwnGuildName		);
	LUA()->RegisterFunction( "get_dungeon_relation",	SCRIPT_GetDungeonRelation	);
	LUA()->RegisterFunction( "get_tax_rate",			SCRIPT_GetTaxRate			);
	LUA()->RegisterFunction( "set_tax_rate",			SCRIPT_SetTaxRate			);
	LUA()->RegisterFunction( "get_tax_amount",			SCRIPT_GetTaxAmount			);
	LUA()->RegisterFunction( "get_tax_chaos_amount",	SCRIPT_GetTaxChaosAmount	);
	LUA()->RegisterFunction( "draw_tax",				SCRIPT_DrawTax				);
	LUA()->RegisterFunction( "draw_tax_chaos",			SCRIPT_DrawTaxChaos			);

#ifndef _USE_NEW_ROAMING_ONLY
	LUA()->RegisterFunction( "add_way_point",			SCRIPT_AddWayPoint			);
	LUA()->RegisterFunction( "set_way_point_type",		SCRIPT_SetWayPointType		);
	LUA()->RegisterFunction( "set_way_point_speed",		SCRIPT_SetWayPointSpeed		);
#endif

	LUA()->RegisterFunction( "recall_feather",			SCRIPT_RecallFeather		);
	LUA()->RegisterFunction( "warp_to_revive_position",	SCRIPT_WarpToRevivePosition	);
	LUA()->RegisterFunction( "get_server_category",		SCRIPT_GetServerCategory	);
	LUA()->RegisterFunction( "find_npc",				SCRIPT_FindNPC				);

	LUA()->RegisterFunction( "set_auto_user",			SCRIPT_SetAutoUser			);
	LUA()->RegisterFunction( "set_auto_account",		SCRIPT_SetAutoAccount		);
	LUA()->RegisterFunction( "clear_auto_account",		SCRIPT_ClearAutoAccount		);

	LUA()->RegisterFunction( "open_url",				SCRIPT_OpenUrl				);
	LUA()->RegisterFunction( "open_popup",				SCRIPT_OpenPopup			);
	LUA()->RegisterFunction( "open_popup_and_set_size",	SCRIPT_OpenPopupAndSetSize	);
	LUA()->RegisterFunction( "get_local_info",			SCRIPT_GetLocalInfo			);

	LUA()->RegisterFunction( "update_gold_chaos",		SCRIPT_SendGoldChaosUpdate	);

	LUA()->RegisterFunction( "show_donation_prop",		SCRIPT_ShowDonationMenu		);

	LUA()->RegisterFunction( "set_pcbang_user",			SCRIPT_SetPCBangUser		);
	LUA()->RegisterFunction( "set_continuous_play_time",	SCRIPT_SetContinuousPlayTime );

	// NPC에 의한 소환수 이름 변경 기능 관련
	LUA()->RegisterFunction( "get_creature_name_id",	SCRIPT_GetCreatureNameID	);
	LUA()->RegisterFunction( "creature_name_change_box",	SCRIPT_ShowCreatureNameChangeBox	);

	// NPC에 의한 소환수 스킬 초기화 기능 관련
	LUA()->RegisterFunction( "reset_summon_skill",		SCRIPT_ResetSummonSkill		);

	// NPC에 의한 소환수 농장 기능 관련
	LUA()->RegisterFunction( "show_creature_farm_window",	SCRIPT_ShowCreatureFarmWindow	);
	LUA()->RegisterFunction( "get_summon_list",			SCRIPT_GetSummonList		);
	LUA()->RegisterFunction( "get_farmed_summon_list",	SCRIPT_GetFarmedSummonList	);
	LUA()->RegisterFunction( "get_farmed_summon_count",	SCRIPT_GetFarmedSummonCount	);
	LUA()->RegisterFunction( "get_farmed_summon_value",	SCRIPT_GetFarmedSummonValue	);
	LUA()->RegisterFunction( "get_farmed_summon_level", SCRIPT_GetFarmedSummonLevel );
	LUA()->RegisterFunction( "get_summon_rate",			SCRIPT_GetSummonRate		);
	LUA()->RegisterFunction( "farm_summon",				SCRIPT_FarmSummon			);
	LUA()->RegisterFunction( "regain_summon",			SCRIPT_RegainSummon			);
	LUA()->RegisterFunction( "nurse_summon",			SCRIPT_NurseSummon			);

	// 호칭 관련
	LUA()->RegisterFunction( "open_title",				SCRIPT_OpenTitle			);
	LUA()->RegisterFunction( "achieve_title",			SCRIPT_AchieveTitle			);
	LUA()->RegisterFunction( "get_main_title",			SCRIPT_GetMainTitle			);
	LUA()->RegisterFunction( "set_main_title",			SCRIPT_SetMainTitle			);
	LUA()->RegisterFunction( "get_sub_title",			SCRIPT_GetSubTitle			);
	LUA()->RegisterFunction( "set_sub_title",			SCRIPT_SetSubTitle			);
	LUA()->RegisterFunction( "update_title_condition",	SCRIPT_UpdateTitleCondition );
	LUA()->RegisterFunction( "set_remain_title_time",	SCRIPT_SetRemainTitleTime	);

	// 경매 관련
	LUA()->RegisterFunction( "show_auction_window",		SCRIPT_ShowAuctionWindow	);
	LUA()->RegisterFunction( "cancel_auction_by_seller",	SCRIPT_CancelAuctionBySeller	);

	// 헌터홀릭 관련
	LUA()->RegisterFunction( "show_huntaholic_lobby_window",	SCRIPT_ShowHuntaholicLobbyWindow );
	LUA()->RegisterFunction( "set_huntaholic_point",	SCRIPT_SetHuntaholicPoint );

	// 이벤트 영역 진입 횟수 기록 관련
	LUA()->RegisterFunction( "get_event_area_enter_count",	SCRIPT_GetEventAreaEnterCount );
	LUA()->RegisterFunction( "set_event_area_enter_count",	SCRIPT_SetEventAreaEnterCount );

	// 서버 강제 덤프 생성 후 종료(카운트 및 유저 정보 저장 처리 없음)
	LUA()->RegisterFunction( "suicide",					SCRIPT_Suicide				);

	// ScheduledCommandManager priority 설정 함수
	LUA()->RegisterFunction( "set_scheduled_command_manager_priority",	SCRIPT_SetScheduledCommandManagerPriority );

	//	인벤토리 아이템 삭제 (GM 본인 인벤토리 아이템 삭제)
	LUA()->RegisterFunction( "clear_inventory",				SCRIPT_ClearInventory		);

	// 계정 관련
	LUA()->RegisterFunction( "set_account_authority",		SCRIPT_SetAccountAuthority	);

//	LUA()->RegisterFunction("update_wear", SCRIPT_UpdateWear);


	StructSummon::BindProperty();
	StructPlayer::BindProperty();
	StructMonster::BindProperty();
	
	// 출력방향 설정
	LUA()->BindLogOutputFunction( ScriptLog );	
}

bool _HerlockIntf::onInit( HWND hWnd )
{
	std::string szTitle = ENV().GetString("app.name", "Rappelz Server");

#ifdef USE_CLIENT97
	szTitle += " (Epic 9.7)";
#elif defined USE_CLIENT96
	szTitle += " (Epic 9.6)";
#elif defined USE_CLIENT952
	szTitle += " (Epic 9.52)";
#elif defined USE_CLIENT94
	szTitle += " (Epic 9.4)";
#else
	szTitle += " (Epic 9.1)";
#endif

#ifdef _DEBUG
	szTitle += " Debug Build";
#endif

	SetWindowText( hWnd, szTitle.c_str() );

	FileLogHandler::Init();
	g_DBTracker.init();

	// init COM
	CoInitialize(NULL);

	// init random
	srand( GetArTime() );

	// init network
	_cprint( "Network initialize...   " );
	if( XNetworkUtil::InitNetwork() )
	{
		_cprint( "ok\n" );
		FILELOG( "Network initialize...   ok" );
	}
	else
	{
		_cprint( "Failed\n" );
		FILELOG( "Network initialize...   failed" );
	}

	void InitNetwork();
	InitNetwork();

	// init script
	InitLua();

	if( !XSecuritySolutionManager::Instance().Init() )
	{
		return true;
	}

	unsigned id;
	HANDLE hThread;
	hThread = reinterpret_cast<HANDLE>( _beginthreadex( NULL, 0, initFunc, NULL, 0, &id ) );
	CloseHandle( hThread );

	b_performanceUpdate = true;
	hThread = reinterpret_cast<HANDLE>( _beginthreadex( NULL, 0, performanceLogFunc, NULL, 0, &id ) );
	CloseHandle( hThread );

	// set timer
	SetTimer( ArcadiaFrameworkIntf::GetViewWindow(), 0, 100, NULL );

	// init log
	LOG::Init( ENV().GetString( "app.name", "CA Game Server" ).c_str() );
	LOG::Log11N4S( LM_SERVER_INFO, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", 0,  "", 0, "", 0, "START", LOG::STR_NTS );

	return true;
}

bool _HerlockIntf::onDeInit()
{
	LOG::Log11N4S( LM_SERVER_INFO, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "", 0,  "", 0, "", 0, "END", LOG::STR_NTS );

	KillTimer( ArcadiaFrameworkIntf::GetViewWindow(), 0 );

	int nShutdownState = ShutdownManager::Instance().m_nShutdownState;
	if( !( nShutdownState & ShutdownManager::SDS_WRAPUP_COMPLETE ) )
	{
		_lprint( "GameLog.txt", "onDeInit is called with SDS 0x%08X.\n", nShutdownState );
	}

	b_performanceUpdate = false;

	g_DBTracker.dump();

	void DeInitGame();
	DeInitGame();

	//LUA()->DeInitThread();
	LUA()->DeInit();


	LOG::DeInit();
	FileLogHandler::DeInit();

	return true;
}

bool _HerlockIntf::onKeyDown( int nVkCode )
{
	return false;
}

void _HerlockIntf::onDrawUser( HDC dc )
{
	static DWORD dwPrevTime = timeGetTime() - 1000;
	if( dwPrevTime + 1000 < timeGetTime() )
	{
		void Draw();
		Draw();
		dwPrevTime = timeGetTime();
		InvalidateRect( GetViewWindow(), NULL, false );
	}
}



bool _HerlockIntf::onMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( msg == WM_TIMER )
	{
		static bool bLoadingComplete = false;
		if( !bLoadingComplete && ENV().GetString( "game.loading" ) == "complete" )
		{
			bLoadingComplete = true;

			extern IStreamSocketConnection * g_pAuthConnection;
			if( !g_pAuthConnection || !g_pAuthConnection->IsConnected() )
			{
				if( !ConnectToAuth() )
				{
					_cprint( "Connecting to the auth server failed. Check auth server status. IP:%s / PORT:%d\n", ENV().GetString( "io.auth.ip", "127.0.0.1" ).c_str(), ENV().GetInt( "io.auth.port", 4502 ) );
					_cprint( "Enter 'connect auth' to retry.\n" );
					FILELOG( "Connecting to the auth server failed. Check auth server status. IP:%s / PORT:%d", ENV().GetString( "io.auth.ip", "127.0.0.1" ).c_str(), ENV().GetInt( "io.auth.port", 4502 ) );
					ENV().Remove( "game.loading" );
					bLoadingComplete = false;
					return true;
				}
			}

			void SendGameServerLoginToAuth( const char * szServerName, const char * szServerScreenshotUrl, unsigned short nServerIdx, const char * szServerIP, int nServerPort );
			SendGameServerLoginToAuth( ENV().GetString( "auth.server_name", "테스트서버" ).c_str(), ENV().GetString( "game.server_screenshot_url", "about:blank" ).c_str(), ENV().GetInt( "auth.server_idx", 1 ), ENV().GetString( "io.ip_address", "192.168.0.84" ).c_str(), ENV().GetInt( "io.port", 4514 ) );

			while( 1 )
			{
				std::string strAuthConnectState = ENV().GetString( "auth.connect", "" );

				if( strAuthConnectState.empty() )
					Sleep( 1 );
				else if( strAuthConnectState == "complete" )
					break;
				else if( strAuthConnectState == "failed" )
				{
					_cprint( "Registering this server to auth server with idx %d is failed.\n", ENV().GetInt( "auth.server_idx", 1 ) );
					_cprint( "Enter 'connect auth' to retry.\n" );
					FILELOG( "Registering this server to auth server with idx %d is failed.", ENV().GetInt( "auth.server_idx", 1 ) );
					ENV().Remove( "game.loading" );
					ENV().Remove( "auth.connect" );
					bLoadingComplete = false;
					return true;
				}
			}

			std::string strUploadConnectState = ENV().GetString( "upload.connect", "" );
			if( strUploadConnectState.empty() )
			{
				ENV().Set( "upload.connect", "failed" );
				ConnectToUpload();
			}

			std::string strCoordinatorConnectState = ENV().GetString( "_coordinator.connect", "" );
			if( strCoordinatorConnectState.empty() )
			{
				ENV().Set( "_coordinator.connect", "failed" );
				//ConnectToCoordinator();
			}

			StartAccept();

			// 접속을 받기 시작한 이후에 ScheduledCommand 실행을 시작
			ScheduledCommandManager::Instance().Init();
		}
#ifdef _MEM_USAGE_DEBUG
		static DWORD dwPrevTime = GetTickCount();
		DWORD dwCurrentTime = GetTickCount();
		if( ENV().GetInt( "game.mem_usage_info_save_interval" ) && dwPrevTime + ENV().GetInt( "game.mem_usage_info_save_interval" ) < dwCurrentTime )
		{
			dwPrevTime = dwCurrentTime;
			XSEH::WriteAllocCountInfo( "MemUsageInfo.txt" );
			HANDLE hFile = INVALID_HANDLE_VALUE;
			if ( (hFile = CreateFile( "MemUsageInfo.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL )) != INVALID_HANDLE_VALUE )
			{
				SetFilePointer( hFile, 0, NULL, FILE_END );
				WriteHeapUsageInfo( hFile );
				CloseHandle( hFile );
			}
		}
#endif
	}

	if( GetViewMode() == ArcadiaFrameworkIntf::VIEW_USER )
	{
		if( msg == WM_PAINT || msg == WM_TIMER )
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hWnd, &ps);
			onDrawUser( hdc );
			EndPaint( hWnd, &ps );
			return true;
		}
	}

	if( GetViewMode() == ArcadiaFrameworkIntf::VIEW_LOG )
	{
		if( msg == WM_TIMER )
		{
			static DWORD dwPrevTime = GetTickCount();
			if( GetTickCount() > dwPrevTime + 200 )
			{
				dwPrevTime = GetTickCount();
				InvalidateRect( hWnd, NULL, false );
			}
			return true;
		}

		if( msg == WM_PAINT )
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hWnd, &ps);
			onDrawLog( hdc );
			EndPaint( hWnd, &ps );
			return true;
		}
	}

	return false;
}

struct _ProfilerScreenOutput
{
	_ProfilerScreenOutput( ArcadiaCommandResultReceiver * pReceiver )	{ m_pReceiver = pReceiver; }

	void operator()( const char * szString, ... )
	{
		char p[2048];
		va_list ap;
		va_start(ap, szString);
		s_vsprintf(p, _countof(p), szString, ap);
		va_end(ap);
		std::string str = p;
		XStringUtil::Replace( str, "%", "%%" );
		m_pReceiver->onWrite( str.c_str() );
	}

	ArcadiaCommandResultReceiver * m_pReceiver;
};

struct _ProfilerFileOutput
{
	_ProfilerFileOutput( const char* szFileName )
	{
		fp = NULL;
		fopen_s( &fp, szFileName, "a+" );
	}

	~_ProfilerFileOutput()
	{
		if( fp ) fclose( fp );
	}

	void operator()( const char * szString, ... )
	{
		char p[2048];
		va_list ap;
		va_start(ap, szString);
		s_vsprintf(p, _countof(p), szString, ap);
		va_end(ap);
		if( fp ) fwrite( p, 1, strlen(p), fp );
	}

	FILE *fp;
};

bool _HerlockIntf::onCommand(	const char* szTarget,
								const char* szFrom,
								const char *szFullCmd,
								std::vector< std::string > & vList,
								ArcadiaCommandResultReceiver & receiver )
{
	LOG::Log11N4S( LM_CHEAT,
		0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
		szTarget, LOG::STR_NTS, szFrom, LOG::STR_NTS,
		"", LOG::STR_NTS,
		szFullCmd, LOG::STR_NTS );

	if( !_stricmp( szFullCmd, "view" ) )
	{
		return true;
	}
	else if( szFullCmd[0] == '#' )
	{
		std::string strCmd = &szFullCmd[1];
		XStringUtil::Trim( strCmd );
		if( strCmd.size() > GameRule::CHAT_MAX_LENGTH )
		{
			receiver.onWrite( "The submitted message is too long. (Limit is %u characters.)\n", GameRule::CHAT_MAX_LENGTH );
			return false;
		}

		std::string strResult;
		LuaVM::GlobalInst()->RunString( strCmd.c_str(), &strResult );

		if( !strResult.empty() && strResult != "nil" )
			receiver.onWrite( strResult.c_str() );

		return true;
	}
	else if( !_stricmp( vList[0].c_str(), "lua_do" ) )
	{
		if( vList.size() > 1 )
		{
			std::string strCmd = vList[1];
			XStringUtil::Trim( strCmd );
			if( strCmd.size() > GameRule::CHAT_MAX_LENGTH )
			{
				receiver.onWrite( "The submitted message is too long. (Limit is %u characters.)\n", GameRule::CHAT_MAX_LENGTH );
				return false;
			}

			std::string strResult;
			LuaVM::GlobalInst()->RunString( strCmd.c_str(), &strResult );

			if( !strResult.empty() && strResult != "nil" )
				receiver.onWrite( strResult.c_str() );

			return true;
		}
	}
	else if( !_stricmp( vList[0].c_str(), "lua_run" ) )
	{
		if( vList.size() > 1 )
		{
			std::string strCmd = vList[1];
			XStringUtil::Trim( strCmd );

			LuaVM::GlobalInst()->LoadScript( strCmd.c_str(), true );
			return true;
		}
	}
	else if( !_stricmp( vList[0].c_str(), "erase_auto" ) )
	{
		if( vList.size() > 1 )
		{
			StructPlayer::EraseFromAutoAccount( atoi( vList[1].c_str() ) );
		}
		
		return true;
	}
	else if( !_stricmp( vList[0].c_str(), "connect" ) )
	{
		if( vList.size() > 1 )
		{
			if( vList[1] == "auth" )
			{
				if( ENV().GetString( "game.loading" ) == "complete" || ENV().GetString( "auth.connect" ) == "complete" )
					receiver.onWrite( "Already once connected to auth server.\n" );
				else
					ENV().Set( "game.loading", "complete" );
			}
			else if( vList[1] == "upload" )
			{
				ConnectToUpload();
			}
			else if( vList[1] == "coordinator" )
			{
				ConnectToCoordinator();
			}
		}

		return true;
	}
	else if( !_stricmp( vList[0].c_str(), "show_process_usage" ) )
	{
		receiver.onWrite( " - Network Processing Info - \n" );
		auto it = g_NetworkReceiverPerformanceTracker.m_data.begin();
		for( ; it != g_NetworkReceiverPerformanceTracker.m_data.end() ; ++it )
		{
			// 총 처리 시간 1초 미만인 경우 보여주지 않음.
			if( (*it).second.m_totalTime < 1000 )
				continue;
			receiver.onWrite( "Packet ID[%u] - Count:%u, ElapsedTime:%ums\n", 
				(*it).first, (*it).second.m_count, (*it).second.m_totalTime );
		}

		receiver.onWrite( " - Scheduling Processing Info - \n" );
		receiver.onWrite( "Scheduling[Player] - Count:%u, ElapsedTime:%ums\n", 
			g_PlayerPerformanceTracker.m_count, g_PlayerPerformanceTracker.m_totalTime );
		receiver.onWrite( "Scheduling[Summon] - Count:%u, ElapsedTime:%ums\n", 
			g_SummonPerformanceTracker.m_count, g_SummonPerformanceTracker.m_totalTime );
		receiver.onWrite( "Scheduling[Monster] - Count:%u, ElapsedTime:%ums\n", 
			g_MonsterPerformanceTracker.m_count, g_MonsterPerformanceTracker.m_totalTime );
		
		return true;
	}

	return false;
}

XEnvStruct& CMD()
{
	static XEnvStruct cmd;
	return cmd;
}

void onInitArcadiaFramework()
{
	// init default environment
	InitDefaultEnv();

	// 파일에서 로드
	CMD().LoadFromFile( "Commands.opt" );
	ENV().LoadFromFile( "GameServer.eop", "*", true );

	if(GameRule::Hardcore)
	{
		// hardcode server면 pkserver가 되어야한다
		GameRule::bIsPKServer = true;
		GameRule::HardcoreExpRate = 1.0f;
	}

	// GameServer.opt 병행 사용 처리
	// game.local_setting_list에 지정된 키 값만 사용하며, game.local_setting_list의 값은 [타입 키이름]|[타입 키이름]|... 과 같아야 함
	// Ex. game.local_setting_list: N game.max_level|S db.c.server|N db.c.port|S db.user.server|N db.user.port
	std::string strLocalSettingList = ENV().GetString( "game.local_setting_list", "" );
	if( !strLocalSettingList.empty() )
	{
		XEnvStruct envLocalSetting;

		if( envLocalSetting.LoadFromFile( "GameServer.opt" ) )
		{
			std::vector< std::string >	vLocalSetting;
			XStringUtil::Split( strLocalSettingList.c_str(), vLocalSetting, "|" );

			for( std::vector< std::string >::const_iterator it = vLocalSetting.begin() ; it != vLocalSetting.end() ; ++it )
			{
				const char * pszKey = (*it).c_str() + 2;

				if( !envLocalSetting.IsExist( pszKey ) )
					continue;

				switch( *(*it).c_str() )
				{
				case 'S':	ENV().Set( pszKey, envLocalSetting.GetString( pszKey ) ); break;
				case 'F':	ENV().Set( pszKey, envLocalSetting.GetFloat( pszKey ) ); break;
				case 'V':	ENV().Set( pszKey, envLocalSetting.GetVector( pszKey ) ); break;
				case 'Q':	ENV().Set( pszKey, envLocalSetting.GetQuaternion( pszKey ) ); break;
				case 'N':	ENV().Set( pszKey, envLocalSetting.GetInt( pszKey ) ); break;
				case 'T':	ENV().Set( pszKey, (short)envLocalSetting.GetInt( pszKey ) ); break;
				}
			}
		}

		ENV().Remove( "game.local_setting_list" );
	}

	// 패스워드 삭제
	CMD().Remove( "db.billing.account" );
	CMD().Remove( "db.billing._password" );
	CMD().Remove( "db.c.account" );
	CMD().Remove( "db.c._password" );
	CMD().Remove( "db.user.account" );
	CMD().Remove( "db.user._password" );

	ENV().Remove( "db.billing.account" );
	ENV().Remove( "db.billing._password" );
	ENV().Remove( "db.c.account" );
	ENV().Remove( "db.c._password" );
	ENV().Remove( "db.user.account" );
	ENV().Remove( "db.user._password" );

	myFrameworkIntf.SetConsoleEopFileName( "GameServer.eop" );

	// 읽기 전용 필드 설정
	ENV().SetReadOnly( "game.party_exp_rate" );
	ENV().SetReadOnly( "game.no_collision_check" );
	ENV().SetReadOnly( "game.disable_dungeon_raid_siege" );

	ENV().SetReadOnly( "game.limit_game_time" );
	ENV().SetReadOnly( "game.max_healthy_game_time" );
	ENV().SetReadOnly( "game.max_tired_game_time" );

	ENV().SetReadOnly( "game.log_required_state_list" );
	ENV().SetReadOnly( "game.log_required_item_list" );

	ENV().SetReadOnly( "game.farm_normal_summon_exp" );
	ENV().SetReadOnly( "game.farm_growth_summon_exp" );

	ArcadiaFrameworkIntf::SetFrameworkInterface( &myFrameworkIntf );

	ArcadiaFrameworkIntf::AddWatchList( "game.server_version" );
	ArcadiaFrameworkIntf::AddWatchList( "engine.ar_time" );
	ArcadiaFrameworkIntf::AddWatchList( "engine.count_client" );
	ArcadiaFrameworkIntf::AddWatchList( "engine.count_movable" );
	ArcadiaFrameworkIntf::AddWatchList( "engine.count_static" );
	ArcadiaFrameworkIntf::AddWatchList( "game.user_count" );
	ArcadiaFrameworkIntf::AddWatchList( "game.item_count" );
	ArcadiaFrameworkIntf::AddWatchList( "io.connection" );
	ArcadiaFrameworkIntf::AddWatchList( "iocp.dis_count" );
	ArcadiaFrameworkIntf::AddWatchList( "io.send_count" );
	ArcadiaFrameworkIntf::AddWatchList( "iocp.send_rest_bytes" );
	ArcadiaFrameworkIntf::AddWatchList( "iocp.max_alloc_size" );
	ArcadiaFrameworkIntf::AddWatchList( "io.recv_count" );
	ArcadiaFrameworkIntf::AddWatchList( "db.user.work_active" );
	ArcadiaFrameworkIntf::AddWatchList( "db.user.work_pending" );
	ArcadiaFrameworkIntf::AddWatchList( "db.user.work_total" );
	ArcadiaFrameworkIntf::AddWatchList( "db.user.exec_query" );
	ArcadiaFrameworkIntf::AddWatchList( "db.user.remain_query" );
}

// 1초마다 서버 전체 성능을 기록
unsigned __stdcall performanceLogFunc( void* )
{
	while( b_performanceUpdate )
	{
		if( g_bPerformanceTracing )
		{
			// Scheduling Thread 관련 성능 기록
			FileLogHandler::GetFileLogHandler()->LogStringToFileEx( NULL, "PerformanceTrackingLog", 
				"%Y-%m-%d.log", "[Scheduling] Player Count[%u] Time[%ums]", g_PlayerPerformanceTracker.m_count, g_PlayerPerformanceTracker.m_totalTime );
			FileLogHandler::GetFileLogHandler()->LogStringToFileEx( NULL, "PerformanceTrackingLog", 
				"%Y-%m-%d.log", "[Scheduling] Summon Count[%u] Time[%ums]", g_SummonPerformanceTracker.m_count, g_SummonPerformanceTracker.m_totalTime );
			FileLogHandler::GetFileLogHandler()->LogStringToFileEx( NULL, "PerformanceTrackingLog", 
				"%Y-%m-%d.log", "[Scheduling] Monster Count[%u] Time[%ums]", g_MonsterPerformanceTracker.m_count, g_MonsterPerformanceTracker.m_totalTime );

			// IOCP 관련 성능 기록
			for( auto it = g_NetworkReceiverPerformanceTracker.m_data.begin() ; it != g_NetworkReceiverPerformanceTracker.m_data.end() ; ++it )
			{
				// 최소 0ms 이상 처리한 것들만 찍어준다.
				if( (*it).second.m_totalTime == 0 )
					continue;

				FileLogHandler::GetFileLogHandler()->LogStringToFileEx( NULL, "PerformanceTrackingLog", 
					"%Y-%m-%d.log", "[IOCP] PacketID[%u] Count[%u] Time[%ums]", (*it).first, (*it).second.m_count, (*it).second.m_totalTime );
			}

			// Thread CPU사용량 관련 기록
			// 현재 vWatchingThreadList에 Lock이 없어 쓰레드가 소멸되면 iterator 무효화가 일어날 수 있다..
			// 지금은 게임 서버 꺼질 때 말고는 vWatchingThreadList에 삭제 처리를 하지 않는다. 보수해야할 코드.
			{
				THREAD_SYNCRONIZE( &XThreadMonitor::xWatchingThreadListCS );
				for( auto it = XThreadMonitor::vWatchingThreadList.begin() ; it != XThreadMonitor::vWatchingThreadList.end() ; ++it )
				{
					FileLogHandler::GetFileLogHandler()->LogStringToFileEx( NULL, "PerformanceTrackingLog", 
						"%Y-%m-%d.log", "[Thread] ThreadName[%s] BouncingCount[%u]", (*it).GetThreadName(), (*it).GetBouncingCount() );
				}
			}
		}

		// 1초 후에 다시 수행
		Sleep( g_nIntervalTimeToLogPerformance );
	}

	return 0;
}