#pragma once

// 전반적인 게임 공식/상수들.
// 헤더에는 아무것도 include 하지 말것.
//
//  percentage 는 0 ~ 1 사이의 값
//
// 2004/05/21	by Testors

#include <string>

#include <mmo/ArType.h>
#include <toolkit/XRandom.h>
#include <toolkit/c_fixed.h>

#include "GameType.h"
#include "StructGold.h"
#include "VersionDefs.h"


namespace GameRule
{
	const AR_UNIT		VISIBLE_RANGE	= 525;
	const AR_UNIT		RUNAWAY_RANGE	= 175;			// 도망형 몬스터의 도망 범위, 시야 거리를 3번에 나눠서 도망친다.
	const int			MAX_STATE_LEVEL = 65535;		// 최대 지속효과 레벨
	const float			DEFAULT_HUMAN_SIZE	 = 1.0f;	// 표준 생물체(인간)의 지름
	const float			DEFAULT_ATTACK_RANGE = 0.5f;	// 기본 사거리 (50센티 정도)
	const float			MAX_ATTACK_RANGE = 0.84f;
	const int			DEFAULT_UNIT_SIZE	= 12;		// 1미터가 좌표상으로 얼마인지를 정의
	const float			SKILL_CAST_COST	= 0.2f;			// 스킬 시전시 20% 소비
	const float			SKILL_FIRE_COST	= 0.8f;			// 스킬 발사시 80% 소비
	const float			SKILL_CANCEL_RATE = 0.005f;		// 캐스팅 캔슬이 일어나기 위한 데미지 비율 (MAXHP 대비) 
	const int			MAX_LEVEL = 300;				// 최대 레벨
	const int			MAX_JOB_DEPTH = 4;				// 0차를 포함한 최대 직업 차수
	const int			MIN_TALENT_POINT_LEVEL = 148;	// 특성포인트 획득 최소 레벨
	const int			MIN_TALENT_POINT_JOB_DEPTH = 3;	// 특성포인트 획득 직업 차수
	const int			DEFAULT_TALENT_POINT = 2;		// 마스터 클래스 전직 시 기본으로 주어지는 특성포인트
	const __int64		MAX_OWNABLE_JP = 99998999999;	// 보유 가능 최대 JP(500억)
										 
	const int			NORMAL_SUMMON_MAX_LEVEL = 60;	// 소환수 기본형 최대 레벨
	const int			GROWTH_SUMMON_MAX_LEVEL = 115;	// 소환수 성장형 최대 레벨
	const int			EVOLVE_SUMMON_MAX_LEVEL = 210;	// 소환수 진화형 최대 레벨
	const int			NORMAL_SUMMON_EVOLVE_LEVEL = 50;// 기본형 진화 레벨
	const int			GROWTH_SUMMON_EVOLVE_LEVEL = 100;// 성장형 진화 레벨

	const int			MAX_SUMMON_LEVEL = 210;			// 크리처 최대 레벨
	const int			MAX_SUMMON_ENHANCE = 5;			// 크리처 최대 강화
	const int			MAX_SUMMON_SKILL_TREE = 5;		// 크리처가 가질 수 있는 최대 스킬트리 개수 (리소스)

	const int			FARM_MAX_LEVEL = 150;			// 농장으로 키울 수 있는 최대 레벨
	const int			FARM_MAX_COUNT = 3;				// 농장에 맡길 수 있는 최대 소환수
	const int			FARM_NON_CASH_MAX_COUNT = 1;	// 일반 이용권으로 맡길 수 있는 최대 소환수의 수

	const float			ATTACK_RANGE_UNIT = 100.0;		// 사정거리 단위. ( 내부적으로는 기획 DB 에서 ATTACK_RANGE_UNIT 를 곱한 값으로 처리된다 )
	const AR_TIME		TAMING_INTERVAL = 30000;		// 테이밍 유지시간은 5분임
	const StructGold	MAX_GOLD_DROP( 1000000 );		// 최대 골드 드랍양
	const int			MAX_FIRST_ATTACK_BONUS_TIME	= 6000;	// 선공 보너스 적용 시간

	const int			SUB_TITLE_RATE_LIMIT = 5;		// 부 호칭에서 사용 가능한 최대 등급
	const int			SUB_TITLE_COUNT = 5;			// 부 호칭의 최대 수
	const float			SUB_TITLE_RATE = 0.1f;			// 부 호칭의 성능 적용률
	const AR_TIME		TITLE_RESET_COOL_TIME = 30000;	// 호칭을 해제하기 위한 쿨타임 5분
	const int			TITLE_CONDITION_MAX_GROUP = 10;	// 호칭 조건의 최대 그룹 수

	const unsigned int	CHAT_YELL_RANGE	= 40;			// 500 / (150 / 12) 반경 500미터
	const unsigned int	CHAT_MAX_LENGTH = 255;			// 최대 채팅 메시지 길이

	const short			DEFAULT_RUSH_SPEED			= 140;	// 러쉬일때 속도오~
	const short			DEFAULT_KNOCK_BACK_SPEED	= 140;	// 넉백일때 속도오~

	const float			UNMOUNT_PENALTY				= 0.05f;	// 말에서 떨어지면 체력 5% 감소
	const int			DEFAULT_UNMOUNT_PROBABILITY_ON_DAMAGE = 30;
	const int			DEFAULT_UNMOUNT_PROBABILITY_ON_CRITICAL_DAMAGE = 75;

	const AR_TIME		CASTING_DELAY_LOW		= 20;	// 캐스팅 딜레이
	const AR_TIME		CASTING_DELAY_MEDIUM	= 50;
	const AR_TIME		CASTING_DELAY_HIGH		= 100;

	const AR_TIME		DEFAULT_MP_DEC_TIME		= 1000;	// 오오라 시전시 MP 소진 시간

	const AR_TIME		BATTLE_WAITING_TIME					= 1000; 				// 대련시 대기시간
	const time_t		COMPETE_ANSWER_WAITTING_DURATION	= 60;					// 대련 요청 후 자동 취소까지 응답 대기 시간(단위:초, 기본값:1분)
	const time_t		COMPETE_COUNTDOWN_DURATION			= 10;					// 대련 수락 후 시작까지 카운트다운 기간(단위:초, 기본값:10초)
	const time_t		COMPETE_DURATION					= 900;					// 대련 시작 후 타임아웃까지 기간(단위:초, 기본값:15분)
	const AR_UNIT		COMPETE_RING_SEMIDIAMETER			= 500;					// 대련 시작 지점으로부터 장외 반지름(단위:AR_UNIT, 기본값:500=약41.6m)
	const c_fixed10		COMPETE_RESURRECT_HP_RECOVER_RATE	= c_fixed10( 0.1 );		// 대련 사망 후 부활 시 회복 HP

