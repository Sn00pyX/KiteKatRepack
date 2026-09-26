#ifndef __C_ARGS_V2_2_H__
#define __C_ARGS_V2_2_H__

/*
 *    A simple, low-overhead argument block type for dynamic message dispatching
 *
 *     2006. 6.28 : v2.2 
 *
 *     2006. 6.19 : v2.1 - Added type checking asserts
 *
 *     2006. 6.15 : v2.0 by Young-Hyun Joo
 */

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

#pragma pack( push, 4 )

struct c_args
{
	
	//
	// argument holder base class (reader interface)
	//

	template< typename ID >
	struct base
	{	
		base()														{ m_slotCount = m_maxSlot = 0; m_bHasId = m_bHasTypePtr = false; }

		int			slotCount() const								{ return m_slotCount; }

		bool		hasIds() const									{ return m_bHasId; }
		ID			idAt( int slot ) const							{ return get_id_base()[ verify( slot ) ]; }
		int			slotBy( ID id ) const							{ return get_slot_by( id ); }				// -1 if unmatched

		bool		hasTypes() const								{ return m_bHasTypePtr; }
		void*		typeAt( int slot ) const						{ return get_typeptr_base()[ verify( slot ) ]; }
		void*		typeBy( ID id ) const							{ return get_typeptr_base()[ verify( get_slot_by( id ) ) ]; }
		
		template< typename T >
		/// access by id
		const T&	by( const ID& id ) const						{ return *get_param_at<T>( get_slot_by( id ) ); }

		template< typename T >
		bool		setBy( const ID& id, const T& val )				{ return set_param_at( get_slot_by( id ), val ); }
		
		template< typename T >
		/// access by slot # 
		const T&	at( int slot ) const							{ return *get_param_at<T>( slot ); }

		template< typename T >
		bool		setAt( int slot, const T& val )					{ return set_param_at( slot, val ); }

		// convenience converters (needs type info)

		int			asIntAt( int slot ) const						{ return get_num_at< int >( slot ); }
		float		asFloatAt( int slot ) const						{ return get_num_at< float >( slot ); }
		double		asDoubleAt( int slot ) const					{ return get_num_at< double >( slot ); }
		long long	asLongLongAt( int slot ) const					{ return get_num_at< long long >( slot ); }
		long double	asLongDoubleAt( int slot ) const				{ return get_num_at< long double >( slot ); }

		int			asIntBy( const ID& id ) const					{ return get_num_by< int >( id ); }
		float		asFloatBy( const ID& id ) const					{ return get_num_by< float >( id ); }
		double		asDoubleBy( const ID& id ) const				{ return get_num_by< double >( id ); }
		long long	asLongLongBy( const ID& id ) const				{ return get_num_by< long long >( id ); }
		long double	asLongDoubleBy( const ID& id ) const			{ return get_num_by< long double >( id ); }

	protected:

		base( int ) {}

		/// mainly for debugging (watch)
		union slot_t
		{
			uintptr_t u;			
			intptr_t  i;
			float	  f;
			void*	  p;
		};

		unsigned char m_slotCount, m_maxSlot;
		bool m_bHasId;
		bool m_bHasTypePtr;

		const slot_t* get_slot_base() const	
		{ 
			return reinterpret_cast< const slot_t* >( this+1 ); 
		}
		
		const ID* get_id_base() const		
		{ 
			assert( m_bHasId && "No IDs" );
			return reinterpret_cast< const ID* >( get_slot_base() + m_maxSlot ); 
		}

		void* const* get_typeptr_base() const
		{
			assert( m_bHasTypePtr && "No Typeptrs" );
			return m_bHasId ? reinterpret_cast< void* const* >( reinterpret_cast< const ID* >( get_slot_base() + m_maxSlot ) + m_maxSlot ) 
							: reinterpret_cast< void* const* >( get_slot_base() + m_maxSlot );
		}

		int verify( int slot ) const
		{
			assert( slot >= 0 && slot < m_slotCount && "Invalid slot number" );
			return slot;
		}

		int	get_slot_by( ID id ) const				
		{
			const ID* pId = get_id_base();
			for ( int i = 0; i < m_slotCount; i++ )
				if ( pId[i] == id )
					return i;
			return -1;
		}

