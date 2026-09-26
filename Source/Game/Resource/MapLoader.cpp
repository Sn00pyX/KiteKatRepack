
#include <string>

#include <mmo/ArType.h>
#include <toolkit/XEnv.h>
#include <kfile/KStream.h>
#include <toolkit/XConsole.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/khash.h>
#include <logging/FileLog.h>
#include <toolkit/XFileUtil.h>

#include "ContentLoader.h"
#include "TerrainSeamlessWorldInfo.h"
#include "ScriptDefine.h"
#include "TerrainPropInfo.h"
#include "LuaVM.h"
#include "FieldPropManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

int g_currentLocationId;

float g_fMapLength = 0.0f;

const int c_nAttrCountPerTile = 8; // 클라이언트랑 맞춰야 함. 클라이언트 바뀌면 서버도 꼭 바꿀껏. 좌표 삐꾸나면 가장 먼저 이 값 확인할 것.

struct ScriptRegion
{
	ScriptRegion( int l, int t, int r, int b, const char *szName ) : left(l), top(t), right(r), bottom(b), strName(szName)
	{
	}

	AR_UNIT			left, top, right, bottom;
	std::string		strName;
};

struct ScriptTag
{	
	int				nTrigger;
	std::string		strFunction;
};

struct ScriptRegionInfo
{
	int								nRegionIndex;
	std::vector< ScriptTag >		vInfoList;
};

/*
struct ScriptPropInfo
{
	enum SCRIPT_TYPE
	{
		NPC		= 0,
		PROP,
	};

	int							nPropIndex;
	float						x, y;
	SCRIPT_TYPE					nScriptType;	
	std::vector< ScriptTag >	vInfoList;
};*/

std::vector< ScriptRegion >		g_vRegionList;
std::vector< ScriptRegionInfo >		g_vScriptEvent;
// std::vector< ScriptPropInfo >		g_vScriptProp;

static float					s_fTileSize		= 1;
static size_t					s_nCurrentRegionIdx = 0;

class _myTerrainSeamlessWorldInfo : public CTerrainSeamlessWorldInfo 
{
protected:
	KStream* GetResourceStream( const char* szFileName )
	{
		KStream *pStream = new KFileStream( ResourceManager::GetFullPathname( szFileName ).c_str() );

		if( !pStream->IsValid() )
		{
			throw XException( std::string( "Map read error ! " ) + szFileName );
		}

		return pStream;
	}
};

class _myTerrainPropInfo : public CTerrainPropInfo 
{
protected:
	KStream* GetResourceStream( const char* szFileName )
	{
		KStream *pStream = new KFileStream( ResourceManager::GetFullPathname( szFileName ).c_str() );

		if( !pStream->IsValid() )
		{
			throw XException( std::string( "Map read error ! " ) + szFileName );
		}

		return pStream;
	}
};

static void LoadStringFromStream( KFileStream & stream, void *p, int size )
{
	int nLength;

	stream.Read( &nLength, sizeof(nLength) );

	if( nLength )
	{
		if( nLength > size ) throw XException( std::string( "too big script name ! " ) );
		stream.Read( p, nLength );
	}

	static_cast< char* >(p)[nLength] = 0;
}

static void LoadFunctionInfo( KFileStream & stream, std::vector< ScriptTag > & v )
{
	DWORD		nFunctionCount = 0;
	ScriptTag	info;
	char szBuf[1024];
	
	stream.Read( &nFunctionCount, sizeof(nFunctionCount) );
	for( ; nFunctionCount; --nFunctionCount )
	{
		stream.Read( &info.nTrigger, sizeof(info.nTrigger) );

		LoadStringFromStream( stream, szBuf, sizeof(szBuf) );
	
		info.strFunction = szBuf;
		
		v.push_back( info );
	}
}

