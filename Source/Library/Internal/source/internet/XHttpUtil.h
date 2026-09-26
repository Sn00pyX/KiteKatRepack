
#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

#include <string>


namespace XHttpUtil
{

	DWORD	ChooseAuthScheme( DWORD dwSupportedSchemes );

	// 추가 에러 함수 디파인
	#define	API_HEADER_STATUS_CODE	1000
	#define	API_HANDLE_CLOSING		1001
	const wchar_t*	GetErrorFunction( DWORD_PTR dwWhere );

	class	XHttpException
	{
	public:

		XHttpException( DWORD_PTR dwWhere, DWORD dwError, const wchar_t* szFunction, int nLine )
			: m_dwWhere( dwWhere )
			, m_dwError( dwError )
			, m_szFunction( szFunction )
			, m_nLine( nLine )
		{
		}

		DWORD_PTR		m_dwWhere;
		DWORD			m_dwError;

		const wchar_t*	m_szFunction;
		int				m_nLine;
	};


	class	XHttpSetRequestHeader
	{
	public:

		explicit XHttpSetRequestHeader( HINTERNET hRequest );
		~XHttpSetRequestHeader();

		bool	Add( const std::wstring& strName, const std::wstring& strValue );
		bool	Add( const std::wstring& strHeader );

	private:

		HINTERNET	m_hRequest;

	};



};

