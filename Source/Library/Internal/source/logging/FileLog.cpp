
#include "../../include/logging/FileLog.h"

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <ctime>
#include <cstdio>

#include "../../include/toolkit/safe_function.h"
#include "../../include/toolkit/XEnv.h"
#include "../../include/toolkit/ILock.h"


static const char*	DEFAULT_LOG_DIR		= "Log\\";
static const int	LOG_BUFFER_LEN		= 2048;
static const int	LOG_FILENAME_LEN	= 512;


FileLogHandler*	FileLogHandler::_pInstance = NULL;



class FileLogHandlerImpl : public FileLogHandler
{
public:
	FileLogHandlerImpl();
	virtual ~FileLogHandlerImpl();

	virtual bool LogStringEx( const char* szLogDir, const char* szAppName, const char* szLog, ... );
	virtual bool LogString( const char* szLog, ... );
	virtual bool LogStringToFileEx( const char* szLogDir, const char* szAppName, const char* szFileName, const char* szLog, ... );

protected:
	bool CreateDir( const char * pDir );
	bool _LogString( const char* szLogDir, const char* szAppName, const char* szLog, va_list va );
	bool _LogStringEx( const char* szLogDir, const char* szAppName, const char* szFileName, const char* szLog, va_list va );

protected:
	XCriticalSection m_CS;
};



bool	FileLogHandler::Init()
{
	if( _pInstance )
	{
		return false;
	}

	_pInstance = new FileLogHandlerImpl;
	if( _pInstance == NULL )
	{
		return false;
	}

	return true;
}

bool	FileLogHandler::DeInit()
{
	if( _pInstance )
	{
		delete _pInstance;
		_pInstance = NULL;

		return true;
	}

	return false;
}

FileLogHandler*	FileLogHandler::GetFileLogHandler()
{
	return _pInstance;
}

FileLogHandler::~FileLogHandler()
{
}



FileLogHandlerImpl::FileLogHandlerImpl()
	: m_CS( "FileLogHandlerImpl" )
{
}

FileLogHandlerImpl::~FileLogHandlerImpl()
{
}

bool FileLogHandlerImpl::LogStringToFileEx( const char* szLogDir, const char* szAppName, const char* szFileName, const char* szLog, ... )
{
	va_list va;
	va_start( va , szLog );
	bool bRet = _LogStringEx( szLogDir, szAppName, szFileName, szLog, va );
	va_end( va );

	return bRet;
}

bool FileLogHandlerImpl::LogStringEx( const char* szLogDir, const char* szAppName, const char* szLog, ... )
{
	va_list va;
	va_start( va , szLog );
	bool bRet = _LogString( szLogDir, szAppName, szLog, va );
	va_end( va );

	return bRet;
}

