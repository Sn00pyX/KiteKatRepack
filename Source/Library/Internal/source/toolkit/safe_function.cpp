
#pragma	warning( push )
#pragma warning( disable : 4996 )
#include <strsafe.h>
#pragma	warning( pop )

#include <cassert>

#include "../../include/toolkit/safe_function.h"


#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
#define	SUPPORT_SAFE_FUNCTION
#endif


//////////////////////////////////////////////////////////////////////////
int		s_sprintf( char* dest, size_t dest_len, const char* format, ... )
{
	va_list args;
	va_start( args, format );

	int ret = s_vsprintf( dest, dest_len, format, args );

	va_end( args );

	return ret;
}

int		s_vsprintf( char* dest, size_t dest_len, const char* format, va_list args )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( format != NULL );

	int ret = -1;
	if( dest != NULL && dest_len > 0 )
	{
		if( format != NULL )
		{
			char* end = NULL;
			size_t remaining = 0;

			HRESULT result = ::StringCchVPrintfExA( dest, dest_len, &end, &remaining, 0, format, args );

			assert( SUCCEEDED( result ) );
			assert( dest_len >= remaining );

			if( SUCCEEDED( result ) && dest_len >= remaining )
			{
				ret = int( dest_len - remaining );
			}
		}
		else
		{
			dest[0] = '\0';
			ret = 0;
		}
	}

	return ret;
}

char*	s_strcpy( char* dest, size_t dest_len, const char* src )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( src != NULL );

	char* end = dest;
	if( dest != NULL && dest_len > 0 )
	{
		if( src != NULL )
		{
			size_t remaining = 0;

#ifdef	_DEBUG
			HRESULT result =
#endif

			::StringCchCopyExA( dest, dest_len, src, &end, &remaining, 0 );

#ifdef	_DEBUG
			assert( SUCCEEDED( result ) );
#endif
		}
		else
		{
			dest[0] = '\0';
		}
	}

	return end;
}

char*	s_strncpy( char* dest, size_t dest_len, const char* src, size_t src_len )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( src != NULL );

	char* end = dest;
	if( dest != NULL && dest_len > 0 )
	{
		if( src != NULL && src_len > 0 )
		{
			size_t remaining = 0;

#ifdef	_DEBUG
			HRESULT result =
#endif

			::StringCchCopyNExA( dest, dest_len, src, src_len, &end, &remaining, 0 );

#ifdef	_DEBUG
			assert( SUCCEEDED( result ) );
#endif
		}
		else
		{
			dest[0] = '\0';
		}
	}

	return end;
}

char*	s_strcat( char* dest, size_t dest_len, const char* src )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( src != NULL );

	char* end = dest;
	if( dest != NULL && dest_len > 0 )
	{
		if( src != NULL )
		{
			size_t remaining = 0;

#ifdef	_DEBUG
			HRESULT result =
#endif

			::StringCchCatExA( dest, dest_len, src, &end, &remaining, 0 );

#ifdef	_DEBUG
			assert( SUCCEEDED( result ) );
#endif
		}
		else
		{
			end = dest + s_strlen( dest, dest_len );
		}
	}

	return end;
}

char*	s_strncat( char* dest, size_t dest_len, const char* src, size_t append_len )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( src != NULL );

	char* end = dest;
	if( dest != NULL && dest_len > 0 )
	{
		if( src != NULL && append_len > 0 )
		{
			size_t remaining = 0;

#ifdef	_DEBUG
			HRESULT result =
#endif

			::StringCchCatNExA( dest, dest_len, src, append_len, &end, &remaining, 0 );

#ifdef	_DEBUG
			assert( SUCCEEDED( result ) );
#endif
		}
		else
		{
			end = dest + s_strlen( dest, dest_len );
		}
	}

	return end;
}

size_t	s_strlen( const char* str, size_t str_len )
{
	assert( str != NULL );
	assert( str_len > 0 );

	size_t count = 0;
	if( str != NULL && str_len > 0 )
	{
#ifdef	_DEBUG
		HRESULT result =
#endif

		::StringCchLengthA( str, str_len, &count );

#ifdef	_DEBUG
		assert( SUCCEEDED( result ) );
#endif
	}

	return count;
}

