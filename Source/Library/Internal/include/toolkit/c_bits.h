#ifndef __C_BITS_H__
#define __C_BITS_H__

#include <stdlib.h>
#include <assert.h>

/*
 *    Fixed-size bit flag type with set expression constructors
 *
 *            2006.9.29 by Young-Hyun Joo
 */

template< int N = 32 >
class c_bits
{
public:

	//struct OR {};				// 'or' is the default operator.
	struct NOT {};
	struct AND {};
	struct XOR {};
	struct MINUS {};

	c_bits()													{}
	c_bits( bool bFillValue )									{ if ( bFillValue ) set(); else reset(); }
	c_bits( const c_bits<N>& s )								{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = s.m_flags[i]; }
	c_bits( const void* pBuf, int bufSize )						{ read_from( pBuf, bufSize ); }

	explicit c_bits( int e )									{ reset(); set(e); }
	c_bits( NOT, int e )										{ set(); reset(e); }
	c_bits( int e1, int e2 )									{ reset(); set(e1); set(e2); }
	c_bits( int e1, int e2, int e3 )							{ reset(); set(e1); set(e2); set(e3); }
	c_bits( int e1, int e2, int e3, int e4 )					{ reset(); set(e1); set(e2); set(e3); set(e4); }
	c_bits( int e1, int e2, int e3, int e4, int e5 )			{ reset(); set(e1); set(e2); set(e3); set(e4); set(e5); }
	c_bits( int e1, int e2, int e3, int e4, int e5, int e6 )	{ reset(); set(e1); set(e2); set(e3); set(e4); set(e5); set(e6);  }
	c_bits( int e, const c_bits<N>& s )							{ *this = s; set(e); }
	c_bits( int e, AND, const c_bits<N>& s )					{ reset(); if ( s[e] ) set(e); }
	c_bits( int e, MINUS, const c_bits<N>& s )					{ reset(); if ( s[e] ) reset(e); }
	c_bits( int e, XOR, const c_bits<N>& s )					{ *this = s; flip(e); }
	c_bits( const c_bits<N>& s, int e )							{ *this = s; set(e); }
	c_bits( const c_bits<N>& s, AND, int e )					{ reset(); if ( s[e] ) set(e); }
	c_bits( const c_bits<N>& s, MINUS, int e )					{ *this = s; reset(e); }
	c_bits( const c_bits<N>& s, XOR, int e )					{ *this = s; flip(e); }
	c_bits( NOT, const c_bits<N>& s )							{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = ~s.m_flags[i]; _n<N%BITS>(this); }
	c_bits( const c_bits<N>& s1, const c_bits<N>& s2 )			{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = s1.m_flags[i] | s2.m_flags[i]; }
	c_bits( const c_bits<N>& s1, AND, const c_bits<N>& s2 )		{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = s1.m_flags[i] & s2.m_flags[i]; }
	c_bits( const c_bits<N>& s1, MINUS, const c_bits<N>& s2 )	{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = s1.m_flags[i] & ~s2.m_flags[i]; }
	c_bits( const c_bits<N>& s1, XOR, const c_bits<N>& s2 )		{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = s1.m_flags[i] ^ s2.m_flags[i]; }

	static int size()											{ return N; }

	int count() const											{ return _count(); }

	bool operator ! () const									{ return none(); }
	bool any() const											{ for ( int i = 0; i < SIZE; i++ ) if ( m_flags[i] ) return true; return false; }
	bool none() const											{ for ( int i = 0; i < SIZE; i++ ) if ( m_flags[i] ) return false; return true; }

	bool operator [] ( int e ) const							{ assert(e>=0 && e<BSIZE); return (m_flags[ e/BITS ] & (1 << (e%BITS))) != 0; }
	bool operator [] ( const c_bits<N>& s ) const				{ for ( int i = 0; i < SIZE; i++ ) if ( m_flags[i] & s.m_flags[i] ) return true; return false; }
	bool test( int e ) const									{ assert(e>=0 && e<BSIZE); return (m_flags[ e/BITS ] & (1 << (e%BITS))) != 0; }
	bool test( const c_bits<N>& s ) const						{ for ( int i = 0; i < SIZE; i++ ) if ( m_flags[i] & s.m_flags[i] ) return true; return false; }

	c_bits<N>& set( int e )										{ assert(e>=0 && e<BSIZE); m_flags[ e/BITS ] |= (1 << (e%BITS)); return *this; }
	c_bits<N>& reset( int e )									{ assert(e>=0 && e<BSIZE); m_flags[ e/BITS ] &= ~(1 << (e%BITS)); return *this; }
	c_bits<N>& flip( int e )									{ assert(e>=0 && e<BSIZE); m_flags[ e/BITS ] ^= (1 << (e%BITS)); return *this; }

