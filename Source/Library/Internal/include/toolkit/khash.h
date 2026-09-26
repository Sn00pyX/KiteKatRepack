
#pragma once


#include <vector>
#include <string>
#include <cassert>

#include "../toolkit/safe_function.h"


// nocase_tr에서 repr 함수가 아스키 값 'a' ~ 'z'일 경우만 처리하던 것을 tolower을 사용하도록 변경(해외 현지어에 따른 처리가 추가됨)
// by Synastry, 2009/09/03

// 단항 lookup 추가. 문자열 메모리를 갖고 있는 string 과 포인터만 갖는 pchar 분리 (hashPr)
// by Tyburn, 2006/05/09

// index 를 -1 로 받아오는 버그 수정함. ( 참고. 음수에는 % 연산이 안먹는다우~ -_- )
// by Testors, 2004/08/30


template< class T, class hashPr >
class KHash
{
public:

	KHash( unsigned int _capacity = 50 )
		: m_nCapacity(_capacity)
	{
		m_nTrace = 0;
		m_nSize = 0;
		m_slots = new slot[m_nCapacity];
		m_pIsSlotUse = new char[m_nCapacity];
		memset( m_pIsSlotUse, 0, m_nCapacity );

		m_nIterSlot = 0;
		m_nIterNode = 0;
	}

	~KHash()
	{
		clear();		
		delete[] m_pIsSlotUse;
		delete[] m_slots;
	}

	size_t size() const		
	{ 
		return m_nSize;
	}

	T* lookup( const typename hashPr::Key& key )
	{
		return _lookup( hashPr::getindex( key, m_nCapacity ), key );
	}

	const T* lookup( const typename hashPr::Key& key ) const
	{
		return _lookup( hashPr::getindex( key, m_nCapacity ), key );
	}

	bool lookup( const typename hashPr::Key& key, T& value ) const
	{
		const T* p = lookup( key );
		if ( p == NULL ) 
			return false;
		value = *p;
		return true;
	}

	bool has( const typename hashPr::Key& key ) const
	{
		return lookup( key ) != NULL;
	}

	void clear()
	{
		for ( int i=0 ; i< m_nCapacity ; i++ )
		{
			{	// VS2005 sp1 버그를 회피하기 위해 brace 추가함. by Testors 
				slot::slot_iterater it = m_slots[i].m_node.begin();
				slot::slot_iterater end = m_slots[i].m_node.end();
				for ( ; it != end ; ++it )
				{
					delete (*it);
				}
			}
			m_slots[i].m_node.clear();
		}
		memset( m_pIsSlotUse, 0, m_nCapacity );
		m_nSize = 0;
	}

	T* add( const typename hashPr::Key& key, const T& value )
	{
		return _add( hashPr::getindex( key, m_nCapacity ), key, value );
	}

	T* modify( const typename hashPr::Key& key, const T& value )
	{
		int index = hashPr::getindex( key, m_nCapacity );
		if ( m_pIsSlotUse[index] == 0 ) 
			return NULL;
		slot* pSlot = m_slots + index;
		node* _node = getNode( pSlot, key );
		if ( _node )
		{
			_node->value = value;
			return &_node->value;
		}
		return NULL;
	}

	bool erase( const typename hashPr::Key& key )
	{
		int index = hashPr::getindex( key, m_nCapacity );
		if ( m_pIsSlotUse[index] == 0 ) 
			return false;
		slot* pSlot = m_slots + index;
		if ( eraseNode( pSlot, key ) == true )
		{
			if ( pSlot->m_node.size() == 0 )
			{
				m_pIsSlotUse[index] = 0;
			}

			--m_nSize;

			return true;
		}
		return false;
	}

	T& operator [] ( const typename hashPr::Key& key )
	{
		int index = hashPr::getindex( key, m_nCapacity );
		
		while ( true )
		{
			if ( m_pIsSlotUse[index] != 0 ) 
			{
				slot* pSlot = m_slots + index;
				node* _node = getNode( pSlot, key );
				if ( _node )
					return _node->value;
			}

			T temp;
			_add( index, key, temp );
		}
	}

	struct node
	{
		node( const T & v, const typename hashPr::Key & k ) : value( v ), key( k )  {}

		T value;
		typename hashPr::Key key;
	};

