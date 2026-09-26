tableName = "MonsterMotionSet"
fileName = "db_monstermotionset(ascii).rdb"

fields =
{
	{ "character_id", INT32 },
	{ "def", STRING, length=256 },
	{ "battle", STRING, length=256 },
	{ "idle", STRING, length=256 },
	{ "walk", STRING, length=256 },
	{ "run", STRING, length=256 },
	{ "be_attack", STRING, length=256 },
	{ "be_attack2", STRING, length=256 },
	{ "dead", STRING, length=256 },
	{ "flying_dead", STRING, length=256 },
	{ "take", STRING, length=256 },
	{ "attack_1", STRING, length=256 },
	{ "attack_2", STRING, length=256 },
	{ "attack_3", STRING, length=256 },
	{ "draw_bow", STRING, length=256 },
	{ "target_bow", STRING, length=256 },
	{ "fire_bow", STRING, length=256 },
	{ "casting_1", STRING, length=256 },
	{ "casting_2", STRING, length=256 },
	{ "casting_3", STRING, length=256 },
	{ "melee_1", STRING, length=256 },
	{ "melee_2", STRING, length=256 },
	{ "melee_3", STRING, length=256 },
	{ "mount", STRING, length=256 },
	{ "unmount", STRING, length=256 }
}