// KStream.cpp: implementation of the KStream class.
//
//////////////////////////////////////////////////////////////////////

#include "../../include/kfile/KStream.h"
#include <Windows.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//
// KStream
//
KStream::KStream()
{
	m_pMappedPtr = NULL;
	// 2010.04.29 - prodongi
	setClassId("KStream");
}

KStream::~KStream()
{
	delete[] m_pMappedPtr;
}

void* KStream::GetMappedPtr( int options )
{
	if ( m_pMappedPtr == NULL )
	{
		m_pMappedPtr = new char[ Size()+1 ];
		reinterpret_cast< char* >( m_pMappedPtr )[ Size() ] = '\0';
		size_t pos = Tell();
		Seek( 0, seekSet );
		Read( m_pMappedPtr, Size() );
		Seek( long( pos ), seekSet );
	}
	return m_pMappedPtr;
}

void KStream::FreeMappedPtr( void* baseptr )
{
	delete[] m_pMappedPtr;
	m_pMappedPtr = NULL;
}

//
// KFileStream
//
KFileStream::KFileStream( const char *filename, int mode )
{
	m_hHandle = INVALID_HANDLE_VALUE;

	Open( filename, mode );
	// 2010.04.29 - prodongi
	setClassId("KFileStream");
}

KFileStream::~KFileStream()
{
	Close();
}

