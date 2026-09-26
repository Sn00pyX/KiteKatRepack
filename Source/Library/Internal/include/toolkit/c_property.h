#ifndef __C_PROPERTY_V1_21_H__
#define __C_PROPERTY_V1_21_H__

#include <stddef.h>

/*
 *    Delphi / C# style low-level property mapper based on macros
 *
 *            2006.7.1 by Young-Hyun Joo
 *
 *       2006.7.3 : (v1.2)
 */


	// -- internals begin ---------------------------------------------------------------

		template< class T >
		struct _prop_base_t
		{
			typedef T value_t; 
		};

		#define _parent_of_property( name )	((parent_t*)((uintptr_t)this - offsetof( parent_t, name )))

		#define _def_property1d_base( name )														\
			template< typename T >																	\
			const elem_t<T> operator [] ( const T& index ) const									\
			{																						\
				return elem_t<T>( _parent_of_property( name ), index );								\
			}																						\
			template< typename T >																	\
			elem_t<T> operator [] ( const T& index )												\
			{																						\
				return elem_t<T>( _parent_of_property( name ), index );								\
			}																						\
			template< typename T >																	\
			const elem_t<T> operator () ( const T& index ) const									\
			{																						\
				return elem_t<T>( _parent_of_property( name ), index );								\
			}																						\
			template< typename T >																	\
			elem_t<T> operator () ( const T& index )												\
			{																						\
				return elem_t<T>( _parent_of_property( name ), index );								\
			}																						

	// -- internals end -----------------------------------------------------------------


// -------------------------------------------------------------------
//     property declaration
// -------------------------------------------------------------------


#define use_property( container )			typedef container _use_property_t

#define use_property_union( container )		use_property( container ); union


// -------------------------------------------------------------------
//     scalar property 
// -------------------------------------------------------------------

// - read mapped to data, write mapped to data

