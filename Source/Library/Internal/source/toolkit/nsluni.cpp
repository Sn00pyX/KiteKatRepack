#include "../../include/toolkit/nsl.h"
#include "../../include/toolkit/nsluni.h"
#include <iconv/iconv.h>
#include <vector>

namespace nsl
{

	namespace uni
	{

		static		iconv_t	s_iconv_from_encoding = (iconv_t)-1;
		static		iconv_t	s_iconv_to_encoding = (iconv_t)-1;
		static		iconv_t	s_iconv_kor_from_encoding = (iconv_t)-1;
		static		iconv_t	s_iconv_kor_to_encoding = (iconv_t)-1;

		std::string s_encoding;
		std::string s_resource_encoding;	/// 2011.06.15 리소스용 인코딩 - prodongi

		bool init( const char *default_encoding )
		{
			s_iconv_kor_from_encoding = iconv_open( "UTF-16LE", "CP949" );
			s_iconv_kor_to_encoding   = iconv_open( "CP949", "UTF-16LE" );

			return set_default_encoding( default_encoding );
		}

		bool deinit()
		{
			if( s_iconv_kor_from_encoding != s_iconv_from_encoding && s_iconv_from_encoding != (iconv_t)-1 )	iconv_close( s_iconv_from_encoding );
			if( s_iconv_kor_to_encoding != s_iconv_to_encoding && s_iconv_to_encoding != (iconv_t)-1 )			iconv_close( s_iconv_to_encoding );
			if( s_iconv_kor_from_encoding != (iconv_t)-1 )	iconv_close( s_iconv_kor_from_encoding );
			if( s_iconv_kor_to_encoding != (iconv_t)-1 )	iconv_close( s_iconv_kor_to_encoding );

			return true;
		}

		const wchar_t* _getPostfix(const std::wstring & josa, bool jong)
		{
			// jong : true면 받침있음, false면 받침없음
		    
			if ( !_wcsnicmp( josa.c_str(), L"(을/를)", 5 ) ) return (jong?L"을":L"를");
			if ( !_wcsnicmp( josa.c_str(), L"(이/가)", 5 ) ) return (jong?L"이":L"가");
			if ( !_wcsnicmp( josa.c_str(), L"(은/는)", 5 ) ) return (jong?L"은":L"는");
			if ( !_wcsnicmp( josa.c_str(), L"(와/과)", 5 ) ) return (jong?L"과":L"와");
			if ( !_wcsnicmp( josa.c_str(), L"(/으)", 4 ) ) return (jong?L"으":L"");
			if ( !_wcsnicmp( josa.c_str(), L"(/이)", 4 ) ) return (jong?L"이":L"");

			// 알 수 없는 조사
			return josa.c_str();
		}

