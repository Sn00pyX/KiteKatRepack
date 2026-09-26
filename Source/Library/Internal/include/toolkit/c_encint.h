#pragma once
#pragma pack(push, 1)

// !! SHORT은 반드시 INT SIZE의 절반이어야 함.
// !! RD는 아래와 같은 static 함수를 가지는 type이다.
// static SHORT generate()

template< typename INT, typename SHORT, typename RD >
class c_encint
{
public:	
	c_encint()																		{}
	c_encint( const c_encint<INT, SHORT, RD>& n )									{ m_H = n.m_H; m_L = n.m_L; }
	c_encint<INT, SHORT, RD>& operator = ( const c_encint<INT, SHORT, RD>& n )		{ m_H = n.m_H; m_L = n.m_L; return *this; }
	operator INT () const															{ return dec(); }
	INT value() const																{ return dec(); }

	__forceinline SHORT hash_id() const
	{
		return sum() - sub();
	}

	__forceinline c_encint<INT, SHORT, RD>& set_new_value( const c_encint<INT, SHORT, RD>& n )
	{
		m_H = n.m_H;
		m_L = n.m_L;

		SHORT r1 = RD::generate();
		SHORT r2 = RD::generate();

		m_H.l = SHORT( m_H.l - r1 - r1 + r2 + r2 );
		m_L.l = SHORT( m_L.l - r1 - r1 - r2 - r2 );

		m_H.l = SHORT( m_H.l + m_H.h + m_H.h - m_L.h - m_L.h );
		m_L.l = SHORT( m_L.l + m_H.h + m_H.h + m_L.h + m_L.h );

		m_L.h = r2;
		m_H.h = r1;

		return *this;
	}

	bool prior ( const c_encint<INT, SHORT, RD>& n ) const
	{
		if( sum() > n.sum() )
		{
			return true;
		}

		else if( sum() < n.sum() )
		{
			return false;
		}

		return sub() > n.sub();
	}

	c_encint<INT, SHORT, RD> operator + () const									{ return *this; }
	c_encint<INT, SHORT, RD> operator - () const									{ return c_encint<INT, SHORT, RD>( -value() ); }

	c_encint<INT, SHORT, RD>& operator ++ ()										{ *this = value() + 1; return *this; }
	c_encint<INT, SHORT, RD>& operator -- ()										{ *this = value() - 1; return *this; }
	c_encint<INT, SHORT, RD> operator ++ (int)										{ c_encint<INT, SHORT, RD> n(*this); *this = value() + 1; return n; }
	c_encint<INT, SHORT, RD> operator -- (int)										{ c_encint<INT, SHORT, RD> n(*this); *this = value() - 1; return n; }
															
	bool operator == ( const c_encint<INT, SHORT, RD>& n ) const
	{
		return ( SHORT( m_H.l - n.m_H.h - n.m_H.h + n.m_L.h + n.m_L.h ) == SHORT( n.m_H.l - m_H.h - m_H.h + m_L.h + m_L.h ) ) &&
			( SHORT( m_L.l - n.m_H.h - n.m_H.h - n.m_L.h - n.m_L.h ) == SHORT( n.m_L.l - m_H.h - m_H.h - m_L.h - m_L.h ) );
	}

	bool operator != ( const c_encint<INT, SHORT, RD>& n ) const
	{
		return ( SHORT( m_H.l - n.m_H.h - n.m_H.h + n.m_L.h + n.m_L.h ) != SHORT( n.m_H.l - m_H.h - m_H.h + m_L.h + m_L.h ) ) ||
			( SHORT( m_L.l - n.m_H.h - n.m_H.h - n.m_L.h - n.m_L.h ) != SHORT( n.m_L.l - m_H.h - m_H.h - m_L.h - m_L.h ) );
	}

