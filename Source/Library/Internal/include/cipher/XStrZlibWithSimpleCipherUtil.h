#pragma once

#include <string>


struct XStrZlibWithSimpleCipherUtil
{
	static std::string Encrypt( const char * str );
	static std::string Decrypt( const char * str );
};