		const wchar_t* getPostFix( const wchar_t *szString, const wchar_t* szPostfix )
		{
			if( !szString || !szString[0] ) return L"";
			bool bHas = true;

			wchar_t c = szString[0];

			if( c >= '0' && c <= '9' )
			{
				if(	c == '1' ||
					c == '3' ||
					c == '6' ||
					c == '7' ||
					c == '8' ||
					c == '0' ) bHas = true;
				else bHas = false;
			}

			if( c >= 'A' && c <= 'Z' ) c -= static_cast< wchar_t >( ( 'A' - 'a' ) );

			if( c >= 'a' && c <= 'z' ) 
			{
				if( c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y' ) bHas = false;
				else bHas = true;
			}

			int code = c - 44032;
			
			// 한글이 아닐때
			if (code >= 0 && code <= 11171)
			{
				if (code % 28 == 0) bHas = false;
				else bHas = true;
			}

			return _getPostfix(szPostfix, bHas);
		}

		void getPrevCharacter( const wchar_t * szString, size_t nPos, std::wstring & strChar )
		{
			wchar_t *p = const_cast< wchar_t * >( szString + nPos );
			while( p > szString )
			{
				--p;

				// 영어 혹은 숫자
				if( ( *p >= '0' && *p <= '9' ) ) // || ( *p >= 'a' && *p <= 'z' ) || ( *p >= 'A' && *p <= 'Z' ) ) 영어는 무시해 달라는 구랴..
				{
					strChar = *p;
					return;
				}

				// 한글
				if( ( *p >= 0xAC00 && *p <= 0xD7A3 ) || ( *p >= 0x3131 && *p <= 0x31FE ) )
				{
					strChar.assign( p, 1 );
					return;
				}
			}
		}

		bool proc_korean_postfix( std::wstring & strString )
		{
			bool bRtn = false;

			while( true )
			{
				size_t tag_len = 0;

				tag_len = 5;
				size_t pos = strString.find( L"(을/를)" );
				if( pos == strString.npos ) pos = strString.find( L"(이/가)" );
				if( pos == strString.npos ) pos = strString.find( L"(은/는)" );
				if( pos == strString.npos ) pos = strString.find( L"(와/과)" );
				if( pos == strString.npos ) 
				{
					pos = strString.find( L"(/으)" );
					if( pos != strString.npos ) tag_len = 4;
				}
				if( pos == strString.npos ) 
				{
					pos = strString.find( L"(/이)" );
					if( pos != strString.npos ) tag_len = 4;
				}		
				if( pos == strString.npos ) break;

				bRtn = true;

				std::wstring strChar;
				getPrevCharacter( strString.c_str(), pos, strChar );
			
				const wchar_t *szPostfix = getPostFix( strChar.c_str(), strString.c_str() + pos );
				strString.replace( strString.begin() + pos, strString.begin() + pos + tag_len, szPostfix, szPostfix + wcslen( szPostfix ) );
			}

			return bRtn;
		}

		bool set_default_encoding( const char *encoding )
		{
			if( s_iconv_kor_from_encoding == (iconv_t)-1 ) return init( encoding );

			if( s_iconv_from_encoding != s_iconv_kor_from_encoding && s_iconv_from_encoding != (iconv_t)-1 ) iconv_close( s_iconv_from_encoding );
			if( s_iconv_to_encoding != s_iconv_kor_to_encoding && s_iconv_to_encoding != (iconv_t)-1 ) iconv_close( s_iconv_to_encoding );

			if( _stricmp( encoding, "CP949" ) == 0 )
			{
				s_iconv_from_encoding = s_iconv_kor_from_encoding;
				s_iconv_to_encoding = s_iconv_kor_to_encoding;
			}
			else
			{
				s_iconv_to_encoding   = iconv_open( encoding, "UTF-16LE" );
				s_iconv_from_encoding = iconv_open( "UTF-16LE", encoding );
			}

			if( s_iconv_from_encoding != (iconv_t)-1 && s_iconv_to_encoding != (iconv_t)-1 )
			{	
				s_encoding = encoding;
				nsl::toupper( &s_encoding );
				return true;
			}

			return false;
		}

		const char* get_default_encoding()
		{
			if( s_encoding.empty() ) return NULL;
			return s_encoding.c_str();
		}

		/// 2011.06.15 리소스용 인코딩 - prodongi
		void set_resource_encoding(char const* encoding)
		{
			s_resource_encoding = encoding;
		}

		char const* get_resource_encoding()
		{
			if (s_resource_encoding.empty())
				return get_default_encoding();
			return s_resource_encoding.c_str();
		}

		bool conv( iconv_t from, const std::string & source,  std::wstring & target )
		{
			size_t	from_len		= source.size();
			std::vector< wchar_t >	vResult;
			const char*	inptr = source.c_str();
			vResult.resize( from_len + 1 );

			target.clear();
			
			while( true )
			{		
				char*	outptr = (char*)&vResult[0];
				size_t	to_len = vResult.capacity() * 2;
				int rtn = (int)iconv( s_iconv_from_encoding, (const char **)&inptr, &from_len, &outptr, &to_len );
				if( rtn == EILSEQ || rtn == EINVAL ) return false;

				if( rtn == E2BIG ) 
				{
					vResult.resize( vResult.capacity() * 2 );
					continue;
				}
				
				target.append( &vResult.front(), &vResult.front() + ( vResult.size() - to_len/2 ) );
				break;
			}

			return true;
		}

		bool conv( iconv_t to, const std::wstring & source, std::string & target  )
		{
			size_t				from_len = source.size()*2;
			const char*			inptr =	(const char*)source.c_str();
			std::vector< char >	vResult;
			vResult.resize( from_len + 1 );

			target.clear();
			
			while( true )
			{		
				char*	outptr = (char*)&vResult[0];
				size_t	to_len = vResult.capacity();
				int rtn = (int)iconv( to, (const char **)&inptr, &from_len, &outptr, &to_len );
				if( rtn == EILSEQ || rtn == EINVAL ) return false;

				if( rtn == E2BIG ) 
				{
					vResult.resize( vResult.capacity() * 2 );
					continue;
				}
				
				target.append( &vResult.front(), &vResult.front() + ( vResult.size() - to_len ) );
				break;
			}

			return true;
		}

		bool conv( const char *source_encoding, const char *target_encoding, void* source, size_t source_len, std::vector< char > * pvResult )
		{
			size_t				from_len = source_len;
			const char*			inptr =	(const char*)source;

			std::vector< char > & vResult = *pvResult;
			vResult.resize( from_len + 1 );

			// vResult.clear();

			libiconv_t encoding   = iconv_open( target_encoding, source_encoding );
			
			while( true )
			{		
				char*	outptr = (char*)&vResult[0];
				size_t	to_len = vResult.capacity();
				int rtn = (int)iconv( encoding, (const char **)&inptr, &from_len, &outptr, &to_len );
				if( rtn == EILSEQ || rtn == EINVAL ) return false;

				if( rtn == E2BIG ) 
				{
					vResult.resize( vResult.capacity() * 2 );
					continue;
				}
				
				vResult.resize( vResult.size() - to_len );
				break;
			}

			return true;
		}

		bool			conv( const std::string & source,  std::wstring & target ) { return conv( s_iconv_from_encoding, source, target ); }
		bool			conv( const std::wstring & source, std::string & target  ) { return conv( s_iconv_to_encoding, source, target ); }

		std::wstring	conv( const std::string & source  )	{ std::wstring tmp; conv( s_iconv_from_encoding, source, tmp ); return tmp; }
		std::string		conv( const std::wstring & source )	{ std::string tmp;  conv( s_iconv_to_encoding, source, tmp ); return tmp; }

		bool			conv( const char *encoding, const std::string & source,  std::wstring & target )
		{
			iconv_t from = iconv_open( encoding, "UTF-16LE" );
			if( from < 0 ) return false;
			bool bRtn = conv( from, source, target );
			iconv_close( from );
			return bRtn;
		}

		bool			conv( const char *encoding, const std::wstring & source, std::string & target  )
		{
			iconv_t to = iconv_open( "UTF-16LE", encoding );
			if( to < 0 ) return false;
			bool bRtn = conv( to, source, target );
			iconv_close( to );
			return bRtn;
		}

		bool proc_korean_postfix( std::string & strString  )
		{
			if( s_encoding != "CP949" ) return false;
			std::wstring strTemp;
			conv( s_iconv_kor_from_encoding, strString, strTemp );
			bool bRtn = proc_korean_postfix( &strTemp );
			conv( s_iconv_kor_to_encoding, strTemp, strString );
			return bRtn;
		}

		CodeGroup get_unicode_group( wchar_t c )
		{
			// latin
			if( c >= 0x0041 && c < 0x005B ) return T_LAT;
			if( c >= 0x0061 && c < 0x007B ) return T_LAT;
			if( c >= 0x00C0 && c < 0x00D7 ) return T_LAT;
			if( c >= 0x00D8 && c < 0x00F7 ) return T_LAT;
			if( c >= 0x00F8 && c < 0x0220 ) return T_LAT;

			// korean
			if( c >= 0x1100 && c < 0x115A ) return T_HAN;
			if( c >= 0x115F && c < 0x11A3 ) return T_HAN;
			if( c >= 0x11A8 && c < 0x11FA ) return T_HAN;
			if( c >= 0x302E && c < 0x3030 ) return T_HAN;
			if( c >= 0x3131 && c < 0x318F ) return T_HAN;
			if( c >= 0xAC00 && c < 0xD7A4 ) return T_HAN;

			// japanese
			if( c >= 0x3041 && c < 0x3095 ) return T_JPN;
			if( c >= 0x309D && c < 0x309F ) return T_JPN;
			if( c >= 0x30A1 && c < 0x30FC ) return T_JPN;
			if( c >= 0x30FD && c < 0x30FF ) return T_JPN;

			if( c < 0x000A ) return T_CTK;
			if( c < 0x000B ) return T_BLN;
			if( c < 0x000D ) return T_CTK;
			if( c < 0x000E ) return T_BLN;
			if( c < 0x0020 ) return T_CTK;
			if( c < 0x0021 ) return T_BLN;
			if( c < 0x0030 ) return T_SPC;
			if( c < 0x003A ) return T_DIG;
			if( c < 0x0041 ) return T_SPC;
			if( c < 0x005B ) return T_LAT;
			if( c < 0x0061 ) return T_SPC;
			if( c < 0x007B ) return T_LAT;
			if( c < 0x007F ) return T_SPC;
			if( c < 0x00A0 ) return T_CTK;
			if( c < 0x00A1 ) return T_BLN;
			if( c < 0x00C0 ) return T_SPC;
			if( c < 0x00D7 ) return T_LAT;
			if( c < 0x00D8 ) return T_SPC;
			if( c < 0x00F7 ) return T_LAT;
			if( c < 0x00F8 ) return T_SPC;
			if( c < 0x0220 ) return T_LAT;
			if( c < 0x0222 ) return T_RES;
			if( c < 0x0234 ) return T_LAT;
			if( c < 0x0250 ) return T_RES;
			if( c < 0x02AE ) return T_LAT;
			if( c < 0x02B0 ) return T_RES;
			if( c < 0x02EF ) return T_SYM;
			if( c < 0x0300 ) return T_RES;
			if( c < 0x034F ) return T_SYM;
			if( c < 0x0360 ) return T_RES;
			if( c < 0x0363 ) return T_SYM;
			if( c < 0x0374 ) return T_RES;
			if( c < 0x0376 ) return T_GRE;
			if( c < 0x037A ) return T_RES;
			if( c < 0x037B ) return T_GRE;
			if( c < 0x037E ) return T_RES;
			if( c < 0x037F ) return T_GRE;
			if( c < 0x0384 ) return T_RES;
			if( c < 0x038B ) return T_GRE;
			if( c < 0x038C ) return T_RES;
			if( c < 0x038D ) return T_GRE;
			if( c < 0x038E ) return T_RES;
			if( c < 0x03A2 ) return T_GRE;
			if( c < 0x03A3 ) return T_RES;
			if( c < 0x03CF ) return T_GRE;
			if( c < 0x03D0 ) return T_RES;
			if( c < 0x03D8 ) return T_GRE;
			if( c < 0x03DA ) return T_RES;
			if( c < 0x03E2 ) return T_GRE;
			if( c < 0x03F0 ) return T_COP;
			if( c < 0x03F4 ) return T_GRE;
			if( c < 0x0400 ) return T_RES;
			if( c < 0x0483 ) return T_CYR;
			if( c < 0x0487 ) return T_SYM;
			if( c < 0x0488 ) return T_RES;
			if( c < 0x048A ) return T_SYM;
			if( c < 0x048C ) return T_RES;
			if( c < 0x04C5 ) return T_CYR;
			if( c < 0x04C7 ) return T_RES;
			if( c < 0x04C9 ) return T_CYR;
			if( c < 0x04CB ) return T_RES;
			if( c < 0x04CD ) return T_CYR;
			if( c < 0x04D0 ) return T_RES;
			if( c < 0x04F6 ) return T_CYR;
			if( c < 0x04F8 ) return T_RES;
			if( c < 0x04FA ) return T_CYR;
			if( c < 0x0531 ) return T_RES;
			if( c < 0x0557 ) return T_ARM;
			if( c < 0x0559 ) return T_RES;
			if( c < 0x0560 ) return T_ARM;
			if( c < 0x0561 ) return T_RES;
			if( c < 0x0588 ) return T_ARM;
			if( c < 0x0589 ) return T_RES;
			if( c < 0x058B ) return T_ARM;
			if( c < 0x0591 ) return T_RES;
			if( c < 0x05A2 ) return T_HEB;
			if( c < 0x05A3 ) return T_RES;
			if( c < 0x05BA ) return T_HEB;
			if( c < 0x05BB ) return T_RES;
			if( c < 0x05C5 ) return T_HEB;
			if( c < 0x05D0 ) return T_RES;
			if( c < 0x05EB ) return T_HEB;
			if( c < 0x05F0 ) return T_RES;
			if( c < 0x05F5 ) return T_HEB;
			if( c < 0x060C ) return T_RES;
			if( c < 0x060D ) return T_ARA;
			if( c < 0x061B ) return T_RES;
			if( c < 0x061C ) return T_ARA;
			if( c < 0x061F ) return T_RES;
			if( c < 0x0620 ) return T_ARA;
			if( c < 0x0621 ) return T_RES;
			if( c < 0x063B ) return T_ARA;
			if( c < 0x0640 ) return T_RES;
			if( c < 0x0656 ) return T_ARA;
			if( c < 0x0660 ) return T_RES;
			if( c < 0x066E ) return T_ARA;
			if( c < 0x0670 ) return T_RES;
			if( c < 0x06EE ) return T_ARA;
			if( c < 0x06F0 ) return T_RES;
			if( c < 0x06FF ) return T_ARA;
			if( c < 0x0700 ) return T_RES;
			if( c < 0x070E ) return T_SYR;
			if( c < 0x070F ) return T_RES;
			if( c < 0x072D ) return T_SYR;
			if( c < 0x0730 ) return T_RES;
			if( c < 0x074B ) return T_SYR;
			if( c < 0x0780 ) return T_RES;
			if( c < 0x07B1 ) return T_THN;
			if( c < 0x0901 ) return T_RES;
			if( c < 0x0904 ) return T_DEV;
			if( c < 0x0905 ) return T_RES;
			if( c < 0x093A ) return T_DEV;
			if( c < 0x093C ) return T_RES;
			if( c < 0x094E ) return T_DEV;
			if( c < 0x0950 ) return T_RES;
			if( c < 0x0955 ) return T_DEV;
			if( c < 0x0958 ) return T_RES;
			if( c < 0x0971 ) return T_DEV;
			if( c < 0x0981 ) return T_RES;
			if( c < 0x0984 ) return T_BNG;
			if( c < 0x0985 ) return T_RES;
			if( c < 0x098D ) return T_BNG;
			if( c < 0x098F ) return T_RES;
			if( c < 0x0991 ) return T_BNG;
			if( c < 0x0993 ) return T_RES;
			if( c < 0x09A9 ) return T_BNG;
			if( c < 0x09AA ) return T_RES;
			if( c < 0x09B1 ) return T_BNG;
			if( c < 0x09B2 ) return T_RES;
			if( c < 0x09B3 ) return T_BNG;
			if( c < 0x09B6 ) return T_RES;
			if( c < 0x09BA ) return T_BNG;
			if( c < 0x09BC ) return T_RES;
			if( c < 0x09BD ) return T_BNG;
			if( c < 0x09BE ) return T_RES;
			if( c < 0x09C5 ) return T_BNG;
			if( c < 0x09C7 ) return T_RES;
			if( c < 0x09C9 ) return T_BNG;
			if( c < 0x09CB ) return T_RES;
			if( c < 0x09CE ) return T_BNG;
			if( c < 0x09D7 ) return T_RES;
			if( c < 0x09D8 ) return T_BNG;
			if( c < 0x09DC ) return T_RES;
			if( c < 0x09DE ) return T_BNG;
			if( c < 0x09DF ) return T_RES;
			if( c < 0x09E4 ) return T_BNG;
			if( c < 0x09E6 ) return T_RES;
			if( c < 0x09FB ) return T_BNG;
			if( c < 0x0A02 ) return T_RES;
			if( c < 0x0A03 ) return T_GUR;
			if( c < 0x0A05 ) return T_RES;
			if( c < 0x0A0B ) return T_GUR;
			if( c < 0x0A0F ) return T_RES;
			if( c < 0x0A11 ) return T_GUR;
			if( c < 0x0A13 ) return T_RES;
			if( c < 0x0A29 ) return T_GUR;
			if( c < 0x0A2A ) return T_RES;
			if( c < 0x0A31 ) return T_GUR;
			if( c < 0x0A32 ) return T_RES;
			if( c < 0x0A34 ) return T_GUR;
			if( c < 0x0A35 ) return T_RES;
			if( c < 0x0A37 ) return T_GUR;
			if( c < 0x0A38 ) return T_RES;
			if( c < 0x0A3A ) return T_GUR;
			if( c < 0x0A3C ) return T_RES;
			if( c < 0x0A3D ) return T_GUR;
			if( c < 0x0A3E ) return T_RES;
			if( c < 0x0A43 ) return T_GUR;
			if( c < 0x0A47 ) return T_RES;
			if( c < 0x0A49 ) return T_GUR;
			if( c < 0x0A4B ) return T_RES;
			if( c < 0x0A4E ) return T_GUR;
			if( c < 0x0A59 ) return T_RES;
			if( c < 0x0A5D ) return T_GUR;
			if( c < 0x0A5E ) return T_RES;
			if( c < 0x0A5F ) return T_GUR;
			if( c < 0x0A66 ) return T_RES;
			if( c < 0x0A75 ) return T_GUR;
			if( c < 0x0A81 ) return T_RES;
			if( c < 0x0A84 ) return T_GUJ;
			if( c < 0x0A85 ) return T_RES;
			if( c < 0x0A8C ) return T_GUJ;
			if( c < 0x0A8D ) return T_RES;
			if( c < 0x0A8E ) return T_GUJ;
			if( c < 0x0A8F ) return T_RES;
			if( c < 0x0A92 ) return T_GUJ;
			if( c < 0x0A93 ) return T_RES;
			if( c < 0x0AA9 ) return T_GUJ;
			if( c < 0x0AAA ) return T_RES;
			if( c < 0x0AB1 ) return T_GUJ;
			if( c < 0x0AB2 ) return T_RES;
			if( c < 0x0AB4 ) return T_GUJ;
			if( c < 0x0AB5 ) return T_RES;
			if( c < 0x0ABA ) return T_GUJ;
			if( c < 0x0ABC ) return T_RES;
			if( c < 0x0AC6 ) return T_GUJ;
			if( c < 0x0AC7 ) return T_RES;
			if( c < 0x0ACA ) return T_GUJ;
			if( c < 0x0ACB ) return T_RES;
			if( c < 0x0ACE ) return T_GUJ;
			if( c < 0x0AD0 ) return T_RES;
			if( c < 0x0AD1 ) return T_GUJ;
			if( c < 0x0AE0 ) return T_RES;
			if( c < 0x0AE1 ) return T_GUJ;
			if( c < 0x0AE6 ) return T_RES;
			if( c < 0x0AF0 ) return T_GUJ;
			if( c < 0x0B01 ) return T_RES;
			if( c < 0x0B04 ) return T_ORI;
			if( c < 0x0B05 ) return T_RES;
			if( c < 0x0B0D ) return T_ORI;
			if( c < 0x0B0F ) return T_RES;
			if( c < 0x0B11 ) return T_ORI;
			if( c < 0x0B13 ) return T_RES;
			if( c < 0x0B29 ) return T_ORI;
			if( c < 0x0B2A ) return T_RES;
			if( c < 0x0B31 ) return T_ORI;
			if( c < 0x0B32 ) return T_RES;
			if( c < 0x0B34 ) return T_ORI;
			if( c < 0x0B36 ) return T_RES;
			if( c < 0x0B3A ) return T_ORI;
			if( c < 0x0B3C ) return T_RES;
			if( c < 0x0B44 ) return T_ORI;
			if( c < 0x0B47 ) return T_RES;
			if( c < 0x0B49 ) return T_ORI;
			if( c < 0x0B4B ) return T_RES;
			if( c < 0x0B4E ) return T_ORI;
			if( c < 0x0B56 ) return T_RES;
			if( c < 0x0B58 ) return T_ORI;
			if( c < 0x0B5C ) return T_RES;
			if( c < 0x0B5E ) return T_ORI;
			if( c < 0x0B5F ) return T_RES;
			if( c < 0x0B62 ) return T_ORI;
			if( c < 0x0B66 ) return T_RES;
			if( c < 0x0B71 ) return T_ORI;
			if( c < 0x0B82 ) return T_RES;
			if( c < 0x0B84 ) return T_TAM;
			if( c < 0x0B85 ) return T_RES;
			if( c < 0x0B8B ) return T_TAM;
			if( c < 0x0B8E ) return T_RES;
			if( c < 0x0B91 ) return T_TAM;
			if( c < 0x0B92 ) return T_RES;
			if( c < 0x0B96 ) return T_TAM;
			if( c < 0x0B99 ) return T_RES;
			if( c < 0x0B9B ) return T_TAM;
			if( c < 0x0B9C ) return T_RES;
			if( c < 0x0B9D ) return T_TAM;
			if( c < 0x0B9E ) return T_RES;
			if( c < 0x0BA0 ) return T_TAM;
			if( c < 0x0BA3 ) return T_RES;
			if( c < 0x0BA5 ) return T_TAM;
			if( c < 0x0BA8 ) return T_RES;
			if( c < 0x0BAB ) return T_TAM;
			if( c < 0x0BAE ) return T_RES;
			if( c < 0x0BB6 ) return T_TAM;
			if( c < 0x0BB7 ) return T_RES;
			if( c < 0x0BBA ) return T_TAM;
			if( c < 0x0BBE ) return T_RES;
			if( c < 0x0BC3 ) return T_TAM;
			if( c < 0x0BC6 ) return T_RES;
			if( c < 0x0BC9 ) return T_TAM;
			if( c < 0x0BCA ) return T_RES;
			if( c < 0x0BCE ) return T_TAM;
			if( c < 0x0BD7 ) return T_RES;
			if( c < 0x0BD8 ) return T_TAM;
			if( c < 0x0BE7 ) return T_RES;
			if( c < 0x0BF3 ) return T_TAM;
			if( c < 0x0C01 ) return T_RES;
			if( c < 0x0C04 ) return T_TEL;
			if( c < 0x0C05 ) return T_RES;
			if( c < 0x0C0D ) return T_TEL;
			if( c < 0x0C0E ) return T_RES;
			if( c < 0x0C11 ) return T_TEL;
			if( c < 0x0C12 ) return T_RES;
			if( c < 0x0C29 ) return T_TEL;
			if( c < 0x0C2A ) return T_RES;
			if( c < 0x0C34 ) return T_TEL;
			if( c < 0x0C35 ) return T_RES;
			if( c < 0x0C3A ) return T_TEL;
			if( c < 0x0C3E ) return T_RES;
			if( c < 0x0C45 ) return T_TEL;
			if( c < 0x0C46 ) return T_RES;
			if( c < 0x0C49 ) return T_TEL;
			if( c < 0x0C4A ) return T_RES;
			if( c < 0x0C4E ) return T_TEL;
			if( c < 0x0C55 ) return T_RES;
			if( c < 0x0C57 ) return T_TEL;
			if( c < 0x0C60 ) return T_RES;
			if( c < 0x0C62 ) return T_TEL;
			if( c < 0x0C66 ) return T_RES;
			if( c < 0x0C70 ) return T_TEL;
			if( c < 0x0C82 ) return T_RES;
			if( c < 0x0C84 ) return T_KAN;
			if( c < 0x0C85 ) return T_RES;
			if( c < 0x0C8D ) return T_KAN;
			if( c < 0x0C8E ) return T_RES;
			if( c < 0x0C91 ) return T_KAN;
			if( c < 0x0C92 ) return T_RES;
			if( c < 0x0CA9 ) return T_KAN;
			if( c < 0x0CAA ) return T_RES;
			if( c < 0x0CB4 ) return T_KAN;
			if( c < 0x0CB5 ) return T_RES;
			if( c < 0x0CBA ) return T_KAN;
			if( c < 0x0CBE ) return T_RES;
			if( c < 0x0CC5 ) return T_KAN;
			if( c < 0x0CC6 ) return T_RES;
			if( c < 0x0CC9 ) return T_KAN;
			if( c < 0x0CCA ) return T_RES;
			if( c < 0x0CCE ) return T_KAN;
			if( c < 0x0CD5 ) return T_RES;
			if( c < 0x0CD7 ) return T_KAN;
			if( c < 0x0CDE ) return T_RES;
			if( c < 0x0CDF ) return T_KAN;
			if( c < 0x0CE0 ) return T_RES;
			if( c < 0x0CE2 ) return T_KAN;
			if( c < 0x0CE6 ) return T_RES;
			if( c < 0x0CF0 ) return T_KAN;
			if( c < 0x0D02 ) return T_RES;
			if( c < 0x0D04 ) return T_MAL;
			if( c < 0x0D05 ) return T_RES;
			if( c < 0x0D0D ) return T_MAL;
			if( c < 0x0D0E ) return T_RES;
			if( c < 0x0D11 ) return T_MAL;
			if( c < 0x0D12 ) return T_RES;
			if( c < 0x0D29 ) return T_MAL;
			if( c < 0x0D2A ) return T_RES;
			if( c < 0x0D3A ) return T_MAL;
			if( c < 0x0D3E ) return T_RES;
			if( c < 0x0D44 ) return T_MAL;
			if( c < 0x0D46 ) return T_RES;
			if( c < 0x0D49 ) return T_MAL;
			if( c < 0x0D4A ) return T_RES;
			if( c < 0x0D4E ) return T_MAL;
			if( c < 0x0D57 ) return T_RES;
			if( c < 0x0D58 ) return T_MAL;
			if( c < 0x0D60 ) return T_RES;
			if( c < 0x0D62 ) return T_MAL;
			if( c < 0x0D66 ) return T_RES;
			if( c < 0x0D70 ) return T_MAL;
			if( c < 0x0D82 ) return T_RES;
			if( c < 0x0D84 ) return T_SIN;
			if( c < 0x0D85 ) return T_RES;
			if( c < 0x0D97 ) return T_SIN;
			if( c < 0x0D9A ) return T_RES;
			if( c < 0x0DB2 ) return T_SIN;
			if( c < 0x0DB3 ) return T_RES;
			if( c < 0x0DBC ) return T_SIN;
			if( c < 0x0DBD ) return T_RES;
			if( c < 0x0DBE ) return T_SIN;
			if( c < 0x0DC0 ) return T_RES;
			if( c < 0x0DC7 ) return T_SIN;
			if( c < 0x0DCA ) return T_RES;
			if( c < 0x0DCB ) return T_SIN;
			if( c < 0x0DCF ) return T_RES;
			if( c < 0x0DD5 ) return T_SIN;
			if( c < 0x0DD6 ) return T_RES;
			if( c < 0x0DD7 ) return T_SIN;
			if( c < 0x0DD8 ) return T_RES;
			if( c < 0x0DE0 ) return T_SIN;
			if( c < 0x0DF2 ) return T_RES;
			if( c < 0x0DF5 ) return T_SIN;
			if( c < 0x0E01 ) return T_RES;
			if( c < 0x0E3B ) return T_THI;
			if( c < 0x0E3F ) return T_RES;
			if( c < 0x0E5C ) return T_THI;
			if( c < 0x0E81 ) return T_RES;
			if( c < 0x0E83 ) return T_LAO;
			if( c < 0x0E84 ) return T_RES;
			if( c < 0x0E85 ) return T_LAO;
			if( c < 0x0E87 ) return T_RES;
			if( c < 0x0E89 ) return T_LAO;
			if( c < 0x0E8A ) return T_RES;
			if( c < 0x0E8B ) return T_LAO;
			if( c < 0x0E8D ) return T_RES;
			if( c < 0x0E8E ) return T_LAO;
			if( c < 0x0E94 ) return T_RES;
			if( c < 0x0E98 ) return T_LAO;
			if( c < 0x0E99 ) return T_RES;
			if( c < 0x0EA0 ) return T_LAO;
			if( c < 0x0EA1 ) return T_RES;
			if( c < 0x0EA4 ) return T_LAO;
			if( c < 0x0EA5 ) return T_RES;
			if( c < 0x0EA6 ) return T_LAO;
			if( c < 0x0EA7 ) return T_RES;
			if( c < 0x0EA8 ) return T_LAO;
			if( c < 0x0EAA ) return T_RES;
			if( c < 0x0EAC ) return T_LAO;
			if( c < 0x0EAD ) return T_RES;
			if( c < 0x0EBA ) return T_LAO;
			if( c < 0x0EBB ) return T_RES;
			if( c < 0x0EBE ) return T_LAO;
			if( c < 0x0EC0 ) return T_RES;
			if( c < 0x0EC5 ) return T_LAO;
			if( c < 0x0EC6 ) return T_RES;
			if( c < 0x0EC7 ) return T_LAO;
			if( c < 0x0EC8 ) return T_RES;
			if( c < 0x0ECE ) return T_LAO;
			if( c < 0x0ED0 ) return T_RES;
			if( c < 0x0EDA ) return T_LAO;
			if( c < 0x0EDC ) return T_RES;
			if( c < 0x0EDE ) return T_LAO;
			if( c < 0x0F00 ) return T_RES;
			if( c < 0x0F48 ) return T_TIB;
			if( c < 0x0F49 ) return T_RES;
			if( c < 0x0F6B ) return T_TIB;
			if( c < 0x0F71 ) return T_RES;
			if( c < 0x0F8C ) return T_TIB;
			if( c < 0x0F90 ) return T_RES;
			if( c < 0x0F98 ) return T_TIB;
			if( c < 0x0F99 ) return T_RES;
			if( c < 0x0FBD ) return T_TIB;
			if( c < 0x0FBE ) return T_RES;
			if( c < 0x0FCD ) return T_TIB;
			if( c < 0x0FCF ) return T_RES;
			if( c < 0x0FD0 ) return T_TIB;
			if( c < 0x1000 ) return T_RES;
			if( c < 0x1022 ) return T_MYA;
			if( c < 0x1023 ) return T_RES;
			if( c < 0x1028 ) return T_MYA;
			if( c < 0x1029 ) return T_RES;
			if( c < 0x102B ) return T_MYA;
			if( c < 0x102C ) return T_RES;
			if( c < 0x1033 ) return T_MYA;
			if( c < 0x1036 ) return T_RES;
			if( c < 0x103A ) return T_MYA;
			if( c < 0x1040 ) return T_RES;
			if( c < 0x105A ) return T_MYA;
			if( c < 0x10A0 ) return T_RES;
			if( c < 0x10C6 ) return T_GEO;
			if( c < 0x10D0 ) return T_RES;
			if( c < 0x10F7 ) return T_GEO;
			if( c < 0x10FB ) return T_RES;
			if( c < 0x10FC ) return T_GEO;
			if( c < 0x1100 ) return T_RES;
			if( c < 0x115A ) return T_HAN;
			if( c < 0x115F ) return T_RES;
			if( c < 0x11A3 ) return T_HAN;
			if( c < 0x11A8 ) return T_RES;
			if( c < 0x11FA ) return T_HAN;
			if( c < 0x1200 ) return T_RES;
			if( c < 0x1207 ) return T_ETH;
			if( c < 0x1208 ) return T_RES;
			if( c < 0x1247 ) return T_ETH;
			if( c < 0x1248 ) return T_RES;
			if( c < 0x1249 ) return T_ETH;
			if( c < 0x124A ) return T_RES;
			if( c < 0x124E ) return T_ETH;
			if( c < 0x1250 ) return T_RES;
			if( c < 0x1257 ) return T_ETH;
			if( c < 0x1258 ) return T_RES;
			if( c < 0x1259 ) return T_ETH;
			if( c < 0x125A ) return T_RES;
			if( c < 0x125E ) return T_ETH;
			if( c < 0x1260 ) return T_RES;
			if( c < 0x1287 ) return T_ETH;
			if( c < 0x1288 ) return T_RES;
			if( c < 0x1289 ) return T_ETH;
			if( c < 0x128A ) return T_RES;
			if( c < 0x128E ) return T_ETH;
			if( c < 0x1290 ) return T_RES;
			if( c < 0x12AF ) return T_ETH;
			if( c < 0x12B0 ) return T_RES;
			if( c < 0x12B1 ) return T_ETH;
			if( c < 0x12B2 ) return T_RES;
			if( c < 0x12B6 ) return T_ETH;
			if( c < 0x12B8 ) return T_RES;
			if( c < 0x12BF ) return T_ETH;
			if( c < 0x12C0 ) return T_RES;
			if( c < 0x12C1 ) return T_ETH;
			if( c < 0x12C2 ) return T_RES;
			if( c < 0x12C6 ) return T_ETH;
			if( c < 0x12C8 ) return T_RES;
			if( c < 0x12CF ) return T_ETH;
			if( c < 0x12D0 ) return T_RES;
			if( c < 0x12D7 ) return T_ETH;
			if( c < 0x12D8 ) return T_RES;
			if( c < 0x12EF ) return T_ETH;
			if( c < 0x12F0 ) return T_RES;
			if( c < 0x130F ) return T_ETH;
			if( c < 0x1310 ) return T_RES;
			if( c < 0x1311 ) return T_ETH;
			if( c < 0x1312 ) return T_RES;
			if( c < 0x1316 ) return T_ETH;
			if( c < 0x1318 ) return T_RES;
			if( c < 0x131F ) return T_ETH;
			if( c < 0x1320 ) return T_RES;
			if( c < 0x1347 ) return T_ETH;
			if( c < 0x1348 ) return T_RES;
			if( c < 0x135B ) return T_ETH;
			if( c < 0x1361 ) return T_RES;
			if( c < 0x137D ) return T_ETH;
			if( c < 0x13A0 ) return T_RES;
			if( c < 0x13F5 ) return T_CHE;
			if( c < 0x1401 ) return T_RES;
			if( c < 0x1677 ) return T_CAN;
			if( c < 0x1680 ) return T_RES;
			if( c < 0x169D ) return T_OGH;
			if( c < 0x16A0 ) return T_RES;
			if( c < 0x16F1 ) return T_RUN;
			if( c < 0x1780 ) return T_RES;
			if( c < 0x17DD ) return T_KHM;
			if( c < 0x17E0 ) return T_RES;
			if( c < 0x17EA ) return T_KHM;
			if( c < 0x1800 ) return T_RES;
			if( c < 0x180F ) return T_MON;
			if( c < 0x1810 ) return T_RES;
			if( c < 0x181A ) return T_MON;
			if( c < 0x1820 ) return T_RES;
			if( c < 0x1878 ) return T_MON;
			if( c < 0x1880 ) return T_RES;
			if( c < 0x18AA ) return T_MON;
			if( c < 0x1E00 ) return T_RES;
			if( c < 0x1E9C ) return T_LAT;
			if( c < 0x1EA0 ) return T_RES;
			if( c < 0x1EFA ) return T_LAT;
			if( c < 0x1F00 ) return T_RES;
			if( c < 0x1F16 ) return T_GRE;
			if( c < 0x1F18 ) return T_RES;
			if( c < 0x1F1E ) return T_GRE;
			if( c < 0x1F20 ) return T_RES;
			if( c < 0x1F46 ) return T_GRE;
			if( c < 0x1F48 ) return T_RES;
			if( c < 0x1F4E ) return T_GRE;
			if( c < 0x1F50 ) return T_RES;
			if( c < 0x1F58 ) return T_GRE;
			if( c < 0x1F59 ) return T_RES;
			if( c < 0x1F5A ) return T_GRE;
			if( c < 0x1F5B ) return T_RES;
			if( c < 0x1F5C ) return T_GRE;
			if( c < 0x1F5D ) return T_RES;
			if( c < 0x1F5E ) return T_GRE;
			if( c < 0x1F5F ) return T_RES;
			if( c < 0x1F7E ) return T_GRE;
			if( c < 0x1F80 ) return T_RES;
			if( c < 0x1FB5 ) return T_GRE;
			if( c < 0x1FB6 ) return T_RES;
			if( c < 0x1FC5 ) return T_GRE;
			if( c < 0x1FC6 ) return T_RES;
			if( c < 0x1FD4 ) return T_GRE;
			if( c < 0x1FD6 ) return T_RES;
			if( c < 0x1FDC ) return T_GRE;
			if( c < 0x1FDD ) return T_RES;
			if( c < 0x1FF0 ) return T_GRE;
			if( c < 0x1FF2 ) return T_RES;
			if( c < 0x1FF5 ) return T_GRE;
			if( c < 0x1FF6 ) return T_RES;
			if( c < 0x1FFF ) return T_GRE;
			if( c < 0x2000 ) return T_RES;
			if( c < 0x2047 ) return T_SYM;
			if( c < 0x2048 ) return T_RES;
			if( c < 0x204E ) return T_SYM;
			if( c < 0x206A ) return T_RES;
			if( c < 0x2071 ) return T_SYM;
			if( c < 0x2074 ) return T_RES;
			if( c < 0x208F ) return T_SYM;
			if( c < 0x20A0 ) return T_RES;
			if( c < 0x20B0 ) return T_SYM;
			if( c < 0x20D0 ) return T_RES;
			if( c < 0x20E4 ) return T_SYM;
			if( c < 0x2100 ) return T_RES;
			if( c < 0x213B ) return T_SYM;
			if( c < 0x2153 ) return T_RES;
			if( c < 0x2184 ) return T_SYM;
			if( c < 0x2190 ) return T_RES;
			if( c < 0x21F4 ) return T_SYM;
			if( c < 0x2200 ) return T_RES;
			if( c < 0x22F2 ) return T_SYM;
			if( c < 0x2300 ) return T_RES;
			if( c < 0x237C ) return T_SYM;
			if( c < 0x237D ) return T_RES;
			if( c < 0x239B ) return T_SYM;
			if( c < 0x2400 ) return T_RES;
			if( c < 0x2427 ) return T_SYM;
			if( c < 0x2440 ) return T_RES;
			if( c < 0x244B ) return T_SYM;
			if( c < 0x2460 ) return T_RES;
			if( c < 0x24EB ) return T_SYM;
			if( c < 0x2500 ) return T_RES;
			if( c < 0x2596 ) return T_SYM;
			if( c < 0x25A0 ) return T_RES;
			if( c < 0x25F8 ) return T_SYM;
			if( c < 0x2600 ) return T_RES;
			if( c < 0x2614 ) return T_SYM;
			if( c < 0x2619 ) return T_RES;
			if( c < 0x2672 ) return T_SYM;
			if( c < 0x2701 ) return T_RES;
			if( c < 0x2705 ) return T_SYM;
			if( c < 0x2706 ) return T_RES;
			if( c < 0x270A ) return T_SYM;
			if( c < 0x270C ) return T_RES;
			if( c < 0x271D ) return T_SYM;
			if( c < 0x271E ) return T_LAT;
			if( c < 0x2728 ) return T_SYM;
			if( c < 0x2729 ) return T_RES;
			if( c < 0x274C ) return T_SYM;
			if( c < 0x274D ) return T_RES;
			if( c < 0x274E ) return T_SYM;
			if( c < 0x274F ) return T_RES;
			if( c < 0x2753 ) return T_SYM;
			if( c < 0x2756 ) return T_RES;
			if( c < 0x2757 ) return T_SYM;
			if( c < 0x2758 ) return T_RES;
			if( c < 0x275F ) return T_SYM;
			if( c < 0x2761 ) return T_RES;
			if( c < 0x2768 ) return T_SYM;
			if( c < 0x2776 ) return T_RES;
			if( c < 0x2795 ) return T_SYM;
			if( c < 0x2798 ) return T_RES;
			if( c < 0x27B0 ) return T_SYM;
			if( c < 0x27B1 ) return T_RES;
			if( c < 0x27BF ) return T_SYM;
			if( c < 0x2800 ) return T_RES;
			if( c < 0x2900 ) return T_SYM;
			if( c < 0x2E80 ) return T_RES;
			if( c < 0x2E9A ) return T_CJK;
			if( c < 0x2E9B ) return T_RES;
			if( c < 0x2EF4 ) return T_CJK;
			if( c < 0x2F00 ) return T_RES;
			if( c < 0x2FD6 ) return T_CJK;
			if( c < 0x2FF0 ) return T_RES;
			if( c < 0x2FFC ) return T_SYM;
			if( c < 0x3000 ) return T_RES;
			if( c < 0x302E ) return T_SYM;
			if( c < 0x3030 ) return T_HAN;
			if( c < 0x303B ) return T_SYM;
			if( c < 0x303E ) return T_RES;
			if( c < 0x3040 ) return T_SYM;
			if( c < 0x3041 ) return T_RES;
			if( c < 0x3095 ) return T_JPN;
			if( c < 0x3099 ) return T_RES;
			if( c < 0x309D ) return T_SYM;
			if( c < 0x309F ) return T_JPN;
			if( c < 0x30A1 ) return T_RES;
			if( c < 0x30FC ) return T_JPN;
			if( c < 0x30FD ) return T_SYM;
			if( c < 0x30FF ) return T_JPN;
			if( c < 0x3105 ) return T_RES;
			if( c < 0x312D ) return T_CJK;
			if( c < 0x3131 ) return T_RES;
			if( c < 0x318F ) return T_HAN;
			if( c < 0x3190 ) return T_RES;
			if( c < 0x31A0 ) return T_SYM;
			if( c < 0x31B8 ) return T_CJK;
			if( c < 0x3200 ) return T_RES;
			if( c < 0x321D ) return T_SYM;
			if( c < 0x3220 ) return T_RES;
			if( c < 0x3244 ) return T_SYM;
			if( c < 0x3260 ) return T_RES;
			if( c < 0x327C ) return T_SYM;
			if( c < 0x327F ) return T_RES;
			if( c < 0x32B1 ) return T_SYM;
			if( c < 0x32C0 ) return T_RES;
			if( c < 0x32CC ) return T_SYM;
			if( c < 0x32D0 ) return T_RES;
			if( c < 0x32FF ) return T_SYM;
			if( c < 0x3300 ) return T_RES;
			if( c < 0x3377 ) return T_SYM;
			if( c < 0x337B ) return T_RES;
			if( c < 0x33DE ) return T_SYM;
			if( c < 0x33E0 ) return T_RES;
			if( c < 0x33FF ) return T_SYM;
			if( c < 0x3400 ) return T_RES;
			if( c < 0x4DB6 ) return T_CJK;
			if( c < 0x4E00 ) return T_RES;
			if( c < 0x9FA6 ) return T_CJK;
			if( c < 0xA000 ) return T_RES;
			if( c < 0xA48D ) return T_YIS;
			if( c < 0xA490 ) return T_RES;
			if( c < 0xA4A2 ) return T_YIS;
			if( c < 0xA4A4 ) return T_RES;
			if( c < 0xA4B4 ) return T_YIS;
			if( c < 0xA4B5 ) return T_RES;
			if( c < 0xA4C1 ) return T_YIS;
			if( c < 0xA4C2 ) return T_RES;
			if( c < 0xA4C5 ) return T_YIS;
			if( c < 0xA4C6 ) return T_RES;
			if( c < 0xA4C7 ) return T_YIS;
			if( c < 0xAC00 ) return T_RES;
			if( c < 0xD7A4 ) return T_HAN;
			if( c < 0xD800 ) return T_RES;
			if( c < 0xD801 ) return T_SYM;
			if( c < 0xDB7F ) return T_RES;
			if( c < 0xDB81 ) return T_SYM;
			if( c < 0xDBFF ) return T_RES;
			if( c < 0xDC01 ) return T_SYM;
			if( c < 0xDFFF ) return T_RES;
			if( c < 0xE001 ) return T_SYM;
			if( c < 0xF8FF ) return T_RES;
			if( c < 0xF900 ) return T_SYM;
			if( c < 0xFA2E ) return T_CJK;
			if( c < 0xFB00 ) return T_RES;
			if( c < 0xFB07 ) return T_LAT;
			if( c < 0xFB13 ) return T_RES;
			if( c < 0xFB18 ) return T_ARM;
			if( c < 0xFB1D ) return T_RES;
			if( c < 0xFB37 ) return T_HEB;
			if( c < 0xFB38 ) return T_RES;
			if( c < 0xFB3D ) return T_HEB;
			if( c < 0xFB3E ) return T_RES;
			if( c < 0xFB3F ) return T_HEB;
			if( c < 0xFB40 ) return T_RES;
			if( c < 0xFB42 ) return T_HEB;
			if( c < 0xFB43 ) return T_RES;
			if( c < 0xFB45 ) return T_HEB;
			if( c < 0xFB46 ) return T_RES;
			if( c < 0xFB50 ) return T_HEB;
			if( c < 0xFBB2 ) return T_ARA;
			if( c < 0xFBD3 ) return T_RES;
			if( c < 0xFD3E ) return T_ARA;
			if( c < 0xFD40 ) return T_SYM;
			if( c < 0xFD50 ) return T_RES;
			if( c < 0xFD90 ) return T_ARA;
			if( c < 0xFD92 ) return T_RES;
			if( c < 0xFDC8 ) return T_ARA;
			if( c < 0xFDF0 ) return T_RES;
			if( c < 0xFDFC ) return T_ARA;
			if( c < 0xFE20 ) return T_RES;
			if( c < 0xFE24 ) return T_SYM;
			if( c < 0xFE30 ) return T_RES;
			if( c < 0xFE45 ) return T_SYM;
			if( c < 0xFE49 ) return T_RES;
			if( c < 0xFE53 ) return T_SYM;
			if( c < 0xFE54 ) return T_RES;
			if( c < 0xFE67 ) return T_SYM;
			if( c < 0xFE68 ) return T_RES;
			if( c < 0xFE6C ) return T_SYM;
			if( c < 0xFE70 ) return T_RES;
			if( c < 0xFE73 ) return T_ARA;
			if( c < 0xFE74 ) return T_RES;
			if( c < 0xFE75 ) return T_ARA;
			if( c < 0xFE76 ) return T_RES;
			if( c < 0xFEFD ) return T_ARA;
			if( c < 0xFEFF ) return T_RES;
			if( c < 0xFF00 ) return T_SYM;
			if( c < 0xFF01 ) return T_RES;
			if( c < 0xFF5F ) return T_SYM;
			if( c < 0xFF61 ) return T_RES;
			if( c < 0xFFBF ) return T_SYM;
			if( c < 0xFFC2 ) return T_RES;
			if( c < 0xFFC8 ) return T_SYM;
			if( c < 0xFFCA ) return T_RES;
			if( c < 0xFFD0 ) return T_SYM;
			if( c < 0xFFD2 ) return T_RES;
			if( c < 0xFFD8 ) return T_SYM;
			if( c < 0xFFDA ) return T_RES;
			if( c < 0xFFDD ) return T_SYM;
			if( c < 0xFFE0 ) return T_RES;
			if( c < 0xFFE7 ) return T_SYM;
			if( c < 0xFFE8 ) return T_RES;
			if( c < 0xFFEF ) return T_SYM;
			if( c < 0xFFF9 ) return T_RES;
			if( c < 0xFFFE ) return T_SYM;
			return T_RES;
		}

		static const char *s_CodeGroupString[] = {
		"None",
		"Arabic",
		"Armenian",
		"Spaces",
		"Bengali",
		"Canadian Syllabics",
		"Cherokee",
		"Chinese",
		"Coptic",
		"Control Characters",
		"Cyrillic",
		"Devanagari",
		"Digits",
		"Ethiopic",
		"Georgian",
		"Greek",
		"Gujarati",
		"Gurmukhi",
		"Hangul",
		"Hebrew",
		"Japanese",
		"Kannada",
		"Khmer",
		"Lao",
		"Latin",
		"Malayalam",
		"Mongolian",
		"Myanmar",
		"Ogham",
		"Oriya",
		"Reserved",
		"Runic",
		"Sinhala",
		"Special Characters",
		"Symbols",
		"Syriac",
		"Tamil",
		"Telugu",
		"Thai",
		"Thaana",
		"Tibetan",
		"Yi Syllables",
		};

		const char *get_unicode_group_name( wchar_t c )
		{
			return s_CodeGroupString[ get_unicode_group( c ) ];
		}

	};

};