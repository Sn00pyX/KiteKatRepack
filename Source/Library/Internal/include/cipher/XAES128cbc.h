
#pragma once

#include <openssl/evp.h>
#include "XSecretKeys.h"


class	XAES128cbc
{
public:

	XAES128cbc();
	~XAES128cbc();

	bool	Initialize( const XAES_128_CBC_KEY& Key );
	bool	Initialize( const unsigned char* pKey, const unsigned char* pIV );

	size_t	GetNeedEncryptSize( size_t nSrcSize )	const;

	bool	Encrypt( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize );
	bool	Decrypt( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize );

private:

	EVP_CIPHER_CTX	m_EnCTX;
	EVP_CIPHER_CTX	m_DnCTX;

};