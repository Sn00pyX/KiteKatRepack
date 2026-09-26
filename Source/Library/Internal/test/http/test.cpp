
#include <tchar.h>
#include <string>

#include "../../include/internet/XHttpClient.h"
//#include "../../include/internet/XInternetSession2.h"


#pragma warning( push )
#pragma warning( disable : 4819 )
#include <boost/network/protocol/http/client.hpp>
#include <boost/network/uri.hpp>
#include <boost/network/uri/uri_io.hpp>
#pragma warning( pop )

#include <iostream>

#include <cpp_netlib_lib_include.h>
#include "../../include/internal_base_include.h"

#pragma comment( lib, "winhttp.lib" )
#pragma comment( lib, "Wininet.lib" )


class	TestContent : public XHttpContent
{
private:

	bool	OnHeader( const XHttpResponseHeader& Header )
	{
		wprintf( L"Status Code: %d\n", Header.StatusCode() );
		wprintf( L"Content-Type: %s\n", Header.ContentType().c_str() );
		wprintf( L"Content-Length: %d\n", Header.ContentLength() );

		wprintf( L"\n---Header---\n%s\n\n", Header.Get().c_str() );
		return true;
	}

	bool	OnBody( const void* pBuffer, size_t nReadBytes )
	{
		std::string data( (const char*)pBuffer, nReadBytes );
		//wprintf( L"OnBody(): %d received\n", nReadBytes );
		printf( "data\n\n%s\n", data.c_str() );
		return true;
	}

	void	OnComplete()
	{
		wprintf( L"OnComplete.\n\n" );
	}

	void	OnError( const wchar_t* szWhere, DWORD dwError )
	{
		wprintf( L"OnError( %s, %d) - detail( %s: %d )\n\n", szWhere, dwError, m_szFunction, m_nLine );
	}


};


