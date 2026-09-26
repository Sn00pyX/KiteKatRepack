// KTemplateInfo.cpp: implementation of the KTemplateInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "../../include/kfile/KTemplateInfo.h"
#include "../../include/kfile/KDataObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


KTemplateInfo::KTemplateInfo()
{
	m_nMemberCount = 0;
}

KTemplateInfo::~KTemplateInfo()
{
	m_members.clear();
}

KDataObject *KTemplateInfo::CreateMember( int index, KTemplateDataObject *parent )
{
	assert( (unsigned)index < m_members.size() );
	MEMBER &m = m_members[index];

	KDataObject *obj = NULL;
	KDataObject *varCount;
	switch ( m.classType )
	{
	case KDataObject::CLASSTYPEID_WORD:
	case KDataObject::CLASSTYPEID_DWORD:
	case KDataObject::CLASSTYPEID_FLOAT:
	case KDataObject::CLASSTYPEID_CHAR:
	case KDataObject::CLASSTYPEID_UCHAR:
	case KDataObject::CLASSTYPEID_STRING:
		obj = new KSimpleDataObject( static_cast< KDataObject::CLASSTYPEID >( m.classType ) );
		break;
	case KDataObject::CLASSTYPEID_ARRAY_CHAR:
	case KDataObject::CLASSTYPEID_ARRAY_UCHAR:
	case KDataObject::CLASSTYPEID_ARRAY_WORD:
	case KDataObject::CLASSTYPEID_ARRAY_DWORD:
	case KDataObject::CLASSTYPEID_ARRAY_FLOAT:
	case KDataObject::CLASSTYPEID_ARRAY_STRING:
		obj = new KSimpleDataArrayObject( static_cast< KDataObject::CLASSTYPEID >( m.classType ) );
		(reinterpret_cast<KSimpleDataArrayObject*>(obj))->SetArrayInfo( m.arraycount, m.arrayvarname.c_str(), parent );
		break;
	case KDataObject::CLASSTYPEID_TEMPLATE:
		obj = new KTemplateDataObject( m.uuidType );
		break;
	case KDataObject::CLASSTYPEID_TEMPLATEARRAY:
		obj = new KTemplateDataArrayObject( m.uuidType );
		if ( m.arraycount == -1 )
			varCount = parent->GetMemberObject( m.arrayvarname.c_str() );
		else varCount = NULL;
		(reinterpret_cast<KTemplateDataArrayObject*>(obj))->SetArrayInfo( m.arraycount, m.arrayvarname.c_str(), parent );
		break;
	}
	return obj;
}

void KTemplateInfo::GenerateSource( KStream &stream )
{
	generateClassHeader( stream );
	generateMemberSource( stream );
	generateClassFooter( stream );

	generateArrayClassHeader( stream );
	generateArrayMemberSource( stream );
	generateArrayClassFooter( stream );
}

void KTemplateInfo::AddMember( const char *name, const char *classname, const GUID *uuidType, int classType, int nArrayCount, const char *strArrayCountVar )
{
	MEMBER mem;
	mem.name = name;
	if ( uuidType )
	{
		mem.uuidType = *uuidType;
		mem.classname = classname;
	}
	else memset( &mem.uuidType, 0, sizeof(mem.uuidType) );

	mem.classType = classType;
	mem.arraycount = nArrayCount;
	mem.arrayvarname = strArrayCountVar;

	m_members.push_back( mem );
	m_hashIndexByName.add( name, m_nMemberCount );
	m_nMemberCount++;
}

/*
std::string m_gen_classname; // %cn%
std::string m_gen_membername; // %mn%
std::string m_gen_membertype; // %mt%
std::string m_gen_membervartype; // %mv%
std::string m_gen_memberclassname; // %mc%
*/

