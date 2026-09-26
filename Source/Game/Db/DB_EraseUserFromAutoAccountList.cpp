
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XConsole.h>

#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"


bool DB_EraseUserFromAutoAccountList::proc( DBConnection & db )
{
	db.command->CommandType = adCmdStoredProc;
	db.command->CommandText = _bstr_t( "dbo.smp_erase_user_from_auto_account_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_erase_user_from_auto_account_list";
	db.command->Parameters->Refresh();
	
	db.command->Parameters->Item[ "@IN_ACCOUNT_ID" ]->Value = m_nAccountID;

	db.command->Execute(NULL, NULL,adCmdStoredProc);

	return true;
}

bool DB_EraseUserFromAutoAccountList::onProcess( DBConnection & db )
{
	proc( db );

	return true;
}

void DB_EraseUserFromAutoAccountList::onFail( const _com_error & exception )
{
	_cprint( "DB_EraseUserFromAutoAccountList failed. nAccount(%d)\n", m_nAccountID );
	FILELOG( "DB_EraseUserFromAutoAccountList failed. nAccount(%d)", m_nAccountID );
}
