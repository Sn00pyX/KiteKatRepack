
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Windowsx.h>
#include <WinUser.h>
#include <cstdio>

#include "../../include/framework/ArcadiaFramework.h"
#include "../../include/framework/Resource.h"

#include "../../include/toolkit/XGDIHelper.h"
#include "../../include/toolkit/XConsole.h"
#include "../../include/toolkit/XEnv.h"
#include "../../include/dump/XExceptionHandler.h"
#include "../../include/toolkit/XStringUtil.h"
#include "../../include/toolkit/XThread.h"
#include "../../include/toolkit/XRandom.h"
#include "../../include/toolkit/XThreadMonitor.h"


#include <crtdbg.h>

#include <string>
#include <list>
#include <vector>

// APP Global 
HWND								hWnd;
HWND								hEdit;
WNDPROC								EditOldProc = NULL;
HINSTANCE							hInst;
TCHAR								szTitle[16]			= "Arcadia";
TCHAR								szWindowClass[]		= "Arcadia";
bool								bIsWhileCommandSearch;
std::list< std::string >			listCommand;
std::list< std::string >::iterator	listCommandIt;
ArcadiaFrameworkIntf*				pArcadiaFrameworkIntf;
RECT								rectWindow;
ArcadiaFrameworkIntf::ViewMode		viewMode = ArcadiaFrameworkIntf::VIEW_LOG;
static std::vector< std::string >	vStatList;

bool InitArcadiaCommandProc();
void DeInitArcadiaCommandProc();


XConsole & GetMainConsole()
{
	static XConsole	inst( 120, 200 );
	return inst;
}


ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	EditWndProc(HWND, UINT, WPARAM, LPARAM);
void				DrawConsole( HDC dc, XConsole * pConsole );
void				DrawTitle( HDC dc, ArcadiaFrameworkIntf* pIntf );

void				ProcCommand( const char* input_tag, const char* from, const char * buf );
void				ProcCommandUp();
void				ProcCommandDown();

const int			COMMAND_BUF			= 128;	// 최대 저장 가능 커맨드
const int			CONSOLE_FONT_HEIGHT = 14;	// 콘솔 폰트 세로 크기
const int			INPUT_BOX_HEIGHT	= 20;	// 하단의 입력창 세로 크기
const int			CONSOLE_LINE_GAP	= 3;	// 라인간 공백
const char *		CONSOLE_FONT		= "Lucida Console";
const int			LINE_HEIGHT			= CONSOLE_FONT_HEIGHT + CONSOLE_LINE_GAP;



ArcadiaFrameworkIntf::ViewMode ArcadiaFrameworkIntf::GetViewMode()
{
	return viewMode;
}

HWND ArcadiaFrameworkIntf::GetViewWindow()
{
	return hWnd;
}

const RECT & ArcadiaFrameworkIntf::GetViewRect()
{
	return rectWindow;
}

void ArcadiaFrameworkIntf::AddWatchList( const std::string & strEnv )
{
	vStatList.push_back( strEnv );
}

void ArcadiaFrameworkIntf::DelWatchList( const std::string & strEnv )
{
	for( std::vector< std::string >::iterator it = vStatList.begin(); it != vStatList.end(); ++it )
	{
		if( strEnv == *it )
		{
			vStatList.erase( it );
			return;
		}
	}
}

bool ArcadiaFrameworkIntf::IsExistWatchList( const std::string & strEnv )
{
	for( std::vector< std::string >::iterator it = vStatList.begin(); it != vStatList.end(); ++it )
	{
		if( strEnv == *it )
		{
			return true;
		}
	}
	return false;
}

const std::string&	ArcadiaFrameworkIntf::GetConsoleEopFileName()	const
{
	return m_strEopFileName;
}
void	ArcadiaFrameworkIntf::SetConsoleEopFileName( const char* filename )
{
	m_strEopFileName = filename;
	ENV().Remove( "console._password" );
}

