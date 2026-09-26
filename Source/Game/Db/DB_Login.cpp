
#include <atlcomtime.h>
#include <oledb.h>
#include <icrsint.h>


#include <network/IConnection.h>
#include <network/XIOCPConnection.h>
#include <toolkit/XRandom.h>
#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>
#include <logging/FileLog.h>
#include <toolkit/XSTLUtil.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"
#include "GameDBUtil.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "StructPlayer.h"
#include "StructItem.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructQuest.h"
#include "PartyManager.h"
#include "GuildManager.h"
#include "GameProc.h"
#include "StructSkill.h"
#include "GameContent.h"
#include "StructWorldLocation.h"
#include "ChannelManager.h"
#include "DungeonManager.h"
#include "HuntaholicManager.h"
#include "InstanceDungeonManager.h"
#include "BattleArenaManager.h"
#include "RandomManager.h"
#include "Constant.h"
#include "Extern.h"
#include "LuaVM.h"
#include "ThreadPlayerHelper.h"

extern XCriticalSection													g_ConnectionTagLock;

char* OUT_SUMMON[]
=
{
	"OUT_SUMMON_0",
	"OUT_SUMMON_1",
	"OUT_SUMMON_2",
	"OUT_SUMMON_3",
	"OUT_SUMMON_4",
	"OUT_SUMMON_5",
};

char* OUT_BELT[]
=
{
	"OUT_BELT_0",
	"OUT_BELT_1",
	"OUT_BELT_2",
	"OUT_BELT_3",
	"OUT_BELT_4",
	"OUT_BELT_5",
	"OUT_BELT_6",
	"OUT_BELT_7",
};

char* OUT_SUB_TITLE[]
=
{
	"OUT_SUB_TITLE_0",
	"OUT_SUB_TITLE_1",
	"OUT_SUB_TITLE_2",
	"OUT_SUB_TITLE_3",
	"OUT_SUB_TITLE_4",
};

void readCreatureSkillList( DBConnection & db, StructCreature *pCreature )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readCreatureSkillList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;

	int sid = 0;
	if( pCreature->IsPlayer() ) 
	{
		sid = static_cast< StructPlayer* >( pCreature )->GetPlayerUID();
		cmd->CommandText = _bstr_t( "dbo.smp_read_player_skill_list" );
	}
	else 
	{
		sid = static_cast< StructSummon* >( pCreature )->GetSummonSID();
		cmd->CommandText = _bstr_t( "dbo.smp_read_summon_skill_list" );
	}

	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, sid ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{	
		pCreature->SetSkill(	pRS->Fields->Item["sid"]->Value,
							pRS->Fields->Item["skill_id"]->Value,
							pRS->Fields->Item["skill_level"]->Value,
							pRS->Fields->Item["cool_time"]->Value );
		pRS->MoveNext();
	}

	// 이런 저런 스킬들 등록
	if( pCreature->IsPlayer() )
	{
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESURRECTION_SCROLL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_RECOVERY_SCROLL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ANTIDOTE_SCROLL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RECHARGE_SCROLL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_TOWN_PORTAL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_PERFECT_CREATURE_RESURRECTION_SCROLL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_CREATURE_TAMING_SCROLL, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_PIECE_OF_STRENGTH, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_PIECE_OF_VITALITY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_PIECE_OF_DEXTERITY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_PIECE_OF_AGILITY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_PIECE_OF_INTELLIGENCE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_PIECE_OF_MENTALITY, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_RETURN_FEATHER, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_RETURN_BACK_FEATHER, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_FIRE_BOMB_PHYSICAL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_FIRE_BOMB_MAGICAL, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_WARP_TO_HUNTAHOLIC_LOBBY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_RANKED_DEATHMATCH_ENTER, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_FREED_DEATHMATCH_ENTER, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL,	StructSkill::SKILL_INSTANCE_GAME_EXIT, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_COLLECTING, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_MINING, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_OPERATING, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_ACTIVATING, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_OPERATE_DEVICE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_OPERATE_DUNGEON_CORE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_OPERATE_EXPLORATION, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_TREE_OF_HEALING_TYPE_A, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_TREE_OF_HEALING_TYPE_B, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_TREE_OF_HEALING_ON_DEATHMATCH, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL_LV1, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL_LV2, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL_LV3, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL_LV4, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL_LV5, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL_LV6, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL_LV7, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL_LV8, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL_LV1, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL_LV2, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL_LV3, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL_LV4, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL_LV5, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL_LV6, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL_LV7, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL_LV8, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_RECOVERY_SCROLL_LV1, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_RECOVERY_SCROLL_LV2, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_RECOVERY_SCROLL_LV3, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_RECOVERY_SCROLL_LV4, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_RECOVERY_SCROLL_LV5, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_RECOVERY_SCROLL_LV6, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_RECOVERY_SCROLL_LV7, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_RECOVERY_SCROLL_LV8, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_CALL_BLACK_PINE_TEA, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_SUMMON_SPEED_UP_SCROLL_LV1, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_SUMMON_SPEED_UP_SCROLL_LV2, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_SUMMON_SPEED_UP_SCROLL_LV3, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_STRENGTH, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_VITALITY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_DEXTERITY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_AGILITY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_INTELLIGENCE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_MENTALITY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_STRENGTH_QUEST, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_VITALITY_QUEST, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_DEXTERITY_QUEST, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_AGILITY_QUEST, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_INTELLIGENCE_QUEST, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_PIECE_OF_MENTALITY_QUEST, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_MAX_LIFE_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_MAX_MANA_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_AGILENESS_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_GUARDIAN_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_QUICKNESS_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_IMPREGNABLENESS_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_CAREFULNESS_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_FATALNESS_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_MANA_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_SHARPNESS_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_SPELL_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_BRILLIANCE_PIECE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_ALTERED_INSIGHT_PIECE, 20, 0 );
		
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_THROW_SMALL_SNOWBALL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_THROW_BIG_SNOWBALL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_THROW_MARBLE, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_THROW_RED_TOMATO, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_THROW_GREEN_TOMATO, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_NEW_YEAR_CHAMPAGNE, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_NAMUIR_LEAF_POISON, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_NAMUIR_RIND_BLEEDING, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESPAWN_SCROLL_RANDOMLY1, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE1, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE2, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE3, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE4, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE5, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE6, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE7, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_RESPAWN_SCROLL_WITH_DIFF_CODE8, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_PROP_SKILL, StructSkill::SKILL_PROP_RESPAWN1, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_REGENERATION_SCROLL_OF_VITALITY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_MANA_SCROLL_OF_VITALITY, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL, StructSkill::SKILL_ITEM_HEALING_SCROLL_OF_VITALITY, 20, 0 );

		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL,	StructSkill::SKILL_ITEM_SPELL_BREAKER_SCROLL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL,	StructSkill::SKILL_ITEM_STONECURSE_SCROLL, 20, 0 );
		pCreature->SetSkill( StructSkill::SKILL_UID_ITEM_SKILL,	StructSkill::SKILL_ITEM_REGION_STONECURSE_SCROLL, 20, 0 );
	}
}

bool DB_Login::readFriendsList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readFriendsList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;

	cmd->CommandText = _bstr_t( "dbo.smp_read_friends_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_friends_list";

	_bstr_t strPlayerName = m_pPlayer->GetName();

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, strPlayerName.length(), strPlayerName ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		m_pPlayer->m_vFriend.push_back( (const char * ) _bstr_t( pRS->Fields->Item["friend_id"]->Value.bstrVal ) );

		pRS->MoveNext();
	}

	return true;
}

bool DB_Login::readDenialsList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readDenialsList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_denials_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_denials_list";

	_bstr_t szCharacterName( m_pPlayer->GetName() );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		m_pPlayer->m_vDenial.push_back( (const char * ) _bstr_t( pRS->Fields->Item["denial_id"]->Value.bstrVal ) );

		pRS->MoveNext();
	}

	return true;
}


bool DB_Login::readFriendOfsList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readFriendOfsList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_friendofs_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_friendofs_list";

	_bstr_t szCharacterName( m_pPlayer->GetName() );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		m_pPlayer->m_vFriendOf.push_back( (const char * ) _bstr_t( pRS->Fields->Item["owner_id"]->Value.bstrVal ) );

		pRS->MoveNext();
	}

	return true;
}

bool DB_Login::readDenialOfsList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readDenialOfsList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_denialofs_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_denialofs_list";

	_bstr_t szCharacterName( m_pPlayer->GetName() );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adBSTR, adParamInput, szCharacterName.length(), szCharacterName ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		m_pPlayer->m_vDenialOf.push_back( (const char * ) _bstr_t( pRS->Fields->Item["owner_id"]->Value.bstrVal ) );

		pRS->MoveNext();
	}

	return true;
}

