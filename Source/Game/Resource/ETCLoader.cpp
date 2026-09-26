
#include <toolkit/XConsole.h>
#include <toolkit/Khash.h>
#include <toolkit/XStringUtil.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "MixManager.h"
#include "StructItem.h"
#include "GameDBUtil.h"
#include "StructPlayer.h"
#include "GlobalVariableManager.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

typedef KHash< _MARKET_INFO* , hashPr_string_nocase >	MARKET_HASH;

static MARKET_HASH & getMarketHash()
{
	static MARKET_HASH _inst;
	return _inst;
}

struct lessByMarketIndex : public std::binary_function< _MARKET_INFO::_MARKET_TAG, _MARKET_INFO::_MARKET_TAG, bool >
{
	result_type operator() ( first_argument_type a, second_argument_type b )
	{
		return a.index < b.index;
	}
};

void AddMarketInfo( const char *szMarketName, int index, ItemBase::ItemCode code, const StructGold & price, const int huntaholic_point, const int arena_point )
{
	_MARKET_INFO *pInfo = NULL;
	pInfo = GameContent::GetMarketInfo( szMarketName );
	if( !pInfo )
	{
		pInfo = new _MARKET_INFO( szMarketName );
		getMarketHash().add( szMarketName, pInfo );
	}

	pInfo->vItemList.push_back( _MARKET_INFO::_MARKET_TAG( index, code, price, huntaholic_point, arena_point ) );

	std::sort( pInfo->vItemList.begin(), pInfo->vItemList.end(), lessByMarketIndex() );

}

_MARKET_INFO * GameContent::GetMarketInfo( const char *szMarketName )
{
	_MARKET_INFO *pInfo = NULL;
	if( !szMarketName ) return NULL;
	getMarketHash().lookup( szMarketName, pInfo );
	return pInfo;
}

struct dbMarket : public CADORecordBinding
{
	char				szMarketName[61];
	int					nIndex;
	ItemBase::ItemCode	nCode;
	_decimal_variant	fPriceRatio;
	_decimal_variant	fHuntaholicRatio;
	_decimal_variant	fArenaRatio;

BEGIN_ADO_BINDING(dbMarket)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, nIndex, sizeof(nIndex), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adVarChar, szMarketName, _countof(szMarketName), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, nCode, sizeof(nCode), FALSE)
	ADO_NUMERIC_ENTRY2(4, adDecimal, fPriceRatio, 10, 3, FALSE)
	ADO_NUMERIC_ENTRY2(5, adDecimal, fHuntaholicRatio, 10, 3, FALSE)
	ADO_NUMERIC_ENTRY2(6, adDecimal, fArenaRatio, 10, 3, FALSE)
   
END_ADO_BINDING()
};

void onMarketInfo( dbMarket * emprs )
{
	const ItemBaseServer &itemBase = StructItem::GetItemBase( emprs->nCode );
	if( itemBase.nCode != emprs->nCode )
		return;

	// 마켓 정보 등록
	c_fixed10 fPriceRatio;
	fPriceRatio.set( emprs->fPriceRatio.getMultipleInteger( 10000 ) );
	c_fixed10 fHuntaholicRatio;
	fHuntaholicRatio.set( emprs->fHuntaholicRatio.getMultipleInteger( 10000 ) );
	c_fixed10 fArenaRatio;
	fArenaRatio.set( emprs->fArenaRatio.getMultipleInteger( 10000 ) );

	StructGold nDefaultPrice( itemBase.nPrice );

	AddMarketInfo( emprs->szMarketName, emprs->nIndex, emprs->nCode,
		StructGold( fPriceRatio * nDefaultPrice.GetRawData() ),
		fHuntaholicRatio * itemBase.nHuntaholicPoint,
		fArenaRatio * itemBase.nArenaPoint );
}


void LoadMarketData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbMarket >( "MarketResource", ConnPtr, onMarketInfo );

	_cprint( "Total %d Market info loaded...\n", cnt );
	FILELOG( "Total %d Market info loaded...", cnt );
}

