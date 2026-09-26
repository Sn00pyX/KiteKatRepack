
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include <process.h>

#include <string>
#include <list>

#include "../../include/framework/ArcadiaFramework.h"
#include "../../include/toolkit/ProcessUsage.h"
#include "../../include/toolkit/XConsole.h"
#include "../../include/toolkit/XStringUtil.h"
#include "../../include/toolkit/XEnv.h"
#include "../../include/toolkit/nsl.h"
#include "../../include/toolkit/XThreadMonitor.h"
#include "../../include/logging/FileLog.h"
#include "../../include/network/IAcceptor.h"
#include "../../include/network/XSyncStreamConnection.h"
#include "../../include/network/XNetworkUtil.h"
#include "../../include/toolkit/XThread.h"
#include "../../include/dump/XExceptionHandler.h"
#include "../../include/cipher/XStrZlibWithSimpleCipherUtil.h"

#pragma comment(lib, "ws2_32.lib")

extern ArcadiaFrameworkIntf*			pArcadiaFrameworkIntf;
extern XConsole &						GetMainConsole();
extern std::list< std::string >			listCommand;

static volatile bool s_bStatUsageUpdate;
static volatile int  s_nCPULoad;
static volatile int  s_nKernelLoad;
static volatile int  s_nUserLoad;
static volatile int  s_nCurrentMem;
static volatile int  s_nPeakMem;
static volatile int  s_nCurrentPagedMem;
static volatile int  s_nPeakPagedMem;

SYSTEMTIME			 g_startTime;

static IAcceptor*				s_pAcceptor;
static std::vector< XSocket >	s_vSocketList;
static XCriticalSection			s_connectionLock( "ArcadiaConnectionLock" );

unsigned __stdcall RemoteChannelProc( void * pArg );
unsigned __stdcall RemoteChannelAcceptor( void * pArg );
unsigned __stdcall StatUpdater( void * pArg );

struct _SocketWriter : ArcadiaCommandResultReceiver
{
	bool onWrite( const char * szString, ... )
	{
		char p[2048];
		va_list ap;
		va_start(ap, szString);
		s_vsprintf(p, _countof(p), szString, ap);
		va_end(ap);
		std::string result = p;
		XStringUtil::Replace( result, "\n", "\r\n" );
		if( pConnection->Write( result.c_str(), (unsigned)result.size() ) <= 0 ) return false;

		return true;
	}

	bool onEnd()
	{
		if( pConnection->Write( "\0", 1 ) <= 0 ) return false;
		return true;
	}

	XSyncStreamConnection *pConnection;
};

struct TelnetConnection : XSyncStreamConnection
{
	enum TelnetNegotiationCode
	{
		ECHO = 1,		HALF = 3,		STATUS = 5,		TIMING_MARK = 6,		TERMINAL_TYPE = 24,		WINDOW_SIZE = 31,		SPEED = 32,		FLOW_CONTROL = 33,		LINEMODE = 34,		ENV_VAR = 36,		SE = 240,		NOP = 241,		DM = 242,		BRK = 243,		IP = 244,		AO = 245,		AYT = 246,		EC = 247,		EL = 248,		GA = 249,		SB = 250,		WILL = 251,		WONT = 252,		DO = 253,		DONT = 254,		IAC = 255
	};

	TelnetConnection()
	{
		m_bPasswordMode = true;
		m_bLineFeed = m_bUseEcho = m_bLineMode = m_bUseDelimiter = false;
		m_cDelimiter = 0;
		m_nPasswordTryCount = 0;
	}

	TelnetConnection( XSocket sock ) : XSyncStreamConnection( sock )
	{
		m_bPasswordMode = true;
		m_bLineFeed = m_bUseEcho = m_bLineMode = m_bUseDelimiter = false;
		m_cDelimiter = 0;
		m_nPasswordTryCount = 0;
		m_bIsConnected = true;
	}

	virtual ~TelnetConnection()
	{
		if( strBuffer.empty() == false )
		{
			const XAddr& peerAddr = GetPeerAddress();
			_cprint( "%s console remain string: %s\n", peerAddr.GetAddr(), strBuffer.c_str() );
			_cprint( "%s console remain string: %s", peerAddr.GetAddr(), strBuffer.c_str() );
		}
	}

