
#include <openssl/pem.h>
#include <openssl/rand.h>

#include "../../include/cipher/XRSA.h"

namespace
{
	int		GenerateRSACb( int p, int n, BN_GENCB* cb )
	{
		char c = '*';

		if( p == 0 ) c='.';
		if( p == 1 ) c='+';
		if( p == 2 ) c='*';
		if( p == 3 ) c='\n';

		BIO_write( static_cast< BIO* >( cb->arg ), &c, 1 );
		BIO_flush( static_cast< BIO* >( cb->arg ) );

		return 1;
	}

	bool	GenerateRSAKey( RSA* pRSA, size_t nKeyBitSize )
	{
		BN_GENCB BBGenCB;
		BIO* bio_err = NULL;
		BN_GENCB_set( &BBGenCB, GenerateRSACb, bio_err );

		// BIGNUM 생성
		BIGNUM* pBigNum = BN_new();
		if( pBigNum == NULL )
		{
			return false;
		}

		// RSA 키 생성
		if( !BN_set_word( pBigNum, RSA_F4 ) || !RSA_generate_key_ex( pRSA, static_cast< int >( nKeyBitSize ), pBigNum, &BBGenCB ) )
		{
			BN_free( pBigNum );
			return false;
		}

		BN_free( pBigNum );
		return true;
	}

	bool	SetRSAPublicKey( RSA** pRSA, const void* pKey, int nSize )
	{
		BIO* pBIOBuf = BIO_new( BIO_s_mem() );
		if( pBIOBuf == NULL )
		{
			return false;
		}

		if( BIO_write( pBIOBuf, pKey, nSize ) <= 0 )
		{
			BIO_free( pBIOBuf );
			return false;
		}

		if( !PEM_read_bio_RSA_PUBKEY( pBIOBuf, pRSA, NULL, NULL ) )
		{
			BIO_free( pBIOBuf );
			return false;
		}

		BIO_free( pBIOBuf );
		return true;
	}
}


XRSA::XRSA()
	: m_pRSA( NULL )
	, m_nMaxEnSize( 0 )
{
}

XRSA::~XRSA()
{
	if( m_pRSA != NULL )
	{
		RSA_free( m_pRSA );
		m_pRSA = NULL;
	}

	m_nMaxEnSize = 0;
}

bool	XRSA::Initialize( size_t nKeyBitSize )
{
	// RSA 생성
	if( m_pRSA != NULL )
	{
		return false;
	}

	m_pRSA = RSA_new();
	if( m_pRSA == NULL )
	{
		return false;
	}

	if( GenerateRSAKey( m_pRSA, nKeyBitSize ) == false )
	{
		RSA_free( m_pRSA );
		m_pRSA = NULL;
		return false;
	}

	m_nMaxEnSize = RSA_size( m_pRSA );
	return true;
}

bool	XRSA::Initialize( const void* pPublicKey, size_t nSize )
{
	if( m_pRSA != NULL || pPublicKey == NULL || nSize <= 0 )
	{
		return false;
	}

	m_pRSA = RSA_new();
	if( m_pRSA == NULL )
	{
		return false;
	}

	if( SetRSAPublicKey( &m_pRSA, pPublicKey, static_cast< int >( nSize ) ) == false )
	{
		RSA_free( m_pRSA );
		m_pRSA = NULL;
		return false;
	}

	m_nMaxEnSize = RSA_size( m_pRSA );
	return true;
}

bool	XRSA::GetPublicKey( void* pBuff, size_t nBuffSize, size_t* pKeySize )	const
{
	if( m_pRSA == NULL || pBuff == NULL || pKeySize == NULL )
	{
		return false;
	}

	BIO* pBIOBuf = BIO_new( BIO_s_mem() );
	if( pBIOBuf == NULL )
	{
		return false;
	}

	if( !PEM_write_bio_RSA_PUBKEY( pBIOBuf, m_pRSA ) )
	{
		BIO_free( pBIOBuf );
		return false;
	}

	if( pBIOBuf->num_write <= 0 || nBuffSize < pBIOBuf->num_write )
	{
		BIO_free( pBIOBuf );
		return false;
	}

	int nReadSize = BIO_read( pBIOBuf, pBuff, pBIOBuf->num_write );
	if( nReadSize <= 0 )
	{
		BIO_free( pBIOBuf );
		return false;
	}

	BIO_free( pBIOBuf );
	*pKeySize = nReadSize;
	return true;
}

