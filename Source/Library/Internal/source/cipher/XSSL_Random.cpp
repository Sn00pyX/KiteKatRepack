
#include <openssl/rand.h>

#include "../../include/cipher/XSSL_Random.h"


void	XSSL_InitRandomSeed()
{
	RAND_screen();
}

bool	XSSL_GetRandomBytes( unsigned char* pBuf, int nSize )
{
	if( !RAND_bytes( pBuf, nSize ) )
	{
		return false;
	}

	return true;
}
