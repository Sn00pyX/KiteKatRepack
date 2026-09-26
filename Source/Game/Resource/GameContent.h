#pragma once


#include <vector>
#include <string>

#include <geometry/X2DPolygon.h>
#include <mmo/ArMove.h>

#include "ContentStruct.h"
#include "CreatureBase.h"
#include "NPCBase.h"
#include "QuestBase.h"
#include "GameRule.h"

struct GameContent
{
	enum
	{
		LOCAL_INFO_KOREA			= 1 << 0,
		LOCAL_INFO_HONGKONG			= 1 << 1,
		LOCAL_INFO_AMERICA			= 1 << 2,
		LOCAL_INFO_GERMAN			= 1 << 3,
		LOCAL_INFO_JAPAN			= 1 << 4,
		LOCAL_INFO_TAIWAN			= 1 << 5,
		LOCAL_INFO_CHINA			= 1 << 6,
		LOCAL_INFO_FRANCE			= 1 << 7,
		LOCAL_INFO_RUSSIA			= 1 << 8,
		LOCAL_INFO_MALAYSIA			= 1 << 9,
		LOCAL_INFO_SINGAPORE		= 1 << 10,
		LOCAL_INFO_VIETNAM			= 1 << 11,
		LOCAL_INFO_THAILAND			= 1 << 12,
		LOCAL_INFO_MIDEAST			= 1 << 13,
		LOCAL_INFO_TURKEY			= 1 << 14,
		LOCAL_INFO_POLAND			= 1 << 15,
		LOCAL_INFO_ITALY			= 1 << 16,

		LOCAL_EXCLUDE_TEST_SERV		= 1 << 29,
		LOCAL_EXCLUDE_SERVICE_SERV	= 1 << 30
	};

	enum
	{
		WAY_POINT_CIRCULAR			= 1,
		WAY_POINT_RETURN			= 2,
	};

	enum
	{
		TYPE_DEATHMATCH_1_RANKED	= 120601,
		TYPE_DEATHMATCH_2_RANKED	= 120602,
		TYPE_DEATHMATCH_3_RANKED	= 120603,
		TYPE_DEATHMATCH_4_RANKED	= 120604,
		TYPE_DEATHMATCH_5_RANKED	= 120501,
		TYPE_DEATHMATCH_6_RANKED	= 120502,
		TYPE_DEATHMATCH_7_RANKED	= 120503,
		TYPE_DEATHMATCH_FREED		= 120504,
	};

#ifndef _USE_NEW_ROAMING_ONLY
	struct WAY_POINT_INFO
	{
		int way_point_id;
		int way_point_type;
		int way_point_speed;

		std::vector< ArPosition > vWayPoint;
	};
#endif

	struct AREA_BOX
	{
		AREA_BOX() : left( 0 ), top( 0 ), right( 0 ), bottom( 0 ) {}
		AREA_BOX( AR_UNIT _left, AR_UNIT _top, AR_UNIT _right, AR_UNIT _bottom )
			: left( _left ), top( _top ), right( _right ), bottom( _bottom ) {}

		AR_UNIT left;
		AR_UNIT top;
		AR_UNIT right;
		AR_UNIT bottom;
	};

	struct RANDOM_AREA_INFO
	{
		RANDOM_AREA_INFO() : random_area_id( 0 ), channel_id( 0 ), dungeon_id( 0 )
		{
		}

		RANDOM_AREA_INFO( int _random_area_id, int _channel_id, int _dungeon_id, GameContent::AREA_BOX randomBox ) : random_area_id( _random_area_id ), channel_id( _channel_id ), dungeon_id( _dungeon_id )
		{
			vRandomBox.push_back( randomBox );
		}

		int random_area_id;
		int channel_id;
		int dungeon_id;

		std::vector< GameContent::AREA_BOX > vRandomBox;
	};

	struct RANDOM_POOL_INFO
	{
		RANDOM_POOL_INFO( int _quest_target_id, int _level ) : quest_target_id( _quest_target_id ), level( _level ) {}

		int quest_target_id;
		int level;
	};

	struct MapLocationInfo : public X2D::Polygon< AR_UNIT >
	{
		int location_id;
		int priority;
	};

	struct MONSTER_RESPAWN_BASE_INFO
	{
		MONSTER_RESPAWN_BASE_INFO( unsigned _id = 0, AR_TIME _interval = 0, unsigned _inc = 0, bool _is_wandering = true, int _way_point_id = 0, unsigned _prespawn_count = 0 )
			:
			id( _id ),
			interval( _interval ),
			layer( 0 ),
			inc( _inc ),
			is_wandering( _is_wandering ),
			way_point_id( _way_point_id ),
			count( 0 ),
			prespawn_count( _prespawn_count ),
			dungeon_id( 0 )
			{}
		
		unsigned	id;								// ID
		AR_TIME		interval;						// 체크 주기
		unsigned char layer;						// 레이어
		unsigned	inc;							// 매 체크시 증가시킬 몹 갯수
		bool		is_wandering;
		int			dungeon_id;
		int			way_point_id;
		// 이하는 런타임시 필요한 변수
		volatile unsigned int	count;				// 현재 몹 갯수
		unsigned int			prespawn_count;		// 월드에 미리 등장시켜둘 몬스터 수
	};