void KTemplateInfo::replaceGenSrc( std::string &src )
{
	int npos = static_cast<int>(src.find( "%" )); // find first

	while ( npos >= 0 && npos < (int)src.size() )
	{
		std::string token = src.substr( npos, 4 );
		std::string target;
		
		bool found = false;

		if ( token.compare( "%cn%") == 0 )
			target = m_gen_classname, found = true;
		else if ( token.compare( "%mn%" ) == 0 )
			target = m_gen_membername, found = true;
		else if ( token.compare( "%mt%" ) == 0 )
			target = m_gen_membertype, found = true;
		else if ( token.compare( "%mv%" ) == 0 )
			target = m_gen_membervartype, found = true;
		else if ( token.compare( "%mm%" ) == 0 )
			target = m_gen_membervarmethod, found = true;
		else if ( token.compare( "%mc%" ) == 0 )
			target = m_gen_memberclassname, found = true;

		if ( found )
			src.replace( npos, 4, target );
		else npos++;

		npos = static_cast<int>(src.find( "%", npos ));
	}
}

void KTemplateInfo::setTypeInfo( int type )
{
	switch( type )
	{
	case KDataObject::CLASSTYPEID_WORD:
		m_gen_membervartype = "unsigned short";
		m_gen_membervarmethod = "WORD";
		m_gen_membertype = "KSimpleDataObject";
		break;
	case KDataObject::CLASSTYPEID_DWORD:
		m_gen_membervartype = "unsigned long";
		m_gen_membervarmethod = "DWORD";
		m_gen_membertype = "KSimpleDataObject";
		break;
	case KDataObject::CLASSTYPEID_FLOAT:
		m_gen_membervartype = "float";
		m_gen_membervarmethod = "Float";
		m_gen_membertype = "KSimpleDataObject";
		break;
	case KDataObject::CLASSTYPEID_CHAR:
		m_gen_membervartype = "char";
		m_gen_membervarmethod = "Char";
		m_gen_membertype = "KSimpleDataObject";
		break;
	case KDataObject::CLASSTYPEID_UCHAR:
		m_gen_membervartype = "unsigned char";
		m_gen_membervarmethod = "UChar";
		m_gen_membertype = "KSimpleDataObject";
		break;
	case KDataObject::CLASSTYPEID_STRING:
		m_gen_membervartype = "char *";
		m_gen_membervarmethod = "String";
		m_gen_membertype = "KSimpleDataObject";
		break;			
	case KDataObject::CLASSTYPEID_ARRAY_CHAR:
		m_gen_membervartype = "char";
		m_gen_membervarmethod = "Char";
		m_gen_membertype = "KSimpleDataArrayObject";
		break;
	case KDataObject::CLASSTYPEID_ARRAY_UCHAR:
		m_gen_membervartype = "unsigned char";
		m_gen_membervarmethod = "UChar";
		m_gen_membertype = "KSimpleDataArrayObject";
		break;
	case KDataObject::CLASSTYPEID_ARRAY_WORD:
		m_gen_membervartype = "unsigned short";
		m_gen_membervarmethod = "WORD";
		m_gen_membertype = "KSimpleDataArrayObject";
		break;
	case KDataObject::CLASSTYPEID_ARRAY_DWORD:
		m_gen_membervartype = "unsigned long";
		m_gen_membervarmethod = "DWORD";
		m_gen_membertype = "KSimpleDataArrayObject";
		break;
	case KDataObject::CLASSTYPEID_ARRAY_FLOAT:
		m_gen_membervartype = "float";
		m_gen_membervarmethod = "Float";
		m_gen_membertype = "KSimpleDataArrayObject";
		break;
	case KDataObject::CLASSTYPEID_ARRAY_STRING:
		m_gen_membervartype = "char *";
		m_gen_membervarmethod = "String";
		m_gen_membertype = "KSimpleDataArrayObject";
		break;
	case KDataObject::CLASSTYPEID_TEMPLATE:
		m_gen_membervartype = "";
		m_gen_membervarmethod = "";
		m_gen_membertype = "KTemplateDataObject";
		break;
	case KDataObject::CLASSTYPEID_TEMPLATEARRAY:
		m_gen_membervartype = "";
		m_gen_membervarmethod = "";
		m_gen_membertype = "KTemplateDataArrayObject";
		break;
	}
}

