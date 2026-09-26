
#pragma once


// cipher
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "cipherd_x64.lib" )
	#else
	#pragma comment( lib, "cipher_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "cipherd.lib" )
	#else
	#pragma comment( lib, "cipher.lib" )
	#endif
#endif

// dump
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "dumpd_x64.lib" )
	#else
	#pragma comment( lib, "dump_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "dumpd.lib" )
	#else
	#pragma comment( lib, "dump.lib" )
	#endif
#endif

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

// kfile
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "kfiled_x64.lib" )
	#else
	#pragma comment( lib, "kfile_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "kfiled.lib" )
	#else
	#pragma comment( lib, "kfile.lib" )
	#endif
#endif

// logging
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "loggingd_x64.lib" )
	#else
	#pragma comment( lib, "logging_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "loggingd.lib" )
	#else
	#pragma comment( lib, "logging.lib" )
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

// network
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "networkd_x64.lib" )
	#else
	#pragma comment( lib, "network_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "networkd.lib" )
	#else
	#pragma comment( lib, "network.lib" )
	#endif
#endif

// toolkit
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "toolkitd_x64.lib" )
	#else
	#pragma comment( lib, "toolkit_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "toolkitd.lib" )
	#else
	#pragma comment( lib, "toolkit.lib" )
	#endif
#endif
