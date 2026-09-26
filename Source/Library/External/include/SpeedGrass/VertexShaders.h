///////////////////////////////////////////////////////////////////////  
//	SpeedTreeRT DirectX Example
//
//	(c) 2003 IDV, Inc.
//
//	This example demonstrates how to render trees using SpeedTreeRT
//	and DirectX.  Techniques illustrated include ".spt" file parsing,
//	static lighting, dynamic lighting, LOD implementation, cloning,
//	instancing, and dynamic wind effects.
//
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


///////////////////////////////////////////////////////////////////////  
//	Includes

#pragma once
#include <map>
#include <string>
#include "D3DUtil.h"


///////////////////////////////////////////////////////////////////////  
//	Vertex shader constant locations

const unsigned int c_nShaderCompositeMatrix = 0;		///< 4 vectors
const unsigned int c_nShaderUnitGrassBillboard = 4;		///< 4 vectors
const unsigned int c_nShaderLodParams = 8;				///< 1 vector
const unsigned int c_nShaderCameraPos = 9;				///< 1 vector
const unsigned int c_nShaderWindDirection = 10;			///< 1 vector
const unsigned int c_nShaderTimeValues = 11;			///< 1 vector
//const unsigned int c_nShaderLightDirection = 21;
//const unsigned int c_nShaderLightAmbient = 22;
//const unsigned int c_nShaderLightDiffuse = 23;
const unsigned int c_nShaderLightInfos = 21;			///< 3 vectors

const float c_fShaderTimeScale = 1.0f;					///< (< 1.0) slower wind, (> 1.0) = faster wind
const float c_fShaderWindPeriod = 1.0f;					///< (< 1.0) longer rolling effects, (> 1.0) = shorter rolling effects


///////////////////////////////////////////////////////////////////////  
///	Grass Vertex Format
static DWORD D3DFVF_SPEEDGRASS_VERTEX =
		D3DFVF_XYZ |
		D3DFVF_NORMAL |
		D3DFVF_DIFFUSE |
		D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0)	 ///< always have first texture layer coords
		| D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE2(1)	 ///< GPU Only - wind weight and index passed in second texture layer
		| D3DFVF_TEX3 | D3DFVF_TEXCOORDSIZE2(2); ///< shadow texture coordinates


///////////////////////////////////////////////////////////////////////
/// FVF Grass Vertex Structure
struct SFVFGrassVertex
{
		D3DXVECTOR3		m_vPosition;			///< Always Used							
		D3DXVECTOR3		m_vNormal;				///< Dynamic Lighting Only			
		DWORD			m_dwDiffuseColor;		///< Static Lighting Only	
		// texture layer 0
		FLOAT			m_fTexCoords[2];		///< Always Used
		// texture layer 1
		FLOAT			m_fVertexIndex;
		FLOAT			m_fBladeSize;
		// texture layer 2
		FLOAT			m_fWindWeight;
		FLOAT			m_fNoiseFactor;
};


///////////////////////////////////////////////////////////////////////  
//	SpeedGrass Vertex Shader Source
//
//	c[0-3]		composite projection/world matrix
//	c[4-7]		unitized billboard
//	c[8].x		far LOD distance (distance at which grass shrinks)
//	c[8].y		shrink transition length
//	c[9]		camera position
//	c[10]		global wind direction
//	c[11].x		global time (in seconds)
//	c[11].y		global wind effect period
//	c20			useful general constants (defined locally)
//	c21			light direction
//	c22			light ambient color
//	c23			light diffuse color
//	c24			grass ambient color scalar
//	c30			used in generating sin/cos
//	c31			used in generating sin/cos
//	c32			used in generating sin/cos
//
//	Texture Layer Contents:
//	-----------------------
//
//		Layer 0 (v7)
//		S: s texture coordinate into grass texture
//		T: t texture coordinate into grass texture
//	
//		Layer 1 (v8)
//		S: vertex index (0 to 3)
//		T: blade size (scalar value around 1.0)
//	
//		Layer 2 (v9)
//		S: wind weight (allows each blade of grass to sway differently)
//		T: noise factor (keeps rolling wind effect from looking too perfect)