	struct MONSTER_RESPAWN_INFO : MONSTER_RESPAWN_BASE_INFO
	{
		MONSTER_RESPAWN_INFO( unsigned _id = 0, AR_TIME _interval = 0, AR_UNIT _left = 0, AR_UNIT _top = 0, AR_UNIT _right = 0, AR_UNIT _bottom = 0, unsigned _monster_id = 0, unsigned _max_num = 0, unsigned _inc = 0, bool _is_wandering = true, int _way_point_id = 0, int _dungeon_id = 0, unsigned _prespawn_count = 0 )
			:
			MONSTER_RESPAWN_BASE_INFO( _id, _interval, _inc, _is_wandering, _way_point_id, ( !_dungeon_id && !_way_point_id && ( _max_num * GameRule::MONSTER_PRESPAWN_RATE >= 1.0f ) ) ? unsigned int( _max_num * GameRule::MONSTER_PRESPAWN_RATE ) : _max_num ),
			left( _left ),
			top( _top ),
			right( _right ),
			bottom( _bottom ),
			monster_id( _monster_id ),
			max_num( _max_num )
		{
		}

		AR_UNIT		left, top, right, bottom;		// 영역
		unsigned	monster_id;						// 몹 ID
		unsigned	max_num;						// 최대 몹 갯수
	};

	struct RANDOM_MONSTER_RESPAWN_INFO : MONSTER_RESPAWN_BASE_INFO
	{
		RANDOM_MONSTER_RESPAWN_INFO( unsigned _id = 0, AR_TIME _interval = 0, unsigned _random_area_id = 0, unsigned _inc = 0, bool _is_wandering = true, int _way_point_id = 0, unsigned _prespawn_count = 0, bool _except_raid_siege = false )
			:
			MONSTER_RESPAWN_BASE_INFO( _id, _interval, _inc, _is_wandering, _way_point_id, _prespawn_count ),
			random_area_id( _random_area_id ),
			except_raid_siege( _except_raid_siege )
		{
		}

		unsigned								random_area_id;		// 랜덤 리스폰 위치 아이디
		std::vector< std::pair< int, int > >	monster_list;		// 몬스터 리스트 < 아이디, 확률 >
		bool									except_raid_siege;	// 레이드와 시즈에서는 리젠되지 않음
	};

	struct RANDOM_MONSTER_GROUP_INFO
	{
		RANDOM_MONSTER_GROUP_INFO( unsigned int _group_id ) : group_id( _group_id ) { }

		unsigned int							group_id;		// 랜덤 몬스터 그룹 아이디
		std::vector< std::pair< int, int > >	monster_list;	// 몬스터 리스트 < 아이디, 확률 >
	};

	struct ROAMING_CREATURE_RESPAWN_INFO
	{
		ROAMING_CREATURE_RESPAWN_INFO( const int eCreatureType /* StructRoamer::ROAMING_CREATURE_TYPE */, const int nCreatureID, const AR_TIME nRespawnInterval, const int nAngle, const AR_UNIT nDistance )
			: m_eCreatureType( eCreatureType )
			, m_nCreatureID( nCreatureID )
			, m_nRespawnInterval( nRespawnInterval )
			, m_nAngle( nAngle )
			, m_nDistance( nDistance )
		{}

		int /* StructRoamer::ROAMING_CREATURE_TYPE */		m_eCreatureType;
		int													m_nCreatureID;
		AR_TIME												m_nRespawnInterval;
		int													m_nAngle;
		AR_UNIT												m_nDistance;
	};

	// 필드 프랍 리젠 정보 보관 용
	struct FIELD_PROP_RESPAWN_INFO
	{
		FIELD_PROP_RESPAWN_INFO( int _nPropId, AR_UNIT _x, AR_UNIT _y, unsigned char _layer, float _fZOffset,
			float _fRotateX, float _fRotateY, float _fRotateZ, float _fScaleX, float _fScaleY, float _fScaleZ, bool _bLockHeight, float _fLockHeight )
			: nPropId( _nPropId )
			, x( _x )
			, y( _y )
			, layer( _layer )
			, fZOffset( _fZOffset )
			, fRotateX( _fRotateX )
			, fRotateY( _fRotateY )
			, fRotateZ( _fRotateZ )
			, fScaleX( _fScaleX )
			, fScaleY( _fScaleY )
			, fScaleZ( _fScaleZ )
			, bLockHeight( _bLockHeight )
			, fLockHeight( _fLockHeight )
		{}

		FIELD_PROP_RESPAWN_INFO( const FIELD_PROP_RESPAWN_INFO & rhs )
		{
			s_memcpy( this, sizeof( *this ), &rhs, sizeof( *this ) );
		}

		FIELD_PROP_RESPAWN_INFO()
		{
			nPropId = 0;
			x = 0;
			y = 0;
			layer = 0;
			fZOffset = 0.0f;
			fRotateX = 0.0f;
			fRotateY = 0.0f;
			fRotateZ = 0.0f;
			fScaleX = 1.0f;
			fScaleY = 1.0f;
			fScaleZ = 1.0f;
			bLockHeight = false;
			fLockHeight = 0.0f;
		}

