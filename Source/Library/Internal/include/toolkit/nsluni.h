//
//  File:       nsluni.h
//
//  Contents:   wrapper of iconv unicode library and some other utility
//
//  Functions:  nsl::uni::init
//				nsl::uni::deinit
//				nsl::uni::set_default_encoding
//				nsl::uni::get_default_encoding
//				nsl::uni::conv
//				nsl::uni::proc_korean_postfix
//				nsl::uni::get_unicode_group
//				nsl::uni::get_unicode_group_name
//
//  Author:     Kang, KiHyun ( testors@nflavor.com )
//

#pragma once

#include  <string>

namespace nsl
{

	namespace uni
	{

	bool			init( const char *default_encoding = "CP949" );
	bool			deinit();
	bool			set_default_encoding( const char *encoding );
	const char*		get_default_encoding();
	/// 2011.06.15 리소스용 인코딩 - prodongi
	void			set_resource_encoding( const char *encoding );
	const char*		get_resource_encoding();

	bool			conv( const char *encoding, const std::string & source,  std::wstring & target );
	bool			conv( const char *encoding, const std::wstring & source, std::string & target  );
	bool			conv( const std::string & source,  std::wstring & target );
	bool			conv( const std::wstring & source, std::string & target  );

	bool			conv( const char *source_encoding, const char *target_encoding, void* source, size_t source_len, std::vector< char > * pvResult );
	
	std::wstring	conv( const std::string & source  );
	std::string		conv( const std::wstring & source );

	bool			proc_korean_postfix( std::string & strString  );
	bool			proc_korean_postfix( std::wstring & strString );

	enum CodeGroup
	{
		T_NON = 0,
		T_ARA,	///< Arabic
		T_ARM,	///< Armenian
		T_BLN,	///< Spaces
		T_BNG,	///< Bengali
		T_CAN,	///< Canadian Syllabics
		T_CHE,	///< Cherokee
		T_CJK,	///< Chinese
		T_COP,	///< Coptic
		T_CTK,	///< Control Characters
		T_CYR,	///< Cyrillic
		T_DEV,	///< Devanagari
		T_DIG,	///< Digits
		T_ETH,	///< Ethiopic
		T_GEO,	///< Georgian
		T_GRE,	///< Greek
		T_GUJ,	///< Gujarati
		T_GUR,	///< Gurmukhi
		T_HAN,	///< Hangul
		T_HEB,	///< Hebrew
		T_JPN,	///< Japanese
		T_KAN,	///< Kannada
		T_KHM,	///< Khmer
		T_LAO,	///< Lao
		T_LAT,	///< Latin
		T_MAL,	///< Malayalam
		T_MON,	///< Mongolian
		T_MYA,	///< Myanmar
		T_OGH,	///< Ogham
		T_ORI,	///< Oriya
		T_RES,	///< Reserved
		T_RUN,	///< Runic
		T_SIN,	///< Sinhala
		T_SPC,	///< Special Characters
		T_SYM,	///< Symbols
		T_SYR,	///< Syriac
		T_TAM,	///< Tamil
		T_TEL,	///< Telugu
		T_THI,	///< Thai
		T_THN,	///< Thaana
		T_TIB,	///< Tibetan
		T_YIS,	///< Yi Syllables
	};

	CodeGroup	get_unicode_group( wchar_t c );
	const char *get_unicode_group_name( wchar_t c );

	};
};



// pointer out-parameter version.


namespace nsl
{
	namespace uni
	{
		inline bool conv( const char *encoding, const std::string & source,  std::wstring *target )
		{
			return conv( encoding, source, *target );
		}

		inline bool conv( const char *encoding, const std::wstring & source, std::string *target  )
		{
			return conv( encoding, source, *target );
		}

		inline bool conv( const std::string & source,  std::wstring *target )
		{
			return conv( source, *target );
		}

		inline bool conv( const std::wstring & source, std::string *target  )
		{
			return conv( source, *target );
		}


		inline bool proc_korean_postfix( std::string *strString  )
		{
			return proc_korean_postfix( *strString );
		}

		inline bool proc_korean_postfix( std::wstring *strString )
		{
			return proc_korean_postfix( *strString );
		}
	};
};