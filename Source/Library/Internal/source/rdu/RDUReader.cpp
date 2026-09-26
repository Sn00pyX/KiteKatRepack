
#include "../../include/rdu/RDUReader.h"
#include "../../include/rdu/RDUFileHeader.h"

namespace
{

	template< typename Length_T >
	bool	ReadStringData( KStream* pStream, std::string* pString )
	{
		Length_T nLength = 0;
		if( pStream->Read( &nLength, sizeof( nLength ) ) != sizeof( nLength ) )
		{
			return false;
		}

		if( nLength > 0 )
		{
			char* pStringData = new char[nLength+1];
			if( pStream->Read( pStringData, nLength ) != nLength )
			{
				delete [] pStringData;
				return false;
			}

			pStringData[nLength] = '\0';
			*pString = pStringData;
			delete [] pStringData;
		}

		return true;
	}

};


RDUReader::RDUReader()
	: m_bIsMyStream( false )
	, m_pRDUFile( NULL )
	, m_nCurrentRowPos( 0 )
	, m_nTotalRowCount( 0 )
{
}

RDUReader::~RDUReader()
{
	Close();
}

bool	RDUReader::Open( const std::string& file_name )
{
	FILE* fp = NULL;
	fopen_s( &fp, file_name.c_str(), "rb" );
	if( fp == NULL )
	{
		return false;
	}

	m_pRDUFile = new KRealFile( fp );
	m_bIsMyStream = true;

	if( ReadHeader() == false )
	{
		return false;
	}

	return true;
}

bool	RDUReader::Open( KStream* pStream )
{
	m_pRDUFile = pStream;
	m_bIsMyStream = false;

	if( ReadHeader() == false )
	{
		return false;
	}

	return true;
}

bool	RDUReader::ReadHeader()
{
	if( CheckHeader() == false )
	{
		return false;
	}

	if( ReadColumnInfo() == false )
	{
		return false;
	}

	if( ReadIndexInfo() == false )
	{
		return false;
	}

	if( ReadDataHeader() == false )
	{
		return false;
	}

	return true;
}

void	RDUReader::Close()
{
	if( m_pRDUFile != NULL )
	{
		if( m_bIsMyStream == true )
		{
			delete m_pRDUFile;
		}

		m_pRDUFile = NULL;
		m_bIsMyStream = false;
	}

	{
		std::vector< COLUMN_SCHEMA* >::const_iterator pos = m_vColumnSchema.begin();
		std::vector< COLUMN_SCHEMA* >::const_iterator end = m_vColumnSchema.end();
		for( ; pos != end; ++pos )
		{
			const COLUMN_SCHEMA* pSchema = (*pos);
			if( pSchema != NULL )
			{
				delete pSchema;
			}
		}

		m_vColumnSchema.clear();
	}

	{
		std::vector< INDEX_SCHEMA* >::const_iterator pos = m_vIndexSchema.begin();
		std::vector< INDEX_SCHEMA* >::const_iterator end = m_vIndexSchema.end();
		for( ; pos != end; ++pos )
		{
			const INDEX_SCHEMA* pSchema = (*pos);
			if( pSchema != NULL )
			{
				delete pSchema;
			}
		}

		m_vIndexSchema.clear();
	}

	m_nCurrentRowPos = 0;
	m_nTotalRowCount = 0;
}

const std::vector< COLUMN_SCHEMA* >&	RDUReader::GetColumnSchema()	const
{
	return m_vColumnSchema;
}

const std::vector< INDEX_SCHEMA* >&	RDUReader::GetIndexSchema()	const
{
	return m_vIndexSchema;
}

int		RDUReader::GetRowCount()	const
{
	return m_nTotalRowCount;
}

bool	RDUReader::CheckHeader()
{
	RDU_FILE_HEADER RDUFileHeader;

	if( m_pRDUFile->Read( &RDUFileHeader, sizeof( RDUFileHeader ) ) != sizeof( RDUFileHeader ) )
	{
		return false;
	}

	if( ::strncmp( RDUFileHeader.szFileTag, RDUFILE_HEADER_TAG, RDU_FILE_HEADER::FILE_TAG_LENGTH ) )
	{
		return false;
	}

	if( RDUFileHeader.nMajorVersion != RDUFILE_MAJOR_VERSION || RDUFileHeader.nMinorVersion != RDUFILE_MINOR_VERSION )
	{
		return false;
	}

	return true;
}

