
#pragma once

#include <map>

#include <toolkit/ILock.h>


class	ThreadSafeIntMap
{
public:

	ThreadSafeIntMap();

	void	Set( int nKey, int nValue );
	void	Remove( int nKey );
	int		Get( int nKey, int nDefault = 0 ) const;

private:

	mutable XCriticalSection	m_csMap;
	std::map< int, int >		m_map;
};
