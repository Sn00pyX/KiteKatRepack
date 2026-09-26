#pragma once

/*

	ItemBase

	by Testors 2004-05-13


	. ItemCode 와 ItemUID 의 차이점.

		ex1) 바스타드 소드의 ItemCode 는 1, 가죽 갑옷의 Item Code 는 2 이다.
		ex2) 바스타드 소드가 두자루 있을경우 각각은 다른 ItemUID 을 갖는다.

*/

#include <string>
#include <vector>

#include <toolkit/XFlag.h>

#include "StructGold.h"
#include "ItemEffect.h"
#include "VersionDefs.h"

struct StructDuraConvert
{
	int nClass;
	int nCnt;
	int nWear;
	int anCodes[10];
};

// 아이템 컨텐츠 정보
struct ItemBase
{
	typedef	int				ItemCode;
	typedef XFlag<int>		BaseFlag;

	enum
	{
		ITEM_CODE_WEARED_BULLET	= 100001,
	};

	enum
	{
		ITEM_CODE_BEGINNERS_BAG						= 490001,	// 수련자의 가방(기본 지급 아이템)
		ITEM_CODE_STAMINA_SAVER						= 900000,	// 스테미너 세이버
		ITEM_CODE_STAMINA_SAVER_NOT_FOR_SALE		= 900010,	// 스테미너 세이버(비매품 - 거래 불가)
		ITEM_CODE_STAMINA_SAVER_FOR_NOVICE			= 900011,	// 초보자의 스테미너 세이버(초보자 지원용)
		ITEM_CODE_STAMINA_SAVER_OF_LOVE				= 2000203,	// 사랑의 스테미너 세이버(발렌타인 데이 이벤트 보상용)
		ITEM_CODE_FAIRY_POTION						= 910004,	// 요정의 병(사용 안 함)
		ITEM_CODE_GRAND_FAIRY_POTION				= 910005,	// 대모요정의 병
		ITEM_CODE_GRAND_FAIRY_POTION_BY_PLAYPOINT	= 2902042,	// 대모요정의 병(플레이 포인트 샵 판매용)
		ITEM_CODE_GRAND_FAIRY_POTION_NOT_FOR_SALE	= 2010454,	// 대모요정의 병(비매품)<09년국내 출석 이벤트>
		ITEM_CODE_RETURN_FEATHER					= 910006,	// 귀환의 깃털
		ITEM_CODE_RETURN_FEATHER_SECRET_DUNGEON		= 910080,	// 귀환의 깃털: 숨겨진 던전 마스터
		ITEM_CODE_RETURN_BACK_FEATHER				= 910009,	// 복귀의 깃털
		ITEM_CODE_EVENT_RETURN_FEATHER				= 2902104,	// 레벨업 달성 이벤트 보상 귀환의 깃털
		ITEM_CODE_EVENT_RETURN_FEATHER2				= 2012099,	// 귀환의 깃털 (이벤트용)
		ITEM_CODE_RUPPI_RETURN_FEATHER				= 9000098,	// 테스트 서버 크루템 일반 판매용 귀환의 깃털
		ITEM_CODE_FAIRY_LOCK						= 920005,	// 요정의 자물쇠
		ITEM_CODE_SECROUTE_WARP_TICKET				= 910023,	// 시크루트 귀환권
		ITEM_CODE_HUNTAHOLIC_POINT_TICKET			= 806808,	// 헌터홀릭 포인트 카드
		ITEM_CODE_RENAME_CHARACTER_TYPE1			= 930040,	// 캐릭터 이름 변경 신청서(친구/차단 등록 삭제용)
		ITEM_CODE_RENAME_CHARACTER_TYPE2			= 930042,	// 캐릭터 이름 변경 신청서(친구/차단 등록 유지/갱신용)
		ITEM_CODE_RENAME_CHARACTER_TYPE3			= 2010459,	// 아바타명 변경 서비스권(비매품)<09년국내 출석 이벤트>
		ITEM_CODE_RENAME_CHARACTER_TYPE4			= 2012645,	// 아바타명 변경권<live><2012><서버통합건>
		ITEM_CODE_MIRROR_OF_TAMING_CARD				= 960019,	// 테이밍 카드의 거울(테이밍 성공/실패 시 소모, 실패 시에 카드 보호)<캐시템>
		ITEM_CODE_MIRROR_OF_TAMING_CARD_TRADABLE	= 960021,	// 테이밍 카드의 거울(테이밍 성공/실패 시 소모, 실패 시에 카드 보호)<거래가능>
		ITEM_CODE_MIRROR_OF_TAMING_CARD_ON_TEST		= 9000111,	// 테이밍 카드의 거울(테이밍 성공/실패 시 소모, 실패 시에 카드 보호)<테스트서버용>
		ITEM_CODE_INGAME_POINT						= 806809,	// 인게임 포인트 아이템
		ITEM_CODE_INGAME_POINT2						= 806815,	// 인게임 포인트 아이템
		ITEM_CODE_INGAME_POINT3						= 806816,	// 인게임 포인트 아이템
		ITEM_CODE_JOKER								= 540070,	// 조커 카드
		ITEM_CODE_CREATURE_TAMING_SCROLL			= 491008,	// 크리처 테이밍 주문서(크리처 테이밍 스킬을 동작시키지만 쿨타임 무시)
		ITEM_CODE_CREATURE_TAMING_SCROLL_EVENT		= 2012861,	// 크리처 테이밍 주문서(이벤트용. 거래불가)
		ITEM_CODE_CREATURE_TAMING_SCROLL_EVENT2		= 2013145,	// 크리처 테이밍 주문서(이벤트용. 캐릭귀속)
		 
	};

#ifdef USE_CLIENT97
	enum
	{
		MAX_COOLTIME_GROUP = 40,
		MAX_OPTION_NUMBER = 4,
		MAX_SOCKET_NUMBER = 4,
		MAX_ITEM_NAME_LENGTH = 32,
		MAX_ITEM_REPAIR = 12,
		MAX_ITEM_WEAR = 36, // 24
		MAX_ITEM_WEAR_LOGIN = 24,
		MAX_SPARE_ITEM_WEAR = 32, // 28
	};

#else
	enum
	{
		MAX_COOLTIME_GROUP = 40,
		MAX_OPTION_NUMBER = 4,
		MAX_SOCKET_NUMBER = 4,
		MAX_ITEM_NAME_LENGTH = 32,
		MAX_ITEM_REPAIR = 12,
		MAX_ITEM_WEAR = 28, // 24
		MAX_ITEM_WEAR_LOGIN = 28,
		MAX_SPARE_ITEM_WEAR = 28, // 28
	};

#endif

