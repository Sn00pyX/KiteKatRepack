//
//  File:       nsl.h
//
//  Contents:   wrapper of iconv unicode library and some other utility
//
//  Functions:  nsl::split
//				nsl::replace
//				nsl::toupper
//				nsl::tolower
//				nsl::wildcmp
//				nsl::sprintf_str
//				nsl::format
//				nsl::itos<>
//				nsl::itos
//				nsl::itosw
//				nsl::insert_comma<>
//				nsl::insert_comma
//				nsl::insert_commaw
//
//				nsl::path::get_path_separator
//				nsl::path::get_file_name
//				nsl::path::get_directory_name
//				nsl::path::get_extension
//				nsl::path::trim_extension
//				nsl::path::get_file_name_without_extension
//				nsl::path::normalize
//				nsl::path::join
//
//  Author:     Kang, KiHyun ( testors@nflavor.com )
//

#pragma once

#include <string>
#include <algorithm>
#include <cstdarg>
#include <locale>
#include <vector>

#include "safe_function.h"

namespace nl
{
	namespace str
	{
		template< typename _Elem >
		inline void sprintf_str( std::basic_string< _Elem > & str, const _Elem* fmt, ... )
		{
			_Elem buf[128];
			va_list args;
			va_start( args, fmt );

			_Elem* p = buf;
			size_t buf_len = sizeof(buf)/sizeof(_Elem);

			while ( -1 == s_vsprintf( p, buf_len, fmt, args ) )
			{
				if ( p != buf ) delete [] p;

				buf_len *= 2;

				p = new _Elem[ buf_len ];
			}

			str = p;

			if ( p != buf ) delete [] p;
		}

		template< typename _Elem >
		inline std::basic_string< _Elem > format( const _Elem* fmt, ... )
		{
			_Elem buf[128];
			va_list args;
			va_start( args, fmt );

			_Elem* p = buf;
			size_t buf_len = sizeof(buf)/sizeof(_Elem);

			while ( -1 == s_vsprintf( p, buf_len, fmt, args ) )
			{
				if ( p != buf ) delete [] p;

				buf_len *= 2;

				p = new _Elem[ buf_len ];
			}

			std::basic_string< _Elem > strTemp = p;

			if ( p != buf ) delete [] p;

			return strTemp;
		}

		template< typename _Elem >
		inline std::basic_string< _Elem > itos( int n )							{ assert( 0 ); return "";				}
		template<> inline std::basic_string< char >  itos< char >( int n )		{ return nl::str::format( "%d", n );	}
		template<> inline std::basic_string< wchar_t > itos< wchar_t >( int n )	{ return nl::str::format( L"%d", n );	}
		inline std::string  itos( int n )										{ return nl::str::format( "%d", n );	}
		inline std::wstring itosw( int n )										{ return nl::str::format( L"%d", n );	}

		template< typename _Elem >
		inline size_t replace( std::basic_string< _Elem >& str, const _Elem* from, size_t from_len, const _Elem* to, size_t to_len, size_t max_cnt = 0xffffffff )
		{
			size_t cnt  = 0;
			size_t nPos = 0;

			while ( true )
			{
				if ( cnt >= max_cnt ) break;

				nPos = str.find( from, nPos );
				if ( nPos == std::string::npos ) break;

				str.replace( nPos, from_len, to );
				nPos += to_len;

				cnt++;
			}

			return cnt;
		}

		template< typename _Elem >
		inline size_t replace( std::basic_string< _Elem >& str, const _Elem* from, const _Elem* to, size_t max_cnt = 0xffffffff )
		{
			size_t from_len = 0;
			size_t to_len = 0;

			const _Elem* p;

			p = from;
			while ( *++p ) ;
			from_len = p - from;

			p = to;
			while ( *++p ) ;
			to_len = p - to;

			return replace( str, from, from_len, to, to_len, max_cnt );
		}

		template< typename _Elem >
		inline size_t strlen( const _Elem* str )
		{
			size_t len = 0;
			while ( *str )
			{
				++len;
				++str;
			}
			return len;
		}

