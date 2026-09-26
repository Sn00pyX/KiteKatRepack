
#include "ThreadSafeIntMap.h"


ThreadSafeIntMap::ThreadSafeIntMap()
	: m_csMap( "BattleArenaManager::ThreadSafeIntMap::m_csMap" )
{
}

void	ThreadSafeIntMap::Set( int nKey, int nValue )
{
	THREAD_SYNCHRONIZE( m_csMap );

	m_map[ nKey ] = nValue;
}

void	ThreadSafeIntMap::Remove( int nKey )
{
	THREAD_SYNCHRONIZE( m_csMap );

	m_map.erase( nKey );
}

int		ThreadSafeIntMap::Get( int nKey, int nDefault ) const
{
	THREAD_SYNCHRONIZE( m_csMap );

	std::map< int, int >::const_iterator & it = m_map.find( nKey );

	return ( it != m_map.end() ) ? (*it).second : nDefault;
}
