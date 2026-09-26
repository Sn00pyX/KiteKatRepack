
#pragma once

#include <string>
#include <vector>



template< typename char_t >
class	csv_traits
{
public:

	bool	is_field_term( char_t check_char ) const;
	bool	is_enclosed_term( char_t check_char ) const;
	bool	is_line_term( char_t check_char ) const;

};


template<>
class	csv_traits< char >
{
public:

	enum { EOF_CHAR = 0 };

	typedef	std::string		string_t;
	typedef	char			char_t;

	explicit csv_traits( const char_t* field = ",", char_t enclosed = '"', char_t line = '\n' )
		: field_( field )
		, enclosed_( enclosed )
		, line_( line )
	{
	}

	bool	is_field_term( char_t check_char ) const
	{
		return (field_.find( check_char ) != string_t::npos);
	}

	bool	is_enclosed_term( char_t check_char ) const
	{
		return (check_char == enclosed_);
	}

	bool	is_line_term( char_t check_char ) const
	{
		return (check_char == line_);
	}

private:

	string_t	field_;
	char_t		enclosed_;
	char_t		line_;

};

template<>
class	csv_traits< wchar_t >
{
public:

	enum { EOF_CHAR = 0 };

	typedef	std::wstring	string_t;
	typedef	wchar_t			char_t;

	explicit csv_traits( const char_t* field = L",", char_t enclosed = L'"', char_t line = L'\n' )
		: field_( field )
		, enclosed_( enclosed )
		, line_( line )
	{
	}

	bool	is_field_term( char_t check_char ) const
	{
		return (field_.find( check_char ) != string_t::npos);
	}

	bool	is_enclosed_term( char_t check_char ) const
	{
		return (check_char == enclosed_);
	}

	bool	is_line_term( char_t check_char ) const
	{
		return (check_char == line_);
	}

private:

	string_t	field_;
	char_t		enclosed_;
	char_t		line_;

};


template< typename char_t >
class	csv_stream
{
public:

	virtual	bool	eof() = 0;
	virtual char_t	get() = 0;
	virtual	char_t	seek_next() = 0;

};

template< typename char_t >
class	csv_string_stream : public csv_stream< char_t >
{
public:

	csv_string_stream( const char_t* string, size_t len )
		: string_( string )
		, len_( len )
		, pos_( 0 )
	{
	}
	csv_string_stream( typename const csv_traits< char_t >::string_t& string )
		: string_( string.c_str() )
		, len_( string.size() )
		, pos_( 0 )
	{
	}

	virtual	bool	eof()
	{
		return (len_ <= pos_);
	}

	virtual char_t	get()
	{
		char_t temp = get( pos_ );
		if( temp != csv_traits< char_t >::EOF_CHAR )
		{
			++pos_;
		}

		return temp;
	}

	virtual	char_t	seek_next()
	{
		return get( pos_ );
	}

private:

	virtual char_t	get( size_t pos )
	{
		char_t temp = csv_traits< char_t >::EOF_CHAR;
		if( pos < len_ )
		{
			temp = string_[pos];
		}

		return temp;
	}

private:

	const char_t*	string_;
	size_t			len_;
	size_t			pos_;

};


template< typename char_t >
class	csv_parser
{
public:

	typedef	std::vector< typename csv_traits< char_t >::string_t >	csv_row_t;

	static csv_row_t get_row( csv_stream< char_t >& stream, const csv_traits< char_t >& traits )
	{
		csv_row_t row;
		get_row( row, stream, traits );
		return row;
	}

	static void	get_row( csv_row_t& row, csv_stream< char_t >& stream, const csv_traits< char_t >& traits )
	{
		enum
		{
			PARSE_NOMAL,
			PARSE_ENCLOSE,
		};

		int state = PARSE_NOMAL;
		csv_traits< char_t >::string_t field;

		while( true )
		{
			char_t curr_char = stream.get();
			if( curr_char == csv_traits< char_t >::EOF_CHAR )
			{
				if( field.empty() == false )
				{
					row.push_back( field );
				}

				break;
			}

			if( state == PARSE_ENCLOSE )	// 문자열이 닫히기전
			{
				if( traits.is_enclosed_term( curr_char ) == true )
				{
					char_t escaped_char = get_escape_char( stream, traits );
					if( escaped_char != csv_traits< char_t >::EOF_CHAR )
					{
						field.push_back( escaped_char );
						continue;
					}

					//문자열 끝
					state = PARSE_NOMAL;
					continue;
				}

				// 무조건 넣는다.
				field.push_back( curr_char );
			}
			else
			{
				if( traits.is_enclosed_term( curr_char ) == true )	// 처음 enclosed 문자가 왔을 때 문자열 시작
				{
					if( field.empty() )
					{
						state = PARSE_ENCLOSE;
					}
					else
					{
						// 뜬금없이 enclosed_ 이 왔을 경우
						// escape 상황일때
						char_t escaped_char = get_escape_char( stream, traits );
						if( escaped_char != csv_traits< char_t >::EOF_CHAR )
						{
							field.push_back( escaped_char );
							continue;
						}

						// escape 상황이 아니면 그냥 추가
						field.push_back( curr_char );
					}
				}
				else if( traits.is_field_term( curr_char ) == true )
				{
					// field 완성
					row.push_back( field );
					field.clear();
				}
				else if( traits.is_line_term( curr_char ) == true )
				{
					// row 완성
					row.push_back( field );
					field.clear();
					break;
				}
				else
				{
					field.push_back( curr_char );
				}
			}
		}
	}

private:

	static char_t	get_escape_char( csv_stream< char_t >& stream, const csv_traits< char_t >& traits )
	{
		char_t next_char = stream.seek_next();
		if( traits.is_enclosed_term( next_char ) == true )
		{
			// 탈출문자
			return stream.get();
		}

		return csv_traits< char_t >::EOF_CHAR;
	}

};
