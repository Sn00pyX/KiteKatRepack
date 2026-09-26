
#pragma once


#include <cstring>
#include "../toolkit/XStringUtil.h"



inline bool IsMilesFileCheck( const char *szFileName )
{
	//마일즈 사운드
	const char * szMilesList[] = 
	{
		"mssdolby.flt",
		"mssds3d.flt",
		"mssdsp.flt",
		"msseax.flt",
		"msssrs.flt",
		"mssmp3.asi",
		"mssogg.asi",
		"mssvoice.asi",
		NULL
	};

	for( int i = 0; szMilesList[i] != NULL; ++i )
	{
		if( !_stricmp( szMilesList[i], szFileName ) )
			return true;
	}

	return false;
}

inline bool IsHackFileCheck( const char *szFileName )
{
	const char*	szHkShList[] = 
	{
		// HackShield
		"EGRNAP.dll",
		"EGRNAPX2.dll",
		"EhSvc.dll",
		"psapi.dll",
		"v3pro32s.dll",
		"v3warpds.v3d",
		"v3warpns.v3d",
		"3N.mhe",
		"AhnUpCtl.dll",
		"AhnUpGS.dll",
		"AspINet.dll",
		"Bz32Ex.dll",
		"HShield.dat",
		"HSInst.dll",
		"HSUpdate.env",
		"HSUpdate.exe",
		"mspatcha.dll",
		"V3Hunt.dll",
		"V3InetGS.dll",
		NULL
	};

	for( int i = 0; szHkShList[i] != NULL; ++i )
	{
		if( !_stricmp( szHkShList[i], szFileName ) )
			return true;
	}

	return false;
}

inline bool IsGameGuardFileCheck( const char *szFileName )
{
	const char*	szGameGuardList[] = 
	{
		"Rappelz*.ini",
		"GameGuard.des",
		NULL
	};

	for( int i = 0; szGameGuardList[i] != NULL; ++i )
	{
		if( XStringUtil::WildCardCmp( szGameGuardList[i], szFileName, true ) )
			return true;
	}

	return false;
}

inline bool IsRealExeFile( const char *szFileName )
{
	if( strlen( szFileName ) < 4 ) return false;

	if( !_stricmp( &szFileName[ strlen( szFileName ) - 4 ], ".exe" ) ) return true;
	if( !_stricmp( &szFileName[ strlen( szFileName ) - 4 ], ".dll" ) ) return true;

	return false;
}

inline bool IsNativeFile( const char *szFileName )
{
	if( strlen( szFileName ) < 4 ) return false;

	if( IsRealExeFile( szFileName ) ) return true;

	if( IsHackFileCheck( szFileName ) ) return true;
	if( IsGameGuardFileCheck( szFileName ) ) return true;
	if( IsMilesFileCheck( szFileName ) ) return true;

	if( XStringUtil::WildCardCmp( "launchpoint.*", szFileName, true ) == true ) return true;	// 러시아 로그인 모듈
	if( XStringUtil::WildCardCmp( "*_license.txt", szFileName, true ) == true ) return true;	// 라이센스 텍스트 파일

	// 내부 테스트 서버용 PatchNo.txt 파일
	if( !_stricmp( szFileName, "PatchNo.txt" ) ) return true;

	return false;
}

inline bool	IsNotRappelzFile( const char* szFileName )
{
	if( IsHackFileCheck( szFileName ) ) return true;
	if( IsGameGuardFileCheck( szFileName ) ) return true;

	if( XStringUtil::WildCardCmp( "launchpoint.*", szFileName, true ) == true ) return true;	// 러시아 로그인 모듈

	return false;
}

inline bool	IsRawFile( const char* szFileExt )
{
	// 확자자가 mp3 혹은 raw 일경우 XOR 풀어 줘야함
	if( !_stricmp( szFileExt, "mp3" ) || 
		!_stricmp( szFileExt, "ogg" ) || 
		!_stricmp( szFileExt, "raw" ) )
	{
		return true;
	}

	return false;
}

inline bool IsNeedXORFile( const char* szFileExt )
{
	// 확자자가 mp3 혹은 raw 일경우 XOR 풀어 줘야함
	if( IsRawFile( szFileExt ) == true )
	{
		return false;
	}

	// 텍스쳐와 모델, 맵파일은 XOR 풀어놓는다.
	if( !_stricmp( szFileExt, "dds" ) ||
		!_stricmp( szFileExt, "tga" ) ||
		!_stricmp( szFileExt, "naf" ) ||
		!_stricmp( szFileExt, "nx3" ) ||
		!_stricmp( szFileExt, "cob" ) ||
		!_stricmp( szFileExt, "nfm" ) )
	{
		return false;
	}

	return true;
}

inline unsigned int	getChecksum( const void* pBuf, size_t size )
{
	const unsigned char* p = reinterpret_cast< const unsigned char* >( pBuf );
	unsigned int nChecksum = 0;

	for( size_t i = 0; i < size; ++i )
		nChecksum += p[i];

	return nChecksum;
}

inline std::string	getDirectoryName( const std::string& strTargetFileName )
{
	unsigned char cKey = 0;
	for( const char *p = strTargetFileName.c_str(); *p; ++p )
	{
		cKey += (unsigned char)(*p);
	}

	cKey %= 100;

	char buf[128] = { 0, };	
	s_sprintf( buf, _countof( buf ), "%03d", (int)cKey );
	return buf;
}
