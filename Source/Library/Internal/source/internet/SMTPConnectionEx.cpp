
#define	_SECURE_ATL	1

#include <atlsmtpconnection.h>
#include <process.h>

#include "../../include/internet/SMTPConnectionEx.h"


std::string	GetDomain( const std::string& strEmailAddr )
{
	std::string strDomain = strEmailAddr;

	std::string::size_type pos = strDomain.find( '@' );
	if( pos != std::string::npos )
	{
		strDomain.erase( 0, pos + 1 );
	}

	return strDomain;
}

unsigned int __stdcall	SendMailThread( void* pParam )
{
	SSendMailInfo* pMailInfo = static_cast <SSendMailInfo *> ( pParam );

	CoInitialize( NULL );

	CMimeMessage msg;

	std::string strDomain = pMailInfo->strServer;
	if( strDomain.empty() )
	{
		strDomain = GetDomain( pMailInfo->strSender );
	}

	msg.SetSender( pMailInfo->strSender.c_str() );
	msg.SetSenderName( pMailInfo->strSenderName.c_str() );

	std::vector< SRecipInfo >::const_iterator pos = pMailInfo->vecRecip.begin();
	std::vector< SRecipInfo >::const_iterator end = pMailInfo->vecRecip.end();
	for( ; pos != end; ++pos )
	{
		const SRecipInfo& info = (*pos);
		msg.AddRecipient( info.strRecipEMail.c_str(), info.strRecipName.c_str());
	}

	msg.SetSubject( pMailInfo->strSubject.c_str() );
	msg.AddText( pMailInfo->strText.c_str() );

	delete pMailInfo;

	CSMTPConnection con;
	if( con.Connect( strDomain.c_str() ) != FALSE )
	{
		con.SendMessage( msg );
		con.Disconnect();
	}

	CoUninitialize();
	return 0;
}

void	SendMail( const SSendMailInfo* pMailInfo )
{
	if( pMailInfo->vecRecip.empty() == false )
	{
		SSendMailInfo* pTemp = new SSendMailInfo;
		*pTemp = *pMailInfo;

		unsigned int nThreadID = 0;
		HANDLE hThread = reinterpret_cast< HANDLE >( _beginthreadex( NULL, 0, SendMailThread, pTemp, 0, &nThreadID ) );
		CloseHandle( hThread );
	}
}