		int nPropId;
		AR_UNIT x, y;
		unsigned char layer;
		float fZOffset;
		float fRotateX;
		float fRotateY;
		float fRotateZ;
		float fScaleX;
		float fScaleY;
		float fScaleZ;
		bool bLockHeight;
		float fLockHeight;
	};

	struct REGEN_INFO
	{
		REGEN_INFO( const FIELD_PROP_RESPAWN_INFO * _pRespawnInfo, AR_TIME _tNextRegen )
			: pRespawnInfo( _pRespawnInfo )
			, tNextRegen( _tNextRegen )
		{}

		const FIELD_PROP_RESPAWN_INFO *	pRespawnInfo;
		AR_TIME							tNextRegen;
	};

	struct PENDED_RESPAWN_INFO
	{
		PENDED_RESPAWN_INFO( const AR_UNIT nX, const AR_UNIT nY, const unsigned char nLayer, const int nMonsterID, const int nRespawnCount, const bool bWandering, const AR_TIME nLifeTime, const AR_HANDLE hInitialEnemy )
			: x( nX ), y( nY ), layer( nLayer ), monsterId( nMonsterID ), respawnCount( nRespawnCount ), wandering( bWandering ), lifeTime( nLifeTime ), initialEnemy( hInitialEnemy )
		{}

		AR_UNIT				x;
		AR_UNIT				y;
		unsigned char		layer;
		int					monsterId;
		int					respawnCount;
		bool				wandering;
		AR_TIME				lifeTime;
		AR_HANDLE			initialEnemy;
	};

	// 헌터홀릭 몬스터 리스폰 정보 보관 용
	struct HUNTAHOLIC_MONSTER_RESPAWN_INFO
	{
		int						nID;				// 몬스터 리스폰 정보 ID

		X2D::Box< AR_UNIT >		bxArea;				// 리스폰 지역 사각형

		int						nMonsterID;			// 몬스터 ID
		int						nCount;				// 리스폰 마리 수
		AR_TIME					nPeriod;			// 리스폰 간격(단위: 1/100 초)

		bool					bWandering;			// 주변 방황 여부(false: 제자리 고정, true: 방황)
	};

	// 헌터홀릭 던전 레벨별 인스턴스화 정보 보관 용
	struct HUNTAHOLIC_INSTANCE_BASE
	{
		int			nID;						// 인스턴스화 타입(유형) ID - 로비 레이어 번호로도 사용 됨

		short		nMinLevel;					// 해당 인스턴스 입장 가능 최소 레벨(이상)
		short		nMaxLevel;					// 해당 인스턴스 입장 가능 최대 레벨(미만)

		const bool	IsProperLevel( const int nLevel ) const 	{ return ( nLevel >= nMinLevel && nLevel < nMaxLevel ); }

		c_fixed10	fPointAdvantage;			// 보상 포인트 받을 시 곱해질 상수(고렙 인스턴스에 어드벤티지를 주는 용도)

		__int64		nRewardExp;					// 던전 완료 시 획득할 보상 경험치
		int			nRewardJp;					// 던전 완료 시 획득할 보상 JP

		int			nRewardSuccessItemCode;		// 던전 성공 시 획득할 보상 아이템
		int			nRewardSuccessItemCount;	// 던전 성공 시 획득할 보상 아이템 수량
		int			nRewardFailItemCode;		// 던전 실패 시 획득할 보상 아이템
		int			nRewardFailItemCount;		// 던전 실패 시 획득할 보상 아이템 수량

		std::vector< const HUNTAHOLIC_MONSTER_RESPAWN_INFO * >	vRespawnInfo;		// 해당 인스턴스화 던전 내에 몬스터 리젠 정보
		std::vector< const FIELD_PROP_RESPAWN_INFO * >			vHealingPropInfo;	// 해당 인스턴스화 던전 내에 힐링 프랍 정보
	};

	// 헌터홀릭 정보 보관 용
	struct HUNTAHOLIC_BASE
	{
		int				nID;					// 헌터홀릭 ID
		int				nNameID;				// 헌터홀릭 던전 이름 String ID

		time_t			tHuntingPeriod;			// 매 회 헌터홀릭 던전 지속 시간(단위: 초)

		int				nObjectivePoint;		// 던전에서 획득 목표 점수(해당 포인트 달성 시 퇴장 가능)
		int				nMaxPoint;				// 던전에서 획득 가능 최대 점수(해당 포인트 달성 시 UI 띄우고 확인 누르면 퇴장, 확인 안 누르고 있으면 던전 종료 시 퇴장)

		unsigned int	nLobbyRegionLeft;		// 대기실 지역 번호 X:좌
		unsigned int	nLobbyRegionTop;		// 대기실 지역 번호 Y:상
		unsigned int	nLobbyRegionRight;		// 대기실 지역 번호 X:우
		unsigned int	nLobbyRegionBottom;		// 대기실 지역 번호 Y:하
		ArPosition		posLobby;				// 대기실 시작 좌표

