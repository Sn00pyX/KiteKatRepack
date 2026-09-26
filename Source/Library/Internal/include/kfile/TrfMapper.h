#pragma once

// -
// -  TRF : Template-based Recursive Format
// -
// -  trfMapper.h
// -
// -    Lazy TRF stream mapper for low-overhead loading  
// -
// -    2006.2.14 by Young-Hyun Joo, nFlavor
// -

#include "TrfMetaData.h"
#include "../toolkit/c_array.h"
#include <guiddef.h>
#include <assert.h>

namespace trf {


struct MapperObjects
{
	typedef const class MTemplate*      TemplatePtr;
	typedef const class MTemplateArray* TemplateArrayPtr;
	typedef const class MArray*         ArrayPtr;
	typedef const class MDict*          DictPtr;
};

typedef const class MTemplate*		MTemplatePtr;
typedef const class MTemplateArray*	MTemplateArrayPtr;
typedef const class MArray*			MArrayPtr;
typedef const class MDict*			MDictPtr;

class Mapper
{
public:

	Mapper( const MetaData* pMetaData, const void* pBuf, int bufSize );
	~Mapper();

	const MetaData* metaData()	{ return m_pMetaData; }
	MDictPtr root()				{ return m_pRoot; }

private:

	friend class MArray;
	friend class MTemplate;
	friend class MTemplateArray;
	friend class MDict;

	const char*		initIdTable( const char* pBuf );
	const char*		initStringTable( const char* pBuf );
	const char*		getString( int i );
	const wchar_t*	getWString( int i );
	const GUID&		getId( int i );

	const MetaData*	m_pMetaData;
	MDict*			m_pRoot;

	const char* m_pBuf;
	const char* m_pBufEnd;

	int			m_idCount;
	const GUID* m_pId;

	c_array< const char* > m_stringTable;
	c_array< const wchar_t* > m_wstringTable;

	c_array< ATInfo > m_ATIs;
	c_array< short > m_TInfoIndexTable;

	bool m_bUnpackedDict;

	const ATInfo* getATI( const GUID& id );
	const ATInfo* getATI( int n ) { return &m_ATIs[ n-1 ]; }

	MTemplate* createTemplate( const TemplateInfo* pInfo, const char* pBuf, bool bID );
	MTemplateArray* createTemplateArray( const TemplateInfo* pInfo, int fixedCount, const char* pBuf );
	MTemplateArray* createTemplateArray( const TemplateInfo* pInfo, const char* pBuf );
	MArray* createArray( int type, int fixedCount, const char* pBuf );
	MArray* createArray( int type, const char* pBuf );
	MDict* createDict( const char* pBuf );

	void deleteTemplate( MTemplate* pTemplate );
	void deleteTemplateArray( MTemplateArray* pTemplateArray );
	void deleteArray( MArray* pArray );
	void deleteDict( MDict* pDict );
};


class MTemplate
{
public:

	// class info 

	const TemplateInfo*			info() const 									{ return m_pInfo; }
	const GUID&					classId() const 								{ return m_pInfo->id(); }
	const char*					className() const 								{ return m_pInfo->name(); }
	int							fixedSize() const								{ return m_pATI->fixedSize; }
	bool						isFixed() const									{ return fixedSize() >= 0; }

	// field info

	int							count() const 									{ return m_pATI->fieldLimit; }

	int							indexBy( const char* pName ) const 				{ return m_pInfo->indexBy( pName ); }
	const char*					nameAt( int i ) const 							{ return m_pInfo->nameAt( i ); }

	int							typeAt( int i ) const 							{ return m_pInfo->typeAt( i ); }
	int							fixedCountAt( int i ) const						{ return m_pInfo->fixedCountAt( i ); }
	int							fixedSizeAt( int i ) const						{ return m_pInfo->fixedSizeAt( i ); }

	// data read 

	int							readFixedData( void* pOutBuf, int bufSize ) const;

	const void*					getSimpleDataAt( int i, int type ) const;