		template< typename _Elem, typename _INTEGER >
		inline std::basic_string< _Elem > itoa( _INTEGER in )
		{
			_INTEGER n = in;
			if ( n < 0 ) n = 0 - n;

			std::basic_string< _Elem > str;

			do 
			{
				str.insert( 0, 1, (_Elem)( '0' + ( n % 10 ) ) );
				n /= 10;
			} while ( n );

			if ( in < 0 ) str.insert( 0, 1, '-' );

			return str;
		}

		template< typename _Elem, typename _INTEGER >
		inline _INTEGER atoi( const _Elem* str )
		{
			bool bIsMinus = false;

			if ( str[0] == '-' )
			{
				bIsMinus = true;
				++str;
			}

			if ( str[0] == '+' ) ++str;

			size_t len = strlen( str );
			if ( len <= 0 ) return static_cast< _INTEGER >( 0 );

			const _Elem* back = ( str + len - 1 );
			_INTEGER nResult	= static_cast< _INTEGER >( 0 );
			_INTEGER nCur		= static_cast< _INTEGER >( 1 );

			while ( back >= str )
			{
				if ( *back > '9' || *back < '0' ) break;
				nResult = static_cast< _INTEGER >( nResult + ( *back - '0' ) * nCur );
				nCur = static_cast< _INTEGER >( nCur * 10 );
				--back;
			}
			
			//bIsMinus 체크
			return bIsMinus ? 0-nResult:nResult;
		}

		template< typename _Elem >
		inline size_t replace( std::basic_string< _Elem >& str, const std::basic_string< _Elem >& from, const std::basic_string< _Elem >& to, size_t max_cnt = 0xffffffff )
		{
			return replace( str, from.c_str(), from.size(), to.c_str(), to.size(), max_cnt );
		}

		template< typename _Elem >
		inline void toupper( std::basic_string< _Elem >& str  )
		{
			for ( std::basic_string< _Elem >::iterator it = str.begin(); it != str.end(); ++it )
			{
				if ( *it >= 'a' && *it <= 'z' ) *it = *it - ('a' - 'A');
			}
		}

		template< typename _Elem >
		inline void tolower( std::basic_string< _Elem >& str  )
		{
			for ( std::basic_string< _Elem >::iterator it = str.begin(); it != str.end(); ++it )
			{
				if ( *it >= 'A' && *it <= 'Z' ) *it = *it + ('a' - 'A');
			}
		}

		template< typename _Elem >
		inline bool isEqual( _Elem ch, _Elem pattern, bool ignore_case )
		{
			if ( ignore_case )
			{
				if ( ch >= 'A' && ch <= 'Z' )			ch -= ( 'A' - 'a' );
				if ( pattern >= 'A' && pattern <= 'Z' )	pattern -= ( 'A' - 'a' );
			}

			return ( pattern == ch || pattern == '?' );
		}

		template< typename _Elem >
		bool wildcmp( const _Elem* pattern, const _Elem* str, bool ignore_case = false )
		{
			if ( !pattern || pattern[0] == '\0' ) return true;
			
			if ( pattern[0] == '*' && pattern[1] == '\0' ) return true;

			const _Elem* cp = 0,* mp = 0;

			while ( (*str) && (*pattern != '*') )
			{
				if ( !isEqual( *str, *pattern, ignore_case ) ) return 0;

				pattern++;
				str++;
			}

			while ( *str )
			{
				if ( *pattern == '*' )
				{
					if ( !*++pattern ) return 1;

					mp = pattern;
					cp = str+1;
				}
				else if ( isEqual( *str, *pattern, ignore_case ) )
				{
					pattern++;
					str++;
				}
				else
				{
					pattern = mp;
					str = cp++;
				}
			}

			while ( *pattern == '*' )
			{
				pattern++;
			}

			return !*pattern;
		}

		template< typename _Elem >	bool wildcmp( const _Elem* pattern, const std::basic_string<_Elem >& str, bool ignore_case = false ) { return wildcmp( pattern, str.c_str(), ignore_case ); }
		template< typename _Elem >	bool wildcmp( const std::basic_string< _Elem >& pattern, const std::basic_string<_Elem >& str, bool ignore_case = false ) { return wildcmp( pattern.c_str(), str.c_str(), ignore_case ); }
		template< typename _Elem >	bool wildcmp( const std::basic_string< _Elem >& pattern, const _Elem* str, bool ignore_case = false ) { return wildcmp( pattern.c_str(), str, ignore_case ); }

