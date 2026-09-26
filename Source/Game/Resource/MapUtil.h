#pragma once

#include <vector>
#include <string>

#include <kfile/KStream.h>


typedef std::vector<std::string> STRING_VECTOR;

namespace CStringUtil
{
	inline bool isCommand( const char *pString, const char *pCmd )
	{
		if( strlen(pString) < strlen(pCmd) ) return false;								// by Testors;
		if( pString[strlen(pCmd)] && pString[strlen(pCmd)] != ' ' ) return false;		// by Testors;
		return !_strnicmp( pString, pCmd, strlen(pCmd ) );
	}

	inline std::string ReadString( KStream* pStream )
	{
		DWORD dwLength;
		pStream->Read( &dwLength, sizeof(dwLength) );

		char* szBuffer = new char[dwLength + 1];
		pStream->Read( szBuffer, sizeof(char) * dwLength );
		szBuffer[dwLength] = '\0';

		std::string str = szBuffer;
		delete [] szBuffer;

		return str;
	}
	inline void WriteString( KStream* pStream, const char* szString )
	{
		DWORD dwLength = DWORD(::strlen( szString ));

		pStream->Write( &dwLength, sizeof(dwLength) );
		pStream->Write( szString, sizeof(char) * dwLength );
	}

	inline void GetTextLinesFromString( const std::string& rEntireText, STRING_VECTOR& rGetTextLines, const char* szLineDivision = "\r\n" )
	{
		rGetTextLines.clear();

		int nDivisionSize = int(::strlen( szLineDivision ));

		// 문자열을 라인 단위로 나눠서 rGetTextLines에 넣는다.
		for( int nCurrentPos = 0; ; )
		{
			std::string strLine;

			int nLineFeed = int( rEntireText.find( szLineDivision, nCurrentPos ) );
			if( nLineFeed != std::string::npos )
			{
				strLine.assign( rEntireText, nCurrentPos, nLineFeed - nCurrentPos );
				if( strLine.size() > 0 ) rGetTextLines.push_back( strLine );

				nCurrentPos = (nLineFeed + nDivisionSize);
			}
			else
			{
				strLine.assign( rEntireText, nCurrentPos, rEntireText.size() - nCurrentPos );
				if( strLine.size() > 0 ) rGetTextLines.push_back( strLine );
				break;
			}
		}
	}

	inline void GetTextLinesFromString( const char* szEntireText, STRING_VECTOR& rGetTextLines, const char* szLineDivision = "\r\n" )
	{
		GetTextLinesFromString( std::string( szEntireText ), rGetTextLines, szLineDivision );
	}

	static void ReplacePhrase( std::string& rText, const std::string& rFindPhrase, const std::string& rReplacePhrase )
	{
		for( size_t nCurrentPos = 0; ; )
		{
			size_t nFindPos = rText.find( rFindPhrase, nCurrentPos );
			if( nFindPos == std::string::npos ) break;

			rText.replace( nFindPos, rFindPhrase.size(), rReplacePhrase );
			nCurrentPos = (nFindPos + rReplacePhrase.size());
		}
	}

	inline bool GetContentString( const std::string& rString, const char* szHeadString, std::string& rGetContentString )
	{
		if( rString.compare( 0, ::strlen( szHeadString ), szHeadString ) == 0 )
		{
			rGetContentString.assign( rString, ::strlen( szHeadString ), rString.size() - ::strlen( szHeadString ) );
			return true;
		}
		return false;
	}

	inline std::string StringFormat( const char* szString, ... )
	{
		char szBuf[1024];

		va_list va;
		va_start( va, szString );
		s_vsprintf( szBuf, _countof(szBuf), szString, va );
		va_end( va );

		return std::string( szBuf );
	}

	inline std::string GetCommaNumberString( int nValue )
	{
		const int c_nCommaGap = 3;

		std::string strNum = CStringUtil::StringFormat( "%d", nValue );
		std::string strCommaNum;

		int nCount( 0 );
		int nCommaPos = int(strNum.size()) % c_nCommaGap;

		for( std::string::iterator it = strNum.begin(); it != strNum.end(); ++it, ++nCount )
		{
			if( it != strNum.begin() && nCommaPos == (nCount % c_nCommaGap) )
				strCommaNum.push_back( ',' );

			strCommaNum.push_back( (*it) );
		}
		return strCommaNum;
	}
	
	inline void GetAniSetName( const char* szString, char szChar, std::string & strOut )
	{
		std::string strName = szString;
		std::string strKey;

		std::string::size_type pos;
		const std::string::size_type npos = -1;
		char szName[_MAX_PATH]; memset( szName, 0, sizeof(szName) );

		pos = strName.rfind(szChar);

		if ( pos != npos )
		{
			strKey = strName.substr( 0, pos );
		}

		strOut = strKey;
	}

	inline void GetStrKey( const char* szString, char szChar, std::string & strOut, BOOL bOld = FALSE )
	{
		//Ani Name은 화일명을 잘라서 만든다.
		std::string strName = szString;
		std::string strKey;

		std::string::size_type pos, pos1, pos2;
		const std::string::size_type npos = -1;
		char szName[128] = { 0, };

		if( bOld )
		{
			pos = strName.rfind(szChar);

			if ( pos != npos )
			{
				pos2 = strName.rfind('.');
				strName._Copy_s(szName, _countof( szName ), (pos2 - (pos+1)), pos+1);
			}
		}
		else
		{
			pos = strName.rfind('\\');
			strName.erase( 0, pos+1 );

			pos  = strName.find('_');
			pos1 = strName.rfind('_');

			//애니키는 모션 타입과 무기 종류 저장
			strKey = strName.substr( pos+1, (pos1-(pos+1)) );
			s_strcpy(szName, _countof( szName ), strKey.c_str() );
		}

		strOut = szName;
	}

	inline void GetEventStrKey( const char* szString, char szChar, std::string & strOut )
	{
		//Ani Name은 화일명을 잘라서 만든다.
		std::string strName = szString;
		std::string strKey;

 		std::string::size_type pos;
		const std::string::size_type npos = -1;
		char szName[128];

		pos = strName.rfind(szChar);

		if ( pos != npos )
		{
			strKey = strName.substr( 0, pos );
			s_strcpy(szName, _countof( szName ), strKey.c_str() );
		}

		strOut = szName;
	}
};
