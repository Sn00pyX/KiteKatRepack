#pragma once

// -
// -  TRF : Template-based Recursive Format
// -
// -  trfMetaData.h
// -
// -    basic datatype and template metadata info
// -
// -    2006.2.14 by Young-Hyun Joo, nFlavor
// -
//
// -	하위호환성을 위해 long 을 32bit 로 간주한다. by Testors 2009/10/08

#include "../toolkit/khash.h"
#include "KStream.h"
#include <guiddef.h>
#include <vector>
#include <string>
#include <windows.h>

namespace trf {


struct Exception
{
	Exception( int _code = 0, const char* _desc = "exception" ) { code = _code; desc = _desc; }

	const char* desc;
	int code;
};


struct Type 
{
	enum 
	{
		_NONE			= 0x00,

		CHAR,
		SHORT,
		LONG,
		LLONG,
		FLOAT,
		DOUBLE,
		STRING,
		WSTRING,

		TEMPLATE,
		DICT,

		_ARRAYMASK		= 0x80,

		CHAR_ARRAY,
		SHORT_ARRAY,
		LONG_ARRAY,
		LLONG_ARRAY,
		FLOAT_ARRAY,
		DOUBLE_ARRAY,
		STRING_ARRAY,
		WSTRING_ARRAY,

		TEMPLATE_ARRAY
	};

	static inline int	getTypeSize( int type );
	static inline bool	isSimpleType( int type );
	static inline bool	isSimpleArrayType( int type );
	static inline int	getArrayType( int type );
	static inline int	getElementType( int type );
};


class TemplateInfo
{
public:

	const GUID&				id() const								{ return m_id; }
	const char*				name() const							{ return m_strName.c_str(); }
	int						fixedSize() const						{ return m_fixedSize; }

	int						indexBy( const char* pName ) const		{ int index; return m_hashByName.lookup( pName, index ) ? index : -1; }

	int						count() const							{ return int( m_fields.size() ); }
	const char*				nameAt( int i ) const					{ return m_fields[ i ].name.c_str(); }
	int						typeAt( int i ) const					{ return m_fields[ i ].type; }
	int						fixedCountAt( int i ) const				{ return m_fields[ i ].fixedCount; }
	int						fixedSizeAt( int i ) const				{ return m_fields[ i ].fixedSize; }

	const TemplateInfo*		infoAt( int i ) const					{ return m_fields[ i ].pInfo; }

	int						getMetaDataIndex() const				{ return m_metaDataIndex; }

	//

	void	clear();
	bool	removeField( const char* pFieldName );

	bool	addSimpleField( const char* pFieldName, int type );
	bool	addArrayField( const char* pFieldName, int elementType, int fixedCount = 0 );
	bool	addTemplateField( const char* pFieldName, const TemplateInfo* pInfo );
	bool	addTemplateArrayField( const char* pFieldName, const TemplateInfo* pElementInfo, int fixedCount = 0 );
	bool	addDictField( const char* pFieldName );

	//

	int		calcFixedSize( int lastField ) const;

private:

	friend class MetaData;

	TemplateInfo( const GUID& uuidType, const char* pName, int i );
	~TemplateInfo()													{ clear(); }

	struct FieldInfo {
		std::string				name;
		int						type;
		const TemplateInfo*		pInfo;
		int						fixedCount;
		int						fixedSize;
	};

	GUID						m_id;
	std::string					m_strName;
	KHash< int, hashPr_string > m_hashByName;
	std::vector< FieldInfo >	m_fields;
	int							m_fixedSize;
	int							m_metaDataIndex;
};


struct ATInfo		// augmented template info (TI with field limit)
{
	int fieldLimit;
	int fixedSize;
	const TemplateInfo* pInfo;
};


class MetaData
{
public:

	MetaData();
	~MetaData();

	void clear();
	bool importTML( KStream& stream );

	const TemplateInfo* getTemplateInfo( const GUID& uuidType ) const;
	const TemplateInfo* getTemplateInfo( const char *pName ) const;

	int getTemplateInfoCount() const												{ return int( m_infos.size() ); }
	const TemplateInfo* getTemplateInfoAt( int i ) const							{ return m_infos[ i ]; }

	TemplateInfo* addNewTemplateInfo( const GUID& uuidType, const char* pName );

private:

	friend class Filer;
	friend class Mapper;

	void addRef() const;
	void subRef() const;

