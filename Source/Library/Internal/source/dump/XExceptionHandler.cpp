
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <tchar.h>
#include <algorithm>
#include <vector>

#include "../../include/toolkit/ILock.h"
#include "../../include/dump/XExceptionHandler.h"
#include "../../include/toolkit/XEnv.h"
#include "../../include/toolkit/safe_function.h"

#pragma comment( lib, "psapi.lib" )


extern "C" {

void * _ReturnAddress();
#pragma intrinsic(_ReturnAddress)

#ifdef _X86_
void * _AddressOfReturnAddress();
#pragma intrinsic(_AddressOfReturnAddress)
#endif /* _X86_ */

}

namespace XSEH
{

void* g_pFaultAddress;

bool g_bNoXAssert;

void* GetFaultAddress() { return g_pFaultAddress; }

bool Assert( const char *szText, const char *szFile, int nLine )
{
	if( g_bNoXAssert ) return false;

	char buf[1024];
	s_sprintf( buf, _countof( buf ), "Do you want to write minidump?\nError : %s\nFile : %s\nLine : %d", szText, szFile, nLine );
	int r = MessageBox( NULL, buf, "Critical Assert !!", MB_YESNOCANCEL | MB_ICONERROR );

	if( r == IDYES )
	{
		XSEH::WriteDump();
		return true;
	}

	if( r == IDNO )
	{
		return true;
	}

	return false;
}

enum BasicType // Stolen from CVCONST.H in the DIA 2.0 SDK
{
	btNoType = 0,
	btVoid = 1,
	btChar = 2,
	btWChar = 3,
	btInt = 6,
	btUInt = 7,
	btFloat = 8,
	btBCD = 9,
	btBool = 10,
	btLong = 13,
	btULong = 14,
	btCurrency = 25,
	btDate = 26,
	btVariant = 27,
	btComplex = 28,
	btBit = 29,
	btBSTR = 30,
	btHresult = 31
};

HANDLE	CreateNewWriteFile( const char* szFullFileName )
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	if ( (hFile = CreateFile( szFullFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL )) == INVALID_HANDLE_VALUE )
	{
		// 파일 열기 실패했다면 파일 이름 끝에 "(숫자)"를 추가하여 생성한다.
		char szDrive[_MAX_DRIVE] = { 0, };
		char szDir[_MAX_DIR] = { 0, };
		char szFileName[_MAX_FNAME] = { 0, };
		char szExt[_MAX_EXT] = { 0, };
		s_splitpath( szFullFileName, szDrive, _countof( szDrive ), szDir, _countof( szDir ), szFileName, _countof( szFileName ), szExt, _countof( szExt ) );

		char szNewFileName[_MAX_FNAME] = { 0, };
		char szNewFullFileName[MAX_PATH] = { 0, };
		int nTryCount = 0;

		while( hFile == INVALID_HANDLE_VALUE )
		{
			++nTryCount;

			s_sprintf( szNewFileName, _countof( szNewFileName ), "%s(%d)", szFileName, nTryCount );
			s_makepath( szNewFullFileName, _countof( szNewFullFileName ), szDrive, szDir, szNewFileName, szExt );
			hFile = ::CreateFile( szNewFullFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
		}
	}

	return hFile;
}


typedef void (WINAPI *PCERUserFunc)( HANDLE hFile );
typedef PCERUserFunc LPCERUserFunc;

class XExceptionHandler
{
public:
	struct ALLOC_COUNT_INFO
	{
		ALLOC_COUNT_INFO( const TCHAR *_type_name )
			: current_alloc_count( 0 )
			, peek_concurrent_alloc_count( 0 )
			, total_alloc_count( 0 )
		{
			s_strcpy( type_name, _countof( type_name ), _type_name );
			type_name[ _countof(type_name)-1 ] = 0;
		}

		TCHAR	type_name[40];
		int		current_alloc_count;
		int		peek_concurrent_alloc_count;
		int		total_alloc_count;
	};

	~XExceptionHandler()
	{
		for( std::vector< ALLOC_COUNT_INFO * >::iterator it = m_vAllocCountInfo.begin() ; it != m_vAllocCountInfo.end() ; ++it )
		{
			delete (*it);
		}
		m_vAllocCountInfo.clear();
	}

	static void SetProgramName( PTSTR pszProgramName );

	static void WriteThreadInfo();
	static void WriteBasicInfo( PEXCEPTION_RECORD pExceptionRecord );
	static void WriteExceptionReport( PEXCEPTION_POINTERS pExceptionInfo );
	static void WriteStackDetails( PCONTEXT pContext, BOOL bWriteVariables );
	static void WriteRegistersInfo( PCONTEXT pContext );
	static void WriteMemoryDump( PCONTEXT pContext );

	static void StoreCoreDump( void );
	static void Dump( DWORD64 pData, DWORD dwSize, BOOL bAlign );

	static LPTSTR GetExceptionString( DWORD dwCode );
	static BOOL GetLogicalAddress( PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset );
	static BOOL CALLBACK EnumerateSymbolsCallback(PSYMBOL_INFO,ULONG, PVOID);
#ifdef _WIN64
	static BOOL FormatSymbolValue( PSYMBOL_INFO, STACKFRAME64 *, char * pszBuffer, unsigned cchBuffer, char * pszNameBuffer, unsigned cchNameBuffer );
#else
	static BOOL FormatSymbolValue( PSYMBOL_INFO, STACKFRAME *, char * pszBuffer, unsigned cchBuffer, char * pszNameBuffer, unsigned cchNameBuffer );
#endif
	static char * DumpTypeIndex( char *, unsigned, DWORD64, DWORD, unsigned, DWORD_PTR, BOOL & );
	static char * FormatOutputValue( char * pszCurrBuffer, unsigned cchBuffer, BasicType basicType, DWORD64 length, PVOID pAddress );
	static BasicType GetBasicType( DWORD typeIndex, DWORD64 modBase );

	static void ProcDump( PEXCEPTION_POINTERS pExceptionInfo );
	static LONG WINAPI MyUnhandledExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo );

	static void MyInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved );
	static void MyPureCallHandler( void );
	static void MySecurityErrorHandler(int code, void * unused);

	static void AddThreadInfo( XSEH::THREAD_INFO * thread_info );

	static void InvokeUnhandledException( DWORD dwExceptionCode, bool bTerminate = true );
	static int __cdecl _tprintf( const TCHAR * format, ... );

	static bool	m_bWriteFile;
	
	static TCHAR m_szLogPrefixName[ MAX_PATH ];
	static TCHAR m_szModuleName[ MAX_PATH ];
	static HANDLE m_hReportFile;
	static HANDLE m_hProcess;
	static BOOL m_bHasSymbol;
	static BOOL m_bIsValidFaultAddress;
	static XCriticalSection m_csDumpLock;

	static std::string m_strExceptionInfo;
	static std::string m_strExceptionAddress;
	static std::string m_strExceptionDetail;	
	static std::string m_strTopStack;
	static std::string m_strReturn;

	static std::vector< XSEH::THREAD_INFO * > m_vThreadInfo;

