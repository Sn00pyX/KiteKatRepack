
#pragma once


#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "ColumnSchema.h"
#include "IndexSchema.h"
#include "RDURow.h"

#include "../kfile/KStream.h"


class	RDUReader
{
public:

	RDUReader();
	~RDUReader();

	bool	Open( const std::string& file_name );
	bool	Open( KStream* pStream );
	void	Close();

	bool	Fetch( RDURow* pRow );

	const std::vector< COLUMN_SCHEMA* >&	GetColumnSchema()	const;
	const std::vector< INDEX_SCHEMA* >&		GetIndexSchema()	const;

	int		GetRowCount()	const;

private:

	bool	ReadHeader();

	bool	CheckHeader();

	bool	ReadColumnInfo();
	bool	ReadColumnSchema( COLUMN_SCHEMA* pCS );

	bool	ReadIndexInfo();
	bool	ReadIndexSchema( INDEX_SCHEMA* pIS );

	bool	ReadDataHeader();
	bool	ReadRow( RDURow* pRow );
	bool	ReadColumnData( ColumnData* pCD );

private:

	bool							m_bIsMyStream;
	KStream*						m_pRDUFile;

	std::vector< COLUMN_SCHEMA* >	m_vColumnSchema;
	std::vector< INDEX_SCHEMA* >	m_vIndexSchema;

	int								m_nCurrentRowPos;
	int								m_nTotalRowCount;

};