		template< typename _Elem >
		inline bool is_delimiter( _Elem _c, const _Elem* delimiter )
		{
			_Elem* c = const_cast< _Elem* >( delimiter );
			while ( *c )
			{
				if ( *c == _c ) return true;
				++c;
			}

			return false;
		}

		template< typename _Elem, typename _Container >
		inline void split_with_string( const _Elem* pszCommand, _Container& vToken, const _Elem* pSeparatorString, bool bAllowBlankString = false )
		{
			if ( !pszCommand ) return;

			const char *p = pszCommand;

			std::string strTemp;
			size_t separator_len = strlen( pSeparatorString );

			while( *p )
			{
				if( strncmp( p, pSeparatorString, separator_len ) == 0 )
				{
					if( bAllowBlankString || !strTemp.empty() )
					{
						vToken.push_back( strTemp );
					}
					p += separator_len;
					strTemp.clear();
					continue;
				}

				strTemp.append( 1, *p );
				++p;				
			}

			if( bAllowBlankString || !strTemp.empty() )
			{
				vToken.push_back( strTemp );
			}
		}

		template< typename _Elem, typename _Container >
		inline void split( const _Elem* pszCommand, _Container& vToken, const _Elem* pDelimiterList, bool bProcSpecialCharacter = true, bool bAllowBlankString = false )
		{
			if ( !pszCommand ) return;

			std::basic_string< _Elem > strTmp;
			
			bool bBackSlash = false;
			bool bQuotationMark = false;

			//_Elem newCmd[512];
			_Elem* pszCmd = const_cast< _Elem* >( pszCommand );
			unsigned len = (unsigned)strlen(pszCmd)+1;
			
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
						( !bQuotationMark && (is_delimiter(pszCmd[i], pDelimiterList) ) ) || 	// 스페이스로 파싱중일때 스페이스가 나왔거나
						( pszCmd[i] == '\0'					) )			// 문장의 끝일경우
						 
						 
					{
						if ( pszCmd[i] == '\"' )
						{
							bQuotationMark ^= true;				// 따옴표가 나오면 토글
							if (bQuotationMark) continue;
						}

						// 구문 저장
						if ( strTmp.size() || bAllowBlankString ) vToken.push_back( strTmp );
						strTmp.erase( strTmp.begin(), strTmp.end() );
						continue;
					}
				}

