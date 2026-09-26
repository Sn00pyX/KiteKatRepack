#pragma once

#include <mmo/ArMove.h>
#include <geometry/X2DPolygon.h>

#include "GameContent.h"
#include "GameRule.h"


// 배틀 아레나 경기장 정보 보관용
enum _BATTLE_ARENA_TYPE
{
	BAT_CLASSIC					= 0,
	BAT_BINGO					= 1,
	BAT_SLAUGHTER				= 2,
};

// 아레나 유저 등급 구분
enum _BATTLE_GRADE
{
	BG_INVALID		= -1,

	BG_ROOKIE		= 0,
	BG_GROW			= 1,
	BG_MAJOR		= 2,

	MAX_BATTLE_GRADE,
};

// 아레나 이탈 유형 코드(BattleArenaBase에서 사용하지 않고 BattleArenaManager나 패킷 방송 시 사용하지만 include 문제를 피하기 위해 여기에 둠)
enum _ARENA_LEAVE_TYPE
{
	ALT_UNKNOWN							= 0,
	ALT_USER_REQUEST					= 1,	// 대기열/경기 이탈 두 경우 모두 사용됨
	ALT_GAME_OVER						= 2,	// 경기 이탈 시에만 사용됨
	ALT_PARTICIPATING_SPECIAL_PARTY		= 3,	// 대기열 이탈 시에만 사용됨
	ALT_FAILED_TO_JOIN_BATTLE			= 4,	// 대기열 이탈 시에만 사용됨
	ALT_FAILED_TO_START_BATTLE			= 5,	// 대기열 이탈 시에만 사용됨(경기 시작 실패)
	ALT_DISCONNECT						= 6,	// 대기열/경기 이탈 두 경우 모두 사용됨(대기열에 있다가 접속 끊긴 경우, 경기 중에 오프라인 된 후에 제한 시간 경과로 이탈되는 경우)
	ALT_ABSENT							= 7,	// 경기 중에 잠수 신고를 통해 경기에서 이탈된 경우
	ALT_LEAVE_ARENA						= 8,	// 아레나에서 워프를 통해 다른 지역으로 이탈함으로 인해 경기에서 이탈된 경우(워프 자체가 근본적으로 차단되어야 함)
	ALT_ENTER_INSTANCE_DUNGEON			= 9,	// 인스턴스 던전에 입장하기 위해 대기열에서 이탈되는 경우
	ALT_ENTER_DEATHMATCH				= 10,	// 데스매치 존에 입장하기 위해 대기열에서 이탈되는 경우
	ALT_ENTER_HUNTAHOLIC				= 11,	// 헌터홀릭 로비(?)에 입장하기 위해 대기열에서 이탈되는 경우
	ALT_EXERCISE_PARTY_KICK				= 12,	// 연습 경기 파티 구성 중 추방에 의해 이탈되는 경우
};

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

// 아레나 경기 내부의 프랍의 상태(슬래터 프랍 또는 빙고 프랍 9개)
enum _PROP_STATE
{
	PS_NOT_EXIST		= 0,
	PS_NEUTRAL			= 1,
	PS_OWNED_BY_TEAM_0	= 2,
	PS_OWNED_BY_TEAM_1	= 3,
};

struct BATTLE_ARENA_MONSTER_RESPAWN
{
	int						nID;				// 몬스터 리스폰 정보 ID

	X2D::Box< AR_UNIT >		bxArea;				// 리스폰 지역 사각형

	int						nMonsterID;			// 몬스터 ID
	int						nCount;				// 리스폰 마리 수
	AR_TIME					nPeriod;			// 리스폰 간격(단위: 1/100 초)

	bool					bWandering;			// 주변 방황 여부(false: 제자리 고정, true: 방황)

	float					fFace;				// 리젠 후 몬스터가 바라볼 방향(스킬 이펙트 방향 지정을 위해 필요)
};

struct BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO : public GameContent::FIELD_PROP_RESPAWN_INFO
{
	BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO( int _nPropId, AR_UNIT _x, AR_UNIT _y, float _fZOffset,
		float _fRotateX, float _fRotateY, float _fRotateZ, float _fScaleX, float _fScaleY, float _fScaleZ, bool _bLockHeight, float _fLockHeight,
		int _nPropIndexInArena )
		: GameContent::FIELD_PROP_RESPAWN_INFO( _nPropId, _x, _y, 0, _fZOffset, _fRotateX, _fRotateY, _fRotateZ, _fScaleX, _fScaleY, _fScaleZ, _bLockHeight, _fLockHeight )
		, nPropIndexInArena( _nPropIndexInArena )
	{}