static void LoadRegionScriptInfo( KFileStream & stream, std::vector< ScriptRegionInfo > & v )
{
	DWORD	nScriptCount = 0;
	ScriptRegionInfo tag;
	
	stream.Read( &nScriptCount, sizeof(nScriptCount) );
	for( ; nScriptCount; --nScriptCount )
	{	
		stream.Read( &tag.nRegionIndex, sizeof(tag.nRegionIndex) );	

		LoadFunctionInfo( stream, tag.vInfoList );

		for( std::vector< ScriptTag >::iterator it = tag.vInfoList.begin(); it != tag.vInfoList.end(); ++it )
		{
			char	szLeft[30];
			char	szRight[30];
			char	szTop[30];
			char	szBottom[30];
			char	szBox[128];
		
			// { 영역 매크로 작업  #LEFT, #RIGHT, #TOP, #BORROM, #BOX 를 해당 좌표로 변경해준다.
			if( strstr( (*it).strFunction.c_str(), "#" ) )
			{
				s_sprintf( szLeft, _countof( szLeft ), "%d",			(int)g_vRegionList[s_nCurrentRegionIdx+tag.nRegionIndex].left );
				s_sprintf( szTop, _countof( szTop ), "%d",				(int)g_vRegionList[s_nCurrentRegionIdx+tag.nRegionIndex].top );
				s_sprintf( szRight, _countof( szRight ), "%d",			(int)g_vRegionList[s_nCurrentRegionIdx+tag.nRegionIndex].right );
				s_sprintf( szBottom, _countof( szBottom ), "%d",		(int)g_vRegionList[s_nCurrentRegionIdx+tag.nRegionIndex].bottom );

				s_sprintf( szBox, _countof( szBox ), "%d,%d,%d,%d",	(int)g_vRegionList[s_nCurrentRegionIdx+tag.nRegionIndex].left,
												(int)g_vRegionList[s_nCurrentRegionIdx+tag.nRegionIndex].top,
												(int)g_vRegionList[s_nCurrentRegionIdx+tag.nRegionIndex].right,
												(int)g_vRegionList[s_nCurrentRegionIdx+tag.nRegionIndex].bottom );		

				XStringUtil::Replace( (*it).strFunction, "#LEFT", szLeft );
				XStringUtil::Replace( (*it).strFunction, "#RIGHT", szRight );
				XStringUtil::Replace( (*it).strFunction, "#TOP", szTop );
				XStringUtil::Replace( (*it).strFunction, "#BOTTOM", szBottom );
				XStringUtil::Replace( (*it).strFunction, "#BOX", szBox );
				XStringUtil::Replace( (*it).strFunction, "#box", szBox );
			}
		}
		
		v.push_back( tag );

		tag.vInfoList.clear();
	}
}
static void LoadPropScriptInfo( CTerrainPropInfo * pTerrainPropInfo, KFileStream & stream, int x, int y, float fMapLength )
{
	extern KHash< size_t, hashPr_mod_int >	& GetPropScriptHash();	// GameContent.cpp 에 있음.

	DWORD	nScriptCount = 0;

	int size = static_cast<int>( stream.GetLength() );
	
	stream.Read( &nScriptCount, sizeof(nScriptCount) );
	for( ; nScriptCount; --nScriptCount )
	{	
		_PROP_CONTACT_SCRIPT_INFO tag;

		// 프랍 정보 읽음
		WORD model_id = 0;
		if( stream.Read( &tag.prop_id, sizeof(tag.prop_id) ) != sizeof(tag.prop_id) ) assert( 0 );		// 프랍 인덱스
		if( stream.Read( &tag.x, sizeof(tag.x) ) != sizeof(tag.x) ) assert( 0 );		
		if( stream.Read( &tag.y, sizeof(tag.y) ) != sizeof(tag.y) ) assert( 0 );		
		if( stream.Read( &model_id, sizeof(model_id) ) != sizeof(model_id) ) assert( 0 );
		
		tag.x += x * fMapLength;
		tag.y += y * fMapLength;
		
		if( pTerrainPropInfo->GetPropType( model_id ) == CTerrainPropInfo::PROP_NPC ) tag.prop_type = _PROP_CONTACT_SCRIPT_INFO::NPC;
		else if( pTerrainPropInfo->GetPropType( model_id ) == CTerrainPropInfo::PROP_USE_NX3 ) tag.prop_type = _PROP_CONTACT_SCRIPT_INFO::PROP;
		{
			// 알수없는 타입.
			// assert( 0 );
		}

		// 펑션 리스트 읽음
		std::vector< ScriptTag > vTagList;
		LoadFunctionInfo( stream, vTagList );

		// 펑션 리스트를 복사.
		std::vector< _PROP_CONTACT_SCRIPT_INFO::_FUNCTION_LIST > vFunctionList;
		for( std::vector< ScriptTag >::iterator it = vTagList.begin(); it != vTagList.end(); ++it )
		{
			_PROP_CONTACT_SCRIPT_INFO::_FUNCTION_LIST tmp;
			if( (*it).nTrigger == SCRTRG_INITIALIZE ) tmp.trigger_id = _PROP_CONTACT_SCRIPT_INFO::_FUNCTION_LIST::TRIGER_INIT;
			else if( (*it).nTrigger == SCRTRG_CONTACT ) tmp.trigger_id = _PROP_CONTACT_SCRIPT_INFO::_FUNCTION_LIST::TRIGER_CONTACT;
			tmp.stdFunction = (*it).strFunction;
			vFunctionList.push_back( tmp );
		}

		// 프랍 스크립트 정보 등록
		GameContent::RegisterPropContactScriptInfo( tag.prop_id, tag.prop_type, model_id, tag.x, tag.y, vFunctionList );
	}
}