protected:
	static ALLOC_COUNT_INFO *	_getAllocCountInfo( const TCHAR * pszTypeName );

public:
	static void IncreaseAllocCount( const TCHAR * pszTypeName, const bool bNeedToIncTotalCount = true );
	static void DecreaseAllocCount( const TCHAR * pszTypeName );
	static BOOL WriteAllocCountInfo( const TCHAR *pszFileName );

	static XCriticalSection						m_csAllocCount;
	static std::vector< ALLOC_COUNT_INFO * >	m_vAllocCountInfo;

};

//LPCERUserFunc XExceptionHandler::__lpUserFunc;
//LPTOP_LEVEL_EXCEPTION_FILTER XExceptionHandler::m_OldFilter;

bool	XExceptionHandler::m_bWriteFile = true;
TCHAR XExceptionHandler::m_szLogPrefixName[ MAX_PATH ];
TCHAR XExceptionHandler::m_szModuleName[ MAX_PATH ];
HANDLE XExceptionHandler::m_hReportFile = INVALID_HANDLE_VALUE;
HANDLE XExceptionHandler::m_hProcess;
BOOL XExceptionHandler::m_bHasSymbol;
BOOL XExceptionHandler::m_bIsValidFaultAddress;
std::vector< XSEH::THREAD_INFO * > XExceptionHandler::m_vThreadInfo;
XCriticalSection XExceptionHandler::m_csDumpLock( "DumpLock" );

std::string XExceptionHandler::m_strExceptionInfo;
std::string XExceptionHandler::m_strExceptionAddress;
std::string XExceptionHandler::m_strExceptionDetail;
std::string XExceptionHandler::m_strTopStack;
std::string XExceptionHandler::m_strReturn;

XCriticalSection										XExceptionHandler::m_csAllocCount;
std::vector< XExceptionHandler::ALLOC_COUNT_INFO * >	XExceptionHandler::m_vAllocCountInfo;

static LPTOP_LEVEL_EXCEPTION_FILTER __OldFilter;
static LPCERUserFunc __lpUserFunc;
static int s_nDumpLevel = DUMP_LEVEL_HIGH;


#pragma comment(linker, "/defaultlib:dbghelp.lib")

//
// Constructor
//

XExceptionHandler & GetXExceptionHandler()
{
	static XExceptionHandler seh;

	return seh;
}

void SetDumpLevel( int nDumpLevel )
{
	s_nDumpLevel = nDumpLevel;
}

void SetProgramName( PTSTR pszProgramName )
{
	GetXExceptionHandler().SetProgramName( pszProgramName );
}

void StartExceptionHandler( PTSTR pszProgramName, bool bWriteFile )
{
	GetXExceptionHandler().m_bWriteFile = bWriteFile;

	GetXExceptionHandler().m_bHasSymbol = FALSE;
	GetXExceptionHandler().m_bIsValidFaultAddress = TRUE;
	GetXExceptionHandler().SetProgramName( pszProgramName );
	SetUserFunc( NULL );

	// Install the unhandled exception filter function
	__OldFilter = SetUnhandledExceptionFilter( XExceptionHandler::MyUnhandledExceptionFilter );
	GetXExceptionHandler().m_hProcess = GetCurrentProcess();

	// CRT invalid parameter handler도 바인딩 해준다.
	_set_invalid_parameter_handler( XExceptionHandler::MyInvalidParameterHandler );
	_set_purecall_handler( XExceptionHandler::MyPureCallHandler );
	//_set_security_error_handler( XExceptionHandler::MySecurityErrorHandler );
}

//
// Destructor
//
void EndExceptionHandler()
{
	SetUnhandledExceptionFilter( __OldFilter );
}

void InvokeUnhandledException( DWORD dwExceptionCode, bool bTerminate )
{
	GetXExceptionHandler().InvokeUnhandledException( dwExceptionCode, bTerminate );
}

static XCriticalSection s_cs;

void AddThreadInfo( THREAD_INFO * info )
{
	THREAD_SYNCRONIZE( s_cs );

	GetXExceptionHandler().AddThreadInfo( info );
}

//
// Set application-specific function
//
void SetUserFunc( LPCERUserFunc lpUserFunc )
{
	__lpUserFunc = lpUserFunc;
}


std::string GetExceptionInfo()
{
	return XExceptionHandler::m_strExceptionInfo;
}

std::string GetExceptionAddress()
{
	return XExceptionHandler::m_strExceptionAddress + " " + XExceptionHandler::m_strTopStack;
}

std::string GetExceptionDetail()
{
	return XExceptionHandler::m_strExceptionDetail;
}


void IncreaseAllocCount( const TCHAR * pszTypeName, const bool bNeedToIncTotalCount )
{
	GetXExceptionHandler().IncreaseAllocCount( pszTypeName, bNeedToIncTotalCount );
}

void DecreaseAllocCount( const TCHAR * pszTypeName )
{
	GetXExceptionHandler().DecreaseAllocCount( pszTypeName );
}

BOOL WriteAllocCountInfo( const TCHAR * pszFileName )
{
	// ALLOC_COUNT_INFO의 포인터를 보관하는 벡터에 push_back 밖에 일어날 수 없으므로
	// 벡터 순회 중에 별도 락이 필요 없음
	return GetXExceptionHandler().WriteAllocCountInfo( pszFileName );
}

//
// Set application-name for prefix of log filename
//
void XExceptionHandler::SetProgramName( PTSTR pszProgramName )
{
	if ( pszProgramName == NULL ) {
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFilename[MAX_PATH], szExt[MAX_PATH];

		// Figure out what the report file will be named, and store it away
		GetModuleFileName( 0, m_szLogPrefixName, MAX_PATH );

		PTSTR pszDot = m_szLogPrefixName;
		// Look for the '.' before the "EXE" extension. Replace '.' to '\0'
		if ( (pszDot = _tcsrchr( pszDot, _T('.') )) != NULL )
			*pszDot = 0;

		s_splitpath( m_szLogPrefixName, szDrive, _countof( szDrive ), szDir, _countof( szDir ), szFilename, _countof( szFilename ), szExt, _countof( szExt ) );
		s_strcpy( m_szLogPrefixName, _countof( m_szLogPrefixName ), szFilename );
	} else {
		s_strcpy( m_szLogPrefixName, _countof( m_szLogPrefixName ), pszProgramName );
	}
}

void XExceptionHandler::MyInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved )
{
	InvokeUnhandledException( STATUS_INVALID_PARAMETER );
}

void XExceptionHandler::MyPureCallHandler()
{
	InvokeUnhandledException( STATUS_INVALID_PARAMETER );
}

void XExceptionHandler::MySecurityErrorHandler(int code, void * unused)
{
	InvokeUnhandledException( STATUS_INVALID_PARAMETER );
}

