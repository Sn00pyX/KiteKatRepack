tableName = "PetResource"
fileName = "db_pet.rdb"

fields =
{
	{ "id", INT32 },
	{ "type", INT32 },
	{ "name_id", INT32 },
	{ "cage_id", INT32 },
	{ "rate", BYTE },
	{ "size", FLOAT },
	{ "scale", FLOAT },
	{ "target_fx_size", FLOAT },
	{ "walk_type", INT32 },
	{ "slant_type", INT32 },
	{ "camera_x", INT32 },
	{ "camera_y", INT32 },
	{ "camera_z", INT32 },
	{ "target_x", FLOAT },
	{ "target_y", FLOAT },
	{ "target_z", FLOAT },
	{ "model", STRING, length=256 },
	{ "motion_file_id", INT32 },
	{ "texture_group", INT32 },
	{ "local_flag", INT32 }
}