	ItemCode	nCode;					// 아이템 코드
	int			nNameId;				// 이름

	int			nType;					// 아이템 타입
	enum		ITEM_TYPE
	{
		TYPE_ETC			= 0,		// 일반
		TYPE_ARMOR			= 1,		// 장비
		TYPE_CARD			= 2,		// 카드
		TYPE_SUPPLY			= 3,		// 소모품
		TYPE_CUBE			= 4,		// 큐브
		TYPE_CHARM			= 5,		// 부적
		TYPE_USE			= 6,		// 사용형 - 소모되지 않고 쿨타임만
		TYPE_SOULSTONE		= 7,		// 소울스톤
		TYPE_USE_CARD		= 8,
		TYPE_COLLECTION		= 9,
	};

	int			nGroup;					// 그룹 ID
	enum		ITEM_GROUP
	{
		GROUP_ETC						= 0,	// 기타
		GROUP_WEAPON					= 1,	// 무기
		GROUP_ARMOR						= 2,	// 의상
		GROUP_SHIELD					= 3,	// 방패
		GROUP_HELM						= 4,	// 투구
		GROUP_GLOVE						= 5,	// 장갑
		GROUP_BOOTS						= 6,	// 부츠
		GROUP_BELT						= 7,	// 벨트
		GROUP_MANTLE					= 8,	// 망토
		GROUP_ACCESSORY					= 9,	// 악세서리
		GROUP_SKILLCARD					= 10,	// 스킬 카드
		GROUP_ITEMCARD					= 11,	// 유니트 카드
		GROUP_SPELLCARD					= 12,	// 스펠 카드(미구현)
		GROUP_SUMMONCARD				= 13,	// 소환수 카드
		GROUP_FACE						= 15,	// 가면
		GROUP_UNDERWEAR					= 16,	// 속옷
		GROUP_BAG						= 17,	// 가방
		GROUP_PET_CAGE					= 18,	// 펫 우리
		GROUP_BACKPACK					= 19,
		GROUP_STRIKE_CUBE				= 21,	// 스트라이크 큐브
		GROUP_DEFENCE_CUBE				= 22,	// 디펜스 큐브
		GROUP_SKILL_CUBE				= 23,	// 스킬 큐브
		GROUP_RESTORATION_CUBE			= 24,	// 환원의 큐브
		GROUP_EMBLEM					= 25,
		GROUP_SOULSTONE					= 93,	// 소울 스톤
		GROUP_BULLET					= 98,	// 탄환
		GROUP_CONSUMABLE				= 99,	// 소모품
		GROUP_NPC_FACE					= 100,	// NPC용 얼굴
		GROUP_DECO						= 110,	// 꾸미기 아이템
		GROUP_RIDING					= 120,	// 탑승용 아이템
		GROUP_ARTIFACT					= 130,	// 아티팩트 아이템
		GROUP_EQUIPMENT_ON_BELT			= 140,	// 벨트 장착용 보스 카드
	};
	
