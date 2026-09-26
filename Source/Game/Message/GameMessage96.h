#pragma once

#include "ItemBase.h"
#include "ItemInstance.h"
#include "CreatureBase.h"
#include "QuestBase.h"
#include "QuestInstance.h"
#include "BattleArenaBase.h"
#include "MessageType.h"

#include <hack_shield/AntiCpXSvr.h>
#include <xtrap/Xtrap_S_Interface.h>

#include "GameRule.h"
#include "GameType.h"

#include "VersionDefs.h"

#ifdef USE_CLIENT97
const unsigned short TM_ADDV_1 = 1000;
const unsigned short TM_ADDV_2 = 400;
#else
const unsigned short TM_ADDV_1 = 0;
const unsigned short TM_ADDV_2 = 0;
#endif

const	unsigned short TM_COMMON = 9998;
const	unsigned short TM_NONE = 9999;
const	unsigned short TM_CS_VERSION_OLD = 50;
#ifdef USE_CLIENT97

const	unsigned short TM_SC_RESULT = 0;
const	unsigned short TM_CS_VERSION = 51;
const	unsigned short TM_CS_CHARACTER_LIST = 2008;	// 2001
const	unsigned short TM_CS_CREATE_CHARACTER = 2002;
const	unsigned short TM_CS_DELETE_CHARACTER = 2003;
const	unsigned short TM_SC_CHARACTER_LIST = 2004;
const	unsigned short TM_CS_ACCOUNT_WITH_AUTH = 2005;
const	unsigned short TM_CS_CHECK_CHARACTER_NAME = 2006;
const	unsigned short TM_SC_STAT_INFO = 1000;		// 스탯정보


const	unsigned short TM_CS_REPORT = 8000;
const	unsigned short TM_SC_OPEN_URL = 9000;
const	unsigned short TM_SC_URL_LIST = 8801;// 8101;

const	unsigned short TM_SC_REQUEST_SECUTIRY_NO = 9004;
const	unsigned short TM_CS_SECURITY_NO = 9005;

const	unsigned short TM_CS_OPEN_ITEM_SHOP = 10000;
const	unsigned short TM_SC_OPEN_ITEM_SHOP = 10001;
const	unsigned short TM_SC_COMMERCIAL_STORAGE_INFO = 10003;	// 캐쉬템 창고에 아이템이 뭔가 들어왔을경우 전송됨
const	unsigned short TM_SC_COMMERCIAL_STORAGE_LIST = 10004;
const	unsigned short TM_CS_TAKEOUT_COMMERCIAL_ITEM = 10005;

#else

const	unsigned short TM_SC_RESULT = 0;
const	unsigned short TM_CS_VERSION = 51;

const	unsigned short TM_CS_CHARACTER_LIST = 2179;	// 2001
const	unsigned short TM_CS_CREATE_CHARACTER = 2002;
const	unsigned short TM_CS_DELETE_CHARACTER = 2003;
const	unsigned short TM_SC_CHARACTER_LIST = 2004;
const	unsigned short TM_CS_ACCOUNT_WITH_AUTH = 2005;
const	unsigned short TM_CS_CHECK_CHARACTER_NAME = 2006;
const	unsigned short TM_SC_STAT_INFO = 1000;		// 스탯정보
const	unsigned short TM_CS_REPORT = 8000;
const	unsigned short TM_SC_OPEN_URL = 9000;
const	unsigned short TM_SC_URL_LIST = 9001;

const	unsigned short TM_SC_REQUEST_SECUTIRY_NO = 9004;
const	unsigned short TM_CS_SECURITY_NO = 9005;

const	unsigned short TM_CS_OPEN_ITEM_SHOP = 10000;
const	unsigned short TM_SC_OPEN_ITEM_SHOP = 10001;
const	unsigned short TM_SC_COMMERCIAL_STORAGE_INFO = 10003;	// 캐쉬템 창고에 아이템이 뭔가 들어왔을경우 전송됨
const	unsigned short TM_SC_COMMERCIAL_STORAGE_LIST = 10004;
const	unsigned short TM_CS_TAKEOUT_COMMERCIAL_ITEM = 10005;

#endif


const	unsigned short TM_SC_INFO = 52;

const	unsigned short TM_SC_ANTI_HACK = 53;
const	unsigned short TM_CS_ANTI_HACK = 54;
const	unsigned short TM_SC_GAME_GUARD_AUTH_QUERY = 55;
const	unsigned short TM_CS_GAME_GUARD_AUTH_ANSWER = 56;
const	unsigned short TM_CS_CHECK_ILLEGAL_USER = 57;
const	unsigned short TM_SC_XTRAP_CHECK = 58;
const	unsigned short TM_CS_XTRAP_CHECK = 59;

const	unsigned short TM_CS_REQUEST = 60;


#ifdef USE_OLD_CODES
const	unsigned short TM_CS_LOGIN = 1;
const	unsigned short TM_SC_LOGIN_RESULT = 4;
const	unsigned short TM_CS_MOVE_REQUEST = 5;
const	unsigned short TM_SC_MOVE_ACK = 6;
const	unsigned short TM_CS_REGION_UPDATE = 7;

#else
const	unsigned short TM_CS_LOGIN = 62;
const	unsigned short TM_SC_LOGIN_RESULT = 64;
const	unsigned short TM_CS_MOVE_REQUEST = 63;
const	unsigned short TM_SC_MOVE_ACK = 66;
const	unsigned short TM_CS_REGION_UPDATE = 67;

#endif	


const	unsigned short TM_TIMESYNC = 2;
const	unsigned short TM_SC_ENTER = 3;
const	unsigned short TM_CS_QUERY = 13;

const	unsigned short TM_SC_REGION_ACK = 11;

const	unsigned short TM_CS_ENTER_EVENT_AREA = 15;
const	unsigned short TM_CS_LEAVE_EVENT_AREA = 16;

const	unsigned short TM_SC_MOVE = 8;
const	unsigned short TM_SC_FORCE_MOVE = 14;
const	unsigned short TM_SC_LEAVE = 9;
const	unsigned short TM_SC_WARP = 12;

const	unsigned short TM_SC_SET_TIME = 10;

const	unsigned short TM_CS_CHAT_REQUEST = 20;
const	unsigned short TM_SC_CHAT_LOCAL = 21;
const	unsigned short TM_SC_CHAT = 22;
const	unsigned short TM_SC_CHAT_RESULT = 24;

const	unsigned short TM_CS_RETURN_LOBBY = 23;
const	unsigned short TM_CS_REQUEST_RETURN_LOBBY = 25;
const	unsigned short TM_CS_REQUEST_LOGOUT = 26;
const	unsigned short TM_CS_LOGOUT = 27;
const	unsigned short TM_SC_DISCONNECT_DESC = 28;

const	unsigned short TM_SC_CHANGE_NAME = 30;
const	unsigned short TM_CS_CHANGE_ALIAS = 31;

const	unsigned short TM_CS_ATTACK_REQUEST = 100;
const	unsigned short TM_SC_ATTACK_EVENT = 101;
const	unsigned short TM_SC_CANT_ATTACK = 102;
const	unsigned short TM_SC_KNOCK_BACK = 104;

const	unsigned short TM_CS_CANCEL_ACTION = 150;

const	unsigned short TM_CS_PUTON_ITEM = 200;		// 아이템 장비 요청
const	unsigned short TM_CS_PUTOFF_ITEM = 201;		// 아이템 장비해제 요청
const	unsigned short TM_SC_WEAR_INFO = 202;		// 아이템 장비 정보 - 첫 enter 시, 혹은 장비 변경시 방송됨
const	unsigned short TM_CS_DROP_ITEM = 203;		// 아이템 버림 요청
const	unsigned short TM_CS_TAKE_ITEM = 204;		// 아이템 줍기 요청
const	unsigned short TM_SC_DROP_RESULT = 205;		// 아이템 버림 결과
const	unsigned short TM_SC_INVENTORY = 207;		// 인벤 내용
const	unsigned short TM_CS_ERASE_ITEM = 208;		// 아이템 파기
const	unsigned short TM_SC_ERASE_ITEM = 209;		// 아이템 파기 결과
const	unsigned short TM_SC_TAKE_ITEM_RESULT = 210;
const	unsigned short TM_SC_OPEN_STORAGE = 211;		// 창고를 여셈~
const	unsigned short TM_CS_STORAGE = 212;		// 창고 아이템 이동

const	unsigned short TM_SC_GET_CHAOS = 213;		// 혼돈 얻음
const	unsigned short TM_CS_PUTON_CARD = 214;		// 벨트 슬롯에 아이템 장착
const	unsigned short TM_CS_PUTOFF_CARD = 215;		// 벨트 슬롯에 아이템 장착
const	unsigned short TM_SC_BELT_SLOT_INFO = 216;		// 벨트 슬롯 정보

const	unsigned short TM_SC_ITEM_COOL_TIME = 217;		// 아이템 쿨타임
const	unsigned short TM_CS_CHANGE_ITEM_POSITION = 218;	// 아이템 위치 변경(정렬)

const	unsigned short TM_CS_ARRANGE_ITEM = 219;		// 인벤토리/창고 아이템 정렬

const	unsigned short TM_SC_HAIR_INFO = 220;		// 헤어 정보 방송 - 첫 enter 시, 혹은 염색/헤어 스타일 변경 시 방송됨
const	unsigned short TM_CS_HIDE_EQUIP_INFO = 221;		// 장비 숨기기 정보 변경 요청
const	unsigned short TM_SC_HIDE_EQUIP_INFO = 222;		// 장비 숨기기 정보 방송 - TM_CS_HIDE_EQUIP_INFO 처리 시 방송됨

const	unsigned short TM_CS_SWAP_EQUIP = 223;		// 장비 스왑
const	unsigned short TM_SC_SKIN_INFO = 224;		// 피부 색상 변경

const	unsigned short TM_SC_NPC_TRADE_INFO = 240;		// 아이템 상점(NPC) 판매/구매 결과

const	unsigned short TM_SC_MARKET = 250;		// 상점 아이템 리스트
const	unsigned short TM_CS_BUY_ITEM = 251;		// 아이템 구입
const	unsigned short TM_CS_SELL_ITEM = 252;		// 아이템 판매
const	unsigned short TM_CS_USE_ITEM = 253;		// 아이템 사용
const	unsigned short TM_SC_DESTROY_ITEM = 254;		// 아이템 제거
const	unsigned short TM_SC_UPDATE_ITEM_COUNT = 255;		// 아이템 제거
const	unsigned short TM_CS_MIX = 256;		// 아이템 조합
const	unsigned short TM_SC_MIX_RESULT = 257;		// 아이템 조합 결과
const	unsigned short TM_CS_DONATE_ITEM = 258;		// 아이템 기부
const	unsigned short TM_CS_DONATE_REWARD = 259;		// 아이템 기부 보상 받기 요청

const	unsigned short TM_SC_SHOW_SOULSTONE_CRAFT_WINDOW = 259;		// 아이템 소켓 UI 표시 요청
const	unsigned short TM_CS_SOULSTONE_CRAFT = 260;		// 소켓 UI에 의한 소울스톤 세공
const	unsigned short TM_SC_SHOW_SOULSTONE_REPAIR_WINDOW = 261;		// 아이템 소울스톤 내구도 수리 UI 표시 요청
const	unsigned short TM_CS_REPAIR_SOULSTONE = 262;		// 소켓 아이템 내구도 수리
const	unsigned short TM_CS_TRANSMIT_ETHEREAL_DURABILITY = 263;		// 아이템 내구도 충전
const	unsigned short TM_CS_TRANSMIT_ETHEREAL_DURABILITY_TO_EQUIPMENT = 264;	// 장비 전체 수리

const	unsigned short TM_CS_DECOMPOSE = 265;		// 아이템 분해
const	unsigned short TM_SC_DECOMPOSE_RESULT = 266;		// 아이템 분해 결과

const	unsigned short TM_TRADE = 280;		// 트레이드
const	unsigned short TM_CS_PUTON_ITEM_SET = 281;		// 풀셋 아이템 장착 요청
const	unsigned short TM_SC_ITEM_DROP_INFO = 282;		// 어떤 몹이 어떤 아템 떨어트리는건지 정보
const	unsigned short TM_SC_USE_ITEM_RESULT = 283;		// 아이템 사용 결과
const	unsigned short TM_CS_BIND_SKILLCARD = 284;
const	unsigned short TM_CS_UNBIND_SKILLCARD = 285;
const	unsigned short TM_SC_SKILLCARD_INFO = 286;
const	unsigned short TM_SC_ITEM_WEAR_INFO = 287;		// 아이템 장비 정보  - 첫 enter 시, 혹은 장비 변경시 방송됨

//const	unsigned short TM_SC_ADD_SUMMON			= 300;		// 소환수 생성됨 통지

const	unsigned short TM_SC_ADD_SUMMON_INFO = 301;		// 편성된 소환수 정보 추가
const	unsigned short TM_SC_REMOVE_SUMMON_INFO = 302;		// 편성된 소환수 정보 제거
const	unsigned short TM_EQUIP_SUMMON = 303;		// 소환수 편성 정보
const	unsigned short TM_CS_SUMMON = 304;		// 소환/역소환 요청
const	unsigned short TM_SC_UNSUMMON = 305;		// 역소환 통지
const	unsigned short TM_SC_UNSUMMON_NOTICE = 306;		// 역소환 알림( x시간 후에 역소환 됨 )
const	unsigned short TM_SC_SUMMON_EVOLUTION = 307;
const	unsigned short TM_SC_TAMING_INFO = 310;		// 테이밍 정보
const	unsigned short TM_SC_MOUNT_SUMMON = 320;
const	unsigned short TM_SC_UNMOUNT_SUMMON = 321;
const	unsigned short TM_SC_SHOW_SUMMON_NAME_CHANGE = 322;	// 소환수 이름 변경 UI 출력 요청
const	unsigned short TM_CS_CHANGE_SUMMON_NAME = 323;		// 변경될 소환수 이름(대상은 StructPlayer가 들고 있음)
const	unsigned short TM_CS_GET_SUMMON_SETUP_INFO = 324;	// 소환수 편성 정보를 포함하는 메시지 요청(편성 창 띄우기로 사용)

const	unsigned short TM_SC_UNSUMMON_PET = 350;		// 펫 역소환 통지
const	unsigned short TM_SC_ADD_PET_INFO = 351;		// 보유 중인 펫 정보 추가
const	unsigned short TM_SC_REMOVE_PET_INFO = 352;		// 보유 중인 펫 정보 제거
const	unsigned short TM_SC_SHOW_SET_PET_NAME = 353;		// 펫 이름 설정 창 표시
const	unsigned short TM_CS_SET_PET_NAME = 354;		// 펫 이름 설정

const	unsigned short TM_CS_SKILL = 400;
const	unsigned short TM_SC_SKILL = 401;
const	unsigned short TM_CS_LEARN_SKILL = 402;
const	unsigned short TM_SC_SKILL_LIST = 403;
const	unsigned short TM_SC_ADDED_SKILL_LIST = 404;
const	unsigned short TM_CS_JOB_LEVEL_UP = 410;
const	unsigned short TM_SC_SKILL_EFFECT = 404;
//const	unsigned short TM_SC_SKILL_CASTING_TIME	= 405;
const	unsigned short TM_SC_STATE_RESULT = 406;
const	unsigned short TM_SC_AURA = 407;
const	unsigned short TM_CS_REQUEST_REMOVE_STATE = 408;

//const	unsigned short TM_CS_SKILL_LEVEL_LIST			= 450;
const	unsigned short TM_SC_SKILL_LEVEL_LIST = 451;
const	unsigned short TM_CS_SUMMON_CARD_SKILL_LIST = 452;

const	unsigned short TM_SC_STATUS_CHANGE = 500;
//const	unsigned short TM_SC_HP					= 501;
//const	unsigned short TM_SC_MP					= 502;
const	unsigned short TM_CS_UPDATE = 503;		// HP/MP 채우는거나 기타등등. 30초가량마다 한번씩 다오~
const	unsigned short TM_SC_DEAD = 504;		// 사망~
const	unsigned short TM_SC_STATE = 505;		// 상태이상 메시지
const	unsigned short TM_SC_PROPERTY = 507;		// 프로퍼티
const	unsigned short TM_CS_SET_PROPERTY = 508;		// 프로퍼티
const	unsigned short TM_SC_HPMP = 509;
const	unsigned short TM_SC_REGEN_INFO = 510;
const	unsigned short TM_CS_TARGETING = 511;
const	unsigned short TM_SC_TARGET = 512;
const	unsigned short TM_CS_RESURRECTION = 513;		// 
const	unsigned short TM_SC_SP = 514;
const	unsigned short TM_SC_ENERGY = 515;
const	unsigned short TM_SC_REGEN_HPMP = 516;
const	unsigned short TM_CS_MONSTER_RECOGNIZE = 517;		// 선공 몬스터의 시야 범위 내에 캐릭터가 들어갔을 때 클라에서 서버로 알려주는 패킷(서버에서는 주기적인 선공 체크밖에 못하므로 실시간 체크는 클라에 의존)

const	unsigned short TM_CS_GET_REGION_INFO = 550;

const	unsigned short TM_SC_QUEST_LIST = 600;
const	unsigned short TM_SC_QUEST_STATUS = 601;
const	unsigned short TM_SC_QUEST_INFOMATION = 602;
const	unsigned short TM_CS_DROP_QUEST = 603;
const	unsigned short TM_CS_QUEST_INFO = 604;
const	unsigned short TM_CS_END_QUEST = 605;

const	unsigned short TM_SC_TITLE_LIST = 625;
const	unsigned short TM_SC_TITLE_CONDITION_LIST = 626;
const	unsigned short TM_SC_REMAIN_TITLE_TIME = 627;
const	unsigned short TM_CS_SET_MAIN_TITLE = 628;
const	unsigned short TM_SC_SET_MAIN_TITLE = 629;
const	unsigned short TM_CS_SET_SUB_TITLE = 630;
const	unsigned short TM_SC_SET_SUB_TITLE = 631;
const	unsigned short TM_CS_BOOKMARK_TITLE = 632;
const	unsigned short TM_SC_BOOKMARK_TITLE = 633;
const	unsigned short TM_SC_ACHIEVE_TITLE = 634;
const	unsigned short TM_SC_OPEN_TITLE = 635;
const	unsigned short TM_SC_CHANGE_TITLE_CONDITION = 636;

const	unsigned short TM_SC_SHOW_CREATE_GUILD = 650;
const	unsigned short TM_SC_OPEN_GUILD_WINDOW = 651;
const	unsigned short TM_SC_UPDATE_GUILD_ICON = 652;
const	unsigned short TM_SC_UPDATE_GUILD_BANNER = 653;

const	unsigned short TM_SC_SHOW_CREATE_ALLIANCE = 660;

const	unsigned short TM_CS_START_BOOTH = 700;		// 개설
const	unsigned short TM_CS_STOP_BOOTH = 701;		// 중지
const	unsigned short TM_CS_WATCH_BOOTH = 702;		// 내용 보기
const	unsigned short TM_SC_WATCH_BOOTH = 703;		// 내용 보내주기
const	unsigned short TM_CS_STOP_WATCH_BOOTH = 704;		// 그만 보기
const	unsigned short TM_CS_BUY_FROM_BOOTH = 705;		// 물건 구입
const	unsigned short TM_CS_SELL_TO_BOOTH = 706;		// 물건 판매
const	unsigned short TM_CS_GET_BOOTHS_NAME = 707;		// 노점 이름 얻어오기
const	unsigned short TM_SC_GET_BOOTHS_NAME = 708;		// 노점 이름들
const	unsigned short TM_SC_BOOTH_CLOSED = 709;		// 노점 닫힘 알림
const	unsigned short TM_SC_BOOTH_TRADE_INFO = 710;		// 노점상에게 물건 거래 알림.
const	unsigned short TM_CS_CHECK_BOOTH_STARTABLE = 711;	// 노점 개설 가능 여부 확인(클라 UI 출력 전 확인)

const	unsigned short TM_CS_TURN_ON_PK_MODE = 800;
const	unsigned short TM_CS_TURN_OFF_PK_MODE = 801;

const	unsigned short TM_CS_CHANGE_LOCATION = 900;
const	unsigned short TM_SC_CHANGE_LOCATION = 901;
const	unsigned short TM_SC_WEATHER_INFO = 902;
const	unsigned short TM_CS_GET_WEATHER_INFO = 903;


const	unsigned short TM_SC_GOLD_UPDATE = 1001;		// 돈 바뀜
const	unsigned short TM_SC_LEVEL_UPDATE = 1002;		// 레벨 바뀜
const	unsigned short TM_SC_EXP_UPDATE = 1003;		// 경험치 바뀜
const	unsigned short TM_SC_BONUS_EXP_JP = 1004;		// 보너스 경험치/잡포 획득 방송
const	unsigned short TM_SC_DETECT_RANGE_UPDATE = 1005;		// 은신 감지 범위 변경

const	unsigned short TM_CS_GAME_TIME = 1100;
const	unsigned short TM_SC_GAME_TIME = 1101;

const	unsigned short TM_SC_EMOTION = 1201;
const	unsigned short TM_CS_EMOTION = 1202;

const	unsigned short TM_CS_AUCTION_SEARCH = 1300;		// 경매 물품 검색 요청
const	unsigned short TM_SC_AUCTION_SEARCH = 1301;		// 경매 물품 검색 결과
const	unsigned short TM_CS_AUCTION_SELLING_LIST = 1302;		// 자신이 등록한 경매 물품 목록 요청
const	unsigned short TM_SC_AUCTION_SELLING_LIST = 1303;		// 자신이 등록한 경매 물품 목록 결과
const	unsigned short TM_CS_AUCTION_BIDDED_LIST = 1304;		// 자신이 입찰한 경매 목록 요청
const	unsigned short TM_SC_AUCTION_BIDDED_LIST = 1305;		// 자신이 입찰한 경매 목록 결과
const	unsigned short TM_CS_AUCTION_BID = 1306;		// 경매 입찰 요청(결과는 TS_SC_RESULT로 보내고 성공시 최고 입찰가는 TS_SC_RESULT 메시지의 value로 전달함)
const	unsigned short TM_CS_AUCTION_INSTANT_PURCHASE = 1308;		// 경매 물품 즉시구매 요청(결과는 TS_SC_RESULT로 보내고 페이지 갱신은 클라에서 요청 따로 보냄)
const	unsigned short TM_CS_AUCTION_REGISTER = 1309;		// 경매 물품 등록 요청(결과는 TS_SC_RESULT로 보내고 페이지 갱신은 클라에서 요청 따로 보냄)
const	unsigned short TM_CS_AUCTION_CANCEL = 1310;		// 자신이 등록한 경매 취소 요청(결과는 TS_SC_RESULT로 보내고 페이지 갱신은 클라에서 요청 따로 보냄)

