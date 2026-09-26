
#pragma once

#include "XSecretKeys.h"


class	XSecretKeyGenerater
{
public:

	static XSecretKeyGenerater&	Instance();

	XAES_128_CBC_KEY	GenerateAES128cbc()	const;

private:

	XSecretKeyGenerater();
	~XSecretKeyGenerater();

};
