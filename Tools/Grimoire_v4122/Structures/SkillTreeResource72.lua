tableName = "SkillTreeResource"
fileName = "db_skilltree(ascii).rdb"

specialCase = DOUBLELOOP

fields = 
{
	{ "job_id", INT32, flag=LOOPCOUNTER },
	{ "skill_id", INT32 },
	{ "min_skill_lv", INT32 },
	{ "max_skill_lv", INT32 },
	{ "lv", INT32 },
	{ "job_lv", INT32 },
	{ "jp_ratio", SINGLE },
	{ "need_skill_id_1", INT32 },
	{ "need_skill_id_2", INT32 },
	{ "need_skill_id_3", INT32 },
	{ "need_skill_lv_1", INT32 },
	{ "need_skill_lv_2", INT32 },
	{ "need_skill_lv_3", INT32 },
	{ "cenhance_min", INT32 },
	{ "cenhance_max", INT32 }
}