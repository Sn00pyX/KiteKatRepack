#pragma once

#include <map>
#include <string>

#include "safe_function.h"

struct property_tag
{
	int type;
	int offset;
	size_t data_len;
};

template< typename KEY >
struct XPropertySetBase
{
private:

	typedef std::map< KEY, property_tag >	PROPERTY_MAP;

public:

	typedef KEY key_type;

	enum {
		PROPERTY_TYPE_NIL	= -1,
		PROPERTY_TYPE_BOOL	= 0,
		PROPERTY_TYPE_UCHAR,
		PROPERTY_TYPE_SHORT,
		PROPERTY_TYPE_INT,			
		PROPERTY_TYPE_UNSIGNED_INT,	
		PROPERTY_TYPE_INT64,
		PROPERTY_TYPE_SIZE,
		PROPERTY_TYPE_FLOAT,
		PROPERTY_TYPE_DOUBLE,
		PROPERTY_TYPE_STRING,
		PROPERTY_TYPE_CSTRING,
	};

	void BindCString( const KEY & key, const char* data, size_t data_len )	{ _bind( key, PROPERTY_TYPE_CSTRING, (int)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), data_len ); }
	void Bind( const KEY & key, bool * data )			{ _bind( key, PROPERTY_TYPE_BOOL, (int)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), sizeof( bool ) ); }
	void Bind( const KEY & key, unsigned char * data )	{ _bind( key, PROPERTY_TYPE_UCHAR, (int)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), sizeof( unsigned char ) ); }
	void Bind( const KEY & key, short * data )			{ _bind( key, PROPERTY_TYPE_SHORT, (int)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), sizeof( short ) ); }
	void Bind( const KEY & key, int * data )			{ _bind( key, PROPERTY_TYPE_INT, (int)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), sizeof( int ) ); }
	void Bind( const KEY & key, size_t * data )			{ _bind( key, PROPERTY_TYPE_SIZE, (size_t)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), sizeof( size_t ) ); }
	void Bind( const KEY & key, __int64 * data )		{ _bind( key, PROPERTY_TYPE_INT64, (int)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), sizeof( __int64 ) ); }
	void Bind( const KEY & key, std::string * data )	{ _bind( key, PROPERTY_TYPE_STRING, (int)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), 0 ); }
	void Bind( const KEY & key, float * data )			{ _bind( key, PROPERTY_TYPE_FLOAT, (int)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), sizeof( float ) ); }
	void Bind( const KEY & key, double * data )			{ _bind( key, PROPERTY_TYPE_DOUBLE, (int)( reinterpret_cast< const char* >( data ) - reinterpret_cast< const char* >( this ) ), sizeof( double ) ); }

	int			GetPropertyType( const KEY & key )
	{
		PROPERTY_MAP * map = getPropertyMap();
		if ( !map ) return PROPERTY_TYPE_NIL;
		PROPERTY_MAP::iterator it = map->find( key );	
		if( it == map->end() ) return PROPERTY_TYPE_NIL;
		return it->second.type;
	}
	
	float		GetFloat( const KEY & key )				{ return *reinterpret_cast< float * >( getOffset( key, PROPERTY_TYPE_FLOAT ) );			}
	float		GetDouble( const KEY & key )			{ return *reinterpret_cast< float * >( getOffset( key, PROPERTY_TYPE_DOUBLE ) );			}
	std::string	GetString( const KEY & key )			{ return *reinterpret_cast< std::string * >( getOffset( key, PROPERTY_TYPE_STRING ) );	}
	int			GetInt( const KEY & key )				{ return *reinterpret_cast< int * >( getOffset( key, PROPERTY_TYPE_INT ) );				}
	int			GetSize( const KEY & key )				{ return *reinterpret_cast< size_t * >( getOffset( key, PROPERTY_TYPE_INT ) );				}
	short		GetShort( const KEY & key )				{ return *reinterpret_cast< short * >( getOffset( key, PROPERTY_TYPE_SHORT ) );			}
	char		GetChar( const KEY & key )				{ return *reinterpret_cast< unsigned char * >( getOffset( key, PROPERTY_TYPE_UCHAR ) );	}
	bool		GetBool( const KEY & key )				{ return *reinterpret_cast< bool * >( getOffset( key, PROPERTY_TYPE_BOOL ) );				}
	__int64		GetInt64( const KEY & key )				{ return *reinterpret_cast< __int64 * >( getOffset( key, PROPERTY_TYPE_INT64 ) );			}
	const char* GetCString( const KEY & key )			{ return reinterpret_cast< const char * >( getOffset( key, PROPERTY_TYPE_CSTRING ) );		}
	
	std::string		GetAsString( const KEY & key, const char *szDefault = "" )
	{	
		PROPERTY_MAP * map = getPropertyMap();
		if ( !map ) return szDefault;

		PROPERTY_MAP::iterator it = map->find( key );	

		if ( it == map->end() ) return onGetEmptyKey( key, szDefault );

		char *pData = reinterpret_cast< char * >( this );
		pData += it->second.offset;
		
		char temp[1024];
		char *pOutput = temp;

		switch( it->second.type )
		{
			case PROPERTY_TYPE_UCHAR:		s_sprintf( temp, _countof( temp ), "%u", *reinterpret_cast< unsigned char* >( pData ) );			break;
			case PROPERTY_TYPE_BOOL:		s_sprintf( temp, _countof( temp ), "%d", *reinterpret_cast< bool* >( pData ) );			break;
			case PROPERTY_TYPE_SHORT:		s_sprintf( temp, _countof( temp ), "%d", *reinterpret_cast< short* >( pData ) );		break;
			case PROPERTY_TYPE_INT:			s_sprintf( temp, _countof( temp ), "%d", *reinterpret_cast< int* >( pData ) );			break;
			case PROPERTY_TYPE_SIZE:		s_sprintf( temp, _countof( temp ), "%u", *reinterpret_cast< size_t* >( pData ) );			break;
			case PROPERTY_TYPE_INT64:		s_sprintf( temp, _countof( temp ), "%I64d", *reinterpret_cast< __int64* >( pData ) );	break;
			case PROPERTY_TYPE_FLOAT:		s_sprintf( temp, _countof( temp ), "%f", *reinterpret_cast< float* >( pData ) );		break;
			case PROPERTY_TYPE_DOUBLE:		s_sprintf( temp, _countof( temp ), "%f", *reinterpret_cast< double* >( pData ) );		break;
			case PROPERTY_TYPE_STRING:	return *reinterpret_cast< std::string * >( pData );
			case PROPERTY_TYPE_CSTRING:	return pData; 
			default: return "<INVALID_TYPE>";
		}

		return pOutput;
	}
	bool SetAsString( const KEY & key, const char * data )
	{
		onBeforeChangeProperty( key, data );

		PROPERTY_MAP * map = getPropertyMap();

		if ( !map ) return false;

		PROPERTY_MAP::iterator it = map->find( key );

		bool bIsValidOperation = true;
		bool bKeyExist = true;

		if ( !isValidChange( key, data ) ) return false;

		// 없다면 KIN
		if ( it == map->end() ) 
		{
			bKeyExist = false;
			bIsValidOperation = onSetEmptyKey( key, data );
		}

		if ( bKeyExist )
		{
			char *pData = reinterpret_cast< char * >( this );
			pData += it->second.offset;
			size_t dat_len = it->second.data_len;

			switch( it->second.type )
			{
				case PROPERTY_TYPE_BOOL:	*reinterpret_cast< bool* >( pData ) = !!atoi( data );			break;
				case PROPERTY_TYPE_UCHAR:	*reinterpret_cast< unsigned char* >( pData ) = atoi( data );	break;
				case PROPERTY_TYPE_SHORT:	*reinterpret_cast< short* >( pData ) = atoi( data );			break;
				case PROPERTY_TYPE_INT:		*reinterpret_cast< int* >( pData ) = atoi( data );				break;
				case PROPERTY_TYPE_SIZE:	*reinterpret_cast< size_t* >( pData ) = atoi( data );			break;
				case PROPERTY_TYPE_INT64:	*reinterpret_cast< __int64* >( pData ) = _atoi64( data );		break;
				case PROPERTY_TYPE_FLOAT:	*reinterpret_cast< float* >( pData ) = (float)atof( data );		break;
				case PROPERTY_TYPE_DOUBLE:	*reinterpret_cast< double* >( pData ) = (double)atof( data );	break;
				case PROPERTY_TYPE_STRING:	*reinterpret_cast< std::string * >( pData ) = data;				break;
				case PROPERTY_TYPE_CSTRING:	s_strcpy( pData, dat_len, data );								break;
				default: break;
			}
		}

		onChangeProperty( key, data );

		return bIsValidOperation;
	}

	std::string		EnumAllProperty()
	{
		std::string strResult;
		PROPERTY_MAP * map = getPropertyMap();

		if ( !map ) return strResult;
	
		for ( PROPERTY_MAP::iterator it = map->begin(); it != map->end(); ++it )
		{
			strResult += it->first;
			strResult += " : ";
			strResult += GetAsString( it->first.c_str() );
			strResult += "\n";
		}

		return strResult;
	}

