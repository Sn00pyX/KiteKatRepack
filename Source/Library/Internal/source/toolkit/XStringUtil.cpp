
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>

#include <cstdio>
#include <algorithm>
#include <functional>
#include <locale>

#include "../../include/toolkit/XStringUtil.h"

#pragma comment( lib, "shlwapi.lib" )


namespace XStringUtil
{
	static inline bool isSep( char _c, const char *cSep )
	{
		char *c = const_cast< char* >( cSep );
		while ( *c )
		{
			if ( *c == _c ) return true;
			++c;
		}

		return false;
	}

	void Split( const char * pszCommand, std::vector< std::string >& vToken, const char *pSeparatorList, bool bProcSpecialCharacter )
	{
		if ( !pszCommand ) return;

		std::string strTmp;
		
		bool bBackSlash = false;
		bool bQuotationMark = false;

		char newCmd[512];
		char *pszCmd = const_cast< char* >( pszCommand );
		unsigned len = (unsigned)strlen(pszCmd)+1;

		// { 백스페이스 처리
		if ( bProcSpecialCharacter && strchr( pszCmd, '\b' ) ) {
			unsigned cp=0;
			for ( unsigned x = 0; x < len ; x ++ ) {
				if ( pszCmd[x] == '\b' ) {
					if ( cp ) cp--;
				} else newCmd[cp++] = pszCmd[x];
			}
			pszCmd = newCmd;
		}
		// }

		
		for ( unsigned i = 0 ; i < len ; i++ )
		{
			// { 역슬래쉬 처리
			if ( !bBackSlash && pszCmd[i] == '\\' && bProcSpecialCharacter ) {
				bBackSlash = true;
				continue;
			}
			// }

			if ( !bBackSlash ) {
				if ( ( pszCmd[i] == '\"' && bProcSpecialCharacter			)   ||	// 따옴표가 나왔거나
					 ( !bQuotationMark && (isSep(pszCmd[i], pSeparatorList) ) ) || 	// 스페이스로 파싱중일때 스페이스가 나왔거나
					 ( pszCmd[i] == '\0'					) )			// 문장의 끝일경우
					 
					 
				{
					if ( pszCmd[i] == '\"' && bProcSpecialCharacter )
					{
						bQuotationMark ^= true;				// 따옴표가 나오면 토글
						if (bQuotationMark) continue;
					}

					// 구문 저장
					if ( strTmp.size() ) vToken.push_back( strTmp );
					strTmp.erase( strTmp.begin(), strTmp.end() );
					continue;
				}
			}

			strTmp += pszCmd[i];
			bBackSlash = false;
		}
	}

	inline bool isEqual( char ch, char pattern, bool bIgnoreCase )
	{
		if ( bIgnoreCase )
		{
			ch = static_cast< char >( tolower( ch ) );
			pattern = static_cast< char >( tolower( pattern ) );
		}

		return ( pattern == ch || pattern == '?' );
	}

	bool WildCardCmp( const char *pszPattern, const char *pszString, bool bIgnoreCase )
	{
		if ( !pszPattern || pszPattern[0] == '\0' ) return true;
		
		if ( pszPattern[0] == '*' && pszPattern[1] == '\0' ) return true;

		// Written by Jack Handy - jakkhandy@hotmail.com

		const char *cp = NULL, *mp = NULL;

		while ( (*pszString) && (*pszPattern != '*') )
		{
			if ( !isEqual( *pszString, *pszPattern, bIgnoreCase ) ) return 0;

			pszPattern++;
			pszString++;
		}

		while ( *pszString )
		{
			if ( *pszPattern == '*' )
			{
				if ( !*++pszPattern ) return 1;

				mp = pszPattern;
				cp = pszString+1;
			}
			else if ( isEqual( *pszString, *pszPattern, bIgnoreCase ) )
			{
				pszPattern++;
				pszString++;
			}
			else
			{
				pszPattern = mp;
				pszString = cp++;
			}
		}

		while ( *pszPattern == '*' )
		{
			pszPattern++;
		}

		return !*pszPattern;
	}