	void SetPeerAddr( const XAddr& peerAddr )
	{
		m_peerAddr = peerAddr;
	}

	bool onRead( char * pData, int nLen, ArcadiaCommandResultReceiver & receiver );
	bool procCommand( char * pData, int nLen, ArcadiaCommandResultReceiver & receiver );
	void TelnetNegotiationAck( unsigned char * cCode, int nLen );
	void TelnetNegotiation( ArcadiaCommandResultReceiver & receiver );

private:

	int m_nPasswordTryCount;
	std::string strBuffer;
	std::vector<char> vBuffer;
	bool m_bPasswordMode;
	bool m_bLineFeed;
	bool m_bUseEcho;
	bool m_bLineMode;
	bool m_bUseDelimiter;
	char m_cDelimiter;
};

static std::string & getUpTime()
{
	static std::string uptime;

	SYSTEMTIME stm;
	GetLocalTime( &stm );

	unsigned __int64 tm1, tm2;
	SystemTimeToFileTime( &g_startTime,	(LPFILETIME)&tm1 );
	SystemTimeToFileTime( &stm,			(LPFILETIME)&tm2 );

	tm2 -= tm1;

	FileTimeToSystemTime( (LPFILETIME)&tm2, &stm );

	stm.wYear -= 1601;
	stm.wMonth--;
	stm.wDayOfWeek--;
	stm.wDay--;

	std::string strTmp;
	char szTmp[64];
	if( stm.wMonth ) { s_sprintf( szTmp, _countof( szTmp ), "%d Month  ", stm.wMonth ); strTmp += szTmp; }
	if( stm.wDay ) { s_sprintf( szTmp, _countof( szTmp ), "%d Day  ", stm.wDay ); strTmp += szTmp; }
	if( stm.wHour ) { s_sprintf( szTmp, _countof( szTmp ), "%d Hour  ", stm.wHour ); strTmp += szTmp; }
	if( stm.wMinute ) { s_sprintf( szTmp, _countof( szTmp ), "%d Minute  ", stm.wMinute ); strTmp += szTmp; }
	s_sprintf( szTmp, _countof( szTmp ), "%d Second", stm.wSecond); strTmp += szTmp;

	uptime = strTmp;

	return uptime;
}

bool InitArcadiaCommandProc()
{	
	if( !XNetworkUtil::InitNetwork() ) return false;

	GetLocalTime( &g_startTime );
	char szBuf[128];
	s_sprintf( szBuf, _countof( szBuf ), "%04d/%02d/%02d-%02d:%02d:%02d", g_startTime.wYear, g_startTime.wMonth, g_startTime.wDay, g_startTime.wHour, g_startTime.wMinute, g_startTime.wSecond );
	ENV().Set( "process.start", szBuf );

	s_pAcceptor = new IAcceptor;

	if( !s_pAcceptor->StartAccept( XAddr( ENV().GetString( "console.ip", "0.0.0.0" ).c_str(), ENV().GetInt( "console.port", 4515 ) ) ) )
	{
		return false;
	}

	unsigned id;
	HANDLE hThread;
	s_bStatUsageUpdate = true;
	hThread = reinterpret_cast<HANDLE>( _beginthreadex( NULL, 0, StatUpdater, NULL, 0, &id ) );
	CloseHandle( hThread );

	hThread = reinterpret_cast<HANDLE>( _beginthreadex( NULL, 0, RemoteChannelAcceptor, NULL, 0, &id ) );
	CloseHandle( hThread );
	
	return true;
}

void DeInitArcadiaCommandProc()
{
	s_pAcceptor->EndAccept();
	delete s_pAcceptor;

	s_bStatUsageUpdate = false;
	while( !s_bStatUsageUpdate ) Sleep( 100 );
}

bool ArcadiaCommandResultReceiver::onWrite( const char * szString, ... )
{
	char szBuf[1024];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	GetMainConsole().Write( szBuf );

	return true;
}

