
#pragma once


#include <string>
#include <windows.h>


bool	XVerifyProcessSigner( const std::wstring& strProcessFullPath, const std::wstring& strSigner, const unsigned char* bySerial, size_t serial_size, SYSTEMTIME* timestamp, std::wstring* msg );
