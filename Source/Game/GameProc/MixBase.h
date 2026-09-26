#pragma once

enum
{
	MIX_CHECK_ITEM_GROUP						= 1,	// 아이템 대분류가 지정 수치와 동일
	MIX_CHECK_ITEM_CLASS						= 2,
	MIX_CHECK_ITEM_ID							= 3,
	MIX_CHECK_ITEM_RANK							= 4,
	MIX_CHECK_ITEM_LEVEL						= 5,
	MIX_CHECK_INSTANCE_FLAG_ON					= 6,
	MIX_CHECK_INSTANCE_FLAG_OFF					= 7,

	MIX_CHECK_ENHANCE_MATCH						= 8,
	MIX_CHECK_ENHANCE_DISMATCH					= 9,
	MIX_CHECK_ITEM_COUNT						= 10,
	MIX_CHECK_ELEMENTAL_EFFECT_MATCH			= 11,
	MIX_CHECK_ELEMENTAL_EFFECT_MISMATCH			= 12,
	MIX_CHECK_ITEM_WEAR_POSITION_MATCH			= 13,
	MIX_CHECK_ITEM_WEAR_POSITION_MISMATCH		= 14,
	MIX_CHECK_ITEM_COUNT_GE						= 15,	// 아이템 개수가 지정 수량 이상

	MIX_CHECK_ITEM_ETHEREAL_DURABILITY_E		= 16,	// 에테리얼 내구도가 지정 수치와 동일
	MIX_CHECK_ITEM_ETHEREAL_DURABILITY_NE		= 17,	// 에테리얼 내구도가 지정 수치와 다름
	MIX_CHECK_ITEM_GRADE						= 18,	// 아이템 등급이 지정 수치와 동일

	MIX_CHECK_SAME_ITEM_ID						= 19,	// 지정된 슬롯의 아이템과 동일한 아이템 코드(보조 재료 상호 참조형)
	MIX_CHECK_SAME_SUMMON_CODE					= 20,	// 지정된 슬롯의 아이템과 동일한 소환수 코드(보조 재료 상호 참조형)
	MIX_CHECK_ITEM_EXPIRED_TIME_GE				= 21,	// 기간제 아이템의 남은 시간이 지정 수치보다 크거나 같음
	MIX_CHECK_ITEM_EXPIRED_TIME_LE				= 22,	// 기간제 아이템의 남은 시간이 지정 수치보다 작거나 같음
	MIX_CHECK_ITEM_FIRST_SOCKET_CODE_MATCH		= 23,	// 첫번째 소켓의 코드가 지정 수치와 동일
	MIX_CHECK_SAME_ITEM_ENHANCE					= 24,	// 지정된 슬롯의 아이템과 동일한 강화(보조 재료 상호 참조형)
	MIX_CHECK_SAME_SKILL_ID						= 25,	// 지정된 슬롯의 아이템과 동일한 스킬 ID(보조 재료 상호 참조형)

	MIX_CHECK_ITEM_MAX_ETHEREAL_DURABILITY_E	= 26,	// 최대 에테리얼 내구도가 지정 수치와 동일
	MIX_CHECK_ITEM_MAX_ETHEREAL_DURABILITY_NE	= 27,	// 최대 에테리얼 내구도가 지정 수치와 다름

	MIX_CHECK_ITEM_FIRST_SOCKET_CODE_G			= 28,	// 첫번째 소켓의 코드가 지정 수치보다 작음(보조 재료 상호 참조형, 소켓의 코드에는 일반적으로 대소 비교의 의미가 없지만 벨트 소켓 비교를 위해 추가)
	MIX_CHECK_ITEM_FIRST_SOCKET_CODE_L			= 29,	// 첫번째 소켓의 코드가 지정 수치보다 작음(보조 재료 상호 참조형, 소켓의 코드에는 일반적으로 대소 비교의 의미가 없지만 벨트 소켓 비교를 위해 추가)
	MIX_CHECK_ITEM_TYPE							= 30,	// 아이템 타입이 지정 수치와 동일
	MIX_CHECK_SAME_ITEM_CLASS					= 31,	// 지정된 슬롯의 아이템과 동일한 중분류(보조 재료 상호 참조형)
	MIX_CHECK_ITEM_GROUP_NE						= 32,	// 아이템 대분류가 지정 수치와 다름
	MIX_CHECK_INCLUDE_RACE_LIMIT				= 33,	// 지정된 슬롯의 아이템의 직업 제한에 포함 관계(허용되는 직업 제한의 예. 아수라⊂아수라, 아수라⊂전종족, 아수라⊂(아수라 or 데바))
	MIX_CHECK_AWAKEN_ITEM						= 34,	// 각성 대상 아이템의 상태 확인

	MIX_CHECK_SAME_SUMMON_RATE					= 35,	// 지정된 슬롯의 아이템과 동일한 소환수 등급
	MIX_CHECK_BASE_FLAG_ON						= 36,
	MIX_CHECK_DESIGNER_DEFIEND_TYPE				= 37,	// 기획자가 지정한 룰에 만족하는지 체크
	MIX_CHECK_BASE_FLAG_OFF						= 38,
	MIX_CHECK_ENHANCE_GE						= 39,
	MIX_CHECK_ENHANCE_LE						= 40,
	MIX_CHECK_SAME_ITEM_GROUP					= 41,
	MIX_CHECK_LOCALE							= 50,
};