bool ArcadiaCommandResultReceiver::onEnd()
{
	return true;
}

static void onSet( ArcadiaCommandResultReceiver & rcv, std::vector< std::string > & vToken )
{
	if( vToken.size() >= 3 )
	{
		ENV().Set( vToken[1], vToken[2] );
		rcv.onWrite( " SET  %s : %s\n", vToken[1].c_str(), vToken[2].c_str() );
	}
}

static void onGet( ArcadiaCommandResultReceiver & rcv, std::vector< std::string > & vToken )
{
	if( vToken.size() > 1 ) 
	{
		std::string str = ENV().GetString( vToken[1] );
		rcv.onWrite( "%s : %s\n", vToken[1].c_str(), str.c_str() );
	}
}

static void onClear( ArcadiaCommandResultReceiver & rcv, std::vector< std::string > & vToken )
{
	if( vToken.size() > 1 ) 
	{
		if( ENV().Remove( vToken[1] ) )
		{
			rcv.onWrite( "DELETE %s\n", vToken[1].c_str() );
		}
		else
		{
			rcv.onWrite( "CAN'T DELETE %s\n", vToken[1].c_str() );
		}
	}
}

static void onHistory( ArcadiaCommandResultReceiver & rcv, std::vector< std::string > & vToken )
{
	if( listCommand.empty() ) return;

	unsigned cnt = 0;
	for( std::list< std::string >::iterator it = listCommand.begin(); it != listCommand.end(); ++it )
	{
		cnt++;
		rcv.onWrite( "%3d : %s\n", cnt, it->c_str() );
	}

}

static void onDelete( ArcadiaCommandResultReceiver & rcv, std::vector< std::string > & vToken )
{
	if( vToken.size() > 1 ) 
	{
		if( ENV().IsExist( vToken[1] ) && ENV().IsContainer( vToken[1] ) )
		{
			rcv.onWrite( "%s is container! can't delete. (use clear command)\n", vToken[1].c_str() );
		}
		else
		{
			if( ENV().Remove( vToken[1] ) )
			{
				rcv.onWrite( "DELETE %s\n", vToken[1].c_str() );
			}
			else
			{
				rcv.onWrite( "CAN'T DELETE %s\n", vToken[1].c_str() );
			}
		}
	}
}

static void onTerminate( ArcadiaCommandResultReceiver & rcv, std::vector< std::string > & vToken )
{
	bool bForce = false;
	int nSecond = 10;

	if( vToken.size() > 2 ) nSecond = atoi( vToken[2].c_str() );
	else if( vToken.size() > 1 ) nSecond = atoi( vToken[1].c_str() );

	if( vToken.size() > 1 )
	{
		if( _stricmp( vToken[1].c_str(), "-f" ) == 0 ) bForce = true;
	}
	
	for( int i = 0; i < nSecond; ++i )
	{
		Beep( 1000, 100 );
		rcv.onWrite( "\nWARNING : It will be terminated after %d seconds.", nSecond - i );
		Sleep( 1000 );
	}

	rcv.onWrite( "\nTerminating..." );
	if( bForce ) TerminateProcess( GetCurrentProcess(), 1 );
	else  exit(-1);
}

static void onList( ArcadiaCommandResultReceiver & rcv, std::vector< std::string > & vToken )
{
	char *szKey = "*";
	if( vToken.size() > 1 ) szKey = const_cast< char* >( vToken[1].c_str() );

	struct _M : XEnvStruct::XEnvFunctor
	{
		_M( ArcadiaCommandResultReceiver *p )	{ pReceiver = p; nCnt = 0; }

		void onString( const std::string & strKey, const std::string & strData )
		{
			if( !strKey.empty() && strKey[0] == '_' ) return;
			nCnt++;
			pReceiver->onWrite( "%-20s : %s\n", strKey.c_str(), strData.c_str() );
		}

		int nCnt;
		ArcadiaCommandResultReceiver * pReceiver;
	};
	
	_M fo( &rcv );

	ENV().DoEachData( fo, szKey, true );

	rcv.onWrite( "Total %d value.\n", fo.nCnt );
}

