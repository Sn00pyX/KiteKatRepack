
#include "../../include/rdu/RDUWriter.h"
#include "../../include/rdu/RDUFileHeader.h"

#include <limits>

namespace
{
	template< typename Length_T >
	bool	WriteStringData( HANDLE hFile, const std::string& strString )
	{
		size_t size = strString.size();
		if( size >= (std::numeric_limits< Length_T >::max)() )
		{
			return false;
		}

		Length_T nLength = static_cast< Length_T >( size );
		DWORD nWrittenBytes = 0;

		if( ::WriteFile( hFile, &nLength, sizeof( nLength ), &nWrittenBytes, NULL ) == FALSE )
		{
			return false;
		}

		if( nLength > 0 )
		{
			if( ::WriteFile( hFile, strString.c_str(), nLength, &nWrittenBytes, NULL ) == FALSE )
			{
				return false;
			}
		}

		return true;
	}
};


RDUWriter::RDUWriter()
	: m_hFile( INVALID_HANDLE_VALUE )
{
}

RDUWriter::~RDUWriter()
{
	Close();
}

bool	RDUWriter::Create( const std::string& file_name,
						   const char* szTag,
						   unsigned char nMajorVersion,
						   unsigned char nMinorVersion )
{
	m_hFile = CreateFile( file_name.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	if( WriteHeader( szTag, nMajorVersion, nMinorVersion ) == false )
	{
		return false;
	}

	return true;
}

void	RDUWriter::Close()
{
	if( m_hFile != INVALID_HANDLE_VALUE )
	{
		::CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

bool	RDUWriter::WriteHeader( const char* szTag,
								unsigned char nMajorVersion,
								unsigned char nMinorVersion )
{
	RDU_FILE_HEADER RDUFileHeader( szTag, nMajorVersion, nMinorVersion );

	DWORD nWrittenBytes = 0;
	if( ::WriteFile( m_hFile, &RDUFileHeader, sizeof( RDUFileHeader ), &nWrittenBytes, NULL ) == FALSE )
	{
		return false;
	}

	return true;
}

bool	RDUWriter::WriteColumnSchema( const std::vector< COLUMN_SCHEMA* >& vColumnSchema )
{
	unsigned short nColumnCount = static_cast< unsigned short >( vColumnSchema.size() );

	DWORD nWrittenBytes = 0;
	if( ::WriteFile( m_hFile, &nColumnCount, sizeof( nColumnCount ), &nWrittenBytes, NULL ) == FALSE )
	{
		return false;
	}

	std::vector< COLUMN_SCHEMA* >::const_iterator pos = vColumnSchema.begin();
	std::vector< COLUMN_SCHEMA* >::const_iterator end = vColumnSchema.end();
	for( ; pos != end; ++pos )
	{
		const COLUMN_SCHEMA* pSchema = (*pos);

		if( WriteStringData< unsigned char >( m_hFile, pSchema->strName ) == false )
		{
			return false;
		}

		if(	::WriteFile( m_hFile, &pSchema->eXUserType, sizeof( pSchema->eXUserType ), &nWrittenBytes, NULL ) == FALSE ||
			::WriteFile( m_hFile, &pSchema->nLength, sizeof( pSchema->nLength ), &nWrittenBytes, NULL ) == FALSE ||
			::WriteFile( m_hFile, &pSchema->nXPrec, sizeof( pSchema->nXPrec ), &nWrittenBytes, NULL ) == FALSE ||
			::WriteFile( m_hFile, &pSchema->nXScale, sizeof( pSchema->nXScale ), &nWrittenBytes, NULL ) == FALSE ||
			::WriteFile( m_hFile, &pSchema->StatusFlag.GetRawData(), sizeof( pSchema->StatusFlag.GetRawData() ), &nWrittenBytes, NULL ) == FALSE )
		{
			return false;
		}

		if( pSchema->StatusFlag.IsOn( COLUMN_SCHEMA::STATUS_FLAG_HAS_DEFAULT_VALUE ) == true )
		{
			if( WriteStringData< unsigned char >( m_hFile, pSchema->strDefaultValue ) == false )
			{
				return false;
			}
		}
		else
		{
			unsigned char nDefaultValueLength = 0;
			if( ::WriteFile( m_hFile, &nDefaultValueLength, sizeof( nDefaultValueLength ), &nWrittenBytes, NULL ) == FALSE )
			{
				return false;
			}
		}

		if( WriteStringData< unsigned char >( m_hFile, pSchema->strCollation ) == false )
		{
			return false;
		}
	}

	return true;
}

bool	RDUWriter::WriteIndexSchema( const std::vector< INDEX_SCHEMA* >& vIndexSchema )
{
	unsigned short nIndexCount = static_cast< unsigned short >( vIndexSchema.size() );

	DWORD nWrittenBytes = 0;
	if( ::WriteFile( m_hFile, &nIndexCount, sizeof( nIndexCount ), &nWrittenBytes, NULL ) == FALSE )
	{
		return false;
	}

	std::vector< INDEX_SCHEMA* >::const_iterator pos = vIndexSchema.begin();
	std::vector< INDEX_SCHEMA* >::const_iterator end = vIndexSchema.end();
	for( ; pos != end; ++pos )
	{
		const INDEX_SCHEMA* pSchema = (*pos);

		if(	::WriteFile( m_hFile, &pSchema->nID, sizeof( pSchema->nID ), &nWrittenBytes, NULL ) == FALSE ||
			::WriteFile( m_hFile, &pSchema->eType, sizeof( pSchema->eType ), &nWrittenBytes, NULL ) == FALSE ||
			::WriteFile( m_hFile, &pSchema->StatusFlag.GetRawData(), sizeof( pSchema->StatusFlag.GetRawData() ), &nWrittenBytes, NULL ) == FALSE )
		{
			return false;
		}


		if( WriteStringData< unsigned char >( m_hFile, pSchema->strName ) == false )
		{
			return false;
		}

		if( WriteIndexColumnInfo( pSchema->vColumnInfo ) == false )
		{
			return false;
		}
	}

	return true;
}

bool	RDUWriter::WriteIndexColumnInfo( const std::vector< INDEX_SCHEMA::COLUMN_INFO* >& vColumnInfo )
{
	unsigned short nColumnCount = static_cast< unsigned short >( vColumnInfo.size() );

	DWORD nWrittenBytes = 0;
	if( ::WriteFile( m_hFile, &nColumnCount, sizeof( nColumnCount ), &nWrittenBytes, NULL ) == FALSE )
	{
		return false;
	}

	std::vector< INDEX_SCHEMA::COLUMN_INFO* >::const_iterator pos = vColumnInfo.begin();
	std::vector< INDEX_SCHEMA::COLUMN_INFO* >::const_iterator end = vColumnInfo.end();
	for( ; pos != end; ++pos )
	{
		const INDEX_SCHEMA::COLUMN_INFO* pInfo = (*pos);

		if( WriteStringData< unsigned char >( m_hFile, pInfo->strName ) == false )
		{
			return false;
		}

		if( ::WriteFile( m_hFile, &pInfo->StatusFlag.GetRawData(), sizeof( pInfo->StatusFlag.GetRawData() ), &nWrittenBytes, NULL ) == FALSE )
		{
			return false;
		}
	}

	return true;
}

bool	RDUWriter::WriteRowCount( int nRowCount )
{
	DWORD nWrittenBytes = 0;
	if( ::WriteFile( m_hFile, &nRowCount, sizeof( nRowCount ), &nWrittenBytes, NULL ) == FALSE )
	{
		return false;
	}

	return true;
}

bool	RDUWriter::WriteColumnData( const void* pData, unsigned short nSize )
{
	DWORD nWrittenBytes = 0;
	if( ::WriteFile( m_hFile, pData, nSize, &nWrittenBytes, NULL ) == FALSE )
	{
		return false;
	}

	return true;
}

bool	RDUWriter::WriteColumnString( const char* szString, unsigned short nLen )
{
	DWORD nWrittenBytes = 0;
	unsigned short nSize = nLen * sizeof( char );
	if( ::WriteFile( m_hFile, &nSize, sizeof( nSize ), &nWrittenBytes, NULL ) == FALSE )
	{
		return false;
	}

	if( nSize > 0 )
	{
		if( ::WriteFile( m_hFile, szString, nSize, &nWrittenBytes, NULL ) == FALSE )
		{
			return false;
		}
	}

	return true;
}

bool	RDUWriter::WriteColumnString( const wchar_t* szString, unsigned short nLen )
{
	DWORD nWrittenBytes = 0;
	unsigned short nSize = nLen * sizeof( wchar_t );
	if( ::WriteFile( m_hFile, &nSize, sizeof( nSize ), &nWrittenBytes, NULL ) == FALSE )
	{
		return false;
	}

	if( nSize > 0 )
	{
		if( ::WriteFile( m_hFile, szString, nSize, &nWrittenBytes, NULL ) == FALSE )
		{
			return false;
		}
	}

	return true;
}