bool	RDUReader::ReadColumnInfo()
{
	unsigned short nColumnCount = 0;
	if( m_pRDUFile->Read( &nColumnCount, sizeof( nColumnCount ) ) != sizeof( nColumnCount ) )
	{
		return false;
	}

	for( int nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex )
	{
		COLUMN_SCHEMA* pCS = new COLUMN_SCHEMA;
		if( ReadColumnSchema( pCS ) == false )
		{
			delete pCS;
			return false;
		}

		m_vColumnSchema.push_back( pCS );
	}

	return true;
}

bool	RDUReader::ReadColumnSchema( COLUMN_SCHEMA* pCS )
{
	if( ReadStringData< unsigned char >( m_pRDUFile, &pCS->strName ) == false )
	{
		return false;
	}

	DWORD nReadBytes = 0;
	StatusFlagRawDataType nColumnStatusFlag;
	if(	m_pRDUFile->Read( &pCS->eXUserType, sizeof( pCS->eXUserType ) ) != sizeof( pCS->eXUserType ) ||
		m_pRDUFile->Read( &pCS->nLength, sizeof( pCS->nLength ) ) != sizeof( pCS->nLength ) ||
		m_pRDUFile->Read( &pCS->nXPrec, sizeof( pCS->nXPrec ) ) != sizeof( pCS->nXPrec ) ||
		m_pRDUFile->Read( &pCS->nXScale, sizeof( pCS->nXScale ) ) != sizeof( pCS->nXScale ) ||
		m_pRDUFile->Read( &nColumnStatusFlag, sizeof( nColumnStatusFlag ) ) != sizeof( nColumnStatusFlag ) )
	{
		return false;
	}

	pCS->StatusFlag.SetRawData( nColumnStatusFlag );


	if( ReadStringData< unsigned char >( m_pRDUFile, &pCS->strDefaultValue ) == false )
	{
		return false;
	}

	if( ReadStringData< unsigned char >( m_pRDUFile, &pCS->strCollation ) == false )
	{
		return false;
	}

	return true;
}

bool	RDUReader::ReadIndexInfo()
{
	unsigned short nIndexCount = 0;
	if( m_pRDUFile->Read( &nIndexCount, sizeof( nIndexCount ) ) != sizeof( nIndexCount ) )
	{
		return false;
	}

	for( int nIndex = 0; nIndex < nIndexCount; ++nIndex )
	{
		INDEX_SCHEMA* pIS = new INDEX_SCHEMA;
		if( ReadIndexSchema( pIS ) == false )
		{
			delete pIS;
			return false;
		}

		m_vIndexSchema.push_back( pIS );
	}

	return true;
}

bool	RDUReader::ReadIndexSchema( INDEX_SCHEMA* pIS )
{
	StatusFlagRawDataType nStatusFlagRawDataBuffer;

	if(	m_pRDUFile->Read( &pIS->nID, sizeof( pIS->nID ) ) != sizeof( pIS->nID ) ||
		m_pRDUFile->Read( &pIS->eType, sizeof( pIS->eType ) ) != sizeof( pIS->eType ) ||
		m_pRDUFile->Read( &nStatusFlagRawDataBuffer, sizeof( nStatusFlagRawDataBuffer ) ) != sizeof( nStatusFlagRawDataBuffer ) )
	{
		return false;
	}

	pIS->StatusFlag.SetRawData( nStatusFlagRawDataBuffer );

	if( ReadStringData< unsigned char >( m_pRDUFile, &pIS->strName ) == false )
	{
		return false;
	}

	unsigned short nColumnCount = 0;
	if( m_pRDUFile->Read( &nColumnCount, sizeof( nColumnCount ) ) != sizeof( nColumnCount ) )
	{
		return false;
	}

	for( int nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex )
	{
		INDEX_SCHEMA::COLUMN_INFO* pCI = new INDEX_SCHEMA::COLUMN_INFO;

		if( ReadStringData< unsigned char >( m_pRDUFile, &pCI->strName ) == false )
		{
			delete pCI;
			return false;
		}

		if( m_pRDUFile->Read( &nStatusFlagRawDataBuffer, sizeof( nStatusFlagRawDataBuffer ) ) != sizeof( nStatusFlagRawDataBuffer ) )
		{
			delete pCI;
			return false;
		}

		pCI->StatusFlag.SetRawData( nStatusFlagRawDataBuffer );

		pIS->vColumnInfo.push_back( pCI );
	}

	return true;
}