		template< typename T >
		const T* get_param_at( int slot ) const
		{
			assert( slot >= 0 && slot < m_slotCount && "Invalid slot number" );
			assert( is_correct_type<T>( slot ) && "type checking failure" );
			return reinterpret_cast< const T* >( get_slot_base() + slot );
		}

		template< typename T >
		bool set_param_at( int slot, const T& val )
		{
			if ( slot >= 0 && slot + (sizeof(T) / sizeof(slot_t)) <= m_slotCount )
			{
				assert( is_correct_type<T>( slot ) && "type error" );
				*const_cast< slot_t* >( get_slot_base() + slot ) = val;
				return true;
			}
			return false;
		}

		template< typename T >
		bool is_correct_type( int slot ) const
		{
			return get_typeptr_base()[ slot ] == TypePtr< T >::ptr();
		}

		#pragma pack(push,1)
		
		template< typename T > struct Filler
		{
			char v[ sizeof(T) ];
		};
		
		#pragma pack(pop)

		template< typename T >
		void bitwise_copy( void* to, const T* from )
		{
			*reinterpret_cast< Filler<T>* >( to ) = *reinterpret_cast< const Filler<T>* >( from );
		}

		template< typename T >
		const T get_num_at( int slot ) const
		{
			void* ptr = typeAt( slot );
			if ( ptr == TypePtr< int >::ptr() )			return T( at< int >( slot ) );
			if ( ptr == TypePtr< bool >::ptr() )		return T( at< bool >( slot ) );
			if ( ptr == TypePtr< float >::ptr() )		return T( at< float >( slot ) );
			if ( ptr == TypePtr< double >::ptr() )		return T( at< double >( slot ) );
			if ( ptr == TypePtr< long long >::ptr() )	return T( at< long long >( slot ) );
			if ( ptr == TypePtr< long double >::ptr() )	return T( at< long double >( slot ) );
			if ( ptr == TypePtr< const char* >::ptr() )	return T( atof( at< const char* >( slot ) ) );

			assert( !"Non-numeric type" );

			return 0;
		}

		template< typename T >
		const T get_num_by( const ID& id ) const
		{
			void* ptr = typeBy( id );
			if ( ptr == TypePtr< int >::ptr() )			return T( by< int >( id ) );
			if ( ptr == TypePtr< bool >::ptr() )		return T( by< bool >( id ) );
			if ( ptr == TypePtr< float >::ptr() )		return T( by< float >( id ) );
			if ( ptr == TypePtr< double >::ptr() )		return T( by< double >( id ) );
			if ( ptr == TypePtr< long long >::ptr() )	return T( by< long long >( id ) );
			if ( ptr == TypePtr< long double >::ptr() )	return T( by< long double >( id ) );
			if ( ptr == TypePtr< const char* >::ptr() )	return T( atof( by< const char* >( id ) ) );

			assert( !"Non-numeric type" );

			return 0;
		}
	};

	//
	// unnamed arguments without type info
	//

	template< typename ID, unsigned char MAX_SLOT >
	struct values : public base< ID >
	{
		values(): base< ID >(0)
		{ init(); }

		template< typename T1 >
		values& operator () ( const T1& v1 )	
		{ add( v1 ); return *this; }

		template< typename T1, typename T2 >
		values& operator () ( const T1& v1, const T2& v2 )	
		{ add( v1 ); add( v2 ); return *this; }

		template< typename T1, typename T2, typename T3 >
		values& operator () ( const T1& v1, const T2& v2, const T3& v3 )	
		{ add( v1 ); add( v2 ); add( v3 ); return *this; }

		template< typename T1, typename T2, typename T3, typename T4 >
		values& operator () ( const T1& v1, const T2& v2, const T3& v3, const T4& v4 )	
		{ add( v1 ); add( v2 ); add( v3 ); add( v4 ); return *this; }

		template< typename T >
		int add( const T& v )
		{
			const int pos = m_slotCount;

			m_slotCount += (sizeof(T) + sizeof(slot_t) - 1) / sizeof(slot_t);
			assert( m_slotCount <= MAX_SLOT && "Arguments exceed slot space" );

			bitwise_copy( m_slots + pos, &v );
			assert( m_typeptrs[ pos ] = TypePtr<T>::ptr() );
			return pos;
		}

