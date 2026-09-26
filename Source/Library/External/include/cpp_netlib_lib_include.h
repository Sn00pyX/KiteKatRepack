
#pragma once

#include "openssl_lib_include.h"

// cpp-netlib client only
#ifdef _WIN64
	#ifdef	_DEBUG
	#pragma comment( lib, "cppnetlib-client-connectionsd_x64.lib" )
	#pragma comment( lib, "cppnetlib-urid_x64.lib" )
	#pragma comment( lib, "cppnetlib-server-parsersd_x64.lib" )
	#else
	#pragma comment( lib, "cppnetlib-client-connections_x64.lib" )
	#pragma comment( lib, "cppnetlib-uri_x64.lib" )
	#pragma comment( lib, "cppnetlib-server-parsers_x64.lib" )
	#endif
#else
	#ifdef	_DEBUG
	#pragma comment( lib, "cppnetlib-client-connectionsd.lib" )
	#pragma comment( lib, "cppnetlib-urid.lib" )
	#pragma comment( lib, "cppnetlib-server-parsersd.lib" )
	#else
	#pragma comment( lib, "cppnetlib-client-connections.lib" )
	#pragma comment( lib, "cppnetlib-uri.lib" )
	#pragma comment( lib, "cppnetlib-server-parsers.lib" )
	#endif
#endif

