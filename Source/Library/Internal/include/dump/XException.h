
#pragma once

#include <exception>
#include <string>
#include <cassert>

#include "../toolkit/safe_function.h"

struct XException : public std::exception
{
	XException()
	{
		s_unExceptionCount++;
	}

	XException( int nErr )
	{
		s_unExceptionCount++;
		char buf[64] = "";
		s_itoa( nErr, buf, _countof( buf ), 10 );
		m_strString = buf;

		assert( raiseDisplayBox() );
	}

	XException( const char * szString )
		: m_strString( szString )
	{
		s_unExceptionCount++;

		assert( raiseDisplayBox() );
	}

	XException( const std::string& strString )
		: m_strString( strString )
	{
		s_unExceptionCount++;

		assert( raiseDisplayBox() );
	}

	const char * what() const
	{
		return m_strString.c_str();
	}

	static unsigned GetExceptionCount()
	{
		return s_unExceptionCount;
	}
	
	XException & DisplayDialogBox();

private:

	bool raiseDisplayBox()
	{
		DisplayDialogBox();

		return false;
	}

	static unsigned s_unExceptionCount;
	std::string m_strString;
};

struct XError
{
	// XSetLastError 는 포인터만 저장하므로 error 는 언제나 유효해야함~!
	// DATA 영역의 것을 사용할것. 스택이나 free 된 힙은 절대 사용 금지!!
	static void	XSetLastError( const char * error );
	static void	XSafeSetLastError( const char * error );
	static const char* XGetLastError();
	static void Debug();
};

extern __declspec( thread ) int  g_nDebugInt[5];

extern void _set_oprint_mask( const char *szMask );
extern void _performance_print( const char *str, ... );
extern void _oprint( const char *str, ... );

