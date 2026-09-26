
#include <memory.h>

#include <zlib/zlib.h>

#include "../../include/cipher/XZlibEncoder.h"
#include "../../include/toolkit/safe_function.h"


int XZlibEncoder::Encode( const void * pSource, const size_t nSourceLen, void * pTarget, const size_t nTargetLen, size_t * pEncodedLen )
{
	if( !nSourceLen )
		return Z_DATA_ERROR;

	void * pFinalTarget = 0;
	if( !( pTarget >= ( static_cast< const char * >( pSource ) + nSourceLen ) || pSource >= ( static_cast< char * >( pTarget ) + nTargetLen ) ) )
	{
		pFinalTarget = pTarget;
		pTarget = new char [ nTargetLen ];
	}

	unsigned long nEncodedLength = static_cast< const unsigned long >( nTargetLen );

	int nErrorCode = compress( reinterpret_cast< unsigned char * >( pTarget ), &nEncodedLength,
		reinterpret_cast< const unsigned char * >( pSource ), static_cast< unsigned long >( nSourceLen ) );

	if( nErrorCode == Z_OK )
	{
		if( pEncodedLen )
		{
			*pEncodedLen = static_cast< size_t >( nEncodedLength );
		}

		if( pFinalTarget )
		{
			s_memcpy( pFinalTarget, nTargetLen, pTarget, nEncodedLength );
			delete[] pTarget;
		}
	}

	return nErrorCode;
}

int XZlibEncoder::Decode( const void * pSource, const size_t nSourceLen, void * pTarget, const size_t nTargetLen, size_t * pDecodedLen )
{
	if( !nSourceLen )
		return Z_DATA_ERROR;

	void * pFinalTarget = 0;
	if( !( pTarget >= ( static_cast< const char * >( pSource ) + nSourceLen ) || pSource >= ( static_cast< char * >( pTarget ) + nTargetLen ) ) )
	{
		pFinalTarget = pTarget;
		pTarget = new char [ nTargetLen ];
	}

	unsigned long nDecodedLength = static_cast< const unsigned long >( nTargetLen );

	int nErrorCode = uncompress( reinterpret_cast< unsigned char * >( pTarget ), &nDecodedLength,
		reinterpret_cast< const unsigned char * >( pSource ), static_cast< unsigned long >( nSourceLen ) );

	if( nErrorCode == Z_OK )
	{
		if( pDecodedLen )
		{
			*pDecodedLen = static_cast< size_t >( nDecodedLength );
		}

		if( pFinalTarget )
		{
			s_memcpy( pFinalTarget, nTargetLen, pTarget, nDecodedLength );
			delete[] pTarget;
		}
	}

	return nErrorCode;
}
