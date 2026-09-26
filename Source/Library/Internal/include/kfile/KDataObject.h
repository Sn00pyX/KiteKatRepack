#if !defined(_KDATAOBJECT_H_INCLUDED_)
#define _KDATAOBJECT_H_INCLUDED_


#include "KStream.h"
#include "../toolkit/khash.h"
#include "KTemplateInfo.h"
#include <Windows.h>

class KDataObjectContainer;
class KTemplateDataObject;
class KDataObject
{
public:
	enum CLASSTYPEID
	{
		CLASSTYPEID_NONE		= 0,

		CLASSTYPEID_WORD		= 0x100,
		CLASSTYPEID_DWORD		= 0x101,
		CLASSTYPEID_FLOAT		= 0x102,
		CLASSTYPEID_CHAR		= 0x103,
		CLASSTYPEID_UCHAR		= 0x104,
		CLASSTYPEID_STRING		= 0x105,

		CLASSTYPEID_ARRAY_CHAR	= 0x200,
		CLASSTYPEID_ARRAY_UCHAR	= 0x201,
		CLASSTYPEID_ARRAY_WORD	= 0x202,
		CLASSTYPEID_ARRAY_DWORD = 0x203,
		CLASSTYPEID_ARRAY_FLOAT = 0x204,
		CLASSTYPEID_ARRAY_STRING = 0x205,

		CLASSTYPEID_TEMPLATE	= 0x300,
		CLASSTYPEID_TEMPLATEARRAY = 0x301,
	};

	KDataObject( CLASSTYPEID idClassType )
		: m_idClassType(idClassType)
		, m_tabSize( 0 )
	{}
	virtual ~KDataObject() {}

	void AddDataObject( KDataObject *obj );
	KDataObject* GetDataObjectByIndex( int index )	{ return m_pDataObjects[index]; }
	int GetDataObjectCount()						{ return static_cast<int>(m_pDataObjects.size()); }

	CLASSTYPEID GetClassType()						{ return m_idClassType; }
	bool IsClassType( CLASSTYPEID idClassType )		{ return m_idClassType == idClassType; }

	enum FileMode
	{
		KDFM_BINARY,
		KDFM_ASCII,
	};
	virtual bool Save( KStream &stream, FileMode filemode = KDFM_BINARY ) = 0;
	virtual bool Load( KStream &stream, FileMode filemode = KDFM_BINARY ) = 0;
	/// CloneTypeInfo로 복사된 녀석들을 위해서
	virtual bool LoadFast(KStream& stream) = 0;
	virtual int GetIndexValue()			{ return -1; }
	virtual KDataObject* CloneTypeInfo(KTemplateDataObject* pParent) = 0;

	void SetTab(int t)	{m_tabSize = t;}
	int GetTab()	{return m_tabSize;}

protected:
	std::vector<KDataObject*> m_pDataObjects;
	CLASSTYPEID m_idClassType;

	int m_tabSize;
};

class KSimpleDataObject : public KDataObject
{
public:
	KSimpleDataObject(CLASSTYPEID idclass);
	virtual ~KSimpleDataObject();

	bool SetData( const void *data, int nSize );
	inline bool GetData( void *data, int &nSize );

	bool SetDWORD( DWORD data );
	bool SetWORD( WORD data );
	bool SetFloat( float data );
	bool SetUChar( unsigned char data );
	bool SetChar( char data );
	bool SetString( const char *data, int nLen );

	inline bool GetDWORD( DWORD *data );
	inline bool GetWORD( WORD *data );
	inline bool GetFloat( float *data );
	inline bool GetUChar( unsigned char *data );
	inline bool GetChar( char *data );
	inline bool GetString( char *data, int &nLen );

	int GetSize()			{ return m_nSize; }

	virtual bool Save( KStream &stream, FileMode filemode = KDFM_BINARY );
	virtual bool Load( KStream &stream, FileMode filemode = KDFM_BINARY );
	virtual bool LoadFast(KStream& stream);
	virtual int GetIndexValue();
	virtual KDataObject* CloneTypeInfo(KTemplateDataObject* pParent);

	bool SaveHeader( KStream &stream, FileMode filemode = KDFM_BINARY );
	bool LoadHeader( KStream &stream, FileMode filemode = KDFM_BINARY );
	bool SaveBody( KStream &stream, FileMode filemode = KDFM_BINARY );
	bool LoadBody( KStream &stream, FileMode filemode = KDFM_BINARY );



protected:
	union{
		DWORD _4byteint;
		WORD  _2byteint;
		float _4bytefloat;
		unsigned char _1byteuchar;
		char _1bytechar; 
	} m_data;
	char *m_pStr;
	int	 m_nSize;
};

