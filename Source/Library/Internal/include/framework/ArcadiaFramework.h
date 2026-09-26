#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <vector>


struct ArcadiaCommandResultReceiver
{
	virtual bool onWrite( const char * szString, ... );
	virtual bool onEnd();
};

extern void onInitArcadiaFramework();

struct ArcadiaFrameworkIntf
{
	static void	 SetFrameworkInterface( struct ArcadiaFrameworkIntf * pIntf );

	virtual bool onInit( HWND hWnd )		{ return true;  }
	virtual bool onDeInit()					{ return true; }

	virtual bool onKeyDown( int nVkCode )	{ return false; }
	virtual bool onMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )		{ return false; }
	virtual bool onCommand( const char* szTarget,
							const char* szFrom,
							const char *szFullCmd,
							std::vector< std::string > & vList,
							struct ArcadiaCommandResultReceiver & receiver )		{ return false; }
	


	// 기본 커맨드 구현 모음
	static void			ProcCommand( const char* szTarget, const char* szFrom, const char *szCommand, struct ArcadiaCommandResultReceiver & receiver );

	// 뷰 관련
	enum   ViewMode		{ VIEW_CONSOLE, VIEW_STATUS, VIEW_LOG, VIEW_ABOUT, VIEW_USER };
	static ViewMode		GetViewMode();
	static void			SetViewMode( ViewMode mode );
	static HWND			GetViewWindow();
	static const RECT &	GetViewRect();

	static void			AddWatchList( const std::string & strEnv );
	static void			DelWatchList( const std::string & strEnv );
	static bool			IsExistWatchList( const std::string & strEnv );

	const std::string&	GetConsoleEopFileName()	const;
	void		SetConsoleEopFileName( const char* filename );

	// 뷰 드로잉 구현
	virtual void onDrawStatus( HDC dc );
	virtual void onDrawUser( HDC dc );
	virtual void onDrawLog( HDC dc );

	// about 뷰 커스터마이즈
	virtual void onAbout( const char*& pOutTitle, const char*& pOutVersion, const char*& pOutCredit1, const char*& pOutCredit2 );

private:

	std::string	m_strEopFileName;
};