
#pragma once


#include <string>


class	XHttpURL
{
public:

	XHttpURL();
	XHttpURL( const std::wstring& strURL );
	XHttpURL( const std::string& strURL );
	XHttpURL( const wchar_t* pURL );
	XHttpURL( const char* pURL );

	bool			Set( const wchar_t* pURL );
	bool			Set( const char* pURL );
	std::wstring	Build()	const;

	const std::wstring&	Scheme()	const;
	bool				IsSecure()	const;

	const std::wstring&	Host()	const;
	unsigned short		Port()	const;

	const std::wstring&	Path()	const;
	const std::wstring&	Extra()	const;

	void	Scheme( const std::wstring& strScheme );
	void	Secure( bool bIsSecure );

	void	Host( const std::wstring& strHost );
	void	Port( unsigned short nPort );

	void	Path( const std::wstring& strPath );
	void	Extra( const std::wstring& strExtra );

private:

	void	Clear();

private:

	std::wstring	m_strScheme;
	bool			m_bIsSecure;

	std::wstring	m_strHost;
	unsigned short	m_nPort;

	std::wstring	m_strPath;
	std::wstring	m_strExtra;

};
