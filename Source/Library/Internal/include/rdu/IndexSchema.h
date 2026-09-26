
#pragma once

#include <vector>
#include <string>

#include "StatusFlag.h"
#include "SchemaCommon.h"


struct	INDEX_SCHEMA
{
	struct	COLUMN_INFO
	{
		enum _STATUS_FLAG
		{
			STATUS_FLAG_DESCENDING		= 1,	// 내림차순 여부(true:내림차순, false:오름차순)
		};

		COLUMN_INFO();
		COLUMN_INFO( const char* _pszName, bool _bIsDescending );
		COLUMN_INFO( const COLUMN_INFO& rhs );

		std::string								strName;	// 인덱스된 컬럼 이름
		CStatusFlag< StatusFlagRawDataType >	StatusFlag;	// _STATUS_FLAG 타입의 비트셋 조합
	};

	enum _TYPE
	{
		TYPE_UNKNOWN				= -1,

		TYPE_INDEX					= 0,		// 일반 인덱스
		TYPE_PRIMARY_KEY			= 1,		// PRIMARY KEY 용 인덱스
		TYPE_UNIQUE_CONSTRAINT		= 2,		// UNIQUE 제약 조건
		TYPE_UNIQUE_INDEX			= 3,		// UNIQUE 인덱스
	};

	enum _STATUS_FLAG
	{
		STATUS_FLAG_CLUSTERED		= 1,	// 클러스터 여부
	};

	INDEX_SCHEMA();
	INDEX_SCHEMA( short _nID, _TYPE _eType, bool _bIsClustered, const char* _pszName );
	INDEX_SCHEMA( const INDEX_SCHEMA& rhs );
	~INDEX_SCHEMA();

	short									nID;			// 인덱스 ID
	_TYPE									eType;			// 인덱스 Type
	CStatusFlag< StatusFlagRawDataType >	StatusFlag;		// _STATUS_FLAG 타입의 비트셋 조합
	std::string								strName;		// 인덱스 이름
	std::vector< COLUMN_INFO* >				vColumnInfo;	// 인덱스된 컬럼 정보 벡터
};
