
#include "../../include/kfile/KDataObject.h"
#include "../../include/kfile/KFiler.h"
#include "../../include/toolkit/safe_function.h"
#include <assert.h>
#include <string>


namespace {

	char whitespacechr[] = {
		'\n', '\r', '{', ' ', '\t', '}' , ','
	};


	char whitespacechr2[] = {
		'\n', '\r', ' ', '\t', ','
	};


	// 공백문자들을 건너뛰고 나서 처음 나오는 문자가 '{' 임을 확인
	bool isNextOBrace( KStream &stream )
	{
		char chr;
		bool isWhiteSpace;
		do  {
			isWhiteSpace = false;
			if ( stream.Read( &chr, 1 ) ) {
				for (int i = 0; i < _countof(whitespacechr); i++ ) {
					if ( chr == whitespacechr[i] ) {
						if ( chr == '{' ) {
							return true;
						} else if (chr == '}') {
							return false;
						} else {
							isWhiteSpace = true;
						}
					}
				}
				if (!isWhiteSpace)
					return false;
			} else {
				return false;
			}
		} while (isWhiteSpace);
		return false;
	}

	// 공백문자들을 건너뛰고 나서 처음 나오는 문자가 '}' 임을 확인
	bool isNextCBrace( KStream &stream )
	{
		char chr;
		bool isWhiteSpace;
		do  {
			isWhiteSpace = false;
			if ( stream.Read( &chr, 1 ) ) {
				for (int i = 0; i < _countof(whitespacechr); i++ ) {
					if ( chr == whitespacechr[i] ) {
						if ( chr == '}' ) {
							return true;
						} else if (chr == '{') {
							return false;
						} else {
							isWhiteSpace = true;
						}
					}
				}
				if (!isWhiteSpace)
					return false;
			} else {
				return false;
			}
		} while (isWhiteSpace);
		return false;
	}

	// 공백문자들 다음에 나오는 문자열 token을 얻는다.
	bool getNextToken( KStream &stream, std::string &token )
	{
		char chr;
		bool isWhiteSpace;
		token.erase();

		// whitespacechr에 정의된 공백부분을 읽고 지나치다가 공백이 아닌 문자가 나오면 정지
		do {
			isWhiteSpace = false;
			if ( stream.Read( &chr, 1 ) ) {
				for ( int i=0 ; i<_countof(whitespacechr2) ; i++ )	{
					if ( chr == whitespacechr2[i] ) {
						isWhiteSpace = true;
						break;
					}
				}
			}
			else return false;
		} while ( isWhiteSpace == true );

		if( chr == '{' || chr == '}' )
			return false;

		// 다음 공백이 나올때까지 문자를 읽어서 token에 저장
		stream.Seek( -1, KStream::seekCur );
		while (1) {
			if ( stream.Read ( &chr, 1 ) ) {
				for ( int i=0 ; i<_countof(whitespacechr) ; i++ ) {
					if ( chr == whitespacechr[i] ) {
						isWhiteSpace = true;
						break;
					}
				}
			}
			else return false;
			if ( !isWhiteSpace )
				token += chr;
			else
				break;
		}
		stream.Seek( -1, KStream::seekCur );
		return true;
	}

	// 공백문자들 다음에 나오는 따옴표로 둘러싸인 문자열을 얻는다.
	bool getNextString( KStream &stream, std::string &token )
	{
		char chr;
		bool isQuote = false;
		token.erase();

		// 쌍따옴표 (")가 나올때까지 읽고 지나감
		do {
			if ( stream.Read( &chr, 1 ) ) {
				if ( chr == 34 )
					isQuote = true;
			}
			else 
				return false;
		} while ( !isQuote );

		// 다음 쌍따옴표가 나올때까지 문자를 읽어서 token에 저장
		isQuote = false;

		do {
			if ( stream.Read ( &chr, 1 ) ) {
				if ( chr == 34 )
					isQuote = true;
			}
			else 
				return false;
			if ( !isQuote )
				token += chr;
		} while ( !isQuote ) ;
		return true;
	}

}

void KDataObject::AddDataObject( KDataObject *obj )
{
	m_pDataObjects.push_back( obj );
}

// SimpleDataObject

KSimpleDataObject::KSimpleDataObject( CLASSTYPEID idclass) 
: KDataObject( idclass )
{
	switch( idclass )
	{
	case CLASSTYPEID_WORD:
		m_nSize = 2;
		break;
	case CLASSTYPEID_DWORD:
	case CLASSTYPEID_FLOAT:
		m_nSize = 4;
		break;
	case CLASSTYPEID_CHAR:
	case CLASSTYPEID_UCHAR:
		m_nSize = 1;
		break;
	case CLASSTYPEID_STRING:
		m_nSize = 0;
		break;
	default:
		m_nSize = 0;
	}
	m_pStr = NULL;
	m_data._4byteint = 0;
}

KSimpleDataObject::~KSimpleDataObject()
{
	if ( m_pStr ) delete[] m_pStr;
}

bool KSimpleDataObject::SetDWORD( DWORD data )
{
	if ( m_idClassType == CLASSTYPEID_DWORD )
	{
		m_data._4byteint = data;
		m_nSize = 4;
		return true;
	}
	return false;
}

bool KSimpleDataObject::SetWORD( WORD data )
{
	if ( m_idClassType == CLASSTYPEID_WORD )
	{
		m_data._2byteint = data;
		m_nSize = 2;
		return true;
	}
	return false;
}

bool KSimpleDataObject::SetFloat( float data )
{
	if ( m_idClassType == CLASSTYPEID_FLOAT )
	{
		m_data._4bytefloat = data;
		m_nSize = 4;
		return true;
	}
	return false;
}

bool KSimpleDataObject::SetUChar( unsigned char data )
{
	if ( m_idClassType == CLASSTYPEID_UCHAR )
	{
		m_data._1byteuchar = data;
		m_nSize = 1;
		return true;
	}
	return false;
}

bool KSimpleDataObject::SetChar( char data )
{
	if ( m_idClassType == CLASSTYPEID_CHAR )
	{
		m_data._1bytechar = data;
		m_nSize = 1;
		return true;
	}
	return false;
}

