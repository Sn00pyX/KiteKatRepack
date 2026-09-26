
#pragma once


#include <string>

#include "StatusFlag.h"
#include "SchemaCommon.h"


struct	COLUMN_SCHEMA
{
	// eXUserType 변수에 세팅될 값
	enum _USERTYPE
	{
		USERTYPE_UNKNOWN			= -1,

		USERTYPE_TINYINT			= 48,
		USERTYPE_SMALLINT			= 52,
		USERTYPE_INT				= 56,
		USERTYPE_SMALLDATETIME		= 58,
		USERTYPE_REAL				= 59,
		USERTYPE_DATETIME			= 61,
		USERTYPE_FLOAT				= 62,
		USERTYPE_BIT				= 104,
		USERTYPE_DECIMAL			= 106,
		USERTYPE_BIGINT				= 127,
		USERTYPE_VARCHAR			= 167,
		USERTYPE_CHAR				= 175,
		USERTYPE_TIMESTAMP			= 189,
		USERTYPE_NVARCHAR			= 231,
		USERTYPE_NCHAR				= 239,
	};

	// StatusFlag 변수에 세팅될 비트셋 인덱스 값
	enum _STATUS_FLAG
	{
		STATUS_FLAG_NULLABLE					= 0,	// NULL 값 허용 여부
		STATUS_FLAG_HAS_DEFAULT_VALUE			= 1,	// 기본 값 사용 여부
		STATUS_FLAG_IDENTITY					= 2,	// Identity 컬럼 여부
		STATUS_FLAG_NOT_FOR_REPLICATION			= 3,	// Identity 컬럼일 때 복제 금지 여부(Set: 금지, Un-set: 허용)
	};

	static bool	IsSupportedColumnType( int nXUserType );


	COLUMN_SCHEMA();
	COLUMN_SCHEMA(	const char* _pszName,
					_USERTYPE _eXUserType,
					unsigned short _nLength,
					unsigned char _nXPrec,
					unsigned char _nXScale,
					bool _bIsNullable,
					const char* _pszDefaultValue = NULL,
					const char* _pszCollation = NULL );

	const char*	GetDefaultValue()	const;
	void		SetDefaultValue( const char* pszDefaultValue );

	bool		IsNullable()	const;

	std::string								strName;				// 컬럼 이름
	_USERTYPE								eXUserType;				// 컬럼 타입 ID
	unsigned short							nLength;				// 컬럼 데이터 길이(단위: Byte)
	char									nXPrec;					// 실수형 데이터의 정수부 정밀도(단위: 자리수)
	char									nXScale;				// 실수형 데이터의 소수부 정밀도(단위: 자리수)
	CStatusFlag< StatusFlagRawDataType >	StatusFlag;				// _STATUS_FLAG 타입의 비트셋 조합
	std::string								strDefaultValue;		// 컬럼 기본 값(StatusFlag.IsOn( STATUS_HAS_DEFAULT_VALUE ) 가 true일 때만 사용됨)
	std::string								strCollation;			// 컬럼 Collation 이름

};