		unsigned int	nDungeonRegionLeft;		// 던전 지역 번호 X:좌
		unsigned int	nDungeonRegionTop;		// 던전 지역 번호 Y:상
		unsigned int	nDungeonRegionRight;	// 던전 지역 번호 X:우
		unsigned int	nDungeonRegionBottom;	// 던전 지역 번호 Y:하
		ArPosition		posDungeon;				// 던전 시작 좌표

		std::vector< const HUNTAHOLIC_INSTANCE_BASE * >	vInstanceBase;		// 적용 가능 인스턴스화 던전 정보

		const unsigned char		GetProperLobbyLayer( const int nLevel ) const
		{
			for( std::vector< const HUNTAHOLIC_INSTANCE_BASE * >::const_iterator it = vInstanceBase.begin() ; it != vInstanceBase.end() ; ++it )
			{
				if( (*it)->IsProperLevel( nLevel ) )
					return static_cast< unsigned char >( (*it)->nID );
			}

			assert( 0 && "HUNTAHOLIC PROPER LAYER" );
			return GameRule::HUNTAHOLIC_UNUSABLE_LOBBY_LAYER;
		}
	};

	// 인스턴스 던전 몬스터 리스폰 정보 보관 용
	struct INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO
	{
		int						nID;					// 몬스터 리스폰 정보 ID

		X2D::Box< AR_UNIT >		bxArea;					// 리스폰 지역 사각형

		int						nMonsterID;				// 몬스터 ID
		int						nCount;					// 리스폰 마리 수
		AR_TIME					nPeriod;				// 리스폰 간격(단위: 1/100 초)

		bool					bWandering;				// 주변 방황 여부(false: 제자리 고정, true: 방황)
		int						nRespawnGroup;			// 몬스터 리스폰 그룹

		bool					bRespawnControl;		// 시작 시 리젠 제어 여부(false: 던전 시작 시 리스폰, true: 외부 제어에 의해 리스폰)

		char					szScriptOnDead[ 256 ];	// 몬스터 사망 시 실행될 스크립트
	};

	// 던전 인스턴스 종류 정보 보관용
	struct INSTANCE_DUNGEON_TYPE_BASE
	{
		int					nID;						// 난이도 ID

		short				nMinLevel;					// 최소 레벨(이상)
		short				nMaxLevel;					// 최대 레벨(미만)
		int					nNeedItemCode;				// 필요 아이템 코드
		__int64				nNeedItemCount;				// 필요 아이템 수량

		char				szScriptOnCreate[ 256 ];	// 인스턴스 생성 시 실행될 스크립트
		char				szScriptOnJoin[ 256 ];		// 인스턴스에 참여 시 실행될 스크립트
		char				szScriptOnLeave[ 256 ];		// 인스턴스에서 이탈 시 실행될 스크립트

		const bool	IsProperLevel( const int nLevel ) const 	{ return ( nLevel >= nMinLevel && nLevel < nMaxLevel ); }

		std::vector< const INSTANCE_DUNGEON_MONSTER_RESPAWN_INFO * >	vRespawnInfo;		// 해당 인스턴스화 던전 내에 몬스터 리젠 정보
		std::vector< const FIELD_PROP_RESPAWN_INFO * >					vHealingPropInfo;	// 해당 인스턴스화 던전 내에 힐링 프랍 정보
	};

	// 인스턴스 던전 정보 보관용
	struct INSTANCE_DUNGEON_BASE
	{
		int				nID;					// 인스턴스 던전 ID
		int				nNameID;				// 인스턴스 던전 이름 String ID

		unsigned int	nDungeonRegionLeft;		// 던전 지역 번호 X:좌
		unsigned int	nDungeonRegionTop;		// 던전 지역 번호 Y:상
		unsigned int	nDungeonRegionRight;	// 던전 지역 번호 X:우
		unsigned int	nDungeonRegionBottom;	// 던전 지역 번호 Y:하
		ArPosition		posDungeon;				// 던전 시작 좌표

		std::vector< const INSTANCE_DUNGEON_TYPE_BASE * >	vInstanceDungeonTypeBase;	// 인스턴스 던전 종류 목록
	};

	struct EVENT_AREA_INFO
	{
		EVENT_AREA_INFO()
			: m_nBeginTime( 0 )
			, m_nEndTime( 0 )
			, m_nMinLevel( 0 )
			, m_nMaxLevel( 0 )
			, m_nRaceJobLimit( 0 )
			, m_nLimitActivateCount( 0 )
			, m_strEnterHandler( "" )
			, m_strLeaveHandler( "" )
		{
			for( int nIdx = 0 ; nIdx < MAX_ACTIVATE_CONDITION ; ++nIdx )
			{
				m_nActivateCondition[ nIdx ] = 0;
				m_nActivateValue[ nIdx ][ 0 ] = 0;
				m_nActivateValue[ nIdx ][ 1 ] = 0;
			}
		}

