
#pragma once

#include <cassert>

template<>
struct	DECIMALToDouble< true >
{
	static	double	ToDoubleFromDECIAL( const ColumnData* pColumnData, double default_value )
	{
		if( pColumnData != NULL )
		{
			if( pColumnData->pSchema->eXUserType == COLUMN_SCHEMA::USERTYPE_DECIMAL )
			{
				static_assert( sizeof( double ) != sizeof( DECIMAL ), "double과 DECIMAL은 사이즈가 달라야 RDU에서 제대로 읽을 수 있다." );
				if( pColumnData->pSchema->nLength == sizeof( double ) )
				{
					return *reinterpret_cast< const double* >( pColumnData->pData );
				}
				else
				{
					assert( sizeof( DECIMAL ) == pColumnData->pSchema->nLength );
					double d = default_value;
					VarR8FromDec( reinterpret_cast< const DECIMAL* >( pColumnData->pData ), &d );
					return d;
				}
			}
		}

		return default_value;
	}
};
