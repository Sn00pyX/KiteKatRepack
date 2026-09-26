
#include <cipher/XStrZlibWithSimpleCipherUtil.h>

#include "GameDBUtil.h"

// 이하 DB 유틸리티 펑션
bool InitAdoConnection( _ConnectionPtr & ConnPtr, const char *szConnectionString, const char *szAccount, const char *szPassword )
{
	_bstr_t strCnn = szConnectionString;

	HRESULT hr;

	hr = ConnPtr.CreateInstance(__uuidof(Connection));

	if( hr != S_OK ) return false;

	hr = ConnPtr->Open(	strCnn,
						szAccount,
						szPassword,
						adConnectUnspecified );

	return hr == S_OK;
}

bool InitAdoConnection( _ConnectionPtr & ConnPtr, const char *szDbName, const char *szServer, const char *szAccount, const char *szPassword )
{
	_bstr_t strCnn;

	char szBuf[1024];
	s_sprintf( szBuf, _countof( szBuf ), "Provider=sqloledb;;Persist Security Info=False;Network Library=dbmssocn;Initial Catalog=%s;Data Source=%s",
				szDbName,
				szServer );
	strCnn = szBuf;

	HRESULT hr;

	hr = ConnPtr.CreateInstance(__uuidof(Connection));

	if( hr != S_OK ) return false;

	hr = ConnPtr->Open(	strCnn,
						szAccount,
						szPassword,
						adConnectUnspecified );

	return hr == S_OK;
}

bool InitContentDbConnection( _ConnectionPtr & ConnPtr )
{
	std::string user_connection_string = "Provider=sqloledb;Persist Security Info=False;Network Library=dbmssocn;Initial Catalog=";
	user_connection_string += ENV().GetString( "db.c.name", "Arcadia" );
	user_connection_string += ";Data Source=";
	user_connection_string += ENV().GetString( "db.c.server", "" );
	user_connection_string += ",";
	user_connection_string += ENV().GetString( "db.c.port", "1433" );

	XEnvStruct temp_loader;
	temp_loader.LoadFromFile( "GameServer.eop", "*", true );
	return InitAdoConnection(	ConnPtr,
								ENV().GetString( "db.c.connection_string", user_connection_string.c_str() ).c_str(),
								temp_loader.GetString( "db.c.account", "" ).c_str(),
								XStrZlibWithSimpleCipherUtil::Decrypt( temp_loader.GetString( "db.c._password", "" ).c_str() ).c_str() );
}

bool InitUserDbConnection( _ConnectionPtr & ConnPtr )
{
	std::string user_connection_string = "Provider=sqloledb;Persist Security Info=False;Network Library=dbmssocn;Initial Catalog=";
	user_connection_string += ENV().GetString( "db.user.name", "Arcadia" );
	user_connection_string += ";Data Source=";
	user_connection_string += ENV().GetString( "db.user.server", "" );
	user_connection_string += ",";
	user_connection_string += ENV().GetString( "db.user.port", "1433" );

	XEnvStruct temp_loader;
	temp_loader.LoadFromFile( "GameServer.eop", "*", true );
	return InitAdoConnection(	ConnPtr,
								ENV().GetString( "db.user.connection_string", user_connection_string.c_str() ).c_str(),
								temp_loader.GetString( "db.user.account", "" ).c_str(),
								XStrZlibWithSimpleCipherUtil::Decrypt( temp_loader.GetString( "db.user._password", "" ).c_str() ).c_str() );
}

bool InitBillingDbConnection( _ConnectionPtr & ConnPtr )
{
	std::string billing_connection_string = "Provider=sqloledb;Persist Security Info=False;Network Library=dbmssocn;Initial Catalog=";
	billing_connection_string += ENV().GetString( "db.billing.name", "" );
	billing_connection_string += ";Data Source=";
	billing_connection_string += ENV().GetString( "db.billing.server", "" );
	billing_connection_string += ",";
	billing_connection_string += ENV().GetString( "db.billing.port", "1433" );

	XEnvStruct temp_loader;
	temp_loader.LoadFromFile( "GameServer.eop", "*", true );
	return InitAdoConnection(	ConnPtr,
								ENV().GetString( "db.billing.connection_string", billing_connection_string.c_str() ).c_str(),
								temp_loader.GetString( "db.billing.account", "" ).c_str(),
								XStrZlibWithSimpleCipherUtil::Decrypt( temp_loader.GetString( "db.billing._password", "" ).c_str() ).c_str() );
}
