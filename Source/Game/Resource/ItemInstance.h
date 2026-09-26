#pragma once

#include <mmo/ArType.h>
#include <toolkit/XFlag.h>

#include "ItemBase.h"
#include "GameType.h"
#include "UID.h"
#include "VersionDefs.h"

struct ItemInstance
{
	ItemInstance()
		: UID( 0 )
		, Code( 0 )
		, nLevel( 1 )
		, nEnhance( 1 )
		, nCount( 1 )
		, nOwnerUID( 0 )
		, nIdx( 0 )
		, nAuctionID( 0 )
		, nItemKeepingID( 0 )
		, nOwnSummonUID( 0 )
		, nCurrentEndurance( 0 )
		, nCurrentEtherealDurability( 0 )
		, GenerateInfo( BY_UNKNOWN )
		, OwnerHandle( 0 )
		, OwnSummonHandle( 0 )
		, tExpire( 0 )
		, cElementalEffectType( 0 )
		, tElementalEffectExpire( 0 )
		, nAppearanceCode( 0 )
		, nSummonCode( 0 )
#ifdef USE_ITEMEFFECT_96
		, nEffectID( 0 )
#endif
	{
		memset( Socket, 0, sizeof( Socket ) );
	}

	enum
	{
		INSTANCE_FLAG_SIZE		= 1	// int 1개 사이즈 라는 뜻
	};

	typedef XFlag< int, INSTANCE_FLAG_SIZE >	InstanceFlag;
	
	AR_HANDLE			OwnerHandle;
	AR_HANDLE			OwnSummonHandle;

	ItemUID				UID;
	ItemUID				PreviousUID;
	ItemBase::ItemCode	Code;

	int					nIdx;

	int					nLevel;		// 레벨     1 ~ 5
	int					nEnhance;	// 강화도	1 ~ 20

	int					nOwnerUID;
	int					nOwnSummonUID;
	int					nAuctionID;
	int					nItemKeepingID;

	__int64				nCount;

	int					nCurrentEtherealDurability;	// 에테리얼 내구도
	int					nCurrentEndurance;			// 내구성
	
	time_t				tExpire;

	// 기간제 속성 이펙트 관련
	unsigned char		cElementalEffectType;
	time_t				tElementalEffectExpire;
	int					nElementalEffectAttackPoint;
	int					nElementalEffectMagicPoint;
	
	// 형상 변환 관련
	ItemBase::ItemCode	nAppearanceCode;

	int					nSummonCode;
	int					nEffectID;

	InstanceFlag		Flag;
	enum
	{
		ITEM_FLAG_NONE				= -1,	// 아무 플래그도 없음(플래그 인덱스가 아니라 상수임)
		ITEM_FLAG_CARD				= 0,	// 카드화 되었음
		ITEM_FLAG_FULL				= 1,	// 슬롯에 무언가 들어있음
		ITEM_FLAG_INSERTED			= 2,	// 다른 아이템의 슬롯이 들어가 있음
		ITEM_FLAG_FAILED			= 3,	// 강화 실패작
		ITEM_FLAG_EVENT				= 4,	// event item
		ITEM_FLAG_CONTAIN_PET		= 5,	// 펫이 들어 있음
		ITEM_FLAG_SUMMON_DURABILITY	= 6,	// 가장 최근의 사망으로 인해 강화된 소환수의 내구도가 감소된 상태
		ITEM_FLAG_LOCKED			= 7,
		ITEM_FLAG_ENHANCE			= 8,
		ITEM_FLAG_PROTECTED			= 9,
		ITEM_FLAG_FARMED_SUMMON		= 27,	// 농장에 맡겨진 소환수
		ITEM_FLAG_NURSED_SUMMON		= 28,	// 돌본 소환수
		ITEM_FLAG_TAMING			= 29,	// 소환수 테이밍에 사용되고 있음(드랍/사망드랍/거래/노점/기부/경매 불가)
		ITEM_FLAG_NON_CHAOS_STONE	= 30,	// 
		ITEM_FLAG_SUMMON			= 31,	// 소환수가 들어 있다네~~!
	};

	enum		GenerateCode
	{
		BY_MONSTER			= 0,	// 몹이 떨굼
		BY_MARKET			= 1,	// 상점에서 구입
		BY_QUEST			= 2,	// 퀘스트로 얻음
		BY_SCRIPT			= 3,	// 스크립트
		BY_MIX				= 4,	// 조합
		BY_GM				= 5,	// GM
		BY_BASIC			= 6,	// 캐릭 생성시 제공되는 아이템
		BY_TRADE			= 7,	// 트레이드시 쪼개져서 생성됨
		BY_DIVIDE			= 8,	// 쪼개짐
		BY_ITEM				= 10,	// 아이템으로 생성
		BY_FIELD_PROP		= 11,	// 필드 프랍에 의해 생성
		BY_AUCTION			= 12,	// 경매장에 의해 생성
		BY_SHOVELING		= 13,	// 펫 삽질에 의해 생성
		BY_HUNTAHOLIC		= 14,	// 헌터홀릭 보상으로 생성
		BY_DONATION_REWARD	= 15,	// 기부 보상으로 생성
		BY_SKILL			= 16,	// 스킬 사용으로 생성
		BY_DECOMPOSE		= 17,	// 분해에 의해 생성
		BY_TAMING			= 18,	// 테이밍에 의해 생성
		BY_CASH				= 19,	// 크루샵박스에서 수령
		BY_UNKNOWN			= 126,	// 알수 없음
	};
	GenerateCode		GenerateInfo;

	ItemBase::ItemWearType		nWearInfo;	// 입은 위치. 기본적으로 ItemWearType 이 들어간다. 1000*n 이면 n번째 소환수가 입고 있다는 뜻.

	ItemBase::ItemCode	Socket[ ItemBase::MAX_SOCKET_NUMBER ];

	/*
	소켓      | 장비아이템 | 스킬카드 | 소환수카드 |
	----------+------------+----------+------------+
	0         | 아이템코드 | 대상 PID | 소환수SID  |		PID : 플레이어 UID
	1         | 아이템코드 | 대상 SID |            |		SID : 소환수   UID
	2         | 아이템코드 |          |            |
	3         | 아이템코드 |          |            |
	*/

	enum
	{
		MAX_AWAKEN_NUMBER			= 5,
		MAX_RANDOM_OPTION_NUMBER	= 7,
	};

	enum RANDOM_TYPE
	{
		AWAKEN		= 0,
		IDENTIFIED	= 1,
		MAX,
	};

	struct OPTION_INFO
	{
		int nType;			// 성능종류
		c_fixed10 fValue1;	// 성능값1
		c_fixed10 fValue2;	// 성능값2
	};

	struct RANDOM_OPTION
	{
		RANDOM_OPTION()
		{
			nSID = 0;
			nRandomType = 0;
			memset( OptionInfo, 0, sizeof( OptionInfo ) );
		}

		int	nSID;											// SID
		int nRandomType;									// 분류
		OPTION_INFO OptionInfo[ MAX_RANDOM_OPTION_NUMBER ];	// OPTION_INFO
	};

	RANDOM_OPTION				RandomOption[ MAX ];
};