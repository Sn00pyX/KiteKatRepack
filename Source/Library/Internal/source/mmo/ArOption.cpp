
#include "../../include/mmo/ArOption.h"

// 가장 직관적인.. boolean matrix -_-
const int s_Matrix[VISIBLE_REGION_BOX_WIDTH][VISIBLE_REGION_BOX_WIDTH] = 
{
	/*
	{ 0,1,1,1,0 },
	{ 1,1,1,1,1 },
	{ 1,1,1,1,1 },
	{ 1,1,1,1,1 },
	{ 0,1,1,1,0 }
	*/

	{ 0,0,1,1,1,0,0 },
	{ 0,1,1,1,1,1,0 },
	{ 1,1,1,1,1,1,1 },
	{ 1,1,1,1,1,1,1 },
	{ 1,1,1,1,1,1,1 },
	{ 0,1,1,1,1,1,0 },
	{ 0,0,1,1,1,0,0 },
};

// 한 region 의 길이를 정의한다.
int		g_nRegionSize	= 150;

volatile bool		g_bUseRegionDebug = false;