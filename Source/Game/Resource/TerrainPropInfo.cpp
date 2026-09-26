
#include <windows.h>

#include <toolkit/XStringUtil.h>
#include <dump/XException.h>

#include "MapUtil.h"
#include "TerrainPropInfo.h"

namespace
{
	const char* c_szNX3Tail = ".nx3";
	const char* c_szNAFTail = "_default.naf";
	const char* c_szSPTTail = ".spt";
	const char* c_szCOBTail = ".cob";
};

CTerrainPropInfo::CTerrainPropInfo()
	: m_pPropInfo( NULL )
{
}

CTerrainPropInfo::~CTerrainPropInfo()
{
	Release();
}

int CTerrainPropInfo::GetShadowFlag( const char * pStr )
{
	//Game Define 의 SHADOW_TYPE 과 같아야 한다.
	if( _stricmp( pStr, "no_ca" ) == 0 ) return (1 << 0); 
	if( _stricmp( pStr, "dy_ca" ) == 0 ) return (1 << 1); 
	if( _stricmp( pStr, "st_ca" ) == 0 ) return (1 << 2); 
	if( _stricmp( pStr, "no_re" ) == 0 ) return (1 << 3); 
	if( _stricmp( pStr, "re"    ) == 0 ) return (1 << 4); 

	return (1 << 0)|(1 << 3);
}

