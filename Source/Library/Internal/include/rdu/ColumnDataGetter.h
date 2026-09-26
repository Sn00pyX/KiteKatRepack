
#pragma once

#include <cassert>

#include "RDURow.h"


template< bool SupportDECIMAL >
struct	DECIMALToDouble;

template<>
struct	DECIMALToDouble< false >
{
	static	double	ToDoubleFromDECIAL( const ColumnData* pColumnData, double default_value )
	{
		if( pColumnData != NULL )
		{
			if( pColumnData->pSchema->eXUserType == COLUMN_SCHEMA::USERTYPE_DECIMAL )
			{
				if( pColumnData->pSchema->nLength == sizeof( double ) )
				{
					return *reinterpret_cast< const double* >( pColumnData->pData );
				}
			}
		}

		return default_value;
	}

};

template< typename DD_T >
struct	ColumnDataGetter
{
	ColumnDataGetter( const ColumnData* pInput )
		: pColumnData( pInput )
	{
	}

	int		ToInterger( int default_value = 0 )
	{
		return ToInteger64( default_value );
	}

	__int64	ToInteger64( __int64 default_value = 0 )
	{
		if( pColumnData == NULL )
			return default_value;

		switch( pColumnData->pSchema->eXUserType )
		{
		case COLUMN_SCHEMA::USERTYPE_BIT:
			{
				assert( sizeof( bool ) == pColumnData->pSchema->nLength );
				return *reinterpret_cast< const bool* >( pColumnData->pData ) ? 1 : 0;
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_TINYINT:
			{
				assert( sizeof( unsigned char ) == pColumnData->pSchema->nLength );
				return *reinterpret_cast< const unsigned char* >( pColumnData->pData );
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_SMALLINT:
			{
				assert( sizeof( short ) == pColumnData->pSchema->nLength );
				return *reinterpret_cast< const short* >( pColumnData->pData );
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_INT:
			{
				assert( sizeof( int ) == pColumnData->pSchema->nLength );
				return *reinterpret_cast< const int* >( pColumnData->pData );
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_BIGINT:
			{
				assert( sizeof( __int64 ) == pColumnData->pSchema->nLength );
				return *reinterpret_cast< const __int64* >( pColumnData->pData );
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_SMALLDATETIME:
		case COLUMN_SCHEMA::USERTYPE_REAL:
		case COLUMN_SCHEMA::USERTYPE_FLOAT:
		case COLUMN_SCHEMA::USERTYPE_DATETIME:
		case COLUMN_SCHEMA::USERTYPE_TIMESTAMP:
		case COLUMN_SCHEMA::USERTYPE_DECIMAL:
			{
				return ToDouble( default_value );
			}
			break;

		case COLUMN_SCHEMA::USERTYPE_VARCHAR:
		case COLUMN_SCHEMA::USERTYPE_CHAR:
			{
				return _atoi64( reinterpret_cast< const char* >( pColumnData->pData ) );
			}
			break;

		case COLUMN_SCHEMA::USERTYPE_NVARCHAR:
		case COLUMN_SCHEMA::USERTYPE_NCHAR:
			{
				return _wtoi64( reinterpret_cast< const wchar_t* >( pColumnData->pData ) );
			}
			break;
		}

		return default_value;
	}

	double	ToDouble( double default_value = 0 )
	{
		if( pColumnData == NULL )
			return default_value;

		switch( pColumnData->pSchema->eXUserType )
		{
		case COLUMN_SCHEMA::USERTYPE_BIT:
		case COLUMN_SCHEMA::USERTYPE_TINYINT:
		case COLUMN_SCHEMA::USERTYPE_SMALLINT:
		case COLUMN_SCHEMA::USERTYPE_INT:
		case COLUMN_SCHEMA::USERTYPE_BIGINT:
			{
				return ToInteger64( default_value );
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_SMALLDATETIME:
		case COLUMN_SCHEMA::USERTYPE_REAL:
			{
				assert( sizeof( float ) == pColumnData->pSchema->nLength );
				return *reinterpret_cast< const float* >( pColumnData->pData );
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_FLOAT:
		case COLUMN_SCHEMA::USERTYPE_DATETIME:
		case COLUMN_SCHEMA::USERTYPE_TIMESTAMP:
			{
				assert( sizeof( double ) == pColumnData->pSchema->nLength );
				return *reinterpret_cast< const double* >( pColumnData->pData );
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_DECIMAL:
			{
				return DD_T::ToDoubleFromDECIAL( pColumnData, default_value );
			}
			break;

		case COLUMN_SCHEMA::USERTYPE_VARCHAR:
		case COLUMN_SCHEMA::USERTYPE_CHAR:
			{
				return atof( reinterpret_cast< const char* >( pColumnData->pData ) );
			}
			break;

		case COLUMN_SCHEMA::USERTYPE_NVARCHAR:
		case COLUMN_SCHEMA::USERTYPE_NCHAR:
			{
				return _wtof( reinterpret_cast< const wchar_t* >( pColumnData->pData ) );
			}
			break;
		}

		return default_value;
	}
	std::string	ToString( int code_page = CP_ACP, const char* default_value = "" )
	{
		if( pColumnData == NULL )
			return default_value;

		switch( pColumnData->pSchema->eXUserType )
		{
		case COLUMN_SCHEMA::USERTYPE_BIT:
		case COLUMN_SCHEMA::USERTYPE_TINYINT:
		case COLUMN_SCHEMA::USERTYPE_SMALLINT:
		case COLUMN_SCHEMA::USERTYPE_INT:
		case COLUMN_SCHEMA::USERTYPE_BIGINT:
			{
				char szInterger[128] = { 0, };
				s_sprintf( szInterger, _countof( szInterger ), "%I64d", ToInteger64() );
				return szInterger;
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_SMALLDATETIME:
		case COLUMN_SCHEMA::USERTYPE_REAL:
		case COLUMN_SCHEMA::USERTYPE_FLOAT:
		case COLUMN_SCHEMA::USERTYPE_DATETIME:
		case COLUMN_SCHEMA::USERTYPE_TIMESTAMP:
		case COLUMN_SCHEMA::USERTYPE_DECIMAL:
			{
				char szDouble[128] = { 0, };
				s_sprintf( szDouble, _countof( szDouble ), "%f", ToDouble() );
				return szDouble;
			}
			break;

		case COLUMN_SCHEMA::USERTYPE_VARCHAR:
		case COLUMN_SCHEMA::USERTYPE_CHAR:
			{
				return reinterpret_cast< const char* >( pColumnData->pData );
			}
			break;

		case COLUMN_SCHEMA::USERTYPE_NVARCHAR:
		case COLUMN_SCHEMA::USERTYPE_NCHAR:
			{
				return XStringUtil::Wide2Multi( reinterpret_cast< const wchar_t* >( pColumnData->pData ), code_page );
			}
			break;
		}

		return default_value;
	}

	std::wstring	ToWString( int code_page = CP_ACP, const wchar_t* default_value = L"" )
	{
		if( pColumnData == NULL )
			return default_value;

		switch( pColumnData->pSchema->eXUserType )
		{
		case COLUMN_SCHEMA::USERTYPE_BIT:
		case COLUMN_SCHEMA::USERTYPE_TINYINT:
		case COLUMN_SCHEMA::USERTYPE_SMALLINT:
		case COLUMN_SCHEMA::USERTYPE_INT:
		case COLUMN_SCHEMA::USERTYPE_BIGINT:
			{
				wchar_t szInterger[64] = { 0, };
				s_sprintf( szInterger, _countof( szInterger ), L"%I64d", ToInteger64() );
				return szInterger;
			}
			break;
		case COLUMN_SCHEMA::USERTYPE_SMALLDATETIME:
		case COLUMN_SCHEMA::USERTYPE_REAL:
		case COLUMN_SCHEMA::USERTYPE_FLOAT:
		case COLUMN_SCHEMA::USERTYPE_DATETIME:
		case COLUMN_SCHEMA::USERTYPE_TIMESTAMP:
		case COLUMN_SCHEMA::USERTYPE_DECIMAL:
			{
				wchar_t szDouble[64] = { 0, };
				s_sprintf( szDouble, _countof( szDouble ), L"%f", ToDouble() );
				return szDouble;
			}
			break;

		case COLUMN_SCHEMA::USERTYPE_VARCHAR:
		case COLUMN_SCHEMA::USERTYPE_CHAR:
			{
				return XStringUtil::Multi2Wide( reinterpret_cast< const char* >( pColumnData->pData ), code_page );
			}
			break;

		case COLUMN_SCHEMA::USERTYPE_NVARCHAR:
		case COLUMN_SCHEMA::USERTYPE_NCHAR:
			{
				return reinterpret_cast< const wchar_t* >( pColumnData->pData );
			}
			break;
		}

		return default_value;
	}

	const ColumnData* pColumnData;
};