		template< typename T >
		/// to treat array literals as pointers
		int add( const T p[] )
		{
			assert( m_slotCount + 1 <= MAX_SLOT && "Arguments exceed slot space" );

			m_slots[ m_slotCount ].p = const_cast< T* >(p);
			assert( m_typeptrs[ m_slotCount ] = TypePtr<T*>::ptr() );
			return m_slotCount++;
		}

	private:

		/// disable copy constructor
		values( const values<ID, MAX_SLOT>& a );

		void init()
		{
			m_slotCount = 0; m_maxSlot = MAX_SLOT; m_bHasId = false; m_bHasTypePtr = false;
			assert( m_bHasTypePtr = true );
			assert( memset( m_typeptrs, 0, sizeof(m_typeptrs) ) );
		}

		slot_t		m_slots[ MAX_SLOT ];

#ifdef _DEBUG

		void*		m_typeptrs[ MAX_SLOT ];
		
#endif
	};

	//
	// named arguments without type info 
	//

	template< typename ID, unsigned char MAX_SLOT >
	struct id_values : public base< ID >
	{
		id_values(): base< ID >(0)
		{ init(); }

		template< typename T >
		id_values& operator () ( const T& v )	
		{ add( ID(), v ); return *this; }

		template< typename T1 >
		id_values& operator () ( ID k1, const T1& v1 )	
		{ add( k1, v1 ); return *this; }

		template< typename T1, typename T2 >
		id_values& operator () ( ID k1, const T1& v1, ID k2, const T2& v2 )	
		{ add( k1, v1 ); add( k2, v2 ); return *this; }

		template< typename T >
		int add( ID id, const T& v )
		{
			const int pos = m_slotCount;

			m_slotCount += (sizeof(T) + sizeof(slot_t) - 1) / sizeof(slot_t);
			assert( m_slotCount <= MAX_SLOT && "Arguments exceed slot space" );

			bitwise_copy( m_slots + pos, &v );
			m_ids[ pos ] = id;
			assert( m_typeptrs[ pos ] = TypePtr<T>::ptr() );

			return pos;
		}

		template< typename T >
		/// to treat array literals as pointers
		int add( ID id, const T p[] )
		{
			assert( m_slotCount + 1 <= MAX_SLOT && "Arguments exceed slot space" );

			m_ids[ m_slotCount ] = id;
			m_slots[ m_slotCount ].p = const_cast< T* >(p);
			assert( m_typeptrs[ m_slotCount ] = TypePtr<T*>::ptr() );
			return m_slotCount++;
		}

	private:

		/// disable copy constructor
		id_values( const id_values<ID, MAX_SLOT>& a );

		void init()
		{
			m_slotCount = 0; m_maxSlot = MAX_SLOT; m_bHasId = true; m_bHasTypePtr = false;
			assert( m_bHasTypePtr = true );
			assert( memset( m_typeptrs, 0, sizeof( m_typeptrs ) ) );
		}

		slot_t		m_slots[ MAX_SLOT ];
		ID			m_ids[ MAX_SLOT ];

#ifdef _DEBUG
	
		void*		m_typeptrs[ MAX_SLOT ];

#endif
	};

	//
	// unnamed arguments with type info
	//

	template< typename ID, unsigned char MAX_SLOT >
	struct tvalues : public base< ID >
	{
		tvalues(): base< ID >(0)
		{ init(); }

		template< typename T1 >
		tvalues& operator () ( const T1& v1 )	
		{ add( v1 ); return *this; }

		template< typename T1, typename T2 >
		tvalues& operator () ( const T1& v1, const T2& v2 )	
		{ add( v1 ); add( v2 ); return *this; }

		template< typename T1, typename T2, typename T3 >
		tvalues& operator () ( const T1& v1, const T2& v2, const T3& v3 )	
		{ add( v1 ); add( v2 ); add( v3 ); return *this; }

		template< typename T1, typename T2, typename T3, typename T4 >
		tvalues& operator () ( const T1& v1, const T2& v2, const T3& v3, const T4& v4 )	
		{ add( v1 ); add( v2 ); add( v3 ); add( v4 ); return *this; }

