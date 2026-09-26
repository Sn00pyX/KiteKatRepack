
#include "../../include/toolkit/SafeTickCount.h"

#include <limits>


DWORD	GetSafeTickCount()
{
	static DWORD s_dwStartTick = ::GetTickCount() - 1;

	DWORD dwCurrentTick = ::GetTickCount();
	if( dwCurrentTick < s_dwStartTick )
	{
		dwCurrentTick = (std::numeric_limits< DWORD >::max)() - s_dwStartTick + dwCurrentTick;
	}
	else
	{
		dwCurrentTick -= s_dwStartTick;
	}

	return dwCurrentTick;
}
