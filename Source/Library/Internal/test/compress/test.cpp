
#include <tchar.h>
#include <string>

#include "../../include/compress/XZip.h"
#include "../../include/internal_base_include.h"

#include <external_lib_include.h>

int _tmain()
{
	FILE *fp = NULL;
	fopen_s( &fp, "n@UB6,~='fyKZ(45mt)D7AuKU", "rb" );
	if( fp == NULL )
	{
		return 0;
	}

	fseek( fp, 0, SEEK_END );
	size_t file_size = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	char* file_buffer = new char[file_size];

	fread( file_buffer, file_size, 1, fp );

	fclose( fp );

	// 압축 해제
	size_t original_size = XZip::GetOriginalSize( file_buffer, file_size );
	char *pNewBuffer = new char[ original_size ];
	if( !XZip::Uncompress( file_buffer, file_size, pNewBuffer, original_size ) )
	{
		delete [] pNewBuffer;
		return 0;
	}

	// 크기 검사
	if( original_size != 4 )
	{
		delete [] pNewBuffer;
		return 0;
	}

	return 0;
}