bool KSimpleDataObject::SetString( const char *data, int nLen )
{
	if ( m_idClassType == CLASSTYPEID_STRING )
	{
		if ( m_pStr ) delete[] m_pStr;
		m_pStr = new char[nLen+1];
		m_pStr[nLen] = '\0';
		s_memcpy( m_pStr, nLen, data, nLen );
		m_nSize = nLen+1;
		return true;
	}
	return false;
}


bool KSimpleDataObject::SetData(const void *data, int nSize)
{
	if ( m_idClassType == CLASSTYPEID_STRING )
	{
		if ( m_pStr ) delete[] m_pStr;
		m_pStr = new char[nSize+1];
		m_pStr[nSize] = '\0';
		s_memcpy( m_pStr, nSize, data, nSize );
		m_nSize = nSize+1;
		return true;
	}
	else if ( nSize == m_nSize )
	{
		switch( m_idClassType )
		{
		case CLASSTYPEID_WORD:
			s_memcpy( &m_data._2byteint, sizeof( m_data._2byteint ), data, m_nSize );
			break;
		case CLASSTYPEID_DWORD:
		case CLASSTYPEID_FLOAT:
			s_memcpy( &m_data._4byteint, sizeof( m_data._4byteint ), data, m_nSize );
			break;
		case CLASSTYPEID_CHAR:
		case CLASSTYPEID_UCHAR:
			s_memcpy( &m_data._1byteuchar, sizeof( m_data._1byteuchar ), data, m_nSize );
			break;
		default:
			return false;
		}
		return true;
	}
	return false;
}


bool KSimpleDataObject::Save( KStream &stream, FileMode filemode )
{
	if ( SaveHeader( stream, filemode ) )
	{
		return SaveBody( stream, filemode );
	}
	return false;
}

bool KSimpleDataObject::Load( KStream &stream, FileMode filemode )
{
	if( LoadHeader( stream, filemode ) )
	{
		return LoadBody( stream, filemode );
	}
	return false;
}

bool KSimpleDataObject::SaveHeader( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		switch( m_idClassType )
		{
		case CLASSTYPEID_WORD:
			stream.Write( &KTOKEN_WORD, 1 );
			break;
		case CLASSTYPEID_DWORD:
			stream.Write( &KTOKEN_DWORD, 1 );
			break;
		case CLASSTYPEID_FLOAT:
			stream.Write( &KTOKEN_FLOAT, 1 );
			break;
		case CLASSTYPEID_CHAR:
			stream.Write( &KTOKEN_CHAR, 1 );
			break;
		case CLASSTYPEID_UCHAR:
			stream.Write( &KTOKEN_UCHAR, 1 );
			break;
		case CLASSTYPEID_STRING:
			stream.Write( &KTOKEN_STRING, 1 );
			break;
		default:
			return false;
		}
		return true;
	}
	else if ( filemode == KDFM_ASCII )
	{
		return true;
	}
	return false;
}

bool KSimpleDataObject::LoadHeader( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		char typetoken;
		stream.Read( &typetoken, 1 );
		switch( typetoken )
		{
		case KTOKEN_WORD:
			m_idClassType = CLASSTYPEID_WORD;
			m_nSize = 2;
			break;
		case KTOKEN_DWORD:
			m_idClassType = CLASSTYPEID_DWORD;
			m_nSize = 4;
			break;
		case KTOKEN_FLOAT:
			m_idClassType = CLASSTYPEID_FLOAT;
			m_nSize = 4;
			break;
		case KTOKEN_CHAR:
			m_idClassType = CLASSTYPEID_CHAR;
			m_nSize = 1;
			break;
		case KTOKEN_UCHAR:
			m_idClassType = CLASSTYPEID_UCHAR;
			m_nSize = 1;
			break;
		case KTOKEN_STRING:
			m_idClassType = CLASSTYPEID_STRING;
			break;
		default:
			return false;
		}
		return true;
	}
	else if ( filemode == KDFM_ASCII )
	{
		return true;
	}
	return false;
}

bool KSimpleDataObject::SaveBody( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		int writesize;
		switch( m_idClassType )
		{
		case CLASSTYPEID_WORD:
			writesize = static_cast<int>(stream.Write( &m_data._2byteint, m_nSize ));
			break;
		case CLASSTYPEID_DWORD:
			writesize = static_cast<int>(stream.Write( &m_data._4byteint, m_nSize ));
			break;
		case CLASSTYPEID_FLOAT:
			writesize = static_cast<int>(stream.Write( &m_data._4bytefloat, m_nSize ));
			break;
		case CLASSTYPEID_CHAR:
			writesize = static_cast<int>(stream.Write( &m_data._1bytechar, m_nSize ));
			break;
		case CLASSTYPEID_UCHAR:
			writesize = static_cast<int>(stream.Write( &m_data._1byteuchar, m_nSize ));
			break;
		case CLASSTYPEID_STRING:
			stream.Write( &m_nSize, sizeof(m_nSize) );
			writesize = static_cast<int>(stream.Write( m_pStr, m_nSize ));
			break;
		default:
			return false;
		}
		return ( writesize == m_nSize );
	}

	else if ( filemode == KDFM_ASCII )
	{
		char buff[512];
		switch( m_idClassType )
		{
		case CLASSTYPEID_WORD:
			s_sprintf(buff, _countof( buff ), "%d", int( m_data._2byteint ));
			stream.Write( buff, strlen(buff));
			break;
		case CLASSTYPEID_DWORD:
			s_sprintf(buff, _countof( buff ), "%d", m_data._4byteint);
			stream.Write( buff, strlen(buff));
			break;
		case CLASSTYPEID_FLOAT:
			s_sprintf(buff, _countof( buff ), "%f", m_data._4bytefloat);
			stream.Write( buff, strlen(buff));
			break;
		case CLASSTYPEID_CHAR:
			s_sprintf(buff, _countof( buff ), "%d", int( m_data._1bytechar ) );
			stream.Write( buff, strlen(buff));
			break;
		case CLASSTYPEID_UCHAR:
			s_sprintf(buff, _countof( buff ), "%d", int( m_data._1byteuchar ) );
			stream.Write( buff, strlen(buff));
			break;
		case CLASSTYPEID_STRING:
			s_sprintf(buff, _countof( buff ), "\"%s\"", m_pStr);
			stream.Write(buff,strlen(buff));
			break;
		default:
			return false;
		}
		return true;
	}
	return false;
}