static void SetDefaultLocation( int x, int y, float fMapLength, int LocationId )
{
	GameContent::MapLocationInfo location_info;

	X2D::Point< AR_UNIT > pt[4];

	pt[0].Set( x * fMapLength, y * fMapLength );
	pt[1].Set( (x + 1) * fMapLength, y * fMapLength );
	pt[2].Set( (x + 1) * fMapLength, (y + 1) * fMapLength );
	pt[3].Set( x * fMapLength, (y + 1) * fMapLength );

	location_info.priority = 0x7ffffffe;
	location_info.location_id = LocationId;
	
	location_info.Set( &pt[0], &pt[0] + 4 );

	GameContent::RegisterMapLocationInfo( location_info );
}

static void LoadFieldPropFile( const char * szFileName, int x, int y, float fAttrLen, float fMapLength )
{
	if( !XFileUtil::IsFile( szFileName ) )
		return;

	std::auto_ptr< KStream > pStream( new KFileStream( szFileName ) );

#pragma pack( 1 )
	struct NFM_QUEST_PROP_HEADER_V1
	{
		char	szSign[18];
		DWORD	dwVersion;
		int		nPropCount;
	};

	struct NFM_QUEST_PROPSTRUCT_V3
	{
		int						nQuestDBID;
		float 					x,y;
		float 					fZOffSet;
		float 					fRotateX, fRotateY, fRotateZ;
		float 					fScaleX, fScaleY, fScaleZ;
		WORD  					wPropNum;
		bool					bLockHeight;
		float					fLockHeight;
		short					nTextureGroupIndex;
	};

	struct NFM_QUEST_PROPSTRUCT_V2
	{
		int						nQuestDBID;
		float 					x,y;
		float 					fZOffSet;
		float 					fRotateX, fRotateY, fRotateZ;
		float 					fScaleX, fScaleY, fScaleZ;
		WORD  					wPropNum;
		bool					bLockHeight;
		float					fLockHeight;
	};

	struct NFM_QUEST_PROPSTRUCT_V1
	{
		int	nQuestDBID;
		float x,y;
		float fZOffSet;
		float fRotateX, fRotateY, fRotateZ;
		float fScale;
		WORD  wPropNum;
	};
#pragma pack()

	NFM_QUEST_PROP_HEADER_V1 header;

	pStream->Read( &header, sizeof( header ) );

	NFM_QUEST_PROPSTRUCT_V3 prop_info;

	for( int i = 0; i < header.nPropCount; ++i )
	{
		pStream->Read( &prop_info, sizeof( prop_info ) );

		AR_UNIT real_x, real_y;

		real_x = fMapLength * x + prop_info.x;
		real_y = fMapLength * y + prop_info.y;

		FieldPropManager::GetInstance().RegisterFieldProp( GameContent::FIELD_PROP_RESPAWN_INFO( prop_info.nQuestDBID, real_x, real_y, 0, prop_info.fZOffSet, prop_info.fRotateX, prop_info.fRotateY, prop_info.fRotateZ, prop_info.fScaleX, prop_info.fScaleY, prop_info.fScaleZ, prop_info.bLockHeight, prop_info.fLockHeight ) );
	}
}