				strTmp += pszCmd[i];
				bBackSlash = false;
			}
		}
		
		// 2010.04.14  bintitle
		// 기존 split() 함수에서는 공백문자를 무시하므로 추가.
		template< typename _Elem, typename _Container >
		inline void split_Ex( const _Elem* pszCommand, _Container& vToken, const _Elem* pDelimiterList, bool bProcSpecialCharacter = true, bool bAllowBlankString = false )
		{
			if ( !pszCommand ) return;

			std::basic_string< _Elem > strTmp;
			
			bool bBackSlash = false;
			bool bQuotationMark = false;

			//_Elem newCmd[512];
			_Elem* pszCmd = const_cast< _Elem* >( pszCommand );
			unsigned len = (unsigned)strlen(pszCmd)+1;
			
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
						( !bQuotationMark && (is_delimiter(pszCmd[i], pDelimiterList) ) ) || 	// 스페이스로 파싱중일때 스페이스가 나왔거나
						( pszCmd[i] == '\0'					) )			// 문장의 끝일경우
						 
						 
					{
						if ( pszCmd[i] == '\"' )
						{
							bQuotationMark ^= true;				// 따옴표가 나오면 토글
							if (bQuotationMark) continue;
						}
					
						// 수정 2010.04.14  bintitle. 
						// 기존 코드는 공백 토큰을 무시하고있다. 
						// 공백토큰도 vToken 배열에 추가하도록 수정.   
						vToken.push_back( strTmp ); 																	
						strTmp.erase( strTmp.begin(), strTmp.end() );
						continue;
					}
				}

				strTmp += pszCmd[i];
				bBackSlash = false;
			}
		}

		template< typename _Elem >
		inline std::vector< std::basic_string< _Elem > > split( const _Elem* pszCommand, const _Elem* pDelimiterList, bool bProcSpecialCharacter = true, bool bAllowBlankString = false )
		{
			std::vector< std::basic_string< _Elem > > tmp;
			split( pszCommand, tmp, pDelimiterList, bProcSpecialCharacter, bAllowBlankString );
			return tmp;
		}
		
		template< typename _Elem >
		inline std::vector< std::basic_string< _Elem > > split_Ex( const _Elem* pszCommand, const _Elem* pDelimiterList, bool bProcSpecialCharacter = true, bool bAllowBlankString = false )
		{
			std::vector< std::basic_string< _Elem > > tmp;
			split_Ex( pszCommand, tmp, pDelimiterList, bProcSpecialCharacter, bAllowBlankString );
			return tmp;
		}

		inline std::string insert_comma( int nValue )
		{
			const int c_nCommaGap = 3;

			std::string strNum = itos< char >( nValue );
			std::string strCommaNum;

			int nCount( 0 );
			int nCommaPos = int(strNum.size()) % c_nCommaGap;

			for ( std::string::iterator it = strNum.begin(); it != strNum.end(); ++it, ++nCount )
			{
				if ( it != strNum.begin() && nCommaPos == (nCount % c_nCommaGap) )
					strCommaNum.push_back( ',' );

				strCommaNum.push_back( (*it) );
			}
			return strCommaNum;
		}

		inline std::wstring insert_commaw( int nValue )
		{
			const int c_nCommaGap = 3;

			std::wstring strNum = itosw( nValue );
			std::wstring strCommaNum;

			int nCount( 0 );
			int nCommaPos = int(strNum.size()) % c_nCommaGap;

			for ( std::wstring::iterator it = strNum.begin(); it != strNum.end(); ++it, ++nCount )
			{
				if ( it != strNum.begin() && nCommaPos == (nCount % c_nCommaGap) )
					strCommaNum.push_back( ',' );

				strCommaNum.push_back( (*it) );
			}
			return strCommaNum;
		}

		template< typename _Elem >
		inline std::basic_string< _Elem > insert_comma( int nValue )				{ return insert_comma( nValue ); }
		template<> inline std::basic_string< char > insert_comma( int nValue )		{ return insert_comma( nValue ); }
		template<> inline std::basic_string< wchar_t > insert_comma( int nValue )	{ return insert_commaw( nValue ); }

		namespace path
		{
			inline char get_path_separator() { return '/'; }

			template< typename _Elem >
			inline std::basic_string< _Elem >	get_file_name( const std::basic_string< _Elem >& path, char path_separator = get_path_separator() )
			{
				size_t pos = path.rfind( path_separator );
				if ( pos == path.npos ) return path;

				std::basic_string< _Elem > file_name( path.begin() + pos + 1, path.end() );
				return file_name;
			}

			template< typename _Elem >
			inline std::basic_string< _Elem >	get_file_name( const _Elem* path )
			{
				return get_file_name( std::basic_string< _Elem >( path ) );
			}

			template< typename _Elem >
			inline std::basic_string< _Elem >	get_directory_name( const std::basic_string< _Elem >& path, char path_separator = get_path_separator() )
			{
				size_t pos = path.rfind( path_separator );
				if ( pos == path.npos ) return path;

				std::basic_string< _Elem > file_name( path.begin(), path.begin() + pos );
				return file_name;
			}

			template< typename _Elem >
			inline std::basic_string< _Elem >	get_directory_name( const _Elem* path )
			{
				return get_directory_name( std::basic_string< _Elem >( path ) );
			}

			template< typename _Elem >
			inline std::basic_string< _Elem >	get_extension( const std::basic_string< _Elem >& path, char path_separator = get_path_separator() )
			{
				std::basic_string< _Elem > temp;
				size_t bpos = path.rfind( path_separator );
				if ( bpos == path.npos ) temp = path;
				else temp.assign( path.begin() + bpos, path.end() );
				
				size_t pos = temp.rfind( '.' );
				if ( pos == temp.npos ) return "";

				std::basic_string< _Elem > extension( temp.begin() + pos, temp.end() );
				return extension;
			}

			template< typename _Elem >
			inline std::basic_string< _Elem >	get_extension( const _Elem* path )
			{
				return get_extension( std::basic_string< _Elem >( path ) );
			}

			template< typename _Elem >
			inline std::basic_string< _Elem >	trim_extension( const std::basic_string< _Elem >& path )
			{
				std::basic_string< _Elem > extension = get_extension( path );
				
				std::basic_string< _Elem > new_path( path.begin(), path.end() - extension.size() );
				return new_path;
			}

			template< typename _Elem >
			inline std::basic_string< _Elem >	get_file_name_without_extension( const std::basic_string< _Elem >& path )
			{
				return get_file_name( trim_extension( path ) );
			}

			template< typename _Elem >
			inline void normalize_( std::basic_string< _Elem >& path, _Elem path_separator = get_path_separator() )
			{
				_Elem sep[2] = { '/', 0 };
				_Elem to[2]  = { path_separator, 0 };

				if ( path_separator == '/' ) sep[0] = '\\';

				nsl::replace( path, sep, to );
			}

			template< typename _Elem >
			inline std::basic_string< _Elem > normalize( const std::basic_string< _Elem >& path )
			{
				std::basic_string< _Elem > tmp = path;
				normalize_( tmp );
				return tmp;
			}

			template< typename _Elem >
			inline std::basic_string< _Elem > normalize( const _Elem* path, _Elem path_separator = get_path_separator() )
			{
				std::basic_string< _Elem > tmp = path;
				normalize_( tmp, path_separator );
				return tmp;
			}

			template< typename _Elem >
			inline std::basic_string< _Elem > join( const std::basic_string< _Elem >& p1, const std::basic_string< _Elem >& p2 )
			{
				int path_separator = get_path_separator();

				if ( p1.empty() ) return p2;
				if ( p2.empty() ) return p1;

				bool bP1 = ( p1[ p1.size() - 1 ] == path_separator );
				bool bP2 = ( p2[ 0 ] == path_separator );

				std::basic_string< _Elem > temp( p1 );

				if ( bP1 && bP2 )
				{
					temp.erase( temp.end() - 1, temp.end() );
					temp.insert( temp.end(), p2.begin(), p2.end() );
					return temp;
				}

				if ( !bP1 && !bP2 ) temp += '/';

				temp += p2;

				return normalize( temp );
			}

			template< typename _Elem >
			inline std::basic_string< _Elem > join( const _Elem * p1, const _Elem * p2 )
			{
				return join( std::basic_string< _Elem >( p1 ), std::basic_string< _Elem >( p2 ) );
			}

			template< typename _Elem >
			inline std::basic_string< _Elem > join( const std::basic_string< _Elem >& p1, const std::basic_string< _Elem >& p2, const std::basic_string< _Elem >& p3 )
			{
				return join( join( p1, p2 ), p3 );
			}

			template< typename _Elem >
			inline std::basic_string< _Elem > join( const _Elem * p1, const _Elem * p2, const _Elem * p3 )
			{
				return join( std::basic_string< _Elem >( p1 ), std::basic_string< _Elem >( p2 ), std::basic_string< _Elem >( p3 ) );
			}

			template< typename _Elem >
			inline std::basic_string< _Elem > join( const std::basic_string< _Elem >& p1, const std::basic_string< _Elem >& p2, const std::basic_string< _Elem >& p3, const std::basic_string< _Elem >& p4 )
			{
				return join( join( p1, p2, p3 ), p4 );
			}

			template< typename _Elem >
			inline std::basic_string< _Elem > join( const _Elem * p1, const _Elem * p2, const _Elem * p3, const _Elem * p4 )
			{
				return join( std::basic_string< _Elem >( p1 ), std::basic_string< _Elem >( p2 ), std::basic_string< _Elem >( p3 ), std::basic_string< _Elem >( p4 ) );
			}
		};
	};
};