void ArcadiaFrameworkIntf::onDrawStatus( HDC dc )
{
	XConsole statConsole;
	
	statConsole.Printf( "app.checksum         : %s        app.filetime : %s\n", ENV().GetString( "app.checksum" ).c_str(), ENV().GetString( "app.filetime" ).c_str() );
	statConsole.Printf( "process.start        : %s\n", ENV().GetString( "process.start" ).c_str() );
	statConsole.Printf( "process.uptime       : %s\n", ENV().GetString( "process.uptime" ).c_str() );
	statConsole.Printf( "process.load         : %3d%%            kernel/user : %3d%% / %3d%%\n", ENV().GetInt( "process.load" ), ENV().GetInt( "process.load_kernel" ),ENV().GetInt( "process.load_user" ) );
	statConsole.Printf( "process.memory       : %dMB / %dMB\n", ENV().GetInt( "process.memory" ) >> 10, ENV().GetInt( "process.memory_peak" ) >> 10 );
	statConsole.Printf( "process.paged_memory : %dMB / %dMB\n", ENV().GetInt( "process.paged_memory" ) >> 10, ENV().GetInt( "process.paged_memory_peak" ) >> 10 );
	statConsole.Printf( "\n" );

	for( std::vector< std::string >::iterator it = vStatList.begin(); it != vStatList.end(); ++it )
	{
		statConsole.Printf( "%-20s : %s\n", (*it).c_str(), ENV().GetString( *it ).c_str() );
	}

	#ifdef _DEV
	{
		THREAD_SYNCRONIZE( &XThreadMonitor::xWatchingThreadListCS );
		for( auto it = XThreadMonitor::vWatchingThreadList.begin() ; it != XThreadMonitor::vWatchingThreadList.end() ; ++it )
		{
			const ThreadUsage::T_THREAD_USAGE_INFO *pInfo = (*it).GetCPUInfo();
			statConsole.Printf( "ThreadCPU-%s : %3.2f%%       kernel/user : %3.2f%% / %3.2f%%\n", (*it).GetThreadName(), pInfo->dTotal, pInfo->dKernel, pInfo->dUser );
		}
	}
	#endif

	DrawConsole( dc, &statConsole );
}

void ArcadiaFrameworkIntf::onDrawUser( HDC dc )
{
	TextOut( dc, 10, 10, "No user screen", 14 );
}

void ArcadiaFrameworkIntf::onDrawLog( HDC dc )
{
	DrawConsole( dc, &XConsole::Inst() );

	//InvalidateRect( GetViewWindow(), NULL, false );
}

void ArcadiaFrameworkIntf::SetViewMode( ArcadiaFrameworkIntf::ViewMode mode )
{
	if( viewMode != mode )
	{
		viewMode = mode;
		InvalidateRect( hWnd, NULL, true );
	}
}

void ArcadiaFrameworkIntf::SetFrameworkInterface( struct ArcadiaFrameworkIntf * pIntf )
{
	pArcadiaFrameworkIntf = pIntf;
}

LRESULT CALLBACK KeyboardProc( WPARAM wParam, LPARAM lParam )
{
	static bool bShift, bCtrl;

	if( HIWORD( lParam ) & KF_UP )
	{
		if( LOWORD( wParam ) == VK_SHIFT   )	{ bShift = false;	return 0L; }
		if( LOWORD( wParam ) == VK_CONTROL )	{ bCtrl  = false;	return 0L; }
	}
	else
	{
		if( LOWORD( wParam ) == VK_SHIFT   )	  bShift = true;
		if( LOWORD( wParam ) == VK_CONTROL )	  bCtrl  = true;	
	}
	
	if( pArcadiaFrameworkIntf->onKeyDown( LOWORD( wParam ) ) )
	{
		return 0;
	}

	bool bEraseFlag = false;

	XConsole * pConsole = &GetMainConsole();

	if( viewMode == ArcadiaFrameworkIntf::VIEW_LOG )
	{
		pConsole = &XConsole::Inst();
	}
	
	switch( LOWORD( wParam ) )
	{
		case VK_HOME:	if( bCtrl ) pConsole->ScreenHome();						break;
		case VK_END :	if( bCtrl ) pConsole->ScreenEnd();						break;
		case VK_UP :	bShift ? pConsole->ScreenUp()   : ProcCommandUp();		break;
		case VK_DOWN:	bShift ? pConsole->ScreenDown() : ProcCommandDown();	break;
		case VK_PRIOR:	pConsole->ScreenPageUp();								break;
		case VK_NEXT:	pConsole->ScreenPageDown();								break;
		
		case VK_F1:		viewMode = ArcadiaFrameworkIntf::VIEW_CONSOLE; bEraseFlag = true;	break;
		case VK_F2:		viewMode = ArcadiaFrameworkIntf::VIEW_STATUS; bEraseFlag = true;	break;
		case VK_F3:		viewMode = ArcadiaFrameworkIntf::VIEW_LOG; bEraseFlag = true;		break;
		case VK_F4:		viewMode = ArcadiaFrameworkIntf::VIEW_USER; bEraseFlag = true;		break;
		case VK_F5:		viewMode = ArcadiaFrameworkIntf::VIEW_ABOUT; bEraseFlag = true;		break;
		default: break;
	}

	InvalidateRect( hWnd, NULL, bEraseFlag );
	
	return 0;
}

