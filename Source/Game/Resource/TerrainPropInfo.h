#pragma once

#include <vector>

const int c_nMaxPropCount = 65535; // WORD형으로 저장하므로

class CTerrainPropInfo
{
public:
	CTerrainPropInfo();
	virtual ~CTerrainPropInfo();

	bool Initialize( const char* szPropInfoFileName );
	void Release();

	const std::vector<std::string>& GetPropCategoryNames() const { return m_CategoryNames; }

	enum PROP_TYPE
	{
		PROP_UNUSED, // 사용되지 않는것.
		PROP_USE_NAF, // NAF 파일로 bone 애니메이션과 NX3 mesh 파일
		PROP_USE_NX3, // 애니메이션이 없는 NX3파일
		PROP_SPEEDTREE, // 스피드트리
		PROP_NPC, // NPC
	};
	PROP_TYPE GetPropType( WORD wPropNum ) const { return m_pPropInfo[ wPropNum ].Type; }
	int		  GetShadowFlag( WORD wPropNum ) const { return m_pPropInfo[ wPropNum ].nShadowFlag; }

	enum RENDER_TYPE
	{
		PROPRENDER_GENERAL,
		PROPRENDER_BUILDING,
	};
	RENDER_TYPE GetPropRenderType( WORD wPropNum ) const { return m_pPropInfo[ wPropNum ].RenderType; }

	bool IsValidProp( WORD wPropNum ) const
		{ return ( m_pPropInfo[ wPropNum ].Type != PROP_UNUSED ); }

	int GetPropCategory( WORD wPropNum ) const
		{ return IsValidProp( wPropNum ) ? m_pPropInfo[ wPropNum ].nCategory : (-1); }

	float GetPropVisibleRatio( WORD wPropNum ) const
		{ return IsValidProp( wPropNum ) ? m_pPropInfo[ wPropNum ].fVisibleRatio : 1.f; }

	const char* GetPropName( WORD wPropNum ) const
		{ return IsValidProp( wPropNum ) ? m_pPropInfo[ wPropNum ].strName.c_str() : "?"; }

	const char* GetPropFileName( WORD wPropNum, bool bNoReturnNull = true ) const
		{ return IsValidProp( wPropNum ) ? m_pPropInfo[ wPropNum ].strName.c_str() : (bNoReturnNull ? "" : NULL); }

	const int   GetPropLineIndex( WORD wPropNum ) const
		{ return IsValidProp( wPropNum ) ? m_pPropInfo[ wPropNum ].nLineIndex : (-1); }

	static std::string GetNX3NameByNAFName( const char* szNAFFileName );

private:
	struct PROPINFO_STRUCT
	{
		int			nLineIndex;
		PROP_TYPE   Type;
		RENDER_TYPE RenderType;
		int         nCategory;
		float       fVisibleRatio;
		int			nShadowFlag;
		std::string strName;
	};
	PROPINFO_STRUCT* m_pPropInfo;

	std::vector<std::string> m_CategoryNames;

	int GetShadowFlag( const char * pStr );

	bool CheckPropFileType( const std::string& rName, const char* szTail ) const
	{
		int nFind = int( rName.find( szTail ) );

		return (std::string::npos != nFind) ? (nFind == (rName.size() - ::strlen(szTail))) : false;
	}

	void SetPropInfo( int nLineIndex, WORD wPropNum, PROP_TYPE Type, RENDER_TYPE RenderType, int nCategory, float fVisibleRatio, const std::string& rName, int nShadowFlag );

protected:
	virtual KStream* GetResourceStream( const char* szFileName ) = 0;
};