	int	CompareStringLogical( const char* sz1, const char* sz2 )
	{
		int nPos1 = -1;
		int nPos2 = -1;
		int nEndPos1 = 0;
		int nEndPos2 = 0;

		while(true)
		{
			++nPos1;
			++nPos2;
			// Make sure we haven't hit the end of 
			//  either of the strings
			if (sz1[nPos1] == 0 && sz2[nPos2] == 0)
			{
				return 0;
			}
			else if (sz1[nPos1] == 0)
			{
				return -1;
			}
			else if (sz2[nPos2] == 0)
			{
				return 1;
			}

			// See if this part of both strings is a number
			if (sz1[nPos1] >= '0' && sz1[nPos1] <= '9' && sz2[nPos2] >= '0' && sz2[nPos2] <= '9')
			{
				// Find the end of each number
				nEndPos1 = nPos1;
				do
				{
					nEndPos1++;
				} while (sz1[nEndPos1] >= '0' && sz1[nEndPos1] <= '9');

				nEndPos2 = nPos2;
				do
				{
					nEndPos2++;
				} while (sz2[nEndPos2] >= '0' && sz2[nEndPos2] <= '9');

				while (true)
				{
					if (nEndPos1 - nPos1 == nEndPos2 - nPos2)
					{
						// Both numbers are the same length, just
						//  compare them
						int nResult = _strnicmp(sz1 + nPos1, sz2 + nPos2, nEndPos1 - nPos1);
						if (nResult == 0)
						{
							nPos1 = nEndPos1 - 1;
							nPos2 = nEndPos2 - 1;
							break;
						}
						else
						{
							return nResult;
						}
					}
					else if (nEndPos1 - nPos1 > nEndPos2 - nPos2)
					{
						// First number is longer, so if it's not zero
						//  padded, it's bigger
						if (sz1[nPos1] == '0')
						{
							++nPos1;
						}
						else
						{
							return 1;
						}
					}
					else
					{
						// Second number is longer, so if it's not zero
						//  padded, it's bigger
						if (sz2[nPos2] == '0')
						{
							++nPos2;
						}
						else
						{
							return -1;
						}
					}
				}
			}
			else
			{
				// One or both characters is not a number, so
				//  just compare them as a string
				int nResult = _strnicmp(sz1 + nPos1, sz2 + nPos2, 1);
				if (nResult != 0)
				{
					return nResult;
				}
			}
		}
	}

	int	CompareStringLogical( const wchar_t* sz1, const wchar_t* sz2 )
	{
		int nPos1 = -1;
		int nPos2 = -1;
		int nEndPos1 = 0;
		int nEndPos2 = 0;

		while(true)
		{
			++nPos1;
			++nPos2;
			// Make sure we haven't hit the end of 
			//  either of the strings
			if (sz1[nPos1] == 0 && sz2[nPos2] == 0)
			{
				return 0;
			}
			else if (sz1[nPos1] == 0)
			{
				return -1;
			}
			else if (sz2[nPos2] == 0)
			{
				return 1;
			}

			// See if this part of both strings is a number
			if (sz1[nPos1] >= L'0' && sz1[nPos1] <= L'9' && sz2[nPos2] >= L'0' && sz2[nPos2] <= L'9')
			{
				// Find the end of each number
				nEndPos1 = nPos1;
				do
				{
					nEndPos1++;
				} while (sz1[nEndPos1] >= L'0' && sz1[nEndPos1] <= L'9');

				nEndPos2 = nPos2;
				do
				{
					nEndPos2++;
				} while (sz2[nEndPos2] >= L'0' && sz2[nEndPos2] <= L'9');

				while (true)
				{
					if (nEndPos1 - nPos1 == nEndPos2 - nPos2)
					{
						// Both numbers are the same length, just
						//  compare them
						int nResult = _wcsnicmp(sz1 + nPos1, sz2 + nPos2, nEndPos1 - nPos1);
						if (nResult == 0)
						{
							nPos1 = nEndPos1 - 1;
							nPos2 = nEndPos2 - 1;
							break;
						}
						else
						{
							return nResult;
						}
					}
					else if (nEndPos1 - nPos1 > nEndPos2 - nPos2)
					{
						// First number is longer, so if it's not zero
						//  padded, it's bigger
						if (sz1[nPos1] == L'0')
						{
							++nPos1;
						}
						else
						{
							return 1;
						}
					}
					else
					{
						// Second number is longer, so if it's not zero
						//  padded, it's bigger
						if (sz2[nPos2] == L'0')
						{
							++nPos2;
						}
						else
						{
							return -1;
						}
					}
				}
			}
			else
			{
				// One or both characters is not a number, so
				//  just compare them as a string
				int nResult = _wcsnicmp(sz1 + nPos1, sz2 + nPos2, 1);
				if (nResult != 0)
				{
					return nResult;
				}
			}
		}
	}