static void LoadAttributeFile( const char * szFileName, int x, int y, float fAttrLen, float fMapLength )
{
	if( !XFileUtil::IsFile( szFileName ) )
		return;

	std::auto_ptr< KStream > pStream( new KFileStream( szFileName ) );

	int nPolygonCnt = 0;

	pStream->Read( &nPolygonCnt, sizeof( nPolygonCnt ) );

	X2D::Polygon< int > block_info;

	for( int i = 0; i < nPolygonCnt; ++i )
	{
		int nPtCnt = 0;
		pStream->Read( &nPtCnt, sizeof( nPtCnt ) );

		if( nPtCnt < 3 ) continue;

		X2D::Point<int> * pPoints = new X2D::Point<int>[nPtCnt];

		pStream->Read( pPoints, sizeof( X2D::Point<int> ) * nPtCnt );

		block_info.Clear();

		for( int nPointNum = 0; nPointNum < nPtCnt; ++nPointNum )
		{
			pPoints[nPointNum].x = fMapLength * x + pPoints[nPointNum].x * fAttrLen;
			pPoints[nPointNum].y = fMapLength * y + pPoints[nPointNum].y * fAttrLen;
		}

		if( block_info.Set( pPoints, pPoints + nPtCnt ) )
		{
			GameContent::RegisterBlockInfo( block_info );
		}

		delete [] pPoints;
	}
}

static void LoadEventAreaFile( const char * szFileName, int x, int y, float fAttrLen, float fMapLength )
{
	if( !XFileUtil::IsFile( szFileName ) )
		return;

	std::auto_ptr< KStream > pStream( new KFileStream( szFileName ) );

	int nEventAreaCount = 0;

	pStream->Read( &nEventAreaCount, sizeof( nEventAreaCount ) );

	for( int nEventAreaIdx = 0 ; nEventAreaIdx < nEventAreaCount ; ++nEventAreaIdx )
	{
		int nEventAreaID = 0;

		pStream->Read( &nEventAreaID, sizeof( nEventAreaID ) );

		int nPolygonCnt = 0;

		pStream->Read( &nPolygonCnt, sizeof( nPolygonCnt ) );

		X2D::Polygon< int > block_info;

		for( int i = 0; i < nPolygonCnt; ++i )
		{
			int nPtCnt = 0;
			pStream->Read( &nPtCnt, sizeof( nPtCnt ) );

			if( nPtCnt < 3 ) continue;

			X2D::Point<int> * pPoints = new X2D::Point<int>[nPtCnt];

			pStream->Read( pPoints, sizeof( X2D::Point<int> ) * nPtCnt );

			block_info.Clear();

			for( int nPointNum = 0; nPointNum < nPtCnt; ++nPointNum )
			{
				pPoints[nPointNum].x = fMapLength * x + pPoints[nPointNum].x * fAttrLen;
				pPoints[nPointNum].y = fMapLength * y + pPoints[nPointNum].y * fAttrLen;
			}

			if( block_info.Set( pPoints, pPoints + nPtCnt ) )
			{
				GameContent::RegisterEventAreaBlock( nEventAreaID, block_info );
			}

			delete [] pPoints;
		}
	}
}

