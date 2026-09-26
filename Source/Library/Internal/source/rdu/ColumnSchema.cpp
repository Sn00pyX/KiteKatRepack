
#include "../../include/rdu/ColumnSchema.h"


bool	COLUMN_SCHEMA::IsSupportedColumnType( const int nXUserType )
{
	switch( nXUserType )
	{
	case USERTYPE_TINYINT:
	case USERTYPE_SMALLINT:
	case USERTYPE_INT:
	case USERTYPE_SMALLDATETIME:
	case USERTYPE_REAL:
	case USERTYPE_DATETIME:
	case USERTYPE_FLOAT:
	case USERTYPE_BIT:
	case USERTYPE_DECIMAL:
	case USERTYPE_BIGINT:
	case USERTYPE_VARCHAR:
	case USERTYPE_CHAR:
	case USERTYPE_TIMESTAMP:
	case USERTYPE_NVARCHAR:
	case USERTYPE_NCHAR:
		return true;
	}

	return false;
}


COLUMN_SCHEMA::COLUMN_SCHEMA()
	: eXUserType( USERTYPE_UNKNOWN )
	, nLength( 0 )
	, nXPrec( 0 )
	, nXScale( 0 )
{
}

COLUMN_SCHEMA::COLUMN_SCHEMA(	const char* _pszName,
								_USERTYPE _eXUserType,
								unsigned short _nLength,
								unsigned char _nXPrec,
								unsigned char _nXScale,
								bool _bIsNullable,
								const char* _pszDefaultValue,
								const char* _pszCollation )
	: eXUserType( _eXUserType )
	, nLength( _nLength )
	, nXPrec( _nXPrec )
	, nXScale( _nXScale )
{
	if( _pszName != NULL )
	{
		strName = _pszName;
	}

	if( _bIsNullable == true )
	{
		StatusFlag.Set( STATUS_FLAG_NULLABLE );
	}

	SetDefaultValue( _pszDefaultValue );

	if( _pszCollation != NULL )
	{
		strCollation = _pszCollation;
	}
}

const char*	COLUMN_SCHEMA::GetDefaultValue()	const
{
	return StatusFlag.IsOn( STATUS_FLAG_HAS_DEFAULT_VALUE ) ? strDefaultValue.c_str() : NULL;
}

void	COLUMN_SCHEMA::SetDefaultValue( const char* pszDefaultValue )
{
	if( pszDefaultValue != NULL )
	{
		strDefaultValue = pszDefaultValue;
		StatusFlag.Set( STATUS_FLAG_HAS_DEFAULT_VALUE );
	}
	else
	{
		strDefaultValue.clear();
		StatusFlag.Set( STATUS_FLAG_HAS_DEFAULT_VALUE, false );
	}
}

bool	COLUMN_SCHEMA::IsNullable()	const
{
	return StatusFlag.IsOn( COLUMN_SCHEMA::STATUS_FLAG_NULLABLE );
}
