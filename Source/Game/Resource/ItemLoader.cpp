
#include <oledberr.h>
#include <set>

#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>
#include <toolkit/XStringUtil.h>

#include "ContentLoader.h"
#include "ItemBase.h"
#include "StructItem.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"



struct dbItem : public CADORecordBinding, public ItemBaseServer
{
	dbItem() { script_string[0] = '\0'; }

	int								price;
	int								effect_id;
	int								enhance_effect_id;

	char							limit_deva;
	char							limit_asura;
	char							limit_gaia;

	char							limit_fighter;
	char							limit_hunter;
	char							limit_magician;
	char							limit_summoner;

	int								item_use_flag;

	_decimal_variant				cst_weight;

	_decimal_variant				dv_range;
	_decimal_variant				base_var1[4];
	_decimal_variant				base_var2[4];

	_decimal_variant				opt_var1[4];
	_decimal_variant				opt_var2[4];

// 	_decimal_variant				enhance_var1[4];
// 	_decimal_variant				enhance_var2[4];
	char							script_string[513];

BEGIN_ADO_BINDING(dbItem)

	ADO_VARIABLE_LENGTH_ENTRY4(	1, adInteger, nCode, sizeof(nCode), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	2, adInteger, nNameId, sizeof(nNameId), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	4, adInteger, nType, sizeof(nType), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	5, adInteger, nGroup, sizeof(nGroup), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	6, adInteger, nClass, sizeof(nClass), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	7, adInteger, WearType, sizeof(WearType), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	8, adInteger, nSetID, sizeof(nSetID), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	9, adInteger, nSetPartFlag, sizeof(nSetPartFlag), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	10, adInteger, nGrade, sizeof(nGrade), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	11, adInteger, nRank, sizeof(nRank), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	12, adInteger, nLevel, sizeof(nLevel), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	13, adInteger, nEnhance, sizeof(nEnhance), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	14, adInteger, nSocketCount, sizeof(nSocketCount), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	15, adInteger, nInstanceFlag, sizeof(nInstanceFlag), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	16, adBoolean, limit_deva, sizeof(limit_deva), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	17, adBoolean, limit_asura, sizeof(limit_asura), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	18, adBoolean, limit_gaia, sizeof(limit_gaia), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	19,	adTinyInt, nJobDepth, sizeof(nJobDepth), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	20, adBoolean, limit_fighter, sizeof(limit_gaia), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	21, adBoolean, limit_hunter, sizeof(limit_gaia), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	22, adBoolean, limit_magician, sizeof(limit_gaia), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	23, adBoolean, limit_summoner, sizeof(limit_gaia), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	24, adInteger, nMinLevel, sizeof(nMinLevel), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	25, adInteger, nMaxLevel, sizeof(nMaxLevel), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	26, adInteger, nTargetMinLevel, sizeof(nTargetMinLevel), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	27, adInteger, nTargetMaxLevel, sizeof(nTargetMaxLevel), FALSE )

