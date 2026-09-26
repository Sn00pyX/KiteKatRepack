#ifndef __XZIP_H_
#define __XZIP_H_

namespace XZip
{
	extern size_t		GetBufferSize( size_t original_size );
	extern size_t		GetOriginalSize( const void *source, size_t source_len );
	extern bool			IsCompressed( const void *source, size_t source_len );
	extern size_t		Compress( const void *source, size_t source_len, void *buffer, size_t buffer_size, int ratio = -1 );
	extern bool			Uncompress( const void *source, size_t source_len, void *buffer, size_t buffer_size );
}

#endif // __XZIP_H_
