
#include <oledb.h>
#include <icrsint.h>
#include <atlcomtime.h>

#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>

#include "DB_Commands.h"
#include "GameDBUtil.h"
#include "SendMessage.h"
#include "StructItem.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructPlayer.h"
#include "GameContent.h"


char* IN_SUMMON[]
=
{
	"IN_SUMMON_0",
	"IN_SUMMON_1",
	"IN_SUMMON_2",
	"IN_SUMMON_3",
	"IN_SUMMON_4",
	"IN_SUMMON_5",
};

char* IN_BELT[]
=
{
	"IN_BELT_0",
	"IN_BELT_1",
	"IN_BELT_2",
	"IN_BELT_3",
	"IN_BELT_4",
	"IN_BELT_5",
	"IN_BELT_6",
	"IN_BELT_7",
};

char* IN_SUB_TITLE[]
=
{
	"IN_SUB_TITLE_0",
	"IN_SUB_TITLE_1",
	"IN_SUB_TITLE_2",
	"IN_SUB_TITLE_3",
	"IN_SUB_TITLE_4",
};

DB_UpdateCharacter::DB_UpdateCharacter( struct StructPlayer * _pPlayer, bool bOnlyCharacter ) : DBProc( "DB_UpdateCharacter" ), pPlayer( _pPlayer ), m_bOnlyCharacter( bOnlyCharacter )
{
	CharacterInfo.IN_SID = pPlayer->GetPlayerUID();
	CharacterInfo.IN_PARTY_SID = pPlayer->GetPartyID();
	CharacterInfo.IN_GUILD_SID = pPlayer->GetGuildID();
	CharacterInfo.IN_PREV_GUILD_SID = pPlayer->GetPrevGuildID();
	CharacterInfo.IN_X = pPlayer->GetX();
	CharacterInfo.IN_Y = pPlayer->GetY();
	CharacterInfo.IN_Z = pPlayer->GetZ();
	CharacterInfo.IN_LAYER = pPlayer->GetLayer();
	CharacterInfo.IN_LV = pPlayer->GetLevel();
	CharacterInfo.IN_MAX_REACHED_LEVEL = pPlayer->GetMaxReachedLevel();
	CharacterInfo.IN_EXP = pPlayer->GetEXP();
	CharacterInfo.IN_LAST_DECREASED_EXP = pPlayer->GetLastDecreasedEXP();
	CharacterInfo.IN_HP = pPlayer->GetHP();
	CharacterInfo.IN_MP = pPlayer->GetMP();
	CharacterInfo.IN_STAMINA = pPlayer->GetStamina();
	CharacterInfo.IN_JOB = pPlayer->GetJobId();
	CharacterInfo.IN_JOB_DEPTH = 0;
	CharacterInfo.IN_JLV = pPlayer->GetJobLevel();
	CharacterInfo.IN_JP = pPlayer->GetJobPoint();
	CharacterInfo.IN_TOTAL_JP = pPlayer->GetTotalJobPoint();
	CharacterInfo.IN_TALENT_POINT = pPlayer->GetTalentPoint();
	CharacterInfo.IN_JOB_0 = pPlayer->GetPrevJobId( 0 );
	CharacterInfo.IN_JOB_1 = pPlayer->GetPrevJobId( 1 );
	CharacterInfo.IN_JOB_2 = pPlayer->GetPrevJobId( 2 );
	CharacterInfo.IN_JLV_0 = pPlayer->GetPrevJobLevel( 0 );
	CharacterInfo.IN_JLV_1 = pPlayer->GetPrevJobLevel( 1 );
	CharacterInfo.IN_JLV_2 = pPlayer->GetPrevJobLevel( 2 );
	CharacterInfo.IN_IP = pPlayer->GetImmoralPoint();
	CharacterInfo.IN_CHA = pPlayer->GetCharisma();
	CharacterInfo.IN_PKC = pPlayer->GetPKC();
	CharacterInfo.IN_DKC = pPlayer->GetDKC();
	CharacterInfo.IN_HUNTAHOLIC_POINT = pPlayer->GetHuntaholicPoint();
	CharacterInfo.IN_HUNTAHOLIC_ENTER_COUNT = pPlayer->GetHuntaholicEnterableCount();
	CharacterInfo.IN_ETHEREAL_STONE_DURABILITY = pPlayer->GetEtherealStoneDurability();
	CharacterInfo.IN_GOLD = pPlayer->GetGold().GetRawData();
	CharacterInfo.IN_CHAOS = pPlayer->GetChaos();
	CharacterInfo.IN_SKIN_COLOR = pPlayer->GetSkinColor();
	pPlayer->EnumFlag( CharacterInfo.IN_FLAG_LIST );
	
	for( int i = 0; i < 6; ++i )
	{
		int sid = 0;
		StructItem *pCard = pPlayer->GetSummonCardAt( i );
		if( pCard ) sid = pCard->GetSummonSID();
		CharacterInfo.IN_SUMMON[i] = sid;
	}

	for( int i = 0; i < 8; ++i )
	{
		__int64 sid = 0;

		StructItem *pCard = pPlayer->GetBeltSlotCardAt( i );

		if( pCard ) sid = pCard->GetItemUID();
		CharacterInfo.IN_BELT[i] = sid;
	}

	CharacterInfo.IN_MAIN_SUMMON = 0;
	CharacterInfo.IN_SUB_SUMMON = 0;
	CharacterInfo.IN_REMAIN_SUMMON_TIME = 0;

	CharacterInfo.IN_MAIN_TITLE = pPlayer->GetMainTitle() ? pPlayer->GetMainTitle()->nID : 0;

	memset( CharacterInfo.IN_SUB_TITLE, 0, sizeof( CharacterInfo.IN_SUB_TITLE ) );
	for( int i = 0; i < 5; i++ )
	{
		const TitleBaseServer * pTitle = pPlayer->GetSubTitle( i );

		if( !pTitle )
			continue;

		CharacterInfo.IN_SUB_TITLE[i] = pPlayer->GetSubTitle( i )->nID;
	}

	AR_TIME t = GetArTime();
	CharacterInfo.IN_REMAIN_TITLE_TIME = pPlayer->GetRemainTitleTime() > t ? pPlayer->GetRemainTitleTime() - t : 0;

	CharacterInfo.IN_ARENA_POINT = pPlayer->GetBattleArenaPoint();
	CharacterInfo.IN_ARENA_MVP_COUNT = pPlayer->GetBattleArenaMVPCount();
	CharacterInfo.IN_ARENA_RECORD[ 0 ][ 0 ] = pPlayer->GetBattleArenaRecord( BAT_CLASSIC, true );
	CharacterInfo.IN_ARENA_RECORD[ 0 ][ 1 ] = pPlayer->GetBattleArenaRecord( BAT_CLASSIC, false );
	CharacterInfo.IN_ARENA_RECORD[ 1 ][ 0 ] = pPlayer->GetBattleArenaRecord( BAT_BINGO, true );
	CharacterInfo.IN_ARENA_RECORD[ 1 ][ 1 ] = pPlayer->GetBattleArenaRecord( BAT_BINGO, false );
	CharacterInfo.IN_ARENA_RECORD[ 2 ][ 0 ] = pPlayer->GetBattleArenaRecord( BAT_SLAUGHTER, true );
	CharacterInfo.IN_ARENA_RECORD[ 2 ][ 1 ] = pPlayer->GetBattleArenaRecord( BAT_SLAUGHTER, false );

	if( pPlayer->GetMainSummon() )
	{
		CharacterInfo.IN_MAIN_SUMMON = pPlayer->GetMainSummon()->GetSID();

		if( pPlayer->GetSubSummon() )
		{
			CharacterInfo.IN_SUB_SUMMON = pPlayer->GetSubSummon()->GetSID();
			CharacterInfo.IN_REMAIN_SUMMON_TIME = pPlayer->GetNextUnSummonTime() - GetArTime();
		}
	}

	CharacterInfo.IN_PET = 0;

	if( pPlayer->GetSummonedPet() )
	{
		CharacterInfo.IN_PET = pPlayer->GetSummonedPet()->GetSID();
	}

	CharacterInfo.IN_REMAIN_CHAT_BLOCK_TIME = pPlayer->GetChatBlockTime();
	CharacterInfo.IN_ADV_CHAT_COUNT = pPlayer->GetAdvChatCount();
	CharacterInfo.IN_AUTO_USED = pPlayer->IsAutoUsed();
	CharacterInfo.IN_PKMODE = pPlayer->IsPKOn();
	CharacterInfo.IN_PLAY_TIME_POINT = pPlayer->m_nTotalPlayTime;
	CharacterInfo.IN_RX = pPlayer->m_nRX;
	CharacterInfo.IN_RY = pPlayer->m_nRY;
	CharacterInfo.IN_HX = pPlayer->m_nHX;
	CharacterInfo.IN_HY = pPlayer->m_nHY;


	if( !m_bOnlyCharacter )
	{
		std::vector< FAVOR_INFO >::iterator it;

		for( it = pPlayer->m_vFavorInfo.begin() ; it != pPlayer->m_vFavorInfo.end(); ++it )
		{
			if( !((*it).bNeedToUpdate) )
				continue;

			(*it).bNeedToUpdate = false;

			vFavorInfo.push_back( (*it) );
		}
	}

	// 어카운트 플래그 정보 전달
	pPlayer->EnumAccountFlag( m_strAccountFlagList );
	m_nAccountID = pPlayer->GetAccountID();
}

