#pragma once

template< typename INT, int gap >
struct scramble_map
{
	scramble_map()
	{
		int len = sizeof( INT ) * 8;

		for( int i = 0; i < len; ++i )
		{
			map[i] = i;
		}

		int idx = gap;

		for( int i = 0; i < len; ++i )
		{

			char t = map[i];

			while( idx >= len )
			{
				idx -= len;
			}

			map[i] = map[idx];
			map[idx] = t;

			idx += gap + i;
		}
	}

	char & operator[]( unsigned idx )				{ return map[idx]; }
	const char & operator[]( unsigned idx ) const	{ return map[idx]; }

	char map[sizeof(INT) * 8];
};

template< typename INT, int gap >
INT bits_scramble( INT c )
{
	static scramble_map< INT, gap > map;

	INT r = 0;

	INT mask;
	for( int i = 0; i < sizeof(INT)*8; i++ )
	{
		mask = 1 << i;
		if ( c & mask )
			r |= (1 << map[i]) ;
	}

	return r;
}

template< typename INT, int gap >
INT restore_bits_scramble( INT s )
{
	static scramble_map< INT, gap > map;

	INT r = 0;

	INT mask;

	for ( int i = 0; i < sizeof(INT)*8; i++ )
	{
		mask = 1 << map[i];
		if ( s & mask )
			r |= ( 1 << i );
	}

	return r;
}
