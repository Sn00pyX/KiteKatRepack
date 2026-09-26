
#include <algorithm>
#include <vector>
#include <map>
#include <set>

#include "../../include/toolkit/XEnv.h"
#include "../../include/toolkit/nsl.h"
#include "../../include/toolkit/ILock.h"
#include "../../include/cipher/XEncrypt.h"
#include "../../include/toolkit/safe_function.h"


const char * XEnvStruct::ENV_FILE_FORMAT_TAG = "EV";

struct TENV
{
	TENV( const char* _szKey, const char* _szData )
	{
		init( _szKey );
		SetData( _szData );
	}

	TENV( const char* _szKey, int _nNumber )
	{
		init( _szKey );
		SetData( _nNumber );
	}

	TENV( const char* _szKey, int* _pNumber )
	{
		init( _szKey );
		Bind( _pNumber );
	}

	TENV( const char* _szKey, short* _pNumber )
	{
		init( _szKey );
		Bind( _pNumber );
	}

	TENV( const char* _szKey, float _fNumber )
	{
		init( _szKey );
		SetData( _fNumber );
	}

	TENV( const char* _szKey, float* _pNumber )
	{
		init( _szKey );
		Bind( _pNumber );
	}

	TENV( const char* _szKey, const XEnvStruct::Vector& v )
	{
		init( _szKey );
		SetData( v );
	}

	TENV( const char* _szKey, const XEnvStruct::Quaternion& q )
	{
		init( _szKey );
		SetData( q );
	}

	TENV( const char* _szKey, XEnvStruct::Vector* pVector )
	{
		init( _szKey );
		Bind( pVector );
	}

	TENV( const char* _szKey, XEnvStruct::Quaternion* pQuaternion )
	{
		init( _szKey );
		Bind( pQuaternion );
	}

	TENV( const char* _szKey, bool* _pBoolean )
	{
		init( _szKey );
		Bind( _pBoolean );
	}

	TENV( const char* _szKey, int (*_nfn)() )
	{
		init( _szKey );
		Bind( _nfn );
	}

	TENV( const char* _szKey, std::string& (*_sfn)() )
	{
		init( _szKey );
		Bind( _sfn );
	}

	TENV( const char* _szKey, std::string* str )
	{
		init( _szKey );
		Bind( str );
	}

	TENV( const char* _szKey )
	{
		init( _szKey );
	}

	~TENV()
	{
		if ( pChildList )
		{
			for ( std::set< TENV*, TENV::TENVLESS >::iterator it = pChildList->begin(); it != pChildList->end(); ++it )
			{
				delete* it;
			}
			delete pChildList;
		}

		if ( env_type == STRING && bind_type == NONE ) delete strData;
	}

	inline unsigned DoEachData( XEnvStruct::XEnvFunctor& fo, const char* szMask, bool bTranslateDataToString = false )
	{
		unsigned cnt = 0;
		if ( env_type != CONTAINER )
		{
			if ( nsl::wildcmp( szMask, strFullKey.c_str(), true ) )
			{
				if ( bTranslateDataToString ) 
				{
					fo.onString( strFullKey, GetString() );
				}
				else
				{
					if ( env_type == INTEGER ) fo.onInteger( strFullKey, GetInteger() );
					else if ( env_type == SHORT ) fo.onShort( strFullKey, static_cast< short >( GetInteger() ) );
					else if ( env_type == FLOAT ) fo.onFloat( strFullKey, GetFloat() );
					else if ( env_type == VECTOR ) fo.onVector( strKey, GetVector() );
					else if ( env_type == QUATERNION ) fo.onQuaternion( strFullKey, GetQuaternion() );
					else fo.onString( strFullKey, GetString() );
				}

				cnt++;
			}
		}

		if ( pChildList )
		{
			for ( std::set< TENV*, TENV::TENVLESS >::iterator it = pChildList->begin(); it != pChildList->end(); ++it )
			{
				cnt += (*it)->DoEachData( fo, szMask, bTranslateDataToString );
			}
		}

		return cnt;
	}

	inline void RemoveChildNode( TENV* p )
	{
		pChildList->erase( p );
	}

	inline void SetData( const char* szData )
	{
		if ( env_type != NONE )
		{
			if ( env_type == VECTOR )
			{
				XEnvStruct::Vector v;
				sscanf_s( szData, "%f;%f;%f", &v.x, &v.y, &v.z );
				SetData( v );
				return;
			}

			if ( env_type == QUATERNION )
			{
				XEnvStruct::Quaternion q;
				sscanf_s( szData, "%f;%f;%f;%f", &q.x, &q.y, &q.z, &q.w );
				SetData( q );
				return;
			}

			if ( env_type == SHORT || env_type == INTEGER || env_type == BOOLEAN )
			{
				SetData( atoi( szData ) );
				return;
			}

			if ( env_type == FLOAT )
			{
				SetData( (float)atof( szData ) );
				return;
			}
		}

		if ( !strData ) 
		{
			strData = new std::string( szData );
			bind_type = NONE;
		}
		else 
		{
			*strData = szData;
		}

		env_type = STRING;
	}

	inline void SetData( float fNumber )
	{
		if ( bind_type != FUNCTION && env_type == STRING )
		{
			nsl::sprintf_str(* strData, "%f", fNumber );
			return;
		}

		if ( bind_type == VARIABLE && env_type == FLOAT )* pFloat = fNumber;
		else fFloat[0] = fNumber;
		env_type = FLOAT;
	}