bool DB_UpdateCharacter::updateFavorInfo( DBConnection & db )
{
	if( vFavorInfo.size() == 0 )
		return true;

	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateCharacter : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_favor" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_favor";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, pPlayer->GetPlayerUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_FAVOR_ID", adInteger, adParamInput, 4, 0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_FAVOR", adInteger, adParamInput, 4, 0 ) );

	std::vector< FAVOR_INFO >::iterator it;

	for( it = vFavorInfo.begin() ; it != vFavorInfo.end(); ++it )
	{
		cmd->Parameters->Item[ "IN_FAVOR_ID" ]->Value = (*it).favor_id;
		cmd->Parameters->Item[ "IN_FAVOR" ]->Value = (*it).favor_id;

		cmd->Execute(NULL,NULL,adCmdStoredProc);
	}
	
	return true;
}

bool DB_UpdateCharacter::updateItemCoolTime( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateCharacter : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_item_cool_time" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_item_cool_time";

	AR_TIME t = GetArTime();

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, pPlayer->GetPlayerUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_00", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime(  1 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_01", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime(  2 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_02", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime(  3 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_03", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime(  4 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_04", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime(  5 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_05", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime(  6 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_06", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime(  7 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_07", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime(  8 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_08", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime(  9 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_09", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 10 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_10", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 11 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_11", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 12 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_12", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 13 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_13", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 14 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_14", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 15 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_15", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 16 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_16", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 17 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_17", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 18 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_18", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 19 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_19", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 20 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_20", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 21 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_21", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 22 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_22", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 23 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_23", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 24 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_24", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 25 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_25", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 26 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_26", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 27 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_27", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 28 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_28", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 29 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_29", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 30 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_30", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 31 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_31", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 32 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_32", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 33 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_33", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 34 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_34", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 35 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_35", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 36 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_36", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 37 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_37", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 38 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_38", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 39 ) - (int) t, (int) 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "COOL_TIME_39", adInteger, adParamInput, 4, std::max( (int) pPlayer->GetItemCoolTime( 40 ) - (int) t, (int) 0 ) ) );

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_UpdateCharacter::updateCharacterInfo( DBConnection & db )
{	
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateCharacter : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_character" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_character";

	// cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adBigInt, adParamInput, 8, pItem->GetItemUID() ));

#ifdef _USE_UPDATE_CHARACTER_CHECKSUM
	// smp_update_character checksum 값 계산 추가(IN_Z, IN_HAVOC에 각각 상/하위 4바이트씩 전달)
	__int64 nChecksum = CharacterInfo.IN_GOLD + CharacterInfo.IN_HP + CharacterInfo.IN_STAMINA - CharacterInfo.IN_EXP - ( ( CharacterInfo.IN_Y ) ? CharacterInfo.IN_Y : 2 );
	nChecksum |= 1880095387259708456;
#endif

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, CharacterInfo.IN_SID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PARTY_SID", adInteger, adParamInput, 4, CharacterInfo.IN_PARTY_SID ) );
	//cmd->Parameters->Append( cmd->CreateParameter( "IN_GUILD_SID", adInteger, adParamInput, 4, CharacterInfo.IN_GUILD_SID ) );
	//cmd->Parameters->Append( cmd->CreateParameter( "IN_PREV_GUILD_SID", adInteger, adParamInput, 4, CharacterInfo.IN_PREV_GUILD_SID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_X", adInteger, adParamInput, 4, CharacterInfo.IN_X ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_Y", adInteger, adParamInput, 4, CharacterInfo.IN_Y ) );
#ifdef _USE_UPDATE_CHARACTER_CHECKSUM
	// shift-right 연산자는 signed 타입의 음수 값에 대해서는 최상위 비트 패딩을 1로 넣어주므로 문제 발생함
	cmd->Parameters->Append( cmd->CreateParameter( "IN_Z", adInteger, adParamInput, 4, static_cast< int >( static_cast< unsigned __int64 >( nChecksum ) >> 32 ) ) );
#else
	cmd->Parameters->Append( cmd->CreateParameter( "IN_Z", adInteger, adParamInput, 4, CharacterInfo.IN_Z ) );
#endif
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LAYER", adInteger, adParamInput, 4, CharacterInfo.IN_LAYER ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LV", adInteger, adParamInput, 4, CharacterInfo.IN_LV ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_MAX_REACHED_LEVEL", adInteger, adParamInput, 4, CharacterInfo.IN_MAX_REACHED_LEVEL ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_EXP", adBigInt, adParamInput, 8, CharacterInfo.IN_EXP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LAST_DECREASED_EXP", adBigInt, adParamInput, 8, CharacterInfo.IN_LAST_DECREASED_EXP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_HP", adInteger, adParamInput, 4, CharacterInfo.IN_HP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_MP", adInteger, adParamInput, 4, CharacterInfo.IN_MP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_STAMINA", adInteger, adParamInput, 4, CharacterInfo.IN_STAMINA ) );
#ifdef _USE_UPDATE_CHARACTER_CHECKSUM
	cmd->Parameters->Append( cmd->CreateParameter( "IN_HAVOC", adInteger, adParamInput, 4, static_cast< int >( nChecksum & 0xFFFFFFFF ) ) );
#else
	cmd->Parameters->Append( cmd->CreateParameter( "IN_HAVOC", adInteger, adParamInput, 4, 0 ) );
#endif
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JOB", adInteger, adParamInput, 4, CharacterInfo.IN_JOB ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JOB_DEPTH", adInteger, adParamInput, 4, CharacterInfo.IN_JOB_DEPTH ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JLV", adInteger, adParamInput, 4, CharacterInfo.IN_JLV ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JP", adBigInt, adParamInput, 8, CharacterInfo.IN_JP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_TOTAL_JP", adBigInt, adParamInput, 8, CharacterInfo.IN_TOTAL_JP ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_TALENT_POINT", adInteger, adParamInput, 4, CharacterInfo.IN_TALENT_POINT ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JOB_0", adInteger, adParamInput, 4, CharacterInfo.IN_JOB_0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JOB_1", adInteger, adParamInput, 4, CharacterInfo.IN_JOB_1 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JOB_2", adInteger, adParamInput, 4, CharacterInfo.IN_JOB_2 ) );		
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JLV_0", adInteger, adParamInput, 4, CharacterInfo.IN_JLV_0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JLV_1", adInteger, adParamInput, 4, CharacterInfo.IN_JLV_1 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_JLV_2", adInteger, adParamInput, 4, CharacterInfo.IN_JLV_2 ) );

	_decimal_variant decImmoral;
	decImmoral.setMultipleInteger( CharacterInfo.IN_IP.get(), 10000 );
	_ParameterPtr paramPtr = cmd->CreateParameter( "IN_IP", adDecimal, adParamInput, sizeof( DECIMAL ), decImmoral );
	paramPtr->NumericScale = 4;
	paramPtr->Precision = 18;
	cmd->Parameters->Append( paramPtr );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_CHA", adInteger, adParamInput, 4, CharacterInfo.IN_CHA ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PKC", adInteger, adParamInput, 4, CharacterInfo.IN_PKC ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_DKC", adInteger, adParamInput, 4, CharacterInfo.IN_DKC ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_HUNTAHOLIC_POINT", adInteger, adParamInput, 4, CharacterInfo.IN_HUNTAHOLIC_POINT ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_HUNTAHOLIC_ENTER_COUNT", adInteger, adParamInput, 4, CharacterInfo.IN_HUNTAHOLIC_ENTER_COUNT ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ETHEREAL_STONE_DURABILITY", adInteger, adParamInput, 4, CharacterInfo.IN_ETHEREAL_STONE_DURABILITY ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_GOLD", adBigInt, adParamInput, 8, CharacterInfo.IN_GOLD ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CHAOS", adInteger, adParamInput, 4, CharacterInfo.IN_CHAOS ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SKIN_COLOR", adInteger, adParamInput, 4, CharacterInfo.IN_SKIN_COLOR ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_FLAG_LIST", adVarChar, adParamInput, 1000, CharacterInfo.IN_FLAG_LIST.c_str() ) );
	for( int i = 0; i < 6; ++i )
	{
		cmd->Parameters->Append( cmd->CreateParameter( IN_SUMMON[i], adInteger, adParamInput, 4, CharacterInfo.IN_SUMMON[i] ) );
	}
	for( int i = 0; i < 8; ++i )
	{
		cmd->Parameters->Append( cmd->CreateParameter( IN_BELT[i], adBigInt, adParamInput, 8, CharacterInfo.IN_BELT[i] ) );
	}
	cmd->Parameters->Append( cmd->CreateParameter( "IN_MAIN_SUMMON", adInteger, adParamInput, 4, CharacterInfo.IN_MAIN_SUMMON ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SUB_SUMMON", adInteger, adParamInput, 4, CharacterInfo.IN_SUB_SUMMON ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_SUMMON_TIME", adInteger, adParamInput, 4, CharacterInfo.IN_REMAIN_SUMMON_TIME ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PET", adInteger, adParamInput, 4, CharacterInfo.IN_PET ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_MAIN_TITLE", adInteger, adParamInput, 4, CharacterInfo.IN_MAIN_TITLE ) );
	for( int i = 0; i < 5; i++ )
	{
		cmd->Parameters->Append( cmd->CreateParameter( IN_SUB_TITLE[i], adInteger, adParamInput, 4, CharacterInfo.IN_SUB_TITLE[i] ) );
	}
	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TITLE_TIME", adInteger, adParamInput, 4, CharacterInfo.IN_REMAIN_TITLE_TIME ) );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ARENA_POINT", adInteger, adParamInput, 4, CharacterInfo.IN_ARENA_POINT ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ARENA_MVP_COUNT", adInteger, adParamInput, 4, CharacterInfo.IN_ARENA_MVP_COUNT ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ARENA_RECORD_0_0", adInteger, adParamInput, 4, CharacterInfo.IN_ARENA_RECORD[ 0 ][ 0 ] ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ARENA_RECORD_0_1", adInteger, adParamInput, 4, CharacterInfo.IN_ARENA_RECORD[ 0 ][ 1 ] ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ARENA_RECORD_1_0", adInteger, adParamInput, 4, CharacterInfo.IN_ARENA_RECORD[ 1 ][ 0 ] ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ARENA_RECORD_1_1", adInteger, adParamInput, 4, CharacterInfo.IN_ARENA_RECORD[ 1 ][ 1 ] ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ARENA_RECORD_2_0", adInteger, adParamInput, 4, CharacterInfo.IN_ARENA_RECORD[ 2 ][ 0 ] ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ARENA_RECORD_2_1", adInteger, adParamInput, 4, CharacterInfo.IN_ARENA_RECORD[ 2 ][ 1 ] ) );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_CHAT_BLOCK_TIME", adInteger, adParamInput, 4, CharacterInfo.IN_REMAIN_CHAT_BLOCK_TIME ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ADV_CHAT_COUNT", adInteger, adParamInput, 4, CharacterInfo.IN_ADV_CHAT_COUNT ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_AUTO_USED", adInteger, adParamInput, 4, CharacterInfo.IN_AUTO_USED ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PKMODE", adTinyInt, adParamInput, 1, CharacterInfo.IN_PKMODE ) );
	cmd->Parameters->Append(cmd->CreateParameter("IN_PLAY_TIME_POINT", adInteger, adParamInput, 1, CharacterInfo.IN_PLAY_TIME_POINT));
	cmd->Parameters->Append(cmd->CreateParameter("IN_RX", adInteger, adParamInput, 1, CharacterInfo.IN_RX));
	cmd->Parameters->Append(cmd->CreateParameter("IN_RY", adInteger, adParamInput, 1, CharacterInfo.IN_RY));
	cmd->Parameters->Append(cmd->CreateParameter("IN_HX", adInteger, adParamInput, 1, CharacterInfo.IN_HX));
	cmd->Parameters->Append(cmd->CreateParameter("IN_HY", adInteger, adParamInput, 1, CharacterInfo.IN_HY));

	cmd->Execute(NULL,NULL,adCmdStoredProc);

	return true;
}

bool DB_UpdateCharacter::updateAccountFlagList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateCharacter::updateAccountFlagList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_set_account_flag" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_set_account_flag";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_SID", adInteger, adParamInput, 4, m_nAccountID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_FLAG", adVarChar, adParamInput, 1000, m_strAccountFlagList.c_str() ) );
	cmd->Execute( NULL, NULL, adCmdStoredProc );

	return true;
}

