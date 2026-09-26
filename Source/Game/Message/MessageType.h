
#pragma once

#include <toolkit/XRandom.h>
#include <toolkit/c_encint.h>

struct c_enc_rand_gen
{
	static short generate()
	{
		return short( XRandom() % 65536 );
	};
};

typedef c_encint< int, short, c_enc_rand_gen> ENC_INT;