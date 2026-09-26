#pragma once

// -
// -  TRF : Template-based Recursive Format
// -
// -  trfFiler.h
// -
// -    On-memory filer for TRF file building & editing
// -
// -    2006.2.14 by Young-Hyun Joo, nFlavor
// -

#include "TrfMetaData.h"
#include "KStream.h"

namespace trf {


struct FilerObjects
{
	typedef const class ITemplate*      TemplatePtr;
	typedef const class ITemplateArray* TemplateArrayPtr;
	typedef const class IArray*         ArrayPtr;
	typedef const class IDict*          DictPtr;
};

typedef class ITemplate*      ITemplatePtr;
typedef class ITemplateArray* ITemplateArrayPtr;
typedef class IArray*         IArrayPtr;
typedef class IDict*          IDictPtr;

class Filer
{
public:

	Filer( MetaData* pMetaData );
	~Filer();

	void clear();

	bool save( KStream& stream );
	bool load( KStream& stream );

	MetaData* metaData();
	IDict* root();

	typedef void (*TInitTemplateFn)( const TemplateInfo* pInfo, ITemplate* pTpl );

	void setTemplateInitCallback( TInitTemplateFn fn = NULL );

private:

	Filer( const Filer& );
	Filer& operator=( const Filer& );

	struct Imp* m_pImp;
};

class ITemplate
{
public:

	// class info 

	const TemplateInfo*				info() const 															{ return m_pATI->pInfo; }
	const GUID&						classId() const 														{ return m_pATI->pInfo->id(); }
	const char*						className() const 														{ return m_pATI->pInfo->name(); }
	int								fixedSize() const														{ return m_pATI->fixedSize; }
	bool							isFixed() const															{ return fixedSize() >= 0; }

	// field info

	int								count() const 															{ return m_pATI->fieldLimit; }

	int								indexBy( const char* pName ) const 										{ return m_pATI->pInfo->indexBy( pName ); }
	const char*						nameAt( int i ) const 													{ return m_pATI->pInfo->nameAt( i ); }

	int								typeAt( int i ) const 													{ return m_pATI->pInfo->typeAt( i ); }
	int								fixedCountAt( int i ) const												{ return m_pATI->pInfo->fixedCountAt( i ); }
	int								fixedSizeAt( int i ) const												{ return m_pATI->pInfo->fixedSizeAt( i ); }

	// data read 

	virtual int						readFixedData( void* pOutBuf, int bufSize = 0 )	const					= 0;

	virtual const void*				getSimpleDataAt( int i, int type ) const								= 0;

	virtual const IArray*			getArrayAt( int i ) const												= 0;
	virtual const ITemplate*		getTemplateAt( int i ) const											= 0;
	virtual const ITemplateArray*	getTemplateArrayAt( int i ) const										= 0;
	virtual const IDict*			getDictAt( int i ) const												= 0;

	// data build

	virtual void					clear()																	= 0;

	virtual bool					setSimpleDataAt( int i, const void* pData, int type )					= 0;

	virtual IArray*					arrayAt( int i )														= 0;
	virtual ITemplate*				templateAt( int i )														= 0;
	virtual ITemplateArray*			templateArrayAt( int i )												= 0;
	virtual IDict*					dictAt( int i )															= 0;

	// string data & index conversion

	virtual const char*				getStringValue( long strIndex ) const									= 0;
	virtual const wchar_t*			getWStringValue( long wstrIndex ) const									= 0;

	virtual long					addStringValue( const char* pStrValue )									= 0;
	virtual long					addWStringValue( const wchar_t* pWStrValue )							= 0;

	// convenience wrappers