bool KFileStream::Open( const char* filename, int mode )
{
	if ( m_hHandle != INVALID_HANDLE_VALUE )
		return false;

	DWORD iomode    = 0;
	DWORD sharemode = 0;
	DWORD option    = 0;
	if ( mode & rdonly )
	{
		sharemode = FILE_SHARE_READ;
		iomode = GENERIC_READ;
		option = OPEN_EXISTING;
	}
	else if ( mode & wronly )
	{
		sharemode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		iomode = GENERIC_READ | GENERIC_WRITE;
		if ( mode & truncate )
			option = CREATE_ALWAYS;
		else
			option = OPEN_ALWAYS;
	}
	else if ( mode & rdwr )
	{
		sharemode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		iomode = GENERIC_READ | GENERIC_WRITE;
		if ( mode & truncate )
			option = TRUNCATE_EXISTING;
		else
			option = OPEN_EXISTING;
	}
	m_hHandle = ::CreateFile( filename, iomode, sharemode, NULL, option, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( m_hHandle == INVALID_HANDLE_VALUE && (mode & truncate) )
	{
		option = OPEN_ALWAYS;
		m_hHandle = ::CreateFile( filename, iomode, sharemode, NULL, option, FILE_ATTRIBUTE_NORMAL, 0 );
	}
	if ( m_hHandle == INVALID_HANDLE_VALUE && ( ( mode & wronly ) || ( mode & rdwr ) ) && ( mode & forcewrite ) )
	{
		DWORD attr = ::GetFileAttributes( filename );
		DWORD tempattr = attr & ( ~FILE_ATTRIBUTE_READONLY );
		::SetFileAttributes( filename, tempattr );
		m_hHandle = ::CreateFile( filename, iomode, sharemode, NULL, option, FILE_ATTRIBUTE_NORMAL, 0 );
		::SetFileAttributes( filename, attr );
	}

	if (m_hHandle == INVALID_HANDLE_VALUE)
	{
		assert(0 && "Invalid File Handle!");
	}

	m_pMappedPtr = NULL;
	m_hMapping = INVALID_HANDLE_VALUE;

	m_nMappedCount = 0;

	return m_hHandle != INVALID_HANDLE_VALUE;
}

void KFileStream::Close()
{
	if ( m_pMappedPtr ) ::UnmapViewOfFile( m_pMappedPtr );
	if ( m_hMapping != INVALID_HANDLE_VALUE ) ::CloseHandle( m_hMapping );
	if ( m_hHandle != INVALID_HANDLE_VALUE ) ::CloseHandle( m_hHandle );

	m_pMappedPtr = NULL;
	m_hMapping = INVALID_HANDLE_VALUE;
	m_hHandle = INVALID_HANDLE_VALUE;
}

size_t KFileStream::Read( void* ptr, size_t size )
{
	DWORD readsize = 0;
	if( ::ReadFile( m_hHandle, ptr, static_cast<DWORD>(size), &readsize, NULL ) == FALSE )
	{
		assert( "ReadFile Error!" );
		return 0;
	}

	return readsize;
}

size_t KFileStream::Write( const void* ptr, size_t size )
{
	DWORD writtensize;
	if( ::WriteFile( m_hHandle, ptr, static_cast<DWORD>(size), &writtensize, NULL ) == FALSE )
	{
		assert( "WriteFile Error!" );
		return 0;
	}

	return writtensize;
}

size_t KFileStream::Seek( long offset, enum_seek_origin origin )
{
	return ::SetFilePointer( m_hHandle, offset, NULL, origin );
}

bool KFileStream::Eos() const
{
	return GetLength() == Tell();
}

size_t KFileStream::Tell() const
{
	return ::SetFilePointer( m_hHandle, 0, NULL, FILE_CURRENT );
}

size_t KFileStream::GetLength() const
{
	return ::GetFileSize(m_hHandle, NULL);
}

bool KFileStream::IsValid() const
{
	return m_hHandle != INVALID_HANDLE_VALUE;
}

void* KFileStream::GetMappedPtr( int options )
{
	DWORD mode;
	DWORD mapmode;

	m_nMappedCount++;

	if ( m_hMapping != INVALID_HANDLE_VALUE )
		return m_pMappedPtr;

	if ( options & MAPPED_WRITE )
	{
		mode = PAGE_READWRITE;
		mapmode = FILE_MAP_ALL_ACCESS;
	}
	else
	{
		mode = PAGE_READONLY;
		mapmode = FILE_MAP_READ;
	}

	m_hMapping = CreateFileMapping( m_hHandle, NULL, mode, 0, static_cast<DWORD>(GetLength()), NULL ); 
	if ( m_hMapping != INVALID_HANDLE_VALUE )
	{
		m_pMappedPtr = MapViewOfFile( m_hMapping, mapmode, 0, 0, 0 );
		return m_pMappedPtr;
	}
	return NULL;
}

void KFileStream::FreeMappedPtr( void* baseptr )
{
	if ( m_nMappedCount > 0 )
	{
		m_nMappedCount--;
		if ( m_nMappedCount <= 0 )
		{
			if ( m_pMappedPtr != NULL )
				UnmapViewOfFile( m_pMappedPtr );
			if ( m_hMapping != INVALID_HANDLE_VALUE )
				CloseHandle( m_hMapping );

			m_pMappedPtr = NULL;
			m_hMapping = INVALID_HANDLE_VALUE;
		}
	}
}


//
// KMemoryStream
//

KMemoryStream::KMemoryStream( size_t size )
{
	m_nCurPos = 0;
	m_nSize = size;

	m_bIsNeedDelete = true;

	m_pMappedPtr = new char[size+1];
	reinterpret_cast< char* >( m_pMappedPtr )[size] = '\0'; // by myunggoni

	// 2010.04.29 - prodongi
	setClassId("KMemoryStream");
}

KMemoryStream::KMemoryStream( void* buf_ptr, size_t size )
{
	m_nCurPos = 0;
	m_nSize = size;

	m_bIsNeedDelete = true;

	if ( m_nSize <= 0 )	{	m_pMappedPtr = NULL;	return;	}

	m_pMappedPtr = new char[m_nSize+1];
	s_memcpy( m_pMappedPtr, m_nSize+1, buf_ptr, m_nSize );
	reinterpret_cast< char* >( m_pMappedPtr )[m_nSize] = '\0'; // by myunggoni

	// 2010.04.29 - prodongi
	setClassId("KMemoryStream");
}

KMemoryStream::KMemoryStream( void* buf_ptr, size_t size, bool bJustLink )
{
	m_nCurPos = 0;
	m_nSize = size;

	m_bIsNeedDelete = false;

	m_pMappedPtr = buf_ptr;

	// 2010.04.29 - prodongi
	setClassId("KMemoryStream");
}

KMemoryStream::~KMemoryStream()
{
	if ( m_bIsNeedDelete )	
		delete[] m_pMappedPtr;

	m_pMappedPtr = NULL;
}

size_t KMemoryStream::Read( void* ptr, size_t size )
{
	if ( size+m_nCurPos > m_nSize )
		size = m_nSize - m_nCurPos;

	if ( size > 0 )
	{
		s_memcpy( ptr, size, reinterpret_cast< char* >( m_pMappedPtr ) + m_nCurPos, size );
		m_nCurPos += size;
		return size;
	}

	return 0;
}

size_t KMemoryStream::Write( const void* ptr, size_t size )
{
	if ( size+m_nCurPos > m_nSize )
		size = m_nSize - m_nCurPos;

	if ( size > 0 )
	{
		s_memcpy( reinterpret_cast< char* >( m_pMappedPtr ) + m_nCurPos, m_nSize - m_nCurPos, ptr, size );
		m_nCurPos += size;
		return size;
	}

	return 0;
}

size_t KMemoryStream::Seek( long offset, enum_seek_origin origin )
{
	switch( origin )
	{
	case seekSet:
		m_nCurPos = offset;
		break;
	case seekCur:
		m_nCurPos += offset;
		break;
	case seekEnd:
		m_nCurPos = m_nSize + offset;
		break;
	};

	if ( m_nCurPos < 0 ) m_nCurPos = 0;
	if ( m_nCurPos > m_nSize ) m_nCurPos = m_nSize;

	return m_nCurPos;
}

bool KMemoryStream::Eos() const
{
	return m_nCurPos == m_nSize;
}

size_t KMemoryStream::Tell() const
{
	return m_nCurPos;
}

size_t KMemoryStream::GetLength() const
{
	return m_nSize;
}

bool KMemoryStream::IsValid() const
{
	return m_pMappedPtr != NULL;
}

void* KMemoryStream::GetMappedPtr( int options )
{
	return m_pMappedPtr;
}

void KMemoryStream::FreeMappedPtr( void* baseptr )
{
	//
}

//
// KVectorStream
//
KVectorStream::KVectorStream( size_t initial_capacity, size_t boundary )
	: KMemoryStream( new char[initial_capacity], 0, true )
{
	m_nCapacity = initial_capacity;
	m_nBoundary = boundary;
}

KVectorStream::~KVectorStream()
{
	delete[] m_pMappedPtr;
}

void KVectorStream::resizeBuffer( size_t newsizebuffer )
{
	if ( newsizebuffer > m_nCapacity )
	{
		m_nCapacity = ((newsizebuffer-1)/m_nBoundary + 1)*m_nBoundary;
		char* newbuffer = new char[m_nCapacity];
		s_memcpy( newbuffer, m_nCapacity, m_pMappedPtr, m_nSize );
		delete[] m_pMappedPtr;
		m_pMappedPtr = newbuffer;
	}
	m_nSize = newsizebuffer;
}

size_t KVectorStream::Write( const void* ptr, size_t size )
{
	if ( size + m_nCurPos > m_nSize )
		resizeBuffer( size + m_nCurPos );
	s_memcpy( reinterpret_cast< char* >( m_pMappedPtr ) + m_nCurPos, m_nSize-m_nCurPos, ptr, size );
	m_nCurPos += size;
	return size;
}