	enum		ItemClass
	{
		// 미분류
		CLASS_ETC			= 0,

		// 무기
		CLASS_DOUBLE_AXE	= 95,		// 쌍도끼
		CLASS_DOUBLE_SWORD	= 96,
		CLASS_DOUBLE_DAGGER	= 98,
		CLASS_EVERY_WEAPON	= 99,
		CLASS_ETCWEAPON		= 100,		// 기타무기
		CLASS_ONEHAND_SWORD	= 101,		// 한손검
		CLASS_TWOHAND_SWORD	= 102,		// 양손검
		CLASS_DAGGER		= 103,		// 단검
		CLASS_TWOHAND_SPEAR	= 104,		// 창
		CLASS_TWOHAND_AXE	= 105,		// 양손도끼
		CLASS_ONEHAND_MACE	= 106,		// 한손메이스
		CLASS_TWOHAND_MACE	= 107,		// 양손메이스
		CLASS_HEAVY_BOW		= 108,		// 헤비 보우
		CLASS_LIGHT_BOW		= 109,		// 라이트 보우
		CLASS_CROSSBOW		= 110,		// 석궁
		CLASS_ONEHAND_STAFF	= 111,		// 한손지팡이
		CLASS_TWOHAND_STAFF	= 112,		// 양손지팡이
		CLASS_ONEHAND_AXE	= 113,		// 한손도끼
		//CLASS_DOUBLE_WEAPON = 199,

		// 방어구
		CLASS_ARMOR			= 200,		// 공용의상
		CLASS_FIGHTER_ARMOR	= 201,		// 전사용
		CLASS_HUNTER_ARMOR	= 202,		// 헌터용
		CLASS_MAGICIAN_ARMOR= 203,		// 마법사용
		CLASS_SUMMONER_ARMOR= 204,		// 소환사용

		//
		CLASS_SHIELD		= 210,		// 공용방패
		CLASS_HELM			= 220,		// 공용투구
		CLASS_BOOTS			= 230,		// 공용부츠
		CLASS_GLOVE			= 240,		// 공용장갑
		CLASS_BELT			= 250,		// 공용벨트
		CLASS_MANTLE		= 260,		// 공용망토
		CLASS_QUIVER		= 265,		// 화살통
		
		// 악세사리
		CLASS_ETC_ACCESSORY	= 300,		// 기타악세사리
		CLASS_RING			= 301,		// 링
		CLASS_EARRING		= 302,		// 귀걸이
		CLASS_ARMULET		= 303,		// 목걸이
		CLASS_EYEGLASS		= 304,		// 안경
		CLASS_MASK			= 305,		// 수염
		CLASS_CUBE			= 306,		// 큐브

		// 포스칩, 소울칩, 루나칩
		CLASS_BOOST_CHIP	= 400,

		// 소울스톤
		CLASS_SOULSTONE		= 401,
		CLASS_CREATURE_FOOD	= 402,
		CLASS_FARM_PASS		= 403,

