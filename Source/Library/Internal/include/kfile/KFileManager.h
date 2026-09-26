#pragma once

#include <string>
#include <vector>
#include "../toolkit/ILock.h"
#include "../toolkit/khash.h"

class KFileManager
{
public:
	/** 소스를 추가한다.
		소스는 실제 Win32 디렉토리가 될 수도 있고 패킹된 거대한 하나의 파일이 될 수도 있다.
		리소스 요청시 먼저 추가된 소스순서로 검색된다. */
	void				AddResourceSource( struct KFileSystem *pFileSystem );
	/// 소스를 삭제한다.
	void				RemoveResourceSource( struct KFileSystem *pFileSystem );
	void				ClearResourceSource();

	// 몇몇 라이브러리(쉐이더, 사운드등..) 은 메모리 블럭으로부터 데이터를 읽지 못하고 파일 패스를 요구한다.
	// 이를 위해 CreateTemporaryFileFromResource() 와 GetFullPathNameFromResource() 가 제공된다.

	/// 리소스로부터 임시 파일을 만들어 패스네임을 리턴한다.				for: 쉐이더 소스
	std::string			CreateTemporaryFileFromResource( const char *szResourceName );
	std::string			CreateTemporaryFileFromResourceWithLocale( const char *szResourceName );
	// 2010.06.29 - prodongi
	char* readData(char const* resourceName);

	/// 실제로 파일단위로 나뉘어져 있는 리소스의 패스네임을 리턴한다.	for: wave file, MP3 file
	std::string			GetFullPathNameFromResource( const char *szResourceName );

	// 
	class KStream*		CreateStreamFromResource( const char *szResourceName, bool bCache = false );

	//
	void				RemoveUnnecessaryStream();
	void				RemoveAllStream();

	//
	bool				IsValidResource( const char *szResourceName );

	bool				DeleteTemporaryFile( const char *szResourceName );
	bool				DeleteStream( class KStream* pStream );

	void				EraseStream( std::string* pStreamName );

	static	KFileManager & Instance();

private:

	KFileManager();
	virtual ~KFileManager();

	struct StreamCache
	{
		StreamCache() : pStream( NULL ), dwLastUse( 0 ) {}
		StreamCache( class KStream*	p, DWORD t ) : pStream( p ), dwLastUse( t ) {}

		class KStream*	pStream;
		DWORD			dwLastUse;
	};

	std::vector< struct KFileSystem* >				m_vSourceList;
	XCriticalSection								m_lckIntf;
	XCriticalSection								m_lckCache;
	KHash< StreamCache, hashPr_string_nocase >		m_hsStreamCacheByName;

public:
	/// @brief 2010.04.29 xzip umcompress, 나중에 private으로 빼야됨 - prodongi
	size_t checkXZipBufferAlloc(void* src, size_t srcSize);
	char* m_xzipBuffer;
	size_t m_xzipBufferSize;
};