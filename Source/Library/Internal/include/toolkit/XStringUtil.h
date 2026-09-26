#pragma once

#include <string>
#include <vector>

#include <cstdlib>
#include <cstdarg>

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../toolkit/safe_function.h"

/*	
	void	Split( const char * pszCommand, std::vector< std::string >& vToken, const char *pSeparatorList = " ", bool bProcSpecialCharacter = true )
	std::vector< std::string > Split( const char * pszCommand, const char *pSeparatorList = " ", bool bProcSpecialCharacter = true )
	void	TranslateKoreanPostfix( std::string & strString )

	int		Replace( std::string & str, const char * pszFrom, size_t unFromLen, const char * pszTo, size_t unToLen )
	int		Replace( std::string & str, const std::string & strFrom, const std::string & strTo )
	int		Replace( std::string & str, const char *pszFrom, const char *pszTo )
	int		Replace( char * pszBuf, const char *pszFrom, const char *pszTo )

	void	TrimLeft( std::string & str, const char *szCharList = " \t\r\n\0B" )
	void	TrimRight( std::string & str, const char *szCharList = " \t\r\n\0B" )
	void	Trim( std::string & str, const char *szCharList = " \t\r\n\0B" )

	void	ToLower( std::string & str )
	void	ToUpper( std::string & str )

	void	Format( std::string & str, const char *fmt, ... )
	void	itos( std::string & str, int n )

	bool	WildCardCmp( const char *pszPattern, const char *pszString, bool bIgnoreCase = false );

	void	EncodeURL( std::string & strURL )

	const char *	stristr( const char * str, const char * strSearch );
	char *			stristr( char * str, const char * strSearch );
*/

namespace XStringUtil
{
	void Split( const char * pszCommand, std::vector< std::string >& vToken, const char *pSeparatorList = " ", bool bProcSpecialCharacter = true );

	bool WildCardCmp( const char *pszPattern, const char *pszString, bool bIgnoreCase = false );
	bool WildCardCmp( const wchar_t *pszPattern, const wchar_t *pszString, bool bIgnoreCase );

	// http://www.scottandmichelle.net/scott/code/index2.mv?codenum=100
	int	CompareStringLogical( const char* sz1, const char* sz2 );
	int	CompareStringLogical( const wchar_t* sz1, const wchar_t* sz2 );

	std::vector< std::string > Split( const char * pszCommand, const char *pSeparatorList = " ", bool bProcSpecialCharacter = true );

	void TranslateKoreanPostfix( std::string & strString );

	inline int Replace( std::string & str, const char * pszFrom, size_t unFromLen, const char * pszTo, size_t unToLen )
	{
		int cnt  = 0;
		size_t nPos = 0;

		while ( true )
		{
			nPos = str.find( pszFrom, nPos );
			if ( nPos == std::string::npos ) break;

			str.replace( nPos, unFromLen, pszTo );
			nPos += unToLen;

			cnt++;
		}

		return cnt;
	}

	inline int Replace( std::string & str, const std::string & strFrom, const std::string & strTo )
	{
		return Replace( str, strFrom.c_str(), strFrom.size(), strTo.c_str(), strTo.size() );
	}

	inline int Replace( std::string & str, const char *pszFrom, const char *pszTo )
	{
		return Replace( str, pszFrom, strlen( pszFrom ), pszTo, strlen( pszTo ) );
	}

	inline int Replace( char * pszBuf, unsigned unBufCount, const char *pszFrom, const char *pszTo )
	{
		std::string strOriginal( pszBuf );

		int nCnt = Replace( strOriginal, pszFrom, strlen( pszFrom ), pszTo, strlen( pszTo ) );

		if ( strOriginal.size() > unBufCount-1 ) return -1;

		s_strcpy( pszBuf, unBufCount, strOriginal.c_str() );

		return nCnt;
	}

	inline int Replace( std::wstring& str, const wchar_t* pszFrom, size_t unFromLen, const wchar_t* pszTo, size_t unToLen )
	{
		int cnt  = 0;
		size_t nPos = 0;

		while( true )
		{
			nPos = str.find( pszFrom, nPos );
			if( nPos == std::wstring::npos ) break;

			str.replace( nPos, unFromLen, pszTo );
			nPos += unToLen;

			cnt++;
		}

		return cnt;
	}

