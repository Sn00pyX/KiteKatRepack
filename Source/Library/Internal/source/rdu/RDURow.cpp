
#include "../../include/rdu/RDURow.h"

#include <cstring>


ColumnData::ColumnData()
	: pSchema( NULL )
	, bIsNull( false )
	, pData( NULL )
	, nDataSize( 0 )
	, nAllocSize( 0 )
{
}

ColumnData::~ColumnData()
{
	pSchema = NULL;

	if( pData != NULL )
	{
		delete [] pData;
		pData = NULL;
	}
	nDataSize = 0;
	nAllocSize = 0;
}

bool	ColumnData::IsNull()	const
{
	if( pSchema != NULL )
	{
		if( pSchema->IsNullable() == true )
		{
			return bIsNull;
		}
	}

	return false;
}

bool	ColumnData::IsEqual( const std::string& strName )	const
{
	if( pSchema == NULL )
	{
		return false;
	}

	if( ::_stricmp( pSchema->strName.c_str(), strName.c_str() ) == 0 )
	{
		return true;
	}

	return false;
}


RDURow::~RDURow()
{
	Clear();
}

void	RDURow::Clear()
{
	std::vector< ColumnData* >::const_iterator pos = vDatas.begin();
	std::vector< ColumnData* >::const_iterator end = vDatas.end();

	for( ; pos != end; ++pos )
	{
		const ColumnData* pData = (*pos);
		delete pData;
	}

	vDatas.clear();
}

ColumnData*	RDURow::At( int nIndex )
{
	if( 0 <= nIndex && nIndex < int( vDatas.size() ) )
	{
		return vDatas[nIndex];
	}

	return NULL;
}

const ColumnData*	RDURow::At( int nIndex )	const
{
	if( 0 <= nIndex && nIndex < int( vDatas.size() ) )
	{
		return vDatas[nIndex];
	}

	return NULL;
}

ColumnData*	RDURow::At( const std::string& strName )
{
	std::vector< ColumnData* >::const_iterator pos = vDatas.begin();
	std::vector< ColumnData* >::const_iterator end = vDatas.end();

	for( ; pos != end; ++pos )
	{
		ColumnData* pData = (*pos);
		if( pData->IsEqual( strName ) == true )
		{
			return pData;
		}
	}

	return NULL;
}

const ColumnData*	RDURow::At( const std::string& strName )	const
{
	std::vector< ColumnData* >::const_iterator pos = vDatas.begin();
	std::vector< ColumnData* >::const_iterator end = vDatas.end();

	for( ; pos != end; ++pos )
	{
		const ColumnData* pData = (*pos);
		if( pData->IsEqual( strName ) == true )
		{
			return pData;
		}
	}

	return NULL;
}