	const unsigned int	MAX_TRADE_ITEM_COUNT				= 10;

	const int			CASTING_CANCEL_LOW		= 20;	// 캐스팅 캔슬 확률 (%)
	const int			CASTING_CANCEL_MEDIUM	= 50;	// 캐스팅 캔슬 확률 (%)
	const int			CASTING_CANCEL_HIGH		= 100;	// 캐스팅 캔슬 확률 (%)

	// 길드/연합/던전 재가입/재도전 기간 제한 관련
	const int			GUILD_REJOIN_BLOCK_DURATION_SEC = 86400 * 7;
	const int			ALLIANCE_REJOIN_BLOCK_DURATION_SEC = 86400 * 7;
	const int			GUILD_DUNGEON_RECHALLENGE_BLOCK_DURATION_SEC = 86400 * 7;

	// 던전 입장 레벨 제한 관련
	const int			DUNGEON_ENTER_LEVEL_LIMIT_UPPER_RANGE	= 10;		// 던전 적정 레벨 기준 입장 가능 상위 레벨 범위
	const int			DUNGEON_ENTER_LEVEL_LIMIT_LOWER_RANGE	= 40;		// 던전 적정 레벨 기준 입장 가능 하위 레벨 범위
	const int			DUNGEON_RAID_REQUEST_LEVEL_LIMIT		= 20;		// 던전 레이드 신청 가능 레벨 제한(Ex. [던전레벨-20] 이상)

	// 불카누스 던전
	const int			DUNGEON_VULCANUS_ID						= 20000;

	// 배틀 아레나 관련
	// 연습 경기의 아레나 ID
	const int			BATTLE_ARENA_EXERCISE_GAME_ARENA_ID		= 2501;
	// 1개의 팀에 구성될 수 있는 최대 파티 수(16:16 경기에서 최대 2개의 파티가 1개의 팀이 됨)
	const int			BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM	= 2;
	// 1개의 팀에 참여할 수 있는 최대 유저 수(값 부분 중 * 8 에서 8은 PartyManager::MAX_PARTY_MEMBER)
	const int			BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM	= BATTLE_ARENA_MAX_PARTY_COUNT_PER_TEAM * 8;
	// 1개의 방에 참여할 수 있는 최대 팀 수
	const int			BATTLE_ARENA_MAX_TEAM_COUNT				= 2;
	// 1개의 방에 참여할 수 있는 최대 유저 수(팀 구분 없이 모든 참가자)
	const int			BATTLE_ARENA_MAX_MEMBER_COUNT			= BATTLE_ARENA_MAX_MEMBER_COUNT_PER_TEAM * BATTLE_ARENA_MAX_TEAM_COUNT;
	// 빠른 입장으로 진행 중이던 경기에 참가하는 경우 카운트 다운 시간
	const AR_TIME		BATTLE_ARENA_QUICK_JOIN_COUNTDOWN		= 1000;
	// 경기 종료 후 모든 유저가 점수판을 보고 있는 상태에서 경기장에서 강제 이탈될 때까지의 시간
	const AR_TIME		BATTLE_ARENA_FORCE_QUIT_DELAY			= 6000;
	// 캐릭터당 최대 보유 가능 AP
	const int			BATTLE_ARENA_MAX_OWNABLE_POINT			= 2500000;
	// 경기 진행 중 접속이 끊긴 유저가 다시 경기에 참여할 때까지 기다려주는 시간(기본값 3분)
	extern AR_TIME		nBattleArenaReconnectWaitDuration;
	// MVP가 받는 승패 보상 비율(MVP 외의 유저는 1.0)
	const c_fixed10		BATTLE_ARENA_MVP_REWARD_RATE			= c_fixed10( 12000, true );
	// 잠수 신고 쿨타임(기본값 3분)
	const AR_TIME		BATTLE_ARENA_ABSENCE_CHECK_COOLTIME		= 18000;
	// 잠수 신고 응답 제한 시간
	const AR_TIME		BATTLE_ARENA_ABSENCE_CHECK_ANSWER_LIMIT	= 6000 + 500;
	// 중첩 페널티 카운트로 인해 경기 결과 보상에 페널티를 받는 최소 페널티 카운트
	const int			BATTLE_ARENA_HALF_REWARD_PENALTY_COUNT	= 4;

	// GM 권한 관련
	const int			PERMISSION_FOR_PLAYER				= 0;	// 일반 플레이어
	const int			PERMISSION_FOR_D_GRADE				= 50;	// D 등급
	const int			PERMISSION_FOR_C_GRADE				= 60;	// C 등급
	const int			PERMISSION_FOR_B_GRADE				= 70;	// B 등급
	const int			PERMISSION_FOR_A_GRADE				= 80;	// A 등급
	const int			PERMISSION_FOR_GM					= 100;	// S 등급

	// 피로도 시스템 관련
	const int			DEFAULT_MAX_STAMINA			= 500000;
	const AR_TIME		STAMINA_UPDATE_TIME			= 6000;
	const int			STAMINA_REGEN_IN_TENT		= 120;
	const int			STAMINA_REGEN_ON_GOOD		= 110;
	const int			STAMINA_REGEN_ON_AVERAGE	= 100;
	const int			STAMINA_REGEN_IN_FIELD		= 30;
	const int			BAD_CONDITION_LIMIT			= 10000;
	const int			AVERAGE_CONDITION_LIMIT		= 130000;
	const float			SECROUTE_STAMINA_REGEN_BONUS_RATE	= 2.0f;

	const AR_UNIT		REFLECT_RANGE				= 4 * DEFAULT_UNIT_SIZE;
	
	const int			CHANNEL_PRESERVE_TIME		= 3600;
	// 기공
	const int			ENERGY_MAX					= 10;
	const AR_TIME		DEFAULT_ENERGY_UPKEEP_TIME	= 180000;	// 기공 유지 임시로 하루

	// PK 관련
	const AR_TIME		PK_ON_TIME					= 1000;
	const AR_TIME		PK_OFF_TIME					= 3000;

	const float			ITEM_SELL_RATIO				= 0.25f;

	extern float		fPlyMod;
	extern float		fEXPRate;			// 경치 비율
	extern float		fGoldDropRate;		// 돈 드랍 비율
	extern float		fItemDropRate;		// 아이템 드랍 비율
	extern float		fPartyEXPRate[ 7 ];	// 파티 경치 보너스
	extern float		fChaosDropRate;		// 혼돈 드랍 비율
	extern float		fPVPDamageRateForPlayer;		// PVP 시 적용 데미지 비율(가해자: 플레이어)
	extern float		fPVPDamageRateForSummon;		// PVP 시 적용 데미지 비율(가해자: 소환수)