	char							getCharAt( int i ) const												{ return *reinterpret_cast< const char* >( getSimpleDataAt( i, Type::CHAR ) ); }
	short							getShortAt( int i ) const												{ return *reinterpret_cast< const short* >( getSimpleDataAt( i, Type::SHORT ) ); }
	long							getLongAt( int i ) const												{ return *reinterpret_cast< const long* >( getSimpleDataAt( i, Type::LONG ) ); }
	long long						getLLongAt( int i ) const												{ return *reinterpret_cast< const long long* >( getSimpleDataAt( i, Type::LLONG ) ); }
	unsigned char					getByteAt( int i ) const												{ return *reinterpret_cast< const unsigned char* >( getSimpleDataAt( i, Type::CHAR ) ); }
	unsigned short					getWordAt( int i ) const												{ return *reinterpret_cast< const unsigned short* >( getSimpleDataAt( i, Type::SHORT ) ); }
	unsigned long					getDWordAt( int i ) const												{ return *reinterpret_cast< const unsigned long* >( getSimpleDataAt( i, Type::LONG ) ); }
	unsigned long long				getQWordAt( int i ) const												{ return *reinterpret_cast< const unsigned long long* >( getSimpleDataAt( i, Type::LLONG ) ); }
	float							getFloatAt( int i ) const												{ return *reinterpret_cast< const float* >( getSimpleDataAt( i, Type::FLOAT ) ); }
	double							getDoubleAt( int i ) const												{ return *reinterpret_cast< const double* >( getSimpleDataAt( i, Type::DOUBLE ) ); }
	const char*						getStringAt( int i ) const												{ return getStringValue( *reinterpret_cast< const long* >( getSimpleDataAt( i, Type::STRING ) ) ); }
	const wchar_t*					getWStringAt( int i ) const												{ return getWStringValue( *reinterpret_cast< const long* >( getSimpleDataAt( i, Type::WSTRING ) ) ); }

	char							getCharBy( const char* pName ) const									{ return getCharAt( indexBy( pName ) ); }
	short							getShortBy( const char* pName ) const									{ return getShortAt( indexBy( pName ) ); }
	long							getLongBy( const char* pName ) const									{ return getLongAt( indexBy( pName ) ); }
	long long						getLLongBy( const char* pName ) const									{ return getLLongAt( indexBy( pName ) ); }
	unsigned char					getByteBy( const char* pName ) const									{ return getByteAt( indexBy( pName ) ); }
	unsigned short					getWordBy( const char* pName ) const									{ return getWordAt( indexBy( pName ) ); }
	unsigned long					getDWordBy( const char* pName ) const									{ return getDWordAt( indexBy( pName ) ); }
	unsigned long long				getQWordBy( const char* pName ) const									{ return getQWordAt( indexBy( pName ) ); }
	float							getFloatBy( const char* pName ) const									{ return getFloatAt( indexBy( pName ) ); }
	double							getDoubleBy( const char* pName ) const									{ return getDoubleAt( indexBy( pName ) ); }
	const char*						getStringBy( const char* pName ) const									{ return getStringAt( indexBy( pName ) ); }
	const wchar_t*					getWStringBy( const char* pName ) const									{ return getWStringAt( indexBy( pName ) ); }
	const IArray*					getArrayBy( const char* pName ) const									{ return getArrayAt( indexBy( pName ) ); }
	const ITemplate*				getTemplateBy( const char* pName ) const								{ return getTemplateAt( indexBy( pName ) ); }
	const ITemplateArray*			getTemplateArrayBy( const char* pName ) const							{ return getTemplateArrayAt( indexBy( pName ) ); }
	const IDict*					getDictBy( const char* pName ) const									{ return getDictAt( indexBy( pName ) ); }