struct dbLevelUpTable : public CADORecordBinding
{
	int		level;
	__int64	exp;
	__int64	jp[4];

BEGIN_ADO_BINDING(dbLevelUpTable)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, level, sizeof(level), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adBigInt, exp, sizeof(exp), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adBigInt, jp[0], sizeof(jp), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adBigInt, jp[1], sizeof(jp), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adBigInt, jp[2], sizeof(jp), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adBigInt, jp[3], sizeof(jp), FALSE)
   
END_ADO_BINDING()
};

struct dbSummonLevelUpTable : public CADORecordBinding
{
	int		level;
	__int64		normal_exp;
	__int64		growth_exp;
	__int64		evolve_exp;

BEGIN_ADO_BINDING(dbSummonLevelUpTable)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, level, sizeof(level), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adBigInt, normal_exp, sizeof(normal_exp), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adBigInt, growth_exp, sizeof(growth_exp), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adBigInt, evolve_exp, sizeof(evolve_exp), FALSE)
END_ADO_BINDING()
};

void onLevelUpTableInfo( dbLevelUpTable * emprs )
{
	// 경험치 정보 등록
	GameContent::RegisterExpTable( emprs->level, emprs->exp, emprs->jp[0], emprs->jp[1], emprs->jp[2], emprs->jp[3] );
}

void onSummonLevelUpTableInfo( dbSummonLevelUpTable * emprs )
{
	GameContent::RegisterSummonExpTable( emprs->level, emprs->normal_exp, emprs->growth_exp, emprs->evolve_exp );
}

void LoadLevelUpTableData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbLevelUpTable >( "LevelResource", ConnPtr, onLevelUpTableInfo );

	_cprint( "Total %d LevelUpTable info loaded...\n", cnt );
	FILELOG( "Total %d LevelUpTable info loaded...", cnt );
}

void LoadSummonLevelUpTableData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbSummonLevelUpTable >( "SummonLevelResource", ConnPtr, onSummonLevelUpTableInfo );

	_cprint( "Total %d SummonLevelUpTable info loaded...\n", cnt );
	FILELOG( "Total %d SummonLevelUpTable info loaded...", cnt );
}