const	unsigned short TM_CS_ITEM_KEEPING_LIST = 1350;		// 자신 소유 보관 아이템 목록 요청
const	unsigned short TM_SC_ITEM_KEEPING_LIST = 1351;		// 자신 소유 보관 아이템 목록 결과
const	unsigned short TM_CS_ITEM_KEEPING_TAKE = 1352;		// 자신 소유 보관 아이템 회수 요청(결과는 TS_SC_RESULT로 보냄)

//const	unsigned short TM_CS_ACCOUNT			= 2000;

const	unsigned short TM_SC_DIALOG = 3000;
const	unsigned short TM_CS_DIALOG = 3001;
const	unsigned short TM_CS_CONTACT = 3002;
const	unsigned short TM_SC_SHOW_WINDOW = 3003;
const	unsigned short TM_SC_GENERAL_MESSAGE_BOX = 3004;

// 헌터홀릭 관련
//{
const	unsigned short TM_CS_HUNTAHOLIC_INSTANCE_LIST = 4000;
const	unsigned short TM_SC_HUNTAHOLIC_INSTANCE_LIST = 4001;
const	unsigned short TM_SC_HUNTAHOLIC_INSTANCE_INFO = 4002;
const	unsigned short TM_CS_HUNTAHOLIC_CREATE_INSTANCE = 4003;
const	unsigned short TM_CS_HUNTAHOLIC_JOIN_INSTANCE = 4004;
const	unsigned short TM_CS_HUNTAHOLIC_LEAVE_INSTANCE = 4005;
const	unsigned short TM_SC_HUNTAHOLIC_HUNTING_SCORE = 4006;
const	unsigned short TM_SC_HUNTAHOLIC_UPDATE_SCORE = 4007;
const	unsigned short TM_CS_HUNTAHOLIC_LEAVE_LOBBY = 4008;
const	unsigned short TM_SC_HUNTAHOLIC_BEGIN_HUNTING = 4009;
const	unsigned short TM_SC_HUNTAHOLIC_MAX_POINT_ACHIEVED = 4010;
const	unsigned short TM_CS_HUNTAHOLIC_BEGIN_HUNTING = 4011;
const	unsigned short TM_SC_HUNTAHOLIC_BEGIN_COUNTDOWN = 4012;
//}

// 인스턴스 게임 관련
const	unsigned short TM_CS_INSTANCE_GAME_ENTER = 4250;
const	unsigned short TM_CS_INSTANCE_GAME_EXIT = 4251;
const	unsigned short TM_CS_INSTANCE_GAME_SCORE_REQUEST = 4252;
const	unsigned short TM_SC_INSTANCE_GAME_SCORE_REQUEST = 4253;

// Compete(필드 대련) 관련
const	unsigned short TM_CS_COMPETE_REQUEST = 4500;
const	unsigned short TM_SC_COMPETE_REQUEST = 4501;
const	unsigned short TM_CS_COMPETE_ANSWER = 4502;
const	unsigned short TM_SC_COMPETE_ANSWER = 4503;
const	unsigned short TM_SC_COMPETE_COUNTDOWN = 4504;
const	unsigned short TM_SC_COMPETE_START = 4505;
const	unsigned short TM_SC_COMPETE_END = 4506;

// 배틀 아레나 관련
const	unsigned short TM_SC_BATTLE_ARENA_PENALTY_INFO = 4700;
const	unsigned short TM_CS_BATTLE_ARENA_JOIN_QUEUE = 4701;
const	unsigned short TM_SC_BATTLE_ARENA_JOIN_QUEUE = 4702;
const	unsigned short TM_SC_BATTLE_ARENA_UPDATE_WAIT_USER_COUNT = 4703;
const	unsigned short TM_CS_BATTLE_ARENA_LEAVE = 4704;
const	unsigned short TM_SC_BATTLE_ARENA_LEAVE = 4705;
const	unsigned short TM_SC_BATTLE_ARENA_BATTLE_INFO = 4706;
const	unsigned short TM_CS_BATTLE_ARENA_ENTER_WHILE_COUNTDOWN = 4707;
const	unsigned short TM_CS_BATTLE_ARENA_EXERCISE_READY = 4708;
const	unsigned short TM_SC_BATTLE_ARENA_EXERCISE_READY_STATUS = 4709;
const	unsigned short TM_CS_BATTLE_ARENA_EXERCISE_START = 4710;
const	unsigned short TM_SC_BATTLE_ARENA_BATTLE_STATUS = 4711;
const	unsigned short TM_SC_BATTLE_ARENA_BATTLE_SCORE = 4712;
const	unsigned short TM_SC_BATTLE_ARENA_JOIN_BATTLE = 4713;
const	unsigned short TM_SC_BATTLE_ARENA_DISCONNECT_BATTLE = 4714;
const	unsigned short TM_SC_BATTLE_ARENA_RECONNECT_BATTLE = 4715;
const	unsigned short TM_SC_BATTLE_ARENA_RESULT = 4716;
const	unsigned short TM_CS_BATTLE_ARENA_ABSENCE_CHECK_REQUEST = 4717;
const	unsigned short TM_SC_BATTLE_ARENA_ABSENCE_CHECK = 4718;
const	unsigned short TM_CS_BATTLE_ARENA_ABSENCE_CHECK_ANSWER = 4719;

// 랭킹 시스템 관련
const	unsigned short TM_CS_RANKING_TOP_RECORD = 5000;
const	unsigned short TM_SC_RANKING_TOP_RECORD = 5001;

// 크리처 농장 관련
const	unsigned short TM_CS_REQUEST_FARM_INFO = 6000;
const	unsigned short TM_SC_FARM_INFO = 6001;
const	unsigned short TM_CS_FOSTER_CREATURE = 6002;
const	unsigned short TM_SC_RESULT_FOSTER = 6003;
const	unsigned short TM_CS_RETRIEVE_CREATURE = 6004;
const	unsigned short TM_SC_RESULT_RETRIEVE = 6005;
const	unsigned short TM_CS_NURSE_CREATURE = 6006;
const	unsigned short TM_SC_RESULT_NURSE = 6007;
const	unsigned short TM_CS_REQUEST_FARM_MARKET = 6008;


const	unsigned short TM_CS_PARTYMATCH_ACTION = 7000;
const	unsigned short TM_SC_PARTYMATCH_LIST = 7001;
const	unsigned short TM_SC_PARTYMATCH_MEMBER = 7002;




// { Auth Server <-> Game Server
const	unsigned short TM_GA_LOGIN = 20001;
const	unsigned short TM_AG_LOGIN_RESULT = 20002;

const	unsigned short TM_GA_CLIENT_LOGIN = 20010;
const	unsigned short TM_AG_CLIENT_LOGIN = 20011;
const	unsigned short TM_GA_CLIENT_LOGOUT = 20012;
const	unsigned short TM_AG_KICK_CLIENT = 20013;
const	unsigned short TM_GA_CLIENT_KICK_FAILED = 20014;

const	unsigned short TM_AG_PCBANG_EXPIRE_WARNING = 20020;
const	unsigned short TM_AG_PCBANG_EXPIRED = 20021;
const	unsigned short TM_AG_PCBANG_CHANGED = 20022;

const	unsigned short TM_AG_ITEM_PURCHASED = 30000;
const	unsigned short TM_AG_ITEM_SUPPLIED = 30001;

const	unsigned short TM_AG_SECURITY_NO_CHECK = 40000;
const	unsigned short TM_GA_SECURITY_NO_CHECK = 40001;
// }

// { Upload Server <-> Game Server
const	unsigned short TM_SU_LOGIN = 50001;
const	unsigned short TM_US_LOGIN_RESULT = 50002;

const	unsigned short TM_SU_REQUEST_UPLOAD = 50003;
const	unsigned short TM_US_REQUEST_UPLOAD = 50004;
//const	unsigned short TM_CU_LOGIN				= 50005;
//const	unsigned short TM_UC_LOGIN_RESULT		= 50006;
//const	unsigned short TM_CU_UPLOAD				= 50007;
//const	unsigned short TM_UC_UPLOAD				= 50008;
const	unsigned short TM_US_UPLOAD = 50009;

#pragma pack( 1 )

// }
#define _INIT(_id)  { memset( this, 0, sizeof(*this) ); id = _id; size = sizeof( *this ); }


struct TS_MESSAGE
{
	TS_MESSAGE()
	{
		size = sizeof(*this);
	}

	unsigned int		size;
	unsigned short		id;
	unsigned char		msg_check_sum;

	bool	IsValidMessage() const
	{
		return (msg_check_sum == get_check_sum());
	}

	void	set_check_sum()
	{
		msg_check_sum = get_check_sum();
	}

protected:

	unsigned char get_check_sum() const
	{
		const char* p = reinterpret_cast<const char*>(&size);
		unsigned char check_sum = 0;

		while (p != reinterpret_cast<const char*>(&msg_check_sum))
		{
			check_sum += *p;
			++p;
		}

		return check_sum;
	}
};

struct TS_SC_RESULT : TS_MESSAGE
{
	TS_SC_RESULT(unsigned short _request_msg_id, unsigned short _result, int _value) : request_msg_id(_request_msg_id), result(_result), value(_value)
	{
		size = sizeof(*this);
		id = TM_SC_RESULT;
		set_check_sum();
	}

	unsigned short		request_msg_id;
	unsigned short		result;
	int					value;
};

struct TS_CS_REPORT : TS_MESSAGE
{
	TS_CS_REPORT()			_INIT(TM_CS_REPORT)

		unsigned short		report_len;
};

struct TS_SC_OPEN_URL : TS_MESSAGE
{
	TS_SC_OPEN_URL()		_INIT(TM_SC_OPEN_URL)

		bool					wait_for_event_scene;

	int						width;
	int						height;

	unsigned short			url_len;
	// 이 뒤로 url_len 만큼의 Url이 붙어감.
};

struct TS_SC_URL_LIST : TS_MESSAGE
{
	TS_SC_URL_LIST()		_INIT(TM_SC_URL_LIST)

		unsigned short url_list_len;

	// 이 뒤로 길이 url_list_len 의 문자열이 붙어감.
};

struct TS_SC_ANTI_HACK : TS_MESSAGE
{
	TS_SC_ANTI_HACK()						_INIT(TM_SC_ANTI_HACK)

		AHNHS_TRANS_BUFFER			AhnHSReqBuffer;
};

struct TS_CS_ANTI_HACK : TS_MESSAGE
{
	TS_CS_ANTI_HACK()						_INIT(TM_CS_ANTI_HACK)

		AHNHS_TRANS_BUFFER			AhnHSAckBuffer;
};

struct TS_SC_GAME_GUARD_AUTH_QUERY : TS_MESSAGE
{
	TS_SC_GAME_GUARD_AUTH_QUERY()			_INIT(TM_SC_GAME_GUARD_AUTH_QUERY)

		unsigned int				auth_data_size;
	// 뒤에 auth_data_size 만큼 데이터가 추가됨
};

struct TS_CS_GAME_GUARD_AUTH_ANSWER : TS_MESSAGE
{
	TS_CS_GAME_GUARD_AUTH_ANSWER()			_INIT(TM_CS_GAME_GUARD_AUTH_ANSWER)

		unsigned int				auth_data_size;
	// 뒤에 auth_data_size 만큼 데이터가 추가됨
};

struct TS_CS_CHECK_ILLEGAL_USER : TS_MESSAGE
{
	TS_CS_CHECK_ILLEGAL_USER()				_INIT(TM_CS_CHECK_ILLEGAL_USER)

		unsigned long				log_code;
};

struct TS_SC_XTRAP_CHECK : TS_MESSAGE
{
	TS_SC_XTRAP_CHECK()			_INIT(TM_SC_XTRAP_CHECK)

		unsigned char			pCheckBuffer[XTRAP_SETINFO_PACKETBUFF_SIZE];
};

struct TS_CS_XTRAP_CHECK : TS_MESSAGE
{
	TS_CS_XTRAP_CHECK()			_INIT(TM_CS_XTRAP_CHECK)

		unsigned char			pCheckBuffer[XTRAP_SETINFO_PACKETBUFF_SIZE];
};

struct TS_AG_PCBANG_EXPIRE_WARNING : TS_MESSAGE
{
	TS_AG_PCBANG_EXPIRE_WARNING()	_INIT(TM_AG_PCBANG_EXPIRE_WARNING)

		char				account[GameRule::MAX_ACCOUNT_LEN + 1];
	int					remain_minutes;
};

struct TS_AG_PCBANG_EXPIRE : TS_MESSAGE
{
	TS_AG_PCBANG_EXPIRE()	_INIT(TM_AG_PCBANG_EXPIRED)

		char				account[GameRule::MAX_ACCOUNT_LEN + 1];
};

struct TS_AG_PCBANG_CHANGED : TS_MESSAGE
{
	TS_AG_PCBANG_CHANGED()	_INIT(TM_AG_PCBANG_CHANGED)

		char				account[GameRule::MAX_ACCOUNT_LEN + 1];
	int					nPCBangMode;
};

struct TS_CS_VERSION : TS_MESSAGE
{
	TS_CS_VERSION()			_INIT(TM_CS_VERSION)

		char				szVersion[20];
	unsigned char		pKey[256];

	// szVersion이 서버 정보 값을 요구하는 새 패킷이라면, 그 카운트만큼 뒤에 이 구조체가 따라 붙음.
	struct ServerENVInfo
	{
		ServerENVInfo()
		{
			memset(szData, 0, sizeof(szData));
		}
		char	szData[40];
	};
};

struct TS_SC_INFO : TS_MESSAGE

{
	const static int MAX_QUERY_COUNT = 20;

	TS_SC_INFO()			_INIT(TM_SC_INFO)

		// 뒤에 count만큼 ServerENVResult가 따라 붙음
		int					count;

	struct ServerENVResult
	{
		ServerENVResult()
		{
			memset(szData, 0, sizeof(szData));
		}

		char	szData[40];
	};
};

struct TS_CS_REQUEST : TS_MESSAGE
{
	TS_CS_REQUEST()			_INIT(TM_CS_REQUEST)

		char				t;
};

struct TS_LOGIN : TS_MESSAGE
{
	TS_LOGIN()			_INIT(TM_CS_LOGIN)

		// below is game conetents
		char			szName[24];
	unsigned char	pzKey[256];
	//unsigned char	race;
};

#ifdef USE_CLIENT97

struct TS_LOGIN_RESULT : TS_MESSAGE
{
	TS_LOGIN_RESULT()	_INIT(TM_SC_LOGIN_RESULT)

		unsigned short	result;

	AR_HANDLE		handle;
	AR_UNIT			x, y, z;
	int				layer;
	float			face_direction;

	int				region_size;

	// below is game conetents
	int				hp;
	int				mp;
	int				max_hp;
	int				max_mp;
	int				sex;
	int				race;
	unsigned long	skin_color;
	ItemBase::ItemCode	faceId;
	ItemBase::ItemCode	hairId;

#if defined USE_CLIENT96 || defined USE_CLIENT97
	char			szName[20];
#else
	char			szName[19];
#endif

	int				cell_size;
	int				guild_id;
	int				unk1;
};

#else
struct TS_LOGIN_RESULT : TS_MESSAGE
{
	TS_LOGIN_RESULT()	_INIT(TM_SC_LOGIN_RESULT)

		unsigned short	result;

	AR_HANDLE		handle;
	AR_UNIT			x, y, z;
	unsigned char	layer;
	float			face_direction;

	int				region_size;

	// below is game conetents
	int				hp;
	int				mp;
	int				max_hp;
	int				max_mp;
	int				sex;
	int				race;
	unsigned long	skin_color;
	ItemBase::ItemCode	faceId;
	ItemBase::ItemCode	hairId;

#if defined USE_CLIENT96
	char			szName[20];
#else
	char			szName[19];
#endif

	int				cell_size;
	int				guild_id;
};

#endif


struct TS_SC_CHANGE_NAME : TS_MESSAGE
{
	TS_SC_CHANGE_NAME()		_INIT(TM_SC_CHANGE_NAME);

	AR_HANDLE			handle;
	char				name[19];
};

struct TS_TIMESYNC : TS_MESSAGE
{
	TS_TIMESYNC()		_INIT(TM_TIMESYNC)

		AR_TIME			time;
};

struct TS_CS_QUERY : TS_MESSAGE
{
	TS_CS_QUERY()			_INIT(TM_CS_QUERY)

		AR_HANDLE handle;
};

#ifdef USE_CLIENT97
struct TS_ENTER : TS_MESSAGE
{
	TS_ENTER()			_INIT(TM_SC_ENTER)

		enum
	{
		PLAYER = 2, NPC = 1, STATIC_OBJECT = 0
	};

	enum //Game Obj Type
	{
		GAME_PLAYER = 0,
		GAME_NPC = 1,
		GAME_ITEM = 2,
		GAME_MOB = 3,
		GAME_SUMMON = 4,
		GAME_SKILL_PROP = 5,
		GAME_FIELD_PROP = 6,
		GAME_PET = 7
	};

	unsigned char	type;
	unsigned char		ObjType;
	unsigned char	layer;
	unsigned char	crap;
	AR_HANDLE		handle;
	AR_UNIT			x, y, z;



	// below is game conetents


	struct TS_ITEM_PICK_UP_ORDER
	{
		AR_TIME					drop_time;
		AR_HANDLE				hPlayer[3];
		int						nPartyID[3];
		/*
		먹자 방지 룰

		drop_time부터 30초 이내:
		pickup_order.hPlayer[0] 가 0이 아닐 경우, hPlayer[0]인 사용자만 집을 수 있습니다.
		pickup_order.hPlayer[0] 가 0이고, pickup_order.nPartyID[0] > 0 일 경우, pickup_order.nPartyID[0]인 파티 소속원만 집을 수 있습니다.

		drop_time부터 40초 이내:
		pickup_order.hPlayer[1] 가 0이 아닐 경우, hPlayer[1]인 사용자만 집을 수 있습니다.
		pickup_order.hPlayer[1] 가 0이고, pickup_order.nPartyID[1] > 0 일 경우, pickup_order.nPartyID[1]인 파티 소속원만 집을 수 있습니다.

		drop_time부터 50초 이내:
		pickup_order.hPlayer[2] 가 0이 아닐 경우, hPlayer[2]인 사용자만 집을 수 있습니다.
		pickup_order.hPlayer[2] 가 0이고, pickup_order.nPartyID[2] > 0 일 경우, pickup_order.nPartyID[2]인 파티 소속원만 집을 수 있습니다.

		50초 이후엔 아무나 집을 수 있습니다.
		*/
	};

	struct ItemInfo
	{
		ENC_INT					code;	// code 가 0인것은 돈임. 액수는 cnt
		__int64					cnt;

		TS_ITEM_PICK_UP_ORDER	pick_up_order;
	};

	struct SkillInfo
	{
		AR_HANDLE	caster;
		AR_TIME		start_time;
		int			skill_num;
	};

	struct FieldPropInfo
	{
		int			prop_id;
		float		fZOffset;
		float		fRotateX, fRotateY, fRotateZ;
		float		fScaleX, fScaleY, fScaleZ;
		bool		bLockHeight;
		float		fLockHeight;
	};

	struct CreatureInfo
	{
		enum
		{
			FLAG_BATTLE_MODE = 1 << 0,
			FLAG_INVISIBLE = 1 << 1,
		};

		unsigned			status;
		float				face_direction;	// 보고있는 방향
		int					hp;
		int					max_hp;
		int					mp;
		int					max_mp;
		int					level;
		short				race;
		short				sex;
		//		unsigned char		race;
		//		unsigned long		skin_color;
		//		bool				is_first_enter;
		//		int					energy;
	};


	struct SummonInfo : CreatureInfo
	{
		AR_HANDLE			master_handle;
		ENC_INT				summon_code;
		char				szName[20];
		unsigned char		enhance;
	};

#ifdef USE_CLIENT97
	struct NPCInfo
	{
		enum
		{
			FLAG_HAS_STARTABLE_QUEST = 1 << 8,
			FLAG_HAS_IN_PROGRESS_QUEST = 1 << 9,
			FLAG_HAS_FINISHABLE_QUEST = 1 << 10,
		};
		unsigned			status;
		float				face_direction;	// 보고있는 방향

		ENC_INT				npc_id;
		//int					model_id;
		//char				szName[25];
	};

	struct MonsterInfo : CreatureInfo
	{
		enum
		{
			FLAG_DEAD = 1 << 8,
			FLAG_DUNGEON_ORIGINAL_OWNER = 1 << 15,
			FLAG_DUNGEON_ORIGINAL_SIEGER = 1 << 17,
		};

		ENC_INT				monster_id;
		bool				is_tamed;		// 테이밍 되었는지 여부
	};

	struct PlayerInfo : CreatureInfo
	{
		enum
		{
			FLAG_SITDOWN = 1 << 8,
			FLAG_BUY_BOOTH = 1 << 9,
			FLAG_SELL_BOOTH = 1 << 10,
			FLAG_PK_ON = 1 << 11,
			FLAG_BLOODY = 1 << 12,
			FLAG_DEMONIAC = 1 << 13,
			FLAG_GM = 1 << 14,
			FLAG_DUNGEON_ORIGINAL_OWNER = 1 << 15,
			FLAG_WALKING = 1 << 16,
			FLAG_DUNGEON_ORIGINAL_SIEGER = 1 << 17,
			FLAG_SHOVELING_SEARCH = 1 << 18,
			FLAG_SHOVELING_APPROACH = 1 << 19,
			FLAG_SHOVELING_DIG = 1 << 20,
			FLAG_COMPETING = 1 << 21,
			FLAG_BATTLE_ARENA_TEAM_0 = 1 << 22,
			FLAG_BATTLE_ARENA_TEAM_1 = 1 << 23,
		};
		unsigned long		skin_color;
		bool				is_first_enter;

		ItemBase::ItemCode	faceId;
		int					faceTextureId;
		int					hairId;
		int					hairColorIndex;
		unsigned int		hairColorRGB;

		unsigned int		hideEquipFlag;

		char	szName[20];

		unsigned short	job_id;

		AR_HANDLE		ride_handle;

		int				guild_id;
		int				title_code;
		unsigned short sex2;
		unsigned short unkp;
		int					energy;
	};


#else
	struct NPCInfo : CreatureInfo
	{
		enum
		{
			FLAG_HAS_STARTABLE_QUEST = 1 << 8,
			FLAG_HAS_IN_PROGRESS_QUEST = 1 << 9,
			FLAG_HAS_FINISHABLE_QUEST = 1 << 10,
		};

		ENC_INT				npc_id;
		//int					model_id;
		//char				szName[25];
	};

	struct MonsterInfo : CreatureInfo
	{
		enum
		{
			FLAG_DEAD = 1 << 8,
			FLAG_DUNGEON_ORIGINAL_OWNER = 1 << 15,
			FLAG_DUNGEON_ORIGINAL_SIEGER = 1 << 17,
		};