#define def_property_dd( name, type, rdata, wdata )											\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	operator const type () const															\
	{																						\
		return _parent_of_property( name )->rdata;											\
	}																						\
	const type operator () () const															\
	{																						\
		return _parent_of_property( name )->rdata;											\
	}																						\
	type operator () ()																		\
	{																						\
		return _parent_of_property( name )->rdata;											\
	}																						\
	void operator = ( const type value )													\
	{																						\
		_parent_of_property( name )->wdata = value;											\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

// - read mapped to data, write mapped to method

#define def_property_df( name, type, rdata, writer_fn )										\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	operator const type () const															\
	{																						\
		return _parent_of_property( name )->rdata;											\
	}																						\
	const type operator () () const															\
	{																						\
		return _parent_of_property( name )->rdata;											\
	}																						\
	type operator () ()																		\
	{																						\
		return _parent_of_property( name )->rdata;											\
	}																						\
	void operator = ( const type value )													\
	{																						\
		_parent_of_property( name )->writer_fn( value );									\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

// - read mapped to method, write mapped to data

#define def_property_fd( name, type, reader_fn, wdata )										\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	operator const type () const															\
	{																						\
		return _parent_of_property( name )->reader_fn();									\
	}																						\
	const type operator () () const															\
	{																						\
		return _parent_of_property( name )->reader_fn();									\
	}																						\
	type operator () ()																		\
	{																						\
		return _parent_of_property( name )->reader_fn();									\
	}																						\
	void operator = ( const type value )													\
	{																						\
		_parent_of_property( name )->wdata = value;											\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

// - read & write mapped to reader & writer methods

#define def_property_ff( name, type, reader_fn, writer_fn )									\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	operator const type () const															\
	{																						\
		return _parent_of_property( name )->reader_fn();									\
	}																						\
	const type operator () () const															\
	{																						\
		return _parent_of_property( name )->reader_fn();									\
	}																						\
	type operator () ()																		\
	{																						\
		return _parent_of_property( name )->reader_fn();									\
	}																						\
	void operator = ( const type value )													\
	{																						\
		_parent_of_property( name )->writer_fn( value );									\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name


// -------------------------------------------------------------------
//     1D array property 
// -------------------------------------------------------------------

// - read only, early evaluation

#define def_eproperty1d_d( name, type, rdata )												\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T >																	\
	const type operator [] ( const T& index ) const											\
	{																						\
		return _parent_of_property( name )->rdata[ index ];									\
	}																						\
	template< typename T >																	\
	type operator [] ( const T& index )														\
	{																						\
		return _parent_of_property( name )->rdata[ index ];									\
	}																						\
	template< typename T >																	\
	const type operator () ( const T& index ) const											\
	{																						\
		return _parent_of_property( name )->rdata[ index ];									\
	}																						\
	template< typename T >																	\
	type operator () ( const T& index )														\
	{																						\
		return _parent_of_property( name )->rdata[ index ];									\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

#define def_eproperty1d_f( name, type, reader_fn )											\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T >																	\
	const type operator [] ( const T& index ) const											\
	{																						\
		return _parent_of_property( name )->reader_fn( index );								\
	}																						\
	template< typename T >																	\
	type operator [] ( const T& index )														\
	{																						\
		return _parent_of_property( name )->reader_fn( index );								\
	}																						\
	template< typename T >																	\
	const type operator () ( const T& index ) const											\
	{																						\
		return _parent_of_property( name )->reader_fn( index );								\
	}																						\
	template< typename T >																	\
	type operator () ( const T& index )														\
	{																						\
		return _parent_of_property( name )->reader_fn( index );								\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

// - read & write

#define def_property1d_dd( name, type, rdata, wdata )										\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T >																	\
	struct elem_t																			\
	{																						\
		parent_t* m_p;																		\
		const T& m_index;																	\
		elem_t( parent_t* p, const T& index ) : m_p(p), m_index(index) {}					\
		operator const type () const														\
		{																					\
			return m_p->rdata[ m_index ];													\
		}																					\
		const type operator () () const														\
		{																					\
			return m_p->rdata[ m_index ];													\
		}																					\
		type operator () ()																	\
		{																					\
			return m_p->rdata[ m_index ];													\
		}																					\
		void operator = ( const type value )												\
		{																					\
			m_p->wdata[ m_index ] = value;													\
		}																					\
	};																						\
	_def_property1d_base( name )															\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

#define def_property1d_df( name, type, rdata, writer_fn )									\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T >																	\
	struct elem_t																			\
	{																						\
		parent_t* m_p;																		\
		const T& m_index;																	\
		elem_t( parent_t* p, const T& index ) : m_p(p), m_index(index) {}					\
		operator const type () const														\
		{																					\
			return m_p->rdata[ m_index ];													\
		}																					\
		const type operator () () const														\
		{																					\
			return m_p->rdata[ m_index ];													\
		}																					\
		type operator () ()																	\
		{																					\
			return m_p->rdata[ m_index ];													\
		}																					\
		void operator = ( const type value )												\
		{																					\
			m_p->writer_fn( m_index, value );												\
		}																					\
	};																						\
	_def_property1d_base( name )															\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

#define def_property1d_fd( name, type, reader_fn, wdata )									\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T >																	\
	struct elem_t																			\
	{																						\
		parent_t* m_p;																		\
		const T& m_index;																	\
		elem_t( parent_t* p, const T& index ) : m_p(p), m_index(index) {}					\
		operator const type () const														\
		{																					\
			return m_p->reader_fn( m_index );												\
		}																					\
		const type operator () () const														\
		{																					\
			return m_p->reader_fn( m_index );												\
		}																					\
		type operator () ()																	\
		{																					\
			return m_p->reader_fn( m_index );												\
		}																					\
		void operator = ( const type value )												\
		{																					\
			m_p->wdata[ m_index ] = value;													\
		}																					\
	};																						\
	_def_property1d_base( name )															\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

#define def_property1d_ff( name, type, reader_fn, writer_fn )								\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T >																	\
	struct elem_t																			\
	{																						\
		parent_t* m_p;																		\
		const T& m_index;																	\
		elem_t( parent_t* p, const T& index ) : m_p(p), m_index(index) {}					\
		operator const type () const														\
		{																					\
			return m_p->reader_fn( m_index );												\
		}																					\
		const type operator () () const														\
		{																					\
			return m_p->reader_fn( m_index );												\
		}																					\
		type operator () ()																	\
		{																					\
			return m_p->reader_fn( m_index );												\
		}																					\
		void operator = ( const type value )												\
		{																					\
			m_p->writer_fn( m_index, value );												\
		}																					\
	};																						\
	_def_property1d_base( name )															\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name


// -------------------------------------------------------------------
//     2D array property 
// -------------------------------------------------------------------

// - read only, early evaluation

#define def_eproperty2d_f( name, type, reader_fn )											\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T1, typename T2 >													\
	const type operator () ( const T1& i1, const T2& i2 ) const								\
	{																						\
		return _parent_of_property( name )->reader_fn( i1, i2 );							\
	}																						\
	template< typename T1, typename T2 >													\
	type operator () ( const T1& i1, const T2& i2 )											\
	{																						\
		return _parent_of_property( name )->reader_fn( i1, i2 );							\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

// read & write

#define def_property2d_ff( name, type, reader_fn, writer_fn )								\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T1, typename T2 >													\
	struct elem_t																			\
	{																						\
		parent_t* m_p;																		\
		const T1& m_i1;	const T2& m_i2;														\
		elem_t( parent_t* p, const T1& i1, const T2& i2 ) : m_p(p), m_i1(i1), m_i2(i2) {}	\
		operator const type () const														\
		{																					\
			return m_p->reader_fn( m_i1, m_i2 );											\
		}																					\
		const type operator () () const														\
		{																					\
			return m_p->reader_fn( m_i1, m_i2 );											\
		}																					\
		type operator () ()																	\
		{																					\
			return m_p->reader_fn( m_i1, m_i2 );											\
		}																					\
		void operator = ( const type value )												\
		{																					\
			m_p->writer_fn( m_i1, m_i2, value );											\
		}																					\
	};																						\
	template< typename T1, typename T2 >													\
	const elem_t<T1,T2> operator () ( const T1& i1, const T2& i2 ) const					\
	{																						\
		return elem_t<T1,T2>( _parent_of_property( name ), i1, i2 );						\
	}																						\
	template< typename T1, typename T2 >													\
	elem_t<T1,T2> operator () ( const T1& i1, const T2& i2 )								\
	{																						\
		return elem_t<T1,T2>( _parent_of_property( name ), i1, i2 );						\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name


// -------------------------------------------------------------------
//     3D array property 
// -------------------------------------------------------------------

// - read only, early evaluation

#define def_eproperty3d_f( name, type, reader_fn )											\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T1, typename T2, typename T3 >										\
	const type operator () ( const T1& i1, const T2& i2, const T3& i3 ) const				\
	{																						\
		return _parent_of_property( name )->reader_fn( i1, i2, i3 );						\
	}																						\
	template< typename T1, typename T2, typename T3 >										\
	type operator () ( const T1& i1, const T2& i2, const T3& i3 )							\
	{																						\
		return _parent_of_property( name )->reader_fn( i1, i2, i3 );						\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name

// - read & write

#define def_property3d_ff( name, type, reader_fn, writer_fn )								\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >		\
{																							\
	template< typename T1, typename T2, typename T3 >										\
	struct elem_t																			\
	{																						\
		parent_t* m_p;																		\
		const T1& m_i1; const T2& m_i2; const T3& m_i3;										\
		elem_t( parent_t* p, const T1& i1, const T2& i2, const T3& i3 )						\
			: m_p(p), m_i1(i1), m_i2(i2), m_i3(i3) {}										\
		operator const type () const														\
		{																					\
			return m_p->reader_fn( m_i1, m_i2, m_i3 );										\
		}																					\
		const type operator () () const														\
		{																					\
			return m_p->reader_fn( m_i1, m_i2, m_i3 );										\
		}																					\
		type operator () ()																	\
		{																					\
			return m_p->reader_fn( m_i1, m_i2, m_i3 );										\
		}																					\
		void operator = ( const type value )												\
		{																					\
			m_p->writer_fn( m_i1, m_i2, m_i3, value );										\
		}																					\
	};																						\
	template< typename T1, typename T2, typename T3 >										\
	const elem_t<T1,T2,T3> operator () ( const T1& i1, const T2& i2, const T3& i3 ) const	\
	{																						\
		return elem_t<T1,T2,T3>( _parent_of_property( name ), i1, i2, i3 );					\
	}																						\
	template< typename T1, typename T2, typename T3 >										\
	elem_t<T1,T2,T3> operator () ( const T1& i1, const T2& i2, const T3& i3 )				\
	{																						\
		return elem_t<T1,T2,T3>( _parent_of_property( name ), i1, i2, i3 );					\
	}																						\
};																							\
friend struct _prop_t_##name< type, _use_property_t >;										\
_prop_t_##name< type, _use_property_t > name


// -------------------------------------------------------------------
//     4D array property 
// -------------------------------------------------------------------

// - read only, early evaluation

#define def_eproperty4d_f( name, type, reader_fn )															\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >						\
{																											\
	template< typename T1, typename T2, typename T3, typename T4 >											\
	const type operator () ( const T1& i1, const T2& i2, const T3& i3, const T4& i4 ) const					\
	{																										\
		return _parent_of_property( name )->reader_fn( i1, i2, i3, i4 );									\
	}																										\
	template< typename T1, typename T2, typename T3, typename T4 >											\
	type operator () ( const T1& i1, const T2& i2, const T3& i3, const T4& i4 )								\
	{																										\
		return _parent_of_property( name )->reader_fn( i1, i2, i3, i4 );									\
	}																										\
};																											\
friend struct _prop_t_##name< type, _use_property_t >;														\
_prop_t_##name< type, _use_property_t > name

// - read & write

#define def_property4d_ff( name, type, reader_fn, writer_fn )												\
template< class T, class parent_t > struct _prop_t_##name : public _prop_base_t< T >						\
{																											\
	template< typename T1, typename T2, typename T3, typename T4 >											\
	struct elem_t																							\
	{																										\
		parent_t* m_p;																						\
		const T1& m_i1; const T2& m_i2; const T3& m_i3; const T4& m_i4;										\
		elem_t( parent_t* p, const T1& i1, const T2& i2, const T3& i3, const T4& i4 )						\
			: m_p(p), m_i1(i1), m_i2(i2), m_i3(i3), m_i4(i4) {}												\
		operator const type () const																		\
		{																									\
			return m_p->reader_fn( m_i1, m_i2, m_i3, m_i4 );												\
		}																									\
		const type operator () () const																		\
		{																									\
			return m_p->reader_fn( m_i1, m_i2, m_i3, m_i4 );												\
		}																									\
		type operator () ()																					\
		{																									\
			return m_p->reader_fn( m_i1, m_i2, m_i3, m_i4 );												\
		}																									\
		void operator = ( const type value )																\
		{																									\
			m_p->writer_fn( m_i1, m_i2, m_i3, m_i4, value );												\
		}																									\
	};																										\
	template< typename T1, typename T2, typename T3, typename T4 >											\
	const elem_t<T1,T2,T3,T4> operator () ( const T1& i1, const T2& i2, const T3& i3, const T4& i4 ) const	\
	{																										\
		return elem_t<T1,T2,T3,T4>( _parent_of_property( name ), i1, i2, i3, i4 );							\
	}																										\
	template< typename T1, typename T2, typename T3, typename T4 >											\
	elem_t<T1,T2,T3,T4> operator () ( const T1& i1, const T2& i2, const T3& i3, const T4& i4 )				\
	{																										\
		return elem_t<T1,T2,T3,T4>( _parent_of_property( name ), i1, i2, i3, i4 );							\
	}																										\
};																											\
friend struct _prop_t_##name< type, _use_property_t >;														\
_prop_t_##name< type, _use_property_t > name


// - aliases


#define def_property( name, type, reader_fn, writer_fn )	def_property_ff( name, type, reader_fn, writer_fn )
#define def_property1d( name, type, reader_fn, writer_fn )	def_property1d_ff( name, type, reader_fn, writer_fn )
#define def_property2d( name, type, reader_fn, writer_fn )	def_property2d_ff( name, type, reader_fn, writer_fn )
#define def_property3d( name, type, reader_fn, writer_fn )	def_property3d_ff( name, type, reader_fn, writer_fn )
#define def_property4d( name, type, reader_fn, writer_fn )	def_property4d_ff( name, type, reader_fn, writer_fn )
#define def_eproperty1d( name, type, reader_fn )			def_eproperty1d_f( name, type, reader_fn )
#define def_eproperty2d( name, type, reader_fn )			def_eproperty2d_f( name, type, reader_fn )
#define def_eproperty3d( name, type, reader_fn )			def_eproperty3d_f( name, type, reader_fn )
#define def_eproperty4d( name, type, reader_fn )			def_eproperty4d_f( name, type, reader_fn )

#endif
