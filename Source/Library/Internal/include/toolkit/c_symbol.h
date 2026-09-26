#ifndef __C_SYMBOL_V2_2_H__
#define __C_SYMBOL_V2_2_H__

/*
 *    Simple symbol(or id) type based on string table 
 *
 *           2005.11.25 by Young-Hyun Joo
 *
 *        ver.2 on 2006.6.14 by Young-Hyun Joo
 */

#include <string.h>
#include <stddef.h>
#include <deque>
#include "khash.h"

struct c_symbol_dummy_lock
{
	void lock() {}
	void unlock() {}
};

template< typename SYMSPACE, typename LOCK = c_symbol_dummy_lock >		///< SYMSPACE : dummy type indicating symbol namespace 
struct c_symbol
{
	class symi;

	// case sensitive version

	class sym
	{
	public:

		sym()															{ m_pStr = NULL; }
		explicit sym( const char* pStr, bool bCopyStrOnAdd = true )		{ m_pStr = c_symbol::manager().get( pStr, bCopyStrOnAdd, -1 ); }
		sym( const char* pStr, size_t len )								{ m_pStr = c_symbol::manager().get( pStr, true, int(len) ); }
		sym( const char* pStr, int len )								{ m_pStr = c_symbol::manager().get( pStr, true, len ); }
		sym( const std::string & str )									{ m_pStr = c_symbol::manager().get( str.c_str(), true, int(str.size()) ); }
		sym( const sym& asym )											{ m_pStr = asym.m_pStr; }
		sym& operator = ( const sym& asym )								{ m_pStr = asym.m_pStr; return *this; }

		operator uintptr_t() const										{ return uintptr_t( m_pStr ); }
		const char* c_str() const										{ return m_pStr; }

		bool operator == ( const char* pStr ) const						{ return m_pStr == pStr || !strcmp( m_pStr, (pStr && !*pStr) ? NULL : pStr ); }
		bool operator != ( const char* pStr ) const						{ return m_pStr != pStr && strcmp( m_pStr, (pStr && !*pStr) ? NULL : pStr ); }
		bool operator == ( int n ) const								{ assert( n == NULL ); return m_pStr == (const char*)n; }
		bool operator != ( int n ) const								{ assert( n == NULL ); return m_pStr != (const char*)n; }

		bool operator == ( const sym& asym ) const						{ return m_pStr == asym.c_str(); }
		bool operator != ( const sym& asym ) const						{ return m_pStr != asym.c_str(); }
		bool operator == ( const symi& asym ) const						{ return m_pStr == asym.c_str() || !stricmp( m_pStr, asym.c_str() ); }
		bool operator != ( const symi& asym ) const						{ return m_pStr != asym.c_str() && stricmp( m_pStr, asym.c_str() ); }
		
		static void ClearTable()										{ c_symbol::manager().clear(); }
		static int Count()												{ return c_symbol::manager().m_table.size(); }

		bool TestAndSet( const char* pStr )								{ m_pStr = c_symbol::manager().lookUp( pStr, -1 ); return pStr == NULL || *pStr == 0 || m_pStr; }
		bool TestAndSet( const char* pStr, size_t len )					{ m_pStr = c_symbol::manager().lookUp( pStr, int(len) ); return pStr == NULL || *pStr == 0 || m_pStr; }
		bool TestAndSet( const char* pStr, int len )					{ m_pStr = c_symbol::manager().lookUp( pStr, len ); return pStr == NULL || *pStr == 0 || m_pStr; }

	private:
		const char* m_pStr;
	};

	class csym : public sym
	{
	public:
		
		csym( const char* pStr ) : sym( pStr, false ) {}
	};


	/// case insensitive version
	class symi
	{
	public:

