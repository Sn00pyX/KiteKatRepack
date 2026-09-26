#include "../../include/kfile/TrfMetaData.h"
#include <windows.h>
#include <assert.h>
#include "../../include/toolkit/safe_function.h"

using namespace trf;


void TemplateInfo::clear()
{
	m_hashByName.clear();
	m_fields.clear();
	m_fixedSize = 0;
}

bool TemplateInfo::removeField( const char* pFieldName )
{
	int index;
	if ( !m_hashByName.lookup( pFieldName, index ) )
		return false;

	if ( m_fixedSize >= 0 )
		m_fixedSize -= m_fields[ index ].fixedSize;

	m_fields.erase( m_fields.begin() + index );
	m_hashByName.erase( pFieldName );
	return true;
}

bool TemplateInfo::addSimpleField( const char* pFieldName, int type )
{
	if ( m_hashByName.has( pFieldName ) )
		return false;

	if ( type == Type::DICT || type == Type::TEMPLATE || (type & Type::_ARRAYMASK) != 0 )
		return false;

	m_fields.resize( m_fields.size()+1 );
	m_fields.back().name = pFieldName;
	m_fields.back().type = type;
	m_fields.back().pInfo = NULL;
	m_fields.back().fixedCount = -1;
	m_fields.back().fixedSize = Type::getTypeSize( type );
	m_hashByName.add( pFieldName, static_cast< int >( m_fields.size() )-1 );

	if ( m_fixedSize >= 0 )
		m_fixedSize += m_fields.back().fixedSize;

	return true;
}

bool TemplateInfo::addArrayField( const char* pFieldName, int elementType, int fixedCount )
{
	if ( m_hashByName.has( pFieldName ) )
		return false;

	m_fields.resize( m_fields.size()+1 );
	m_fields.back().name = pFieldName;
	m_fields.back().type = Type::getArrayType( elementType );
	m_fields.back().pInfo = NULL;

	if ( fixedCount > 0 ) 
	{
		m_fields.back().fixedCount = fixedCount;
		m_fields.back().fixedSize = fixedCount * Type::getTypeSize( elementType );
		if ( m_fixedSize >= 0 )
			m_fixedSize += m_fields.back().fixedSize;
	}
	else
	{
		m_fields.back().fixedCount = -1;
		m_fields.back().fixedSize = -1;
		m_fixedSize = -1;
	}

	m_hashByName.add( pFieldName, static_cast< int >( m_fields.size() )-1 );

	return true;
}

bool TemplateInfo::addTemplateField( const char* pFieldName, const TemplateInfo* pInfo )
{
	if ( m_hashByName.has( pFieldName ) )
		return false;

	m_fields.resize( m_fields.size()+1 );
	m_fields.back().name = pFieldName;
	m_fields.back().type = Type::TEMPLATE;
	m_fields.back().pInfo = pInfo;
	m_fields.back().fixedCount = -1;
	m_fields.back().fixedSize = pInfo->m_fixedSize >= 0 ? pInfo->fixedSize() : -1;
	m_hashByName.add( pFieldName, static_cast< int >( m_fields.size() )-1 );

	if ( m_fixedSize >= 0 && m_fields.back().fixedSize >= 0 )
		m_fixedSize += m_fields.back().fixedSize;

	return true;
}

bool TemplateInfo::addTemplateArrayField( const char* pFieldName, const TemplateInfo* pElementInfo, int fixedCount )
{
	if ( m_hashByName.has( pFieldName ) )
		return false;

	m_fields.resize( m_fields.size()+1 );
	m_fields.back().name = pFieldName;
	m_fields.back().type = Type::TEMPLATE_ARRAY;
	m_fields.back().pInfo = pElementInfo;

	if ( fixedCount > 0 )
	{
		m_fields.back().fixedCount = fixedCount;
		m_fields.back().fixedSize = pElementInfo->fixedSize() >= 0 ? fixedCount * pElementInfo->fixedSize() : -1;
		if ( m_fixedSize >= 0 && m_fields.back().fixedSize >= 0 )
			m_fixedSize += m_fields.back().fixedSize;
	}
	else
	{
		m_fields.back().fixedCount = -1;
		m_fields.back().fixedSize = -1;
		m_fixedSize = -1;
	}

	m_hashByName.add( pFieldName, static_cast< int >( m_fields.size() )-1 );

	return true;
}