bool KSimpleDataObject::LoadBody( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		int readsize;
		switch( m_idClassType )
		{
		case CLASSTYPEID_WORD:
			readsize = static_cast<int>(stream.Read( &m_data._2byteint, m_nSize ));
			break;
		case CLASSTYPEID_DWORD:
			readsize = static_cast<int>(stream.Read( &m_data._4byteint, m_nSize ));
			break;
		case CLASSTYPEID_FLOAT:
			readsize = static_cast<int>(stream.Read( &m_data._4bytefloat, m_nSize ));
			break;
		case CLASSTYPEID_CHAR:
			readsize = static_cast<int>(stream.Read( &m_data._1bytechar, m_nSize ));
			break;
		case CLASSTYPEID_UCHAR:
			readsize = static_cast<int>(stream.Read( &m_data._1byteuchar, m_nSize ));
			break;
		case CLASSTYPEID_STRING:
			stream.Read( &m_nSize, sizeof(m_nSize) );
			if ( m_pStr ) delete[] m_pStr;
			m_pStr = new char[m_nSize+1];
			m_pStr[m_nSize] = '\0';
			readsize = static_cast<int>(stream.Read( m_pStr, m_nSize ));
			break;
		default:
			return false;
		}
		return ( readsize == m_nSize );
	}
	else if ( filemode == KDFM_ASCII )
	{
		std::string token;

		if ( m_idClassType == CLASSTYPEID_STRING )
		{
			if ( !getNextString( stream, token ) )
				return false;
		}
		else
		{
			if (! getNextToken( stream, token ) )
				return false;
		}

		switch( m_idClassType )
		{
		case CLASSTYPEID_WORD:
			SetWORD((unsigned short)atoi(token.c_str() ));
			break;
		case CLASSTYPEID_DWORD:
			SetDWORD((unsigned long)atoi( token.c_str() ));
			break;
		case CLASSTYPEID_FLOAT:
			SetFloat((float)atof( token.c_str() ));
			break;
		case CLASSTYPEID_CHAR:
			{
				int value = atoi( token.c_str() );
				SetChar( char( value ) );
			}
			break;
		case CLASSTYPEID_UCHAR:
			{
				int value = atoi( token.c_str() );
				SetUChar( unsigned char( value ) );
			}
			break;
		case CLASSTYPEID_STRING:
			SetString( token.c_str(), static_cast<int>(token.size()) );
			break;
		default:
			return false;
		}
		return true;
	}
	return false;
}

bool KSimpleDataObject::LoadFast(KStream& stream)
{
	assert(m_idClassType != CLASSTYPEID_NONE);
	stream.Seek(1,KStream::seekCur);
	return LoadBody(stream);
}

int KSimpleDataObject::GetIndexValue()
{
	switch( m_idClassType )
	{
	case CLASSTYPEID_WORD:
		return static_cast<int>(m_data._2byteint);
	case CLASSTYPEID_DWORD:
		return static_cast<int>(m_data._4byteint);
	case CLASSTYPEID_FLOAT:
		return static_cast<int>(m_data._4bytefloat);
	case CLASSTYPEID_CHAR:
		return static_cast<int>(m_data._1bytechar);
	case CLASSTYPEID_UCHAR:
		return static_cast<int>(m_data._1byteuchar);
	}
	return -1;
}

KDataObject* KSimpleDataObject::CloneTypeInfo(KTemplateDataObject* pParent)
{
	KSimpleDataObject* pObject = new KSimpleDataObject(m_idClassType);
	pObject->m_nSize = m_nSize;
	return pObject;
}
bool KSimpleDataArrayObject::SetDataString( int index, const char *str )
{
	if ( m_idClassType == CLASSTYPEID_ARRAY_STRING )
	{
		m_vecStringArray[index] = str;
		return true;
	}
	return false;
}

bool KSimpleDataArrayObject::SetDataChar( int index, const char *data, int nCount )
{
	if ( m_idClassType == CLASSTYPEID_ARRAY_CHAR )
	{
		if ( m_pArrayData )
		{
			m_nDataSize = 1;
			size_t byte_offset = m_nDataSize*index;
			size_t data_size = GetDataCount() * m_nDataSize;
			s_memcpy( m_pArrayData + byte_offset, data_size - byte_offset, data, m_nDataSize*nCount );
			return true;
		}
	}
	return false;
}

bool KSimpleDataArrayObject::SetDataUChar( int index, const unsigned char *data, int nCount )
{
	if ( m_idClassType == CLASSTYPEID_ARRAY_UCHAR )
	{
		if ( m_pArrayData )
		{
			m_nDataSize = 1;
			size_t byte_offset = m_nDataSize*index;
			size_t data_size = GetDataCount() * m_nDataSize;
			s_memcpy( m_pArrayData + byte_offset, data_size - byte_offset, data, m_nDataSize*nCount );
			return true;
		}
	}
	return false;
}

bool KSimpleDataArrayObject::SetDataDWORD( int index, const DWORD *data, int nCount )
{
	if ( m_idClassType == CLASSTYPEID_ARRAY_DWORD )
	{
		if ( m_pArrayData )
		{
			m_nDataSize = 4;
			size_t byte_offset = m_nDataSize*index;
			size_t data_size = GetDataCount() * m_nDataSize;
			s_memcpy( m_pArrayData + byte_offset, data_size - byte_offset, data, m_nDataSize*nCount );
			return true;
		}
	}
	return false;
}

bool KSimpleDataArrayObject::SetDataWORD( int index, const WORD *data, int nCount )
{
	if ( m_idClassType == CLASSTYPEID_ARRAY_WORD )
	{
		if ( m_pArrayData )
		{
			m_nDataSize = 2;
			size_t byte_offset = m_nDataSize*index;
			size_t data_size = GetDataCount() * m_nDataSize;
			s_memcpy( m_pArrayData + byte_offset, data_size - byte_offset, data, m_nDataSize*nCount );
			return true;
		}
	}
	return false;
}

bool KSimpleDataArrayObject::SetDataFloat( int index, const float *data, int nCount )
{
	if ( m_idClassType == CLASSTYPEID_ARRAY_FLOAT )
	{
		if ( m_pArrayData )
		{
			m_nDataSize = 4;
			size_t byte_offset = m_nDataSize*index;
			size_t data_size = GetDataCount() * m_nDataSize;
			s_memcpy( m_pArrayData + byte_offset, data_size - byte_offset, data, m_nDataSize*nCount );
			return true;
		}
	}
	return false;
}


