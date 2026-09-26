
#pragma once

#include <cstdio>
#include <vector>

#include "KPackingFileSystem.h"


class	KPackingFileMaker
{
private:

	struct sDataFileInfo
	{
		sDataFileInfo()
			: m_file(NULL)
			, m_size(0)
		{
		}
		~sDataFileInfo()
		{
			close();
		}

		bool	create( const char* fileName )
		{
			assert( !m_file );
			m_file = NULL;
			fopen_s( &m_file, fileName, "wb" );
			assert( (m_file != NULL) && "failed open data file" );

			return (m_file != NULL);
		}

		void	close()
		{
			if( m_file != NULL )
			{
				fclose( m_file );
				m_file = NULL;
			}

			m_size = 0;
		}

		bool	write( const char* buffer, size_t size, size_t* offset )
		{
			size_t oldOffset = m_size;
			if( fwrite( buffer, size, 1, m_file ) != 1 )
			{
				return false;
			}

			m_size += size;
			*offset = oldOffset;
			return true;
		}

		FILE*	m_file;
		size_t	m_size;
	};

public:

	explicit KPackingFileMaker( KPackingFileSystem* pPackingFS );
	~KPackingFileMaker();

	bool	packingFile( const char *szFileName, const char* pBuffer, size_t size, bool bCompress );
	bool	saveIndexFile();

	void	close();

private:

	sDataFileInfo*	getDataFileInfo( size_t hash_key );

private:

	KPackingFileSystem*	m_pPackingFS;

	sDataFileInfo*		m_destDataFileInfo;
	int					m_nDataFileCount;

	std::vector< KPackingFileSystem::KFileInfo >	m_destIndexFileList;

};