	bool							setCharAt( int i, char value )											{ return setSimpleDataAt( i, &value, Type::CHAR ); }
	bool							setShortAt( int i, short value )										{ return setSimpleDataAt( i, &value, Type::SHORT ); }
	bool							setLongAt( int i, long value )											{ return setSimpleDataAt( i, &value, Type::LONG ); }
	bool							setLLongAt( int i, long long value )									{ return setSimpleDataAt( i, &value, Type::LLONG ); }
	bool							setByteAt( int i, unsigned char value )									{ return setSimpleDataAt( i, &value, Type::CHAR ); }
	bool							setWordAt( int i, unsigned short value )								{ return setSimpleDataAt( i, &value, Type::SHORT ); }
	bool							setDWordAt( int i, unsigned long value )								{ return setSimpleDataAt( i, &value, Type::LONG ); }
	bool							setQWordAt( int i, unsigned long long value )							{ return setSimpleDataAt( i, &value, Type::LLONG ); }
	bool							setFloatAt( int i, float value )										{ return setSimpleDataAt( i, &value, Type::FLOAT ); }
	bool							setDoubleAt( int i, double value )										{ return setSimpleDataAt( i, &value, Type::DOUBLE ); }
	bool							setStringAt( int i, const char* pValue )								{ long v = addStringValue( pValue); return setSimpleDataAt( i, &v, Type::STRING ); }
	bool							setWStringAt( int i, const wchar_t* pValue )							{ long v = addWStringValue( pValue); return setSimpleDataAt( i, &v, Type::WSTRING ); }

	bool							setCharBy( const char* pName, char value )								{ return setCharAt( indexBy( pName ), value ); }
	bool							setShortBy( const char* pName, short value )							{ return setShortAt( indexBy( pName ), value ); }
	bool							setLongBy( const char* pName, long value )								{ return setLongAt( indexBy( pName ), value ); }
	bool							setLLongBy( const char* pName, long long value )						{ return setLLongAt( indexBy( pName ), value ); }
	bool							setByteBy( const char* pName, unsigned char value )						{ return setByteAt( indexBy( pName ), value ); }
	bool							setWordBy( const char* pName, unsigned short value )					{ return setWordAt( indexBy( pName ), value ); }
	bool							setDWordBy( const char* pName, unsigned long value )					{ return setDWordAt( indexBy( pName ), value ); }
	bool							setQWordBy( const char* pName, unsigned long long value )				{ return setQWordAt( indexBy( pName ), value ); }
	bool							setFloatBy( const char* pName, float value )							{ return setFloatAt( indexBy( pName ), value ); }
	bool							setDoubleBy( const char* pName, double value )							{ return setDoubleAt( indexBy( pName ), value ); }
	bool							setStringBy( const char* pName, const char* pValue )					{ return setStringAt( indexBy( pName ), pValue ); }
	bool							setWStringBy( const char* pName, const wchar_t* pValue )				{ return setWStringAt( indexBy( pName ), pValue ); }
	IArray*							arrayBy( const char* pName )											{ return arrayAt( indexBy( pName ) ); }
	ITemplate*						templateBy( const char* pName )											{ return templateAt( indexBy( pName ) ); }
	ITemplateArray*					templateArrayBy( const char* pName )									{ return templateArrayAt( indexBy( pName ) ); }
	IDict*							dictBy( const char* pName )												{ return dictAt( indexBy( pName ) ); }

protected:
	const ATInfo* m_pATI;
};



class ITemplateArray 
{
public:

	// element class info & count

	virtual int						count() const															= 0;

	const TemplateInfo*				info() const															{ return m_pATI->pInfo; }
	const GUID&						classId() const 														{ return m_pATI->pInfo->id(); }
	const char*						className() const 														{ return m_pATI->pInfo->name(); }
	int								fieldCount() const														{ return m_pATI->fieldLimit; }
	int								fixedSize() const														{ return m_pATI->fixedSize; }
	bool							isFixed() const															{ return fixedSize() >= 0; }

	// data read interface

	virtual int						readFixedData( int i, int n, void* pOutBuf, int bufSize = 0 ) const		= 0;
	int								readFixedData( void* pOutBuf, int bufSize = 0 )	const					{ return readFixedData( 0, count(), pOutBuf, bufSize ); }

	virtual const ITemplate*		getTemplateAt( int i ) const											= 0;

	// data build interface

	virtual void					clear()																	= 0;
	virtual ITemplate*				addNewTemplate()														= 0;