void KSimpleDataArrayObject::SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
{
	m_nDataCount = nCount;
	m_pParent = pParent;

	if ( m_nDataCount == -1 )
	{
		m_strVarCount = varCount;
		m_varCount = m_pParent->GetMemberObject( varCount );
	}
	RefreshArray();
}

void KSimpleDataArrayObject::RefreshArray()
{
	if ( m_idClassType == CLASSTYPEID_STRING ||
		m_idClassType == CLASSTYPEID_ARRAY_STRING )
	{
		m_vecStringArray.resize(GetDataCount());
		if ( m_pArrayData ) delete[] m_pArrayData;
		m_pArrayData = NULL;
	}
	else
	{
		m_vecStringArray.clear();
		switch( m_idClassType )
		{
		case CLASSTYPEID_UCHAR:
		case CLASSTYPEID_ARRAY_UCHAR:
			m_nDataSize = 1;
			m_idClassType = CLASSTYPEID_ARRAY_UCHAR;
			break;
		case CLASSTYPEID_CHAR:
		case CLASSTYPEID_ARRAY_CHAR:
			m_nDataSize = 1;
			m_idClassType = CLASSTYPEID_ARRAY_CHAR;
			break;
		case CLASSTYPEID_WORD:
		case CLASSTYPEID_ARRAY_WORD:
			m_nDataSize = 2;
			m_idClassType = CLASSTYPEID_ARRAY_WORD;
			break;
		case CLASSTYPEID_DWORD:
		case CLASSTYPEID_ARRAY_DWORD:
			m_idClassType = CLASSTYPEID_ARRAY_DWORD;
			m_nDataSize = 4;
			break;
		case CLASSTYPEID_FLOAT:
		case CLASSTYPEID_ARRAY_FLOAT:
			m_idClassType = CLASSTYPEID_ARRAY_FLOAT;
			m_nDataSize = 4;
			break;
		default:
			return;
		}
		if ( m_pArrayData ) delete[] m_pArrayData;
		m_pArrayData = new char[GetDataCount() * m_nDataSize];
	}
}


bool KSimpleDataArrayObject::Save( KStream &stream, FileMode filemode )
{
	if ( SaveHeader( stream, filemode ) )
	{
		return SaveBody( stream, filemode );
	}
	return false;
}

bool KSimpleDataArrayObject::Load( KStream &stream, FileMode filemode )
{
	if ( LoadHeader( stream, filemode ) )
	{
		return LoadBody( stream, filemode );
	}
	return false;
}

bool KSimpleDataArrayObject::SaveHeader( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		stream.Write( &KTOKEN_ARRAY, 1 );
		switch( m_idClassType )
		{
		case CLASSTYPEID_ARRAY_STRING:
			stream.Write( &KTOKEN_STRING, 1 );
			break;
		case CLASSTYPEID_ARRAY_UCHAR:
			stream.Write( &KTOKEN_UCHAR, 1 );
			break;
		case CLASSTYPEID_ARRAY_CHAR:
			stream.Write( &KTOKEN_CHAR, 1 );
			break;
		case CLASSTYPEID_ARRAY_WORD:
			stream.Write( &KTOKEN_WORD, 1 );
			break;
		case CLASSTYPEID_ARRAY_DWORD:
			stream.Write( &KTOKEN_DWORD, 1 );
			break;
		case CLASSTYPEID_ARRAY_FLOAT:
			stream.Write( &KTOKEN_FLOAT, 1 );
			break;
		default:
			return false;
		}
		int nCount = GetDataCount();
		stream.Write( &nCount, 4 );

		if ( m_nDataCount == -1 )
		{
			int strCount = static_cast<int>(m_strVarCount.size()) + 1;
			stream.Write( &strCount, 4 );
			stream.Write( m_strVarCount.c_str(), strCount );
		}
		else
		{
			int strCount = 0;
			stream.Write( &strCount, 4 );
		}
		return true;
	}
	if ( filemode == KDFM_ASCII )
	{
		return true;
	}
	return false;
}

bool KSimpleDataArrayObject::LoadHeader( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		char token;
		stream.Read( &token, 1 );
		if ( token == KTOKEN_ARRAY )
		{
			stream.Read( &token, 1 );
			switch( token )
			{
			case KTOKEN_UCHAR:
				m_idClassType = CLASSTYPEID_ARRAY_UCHAR;
				m_nDataSize = 1;
				break;
			case KTOKEN_CHAR:
				m_idClassType = CLASSTYPEID_ARRAY_CHAR;
				m_nDataSize = 1;
				break;
			case KTOKEN_WORD:
				m_idClassType = CLASSTYPEID_ARRAY_WORD;
				m_nDataSize = 2;
				break;
			case KTOKEN_DWORD:
				m_idClassType = CLASSTYPEID_ARRAY_DWORD;
				m_nDataSize = 4;
				break;
			case KTOKEN_FLOAT:
				m_idClassType = CLASSTYPEID_ARRAY_FLOAT;
				m_nDataSize = 4;
				break;
			case KTOKEN_STRING:
				m_idClassType = CLASSTYPEID_ARRAY_STRING;
				m_nDataSize = 0;
				break;
			default:
				return false;
			}

			stream.Read( &m_nDataCount, 4 );			

			// 이만큼 읽을 필요 없음.
			stream.Read( &m_strLength, 4 );
			stream.Seek(m_strLength, KStream::seekCur);

			RefreshArray();
			return true;
		}
	}
	else if ( filemode == KDFM_ASCII )
	{
		RefreshArray();
		return true;
	}
	return false;
}

