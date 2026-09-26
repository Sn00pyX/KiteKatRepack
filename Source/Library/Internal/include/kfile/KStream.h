#ifndef __KSTREAM_H__
#define __KSTREAM_H__

// KStream.h: interface for the KStream class.
//
//////////////////////////////////////////////////////////////////////

#include <cstddef>

/// 2010.04.29 typeid - prodongi
#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>

#include "../toolkit/safe_function.h"

#define CLASSID_SIZE 64

class KStream  
{
public:

	KStream();
	virtual ~KStream();

	enum enum_seek_origin
	{
		seekSet = 0,
		seekCur = 1,
		seekEnd = 2,
	};

	virtual size_t Read( void* ptr, size_t size ) = 0;
	virtual size_t Write( const void* ptr, size_t size ) = 0;
	virtual size_t Seek( long offset, enum_seek_origin origin ) = 0;
	virtual size_t Tell() const = 0;
	virtual size_t GetLength() const = 0;
	size_t Size() const													{ return GetLength(); }

	virtual bool Eos() const											{ return Tell() >= GetLength(); }
	virtual bool IsValid() const										{ return true; }

	enum
	{
		MAPPED_READ = 1,
		MAPPED_WRITE = 2,
	};

	virtual void* GetMappedPtr( int options );
	virtual void FreeMappedPtr( void* baseptr );

	/// 2010.04.29 typeid - prodongi
	char const* getClassId() const				{	return m_classId;						}
	bool isClassId(char const* classId) const	{	return strcmp(m_classId, classId) == 0;	}
	virtual size_t getOriginalSize() const							{	return Size();	}
	virtual size_t getOriginalSize(size_t /*originalSize*/) const	{	return 0;		}

protected:
	/// 2010.04.29 typeid - prodongi
	void setClassId(char const* id)				{	s_strcpy(m_classId, _countof( m_classId ), id );	}

protected:

	void* m_pMappedPtr;

private:
	/// 2010.04.29 typeid - prodongi
	char m_classId[CLASSID_SIZE];
};


class KFileStream : public KStream
{
public:

	enum enum_file_mode
	{
		rdonly		= 1,
		wronly		= 2,
		rdwr		= 4,
		truncate	= 8,
		forcewrite	= 16,
	};
	
	KFileStream( const char* filename, int mode = rdonly );
	virtual ~KFileStream();

	bool Open( const char* filename, int mode = rdonly );
	void Close();

	virtual size_t Read( void* ptr, size_t size );
	virtual size_t Write( const void* ptr, size_t size );
	virtual size_t Seek( long offset, enum_seek_origin origin );
	virtual bool Eos() const;
	virtual size_t Tell() const;
	virtual size_t GetLength() const;
	virtual bool IsValid() const;

	virtual void* GetMappedPtr( int options );
	virtual void FreeMappedPtr( void* baseptr );

private:

	int		m_nMappedCount;
	void*	m_hMapping;
	void*	m_hHandle;
};


class KMemoryStream : public KStream
{
public:

	KMemoryStream( size_t size );
	KMemoryStream( void* buf_ptr, size_t size );
	KMemoryStream( void* buf_ptr, size_t size, bool bJustLink );
	virtual ~KMemoryStream();
	
	virtual size_t Read( void* ptr, size_t size );
	virtual size_t Write( const void* ptr, size_t size );
	virtual size_t Seek( long offset, enum_seek_origin origin );
	virtual bool Eos() const;
	virtual size_t Tell() const;
	virtual size_t GetLength() const;
	virtual bool IsValid() const;

	virtual void* GetMappedPtr( int options );
	virtual void FreeMappedPtr( void* baseptr );

protected:

	size_t	m_nCurPos;
	size_t	m_nSize;
	bool	m_bIsNeedDelete;
};


struct KRealFile : public KStream
{
	KRealFile( FILE *_fp )
		: fp( _fp ) 
	{
		// 2010.04.29 - prodongi
		setClassId("KRealFile");
	}
	~KRealFile()												{ fclose( fp ); }

	virtual size_t Read( void *pBuf, size_t read_size )			{ return fread( pBuf, 1, read_size, fp );	}
	virtual size_t Write( const void *pBuf, size_t write_size )	{ return fwrite( pBuf, 1, write_size, fp );	}
	virtual size_t Seek( long offset, enum_seek_origin origin ) { return fseek( fp, offset, origin );		}
	virtual size_t Tell() const									{ return ftell( fp );						}
	virtual size_t GetLength() const
	{
		int fd = _fileno( fp );
		struct stat st;
		memset( &st, 0, sizeof(st) );
		fstat( fd, &st );
		return st.st_size;
	}
	virtual bool Eos() const									{ return !!feof( fp ); }
	virtual bool IsValid() const								{ return fp != NULL; }

	FILE *fp;
};



class KVectorStream : public KMemoryStream
{
public:

	KVectorStream( size_t initial_capacity = 256, size_t boundary = 128 );
	virtual ~KVectorStream();

	virtual size_t Write( const void* ptr, size_t size );

private:

	void resizeBuffer( size_t newsizebuffer );

	size_t	m_nCapacity;
	size_t	m_nBoundary;
};

#endif