class KSimpleDataArrayObject : public KDataObject
{
public:
	KSimpleDataArrayObject( CLASSTYPEID idclass = CLASSTYPEID_NONE, KTemplateDataObject *pParent = NULL )
		: KDataObject( CLASSTYPEID_NONE )
	{
		m_idClassType = idclass;
		m_pArrayData = NULL;
		m_nDataSize = m_nDataCount = 0;
		m_pParent = pParent;
		m_strLength = 0;
		m_varCount = NULL;
	}
	~KSimpleDataArrayObject()
	{
		if ( m_pArrayData ) delete[] m_pArrayData;
	}

	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent );

	bool SetDataString( int index, const char *str ); 
	bool SetDataChar( int index, const char *data, int nCount = 1 );
	bool SetDataUChar( int index, const unsigned char *data, int nCount = 1 );
	bool SetDataDWORD( int index, const DWORD *data, int nCount = 1 );
	bool SetDataWORD( int index, const WORD *data, int nCount = 1 );
	bool SetDataFloat( int index, const float *data, int nCount = 1 );

	inline const char *GetDataString( int index );
	inline bool GetDataChar( int index, char *data, size_t data_size, int nCount );
	inline bool GetDataUChar( int index, unsigned char *data, size_t data_size, int nCount );
	inline bool GetDataDWORD( int index, DWORD *data, size_t data_size, int nCount );
	inline bool GetDataWORD( int index, WORD *data, size_t data_size, int nCount );
	inline bool GetDataFloat( int index, float *data, size_t data_size, int nCount );

	inline int GetDataCount();

	void RefreshArray();

	virtual bool Save( KStream &stream, FileMode filemode = KDFM_BINARY );
	virtual bool Load( KStream &stream, FileMode filemode = KDFM_BINARY );
	virtual bool LoadFast(KStream& stream);
	virtual KDataObject* CloneTypeInfo(KTemplateDataObject* pParent);

	bool SaveHeader( KStream &stream, FileMode filemode = KDFM_BINARY );
	bool LoadHeader( KStream &stream, FileMode filemode = KDFM_BINARY );
	bool SaveBody( KStream &stream, FileMode filemode = KDFM_BINARY );
	bool LoadBody( KStream &stream, FileMode filemode = KDFM_BINARY );

private:
	inline bool _MemCopy(void *pDataOut, size_t out_size, int index, int count);

protected:
	char *m_pArrayData;
	std::vector<std::string> m_vecStringArray;
	int m_nDataSize;
	int m_nDataCount;
	long m_strLength;

	std::string m_strVarCount;
	KDataObject *m_varCount;
	KTemplateDataObject *m_pParent;
};

class KTemplateDataArrayObject : public KDataObject
{
public:
	KTemplateDataArrayObject(KTemplateDataObject *pParent) : KDataObject( CLASSTYPEID_TEMPLATEARRAY )
	{
		m_pParent = pParent;
		m_tabSize = 0;
		m_nDataCount = 0;
		m_strLength = 0;
		m_varCount = NULL;
	}


	KTemplateDataArrayObject( const GUID &uuidDataTemplateType ) : KDataObject( CLASSTYPEID_TEMPLATEARRAY )
	{
		m_uuidTemplateType = uuidDataTemplateType;
		m_pParent = NULL;
		m_tabSize = 0;
		m_nDataCount = 0;
		m_strLength = 0;
		m_varCount = NULL;
	}
	KTemplateDataArrayObject( KStream &stream, FileMode filemode = KDFM_BINARY, KTemplateDataObject *pParent = NULL ) : KDataObject( CLASSTYPEID_TEMPLATEARRAY )
	{
		m_pParent = pParent;
		Load( stream, filemode );
		m_tabSize = 0;
	}
	virtual ~KTemplateDataArrayObject();
	void Clear();

	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent );

	void RefreshArray();

	void ChangeGUID(const GUID& uuid);

	void SetData( KStream &streamData, int data_count, FileMode filemode = KDFM_BINARY );
	inline int GetDataCount();
	inline KTemplateDataObject *GetData( int index );
	KTemplateDataObject *GetParent()
	{
		return m_pParent;
	}

	virtual bool Save( KStream &stream, FileMode filemode = KDFM_BINARY );
	virtual bool Load( KStream &stream, FileMode filemode = KDFM_BINARY );
	virtual bool LoadFast(KStream& stream);
	virtual KDataObject* CloneTypeInfo(KTemplateDataObject* pParent);

	bool SaveHeader( KStream &stream, FileMode filemode = KDFM_BINARY );
	bool LoadHeader( KStream &stream, FileMode filemode = KDFM_BINARY );	

	inline bool LoadBody( KStream &stream, FileMode filemode = KDFM_BINARY );

	void PrintTab( KStream &stream,FileMode filemode );