	inline int Replace( std::wstring & str, const std::wstring & strFrom, const std::wstring & strTo )
	{
		return Replace( str, strFrom.c_str(), strFrom.size(), strTo.c_str(), strTo.size() );
	}

	inline int ReplaceFormat( std::string & str, const char * pszFrom, const char * pszToFormat, ... )
	{
		char p[1024];
		va_list ap;
		va_start(ap, pszToFormat);
		s_vsprintf( p, _countof( p ), pszToFormat, ap );
		va_end(ap);

		return Replace( str, pszFrom, strlen( pszFrom ), p, strlen( p ) );
	}

	void ToLower( std::string & str );
	void ToUpper( std::string & str );
	void ToLowerThroughWChar( char *pszTarget, size_t nSize, const char *pszSource, size_t nCount, int nCodePage );
	void ToUpperThroughWChar( char *pszTarget, size_t nSize, const char *pszSource, size_t nCount, int nCodePage );

	void TrimLeft( std::string & str );
	void TrimRight( std::string & str );
	inline void Trim( std::string & str )
	{
		TrimRight( str );
		TrimLeft( str );
	}

	void TrimLeft( std::wstring & str );
	void TrimRight( std::wstring & str );
	inline void Trim( std::wstring & str )
	{
		TrimRight( str );
		TrimLeft( str );
	}

	void TrimLeft( char *str );
	void TrimRight( char *str );
	inline void Trim( char *str )
	{
		TrimRight( str );
		TrimLeft( str );
	}

	void TrimLeft( wchar_t *str );
	void TrimRight( wchar_t *str );
	inline void Trim( wchar_t *str )
	{
		TrimRight( str );
		TrimLeft( str );
	}

	inline std::wstring	Multi2Wide( const char* szString, int code_page = CP_ACP )
	{
		std::wstring temp;
		int wstrLen = ::MultiByteToWideChar( code_page, 0, szString, -1, NULL, 0 );
		if( wstrLen > 0 )
		{
			wchar_t* wstr = new wchar_t[wstrLen+1];
			wstr[0] = L'\0';

			if( ::MultiByteToWideChar( code_page, 0, szString, -1, wstr, wstrLen ) > 0 )
			{
				temp = wstr;
			}

			delete [] wstr;
		}

		return temp;
	}

	inline std::string	Wide2Multi( const wchar_t* szWString, int code_page = CP_ACP )
	{
		std::string temp;
		int strLen = ::WideCharToMultiByte( code_page, 0, szWString, -1, NULL, 0, NULL, NULL );
		if( strLen > 0 )
		{
			char* str = new char[strLen+1];
			str[0] = '\0';

			if( ::WideCharToMultiByte( code_page, 0, szWString, -1, str, strLen, NULL, NULL ) > 0 )
			{
				temp = str;
			}

			delete [] str;

		}

		return temp;
	}


	inline void Format( std::string & str, const char *fmt, ... )
	{
		char p[1024];
		va_list ap;
		va_start(ap, fmt);
		s_vsprintf( p, _countof( p ), fmt, ap );
		va_end(ap);
		str = p;
	}

	inline void itos( std::string & str, int n )
	{
		char buf[128];
		s_sprintf( buf, _countof( buf ), "%d", n );
		str = buf;
	}

	inline void itos64( std::string & str, __int64 n )
	{
		char buf[128];
		s_sprintf( buf, _countof( buf ), "%I64d", n );
		str = buf;
	}

	// Int TO String punctuated with Comma(s)
	void itosc( /*OUT*/ std::string & strNumWithComma, /*IN*/ const int nData, /*IN*/ const unsigned char nGap = 3, /*IN*/ const unsigned char nPunctuator = ',' );
	void itosc64( /*OUT*/ std::string & strNumWithComma, /*IN*/ const __int64 nData, /*IN*/ const unsigned char nGap = 3, /*IN*/ const unsigned char nPunctuator = ',' );

	void EncodeURL( std::string & strURL );

	// Stable version(Using stack/heap buffer with parameter length check)
	const char *	stristr( const char * str, const char * strSearch );
	char *			stristr( char * str, const char * strSearch );
	// Fast version(Using stack buffer only without any parameter length check)
	const char *	stristr_fast( const char * str, const char * strSearch );
	char *			stristr_fast( char * str, const char * strSearch );
};