static void onWatch( ArcadiaCommandResultReceiver & rcv, std::vector< std::string > & vToken )
{
	if( vToken.size() < 2 ) return;

	if( ArcadiaFrameworkIntf::IsExistWatchList( vToken[1] ) )
	{
		ArcadiaFrameworkIntf::DelWatchList( vToken[1] );
		rcv.onWrite( "remove '%s' from watch list\n", vToken[1].c_str() );
	}
	else
	{
		ArcadiaFrameworkIntf::AddWatchList( vToken[1] );
		rcv.onWrite( "add to watch list '%s'\n", vToken[1].c_str() );
	}
}

static void onMem( ArcadiaCommandResultReceiver & rcv, std::vector< std::string > & vToken )
{
	DWORD processID = GetCurrentProcessId();

	if( vToken.size() > 1 )
	{
		processID = atoi( vToken[1].c_str() );
	}

	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc;

	// Print the process identifier.
	rcv.onWrite( "Process ID                 : %u\n", processID );

	// Print information about the memory usage of the process.

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
									PROCESS_VM_READ,
									FALSE, processID );
	if (NULL == hProcess)
	{
		rcv.onWrite( "Can't open process %d\n", processID );
		return;
	}

	if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
	{
		rcv.onWrite( "PageFaultCount             : %d\n", pmc.PageFaultCount );
		rcv.onWrite( "PeakWorkingSetSize         : %d (%d kB)\n", pmc.PeakWorkingSetSize, pmc.PeakWorkingSetSize>>10 );
		rcv.onWrite( "WorkingSetSize             : %d (%d kB)\n", pmc.WorkingSetSize, pmc.WorkingSetSize>>10 );
		rcv.onWrite( "QuotaPeakPagedPoolUsage    : %d\n", pmc.QuotaPeakPagedPoolUsage );
		rcv.onWrite( "QuotaPagedPoolUsage        : %d\n", pmc.QuotaPagedPoolUsage );
		rcv.onWrite( "QuotaPeakNonPagedPoolUsage : %d\n", pmc.QuotaPeakNonPagedPoolUsage );
		rcv.onWrite( "QuotaNonPagedPoolUsage     : %d\n", pmc.QuotaNonPagedPoolUsage );
		rcv.onWrite( "PagefileUsage              : %d (%d kB)\n", pmc.PagefileUsage, pmc.PagefileUsage>>10 ); 
		rcv.onWrite( "PeakPagefileUsage          : %d (%d kB)\n", pmc.PeakPagefileUsage, pmc.PagefileUsage>>10 );
	}

	CloseHandle( hProcess );
}

void onCwd( ArcadiaCommandResultReceiver & rcv )
{
	char buf[1024];
	GetCurrentDirectory( 1024, buf );
	rcv.onWrite( "Current Working Directory : %s\n", buf );
}

void ArcadiaFrameworkIntf::ProcCommand( const char* szTarget, const char* szFrom, const char *szFullCommand, ArcadiaCommandResultReceiver & rcv )
{
	std::vector< std::string > vToken;
	XStringUtil::Split( szFullCommand, vToken );

	if( FileLogHandler::GetFileLogHandler() ) FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "Command", "%s(%s)\t%s", szTarget, szFrom, szFullCommand );

	if( !vToken.empty() && !pArcadiaFrameworkIntf->onCommand( szTarget, szFrom, szFullCommand, vToken, rcv ) )
	{
		if	   ( vToken[0] == "terminal") ArcadiaFrameworkIntf::SetViewMode( ArcadiaFrameworkIntf::VIEW_CONSOLE );
		else if( vToken[0] == "about"   ) ArcadiaFrameworkIntf::SetViewMode( ArcadiaFrameworkIntf::VIEW_ABOUT );
		else if( vToken[0] == "status"  ) ArcadiaFrameworkIntf::SetViewMode( ArcadiaFrameworkIntf::VIEW_STATUS );
		else if( vToken[0] == "log"     ) ArcadiaFrameworkIntf::SetViewMode( ArcadiaFrameworkIntf::VIEW_LOG );
		else if( vToken[0] == "set"     ) onSet( rcv, vToken );
		else if( vToken[0] == "clear"	) onClear( rcv, vToken );
		else if( vToken[0] == "terminate"	) onTerminate( rcv, vToken );
		else if( vToken[0] == "list"	) onList( rcv, vToken );
		else if( vToken[0] == "history" ) onHistory( rcv, vToken );
		else if( vToken[0] == "mem"		) onMem( rcv, vToken );
		else if( vToken[0] == "get"		||
				 vToken[0] == "show"	) onGet( rcv, vToken );
		else if( vToken[0] == "del"		||
				 vToken[0] == "delete"	||
				 vToken[0] == "remove"	) onDelete( rcv, vToken );
		else if( vToken[0] == "watch"   ) onWatch( rcv, vToken );
		else if( vToken[0] == "getcwd"  ) onCwd( rcv );
		else if( vToken[0] == "suicide"	) XSEH::InvokeUnhandledException( 0xC000000DL );
		else rcv.onWrite( "Invalid command...\n" );
	}

	rcv.onWrite( "> " );
	rcv.onEnd();
}

