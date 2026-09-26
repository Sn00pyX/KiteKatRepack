
#pragma once

#include <vector>
#include <string>

#include "ColumnSchema.h"

struct	ColumnData
{
	ColumnData();
	~ColumnData();

	bool		IsNull()	const;
	bool		IsEqual( const std::string& strName )	const;

	template< typename Value_T >
	Value_T			Get()	const
	{
		return *(reinterpret_cast< Value_T* >( pData ));
	}
	template<>
	const char*		Get()	const
	{
		return (reinterpret_cast< const char* >( pData ));
	}
	template<>
	const wchar_t*	Get()	const
	{
		return (reinterpret_cast< const wchar_t* >( pData ));
	}

	const COLUMN_SCHEMA*	pSchema;
	bool					bIsNull;
	char*					pData;
	unsigned short			nDataSize;
	unsigned short			nAllocSize;
};


struct	RDURow
{
	~RDURow();

	void	Clear();

	ColumnData*			At( int nIndex );
	const ColumnData*	At( int nIndex )	const;

	ColumnData*			At( const std::string& strName );
	const ColumnData*	At( const std::string& strName )	const;

	std::vector< ColumnData* >	vDatas;

};