void XExceptionHandler::AddThreadInfo( XSEH::THREAD_INFO * thread_info )
{
	m_vThreadInfo.push_back( thread_info );
}

XExceptionHandler::ALLOC_COUNT_INFO * XExceptionHandler::_getAllocCountInfo( const TCHAR * pszTypeName )
{
	for( std::vector< ALLOC_COUNT_INFO * >::const_iterator it = m_vAllocCountInfo.begin() ; it != m_vAllocCountInfo.end() ; ++it )
	{
		if( !_tcscmp( (*it)->type_name, pszTypeName ) )
		{
			return (*it);
		}
	}

	return NULL;
}

void XExceptionHandler::IncreaseAllocCount( const TCHAR * pszTypeName, const bool bNeedToIncTotalCount )
{
	THREAD_SYNCRONIZE( m_csAllocCount );

	ALLOC_COUNT_INFO *pAllocCountInfo = _getAllocCountInfo( pszTypeName );

	if( !pAllocCountInfo )
	{
		pAllocCountInfo = new ALLOC_COUNT_INFO( pszTypeName );
		m_vAllocCountInfo.push_back( pAllocCountInfo );
	}

	if( pAllocCountInfo->peek_concurrent_alloc_count < ++(pAllocCountInfo->current_alloc_count) )
		pAllocCountInfo->peek_concurrent_alloc_count = pAllocCountInfo->current_alloc_count;

	if( bNeedToIncTotalCount )
		++(pAllocCountInfo->total_alloc_count);

}

void XExceptionHandler::DecreaseAllocCount( const TCHAR * pszTypeName )
{
	THREAD_SYNCRONIZE( m_csAllocCount );

	ALLOC_COUNT_INFO *pAllocCountInfo = _getAllocCountInfo( pszTypeName );

	if( !pAllocCountInfo )
		return;

	--(pAllocCountInfo->current_alloc_count);
}

BOOL XExceptionHandler::WriteAllocCountInfo( const TCHAR *pszFileName )
{
	FILE *fp = NULL;
	if( _tfopen_s( &fp, pszFileName, _T( "at" ) ) )
		return FALSE;

	SYSTEMTIME stSystemTime;
	GetLocalTime( &stSystemTime );

	_ftprintf( fp, _T( "------------------------------------------------------------------------------\r\n" ) );
	_ftprintf( fp, _T( "    Allocation Count Info(by _MEM_USAGE_DEBUG) - %04d/%02d/%02d %02d:%02d:%02d\r\n" ), stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay, stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond );
	_ftprintf( fp, _T( "------------------------------------------------------------------------------\r\n" ) );
	_ftprintf( fp, _T( "\r\n" ) );

	HANDLE hProcess = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc) );
	_ftprintf( fp, _T( "Mem Usage    : %10dKB / %10dKB\r\n"), (int)( pmc.WorkingSetSize >> 10 ), (int)( pmc.PagefileUsage >> 10 ) );
	_ftprintf( fp, _T( "\r\n" ) );

	for( std::vector< ALLOC_COUNT_INFO * >::const_iterator it = m_vAllocCountInfo.begin() ; it != m_vAllocCountInfo.end() ; ++it )
	{
		_ftprintf( fp, _T( "Type: %-40s   Allocated: [Current:%8d]/[Peek Concurrent:%8d]/[Total:%8d]\r\n" ), (*it)->type_name, (*it)->current_alloc_count, (*it)->peek_concurrent_alloc_count, (*it)->total_alloc_count );
	}
	_ftprintf( fp, _T( "\r\n" ) );

	fclose( fp );

	return TRUE;
}


void XExceptionHandler::InvokeUnhandledException( DWORD dwExceptionCode, bool bTerminate )
{
	/* Fake an exception to call reportfault. */
	EXCEPTION_RECORD ExceptionRecord;
	CONTEXT ContextRecord;
	EXCEPTION_POINTERS ExceptionPointers;

#ifdef _X86_

	__asm {
		mov dword ptr [ContextRecord.Eax], eax
		mov dword ptr [ContextRecord.Ecx], ecx
		mov dword ptr [ContextRecord.Edx], edx
		mov dword ptr [ContextRecord.Ebx], ebx
		mov dword ptr [ContextRecord.Esi], esi
		mov dword ptr [ContextRecord.Edi], edi
		mov word ptr [ContextRecord.SegSs], ss
		mov word ptr [ContextRecord.SegCs], cs
		mov word ptr [ContextRecord.SegDs], ds
		mov word ptr [ContextRecord.SegEs], es
		mov word ptr [ContextRecord.SegFs], fs
		mov word ptr [ContextRecord.SegGs], gs
		pushfd
		pop [ContextRecord.EFlags]
	}

	ContextRecord.ContextFlags = CONTEXT_CONTROL;
	ContextRecord.Eip = (ULONG)_ReturnAddress();
	ContextRecord.Esp = (ULONG)_AddressOfReturnAddress();
	ContextRecord.Ebp = *((ULONG *)_AddressOfReturnAddress()-1);

#elif defined (_IA64_) || defined (_AMD64_)

	/* Need to fill up the Context in IA64 and AMD64. */
	RtlCaptureContext(&ContextRecord);

#else /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ContextRecord, sizeof(ContextRecord));

#endif /* defined (_IA64_) || defined (_AMD64_) */

	ZeroMemory(&ExceptionRecord, sizeof(ExceptionRecord));

	ExceptionRecord.ExceptionCode = dwExceptionCode;
	ExceptionRecord.ExceptionAddress = _ReturnAddress();

	ExceptionPointers.ExceptionRecord = &ExceptionRecord;
	ExceptionPointers.ContextRecord = &ContextRecord;

	if( IsDebuggerPresent() )
	{
		DebugBreak();
	}

	if( bTerminate ) 
	{
		UnhandledExceptionFilter(&ExceptionPointers);
	}
	else
	{
		DoExceptionFilter(&ExceptionPointers);
	}

	if( bTerminate ) 
	{
		TerminateProcess(GetCurrentProcess(), dwExceptionCode);
	}
}

volatile bool g_bUnhandledExceptionRaised = false;

LONG WINAPI DoExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo )
{
	XExceptionHandler::ProcDump( pExceptionInfo );
	return EXCEPTION_EXECUTE_HANDLER;
}

