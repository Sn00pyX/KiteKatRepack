#pragma once

#include <time.h>


struct NPCBase
{
	int id;	
	int text_id;
	int name_text_id;
	int x;
	int y;
	int z;
	int face;
	int	local_flag;

	bool is_periodic;
	time_t begin_of_period;
	time_t end_of_period;

	int roaming_id;
	int standard_walk_speed;
	int standard_run_speed;
	int walk_speed;
	int run_speed;

	int attackable;

	enum
	{
		SPAWN_NORMAL = 0, 
		SPAWN_SIEGE_DUNGEON,
		SPAWN_NORMAL_DUNGEON,
		SPAWN_NONE,
		SPAWN_CASH,
	};

	int offensive_type;
	int spawn_type;
	int chase_range;
	int regen_time;
	int level;

	int stat_id;

	int attack_range;
	int attack_speed_type;			// 2006/08/02 Ãß°¡
	int hp;
	int mp;
	int attack_point;
	int magic_point;
	int defence;
	int magic_defence;
	int attack_speed;
	int magic_speed;
	int accuracy;
	int avoid;
	int magic_accuracy;
	int magic_avoid;

	char ai_script[256];
	char contact_script[256];
};

struct NPCEventPeriodBase
{
	int id;
	time_t begin_of_period;
	time_t end_of_period;
};