	virtual ITemplate*				templateAt( int i )														= 0;

protected:
	const ATInfo* m_pATI;
};



class IArray 
{
public:

	// element info & count

	virtual int						count() const															= 0;

	virtual int						type() const															= 0;
	int								fixedSize() const														{ return Type::getTypeSize( type() ); }
	bool							isFixed() const															{ return true; }

	// data read interface

	virtual int						readFixedData( int i, int n, void* pOutBuf, int bufSize = 0 ) const		= 0;
	int								readFixedData( void* pOutBuf, int bufSize = 0 )	const					{ return readFixedData( 0, count(), pOutBuf, bufSize ); }

	virtual const void*				getSimpleDataArray( int start, int type ) const							= 0;

	// data build interface

	virtual void					clear()																	= 0;

	virtual bool					addSimpleDataArray( const void* pData, int type, int count = 1 )		= 0;

	virtual bool					setSimpleDataAt( int i, const void* pData, int type )					= 0;

	// string data & index conversion

	virtual const char*				getStringValue( long strIndex ) const									= 0;
	virtual const wchar_t*			getWStringValue( long wstrIndex ) const									= 0;

	virtual long					addStringValue( const char* pStrValue )									= 0;
	virtual long					addWStringValue( const wchar_t* pWStrValue )							= 0;

	// convenience wrappers

	char							getCharAt( int i ) const												{ return *reinterpret_cast< const char* >( getSimpleDataArray( i, Type::CHAR ) ); }
	short							getShortAt( int i ) const												{ return *reinterpret_cast< const short* >( getSimpleDataArray( i, Type::SHORT ) ); }
	long							getLongAt( int i ) const												{ return *reinterpret_cast< const long* >( getSimpleDataArray( i, Type::LONG ) ); }
	long long						getLLongAt( int i ) const												{ return *reinterpret_cast< const long long* >( getSimpleDataArray( i, Type::LLONG ) ); }
	unsigned char					getByteAt( int i ) const												{ return *reinterpret_cast< const unsigned char* >( getSimpleDataArray( i, Type::CHAR ) ); }
	unsigned short					getWordAt( int i ) const												{ return *reinterpret_cast< const unsigned short* >( getSimpleDataArray( i, Type::SHORT ) ); }
	unsigned long					getDWordAt( int i ) const												{ return *reinterpret_cast< const unsigned long* >( getSimpleDataArray( i, Type::LONG ) ); }
	unsigned long long				getQWordAt( int i ) const												{ return *reinterpret_cast< const unsigned long long* >( getSimpleDataArray( i, Type::LLONG ) ); }
	float							getFloatAt( int i ) const												{ return *reinterpret_cast< const float* >( getSimpleDataArray( i, Type::FLOAT ) ); }
	double							getDoubleAt( int i ) const												{ return *reinterpret_cast< const double* >( getSimpleDataArray( i, Type::DOUBLE ) ); }
	const char*						getStringAt( int i ) const												{ return getStringValue( *reinterpret_cast< const long* >( getSimpleDataArray( i, Type::STRING ) ) ); }
	const wchar_t*					getWStringAt( int i ) const												{ return getWStringValue( *reinterpret_cast< const long* >( getSimpleDataArray( i, Type::WSTRING ) ) ); }