		symi()															{ m_pStr = NULL; }
		explicit symi( const char* pStr, bool bCopyStrOnAdd = true )	{ m_pStr = c_symbol::manageri().get( pStr, bCopyStrOnAdd, -1 ); }
		symi( const char* pStr, int len )								{ m_pStr = c_symbol::manageri().get( pStr, true, len ); }
		symi( const char* pStr, size_t len )							{ m_pStr = c_symbol::manageri().get( pStr, true, int(len) ); }
		symi( const symi& asym )										{ m_pStr = asym.m_pStr; }
		symi& operator = ( const symi& asym )							{ m_pStr = asym.m_pStr; return *this; }

		operator uintptr_t() const										{ return uintptr_t( m_pStr ); }
		const char* c_str() const										{ return m_pStr; }
        const char* safe_str() const                                    { return m_pStr ? m_pStr : ""; }

		bool operator == ( const char* pStr ) const						{ return m_pStr == pStr || !stricmp( m_pStr, (pStr && !*pStr) ? NULL : pStr ); }
		bool operator != ( const char* pStr ) const						{ return m_pStr != pStr && stricmp( m_pStr, (pStr && !*pStr) ? NULL : pStr ); }
		bool operator == ( int n ) const								{ assert( n == NULL ); return m_pStr == (const char*)n; }
		bool operator != ( int n ) const								{ assert( n == NULL ); return m_pStr != (const char*)n; }

		bool operator == ( const symi& asym ) const						{ return m_pStr == asym.c_str(); }
		bool operator != ( const symi& asym ) const						{ return m_pStr != asym.c_str(); }
		bool operator == ( const sym& asym ) const						{ return m_pStr == asym.c_str() || !stricmp( m_pStr, asym.c_str() ); }
		bool operator != ( const sym& asym ) const						{ return m_pStr != asym.c_str() && stricmp( m_pStr, asym.c_str() ); }
		
		static void ClearTable()										{ c_symbol::manageri().clear(); }
		static int Count()												{ return c_symbol::manageri().m_table.size(); }

		bool TestAndSet( const char* pStr )								{ m_pStr = c_symbol::manageri().lookUp( pStr, -1 ); return pStr == NULL || *pStr == 0 || m_pStr; }
		bool TestAndSet( const char* pStr, size_t len )					{ m_pStr = c_symbol::manageri().lookUp( pStr, int(len) ); return pStr == NULL || *pStr == 0 || m_pStr; }
		bool TestAndSet( const char* pStr, int len )					{ m_pStr = c_symbol::manageri().lookUp( pStr, len ); return pStr == NULL || *pStr == 0 || m_pStr; }

	private:
		const char* m_pStr;
	};

	class csymi : public symi
	{
	public:
		
		csymi( const char* pStr ) : symi( pStr, false ) {}
	};

private:

	friend class sym;
	friend class symi;

	template< typename HashPr >
	struct symManager
	{
		void clear()
		{
			m_table.clear();
			m_holder.clear();
		}

		const char* lookUp( const char* pStr, int len )
		{
			if ( pStr == NULL || *pStr == 0 )
				return NULL;

			m_cs.lock();

			if ( len > 0 ) {

				std::string key( pStr, len );
				if ( !m_table.lookup( key.c_str(), pStr ) )
					pStr = NULL;

			} else {

				if ( !m_table.lookup( pStr, pStr ) )
					pStr = NULL;
			}

			m_cs.unlock();
			return pStr;
		}

		const char* get( const char* pStr, bool bCopyStr, int len )
		{
			if ( pStr == NULL || *pStr == 0 )
				return NULL;

			m_cs.lock();

			if ( len > 0 ) {

				std::string key( pStr, len );
				if ( m_table.lookup( key.c_str(), pStr ) )
				{
					m_cs.unlock();
					return pStr;
				}

				m_holder.push_back( key );
				pStr = m_holder.back().c_str();

			} else {

				if ( m_table.lookup( pStr, pStr ) )
				{
					m_cs.unlock();
					return pStr;
				}

				if ( bCopyStr ) {
					m_holder.push_back( std::string( pStr ) );
					pStr = m_holder.back().c_str();
				}
			}

			m_table.add( pStr, pStr );

			m_cs.unlock();
			return pStr;
		}