bool TemplateInfo::addDictField( const char* pFieldName )
{
	if ( m_hashByName.has( pFieldName ) )
		return false;

	m_fields.resize( m_fields.size()+1 );
	m_fields.back().name = pFieldName;
	m_fields.back().type = Type::DICT;
	m_fields.back().pInfo = NULL;
	m_fields.back().fixedCount = -1;
	m_fields.back().fixedSize = -1;
	m_hashByName.add( pFieldName, static_cast< int >( m_fields.size() )-1 );

	m_fixedSize = -1;
	return true;
}

int TemplateInfo::calcFixedSize( int n ) const
{
	int sum = 0;
	for ( int i = 0; i < n; i++ )
	{
		int fieldFixedSize = m_fields[ i ].fixedSize;
		if ( fieldFixedSize < 0 )
			return -1;
		sum += fieldFixedSize;
	}
	return sum;
}

// ---------------------------------------------------------------------------------------------------

MetaData::MetaData()
{
	m_ref = 0;
}

MetaData::~MetaData()
{
	clear();
}

void MetaData::clear()
{
	assert( m_ref == 0 );	// 중요. 이 메타데이터를 사용하는 filer 나 mapper 가 남아있을 경우 
							// 이후 그 filer/mapper 를 지울때 메모리를 제대로 반환하지 못할 수 있다.
	m_hashByGUID.clear();
	m_hashByName.clear();

	for ( std::vector< TemplateInfo* >::iterator it = m_infos.begin(); it != m_infos.end(); ++it )
	{
		delete *it;
	}
	m_infos.clear();
}

void MetaData::addRef() const
{
	InterlockedIncrement( &m_ref );	// multi-thread 에서 cnt가 제대로 증가/감소가 되지 않아서 assert( m_ref == 0 ) 에서 오류가 발생. thread-safe 형태로 수정함. by ekflame. 090719
}

void MetaData::subRef() const
{
	InterlockedDecrement( &m_ref);		// multi-thread 에서 cnt가 제대로 증가/감소가 되지 않아서 assert( m_ref == 0 ) 에서 오류가 발생. thread-safe 형태로 수정함. by ekflame. 090719
}

const TemplateInfo* MetaData::getTemplateInfo( const GUID& uuidType ) const
{
	TemplateInfo* pInfo;
	return m_hashByGUID.lookup( uuidType, pInfo ) ? pInfo : NULL;
}

const TemplateInfo* MetaData::getTemplateInfo( const char *pName ) const
{
	TemplateInfo* pInfo;
	return m_hashByName.lookup( pName, pInfo ) ? pInfo : NULL;
}

TemplateInfo* MetaData::addNewTemplateInfo( const GUID& uuidType, const char* pName )
{
	if ( m_hashByGUID.has( uuidType ) || m_hashByName.has( pName ) )
		return NULL;

	TemplateInfo* pInfo = new TemplateInfo( uuidType, pName, static_cast< int >( m_infos.size() ) );
	m_hashByGUID.add( uuidType, pInfo );
	m_hashByName.add( pName, pInfo );
	m_infos.push_back( pInfo );

	return pInfo;
}


// template parsing


