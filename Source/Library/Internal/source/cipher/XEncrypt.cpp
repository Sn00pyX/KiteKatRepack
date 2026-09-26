
#include <map>

#include "../../include/cipher/XEncrypt.h"
#include "../../include/cipher/XZlibEncoder.h"
#include "../../include/cipher/SimpleCipher.h"
#include "../../include/toolkit/safe_function.h"


XEncrypt::XEncryptImpl * XEncrypt::getImpl( _ENCRYPT_TYPE eType )
{
	static std::map< _ENCRYPT_TYPE, XEncryptImpl * > s_mapImpl;
	static bool bInit = false;
	if( !bInit )
	{
		bInit = true;

		static XEncryptNone instXEncryptNone;
		static XEncryptZlib instXEncryptZlib;
		static XEncryptZlib64 instXEncryptZlib64;
		static XEncryptZlibWithSimpleCipher instXEncryptZlibWithSimpleCipher2;

		s_mapImpl[ ET_NONE ] = &instXEncryptNone;
		s_mapImpl[ ET_ZLIB ] = &instXEncryptZlib;
		s_mapImpl[ ET_ZLIB64 ] = &instXEncryptZlib64;
		s_mapImpl[ ET_ZLIB_WITH_SIMPLE_CIPHER ] = &instXEncryptZlibWithSimpleCipher2;
	}

	if( s_mapImpl.find( eType ) == s_mapImpl.end() )
	{
		assert( 0 );
		return s_mapImpl[ ET_NONE ];
	}

	return s_mapImpl[ eType ];
}

bool XEncrypt::Encrypt( _ENCRYPT_TYPE eType, const char * pszFmtTag, const unsigned int nVersion, const void * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	return getImpl( eType )->Encrypt( pszFmtTag, nVersion, static_cast< const BYTE * >( pSource ), nSourceLength, vResult );
}

bool XEncrypt::Decrypt( _ENCRYPT_TYPE eType, const char * pszFmtTag, const unsigned int nVersion, const void * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	return getImpl( eType )->Decrypt( pszFmtTag, nVersion, static_cast< const BYTE * >( pSource ), nSourceLength, vResult );
}


// XEncryptNone
bool XEncrypt::XEncryptNone::Encrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	vResult.clear();
	vResult.insert( vResult.end(), pSource, pSource + nSourceLength );
	return true;
}
bool XEncrypt::XEncryptNone::Decrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	vResult.clear();
	vResult.insert( vResult.end(), pSource, pSource + nSourceLength );
	return true;
}

// XEncryptZlib
bool XEncrypt::XEncryptZlib::Encrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	size_t nFmtTagLength = strlen( pszFmtTag );
	if( nFmtTagLength > 8 )
	{
		assert( 0 );
		return false;
	}

	Header header;
	header.nVersion = nVersion;
	header.nOriginalLength = static_cast< unsigned int >( nSourceLength );
	// The required buffer for compression size is calculated as described on http://www.zlib.net/manual.html#compress.
	header.nCompressedLength = ( nSourceLength ) ? static_cast< unsigned int >( 1.001 * ( nSourceLength + 12 ) + 1 ) : 0;

	size_t nEncodedLength = nFmtTagLength + sizeof( Header ) + header.nCompressedLength;

	std::vector< BYTE > vBuffer( nEncodedLength );
	BYTE * pBuffer = &vBuffer.front();

	if( nSourceLength )
	{
		size_t nWrittenLength = 0;

		if( XZlibEncoder::Encode( pSource, nSourceLength, pBuffer + nFmtTagLength + sizeof( Header ), nEncodedLength - nFmtTagLength - sizeof( Header ), &nWrittenLength ) )
		{
			assert( 0 );
			return false;
		}

		if( nWrittenLength > header.nCompressedLength )
		{
			assert( 0 );
			return false;
		}

		header.nCompressedLength = unsigned int( nWrittenLength );
		nEncodedLength = nFmtTagLength + sizeof( Header ) + nWrittenLength;

		vBuffer.resize( nEncodedLength );
		// Preventing pBuffer to be invalid by reallocation by resize()
		pBuffer = &vBuffer.front();
	}

	assert( nEncodedLength == header.nCompressedLength + sizeof( Header ) + nFmtTagLength && nEncodedLength == vBuffer.size() );

	// Writing header
	s_memcpy( pBuffer, vBuffer.size(), pszFmtTag, nFmtTagLength );
	s_memcpy( pBuffer + nFmtTagLength, vBuffer.size() - nFmtTagLength, &header, sizeof( Header ) );

	vResult.swap( vBuffer );
	return true;
}