	inline bool isEqual( wchar_t ch, wchar_t pattern, bool bIgnoreCase )
	{
		if( pattern == L'?' ) return true;

		if( bIgnoreCase )
		{
			ch = towlower( ch );
			pattern = towlower( pattern );
		}

		return ( pattern == ch );
	}

	bool WildCardCmp( const wchar_t *pszPattern, const wchar_t *pszString, bool bIgnoreCase )
	{
		if( !pszPattern || pszPattern[0] == L'\0' ) return true;

		if( pszPattern[0] == L'*' && pszPattern[1] == L'\0' ) return true;

		// Written by Jack Handy - jakkhandy@hotmail.com

		const wchar_t *cp = 0, *mp = 0;

		while( (*pszString) && (*pszPattern != L'*') )
		{
			if( !isEqual( *pszString, *pszPattern, bIgnoreCase ) ) return 0;

			pszPattern++;
			pszString++;
		}

		while( *pszString )
		{
			if( *pszPattern == L'*' )
			{
				if( !*++pszPattern ) return 1;

				mp = pszPattern;
				cp = pszString+1;
			}
			else if( isEqual( *pszString, *pszPattern, bIgnoreCase ) )
			{
				pszPattern++;
				pszString++;
			}
			else
			{
				pszPattern = mp;
				pszString = cp++;
			}
		}

		while( *pszPattern == L'*' )
		{
			pszPattern++;
		}

		return !*pszPattern;
	}



	std::vector< std::string > Split( const char * pszCommand, const char *pSeparatorList, bool bProcSpecialCharacter )
	{
		std::vector< std::string > vList;
		Split( pszCommand, vList, pSeparatorList, bProcSpecialCharacter );
		return vList;
	}

	void TrimLeft( char *str )
	{
		const char* buf = str;
		size_t str_len = strlen( str );
		const char* end = str + (str_len+1);
		while( buf != end && *buf && isspace( *buf ) )
		{
			++buf;
		}

		if( str != buf && buf != end )
		{
			size_t len = str_len - (buf - str);
			s_memmove( str, str_len, buf, len*sizeof( char ) );
			str[len] = '\0';
		}
	}

	void TrimRight( char *str )
	{
		size_t len = strlen( str );
		while( len && isspace( str[len-1] ) )
		{
			--len;
		}

		str[len] = '\0';
	}

	void TrimLeft( wchar_t *str )
	{
		const wchar_t* buf = str;
		size_t str_len = wcslen( str );
		const wchar_t* end = str + (str_len+1);
		while( buf != end && *buf && iswspace( *buf ) )
		{
			++buf;
		}

		if( str != buf && buf != end )
		{
			size_t len = str_len - (buf - str);
			s_memmove( str, str_len, buf, len*sizeof( wchar_t ) );
			str[len] = L'\0';
		}
	}

