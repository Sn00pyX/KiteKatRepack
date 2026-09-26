#pragma once

#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <vector>
#include <cassert>


class XEncrypt
{
public:
	enum _ENCRYPT_TYPE
	{
		ET_NONE							= 0,
		ET_ZLIB							= 1,	// Deprecated. It's leaked.
		ET_ZLIB64						= 2,	// Deprecated. It's leaked.
		ET_ZLIB_WITH_SIMPLE_CIPHER		= 3,	// Corrected version of ET_ZLIB_WITH_SIMPLE_CIPHER.

		MAX_ENCRYPT_TYPE
	};

	static const unsigned int		INVALID_VERSION		= 0xFFFFFFFF;

	// Encrypted data should include following items.
	// 1. Format tag			: VariableLength between 1 and 8 bytes
	// 2. Version of format		: unsigned int

	// pSource should not include eType and pszFmtTag but vResult will include them.
	static bool Encrypt( _ENCRYPT_TYPE eType, const char * pszFmtTag, const unsigned int nVersion, const void * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );
	// pSource should include eType and pszFmtTag but vResult will not include them.
	static bool Decrypt( _ENCRYPT_TYPE eType, const char * pszFmtTag, const unsigned int nVersion, const void * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );

	static bool				IsValidFormat( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength )
	{
		// FmtTag validation
		size_t nFmtTagLength = strlen( pszFmtTag );
		if( nSourceLength < nFmtTagLength || memcmp( pSource, pszFmtTag, nFmtTagLength ) )
		{
			assert( 0 );
			return false;
		}

		// Version validation
		if( nSourceLength < nFmtTagLength + sizeof( unsigned int ) || *reinterpret_cast< const unsigned int * >( pSource + nFmtTagLength ) != nVersion )
		{
			assert( 0 );
			return false;
		}

		return true;
	}
	static unsigned int		GetFormatVersion( const char * pszFmtTag, const BYTE * pSource, size_t nSourceLength )
	{
		// FmtTag validation
		size_t nFmtTagLength = strlen( pszFmtTag );
		if( nSourceLength < nFmtTagLength + sizeof( unsigned int ) || memcmp( pSource, pszFmtTag, nFmtTagLength ) )
		{
			assert( 0 );
			return INVALID_VERSION;
		}

		return *reinterpret_cast< const unsigned int * >( pSource + nFmtTagLength );
	}

private:
	// Abstract interface class
	class XEncryptImpl
	{
	public:
		XEncryptImpl()				{}
		virtual ~XEncryptImpl()		{}

		virtual _ENCRYPT_TYPE	GetType() const = NULL;
		virtual bool			Encrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult ) = NULL;
		virtual bool			Decrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult ) = NULL;
	};

	// Factory function
	static XEncryptImpl *		getImpl( _ENCRYPT_TYPE eType );

	// Implementation classes
	class XEncryptNone : public XEncryptImpl
	{
	public:
		virtual _ENCRYPT_TYPE	GetType() const		{ return ET_NONE; }
		virtual bool			Encrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );
		virtual bool			Decrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );
	};

	class XEncryptZlib : public XEncryptImpl
	{
	public:
		virtual _ENCRYPT_TYPE	GetType() const		{ return ET_ZLIB; }
		virtual bool			Encrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );
		virtual bool			Decrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );

	private:
#pragma pack(1)
		struct Header
		{
			// FmtTag: Variable length
			unsigned int	nVersion;
			unsigned int	nCompressedLength;
			unsigned int	nOriginalLength;
		};
#pragma pack()
	};

	class XEncryptZlib64 : public XEncryptImpl
	{
	public:
		virtual _ENCRYPT_TYPE	GetType() const		{ return ET_ZLIB64; }
		virtual bool			Encrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );
		virtual bool			Decrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );

	private:
#pragma pack(1)
		struct Header
		{
			// FmtTag: Variable length
			unsigned int		nVersion;
			unsigned __int64	nCompressedLength;
			unsigned __int64	nOriginalLength;
		};
#pragma pack()
	};

	class XEncryptZlibWithSimpleCipher : public XEncryptImpl
	{
	public:
		virtual _ENCRYPT_TYPE	GetType() const		{ return ET_ZLIB_WITH_SIMPLE_CIPHER; }
		virtual bool			Encrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );
		virtual bool			Decrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult );
	};
};
