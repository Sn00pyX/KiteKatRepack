// Time.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <internal_base_include.h>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <ctime>
#include <Windows.h>

#include "../../include/toolkit/XVerifyProcessSigner.h"



int _tmain(int argc, _TCHAR* argv[])
{
	WCHAR szFileName[MAX_PATH];

#ifdef UNICODE
	lstrcpynW(szFileName, argv[1], MAX_PATH);
#else
	if (mbstowcs(szFileName, argv[1], MAX_PATH) == -1)
	{
		printf("Unable to convert to unicode.\n");
		return 1;
	}
#endif

	BYTE bySerialNumber[16] = { 0x0d, 0xf9, 0xee, 0x3c, 0xfb, 0xc6, 0xd8, 0xde, 0xe0, 0x77, 0x7f, 0x92, 0x63, 0xce, 0x06, 0xdf };
	std::wstring msg;
	SYSTEMTIME timestamp;
	if( XVerifyProcessSigner( szFileName, L"Gala Lab Corp.", bySerialNumber, sizeof( bySerialNumber ), &timestamp, &msg ) == false )
	{
		return 1;
	}

	return 0;
}

