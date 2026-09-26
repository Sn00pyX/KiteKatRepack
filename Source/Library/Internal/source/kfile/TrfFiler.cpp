#include "../../include/kfile/trffiler.h"
#include <algorithm>
#include <vector>
#include <assert.h>


namespace trf {

template< typename T > static inline T min2( T a, T b ) { return a < b ? a : b; }
template< typename T > static inline T max2( T a, T b ) { return a > b ? a : b; }

template< typename T > static inline T toAlign4( T a )	{ return (a%4)>0 ? 4-(a%4) : 0; }

class FilerFixedArray;
class FilerArray;
class FilerTemplate;
class FilerFixedTemplateArray;
class FilerTemplateArray;
class FilerDict;


struct MergeTable
{
	std::vector< unsigned short > id;
	std::vector< long > str;
	std::vector< long > wstr;
};

struct Imp
{
	MetaData* m_pMetaData;
	FilerDict* m_pRoot;
	Filer::TInitTemplateFn m_pTemplateInitCallback;

	struct StrRec
	{
		std::string str;
		int index;
	};

	struct WStrRec
	{
		std::wstring str;
		int index;
	};

	std::vector< StrRec > m_stringTable;
	std::vector< WStrRec > m_wstringTable;
	std::vector< GUID > m_idTable;

	bool m_bUnpackedDict;

	bool saveIdTable( KStream& stream );
	bool loadIdTable( KStream& stream, std::vector< unsigned short >& idMergeTable );
	void applyStringIndex();
	bool saveStringTable( KStream& stream );
	bool loadStringTable( KStream& stream, std::vector< long >& strMergeTable, std::vector< long >& wstrMergeTable );

	const char* getString( int i )
	{
		assert( i >= 0 && i <= int( m_stringTable.size() ) );
		return i == 0 ? NULL : m_stringTable[ i-1 ].str.c_str();
	}

	int getStringIndex( const char* pString )
	{
		if ( pString == NULL || *pString == 0 )
			return 0;

		for ( int i = 0, n = static_cast< int >( m_stringTable.size() ); i < n; i++ )
			if ( !strcmp( pString, m_stringTable[ i ].str.c_str() ) )
				return i+1;

		return -1;
	}

	int addString( const char* pString )
	{
		int index = getStringIndex( pString );
		if ( index >= 0 ) 
			return index;

		StrRec rec;
		rec.str = pString;
		rec.index = -1;
		m_stringTable.push_back( rec );
		return int( m_stringTable.size() );
	}

	const wchar_t* getWString( int i )
	{
		assert( i >= 0 && i <= int( m_wstringTable.size() ) );
		return i == 0 ? NULL : m_wstringTable[ i-1 ].str.c_str();
	}

	int getWStringIndex( const wchar_t* pString )
	{
		if ( pString == NULL || *pString == 0 )
			return 0;

		for ( int i = 0, n = static_cast< int >( m_wstringTable.size() ); i < n; i++ )
			if ( !wcscmp( pString, m_wstringTable[ i ].str.c_str() ) )
				return i+1;

		return -1;
	}

	int addWString( const wchar_t* pString )
	{
		int index = getWStringIndex( pString );
		if ( index >= 0 ) 
			return index;

		WStrRec rec;
		rec.str = pString;
		rec.index = -1;
		m_wstringTable.push_back( rec );
		return int( m_wstringTable.size() );
	}

	int setStringIndex( int i, int& newIndex )
	{
		if ( i == 0 )
			return 0;

		int& index = m_stringTable[ i-1 ].index;
		if ( index == -1 )
			index = newIndex++;
		return index;
	}

	int setWStringIndex( int i, int& newIndex )
	{
		if ( i == 0 )
			return 0;

		int& index = m_wstringTable[ i-1 ].index;
		if ( index == -1 )
			index = newIndex++;
		return index;
	}

	const GUID* getId( int i )
	{
		assert( i >= 0 && i <= int( m_idTable.size() ) );
		return i == 0 ? NULL : &m_idTable[ i-1 ];
	}

	int getIdIndex( const GUID* id )
	{
		if ( id == NULL )
			return 0;

		for ( int i = 0, n = static_cast< int >( m_idTable.size() ); i < n; i++ )
			if ( !memcmp( id, &m_idTable[ i ], sizeof(*id) ) )
				return i+1;

		return -1;
	}

	int addId( const GUID* id )
	{
		int index = getIdIndex( id );
		if ( index >= 0 ) 
			return index;

		m_idTable.push_back( *id );
		return int( m_idTable.size() );
	}

	std::vector< ATInfo > m_fieldLimits;

	void initATIs()
	{
		int n = m_pMetaData->getTemplateInfoCount();
		m_fieldLimits.resize( n );
		for ( int i = 0; i < n; i++ )
		{
			ATInfo& fl = m_fieldLimits[ i ];
			fl.pInfo = m_pMetaData->getTemplateInfoAt( i );
			fl.fieldLimit = fl.pInfo->count();
			fl.fixedSize = fl.pInfo->calcFixedSize( fl.fieldLimit );
		}
		
	}

	const ATInfo* getATI( int n ) 
	{ 
		return &m_fieldLimits[ n ]; 
	}
};


union Var
{
	char				asChar;
	short				asShort;
	long				asLong;
	long long			asLLong;
	float				asFloat;
	double				asDouble;
	FilerArray*			asArray;
	FilerTemplate*		asTemplate;
	FilerTemplateArray*	asTemplateArray;
	FilerDict*			asDict;
};


// =================================================================================================


class FilerTemplate: public ITemplate
{
public:

	FilerTemplate( Imp* pParent, const ATInfo* pFL )
	{
		m_pParent = pParent;
		init( pFL );
	}

	~FilerTemplate()
	{
		close();
	}

	void close();

	int readFixedData( void* pOutBuf, int bufSize = 0 ) const;

	const void* getSimpleDataAt( int i, int type ) const
	{
		assert( i >= 0 && i < count() && typeAt( i ) == type );

		switch ( type )
		{
			case Type::CHAR:	return &m_fields[ i ].asChar;
			case Type::SHORT:	return &m_fields[ i ].asShort;
			case Type::STRING:
			case Type::WSTRING:
			case Type::LONG:	return &m_fields[ i ].asLong;
			case Type::LLONG:	return &m_fields[ i ].asLLong;
			case Type::FLOAT:	return &m_fields[ i ].asFloat;
			case Type::DOUBLE:  return &m_fields[ i ].asDouble;
			default:
				assert( false );
				return NULL;
		}
	}

	const char*	getStringValue( long strIndex ) const
	{
		return m_pParent->getString( strIndex );
	}

	const wchar_t* getWStringValue( long wstrIndex ) const
	{
		return m_pParent->getWString( wstrIndex );
	}

	const IArray* getArrayAt( int i ) const;
	const ITemplate* getTemplateAt( int i ) const;
	const ITemplateArray* getTemplateArrayAt( int i ) const;
	const IDict* getDictAt( int i ) const;

	//

	void init( const ATInfo* pFL );
	
	void clear()
	{
		close();
		init( m_pATI );
	}

	bool setSimpleDataAt( int i, const void* pData, int type )
	{
		if ( i < 0 || i >= count() || typeAt( i ) != type )
			return false;

		switch ( type )
		{
			case Type::CHAR:	m_fields[ i ].asChar = *reinterpret_cast< const char* >( pData ); break;
			case Type::SHORT:	m_fields[ i ].asShort = *reinterpret_cast< const short* >( pData ); break;
			case Type::STRING:
			case Type::WSTRING:
			case Type::LONG:	m_fields[ i ].asLong = *reinterpret_cast< const long* >( pData ); break;
			case Type::LLONG:	m_fields[ i ].asLLong = *reinterpret_cast< const long long* >( pData ); break;
			case Type::FLOAT:	m_fields[ i ].asFloat = *reinterpret_cast< const float* >( pData ); break;
			case Type::DOUBLE:  m_fields[ i ].asDouble = *reinterpret_cast< const double* >( pData ); break;
			default:
				return false;
		}

		return true;
	}

