//
//  File:       nfl.h
//
//  Contents:   wrapper of iconv unicode library and some other utility
//
//  Functions:	bool	nfl::dir( *vector, path, wildcard, dir_flag )
//				vector	nfl::dir( path, wildcard, dir_flag )
//				bool	nfl::dir( *vector, wildcard, dir_flag )
//				vector	nfl::dir( wildcard, dir_flag )
//				size_t	nfl::file_size( filename )
//				bool	nfl::is_exist( filename )
//				bool	nfl::is_file( filename )
//				bool	nfl::is_directory( filename )
//				bool	nfl::readfile( *container, path )
//				bool	nfl::textfile( *vector, path )
//				bool	nfl::textfile( *str, path )
//				str		nfl::textfile( path )
//				str		nfl::checksum( *data, size )
//				str		nfl::checksum( path )
//
//  Author:     Kang, KiHyun ( testors@nflavor.com )
//

#pragma once

#include "nsl.h"
#include <string>
#include <vector>
#include <io.h>
#include <wchar.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>

namespace nl
{
	namespace file
	{
		enum 
		{
			DEFAULT_IO_SIZE		= 65535,
		};

		namespace path	{ using namespace nsl::path;	};

		enum dir_flag
		{
			DOT_DIRECTORY		= 0x01,		// . , ..
			SUB_DIRECTORY		= 0x02,
			FILE				= 0x04,
			RECURSIVE			= 0x08,
			DIRECTORY			= ( DOT_DIRECTORY | SUB_DIRECTORY ),
			DEFAULT				= ( SUB_DIRECTORY | FILE ),
			ALL					= ( DIRECTORY | FILE ),
		};

		template< typename _Elem > struct fs_api {};

		template<>
		struct fs_api< char >
		{
			typedef _finddata_t find_data;
			static const char*		wildcard_all()			 { return "*"; }
			static ::FILE*	fopen( const char* fn, const char* mode )
			{
				::FILE* fp = NULL;
				::fopen_s( &fp, fn, mode );
				return fp;
			}
			static int      open( const char *fn, int of, int pm )
			{
				int fh = 0;
				_sopen_s( &fh, fn, of, _SH_DENYWR, pm );
				return fh;
			}
			static int      stat( const char *path, struct _stat * buf )	{ return _stat( path, buf );	}
			static intptr_t findfirst( const char *f, find_data * p )		{ return _findfirst( f, p );	}
			static intptr_t findnext( intptr_t h, find_data * p )			{ return _findnext( h, p );		}
		};

		template<>
		struct fs_api< wchar_t >
		{
			typedef _wfinddata_t find_data;
			static const wchar_t*	wildcard_all()			 { return L"*"; }
			static ::FILE*	fopen( const wchar_t* fn, const wchar_t* mode )
			{
				::FILE* fp = NULL;
				_wfopen_s( &fp, fn, mode );
				return fp;
			}
			static int      open( const wchar_t *fn, int of, int pm )
			{
				int fh = 0;
				_wsopen_s( &fh, fn, of, _SH_DENYWR, pm );
				return fh;
			}
			static int      stat( const wchar_t *path, struct _stat * buf )	{ return _wstat( path, buf );	}
			static intptr_t findfirst( const wchar_t *f, find_data * p )	{ return _wfindfirst( f, p );	}
			static intptr_t findnext( intptr_t h, find_data * p )			{ return _wfindnext( h, p );	}
		};

		struct file
		{
			file() : fp( 0 ), ref_count( 0 )							{}
			file( const char* fn, const char* mode )					{ fp = fs_api< char >::fopen( fn, mode ); ref_count = new int(1);			}
			file( const wchar_t* fn, const wchar_t* mode )				{ fp = fs_api< wchar_t >::fopen( fn, mode ); ref_count = new int(1);		}
			file( ::FILE* f, bool bAutoRelease = false )				{ fp = f; if( bAutoRelease ) ref_count = new int(1); else ref_count = 0;	}
			file( const file & rh )										{ copy_from( rh );															}

			size_t	write( const void* buf, size_t size, size_t count )	{ return fwrite( buf, size, count, fp );		}
			size_t	write( const void* buf, size_t size )				{ return fwrite( buf, size, 1, fp );			}