bool CTerrainPropInfo::Initialize( const char* szPropInfoFileName )
{
	Release();

	m_pPropInfo = new PROPINFO_STRUCT[ c_nMaxPropCount ];
	for( int n = 0; n < c_nMaxPropCount; ++n )
	{
		m_pPropInfo[n].Type          = PROP_UNUSED;
		m_pPropInfo[n].RenderType    = PROPRENDER_GENERAL;
		m_pPropInfo[n].nCategory     = 0;
		m_pPropInfo[n].fVisibleRatio = 1.f;
		m_pPropInfo[n].strName.clear();
	}

	// 파일로부터 text들을 읽어낸다.
	STRING_VECTOR TextLines;
	{
		KStream* pStream = GetResourceStream( szPropInfoFileName );
		if( NULL == pStream ) return false;

		int nTextLength = int( pStream->GetLength() );
		char* pEntireText = new char[ nTextLength + 1 ];

		pStream->Read( pEntireText, sizeof(char) * nTextLength );
		pEntireText[ nTextLength ] = '\0';
		delete pStream;

		CStringUtil::GetTextLinesFromString( pEntireText, TextLines );
		delete [] pEntireText;
	}

	const char* c_szRenderType   = "RENDERTYPE=";
	const char* c_szCategory     = "CATEGORY=";
	const char* c_szVisibleRatio = "VISIBLE_RATIO=";
	const char* c_szPropName     = "PROPNAME=";

	const char* c_szRenderTypeGeneral  = "general";
	const char* c_szRenderTypeBuilding = "building";

	RENDER_TYPE CurrentRenderType = PROPRENDER_GENERAL;
	int nCurrentCategory = 0;
	float fCurrentVisibleRatio = 1.f;

	for( size_t n = 0; n < TextLines.size(); ++n )
	{
		std::string strLine = TextLines[n];

		if( ';' == strLine[0] ) continue;
		std::string strContent;

		// prop
		if( CStringUtil::GetContentString( strLine, c_szPropName, strContent ) )
		{
			std::vector< std::string > vString;
			XStringUtil::Split( strContent.c_str(), vString, ", " ); //,

			if( vString.size() <= 1 ) continue;

			std::string strPropNum  = vString[0].c_str();
			std::string strPropName = vString[1].c_str();
			int			nShadowFlag = (1 << 0)|(1 << 3);

			if( vString.size() == 4 )
			{
				//그림자 캐스팅 까지 설정
				nShadowFlag  = GetShadowFlag( vString[2].c_str() );
				nShadowFlag |= GetShadowFlag( vString[3].c_str() );
			}

			//// ',' 문자를 찾는다.
			//int nCommaPos = static_cast<int>( strContent.find( ',' ) );
			//if( nCommaPos != std::string::npos )
			//{
			//	// ',' 앞의 prop 번호
			//	std::string strPropNum;
			//	strPropNum.assign( strContent, 0, nCommaPos );

			//	// ',' 뒤의 prop 이름
			//	std::string strPropName;
			//	strPropName.assign( strContent, nCommaPos + 1, strContent.size() - (nCommaPos + 1) );

			if( strPropNum.size() > 0 && strPropName.size() > 0 )
			{
				WORD wPropNum = static_cast<WORD>( ::atoi( strPropNum.c_str() ) );

				if( CheckPropFileType( strPropName, c_szNX3Tail ) )
					SetPropInfo( static_cast<int>( n ), wPropNum, PROP_USE_NX3, CurrentRenderType, nCurrentCategory, fCurrentVisibleRatio, strPropName, nShadowFlag );

				else if( CheckPropFileType( strPropName, c_szNAFTail ) )
					SetPropInfo( static_cast<int>( n ), wPropNum, PROP_USE_NAF, CurrentRenderType, nCurrentCategory, fCurrentVisibleRatio, strPropName, nShadowFlag );

				else if( CheckPropFileType( strPropName, c_szSPTTail ) )
					SetPropInfo( static_cast<int>( n ), wPropNum, PROP_SPEEDTREE, CurrentRenderType, nCurrentCategory, fCurrentVisibleRatio, strPropName, nShadowFlag );

				else if( CheckPropFileType( strPropName, c_szCOBTail ) )
				{
//					_oprint( "PROP_NPC : %d, %s\n", wPropNum, strPropName.c_str() );
					SetPropInfo( static_cast<int>( n ), wPropNum, PROP_NPC, CurrentRenderType, nCurrentCategory, fCurrentVisibleRatio, strPropName, nShadowFlag );
				}
			}
		}
		// category
		else if( CStringUtil::GetContentString( strLine, c_szCategory, strContent ) )
		{
			if( strContent.size() > 0 )
			{
				nCurrentCategory = (-1);
				for( size_t nCategory = 0; nCategory < m_CategoryNames.size(); ++nCategory )
				{
					if( m_CategoryNames[nCategory] == strContent )
					{
						// 이미 존재하는 category명인 경우 그대로 사용
						nCurrentCategory = static_cast<int>(nCategory);
						break;
					}
				}

				// 새로운 category명을 추가
				if( (-1) == nCurrentCategory )
				{
					m_CategoryNames.push_back( strContent );
					nCurrentCategory = static_cast<int>( m_CategoryNames.size() - 1 );
				}
			}
		}
		// visible ratio
		else if( CStringUtil::GetContentString( strLine, c_szVisibleRatio, strContent ) )
		{
			fCurrentVisibleRatio = float(::atof( strContent.c_str() ));
			if( 0.f == fCurrentVisibleRatio ) fCurrentVisibleRatio = 1.f;
		}
		// render type
		else if( CStringUtil::GetContentString( strLine, c_szRenderType, strContent ) )
		{
			if( strContent == c_szRenderTypeGeneral )
				CurrentRenderType = PROPRENDER_GENERAL;

			else if( strContent == c_szRenderTypeBuilding )
				CurrentRenderType = PROPRENDER_BUILDING;
		}
	}

	// category가 한번도 안 나온 경우
	if( m_CategoryNames.size() == 0 )
		m_CategoryNames.push_back( "?" );

	return true;
}

void CTerrainPropInfo::Release()
{
	m_CategoryNames.clear();
	if( NULL != m_pPropInfo )
	{
		delete [] m_pPropInfo;
		m_pPropInfo = NULL;
	}
}

std::string CTerrainPropInfo::GetNX3NameByNAFName( const char* szNAFFileName )
{
	std::string strFileName;
	strFileName.assign( szNAFFileName, 0, ::strlen(szNAFFileName) - ::strlen(c_szNAFTail) );

	return (strFileName + c_szNX3Tail);
}

void CTerrainPropInfo::SetPropInfo( int nLineIndex, WORD wPropNum, PROP_TYPE Type, RENDER_TYPE RenderType, int nCategory, float fVisibleRatio, const std::string& rName, int nShadowFlag )
{
	m_pPropInfo[ wPropNum ].nLineIndex    = nLineIndex;
	m_pPropInfo[ wPropNum ].Type          = Type;
	m_pPropInfo[ wPropNum ].RenderType    = RenderType;
	m_pPropInfo[ wPropNum ].nCategory     = nCategory;
	m_pPropInfo[ wPropNum ].fVisibleRatio = fVisibleRatio;
	m_pPropInfo[ wPropNum ].strName       = rName;
	m_pPropInfo[ wPropNum ].nShadowFlag   = nShadowFlag;
}