		// 에테리얼 스톤(충전용 배터리)
		CLASS_ETHEREAL_STONE		= 451,

		// 꾸미기 아이템
		//CLASS_DECO_WEAPON			= 600,		// 꾸미기 무기, 사용하지 않음(무기 종류별 상세 구분 ID로 대체, 609~621)
		CLASS_DECO_SHIELD			= 601,		// 꾸미기 방패
		CLASS_DECO_ARMOR			= 602,		// 꾸미기 갑옷
		CLASS_DECO_HELM				= 603,		// 꾸미기 투구
		CLASS_DECO_GLOVE			= 604,		// 꾸미기 장갑
		CLASS_DECO_BOOTS			= 605,		// 꾸미기 부츠
		CLASS_DECO_MALTLE			= 606,		// 꾸미기 망토
		CLASS_DECO_SHOULDER			= 607,		// 꾸미기 어깨(날개)
		CLASS_DECO_HAIR				= 608,		// 꾸미기 헤어
		CLASS_DECO_ONEHAND_SWORD	= 609,		// 꾸미기 한손검
		CLASS_DECO_TWOHAND_SWORD	= 610,		// 꾸미기 양손검
		CLASS_DECO_DAGGER			= 611,		// 꾸미기 단검
		CLASS_DECO_TWOHAND_SPEAR	= 612,		// 꾸미기 양손창
		CLASS_DECO_TWOHAND_AXE		= 613,		// 꾸미기 양손도끼
		CLASS_DECO_ONEHAND_MACE		= 614,		// 꾸미기 한손메이스
		CLASS_DECO_TWOHAND_MACE		= 615,		// 꾸미기 양손메이스
		CLASS_DECO_HEAVY_BOW		= 616,		// 꾸미기 헤비 보우
		CLASS_DECO_LIGHT_BOW		= 617,		// 꾸미기 라이트 보우
		CLASS_DECO_CROSSBOW			= 618,		// 꾸미기 석궁
		CLASS_DECO_ONEHAND_STAFF	= 619,		// 꾸미기 한손지팡이
		CLASS_DECO_TWOHAND_STAFF	= 620,		// 꾸미기 양손지팡이
		CLASS_DECO_ONEHAND_AXE		= 621 ,  	// 꾸미기 한손도끼

		CLASS_ELEMENT_EFFECT			= 700,		// 속성이펙트
		CLASS_ELEMENT_ONEHAND_PHYSICAL	= 701,		// 속성이펙트공격력강화 (한손)
		CLASS_ELEMENT_ONEHAND_MAGICAL	= 702,		// 속성이펙트마력강화 (한손)
		CLASS_ELEMENT_TWOHAND_PHYSICAL	= 703,		// 속성이펙트공격력강화 (양손)
		CLASS_ELEMENT_TWOHAND_MAGICAL	= 704		// 속성이펙트마력강화 (양손)
	};
	ItemClass	nClass;					// 계열

	int			nSetID;					// 세트 아이디
	int			nSetPartFlag;			// 세트 파트 플래그

	unsigned char	nGrade;				// 등급(에테리얼 내구도 관련)
	int			nRank;					// 랭크

	int			nLevel;					// 아이템 생성시 기본 레벨
	int			nEnhance;				// 아이템 생성시 기본 강화

	int			nSocketCount;

	int			nInstanceFlag;

	char		nJobDepth;				// 직업 차수( 비트셋형태, 0차 직업 = 1, 1차 직업 = 2, 2차 직업 = 4, 3차 직업 = 8 )

	int			nMinLevel;
	int			nMaxLevel;

	int			nTargetMinLevel;
	int			nTargetMaxLevel;

	int			nRange;					// 사정거리
	float		fWeight;				// 무게
	StructGold	nPrice;					// 상점 판매가 - 루피(플레이어 입장 구매가)
	int			nHuntaholicPoint;		// 상점 판매가 - 헌터홀릭 포인트(플레이어 입장 구매가)
	int			nArenaPoint;			// 상점 판매가 - 아레나 포인트(플레이어 입장 구매가)
	int			nEtherealDurability;	// 에테리얼 내구도
	int			nEndurance;				// 내구성

