
#pragma once

#include "internal_base_include.h"


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


// renderer
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "rendererd_x64.lib" )
	#else
	#pragma comment( lib, "renderer_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "rendererd.lib" )
	#else
	#pragma comment( lib, "renderer.lib" )
	#endif
#endif


// sound
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "soundd_x64.lib" )
	#else
	#pragma comment( lib, "sound_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "soundd.lib" )
	#else
	#pragma comment( lib, "sound.lib" )
	#endif
#endif

