
#include "../../include/kfile/SPatchData.h"
#include "../../include/kfile/SFileRoot.h"

#include <assert.h>

SFileRoot::SFileRoot(int nOpenMode)
	: m_strFileName( "Unnamed.sxf" )
{
	m_nExtType = 0;
	m_nCompressMode = 0;
	m_hFileHandle	= NULL;
	m_nOpenMode		= nOpenMode;
	Init();
}

SFileRoot::SFileRoot( const char * pFileName, int nOpenMode )
	: m_strFileName( pFileName )
	, m_strOnlyFileName( pFileName )
{
	m_nExtType = 0;
	m_nCompressMode = 0;
	m_hFileHandle	= NULL;
	m_nOpenMode		= nOpenMode;
	
	std::string::size_type pos = m_strOnlyFileName.rfind( '\\' );
	if( pos != std::string::npos )
	{
		m_strOnlyDir      = m_strFileName.substr( 0, pos+1 );                      //폴더
		m_strOnlyFileName = ( pFileName + pos + 1 );							// by Testors
	}

	Init();
}

const char * SFileRoot::GetOnlyDirName()
{
	return m_strOnlyDir.c_str();
}

const char * SFileRoot::GetOnlyFileName()
{
	return m_strOnlyFileName.c_str();
}


SFileRoot::~SFileRoot()
{
	if( m_hFileHandle ) CloseHandle( m_hFileHandle );
}

void SFileRoot::Init()
{
	HANDLE hFileHandle = NULL;
	if( m_nOpenMode == OPEN_SAVE )
		hFileHandle = CreateFile( m_strFileName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	else if( m_nOpenMode == OPEN_READ )
		hFileHandle = CreateFile( m_strFileName.c_str(), GENERIC_READ , FILE_SHARE_READ , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFileHandle == INVALID_HANDLE_VALUE )
	{
		return;
	}

	m_hFileHandle = hFileHandle;

	//m_strFileExt = m_strFileName;
	//std::string::size_type pos;
	//std::string::size_type npos = -1;
	//pos = m_strFileExt.rfind('.');
	//if( pos != npos )
	//{
	//	m_strFileExt.erase( 0, pos+1 );
	//	m_nExtType = SCOMPRESS_HEADER::TYPE_DAT; //기타

	//	if( _stricmp( m_strFileExt.c_str(), "nx3" ) == 0 )
	//		m_nExtType = SCOMPRESS_HEADER::TYPE_NX3;
	//	if( _stricmp( m_strFileExt.c_str(), "naf" ) == 0 )
	//		m_nExtType = SCOMPRESS_HEADER::TYPE_NAF;
	//	if( _stricmp( m_strFileExt.c_str(), "cob" ) == 0 )
	//		m_nExtType = SCOMPRESS_HEADER::TYPE_COB;

	//	if( _stricmp( m_strFileExt.c_str(), "dds" ) == 0 ||
	//		_stricmp( m_strFileExt.c_str(), "tga" ) == 0 ||
	//		_stricmp( m_strFileExt.c_str(), "jpg" ) == 0 ||
	//		_stricmp( m_strFileExt.c_str(), "bmp" ) == 0 )
	//		m_nExtType = SCOMPRESS_HEADER::TYPE_TEX;
	//}
}

void SFileRoot::Close()
{
	if( m_hFileHandle ) 
	{
		CloseHandle( m_hFileHandle );	
		m_hFileHandle = NULL;
	}
}

int  SFileRoot::GetFSize()
{
	if( IsVaild() )
	{
		DWORD dwFileSizeLow, dwFileSizeHigh; //화일 크기

		//화일 크기를 알아 낸다.
		dwFileSizeLow = GetFileSize( m_hFileHandle, &dwFileSizeHigh );
		if (dwFileSizeLow == INVALID_FILE_SIZE) 
		{ 
			return -2;
		}

		if (dwFileSizeLow == INVALID_FILE_SIZE && GetLastError() != NO_ERROR )
		{ 
			return -3;
		} 
		return dwFileSizeLow;
	}
	else
		return -1;
}

int SFileRoot::Write( void * pData, DWORD dwDataSize )
{
	if( !IsVaild() || m_nOpenMode != OPEN_SAVE ) return 0;

	DWORD dwWriteSize = 0;

	if( WriteFile( m_hFileHandle, pData, dwDataSize, &dwWriteSize, NULL ) == FALSE )
	{
		dwWriteSize = 0;
	}

	return dwWriteSize;
}

int SFileRoot::Read ( void * pData, DWORD dwDataSize )
{
	if( !IsVaild() || m_nOpenMode != OPEN_READ ) return 0;

	DWORD dwReadSize = 0;

	if( ReadFile( m_hFileHandle, pData, dwDataSize, &dwReadSize, NULL ) == FALSE )
	{
		dwReadSize = 0;
	}

	return dwReadSize;
}

void SFileRoot::Save( int nMode )
{
	switch(nMode)
	{
	case MODE_BIN : SaveBin();		break;
	case MODE_TEXT: SaveText();		break;
	default       : break;
	}
}

void SFileRoot::Load( int nMode )
{
	switch(nMode)
	{
	case MODE_BIN : LoadBin();		break;
	case MODE_TEXT: LoadText();		break;
	default       : break;
	}
}

void SFileRoot::SaveBin()
{

}

void SFileRoot::SaveText()
{

}

void SFileRoot::LoadBin()
{

}

void SFileRoot::LoadText()
{

}