		template< typename T >
		int add( const T& v )
		{
			const int pos = m_slotCount;

			m_slotCount += (sizeof(T) + sizeof(slot_t) - 1) / sizeof(slot_t);
			assert( m_slotCount <= MAX_SLOT && "Arguments exceed slot space" );

			bitwise_copy( m_slots + pos, &v );
			m_typeptrs[ pos ] = TypePtr<T>::ptr();
			return pos;
		}

		template< typename T >
		/// to treat array literals as pointers
		int add( const T p[] )
		{
			assert( m_slotCount + 1 <= MAX_SLOT && "Arguments exceed slot space" );

			m_slots[ m_slotCount ].p = const_cast< T* >(p);
			m_typeptrs[ m_slotCount ] = TypePtr<T*>::ptr();
			return m_slotCount++;
		}

	private:

		/// disable copy constructor
		tvalues( const tvalues<ID, MAX_SLOT>& a );

		void init()
		{
			m_slotCount = 0; m_maxSlot = MAX_SLOT; m_bHasId = false; m_bHasTypePtr = true;
			memset( m_typeptrs, 0, sizeof(m_typeptrs) );
		}

		slot_t		m_slots[ MAX_SLOT ];
		void*		m_typeptrs[ MAX_SLOT ];
	};

	//
	// named arguments with type info
	//

	template< typename ID, unsigned char MAX_SLOT >
	struct id_tvalues : public base< ID >
	{
		id_tvalues(): base< ID >(0)
		{ init(); }

		template< typename T >
		id_tvalues& operator () ( const T& v )	
		{ add( ID(), v ); return *this; }

		template< typename T1 >
		id_tvalues& operator () ( ID k1, const T1& v1 )	
		{ add( k1, v1 ); return *this; }

		template< typename T1, typename T2 >
		id_tvalues& operator () ( ID k1, const T1& v1, ID k2, const T2& v2 )	
		{ add( k1, v1 ); add( k2, v2 ); return *this; }

		template< typename T >
		int add( ID id, const T& v )
		{
			const int pos = m_slotCount;

			m_slotCount += (sizeof(T) + sizeof(slot_t) - 1) / sizeof(slot_t);
			assert( m_slotCount <= MAX_SLOT && "Arguments exceed slot space" );

			bitwise_copy( m_slots + pos, &v );
			m_ids[ pos ] = id;
			m_typeptrs[ pos ] = TypePtr<T>::ptr();

			return pos;
		}

		template< typename T >
		/// to treat array literals as pointers
		int add( ID id, const T p[] )
		{
			assert( m_slotCount + 1 <= MAX_SLOT && "Arguments exceed slot space" );

			m_ids[ m_slotCount ] = id;
			m_slots[ m_slotCount ].p = const_cast< T* >(p);
			m_typeptrs[ m_slotCount ] = TypePtr<T*>::ptr();
			return m_slotCount++;
		}

	private:

		/// disable copy constructor
		id_tvalues( const id_tvalues<ID, MAX_SLOT>& a );

		void init()
		{
			m_slotCount = 0; m_maxSlot = MAX_SLOT; m_bHasId = true; m_bHasTypePtr = true;
			memset( m_typeptrs, 0, sizeof(m_typeptrs) );
		}

		slot_t		m_slots[ MAX_SLOT ];
		ID			m_ids[ MAX_SLOT ];
		void*		m_typeptrs[ MAX_SLOT ];
	};

	//
	// slot size calculator
	//

	template< typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void, 
			  typename T5 = void, typename T6 = void, typename T7 = void, typename T8 = void >
	struct SlotSize {
		enum { 
			result = CalcSlot<T1>::result + CalcSlot<T2>::result + CalcSlot<T3>::result + CalcSlot<T4>::result
				   + CalcSlot<T5>::result + CalcSlot<T6>::result + CalcSlot<T7>::result + CalcSlot<T8>::result
		};
	};

	//
	// custom type infos. 	
	//

		// int-coerciable types (char, short, long, enum...)  ->  int

	template< typename T > struct TypePtr { 
		static void* ptr() { 
			return IntConvChecker< T, sizeof(int_conv_tester(T())) == sizeof(yes_t) && sizeof(T) <= sizeof(int) >::ptr(); 
		}
	};

		// primitive types