bool KSimpleDataArrayObject::SaveBody( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		int nCount = GetDataCount();
		if ( m_idClassType == CLASSTYPEID_ARRAY_STRING )
		{
			for ( int i=0 ; i<nCount ; ++i )
			{
				const char *str = m_vecStringArray[i].c_str();
				int len = static_cast<int>(m_vecStringArray[i].size()) + 1;
				stream.Write( &len, 4 );
				stream.Write( str, len );
			}
			return true;
		}
		return stream.Write( m_pArrayData, m_nDataSize*nCount ) == (size_t)m_nDataSize*nCount;
	}
	if ( filemode == KDFM_ASCII )
	{
		char buff[512];
		int nCount = GetDataCount();
		if ( m_idClassType == CLASSTYPEID_ARRAY_STRING )
		{
			s_sprintf(buff, _countof( buff ), "{ ");
			stream.Write(buff,strlen("{ "));
			for ( int i=0 ; i<nCount ; ++i )
			{
				s_sprintf(buff, _countof( buff ), "\"%s\", ", m_vecStringArray[i].c_str());
				stream.Write(buff,strlen(buff));
			}
			stream.Write(buff,strlen(buff));
			s_sprintf(buff, _countof( buff ), "}");
			stream.Write(buff,strlen("}"));

			return true;
		}
		else 
		{
			s_sprintf(buff, _countof( buff ), "{ ");
			stream.Write(buff,strlen("{ "));
			for (int i=0 ; i<nCount ; ++i )
			{
				char temp[4];
				s_memcpy( temp, sizeof( temp ), m_pArrayData + m_nDataSize * i, m_nDataSize );
				switch( m_idClassType )
				{
				case CLASSTYPEID_ARRAY_WORD:
					s_sprintf(buff, _countof( buff ), "%d", *((WORD*)temp));
					stream.Write( buff, strlen(buff));
					break;
				case CLASSTYPEID_ARRAY_DWORD:
					s_sprintf(buff, _countof( buff ), "%lu", *((DWORD*)temp));
					stream.Write( buff, strlen(buff));
					break;
				case CLASSTYPEID_ARRAY_FLOAT:
					s_sprintf(buff, _countof( buff ), "%f", *((float*)temp));
					stream.Write( buff, strlen(buff));
					break;
				case CLASSTYPEID_ARRAY_CHAR:
					s_sprintf(buff, _countof( buff ), "%c", *((char*)temp));
					stream.Write( buff, strlen(buff));
					break;
				case CLASSTYPEID_ARRAY_UCHAR:
					s_sprintf(buff, _countof( buff ), "%c", *((unsigned char*)temp));
					stream.Write( buff, strlen(buff));
					break;
				default:
					return false;
				}
				if( i != nCount-1 ) stream.Write(", ", strlen(", "));
			}
			s_sprintf(buff, _countof( buff ), "}");
			stream.Write(buff,strlen("}"));
			return true;
		}
	}
	return false;
}

bool KSimpleDataArrayObject::LoadBody( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		int nCount = GetDataCount();
		if ( m_idClassType == CLASSTYPEID_ARRAY_STRING )
		{
			std::vector<char> tempstr;
			for ( int i=0 ; i<nCount ; ++i )
			{
				int len;
				stream.Read( &len, 4 );
				tempstr.resize( len + 1 );
				stream.Read( &tempstr.front(), len );
				m_vecStringArray[i] = &tempstr.front();
			}			
		}
		else
		{
			return stream.Read( m_pArrayData, m_nDataSize * nCount ) == (size_t)nCount * m_nDataSize;
		}
		return true;
	}
	else if ( filemode == KDFM_ASCII )
	{
		int nCount = GetDataCount();
		std::string token;
		if( !isNextOBrace( stream ) )
			return false;
		if ( m_idClassType == CLASSTYPEID_ARRAY_STRING )
		{
			for ( int i=0 ; i < nCount ; ++i )
			{
				if ( !getNextString( stream, token ) ) return false;
				m_vecStringArray[i] = token;
				m_vecStringArray[i] += '\0';
			}			
		}
		else
		{
			WORD	wTemp;
			DWORD	dwTemp;
			float	fTemp;
			char	cTemp;
			unsigned char	ucTemp;

			char *ptr = m_pArrayData;
			size_t data_size = GetDataCount() * m_nDataSize;

			for ( int i=0 ; i < nCount ; ++i )
			{
				if ( !getNextToken( stream, token ) ) return false;
				switch ( m_idClassType ) {
				case CLASSTYPEID_ARRAY_WORD:
					wTemp = (unsigned short)atoi( token.c_str() );
					s_memcpy( ptr, data_size, &wTemp, m_nDataSize );
					break;
				case CLASSTYPEID_ARRAY_DWORD:
					dwTemp = atol( token.c_str() );
					s_memcpy( ptr, data_size, &dwTemp, m_nDataSize );
					break;
				case CLASSTYPEID_ARRAY_FLOAT:
					fTemp = (float)atof( token.c_str() );
					s_memcpy( ptr, data_size, &fTemp, m_nDataSize );
					break;
				case CLASSTYPEID_ARRAY_CHAR:
					cTemp = token.at(0);
					s_memcpy( ptr, data_size, &cTemp, m_nDataSize );
					break;
				case CLASSTYPEID_ARRAY_UCHAR:
					ucTemp = token.at(0);
					s_memcpy( ptr, data_size, &ucTemp, m_nDataSize );
					break;
				default:
					return false;
				}
				ptr += m_nDataSize;
				data_size -= m_nDataSize;
			}
		}
		if( !isNextCBrace( stream ) )
			return false;
		return true;
	}
	return false;
}

bool KSimpleDataArrayObject::LoadFast(KStream& stream)
{
	assert(m_idClassType != CLASSTYPEID_NONE);

	// type + array
	stream.Seek(1 + 1,KStream::seekCur);
	stream.Read( &m_nDataCount, 4 );

	// var count + str length
	stream.Seek(4 + m_strLength, KStream::seekCur);
	RefreshArray();

	return LoadBody(stream);
}

KDataObject* KSimpleDataArrayObject::CloneTypeInfo(KTemplateDataObject* pParent)
{
	KSimpleDataArrayObject* pObject = new KSimpleDataArrayObject(m_idClassType, pParent);
	pObject->m_nDataSize = m_nDataSize;
	pObject->m_strLength = m_strLength;
	return pObject;

}
KTemplateDataArrayObject::~KTemplateDataArrayObject()
{
	Clear();
}

void KTemplateDataArrayObject::Clear()
{
	std::vector<KTemplateDataObject*>::iterator it = m_vectorData.begin();

	for ( ; it != m_vectorData.end() ; ++it )
	{
		delete (*it);
	}
	m_vectorData.clear();
}

void KTemplateDataArrayObject::SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
{
	m_nDataCount = nCount;
	m_varCount = NULL;
	if ( m_nDataCount == -1 )
	{
		m_strVarCount = varCount;
		m_pParent = pParent;
		m_varCount = m_pParent->GetMemberObject( m_strVarCount.c_str() );
	}
	RefreshArray();
}

