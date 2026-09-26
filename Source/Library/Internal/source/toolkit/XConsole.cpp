
#include "../../include/toolkit/XConsole.h"

const int DEFAULT_CONSOLE_HEIGHT = 1024;

static void dumpConsole( XConsole & console, const char *szFileName )
{
	if ( szFileName )
	{
		FILE *fp = NULL;
		if( fopen_s( &fp, szFileName, "w" ) == 0 )
		{
			if ( fp )
			{
				for ( int i = 0; i < DEFAULT_CONSOLE_HEIGHT; i++ )
				{	
					fprintf( fp, "%s\n", console.GetLine( i ) );
				}
				fclose(fp);
			}
		}
	}
}

XConsole & XConsole::ErrorInst( const char *szFileName )
{
	static XConsole console( 120, DEFAULT_CONSOLE_HEIGHT );

	if ( szFileName ) dumpConsole( console, szFileName );

	return console;
}

XConsole & XConsole::Inst( const char *szFileName )
{
	static XConsole console( 120, DEFAULT_CONSOLE_HEIGHT );

	if ( szFileName ) dumpConsole( console, szFileName );

	return console;
}