	const char*						getCharArray( int start = 0 ) const										{ return reinterpret_cast< const char* >( getSimpleDataArray( start, Type::CHAR ) ); }
	const short*					getShortArray( int start = 0 ) const									{ return reinterpret_cast< const short* >( getSimpleDataArray( start, Type::SHORT ) ); }
	const long*						getLongArray( int start = 0 ) const										{ return reinterpret_cast< const long* >( getSimpleDataArray( start, Type::LONG ) ); }
	const long long*				getLLongArray( int start = 0 ) const									{ return reinterpret_cast< const long long* >( getSimpleDataArray( start, Type::LLONG ) ); }
	const unsigned char*			getByteArray( int start = 0 ) const										{ return reinterpret_cast< const unsigned char* >( getSimpleDataArray( start, Type::CHAR ) ); }
	const unsigned short*			getWordArray( int start = 0 ) const										{ return reinterpret_cast< const unsigned short* >( getSimpleDataArray( start, Type::SHORT ) ); }
	const unsigned long*			getDWordArray( int start = 0 ) const									{ return reinterpret_cast< const unsigned long* >( getSimpleDataArray( start, Type::LONG ) ); }
	const unsigned long long*		getQWordArray( int start = 0 ) const									{ return reinterpret_cast< const unsigned long long* >( getSimpleDataArray( start, Type::LLONG ) ); }
	const float*					getFloatArray( int start = 0 ) const									{ return reinterpret_cast< const float* >( getSimpleDataArray( start, Type::FLOAT ) ); }
	const double*					getDoubleArray( int start = 0 ) const									{ return reinterpret_cast< const double* >( getSimpleDataArray( start, Type::DOUBLE ) ); }

	bool							addChar( char value )													{ return addSimpleDataArray( &value, Type::CHAR ); }
	bool							addShort( short value )													{ return addSimpleDataArray( &value, Type::SHORT ); }
	bool							addLong( long value )													{ return addSimpleDataArray( &value, Type::LONG ); }
	bool							addLLong( long long value )												{ return addSimpleDataArray( &value, Type::LLONG ); }
	bool							addByte( unsigned char value )											{ return addSimpleDataArray( &value, Type::CHAR ); }
	bool							addWord( unsigned short value )											{ return addSimpleDataArray( &value, Type::SHORT ); }
	bool							addDWord( unsigned long value )											{ return addSimpleDataArray( &value, Type::LONG ); }
	bool							addQWord( unsigned long long value )									{ return addSimpleDataArray( &value, Type::LLONG ); }
	bool							addFloat( float value )													{ return addSimpleDataArray( &value, Type::FLOAT ); }
	bool							addDouble( double value )												{ return addSimpleDataArray( &value, Type::DOUBLE ); }
	bool							addString( const char* pValue )											{ long v = addStringValue( pValue); return addSimpleDataArray( &v, Type::STRING ); }
	bool							addWString( const wchar_t* pValue )										{ long v = addWStringValue( pValue); return addSimpleDataArray( &v, Type::WSTRING ); }

	bool							addCharArray( const char* pValue, int count )							{ return addSimpleDataArray( pValue, Type::CHAR, count ); }
	bool							addShortArray( const short* pValue, int count )							{ return addSimpleDataArray( pValue, Type::SHORT, count ); }
	bool							addLongArray( const long* pValue, int count )							{ return addSimpleDataArray( pValue, Type::LONG, count ); }
	bool							addLLongArray( const long long* pValue, int count )						{ return addSimpleDataArray( pValue, Type::LLONG, count ); }
	bool							addByteArray( const unsigned char* pValue, int count )					{ return addSimpleDataArray( pValue, Type::CHAR, count ); }
	bool							addWordArray( const unsigned short* pValue, int count )					{ return addSimpleDataArray( pValue, Type::SHORT, count ); }
	bool							addDWordArray( const unsigned long* pValue, int count )					{ return addSimpleDataArray( pValue, Type::LONG, count ); }
	bool							addQWordArray( const unsigned long long* pValue, int count )			{ return addSimpleDataArray( pValue, Type::LLONG, count ); }
	bool							addFloatArray( const float* pValue, int count )							{ return addSimpleDataArray( pValue, Type::FLOAT, count ); }
	bool							addDoubleArray( const double* pValue, int count )						{ return addSimpleDataArray( pValue, Type::DOUBLE, count ); }