	long addStringValue( const char* pStrValue )
	{ 
		return m_pParent->addString( pStrValue );
	}

	long addWStringValue( const wchar_t* pWStrValue )
	{ 
		return m_pParent->addWString( pWStrValue );
	}

	IArray* arrayAt( int i );
	ITemplate* templateAt( int i );
	ITemplateArray* templateArrayAt( int i );
	IDict* dictAt( int i );

	//

	void makeTables( int& newStrIndex, int& newWStrIndex );
	int save( KStream& stream, bool bID, bool bInfoUnknown );
	bool load( KStream& stream, bool bID, bool bInfoUnknown, MergeTable& mergeTable );

protected:

	Imp* m_pParent;

	std::vector< Var > m_fields;
};


// -----------------------------------------------------------------------------------


class FilerTemplateArray: public ITemplateArray
{
public:

	FilerTemplateArray( Imp* pParent, const ATInfo* pFL )
	{
		m_pParent = pParent;
		m_pATI = pFL;
	}

	virtual ~FilerTemplateArray()
	{
		clear();
	}

	int	count() const
	{
		return int( m_fields.size() );
	}

	int readFixedData( int i, int n, void* pOutBuf, int bufSize ) const
	{
		int elementSize = fixedSize();
		if ( elementSize <= 0 )
			return -1;

		if ( i < 0 || n < 0 || i >= count() || pOutBuf == NULL || bufSize <= 0 )
			return 0;

		if ( i+n > count() ) 
			n = count()-i;

		if ( bufSize > 0 && bufSize < n * elementSize )
			n = bufSize / elementSize;

		char* p = reinterpret_cast< char* >( pOutBuf );
		for ( int j = i; n--; j++ )
			m_fields[ i ]->readFixedData( p );

		return n * elementSize;
	}

	const ITemplate* getTemplateAt( int i ) const
	{
		assert( i >= 0 && i < int( m_fields.size() ) );
		return m_fields[ i ];
	}

	//

	void clear()
	{
		for ( int i = 0, n = count(); i < n; i++ )
		{
			if ( m_fields[ i ] )
				delete m_fields[ i ];
		}

		m_fields.clear();
	}

	ITemplate* addNewTemplate()
	{
		m_fields.push_back( new FilerTemplate( m_pParent, m_pATI ) );
		return m_fields.back();
	}

	ITemplate* templateAt( int i )
	{
		if ( i < 0 || i >= count() )
			return NULL;

		return m_fields[ i ];
	}

	void makeTables( int& newStrIndex, int& newWStrIndex );
	virtual int save( KStream& stream );
	virtual bool load( KStream& stream, MergeTable& mergeTable );

protected:

	Imp* m_pParent;

	std::vector< FilerTemplate* > m_fields;
};

class FilerFixedTemplateArray: public FilerTemplateArray
{
public:

	FilerFixedTemplateArray( Imp* pParent, const ATInfo* pFL, int count )
		: FilerTemplateArray( pParent, pFL )
	{
		m_maxCount = count;
	}

	//

	ITemplate* addNewTemplate()
	{
		if ( int( m_fields.size() ) >= m_maxCount )
			return NULL;

		m_fields.push_back( new FilerTemplate( m_pParent, m_pATI ) );
		return m_fields.back();
	}

	int save( KStream& stream );
	bool load( KStream& stream, MergeTable& mergeTable );

protected:

	int m_maxCount;
};


// -----------------------------------------------------------------------------------


class FilerArray: public IArray
{
public:

	FilerArray( Imp* pParent )
	{
		m_pParent = pParent;
		m_elemType = Type::_NONE;
		m_elemSize = 0;
	}

	FilerArray( Imp* pParent, int elemType )
	{
		m_pParent = pParent;
		init( elemType );
	}

	virtual ~FilerArray()
	{
		clear();
	}

	int count() const
	{
		return int( m_buf.size() ) / m_elemSize;
	}

	int type() const
	{
		return m_elemType;
	}

	int readFixedData( int i, int n, void* pOutBuf, int bufSize ) const
	{
		if ( i < 0 || n < 0 || i >= count() || pOutBuf == NULL || bufSize <= 0 )
			return 0;

		if ( i+n > count() ) 
			n = count()-i;

		if ( bufSize > 0 && bufSize < n * m_elemSize )
			n = bufSize / m_elemSize;

		if ( n > 0 )
			s_memcpy( pOutBuf, bufSize, &m_buf.front() + i * m_elemSize, n * m_elemSize );
		return n * m_elemSize;
	}

	const void* getSimpleDataArray( int start, int type ) const
	{
		check_error( start, type );
		return &m_buf.front() + start * m_elemSize;
	}

	const char*	getStringValue( long strIndex ) const
	{
		return m_pParent->getString( strIndex );
	}

	const wchar_t* getWStringValue( long wstrIndex ) const
	{
		return m_pParent->getWString( wstrIndex );
	}

	void init( int elemType )
	{
		m_elemType = static_cast< unsigned char >( elemType );
		m_elemSize = Type::getTypeSize( elemType );
		
		assert( Type::isSimpleType( elemType ) );
	}

	void clear()
	{
		m_buf.clear();
	}

	bool addSimpleDataArray( const void* pData, int type, int count )
	{
		if ( m_elemType != type || count < 0 )
			 return false;

		int size = static_cast< int >( m_buf.size() );
		m_buf.resize( size + count * m_elemSize );
		if ( count > 0 )
			s_memcpy( &m_buf.front() + size, m_buf.size() - size, pData, count * m_elemSize );

		return true;
	}

	bool setSimpleDataAt( int i, const void* pData, int type )
	{
		if ( i < 0 || i >= count() || m_elemType != type )
			return false;

		s_memcpy( &m_buf.front() + (m_elemSize*i), m_buf.size() - (m_elemSize*i), pData, m_elemSize );
		return true;
	}

	long addStringValue( const char* pStrValue )
	{ 
		return m_pParent->addString( pStrValue );
	}

	long addWStringValue( const wchar_t* pWStrValue )
	{ 
		return m_pParent->addWString( pWStrValue );
	}

	void makeTables( int& newStrIndex, int& newWStrIndex );
	virtual int save( KStream& stream );
	virtual bool load( KStream& stream, MergeTable& mergeTable );

protected:

	void check_error( int i, int type ) const
	{
		assert( i >= 0 && i < count() );
		assert( type == m_elemType );
	}

	Imp*	m_pParent;
	unsigned char	m_elemType;
	int				m_elemSize;

	std::vector< char > m_buf;

};

class FilerFixedArray: public FilerArray
{
public:

	FilerFixedArray( Imp* pParent, int elemType, int count )
		: FilerArray( pParent, elemType )
	{
		m_maxCount = count;
	}

	bool addSimpleDataArray( const void* pData, int type, int count )
	{
		if ( this->count() + count > m_maxCount )
			return false;

		return FilerArray::addSimpleDataArray( pData, type, count );

	}

	int save( KStream& stream );
	bool load( KStream& stream, MergeTable& mergeTable );

protected:

	int m_maxCount;
};


// -----------------------------------------------------------------------------------


class FilerDict: public IDict
{
public:

	FilerDict( Imp* pParent )
	{
		m_pParent = pParent;
	}

	~FilerDict()
	{
		clear();
	}

	int indexBy( const char* pName ) const
	{
		if ( pName == NULL )
			return -1;

		int name = m_pParent->getStringIndex( pName );
		for ( int i = 0, n = static_cast< int >( m_fields.size() ); i < n; i++ )
		{
			if ( m_fields[ i ].name == name )
				return i;
		}
		return -1;
	}

	int count() const
	{
		return int( m_fields.size() );
	}