			size_t	read( void* buf, size_t size, size_t count )		{ return fread( buf, size, count, fp );			}
			size_t	read( void* buf, size_t size )						{ return fread( buf, size, 1, fp );				}

			file & operator<<( const char* str )						{ write( str, nsl::strlen< char >( str ) );	return *this;							}
			file & operator<<( const wchar_t* str )						{ write( str, nsl::strlen< wchar_t >( str ) * sizeof( wchar_t ) );	return *this;	}

			struct SEEK_MODE
			{
				enum { CUR = SEEK_CUR, END = SEEK_END, SET = SEEK_SET };
			};
			long	tell()												{ return ftell( fp );							}
			long	seek( long offset, int seek_mode )					{ return fseek( fp, offset, seek_mode );		}

			bool	is_valid()											{ return fp != 0;								}
			bool	close()												{ if( !fp ) return false;	
																		  fclose( fp ); fp = 0; ref_count = 0; return true;	}
			bool	flush()												{ return fflush( fp ) == 0;						}

			file & operator=( const file & rh )							{ copy_from( rh );	return *this;				}

			void copy_from( const file & rh )							{ ref_count = rh.ref_count; fp = rh.fp; if( ref_count ) ++(*ref_count);		}

			~file()
			{
				if( ref_count )
				{
					if( --(*ref_count) == 0 ) close();
				}
			}

			::FILE* & operator*()										{	return fp;	}

		private:

			int*		ref_count;
			::FILE*		fp;
		};

		template< typename _Elem >
		inline bool dir( std::vector< std::basic_string< _Elem > > * result, const _Elem * path, const _Elem * wildcard, int flag = ALL )
		{
			std::basic_string< _Elem > strParent = path::normalize( path );
			std::basic_string< _Elem > strWildcard = path::join( strParent, std::basic_string< _Elem >( fs_api< _Elem >::wildcard_all() ) );

			fs_api< _Elem >::find_data c_file;
			intptr_t hFile;

			if( (hFile = fs_api< _Elem >::findfirst( strWildcard.c_str(), &c_file )) == -1L ) return false;
			do {
			
				bool bAdd = false;
				bool bIsFile = !( c_file.attrib & _A_SUBDIR );
				bool bIsDirectory = !bIsFile;
				bool bDotDirectory = ( c_file.name[0] == '.' && c_file.name[1] == 0 ) || ( c_file.name[0] == '.' && c_file.name[1] == '.' && c_file.name[2] == 0 );

				if( bIsFile && ( flag & FILE ) )									bAdd = true;
				if( bDotDirectory && ( flag & DOT_DIRECTORY ) )						bAdd = true;
				if( bIsDirectory && !bDotDirectory && ( flag & SUB_DIRECTORY ) )	bAdd = true;

				std::basic_string< _Elem > strFileName = path::join( strParent, std::basic_string< _Elem >( c_file.name ) );

				if( bAdd && nsl::wildcmp( wildcard, c_file.name, true ) ) result->push_back( strFileName );

				if( bIsDirectory && !bDotDirectory && ( flag & RECURSIVE ) )
				{
					dir( result, strFileName.c_str(), wildcard, flag );
				}
			} while( fs_api< _Elem >::findnext( hFile, &c_file ) == 0 );
			_findclose( hFile );

			return true;
		}

		template< typename _Elem >
		inline bool dir( std::vector< std::basic_string< _Elem > > * result, const _Elem * wildcard, int flag = ALL )
		{
			return dir( result, std::basic_string< _Elem >().c_str(), wildcard, flag );
		}

		template< typename _Elem >
		inline std::vector< std::basic_string< _Elem > > dir( const _Elem * path, const _Elem * wildcard, int flag = ALL )
		{
			std::vector< std::basic_string< _Elem > > vList;
			dir( &vList, path, wildcard, flag );
			return vList;
		}

		template< typename _Elem >
		inline std::vector< std::basic_string< _Elem > > dir( const _Elem * wildcard, int flag = ALL )
		{
			std::vector< std::basic_string< _Elem > > vList;
			dir( &vList, wildcard, flag );
			return vList;
		}