		ENC_INT				monster_id;
		bool				is_tamed;		// 테이밍 되었는지 여부
	};

	struct PlayerInfo : CreatureInfo
	{
		enum
		{
			FLAG_SITDOWN = 1 << 8,
			FLAG_BUY_BOOTH = 1 << 9,
			FLAG_SELL_BOOTH = 1 << 10,
			FLAG_PK_ON = 1 << 11,
			FLAG_BLOODY = 1 << 12,
			FLAG_DEMONIAC = 1 << 13,
			FLAG_GM = 1 << 14,
			FLAG_DUNGEON_ORIGINAL_OWNER = 1 << 15,
			FLAG_WALKING = 1 << 16,
			FLAG_DUNGEON_ORIGINAL_SIEGER = 1 << 17,
			FLAG_SHOVELING_SEARCH = 1 << 18,
			FLAG_SHOVELING_APPROACH = 1 << 19,
			FLAG_SHOVELING_DIG = 1 << 20,
			FLAG_COMPETING = 1 << 21,
			FLAG_BATTLE_ARENA_TEAM_0 = 1 << 22,
			FLAG_BATTLE_ARENA_TEAM_1 = 1 << 23,
		};
		unsigned char sex;

		ItemBase::ItemCode	faceId;
		int					faceTextureId;
		int					hairId;
		int					hairColorIndex;
		unsigned int		hairColorRGB;

		unsigned int		hideEquipFlag;

		char	szName[20];

		unsigned short	job_id;

		AR_HANDLE		ride_handle;

		int				guild_id;
		int				title_code;
	};


#endif

	struct PetInfo : CreatureInfo
	{
		AR_HANDLE		master_handle;
		ENC_INT			pet_code;
		char			szName[19];
	};
};

#else

struct TS_ENTER : TS_MESSAGE
{
	TS_ENTER()			_INIT(TM_SC_ENTER)

		enum
	{
		PLAYER = 0, NPC = 1, STATIC_OBJECT = 2
	};

	enum //Game Obj Type
	{
		GAME_PLAYER = 0,
		GAME_NPC = 1,
		GAME_ITEM = 2,
		GAME_MOB = 3,
		GAME_SUMMON = 4,
		GAME_SKILL_PROP = 5,
		GAME_FIELD_PROP = 6,
		GAME_PET = 7
	};

	unsigned char	type;
	AR_HANDLE		handle;
	AR_UNIT			x, y, z;
	unsigned char	layer;

	// below is game conetents

	unsigned char		ObjType;

	struct TS_ITEM_PICK_UP_ORDER
	{
		AR_TIME					drop_time;
		AR_HANDLE				hPlayer[3];
		int						nPartyID[3];
		/*
		먹자 방지 룰

		drop_time부터 30초 이내:
		pickup_order.hPlayer[0] 가 0이 아닐 경우, hPlayer[0]인 사용자만 집을 수 있습니다.
		pickup_order.hPlayer[0] 가 0이고, pickup_order.nPartyID[0] > 0 일 경우, pickup_order.nPartyID[0]인 파티 소속원만 집을 수 있습니다.

		drop_time부터 40초 이내:
		pickup_order.hPlayer[1] 가 0이 아닐 경우, hPlayer[1]인 사용자만 집을 수 있습니다.
		pickup_order.hPlayer[1] 가 0이고, pickup_order.nPartyID[1] > 0 일 경우, pickup_order.nPartyID[1]인 파티 소속원만 집을 수 있습니다.

		drop_time부터 50초 이내:
		pickup_order.hPlayer[2] 가 0이 아닐 경우, hPlayer[2]인 사용자만 집을 수 있습니다.
		pickup_order.hPlayer[2] 가 0이고, pickup_order.nPartyID[2] > 0 일 경우, pickup_order.nPartyID[2]인 파티 소속원만 집을 수 있습니다.

		50초 이후엔 아무나 집을 수 있습니다.
		*/
	};

	struct ItemInfo
	{
		ENC_INT					code;	// code 가 0인것은 돈임. 액수는 cnt
		__int64					cnt;

		TS_ITEM_PICK_UP_ORDER	pick_up_order;
	};

	struct SkillInfo
	{
		AR_HANDLE	caster;
		AR_TIME		start_time;
		int			skill_num;
	};

	struct FieldPropInfo
	{
		int			prop_id;
		float		fZOffset;
		float		fRotateX, fRotateY, fRotateZ;
		float		fScaleX, fScaleY, fScaleZ;
		bool		bLockHeight;
		float		fLockHeight;
	};

	struct CreatureInfo
	{
		enum
		{
			FLAG_BATTLE_MODE = 1 << 0,
			FLAG_INVISIBLE = 1 << 1,
		};

		unsigned			status;

		float				face_direction;	// 보고있는 방향
		int					hp;
		int					max_hp;
		int					mp;
		int					max_mp;
		int					level;
		unsigned char		race;
		unsigned long		skin_color;
		bool				is_first_enter;
		int					energy;
	};

	struct MonsterInfo : CreatureInfo
	{
		enum
		{
			FLAG_DEAD = 1 << 8,
			FLAG_DUNGEON_ORIGINAL_OWNER = 1 << 15,
			FLAG_DUNGEON_ORIGINAL_SIEGER = 1 << 17,
		};

		ENC_INT				monster_id;
		bool				is_tamed;		// 테이밍 되었는지 여부
	};

	struct SummonInfo : CreatureInfo
	{
		AR_HANDLE			master_handle;
		ENC_INT				summon_code;
		char				szName[19];
		unsigned char		enhance;
	};

	struct NPCInfo : CreatureInfo
	{
		enum
		{
			FLAG_HAS_STARTABLE_QUEST = 1 << 8,
			FLAG_HAS_IN_PROGRESS_QUEST = 1 << 9,
			FLAG_HAS_FINISHABLE_QUEST = 1 << 10,
		};

		ENC_INT				npc_id;
		//int					model_id;
		//char				szName[25];
	};

	struct PlayerInfo : CreatureInfo
	{
		enum
		{
			FLAG_SITDOWN = 1 << 8,
			FLAG_BUY_BOOTH = 1 << 9,
			FLAG_SELL_BOOTH = 1 << 10,
			FLAG_PK_ON = 1 << 11,
			FLAG_BLOODY = 1 << 12,
			FLAG_DEMONIAC = 1 << 13,
			FLAG_GM = 1 << 14,
			FLAG_DUNGEON_ORIGINAL_OWNER = 1 << 15,
			FLAG_WALKING = 1 << 16,
			FLAG_DUNGEON_ORIGINAL_SIEGER = 1 << 17,
			FLAG_SHOVELING_SEARCH = 1 << 18,
			FLAG_SHOVELING_APPROACH = 1 << 19,
			FLAG_SHOVELING_DIG = 1 << 20,
			FLAG_COMPETING = 1 << 21,
			FLAG_BATTLE_ARENA_TEAM_0 = 1 << 22,
			FLAG_BATTLE_ARENA_TEAM_1 = 1 << 23,
		};
		unsigned char sex;

		ItemBase::ItemCode	faceId;
		int					faceTextureId;
		int					hairId;
		int					hairColorIndex;
		unsigned int		hairColorRGB;

		unsigned int		hideEquipFlag;

		char	szName[19];

		unsigned short	job_id;

		AR_HANDLE		ride_handle;

		int				guild_id;
		int				title_code;
	};

	struct PetInfo : CreatureInfo
	{
		AR_HANDLE		master_handle;
		ENC_INT			pet_code;
		char			szName[19];
	};
};

#endif

struct TS_SC_STATUS_CHANGE : TS_MESSAGE
{
	TS_SC_STATUS_CHANGE()		_INIT(TM_SC_STATUS_CHANGE)

		AR_HANDLE		handle;
	unsigned		status;
};

struct TS_CS_RESURRECTION : TS_MESSAGE
{
	TS_CS_RESURRECTION()			_INIT(TM_CS_RESURRECTION)

		AR_HANDLE		handle;

	enum
	{
		RESURRECT_TYPE_NORMAL = 0,	// 일반 부활
		RESURRECT_TYPE_BY_STATE = 1,	// 버프 부활
		RESURRECT_TYPE_BY_POTION = 2,	// 대모 요정의 병 부활
		RESURRECT_TYPE_BY_COMPETE = 3,	// 대련 사망 패배 후 부활
		RESURRECT_TYPE_BY_DEATHMATCH = 4,	// 데스매치에서 사망 후 부활
	};

	char			type;
};

struct TS_SC_TARGET : TS_MESSAGE
{
	TS_SC_TARGET()			_INIT(TM_SC_TARGET)

		AR_HANDLE		target;
};


struct TS_CS_TARGETING : TS_MESSAGE
{
	TS_CS_TARGETING()			_INIT(TM_CS_TARGETING)

		AR_HANDLE		target;
};

struct TS_SC_STATE : TS_MESSAGE
{
	TS_SC_STATE()			_INIT(TM_SC_STATE)

		AR_HANDLE		handle;			// 대상 크리쳐
	unsigned short	state_handle;	// 상태이상 핸들
	unsigned short	state_level;	// 상태이상의 레벨 (0이면 풀리는 메시지임)
	unsigned int	state_code;		// 상태이상 종류

	AR_TIME			end_time;		// 끝나는 시간 (0이면 영원히) ( -1 : aura )
	AR_TIME			start_time;		// 시작시간

	int				state_value;	// 지속효과 관련 추가값

//	char			state_string_value[32];
};

struct TS_SC_PROPERTY : TS_MESSAGE
{
	TS_SC_PROPERTY()					_INIT(TM_SC_PROPERTY)

		AR_HANDLE	handle;

	bool		is_number;				// 이게 참이라면 value 에 숫자가 들어감. 거짓이라면 문자열 값인데 메시지 끝이 '\0' 으로 끝나는 문자열이 들어감.
	char		name[16];
	__int64		value;
};

struct TS_CS_SET_PROPERTY : TS_MESSAGE
{
	TS_CS_SET_PROPERTY()					_INIT(TM_CS_SET_PROPERTY)

		char		name[16];
};

//struct TS_SC_HP : TS_MESSAGE
//{
//	TS_SC_HP()					_INIT( TM_SC_HP )
//
//	AR_HANDLE	handle;
//	int			hp;
//	unsigned char hp_percentage;
//};
//
struct TS_SC_HPMP : TS_MESSAGE
{
	TS_SC_HPMP()					_INIT(TM_SC_HPMP)

		AR_HANDLE	handle;
	int			add_hp;
	int			hp;
	int			max_hp;
	int			add_mp;
	int			mp;
	int			max_mp;
	bool		need_to_display;
};

//struct TS_SC_MP : TS_MESSAGE
//{
//	TS_SC_MP()					_INIT( TM_SC_MP )
//
//	AR_HANDLE	handle;
//	short		mp;
//};

struct TS_SC_SP : TS_MESSAGE
{
	TS_SC_SP()					_INIT(TM_SC_SP)

		AR_HANDLE	handle;
	short		sp;
	short		max_sp;
};

struct TS_SC_ENERGY : TS_MESSAGE
{
	TS_SC_ENERGY()					_INIT(TM_SC_ENERGY)

		AR_HANDLE	handle;
	short		energy;
};

struct TS_SC_REGEN_HPMP : TS_MESSAGE
{
	TS_SC_REGEN_HPMP()					_INIT(TM_SC_REGEN_HPMP)

		AR_HANDLE	handle;
	int			hp_regen;
	int			mp_regen;
	int			hp;
	int			mp;
};

struct TS_CS_MONSTER_RECOGNIZE : TS_MESSAGE
{
	TS_CS_MONSTER_RECOGNIZE()		_INIT(TM_CS_MONSTER_RECOGNIZE)

		AR_HANDLE	recognizer_handle;
	AR_HANDLE	monster_handle;
};

struct TS_CS_UPDATE : TS_MESSAGE
{
	TS_CS_UPDATE()					_INIT(TM_CS_UPDATE)

		AR_HANDLE		handle;
};

struct TS_SC_TAMING_INFO : TS_MESSAGE
{
	TS_SC_TAMING_INFO()				_INIT(TM_SC_TAMING_INFO)

		enum
	{
		START = 0,	// tamer 가 target 에게 테이밍 스킬을 걸었음
		CLEAR = 1,	// 테이밍이 초기화됨 (tamer_handle 은 0)
		SUCCESS = 2,	// target 이 죽어서 tamer 에게 테이밍됨.
		FAILED = 3,	// target 이 죽었지만 테이밍은 실패!
	};

	unsigned char	mode;

	AR_HANDLE		tamer_handle;
	AR_HANDLE		target_handle;
};

struct TS_WEAR_INFO : TS_MESSAGE
{
	TS_WEAR_INFO()		_INIT(TM_SC_WEAR_INFO)

		AR_HANDLE			handle;
	ItemBase::ItemCode	itemCode[ItemBase::MAX_SPARE_ITEM_WEAR];
	int					itemEnhance[ItemBase::MAX_SPARE_ITEM_WEAR];
	int					itemLevel[ItemBase::MAX_SPARE_ITEM_WEAR];
	unsigned char		elemental_effect_type[ItemBase::MAX_SPARE_ITEM_WEAR];
	ItemBase::ItemCode	appearance_code[ItemBase::MAX_SPARE_ITEM_WEAR];
};

struct TS_SC_ITEM_WEAR_INFO : TS_MESSAGE
{
	TS_SC_ITEM_WEAR_INFO()		_INIT(TM_SC_ITEM_WEAR_INFO)

		AR_HANDLE					item_handle;
	short						wear_position;		// -1이면 장착 안된 아이템
	AR_HANDLE					target_handle;
	int							enhance;
	unsigned char				elemental_effect_type;
	ItemBase::ItemCode			appearance_code;
};

struct TS_LEAVE : TS_MESSAGE
{
	TS_LEAVE()			_INIT(TM_SC_LEAVE)

		AR_HANDLE		handle;
};

struct TS_MOVE_REQUEST : TS_MESSAGE
{
	TS_MOVE_REQUEST()	_INIT(TM_CS_MOVE_REQUEST)

		AR_HANDLE	handle;
	//	AR_UNIT		tx,ty;

	AR_UNIT		x, y;

	AR_TIME		cur_time;
	//	unsigned char tlayer;

	bool		speed_sync;

	unsigned short	count;

	struct MOVE_INFO
	{
		AR_UNIT		tx, ty;
	};
	// 이 뒤에 MOVE_INFO가 COUNT만큼 붙어서 온다.
};

struct TS_MOVE : TS_MESSAGE
{
	TS_MOVE()			_INIT(TM_SC_MOVE)

		AR_TIME			start_time;
	AR_HANDLE		handle;
	//AR_UNIT			tx, ty, tz;

	unsigned char	tlayer;
	unsigned char	speed;

	unsigned short	count;

	struct MOVE_INFO
	{
		AR_UNIT		tx, ty;
	};
};

struct TS_SC_MOUNT_SUMMON : TS_MESSAGE
{
	TS_SC_MOUNT_SUMMON()	_INIT(TM_SC_MOUNT_SUMMON)

		AR_HANDLE		handle;
	AR_HANDLE		summon_handle;
	AR_UNIT			x, y;
	bool			success;
};

struct TS_SC_UNMOUNT_SUMMON : TS_MESSAGE
{
	TS_SC_UNMOUNT_SUMMON()	_INIT(TM_SC_UNMOUNT_SUMMON)

		AR_HANDLE		handle;
	AR_HANDLE		summon_handle;

	enum
	{
		NORMAL = 0,
		FALL = 1,
		UNSUMMON = 2,
	};

	char			flag;
};

struct TS_SC_SHOW_SUMMON_NAME_CHANGE : TS_MESSAGE
{
	TS_SC_SHOW_SUMMON_NAME_CHANGE()	_INIT(TM_SC_SHOW_SUMMON_NAME_CHANGE)

		AR_HANDLE		handle;
};

struct TS_CS_CHANGE_SUMMON_NAME : TS_MESSAGE
{
	TS_CS_CHANGE_SUMMON_NAME()		_INIT(TM_CS_CHANGE_SUMMON_NAME)

		char			szName[19];
};

struct TS_CS_GET_SUMMON_SETUP_INFO : TS_MESSAGE
{
	TS_CS_GET_SUMMON_SETUP_INFO()	_INIT(TM_CS_GET_SUMMON_SETUP_INFO)

		bool			show_dialog;
};

struct TS_SC_UNSUMMON_PET : TS_MESSAGE
{
	TS_SC_UNSUMMON_PET()			_INIT(TM_SC_UNSUMMON_PET)

		AR_HANDLE		handle;
};

struct TS_SC_ADD_PET_INFO : TS_MESSAGE
{
	TS_SC_ADD_PET_INFO()			_INIT(TM_SC_ADD_PET_INFO)

		AR_HANDLE		cage_handle;
	AR_HANDLE		pet_handle;

	char			name[19];
	int				code;
};

struct TS_SC_REMOVE_PET_INFO : TS_MESSAGE
{
	TS_SC_REMOVE_PET_INFO()			_INIT(TM_SC_REMOVE_PET_INFO)

		AR_HANDLE		handle;
};

struct TS_SC_SHOW_SET_PET_NAME : TS_MESSAGE
{
	TS_SC_SHOW_SET_PET_NAME()		_INIT(TM_SC_SHOW_SET_PET_NAME)

		AR_HANDLE		handle;
};

struct TS_CS_SET_PET_NAME : TS_MESSAGE
{
	TS_CS_SET_PET_NAME()			_INIT(TM_CS_SET_PET_NAME)

		AR_HANDLE		handle;
	char			name[19];
};

struct TS_FORCE_MOVE : TS_MESSAGE
{
	TS_FORCE_MOVE()			_INIT(TM_SC_FORCE_MOVE)

		AR_HANDLE		handle;
	AR_UNIT			x, y, z;
	unsigned char	layer;
	float			face;
};

struct TS_REGION_UPDATE : TS_MESSAGE
{
	TS_REGION_UPDATE()	_INIT(TM_CS_REGION_UPDATE)

		AR_TIME			update_time;
	AR_UNIT			x, y, z;
	bool			bIsStopMessage;
};

struct TS_SC_WARP : TS_MESSAGE
{
	TS_SC_WARP()	_INIT(TM_SC_WARP)

		AR_UNIT			x, y, z;
	unsigned char	layer;
};

struct TS_REGION_ACK : TS_MESSAGE
{
	TS_REGION_ACK()		_INIT(TM_SC_REGION_ACK)

		unsigned		rx, ry;
};

struct TS_CS_ENTER_EVENT_AREA : TS_MESSAGE
{
	TS_CS_ENTER_EVENT_AREA()	_INIT(TM_CS_ENTER_EVENT_AREA)

		int				event_area_id;
	int				area_index;
};

struct TS_CS_LEAVE_EVENT_AREA : TS_MESSAGE
{
	TS_CS_LEAVE_EVENT_AREA()	_INIT(TM_CS_LEAVE_EVENT_AREA)

		int				event_area_id;
	int				area_index;
};

struct TS_MOVE_ACK : TS_MESSAGE
{
	TS_MOVE_ACK()		_INIT(TM_SC_MOVE_ACK)

		AR_TIME			time;
	unsigned char	speed;
};

struct TS_SET_TIME : TS_MESSAGE
{
	TS_SET_TIME()		_INIT(TM_SC_SET_TIME)

		int		gap;
};

struct TS_ATTACK_REQUEST : TS_MESSAGE
{
	TS_ATTACK_REQUEST()		_INIT(TM_CS_ATTACK_REQUEST)

		AR_HANDLE		handle;
	AR_HANDLE		target_handle;	// NULL 이면 공격 중지 메시지
};

struct TS_SC_STATE_RESULT : TS_MESSAGE
{
	TS_SC_STATE_RESULT()		_INIT(TM_SC_STATE_RESULT)

		AR_HANDLE		caster_handle;
	AR_HANDLE		target_handle;

	int				code;
	unsigned short	level;

	// result_type 종류
	enum
	{
		STATE_DAMAGE_HP = 1,
		STATE_DAMAGE_MP = 2,
		STATE_DAMAGE_SP = 3,
		STATE_HEAL_HP = 4,
		STATE_HEAL_MP = 5,
		STATE_HEAL_SP = 6,
	};

	unsigned short	result_type;	// 위의 enum 값 중 하나
	int				value;			// enum에 해당하는 종류의 수치
	int				target_value;	// 처리 후 target의 해당 수치

	bool			final;
	int				total_amount;
};

struct TS_ATTACK_EVENT : TS_MESSAGE
{
	TS_ATTACK_EVENT()		_INIT(TM_SC_ATTACK_EVENT)

		AR_HANDLE		attacker_handle;
	AR_HANDLE		target_handle;

	enum
	{
		FLAG_CRITICAL = (1 << 3),
		FLAG_MISS = (1 << 2),
		FLAG_BLOCK = (1 << 1),
		FLAG_PERFECT_BLOCK = (1 << 0),
	};

	struct ATTACK_INFO
	{
		int				damage;
		int				mp_damage;
		unsigned char	flag;
		int				elemental_damage[Elemental::TYPE_COUNT];
		int				target_hp;
		int				target_mp;
		int				attacker_damage;
		int				attacker_mp_damage;
		int				attacker_hp;
		int				attacker_mp;
	};

	unsigned short	attack_speed;			// ms
	unsigned short	attack_delay;			// ms

	enum
	{
		ATTACK_END = 1,
		ATTACK_AIMING = 2,
		ATTACK_ATTACK = 3,
		ATTACK_CANCEL = 4,
	};

	char			attack_action;

	enum
	{
		ATTACK_FLAG_BOW = (1 << 0),
		ATTACK_FLAG_CROSS_BOW = (1 << 1),
		ATTACK_FLAG_DOUBLE_WEAPON = (1 << 2),
		ATTACK_FLAG_DOUBLE_ATTACK = (1 << 3),
	};

	char			attack_flag;

	unsigned char	count;

	// double weapon의 경우, right, left 순서
	// double weapon, double attack의 경우, right, left, right, left 순서
};

struct TS_SC_KNOCK_BACK : TS_MESSAGE
{
	TS_SC_KNOCK_BACK()		_INIT(TM_SC_KNOCK_BACK)

		AR_HANDLE		handle;
	AR_UNIT			x, y, z;
	unsigned char	layer;
	float			face;
	AR_TIME			next_movable_time;
};

struct TS_SC_CANT_ATTACK : TS_MESSAGE
{
	TS_SC_CANT_ATTACK()		_INIT(TM_SC_CANT_ATTACK)

		AR_HANDLE		attacker_handle;
	AR_HANDLE		target_handle;

	int				reason;
};

