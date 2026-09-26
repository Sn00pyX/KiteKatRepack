
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>
#include <toolkit/XEnv.h>

#include "DB_Commands.h"
#include "SendMessage.h"
#include "StructPlayer.h"
#include "RandomManager.h"


bool DB_OnStartUp::callStartupHandler( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "on_server_startup" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "on_server_startup";
	db.command->Parameters->Refresh();
	
	db.command->Execute( NULL,NULL,adCmdStoredProc );

	return true;
}

bool DB_OnStartUp::connectToBillingServer( DBConnection & db )
{
	// 권한이 없어서 생략 -_-
	return true;

	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ConnectToBillingServer: CreateInstance(command) error" );

	_cprint( "B1\n" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_connect_billing_server" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_connect_billing_server";

	_cprint( "B2\n" );

	XEnvStruct temp_loader;
	temp_loader.LoadFromFile( "GameServer.eop", "*", true );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_NAME", adVarChar, adParamInput, 31, _bstr_t( "Billing" ) ) );
	_cprint( "!\n" );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PROVSTR", adVarChar, adParamInput, 61, ENV().GetString( "db.billing.connection_string", "" ).c_str() ) );
	_cprint( "!\n" );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SERVER", adVarChar, adParamInput, 31, ENV().GetString( "db.billing.server", "" ).c_str() ) );
	_cprint( "!\n" );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT", adVarChar, adParamInput, 31, temp_loader.GetString( "db.billing.account", "" ).c_str() ) );
	_cprint( "!\n" );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PASSWORD", adVarChar, adParamInput, 61, temp_loader.GetString( "db.billing._password", "" ).c_str() ) );
	_cprint( "!\n" );

	_cprint( "B3\n" );

	_variant_t vNull;
	vNull.vt=VT_ERROR;
	vNull.scode=DISP_E_PARAMNOTFOUND;

	cmd->Execute(&vNull,&vNull,adCmdStoredProc);

	_cprint( "B4\n" );

	/*
	if( cmd->Parameters->Item["RETURN_VALUE"]->Value.lVal == 1 )
	{	
		_cprint( "Billing DB connected" );
	}
	else
	{
		_cprint( "DB ERROR : Billing DB connection failed!" );
	}
	*/

	return true;
}

bool DB_OnStartUp::readMaxItemIndex( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_max_item_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_item_sid";
	
	db.command->Parameters->Refresh();
	
	_RecordsetPtr rs = db.command->Execute(NULL,NULL,adCmdText);

	if( rs->Fields->GetItem ("max_item_sid")->Value.vt == VT_NULL )
	{
		StructPlayer::SetMaxItemUID( 1 );
	}
	else
	{
		StructPlayer::SetMaxItemUID( rs->Fields->GetItem ("max_item_sid")->Value.llVal + 1 );
	}

	return true;
}

bool DB_OnStartUp::readMaxSkillIndex( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_max_skill_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_skill_sid";
	db.command->Parameters->Refresh();
	
	_RecordsetPtr rs = db.command->Execute(NULL,NULL,adCmdText);

	if( rs->Fields->GetItem ("max_skill_sid")->Value.vt == VT_NULL )
	{
		StructCreature::SetMaxSkillUID( 0 );
	}
	else
	{
		StructCreature::SetMaxSkillUID( rs->Fields->GetItem ("max_skill_sid")->Value.lVal );
	}

	return true;
}

bool DB_OnStartUp::readMaxStateIndex( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_max_state_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_state_sid";
	db.command->Parameters->Refresh();

	_RecordsetPtr rs = db.command->Execute(NULL,NULL,adCmdText);

	if( rs->Fields->GetItem ("max_state_sid")->Value.vt == VT_NULL )
	{
		StructCreature::SetMaxStateUID( 0 );
	}
	else
	{
		StructCreature::SetMaxStateUID( rs->Fields->GetItem ("max_state_sid")->Value.lVal );
	}

	return true;
}

bool DB_OnStartUp::readDisabledStateList( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ReadDisabledStateList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_disabled_state_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_disabled_state_list";

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	std::vector< int > vDisabledStateList;

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		int nUID = pRS->Fields->Item["sid"]->Value;
		vDisabledStateList.push_back( nUID );
	}

	StructCreature::SetDisabledStateList( vDisabledStateList );

	return true;
}

bool DB_OnStartUp::removeDeletedCharacter( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_update_delete_character_name" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_delete_character_name";
	db.command->Parameters->Refresh();
	
	db.command->Execute(NULL,NULL,adCmdText);

	return true;
}