	c_bits<N>& set( const c_bits<N>& s )						{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] |= s.m_flags[i]; return *this; }
	c_bits<N>& reset( const c_bits<N>& s )						{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] &= ~s.m_flags[i]; return *this; }
	c_bits<N>& flip( const c_bits<N>& s )						{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] ^= s.m_flags[i]; return *this; }

	c_bits<N>& set()											{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = ~0; _n<N%BITS>(this); return *this; }
	c_bits<N>& reset()											{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = 0; return *this; }
	c_bits<N>& flip()											{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = ~m_flags[i]; _n<N%BITS>(this); return *this; }

	c_bits<N>& operator = ( int e )								{ reset(); set(e); return *this; }
	c_bits<N>& operator &= ( int e )							{ if ( (*this)[e] ) *this = e; else reset(); return *this; }
	c_bits<N>& operator |= ( int e )							{ return set(e); }
	c_bits<N>& operator ^= ( int e )							{ return flip(e); }

	c_bits<N>& operator = ( const c_bits<N>& s )				{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] = s.m_flags[i]; return *this; }
	c_bits<N>& operator &= ( const c_bits<N>& s )				{ for ( int i = 0; i < SIZE; i++ ) m_flags[i] &= s.m_flags[i]; return *this; }
	c_bits<N>& operator |= ( const c_bits<N>& s )				{ return set(s); }
	c_bits<N>& operator ^= ( const c_bits<N>& s )				{ return flip(s); }

	bool operator == ( const c_bits<N>& s ) const				{ for ( int i = 0; i < SIZE; i++ ) if ( m_flags[i] != s.m_flags[i] ) return false; return true; }
	bool operator != ( const c_bits<N>& s ) const				{ for ( int i = 0; i < SIZE; i++ ) if ( m_flags[i] != s.m_flags[i] ) return true; return false; }

	c_bits<N> operator & ( int e ) const						{ return c_bits<N>( *this, AND(), e ); }
	c_bits<N> operator | ( int e ) const						{ return c_bits<N>( *this, e ); }
	c_bits<N> operator ^ ( int e ) const						{ return c_bits<N>( *this, XOR(), e ); }

	c_bits<N> operator & ( const c_bits<N>& s ) const			{ return c_bits<N>( *this, AND(), s ); }
	c_bits<N> operator | ( const c_bits<N>& s ) const			{ return c_bits<N>( *this, s ); }
	c_bits<N> operator ^ ( const c_bits<N>& s ) const			{ return c_bits<N>( *this, XOR(), s ); }
	c_bits<N> operator ~ () const								{ return c_bits<N>( NOT(), *this ); }

	int read_from( const void* pBuf, int bufSize )
	{
		int size = (bufSize >= 0 && bufSize < SIZE * sizeof(int) ) ? bufSize : SIZE * sizeof(int);
		s_memcpy( m_flags, sizeof( m_flags ), pBuf, size );
		_n<N%BITS>(this); 
		return size;
	}

	int write_to( void* pBuf, int bufSize ) const
	{
		int size = (bufSize >= 0 && bufSize < SIZE * sizeof(int) ) ? bufSize : SIZE * sizeof(int);
		s_memcpy( pBuf, bufSize, m_flags, size );
		return size;
	}

	unsigned to_uint() const
	{
		assert( N <= sizeof(unsigned) * 8 );
		return m_flags[0];
	}

	void from_uint( unsigned flags )
	{
		assert( N <= sizeof(unsigned) * 8 );
		m_flags[0] = flags;
		_n<N%BITS>(this); 
	}

	template< typename T >
	void for_each_on( T fn )
	{
		struct FnPass { unsigned operator () ( unsigned n ) { return n; } };
		_for_each( fn, FnPass() );
	}

	template< typename T >
	void for_each_off( T fn )
	{
		struct FnInv { unsigned operator () ( unsigned n ) { return ~n; } };
		_for_each( fn, FnInv() );
	}

private:

	/// very dangerous! 
	operator bool () const;

	enum {
		BITS = sizeof(unsigned)*8,
		SIZE = (N+BITS-1) / BITS,
		BSIZE = SIZE * BITS,
		LAST_MASK = (N % BITS) ? (1 << (N % BITS)) - 1 : ~0
	};

	int _count() const
	{
		static const char bitCount[ 16 ] = { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 };
		int c = 0;
		for ( int i = 0; i < SIZE-1; i++ )
			for ( unsigned f = m_flags[i]; f != 0; f /= 16 )
				c += bitCount[ f % 16 ];

		for ( unsigned f = m_flags[ SIZE-1 ] & LAST_MASK; f != 0; f /= 16 )
			c += bitCount[ f % 16 ];

		return c;
	}

	template< typename T, typename OP >
	void _for_each( T fn, OP op )
	{
		for ( int i = 0, bit = 0; i < SIZE; i++, bit += BITS )
		{
			unsigned f = op( m_flags[i] );
			if ( i == SIZE-1 )
				f &= LAST_MASK;
			if ( f == 0 )
				continue;

			for ( int j = 0; f != 0; j+=4, f /= 16 )
			{
				if ( f % 16 == 0 )
					continue;

				if ( f&1 ) fn( bit+j   );
				if ( f&2 ) fn( bit+j+1 );
				if ( f&4 ) fn( bit+j+2 );
				if ( f&8 ) fn( bit+j+3 );
			}
		}
	}

	template< int M > struct _n		{ _n( c_bits<N>* p ) { p->m_flags[ SIZE-1 ] &= LAST_MASK; } };
	template<> struct _n< 0 >		{ _n( c_bits<N>* p ) {} };

	unsigned m_flags[ SIZE ];
};

#endif