char*	s_tolower( char* str, size_t str_len )
{
	assert( str != NULL );
	assert( str_len > 0 );
	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
#ifdef	_DEBUG
		errno_t error =
#endif
		::_strlwr_s( str, str_len );

#ifdef	_DEBUG
		 assert( error == 0 );
#endif
#else
		::_strlwr( str );
#endif
	}

	return str;
}

char*	s_toupper( char* str, size_t str_len )
{
	assert( str != NULL );
	assert( str_len > 0 );
	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
#ifdef	_DEBUG
		errno_t error =
#endif

		::_strupr_s( str, str_len );

#ifdef	_DEBUG
		assert( error == 0 );
#endif
#else
		::_strupr( str );
#endif
	}

	return str;
}

char*	s_itoa( int value, char* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );
	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_itoa_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = '\0';
		}
#else
		::_itoa( value, str, radix );
#endif
	}

	return str;
}

char*	s_ltoa( long value, char* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );
	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_ltoa_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = '\0';
		}
#else
		::_ltoa( value, str, radix );
#endif
	}

	return str;
}

char*	s_ultoa( unsigned long value, char* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );
	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_ultoa_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = '\0';
		}
#else
		::_ultoa( value, str, radix );
#endif
	}

	return str;
}

char*	s_i64toa( __int64 value, char* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );
	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_i64toa_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = '\0';
		}
#else
		::_i64toa( value, str, radix );
#endif
	}

	return str;
}

char*	s_ui64toa( unsigned __int64 value, char* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );
	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_ui64toa_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = '\0';
		}
#else
		::_ui64toa( value, str, radix );
#endif
	}

	return str;
}

char*	s_totimestr( char* str, size_t str_len, const __time32_t* time )
{
	assert( str != NULL );
	assert( str_len > 0 );
	assert( time != NULL );
	if( str != NULL && str_len > 0 )
	{
		if( time != NULL )
		{
#ifdef	SUPPORT_SAFE_FUNCTION
			errno_t error = ::_ctime32_s( str, str_len, time );
			assert( error == 0 );
			if( error != 0 )
			{
				str[0] = '\0';
			}
#else
			s_strcpy( str, str_len, ::_ctime32( time ) );
#endif
		}
		else
		{
			str[0] = '\0';
		}
	}

	return str;
}

char*	s_totime64str( char* str, size_t str_len, const __time64_t* time )
{
	assert( str != NULL );
	assert( str_len > 0 );
	assert( time != NULL );
	if( str != NULL && str_len > 0 )
	{
		if( time != NULL )
		{
#ifdef	SUPPORT_SAFE_FUNCTION
			errno_t error = ::_ctime64_s( str, str_len, time );
			assert( error == 0 );
			if( error != 0 )
			{
				str[0] = '\0';
			}
#else
			s_strcpy( str, str_len, ::_ctime64( time ) );
#endif
		}
		else
		{
			str[0] = '\0';
		}
	}

	return str;
}

errno_t	s_splitpath( const char* path,
					char* drive, size_t drive_len,
					char* dir, size_t dir_len,
					char* filename, size_t filename_len,
					char* ext, size_t ext_len )
{
	assert( path != NULL );
	errno_t error = -1;
	if( path != NULL )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		error = ::_splitpath_s( path, drive, drive_len, dir, dir_len, filename, filename_len, ext, ext_len );
		assert( error == 0 );
		if( error != 0 )
		{
			if( drive != NULL && drive_len > 0 )		drive[0] = '\0';
			if( dir != NULL && dir_len > 0 )			dir[0] = '\0';
			if( filename != NULL && filename_len > 0 )	filename[0] = '\0';
			if( ext != NULL && ext_len > 0 )			ext[0] = '\0';
		}
#else
		(drive_len);
		(dir_len);
		(filename_len);
		(ext_len);
		::_splitpath( path, drive, dir, filename, ext );
		error = 0;
#endif
	}
	else
	{
		if( drive != NULL && drive_len > 0 )		drive[0] = '\0';
		if( dir != NULL && dir_len > 0 )			dir[0] = '\0';
		if( filename != NULL && filename_len > 0 )	filename[0] = '\0';
		if( ext != NULL && ext_len > 0 )			ext[0] = '\0';
	}

	return error;
}