bool DB_UpdateCharacter::onProcess( DBConnection & db )
{
	try
	{
		updateCharacterInfo( db );
		updateAccountFlagList( db );
		
		if( !m_bOnlyCharacter ) 
		{
			updateItemCoolTime( db );
			updateFavorInfo( db );
		}
		
		pPlayer->onEndQuery();
	}
	catch( ... )
	{
		pPlayer->onEndQuery();
		throw;
	}

	return true;
}

bool DB_UpdateCharacterHair::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateCharacterHair : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_character_hair" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_character_hair";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HAIR_ID", adInteger, adParamInput, 4, m_pPlayer->GetBaseModelId( 0 ) ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HAIR_COLOR_INDEX", adInteger, adParamInput, 4, m_pPlayer->GetHairColorIndex() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HAIR_COLOR_RGB", adInteger, adParamInput, 4, m_pPlayer->GetHairColorRGB() ) );

		_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();
		throw;
	}

	return true;
}

bool DB_UpdateCharacterSkin::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateCharacterSkin : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_character_skin" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_character_skin";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SKIN_ID", adInteger, adParamInput, 4, m_pPlayer->GetSkinColor() ) );

		_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();
		throw;
	}

	return true;
}

bool DB_UpdateCharacterSex::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateCharacterSex : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_character_sex" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_character_sex";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SEX", adInteger, adParamInput, 4, m_pPlayer->GetSex() ) );

		_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();
		throw;
	}

	return true;
}