void XExceptionHandler::ProcDump( PEXCEPTION_POINTERS pExceptionInfo )
{
	TCHAR szFileName[ MAX_PATH ];

	SYSTEMTIME stSystemTime;
	GetLocalTime( &stSystemTime );

	THREAD_SYNCRONIZE( m_csDumpLock );

	s_strcpy( szFileName, _countof( szFileName ), m_szLogPrefixName );
	s_sprintf( szFileName + strlen( szFileName ), _countof( szFileName ) - strlen( szFileName ), " %04d-%02d-%02d %02d-%02d-%02d.dmp", 
		stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay,
		stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond );

	if( m_bWriteFile == true )
	{
		HANDLE hDumpFile = INVALID_HANDLE_VALUE;
		if ( (hDumpFile = CreateNewWriteFile( szFileName )) != INVALID_HANDLE_VALUE ) {
			MINIDUMP_EXCEPTION_INFORMATION ExInfo;

			ExInfo.ThreadId = ::GetCurrentThreadId();
			ExInfo.ExceptionPointers = pExceptionInfo;
			ExInfo.ClientPointers = NULL;
			if( !MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, s_nDumpLevel == DUMP_LEVEL_HIGH ? MiniDumpWithFullMemory : MiniDumpNormal, &ExInfo, NULL, NULL ) )
			{
				// If MiniDumpWriteDump failed, write the last error code into the dmp file.
				DWORD nLastError = GetLastError();
				DWORD nWrittenBytes = 0;
				TCHAR szBuffer[24];
				s_sprintf( szBuffer, _countof( szBuffer ), "0x%08X(%u)", nLastError, nLastError );
				WriteFile( hDumpFile, szBuffer, static_cast< DWORD >( strlen( szBuffer ) ), &nWrittenBytes, NULL );
			}
			CloseHandle( hDumpFile );
		}
	}

	s_strcpy( szFileName, _countof( szFileName ), m_szLogPrefixName );
	s_sprintf( szFileName + strlen( szFileName ), _countof( szFileName ) - strlen( szFileName ), " %04d-%02d-%02d %02d-%02d-%02d.txt", 
		stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay,
		stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond );

	if( m_bWriteFile == true )
	{
		m_hReportFile = CreateNewWriteFile( szFileName );
		if( m_hReportFile == INVALID_HANDLE_VALUE )
		{
			SetFilePointer( m_hReportFile, 0, 0, FILE_END );
		}
	}

	WriteExceptionReport( pExceptionInfo );

	if( m_hReportFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_hReportFile );
		m_hReportFile = INVALID_HANDLE_VALUE;
	}


//	if ( m_OldFilter )
//		return m_OldFilter( pExceptionInfo );
//	else
//		return EXCEPTION_CONTINUE_SEARCH;
}

//
// This is called when unhandled exception occurs
//
LONG WINAPI XExceptionHandler::MyUnhandledExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo )
{
	// { 예외 재귀호출 회수 제한
	static int nCnt = 0;
	if ( nCnt++ > 3 ) return EXCEPTION_EXECUTE_HANDLER;
	// }

	g_bUnhandledExceptionRaised = true;

	DoExceptionFilter( pExceptionInfo );

	TerminateProcess( m_hProcess, 999 );

	return EXCEPTION_EXECUTE_HANDLER;
}

//
// Write informations to Report file. Called by UnhandledExceptionFilter
//
void XExceptionHandler::WriteExceptionReport( PEXCEPTION_POINTERS pExceptionInfo )
{
	// Start out with a banner
	_tprintf( _T( "==============================================================================\r\n\r\n" ) );
	WriteThreadInfo();
	WriteBasicInfo( pExceptionInfo->ExceptionRecord );
	WriteRegistersInfo( pExceptionInfo->ContextRecord );

	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "    Environment dump\r\n" ) );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );

	struct _MyF : XEnvStruct::XEnvFunctor
	{
		void onString( const std::string & strKey, const std::string & strData )
		{
			if( !strKey.empty() && strKey[0] == '_' ) return;
			_tprintf( "%-35s : %s\r\n", strKey.c_str(), strData.c_str() );
		}
	};

	_MyF fo;

	ENV().DoEachData( fo, "*", true );

	_tprintf( _T( "\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "    Application-specific log\r\n" ) );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	_tprintf( _T( "\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	SymSetOptions( SYMOPT_DEFERRED_LOADS );
	// Initialize DbgHelp
	if ( !SymInitialize( GetCurrentProcess(), 0, TRUE ) )
		return;
	CONTEXT trashableContext = *(pExceptionInfo->ContextRecord);
	WriteStackDetails( &trashableContext, FALSE );
	SymCleanup( GetCurrentProcess() );

	_tprintf( _T("\r\n") );
	_tprintf( _T("\r\n") );

	WriteMemoryDump( pExceptionInfo->ContextRecord );

	_tprintf( _T( "==============================================================================\r\n" ) );

	if (__lpUserFunc) {
		__lpUserFunc( m_hReportFile );
	}
}

//
// Write thread infomations
//
void XExceptionHandler::WriteThreadInfo()
{
	if ( m_vThreadInfo.size() )
	{
		_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
		_tprintf( _T( "    Thread Information\r\n" ) );
		_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
		_tprintf( _T( "\r\n" ) );

		int cnt = 0;

		for ( std::vector< XSEH::THREAD_INFO * >::iterator it = m_vThreadInfo.begin(); it != m_vThreadInfo.end(); ++it, ++cnt )
		{
			_tprintf( _T( "%02d Thread name     : %s\r\n" ), cnt, (*it)->thread_name );
			_tprintf( _T( "%02d job info        : %s\r\n" ), cnt, (*it)->job_info );
			_tprintf( _T( "%02d job id          : %d\r\n" ), cnt, (*it)->job_id );
			_tprintf( _T( "%02d counter         : %d\r\n" ), cnt, (*it)->counter );
			_tprintf( _T( "%02d execute time    : %d\r\n" ), cnt, (*it)->last_execute_time );
		}

		_tprintf( _T( "\r\n" ) );
		_tprintf( _T( "\r\n" ) );
	}
}

//
// Write basic informations(user, computer name, error type)
//
void XExceptionHandler::WriteBasicInfo( PEXCEPTION_RECORD pExceptionRecord )
{
	TCHAR szFileName[MAX_PATH] = _T( "" ), szUserName[MAX_PATH] = _T( "" ), szComputerName[MAX_PATH] = _T( "" );
	DWORD dwUserLen = MAX_PATH, dwComputerLen = MAX_PATH;

	SYSTEMTIME stSystemTime;
	GetLocalTime( &stSystemTime );

	GetModuleFileName( (HMODULE) NULL, szFileName, MAX_PATH );
	GetUserName( szUserName, &dwUserLen );
	GetComputerName( szComputerName, &dwComputerLen );

	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	hProcess = GetCurrentProcess();
	GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc) );

	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "    Basic Information\r\n" ) );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	_tprintf( _T( "Program Name : %s\r\n" ), m_szLogPrefixName );
	_tprintf( _T( "EXE          : %s\r\n" ), szFileName );
	_tprintf( _T( "User         : %s\r\n" ), szUserName );
	_tprintf( _T( "Computer     : %s\r\n" ), szComputerName );
	_tprintf( _T( "\r\n" ) );

	// Print information about the type of fault and where the fault occured
	_tprintf( _T( "Program      : %s\r\n" ), m_szModuleName );
	_tprintf( _T( "Exception    : %08X (%s)\r\n"), pExceptionRecord->ExceptionCode, GetExceptionString( pExceptionRecord->ExceptionCode ) );

	char buf[1024];
	s_sprintf( buf, _countof( buf ), "%08X (%s)", pExceptionRecord->ExceptionCode, GetExceptionString( pExceptionRecord->ExceptionCode ) );
	m_strExceptionInfo = buf;

	DWORD section = 0, offset = 0;
	m_bIsValidFaultAddress = GetLogicalAddress( pExceptionRecord->ExceptionAddress, m_szModuleName, _countof( m_szModuleName ), section, offset );
	if( m_bIsValidFaultAddress )
	{
		g_pFaultAddress = pExceptionRecord->ExceptionAddress;

#ifdef _WIN64
		_tprintf( _T( "Fault Address: %016I64X %02X:%08X\r\n"), pExceptionRecord->ExceptionAddress, section, offset );
#else
		_tprintf( _T( "Fault Address: %08I64X %02X:%08X\r\n"), pExceptionRecord->ExceptionAddress, section, offset );
		s_sprintf( buf, _countof( buf ), "%08I64X %02X:%08X", pExceptionRecord->ExceptionAddress, section, offset );
#endif
	}
	else
	{
		_tprintf( _T( "Fault Address: Undeterminable(%08I64X %02X:%08X)\r\n"), pExceptionRecord->ExceptionAddress, section, offset );
		s_sprintf( buf, _countof( buf ), "Undeterminable(%08I64X %02X:%08X)", pExceptionRecord->ExceptionAddress, section, offset );
	}

	m_strExceptionAddress = buf;

	_tprintf( _T( "Mem Usage    : %10dKB / %10dKB\r\n"), (int)( pmc.WorkingSetSize >> 10 ), (int)( pmc.PagefileUsage >> 10 ) );
	_tprintf( _T( "\r\n" ) );

	if( m_vAllocCountInfo.empty() == false )
	{
		_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
		_tprintf( _T( "    Allocation Count Info(by _MEM_USAGE_DEBUG)\r\n" ) );
		_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
		_tprintf( _T( "\r\n" ) );

		for( std::vector< ALLOC_COUNT_INFO * >::const_iterator it = m_vAllocCountInfo.begin() ; it != m_vAllocCountInfo.end() ; ++it )
		{
			_tprintf( _T( "Type: %-40s   Allocated: [Current:%8d]/[Peek Concurrent:%8d]/[Total:%8d]\r\n" ), (*it)->type_name, (*it)->current_alloc_count, (*it)->peek_concurrent_alloc_count, (*it)->total_alloc_count );
		}
		_tprintf( _T( "\r\n" ) );
	}

	// TODO: System Information!

	_tprintf( _T( "\r\n" ) );
}

