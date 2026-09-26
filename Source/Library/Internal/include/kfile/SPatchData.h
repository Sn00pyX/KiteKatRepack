
#pragma once

#include <Windows.h>

/// Compress Header
struct SCOMPRESS_HEADER
{
	/// 압축종류
	enum
	{
		COMPRESS_NONE,
		COMPRESS_BZ_LIB,
		COMPRESS_UCL_LIB,
	};

	int     nPatchVer;        ///< 패치 버전
	int     nCompressMode;    ///< 압축 종류
	DWORD   dwSizeOriginal;   ///< 기본 크기
	DWORD   dwSizeCompressed; ///< 압축 크기
	DWORD   dwReserved[4];    ///< 범퍼.
};
