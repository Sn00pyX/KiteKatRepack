#pragma once

#include <stdio.h>
#include <string.h>

#include "../../toolkit/safe_function.h"


struct PStreamReader
{
	virtual ~PStreamReader()	{}
	virtual size_t	Read( void * p, size_t len ) = 0;
	virtual bool	Seek( long pos, int cur ) = 0;
	virtual bool	IsEOF() = 0;
	virtual size_t	Tell()  = 0;
	virtual size_t	GetLength() = 0;
	virtual const char*  GetName() = 0;
};

struct PFileStreamReader : PStreamReader
{
	PFileStreamReader( const char *szFileName )
	{
		if( szFileName ) s_strcpy( szName, _countof( szName ), szFileName );
		fp = NULL;
		fopen_s( &fp, szFileName, "rb" );

		fseek( fp, 0, SEEK_END );
        Length = ftell(fp);                        
        fseek( fp, 0, SEEK_SET );

	}
	virtual ~PFileStreamReader()
	{
		if ( fp ) 
		{
			fclose( fp );
		}
	}
	size_t Read( void * p, size_t len )
	{
		if ( p ) return fread( p, 1, len, fp );
		
		if ( fseek( fp, (long)len, SEEK_CUR ) ) return static_cast< size_t >( -1 );

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

	size_t	Tell()
	{
		return ftell( fp);
	}

	size_t	GetLength()
	{
		return Length;
	}
	const char*  GetName()
	{
		return szName;
	}


private:
	char    szName[256];
	size_t  Length;
	FILE    *fp;
};