
#pragma once

// openssl
#ifdef _WIN64
	#pragma comment( lib, "libeay32MT_x64.lib" )
	#pragma comment( lib, "ssleay32MT_x64.lib" )
#else
	#pragma comment( lib, "libeay32MT.lib" )
	#pragma comment( lib, "ssleay32MT.lib" )
#endif

