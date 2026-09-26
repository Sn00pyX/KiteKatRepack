
#pragma once

/*

	XFlag

	by Testors, 2004-05-13

	bit flag 템플릿.

	ex)

		XFlag< int, 4 >	flag;	// int 4개 크기의 플래그 생성.

		flag.On(0);				// 0번째 플래그 on

		if ( flag[0] ) blah();	// 0번째 플래그가 세팅되어 있으면 blah() 실행.

		flag.Off(0);			// 0번째 플래그 off

		flag.Clear();			// 모든 플래그 청소

		char buf[sizeof(int)*4];				
		flag.CopyTo( buf, sizeof(buf) );	// buf 에 플래그 복사

		flag.CopyFrom( buf );	// buf 로부터 플래그 복사

		int a[4];
		s_memcpy( &a, sizeof( a ), flag.GetRawData(), sizeof(int)*4 );		// a 에 플래그 복사

*/


template< typename T = int, unsigned COUNT = 1 >
struct XFlag
{
	enum
	{
		BITS_PER_TYPE = ( sizeof( T ) * 8 ),
		TOTAL_BITS	  = ( sizeof( T ) * 8 * COUNT )
	};

	XFlag()
	{
		Clear();
	}

	inline void Clear()
	{
		memset( m_flag, 0, sizeof( m_flag ) );
	}

	inline bool IsOn( unsigned idx ) const
	{
		if ( ( 1 << (idx % BITS_PER_TYPE) ) & m_flag[ ( idx / BITS_PER_TYPE ) ] ) return true;

		return false;
	}

	inline const bool operator[] ( unsigned idx ) const
	{
		return IsOn( idx );
	}

	inline void On( unsigned idx )
	{
		m_flag[ idx / BITS_PER_TYPE ] |= ( 1 << ( idx % BITS_PER_TYPE ) );
	}

	inline void Off( unsigned idx )
	{
		m_flag[ idx / BITS_PER_TYPE ] &= ~( 1 << ( idx % BITS_PER_TYPE ) );
	}

	inline void Set( const unsigned idx, const bool on )
	{
		if( on )
			On( idx );
		else
			Off( idx );
	}

	inline bool CopyFrom( const void* buf )
	{
		s_memcpy( m_flag, sizeof( m_flag ), buf, sizeof( m_flag ) );

		return true;
	}

	inline bool CopyTo( void* buf, size_t buff_size ) const
	{
		if ( buff_size < sizeof( m_flag ) ) return false;

		s_memcpy( buf, buff_size, m_flag, sizeof( m_flag ) );

		return true;
	}

	inline void Assign( const T& val )
	{
		for ( int i = 0; i < COUNT; i++ )
			m_flag[ i ] = val;
	}

	inline void Assign( int n, const T& val )
	{
		m_flag[ n ] = val;
	}

	inline const T* GetRawData() const 
	{
		return &m_flag[ 0 ];
	}

protected:

	T			m_flag[ COUNT ];
};
