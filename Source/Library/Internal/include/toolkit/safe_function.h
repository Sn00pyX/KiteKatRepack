
#pragma once

#include <ctime>
#include <cstdlib>


//////////////////////////////////////////////////////////////////////////

int			s_sprintf( char* dest, size_t dest_len, const char* format, ... );
int			s_vsprintf( char* dest, size_t dest_len, const char* format, va_list args );

char*		s_strcpy( char* dest, size_t dest_len, const char* src );
char*		s_strncpy( char* dest, size_t dest_len, const char* src, size_t src_len );

char*		s_strcat( char* dest, size_t dest_len, const char* src );
char*		s_strncat( char* dest, size_t dest_len, const char* src, size_t append_len );

size_t		s_strlen( const char* str, size_t str_len );

char*		s_tolower( char* str, size_t str_len );
char*		s_toupper( char* str, size_t str_len );

char*		s_itoa( int value, char* str, size_t str_len, int radix );
char*		s_ltoa( long value, char* str, size_t str_len, int radix );
char*		s_ultoa( unsigned long value, char* str, size_t str_len, int radix );
char*		s_i64toa( __int64 value, char* str, size_t str_len, int radix );
char*		s_ui64toa( unsigned __int64 value, char* str, size_t str_len, int radix );

char*		s_totimestr( char* str, size_t str_len, const __time32_t* time );
char*		s_totime64str( char* str, size_t str_len, const __time64_t* time );

errno_t		s_splitpath( const char* path,
						char* drive, size_t drive_len,
						char* dir, size_t dir_len,
						char* filename, size_t filename_len,
						char* ext, size_t ext_len );
errno_t		s_makepath( char* path, size_t path_len, const char* drive, const char* dir, const char* filename, const char* ext );

char*		s_gets( char* dest, size_t dest_len );

//////////////////////////////////////////////////////////////////////////

int			s_sprintf( wchar_t* dest, size_t dest_len, const wchar_t* format, ... );
int			s_vsprintf( wchar_t* dest, size_t dest_len, const wchar_t* format, va_list args );

wchar_t*	s_strcpy( wchar_t* dest, size_t dest_len, const wchar_t* src );
wchar_t*	s_strncpy( wchar_t* dest, size_t dest_len, const wchar_t* src, size_t src_len );

wchar_t*	s_strcat( wchar_t* dest, size_t dest_len, const wchar_t* src );
wchar_t*	s_strncat( wchar_t* dest, size_t dest_len, const wchar_t* src, size_t append_len );

size_t		s_strlen( const wchar_t* str, size_t str_len );

wchar_t*	s_tolower( wchar_t* str, size_t str_len );
wchar_t*	s_toupper( wchar_t* str, size_t str_len );

wchar_t*	s_itoa( int value, wchar_t* str, size_t str_len, int radix );
wchar_t*	s_ltoa( long value, wchar_t* str, size_t str_len, int radix );
wchar_t*	s_ultoa( unsigned long value, wchar_t* str, size_t str_len, int radix );
wchar_t*	s_i64toa( __int64 value, wchar_t* str, size_t str_len, int radix );
wchar_t*	s_ui64toa( unsigned __int64 value, wchar_t* str, size_t str_len, int radix );

wchar_t*	s_totimestr( wchar_t* str, size_t str_len, const __time32_t* time );
wchar_t*	s_totime64str( wchar_t* str, size_t str_len, const __time64_t* time );

errno_t		s_splitpath( const wchar_t* path,
						wchar_t* drive, size_t drive_len,
						wchar_t* dir, size_t dir_len,
						wchar_t* filename, size_t filename_len,
						wchar_t* ext, size_t ext_len );
errno_t		s_makepath( wchar_t* path, size_t path_len, const wchar_t* drive, const wchar_t* dir, const wchar_t* filename, const wchar_t* ext );

wchar_t*	s_gets( wchar_t* dest, size_t dest_len );

//////////////////////////////////////////////////////////////////////////

void*		s_memcpy( void* dest, size_t dest_size, const void* src, size_t copy_size );
void*		s_memmove( void* dest, size_t dest_size, const void* src, size_t move_size );
