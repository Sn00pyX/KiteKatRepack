
#pragma once

// freetype
#ifdef _WIN64
#else
	#pragma comment( lib, "freetype.lib" )
#endif

// iconv
#ifdef _WIN64
#else
	#pragma comment( lib, "libiconv.lib" )
#endif


// greta
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "gretad_x64.lib" )
	#else
	#pragma comment( lib, "greta_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "gretad.lib" )
	#else
	#pragma comment( lib, "greta.lib" )
	#endif
#endif


// lua
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "luad_x64.lib" )
	#else
	#pragma comment( lib, "lua_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "luad.lib" )
	#else
	#pragma comment( lib, "lua.lib" )
	#endif
#endif


// tinyxml2
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "tinyxml2d_x64.lib" )
	#else
	#pragma comment( lib, "tinyxml2_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "tinyxml2d.lib" )
	#else
	#pragma comment( lib, "tinyxml2.lib" )
	#endif
#endif


// zlib
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "zlibd_x64.lib" )
	#else
	#pragma comment( lib, "zlib_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "zlibd.lib" )
	#else
	#pragma comment( lib, "zlib.lib" )
	#endif
#endif


#include "openssl_lib_include.h"

