#pragma once

#include <vector>
#include <limits>
#include <algorithm>

/*
 GRAPH 인터페이스 : 
		bool   IsReacheable( from, to );
		T	   GetCost( from, to );
*/

template< typename T, typename GRAPH >
struct XDijkstra
{
	XDijkstra()
	{
	}

	T Find( const GRAPH & graph, size_t node_count, size_t start_index, size_t end_index, std::vector< size_t > * pvNodeList )
	{
		T MAX = std::numeric_limits< T >::max();

		m_vDistance.reserve( node_count );
		m_vFlag.reserve( node_count );
		m_vVia.reserve( node_count );

		m_vDistance.erase( m_vDistance.begin(), m_vDistance.end() );
		m_vFlag.erase( m_vFlag.begin(), m_vFlag.end() );
		m_vVia.erase( m_vVia.begin(), m_vVia.end() );

		m_vDistance.assign( node_count, MAX );
		m_vFlag.assign( node_count, 0 );
		m_vVia.assign( node_count, 0 );

		m_vDistance[ start_index ] = 0;

		size_t i, j, k;
		T min;

		for ( i=0; i<node_count; i++ )
		{
			min = MAX;
			for ( j=0; j<node_count; j++ )
			{
				if ( m_vFlag[j] == 0 && m_vDistance[j] < min )
				{
					k = j;
					min = m_vDistance[j];
				}
			}

			m_vFlag[k] = 1;

			if ( min == MAX ) break;

			for ( j=0; j<node_count; j++ )
			{
				if ( !graph.IsReachable( k, j ) ) continue;
				T cost = graph.GetCost( k, j );
				if ( m_vDistance[j] > m_vDistance[k] + cost )
				{
					m_vDistance[j] = m_vDistance[k] + cost;
					m_vVia[j] = static_cast< std::vector< size_t >::value_type >( k );
				}
			}
		}

		k = end_index;
		while ( true )
		{
			pvNodeList->push_back( k );
			if ( k == start_index ) break;
			k = m_vVia[k];
		}
		std::reverse( pvNodeList->begin(), pvNodeList->end() );

		return m_vDistance[ node_count - 1 ];
	}

	std::vector< char >			m_vFlag;
	std::vector< size_t >		m_vVia;
	std::vector< T >			m_vDistance;
};
