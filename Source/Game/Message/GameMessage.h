#pragma once

#include <mmo/ArType.h>
#include <mmo/ArTime.h>

#include "ItemBase.h"
#include "ItemInstance.h"
#include "CreatureBase.h"
#include "QuestBase.h"
#include "QuestInstance.h"
#include "BattleArenaBase.h"
#include "MessageType.h"

#include <hack_shield/AntiCpXSvr.h>
#include <xtrap/Xtrap_S_Interface.h>

#include "GameRule.h"
#include "GameType.h"
#include "VersionDefs.h"

#ifdef USE_CLIENT97
#include "GameMessage97.h"
#elif defined USE_CLIENT96
#include "GameMessage96.h"
#elif defined USE_CLIENT952
#include "GameMessage952.h"
#elif defined USE_CLIENT94
#include "GameMessage94.h"
#else
#include "GameMessage91.h"
#endif







#define BEGIN_COMMAND \
	static SCheatCommand s_Command[] = \
	{

#define END_COMMAND \
		{ NULL, NULL, GameRule::PERMISSION_FOR_PLAYER, 0 } \
	};

#define NORMAL1( message, function ) \
{ message, (SCheatCommand::CMD_FUNCTION)(function), GameRule::PERMISSION_FOR_PLAYER, 1 },

#define NORMAL2( message, function ) \
	{ message, (SCheatCommand::CMD_FUNCTION)(function), GameRule::PERMISSION_FOR_PLAYER, 2 },

#define NORMALA( message, function ) \
	{ message, (SCheatCommand::CMD_FUNCTION)(function), GameRule::PERMISSION_FOR_PLAYER, 0 },

#define GM1( message, function, permission ) \
	{ message, (SCheatCommand::CMD_FUNCTION)(function), permission, 1 },

#define GM2( message, function, permission ) \
	{ message, (SCheatCommand::CMD_FUNCTION)(function), permission, 2 },

#define GMA( message, function, permission ) \
	{ message, (SCheatCommand::CMD_FUNCTION)(function), permission, 0 },


