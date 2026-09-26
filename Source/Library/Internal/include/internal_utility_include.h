
#pragma once

#include "internal_base_include.h"

// internet
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "internetd_x64.lib" )
	#else
	#pragma comment( lib, "internet_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "internetd.lib" )
	#else
	#pragma comment( lib, "internet.lib" )
	#endif
#endif

// rdu
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "rdud_x64.lib" )
	#else
	#pragma comment( lib, "rdu_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "rdud.lib" )
	#else
	#pragma comment( lib, "rdu.lib" )
	#endif
#endif