	extern int			nGuildDonateGold;
	extern int			nGuildBuffMinute;


	extern float		fStaminaBonusRate;						// 스테미너 보너스 혜택 비율(EXP/JP)
	extern float		fSuperSaveBonusRate;					// 성장의 물약(구 슈퍼 세이버) 보너스 혜택 비율(EXP/JP)
	extern int			anSuperSaveLevelMinLimit[ 7 ];			// 성장의 물약(구 슈퍼 세이버) 제한 최소 레벨
	extern int			anSuperSaveLevelMaxLimit[ 7 ];			// 성장의 물약(구 슈퍼 세이버) 제한 최대 레벨
	extern float		fSummonStatminaSaveBonusRate;			// 소환수 스테미너 세이버(오곡 크래커) 보너스 혜택 비율(EXP)

	extern float		fAllyPCBangBonusRate;					// 멤버쉽 PC방 혜택 비율(EXP/JP)
	extern float		fAllyPCBangChaosBonusRate;				// 멤버쉽 PC방 혜택 비율(Lac)
	extern float		fPremiumPCBangBonusRate;				// 더블 플러스 PC방 혜택 비율(EXP/JP)
	extern float		fPremiumPCBangChaosBonusRate;			// 더블 플러스 PC방 혜택 비율(Lac)
	extern float		fPremiumPCBangGoldBonusDropRate;		// 더블 플러스 PC방 혜택 비율(루피 드랍율, 기본값: 1.0)
	extern float		fPremiumPCBangItemBonusDropRate;		// 더블 플러스 PC방 혜택 비율(아이템 드랍율, 기본값: 1.0)
	extern float		fPremiumPCBangChaosBonusDropRate;		// 더블 플러스 PC방 혜택 비율(라크 드랍율, 기본값: 1.0)
	extern bool		bApplyStaminaBonusInPremiumPCBang;		// 더블 플러스 PC방에서 스테미너 효과 적용 허용 여부

	extern bool			bUsePlayPoint;						// 플레이 포인트 누적 여부
	extern int			nPlayPointAccumulateTerm;			// 플레이 포인트 누적 간격
	extern int			nPlayPointAccumulateAmount;			// 한 번의 플레이 포인트 누적시 누적될 포인트 량
	extern float			fPremiumPCBangPlayPointBonusRate;	// 더블 플러스 PC방 혜택 비율(플레이 포인트)

	extern bool			bUseTimeBasedEventScript;				// 스크립트 기반 시간제 이벤트 사용 여부
	extern bool			bUseTimeBasedEventDB;					// DB SP 기반 시간제 이벤트 사용 여부
	extern int			nTermForTimeBasedEventScript;			// 스크립트 기반 시간제 이벤트 발생 간격(분 단위)
	extern int			nTermForTimeBasedEventDB;				// DB SP 기반 시간제 이벤트 발생 간격(분 단위)

	extern bool			bIsNoCollisionCheck;
	extern bool			bSkipLoadingAttribute;

	extern bool			bMonsterWandering;	// 몬스터 방황 여부.-_-
	extern bool			bMonsterCollisionToLine;	// 반드시 시야 확인을 해야 하는 경우
	extern bool			bMonsterPathFinding; // 몬스터 길찾기 여부
	extern bool			bLogMonsterPathFinding; // 몬스터 길찾기 관련한 로직들의 발생 회수를 남긴다 (로깅 여부)
	extern bool			bLogSchedulingStatus; // 스케쥴링 관련 로직들의 발생 회수와 시간을 체크 (로깅 여부)
	extern bool			bIgnoreSkillCoolTime;// 스킬 쿨타임 무시 여부
	extern bool			bIsPKServer;		// PK Server
	extern bool			Hardcore; // hardcore server
	extern float		HardcoreExpRate; // 유저가 죽었을때 pk 서버 대비 경험치 감소율 ( 2.0이면 pk 서버보다 2배 더 경험치가 감소한다)
	extern int			nPKPenaltyLevel;
	extern bool			bDisablePKOn;		// PK On 완전 금지
	extern bool			bIsAdultServer;		// Adult Server
	extern bool			bRestrictSpeicialChar;	// 로그인시 계정에 특수 문자 사용 제한 여부
	extern std::string	strAllowedSpecialChar;	// 로그인시 계정에 허용할 특수 문자
	extern bool			bAutoOpen;

	extern bool			bDisableHuntaholic;					// Huntaholic 가능 여부

	extern bool			bUseAutoJail;						// 무저갱 사용 여부

	extern int			nSecuritySolutionType;				// 보안 솔루션 종류(0: 사용 안 함, 1: 게임가드, 2: 핵실드, 3: X-Trap)
	extern AR_TIME		nPeriodOfSecuritySolutionCheck;		// 보안 솔루션 클라/서버 체크 주기
	extern AR_TIME		nSecuritySolutionResponseTimeout;	// 보안 솔루션 클라/서버 체크 요청 후 응답까지 제한 시간
	extern std::string	strSecuritySolutionExceptionalIP;	// 보안 솔루션 클라/서버 체크 미적용 IP 주소 목록(;으로 구분)

	extern bool			bDisableDungeonRaidSiege;
	extern bool			bIsCashUsableServer;
	extern bool			bUseAccountAuthorityDB;
	extern bool			bCashItemDropable;
	extern bool			bUseAutoTrap;
	extern bool			bBroadcastEventItemPickup;
	extern bool			bUseGuildDonationPoint;
	extern bool			bRestrictBanWordForBooth;
	extern int			nMinBoothStartableLevel;
	extern bool			bLimitBoothOpenableLayerToZero;	// 노점을 0번 레이어에서만 열 수 있도록 제한(수련자의 섬 사기 노점 악용 방지)
	extern bool			bDisableBuyBooth;				// 구매 노점을 사용할 수 없도록 제한하는 기능
	const unsigned int	MAX_BOOTH_ITEM_COUNT	= 8;	// 노점에서 최대로 아이템을 등록할 수 있는 종류 수
	extern bool			bDisableBooth;					// 노점 이용 불가 설정
	extern bool			bDisableTrade;					// 거래 이용 불가 설정

	extern bool			bLimitAdvChatCount;
	extern int			nMinGlobalChatUsableLevel;
	extern int			nMaxStorageItemCount;					// 창고 보관 가능 최대 아이템 수(DB 데이터 수)
	extern int			nMaxCharactersPerAccount;				// 계정당 최대 캐릭터 수

