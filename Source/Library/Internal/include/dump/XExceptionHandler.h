#pragma once

#include <dbghelp.h>
#include <algorithm>

#include "../toolkit/XThread.h"

// UserFunc 타입 : void (WINAPI *PCERUserFunc)( HANDLE hFile );

#ifndef STATUS_INVALID_PARAMETER
#define STATUS_INVALID_PARAMETER         ((LONG)0xC000000DL)
#endif  /* STATUS_INVALID_PARAMETER */

namespace XSEH
{

	extern volatile bool g_bUnhandledExceptionRaised;

	typedef XThread::ThreadInfo THREAD_INFO;

	enum DUMP_LEVEL
	{
		DUMP_LEVEL_NORMAL = 0,
		DUMP_LEVEL_HIGH = 1
	};
	extern void StartExceptionHandler( PTSTR pszProgamName = NULL, bool bWriteFile = true );
	extern void SetProgramName( PTSTR pszProgamName = NULL );
	extern void SetDumpLevel( DUMP_LEVEL nDumpLevel );
	extern void EndExceptionHandler();
	extern void SetUserFunc( void (WINAPI *lpUserFunc)( HANDLE ) );
	extern void AddThreadInfo( THREAD_INFO * info );
	extern void InvokeUnhandledException( DWORD dwExceptionCode, bool bTerminate = true );
	inline void WriteDump()	{ InvokeUnhandledException( STATUS_INVALID_PARAMETER, false ); }
	extern LONG WINAPI DoExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo );

	extern std::string GetExceptionInfo();
	extern std::string GetExceptionAddress();
	extern std::string GetExceptionDetail();

	extern bool Assert( const char *szText, const char *szFile, int nLine );
	extern void* GetFaultAddress();
	extern bool g_bNoXAssert;

	extern void IncreaseAllocCount( const TCHAR * pszTypeName, const bool bNeedToIncTotalCount = true );
	extern void DecreaseAllocCount( const TCHAR * pszTypeName );
	extern BOOL WriteAllocCountInfo( const TCHAR * pszFileName );

}; // namespace XSEH

#define XASSERT( str )	{ static bool _Proc = false; if( !_Proc ) { _Proc = XSEH::Assert( str, __FILE__, __LINE__ ); } }
