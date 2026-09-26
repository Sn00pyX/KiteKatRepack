
#include "../../include/internet/XHttpRequestParam.h"

#include <ctime>
#include <cstdlib>


XHttpRequestParam::XHttpRequestParam()
{
}

XHttpRequestParam::~XHttpRequestParam()
{
}

void	XHttpRequestParam::Add( const std::wstring& strName, int nValue )
{
	wchar_t szValue[64] = { 0, };
	::_itow_s( nValue, szValue, _countof( szValue ), 10 );

	m_listParam.push_back( ParamSet( strName, szValue ) );
}

void	XHttpRequestParam::Add( const std::wstring& strName, const std::wstring& strValue )
{
	m_listParam.push_back( ParamSet( strName, strValue ) );
}

void	XHttpRequestParam::Add( const std::wstring& strName, time_t nLocalTime )
{
	struct tm lt;
	::localtime_s( &lt, &nLocalTime );

	wchar_t szDate[64] = { 0, };
	::swprintf_s( szDate, _countof( szDate ),
		L"%04d-%02d-%02d %2d:%2d:%2d",
		lt.tm_year+1900, lt.tm_mon+1, lt.tm_mday,
		lt.tm_hour, lt.tm_min, lt.tm_sec );

	m_listParam.push_back( ParamSet( strName, szDate ) );
}

std::wstring	XHttpRequestParam::Build()	const
{
	std::wstring strBuild;

	if( m_listParam.empty() == false )
	{
		strBuild += L'?';

		std::list< ParamSet >::const_iterator pos = m_listParam.begin();
		std::list< ParamSet >::const_iterator begin = m_listParam.begin();
		std::list< ParamSet >::const_iterator end = m_listParam.end();

		for( ; pos != end; ++pos )
		{
			if( pos != begin )
			{
				strBuild += L'&';
			}

			const ParamSet& pm = (*pos);
			strBuild += pm.strName + L'=' + pm.strValue;
		}
	}

	return strBuild;
}