	void TrimRight( wchar_t *str )
	{
		size_t len = wcslen( str );
		while( len && iswspace( str[len-1] ) )
		{
			--len;
		}

		str[len] = L'\0';
	}

	void TrimLeft( std::string & str )
	{
		if ( str.empty() ) return;

		std::string::iterator iter = std::find_if( str.begin(), str.end(), std::not1( std::ptr_fun( isspace ) ) );
		str.erase( str.begin(), iter );
	}

	void TrimRight( std::string & str )
	{
		if ( str.empty() ) return;

		std::string::reverse_iterator iter = std::find_if( str.rbegin(), str.rend(), std::not1( std::ptr_fun( isspace ) ) );
		str.erase( iter.base(), str.end() );
	}

	void TrimLeft( std::wstring & str )
	{
		if ( str.empty() ) return;

		std::wstring::iterator iter = std::find_if( str.begin(), str.end(), std::not1( std::ptr_fun( iswspace ) ) );
		str.erase( str.begin(), iter );
	}

	void TrimRight( std::wstring & str )
	{
		if ( str.empty() ) return;

		std::wstring::reverse_iterator iter = std::find_if( str.rbegin(), str.rend(), std::not1( std::ptr_fun( iswspace ) ) );
		str.erase( iter.base(), str.end() );
	}

	void ToLower( std::string & str )
	{
		size_t pos = 0;
		size_t len = str.length();

		for ( pos = 0; pos < len; ++pos )
		{
			if ( str[ pos ] >= 'A' && str[ pos ] <= 'Z' ) str[ pos ] -= ( 'A' - 'a' );
		}
	}

	void ToUpper( std::string & str )
	{
		size_t pos = 0;
		size_t len = str.length();

		for ( pos = 0; pos < len; ++pos )
		{
			if ( str[ pos ] >= 'a' && str[ pos ] <= 'z' ) str[ pos ] += ( 'A' - 'a' );
		}
	}

	void ToLowerThroughWChar( char *pszTarget, size_t nSize, const char *pszSource, size_t nCount, int nCodePage )
	{
		if( !pszTarget || !pszSource || !nSize )
			return;

		memset( pszTarget, 0, nSize );

		if( nSize <= nCount )
			return;

		wchar_t	*pwcBuffer = new wchar_t[ nSize + 1 ];
		
		if( !pwcBuffer )
			return;

		memset( pwcBuffer, 0, sizeof( wchar_t ) * ( nSize + 1 ) );

		MultiByteToWideChar( nCodePage, 0, pszSource, static_cast< int >( nCount ), pwcBuffer, static_cast< int >( nSize ) );

		s_tolower( pwcBuffer, nSize + 1 );

		WideCharToMultiByte( nCodePage, 0, pwcBuffer, static_cast< int >( nSize ), pszTarget, static_cast< int >( nSize ), NULL, NULL );

		delete[] pwcBuffer;

		return;
	}

	void ToUpperThroughWChar( char *pszTarget, size_t nSize, const char *pszSource, size_t nCount, int nCodePage )
	{
		if( !pszTarget || !pszSource || !nSize )
			return;

		memset( pszTarget, 0, nSize );

		if( nSize <= nCount )
			return;

		wchar_t	*pwcBuffer = new wchar_t[ nSize + 1 ];
		
		if( !pwcBuffer )
			return;

		memset( pwcBuffer, 0, sizeof( wchar_t ) * ( nSize + 1 ) );

		MultiByteToWideChar( nCodePage, 0, pszSource, static_cast< int >( nCount ), pwcBuffer, static_cast< int >( nSize ) );

		s_toupper( pwcBuffer, nSize + 1 );

		WideCharToMultiByte( nCodePage, 0, pwcBuffer, static_cast< int >( nSize ), pszTarget, static_cast< int >( nSize ), NULL, NULL );

		delete[] pwcBuffer;

		return;
	}

	//////////////////////////////////////////////////////////////////////////////