int _tmain()
{
	/*
	struct myInternetFileWriter : XInternetFileWriter2
	{
		myInternetFileWriter()
		{
		}

		virtual ~myInternetFileWriter()
		{
		}		

		bool Write( const char* pData, unsigned nSize )
		{

			return true;
		}
	};

	const char* url = "http://cdn.gpotato.jp/rappelz/patch_ra/021/n@UB6,~='fyKZ(45mt)D7AuKU";
	XInternetSession2* pSession = XInternetSession2Factory::CreateSessionAndConnect( url );

	myInternetFileWriter _fo;
	if( pSession->GetFile( "rappelz/patch_ra/021/n@UB6,~='fyKZ(45mt)D7AuKU", &_fo ) < 1 )
	{
		return 0;
	}

	delete pSession;
	return 1;
	*/

	using namespace boost::network;

	try {
		uri::uri url;
		url << uri::scheme("https")
			<< uri::host("api.gpotato.eu")
			<< uri::path( "Account/Authentication.aspx" )
			<< uri::query( "MODE=L&GAMECODE=G010&GPOTATOID=aaaaaa&PASSWORD=bbbbbb&USERIP=1.1.1.1" );

		std::string urlstr = url.string();
		std::cout << urlstr << std::endl;

		http::client client;
		http::client::request request( url );
		http::client::response response = client.get(request);
		std::string result = body( response );
		std::cout << result << std::endl;
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}



	/*
	using namespace boost::network;

	try {
		uri::uri url;
		url << uri::scheme("http")
			<< uri::host("api.gamexpconnect.com")
			<< uri::port( 80 )
			<< uri::path("/api.php")
			<< uri::query( "method_name=get_user_info&gamexp_sid=123456789012345678&ip=88.248.255.250&client_id=501101&secret_key=d41d8cd98f00b204e9800998ecf8427e&format=json" );

		std::string urlstr = url.string();
		std::cout << urlstr << std::endl;

		//std::string url = "http://api.gamexpconnect.com/api.php?method_name=get_user_info&gamexp_sid=123456789012345678&ip=88.248.255.250&client_id=501101&secret_key=d41d8cd98f00b204e9800998ecf8427e&format=json";

        http::client client;
        http::client::request request( url );
        http::client::response response = client.get(request);
		std::string result = body( response );
        std::cout << result << std::endl;
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
	*/

	/*
	{
		XHttpClient Client;
		Client.Timeout( 60*1000 );

		//const std::wstring gamecode = L"method_name=get_user_info&gamexp_sid=%SID&ip=%IP&client_id=%CLIENTID&secret_key=%SECRETKEY&format=plain";
		const std::string gamecode = "method_name=get_user_info&gamexp_sid=123456789012345678&ip=88.248.255.250&client_id=501101&secret_key=d41d8cd98f00b204e9800998ecf8427e&format=json";

		XHttpURL URL;

		//URL.Scheme( L"http" );
		//URL.Secure( false );
		//URL.Host( L"api.gamexpconnect.com" );
		//URL.Port( INTERNET_DEFAULT_HTTP_PORT );
		//URL.Path( L"api.php" );
		//URL.Extra( gamecode );


		URL.Set( L"http://api.gamexpconnect.com/api.php?method_name=get_user_info&gamexp_sid=123456789012345678&ip=88.248.255.250&client_id=501101&secret_key=d41d8cd98f00b204e9800998ecf8427e&format=json" );

		std::string strData = gamecode;

		XHttpRequest Request( URL );
		//Request.Referer( L"http://rz_client" );
		//Request.SetData( strData.data(), strData.size()+1 );
		//Request.AddHeader( L"Content-Type", L"application/x-www-form-urlencoded" );

		wchar_t szContentLen[64] = { 0, };
		swprintf_s( szContentLen, _countof( szContentLen ), L"%d", URL.Extra().size()*sizeof( wchar_t ) );
		Request.AddHeader( L"Content-Length", szContentLen );


		//XHttpURL URL( L"http://img.naver.net/static/www/u/2010/0611/nmms_215646753.gif" );
		//XHttpRequest Request( URL );

		TestContent Content;
		if( Client.Get< XHttpSync >( Request, &Content ) == false )
		{
		}

		Content.Wait();

		wprintf( L"url: %s...", URL.Build().c_str() );
		if( Content.IsSucceeded() == true )
		{
			wprintf( L"ok.\n" );
		}
		else
		{
			wprintf( L"failed.\n" );
		}
	}
	*/

	/*
	{
		XHttpClient Client;
		Client.Timeout( 60*1000 );

		XHttpURL URL( L"http://dic.daum.net/search.do" );
		XHttpRequest Request( URL );
		Request.AddHeader( L"Content-Type", L"application/x-www-form-urlencoded" );

		XHttpRequestParam Param;
		Param.Add( L"dic", L"eng" );
		Param.Add( L"q", L"apple" );

		Request.Param( &Param );
		TestContent Content;
		Client.Get< XHttpAsync >( Request, &Content );

		Content.Wait();

		wprintf( L"url: %s%s...", URL.Build().c_str(), Param.Build().c_str() );
		if( Content.IsSucceeded() == true )
		{
			wprintf( L"ok.\n" );
		}
		else
		{
			wprintf( L"failed.\n" );
		}
	}
	*/


/*
	{
		XHttpClient Client;
		Client.Timeout( 60*1000 );

		XHttpURL URL( L"https://www.google.co.kr" );
		XHttpRequest Request( URL );
		//Request.AddHeader( L"Content-Type", L"application/x-www-form-urlencoded" );

		//XHttpRequestParam Param;
		//Param.Add( L"dic", L"eng" );
		//Param.Add( L"q", L"apple" );

		//Request.Param( &Param );
		TestContent Content;
		Client.Get< XHttpAsync >( Request, &Content );

		Content.Wait();

		wprintf( L"url: %s...", URL.Build().c_str() );
		if( Content.IsSucceeded() == true )
		{
			wprintf( L"ok.\n" );
		}
		else
		{
			wprintf( L"failed.\n" );
		}
	}
	*/


	return 0;
}

