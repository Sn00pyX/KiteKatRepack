#pragma once

#include "KTypes.h"


// 맵 파일 관련
#define NFMFILE_SIGN "nFlavor Map"

const DWORD c_dwNFMCurrentVer = 22;
const DWORD c_dwNFMSegmentVer = 0;

#pragma pack(push, 1)

// 맵 팀에서 21 버전 NFM 파일을 22 버전으로 컨버팅 작업 완료하기 전까지는 임시로 21 버전 사용(2009.09.24)
typedef struct NFM_HEADER_V22 NFM_LATESTHEADER;

struct NFM_HEADER_V22
{
	char szSign[16];
	DWORD dwVersion;
	DWORD dwMapPropertiesOffset;	// 맵 속성 정보 시작 위치
	DWORD dwTerrainSegmentOffset;	// 지형 세그먼트 정보 시작 위치
	DWORD dwPropOffset;				// Prop 정보 시작 위치
	DWORD dwVectorAttrOffset;		// 벡터 속성 정보 시작 위치
	DWORD dwWaterOffset;			// 물 정보 시작 위치
	DWORD dwGrassColonyOffset;		// 풀 군락 정보 시작 위치
	DWORD dwEventAreaOffset;		// 이벤트 영역 정보 시작 위치
	int nTileCountPerSegment;
	int nSegmentCountPerMap;
	float fTileLength;
};

struct NFM_HEADER_V19
{
	char szSign[16];
	DWORD dwVersion;
	DWORD dwMapPropertiesOffset;	// 맵 속성 정보 시작 위치
	DWORD dwTerrainSegmentOffset;	// 지형 세그먼트 정보 시작 위치
	DWORD dwPropOffset;				// Prop 정보 시작 위치
	DWORD dwVectorAttrOffset;		// 벡터 속성 정보 시작 위치
	DWORD dwWaterOffset;			// 물 정보 시작 위치
	DWORD dwGrassColonyOffset;		// 풀 군락 정보 시작 위치
	int nTileCountPerSegment;
	int nSegmentCountPerMap;
	float fTileLength;
};

typedef NFM_HEADER_V19 NFM_HEADER_V18;
typedef NFM_HEADER_V18 NFM_HEADER_V17;

struct NFM_HEADER_V16
{
	char szSign[16];
	DWORD dwVersion;
	DWORD dwMapPropertiesOffset; // 맵 속성 정보 시작 위치
	DWORD dwTerrainSegmentOffset; // 지형 세그먼트 정보 시작 위치
	DWORD dwPropOffset; // Prop 정보 시작 위치
	DWORD dwVectorAttrOffset; // 벡터 속성 정보 시작 위치
	DWORD dwWaterOffset; // 물 정보 시작 위치
	int nTileCountPerSegment;
	int nSegmentCountPerMap;
	float fTileLength;
};
typedef NFM_HEADER_V16 NFM_HEADER_V15;

struct NFM_HEADER_V14
{
	char szSign[16];
	DWORD dwVersion;
	DWORD dwTerrainSegmentOffset; // 지형 세그먼트 정보 시작 위치
	DWORD dwPropOffset; // Prop 정보 시작 위치
	DWORD dwVectorAttrOffset; // 벡터 속성 정보 시작 위치
	int nTileCountPerSegment;
	int nSegmentCountPerMap;
	float fTileLength;
};
typedef NFM_HEADER_V14 NFM_HEADER_V13;
typedef NFM_HEADER_V13 NFM_HEADER_V12;

struct NFM_HEADER_V10
{
	char szSign[16];
	DWORD dwVersion;
	DWORD dwTerrainSegmentOffset; // 지형 세그먼트 정보 시작 위치
	DWORD dwPropOffset; // Prop 정보 시작 위치
	int nTileCountPerSegment;
	int nSegmentCountPerMap;
	float fTileLength;
};
typedef NFM_HEADER_V10 NFM_HEADER_V11;

struct NFM_SEGMENTHEADER_V11
{
	DWORD dwVersion;
	WORD tile[3];
};

struct NFM_VERTEXSTRUCT_V11
{
	float fHeight;
	DWORD wFillBits[2];	// 제일 낮은 단계의 바닥 텍스처가 아닌경우에 유효하다. 4*4의 2bit 배열
	unsigned __int64  wAttribute;
	KTripleColor Color;
};

struct NFM_VERTEXSTRUCT_V10
{
	float fHeight;
	WORD  wTile;
	WORD  wAttribute;
	KTripleColor Color;
};

struct NFM_VERTEXSTRUCT_V9
{
	float fHeight;
	WORD  wTile;
	KTripleColor Color;
};

struct NFM_VERTEXSTRUCT_V8
{
	float fHeight;
	WORD  wTile;
};

struct NFM_PROPSTRUCT_V15
{
	int nPropIndex;
	float x, y;
	float fZOffset;
	float fRotateX, fRotateY, fRotateZ;
	float fScaleX, fScaleY, fScaleZ;
	WORD  wPropNum;
	bool  bLockHeight;
	float fLockHeight;
	short nTextureGroupIndex;
};

struct NFM_PROPSTRUCT_V14
{
	int nPropIndex;
	float x, y;
	float fZOffset;
	float fRotateX, fRotateY, fRotateZ;
	float fScaleX, fScaleY, fScaleZ;
	WORD  wPropNum;
	bool  bLockHeight;
	float fLockHeight;
};

struct NFM_PROPSTRUCT_V13
{
	int nPropIndex;
	float x, y;
	float fZOffset;
	float fRotateX, fRotateY, fRotateZ;
	float fScale;
	WORD  wPropNum;
};

struct NFM_PROPSTRUCT_V10
{
	int nPropIndex;
	float x, y;
	float fRotation;
	WORD  wPropNum;
};

struct NFM_PROPSTRUCT_V8
{
	float x, y;
	float fRotation;
	WORD  wPropNum;
};
#pragma pack(pop)

// Prop 관련

// 상위 12비트는 map index, 하위 20비트는 prop index로 사용한다.
// 따라서 map index는 0 ~ 4095까지 가능하고, prop index는 0 ~ 1048575 까지 가능하다.
// ( 일단 충분히 큰 값이라 생각하고 현재 맵툴에서 따로 index의 한계를 제한해두지는 않았음. )
typedef DWORD TERRAINPROP_ID;

inline TERRAINPROP_ID CreateTerrainPropID( int nMapIndex, int nPropIndex )
	{ return (TERRAINPROP_ID(nMapIndex) << 20) | (TERRAINPROP_ID(nPropIndex) & 0x000FFFFF); }

inline int GetMapIndexOfTerrainPropID( TERRAINPROP_ID TerrainPropID )
	{ return int(TerrainPropID >> 20); }

inline int GetPropIndexOfTerrainPropID( TERRAINPROP_ID TerrainPropID )
	{ return int(TerrainPropID & 0x000FFFFF); }