	struct hashPr_GUID
	{
		struct Key {
			Key( const Key& _guid, bool )	  { s_memcpy( &m_guid, sizeof( m_guid ), &_guid.m_guid, sizeof(m_guid) ); }
			Key( const GUID& _guid )		  { s_memcpy( &m_guid, sizeof( m_guid ), &_guid, sizeof(m_guid) ); }
			GUID m_guid;
		};

		static inline unsigned int getindex( Key key, int nCapacity ) {
			return ((unsigned int)key.m_guid.Data1 + key.m_guid.Data2 + key.m_guid.Data3) % nCapacity;
		}

		static inline bool isequal( Key key1, Key key2 ) {
			return memcmp( &key1, &key2, sizeof(GUID) ) == 0;
		}

		static inline bool isless( Key key1, Key key2 ) {
			return memcmp( &key1, &key2, sizeof(GUID) ) < 0;
		}
	};

	KHash< TemplateInfo*, hashPr_GUID >		m_hashByGUID;
	KHash< TemplateInfo*, hashPr_string >	m_hashByName;
	std::vector< TemplateInfo* >			m_infos;

	volatile mutable LONG m_ref;
};


bool IsTRFStream( KStream& stream );


// ====================================================================================================================


inline int Type::getTypeSize( int type )
{
	switch ( type )
	{
	case CHAR:		return sizeof(char);
	case SHORT:		return sizeof(short);
	case STRING:	
	case WSTRING:	
	case LONG:		return sizeof(int);
	case LLONG:		return sizeof(long long);
	case FLOAT:		return sizeof(float);
	case DOUBLE:	return sizeof(double);
	default:		return -1;
	}
}

inline bool Type::isSimpleType( int type )
{
	switch ( type )
	{
	case CHAR:
	case SHORT:
	case LONG:
	case LLONG:
	case FLOAT:
	case DOUBLE:
	case STRING:
	case WSTRING:	return true;
	default:		return false;
	}
}

inline bool Type::isSimpleArrayType( int type )
{
	switch ( type )
	{
	case CHAR_ARRAY:
	case SHORT_ARRAY:
	case LONG_ARRAY:
	case LLONG_ARRAY:
	case FLOAT_ARRAY:
	case DOUBLE_ARRAY:	
	case STRING_ARRAY:	
	case WSTRING_ARRAY:	return true;
	default:			return false;
	}
}

inline int Type::getArrayType( int type )
{
	return type | Type::_ARRAYMASK;
}

inline int Type::getElementType( int type )
{
	return type & ~Type::_ARRAYMASK;
}

inline TemplateInfo::TemplateInfo( const GUID& uuidType, const char* pName, int i )
	: m_strName( pName )
{ 
	m_id = uuidType;
	m_fixedSize = 0;
	m_metaDataIndex = i;
}



// ====================================================================================================================


#pragma pack(push,1)

struct FileHeader
{
	char	idstr[ 4 ];
	// long	headerInfoSize;	// 64비트 때문에.. by Testors
	int		headerInfoSize;
};

struct HeaderInfo
{
	short	majorVersion;
	short	minorVersion;
};

#pragma pack(pop)


template< class T >
class SmartPtr
{
public:

	SmartPtr( const T* pObj = 0 )
	{
		m_pObj = pObj;
		
		if (m_pObj) ++m_pObj->m_refCount;
	}

	~SmartPtr()
	{
		if (m_pObj) m_pObj->release();
	}

	// Copy constructor

	SmartPtr( const SmartPtr& rhs )
	{
		m_pObj = rhs.m_pObj;

		if (m_pObj) ++m_pObj->m_refCount;
	}

	// Assign

	SmartPtr& operator = ( const SmartPtr& rhs )	{ _Copy(rhs.m_pObj); return *this; }
	SmartPtr& operator = ( T* pObject )				{ _Copy(pObject); return *this; }

	// Conversion

	operator T* ()									{ return m_pObj; }
	operator const T* () const						{ return m_pObj; }
    const T* operator -> () const					{ return m_pObj; }
	const T& operator * () const					{ return *m_pObj; }
	T* operator -> ()								{ return m_pObj; }
	T& operator * ()								{ return *m_pObj; }
	
	// Compare
	
	bool operator == ( const SmartPtr& ptr ) const	{ return m_pObj == ptr.m_pObj; }
	bool operator != ( const SmartPtr& ptr ) const	{ return m_pObj != ptr.m_pObj; }

private:

	void _Copy( T* pObj )
	{
		if ( pObj != m_pObj ) {
			if ( m_pObj ) m_pObj->release();
			m_pObj = pObj;
			if ( m_pObj ) ++m_pObj->m_refCount;
		}
	}

	const T*	m_pObj;
};


} // namespace trf