	extern bool			bUseSecurityNo;							// 보안 비밀번호 적용 여부
	extern bool			bUseSecurityNoForStorage;				// 창고 이용 시 보안 비밀번호 적용 여부
	extern bool			bUseSecurityNoForDeletingCharacter;		// 캐릭터 삭제 시 보안 비밀번호 적용 여부
	extern bool			bCheckStorageSecurityAlways;			// 창고 이용 시 보안 비밀번호를 열때 마다 확인 여부

	extern bool			bLimitFieldLogout;						// 블러디/데모니악 아닌 일반 캐릭터가 필드에서 로그아웃할 때 접속 종료 대기시간 적용 여부
	extern AR_TIME		nLogoutTimer;							// 접속 종료 대기시간 기간
	const AR_TIME		LOGOUT_AVAILABLE_DURATION		= 1000;	// 접속 종료 대기시간 체크 완료 시점부터의 로그아웃 가능 기간(타이머 체크 완료 후 10초 경과시 로그아웃 처리 안 됨)
	extern bool			bForceUnregisterAccountOnKickFail;		// 유저 접속 시도 시 킥 처리가 정상적으로 이루어지지 못할 경우 강제 계정 로그인 정보 리셋 여부

	extern bool			bForbiddenScriptInitialized;			// 채팅 창을 통해 입력된 명령어 중 무시할 명령어 목록 초기화 여부
																// 채팅 창을 통해 스크립트 명령어 입력 시 이 값이 false면 설정값을 다시 파싱함(onRunScript 참조)
	extern std::string	strForbiddenScript;						// 채팅 창을 통해 입력된 명령어 중 무시할 명령어 목록(행 구분자 = ';', 행 내의 항목 구분자 = ',')

#ifdef	USE_CLIENT97
	const unsigned int	MAX_ACCOUNT_LEN			= 63;	// 유저 계정 최대 길이
#else
	const unsigned int	MAX_ACCOUNT_LEN = 60;	// 유저 계정 최대 길이
#endif

	extern bool			bUseLoginLogoutDebug;			// 로그인/로그아웃 로그 메시지 기록 기능 사용 여부(디버그용)

	extern bool			bLogVulcanusDungeon;			// 불카누스 던전 로그 여부

	extern bool			bLimitGameTime;					// 게임 중독 방지 시스템 적용 여부
	extern int			nMaxGameTimeLimitedAge;			// 게임 중독 방지 시스템 - 게임 중독 방지 시스템 적용 최대 유저 나이
	extern AR_TIME		nMaxHealthyGameTime;			// 게임 중독 방지 시스템 - 최대 건강 게임시간 제한
	extern AR_TIME		nMaxTiredGameTime;				// 게임 중독 방지 시스템 - 최대 피로 게임시간 제한

	extern int			nEtherealDurabilityBaseConsumptionOnNormalAttack;	// 에테리얼 내구도 평타 공격 시 기본 소모량
	extern int			nEtherealDurabilityBaseConsumptionOnSkillAttack;	// 에테리얼 내구도 스킬 공격 시 기본 소모량(버프 포함)
	extern int			nEtherealDurabilityBaseConsumptionOnDamage;			// 에테리얼 내구도 피격 시 기본 소모량

	extern int			nMaxLevel;			// 최고 레벨

	extern std::string	strLogRequiredStateList;		// 소멸 시 로그를 남겨야 하는 지속효과 ID 목록
	extern std::string	strLogRequiredItemList;			// 기간 만료 시 로그를 남겨야 하는 아이템 ID 목록

	const c_fixed10		MORAL_LIMIT				= c_fixed10( 100 );		// 블러디 캐릭터가 되기 위한 IP 수치(천벌 1Lv)
	const c_fixed10		SEMI_CRIME_LIMIT		= c_fixed10( 500 );		// 천벌 2Lv을 받기위한 IP 수치
	const c_fixed10		CRIME_LIMIT				= c_fixed10( 1000 );	// 데모니악 캐릭터가 되기 위한 IP 수치(천벌 3Lv)

	// 기부 관련
	const char			MAX_DONATE_ITEM_COUNT	= 9;		// 기부 시스템에서 최대 한 번에 기부할 수 있는 서로 다른 종류의 아이템 수
	const StructGold	MIN_DONATE_GOLD			= StructGold( 0 );
	//const int			MIN_DONATE_JP			= 20000;
	const __int64		DONATE_GOLD_UNIT_COUNT	= 10000;
	//const c_fixed10		DONATE_GOLD_PER_JP		= c_fixed10( 0.5 );

	const AR_TIME		ITEM_ARRANGE_COOL_TIME	= 3000;		// 인벤토리/창고 정렬 기능 쿨타임

	const float			BOW_AIMING_TIME_RATIO	= 0.8f;
	const AR_TIME		DEFAULT_HEAVY_BOW_INTERVAL	= 50;		// 기본 활 휴지기
	const AR_TIME		DEFAULT_LIGHT_BOW_INTERVAL	= 250;		// 기본 활 휴지기

	const AR_TIME		DEFAULT_DOUBLE_SUMMON_TIME	= 1000;	// 기본 동시 소환 가능 시간
	const AR_TIME		BIND_SUMMON_UPDATE_TIME	= 1000;		// 소환 안된 크리쳐 업데이트 타임.

	const AR_UNIT		CHANGE_LOCATION_ERROR_RANGE		= 10 * DEFAULT_UNIT_SIZE; // 위치 바꿀 때 오차범위
	const AR_UNIT		TACTICAL_POSITION_REBIRTH_ERROR	= 4 * DEFAULT_UNIT_SIZE;	// 던전 시즈에서 전략 거점 부활시 좌표 오차 거리

	const float			MONSTER_PRESPAWN_RATE				= 0.5f;		// 몬스터 월드에 미리 리젠해놓는 몬스터 비율
#ifdef _DEBUG
	const AR_TIME		MONSTER_RESPAWN_DECREASE_INTERVAL	= 6000;		// 리젠된 몬스터 수 최소로 줄이는 시간 간격(디버깅 모드: 1분)
#else
	const AR_TIME		MONSTER_RESPAWN_DECREASE_INTERVAL	= 18000;	// 리젠된 몬스터 수 최소로 줄이는 시간 간격(릴리즈 모드: 3분)
#endif

	const AR_UNIT		PARTY_ITEM_SHARE_RANGE				= 400;		// 파티 아이템 분배 방식에 영향을 받는 유저 판별 최대 거리

