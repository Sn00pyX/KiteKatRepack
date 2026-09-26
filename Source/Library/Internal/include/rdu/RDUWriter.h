
#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <vector>

#include "ColumnSchema.h"
#include "IndexSchema.h"


class	RDUWriter
{
public:

	RDUWriter();
	~RDUWriter();

	bool	Create( const std::string& file_name,
					const char* szTag,
					unsigned char nMajorVersion,
					unsigned char nMinorVersion );
	void	Close();

	bool	WriteColumnSchema( const std::vector< COLUMN_SCHEMA* >& vColumnSchema );
	bool	WriteIndexSchema( const std::vector< INDEX_SCHEMA* >& vIndexSchema );

	bool	WriteRowCount( int nRowCount );

	template< typename Data_T >
	bool	WriteColumnData( const Data_T& tData )
	{
		return WriteColumnData( &tData, sizeof( tData ) );
	}

	bool	WriteColumnData( const void* pData, unsigned short nSize );
	bool	WriteColumnString( const char* szString, unsigned short nLen );
	bool	WriteColumnString( const wchar_t* szString, unsigned short nLen );

private:

	bool	WriteHeader( const char* szTag,
						 unsigned char nMajorVersion,
						 unsigned char nMinorVersion );

	bool	WriteIndexColumnInfo( const std::vector< INDEX_SCHEMA::COLUMN_INFO* >& vColumnInfo );

private:

	HANDLE	m_hFile;

};

