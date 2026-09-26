#pragma once

/**
	@brief	일반 화일 Write, Read 지원\n
			Low Control 가능.
*/

#include <string>
#include <WTypes.h>
#include "SPatchData.h"

/// 상위 File Class
class SFileRoot
{
public:
	enum
	{
		MODE_BIN,
		MODE_TEXT,
	};

	enum
	{
		OPEN_READ,
		OPEN_SAVE,
	};

	SFileRoot( int nOpenMode = OPEN_SAVE );
	SFileRoot( const char * pFileName, int nOpenMode = OPEN_SAVE );
	virtual ~SFileRoot();

	/// Low
	int Write( void * pData, DWORD dwDataSize );
	int Read ( void * pData, DWORD dwDataSize );

	/// High
	void Save( int nMode=MODE_BIN );
	void Load( int nMode=MODE_BIN );

	const char * GetOnlyDirName();
	const char * GetOnlyFileName();

	BOOL IsVaild() { return m_hFileHandle ? TRUE : FALSE; }

	void SetCompressMode( int nCompressMode ) { m_nCompressMode = nCompressMode; }

	int  GetFSize();
	void Close();


protected:
	void Init();

	void SaveBin();	
	void SaveText();

	void LoadBin();
	void LoadText();

	std::string m_strOnlyDir;
	std::string m_strOnlyFileName;
	std::string m_strFileName;
	std::string m_strFileExt;
	int			m_nExtType;
	HANDLE		m_hFileHandle;
	int         m_nOpenMode;

	int			m_nCompressMode;

private:

};
