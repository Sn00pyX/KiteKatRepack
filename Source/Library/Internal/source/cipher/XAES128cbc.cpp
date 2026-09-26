
#include <openssl/aes.h>

#include "../../include/cipher/XAES128cbc.h"


XAES128cbc::XAES128cbc()
{
	EVP_CIPHER_CTX_init( &m_EnCTX );
	EVP_CIPHER_CTX_init( &m_DnCTX );
}

XAES128cbc::~XAES128cbc()
{
	EVP_CIPHER_CTX_cleanup( &m_EnCTX );
	EVP_CIPHER_CTX_cleanup( &m_DnCTX );
}

bool	XAES128cbc::Initialize( const unsigned char* pKey, const unsigned char* pIV )
{
	if( pKey == NULL || pIV == NULL )
	{
		return false;
	}

	if( !EVP_EncryptInit_ex( &m_EnCTX, EVP_aes_128_cbc(), NULL, pKey, pIV ) )
	{
		return false;
	}

	if( !EVP_DecryptInit_ex( &m_DnCTX, EVP_aes_128_cbc(), NULL, pKey, pIV ) )
	{
		return false;
	}

	return true;
}

bool	XAES128cbc::Initialize( const XAES_128_CBC_KEY& Key )
{
	return Initialize( Key.byKey, Key.byIV );
}

size_t	XAES128cbc::GetNeedEncryptSize( size_t nSrcSize )	const
{
	return ((nSrcSize/AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
}

bool	XAES128cbc::Encrypt( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize )
{
	if( pSrc == NULL || pDest == NULL || pEnSize == NULL )
	{
		return false;
	}

	size_t nNeedSize = GetNeedEncryptSize( nSrcSize );
	if( nNeedSize > nDestSize )
	{
		return false;
	}

	const unsigned char* pIn = static_cast< const unsigned char* >( pSrc );
	unsigned char* pOut = static_cast< unsigned char* >( pDest );

	if( !EVP_EncryptInit_ex( &m_EnCTX, NULL, NULL, NULL, NULL ) )
	{
		return false;
	}

	int nOutSize = 0;
	if( !EVP_EncryptUpdate( &m_EnCTX, pOut, &nOutSize, pIn, static_cast< int >( nSrcSize ) ) )
    {
        return false;
    }

	int nFinalSize = 0;
    if( !EVP_EncryptFinal_ex( &m_EnCTX, pOut + nOutSize, &nFinalSize ) )
    {
		return false;
    }

   *pEnSize = nOutSize + nFinalSize;
	return true;
}

bool	XAES128cbc::Decrypt( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize )
{
	if( pSrc == NULL || pDest == NULL || pDeSize == NULL )
	{
		return false;
	}

	// decrypt 할때는 암호화되기 전 사이즈보다 크다면 성공이지만,
	// 암호화 되기 전 사이즈를 모르고 openssl은 dest 버퍼의 사이즈를 고려안해
	// 버퍼 오버플로우가 발생하니 안전하게 가자.
	if( nSrcSize > nDestSize )
	{
		return false;
	}

	const unsigned char* pIn = static_cast< const unsigned char* >( pSrc );
	unsigned char* pOut = static_cast< unsigned char* >( pDest );

	if( !EVP_DecryptInit_ex( &m_DnCTX, NULL, NULL, NULL, NULL ) )
	{
		return false;
	}

	int nOutSize = 0;
	if( !EVP_DecryptUpdate( &m_DnCTX, pOut, &nOutSize, pIn, static_cast< int >( nSrcSize ) ) )
	{
		return false;
	}

	int nFinalSize = 0;
	if( !EVP_DecryptFinal_ex( &m_DnCTX, pOut + nOutSize, &nFinalSize ) )
	{
		return false;
	}

	*pDeSize = nOutSize + nFinalSize;
	return true;
}