//
// Show the registers
//
void XExceptionHandler::WriteRegistersInfo( PCONTEXT pContext )
{
#ifdef _WIN64
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "    x86 Registers\r\n" ) );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	_tprintf( _T("RAX=%016I64X  RBX=%016I64X  RCX=%016I64X  RDX=%016I64X\r\n"), pContext->Rax, pContext->Rbx, pContext->Rcx, pContext->Rdx );
	_tprintf( _T("ESI=%016I64X  EDI=%016I64X  EBP=%016I64X\r\n"), pContext->Rsi, pContext->Rdi, pContext->Rbp );
	_tprintf( _T("DS =%04X      ES=%04X       FS=%04X       GS:%04X\r\n"), pContext->SegDs, pContext->SegEs, pContext->SegFs, pContext->SegGs );
	_tprintf( _T("CS:RIP=%04X:%016I64X\r\n"), pContext->SegCs, pContext->Rip );
	_tprintf( _T("SS:RSP=%04X:%016I64X\r\n"), pContext->SegSs, pContext->Rsp );
	_tprintf( _T("Flags=%08X\r\n"), pContext->EFlags );
	_tprintf( _T("\r\n") );
	_tprintf( _T("\r\n") );
#else
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "    x86 Registers\r\n" ) );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	_tprintf( _T("EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X\r\n"), pContext->Eax, pContext->Ebx, pContext->Ecx, pContext->Edx );
	_tprintf( _T("ESI=%08X  EDI=%08X  EBP=%08X\r\n"), pContext->Esi, pContext->Edi, pContext->Ebp );
	_tprintf( _T("DS =%04X      ES=%04X       FS=%04X       GS:%04X\r\n"), pContext->SegDs, pContext->SegEs, pContext->SegFs, pContext->SegGs );
	_tprintf( _T("CS:EIP=%04X:%08X\r\n"), pContext->SegCs, pContext->Eip );
	_tprintf( _T("SS:ESP=%04X:%08X\r\n"), pContext->SegSs, pContext->Esp );
	_tprintf( _T("Flags=%08X\r\n"), pContext->EFlags );
	_tprintf( _T("\r\n") );
	_tprintf( _T("\r\n") );
#endif
}

//
// Show the Memory
//
void XExceptionHandler::WriteMemoryDump( PCONTEXT pContext )
{
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "    Memory Dump\r\n" ) );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	if( !m_bIsValidFaultAddress )
	{
		_tprintf( _T( "Fault address may be corrupted. Memory dump is terminated.\r\n\r\n" ) );
		return;
	}

#ifdef _WIN64
	_tprintf( _T( "Code: %d bytes starting at (RIP = %016lX)\r\n" ), 16, pContext->Rip );
	Dump( pContext->Rip, 16, FALSE );
	_tprintf( _T("\r\n") );

	_tprintf( _T( "Stack: %d bytes starting at (RSP = %016lX)\r\n" ), 1024, pContext->Rsp );
	Dump( pContext->Rsp, 1024, TRUE );
	_tprintf( _T("\r\n") );
#else
	_tprintf( _T( "Code: %d bytes starting at (EIP = %08lX)\r\n" ), 16, pContext->Eip );
	Dump( pContext->Eip, 16, FALSE );
	_tprintf( _T("\r\n") );

	_tprintf( _T( "Stack: %d bytes starting at (ESP = %08lX)\r\n" ), 1024, pContext->Esp );
	Dump( pContext->Esp, 1024, TRUE );
	_tprintf( _T("\r\n") );
#endif
	_tprintf( _T("\r\n") );
}

//
// Given an exception code, returns a pointer to a static string with a description of the exception
//
#define EXCEPTION( x ) case EXCEPTION_##x: return _T(#x);