	enum		ItemWearType
	{	
		WEAR_CANTWEAR			= -1,	// 장비불가
		WEAR_NONE				= -1,	// 장비 하지 않음

		WEAR_WEAPON				= 0,	// 오른손전용    (방패라던가)
		WEAR_SHIELD				= 1,	// 왼손전용
		WEAR_ARMOR				= 2,	// 갑옷
		WEAR_HELM				= 3,	// 투구
		WEAR_GLOVE				= 4,	// 장갑
		WEAR_BOOTS				= 5,	// 부츠
		WEAR_BELT				= 6,	// 벨트
		WEAR_MANTLE				= 7,	// 망토
		WEAR_ARMULET			= 8,	// 목걸이
		WEAR_RING				= 9,	// 반지		
										// 10 은 반지 두번째 슬롯을 위해 예약
		WEAR_EAR				= 11,	// 귀걸이
		WEAR_FACE				= 12,	// 눈
		WEAR_BACKPACK			= 13,	// 입
		WEAR_DECO_WEAPON		= 14,
		WEAR_DECO_SHIELD		= 15,
		WEAR_DECO_ARMOR			= 16,
		WEAR_DECO_HELM			= 17,
		WEAR_DECO_GLOVE			= 18,
		WEAR_DECO_BOOTS			= 19,
		WEAR_DECO_MANTLE		= 20,
		WEAR_DECO_SHOULDER		= 21,
		WEAR_RIDE_ITEM			= 22,
		WEAR_BAG_SLOT			= 23,


		WEAR_BOOSTER			= 24,
		WEAR_EMBLEM				= 25,
		WEAR_EAR2				= 26,
		WEAR_CHAOS_STONE		= 27,

		WEAR_SPARE_WEAPON		= 28,
		WEAR_SPARE_SHIELD		= 29,
		WEAR_SPARE_DECO_WEAPON	= 30,
		WEAR_SPARE_DECO_SHIELD	= 31,
		
		WEAR_TWOFINGER_RING		= 94,	// 반지 두칸 소모
		WEAR_TWOHAND			= 99,	// 투핸드

		WEAR_SKILL				= 100,	// 스킬
		WEAR_SUMMON_ONLY		= 200,	// 크리처 전용 장비

		WEAR_SECOND_RING		= 10,
		WEAR_RIGHTHAND			= WEAR_WEAPON,	// 오른손전용    
		WEAR_LEFTHAND			= WEAR_SHIELD,	// 왼손전용		(방패라던가)
		WEAR_BULLET				= WEAR_SHIELD,	// 탄환

		WEAR_SPARE_RIGHTHAND	= WEAR_SPARE_WEAPON,	// 오른손전용    
		WEAR_SPARE_LEFTHAND		= WEAR_SPARE_SHIELD,	// 왼손전용		(방패라던가)
		WEAR_SPARE_BULLET		= WEAR_SPARE_SHIELD,	// 탄환
	};
	ItemWearType	WearType;

	enum		LIMIT_FLAG
	{
		LIMIT_DEVA		= (1 << 2),		// 데바
		LIMIT_ASURA		= (1 << 3),		// 아수라
		LIMIT_GAIA		= (1 << 4),		// 가이아

		LIMIT_FIGHTER	= (1 << 10),	// 전사
		LIMIT_HUNTER	= (1 << 11),	// 헌터
		LIMIT_MAGICIAN	= (1 << 12),	// 마법사
		LIMIT_SUMMONER	= (1 << 13),	// 소환사		
	};
	int			nLimit;					// 사용제한
	
