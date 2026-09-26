#pragma once

//
//		bugfixed and revised by Young-Hyun Joo, 2006.5.21
//

#include "nsl.h"
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <wchar.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <assert.h>

/*
	ex) test.txt :						-- NOTE: field 간의 구분자는 tab. space 는 구분자로 인식되지 않음. --
	id	field1	field2	field3
	0	하하	호호	후후
	1	ABC	DEF	GHI
	2	QWE	RTY	YUI

	std::string data;
	XTextRecordSet<>	rs( "readme.txt", "id" );
	rs.SetCursor( "1" );
	data = rs.GetColumn( "field1" );	// data = ABC
	rs.NextCursor();
	data = rs.GetColumn( "field1" );	// data = QWE
*/

template< typename _Elem = char >
struct IRecordSet
{
	typedef std::basic_string< _Elem > _String;

	virtual ~IRecordSet()	{}

	virtual size_t	GetRowCount() const										= 0;
	virtual bool	SetCursorAtRow( int row )								= 0;
	virtual int		GetCurrentRow() const									= 0;
	virtual int		GetRow( const _String& key )							= 0;
	
	bool			IsExist( const _String& key )							{ return GetRow( key ) != -1; }
	virtual bool	SetCursor( const _String& key )							{ return SetCursorAtRow( GetRow( key ) ); }

	virtual bool	FirstCursor()											= 0;
	virtual bool	NextCursor()											= 0;
	virtual bool	PrevCursor()											= 0;

    virtual const _Elem*    GetColumn( const _String& field )						= 0;
    virtual const _Elem*    GetColumn( const _String& key, const _String& field )	= 0;

	virtual int		GetIntColumn( const _String& field )							{ return nsl::atoi< _Elem, int >( GetColumn( field ) );		}
	virtual int		GetIntColumn( const _String& key, const _String& field )		{ return nsl::atoi< _Elem, int >( GetColumn( key, field ) );	}

	/*
	virtual float	GetFloatColumn( const _String& field )							{ return atof??( GetColumn( field ).c_str() );		}
	virtual float	GetFloatColumn( const _String& key, const _String& field )		{ return atof( GetColumn( key, field ).c_str() );	}
	*/
};

template< typename _Elem = char, typename _Pr = std::less< std::basic_string< _Elem > > >
struct XRecordSet : public IRecordSet< _Elem >
{
	XRecordSet() : m_pRecords( 0 )/*, m_Cursor( 0 )*/
	{
		m_nKeyColumn = 0;
		FirstCursor();
		// m_Cursor = m_Index.end();
	}

	virtual ~XRecordSet()
	{
		if ( m_pRecords ) delete [] m_pRecords;
	}

	virtual size_t GetRowCount() const
	{
		if ( m_pRecords == 0 ) return 0;

		return m_pRecords[0].size();		
	}

	virtual bool SetCursorAtRow( int row )
	{
		if ( row < 0 || row >= int(GetRowCount()) )
			return false;

		m_nCursor		= row;
		m_strCursor		= m_pRecords[m_nKeyColumn][m_nCursor];
		return true;
	}

	virtual int GetCurrentRow() const
	{
		return int(m_nCursor);
	}

	virtual int GetRow( const _String& key )
	{
		std::map< _String, size_t, _Pr >::iterator it = m_Index.find( key );
		if ( it == m_Index.end() )
			return -1;
		return int(it->second);
	}

	virtual bool SetCursor( const _String& key )		
	{ 
		return (key == m_strCursor) ? true : SetCursorAtRow( GetRow( key ) ); 
	}

	virtual bool FirstCursor()
	{
		m_nCursor = 0;
		if ( m_pRecords && m_pRecords[m_nKeyColumn].size() > m_nCursor )
		{
			m_strCursor = m_pRecords[m_nKeyColumn][m_nCursor];
		}
		else
		{
			m_strCursor.clear();
		}

		return true;
	}

	virtual bool NextCursor()
	{
		if ( m_nCursor == GetRowCount()-1 )
			return false;
		m_strCursor = m_pRecords[m_nKeyColumn][++m_nCursor];
		return true;
	}

	virtual bool PrevCursor()
	{
		if ( m_nCursor == 0 )
			return false;
		m_strCursor = m_pRecords[m_nKeyColumn][--m_nCursor];
		return true;
	}

