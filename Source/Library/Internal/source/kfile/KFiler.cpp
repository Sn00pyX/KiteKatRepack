// KFiler.cpp: implementation of the KFiler class.
//
//////////////////////////////////////////////////////////////////////

#include "../../include/kfile/KFiler.h"
#include <assert.h>
#include "../../include/kfile/KTemplateInfo.h"
#include "../../include/toolkit/safe_function.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
KFiler::KFiler()
{

}

KFiler::~KFiler()
{
	Clear();
}

KHash<KTemplateInfo*, hashPr_GUID> KFiler::m_hashByGUID;
KHash<KTemplateInfo*, hashPr_string> KFiler::m_hashByName;
std::vector<KTemplateInfo*> KFiler::m_vectorTemplateMember;

namespace
{
void GetFilename( const char *filename, char *buf, size_t buf_count )
{
	int srclen = static_cast<int>(strlen( filename ));

	do
	{
		if ( filename[srclen] == '\\' )
			break;
	} while ( --srclen );

	const char *fname = &filename[srclen];
	s_strcpy( buf, buf_count, fname );
	srclen = static_cast<int>(strlen(buf));
	for ( int i=0 ; i<srclen ; i++ )
	{
		if ( buf[i] == '.' )
			buf[i] = '_';
	}
	s_strcat( buf, buf_count, "_" );
	s_toupper( buf, buf_count );
}
}

void KFiler::Clear()
{
	std::vector<KTemplateDataObject*>::iterator it = m_vectorCreated.begin();
	for ( ; it != m_vectorCreated.end() ; ++it )
	{
		delete (*it);
	}
	m_vectorCreated.clear();
}

void KFiler::GenerateSource( KStream &stream, const char *filename )
{
	std::string src;
	char buf[MAX_PATH];
	GetFilename( filename, buf, _countof( buf ) );
	src = "#ifndef _";
	src += buf;	src += "\n";
	src += "#define _";
	src += buf;	src += "\n";
	src += "#include \"KFiler.h\"\n\n";
	stream.Write( src.c_str(), src.size() );
	std::vector<KTemplateInfo*>::iterator it = m_vectorTemplateMember.begin();
	for ( ; it != m_vectorTemplateMember.end() ; ++it )
	{
		(*it)->GenerateSource( stream );
	}
	src = "#endif";
	stream.Write( src.c_str(), src.size() );
}

KTemplateInfo *KFiler::GetTemplateInfo( const GUID &uuidType )
{
	KTemplateInfo *info = NULL;
	m_hashByGUID.lookup( uuidType, info );
	return info;
}

KTemplateInfo *KFiler::GetTemplateInfo( const char *template_name )
{
	KTemplateInfo *info = NULL;
	m_hashByName.lookup( template_name, info );
	return info;
}

void KFiler::addTemplate( KTemplateInfo *template_data )
{
	m_vectorTemplateMember.push_back( template_data );
	m_hashByGUID.add( template_data->GetTemplateGUID(), template_data );
	m_hashByName.add( template_data->GetTemplateName(), template_data );
}

void KFiler::ClearRegisteredTemplates()
{
	std::vector<KTemplateInfo*>::iterator it = m_vectorTemplateMember.begin();
	for ( ; it != m_vectorTemplateMember.end() ; ++it )
	{
		delete (*it);
	}
	m_vectorTemplateMember.clear();
	m_hashByGUID.clear();
	m_hashByName.clear();
}


// create basic object
KTemplateDataObject* KFiler::CreateTemplateObject( const GUID &uuidType )
{
	KTemplateInfo *info = KFiler::GetTemplateInfo( uuidType );
	if ( info )
	{
		KTemplateDataObject *obj = new KTemplateDataObject( info->GetTemplateGUID() );
		m_vectorCreated.push_back( obj );
		return obj;
	}
	return NULL;
}

KTemplateDataObject* KFiler::CreateTemplateObject( const char *template_name )
{
	KTemplateInfo *info = KFiler::GetTemplateInfo( template_name );
	if ( info )
	{
		KTemplateDataObject *obj = new KTemplateDataObject( info->GetTemplateGUID() );
		m_vectorCreated.push_back( obj );
		return obj;
	}
	return NULL;
}