	const char* nameAt( int i ) const
	{
		assert( i >= 0 && i < count() );
		return m_pParent->getString( m_fields[ i ].name );
	}

	int typeAt( int i ) const
	{
		assert( i >= 0 && i < count() );
		return m_fields[ i ].type;
	}

	int fixedSizeAt( int i ) const
	{
		int type = typeAt( i );

		if ( Type::isSimpleType( type ) )
			return Type::getTypeSize( type );
		else if ( type == Type::TEMPLATE )
			return m_fields[ i ].value.asTemplate->fixedSize();
		else 
			return -1;
	}

	const void* getSimpleDataAt( int i, int type ) const
	{
		assert( i >= 0 && i < count() && typeAt( i ) == type );
		switch ( type )
		{
			case Type::CHAR:	return &m_fields[ i ].value.asChar;
			case Type::SHORT:	return &m_fields[ i ].value.asShort;
			case Type::STRING:  
			case Type::WSTRING: 
			case Type::LONG:	return &m_fields[ i ].value.asLong;
			case Type::LLONG:	return &m_fields[ i ].value.asLLong;
			case Type::FLOAT:	return &m_fields[ i ].value.asFloat;
			case Type::DOUBLE:	return &m_fields[ i ].value.asDouble;
			default:
				assert( false );
				return NULL;
		}
	}

	const char*	getStringValue( long strIndex ) const
	{
		return m_pParent->getString( strIndex );
	}

	const wchar_t* getWStringValue( long wstrIndex ) const
	{
		return m_pParent->getWString( wstrIndex );
	}

	const IArray* getArrayAt( int i ) const
	{
		assert( i >= 0 && i < count() && Type::isSimpleArrayType( typeAt( i ) ) );
		return m_fields[ i ].value.asArray;
	}

	const ITemplate* getTemplateAt( int i ) const
	{
		assert( i >= 0 && i < count() && typeAt( i ) == Type::TEMPLATE );
		return m_fields[ i ].value.asTemplate;
	}

	const ITemplateArray* getTemplateArrayAt( int i ) const
	{
		assert( i >= 0 && i < count() && typeAt( i ) == Type::TEMPLATE_ARRAY );
		return m_fields[ i ].value.asTemplateArray;
	}

	const IDict* getDictAt( int i ) const
	{
		assert( i >= 0 && i < count() && typeAt( i ) == Type::DICT );
		return m_fields[ i ].value.asDict;
	}

	void clear()
	{
		for ( int i = 0, n = static_cast< int >( m_fields.size() ); i < n; i++ )
		{
			int type = typeAt( i );
			if ( Type::isSimpleArrayType( type ) )
				delete m_fields[ i ].value.asArray;
			else if ( type == Type::TEMPLATE_ARRAY )
				delete m_fields[ i ].value.asTemplateArray;
			else if ( type == Type::TEMPLATE )
				delete m_fields[ i ].value.asTemplate;
			else if ( type == Type::DICT )
				delete m_fields[ i ].value.asDict;
		}

		m_fields.clear();
	}

	bool addSimpleData( const char* pName, const void* pData, int type )
	{
		if ( indexBy( pName ) != -1 )
			return false;

		_add_data( pName, type );
		
		switch ( type )
		{
			case Type::CHAR:	m_fields.back().value.asChar = *reinterpret_cast< const char* >( pData ); break;
			case Type::SHORT:	m_fields.back().value.asShort = *reinterpret_cast< const short* >( pData ); break;
			case Type::STRING:  
			case Type::WSTRING: 
			case Type::LONG:	m_fields.back().value.asLong = *reinterpret_cast< const long* >( pData ); break;
			case Type::LLONG:	m_fields.back().value.asLLong = *reinterpret_cast< const long long* >( pData ); break;
			case Type::FLOAT:	m_fields.back().value.asFloat = *reinterpret_cast< const float* >( pData ); break;
			case Type::DOUBLE:	m_fields.back().value.asDouble = *reinterpret_cast< const double* >( pData ); break;
			default:
				m_fields.pop_back();
				return false;
		}

		return true;
	}

	bool setSimpleDataAt( int i, const void* pData, int type )
	{
		if ( i >= count() || i < 0 || m_fields[ i ].type != type )
			return false;

		switch ( type )
		{
			case Type::CHAR:	m_fields[ i ].value.asChar = *reinterpret_cast< const char* >( pData ); break;
			case Type::SHORT:	m_fields[ i ].value.asShort = *reinterpret_cast< const short* >( pData ); break;
			case Type::STRING:  
			case Type::WSTRING: 
			case Type::LONG:	m_fields[ i ].value.asLong = *reinterpret_cast< const long* >( pData ); break;
			case Type::LLONG:	m_fields[ i ].value.asLLong = *reinterpret_cast< const long long* >( pData ); break;
			case Type::FLOAT:	m_fields[ i ].value.asFloat = *reinterpret_cast< const float* >( pData ); break;
			case Type::DOUBLE:	m_fields[ i ].value.asDouble = *reinterpret_cast< const double* >( pData ); break;
			default:
				return false;
		}

		return true;
	}

	IArray*	arrayAt( int i )
	{
		if ( i >= count() || i < 0 || !Type::isSimpleArrayType( m_fields[ i ].type ) )
			return NULL;

		return m_fields[ i ].value.asArray;
	}
	
	ITemplate* templateAt( int i )
	{
		if ( i >= count() || i < 0 || m_fields[ i ].type != Type::TEMPLATE )
			return NULL;

		return m_fields[ i ].value.asTemplate;
	}
	
	ITemplateArray*	templateArrayAt( int i )
	{
		if ( i >= count() || i < 0 || m_fields[ i ].type != Type::TEMPLATE_ARRAY )
			return NULL;

		return m_fields[ i ].value.asTemplateArray;
	}
	
	IDict* dictAt( int i )
	{
		if ( i >= count() || i < 0 || m_fields[ i ].type != Type::DICT )
			return NULL;

		return m_fields[ i ].value.asDict;
	}

	long addStringValue( const char* pStrValue )
	{ 
		return m_pParent->addString( pStrValue );
	}

	long addWStringValue( const wchar_t* pWStrValue )
	{ 
		return m_pParent->addWString( pWStrValue );
	}

	IArray* addNewArray( const char* pName, int type )
	{
		if ( indexBy( pName ) != -1 )
			return NULL;

		_add_data( pName, Type::getArrayType( type ) );
		m_fields.back().value.asArray = new FilerArray( m_pParent, type );
		return m_fields.back().value.asArray;
	}

	ITemplate* addNewTemplate( const char* pName, const TemplateInfo* pInfo )
	{
		if ( indexBy( pName ) != -1 )
			return NULL;

		_add_data( pName, Type::TEMPLATE );
		m_fields.back().value.asTemplate = new FilerTemplate( m_pParent, m_pParent->getATI( pInfo->getMetaDataIndex() ) );
		return m_fields.back().value.asTemplate;
	}

	ITemplateArray* addNewTemplateArray( const char* pName, const TemplateInfo* pInfo )
	{
		if ( indexBy( pName ) != -1 )
			return NULL;

		_add_data( pName, Type::TEMPLATE_ARRAY );
		m_fields.back().value.asTemplateArray = new FilerTemplateArray( m_pParent, m_pParent->getATI( pInfo->getMetaDataIndex() ) );
		return m_fields.back().value.asTemplateArray;
	}

	IDict* addNewDict( const char* pName )
	{
		if ( indexBy( pName ) != -1 )
			return NULL;

		_add_data( pName, Type::DICT );
		m_fields.back().value.asDict = new FilerDict( m_pParent );
		return m_fields.back().value.asDict;
	}

	void makeTables( int& newStrIndex, int& newWStrIndex );
	int save( KStream& stream );
	bool load( KStream& stream, MergeTable& mergeTable );

protected:

