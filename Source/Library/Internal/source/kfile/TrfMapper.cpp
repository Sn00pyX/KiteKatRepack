#include "../../include/kfile/TrfMapper.h"


namespace trf {

// =================================================================================================================

void MTemplate::init( Mapper* pParent, const TemplateInfo* pInfo, const char* pBuf, bool bID )
{
	m_pParent = pParent;
	m_pInfo = pInfo;
	m_pATI = pInfo ? m_pParent->getATI( m_pInfo->id() ) : NULL;

	if ( m_pATI == NULL || fixedSize() < 0 )
	{
		unsigned int size = *reinterpret_cast< const unsigned int* >( pBuf );
		pBuf += sizeof(size);
		if ( size < 0 || pBuf + size > m_pParent->m_pBufEnd )
			throw Exception( 1 );
	}

	if ( bID ) 
	{
		short nId = *reinterpret_cast< const short* >( pBuf );
		pBuf += sizeof(nId);

		m_pATI = m_pParent->getATI( nId );
		m_pInfo = m_pATI->pInfo;

		if ( m_pInfo == NULL || (pInfo != NULL && m_pInfo != pInfo) )
			throw Exception( 2 );
	}

	// mapper 는 filer 와 달리 내부적으로도 field limit 를 존중한다

	m_fields.resize( count() );
	memset( &m_fields.front(), 0, count() * sizeof(Field) );

	for ( int i = 0, n = count(); i < n; i++ )
	{
		m_fields[ i ].ptr = pBuf;
		int size = fixedSizeAt( i );
		if ( size > 0 )
			pBuf += size;
		else
			pBuf += *reinterpret_cast< const unsigned int* >( pBuf ) + sizeof(unsigned int);
	}
}

void MTemplate::close()
{
	for ( int i = 0, n = count(); i < n; i++ )
	{
		int type = typeAt( i );
		if ( Type::isSimpleArrayType( type ) )
			m_pParent->deleteArray( m_fields[ i ].asArray );
		else if ( type == Type::TEMPLATE )
			m_pParent->deleteTemplate( m_fields[ i ].asTemplate );
		else if ( type == Type::TEMPLATE_ARRAY )
			m_pParent->deleteTemplateArray( m_fields[ i ].asTemplateArray );
		else if ( type == Type::DICT )
			m_pParent->deleteDict( m_fields[ i ].asDict );
	}
}

void MTemplateArray::init( Mapper* pParent, const TemplateInfo* pInfo, int fixedCount, const char* pBuf )
{
	// fixed array

	m_pInfo = pInfo;
	m_pParent = pParent;
	m_pATI = pParent->getATI( pInfo->id() );

	m_fields.resize( fixedCount );
	memset( &m_fields.front(), 0, fixedCount * sizeof(Field) );

	if ( m_pInfo->fixedSize() < 0 )
	{
		unsigned int size = *reinterpret_cast< const unsigned int* >( pBuf );
		pBuf += sizeof(size);
		if ( size < 0 || pBuf + size > m_pParent->m_pBufEnd )
			throw Exception( 3 );
	}

	int fixSize = fixedSize();
	if ( fixSize > 0 )
		for ( int i = 0; i < fixedCount; i++ )
		{
			m_fields[ i ].ptr = pBuf;
			pBuf += fixSize;
		}
	else
		for ( int i = 0; i < fixedCount; i++ )
		{
			m_fields[ i ].ptr = pBuf;
			pBuf += *reinterpret_cast< const unsigned int* >( pBuf ) + sizeof(unsigned int);
		}
}

void MTemplateArray::init( Mapper* pParent, const TemplateInfo* pInfo, const char* pBuf )
{
	// variable array

	m_pInfo = pInfo;
	m_pParent = pParent;

	unsigned int size = *reinterpret_cast< const unsigned int* >( pBuf );
	pBuf += sizeof(size);

	if ( size < 0 || pBuf + size > m_pParent->m_pBufEnd )
		throw Exception( 4 );

	short nId = *reinterpret_cast< const short* >( pBuf );
	pBuf += sizeof(nId);

	m_pATI = pParent->getATI( nId );
	m_pInfo = m_pATI->pInfo;

	if ( m_pInfo == NULL || (pInfo != NULL && m_pInfo != pInfo) )
		throw Exception( 5 );

	int n = *reinterpret_cast< const int* >( pBuf );
	pBuf += sizeof(n);

	if ( n < 0 )
		throw Exception( 6 );

	m_fields.resize( n );
	if ( n > 0 )
		memset( &m_fields.front(), 0, n * sizeof(Field) );

	int fixSize = fixedSize();
	if ( fixSize > 0 )
		for ( int i = 0, n = count(); i < n; i++ )
		{
			m_fields[ i ].ptr = pBuf;
			pBuf += fixSize;
		}
	else
		for ( int i = 0, n = count(); i < n; i++ )
		{
			m_fields[ i ].ptr = pBuf;
			pBuf += *reinterpret_cast< const unsigned int* >( pBuf ) + sizeof(unsigned int);
		}
}

void MTemplateArray::close()
{
	for ( int i = 0, n = m_fields.size(); i < n; i++ )
		m_pParent->deleteTemplate( m_fields[ i ].asTemplate );
}

void MArray::init( Mapper* pParent, int type, int fixedCount, const char* pBuf ) 
{
	// fixed array

	m_pParent = pParent;
	m_type = type;

	m_pBuf = pBuf;
	m_count = fixedCount;
}

void MArray::init( Mapper* pParent, int type, const char* pBuf )
{
	// variable array

	m_pParent = pParent;
	m_type = type;

	unsigned int size = *reinterpret_cast< const unsigned int* >( pBuf );
	pBuf += sizeof( size );

	if ( size < 0 || pBuf + size > m_pParent->m_pBufEnd )
		throw Exception( 7 );

	m_type = *(pBuf++);
	int elemSize = Type::getTypeSize( m_type );
	
	m_pBuf = pBuf;
	m_count = (size - 1) / elemSize;
}

void MDict::init( Mapper* pParent, const char* pBuf )
{
	m_pParent = pParent;

	unsigned int size = *reinterpret_cast< const unsigned int* >( pBuf );
	pBuf += sizeof(size);

	if ( size < 0 || pBuf + size > m_pParent->m_pBufEnd )
		throw Exception( 8 );

	int n = *reinterpret_cast< const int* >( pBuf );
	pBuf += sizeof( n );

	if ( n < 0 )
		throw Exception( 9 );

	m_fields.resize( n );
	if ( n > 0 )
		memset( &m_fields.front(), 0, n * sizeof(Field) );

	unsigned int fieldHdrSize = m_pParent->m_bUnpackedDict ? sizeof(UnpackedFieldHdr) : sizeof(FieldHdr);

	for ( int i = 0; i < n; i++ )
	{
		Field& field = m_fields[ i ];
		field.ptr = reinterpret_cast< const FieldHdr* >( pBuf );
		pBuf += fieldHdrSize;

		int type = _getFieldType( field.ptr );

		if ( Type::isSimpleType( type ) )
		{
			pBuf += Type::getTypeSize( type );
		}
		else
		{
			unsigned int blockLen = *reinterpret_cast< const unsigned int* >( pBuf );
			if ( blockLen < 0 )
				throw Exception( 10 );
			pBuf += blockLen + sizeof(blockLen);
		}
	}
}

void MDict::close()
{
	for ( int i = 0, n = m_fields.size(); i < n; i++ )
	{
		int type = typeAt( i );
		if ( Type::isSimpleArrayType( type ) )
			m_pParent->deleteArray( m_fields[ i ].asArray );
		else if ( type == Type::TEMPLATE )
			m_pParent->deleteTemplate( m_fields[ i ].asTemplate );
		else if ( type == Type::TEMPLATE_ARRAY )
			m_pParent->deleteTemplateArray( m_fields[ i ].asTemplateArray );
		else if ( type == Type::DICT )
			m_pParent->deleteDict( m_fields[ i ].asDict );
	}
}


// =================================================================================================================

static const char g_idStr[4] = { 'B','T','R','F' };

Mapper::Mapper( const MetaData* pMetaData, const void* pBuf, int bufSize )
{
	m_pMetaData = pMetaData;
	m_pBuf = reinterpret_cast< const char* >( pBuf );
	m_pBufEnd = m_pBuf + bufSize;
	m_pRoot = NULL;

	if ( m_pMetaData )
		m_pMetaData->addRef();

	try
	{
		const char* p = m_pBuf;

		const FileHeader* pFileHeader = reinterpret_cast< const FileHeader* >( p );
		if ( memcmp( pFileHeader->idstr, g_idStr, 4 ) )
			throw Exception( 11 );

		p += sizeof( FileHeader );
		bufSize -= sizeof( FileHeader );

		const HeaderInfo* pHeaderInfo = reinterpret_cast< const HeaderInfo* >( p );

		m_bUnpackedDict = pHeaderInfo->majorVersion == 1 && pHeaderInfo->minorVersion == 0;

		p += pFileHeader->headerInfoSize;
		bufSize -= pFileHeader->headerInfoSize;

		p = initIdTable( p );
		p = initStringTable( p );

		m_pRoot = createDict( p );
	} 
	catch ( Exception& )
	{
		if ( m_pMetaData )
			m_pMetaData->subRef();
		m_pMetaData = NULL;
		throw;
	}
}

Mapper::~Mapper()
{
	if ( m_pRoot )
		m_pRoot->close();
	delete m_pRoot;

	if ( m_pMetaData ) {
		m_pMetaData->subRef();
	}
}

const char* Mapper::initIdTable( const char* pBuf )
{
	int size = *reinterpret_cast< const int* >( pBuf );
	pBuf += sizeof(size);
	const char* pBufEnd = pBuf + size;

	if ( size < 0 )
		throw Exception( size, "idtable size error!" );

	// map GUID pointer

	m_idCount = size / (sizeof(GUID) + sizeof(unsigned short));
	m_pId = reinterpret_cast< const GUID* >( pBuf );

	// set template field limits

	pBuf += m_idCount * sizeof(GUID);
	const unsigned short* pATInfos = reinterpret_cast< const unsigned short* >( pBuf );

	m_TInfoIndexTable.resize( m_pMetaData->getTemplateInfoCount() );
	memset( m_TInfoIndexTable, 0xff, m_pMetaData->getTemplateInfoCount() * sizeof(short) );

	m_ATIs.resize( m_idCount );
	for ( int i = 0; i < m_idCount; i++ )
	{
		const TemplateInfo* pInfo = m_pMetaData->getTemplateInfo( m_pId[ i ] );
		if ( pInfo == NULL )
			throw Exception( 0, "unknown template GUID" );

		ATInfo& fl = m_ATIs[ i ];
		fl.fieldLimit = pATInfos[ i ];
		fl.fixedSize = pInfo->calcFixedSize( fl.fieldLimit );
		fl.pInfo = pInfo;
		m_TInfoIndexTable[ pInfo->getMetaDataIndex() ] = static_cast< short >( i );
	}

	if ( pBuf + m_idCount * sizeof(unsigned short) > pBufEnd )
		throw Exception( size, "idtable error!" );

	return pBufEnd;
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

const char* Mapper::initStringTable( const char* pBuf )
{
	int size = *reinterpret_cast< const int* >( pBuf );
	pBuf += sizeof(size);
	const char* pBufEnd = pBuf + size;

	if ( size < 0 )
		throw Exception( size, "stringtable size error!" );

	int i;
	int n;

	// map string table

	n = *reinterpret_cast< const int* >( pBuf );
	pBuf += sizeof( n );
	size -= sizeof( n );

	if ( n < 0 )
		throw Exception( n, "str table count error" );

	m_stringTable.resize( n );
	for ( i = 0; i < n; i++ )
	{
		int len = strnlen_( pBuf, size );
		m_stringTable[ i ] = pBuf;

		pBuf += len+1;
		size -= len+1;
	}

	// map wstring table

	n = *reinterpret_cast< const int* >( pBuf );
	pBuf += sizeof( n );
	size -= sizeof( n );

	if ( n < 0 )
		throw Exception( n, "wstr table count error" );

	m_wstringTable.resize( n );
	for ( i = 0; i < n; i++ )
	{
		int len = strnlen_( reinterpret_cast< const wchar_t* >( pBuf ), size / sizeof(wchar_t) );
		m_wstringTable[ i ] = reinterpret_cast< const wchar_t* >( pBuf );

		pBuf += (len+1) * sizeof(wchar_t);
		size -= (len+1) * sizeof(wchar_t);
	}

	if ( pBuf > pBufEnd )
		throw Exception( size, "stringtable error!" );

	return pBufEnd;
}

MTemplate* Mapper::createTemplate( const TemplateInfo* pInfo, const char* pBuf, bool bID )
{
	MTemplate* p = new MTemplate;
	p->init( this, pInfo, pBuf, bID );
	return p;
}

MTemplateArray* Mapper::createTemplateArray( const TemplateInfo* pInfo, int fixedCount, const char* pBuf )
{
	MTemplateArray* p = new MTemplateArray;
	p->init( this, pInfo, fixedCount, pBuf );
	return p;
}

MTemplateArray* Mapper::createTemplateArray( const TemplateInfo* pInfo, const char* pBuf )
{
	MTemplateArray* p = new MTemplateArray;
	p->init( this, pInfo, pBuf );
	return p;
}

MArray* Mapper::createArray( int type, int fixedCount, const char* pBuf )
{
	MArray* p = new MArray;
	p->init( this, type, fixedCount, pBuf );
	return p;
}

MArray* Mapper::createArray( int type, const char* pBuf )
{
	MArray* p = new MArray;
	p->init( this, type, pBuf );
	return p;
}

MDict* Mapper::createDict( const char* pBuf )
{
	MDict* p = new MDict;
	p->init( this, pBuf );
	return p;
}

void Mapper::deleteTemplate( MTemplate* pTemplate )
{
	if ( pTemplate )
		pTemplate->close();
	delete pTemplate;
}

void Mapper::deleteTemplateArray( MTemplateArray* pTemplateArray )
{
	if ( pTemplateArray )
		pTemplateArray->close();
	delete pTemplateArray;
}

void Mapper::deleteArray( MArray* pArray )
{
	delete pArray;
}

void Mapper::deleteDict( MDict* pDict )
{
	if ( pDict )
		pDict->close();
	delete pDict;
}

} // namespace trf