protected:
	// 만약 RTTI 를 사용한다면 이런식으로 구현하지 않아도 된다.
	virtual const char* getClassName()	= 0;

	virtual std::string	onGetEmptyKey( const KEY & key, const char *szDefault = "" )						{ return szDefault; }
	virtual bool		onSetEmptyKey( const KEY & key, const char *data )	{ return false; }

	virtual bool		isValidChange( const KEY & key, const char *data ) { return true; }
	virtual void		onBeforeChangeProperty( const KEY & key, const char *data ) { return; }
	virtual void		onChangeProperty( const KEY & key, const char *data ) {}

private:

	static std::map< std::string, typename XPropertySetBase::PROPERTY_MAP > & XPropertySetBase::getTypeMap()
	{
		static std::map< std::string, XPropertySetBase< KEY >::PROPERTY_MAP > _inst;
		return _inst;
	}

	PROPERTY_MAP *	getPropertyMap()
	{
		// 해당 타입에 대한 맵을 얻는다
		std::map< std::string, PROPERTY_MAP >::iterator it;
		it = getTypeMap().find( getClassName() );
		
		// 존재하지 않는 타입이면 KIN
		if ( it == getTypeMap().end() ) return NULL;

		return &it->second;
	}

	char* getOffset( const KEY & key, int type )
	{
		// property 가 없을 경우 돌려줄 것들
		static std::string strTmp( "<NULL>" );
		static char szTmp[32] = {0,};

		PROPERTY_MAP * map = getPropertyMap();
		PROPERTY_MAP::iterator it = map->find( key );

		// 타입 불일치라면 KIN
		if ( !map || it == map->end() || it->second.type != type ) 
		{
			if ( type == PROPERTY_TYPE_STRING ) return reinterpret_cast< char* >( &strTmp );
			else if ( type == PROPERTY_TYPE_CSTRING ) return "<NULL>";
			else return szTmp;
		}
		
		char *pData = reinterpret_cast< char * >( this );
		pData += it->second.offset;

		return pData;
	}

	inline void _bind( const KEY& key, int type, int offset, size_t data_len )
	{
		property_tag tag;
		tag.type = type;
		tag.offset = offset;
		tag.data_len = data_len;
		(getTypeMap()[ getClassName() ])[ key ] = tag;
	}

};

struct XPropertySet : public XPropertySetBase< std::string >
{
};