		EVENT_AREA_INFO( const EVENT_AREA_INFO & rhs )
			: m_vArea( rhs.m_vArea )
			, m_nBeginTime( rhs.m_nBeginTime )
			, m_nEndTime( rhs.m_nEndTime )
			, m_nMinLevel( rhs.m_nMinLevel )
			, m_nMaxLevel( rhs.m_nMaxLevel )
			, m_nRaceJobLimit( rhs.m_nRaceJobLimit )
			, m_nLimitActivateCount( rhs.m_nLimitActivateCount )
			, m_strEnterHandler( rhs.m_strEnterHandler )
			, m_strLeaveHandler( rhs.m_strLeaveHandler )
		{
			for( int nIdx = 0 ; nIdx < MAX_ACTIVATE_CONDITION ; ++nIdx )
			{
				m_nActivateCondition[ nIdx ] = rhs.m_nActivateCondition[ nIdx ];
				m_nActivateValue[ nIdx ][ 0 ] = rhs.m_nActivateValue[ nIdx ][ 0 ];
				m_nActivateValue[ nIdx ][ 1 ] = rhs.m_nActivateValue[ nIdx ][ 1 ]; 
			}
		}

		const bool IsActivatable( const struct StructPlayer * pPlayer, const int nAreaIndex ) const;

		std::vector< X2D::Polygon< int > * >	m_vArea;

		// 발동 제한 조건 관련
		int			m_nBeginTime;			// 발동 제한 시작 시각(1일 중 시/분/초 데이터를 초 단위로 환산한 값)
		int			m_nEndTime;				// 발동 제한 종료 시각(1일 중 시/분/초 데이터를 초 단위로 환산한 값)
		int			m_nMinLevel;			// 발동 제한 최소 레벨
		int			m_nMaxLevel;			// 발동 제한 최대 레벨

		// 종족/직업 제한 관련 상수(enum은 32비트형이라 별 수 없이 static const __int64 ㄱㄱ ;ㅁ;
		static const __int64	RACE_JOB_FLAG_RACE_GAIA				= (__int64)1 << 0;	// 가이아
		static const __int64	RACE_JOB_FLAG_RACE_DEVA				= (__int64)1 << 1;	// 데바
		static const __int64	RACE_JOB_FLAG_RACE_ASURA			= (__int64)1 << 2;	// 아수라

		static const __int64	RACE_JOB_FLAG_ALL_RACES				= ( RACE_JOB_FLAG_RACE_GAIA | RACE_JOB_FLAG_RACE_DEVA | RACE_JOB_FLAG_RACE_ASURA );

		static const __int64	RACE_JOB_FLAG_JOB_ROGUE				= (__int64)1 << 3;	// 방랑자
		static const __int64	RACE_JOB_FLAG_JOB_FIGHTER			= (__int64)1 << 4;	// 투사
		static const __int64	RACE_JOB_FLAG_JOB_KAHUNA			= (__int64)1 << 5;	// 주술사
		static const __int64	RACE_JOB_FLAG_JOB_SPELL_SINGER		= (__int64)1 << 6;	// 야수사
		static const __int64	RACE_JOB_FLAG_JOB_CHAMPION			= (__int64)1 << 7;	// 전사
		static const __int64	RACE_JOB_FLAG_JOB_ARCHER			= (__int64)1 << 8;	// 궁사
		static const __int64	RACE_JOB_FLAG_JOB_DRUID				= (__int64)1 << 9;	// 마도사
		static const __int64	RACE_JOB_FLAG_JOB_BATTLE_KAHUNA		= (__int64)1 << 10;	// 마투사
		static const __int64	RACE_JOB_FLAG_JOB_EVOKER			= (__int64)1 << 11;	// 마수사
		static const __int64	RACE_JOB_FLAG_JOB_BERSERKER			= (__int64)1 << 12;	// 광전사
		static const __int64	RACE_JOB_FLAG_JOB_MASTER_ARCHER		= (__int64)1 << 13;	// 궁투사
		static const __int64	RACE_JOB_FLAG_JOB_HIGH_DRUID		= (__int64)1 << 14;	// 진마도사
		static const __int64	RACE_JOB_FLAG_JOB_GREAT_KAHUNA		= (__int64)1 << 15;	// 진마투사
		static const __int64	RACE_JOB_FLAG_JOB_BEAST_MASTER		= (__int64)1 << 16;	// 질풍마수사
		static const __int64	RACE_JOB_FLAG_JOB_GUIDE				= (__int64)1 << 17;	// 가이드
		static const __int64	RACE_JOB_FLAG_JOB_HOLY_WARRIOR		= (__int64)1 << 18;	// 파이터
		static const __int64	RACE_JOB_FLAG_JOB_CLERIC			= (__int64)1 << 19;	// 클레릭
		static const __int64	RACE_JOB_FLAG_JOB_TAMER				= (__int64)1 << 20;	// 테이머
		static const __int64	RACE_JOB_FLAG_JOB_KNIGHT			= (__int64)1 << 21;	// 나이트
		static const __int64	RACE_JOB_FLAG_JOB_SOLDIER			= (__int64)1 << 22;	// 워리어
		static const __int64	RACE_JOB_FLAG_JOB_MAGE				= (__int64)1 << 23;	// 메이지
		static const __int64	RACE_JOB_FLAG_JOB_PRIEST			= (__int64)1 << 24;	// 프리스트
		static const __int64	RACE_JOB_FLAG_JOB_BREEDER			= (__int64)1 << 25;	// 브리더
		static const __int64	RACE_JOB_FLAG_JOB_CRUSADER			= (__int64)1 << 26;	// 크루세이더
		static const __int64	RACE_JOB_FLAG_JOB_BLADER			= (__int64)1 << 27;	// 블레이더
		static const __int64	RACE_JOB_FLAG_JOB_ARCH_MAGE			= (__int64)1 << 28;	// 아크 메이지
		static const __int64	RACE_JOB_FLAG_JOB_HIGH_PRIEST		= (__int64)1 << 29;	// 하이 프리스트
		static const __int64	RACE_JOB_FLAG_JOB_SOUL_BREEDER		= (__int64)1 << 30;	// 소울 브리더
		static const __int64	RACE_JOB_FLAG_JOB_STEPPER			= (__int64)1 << 31;	// 스테퍼
		static const __int64	RACE_JOB_FLAG_JOB_STRIDER			= (__int64)1 << 32;	// 스트라이더
		static const __int64	RACE_JOB_FLAG_JOB_MAGICIAN			= (__int64)1 << 33;	// 매지션
		static const __int64	RACE_JOB_FLAG_JOB_SUMMONER			= (__int64)1 << 34;	// 서모너
		static const __int64	RACE_JOB_FLAG_JOB_ASSASSIN			= (__int64)1 << 35;	// 어쌔신
		static const __int64	RACE_JOB_FLAG_JOB_RANGER			= (__int64)1 << 36;	// 레인저
		static const __int64	RACE_JOB_FLAG_JOB_SORCERER			= (__int64)1 << 37;	// 소서러
		static const __int64	RACE_JOB_FLAG_JOB_DARK_MAGICIAN		= (__int64)1 << 38;	// 다크 매지션
		static const __int64	RACE_JOB_FLAG_JOB_BATTLE_SUMMONER	= (__int64)1 << 39;	// 배틀 서모너
		static const __int64	RACE_JOB_FLAG_JOB_SHADOW_CHASER		= (__int64)1 << 40;	// 섀도우 체이서
		static const __int64	RACE_JOB_FLAG_JOB_SHADOW_HUNTER		= (__int64)1 << 41;	// 섀도우 헌터
		static const __int64	RACE_JOB_FLAG_JOB_LICH				= (__int64)1 << 42;	// 리치
		static const __int64	RACE_JOB_FLAG_JOB_WARLOCK			= (__int64)1 << 43;	// 워락
		static const __int64	RACE_JOB_FLAG_JOB_NECROMANCER		= (__int64)1 << 44;	// 네크로맨서