		template< typename _Elem >
		inline size_t file_size( const _Elem* filename )
		{
			struct _stat st;
			if( fs_api< _Elem >::stat( filename, &st ) == -1 ) return -1;
			return st.st_size;
		}

		template< typename _Elem >
		inline bool is_exist( const _Elem* filename )
		{
			struct _stat st;
			return ( fs_api< _Elem >::stat( filename, &st ) != -1 );
		}

		template< typename _Elem >
		inline bool is_file( const _Elem* filename )
		{
			struct _stat st;
			if( fs_api< _Elem >::stat( filename, &st ) == -1 ) return false;
			return !(st.st_mode & _S_IFDIR);
		}

		template< typename _Elem >
		inline bool is_directory( const _Elem* filename )
		{
			struct _stat st;
			if( fs_api< _Elem >::stat( filename, &st ) == -1 ) return false;
			return st.st_mode & _S_IFDIR;
		}

		template< typename _Elem, typename _FileNameElem, typename _Container >
		inline bool readfile( _Container * data, const _FileNameElem *path )
		{
			size_t size = file_size( path );
			if( size == -1 ) return false;

			int fd = fs_api< _FileNameElem >::open( path, _O_RDONLY | _O_BINARY, 0 );
			if( fd == -1 ) return false;

			data->reserve( size );

			_Elem buf[ DEFAULT_IO_SIZE ];
			size_t cur = 0;
			while( size )
			{	
				int rtn = _read( fd, buf, (unsigned int )( size > sizeof(buf) ? sizeof(buf) : size ) );
				if( rtn <= 0 ) break;
				size -= rtn;
				data->insert( data->end(), buf, buf + rtn/sizeof(_Elem) );
			}
			
			_close( fd );

			return true;
		}

		template< typename _Elem, typename _FileNameElem >
		inline bool writefile( const _FileNameElem *path, const _Elem * data, size_t size )
		{
			int fd = fs_api< _FileNameElem >::open( path, _O_WRONLY | _O_CREAT | _O_BINARY | _O_TRUNC, _S_IREAD | _S_IWRITE );
			if( fd == -1 ) return false;

			size_t offset = 0;
			while( size )
			{	
				int rtn = _write( fd, &data[offset], (unsigned int)( size > DEFAULT_IO_SIZE ? sizeof(_Elem)*DEFAULT_IO_SIZE : sizeof(_Elem)*size ) );
				if( rtn <= 0 ) break;
				size -= rtn/sizeof(_Elem);
				offset += rtn/sizeof(_Elem);
			}
			
			_close( fd );

			return true;
		}

		template< typename _Elem, typename _FileNameElem >
		inline bool textfile( std::basic_string< _Elem > *str, const _FileNameElem *path )
		{
			str->clear();
			return readfile< _Elem, _FileNameElem, std::basic_string< _Elem > >( str, path );
		}

		template< typename _Elem, typename _FileNameElem >
		inline bool textfile( std::vector< _Elem > *list, const _FileNameElem *path )
		{
			std::string str;
			if( !readfile< _Elem, _FileNameElem, std::basic_string< _Elem > >( &str, path ) ) return false;
			nsl::split( str.c_str(), list, "\r\n", false );
			return true;
		}

		template< typename _Elem, typename _FileNameElem >
		inline std::basic_string< _Elem > textfile( const _FileNameElem *path )
		{
			std::basic_string< _Elem > str;
			textfile( &str, path );
			return str;
		}

		template< typename _Elem >
		inline std::basic_string< _Elem > checksum( const void *data, size_t size )
		{
			unsigned int checksum = 0;
			for( size_t i = 0; i < size; ++i )
			{
				checksum += reinterpret_cast< const unsigned char* >( data )[i];
			}
			_Elem fmt[5] = { '%', '0', '8', 'X', '\0' };
			return nsl::format( fmt, checksum );
		}

		template< typename _Elem, typename _FileNameElem >
		inline std::basic_string< _Elem > checksum( const _FileNameElem *path )
		{
			std::vector< char > file;
			if( !readfile< char >( &file, path ) ) return std::basic_string< _Elem >();
			return checksum< _Elem >( &(*file.begin()), file.size() );
		}

	};
};

namespace nfl
{
	using namespace nl::file;
	namespace path	{ using namespace nl::str::path; };
};