	const AR_UNIT		AGGRESIVE_MONSTER_REACTION_RANGE	= 10 * DEFAULT_UNIT_SIZE;	// 선공몹 어그로 범위
	const AR_UNIT		MONSTER_TRACKING_RANGE_BY_TIME		= 30 * DEFAULT_UNIT_SIZE;	// 추적 범위 ( 이 이상 거리가 멀어지면 다른 타겟을 찾아갑니다~ )
	const AR_UNIT		MONSTER_TRACKING_RANGE				= 50 * DEFAULT_UNIT_SIZE;	// 추적 범위 ( 이 이상 거리가 멀어지면 다른 타겟을 찾아갑니다~ )
	const AR_UNIT		MONSTER_COME_BACK_HOME_RANGE		= 150 * DEFAULT_UNIT_SIZE;	// 집-ㅅ-에서 너무 멀리 떨어져나와, 향수를 느껴 집으로 다시 돌아가게 되는 거리.

	const AR_UNIT		SUMMON_FOLLOWING_FIRST_SPEED_UP_RANGE		= 3 * DEFAULT_UNIT_SIZE;	// 소환수가 주인 이동속도의 1차 배율로 따라가는 거리
	const float			SUMMON_FOLLOWING_FIRST_SPEED_UP_RATE		= 1.2f;						// 소환수의 주인 이동속도의 1차 배율
	const AR_UNIT		SUMMON_FOLLOWING_SECOND_SPEED_UP_RANGE		= 40 * DEFAULT_UNIT_SIZE;	// 소환수가 주인 이동속도의 2차 배율로 따라가는 거리
	const float			SUMMON_FOLLOWING_SECOND_SPEED_UP_RATE		= 1.5f;						// 소환수의 주인 이동속도의 2차 배율
	const AR_UNIT		SUMMON_FOLLOWING_LIMIT_RANGE				= 150 * DEFAULT_UNIT_SIZE;	// 소환수가 주인 따라가기를 멈추는 거리

	const float			PET_FOLLOWING_COMMON_SPEED_RATE		= 1.1f;						// 펫의 평소 이동속도의 주인 이동속도에 대한 배율
	const AR_UNIT		PET_FOLLOWING_HURRY_SPPED_RANGE		= 10 * DEFAULT_UNIT_SIZE;	// 펫의 더 빠른 속도로 주인을 따라가는 거리
	const float			PET_FOLLOWING_HURRY_SPEED_RATE		= 2.0f;						// 펫이 주인과 멀어져서 빨리 따라갈 때 주인 이동속도에 대한 비율
	const AR_UNIT		PET_FOLLOWING_LIMIT_RANGE			= 150 * DEFAULT_UNIT_SIZE;	// 펫이 주인 따라가기를 멈추는 거리

	const int			PET_SHOVELING_SUCCESS_PERCENT_NORMAL		= 30;				// 일반 펫이 삽질에 성공할 확률
	const int			PET_SHOVELING_SUCCESS_PERCENT_RARE			= 50;				// 레어 펫이 삽질에 성공할 확률

	enum PET_SHOVELING_REWARD_TYPE
	{
		PET_SHOVELING_REWARD_UNKNOWN	= -1,

		PET_SHOVELING_REWARD_ITEM		= 0,
		PET_SHOVELING_REWARD_STATE		= 1,
		PET_SHOVELING_REWARD_MONSTER	= 2,

		PET_SHOVELING_REWARD_TYPE_COUNT			// 삽질 성공 시 나올 보상 종류 수
	};
	const int			PET_SHOVELING_REWARD_TYPE_PERCENT[ PET_SHOVELING_REWARD_TYPE_COUNT ]	= { 85, 5, 10 };	// 삽질 성공 시 나올 보상 종류에 따른 확률(아이템, 지속효과, 몬스터 순)
	const AR_TIME		PET_SHOVELING_REWARD_STATE_DURATION										= 120000;			// 삽질 성공 시 보상으로 걸리는 지속효과의 기간
	const int			PET_SHOVELING_REWARD_MONSTER_PET_PERCENT								= 1;				// 삽질 성공 시 보상으로 몹이 리젠될 때 레어 펫이 일반 펫용 몬스터를 소환할 확률

	const AR_UNIT		PET_SHOVELING_LIMIT_DISTANCE_FROM_MASTER								= 10 * DEFAULT_UNIT_SIZE;	// 삽질 중 주인과의 한계 거리
	const AR_UNIT		PET_SHOVELING_LIMIT_ROAMING_DISTANCE									= 8 * DEFAULT_UNIT_SIZE;	// 삽질 중 주인과의 한계 거리

	const AR_TIME		PET_SHOVELING_SEARCH_DURATION											= 200;			// 삽질 - 주변 탐색 기간
	const AR_TIME		PET_SHOVELING_APPROACH_DURATION											= 300;			// 삽질 - 이동 기간
	const int			PET_SHOVELING_APPROACH_SPEED											= 75;			// 삽질 이동 기간 중 이동 속도

	const int			MONSTER_FIND_ATTACK_POS_RATIO		= 5;					// 공격 중 다른 몬스터와 겹칠때 적절한 위치를 찾을 확률

	const float			ALLOWED_POSITION_TIME_SYNC_MODIFIER	= 6.0f;
	const AR_UNIT		ALLOWED_POSITION_SYNC_RANGE			= 10 * DEFAULT_UNIT_SIZE;
	const int			ALLOWED_NON_REGION_UPDATE_COUNT		= 5;

	// PC 방 보너스 관련
	const char			PCBANG_NO_BONUS						= 0;
	const char			PCBANG_ALLY_BONUS					= 1;
	const char			PCBANG_PREMIUM_BONUS				= 2;

	// 기부 관련
	const int			DONATION_MAX_REWARD_ITEM_TYPE		= 4;
	const c_fixed10		DONATION_POINT_FOR_REWARD_ITEM[DONATION_MAX_REWARD_ITEM_TYPE]	= { 1000, 5000, 10000, 30000 };				// 기부 보상을 받기 위한 등급별 모럴 포인트
	const int			DONATION_REWARD_ITEM_CODE[DONATION_MAX_REWARD_ITEM_TYPE]		= { 3620026, 3620025, 3620024, 3620023 };	// 기부 보상으로 받게되는 등급별 아이템 코드

	const char			MAX_MORAL_GRADE						= 20;

