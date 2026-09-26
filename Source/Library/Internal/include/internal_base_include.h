
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

// compress
#ifdef _WIN64
	#ifdef _DEBUG
	#pragma comment( lib, "compressd_x64.lib" )
	#else
	#pragma comment( lib, "compress_x64.lib" )
	#endif
#else
	#ifdef _DEBUG
	#pragma comment( lib, "compressd.lib" )
	#else
	#pragma comment( lib, "compress.lib" )
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