bool DB_UpdateCharacterHideEquipFlag::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateCharacterHideEquipFlag : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_character_hide_equip_flag" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_character_hide_equip_flag";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_HIDE_EQUIP_FLAG", adInteger, adParamInput, 4, m_pPlayer->GetHideEquipFlag() ) );

		_RecordsetPtr pRS = cmd->Execute( NULL, NULL, adCmdStoredProc );

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();
		throw;
	}

	return true;
}

UpdateOTPCF_PendMessage::UpdateOTPCF_PendMessage( StructPlayer * pPlayer, const TS_MESSAGE * pMsg )
	: m_pPlayer( pPlayer )
{
	m_pMsg = reinterpret_cast< TS_MESSAGE* >(new char[ pMsg->size ]);
	s_memcpy( m_pMsg, pMsg->size, pMsg, pMsg->size );
}

UpdateOTPCF_PendMessage::~UpdateOTPCF_PendMessage()
{
	delete[] (char *)m_pMsg;
}

void UpdateOTPCF_PendMessage::onComplete( const bool bSuccess )
{
	if( bSuccess )
		PendMessage( m_pPlayer, m_pMsg );
	else
	{
		std::string strMessage( GameContent::GetString( 103 ) );
		strMessage += "(8)";
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, strMessage.c_str() );
	}
}