protected:
	GUID m_uuidTemplateType;
	std::vector<KTemplateDataObject*> m_vectorData;
	int m_nDataCount;
	long m_strLength;
	std::string m_strVarCount;
	KDataObject *m_varCount;
	KTemplateDataObject *m_pParent;
};

class KTemplateInfo;
class KTemplateDataObject : public KDataObject
{
public:
	KTemplateDataObject() : KDataObject( CLASSTYPEID_TEMPLATE )
	{
		m_tabSize = 0;
		m_memberInfo = NULL;
	}
	KTemplateDataObject( const GUID &uuidType ) : KDataObject( CLASSTYPEID_TEMPLATE )
		, m_memberInfo(NULL)
	{
		m_uuidTemplate = uuidType;
		getMemberData(true);
		m_tabSize = 0;
	}
	KTemplateDataObject( KStream &stream, FileMode filemode = KDFM_BINARY ) : KDataObject( CLASSTYPEID_TEMPLATE )
		, m_memberInfo(NULL)
	{
		Load( stream, filemode );
		m_tabSize = 0;
	}

	KTemplateDataObject( KStream &stream, bool &retval, FileMode filemode = KDFM_BINARY ) : KDataObject( CLASSTYPEID_TEMPLATE )
		, m_memberInfo(NULL)
	{
		retval = Load( stream, filemode );
		m_tabSize = 0;
	}


	virtual ~KTemplateDataObject();

	inline const char *GetTemplateName();
	
	void ChangeTemplateInfo(KTemplateInfo* pNewInfo);

	void AddExtraTemplateMember( KTemplateDataObject *obj );
	int GetExtraMemberCount();
	KTemplateDataObject* GetExtraMember( int index );
	const char *GetExtraMemberName( int index );

	// enumeration
	inline KDataObject* GetMemberObject( const char *name );
	inline KSimpleDataObject* GetSimpleDataMemberObject( const char *name );
	inline KSimpleDataArrayObject* GetSimpleDataArrayMemberObject( const char *name );
	inline KTemplateDataObject* GetTemplateDataMemberObject( const char *name );
	inline KTemplateDataArrayObject* GetTemplateDataArrayMemberObject( const char *name );

	int GetMemberCount();
	const char* GetMemberNameByIndex( int index );

	
	/// 특정 Membe 이후로 count 만큼 넓혀준다. index 뒤에 있는 녀석들은 넒어진 공간만큼 뒤로 밀림)
	void ExtendMemberVector(const char* name, size_t count);
	void SetMemberObject(size_t index, KDataObject* pMemberObject);
	void AddMemberObject(KDataObject* pNewObj);

	// basic i/o
	virtual bool Save( KStream &stream, FileMode filemode = KDFM_BINARY );
	virtual bool Load( KStream &stream, FileMode filemode = KDFM_BINARY );
	virtual bool LoadFast(KStream& stream);
	virtual KDataObject* CloneTypeInfo(KTemplateDataObject* pParent);

	void PrintTab( KStream &stream,FileMode filemode );

protected:
	void clearMembers();
	void getMemberData( bool bCreateMember );
protected:
	///KHash<KDataObject*, hashPr_string> m_hashByName;
	std::vector<KDataObject*> m_vectorMemberObjects;

	GUID m_uuidTemplate;

	// member
	KTemplateInfo *m_memberInfo;
};

struct hashPr_GUID
{
	struct Key 
	{
		Key()							  {}
		Key( const Key& _guid, bool )	  { s_memcpy( &m_guid, sizeof( m_guid ), &_guid.m_guid, sizeof(m_guid) ); }
		Key( const GUID& _guid )		  { s_memcpy( &m_guid, sizeof( m_guid ), &_guid, sizeof(m_guid) ); }
		~Key()							  {}
		GUID m_guid;
	};

	static inline unsigned getindex( Key key, int nCapacity )
	{
		return (key.m_guid.Data1 + key.m_guid.Data2 + key.m_guid.Data3) % nCapacity;
	}
	static inline bool isequal( Key key1, Key key2 )
	{
		return memcmp( &key1.m_guid, &key2.m_guid, sizeof(key1.m_guid) ) == 0;
	}
	static inline bool isless( Key key1, Key key2 )
	{
		return memcmp( &key1.m_guid, &key2.m_guid, sizeof(key1.m_guid) ) < 0;
	}
};