		__int64		m_nRaceJobLimit;		// 발동 제한 종족&직업(비트셋으로 사용, RACE_JOB_FLAG_RACE_* 과 RACE_JOB_FLAG_JOB_* 의 값들)

		enum _LIMIT_CONDITION
		{
			LIMIT_CONDITION_NONE			= 0,
			LIMIT_CONDITION_ITEM_COUNT_GE	= 1,	// 지정 아이템을 지정 수량 이상 보유 중인지 체크
			LIMIT_CONDITION_QUEST_STATUS	= 2,	// 퀘스트 상태값 체크
			LIMIT_CONDITION_SKILL_LEVEL_GE	= 3,	// 지정 스킬의 지정 레벨 이상 습득 여부 체크
			LIMIT_CONDITION_ITEM_WEARING	= 4,	// 지정 아이템의 장착 여부 체크
			LIMIT_CONDITION_SUMMON_OWNING	= 5,	// 테이밍 된 소환수 카드 인벤토리 내 보유 여부 체크
			LIMIT_CONDITION_STATE			= 6,	// 지정된 지속효과 보유 여부 체크
		};

		enum
		{
			MAX_ACTIVATE_CONDITION		= 6
		};

		int			m_nActivateCondition[ MAX_ACTIVATE_CONDITION ];		// _LIMIT_CONDITION 값이 들어감
		int			m_nActivateValue[ MAX_ACTIVATE_CONDITION ][ 2 ];	// m_nActivateCondition 값에 따른 발동값/부가값이 들어감

		int			m_nLimitActivateCount;								// 해당 이벤트 영역 발동 횟수 제한(플레이어당 개별 제한)

		std::string	m_strEnterHandler;
		std::string	m_strLeaveHandler;
	};


	static void					SetWayPointSpeed( int way_point_id, int way_point_speed );
	static void					SetWayPointType( int way_point_id, int way_point_type );
	static void					AddWayPoint( int way_point_id, AR_UNIT x, AR_UNIT y );
#ifndef _USE_NEW_ROAMING_ONLY
	static const GameContent::WAY_POINT_INFO * GetWayPoint( int way_point_id );
#endif
	static const bool			SelectItemIDFromDropGroup( const int nDropGroupID, ItemBase::ItemCode & nItemID, __int64 & nItemCount );
	static void					EnumNPCByNPCId( std::vector< struct StructNPC* > & vNPCList, int nNPCId );
	static const JobInfo *		GetJobInfo( int job_id );
	static const CreatureStat &	GetStatInfo( int stat_id );

