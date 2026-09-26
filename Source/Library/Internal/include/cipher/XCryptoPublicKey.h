
#pragma once


#include "XRSA.h"
#include "XAES128cbc.h"


class	XCryptoPublicKey
{
public:

	XCryptoPublicKey();
	~XCryptoPublicKey();

	bool	GeneratePublicKey();
	bool	SetRemotePublicKey( const void* pKey, size_t nSize );

	bool	GetLocalPublicKey( void* pBuff, size_t nBuffSize, size_t* pKeySize )	const;

	bool	SetSecretKey( const XAES_128_CBC_KEY& Key );

	bool	EncryptForRemote( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize );
	bool	DecryptForLocal( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize );

	bool	EncryptForSecret( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize );
	bool	DecryptForSecret( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize );

private:

	XRSA		m_Local;
	XRSA		m_Remote;

	XAES128cbc	m_AES;

};