	// 에테리얼 내구도 관련
	const int			MAX_ETHEREAL_DURABILITY									= 2000000000;	// 최대 에테리얼 내구도 충전 가능량
	const int			MAX_ETHEREAL_STONE_DURABILITY							= 200000000;	// 최대 에테리얼 스톤 충전 가능량
	const c_fixed10		ETHEREAL_DURABILITY_ENVIRONMENTAL_CONSUME_RATE_ON_PVP	= c_fixed10( 0.05 );
	const int			ETHEREAL_DURABILITY_ENVIRONMENTAL_CONSUME_RATE_ON_PK	= 2;

	// 소울스톤 관련
	const int			ENDURANCE_REDUCE_ON_ATTACK			= 500;			// 공격시 내구도 감소량
	const int			ENDURANCE_REDUCE_ON_SKILL			= 1000;			// 마법, 스킬 사용시 내구도 감소량
	const int			ENDURANCE_REDUCE_ON_DAMAGE			= 75;			// 피격시 내구도 감소량
	const double		COST_CHAOS_PER_PEPAIR_POINT			= 1.2;			// 영혼력 1 충전시 소모 라크량

	// 소지금 한도 관련
	const StructGold	MAX_GOLD_FOR_INVENTORY( 1000000000000 );				// 인벤토리 소지금 한도 100억 => 2014/02/26 러시아 퍼블리셔의 건의로 전 서버 소지금 한도 1000억 루피로 증가
	const StructGold	MAX_GOLD_FOR_STORAGE( 1000000000000 );				// 창고 소지금 한도 1000억 루피

	// 경매 관련
	const int			AUCTION_ITEM_KEEP_DURATION					= 60 * 60 * 24 * 15;	// 경매 아이템 보관 기간(15일)
	const c_fixed10		AUCTION_MIN_BIDDING_PRICE_RATE				= c_fixed10( 1.01 );	// 경매 입찰시 최저 입찰가 비율
	const c_fixed10		AUCTION_MIN_INSTANT_PURCHASE_PRICE_RATE		= c_fixed10( 1.00 );	// 경매 등록시 최저 즉구가 비율
	const c_fixed10		AUCTION_SELL_TAX_RATE						= c_fixed10( 0.03 );	// 경매 물품 판매시 판매 가격에 대한 수수료율
	const time_t		AUCTION_DURATION_SHORTTERM					= 60 * 60 * 6;			// 경매 단기 등록 기간(6시간)
	const time_t		AUCTION_DURATION_MIDTERM					= 60 * 60 * 24;			// 경매 중기 등록 기간(24시간)
	const time_t		AUCTION_DURATION_LONGTERM					= 60 * 60 * 72;			// 경매 장기 등록 기간(72시간)
	const c_fixed10		AUCTION_REGISTER_TAX_RATE_SHORTTERM			= c_fixed10( 0.03 );	// 경매 단기 등록 수수료율
	const c_fixed10		AUCTION_REGISTER_TAX_RATE_MIDTERM			= c_fixed10( 0.04 );	// 경매 중기 등록 수수료율
	const c_fixed10		AUCTION_REGISTER_TAX_RATE_LONGTERM			= c_fixed10( 0.05 );	// 경매 장기 등록 수수료율
	extern AR_TIME		nAuctionSearchRequestMinInterval;									// 경매 검색 반복 가능 최단 시간 간격(1/100 초 단위)
	extern AR_TIME		nAuctionProcessRequestMinInterval;									// 경매 일반 동작 반복 가능 최단 시간 간격(검색 제외 모두 적용)

	const int			AUCTION_INFO_COUNT_PER_PAGE_FOR_SEARCHED		= 40;	// 클라이언트 경매장 UI 중 물품 검색 창에서 한 페이지당 표시되는 경매 데이터 개수(화면당 8개 동시 표시)
	const int			AUCTION_INFO_COUNT_PER_PAGE_FOR_REGISTERED		= 40;	// 클라이언트 경매장 UI 중 경매 등록 창에서 한 페이지당 표시되는 경매 데이터 개수(화면당 9개 동시 표시)
	const int			AUCTION_INFO_COUNT_PER_PAGE_FOR_BIDDED			= 40;	// 클라이언트 경매장 UI 중 입찰 목록창에서 한 페이지당 표시되는 경매 데이터 개수(화면당 9개 동시 표시)
	const int			AUCTION_INFO_COUNT_PER_PAGE_FOR_ITEM_KEEPING	= 40;	// 클라이언트 경매장 UI 중 보관함 창에서 한 페이지당 표시되는 아이템 보관 데이터 개수(화면당 9개 동시 표시)

#ifdef _DEBUG
	const int			AUCTION_MAX_BIDDER_LIST_LENGTH					= 3;	// 특정 경매에 대해서 보관할 지난 입찰자들 수(vBidderUID 벡터의 길이를 제한)
#else
	const int			AUCTION_MAX_BIDDER_LIST_LENGTH					= 100;	// 특정 경매에 대해서 보관할 지난 입찰자들 수(vBidderUID 벡터의 길이를 제한)
#endif

	// 로밍 관련
	const AR_UNIT		ROAMING_MAX_DISTANCE_FROM_OWN_POSITION			= static_cast< AR_UNIT >( 0.2f * DEFAULT_UNIT_SIZE );	// 로밍시 몹/NPC가 자신의 원래 위치에 있다고 판단하는 최대 거리
	const float			ROAMING_ROATING_MOVE_SPEED_RATE					= 1.5f;													// 로밍 중 대형 방향 전환 시 이동 속도

