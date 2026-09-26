
#pragma once


#include <string>
#include <list>


class	XHttpRequestParam
{
public:

	XHttpRequestParam();
	~XHttpRequestParam();

	void			Add( const std::wstring& strName, int nValue );
	void			Add( const std::wstring& strName, const std::wstring& strValue );
	void			Add( const std::wstring& strName, time_t nLocalTime );

	std::wstring	Build()	const;

private:

	struct	ParamSet
	{
		ParamSet( const std::wstring& n, const std::wstring& v )
			: strName( n )
			, strValue( v )
		{
		}

		std::wstring	strName;
		std::wstring	strValue;
	};

	std::list< ParamSet >	m_listParam;

};