#ifdef USE_CLIENT97
struct TS_ITEM_BASE_INFO
{
	AR_HANDLE					handle;
	ItemBase::ItemCode			Code;
	ItemUID		uid;

	// TS_SC_ITEM_KEEPING_LIST::TS_ITEM_KEEPING_INFO 에 돈이 들어갈 경우 수량이 액수가 되어야 해서 __int64 타입 사용함
	__int64						count;
	int							ethereal_durability;
	unsigned int				endurance;
	unsigned char				enhance;
	unsigned char				level;
	short						enhance_chance;		// 강화 확률 add
//	short						unk;
	int							Flag;

	int							socket[ItemBase::MAX_SOCKET_NUMBER];

	struct AwakenOption
	{
		int		nValue1[ItemInstance::MAX_AWAKEN_NUMBER];
		int		nValue2[ItemInstance::MAX_AWAKEN_NUMBER];
	};
	AwakenOption				awakenoption;

	struct RandomOption
	{
		int			nType[ItemInstance::MAX_RANDOM_OPTION_NUMBER];
		short	fValue1[ItemInstance::MAX_RANDOM_OPTION_NUMBER];
		short	fValue2[ItemInstance::MAX_RANDOM_OPTION_NUMBER];
	};
	RandomOption				random_option;

	int							remain_time;

	unsigned char				elemental_effect_type;
	int							elemental_effect_remain_time;
	int							elemental_effect_attack_point;
	int							elemental_effect_magic_point;
	ItemBase::ItemCode			appearance_code;
	int							summon_code;
	int							effect_id;
	int unk;
};

#else

struct TS_ITEM_BASE_INFO
{
	AR_HANDLE					handle;
	ItemBase::ItemCode			Code;
	ItemUID		uid;

	// TS_SC_ITEM_KEEPING_LIST::TS_ITEM_KEEPING_INFO 에 돈이 들어갈 경우 수량이 액수가 되어야 해서 __int64 타입 사용함
	__int64						count;
	int							ethereal_durability;
	unsigned int				endurance;
	unsigned char				enhance;
	unsigned char				level;
	short						enhance_chance;		// 강화 확률 add
	int							Flag;

	int							socket[ItemBase::MAX_SOCKET_NUMBER];

	struct AwakenOption
	{
		int		nValue1[ItemInstance::MAX_AWAKEN_NUMBER];
		int		nValue2[ItemInstance::MAX_AWAKEN_NUMBER];
	};
	AwakenOption				awakenoption;

	struct RandomOption
	{
		int			nType[ItemInstance::MAX_RANDOM_OPTION_NUMBER];
		c_fixed10	fValue1[ItemInstance::MAX_RANDOM_OPTION_NUMBER];
		c_fixed10	fValue2[ItemInstance::MAX_RANDOM_OPTION_NUMBER];
	};
	RandomOption				random_option;

	int							remain_time;

	unsigned char				elemental_effect_type;
	int							elemental_effect_remain_time;
	int							elemental_effect_attack_point;
	int							elemental_effect_magic_point;
	ItemBase::ItemCode			appearance_code;
	int							summon_code;
#ifdef USE_ITEMEFFECT_96	
	int							effect_id;
#endif
};

#endif

struct TS_ITEM_INFO : TS_ITEM_BASE_INFO
{
	short						wear_position;		// -1이면 장착 안된 아이템, 
													// -2이면 창고 아이템
													// -3이면 캐쉬템 아이템

	AR_HANDLE					own_summon_handle;	// 소환수 장착 아니면 0

	int							index;
};

struct TS_CS_BIND_SKILLCARD : TS_MESSAGE
{
	TS_CS_BIND_SKILLCARD()		_INIT(TM_CS_BIND_SKILLCARD)

		AR_HANDLE		item_handle;
	AR_HANDLE		target_handle;
};

struct TS_CS_UNBIND_SKILLCARD : TS_MESSAGE
{
	TS_CS_UNBIND_SKILLCARD()		_INIT(TM_CS_UNBIND_SKILLCARD)

		AR_HANDLE		item_handle;
	AR_HANDLE		target_handle;
};

struct TS_SC_SKILLCARD_INFO : TS_MESSAGE
{
	TS_SC_SKILLCARD_INFO()			_INIT(TM_SC_SKILLCARD_INFO)

		AR_HANDLE		item_handle;
	AR_HANDLE		target_handle;	// 0 이면 장착해지
};

#ifdef USE_CLIENT97
struct TS_CS_PUTON_ITEM : TS_MESSAGE
{
	TS_CS_PUTON_ITEM()		_INIT(TM_CS_PUTON_ITEM)

		AR_HANDLE		item_handle;
	AR_HANDLE		target_handle;
	unsigned char	position;
};
#else
struct TS_CS_PUTON_ITEM : TS_MESSAGE
{
	TS_CS_PUTON_ITEM()		_INIT(TM_CS_PUTON_ITEM)7

		unsigned char	position;
	AR_HANDLE		item_handle;
	AR_HANDLE		target_handle;
};
#endif

struct TS_CS_PUTON_ITEM_SET : TS_MESSAGE
{
	TS_CS_PUTON_ITEM_SET()	_INIT(TM_CS_PUTON_ITEM_SET)

		AR_HANDLE		handle[ItemBase::MAX_ITEM_WEAR];
};

struct TS_CS_PUTON_CARD : TS_MESSAGE
{
	TS_CS_PUTON_CARD()		_INIT(TM_CS_PUTON_CARD)

		unsigned char	position;
	AR_HANDLE		item_handle;
};

struct TS_CS_PUTOFF_CARD : TS_MESSAGE
{
	TS_CS_PUTOFF_CARD()		_INIT(TM_CS_PUTOFF_CARD)

		unsigned char	position;
};

struct TS_SC_BELT_SLOT_INFO : TS_MESSAGE
{
	TS_SC_BELT_SLOT_INFO()	_INIT(TM_SC_BELT_SLOT_INFO)

		AR_HANDLE		handle[8];
};


struct TS_CS_PUTOFF_ITEM : TS_MESSAGE
{
	TS_CS_PUTOFF_ITEM()		_INIT(TM_CS_PUTOFF_ITEM)

		unsigned char	position;
	AR_HANDLE		target_handle;
};

struct TS_CS_DROP_ITEM : TS_MESSAGE
{
	TS_CS_DROP_ITEM()		_INIT(TM_CS_DROP_ITEM)

		AR_HANDLE		item_handle;
	int				count;
};

struct TS_CS_TAKE_ITEM : TS_MESSAGE
{
	TS_CS_TAKE_ITEM()		_INIT(TM_CS_TAKE_ITEM)

		AR_HANDLE		taker_handle;
	AR_HANDLE		item_handle;
};

struct TS_SC_TAKE_ITEM_RESULT : TS_MESSAGE
{
	TS_SC_TAKE_ITEM_RESULT()	_INIT(TM_SC_TAKE_ITEM_RESULT)

		AR_HANDLE		item_handle;
	AR_HANDLE		item_taker;
};


struct TS_SC_DROP_RESULT : TS_MESSAGE
{
	TS_SC_DROP_RESULT()		_INIT(TM_SC_DROP_RESULT)

		AR_HANDLE		item_handle;
	bool			isAccepted;
};

struct TS_CS_ERASE_ITEM : TS_MESSAGE
{
	struct EraseItemInfo
	{
		AR_HANDLE	item_handle;
		__int64		count;
		char		is_in_storage;
	};

	TS_CS_ERASE_ITEM()		_INIT(TM_CS_ERASE_ITEM)

		unsigned char	item_count;
};

struct TS_SC_ERASE_ITEM : TS_MESSAGE
{
	struct EraseItemInfo
	{
		AR_HANDLE	item_handle;
		__int64		count;
	};

	TS_SC_ERASE_ITEM()		_INIT(TM_SC_ERASE_ITEM)

		unsigned char	item_count;
};

struct TS_SC_INVENTORY : TS_MESSAGE
{
	TS_SC_INVENTORY()		_INIT(TM_SC_INVENTORY)

		unsigned short	count;

	// 이하 TS_ITEM_INFO 가 count 만큼 붙게됨
};

struct TS_SC_OPEN_STORAGE : TS_MESSAGE
{
	TS_SC_OPEN_STORAGE()		_INIT(TM_SC_OPEN_STORAGE)

		int maxStorageItemCount;
};

struct TS_SC_COMMERCIAL_STORAGE_INFO : TS_MESSAGE
{
	TS_SC_COMMERCIAL_STORAGE_INFO()		_INIT(TM_SC_COMMERCIAL_STORAGE_INFO)

		unsigned	short total_item_count;
	unsigned	short new_item_count;
};

struct TS_SC_COMMERCIAL_STORAGE_LIST : TS_MESSAGE
{
	TS_SC_COMMERCIAL_STORAGE_LIST()			_INIT(TM_SC_COMMERCIAL_STORAGE_LIST)

		struct CommercialItemInfo
	{
		unsigned int			commercial_item_uid;	// cuid (캐쉬템 고유번호)
		ItemBase::ItemCode		code;					// 아이템 코드
		unsigned short			count;					// 갯수
	};

	unsigned short count;

	// 이하 CommercialItemInfo 가 count 만큼 붙음
};

struct TS_CS_TAKEOUT_COMMERCIAL_ITEM : TS_MESSAGE
{
	TS_CS_TAKEOUT_COMMERCIAL_ITEM()			_INIT(TM_CS_TAKEOUT_COMMERCIAL_ITEM)

		unsigned int	commercial_item_uid;		// 꺼낼 아이템 cuid
	unsigned short	count;						// 꺼낼 갯수
};

struct TS_CS_GET_REGION_INFO : TS_MESSAGE
{
	TS_CS_GET_REGION_INFO()			_INIT(TM_CS_GET_REGION_INFO)

		AR_UNIT x;
	AR_UNIT y;
};

struct TS_SC_SHOW_CREATE_GUILD : TS_MESSAGE
{
	TS_SC_SHOW_CREATE_GUILD()		_INIT(TM_SC_SHOW_CREATE_GUILD)
};

struct TS_SC_SHOW_CREATE_ALLIANCE : TS_MESSAGE
{
	TS_SC_SHOW_CREATE_ALLIANCE()		_INIT(TM_SC_SHOW_CREATE_ALLIANCE)
};

struct TS_SC_OPEN_GUILD_WINDOW : TS_MESSAGE
{
	TS_SC_OPEN_GUILD_WINDOW()			_INIT(TM_SC_OPEN_GUILD_WINDOW)

		int			client_id;
	int			account_id;
	int			one_time_password;
	char		raw_server_name[32];
};

struct TS_SC_UPDATE_GUILD_ICON : TS_MESSAGE
{
	TS_SC_UPDATE_GUILD_ICON()			_INIT(TM_SC_UPDATE_GUILD_ICON)

		int			client_id;
	int			account_id;
	int			one_time_password;
	char		raw_server_name[32];
};

struct TS_SC_UPDATE_GUILD_BANNER : TS_MESSAGE
{
	TS_SC_UPDATE_GUILD_BANNER()			_INIT(TM_SC_UPDATE_GUILD_BANNER)

		int			client_id;
	int			account_id;
	int			one_time_password;
	char		raw_server_name[32];
};

struct TS_CS_CHANGE_ITEM_POSITION : TS_MESSAGE
{
	TS_CS_CHANGE_ITEM_POSITION()	_INIT(TM_CS_CHANGE_ITEM_POSITION)

		bool		is_storage;

	AR_HANDLE	item_handle_1;
	AR_HANDLE	item_handle_2;
};

struct TS_CS_STORAGE : TS_MESSAGE
{
	TS_CS_STORAGE()				_INIT(TM_CS_STORAGE)

		enum
	{
		ITEM_INVENTORY_TO_STORAGE = 0,
		ITEM_STORAGE_TO_INVENTORY = 1,
		GOLD_INVENTORY_TO_STORAGE = 2,
		GOLD_STORAGE_TO_INVENTORY = 3,
		CLOSE = 4,
	};

	AR_HANDLE				item_handle;
	unsigned char			mode;
	__int64					count;
};

struct TS_SC_ITEM_COOL_TIME : TS_MESSAGE
{
	TS_SC_ITEM_COOL_TIME()		_INIT(TM_SC_ITEM_COOL_TIME)

		enum
	{
		MAX_ITEM_COOLTIME_GROUP = 40,
	};

	AR_TIME	cool_time[MAX_ITEM_COOLTIME_GROUP];
};

struct TS_SC_GET_CHAOS : TS_MESSAGE
{
	TS_SC_GET_CHAOS()			_INIT(TM_SC_GET_CHAOS);

	AR_HANDLE				hPlayer;
	AR_HANDLE				hCorpse;
	int						nChaos;

	enum CHAOS_BONUS_TYPE
	{
		CHAOS_BONUS_NONE = 0,
		CHAOS_BONUS_PCBANG = 1,
		CHAOS_BONUS_PREMIUM_PCBANG = 2,
	};

	char				nBonusType;
	unsigned char		nBonusPercent;
	int					nBonus;
};

struct TS_CS_ARRANGE_ITEM : TS_MESSAGE
{
	TS_CS_ARRANGE_ITEM()		_INIT(TM_CS_ARRANGE_ITEM);

	bool			bIsStorage;
};

struct TS_SC_HAIR_INFO : TS_MESSAGE
{
	TS_SC_HAIR_INFO()		_INIT(TM_SC_HAIR_INFO)

		AR_HANDLE		hPlayer;
	int				nHairID;
	int				nHairColorIndex;
	unsigned int	nHairColorRGB;
};

struct TS_SC_SKIN_INFO : TS_MESSAGE
{
	TS_SC_SKIN_INFO()		_INIT(TM_SC_SKIN_INFO)

		AR_HANDLE		hPlayer;
	int				nSkinColor;
};

struct TS_CS_HIDE_EQUIP_INFO : TS_MESSAGE
{
	TS_CS_HIDE_EQUIP_INFO()		_INIT(TM_CS_HIDE_EQUIP_INFO)

		unsigned int	nHideEquipFlag;
};

struct TS_SC_HIDE_EQUIP_INFO : TS_MESSAGE
{
	TS_SC_HIDE_EQUIP_INFO()		_INIT(TM_SC_HIDE_EQUIP_INFO)

		AR_HANDLE		hPlayer;
	unsigned int	nHideEquipFlag;
};

struct TS_CS_SWAP_EQUIP : TS_MESSAGE
{
	TS_CS_SWAP_EQUIP()			_INIT(TM_CS_SWAP_EQUIP)
};

struct TS_CS_MIX : TS_MESSAGE
{
	TS_CS_MIX()				_INIT(TM_CS_MIX)

		struct MIX_INFO
	{
		AR_HANDLE		handle;
		unsigned short	count;
	};

	MIX_INFO main_item;

	unsigned short count;

	// 이하 MIX_INFO 타입이 count 만큼 붙게 됨.
};

struct TS_SC_MIX_RESULT : TS_MESSAGE
{
	TS_SC_MIX_RESULT()		_INIT(TM_SC_MIX_RESULT)

		enum MIX_TYPE
	{
		MIX_TYPE_NONE,
		MIX_TYPE_AWAKEN,		// 각성
		MIX_TYPE_RANDOM_OPTION,	// 랜덤옵션
	};

	unsigned		count;		// 이하 AR_HANDLE 이 count 만큼 붙게 됨. count 가 0 이면 조합 실패.
	unsigned char	type;		// 각성 Type
};

struct TS_CS_DECOMPOSE : TS_MESSAGE
{
	TS_CS_DECOMPOSE()		_INIT(TM_CS_DECOMPOSE)

		struct DECOMPOSE_INFO
	{
		DECOMPOSE_INFO(AR_HANDLE _handle = 0, unsigned int _count = 0) : handle(_handle), count(_count)
		{}

		AR_HANDLE			handle;
		unsigned int 		count;
	};

	unsigned int		count;		// 이하 DECOMPOSE_INFO 가 count 만큼 붙게 됨.
};

struct TS_SC_DECOMPOSE_RESULT : TS_MESSAGE
{
	TS_SC_DECOMPOSE_RESULT()	_INIT(TM_SC_DECOMPOSE_RESULT)

		struct DECOMPOSE_INFO
	{
		DECOMPOSE_INFO(AR_HANDLE _handle = 0, unsigned int _count = 0) : handle(_handle), count(_count)
		{}

		AR_HANDLE			handle;
		unsigned int 		count;
	};

	unsigned int		total_count;		// 이하 DECOMPOSE_INFO 가 total_count 만큼 붙게 됨. total_count는 항상 1 이상 (분해실패하면 TS_SC_RESULT 패킷 보냄)
};

struct TS_SC_CHAT_LOCAL : TS_MESSAGE
{
	TS_SC_CHAT_LOCAL()			_INIT(TM_SC_CHAT_LOCAL)

		AR_HANDLE		handle;

	unsigned char	len;
	unsigned char	type;
};

struct TS_SC_CHAT_RESULT : TS_MESSAGE
{
	TS_SC_CHAT_RESULT()			_INIT(TM_SC_CHAT_RESULT)

		enum CHAT_RESULT
	{
		RESULT_DECREASE_HPMP = 0,
		RESULT_COOLTIME = 1,
		RESULT_NEED_MORE_HPMP = 2,
		RESULT_LEVEL_RESTRICT = 3,
	};

	unsigned char	type;
	unsigned char	percentage;
	int				result;
	int				reserved;
};


enum CHAT_TYPE
{
	CHAT_NORMAL = 0,	// 일반채팅
	CHAT_YELL = 1,	// 외치기
	CHAT_ADV = 2,	// 광고채팅
	CHAT_WHISPER = 3,	// 귓속말
	CHAT_GLOBAL = 4,	// 전체메시지
	CHAT_EMOTION = 5,	// 이모션
	CHAT_GM = 6,	// GM 메시지(색상 분리를 위해)
	CHAT_GM_WHISPER = 7,	// GM 귓말 메시지(색상 분리를 위해)
	CHAT_PARTY = 10,	// 파티
	CHAT_GUILD = 11,	// 길드
	CHAT_ATTACKTEAM = 12,	// 동맹(시즈 공대)
	CHAT_FRIEND = 13,	// 친구
	CHAT_NOTICE = 20,	// 공지
	CHAT_ANNOUNCE = 21,	// 알림
	CHAT_CENTER_NOTICE = 22,	// 중앙 공지(UI 팀에서 추가한 화면 중앙 상단에 위치한 별도의 공지 공간에 출력됨)
	CHAT_EXP = 30,	// 경험치 관련
	CHAT_DAMAGE = 31,	// 데미지	(공격&마법데미지양)
	CHAT_ITEM = 32,	// 아이템 관련
	CHAT_BATTLE = 33,	// 전투관련
	CHAT_SUMMON = 34,	// 소환수 관련
	CHAT_ETC = 35,	// 기타
	CHAT_TITLE = 36,	// 호칭
	CHAT_NPC = 40,	// NPC 메시지
	CHAT_DEBUG = 50,	// 디버그 메시지 (개발용. 콘솔에 출력)

	CHAT_PARTY_SYSTEM = 100,	// 파티 시스템 메시지
	CHAT_GUILD_SYSTEM = 110,	// 길드 시스템 메시지
	CHAT_QUEST_SYSTEM = 120,	// 퀘스트 메시지
	CHAT_RAID_SYSTEM = 130,	// 레이드 시스템 메시지
	CHAT_FRIEND_SYSTEM = 140,	// 친구 시스템 메시지
	CHAT_ALLIANCE_SYSTEM = 150,	// 길드 연합 시스템 메시지
	CHAT_HUNTAHOLIC_SYSTEM = 160,	// 헌터홀릭 시스템 메시지
	CHAT_DUNGEON_SYSTEM = 170,	// 인스턴스 던전 시스템 메시지
};

struct TS_SC_CHAT : TS_MESSAGE
{
	TS_SC_CHAT()			_INIT(TM_SC_CHAT)

		char			szSender[21];

	unsigned short	len;
	unsigned char	type;
};

struct TS_CS_CHAT_REQUEST : TS_MESSAGE
{
	TS_CS_CHAT_REQUEST()			_INIT(TM_CS_CHAT_REQUEST)

		char			szTarget[21];

	unsigned char	request_id;
	unsigned char	len;
	unsigned char	type;
};

struct TS_SC_STAT_INFO : TS_MESSAGE
{
	TS_SC_STAT_INFO()				_INIT(TM_SC_STAT_INFO)

		AR_HANDLE				handle;

	CreatureStat			stat;
	CreatureAttribute		attribute;

	enum
	{
		TOTAL = 0,
		BY_ITEM = 1,
	};

	unsigned char	type;
};

struct TS_SC_REGEN_INFO : TS_MESSAGE
{
	TS_SC_REGEN_INFO()				_INIT(TM_SC_REGEN_INFO)

		AR_HANDLE		handle;
	short			hp_regen_percentage;
	int				hp_regen_point;
	short			mp_regen_percentage;
	int				mp_regen_point;
};

struct TS_CS_CANCEL_ACTION : TS_MESSAGE
{
	TS_CS_CANCEL_ACTION()		_INIT(TM_CS_CANCEL_ACTION)

		AR_HANDLE		handle;
};

struct TS_CS_SKILL : TS_MESSAGE
{
	TS_CS_SKILL()				_INIT(TM_CS_SKILL)

		unsigned short			skill_id;

	AR_HANDLE				caster;
	AR_HANDLE				target;

	AR_UNIT					x, y, z;
	unsigned char			layer;
	/*
		enum
		{
			CASTING = 1,
			CANCEL	= 2,
		};
		char					type;
	*/
	char					skill_level;
};
/*
struct TS_SC_SKILL_CASTING_TIME	: TS_MESSAGE
{
	TS_SC_SKILL_CASTING_TIME()				_INIT( TM_SC_SKILL_CASTING_TIME )

	AR_HANDLE				caster;
	AR_TIME					tm;
};
*/


struct TS_SC_SKILL : TS_MESSAGE
{
	TS_SC_SKILL()				_INIT(TM_SC_SKILL)

		unsigned short			skill_id;

	char					skill_level;

	AR_HANDLE				caster;
	AR_HANDLE				target;

	AR_UNIT					x, y, z;
	unsigned char			layer;

	char					type;

	int						hp_cost;
	int						mp_cost;

	int						caster_hp;
	int						caster_mp;

	enum
	{
		FIRE = 0,
		CASTING = 1,
		CASTING_UPDATE = 2,
		CANCEL = 3,
		REGION_FIRE = 4,	// 지역 스킬의 fire
		COMPLETE = 5,
	};

	struct CancelType
	{
		// 내용 없음
	};

	struct CompleteType
	{
		// 내용 없음
	};

	struct CastType				// Casting 이거나 Casting_update일 경우 날아감.
	{
		AR_TIME					tm;			// 캐스팅 타임
		unsigned short			nErrorCode;	// 성공 여부 ( 0이면 성공 )
	};