static void LoadLocationFile( const char * szFileName, int x, int y, float fAttrLen, float fMapLength )
{
#pragma pack( 1 )
	struct LOCATION_INFO_HEADER
	{
		int			nPriority;
		
		struct
		{
			float x;
			float y;
			float z;
		} vCenterPosition;

		float		fRadius;
	};
#pragma pack()

	GameContent::MapLocationInfo location_info;

	if( !XFileUtil::IsFile( szFileName ) )
		return;

	std::auto_ptr< KStream > pStream( new KFileStream( szFileName ) );

	if( pStream.get() == NULL )
		return;

	if( !pStream->IsValid() )
		return;

	int nLocalSize = 0;

	pStream->Read( &nLocalSize, sizeof( int ) );

	for( int nLocalCount = 0; nLocalCount < nLocalSize; ++nLocalCount )
	{
		LOCATION_INFO_HEADER	LocationInfoHeader;

		pStream->Read( &LocationInfoHeader, sizeof( LOCATION_INFO_HEADER ) );

		location_info.priority = LocationInfoHeader.nPriority;

		//스크립트 읽기
		int nCharSize = 0;

		pStream->Read( &nCharSize, sizeof( int ) );

		if( nCharSize > 1 )
		{
			// 이름. 서버에서 쓸 일 없음. 그냥 read만 해주3
			char* pLocalName = new char[nCharSize];
			pStream->Read( pLocalName, nCharSize );
			delete [] pLocalName;
		}

		pStream->Read( &nCharSize, sizeof( int ) );

		g_currentLocationId = 0;

		if( nCharSize > 1 )
		{
			char* pScript = new char[nCharSize];
			pStream->Read( pScript, nCharSize );
			XStringUtil::Trim( pScript );
			LUA()->RunString( pScript );
			delete [] pScript;
		}

		// 예전에 룰 정의 되기 전 nfc파일들은 정보를 제대로 가지고 있지 않다.
		if( !g_currentLocationId )
			return;

		location_info.location_id = g_currentLocationId;

		int nPolygonSize = 0;

		pStream->Read( &nPolygonSize, sizeof( int ) );

		for( int nPolygonCount = 0; nPolygonCount < nPolygonSize ; ++nPolygonCount )
		{
			int nPointCount = 0;

			location_info.Clear();

			pStream->Read( &nPointCount, sizeof( int ) );

			POINT* pPoints = new POINT[nPointCount];
			pStream->Read( pPoints, sizeof( POINT ) * nPointCount );

			X2D::Point< AR_UNIT > * pt = new X2D::Point< AR_UNIT >[nPointCount];

			// 아. 변수 이름 구리군.;
			for( int nPointNum = 0; nPointNum < nPointCount; ++nPointNum )
			{
				pt[nPointNum].x = fMapLength * x + pPoints[nPointNum].x * fAttrLen;
				pt[nPointNum].y = fMapLength * y + pPoints[nPointNum].y * fAttrLen;
			}

			location_info.Set( &pt[0], &pt[0]+nPointCount );


			GameContent::RegisterMapLocationInfo( location_info );

			delete [] pPoints;
			delete [] pt;
		}
	}
}

static void LoadRegionInfo( KFileStream & stream, int x, int y, float fMapLength )
{
	DWORD	nLocationCount = 0;
	int		anLocation[4];
	char	szBuf[1024];
	
	stream.Read( &nLocationCount, sizeof(nLocationCount) );
	for( ; nLocationCount; --nLocationCount )
	{
		szBuf[0] = 0;
		stream.Read( anLocation, sizeof(anLocation) );

		anLocation[0] *= s_fTileSize;
		anLocation[1] *= s_fTileSize;
		anLocation[2] *= s_fTileSize;
		anLocation[3] *= s_fTileSize;

		anLocation[0] += ( x * fMapLength );
		anLocation[1] += ( y * fMapLength ); 
		anLocation[2] += ( x * fMapLength );
		anLocation[3] += ( y * fMapLength ); 

		int nLength;
		stream.Read( &nLength, sizeof(nLength) );
		if( nLength > sizeof( szBuf ) ) throw XException( "too big region name!" );
		stream.Read( szBuf, nLength );
		szBuf[nLength] = '\0';
		
		g_vRegionList.push_back( ScriptRegion( anLocation[0],anLocation[1],anLocation[2],anLocation[3], szBuf ) );
	}
}