	char						getCharAt( int i ) const						{ return *reinterpret_cast< const char* >( getSimpleDataAt( i, Type::CHAR ) ); }
	short						getShortAt( int i ) const						{ return *reinterpret_cast< const short* >( getSimpleDataAt( i, Type::SHORT ) ); }
	int							getLongAt( int i ) const						{ return *reinterpret_cast< const int* >( getSimpleDataAt( i, Type::LONG ) ); }
	long long					getLLongAt( int i ) const						{ return *reinterpret_cast< const long long* >( getSimpleDataAt( i, Type::LLONG ) ); }
	unsigned char				getByteAt( int i ) const						{ return *reinterpret_cast< const unsigned char* >( getSimpleDataAt( i, Type::CHAR ) ); }
	unsigned short				getWordAt( int i ) const						{ return *reinterpret_cast< const unsigned short* >( getSimpleDataAt( i, Type::SHORT ) ); }
	unsigned int				getDWordAt( int i ) const						{ return *reinterpret_cast< const unsigned int* >( getSimpleDataAt( i, Type::LONG ) ); }
	unsigned long long			getQWordAt( int i ) const						{ return *reinterpret_cast< const unsigned long long* >( getSimpleDataAt( i, Type::LLONG ) ); }
	float						getFloatAt( int i ) const						{ return *reinterpret_cast< const float* >( getSimpleDataAt( i, Type::FLOAT ) ); }
	double						getDoubleAt( int i ) const						{ return *reinterpret_cast< const double* >( getSimpleDataAt( i, Type::DOUBLE ) ); }
	const char*					getStringAt( int i ) const						{ return getStringValue( *reinterpret_cast< const int* >( getSimpleDataAt( i, Type::STRING ) ) ); }
	const wchar_t*				getWStringAt( int i ) const						{ return getWStringValue( *reinterpret_cast< const int* >( getSimpleDataAt( i, Type::WSTRING ) ) ); }
	MArrayPtr					getArrayAt( int i ) const;
	MTemplatePtr				getTemplateAt( int i ) const;
	MTemplateArrayPtr			getTemplateArrayAt( int i ) const;
	MDictPtr					getDictAt( int i ) const;

	char						getCharBy( const char* pName ) const			{ return getCharAt( indexBy( pName ) ); }
	short						getShortBy( const char* pName ) const			{ return getShortAt( indexBy( pName ) ); }
	int							getLongBy( const char* pName ) const			{ return getLongAt( indexBy( pName ) ); }
	long long					getLLongBy( const char* pName ) const			{ return getLLongAt( indexBy( pName ) ); }
	unsigned char				getByteBy( const char* pName ) const			{ return getByteAt( indexBy( pName ) ); }
	unsigned short				getWordBy( const char* pName ) const			{ return getWordAt( indexBy( pName ) ); }
	unsigned int				getDWordBy( const char* pName ) const			{ return getDWordAt( indexBy( pName ) ); }
	unsigned long long			getQWordBy( const char* pName ) const			{ return getQWordAt( indexBy( pName ) ); }
	float						getFloatBy( const char* pName ) const			{ return getFloatAt( indexBy( pName ) ); }
	double						getDoubleBy( const char* pName ) const			{ return getDoubleAt( indexBy( pName ) ); }
	const char*					getStringBy( const char* pName ) const			{ return getStringAt( indexBy( pName ) ); }
	const wchar_t*				getWStringBy( const char* pName ) const			{ return getWStringAt( indexBy( pName ) ); }
	MArrayPtr					getArrayBy( const char* pName ) const			{ return getArrayAt( indexBy( pName ) ); }
	MTemplatePtr				getTemplateBy( const char* pName ) const		{ return getTemplateAt( indexBy( pName ) ); }
	MTemplateArrayPtr			getTemplateArrayBy( const char* pName ) const	{ return getTemplateArrayAt( indexBy( pName ) ); }
	MDictPtr					getDictBy( const char* pName ) const			{ return getDictAt( indexBy( pName ) ); }

	// string data & index conversion

	const char*					getStringValue( int strIndex ) const			{ return m_pParent->getString( strIndex ); }
	const wchar_t*				getWStringValue( int wstrIndex ) const			{ return m_pParent->getWString( wstrIndex ); }

private:

	friend class Mapper;

	MTemplate()  {}
	~MTemplate() {}

	void init( Mapper* pParent, const TemplateInfo* pInfo, const char* pBuf, bool bID );
	void close();

	Mapper* m_pParent;
	const TemplateInfo* m_pInfo;
	const ATInfo* m_pATI;

