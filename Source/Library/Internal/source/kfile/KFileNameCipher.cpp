
#include "../../include/kfile/KFileNameCipher.h"
#include <assert.h>
#include <locale>

static const int MAGIC_DEPTH_NUMBER = 32;

static char encTable[] =
{
//	0,		1,		2,		3,		4,		5,		6,		7,		8,		9,		10,		11,		12,		13,		14,		15,		16,		17,		18,		19,		20,		21,		22,		23,		24,		25,		26,		27,		28,		29,		30,		31,
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	'g',	' ',	0,		'&',	'w',	',',	'l',	'N',	'X',	'O',	0,		'7',	'.',	'%',	'e',	0,		'8',	'_',	']',	'#',	'P',	'1',	'-',	'$',	'V',	'[',	0,		'Y',	0,		'^',	0,		0,
	'K',	'}',	'j',	'0',	'@',	'G',	'S',	')',	'A',	'x',	'y',	'6',	'9',	'E',	'F',	'{',	'W',	'b',	'=',	'R',	'v',	't',	'h',	'2',	'4',	'M',	'(',	'k',	0,		'm',	'a',	'+',
	'~',	'D',	'\'',	'C',	'!',	'J',	'I',	'd',	'B',	'U',	'`',	'q',	'f',	'p',	'H',	'Q',	'3',	'L',	'n',	'o',	'Z',	'i',	'r',	's',	'u',	';',	'z',	'c',	0,		'T',	'5',	0,
};

static char decTable[] =
{
//	0,		1,		2,		3,		4,		5,		6,		7,		8,		9,		10,		11,		12,		13,		14,		15,		16,		17,		18,		19,		20,		21,		22,		23,		24,		25,		26,		27,		28,		29,		30,		31,
	0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,		0,
	'!',	'd',	0,		'3',	'7',	'-',	'#',	'b',	'Z',	'G',	0,		'_',	'%',	'6',	',',	0,		'C',	'5',	'W',	'p',	'X',	'~',	'K',	'+',	'0',	'L',	0,		'y',	0,		'R',	0,		0,
	'D',	'H',	'h',	'c',	'a',	'M',	'N',	'E',	'n',	'f',	'e',	'@',	'q',	'Y',	'\'',	')',	'4',	'o',	'S',	'F',	'}',	'i',	'8',	'P',	'(',	';',	't',	'9',	0,		'2',	'=',	'1',
	'j',	'^',	'Q',	'{',	'g',	'.',	'l',	' ',	'V',	'u',	'B',	'[',	'&',	']',	'r',	's',	'm',	'k',	'v',	'w',	'U',	'x',	'T',	'$',	'I',	'J',	'z',	'O',	0,		'A',	'`',	0,
};

static char refTable[] = "^&T_Nsd{xo5v`rOYV+,iIU#kCJq8$'~L0P]FeBn-Au(pXHZhwDy2}agWG7K=bQ;SRt)46l@jE%9!c1[3fmMz";

static char GetEncChar( char c, int depth )
{
	assert( depth > 0 );
	assert( c > 0 );

	char ret = c;

	for ( int i = 0; i < depth; ++i )
	{
		ret = encTable[ret];
	}

	return ret;
}

static char GetDecChar( char c, int depth )
{
	assert( depth > 0 );
	assert( c > 0 );

	char ret = c;

	for ( int i = 0; i < depth; ++i )
	{
		ret = decTable[ret];
	}

	return ret;
}

static int GetStartDepth( const std::string& strFileName )
{
	int key = 0;

	for ( size_t i = 0; i < strFileName.length(); ++i )
	{
		key +=  17 * strFileName[i] + 1;
	}

	key += static_cast< int >( strFileName.length() );

	int ret = key % MAGIC_DEPTH_NUMBER;

	if ( ret == 0 )
	{
		ret = MAGIC_DEPTH_NUMBER;
	}

	return ret;
}

static char GetParityChar( const std::string & strEncName )
{
	int key = 0;
	for ( size_t i = 0; i < strEncName.length(); ++i )
	{
		key += strEncName[i];
	}

	key = key % strlen( refTable );

	return refTable[ key ];
}

static void ReverseString( std::string & strString )
{
	size_t len = strString.size();

	std::swap( strString[0], strString[(int)(len*0.66f)] );
	std::swap( strString[1], strString[(int)(len*0.33f)] );
}

namespace KFileNameCipher
{
bool IsEncodedName( const std::string & strFileName )
{
	if ( strFileName.length() < 4 )
		return false;

	std::string strExt = strFileName.substr( strFileName.length() - 4, strFileName.length() );

	if( _stricmp( strExt.c_str(), ".ogg" ) == 0 ) return false;

	std::string strTest = strFileName.substr( 1, strFileName.length() - 2 );

	ReverseString( strTest );

	if ( strFileName[ 0 ] == GetParityChar( strTest ) )
		return true;

	return false;
}

void EncodeFileName( std::string & strFileName )
{
	std::string strString = strFileName;
	for ( size_t i = 0; i < strString.size(); ++i )
	{
		strString[i] = static_cast< char >( ::tolower( strString[i] ) );
	}

	if ( strString.empty() )
		return;

	size_t len = strString.length();
	int start_depth;
	int depth;
	start_depth = depth = GetStartDepth( strString );

	for ( size_t i = 0; i < len; ++i )
	{
		char c = strString[ i ];
		strString[ i ] = GetEncChar( c, depth );

		depth = (depth +  c * 17 + 1) % MAGIC_DEPTH_NUMBER;

		if ( depth == 0 )
		{
			depth = MAGIC_DEPTH_NUMBER;
		}
	}

	std::string strResult;

	strResult += GetParityChar( strString );	

	ReverseString( strString );

	strResult +=  strString;

	strResult += refTable[ start_depth ];

	strFileName = strResult;
}

void DecodeFileName( std::string & strFileName )
{
	std::string strString = strFileName;

	if ( !IsEncodedName( strString ) )
		return;

	int depth = 0;

	int refTableLen = static_cast< int >( strlen( refTable ) );
	for ( int i = 0; i < refTableLen; ++i )
	{
		if ( refTable[i] == strString[strString.length() - 1] )
		{
			depth = i;
			break;
		}
	}
	
	if ( depth == 0 )
		return;

	strString = strString.substr( 1, strString.length() - 2 );

	ReverseString( strString );

	size_t len = strString.length();

	for ( size_t i = 0; i < len; ++i )
	{
		strString[ i ] = GetDecChar( strString[ i ], depth );

		depth = (depth +  strString[ i ] * 17 + 1) % MAGIC_DEPTH_NUMBER;

		if ( depth == 0 )
		{
			depth = MAGIC_DEPTH_NUMBER;
		}
	}

	strFileName = strString;
}
};