bool	XRSA::EncryptbyPublicKey( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize )
{
	if( m_pRSA == NULL || pSrc == NULL || pDest == NULL || pEnSize == NULL )
	{
		return false;
	}

	// 정확한 SrcSize의 최대치를 구해낼 수 없다.
	// 일단 확실 한 것은 key사이즈 보다는 작다. 1024비트(128바이트)일때 117바이트라는 값이 나온다.
	// encrypt를 하면 항상 m_nMaxEnSize가 된다.
	if( nSrcSize > m_nMaxEnSize || nDestSize < m_nMaxEnSize )
	{
		return false;
	}

	const unsigned char* pIn = static_cast< const unsigned char* >( pSrc );
	unsigned char* pOut = static_cast< unsigned char* >( pDest );

	int nEnSize = RSA_public_encrypt( static_cast< int >( nSrcSize ), pIn, pOut, m_pRSA, RSA_PKCS1_PADDING );
	if( nEnSize <= 0 )
	{
		return false;
	}

	*pEnSize = nEnSize;
	return true;
}

bool	XRSA::DecryptbyPrivateKey( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize )
{
	if( m_pRSA == NULL || pSrc == NULL || pDest == NULL || pDeSize == NULL )
	{
		return false;
	}

	// decrypt 할때는 암호화되기 전 사이즈보다 크다면 성공이지만,
	// 암호화 되기 전 사이즈를 모르고 openssl은 dest 버퍼의 사이즈를 고려안해
	// 버퍼 오버플로우가 발생하니 안전하게 가자.
	if( nDestSize < nSrcSize )
	{
		return false;
	}

	const unsigned char* pIn = static_cast< const unsigned char* >( pSrc );
	unsigned char* pOut = static_cast< unsigned char* >( pDest );

	int nDeSize = RSA_private_decrypt( static_cast< int >( nSrcSize ), pIn, pOut, m_pRSA, RSA_PKCS1_PADDING );
	if( nDeSize <= 0 )
	{
		return false;
	}

	*pDeSize = nDeSize;
	return true;
}

bool	XRSA::EncryptbyPrivateKey( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pEnSize )
{
	if( m_pRSA == NULL || pSrc == NULL || pDest == NULL || pEnSize == NULL )
	{
		return false;
	}

	// 정확한 SrcSize의 최대치를 구해낼 수 없다.
	// 일단 확실 한 것은 key사이즈 보다는 작다. 1024비트(128바이트)일때 117바이트라는 값이 나온다.
	// encrypt를 하면 항상 m_nMaxEnSize가 된다.
	if( nSrcSize > m_nMaxEnSize || nDestSize < m_nMaxEnSize )
	{
		return false;
	}

	const unsigned char* pIn = static_cast< const unsigned char* >( pSrc );
	unsigned char* pOut = static_cast< unsigned char* >( pDest );

	int nEnSize = RSA_private_encrypt( static_cast< int >( nSrcSize ), pIn, pOut, m_pRSA, RSA_PKCS1_PADDING );
	if( nEnSize <= 0 )
	{
		return false;
	}

	*pEnSize = nEnSize;
	return true;
}

bool	XRSA::DecryptbyPublicKey( const void* pSrc, size_t nSrcSize, void* pDest, size_t nDestSize, size_t* pDeSize )
{
	if( m_pRSA == NULL || pSrc == NULL || pDest == NULL || pDeSize == NULL )
	{
		return false;
	}

	// decrypt 할때는 암호화되기 전 사이즈보다 크다면 성공이지만,
	// 암호화 되기 전 사이즈를 모르고 openssl은 dest 버퍼의 사이즈를 고려안해
	// 버퍼 오버플로우가 발생하니 안전하게 가자.
	if( nDestSize < nSrcSize )
	{
		return false;
	}

	const unsigned char* pIn = static_cast< const unsigned char* >( pSrc );
	unsigned char* pOut = static_cast< unsigned char* >( pDest );

	int nDeSize = RSA_public_decrypt( static_cast< int >( nSrcSize ), pIn, pOut, m_pRSA, RSA_PKCS1_PADDING );
	if( nDeSize <= 0 )
	{
		return false;
	}

	*pDeSize = nDeSize;
	return true;
}
