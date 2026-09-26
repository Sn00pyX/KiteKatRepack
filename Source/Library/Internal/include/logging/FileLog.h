
#pragma once

// FileLogHandler Declaration
// Last Modified 2004.1.19 by P.Lotos


class FileLogHandler
{
public:

	static bool				Init();
	static bool				DeInit();
	static FileLogHandler*	GetFileLogHandler();

	virtual ~FileLogHandler();

	virtual bool	LogStringEx( const char* szLogDir, const char* szAppName, const char* szLog, ... ) = 0;
	virtual bool	LogString( const char* szLog, ... ) = 0;
	// strftime 함수를 사용해 szFileName을 포매팅 해서 로그 파일 이름을 지정
	virtual bool	LogStringToFileEx( const char* szLogDir, const char* szAppName, const char* szFileName, const char* szLog, ... ) = 0;

protected:

	static FileLogHandler*	_pInstance;

};

#define	FILELOG	FileLogHandler::GetFileLogHandler()->LogString