	struct FireType				// Region Fire일 경우에도 얘가 날아감.
	{
		bool					bMultiple;
		AR_UNIT					range;			// 스킬 효과 범위

		unsigned char			target_count;
		unsigned char			fire_count;
		unsigned short			count;
	};

	union
	{
		CancelType				cancel;
		CompleteType			complete;
		CastType				cast;
		FireType				fire;
	};
	// Fire나 Region Fire일 경우 추가로 뒤에 SkillResult 가 count 만큼 따라간다
};

struct TS_SC_AURA : TS_MESSAGE
{
	TS_SC_AURA()						_INIT(TM_SC_AURA)

		AR_HANDLE			caster;

	unsigned short		skill_id;
	bool				status;
};

struct TS_CS_REQUEST_REMOVE_STATE : TS_MESSAGE
{
	TS_CS_REQUEST_REMOVE_STATE()		_INIT(TM_CS_REQUEST_REMOVE_STATE)

		AR_HANDLE			target;

	int					state_code;
};

struct TS_CS_SUMMON_CARD_SKILL_LIST : TS_MESSAGE
{
	TS_CS_SUMMON_CARD_SKILL_LIST()			_INIT(TM_CS_SUMMON_CARD_SKILL_LIST)

		AR_HANDLE			item_handle;
};

struct TS_SC_SKILL_LEVEL_LIST : TS_MESSAGE
{
	TS_SC_SKILL_LEVEL_LIST()		_INIT(TM_SC_SKILL_LEVEL_LIST)

		struct SkillLevelInfo
	{
		int				skill_id;
		unsigned char	skill_level;
	};

	unsigned short		count;
};

struct TS_SC_SKILL_EFFECT : TS_MESSAGE
{
	TS_SC_SKILL_EFFECT()				_INIT(TM_SC_SKILL_EFFECT)

		AR_HANDLE			caster;
	AR_HANDLE			target;

	unsigned short skill_id;
	unsigned char skill_level;

	int var1;				// Damage 양
	unsigned char var2;		// HP Percentage
	char var3;				// Flag
};

// 라크 갱신도 같이 처리
struct TS_SC_GOLD_UPDATE : TS_MESSAGE
{
	TS_SC_GOLD_UPDATE()				_INIT(TM_SC_GOLD_UPDATE)

		__int64					gold;
	int						chaos;
};

struct TS_SC_EXP_UPDATE : TS_MESSAGE
{
	TS_SC_EXP_UPDATE()				_INIT(TM_SC_EXP_UPDATE)

		AR_HANDLE				handle;

	__int64					exp;
	__int64					jp;
};

struct TS_SC_BONUS_EXP_JP : TS_MESSAGE
{
	TS_SC_BONUS_EXP_JP()				_INIT(TM_SC_BONUS_EXP_JP)

		AR_HANDLE				handle;

	unsigned short			count;

	struct BONUS_INFO
	{
		int				type;	// 보너스 종류
		int				rate;	// 보너스 비율(메시지 출력용)
		__int64			exp;	// 보너스 경험치
		int				jp;		// 보너스 JP
	};

	// 이 뒤로 count개 만큼의 BONUS_INFO가 붙어서 감
};

struct TS_SC_DETECT_RANGE_UPDATE : TS_MESSAGE
{
	TS_SC_DETECT_RANGE_UPDATE()		_INIT(TM_SC_DETECT_RANGE_UPDATE)

		AR_HANDLE				handle;

	AR_UNIT					detect_range;
};

struct TS_SC_LEVEL_UPDATE : TS_MESSAGE
{
	TS_SC_LEVEL_UPDATE()				_INIT(TM_SC_LEVEL_UPDATE)

		AR_HANDLE				handle;

	int						level;
	int						job_level;
};

/////////////////////////////////////////////////////////////////////////////
//  로비에서 쓰기 위한것들

struct MODEL_INFO
{
	int				sex;
	int				race;
	int				model_id[5];
	int				hair_color_index;
	unsigned int	hair_color_rgb;
	unsigned int	hide_equip_flag;
	int				texture_id;
	int				wear_info[ItemBase::MAX_ITEM_WEAR_LOGIN];
};

#ifdef USE_CLIENT97
struct LOBBY_CHARACTER_INFO : MODEL_INFO
{
	int					level;
	int					job;
	int					job_level;
	int					exp_percentage;	// ( 0 ~ 100 )
	int					hp;
	int					mp;
	int					permission;
	bool				is_banned;
	char				name[20];
	unsigned long		skin_color;
	char				szCreateTime[36];
	char				szDeleteTime[36];
	//	int					wear_item_enhance_info[ItemBase::MAX_ITEM_WEAR];
	//	int					wear_item_level_info[ItemBase::MAX_ITEM_WEAR];
	//	char				wear_item_elemental_type[ItemBase::MAX_ITEM_WEAR];
	ItemBase::ItemCode	wear_appearance_code[ItemBase::MAX_ITEM_WEAR_LOGIN];
};


#else

struct LOBBY_CHARACTER_INFO : MODEL_INFO
{
	int					level;
	int					job;
	int					job_level;
	int					exp_percentage;	// ( 0 ~ 100 )
	int					hp;
	int					mp;
	int					permission;
	bool				is_banned;
#ifdef USE_CLIENT96
	char				name[20];
#else
	char				name[19];
#endif
	unsigned long		skin_color;
	char				szCreateTime[30];
	char				szDeleteTime[30];
	int					wear_item_enhance_info[ItemBase::MAX_ITEM_WEAR_LOGIN];
	int					wear_item_level_info[ItemBase::MAX_ITEM_WEAR_LOGIN];
	char				wear_item_elemental_type[ItemBase::MAX_ITEM_WEAR_LOGIN];
	ItemBase::ItemCode	wear_appearance_code[ItemBase::MAX_ITEM_WEAR_LOGIN];
};

#endif

//struct TS_CS_ACCOUNT : TS_MESSAGE
//{
//	TS_CS_ACCOUNT()			_INIT( TM_CS_ACCOUNT )
//
//	char		account[19];
//	char		password[32];
//};

struct TS_CS_CHARACTER_LIST : TS_MESSAGE
{
	TS_CS_CHARACTER_LIST()			_INIT(TM_CS_CHARACTER_LIST)

		char			account[GameRule::MAX_ACCOUNT_LEN + 1];
};

struct TS_CS_CHECK_CHARACTER_NAME : TS_MESSAGE
{
	TS_CS_CHECK_CHARACTER_NAME()	_INIT(TM_CS_CHECK_CHARACTER_NAME)

		char			name[19];
};

struct TS_CS_CHANGE_ALIAS : TS_MESSAGE
{
	TS_CS_CHANGE_ALIAS()			_INIT(TM_CS_CHANGE_ALIAS)

		char			alias[19];
};

struct TS_CS_CREATE_CHARACTER : TS_MESSAGE
{
	TS_CS_CREATE_CHARACTER()		_INIT(TM_CS_CREATE_CHARACTER)

		LOBBY_CHARACTER_INFO		info;
};

struct TS_CS_DELETE_CHARACTER : TS_MESSAGE
{
	TS_CS_DELETE_CHARACTER()		_INIT(TM_CS_DELETE_CHARACTER)

		char		name[19];
};

struct TS_SC_CHARACTER_LIST : TS_MESSAGE
{
	TS_SC_CHARACTER_LIST()			_INIT(TM_SC_CHARACTER_LIST)

		unsigned int		current_server_time;	// 1970년 1월 1일 0시 0분 0초로부터 몇초가 경과되었는지를 알려줌.
													// ( CRT 의 time() 리턴값임)
	unsigned short		last_login_index;

	unsigned short		count;

	// 이하 LOBBY_CHARACTER_INFO 가 count 만큼 붙게됨
};

struct TS_CS_ACCOUNT_WITH_AUTH : TS_MESSAGE
{
	TS_CS_ACCOUNT_WITH_AUTH()			_INIT(TM_CS_ACCOUNT_WITH_AUTH)

		char		account[GameRule::MAX_ACCOUNT_LEN + 1];
	__int64		one_time_key;
};

// NPC 다이얼로그
struct TS_SC_DIALOG : TS_MESSAGE
{
	TS_SC_DIALOG()				_INIT(TM_SC_DIALOG)

		enum DIALOG_TYPE
	{
		TYPE_NPC = 0,
		TYPE_DUNGEON_STONE = 1,
		TYPE_DONATION_PROP = 2,
		TYPE_QUEST_INFO_AND_START = 3,
		TYPE_AUCTION_WINDOW = 4,
		TYPE_HUNTAHOLIC_LOBBY = 5,
		TYPE_DONATION_REWARD_NOTIFY = 6,
		TYPE_QUEST_INFO_IN_PROGRESS = 7,
		TYPE_QUEST_INFO_AND_END = 8,
		TYPE_CREATURE_FARM_WINDOW = 9,
		TYPE_OTHER_INSTANCE_DUNGEON_CONFIRM_WINDOW = 10,
	};

	int							type;

	AR_HANDLE					npc_handle;	// NULL 일수도 있음.

	// 만약 length가 모두 0일경우는 열려 있던 다이얼로그를 닫을것.
	unsigned short				title_length;
	unsigned short				text_length;
#ifdef USE_CLIENT97
	unsigned short				unk_length;
#endif
	unsigned short				menu_length;

	// 이하 다이얼로그 정보가 text string 으로 붙게됨. 메뉴는   |문자열|트리거|  의 형식으로 붙게 된다.
	/*

	 +-----------------------+   title_length : 6
	 |        경비병         |   text_length  : 20
	 +-----------------------+   menu_length  : 41
	 |                       |
	 | 위험하니 돌아가세요.  |   data : 경비병위험하니 돌아가세요.|예|on_click_yes()||아니오|on_click_no()|
	 |                       |
	 |                       |
	 |         [예] [아니오] |
	 +-----------------------+
	*/

};

// 다이얼로그 메뉴 클릭 결과를 전송한다. 이 헤더 이후에 스트링이 따라 붙어 오게 된다.
struct TS_CS_DIALOG : TS_MESSAGE
{
	TS_CS_DIALOG()					_INIT(TM_CS_DIALOG)

		unsigned short				trigger_length;
};

// 프랍이나 NPC 를 클릭했을때 서버로 보낸다. 결과로는 TS_SC_DIALOG 나 기타등등의 메시지가 갈 수 있다.
struct TS_CS_CONTACT : TS_MESSAGE
{
	TS_CS_CONTACT()					_INIT(TM_CS_CONTACT)

		AR_HANDLE					handle;
};

// 특수 다이얼로그 처리
struct TS_SC_SHOW_WINDOW : TS_MESSAGE
{
	TS_SC_SHOW_WINDOW()			_INIT(TM_SC_SHOW_WINDOW)

		unsigned short				window_length;
	unsigned short				argument_length;
	unsigned short				trigger_length;
};

// 메시지 박스 처리
struct TS_SC_GENERAL_MESSAGE_BOX : TS_MESSAGE
{
	TS_SC_GENERAL_MESSAGE_BOX()	_INIT(TM_SC_GENERAL_MESSAGE_BOX)

		unsigned short				text_length;
};

struct TS_SC_NPC_TRADE_INFO : TS_MESSAGE
{
	TS_SC_NPC_TRADE_INFO()		_INIT(TM_SC_NPC_TRADE_INFO)

		bool				is_sell;
	ItemBase::ItemCode	code;
	__int64				count;
	__int64				price;
	int					huntaholic_point;
	int					arena_point;
	AR_HANDLE			target;
};

struct TS_SC_MARKET : TS_MESSAGE
{
	struct ItemInfo
	{
		ItemBase::ItemCode	code;				// 상점 물품 아이템 코드
		__int64				price;				// 상점 판매가 - 루피(플레이어 입장 구매가)
		int					huntaholic_point;	// 상점 판매가 - 헌터홀릭 포인트(플레이어 입장 구매가)
		int					arena_point;		// 상점 판매가 - 아레나 포인트(플레이어 입장 구매가)
	};

	TS_SC_MARKET()				_INIT(TM_SC_MARKET)

		AR_HANDLE					npc_handle;

	unsigned short				item_count;
};

struct TS_CS_BUY_ITEM : TS_MESSAGE
{
	TS_CS_BUY_ITEM()			_INIT(TM_CS_BUY_ITEM)

		ItemBase::ItemCode		item_code;
	unsigned short			buy_count;
};

struct TS_CS_SELL_ITEM : TS_MESSAGE
{
	TS_CS_SELL_ITEM()			_INIT(TM_CS_SELL_ITEM)

		AR_HANDLE	handle;
	__int64		sell_count;
};

struct TS_CS_DONATE_ITEM : TS_MESSAGE
{
	struct DonateItemInfo
	{
		AR_HANDLE		handle;
		__int64			count;
	};

	TS_CS_DONATE_ITEM()			_INIT(TM_CS_DONATE_ITEM)

		__int64					gold;
	int						jp;
	unsigned char			item_count;

	// 이 뒤로 DonateItemInfo가 item_count 개수만큼 붙음
};

struct TS_CS_DONATE_REWARD : TS_MESSAGE
{
	TS_CS_DONATE_REWARD()		_INIT(TM_CS_DONATE_REWARD)

		struct RewardInfo
	{
		unsigned char			reward_type;
		unsigned short			count;
	};

	unsigned char		reward_count;
	// 이 뒤로 reward_count개 만큼의 RewardInfo가 붙어서 옴
};

struct TS_SC_SHOW_SOULSTONE_CRAFT_WINDOW : TS_MESSAGE
{
	TS_SC_SHOW_SOULSTONE_CRAFT_WINDOW()	_INIT(TM_SC_SHOW_SOULSTONE_CRAFT_WINDOW);
};

struct TS_CS_SOULSTONE_CRAFT : TS_MESSAGE
{
	TS_CS_SOULSTONE_CRAFT()		_INIT(TM_CS_SOULSTONE_CRAFT);

	AR_HANDLE				craft_item_handle;

	enum
	{
		MAX_SOULSTONE_NUM = 4,
	};

	AR_HANDLE				soulstone_handle[MAX_SOULSTONE_NUM];
};

struct TS_SC_SHOW_SOULSTONE_REPAIR_WINDOW : TS_MESSAGE
{
	TS_SC_SHOW_SOULSTONE_REPAIR_WINDOW()	_INIT(TM_SC_SHOW_SOULSTONE_REPAIR_WINDOW);
};

struct TS_CS_REPAIR_SOULSTONE : TS_MESSAGE
{
	TS_CS_REPAIR_SOULSTONE()				_INIT(TM_CS_REPAIR_SOULSTONE);

	enum
	{
		MAX_REPAIR_COUNT = 6,
	};

	AR_HANDLE				item_handle[MAX_REPAIR_COUNT];
};

struct TS_CS_TRANSMIT_ETHEREAL_DURABILITY : TS_MESSAGE
{
	TS_CS_TRANSMIT_ETHEREAL_DURABILITY()	_INIT(TM_CS_TRANSMIT_ETHEREAL_DURABILITY);

	AR_HANDLE	handle;
};

struct TS_CS_TRANSMIT_ETHEREAL_DURABILITY_TO_EQUIPMENT : TS_MESSAGE
{
	TS_CS_TRANSMIT_ETHEREAL_DURABILITY_TO_EQUIPMENT()	_INIT(TM_CS_TRANSMIT_ETHEREAL_DURABILITY_TO_EQUIPMENT);

	float	rate;
	char	target;
};

struct TS_CS_USE_ITEM : TS_MESSAGE
{
	TS_CS_USE_ITEM()			_INIT(TM_CS_USE_ITEM)

		AR_HANDLE				item_handle;
	AR_HANDLE				target_handle;	// 혼자 쓰는거라면 0
	char					szParameter[32];
};

struct TS_SC_USE_ITEM_RESULT : TS_MESSAGE
{
	TS_SC_USE_ITEM_RESULT()			_INIT(TM_SC_USE_ITEM_RESULT)

		AR_HANDLE				item_handle;
	AR_HANDLE				target_handle;
};


struct TS_SC_DESTROY_ITEM : TS_MESSAGE
{
	TS_SC_DESTROY_ITEM()			_INIT(TM_SC_DESTROY_ITEM)

		AR_HANDLE				item_handle;
};

struct TS_SC_UPDATE_ITEM_COUNT : TS_MESSAGE
{
	TS_SC_UPDATE_ITEM_COUNT()			_INIT(TM_SC_UPDATE_ITEM_COUNT)

		AR_HANDLE				item_handle;
	__int64					count;
};


struct TS_TRADE : TS_MESSAGE
{
	TS_TRADE()					_INIT(TM_TRADE)

		enum {
		REQUEST_TRADE = 0,			// 트레이드 요청을 시작한다
		ACCEPT_TRADE = 1,			// 트레이드 요청을 수락한다
		BEGIN_TRADE = 2,			// 트레이드 시작
		CANCEL_TRADE = 3,			// 트레이드 취소
		REJECT_TRADE = 4,			// 트레이드 거부
		ADD_ITEM = 5,			// 아이템을 올린다.
		ADD_GOLD = 6,			// 돈을 올린다. 돈을 올릴때는 item_info 를 모두 0 으로 채우고 count 만 액수로 설정한다.
		FREEZE_TRADE = 7,			// 확인버튼을 누른다.
		CONFIRM_TRADE = 8,			// 최종결정 버튼을 누른다.
		PROCESS_TRADE = 9,			// 거래 완료
		REMOVE_ITEM = 10,			// 아이템을 내린다(확인 버튼 누르기 이전에 아이템 올릴 수 있을 때만 가능)
		MODIFY_COUNT = 11,			// 아이템 수량을 바꾼다
	};

	/* 트레이드 진행 순서의 예

	   . 트레이드 요청하는이를 A, 대상을 B 라고 한다.

	 1. A 가 B 의 핸들을 적어 서버에게 REQUEST_TRADE 를 보낸다.
	 2. B 에게는 REQUEST_TRADE 메시지가 도착한다.
	 3. 만약 B 가 거부하려면 서버에게 REJECT_TRADE 메시지를 보낸다. 그러면 A 에게 REJECT_TRADE 메시지가 전달됨.
	 4. 만약 B 가 승락하려면 서버에게 ACCEPT_TRADE 메시지를 보낸다. 그러면 A 와 B 에게 BEGIN_TRADE 메시지가 전달됨.
	 5. 이후 아이템이나 돈을 올려 놓으려면 서버에게 ADD_ITEM 이나 ADD_GOLD 메시지를 보낸다. 그러면 A 와 B 에게 해당 메시지가 전달됨.
	 6. 만약 중도에 트레이드를 취소하려면 서버에 CANCEL_TRADE 버튼을 누른다. 그러면 A 와 B 에게 CANCEL_TRADE 메시지가 전달됨.
	 7. 확인 버튼을 누르려면 서버에게 FREEZE_TRADE 메시지를 보낸다. 그러면 A 와 B 에게 FREEZE_TRADE 메시지가 전달됨.
	 8. 둘다 확인버튼을 누른 상태에서 또 둘다 CONFIRM_TRADE 메시지를 보내면 트레이드가 성사된다.
		A 와 B 에게 PROCESS_TRADE 가 전송되며 바뀐 인벤 데이터와 금액 데이터는 다른 메시지로 전송된다.

	*/


	AR_HANDLE			target_player;
	char				mode;
	TS_ITEM_INFO		item_info;
};

struct TS_SC_SKILL_LIST : TS_MESSAGE
{
	TS_SC_SKILL_LIST()		_INIT(TM_SC_SKILL_LIST);

	struct SkillInfo
	{
		int				skill_id;
		unsigned char	base_skill_level;
		unsigned char	current_skill_level;
		AR_TIME			total_cool_time;
		AR_TIME			remain_cool_time;
	};

	AR_HANDLE			target;	// 플레이어 혹은 크리쳐
	unsigned short		count;
	unsigned char		modification_type; // enum ModificationType

	// 이후 따라오는 스킬 리스트가 어떤 의미를 가지는가를 의미하는 열거형
	enum ModificationType
	{
		UPDATE = 0,	// 스킬이 업데이트됨
		REFRESH = 1,	// 스킬 리스트를 갱신
	};

	// 이후 count만큼의 SkillInfo가 따라옴
};

// 스킬 레벨의 변동이 발생
struct TS_SC_ADDED_SKILL_LIST : TS_MESSAGE
{
	TS_SC_ADDED_SKILL_LIST()	_INIT(TM_SC_ADDED_SKILL_LIST);

	struct AddedSkillInfo
	{

		int skill_id;	// 스킬 ID
		bool restricted_to_type;
		char added_skill_level;
	};

	AR_HANDLE			target;
	unsigned short		count;
};

// 스킬을 배우거나, 스킬레벨을 올린다.
struct TS_CS_LEARN_SKILL : TS_MESSAGE
{
	TS_CS_LEARN_SKILL()		_INIT(TM_CS_LEARN_SKILL)

		AR_HANDLE			target;				// 자기 자신 혹은 자신의 크리쳐
	int					skill_id;			// 배우려는 목표 스킬 ID
	int					original_skill_id;	// 랜덤 스킬로 찍혔을 때 원본 랜덤 스킬 ID(껍데기스킬), 아니면 0
	short				skill_level;		// 배울 스킬 레벨
};

// 잡레벨을 올린다 (1씩)
struct TS_CS_JOB_LEVEL_UP : TS_MESSAGE
{
	TS_CS_JOB_LEVEL_UP()		_INIT(TM_CS_JOB_LEVEL_UP)

		AR_HANDLE			target;	// 자기 자신 혹은 자신의 크리쳐
};



//////////////////////////////////////////////////////////////////////////////
// 이하 소환수

struct TS_SC_SUMMON_EVOLUTION : TS_MESSAGE
{
	TS_SC_SUMMON_EVOLUTION()		_INIT(TM_SC_SUMMON_EVOLUTION)

		AR_HANDLE				card_handle;
	AR_HANDLE				summon_handle;

	char					name[19];
	int						code;
};

struct TS_SC_ADD_SUMMON_INFO : TS_MESSAGE
{
	TS_SC_ADD_SUMMON_INFO()			_INIT(TM_SC_ADD_SUMMON_INFO)

		AR_HANDLE				card_handle;
	AR_HANDLE				summon_handle;

	char					name[19];
	int						code;

	// 이하 정보 필요
	int						level;
	int						sp;
};

struct TS_SC_REMOVE_SUMMON_INFO : TS_MESSAGE
{
	TS_SC_REMOVE_SUMMON_INFO()		_INIT(TM_SC_REMOVE_SUMMON_INFO)

		AR_HANDLE				card_handle;
};

struct TS_EQUIP_SUMMON : TS_MESSAGE
{
	TS_EQUIP_SUMMON()			_INIT(TM_EQUIP_SUMMON)

		bool				open_dialog;
	AR_HANDLE			card_handle[6];
};

