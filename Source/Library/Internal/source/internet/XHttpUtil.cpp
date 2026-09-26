
#include "XHttpUtil.h"


namespace XHttpUtil
{

	DWORD	ChooseAuthScheme( DWORD dwSupportedSchemes )
	{
		if( dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NEGOTIATE )
			return WINHTTP_AUTH_SCHEME_NEGOTIATE;
		else if( dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NTLM )
			return WINHTTP_AUTH_SCHEME_NTLM;
		else if( dwSupportedSchemes & WINHTTP_AUTH_SCHEME_PASSPORT )
			return WINHTTP_AUTH_SCHEME_PASSPORT;
		else if( dwSupportedSchemes & WINHTTP_AUTH_SCHEME_DIGEST )
			return WINHTTP_AUTH_SCHEME_DIGEST;
		else if( dwSupportedSchemes & WINHTTP_AUTH_SCHEME_BASIC )
			return WINHTTP_AUTH_SCHEME_BASIC;

		return 0;
	}

	const wchar_t*	GetErrorFunction( DWORD_PTR dwWhere )
	{
		switch( dwWhere )
		{   
		case API_RECEIVE_RESPONSE:
			return L"WinHttpReceiveResponse";
		case API_QUERY_DATA_AVAILABLE:
			return L"WinHttpQueryDataAvailable";
		case API_READ_DATA:
			return L"WinHttpReadData";
		case API_WRITE_DATA:
			return L"WinHttpWriteData";
		case API_SEND_REQUEST:
			return L"WinHttpSendRequest";
		case API_HEADER_STATUS_CODE:
			return L"WinHttpQueryHeaders_StatusCode";
		case API_HANDLE_CLOSING:
			return L"Callback_Closing";
		}

		return L"Unknown_function";
	}



	XHttpSetRequestHeader::XHttpSetRequestHeader( HINTERNET hRequest )
		: m_hRequest( hRequest )
	{
	}

	XHttpSetRequestHeader::~XHttpSetRequestHeader()
	{
		m_hRequest = NULL;
	}

	bool	XHttpSetRequestHeader::Add( const std::wstring& strName, const std::wstring& strValue )
	{
		std::wstring strHeader = strName + L": " + strValue + L"\r\n";
		return Add( strHeader );
	}

	bool	XHttpSetRequestHeader::Add( const std::wstring& strHeader )
	{
		if( strHeader.empty() == false )
		{
			if( ::WinHttpAddRequestHeaders(
					m_hRequest,
					strHeader.c_str(),
					static_cast< DWORD >( strHeader.size() ),
					WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE )
				== FALSE )
			{
				return false;
			}
		}

		return true;
	}



};

