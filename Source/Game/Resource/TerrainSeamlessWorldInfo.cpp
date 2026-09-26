#include <windows.h>
#include "MapUtil.h"
#include "MapDefine.h"
#include "TerrainSeamlessWorldInfo.h"


namespace
{
	const char* c_szMapFileExt					= ".nfm";
	const char* c_szScriptFileExt				= ".nfs";
	const char* c_szLocationFileExt				= ".nfc";
	const char* c_szAttributePolygonFileExt		= ".nfa";
	const char* c_szMinimapFileExt				= ".bmp";
	const char* c_szLQWaterFileExt				= ".nfw";
	const char* c_szPropFileExt					= ".qpf";
	const char* c_szEventAreaPolygonFileExt		= ".nfe";
};


std::string CTerrainSeamlessWorldInfo::GetMapFileName( int nMapPosX, int nMapPosY ) const
	{ return GetFileNameWithExt( nMapPosX, nMapPosY, c_szMapFileExt ); }

std::string CTerrainSeamlessWorldInfo::GetScriptFileName( int nMapPosX, int nMapPosY ) const
	{ return GetFileNameWithExt( nMapPosX, nMapPosY, c_szScriptFileExt ); }

std::string CTerrainSeamlessWorldInfo::GetLocationFileName( int nMapPosX, int nMapPosY ) const
	{ return GetFileNameWithExt( nMapPosX, nMapPosY, c_szLocationFileExt ); }

std::string CTerrainSeamlessWorldInfo::GetAttributePolygonFileName( int nMapPosX, int nMapPosY ) const
	{ return GetFileNameWithExt( nMapPosX, nMapPosY, c_szAttributePolygonFileExt ); }

std::string CTerrainSeamlessWorldInfo::GetFieldPropFileName( int nMapPosX, int nMapPosY ) const
	{ return GetFileNameWithExt( nMapPosX, nMapPosY, c_szPropFileExt ); }

std::string CTerrainSeamlessWorldInfo::GetMinimapImageFileName( int nMapPosX, int nMapPosY ) const
	{ return GetFileNameWithExt( nMapPosX, nMapPosY, c_szMinimapFileExt ); }

std::string CTerrainSeamlessWorldInfo::GetLQWaterFileName( int nMapPosX, int nMapPosY ) const
	{ return GetFileNameWithExt( nMapPosX, nMapPosY, c_szLQWaterFileExt ); }

std::string CTerrainSeamlessWorldInfo::GetEventAreaFileName( int nMapPosX, int nMapPosY ) const
	{ return GetFileNameWithExt( nMapPosX, nMapPosY, c_szEventAreaPolygonFileExt ); }

int CTerrainSeamlessWorldInfo::GetWorldID( int nMapPosX, int nMapPosY ) const
{ 
	if( 0 <= nMapPosX && nMapPosX < m_sizMapCount.cx && 0 <= nMapPosY && nMapPosY < m_sizMapCount.cy )
	{
		FILENAMEMAP::const_iterator it = m_FileNameMap.find( (nMapPosY * m_sizMapCount.cx) + nMapPosX );
		if( it != m_FileNameMap.end() ) 
			return it->second.m_nWorldID;
	}
	return -1;
}


CTerrainSeamlessWorldInfo::CTerrainSeamlessWorldInfo()
	: m_nTileCountPerSegment( 0 )
	, m_nSegmentCountPerMap( 0 )
	, m_fTileLength( 0.f )
	, m_sizMapCount( KSize( 0, 0 ) )
	, m_fFOV( 0.f ) 
	, m_nMapLayer( 0 ) 
{
}

CTerrainSeamlessWorldInfo::~CTerrainSeamlessWorldInfo()
{
	Release();
}