	// 헌터홀릭 관련
	const size_t		HUNTAHOLIC_MAX_INSTANCE_COUNT					= 200;			// 헌터홀릭 인스턴스 던전 생성 최대 개수
	const size_t		HUNTAHOLIC_MAX_INSTANCE_COUNT_PER_PAGE			= 16;			// 헌터홀릭 인스턴스 던전 목록 UI에서 한 페이지에 보여지는 최대 개수
	const int			HUNTAHOLIC_MAX_INSTANCE_NAME_LENGTH				= 30;			// 헌터홀릭 인스턴스 던전의 이름 최대 길이(30 바이트)
	const int			HUNTAHOLIC_MAX_INSTANCE_PASSWORD_LENGTH			= 16;			// 헌터홀릭 인스턴스 던전의 비밀번호 최대 길이(16 바이트)
	const int			HUNTAHOLIC_MAX_OWNABLE_POINT					= 2000000000;	// 헌터홀릭 포인트 최대 보유 가능량(20억)
	const int			HUNTAHOLIC_SCORE_FOR_LV1_MONSTER				= 1;			// 헌터홀릭 1등급 몬스터가 주는 점수
	const int			HUNTAHOLIC_SCORE_FOR_LV2_MONSTER				= 5;			// 헌터홀릭 2등급 몬스터가 주는 점수
	const int			HUNTAHOLIC_SCORE_FOR_LV3_MONSTER				= 10;			// 헌터홀릭 3등급 몬스터가 주는 점수
	const int			HUNTAHOLIC_SCORE_FOR_BOSS_MONSTER				= 150;			// 헌터홀릭 보스 몬스터가 주는 점수
	const unsigned char	HUNTAHOLIC_UNUSABLE_LOBBY_LAYER					= 0xFF;			// 헌터홀릭 로비 레이어 중 사용 불가 레이어(오류 코드용)
	const AR_TIME		HUNTAHOLIC_DUNGEON_BEGIN_COUNTDOWN_TIME			= 1000;			// 헌터홀릭 사냥 시작 전 카운트 다운 기간(시작 버튼 클릭 시점으로부터 value 만큼)
	const AR_TIME		HUNTAHOLIC_DUNGEON_END_NOTICE_TIME				= 6000;			// 헌터홀릭 사냥 종료 알림 공지 시각(종료 시점으로부터 value 전)
	const AR_TIME		HUNTAHOLIC_QUITTING_PENALTY_DEBUFF_TIME			= 6000;			// 헌터홀릭 사냥 중 직접 중도 포기하는 경우 주어지는 페널티 디버프(이속 저하)의 기간
	const int			HUNTAHOLIC_ENTERANCE_COUNT_PER_DAY				= 12;			// 헌터홀릭 사냥 도전 제한 횟수

	// 데스매치 관련
	const AR_TIME		BEGINNING_TIME_IN_DEATHMATCH					= 2000;
	const float			HEALING_RATE_ON_KILL_IN_DEATHMATCH				= 0.3f;

	// 채팅 금지 관련
	const int			MAX_BLOCK_TIME									= 144000;		// 채팅 금지 최대 제한 시간(100일, 분 단위)

	// 캐쉬 관련
	const int			MAX_SECROUTE_REMAIN_TIME						= 200 * 86400;	// 최대 시크루트 프리패스 잔여 기간(보이기에만 제한)

	// 아이템 파괴 관련
	const char			MAX_ERASE_ITEM_COUNT							= 25;			// 한 번에 파괴할 수 있는 서로 다른 종류의 아이템 수

	// 아이템 분해 관련
	const char			MAX_DECOMPOSE_ITEM_COUNT						= 16;			// 한 번에 분해할 수 있는 서로 다른 종류의 아이템 수
	const char			MAX_DECOMPOSE_COUNT_PER_ITEM					= 100;			// 한 번에 분해할 수 있는 한 종류의 중첩 아이템 개수

	// Monster Creature 관련
	const char			MAX_MONSTER_CREATURE_COUNT						= 10;
	const char			MAX_RANDOM_SKILL_COUNT							= 15;
	const char			USE_SKILL_ALL_OBJECT							= 99;

	// 농장 관련
	extern int			nFarmNormalSummonEXP;	// 소환수 기본형의 농장 시간당 경험치
	extern int			nFarmGrowthSummonEXP;	// 소환수 성장형의 농장 시간당 경험치
	extern int			nFarmEvolveSummonEXP;	// 소환수 진화형의 농장 시간당 경험치

	extern int			nPremiumFarmNormalSummonEXP;	// 소환수 기본형의 농장 시간당 경험치 (프리미엄 티켓 이용)
	extern int			nPremiumFarmGrowthSummonEXP;	// 소환수 기본형의 농장 시간당 경험치 (프리미엄 티켓 이용)
	extern int			nPremiumFarmEvolveSummonEXP;	// 소환수 기본형의 농장 시간당 경험치 (프리미엄 티켓 이용)

	extern int			SummonExpPenaltyLevel;
	extern int			PartyExpPenaltyLevel;
	extern int			PartyExpPenaltyLevelCutoff;
	extern float		fTamingSuccessRate2;
	extern float		fTamingSuccessRate3;
	extern float		fTamingSuccessRate4;
	extern float		fTamingSuccessRate5;

	extern int			nSkyFortressStartIndex;
	extern bool			bGMsNeverFail;


	// 시크루트를 포함한 계정 권한 관련
	enum AccountAuthority
	{
		AUTHORITY_SECROUTE = 910000,

	};

	int GetSummonEXPLimit( int level );
	int GetPlayerEXPLimit( int level );
	float GetSummonLevelPenalty( int master_level, int summon_level );
	float GetSummonStatPenalty( int master_level, int summon_level );

	void RegisterBlockAccount( const char * szAccount );
	void DeleteFromBlockAccount( const char * szAccount );
	bool IsBlockedAccount( const char * szAccount );

	// 먹자 방지 시간
	inline AR_TIME GetPickupOrderTime( int idx )														{ return 3000 + idx * 1000; }
	// 시체 유지 시간
	inline int GetCorpseHoldTime()																		{ return 1200; }	// 10초

	// 소환수 사망 후 역소환 대기 시간
	inline int GetDeadSummonHoldTime()																	{ return 6000; }	// 60초

	inline const c_fixed10 GetCritical( const c_fixed10 & luck )										{ return luck * 0.2 + 3; }

	inline const c_fixed10 GetItemChance( const c_fixed10 & luck )										{ return luck * 0.2; }

	// 공격력	(2010-05-07 리뉴얼 적용)
	inline const c_fixed10 GetBowAttackPoint( const c_fixed10 & agility, const c_fixed10 & dexterity, const float level, const c_fixed10 & fCM )
																										{ return ( agility * 1.2 + dexterity * 2.2 ) * fCM + level; }
	inline const c_fixed10 GetAttackPoint( const c_fixed10 & strength, const float level, const c_fixed10 & fCM )
																										{ return strength * 2.8 * fCM + level; }

	// 마법 공격력	(2007-03-24 리뉴얼 적용)
	inline const c_fixed10 GetMagicPoint( const c_fixed10 & intelligence, const float level, const c_fixed10 & fCM  )
																										{ return intelligence * 2 * fCM + level; }

	// 방어력	(2010-05-07 리뉴얼 적용)
	inline const c_fixed10 GetDefence( const float level, const c_fixed10 & vital, const c_fixed10 & fCM )
																										{ return vital * 1.6 * fCM + level; }

	// 마법 방어력	(2007-03-24 리뉴얼 적용)
	inline const c_fixed10 GetMagicDefence( const float level, const c_fixed10 & men, const c_fixed10 & fCM )
																										{ return men * 2 * fCM + level; }

	// 명중수치
	inline const c_fixed10 GetAccuracy( const c_fixed10 & dexterity, const float level, const c_fixed10 & fCM )
																										{ return dexterity * 0.5 * fCM + level; }

