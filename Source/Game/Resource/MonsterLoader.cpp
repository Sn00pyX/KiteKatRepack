
#include <oledberr.h>

#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "StructWorldLocation.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"


struct dbMonster : public CADORecordBinding, public MonsterBase
{

BEGIN_ADO_BINDING(dbMonster)

	ADO_VARIABLE_LENGTH_ENTRY4(	1, adInteger, uid, sizeof( uid ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	2, adInteger, monster_group, sizeof( monster_group ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	3, adInteger, name_id, sizeof( name_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	4, adInteger, location_id, sizeof( location_id ), FALSE )
	// 5 - model
	// 6 - motion_file_id
	ADO_VARIABLE_LENGTH_ENTRY4(	7, adInteger, transform_level, sizeof( transform_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	8, adInteger, walk_type, sizeof( walk_type ), FALSE )
	// 9 - slant_type
	ADO_NUMERIC_ENTRY2(			10, adDecimal, dv_size,		10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			11, adDecimal, dv_scale,	10, 2, FALSE )

	// 12 - target_fx_size
	// 13 - camera_x
	// 14 - camera_y
	// 15 - camera_z
	// 16 - target_x
	// 17 - target_y
	// 18 - target_z

	// 카메라 int 3개, 몬스터 int 3개 추가 클라 전용

	ADO_VARIABLE_LENGTH_ENTRY4(	19, adInteger, level, sizeof( level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	20, adInteger, group, sizeof( group ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	21, adInteger, affiliation, sizeof( affiliation ), FALSE )

	// 22 - species_name_id

	ADO_VARIABLE_LENGTH_ENTRY4(	23, adInteger, magic_type, sizeof( magic_type ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	24, adInteger, race, sizeof( race ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	25, adInteger, visible_range, sizeof( visible_range ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	26, adInteger, chase_range, sizeof( chase_range ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	27, adTinyInt, f_first_attack, sizeof( f_first_attack ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	28, adTinyInt, f_group_first_attack, sizeof( f_group_first_attack ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	29, adTinyInt, f_response_casting, sizeof( f_response_casting ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	30, adTinyInt, f_response_race, sizeof( f_response_race ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	31, adTinyInt, f_response_battle, sizeof( f_response_battle ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	32, adInteger, monster_type, sizeof( monster_type ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	33, adInteger, monster_grade_icon, sizeof( monster_grade_icon ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	34, adInteger, stat_id, sizeof( stat_id ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	35, adInteger, fight_type, sizeof( fight_type ), FALSE)	

	ADO_VARIABLE_LENGTH_ENTRY4(	36, adInteger, skill_trigger_id, sizeof( skill_trigger_id ), FALSE )

	// 37 - meterial
	// 38 - weapon_type

	// 재질 int, 무기 종류 int 클라 전용

	ADO_VARIABLE_LENGTH_ENTRY4(	39, adInteger, attack_speed_type, sizeof( attack_speed_type ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	40, adInteger, ability, sizeof( ability ), FALSE)	

	// 41 standard_walk_speed
	// 42 standard_run_speed

	// 표준 걷기 속도, 표준 달리기 속도 클라 전용

	ADO_VARIABLE_LENGTH_ENTRY4(	43, adInteger, walk_speed, sizeof( walk_speed ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	44, adInteger, run_speed, sizeof( run_speed ), FALSE)	
	ADO_NUMERIC_ENTRY2(			45, adDecimal, dv_attack_range, 10, 2, FALSE)	
	ADO_NUMERIC_ENTRY2(			46, adDecimal, dv_hidesense_range, 10, 2, FALSE)	

	ADO_VARIABLE_LENGTH_ENTRY4(	47, adInteger, hp, sizeof( hp ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	48, adInteger, mp, sizeof( mp ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	49, adInteger, attack_point, sizeof( attack_point ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	50, adInteger, magic_point, sizeof( magic_point ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	51, adInteger, defence, sizeof( defence ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	52, adInteger, magic_defence, sizeof( magic_defence ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	53, adInteger, attack_speed, sizeof( attack_speed ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	54, adInteger, casting_speed, sizeof( casting_speed ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	55, adInteger, accuracy, sizeof( accuracy ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	56, adInteger, avoid, sizeof( avoid ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	57, adInteger, magic_accuracy, sizeof( magic_accuracy ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	58, adInteger, magic_avoid, sizeof( magic_avoid ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	59, adInteger, taming_code, sizeof( taming_code ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	60, adInteger, creature_taming_code_group, sizeof( creature_taming_code_group ), FALSE )

	ADO_NUMERIC_ENTRY2(			61, adDecimal, dv_taming_percentage,	12, 4, FALSE )
	ADO_NUMERIC_ENTRY2(			62, adDecimal, dv_taming_exp_mod,	10, 2, FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	63, adInteger, exp, sizeof( exp ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	64, adInteger, jp, sizeof( jp ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	65, adInteger, gold_drop_percentage, sizeof( gold_drop_percentage ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	66, adInteger, gold_min, sizeof( gold_min ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	67, adInteger, gold_max, sizeof( gold_max ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	68, adInteger, chaos_drop_percentage, sizeof( chaos_drop_percentage ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	69, adInteger, chaos_min, sizeof( chaos_min ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	70, adInteger, chaos_max, sizeof( chaos_max ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	71, adInteger, exp_2, sizeof( exp_2 ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	72, adInteger, jp_2, sizeof( jp_2 ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	73, adInteger, gold_min_2, sizeof( gold_min_2 ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	74, adInteger, gold_max_2, sizeof( gold_max_2 ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	75, adInteger, chaos_min_2, sizeof( chaos_min_2 ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	76, adInteger, chaos_max_2, sizeof( chaos_max_2 ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	77, adInteger, item_drop_table_id, sizeof( item_drop_table_id ), FALSE )

	// 78 - texture_group 클라 전용

	ADO_VARIABLE_LENGTH_ENTRY4(	79, adInteger, local_flag, sizeof( local_flag ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 80, adVarChar, script_on_dead, _countof( script_on_dead ), FALSE )
   
END_ADO_BINDING()

	_decimal_variant			dv_size, dv_scale, dv_attack_range, dv_hidesense_range;
	_decimal_variant			dv_taming_percentage;
	_decimal_variant			dv_taming_exp_mod;

	int skill_trigger_id;
	int item_drop_table_id;

	char f_first_attack;
	char f_group_first_attack;
	char f_response_casting;
	char f_response_race;
	char f_response_battle;

	int location_id;

	int  local_flag;
};

struct dbMonsterSkill : public CADORecordBinding
{
	int									nID;

	MonsterBase::MONSTER_TRIGGER		trigger[ MonsterBase::NUMBER_OF_TRIGGERS_PER_ROW ];
	MonsterBase::MONSTER_SKILL_INFO		skill_info[ MonsterBase::NUMBER_OF_SKILLS_PER_ROW ];

	_decimal_variant					dv_trigger[MonsterBase::NUMBER_OF_TRIGGERS_PER_ROW][2];
	_decimal_variant					dv_skill_probability[MonsterBase::NUMBER_OF_SKILLS_PER_ROW];

BEGIN_ADO_BINDING(dbMonsterSkill)
	ADO_VARIABLE_LENGTH_ENTRY4(	1, adInteger, nID, sizeof( nID ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	3, adInteger, trigger[0].id, sizeof( trigger[0].id ), FALSE)	
	ADO_NUMERIC_ENTRY2(			4, adDecimal, dv_trigger[0][0],	8, 4, FALSE)	
	ADO_NUMERIC_ENTRY2(			5, adDecimal, dv_trigger[0][1],	8, 4, FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	6, adVarChar, trigger[0].script, _countof( trigger[0].script ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	7, adInteger, trigger[1].id, sizeof( trigger[1].id ), FALSE)	
	ADO_NUMERIC_ENTRY2(			8, adDecimal, dv_trigger[1][0],	8, 4, FALSE)	
	ADO_NUMERIC_ENTRY2(			9, adDecimal, dv_trigger[1][1],	8, 4, FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	10, adVarChar, trigger[1].script, _countof( trigger[1].script ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	11, adInteger, trigger[2].id, sizeof( trigger[2].id ), FALSE)	
	ADO_NUMERIC_ENTRY2(			12, adDecimal, dv_trigger[2][0],	8, 4, FALSE)	
	ADO_NUMERIC_ENTRY2(			13, adDecimal, dv_trigger[2][1],	8, 4, FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	14, adVarChar, trigger[2].script, _countof( trigger[2].script ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	15, adInteger, trigger[3].id, sizeof( trigger[3].id ), FALSE)	
	ADO_NUMERIC_ENTRY2(			16, adDecimal, dv_trigger[3][0],	8, 4, FALSE)	
	ADO_NUMERIC_ENTRY2(			17, adDecimal, dv_trigger[3][1],	8, 4, FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	18, adVarChar, trigger[3].script, _countof( trigger[3].script ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	19, adInteger, trigger[4].id, sizeof( trigger[4].id ), FALSE)	
	ADO_NUMERIC_ENTRY2(			20, adDecimal, dv_trigger[4][0],	8, 4, FALSE)	
	ADO_NUMERIC_ENTRY2(			21, adDecimal, dv_trigger[4][1],	8, 4, FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	22, adVarChar, trigger[4].script, _countof( trigger[4].script ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	23, adInteger, trigger[5].id, sizeof( trigger[5].id ), FALSE)	
	ADO_NUMERIC_ENTRY2(			24, adDecimal, dv_trigger[5][0],	8, 4, FALSE)	
	ADO_NUMERIC_ENTRY2(			25, adDecimal, dv_trigger[5][1],	8, 4, FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(	26, adVarChar, trigger[5].script, _countof( trigger[5].script ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	27, adInteger, skill_info[0].skill_id, sizeof( skill_info[0].skill_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	28, adInteger, skill_info[0].skill_lv, sizeof( skill_info[0].skill_lv ), FALSE )
	ADO_NUMERIC_ENTRY2(			29,	adDecimal, dv_skill_probability[0],	8, 4, FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	30, adInteger, skill_info[1].skill_id, sizeof( skill_info[1].skill_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	31, adInteger, skill_info[1].skill_lv, sizeof( skill_info[1].skill_lv ), FALSE )
	ADO_NUMERIC_ENTRY2(			32,	adDecimal, dv_skill_probability[1],	8, 4, FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	33, adInteger, skill_info[2].skill_id, sizeof( skill_info[2].skill_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	34, adInteger, skill_info[2].skill_lv, sizeof( skill_info[2].skill_lv ), FALSE )
	ADO_NUMERIC_ENTRY2(			35,	adDecimal, dv_skill_probability[2],	8, 4, FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	36, adInteger, skill_info[3].skill_id, sizeof( skill_info[3].skill_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	37, adInteger, skill_info[3].skill_lv, sizeof( skill_info[3].skill_lv ), FALSE )
	ADO_NUMERIC_ENTRY2(			38,	adDecimal, dv_skill_probability[3],	8, 4, FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	39, adInteger, skill_info[4].skill_id, sizeof( skill_info[4].skill_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	40, adInteger, skill_info[4].skill_lv, sizeof( skill_info[4].skill_lv ), FALSE )
	ADO_NUMERIC_ENTRY2(			41,	adDecimal, dv_skill_probability[4],	8, 4, FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	42, adInteger, skill_info[5].skill_id, sizeof( skill_info[5].skill_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	43, adInteger, skill_info[5].skill_lv, sizeof( skill_info[5].skill_lv ), FALSE )
	ADO_NUMERIC_ENTRY2(			44,	adDecimal, dv_skill_probability[5],	8, 4, FALSE )

END_ADO_BINDING()
};

struct dbMonsterItemDrop : public CADORecordBinding
{
	int									nID;

	MonsterBase::MONSTER_ITEM_DROP_INFO item_drop[ MonsterBase::NUMBER_OF_ITEM_DROPS_PER_ROW ];
	_decimal_variant					dv_drop_percentage[10];

BEGIN_ADO_BINDING(dbMonsterItemDrop)
	ADO_VARIABLE_LENGTH_ENTRY4(	1, adInteger, nID, sizeof( nID ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	3, adInteger, item_drop[0].item_id, sizeof( item_drop[0].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2(			4, adDecimal, dv_drop_percentage[0], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	5, adSmallInt, item_drop[0].min_count, sizeof( item_drop[0].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	6, adSmallInt, item_drop[0].max_count, sizeof( item_drop[0].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	7, adSmallInt, item_drop[0].min_level, sizeof( item_drop[0].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	8, adSmallInt, item_drop[0].max_level, sizeof( item_drop[0].max_level ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	9, adInteger, item_drop[1].item_id, sizeof( item_drop[1].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2(			10, adDecimal, dv_drop_percentage[1], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	11, adSmallInt, item_drop[1].min_count, sizeof( item_drop[1].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	12, adSmallInt, item_drop[1].max_count, sizeof( item_drop[1].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	13, adSmallInt, item_drop[1].min_level, sizeof( item_drop[1].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	14, adSmallInt, item_drop[1].max_level, sizeof( item_drop[1].max_level ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	15, adInteger, item_drop[2].item_id, sizeof( item_drop[2].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2(			16, adDecimal, dv_drop_percentage[2], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	17, adSmallInt, item_drop[2].min_count, sizeof( item_drop[2].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	18, adSmallInt, item_drop[2].max_count, sizeof( item_drop[2].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	19, adSmallInt, item_drop[2].min_level, sizeof( item_drop[2].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	20, adSmallInt, item_drop[2].max_level, sizeof( item_drop[2].max_level ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	21, adInteger, item_drop[3].item_id, sizeof( item_drop[3].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2(			22, adDecimal, dv_drop_percentage[3], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	23, adSmallInt, item_drop[3].min_count, sizeof( item_drop[3].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	24, adSmallInt, item_drop[3].max_count, sizeof( item_drop[3].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	25, adSmallInt, item_drop[3].min_level, sizeof( item_drop[3].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	26, adSmallInt, item_drop[3].max_level, sizeof( item_drop[3].max_level ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	27, adInteger, item_drop[4].item_id, sizeof( item_drop[4].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2(			28, adDecimal, dv_drop_percentage[4], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	29, adSmallInt, item_drop[4].min_count, sizeof( item_drop[4].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	30, adSmallInt, item_drop[4].max_count, sizeof( item_drop[4].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	31, adSmallInt, item_drop[4].min_level, sizeof( item_drop[4].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	32, adSmallInt, item_drop[4].max_level, sizeof( item_drop[4].max_level ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	33, adInteger, item_drop[5].item_id, sizeof( item_drop[5].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2(			34, adDecimal, dv_drop_percentage[5], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	35, adSmallInt, item_drop[5].min_count, sizeof( item_drop[5].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	36, adSmallInt, item_drop[5].max_count, sizeof( item_drop[5].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	37, adSmallInt, item_drop[5].min_level, sizeof( item_drop[5].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	38, adSmallInt, item_drop[5].max_level, sizeof( item_drop[5].max_level ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	39, adInteger, item_drop[6].item_id, sizeof( item_drop[6].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2(			40, adDecimal, dv_drop_percentage[6], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	41, adSmallInt, item_drop[6].min_count, sizeof( item_drop[6].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	42, adSmallInt, item_drop[6].max_count, sizeof( item_drop[6].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	43, adSmallInt, item_drop[6].min_level, sizeof( item_drop[6].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	44, adSmallInt, item_drop[6].max_level, sizeof( item_drop[6].max_level ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	45, adInteger, item_drop[7].item_id, sizeof( item_drop[7].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2(			46, adDecimal, dv_drop_percentage[7], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	47, adSmallInt, item_drop[7].min_count, sizeof( item_drop[7].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	48, adSmallInt, item_drop[7].max_count, sizeof( item_drop[7].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	49, adSmallInt, item_drop[7].min_level, sizeof( item_drop[7].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	50, adSmallInt, item_drop[7].max_level, sizeof( item_drop[7].max_level ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	51, adInteger, item_drop[8].item_id, sizeof( item_drop[8].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2(			52, adDecimal, dv_drop_percentage[8], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	53, adSmallInt, item_drop[8].min_count, sizeof( item_drop[8].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	54, adSmallInt, item_drop[8].max_count, sizeof( item_drop[8].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	55, adSmallInt, item_drop[8].min_level, sizeof( item_drop[8].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	56, adSmallInt, item_drop[8].max_level, sizeof( item_drop[8].max_level ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	57, adInteger, item_drop[9].item_id, sizeof( item_drop[9].item_id ), FALSE )
	ADO_NUMERIC_ENTRY2        (	58, adDecimal, dv_drop_percentage[9], 10, 8, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	59, adSmallInt, item_drop[9].min_count, sizeof( item_drop[9].min_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	60, adSmallInt, item_drop[9].max_count, sizeof( item_drop[9].max_count ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	61, adSmallInt, item_drop[9].min_level, sizeof( item_drop[9].min_level ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	62, adSmallInt, item_drop[9].max_level, sizeof( item_drop[9].max_level ), FALSE )
END_ADO_BINDING()
};

struct dbDropGroup : public CADORecordBinding, public DropGroup
{

BEGIN_ADO_BINDING(dbDropGroup)

	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, uid, sizeof( uid ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(	2, adInteger, drop_item_id[0], sizeof( drop_item_id[0] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	3, adInteger, drop_min_count[0], sizeof( drop_min_count[0] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	4, adInteger, drop_max_count[0], sizeof( drop_max_count[0] ), FALSE )
	ADO_NUMERIC_ENTRY2(			5, adDecimal, dv_drop_percentage[0],	10, 2, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	6, adInteger, drop_item_id[1], sizeof( drop_item_id[1] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	7, adInteger, drop_min_count[1], sizeof( drop_min_count[1] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	8, adInteger, drop_max_count[1], sizeof( drop_max_count[1] ), FALSE )
	ADO_NUMERIC_ENTRY2(			9, adDecimal, dv_drop_percentage[1],	10, 2, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	10, adInteger, drop_item_id[2], sizeof( drop_item_id[2] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	11, adInteger, drop_min_count[2], sizeof( drop_min_count[2] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	12, adInteger, drop_max_count[2], sizeof( drop_max_count[2] ), FALSE )
	ADO_NUMERIC_ENTRY2(			13, adDecimal, dv_drop_percentage[2],	10, 2, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	14, adInteger, drop_item_id[3], sizeof( drop_item_id[3] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	15, adInteger, drop_min_count[3], sizeof( drop_min_count[3] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	16, adInteger, drop_max_count[3], sizeof( drop_max_count[3] ), FALSE )
	ADO_NUMERIC_ENTRY2(			17, adDecimal, dv_drop_percentage[3],	10, 2, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	18, adInteger, drop_item_id[4], sizeof( drop_item_id[4] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	19, adInteger, drop_min_count[4], sizeof( drop_min_count[4] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	20, adInteger, drop_max_count[4], sizeof( drop_max_count[4] ), FALSE )
	ADO_NUMERIC_ENTRY2(			21,	adDecimal, dv_drop_percentage[4],	10, 2, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	22, adInteger, drop_item_id[5], sizeof( drop_item_id[5] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	23, adInteger, drop_min_count[5], sizeof( drop_min_count[5] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	24, adInteger, drop_max_count[5], sizeof( drop_max_count[5] ), FALSE )
	ADO_NUMERIC_ENTRY2(			25,	adDecimal, dv_drop_percentage[5],	10, 2, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	26, adInteger, drop_item_id[6], sizeof( drop_item_id[6] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	27, adInteger, drop_min_count[6], sizeof( drop_min_count[6] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	28, adInteger, drop_max_count[6], sizeof( drop_max_count[6] ), FALSE )
	ADO_NUMERIC_ENTRY2(			29,	adDecimal, dv_drop_percentage[6],	10, 2, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	30, adInteger, drop_item_id[7], sizeof( drop_item_id[7] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	31, adInteger, drop_min_count[7], sizeof( drop_min_count[7] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	32, adInteger, drop_max_count[7], sizeof( drop_max_count[7] ), FALSE )
	ADO_NUMERIC_ENTRY2(			33,	adDecimal, dv_drop_percentage[7],	10, 2, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	34, adInteger, drop_item_id[8], sizeof( drop_item_id[8] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	35, adInteger, drop_min_count[8], sizeof( drop_min_count[8] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	36, adInteger, drop_max_count[8], sizeof( drop_max_count[8] ), FALSE )
	ADO_NUMERIC_ENTRY2(			37,	adDecimal, dv_drop_percentage[8],	10, 2, FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	38, adInteger, drop_item_id[9]      , sizeof( drop_item_id[9] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	39, adInteger, drop_min_count[9], sizeof( drop_min_count[9] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(	40, adInteger, drop_max_count[9], sizeof( drop_max_count[9] ), FALSE )
	ADO_NUMERIC_ENTRY2        (	41, adDecimal, dv_drop_percentage[9],	10, 2, FALSE )
	
END_ADO_BINDING()

	_decimal_variant			dv_drop_percentage[10];

};

struct dbSummon : public CADORecordBinding, public SummonBase
{
BEGIN_ADO_BINDING(dbSummon)

	ADO_VARIABLE_LENGTH_ENTRY4(1, 	adInteger, uid,			sizeof( uid ), FALSE )
	// 2 - model_id
	ADO_VARIABLE_LENGTH_ENTRY4(3, 	adInteger, name_id,		sizeof( name_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(4, 	adInteger, type,		sizeof( type ), FALSE)	

	// 5 - affiliation ( 세부 계열 )
	// 6 - species_id  ( 종명 ID )
	// 7 - color ( 컬러 - 클라사용 )

	ADO_VARIABLE_LENGTH_ENTRY4(8,  	adInteger, magic_type,	sizeof( magic_type ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(9,  	adTinyInt, _rate,		sizeof( _rate ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(10, 	adInteger, stat_id,		sizeof( stat_id ), FALSE)	
	ADO_NUMERIC_ENTRY2(	11,		   	adDecimal, dv_size,	10, 2, FALSE)	
	ADO_NUMERIC_ENTRY2(	12,		   	adDecimal, dv_scale,	10, 2, FALSE)	

	// 13 - target_fx_size
	// 14 - standard_walk_speed
	// 15 - standard_run_speed

	ADO_VARIABLE_LENGTH_ENTRY4(16, 	adInteger, riding_speed, sizeof( riding_speed ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(17, 	adInteger, run_speed,	sizeof( run_speed ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(18, 	adTinyInt, is_riding_only, sizeof( is_riding_only ), FALSE )

	// 19 - riding_motion_type

	ADO_NUMERIC_ENTRY2(	20, adDecimal, dv_attack_range,	10, 2, FALSE)	

	// 21 - walk_type
	// 22 - slant_type
	// 23 - material
	// 24 - attack_motion_speed
	// 25 - from

	ADO_VARIABLE_LENGTH_ENTRY4(26, adInteger, evolve_type, sizeof( evolve_type ), FALSE)	
	ADO_VARIABLE_LENGTH_ENTRY4(27, adInteger, evolve_target, sizeof( evolve_target ), FALSE)	

	// 28 - camera_x
	// 29 - camera_y
	// 30 - camera_z
	// 31 - target_x
	// 32 - target_y
	// 33 - target_z
	// 34 - model
	// 35 - motion_file_id
	// 36 - face_id
	// 37 - face_file_name

	ADO_VARIABLE_LENGTH_ENTRY4(38, adInteger, card_id, sizeof( card_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(39, adVarChar, szScript, _countof( szScript ), FALSE )

	// 40 - illust_file_name
	// 41 - text_feature_id
	// 42 - text_name_id
	ADO_VARIABLE_LENGTH_ENTRY4(43, adInteger, skill_tree_id[0], sizeof( skill_tree_id[0] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(44, adInteger, skill_tree_id[1], sizeof( skill_tree_id[1] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(45, adInteger, skill_tree_id[2], sizeof( skill_tree_id[2] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(46, adInteger, skill_tree_id[3], sizeof( skill_tree_id[3] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(47, adInteger, skill_tree_id[4], sizeof( skill_tree_id[4] ), FALSE )	
	// 48 - texture_group

	ADO_VARIABLE_LENGTH_ENTRY4(49, adInteger, local_flag, sizeof( local_flag ), FALSE )

END_ADO_BINDING()

	char						_rate;
	_decimal_variant			dv_size, dv_scale, dv_attack_range;
	char						szScript[512];

	int							local_flag;
};

struct dbSummonDefaultName : public CADORecordBinding
{

BEGIN_ADO_BINDING(dbSummonDefaultName)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, is_post_fix, sizeof( is_post_fix ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, text_id, sizeof( text_id ), FALSE)	
END_ADO_BINDING()

	int is_post_fix;
	int text_id;
};

struct dbSummonUniqueName : public CADORecordBinding
{

BEGIN_ADO_BINDING(dbSummonUniqueName)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, summon_type, sizeof( summon_type ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, text_id, sizeof( text_id ), FALSE)	
END_ADO_BINDING()

	int summon_type;
	int text_id;
};

struct dbPet : public CADORecordBinding, public PetBase
{
BEGIN_ADO_BINDING(dbPet)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, uid, sizeof( uid ), FALSE )
	// 2 - species_id
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, type, sizeof( type ), FALSE )
	// 4 - affiliation
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, name_id, sizeof( name_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, cage_id, sizeof( cage_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, skill_tree_id, sizeof( skill_tree_id ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, attribute_flag, sizeof( attribute_flag ), FALSE )
	ADO_NUMERIC_ENTRY2(			9, adDecimal, dv_size, 10, 2, FALSE )
	ADO_NUMERIC_ENTRY2(			10, adDecimal, dv_scale, 10, 2, FALSE )
																									
	ADO_VARIABLE_LENGTH_ENTRY4( 23, adInteger, local_flag, sizeof( local_flag ), FALSE )
END_ADO_BINDING()

	_decimal_variant			dv_size, dv_scale;
	int							local_flag;
};

static void onSummonDefaultName( dbSummonDefaultName * emprs )
{
	GameContent::RegisterSummonDefaultName( emprs->text_id, !emprs->is_post_fix );
}

static void onSummonUniqueName( dbSummonUniqueName * emprs )
{
	GameContent::RegisterSummonUniqueName( emprs->summon_type, emprs->text_id );
}

static void onMonsterInfo( dbMonster * monster )
{
	extern volatile int g_nCurrentLocalFlag;

	//assert(monster->uid != 104007);

	//if( g_nCurrentLocalFlag & monster->local_flag )
	//{
		//return;
	//}

	// 크기
	monster->size			= monster->dv_size.getFloat();
	monster->scale			= monster->dv_scale.getFloat();
	monster->attack_range	= monster->dv_attack_range.getFloat() * GameRule::ATTACK_RANGE_UNIT;
	monster->hidesense_range = monster->dv_hidesense_range.getFloat() * GameRule::DEFAULT_UNIT_SIZE;

	// 시야 거리(DB값을 게임 내 단위로 변경)
	monster->visible_range *= GameRule::DEFAULT_UNIT_SIZE;

	// 공격타입
	monster->attack_type = 0;
	if( monster->f_first_attack )		monster->attack_type |= MonsterBase::AT_FIRST_ATTACK;
	if( monster->f_group_first_attack )	monster->attack_type |= MonsterBase::AT_GROUP_FIRST_ATTACK;
	if( monster->f_response_casting )	monster->attack_type |= MonsterBase::AT_RESPONSE_CASTING;
	if( monster->f_response_race )		monster->attack_type |= MonsterBase::AT_RESPONSE_RACE;
	if( monster->f_response_battle )	monster->attack_type |= MonsterBase::AT_RESPONSE_BATTLE;

	// 테이밍
	monster->taming_percentage.set( monster->dv_taming_percentage.getMultipleInteger( 10000 ) );
	monster->taming_exp_adjust			= monster->dv_taming_exp_mod.getFloat();

	// 몬스터 사냥시 획득 경험치, 루피, 라크량 2세트 적용 여부 체크
	if( ENV().GetInt( "game.change_monster_drop_set", 0 ) )
	{
		monster->exp = monster->exp_2;
		monster->jp = monster->jp_2;

		monster->gold_min = monster->gold_min_2;
		monster->gold_max = monster->gold_max_2;

		monster->chaos_min = monster->chaos_min_2;
		monster->chaos_max = monster->chaos_max_2;
	}

	monster->trigger_list = GameContent::GetMonsterTriggerInfo( monster->skill_trigger_id );
	monster->skill_info_list = GameContent::GetMonsterSkillInfo( monster->skill_trigger_id );
	monster->item_drop_list = GameContent::GetMonsterDropTable( monster->item_drop_table_id );

	// 실행할 내용이 없는 트리거는 길이를 0으로 만들어 줌
	if( !strcmp( monster->script_on_dead, "0" ) || !strcmp( monster->script_on_dead, " " ) )
		monster->script_on_dead[ 0 ] = NULL;

	GameContent::RegisterMonsterInfo( *monster );
	WorldLocationManager::Instance().RegisterMonsterLocation( monster->location_id, monster->uid );
}

void onMonsterSkillInfo( dbMonsterSkill * emprs )
{
	static std::vector< MonsterBase::MONSTER_SKILL_INFO > vSkillInfo;
	static std::vector< MonsterBase::MONSTER_TRIGGER > vTriggerInfo;
	static int nLastID = 0;

	if( nLastID != emprs->nID )
	{
		if( !vSkillInfo.empty() )
			GameContent::RegisterMonsterSkillInfo( nLastID, vSkillInfo );

		if( !vTriggerInfo.empty() )
			GameContent::RegisterMonsterTriggerInfo( nLastID, vTriggerInfo );

		vSkillInfo.clear();
		vTriggerInfo.clear();
		nLastID = emprs->nID;
	}

	for( int i = 0 ; i < MonsterBase::NUMBER_OF_SKILLS_PER_ROW ; ++i )
	{
		// 스킬 ID가 지정되지 않은 항목은 등록하지 않음
		if( !emprs->skill_info[ i ].skill_id )
			continue;

		emprs->skill_info[ i ].skill_probability = emprs->dv_skill_probability[ i ].getFloat();
		vSkillInfo.push_back( emprs->skill_info[ i ] );
	}
	for( int i = 0 ; i < MonsterBase::NUMBER_OF_TRIGGERS_PER_ROW ; ++i )
	{
		// 실행할 내용이 없는 트리거는 등록하지 않음
		if( !strlen( emprs->trigger[ i ].script ) || !strcmp( emprs->trigger[ i ].script, "0" ) )
			continue;

		// 조건이 지정되지 않은 트리거는 등록하지 않음
		if( emprs->trigger[ i ].id == MonsterBase::TRIGGER_TYPE_NONE )
			continue;

		emprs->trigger[ i ].value[ 0 ] = emprs->dv_trigger[ i ][ 0 ].getFloat();
		emprs->trigger[ i ].value[ 1 ] = emprs->dv_trigger[ i ][ 1 ].getFloat();

		vTriggerInfo.push_back( emprs->trigger[ i ] );
	}
}

void onMonsterItemDropInfo( dbMonsterItemDrop * emprs )
{
	static std::vector< MonsterBase::MONSTER_ITEM_DROP_INFO > vItemDropInfo;
	static int nLastID = 0;

	if( nLastID != emprs->nID )
	{
		if( !vItemDropInfo.empty() )
			GameContent::RegisterMonsterDropTable( nLastID, vItemDropInfo );

		vItemDropInfo.clear();
		nLastID = emprs->nID;
	}

	for( int i = 0 ; i < MonsterBase::NUMBER_OF_ITEM_DROPS_PER_ROW ; ++i )
	{
		// 아이템 코드가 0이거나 최소&최대 드랍 수량이 둘 다 0인 경우 등록하지 않음
		if( !emprs->item_drop[ i ].item_id || ( !emprs->item_drop[ i ].min_count && !emprs->item_drop[ i ].max_count ) )
			continue;

		if( emprs->item_drop[i].min_count > emprs->item_drop[i].max_count ||
			emprs->item_drop[i].min_level > emprs->item_drop[i].max_level )
		{
			_cprint( "item drop table min max error, dropid(%d), itemid(%d), min_count(%d), max_count(%d), min_level(%d), max_level(%d)\n",
				emprs->nID, emprs->item_drop[i].item_id,
				emprs->item_drop[i].min_count, emprs->item_drop[i].max_count,
				emprs->item_drop[i].min_level, emprs->item_drop[i].max_level );

			FILELOG( "item drop table min max error, dropid(%d), itemid(%d), min_count(%d), max_count(%d), min_level(%d), max_level(%d)",
				emprs->nID, emprs->item_drop[i].item_id,
				emprs->item_drop[i].min_count, emprs->item_drop[i].max_count,
				emprs->item_drop[i].min_level, emprs->item_drop[i].max_level );
		}

		emprs->item_drop[ i ].percentage = emprs->dv_drop_percentage[ i ].getMultipleInteger( 100000000 );

		vItemDropInfo.push_back( emprs->item_drop[ i ] );
	}

}

static void onDropGroupInfo( dbDropGroup * dropGroup )
{
	// 드랍 확률
	for( int i = 0; i < 10; ++i )
	{		
		dropGroup->drop_percentage[i] = dropGroup->dv_drop_percentage[i].getFloat() * 100000000;
	}

	GameContent::RegisterDropGroupInfo( *dropGroup );
}

static void onDropGroupLocalInfo( dbDropGroup * dropGroup )
{
	// 이미 로드되어 있는 데이터면 무시
	if( GameContent::GetDropGroupInfo( dropGroup->uid )->uid )
	{
		_cprint( "Already loaded resource loaded from [DropGroupLocalResource]. id(%d)\n", dropGroup->uid );
		FILELOG( "Already loaded resource loaded from [DropGroupLocalResource]. id(%d)", dropGroup->uid );
		return;
	}

	onDropGroupInfo( dropGroup );
}

static void onPetInfo( dbPet * emprs )
{
	extern volatile int g_nCurrentLocalFlag;
	if( emprs->local_flag & g_nCurrentLocalFlag )
		return;

	emprs->size = emprs->dv_size.getFloat();
	emprs->scale = emprs->dv_scale.getFloat();

	GameContent::RegisterPetInfo( *emprs );
}

static bool LoadMonsterData()
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbMonsterSkill >( "SELECT * FROM dbo.MonsterSkillResource ORDER BY id, sub_id", ConnPtr, onMonsterSkillInfo, adCmdText );
	// 마지막으로 로드된 채로 등록 안 된 스킬 정보를 등록하도록 더미 데이터를 한 번 더 입력(onMonsterSkillInfo 함수 구현 참조)
	{
		dbMonsterSkill dbDummy;
		dbDummy.nID = -1;
		for( int i = 0 ; i < MonsterBase::NUMBER_OF_SKILLS_PER_ROW ; ++i )
			dbDummy.skill_info[ i ].skill_id = 0;
		for( int i = 0 ; i < MonsterBase::NUMBER_OF_TRIGGERS_PER_ROW ; ++i )
			dbDummy.trigger[ i ].id = 0;
		onMonsterSkillInfo( &dbDummy );
	}
	_cprint( "Total %d Monster skill/trigger info loaded...\n", cnt );
	FILELOG( "Total %d Monster skill/trigger info loaded...", cnt );

	cnt = LoadDbResource< dbMonsterItemDrop >( "SELECT * FROM dbo.MonsterDropTableResource ORDER BY id, sub_id", ConnPtr, onMonsterItemDropInfo, adCmdText );
	// 마지막으로 로드된 채로 등록 안 된 아이템 드랍 정보를 등록하도록 더미 데이터를 한 번 더 입력(onMonsterItemDropInfo 함수 구현 참조)
	{
		dbMonsterItemDrop dbDummy;
		dbDummy.nID = -1;
		for( int i = 0 ; i < MonsterBase::NUMBER_OF_ITEM_DROPS_PER_ROW ; ++i )
			dbDummy.item_drop[ i ].item_id = 0;
		onMonsterItemDropInfo( &dbDummy );
	}
	_cprint( "Total %d Monster item drop info loaded...\n", cnt );
	FILELOG( "Total %d Monster item drop info loaded...", cnt );

	cnt = LoadDbResource< dbMonster >( "MonsterResource", ConnPtr, onMonsterInfo );
	_cprint( "Total %d Monster info loaded...\n", cnt );
	FILELOG( "Total %d Monster info loaded...", cnt );

	return true;
}

static bool LoadDropGroupData()
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbDropGroup >( "DropGroupResource", ConnPtr, onDropGroupInfo );

	_cprint( "Total %d DropGroup info loaded...\n", cnt );
	FILELOG( "Total %d DropGroup info loaded...", cnt );

	try
	{
		cnt = 0;
		cnt = LoadDbResource< dbDropGroup >( "DropGroupLocalResource", ConnPtr, onDropGroupLocalInfo );
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

	_cprint( "Total %d DropGroup(local) info loaded...\n", cnt );
	FILELOG( "Total %d DropGroup(local) info loaded...", cnt );

	return true;
}

static bool LoadSummonNames()
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbSummonDefaultName >( "SummonDefaultNameResource", ConnPtr, onSummonDefaultName );
	_cprint( "Total %d Summon Default Name loaded...\n", cnt );
	FILELOG( "Total %d Summon Default Name loaded...", cnt );

	cnt = LoadDbResource< dbSummonUniqueName >( "SummonUniqueNameResource", ConnPtr, onSummonUniqueName );
	_cprint( "Total %d Summon Unique Name loaded...\n", cnt );
	FILELOG( "Total %d Summon Unique Name loaded...", cnt );

	return true;
}

static void onSummonInfo( dbSummon * Summon )
{
	extern volatile int g_nCurrentLocalFlag;

	if( g_nCurrentLocalFlag & Summon->local_flag )
	{
		return;
	}

	Summon->rate = Summon->_rate;
	Summon->size = Summon->dv_size.getFloat();
	Summon->scale = Summon->dv_scale.getFloat();
	Summon->attack_range = Summon->dv_attack_range.getFloat() * GameRule::ATTACK_RANGE_UNIT;
	Summon->strScript = Summon->szScript;

	for( int i = 0 ; i < GameRule::MAX_SUMMON_SKILL_TREE ; ++i )
		Summon->skill_tree[i] = NULL;

	GameContent::RegisterSummonInfo( *Summon );
}

static bool LoadSummonData()
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbSummon >( "SummonResource", ConnPtr, onSummonInfo );

	_cprint( "Total %d Summon info loaded...\n", cnt );
	FILELOG( "Total %d Summon info loaded...", cnt );

	return true;
}

struct dbSummonLevelBonus : public CADORecordBinding
{
	int								id;
	_decimal_variant				cst_stat_bonus[7];

BEGIN_ADO_BINDING(dbSummonLevelBonus)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, id, sizeof( id ), FALSE )

	ADO_NUMERIC_ENTRY2( 2,	adDecimal, cst_stat_bonus[0],	10, 2, FALSE )
	ADO_NUMERIC_ENTRY2( 3,	adDecimal, cst_stat_bonus[1],	10, 2, FALSE )
	ADO_NUMERIC_ENTRY2( 4,	adDecimal, cst_stat_bonus[2],	10, 2, FALSE )
	ADO_NUMERIC_ENTRY2( 5,	adDecimal, cst_stat_bonus[3],	10, 2, FALSE )
	ADO_NUMERIC_ENTRY2( 6,	adDecimal, cst_stat_bonus[4],	10, 2, FALSE )
	ADO_NUMERIC_ENTRY2( 7,	adDecimal, cst_stat_bonus[5],	10, 2, FALSE )
	ADO_NUMERIC_ENTRY2( 8,	adDecimal, cst_stat_bonus[6],	10, 2, FALSE )
END_ADO_BINDING()
};

static void onSummonLevelBonusInfo( dbSummonLevelBonus * emprs )
{
	// 소환수 종류별 스텟 보너스 정보 등록
	SummonLevelBonus bonus;

	bonus.id = emprs->id;
	bonus.strength.set( emprs->cst_stat_bonus[0].getMultipleInteger( 10000 ) );
	bonus.vitality.set( emprs->cst_stat_bonus[1].getMultipleInteger( 10000 ) );
	bonus.dexterity.set( emprs->cst_stat_bonus[2].getMultipleInteger( 10000 ) );
	bonus.agility.set( emprs->cst_stat_bonus[3].getMultipleInteger( 10000 ) );
	bonus.intelligence.set( emprs->cst_stat_bonus[4].getMultipleInteger( 10000 ) );
	bonus.mentality.set( emprs->cst_stat_bonus[5].getMultipleInteger( 10000 ) );
	bonus.luck.set( emprs->cst_stat_bonus[6].getMultipleInteger( 10000 ) );

	GameContent::RegisterSummonLevelBonusInfo( bonus );
}

static bool LoadSummonLevelBonusData()
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbSummonLevelBonus >( "CreatureLevelBonus", ConnPtr, onSummonLevelBonusInfo );

	_cprint( "Total %d Summon Level Bonus info loaded...\n", cnt );
	FILELOG( "Total %d Summon Level Bonus info loaded...", cnt );

	return true;
}


struct dbMonsterCreature : public CADORecordBinding, public MonsterCreatureInfo
{
BEGIN_ADO_BINDING(dbMonsterCreature)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, nSid,				 sizeof( nSid ),			  FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, nUseCode,			 sizeof( nUseCode ),		  FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, arSummonCode[ 0 ], sizeof( arSummonCode[ 0 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, arSummonCode[ 1 ], sizeof( arSummonCode[ 1 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, arSummonCode[ 2 ], sizeof( arSummonCode[ 2 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, arSummonCode[ 3 ], sizeof( arSummonCode[ 3 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, arSummonCode[ 4 ], sizeof( arSummonCode[ 4 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, arSummonCode[ 5 ], sizeof( arSummonCode[ 5 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, arSummonCode[ 6 ], sizeof( arSummonCode[ 6 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, arSummonCode[ 7 ], sizeof( arSummonCode[ 7 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 11, adInteger, arSummonCode[ 8 ], sizeof( arSummonCode[ 8 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 12, adInteger, arSummonCode[ 9 ], sizeof( arSummonCode[ 9 ] ), FALSE )
END_ADO_BINDING()
};

static void onMonsterCreatureInfo( dbMonsterCreature * emprs )
{
	MonsterCreatureInfo kInfo;
	kInfo.nSid = emprs->nSid;
	kInfo.nUseCode = emprs->nUseCode;
	s_memcpy( kInfo.arSummonCode, sizeof(kInfo.arSummonCode), emprs->arSummonCode, sizeof(kInfo.arSummonCode) ); 
	GameContent::RegisterMonsterCreatureInfo( kInfo );
}

static bool LoadMonsterCreatureData()
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbMonsterCreature >( "MonsterCreatureResource", ConnPtr, onMonsterCreatureInfo );

	_cprint( "Total %d Monster Creature info loaded...\n", cnt );
	FILELOG( "Total %d Monster Creature info loaded...", cnt );

	return true;
}


struct dbSummonRandomSkill : public CADORecordBinding, public SummonRandomSkillInfo
{
BEGIN_ADO_BINDING(dbSummonRandomSkill)
	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, nSid,				 	sizeof( nSid ),					FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, nUseCode,		 		sizeof( nUseCode ),				FALSE )
	// 매크로의 상수 루프 표현식이 가능하다면 아래 코드를 간단히 할 수 있을 듯
	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, arSummonSkillID[ 0 ], sizeof( arSummonSkillID[ 0 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, arSummonSkillID[ 1 ], sizeof( arSummonSkillID[ 1 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, arSummonSkillID[ 2 ], sizeof( arSummonSkillID[ 2 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, arSummonSkillID[ 3 ], sizeof( arSummonSkillID[ 3 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, arSummonSkillID[ 4 ], sizeof( arSummonSkillID[ 4 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, arSummonSkillID[ 5 ], sizeof( arSummonSkillID[ 5 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, arSummonSkillID[ 6 ], sizeof( arSummonSkillID[ 6 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, arSummonSkillID[ 7 ], sizeof( arSummonSkillID[ 7 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 11, adInteger, arSummonSkillID[ 8 ], sizeof( arSummonSkillID[ 8 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 12, adInteger, arSummonSkillID[ 9 ], sizeof( arSummonSkillID[ 9 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 13, adInteger, arSummonSkillID[ 10 ], sizeof( arSummonSkillID[ 10 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 14, adInteger, arSummonSkillID[ 11 ], sizeof( arSummonSkillID[ 11 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 15, adInteger, arSummonSkillID[ 12 ], sizeof( arSummonSkillID[ 12 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 16, adInteger, arSummonSkillID[ 13 ], sizeof( arSummonSkillID[ 13 ] ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 17, adInteger, arSummonSkillID[ 14 ], sizeof( arSummonSkillID[ 14 ] ), FALSE )
END_ADO_BINDING()
};

static void onSummonRandomSkillInfo( dbSummonRandomSkill * emprs )
{
	SummonRandomSkillInfo kInfo;
	kInfo.nSid = emprs->nSid;
	kInfo.nUseCode = emprs->nUseCode;
	s_memcpy( kInfo.arSummonSkillID, sizeof(kInfo.arSummonSkillID), emprs->arSummonSkillID, sizeof(kInfo.arSummonSkillID) ); 
	GameContent::RegisterSummonRandomSkillInfo( kInfo );
}

static bool LoadSummonRandomSkillData()
{	
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbSummonRandomSkill >( "SummonRandomSkillResource", ConnPtr, onSummonRandomSkillInfo );

	_cprint( "Total %d Monster Creature info loaded...\n", cnt );
	FILELOG( "Total %d Monster Creature info loaded...", cnt );

	return true;
}

bool MonsterLoader::onProcess( int nThreadNum )
{
	HRESULT  hr = S_OK;
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadMonsterData();
		LoadSummonData();
		LoadSummonLevelBonusData();
		LoadDropGroupData();
		LoadSummonNames();
		LoadMonsterCreatureData();
		LoadSummonRandomSkillData();
		helper.end( "MonsterLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "MonsterLoader" );
		LogDBError( e, "MonsterLoader", "MonsterLoader::onProcess()" );

		std::string strError = "MONSTER RESOURCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}

bool PetLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		_ConnectionPtr		ConnPtr = NULL;

		InitContentDbConnection( ConnPtr );

		helper.start();
		size_t cnt = LoadDbResource< dbPet >( "PetResource", ConnPtr, onPetInfo );
		helper.end( "PetLoader" );

		_cprint( "Total %d Pet info loaded...\n", cnt );
		FILELOG( "Total %d Pet info loaded...", cnt );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "PetLoader" );
		LogDBError( e, "PetLoader", "PetResource" );

		std::string strError = "PET RESOURCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}