UpdateOTPCF_SendOpenURL::UpdateOTPCF_SendOpenURL( StructPlayer * pPlayer, const char * pszURL, const bool bWaitForEventScene, const int nWidth, const int nHeight )
	: m_pPlayer( pPlayer )
	, m_strURL( pszURL )
	, m_bWaitForEventScene( bWaitForEventScene )
	, m_nWidth( nWidth )
	, m_nHeight( nHeight )
{}

void UpdateOTPCF_SendOpenURL::onComplete( const bool bSuccess )
{
	if( bSuccess )
		SendOpenUrl( m_pPlayer, m_strURL.c_str(), m_bWaitForEventScene, m_nWidth, m_nHeight );
	else
	{
		std::string strMessage( GameContent::GetString( 103 ) );
		strMessage += "(9)";
		SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, strMessage.c_str() );
	}
}

bool DB_UpdateOneTimePassword::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateOneTimePassword : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_one_time_password" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_one_time_password";

		std::string strServer = ENV().GetString( "app.name", "Game001" );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SERVER", adVarChar, adParamInput, (ADO_LONGPTR)strServer.length(), strServer.c_str() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, m_nOwnerId ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ONE_TIME_PASSWORD", adInteger, adParamInput, 4, m_nPassWord ) );

		_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

	}
	catch( ... )
	{
		m_pFo->onComplete( false );
		throw;
	}

	m_pFo->onComplete( true );
	return true;
}

