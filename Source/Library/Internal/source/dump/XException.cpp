
#ifndef	_WIN32_WINNT
#define	_WIN32_WINNT	0x0501
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>

#include "../../include/dump/XException.h"
#include "../../include/toolkit/nsl.h"
#include "../../include/toolkit/safe_function.h"

XException & XException::DisplayDialogBox()
{
	MessageBox( NULL, m_strString.c_str(), "XException rised!", MB_OK | MB_ICONEXCLAMATION );
	return *this;
}

// thread local storage
__declspec( thread ) int  g_nDebugInt[5];
__declspec( thread ) char * g_XLastError = "Success";
__declspec( thread ) char g_XLastErrorData[256] = "Success";
static std::vector< std::string > s_vDebugMask;

void XError::XSetLastError( const char * error )
{
	g_XLastError = const_cast< char* >( error );
}

void XError::XSafeSetLastError( const char * error )
{
	s_strcpy( g_XLastErrorData, _countof( g_XLastErrorData ), error );
	g_XLastError = g_XLastErrorData;
}

const char* XError::XGetLastError()
{
	return g_XLastError;
}

void XError::Debug()
{

}

void _set_oprint_mask( const char *szMask )
{
	s_vDebugMask.clear();
	nsl::split( szMask, s_vDebugMask, ";" );
}

void _performance_print( const char *str, ... )
{
	char szBuf[1024];
	va_list va;
	va_start( va, str );
	s_vsprintf( szBuf, _countof(szBuf), str, va );
	va_end( va );

	OutputDebugStringA( szBuf );
}

void _oprint( const char *str, ... )
{
	if ( !IsDebuggerPresent() ) return;

	char szBuf[1024];
	va_list va;
	va_start( va, str );
	s_vsprintf( szBuf, _countof(szBuf), str, va );
	va_end( va );

	bool bOutputString = true;

	if ( !s_vDebugMask.empty() )
	{
		bOutputString = false;
		for ( std::vector< std::string >::iterator it = s_vDebugMask.begin(); it != s_vDebugMask.end(); ++it )
		{
			if ( nsl::wildcmp( (*it).c_str(), szBuf, false ) ) 
			{
				bOutputString = true;
				break;
			}
		}
	}

	if ( bOutputString) OutputDebugStringA( szBuf );
}

unsigned XException::s_unExceptionCount;
