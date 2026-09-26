#pragma once


template< typename _VECTOR, typename _ITERATOR >
inline void vector_fast_erase( _VECTOR * v, _ITERATOR & it )
{
	if ( it != (v->begin() + v->size() - 1) ) *it = v->back();
	v->pop_back();
}