// KTOKENs

const char KTOKEN_EOF			= -1;

const char KTOKEN_NAME			= 1;
const char KTOKEN_INTEGER		= 3;
const char KTOKEN_GUID			= 5;
const char KTOKEN_INTEGER_LIST	= 6;
const char KTOKEN_FLOAT_LIST	= 7;

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
const char KTOKEN_UCHAR			= 45;
const char KTOKEN_SWORD			= 46;
const char KTOKEN_SDWORD		= 47;
const char KTOKEN_VOID			= 48;
const char KTOKEN_LPSTR			= 49;
const char KTOKEN_UNICODE		= 50;
const char KTOKEN_STRING		= 51;
const char KTOKEN_ARRAY			= 52;

struct KTemplateToken
{
	char TokenID;
	std::string StrToken;
};

// -----------------------------------------------------------------------------------

inline bool KSimpleDataObject::GetDWORD( DWORD *data )
{
	assert(m_idClassType == CLASSTYPEID_DWORD);
	*data = m_data._4byteint;
	return true;
}

inline bool KSimpleDataObject::GetWORD( WORD *data )
{
	assert(m_idClassType == CLASSTYPEID_WORD);
	*data = m_data._2byteint;
	return true;
}

inline bool KSimpleDataObject::GetFloat( float *data )
{
	assert(m_idClassType == CLASSTYPEID_FLOAT);
	*data = m_data._4bytefloat;
	return true;
}

inline bool KSimpleDataObject::GetUChar( unsigned char *data )
{
	assert(m_idClassType == CLASSTYPEID_UCHAR);
	*data = m_data._1byteuchar;
	return true;
}

inline bool KSimpleDataObject::GetChar( char *data )
{
	assert(m_idClassType == CLASSTYPEID_CHAR);
	*data = m_data._1bytechar;
	return true;
}

inline bool KSimpleDataObject::GetString( char *data, int &nLen )
{
	assert(m_idClassType == CLASSTYPEID_STRING);
	int data_size = nLen;
	if ( nLen < m_nSize )
	{
		nLen = m_nSize;
		return false;
	}
	
	if ( nLen > m_nSize )
		nLen = m_nSize;

	s_memcpy( data, data_size, m_pStr, nLen );
	return true;
}

inline bool KSimpleDataObject::GetData( void *data, int &nSize )
{
	if ( m_idClassType == CLASSTYPEID_STRING )
	{
		if ( nSize >= m_nSize )
		{
			s_memcpy( data, nSize, m_pStr, m_nSize );
			return true;
		}
	}
	else if ( nSize >= m_nSize )
	{
		switch( m_idClassType )
		{
		case CLASSTYPEID_WORD:
			s_memcpy( data, nSize, &m_data._2byteint, m_nSize );
			break;
		case CLASSTYPEID_DWORD:
		case CLASSTYPEID_FLOAT:
			s_memcpy( data, nSize, &m_data._4byteint, m_nSize );
			break;
		case CLASSTYPEID_CHAR:
		case CLASSTYPEID_UCHAR:
			s_memcpy( data, nSize, &m_data._1byteuchar, m_nSize );
			break;
		default:
			return false;
		}
		return true;
	}
	return false;
}


inline int KSimpleDataArrayObject::GetDataCount()
{
	if ( m_nDataCount == -1 )
	{
		assert( m_varCount );
		return m_varCount->GetIndexValue();
	}
	return m_nDataCount;
}

inline const char *KSimpleDataArrayObject::GetDataString( int index )
{
	assert(m_idClassType == CLASSTYPEID_ARRAY_STRING && index >= 0 && index < static_cast<int>(m_vecStringArray.size() ) );
	return m_vecStringArray[index].c_str();
}

inline bool KSimpleDataArrayObject::GetDataChar( int index, char *data, size_t data_size, int count )
{
	assert(m_idClassType == CLASSTYPEID_ARRAY_CHAR && m_pArrayData);

	m_nDataSize = 1;
	return _MemCopy(data, data_size, index, count);
}

inline bool KSimpleDataArrayObject::GetDataUChar( int index, unsigned char *data, size_t data_size, int count )
{
	assert(m_idClassType == CLASSTYPEID_ARRAY_UCHAR && m_pArrayData);

	m_nDataSize = 1;
	return _MemCopy(data, data_size, index, count);
}

inline bool KSimpleDataArrayObject::GetDataDWORD( int index, DWORD *data, size_t data_size, int count )
{
	assert(m_idClassType == CLASSTYPEID_ARRAY_DWORD && m_pArrayData);

	m_nDataSize = 4;
	return _MemCopy(data, data_size, index, count);
}