	bool get_first_node( node*& _node )
	{
		m_nIterSlot = 0;
		m_nIterNode = 0;
		slot* s = NULL;
		while (m_nIterSlot < m_nCapacity)
		{
			if ( m_pIsSlotUse[m_nIterSlot] ==1 )
			{
				s = m_slots + m_nIterSlot;
				assert( s->m_node.size() > 0 );
				_node = s->m_node[m_nIterNode++];
				return true;
			}
			++m_nIterSlot;
		}
		_node = NULL;
		return false;
	}

	bool get_next_node( node*& _node )
	{
		slot* s = NULL;
		s = m_slots + m_nIterSlot;
		if ( m_nIterNode == int(s->m_node.size()) )
		{
			m_nIterNode = 0;
			++m_nIterSlot;
			while (m_nIterSlot < m_nCapacity)
			{
				if ( m_pIsSlotUse[m_nIterSlot] ==1 )
				{
					s = m_slots + m_nIterSlot;
					assert( s->m_node.size() > 0 );
					_node = s->m_node[m_nIterNode++];
					return true;
				}
				++m_nIterSlot;
			}
			_node = NULL;
			return false;
		}
		_node = s->m_node[m_nIterNode++];
		return true;
	}

	bool get_first_value( T& value )
	{
		node* pNode;
		if ( !get_first_node( pNode ) )
			return false;
		value = pNode->value;
		return true;
	}

	bool get_next_value( T& value )
	{
		node* pNode;
		if ( !get_next_node( pNode ) )
			return false;
		value = pNode->value;
		return true;
	}

private:

	KHash( const KHash< T, hashPr >& );
	KHash< T, hashPr >& operator=( const KHash< T, hashPr >& );

	T* _lookup( int index, const typename hashPr::Key& key )
	{
		if ( m_pIsSlotUse[index] == 0 ) 
			return NULL;
		
		slot* pSlot = m_slots + index;
		node* _node = getNode( pSlot, key );
		return _node ? &_node->value : NULL;
	}

	const T* _lookup( int index, const typename hashPr::Key& key ) const
	{
		if ( m_pIsSlotUse[index] == 0 ) 
			return NULL;

		slot* pSlot = m_slots + index;
		const node* _node = getNode( pSlot, key );
		return _node ? &_node->value : NULL;
	}

	T* _add( int index, const typename hashPr::Key& key, const T& value )
	{
		slot* pSlot = m_slots + index;
		node* _node = new node( value, key );

		if ( setNode( pSlot, _node ) == false )
		{
			delete _node;
			return NULL;
		}

		m_pIsSlotUse[ index ] = 1;
		++m_nSize;
		return &_node->value;
	}

	struct slot
	{
		slot() {}
		typename typedef std::vector< node* >::iterator slot_iterater;
		std::vector< node* > m_node;
	};
	
	template<class _FI, class _Ty, class _Pd, class _Pr> inline
	_FI _Lowerbound(_FI _F, _FI _L, const _Ty& _V, _Pr _P, _Pd *) const
	{
		_Pd _N = 0;
		std::_Distance(_F, _L, _N);
		for (; 0 < _N; )
		{
			_Pd _N2 = _N / 2;
			_FI _M = _F;
			std::advance(_M, _N2);
			if (_P((*_M)->key, _V))
				_F = ++_M, _N -= _N2 + 1;
			else
				_N = _N2;
		}
		return (_F);
	}
	
	template<class _FI, class _Ty, class _Pr> inline
	_FI lowerbound(_FI _F, _FI _L, const _Ty& _V, _Pr _P) const
	{
		return (_Lowerbound(_F, _L, _V, _P, std::_Dist_type(_F))); 
	}

	bool eraseNode( slot* _slot, const typename hashPr::Key& key )
	{
		slot::slot_iterater it = lowerbound( _slot->m_node.begin(), _slot->m_node.end(), key, hashPr::isless );
		if ( it == _slot->m_node.end() ) 
		{
			//assert(0);
			return false;
		}
		if ( hashPr::isequal( key, (*it)->key ) )
		{
			delete (*it);
			_slot->m_node.erase( it );
			return true;
		}
		return false;
	}

	bool setNode( slot* _slot, typename node* _node )
	{
		slot::slot_iterater it = lowerbound( _slot->m_node.begin(), _slot->m_node.end(), _node->key, hashPr::isless );
		if ( it != _slot->m_node.end() )
		{
			if ( hashPr::isequal( _node->key, (*it)->key ) )
				return false;
		}
		if ( _slot->m_node.size() > 0 )
			_slot->m_node.insert( it, _node );
		else _slot->m_node.push_back( _node );
		return true;
	}

