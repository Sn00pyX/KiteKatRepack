#pragma once

// FreeBSD sysctl 스타일 환경변수 저장소
//
// * '.' 을 구분자로 사용한다.
// * thread-safe 하다.
//
//  2004/02/14
//	by Testors

#include <string>
#include <vector>

struct XEnvStruct
{
	XEnvStruct();
	~XEnvStruct();

	struct		Vector
	{
		Vector() {}
		Vector( float _x, float _y, float _z ) : x( _x ), y( _y ), z( _z ) {}
		float x, y, z;
	};

	struct Quaternion : Vector
	{
		Quaternion() {}
		Quaternion( float _x, float _y, float _z, float _w ) : Vector( _x, _y, _z ), w( _w ) {}
		float w;
	};

	static const char *			ENV_FILE_FORMAT_TAG;
	static const unsigned int	ENV_FILE_VERSION		= 0x00010000;
	static const unsigned int	ENV_FILE_VERSION3		= 0x00030000;
	static const unsigned int	ENV_LATEST_FILE_VERSION	= ENV_FILE_VERSION3;

	struct XEnvFunctor
	{	
		virtual void onString(	   const std::string& strKey, const std::string& strData )			{}
		virtual void onFloat(	   const std::string& strKey, float fNumber )						{}
		virtual void onVector(	   const std::string& strKey, const XEnvStruct::Vector& v )			{}
		virtual void onQuaternion( const std::string& strKey, const XEnvStruct::Quaternion& q )		{}
		virtual void onInteger(	   const std::string& strKey, int nNumber )							{}
		virtual void onShort(	   const std::string& strKey, short nNumber )						{ onInteger( strKey, nNumber ); }
	};

	// 주의. Bind() 해 놓은 변수의 경우 이벤트를 받을 수 없다.
	struct XEnvEventReceiver
	{
		virtual void onEnvUpdate( const char* szFullKey ) = 0;
		virtual void onEnvInsert( const char* szFullKey ) = 0;
	};	
	void		SetEventReceiver( XEnvEventReceiver* pReceiver );

	void		SetReadOnly( const char* szKey );
	// Add() 인터페이스는 따로 지원하지 않는다. 키가 없을경우 자동으로 생성된다.
	void		Set( const char* szKey, const Quaternion& q );
	void		Set( const char* szKey, const Vector& v );
	void		Set( const char* szKey, int nData );
	void		Set( const char* szKey, float fData );
	void		Set( const char* szKey, const char* szData );

	void		SetConstant( const char* szKey, const char* szName, int nFlagIndex );
	int			GetConstant( const char* szKey, const char* szName, int nDefault = 0 );
	void		SetConstant( const char* szName, int nFlagIndex )	{ SetConstant( NULL, szName, nFlagIndex ); }
	int			GetConstant( const char* szName, int nDefault = 0 ) { return GetConstant( NULL, szName, nDefault ); }

	void		SetFlag( const char* szKey, int nFlagIndex, bool bFlag );

	void		Bind( const char* szKey, XEnvStruct::Quaternion* pQuaternion );
	void		Bind( const char* szKey, XEnvStruct::Vector* pVector );
	void		Bind( const char* szKey, bool* pData );
	void		Bind( const char* szKey, short* pData );
	void		Bind( const char* szKey, int* pData );
	void		Bind( const char* szKey, float* pData );
	void		Bind( const char* szKey, int (*fn)() );
	void		Bind( const char* szKey, std::string& (*fn)() );
	void		Bind( const char* szKey, std::string* str );

	std::string	GetString( const char* szKey, const char* szDefault = NULL ) const;
	int			GetInt( const char* szKey, int nDefault = 0 ) const;
	float		GetFloat( const char* szKey, float fDefault = 0 ) const;
	Quaternion	GetQuaternion( const char* szKey, float default_x = 0.0f, float default_y = 0.0f, float default_z = 0.0f, float default_w = 0.0f ) const;
	Vector		GetVector( const char* szKey, float default_x = 0.0f, float default_y = 0.0f, float default_z = 0.0f ) const;	

	bool		Remove( const char* szKey );

	bool		IsBinded( const char* szKey ) const;
	bool		IsContainer( const char* szKey ) const;
	bool		IsExist( const char* szKey ) const;

	unsigned	DoEachData( XEnvFunctor& fo, const char* szMask= "*", bool bTranslateDataToString = false );

	void		SetDefaultValue( const char* szKey, int nDefault );
	void		SetDefaultValue( const char* szKey, const char* szData );

	int			ParseCmdLine( const char* szArgument = NULL );
	bool		LoadFromFile( const char* szFileName, const char* szMask = "*", const bool bEncode = false, const unsigned int nStrictFileVersion = ENV_LATEST_FILE_VERSION );
	bool		SaveToFile( const char* szFileName, const char* szMask = "*", const bool bEncode = false );

	bool		LoadFromMemory( const void* pBuf, size_t bufSize, const char* szMask = "*" );

	//

	void		Set( const std::string& strKey, int nData )										{ Set( strKey.c_str(), nData ); }
	void		Set( const std::string& strKey, const std::string& strData )					{ Set( strKey.c_str(), strData.c_str() ); }

	void		Bind( const std::string& strKey, int* pData )									{ Bind( strKey.c_str(), pData ); }
	void		Bind( const std::string& strKey, int (*fn)() )									{ Bind( strKey.c_str(), fn ); }

	std::string	GetString( const std::string& strKey, const char* szDefault = NULL ) const		{ return GetString( strKey.c_str(), szDefault ); }
	int			GetInt( const std::string& strKey ) const										{ return GetInt( strKey.c_str() );    }	
	bool		Remove( const std::string& strKey )												{ return Remove( strKey.c_str() );	  }
	bool		IsBinded( const std::string& strKey ) const										{ return IsBinded( strKey.c_str() ); }
	bool		IsContainer( const std::string& strKey ) const									{ return IsContainer( strKey.c_str() ); }
	bool		IsExist( const std::string& strKey ) const										{ return IsExist( strKey.c_str() );   }

private:

	XEnvStruct( const XEnvStruct& );
	XEnvStruct& operator=( const XEnvStruct& );

	struct _XEnvStructImpl* m_pImpl;
};

XEnvStruct& ENV();

struct XEnvScopedEnv
{
	XEnvScopedEnv( const char* szKey ) : m_szKey( szKey )
	{
		ENV().Set( szKey, "doing" );
	}

	~XEnvScopedEnv()
	{
		ENV().Remove( m_szKey );
	}

	const char* m_szKey;
};

#define SCOPED_ENV( keyname ) XEnvScopedEnv __scoped_env( keyname );