    virtual const _Elem* GetColumn( const _String& field )
	{
		int idx = getColumnIndex( field );
        if ( idx < 0 ) return NULL;
		if ( m_pRecords->size() <= m_nCursor ) return NULL;
		return m_pRecords[idx][m_nCursor].c_str() ;
	}

    const _Elem* GetColumn( const _String& key, const _String& field )
	{
		int idx = getColumnIndex( field );
		std::map< _String, size_t, _Pr >::iterator it = m_Index.find( key );
		if ( it == m_Index.end() ) return NULL;
		if ( m_pRecords->size() <= (*it).second ) return NULL;
		return m_pRecords[idx][(*it).second].c_str();
	}

	bool SetColumn( const _String& field, const _String& value )
	{
		int idx = getColumnIndex( field );
		if ( idx < 0 ) return false;
		if ( m_pRecords->size() <= m_nCursor ) return false;
		m_pRecords[idx][m_nCursor] = value;
		return true;
	}

	bool SetColumn( const _String& key, const _String& field, const _String& value )
	{
		int idx = getColumnIndex( field );
		std::map< _String, size_t, _Pr >::iterator it = m_Index.find( key );
		if ( it == m_Index.end() ) return false;
		if ( m_pRecords->size() <= (*it).second ) return false;
		m_pRecords[idx][(*it).second] = value;
		return true;
	}

	bool SetPrimaryKey( const _String& name )
	{
		int index = getColumnIndex( name );
		if ( index == m_nKeyColumn )
			return true;
		if ( index >= 0 )
		{
			m_nKeyColumn = index;
			m_strCursor = m_pRecords[m_nKeyColumn][m_nCursor];

			m_Index.clear();
			for ( size_t i = 0, n = GetRowCount(); i < n; ++i )
			{
				m_Index[ m_pRecords[m_nKeyColumn][i] ] = i;
			}
			return true;
		}
		return false;
	}

	void SetColumnName( size_t idx, const _String& name, bool bIsPrimaryKey = false )
	{
		m_ColumnIndex[ name ] = idx;

		if ( bIsPrimaryKey ) m_nKeyColumn = idx;
	}

	bool AppendRow( const _String& key )
	{
		if ( !m_pRecords )
		{
			assert( 0 );
			return false;
		};

		size_t nColumnCnt = m_ColumnIndex.size();
		size_t nRowCnt = GetRowCount();

		if ( nRowCnt == 0 )
			return false;

		if ( m_Index.find( key ) != m_Index.end() )
			return false;

		for ( size_t i = 0; i < nColumnCnt; ++i )
		{
			m_pRecords[i].push_back( i == m_nKeyColumn ? key : "" );
		}

		m_Index[ key ] = nRowCnt;
		SetCursorAtRow( int(nRowCnt) );
		return true;
	}

	bool RemoveCurrentRow()
	{
		if ( !m_pRecords )
		{
			assert( 0 );
			return false;
		};

		if ( m_nCursor == 0 || m_pRecords->size() <= m_nCursor ) 		// field name 행은 지울수 없다
			return false;

		size_t nColumnCnt = m_ColumnIndex.size();

		m_Index.erase( m_pRecords[m_nKeyColumn][m_nCursor] );

		for ( size_t i = 0; i < nColumnCnt; ++i )
		{
			m_pRecords[i].erase( m_pRecords[i].begin() + m_nCursor );
		}

		for ( size_t i = m_nCursor, n = GetRowCount(); i < n; ++i )		// 당겨지는 row 들의 index 수정
		{
			m_Index[ m_pRecords[m_nKeyColumn][i] ] = i;
		}

		return true;
	}