bool CTerrainSeamlessWorldInfo::Initialize( const char* szSeamlessWorldInfoFileName, bool bMapFileCheck/* = true*/ )
{
	Release();

	// 파일로부터 text들을 읽어낸다.
	STRING_VECTOR TextLines;
	{
		KStream* pStream = GetResourceStream( szSeamlessWorldInfoFileName );
		if( NULL == pStream ) return false;

		int nTextLength = int( pStream->GetLength() );
		char* pEntireText = new char[ nTextLength + 1 ];

		pStream->Read( pEntireText, sizeof(char) * nTextLength );
		pEntireText[ nTextLength ] = '\0';
		delete pStream;

		CStringUtil::GetTextLinesFromString( pEntireText, TextLines );
		delete [] pEntireText;
	}

	const char* c_szTileLength          = "TILE_LENGTH=";
	const char* c_szTileCountPerSegment = "TILECOUNT_PER_SEGMENT=";
	const char* c_szSegmentCountPerMap  = "SEGMENTCOUNT_PER_MAP=";
	const char* c_szMapSize             = "MAPSIZE=";
	const char* c_szMapFIle             = "MAPFILE=";
	const char* c_szFOV                 = "SETFOV=";
	const char* c_szMapLayer			= "MAPLAYER=";

	float fTileLength = 0.f;
	int nTileCountPerSegment = 0;
	int nSegmentCountPerMap = 0;
	int nMapCountX = 0;
	int nMapCountY = 0;
	float fFOV = 0.f;
	int nMapLayer = 0;

	for( size_t n = 0; n < TextLines.size(); ++n )
	{
		std::string strLine = TextLines[n];

		if( ';' == strLine[0] ) continue;
		std::string strContent;

		// tile length
		if( CStringUtil::GetContentString( strLine, c_szTileLength, strContent ) )
			fTileLength = float( ::atof( strContent.c_str() ) );

		// tile count per segment
		else if( CStringUtil::GetContentString( strLine, c_szTileCountPerSegment, strContent ) )
			nTileCountPerSegment = ::atoi( strContent.c_str() );

		// segment count per map
		else if( CStringUtil::GetContentString( strLine, c_szSegmentCountPerMap, strContent ) )
			nSegmentCountPerMap = ::atoi( strContent.c_str() );

		else if( CStringUtil::GetContentString( strLine, c_szFOV, strContent ) )
			fFOV = float( ::atof( strContent.c_str() ) );
		else if( CStringUtil::GetContentString( strLine, c_szMapLayer, strContent ) )
			nMapLayer = ::atoi( strContent.c_str() );

		// map size
		else if( CStringUtil::GetContentString( strLine, c_szMapSize, strContent ) )
		{
			if( 0 != nMapCountX || 0 != nMapCountY ) return false; // map size항목이 2번 이상 나온 경우

			// ',' 문자를 찾는다.
			std::string::size_type CommaPos = strContent.find( ',' );
			if( CommaPos == std::string::npos ) return false;

			// ',' 앞의 가로 갯수
			std::string strMapCountX;
			strMapCountX.assign( strContent, 0, CommaPos );

			// ',' 뒤의 세로 갯수
			std::string strMapCountY;
			strMapCountY.assign( strContent, CommaPos + 1, strContent.size() - (CommaPos + 1) );

			if( strMapCountX.empty() || strMapCountY.empty() ) return false;

			nMapCountX = ::atoi( strMapCountX.c_str() );
			nMapCountY = ::atoi( strMapCountY.c_str() );
			if( nMapCountX <= 0 || nMapCountY <= 0 ) return false;
		}

		// map file
		else if( CStringUtil::GetContentString( strLine, c_szMapFIle, strContent ) )
		{
			if( 0 == nMapCountX && 0 == nMapCountY ) return false; // map size항목이 아직 안나온 경우

			// ',' 문자를 2번 찾는다.
			std::string::size_type CommaPos1 = strContent.find( ',' );
			if( CommaPos1 == std::string::npos ) return false;

			std::string::size_type CommaPos2 = strContent.find( ',', (CommaPos1 + 1) );
			if( CommaPos2 == std::string::npos ) return false;

			std::string::size_type CommaPos3 = strContent.find( ',', (CommaPos2 + 1) );
			if( CommaPos3 == std::string::npos ) return false;

			std::string::size_type CommaPos4 = strContent.find( ',', (CommaPos3 + 1) );
			if( CommaPos4 == std::string::npos ) return false;

			// 첫번째 ',' 앞의 맵 x위치
			std::string strMapX;
			strMapX.assign( strContent, 0, CommaPos1 );

			// 두번째 ',' 앞의 맵 y위치
			std::string strMapY;
			strMapY.assign( strContent, (CommaPos1 + 1), CommaPos2 - (CommaPos1 + 1) );

			// 세번째 ',' 앞의 맵 layer위치
			std::string strMapLayer;
			strMapLayer.assign( strContent, (CommaPos2 + 1), CommaPos3 - (CommaPos2 + 1) );

			// 세번째 ',' 뒤의 맵 파일명
			std::string strMapFileName;
			strMapFileName.assign( strContent, (CommaPos3 + 1), CommaPos4 - (CommaPos3 + 1) );

			std::string strWorldMap;
			strWorldMap.assign( strContent, (CommaPos4 + 1 ), strContent.size() - (CommaPos4 + 1) );

			if( strMapX.empty() || strMapY.empty() || strMapLayer.empty() || strMapFileName.empty() ) return false;

			int nMapX = ::atoi( strMapX.c_str() );
			int nMapY = ::atoi( strMapY.c_str() );
			int nMapLay = ::atoi( strMapLayer.c_str() );
			if( nMapX < 0 || nMapX >= nMapCountX ) return false;
			if( nMapY < 0 || nMapY >= nMapCountY ) return false;
			if( nMapLay < 0 )					   return false;

			int nWorldMapID = ::atoi( strWorldMap.c_str() );

			int nMapIndex	= (nMapY * nMapCountX) + nMapX + nMapLay;

			// 같은 위치에 map file항목이 2번 이상 나온 경우
			if( m_FileNameMap.find( nMapIndex ) != m_FileNameMap.end() ) return false;

			FILENAME_MAPINFO	mapinfo;
			mapinfo.m_strMapFileName	=	strMapFileName;
			mapinfo.m_nWorldID			=	nWorldMapID;
			m_FileNameMap.insert( std::make_pair( nMapIndex, mapinfo ) );
		}
	}

	// 파라미터가 정상인지 확인
	if( fTileLength <= 0.f || nTileCountPerSegment <= 0 || nSegmentCountPerMap <= 0 ) return false;

	// 한개 이상의 맵이 로드 되었는지 확인
	if( m_FileNameMap.empty() ) return false;


	m_nTileCountPerSegment = nTileCountPerSegment;
	m_nSegmentCountPerMap  = nSegmentCountPerMap;
	m_fTileLength          = fTileLength;
	m_nMapLayer			   = nMapLayer;

	m_sizMapCount.cx = nMapCountX;
	m_sizMapCount.cy = nMapCountY;

	if( 0.f < fFOV ) m_fFOV = fFOV;

	// 모든 맵 파일이 정상인지 확인
	if( bMapFileCheck )
	{
		for( int nY( 0 ); nY < nMapCountY; ++nY )
		{
			for( int nX( 0 ); nX < nMapCountX; ++nX )
			{
				std::string strMapFileName = GetMapFileName( nX, nY );
				if( !strMapFileName.empty() )
				{
					// 맵 파일 열기
					KStream* pStream = GetResourceStream( strMapFileName.c_str() );
					if( NULL == pStream ) return false;

					// 헤더 읽기
					NFM_LATESTHEADER header;
					if( sizeof(header) != pStream->Read( &header, sizeof(header) ) )
					{
						delete pStream;
						return false;
					}
					delete pStream;

					// 식별 문자열 검사
					std::string strSign = header.szSign;
					if( strSign.compare( NFMFILE_SIGN ) != 0 ) return false;

					// 버전 체크
					if( header.dwVersion != c_dwNFMCurrentVer ) return false;

					// 파라미터가 동일한지 체크
					if( header.nTileCountPerSegment != nTileCountPerSegment ||
						header.nSegmentCountPerMap  != nSegmentCountPerMap ||
						header.fTileLength          != fTileLength ) return false;
				}
			}
		}
	}
	return true;
}

void CTerrainSeamlessWorldInfo::Release()
{
	m_nTileCountPerSegment = 0;
	m_nSegmentCountPerMap = 0;
	m_fTileLength = 0.f;
	m_sizMapCount = KSize( 0, 0 );
	m_FileNameMap.clear();
	m_nMapLayer = 0;
}
