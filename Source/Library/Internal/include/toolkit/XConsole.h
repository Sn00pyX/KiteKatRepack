#pragma once

// 가상 콘솔.
// 
// 일정 크기의 버퍼를 가지고 있고 거기다 Printf() 로 찍어댈수 있으며,
// 화면 출력을 위해 ScreenXXX() 계열 함수를 지원한다.
//
//  . _cprint( "blah blah" ); 라고 하면 언제 어디에서나 찍을 수 있다.
//
//  2004/02/14
//	by Testors

#include <cstdio>
#include <cstdarg>

#include "ILock.h"

class XConsole
{
public:
	XConsole( int nWidth = 120, int nHeight = 50 ) : m_Lock( "XConsole" )
	{
		m_nWidth = nWidth;
		m_nScreenHeight = m_nHeight = nHeight;
		bIsFull = false;
		m_nCount = 0;
		m_nNewAdjust = m_nScreenAdjust = 0;

		m_pLines = new char*[nHeight];

		for ( int i = 0; i < nHeight; i++ )
		{
			m_pLines[i] = new char[nWidth+1];
			memset( m_pLines[i], 0, sizeof( char ) * (nWidth+1) );
		}

		m_nX = m_nY = 0;
	}

	~XConsole()
	{
		for ( int i = 0; i < m_nHeight; i++ ) delete [] m_pLines[i];

		delete [] m_pLines;
	}

	void Write( const char *str )
	{
		_Printf( str );
	}

	void Printf( const char *str, ... )
	{
		THREAD_SYNCHRONIZE( &m_Lock );

		char szBuf[2048];
		va_list va;
		va_start( va, str );
		s_vsprintf( szBuf, _countof(szBuf), str, va );
		va_end( va );

		_Printf( szBuf );
	}

	void PrintfWithTimestamp( const char *str, ... )
	{
		THREAD_SYNCHRONIZE( &m_Lock );

		char szBuf[2048];

		SYSTEMTIME st;
		GetLocalTime( &st );

		int nWrittenCount = s_sprintf( szBuf, _countof( szBuf ), "%04d/%02d/%02d %02d:%02d:%02d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

		va_list va;
		va_start( va, str );
		s_vsprintf( szBuf + nWrittenCount, _countof(szBuf) - nWrittenCount, str, va );
		va_end( va );

		_Printf( szBuf );
	}

	const char* GetLine( unsigned line )
	{
		m_Lock.Lock();
		

		if ( line > (unsigned)m_nHeight )
		{
			m_Lock.UnLock();
			return "~";
		}

		int nl = line;

		if ( bIsFull )
		{
			nl = m_nY + line;
			nl %= m_nHeight;
		}

		m_Lock.UnLock();

		return m_pLines[nl];
	}
	
	int GetWidth()	const	{ return m_nWidth; };
	int GetHeight()	const	{ return m_nHeight; };
	
	// 현재 저장중인 라인 수를 리턴
	int GetCount()  const	{ return m_nCount; }

	void SetScreenHeight( unsigned height )	{ m_nScreenHeight = height; };
	int  GetScreenHeight() const			{ return m_nScreenHeight; };
	void ScreenUp()							{ onScreenPos( m_nScreenAdjust - 1 ); }
	void ScreenDown()						{ onScreenPos( m_nScreenAdjust + 1 ); }
	void ScreenPageUp()						{ onScreenPos( m_nScreenAdjust - m_nScreenHeight / 3 ); }
	void ScreenPageDown()					{ onScreenPos( m_nScreenAdjust + m_nScreenHeight / 3 ); }
	void ScreenHome()						{ onScreenPos( 0 - m_nCount + m_nScreenHeight - 1 );	}
	void ScreenEnd()						{ m_nScreenAdjust = 0; }
	const char* GetScreenLine( unsigned l )	{ return GetLine( l + m_nScreenAdjust ); }

	void Clear()
	{
		m_nCount = 0;
		for ( int i = 0; i < m_nHeight; i++ ) m_pLines[i][0] = 0;
		m_nX = m_nY = 0;
		bIsFull = false;
	}

	static XConsole & Inst( const char *szFileName = NULL );
	static XConsole & ErrorInst( const char *szFileName = NULL );

private:

	XConsole( const XConsole& );
	XConsole& operator=( const XConsole& );

private:
	int m_nCount;
	int m_nWidth, m_nHeight;
	int m_nX, m_nY;
	bool bIsFull;
	int m_nScreenHeight;
	int m_nScreenAdjust;
	int m_nNewAdjust;

	void _Printf( const char *szBuf )
	{
		unsigned feed;

		char *p = const_cast< char* >( szBuf );
		char *sp = p;
		for ( ;;*p++)
		{	
			if ( *p == '\n' )
			{	
				feed = putString( sp, (unsigned)(p - sp) );
				
				sp = p;

				addY();
				m_pLines[m_nY][0] = 0;

				if ( feed )
				{
					sp -= feed;
					_Printf( sp );
					return;
				}

				sp++;

				continue;
			}

			if ( !*p )
			{
				if ( p - sp ) 
				{
					feed = putString( sp, (unsigned)(p - sp) );
					if ( feed )
					{
						addY();
						_Printf( sp + feed );
					}
				}
				break;
			}
		}
	}

	unsigned putString( const char *str, unsigned len )
	{
		int y = m_nY;

		unsigned newlen = len;

		if ( m_nX + len >= (unsigned)m_nWidth )
		{
			newlen = m_nWidth - m_nX;
		}

		s_strncpy( &m_pLines[y][m_nX], (m_nWidth-m_nX)+1, str, newlen );

		m_nX += len;

		return len - newlen;
	}

	void addY()
	{
		m_nX = 0;
		if ( ++m_nY == m_nHeight )
		{	
			m_nY = 0;
			bIsFull = true;
		}
		if ( m_nCount < m_nHeight ) ++m_nCount;
	}

	void onScreenPos( int nNewAdjust )
	{
		m_Lock.Lock();

		// 위로 튕기는것
		if ( m_nCount - m_nScreenHeight + nNewAdjust < 0 )
		{
			nNewAdjust = 0 - m_nCount + m_nScreenHeight - 1;
		}

		// 아래로 튕기는것
		if ( nNewAdjust > 0 ) nNewAdjust = 0;

		m_nScreenAdjust = nNewAdjust;

		m_Lock.UnLock();
	}

	XCriticalSection m_Lock;
	char **m_pLines;
};

#define _cprint	XConsole::Inst().Printf
#define _ctprint XConsole::Inst().PrintfWithTimestamp

inline void _lprint( const char *szFileName, const char *str, ... )
{
	FILE *fp = NULL;
	if( fopen_s( &fp, szFileName, "a+" ) != 0 )
	{
		return;
	}

	if ( !fp ) return;

	char szBuf[2048];
	va_list va;
	va_start( va, str );
	s_vsprintf( szBuf, _countof(szBuf), str, va );
	va_end( va );

	// print time 
	SYSTEMTIME st; 
	GetLocalTime( &st ); 
	fprintf( fp,  "%04d/%02d/%02d %02d:%02d:%02d ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond ); 

	fwrite( szBuf, strlen( szBuf ), 1, fp );

	XConsole::Inst().Write( szBuf );

	fclose( fp );
}