	inline void SetData( const XEnvStruct::Vector& v )
	{
		if ( bind_type != FUNCTION && env_type == STRING )
		{
			nsl::sprintf_str(* strData, "%f;%f;%f", v.x, v.y, v.z );
			return;
		}

		if ( bind_type == VARIABLE && env_type == VECTOR )
		{
			pQuaternion->x = v.x;
			pQuaternion->y = v.y;
			pQuaternion->z = v.z;
		}
		
		if ( bind_type == NONE )
		{
			fFloat[0] = v.x;
			fFloat[1] = v.y;
			fFloat[2] = v.z;
			env_type = VECTOR;
		}	
	}

	inline void SetData( const XEnvStruct::Quaternion& q )
	{
		if ( bind_type != FUNCTION && env_type == STRING )
		{
			nsl::sprintf_str(* strData, "%f;%f;%f;%f", q.x, q.y, q.z, q.w );
			return;
		}

		if ( bind_type == VARIABLE && env_type == QUATERNION )
		{
			pQuaternion->x = q.x;
			pQuaternion->y = q.y;
			pQuaternion->z = q.z;
			pQuaternion->w = q.w;
		}
		
		if ( bind_type == NONE )
		{
			fFloat[0] = q.x;
			fFloat[1] = q.y;
			fFloat[2] = q.z;
			env_type = QUATERNION;
		}		
	}

	inline void SetData( int nNumber )
	{
		if ( bind_type != FUNCTION && env_type == STRING )
		{
			nsl::sprintf_str(* strData, "%d", nNumber );
			return;
		}

		if ( bind_type == VARIABLE )
		{
			if ( env_type == BOOLEAN )
			{	
				if( nNumber == 0 )
				{
					*pBoolean = false;
				}
				else
				{
					*pBoolean = true;
				}
			}
			else if ( env_type == SHORT )
			{
				*pShort = static_cast< short >( nNumber );
			}
			else if ( env_type == INTEGER )
			{
				*pInteger = nNumber;
			}
		}
		else if ( bind_type == NONE )
		{
			nInteger = nNumber;
			env_type = INTEGER;
		}			
	}

	inline float GetFloat() const
	{
		if ( bind_type == VARIABLE && env_type == FLOAT )
			return* pFloat;
		if ( bind_type == NONE && env_type == FLOAT )
			return fFloat[0];
		if ( env_type == STRING )
			return (float)atof( GetString().c_str() );

		return (float)nInteger;
	}

	inline bool GetBoolean() const
	{
		if ( bind_type == VARIABLE && env_type == BOOLEAN ) return* pBoolean;
		return !!(int)nInteger;
	}

	inline XEnvStruct::Vector GetVector() const
	{
		if ( env_type == STRING && strData )
		{
			XEnvStruct::Vector v;
			sscanf_s( strData->c_str(), "%f;%f;%f", &v.x, &v.y, &v.z );
			return v;
		}

		if ( bind_type == VARIABLE && env_type == VECTOR ) return XEnvStruct::Vector( pVector->x, pVector->y, pVector->z );

		return XEnvStruct::Vector( fFloat[0], fFloat[1], fFloat[2] );
	}

	inline XEnvStruct::Quaternion GetQuaternion() const
	{
		if ( env_type == STRING && strData )
		{
			XEnvStruct::Quaternion q;
			sscanf_s( strData->c_str(), "%f;%f;%f;%f", &q.x, &q.y, &q.z, &q.w );
			return q;
		}

		if ( bind_type == VARIABLE && env_type == QUATERNION ) return XEnvStruct::Quaternion( pQuaternion->x, pQuaternion->y, pQuaternion->z, pQuaternion->w );

		return XEnvStruct::Quaternion( fFloat[0], fFloat[1], fFloat[2], fFloat[3] );
	}

	inline int GetInteger() const
	{
		if ( bind_type == NONE ) return nInteger;
		
		if ( bind_type == FUNCTION )
		{
			if ( env_type == INTEGER && nfn ) return nfn();
		}

		if ( bind_type == VARIABLE )
		{
			if ( env_type == BOOLEAN ) return* pBoolean;
			if ( env_type == INTEGER ) return* pInteger;
			if ( env_type == SHORT ) return* pShort;
		}

		return 0;
	}

	inline void Bind( int* pNumber )
	{
		env_type = INTEGER;
		bind_type = VARIABLE;
		pInteger = pNumber;
	}

	inline void Bind( short* pNumber )
	{
		env_type = SHORT;
		bind_type = VARIABLE;
		pShort = pNumber;
	}

	inline void Bind( XEnvStruct::Quaternion* _pQuaternion )
	{
		env_type = QUATERNION;
		bind_type = VARIABLE;
		pQuaternion = _pQuaternion;
	}

	inline void Bind( XEnvStruct::Vector* pfVector )
	{
		env_type = VECTOR;
		bind_type = VARIABLE;
		pVector = pfVector;
	}

	inline void Bind( float* pNumber )
	{
		env_type = FLOAT;
		bind_type = VARIABLE;
		pFloat = pNumber;
	}

	inline void Bind( bool* _pBoolean )
	{
		env_type = BOOLEAN;
		bind_type = VARIABLE;
		pBoolean = _pBoolean;
	}

	inline void Bind( int (*_fn)() )
	{
		env_type = INTEGER;
		bind_type = FUNCTION;
		nfn = _fn;
	}