inline bool KSimpleDataArrayObject::GetDataWORD( int index, WORD *data, size_t data_size, int count )
{
	assert(m_idClassType == CLASSTYPEID_ARRAY_WORD && m_pArrayData);

	m_nDataSize = 2;
	return _MemCopy(data, data_size, index, count);
}

inline bool KSimpleDataArrayObject::GetDataFloat( int index, float *data, size_t data_size, int count )
{
	assert(m_idClassType == CLASSTYPEID_ARRAY_FLOAT && m_pArrayData);

	m_nDataSize = 4;
	return _MemCopy(data, data_size, index, count);
}


inline bool KSimpleDataArrayObject::_MemCopy(void *pDataOut, size_t out_size, int index, int count)
{
	s_memcpy( pDataOut, out_size, m_pArrayData + (m_nDataSize*index), m_nDataSize*count );
	return true;
}

inline KDataObject* KTemplateDataObject::GetMemberObject( const char *name )
{
	int index = m_memberInfo->GetMemberIndex( name );
	if ( index == -1 || index >= static_cast<int>( m_vectorMemberObjects.size() ) )
		return NULL;
	return m_vectorMemberObjects[index];
}

inline KSimpleDataObject* KTemplateDataObject::GetSimpleDataMemberObject( const char *name )
{
	KDataObject *obj = GetMemberObject( name );
	if ( obj == NULL ) return NULL;
	KSimpleDataObject *result = NULL;
	switch ( obj->GetClassType() )
	{
	case CLASSTYPEID_WORD:
	case CLASSTYPEID_DWORD:
	case CLASSTYPEID_FLOAT:
	case CLASSTYPEID_CHAR:
	case CLASSTYPEID_UCHAR:
	case CLASSTYPEID_STRING:
		result = reinterpret_cast<KSimpleDataObject*>(obj);
		break;
	}
	return result;
}

inline KSimpleDataArrayObject* KTemplateDataObject::GetSimpleDataArrayMemberObject( const char *name )
{
	KDataObject *obj = GetMemberObject( name );
	if ( obj == NULL ) return NULL;
	KSimpleDataArrayObject *result = NULL;
	switch ( obj->GetClassType() )
	{
	case CLASSTYPEID_ARRAY_CHAR:
	case CLASSTYPEID_ARRAY_UCHAR:
	case CLASSTYPEID_ARRAY_WORD:
	case CLASSTYPEID_ARRAY_DWORD:
	case CLASSTYPEID_ARRAY_FLOAT:
	case CLASSTYPEID_ARRAY_STRING:
		result = reinterpret_cast<KSimpleDataArrayObject*>(obj);
		break;
	}
	return result;
}

inline KTemplateDataObject* KTemplateDataObject::GetTemplateDataMemberObject( const char *name )
{
	KDataObject *obj = GetMemberObject( name );
	if ( obj == NULL ) return NULL;
	KTemplateDataObject *result = NULL;
	if ( obj->GetClassType() == CLASSTYPEID_TEMPLATE )
	{
		result = reinterpret_cast<KTemplateDataObject*>(obj);
	}
	return result;
}

inline KTemplateDataArrayObject* KTemplateDataObject::GetTemplateDataArrayMemberObject( const char *name )
{
	KDataObject *obj = GetMemberObject( name );
	if ( obj == NULL ) return NULL;
	KTemplateDataArrayObject *result = NULL;
	if ( obj->GetClassType() == CLASSTYPEID_TEMPLATEARRAY )
	{
		result = reinterpret_cast<KTemplateDataArrayObject*>(obj);
	}
	return result;
}

inline int KTemplateDataObject::GetMemberCount()
{
	return m_memberInfo->GetMemberCount();
}

inline const char* KTemplateDataObject::GetMemberNameByIndex( int index )
{
	if ( index < GetMemberCount() )
		return m_memberInfo->GetMemberName( index );
	return NULL;
}

inline const char *KTemplateDataObject::GetTemplateName()
{
	if ( m_memberInfo )
	{
		return m_memberInfo->GetTemplateName();
	}
	return NULL;
}

inline int KTemplateDataArrayObject::GetDataCount()
{
	if ( m_nDataCount == -1 )
	{
		assert( m_varCount );
		return m_varCount->GetIndexValue();
	}
	return m_nDataCount;
}

inline KTemplateDataObject *KTemplateDataArrayObject::GetData( int index )
{
	if ( (int)m_vectorData.size() > index )
	{
		return m_vectorData[index];
	}
	assert(false);
	return NULL;
}

#endif  // !defined(_KDATAOBJECT_H_INCLUDED_)