	static MonsterBase *												GetMonsterInfo( int monster_id );
	static const std::vector< MonsterBase::MONSTER_TRIGGER > *			GetMonsterTriggerInfo( int nSkillTriggerID );
	static const std::vector< MonsterBase::MONSTER_SKILL_INFO > *		GetMonsterSkillInfo( int nSkillTriggerID );
	static const std::vector< MonsterBase::MONSTER_ITEM_DROP_INFO > *	GetMonsterDropTable( int nItemDropInfoID );

	static DropGroup*			GetDropGroupInfo( int drop_group_id );

	static SummonBase *			GetSummonInfo( int summon_id );
	static bool					IsUniqueSummonName( const char * name );
	static std::string			GetUniqueName( int summon_type );
	static std::string			GetSummonName();

	static PetBase *			GetPetInfo( int pet_id );

	static const StateInfo*				GetStateInfo( int state_id );
	static SkillBase *					GetSkillBase( int skill_id );
	static _MARKET_INFO *				GetMarketInfo( const char *szMarketName );
	static const ArPosition				GetDeathmatchPosition( const int level, const int deathmatch_type );
	static const int					GetDeathmatchType( const ArPosition pos );
	static const CreatureEnhanceInfo *	GetCreatureEnhanceInfo( const short enhance_level );

	static const char					GetCreatureFarmTicketCount( const char rate, const char form, const char enhance_level );
	static __int64						GetNeedExp( int level );
	static __int64						GetNeedSummonExp( int level );
	static const int					GetAllowedMaxSkillLevel( std::vector< SkillTree >* skillTree, const int skill_id );
	static __int64						GetNeedJpForJobLevelUp( int level, int job_depth );
	static __int64						GetNeedJpForSkillLevelUp( int skill_id, int skill_level, int skill_tree_id );
	static int							GetNeedTpForSkillLevelUp( int skill_id, int skill_level );
	static bool							IsValidItemCode( ItemBase::ItemCode code );
	static bool							IsBannedWord( int code_page, const char *szString );
	static int							isLearnableSkill( struct StructCreature *pCreature, int skill_id, int skill_level, int skill_tree_id, int creature_job_level ); // ErrorCode 리턴함
	static bool							LearnAllSkill( struct StructCreature *pCreature );
	static CreatureStat					GetJobLevelBonus( int depth, int * jobs, int * levels );
	static CreatureStat					GetSummonLevelBonus( int summon_code, int growth_depth, int level );

	static const char*			GetString( int string_id );

	static bool					GetRandomPoolInfo( int group_id, std::vector< GameContent::RANDOM_POOL_INFO > & rRandomPoolInfo, int level );
	static bool					IsInRandomPoolMonster( int group_id, int monster_id );

	static const std::vector< SetItemEffectInfo * > *		GetSetItemEffectInfoVector( const int nSetID );
	static const std::vector< EffectInfo * > *				GetEffectInfoVector( const int nEffectID );
	static const std::vector< EnhanceEffectInfo * > *		GetEnhanceEffectVector( const int nEnhanceEffectID );
	
	static void Nomalize( AR_UNIT * px, AR_UNIT * py );
	static bool	IsBlocked( AR_UNIT x, AR_UNIT y );		// 갈 수 없는 지역인지?
	static bool	FindPath( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2, std::vector< ArPosition >& vMoveInfo );
	static void GetCollisionPolygons( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2, std::vector<X2D::Polygon<int> *> & vPolygons );
	static bool GetIntersectPoint( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2, AR_UNIT& xr, AR_UNIT& yr );	// 직선을 그었을 때 최종 충돌 좌표 얻기(충돌이 없다면 마지막 좌표)
	static bool CollisionToLine( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2 );
	static bool GetValidRandomPos( const ArPosition& center, ArPosition& valid_pos, int v );

	static const bool			IsActivatableEventArea( const StructPlayer * pPlayer, const int nEventAreaID, const int nAreaIndex );
	static const int			GetEnterCountLimitOfEventArea( const int nEventAreaID );
	static const char *			GetEventAreaEnterHandler( const int nEventAreaID );
	static const char *			GetEventAreaLeaveHandler( const int nEventAreaID );

	// 랜덤 리젠 
	static const GameContent::RANDOM_AREA_INFO		GetRandomAreaInfo( int random_area_id );
	static const GameContent::AREA_BOX				GetRandomRespawnBox( int random_area_id );
	static const int								GetRandomRespawnChannelID( int random_area_id );
	static const int								GetRandomRespawnDungeonID( int random_area_id );
	static const int								GetRandomMonsterID( const int group_id );

	static int GetLocationId( AR_UNIT x, AR_UNIT y );

	static void LoadScript();
	static void ApplyQuestLink();	// NPC 배치
	static void ClearQuestLink();	// NPC 배치

	static const int GetQuestTextId( const QuestBase::QuestCode nQuestCode, const int nProgress );

	static StructNPC * GetNewNPC( const NPCBase & info, const unsigned char nLayer );
	static void AddNPCToWorld();
	static void AddRespawnObjectToWorld();

	static void ClearBannedWord();
	static void RegisterBannedWord( const char *szString );

