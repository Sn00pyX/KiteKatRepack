
/*************************************************************************************************/
//  X-TRAP Interface Library For Launcher
//  ---------------------------------------------------------------------------------------------
// Copyright (c) 2005 - 2009 WiseLogic. All Rights Reserved
/*************************************************************************************************/

#ifndef __XTRAPAPI_LAUNCHER_H
#define __XTRAPAPI_LAUNCHER_H

#ifdef _USE_XTRAP_MODULE

#pragma comment(lib, "urlmon")
#pragma comment(lib, "wininet")
#pragma comment(lib, "./XTrap/090109_009_XTrap4Launcher_mt.lib")

#endif

VOID XTrap_L_Patch(
	IN LPCSTR	lpArgv,
	IN LPCSTR	lpGamePath, 
	IN DWORD	dwTimeout
);

VOID XTrap_L_Patch(
	IN  LPCSTR	lpArgv, 
	IN  LPCSTR	lpGamePath, 
	IN  DWORD	dwTimeout, 
	OUT LPCSTR	pMsg, 
	OUT LPCSTR	pErrCode,
	OUT BOOL   *pErrFlag
);

#endif