	struct Field {
		const char* ptr;
		union {
			MTemplate*		asTemplate;
			MTemplateArray*	asTemplateArray;
			MArray*			asArray;
			MDict*			asDict;
		};
	};

	c_array< Field > m_fields;
};


class MTemplateArray
{
public:

	// element class info & count

	int							count() const									{ return int( m_fields.size() ); }

	const TemplateInfo*			info() const									{ return m_pInfo; }
	const GUID&					classId() const 								{ return m_pInfo->id(); }
	const char*					className() const 								{ return m_pInfo->name(); }
	int							fieldCount() const								{ return m_pATI->fieldLimit; }
	int							fixedSize() const								{ return m_pATI->fixedSize; }
	bool						isFixed() const									{ return fixedSize() >= 0; }

	// data read interface

	int							readFixedData( int i, int n, void* pOutBuf, int bufSize ) const;
	int							readFixedData( void* pOutBuf, int bufSize )	const;

	MTemplatePtr				getTemplateAt( int i ) const;
	const void*					getFixedData( int i ) const;

protected:

	friend class Mapper;

	MTemplateArray()
		: m_pParent( NULL )
		, m_pInfo( NULL )
		, m_pATI( NULL )
	{}
	~MTemplateArray() {}

	void init( Mapper* pParent, const TemplateInfo* pInfo, int fixedCount, const char* pBuf );
	void init( Mapper* pParent, const TemplateInfo* pInfo, const char* pBuf );
	void close();

	Mapper* m_pParent;
	const TemplateInfo* m_pInfo;
	const ATInfo* m_pATI;

	struct Field
	{														
		const char* ptr;
		MTemplate*  asTemplate;
	};

	c_varray< Field > m_fields;
};


class MArray
{
public:
	
	// element info & count

	int							count() const									{ return m_count; }

	int							type() const									{ return m_type; }
	int							fixedSize() const								{ return Type::getTypeSize( m_type ); }
	bool						isFixed() const									{ return true; }

	// data read interface

	int							readFixedData( int i, int n, void* pOutBuf, int bufSize ) const;
	int							readFixedData( void* pOutBuf, int bufSize ) const;

	const void*					getSimpleDataArray( int start, int type ) const;
	
	char						getCharAt( int i ) const						{ return *reinterpret_cast< const char* >( getSimpleDataArray( i, Type::CHAR ) ); }
	short						getShortAt( int i ) const						{ return *reinterpret_cast< const short* >( getSimpleDataArray( i, Type::SHORT ) ); }
	int							getLongAt( int i ) const						{ return *reinterpret_cast< const int* >( getSimpleDataArray( i, Type::LONG ) ); }
	long long					getLLongAt( int i ) const						{ return *reinterpret_cast< const long long* >( getSimpleDataArray( i, Type::LLONG ) ); }
	unsigned char				getByteAt( int i ) const						{ return *reinterpret_cast< const unsigned char* >( getSimpleDataArray( i, Type::CHAR ) ); }
	unsigned short				getWordAt( int i ) const						{ return *reinterpret_cast< const unsigned short* >( getSimpleDataArray( i, Type::SHORT ) ); }
	unsigned int				getDWordAt( int i ) const						{ return *reinterpret_cast< const unsigned int* >( getSimpleDataArray( i, Type::LONG ) ); }
	unsigned long long			getQWordAt( int i ) const						{ return *reinterpret_cast< const unsigned long long* >( getSimpleDataArray( i, Type::LLONG ) ); }
	float						getFloatAt( int i ) const						{ return *reinterpret_cast< const float* >( getSimpleDataArray( i, Type::FLOAT ) ); }
	double						getDoubleAt( int i ) const						{ return *reinterpret_cast< const double* >( getSimpleDataArray( i, Type::DOUBLE ) ); }
	const char*					getStringAt( int i ) const						{ return getStringValue( *reinterpret_cast< const int* >( getSimpleDataArray( i, Type::STRING ) ) ); }
	const wchar_t*				getWStringAt( int i ) const						{ return getWStringValue( *reinterpret_cast< const int* >( getSimpleDataArray( i, Type::WSTRING ) ) ); }