unsigned __stdcall RemoteChannelProc( void * pArg )
{
	XSetThreadName( -1, "AdminChannel" );

	TelnetConnection *pConnection = reinterpret_cast< TelnetConnection* >( pArg );

	_SocketWriter myWriter;
	myWriter.pConnection = pConnection;

	pConnection->TelnetNegotiation( myWriter );

	myWriter.onWrite( "[%s] remote control session.\nPassword:", ENV().GetString( "app.name", "ARF" ).c_str() );
	myWriter.onEnd();

	char buf[2048];
	while( true )
	{
		ZeroMemory( buf, sizeof(buf) );
		int len = pConnection->Read( buf, sizeof(buf)-1 );

		if( !pConnection->onRead( buf, len, myWriter ) ) break;
	}

	s_connectionLock.Lock();
	for( unsigned i = 0; i < s_vSocketList.size(); i++ )
	{
		if( pConnection->GetXSocket().GetSocketHandle() != s_vSocketList[i].GetSocketHandle() ) continue;
		s_vSocketList.erase( s_vSocketList.begin() + i );
		break;
	}
	s_connectionLock.UnLock();

	const XAddr& peerAddr = pConnection->GetPeerAddress();
	_cprint( "%s console disconnect.\n", peerAddr.GetAddr() );
	FILELOG( "%s console disconnect.", peerAddr.GetAddr() );

	delete pConnection;

	return 0;
}

unsigned __stdcall RemoteChannelAcceptor( void * pArg )
{
	XSetThreadName( -1, "AdminAcceptor" );

	XAddr	peerAddr;
	XSocket mySock;
	HANDLE hThread;
	unsigned id;
	while( true )
	{
		mySock = s_pAcceptor->Accept( peerAddr );

		if( !mySock.IsValidSocket() ) break;

		_cprint( "IP(%s) connected console.\n", peerAddr.GetAddr() );
		FILELOG( "IP(%s) connected console.", peerAddr.GetAddr() );

		bool bIsAllowIP = false;
		std::string strIPMask = ENV().GetString( "console.allow_ip", "*" );
		if( !strIPMask.empty() )
		{
			std::vector< std::string > vToken;
			XStringUtil::Split( strIPMask.c_str(), vToken, ";", false  );

			std::vector< std::string >::iterator it;
			for( it = vToken.begin(); it != vToken.end(); ++it )
			{
				if( nsl::wildcmp( (*it).c_str(), peerAddr.GetAddr() ) )
				{
					bIsAllowIP = true;
					break;
				}
			}
		}

		if( bIsAllowIP == false )
		{
			_cprint( "IP(%s) is not allow console ip.\n", peerAddr.GetAddr() );
			FILELOG( "IP(%s) is not allow console ip.", peerAddr.GetAddr() );

			mySock.Destroy();
			continue;
		}

		TelnetConnection *pConnection = new TelnetConnection( mySock );
		if( pConnection == NULL )
		{
			mySock.Destroy();
			continue;
		}

		pConnection->SetPeerAddr( peerAddr );

		s_connectionLock.Lock();
		s_vSocketList.push_back( mySock );
		s_connectionLock.UnLock();
		
		hThread = reinterpret_cast<HANDLE>( _beginthreadex( NULL, 0, RemoteChannelProc, reinterpret_cast<void*>( pConnection ), 0, &id ) );
		CloseHandle( hThread );
	}

	s_connectionLock.Lock();
	for( unsigned i = 0; i < s_vSocketList.size(); i++ ) s_vSocketList[i].Destroy();
	s_connectionLock.UnLock();

	return 0;
}

