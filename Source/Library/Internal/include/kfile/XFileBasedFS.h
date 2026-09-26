#pragma once

#include "IFileSystem.h"
#include <string>


struct XFileBasedFS : KFileSystem
{
	XFileBasedFS();
	virtual ~XFileBasedFS();

	bool Init( const char *szIndexFileName, const char *szDataFileName );
	bool DeInit();

	virtual KStream*		open( const char* szFileName, ACCESS_MODE access_mode );

	virtual void			doEachFile( FileHandler & handler )							{}
	virtual bool			isFile( const char *szFileName ) const						{ return true; };

	virtual size_t			getFileSize( const char *szFileName ) const;

	virtual std::string		getCurDirectory() const										{return "";};
	virtual bool			changeDirectory( const char *szDirectory )					{return false;};
	virtual bool			makeDirectory( const char *szDirectory )					{return false;};
	virtual bool			deleteDirectory( const char *szDirectory )					{return false;};
	virtual bool			deleteFile( const char *szFile )							{ return false; }

private:

	friend struct XFile;

	bool close( struct XFile* pFile );
	bool writeIndexFile();
	bool initIndexFile( const char *szIndexFileName );

	struct XFSInfo *m_pInfo;
};