	template< typename INPUT_STREAM >
	bool LoadFrom( INPUT_STREAM& fs, const _Elem* key_field = NULL )
	{
		_String str1, str2;

		bool bInit = false;

		std::vector< _String > vList;
		m_Index.clear();

		_Elem seps[2] = { '\t', 0 };

		m_nKeyColumn = 0;
		size_t	cnt = 0;

		size_t	filed_cnt = 0;

		while ( !fs.eof() )
		{	
			std::getline( fs, str1 );
			if( str1.empty() ) break;
            if ( !str1.empty() && str1[ str1.size() -1 ] == _Elem('\r') ) str1.erase( str1.begin() + str1.size() - 1, str1.end() );

			// 일단 tab 으로 나눠야함.
			if ( filed_cnt == 0 )
			{
				vList.erase( vList.begin(), vList.end() );
				nsl::split< _Elem, std::vector< _String > >( str1.c_str(), &vList, seps, false, true );
			}
			else
			{
				vList.resize( filed_cnt );
				_String::iterator begin = str1.begin();
				size_t i = 0;
				for ( ; i < filed_cnt; ++i )
				{
					_String::iterator it = std::find( begin, str1.end(), _Elem('\t') );
					
					vList[i].assign( begin, it );
					if ( it == str1.end() ) break;
					begin = it + 1;
				}

				if( i+1 != filed_cnt ) 
					vList.clear();
			}
			
			if ( !bInit )
			{
				if ( m_pRecords ) delete [] m_pRecords;

				m_pRecords = new std::vector< _String >[ vList.size() ];

				for ( size_t i = 0; i < vList.size(); ++i )
				{
					SetColumnName( i, vList[i] );
					if ( key_field != 0 && vList[i] == _String( key_field ) ) 
						m_nKeyColumn = i;
				}

				bInit = true;

				filed_cnt = vList.size();
				continue;
			}

			if ( !vList.empty() ) 
				addData( &vList.front(), filed_cnt );

			++cnt;
		}

		return true;
	}

	template< typename OUTPUT_STREAM >
	void SaveTo( OUTPUT_STREAM& fs )
	{
		if ( !m_pRecords )
			return;

		size_t nColumnCnt = m_ColumnIndex.size();
		std::vector< const _String* > columns( nColumnCnt );

		for ( std::map< _String, size_t, _Pr >::iterator it = m_ColumnIndex.begin(); it != m_ColumnIndex.end(); ++it )
		{
			columns[ (*it).second ] = &(*it).first;
		}

		fs.write( columns[0]->c_str(), std::streamsize( columns[0]->length() * sizeof(_Elem) ) );
		for ( size_t i = 1; i < nColumnCnt; ++i )
		{
			fs.put( _Elem('\t') );
			fs.write( columns[i]->c_str(), std::streamsize( columns[i]->length() * sizeof(_Elem) ) );
		}
		fs.put( _Elem('\n') );

		size_t nRowCnt = GetRowCount();

		for ( size_t row = 0; row < nRowCnt; ++row )
		{
			fs.write( m_pRecords[0][row].c_str(), std::streamsize( m_pRecords[0][row].length() * sizeof(_Elem) ) );
			for ( size_t i = 1; i < nColumnCnt; ++i )
			{
				fs.put( _Elem('\t') );
				fs.write( m_pRecords[i][row].c_str(), std::streamsize( m_pRecords[i][row].length() * sizeof(_Elem) ) );
			}
			fs.put( _Elem('\n') );
		}
	}

protected:

	int getColumnIndex( const _String& name )
	{
		std::map< _String, size_t, _Pr >::iterator it = m_ColumnIndex.find( name );

		if ( it == m_ColumnIndex.end() ) return -1;

		return static_cast< int >( (*it).second );
	}

	void addData( const _String* data, size_t cnt )
	{
		if ( !m_pRecords ) 
		{
			assert( 0 );
			return;
		}

		size_t nColumnCnt = m_ColumnIndex.size();
		if ( cnt != nColumnCnt )
		{
			assert( 0 );
			return;
		}

		for ( size_t i = 0; i < nColumnCnt; ++i )
		{
			m_pRecords[i].push_back( data[i] );
		}

		m_Index[ data[ m_nKeyColumn ] ] = m_pRecords[0].size()-1;
	}

	size_t											m_nKeyColumn;
	std::map< _String, size_t, _Pr >				m_Index;
	std::map< _String, size_t, _Pr >				m_ColumnIndex;
	std::vector< _String >*							m_pRecords;

	_String											m_strCursor;
	size_t											m_nCursor;
};

// 호환을 위해 남겨뒀음 ;
template< typename _Elem = char, typename _Pr = std::less< std::basic_string< _Elem > > >
struct XTextRecordSet : public XRecordSet< _Elem, _Pr >
{
	XTextRecordSet( const _Elem* file_name, const _Elem* key_field ) : XRecordSet< _Elem, _Pr >()
	{
		LoadFrom( std::basic_ifstream< _Elem >( file_name, std::ios_base::in | std::ios_base::binary ) );

		FirstCursor();
	}

	XTextRecordSet( const _Elem* buffer ) : XRecordSet< _Elem, _Pr >()
	{
		LoadFrom( std::basic_istringstream< _Elem >( buffer ) );

		FirstCursor();
	}

	virtual ~XTextRecordSet()
	{
	}
};