void onWmSize( int width, int height )
{	
	static bool bInit = false;

	if( !bInit )
	{
		static XFont hFont( CONSOLE_FONT, CONSOLE_FONT_HEIGHT/2, CONSOLE_FONT_HEIGHT );

		hEdit = CreateWindow( "edit", NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL, 0, height - INPUT_BOX_HEIGHT, width, INPUT_BOX_HEIGHT, hWnd, (HMENU)1, hInst, NULL );
		SendMessage( hEdit, WM_SETFONT, (WPARAM)(HFONT)hFont, 1 );
		ShowWindow( hEdit, SW_SHOW );
		SetFocus( hEdit );

		EditOldProc = (WNDPROC) SetWindowLongPtr( hEdit, GWLP_WNDPROC, (LONG_PTR)EditWndProc );

		bInit = true;
	}

	rectWindow.right = width;
	rectWindow.bottom = height - INPUT_BOX_HEIGHT;

	GetMainConsole().SetScreenHeight( ( height - INPUT_BOX_HEIGHT ) / LINE_HEIGHT );
	XConsole::Inst().SetScreenHeight( ( height - INPUT_BOX_HEIGHT ) / LINE_HEIGHT );

	SetWindowPos( hEdit, NULL, - 2, height - INPUT_BOX_HEIGHT + 3, width + 6,INPUT_BOX_HEIGHT, 0 );
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
#ifdef _DEBUG
	// CRT Debug heap
	_CrtMemDumpAllObjectsSince(0);
#endif

	// SEH 핸들러 설정
	XSEH::StartExceptionHandler( szTitle );

	XSetThreadName( GetCurrentThreadId(), "Main" );
	XInitRandom( ::GetTickCount() );

	// 커맨드라인 아규먼트 세팅
	ENV().ParseCmdLine();

	if( ENV().IsExist( "app.name" ) )
	{
		s_strcpy( szTitle, _countof( szTitle ), ENV().GetString( "app.name" ).c_str() );
	}

	GetMainConsole().Printf( "F1 : terminal screen\n" );
	GetMainConsole().Printf( "F2 : status screen\n" );
	GetMainConsole().Printf( "F3 : log screen\n" );
	GetMainConsole().Printf( "F4 : custom screen\n" );
	GetMainConsole().Printf( "F5 : about screen\n" );
	GetMainConsole().Printf( "> " );

	// 프레임웍 초기화
	struct _ArcadiaFrameworkIntf : ArcadiaFrameworkIntf
	{
	} _defaultIntf;
	pArcadiaFrameworkIntf = &_defaultIntf;
	onInitArcadiaFramework();

	::setlocale( LC_CTYPE, ENV().GetString( "app.crt_locale", "" ).c_str() );
	::_wsetlocale( LC_CTYPE,
		XStringUtil::Multi2Wide(
				ENV().GetString( "app.crt_locale", "" ).c_str(),
				ENV().GetInt( "codepage", CP_ACP )
			).c_str()
		);

	if( !InitArcadiaCommandProc() )
	{
		GetMainConsole().Printf( "ERROR : Can't init command processor\n" );
	}

	// 윈도우 초기화
	MyRegisterClass(hInstance);
	if (!InitInstance (hInstance, nCmdShow)) return FALSE;

	// 프레임웍 초기화
	pArcadiaFrameworkIntf->onInit( hWnd );


	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeInitArcadiaCommandProc();

	XSEH::EndExceptionHandler();

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(wcex); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ARCADIA);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_ARCADIA);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   hWnd = CreateWindow( szWindowClass , szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( pArcadiaFrameworkIntf->onMessage( hWnd, message, wParam, lParam ) ) return 1L;

	switch (message) 
	{
	case WM_ACTIVATE:
		{
			SetFocus( hEdit );
			break;
		}
	case WM_COMMAND:
		{
			char buf[512];
			
			if( lParam != (LPARAM)hEdit ) break;

			if( HIWORD( wParam ) != EN_UPDATE ) break;

			GetWindowText( hEdit, buf, _countof(buf) );
			buf[sizeof(buf)-1] = 0;

			if( strstr( buf, "\r\n" ) != NULL )
			{
				SetWindowText( hEdit, "" );
				
				std::string strCommand = buf;

				XStringUtil::Replace( strCommand, "\r\n", "\n" );
				char t[] = { 10, 0 };
				XStringUtil::Replace( strCommand, t, "" );

				ProcCommand( "@system_window", "admin_input", strCommand.c_str() );

				InvalidateRect( hWnd, NULL, false );
			}
			break;
		}
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			switch( viewMode )
			{
			case ArcadiaFrameworkIntf::VIEW_CONSOLE:	DrawConsole( hdc, &GetMainConsole() );		break;
			case ArcadiaFrameworkIntf::VIEW_ABOUT:		DrawTitle( hdc, pArcadiaFrameworkIntf );	break;
			case ArcadiaFrameworkIntf::VIEW_STATUS:		pArcadiaFrameworkIntf->onDrawStatus( hdc );	break;
			case ArcadiaFrameworkIntf::VIEW_USER:		pArcadiaFrameworkIntf->onDrawUser( hdc );	break;
			case ArcadiaFrameworkIntf::VIEW_LOG:		pArcadiaFrameworkIntf->onDrawLog( hdc );	break;
			}
			
			EndPaint(hWnd, &ps);
			break;
		}
	case WM_CLOSE:
		pArcadiaFrameworkIntf->onDeInit();
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		onWmSize( LOWORD( lParam ), HIWORD( lParam ) );
		break;
	default:
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}