	inline void Bind( std::string& (*_fn)() )
	{
		env_type = STRING;
		bind_type = FUNCTION;
		sfn = _fn;
	}

	inline void Bind( std::string* str )
	{
		env_type = STRING;
		bind_type = VARIABLE;
		strData = str;
	}

	inline void SetReadOnly()
	{
		read_only = true;
	}

	inline bool IsReadOnly() { return read_only; }

	inline const std::string &	GetKey() const					{ return strKey; }
	inline void					SetKey( const char* szKey )
	{
		strKey = szKey;
	}
	
	
	inline std::string 		GetString() const
	{
		if ( IsContainer() ) return "<NULL>";

		if ( env_type != STRING ) 
		{
			if ( IsNumber() )
			{
				std::string buf;
				nsl::sprintf_str( buf, "%d", GetInteger() );
				return buf;
			}
			if ( IsVector() )
			{
				std::string buf;
				XEnvStruct::Vector v = GetVector();
				nsl::sprintf_str( buf, "%f;%f;%f", v.x, v.y, v.z );
				return buf;
			}
			if ( IsQuaternion() )
			{
				std::string buf;
				XEnvStruct::Quaternion q = GetQuaternion();
				nsl::sprintf_str( buf, "%f;%f;%f;%f", q.x, q.y, q.z, q.w );
				return buf;
			}
			if ( IsFloat() )
			{
				std::string buf;
				nsl::sprintf_str( buf, "%f", GetFloat() );
				return buf;
			}
			if ( env_type == BOOLEAN )
			{
				std::string buf;
				nsl::sprintf_str( buf, "%d", GetBoolean() ? 1 : 0 );
				return buf;
			}
		}

		if ( bind_type == FUNCTION ) return sfn();

		return* strData;
	}


	inline bool	IsString() const	{ return env_type == STRING;	}
	inline bool	IsInteger() const	{ return env_type == INTEGER;	}
	inline bool	IsShort() const		{ return env_type == SHORT;	}
	inline bool	IsNumber() const	{ return env_type == INTEGER || env_type == SHORT;	}
	inline bool	IsFloat() const		{ return env_type == FLOAT;		}
	inline bool	IsBoolean() const	{ return env_type == BOOLEAN;	}
	inline bool IsQuaternion() const{ return env_type == QUATERNION;}
	inline bool	IsVector() const	{ return env_type == VECTOR;	}
	inline bool	IsContainer() const	{ return env_type == CONTAINER;	}
	inline bool IsBinded() const	{ return ( pVector || pInteger || pShort || pFloat || pBoolean || sfn || nfn ); }

	inline void SetAsContainer()	{ env_type = CONTAINER;	}
	
	struct TENVLESS
	{
		inline bool operator()( const TENV* left, TENV* right ) const
		{
			return left->GetKey() < right->GetKey();
		}
	};

	std::set< TENV*, TENV::TENVLESS >		* pChildList;
	TENV* pParentNode;

private:

	void init( const char* _szKey )
	{
		read_only = false;
		strFullKey = _szKey;
		SetKey( _szKey );
		
		bind_type = NONE;
		strData = NULL;
		pParentNode = NULL;
		pChildList = NULL;
		nInteger = 0;
		fFloat[0] = 0;
		fFloat[1] = 0;
		fFloat[2] = 0;
		pFloat = NULL;
		pVector = NULL;
		pQuaternion = NULL;
		pInteger = NULL;
		pShort = NULL;
		pBoolean = NULL;
		env_type = NONE;
		sfn = NULL;
		nfn = NULL;		
	}

	std::string		strKey;
	std::string		strFullKey;

	union
	{
		std::string	& (*sfn)();
		int			  (*nfn)();
		bool*		  pBoolean;		
		std::string* strData;		
		int			  nInteger;
		float		  fFloat[4];
		volatile int* pInteger;
		volatile short* pShort;
		volatile float* pFloat;
		volatile XEnvStruct::Vector* pVector;
		volatile XEnvStruct::Quaternion* pQuaternion;
	};

	enum {	NONE = -1, 
			CONTAINER = 0, BOOLEAN, INTEGER, SHORT, FLOAT, VECTOR, STRING, QUATERNION,
			FUNCTION , VARIABLE };

	char		env_type;
	char		bind_type;
	bool		read_only;
};



typedef std::set< TENV*, TENV::TENVLESS > ENVMAP;

struct _XEnvStructImpl
{
	_XEnvStructImpl();
	~_XEnvStructImpl();

	template< typename T >
	void Bind( const char* szKey, T data )
	{
		TENV* pNew = find( szKey, m_pSet );
	
		if ( pNew )
		{
			pNew->Bind( data );
			onEnvUpdate(szKey);
		}
		else
		{
			insert( szKey, new TENV( szKey, data ), m_pSet );
			onEnvInsert(szKey);
		}		
	}

	void SetReadOnly( const char* szKey )
	{
		TENV* pNew = find( szKey, m_pSet );

		if ( pNew )
		{
			pNew->SetReadOnly();
		}
	}

	template< typename T >
	void Set( const char* szKey, T data )
	{
		TENV* pNew = find( szKey, m_pSet );

		if ( pNew )
		{
			if ( pNew->IsReadOnly() )
				return;

			pNew->SetData( data );
			onEnvUpdate( szKey );
		}
		else
		{
			insert( szKey, new TENV( szKey, data ), m_pSet );
			onEnvInsert( szKey );
		}		
	}