	const char*					getCharArray( int start = 0 ) const				{ return reinterpret_cast< const char* >( getSimpleDataArray( start, Type::CHAR ) ); }
	const short*				getShortArray( int start = 0 ) const			{ return reinterpret_cast< const short* >( getSimpleDataArray( start, Type::SHORT ) ); }
	const int*					getLongArray( int start = 0 ) const				{ return reinterpret_cast< const int* >( getSimpleDataArray( start, Type::LONG ) ); }
	const long long*			getLLongArray( int start = 0 ) const			{ return reinterpret_cast< const long long* >( getSimpleDataArray( start, Type::LLONG ) ); }
	const unsigned char*		getByteArray( int start = 0 ) const				{ return reinterpret_cast< const unsigned char* >( getSimpleDataArray( start, Type::CHAR ) ); }
	const unsigned short*		getWordArray( int start = 0 ) const				{ return reinterpret_cast< const unsigned short* >( getSimpleDataArray( start, Type::SHORT ) ); }
	const unsigned int*			getDWordArray( int start = 0 ) const			{ return reinterpret_cast< const unsigned int* >( getSimpleDataArray( start, Type::LONG ) ); }
	const unsigned long long*	getQWordArray( int start = 0 ) const			{ return reinterpret_cast< const unsigned long long* >( getSimpleDataArray( start, Type::LLONG ) ); }
	const float*				getFloatArray( int start = 0 ) const			{ return reinterpret_cast< const float* >( getSimpleDataArray( start, Type::FLOAT ) ); }
	const double*				getDoubleArray( int start = 0 ) const			{ return reinterpret_cast< const double* >( getSimpleDataArray( start, Type::DOUBLE ) ); }

	// string data & index conversion

	const char*					getStringValue( int strIndex ) const			{ return m_pParent->getString( strIndex ); }
	const wchar_t*				getWStringValue( int wstrIndex ) const			{ return m_pParent->getWString( wstrIndex ); }

private:

	friend class Mapper;

	MArray()  {}
	~MArray() {}

	void init( Mapper* pParent, int type, int fixedCount, const char* pBuf );
	void init( Mapper* pParent, int type, const char* pBuf );

	Mapper* m_pParent;
	const char* m_pBuf;
	int m_count;
	int m_type;
};


class MDict
{
public:

	// field / element info

	int							count() const									{ return int( m_fields.size() ); }

	const char*					nameAt( int i ) const;
	int							indexBy( const char* pName ) const;

	int							typeAt( int i ) const;
	int							fixedSizeAt( int i ) const;

	// data read interface

	const void*					getSimpleDataAt( int i, int type ) const;

	char						getCharAt( int i ) const						{ return *reinterpret_cast< const char* >( getSimpleDataAt( i, Type::CHAR ) ); }
	short						getShortAt( int i ) const						{ return *reinterpret_cast< const short* >( getSimpleDataAt( i, Type::SHORT ) ); }
	int							getLongAt( int i ) const						{ return *reinterpret_cast< const int* >( getSimpleDataAt( i, Type::LONG ) ); }
	long long					getLLongAt( int i ) const						{ return *reinterpret_cast< const long long* >( getSimpleDataAt( i, Type::LLONG ) ); }
	unsigned char				getByteAt( int i ) const						{ return *reinterpret_cast< const unsigned char* >( getSimpleDataAt( i, Type::CHAR ) ); }
	unsigned short				getWordAt( int i ) const						{ return *reinterpret_cast< const unsigned short* >( getSimpleDataAt( i, Type::SHORT ) ); }
	unsigned int				getDWordAt( int i ) const						{ return *reinterpret_cast< const unsigned int* >( getSimpleDataAt( i, Type::LONG ) ); }
	unsigned long long			getQWordAt( int i ) const						{ return *reinterpret_cast< const unsigned long long* >( getSimpleDataAt( i, Type::LLONG ) ); }
	float						getFloatAt( int i ) const						{ return *reinterpret_cast< const float* >( getSimpleDataAt( i, Type::FLOAT ) ); }
	double						getDoubleAt( int i ) const						{ return *reinterpret_cast< const double* >( getSimpleDataAt( i, Type::DOUBLE ) ); }
	const char*					getStringAt( int i ) const						{ return getStringValue( *reinterpret_cast< const int* >( getSimpleDataAt( i, Type::STRING ) ) ); }
	const wchar_t*				getWStringAt( int i ) const						{ return getWStringValue( *reinterpret_cast< const int* >( getSimpleDataAt( i, Type::WSTRING ) ) ); }
	MArrayPtr					getArrayAt( int i ) const;
	MTemplatePtr				getTemplateAt( int i ) const;
	MTemplateArrayPtr			getTemplateArrayAt( int i ) const;
	MDictPtr					getDictAt( int i ) const;