enum
{
	DESIGNER_TYPE_WEAPON						= 0,	// 무기류
	DESIGNER_TYPE_ARMOR							= 1,	// 방어구류 (강화 가능한 모든 방어구)
	DESIGNER_TYPE_WEAR_PRIMARY					= 2,
	DESIGNER_TYPE_WEAR_ALL						= 3,
};

struct MaterialInfo
{
	static const int	MATERIAL_INFO_COUNT		= 5;

	int			type[ MATERIAL_INFO_COUNT ];
	int			value[ MATERIAL_INFO_COUNT ];
};

struct MixBase
{
	enum
	{
		MIX_ENHANCE																= 101,
		MIX_ENHANCE_SKILL_CARD													= 102,
		MIX_ENHANCE_WITHOUT_FAIL												= 103,
		MIX_ENHANCE_CREATURE_CARD												= 104,
		MIX_ENHANCE_CREATURE_CARD_WITH_JOKER									= 105,
		MIX_ENHANCE_SKILL_CARD_WITHOUT_FAIL										= 106,
		MIX_ULTIMATE_ENHANCE													= 111,
		MIX_SET_LEVEL															= 201,
		MIX_SET_LEVEL_CREATE_ITEM												= 202,
		MIX_UNK203																= 203,
		MIX_SET_LEVEL_SET_FLAG													= 211,
		MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM										= 212,
		MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL				= 213,
		MIX_SET_LEVEL_ON_SUB_MATERIAL_LEVEL_SET_FLAG							= 214,
		MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL_SET_ZERO	= 215,
		MIX_ADD_LEVEL															= 301,
		MIX_ADD_LEVEL_CREATE_ITEM												= 302,
		MIX_ADD_LEVEL_SET_FLAG													= 311,
		MIX_ADD_LEVEL_SET_FLAG_CREATE_ITEM										= 312,
		MIX_SET_LEVEL_FROM_MATERIAL												= 313,
		MIX_RECYCLE																= 401,
		MIX_RECYCLE_ENHANCE														= 402,
		MIX_RESTORE_ENHANCE_SET_FLAG											= 501,
		MIX_CREATE_ITEM															= 601,
		MIX_REPLACE_WITH														= 602,
		MIX_CHANGE_APPEARANCE_CODE												= 603,
		MIX_CHANGE_USABLE_PERIOD												= 604,
		MIX_CREATE_ITEM_IN_MASS													= 605,
		MIX_CHANGE_CODE_ADD														= 606,
		MIX_REMOVE_APPEARANCE_CODE												= 607,
		MIX_REPLACE_WITH_RANDOM													= 608,
		MIX_SET_ELEMENTAL_EFFECT												= 701,
		MIX_SET_ELEMENTAL_EFFECT_PARAMETER										= 702,
		MIX_SET_SOCKET															= 703,
		MIX_REPLACE_SOCKET_WITH													= 704,
		MIX_SACRIFICE_ITEM_FOR_ETHEREAL_DURABILITY_WITH_MESSAGE					= 801,
		MIX_TRANSMIT_ETHEREAL_DURABILITY										= 802,
		MIX_RECOVER_EXHAUSTED_ETHEREAL_DURABILITY								= 803,
		MIX_SACRIFICE_ITEM_FOR_ETHEREAL_DURABILITY								= 804,
		MIX_SACRIFICE_ITEM_FOR_ETHEREAL_STONE_DURABILITY						= 805,
		MIX_TRANSMIT_ETHEREAL_DURABILITY_FROM_ETHEREAL_STONE					= 806,
		MIX_AWAKEN_ITEM															= 807,		// 아이템 각성
		MIX_DELETE_AWAKEN_OPTION												= 808,		// 아이템 각성 해제
		MIX_UNK_810																= 810,
		MIX_ITEM_IDENTIFY_FOR_RANDOM_OPTION										= 901,		// 아이템 랜덤화
		MIX_UNK_902																= 902,
		MIX_UNK_903																= 903,
		MIX_UNK_904															= 904,
	};

	// 소켓과 관련된 조합의 후 처리 정책
	enum
	{
		RESULT_SOCKET_DO_NOTHING	= 0,
		RESULT_SOCKET_ERASE			= 1,
		RESULT_SOCKET_RESET			= 2,
	};

	static const int		VALUE_COUNT				= 6;
	static const int		MAX_SUB_MATERIAL_COUNT	= 9;
	static const int		MAX_CREATABLE_ITEM		= 1000;

	int				id;
	int				type;
	int				value[ VALUE_COUNT ];
	int				sub_material_cnt;

	MaterialInfo	main_material;
	MaterialInfo	sub_material[ MAX_SUB_MATERIAL_COUNT ];
};

struct DecomposeBase
{
	enum
	{
		DECOMPOSE_CREATE_ITEM				= 101,
	};

	int						id;				// 분해식 ID
	int						type;			// 유형 ID
	int						neededPrice;	// 필요돈

	int						value[ MixBase::VALUE_COUNT ]; // 조합유형에 따른 값 

	MaterialInfo	material;		// 재료 식별 형태
};