	XEnvStruct::Quaternion GetQuaternion( const char* szKey, float default_x, float default_y, float default_z, float default_w ) const;
	XEnvStruct::Vector GetVector( const char* szKey, float default_x, float default_y, float default_z ) const;
	std::string GetString( const char* szKey, const char* szDefault ) const;
	int GetInt( const char* szKey, int nData ) const;
	float GetFloat( const char* szKey, float fData ) const;

	void SetFlag( const char* szKey, int nFlagIndex, bool bFlag );
	void SetFlag( const char* szKey, const char* szFlagName, bool bFlag );
	void SetConstant( const char* szKey, const char* szName, int nFlagIndex );
	int  GetConstant( const char* szKey, const char* szName, int nDefault );
	
	bool Remove( const char* szKey );
	bool Remove( TENV* p );
	
	bool IsExist( const char* szKey ) const;
	bool IsContainer( const char* szKey ) const;
	bool IsBinded( const char* szKey ) const;

	unsigned DoEachData( XEnvStruct::XEnvFunctor& fo, const char* szMask, bool bTranslateDataToString = false );

	TENV* find( const char* szKey, TENV* pSet ) const;
	void insert( const char* szKey, TENV* pData, TENV* pSet );

	void SetEventReceiver( XEnvStruct::XEnvEventReceiver* pReceiver );

	void onEnvUpdate( const char* szKey )
	{
		if ( m_pReceiver ) m_pReceiver->onEnvUpdate( szKey );
	}

	void onEnvInsert( const char* szKey )
	{
		if ( m_pReceiver ) m_pReceiver->onEnvInsert( szKey );
	}

	XCriticalSection csLock;

private:

	_XEnvStructImpl( const _XEnvStructImpl& );
	_XEnvStructImpl& operator=( const _XEnvStructImpl& );

	XEnvStruct::XEnvEventReceiver*	m_pReceiver;
	TENV*							m_pSet;
	std::map< std::string, int >	m_mapFlagName;
};

_XEnvStructImpl::_XEnvStructImpl() : csLock( "_XEnvStructImpl" )
{
	m_pSet = new TENV( "ROOT" );
	m_pSet->SetAsContainer();
	m_pReceiver = NULL;
}

_XEnvStructImpl::~_XEnvStructImpl()
{
	delete m_pSet;
}

const char* CONSTANT_SEPARATOR = "\r\t\n";

int  _XEnvStructImpl::GetConstant( const char* szKey, const char* szName, int nDefault )
{
	std::string strKey;

	if ( szKey ) strKey += szKey;

	strKey += CONSTANT_SEPARATOR;
	strKey += szName;

	std::map< std::string, int >::iterator it = m_mapFlagName.find( strKey );
	if ( it == m_mapFlagName.end() ) return nDefault;

	return it->second;
}

void _XEnvStructImpl::SetConstant( const char* szKey, const char* szName, int nFlagIndex )
{
	std::string strKey;

	if ( szKey ) strKey += szKey;

	strKey += CONSTANT_SEPARATOR;
	strKey += szName;

	m_mapFlagName[ strKey ] = nFlagIndex;
}

void _XEnvStructImpl::insert( const char* szKey, TENV* pData, TENV* pSet )
{	
	char* p = const_cast< char* >( szKey );
	char* szFirstKey = p;
	const char* pDot;
	bool bIsTerminal = true;

	char buf[128];

	if ( (pDot = strchr( szKey, '.' )) != NULL ) 
	{	
		s_strncpy( buf, _countof( buf ), szKey, pDot - szKey );
		buf[pDot - szKey] = 0;
		szFirstKey = buf;
		bIsTerminal = false;
	}

	if ( !pSet->pChildList )
	{
		pSet->pChildList = new ENVMAP;
	}

	if ( bIsTerminal ) 
	{
		pData->SetKey( szKey );
		pSet->pChildList->insert( pData );
		pData->pParentNode = pSet;
		return;
	}

	TENV Key( szFirstKey );
	ENVMAP::iterator it = pSet->pChildList->find( &Key );

	if ( it == pSet->pChildList->end() )
	{
		TENV* pNew = new TENV( szFirstKey );
		pNew->SetAsContainer();
		pSet->pChildList->insert( pNew );
		pNew->pParentNode = pSet;
		insert( pDot+1, pData, pNew );
	}
	else
	{
		insert( pDot+1, pData,* it );
	}

	
}

TENV* _XEnvStructImpl::find( const char* szKey, TENV* pSet ) const
{
	const char* pDot;

	if ( (pDot = strchr( szKey, '.' )) != NULL ) 
	{
		if ( !pSet->pChildList ) return NULL;

		char buf[128];
		s_strncpy( buf, _countof( buf ), szKey, pDot - szKey );
		buf[pDot - szKey] = 0;

		TENV* pNode = NULL;
		TENV Key( buf );
		ENVMAP::iterator it = pSet->pChildList->find( &Key );
		if ( it != pSet->pChildList->end() ) pNode =* it;

		if ( !pNode ) return NULL;

		return find( pDot+1, pNode );
	}

	if ( !pSet->pChildList )
	{
		if ( _stricmp( szKey, pSet->GetKey().c_str() ) ) return NULL;
		return pSet;
	}

	TENV Key( szKey );
	ENVMAP::iterator it = pSet->pChildList->find( &Key );
	if ( it == pSet->pChildList->end() )
	{
		return NULL;
	}

	return* it;
}

