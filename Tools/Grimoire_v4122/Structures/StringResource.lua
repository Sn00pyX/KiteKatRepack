tableName = "StringResource"
fileName = "db_string.rdb"

fields = 
{
	{ "name_len", STRING_LEN, show=0 },
	{ "value_len", STRING_LEN, show=0 },
	{ "name", STRING_BY_LEN, dependency="name_len" },
	{ "value", STRING_BY_LEN, dependency="value_len" },
	{ "code", INT32 },
	{ "group_id", INT32 },
	{ "unknown", BYTE, length=16, show=0 }
}