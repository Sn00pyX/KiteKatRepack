
#include <oledb.h>
#include <icrsint.h>
#include <atlcomtime.h>

#include <network/XIOCPConnection.h>
#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"
#include "GameMessage.h"
#include "GameContent.h"
#include "SendMessage.h"

bool DB_CharacterList::proc( DBConnection & db )
{	
	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_CharacterList : CreateInstance(command) error" );

		_CommandPtr cmdWearInfo;
		if( db.CreateCommand( cmdWearInfo ) == false )	throw XException( "DB_CharacterList : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_read_character_list" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_character_list";

		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, sizeof(nAccountID), nAccountID ) );
		s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_ACCOUNT_ID: %d", nAccountID );

		_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

		unsigned count = 0;
		LOBBY_CHARACTER_INFO	aInfo[10];

		memset( aInfo, 0, sizeof( aInfo ) );

		_bstr_t bstrCharacterName;

		cmdWearInfo->CommandType = adCmdStoredProc;
		cmdWearInfo->CommandText = _bstr_t( "dbo.smp_read_item_equip_info" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_item_equip_info";

		cmdWearInfo->Parameters->Append( cmd->CreateParameter( "IN_SID", adInteger, adParamInput, 4, 0 ) );

		unsigned short last_login_index = 0;
		DATE last_login_time = 0;

		while( pRS->State != adStateClosed && !pRS->EndOfFile && count < 10 )
		{	
			int owner_id = pRS->Fields->Item["sid"]->Value;
			bstrCharacterName = pRS->Fields->Item["name"]->Value;
			// _cprint( "Ä³¸¯ÅÍ : (%s)\n", static_cast< const char* >( bstrCharacterName ) );

			memset( aInfo[count].name, 0, sizeof(aInfo[count].name) );
			s_strcpy( aInfo[count].name, _countof( aInfo[count].name ), static_cast< const char* >( bstrCharacterName ) );
			aInfo[count].race = pRS->Fields->Item["race"]->Value;
			aInfo[count].skin_color = pRS->Fields->Item["skin_color"]->Value;
			aInfo[count].model_id[0] = pRS->Fields->Item["model_00"]->Value;
			aInfo[count].model_id[1] = pRS->Fields->Item["model_01"]->Value;
			aInfo[count].model_id[2] = pRS->Fields->Item["model_02"]->Value;
			aInfo[count].model_id[3] = pRS->Fields->Item["model_03"]->Value;
			aInfo[count].model_id[4] = pRS->Fields->Item["model_04"]->Value;
			aInfo[count].hair_color_index = pRS->Fields->Item["hair_color_index"]->Value;
			aInfo[count].hair_color_rgb = pRS->Fields->Item["hair_color_rgb"]->Value.uintVal;
			aInfo[count].hide_equip_flag = pRS->Fields->Item["hide_equip_flag"]->Value.uintVal;
			aInfo[count].texture_id = pRS->Fields->Item["texture_id"]->Value;

			aInfo[count].level = pRS->Fields->Item["lv"]->Value;
			__int64 exp = pRS->Fields->Item["exp"]->Value;
			aInfo[count].exp_percentage = exp / GameContent::GetNeedExp( aInfo[count].level ) * 100.0f;
			aInfo[count].job_level = pRS->Fields->Item["jlv"]->Value;

#ifndef USE_CLIENT97
			DateToString( pRS->Fields->Item["create_time"]->Value.date, aInfo[count].szCreateTime, _countof( aInfo[count].szCreateTime ) );
			DateToString( pRS->Fields->Item["delete_time"]->Value.date, aInfo[count].szDeleteTime, _countof( aInfo[count].szDeleteTime ) );
#endif
			aInfo[count].sex = pRS->Fields->Item["sex"]->Value;
			aInfo[count].job = pRS->Fields->Item["job"]->Value;

			aInfo[count].is_banned = false;
			aInfo[count].permission = pRS->Fields->Item["permission"]->Value;

			cmdWearInfo->Parameters->Item["IN_SID"]->Value = owner_id;
			s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_SID: %d / [dbo.smp_read_character_list]@IN_ACCOUNT_ID: %d", owner_id, nAccountID );

			_RecordsetPtr pWearRS = cmdWearInfo->Execute(NULL,NULL,adCmdStoredProc);

			while( pWearRS->State != adStateClosed && !pWearRS->EndOfFile )
			{
				int wear_pos = pWearRS->Fields->Item["wear_info"]->Value;

#ifdef USE_CLIENT972
				if (wear_pos >= 0 && wear_pos < 10)
				{
					int nCode = pWearRS->Fields->Item["code"]->Value;
					aInfo[count].wear_info[wear_pos] = nCode;
					int nClass = StructItem::GetItemBase(nCode).nClass;
					aInfo[count].wear_type[wear_pos] = nClass;
				}

				if (wear_pos >= 0 && wear_pos < ItemBase::MAX_ITEM_WEAR_LOGIN)
				{
					//aInfo[count].wear_info[wear_pos] = pWearRS->Fields->Item["code"]->Value;
//					aInfo[count].wear_item_enhance_info[wear_pos] = pWearRS->Fields->Item["enhance"]->Value;
//					aInfo[count].wear_item_level_info[wear_pos] = pWearRS->Fields->Item["level"]->Value;
//					aInfo[count].wear_item_elemental_type[wear_pos] = pWearRS->Fields->Item["elemental_effect_type"]->Value.cVal;
//					aInfo[count].wear_appearance_code[wear_pos] = pWearRS->Fields->Item["appearance_code"]->Value;
				}

#else
				if (wear_pos >= 0 && wear_pos < ItemBase::MAX_ITEM_WEAR_LOGIN)
				{
					aInfo[count].wear_info[wear_pos] = pWearRS->Fields->Item["code"]->Value;
					aInfo[count].wear_item_enhance_info[wear_pos] = pWearRS->Fields->Item["enhance"]->Value;
					aInfo[count].wear_item_level_info[wear_pos] = pWearRS->Fields->Item["level"]->Value;
					aInfo[count].wear_item_elemental_type[wear_pos] = pWearRS->Fields->Item["elemental_effect_type"]->Value.cVal;
					aInfo[count].wear_appearance_code[wear_pos] = pWearRS->Fields->Item["appearance_code"]->Value;
				}
#endif


				pWearRS->MoveNext();
			}

			DATE login_time = pRS->Fields->Item["login_time"]->Value.date;
			if( last_login_time < login_time )
			{
				last_login_time = login_time;
				last_login_index = count;
			}

			++count;

			pRS->MoveNext();
		}

		SendCharacterList( pConnection, aInfo, count, last_login_index );
	}
	catch(_com_error& pComError)
	{
		char ErrorString[256];
		sprintf_s(ErrorString, sizeof(ErrorString), "Code = %08lx, Message = %s, Source = %s, Description = %s",\
			pComError.Error(), pComError.ErrorMessage(), (LPCSTR)pComError.Source(), (LPCSTR)pComError.Description());
		FILELOG("%s\n", ErrorString);
		_cprint("%s\n", ErrorString);
	}
	return true;
}

bool DB_CharacterList::onProcess( DBConnection & db )
{
	proc( db );

	static_cast< XIOCPConnection* >( pConnection )->DecVar();

	return true;
}

const std::string DB_CharacterList::getDebugInfo()
{
	char szDebugInfo[ 30 ];

	s_sprintf( szDebugInfo, _countof( szDebugInfo ), "@IN_ACCOUNT_ID: %d", nAccountID );

	return szDebugInfo;
}

void DB_CharacterList::onFail( const _com_error & exception )
{
	SendResult( pConnection, TM_CS_CHARACTER_LIST, RESULT_DB_ERROR );
	static_cast< XIOCPConnection* >( pConnection )->DecVar();
}