struct TS_CS_SUMMON : TS_MESSAGE
{
	TS_CS_SUMMON()			_INIT(TM_CS_SUMMON)

		unsigned char		is_summon;	// 1이면 소환, 0이면 역소환
	AR_HANDLE			card_handle;
};

struct TS_SC_UNSUMMON_NOTICE : TS_MESSAGE
{
	TS_SC_UNSUMMON_NOTICE()			_INIT(TM_SC_UNSUMMON_NOTICE)

		AR_HANDLE			summon_handle;
	AR_TIME				unsummon_duration;
};

struct TS_SC_UNSUMMON : TS_MESSAGE
{
	TS_SC_UNSUMMON()			_INIT(TM_SC_UNSUMMON)

		AR_HANDLE			summon_handle;
};

struct TS_SC_ITEM_DROP_INFO : TS_MESSAGE // Broadcast only
{
	TS_SC_ITEM_DROP_INFO()			_INIT(TM_SC_ITEM_DROP_INFO)

		AR_HANDLE			monster_handle;
	AR_HANDLE			item_handle;
};

struct TS_SC_QUEST_LIST : TS_MESSAGE
{
	enum
	{
		MAX_VALUE = 6,
	};

	struct TS_QUEST_INFO
	{
		QuestBase::QuestCode	code;
		int						nStartID;
		int						nValue[MAX_VALUE];
		int						nStatus[QuestInstance::MAX_STATUS];
		unsigned char			nProgress;
		AR_TIME					nTimeLimit;
	};

	struct TS_PENDING_QUEST_INFO
	{
		QuestBase::QuestCode	code;
		int						nStartID;
	};

	TS_SC_QUEST_LIST()				_INIT(TM_SC_QUEST_LIST)

		unsigned short	count_active;
	unsigned short	count_pending;

	// 이하 TS_QUEST_INFO가 count_active만큼 붙고 그 뒤로 TS_PENDING_QUEST_INFO가 count_pending만큼 붙게 됨.
};

struct TS_SC_QUEST_STATUS : TS_MESSAGE
{
	TS_SC_QUEST_STATUS()	_INIT(TM_SC_QUEST_STATUS)

		QuestBase::QuestCode	code;
	int						nStatus[QuestInstance::MAX_STATUS];
	unsigned char			nProgress;
	AR_TIME					nTimeLimit;
};

struct TS_SC_QUEST_INFOMATION : TS_MESSAGE
{
	enum QUEST_PROGRESS
	{
		IS_STARTABLE = 0,
		IS_IN_PROGRESS = 1,
		IS_FINISHABLE = 2,
	};

	TS_SC_QUEST_INFOMATION()	_INIT(TM_SC_QUEST_INFOMATION)

		QuestBase::QuestCode	code;
	QUEST_PROGRESS			nProgress;
	unsigned short			trigger_length;
};

struct TS_CS_DROP_QUEST : TS_MESSAGE
{
	TS_CS_DROP_QUEST()	_INIT(TM_CS_DROP_QUEST)

		QuestBase::QuestCode	code;
};

struct TS_CS_QUEST_INFO : TS_MESSAGE
{
	TS_CS_QUEST_INFO()				_INIT(TM_CS_QUEST_INFO)

		QuestBase::QuestCode	code;
};

struct TS_CS_END_QUEST : TS_MESSAGE
{
	TS_CS_END_QUEST()	_INIT(TM_CS_END_QUEST)

		QuestBase::QuestCode	code;
	unsigned char			nOptionalReward;
};

struct TS_SC_TITLE_LIST : TS_MESSAGE
{
	struct TS_TITLE_INFO
	{
		int	code;
		int	status;
	};

	TS_SC_TITLE_LIST()				_INIT(TM_SC_TITLE_LIST)

		unsigned short	count;
	// 이하 TS_TITLE_INFO가 count만큼 붙는다.
};

struct TS_SC_TITLE_CONDITION_LIST : TS_MESSAGE
{
	struct TS_TITLE_CONDITION_INFO
	{
		int type;
		__int64 count;
	};

	TS_SC_TITLE_CONDITION_LIST()	_INIT(TM_SC_TITLE_CONDITION_LIST)

		unsigned short	count;
	// 이하 TS_TITLE_CONDITION_INFO가 count만큼 붙는다.
};

struct TS_SC_REMAIN_TITLE_TIME : TS_MESSAGE
{
	TS_SC_REMAIN_TITLE_TIME()		_INIT(TM_SC_REMAIN_TITLE_TIME)

		AR_TIME remain_title_time;
};

struct TS_CS_SET_MAIN_TITLE : TS_MESSAGE
{
	TS_CS_SET_MAIN_TITLE()	_INIT(TM_CS_SET_MAIN_TITLE)

		int code;
};

struct TS_SC_SET_MAIN_TITLE : TS_MESSAGE
{
	TS_SC_SET_MAIN_TITLE()	_INIT(TM_SC_SET_MAIN_TITLE)

		int handle;
	int code;
};

struct TS_CS_SET_SUB_TITLE : TS_MESSAGE
{
	TS_CS_SET_SUB_TITLE()	_INIT(TM_CS_SET_SUB_TITLE)

		int index;
	int code;
};

struct TS_SC_SET_SUB_TITLE : TS_MESSAGE
{
	TS_SC_SET_SUB_TITLE()	_INIT(TM_SC_SET_SUB_TITLE)

		int index;
	int code;
};

struct TS_CS_BOOKMARK_TITLE : TS_MESSAGE
{
	TS_CS_BOOKMARK_TITLE()	_INIT(TM_CS_BOOKMARK_TITLE)

		int code;
};

struct TS_SC_BOOKMARK_TITLE : TS_MESSAGE
{
	TS_SC_BOOKMARK_TITLE()	_INIT(TM_SC_BOOKMARK_TITLE)

		int code;
	bool bookmarked;
};

struct TS_SC_ACHIEVE_TITLE : TS_MESSAGE
{
	TS_SC_ACHIEVE_TITLE()	_INIT(TM_SC_ACHIEVE_TITLE)

		int code;
};

struct TS_SC_OPEN_TITLE : TS_MESSAGE
{
	TS_SC_OPEN_TITLE()	_INIT(TM_SC_OPEN_TITLE)

		int code;
};

struct TS_SC_CHANGE_TITLE_CONDITION : TS_MESSAGE
{
	TS_SC_CHANGE_TITLE_CONDITION()	_INIT(TM_SC_CHANGE_TITLE_CONDITION)

		int condition_id;
	__int64 count;
};

struct TS_CS_RETURN_LOBBY : TS_MESSAGE
{
	TS_CS_RETURN_LOBBY()	_INIT(TM_CS_RETURN_LOBBY)
};

struct TS_CS_REQUEST_RETURN_LOBBY : TS_MESSAGE
{
	TS_CS_REQUEST_RETURN_LOBBY()	_INIT(TM_CS_REQUEST_RETURN_LOBBY)
};

struct TS_CS_REQUEST_LOGOUT : TS_MESSAGE
{
	TS_CS_REQUEST_LOGOUT()	_INIT(TM_CS_REQUEST_LOGOUT)
};

struct TS_CS_LOGOUT : TS_MESSAGE
{
	TS_CS_LOGOUT()	_INIT(TM_CS_LOGOUT)
};

struct TS_SC_DISCONNECT_DESC : TS_MESSAGE
{
	TS_SC_DISCONNECT_DESC()		_INIT(TM_SC_DISCONNECT_DESC)

		enum _DISCONNECT_TYPE
	{
		// { TS_AG_KICK_CLIENT::_KICK_TYPE
		DISCONNECT_TYPE_ANOTHER_LOGIN = 0,	// 다른 유저의 접속으로 인한 접속 종료
		DISCONNECT_TYPE_DUPLICATED_LOGIN = 1,	// 인증 서버에서 게임 서버 로그인 처리 시 중복된 로그인 감지로 인한 접속 종료(내부 오류 또는 해킹에 의한 발생)
		DISCONNECT_TYPE_BILLING_EXPIRED = 2,	// 결재 정보 만료로 인한 접속 종료
		DISCONNECT_TYPE_GAME_ADDICTION = 3,	// 게임 중독 방지로 인한 접속 종료
		// } TS_AG_KICK_CLIENT::_KICK_TYPE

		DISCONNECT_TYPE_DB_ERROR = 100,	// DB 오류로 인한 예외 처리로 인한 접속 종료
		DISCONNECT_TYPE_ANTI_HACK = 101,	// 게임 보안 솔루션 오류로 인한 접속 종료
		DISCONNECT_TYPE_SCRIPT = 102,	// 스크립트 명령어(SCRIPT_Kick)으로 인한 접속 종료
	};

	char		desc_id;
};

struct TS_CS_START_BOOTH : TS_MESSAGE
{
	struct TS_BOOTH_OPEN_ITEM_INFO
	{
		AR_HANDLE		item_handle;
		int				cnt;
		__int64			gold;
	};

	TS_CS_START_BOOTH()		_INIT(TM_CS_START_BOOTH)

		char					name[49];
	char					type;			// 1이면 판매 부스. 2이면 구매 부스

	unsigned short			cnt;


	// 이하 TS_BOOTH_ITEM_OPEN_INFO들이 CNT만큼 달려서 온다.

};

struct TS_CS_STOP_BOOTH : TS_MESSAGE
{
	TS_CS_STOP_BOOTH()		_INIT(TM_CS_STOP_BOOTH)
};

struct TS_CS_WATCH_BOOTH : TS_MESSAGE
{
	TS_CS_WATCH_BOOTH()		_INIT(TM_CS_WATCH_BOOTH)

		AR_HANDLE	target;			// 보고 싶은 사용자
};

struct TS_SC_WATCH_BOOTH : TS_MESSAGE
{
	struct TS_BOOTH_ITEM_INFO : TS_ITEM_BASE_INFO
	{
		__int64			gold;
	};

	TS_SC_WATCH_BOOTH()		_INIT(TM_SC_WATCH_BOOTH)

		AR_HANDLE	target;			// 노점을 연 사용자
	char		type;			// 1이면 판매 부스. 2이면 구매 부스

	unsigned short cnt;			// 아이템 숫자

	// 아래로 TS_BOOTH_ITEM_INFO가 CNT만큼 붙게 됨
};

struct TS_CS_STOP_WATCH_BOOTH : TS_MESSAGE
{
	TS_CS_STOP_WATCH_BOOTH()		_INIT(TM_CS_STOP_WATCH_BOOTH)

		AR_HANDLE	target;			// 보고 싶은 사용자
};

struct TS_CS_BUY_FROM_BOOTH : TS_MESSAGE
{
	TS_CS_BUY_FROM_BOOTH()		_INIT(TM_CS_BUY_FROM_BOOTH)

		AR_HANDLE	target;			// 노점을 연 사용자
	short		cnt;

	// 이하 TS_ITEM_BASE_INFO	가 count만큼 들어감.
};

struct TS_BOOTH_TRADE_ITEM_INFO : TS_ITEM_BASE_INFO
{
	__int64		price;
};

struct TS_SC_BOOTH_TRADE_INFO : TS_MESSAGE
{
	TS_SC_BOOTH_TRADE_INFO()	_INIT(TM_SC_BOOTH_TRADE_INFO)

		AR_HANDLE	target;		// 거래가 이루어진 대상
	bool		is_sell;	// 판매인지 구매인지 여부. true이면 판매. false이면 구매.
	short		cnt;

	// 이하 TS_BOOTH_TRADE_ITEM_INFO 가 count만큼 들어감.
};

struct TS_CS_SELL_TO_BOOTH : TS_MESSAGE
{
	TS_CS_SELL_TO_BOOTH()		_INIT(TM_CS_SELL_TO_BOOTH)

		AR_HANDLE	target;			// 노점을 연 사용자
	AR_HANDLE	item_handle;	// 판매할 사용자의 아이템 핸들 (노점에 있는 아이템 핸들이 아님!)
	int			cnt;
};

struct TS_CS_GET_BOOTHS_NAME : TS_MESSAGE
{
	TS_CS_GET_BOOTHS_NAME()		_INIT(TM_CS_GET_BOOTHS_NAME)

		unsigned	cnt;

	// 이하 cnt만큼의 AR_HANDLE(플레이어 핸들) 이 달려오게 된다.
};

struct TS_SC_GET_BOOTHS_NAME : TS_MESSAGE
{
	TS_SC_GET_BOOTHS_NAME()		_INIT(TM_SC_GET_BOOTHS_NAME)

		struct TS_BOOTH_NAME
	{
		AR_HANDLE	handle;
		char		name[49];
	};

	unsigned	cnt;

	// 이하 cnt만큼의 TS_BOOTH_NAME 이 달려오게 된다.
};

struct TS_SC_BOOTH_CLOSED : TS_MESSAGE
{
	TS_SC_BOOTH_CLOSED()		_INIT(TM_SC_BOOTH_CLOSED)

		AR_HANDLE	target;			// 노점을 연 사용자
};

struct TS_CS_CHECK_BOOTH_STARTABLE : TS_MESSAGE
{
	TS_CS_CHECK_BOOTH_STARTABLE()		_INIT(TM_CS_CHECK_BOOTH_STARTABLE)
};

struct TS_CS_TURN_ON_PK_MODE : TS_MESSAGE
{
	TS_CS_TURN_ON_PK_MODE()		_INIT(TM_CS_TURN_ON_PK_MODE)
};

struct TS_CS_TURN_OFF_PK_MODE : TS_MESSAGE
{
	TS_CS_TURN_OFF_PK_MODE()	_INIT(TM_CS_TURN_OFF_PK_MODE)
};

struct TS_CS_CHANGE_LOCATION : TS_MESSAGE
{
	TS_CS_CHANGE_LOCATION()	_INIT(TM_CS_CHANGE_LOCATION)

		AR_UNIT x;
	AR_UNIT y;
};

struct TS_SC_CHANGE_LOCATION : TS_MESSAGE
{
	TS_SC_CHANGE_LOCATION()	_INIT(TM_SC_CHANGE_LOCATION)

		int prev_location_id;
	int cur_location_id;
};

struct TS_CS_GET_WEATHER_INFO : TS_MESSAGE
{
	TS_CS_GET_WEATHER_INFO()	_INIT(TM_CS_GET_WEATHER_INFO)

		unsigned int	region_id;
};

struct TS_SC_WEATHER_INFO : TS_MESSAGE
{
	TS_SC_WEATHER_INFO()	_INIT(TM_SC_WEATHER_INFO)

		enum
	{
		CLEAR = 0, LIGHT_RAINY = 1, RAINY = 2, HEAVY_RAINY = 3, LIGHT_SNOWY = 4, SNOWY = 5, HEAVY_SNOWY = 6
	};

	unsigned int	region_id;
	unsigned short	weather_id;
};

struct TS_SC_EMOTION : TS_MESSAGE
{
	TS_SC_EMOTION()		_INIT(TM_SC_EMOTION)

		AR_HANDLE handle;
	int emotion;
};

struct TS_CS_EMOTION : TS_MESSAGE
{
	TS_CS_EMOTION()		_INIT(TM_CS_EMOTION)

		int emotion;
};

struct TS_CS_GAME_TIME : TS_MESSAGE
{
	TS_CS_GAME_TIME()	_INIT(TM_CS_GAME_TIME)
};

struct TS_SC_GAME_TIME : TS_MESSAGE
{
	TS_SC_GAME_TIME()	_INIT(TM_SC_GAME_TIME)

		AR_TIME	t;
	time_t game_time;
};

// 경매 데이터
struct TS_AUCTION_INFO
{
	enum AUCTION_DURATION_TYPE
	{
		DURATION_SHORTTERM = 1,	// 마감 시한: 단기
		DURATION_MIDTERM = 2,	// 마감 시한: 중기
		DURATION_LONGTERM = 3		// 마감 시한: 장기
	};

	int					auction_uid;				// 경매 번호
	TS_ITEM_BASE_INFO	item_info;					// 아이템 정보
	unsigned char		duration_type;				// 마감 시한 타입(AUCTION_DURATION_TYPE 값이 들어감)
	__int64				bidded_price;				// 현재 입찰가
	__int64				instant_purchase_price;		// 즉시 구매가
};

// 경매 물품 검색 요청
struct TS_CS_AUCTION_SEARCH : TS_MESSAGE
{
	TS_CS_AUCTION_SEARCH()				_INIT(TM_CS_AUCTION_SEARCH)

		int		category_id;			// 검색 카테고리(-1: 전체 혹은 기타인데 sub_category_id에 의해 결정 / 0 이상: 상위 카테고리 번호)
	int		sub_category_id;		// 검색 카테고리(category_id가 -1인 경우: -1은 전체, 0은 기타 / 그외: -1은 상위 카테고리임, 0 이상: 하위 카테고리임)
	char	keyword[31];			// 검색 키워드(없으면 빈 문자열이면 됨)
	int		page_num;				// 요청 페이지 번호
	bool	is_equipable;			// 착용 가능한 장비 검색 여부
};

// 경매 물품 검색 결과
struct TS_SC_AUCTION_SEARCH : TS_MESSAGE
{
	TS_SC_AUCTION_SEARCH()				_INIT(TM_SC_AUCTION_SEARCH)

		struct SEARCHED_AUCTION_INFO : TS_AUCTION_INFO
	{
		char				seller_name[31];
		char				flag;

		enum SEARCHED_AUCTION_INFO_FLAG
		{
			FLAG_IS_HIGHEST_BIDDER = 1 << 0,		// 자신이 최고 입찰자인지 여부
			FLAG_NO_OTHER_BIDDER = 1 << 1		// 자신이 최초 입찰자인지 여부(현재 최고 입찰가로 입찰이 가능 - 5% 높은 가격 룰 제외됨)
		};
	};

	int						page_num;															// 현재 페이지 번호
	int						total_page_count;													// 전체 페이지 수
	int						auction_info_count;													// 현재 페이지에 해당하는 경매 데이터 수
	SEARCHED_AUCTION_INFO	auction_info[GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_SEARCHED];	// 현재 페이지에 해당하는 경매 데이터
};

// 자신이 등록한 경매 물품 목록 요청
struct TS_CS_AUCTION_SELLING_LIST : TS_MESSAGE
{
	TS_CS_AUCTION_SELLING_LIST()		_INIT(TM_CS_AUCTION_SELLING_LIST)

		int			page_num;		// 요청 페이지 번호
};

// 자신이 등록한 경매 물품 목록 결과
struct TS_SC_AUCTION_SELLING_LIST : TS_MESSAGE
{
	TS_SC_AUCTION_SELLING_LIST()		_INIT(TM_SC_AUCTION_SELLING_LIST)

		struct REGISTERED_AUCTION_INFO : TS_AUCTION_INFO
	{
		enum REGISTERED_AUCTION_STATUS
		{
			STATUS_NO_BIDDER = 1,	// 상태: 입찰자 없음
			STATUS_BID_CALLING = 2		// 상태: 입찰 진행중
		};

		unsigned char		status;		// REGISTERED_AUCTION_STATUS 값이 들어감
	};

	int						page_num;																// 현재 페이지 번호
	int						total_page_count;														// 전체 페이지 수
	int						auction_info_count;														// 현재 페이지에 해당하는 경매 데이터 수
	REGISTERED_AUCTION_INFO	auction_info[GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_REGISTERED];	// 현재 페이지에 해당하는 경매 데이터
};

// 자신이 입찰한 경매 목록 요청
struct TS_CS_AUCTION_BIDDED_LIST : TS_MESSAGE
{
	TS_CS_AUCTION_BIDDED_LIST()			_INIT(TM_CS_AUCTION_BIDDED_LIST)

		int			page_num;		// 요청 페이지 번호
};

// 자신이 입찰한 경매 목록 결과
struct TS_SC_AUCTION_BIDDED_LIST : TS_MESSAGE
{
	TS_SC_AUCTION_BIDDED_LIST()			_INIT(TM_SC_AUCTION_BIDDED_LIST)

		struct BIDDED_AUCTION_LIST : TS_AUCTION_INFO
	{
		enum BIDDED_AUCTION_STATUS
		{
			STATUS_MINE = 1,	// 입찰 현황: 본인 입찰
			STATUS_OTHERS = 2		// 입찰 현황: 타인 입찰
		};

		unsigned char		status;		// BIDDED_AUCTION_STATUS 값이 들어감
	};

	int						page_num;															// 현재 페이지 번호
	int						total_page_count;													// 전체 페이지 수
	int						auction_info_count;													// 현재 페이지에 해당하는 경매 데이터 수
	BIDDED_AUCTION_LIST		auction_info[GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_BIDDED];	// 현재 페이지에 해당하는 경매 데이터
};

// 경매 입찰 요청
struct TS_CS_AUCTION_BID : TS_MESSAGE
{
	TS_CS_AUCTION_BID()					_INIT(TM_CS_AUCTION_BID)

		int			auction_uid;	// 입찰 대상 경매 번호
	__int64		bidding_price;	// 입찰 요청 가격
};

// 경매 물품 즉시구매 요청(결과는 TS_SC_RESULT로 보내고 페이지 갱신은 클라에서 요청 따로 보냄)
struct TS_CS_AUCTION_INSTANT_PURCHASE : TS_MESSAGE
{
	TS_CS_AUCTION_INSTANT_PURCHASE()	_INIT(TM_CS_AUCTION_INSTANT_PURCHASE)

		int			auction_uid;	// 즉시 구매 대상 경매 번호
};

// 경매 물품 등록 요청(결과는 TS_SC_RESULT로 보내고 페이지 갱신은 클라에서 요청 따로 보냄)
struct TS_CS_AUCTION_REGISTER : TS_MESSAGE
{
	TS_CS_AUCTION_REGISTER()			_INIT(TM_CS_AUCTION_REGISTER)

		enum AUCTION_DURATION_TYPE
	{
		DURATION_SHORTTERM = 1,	// 경매 기간: 단기
		DURATION_MIDTERM = 2,	// 경매 기간: 중기
		DURATION_LONGTERM = 3		// 경매 기간: 장기
	};

	AR_HANDLE				item_handle;				// 경매 등록할 아이템 핸들
	int						item_count;					// 경매 등록할 아이템 수량
	__int64					start_price;				// 경매 시작가
	__int64					instant_purchase_price;		// 경매 물품 즉구가
	unsigned char			duration_type;				// 경매 기간 타입(AUCTION_DURATION_TYPE 값이 들어감)
};

// 자신이 등록한 경매 취소 요청(결과는 TS_SC_RESULT로 보내고 페이지 갱신은 클라에서 요청 따로 보냄)
struct TS_CS_AUCTION_CANCEL : TS_MESSAGE
{
	TS_CS_AUCTION_CANCEL()				_INIT(TM_CS_AUCTION_CANCEL)

		int			auction_uid;	// 취소할 경매 번호
};

