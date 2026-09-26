///////////////////////////////////////////////////////////////////////  
//	Constants
//
//	(c) 2003 IDV, Inc.
//
//	*** INTERACTIVE DATA VISUALIZATION (IDV) PROPRIETARY INFORMATION ***
//
//	This software is supplied under the terms of a license agreement or
//	nondisclosure agreement with Interactive Data Visualization and may
//	not be copied or disclosed except in accordance with the terms of
//	that agreement.
//
//      Copyright (c) 2001-2003 IDV, Inc.
//      All Rights Reserved.
//
//		IDV, Inc.
//		1233 Washington St. Suite 610
//		Columbia, SC 29201
//		Voice: (803) 799-1699
//		Fax:   (803) 931-0320
//		Web:   http://www.idvinc.com
//

#pragma once
#include "Filename.h"


// paths
const CFilename c_strDataPath = "../data/TheValley/";

// filenames
static const char* c_pBrushMapFilename = "../data/TheValley/brush_2.dds";
static const char* c_pTerrainMapFilename = "../data/TheValley/base.dds";
static const char* c_pTerrainDetail1 = "../data/TheValley/detail.dds";
static const char* c_pTerrainDetail2 = "../data/TheValley/detail2.dds";

// vertex shader constant locations
const	int		c_nShaderLightPosition = 16;
const	int		c_nShaderGrassBillboard = 4;
const	int		c_nShaderGrassLodParameters = 8;
const	int		c_nShaderCameraPosition = 9;
const	int		c_nShaderGrassWindDirection = 10;
const	int		c_nShaderGrassPeriods = 11;
const	int		c_nShaderLeafAmbient = 47;
const	int		c_nShaderLeafDiffuse = 48;
const	int		c_nShaderLeafSpecular = 49;
const	int		c_nShaderLeafLightingAdjustment = 50;

// lighting
const	float	c_afLightDirBumpMapping[3] = { 0.8f, 0.0f, 0.4f };
const	float	c_afLightDir[3] = { 0.7434f, 0.0f, 0.57f };
const	float	c_afLightPos[3] = { c_afLightDir[0] * 1000.0f, c_afLightDir[1] * 1000.0f, c_afLightDir[2] * 1000.0f };

// grass
const	float	c_fDefaultGrassFarLod = 300.0f;
const	float	c_fGrassFadeLength = 50.0f;
const	float	c_fMinBladeNoise = -0.2f;
const	float	c_fMaxBladeNoise = 0.2f;
const	float	c_fMinBladeThrow = 1.0f;
const	float	c_fMaxBladeThrow = 2.5f;
const	float	c_fMinBladeSize = 7.5f;
const	float	c_fMaxBladeSize = 10.0f;
const	int		c_nNumBladeMaps = 2;
const	float	c_fGrassBillboardWideScalar = 1.75f;
const	float	c_fWalkerHeight = 12.0f;
const	int		c_nDefaultGrassBladeCount = 33000;
const	int		c_nGrassRegionCount = 20;

// terrain
const	float	c_fTerrainBaseRepeat = 1.0f;
const	float	c_fTerrainDetail1Repeat = 80.0f;
const	float	c_fTerrainDetail2Repeat = 8.0f;
const	float	c_fShimmerExponent = 3.0f;							///< controls linearity of shimmer speed

// lod
const   float   c_fShadowCutoff = 5000.0f;                          ///< how far out shadows are no longer drawn (feet)
const   float   c_fBillboardAlphaValue = 0.3294f;                   ///< value for glAlphaFunc() for single billboards
const	float	c_fNearLod = 200.0f;
const	float	c_fFarLod = 750.0f;

// misc
const   float   c_fDeg2Rad = 57.29578f;                             ///< divide degrees by this to get radians