/*
KSimpleDataObject* KFiler::CreateSimpleDataObject( const KDataObject::CLASSTYPEID &classtype, void *initial_pData, int nSize )
{
	KSimpleDataObject *obj = new KSimpleDataObject( classtype );
	obj->SetData( initial_pData, nSize );
	return obj;
}
*/

// create array object
/*
KTemplateDataArrayObject* KFiler::CreateTemplateArrayObject( const GUID &uuidType, int nCount, const char *countVarName, KTemplateDataObject *pParent )
{
	KTemplateDataArrayObject *obj = new KTemplateDataArrayObject( uuidType );
	obj->SetArrayInfo( nCount, countVarName, pParent );
	m_vectorCreated.push_back( obj );
	return obj;
}
*/

/*
KSimpleDataArrayObject* KFiler::CreateSimpleDataArrayObject( const KDataObject::CLASSTYPEID &classtype, void *initial_pData, int nSize, int nCount, const char *countVarName, KTemplateDataObject *pParent )
{
	KSimpleDataArrayObject *obj = new KSimpleDataArrayObject( classtype );
	obj->SetArrayInfo( nCount, countVarName, pParent );
	return obj;
}
*/

// add new object
void KFiler::AddDataObject( KTemplateDataObject *dataobj )
{
	m_vectorData.push_back( dataobj );
}

// enumeration
int KFiler::GetDataObjectCount()
{
	return static_cast<int>(m_vectorData.size());
}

KTemplateDataObject* KFiler::GetDataObjectAt(int index)
{
	return m_vectorData[index];
}


// template parsing


namespace {
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
	, KTOKEN_DWORD		// dword
	, KTOKEN_FLOAT		// float
	, KTOKEN_DOUBLE		// double
	, KTOKEN_CHAR		// char
	, KTOKEN_STRING		// string
};
		
char *reserved_word[] = {
	"template"
	,"word"
	,"dword"
	,"float"
	,"double"
	,"char"
	,"string"
};
		
