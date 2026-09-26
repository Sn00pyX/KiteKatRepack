
#pragma once

#include <openssl/ossl_typ.h>

class	XRSA
{
public:

	XRSA();
	~XRSA();

	bool	Initialize( size_t nKeyBitSize = 1024 );
	bool	Initialize( const void* pPublicKey, size_t nSize );

	bool	GetPublicKey( void* pBuff, size_t nBuffSize, size_t* pKeySize )	const;

	bool	EncryptbyPublicKey( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize );
	bool	DecryptbyPrivateKey( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize );

	bool	EncryptbyPrivateKey( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize );
	bool	DecryptbyPublicKey( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize );

private:

	RSA*	m_pRSA;

	size_t	m_nMaxEnSize;

};
