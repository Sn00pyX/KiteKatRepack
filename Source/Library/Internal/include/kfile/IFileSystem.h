#pragma once

#include "KStream.h"
#include <string>


struct KFileSystem
{
	enum	{	MIN_COMPRESS_FILE_SIZE = 5 };/// 압축이 가능한 최소 사이즈

	enum ACCESS_MODE
	{
		READ	= ( 1 << 0 ),
		WRITE	= ( 1 << 1 ),
		CREATE	= ( ( 1 << 2 )  | READ | WRITE ),
		APPEND	= ( ( 1 << 3 )	| READ | WRITE ),

		READ_ONLY	= READ,
		READ_WRITE	= ( READ | WRITE ),
	};

	struct FileHandler
	{
		virtual bool onFile( const char *szFileName ) = 0;
	};

	virtual KStream*		open( const char* szFileName, ACCESS_MODE access_mode )		= 0;

	virtual std::string		getFullPathName( const char *szFileName ) const				{ return ""; }
	virtual void			doEachFile( FileHandler & handler )							= 0;

	virtual size_t			getFileSize( const char *szFileName ) const					= 0;
	virtual bool			isFile( const char *szFileName ) const						= 0;
	virtual bool			isDirectory( const char *szFileName ) const					{ return !isFile( szFileName); }
	virtual std::string		getCurDirectory() const										= 0;
	virtual bool			changeDirectory( const char *szDirectory )					= 0;
	virtual bool			makeDirectory( const char *szDirectory )					= 0;
	virtual bool			deleteDirectory( const char *szDirectory )					= 0;
	virtual bool			deleteFile( const char *szFile )							= 0;
};