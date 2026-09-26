#pragma once

#include "IFileSystem.h"
#include "../toolkit/khash.h"
#include <map>
#include "../toolkit/ILock.h"


#define KHFILE HANDLE
inline bool IsValidKHFile( KHFILE handle )
{
	return ( handle != NULL && handle != INVALID_HANDLE_VALUE );
}


struct KPackingFileSystem : KFileSystem
{
public:

	struct KFileInfo
	{
		KFileInfo( const char* szName, size_t nFileSize, size_t nPos, unsigned short nIndex )
			: strFileName( szName )
			, m_Size( nFileSize )
			, m_Offset( nPos )
			, m_Index( nIndex )
		{
		}
		KFileInfo()
			: m_Size( 0 )
			, m_Offset( 0 )
			, m_Index( 0 )
		{
		}

		bool operator < ( const KFileInfo & rh ) const
		{
			return m_Offset < rh.m_Offset;
		}

		bool saveFileInfo( KHFILE fp, int *seed ) const;
		bool loadFileInfo( KHFILE fp, int* seed, int nDataFileCount );

		std::string strFileName;
		size_t m_Size;
		size_t m_Offset;
		unsigned short m_Index;
	};

	static size_t			getFileHashKey( const char* szFileName, int nDataFileCount );
	static std::string		getDataFileName( const char* szDataFileName, size_t hask_key );

	KPackingFileSystem( int nDataFileCount = 8, int nThreadCount = 3, bool bIsReadOnly = false );
	virtual ~KPackingFileSystem();

	/*
	 *	KFileSystem : 파일 이름(풀패스 아님)
	 */
	virtual KStream*		open( const char* szFileName, ACCESS_MODE access_mode );

	virtual void			doEachFile( FileHandler & handler );
	virtual bool			isFile( const char *szFileName ) const { return m_hsIndexFileList.has( szFileName ); };

	virtual size_t			getFileSize( const char *szFileName ) const;
	unsigned __int64		getDataFileSize();
	unsigned __int64		getFreeSize() const;
	
	virtual std::string		getFullPathName( const char *szFileName ) const	{ return m_strCurDirectory+szFileName;	};
	virtual std::string		getCurDirectory() const							{ return m_strCurDirectory; };
	virtual bool			changeDirectory( const char *szDirectory );
	virtual bool			makeDirectory( const char *szDirectory );
	virtual bool			deleteDirectory( const char *szDirectory );
	virtual bool			deleteFile( const char *szFile );

	bool					Init( const char* szIndexFileName, const char* szDataFileName );	///< index, data 관리
	bool					DeInit( bool bSaveIndexFile = true );
	bool					Flush( bool bSaveIndexFile );

	bool					SaveIndexFile();		///< index list save, 패킹툴에서 save 명령시 사용하므로 public
	void					DeleteIndexFile();		///< index list save, 패킹툴에서 save 명령시 사용하므로 public

	/// write 모드로 패킹 파일 
	bool					close( class KStream* pStream );
	bool					close( class KStream* pStream, const void* pBuffer, size_t size );

	/// 패킹툴에서 인덱스 리스트(이름) 참조시 사용
	const size_t			GetIndexNameList( std::vector<std::string>& vecNameList );
	size_t					GetDataFileCount() { return m_nDataFileCount; }
	const std::string&		GetIndexFileName() { return m_strIndexFileName; }
	const std::string&		GetDataFileName() { return m_strDataFileName; }

private:

	//T : 이하 내용들은 인터페이스가 아니므로 public 일 필요가 없음.

	friend struct KPackingFile;

	/// open index file for only reading
	bool LoadIndexFile();
	void CalcurateFreeBlock();
	void CalcurateDataFileSize();

	size_t m_nDataFileCount;

	struct KFileHandleMap
	{
		KFileHandleMap( int max_handle_count, int thread_id )
		{
			threadId = thread_id;
			maxHandleCount = max_handle_count;
			handle = new KHFILE[max_handle_count];
			for ( int i = 0; i < max_handle_count; ++i ) handle[i] = NULL;
		}
		~KFileHandleMap();

		int		threadId;
		int		maxHandleCount;
		KHFILE*	handle;

	private:

		KFileHandleMap( const KFileHandleMap& );
		KFileHandleMap& operator=( const KFileHandleMap& );
	};

	// 한개
	KHash< KFileInfo, hashPr_string_nocase >				m_hsIndexFileList;
	KHash< struct KPackingFile*, hashPr_string_nocase >		m_hsIndexWritingFileList;

	bool													m_bIsReadOnly;
	size_t													m_nMaxThreadCount;
//	std::map< int, KFileHandleMap >							m_mapFileHandle;
	std::vector< KFileHandleMap* >							m_vecFileHandle;
	XCriticalSection										m_lckDataFileHandle;
	KHFILE													getDataFileHandle( const char *szFileName );	

	struct KPackingFreeBlockManager*						m_pFreeBlockManager;
	size_t		*m_nDataFileSize;
	std::string m_strIndexFileName;
	std::string m_strDataFileName;
	std::string m_strCurDirectory;
};