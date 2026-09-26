
#pragma once

#include "internal_base_include.h"

// framework
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "frameworkd_x64.lib" )
	#else
	#pragma comment( lib, "framework_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "frameworkd.lib" )
	#else
	#pragma comment( lib, "framework.lib" )
	#endif
#endif

// mmo
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "mmod_x64.lib" )
	#else
	#pragma comment( lib, "mmo_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "mmod.lib" )
	#else
	#pragma comment( lib, "mmo.lib" )
	#endif
#endif
