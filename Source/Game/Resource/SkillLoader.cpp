
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbSkill : public CADORecordBinding, public SkillBase
{
	_decimal_variant			  cst_delay_cast;
	_decimal_variant			  cst_delay_cast_per_skl;
	_decimal_variant			  cst_delay_cast_mod_per_enhance;
	_decimal_variant			  cst_delay_common;
	_decimal_variant			  cst_delay_cooltime;
	_decimal_variant			  cst_delay_cooltime_per_skl;
	_decimal_variant			  cst_delay_cooltime_mod_per_enhance;

	_decimal_variant			  cst_state_level_per_skl;
	_decimal_variant			  cst_state_level_per_enhance;
	_decimal_variant			  cst_state_time;
	_decimal_variant			  cst_state_time_per_skl;
	_decimal_variant			  cst_state_time_per_enhance;

	_decimal_variant			  cst_cost_hp_per;
	_decimal_variant			  cst_cost_hp_per_skl_per;
	_decimal_variant			  cst_cost_mp_per;
	_decimal_variant			  cst_cost_mp_per_skl_per;
	_decimal_variant			  cst_cost_energy;
	_decimal_variant			  cst_cost_energy_per_skl;

	_decimal_variant			  cst_hate_mod;
	_decimal_variant			  cst_hate_per_skl;
	_decimal_variant			  cst_hate_per_enhance;

	_decimal_variant			  cst_var[ SkillBase::MAX_SKILL_VALUE ];

	_decimal_variant			  cst_projectile_speed;
	_decimal_variant			  cst_projectile_acceleration;

	static void onSkillInfo( dbSkill * emprs );

BEGIN_ADO_BINDING(dbSkill)

	ADO_VARIABLE_LENGTH_ENTRY4(1, 	adInteger, uid,					sizeof(uid),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, 	adInteger, name_id,				sizeof(name_id),			FALSE)
	
	ADO_VARIABLE_LENGTH_ENTRY4(5, 	adTinyInt, is_valid,			sizeof(is_valid),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, 	adTinyInt, elemental,			sizeof(elemental),			FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(7, 	adTinyInt, is_active,			sizeof(is_active),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, 	adTinyInt, is_spell_act,		sizeof(is_spell_act),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, 	adTinyInt, is_harmful,			sizeof(is_harmful),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, 	adTinyInt, is_need_target,		sizeof(is_need_target),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, 	adTinyInt, is_corpse,			sizeof(is_corpse),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, 	adTinyInt, is_toggle,			sizeof(is_toggle),			FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(13, 	adInteger, toggle_group,		sizeof(toggle_group),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, 	adTinyInt, cancel_type,			sizeof(cancel_type),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, 	adTinyInt, cancel_level,		sizeof(cancel_level),		FALSE)
	
	ADO_VARIABLE_LENGTH_ENTRY4(16, 	adInteger, cast_range,			sizeof(cast_range),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(17, 	adInteger, valid_range,			sizeof(valid_range),		FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(18,	adInteger, cost_hp,				sizeof(cost_hp),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(19,	adInteger, cost_hp_per_skl,		sizeof(cost_hp_per_skl),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(20,	adInteger, cost_mp,				sizeof(cost_mp),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(21,	adInteger, cost_mp_per_skl,		sizeof(cost_mp_per_skl),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(22,	adInteger, cost_mp_per_enhance,	sizeof(cost_mp_per_enhance),FALSE)
	ADO_NUMERIC_ENTRY2( 23,	adDecimal, cst_cost_hp_per,				10, 2, FALSE)
	ADO_NUMERIC_ENTRY2( 24,	adDecimal, cst_cost_hp_per_skl_per,		10, 2, FALSE)
	ADO_NUMERIC_ENTRY2( 25,	adDecimal, cst_cost_mp_per,				10, 2, FALSE)
	ADO_NUMERIC_ENTRY2( 26,	adDecimal, cst_cost_mp_per_skl_per,		10, 2, FALSE)
	ADO_NUMERIC_ENTRY2( 29,	adDecimal, cst_cost_energy,				10, 2, FALSE)
	ADO_NUMERIC_ENTRY2( 30,	adDecimal, cst_cost_energy_per_skl,		10, 2, FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(31,	adInteger, cost_exp,			sizeof(cost_exp),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(32,	adInteger, cost_exp_per_enhance,sizeof(cost_exp_per_enhance),FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(33,	adInteger, cost_jp,				sizeof(cost_jp),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(34,	adInteger, cost_jp_per_enhance,	sizeof(cost_jp_per_enhance),FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(35,	adInteger, cost_item,			sizeof(cost_item),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(36,	adInteger, cost_item_count,		sizeof(cost_item_count),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(37,	adInteger, cost_item_count_per_skl,		sizeof(cost_item_count_per_skl),	FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(38,	adInteger, need_level,			sizeof(need_level),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(39,	adInteger, need_hp,				sizeof(need_hp),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(40,	adInteger, need_mp,				sizeof(need_mp),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(43,	adInteger, need_state_id,		sizeof(need_state_id),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(44,	adTinyInt, need_state_level,	sizeof(need_state_level),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(45,	adTinyInt, need_state_exhaust,	sizeof(need_state_exhaust),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(46,	adTinyInt, vf_one_hand_sword,	sizeof(vf_one_hand_sword),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(47,	adTinyInt, vf_two_hand_sword,	sizeof(vf_two_hand_sword),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(48,	adTinyInt, vf_double_sword,		sizeof(vf_double_sword),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(49,	adTinyInt, vf_dagger,			sizeof(vf_double_dagger),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(50,	adTinyInt, vf_double_dagger,	sizeof(vf_double_dagger),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(51,	adTinyInt, vf_spear,			sizeof(vf_spear),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(52,	adTinyInt, vf_axe,				sizeof(vf_axe),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(53,	adTinyInt, vf_one_hand_axe,		sizeof(vf_one_hand_axe),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(54,	adTinyInt, vf_double_axe,		sizeof(vf_double_axe),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(55,	adTinyInt, vf_one_hand_mace,	sizeof(vf_one_hand_mace),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(56,	adTinyInt, vf_two_hand_mace,	sizeof(vf_two_hand_mace),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(57,	adTinyInt, vf_lightbow,			sizeof(vf_lightbow),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(58,	adTinyInt, vf_heavybow,			sizeof(vf_heavybow),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(59,	adTinyInt, vf_crossbow,			sizeof(vf_crossbow),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(60,	adTinyInt, vf_one_hand_staff,	sizeof(vf_one_hand_staff),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(61,	adTinyInt, vf_two_hand_staff,	sizeof(vf_two_hand_staff),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(62,	adTinyInt, vf_shield_only,		sizeof(vf_shield_only),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(63,	adTinyInt, vf_is_not_need_weapon,	sizeof(vf_is_not_need_weapon),	FALSE)

	ADO_NUMERIC_ENTRY2( 64,	adDecimal, cst_delay_cast,						10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 65,	adDecimal, cst_delay_cast_per_skl,				10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 66,	adDecimal, cst_delay_cast_mod_per_enhance,		10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 67,	adDecimal, cst_delay_common,					10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 68,	adDecimal, cst_delay_cooltime,					10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 69, adDecimal, cst_delay_cooltime_per_skl,			10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 70,	adDecimal, cst_delay_cooltime_mod_per_enhance,	10,2,FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(71,	adInteger, cool_time_group_id,	sizeof(cool_time_group_id),	FALSE)			
	ADO_VARIABLE_LENGTH_ENTRY4(72,	adTinyInt, uf_self,				sizeof(uf_self),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(73,	adTinyInt, uf_party,			sizeof(uf_party),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(74,	adTinyInt, uf_guild,			sizeof(uf_guild),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(75,	adTinyInt, uf_neutral,			sizeof(uf_neutral),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(76,	adTinyInt, uf_purple,			sizeof(uf_purple),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(77,	adTinyInt, uf_enemy,			sizeof(uf_enemy),			FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(78,	adTinyInt, tf_avatar,			sizeof(tf_avatar),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(79,	adTinyInt, tf_summon,			sizeof(tf_summon),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(80,	adTinyInt, tf_monster,			sizeof(tf_monster),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(81,	adTinyInt, skill_lvup_limit,	sizeof(skill_lvup_limit),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(82,	adSmallInt, target,				sizeof(target),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(83,	adSmallInt, effect_type,		sizeof(effect_type),		FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(84,	adInteger, skill_enchant_link_id,	sizeof(skill_enchant_link_id),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(85,	adInteger,	state_id,			sizeof(state_id),			FALSE)			
	ADO_VARIABLE_LENGTH_ENTRY4(86,	adInteger,	state_level_base,	sizeof(state_level_base),	FALSE)			
	ADO_NUMERIC_ENTRY2( 87,	adDecimal, cst_state_level_per_skl,		10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 88,	adDecimal, cst_state_level_per_enhance,	10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 89,	adDecimal, cst_state_time,				10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 90,	adDecimal, cst_state_time_per_skl,		10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 91,	adDecimal, cst_state_time_per_enhance,	10,2,FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(92,	adInteger, probability_on_hit,				sizeof(probability_on_hit),			FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(93,	adInteger, probability_inc_by_slv,				sizeof(probability_inc_by_slv),			FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(94,	adSmallInt, hit_bonus,				sizeof(hit_bonus),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(95,	adSmallInt, hit_bonus_enhace,		sizeof(hit_bonus_enhace),	FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(96,	adSmallInt, percentage,				sizeof(percentage),				FALSE)
	ADO_NUMERIC_ENTRY2( 97,	adDecimal, cst_hate_mod,		10,2,FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(98,	adInteger, hate_basic,				sizeof(hate_basic),				FALSE)
	ADO_NUMERIC_ENTRY2( 99,	adDecimal, cst_hate_per_skl,	10,2,FALSE)
	ADO_NUMERIC_ENTRY2( 100,	adDecimal, cst_hate_per_enhance,	10,2,FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4( 101,	adInteger, critical_bonus, sizeof(critical_bonus), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 102,	adInteger, critical_bonus_per_skl, sizeof(critical_bonus_per_skl), FALSE)

	ADO_NUMERIC_ENTRY2(103,	adDecimal, cst_var[0],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(104,	adDecimal, cst_var[1],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(105,	adDecimal, cst_var[2],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(106,	adDecimal, cst_var[3],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(107,	adDecimal, cst_var[4],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(108,	adDecimal, cst_var[5],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(109,	adDecimal, cst_var[6],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(110,	adDecimal, cst_var[7],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(111,	adDecimal, cst_var[8],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(112,	adDecimal, cst_var[9],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(113,	adDecimal, cst_var[10],	10,4,FALSE)
	ADO_NUMERIC_ENTRY2(114,	adDecimal, cst_var[11],	10,4,FALSE)	
	ADO_NUMERIC_ENTRY2(115,	adDecimal, cst_var[12],	10,4,FALSE)	
	ADO_NUMERIC_ENTRY2(116,	adDecimal, cst_var[13],	10,4,FALSE)	
	ADO_NUMERIC_ENTRY2(117,	adDecimal, cst_var[14],	10,4,FALSE)	
	ADO_NUMERIC_ENTRY2(118,	adDecimal, cst_var[15],	10,4,FALSE)	
	ADO_NUMERIC_ENTRY2(119,	adDecimal, cst_var[16],	10,4,FALSE)	
	ADO_NUMERIC_ENTRY2(120,	adDecimal, cst_var[17],	10,4,FALSE)	
	ADO_NUMERIC_ENTRY2(121,	adDecimal, cst_var[18],	10,4,FALSE)	
	ADO_NUMERIC_ENTRY2(122,	adDecimal, cst_var[19],	10,4,FALSE)	

	ADO_VARIABLE_LENGTH_ENTRY4(125,	adTinyInt,	is_projectile,			sizeof(is_projectile),			FALSE)
	ADO_NUMERIC_ENTRY2( 126,	adDecimal, cst_projectile_speed,	10,2,FALSE)	
	ADO_NUMERIC_ENTRY2( 127,	adDecimal, cst_projectile_acceleration,	10,2,FALSE)	
END_ADO_BINDING()
};

struct dbSkillTree : public CADORecordBinding, public SkillTree
{
	static void onSkillTreeInfo( dbSkillTree * emprs );

BEGIN_ADO_BINDING(dbSkillTree)

	ADO_VARIABLE_LENGTH_ENTRY4(  1, adInteger, skill_tree_id	, sizeof(skill_tree_id	 ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  2, adInteger, skill_id			, sizeof(skill_id		 ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  3, adInteger, skill_group_id	, sizeof(skill_group_id	 ), FALSE )

	ADO_VARIABLE_LENGTH_ENTRY4(  4, adInteger, min_skill_lv		, sizeof(min_skill_lv	 ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  5, adInteger, max_skill_lv		, sizeof(max_skill_lv	 ), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  6, adInteger, lv				, sizeof(lv					), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  7, adInteger, job_lv			, sizeof(job_lv			 ), FALSE )
	ADO_NUMERIC_ENTRY2(			 8, adDecimal, cst_var			, 10,2,						FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4(  9, adInteger, need_skill_id[0]	, sizeof(need_skill_id[0]), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, need_skill_lv[0]	, sizeof(need_skill_lv[0]), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 11, adInteger, need_skill_id[1]	, sizeof(need_skill_id[1]), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 12, adInteger, need_skill_lv[1]	, sizeof(need_skill_lv[1]), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 13, adInteger, need_skill_id[2]	, sizeof(need_skill_id[2]), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 14, adInteger, need_skill_lv[2]	, sizeof(need_skill_lv[2]), FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 15, adInteger, min_enhance		, sizeof(min_enhance),		FALSE )
	ADO_VARIABLE_LENGTH_ENTRY4( 16, adInteger, max_enhance		, sizeof(max_enhance),		FALSE )

END_ADO_BINDING()

	_decimal_variant			  cst_var;
};

struct dbSkillJP : public CADORecordBinding, public SkillBase
{
	static void onSkillJPInfo( dbSkillJP * emprs );

BEGIN_ADO_BINDING(dbSkillJP)

	ADO_VARIABLE_LENGTH_ENTRY4( 1, 	adInteger, uid,					sizeof(uid),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 2, 	adInteger, need_jp[0],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 3, 	adInteger, need_jp[1],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 4, 	adInteger, need_jp[2],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 5, 	adInteger, need_jp[3],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 6, 	adInteger, need_jp[4],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 7, 	adInteger, need_jp[5],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 8, 	adInteger, need_jp[6],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 9, 	adInteger, need_jp[7],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, 	adInteger, need_jp[8],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, 	adInteger, need_jp[9],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, 	adInteger, need_jp[10],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(13, 	adInteger, need_jp[11],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, 	adInteger, need_jp[12],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, 	adInteger, need_jp[13],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(16, 	adInteger, need_jp[14],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(17, 	adInteger, need_jp[15],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(18, 	adInteger, need_jp[16],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(19, 	adInteger, need_jp[17],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(20, 	adInteger, need_jp[18],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(21, 	adInteger, need_jp[19],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(22, 	adInteger, need_jp[20],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(23, 	adInteger, need_jp[21],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(24, 	adInteger, need_jp[22],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(25, 	adInteger, need_jp[23],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(26, 	adInteger, need_jp[24],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(27, 	adInteger, need_jp[25],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(28, 	adInteger, need_jp[26],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(29, 	adInteger, need_jp[27],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(30, 	adInteger, need_jp[28],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(31, 	adInteger, need_jp[29],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(32, 	adInteger, need_jp[30],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(33, 	adInteger, need_jp[31],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(34, 	adInteger, need_jp[32],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(35, 	adInteger, need_jp[33],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(36, 	adInteger, need_jp[34],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(37, 	adInteger, need_jp[35],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(38, 	adInteger, need_jp[36],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(39, 	adInteger, need_jp[37],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(40, 	adInteger, need_jp[38],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(41, 	adInteger, need_jp[39],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(42, 	adInteger, need_jp[40],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(43, 	adInteger, need_jp[41],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(44, 	adInteger, need_jp[42],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(45, 	adInteger, need_jp[43],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(46, 	adInteger, need_jp[44],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(47, 	adInteger, need_jp[45],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(48, 	adInteger, need_jp[46],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(49, 	adInteger, need_jp[47],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(50, 	adInteger, need_jp[48],			sizeof(int),				FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(51, 	adInteger, need_jp[49],			sizeof(int),				FALSE)

END_ADO_BINDING()
};

void dbSkill::onSkillInfo( dbSkill * emprs )
{
	memset( emprs->need_jp, 0, sizeof(emprs->need_jp) );

	// 초단위 데이터를 AR_TIME 으로 변경
	emprs->delay_cast			= emprs->cst_delay_cast.getFloat();
	emprs->delay_cast_per_skl	= emprs->cst_delay_cast_per_skl.getFloat();
	emprs->delay_cast_mod_per_enhance	= emprs->cst_delay_cast_mod_per_enhance.getFloat();
	emprs->delay_common				= emprs->cst_delay_common.getFloat();
	emprs->delay_cooltime			= emprs->cst_delay_cooltime.getFloat();
	emprs->delay_cooltime_per_skl	= emprs->cst_delay_cooltime_per_skl.getFloat();
	emprs->delay_cooltime_mod_per_enhance = emprs->cst_delay_cooltime_mod_per_enhance.getFloat();

	emprs->state_level_per_skl = emprs->cst_state_level_per_skl.getFloat();
	emprs->state_level_per_enhance = emprs->cst_state_level_per_enhance.getFloat();
	emprs->state_second			= static_cast<AR_TIME>( emprs->cst_state_time.getFloat() * 100 );
	emprs->state_second_per_skl = static_cast<AR_TIME>( emprs->cst_state_time_per_skl.getFloat() * 100 );
	emprs->state_second_per_enhance			= static_cast<AR_TIME>( emprs->cst_state_time_per_enhance.getFloat() * 100 );
	// -1이면 무한 지속, 그 외의 값이면 -값이 들어간 것임
	assert( emprs->cst_state_time.getFloat() > -1.0001f && emprs->cst_state_time_per_skl.getFloat() > -1.0001f && emprs->cst_state_time_per_enhance.getFloat() > -1.0001f );

	emprs->hate_mod = emprs->cst_hate_mod.getFloat();
	emprs->hate_slv = emprs->cst_hate_per_skl.getFloat();
	emprs->hate_enhance = emprs->cst_hate_per_enhance.getFloat();
	emprs->projectile_speed = emprs->cst_projectile_speed.getFloat();
	emprs->projectile_acceleration = emprs->cst_projectile_acceleration.getFloat();

	emprs->cost_hp_per			= emprs->cst_cost_hp_per.getFloat();
	emprs->cost_hp_per_skl_per	= emprs->cst_cost_hp_per_skl_per.getFloat();
	emprs->cost_mp_per			= emprs->cst_cost_mp_per.getFloat();
	emprs->cost_mp_per_skl_per	= emprs->cst_cost_mp_per_skl_per.getFloat();
	emprs->cost_energy.set( emprs->cst_cost_energy.getMultipleInteger( 10000 ) );
	emprs->cost_energy_per_skl.set( emprs->cst_cost_energy_per_skl.getMultipleInteger( 10000 ) );

	for( int i = 0; i < SkillBase::MAX_SKILL_VALUE; ++i )
	{
		emprs->var[ i ].set( emprs->cst_var[i].getMultipleInteger( 10000 ) );
	}

	// 타입 플래그 설정
	emprs->effect_flag = 0;
	if( emprs->GetSkillEffectType() == EF_ADD_STATE ) emprs->effect_flag |= ( SkillBase::TF_ADD_STATE );
	
/*	if( emprs->vf_one_hand_sword &&		emprs->vf_two_hand_sword &&		emprs->vf_double_sword &&		emprs->vf_dagger &&		emprs->vf_double_dagger &&
		emprs->vf_spear &&		emprs->vf_axe &&		emprs->vf_one_hand_mace &&		emprs->vf_two_hand_mace &&		emprs->vf_bow &&
		emprs->vf_crossbow &&		emprs->vf_one_hand_staff &&		emprs->vf_two_hand_staff  ) emprs->vf_all = true;
	else emprs->vf_all = false;
*/
	GameContent::RegisterSkillBase( *emprs );
}

void dbSkillJP::onSkillJPInfo( dbSkillJP * emprs )
{
	GameContent::RegisterSkillJobPointTable( emprs->uid, emprs->need_jp );
}

void dbSkillTree::onSkillTreeInfo( dbSkillTree * emprs )
{
	emprs->jp_ratio = emprs->cst_var.getFloat();
	GameContent::RegisterSkillTree( *emprs );
}

static bool LoadSkillData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbSkill >( "SkillResource", ConnPtr, dbSkill::onSkillInfo );

	_cprint( "Total %d Skill info loaded...\n", cnt );
	FILELOG( "Total %d Skill info loaded...", cnt );

	return true;
}

static bool LoadSkillJPData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbSkillJP >( "SkillJPResource", ConnPtr, dbSkillJP::onSkillJPInfo );

	_cprint( "Total %d Skill Job Point table loaded...\n", cnt );
	FILELOG( "Total %d Skill Job Point table loaded...", cnt );

	return true;
}

static bool LoadSkillTreeData()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	size_t cnt = LoadDbResource< dbSkillTree >( "SkillTreeResource", ConnPtr, dbSkillTree::onSkillTreeInfo );

	_cprint( "Total %d Skill Tree table loaded...\n", cnt );
	FILELOG( "Total %d Skill Tree table loaded...", cnt );

	return true;
}

bool SkillLoader::onProcess( int nThreadNum )
{
	HRESULT  hr = S_OK;
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadSkillData();
		LoadSkillJPData();
		LoadSkillTreeData();
		helper.end( "SkillLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "SkillLoader" );
		LogDBError( e, "SkillLoader", "SkillLoader::onProcess()" );

		std::string strError = "Skill RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}