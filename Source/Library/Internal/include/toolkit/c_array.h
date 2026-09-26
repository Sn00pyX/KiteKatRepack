#pragma once

#include <stddef.h>

//
// Simple run-time fixed length array (4 byte footprint)
//
//    Note : data is destroyed by resizing

template< typename T >
class c_array
{
public:

	c_array()
	{
		m_pBuf = NULL;
#ifdef _DEBUG
		m_size = 0;
#endif
	}

	c_array( unsigned int n )
	{
		m_pBuf = new T[ n ];
#ifdef _DEBUG
		m_size = n;
#endif
	}

	~c_array()
	{
		delete[] m_pBuf;
	}

	operator const T* () const
	{
		return m_pBuf;
	}

	operator T* ()
	{
		return m_pBuf;
	}

	// like std::vector

	const T& front() const
	{
		return *m_pBuf;
	}

	T& front() 
	{
		return *m_pBuf;
	}

	void resize( unsigned int n )
	{
		if ( m_pBuf )
			delete[] m_pBuf;
		m_pBuf = new T[ n ];

#ifdef _DEBUG
		m_size = n;
#endif
	}

#ifdef _DEBUG
	unsigned int size() const 
	{ 
		return m_size; 
	}
#endif

private:
	c_array( const c_array<T>& );
	c_array<T> operator = ( const c_array<T>& );

	T* m_pBuf;

#ifdef _DEBUG
	unsigned int m_size;
#endif
};

//
// Simple run-time variable length array (8 byte footprint)
//

template< typename T >
class c_varray 
{
public:

	c_varray()
	{
		m_pBuf = NULL;
		m_size = 0;
	}

	c_varray( unsigned int n )
	{
		m_pBuf = new T[ n ];
		m_size = n;
	}

	~c_varray()
	{
		delete[] m_pBuf;
	}

	unsigned int size() const
	{
		return m_size;
	}

	operator const T* () const
	{
		return m_pBuf;
	}

	operator T* ()
	{
		return m_pBuf;
	}

	// like std::vector

	const T& front() const
	{
		return *m_pBuf;
	}

	T& front() 
	{
		return *m_pBuf;
	}

	const T& back() const
	{
		return m_pBuf[ m_size-1 ];
	}

	T& back() 
	{
		return m_pBuf[ m_size-1 ];
	}

	void resize( unsigned int n )
	{
		if ( n > m_size || n < m_size / 2 )
		{
			T* pNewBuf = new T[ n ];
			s_memcpy( pNewBuf, n*sizeof( T ), m_pBuf, (n < m_size ? n : m_size) * sizeof(T) );
			if ( m_pBuf )
				delete[] m_pBuf;
			m_pBuf = pNewBuf;
		}
		m_size = n;
	}

private:
	c_varray( const c_varray<T>& );
	c_varray<T>& operator = ( const c_varray<T>& );

	T* m_pBuf;
	unsigned int m_size;
};

//
// c_varray with reference counting
//

template< typename T >
class c_rarray 
{
public:

	c_rarray()
	{
		m_pBuf = NULL;
		m_pRefCount = NULL;
		m_size = 0;
	}

	c_rarray( unsigned int n )
	{
		m_pBuf = new T[ n ];
		m_pRefCount = NULL;
		m_size = n;

		_addref();
	}

	c_rarray( const c_rarray< T >& rhs )
	{
		m_pBuf = NULL;
		m_pRefCount = NULL;
		m_size = 0;

		_copy( rhs );
	}

	~c_rarray()
	{
		_release();
	}

	unsigned int size() const
	{
		return m_size;
	}

	operator const T* () const
	{
		return m_pBuf;
	}

	operator T* ()
	{
		return m_pBuf;
	}

	c_rarray< T >& operator= ( const c_rarray< T >& rhs )
	{
		_copy( rhs );

		return *this;
	}

	// like std::vector

	const T& front() const
	{
		return *m_pBuf;
	}

	T& front() 
	{
		return *m_pBuf;
	}

	const T& back() const
	{
		return m_pBuf[ m_size - 1 ];
	}

	T& back()
	{
		return m_pBuf[ m_size - 1 ];
	}

	void resize( unsigned int n )
	{
		if ( n != m_size )
		{
			T* pNewBuf = new T[ n ];
			s_memcpy( pNewBuf, n*sizeof( T ), m_pBuf, (m_size < n ? m_size : n) * sizeof(T) );

			_release();

			m_pBuf = pNewBuf;
			m_pRefCount = NULL;
			m_size = n;

			_addref();
		}
	}

private:

	void _addref()
	{
		if( !m_pBuf )
			return;

		if( !m_pRefCount )
		{
			m_pRefCount = new int;
			*m_pRefCount = 0;
		}

		( *m_pRefCount )++;
	}

	void _release()
	{
		if( !m_pRefCount )
			return;

		( *m_pRefCount )--;

		if( ( *m_pRefCount ) == 0 )
		{
			delete[] m_pBuf;
			m_pBuf = NULL;

			delete m_pRefCount;
			m_pRefCount = NULL;

			m_size = 0;
		}
	}

	void _copy( const c_rarray< T >& rhs )
	{
		_release();

		m_pBuf = rhs.m_pBuf;
		m_pRefCount = rhs.m_pRefCount;
		m_size = rhs.m_size;

		_addref();
	}

	T* m_pBuf;
	int* m_pRefCount;
	unsigned int m_size;
};

