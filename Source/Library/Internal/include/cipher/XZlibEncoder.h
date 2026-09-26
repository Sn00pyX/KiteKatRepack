#pragma once


namespace XZlibEncoder
{
	// Return value: 0 if successful, an error code on failure. pSource and pTarget can be same or overlapped. In that case, temporary buffer is allocated in heap memory while encoding/decoding and freed on return.
	int Encode( const void * pSource, const size_t nSourceLen, void * pTarget, const size_t nTargetLen, size_t * pEncodedLen = 0 );
	// Return value: 0 if successful, an error code on failure. pSource and pTarget can be same or overlapped. In that case, temporary buffer is allocated in heap memory while encoding/decoding and freed on return.
	int Decode( const void * pSource, const size_t nSourceLen, void * pTarget, const size_t nTargetLen, size_t * pDecodedLen = 0 );
};