void DB_UpdateOneTimePassword::onEnd( bool bIsCancel )
{
	m_pFo->onEnd();
	delete this;
}

bool DB_UpdatePlayTimePoint::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdatePlayTimePoint : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_update_play_time_point" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_update_play_time_point";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_PLAY_TIME", adInteger, adParamInput, 4, m_nPlayTimeInMin ) );
		cmd->Parameters->Append( cmd->CreateParameter( "IN_POINT", adInteger, adParamInput, 4, m_nPoint ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	m_pPlayer->onEndQuery();

	return true;
}


bool DB_UpdateTimeBasedEvent::onProcess( DBConnection & db )
{
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateTimeBasedEvent : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.usp_rz_web_event_exec" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.usp_rz_web_event_exec";

		BYTE byPCBangMode = 1;
		if( m_pPlayer->GetPCBangMode() == GameRule::PCBANG_PREMIUM_BONUS )
		{
			byPCBangMode = 2;
		}

		cmd->Parameters->Append( cmd->CreateParameter( "nSID", adInteger, adParamInput, 4, m_pPlayer->GetPlayerUID() ) );
		cmd->Parameters->Append( cmd->CreateParameter( "tnPCBang", adTinyInt, adParamInput, 1, byPCBangMode ) );

		cmd->Execute( NULL, NULL, adCmdStoredProc );
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	m_pPlayer->onEndQuery();

	return true;
}
