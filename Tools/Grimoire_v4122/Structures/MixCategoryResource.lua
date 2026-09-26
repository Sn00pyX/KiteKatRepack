tableName = "MixCategoryResource"
fileName = "db_mixcategory.rdb"

fields = {
	{ "id", INT16 },
	{ "synthetic_id", BYTE },
	{ "local_flag", INT32 },
	{ "high_category_id", INT16 },
	{ "middle_category_id", INT16 },
	{ "low_category_id", INT16 },
	{ "category_text_id", INT32 },
	{ "formal_id", INT32 },
	{ "result_id", INT32 }
}