const unsigned short DB_Login::readCharacterInfo( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readCharacterInfo : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_login_character" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_login_character";

	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SID", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ACCOUNT", adVarChar, adParamOutput, GameRule::MAX_ACCOUNT_LEN, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_PERMISSION", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_PARTY_SID", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_GUILD_SID", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_PREV_GUILD_SID", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_GUILD_PERMISSION", adTinyInt, adParamOutput, 1, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_X", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_Y", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_Z", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_LAYER", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_RACE", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SEX", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_LV", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_MAX_REACHED_LEVEL", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_EXP", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_LAST_DECREASED_EXP", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_HP", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_MP", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_STAMINA", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_HAVOC", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JOB", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JOB_DEPTH", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JLV", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JP", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_TOTAL_JP", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_TALENT_POINT", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JOB_0", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JOB_1", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JOB_2", adInteger, adParamOutput, 4, 0 ) );	
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JLV_0", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JLV_1", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_JLV_2", adInteger, adParamOutput, 4, 0 ) );

	_ParameterPtr paramPtr = cmd->CreateParameter( "OUT_IP", adDecimal, adParamOutput, sizeof( DECIMAL ) );
	paramPtr->NumericScale = 4;
	paramPtr->Precision = 18;
	cmd->Parameters->Append( paramPtr );

	cmd->Parameters->Append( cmd->CreateParameter( "OUT_CHA", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_PKC", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_DKC", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_HUNTAHOLIC_POINT", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_HUNTAHOLIC_ENTER_COUNT", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ETHEREAL_STONE_DURABILITY", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUMMON_0", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUMMON_1", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUMMON_2", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUMMON_3", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUMMON_4", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUMMON_5", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SKIN_COLOR", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_MODEL_00", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_MODEL_01", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_MODEL_02", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_MODEL_03", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_MODEL_04", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_HAIR_COLOR_INDEX", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_HAIR_COLOR_RGB", adUnsignedInt, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_HIDE_EQUIP_FLAG", adUnsignedInt, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_TEXTURE_ID", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_BELT_0", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_BELT_1", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_BELT_2", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_BELT_3", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_BELT_4", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_BELT_5", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append(cmd->CreateParameter("OUT_BELT_6", adBigInt, adParamOutput, 8, 0L));
	cmd->Parameters->Append(cmd->CreateParameter("OUT_BELT_7", adBigInt, adParamOutput, 8, 0L));
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_GOLD", adBigInt, adParamOutput, 8, 0L ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_CHAOS", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_FLAG_LIST", adVarChar, adParamOutput, 1000, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_LOGIN_COUNT", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_MAIN_SUMMON", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUB_SUMMON", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_REMAIN_SUMMON_TIME", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_PET", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_MAIN_TITLE", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUB_TITLE_0", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUB_TITLE_1", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUB_TITLE_2", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUB_TITLE_3", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SUB_TITLE_4", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_REMAIN_TITLE_TIME", adInteger, adParamOutput, 4, 0 ) );

	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_POINT", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_BLOCK_TIME", adDate, adParamOutput, sizeof( DATE ), 0.0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_PENALTY_COUNT", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_PENALTY_DEC_TIME", adDate, adParamOutput, sizeof( DATE ), 0.0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_MVP_COUNT", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_RECORD_0_0", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_RECORD_0_1", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_RECORD_1_0", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_RECORD_1_1", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_RECORD_2_0", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ARENA_RECORD_2_1", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ALIAS", adVarWChar, adParamOutput, 31, 0 ) );

	cmd->Parameters->Append( cmd->CreateParameter( "OUT_REMAIN_CHAT_BLOCK_TIME", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "ADV_CHAT_COUNT", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_LOGOUT_DURATION", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_TOTAL_PLAY_TIME", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_NAME_CHANGED", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_AUTO_USED", adInteger, adParamOutput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_GUILD_BLOCK_TIME", adDate, adParamOutput, sizeof( DATE ), 0.0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_PKMODE", adTinyInt, adParamOutput, 1, 0 ) );
	cmd->Parameters->Append(cmd->CreateParameter("OUT_GUILD_POINT", adInteger, adParamOutput, 4, 0));
	cmd->Parameters->Append(cmd->CreateParameter("OUT_GUILD_TOTAL_POINT", adInteger, adParamOutput, 4, 0));

	cmd->Parameters->Append(cmd->CreateParameter("OUT_PLAY_TIME_POINT", adInteger, adParamOutput, 4, 0));
	cmd->Parameters->Append(cmd->CreateParameter("OUT_RX", adInteger, adParamOutput, 4, 0));
	cmd->Parameters->Append(cmd->CreateParameter("OUT_RY", adInteger, adParamOutput, 4, 0));
	cmd->Parameters->Append(cmd->CreateParameter("OUT_HX", adInteger, adParamOutput, 4, 0));
	cmd->Parameters->Append(cmd->CreateParameter("OUT_HY", adInteger, adParamOutput, 4, 0));

	_bstr_t strCharacterName( szCharacterName );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adBSTR, adParamInput, strCharacterName.length(), strCharacterName ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, sizeof(nAccountID), nAccountID ) );

	cmd->Execute( NULL, NULL, adCmdStoredProc );

	if(	cmd->Parameters->Item[ "OUT_SID" ]->Value.vt != VT_NULL )
	{
		m_pPlayer->SetName( szCharacterName );

		// 로드하자
		sid = m_pPlayer->m_nUID = cmd->Parameters->Item[ "OUT_SID" ]->Value.intVal;
		_bstr_t account = cmd->Parameters->Item[ "OUT_ACCOUNT" ]->Value.bstrVal;
		
		s_strcpy( m_pPlayer->m_szAccountName, _countof( m_pPlayer->m_szAccountName ), account );
		m_pPlayer->SetCurrentXY(	cmd->Parameters->Item[ "OUT_X" ]->Value.intVal,
								cmd->Parameters->Item[ "OUT_Y" ]->Value.intVal );
#ifndef _USE_UPDATE_CHARACTER_CHECKSUM
		m_pPlayer->SetCurrentZ( cmd->Parameters->Item[ "OUT_Z" ]->Value.intVal );
#endif
		m_pPlayer->SetCurrentLayer( cmd->Parameters->Item[ "OUT_LAYER" ]->Value.intVal );
		m_pPlayer->m_nRace = cmd->Parameters->Item[ "OUT_RACE" ]->Value.intVal;

		_bstr_t flag_list = cmd->Parameters->Item[ "OUT_FLAG_LIST" ]->Value.bstrVal;
		char *pBegin = flag_list;
		char *pEnd = NULL;
		while( pEnd = strchr( pBegin, '\n' ) )
		{	
			char *ps = strchr( pBegin, ':' );
			std::string strKey( pBegin, ps );
			std::string strData( ps+1, pEnd );
			m_pPlayer->SetFlag( strKey.c_str(), strData.c_str() );

			pBegin = pEnd + 1;
		}

		// 캐릭터 플래그 읽고 나서 계정 플래그도 읽어주자.
		_readAccountFlag( db );

		m_pPlayer->m_nPartyID = cmd->Parameters->Item[ "OUT_PARTY_SID" ]->Value.intVal;
		m_pPlayer->m_nGuildId = cmd->Parameters->Item[ "OUT_GUILD_SID" ]->Value.intVal;
		m_pPlayer->m_nPrevGuildId = cmd->Parameters->Item[ "OUT_PREV_GUILD_SID" ]->Value.intVal;
		m_pPlayer->m_nGuildPermission = cmd->Parameters->Item[ "OUT_GUILD_PERMISSION" ]->Value.cVal;

		m_pPlayer->m_nSex = cmd->Parameters->Item[ "OUT_SEX" ]->Value.intVal;
		m_pPlayer->m_nLevel = cmd->Parameters->Item[ "OUT_LV" ]->Value.intVal;
		m_pPlayer->m_nMaxReachedLevel = cmd->Parameters->Item[ "OUT_MAX_REACHED_LEVEL" ]->Value.intVal;
		if( m_pPlayer->m_nMaxReachedLevel < m_pPlayer->m_nLevel )
			m_pPlayer->m_nMaxReachedLevel = m_pPlayer->m_nLevel;
		m_pPlayer->m_nEXP = cmd->Parameters->Item[ "OUT_EXP" ]->Value.llVal;
		m_pPlayer->m_nLastDecreasedEXP = cmd->Parameters->Item[ "OUT_LAST_DECREASED_EXP" ]->Value.llVal;
		m_pPlayer->m_nHP = cmd->Parameters->Item[ "OUT_HP" ]->Value.intVal;
		m_pPlayer->m_nMP = cmd->Parameters->Item[ "OUT_MP" ]->Value.intVal;
		m_pPlayer->m_nStamina = cmd->Parameters->Item[ "OUT_STAMINA" ]->Value.intVal;
#ifndef _USE_UPDATE_CHARACTER_CHECKSUM
		m_pPlayer->m_nCharacterChecksum = cmd->Parameters->Item[ "OUT_HAVOC" ]->Value.intVal;
#endif
		m_pPlayer->m_nJobLevel = cmd->Parameters->Item[ "OUT_JLV" ]->Value.intVal;
		m_pPlayer->m_nPrevJobId[0] = cmd->Parameters->Item[ "OUT_JOB_0" ]->Value.intVal;
		m_pPlayer->m_nPrevJobId[1] = cmd->Parameters->Item[ "OUT_JOB_1" ]->Value.intVal;
		m_pPlayer->m_nPrevJobId[2] = cmd->Parameters->Item[ "OUT_JOB_2" ]->Value.intVal;
		m_pPlayer->m_nPrevJobLevel[0] = cmd->Parameters->Item[ "OUT_JLV_0" ]->Value.intVal;
		m_pPlayer->m_nPrevJobLevel[1] = cmd->Parameters->Item[ "OUT_JLV_1" ]->Value.intVal;
		m_pPlayer->m_nPrevJobLevel[2] = cmd->Parameters->Item[ "OUT_JLV_2" ]->Value.intVal;
		m_pPlayer->m_nJobPoint = cmd->Parameters->Item[ "OUT_JP" ]->Value.llVal;
		m_pPlayer->m_nTotalJobPoint = cmd->Parameters->Item[ "OUT_TOTAL_JP" ]->Value.llVal;
		m_pPlayer->m_nTalentPoint = cmd->Parameters->Item[ "OUT_TALENT_POINT" ]->Value.intVal;

		_decimal_variant decImmoral;
		*static_cast< DECIMAL * >( &decImmoral ) = cmd->Parameters->Item[ "OUT_IP" ]->Value.decVal;
		m_pPlayer->m_fImmoralPoint.set( decImmoral.getMultipleInteger( 10000 ) );
		m_pPlayer->m_TitleManager.UpdateTitleConditionByImmoralPoint( m_pPlayer->m_fImmoralPoint );

		m_pPlayer->m_nCharisma = cmd->Parameters->Item[ "OUT_CHA" ]->Value.intVal;
		m_pPlayer->m_nPKC = cmd->Parameters->Item[ "OUT_PKC" ]->Value.intVal;
		m_pPlayer->m_nDKC = cmd->Parameters->Item[ "OUT_DKC" ]->Value.intVal;
		m_pPlayer->m_nHuntaholicPoint = cmd->Parameters->Item[ "OUT_HUNTAHOLIC_POINT" ]->Value.intVal;
		m_pPlayer->m_nHuntaholicEnterableCount = cmd->Parameters->Item[ "OUT_HUNTAHOLIC_ENTER_COUNT" ]->Value.intVal;
		m_pPlayer->m_nEtherealStoneDurability = cmd->Parameters->Item[ "OUT_ETHEREAL_STONE_DURABILITY" ]->Value.intVal;
		m_pPlayer->m_tNextHuntaholicEnterableCountRefill = 0;
		m_pPlayer->m_nJob = cmd->Parameters->Item[ "OUT_JOB" ]->Value.intVal;
		m_pPlayer->m_nPermission = cmd->Parameters->Item[ "OUT_PERMISSION" ]->Value.intVal;
		m_pPlayer->m_nGold.SetRawData( cmd->Parameters->Item[ "OUT_GOLD" ]->Value.llVal );
		m_pPlayer->m_nChaos = cmd->Parameters->Item[ "OUT_CHAOS" ]->Value.intVal;
		m_pPlayer->m_nSkinColor = cmd->Parameters->Item[ "OUT_SKIN_COLOR" ]->Value.intVal;
		m_pPlayer->m_nLoginCount = cmd->Parameters->Item[ "OUT_LOGIN_COUNT" ]->Value.intVal;
		m_pPlayer->m_nJobDepth = m_pPlayer->GetJobDepth();
		m_pPlayer->m_nBaseModelId[0] = cmd->Parameters->Item[ "OUT_MODEL_00" ]->Value.intVal;
		m_pPlayer->m_nBaseModelId[1] = cmd->Parameters->Item[ "OUT_MODEL_01" ]->Value.intVal;
		m_pPlayer->m_nBaseModelId[2] = cmd->Parameters->Item[ "OUT_MODEL_02" ]->Value.intVal;
		m_pPlayer->m_nBaseModelId[3] = cmd->Parameters->Item[ "OUT_MODEL_03" ]->Value.intVal;
		m_pPlayer->m_nBaseModelId[4] = cmd->Parameters->Item[ "OUT_MODEL_04" ]->Value.intVal;
		m_pPlayer->m_nFaceTextureId = cmd->Parameters->Item[ "OUT_TEXTURE_ID" ]->Value.intVal;
		m_pPlayer->m_nHairColorIndex = cmd->Parameters->Item[ "OUT_HAIR_COLOR_INDEX" ]->Value.intVal;
		m_pPlayer->m_nHairColorRGB = cmd->Parameters->Item[ "OUT_HAIR_COLOR_RGB" ]->Value.uintVal;
		m_pPlayer->m_nHideEquipFlag = cmd->Parameters->Item[ "OUT_HIDE_EQUIP_FLAG" ]->Value.uintVal;
		m_pPlayer->m_nGuildPoint = cmd->Parameters->Item["OUT_GUILD_POINT"]->Value.intVal;
		m_pPlayer->m_nGuildTotalPoint = cmd->Parameters->Item["OUT_GUILD_TOTAL_POINT"]->Value.intVal;

		if( !_readClientInfo( db ) )
		{
			assert( 0 && "Fail to read client info" );
			m_pPlayer->m_strClientInfo = "";
			m_pPlayer->m_strQuickSlot = "";
			m_pPlayer->m_strCurrentKey = "";
			m_pPlayer->m_strSavedKey = "";
		}

		m_pPlayer->m_nArenaPoint = cmd->Parameters->Item[ "OUT_ARENA_POINT" ]->Value.intVal;
		COleDateTime dtArenaBlockTime( cmd->Parameters->Item[ "OUT_ARENA_BLOCK_TIME" ]->Value.date );
		struct tm tmArenaBlockTime;
		tmArenaBlockTime.tm_year = dtArenaBlockTime.GetYear() - 1900;
		tmArenaBlockTime.tm_mon = dtArenaBlockTime.GetMonth() - 1;
		tmArenaBlockTime.tm_mday = dtArenaBlockTime.GetDay();
		tmArenaBlockTime.tm_hour = dtArenaBlockTime.GetHour();
		tmArenaBlockTime.tm_min = dtArenaBlockTime.GetMinute();
		tmArenaBlockTime.tm_sec = dtArenaBlockTime.GetSecond();
		tmArenaBlockTime.tm_isdst = -1;
		m_pPlayer->m_tArenaBlock = mktime( &tmArenaBlockTime );
		if( m_pPlayer->m_tArenaBlock == -1 )
			m_pPlayer->m_tArenaBlock = time( NULL );
		m_pPlayer->m_nArenaPenaltyCount = cmd->Parameters->Item[ "OUT_ARENA_PENALTY_COUNT" ]->Value.intVal;
		COleDateTime dtArenaPenaltyDecTime( cmd->Parameters->Item[ "OUT_ARENA_PENALTY_DEC_TIME" ]->Value.date );
		struct tm tmArenaPenaltyDecTime;
		tmArenaPenaltyDecTime.tm_year = dtArenaPenaltyDecTime.GetYear() - 1900;
		tmArenaPenaltyDecTime.tm_mon = dtArenaPenaltyDecTime.GetMonth() - 1;
		tmArenaPenaltyDecTime.tm_mday = dtArenaPenaltyDecTime.GetDay();
		tmArenaPenaltyDecTime.tm_hour = dtArenaPenaltyDecTime.GetHour();
		tmArenaPenaltyDecTime.tm_min = dtArenaPenaltyDecTime.GetMinute();
		tmArenaPenaltyDecTime.tm_sec = dtArenaPenaltyDecTime.GetSecond();
		tmArenaPenaltyDecTime.tm_isdst = -1;
		m_pPlayer->m_tArenaPenaltyDecrease = mktime( &tmArenaPenaltyDecTime );
		if( m_pPlayer->m_tArenaPenaltyDecrease == -1 )
			m_pPlayer->m_tArenaPenaltyDecrease = time( NULL );
		m_pPlayer->m_nArenaMVPCount = cmd->Parameters->Item[ "OUT_ARENA_MVP_COUNT" ]->Value.intVal;
		m_pPlayer->m_anArenaRecord[ 0 ][ 0 ] = cmd->Parameters->Item[ "OUT_ARENA_RECORD_0_0" ]->Value.intVal;
		m_pPlayer->m_anArenaRecord[ 0 ][ 1 ] = cmd->Parameters->Item[ "OUT_ARENA_RECORD_0_1" ]->Value.intVal;
		m_pPlayer->m_anArenaRecord[ 1 ][ 0 ] = cmd->Parameters->Item[ "OUT_ARENA_RECORD_1_0" ]->Value.intVal;
		m_pPlayer->m_anArenaRecord[ 1 ][ 1 ] = cmd->Parameters->Item[ "OUT_ARENA_RECORD_1_1" ]->Value.intVal;
		m_pPlayer->m_anArenaRecord[ 2 ][ 0 ] = cmd->Parameters->Item[ "OUT_ARENA_RECORD_2_0" ]->Value.intVal;
		m_pPlayer->m_anArenaRecord[ 2 ][ 1 ] = cmd->Parameters->Item[ "OUT_ARENA_RECORD_2_1" ]->Value.intVal;

		s_strcpy( m_pPlayer->m_szAlias, _countof( m_pPlayer->m_szAlias ), static_cast< const char* >( _bstr_t( cmd->Parameters->Item[ "OUT_ALIAS" ]->Value.bstrVal ) ) );
		XStringUtil::Trim( m_pPlayer->m_szAlias );
		if( strlen( m_pPlayer->m_szAlias ) > 0 )
			s_strcat( m_pPlayer->m_szAlias, _countof( m_pPlayer->m_szAlias ), "*" );

		m_pPlayer->m_nChatBlockTime = cmd->Parameters->Item[ "OUT_REMAIN_CHAT_BLOCK_TIME" ]->Value.intVal + GetArTime() / 100;
		m_pPlayer->m_nAdvChatCount = cmd->Parameters->Item[ "ADV_CHAT_COUNT" ]->Value.intVal;
		m_pPlayer->m_nNameChanged = cmd->Parameters->Item[ "OUT_NAME_CHANGED" ]->Value.intVal;
		m_pPlayer->m_bAutoUsed = cmd->Parameters->Item[ "OUT_AUTO_USED" ]->Value.intVal;

		COleDateTime dtGuildBlockTime( cmd->Parameters->Item[ "OUT_GUILD_BLOCK_TIME" ]->Value.date );
		struct tm tmGuildBlockTime;
		tmGuildBlockTime.tm_year = dtGuildBlockTime.GetYear() - 1900;
		tmGuildBlockTime.tm_mon = dtGuildBlockTime.GetMonth() - 1;
		tmGuildBlockTime.tm_mday = dtGuildBlockTime.GetDay();
		tmGuildBlockTime.tm_hour = dtGuildBlockTime.GetHour();
		tmGuildBlockTime.tm_min = dtGuildBlockTime.GetMinute();
		tmGuildBlockTime.tm_sec = dtGuildBlockTime.GetSecond();
		tmGuildBlockTime.tm_isdst = -1;
		m_pPlayer->m_tGuildBlockTime = mktime( &tmGuildBlockTime );
		if( m_pPlayer->m_tGuildBlockTime == -1 )
			m_pPlayer->m_tGuildBlockTime = time( NULL );

		m_pPlayer->m_bIsPK = cmd->Parameters->Item[ "OUT_PKMODE" ]->Value;
		m_pPlayer->m_nPlayTimePoint = cmd->Parameters->Item["OUT_PLAY_TIME_POINT"]->Value.intVal;
		m_pPlayer->m_nRX = cmd->Parameters->Item["OUT_RX"]->Value.intVal;
		m_pPlayer->m_nRY = cmd->Parameters->Item["OUT_RY"]->Value.intVal;
		m_pPlayer->m_nHX = cmd->Parameters->Item["OUT_HX"]->Value.intVal;
		m_pPlayer->m_nHY = cmd->Parameters->Item["OUT_HY"]->Value.intVal;

		m_pPlayer->UpdateTitleConditionByPKOn( m_pPlayer->m_bIsPK );

		// 비정상 좌표에 대한 체크
		if( m_pPlayer->GetX() < 0 || m_pPlayer->GetX() >= g_nMapWidth ||
			m_pPlayer->GetY() < 0 || m_pPlayer->GetY() >= g_nMapHeight )
		{
			_cprint( "Login attempted with invalid position info[Account:%s/Character:%s/X:%f/Y:%f]\n", m_pPlayer->m_szAccountName, m_pPlayer->GetName(), m_pPlayer->GetX(), m_pPlayer->GetY() );
			FILELOG( "Login attempted with invalid position info[Account:%s/Character:%s/X:%f/Y:%f]", m_pPlayer->m_szAccountName, m_pPlayer->GetName(), m_pPlayer->GetX(), m_pPlayer->GetY() );

			return RESULT_NOT_ACTABLE_HERE;
		}

		logout_duration = cmd->Parameters->Item[ "OUT_LOGOUT_DURATION" ]->Value.intVal; // OUT_LOGOUT_DURATION 분 단위
		// 스태미너 회복률이 완전히 결정된 후에 스태미너 회복량 계산에 사용될 기간 설정
		m_pPlayer->SetLogoutDuration( logout_duration );
		
		m_pPlayer->m_nTotalPlayTime = cmd->Parameters->Item[ "OUT_TOTAL_PLAY_TIME" ]->Value.intVal; // 총 플레이 시간, 초 단위

		// 헌터홀릭 도전 제한 횟수 리필 처리
		do
		{
			time_t tCurrent = time( NULL );

			// 재접속을 1분 이내에 한 경우 1분 이전 시간과 비교
			time_t tLogoutDuration = ( logout_duration ) ? logout_duration * 60 : 60;

			// 로그아웃 했던 시점 이후로 적용되어야 할 가장 가까운 충전 시점 구하기
			time_t tRefill;
			{
				time_t tLogout = tCurrent - tLogoutDuration;

				struct tm tmLogoutDayRefill;
				errno_t nError = localtime_s( &tmLogoutDayRefill, &tLogout );
				if( nError )
					break;

				tmLogoutDayRefill.tm_hour = 6;
				tmLogoutDayRefill.tm_min = 0;
				tmLogoutDayRefill.tm_sec = 0;
				tmLogoutDayRefill.tm_isdst = -1;

				tRefill = mktime( &tmLogoutDayRefill );

				// 로그아웃 당일의 충전 시점 이후에 로그아웃 했었다면 적용되어야 할 충전 시점은 로그아웃 당일 충전 시점 + 1일
				if( tLogout > tRefill )
				{
					// DST 문제를 피하기 위해 tm 값을 세팅하고 local-time에서 다시 time_t 값을 계산함
					++tmLogoutDayRefill.tm_mday;
					tmLogoutDayRefill.tm_hour = 6;
					tmLogoutDayRefill.tm_min = 0;
					tmLogoutDayRefill.tm_sec = 0;
					tmLogoutDayRefill.tm_isdst = -1;

					tRefill = mktime( &tmLogoutDayRefill );
				}
			}

			// 로그아웃 했던 시점 이후로 적용되어야 할 가장 가까운 충전 시점을 지난 상태면 충전 처리
			if( tCurrent >= tRefill )
			{
				m_pPlayer->m_nHuntaholicEnterableCount = GameRule::HUNTAHOLIC_ENTERANCE_COUNT_PER_DAY;

				// 다음 충전 시점을 캐릭터 정보에 미리 세팅
				struct tm tmNextRefill;
				errno_t nError = localtime_s( &tmNextRefill, &tCurrent );
				if( nError )
					break;

				// 이미 오늘 충전 시점이 지났으면 내일 충전 시점을 얻음
				if( tmNextRefill.tm_hour >= 6 )
					++tmNextRefill.tm_mday;
				tmNextRefill.tm_hour = 6;
				tmNextRefill.tm_min = 0;
				tmNextRefill.tm_sec = 0;
				tmNextRefill.tm_isdst = -1;

				// 오류 나도 처리 안 함. 그대로 로그인 해 있으면 m_tNextHuntaholicEnterableCountRefill가 -1(INFINITE)이라서 영원히 충전 안 됨(다음 충전 시점을 못 구하는데 어쩌라규 -_ -;;)
				m_pPlayer->m_tNextHuntaholicEnterableCountRefill = mktime( &tmNextRefill );
			}
			// 로그아웃 했던 시점 이후로 적용되어야 할 가장 가까운 충전 시점을 아직 안 지났으면 다음 충전 시점 정보만 세팅해줌
			else
			{
				m_pPlayer->m_tNextHuntaholicEnterableCountRefill = tRefill;
			}
		} while( false );

		bool bLayerProc = false;

		int huntaholic_id = HuntaholicManager::Instance().GetHuntaholicID( m_pPlayer->GetPos() );
		int instance_dungeon_id = InstanceDungeonManager::Instance().GetInstanceDungeonID( m_pPlayer->GetPos() );
		int location_type = WorldLocationManager::Instance().GetLocationType( GameContent::GetLocationId( m_pPlayer->GetX(), m_pPlayer->GetY() ) );

		if( huntaholic_id )
		{
			if( HuntaholicManager::Instance().IsHuntaholicDungeon( m_pPlayer->GetPos() ) )
			{
				// 인스턴스 던전 소속이 아니거나 탈퇴 처리가 제대로 안 됐으면 로비로 강제 이동
				if( !m_pPlayer->IsInParty() || HuntaholicManager::Instance().QuitHunting( huntaholic_id, m_pPlayer, false ) != RESULT_SUCCESS )
				{
					ArPosition pos = HuntaholicManager::Instance().GetLobbyPosition( huntaholic_id );

					m_pPlayer->SetCurrentXY( pos.x, pos.y );

					int nPartyID = m_pPlayer->GetPartyID();
					if( nPartyID )
					{
						if( PartyManager::GetInstance().IsLeader( nPartyID, m_pPlayer->GetPlayerUID() ) )
						{
							LOG::Log11N4S( LM_PARTY_DESTROY, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), nPartyID, m_pPlayer->GetX(), m_pPlayer->GetY(), m_pPlayer->GetLayer(), 10, 0, 0, 0, 0,
								m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

							BroadcastPartyDestroy( nPartyID );

							PartyManager::GetInstance().DestroyParty( nPartyID );
						}
						else
						{
							BroadcastPartyLeave( m_pPlayer );

							LOG::Log11N4S( LM_PARTY_LEAVE, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), nPartyID, m_pPlayer->GetX(), m_pPlayer->GetY(), m_pPlayer->GetLayer(), 4, 0, 0, 0, 0,
								m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

							PartyManager::GetInstance().LeaveParty( nPartyID, m_pPlayer->GetPlayerUID() );
						}

						// 파티 탈퇴가 제대로 되었어야 함
						assert( !PartyManager::GetInstance().IsMember( nPartyID, m_pPlayer->GetPlayerUID() ) );

						// SendLoginResult 메시지가 보내지기 전에는 캐릭터가 로그인되지 않은 것으로 간주되므로
						// 해당 캐릭터의 m_nPartyID 값이 설정되지 않음. 그래서 직접 설정~
						m_pPlayer->SetPartyID( 0 );
					}
				}
			}
			else
			{
				// 로비로 접속했는데 파티 ID가 있으면 방 탈퇴 또는 파티 탈퇴/해산
				if( m_pPlayer->IsInParty() && HuntaholicManager::Instance().LeaveInstanceDungeon( huntaholic_id, m_pPlayer ) != RESULT_SUCCESS )
				{
					int nPartyID = m_pPlayer->GetPartyID();
					if( nPartyID )
					{
						if( PartyManager::GetInstance().IsLeader( nPartyID, m_pPlayer->GetPlayerUID() ) )
						{
							LOG::Log11N4S( LM_PARTY_DESTROY, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), nPartyID, m_pPlayer->GetX(), m_pPlayer->GetY(), m_pPlayer->GetLayer(), 10, 0, 0, 0, 0,
								m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

							BroadcastPartyDestroy( nPartyID );

							PartyManager::GetInstance().DestroyParty( nPartyID );
						}
						else
						{
							BroadcastPartyLeave( m_pPlayer );

							LOG::Log11N4S( LM_PARTY_LEAVE, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), nPartyID, m_pPlayer->GetX(), m_pPlayer->GetY(), m_pPlayer->GetLayer(), 4, 0, 0, 0, 0,
								m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, PartyManager::GetInstance().GetPartyName( nPartyID ).c_str(), LOG::STR_NTS, "", 0 );

							PartyManager::GetInstance().LeaveParty( nPartyID, m_pPlayer->GetPlayerUID() );
						}

						// 파티 탈퇴가 제대로 되었어야 함
						assert( !PartyManager::GetInstance().IsMember( nPartyID, m_pPlayer->GetPlayerUID() ) );

						// SendLoginResult 메시지가 보내지기 전에는 캐릭터가 로그인되지 않은 것으로 간주되므로
						// 해당 캐릭터의 m_nPartyID 값이 설정되지 않음. 그래서 직접 설정~
						m_pPlayer->SetPartyID( 0 );
					}
				}
			}

			m_pPlayer->SetCurrentLayer( HuntaholicManager::Instance().GetProperLobbyLayer( huntaholic_id, m_pPlayer->GetLevel() ) );

			bLayerProc = true;
		}
		else if( location_type == StructWorldLocation::LOCATION_DEATHMATCH 
			|| location_type == StructWorldLocation::LOCATION_SECRET_DUNGEON
			|| location_type == StructWorldLocation::LOCATION_INSTANCE_DUNGEON )
		{
			ArPosition pos;
			if( location_type == StructWorldLocation::LOCATION_DEATHMATCH )
			{
				m_pPlayer->GetPositionOnEnterInstanceGame( &pos );
				bWasInDeathmatch = true;
			}
			else if( instance_dungeon_id )
			{
				// 인던 내부에서 로그인하는 경우 밖으로 쫓아냄
				m_pPlayer->GetPositionOnEnterInstanceGame( &pos );
			}
			else
				m_pPlayer->GetLastTownPosition( &pos );

			if( pos.x || pos.y || pos.z )
			{
				unsigned char layer = 0;

				int current_channel = ChannelManager::GetChannelId( m_pPlayer->GetX(), m_pPlayer->GetY() );
				int target_channel = ChannelManager::GetChannelId( pos.x, pos.y );

				if( current_channel && current_channel == target_channel )
				{
					layer = m_pPlayer->GetLayer();
				}
				else if( target_channel )
				{
					layer = ChannelManager::GetProperLayer( pos.x, pos.y );
				}

				m_pPlayer->SetCurrentXY( pos.x, pos.y );
				m_pPlayer->SetCurrentLayer( layer );

				bLayerProc = true;
			}
		}
		// 진행 중인 경기에 재참여되는 조건은 캐릭터의 위치로는 처리 불가능(카운트다운 중에는 경기장 밖에 존재할 수 있으므로)
		else if( m_pPlayer->GetPartyID() && PartyManager::GetInstance().IsBattleArenaTeamParty( m_pPlayer->GetPartyID() ) )
		{
			// 배틀 아레나 경기에 재참여 시도
			ArPosition pos = m_pPlayer->GetPos();;
			unsigned char layer = m_pPlayer->GetLayer();
			unsigned short nResult = RESULT_SUCCESS;

			// OnReconnect 안에서 스크립트 실행이 있으므로 지역락을 걸고 호출
			{
				ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

				// 경기장 밖에 있다면 좌표 조정 없이 경기장 밖에 로그인되게 하고,
				// 경기장 내부에 있었다면 경기 시작 좌표로 로그인되도록 조정
				if( location_type == StructWorldLocation::LOCATION_BATTLE_ARENA )
					nResult = BattleArenaManager::Instance().OnReconnect( m_pPlayer, &pos, &layer );
				else
					nResult = BattleArenaManager::Instance().OnReconnect( m_pPlayer, NULL, NULL );
			}

			if( nResult != RESULT_SUCCESS )
			{
				// 오프라인 상태에서 페널티를 받은 유저면 안내 메시지 출력 예약
				bBattleArenaOfflinePenaltyReceived = BattleArenaManager::Instance().IsOfflinePenaltyReceivedPlayer( m_pPlayer->GetPlayerUID() );

				// 실패 시에는 입장 전 위치로 돌려 보냄
				m_pPlayer->GetPositionOnEnterInstanceGame( &pos );
				layer = ChannelManager::GetProperLayer( pos.x, pos.y );

				m_pPlayer->RestoreStatesOnLeaveInstanceGame( false );

				// 사망 상태에서 경기장 내부였어서 외부로 내보내는 경우는 부활시켜줌(대모요정의 병으로 부활 시 각종 부작용 생기므로...)
				// 아레나 내부에서 부활 시에는 모든 지속효과를 복구하지만 이 시점에서는 복구를 해주더라도 퇴장 처리에서 결국 모두 삭제되기 때문에 복구해주지 않는다.
				m_pPlayer->m_nHP = m_pPlayer->GetMaxHP();
				m_pPlayer->m_nMP = m_pPlayer->GetMaxMP();
				LOG::Log11N4S( LM_CHARACTER_RESURRECTION, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), CRT_BATTLE_ARENA, 0, 0, 0,
					m_pPlayer->GetX(), m_pPlayer->GetY(), m_pPlayer->GetLayer(), 0, m_pPlayer->GetEXP(),
					m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "" , LOG::STR_NTS );

				bWasInBattleArena = true;
			}
			else
				bBattleArenaReconnectSucceed = true;

			m_pPlayer->SetCurrentXY( pos.x, pos.y );
			m_pPlayer->SetCurrentLayer( layer );

			bLayerProc = ( location_type == StructWorldLocation::LOCATION_BATTLE_ARENA && nResult == RESULT_SUCCESS );
		}
		// 파티 정보를 기준으로 BattleArenaManager::OnReconnect가 호출되지는 못했는데(위의 if문에 안 걸렸는데)
		// 경기장 내부에서 유저가 로그인했다면 외부로 이탈시킴
		// * 오프라인이던 사이에 경기가 종료되거나 파티가 해산된 경우
		else if( location_type == StructWorldLocation::LOCATION_BATTLE_ARENA )
		{
			// 오프라인 상태에서 페널티를 받은 유저면 안내 메시지 출력 예약
			bBattleArenaOfflinePenaltyReceived = BattleArenaManager::Instance().IsOfflinePenaltyReceivedPlayer( m_pPlayer->GetPlayerUID() );

			ArPosition pos;

			// 입장 전 위치로 돌려 보냄
			m_pPlayer->GetPositionOnEnterInstanceGame( &pos );
			unsigned char layer = ChannelManager::GetProperLayer( pos.x, pos.y );

			m_pPlayer->RestoreStatesOnLeaveInstanceGame( false );

			// 사망 상태에서 경기장 내부였어서 외부로 내보내는 경우는 부활시켜줌(대모요정의 병으로 부활 시 각종 부작용 생기므로...)
			// 아레나 내부에서 부활 시에는 모든 지속효과를 복구하지만 이 시점에서는 복구를 해주더라도 퇴장 처리에서 결국 모두 삭제되기 때문에 복구해주지 않는다.
			m_pPlayer->m_nHP = m_pPlayer->GetMaxHP();
			m_pPlayer->m_nMP = m_pPlayer->GetMaxMP();
			LOG::Log11N4S( LM_CHARACTER_RESURRECTION, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), CRT_BATTLE_ARENA, 0, 0, 0,
				m_pPlayer->GetX(), m_pPlayer->GetY(), m_pPlayer->GetLayer(), 0, m_pPlayer->GetEXP(),
				m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "" , LOG::STR_NTS );

			bWasInBattleArena = true;

			m_pPlayer->SetCurrentXY( pos.x, pos.y );
			m_pPlayer->SetCurrentLayer( layer );

			bLayerProc = true;
		}
		else if( m_pPlayer->GetLayer() > 0 )
		{
			int dungeon_id = DungeonManager::Instance().GetDungeonID( m_pPlayer->GetX(), m_pPlayer->GetY() );

			if( dungeon_id )
			{
				do
				{
					if( logout_duration * 60 > GameRule::CHANNEL_PRESERVE_TIME || m_pPlayer->GetLayer() == DungeonManager::CUSTOMIZE_DUNGEON_LAYER )
						break;

					if( !m_pPlayer->GetPartyID() )
						break;

					int guild_id = PartyManager::GetInstance().GetAttackTeamGuildID( m_pPlayer->GetPartyID() );

					if( !guild_id )
						break;

					if( m_pPlayer->GetLayer() == DungeonManager::DUNGEON_SIEGE_LAYER )
					{
						if( !DungeonManager::Instance().IsEnterableSiegeDungeon( dungeon_id, guild_id ) )
							break;

						ArPosition curPos = m_pPlayer->GetPos();
						int nGuildID = m_pPlayer->GetGuildID();
						int nAllianceID = GuildManager::GetInstance().GetAllianceID( nGuildID );
						if( nAllianceID )
						{
							nGuildID = GuildManager::GetInstance().GetAllianceLeaderGuildID( nAllianceID );
						}

						// 던전을 소유하여 방어자 길드일 경우 1, 그렇지 않아 공격자 길드일 경우 0
						int nPosition = ( nGuildID == DungeonManager::Instance().GetOwnGuildID( dungeon_id ) );

						ArPosition pos = nPosition ? DungeonManager::Instance().GetSiegeDefencePosition( dungeon_id ) : DungeonManager::Instance().GetSiegeStartPosition( dungeon_id );

						m_pPlayer->SetCurrentXY( pos.x, pos.y );

						bLayerProc = true;

						if( nPosition )
							PrintfChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", m_pPlayer, "SIEGE_OPPONENT|%s|", GuildManager::GetInstance().GetGuildName( DungeonManager::Instance().GetRaidGuildID( dungeon_id ) ).c_str() );
						else
							PrintfChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", m_pPlayer, "SIEGE_OPPONENT|%s|", GuildManager::GetInstance().GetGuildName( DungeonManager::Instance().GetOwnGuildID( dungeon_id ) ).c_str() );

						PrintfChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", m_pPlayer, "SIEGE_STATUS|%d|%d|%d|", DungeonManager::Instance().GetConnectorHP( dungeon_id ), DungeonManager::Instance().GetDungeonCoreHP( dungeon_id ), nPosition );

						// 공격자인지 방어자인지는 원래 던전 소유길드를 기준으로 판단되어야 한다.
						m_pPlayer->UpdateTitleConditionByDungeonSiegeStart( dungeon_id, nGuildID != DungeonManager::Instance().GetOriginalOwnGuildID( dungeon_id ) );
					}
					else
					{
						unsigned char layer = DungeonManager::Instance().GetRaidDungeonLayer( dungeon_id, guild_id );

						if( !layer )
							break;

						if( layer != m_pPlayer->GetLayer() )
							break;

						ArPosition pos = DungeonManager::Instance().GetRaidStartPosition( dungeon_id );
						m_pPlayer->SetCurrentXY( pos.x, pos.y );

						bLayerProc = true;

						PrintfChatMessage( false, CHAT_RAID_SYSTEM, "@RAID", m_pPlayer, "RAID_ENTER|%d|", DungeonManager::Instance().GetElaspedRaidTime( dungeon_id, layer ) );
					}


				} while( false );

				if( !bLayerProc )
				{
					ArPosition pos = DungeonManager::Instance().GetRaidStartPosition( dungeon_id );

					m_pPlayer->SetCurrentXY( pos.x, pos.y );
					m_pPlayer->SetCurrentLayer( 0 );

					bLayerProc = true;
				}
			}
		}

		if( !bLayerProc )
		{
#ifdef _DEBUG
			int nChannelID = ChannelManager::GetChannelId( m_pPlayer->GetX(), m_pPlayer->GetY() );
			if( ( 120501 <= nChannelID && nChannelID <= 120504 ) || ( 120601 <= nChannelID && nChannelID <= 120604 ) )
			{
				_cprint( "Player is in deathmatch channel but the world location info does not match(Login)[%f,%f ChannelID:%d].\n", m_pPlayer->GetX(), m_pPlayer->GetY(), nChannelID );
				FILELOG( "Player is in deathmatch channel but the world location info does not match(Login)[%f,%f ChannelID:%d].", m_pPlayer->GetX(), m_pPlayer->GetY(), nChannelID );
			}
#endif

			unsigned char layer = ChannelManager::GetProperLayer( m_pPlayer->GetX(), m_pPlayer->GetY() );

			if( logout_duration * 60 > GameRule::CHANNEL_PRESERVE_TIME || layer == 0 )
			{
				m_pPlayer->SetCurrentLayer( layer );
			}
		}

		int i;

		for( i = 0; i < 6; ++i )
		{
			bindSummon[i] = cmd->Parameters->Item[ OUT_SUMMON[i] ]->Value;
		}

		for (i = 0; i < 8; ++i)
		{
			beltSlot[i] = cmd->Parameters->Item[OUT_BELT[i]]->Value;
		}


		mainSummon = cmd->Parameters->Item[ "OUT_MAIN_SUMMON" ]->Value.intVal;
		subSummon = cmd->Parameters->Item[ "OUT_SUB_SUMMON" ]->Value.intVal;
		remainSummonTime = cmd->Parameters->Item[ "OUT_REMAIN_SUMMON_TIME" ]->Value.intVal;

		pet = cmd->Parameters->Item[ "OUT_PET" ]->Value.intVal;

		main_title = cmd->Parameters->Item[ "OUT_MAIN_TITLE" ]->Value;
		for( i = 0; i < GameRule::SUB_TITLE_COUNT; i++ )
		{
			sub_title[i] = cmd->Parameters->Item[ OUT_SUB_TITLE[i] ]->Value;
		}

		remain_title_time = cmd->Parameters->Item[ "OUT_REMAIN_TITLE_TIME" ]->Value.intVal;

		// 첫 로그인이면
		if( !m_pPlayer->GetLevel() )
		{
			m_pPlayer->m_nLevel = 1;
			m_pPlayer->m_nMaxReachedLevel = 1;
			m_pPlayer->m_nJobLevel = 1;
			// 어차피 스크립트 on_Login.lua에서 하기로 했3 'ㅡ'; 여기서 하면 너무 일찍(직업 결정 전)임
			m_pPlayer->m_nHP = m_pPlayer->GetMaxHP();
			m_pPlayer->m_nMP = m_pPlayer->GetMaxMP();
		}

		// 스킬 읽자
		readCreatureSkillList( db, m_pPlayer );
		for( std::vector< struct StructSkill * >::const_iterator it = m_pPlayer->m_vAllSkillList.begin(); it != m_pPlayer->m_vAllSkillList.end(); ++it )
		{
			if( (*it)->GetSkillUID() < 0 )
				continue;

			m_pPlayer->m_TitleManager.UpdateTitleConditionBySkillLevel( (*it)->GetSkillId(), (*it)->GetBaseSkillLevel() );
		}

		// 여기서 완료시키면 Charm 류의 아이템 정보를 readItemList에서 읽을 때 CalculateStat 함수가 호출되므로
		// readItemList까지 완료된 후에 CalculateStat이 호출되어야 함.
		//m_pPlayer->SetLoginComplete();
	}
	else
	{
		FILELOG( "Character not found~! (%s)", szCharacterName );
		_cprint( "Character not found~! (%s)\n", szCharacterName );
		return RESULT_NOT_EXIST;
	}

	return RESULT_SUCCESS;
}