static void LoadScripFile( const char *szFullPathName, int x, int y, float fMapLength, CTerrainPropInfo *pPropInfo )
{
	NFS_HEADER_V02 header;

	if( !szFullPathName || !szFullPathName[0] ) return;

	// 화일 열기
	KFileStream stream( szFullPathName );

	// 헤더 읽음
	if( stream.Read( &header, sizeof(header) ) != sizeof(header) ) throw XException( std::string( "Invalid file ! " ) );

	// 버젼 확인
	if( memcmp( header.szSign, NFSFILE_SIGN, strlen(NFSFILE_SIGN) ) )
	{
		throw XException( std::string( "Invalid script header ! " ) + szFullPathName );
	}
	if( header.dwVersion != c_dwNFSCurrentVer )
	{
		throw XException( std::string( "Invalid script version ! " ) + szFullPathName );
	}

	// 영역정보 로딩
	stream.Seek( header.dwEventLocationOffset, KFileStream::seekSet );
	LoadRegionInfo( stream, x, y, fMapLength );
	
	// 영역 스크립트 로딩
	stream.Seek( header.dwEventScriptOffset, KFileStream::seekSet );
	LoadRegionScriptInfo( stream, g_vScriptEvent );

	// 프랍정보 & 스크립트 로딩
	stream.Seek( header.dwPropScriptOffset, KFileStream::seekSet );
	LoadPropScriptInfo( pPropInfo, stream, x, y, fMapLength );

	// 매 맵파일의 영역정보 인덱스는 0 부터 새로 시작하기에 각 맵별 영역 인덱스를 구분하기 위해서 현재까지 읽은 영역정보를 보관
	s_nCurrentRegionIdx = g_vRegionList.size();
}

bool MapLoader::onProcess( int nThreadNum )
{
	// 심리스 설정 파일 초기화
	bool bResult;
	_myTerrainSeamlessWorldInfo seamlessWorldInfo;
	bResult = seamlessWorldInfo.Initialize( "TerrainSeamlessWorld.cfg", false );
	if( !bResult ) throw XException( std::string( "TerrainSeamlessWorld.cfg read error ! " ) );

	// 프랍 설정 파일 초기화
	_myTerrainPropInfo			propInfo;
	bResult = propInfo.Initialize( "TerrainPropInfo.cfg" );
	if( !bResult ) throw XException( std::string( "TerrainPropInfo.cfg read error ! " ) );

	// 타일 사이즈 설정
	s_fTileSize = seamlessWorldInfo.GetTileLength();
	ENV().Bind( "game.tile_size", &s_fTileSize );

	// 맵 갯수 설정
	const KSize& rMapCount = seamlessWorldInfo.GetMapCount();

	// 한 맵의 길이 설정
	float fMapLength = seamlessWorldInfo.GetTileLength()*seamlessWorldInfo.GetSegmentCountPerMap()*seamlessWorldInfo.GetTileCountPerSegment();
	g_fMapLength = fMapLength;
	float fAttrLen = seamlessWorldInfo.GetTileLength() / (float) c_nAttrCountPerTile;

	// 각 맵의 로케이션 정보와 스크립트 파일을 읽는다
	for( int y = 0; y < rMapCount.cy; ++y )
	{
		for( int x = 0; x < rMapCount.cx; ++x )
		{
			// { 로케이션 정보 읽기
			std::string strLocationFileName = seamlessWorldInfo.GetLocationFileName( x, y );

			if( strLocationFileName.empty() ) continue;

			if( seamlessWorldInfo.GetWorldID( x, y ) != -1 )
				SetDefaultLocation( x, y, fMapLength, seamlessWorldInfo.GetWorldID( x, y ) );

			LoadLocationFile( ResourceManager::GetFullPathname( strLocationFileName.c_str() ).c_str(), x, y, fAttrLen, fMapLength );

			// }

			// { 스크립트 파일 읽기
			std::string strScriptFileName = seamlessWorldInfo.GetScriptFileName( x, y );

			if( strScriptFileName.empty() ) continue;
			
			FILELOG( "Loading Script : %s", strScriptFileName.c_str() );
			_cprint( "Loading Script : %s\n", strScriptFileName.c_str() );
			LoadScripFile( ResourceManager::GetFullPathname( strScriptFileName.c_str() ).c_str(), x, y, fMapLength, &propInfo );
			// }

			// { 속성 정보 읽기
			std::string strAttributeFileName = seamlessWorldInfo.GetAttributePolygonFileName( x, y );

			if( strAttributeFileName.empty() ) continue;

			if( !GameRule::bSkipLoadingAttribute )
				LoadAttributeFile( ResourceManager::GetFullPathname( strAttributeFileName.c_str() ).c_str(), x, y, fAttrLen, fMapLength );
			// }

			// { 이벤트 영역 정보 읽기
			std::string strEventAreaFileName = seamlessWorldInfo.GetEventAreaFileName( x, y );

			if( strEventAreaFileName.empty() ) continue;

			LoadEventAreaFile( ResourceManager::GetFullPathname( strEventAreaFileName.c_str() ).c_str(), x, y, fAttrLen, fMapLength );
			// }
		}
	}

	FILELOG( "Map Loading Complete.." );
	_cprint( "Map Loading Complete..\n" );

	return true;
}