	bool							setCharAt( int i, char value )											{ return setSimpleDataAt( i, &value, Type::CHAR ); }
	bool							setShortAt( int i, short value )										{ return setSimpleDataAt( i, &value, Type::SHORT ); }
	bool							setLongAt( int i, long value )											{ return setSimpleDataAt( i, &value, Type::LONG ); }
	bool							setLLongAt( int i, long long value )									{ return setSimpleDataAt( i, &value, Type::LLONG ); }
	bool							setByteAt( int i, unsigned char value )									{ return setSimpleDataAt( i, &value, Type::CHAR ); }
	bool							setWordAt( int i, unsigned short value )								{ return setSimpleDataAt( i, &value, Type::SHORT ); }
	bool							setDWordAt( int i, unsigned long value )								{ return setSimpleDataAt( i, &value, Type::LONG ); }
	bool							setQWordAt( int i, unsigned long long value )							{ return setSimpleDataAt( i, &value, Type::LLONG ); }
	bool							setFloatAt( int i, float value )										{ return setSimpleDataAt( i, &value, Type::FLOAT ); }
	bool							setDoubleAt( int i, double value )										{ return setSimpleDataAt( i, &value, Type::DOUBLE ); }
	bool							setStringAt( int i, const char* pValue )								{ long v = addStringValue( pValue); return setSimpleDataAt( i, &v, Type::STRING ); }
	bool							setWStringAt( int i, const wchar_t* pValue )							{ long v = addWStringValue( pValue); return setSimpleDataAt( i, &v, Type::WSTRING ); }
};



class IDict 
{
public:

	// field / element info

	virtual int						count()	const															= 0;

	virtual const char*				nameAt( int i ) const													= 0;
	virtual int						indexBy( const char* pName ) const										= 0;

	virtual int						typeAt( int i )	const													= 0;
	virtual int						fixedSizeAt( int i ) const												= 0;

	// data read interface

	virtual const void*				getSimpleDataAt( int i, int type ) const								= 0;

	virtual const IArray*			getArrayAt( int i ) const												= 0;
	virtual const ITemplate*		getTemplateAt( int i ) const											= 0;
	virtual const ITemplateArray*	getTemplateArrayAt( int i ) const										= 0;
	virtual const IDict*			getDictAt( int i ) const												= 0;

	// data build interface

	virtual void					clear()																	= 0;

	virtual bool					addSimpleData( const char* pName, const void* pData, int type )			= 0;

	virtual IArray*					addNewArray( const char* pName, int type )								= 0;
	virtual ITemplate*				addNewTemplate( const char* pName, const TemplateInfo* pInfo )			= 0;
	virtual ITemplateArray*			addNewTemplateArray( const char* pName, const TemplateInfo* pInfo )		= 0;
	virtual IDict*					addNewDict( const char* pName )											= 0;

	virtual bool					setSimpleDataAt( int i, const void* pData, int type )					= 0;

	virtual IArray*					arrayAt( int i )														= 0;
	virtual ITemplate*				templateAt( int i )														= 0;
	virtual ITemplateArray*			templateArrayAt( int i )												= 0;
	virtual IDict*					dictAt( int i )															= 0;

	// string data & index conversion

	virtual const char*				getStringValue( long strIndex ) const									= 0;
	virtual const wchar_t*			getWStringValue( long wstrIndex ) const									= 0;

	virtual long					addStringValue( const char* pStrValue )									= 0;
	virtual long					addWStringValue( const wchar_t* pWStrValue )							= 0;

	// convenience wrappers