bool DB_Login::readPlayTime( DBConnection & db )
{
	if( !GameRule::bUsePlayPoint )
	{
		return false;
	}

	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readPlayTime : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_play_time" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_play_time";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_PLAY_TIME", adInteger, adParamOutput, 4, 0 ) );

	cmd->Execute( NULL, NULL, adCmdStoredProc );

	m_pPlayer->SetPlayTime( cmd->Parameters->Item[ "OUT_PLAY_TIME" ]->Value.intVal );	// OUT_PLAY_TIME 분 단위

	return true;
}

bool DB_Login::readItemCoolTime( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readItemCoolTime : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_item_cool_time" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_item_cool_time";
	
	cmd->Parameters->Append( cmd->CreateParameter( "@IN_OWNER_UID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	AR_TIME t = GetArTime();

	if( pRS->State != adStateClosed && !pRS->EndOfFile )
	{	
		m_pPlayer->m_nItemCoolTime[0] = t + pRS->Fields->Item["cool_time_00"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[1] = t + pRS->Fields->Item["cool_time_01"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[2] = t + pRS->Fields->Item["cool_time_02"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[3] = t + pRS->Fields->Item["cool_time_03"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[4] = t + pRS->Fields->Item["cool_time_04"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[5] = t + pRS->Fields->Item["cool_time_05"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[6] = t + pRS->Fields->Item["cool_time_06"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[7] = t + pRS->Fields->Item["cool_time_07"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[8] = t + pRS->Fields->Item["cool_time_08"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[9] = t + pRS->Fields->Item["cool_time_09"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[10] = t + pRS->Fields->Item["cool_time_10"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[11] = t + pRS->Fields->Item["cool_time_11"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[12] = t + pRS->Fields->Item["cool_time_12"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[13] = t + pRS->Fields->Item["cool_time_13"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[14] = t + pRS->Fields->Item["cool_time_14"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[15] = t + pRS->Fields->Item["cool_time_15"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[16] = t + pRS->Fields->Item["cool_time_16"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[17] = t + pRS->Fields->Item["cool_time_17"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[18] = t + pRS->Fields->Item["cool_time_18"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[19] = t + pRS->Fields->Item["cool_time_19"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[20] = t + pRS->Fields->Item["cool_time_20"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[21] = t + pRS->Fields->Item["cool_time_21"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[22] = t + pRS->Fields->Item["cool_time_22"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[23] = t + pRS->Fields->Item["cool_time_23"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[24] = t + pRS->Fields->Item["cool_time_24"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[25] = t + pRS->Fields->Item["cool_time_25"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[26] = t + pRS->Fields->Item["cool_time_26"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[27] = t + pRS->Fields->Item["cool_time_27"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[28] = t + pRS->Fields->Item["cool_time_28"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[29] = t + pRS->Fields->Item["cool_time_29"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[30] = t + pRS->Fields->Item["cool_time_30"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[31] = t + pRS->Fields->Item["cool_time_31"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[32] = t + pRS->Fields->Item["cool_time_32"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[33] = t + pRS->Fields->Item["cool_time_33"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[34] = t + pRS->Fields->Item["cool_time_34"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[35] = t + pRS->Fields->Item["cool_time_35"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[36] = t + pRS->Fields->Item["cool_time_36"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[37] = t + pRS->Fields->Item["cool_time_37"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[38] = t + pRS->Fields->Item["cool_time_38"]->Value.intVal;
		m_pPlayer->m_nItemCoolTime[39] = t + pRS->Fields->Item["cool_time_39"]->Value.intVal;

		// 36~40번 그룹은 로그아웃 시간만큼 쿨타임을 차감
		AR_TIME nLogoutDurationInArTime = logout_duration * 100 * 60;
		m_pPlayer->m_nItemCoolTime[35] = ( m_pPlayer->m_nItemCoolTime[35] > nLogoutDurationInArTime ) ? m_pPlayer->m_nItemCoolTime[35] - nLogoutDurationInArTime : 0;
		m_pPlayer->m_nItemCoolTime[36] = ( m_pPlayer->m_nItemCoolTime[36] > nLogoutDurationInArTime ) ? m_pPlayer->m_nItemCoolTime[36] - nLogoutDurationInArTime : 0;
		m_pPlayer->m_nItemCoolTime[37] = ( m_pPlayer->m_nItemCoolTime[37] > nLogoutDurationInArTime ) ? m_pPlayer->m_nItemCoolTime[37] - nLogoutDurationInArTime : 0;
		m_pPlayer->m_nItemCoolTime[38] = ( m_pPlayer->m_nItemCoolTime[38] > nLogoutDurationInArTime ) ? m_pPlayer->m_nItemCoolTime[38] - nLogoutDurationInArTime : 0;
		m_pPlayer->m_nItemCoolTime[39] = ( m_pPlayer->m_nItemCoolTime[39] > nLogoutDurationInArTime ) ? m_pPlayer->m_nItemCoolTime[39] - nLogoutDurationInArTime : 0;
	}

	return true;
}

bool DB_Login::readSummonList( DBConnection & db )
{
	if( m_pPlayer->GetLogoutTime() ) return false;

	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readSummonList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_summon_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_summon_list";
	
	cmd->Parameters->Item["@IN_SID"]->Value = sid ;

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{	
		StructSummon *pSummon = StructSummon::AllocSummon( m_pPlayer, pRS->Fields->Item["code"]->Value );
		// _oprint( "ALLOC SUMMON : %08X\n", pSummon );
		pSummon->SetEXP( pRS->Fields->Item["exp"]->Value );
		pSummon->SetLevel( pRS->Fields->Item["lv"]->Value );
		pSummon->SetJobLevel( pRS->Fields->Item["lv"]->Value );
		pSummon->SetMaxReachedLevel( pRS->Fields->Item["max_level"]->Value );
		pSummon->SetSummonSID( pRS->Fields->Item["sid"]->Value );
		pSummon->SetJP( pRS->Fields->Item["jp"]->Value );
		pSummon->SetName( static_cast< _bstr_t >( pRS->Fields->Item["name"]->Value ) );
		pSummon->SetCurrentXY( m_pPlayer->GetX(), m_pPlayer->GetY() );
		pSummon->SetPrevJobId( 0, pRS->Fields->Item["prev_id_01"]->Value );
		pSummon->SetPrevJobId( 1, pRS->Fields->Item["prev_id_02"]->Value );
		pSummon->SetPrevJobLevel( 0, pRS->Fields->Item["prev_level_01"]->Value );
		pSummon->SetPrevJobLevel( 1, pRS->Fields->Item["prev_level_02"]->Value );

		pSummon->m_nLastDecreasedEXP = pRS->Fields->Item["last_decreased_exp"]->Value.llVal;

		// 카드 정보 얻기
		bool bIsInInventory = true;
		ItemUID uid = pRS->Fields->Item["card_uid"]->Value.llVal;
		StructItem *pCard = m_pPlayer->FindItem( uid );

		if( !pCard )
		{
			// 농장에 맡겨진 카드는 인벤에서 찾을 수 없지만 소환수 정보를 할당해 주어야 한다
			bIsInInventory = false;

			for( std::vector< StructItem * >::const_iterator it = vFarmedSummonCard.begin(); it != vFarmedSummonCard.end(); ++it )
			{
				if( (*it)->GetItemUID() == uid )
				{
					pCard = (*it);
					break;
				}
			}
		}

		if( pCard ) 
		{
			pSummon->SetParentCard( pCard );
			pCard->SetSummonStruct( pSummon );
			pCard->SetSummonSID( pSummon->GetSummonSID() );

			// 소환수 카드와 아이템의 소환수 코드가 잘못 연결 되 있으면 제대로 연결
			if( pCard->GetSummonCode() != pSummon->GetSummonCode() )
			{
				pCard->SetSummonCode( pSummon->GetSummonCode() );
				pCard->DBQuery( new DB_UpdateItem( pCard ) );
				// 크리쳐 농장 등에 맡긴 경우 카드가 실제 인벤토리에 없을 수도 있다.
				if( m_pPlayer->m_Inventory.Find( pCard->GetItemUID() ) )
					SendItemMessage( m_pPlayer, pCard );
			}

			if( bIsInInventory )
				m_pPlayer->AddSummon( pSummon, false, false );

			readCreatureSkillList( db, pSummon );
			
			readStateInfo( db, pSummon );

			pSummon->SetLoginComplete();

			pSummon->m_nSP = pRS->Fields->Item["sp"]->Value;
			pSummon->m_nHP = pRS->Fields->Item["hp"]->Value;
			pSummon->m_nMP = pRS->Fields->Item["mp"]->Value;

#ifdef USE_CLIENT97
			if (pSummon->m_nSP > 10)
				pSummon->m_nSP = 0;
#endif 
		}
		else
		{
			// 대응하는 카드가 없으면 소환수 제거
			pSummon->SetAccountId( 0 );
			pSummon->SetMaster( NULL );
			pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );
			ArcadiaServer::Instance().DeleteObject( pSummon );
		}

		pRS->MoveNext();
	}

	for( std::vector< StructItem * >::const_iterator it = vFarmedSummonCard.begin(); it != vFarmedSummonCard.end(); ++it )
	{
		StructItem * pItem = (*it);
		StructSummon * pSummon = pItem->GetSummonStruct();

		if( !pSummon )
		{
			assert( 0 && "Farming info exist, but corresponding summon info does not");
			_cprint( "Lost summon info!: Account[%s] Character[%s] Item(%I64d)\n", m_pPlayer->GetAccountName(), m_pPlayer->GetName(), (*it)->GetItemUID() );
			FILELOG( "Lost summon info!: Account[%s] Character[%s] Item(%I64d)", m_pPlayer->GetAccountName(), m_pPlayer->GetName(), (*it)->GetItemUID() );

			// 할당
			pSummon = AllocNewSummon( m_pPlayer, pItem );
			pSummon->SetLoginComplete();

			std::string strFirstSummonHandler;

			XStringUtil::Format( strFirstSummonHandler, "on_first_summon( %d, %u )", pSummon->GetSummonCode(), pSummon->GetHandle() );

			// 경우에 따라 지역락이 필요할 수도 있음
			// * 소환수는 월드에 있지도 않지만 간혹 소환수의 좌표값만 참조해서 월드에 방송 날리는 씹숑키 함수들이 있음.
			//   해당 경우가 발생하면 pSummon->SetCurrentXY, SetCurrentLayer 호출해서 플레이어와 같은 위치로 맞춰주고 지역락 걸어주면 됨.
			ThreadPlayerHelper TPHelper( m_pPlayer );
			LUA()->RunString( strFirstSummonHandler.c_str() );

			pSummon->CalculateStat();

			// DB에 기록
			m_pPlayer->DBQuery( new DB_InsertSummon( m_pPlayer, pSummon->GetSID(), 0, m_pPlayer->GetPlayerUID(), pSummon->GetSummonCode(), pSummon->GetParentCard()->GetItemUID(), pSummon->GetName(), pSummon->GetSP(), pSummon->GetHP(), pSummon->GetMP(), pSummon->GetJobPoint() ) );
		}
	}

	return true;
}

bool DB_Login::readFarmedSummonInfoList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readFarmedSummonInfoList : CreateInstance(command) error" );
	
	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_farm_info" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_farm_info";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_pPlayer->GetSID() ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		bool bFound = false;
		for( std::vector< StructItem * >::iterator it = vFarmedSummonCard.begin(); it != vFarmedSummonCard.end(); ++it )
		{
			if( (*it)->GetItemUID() == static_cast< __int64 >( pRS->Fields->Item["item_id"]->Value ) )
			{
				bFound = true;
				int farm_sid = pRS->Fields->Item["sid"]->Value;
				StructItem *item = (*it);
				char slot = pRS->Fields->Item["slot"]->Value;
				slot -= 1;
				int max_level = pRS->Fields->Item["max_level"]->Value;
				bool is_using_cracker = pRS->Fields->Item["is_using_cracker"]->Value;
				bool is_cash = pRS->Fields->Item["is_cash"]->Value;
				int duration = pRS->Fields->Item["duration"]->Value;
				
				COleDateTime dtTime( pRS->Fields->Item["registration_time"]->Value.date );
				struct tm tmTime;
				tmTime.tm_year = dtTime.GetYear() - 1900;
				tmTime.tm_mon = dtTime.GetMonth() - 1;
				tmTime.tm_mday = dtTime.GetDay();
				tmTime.tm_hour = dtTime.GetHour();
				tmTime.tm_min = dtTime.GetMinute();
				tmTime.tm_sec = dtTime.GetSecond();
				tmTime.tm_isdst = -1;
				time_t registration_time = mktime( &tmTime );
				dtTime = COleDateTime( pRS->Fields->Item["nursing_time"]->Value.date );
				tmTime.tm_year = dtTime.GetYear() - 1900;
				tmTime.tm_mon = dtTime.GetMonth() - 1;
				tmTime.tm_mday = dtTime.GetDay();
				tmTime.tm_hour = dtTime.GetHour();
				tmTime.tm_min = dtTime.GetMinute();
				tmTime.tm_sec = dtTime.GetSecond();
				tmTime.tm_isdst = -1;
				time_t nursing_time = mktime( &tmTime );

				m_pPlayer->SetFarmedSummonInfo( slot, new StructPlayer::FARMED_SUMMON_INFO( farm_sid, item, max_level, is_using_cracker, is_cash, registration_time, duration, nursing_time ) );

				if( registration_time + duration < time( NULL ) )
				{
					// 만료된 농장 정보 제거 및 로그
					time_t tExpiredTime = registration_time + duration;
					struct tm tmExpiredTime;
					errno_t nError = localtime_s( &tmExpiredTime, &tExpiredTime );

					// 문제가 있다면 삭제 처리는 하지 않고 남겨둔다.
					if( nError )
					{
						assert( 0 && "Invalid farm expired time" );
						_cprint( "Invalid farm expired time!: Account[%s] Character[%s] Item(%I64d), Time(%I64d)\n", m_pPlayer->GetAccountName(), m_pPlayer->GetName(), (*it)->GetItemUID(), tExpiredTime );
						FILELOG( "Invalid farm expired time!: Account[%s] Character[%s] Item(%I64d), Time(%I64d)", m_pPlayer->GetAccountName(), m_pPlayer->GetName(), (*it)->GetItemUID(), tExpiredTime );
					}
					else
					{
						m_pPlayer->RegainSummon( item->GetHandle() );
						PrintfChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, "@1158" );
					}
				}

				vector_fast_erase( &vFarmedSummonCard, it );

				break;
			}
		}

		if( !bFound )
		{
			// 카드 정보를 찾을 수 없음
			assert( 0 && "Lost farmed card" );
			_cprint( "Lost farmed card!: Account[%s] Character[%s] Item(%I64d)\n", m_pPlayer->GetAccountName(), m_pPlayer->GetName(), static_cast< __int64 >( pRS->Fields->Item["item_id"]->Value ) );
			FILELOG( "Lost farmed card!: Account[%s] Character[%s] Item(%I64d)", m_pPlayer->GetAccountName(), m_pPlayer->GetName(), static_cast< __int64 >( pRS->Fields->Item["item_id"]->Value ) );

			m_pPlayer->DBQuery( new DB_DeleteFarmInfo( m_pPlayer, pRS->Fields->Item["sid"]->Value ) );
		}

		pRS->MoveNext();
	}

	// 남아있는 카드는 농장 정보를 못 찾은 카드
	for( std::vector< StructItem * >::iterator it = vFarmedSummonCard.begin(); it != vFarmedSummonCard.end(); ++it )
	{
		assert( 0 && "Lost farm info" );
		_cprint( "Lost farm info!: Account[%s] Character[%s] Item(%I64d)\n", m_pPlayer->GetAccountName(), m_pPlayer->GetName(), (*it)->GetItemUID() );
		FILELOG( "Lost farm info!: Account[%s] Character[%s] Item(%I64d)", m_pPlayer->GetAccountName(), m_pPlayer->GetName(), (*it)->GetItemUID() );

		(*it)->SetOwnerInfo( NULL, 0, 0 );
		(*it)->DBQuery( new DB_UpdateItemOwner( (*it) ) );
		StructItem::PendFreeItem( (*it) );
	}
	return true;
}

bool DB_Login::readPetList( DBConnection & db )
{
	if( m_pPlayer->GetLogoutTime() ) return false;

	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readPetList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_pet_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_pet_list";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, sid ) );

	_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{	

		StructPet *pPet = StructPet::AllocPet( m_pPlayer, pRS->Fields->Item["code"]->Value );
		pPet->SetPetSID( pRS->Fields->Item["sid"]->Value );
		pPet->SetName( static_cast< _bstr_t >( pRS->Fields->Item["name"]->Value ) );
		pPet->SetNameChanged( pRS->Fields->Item["name_changed"]->Value );
		pPet->GetSkill( StructSkill::SKILL_SHOVELING )->SetRemainCoolTime( pRS->Fields->Item["cool_time_01"]->Value.intVal * 100 );

		pPet->SetCurrentXY( m_pPlayer->GetX(), m_pPlayer->GetY() );

		// 우리 정보 얻기
		StructItem *pCage = m_pPlayer->FindItem( pRS->Fields->Item["cage_uid"]->Value.llVal );
		if( pCage ) 
		{
			pPet->SetParentCage( pCage );
			pCage->SetPetStruct( pPet );
			pCage->SetPetSID( pPet->GetPetSID() );

			m_pPlayer->AddPet( pPet, false, false );

			pPet->SetLoginComplete();

			pPet->m_nHP = 100;
			pPet->m_nMP = 100;
		}
		else
		{
			// 대응하는 우리가 없으면 펫 제거
			pPet->SetMaster( NULL );
			pPet->DBQuery( new DB_UpdatePet( pPet ) );
			ArcadiaServer::Instance().DeleteObject( pPet );
		}

		pRS->MoveNext();
	}

	return true;
}

bool DB_Login::readItemList( DBConnection & db )
{
	if( m_pPlayer->GetLogoutTime() ) return false;

	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	// 느린 쿼리 타임아웃 조정
	cmd->CommandTimeout = 60;
	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_item_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_item_list";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	std::vector< StructItem::INDEX_FINDER > vItemList;

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		int		nItemCode = pRS->Fields->Item["code"]->Value;
		__int64	nCount = pRS->Fields->Item["cnt"]->Value.llVal;

		COleDateTime dtElementalEffectExpire( pRS->Fields->Item["elemental_effect_expire_time"]->Value.date );

		struct tm tmElementalEffectExpire;
		tmElementalEffectExpire.tm_year = dtElementalEffectExpire.GetYear() - 1900;
		tmElementalEffectExpire.tm_mon = dtElementalEffectExpire.GetMonth() - 1;
		tmElementalEffectExpire.tm_mday = dtElementalEffectExpire.GetDay();
		tmElementalEffectExpire.tm_hour = dtElementalEffectExpire.GetHour();
		tmElementalEffectExpire.tm_min = dtElementalEffectExpire.GetMinute();
		tmElementalEffectExpire.tm_sec = dtElementalEffectExpire.GetSecond();
		tmElementalEffectExpire.tm_isdst = -1;
		time_t tElementalEffectExpire = mktime( &tmElementalEffectExpire );

		StructItem * pItem = StructItem::AllocItem(	pRS->Fields->Item["sid"]->Value,
										nItemCode,
										nCount,
										static_cast< ItemInstance::GenerateCode >( static_cast< int >( pRS->Fields->Item["gcode"]->Value ) ),
										pRS->Fields->Item["level"]->Value,
										pRS->Fields->Item["enhance"]->Value,
										pRS->Fields->Item["flag"]->Value,
										pRS->Fields->Item["socket_0"]->Value,
										pRS->Fields->Item["socket_1"]->Value,
										pRS->Fields->Item["socket_2"]->Value,
										pRS->Fields->Item["socket_3"]->Value,
										pRS->Fields->Item["awaken_sid"]->Value,
										pRS->Fields->Item["random_option_sid"]->Value,
										std::max( (int) pRS->Fields->Item["remain_time"]->Value, 0 ),
										pRS->Fields->Item["elemental_effect_type"]->Value.intVal,
										tElementalEffectExpire,
										pRS->Fields->Item["elemental_effect_attack_point"]->Value,
										pRS->Fields->Item["elemental_effect_magic_point"]->Value,
										pRS->Fields->Item["appearance_code"]->Value,
										pRS->Fields->Item["summon_code"]->Value,
										pRS->Fields->Item["effect_id"]->Value);


		if( pItem->IsAwaken() )
		{
			ItemInstance::RANDOM_OPTION kAwakenOption;
			getRandomOption( pItem->GetAwakenSID(), &kAwakenOption );
			pItem->SetAwakenOption( kAwakenOption );
		}

		if( pItem->IsIdentified() )
		{
			ItemInstance::RANDOM_OPTION kRandomOption;
			getRandomOption( pItem->GetIdentifiedSID(), &kRandomOption );
			pItem->SetIdentifiedOption( kRandomOption );
		}

		// 소환수 테이밍 중이라는 플래그는 DB에서 읽어지면 안 됨
		pItem->GetInstanceFlag().Off( ItemInstance::ITEM_FLAG_TAMING );

		pItem->SetCurrentEtherealDurability( pRS->Fields->Item["ethereal_durability"]->Value );
		pItem->SetCurrentEndurance( pRS->Fields->Item["endurance"]->Value );
		pItem->TurnOffDbUpdateFlag();

		if (pItem->GetItemCode() != nItemCode)
		{
			pItem->TurnOnUpdateFlag();
		}

		// 돈일 경우(인벤토리 소지 아이템으로는 돈이 존재할 수 없음. 제거함)
		if( !nItemCode )
		{
			pItem->SetOwnerInfo( NULL, 0, 0 );
			pItem->DBQuery( new DB_UpdateItemOwner( pItem ) );

			StructItem::PendFreeItem( pItem );
			continue;
		}

		// DB의 wear_info가 ItemBase::WEAR_NONE일 경우, 값의 변경이 없으므로 저장이 필요없으며,
		// 만약 wear_info가 ItemBase::WEAR_NONE이 아닐 경우, smp_read_equip_item_list_by_character의 조건에 의해 읽혀 readEquipItemList 함수에서 처리가 되므로 저장이 필요없다.
		pItem->SetWearInfo( ItemBase::WEAR_NONE, true );

		if( pItem->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FARMED_SUMMON ) )
		{
			pItem->SetOwnerInfo( m_pPlayer->GetHandle(), m_pPlayer->GetSID(), 0 );
			vFarmedSummonCard.push_back( pItem );
			
			continue;
		}

		pItem->SetPreviousUID( pRS->Fields->Item["previous_sid"]->Value );

		vItemList.push_back( StructItem::INDEX_FINDER( pItem ) );		
	}

	m_pPlayer->m_Inventory.LoadItemList( vItemList, false );
	
	return true;
}
bool DB_Login::setBeltSlotInfo()
{
	for( int i = 0; i < 8; ++i )
	{
		if( !beltSlot[i] )
			continue;

		StructItem * pItem = m_pPlayer->FindItem( beltSlot[i] );

		if( !pItem )
			continue;

		m_pPlayer->PutOnBelt( i, pItem );
	}

	return true;
}

bool DB_Login::readEquipItemList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_equip_item_list_by_character" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_equip_item_list_by_character";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	StructCreature * pCreature;
	
	ItemUID item_sid;
	ItemBase::ItemWearType pos;
	std::vector< int > vOverlappItemList;
	StructItem *pItem;

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		int summon_sid = pRS->Fields->Item["summon_id"]->Value;
		item_sid = pRS->Fields->Item["sid"]->Value;
		pos = static_cast< ItemBase::ItemWearType > ( static_cast< int > ( pRS->Fields->Item["wear_info"]->Value ) );

		if( pos >= ItemBase::MAX_ITEM_WEAR )
			continue;

		if( 0 == summon_sid )
		{
			pCreature = m_pPlayer;
		}
		else
		{
			pCreature = m_pPlayer->GetSummon( summon_sid );

			if( !pCreature )
				continue;
		}

		pItem = m_pPlayer->FindItem( item_sid );

		if( !pItem )
			continue;

		vOverlappItemList.clear();

#ifdef USE_CLIENT97
		if( !pCreature->TranslateWearPosition( pos, pItem, &vOverlappItemList ) || vOverlappItemList.size() > 0 )
		{
			pItem->TurnOnUpdateFlag();
			continue;
		}
#else
		if (pCreature->m_anWear[pos] || !pCreature->TranslateWearPosition(pos, pItem, &vOverlappItemList) || vOverlappItemList.size() > 0)
		{
			pItem->TurnOnUpdateFlag();
			continue;
		}

#endif

		pCreature->m_anWear[pos] = pItem;

		// DB에 저장되어있던 wear_info와 동일한 위치에 장착에 성공을 하였기에 저장할 필요가 없다.
		// 장착에 실패하여 저장 위치를 0으로 바꾸는 처리는 WearInfo를 설정하지 않는 채로 
		pItem->SetWearInfo( pos, true );

		// 장착된 아이템은 무게 체크 안해야 함
		m_pPlayer->GetInventory()->AddWeightModifier( -pItem->GetWeight() );

		if( pCreature->IsPlayer() && pos == ItemBase::WEAR_LEFTHAND && pItem->IsWeapon() )
		{
			pCreature->m_StatusFlag.On( StructCreature::STATUS_USING_DOUBLE_WEAPON );
		}

		if( pCreature->IsPlayer() )
		{
			m_pPlayer->UpdateTitleConditionByItemEquip( pItem->GetItemCode(), true );
		}
		else if( pCreature->IsSummon() )
		{
			pItem->SetOwnSummonInfo( pCreature->GetHandle(), pCreature->GetSID() );
		}

		pItem->SetBindedCreatureHandle( pCreature->GetHandle() );
	}
	m_pPlayer->UpdateWeightWithInventory();
	return true;
}

bool DB_Login::readStateInfo( DBConnection & db, StructSummon * pSummon )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_state_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_state_list";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, pSummon ? 0 : sid  ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_ID", adInteger, adParamInput, 4, pSummon ? pSummon->GetSummonSID() : 0  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	StructState state;
	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		int sid = pRS->Fields->Item["sid"]->Value;
		int code = pRS->Fields->Item["code"]->Value;

		unsigned short level = pRS->Fields->Item["level"]->Value;
		AR_TIME duration = pRS->Fields->Item["duration"]->Value;
		AR_TIME remain_time = pRS->Fields->Item["remain_time"]->Value;
		int base_damage = pRS->Fields->Item["base_damage"]->Value;
		AR_TIME remain_fire_time = pRS->Fields->Item["remain_fire_time"]->Value;

		int state_value = pRS->Fields->Item["state_value"]->Value;

		_bstr_t state_string_value = pRS->Fields->Item["state_string_value"]->Value;

		int enable = pRS->Fields->Item["enable"]->Value;

		const StateInfo * pInfo = GameContent::GetStateInfo( code );

		if( !pInfo )
			continue;

		if(	pInfo->state_time_type & StateInfo::TIME_DECREASE_ON_LOGOUT )
		{
			if( remain_time != AR_TIME(-1) )
			{
				// 만료 된 녀석이라면 remain_time을 0으로 넘겨 로그인 이후 만료되게 함.
				if( remain_time < AR_TIME(logout_duration * 100 * 60) )
					remain_time = 0;
				else
					remain_time -= AR_TIME(logout_duration * 100 * 60);
			}
		}

		AR_HANDLE handle = m_pPlayer->GetHandle();

		if( pSummon )
		{
			if( enable )
			{
				state.SetState( (StructState::StateCode) code, sid, handle, level, duration, remain_time, GetArTime() + remain_fire_time - pInfo->fire_interval * 100, base_damage, state_value, state_string_value, enable );

				pSummon->m_vStateList.push_back( state );
			}
		}
		else
		{
			if( duration != 0 )
			{
				if( enable == 1 )
				{
					state.SetState( (StructState::StateCode) code, sid, handle, level, duration, remain_time, GetArTime() + remain_fire_time - pInfo->fire_interval * 100, base_damage, state_value, state_string_value, enable );

					m_pPlayer->m_vStateList.push_back( state );
				}
				else if( enable == 2 )
				{
					state.SetState( (StructState::StateCode) code, sid, handle, level, duration, remain_time, GetArTime() + remain_fire_time - pInfo->fire_interval * 100, base_damage, state_value, state_string_value, enable );

					m_pPlayer->m_vStateListRemovedByDeath.push_back( state );
				}
			}
		}
	}

	return true;
}

bool DB_Login::readFavorList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_favor_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_favor_list";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		m_pPlayer->SetFavor( pRS->Fields->Item["favor_id"]->Value, pRS->Fields->Item["favor"]->Value, false );
	}

	return true;
}

bool DB_Login::readRandomQuestList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_random_quest_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_random_quest_list";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		int code = pRS->Fields->Item["code"]->Value;

		int nKey[QuestInstance::MAX_RANDOM_VALUE];
		nKey[0] = pRS->Fields->Item["key1"]->Value;
		nKey[1] = pRS->Fields->Item["key2"]->Value;
		nKey[2] = pRS->Fields->Item["key3"]->Value;

		int nValue[QuestInstance::MAX_RANDOM_VALUE];
		nValue[0] = pRS->Fields->Item["value1"]->Value;
		nValue[1] = pRS->Fields->Item["value2"]->Value;
		nValue[2] = pRS->Fields->Item["value3"]->Value;

		int nIsDropped = pRS->Fields->Item["is_dropped"]->Value;

		m_pPlayer->m_QuestManager.AddRandomQuestInfo( code, nKey, nValue, nIsDropped );
	}

	return true;
}

bool DB_Login::readQuestList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_quest_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_quest_list";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	int nStatus[QuestInstance::MAX_STATUS];
	AR_TIME nTimeLimit;
	AR_TIME tCurrent = GetArTime();

	QuestInstance::QUEST_PROGRESS nProgress;

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		int nId = pRS->Fields->Item["id"]->Value;
		QuestBase::QuestCode code = pRS->Fields->Item["code"]->Value;
		int nStartID = pRS->Fields->Item["start_id"]->Value;

		nProgress = static_cast< QuestInstance::QUEST_PROGRESS >( static_cast< int >( pRS->Fields->Item["progress"]->Value ) );

		if( nProgress == QuestInstance::IN_PROGRESS || nProgress == QuestInstance::FINISHABLE || nProgress == QuestInstance::FAIL )
		{
			nStatus[0] = pRS->Fields->Item["status1"]->Value;
			nStatus[1] = pRS->Fields->Item["status2"]->Value;
			nStatus[2] = pRS->Fields->Item["status3"]->Value;
			nStatus[3] = pRS->Fields->Item["status4"]->Value;
			nStatus[4] = pRS->Fields->Item["status5"]->Value;
			nStatus[5] = pRS->Fields->Item["status6"]->Value;
		}
		else
			memset( nStatus, 0, sizeof( nStatus ) );

		switch( StructQuest::GetQuestBase( code ).eTimeLimitType )
		{
		case QuestBase::TIME_LIMIT_TYPE_PERMANENT:
			nTimeLimit = 0;
			break;
		case QuestBase::TIME_LIMIT_TYPE_DECREASE_ON_GAME:
			nTimeLimit = tCurrent + pRS->Fields->Item["remain_time"]->Value.intVal * 100;
			break;
		case QuestBase::TIME_LIMIT_TYPE_DECREASE_ALWAYS:
			nTimeLimit = tCurrent + pRS->Fields->Item["remain_time"]->Value.intVal * 100 - ( logout_duration * 100 * 60 );
			break;
		}

		StructQuest * pQuest = StructQuest::AllocQuest( m_pPlayer, nId, code, nStatus, nTimeLimit, nProgress, nStartID );
		if( !m_pPlayer->m_QuestManager.AddQuest( pQuest ) )
			pQuest->FreeQuest();

		if( pQuest->GetProgress() != QuestInstance::FINISHED )
			m_pPlayer->m_TitleManager.UpdateTitleConditionByQuestStart( pQuest->GetQuestCode() );
		else
		{
			m_pPlayer->m_TitleManager.UpdateTitleConditionByQuestStart( pQuest->GetQuestCode() );
			m_pPlayer->m_TitleManager.UpdateTitleConditionByQuestEnd( pQuest->GetQuestCode() );
		}
	}

	return true;
}

bool DB_Login::readTitleList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_title_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_title_list";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		int nSID = pRS->Fields->Item["sid"]->Value;
		int nCode = pRS->Fields->Item["code"]->Value;
		int nStatus = pRS->Fields->Item["status"]->Value;

		StructTitle * pTitle = StructTitle::AllocTitle( m_pPlayer, nSID, nCode, nStatus );
		m_pPlayer->m_TitleManager.AddTitle( pTitle );
	}

	return true;
}

bool DB_Login::readTitleConditionList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_title_condition_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_title_condition_list";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		int nSID = pRS->Fields->Item["sid"]->Value;
		int nType = pRS->Fields->Item["type"]->Value;
		__int64 nCount = pRS->Fields->Item["count"]->Value;

		StructTitleCondition * pTitleCondition = StructTitleCondition::AllocTitleCondition( nSID, nType, nCount );
		m_pPlayer->m_TitleManager.AddTitleCondition( pTitleCondition );
	}

	return true;
}

bool DB_Login::readMaxQuestId( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_max_quest_id" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_quest_id";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

	m_pPlayer->m_QuestManager.SetMaxQuestID( pRS->Fields->Item["max_quest_id"]->Value );

	return true;
}

bool DB_Login::readQuestCoolTimeList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_quest_cool_time_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_quest_cool_time_list";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	QuestBase::QuestCode code;
	AR_TIME tCurrent = GetArTime();
	QuestInstance::QUEST_PROGRESS nProgress;

	std::vector< int > vExpiredQuestCooltime;

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		code = pRS->Fields->Item["code"]->Value;
		COleDateTime dtCoolTime( pRS->Fields->Item["cool_time"]->Value.date );
		struct tm tmCoolTime;
		tmCoolTime.tm_year = dtCoolTime.GetYear() - 1900;
		tmCoolTime.tm_mon = dtCoolTime.GetMonth() - 1;
		tmCoolTime.tm_mday = dtCoolTime.GetDay();
		tmCoolTime.tm_hour = dtCoolTime.GetHour();
		tmCoolTime.tm_min = dtCoolTime.GetMinute();
		tmCoolTime.tm_sec = dtCoolTime.GetSecond();
		tmCoolTime.tm_isdst = -1;
		time_t tCoolTime = mktime( &tmCoolTime );
		nProgress = static_cast< QuestInstance::QUEST_PROGRESS >( static_cast< int >( pRS->Fields->Item["progress"]->Value ) );

		if( tCurrent < tCoolTime )
		{
			m_pPlayer->m_QuestManager.ReadQuestCoolTime( code, tCoolTime, nProgress );
		}
		else
		{
			vExpiredQuestCooltime.push_back( code );
		}
	}

	for( std::vector< int >::const_iterator it = vExpiredQuestCooltime.begin(); it != vExpiredQuestCooltime.end(); ++it )
	{
		m_pPlayer->DBQuery( new DB_DeleteQuestCoolTime( m_pPlayer, m_pPlayer->GetSID(), *it ) );
	}

	return true;
}

bool DB_Login::readEventAreaEnterCount( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_event_area_enter_count" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_event_area_enter_count";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAYER_SID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		m_pPlayer->SetEventAreaEnterCount( pRS->Fields->Item["event_area_id"]->Value, pRS->Fields->Item["enter_count"]->Value );
	}

	return true;
}

bool DB_Login::readRandomOptionList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_random_option_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_random_option_list";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, sid  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		struct ItemInstance::RANDOM_OPTION RandomOption;
		std::string strBuffer;
		_decimal_variant decBuffer;

		RandomOption.nSID		= pRS->Fields->Item["sid"]->Value;
		RandomOption.nRandomType= pRS->Fields->Item["random_type"]->Value;

		for( int i = 0; i < ItemInstance::MAX_RANDOM_OPTION_NUMBER; ++i )
		{
			XStringUtil::Format( strBuffer, "type_%02d", i + 1 );
			RandomOption.OptionInfo[ i ].nType = pRS->Fields->Item[strBuffer.c_str()]->Value;

			XStringUtil::Format( strBuffer, "value1_%02d", i + 1 );
			*static_cast< DECIMAL * >( &decBuffer ) = pRS->Fields->Item[strBuffer.c_str()]->Value.decVal;
			RandomOption.OptionInfo[ i ].fValue1.set( decBuffer.getMultipleInteger( 10000 ) );

			XStringUtil::Format( strBuffer, "value2_%02d", i + 1 );
			*static_cast< DECIMAL * >( &decBuffer ) = pRS->Fields->Item[strBuffer.c_str()]->Value.decVal;
			RandomOption.OptionInfo[ i ].fValue2.set( decBuffer.getMultipleInteger( 10000 ) );
		}

		vRandomOptionList.push_back( RandomOption );	
	}

	return true;
}

bool DB_Login::_readClientInfo( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login::_readClientInfo : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_client_info" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_client_info";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID()  ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_CLIENT_INFO", adVarChar, adParamOutput, 4096, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_QUICK_SLOT", adVarChar, adParamOutput, 4096, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_CURRENT_KEY", adVarChar, adParamOutput, 4096, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_SAVED_KEY", adVarChar, adParamOutput, 4096, 0 ) );

	_RecordsetPtr pRS = cmd->Execute( NULL, NULL,adCmdStoredProc );
	
	// 혹시나 키 저장 정보 날라갔으면 다시 넣어줌.
	if( cmd->Parameters->Item[ "OUT_CLIENT_INFO" ]->Value.bstrVal == NULL )
	{
		DB_CreateCharacter::setDefaultClientInfo( db, m_pPlayer->GetPlayerUID() );
		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	
	m_pPlayer->m_strClientInfo = static_cast< const char* >( static_cast< _bstr_t >(cmd->Parameters->Item[ "OUT_CLIENT_INFO" ]->Value.bstrVal ) );
	m_pPlayer->m_strQuickSlot = static_cast< const char* >( static_cast< _bstr_t >(cmd->Parameters->Item[ "OUT_QUICK_SLOT" ]->Value.bstrVal ) );
	m_pPlayer->m_strCurrentKey = static_cast< const char* >( static_cast< _bstr_t >(cmd->Parameters->Item[ "OUT_CURRENT_KEY" ]->Value.bstrVal ) );
	m_pPlayer->m_strSavedKey = static_cast< const char* >( static_cast< _bstr_t >(cmd->Parameters->Item[ "OUT_SAVED_KEY" ]->Value.bstrVal ) );		
	
	return true;
}

void DB_Login::_readAccountFlag( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_Login::_readAccountFlag : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_account_flag" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_account_flag";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID()  ) );
	cmd->Parameters->Append( cmd->CreateParameter( "OUT_ACCOUNT_FLAG", adVarChar, adParamOutput, 1000, 0 ) );

	_RecordsetPtr pRS = cmd->Execute( NULL, NULL,adCmdStoredProc );

	// 어카운트 플래그 정보 없다면 패스 (이번 로그아웃 처리 할 때 생성 될 예정)
	if( cmd->Parameters->Item[ "OUT_ACCOUNT_FLAG" ]->Value.bstrVal == NULL )
	{
		return;
	}

	_bstr_t flag_list = cmd->Parameters->Item[ "OUT_ACCOUNT_FLAG" ]->Value.bstrVal;
	char *pBegin = flag_list;
	char *pEnd = NULL;
	while( pEnd = strchr( pBegin, '\n' ) )
	{	
		char *ps = strchr( pBegin, ':' );
		std::string strKey( pBegin, ps );
		std::string strData( ps+1, pEnd );
		m_pPlayer->SetAccountFlag( strKey.c_str(), strData.c_str() );

		pBegin = pEnd + 1;
	}
}

void DB_Login::getRandomOption( int _SID, ItemInstance::RANDOM_OPTION * _pRandomOption )
{
	std::vector< ItemInstance::RANDOM_OPTION >::iterator it = vRandomOptionList.begin();
	for( ; it != vRandomOptionList.end() ; ++it )
	{
		if( _SID == (*it).nSID )
		{
			_pRandomOption->nSID = _SID;
			_pRandomOption->nRandomType = it->nRandomType;

			for( int nCnt = 0 ; nCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++nCnt )
			{
				_pRandomOption->OptionInfo[ nCnt ].nType = (*it).OptionInfo[ nCnt ].nType;
				_pRandomOption->OptionInfo[ nCnt ].fValue1 = (*it).OptionInfo[ nCnt ].fValue1;
				_pRandomOption->OptionInfo[ nCnt ].fValue2 = (*it).OptionInfo[ nCnt ].fValue2;
			}

			vRandomOptionList.erase( it );
			return;
		}
	}
}

bool DB_Login::onProcess( DBConnection & db )
{
	int nSummonHP[6];
	int nSummonMP[6];

	unsigned short nResult = RESULT_UNKNOWN;
	bWasInDeathmatch = false;
	bWasInBattleArena = false;
	bBattleArenaOfflinePenaltyReceived = false;

	do
	{
		if( ( nResult = readCharacterInfo( db ) ) != RESULT_SUCCESS ) break;
		if( GameRule::bUsePlayPoint && !readPlayTime( db ) ) break;
		if( !readRandomOptionList( db ) ) break;
		if( !readItemCoolTime( db ) ) break;
		if( !readItemList( db ) ) break;

		int nPlayerHP = m_pPlayer->GetHP();
		int nPlayerMP = m_pPlayer->GetMP();

		if( !readEventAreaEnterCount( db ) ) break;

		// 기존에 readCharacterInfo만 완료되면 로그인 완료로 처리하였으나 아이템 정보 중 Charm류 아이템이
		// 로드될 때 CalulateStat 함수를 호출하게 되면 최대 HP/MP에 문제가 생기므로 로그인을 여기서 완료로
		// 설정하고, 인벤에 Charm이 추가될 때 스텟 재계산을 하는 건 로그인이 완료된 이후에만 작동
		m_pPlayer->SetLoginComplete();

		// 헌터 의상 착용때문에 여기서 함 해준다.
		m_pPlayer->CalculateStat();

		if( !readSummonList( db ) ) break;
		if( !readFarmedSummonInfoList( db ) ) break;
		if( !readPetList( db ) ) break;
		if( !readRandomQuestList( db ) ) break;
		if( !readQuestList( db ) ) break;
		if( !readQuestCoolTimeList( db ) ) break;
		if( !readTitleList( db ) ) break;
		if( !readTitleConditionList( db ) ) break;
		if( !readMaxQuestId( db ) ) break;
		if( !readStateInfo( db, NULL ) ) break;
		if( !readFavorList( db ) ) break;

		if( !readFriendsList( db ) ) break;
		if( !readFriendOfsList( db ) ) break;
		if( !readDenialsList( db ) ) break;
		if( !readDenialOfsList( db ) ) break;

		
		// 파티 로그인 메세지
		if( m_pPlayer->m_nPartyID != 0 )
		{	
			if( !PartyManager::GetInstance().GetMemberCount( m_pPlayer->m_nPartyID ) )
			{
				m_pPlayer->m_nPartyID = 0;
			}
		}
		// 길드 로그인 메세지
		if( m_pPlayer->IsInGuild() )
		{	
			if( !GuildManager::GetInstance().GetMemberCount( m_pPlayer->m_nGuildId ) )
			{
				m_pPlayer->m_nGuildId = 0;
			}
		}

		// 소환수 bind 적용(카드/소환수 정보 불일치 발생시 m_aBindSummonCard에
		// 적용되는 인덱스를 별도로 사용해야 하므로 nSummonIdx 별도 사용)
		for( int i = 0, nSummonIdx = 0 ; i < 6 ; ++i )
		{
			if( !bindSummon[i] )
				continue;

			StructSummon *pSummon = m_pPlayer->GetSummon( static_cast< int >( bindSummon[i] ) );
			if( !pSummon ) 
			{
				bindSummon[i] = 0;
				FILELOG( "card/summon information not equal!" );
				_cprint( "card/summon information not equal!\n" );
				continue;
			}

			nSummonHP[ nSummonIdx ] = pSummon->GetHP();
			nSummonMP[ nSummonIdx ] = pSummon->GetMP();

			pSummon->SetSummonSlotIndex( nSummonIdx );

			pSummon->CalculateStat();		// 아이템 장착을 하기 전 미리 이녀석을 해 놓아야 최대 장착할 수 있는 아이템 갯수를 얻을 수 있음.

			m_pPlayer->m_aBindSummonCard[ nSummonIdx ] = pSummon->GetParentCard();
			m_pPlayer->UpdateTitleConditionBySummonEquip( pSummon->GetParentCard()->GetSummonCode(), pSummon->GetRate(), pSummon->GetParentCard()->GetItemEnhance(), 1 );

			++nSummonIdx;
		} 

		if( mainSummon && m_pPlayer->GetSummon( mainSummon ) )
		{
			m_pPlayer->m_pMainSummon = m_pPlayer->GetSummon( mainSummon );

			if( subSummon && m_pPlayer->GetSummon( subSummon ) )
			{
				m_pPlayer->m_pSubSummon = m_pPlayer->GetSummon( subSummon );

				m_pPlayer->m_nNextUnSummonTime = GetArTime() + remainSummonTime;
			}
		}

		// 호칭 정보, 획득한 호칭 정보가 읽어온 후에야 등록이 가능하므로 여기서 처리한다.
		m_pPlayer->m_pMainTitle = StructTitleManager::GetTitleBase( main_title );
		for( int i = 0; i < GameRule::SUB_TITLE_COUNT; i++ )
			m_pPlayer->m_pSubTitle[i] = StructTitleManager::GetTitleBase( sub_title[i] );

		m_pPlayer->m_tRemainTitleTime = GetArTime() + remain_title_time;

		// 이름이 바뀌어 있지 않은 펫은 소환된 상태가 되지 못하도록 변경
		// 원인 불명이지만 종종 name_changed == 0 인 펫이 소환된 상태로 로그인되는 경우가 발생함
		StructPet * pPet = ( pet ) ? m_pPlayer->GetPet( pet ) : NULL;
		assert( !pPet || pPet->IsNameChanged() );
		if( pPet && pPet->IsNameChanged() )
		{
			m_pPlayer->m_pSummonedPet = pPet;
		}

		if( !readEquipItemList( db ) ) break;

		// 최대 벨트 슬롯 개수 갱신을 위해 스텟 계산
		m_pPlayer->CalculateStat();

		// { 벨트 슬롯 load
		setBeltSlotInfo();
		// }

		// 마지막 스탯 계산
		m_pPlayer->CalculateStat();

		m_pPlayer->SetHP( nPlayerHP );
		m_pPlayer->SetMP( nPlayerMP );

		for( int i = 0; i < 6; ++i )
		{
			StructSummon *pSummon = m_pPlayer->GetSummonAt( i );

			if( pSummon )
			{
				pSummon->CalculateStat();

				pSummon->SetHP( nSummonHP[i] );
				pSummon->SetMP( nSummonMP[i] );
			}
		}

		// 위에서 readCharacterInfo 함수의 리턴 값이 RESULT_SUCCESS일 때만 여기까지 실행되므로 nResult 를 굳이 RESULT_SUCCESS로 다시 세팅할 필요 없음
		// 만일 다른 로딩 관련 함수들에 대해서도 nResult 값을 변경하는 방식이 적용될 경우에는 필요에 따라 해당 처리를 해줘야 할 수도 있음
		//nResult = RESULT_SUCCESS;
	} while( false );


	if( nResult != RESULT_SUCCESS )
	{
		SendLoginResult( pConnection, m_pPlayer, 0, nResult );

		static_cast< XIOCPConnection* >( pConnection )->DecVar();
		m_pPlayer->LogoutNow( 3 );

		return false;
	}

	int nDungeonID = DungeonManager::Instance().GetDungeonID( m_pPlayer->GetPos().x, m_pPlayer->GetPos().y );
	int nGuildID = m_pPlayer->GetGuildID();

	if( nDungeonID && nGuildID && !m_pPlayer->GetLayer() )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

		DungeonManager::Instance().OnEnterDungeon( nDungeonID, m_pPlayer, m_pPlayer->GetLayer() );
	}

	// 로그아웃과 동시에 퇴장 처리되는 데스매치의 특성상 로그인 시점에 케릭터가 데스매치 안에 있는 경우는 없으므로 데스매치 아이템은 항상 가지고 있어서는 안 된다.
	m_pPlayer->RemoveItemByQuittingDeathmatch();
	if( bWasInDeathmatch )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

		m_pPlayer->RemoveAllStateByQuittingDeathmatch();

		if( m_pPlayer->GetMainSummon() )
			m_pPlayer->GetMainSummon()->RemoveAllStateByQuittingDeathmatch();
		if( m_pPlayer->GetSubSummon() )
			m_pPlayer->GetSubSummon()->RemoveAllStateByQuittingDeathmatch();

		m_pPlayer->RestoreStatesOnLeaveInstanceGame( true );
	}
	// 배틀 아레나에서 로그인하면서 퇴장된 경우 뒷처리
	if( bWasInBattleArena )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

		m_pPlayer->RemoveAllStateByQuittingBattleArena();

		if( m_pPlayer->GetMainSummon() )
			m_pPlayer->GetMainSummon()->RemoveAllStateByQuittingBattleArena();
		if( m_pPlayer->GetSubSummon() )
			m_pPlayer->GetSubSummon()->RemoveAllStateByQuittingBattleArena();
	}

	m_pPlayer->m_nWarpEndTime = GetArTime();
	m_pPlayer->SetInvincible( true );

	AR_TIME t = GetArTime();
	if( GameRule::bAutoOpen )
	{
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

		if( GameRule::bUseAutoJail )
		{
			m_pPlayer->SetAutoUsed();
			m_pPlayer->AddState( StructState::NEMESIS_FOR_AUTO, NULL, 11, t, t + 86400000 );

			m_pPlayer->Save( true );

			FILELOG( "Checked Auto Open: %s\n", m_pPlayer->GetName() );
			StructPlayer::AddToAutoAccountList( m_pPlayer->GetAccountID() );
		}

		LOG::Log11N4S( LM_AUTO_USER_CHECKED, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), m_pPlayer->GetLevel(), m_pPlayer->GetJobLevel(), m_pPlayer->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::AUTO_OPEN_ONLY, 0, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}
	else if( m_pPlayer->IsAutoUsed() || StructPlayer::IsAutoAccount( m_pPlayer->GetAccountID() ) )
	{
		if( GameRule::bUseAutoJail )
		{
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

			m_pPlayer->SetAutoUsed();
			m_pPlayer->AddState( StructState::NEMESIS_FOR_AUTO, NULL, 11, t, t + 86400000 );
			m_pPlayer->Save( true );

			FILELOG( "Checked Auto Account: %s\n", m_pPlayer->GetName() );
		}

		LOG::Log11N4S( LM_AUTO_USER_CHECKED, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), m_pPlayer->GetLevel(), m_pPlayer->GetJobLevel(), m_pPlayer->GetJobId(), 0, 0, 0, 0, AUTO_USER_CHECK_TYPE::AUTO_USER_LOGIN, 0, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}

	// 호칭 조건을 위해 이 부분에서 모든 정보를 읽은 시점을 저장한다.
	// SendLoginResult를 전송하기 전에 모든 호칭과 호칭 상태를 구성해둬야 한다.
	m_pPlayer->SetReadInfoComplete();
	m_pPlayer->m_TitleManager.CheckTitleConditionByLogin();

	SendLoginResult( pConnection, m_pPlayer, m_nPCBangMode, RESULT_SUCCESS );

	m_pPlayer->ChangeLocation( m_pPlayer->GetX(), m_pPlayer->GetY(), false );
	m_pPlayer->SetEventCode( m_nEventCode );
	m_pPlayer->SetAge( m_nAge );

	// _CONNECTION_TAG로부터 nContinuousPlayTime, nContinuousLogoutTime을 StructPlayer의 m_nContinuousPlayTime, m_nContinuousLogoutTime으로 가져 옴
	{
		THREAD_SYNCHRONIZE( g_ConnectionTagLock );

		_CONNECTION_TAG *pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );

		if( pTag )
		{
			// SetContinuousPlayTime에서 스테미너 세이버 관련 지속효과 변화가 있을 경우 지역 방송 발생 함
			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

			m_pPlayer->SetContinuousPlayTime( m_nContinuousPlayTime );
			m_pPlayer->SetContinuousLogoutTime( m_nContinuousLogoutTime );
		}
	}

	// 배틀 아레나 경기에 재입장된 유저라면 재입장에 따라 방송해야 할 내용을 여기서 방송(재접속에 따른 스크립트 실행도 여기서 처리됨)
	if( bBattleArenaReconnectSucceed )
	{
		// OnReconnectPostProc 안에서 스크립트 실행도 있고 패킷 방송도 하니 일단 지역 락 걸고 ㄱㄱ싱
		ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( m_pPlayer ) );

		// 여기선 위에서 OnReconnect 호출한 시점 이후에 경기가 해산되거나 끝났을 가능성도 있으므로 실패해도 상관 없음
		BattleArenaManager::Instance().OnReconnectPostProc( m_pPlayer );
	}
	// 오프라인 상태에서 배틀 아레나 페널티를 받은 유저면 안내 메시지 출력
	else if( bBattleArenaOfflinePenaltyReceived )
	{
		if( m_pPlayer->GetBattleArenaPenaltyCount() > 1 )
			SendChatMessage( false, CHAT_CENTER_NOTICE, "@NOTICE", m_pPlayer, "@2448" );
		else
			SendChatMessage( false, CHAT_CENTER_NOTICE, "@NOTICE", m_pPlayer, "@2447" );
	}

	// 계정 권한관리 DB부터 업데이트 하도록 한다. (캐쉬템 업데이트가 계정 권한과 관련있다면 eg. 시크루트, synchronize 루틴에 의해 같이 반영됨)
	if( GameRule::bUseAccountAuthorityDB )
	{
		m_pPlayer->DBQuery( new DB_ReadAccountAuthorityInfo( m_pPlayer ) );
	}

	// TODO : 캐쉬템 창고에 뭔가 업데된게 있는지 확인 및 로그아웃 기간 동안 스테미너 회복, 로그인 로그
	if( GameRule::bIsCashUsableServer )
	{
		// 크루박스 확인을 하는 서버의 경우 DB_CommercialItemStorage::onProcess에서
		// 크루박스 확인 후에 로그아웃 기간동안 회복되었어야 할 스테미너 회복시킴
		// 로그인 로그도 마찬가지로 DB_CommercialItemStorage::onProcess에서 남김
		m_pPlayer->DBQuery( new DB_GetCommercialStorageInfo( m_pPlayer, true ) );
	}
	else
	{
		// 캐쉬 서버가 아닌 경우이므로 m_pPlayer->IsGaiaMember()는 무조건 false임
		// 그 외에 캐쉬 사용 불가능 서버인데 정식 서버인 경우(정액제 서버 - 홍콩), 텐트 사용자의 경우 스테 회복
		if( ENV().GetInt( "game.ServiceServer" ) || m_pPlayer->IsUsingTent() || m_pPlayer->IsGaiaMember() )
		{
			m_pPlayer->AddStamina( logout_duration * m_pPlayer->GetStaminaRegenRate() );
		}

		// 이벤트 지속효과 부여 처리
		m_pPlayer->ProcEventState();

		LOG::Log11N4S( LM_CHARACTER_ENTER, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), 0, 0, m_pPlayer->GetPCBangMode(), m_pPlayer->GetStamina(), m_pPlayer->GetGold().GetRawData(), m_pPlayer->GetLayer(), m_pPlayer->GetX(), m_pPlayer->GetY(), 0, m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		LOG::Log11N4S( LM_CHARACTER_INFO, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), m_pPlayer->GetLevel(), m_pPlayer->GetJobLevel(), m_pPlayer->GetJobPoint(), m_pPlayer->GetJobId(), m_pPlayer->GetGold().GetRawData(), m_pPlayer->GetStorageGold().GetRawData(), m_pPlayer->GetChaos(), m_pPlayer->GetImmoralPoint(), m_pPlayer->GetEXP(), m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
	}

	ArcadiaServer::Instance().SetObjectPriority( m_pPlayer, ArSchedulerObject::UPDATE_PRIORITY_NORMAL );

	static_cast< XIOCPConnection* >( pConnection )->DecVar();

	return true;
}

void DB_Login::onFail( const _com_error & exception )
{
	SendLoginResult( pConnection, m_pPlayer, 0, RESULT_DB_ERROR );

	static_cast< XIOCPConnection* >( pConnection )->DecVar();
	m_pPlayer->LogoutNow( 3 );
}
