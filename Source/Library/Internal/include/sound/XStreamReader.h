#pragma once

#include <stdio.h>
#include <string.h>

#include "../toolkit/safe_function.h"

struct XStreamReader
{
	virtual ~XStreamReader()	{}
	virtual size_t	Read( void * p, size_t len ) = 0;
	virtual bool	Seek( long pos, int cur ) = 0;
	virtual bool	IsEOF() = 0;
};

struct XFileStreamReader : XStreamReader
{
	XFileStreamReader( const char *szFileName )
	{	
		if( szFileName ) s_strcpy( szName, _countof( szName ), szFileName );
		fp = NULL;
		fopen_s( &fp, szFileName, "rb" );
	}
	virtual ~XFileStreamReader()
	{
		if( fp ) 
		{
			fclose( fp );
		}
	}
	size_t Read( void * p, size_t len )
	{
		if( p ) return fread( p, 1, len, fp );
		
		if( fseek( fp, (long)len, SEEK_CUR ) ) return static_cast< size_t >( -1 );

		return len;
	}
	bool IsEOF()
	{
		return !!feof( fp );
	}
	bool Seek( long pos, int cur )
	{
		return !fseek( fp, pos, cur );
	}

private:
	char szName[256];
	FILE *fp;
};


