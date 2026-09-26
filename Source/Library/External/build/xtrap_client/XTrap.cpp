
/******************************************************************************
// XTrap.cpp Version 1005
// By WiseLogic
//****************************************************************************/

#ifdef __XTRAPMODULE_H

#include "stdafx.h"
#include "stdio.h"
#include "XTrap.h"


///////////////////////////////////////////////////////////////////////////
// XTrap Start
///////////////////////////////////////////////////////////////////////////
void SetSocketHandle(HANDLE SockHandle, DWORD Port);

void XTrapStartFunc()
{
	char	sGameName[MAX_PATH];	

	DWORD	ApiVersion		= 0xA5001128;
	
	DWORD	VendorCode		= 0x00000005;				
	DWORD	KeyboardType	= 0x00000000;
	DWORD	PatchType		= 0x00000010;	// Patch Enable : 0x00000010
	DWORD	ModuleType		= 0x0FFFFFFF;

	sprintf(sGameName, "%s",	"Rappelz");					/* Game Name				*/
	
	/*Option SetXTrapMgrInfo(char *pMgrIp)		*/
	SetXTrapPatchHttpUrl("http://securitypatch.rappelz.com");
	SetXTrapStartInfo	(sGameName, ApiVersion, VendorCode, KeyboardType, PatchType, ModuleType);

	XTrapStart			();
	XTrapKeepAlive		();
}

///////////////////////////////////////////////////////////////////////////
// XTrap Stop
///////////////////////////////////////////////////////////////////////////
void XTrapStopFunc()
{
	XTrapStop();
}

///////////////////////////////////////////////////////////////////////////
// Example XTrapMessageThreadProc
///////////////////////////////////////////////////////////////////////////
void XTrapMessageFunc()
{	
	return;

	if (g_bApiMal			== TRUE ||
		g_bMemoryMdl		== TRUE ||
		g_bAutoMousMdl		== TRUE ||
		g_bAutoKeybMdl		== TRUE ||
		g_bMalMdl			== TRUE ||
		g_bSpeedMdl			== TRUE ||
		g_bFileMdl			== TRUE ||
		g_bApiHookMdl		== TRUE ||
		g_bDebugModMdl		== TRUE ||
		g_bMemoryCrack		== TRUE ||
		g_bFileCrack		== TRUE ||
		g_bApiHookCrack		== TRUE ||
		g_bOsMdl			== TRUE	||
		g_bPatchMdl			== TRUE ||
		g_bApiCrack			== TRUE ||
		g_bStartXTrap		== FALSE)
	{
		// 통신 채널 정리
		// WinsockEnd();
	}
	else
		return;

	if(g_bApiMal)
	{	MessageBox(NULL,"프로그램이 정상작동하지 않습니다","보안경고",NULL);									ExitProcess(0);	}

	if(g_bMemoryMdl)
	{	MessageBox(NULL,"메모리 상태이상입니다","보안경고",NULL); 												ExitProcess(0);	}

	if(g_bAutoMousMdl)
	{	MessageBox(NULL,"오토마우스가 검출되었습니다","보안경고",NULL); 										ExitProcess(0);	}

	if(g_bAutoKeybMdl)
	{	MessageBox(NULL,"오토키보드가 검출되었습니다","보안경고",NULL);											ExitProcess(0);	}

	if(g_bMalMdl)
	{	MessageBox(NULL,"게임에 영향을주는 프로그램이 실행중입니다","보안경고",NULL); 							ExitProcess(0);	}

	if(g_bSpeedMdl)
	{	MessageBox(NULL,"게임속도가 비정상적입니다","보안경고",NULL); 											ExitProcess(0);	}

	if(g_bFileMdl)
	{	MessageBox(NULL,"비정상적인 파일입니다","보안경고",NULL);												ExitProcess(0);	}

	if(g_bApiHookMdl)
	{	MessageBox(NULL,"윈도우 상태이상입니다","보안경고",NULL); 												ExitProcess(0);	}

	if(g_bDebugModMdl)
	{	MessageBox(NULL,"디버그 모드로 실행되었습니다","보안경고",NULL); 										ExitProcess(0);	}

	if(g_bMemoryCrack)
	{	MessageBox(NULL,"비정상적인 접근이 탐지되었습니다 [MC]","보안경고",NULL); 								ExitProcess(0);	}

	if(g_bFileCrack)
	{	MessageBox(NULL,"비정상적인 접근이 탐지되었습니다 [FC]","보안경고",NULL); 								ExitProcess(0);	}

	if(g_bApiHookCrack)
	{	MessageBox(NULL,"비정상적인 접근이 탐지되었습니다 [HC]","보안경고",NULL);								ExitProcess(0);	}

	if (g_bOsMdl)
	{	MessageBox(NULL,"파일 속성에서 호환성 모드가 설정되었습니다. 설정을 해제하십시오.","보안경고",NULL);	ExitProcess(0);	}

	if (g_bPatchMdl)
	{	MessageBox(NULL,"패치 서버 접속에 문제가 있습니다. 네트워크 상황을 확인하십시오.","보안경고",NULL); 	ExitProcess(0);	}

	if (g_bApiCrack)
	{	MessageBox(NULL,"비 정상적인 접근 시도가 감지 되었습니다.","보안경고",NULL); 	ExitProcess(0);	}

	if (g_bStartXTrap == FALSE)
	{	MessageBox(NULL,"게임 보안모듈을 실행할수 없습니다.","보안경고",NULL); 									ExitProcess(0);	}


	TerminateProcess(GetCurrentProcess(),0);

}



DWORD WINAPI XTrapMessageThreadProc(LPVOID lpParameter)
{
	DWORD dwReturn;

	do
	{
		dwReturn = WaitForSingleObject(g_XTrapMsgEvent, 5000);

		if		(dwReturn == WAIT_FAILED)			break;
		else if (dwReturn == WAIT_ABANDONED)		break;
		else if (dwReturn == WAIT_OBJECT_0)			break;
		else if (dwReturn == WAIT_TIMEOUT)
		{
			XTrapMessageFunc();
			continue;
		}

	}while(TRUE);

	CloseHandle(g_XTrapMsgEvent);
	g_XTrapMsgEvent = NULL;
	
	return TRUE;
}

#endif