
#include "../../include/cipher/XStrZlibWithSimpleCipherUtil.h"
#include "../../include/cipher/XEncrypt.h"
#include "../../include/toolkit/XEnv.h"
#include "../../include/toolkit/safe_function.h"

std::string XStrZlibWithSimpleCipherUtil::Encrypt( const char * str )
{
	if( !strlen( str ) )
		return "";

	std::vector< BYTE > vEncrypted;
	XEncrypt::Encrypt( XEncrypt::ET_ZLIB_WITH_SIMPLE_CIPHER, XEnvStruct::ENV_FILE_FORMAT_TAG, XEnvStruct::ENV_FILE_VERSION3, str, strlen( str ), vEncrypted );

	std::string strEncrypt = "";
	for( std::vector< BYTE >::const_iterator it = vEncrypted.begin(); it != vEncrypted.end(); ++it )
	{
		char szBuf[3] = { 0 , };
		s_sprintf( szBuf, _countof( szBuf ), "%02x", (*it) );
		strEncrypt += szBuf;
	}

	return strEncrypt;
}

std::string XStrZlibWithSimpleCipherUtil::Decrypt( const char * str )
{
	if( !strlen( str ) || strlen( str ) % 2 != 0 )
		return "";

	int nContentsLength = static_cast< int >( strlen( str ) / 2 );
	BYTE * pBuffer = new BYTE[ nContentsLength + 3 ];
	struct MemoryDeallocator
	{
		MemoryDeallocator( BYTE * _pBuffer ) : pBuffer( _pBuffer ) {}
		~MemoryDeallocator() { delete [] pBuffer; }

		BYTE * pBuffer;
	} md( pBuffer );

	for( int i = 0; i < nContentsLength; ++i )
	{
		sscanf_s( &str[ 2 * i ], "%02x", &pBuffer[ i ] );
	}

	std::vector< BYTE > vDecrypted;
	XEncrypt::Decrypt( XEncrypt::ET_ZLIB_WITH_SIMPLE_CIPHER, XEnvStruct::ENV_FILE_FORMAT_TAG, XEnvStruct::ENV_FILE_VERSION3, pBuffer, nContentsLength, vDecrypted );

	std::string strDecrypt = "";
	for( std::vector< BYTE >::const_iterator it = vDecrypted.begin(); it != vDecrypted.end(); ++it )
	{
		strDecrypt += (*it);
	}

	return strDecrypt;
}