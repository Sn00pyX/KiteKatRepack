
#include "../../include/rdu/IndexSchema.h"


INDEX_SCHEMA::COLUMN_INFO::COLUMN_INFO()
{
}

INDEX_SCHEMA::COLUMN_INFO::COLUMN_INFO( const char* _pszName, bool _bIsDescending )
{
	if( _pszName != NULL )
	{
		strName = _pszName;
	}

	if( _bIsDescending == true )
	{
		StatusFlag.Set( STATUS_FLAG_DESCENDING );
	}
}

INDEX_SCHEMA::COLUMN_INFO::COLUMN_INFO( const COLUMN_INFO& rhs )
	: strName( rhs.strName )
{
	StatusFlag.SetRawData( rhs.StatusFlag.GetRawData() );
}


INDEX_SCHEMA::INDEX_SCHEMA()
	: nID( 0 )
	, eType( TYPE_UNKNOWN )
{
}

INDEX_SCHEMA::INDEX_SCHEMA( short _nID, _TYPE _eType, bool _bIsClustered, const char* _pszName )
	: nID( _nID )
	, eType( _eType )
{
	if( _bIsClustered == true )
	{
		StatusFlag.Set( STATUS_FLAG_CLUSTERED );
	}

	if( _pszName != NULL )
	{
		strName = _pszName;
	}
}

INDEX_SCHEMA::INDEX_SCHEMA( const INDEX_SCHEMA& rhs )
	: nID( rhs.nID )
	, eType( rhs.eType )
{
	StatusFlag.SetRawData( rhs.StatusFlag.GetRawData() );
	strName = rhs.strName;

	std::vector< COLUMN_INFO* >::const_iterator pos = rhs.vColumnInfo.begin();
	std::vector< COLUMN_INFO* >::const_iterator end = rhs.vColumnInfo.end();

	for( ; pos != end; ++pos )
	{
		const COLUMN_INFO* pColumn = (*pos);
		vColumnInfo.push_back( new COLUMN_INFO( *pColumn ) );
	}
}

INDEX_SCHEMA::~INDEX_SCHEMA()
{
	std::vector< COLUMN_INFO* >::const_iterator pos = vColumnInfo.begin();
	std::vector< COLUMN_INFO* >::const_iterator end = vColumnInfo.end();

	for( ; pos != end; ++pos )
	{
		const COLUMN_INFO* pColumn = (*pos);
		delete pColumn;
	}

	vColumnInfo.clear();
}