errno_t	s_makepath( char* path, size_t path_len, const char* drive, const char* dir, const char* filename, const char* ext )
{
	assert( path != NULL );
	assert( path_len > 0 );
	errno_t error = -1;
	if( path != NULL && path_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		error = ::_makepath_s( path, path_len, drive, dir, filename, ext );
		assert( error == 0 );
		if( error != 0 )
		{
			path[0] = '\0';
		}
#else
		::_makepath( path, drive, dir, filename, ext );
		error = 0;
#endif
	}

	return error;
}

char*	s_gets( char* dest, size_t dest_len )
{
	assert( dest != NULL );
	assert( dest_len > 0 );

	if( dest != NULL && dest_len > 0 )
	{
		size_t remaining = 0;
		char* end = NULL;

#ifdef	_DEBUG
		HRESULT result =
#endif

		::StringCchGetsExA( dest, dest_len, &end, &remaining, 0 );

#ifdef	_DEBUG
		assert( SUCCEEDED( result ) );
#endif
	}

	return dest;
}

//////////////////////////////////////////////////////////////////////////

int		s_sprintf( wchar_t* dest, size_t dest_len, const wchar_t* format, ... )
{
	va_list args;
	va_start( args, format );

	int ret = s_vsprintf( dest, dest_len, format, args );

	va_end( args );

	return ret;
}

int		s_vsprintf( wchar_t* dest, size_t dest_len, const wchar_t* format, va_list args )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( format != NULL );

	int ret = -1;
	if( dest != NULL && dest_len > 0 )
	{
		if( format != NULL )
		{
			wchar_t* end = NULL;
			size_t remaining = 0;

			HRESULT result = ::StringCchVPrintfExW( dest, dest_len, &end, &remaining, 0, format, args );

			assert( SUCCEEDED( result ) );
			assert( dest_len >= remaining );

			if( SUCCEEDED( result ) && dest_len >= remaining )
			{
				ret = int( dest_len - remaining );
			}
		}
		else
		{
			dest[0] = L'\0';
			ret = 0;
		}
	}

	return ret;
}

wchar_t*	s_strcpy( wchar_t* dest, size_t dest_len, const wchar_t* src )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( src != NULL );

	wchar_t* end = dest;
	if( dest != NULL && dest_len > 0 )
	{
		if( src != NULL )
		{
			size_t remaining = 0;

#ifdef	_DEBUG
			HRESULT result =
#endif

			::StringCchCopyExW( dest, dest_len, src, &end, &remaining, 0 );

#ifdef	_DEBUG
			assert( SUCCEEDED( result ) );
#endif
		}
		else
		{
			dest[0] = L'\0';
		}
	}

	return end;
}

wchar_t*	s_strncpy( wchar_t* dest, size_t dest_len, const wchar_t* src, size_t src_len )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( src != NULL );

	wchar_t* end = dest;
	if( dest != NULL && dest_len > 0 )
	{
		if( src != NULL && src_len > 0 )
		{
			size_t remaining = 0;

#ifdef	_DEBUG
			HRESULT result =
#endif

			::StringCchCopyNExW( dest, dest_len, src, src_len, &end, &remaining, 0 );

#ifdef	_DEBUG
			assert( SUCCEEDED( result ) );
#endif
		}
		else
		{
			dest[0] = L'\0';
		}
	}

	return end;
}

wchar_t*	s_strcat( wchar_t* dest, size_t dest_len, const wchar_t* src )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( src != NULL );

	wchar_t* end = dest;
	if( dest != NULL && dest_len > 0 )
	{
		if( src != NULL )
		{
			size_t remaining = 0;

#ifdef	_DEBUG
			HRESULT result =
#endif

			::StringCchCatExW( dest, dest_len, src, &end, &remaining, 0 );

#ifdef	_DEBUG
			assert( SUCCEEDED( result ) );
#endif
		}
		else
		{
			end = dest + s_strlen( dest, dest_len );
		}
	}

	return end;
}