	char						getCharBy( const char* pName ) const			{ return getCharAt( indexBy( pName ) ); }
	short						getShortBy( const char* pName ) const			{ return getShortAt( indexBy( pName ) ); }
	int							getLongBy( const char* pName ) const			{ return getLongAt( indexBy( pName ) ); }
	long long					getLLongBy( const char* pName ) const			{ return getLLongAt( indexBy( pName ) ); }
	unsigned char				getByteBy( const char* pName ) const			{ return getByteAt( indexBy( pName ) ); }
	unsigned short				getWordBy( const char* pName ) const			{ return getWordAt( indexBy( pName ) ); }
	unsigned int				getDWordBy( const char* pName ) const			{ return getDWordAt( indexBy( pName ) ); }
	unsigned long long			getQWordBy( const char* pName ) const			{ return getQWordAt( indexBy( pName ) ); }
	float						getFloatBy( const char* pName ) const			{ return getFloatAt( indexBy( pName ) ); }
	double						getDoubleBy( const char* pName ) const			{ return getDoubleAt( indexBy( pName ) ); }
	const char*					getStringBy( const char* pName ) const			{ return getStringAt( indexBy( pName ) ); }
	const wchar_t*				getWStringBy( const char* pName ) const			{ return getWStringAt( indexBy( pName ) ); }
	MArrayPtr					getArrayBy( const char* pName ) const			{ return getArrayAt( indexBy( pName ) ); }
	MTemplatePtr				getTemplateBy( const char* pName ) const		{ return getTemplateAt( indexBy( pName ) ); }
	MTemplateArrayPtr			getTemplateArrayBy( const char* pName ) const	{ return getTemplateArrayAt( indexBy( pName ) ); }
	MDictPtr					getDictBy( const char* pName ) const			{ return getDictAt( indexBy( pName ) ); }

	// string data & index conversion

	const char*					getStringValue( int strIndex ) const			{ return m_pParent->getString( strIndex ); }
	const wchar_t*				getWStringValue( int wstrIndex ) const			{ return m_pParent->getWString( wstrIndex ); }

private:

	friend class Mapper;

	MDict()  {}
	~MDict() {}

	void init( Mapper* pParent, const char* pBuf );
	void close();

	Mapper* m_pParent;

	#pragma pack(push,1)

	union FieldHdr {
		int _typeAndName;
		struct Hdr{
			char _dummy[3];
			unsigned char _type;
		} hdr;
	};

	struct UnpackedFieldHdr {
		int _name;
		unsigned char _type;
	};

	#pragma pack(pop)

	struct Field {
		const FieldHdr* ptr;
		union {
			MTemplate*		asTemplate;
			MTemplateArray*	asTemplateArray;
			MArray*			asArray;
			MDict*			asDict;
		};
	};

	c_varray< Field > m_fields;

