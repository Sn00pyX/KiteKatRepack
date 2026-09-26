tableName = "MonsterResource"
fileName = "db_monster(ascii).rdb"

fields = 
{
	{ "id", INT32 }, -- old_uid?
	{ "monster_group", INT32 },
	{ "monster_type", INT32 },
	{ "name_id", INT32 },
	{ "location_id", INT32 }, -- respawn_name_id?
	{ "race", INT32 },
	{ "grp", INT32 },
	{ "transform_level", INT32 },
	{ "level", INT32 },
	{ "size", SINGLE },
	{ "scale", SINGLE },
	{ "target_fx_size", SINGLE },
	{ "walk_type", INT32 },
	{ "slant_type", INT32 },
	{ "visible_range", INT32 },
	{ "chase_range", INT32 },
	{ "magic_type", INT32 },
	{ "attack_type_vector", BIT_VECTOR, show=0 },
	{ "f_fisrt_attack", BIT_FROM_VECTOR, dependency="attack_type_vector", bit_position=32},
	{ "f_group_first_attack", BIT_FROM_VECTOR, dependency="attack_type_vector", bit_position=4},
	{ "f_response_casting", BIT_FROM_VECTOR, dependency="attack_type_vector", bit_position=3},
	{ "f_response_race", BIT_FROM_VECTOR, dependency="attack_type_vector", bit_position=2},
	{ "f_response_battle", BIT_FROM_VECTOR, dependency="attack_type_vector", bit_position=1},
	{ "hp", INT32 },
	{ "mp", INT32 },
	{ "attack_point", INT32 },
	{ "magic_point", INT32 },
	{ "defence", INT32 },
	{ "magic_defence", INT32 },
	{ "attack_speed", INT32 },
	{ "attack_speed_type", INT32, default=15, show=0 },
	{ "magic_speed", INT32 },
	{ "accuracy", INT32 },
	{ "avoid", INT32 },
	{ "magic_accuracy", INT32 },
	{ "magic_avoid", INT32 },
	{ "taming_id", INT32 },
	{ "taming_percentage", SINGLE },
	{ "taming_exp_mod", SINGLE },
	{ "exp", INT32 },
	{ "jp", INT32 },
	{ "monster_skill_link_id", INT32 },
	{ "stat_id", INT32 },
	{ "ability", INT32 },
	{ "standard_walk_spped", INT32 },
	{ "standard_run_spped", INT32 },
	{ "walk_speed", INT32 },
	{ "run_speed", INT32 },
	{ "attack_range", DECIMAL },
	{ "hidesense_range", DECIMAL },
	{ "gold_drop_percentage", INT32 },
	{ "gold_min", INT32 },
	{ "gold_max", INT32 },
	{ "chaos_drop_percentage", INT32 },
	{ "chaos_min", INT32 },
	{ "chaos_max", INT32 },
	{ "exp_2", INT32 },
	{ "jp_2", INT32 },
	{ "gold_min_2", INT32 },
	{ "gold_max_2", INT32 },
	{ "chaos_min_2", INT32 },
	{ "chaos_max_2", INT32 },
	{ "drop_table_link_id", INT32 },
	{ "texture_group", INT32 },
	{ "local_flag", INT32 },
	{ "model", STRING, length=256 },
	{ "motion_file_id", INT32 },
	{ "weapon_type", INT32 },
	{ "camera_x", INT32 },
	{ "camera_y", INT32 },
	{ "camera_z", INT32 },
	{ "target_x", SINGLE },
	{ "target_y", SINGLE },
	{ "target_z", SINGLE },
	{ "material", INT32 },
	{ "attack_motion_speed", INT32 },	
	{ "fight_type", INT32 },	
}

local encodeMap = {}
local decodeMap = {}

function compute_bit_swapping()
	local j, oldValue

	for i = 0,31 do
		encodeMap[i] = i;
	end

	j = 3
	for i = 0,31 do
		oldValue = encodeMap[i]
		encodeMap[i] = encodeMap[j]
		encodeMap[j] = oldValue
		j = (j + i + 3) % 32
	end

	for i = 0,31 do
		decodeMap[encodeMap[i]] = i
	end
end

compute_bit_swapping()

ProcessRow = function (mode, row, rowNum)
	
	local value = row["id"]
	local result = 0
	
	if mode == READ then		
		for i = 0,31 do
			if bit32.band(bit32.lshift(1, i), value) ~= 0 then
				result = bit32.bor(result, (bit32.lshift(1, decodeMap[i])))
			end
		end
	elseif mode == WRITE then
		for i = 0,31 do
			if bit32.band(bit32.lshift(1, i), value) ~= 0 then
				result = bit32.bor(result, (bit32.lshift(1, encodeMap[i])))
			end
		end
	end
	
	row["id"] = result
end