bool DB_OnStartUp::readMaxSummonIndex( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_max_summon_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_summon_sid";
	db.command->Parameters->Refresh();
	
	_RecordsetPtr rs = db.command->Execute(NULL,NULL,adCmdText);

	if( rs->Fields->GetItem ("max_summon_sid")->Value.vt == VT_NULL )
	{
		StructPlayer::SetMaxSummonSID( 1 );
	}
	else
	{
		StructPlayer::SetMaxSummonSID( rs->Fields->GetItem ("max_summon_sid")->Value.lVal + 1 );
	}

	return true;
}

bool DB_OnStartUp::readMaxPetIndex( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_max_pet_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_pet_sid";
	db.command->Parameters->Refresh();
	
	_RecordsetPtr rs = db.command->Execute(NULL,NULL,adCmdText);

	if( rs->Fields->GetItem ("max_pet_sid")->Value.vt == VT_NULL )
	{
		StructPlayer::SetMaxPetSID( 1 );
	}
	else
	{
		StructPlayer::SetMaxPetSID( rs->Fields->GetItem ("max_pet_sid")->Value.lVal + 1 );
	}

	return true;
}

bool DB_OnStartUp::readMaxFarmIndex( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_max_farm_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_farm_sid";
	db.command->Parameters->Refresh();

	_RecordsetPtr rs = db.command->Execute(NULL,NULL,adCmdText);

	if( rs->Fields->GetItem ("max_farm_sid")->Value.vt == VT_NULL )
	{
		StructPlayer::SetMaxFarmSID( 1 );
	}
	else
	{
		StructPlayer::SetMaxFarmSID( rs->Fields->GetItem ("max_farm_sid")->Value.lVal + 1 );
	}

	return true;
}

bool DB_OnStartUp::readMaxTitleIndex( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_max_title_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_title_sid";
	db.command->Parameters->Refresh();

	_RecordsetPtr rs = db.command->Execute(NULL,NULL,adCmdText);

	if( rs->Fields->GetItem ("max_title_sid")->Value.vt == VT_NULL )
	{
		StructTitleManager::SetTitleMaxID( 0 );
	}
	else
	{
		StructTitleManager::SetTitleMaxID( rs->Fields->GetItem ("max_title_sid")->Value.lVal );
	}

	return true;
}

bool DB_OnStartUp::readMaxTitleConditionIndex( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_max_title_condition_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_title_condition_sid";
	db.command->Parameters->Refresh();

	_RecordsetPtr rs = db.command->Execute(NULL,NULL,adCmdText);

	if( rs->Fields->GetItem ("max_title_condition_sid")->Value.vt == VT_NULL )
	{
		StructTitleManager::SetTitleConditionMaxID( 0 );
	}
	else
	{
		StructTitleManager::SetTitleConditionMaxID( rs->Fields->GetItem ("max_title_condition_sid")->Value.lVal );
	}

	return true;
}


bool DB_OnStartUp::readMaxRandomOptionIndex( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_read_max_random_option_sid" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_max_random_option_sid";
	db.command->Parameters->Refresh();

	_RecordsetPtr rs = db.command->Execute(NULL,NULL,adCmdText);

	if( rs->Fields->GetItem ("max_random_option_sid")->Value.vt == VT_NULL )
	{
		RandomManager::SetMaxRandomOptionSID( 0 );
	}
	else
	{
		RandomManager::SetMaxRandomOptionSID( rs->Fields->GetItem ("max_random_option_sid")->Value.lVal );
	}

	return true;
}


bool DB_OnStartUp::onProcess( DBConnection & db )
{
	try
	{
		callStartupHandler( db );

		{
			XEnvStruct temp_loader;
			temp_loader.LoadFromFile( "GameServer.eop", "*", true );

			if( temp_loader.IsExist( "db.billing.account" ) ) connectToBillingServer( db );
		}

		readMaxItemIndex( db );
		readMaxSkillIndex( db );
		readMaxStateIndex( db );
		readDisabledStateList( db );
		readMaxSummonIndex( db );
		readMaxPetIndex( db );
		readMaxFarmIndex( db );
		readMaxTitleIndex( db );
		readMaxTitleConditionIndex( db );
		removeDeletedCharacter( db );
		readMaxRandomOptionIndex( db );
	}
	catch( ... )
	{
		throw;
	}

	return true;
}
