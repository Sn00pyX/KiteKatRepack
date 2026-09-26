#pragma once
#include "GameContent.h"
#include "StructMonster.h"

StructMonster * respawnMonster( unsigned x, unsigned y, unsigned char layer, unsigned id, bool is_wandering, int way_point_id, StructMonster::MonsterDeleteHandler * pDeleteHandler, bool bNeedLock = true, unsigned char difficulty = 0, float face = 0.0f );
void AddRespawnObject( const GameContent::MONSTER_RESPAWN_INFO & info );
void AddRandomRespawnObject( const GameContent::RANDOM_MONSTER_RESPAWN_INFO & info );