static const char g_szGrassVertexProgram[] = 
{
	// identity shader version
	"vs.1.1\n"
	
	"dcl_position		v0\n"
	"dcl_normal			v3\n"
	"dcl_color			v5\n"
	"dcl_texcoord0		v7\n"
	"dcl_texcoord1		v8\n"
	"dcl_texcoord2		v9\n"


	//////////////////////////////////////////////////////////////////////////////
	// Section:		Local constants definitons

	// general constants
	"def		c20,		1.0,		0.5,		0.0,		0.0\n"

	// lighting constants
	//"def		c21,		-0.69,		0.0,		0.717,		1.0\n"	// light direction
	//"def		c22,		0.0,		0.0,		0.0,		1.0\n"	// light ambient color
	//"def		c23,		2.0,		2.0,		2.0,		1.0\n"	// light diffuse color
	//"def		c24,		0.33,		0.33,		0.33,		1.0\n"	// grass ambient color scalar
	"def		c24,		1.0,		1.0,		1.0,		1.0\n"	// grass ambient color scalar

	// sin/cos constants
	"def		c30,	0.25,				0.5,		0.75,					1.0\n"
	"def		c31,	-24.9808039603f,	60.1458091736f, -85.4537887573f,	64.9393539429f\n"
	"def		c32,	-19.7392082214f,	1.0f,		-1.0f,					0.159154\n"


	//////////////////////////////////////////////////////////////////////////////
	// Section:		Compute unique angle value for this vertex to
	//				pass into sin/cos section for rolling wind effect
	// Input:		v0 (raw vertex pos)
	//				v9.y (noise factor)
	// Output:		r1.x = unique angle
	// Registers:	r1
	// Constants:	c10, c11
	
	"mov		r1.y,		v9.y\n"							// mad cannot access two vertex registers at once
	"mad		r1.xyz,		v0,			c11.y,		r1.y\n"	// made unique point using original vertex (v0), noise of
															// blade (r1.y) and global period (c11.y)
	"dp3		r1,			r1,			c10\n"				// take wind direction into account
	"add		r1.x,		r1.x,		c11.x\n"			// add global time in order to animate the wind effect


	//////////////////////////////////////////////////////////////////////////////
	// Section:		Compute Sine and Cosine of same angle
	// Input:		r1.x = angle (in radians)
	// Ouput:		r0.x = cos(r1.x)
	//				r0.y = sin(r1.x)
	// Registers:	r0, r1, r2
	// Constants:	c30, c31, c32
	//
	// * Taken from NVIDIA paper "Where Is That Instruction?  How
	//   to Implement "Missing" Vertex Shader Instructions," #20,
	//	 written by Matthias Wloka (mwloka@nvidia.com)

	"mul		r1.x,		c32.w,		r1.x\n"				// normalize input
	"expp		r1.y,		r1.x\n"							// and extract fraction
	"slt		r2,			r1.yyyy,	c30\n"				// range check, one each to indicate:
	"add		r2.yzw,		r2.xyzw,	-r2.xxyz\n"			// [0,.25),[.25,.5),[.5,.75),[.75,1.0)
	"dp3		r1.z,		r2.yzwx,	c30.yywx\n"			// calc shift for cos
	"dp4		r1.w,		r2,			c30.xxzz\n"			// calc shift for sin
	"add		r0.xz,		r1.yyyy,	-r1.zzww\n"			// x for cos, z for sin
	"mul		r0.xz,		r0.xxzz,	r0.xxzz\n"			// [ x^2, #, z^2, #]
	"mul		r0.yw,		r0.xxzz,	r0.xxzz\n"			// [ x^2, x^4, z^2, z^4]
	"mad		r1,			c31.xyxy,	r0.yyww, c31.zwzw\n"
	"mad		r1,			r1,			r0.yyww, c32.xyxy\n"
	"mad		r1.xz,		r1,			r0.xxzz, r1.yyww\n"
	"dp4		r0.x,		r2,			c32.yzzy\n"			// sign for cos
	"dp4		r0.y,		r2,			c32.yyzz\n"			// sign for sin
	"mul		r0.xy,		r0.xyww,	r1.xzww\n"


	//////////////////////////////////////////////////////////////////////////////
	// Section:		Move vertex based on wind effects
	// Input:		v0 (raw vertex pos)
	//				r0.x = cosine
	//				r0.y = sine
	//				v9.x = wind weight
	// Ouput:		r6 = wind-blown vertex
	// Registers:	
	// Constants:	none

	"mov		r6,			v0\n"								// passthrough .z and .w values
	"mad		r6.xy,		v9.x,		r0,			r6.xy\n"	// (x,y) value of vertex follows sin/cos


	//////////////////////////////////////////////////////////////////////////////
	// Section:		Compute distance from camera position to current vertex
	// Input:		v0 (raw vertex pos)
	// Ouput:		r3.x = distance(camera, vertex)
	// Registers:	r3
	// Constants:	c9 (camera pos)

	"sub		r3,			v0,			c9\n"				// find vector difference between camera and vertex
	"dp3		r3.x,		r3,			r3\n"				// square and sum the distance components
	"rsq		r3.x,		r3.x\n"							// find square root for true distance, not squared
	"rcp		r3.x,		r3.x\n"


	//////////////////////////////////////////////////////////////////////////////
	// Section:		In order for the blades of grass to fade out, alpha values
	//				need to fade from 1.0 to 0.0 when a vertex is between
	//				c8.x (lod far distance) and c8.x + c8.y (lod far + shrink
	//				distance away).  The specific equation is:
	//
	//				alpha = 1.0 - (distance - far_lod) / shrink_distance
	//
	//				The alpha value is then clipped to 0.0 to 1.0.
	// Input:		r3.x = distance(camera, vertex)
	// Ouput:		r4.x = alpha value
	// Registers:	r3
	// Constants:	c8.x, c8.y

	"sub		r4.y,		r3.x,		c8.x\n"				// subtract far_lod from distance
	"rcp		r4.z,		c8.y\n"							// divide difference by shrink_dist
	"mul		r4.w,		r4.y,		r4.z\n"				
	"sub		r4.x,		c20.x,		r4.w\n"				// subtract value from 1.0
	"max		r4.x,		r4.x,		c20.z\n"			// clamp to > 0.0
	"min		r4.x,		r4.x,		c20.x\n"			// clamp to < 1.0
	

	//////////////////////////////////////////////////////////////////////////////
	// Section:		Takes the unit grass blade billboard and adds it to the
	//				incoming position.  The vertex index (stored in v8.x)
	//				is used to know which of the four vertices is incoming.
	// Input:		v8.x (vertex index)
	//				r6 (wind blown vertex)
	// Ouput:		r4.x = alpha value
	// Registers:	r3
	// Constants:	c4, c5, c6, c7 (grass blade unit billboard)

	"mov	a0.x,		v8.x\n"							// vertex index stored in v8.x
	"mad	r1,			c[a0.x+4],	v8.y,		r6\n"	// scale the billboard c[a0.x+4] by 
														// by v8.y and add r6 (wind blown vertex)
	

	//////////////////////////////////////////////////////////////////////////////
	// Section:		Lighting computation (optional)
	// Input:		
	// Ouput:		
	// Registers:	
	// Constants:	

	"mov	r2,			c22\n"							// move lighting ambient into register
	"mul	r3,			r2,			v5\n"				// multiply grass ambient by lighting ambient
	"mov	r2,			c23\n"							// move lighting diffuse into register
	"mul	r5,			r2,			v5\n"				// multiply grass diffuse by lighting ambient
	"dp3	r2.x,		v3,			c21\n"				// dot light direction with vertex normal
	"max	r2.x,		r2.x,		c20.z\n"			// clamp dot to >= 0.0
	"mad	oD0.xyz,	r2.x,		r5,			r3\n"
	
	//"mov	oD0.xyz,	v5\n"	// 머여 이건.. ㅡ.,ㅡ by blackfish


	//////////////////////////////////////////////////////////////////////////////
	// Section:		Write final outputs
	// Input:		r1 (wind blown position)
	//				v5 (color)
	//				r4.x (alpha value)
	// Ouput:		oPos, oD0, oT0
	// Registers:	none
	// Constants:	c0, c1, c2, c3 (composite matrix)

	"m4x4	oPos,		r1,			c[0]\n"			    // project to screen
	"mov	oD0.w,		r4.x\n"							// color alpha value
	"mov	oT0.xy,		v7\n"							// pass texcoords through

	// end vertex shader
};


