
#include "../../include/compress/XZip.h"
#include "../../include/toolkit/safe_function.h"

#include <cstring>
#include <cassert>

#include <zlib/zlib.h>


namespace XZip
{

	const unsigned char tag_mark[5] = { 0x00, 0x51, 0x3f, 0x99, 0x1a };

#pragma pack(1)
	struct	XZipHeader_t
	{
		unsigned char	tag[5];
		unsigned int	src_size;
		unsigned int	compress_size;
	};
#pragma pack()


	size_t	GetBufferSize( size_t original_size )
	{
		return compressBound( original_size ) + sizeof( XZipHeader_t );
	}

	size_t	GetOriginalSize( const void* source, size_t source_len )
	{
		if( IsCompressed( source, source_len ) == false )
		{
			return 0;
		}

		const XZipHeader_t* header = static_cast< const XZipHeader_t* >( source );
		return header->src_size;
	}

	bool	IsCompressed( const void *source, size_t source_len )
	{
		if( source == 0 || source_len < sizeof( XZipHeader_t ) )
		{
			return false;
		}

		const XZipHeader_t* header = static_cast< const XZipHeader_t* >( source );
		if( ::memcmp( header->tag, tag_mark, sizeof( tag_mark ) ) == 0 )
		{
			return true;
		}

		return false;
	}

	size_t	Compress( const void* source, size_t source_len, void* buffer, size_t buffer_size, int ratio )
	{
		if( source == 0 || buffer == 0 || buffer_size <= sizeof( XZipHeader_t ) )
		{
			return 0;
		}

		XZipHeader_t* header = static_cast< XZipHeader_t* >( buffer );
		s_memcpy( header->tag, sizeof( header->tag ), tag_mark, sizeof( header->tag ) );
		header->src_size = source_len;

		uLongf compress_size = buffer_size - sizeof( XZipHeader_t );
		Bytef* compress_buffer = reinterpret_cast< Bytef* >( (static_cast< char* >( buffer ) + sizeof( XZipHeader_t )) );

		if( compress2( compress_buffer, &compress_size, static_cast< const Bytef* >( source ), source_len, ratio ) != Z_OK )
		{
			return 0;
		}

		header->compress_size = compress_size;
		return header->compress_size + sizeof( XZipHeader_t );
	}

	bool	Uncompress( const void* source, size_t source_len, void* buffer, size_t buffer_size )
	{
		if( source == 0 || source_len <= sizeof( XZipHeader_t ) || buffer == 0 )
		{
			return false;
		}

		if( IsCompressed( source, source_len ) == false )
		{
			return false;
		}

		size_t org_size = GetOriginalSize( source, source_len );
		if( buffer_size < org_size )
		{
			return false;
		}

		uLongf uncompress_size = buffer_size;
		const Bytef* uncompress_buffer = reinterpret_cast< const Bytef* >( (static_cast< const char* >( source ) + sizeof( XZipHeader_t )) );

		if( uncompress( static_cast< Bytef* >( buffer ), &uncompress_size, uncompress_buffer, source_len - sizeof( XZipHeader_t ) ) != Z_OK )
		{
			return false;
		}

		assert( uncompress_size == org_size );
		return true;
	}

}