struct dbStateInfo : public CADORecordBinding, public StateInfo
{
BEGIN_ADO_BINDING(dbStateInfo)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, id, sizeof(id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, name_id, sizeof(name_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, tooltip_id, sizeof(tooltip_id), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(4, adBoolean, is_harmful, sizeof(is_harmful), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, state_time_type, sizeof(state_time_type), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, state_group, sizeof(state_group), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(7, adInteger, duplicate_group[0], sizeof(duplicate_group[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, duplicate_group[1], sizeof(duplicate_group[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, duplicate_group[2], sizeof(duplicate_group[2]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(10, adTinyInt, uf_avatar, sizeof(uf_avatar), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, adTinyInt, uf_summon, sizeof(uf_summon), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, adTinyInt, uf_monster, sizeof(uf_monster), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(13, adSmallInt, reiteration_count, sizeof(reiteration_count), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, adInteger, base_effect_id, sizeof(base_effect_id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, adInteger, fire_interval, sizeof(fire_interval), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(16, adInteger, elemental_type, sizeof(elemental_type), FALSE)
	ADO_NUMERIC_ENTRY2( 17,	adDecimal, cst_amplify_base,		13, 3, FALSE)
	ADO_NUMERIC_ENTRY2( 18,	adDecimal, cst_amplify_per_skl,		13, 3, FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(19, adInteger, add_damage_base, sizeof(add_damage_base), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(20, adInteger, add_damage_per_skl, sizeof(add_damage_per_skl), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(21, adInteger, effect_type, sizeof(effect_type), FALSE)

	ADO_NUMERIC_ENTRY2( 22,	adDecimal, cst_var[0],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 23,	adDecimal, cst_var[1],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 24,	adDecimal, cst_var[2],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 25,	adDecimal, cst_var[3],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 26,	adDecimal, cst_var[4],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 27,	adDecimal, cst_var[5],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 28,	adDecimal, cst_var[6],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 29,	adDecimal, cst_var[7],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 30,	adDecimal, cst_var[8],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 31,	adDecimal, cst_var[9],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 32,	adDecimal, cst_var[10],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 33,	adDecimal, cst_var[11],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 34,	adDecimal, cst_var[12],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 35,	adDecimal, cst_var[13],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 36,	adDecimal, cst_var[14],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 37,	adDecimal, cst_var[15],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 38,	adDecimal, cst_var[16],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 39,	adDecimal, cst_var[17],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 40,	adDecimal, cst_var[18],	13,3,FALSE)
	ADO_NUMERIC_ENTRY2( 41,	adDecimal, cst_var[19],	13,3,FALSE)

END_ADO_BINDING()

	_decimal_variant				cst_var[ StateInfo::MAX_STATE_VALUE ];
	_decimal_variant				cst_amplify_base;
	_decimal_variant				cst_amplify_per_skl;
};

void onStateInfo( dbStateInfo * emprs )
{
	for( int i = 0 ; i < StateInfo::MAX_STATE_VALUE ; ++i )
	{
		emprs->fValue[i].set( emprs->cst_var[i].getMultipleInteger( 10000 ) );
	}
	emprs->amplify_base.set( emprs->cst_amplify_base.getMultipleInteger( 10000 ) );
	emprs->amplify_per_skl.set( emprs->cst_amplify_per_skl.getMultipleInteger( 10000 ) );

	// 지속효과 소멸 시 로그 남겨야 하는지에 대한 설정 별도 로딩 처리
	// * 런타임 성능을 위해 지속효과 소멸 시 마다 설정 리스트를 체크하지 않고 StateInfo에 미리 세팅해 둠
	{
		// 로딩 중에 GameRule::strLogRequiredStateList가 바뀔 일은 없다고 가정하고 static std::set 컨테이너에 토큰화시켜서 담아놓고 처리 함
		static bool bInitComplete = false;
		static std::set< int /* StructState::StateCode */ > setLogRequiredStateList;

		// 최초 1회만 토큰화 처리 적용
		if( !bInitComplete )
		{
			bInitComplete = true;

			std::vector< std::string > vTokenList;
			XStringUtil::Split( GameRule::strLogRequiredStateList.c_str(), vTokenList, "|", false );

			for( std::vector< std::string >::const_iterator it = vTokenList.begin() ; it != vTokenList.end() ; ++it )
			{
				int nStateID = atoi( (*it).c_str() );

				if( nStateID )
					setLogRequiredStateList.insert( nStateID );
			}
		}

		if( setLogRequiredStateList.size() && setLogRequiredStateList.find( emprs->id ) != setLogRequiredStateList.end() )
			emprs->is_log_required_on_expiration = true;
		else
			emprs->is_log_required_on_expiration = false;
	}

	// 일어설 때 삭제되는 지속효과라면 반드시 로그아웃 시에도 사라지는 지속효과여야 한다.
	// 현재는 앉기 여부를 저장하기 않기 때문에 로그인 시에는 무조건 일어선 상태일 것이고 만약 로그아웃 시에 삭제가 되지 않았다면 일어선 채로 일어선 상대에서 부여될 수 없는 지속효과가 걸릴 것이기 때문이다.
	// 물론 일어설 때 삭제되는 플래그가 설정되어있다면 로그아웃 시에 알아서 제거해줘도 되겠지만,
	// 제어할 수 있는 별도의 플래그가 존재하며 앉기와 로그아웃은 개념상으로 완벽히 분리된 내용이기 때문에 굳이 파악하기 힘든 연관 관계를 만들지 않고 기획 데이터에 의존한다.
	assert( !( emprs->state_time_type & StateInfo::ERASE_ON_STAND_UP ) || ( emprs->state_time_type & StateInfo::ERASE_ON_LOGOUT ) );

	GameContent::RegisterStateInfo( *emprs );
}

void LoadStateData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbStateInfo >( "StateResource", ConnPtr, onStateInfo );

	_cprint( "Total %d State info loaded...\n", cnt );
	FILELOG( "Total %d State info loaded...", cnt );
}

struct dbStatInfo : public CADORecordBinding, public CreatureStat
{
BEGIN_ADO_BINDING(dbStatInfo)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adSmallInt, stat_id, sizeof(stat_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(2, adSmallInt, strength, sizeof(strength), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(3, adSmallInt, vital, sizeof(vital), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(4, adSmallInt, dexterity, sizeof(dexterity), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(5, adSmallInt, agility, sizeof(agility), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(6, adSmallInt, intelligence, sizeof(intelligence), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(7, adSmallInt, mentality, sizeof(mentality), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(8, adSmallInt, luck, sizeof(luck), FALSE )

END_ADO_BINDING()
};


void onStatInfo( dbStatInfo * emprs )
{
	// 스텟 정보 등록
	GameContent::RegisterStatInfo( *emprs );
}


void LoadStatData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbStatInfo >( "StatResource", ConnPtr, onStatInfo );

	_cprint( "Total %d Stat info loaded...\n", cnt );
	FILELOG( "Total %d Stat info loaded...", cnt );
}


struct dbJobInfo : public CADORecordBinding, public JobInfo
{
BEGIN_ADO_BINDING(dbJobInfo)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, id, sizeof(id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, text_id, sizeof(text_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, stat_id, sizeof(stat_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, skill_tree_id, sizeof(skill_tree_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(5, adTinyInt, job_class, sizeof(job_class), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(6, adTinyInt, job_depth, sizeof(job_depth), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(7, adSmallInt, up_lv, sizeof(up_lv), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(8, adSmallInt, up_jlv, sizeof(up_jlv), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(9, adSmallInt, availble_job[0], sizeof(availble_job[0]), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(10, adSmallInt, availble_job[1], sizeof(availble_job[1]), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(11, adSmallInt, availble_job[2], sizeof(availble_job[2]), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(12, adSmallInt, availble_job[3], sizeof(availble_job[3]), FALSE )

END_ADO_BINDING()
};

struct dbJobLevelBonus : public CADORecordBinding, public JobLevelBonus
{
BEGIN_ADO_BINDING(dbJobLevelBonus)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, job_id, sizeof(job_id), FALSE )

	ADO_NUMERIC_ENTRY2(2,	adDecimal, dec_str[0],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(3,	adDecimal, dec_vit[0],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(4,	adDecimal, dec_dex[0],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(5,	adDecimal, dec_agi[0],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(6,	adDecimal, dec_int[0],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(7,	adDecimal, dec_men[0],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(8,	adDecimal, dec_luk[0],	10,3,FALSE)

	ADO_NUMERIC_ENTRY2(9,	adDecimal, dec_str[1],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(10,	adDecimal, dec_vit[1],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(11,	adDecimal, dec_dex[1],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(12,	adDecimal, dec_agi[1],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(13,	adDecimal, dec_int[1],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(14,	adDecimal, dec_men[1],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(15,	adDecimal, dec_luk[1],	10,3,FALSE)

	ADO_NUMERIC_ENTRY2(16,	adDecimal, dec_str[2],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(17,	adDecimal, dec_vit[2],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(18,	adDecimal, dec_dex[2],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(19,	adDecimal, dec_agi[2],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(20,	adDecimal, dec_int[2],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(21,	adDecimal, dec_men[2],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(22,	adDecimal, dec_luk[2],	10,3,FALSE)

	ADO_NUMERIC_ENTRY2(23,	adDecimal, dec_str[3],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(24,	adDecimal, dec_vit[3],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(25,	adDecimal, dec_dex[3],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(26,	adDecimal, dec_agi[3],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(27,	adDecimal, dec_int[3],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(28,	adDecimal, dec_men[3],	10,3,FALSE)
	ADO_NUMERIC_ENTRY2(29,	adDecimal, dec_luk[3],	10,3,FALSE)
END_ADO_BINDING()

	_decimal_variant			  dec_str[4];
	_decimal_variant			  dec_vit[4];
	_decimal_variant			  dec_dex[4];
	_decimal_variant			  dec_agi[4];
	_decimal_variant			  dec_int[4];
	_decimal_variant			  dec_men[4];
	_decimal_variant			  dec_luk[4];
};

void onJobInfo( dbJobInfo * emprs )
{
	// 직업 정보 등록
	emprs->skill_tree = NULL;
	GameContent::RegisterJobInfo( *emprs );
}

void onJobLevelBonus( dbJobLevelBonus * emprs )
{
	for( int i = 0; i < 4; ++i )
	{
		emprs->strength[i] = emprs->dec_str[i].getMultipleInteger( 100 );
		emprs->vital[i] = emprs->dec_vit[i].getMultipleInteger( 100 );
		emprs->dexterity[i] = emprs->dec_dex[i].getMultipleInteger( 100 );
		emprs->agility[i] = emprs->dec_agi[i].getMultipleInteger( 100 );
		emprs->intelligence[i] = emprs->dec_int[i].getMultipleInteger( 100 );
		emprs->mentality[i] = emprs->dec_men[i].getMultipleInteger( 100 );
		emprs->luck[i] = emprs->dec_luk[i].getMultipleInteger( 100 );
	}
	
	GameContent::RegisterJobLevelBonusInfo( *emprs );
}

void LoadJobData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbJobInfo >( "JobResource", ConnPtr, onJobInfo );
	size_t job_bonus_cnt = LoadDbResource< dbJobLevelBonus >( "JobLevelBonus", ConnPtr, onJobLevelBonus );

	_cprint( "Total %d Job info loaded...\n", cnt );
	FILELOG( "Total %d Job info loaded...", cnt );
}

struct dbBannedWord : public CADORecordBinding
{
BEGIN_ADO_BINDING(dbBannedWord)

	ADO_VARIABLE_LENGTH_ENTRY4(1, adVarWChar, szBannedWord, _countof(szBannedWord) - 1, FALSE)
   
END_ADO_BINDING()

	wchar_t				szBannedWord[512];
};

void onBannedWordInfo( dbBannedWord * emprs )
{
	// 금지단어 정보 등록(대문자화)
	wchar_t szUprWord[512];
	char szResultWord[512];

	s_strcpy( szUprWord, _countof( szUprWord ), emprs->szBannedWord );

	s_toupper( szUprWord, _countof( szUprWord ) );

	WideCharToMultiByte( ENV().GetInt( "CodePage", CP_ACP ), 0, szUprWord, _countof( szUprWord ), szResultWord, _countof( szResultWord ), NULL, NULL );

	GameContent::RegisterBannedWord( szResultWord );
}


void LoadBannedWordData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	GameContent::ClearBannedWord();
	size_t cnt = LoadDbResource< dbBannedWord >( "BanWordResource", ConnPtr, onBannedWordInfo );

	_cprint( "Total %d BannedWord info loaded...\n", cnt );
	FILELOG( "Total %d BannedWord info loaded...", cnt );
}

struct dbEnhance : public CADORecordBinding , EnhanceInfo
{	

BEGIN_ADO_BINDING(dbEnhance)

	ADO_VARIABLE_LENGTH_ENTRY4(1,  adInteger, nSID, sizeof(nSID), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, nFailResult, sizeof(nFailResult), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, nMaxEnhance, sizeof(nMaxEnhance), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, nLocalFlag, sizeof(nLocalFlag), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, nNeedItemCode, sizeof(nNeedItemCode), FALSE)

	ADO_NUMERIC_ENTRY2(   7,	adDecimal, cst_var[0],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(   8,	adDecimal, cst_var[1],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(   9,	adDecimal, cst_var[2],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  10,	adDecimal, cst_var[3],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  11,	adDecimal, cst_var[4],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  12,	adDecimal, cst_var[5],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  13,	adDecimal, cst_var[6],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  14,	adDecimal, cst_var[7],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  15,	adDecimal, cst_var[8],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  16,	adDecimal, cst_var[9],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  17,	adDecimal, cst_var[10],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  18,	adDecimal, cst_var[11],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  19,	adDecimal, cst_var[12],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  20,	adDecimal, cst_var[13],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  21,	adDecimal, cst_var[14],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  22,	adDecimal, cst_var[15],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  23,	adDecimal, cst_var[16],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  24,	adDecimal, cst_var[17],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  25,	adDecimal, cst_var[18],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  26,	adDecimal, cst_var[19],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  27,	adDecimal, cst_var[20],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  28,	adDecimal, cst_var[21],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  29,	adDecimal, cst_var[22],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  30,	adDecimal, cst_var[23],	10,5,FALSE)
	ADO_NUMERIC_ENTRY2(  31,	adDecimal, cst_var[24],	10,5,FALSE)

   
END_ADO_BINDING()

	_decimal_variant			  cst_var[25];
};

void onEnhanceInfo( dbEnhance * emprs )
{
	extern volatile int g_nCurrentLocalFlag;
	if( !(emprs->nLocalFlag & g_nCurrentLocalFlag ) )
		return;

	for( int i = 0; i < 25; ++i )
	{
		emprs->fPercentage[i].set( emprs->cst_var[i].getMultipleInteger( 10000 ) );
	}

	MixManager::RegisterEnhanceInfo( *emprs );
}


void LoadEnhanceData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbEnhance >( "EnhanceResource", ConnPtr, onEnhanceInfo );

	_cprint( "Total %d Enhance info loaded...\n", cnt );
	FILELOG( "Total %d Enhance info loaded...", cnt );
}

void LoadAutoAccountData()
{
	DBConnection db;
	InitUserDbConnection( db.connection );
	if( db.CreateCommand( db.command ) == false )	throw XException( "LoadAutoAccountData : CreateInstance(command) error" );

	// 느린 쿼리 타임아웃 조정
	db.command->CommandTimeout = 60;
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_auto_account_list" );

	_RecordsetPtr pRS = db.command->Execute(NULL,NULL,adCmdStoredProc);

	unsigned cnt = 0;

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{	
		StructPlayer::AddToAutoAccountList( pRS->Fields->Item["account_id"]->Value );

		pRS->MoveNext();

		++cnt;
	}

	_cprint( "Total %d Auto account loaded...\n", cnt );
	FILELOG( "Total %d Auto account loaded...", cnt );
}

void LoadGlobalVariableData()
{
	DBConnection db;
	InitUserDbConnection( db.connection );
	if( db.CreateCommand( db.command ) == false )	throw XException( "LoadGlobalVariableData : CreateInstance(command) error" );

	db.command->CommandType = adCmdTable;
	db.command->CommandText = _bstr_t( "dbo.GlobalVariable" );

	_RecordsetPtr pRS = db.command->Execute( NULL, NULL, adCmdTable );

	size_t cnt = 0;

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{	
		GVM().Set( _bstr_t( pRS->Fields->Item["name"]->Value.bstrVal ), _bstr_t( pRS->Fields->Item["value"]->Value.bstrVal ), true );

		pRS->MoveNext();

		++cnt;
	}

	_cprint( "Total %d GlobalVariable loaded...\n", cnt );
	FILELOG( "Total %d GlobalVariable loaded...", cnt );
}

bool ETCLoader::onProcess( int nThreadNum )
{
	HRESULT  hr = S_OK;

	DBPerformanceTrackHelper helper;
	try
	{
		while( ENV().GetString( "game.item_loading" ) != "complete" )
		{
			Sleep( 1000 );
		}

		helper.start();
		LoadMarketData();
		LoadLevelUpTableData();
		LoadSummonLevelUpTableData();
		LoadStatData();
		LoadJobData();
		LoadStateData();
		LoadBannedWordData();
		LoadEnhanceData();
		LoadAutoAccountData();
		LoadGlobalVariableData();
		helper.end( "ETCLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "ETCLoader" );
		LogDBError( e, "ETCLoader", "ETCLoader::onProcess()" );

		std::string strError = "ETC RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}