	BaseFlag	Flag;
	enum		ITEM_FLAG
	{
		FLAG_CANT_DONATE				= 0,		// 기부 불가 여부
		FLAG_CANT_STORAGE				= 1,		// 창고 집어넣을 수 있는지 없는지 여부
		FLAG_CANT_ENHANCE				= 2,		// 강화 불가
		FLAG_USE						= 3,		// 소모성 사용 가능 여부
		FLAG_CARD						= 4,		// 카드화 가능여부
		FLAG_SOCKET						= 5,		// 소켓삽입가능여부
		FLAG_JOIN						= 6,		// 중첩 가능 여부
		FLAG_TARGET_USE					= 7,		// 타겟 지정 사용 여부
		FLAG_WARP						= 8,		// 사용시 워프하는 아이템임
		FLAG_CANT_TRADE					= 9,		// 트레이드가능여부 (1이면 불가인듯)
		FLAG_CANT_SELL					= 10,		// 상점 매각 가능 여부
		FLAG_QUEST						= 11,		// 퀘스트 아이템 여부
		FLAG_CANT_USE_OVERWEIGHT		= 12,		// 무게 초과시 사용 불가 여부
		FLAG_CASHITEM					= 13,		// 캐쉬템 여부
		FLAG_CANT_USE_RIDING			= 14,		// 라이딩시 사용 불가
		FLAG_CANT_DROP					= 15,		// 드랍 불가
		FLAG_CANT_USE_MOVING			= 16,		// 이동시 사용 불가
		FLAG_QUEST_DISTRIBUTE			= 17,		// 퀘스트 분배 아이템
		FLAG_CANT_USE_SIT				= 18,		// 앉아서 사용 불가
		FLAG_CANT_USE_IN_RAID_SIEGE 	= 19,		// 레이드/시즈 던전 내부에서 사용 불가
		FLAG_CANT_USE_IN_SECROUTE		= 20,		// 시크루트에서 사용 불가
		FLAG_CANT_USE_IN_EVENTMAP		= 21,		// 이벤트 지역에서 사용 불가
		FLAG_CANT_USE_IN_HUNTAHOLIC		= 22,		// 헌터홀릭 로비/던전에서 사용 불가
		FLAG_USABLE_IN_ONLY_HUNTAHOLIC	= 23,		// 헌터홀릭 로비/던전에서만 사용 가능
		FLAG_CANT_USE_IN_DEATHMATCH		= 24,		// 데스매치에서 사용 불가
		FLAG_USABLE_IN_ONLY_DEATHMATCH	= 25,		// 데스매치에서만 사용 가능
		FLAG_NOT_ERASABLE				= 26,		// 아이템 파괴 불가능(0:가능 1:불가능)
		FLAG_USABLE_ON_ONLY_SIT			= 27,		// 앉아서만 사용 가능
		FLAG_CANT_USE_IN_SECRET_DUNGEON	= 28,		// 숨겨진 던전 내에서 사용 불가(0:사용 가능, 1: 사용 불가)
		FLAG_CANT_USE_IN_BATTLE_ARENA	= 29,		// 배틀 아레나 내에서 사용 불가(0:사용 가능, 1: 사용 불가)
		FLAG_CANT_DECOMPOSE				= 30,		// 분해 불가능 여부 (0:분해 가능, 1:분해 불가) 
		FLAG_RANDOMIZABLE				= 31,		// 랜덤화 가능 여부 (0:불가능, 1:가능)
	};

	int			nSummonId;
	int			nThrowRange;
	
	short		nBaseType[MAX_OPTION_NUMBER];
	c_fixed10	fBaseVar1[MAX_OPTION_NUMBER];
	c_fixed10	fBaseVar2[MAX_OPTION_NUMBER];

	short		nOptType[MAX_OPTION_NUMBER];
	c_fixed10	fOptVar1[MAX_OPTION_NUMBER];
	c_fixed10	fOptVar2[MAX_OPTION_NUMBER];

	int			nSkillID;

	int			nStateCode;
	int			nStateLevel;
	int			nStateTime;
	int			nCoolTime;

	short		nCoolTimeGroup;

	int			nArrangeCode;		// 아이템 정렬 기준 (Since Epic 8.3)

	int			available_time;

	enum
	{
		PERMANENT			= 0,
		DECREASE_ON_GAME	= 1,
		DECREASE_ALWAYS		= 2
	};
	char		decrease_type;

	/* 이하 클라이언트 정보
	char szModel[61][6];
	char szDropModel[61];
	int  nIconId;
	*/

	// ItemResource 테이블에서 로드되지 않고 서버 설정/명령어에 따라 세팅됨
	bool		bLogRequiredOnExpiration;
};

struct ItemBaseServer : public ItemBase
{
	const std::vector< EffectInfo * > *		pvEffectList;
	const std::vector< EnhanceEffectInfo * > * pvEnhanceEffectList;
	std::string								strScript;
};