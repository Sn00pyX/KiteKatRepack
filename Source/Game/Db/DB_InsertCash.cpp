
#include <openssl/md5.h>

#include <toolkit/XConsole.h>
#include <toolkit/XEnv.h>

#include "DB_Commands.h"

#include "StructPlayer.h"


bool DB_InsertCash::proc( DBConnection & db )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertCash : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_cash_card_use_info" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_insert_cash_card_use_info";

	std::string appName = ENV().GetString( "app.name", "Unknown" );
	char nServerIdx = appName[ appName.length()-1 ]-'0' + 2;
	__int64 validationCode =
		m_nItemSID + ( m_nAccountID + m_strAccountName[0] * m_strAccountName[ m_strAccountName.length()-1 ] )
		* ( nServerIdx * nServerIdx ) + m_nItemCode * 3;

	unsigned char szMD5Buffer[20];
	s_sprintf( reinterpret_cast<char *>(szMD5Buffer), _countof( szMD5Buffer ), "%I64d", validationCode );

	MD5_CTX context;
	MD5_Init( &context );
	MD5_Update( &context, reinterpret_cast<unsigned char *>(szMD5Buffer), static_cast< unsigned int >( strlen( reinterpret_cast<const char *>(szMD5Buffer) ) ) );

	unsigned char digest[ 16 ];
	MD5_Final( digest, &context );
	char szOutput[ 33 ];
	s_sprintf( szOutput, _countof( szOutput ), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7],
		digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15] );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT", adVarChar, adParamInput, 60, m_strAccountName.c_str() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, sizeof( m_nAccountID ), m_nAccountID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SERVER_NAME", adVarChar, adParamInput, 20, ENV().GetString( "app.name", "Unknown" ).c_str() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_CODE", adInteger, adParamInput, sizeof( m_nItemCode ), m_nItemCode ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ITEM_SID", adBigInt, adParamInput, sizeof( m_nItemSID ), m_nItemSID ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_VALIDATION_CODE", adVarChar, adParamInput, _countof( szOutput ), szOutput ) );

	cmd->Execute( NULL, NULL, adCmdStoredProc );
	
	return true;
}

bool DB_InsertCash::onProcess( DBConnection & db )
{
	try
	{
		proc( db );

		m_pPlayer->onEndQuery();
	}
	catch( ... )
	{
		m_pPlayer->onEndQuery();

		throw;
	}

	return true;
}