void MapLoader::onEnd( bool bIsCancel )
{
}

bool MapLoader::InitMapInfo()
{
	std::vector< ScriptRegionInfo >::iterator it1;
	std::vector< ScriptTag >::iterator	it2;
	
	// 리스폰 정보라면 리스폰 스크립트 호출후 해당 스크립트를 제거한다.
	for( it1 = g_vScriptEvent.begin(); it1 != g_vScriptEvent.end(); ++it1 )
	{
		for( it2 = (*it1).vInfoList.begin(); it2 != (*it1).vInfoList.end();  )
		{
			if( (*it2).nTrigger == _PROP_CONTACT_SCRIPT_INFO::_FUNCTION_LIST::TRIGER_INIT )
			{
				// _oprint( "INIT:%s\n", (*it2).strFunction.c_str() );
				LUA()->RunString( (*it2).strFunction.c_str() );
				it2 = (*it1).vInfoList.erase( it2 );
				continue;
			}
			++it2;
		}
	}

	return true;
}

bool FieldPropDataLoader::onProcess( int nThreadNum )
{
	// 심리스 설정 파일 초기화
	bool bResult;
	_myTerrainSeamlessWorldInfo seamlessWorldInfo;
	bResult = seamlessWorldInfo.Initialize( "TerrainSeamlessWorld.cfg", false );
	if( !bResult ) throw XException( std::string( "TerrainSeamlessWorld.cfg read error ! " ) );

	// 프랍 설정 파일 초기화
	_myTerrainPropInfo			propInfo;
	bResult = propInfo.Initialize( "TerrainPropInfo.cfg" );
	if( !bResult ) throw XException( std::string( "TerrainPropInfo.cfg read error ! " ) );

	// 타일 사이즈 설정
	s_fTileSize = seamlessWorldInfo.GetTileLength();
	ENV().Bind( "game.tile_size", &s_fTileSize );

	// 맵 갯수 설정
	const KSize& rMapCount = seamlessWorldInfo.GetMapCount();

	// 한 맵의 길이 설정
	float fMapLength = seamlessWorldInfo.GetTileLength()*seamlessWorldInfo.GetSegmentCountPerMap()*seamlessWorldInfo.GetTileCountPerSegment();
	g_fMapLength = fMapLength;
	float fAttrLen = seamlessWorldInfo.GetTileLength() / (float) c_nAttrCountPerTile;

	// 각 맵의 로케이션 정보와 스크립트 파일을 읽는다
	for( int y = 0; y < rMapCount.cy; ++y )
	{
		for( int x = 0; x < rMapCount.cx; ++x )
		{
			// { 프랍 정보 읽기
			std::string strPropFileName = seamlessWorldInfo.GetFieldPropFileName( x, y );

			if( strPropFileName.empty() ) continue;

			LoadFieldPropFile( ResourceManager::GetFullPathname( strPropFileName.c_str() ).c_str(), x, y, fAttrLen, fMapLength );
			// }

		}
	}

	FILELOG( "FieldPropData Loading Complete.." );
	_cprint( "FieldPropData Loading Complete..\n" );

	return true;
}


void FieldPropDataLoader::onEnd( bool bIsCancel )
{
}