bool XEncrypt::XEncryptZlib::Decrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	if( !IsValidFormat( pszFmtTag, nVersion, pSource, nSourceLength ) )
		return false;

	size_t nFmtTagLength = strlen( pszFmtTag );
	if( nSourceLength < nFmtTagLength + sizeof( Header ) )
	{
		assert( 0 );
		return false;
	}
	const Header * pHeader = reinterpret_cast< const Header * >( pSource + nFmtTagLength );

	if( pHeader->nCompressedLength > nSourceLength - nFmtTagLength - sizeof( Header ) )
	{
		assert( 0 );
		return false;
	}

	// An empty valid file
	if( nSourceLength == nFmtTagLength + sizeof( Header ) )
	{
		vResult.clear();
		return true;
	}

	// Invalid nOriginalLength information
	if( !pHeader->nOriginalLength )
	{
		assert( 0 );
		return false;
	}

	std::vector< BYTE > vBuffer( pHeader->nOriginalLength );
	size_t nDecodedLength = 0;
	if( XZlibEncoder::Decode( pSource + nFmtTagLength + sizeof( Header ), pHeader->nCompressedLength, &vBuffer.front(), vBuffer.size(), &nDecodedLength ) )
	{
		assert( 0 );
		return false;
	}

	if( nDecodedLength != pHeader->nOriginalLength )
	{
		assert( 0 );
		return false;
	}

	vResult.swap( vBuffer );
	return true;
}

// XEncryptZlib64
bool XEncrypt::XEncryptZlib64::Encrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	size_t nFmtTagLength = strlen( pszFmtTag );
	if( nFmtTagLength > 8 )
	{
		assert( 0 );
		return false;
	}

	Header header;
	header.nVersion = nVersion;
	header.nOriginalLength = static_cast< unsigned int >( nSourceLength );
	// The required buffer for compression size is calculated as described on http://www.zlib.net/manual.html#compress.
	header.nCompressedLength = ( nSourceLength ) ? static_cast< unsigned int >( 1.001 * ( nSourceLength + 12 ) + 1 ) : 0;

	size_t nEncodedLength = nFmtTagLength + sizeof( Header ) + static_cast< size_t >( header.nCompressedLength );

	std::vector< BYTE > vBuffer( nEncodedLength );
	BYTE * pBuffer = &vBuffer.front();

	if( nSourceLength )
	{
		size_t nWrittenLength = 0;

		if( XZlibEncoder::Encode( pSource, nSourceLength, pBuffer + nFmtTagLength + sizeof( Header ), nEncodedLength - nFmtTagLength - sizeof( Header ), &nWrittenLength ) )
		{
			assert( 0 );
			return false;
		}

		if( nWrittenLength > header.nCompressedLength )
		{
			assert( 0 );
			return false;
		}

		header.nCompressedLength = nWrittenLength;
		nEncodedLength = nFmtTagLength + sizeof( Header ) + nWrittenLength;

		vBuffer.resize( nEncodedLength );
		// Preventing pBuffer to be invalid by reallocation by resize()
		pBuffer = &vBuffer.front();
	}

	assert( nEncodedLength == header.nCompressedLength + sizeof( Header ) + nFmtTagLength && nEncodedLength == vBuffer.size() );

	// Writing header
	s_memcpy( pBuffer, vBuffer.size(), pszFmtTag, nFmtTagLength );
	s_memcpy( pBuffer + nFmtTagLength, vBuffer.size() - nFmtTagLength, &header, sizeof( Header ) );

	vResult.swap( vBuffer );
	return true;
}