static char *s_strClassHeader0 = "class KFT_%cn%\n{\npublic:\n\tKFT_%cn%()\n\t{\n\t\tm_pObj = NULL;\n";
static char *s_strClassHeader0_m = "\t\tm_pMem_%mn% = NULL;\n";
static char *s_strClassHeader1 = "\tKFT_%cn%( KFiler *filer )\n\t{\n\t\tm_pObj = filer->CreateTemplateObject( \"%cn%\" );\n";
static char *s_strClassHeader1_m = "\t\tm_pMem_%mn% = reinterpret_cast<%mt%*>(m_pObj->GetMemberObject( \"%mn%\" ));\n";
static char *s_strClassHeader2 = "\tKFT_%cn%( KTemplateDataObject *pObj )\n\t{\n\t\tm_pObj = pObj;\n";
static char *s_strClassHeader2_m = "\t\tm_pMem_%mn% = reinterpret_cast<%mt%*>(m_pObj->GetMemberObject( \"%mn%\" ));\n";
static char *s_strClassHeader3 = "\t~KFT_%cn%()\n\t{\n\t}\n";
static char *s_strClassHeader4 = "\tvoid SetDataObject( KTemplateDataObject *pObj )\n\t{\n\t\tm_pObj = pObj;\n";
static char *s_strClassHeader4_m = "\t\tm_pMem_%mn% = reinterpret_cast<%mt%*>(m_pObj->GetMemberObject( \"%mn%\" ));\n";

void KTemplateInfo::generateClassHeader( KStream &stream )
{
	m_gen_classname = m_strTemplateName;
	std::string src;
	src = s_strClassHeader0;
	replaceGenSrc( src );
	stream.Write( src.c_str(), src.size() );
	int i;
	for ( i=0 ; i<m_nMemberCount ; ++i )
	{
		MEMBER &mem = m_members[i];
		m_gen_membername = mem.name;
		setTypeInfo( mem.classType );
		src = s_strClassHeader0_m;
		replaceGenSrc( src );
		stream.Write( src.c_str(), src.size() );
	}
	stream.Write( "\t}\n", 3 );
	src = s_strClassHeader1;
	replaceGenSrc( src );
	stream.Write( src.c_str(), src.size() );
	for ( i=0 ; i<m_nMemberCount ; ++i )
	{
		MEMBER &mem = m_members[i];
		m_gen_membername = mem.name;
		setTypeInfo( mem.classType );
		src = s_strClassHeader1_m;
		replaceGenSrc( src );
		stream.Write( src.c_str(), src.size() );
	}
	stream.Write( "\t}\n", 3 );
	src = s_strClassHeader2;
	replaceGenSrc( src );
	stream.Write( src.c_str(), src.size() );
	for ( i=0 ; i<m_nMemberCount ; ++i )
	{
		MEMBER &mem = m_members[i];
		m_gen_membername = mem.name;
		setTypeInfo( mem.classType );
		src = s_strClassHeader2_m;
		replaceGenSrc( src );
		stream.Write( src.c_str(), src.size() );
	}
	stream.Write( "\t}\n", 3 );
	src = s_strClassHeader3;
	replaceGenSrc( src );
	stream.Write( src.c_str(), src.size() );
	src = s_strClassHeader4;
	replaceGenSrc( src );
	stream.Write( src.c_str(), src.size() );
	for ( i=0 ; i<m_nMemberCount ; ++i )
	{
		MEMBER &mem = m_members[i];
		m_gen_membername = mem.name;
		setTypeInfo( mem.classType );
		src = s_strClassHeader4_m;
		replaceGenSrc( src );
		stream.Write( src.c_str(), src.size() );
	}
	stream.Write( "\t}\n", 3 );
}


