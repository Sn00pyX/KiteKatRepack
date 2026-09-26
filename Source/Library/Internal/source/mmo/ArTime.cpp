
#include "../../include/mmo/ArTime.h"


int		g_arTimeAdjust = 0;

AR_TIME	GetArTime()
{
	return (GetSafeTickCount() / 10) + g_arTimeAdjust;
}

void	SetArTimeAdjust( int adjust )
{
	g_arTimeAdjust = adjust;
}

int		GetArTimeAdjust()
{
	return g_arTimeAdjust;
}
