
#pragma once

#include <memory.h>
#include <string.h>

#include "../toolkit/safe_function.h"

/// XAddr 의 주소문자열의 버퍼 크기
const int XADDRESS_MAX_LENGTH		= 128;


class XAddr
{
public:

	const char *	GetAddr() const					{ return m_szAddr;		}	
	int				GetPort() const					{ return m_nPort;		}

	void			SetAddr( const char *szAddr )	{ setAddr( szAddr );	}
	void			SetPortNum( int nPort )			{ setPort( nPort );		}

	XAddr & operator = ( const XAddr & addr )
	{
		setAddr( addr.GetAddr() );
		setPort( addr.GetPort() );

		return *this;
	}

	XAddr( const char * szAddr = 0, int nPort = 0 )
	{
		setAddr( szAddr );
		setPort( nPort );
	}

	bool operator == ( const XAddr & addr ) const
	{
		if ( strcmp( GetAddr(), addr.GetAddr() ) )
			return false;

		if ( GetPort() != addr.GetPort() )
			return false;

		return true;
	}

private:

	void setAddr( const char *szAddr )
	{
		if ( szAddr ) {
			s_strcpy( m_szAddr, _countof( m_szAddr ), szAddr );
		} else {
			m_szAddr[0] = 0;
		}
	}

	void setPort( int nPort )
	{
		m_nPort = nPort;
	}

	char	m_szAddr[XADDRESS_MAX_LENGTH];
	int		m_nPort;
};