	const char* _getFieldData( const MDict::FieldHdr* ptr ) const;
	int _getFieldName( const MDict::FieldHdr* ptr ) const;
	unsigned char _getFieldType( const MDict::FieldHdr* ptr ) const;
};




// =================================================================================================
//   inline method implementations


inline const char* Mapper::getString( int i )
{
	assert( i >= 0 && i <= int( m_stringTable.size() ) );
	return i == 0 ? NULL : m_stringTable[ i-1 ];
}

inline const wchar_t* Mapper::getWString( int i )
{
	assert( i >= 0 && i <= int( m_wstringTable.size() ) );
	return i == 0 ? NULL : m_wstringTable[ i-1 ];
}

inline const GUID& Mapper::getId( int i )
{
	assert( i > 0 && i <= m_idCount );
	return m_pId[ i-1 ];
}

inline const ATInfo* Mapper::getATI( const GUID& id )
{
	const TemplateInfo* pInfo = m_pMetaData->getTemplateInfo( id );
	int i = m_TInfoIndexTable[ pInfo->getMetaDataIndex() ];
	return i == -1 ? NULL : &m_ATIs[ i ];
}

inline const void* MTemplate::getSimpleDataAt( int i, int type ) const
{
	assert( i >= 0 && i < count() && type == typeAt( i ) );
	return m_fields[ i ].ptr;
}

inline int MTemplate::readFixedData( void* pOutBuf, int bufSize ) const
{
	int size = fixedSize();
	if ( size < 0 )
		return -1;
	if ( bufSize < size )
		return 0;

	s_memcpy( pOutBuf, bufSize, m_fields.front().ptr, size );
	return size;
}

inline MArrayPtr MTemplate::getArrayAt( int i ) const
{
	assert( i >= 0 && i < count() && Type::isSimpleArrayType( typeAt( i ) ) );

	Field& v = const_cast< Field& >( m_fields[ i ] );
	if ( v.asArray == NULL )
	{
		if ( fixedSizeAt( i ) > 0 )
			v.asArray = m_pParent->createArray( Type::getElementType( typeAt( i ) ), fixedCountAt( i ), m_fields[ i ].ptr );
		else
			v.asArray = m_pParent->createArray( Type::getElementType( typeAt( i ) ), m_fields[ i ].ptr );
	}
	return v.asArray;
}

inline MTemplatePtr MTemplate::getTemplateAt( int i ) const
{
	assert( i >= 0 && i < count() && Type::TEMPLATE == typeAt( i ) );

	Field& v = const_cast< Field& >( m_fields[ i ] );
	if ( v.asTemplate == NULL )
	{
		v.asTemplate = m_pParent->createTemplate( info()->infoAt( i ), m_fields[ i ].ptr, false );
	}
	return v.asTemplate;
}

inline MTemplateArrayPtr MTemplate::getTemplateArrayAt( int i ) const
{
	assert( i >= 0 && i < count() && Type::TEMPLATE_ARRAY == typeAt( i ) );

	Field& v = const_cast< Field& >( m_fields[ i ] );
	if ( v.asTemplateArray == NULL )
	{
		if ( fixedSizeAt( i ) > 0 )
			v.asTemplateArray = m_pParent->createTemplateArray( info()->infoAt( i ), fixedCountAt( i ), m_fields[ i ].ptr );
		else
			v.asTemplateArray = m_pParent->createTemplateArray( info()->infoAt( i ), m_fields[ i ].ptr );
	}
	return v.asTemplateArray;
}

inline MDictPtr MTemplate::getDictAt( int i ) const
{
	assert( i >= 0 && i < count() && Type::DICT == typeAt( i ) );

	Field& v = const_cast< Field& >( m_fields[ i ] );
	if ( v.asDict == NULL )
	{
		v.asDict = m_pParent->createDict( m_fields[ i ].ptr );
	}
	return v.asDict;
}

inline int MTemplateArray::readFixedData( int i, int n, void* pOutBuf, int bufSize ) const
{
	int size = fixedSize();
	if ( size < 0 || i < 0 || n < 0 || bufSize <= 0 )
		return -1;
	
	if ( bufSize > 0 && (i+n)*size > bufSize )
		n = bufSize/size - i;

	if ( n > 0 )
		s_memcpy( pOutBuf, bufSize, m_fields.front().ptr + i * size, n * size );
	return n * size;
}

inline int MTemplateArray::readFixedData( void* pOutBuf, int bufSize ) const
{ 
	return readFixedData( 0, count(), pOutBuf, bufSize ); 
}

inline const void* MTemplateArray::getFixedData( int i ) const
{
	int size = fixedSize();
	if ( size < 0 || i < 0 || i >= count() )
		return NULL;

	return m_fields.front().ptr + i * size;
}

inline MTemplatePtr MTemplateArray::getTemplateAt( int i ) const
{
	assert( i >= 0 && i < int( m_fields.size() ) );
	Field& f = const_cast< Field& >( m_fields[ i ] );

	if ( f.asTemplate == NULL ) {
		f.asTemplate = m_pParent->createTemplate( m_pInfo, f.ptr, false );
	}
	return f.asTemplate;
}

inline int MArray::readFixedData( int i, int n, void* pOutBuf, int bufSize ) const
{
	if ( i < 0 || n < 0 || i >= count() || pOutBuf == NULL || bufSize <= 0 )
		return 0;

	if ( i+n > count() ) 
		n = count()-i;

	int elemSize = fixedSize();

	if ( bufSize > 0 && bufSize < n * elemSize )
		n = bufSize / elemSize;

	if ( n > 0 )
		s_memcpy( pOutBuf, bufSize, m_pBuf + i * elemSize, n * elemSize );
	return n * elemSize;
}

inline int MArray::readFixedData( void* pOutBuf, int bufSize ) const					
{ 
	return readFixedData( 0, count(), pOutBuf, bufSize ); 
}

inline const void* MArray::getSimpleDataArray( int start, int type ) const
{
	assert( type == m_type );
	return m_pBuf + start * Type::getTypeSize( type );
}

inline const char* MDict::_getFieldData( const MDict::FieldHdr* ptr ) const
{
	return reinterpret_cast< const char* >( ptr ) + (m_pParent->m_bUnpackedDict ? sizeof(UnpackedFieldHdr) : sizeof(FieldHdr) );
}

inline int MDict::_getFieldName( const MDict::FieldHdr* ptr ) const
{
	return m_pParent->m_bUnpackedDict ? reinterpret_cast< const UnpackedFieldHdr* >( ptr )->_name : ptr->_typeAndName & 0xffffff;
}

inline unsigned char MDict::_getFieldType( const MDict::FieldHdr* ptr ) const
{
	return m_pParent->m_bUnpackedDict ? reinterpret_cast< const UnpackedFieldHdr* >( ptr )->_type : ptr->hdr._type;
}

inline const char* MDict::nameAt( int i ) const							
{
	return m_pParent->getString( _getFieldName( m_fields[ i ].ptr ) ); 
}

inline int MDict::typeAt( int i ) const							
{ 
	return _getFieldType( m_fields[ i ].ptr ); 
}

inline int MDict::indexBy( const char* pName ) const
{
	if ( pName == NULL )
		return -1;

	for ( int i = 0, n = int( m_fields.size() ); i < n; i++ )
	{
		const char* pFieldName = m_pParent->getString( _getFieldName( m_fields[ i ].ptr ) );
		if ( !strcmp( pFieldName, pName ) )
			return i;
	}
	return -1;
}

inline int MDict::fixedSizeAt( int i ) const
{
	int type = typeAt( i );

	if ( Type::isSimpleType( type ) )
		return Type::getTypeSize( type );
	else if ( type == Type::TEMPLATE )
		return getTemplateAt( i )->fixedSize();
	else 
		return -1;
}

inline const void* MDict::getSimpleDataAt( int i, int type ) const
{
	assert( i >= 0 && i < int( m_fields.size() ) && type == _getFieldType( m_fields[ i ].ptr ) );
	return _getFieldData( m_fields[ i ].ptr );
}

inline MArrayPtr MDict::getArrayAt( int i ) const
{
	int type = typeAt( i );
	assert( Type::isSimpleArrayType( type ) );

	MArray*& pArray = const_cast< MArray*& >( m_fields[ i ].asArray );
	if ( pArray == NULL ) {
		pArray = m_pParent->createArray( type, _getFieldData( m_fields[ i ].ptr ) );
	}
	return pArray;
}

inline MTemplatePtr MDict::getTemplateAt( int i ) const
{
	assert( typeAt( i ) == Type::TEMPLATE );

	MTemplate*& pTpl = const_cast< MTemplate*& >( m_fields[ i ].asTemplate );
	if ( pTpl == NULL ) {
		pTpl = m_pParent->createTemplate( NULL, _getFieldData( m_fields[ i ].ptr ), true );
	}
	return pTpl;
}

inline MTemplateArrayPtr MDict::getTemplateArrayAt( int i ) const
{
	assert( typeAt( i ) == Type::TEMPLATE_ARRAY );

	MTemplateArray*& pTplArray = const_cast< MTemplateArray*& >( m_fields[ i ].asTemplateArray );
	if ( pTplArray == NULL ) {
		pTplArray = m_pParent->createTemplateArray( NULL, _getFieldData( m_fields[ i ].ptr ) );
	}
	return pTplArray;
}

inline MDictPtr MDict::getDictAt( int i ) const
{
	assert( typeAt( i ) == Type::DICT );

	MDict*& pDict = const_cast< MDict*& >( m_fields[ i ].asDict );
	if ( pDict == NULL ) {
		pDict = m_pParent->createDict( _getFieldData( m_fields[ i ].ptr ) );
	}
	return pDict;
}

} // namespace trf