char whitespacechr[] = {
	'\n', '\r', ' ', '\t'

};
/*
, KTOKEN_NAME			,
, KTOKEN_INTEGER		,
, KTOKEN_GUID			,
, KTOKEN_INTEGER_LIST	,
, KTOKEN_FLOAT_LIST		,
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
		DWORD pos = static_cast<DWORD>(stream.Tell());
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
			pos = static_cast<DWORD>(stream.Tell());
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
	while( loop )
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

bool getTemplateInfo( KStream &stream, KTemplateInfo &info, int &linecount )
{
	KTemplateToken token;
	bool loop = true;
	int stage = 0;

	std::string var_name;
	char var_type_token = 0;
	std::string var_array_varname;
	int var_array_count = 0;
	KTemplateInfo *tinfo = NULL;

	GUID uuidTemplate;
	int i = 0;
	
	int token_count[] = { 1, 1, 1, 1, 1, 8, 1, 3, 1, 1, 1 };
	char token_list[][10] = { 
		{ KTOKEN_TEMPLATE },// template start
		{ KTOKEN_OBRACE },	// guid start
		{ KTOKEN_NAME },	// guid
		{ KTOKEN_CBRACE },	// guid end
		{ KTOKEN_OBRACE },	// member start
		{ KTOKEN_CBRACE, KTOKEN_NAME, KTOKEN_WORD, KTOKEN_DWORD,
		KTOKEN_FLOAT, KTOKEN_DOUBLE,KTOKEN_CHAR, KTOKEN_STRING },
								// member type
		{ KTOKEN_NAME },	// member name
		{ KTOKEN_OBRACKET, KTOKEN_SEMICOLON }, // member sentence end or array start
		{ KTOKEN_NAME },	// array count
		{ KTOKEN_CBRACKET }, // array end
		{ KTOKEN_NAME },	// template name
	};
	
	while(loop)
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
			info.SetTemplateGUID( uuidTemplate );
			stage = 3;
			break;
		case 3: // guid end
			stage = 4;
			break;
		case 4: // member start
			stage = 5;
			break;
		case 5: // member declare
			{
			switch( token.TokenID )
			{
			case KTOKEN_CBRACE: // end declare template
				return loop;
			case KTOKEN_NAME:
				tinfo = KFiler::GetTemplateInfo( token.StrToken.c_str() );
				if ( tinfo == NULL )
				{
					// check recursive format
					if ( strcmp( token.StrToken.c_str(), info.GetTemplateName() ) == 0 )
					{
						tinfo = &info;
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
			case KTOKEN_DWORD:
			case KTOKEN_FLOAT:
			case KTOKEN_DOUBLE:
			case KTOKEN_CHAR:
			case KTOKEN_STRING:
				tinfo = NULL;
				var_type_token = token.TokenID;
				var_array_count = 0;
				stage = 6;
				break;
			}
			}
			break;
		case 6: // member name
			var_name = token.StrToken;
			stage = 7;
			break;
		case 7: // sentence end or array
			{
			KDataObject::CLASSTYPEID classid = KDataObject::CLASSTYPEID_NONE;
			switch( token.TokenID )
			{
			case KTOKEN_SEMICOLON: // end sentence

				switch( var_type_token )
				{
				case KTOKEN_TEMPLATE:
					if ( var_array_count )
						classid = KDataObject::CLASSTYPEID_TEMPLATEARRAY;
					else
						classid = KDataObject::CLASSTYPEID_TEMPLATE;
					break;
				case KTOKEN_WORD:
					if ( var_array_count )
						classid = KDataObject::CLASSTYPEID_ARRAY_WORD;
					else
						classid = KDataObject::CLASSTYPEID_WORD;
					break;
				case KTOKEN_DWORD:
					if ( var_array_count )
						classid = KDataObject::CLASSTYPEID_ARRAY_DWORD;
					else
						classid = KDataObject::CLASSTYPEID_DWORD;
					break;
				case KTOKEN_DOUBLE:
				case KTOKEN_FLOAT:
					if ( var_array_count )
						classid = KDataObject::CLASSTYPEID_ARRAY_FLOAT;
					else
						classid = KDataObject::CLASSTYPEID_FLOAT;
					break;
				case KTOKEN_CHAR:
					if ( var_array_count )
						classid = KDataObject::CLASSTYPEID_ARRAY_CHAR;
					else
						classid = KDataObject::CLASSTYPEID_CHAR;
					break;
				case KTOKEN_STRING:
					if ( var_array_count )
						classid = KDataObject::CLASSTYPEID_ARRAY_STRING;
					else
						classid = KDataObject::CLASSTYPEID_STRING;
					break;
				}
				info.AddMember( var_name.c_str(), (tinfo==NULL)?(NULL):(tinfo->GetTemplateName()), 
					(tinfo==NULL)?(NULL):(&tinfo->GetTemplateGUID()),
					classid, var_array_count, var_array_varname.c_str() );
				stage = 5;
				break;
			case KTOKEN_OBRACKET:
				var_array_count = 0;
				stage = 8;
				break;
			}
			}
			break;
		case 8: // array count
			if ( isdigit(token.StrToken.c_str()[0]) ) // constant number
			{
				var_array_count = atoi( token.StrToken.c_str() );
				if ( var_array_count == 0 )
				{
#ifdef _DEBUG
					char msg[128];
					s_sprintf( msg, _countof( msg ), "Variable Size Cannot be 0 Error line at %d", linecount );
					MessageBox( NULL, msg, "Parser Error", MB_OK | MB_ICONERROR );
#endif
				}
			}
			else // variable count
			{
				if ( info.IsMember( token.StrToken.c_str() ) )
				{
					var_array_count = -1;
					var_array_varname = token.StrToken;
				}
			}
			if ( var_array_count == 0 )
			{
#ifdef _DEBUG
				char msg[128];
				s_sprintf( msg, _countof( msg ), "Variable Size Cannot be 0 Error line at %d", linecount );
				MessageBox( NULL, msg, "Parser Error", MB_OK | MB_ICONERROR );
#endif
			}
			stage = 9;
			break;
		case 9: // end array
			stage = 7;
			break;

		case 10: // template name
			info.SetTemplateName( token.StrToken.c_str() );
			stage = 1;
			break;
		}	
	}
	return false;
}

}
void KFiler::RegisterTemplates( KStream &streamTemplate )
{
	bool loop = true;
	int nLineCount = 1;
	while ( loop )
	{
		KTemplateInfo *info;
		info = new KTemplateInfo;
		if ( (loop = getTemplateInfo( streamTemplate, *info, nLineCount ) ) == true )
			addTemplate( info );
		else
			delete info;
	}
	atexit( KFiler::ClearRegisteredTemplates );
}