	char							getCharAt( int i ) const												{ return *reinterpret_cast< const char* >( getSimpleDataAt( i, Type::CHAR ) ); }
	short							getShortAt( int i ) const												{ return *reinterpret_cast< const short* >( getSimpleDataAt( i, Type::SHORT ) ); }
	long							getLongAt( int i ) const												{ return *reinterpret_cast< const long* >( getSimpleDataAt( i, Type::LONG ) ); }
	long long						getLLongAt( int i ) const												{ return *reinterpret_cast< const long long* >( getSimpleDataAt( i, Type::LLONG ) ); }
	unsigned char					getByteAt( int i ) const												{ return *reinterpret_cast< const unsigned char* >( getSimpleDataAt( i, Type::CHAR ) ); }
	unsigned short					getWordAt( int i ) const												{ return *reinterpret_cast< const unsigned short* >( getSimpleDataAt( i, Type::SHORT ) ); }
	unsigned long					getDWordAt( int i ) const												{ return *reinterpret_cast< const unsigned long* >( getSimpleDataAt( i, Type::LONG ) ); }
	unsigned long long				getQWordAt( int i ) const												{ return *reinterpret_cast< const unsigned long long* >( getSimpleDataAt( i, Type::LLONG ) ); }
	float							getFloatAt( int i ) const												{ return *reinterpret_cast< const float* >( getSimpleDataAt( i, Type::FLOAT ) ); }
	double							getDoubleAt( int i ) const												{ return *reinterpret_cast< const double* >( getSimpleDataAt( i, Type::DOUBLE ) ); }
	const char*						getStringAt( int i ) const												{ return getStringValue( *reinterpret_cast< const long* >( getSimpleDataAt( i, Type::STRING ) ) ); }
	const wchar_t*					getWStringAt( int i ) const												{ return getWStringValue( *reinterpret_cast< const long* >( getSimpleDataAt( i, Type::WSTRING ) ) ); }

	char							getCharBy( const char* pName ) const									{ return getCharAt( indexBy( pName ) ); }
	short							getShortBy( const char* pName ) const									{ return getShortAt( indexBy( pName ) ); }
	long							getLongBy( const char* pName ) const									{ return getLongAt( indexBy( pName ) ); }
	long long						getLLongBy( const char* pName ) const									{ return getLLongAt( indexBy( pName ) ); }
	unsigned char					getByteBy( const char* pName ) const									{ return getByteAt( indexBy( pName ) ); }
	unsigned short					getWordBy( const char* pName ) const									{ return getWordAt( indexBy( pName ) ); }
	unsigned long					getDWordBy( const char* pName ) const									{ return getDWordAt( indexBy( pName ) ); }
	unsigned long long				getQWordBy( const char* pName ) const									{ return getQWordAt( indexBy( pName ) ); }
	float							getFloatBy( const char* pName ) const									{ return getFloatAt( indexBy( pName ) ); }
	double							getDoubleBy( const char* pName ) const									{ return getDoubleAt( indexBy( pName ) ); }
	const char*						getStringBy( const char* pName ) const									{ return getStringAt( indexBy( pName ) ); }
	const wchar_t*					getWStringBy( const char* pName ) const									{ return getWStringAt( indexBy( pName ) ); }
	const IArray*					getArrayBy( const char* pName ) const									{ return getArrayAt( indexBy( pName ) ); }
	const ITemplate*				getTemplateBy( const char* pName ) const								{ return getTemplateAt( indexBy( pName ) ); }
	const ITemplateArray*			getTemplateArrayBy( const char* pName ) const							{ return getTemplateArrayAt( indexBy( pName ) ); }
	const IDict*					getDictBy( const char* pName ) const									{ return getDictAt( indexBy( pName ) ); }

	bool							addChar( const char* pName, char value )								{ return addSimpleData( pName, &value, Type::CHAR ); }
	bool							addShort( const char* pName, short value )								{ return addSimpleData( pName, &value, Type::SHORT ); }
	bool							addLong( const char* pName, long value )								{ return addSimpleData( pName, &value, Type::LONG ); }
	bool							addLLong( const char* pName, long long value )							{ return addSimpleData( pName, &value, Type::LLONG ); }
	bool							addByte( const char* pName, unsigned char value )						{ return addSimpleData( pName, &value, Type::CHAR ); }
	bool							addWord( const char* pName, unsigned short value )						{ return addSimpleData( pName, &value, Type::SHORT ); }
	bool							addDWord( const char* pName, unsigned long value )						{ return addSimpleData( pName, &value, Type::LONG ); }
	bool							addQWord( const char* pName, unsigned long long value )					{ return addSimpleData( pName, &value, Type::LLONG ); }
	bool							addFloat( const char* pName, float value )								{ return addSimpleData( pName, &value, Type::FLOAT ); }
	bool							addDouble( const char* pName, double value )							{ return addSimpleData( pName, &value, Type::DOUBLE ); }
	bool							addString( const char* pName, const char* pValue )						{ long v = addStringValue( pValue); return addSimpleData( pName, &v, Type::STRING ); }
	bool							addWString( const char* pName, const wchar_t* pValue )					{ long v = addWStringValue( pValue); return addSimpleData( pName, &v, Type::WSTRING ); }