struct dbEventAreaHandler : public CADORecordBinding, GameContent::EVENT_AREA_INFO
{
	int		nEventAreaID;
	char	szEnterHandler[ 512 ];
	char	szLeaveHandler[ 512 ];

BEGIN_ADO_BINDING( dbEventAreaHandler )
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, nEventAreaID, sizeof(nEventAreaID), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, m_nBeginTime, sizeof(m_nBeginTime), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, m_nEndTime, sizeof(m_nEndTime), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, m_nMinLevel, sizeof(m_nMinLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, m_nMaxLevel, sizeof(m_nMaxLevel), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adBigInt, m_nRaceJobLimit, sizeof(m_nRaceJobLimit), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(7, adInteger, m_nActivateCondition[ 0 ], sizeof(m_nActivateCondition[ 0 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(8, adInteger, m_nActivateValue[ 0 ][ 0 ], sizeof(m_nActivateValue[ 0 ][ 0 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(9, adInteger, m_nActivateValue[ 0 ][ 1 ], sizeof(m_nActivateValue[ 0 ][ 1 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(10, adInteger, m_nActivateCondition[ 1 ], sizeof(m_nActivateCondition[ 1 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, adInteger, m_nActivateValue[ 1 ][ 0 ], sizeof(m_nActivateValue[ 1 ][ 0 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, adInteger, m_nActivateValue[ 1 ][ 1 ], sizeof(m_nActivateValue[ 1 ][ 1 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(13, adInteger, m_nActivateCondition[ 2 ], sizeof(m_nActivateCondition[ 2 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, adInteger, m_nActivateValue[ 2 ][ 0 ], sizeof(m_nActivateValue[ 2 ][ 0 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, adInteger, m_nActivateValue[ 2 ][ 1 ], sizeof(m_nActivateValue[ 2 ][ 1 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(16, adInteger, m_nActivateCondition[ 3 ], sizeof(m_nActivateCondition[ 3 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(17, adInteger, m_nActivateValue[ 3 ][ 0 ], sizeof(m_nActivateValue[ 3 ][ 0 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(18, adInteger, m_nActivateValue[ 3 ][ 1 ], sizeof(m_nActivateValue[ 3 ][ 1 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(19, adInteger, m_nActivateCondition[ 4 ], sizeof(m_nActivateCondition[ 4 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(20, adInteger, m_nActivateValue[ 4 ][ 0 ], sizeof(m_nActivateValue[ 4 ][ 0 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(21, adInteger, m_nActivateValue[ 4 ][ 1 ], sizeof(m_nActivateValue[ 4 ][ 1 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(22, adInteger, m_nActivateCondition[ 5 ], sizeof(m_nActivateCondition[ 5 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(23, adInteger, m_nActivateValue[ 5 ][ 0 ], sizeof(m_nActivateValue[ 5 ][ 0 ]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(24, adInteger, m_nActivateValue[ 5 ][ 1 ], sizeof(m_nActivateValue[ 5 ][ 1 ]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(25, adInteger, m_nLimitActivateCount, sizeof(m_nLimitActivateCount), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(26, adVarChar, szEnterHandler, _countof(szEnterHandler), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(27, adVarChar, szLeaveHandler, _countof(szLeaveHandler), FALSE)
END_ADO_BINDING()
};

void onEventAreaHandlerInfo( dbEventAreaHandler * emprs )
{
	emprs->m_strEnterHandler = emprs->szEnterHandler;
	emprs->m_strLeaveHandler = emprs->szLeaveHandler;

	// 이벤트 영역 추가할 필요가 없는 경우 로딩 제외
	// 스크립트의 내용이 비어있거나 DB 업데이트 파일 추출 수식 오류로 인해 0 이라고만 들어있을 경우(엑셀함수 Cell을 1번째 인자를 "contents"로 사용하면 대상 셀이 비어있을 경우 0으로 나오는 문제)
	if( ( emprs->m_strEnterHandler.empty() || !emprs->m_strEnterHandler.compare( "0" ) ) &&
		( emprs->m_strLeaveHandler.empty() || !emprs->m_strLeaveHandler.compare( "0" ) ) )
	{
		assert( 0 );
		return;
	}

	GameContent::RegisterEventAreaInfo( emprs->nEventAreaID, *emprs );
}

bool EventAreaHandlerLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		_ConnectionPtr		ConnPtr = NULL;

		InitContentDbConnection( ConnPtr );

		helper.start();
		size_t cnt = LoadDbResource< dbEventAreaHandler >( "EventAreaResource", ConnPtr, onEventAreaHandlerInfo );
		helper.end( "EventAreaHandlerLoader" );

		_cprint( "Total %d Event Area info loaded...\n", cnt );
		FILELOG( "Total %d Event Area info loaded...", cnt );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "EventAreaHandlerLoader" );
		LogDBError( e, "EventAreaHandlerLoader", "EventAreaResource" );

		std::string strError = "EVENT AREA RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}

void EventAreaHandlerLoader::onEnd( bool bIsCancel )
{
}