void setExecuteFileChecksum()
{
	char exeFileName[ MAX_PATH ] = {0,};
	char szFileTime[256] = {0,};
	unsigned checksum = 0;
	unsigned fs = 0;
	FILETIME fwTime, flTime;
	SYSTEMTIME flsTime;
	HANDLE hFile = INVALID_HANDLE_VALUE;

	if( !GetModuleFileName( NULL, exeFileName, _countof( exeFileName ) ) )
	{
		s_sprintf( szFileTime, _countof( szFileTime ), "Can't retrieve module file name" );
	}
	else if ( ( hFile = CreateFile( exeFileName, GENERIC_READ | READ_CONTROL, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL ) ) == INVALID_HANDLE_VALUE )
	{
		s_sprintf( szFileTime, _countof( szFileTime ), "Can't open file %s", exeFileName );
	} 
	else
	{
		char buf[1024];
		DWORD byte;
		
		while ( true )
		{
			if ( !ReadFile( hFile, buf, 1024, &byte, NULL ) )
			{
				if( GetLastError() == ERROR_HANDLE_EOF ) break;
			}
			if ( !byte ) break;
			fs += byte;
			for ( unsigned i = 0 ; i < byte; i ++ )
			{
				checksum += buf[i];
			}
		}

		if ( !GetFileTime( hFile, NULL, NULL, &fwTime ) )
		{
			s_sprintf( szFileTime, _countof( szFileTime ), "Can't get file time" );
		} 
		else
		{
			FileTimeToLocalFileTime( &fwTime, &flTime );
			FileTimeToSystemTime( &flTime, &flsTime );
			s_sprintf( szFileTime, _countof( szFileTime ), "%02d/%02d/%02d-%02d:%02d:%02d", flsTime.wYear, flsTime.wMonth, flsTime.wDay, flsTime.wHour, flsTime.wMinute, flsTime.wSecond );
		}

		CloseHandle( hFile );
	}

	char szChecksum[ 10 ];
	s_sprintf( szChecksum, _countof( szChecksum ), "%08X", checksum );

	ENV().Set( "app.checksum", szChecksum );
	ENV().Set( "app.filetime", szFileTime );
	ENV().Set( "app.filesize", (int) fs );
}