// 자신 소유 보관 아이템 목록 요청
struct TS_CS_ITEM_KEEPING_LIST : TS_MESSAGE
{
	TS_CS_ITEM_KEEPING_LIST()			_INIT(TM_CS_ITEM_KEEPING_LIST)

		int			page_num;		// 요청 페이지 번호
};

// 자신 소유 보관 아이템 목록 결과
struct TS_SC_ITEM_KEEPING_LIST : TS_MESSAGE
{
	TS_SC_ITEM_KEEPING_LIST()			_INIT(TM_SC_ITEM_KEEPING_LIST)

		// 아이템 보관 데이터
		struct TS_ITEM_KEEPING_INFO
	{
		int					keeping_uid;				// 아이템 보관 번호
		TS_ITEM_BASE_INFO	item_info;					// 아이템 정보
		int					duration;					// 보관 기간
		unsigned char		keeping_type;				// 보관 타입(AuctionBase.h의 enum KEEPING_TYPE 참조)
		ItemBase::ItemCode	related_item_code;			// 관련 아이템 코드(보관 아이템이 돈일 경우만 사용됨)
		int					related_item_enhance;		// 관련 아이템 강화 레벨(보관 아이템이 돈일 경우만 사용됨)
		int					related_item_level;			// 관련 아이템 대장작 레벨(보관 아이템이 돈일 경우만 사용됨)
	};

	int						page_num;																// 현재 페이지 번호
	int						total_page_count;														// 전체 페이지 수
	int						keeping_info_count;														// 현재 페이지에 해당하는 아이템 보관 데이터 수
	TS_ITEM_KEEPING_INFO	keeping_info[GameRule::AUCTION_INFO_COUNT_PER_PAGE_FOR_ITEM_KEEPING];	// 현재 페이지에 해당하는 아이템 보관 데이터
};

// 자신 소유 보관 아이템 회수 요청(결과는 TS_SC_RESULT로 보내고 페이지 갱신은 클라에서 요청 따로 보냄)
struct TS_CS_ITEM_KEEPING_TAKE : TS_MESSAGE
{
	TS_CS_ITEM_KEEPING_TAKE()			_INIT(TM_CS_ITEM_KEEPING_TAKE)

		int			keeping_uid;	// 요청 아이템 보관 번호
};

// 헌터홀릭 인스턴스 던전 목록 송신 요청
struct TS_CS_HUNTAHOLIC_INSTANCE_LIST : TS_MESSAGE
{
	TS_CS_HUNTAHOLIC_INSTANCE_LIST()			_INIT(TM_CS_HUNTAHOLIC_INSTANCE_LIST)

		int		page;		// 목록 페이지 번호
};

// 헌터홀릭 인스턴스 던전 정보 보관용(메시지/데이터 공용)
struct TS_HUNTAHOLIC_INSTANCE_INFO
{
	int				instance_no;													// 방 번호
	char			name[GameRule::HUNTAHOLIC_MAX_INSTANCE_NAME_LENGTH + 1];		// 방 이름
	unsigned char	current_member_count;											// 현재 참가 인원
	unsigned char	max_member_count;												// 최대 참가 인원
	bool			require_password;												// 비번방 여부(true: 비번방, false: 공개방)
};

// 헌터홀릭 인스턴스 던전 목록
struct TS_SC_HUNTAHOLIC_INSTANCE_LIST : TS_MESSAGE
{
	TS_SC_HUNTAHOLIC_INSTANCE_LIST()			_INIT(TM_SC_HUNTAHOLIC_INSTANCE_LIST)

		int		huntaholic_id;	// 헌터홀릭 ID
	int		page;			// 현재 페이지 번호
	int		count;			// 현재 페이지에 해당하는 인스턴스 던전 개수
	int		total_page;		// 전체 인스턴스 목록의 페이지 수

	// 이 뒤로 TS_HUNTAHOLIC_INSTANCE_INFO 가 count 개 만큼 이어 붙음
};

// 헌터홀릭 인스턴스 던전 정보(현재 소속된 던전만)
struct TS_SC_HUNTAHOLIC_INSTANCE_INFO : TS_MESSAGE
{
	TS_SC_HUNTAHOLIC_INSTANCE_INFO()			_INIT(TM_SC_HUNTAHOLIC_INSTANCE_INFO)

		TS_HUNTAHOLIC_INSTANCE_INFO	info;
};

// 헌터홀릭 인스턴스 던전 생성 요청
struct TS_CS_HUNTAHOLIC_CREATE_INSTANCE : TS_MESSAGE
{
	TS_CS_HUNTAHOLIC_CREATE_INSTANCE()			_INIT(TM_CS_HUNTAHOLIC_CREATE_INSTANCE)

		char			name[GameRule::HUNTAHOLIC_MAX_INSTANCE_NAME_LENGTH + 1];
	unsigned char	max_member_count;
	char			password[GameRule::HUNTAHOLIC_MAX_INSTANCE_PASSWORD_LENGTH + 1];
};

// 헌터홀릭 인스턴스 던전 참가 요청
struct TS_CS_HUNTAHOLIC_JOIN_INSTANCE : TS_MESSAGE
{
	TS_CS_HUNTAHOLIC_JOIN_INSTANCE()			_INIT(TM_CS_HUNTAHOLIC_JOIN_INSTANCE)

		int			instance_no;
	char		password[GameRule::HUNTAHOLIC_MAX_INSTANCE_PASSWORD_LENGTH + 1];
};

// 헌터홀릭 인스턴스 던전 탈퇴 요청
struct TS_CS_HUNTAHOLIC_LEAVE_INSTANCE : TS_MESSAGE
{
	TS_CS_HUNTAHOLIC_LEAVE_INSTANCE()			_INIT(TM_CS_HUNTAHOLIC_LEAVE_INSTANCE)
};

// 헌터홀릭 인스턴스 던전 사냥 결과
struct TS_SC_HUNTAHOLIC_HUNTING_SCORE : TS_MESSAGE
{
	TS_SC_HUNTAHOLIC_HUNTING_SCORE()			_INIT(TM_SC_HUNTAHOLIC_HUNTING_SCORE)

		int		huntaholic_id;			// 헌터홀릭 ID
	int		personal_kill_count;	// 개인 킬 수
	int		personal_score;			// 개인 점수
	int		kill_count;				// 킬 수(해당 인스턴스 던전 내의 합산)
	int		score;					// 점수(해당 인스턴스 던전 내의 합산)
	double	point_advantage;		// 방 레벨대에 따른 점수 증폭치
	double	point_rate;				// 올림방 여부에 따른 증폭치(사용되지 않음. 무조건 1.0)
	int		gain_point;				// 최종 계산된 획득 포인트
	char	result_type;			// 사냥 결과(0: 성공, 1: 사망, 2: 리타이어)
};

// 헌터홀릭 인스턴스 던전 사냥 중 점수 갱신
struct TS_SC_HUNTAHOLIC_UPDATE_SCORE : TS_MESSAGE
{
	TS_SC_HUNTAHOLIC_UPDATE_SCORE()				_INIT(TM_SC_HUNTAHOLIC_UPDATE_SCORE)

		int		kill_count;			// 킬 수
	int		score;				// 킬 수에 따른 점수
};

// 헌터홀릭 로비에서 로비 입장 지점으로 귀환
struct TS_CS_HUNTAHOLIC_LEAVE_LOBBY : TS_MESSAGE
{
	TS_CS_HUNTAHOLIC_LEAVE_LOBBY()				_INIT(TM_CS_HUNTAHOLIC_LEAVE_LOBBY)
};

// 헌터홀릭 로비에서 던전으로 입장, 사냥 시작을 클라이언트에 알림
struct TS_SC_HUNTAHOLIC_BEGIN_HUNTING : TS_MESSAGE
{
	TS_SC_HUNTAHOLIC_BEGIN_HUNTING()			_INIT(TM_SC_HUNTAHOLIC_BEGIN_HUNTING)

		AR_TIME		begin_time;
};

// 헌터홀릭 던전에서 최대 획득 가능 점수에 도달
struct TS_SC_HUNTAHOLIC_MAX_POINT_ACHIEVED : TS_MESSAGE
{
	TS_SC_HUNTAHOLIC_MAX_POINT_ACHIEVED()		_INIT(TM_SC_HUNTAHOLIC_MAX_POINT_ACHIEVED)
};

// 헌터홀릭 로비에서 던전으로 입장 요청(방에 참여 중인 유저들이 사냥 던전으로 진입)
struct TS_CS_HUNTAHOLIC_BEGIN_HUNTING : TS_MESSAGE
{
	TS_CS_HUNTAHOLIC_BEGIN_HUNTING()			_INIT(TM_CS_HUNTAHOLIC_BEGIN_HUNTING)
};

// 헌터홀릭 로비에서 던전으로 입장을 위한 카운트 다운 시작(이후 10초간 카운트하고 TS_SC_HUNTAHOLIC_BEGIN_HUNTING 메시지 날아감)
struct TS_SC_HUNTAHOLIC_BEGIN_COUNTDOWN : TS_MESSAGE
{
	TS_SC_HUNTAHOLIC_BEGIN_COUNTDOWN()			_INIT(TM_SC_HUNTAHOLIC_BEGIN_COUNTDOWN)
};

// 인스턴스 게임으로 워프
struct TS_CS_INSTANCE_GAME_ENTER : TS_MESSAGE
{
	enum MAP_TYPE
	{
		HUNTAHOLIC_BEAR_ROAD = 0,
		RANKED_DEATHMATCH = 1,
		FREED_DEATHMATCH = 2,
	};

	TS_CS_INSTANCE_GAME_ENTER()			_INIT(TM_CS_INSTANCE_GAME_ENTER)

		int					instance_game_type;
};

// 인스턴스 게임에서 원래 맵으로 복귀
struct TS_CS_INSTANCE_GAME_EXIT : TS_MESSAGE
{
	TS_CS_INSTANCE_GAME_EXIT()				_INIT(TM_CS_INSTANCE_GAME_EXIT)
};

// 인스턴스 게임의 스코어 요청
struct TS_CS_INSTANCE_GAME_SCORE_REQUEST : TS_MESSAGE
{
	TS_CS_INSTANCE_GAME_SCORE_REQUEST()		_INIT(TM_CS_INSTANCE_GAME_SCORE_REQUEST)
};

// 인스턴스 게임의 스코어 전달
struct TS_SC_INSTANCE_GAME_SCORE_REQUEST : TS_MESSAGE
{
	TS_SC_INSTANCE_GAME_SCORE_REQUEST()		_INIT(TM_SC_INSTANCE_GAME_SCORE_REQUEST)

		// 베어로드
		unsigned int		holicpoint;
	unsigned int		bearroad_ranking;

	// 데스매치
	unsigned int		deathmatch_kill_count;
	unsigned int		deathmatch_death_count;

	// 배틀 아레나
	int					battle_arena_point;
	int					battle_arena_mvp_count;
	int					battle_arena_record[3 /* _BATTLE_ARENA_TYPE */][2 /* win, lose */];
};

// 플레이어가 타 플레이어에게 대련 요청
struct TS_CS_COMPETE_REQUEST : TS_MESSAGE
{
	TS_CS_COMPETE_REQUEST()			_INIT(TM_CS_COMPETE_REQUEST)

		unsigned char		compete_type;		// _COMPETE_TYPE 값이 사용됨
	char				requestee[31];	// 도전 받은 플레이어 이름(개인/파티전은 플레이어/파티장 이름, 길드/연합전은 길드/연합의 이름이 사용됨)
};

// 대련 요청받은 플레이어에게 요청 전달
struct TS_SC_COMPETE_REQUEST : TS_MESSAGE
{
	TS_SC_COMPETE_REQUEST()			_INIT(TM_SC_COMPETE_REQUEST)

		unsigned char		compete_type;		// _COMPETE_TYPE 값이 사용됨
	char				requester[31];	// 도전자 이름(개인/파티전은 플레이어/파티장 이름, 길드/연합전은 길드/연합의 이름이 사용됨)
};

// 대련 요청받은 플레이어가 응답
struct TS_CS_COMPETE_ANSWER : TS_MESSAGE
{
	TS_CS_COMPETE_ANSWER()			_INIT(TM_CS_COMPETE_ANSWER)

		unsigned char		compete_type;	// _COMPETE_TYPE 값이 사용됨
	unsigned char		answer_type;	// _COMPETE_ANSWER_TYPE 값이 사용됨
};

// 최초 대련 요청에 대한 결과 전달(수락/거부/자동거부)
struct TS_SC_COMPETE_ANSWER : TS_MESSAGE
{
	TS_SC_COMPETE_ANSWER()			_INIT(TM_SC_COMPETE_ANSWER)

		unsigned char		compete_type;		// _COMPETE_TYPE 값이 사용됨
	unsigned char		answer_type;		// _COMPETE_ANSWER_TYPE 값이 사용됨
	char				requestee[31];	// 도전 받은 플레이어 이름(개인/파티전은 플레이어/파티장 이름, 길드/연합전은 길드/연합의 이름이 사용됨)
};

// 대련 시작 전 카운트 다운 개시
struct TS_SC_COMPETE_COUNTDOWN : TS_MESSAGE
{
	TS_SC_COMPETE_COUNTDOWN()		_INIT(TM_SC_COMPETE_COUNTDOWN)

		unsigned char		compete_type;		// _COMPETE_TYPE 값이 사용됨
	char				competitor[31];	// 상대방 플레이어 이름(개인/파티전은 플레이어/파티장 이름, 길드/연합전은 길드/연합의 이름이 사용됨)
	AR_HANDLE			handle_competitor;	// 상대방 플레이어 핸들(1:1 PVP에서만 사용됨)
};

// 대련 시작
struct TS_SC_COMPETE_START : TS_MESSAGE
{
	TS_SC_COMPETE_START()			_INIT(TM_SC_COMPETE_START)

		unsigned char		compete_type;		// _COMPETE_TYPE 값이 사용됨
	char				competitor[31];	// 상대방 플레이어 이름(개인/파티전은 플레이어/파티장 이름, 길드/연합전은 길드/연합의 이름이 사용됨)
};

// 대련 종료(결과 방송)
struct TS_SC_COMPETE_END : TS_MESSAGE
{
	TS_SC_COMPETE_END()				_INIT(TM_SC_COMPETE_END)

		unsigned char		compete_type;		// _COMPETE_TYPE 값이 사용됨
	unsigned char		end_type;			// _COMPETE_END_TYPE 값이 사용됨
	char				winner[31];		// 승자 이름(개인/파티전은 플레이어/파티장 이름, 길드/연합전은 길드/연합의 이름이 사용됨)
	char				loser[31];		// 패자 이름(개인/파티전은 플레이어/파티장 이름, 길드/연합전은 길드/연합의 이름이 사용됨)
};

struct TS_SC_BATTLE_ARENA_PENALTY_INFO : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_PENALTY_INFO()			_INIT(TM_SC_BATTLE_ARENA_PENALTY_INFO)

		AR_TIME			nBlockTime;
	int				nPenaltyCount;
	AR_TIME			nPenaltyCountDecTime;
};

struct TS_CS_BATTLE_ARENA_JOIN_QUEUE : TS_MESSAGE
{
	TS_CS_BATTLE_ARENA_JOIN_QUEUE()				_INIT(TM_CS_BATTLE_ARENA_JOIN_QUEUE)

		int					nArenaID;
};

struct TS_SC_BATTLE_ARENA_JOIN_QUEUE : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_JOIN_QUEUE()				_INIT(TM_SC_BATTLE_ARENA_JOIN_QUEUE)

		int					nArenaID;

	/*
	// 아레나 유저 등급 구분
	enum _BATTLE_GRADE
	{
		BG_INVALID		= -1,

		BG_ROOKIE		= 0,
		BG_GROW			= 1,
		BG_MAJOR		= 2,

		MAX_BATTLE_GRADE,
	};
	*/

	_BATTLE_GRADE		eGrade;
};

struct TS_SC_BATTLE_ARENA_UPDATE_WAIT_USER_COUNT : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_UPDATE_WAIT_USER_COUNT()		_INIT(TM_SC_BATTLE_ARENA_UPDATE_WAIT_USER_COUNT)

		int			nWaitUserCount;
};

struct TS_CS_BATTLE_ARENA_LEAVE : TS_MESSAGE
{
	TS_CS_BATTLE_ARENA_LEAVE()					_INIT(TM_CS_BATTLE_ARENA_LEAVE)
};

struct TS_SC_BATTLE_ARENA_LEAVE : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_LEAVE()					_INIT(TM_SC_BATTLE_ARENA_LEAVE)

		/*
		// 아레나 이탈 유형 코드
		enum _ARENA_LEAVE_TYPE
		{
			ALT_UNKNOWN							= 0,
			ALT_USER_REQUEST					= 1,	// 대기열/경기 이탈 두 경우 모두 사용됨
			ALT_GAME_OVER						= 2,	// 경기 이탈 시에만 사용됨
			ALT_PARTICIPATING_SPECIAL_PARTY		= 3,	// 대기열 이탈 시에만 사용됨
			ALT_FAILED_TO_JOIN_BATTLE			= 4,	// 대기열 이탈 시에만 사용됨
			ALT_FAILED_TO_START_BATTLE			= 5,	// 대기열 이탈 시에만 사용됨(경기 시작 실패)
			ALT_DISCONNECT						= 6,	// 대기열 이탈 시에만 사용됨(대기열에 있다가 접속 끊긴 경우)
			ALT_ABSENT							= 7,	// 경기 중에 잠수 신고를 통해 경기에서 이탈된 경우
			ALT_LEAVE_ARENA						= 8,	// 아레나에서 워프를 통해 다른 지역으로 이탈함으로 인해 경기에서 이탈된 경우(워프 자체가 근본적으로 차단되어야 함)
			ALT_ENTER_INSTANCE_DUNGEON			= 9,	// 인스턴스 던전에 입장하기 위해 대기열에서 이탈되는 경우
			ALT_ENTER_DEATHMATCH				= 10,	// 데스매치 존에 입장하기 위해 대기열에서 이탈되는 경우
			ALT_ENTER_HUNTAHOLIC				= 11,	// 헌터홀릭 로비(?)에 입장하기 위해 대기열에서 이탈되는 경우
			ALT_EXERCISE_PARTY_KICK				= 12,	// 연습 경기 파티 구성 중 추방에 의해 이탈되는 경우
		};
		*/

		_ARENA_LEAVE_TYPE	eLeaveType;
	char				szName[19];
};

struct TS_SC_BATTLE_ARENA_BATTLE_INFO : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_BATTLE_INFO()			_INIT(TM_SC_BATTLE_ARENA_BATTLE_INFO)

		int					nArenaID;

	// 패킷 수신자 개인 종속적인 내용
	AR_TIME				nForceEnterTime;

	// 경기 종속적인 내용(점수는 별도의 패킷이 있으므로 제외)
	AR_TIME				nStartTime;
	_BATTLE_GRADE		eGrade;
	AR_TIME				nEndTime;

	unsigned char		nPlayerCountPerTeam[2];

	struct PlayerInfo
	{
		AR_HANDLE		handle;				// handle == 0 이면 오프라인 유저
		int				nJobID;
		char			szName[19];		// 기존에 AR_HANDLE과 묶인 이름 정보가 있더라도 가명에 의해 다른 이름이 지정될 수 있음
	};
	// 이 뒤로 PlayerInfo가 nPlayerCountPerTeam[ 0 ] + nPlayerCountPerTeam[ 1 ]개 만큼 붙음
};

struct TS_CS_BATTLE_ARENA_ENTER_WHILE_COUNTDOWN : TS_MESSAGE
{
	TS_CS_BATTLE_ARENA_ENTER_WHILE_COUNTDOWN()	_INIT(TM_CS_BATTLE_ARENA_ENTER_WHILE_COUNTDOWN)
};

struct TS_CS_BATTLE_ARENA_EXERCISE_READY : TS_MESSAGE
{
	TS_CS_BATTLE_ARENA_EXERCISE_READY()			_INIT(TM_CS_BATTLE_ARENA_EXERCISE_READY)

		bool	bReady;
};

struct TS_SC_BATTLE_ARENA_EXERCISE_READY_STATUS : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_EXERCISE_READY_STATUS()	_INIT(TM_SC_BATTLE_ARENA_EXERCISE_READY_STATUS)

		int		nReadyState;
};

struct TS_CS_BATTLE_ARENA_EXERCISE_START : TS_MESSAGE
{
	TS_CS_BATTLE_ARENA_EXERCISE_START()			_INIT(TM_CS_BATTLE_ARENA_EXERCISE_START)
};

struct TS_SC_BATTLE_ARENA_BATTLE_STATUS : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_BATTLE_STATUS()			_INIT(TM_SC_BATTLE_ARENA_BATTLE_STATUS)

		int					nStatus;

	/*
	enum _PROP_STATE
	{
		PS_NOT_EXIST		= 0,
		PS_NEUTRAL			= 1,
		PS_OWNED_BY_TEAM_0	= 2,
		PS_OWNED_BY_TEAM_1	= 3,
	};
	*/

	_PROP_STATE GetPropState(int nPropIndex) const { assert(nPropIndex >= 0 && nPropIndex < 9); return static_cast<_PROP_STATE>((nStatus >> (nPropIndex * 2)) & 0x3); }
	bool IsActivatedBingoLine(int nLineIndex) { assert(nLineIndex >= 0 && nLineIndex < 8); return ((nStatus >> 24) & (1 << nLineIndex)) != 0; }

	// 경기 타입에 따라 nStatus의 의미가 달라짐
	// 클래식 - 이 패킷 자체를 사용하지 않음
	// 슬래터 - 슬래터 프랍 소속 상태(GetPropState( 0 )으로 얻을 수 있음)
	// 빙고  - 3x3 프랍의 소속 정보 및 빙고 달성 정보
	//        1. 뒤에서부터 2 비트( ( nStatus >> ( nPropIndex * 2 ) ) & 0x3 )씩 잘라서 각 프랍의 소속 상태로 사용(0: 프랍 없음, 1: 중립, 2: 연합군, 3: 마녀군)
	//           총 9개의 프랍이 있으므로 32비트 중 18비트가 사용됨
	//           * GetPropState을 사용해 각 프랍별 상태를 확인할 수 있음
	//        2. 최상위의 8비트( int nActivatedLineFlag = (nStatus >> 24) & 0xFF )를 이번 nStatus 변경으로 인해 달성된 빙고 라인이 어느 것인지 표현
	//           0---1---2
	//           |   |   |
	//           3---4---5
	//           |   |   |
	//           6---7---8
	//           위와 같이 각 빙고 프랍에 대해 0 ~ 8 까지 Index가 지정되어 있을 때
	//           0 - 1 - 2 (가로줄 1)이 달성되었으면 최하위 0번째 비트가 On ( nActivatedLineFlag & ( 1 << 0 ) ) != 0
	//           3 - 4 - 5 (가로줄 2)가 달성되었으면 최하위 1번째 비트가 On ( nActivatedLineFlag & ( 1 << 1 ) ) != 0
	//           6 - 7 - 8 (가로줄 3)이 달성되었으면 최하위 2번째 비트가 On ( nActivatedLineFlag & ( 1 << 2 ) ) != 0
	//           0 - 3 - 6 (세로줄 1)이 달성되었으면 최하위 3번째 비트가 On ( nActivatedLineFlag & ( 1 << 3 ) ) != 0
	//           1 - 4 - 7 (세로줄 2)가 달성되었으면 최하위 4번째 비트가 On ( nActivatedLineFlag & ( 1 << 4 ) ) != 0
	//           2 - 5 - 8 (세로줄 3)이 달성되었으면 최하위 5번째 비트가 On ( nActivatedLineFlag & ( 1 << 5 ) ) != 0
	//           0 - 4 - 8 (대각선 1)이 달성되었으면 최하위 6번째 비트가 On ( nActivatedLineFlag & ( 1 << 6 ) ) != 0
	//           2 - 4 - 6 (대각선 2)가 달성되었으면 최하위 7번째 비트가 On ( nActivatedLineFlag & ( 1 << 7 ) ) != 0
	//         * 빙고에 참여된 프랍은 사라져야 하기 때문에 ( ( nStatus >> nPropIdx * 2 ) & 0x3 ) 값이 0이 아니었다가 0이 됨(빙고라는 걸 방송하는 시점에는 0이 된 상태로 방송됨)
	//         * >> (Bit right shift) 연산을 nStatus 에 할 때, signed 변수이기 때문에 만일 최상위 비트가 On 되어있었을 경우(int 값으로는 음수)에는
	//           왼쪽에서부터 1이 채워져 들어오게 되므로 Shift 연산 후의 결과가 음수가 됨. 따라서 Shift 연산 후에는 반드시 & (Bitwise AND) 연산자를 통해 얻고자 하는 값만 잘라내야 함
	//         * IsActivatedBingoLine 함수로 특정 라인이 빙고 상태가 됐는지 확인 가능
};