namespace {

// KTOKENs

const char KTOKEN_EOF			= -1;

const char KTOKEN_NAME			= 1;
const char KTOKEN_INTEGER		= 3;
const char KTOKEN_GUID			= 5;

const char KTOKEN_OBRACE		= 10;
const char KTOKEN_CBRACE		= 11;
const char KTOKEN_OPAREN		= 12;
const char KTOKEN_CPAREN		= 13;
const char KTOKEN_OBRACKET		= 14;
const char KTOKEN_CBRACKET		= 15;
const char KTOKEN_OANGLE		= 16;
const char KTOKEN_CANGLE		= 17;
const char KTOKEN_DOT			= 18;
const char KTOKEN_COMMA			= 19;
const char KTOKEN_SEMICOLON		= 20;
const char KTOKEN_TEMPLATE		= 31;
const char KTOKEN_WORD			= 40;
const char KTOKEN_DWORD			= 41;
const char KTOKEN_FLOAT			= 42;
const char KTOKEN_DOUBLE		= 43;
const char KTOKEN_CHAR			= 44;
const char KTOKEN_BYTE			= 45;
const char KTOKEN_SHORT			= 46;
const char KTOKEN_LONG			= 47;
const char KTOKEN_VOID			= 48;
const char KTOKEN_LPSTR			= 49;
const char KTOKEN_STRING		= 50;
const char KTOKEN_WSTRING		= 51;
const char KTOKEN_ARRAY			= 52;
const char KTOKEN_LLONG			= 53;
const char KTOKEN_DICT			= 54;


struct KTemplateToken
{
	char TokenID;
	std::string StrToken;
};

char tokendata[] = {
		  KTOKEN_OBRACE			, '{'
		, KTOKEN_CBRACE			, '}'
		, KTOKEN_OPAREN			, '(' 
		, KTOKEN_CPAREN			, ')'
		, KTOKEN_OBRACKET		, '['
		, KTOKEN_CBRACKET		, ']'
		, KTOKEN_OANGLE			, '<'
		, KTOKEN_CANGLE			, '>'
		, KTOKEN_DOT			, '.'
		, KTOKEN_COMMA			, ','
		, KTOKEN_SEMICOLON		, ';'
};

char reserved_id[] = {
	  KTOKEN_TEMPLATE	// template
	, KTOKEN_WORD		// word
	, KTOKEN_SHORT		// short
	, KTOKEN_DWORD		// dword
	, KTOKEN_LONG		// long
	, KTOKEN_LLONG		// long long
	, KTOKEN_FLOAT		// float
	, KTOKEN_DOUBLE		// double
	, KTOKEN_CHAR		// char
	, KTOKEN_BYTE		// byte
	, KTOKEN_STRING		// string
	, KTOKEN_WSTRING	// wide string
	, KTOKEN_DICT		// dict
};
		
char *reserved_word[] = {
	 "template"
	,"word"
	,"short"
	,"dword"
	,"long"
	,"int64"
	,"float"
	,"double"
	,"char"
	,"byte"
	,"string"
	,"wstring"
	,"dict"
};
		
char whitespacechr[] = {
	'\n', '\r', ' ', '\t'

};
/*
, KTOKEN_NAME			,
, KTOKEN_INTEGER		,
, KTOKEN_GUID			,
, KTOKEN_ARRAY					
*/

bool getNextChar( KStream &stream, char *c, int &linecount )
{
	char chr;
	while ( 1 )
	{
nextchar:
		if ( stream.Read( &chr, 1 ) )
		{
			if ( chr == '\n' ) ++linecount;
			for ( int i=0 ; i<_countof(whitespacechr) ; i++ )
				if ( chr == whitespacechr[i] ) goto nextchar;
			*c = chr;
			return true;
		}
		else return false;
	}
}

bool getNextWord( KStream &stream, char *c, bool &isWhiteSpace, int &linecount )
{
	char chr;
	isWhiteSpace = false;
	while ( 1 )
	{
		if ( stream.Read( &chr, 1 ) )
		{
			for ( int i=0 ; i<_countof(whitespacechr) ; i++ )
			{
				if ( chr == whitespacechr[i] )
				{
					if ( chr == '\n' ) ++linecount;
					isWhiteSpace = true;
					*c = chr;
					return true;
				}
			}
			*c = chr;
			return true;
		}
		else return false;
	}
}

void checkReservedWord( KTemplateToken &token )
{
	for ( int i=0 ; i<_countof(reserved_id) ; ++i )
	{
		if ( strcmp( token.StrToken.c_str(), reserved_word[i] ) == 0 )
		{
			token.TokenID = reserved_id[i];
			return;
		}
	}
}

bool skipline( KStream &stream, int &linecount )
{
	char chr;
	while ( 1 )
	{
		if ( stream.Read( &chr, 1 ) )
		{
			if ( chr == '\n' )
			{
				++linecount;
				return true;
			}
		}
		else return false;
	}
}

bool skiptocomment( KStream &stream, int &linecount )
{
	char chr;
	while ( 1 )
	{
		if ( stream.Read( &chr, 1 ) )
		{
			if ( chr == '\n' )
				++linecount;
			if ( chr == '*' )
			{
				stream.Read( &chr, 1 );
				if ( chr == '/' )
					return true;
				else stream.Seek( -1, KStream::seekCur );
			}
		}
		else return false;
	}
}

bool getNextToken( KStream &stream, KTemplateToken &token, int &linecount )
{
	char c;
	std::string name;
	while ( 1 )
	{
		if ( getNextChar( stream, &c, linecount ) == false ) return false;
		for ( int i=0 ; i<_countof(tokendata)/2 ; i++ )
		{
			if ( tokendata[i*2+1] == c )
			{
				token.TokenID = tokendata[i*2];
				return true;
			}
		}
		
		name += c;
		unsigned short pos = static_cast<unsigned short>(stream.Tell());
		bool ws;
		if ( getNextWord( stream, &c, ws, linecount ) == false )
		{
			token.TokenID = KTOKEN_NAME;
			token.StrToken = name;
			return false;
		}

		std::string tempname = name;
		tempname += c;
		if ( strcmp( tempname.c_str(), "//" ) == 0 )
		{
			name = "";
			if ( skipline( stream, linecount ) == false ) return false;
			continue;
		}
		if ( strcmp( tempname.c_str(), "/*") == 0 )
		{
			name = "";
			if ( skiptocomment( stream, linecount ) == false ) return false;
			continue;
		}
		stream.Seek( pos, KStream::seekSet );
		
		while ( 1 )
		{
			pos = static_cast<unsigned short>(stream.Tell());
			if ( getNextWord( stream, &c, ws, linecount ) == false )
			{
				token.TokenID = KTOKEN_NAME;
				token.StrToken = name;
				return false;
			}
			if ( ws ) // word end
			{
				token.TokenID = KTOKEN_NAME;
				token.StrToken = name;
				checkReservedWord( token );
				return true;
			}
			for ( int i=0 ; i<_countof(tokendata)/2 ; i++ )
			{
				if ( tokendata[i*2+1] == c )
				{
					stream.Seek( pos, KStream::seekSet );
					token.TokenID = KTOKEN_NAME;
					token.StrToken = name;
					checkReservedWord( token );
					return true;
				}
			}
			name += c;
		}

	}

}

bool findToken( KStream &stream, char *tokenlist, int tokencount, KTemplateToken &found_token, int &linecount ) 
{
	KTemplateToken token;
	bool loop = true;
	while ( loop )
	{
		loop = getNextToken( stream, token, linecount );
		for ( int i=0 ; i<tokencount ; i++ )
		{
			if ( tokenlist[i] == token.TokenID )
			{
				found_token = token;
				return loop;
			}
		}
	}
	found_token.TokenID = KTOKEN_EOF;
	return loop;
}

unsigned long ahtoi( const char *str, int count )
{
	unsigned long result = 0;
	unsigned long hex = 1;
	for ( int i=count-1 ; i>=0 ; i-- )
	{
		if ( str[i] >= 'A' && str[i] <= 'F' )
			result += (str[i]-'A'+10) * hex;
		else if ( str[i] >= 'a' && str[i] <= 'f' )
			result += (str[i]-'a'+10) * hex;
		else result += (str[i]-'0') * hex;
		hex *= 16;
	}
	return result;
}

unsigned short ahtos( const char *str, int count )
{
	unsigned result = 0;
	unsigned short hex = 1;
	for ( int i=count-1 ; i>=0 ; i-- )
	{
		if ( str[i] >= 'A' && str[i] <= 'F' )
			result += ((str[i]-'A'+10) * hex);
		else if ( str[i] >= 'a' && str[i] <= 'f' )
			result += ((str[i]-'a'+10) * hex);
		else result += ((str[i]-'0') * hex);
		hex *= 16;
	}
	return (unsigned short)result;
}

unsigned char ahtoc( const char *str, int count )
{
	unsigned result = 0;
	unsigned char hex = 1;
	for ( int i=count-1 ; i>=0 ; i-- )
	{
		if ( str[i] >= 'A' && str[i] <= 'F' )
			result += ((str[i]-'A'+10) * hex);
		else if ( str[i] >= 'a' && str[i] <= 'f' )
			result += ((str[i]-'a'+10) * hex);
		else result += ((str[i]-'0') * hex);
		hex *= 16;
	}
	return (unsigned char)result;
}

bool GetTemplateInfo( MetaData* pMetaData, KStream& stream, int& linecount )
{
	TemplateInfo* pInfo = NULL;

	KTemplateToken token;
	bool loop = true;

	int stage = 0;

	std::string var_name;
	char var_type_token = 0;
	int var_array_count = 0;
	const TemplateInfo *tinfo = NULL;

	std::string strTemplateName;
	GUID uuidTemplate;
	int i = 0;
	
	int token_count[] = { 1, 1, 1, 1, 1, 14, 1, 2, 2, 1, 1 };
	char token_list[][14] = { 
		{ KTOKEN_TEMPLATE },// template start
		{ KTOKEN_OBRACE },	// guid start
		{ KTOKEN_NAME },	// guid
		{ KTOKEN_CBRACE },	// guid end
		{ KTOKEN_OBRACE },	// member start
		{ KTOKEN_CBRACE, KTOKEN_NAME, KTOKEN_WORD, KTOKEN_SHORT, KTOKEN_DWORD, KTOKEN_LONG, KTOKEN_LLONG, 
		  KTOKEN_FLOAT, KTOKEN_DOUBLE, KTOKEN_CHAR, KTOKEN_BYTE, KTOKEN_STRING, KTOKEN_WSTRING, KTOKEN_DICT }, // member type
		{ KTOKEN_NAME },	// member name
		{ KTOKEN_OBRACKET, KTOKEN_SEMICOLON }, // member sentence end or array start
		{ KTOKEN_CBRACKET, KTOKEN_NAME },	// array count
		{ KTOKEN_CBRACKET }, // array end
		{ KTOKEN_NAME },	// template name
	};
	
	while (loop)
	{
		loop = findToken( stream, token_list[stage], token_count[stage], token, linecount );
		
		if ( token.TokenID == KTOKEN_EOF )
		{
			if ( stage != 0 )
			{
	
#ifdef _DEBUG
				char msg[128];
				s_sprintf( msg, _countof( msg ), "Parser Error line at %d", linecount );
				MessageBox( NULL, msg, "Parser Error", MB_OK | MB_ICONERROR );
#endif
			}
			return false;
		}

		switch( stage )
		{
			case 0: // template start
				stage = 10;
				break;

			case 1: // guid start ex>{3C1EC6CB-A0FD-4998-B420-778A55B57744}
				stage = 2;
				break;

			case 2: // guid name
				if ( token.StrToken.size() == 36 )
				{
					uuidTemplate.Data1 = ahtoi( token.StrToken.substr(0, 8).c_str(), 8 );
					uuidTemplate.Data2 = ahtos( token.StrToken.substr(9, 4).c_str(), 4 );
					uuidTemplate.Data3 = ahtos( token.StrToken.substr(14,4).c_str(), 4 );
					for ( i=0 ; i<2 ; i++ )
					{
						uuidTemplate.Data4[i] = ahtoc( token.StrToken.substr( 19+i*2, 2 ).c_str(), 2 );
					}
					for ( i=2 ; i<8 ; i++ )
					{
						uuidTemplate.Data4[i] = ahtoc( token.StrToken.substr( 24+(i-2)*2, 2 ).c_str(), 2 );
					}
				}
				else
				{
					{
#ifdef _DEBUG
						char msg[128];
						s_sprintf( msg, _countof( msg ), "Parser Error line at %d", linecount );
						MessageBox( NULL, msg, "Parser Error", MB_OK | MB_ICONERROR );
#endif
					}
					return false;
				}

				stage = 3;
				break;

			case 3: // guid end
				pInfo = pMetaData->addNewTemplateInfo( uuidTemplate, strTemplateName.c_str() );
				stage = 4;
				break;

			case 4: // member start
				stage = 5;
				break;

			case 5: // member declare
				switch( token.TokenID )
				{
					case KTOKEN_CBRACE: // end declare template
						return loop;
					case KTOKEN_NAME:
						tinfo = pMetaData->getTemplateInfo( token.StrToken.c_str() );
						if ( pInfo != NULL && tinfo == NULL )
						{
							// check recursive format
							if ( strcmp( token.StrToken.c_str(), pInfo->name() ) == 0 )
							{
								tinfo = pInfo;
							}
							else
							{
								{
#ifdef _DEBUG
									char msg[128];
									s_sprintf( msg, _countof( msg ), "Parser Error line at %d", linecount );
									MessageBox( NULL, msg, "Parser Error", MB_OK | MB_ICONERROR );
#endif
								}
								return false;
							}
						}
						var_type_token = KTOKEN_TEMPLATE;
						var_array_count = 0;
						stage = 6;
						break;
					case KTOKEN_WORD:
					case KTOKEN_SHORT:
					case KTOKEN_DWORD:
					case KTOKEN_LONG:
					case KTOKEN_LLONG:
					case KTOKEN_FLOAT:
					case KTOKEN_DOUBLE:
					case KTOKEN_CHAR:
					case KTOKEN_BYTE:
					case KTOKEN_STRING:
					case KTOKEN_WSTRING:
					case KTOKEN_DICT:
						tinfo = NULL;
						var_type_token = token.TokenID;
						var_array_count = 0;
						stage = 6;
						break;
				}
				break;
			
			case 6: // member name
				var_name = token.StrToken;
				stage = 7;
				break;
			
			case 7: // sentence end or array
			{
				int classid = Type::_NONE;
				switch( token.TokenID )
				{
					case KTOKEN_SEMICOLON: // end sentence

						switch( var_type_token )
						{
							case KTOKEN_TEMPLATE:
								if ( var_array_count )
									classid = Type::TEMPLATE_ARRAY;
								else
									classid = Type::TEMPLATE;
								break;

							case KTOKEN_WORD:
							case KTOKEN_SHORT:
								if ( var_array_count )
									classid = Type::SHORT_ARRAY;
								else
									classid = Type::SHORT;
								break;

							case KTOKEN_DWORD:
							case KTOKEN_LONG:
								if ( var_array_count )
									classid = Type::LONG_ARRAY;
								else
									classid = Type::LONG;
								break;

							case KTOKEN_LLONG:
								if ( var_array_count )
									classid = Type::LLONG_ARRAY;
								else
									classid = Type::LLONG;
								break;

							case KTOKEN_DOUBLE:
								if ( var_array_count )
									classid = Type::DOUBLE_ARRAY;
								else
									classid = Type::DOUBLE;
								break;

							case KTOKEN_FLOAT:
								if ( var_array_count )
									classid = Type::FLOAT_ARRAY;
								else
									classid = Type::FLOAT;
								break;

							case KTOKEN_BYTE:
							case KTOKEN_CHAR:
								if ( var_array_count )
									classid = Type::CHAR_ARRAY;
								else
									classid = Type::CHAR;
								break;

							case KTOKEN_STRING:
								if ( var_array_count )
									classid = Type::STRING_ARRAY;
								else
									classid = Type::STRING;
								var_array_count = -1;
								break;

							case KTOKEN_WSTRING:
								if ( var_array_count )
									classid = Type::WSTRING_ARRAY;
								else
									classid = Type::WSTRING;
								var_array_count = -1;
								break;

							case KTOKEN_DICT:
								classid = Type::DICT;
								break;
						}

						if ( pInfo != NULL )
						{
							if ( tinfo == NULL )
							{
								if ( Type::isSimpleArrayType( classid ) )
									pInfo->addArrayField( var_name.c_str(), Type::getElementType( classid ), var_array_count );
								else if ( classid == Type::DICT )
									pInfo->addDictField( var_name.c_str() );
								else
									pInfo->addSimpleField( var_name.c_str(), classid );
							}
							else
							{
								const TemplateInfo* pMInfo = pMetaData->getTemplateInfo( tinfo->id() );
								if ( classid == Type::TEMPLATE_ARRAY ) 
									pInfo->addTemplateArrayField( var_name.c_str(), pMInfo, var_array_count );
								else
									pInfo->addTemplateField( var_name.c_str(), pMInfo );
							}
						}

						stage = 5;
						break;

					case KTOKEN_OBRACKET:
						var_array_count = 0;
						stage = 8;
						break;
				}
				break;
			}
			
			case 8: // array count
				switch( token.TokenID )
				{
					case KTOKEN_CBRACKET:

						var_array_count = -1;
						stage = 7;
						break;

					case KTOKEN_NAME:

						if ( pInfo != NULL )
						{
							if ( isdigit(token.StrToken.c_str()[0]) ) // constant number
							{
								var_array_count = atoi( token.StrToken.c_str() );
								if ( var_array_count == 0 )
								{
#ifdef _DEBUG
									char msg[128];
									s_sprintf( msg, _countof( msg ), "Constant Size Cannot be 0 Error line at %d", linecount );
									MessageBox( NULL, msg, "Parser Error", MB_OK | MB_ICONERROR );
#endif
								}
							}
							else // variable count
							{
								if ( pInfo->indexBy( token.StrToken.c_str() ) != -1 )
								{
									pInfo->removeField( token.StrToken.c_str() );
								}
								var_array_count = -1;
							}
							if ( var_array_count == 0 )
							{
#ifdef _DEBUG
								char msg[128];
								s_sprintf( msg, _countof( msg ), "Constant Size Cannot be 0 Error line at %d", linecount );
								MessageBox( NULL, msg, "Parser Error", MB_OK | MB_ICONERROR );
#endif
							}
						}
						stage = 9;
						break;
				}
				break;

			case 9: // end array
				stage = 7;
				break;

			case 10: // template name
				strTemplateName = token.StrToken.c_str();
				stage = 1;
				break;
		}	
	}
	return false;
}

} // namespace

bool MetaData::importTML( KStream& stream )
{
	bool loop = true;
	int nLineCount = 1;
	while ( loop )
	{
		loop = GetTemplateInfo( this, stream, nLineCount );
	}

	return true;
}

bool trf::IsTRFStream( KStream& stream )
{
	int idStr;
	stream.Read( &idStr, sizeof(idStr) );
	stream.Seek( -int(sizeof(idStr)), KStream::seekCur );
	return idStr == 'FRTB';
}