	const char* _getPostfix(const std::string & josa, bool jong)
	{
		// jong : true면 받침있음, false면 받침없음
	    
		if ( !_strnicmp( josa.c_str(), "(을/를)", 7 ) ) return (jong?"을":"를");
		if ( !_strnicmp( josa.c_str(), "(이/가)", 7 ) ) return (jong?"이":"가");
		if ( !_strnicmp( josa.c_str(), "(은/는)", 7 ) ) return (jong?"은":"는");
		if ( !_strnicmp( josa.c_str(), "(와/과)", 7 ) ) return (jong?"과":"와");
		if ( !_strnicmp( josa.c_str(), "(로/으로)", 9 ) ) return (jong?"으로":"로");
		if ( !_strnicmp( josa.c_str(), "(라는/이라는)", 13 ) ) return (jong?"이라는":"라는");

		// 알 수 없는 조사
		return josa.c_str();
	}

	const char* getPostFix( const char *szString, const char* szPostfix )
	{
		if ( !szString || !szString[0] ) return "";
		size_t len = strlen( szString );
		bool bHas = true;

		if ( len == 1 )
		{
			char c = szString[0];

			if ( c >= '0' && c <= '9' )
			{
				if (	c == '1' ||
					c == '3' ||
					c == '6' ||
					c == '7' ||
					c == '8' ||
					c == '0' ) bHas = true;
				else bHas = false;
			}

			if ( c >= 'A' && c <= 'Z' ) c -= ( 'A' - 'a' );

			if ( c >= 'a' && c <= 'z' ) 
			{
				if ( c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y' ) bHas = false;
				else bHas = true;
			}
		}
		else
		{
			WCHAR *buffer = new WCHAR[ len ];
			size_t nRtn = MultiByteToWideChar( CP_OEMCP, 0, szString, (int)len, buffer, (int)len );

			int code = buffer[nRtn-1] - 44032;
		    
			// 한글이 아닐때
			if (code < 0 || code > 11171)
			{
				delete [] buffer;
				return "";
			}

			if (code % 28 == 0) bHas = false;
			else bHas = true;

			delete [] buffer;
		}

		return _getPostfix(szPostfix, bHas);
	}

	void getPrevCharacter( const char * szString, size_t nPos, std::string & strChar )
	{
		char *p = const_cast< char * >( szString + nPos );
		char prev_char = 0;

		while ( p > szString )
		{
			--p;

			// 영어 혹은 숫자
			if ( ( *p >= '0' && *p <= '9' ) ) // || ( *p >= 'a' && *p <= 'z' ) || ( *p >= 'A' && *p <= 'Z' ) ) 영어는 무시해 달라는 구랴..
			{
				strChar = *p;
				return;
			}

			// 한글
			if ( 0x80 & *p )
			{
				if ( prev_char )
				{
					char buf[3] = {0,0,0};
					buf[0] = *p;
					buf[1] = prev_char;
					strChar = buf;
					return;
				}

				prev_char = *p;
				continue;
			}
			else
			{
				prev_char = 0;
				continue;
			}
		}
	}

	void TranslateKoreanPostfix( std::string & strString )
	{
		while ( true )
		{
			size_t tag_len = 0;

			tag_len = 7;
			size_t pos = strString.find( "(을/를)" );
			if ( pos == strString.npos ) pos = strString.find( "(이/가)" );
			if ( pos == strString.npos ) pos = strString.find( "(은/는)" );
			if ( pos == strString.npos ) pos = strString.find( "(와/과)" );
			
			if ( pos == strString.npos ) 
			{
				pos = strString.find( "(로/으로)" );
				if ( pos != strString.npos ) tag_len = 9;
			}

			if ( pos == strString.npos ) 
			{
				pos = strString.find( "(라는/이라는)" );
				if ( pos != strString.npos ) tag_len = 13;
			}
			if ( pos == strString.npos ) break;

			std::string strChar;
			getPrevCharacter( strString.c_str(), pos, strChar );
		
			const char *szPostfix = getPostFix( strChar.c_str(), strString.c_str() + pos );
			strString.replace( strString.begin() + pos, strString.begin() + pos + tag_len, szPostfix, strlen( szPostfix ) );
		}
	}

	void itosc( /*OUT*/ std::string & strNumWithComma, /*IN*/ const int nData, /*IN*/ const unsigned char nGap, /*IN*/ const unsigned char nPunctuator )
	{
		std::string strNum;
		itos( strNum, nData );

		if( strNum.size() <= nGap )
		{
			strNumWithComma = strNum;
			return;
		}

		strNumWithComma.clear();
		strNumWithComma.reserve( strNum.size() + strNum.size() / nGap );

		int nDigitCount = static_cast< int >( ( ( strNum.size() % nGap ) ? strNum.size() % nGap : nGap ) + ( ( nData < 0 ) ? 1 : 0 ) );

		for( std::string::iterator it = strNum.begin() ; it != strNum.end() ; ++it )
		{
			if( nDigitCount-- <= 0 )
			{
				nDigitCount = nGap - 1;
				strNumWithComma += nPunctuator;
			}

			strNumWithComma += (*it);
		}
	}

	void itosc64( /*OUT*/ std::string & strNumWithComma, /*IN*/ const __int64 nData, /*IN*/ const unsigned char nGap, /*IN*/ const unsigned char nPunctuator )
	{
		std::string strNum;
		itos64( strNum, nData );

		if( strNum.size() <= nGap )
		{
			strNumWithComma = strNum;
			return;
		}

		strNumWithComma.clear();
		strNumWithComma.reserve( strNum.size() + strNum.size() / nGap );

		int nDigitCount = static_cast< int >( ( ( strNum.size() % nGap ) ? strNum.size() % nGap : nGap ) + ( ( nData < 0 ) ? 1 : 0 ) );

		for( std::string::iterator it = strNum.begin() ; it != strNum.end() ; ++it )
		{
			if( nDigitCount-- <= 0 )
			{
				nDigitCount = nGap - 1;
				strNumWithComma += nPunctuator;
			}

			strNumWithComma += (*it);
		}
	}

	void EncodeURL( std::string & strURL )
	{
		char szBuf[1024] = {0, };
		std::string strTemp = strURL;
		XStringUtil::Replace( strTemp, "%", "TESTORSGENIUS" );
		DWORD len = sizeof(szBuf);
		UrlCanonicalize( strTemp.c_str(), szBuf, &len, URL_ESCAPE_UNSAFE | URL_ESCAPE_PERCENT );
		strTemp = szBuf;
		XStringUtil::Replace( strTemp, "TESTORSGENIUS", "%25" );
		XStringUtil::Replace( strTemp, "#", "%23" );
		XStringUtil::Replace( strTemp, "&", "%26" );
		XStringUtil::Replace( strTemp, "+", "%2B" );
		strURL = strTemp;
	}

	// To use case by case stack/heap buffer, more pointers should be used. Stable but slower.
	const char * stristr( const char * str, const char * strSearch )
	{
		// No string appears in a string of zero length.
		if( !strlen( str ) )
			return NULL;

		// Following a spec. of strstr.
		if( !strlen( strSearch ) )
			return str;

		// Longer string never appears in shorter one.
		if( strlen( str ) < strlen( strSearch ) )
			return NULL;

		// If the length of both of strings are same, checking can be simplified like below.
		if( strlen( str ) == strlen( strSearch ) )
			return ( _stricmp( str, strSearch ) ) ? NULL : str;

		char szStackBuffer[ 1024 ];
		char * pszBuffer = szStackBuffer;
		size_t nBufferLength = sizeof( szStackBuffer );

		if( strlen( str ) >= nBufferLength )
		{
			nBufferLength = strlen( str ) + 1;
			pszBuffer = new char[ nBufferLength ];
		}

		const char * pszResult = NULL;

		if( !strcpy_s( pszBuffer, nBufferLength, str ) && !_strupr_s( pszBuffer, nBufferLength ) )
		{
			char szStackBuffer_Search[ 1024 ];
			char * pszBuffer_Search = szStackBuffer_Search;
			size_t nBufferLength_Search = sizeof( szStackBuffer_Search );

			if( strlen( strSearch ) >= nBufferLength_Search )
			{
				nBufferLength_Search = strlen( strSearch ) + 1;
				pszBuffer_Search = new char[ nBufferLength_Search ];
			}

			if( !strcpy_s( pszBuffer_Search, nBufferLength_Search, strSearch ) && !_strupr_s( pszBuffer_Search, nBufferLength_Search ) )
			{
				const char * pszChar = pszBuffer;
				const char * pszChar_Search = pszBuffer_Search;

				while( *pszChar )
				{
					if( *pszChar != *pszChar_Search )
					{
						++pszChar;
						continue;
					}

					//if( !strcmp( pszChar + 1, pszChar_Search + 1 ) )
					//{
					const char * pszChar_Temp = pszChar + 1;
					++pszChar_Search;
					while( *pszChar_Search )
					{
						if( *pszChar_Temp != *pszChar_Search )
						{
							pszChar_Search = pszBuffer_Search;
							break;
						}

						++pszChar_Temp;
						++pszChar_Search;
					}
					if( !(*pszChar_Search) )
					//}
					{
						pszResult = str + ( pszChar - pszBuffer );
						break;
					}

					++pszChar;
				}
			}

			if( pszBuffer_Search != szStackBuffer_Search )
				delete[] pszBuffer_Search;
		}

		if( pszBuffer != szStackBuffer )
			delete[] pszBuffer;

		return pszResult;
	}

	char * stristr( char * str, const char * strSearch )
	{
		return const_cast< char * >( stristr( const_cast< const char * >( str ), strSearch ) );
	}

	// Because of using only stack buffer, implementable with less pointers. Faster but unstable in case of long string inputs.
	const char * stristr_fast( const char * str, const char * strSearch )
	{
		// No string appears in a string of zero length.
		if( !strlen( str ) )
			return NULL;

		// Following a spec. of strstr.
		if( !strlen( strSearch ) )
			return str;

		// Longer string never appears in shorter one.
		if( strlen( str ) < strlen( strSearch ) )
			return NULL;

		// If the length of both of strings are same, checking can be simplified like below.
		if( strlen( str ) == strlen( strSearch ) )
			return ( _stricmp( str, strSearch ) ) ? NULL : str;

		char szBuffer[ 1024 ];
		s_strcpy( szBuffer, _countof( szBuffer ), str );
		s_toupper( szBuffer, _countof( szBuffer ) );

		char szBuffer_Search[ 1024 ];
		s_strcpy( szBuffer_Search, sizeof( szBuffer_Search ), strSearch );
		s_toupper( szBuffer_Search, sizeof( szBuffer_Search ) );

		const char * pszChar = szBuffer;
		const char * pszChar_Search = szBuffer_Search;

		while( *pszChar )
		{
			if( *pszChar != *pszChar_Search )
			{
				++pszChar;
				continue;
			}

			//if( !strcmp( pszChar + 1, pszChar_Search + 1 ) )
			//{
			const char * pszChar_Temp = pszChar + 1;
			++pszChar_Search;
			while( *pszChar_Search )
			{
				if( *pszChar_Temp != *pszChar_Search )
				{
					pszChar_Search = szBuffer_Search;
					break;
				}

				++pszChar_Temp;
				++pszChar_Search;
			}
			if( !(*pszChar_Search) )
			//}
				return str + ( pszChar - szBuffer );

			++pszChar;
		}

		return NULL;
	}

	char * stristr_fast( char * str, const char * strSearch )
	{
		return const_cast< char * >( stristr_fast( const_cast< const char * >( str ), strSearch ) );
	}

};