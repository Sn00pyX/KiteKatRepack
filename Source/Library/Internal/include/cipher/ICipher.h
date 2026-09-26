#pragma once

#include <memory.h>
#include "../toolkit/safe_function.h"

struct ICipher
{
	ICipher()			{}
	virtual ~ICipher()	{}

	virtual void Encode( const void* pSource, void* pTarget, unsigned target_len, bool bIsPeek = false ) = 0;
	virtual void Decode( const void* pSource, void* pTarget, unsigned target_len, bool bIsPeek = false ) = 0;
	virtual void Clear() = 0;
};

struct IDummyCipher : public ICipher
{
	virtual void Encode( const void* pSource, void* pTarget, unsigned target_len, bool bIsPeek = false )
	{
		if ( pSource == pTarget ) return;

		s_memcpy( pTarget, target_len, pSource, target_len );
	}

	virtual void Decode( const void* pSource, void* pTarget, unsigned target_len, bool bIsPeek = false )
	{
		if ( pSource == pTarget ) return;

		s_memcpy( pTarget, target_len, pSource, target_len  );
	}

	virtual void Clear()
	{
	}
};