LPTSTR XExceptionHandler::GetExceptionString( DWORD dwCode )
{
	switch ( dwCode ) {
		EXCEPTION( ACCESS_VIOLATION )
		EXCEPTION( DATATYPE_MISALIGNMENT )
		EXCEPTION( BREAKPOINT )
		EXCEPTION( SINGLE_STEP )
		EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
		EXCEPTION( FLT_DENORMAL_OPERAND )
		EXCEPTION( FLT_DIVIDE_BY_ZERO )
		EXCEPTION( FLT_INEXACT_RESULT )
		EXCEPTION( FLT_INVALID_OPERATION )
		EXCEPTION( FLT_OVERFLOW )
		EXCEPTION( FLT_STACK_CHECK )
		EXCEPTION( FLT_UNDERFLOW )
		EXCEPTION( INT_DIVIDE_BY_ZERO )
		EXCEPTION( INT_OVERFLOW )
		EXCEPTION( PRIV_INSTRUCTION )
		EXCEPTION( IN_PAGE_ERROR )
		EXCEPTION( ILLEGAL_INSTRUCTION )
		EXCEPTION( NONCONTINUABLE_EXCEPTION )
		EXCEPTION( STACK_OVERFLOW )
		EXCEPTION( INVALID_DISPOSITION )
		EXCEPTION( GUARD_PAGE )
		EXCEPTION( INVALID_HANDLE )
	}

	// If not one of the "known" exceptions, try to get the string
	// from NTDLL.DLL's message table.

	static TCHAR szBuffer[512] = { 0 };

	FormatMessage(	FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
					GetModuleHandle( _T("NTDLL.DLL") ),
					dwCode, 0, szBuffer, _countof( szBuffer ), 0 );

	return szBuffer;
}

#undef EXCEPTION

//
// Given a linear address, locates the module, section, and offset containing that address.
// Note: the szModule paramater buffer is an output buffer of length specified by the len parameter (in characters!)
//
BOOL XExceptionHandler::GetLogicalAddress( PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset )
{
	MEMORY_BASIC_INFORMATION mbi;

	if ( !VirtualQuery( addr, &mbi, sizeof( mbi ) ) )
		return FALSE;

	PVOID hMod = mbi.AllocationBase;

	if ( !GetModuleFileName( (HMODULE) hMod, szModule, len ) )
		return FALSE;

	// Point to the DOS header in memory
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER) hMod;

	if( !pDosHdr )
		return FALSE;

	// From the DOS header, find the NT (PE) header
	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(((DWORD64) hMod) + pDosHdr->e_lfanew);

	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );

	DWORD64 rva = (DWORD64)addr - (DWORD64)hMod; // RVA is offset from module load address

	// Iterate through the section table, looking for the one that encompasses
	// the linear address.
	for ( unsigned i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++, pSection++ ) {
		DWORD sectionStart = pSection->VirtualAddress;
		DWORD sectionEnd = sectionStart + (std::max)(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

		// Is the address in this section???
		if ( (rva >= sectionStart) && (rva <= sectionEnd) ) {
			// Yes, address is in the section. Calculate section and offset,
			// and store in the "section" & "offset" params, which were
			// passed by reference.
			section = i+1;
			offset = (DWORD) (rva - sectionStart);
			return TRUE;
		}
	}

	return FALSE;	// Should never get here!
}

//
// Walks the stack, and writes the results to the report file 
//
void XExceptionHandler::WriteStackDetails( PCONTEXT pContext, BOOL bWriteVariables ) // TRUE if local/params should be output
{
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "    Call Stack (%s)\r\n" ), bWriteVariables ? "Detail" : "Short" );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );
	DWORD dwMachineType = 0;
	// Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag

	bool bIsTop = true;
	std::string strTemp;

#ifdef _WIN64
	_tprintf( _T("Address           Frame             Function                                  SourceFile\r\n") );
	STACKFRAME64 sf;
	memset( &sf, 0, sizeof(sf) );
	sf.AddrPC.Offset		= pContext->Rip;
	sf.AddrPC.Mode			= AddrModeFlat;
	sf.AddrStack.Offset		= pContext->Rsp;
	sf.AddrStack.Mode		= AddrModeFlat;
	sf.AddrFrame.Offset		= pContext->Rbp;
	sf.AddrFrame.Mode		= AddrModeFlat;

	dwMachineType = IMAGE_FILE_MACHINE_AMD64;
#else
	_tprintf( _T("Address   Frame     Function                        SourceFile\r\n") );
	// Initialize the STACKFRAME structure for the first call. This is only
	// necessary for Intel CPUs, and isn't mentioned in the documentation.
	STACKFRAME sf;
	memset( &sf, 0, sizeof(sf) );
	sf.AddrPC.Offset		= pContext->Eip;
	sf.AddrPC.Mode			= AddrModeFlat;
	sf.AddrStack.Offset		= pContext->Esp;
	sf.AddrStack.Mode		= AddrModeFlat;
	sf.AddrFrame.Offset		= pContext->Ebp;
	sf.AddrFrame.Mode		= AddrModeFlat;

	dwMachineType = IMAGE_FILE_MACHINE_I386;
#endif

	while ( 1 ) {
		// Get the next stack frame
#ifdef _WIN64
		if ( !StackWalk64( dwMachineType, m_hProcess, GetCurrentThread(), &sf, pContext, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0 ) )
#else
		if ( !StackWalk( dwMachineType, m_hProcess, GetCurrentThread(), &sf, pContext, 0, SymFunctionTableAccess, SymGetModuleBase, 0 ) )
#endif
			break;
		if ( sf.AddrFrame.Offset == 0 )	// Basic sanity check to make sure
			break;						// the frame is OK. Bail if not.

		strTemp += m_strReturn;

#ifdef _WIN64
		_tprintf( _T("%016I64X  %016I64X  "), sf.AddrPC.Offset, sf.AddrFrame.Offset );
#else
		_tprintf( _T("%08X  %08X  "), sf.AddrPC.Offset, sf.AddrFrame.Offset );
#endif

		// Get the name of the function for this stack frame entry
		BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = 1024;

		DWORD64 symDisplacement = 0;	// Displacement of the input address,
										// relative to the start of the symbol
		if ( SymFromAddr( m_hProcess,sf.AddrPC.Offset,&symDisplacement,pSymbol ) ) {
			TCHAR szModule[MAX_PATH] = _T("");

			s_sprintf( szModule, _countof( szModule ), _T("%-s+%I64X"), pSymbol->Name, symDisplacement );
			_tprintf( _T( "%-40s" ), szModule );
			strTemp += m_strReturn;
		} else {
			// No symbol found. Print out the logical address instead.
			TCHAR szModule[MAX_PATH] = _T("");
			DWORD section = 0, offset = 0;

			int se = GetLastError();

			m_bIsValidFaultAddress = GetLogicalAddress( (PVOID) (DWORD64) sf.AddrPC.Offset, szModule, _countof( szModule ), section, offset );
			if( m_bIsValidFaultAddress )
				_tprintf( _T("%04X:%08X                   %s (e%d)   "), section, offset, szModule, se );
			else
				_tprintf( _T("Undeterminable(%04X:%08X)   %s (e%d)   "), section, offset, szModule, se );

			strTemp += m_strReturn;
		}

#ifdef _WIN64
		// Get the source line for this stack frame entry
		IMAGEHLP_LINE64 lineInfo = { sizeof(IMAGEHLP_LINE64) };
		DWORD dwLineDisplacement;
		if ( SymGetLineFromAddr64( m_hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo ) ) {
#else
		// Get the source line for this stack frame entry
		IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
		DWORD dwLineDisplacement;
		if ( SymGetLineFromAddr( m_hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo ) ) {
#endif
			_tprintf(_T("  %s line %u"),lineInfo.FileName,lineInfo.LineNumber); 
			strTemp += m_strReturn;
			if ( m_bHasSymbol == FALSE )
				m_bHasSymbol = TRUE;
		}
		_tprintf( _T("\r\n") );

		// Write out the variables, if desired
		if ( bWriteVariables ) {
			// Use SymSetContext to get just the locals/params for this frame
			IMAGEHLP_STACK_FRAME imagehlpStackFrame;

			imagehlpStackFrame.InstructionOffset = sf.AddrPC.Offset;
			SymSetContext( m_hProcess, &imagehlpStackFrame, 0 );

			// Enumerate the locals/parameters
			SymEnumSymbols( m_hProcess, 0, 0, EnumerateSymbolsCallback, &sf );

			_tprintf( _T("\r\n") );
		}

		if( bIsTop ) m_strTopStack = strTemp;
		bIsTop = false;
	}
	_tprintf( _T( "\r\n" ) );
	_tprintf( _T( "\r\n" ) );
}