void KTemplateDataArrayObject::RefreshArray()
{
	Clear();
	KTemplateDataObject *temp;

	m_vectorData.resize( GetDataCount() );

	for ( int i=0 ; i<GetDataCount() ; ++i )
	{
		temp = new KTemplateDataObject( m_uuidTemplateType );
		m_vectorData[i] = temp;
	}
}

void KTemplateDataArrayObject::SetData( KStream &streamData, int data_count, FileMode filemode )
{
	for ( int i=0 ; i<data_count ; ++i )
	{
		m_vectorData[i]->Load( streamData, filemode );
	}

}



void KTemplateDataArrayObject::ChangeGUID(const GUID& uuid)
{
	m_uuidTemplateType = uuid;
}

bool KTemplateDataArrayObject::Save( KStream &stream, FileMode filemode )
{
	return SaveHeader( stream, filemode );
}

bool KTemplateDataArrayObject::Load( KStream &stream, FileMode filemode )
{
	return LoadHeader( stream, filemode );
}

bool KTemplateDataArrayObject::SaveHeader( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		stream.Write( &KTOKEN_ARRAY, 1 );
		stream.Write( &KTOKEN_TEMPLATE, 1 );
		stream.Write( &m_uuidTemplateType, sizeof(m_uuidTemplateType) );
		int count = GetDataCount();
		stream.Write( &count, 4 );
		if ( m_nDataCount == -1 )
		{
			int strCount = static_cast<int>(m_strVarCount.size()) + 1;
			stream.Write( &strCount, 4 );
			stream.Write( m_strVarCount.c_str(), strCount );
		}
		else
		{
			int strCount = 0;
			stream.Write( &strCount, 4 );
		}

		for ( int i=0 ; i<count ; ++i )
		{
			KTemplateDataObject *temp = m_vectorData[i];
			if ( temp->Save( stream, filemode ) == false ) 
				return false;
		}
		return true;
	}
	else if ( filemode == KDFM_ASCII )
	{
		char buff[512];
		s_sprintf(buff, _countof( buff ), "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\n",m_uuidTemplateType.Data1,m_uuidTemplateType.Data2,m_uuidTemplateType.Data3,m_uuidTemplateType.Data4[0],m_uuidTemplateType.Data4[1],m_uuidTemplateType.Data4[2],m_uuidTemplateType.Data4[3],m_uuidTemplateType.Data4[4],m_uuidTemplateType.Data4[5],m_uuidTemplateType.Data4[6],m_uuidTemplateType.Data4[7]);
		stream.Write( buff , strlen(buff) );

		int count = GetDataCount();

		PrintTab(stream,filemode);
		s_sprintf(buff, _countof( buff ), "{\n",GetTab());
		stream.Write(buff,strlen(buff));
		SetTab(GetTab()+1);
		for ( int i=0 ; i<count ; ++i )
		{
			PrintTab(stream,filemode);
			KTemplateDataObject *temp = m_vectorData[i];
			temp->SetTab(GetTab());
			if ( temp->Save( stream, filemode ) == false ) return false;
			if ( i!=count-1 ) stream.Write(",\n",strlen(",\n"));
		}
		SetTab(GetTab()-1);
		stream.Write("\n",strlen("\n"));
		PrintTab(stream,filemode);
		stream.Write("}",strlen("}"));

		return true;
	}
	return false;
}

bool KTemplateDataArrayObject::LoadHeader( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		char token;
		Clear();
		stream.Read( &token, 1 );
		if ( token == KTOKEN_ARRAY )
		{
			stream.Read( &token, 1 );
			if ( token == KTOKEN_TEMPLATE )
			{
				stream.Read( &m_uuidTemplateType, sizeof(m_uuidTemplateType) );
				stream.Read( &m_nDataCount, 4 );

				stream.Read( &m_strLength, 4 );
				stream.Seek(m_strLength, KStream::seekCur);

				return LoadBody(stream);
			}
		}
	}
	else if ( filemode == KDFM_ASCII )
	{
		char cTemp[256];
		std::string token;
		int count;

		if( !isNextOBrace( stream ) )
			return false;

		// Read Template UID
		if ( !getNextToken( stream, token ) )
			return false;

		// Template 이름으로 알아낸 UID와 파일에서 직접 읽은 UID가 같은지 확인
		s_sprintf(cTemp, _countof( cTemp ), "%08X-%04hX-%04hX-%04X-%04X%04X%04X",
			m_uuidTemplateType.Data1, m_uuidTemplateType.Data2, m_uuidTemplateType.Data3, 
			m_uuidTemplateType.Data4[0]*256 + m_uuidTemplateType.Data4[1], m_uuidTemplateType.Data4[2]*256 + m_uuidTemplateType.Data4[3], 
			m_uuidTemplateType.Data4[4]*256 + m_uuidTemplateType.Data4[5], m_uuidTemplateType.Data4[6]*256 + m_uuidTemplateType.Data4[7]);
		if ( strcmp(token.c_str(), cTemp) != 0 )
			return false;

		if( !isNextCBrace( stream ) )
			return false;

		Clear();
		count = GetDataCount();

		if( !isNextOBrace( stream ) )
			return false;

		for ( int i=0 ; i<count ; ++i )
		{
			KTemplateDataObject *tempobj = new KTemplateDataObject(m_uuidTemplateType);
			if (tempobj->Load( stream, filemode ) == false)
				return false;
			m_vectorData.push_back( tempobj );
		}

		if( !isNextCBrace( stream ) )
			return false;
		return true;
	}
	return false;
}

bool KTemplateDataArrayObject::LoadBody( KStream &stream, FileMode filemode )
{
	//for ( int i=0 ; i<m_nDataCount ; ++i )
	//{
	//	KTemplateDataObject *tempobj = new KTemplateDataObject(m_uuidTemplateType);
	//	tempobj->Load( stream, filemode );
	//	m_vectorData.push_back( tempobj );
	//}
	//return true;

	// 조금이나마 속도 향상을 위해서


	if(m_nDataCount > 0)
	{
		m_vectorData.resize( m_nDataCount ); 
		m_vectorData[0] = new KTemplateDataObject(stream, filemode);

		for ( int i = 1 ; i < m_nDataCount ; ++i )
		{
			m_vectorData[i] = static_cast<KTemplateDataObject*>( m_vectorData[0]->CloneTypeInfo(NULL) );
			m_vectorData[i]->LoadFast(stream);
		}
	}
	return true;
}

