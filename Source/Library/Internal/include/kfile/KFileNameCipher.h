#pragma once

#include <string>

namespace KFileNameCipher
{
	bool IsEncodedName( const std::string & strFileName );
	void EncodeFileName( std::string & strFileName );
	void DecodeFileName( std::string & strFileName );
};