struct TS_SC_BATTLE_ARENA_BATTLE_SCORE : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_BATTLE_SCORE()			_INIT(TM_SC_BATTLE_ARENA_BATTLE_SCORE)

		struct TeamScore
	{
		short		nTotalKillCount;	// 각 팀별 킬 수 총합
		short		nTotalDeathCount;	// 각 팀별 데스 수 총합
		short		nScore;				// 각 팀별 현재 점수
	};

	struct PlayerScore
	{
		char		szName[19];
		short		nKillCount;
		short		nDeathCount;
		short		nPropActivateCount;
		int			nTotalGainAP;
	};

	TeamScore		aTeamScore[2 /* GameRule::BATTLE_ARENA_MAX_TEAM_COUNT */];

	unsigned char	nPlayerScoreCount;
	// 이 뒤로 PlayerScore가 nPlayerScoreCount개 만큼 붙음
};

struct TS_SC_BATTLE_ARENA_JOIN_BATTLE : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_JOIN_BATTLE()			_INIT(TM_SC_BATTLE_ARENA_JOIN_BATTLE)

		int				nTeamNo;
	AR_HANDLE		handle;
	int				nJobID;
	char			szName[19];
};

struct TS_SC_BATTLE_ARENA_DISCONNECT_BATTLE : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_DISCONNECT_BATTLE()		_INIT(TM_SC_BATTLE_ARENA_DISCONNECT_BATTLE)

		AR_HANDLE		handle;
};

struct TS_SC_BATTLE_ARENA_RECONNECT_BATTLE : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_RECONNECT_BATTLE()		_INIT(TM_SC_BATTLE_ARENA_RECONNECT_BATTLE)

		// 유저가 로그아웃되었다가 다시 접속될 경우 서버에서 AR_HANDLE값이 재부여되기 때문에
		// 클라이언트에서는 기존의 AR_HANDLE 값을 사용하면 안되고 이 패킷으로 받은 값으로 갱신해서 사용해야 함
		// 이 경우 어떤 유저가 재접속했는지는 szName을 기준으로 처리해야 함
		char			szName[19];
	AR_HANDLE		newHandle;
};

struct TS_SC_BATTLE_ARENA_RESULT : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_RESULT()					_INIT(TM_SC_BATTLE_ARENA_RESULT)

		/*
		// 아레나 경기 종료 타입
		enum _ARENA_END_TYPE
		{
			AET_UNKNOWN						= 0,

			AET_ONE_TEAM_NO_MEMBER			= 1,	// 한 팀의 멤버가 모두 이탈하여 경기 종료(종료 유형이지만 보상이 없다는 내용도 포함)
			AET_MAX_SCORE_REACHED			= 2,	// 목표 점수 도달로 경기 종료
			AET_TIME_OVER					= 3,	// 시간 초과로 경기 종료
		};

		// 아레나 경기 보상 타입
		enum _ARENA_REWARD_TYPE
		{
			ART_UNKNOWN						= 0,

			ART_FULL_REWARD					= 1,	// 값 없음(지정 안하면 이거)
			ART_HALF_REWARD_BY_FEW_MEMBER	= 2,	// 한 팀의 멤버 수가 팀 최대 인원의 50% 미만이어서 보상이 절반으로 감소
			ART_NO_REWARD_BY_NO_MEMBER		= 3,	// 한 팀의 멤버가 모두 이탈해서 경기가 중단되어 보상 없음
			ART_NO_REWARD_BY_MIN_TIME		= 4,	// 최소 경기 시간 미달로 인해 보상 없음
			ART_NO_REWARD_BY_EXERCISE		= 5,	// 연습 경기는 보상 없음
		};
		*/

		_ARENA_END_TYPE		eEndType;
	_ARENA_REWARD_TYPE	eRewardType;
	int					nWinnerTeamNo;		// 0: 연합군, 1:마녀군

	unsigned char		nMVPCount;
	// 이 뒤로 char[ 19 ] 가 nMVPCount 만큼 붙음
};

struct TS_CS_BATTLE_ARENA_ABSENCE_CHECK_REQUEST : TS_MESSAGE
{
	TS_CS_BATTLE_ARENA_ABSENCE_CHECK_REQUEST()		_INIT(TM_CS_BATTLE_ARENA_ABSENCE_CHECK_REQUEST)

		AR_HANDLE		hCheckTarget;
};

struct TS_SC_BATTLE_ARENA_ABSENCE_CHECK : TS_MESSAGE
{
	TS_SC_BATTLE_ARENA_ABSENCE_CHECK()				_INIT(TM_SC_BATTLE_ARENA_ABSENCE_CHECK)

		AR_TIME			nLimitTime;
};

struct TS_CS_BATTLE_ARENA_ABSENCE_CHECK_ANSWER : TS_MESSAGE
{
	TS_CS_BATTLE_ARENA_ABSENCE_CHECK_ANSWER()		_INIT(TM_CS_BATTLE_ARENA_ABSENCE_CHECK_ANSWER)

		bool			bSuccess;
};

// 랭킹 타입별 상위 랭킹 차트와 자신의 랭크 요청
struct TS_CS_RANKING_TOP_RECORD : TS_MESSAGE
{
	TS_CS_RANKING_TOP_RECORD()		_INIT(TM_CS_RANKING_TOP_RECORD)

		unsigned char		ranking_type;		// _RANKING_TYPE 값이 사용됨
};

// 랭킹 타입별 상위 랭킹 차트와 요청자의 랭크
struct TS_SC_RANKING_TOP_RECORD : TS_MESSAGE
{
	TS_SC_RANKING_TOP_RECORD()		_INIT(TM_SC_RANKING_TOP_RECORD)

		unsigned char		ranking_type;		// _RANKING_TYPE 값이 사용됨
	unsigned short		requester_rank;		// 요청자 랭크(0이면 랭킹 미등록)
	__int64				requester_score;	// 요청자 점수

	struct RANKING_RECORD
	{
		unsigned short	rank;
		char			ranker_name[31];
		__int64			score;
	};

	unsigned short		record_count;

	// 이 뒤로 RANKING_RECORD 가 record_count 개 만큼 이어 붙음
};


// 이하 크리처 농장 관련 패킷

// 크리처 농장 정보 요청
struct TS_CS_REQUEST_FARM_INFO : TS_MESSAGE
{
	TS_CS_REQUEST_FARM_INFO()	_INIT(TM_CS_REQUEST_FARM_INFO)
};

// 크리처 농장 정보 반환
struct TS_SC_FARM_INFO : TS_MESSAGE
{
	TS_SC_FARM_INFO()			_INIT(TM_SC_FARM_INFO)

		char						creature_count;

	struct SUMMON_INFO {
		int index;				// 농장 슬롯
		__int64 exp;
		char name[19];
		int duration;			// 맡기는 기간
		int elasped_time;		// 경과 시간		
		int refresh_time;		// 돌보기할 수 있는 시점까지 남은 시간
		char using_cash;
		char using_cracker;
		TS_ITEM_BASE_INFO card_info; // 크리처 카드의 아이템 정보
	};

	// 이후 creature_count만큼 SUMMON_INFO 구조체가 따라옴.
};

// 크리처 맡기기 요청
struct TS_CS_FOSTER_CREATURE : TS_MESSAGE
{
	TS_CS_FOSTER_CREATURE()		_INIT(TM_CS_FOSTER_CREATURE)

		AR_HANDLE					creature_card_handle;
	int							ticket_info_count;
	int							cracker_info_count;

	struct TICKET_INFO {
		AR_HANDLE				ticket_handle;
		int						ticket_count;
	};

	struct CRACKER_INFO {
		AR_HANDLE				cracker_handle;
		int						cracker_count;
	};

	// 이후 TICKET_INFO, CRACKER_INFO 순서로 관련 정보들이 ticket_info_count, cracker_info_count만큼 따라옴
};

// 크리처 맡기기 결과
struct TS_SC_RESULT_FOSTER : TS_MESSAGE
{
	TS_SC_RESULT_FOSTER()		_INIT(TM_SC_RESULT_FOSTER)

		char						result;
};

// 크리처 꺼내기 요청
struct TS_CS_RETRIEVE_CREATURE : TS_MESSAGE
{
	TS_CS_RETRIEVE_CREATURE()	_INIT(TM_CS_RETRIEVE_CREATURE)

		AR_HANDLE					creature_card_handle;
};

// 크리처 꺼내기 결과 
struct TS_SC_RESULT_RETRIEVE : TS_MESSAGE
{
	TS_SC_RESULT_RETRIEVE()		_INIT(TM_SC_RESULT_RETRIEVE)

		char						result;
};

// 크리처 돌보기 요청
struct TS_CS_NURSE_CREATURE : TS_MESSAGE
{
	TS_CS_NURSE_CREATURE()		_INIT(TM_CS_NURSE_CREATURE)

		AR_HANDLE					creature_card_handle;
};

// 크리처 돌보기 결과
struct TS_SC_RESULT_NURSE : TS_MESSAGE
{
	TS_SC_RESULT_NURSE()		_INIT(TM_SC_RESULT_NURSE)

		char						result;

	enum Result
	{
		FAILED = 0,	// 무언가 잘못된 정보를 보냈으므로 요청을 무시.
		NO_REWARD = 1,	// 돌보기를 하였으나 크리처가 보상을 주지는 않음
		REWARDED = 2,	// 돌보기 결과 크리처가 보상을 줌
	};
};

struct TS_CS_REQUEST_FARM_MARKET : TS_MESSAGE
{
	TS_CS_REQUEST_FARM_MARKET()		_INIT(TM_CS_REQUEST_FARM_MARKET)
};
// 크리처 농장 패킷 끝





///////////////////////////////////////////////////////////////////
// 파티매치 메시지
#define PARTYMATCH_MEMBER_MAX	10
#define PARTYMATCH_ROOM_MAX		100
#define PARTYMATCH_ROOM_PAGE	10
#define PARTYMATCH_ROOM_TITLE	32
struct STRUCT_PARTYMATCH_ROOM
{
	int		nRoom;			// Room 이 0이면 방이 없는것
	int		nParty;
	short   nArea;
	short   nLevel;
	short	nMember;
	short	nMaxMember;
	char	szText[PARTYMATCH_ROOM_TITLE];
};
struct STRUCT_PARTYMATCH_MEMBER
{
	int				nMode;		// 맴버 표시정보. 현재로서는 0x80000000 | (nLevel<<16) | nJobID;
	//short			nLevel;
	//short			nJobID;
	AR_HANDLE		nMember;
	char		    szName[20];
};
struct TS_CS_PARTYMATCH_ACTION : TS_MESSAGE
{
	TS_CS_PARTYMATCH_ACTION()	_INIT(TM_CS_PARTYMATCH_ACTION)
		int				nAction;	// 0:목록요청	1:맴버요청	2:방만들기	3:방없애기   4:Apply
	int				nPage;		// 목록요청에만 사용.
	AR_HANDLE		nMaster;	// Apply 에만 사용.
	STRUCT_PARTYMATCH_ROOM stRoom;
};

struct TS_SC_PARTYMATCH_LIST : TS_MESSAGE
{
	TS_SC_PARTYMATCH_LIST()	_INIT(TM_SC_PARTYMATCH_LIST)
		int	nPage;
	int nRoomTotal;
	STRUCT_PARTYMATCH_ROOM pRoom[PARTYMATCH_ROOM_PAGE];
};

struct TS_SC_PARTYMATCH_MEMBER : TS_MESSAGE
{
	TS_SC_PARTYMATCH_MEMBER()	_INIT(TM_SC_PARTYMATCH_MEMBER)
		int				nRoom;
	AR_HANDLE		nMaster;		// master ID 아래에 pMeber중 하나가 이와 같은 값이다.
	STRUCT_PARTYMATCH_MEMBER pMember[PARTYMATCH_MEMBER_MAX];
};



// { Game Server <-> Auth Server
struct TS_GA_LOGIN : TS_MESSAGE
{
	TS_GA_LOGIN()		_INIT(TM_GA_LOGIN)

		unsigned short	server_idx;
	char			server_name[21];
	char			server_screenshot_url[256];
	bool			is_adult_server;
	char			server_ip[16];
	int				server_port;
};

struct TS_AG_LOGIN_RESULT : TS_MESSAGE
{
	TS_AG_LOGIN_RESULT()		_INIT(TM_AG_LOGIN_RESULT)

		unsigned short		result;
};

struct TS_GA_CLIENT_LOGIN : TS_MESSAGE
{
	TS_GA_CLIENT_LOGIN()		_INIT(TM_GA_CLIENT_LOGIN)

#ifdef	USE_CLIENT97
		char				account[GameRule::MAX_ACCOUNT_LEN - 2];
#else
		char				account[GameRule::MAX_ACCOUNT_LEN + 1];
#endif
	__int64		one_time_key;
};

struct TS_AG_CLIENT_LOGIN : TS_MESSAGE
{
	TS_AG_CLIENT_LOGIN()		_INIT(TM_AG_CLIENT_LOGIN)

#ifdef	USE_CLIENT97
		char				account[GameRule::MAX_ACCOUNT_LEN - 2];
#else
		char				account[GameRule::MAX_ACCOUNT_LEN + 1];
#endif
	int					nAccountID;
	unsigned short		result;
	char				nPCBangMode;
	int					nEventCode;
	int					nAge;
	AR_TIME				nContinuousPlayTime;
	AR_TIME				nContinuousLogoutTime;
};

struct TS_GA_CLIENT_LOGOUT : TS_MESSAGE
{
	TS_GA_CLIENT_LOGOUT()		_INIT(TM_GA_CLIENT_LOGOUT)

		char				account[GameRule::MAX_ACCOUNT_LEN + 1];
	AR_TIME				nContinuousPlayTime;
};

struct TS_AG_KICK_CLIENT : TS_MESSAGE
{
	TS_AG_KICK_CLIENT()			_INIT(TM_AG_KICK_CLIENT)

		enum _KICK_TYPE
	{
		KICK_TYPE_ANOTHER_LOGIN = 0,	// 다른 유저의 접속으로 인한 접속 종료
		KICK_TYPE_DUPLICATED_LOGIN = 1,	// 인증 서버에서 게임 서버 로그인 처리 시 중복된 로그인 감지로 인한 접속 종료(내부 오류 또는 해킹에 의한 발생)
		KICK_TYPE_BILLING_EXPIRED = 2,	// 결재 정보 만료로 인한 접속 종료
		KICK_TYPE_GAME_ADDICTION = 3,	// 게임 중독 방지로 인한 접속 종료
	};

	char		account[GameRule::MAX_ACCOUNT_LEN + 1];
	char		kick_type;		// _KICK_TYPE 값이 사용됨
};

struct TS_GA_CLIENT_KICK_FAILED : TS_MESSAGE
{
	TS_GA_CLIENT_KICK_FAILED()			_INIT(TM_GA_CLIENT_KICK_FAILED)

		char		account[GameRule::MAX_ACCOUNT_LEN + 1];
};

struct TS_AG_ITEM_PURCHASED : TS_MESSAGE
{
	TS_AG_ITEM_PURCHASED()		_INIT(TM_AG_ITEM_PURCHASED)

		char				account[GameRule::MAX_ACCOUNT_LEN + 1];
	int					nAccountID;
};

struct TS_AG_ITEM_SUPPLIED : TS_MESSAGE
{
	TS_AG_ITEM_SUPPLIED()		_INIT(TM_AG_ITEM_SUPPLIED)

		char				account[GameRule::MAX_ACCOUNT_LEN + 1];
};

//	2009. 8. 11 floyd 보안비밀번호 2차 수정
struct TS_AG_SECURITY_NO_CHECK : TS_MESSAGE
{
	TS_AG_SECURITY_NO_CHECK()	_INIT(TM_AG_SECURITY_NO_CHECK)

		char		account[GameRule::MAX_ACCOUNT_LEN + 1];
	int			mode;
	int			result;

};

//	2009. 8. 11 floyd 보안비밀번호 2차 수정
struct TS_GA_SECURITY_NO_CHECK : TS_MESSAGE
{
	TS_GA_SECURITY_NO_CHECK()	_INIT(TM_GA_SECURITY_NO_CHECK)

		char		account[GameRule::MAX_ACCOUNT_LEN + 1];
	char		password[19];
	int			mode;
};


struct TS_CS_OPEN_ITEM_SHOP : TS_MESSAGE
{
	TS_CS_OPEN_ITEM_SHOP()			_INIT(TM_CS_OPEN_ITEM_SHOP)
};

struct TS_SC_OPEN_ITEM_SHOP : TS_MESSAGE
{
	TS_SC_OPEN_ITEM_SHOP()			_INIT(TM_SC_OPEN_ITEM_SHOP)

		int			client_id;
	int			account_id;
	int			one_time_password;
	char		raw_server_name[32];
};

//	2009. 8. 11 floyd 보안비밀번호 2차 수정
/*
struct TS_SC_CREATE_SECURITY_NO : TS_MESSAGE
{
	TS_SC_CREATE_SECURITY_NO()			_INIT( TM_SC_CREATE_SECUTIRY_NO )
};
*/

struct TS_SC_REQUEST_SECURITY_NO : TS_MESSAGE
{
	TS_SC_REQUEST_SECURITY_NO()		_INIT(TM_SC_REQUEST_SECUTIRY_NO);

	enum { SC_NONE, SC_OPEN_STORAGE, SC_DELETE_CHARACTER };

	int			mode;
};

struct TS_CS_SECURITY_NO : TS_MESSAGE
{
	TS_CS_SECURITY_NO()		_INIT(TM_CS_SECURITY_NO);

	int			mode; // 0 - None, 1 - Storage open
	char		security_no[19];
};

//	2009. 8. 11 floyd 보안비밀번호 2차 수정
/*
struct TS_CS_CREATE_SECURITY_NO : TS_MESSAGE
{
	TS_CS_CREATE_SECURITY_NO() _INIT( TM_CS_CREATE_SECURITY_NO );

	char		password[19];
	char		security_no[19];
};

struct TS_CS_REQUEST_SECURITY_NO_CHANGE : TS_MESSAGE
{
	TS_CS_REQUEST_SECURITY_NO_CHANGE() _INIT( TM_CS_REQUEST_SECURITY_NO_CHANGE );
};

struct TS_SC_CHANGE_SECURITY_NO : TS_MESSAGE
{
	TS_SC_CHANGE_SECURITY_NO() _INIT( TM_SC_CHANGE_SECURITY_NO );
};

struct TS_CS_CHANGE_SECURITY_NO : TS_MESSAGE
{
	TS_CS_CHANGE_SECURITY_NO() _INIT( TM_CS_CHANGE_SECURITY_NO );

	char		security_no_old[19];
	char		security_no[19];
};

struct TS_CS_REQUEST_CLEAR_SECURITY_NO : TS_MESSAGE
{
	TS_CS_REQUEST_CLEAR_SECURITY_NO()	_INIT( TM_CS_REQUEST_CLEAR_SECURITY_NO );
};

struct TS_SC_CLEAR_SECURITY_NO : TS_MESSAGE
{
	TS_SC_CLEAR_SECURITY_NO()			_INIT( TM_SC_CLEAR_SECURITY_NO );

	bool		clearable;
};

struct TS_CS_CLEAR_SECURITY_NO : TS_MESSAGE
{
	TS_CS_CLEAR_SECURITY_NO()			_INIT( TM_CS_CLEAR_SECURITY_NO );

	char		password[19];
	char		security_no[19];
};
*/

// }

// { Upload Server <-> Game Server
struct TS_SU_LOGIN : TS_MESSAGE
{
	TS_SU_LOGIN()			_INIT(TM_SU_LOGIN)

		char					server_name[21];
};

struct TS_US_LOGIN_RESULT : TS_MESSAGE
{
	TS_US_LOGIN_RESULT()	_INIT(TM_US_LOGIN_RESULT)

		unsigned short			result;
};

struct TS_SU_REQUEST_UPLOAD : TS_MESSAGE
{
	TS_SU_REQUEST_UPLOAD()	_INIT(TM_SU_REQUEST_UPLOAD)

		enum
	{
		UPLOAD_TYPE_ICON = 0,
		UPLOAD_TYPE_BANNER = 1,
	};

	int						client_id;
	int						account_id;
	int						guild_sid;
	int						one_time_password;
	unsigned char			type;
};

struct TS_US_REQUEST_UPLOAD : TS_MESSAGE
{
	TS_US_REQUEST_UPLOAD()	_INIT(TM_US_REQUEST_UPLOAD)

		unsigned short			result;
};

struct TS_US_UPLOAD : TS_MESSAGE
{
	TS_US_UPLOAD()			_INIT(TM_US_UPLOAD)

		int						guild_id;
	int						file_size;
	unsigned char			filename_length;
	unsigned char			type;
};

// }

#undef _INIT

#pragma pack()