	node* getNode( slot* _slot, const typename hashPr::Key& key )
	{	
		slot::slot_iterater it = lowerbound( _slot->m_node.begin(), _slot->m_node.end(), key, hashPr::isless );
		if ( it == _slot->m_node.end() || !hashPr::isequal( key, (*it)->key ) )
		{
			return NULL;
		}
		return (*it);
	}

	const node* getNode( slot* _slot, const typename hashPr::Key& key ) const
	{	
		slot::slot_iterater it = lowerbound( _slot->m_node.begin(), _slot->m_node.end(), key, hashPr::isless );
		if ( it == _slot->m_node.end() || !hashPr::isequal( key, (*it)->key ) )
		{
			return NULL;
		}
		return (*it);
	}

	int m_nTrace;
  	int m_nCapacity;
	slot* m_slots;
	char* m_pIsSlotUse;

	// for iterating

	int m_nIterSlot;
	int m_nIterNode;
	size_t m_nSize;
};


namespace khash_def
{
	template< typename T >
	struct hashPr_mod_basic
	{
		typedef T Key;

		static inline unsigned getindex( Key key, int nCapacity )
		{
			return unsigned( uintptr_t(key) % nCapacity );
		}

		static inline bool isequal( Key key1, Key key2 )
		{
			return key1 == key2;
		}
		
		static inline bool isless( Key key1, Key key2 )
		{
			return key1 < key2;
		}
	};

	template< typename mem_traits, typename comp_traits >
	struct hashPr_str
	{
		struct Key 
		{
			Key()						{ m_pStr = NULL; m_unIntKey = 0; }
			Key( const Key& str )		{ mem_traits::assign( m_pStr, str.m_pStr ); init_int_key(); }
			Key( const char* pStr )		{ mem_traits::assign( m_pStr, pStr ); init_int_key(); }
			Key( std::string & str )	{ mem_traits::assign( m_pStr, str.c_str() ); init_int_key(); }
			~Key()						{ mem_traits::release( m_pStr ); }

			void init_int_key()
			{
				const char *str = m_pStr;
				for ( m_unIntKey = 0; *str; str++ ) m_unIntKey = m_unIntKey * 32 - m_unIntKey + comp_traits::repr( *str );
			}

			char* m_pStr;
			unsigned int m_unIntKey;
		};
		
		static inline unsigned getindex( const Key &keystr, int nCapacity )
		{
			return keystr.m_unIntKey % nCapacity;
		}

		static inline bool isequal( const Key &str1, const Key &str2 )
		{
			if ( str1.m_unIntKey != str2.m_unIntKey ) return false;
			return comp_traits::compare( str1.m_pStr, str2.m_pStr ) == 0;
		}

		static inline bool isless( const Key &str1, const Key &str2 )
		{
			if ( str1.m_unIntKey < str2.m_unIntKey ) return true;
			if ( str1.m_unIntKey > str2.m_unIntKey ) return false;
			return comp_traits::compare( str1.m_pStr, str2.m_pStr ) < 0;
		}
	};

	struct string_tr
	{
		static inline void assign( char*& pDst, const char* pSrc )
		{
			size_t len = strlen( pSrc ) + 1;
			pDst = new char[len];
			s_strcpy( pDst, len, pSrc );
		}
		static inline void release( char* p )						{ delete[] p; }
	};

	struct pchar_tr
	{
		static inline void assign( char*& pDst, const char* pSrc )	{ pDst = const_cast< char* >( pSrc ); }
		static inline void release( char* p )						{}
	};

	struct case_tr
	{
		static inline int compare( const char* p, const char* q )	{ return strcmp( p, q ); }
		static inline int repr( int c )								{ return c; }
	};

	struct nocase_tr
	{
		static inline int compare( const char* p, const char* q )	{ return _stricmp( p, q ); }
		static inline int repr( int c )								{ return tolower( c ); }
	};
}

using khash_def::hashPr_mod_basic;

typedef khash_def::hashPr_mod_basic< int >                                  hashPr_mod_int;
typedef khash_def::hashPr_str< khash_def::string_tr, khash_def::case_tr >   hashPr_string;
typedef khash_def::hashPr_str< khash_def::string_tr, khash_def::nocase_tr > hashPr_string_nocase;
typedef khash_def::hashPr_str< khash_def::pchar_tr, khash_def::case_tr >    hashPr_pchar;
typedef khash_def::hashPr_str< khash_def::pchar_tr, khash_def::nocase_tr >  hashPr_pchar_nocase;