void _XEnvStructImpl::SetEventReceiver( XEnvStruct::XEnvEventReceiver* pReceiver )
{
	m_pReceiver = pReceiver;
}

XEnvStruct::Vector _XEnvStructImpl::GetVector( const char* szKey, float default_x, float default_y, float default_z ) const
{
	TENV* pNew = find( szKey, m_pSet );

	if ( pNew ) return pNew->GetVector();

	return XEnvStruct::Vector( default_x, default_y, default_z );
}

XEnvStruct::Quaternion _XEnvStructImpl::GetQuaternion( const char* szKey, float default_x, float default_y, float default_z, float default_w ) const
{
	TENV* pNew = find( szKey, m_pSet );

	if ( pNew ) return pNew->GetQuaternion();

	return XEnvStruct::Quaternion( default_x, default_y, default_z, default_w );
}

std::string _XEnvStructImpl::GetString( const char* szKey, const char* szDefault ) const
{
	TENV* pNew = find( szKey, m_pSet );

	if ( pNew ) return pNew->GetString();

	if ( szDefault ) return std::string( szDefault );

	return std::string( "<NULL>" );
}

int _XEnvStructImpl::GetInt( const char* szKey, int nDefault ) const
{
	TENV* pNew = find( szKey, m_pSet );

	if ( pNew )
	{
		if ( pNew->IsNumber() ) return pNew->GetInteger();
		else if ( pNew->IsFloat() ) return static_cast< int >( pNew->GetFloat() );
		return atoi( pNew->GetString().c_str() );
	}

	return nDefault;
}

float _XEnvStructImpl::GetFloat( const char* szKey, float fDefault ) const
{
	TENV* pNew = find( szKey, m_pSet );

	if ( pNew )
	{
		if ( pNew->IsNumber() ) return float( pNew->GetInteger() );
		else if ( pNew->IsFloat() ) return pNew->GetFloat();
		return static_cast< float >( atof( pNew->GetString().c_str() ) );
	}

	return fDefault;
}

bool _XEnvStructImpl::Remove( TENV* p )
{	
	if ( !p->pParentNode )
	{
		return true;
	}

	p->pParentNode->RemoveChildNode( p );

	if ( p->pParentNode->IsContainer() && !p->pParentNode->pChildList->size() )
	{
		Remove( p->pParentNode );
	}

	delete p;

	return true;
}

void _XEnvStructImpl::SetFlag( const char* szKey, const char* szFlagName, bool bFlag )
{
	std::map< std::string, int >::iterator it = m_mapFlagName.find( szKey );
	if ( it == m_mapFlagName.end() ) return;

	return SetFlag( szKey, it->second, bFlag );
}

void _XEnvStructImpl::SetFlag( const char* szKey, int nFlagIndex, bool bFlag )
{
	TENV* pNew = find( szKey, m_pSet );
	
	if ( pNew )
	{
		int nData = pNew->GetInteger();

		if ( bFlag ) nData |= ( 1 << nFlagIndex );
		else		nData &= ~( 1 << nFlagIndex );

		pNew->SetData( nData );

		onEnvUpdate( szKey );
	}
	else
	{
		int nData = 0;
		if ( bFlag ) nData |= ( 1 << nFlagIndex );

		insert( szKey, new TENV( szKey, nData ), m_pSet );

		onEnvInsert( szKey );
	}
}

bool _XEnvStructImpl::Remove( const char* szKey )
{
	TENV* pNew = find( szKey, m_pSet );

	if ( !pNew ) return false;

	return Remove( pNew );
}

bool _XEnvStructImpl::IsExist( const char* szKey ) const
{
	return (find( szKey, m_pSet ) != NULL);
}

bool _XEnvStructImpl::IsContainer( const char* szKey ) const
{
	TENV* pNew = find( szKey, m_pSet );

	if ( pNew && pNew->IsContainer() ) return true;

	return false;
}

bool _XEnvStructImpl::IsBinded( const char* szKey ) const
{
	TENV* pNew = find( szKey, m_pSet );

	if ( pNew && pNew->IsBinded() ) return true;

	return false;
}

unsigned _XEnvStructImpl::DoEachData( XEnvStruct::XEnvFunctor& fo, const char* szMask, bool bTranslateDataToString )
{
	return m_pSet->DoEachData( fo, szMask, bTranslateDataToString );
}


XEnvStruct::XEnvStruct()
{
	m_pImpl = new _XEnvStructImpl();
}

XEnvStruct::~XEnvStruct()
{
	_XEnvStructImpl * pImpl = m_pImpl;
	m_pImpl = NULL;
	delete pImpl;
}

void XEnvStruct::SetEventReceiver( XEnvStruct::XEnvEventReceiver* pReceiver )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->SetEventReceiver( pReceiver );
}

void XEnvStruct::Set( const char* szKey, int nData )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Set( szKey, nData );
}

void XEnvStruct::Set( const char* szKey, float fData )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Set( szKey, fData );
}

void XEnvStruct::Set( const char* szKey, const char* szData )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Set( szKey, szData );
}

void XEnvStruct::SetReadOnly( const char* szKey )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->SetReadOnly( szKey );
}

void XEnvStruct::Set( const char* szKey, const Quaternion& q )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Set( szKey, q );
}

void XEnvStruct::Set( const char* szKey, const Vector& v )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Set( szKey, v );
}

