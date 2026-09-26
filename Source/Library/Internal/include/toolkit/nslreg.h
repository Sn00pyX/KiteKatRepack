//
//  File:       nslreg.h
//
//  Contents:   wrapper of GRETA regular expression library
//
//  Functions:  nsl::regex::match
//              nsl::regex::replace
//
//  Author:     Kang, KiHyun ( testors@nflavor.com )
//

//  . Define REGEX_WIDE_AND_NARROW if you plan to match wide and narrow strings in the same executable.
//  . This forces the instantiation of both the wide and narrow versions of the pattern matching routines.
//  . Note that this will make your executable bigger.


#pragma once

#include <string>
#include <greta/regexpr2.h>

namespace nsl
{
	namespace regex
	{
		enum FLAGS
		{
			NOFLAGS       = ::regex::NOFLAGS,
			NOCASE        = ::regex::NOCASE,		///< ignore case
			GLOBAL        = ::regex::GLOBAL,		///< match everywhere in the string
			MULTILINE     = ::regex::MULTILINE,		///< ^ and $ can match internal line breaks
			SINGLELINE    = ::regex::SINGLELINE,	///< . can match newline character
			RIGHTMOST     = ::regex::RIGHTMOST,		///< start matching at the right of the string
			NOBACKREFS    = ::regex::NOBACKREFS,	///< only meaningful when used with GLOBAL and substitute
			FIRSTBACKREFS = ::regex::FIRSTBACKREFS, ///< only meaningful when used with GLOBAL
			ALLBACKREFS   = ::regex::ALLBACKREFS,	///< only meaningful when used with GLOBAL
			NORMALIZE     = ::regex::NORMALIZE,		///< Preprocess patterns: "\\n" => "\n", etc.
			EXTENDED      = ::regex::EXTENDED,		///< ignore whitespace in pattern
		};


		template< typename IT, typename STR, typename RESULT >
		inline bool match( const STR & pattern, const STR & str, RESULT & result, FLAGS flag = NOFLAGS )
		{	
			::regex::basic_rpattern< IT >						p( pattern, static_cast< ::regex::REGEX_FLAGS >( flag ) );
			::regex::basic_match_results< IT >::backref_type	r;

			r = p.match( str, result );

			return r.matched;
		}

		template< typename _Elem >
		inline bool match( const _Elem * pattern, const _Elem * str, FLAGS flag = NOFLAGS )
		{
			typedef ::regex::detail::select< REGEX_FOLD_INSTANTIATIONS &&
					 ::regex::detail::is_convertible< _Elem const *, std::basic_string<_Elem>::const_iterator >::value,
					 std::basic_string< _Elem >::const_iterator,
					 _Elem const * >::type IT;

			::regex::basic_match_results< IT > result;

			return match< IT, const _Elem * >( pattern, str, result, flag );
		}

		template< typename _Elem >
		inline bool match( const std::basic_string< _Elem > & pattern, const std::basic_string< _Elem > & str, FLAGS flag = NOFLAGS )
		{
			typedef std::basic_string< _Elem >::const_iterator IT;

			::regex::basic_match_results< IT > result;

			return match< IT, std::basic_string< _Elem >, ::regex::basic_match_results< IT > >( pattern, str, result, flag );
		}

		template< typename _Elem, typename _Cont >
		inline bool match( const _Elem * pattern, const _Elem * str, _Cont & cont, FLAGS flag = NOFLAGS )
		{
			typedef ::regex::detail::select< REGEX_FOLD_INSTANTIATIONS &&
					 ::regex::detail::is_convertible< _Elem const *, std::basic_string<_Elem>::const_iterator >::value,
					 std::basic_string< _Elem >::const_iterator,
					 _Elem const * >::type IT;

			::regex::basic_match_results< IT > result;

			if ( !match< IT, const _Elem * >( pattern, str, result, flag ) ) return false;

			for ( size_t i = 0; i < result.cbackrefs(); ++i )
			{
				size_t begin = result.rstart( i );
				cont.push_back( std::basic_string< _Elem >( str + begin, str + begin + result.rlength( i ) ) );
			}

			return true;
		}

		template< typename _Elem, typename _Cont >
		inline bool match( const std::basic_string< _Elem > & pattern, const std::basic_string< _Elem > & str, _Cont & cont, FLAGS flag = NOFLAGS )
		{
			return match( pattern.c_str(), str.c_str(), cont, flag );
		}



		template< typename IT, typename _Elem, typename STR >
		inline size_t replace( const STR & from, const STR & to, std::basic_string< _Elem > & str, FLAGS flag = NOFLAGS )
		{
			::regex::basic_rpattern< IT >	p( from, to, static_cast< ::regex::REGEX_FLAGS >( flag ) );
			::regex::basic_subst_results< _Elem > result;

			return p.substitute( str, result );
		}

		template< typename _Elem >
		inline size_t replace( const std::basic_string< _Elem > & from, const std::basic_string< _Elem > & to, std::basic_string< _Elem > & str, FLAGS flag = NOFLAGS )
		{
			typedef std::basic_string< _Elem >::const_iterator IT;

			return replace< IT, _Elem, std::basic_string< _Elem > >( from, to, str, flag );
		}

		template< typename _Elem >
		inline size_t replace( const _Elem * from, const _Elem * to, std::basic_string< _Elem > & str, FLAGS flag = NOFLAGS )
		{
			typedef std::basic_string< _Elem >::const_iterator IT;

			return replace< IT, _Elem, std::basic_string< _Elem > >( std::basic_string< _Elem >( from ), std::basic_string< _Elem >( to ), str, flag );
		}
	};
};


// pointer out-parameter version. deprecated for inline inefficiency - by Tyburn


namespace nsl
{
	namespace regex
	{
		template< typename IT, typename STR, typename RESULT >
		inline bool match( const STR & pattern, const STR & str, RESULT * result, FLAGS flag = NOFLAGS )
		{	
			return match( pattern, str, *result, flag );
		}

		template< typename _Elem, typename _Cont >
		inline bool match( const _Elem * pattern, const _Elem * str, _Cont * pCont, FLAGS flag = NOFLAGS )
		{
			return match( pattern, str, *pCont, flag );
		}

		template< typename _Elem, typename _Cont >
		inline bool match( const std::basic_string< _Elem > & pattern, const std::basic_string< _Elem > & str, _Cont * pCont, FLAGS flag = NOFLAGS )
		{
			return match( pattern.c_str(), str.c_str(), *pCont, flag );
		}

		template< typename IT, typename _Elem, typename STR >
		inline size_t replace( const STR & from, const STR & to, std::basic_string< _Elem > * str, FLAGS flag = NOFLAGS )
		{
			return replace< IT, _Elem, std::basic_string< _Elem > >( from, to, *str, flag );
		}

		template< typename _Elem >
		inline size_t replace( const std::basic_string< _Elem > & from, const std::basic_string< _Elem > & to, std::basic_string< _Elem > * str, FLAGS flag = NOFLAGS )
		{
			typedef std::basic_string< _Elem >::const_iterator IT;

			return replace< IT, _Elem, std::basic_string< _Elem > >( from, to, *str, flag );
		}

		template< typename _Elem >
		inline size_t replace( const _Elem * from, const _Elem * to, std::basic_string< _Elem > * str, FLAGS flag = NOFLAGS )
		{
			typedef std::basic_string< _Elem >::const_iterator IT;

			return replace< IT, _Elem, std::basic_string< _Elem > >( std::basic_string< _Elem >( from ), std::basic_string< _Elem >( to ), str, flag );
		}
	};
};