	template<> struct TypePtr< void >							{ static void* ptr() { return NULL; } };
	template<> struct TypePtr< int >							{ static void* ptr() { return (void*)1; } };
	template<> struct TypePtr< long long >						{ static void* ptr() { return (void*)2; } };
	template<> struct TypePtr< float >							{ static void* ptr() { return (void*)3; } };
	template<> struct TypePtr< double >							{ static void* ptr() { return (void*)4; } };
	template<> struct TypePtr< long double >					{ static void* ptr() { return (void*)5; } };
	template<> struct TypePtr< char* >							{ static void* ptr() { return (void*)6; } };
	template<> struct TypePtr< bool >							{ static void* ptr() { return (void*)7; } };

		// const  ->  non-const

	template< typename T > struct TypePtr< const T >			{ static void* ptr() { return TypePtr<T>::ptr(); } };
	template< typename T > struct TypePtr< const T* >			{ static void* ptr() { return TypePtr<T*>::ptr(); }	};

		// array  ->  pointer

	template< typename T, int N > struct TypePtr< T[N] >		{ static void* ptr() { return TypePtr<T*>::ptr(); } };
	template< typename T, int N > struct TypePtr< const T[N] >	{ static void* ptr() { return TypePtr<T*>::ptr(); } };

		// unsigned  ->  signed

	template<> struct TypePtr< unsigned int >					{ static void* ptr() { return TypePtr< int >::ptr(); } };
	template<> struct TypePtr< unsigned char >					{ static void* ptr() { return TypePtr< int >::ptr(); } };
	template<> struct TypePtr< unsigned short >					{ static void* ptr() { return TypePtr< int >::ptr(); } };
	template<> struct TypePtr< unsigned long >					{ static void* ptr() { return TypePtr< int >::ptr(); } };
	template<> struct TypePtr< unsigned long long >				{ static void* ptr() { return TypePtr< long long >::ptr(); } };

	template<> struct TypePtr< unsigned int* >					{ static void* ptr() { return TypePtr< int* >::ptr(); }	};
	template<> struct TypePtr< unsigned char* >					{ static void* ptr() { return TypePtr< char* >::ptr(); } };
	template<> struct TypePtr< unsigned short* >				{ static void* ptr() { return TypePtr< short* >::ptr(); } };
	template<> struct TypePtr< unsigned long* >					{ static void* ptr() { return TypePtr< long* >::ptr(); } };
	template<> struct TypePtr< unsigned long long* >			{ static void* ptr() { return TypePtr< long long* >::ptr(); } };

	template< int N > struct TypePtr< unsigned int[N] >			{ static void* ptr() { return TypePtr< int* >::ptr(); } };
	template< int N > struct TypePtr< unsigned char[N] >		{ static void* ptr() { return TypePtr< char* >::ptr(); } };
	template< int N > struct TypePtr< unsigned short[N] >		{ static void* ptr() { return TypePtr< short* >::ptr(); } };
	template< int N > struct TypePtr< unsigned long[N] >		{ static void* ptr() { return TypePtr< long* >::ptr(); } };
	template< int N > struct TypePtr< unsigned long long[N] >	{ static void* ptr() { return TypePtr< long long* >::ptr(); } };

private:

	template< typename T, bool b > struct IntConvChecker;
	template< typename T > struct IntConvChecker< T, true >		{ static void* ptr() { return TypePtr<int>::ptr(); } };
	template< typename T > struct IntConvChecker< T, false >	{ static void* ptr() { static char m; return &m; } };

	typedef char no_t;
	typedef long yes_t; 

	struct int_conv_type { int_conv_type( int ); }; 

	static no_t  int_conv_tester( ... ); 
	static yes_t int_conv_tester( int_conv_type ); 

	template< typename T > struct CalcSlot						{ enum { result = ( sizeof(T) + sizeof(void*)-1 ) / sizeof(void*) }; };
	template< typename T, int N > struct CalcSlot< T[N] >		{ enum { result = 1 }; };
	template< typename T, int N > struct CalcSlot< const T[N] >	{ enum { result = 1 }; };
	template<> struct CalcSlot< void >							{ enum { result = 0 }; };
	template<> struct CalcSlot< const void >					{ enum { result = 0 }; };
};

#pragma pack( pop )

#endif