// pointer out-parameter version. deprecated for inline inefficiency - by Tyburn


namespace nl
{
	namespace str
	{
		template< typename _Elem >
		inline void sprintf_str( std::basic_string< _Elem > * str, const _Elem* fmt, ... )
		{
			_Elem buf[128];
			va_list args;
			va_start( args, fmt );

			_Elem* p = buf;
			size_t buf_len = sizeof(buf)/sizeof(_Elem);

			while ( -1 == s_vsprintf( p, buf_len, fmt, args ) )
			{
				if ( p != buf ) delete [] p;

				buf_len *= 2;

				p = new _Elem[ buf_len ];
			}

			*str = p;

			if ( p != buf ) delete [] p;
		}

		template< typename _Elem >
		inline size_t replace( std::basic_string< _Elem >* str, const _Elem* from, size_t from_len, const _Elem* to, size_t to_len, size_t max_cnt = 0xffffffff )
		{
			return replace( *str, from, from_len, to, to_len, max_cnt );
		}

		template< typename _Elem >
		inline size_t replace( std::basic_string< _Elem >* str, const _Elem* from, const _Elem* to, size_t max_cnt = 0xffffffff )
		{
			return replace( *str, from, to, max_cnt );
		}

		template< typename _Elem >
		inline size_t replace( std::basic_string< _Elem >* str, const std::basic_string< _Elem >& from, const std::basic_string< _Elem >& to, size_t max_cnt = 0xffffffff )
		{
			return replace( str, from.c_str(), from.size(), to.c_str(), to.size(), max_cnt );
		}