// 시스템 상태를 세팅하기 위해 계속 수행되는 쓰레드.
// 초당 1회씩 깨어나서 변수들을 업데이트 한다.
unsigned __stdcall StatUpdater( void * pArg )
{
	XSetThreadName( -1, "StatUpdater" );

	// { XEnv 에바인딩 해 놓는다.
	ENV().Bind( "process.load", (int*)&s_nCPULoad );
	ENV().Bind( "process.load_kernel", (int*)&s_nKernelLoad );
	ENV().Bind( "process.load_user", (int*)&s_nUserLoad );
	ENV().Bind( "process.memory", (int*)&s_nCurrentMem );
	ENV().Bind( "process.memory_peak", (int*)&s_nPeakMem );
	ENV().Bind( "process.paged_memory", (int*)&s_nCurrentPagedMem );
	ENV().Bind( "process.paged_memory_peak", (int*)&s_nPeakPagedMem  );
	ENV().Bind( "process.uptime", getUpTime );
	// }

	// PID 세팅
	ENV().Set( "process.id", (int) GetCurrentProcessId() );

	ProcessUsage cUsage;
	cUsage.Open( GetCurrentProcessId() );

	// { 실행파일 체크섬
	setExecuteFileChecksum();
	// }

	while( s_bStatUsageUpdate )
	{
		XThreadMonitor::UpdateThreadCPU();

		ProcessUsage::T_CPU_USAGE_INFO tCPUInfo = cUsage.GetCPU();
		ProcessUsage::T_MEMORY_USAGE_INFO tMemoryUsage = cUsage.GetMemory();

		// 메모리 사용량을 KB 단위로 변환
		s_nCurrentMem = (int)( tMemoryUsage.WorkingSetSize >> 10 );
		s_nPeakMem = (int)( tMemoryUsage.PeakWorkingSetSize >> 10 );
		s_nCurrentPagedMem = (int)( tMemoryUsage.PrivateUsage >> 10 );
		s_nPeakPagedMem = (int)( tMemoryUsage.PeakPrivateUsage >> 10 );

		// CPU 사용량을 % 단위로 변환
		s_nCPULoad		= (int)tCPUInfo.dTotal;
		s_nKernelLoad	= (int)tCPUInfo.dKernel;
		s_nUserLoad		= (int)tCPUInfo.dUser;

		// 만약 화면모드가 서버 상황보기 상태라면 화면 업데이트 메세지 전송
		if( ArcadiaFrameworkIntf::GetViewMode() == ArcadiaFrameworkIntf::VIEW_STATUS ||
			ArcadiaFrameworkIntf::GetViewMode() == ArcadiaFrameworkIntf::VIEW_LOG )
		{
			InvalidateRect( ArcadiaFrameworkIntf::GetViewWindow(), NULL, false );
		}

		// 1초간 Sleep() 했다가 재시도 한다.
		for( int lc = 0; lc< 10 && s_bStatUsageUpdate; ++lc )
		{
			Sleep( 100 );
		}
	}

	cUsage.Close();
	s_bStatUsageUpdate = true;

	return 0;
}


bool TelnetConnection::onRead( char * pData, int nLen, ArcadiaCommandResultReceiver & receiver )
{
	if ( nLen <= 0 )
		return false;

	// 제어 문자 처리
	unsigned char * pControl = (unsigned char *)pData;
	if ( pControl[0] == 255 )
	{
		TelnetNegotiationAck( pControl, nLen );
		return true;
	}

	bool bFlag = true;

	// 에코 처리
	if ( m_bUseEcho )
	{
		if ( m_bPasswordMode )
		{
			for ( int i = 0 ; i < nLen ; i++ )
			{
				if ( pData[i] == '\r' )
				{
					if ( pData[i+1] == '\n' )
					{
						m_bLineFeed = false;
						Write( "\r\n", 2 );
					}
				}
				else if ( pData[i] == '\n' || pData[i] == '\0' )
				{
					continue;
				}
				else
				{
					Write( "*", 1 );
				}
			}
		}
		else
		{
			for ( int i = 0 ; i < nLen ; i++ )
			{
				if ( pData[i] != '\n' && pData[i] != '\r' ) Write( &pData[i], 1 );
			}
		}
	}

	// 수신 메세지 처리
	if ( !m_bUseDelimiter )
	{
		bFlag = procCommand( pData, nLen, receiver );
	}
	else
	{
		for (int i = 0 ; i < nLen ; i++ )
		{
			if ( pData[i] == m_cDelimiter )
			{
				vBuffer.push_back( '\0' );
				bFlag = procCommand( &vBuffer.front(), (int)vBuffer.size(), receiver );
				vBuffer.clear();

				if( !bFlag ) break;

				continue;
			}

			if ( pData[i] == '\n' || pData[i] == '\0' )
				continue;

			vBuffer.push_back( pData[i] );
		}
	}

	return bFlag;
}

void TelnetConnection::TelnetNegotiation( ArcadiaCommandResultReceiver & receiver )
{
	char szBuffer[4];

	// ECHO 여부를 확인한다.
	s_sprintf(szBuffer, _countof( szBuffer ), "%c%c%c", IAC, WILL, ECHO);
	receiver.onWrite( szBuffer );

	// LINEMODE 여부를 확인한다.
	s_sprintf(szBuffer, _countof( szBuffer ), "%c%c%c", IAC, DO, LINEMODE);
	receiver.onWrite( szBuffer );
}