static char *s_strClassMember_simple = "\t%mv% GetValue_%mn%()\n\t{\n\t\t%mv% res;\n\t\tm_pMem_%mn%->Get%mm%( &res );\n\t\treturn res;\n\t}\n\
\tvoid SetValue_%mn%( %mv% value )\n\t{\n\t\tm_pMem_%mn%->Set%mm%( value );\n\t}\n";
static char *s_strClassMember_simplestring = "\tbool GetString_%mn%( char *data, int &nLen )\n\t{\n\t\treturn m_pMem_%mn%->GetString( data, nLen );\n\t}\n\
\tbool SetString_%mn%( const char *data, int nLen )\n\t{\n\t\treturn m_pMem_%mn%->SetString( data, nLen );\n\t}\n";

static char *s_strClassMember_simplearray = "\tint GetArrayCount_%mn%()\n\t{\n\t\treturn m_pMem_%mn%->GetDataCount();\n\t}\n\
\tvoid GetArrayValue_%mn%( int index, %mv% *data, int count = 1 )\n\t{\n\t\tm_pMem_%mn%->GetData%mm%( index, data, count );\n\t}\n\
\tvoid RefreshArray_%mn%()\n\t{\n\t\tm_pMem_%mn%->RefreshArray();\n\t}\n\
\tvoid SetArrayValue_%mn%( int index, const %mv% *data, int count = 1 )\n\t{\n\t\tm_pMem_%mn%->SetData%mm%( index, data, count );\n\t}\n";

static char *s_strClassMember_simplestringarray = "\tint GetArrayCount_%mn%()\n\t{\n\t\treturn m_pMem_%mn%->GetDataCount();\n\t}\n\
\tconst char *GetArrayString_%mn%( int index )\n\t{\n\t\treturn m_pMem_%mn%->GetDataString( index );\n\t}\n\
\tvoid RefreshArray_%mn%()\n\t{\n\t\tm_pMem_%mn%->RefreshArray();\n\t}\n\
\tbool SetArrayString_%mn%( int index, const char *data )\n\t{\n\t\treturn m_pMem_%mn%->SetDataString( index, data );\n\t}\n";

static char *s_strClassMember_template = "\tvoid GetMemberTemplate_%mn%( KFT_%mc% &obj )\n\t{\n\t\tobj.SetData( m_pMem_%mn% );\n\t}\n";
static char *s_strClassMember_templatearray = "\tvoid GetMemberTemplateArray_%mn%( KFTA_%mc% &obj )\n\t{\n\t\tobj.SetData( m_pMem_%mn% );\n\t}\n";


void KTemplateInfo::generateMemberSource( KStream &stream )
{
	std::string src;
	for ( int i=0 ; i<m_nMemberCount ; ++i )
	{
		MEMBER &mem = m_members[i];
		m_gen_membername = mem.name;
		setTypeInfo( mem.classType );
		switch( mem.classType )
		{
		case KDataObject::CLASSTYPEID_WORD:
		case KDataObject::CLASSTYPEID_DWORD:
		case KDataObject::CLASSTYPEID_FLOAT:
		case KDataObject::CLASSTYPEID_CHAR:
		case KDataObject::CLASSTYPEID_UCHAR:
			src = s_strClassMember_simple;
			m_gen_memberclassname = "";
			break;			
		case KDataObject::CLASSTYPEID_STRING:
			src = s_strClassMember_simplestring;
			m_gen_memberclassname = "";
			break;			
		case KDataObject::CLASSTYPEID_ARRAY_CHAR:
		case KDataObject::CLASSTYPEID_ARRAY_UCHAR:
		case KDataObject::CLASSTYPEID_ARRAY_WORD:
		case KDataObject::CLASSTYPEID_ARRAY_DWORD:
		case KDataObject::CLASSTYPEID_ARRAY_FLOAT:
			src = s_strClassMember_simplearray;
			m_gen_memberclassname = "";
			break;
		case KDataObject::CLASSTYPEID_ARRAY_STRING:
			src = s_strClassMember_simplestringarray;
			m_gen_memberclassname = "";
			break;
		case KDataObject::CLASSTYPEID_TEMPLATE:
			src = s_strClassMember_template;
			m_gen_memberclassname = mem.classname;
			break;
		case KDataObject::CLASSTYPEID_TEMPLATEARRAY:
			src = s_strClassMember_templatearray;
			m_gen_memberclassname = mem.classname;
			break;
		}
		replaceGenSrc( src );
		stream.Write( src.c_str(), src.size() );
	}
}