wchar_t*	s_strncat( wchar_t* dest, size_t dest_len, const wchar_t* src, size_t append_len )
{
	assert( dest != NULL );
	assert( dest_len > 0 );
	assert( src != NULL );

	wchar_t* end = dest;
	if( dest != NULL && dest_len > 0 )
	{
		if( src != NULL && append_len > 0 )
		{
			size_t remaining = 0;

#ifdef	_DEBUG
			HRESULT result =
#endif

			::StringCchCatNExW( dest, dest_len, src, append_len, &end, &remaining, 0 );

#ifdef	_DEBUG
			assert( SUCCEEDED( result ) );
#endif
		}
		else
		{
			end = dest + s_strlen( dest, dest_len );
		}
	}

	return end;
}

size_t	s_strlen( const wchar_t* str, size_t str_len )
{
	assert( str != NULL );
	assert( str_len > 0 );

	size_t count = 0;
	if( str != NULL && str_len > 0 )
	{
#ifdef	_DEBUG
		HRESULT result =
#endif

		::StringCchLengthW( str, str_len, &count );

#ifdef	_DEBUG
		assert( SUCCEEDED( result ) );
#endif
	}

	return count;
}

wchar_t*	s_tolower( wchar_t* str, size_t str_len )
{
	assert( str != NULL );
	assert( str_len > 0 );

	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
#ifdef	_DEBUG
		errno_t error =
#endif

		::_wcslwr_s( str, str_len );

#ifdef	_DEBUG
		assert( error == 0 );
#endif
#else
		::_wcslwr( str );
#endif
	}

	return str;
}

wchar_t*	s_toupper( wchar_t* str, size_t str_len )
{
	assert( str != NULL );
	assert( str_len > 0 );

	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
#ifdef	_DEBUG
		errno_t error =
#endif

		::_wcsupr_s( str, str_len );

#ifdef	_DEBUG
		assert( error == 0 );
#endif
#else
		::_wcsupr( str );
#endif
	}

	return str;
}

wchar_t*	s_itoa( int value, wchar_t* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );

	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_itow_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = L'\0';
		}
#else
		::_itow( value, str, radix );
#endif
	}

	return str;
}

wchar_t*	s_ltoa( long value, wchar_t* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );

	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_ltow_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = L'\0';
		}
#else
		::_ltow( value, str, radix );
#endif
	}

	return str;
}

wchar_t*	s_ultoa( unsigned long value, wchar_t* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );

	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_ultow_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = L'\0';
		}
#else
		::_ultow( value, str, radix );
#endif
	}

	return str;
}

wchar_t*	s_i64toa( __int64 value, wchar_t* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );

	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_i64tow_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = L'\0';
		}
#else
		::_i64tow( value, str, radix );
#endif
	}

	return str;
}

wchar_t*	s_ui64toa( unsigned __int64 value, wchar_t* str, size_t str_len, int radix )
{
	assert( str != NULL );
	assert( str_len > 0 );

	if( str != NULL && str_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		errno_t error = ::_ui64tow_s( value, str, str_len, radix );
		assert( error == 0 );
		if( error != 0 )
		{
			str[0] = L'\0';
		}
#else
		::_ui64tow( value, str, radix );
#endif
	}

	return str;
}

wchar_t*	s_totimestr( wchar_t* str, size_t str_len, const __time32_t* time )
{
	assert( str != NULL );
	assert( str_len > 0 );
	assert( time != NULL );
	if( str != NULL && str_len > 0 )
	{
		if( time != NULL )
		{
#ifdef	SUPPORT_SAFE_FUNCTION
			errno_t error = ::_wctime32_s( str, str_len, time );
			assert( error == 0 );
			if( error != 0 )
			{
				str[0] = L'\0';
			}
#else
			s_strcpy( str, str_len, ::_wctime32( time ) );
#endif
		}
		else
		{
			str[0] = L'\0';
		}
	}

	return str;
}

wchar_t*	s_totime64str( wchar_t* str, size_t str_len, const __time64_t* time )
{
	assert( str != NULL );
	assert( str_len > 0 );
	assert( time != NULL );
	if( str != NULL && str_len > 0 )
	{
		if( time != NULL )
		{
#ifdef	SUPPORT_SAFE_FUNCTION
			errno_t error = ::_wctime64_s( str, str_len, time );
			assert( error == 0 );
			if( error != 0 )
			{
				str[0] = L'\0';
			}
#else
			s_strcpy( str, str_len, ::_wctime64( time ) );
#endif
		}
		else
		{
			str[0] = L'\0';
		}
	}

	return str;
}

