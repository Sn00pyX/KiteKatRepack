#pragma once

#include "KTypes.h"
#include <map>


class CTerrainSeamlessWorldInfo
{
public:
	CTerrainSeamlessWorldInfo();
	virtual ~CTerrainSeamlessWorldInfo();

	bool Initialize( const char* szSeamlessWorldInfoFileName, bool bMapFileCheck = true );
	void Release();

	int GetTileCountPerSegment() const { return m_nTileCountPerSegment; }
	int GetSegmentCountPerMap() const { return m_nSegmentCountPerMap; }
	float GetTileLength() const { return m_fTileLength; }

	// "MAPSIZE=" 항목에 정한 가로크기와 세로크기
	const KSize& GetMapCount() const { return m_sizMapCount; }

	int	GetMapLayer() const { return m_nMapLayer; }

	// 맵 파일명/스크립트 파일명을 얻는다. 해당 맵이 없으면 빈 문자열을 리턴한다.
	std::string GetMapFileName( int nMapPosX, int nMapPosY ) const;
	std::string GetScriptFileName( int nMapPosX, int nMapPosY ) const;
	std::string GetLocationFileName( int nMapPosX, int nMapPosY ) const;
	std::string GetAttributePolygonFileName( int nMapPosX, int nMapPosY ) const;
	std::string GetFieldPropFileName( int nMapPosX, int nMapPosY ) const;
	std::string GetMinimapImageFileName( int nMapPosX, int nMapPosY ) const;
	std::string GetLQWaterFileName( int nMapPosX, int nMapPosY ) const;
	std::string GetEventAreaFileName( int nMapPosX, int nMapPosY ) const;
	int			GetWorldID( int nMapPosX, int nMapPosY ) const;

	float GetFOV() const { return m_fFOV; }

private:
	int m_nTileCountPerSegment; // 세그먼트 한쪽면의 타일 갯수
	int m_nSegmentCountPerMap;  // 맵 한쪽면의 세그먼트 갯수
	float m_fTileLength;        // 타일의 한쪽면 길이
	KSize m_sizMapCount;        // 전체 seamless 중 가로/세로 한쪽면의 맵의 갯수
	int m_nMapLayer;

	struct FILENAME_MAPINFO
	{
		std::string	m_strMapFileName;
		int			m_nWorldID;
	};

	typedef std::map<int, FILENAME_MAPINFO> FILENAMEMAP;
	FILENAMEMAP m_FileNameMap; // 파일명 map

	float m_fFOV; // 카메라 FOV값

	std::string GetFileNameWithExt( int nMapPosX, int nMapPosY, const char* szExt ) const
	{
		std::string strFileName;

		if( 0 <= nMapPosX && nMapPosX < m_sizMapCount.cx && 0 <= nMapPosY && nMapPosY < m_sizMapCount.cy )
		{
			FILENAMEMAP::const_iterator it = m_FileNameMap.find( (nMapPosY * m_sizMapCount.cx) + nMapPosX );
			if( it != m_FileNameMap.end() )
			{
				strFileName = it->second.m_strMapFileName;
				strFileName += szExt;
			}
		}
		return strFileName;
	}

protected:
	virtual KStream* GetResourceStream( const char* szFileName ) = 0;
};
