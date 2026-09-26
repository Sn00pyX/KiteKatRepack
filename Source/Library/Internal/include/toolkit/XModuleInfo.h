
#pragma once


#define	WIN32_LEAN_AND_MEAN
#include <Windows.h>


class	XModuleInfo
{
public:

	XModuleInfo();

	bool	Load( const TCHAR* szPath );
	bool	Load( HANDLE hProcess );

	const TCHAR*	CompanyName()		const;
	const TCHAR*	ProductName()		const;
	const TCHAR*	FileDescription()	const;

	const WORD*		FileVersion()		const;
	const WORD*		ProductVersion()	const;

private:

	bool	Query( void* pData );
	bool	QueryFixedInfo( void* pData, VS_FIXEDFILEINFO* pFixedInfo );
	bool	QueryStringInfo( void* pData, const TCHAR* szKey, TCHAR* szValue, size_t cchCount );

private:

	WORD	m_wFileVersion[4];
	WORD	m_wProductVersion[4];

	TCHAR	m_szCompanyName[128];
	TCHAR	m_szProductName[128];
	TCHAR	m_szFileDescription[512];

};


