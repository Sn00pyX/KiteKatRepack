#ifndef __C_HIDINT_H__
#define __C_HIDINT_H__

template< typename INT, int MAGIC_NUM = 1 >
class c_hidint
{
public:

	typedef c_hidint< INT, MAGIC_NUM > HidInt;

	c_hidint()																{}
	template< int N > c_hidint( const c_hidint<INT, N>& n )					{ m_val = _copy( n ); }
	template< int N > HidInt& operator = ( const c_hidint<INT, N>& n )		{ m_val = _copy( n ); return *this; }
	operator INT () const													{ return _dec(); }
	INT value() const														{ return _dec(); }
															
	HidInt operator + () const												{ return HidInt( value() ); }
	HidInt operator - () const												{ return HidInt( -value() ); }
															
	HidInt& operator ++ ()													{ ++m_val; return *this; }
	HidInt& operator -- ()													{ --m_val; return *this; }
	HidInt operator ++ (int)												{ HidInt n(*this); ++m_val; return n; }
	HidInt operator -- (int)												{ HidInt n(*this); --m_val; return n; }
															
	template< int N > bool operator == ( const c_hidint<INT, N>& n ) const	{ return m_val == _copy( n ); }
	template< int N > bool operator != ( const c_hidint<INT, N>& n ) const	{ return m_val != _copy( n ); }
	template< int N > bool operator < ( const c_hidint<INT, N>& n ) const	{ return value() < n.value(); }
	template< int N > bool operator > ( const c_hidint<INT, N>& n ) const	{ return value() > n.value(); }
	template< int N > bool operator <= ( const c_hidint<INT, N>& n ) const	{ return value() <= n.value(); }
	template< int N > bool operator >= ( const c_hidint<INT, N>& n ) const	{ return value() >= n.value(); }
															
	template< int N > HidInt& operator += ( const c_hidint<INT, N>& n )		{ m_val += n.value(); return *this; }
	template< int N > HidInt& operator -= ( const c_hidint<INT, N>& n )		{ m_val -= n.value(); return *this; }
	template< int N > HidInt& operator *= ( const c_hidint<INT, N>& n )		{ *this = value() * n.value(); return *this; }
	template< int N > HidInt& operator /= ( const c_hidint<INT, N>& n )		{ *this = value() / n.value(); return *this; }
	template< int N > HidInt& operator %= ( const c_hidint<INT, N>& n )		{ *this = value() % n.value(); return *this; }

	template< int N > HidInt operator + ( const c_hidint<INT, N>& n ) const	{ return HidInt( value() + n.value() ); }
	template< int N > HidInt operator - ( const c_hidint<INT, N>& n ) const	{ return HidInt( value() - n.value() ); }
	template< int N > HidInt operator * ( const c_hidint<INT, N>& n ) const	{ return HidInt( value() * n.value() ); }
	template< int N > HidInt operator / ( const c_hidint<INT, N>& n ) const	{ return HidInt( value() / n.value() ); }
	template< int N > HidInt operator % ( const c_hidint<INT, N>& n ) const	{ return HidInt( value() % n.value() ); }

	template< typename T > c_hidint( T n )									{ m_val = _enc( n ); }
	template< typename T > HidInt& operator = ( T n )						{ m_val = _enc( n ); return *this; }

	template< typename T > bool operator == ( T n ) const					{ return m_val == _enc( n ); }
	template< typename T > bool operator != ( T n ) const					{ return m_val != _enc( n ); }
	template< typename T > bool operator < ( T n ) const					{ return value() < INT(n); }
	template< typename T > bool operator > ( T n ) const					{ return value() > INT(n); }
	template< typename T > bool operator <= ( T n ) const					{ return value() <= INT(n); }
	template< typename T > bool operator >= ( T n ) const					{ return value() >= INT(n); }

	template< typename T > HidInt& operator += ( T n )						{ m_val += INT(n); return *this; }
	template< typename T > HidInt& operator -= ( T n )						{ m_val -= INT(n); return *this; }
	template< typename T > HidInt& operator *= ( T n )						{ *this = value() * INT(n); return *this; }
	template< typename T > HidInt& operator /= ( T n )						{ *this = value() / INT(n); return *this; }
	template< typename T > HidInt& operator %= ( T n )						{ *this = value() % INT(n); return *this; }
											
	template< typename T > HidInt operator + ( T n ) const					{ return HidInt( value() + INT(n) ); }
	template< typename T > HidInt operator - ( T n ) const					{ return HidInt( value() - INT(n) ); }
	template< typename T > HidInt operator * ( T n ) const					{ return HidInt( value() * INT(n) ); }
	template< typename T > HidInt operator / ( T n ) const					{ return HidInt( value() / INT(n) ); }
	template< typename T > HidInt operator % ( T n ) const					{ return HidInt( value() % INT(n) ); }

private:

	template< typename T > INT _enc( T n ) const							{ return n - INT(&m_val) - INT(&m_val) - MAGIC_NUM; }
	INT _dec() const 														{ return m_val + INT(&m_val) + INT(&m_val) + MAGIC_NUM; }
	template< int N > INT _copy( const c_hidint<INT, N>& v ) const			{ return v.m_val + 2 * ( INT(&v) - INT(&m_val) ) + ( N - MAGIC_NUM ); }

	INT m_val;
};

template< typename T, typename INT, int N > inline c_hidint<INT, N+2> operator + ( T n, const c_hidint<INT, N>& m ) 	
{ return c_hidint<INT, N+2>( INT(n) + m.value() ); }
template< typename T, typename INT, int N > inline c_hidint<INT, N+5> operator - ( T n, const c_hidint<INT, N>& m ) 	
{ return c_hidint<INT, N+5>( INT(n) - m.value() ); }
template< typename T, typename INT, int N > inline c_hidint<INT, N+7> operator * ( T n, const c_hidint<INT, N>& m ) 	
{ return c_hidint<INT, N+7>( INT(n) * m.value() ); }
template< typename T, typename INT, int N > inline c_hidint<INT, N-11> operator / ( T n, const c_hidint<INT, N>& m )	
{ return c_hidint<INT, N-11>( INT(n) / m.value() ); }
template< typename T, typename INT, int N > inline c_hidint<INT, N-3> operator % ( T n, const c_hidint<INT, N>& m )	
{ return c_hidint<INT, N-3>( INT(n) % m.value() ); }
template< typename T, typename INT, int N > inline bool operator == ( T n, const c_hidint<INT, N>& m )	{ return m == n; }
template< typename T, typename INT, int N > inline bool operator != ( T n, const c_hidint<INT, N>& m )	{ return m != n; }
template< typename T, typename INT, int N > inline bool operator < ( T n, const c_hidint<INT, N>& m ) 	{ return m > n; }
template< typename T, typename INT, int N > inline bool operator > ( T n, const c_hidint<INT, N>& m ) 	{ return m < n; }
template< typename T, typename INT, int N > inline bool operator <= ( T n, const c_hidint<INT, N>& m )	{ return m >= n; }
template< typename T, typename INT, int N > inline bool operator >= ( T n, const c_hidint<INT, N>& m )	{ return m <= n; }

#endif