bool FileLogHandlerImpl::CreateDir( const char * pDir )
{
	DWORD dwRs = GetFileAttributes(pDir);
	if( dwRs == -1 )
	{	
		DWORD dwError = GetLastError();
		if( dwError == ERROR_FILE_NOT_FOUND )
		{
			if( !::CreateDirectory(pDir, NULL) )
			{
				return false;
			}
		}
		else if ( dwError == ERROR_PATH_NOT_FOUND )
		{
			char szDirectory[255];
			s_strcpy( szDirectory, _countof( szDirectory ), pDir );

			int i = static_cast< int >( strlen( pDir ) - 1 );

			while( i >= 0 && ( pDir[i] == '\\' || pDir [i] == '/' ) )
				i--;

			if( i < 0 )
				return false;
			
			while( i > 0 && ( pDir[i] != '\\' && pDir [i] != '/' ) )
				i--;

			if( i < 1 )
				return false;

			szDirectory[i] = 0;

			if( !CreateDir( szDirectory ) )
			{
				return false;
			}

			if( !::CreateDirectory(pDir, NULL) )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else if( !(dwRs & FILE_ATTRIBUTE_DIRECTORY) )
	{
		return false;
	}

	return true;

}

bool FileLogHandlerImpl::LogString( const char* szLog, ... )
{
	va_list va;
	va_start( va , szLog );
	bool bRet = _LogString( NULL, ENV().GetString( "app.name", "Logging" ).c_str(), szLog, va );
	va_end( va );

	return bRet;
}

bool FileLogHandlerImpl::_LogString( const char* szLogDir, const char* szAppName, const char* szLog, va_list va )
{
	THREAD_SYNCRONIZE( &m_CS );

	char szFileName[LOG_FILENAME_LEN];
	char _szLogDir[LOG_FILENAME_LEN];
	char buf[LOG_BUFFER_LEN];

	char* _szApp;

	time_t tt = time( NULL );
	struct tm lt;
	localtime_s( &lt, &tt );

	if( szAppName == NULL )
	{
		_szApp = "Unknown";
	}
	else
	{
		_szApp = const_cast< char* >( szAppName );
	}

	s_sprintf( szFileName, _countof( szFileName ), "%s_%04d-%02d-%02d.log", _szApp, lt.tm_year+1900, lt.tm_mon+1, lt.tm_mday );

	if( szLogDir == NULL )
	{
		s_strcpy( _szLogDir, _countof( _szLogDir ), DEFAULT_LOG_DIR );
	}
	else
	{
		s_strcpy( _szLogDir, _countof( _szLogDir ), szLogDir );

		int nLen = static_cast< int >( strlen( _szLogDir ) );

		if( _szLogDir[nLen - 1] != '/' && _szLogDir[nLen - 1] != '\\' )
		{
			s_strcat( _szLogDir, _countof( _szLogDir ), "/" );
			nLen++;
		}
	}

	if( !CreateDir( _szLogDir ) )
	{
		return false;
	}

	s_strcat( _szLogDir, _countof( _szLogDir ), szFileName );

	FILE *fp = NULL;
	if( fopen_s( &fp, _szLogDir, "a+" ) != 0 ) return false;
	if( !fp ) return false;


	s_sprintf( buf, _countof( buf ), "%04d-%02d-%02d %02d:%02d:%02d ", lt.tm_year+1900, lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec );

	s_vsprintf( &buf[20], _countof(buf) - 21, szLog, va );

	s_strcat( buf, _countof( buf ), "\r\n" );

	int rtn = fputs( buf, fp );

	fclose( fp );

	if( rtn == EOF ) return false;

	return true;
}

bool FileLogHandlerImpl::_LogStringEx( const char* szLogDir, const char* szAppName, const char* szFileName, const char* szLog, va_list va )
{
	THREAD_SYNCRONIZE( &m_CS );

	char _szFileName[LOG_FILENAME_LEN];
	char _szLogDir[LOG_FILENAME_LEN];
	char buf[LOG_BUFFER_LEN];

	char* _szApp;

	time_t tt = time( NULL );
	struct tm lt;
	localtime_s( &lt, &tt );

	if( szAppName == NULL )
	{
		_szApp = "Unknown";
	}
	else
	{
		_szApp = const_cast< char* >( szAppName );
	}

	int nLen = s_sprintf( _szFileName, _countof( _szFileName ), "%s_", _szApp );
	if( nLen <= 0 )
		return false;

	strftime( _szFileName + nLen, _countof( _szFileName ) - nLen, szFileName, &lt );

	if( szLogDir == NULL )
	{
		s_strcpy( _szLogDir, _countof( _szLogDir ), DEFAULT_LOG_DIR );
	}
	else
	{
		s_strcpy( _szLogDir, _countof( _szLogDir ), szLogDir );

		nLen = (int)strlen(_szLogDir);

		if( _szLogDir[nLen - 1] != '/' && _szLogDir[nLen - 1] != '\\' )
		{
			s_strcat( _szLogDir, _countof( _szLogDir ), "/" );
			nLen++;
		}
	}

	if( !CreateDir( _szLogDir ) )
	{
		return false;
	}

	s_strcat( _szLogDir, _countof( _szLogDir ), _szFileName );

	FILE *fp = NULL;
	if( fopen_s( &fp, _szLogDir, "a+" ) != 0 ) return false;
	if( !fp ) return false;

	s_sprintf( buf, _countof( buf ), "%04d-%02d-%02d %02d:%02d:%02d ", lt.tm_year+1900, lt.tm_mon+1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec );

	s_vsprintf( &buf[20], _countof(buf) - 21, szLog, va );

	s_strcat( buf, _countof( buf ), "\r\n" );

	int rtn = fputs( buf, fp );

	fclose( fp );

	if( rtn == EOF ) return false;

	return true;
}