		KHash< const char*, HashPr > m_table;
		std::deque< std::string > m_holder;
		LOCK m_cs;
	};

	static symManager< hashPr_pchar >& manager()			
	{ 
		static symManager< hashPr_pchar > mgr; 
		return mgr; 
	}
	
	static symManager< hashPr_pchar_nocase >& manageri() 	
	{ 
		static symManager< hashPr_pchar_nocase > mgr; 
		return mgr; 
	}
};

template< typename SYMSPACE > inline bool operator == ( const char* pStr, const typename c_symbol< SYMSPACE >::sym& asym )	
{ return pStr == asym.c_str() || !strcmp( asym.c_str(), (pStr && !*pStr) ? NULL : pStr ); }

template< typename SYMSPACE > inline bool operator != ( const char* pStr, const typename c_symbol< SYMSPACE >::sym& asym )	
{ return pStr != asym.c_str() && strcmp( asym.c_str(), (pStr && !*pStr) ? NULL : pStr ); }

template< typename SYMSPACE > inline bool operator == ( const char* pStr, const typename c_symbol< SYMSPACE >::symi& asym )	
{ return pStr == asym.c_str() || !stricmp( asym.c_str(), (pStr && !*pStr) ? NULL : pStr ); }

template< typename SYMSPACE > inline bool operator != ( const char* pStr, const typename c_symbol< SYMSPACE >::symi& asym )	
{ return pStr != asym.c_str() && stricmp( asym.c_str(), (pStr && !*pStr) ? NULL : pStr ); }

template< typename S1, typename S2 > inline bool operator == ( const typename c_symbol<S1>::sym& asym1, const typename c_symbol<S2>::sym& asym2 )
{ return asym1.c_str() == asym2.c_str() || !strcmp( asym1.c_str(), asym2.c_str() ); }

template< typename S1, typename S2 > inline bool operator != ( const typename c_symbol<S1>::sym& asym1, const typename c_symbol<S2>::sym& asym2 )
{ return asym1.c_str() != asym2.c_str() && strcmp( asym1.c_str(), asym2.c_str() ); }

template< typename S1, typename S2 > inline bool operator == ( const typename c_symbol<S1>::sym& asym1, const typename c_symbol<S2>::symi& asym2 )
{ return asym1.c_str() == asym2.c_str() || !stricmp( asym1.c_str(), asym2.c_str() ); }

template< typename S1, typename S2 > inline bool operator != ( const typename c_symbol<S1>::sym& asym1, const typename c_symbol<S2>::symi& asym2 )
{ return asym1.c_str() != asym2.c_str() && stricmp( asym1.c_str(), asym2.c_str() ); }

template< typename S1, typename S2 > inline bool operator == ( const typename c_symbol<S1>::symi& asym1, const typename c_symbol<S2>::sym& asym2 )
{ return asym1.c_str() == asym2.c_str() || !stricmp( asym1.c_str(), asym2.c_str() ); }

template< typename S1, typename S2 > inline bool operator != ( const typename c_symbol<S1>::symi& asym1, const typename c_symbol<S2>::sym& asym2 )
{ return asym1.c_str() != asym2.c_str() && stricmp( asym1.c_str(), asym2.c_str() ); }

template< typename S1, typename S2 > inline bool operator == ( const typename c_symbol<S1>::symi& asym1, const typename c_symbol<S2>::symi& asym2 )
{ return asym1.c_str() == asym2.c_str() || !stricmp( asym1.c_str(), asym2.c_str() ); }

template< typename S1, typename S2 > inline bool operator != ( const typename c_symbol<S1>::symi& asym1, const typename c_symbol<S2>::symi& asym2 )
{ return asym1.c_str() != asym2.c_str() && stricmp( asym1.c_str(), asym2.c_str() ); }


#endif
