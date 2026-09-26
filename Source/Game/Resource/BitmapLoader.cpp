
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>

#include "ContentLoader.h"
#include "BitmapBox.h"

struct NBA_HEADER
{
	char szSign[16];
	DWORD dwVersion;
	float fAttrLength;
	int nWidth;
	int nHeight;
};

static void LoadBitmap()
{
	// 비트맵 정보 더이상 안쓴다.

	//NBA_HEADER header;

	//std::string strFileName = ResourceManager::GetFullPathname( ENV().GetString( "game.terrain_bitmap_file", "TerrainAttribute.nba" ).c_str() );

	//if( strFileName.empty() ) 
	//{
	//	return;
	//	throw XException( "BitmapLoader() : 맵 비트맵 속성 파일이 없습니다." );
	//}

	//FILE *fp = fopen( strFileName.c_str(), "rb" );

	//if( !fp ) throw XException( "BitmapLoader() : 맵 비트맵 속성 파일 열기에 실패했습니다." );

	//fread( &header, sizeof(header), 1, fp );

	//g_pTerrainBitmap = new BitmapBox( header.nWidth, header.nHeight );

	////int nSize = ( header.nWidth / 8 ) + ( header.nWidth % 8 ? 1 : 0 );

	//for( int i = 0; i < header.nHeight; ++i )
	//{
	//	if( fread( g_pTerrainBitmap->m_pData[i], header.nWidth, 1, fp ) != 1 )
	//	{
	//		fclose( fp );
	//		throw XException( "BitmapLoader() : 맵 비트맵 속성 파일 읽기에 실패했습니다." );
	//	}
	//}

	//g_pTerrainBitmap->m_fAttrLength = header.fAttrLength;

	//fclose( fp );
}

bool BitmapLoader::onProcess( int nThreadNum )
{
	try
	{
		LoadBitmap();
	}
	catch( ... )
	{
		throw ;
	}

	return true;
}

void BitmapLoader::onEnd( bool bIsCancel )
{
}