//
// The function invoked by SymEnumSymbols
//
BOOL CALLBACK XExceptionHandler::EnumerateSymbolsCallback( PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext )
{
	char szBuffer[2048];

	char szNameBuffer[1024];

	__try {
#ifdef _WIN64
		if ( FormatSymbolValue( pSymInfo, (STACKFRAME64*)UserContext, szBuffer, _countof(szBuffer), szNameBuffer, _countof( szNameBuffer ) ) ) {
#else
		if ( FormatSymbolValue( pSymInfo, (STACKFRAME*)UserContext, szBuffer, _countof(szBuffer), szNameBuffer, _countof( szNameBuffer ) ) ) {
#endif

			if ( szNameBuffer[0] == '?' ) return TRUE;
			if ( szNameBuffer[1] != '_' ) return TRUE;
			if ( szNameBuffer[0] == '_' ) return TRUE;

			if ( !strcmp( szNameBuffer, "g_round_expansion" ) ) return TRUE;
			if ( !strcmp( szNameBuffer, "g_pflt" ) ) return TRUE;
			if ( !strcmp( szNameBuffer, "g_magnitude" ) ) return TRUE;
			if ( !strcmp( szNameBuffer, "g_fmt" ) ) return TRUE;

			_tprintf( _T("\t%s\r\n"), szBuffer );
		}
	}
	__except( 1 ) {
		_tprintf( _T("punting on symbol %s\r\n"), pSymInfo->Name );
	}
	return TRUE;
}

//
// Given a SYMBOL_INFO representing a particular variable, displays its contents.
// If it's a user defined type, display the members and their values.
//
BOOL XExceptionHandler::FormatSymbolValue( PSYMBOL_INFO pSym, STACKFRAME * sf, char * pszBuffer, unsigned cchBuffer, char * pszNameBuffer, unsigned cchNameBuffer )
{
	char * pszCurrBuffer = pszBuffer;

	// Indicate if the variable is a local or parameter
	if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
	{
		pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, "Parameter " );
		cchBuffer = (unsigned int)(pszCurrBuffer-pszBuffer)/sizeof( *pszCurrBuffer );
	}
	else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
	{
		pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, "Local " );
		cchBuffer = (unsigned int)(pszCurrBuffer-pszBuffer)/sizeof( *pszCurrBuffer );
	}

	// If it's a function, don't do anything.
	if ( pSym->Tag == 5 )	// SymTagFunction from CVCONST.H from the DIA SDK
		return FALSE;

	s_strcpy( pszNameBuffer, cchNameBuffer, pSym->Name );

	// Emit the variable name
	pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, "\'%s\'", pSym->Name );
	cchBuffer = (unsigned int)(pszCurrBuffer-pszBuffer)/sizeof( *pszCurrBuffer );

	DWORD_PTR pVariable = 0;	// Will point to the variable's data in memory

	if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE ) {
		// if ( pSym->Register == 8 )	// EBP is the value 8 (in DBGHELP 5.1)
		{								// This may change!!!
			pVariable = sf->AddrFrame.Offset;
			pVariable += (DWORD_PTR)pSym->Address;
		}
		// else
		// return FALSE;
	}
	else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER ) {
		return FALSE;	// Don't try to report register variable
	} else {
		pVariable = (DWORD_PTR)pSym->Address;	// It must be a global variable
	}

	// Determine if the variable is a user defined type (UDT). IF so, bHandled
	// will return TRUE.
	BOOL bHandled;
	pszCurrBuffer = DumpTypeIndex(pszCurrBuffer, cchBuffer, pSym->ModBase, pSym->TypeIndex, 0, pVariable, bHandled );
	cchBuffer = (unsigned int)(pszCurrBuffer-pszBuffer)/sizeof( *pszCurrBuffer );
	if ( !bHandled ) {
		// The symbol wasn't a UDT, so do basic, stupid formatting of the
		// variable. Based on the size, we're assuming it's a char, WORD, or
		// DWORD.
		BasicType basicType = GetBasicType( pSym->TypeIndex, pSym->ModBase );
		pszCurrBuffer = FormatOutputValue(pszCurrBuffer, cchBuffer, basicType, pSym->Size, (PVOID) pVariable ); 
		cchBuffer = (unsigned int)(pszCurrBuffer-pszBuffer)/sizeof( *pszCurrBuffer );
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// If it's a user defined type (UDT), recurse through its members until we're
// at fundamental types. When he hit fundamental types, return
// bHandled = FALSE, so that FormatSymbolValue() will format them.
//////////////////////////////////////////////////////////////////////////////
char *XExceptionHandler::DumpTypeIndex( char *pszCurrBuffer, unsigned cchBuffer, DWORD64 modBase, DWORD dwTypeIndex, unsigned nestingLevel, DWORD_PTR offset, BOOL & bHandled )
{
	char* szSrcBuffer = pszCurrBuffer;
	bHandled = FALSE;

	// Get the name of the symbol. This will either be a Type name (if a UDT), or the structure member name.
	WCHAR * pwszTypeName;
	if ( SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_SYMNAME, &pwszTypeName ) ) {
		pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, " %ls", pwszTypeName );
		cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );

		LocalFree( pwszTypeName );
	}

	// Determine how many children this type has.
	DWORD dwChildrenCount = 0;
	SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_CHILDRENCOUNT, &dwChildrenCount );

	if ( !dwChildrenCount )	 // If no children, we're done
		return pszCurrBuffer;

	// Prepare to get an array of "TypeIds", representing each of the children.
	// SymGetTypeInfo(TI_FINDCHILDREN) expects more memory than just a
	// TI_FINDCHILDREN_PARAMS struct has. Use derivation to accomplish this.
	struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS {
		ULONG MoreChildIds[1024];
		FINDCHILDREN()
		{
			Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);
			::memset( MoreChildIds, 0, sizeof( MoreChildIds ) );
		}
	} children;

	children.Count = dwChildrenCount;
	children.Start= 0;
	// Get the array of TypeIds, one for each child type
	if ( !SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_FINDCHILDREN, &children ) ) {
		return pszCurrBuffer;
	}

	// Append a line feed
	pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, "\r\n" );
	cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );

	// Iterate through each of the children
	for ( unsigned i = 0; i < dwChildrenCount; i++ ) {
		// Add appropriate indentation level (since this routine is recursive)
		for ( unsigned j = 0; j <= nestingLevel+1; j++ )
		{
			pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, "\t" );
			cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
		}

		// Recurse for each of the child types
		BOOL bHandled2;
		pszCurrBuffer = DumpTypeIndex( pszCurrBuffer, cchBuffer, modBase, children.ChildId[i], nestingLevel+1, offset, bHandled2 );

		// If the child wasn't a UDT, format it appropriately
		if ( !bHandled2 ) {
			// Get the offset of the child member, relative to its parent
			DWORD dwMemberOffset;
			SymGetTypeInfo( m_hProcess, modBase, children.ChildId[i], TI_GET_OFFSET, &dwMemberOffset );

			// Get the real "TypeId" of the child. We need this for the SymGetTypeInfo( TI_GET_TYPEID ) call below.
			DWORD typeId;
			SymGetTypeInfo( m_hProcess, modBase, children.ChildId[i], TI_GET_TYPEID, &typeId );

			// Get the size of the child member
			ULONG64 length;
			SymGetTypeInfo(m_hProcess, modBase, typeId, TI_GET_LENGTH,&length);

			// Calculate the address of the member
			DWORD_PTR dwFinalOffset = offset + dwMemberOffset;

			BasicType basicType = GetBasicType(children.ChildId[i], modBase );

			pszCurrBuffer = FormatOutputValue( pszCurrBuffer, cchBuffer, basicType, length, (PVOID)dwFinalOffset );
			cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );

			pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, "\r\n" );
			cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
		}
	}

	bHandled = TRUE;
	return pszCurrBuffer;
}

