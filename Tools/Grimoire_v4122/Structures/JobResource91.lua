fileName = "db_job(ascii).rdb"
tableName = "JobResource"

fields = {
	{"id", INT32},
	{"text_id", INT32},
	{"stati_id", INT32},
	{"skill_tree_id", INT32},
	{"job_class", BYTE, length=1},
	{"job_depth", BYTE, length=1},
	{"up_lv", INT16},
	{"up_jlv", INT16},
	{"avable_job_0", INT16},
	{"avable_job_1", INT16},
	{"avable_job_2", INT16},
	{"avable_job_3", INT16},
	{"unknown_value", INT16, show=0, default=163},
	{"icon_id", INT32},
	{"icon_file_name", STRING, length=256},
}