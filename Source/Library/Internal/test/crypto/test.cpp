// Time.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <openssl_lib_include.h>
#include <cstring>
#include <cassert>
#include <ctime>
#include <Windows.h>

#include "../../include/cipher/XRSA.h"
#include "../../include/cipher/XAES128cbc.h"
#include "../../include/cipher/XSecretKeyGenerater.h"



int _tmain(int argc, _TCHAR* argv[])
{
	XRSA local;
	assert( local.Initialize() == true );

	XRSA remote;

	unsigned char key[1024];
	size_t key_len = 0;
	assert( local.GetPublicKey( key, sizeof( key ), &key_len ) == true );
	assert( remote.Initialize( key, key_len ) == true );

	// max src size
	{
		srand( (unsigned int)time( NULL ) );

		unsigned char en_bytes[1024];
		size_t en_size = 0;

		char src[1024];
		for( int i = 0; i < _countof( src ); ++i )
		{
			src[i] = (char)rand();
			if( local.EncryptbyPrivateKey( src, i, en_bytes, sizeof( en_bytes ), &en_size ) == true )
			{
				printf( "src_size: %d, en_size: %d\n", i, en_size );
			}
			else
			{
				break;
			}
		}
	}

	// public key로 암호화하면 private key로만 풀수있다.
	{
		char src[100] = "Hello~!!!!";
		size_t src_size = ::strlen( src );
		unsigned char en_bytes[128];
		size_t en_size = 0;

		assert( local.EncryptbyPublicKey( src, src_size, en_bytes, sizeof( en_bytes ), &en_size ) == true );
		assert( remote.EncryptbyPublicKey( src, src_size, en_bytes, sizeof( en_bytes ), &en_size ) == true );

		char en_bytes_short[5];
		size_t en_size_short = 0;
		assert( local.EncryptbyPublicKey( src, src_size, en_bytes_short, sizeof( en_bytes_short ), &en_size_short ) == false );

		char de_src[1024];
		size_t de_size = 0;
		assert( local.DecryptbyPrivateKey( en_bytes, en_size, de_src, sizeof( de_src ), &de_size ) == true );
		de_src[de_size] = '\0';
		assert( strcmp( de_src, src ) == 0 );

		assert( local.DecryptbyPublicKey( en_bytes, en_size, de_src, sizeof( de_src ), &de_size ) == false );

		// 작은 사이즈
		char de_src_short[11];
		assert( local.DecryptbyPrivateKey( en_bytes, en_size, de_src_short, sizeof( de_src_short ), &de_size ) == false );

		// 손상
		en_bytes[0] = en_bytes[0]+100;
		assert( local.DecryptbyPrivateKey( en_bytes, en_size, de_src, sizeof( de_src ), &de_size ) == false );
	}

	// private key로 암호화하면 public key로만 풀수있다.
	{
		char src[100] = "Hello~!!!!";
		unsigned char en_bytes[1024];
		size_t en_size = 0;

		assert( local.EncryptbyPrivateKey( src, strlen( src ), en_bytes, sizeof( en_bytes ), &en_size ) == true );

		char de_src[1024];
		size_t de_size = 0;
		assert( local.DecryptbyPublicKey( en_bytes, en_size, de_src, sizeof( de_src ), &de_size ) == true );
		assert( de_size == strlen( src ) );
		de_src[de_size] = '\0';
		assert( strcmp( de_src, src ) == 0 );

		assert( remote.DecryptbyPublicKey( en_bytes, en_size, de_src, sizeof( de_src ), &de_size ) == true );
		assert( de_size == strlen( src ) );
		de_src[de_size] = '\0';
		assert( strcmp( de_src, src ) == 0 );

		assert( local.DecryptbyPrivateKey( en_bytes, en_size, de_src, sizeof( de_src ), &de_size ) == false );

		// 손상
		en_bytes[0] = en_bytes[0]+100;
		assert( local.DecryptbyPublicKey( en_bytes, en_size, de_src, sizeof( de_src ), &de_size ) == false );
	}

	{
		XAES128cbc aes;
		assert( aes.Initialize( XSecretKeyGenerater::Instance().GenerateAES128cbc() ) == true );

		char dummy[32];
		char dest[48];
		size_t en_size = 0;

		char de_dest[48];
		size_t de_size = 0;

		assert( aes.Encrypt( dummy, sizeof( dummy ), dest, sizeof( dest ), &en_size ) == true );
		assert( aes.Decrypt( dest, en_size, de_dest, sizeof( de_dest ), &de_size ) == true );
		assert( de_size == sizeof( dummy ) );
		assert( memcmp( dummy, de_dest, de_size ) == 0 );

		// 손상
		dest[0] = dest[0]+100;
		assert( aes.Decrypt( dest, en_size, de_dest, sizeof( de_dest ), &de_size ) == true );
		assert( memcmp( dummy, de_dest, sizeof( dummy ) ) != 0 );

	}

	{
		printf( "key generater run test\n" );
		DWORD dwStart = ::GetTickCount();
		for( int i = 0; i < 10000; ++i )
		{
			XAES_128_CBC_KEY Key = XSecretKeyGenerater::Instance().GenerateAES128cbc();
		}
		printf( "runtime: %d\n", ::GetTickCount() - dwStart );
	}


	{
		printf( "AES encrypt run test\n" );

		XAES128cbc aes;
		assert( aes.Initialize( XSecretKeyGenerater::Instance().GenerateAES128cbc() ) == true );

		char dummy[1024];
		char dest[2048];
		size_t en_size = 0;

		DWORD dwStart = ::GetTickCount();
		for( int i = 0; i < 10000; ++i )
		{
			aes.Encrypt( dummy, sizeof( dummy ), dest, sizeof( dest ), &en_size );
		}
		printf( "runtime: %d\n", ::GetTickCount() - dwStart );
	}

	{
		printf( "AES encrypt run test\n" );

		XAES128cbc aes;
		assert( aes.Initialize( XSecretKeyGenerater::Instance().GenerateAES128cbc() ) == true );

		char dummy[1024];
		char dest[2048];
		size_t en_size = 0;

		char de_dest[2048];
		size_t de_size = 0;

		assert( aes.Encrypt( dummy, sizeof( dummy ), dest, sizeof( dest ), &en_size ) == true );

		DWORD dwStart = ::GetTickCount();
		for( int i = 0; i < 10000; ++i )
		{
			aes.Decrypt( dest, en_size, de_dest, sizeof( de_dest ), &de_size );
		}
		printf( "runtime: %d\n", ::GetTickCount() - dwStart );
	}

	return 0;
}