bool KTemplateDataArrayObject::LoadFast(KStream& stream)
{
	// TYPE + TYPE + GUID
	const long OFFSET = 2 + sizeof(GUID);
	stream.Seek(OFFSET,KStream::seekCur);
	stream.Read( &m_nDataCount, 4 );

	// strCount + strVar 길이 만큼 스킵
	stream.Seek(4 + m_strLength, KStream::seekCur);
	return LoadBody(stream);
}
KDataObject* KTemplateDataArrayObject::CloneTypeInfo(KTemplateDataObject* pParent)
{
	KTemplateDataArrayObject *pObject = new KTemplateDataArrayObject(pParent);
	pObject->m_uuidTemplateType = m_uuidTemplateType;
	pObject->m_strLength = m_strLength;

	return pObject;
}

KTemplateDataObject::~KTemplateDataObject()
{
	clearMembers();
}

void KTemplateDataObject::clearMembers()
{
	std::vector<KDataObject*>::iterator it = m_vectorMemberObjects.begin();
	for ( ; it != m_vectorMemberObjects.end() ; ++it )
	{
		delete *it;
	}
	m_vectorMemberObjects.clear();
	//m_hashByName.clear();
}

void KTemplateDataObject::ChangeTemplateInfo(KTemplateInfo* pNewInfo)
{
	m_memberInfo = pNewInfo;
	m_uuidTemplate = m_memberInfo->GetTemplateGUID();
}

int KTemplateDataObject::GetExtraMemberCount()
{
	return static_cast<int>(m_vectorMemberObjects.size()) - m_memberInfo->GetMemberCount();
}

KTemplateDataObject* KTemplateDataObject::GetExtraMember( int index )
{
	return reinterpret_cast<KTemplateDataObject*>(m_vectorMemberObjects[m_memberInfo->GetMemberCount() + index]);
}

const char *KTemplateDataObject::GetExtraMemberName( int index )
{
	KTemplateDataObject *obj = reinterpret_cast<KTemplateDataObject*>(m_vectorMemberObjects[m_memberInfo->GetMemberCount() + index]);
	return obj->GetTemplateName();
}

// enumeration
KSimpleDataObject* GetSimpleDataMemberObject( const char *name );
KSimpleDataArrayObject* GetSimpleDataArrayMemberObject( const char *name );
KTemplateDataObject* GetTemplateDataMemberObject( const char *name );
KTemplateDataArrayObject* GetTemplateDataArrayMemberObject( const char *name );



void KTemplateDataObject::ExtendMemberVector(const char* name, size_t count)
{
	size_t curSize = GetMemberCount();
	
	size_t newSize = curSize + count;
	std::vector<KDataObject*> newVector(newSize);
	newVector.resize(newSize);

	size_t index = m_memberInfo->GetMemberIndex(name);

	for(size_t i = 0; i < index; ++i)
	{
		newVector.at( i ) = m_vectorMemberObjects.at(i);
	}

	for(size_t i = index + count; i < newSize; ++i)
	{
		newVector.at(i) = m_vectorMemberObjects.at( i - count );
	}
	
	m_vectorMemberObjects = newVector;
}
void KTemplateDataObject::SetMemberObject(size_t index, KDataObject* pMemberObject)
{
	m_vectorMemberObjects.at(index) = pMemberObject;

}
void KTemplateDataObject::AddMemberObject(KDataObject* pNewObj)
{
	m_vectorMemberObjects.push_back(pNewObj);
}

bool KTemplateDataObject::Save( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		stream.Write( &KTOKEN_TEMPLATE, 1 );
		stream.Write( &m_uuidTemplate, sizeof(m_uuidTemplate) );

		int nMembers = static_cast<int>(m_vectorMemberObjects.size());
		stream.Write( &nMembers, sizeof(nMembers) );

		std::vector<KDataObject*>::iterator it = m_vectorMemberObjects.begin();
		for( ; it != m_vectorMemberObjects.end() ; ++it )
		{
			if ( (*it)->Save( stream, filemode ) == false ) 
				return false;
		}
		return true;
	}
	else if ( filemode == KDFM_ASCII )
	{
		//Template 이름, UUID AsciiType으로 저장
		char buff[512];
		s_sprintf(buff, _countof( buff ), GetTemplateName());
		stream.Write( buff , strlen(buff) );
		stream.Write( "\n", strlen("\n"));
		s_sprintf(buff, _countof( buff ), "{%08X-%04hX-%04hX-%04X-%04X%04X%04X}\n",
			m_uuidTemplate.Data1, m_uuidTemplate.Data2, m_uuidTemplate.Data3, 
			m_uuidTemplate.Data4[0]*256 + m_uuidTemplate.Data4[1], m_uuidTemplate.Data4[2]*256 + m_uuidTemplate.Data4[3], 
			m_uuidTemplate.Data4[4]*256 + m_uuidTemplate.Data4[5], m_uuidTemplate.Data4[6]*256 + m_uuidTemplate.Data4[7]);
		PrintTab(stream,filemode);
		stream.Write( buff , strlen(buff) );
		PrintTab(stream,filemode);
		stream.Write( "{\n", strlen("{\n") );

		//Member objects를 하나씩 저장
		std::vector<KDataObject*>::iterator it = m_vectorMemberObjects.begin();
		this->SetTab(this->GetTab()+1);
		for( ; it != m_vectorMemberObjects.end() ; )
		{
			PrintTab(stream,filemode);
			(*it)->SetTab(this->GetTab());
			if ( (*it)->Save( stream, filemode ) == false ) return false;
			++it;
			if( it!= m_vectorMemberObjects.end() ) stream.Write(",\n",strlen(",\n"));
		}
		this->SetTab(this->GetTab()-1);
		stream.Write("\n",strlen("\n"));
		PrintTab(stream,filemode);
		stream.Write("}",strlen("}"));
		return true;
	}
	return false;
}