	bool							setCharAt( int i, char value )											{ return setSimpleDataAt( i, &value, Type::CHAR ); }
	bool							setShortAt( int i, short value )										{ return setSimpleDataAt( i, &value, Type::SHORT ); }
	bool							setLongAt( int i, long value )											{ return setSimpleDataAt( i, &value, Type::LONG ); }
	bool							setLLongAt( int i, long long value )									{ return setSimpleDataAt( i, &value, Type::LLONG ); }
	bool							setByteAt( int i, unsigned char value )									{ return setSimpleDataAt( i, &value, Type::CHAR ); }
	bool							setWordAt( int i, unsigned short value )								{ return setSimpleDataAt( i, &value, Type::SHORT ); }
	bool							setDWordAt( int i, unsigned long value )								{ return setSimpleDataAt( i, &value, Type::LONG ); }
	bool							setQWordAt( int i, unsigned long long value )							{ return setSimpleDataAt( i, &value, Type::LLONG ); }
	bool							setFloatAt( int i, float value )										{ return setSimpleDataAt( i, &value, Type::FLOAT ); }
	bool							setDoubleAt( int i, double value )										{ return setSimpleDataAt( i, &value, Type::DOUBLE ); }
	bool							setStringAt( int i, const char* pValue )								{ long v = addStringValue( pValue); return setSimpleDataAt( i, &v, Type::STRING ); }
	bool							setWStringAt( int i, const wchar_t* pValue )							{ long v = addWStringValue( pValue); return setSimpleDataAt( i, &v, Type::WSTRING ); }

	bool							setCharBy( const char* pName, char value )								{ return setCharAt( indexBy( pName ), value ); }
	bool							setShortBy( const char* pName, short value )							{ return setShortAt( indexBy( pName ), value ); }
	bool							setLongBy( const char* pName, long value )								{ return setLongAt( indexBy( pName ), value ); }
	bool							setLLongBy( const char* pName, long long value )						{ return setLLongAt( indexBy( pName ), value ); }
	bool							setByteBy( const char* pName, unsigned char value )						{ return setByteAt( indexBy( pName ), value ); }
	bool							setWordBy( const char* pName, unsigned short value )					{ return setWordAt( indexBy( pName ), value ); }
	bool							setDWordBy( const char* pName, unsigned long value )					{ return setDWordAt( indexBy( pName ), value ); }
	bool							setQWordBy( const char* pName, unsigned long long value )				{ return setQWordAt( indexBy( pName ), value ); }
	bool							setFloatBy( const char* pName, float value )							{ return setFloatAt( indexBy( pName ), value ); }
	bool							setDoubleBy( const char* pName, double value )							{ return setDoubleAt( indexBy( pName ), value ); }
	bool							setStringBy( const char* pName, const char* pValue )					{ return setStringAt( indexBy( pName ), pValue ); }
	bool							setWStringBy( const char* pName, const wchar_t* pValue )				{ return setWStringAt( indexBy( pName ), pValue ); }
	IArray*							arrayBy( const char* pName )											{ return arrayAt( indexBy( pName ) ); }
	ITemplate*						templateBy( const char* pName )											{ return templateAt( indexBy( pName ) ); }
	ITemplateArray*					templateArrayBy( const char* pName )									{ return templateArrayAt( indexBy( pName ) ); }
	IDict*							dictBy( const char* pName )												{ return dictAt( indexBy( pName ) ); }
};

} // namespace trf