static char *s_strFooter = "\tvoid Save( KFiler &filer )\n\t{\n\t\tfiler.AddDataObject( m_pObj );\n\t}\
\n\tstatic bool Check( KTemplateDataObject *pObj)\n\t{\n\t\treturn strcmp( pObj->GetTemplateName(), \"%cn%\" ) == 0;\n\t}\n\
private:\n\tKTemplateDataObject*\t\tm_pObj;\n";
static char *s_strFooter_m = "\t%mt%*\t\tm_pMem_%mn%;\n";

void KTemplateInfo::generateClassFooter( KStream &stream )
{
	std::string src = s_strFooter;
	replaceGenSrc( src );
	stream.Write( src.c_str(), src.size() );

	for ( int i=0 ; i<m_nMemberCount ; ++i )
	{
		MEMBER &mem = m_members[i];
		m_gen_membername = mem.name;
		setTypeInfo( mem.classType );
		src = s_strFooter_m;
		replaceGenSrc( src );
		stream.Write( src.c_str(), src.size() );
	}
	stream.Write( "};\n", 3 );
}

static char *s_strArrayClassHeader = "class KFTA_%cn%\n{\npublic:\n\
\tKFTA_%cn%()\n\t{\n\t\tm_pObj = NULL;\n\t\tm_pParent = NULL;\n\t}\n\
\tKFTA_%cn%( KTemplateDataArrayObject* pObj )\n\t{\n\t\tm_pObj = pObj;\n\t\tm_pParent = m_pObj->GetParent();\n\t}\n\
\tKFTA_%cn%( KTemplateDataObject *pParent )\n\t{\n\t\tm_pParent = pParent;\n\t\tm_pObj = pParent->GetTemplateDataArrayMemberObject( \"%cn%\" );\n\t}\n\
\t~KFTA_%cn%()\n\t{\n\t}\n";

void KTemplateInfo::generateArrayClassHeader( KStream &stream )
{
	std::string src;
	m_gen_classname = m_strTemplateName;
	src = s_strArrayClassHeader;
	replaceGenSrc( src );
	stream.Write( src.c_str(), src.size() );
}

static char *s_strArrayClassBody = "\tvoid SetData( KTemplateDataArrayObject *pObj )\n\t{\n\t\tm_pObj = pObj;\n\t\tm_pParent = m_pObj->GetParent();\n\t}\n\
\tvoid SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )\n\t{\n\t\tm_pObj->SetArrayInfo( nCount, varCount, pParent );\n\t}\n\
\tvoid RefreshArray()\n\t{\n\t\tm_pObj->RefreshArray();\n\t}\n\
\tint GetDataCount()\n\t{\n\t\treturn m_pObj->GetDataCount();\n\t}\n\
\tvoid GetData( int index, KFT_%cn% &obj )\n\t{\n\t\tobj.SetDataObject( m_pObj->GetData( index ) );\n\t}\n";

void KTemplateInfo::generateArrayMemberSource( KStream &stream )
{
	std::string src;
	m_gen_classname = m_strTemplateName;
	src = s_strArrayClassBody;
	replaceGenSrc( src );
	stream.Write( src.c_str(), src.size() );
}

static char *s_strArrayClassFooter = "\nprivate:\n\tKTemplateDataObject*\t\tm_pParent;\n\tKTemplateDataArrayObject*\t\tm_pObj;\n};\n";

void KTemplateInfo::generateArrayClassFooter( KStream &stream )
{
	std::string src;
	m_gen_classname = m_strTemplateName;
	src = s_strArrayClassFooter;
	replaceGenSrc( src );
	stream.Write( src.c_str(), src.size() );
}