LRESULT CALLBACK	EditWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		{
			KeyboardProc( wParam, lParam );
		}
		break;
	}

	if( EditOldProc != NULL )
	{
		return CallWindowProc( EditOldProc, hWnd, message, wParam, lParam );
	}

	return 0;
}

void DrawConsole( HDC dc, XConsole * pConsole )
{
	struct _Console
	{
		_Console( HDC dc )
		{
			consoleDC = CreateCompatibleDC( dc );
			consoleBitmap = CreateCompatibleBitmap( dc, 1600, 1200 );
			oldObject = SelectObject( consoleDC, consoleBitmap );

			consoleFont = XCreateFont( "굴림체", CONSOLE_FONT_HEIGHT/2, CONSOLE_FONT_HEIGHT );
			consoleBrush = CreateSolidBrush( RGB( 255,255,255) );
			oldFont = SelectFont( consoleDC, consoleFont );
			SetTextColor( consoleDC, RGB( 0, 0, 0 ) );
			SetBkMode( consoleDC, TRANSPARENT );
		}

		~_Console()
		{
			SelectObject( consoleDC, oldObject );
			SelectFont( consoleDC, oldFont );
			DeleteDC( consoleDC );
			DeleteObject( consoleBitmap );
			DeleteObject( consoleBrush );
			DeleteObject( consoleFont );
		}

		void Clear( const RECT & rect )
		{
			FillRect( consoleDC, &rect, consoleBrush );
		}

		HFONT oldFont;
		HFONT consoleFont;
		HDC consoleDC;
		HBITMAP consoleBitmap;
		HBRUSH consoleBrush;

		HGDIOBJ oldObject;
	};

	static _Console _sc( dc );

	RECT rect;
	GetWindowRect( hWnd, &rect );
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	int nY = rect.bottom;
	nY -= ( INPUT_BOX_HEIGHT + 2 );

	RECT scRect;
	scRect.bottom = 1200;
	scRect.top = 0;
	scRect.left = 0;
	scRect.right = 1600;
	_sc.Clear( scRect );

	if( (pConsole->GetCount()+CONSOLE_LINE_GAP) * (LINE_HEIGHT) < nY )
	{
		for( int i = 0; i < pConsole->GetHeight(); i++ )
		{
			TextOut( _sc.consoleDC, 5, 5 + i*LINE_HEIGHT, pConsole->GetScreenLine(i), (int)strlen( pConsole->GetScreenLine(i) ) );
		}
	}
	else
	{
		int cnt = 3;
		for( int i = pConsole->GetCount(); i >= 0; i--, cnt++ )
		{
			TextOut( _sc.consoleDC, 5, nY - cnt * LINE_HEIGHT + CONSOLE_LINE_GAP*2, pConsole->GetScreenLine(i), (int)strlen( pConsole->GetScreenLine(i) ) );
		}
	}

	BitBlt( dc, 0, 0, 1600, 1200, _sc.consoleDC, 0, 0, SRCCOPY );
}

void ArcadiaFrameworkIntf::onAbout( const char*& pOutTitle, const char*& pOutVersion, const char*& pOutCredit1, const char*& pOutCredit2 )
{
	pOutTitle = "Arcadia";
	pOutVersion = "v0.3";
	pOutCredit1 = "Copyright (c) Gala Lab Corp.";
	pOutCredit2 = "All Rights Reserved.";
}