int XEnvStruct::GetConstant( const char* szKey, const char* szName, int nDefault )
{
	if( !m_pImpl )
		return nDefault;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->GetConstant( szKey, szName, nDefault );
}

void XEnvStruct::SetConstant( const char* szKey, const char* szName, int nFlagIndex )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->SetConstant( szKey, szName, nFlagIndex );
}

void XEnvStruct::SetFlag( const char* szKey, int nFlagIndex, bool bFlag )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->SetFlag( szKey, nFlagIndex, bFlag );
}

void XEnvStruct::Bind( const char* szKey, bool* pData )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Bind( szKey, pData );
}

void XEnvStruct::Bind( const char* szKey, float* pData )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Bind( szKey, pData );
}

void XEnvStruct::Bind( const char* szKey, XEnvStruct::Vector* pVector )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Bind( szKey, pVector );
}

void XEnvStruct::Bind( const char* szKey, short* pData )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Bind( szKey, pData );
}

void XEnvStruct::Bind( const char* szKey, XEnvStruct::Quaternion* pQuaternion )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Bind( szKey, pQuaternion );
}


void XEnvStruct::Bind( const char* szKey, int* pData )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Bind( szKey, pData );
}

void XEnvStruct::Bind( const char* szKey, int (*fn)() )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Bind( szKey, fn );
}

void XEnvStruct::Bind( const char* szKey, std::string& (*fn)() )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Bind( szKey, fn );
}

void XEnvStruct::Bind( const char* szKey, std::string* str )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	m_pImpl->Bind( szKey, str );
}

std::string XEnvStruct::GetString( const char* szKey, const char* szDefault ) const
{
	if( !m_pImpl )
		return szDefault;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->GetString( szKey, szDefault );
}

XEnvStruct::Vector XEnvStruct::GetVector( const char* szKey, float default_x, float default_y, float default_z ) const
{
	if( !m_pImpl )
		return XEnvStruct::Vector( default_x, default_y, default_z );

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->GetVector( szKey, default_x, default_y, default_z );
}

XEnvStruct::Quaternion XEnvStruct::GetQuaternion( const char* szKey, float default_x, float default_y, float default_z, float default_w ) const
{
	if( !m_pImpl )
		return XEnvStruct::Quaternion( default_x, default_y, default_z, default_w );

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->GetQuaternion( szKey, default_x, default_y, default_z, default_w );
}

int XEnvStruct::GetInt( const char* szKey, int nDefault ) const
{
	if( !m_pImpl )
		return nDefault;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->GetInt( szKey, nDefault );
}

float XEnvStruct::GetFloat( const char* szKey, float fDefault ) const
{
	if( !m_pImpl )
		return fDefault;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->GetFloat( szKey, fDefault );
}

bool XEnvStruct::Remove( const char* szKey )
{
	if( !m_pImpl )
		return false;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->Remove( szKey );
}

void XEnvStruct::SetDefaultValue( const char* szKey, int nDefault )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	if ( m_pImpl->IsExist( szKey ) ) return;
	m_pImpl->Set( szKey, nDefault );
}

void XEnvStruct::SetDefaultValue( const char* szKey, const char* szData )
{
	if( !m_pImpl )
		return;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	if ( m_pImpl->IsExist( szKey ) ) return;
	return m_pImpl->Set( szKey, szData );
}

unsigned  XEnvStruct::DoEachData( XEnvFunctor& fo, const char* szMask, bool bTranslateDataToString )
{
	if( !m_pImpl )
		return 0;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );

	return m_pImpl->DoEachData( fo, szMask, bTranslateDataToString );
}

XEnvStruct& ENV()
{
	static XEnvStruct env;
	return env;
}

bool XEnvStruct::IsExist( const char* szKey ) const
{
	if( !m_pImpl )
		return false;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->IsExist( szKey );
}

bool XEnvStruct::IsBinded( const char* szKey ) const
{
	if( !m_pImpl )
		return false;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->IsBinded( szKey );
}

bool XEnvStruct::IsContainer( const char* szKey ) const
{
	if( !m_pImpl )
		return false;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	return m_pImpl->IsContainer( szKey );
}