	bool operator <	 ( const c_encint<INT, SHORT, RD>& n ) const					{ return value() <  n.value(); }
	bool operator >  ( const c_encint<INT, SHORT, RD>& n ) const					{ return value() >  n.value(); }
	bool operator <= ( const c_encint<INT, SHORT, RD>& n ) const					{ return value() <= n.value(); }
	bool operator >= ( const c_encint<INT, SHORT, RD>& n ) const					{ return value() >= n.value(); }
															
	c_encint<INT, SHORT, RD>& operator += ( const c_encint<INT, SHORT, RD>& n )		{ *this = value() + n.value(); return *this; }
	c_encint<INT, SHORT, RD>& operator -= ( const c_encint<INT, SHORT, RD>& n )		{ *this = value() - n.value(); return *this; }
	c_encint<INT, SHORT, RD>& operator *= ( const c_encint<INT, SHORT, RD>& n )		{ *this = value() * n.value(); return *this; }
	c_encint<INT, SHORT, RD>& operator /= ( const c_encint<INT, SHORT, RD>& n )		{ *this = value() / n.value(); return *this; }
	c_encint<INT, SHORT, RD>& operator %= ( const c_encint<INT, SHORT, RD>& n )		{ *this = value() % n.value(); return *this; }

	c_encint<INT, SHORT, RD> operator + ( const c_encint<INT, SHORT, RD>& n ) const	{ return c_encint<INT, SHORT, RD>( value() + n.value() ); }
	c_encint<INT, SHORT, RD> operator - ( const c_encint<INT, SHORT, RD>& n ) const	{ return c_encint<INT, SHORT, RD>( value() - n.value() ); }
	c_encint<INT, SHORT, RD> operator * ( const c_encint<INT, SHORT, RD>& n ) const	{ return c_encint<INT, SHORT, RD>( value() * n.value() ); }
	c_encint<INT, SHORT, RD> operator / ( const c_encint<INT, SHORT, RD>& n ) const	{ return c_encint<INT, SHORT, RD>( value() / n.value() ); }
	c_encint<INT, SHORT, RD> operator % ( const c_encint<INT, SHORT, RD>& n ) const	{ return c_encint<INT, SHORT, RD>( value() % n.value() ); }

	template< typename T > c_encint( T n )											{ enc( INT( n ) ); }
	template< typename T > c_encint<INT, SHORT, RD>& operator = ( T n )				{ enc( INT( n ) ); return *this; }

	template< typename T > bool operator == ( T n ) const							{ return value() == INT(n); }
	template< typename T > bool operator != ( T n ) const							{ return value() != INT(n); }
	template< typename T > bool operator < ( T n ) const							{ return value() < INT(n); }
	template< typename T > bool operator > ( T n ) const							{ return value() > INT(n); }
	template< typename T > bool operator <= ( T n ) const							{ return value() <= INT(n); }
	template< typename T > bool operator >= ( T n ) const							{ return value() >= INT(n); }

	template< typename T > c_encint<INT, SHORT, RD>& operator += ( T n )			{ *this = value() + INT(n); return *this; }
	template< typename T > c_encint<INT, SHORT, RD>& operator -= ( T n )			{ *this = value() - INT(n); return *this; }
	template< typename T > c_encint<INT, SHORT, RD>& operator *= ( T n )			{ *this = value() * INT(n); return *this; }
	template< typename T > c_encint<INT, SHORT, RD>& operator /= ( T n )			{ *this = value() / INT(n); return *this; }
	template< typename T > c_encint<INT, SHORT, RD>& operator %= ( T n )			{ *this = value() % INT(n); return *this; }
																				
	template< typename T > c_encint<INT, SHORT, RD> operator + ( T n ) const		{ return c_encint<INT, SHORT, RD>( value() + INT(n) ); }
	template< typename T > c_encint<INT, SHORT, RD> operator - ( T n ) const		{ return c_encint<INT, SHORT, RD>( value() - INT(n) ); }
	template< typename T > c_encint<INT, SHORT, RD> operator * ( T n ) const		{ return c_encint<INT, SHORT, RD>( value() * INT(n) ); }
	template< typename T > c_encint<INT, SHORT, RD> operator / ( T n ) const		{ return c_encint<INT, SHORT, RD>( value() / INT(n) ); }
	template< typename T > c_encint<INT, SHORT, RD> operator % ( T n ) const		{ return c_encint<INT, SHORT, RD>( value() % INT(n) ); }

private:
	struct HiLo
	{
		SHORT h;
		SHORT l;
	};


