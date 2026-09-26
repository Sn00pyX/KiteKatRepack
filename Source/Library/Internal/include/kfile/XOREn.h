
#pragma once

namespace XOREn
{
	extern const unsigned char szResourceEncodeKey[];
	extern const unsigned int unResourceEncodeKey[];

	inline unsigned int GetEncodeKeyInt( size_t index )
	{
		return unResourceEncodeKey[ ( index / sizeof(int) ) % 64 ];
	}

	inline unsigned char GetEncodeKeyChar( size_t index )
	{
		return szResourceEncodeKey[ index % 256 ];
	}
}