	int		nPropIndexInArena;
};

// 서버/클라 공용 BattleArenaBase
struct BattleArenaBase
{
	int						nID;
	_BATTLE_ARENA_TYPE		eType;
	char					nTeamCount;
	char					nMinMember;					// 경기를 시작하기 위한 최소 인원
	char					nMaxMember;					// 경기에 참여할 수 있는 최대 인원

	int						nObjectivePoint;			// 경기 승리를 위해 먼저 도달해야할 목표 점수
	AR_TIME					nMinPlayDurationForReward;	// 경기 종료 시 보상을 정상적으로 받기 위해 최소한으로 플레이해야 하는 시간

	unsigned int			nArenaRegionLeft;
	unsigned int			nArenaRegionTop;
	unsigned int			nArenaRegionRight;
	unsigned int			nArenaRegionBottom;

	AR_TIME					nCountdownDuration;			// 경기 생성 시점부터 경기 시작 및 입장 제한 카운트다운 시간
	AR_TIME					nBattleDuration;			// 경기 진행 시간
};

// 서버용 BattleArenaBase
struct BattleArenaBaseServer : public BattleArenaBase
{
	inline bool				IsExerciseGameArena() const		{ return nID == GameRule::BATTLE_ARENA_EXERCISE_GAME_ARENA_ID; }

	int						nQuickJoinLimitPoint;	// 빠른 입장으로 경기 중간에 입장을 혀용하는 최대 점수(특정 팀의 점수가 이 수치보다 높으면 빠른 입장으로 입장 불가)
	AR_TIME					nQuickJoinLimitTime;	// 빠른 입장으로 경기 중간에 입장을 혀용하는 최대 시간(시작 시점 + nQuickJoinLimitTime 이전까지만 빠른 입장 가능)

	int						nScorePerKill;			// 경기 중 1킬당 획득 점수
	int						nAPPerKill;				// 경기 중 1킬당 획득 AP(실시간 지급)
	int						nMaxAPByKill;			// 경기 중 킬을 통해 얻을 수 있는 AP 상한
	int						nAPPerPropActivate;		// 경기 중 프랍 활성화 1회당 획득 AP(실시간 지급)
	int						nMaxAPByPropActivate;	// 경기 중 프랍 활성화를 통해 얻을 수 있는 AP 상한

	int						nRewardAPWin[ 3 /* BattleArenaManager::MAX_BATTLE_GRADE */ ];	// 승리 시 얻는 등급별 보상 AP
	int						nRewardAPDraw[ 3 /* BattleArenaManager::MAX_BATTLE_GRADE */ ];	// 무승부 시 얻는 등급별 보상 AP
	int						nRewardAPLose[ 3 /* BattleArenaManager::MAX_BATTLE_GRADE */ ];	// 패배 시 얻는 등급별 보상 AP

	int						nMinAPForMVP;

	ArPosition				posStart[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ];

	std::string				strScriptOnCreate;
	std::string				strScriptOnEnter;
	std::string				strScriptOnStart;
	std::string				strScriptOnEnd;
	std::string				strScriptOnLeave;
	std::string				strScriptOnDestroy;
	std::string				strScriptOnKill;

	// 경기 시작 시점부터 리젠될 몬스터 리젠 정보
	std::vector< const BATTLE_ARENA_MONSTER_RESPAWN * >				vMonsterRespawnList;
	// 경기 시작 시점부터 리젠될 필드 프랍 리젠 정보(슬로터 프랍, 빙고 프랍 등)
	std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >		vFieldPropRespawnList;
	// 경기 생성 시점에 리젠되어 경기 시작 시점에 제거될 프랍(경기장 진입 제한용 프랍)
	std::vector< const BATTLE_ARENA_FIELD_PROP_RESPAWN_INFO * >		vArenaBlockerFieldPropRespawnList;

	// 경기 시작 영역(경기 생성 시점부터 시작 전까지는 해당 영역을 벗어날 수 없음)
	const X2D::Polygon< AR_UNIT > *									apBeginAreaList[ GameRule::BATTLE_ARENA_MAX_TEAM_COUNT ];
};