bool XEnvStruct::SaveToFile( const char* szFileName, const char* szMask, const bool bEncode )
{
	if( !m_pImpl )
		return false;

	FILE* fp = NULL;
	if( fopen_s( &fp, szFileName, (bEncode) ? "wb" : "wt" ) != 0 )
		return false;
	if ( !fp )
		return false;

	struct FileCloser
	{
		FileCloser( FILE * _fp ) : fp( _fp )	{}
		~FileCloser()							{ fclose( fp ); }

		FILE * fp;
	} fc( fp );

	std::string strBuffer;

	struct _M : XEnvFunctor
	{
		_M( std::string & strBuffer )
			: m_strBuffer( strBuffer )
		{
			::memset( m_szBuffer, 0, sizeof( m_szBuffer ) );
		}

		virtual void onString(	   const std::string& strKey, const std::string& strData )			{ s_sprintf( m_szBuffer, _countof( m_szBuffer ), "S:%s:%s\n", strKey.c_str(), strData.c_str() ); m_strBuffer += m_szBuffer; }
		virtual void onFloat(	   const std::string& strKey, float fNumber )						{ s_sprintf( m_szBuffer, _countof( m_szBuffer ), "F:%s:%f\n", strKey.c_str(), fNumber ); m_strBuffer += m_szBuffer; }
		virtual void onVector(	   const std::string& strKey, const XEnvStruct::Vector& v )			{ s_sprintf( m_szBuffer, _countof( m_szBuffer ), "V:%s:%f;%f;%f\n", strKey.c_str(), v.x, v.y, v.z ); m_strBuffer += m_szBuffer; }
		virtual void onQuaternion( const std::string& strKey, const XEnvStruct::Quaternion& q )		{ s_sprintf( m_szBuffer, _countof( m_szBuffer ), "Q:%s:%f;%f;%f;%f\n", strKey.c_str(), q.x, q.y, q.z, q.w ); m_strBuffer += m_szBuffer; }
		virtual void onInteger(	   const std::string& strKey, int nNumber )							{ s_sprintf( m_szBuffer, _countof( m_szBuffer ), "N:%s:%d\n", strKey.c_str(), nNumber ); m_strBuffer += m_szBuffer; }
		virtual void onShort(	   const std::string& strKey, short nNumber )						{ s_sprintf( m_szBuffer, _countof( m_szBuffer ), "T:%s:%d\n", strKey.c_str(), nNumber ); m_strBuffer += m_szBuffer; }

		std::string &	m_strBuffer;
		char			m_szBuffer[1024];

	private:
		const _M& operator=( const _M& )
		{
			return *this;
		}

	} _m( strBuffer );
	
	DoEachData( _m, szMask );

	size_t nContentsLength = strBuffer.size();

	if( bEncode )
	{
		std::vector< BYTE > vEncrypted;
		if( !XEncrypt::Encrypt( XEncrypt::ET_ZLIB_WITH_SIMPLE_CIPHER, ENV_FILE_FORMAT_TAG, ENV_FILE_VERSION3, strBuffer.c_str(), nContentsLength, vEncrypted ) )
			return false;

		if( fwrite( &vEncrypted.front(), vEncrypted.size(), 1, fp ) != 1 )
		{
			assert( 0 );
			return false;
		}
	}
	else
	{
		if( nContentsLength && fwrite( strBuffer.data(), 1, nContentsLength, fp ) != nContentsLength )
		{
			assert( 0 );
			return false;
		}
	}

	return true;
}

bool XEnvStruct::LoadFromFile( const char* szFileName, const char* szMask, const bool bEncode, const unsigned int nStrictFileVersion )
{
	if( !m_pImpl )
		return false;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );
	bool bRtn = false;

	FILE* fp = NULL;
	if( fopen_s( &fp, szFileName, (bEncode) ? "rb" : "rt" ) != 0 )
		return false;
	if ( !fp )
		return false;

	struct FileCloser
	{
		FileCloser( FILE * _fp ) : fp( _fp )	{}
		~FileCloser()							{ fclose( fp ); }

		FILE * fp;
	} fc( fp );

	std::string strContents;
	size_t nContentsLength = 0;

	fseek( fp, 0, SEEK_END );
	nContentsLength = ftell( fp );
	rewind( fp );

	if( !nContentsLength )
		return true;

	if( bEncode )
	{
		// No header
		if( !nContentsLength )
		{
			assert( 0 );
			return false;
		}

		BYTE * pBuffer = new BYTE[ nContentsLength ];
		struct MemoryDeallocator
		{
			MemoryDeallocator( BYTE * _pBuffer ) : pBuffer( _pBuffer )	{}
			~MemoryDeallocator()										{ delete [] pBuffer; }

			BYTE * pBuffer;
		} md( pBuffer );

		if( fread( pBuffer, nContentsLength, 1, fp ) != 1 )
		{
			assert( 0 );
			return false;
		}

		const unsigned int nVersion = XEncrypt::GetFormatVersion( ENV_FILE_FORMAT_TAG, pBuffer, nContentsLength );
		if( nVersion == XEncrypt::INVALID_VERSION || nStrictFileVersion && nVersion != nStrictFileVersion )
		{
			assert( 0 );
			return false;
		}

		std::vector< BYTE > vDecrypted;

		switch( nVersion )
		{
		case ENV_FILE_VERSION:
			if( !XEncrypt::Decrypt( XEncrypt::ET_ZLIB, ENV_FILE_FORMAT_TAG, ENV_FILE_VERSION, pBuffer, nContentsLength, vDecrypted ) )
			{
				assert( 0 );
				return false;
			}
			break;
		case ENV_FILE_VERSION3:
			if( !XEncrypt::Decrypt( XEncrypt::ET_ZLIB_WITH_SIMPLE_CIPHER, ENV_FILE_FORMAT_TAG, ENV_FILE_VERSION3, pBuffer, nContentsLength, vDecrypted ) )
			{
				assert( 0 );
				return false;
			}
			break;
		default:
			assert( 0 );
			return false;
		}

		if( vDecrypted.empty() )
			return true;

		vDecrypted.push_back( NULL );
		strContents = reinterpret_cast< char * >( &vDecrypted.front() );
	}
	else
	{
		strContents.reserve( nContentsLength + 1 );		// Additional 1 byte is added for terminating null.

		nContentsLength = fread( const_cast< char * >( strContents.data() ), 1, nContentsLength, fp );
		if( !nContentsLength )
		{
			assert( 0 );
			return false;
		}

		const_cast< char * >( strContents.data() )[ nContentsLength ] = '\0';
	}

	char* p;
	bRtn = true;
	char * buf = NULL, * next_token = NULL;

	buf = strtok_s( const_cast< char * >( strContents.data() ), "\n", &next_token );
	while( buf )
	{
		p = strchr( &buf[2], ':' );

		if ( !p ) {
			bRtn = false;
			break;
		}

		*p = '\0';
		p++;

		if (* p )
		{
			if ( p[ strlen( p ) - 1 ] == '\n' ) p[ strlen( p ) - 1 ] = '\0';
		}

		char type  = buf[0];
		char* key  = &buf[2];
		char* data = p;

		if ( !nsl::wildcmp( szMask, key, true ) ) continue;

		Vector v;
		Quaternion q;

		v.x = v.y = v.z = 0.0f;
		q.x = q.y = q.z = q.w = 0.0f;
		
		switch( type )
		{
		case 'S':	Set( key, data ); break;
		case 'F':	Set( key, (float)atof(data) ); break;
		case 'V':	sscanf_s( data, "%f;%f;%f", &v.x, &v.y, &v.z ); Set( key, v ); break;
		case 'Q':	sscanf_s( data, "%f;%f;%f;%f", &q.x, &q.y, &q.z, &q.w ); Set( key, q ); break;
		case 'N':	Set( key, atoi(data) ); break;
		case 'T':	Set( key, (short)atoi(data) ); break;
		}

		buf = strtok_s( NULL, "\n", &next_token );
	}

	return bRtn;
}

