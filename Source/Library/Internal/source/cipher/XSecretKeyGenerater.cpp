
#include "../../include/cipher/XSecretKeyGenerater.h"
#include "../../include/cipher/XSSL_Random.h"


XSecretKeyGenerater&	XSecretKeyGenerater::Instance()
{
	static XSecretKeyGenerater generater;
	return generater;
}

XSecretKeyGenerater::XSecretKeyGenerater()
{
	XSSL_InitRandomSeed();
}

XSecretKeyGenerater::~XSecretKeyGenerater()
{
}

XAES_128_CBC_KEY	XSecretKeyGenerater::GenerateAES128cbc()	const
{
	XAES_128_CBC_KEY Key;
	XSSL_GetRandomBytes( Key.byKey, sizeof( Key.byKey ) );
	XSSL_GetRandomBytes( Key.byIV, sizeof( Key.byIV ) );
	return Key;
}

