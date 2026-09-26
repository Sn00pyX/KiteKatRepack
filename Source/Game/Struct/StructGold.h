#pragma once

struct StructGold
{
public:
	// Constructor
	explicit StructGold() : m_nGold( 0 ) {};
	StructGold( const __int64 & nGold ) : m_nGold( nGold ) {};

	// Raw data handler
	const __int64 & GetRawData() const								{ return m_nGold; }
	void SetRawData( const __int64 & nGold )						{ m_nGold = nGold; }

	// Unary operators
	// 아래 연산자는 생성자의 explicit을 없애고 __int64 operator 를 추가한 후에 삭제되면 됨
	const bool operator ! () const									{ return ( this->m_nGold == 0 ); }
	// bool 타입 캐스팅 연산자는 정의하면 암시적 캐스팅에 의해 StructGold + __int64 의 경우 모호성 발생함( bool + __int64 or StructGold + StructGold )

	// Binary operators
	StructGold& operator = ( const StructGold & rhs )				{ this->m_nGold = rhs.GetRawData(); return (*this); }

	const StructGold operator += ( const StructGold & rhs )			{ this->m_nGold += rhs.GetRawData(); return (*this); }
	const StructGold operator -= ( const StructGold & rhs )			{ this->m_nGold -= rhs.GetRawData(); return (*this); }
	const StructGold operator *= ( const StructGold & rhs )			{ this->m_nGold *= rhs.GetRawData(); return (*this); }
	const StructGold operator /= ( const StructGold & rhs )			{ this->m_nGold /= rhs.GetRawData(); return (*this); }

	const StructGold operator + ( const StructGold & rhs ) const	{ return StructGold( this->m_nGold + rhs.m_nGold ); }
	const StructGold operator - ( const StructGold & rhs ) const	{ return StructGold( this->m_nGold - rhs.m_nGold ); }
	const StructGold operator * ( const StructGold & rhs ) const	{ return StructGold( this->m_nGold * rhs.m_nGold ); }
	const StructGold operator / ( const StructGold & rhs ) const	{ return StructGold( this->m_nGold / rhs.m_nGold ); }

	const bool operator == ( const StructGold & rhs ) const			{ return this->m_nGold == rhs.m_nGold; }
	const bool operator != ( const StructGold & rhs ) const			{ return this->m_nGold != rhs.m_nGold; }
	const bool operator < ( const StructGold & rhs ) const			{ return this->m_nGold < rhs.m_nGold; }
	const bool operator > ( const StructGold & rhs ) const			{ return this->m_nGold > rhs.m_nGold; }
	const bool operator <= ( const StructGold & rhs ) const			{ return this->m_nGold <= rhs.m_nGold; }
	const bool operator >= ( const StructGold & rhs ) const			{ return this->m_nGold >= rhs.m_nGold; }

private:
	__int64			m_nGold;
};
