
#include "../../include/cipher/XRc4Cipher.h"
#include "../../include/cipher/rc4_cipher.h"


struct XRC4Cipher::TImpl : public RC4Cipher 
{};


XRC4Cipher::XRC4Cipher()
	: m_strKey( "Neat & Simple" )
{
	m_pImpl = new TImpl();
	Clear();
}

XRC4Cipher::~XRC4Cipher()
{
	delete m_pImpl;
}

void XRC4Cipher::SetKey( const char* pKey )
{
	m_strKey = pKey;
	m_pImpl->init( m_strKey.c_str(), 0 );
}

void XRC4Cipher::tryCipher( const void* pSource, void* pTarget, unsigned len )
{
	RC4Cipher::State backup;
	m_pImpl->saveStateTo( backup );

	doCipher( pSource, pTarget, len );

	m_pImpl->loadStateFrom( backup );
}

void XRC4Cipher::doCipher( const void* pSource, void* pTarget, unsigned len )
{
	m_pImpl->code( reinterpret_cast< const unsigned char* >( pSource ), reinterpret_cast< unsigned char* >( pTarget ), len );
}

void XRC4Cipher::Encode( const void* pSource, void* pTarget, unsigned len, bool bIsPeek )
{
	if ( bIsPeek )
		tryCipher( pSource, pTarget, len );
	else
		doCipher( pSource, pTarget, len );
}

void XRC4Cipher::Decode( const void* pSource, void* pTarget, unsigned len, bool bIsPeek )
{
	if ( bIsPeek )
		tryCipher( pSource, pTarget, len );
	else
		doCipher( pSource, pTarget, len );
}

void XRC4Cipher::Clear()
{
	m_pImpl->init( m_strKey.c_str(), 0 );
}