bool XEncrypt::XEncryptZlib64::Decrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	if( !IsValidFormat( pszFmtTag, nVersion, pSource, nSourceLength ) )
		return false;

	size_t nFmtTagLength = strlen( pszFmtTag );
	if( nSourceLength < nFmtTagLength + sizeof( Header ) )
	{
		assert( 0 );
		return false;
	}
	const Header * pHeader = reinterpret_cast< const Header * >( pSource + nFmtTagLength );

	if( pHeader->nCompressedLength > nSourceLength - nFmtTagLength - sizeof( Header ) )
	{
		assert( 0 );
		return false;
	}

	// An empty valid file
	if( nSourceLength == nFmtTagLength + sizeof( Header ) )
	{
		vResult.clear();
		return true;
	}

	std::vector< BYTE > vBuffer( static_cast< size_t >( pHeader->nOriginalLength ) );
	size_t nDecodedLength = 0;
	if( XZlibEncoder::Decode( pSource + nFmtTagLength + sizeof( Header ), static_cast< size_t >( pHeader->nCompressedLength ), &vBuffer.front(), vBuffer.size(), &nDecodedLength ) )
	{
		assert( 0 );
		return false;
	}

	if( nDecodedLength != pHeader->nOriginalLength )
	{
		assert( 0 );
		return false;
	}

	vResult.swap( vBuffer );
	return true;
}

// XEncryptZlibWithSimpleCipher
bool XEncrypt::XEncryptZlibWithSimpleCipher::Encrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	size_t nFmtTagLength = strlen( pszFmtTag );
	if( nFmtTagLength > 8 )
	{
		assert( 0 );
		return false;
	}

	// The required buffer for compression size is calculated as described on http://www.zlib.net/manual.html#compress.
	std::vector< BYTE > vBuffer( ( nSourceLength ) ? static_cast< size_t >( 1.001 * ( nSourceLength + 12 ) + 1 ) : 0 );
	BYTE * pBuffer = &vBuffer.front();

	if( nSourceLength )
	{
		size_t nWrittenLength = 0;

		if( XZlibEncoder::Encode( pSource, nSourceLength, pBuffer, vBuffer.size(), &nWrittenLength ) )
		{
			assert( 0 );
			return false;
		}

		if( nWrittenLength > vBuffer.size() )
		{
			assert( 0 );
			return false;
		}

		vBuffer.resize( nWrittenLength );
		// Preventing pBuffer to be invalid by reallocation by resize()
		pBuffer = &vBuffer.front();
	}

	std::vector< BYTE > vOutput;
	SimpleCipher::Encrypt( nSourceLength, vBuffer, vOutput );

	vResult.clear();
	vResult.resize( nFmtTagLength + sizeof( unsigned int ) );
	s_memcpy( &vResult.front(), vResult.size(), pszFmtTag, nFmtTagLength );
	s_memcpy( &vResult.front() + nFmtTagLength, vResult.size() - nFmtTagLength, &nVersion, sizeof( unsigned int ) );
	vResult.insert( vResult.end(), vOutput.begin(), vOutput.end() );
	return true;
}

bool XEncrypt::XEncryptZlibWithSimpleCipher::Decrypt( const char * pszFmtTag, const unsigned int nVersion, const BYTE * pSource, size_t nSourceLength, std::vector< BYTE > & vResult )
{
	if( !IsValidFormat( pszFmtTag, nVersion, pSource, nSourceLength ) )
		return false;

	size_t nFmtTagLength = strlen( pszFmtTag );
	size_t nEncryptedLength = nSourceLength - nFmtTagLength - sizeof( unsigned int );

	// An empty valid file
	if( !nEncryptedLength )
	{
		vResult.clear();
		return true;
	}

	std::vector< BYTE > vBuffer( nEncryptedLength - sizeof( unsigned int ) * 2 );
	size_t nOriginalSize = 0;

	// Fucking SimpleCipher -_ - Input and Ouput should be a container.
	const std::vector< BYTE > vSource( pSource + nFmtTagLength + sizeof( unsigned int ), pSource + nSourceLength );
	SimpleCipher::Decrypt( vSource, vBuffer, &nOriginalSize );
	
	vResult.clear();
	vResult.resize( nOriginalSize );
	if( XZlibEncoder::Decode( &vBuffer.front(), vBuffer.size(), &vResult.front(), vResult.size(), &nOriginalSize ) )
	{
		assert( 0 );
		vResult.clear();
		return false;
	}

	if( nOriginalSize != vResult.size() )
	{
		assert( 0 );
		vResult.clear();
		return false;
	}

	return true;
}