	ADO_NUMERIC_ENTRY2(			28,	adDecimal, dv_range,	10,2,FALSE )
	ADO_NUMERIC_ENTRY2(			29,	adDecimal, cst_weight,	10,2,FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	30, adInteger, price, sizeof(price), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	31, adInteger, nHuntaholicPoint, sizeof( nHuntaholicPoint ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	32, adInteger, nArenaPoint, sizeof( nArenaPoint ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	33, adInteger, nEtherealDurability, sizeof(nEtherealDurability), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	34, adInteger, nEndurance, sizeof(nEndurance), FALSE )

	// 35: 재질 int 추가 클라 전용

	ADO_VARIABLE_LENGTH_ENTRY4(	36, adInteger, nSummonId, sizeof(nSummonId), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 37, adInteger, item_use_flag, sizeof(item_use_flag), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	38, adInteger, available_time, sizeof(available_time), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	39, adTinyInt, decrease_type, sizeof(decrease_type), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	40, adInteger, nThrowRange, sizeof(nThrowRange), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	41, adSmallInt, nBaseType[0], sizeof(nBaseType[0]), FALSE )
	ADO_NUMERIC_ENTRY2( 		42,	adDecimal, base_var1[0],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2( 		43,	adDecimal, base_var2[0],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	44, adSmallInt, nBaseType[1], sizeof(nBaseType[1]), FALSE )
	ADO_NUMERIC_ENTRY2(			45,	adDecimal, base_var1[1],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			46,	adDecimal, base_var2[1],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	47, adSmallInt, nBaseType[2], sizeof(nBaseType[2]), FALSE )
	ADO_NUMERIC_ENTRY2(			48,	adDecimal, base_var1[2],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			49,	adDecimal, base_var2[2],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	50, adSmallInt, nBaseType[3], sizeof(nBaseType[3]), FALSE )
	ADO_NUMERIC_ENTRY2(			51,	adDecimal, base_var1[3],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			52,	adDecimal, base_var2[3],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	53, adSmallInt, nOptType[0], sizeof(nOptType[0]), FALSE )
	ADO_NUMERIC_ENTRY2(			54,	adDecimal, opt_var1[0],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			55,	adDecimal, opt_var2[0],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	56, adSmallInt, nOptType[1], sizeof(nOptType[1]), FALSE )
	ADO_NUMERIC_ENTRY2(			57,	adDecimal, opt_var1[1],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			58,	adDecimal, opt_var2[1],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	59, adSmallInt, nOptType[2], sizeof(nOptType[2]), FALSE )
	ADO_NUMERIC_ENTRY2(			60,	adDecimal, opt_var1[2],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			61,	adDecimal, opt_var2[2],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	62, adSmallInt, nOptType[3], sizeof(nOptType[3]), FALSE )
	ADO_NUMERIC_ENTRY2(			63,	adDecimal, opt_var1[3],	12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			64,	adDecimal, opt_var2[3],	12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4( 65, adInteger, effect_id, sizeof( effect_id ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	66, adInteger, enhance_effect_id, sizeof(enhance_effect_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	67, adInteger, nSkillID, sizeof(nSkillID), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	68, adInteger, nStateCode, sizeof(nStateCode), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	69, adInteger, nStateLevel, sizeof(nStateLevel), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	70, adInteger, nStateTime, sizeof(nStateTime), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	71, adInteger, nCoolTime, sizeof(nCoolTime), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	72, adSmallInt, nCoolTimeGroup, sizeof(nCoolTimeGroup), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(102, adVarChar, script_string, _countof(script_string), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(103, adInteger, nArrangeCode, sizeof(nArrangeCode), FALSE)
//	ADO_VARIABLE_LENGTH_ENTRY4(	90, adVarChar, script_string, _countof(script_string), FALSE )
//	ADO_VARIABLE_LENGTH_ENTRY4( 91, adInteger, nArrangeCode, sizeof(nArrangeCode), FALSE )
  
END_ADO_BINDING()

};

static void onItemData( dbItem * emprs )
{
	emprs->Flag.Clear();

	// 사거리
	emprs->nRange = emprs->dv_range.getFloat() * GameRule::ATTACK_RANGE_UNIT;

	emprs->nPrice.SetRawData( emprs->price );

	for( int x = 0; x < ItemBase::MAX_OPTION_NUMBER; ++x )
	{
		emprs->fBaseVar1[x].set( emprs->base_var1[x].getMultipleInteger( 10000 ) );
		emprs->fBaseVar2[x].set( emprs->base_var2[x].getMultipleInteger( 10000 ) );
		emprs->fOptVar1[x].set( emprs->opt_var1[x].getMultipleInteger( 10000 ) );
		emprs->fOptVar2[x].set( emprs->opt_var2[x].getMultipleInteger( 10000 ) );

		if( emprs->nBaseType[ x ] == ITEM_EFFECT_INSTANT::WARP ) emprs->item_use_flag |= 1 << ItemBase::FLAG_WARP;
		if(  emprs->nOptType[ x ] == ITEM_EFFECT_INSTANT::WARP ) emprs->item_use_flag |= 1 << ItemBase::FLAG_WARP;
	}

	emprs->Flag.CopyFrom( &emprs->item_use_flag );

	emprs->nLimit = 0;
	if( emprs->limit_asura )	emprs->nLimit	|= ItemBase::LIMIT_ASURA;
	if( emprs->limit_gaia )		emprs->nLimit	|= ItemBase::LIMIT_GAIA;
	if( emprs->limit_deva )		emprs->nLimit	|= ItemBase::LIMIT_DEVA;

	if( emprs->limit_hunter )	emprs->nLimit	|= ItemBase::LIMIT_HUNTER;
	if( emprs->limit_fighter )	emprs->nLimit	|= ItemBase::LIMIT_FIGHTER;
	if( emprs->limit_magician ) emprs->nLimit	|= ItemBase::LIMIT_MAGICIAN;
	if( emprs->limit_summoner ) emprs->nLimit	|= ItemBase::LIMIT_SUMMONER;

	emprs->fWeight = emprs->cst_weight.getFloat();

	if( emprs->effect_id )
		emprs->pvEffectList = GameContent::GetEffectInfoVector( emprs->effect_id );
	else
		emprs->pvEffectList = NULL;

	if( emprs->enhance_effect_id )
		emprs->pvEnhanceEffectList = GameContent::GetEnhanceEffectVector( emprs->enhance_effect_id );
	else
		emprs->pvEnhanceEffectList = NULL;

	char * pszScript = emprs->script_string;
	// 빈 스크립트의 경우 띄어쓰기 한 칸이 들어가 있게 되므로 앞쪽 공백 제거 처리
	while( *pszScript == ' ' )
		++pszScript;
	emprs->strScript = pszScript;
	
	// 기간제 아이템 소멸 시 로그 남겨야 하는지에 대한 설정 별도 로딩 처리(기간제가 아니면 처리 필요 없음)
	// * 런타임 성능을 위해 기간제 아이템 만료 시 마다 설정 리스트를 체크하지 않고 ItemBase에 미리 세팅해 둠
	if( emprs->decrease_type != ItemBase::PERMANENT )
	{
		// 로딩 중에 GameRule::strLogRequiredItemList가 바뀔 일은 없다고 가정하고 static std::set 컨테이너에 토큰화시켜서 담아놓고 처리 함
		static bool bInitComplete = false;
		static std::set< int /* ItemBase::ItemCode */ > setLogRequiredItemList;

		// 최초 1회만 토큰화 처리 적용
		if( !bInitComplete )
		{
			bInitComplete = true;

			std::vector< std::string > vTokenList;
			XStringUtil::Split( GameRule::strLogRequiredItemList.c_str(), vTokenList, "|", false );

			for( std::vector< std::string >::const_iterator it = vTokenList.begin() ; it != vTokenList.end() ; ++it )
			{
				int nItemID = atoi( (*it).c_str() );

				if( nItemID )
					setLogRequiredItemList.insert( nItemID );
			}
		}

		if( setLogRequiredItemList.size() && setLogRequiredItemList.find( emprs->nCode ) != setLogRequiredItemList.end() )
			emprs->bLogRequiredOnExpiration = true;
		else
			emprs->bLogRequiredOnExpiration = false;
	}
	else
		emprs->bLogRequiredOnExpiration = false;

	StructItem::RegisterItemBase( *emprs );
}

static void onItemLocalData( dbItem * emprs )
{
	// 이미 로드되어 있는 데이터면 무시
	if( StructItem::IsValidItemCode( emprs->nCode ) )
	{
		_cprint( "Already loaded resource loaded from [ItemLocalResource]. id(%d)\n", emprs->nCode );
		FILELOG( "Already loaded resource loaded from [ItemLocalResource]. id(%d)", emprs->nCode );
		return;
	}
}

static bool LoadItemData( _ConnectionPtr & ConnPtr )
{
	size_t cnt = LoadDbResource< dbItem >( "ItemResource", ConnPtr, onItemData );

	_cprint( "Total %d Item loaded...\n", cnt );
	FILELOG( "Total %d Item loaded...", cnt );

	try
	{
		cnt = 0;
		cnt = LoadDbResource< dbItem >( "ItemLocalResource", ConnPtr, onItemLocalData );
	}
	catch( _com_error & e )
	{
		if( e.Error() != DB_E_NOTABLE )
			throw;
	}
	catch( ... )
	{
		throw;
	}

	_cprint( "Total %d Item(local) info loaded...\n", cnt );
	FILELOG( "Total %d Item(local) info loaded...", cnt );

	return true;
}

struct dbSetItemEffectInfo : public CADORecordBinding, public SetItemEffectInfo
{
	int								set_id;

	_decimal_variant				base_var1[4];
	_decimal_variant				base_var2[4];

	_decimal_variant				opt_var1[4];
	_decimal_variant				opt_var2[4];

	int								effect_id;

BEGIN_ADO_BINDING(dbSetItemEffectInfo)

	ADO_VARIABLE_LENGTH_ENTRY4(	1,	adInteger, set_id, sizeof(set_id), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	2,	adInteger, nSetPartID, sizeof(nSetPartID), FALSE )

    ADO_VARIABLE_LENGTH_ENTRY4(	5,	adSmallInt, nBaseType[0], sizeof(nBaseType[0]), FALSE )
	ADO_NUMERIC_ENTRY2( 		6,	adDecimal, base_var1[0], 12,2,FALSE )
	ADO_NUMERIC_ENTRY2( 		7,	adDecimal, base_var2[0], 12,2,FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	8,	adSmallInt, nBaseType[1], sizeof(nBaseType[1]), FALSE )
	ADO_NUMERIC_ENTRY2(			9,	adDecimal, base_var1[1], 12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			10,	adDecimal, base_var2[1], 12,2,FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	11,	adSmallInt, nBaseType[2], sizeof(nBaseType[2]), FALSE )
	ADO_NUMERIC_ENTRY2(			12,	adDecimal, base_var1[2], 12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			13,	adDecimal, base_var2[2], 12,2,FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	14,	adSmallInt, nBaseType[3], sizeof(nBaseType[3]), FALSE )
	ADO_NUMERIC_ENTRY2(			15,	adDecimal, base_var1[3], 12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			16,	adDecimal, base_var2[3], 12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	17,	adSmallInt, nOptType[0], sizeof(nOptType[0]), FALSE )
	ADO_NUMERIC_ENTRY2(			18,	adDecimal, opt_var1[0], 12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			19,	adDecimal, opt_var2[0], 12,2,FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	20,	adSmallInt, nOptType[1], sizeof(nOptType[1]), FALSE )
	ADO_NUMERIC_ENTRY2(			21,	adDecimal, opt_var1[1], 12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			22,	adDecimal, opt_var2[1], 12,2,FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	23,	adSmallInt, nOptType[2], sizeof(nOptType[2]), FALSE )
	ADO_NUMERIC_ENTRY2(			24,	adDecimal, opt_var1[2], 12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			25,	adDecimal, opt_var2[2], 12,2,FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	26,	adSmallInt, nOptType[3], sizeof(nOptType[3]), FALSE )
	ADO_NUMERIC_ENTRY2(			27,	adDecimal, opt_var1[3], 12,2,FALSE )
	ADO_NUMERIC_ENTRY2(			28,	adDecimal, opt_var2[3], 12,2,FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	29,	adInteger, effect_id, sizeof( effect_id ), FALSE )

END_ADO_BINDING()

};

static void onSetItemEffectData( dbSetItemEffectInfo * emprs )
{
	for( int x = 0; x < SetItemEffectInfo::MAX_OPTION_NUMBER; ++x )
	{
		emprs->fBaseVar1[x].set( emprs->base_var1[x].getMultipleInteger( 10000 ) );
		emprs->fBaseVar2[x].set( emprs->base_var2[x].getMultipleInteger( 10000 ) );
		emprs->fOptVar1[x].set( emprs->opt_var1[x].getMultipleInteger( 10000 ) );
		emprs->fOptVar2[x].set( emprs->opt_var2[x].getMultipleInteger( 10000 ) );
	}

	if( emprs->effect_id )
		emprs->pvEffectList = GameContent::GetEffectInfoVector( emprs->effect_id );
	else
		emprs->pvEffectList = NULL;

	GameContent::RegisterSetItemEffectInfo( emprs->set_id, *emprs );
}

static bool LoadSetItemEffectData( _ConnectionPtr & ConnPtr )
{
	size_t cnt = LoadDbResource< dbSetItemEffectInfo >( "SetItemEffectResource", ConnPtr, onSetItemEffectData );

	_cprint( "Total %d SetItemEffect loaded...\n", cnt );
	FILELOG( "Total %d SetItemEffect loaded...", cnt );

	return true;
}

struct dbEffectInfo : public CADORecordBinding, public EffectInfo
{
	int					nID;

	_decimal_variant	dvValue[ EffectInfo::NUMBER_OF_VALUE ];

BEGIN_ADO_BINDING(dbEffectInfo)

	ADO_VARIABLE_LENGTH_ENTRY4(	1,	adInteger, nID, sizeof( nID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	2,	adInteger, nOrdinalID, sizeof( nOrdinalID ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4( 4,	adInteger, nMinLevel, sizeof( nMinLevel ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5,	adInteger, nMaxLevel, sizeof( nMaxLevel ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	6,	adInteger, eType, sizeof( eType ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	7,	adInteger, nEffectID, sizeof( nEffectID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	8,	adInteger, nEffectLevel, sizeof( nEffectLevel ), FALSE )

	ADO_NUMERIC_ENTRY2(			9,	adDecimal, dvValue[ 0 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			10,	adDecimal, dvValue[ 1 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			11,	adDecimal, dvValue[ 2 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			12,	adDecimal, dvValue[ 3 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			13,	adDecimal, dvValue[ 4 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			14,	adDecimal, dvValue[ 5 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			15,	adDecimal, dvValue[ 6 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			16,	adDecimal, dvValue[ 7 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			17,	adDecimal, dvValue[ 8 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			18,	adDecimal, dvValue[ 9 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			19,	adDecimal, dvValue[ 10 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			20,	adDecimal, dvValue[ 11 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			21,	adDecimal, dvValue[ 12 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			22,	adDecimal, dvValue[ 13 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			23,	adDecimal, dvValue[ 14 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			24,	adDecimal, dvValue[ 15 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			25,	adDecimal, dvValue[ 16 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			26,	adDecimal, dvValue[ 17 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			27,	adDecimal, dvValue[ 18 ], 12, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			28,	adDecimal, dvValue[ 19 ], 12, 2, FALSE )

END_ADO_BINDING()
};

static void onEffectInfo( dbEffectInfo * emprs )
{
	for( int i = 0 ; i < EffectInfo::NUMBER_OF_VALUE ; ++i )
	{
		emprs->fValue[ i ].set( emprs->dvValue[ i ].getMultipleInteger( 10000 ) );
	}

	GameContent::RegisterEffectInfo( emprs->nID, *emprs );
}

static bool LoadEffectData( _ConnectionPtr & ConnPtr )
{
	size_t cnt = LoadDbResource< dbEffectInfo >( "SELECT * FROM dbo.ItemEffectResource ORDER BY id, ordinal_id", ConnPtr, onEffectInfo, adCmdText );

	_cprint( "Total %d ItemEffect loaded...\n", cnt );
	FILELOG( "Total %d ItemEffect loaded...", cnt );

	return true;
}

struct DbEnhanceEffectInfo : public CADORecordBinding, public EnhanceEffectInfo
{
	_decimal_variant	dvValue[ 25 ];

BEGIN_ADO_BINDING(DbEnhanceEffectInfo)

	ADO_VARIABLE_LENGTH_ENTRY4(	1,	adInteger, nID, sizeof( nID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	2,	adSmallInt, nSubID, sizeof( nSubID ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3,	adSmallInt, nEnhanceEffectID, sizeof( nEnhanceEffectID ), FALSE )

	ADO_NUMERIC_ENTRY2(			4,	adDecimal, dvValue[ 0 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			5,	adDecimal, dvValue[ 1 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			6,	adDecimal, dvValue[ 2 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			7,	adDecimal, dvValue[ 3 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			8,	adDecimal, dvValue[ 4 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			9,	adDecimal, dvValue[ 5 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			10,	adDecimal, dvValue[ 6 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			11,	adDecimal, dvValue[ 7 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			12,	adDecimal, dvValue[ 8 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			13,	adDecimal, dvValue[ 9 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			14,	adDecimal, dvValue[ 10 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			15,	adDecimal, dvValue[ 11 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			16,	adDecimal, dvValue[ 12 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			17,	adDecimal, dvValue[ 13 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			18,	adDecimal, dvValue[ 14 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			19,	adDecimal, dvValue[ 15 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			20,	adDecimal, dvValue[ 16 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			21,	adDecimal, dvValue[ 17 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			22,	adDecimal, dvValue[ 18 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			23,	adDecimal, dvValue[ 19 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			24,	adDecimal, dvValue[ 20 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			25,	adDecimal, dvValue[ 21 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			26,	adDecimal, dvValue[ 22 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			27,	adDecimal, dvValue[ 23 ], 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			28,	adDecimal, dvValue[ 24 ], 10, 2, FALSE )

END_ADO_BINDING()
};

static void onEnhanceEffectInfo( DbEnhanceEffectInfo * emprs )
{
	for( int i = 0 ; i < EnhanceEffectInfo::NUMBER_OF_VALUE_ENHANCE ; ++i )
	{
		emprs->fValue[ i ].set( emprs->dvValue[ i ].getMultipleInteger( 10000 ) );
	}

	GameContent::RigsterEnhanceEffectInfo( emprs->nID, emprs->nSubID, *emprs );
}

static bool LoadEnhanceEffectData( _ConnectionPtr & ConnPtr )
{
	size_t cnt = LoadDbResource< DbEnhanceEffectInfo >( "SELECT * FROM dbo.EnhanceEffectResource ORDER BY sid, sub_id", ConnPtr, onEnhanceEffectInfo, adCmdText );
	
	_cprint( "Total %d EnhanceEffect loaded...\n", cnt );
	FILELOG( "Total %d EnhanceEffect loaded...", cnt );

	return true;
}

struct dbItemConvertInfo : public CADORecordBinding
{
	int nItemCodeOriginal;
	int nItemCodeTarget;

	BEGIN_ADO_BINDING(dbItemConvertInfo)
		ADO_VARIABLE_LENGTH_ENTRY4(	1,	adInteger, nItemCodeOriginal, sizeof( nItemCodeOriginal ), FALSE )
		ADO_VARIABLE_LENGTH_ENTRY4(	2,	adInteger, nItemCodeTarget, sizeof( nItemCodeTarget ), FALSE )
	END_ADO_BINDING()
};

static void onItemConvertInfo( dbItemConvertInfo * emprs )
{
	if( !StructItem::RegisterConvertingInfo( emprs->nItemCodeOriginal, emprs->nItemCodeTarget ) )
	{
		assert( 0 );
		_cprint( "failed to insert item converting info: original[%d] target[%d]\n", emprs->nItemCodeOriginal, emprs->nItemCodeTarget );
		_cprint( "failed to insert item converting info: original[%d] target[%d]", emprs->nItemCodeOriginal, emprs->nItemCodeTarget );
	}
}

static bool LoadItemConvertData( _ConnectionPtr & ConnPtr )
{
	size_t cnt = LoadDbResource< dbItemConvertInfo >( "ItemConversionResource", ConnPtr, onItemConvertInfo );

	_cprint( "Total %d item convert info loaded...\n", cnt );
	FILELOG( "Total %d item convert info loaded...", cnt );

	return true;
}

bool ItemLoader::onProcess( int nThreadNum )
{
	HRESULT  hr = S_OK;
	DBPerformanceTrackHelper helper;
	try
	{
		_ConnectionPtr		ConnPtr = NULL;

		InitContentDbConnection( ConnPtr );

		helper.start();
		LoadEffectData( ConnPtr );
		LoadEnhanceEffectData( ConnPtr );
		LoadSetItemEffectData( ConnPtr );
		LoadItemData( ConnPtr );
		LoadItemConvertData( ConnPtr );
		helper.end( "ItemLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "ItemLoader" );
		LogDBError( e, "ItemLoader", "ItemLoader::onProcess()" );

		std::string strError = "ITEM RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	ENV().Set( "game.item_loading", "complete" );

	return true;
}