bool KTemplateDataObject::Load( KStream &stream, FileMode filemode )
{
	if ( filemode == KDFM_BINARY )
	{
		char token;
		stream.Read( &token, 1 );
		if ( token == KTOKEN_TEMPLATE )
		{
			KDataObject *dataobj = NULL;
			stream.Read( &m_uuidTemplate, sizeof(m_uuidTemplate) );
			clearMembers();
			getMemberData(false);
			int nMembers;
			stream.Read( &nMembers, sizeof(nMembers) );

			//assert( nMembers == GetMemberCount() );

			for ( int i=0 ; i< nMembers ; ++i )
			{
				stream.Read( &token, 1 );
				if ( token == KTOKEN_ARRAY )
				{
					stream.Read( &token, 1 );
					stream.Seek( -2, KStream::seekCur );
					switch( token )
					{
					case KTOKEN_TEMPLATE:
						dataobj = new KTemplateDataArrayObject( stream, filemode, this );
						break;
					case KTOKEN_WORD:
					case KTOKEN_DWORD:
					case KTOKEN_FLOAT:
					case KTOKEN_CHAR:
					case KTOKEN_UCHAR:
					case KTOKEN_STRING:
						dataobj = new KSimpleDataArrayObject( CLASSTYPEID_NONE, this );
						dataobj->Load( stream, filemode );
						break;
					}
				}
				else
				{
					stream.Seek( -1, KStream::seekCur );
					switch( token )
					{
					case KTOKEN_WORD:
					case KTOKEN_DWORD:
					case KTOKEN_FLOAT:
					case KTOKEN_CHAR:
					case KTOKEN_UCHAR:
					case KTOKEN_STRING:
						dataobj = new KSimpleDataObject( CLASSTYPEID_NONE );
						dataobj->Load( stream, filemode );
						break;
					case KTOKEN_TEMPLATE:
						dataobj = new KTemplateDataObject( stream, filemode );
						break;
					}
				}
				m_vectorMemberObjects.push_back( dataobj );
				const char *name;
				if ( i < GetMemberCount() )
				{
					assert( dataobj && dataobj->GetClassType() == m_memberInfo->GetMemberClassID(i) );
					name = m_memberInfo->GetMemberName(i);
				}
				else
				{
					KTemplateDataObject *obj = reinterpret_cast<KTemplateDataObject*>(dataobj);
					name = obj->GetTemplateName();
				}
				//m_hashByName.add( name, dataobj );
			}
		}
	}
	else if ( filemode == KDFM_ASCII )
	{
		std::string token;
		char cTemp[256];
		KTemplateInfo *pTemplateInfo;
		GUID uuidTemp;
		int nCount;

		// Read Template Name
		if ( !getNextToken( stream, token ) )
			return false;

		if  ( ( pTemplateInfo = KFiler::GetTemplateInfo( token.c_str() ) ) == NULL )
			return false;

		if( !isNextOBrace( stream ) )
			return false;

		if( !getNextToken( stream, token) )
			return false;

		// Template 이름으로 알아낸 UID와 파일에서 직접 읽은 UID가 같은지 확인
		uuidTemp = pTemplateInfo->GetTemplateGUID();
		s_sprintf(cTemp, _countof( cTemp ), "%08X-%04hX-%04hX-%04X-%04X%04X%04X",
			uuidTemp.Data1, uuidTemp.Data2, uuidTemp.Data3, 
			uuidTemp.Data4[0]*256 + uuidTemp.Data4[1], uuidTemp.Data4[2]*256 + uuidTemp.Data4[3], 
			uuidTemp.Data4[4]*256 + uuidTemp.Data4[5], uuidTemp.Data4[6]*256 + uuidTemp.Data4[7]);
		if ( strcmp(token.c_str(), cTemp) != 0 )
			return false;

		if( !isNextCBrace( stream ) )
			return false;

		m_uuidTemplate = uuidTemp;
		clearMembers();
		getMemberData(true);

		nCount = m_memberInfo->GetMemberCount();

		if( !isNextOBrace( stream ) )
			return false;

		for (int i = 0; i < nCount ; i++ )
		{
			KDataObject *dataobj = m_vectorMemberObjects[i];
			if  (dataobj->Load( stream, filemode ) == false)
				return false;
		}

		if( !isNextCBrace( stream ) )
			return false;

		return true;
	}
	return false;
}

bool KTemplateDataObject::LoadFast(KStream& stream)
{
	// type + GUID + member count
	const long OFFSET = 1 + sizeof(GUID) + sizeof(int);
	stream.Seek(OFFSET, KStream::seekCur );

	for(size_t i = 0; i < m_vectorMemberObjects.size(); ++i)
	{
		// 이미 type 정보들이 복사 되어 있으므로..
		m_vectorMemberObjects[i]->LoadFast(stream);
	}
	return true;
}

KDataObject* KTemplateDataObject::CloneTypeInfo(KTemplateDataObject* pParent)
{
	KTemplateDataObject* pObject = new KTemplateDataObject;

	pObject->m_uuidTemplate = m_uuidTemplate;
	pObject->m_memberInfo = m_memberInfo;

	size_t count = m_vectorMemberObjects.size();
	pObject->m_vectorMemberObjects.resize(count);

	for(size_t i = 0; i <count; ++i)
	{
		pObject->m_vectorMemberObjects[i] = m_vectorMemberObjects[i]->CloneTypeInfo(pObject);
	}
	return pObject;		
}


void KTemplateDataObject::getMemberData( bool bCreateMember )
{
	m_memberInfo = KFiler::GetTemplateInfo( m_uuidTemplate );
	m_uuidTemplate = m_memberInfo->GetTemplateGUID();

	if ( bCreateMember == true )
	{
		m_vectorMemberObjects.clear();
		//m_hashByName.clear();
		int count = m_memberInfo->GetMemberCount();

		m_vectorMemberObjects.resize( count );
		for ( int i=0 ; i<count ; ++i )
		{
			KDataObject *obj = m_memberInfo->CreateMember( i, this );
			m_vectorMemberObjects[i] = obj;
			//m_hashByName.add( m_memberInfo->GetMemberName(i), obj );
		}
	}
}

//ascii모드에서 tab 출력위해
void KTemplateDataArrayObject::PrintTab( KStream &stream,FileMode filemode )
{
	if(filemode==KDFM_ASCII)
	{
		int t_size=GetTab();
		for(int i=0;i<t_size;i++)
		{
			stream.Write("\t",strlen("\t"));
		}
	}
}

void KTemplateDataObject::PrintTab( KStream &stream,FileMode filemode )
{
	if(filemode==KDFM_ASCII)
	{
		int t_size=GetTab();
		for(int i=0;i<t_size;i++)
		{
			stream.Write("\t",strlen("\t"));
		}
	}
}


