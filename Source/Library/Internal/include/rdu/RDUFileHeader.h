
#pragma once

#include <cassert>

#include "../toolkit/safe_function.h"


const static char*		RDUFILE_HEADER_TAG		= "RC_RDU";
const static char		RDUFILE_MAJOR_VERSION	= 1;
const static char		RDUFILE_MINOR_VERSION	= 1;

struct RDU_FILE_HEADER
{
	static const unsigned int		FILE_TAG_LENGTH		= 6;

	RDU_FILE_HEADER( const char *pszFileTag = "", const unsigned char _nMajorVersion = 0, const unsigned char _nMinorVersion = 0 )
		: nMajorVersion( _nMajorVersion )
		, nMinorVersion( _nMinorVersion )
	{
		memset( szFileTag, 0, sizeof( szFileTag ) );

		size_t nFileTagLength = strlen( pszFileTag );
		if( !nFileTagLength )
			return;

		assert( nFileTagLength <= FILE_TAG_LENGTH );

		s_memcpy( szFileTag, sizeof( szFileTag ), pszFileTag, nFileTagLength );
	}

	char			szFileTag[ FILE_TAG_LENGTH ];
	unsigned char	nMajorVersion;
	unsigned char	nMinorVersion;
};