bool XEnvStruct::LoadFromMemory( const void* pBuf, size_t bufSize, const char* szMask )
{
	if( !m_pImpl )
		return false;

	const char* srcBuf = reinterpret_cast< const char* >( pBuf );
	const char* srcBufEnd = srcBuf + bufSize;

	if ( !srcBuf || bufSize == 0 ) return false;

	THREAD_SYNCRONIZE( &m_pImpl->csLock );

	char buf[1024];
	char* p;

	bool bRtn = true;

	while ( srcBuf < srcBufEnd )
	{
		{
			const char* pp = srcBuf;
			for (; pp < srcBuf + 1023 && pp < srcBufEnd; ++pp )
			{
				if ( *pp == '\n' || *pp == '\r' )
					break;
			}
			s_memcpy( buf, sizeof( buf ), srcBuf, pp - srcBuf );
			buf[ pp - srcBuf ] = 0;

			for (; pp < srcBufEnd; ++pp )
			{
				if ( *pp != '\n' && *pp != '\r' )
					break;
			}
			srcBuf = pp;
		}

		p = strchr( &buf[2], ':' );

		if ( !p ) {
			bRtn = false;
			break;
		}

		*p = '\0';
		p++;

		if (* p )
		{
			if ( p[ strlen( p ) - 1 ] == '\n' ) p[ strlen( p ) - 1 ] = '\0';
		}

		char type  = buf[0];
		char* key  = &buf[2];
		char* data = p;

		if ( !nsl::wildcmp( szMask, key, true ) ) continue;

		Vector v;
		Quaternion q;

		v.x = v.y = v.z = 0.0f;
		q.x = q.y = q.z = q.w = 0.0f;
		
		switch( type )
		{
		case 'S':	Set( key, data ); break;
		case 'F':	Set( key, (float)atof(data) ); break;
		case 'V':	sscanf_s( data, "%f;%f;%f", &v.x, &v.y, &v.z ); Set( key, v ); break;
		case 'Q':	sscanf_s( data, "%f;%f;%f;%f", &q.x, &q.y, &q.z, &q.w ); Set( key, q ); break;
		case 'N':	Set( key, atoi(data) ); break;
		case 'T':	Set( key, (short)atoi(data) ); break;
		}
	}

	return bRtn;
}

int XEnvStruct::ParseCmdLine( const char* szArgument )
{
	if( !m_pImpl )
		return 0;

	int unCnt = 0;

	const char* szCmdLine = szArgument;

	Set( "app.cmdline", GetCommandLineA() );
	
	if ( !szCmdLine ) szCmdLine = GetCommandLineA();

	// { 커맨드 라인 아규먼트 세팅
	if ( szCmdLine ) {
		std::vector< std::string > vArg;
		nsl::split( szCmdLine, vArg, " " );
		
		// { 바보 바운즈 체커의 워닝을 피하기 위해.. -_-
		char* p = const_cast< char* >( szCmdLine );
		std::string strCmdLine;
		while (* p )	strCmdLine.push_back(* p++ );		
		// }

		char* szArg;
		for ( unsigned i = 1; i < vArg.size(); i++ ) 
		{
			 szArg = const_cast< char* >( vArg[i].c_str() );
			 if ( szArg[0] == '/' && strlen( szArg ) > 1 )
			 {
				 szArg++;

				 std::string strKey;
				 std::string strData;
				 if ( strchr( szArg, ':' ) )
				 {
					 while (* szArg &&* szArg != ':' )
					 {
						 strKey += (*szArg++);
					 }

					 if (* szArg == ':' )
					 {
						 szArg++;
						 strData = szArg;

						 Set( strKey, strData );
					 }
				 }
				 else if( strchr( szArg, '=' ) )
				 {
					 while( *szArg && *szArg != '=' )
					 {
						 strKey += (*szArg++);
					 }

					 if( *szArg == '=' )
					 {
						 szArg++;
						 strData = szArg;

						 Set( strKey, strData );
					 }
				 }
				 else
				 {
					 Set( szArg, 1 );
				 }

				 unCnt++;
			 }
		}
	}
	// }

	return unCnt;
}

