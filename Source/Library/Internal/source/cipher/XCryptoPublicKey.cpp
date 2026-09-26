
#include "../../include/cipher/XCryptoPublicKey.h"


XCryptoPublicKey::XCryptoPublicKey()
{
}

XCryptoPublicKey::~XCryptoPublicKey()
{
}

bool	XCryptoPublicKey::GeneratePublicKey()
{
	if( m_Local.Initialize() == false )
	{
		return false;
	}

	return true;
}

bool	XCryptoPublicKey::SetRemotePublicKey( const void* pKey, size_t nSize )
{
	if( m_Remote.Initialize( pKey, nSize ) == false )
	{
		return false;
	}

	return true;
}

bool	XCryptoPublicKey::GetLocalPublicKey( void* pBuff, size_t nBuffSize, size_t* pKeySize )	const
{
	return m_Local.GetPublicKey( pBuff, nBuffSize, pKeySize );
}

bool	XCryptoPublicKey::SetSecretKey( const XAES_128_CBC_KEY& Key )
{
	return m_AES.Initialize( Key );
}

bool	XCryptoPublicKey::EncryptForRemote( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize )
{
	return m_Remote.EncryptbyPublicKey( pSrc, nSrcSize, pDest, nDestSize, pEnSize );
}

bool	XCryptoPublicKey::DecryptForLocal( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize )
{
	return m_Local.DecryptbyPrivateKey( pSrc, nSrcSize, pDest, nDestSize, pDeSize );
}

bool	XCryptoPublicKey::EncryptForSecret( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize )
{
	return m_AES.Encrypt( pSrc, nSrcSize, pDest, nDestSize, pEnSize );
}

bool	XCryptoPublicKey::DecryptForSecret( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize )
{
	return m_AES.Decrypt( pSrc, nSrcSize, pDest, nDestSize, pDeSize );
}