void TelnetConnection::TelnetNegotiationAck( unsigned char * cCode, int nLen )
{
	int nStep = 0;
	unsigned char * pCur = cCode;
	unsigned char cCommand = 0;

	for ( int i = 0; i < nLen; i++, pCur++ )
	{
		if ( *pCur == IAC )
		{
			nStep = 1;
			continue;
		}

		if ( *pCur == DO || *pCur == DONT )
		{
			nStep = 2;
			cCommand = *pCur;
			continue;
		}

		if ( nStep == 2 )
		{
			switch ( *pCur )
			{
			case ECHO:
				if ( cCommand == DO )
				{
					m_bUseEcho = true;
				}
				else
				{
					m_bUseEcho = false;
				}
				break;

			case LINEMODE:
				// 아직 처리 안함.
				if ( cCommand == DO )
				{
					m_bLineMode = true;
				}
				else
				{
					m_bLineMode = false;
				}
				break;

			default:
				break;
			}
			nStep = 0;
		}
	}
}

bool TelnetConnection::procCommand( char * pData, int nLen, ArcadiaCommandResultReceiver & receiver )
{
	bool bFlag = true;

	strBuffer.append( pData, strlen(pData) );
	while( strBuffer.find('\r') != std::string::npos || strBuffer.find('\n') != std::string::npos )
	{
		std::string strCommand;
		strCommand = strBuffer.substr( 0, strBuffer.find('\n') );
		if( strCommand.empty() )
		{
			strCommand = strBuffer.substr( 0, strBuffer.find('\r') );
			strBuffer.erase( 0, strCommand.size() + 1 );
		}
		else strBuffer.erase( 0, strCommand.size() + 1 );

		XStringUtil::Replace( strCommand, "\r", "" );
		XStringUtil::Replace( strCommand, "\n", "" );
		XStringUtil::Trim( strCommand );

		if( strCommand.empty() )
			continue;

		//_SocketWriter myWriter;
		//receiver.pConnection = this;
		receiver.onWrite( "\n" );		

		if( m_bPasswordMode )
		{
			std::string strPassword = "genius";
			std::string strEncrypt = ENV().GetString( "console._password", "" );

			if( pArcadiaFrameworkIntf != NULL )
			{
				const std::string& file_name = pArcadiaFrameworkIntf->GetConsoleEopFileName();
				if( file_name.empty() == false )
				{
					XEnvStruct temp_loader;
					temp_loader.LoadFromFile( file_name.c_str(), "*", true );	// 우선 암호화로 로드 시도
					temp_loader.LoadFromFile( file_name.c_str() );	// 비암호화로 로드 시도

					strEncrypt = temp_loader.GetString( "console._password", "" );
				}
			}

			if( strEncrypt.empty() == false )
			{
				strPassword = XStrZlibWithSimpleCipherUtil::Decrypt( strEncrypt.c_str() );
			}

			if( strPassword == strCommand.c_str() )
			{
				const XAddr& peerAddr = GetPeerAddress();
				_cprint( "%s console logined.\n", peerAddr.GetAddr() );
				FILELOG( "%s console logined.", peerAddr.GetAddr() );

				m_bPasswordMode = false;
				receiver.onWrite( "Welcome.\n> " );
				receiver.onEnd();
			}
			else
			{
				_cprint( "Password(%s) is not console password.\n", strCommand.c_str() );
				FILELOG( "Password(%s) is not console password.", strCommand.c_str() );

				m_nPasswordTryCount++;
				if( m_nPasswordTryCount > 3 ) bFlag = false;
				receiver.onWrite( "Invalid password.\nPassword:" );
				receiver.onEnd();
			}
		}
		else
		{
			if( strCommand == "quit" )
			{
				bFlag = false;
			}
			else
			{
				if( !strCommand.empty() )
				{
					const XAddr& peerAddr = GetPeerAddress();
					ArcadiaFrameworkIntf::ProcCommand( "@system_console", peerAddr.GetAddr(), strCommand.c_str(), receiver );
				}
			}
		}
	}

	return bFlag;
}