	__forceinline SHORT sum() const
	{
		return SHORT( m_H.l + m_L.l + m_H.h + m_H.h + m_H.h + m_H.h );
	}

	__forceinline SHORT sub() const
	{
		return SHORT( m_L.l - m_H.l + m_L.h + m_L.h + m_L.h + m_L.h );
	}

	__forceinline void enc( INT val )
	{
		HiLo & v = *reinterpret_cast< HiLo * >( &val );

		SHORT r1 = RD::generate();
		SHORT r2 = RD::generate();

		m_L.h = r2 ;
		m_L.l = SHORT( v.h - r1 - r1 - r2 - r2 );

		m_H.h = r1;
		m_H.l = SHORT( v.l - r1 - r1 + r2 + r2 );
	}

	__forceinline INT dec() const
	{
		INT val;
		HiLo & r_val = *reinterpret_cast< HiLo * >( &val );

		r_val.h = m_L.l + m_H.h + m_H.h + m_L.h + m_L.h;
		r_val.l = m_H.l + m_H.h + m_H.h - m_L.h - m_L.h;

		return val;
	}

	HiLo m_H;
	HiLo m_L;
};

template< typename T, typename INT, typename SHORT, typename RD > inline bool operator == ( T n, const c_encint<INT, SHORT, RD>& m )			
{ return m == n; }
template< typename T, typename INT, typename SHORT, typename RD > inline bool operator != ( T n, const c_encint<INT, SHORT, RD>& m )			
{ return m != n; }
template< typename T, typename INT, typename SHORT, typename RD > inline bool operator < ( T n, const c_encint<INT, SHORT, RD>& m ) 			
{ return m > n; }
template< typename T, typename INT, typename SHORT, typename RD > inline bool operator > ( T n, const c_encint<INT, SHORT, RD>& m ) 			
{ return m < n; }
template< typename T, typename INT, typename SHORT, typename RD > inline bool operator <= ( T n, const c_encint<INT, SHORT, RD>& m )			
{ return m >= n; }
template< typename T, typename INT, typename SHORT, typename RD > inline bool operator >= ( T n, const c_encint<INT, SHORT, RD>& m )			
{ return m <= n; }
template< typename T, typename INT, typename SHORT, typename RD > inline c_encint<INT, SHORT, RD> operator + ( T n, const c_encint<INT, SHORT, RD>& m ) 	
{ return c_encint<INT, SHORT, RD>( INT(n) + m.value() ); }
template< typename T, typename INT, typename SHORT, typename RD > inline c_encint<INT, SHORT, RD> operator - ( T n, const c_encint<INT, SHORT, RD>& m ) 	
{ return c_encint<INT, SHORT, RD>( INT(n) - m.value() ); }
template< typename T, typename INT, typename SHORT, typename RD > inline c_encint<INT, SHORT, RD> operator * ( T n, const c_encint<INT, SHORT, RD>& m ) 	
{ return c_encint<INT, SHORT, RD>( INT(n) * m.value() ); }
template< typename T, typename INT, typename SHORT, typename RD > inline c_encint<INT, SHORT, RD> operator / ( T n, const c_encint<INT, SHORT, RD>& m )	
{ return c_encint<INT, SHORT, RD>( INT(n) / m.value() ); }
template< typename T, typename INT, typename SHORT, typename RD > inline c_encint<INT, SHORT, RD> operator % ( T n, const c_encint<INT, SHORT, RD>& m )	
{ return c_encint<INT, SHORT, RD>( INT(n) % m.value() ); }

#pragma pack( pop )