	void _add_data( const char* pName, int type )
	{
		m_fields.resize( m_fields.size() + 1 );
		m_fields.back().name = m_pParent->addString( pName );
		m_fields.back().type = static_cast< unsigned char >( type );
	}

	Imp*	m_pParent;

	struct Field
	{
		unsigned char	type;
		long			name;
		Var				value;
	};

	std::vector< Field > m_fields;
};


// =================================================================================================


const IArray* FilerTemplate::getArrayAt( int i ) const
{
	assert( i >= 0 && i < count() && Type::isSimpleArrayType( typeAt( i ) ) );
	return m_fields[ i ].asArray;
}

const ITemplate* FilerTemplate::getTemplateAt( int i ) const
{
	assert( i >= 0 && i < count() && typeAt( i ) == Type::TEMPLATE );
	return m_fields[ i ].asTemplate;
}

const ITemplateArray* FilerTemplate::getTemplateArrayAt( int i ) const
{
	assert( i >= 0 && i < count() && typeAt( i ) == Type::TEMPLATE_ARRAY );
	return m_fields[ i ].asTemplateArray;
}

int FilerTemplate::readFixedData( void* pOutBuf, int bufSize ) const
{
	int size = fixedSize();
	if ( size < 0 )
		return -1;
	if ( bufSize > 0 && bufSize < size )
		return 0;

	char* p = reinterpret_cast< char* >( pOutBuf );
	for ( int i = 0, n = count(); i < n; i++ )
	{
		switch ( typeAt( i ) )
		{
			case Type::CHAR:			*(p++) = m_fields[ i ].asChar;	break;
			case Type::SHORT:			*(reinterpret_cast< short*& >( p )++) = m_fields[ i ].asShort; break;
			case Type::STRING:
			case Type::WSTRING:
			case Type::LONG:			*(reinterpret_cast< long*& >( p )++) = m_fields[ i ].asLong; break;
			case Type::LLONG:			*(reinterpret_cast< long long*& >( p )++) = m_fields[ i ].asLLong; break;
			case Type::FLOAT:			*(reinterpret_cast< float*& >( p )++) = m_fields[ i ].asFloat; break;
			case Type::DOUBLE:			*(reinterpret_cast< double*& >( p )++) = m_fields[ i ].asDouble; break;
			
			case Type::CHAR_ARRAY:	
			case Type::SHORT_ARRAY:	 
			case Type::STRING_ARRAY:
			case Type::WSTRING_ARRAY:
			case Type::LONG_ARRAY:	 
			case Type::LLONG_ARRAY:	 
			case Type::FLOAT_ARRAY:	 
			case Type::DOUBLE_ARRAY:	p += m_fields[ i ].asArray->IArray::readFixedData( p ); break;

			case Type::TEMPLATE:		p += m_fields[ i ].asTemplate->readFixedData( p ); break;
			case Type::TEMPLATE_ARRAY:	p += m_fields[ i ].asTemplateArray->ITemplateArray::readFixedData( p ); break;

			default:
			case Type::DICT:
				assert(false);
		}
	}

	return size;
}

const IDict* FilerTemplate::getDictAt( int i ) const
{
	assert( i >= 0 && i < count() && typeAt( i ) == Type::DICT );
	return m_fields[ i ].asDict;
}

IArray* FilerTemplate::arrayAt( int i )
{
	if ( !Type::isSimpleArrayType( typeAt( i ) ) )
		return NULL;

	if ( m_fields[ i ].asArray == NULL )
	{
		if ( fixedCountAt( i ) >= 0 )
			m_fields[ i ].asArray = new FilerFixedArray( m_pParent, Type::getElementType( typeAt( i ) ), fixedCountAt( i ) );
		else
			m_fields[ i ].asArray = new FilerArray( m_pParent, Type::getElementType( typeAt( i ) ) );
	}

	return m_fields[ i ].asArray;
}

ITemplate* FilerTemplate::templateAt( int i )
{
	if ( typeAt( i ) != Type::TEMPLATE )
		return NULL;

	if ( m_fields[ i ].asTemplate == NULL )
		m_fields[ i ].asTemplate = new FilerTemplate( m_pParent, m_pParent->getATI( info()->infoAt( i )->getMetaDataIndex() ) );
	return m_fields[ i ].asTemplate;
}

ITemplateArray* FilerTemplate::templateArrayAt( int i )
{
	if ( typeAt( i ) != Type::TEMPLATE_ARRAY )
		return NULL;

	if ( m_fields[ i ].asTemplateArray == NULL )
	{
		const ATInfo* pFL = m_pParent->getATI( m_pATI->pInfo->infoAt( i )->getMetaDataIndex() );
		if ( fixedCountAt( i ) >= 0 )
			m_fields[ i ].asTemplateArray = new FilerFixedTemplateArray( m_pParent, pFL, fixedCountAt( i ) );
		else
			m_fields[ i ].asTemplateArray = new FilerTemplateArray( m_pParent, pFL );
	}

	return m_fields[ i ].asTemplateArray;
}

IDict* FilerTemplate::dictAt( int i )
{
	if ( typeAt( i ) != Type::DICT )
		return NULL;

	if ( m_fields[ i ].asDict == NULL )
		m_fields[ i ].asDict = new FilerDict( m_pParent );
	return m_fields[ i ].asDict;
}

void FilerTemplate::init( const ATInfo* pFL )
{
	m_pATI = pFL;
	if ( pFL == NULL )
		return;

	// filer 는 내부적으로 field limit 를 무시한다

	m_fields.resize( m_pATI->pInfo->count() );
	memset( &m_fields.front(), 0, m_pATI->pInfo->count() * sizeof( Var ) );

	if ( m_pParent->m_pTemplateInitCallback )
		m_pParent->m_pTemplateInitCallback( m_pATI->pInfo, this );

	for ( int i = 0, n = m_pATI->pInfo->count(); i < n; i++ )
	{
		int type = typeAt( i );
		if ( Type::isSimpleArrayType( type ) ) 
		{
			if ( fixedCountAt( i ) >= 0 )
				m_fields[ i ].asArray = new FilerFixedArray( m_pParent, Type::getElementType( type ), fixedCountAt( i ) );
			else
				m_fields[ i ].asArray = new FilerArray( m_pParent, Type::getElementType( type ) );
		}
		else if ( type == Type::TEMPLATE_ARRAY )
		{
			const ATInfo* pFL = m_pParent->getATI( m_pATI->pInfo->infoAt( i )->getMetaDataIndex() );
			if ( fixedCountAt( i ) >= 0 )
				m_fields[ i ].asTemplateArray = new FilerFixedTemplateArray( m_pParent, pFL, fixedCountAt( i ) );
			else
				m_fields[ i ].asTemplateArray = new FilerTemplateArray( m_pParent, pFL );
		}
		else if ( type == Type::TEMPLATE )
		{
			const ATInfo* pFL = m_pParent->getATI( m_pATI->pInfo->infoAt( i )->getMetaDataIndex() );
			m_fields[ i ].asTemplate = new FilerTemplate( m_pParent, pFL );
		}
		else if ( type == Type::DICT )
			m_fields[ i ].asDict = new FilerDict( m_pParent );
	}
}

void FilerTemplate::close()
{
	for ( int i = 0, n = m_pATI->pInfo->count(); i < n; i++ )
	{
		int type = typeAt( i );
		if ( Type::isSimpleArrayType( type ) )
			delete m_fields[ i ].asArray;
		else if ( type == Type::TEMPLATE )
			delete m_fields[ i ].asTemplate;
		else if ( type == Type::TEMPLATE_ARRAY )
			delete m_fields[ i ].asTemplateArray;
		else if ( type == Type::DICT )
			delete m_fields[ i ].asDict;
	}

	m_fields.clear();
}

// =================================================================================================

// Binary save & load routines

void FilerTemplate::makeTables( int& newStrIndex, int& newWStrIndex )
{
	for ( int i = 0, n = m_pATI->pInfo->count(); i < n; i++ )
	{
		int type = typeAt( i );
		if ( type == Type::TEMPLATE )
		{
			FilerTemplate* pTpl = m_fields[ i ].asTemplate;
			m_pParent->addId( &pTpl->classId() );
			pTpl->makeTables( newStrIndex, newWStrIndex );
		}
		else if ( type == Type::TEMPLATE_ARRAY )
		{
			FilerTemplateArray* pTpls = m_fields[ i ].asTemplateArray;
			m_pParent->addId( &pTpls->info()->id() );
			pTpls->makeTables( newStrIndex, newWStrIndex );
		}
		else if ( Type::isSimpleArrayType( type ) )
		{
			FilerArray* pArray = m_fields[ i ].asArray;
			pArray->makeTables( newStrIndex, newWStrIndex );
		}
		else if ( type == Type::DICT )
		{
			FilerDict* pDict = m_fields[ i ].asDict;
			pDict->makeTables( newStrIndex, newWStrIndex );
		}
		else if ( type == Type::STRING )
		{
			m_fields[ i ].asLong = m_pParent->setStringIndex( m_fields[ i ].asLong, newStrIndex );
		}
		else if ( type == Type::WSTRING )
		{
			m_fields[ i ].asLong = m_pParent->setWStringIndex( m_fields[ i ].asLong, newWStrIndex );
		}
	}
}

void FilerTemplateArray::makeTables( int& newStrIndex, int& newWStrIndex )
{
	for ( int i = 0, n = count(); i < n; i++ )
	{
		FilerTemplate* pTpl = m_fields[ i ];
		pTpl->makeTables( newStrIndex, newWStrIndex );
	}
}

void FilerArray::makeTables( int& newStrIndex, int& newWStrIndex )
{
	if ( type() == Type::STRING )
	{
		long* p = count() ? reinterpret_cast< long* >( &m_buf.front() ) : NULL;
		for ( int i = 0, n = count(); i < n; i++ )
		{
			p[ i ] = m_pParent->setStringIndex( p[ i ], newStrIndex );
		}
	}
	else if ( type() == Type::WSTRING )
	{
		long* p = count() ? reinterpret_cast< long* >( &m_buf.front() ) : NULL;
		for ( int i = 0, n = count(); i < n; i++ )
		{
			p[ i ] = m_pParent->setWStringIndex( p[ i ], newWStrIndex );
		}
	}
}

void FilerDict::makeTables( int& newStrIndex, int& newWStrIndex )
{
	for ( int i = 0, n = count(); i < n; i++ )
	{
		m_fields[ i ].name = m_pParent->setStringIndex( m_fields[ i ].name, newStrIndex );

		int type = typeAt( i );
		if ( type == Type::TEMPLATE )
		{
			FilerTemplate* pTpl = m_fields[ i ].value.asTemplate;
			m_pParent->addId( &pTpl->classId() );
			pTpl->makeTables( newStrIndex, newWStrIndex );
		}
		else if ( type == Type::TEMPLATE_ARRAY )
		{
			FilerTemplateArray* pTpls = m_fields[ i ].value.asTemplateArray;
			m_pParent->addId( &pTpls->info()->id() );
			pTpls->makeTables( newStrIndex, newWStrIndex );
		}
		else if ( Type::isSimpleArrayType( type ) )
		{
			FilerArray* pArray = m_fields[ i ].value.asArray;
			pArray->makeTables( newStrIndex, newWStrIndex );
		}
		else if ( type == Type::DICT )
		{
			FilerDict* pDict = m_fields[ i ].value.asDict;
			pDict->makeTables( newStrIndex, newWStrIndex );
		}
		else if ( type == Type::STRING )
		{
			m_fields[ i ].value.asLong = m_pParent->setStringIndex( m_fields[ i ].value.asLong, newStrIndex );
		}
		else if ( type == Type::WSTRING )
		{
			m_fields[ i ].value.asLong = m_pParent->setWStringIndex( m_fields[ i ].value.asLong, newWStrIndex );
		}
	}
}

// -----------------------------------------------------------------------------------

static inline unsigned int SaveVar( KStream& stream, int type, const Var& value, bool bID = true, bool bInfoUnknown = false )
{
	switch ( type )
	{
		case Type::CHAR:
			return static_cast< unsigned int >( stream.Write( &value.asChar, sizeof(value.asChar) ) );

		case Type::SHORT:
			return static_cast< unsigned int >( stream.Write( &value.asShort, sizeof(value.asShort) ) );

		case Type::STRING:
		case Type::WSTRING:
		case Type::LONG:
			return static_cast< unsigned int >( stream.Write( &value.asLong, sizeof(value.asLong) ) );

		case Type::LLONG:
			return static_cast< unsigned int >( stream.Write( &value.asLLong, sizeof(value.asLLong) ) );

		case Type::FLOAT:
			return static_cast< unsigned int >( stream.Write( &value.asFloat, sizeof(value.asFloat) ) );

		case Type::DOUBLE:
			return static_cast< unsigned int >( stream.Write( &value.asDouble, sizeof(value.asDouble) ) );

		case Type::CHAR_ARRAY:
		case Type::SHORT_ARRAY:
		case Type::LONG_ARRAY:
		case Type::STRING_ARRAY:
		case Type::WSTRING_ARRAY:
		case Type::FLOAT_ARRAY:
		case Type::DOUBLE_ARRAY:
			return value.asArray->save( stream );

		case Type::TEMPLATE:
			return value.asTemplate->save( stream, bID, bInfoUnknown );

		case Type::TEMPLATE_ARRAY:
			return value.asTemplateArray->save( stream );

		default:
		case Type::DICT:
			return value.asDict->save( stream );
	}
}

static inline bool StreamRead( KStream& stream, void* pBuf, unsigned int readSize )
{
	return stream.Read( pBuf, readSize ) == readSize;
}

static inline bool LoadVar( KStream& stream, int type, Var& outValue, MergeTable& mergeTable, bool bID = true, bool bInfoUnknown = false )
{
	bool b;
	switch ( type )
	{
		case Type::CHAR:
			return StreamRead( stream, &outValue.asChar, sizeof(outValue.asChar) );

		case Type::SHORT:
			return StreamRead( stream, &outValue.asShort, sizeof(outValue.asShort) );

		case Type::LONG:
			return StreamRead( stream, &outValue.asLong, sizeof(outValue.asLong) );

		case Type::LLONG:
			return StreamRead( stream, &outValue.asLLong, sizeof(outValue.asLLong) );

		case Type::FLOAT:
			return StreamRead( stream, &outValue.asFloat, sizeof(outValue.asFloat) );

		case Type::DOUBLE:
			return StreamRead( stream, &outValue.asDouble, sizeof(outValue.asDouble) );

		case Type::STRING:
			b = StreamRead( stream, &outValue.asLong, sizeof(outValue.asLong) );
			if ( outValue.asLong > 0 && outValue.asLong <= long(mergeTable.str.size()) )
				outValue.asLong = mergeTable.str[ outValue.asLong-1 ];
			return b;

		case Type::WSTRING:
			b = StreamRead( stream, &outValue.asLong, sizeof(outValue.asLong) );
			if ( outValue.asLong > 0 && outValue.asLong <= long(mergeTable.wstr.size()) )
				outValue.asLong = mergeTable.wstr[ outValue.asLong-1 ];
			return b;

		case Type::CHAR_ARRAY:
		case Type::SHORT_ARRAY:
		case Type::LONG_ARRAY:
		case Type::STRING_ARRAY:
		case Type::WSTRING_ARRAY:
		case Type::FLOAT_ARRAY:
		case Type::DOUBLE_ARRAY:
			return outValue.asArray->load( stream, mergeTable );

		case Type::TEMPLATE:
			return outValue.asTemplate->load( stream, bID, bInfoUnknown, mergeTable );

		case Type::TEMPLATE_ARRAY:
			return outValue.asTemplateArray->load( stream, mergeTable );

		default:
		case Type::DICT:
			return outValue.asDict->load( stream, mergeTable );
	}
}

int FilerTemplate::save( KStream& stream, bool bID, bool bInfoUnknown )
{
	unsigned int pos = static_cast< unsigned int >( stream.Tell() );

	if ( bInfoUnknown || m_pATI->pInfo->fixedSize() < 0 )
		stream.Seek( sizeof(unsigned int), KStream::seekCur );			// 이 템플릿의 길이가 가변일 때만 길이를 기록한다

	unsigned int total = 0;

	if ( bID )
	{
		short nId = static_cast< short >( m_pParent->getIdIndex( &classId() ) );			// 템플릿 배열의 일부일 때는 id 를 각각 기록하지 않는다.
		total += static_cast< unsigned int >( stream.Write( &nId, sizeof(nId) ) );
	}

	for ( int i = 0, n = m_pATI->pInfo->count(); i < n; i++ )		// 세이브할 때는 field limit 를 무시하고 항상 최신 버전으로 저장한다.
		total += SaveVar( stream, typeAt( i ), m_fields[ i ], false, false );

	if ( bInfoUnknown || m_pATI->pInfo->fixedSize() < 0 ) 
	{
		stream.Seek( pos, KStream::seekSet );
		stream.Write( &total, sizeof(total) );
		stream.Seek( total, KStream::seekCur );
		return int( total ) + sizeof( unsigned int );
	}
	else
		return int( total );
}

bool FilerTemplate::load( KStream& stream, bool bID, bool bInfoUnknown, MergeTable& mergeTable )
{
	unsigned int size = 0, posEnd = 0;

	if ( bInfoUnknown || fixedSize() < 0 )
	{
		if ( !StreamRead( stream, &size, sizeof(size) ) )
			return false;

		posEnd = static_cast< unsigned int >( stream.Tell() ) + size;
	}

	if ( bID ) 
	{
		short nId;
		if ( !StreamRead( stream, &nId, sizeof(nId) ) )
			return false;

		if ( nId > 0 && nId <= long(mergeTable.id.size()) )
			nId = mergeTable.id[ nId-1 ];

		const GUID* pId = m_pParent->getId( nId );
		const TemplateInfo* pInfo = m_pParent->m_pMetaData->getTemplateInfo( *pId );

		if ( pInfo == NULL || (m_pATI != NULL && m_pATI->pInfo != pInfo) )
			return false;

		if ( m_pATI == NULL )
			init( m_pParent->getATI( pInfo->getMetaDataIndex() ) );
	}

	for ( int i = 0, n = count(); i < n; i++ )
		if ( !LoadVar( stream, typeAt( i ), m_fields[ i ], mergeTable, false, false ) )
			return false;

	if ( bInfoUnknown || fixedSize() < 0 )
		stream.Seek( posEnd, KStream::seekSet );

	return true;
}

int FilerFixedArray::save( KStream& stream )
{
	m_buf.resize( m_elemSize * m_maxCount, 0 );
	return static_cast< int >( stream.Write( &m_buf.front(), m_maxCount * m_elemSize ) );
}

bool FilerFixedArray::load( KStream& stream, MergeTable& mergeTable )
{
	assert( m_elemType != Type::_NONE );

	bool b = true;

	if ( m_maxCount > 0 )
	{
		m_buf.resize( m_elemSize * m_maxCount, 0 );
		b = StreamRead( stream, &m_buf.front(), m_elemSize * m_maxCount );

		long* p = reinterpret_cast< long* >( &m_buf.front() );

		if ( m_elemType == Type::STRING ) 
		{
			for ( int i = 0; i < m_maxCount; i++ )
				p[i] = mergeTable.str[ p[i]-1 ];
		}
		else if ( m_elemType == Type::WSTRING )
		{
			for ( int i = 0; i < m_maxCount; i++ )
				p[i] = mergeTable.wstr[ p[i]-1 ];
		}
	}

	return b;
}

int FilerFixedTemplateArray::save( KStream& stream )
{
	unsigned int pos = static_cast< unsigned int >( stream.Tell() );

	if ( m_pATI->pInfo->fixedSize() < 0 )
		stream.Seek( sizeof(unsigned int), KStream::seekCur );			// 원소 템플릿의 길이가 가변일 때만 전체 길이를 기록한다

	unsigned int total = 0;
	int i,n;

	for ( i = 0, n = count(); i < n; i++ )
		total += m_fields[ i ]->save( stream, false, false );

	for ( i = static_cast< int >( m_fields.size() ); i < m_maxCount; i++ )
	{
		m_fields.push_back( new FilerTemplate( m_pParent, m_pATI ) );
		total += m_fields.back()->save( stream, false, false );
	}

	if ( m_pATI->pInfo->fixedSize() < 0 )
	{
		stream.Seek( pos, KStream::seekSet );
		stream.Write( &total, sizeof(total) );
		stream.Seek( total, KStream::seekCur );
		return int( total ) + sizeof( unsigned int );
	}
	else
		return int( total );
}

bool FilerFixedTemplateArray::load( KStream& stream, MergeTable& mergeTable )
{
	assert( m_pATI != NULL );

	unsigned int size = 0, posEnd = 0;
	if ( m_pATI->pInfo->fixedSize() < 0 )
	{
		if ( !StreamRead( stream, &size, sizeof(size) ) )
			return false;

		posEnd = static_cast< unsigned int >( stream.Tell() ) + size;
	}

	m_fields.resize( m_maxCount );
	for ( int i = 0; i < m_maxCount; i++ )
	{
		m_fields[ i ] = new FilerTemplate( m_pParent, m_pATI );
		if ( !m_fields[ i ]->load( stream, false, false, mergeTable ) )
			return false;
	}

	if ( m_pATI->pInfo->fixedSize() < 0 )
		stream.Seek( posEnd, KStream::seekSet );
	return true;
}

int FilerTemplateArray::save( KStream& stream )
{
	unsigned int pos = static_cast< unsigned int >( stream.Tell() );
	stream.Seek( sizeof(unsigned int), KStream::seekCur );

	short nId = static_cast< short >( m_pParent->getIdIndex( &info()->id() ) );
	unsigned int total = static_cast< unsigned int >( stream.Write( &nId, sizeof(nId) ) );

	long n = count();
	total += static_cast< unsigned int >( stream.Write( &n, sizeof(n) ) );

	for ( int i = 0, n = count(); i < n; i++ )
		total += m_fields[ i ]->save( stream, false, false );

	stream.Seek( pos, KStream::seekSet );
	stream.Write( &total, sizeof(total) );
	stream.Seek( total, KStream::seekCur );

	return int( total ) + sizeof( unsigned int );
}

bool FilerTemplateArray::load( KStream& stream, MergeTable& mergeTable )
{
	unsigned int size;
	if ( !StreamRead( stream, &size, sizeof(size) ) )
		return false;

	unsigned int posEnd = static_cast< unsigned int >( stream.Tell() ) + size;

	short nId;
	if ( !StreamRead( stream, &nId, sizeof(nId) ) )
		return false;

	if ( nId > 0 && nId <= long(mergeTable.id.size()) )
		nId = mergeTable.id[ nId-1 ];

	const GUID* pId = m_pParent->getId( nId );
	m_pATI = m_pParent->getATI( m_pParent->m_pMetaData->getTemplateInfo( *pId )->getMetaDataIndex() );

	if ( m_pATI == NULL )
		return false;

	long n;
	if ( !StreamRead( stream, &n, sizeof(n) ) || n < 0 )
		return false;

	m_fields.resize( n );
	for ( int i = 0; i < n; i++ )
	{
		m_fields[ i ] = new FilerTemplate( m_pParent, m_pATI );
		if ( !m_fields[ i ]->load( stream, false, false, mergeTable ) )
			return false;
	}

	stream.Seek( posEnd, KStream::seekSet );
	return true;
}

int FilerArray::save( KStream& stream )
{
	unsigned int pos = static_cast< unsigned int >( stream.Tell() );
	stream.Seek( sizeof(unsigned int), KStream::seekCur );

	unsigned int total = static_cast< unsigned int >( stream.Write( &m_elemType, sizeof(m_elemType) ) );

	long n = count();
	if ( n > 0 )
		total += static_cast< unsigned int >( stream.Write( &m_buf.front(), n * m_elemSize ) );

	stream.Seek( pos, KStream::seekSet );
	stream.Write( &total, sizeof(total) );
	stream.Seek( total, KStream::seekCur );

	return int( total ) + sizeof( unsigned int );
}

bool FilerArray::load( KStream& stream, MergeTable& mergeTable )
{
	unsigned int size;
	if ( !StreamRead( stream, &size, sizeof(size) ) )
		return false;

	unsigned int posEnd = static_cast< unsigned int >( stream.Tell() ) + size;

	unsigned char elemType;
	if ( !StreamRead( stream, &elemType, sizeof(elemType) ) )
		return false;

	if ( elemType == Type::_NONE || (m_elemType != Type::_NONE && m_elemType != elemType ) )
		return false;

	if ( m_elemType == Type::_NONE )
		init( elemType );

	long n = (size - sizeof(elemType)) / m_elemSize;
	m_buf.resize( n * m_elemSize );

	if ( n > 0 )
	{
		if ( !StreamRead( stream, &m_buf.front(), static_cast< unsigned int >( m_buf.size() ) ) )
			return false;

		long* p = reinterpret_cast< long* >( &m_buf.front() );

		if ( elemType == Type::STRING ) 
		{
			for ( int i = 0; i < n; i++ )
				p[i] = mergeTable.str[ p[i]-1 ];
		}
		else if ( elemType == Type::WSTRING )
		{
			for ( int i = 0; i < n; i++ )
				p[i] = mergeTable.wstr[ p[i]-1 ];
		}
	}

	stream.Seek( posEnd, KStream::seekSet );
	return true;
}

int FilerDict::save( KStream& stream )
{
	unsigned int pos = static_cast< unsigned int >( stream.Tell() );
	stream.Seek( sizeof(unsigned int), KStream::seekCur );

	long n = count();
	unsigned int total = static_cast< unsigned int >( stream.Write( &n, sizeof(n) ) );

	for ( int i = 0; i < n; i++ )
	{
		Field& field = m_fields[ i ];

		assert( (field.name & 0xff000000) == 0 );
		
		unsigned long type_and_name = (field.type << 24) + (field.name & 0x00ffffff);
		total += static_cast< unsigned int >( stream.Write( &type_and_name, sizeof(type_and_name) ) );
		total += SaveVar( stream, field.type, field.value, true, true );
	}

	stream.Seek( pos, KStream::seekSet );
	stream.Write( &total, sizeof(total) );
	stream.Seek( total, KStream::seekCur );

	return int( total ) + sizeof( unsigned int );
}

bool FilerDict::load( KStream& stream, MergeTable& mergeTable )
{
	bool bUnpackedDict = m_pParent->m_bUnpackedDict;

	unsigned int size;
	if ( !StreamRead( stream, &size, sizeof(size) ) )
		return false;

	unsigned int posEnd = static_cast< unsigned int >( stream.Tell() ) + size;

	long n;
	if ( !StreamRead( stream, &n, sizeof(n) ) || n < 0 )
		return false;

	m_fields.reserve( m_fields.size() + n );

	for ( int i = 0; i < n; i++ )
	{
		Field field;
		if ( bUnpackedDict )
		{
			if ( !StreamRead( stream, &field.name, sizeof(field.name) ) )
				return false;
			if ( !StreamRead( stream, &field.type, sizeof(field.type) ) )
				return false;
		}
		else
		{
			unsigned long type_and_name;
			if ( !StreamRead( stream, &type_and_name, sizeof(type_and_name) ) )
				return false;
			field.name = long( type_and_name & 0x00ffffff );
			field.type = unsigned char( type_and_name >> 24 );
		}

		if ( field.name > 0 )
			field.name = mergeTable.str[ field.name-1 ];

		int ndx = indexBy( m_pParent->getString( field.name ) );
		if ( ndx < 0 )
		{
			// 이름이 NULL 이거나 같은 이름 없음. 뒤에 추가

			if ( Type::isSimpleArrayType( field.type ) )
				field.value.asArray = new FilerArray( m_pParent );
			else if ( field.type == Type::TEMPLATE )
				field.value.asTemplate = new FilerTemplate( m_pParent, NULL );
			else if ( field.type == Type::TEMPLATE_ARRAY )
				field.value.asTemplateArray = new FilerTemplateArray( m_pParent, NULL );
			else if ( field.type == Type::DICT )
				field.value.asDict = new FilerDict( m_pParent );

			if ( !LoadVar( stream, field.type, field.value, mergeTable, true, true ) )
				return false;

			m_fields.push_back( field );
		}
		else
		{
			// 이름이 이미 존재. dict 라면 merge, 그 외는 skip.

			if ( typeAt( ndx ) == Type::DICT && field.type == Type::DICT )
				m_fields[ ndx ].value.asDict->load( stream, mergeTable );
			else if ( Type::isSimpleType( field.type ) )
				stream.Seek( Type::getTypeSize( field.type ), KStream::seekCur );
			else
			{
				long size;
				stream.Read( &size, sizeof(size) );
				stream.Seek( size, KStream::seekCur );
			}
		} 
	}

	stream.Seek( posEnd, KStream::seekSet );
	return true;
}

// =================================================================================================

static char g_filler[8] = {'1','2','3','4','5','6','7','8'};

bool Imp::saveIdTable( KStream& stream )
{
	long size = static_cast< unsigned int >( m_idTable.size() ) * (sizeof(GUID) + sizeof(unsigned short));

	long skip = toAlign4( size );
	size += skip;

	stream.Write( &size, sizeof(size) );

	if ( m_idTable.size() > 0 )
		stream.Write( &m_idTable.front(), m_idTable.size() * sizeof(GUID) );

	for ( int i = 0, n = static_cast< int >( m_idTable.size() ); i < n; i++ )
	{
		const TemplateInfo* pInfo = m_pMetaData->getTemplateInfo( m_idTable[ i ] );
		unsigned short fieldCount = static_cast< unsigned short >( pInfo->count() );
		stream.Write( &fieldCount, sizeof(fieldCount) );
	}

	if ( skip > 0 )
		stream.Write( g_filler, skip );

	return true;
}

bool Imp::loadIdTable( KStream& stream, std::vector< unsigned short >& idMergeTable )
{
	long size;
	StreamRead( stream, &size, sizeof(size) );

	unsigned int posEnd = static_cast< unsigned int >( stream.Tell() ) + size;

	size /= sizeof(GUID) + sizeof(unsigned short);
	idMergeTable.reserve( size );

	int i;
	for ( i = 0; i < size; i++ )
	{
		GUID id;
		StreamRead( stream, &id, sizeof(id) );

		idMergeTable.push_back( static_cast< unsigned short >( addId( &id ) ) );
	}

	for ( i = 0; i < size; i++ )
	{
		unsigned short fieldCount;
		StreamRead( stream, &fieldCount, sizeof(fieldCount) );

		const GUID* pId = getId( idMergeTable[ i ] );
		const TemplateInfo* pInfo = m_pMetaData->getTemplateInfo( *pId );
		ATInfo& fl = m_fieldLimits[ pInfo->getMetaDataIndex() ];
		
		assert( fl.pInfo == pInfo );
		
		fl.fieldLimit = fieldCount;
		fl.fixedSize = pInfo->calcFixedSize( fieldCount );
	}

	stream.Seek( posEnd, KStream::seekSet );
	return true;
}

template< typename T > struct FnNoIndex {
	bool operator() ( const T& a ) {
		return a.index == -1;
	}
};

template< typename T > struct FnLessIndex {
	bool operator() ( const T& a, const T& b ) {
		return a.index < b.index;
	}
};

template< typename T > struct FnResetIndex {
	void operator() ( T& a ) {
		a.index = -1;
	}
};

void Imp::applyStringIndex()
{
	m_stringTable.erase( std::remove_if( m_stringTable.begin(), m_stringTable.end(), FnNoIndex< StrRec >() ), m_stringTable.end() );
	std::sort( m_stringTable.begin(), m_stringTable.end(), FnLessIndex< StrRec >() );
	std::for_each( m_stringTable.begin(), m_stringTable.end(), FnResetIndex< StrRec >() );

	m_wstringTable.erase( std::remove_if( m_wstringTable.begin(), m_wstringTable.end(), FnNoIndex< WStrRec >() ), m_wstringTable.end() );
	std::sort( m_wstringTable.begin(), m_wstringTable.end(), FnLessIndex< WStrRec >() );
	std::for_each( m_wstringTable.begin(), m_wstringTable.end(), FnResetIndex< WStrRec >() );
}

bool Imp::saveStringTable( KStream& stream )
{
	unsigned int pos = static_cast< unsigned int >( stream.Tell() );
	stream.Seek( sizeof(unsigned int), KStream::seekCur );

	int i;
	long n;
	unsigned int total = 0;

	// write string table

	n = int( m_stringTable.size() );
	total += static_cast< unsigned int >( stream.Write( &n, sizeof(n) ) );

	for ( i = 0; i < n; i++ )
		total += static_cast< unsigned int >( stream.Write( m_stringTable[ i ].str.c_str(), m_stringTable[ i ].str.size()+1 ) );

	// write wstring table

	n = int( m_wstringTable.size() );
	total += static_cast< unsigned int >( stream.Write( &n, sizeof(n) ) );

	for ( i = 0; i < n; i++ )
		total += static_cast< unsigned int >( stream.Write( m_wstringTable[ i ].str.c_str(), (m_wstringTable[ i ].str.size()+1) * sizeof(wchar_t) ) );

	// write block size

	long skip = toAlign4( total );
	total += skip;

	if ( skip > 0 )
		stream.Write( g_filler, skip );

	stream.Seek( pos, KStream::seekSet );
	stream.Write( &total, sizeof(total) );
	stream.Seek( total, KStream::seekCur );

	return true;
}

template< typename CHAR > 
inline static int strnlen_( const CHAR* pStr, int limit )
{
	if ( pStr == NULL )
		return 0;

	for ( int i = 0; i < limit; i++, pStr++ )
	{
		if ( *pStr == 0 )
			return i;
	}
	return limit;
}

bool Imp::loadStringTable( KStream& stream, std::vector< long >& strMergeTable, std::vector< long >& wstrMergeTable )
{
	unsigned int size;
	if ( !StreamRead( stream, &size, sizeof(size) ) )
		return false;

	unsigned int posEnd = static_cast< unsigned int >( stream.Tell() ) + size;

	// read all block to memory

	std::vector< char > buf( size );
	if ( !StreamRead( stream, &buf.front(), size ) )
		return false;

	int i;
	long n;
	char* p = &buf.front();

	// decode string table

	n = *reinterpret_cast< long* >( p );
	p += sizeof( n );
	size -= sizeof( n );

	if ( n < 0 )
		return false;

	strMergeTable.reserve( n );
	for ( i = 0; i < n; i++ )
	{
		int len = strnlen_( p, size );
		strMergeTable.push_back( addString( p ) );
		
		p += len+1;
		size -= len+1;
	}

	// decode wstring table

	n = *reinterpret_cast< long* >( p );
	p += sizeof( n );
	size -= sizeof( n );

	if ( n < 0 )
		return false;

	wstrMergeTable.reserve( n );
	for ( i = 0; i < n; i++ )
	{
		int len = strnlen_( reinterpret_cast< wchar_t* >( p ), size / sizeof(wchar_t) );
		wstrMergeTable.push_back( addWString( reinterpret_cast< wchar_t* >( p ) ) );

		p += (len+1) * sizeof(wchar_t);
		size -= (len+1) * sizeof(wchar_t);
	}

	stream.Seek( posEnd, KStream::seekSet );
	return size < 4;
}

// =================================================================================================

Filer::Filer( MetaData* pMetaData )
{
	m_pImp = new Imp;
	m_pImp->m_pMetaData = pMetaData;
	m_pImp->m_pRoot = NULL;
	m_pImp->m_bUnpackedDict = false;
	m_pImp->m_pTemplateInitCallback = NULL;

	if ( m_pImp->m_pMetaData )
	{
		m_pImp->m_pMetaData->addRef();
		m_pImp->initATIs();
	}
}

Filer::~Filer()
{
	clear();

	if ( m_pImp->m_pMetaData )
		m_pImp->m_pMetaData->subRef();

	delete m_pImp;
}

void Filer::clear()
{
	delete m_pImp->m_pRoot;
	m_pImp->m_pRoot = NULL;
}

void Filer::setTemplateInitCallback( TInitTemplateFn fn )
{
	m_pImp->m_pTemplateInitCallback = fn;
}

const static FileHeader g_fileHeader = 
{
	{ 'B','T','R','F' },
	sizeof( HeaderInfo )
};

const static HeaderInfo g_hdr =
{
	1,
	1,
};

bool Filer::save( KStream& stream )
{
	// header

	stream.Write( &g_fileHeader, sizeof(g_fileHeader) );
	stream.Write( &g_hdr, sizeof(g_hdr) );

	// id (GUID) & string table

	root();
	m_pImp->m_idTable.clear();

	int newStrIndex = 1, newWStrIndex = 1;
	m_pImp->m_pRoot->makeTables( newStrIndex, newWStrIndex );
	m_pImp->applyStringIndex();

	if ( !m_pImp->saveIdTable( stream ) )
		return false;

	if ( !m_pImp->saveStringTable( stream ) )
		return false;

	// data

	if ( !m_pImp->m_pRoot->save( stream ) )
		return false;

	return true;
}

bool Filer::load( KStream& stream )
{
	// header
	{
		FileHeader header;
		if ( !StreamRead( stream, &header, sizeof(header) ) || strncmp( header.idstr, g_fileHeader.idstr, 4 ) || header.headerInfoSize < 0 )
			return false;

		HeaderInfo headerInfo;
		if ( header.headerInfoSize >= sizeof(headerInfo) )
		{
			if ( !StreamRead( stream, &headerInfo, sizeof(headerInfo) ) )
				return false;
		}
		else
		{
			if ( !StreamRead( stream, &headerInfo, header.headerInfoSize ) )
				return false;
			stream.Seek( sizeof(headerInfo) - header.headerInfoSize, KStream::seekCur );
		}

		m_pImp->m_bUnpackedDict = headerInfo.majorVersion * 256 + headerInfo.minorVersion == 256 + 0;		// ver 1.1 이상은 packed dict format
	}

	MergeTable mergeTable;

	// id (GUID) & string table

	if ( !m_pImp->loadIdTable( stream, mergeTable.id ) )
		return false;

	if ( !m_pImp->loadStringTable( stream, mergeTable.str, mergeTable.wstr ) )
		return false;

	// data

	root();

	if ( !m_pImp->m_pRoot->load( stream, mergeTable ) )
		return false;

	m_pImp->initATIs();

	return true;
}

MetaData* Filer::metaData()
{
	return m_pImp->m_pMetaData;
}

IDict* Filer::root()
{
	if ( m_pImp->m_pRoot == NULL )
		m_pImp->m_pRoot = new FilerDict( m_pImp );

	return m_pImp->m_pRoot;
}

} // namespace trf