		template< typename _Elem >
		inline void toupper( std::basic_string< _Elem >* str  )
		{
			toupper( *str );
		}

		template< typename _Elem >
		inline void tolower( std::basic_string< _Elem >* str  )
		{
			tolower( *str );
		}

		template< typename _Elem, typename _Container >
		inline void split( const _Elem* pszCommand, _Container* vToken, const _Elem* pDelimiterList, bool bProcSpecialCharacter = true, bool bAllowBlankString = false )
		{
			split( pszCommand, *vToken, pDelimiterList, bProcSpecialCharacter, bAllowBlankString );
		}

		template< typename _Elem, typename _Container >
		inline void split_with_string( const _Elem* pszCommand, _Container* vToken, const _Elem* pSeparatorString, bool bAllowBlankString = false )
		{
			split_with_string( pszCommand, *vToken, pSeparatorString, bAllowBlankString );
		}

		namespace path
		{
			template< typename _Elem >
			inline void normalize( std::basic_string< _Elem >* path, char path_separator = get_path_separator() )
			{
				normalize_( *path, path_separator );
			}
		};

		template< typename _Elem >
		void url_encode( std::basic_string< _Elem > *pString  )
		{			
			for( size_t i = 0; i < pString->size(); ++i )
			{
				bool bEncode = false;

				int nChar = (*pString)[i];

				// ASCII Control characters
				if( ( nChar >= 0x00 && nChar <= 0x1F ) || ( nChar == 0x7F ) ) bEncode = true;
					
				// Non-ASCII characters
				else if( nChar >= 0x80 && nChar <= 0xFF ) bEncode = true;

				// Resserved characters
				else if( ( nChar == 0x24 ) || ( nChar == 0x26 ) ||	
						 ( nChar == 0x2B ) || ( nChar == 0x2C ) ||
						 ( nChar == 0x2F ) || ( nChar == 0x3A ) ||
						 ( nChar == 0x3B ) || ( nChar == 0x3D ) ||
						 ( nChar == 0x3F ) || ( nChar == 0x40 ) ) bEncode = true;
		
				// Unsafe characters
				else if( ( nChar == 0x20 ) || ( nChar == 0x22 ) ||
						 ( nChar == 0x3C ) || ( nChar == 0x3E ) ||
						 ( nChar == 0x23 ) || ( nChar == 0x25 ) ) bEncode = true;

				// Misc
				else if( ( nChar == 0x7B ) || ( nChar == 0x7D ) ||
						 ( nChar == 0x7C ) || ( nChar == 0x5C ) ||
						 ( nChar == 0x5E ) || ( nChar == 0x7E ) ||
						 ( nChar == 0x5B ) || ( nChar == 0x5D ) ||
						 ( nChar == 0x60 ) ) bEncode = true;

				if( bEncode )
				{
					(*pString)[i++] = '%';
					int nHigh = nChar / 16;
					int nLow  = nChar % 16;	
					nHigh = nHigh > 9 ? 'A' + nHigh - 10 : '0' + nHigh;
					nLow = nLow > 9 ? 'A' + nLow - 10 : '0' + nLow;
					pString->insert( i++, 1, nHigh );
					pString->insert( i, 1, nLow );
				}
			}
		}
	};
};
	
namespace nsl	{ using namespace nl::str;	};