errno_t	s_splitpath( const wchar_t* path,
					wchar_t* drive, size_t drive_len,
					wchar_t* dir, size_t dir_len,
					wchar_t* filename, size_t filename_len,
					wchar_t* ext, size_t ext_len )
{
	assert( path != NULL );
	errno_t error = -1;
	if( path != NULL )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		error = ::_wsplitpath_s( path, drive, drive_len, dir, dir_len, filename, filename_len, ext, ext_len );
		assert( error == 0 );
		if( error != 0 )
		{
			if( drive != NULL && drive_len > 0 )		drive[0] = '\0';
			if( dir != NULL && dir_len > 0 )			dir[0] = '\0';
			if( filename != NULL && filename_len > 0 )	filename[0] = '\0';
			if( ext != NULL && ext_len > 0 )			ext[0] = '\0';
		}
#else
		(drive_len);
		(dir_len);
		(filename_len);
		(ext_len);
		::_wsplitpath( path, drive, dir, filename, ext );
		error = 0;
#endif
	}
	else
	{
		if( drive != NULL && drive_len > 0 )		drive[0] = '\0';
		if( dir != NULL && dir_len > 0 )			dir[0] = '\0';
		if( filename != NULL && filename_len > 0 )	filename[0] = '\0';
		if( ext != NULL && ext_len > 0 )			ext[0] = '\0';
	}

	return error;
}

errno_t	s_makepath( wchar_t* path, size_t path_len, const wchar_t* drive, const wchar_t* dir, const wchar_t* filename, const wchar_t* ext )
{
	assert( path != NULL );
	assert( path_len > 0 );
	errno_t error = -1;
	if( path != NULL && path_len > 0 )
	{
#ifdef	SUPPORT_SAFE_FUNCTION
		error = ::_wmakepath_s( path, path_len, drive, dir, filename, ext );
		assert( error == 0 );
		if( error != 0 )
		{
			path[0] = '\0';
		}
#else
		::_wmakepath( path, drive, dir, filename, ext );
		error = 0;
#endif
	}

	return error;
}

wchar_t*	s_gets( wchar_t* dest, size_t dest_len )
{
	assert( dest != NULL );
	assert( dest_len > 0 );

	if( dest != NULL && dest_len > 0 )
	{
		size_t remaining = 0;
		wchar_t* end = NULL;

#ifdef	_DEBUG
		HRESULT result =
#endif

		::StringCchGetsExW( dest, dest_len, &end, &remaining, 0 );

#ifdef	_DEBUG
		assert( SUCCEEDED( result ) );
#endif
	}

	return dest;
}

//////////////////////////////////////////////////////////////////////////
void*	s_memcpy( void* dest, size_t dest_size, const void* src, size_t copy_size )
{
	assert( dest != NULL );
	//assert( src != NULL );
	assert( dest_size >= copy_size );

	if( src != NULL && copy_size > 0 )
	{
		if( dest != NULL && dest_size >= copy_size )
		{
#ifdef	SUPPORT_SAFE_FUNCTION
#ifdef	_DEBUG
			errno_t error =
#endif

			::memcpy_s( dest, dest_size, src, copy_size );

#ifdef	_DEBUG
			assert( error == 0 );
#endif
#else
			::memcpy( dest, src, copy_size );
#endif
		}
	}

	return dest;
}

void*	s_memmove( void* dest, size_t dest_size, const void* src, size_t move_size )
{
	assert( dest != NULL );
	assert( src != NULL );
	assert( dest_size >= move_size );

	if( src != NULL && move_size > 0 )
	{
		if( dest != NULL && dest_size >= move_size )
		{
#ifdef	SUPPORT_SAFE_FUNCTION
#ifdef	_DEBUG
			errno_t error =
#endif

			::memmove_s( dest, dest_size, src, move_size );

#ifdef	_DEBUG
			assert( error == 0 );
#endif
#else
			::memmove( dest, src, move_size );
#endif
		}
	}

	return dest;
}