bool	RDUReader::ReadDataHeader()
{
	if( m_pRDUFile->Read( &m_nTotalRowCount, sizeof( m_nTotalRowCount ) ) != sizeof( m_nTotalRowCount ) )
	{
		return false;
	}

	m_nCurrentRowPos = 0;
	return true;
}

bool	RDUReader::Fetch( RDURow* pRow )
{
	if( m_nCurrentRowPos >= m_nTotalRowCount )
	{
		return false;
	}

	if( ReadRow( pRow ) == false )
	{
		pRow->Clear();
		return false;
	}

	++m_nCurrentRowPos;
	return true;
}

bool	RDUReader::ReadRow( RDURow* pRow )
{
	if( pRow->vDatas.size() != m_vColumnSchema.size() )
	{
		pRow->Clear();
	}

	std::vector< COLUMN_SCHEMA* >::const_iterator pos = m_vColumnSchema.begin();
	std::vector< COLUMN_SCHEMA* >::const_iterator end = m_vColumnSchema.end();

	for( int nIndex = 0; pos != end; ++pos, ++nIndex )
	{
		COLUMN_SCHEMA* pColumn = (*pos);

		ColumnData* pCD = pRow->At( nIndex );
		if( pCD == NULL )
		{
			pCD = new ColumnData;
			pRow->vDatas.push_back( pCD );
		}

		pCD->pSchema = pColumn;

		if( ReadColumnData( pCD ) == false )
		{
			return false;
		}
	}

	return true;
}

bool	RDUReader::ReadColumnData( ColumnData* pCD )
{
	// NULL 허용인 컬럼의 경우 NULL 값 여부를 나타내는 1바이트 확인
	if( pCD->pSchema->IsNullable() == true )
	{
		bool bIsNull = false;
		if( m_pRDUFile->Read( &bIsNull, sizeof( bIsNull ) ) != sizeof( bIsNull ) )
		{
			return false;
		}

		pCD->bIsNull = bIsNull;
	}

	if( pCD->bIsNull == false )
	{
		pCD->nDataSize = pCD->pSchema->nLength;
		unsigned short nBytesToAlloc = pCD->nDataSize;
		switch( pCD->pSchema->eXUserType )
		{
		case COLUMN_SCHEMA::USERTYPE_VARCHAR:
		case COLUMN_SCHEMA::USERTYPE_CHAR:
		case COLUMN_SCHEMA::USERTYPE_NVARCHAR:
		case COLUMN_SCHEMA::USERTYPE_NCHAR:
			{
				if( m_pRDUFile->Read( &pCD->nDataSize, sizeof( pCD->nDataSize ) ) != sizeof( pCD->nDataSize ) )
				{
					return false;
				}

				nBytesToAlloc = pCD->nDataSize;

				nBytesToAlloc += sizeof( char );	// NULL 추가를 위해
				if( pCD->pSchema->eXUserType == COLUMN_SCHEMA::USERTYPE_NVARCHAR ||
					pCD->pSchema->eXUserType == COLUMN_SCHEMA::USERTYPE_NCHAR )
				{
					nBytesToAlloc += (sizeof( wchar_t ) - sizeof( char ));	// 유니코드
				}
			}
			break;
		}

		assert( nBytesToAlloc >= pCD->nDataSize );
		if( nBytesToAlloc < pCD->nDataSize )
		{
			return false;
		}

		if( pCD->pData == NULL || pCD->nAllocSize < nBytesToAlloc )
		{
			pCD->pData = new char[nBytesToAlloc];
			pCD->nAllocSize = nBytesToAlloc;
		}

		if( m_pRDUFile->Read( pCD->pData, pCD->nDataSize ) != pCD->nDataSize )
		{
			return false;
		}

		if( pCD->pSchema->eXUserType == COLUMN_SCHEMA::USERTYPE_VARCHAR ||
			pCD->pSchema->eXUserType == COLUMN_SCHEMA::USERTYPE_CHAR )
		{
			pCD->pData[pCD->nDataSize] = '\0';
		}
		else if( pCD->pSchema->eXUserType == COLUMN_SCHEMA::USERTYPE_NVARCHAR ||
			pCD->pSchema->eXUserType == COLUMN_SCHEMA::USERTYPE_NCHAR )
		{
			wchar_t* pWData = reinterpret_cast< wchar_t* >( pCD->pData );
			pWData[pCD->nDataSize/sizeof( wchar_t )] = L'\0';
		}
	}

	return true;
}