	// 마법명중수치
	inline const c_fixed10 GetMagicAccuracy( const c_fixed10 & mental, const c_fixed10 & dexterity, const float level, const c_fixed10 & fCM )
																										{ return ( ( mental * 0.4 ) + ( dexterity * 0.1 ) ) * fCM + level; }

	// 마법저항수치	(2007-03-24 리뉴얼 적용)
	inline const c_fixed10 GetMagicAvoid( const c_fixed10 & mental, const float level, const c_fixed10 & fCM )
																										{ return ( mental * 0.5 ) * fCM + level; }

	// 회피수치	(2007-03-24 리뉴얼 적용)
	inline const c_fixed10 GetAvoid( const c_fixed10 & agility, const float level, const c_fixed10 & fCM )
																										{ return ( agility * 0.5 ) * fCM + level; }

	// 시전속도	(2007-03-24 리뉴얼 적용)
	inline const c_fixed10 GetCastingTime( const c_fixed10 & dexterity, const c_fixed10 & men, const float level )
																										{ return 100; /*return ( (dexterity * 7) + (men * 3) ) * 0.04 + 70;*/ }

	// 기본이동속도: 캐릭터만 적용됨, 그 외는 DB에서 정의하는 값을 사용	(2007-03-24 리뉴얼 적용)
	inline int GetBaseMoveSpeed()																		{ return 120; }

	// 최대 HP	(2010-05-07 리뉴얼 적용)
	inline int GetMaxHP( const c_fixed10 & vital, const float level, const c_fixed10 & fCM )			{ return int(( vital * fCM * 33 ) + ( level * 20 )); }

	// 최대 MP	(2007-03-24 리뉴얼 적용)
	inline int GetMaxMP( const c_fixed10 & intelligence, const float level, const c_fixed10 & fCM  )	{ return int(( intelligence * fCM * 30 ) + ( level * 20 )); }

	template< typename INT_TYPE >
	inline INT_TYPE GetIntValueByRandom( const double & fValue )
	{
		double fmod = (INT_TYPE)fValue + (double) ( XRandom()  % 100 ) / 100.0f;

		if( fmod < fValue )
		{
			return (INT_TYPE) fValue + 1;
		}

		return  (INT_TYPE) fValue;
	}

	// 공격속도	(2010-05-07 리뉴얼 적용)
	inline const c_fixed10 GetAttackSpeed( const c_fixed10 & agility )
	{
		return agility * 0.1 + 100;
	}
	
	// HP 리젠율% (60초당)	(2007-03-24 리뉴얼 적용)
	inline int GetHPRegenPercentage( const c_fixed10 & fCM )
	{
		return int( fCM * 5 );
	}

	// HP 리젠율+ (60초당)	(2007-03-24 리뉴얼 적용)
	inline int GetHPRegenPoint( float level )
	{
		return int( (level * 2) + 48 );
	}

	// MP 리젠율% (60초당)	(2007-03-24 리뉴얼 적용)
	inline int GetMPRegenPercentage( const c_fixed10 & fCM )
	{
		return int( fCM * 5 );
	}

	// MP 리젠율 (60초당)	(2010-05-07 리뉴얼 적용)
	inline int GetMPRegenPoint( float level, const c_fixed10 & mentality )
	{
		return int( level * 2 + 48 + mentality * 4.1 );
	}

	inline const c_fixed10 GetPerfectBlock()
	{
		return 20.0f;
	}

	
	float GetStaminaRatio( int level );

	// 플레이어가 최대로 들 수 있는 무게	(2007-03-24 리뉴얼 적용)
	int	GetMaxWeight( int level, int strength );

	// 물건을 주울 수 있는 거리
	int GetPickableRange();

	// 등급에 따른 아이템 레벨 제한
	int GetItemLevelLimitByRank( int item_rank );

	// 아이템 레벨에 따른 페널티
	c_fixed10 GetItemLevelPenalty( int creature_level, int item_rank, int item_level, int min_item_usable_level );

	// 등급별 아이템 권장레벨 조정 테이블
	int GetItemRecommendModTable( int item_rank );

	// 아이템 권장 레벨
	int GetItemRecommendLevel( int item_rank, int item_level, int min_item_usable_level );

	// 아이템 내구도 손상 포인트
	int GetDecreasedEndurancePoint( int previous_endurance, int current_endurance );

	// 에테리얼 내구도 소모율 계산(아이템과 관계 없이 미리 계산 가능한 부분과 아이템 정보가 반드시 필요한 부분 2가지로 분할)
	const c_fixed10	GetEtherealDurabilityConsumeRate( const int nLevel, const int nJobID, const bool bIsAttack, const int nDamage );
	const c_fixed10	GetEtherealDurabilityConsumeRateByItem( const int nRank, const int nGrade );
	// 에테리얼 내구도 기본 소모량 얻기(StructCreature::DamageType에 따라 결정됨)
	const int		GetEtherealDurabilityBaseConsumption( const bool bIsAttack, /*StructCreature::DamageType*/ const int nDamageType );

	// 각 레벨에 따른 성능 조정 (데미지, 방어구 동일)
	const c_fixed10 GetItemValue( c_fixed10 item_current_value, int item_rank_value, int creature_level, int item_rank, int item_level, int min_item_usable_level );

	// 기부 아이템에 대한 모럴 획득량
	const c_fixed10 GetDonationRewardMoralPoint( const __int64 & nDonateGoldAmount );

	const StructGold GetItemSellPrice( const StructGold & price, int rank, int lv, const bool same_price_for_buying, const bool ethereal_durability_exhausted, const bool is_equipment );

	bool IsValidName( int code_page, const char * name, int nBufferSize, int nLimitMin, int nLimitMax );
	bool ReformatName( char * name );
	bool IsValidPartyName( const char * name, int nBufferSize, int nLimitMin, int nLimitMax );

	int AppendOnetimePassword( char * pBuf, size_t buf_len, int one_time_key, int nSID, int nAccountID );

	// 게임 중독 방지 시스템 관련 각종 수입 페널티
	c_fixed10 GetGameTimeLimitPenalty( AR_TIME continuous_play_time );

	// 펫 삽질 보상 중 지속효과 코드 얻기
	const int GetPetShovelingRewardStateCode();

	const c_fixed10 GetDifficultyBonus( const unsigned char nDifficulty );

	int GetBattleArenaTeamNameStringID( int nTeamNo, bool bWithColorTag );
	time_t GetBattleArenaBlockDuration( int nPenaltyCount );
	time_t GetBattleArenaPenaltyDuration( int nPenaltyCount );
};