///////////////////////////////////////////////////////////////////////  
///	LoadGrassShader
static LPDIRECT3DVERTEXSHADER9 LoadGrassShader(LPDIRECT3DDEVICE9 pDx)
{
    // assemble shader
	LPDIRECT3DVERTEXSHADER9 dwShader;
	LPD3DXBUFFER pCode, pError;
    if (D3DXAssembleShader(g_szGrassVertexProgram, sizeof(g_szGrassVertexProgram) - 1, 0, NULL, 0, &pCode, &pError) == D3D_OK)
	{
		if (pDx->CreateVertexShader((DWORD*)pCode->GetBufferPointer( ), &dwShader) != D3D_OK)
		{
			char szError[1024];
			sprintf(szError, "Failed to create grass vertex shader.");
			MessageBox(NULL, szError, "Rappelz-Vertex Shader Error", MB_ICONSTOP);
		}
	}
	else
    {
        char szError[1024];
	    sprintf(szError, "Failed to assemble grass vertex shader.\nThe error reported is [ %s ].\n", pError->GetBufferPointer( ));
	    MessageBox(NULL, szError, "Rappelz-Vertex Shader Error", MB_ICONSTOP);
    }

	if (pCode)
    	pCode->Release( );

	return dwShader;
}


/////////////////////////////////////////////////////////////////////////  
////	SetupVertexShaderConstants
//
//static void SetupVertexShaderConstants(LPDIRECT3DDEVICE9 pDx, D3DXMATRIX& cCompositeMatrix, float fTime)
//{
//	// composite matrix
//	D3DXMatrixTranspose(&cCompositeMatrix, &cCompositeMatrix);
//	pDx->SetVertexShaderConstantF(c_nShaderCompositeMatrix, reinterpret_cast<float*>(&cCompositeMatrix), 4);
//
//	// pass in unitized billboarded grass quad
//	const float* pUnitBB = CSpeedGrassWrapper::GetUnitBillboard( );
//	const float c_fGrassWidth = 2.0f;
//	for (int i = 0; i < 4; ++i)
//	{
//		float afVector[4] = { c_fGrassWidth * pUnitBB[i * 3 + 0],
//							  c_fGrassWidth * pUnitBB[i * 3 + 1],
//							  pUnitBB[i * 3 + 2],
//							  0.0f };
//		pDx->SetVertexShaderConstantF(c_nShaderUnitGrassBillboard + i, afVector, 1);
//	}
//
//	// setup LOD information
//	float afVector[4] = { 0.0f };
//	CSpeedGrassWrapper::GetLodParams(afVector[0], afVector[1]);
//	pDx->SetVertexShaderConstantF(c_nShaderLodParams, afVector, 1);
//
//	// setup camera information
//	const float* pCameraPos = CSpeedGrassWrapper::GetCameraPos( );
//	pDx->SetVertexShaderConstantF(c_nShaderCameraPos, pCameraPos, 1);
//
//	// set global wind direction
//	pDx->SetVertexShaderConstantF(c_nShaderWindDirection, CSpeedGrassWrapper::GetWindDirection( ), 1);
//
//	// set time values
//	float afTimeValues[4] = { fTime * c_fShaderTimeScale, c_fShaderWindPeriod, 0.0f, 0.0f };
//	pDx->SetVertexShaderConstantF(c_nShaderTimeValues, afTimeValues, 1);
//}