void DrawTitle( HDC dc, ArcadiaFrameworkIntf* pIntf )
{
	static bool bInit;

	struct _Logo
	{
		~_Logo()
		{
			DeleteObject( bufferBitmap );
			DeleteObject( logoBitmap );
			DeleteObject( creditBitmap );
			DeleteDC( creditDC );
			DeleteDC( logoDC );
			DeleteDC( memDC );
		}

		HBITMAP bufferBitmap;
		HBITMAP logoBitmap;
		HBITMAP creditBitmap;
		HDC memDC;
		HDC logoDC;
		HDC creditDC;
	};
	static _Logo _scr;


	if( !bInit )
	{
		_scr.logoDC = CreateCompatibleDC( dc );
		_scr.logoBitmap = LoadBitmap( hInst, MAKEINTRESOURCE( IDB_ARCADIA ) );
		SelectObject( _scr.logoDC, _scr.logoBitmap );

		_scr.memDC = CreateCompatibleDC( dc );
		_scr.bufferBitmap = CreateCompatibleBitmap( dc, 1600, 1200 );
		SelectObject( _scr.memDC, _scr.bufferBitmap );

		_scr.creditDC = CreateCompatibleDC( dc );
		_scr.creditBitmap = CreateCompatibleBitmap( dc, 480, 1800 );
		SelectObject( _scr.creditDC, _scr.creditBitmap );

		BitBlt( _scr.memDC, 70, 30, 479, 186, _scr.logoDC, 0, 0, SRCCOPY );


		SetTextColor( _scr.creditDC, RGB( 230, 230, 230 ) );
		SetBkMode( _scr.creditDC, TRANSPARENT );
		
		const char *pTitle, *pVersion, *pCredit1, *pCredit2;
		pIntf->onAbout( pTitle, pVersion, pCredit1, pCredit2 );

		HGDIOBJ oldFont;
		oldFont = SelectObject( _scr.creditDC, XFont( "Times", 16, 50 ) );
		if ( pTitle )
			TextOut( _scr.creditDC, 10, 0, pTitle, static_cast< int >( strlen(pTitle) ) );
		SelectObject( _scr.creditDC, oldFont );

		oldFont = SelectObject( _scr.creditDC, XFont( "Times", 9, 24 ) );
		if ( pVersion )
			TextOut( _scr.creditDC, 50, 50, pVersion, static_cast< int >( strlen(pVersion) ) );
		SelectObject( _scr.creditDC, oldFont );

		oldFont = SelectObject( _scr.creditDC, XFont( "MS Mincho", 6, 12 ) );
		if ( pCredit1 )
			TextOut( _scr.creditDC, 290, 60, pCredit1, static_cast< int >( strlen(pCredit1) ) );
		if ( pCredit2 )
			TextOut( _scr.creditDC, 350, 75, pCredit2, static_cast< int >( strlen(pCredit2) ) );
		SelectObject( _scr.creditDC, oldFont );

		BitBlt( _scr.memDC, 70, 240, 480, 1800, _scr.creditDC, 0, 0, SRCCOPY );

		bInit = true;
	}

	BitBlt( dc, 0, 0, 1600, 1200, _scr.memDC, 0, 0, SRCCOPY );
}

void ProcCommand( const char* input_tag, const char* from, const char * buf )
{
	static ArcadiaCommandResultReceiver consoleWriter;

	consoleWriter.onWrite( "%s\n", buf );

	// { action history
	bIsWhileCommandSearch = false;
	if( strlen(buf) ) listCommand.push_back( std::string( buf ) );
	if( listCommand.size() > COMMAND_BUF )
	{
		listCommand.pop_front();
	}
	// }

	ArcadiaFrameworkIntf::ProcCommand( input_tag, from, buf, consoleWriter );

	InvalidateRect( hWnd, NULL, false );
}

void ProcCommandUp()
{
	if( listCommand.empty() ) return;
	if( !bIsWhileCommandSearch ) 
	{
		listCommandIt = listCommand.end();
	}

	if( listCommandIt != listCommand.begin() )
	{
		--listCommandIt;
	}
	
	SetWindowText( hEdit, listCommandIt->c_str() );
	SendMessage( hEdit, EM_SETSEL, listCommandIt->size(), listCommandIt->size() );
	bIsWhileCommandSearch = true;
}

void ProcCommandDown()
{
	if( listCommand.empty() ) return;
	if( !bIsWhileCommandSearch )
	{
		listCommandIt = listCommand.end();
		--listCommandIt;
	}

	if( &(*listCommandIt) != &listCommand.back() )
	{
		++listCommandIt;
		bIsWhileCommandSearch = true;
		SetWindowText( hEdit, (*listCommandIt).c_str() );
	}
	else
	{	
		bIsWhileCommandSearch = false;
		SetWindowText( hEdit, "" );
	}

	SendMessage( hEdit, EM_SETSEL, listCommandIt->size(), listCommandIt->size() );
}