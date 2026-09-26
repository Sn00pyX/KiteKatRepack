
#pragma once


template< typename T >
class	CStatusFlag
{
public:

	CStatusFlag()
		: m_nStatusFlag( 0 )
	{}

	const T&	GetRawData()	const
	{
		return m_nStatusFlag;
	}
	void		SetRawData( const T& nData )
	{
		m_nStatusFlag = nData;
	}

	size_t		GetRawDataSize()	const
	{
		return sizeof( m_nStatusFlag );
	}

	bool		IsOn( unsigned char nStatusFlagIndex )	const
	{
		return (m_nStatusFlag & ( 1 << nStatusFlagIndex )) != 0;
	}
	void		Set( unsigned char nStatusFlagIndex, bool _bTurnOn = true )
	{
		if( _bTurnOn )
		{
			m_nStatusFlag |= 1 << nStatusFlagIndex;
		}
		else if( IsOn( nStatusFlagIndex ) == true )
		{
			m_nStatusFlag ^= 1 << nStatusFlagIndex;
		}
		// else : 해당 플래그가 켜져 있지 않으므로 아무것도 하지 않음
	}

	void		Clear()
	{
		m_nStatusFlag = 0;
	}

private:

	T	m_nStatusFlag;

};