char *XExceptionHandler::FormatOutputValue( char *pszCurrBuffer, unsigned cchBuffer, BasicType basicType, DWORD64 length, PVOID pAddress )
{
	char* szSrcBuffer = pszCurrBuffer;
	// Format appropriately (assuming it's a 1, 2, or 4 bytes (!!!)
	if ( length == 1 )
	{
		pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, " = %X", *(PBYTE)pAddress );
		cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
	}
	else if ( length == 2 )
	{
		pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, " = %X", *(PWORD)pAddress );
		cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
	}
	else if ( length == 4 ) {
		if ( basicType == btFloat ) {
			pszCurrBuffer += s_sprintf(pszCurrBuffer, cchBuffer, " = %f", *(PFLOAT)pAddress);
			cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
		} else if ( basicType == btChar ) {
			if ( !IsBadStringPtr( *(PSTR*)pAddress, 32) ) {
				pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, " = \"%.31s\"", *(PDWORD)pAddress );
				cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
			} else
			{
				pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, " = %X", *(PDWORD)pAddress );
				cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
			}
		}
		else
		{
			pszCurrBuffer += s_sprintf(pszCurrBuffer, cchBuffer, " = %X", *(PDWORD)pAddress);
			cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
		}
	} else if ( length == 8 ) {
		if ( basicType == btFloat )
		{
			pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, " = %lf", *(double *)pAddress );
			cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
		}
		else
		{
			pszCurrBuffer += s_sprintf( pszCurrBuffer, cchBuffer, " = %I64X", *(DWORD64*)pAddress );
			cchBuffer = (unsigned int)(pszCurrBuffer-szSrcBuffer)/sizeof( *pszCurrBuffer );
		}
	}
	return pszCurrBuffer;
}

BasicType XExceptionHandler::GetBasicType( DWORD typeIndex, DWORD64 modBase )
{
	BasicType basicType;

	if ( SymGetTypeInfo( m_hProcess, modBase, typeIndex, TI_GET_BASETYPE, &basicType ) ) {
		return basicType;
	}
	// Get the real "TypeId" of the child. We need this for the
	// SymGetTypeInfo( TI_GET_TYPEID ) call below.
	DWORD typeId;
	if (SymGetTypeInfo(m_hProcess,modBase, typeIndex, TI_GET_TYPEID, &typeId)) {
		if ( SymGetTypeInfo( m_hProcess, modBase, typeId, TI_GET_BASETYPE, &basicType ) ) {
			return basicType;
		}
	}
	return btNoType;
}

#define BYTES_PER_LINE		16

//
// Memory Dump function
//
void XExceptionHandler::Dump( DWORD64 dw64Offset, DWORD dwSize, BOOL bAlign )
{
	if( !dw64Offset )
	{
		_tprintf( _T( "XExceptionHandler::Dump: Unable to dump code segment. The starting address to dump is NULL.\r\n" ) );
		return;
	}

	DWORD dwLoc, dwILoc, dwX;
	LPBYTE pOut = (LPBYTE) dw64Offset;
	
	if ( bAlign == TRUE )
		pOut = (LPBYTE) ((dw64Offset >> 4) << 4);
	for ( dwLoc = 0; dwLoc < dwSize; dwLoc += 16, pOut += BYTES_PER_LINE ) {
		LPBYTE pLine = pOut;

		_tprintf( _T( "%08lX: " ), (DWORD64) pOut );
		for ( dwX = 0, dwILoc = dwLoc; dwX < BYTES_PER_LINE; dwX++ ) {
			if ( dwX == (BYTES_PER_LINE / 2) )
				_tprintf( _T( " " ) );
			if ( dwILoc++ > dwSize ) {
				_tprintf( _T( "?? " ) );
			} else {
				_tprintf( _T( "%02X " ), *(pLine++) );
			}
		}
		pLine = pOut;
		_tprintf( " " );
		for ( dwX = 0, dwILoc = dwLoc; dwX < BYTES_PER_LINE; dwX++ ) {
			if ( dwILoc++ > dwSize ) {
				_tprintf( _T( " " ) );
			} else {
				_tprintf( _T( "%c" ), isprint( *pLine ) ? *pLine : '.');
				pLine++;
			}
		}
		_tprintf( "\r\n" );
	}
}

//
// Helper function that writes to the report file, and allows the user to use printf style formating
//
int __cdecl XExceptionHandler::_tprintf( const TCHAR * format, ... )
{
	TCHAR szBuff[2048];
	int retValue;
	DWORD cbWritten;
	va_list argptr;

	va_start( argptr, format );
	retValue = s_vsprintf( szBuff, _countof( szBuff ), format, argptr );
	va_end( argptr );

	m_strExceptionDetail += szBuff;
	m_strReturn = szBuff;

	if( m_hReportFile != INVALID_HANDLE_VALUE )
	{
		WriteFile( m_hReportFile, szBuff, retValue * sizeof( TCHAR ), &cbWritten, 0 );
	}

	return retValue;
}

}; // namespace XSEH