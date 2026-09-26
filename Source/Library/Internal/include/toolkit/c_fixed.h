#pragma once
#pragma pack(push, 1)

template< int FACTOR >
class c_fixed
{
public:	
	c_fixed() : value( 0 )										{}
	c_fixed( const float & _value )								{ set( static_cast< __int64 >( _value * FACTOR ) ); }
	c_fixed( const double & _value )							{ set( static_cast< __int64 >( _value * FACTOR ) ); }
	c_fixed( const int _value )									{ set( static_cast< __int64 >( _value ) * FACTOR ); }
	// 오버 플로우 가능성 있지만 사용하도록... ㄷㄷ;;;
	c_fixed( const __int64 & value )							{ set( value * FACTOR ); }
	c_fixed( const unsigned int & _value )						{ set( static_cast< __int64 >( _value ) * FACTOR ); }
	c_fixed( const unsigned long & _value )						{ set( static_cast< __int64 >( _value ) * FACTOR ); }
	c_fixed( const c_fixed & n )								{ set( n.value ); }
	c_fixed( const __int64 & value, const bool bRawValue )		{ set( ( bRawValue ) ? value : value * FACTOR ); }

	c_fixed& operator = ( const c_fixed & n )					{ set( n.value ); return *this; }
	operator double() const										{ return static_cast< double >( value ) / static_cast< double >( FACTOR ); }
	const __int64 GetAsInt64() const							{ return ( value / FACTOR ); }

	void set( const __int64 & _value )							{ value = _value; }
	const __int64 & get() const									{ return value; }

	c_fixed operator + () const									{ return *this; }
	c_fixed operator - () const									{ return *this * -1; }

	bool operator == ( const c_fixed & n ) const
	{
		return value == n.value;
	}

	bool operator != ( const c_fixed & n ) const
	{
		return value != n.value;
	}

	bool operator <	 ( const c_fixed & n ) const					{ return value <  n.value; }
	bool operator >  ( const c_fixed & n ) const					{ return value >  n.value; }
	bool operator <= ( const c_fixed & n ) const					{ return value <= n.value; }
	bool operator >= ( const c_fixed & n ) const					{ return value >= n.value; }

	c_fixed& operator += ( const c_fixed & n )		{ value += n.value; return *this; }
	c_fixed& operator -= ( const c_fixed & n )		{ value -= n.value; return *this; }
	c_fixed& operator *= ( const c_fixed & n )		{ value = value * n.value / FACTOR; return *this; }
	c_fixed& operator /= ( const c_fixed & n )		{ value = value * FACTOR / n.value; return *this; }

	c_fixed operator + ( const c_fixed & n ) const
	{
		c_fixed res;
		res.set( value + n.value );

		return res;
	}
	
	c_fixed operator - ( const c_fixed & n ) const
	{
		c_fixed res;
		res.set( value - n.value );

		return res;
	}
	
	c_fixed operator * ( const c_fixed & n ) const
	{
		c_fixed res;
		res.set( value * n.value / FACTOR );

		return res;
	}

	c_fixed operator / ( const c_fixed & n ) const
	{
		c_fixed res;
		res.set( value * FACTOR / n.value );

		return res;
	}

	template< typename T > c_fixed operator + ( const T n ) const	{ return *this + c_fixed( n ); }
	template< typename T > c_fixed operator - ( const T n ) const	{ return *this - c_fixed( n ); }
	template< typename T > c_fixed operator * ( const T n ) const	{ return *this * c_fixed( n ); }
	template< typename T > c_fixed operator / ( const T n ) const	{ return *this / c_fixed( n ); }

	template< typename T > c_fixed & operator += ( const T n )		{ *this += c_fixed( n ); return *this; }
	template< typename T > c_fixed & operator -= ( const T n )		{ *this -= c_fixed( n ); return *this; }
	template< typename T > c_fixed & operator *= ( const T n )		{ *this *= c_fixed( n ); return *this; }
	template< typename T > c_fixed & operator /= ( const T n )		{ *this /= c_fixed( n ); return *this; }

	template< typename T >bool operator <	( const T n ) const		{ return *this <  c_fixed( n ); }
	template< typename T >bool operator >	( const T n ) const		{ return *this >  c_fixed( n ); }
	template< typename T >bool operator <=	( const T n ) const		{ return *this <= c_fixed( n ); }
	template< typename T >bool operator >=	( const T n ) const		{ return *this >= c_fixed( n ); }

	template< typename T >bool operator ==	( const T n ) const		{ return *this == c_fixed( n ); }
	template< typename T >bool operator !=	( const T n ) const		{ return *this != c_fixed( n ); }

private:

	__int64		value;
};

typedef c_fixed< 10000 > c_fixed10;

#pragma pack( pop )