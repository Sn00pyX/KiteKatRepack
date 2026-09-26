#pragma once

const int L_CHARACTER_NAME_LEN		= 24;

const int ITEM_COUNT				= 0;
const int NPC_COUNT					= 500;

// Each description is on ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/intl/unicode_81rn.htm
enum CODEPAGE
{
	CP_THAILAND			= 874,		// ANSI/OEM - Thai (same as 28605, ISO 8859-15)
	CP_JAPAN			= 932,		// ANSI/OEM - Japanese, Shift-JIS
	CP_CHINA			= 936,		// ANSI/OEM - Simplified Chinese (PRC, Singapore)
	CP_KOREA			= 949,		// ANSI/OEM - Korean (Unified Hangeul Code)
	CP_HONGKONG			= 950,		// ANSI/OEM - Traditional Chinese (Taiwan; Hong Kong SAR, PRC)
	CP_CENTRAL_EUROPE	= 1250,		// ANSI - Central European(Poland)
	CP_RUSSIA			= 1251,		// ANSI - Cyrillic
	CP_WEST_EUROPE		= 1252,		// ANSI - Latin I(France, German, Italy)
	CP_TURKEY			= 1254,		// ANSI - Turkish
	CP_MIDEAST			= 1256		// ANSI - Arabic
};

// LM_AUTO_USER_CHECKED 로그의 n10에 오토 체크 원인 코드로 사용
namespace AUTO_USER_CHECK_TYPE
{
	enum _AUTO_USER_CHECK_TYPE
	{
		UNKNOWN									= 0,

		AUTO_OPEN_ONLY							= 1,
		AUTO_USER_LOGIN							= 2,
		ZERO_ITEM_DROP_ATTEMPTION				= 3,
		INVALID_SECURITY_SOLUTION_RESPONSE		= 4,
		BLOCKING_INIT_INCOMPLETE_CLIENT			= 6,
		BY_SCRIPT								= 7,
		HACK_DETECTED_FROM_CLIENT				= 8,	// 클라에서 해킹 툴 검출을 알려 온 경우
		HACK_DETECTED_FROM_CLIENT_2				= 9,	// 클라에서 해킹 툴 검출을 알려 온 경우(계정에 대한 오토 체크 로그, 8번으로 로그 남기므로 사용되지 않음)
		MULTIPLE_LOGIN_DETECTED					= 10,	// 1개의 계정으로 다수의 로그인이 시도된 경우
		MIX_BUFFER_OVERRUN_ATTEMPTION			= 11,	// 조합 패킷 처리부의 결함을 이용한 버퍼 오버런 공격
		DELETING_OTHERS_CHARACTER_ATTEMPTION	= 12,	// 자신의 계정에 속해있지 않은 캐릭터를 삭제하려고 시도
		DECOMPOSE_BUFFER_OVERRUN_ATTEMPTION		= 13,	// 분해시 맞지않는 패킷 보냄
	};
};

// LM_CHARACTER_RESURRECTION 로그의 n3에 사용되는 부활 유형으로 사용
enum _CHARACTER_RESURRECTION_TYPE
{
	CRT_NORMAL					= 0,	// 일반 부활
	CRT_BATTLE					= 1,	// 대련장 대련 부활(구 대련, 사용되지 않음)
	CRT_COMPETE					= 2,	// 대련 부활
	CRT_SKILL					= 3,	// 부활 스킬에 의한 부활(경험치 복구 없음)
	CRT_SKILL_WITH_RECOVER		= 4,	// 부활 스킬에 의한 부활(경험치 복구 조금 있음)
	CRT_ITEM					= 5,	// 아이템에 의한 부활
	CRT_STATE					= 6,	// 지속효과에 의한 부활
	CRT_FAIRY_POTION			= 7,	// 대모요정의 병에 의한 부활
	CRT_HUNTAHOLIC				= 8,	// 헌터홀릭 내부 또는 로비에서 부활
	CRT_BATTLE_ARENA			= 9,	// 배틀 아레나 내부 부활
};