	static void RegisterStateInfo( const StateInfo & info );
	static void RegisterSkillTree( const SkillTree & info );
	static void RegisterJobInfo( JobInfo & info );
	static void RegisterStatInfo( const CreatureStat & );
	static void RegisterExpTable( int level, __int64 exp, __int64 jp_0, __int64 jp_1, __int64 jp_2, __int64 jp_3 );
	static void RegisterSummonExpTable( int level, __int64 normal_exp, __int64 growth_exp, __int64 evolve_exp );
	static void RegisterDropGroupInfo( DropGroup & info );
	static void RegisterMonsterInfo( MonsterBase & info );
	static void RegisterMonsterTriggerInfo( const int nID, const std::vector< MonsterBase::MONSTER_TRIGGER > & vMonsterTriggerInfo );
	static void RegisterMonsterSkillInfo( const int nID, const std::vector< MonsterBase::MONSTER_SKILL_INFO > & vMonsterSkillInfo );
	static void RegisterMonsterDropTable( const int nID, const std::vector< MonsterBase::MONSTER_ITEM_DROP_INFO > & vMonsterItemDropInfo );
	static bool RegisterQuestLink( QuestLink & info );
	static void RegisterQuestTextId( const QuestBase::QuestCode nQuestCode, const int nStartTextId, const int nInProgressTextId, const int nEndTextId );
	static bool RegisterSummonInfo( SummonBase & info );
	static bool RegisterPetInfo( PetBase & info );
	static bool RegisterSkillBase( SkillBase & info );
	static bool RegisterSkillJobPointTable( int skill_id, int *need_jp );
	static bool RegisterStringInfo( int string_id, const char *szString );
	static bool RegisterPropContactScriptInfo( int prop_id, int prop_type, int model_info, float x, float y, std::vector< _PROP_CONTACT_SCRIPT_INFO::_FUNCTION_LIST > vFunctionList );
	static void RegisterJobLevelBonusInfo( const JobLevelBonus & level_bonus );
	static void RegisterSummonLevelBonusInfo( const SummonLevelBonus & level_bonus );
	static void RegisterBlockInfo( const X2D::Polygon< int > & block_info );
	static void RegisterAutoCheckBlockInfo( const X2D::Polygon< int > & block_info );
	static void RegisterEventAreaInfo( const int nEventAreaID, const EVENT_AREA_INFO & event_area );
	static void RegisterEventAreaBlock( const int nEventAreaID, const X2D::Polygon< int > & block );
	static void RegisterMapLocationInfo( const MapLocationInfo & location_info );

	static void RegisterSummonDefaultName( int id, bool bIsPrefix );
	static void RegisterSummonUniqueName( int summon_type, int id );
	static void RegisterRandomPoolInfo( int group_id, int quest_target_id, int target_level );

	static void RegisterNPCInfo( struct NPCBase & npc_info );
	static struct NPCBase & GetNPCInfo( const int id );

	static void RegisterMonsterRespawnInfo( const GameContent::MONSTER_RESPAWN_INFO & info );
	static void RegisterRaidMonsterRespawnInfo( const GameContent::MONSTER_RESPAWN_INFO & info );
	
	static void	RegisterRandomMonsterRespawnInfo( const GameContent::RANDOM_MONSTER_RESPAWN_INFO & info );
	static void	AddRandomAreaInfo( int random_area_id, AR_UNIT left, AR_UNIT top, AR_UNIT right, AR_UNIT bottom );
	static void	AddRandomMonster( int id, int monster_id, int ratio );
	static void AddRandomMonsterGroup( int monster_group_id, int monster_id, int ratio );

	static void	RegisterSetItemEffectInfo( const int nSetID, const SetItemEffectInfo & info );
	static void RegisterEffectInfo( const int nEffectGroupID, const EffectInfo & info );
	static void RigsterEnhanceEffectInfo( const int nEnhanceEffectID, const int nSubID, const EnhanceEffectInfo & info );
	static void RegisterDeathmatchInfo( const int id, const int deathmatch_type, const int min_level, const int max_level, const ArPosition position[] );
	static void RegisterCreatureEnhanceInfo( const short enhance_level, const float stat_amplify, const short card_durability, const short slot_amount, const short jp_addition );
	static void RegisterCreatureFarmInfo( const char rate, const char form, const char enhance_level, const char ticket_count );

	static void RegisterMonsterCreatureInfo( MonsterCreatureInfo & monster_creater );
	static const int  GetMonsterCreatureTameCode( int _nMonsterCreatureCode );
	static bool IsSoulCreature( int nSummonCode ); // 소울 크리처인지 판별하는 함수. 사용 전 정의부에 있는 주석 참조 바람

	static void RegisterSummonRandomSkillInfo( SummonRandomSkillInfo & summon_random_skill );
	static const int GetSummonRandomSkillID( int _nRandomSkillGroupID );
	static const int GetRandomSkillIDFromGroupID( int nGroupID, int nIndex );
	static const int GetSkillTreeGroupID( StructCreature *_pCreature, int _skill_id );
	static std::vector< SkillTree >* GetSkillTree( int skill_tree_id );
};


namespace ResourceManager
{	
	void		Init();

	std::string GetFullPathname( const char *szFileName );
	void		RegisterFile( const char *szFullPathName, const char *szFileName );
};

